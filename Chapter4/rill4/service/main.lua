local skynet = require "skynet"
local skynet_manager = require "skynet.manager"
local runconfig = require "runconfig"
local cluster = require "skynet.cluster"



skynet.start(function()
	--閸掓繂锟藉��瀵�
	local mynode = skynet.getenv("node")
	local nodecfg = runconfig[mynode]
	--閼哄倻鍋ｇ粻锛勬倞
	local nodemgr = skynet.newservice("nodemgr","nodemgr", 0)
	skynet.name("nodemgr", nodemgr)
	--闂嗗棛鍏�
	cluster.reload(runconfig.cluster)
	cluster.open(mynode)
	--gate
	for i, v in pairs(nodecfg.gateway or {}) do
		local srv = skynet.newservice("gateway","gateway", i)
		skynet.name("gateway"..i, srv)
	end
	--login
	for i, v in pairs(nodecfg.login or {})  do
	local srv = skynet.newservice("login","login", i)
		skynet.name("login"..i, srv)
	end
	--agentmgr
	local anode = runconfig.agentmgr.node
	if mynode == anode then
		local srv = skynet.newservice("agentmgr", "agentmgr", 0)
		skynet.name("agentmgr", srv)
	else
		local proxy = cluster.proxy(anode, "agentmgr")
		skynet.name("agentmgr", proxy)
	end
	--scene (sid->sceneid)
	for _, sid in pairs(runconfig.scene[mynode] or {}) do
		local srv = skynet.newservice("scene", "scene", sid)
		skynet.name("scene"..sid, srv)
	end
	--admin
	local admin_node = "node1" --鐠囩备unconfig闁板秶鐤�
	if mynode == anode then
		skynet.newservice("admin", "admin", 0)
	end
	--闁�鈧�閸戦缚鍤滈煬锟�
    skynet.exit()
end)