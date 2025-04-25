
local skynet = require "skynet"
local s = require "service"
local runconfig = require "runconfig"
local mynode = skynet.getenv("node")

s.snode = nil --scene_node
s.sname = nil --scene_id

local function random_scene()
    --閫夋嫨node
    local nodes = {}
    for i, v in pairs(runconfig.scene) do
        table.insert(nodes, i)
        if runconfig.scene[mynode] then
            table.insert(nodes, mynode)
        end
    end
    local idx = math.random( 1, #nodes)
    local scenenode = nodes[idx]
    --鍏蜂綋鍦烘櫙
    local scenelist = runconfig.scene[scenenode]
    local idx = math.random( 1, #scenelist)
    local sceneid = scenelist[idx]
    return scenenode, sceneid
end

s.client.enter = function(msg)
    if s.sname then
        return {"enter",1,"宸插湪鍦烘櫙"}
    end
    local snode, sid = random_scene()
    local sname = "scene"..sid
    local isok = s.call(snode, sname, "enter", s.id, mynode, skynet.self())
    if not isok then
        return {"enter",1,"杩涘叆澶辫触"}
    end
    s.snode = snode
    s.sname = sname
    return nil
end

--鏀瑰彉鏂瑰悜
s.client.shift  = function(msg)
    if not s.sname then
        return
    end
    local x = msg[2] or 0
    local y = msg[3] or 0
    s.call(s.snode, s.sname, "shift", s.id, x, y)
end

s.leave_scene = function()
    --涓嶅湪鍦烘櫙
    if not s.sname then
        return
    end
    s.call(s.snode, s.sname, "leave", s.id)
    s.snode = nil
    s.sname = nil
end