--===== 留言板服务器示例(MySQL版) =====
-- 导入skynet核心库、socket模块和MySQL模块
local skynet = require "skynet"
local socket = require "skynet.socket"
local mysql = require "skynet.db.mysql"

-- 全局数据库连接对象
local db = nil

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
			-- 处理"get"命令 - 返回所有留言内容
			if readdata == "get\r\n" then
				-- 查询数据库中的所有留言
				local res = db:query("select * from msgs")
				-- 遍历结果集，将每条留言发送给客户端
				for i,v in pairs(res) do
					socket.write(fd, v.id.." "..v.text.."\r\n")
				end
			-- 处理"set"命令 - 添加新留言
			else
				-- 使用正则表达式从命令中提取留言内容
				local data = string.match(readdata, "set (.-)\r\n")
				-- 将留言插入数据库
				db:query("insert into msgs (text) values (\'"..data.."\')")
			end
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
    
    -- 连接MySQL数据库
    db = mysql.connect({
            host="127.0.0.1",           -- 数据库主机地址
            port=3306,                  -- 数据库端口
            database="message_board",   -- 数据库名称
            user="root",                -- 数据库用户名
            password="12345678aB+",     -- 数据库密码
            max_packet_size = 1024 * 1024,  -- 最大数据包大小
            on_connect = nil            -- 连接回调函数(未使用)
        })
end)