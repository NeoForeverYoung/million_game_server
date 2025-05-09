local skynet = require "skynet"
local socket = require "skynet.socket"
local s = require "service"
local runconfig = require "runconfig"

conns = {} --[socket_id] = conn
players = {} --[playerid] = gateplayer

--杩炴帴绫�
function conn()
    local m = {
        fd = nil,
        playerid = nil,
    }
    return m
end

--鐜╁�剁被
function gateplayer()
    local m = {
        playerid = nil,
        agent = nil,
		conn = nil,
    }
    return m
end

local str_pack = function(cmd, msg)
    return table.concat( msg, ",").."\r\n"
end

local str_unpack = function(msgstr)
    local msg = {}

    while true do
        local arg, rest = string.match( msgstr, "(.-),(.*)")
        if arg then
            msgstr = rest
            table.insert(msg, arg)
        else
            table.insert(msg, msgstr)
            break
        end
    end

    return msg[1], msg
end

s.resp.send_by_fd = function(source, fd, msg)
    if not conns[fd] then
        return
    end

    local buff = str_pack(msg[1], msg)
    skynet.error("send "..fd.." ["..msg[1].."] {"..table.concat( msg, ",").."}")
	socket.write(fd, buff)
end

s.resp.send = function(source, playerid, msg)
	local gplayer = players[playerid]
    if gplayer == nil then
		return
    end
    local c = gplayer.conn
    if c == nil then
		return
    end
    
    s.resp.send_by_fd(nil, c.fd, msg)
end

s.resp.sure_agent = function(source, fd, playerid, agent)

	local conn = conns[fd]
	if not conn then --鐧婚檰杩囩▼涓�宸茬粡涓嬬嚎
		skynet.call("agentmgr", "lua", "reqkick", playerid, "鏈�瀹屾垚鐧婚檰鍗充笅绾�")
		return false
	end
	
	conn.playerid = playerid
	
    local gplayer = gateplayer()
    gplayer.playerid = playerid
    gplayer.agent = agent
	gplayer.conn = conn
    players[playerid] = gplayer
    
	return true
end

local disconnect = function(fd)
    local c = conns[fd]
    if not c then
        return
    end

    local playerid = c.playerid
    --杩樻病瀹屾垚鐧诲綍
    if not playerid then
        return
    --宸插湪娓告垙涓�
    else
        players[playerid] = nil
        local reason = "鏂�绾�"
        skynet.call("agentmgr", "lua", "reqkick", playerid, reason)
    end
end

s.resp.kick = function(source, playerid)
    local gplayer = players[playerid]
    if not gplayer then
        return
    end

    local c = gplayer.conn
	players[playerid] = nil
	
    if not c then
        return
    end
    conns[c.fd] = nil

    disconnect(c.fd)
    socket.close(c.fd)
end






local process_msg = function(fd, msgstr)
    local cmd, msg = str_unpack(msgstr)
    skynet.error("recv "..fd.." ["..cmd.."] {"..table.concat( msg, ",").."}")

    local conn = conns[fd]
    local playerid = conn.playerid
    --灏氭湭瀹屾垚鐧诲綍娴佺▼
    if not playerid then
        local node = skynet.getenv("node")
        local nodecfg = runconfig[node]
        local loginid = math.random(1, #nodecfg.login)
        local login = "login"..loginid
		skynet.send(login, "lua", "client", fd, cmd, msg)
    --瀹屾垚鐧诲綍娴佺▼
    else
        local gplayer = players[playerid]
        local agent = gplayer.agent
		skynet.send(agent, "lua", "client", cmd, msg)
    end
end


local process_buff = function(fd, readbuff)
    while true do
        local msgstr, rest = string.match( readbuff, "(.-)\r\n(.*)")
        if msgstr then
            readbuff = rest
            process_msg(fd, msgstr)
        else
            return readbuff
        end
    end
end

--姣忎竴鏉¤繛鎺ユ帴鏀舵暟鎹�澶勭悊
--鍗忚��鏍煎紡 cmd,arg1,arg2,...#
local recv_loop = function(fd)
    socket.start(fd)
    skynet.error("socket connected " ..fd)
    local readbuff = ""
    while true do
        local recvstr = socket.read(fd)
        if recvstr then
            readbuff = readbuff..recvstr
            readbuff = process_buff(fd, readbuff)
        else
            skynet.error("socket close " ..fd)
			disconnect(fd)
            socket.close(fd)
            return
        end
    end
end

--鏈夋柊杩炴帴鏃�
local connect = function(fd, addr)
    print("connect from " .. addr .. " " .. fd)
	local c = conn()
    conns[fd] = c
    c.fd = fd
    skynet.fork(recv_loop, fd)
end

function s.init()
    local node = skynet.getenv("node")
    local nodecfg = runconfig[node]
    local port = nodecfg.gateway[s.id].port

    local listenfd = socket.listen("0.0.0.0", port)
    skynet.error("Listen socket :", "0.0.0.0", port)
    socket.start(listenfd , connect)
end


s.start(...)

--[[ 甯︽湁绮樺寘澶勭悊
function process_msgbuff(id, msgbuff)
    skynet.error("process_msgbuff" .. msgbuff)
    
    local cmd, msg = jspack.unpack(msgbuff)
    
    print(cmd)
    print(msg.hello)
    print(msg.a)
    --socket.write(id, msgbuff)
    --鍒嗗彂
end

function process_buff(id, readbuff)
    while true do
        local bufflen = string.len(readbuff)
        if bufflen < 2 then 
            break
        end
        local len, remain = string.unpack(string.format("> i2 c%d", bufflen-2), readbuff)
        if bufflen < len then
            break
        end

        local str, nextbuff = string.unpack(string.format("> c%d c%d", len, bufflen-2-len), remain)
        readbuff = nextbuff or ""
        
        process_msgbuff(id, str)
    end
    return  readbuff 
end

--姣忎竴鏉¤繛鎺ュ仛澶勭悊
function run(id)
    socket.start(id)
    local readbuff = ""
	while true do
		local str = socket.read(id)
        if str then
            readbuff = readbuff..str
            skynet.error("recv " ..str)
            readbuff = process_buff(id, readbuff)
        else
            skynet.error("close " ..id)
            socket.close(id)
            return
		end
	end
end
--]]
