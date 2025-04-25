--===== MySQL数据库操作示例 =====
-- 导入skynet核心库和MySQL模块
local skynet = require "skynet"
local mysql = require "skynet.db.mysql"

-- skynet服务的入口函数
skynet.start(function()
    -- 连接MySQL数据库
    local db = mysql.connect({
        host="39.100.116.101",      -- 数据库主机地址
        port=3306,                  -- 数据库端口
        database="message_board",   -- 数据库名称
        user="root",                -- 数据库用户名
        password="7a77-788b889aB",  -- 数据库密码
        max_packet_size = 1024 * 1024,  -- 最大数据包大小(1MB)
        on_connect = nil            -- 连接回调函数(未使用)
    })
    
    -- 向留言表中插入一条新记录
    local res = db:query("insert into msgs (text) values (\'hehe\')")
    
    -- 查询留言表中的所有记录
    res = db:query("select * from msgs")
    
    -- 遍历查询结果集并打印每条记录的ID和内容
    for i, v in pairs(res) do
        print(i, " ", v.id, " ", v.text)
    end
end)