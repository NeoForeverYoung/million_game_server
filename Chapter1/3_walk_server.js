var net = require('net');

class Role{
    constructor() {
        this.x = 0;
        this.y = 0;
    }
}

var roles = new Map();

var server = net.createServer(function(socket){
    //鏂拌繛鎺�
    roles.set(socket, new Role())

    //鎺ユ敹鍒版暟鎹�
    socket.on('data', function(data){
        var role = roles.get(socket);
        var cmd = String(data);
        //鏇存柊浣嶇疆
        if(cmd == "left\r\n") role.x--;
        else if(cmd == "right\r\n") role.x++;
        else if(cmd == "up\r\n") role.y--;
        else if(cmd == "down\r\n") role.y++;
		else { 
            // 灏嗘秷鎭�杞�鍙戠粰鑱婂ぉ瀹�
            chatSocket.write(data);
            return;
        };
        //骞挎挱
        for (let s of roles.keys()) {
            var id = socket.remotePort;
            var str = id + " move to " + role.x + " " + role.y + "\n";
            s.write(str);
        }
    });

    //鏂�寮€杩炴帴
    socket.on('close',function(){
        roles.delete(socket)
    });
});

server.listen(8001);


var chatSocket = net.connect({port: 8010}, function() {});
// 鑱婂ぉ瀹�, 灏嗘秷鎭�杞�鍙戠粰walk server鎵€鏈夎繛鎺ョ殑瀹㈡埛绔�
chatSocket.on('data', function(data){
    for (let s of roles.keys()) {
        s.write(data);
    }
});