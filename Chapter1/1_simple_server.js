var net = require('net');

var server = net.createServer(function(socket){
    console.log('connected, port:' + socket.remotePort);

    //鎺ユ敹鍒版暟鎹�
    socket.on('data', function(data){
        console.log('client send:' + data);
        var ret = "鍥炲��," + data;
        socket.write(ret);
    });

    //鏂�寮€杩炴帴
    socket.on('close',function(){
        console.log('closed, port:' + socket.remotePort);
    });
});
server.listen(8001);