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
    // 新连接建立时，为客户端创建角色
    roles.set(socket, new Role())

    // 接收到客户端发送的数据
    socket.on('data', function(data){
        var role = roles.get(socket);
        var cmd = String(data);
        // 根据命令更新角色位置
        if(cmd == "left\r\n") role.x--;
        else if(cmd == "right\r\n") role.x++;
        else if(cmd == "up\r\n") role.y--;
        else if(cmd == "down\r\n") role.y++;
		else { 
            // 将非移动消息转发给聊天服务器
            chatSocket.write(data);
            return;
        };
        // 广播位置变化给所有客户端
        for (let s of roles.keys()) {
            var id = socket.remotePort;
            var str = id + " move to " + role.x + " " + role.y + "\n";
            s.write(str);
        }
    });

    // 客户端断开连接
    socket.on('close',function(){
        roles.delete(socket)
    });
});

server.listen(8001); // 监听8001端口


var chatSocket = net.connect({port: 8010}, function() {}); // 连接到聊天服务器
// 聊天室，将聊天服务器的消息转发给walk server的所有客户端
chatSocket.on('data', function(data){
    for (let s of roles.keys()) {
        s.write(data);
    }
});