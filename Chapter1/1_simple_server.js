var net = require('net');

var server = net.createServer(function(socket){
    console.log('connected, port:' + socket.remotePort);

    // 接收到客户端发送的数据时触发
    socket.on('data', function(data){
        console.log('client send:' + data);
        var ret = "回复," + data;
        socket.write(ret);
    });

    // 客户端断开连接时触发
    socket.on('close',function(){
        console.log('closed, port:' + socket.remotePort);
    });
});
server.listen(8001); // 监听8001端口