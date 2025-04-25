local skynet = require "skynet"
local s = require "service"

s.client = {}
s.resp.client = function(source, fd, cmd, msg)
    if s.client[cmd] then
        local ret_msg = s.client[cmd]( fd, msg, source)
        skynet.send(source, "lua", "send_by_fd", fd, ret_msg)
    else
        skynet.error("s.resp.client fail", cmd)
    end
end

s.client.login = function(fd, msg, source)
	local playerid = tonumber(msg[2])
	local pw = tonumber(msg[3])
	local gate = source
	node = skynet.getenv("node")
    --鏍￠獙鐢ㄦ埛鍚嶅瘑鐮�
	if pw ~= 123 then
		return {"login", 1, "瀵嗙爜閿欒��"}
	end
	--鍙戠粰agentmgr
	local isok, agent = skynet.call("agentmgr", "lua", "reqlogin", playerid, node, gate)
	if not isok then
		return {"login", 1, "璇锋眰mgr澶辫触"}
	end
	--鍥炲簲gate
	local isok = skynet.call(gate, "lua", "sure_agent", fd, playerid, agent)
	if not isok then
		return {"login", 1, "gate娉ㄥ唽澶辫触"}
	end
    skynet.error("login succ "..playerid)
    return {"login", 0, "鐧婚檰鎴愬姛"}
end

s.start(...)

