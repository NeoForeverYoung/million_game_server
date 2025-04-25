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

--鑾峰彇鍦ㄧ嚎浜烘暟
function get_online_count()
	local count = 0
	for playerid, player in pairs(players) do
		count = count+1
	end
	return count
end

--灏唍um鏁伴噺鐨勭帺瀹惰涪涓嬬嚎
s.resp.shutdown = function(source, num)
	--褰撳墠鐜╁�舵暟
	local count = get_online_count()
	--韪�涓嬬嚎
	local n = 0
	for playerid, player in pairs(players) do
		skynet.fork(s.resp.reqkick, nil, playerid, "close server")
		n = n + 1	--璁℃暟锛屾€诲叡鍙憂um鏉′笅绾挎秷鎭�
		if n >= num then
			break
		end
	end
	--绛夊緟鐜╁�舵暟涓嬮檷
	while true do
		skynet.sleep(200)
		local new_count = get_online_count()
		skynet.error("shutdown online:"..new_count)
		if new_count <= 0 or new_count <= count-num then
			return new_count
		end
	end
end





--鎯呭喌 姘镐笉涓嬬嚎





s.start(...)
