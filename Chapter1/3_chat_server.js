var net = require('net');

// 存储所有连接到聊天服务器的客户端
var scenes = new Map();

var server = net.createServer(function(socket){
    scenes.set(socket, true) // 记录新连接的客户端

    socket.on('data', function(data) { // 接收到客户端发送的聊天消息
        for (let s of scenes.keys()) {
            s.write(data); // 将消息广播给所有连接的客户端
        }
    });
});

server.listen(8010); // 监听8010端口