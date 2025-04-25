--===== Echo服务器示例 =====
-- 导入skynet核心库和socket模块
local skynet = require "skynet"
local socket = require "skynet.socket"

-- 客户端连接处理函数
-- @param fd: socket文件描述符，用于标识连接
-- @param addr: 客户端的地址信息
function connect(fd, addr)
    -- 输出连接信息
    print(fd.." connected addr:"..addr)
    
    -- 启动socket，准备接收数据
    socket.start(fd)
    
    -- 循环处理客户端消息
    while true do
        -- 从socket中读取数据，如果客户端断开连接则返回nil
        local readdata = socket.read(fd)
        
        -- 如果成功读取到数据
        if readdata ~= nil then
            -- 输出接收到的数据
            print(fd.." recv "..readdata)
            -- 将数据原样发送回客户端(echo功能)
            socket.write(fd, readdata)
        -- 如果客户端断开连接
        else
            -- 输出关闭连接信息
            print(fd.." close ")
            -- 关闭socket连接
            socket.close(fd)
            -- 退出循环，结束该连接的处理
            break
        end
    end
end
    
-- skynet服务的入口函数
skynet.start(function()
    -- 在指定地址和端口创建监听socket
    local listenfd = socket.listen("0.0.0.0", 8888)
    -- 启动监听socket，当有新连接时调用connect函数处理
    socket.start(listenfd, connect)
end)