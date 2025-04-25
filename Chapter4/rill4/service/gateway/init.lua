local skynet = require "skynet"
local socket = require "skynet.socket"
local s = require "service"
local runconfig = require "runconfig"

conns = {} --[socket_id] = conn
players = {} --[playerid] = gateplayer

local closing = false
--娑撳秴鍟€閹恒儲鏁归弬鎷岀箾閹猴拷
s.resp.shutdown = function()
    skynet.error(s.name..s.id.." shutdown")
    closing = true
end

--鏉╃偞甯寸猾锟�
function conn()
    local m = {
        fd = nil,
        playerid = nil,
    }
    return m
end

--閻溾晛锟藉墎琚�
function gateplayer()
    local m = {
        playerid = nil,
        agent = nil,
        conn = nil,
        key = 675475980 ,--math.random( 1, 999999999),
        lost_conn_time = nil,
        msgcache = {}, --閺堬拷閸欐垿鈧�浣烘畱濞戝牊浼呯紓鎾崇摠
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
        --return 娑斿��澧犻弰锟介惄瀛樺复return
        --閹哄�屽殠閿涘瞼绱︾€涙ê锟藉嫮鎮婇妴锟�
        --濞夈劍鍓伴敍姘�锟姐倗绱︾€涙�奸兇缂佺喎锟界懓绨查惃鍕�妲哥€广垺鍩涚粩锟芥稉璇插З閹哄�屽殠閵嗕線鏀ｇ仦蹇曠搼閹�鍛�鍠岀€佃壈鍤ч惃鍕�甯€缁惧尅绱濋懓灞肩瑝閺勶拷闁炬崘鐭炬稉宥夆偓姘辨畱閹�鍛�鍠岄妴鍌滅处鐎涙ɑ鏆熼幑锟介崣锟介張澶嬪竴缁惧灝鎮楅惃鍕�绔村▓纰夌礉娑撳秴瀵橀崥锟介崜宄磑cket閸欐垿鈧�浣搞亼鐠愩儳娈�
        table.insert( gplayer.msgcache, msg )
        local len = #gplayer.msgcache
        if len > 500 then
            skynet.call("agentmgr", "lua", "reqkick", playerid, "gate濞戝牊浼呯紓鎾崇摠鏉╁洤锟斤拷")
        end
        return
    end
    
    s.resp.send_by_fd(nil, c.fd, msg)
end

s.resp.sure_agent = function(source, fd, playerid, agent)

	local conn = conns[fd]
	if not conn then --閻у�氭�版潻鍥┾柤娑擄拷瀹歌尙绮℃稉瀣�鍤�
		skynet.call("agentmgr", "lua", "reqkick", playerid, "閺堬拷鐎瑰本鍨氶惂濠氭�伴崡鍏呯瑓缁撅拷")
		return false
	end
	
	conn.playerid = playerid
	
    local gplayer = gateplayer()
    skynet.error("sure_agent key:"..gplayer.key)
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
    --鏉╂ɑ鐥呯€瑰本鍨氶惂璇茬秿
    if not playerid then
        return
    --瀹告彃婀�濞撳憡鍨欐稉锟�
    else
        local gplayer = players[playerid]
        gplayer.conn = nil --  players[playerid] = nil
        skynet.timeout(30*100, function()
            if gplayer.conn ~= nil then
                return
            end
            local reason = "閺傦拷缁捐儻绉撮弮锟�"
            skynet.call("agentmgr", "lua", "reqkick", playerid, reason)
        end)
        
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





local process_reconnect = function(fd, msg)
    local playerid = tonumber(msg[2])
    local key = tonumber(msg[3])
    --conn
    local conn = conns[fd]
    if not conn then
        skynet.error("reconnect fail, conn not exist")
        return
    end   
    --gplayer
    local gplayer = players[playerid]
    if not gplayer then
        skynet.error("reconnect fail, player not exist")
        return
    end
    if gplayer.conn then
        skynet.error("reconnect fail, conn not break")
        return
    end
    if gplayer.key ~= key then
        skynet.error("reconnect fail, key error")
        return
    end
    --缂佹垵鐣�
    gplayer.conn = conn
    conn.playerid = playerid
    --閸ョ偛绨�
    s.resp.send_by_fd(nil, fd, {"reconnect", 0})
    --閸欐垿鈧�浣虹处鐎涙ɑ绉烽幁锟�
    for i, cmsg in ipairs(gplayer.msgcache) do
        s.resp.send_by_fd(nil, fd, cmsg)
    end
    gplayer.msgcache = {}
end


local process_msg = function(fd, msgstr)
    local cmd, msg = str_unpack(msgstr)
    skynet.error("recv "..fd.." ["..cmd.."] {"..table.concat( msg, ",").."}")

    local conn = conns[fd]
    local playerid = conn.playerid
    --閻楄�勭暕閺傦拷缁惧潡鍣告潻锟�
    if cmd == "reconnect" then
        process_reconnect(fd, msg)
        return
    end
    --鐏忔碍婀�鐎瑰本鍨氶惂璇茬秿濞翠胶鈻�
    if not playerid then
        local node = skynet.getenv("node")
        local nodecfg = runconfig[node]
        local loginid = math.random(1, #nodecfg.login)
        local login = "login"..loginid
		skynet.send(login, "lua", "client", fd, cmd, msg)
    --鐎瑰本鍨氶惂璇茬秿濞翠胶鈻�
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

--濮ｅ繋绔撮弶陇绻涢幒銉﹀复閺€鑸垫殶閹癸拷婢跺嫮鎮�
--閸楀繗锟斤拷閺嶇厧绱� cmd,arg1,arg2,...#
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

--閺堝�嬫煀鏉╃偞甯撮弮锟�
local connect = function(fd, addr)
    if closing then
        return
    end
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
