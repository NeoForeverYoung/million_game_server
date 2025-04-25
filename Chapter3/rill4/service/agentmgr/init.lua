local skynet = require "skynet"
local s = require "service"

--鐘舵€�
STATUS = {
	LOGIN = 2,
	GAME = 3,
	LOGOUT = 4,
}

--鐜╁�跺垪琛�
local players = {}

--鐜╁�剁被
function mgrplayer()
    local m = {
        playerid = nil,
		node = nil,
        agent = nil,
		status = nil,
		gate = nil,
    }
    return m
end

s.resp.reqlogin = function(source, playerid, node, gate)
	local mplayer = players[playerid]
	--鐧婚檰杩囩▼绂佹�㈤《鏇�
	if mplayer and mplayer.status == STATUS.LOGOUT then
		skynet.error("reqlogin fail, at status LOGOUT " ..playerid )
		return false
	end
	if mplayer and mplayer.status == STATUS.LOGIN then
		skynet.error("reqlogin fail, at status LOGIN " ..playerid)
		return false
	end
	--鍦ㄧ嚎锛岄《鏇�
	if mplayer then
		local pnode = mplayer.node
		local pagent = mplayer.agent
		local pgate = mplayer.gate
		mplayer.status = STATUS.LOGOUT,
		s.call(pnode, pagent, "kick")
		s.send(pnode, pagent, "exit")
		s.send(pnode, pgate, "send", playerid, {"kick","椤舵浛涓嬬嚎"})
		s.call(pnode, pgate, "kick", playerid)
	end
	--涓婄嚎
	local player = mgrplayer()
	player.playerid = playerid
	player.node = node
	player.gate = gate
    player.agent = nil
	player.status = STATUS.LOGIN
	players[playerid] = player
	local agent = s.call(node, "nodemgr", "newservice", "agent", "agent", playerid)
	player.agent = agent
	player.status = STATUS.GAME
	return true, agent
end

s.resp.reqkick = function(source, playerid, reason)
	local mplayer = players[playerid]
	if not mplayer then
		return false
	end
	
	if mplayer.status ~= STATUS.GAME then
		return false
	end

	local pnode = mplayer.node
	local pagent = mplayer.agent
	local pgate = mplayer.gate
	mplayer.status = STATUS.LOGOUT

	s.call(pnode, pagent, "kick")
	s.send(pnode, pagent, "exit")
	s.send(pnode, pgate, "kick", playerid)
	players[playerid] = nil

	return true
end

--鎯呭喌 姘镐笉涓嬬嚎


s.start(...)
