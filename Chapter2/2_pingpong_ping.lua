--===== Ping-Pong示例服务 =====
-- 导入skynet核心库
local skynet = require "skynet"

-- 命令处理表，用于处理接收到的不同类型的消息
local CMD = {}

-- 启动ping-pong通信的处理函数
-- @param source: 消息发送者的服务地址
-- @param target: 目标服务的地址
function CMD.start(source, target)
    -- 向目标服务发送ping消息，初始计数为1
    skynet.send(target, "lua", "ping", 1)
end

-- 处理接收到的ping消息
-- @param source: 消息发送者的服务地址
-- @param count: ping-pong的计数值
function CMD.ping(source, count)
    -- 获取当前服务的地址
    local id = skynet.self()
    -- 输出日志，显示接收到的ping消息和计数
    skynet.error("["..id.."] recv ping count="..count)
    -- 休眠100个时间单位(10ms)，模拟处理延迟
    skynet.sleep(100)
    -- 向消息发送者回复ping消息，计数值加1
    skynet.send(source, "lua", "ping", count+1)
end

-- skynet服务的入口函数
skynet.start(function()
    -- 注册lua类型消息的分发处理函数
    skynet.dispatch("lua", function(session, source, cmd, ...)
      -- 根据命令名查找对应的处理函数
      local f = assert(CMD[cmd])
      -- 调用处理函数，传入消息源和其他参数
      f(source,...)
    end)
end)