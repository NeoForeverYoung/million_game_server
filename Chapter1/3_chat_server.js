var net = require('net');

var scenes = new Map();

var server = net.createServer(function(socket){
    scenes.set(socket, true) //閺傛媽绻涢幒锟�

    socket.on('data', function(data) { //閺€璺哄煂濞戝牊浼�
        for (let s of scenes.keys()) {
            s.write(data);
        }
    });
});

server.listen(8010);