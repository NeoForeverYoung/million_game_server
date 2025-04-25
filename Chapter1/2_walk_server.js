var net = require('net');

// 角色类，用于存储角色的位置信息
class Role{
    constructor() {
        this.x = 0;
        this.y = 0;
    }
}

// 存储所有连接的客户端对应的角色信息
var roles = new Map();

var server = net.createServer(function(socket){
    // 新客户端连接时，为其创建一个角色
    roles.set(socket, new Role())

    // 处理客户端发送的命令
    socket.on('data', function(data){
        var role = roles.get(socket);
        var cmd = String(data);
        // 根据命令更新角色位置
        if(cmd == "left\r\n") role.x--;
        else if(cmd == "right\r\n") role.x++;
        else if(cmd == "up\r\n") role.y--;
        else if(cmd == "down\r\n") role.y++;

        // 广播位置变化给所有客户端
        for (let s of roles.keys()) {
            // 获取移动角色的客户端信息并通知所有客户端
            var ip = socket.remoteAddress;
            var id = socket.remotePort;
            var str = ip + ":" + id + " move to " + role.x + " " + role.y + "\n";
            s.write(str);
        }
    });

    // 客户端断开连接时，删除对应的角色
    socket.on('close',function(){
        roles.delete(socket)
    });
});

server.listen(8001); // 监听8001端口