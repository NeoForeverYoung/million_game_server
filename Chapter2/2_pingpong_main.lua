--===== Ping-Pong示例主服务 =====
-- 导入skynet核心库
local skynet = require "skynet"

-- skynet服务的入口函数
skynet.start(function()
    -- 输出日志信息，表示主服务已启动
    skynet.error("[Pmain] start")
    
    -- 创建两个ping服务实例，每个实例都是一个独立的服务，拥有唯一的服务ID
    local ping1 = skynet.newservice("ping")  -- 返回服务1的地址(handle)
    local ping2 = skynet.newservice("ping")  -- 返回服务2的地址(handle)
    
    -- 向ping1服务发送异步消息，消息类型为"lua"，内容为"start"命令和ping2的地址
    -- 这将启动两个服务之间的ping-pong通信
    skynet.send(ping1, "lua", "start", ping2)
    
    -- 退出当前服务(主服务)，但不会影响已创建的ping服务继续运行
    skynet.exit()
end)