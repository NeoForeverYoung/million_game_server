var net = require('net');

class Role{
    constructor() {
        this.x = 0;
        this.y = 0;
    }
}

var roles = new Map();

var server = net.createServer(function(socket){
    //�����𣬚�偦鑬嚙�
    roles.set(socket, new Role())

    //�鑬��行鈫��垍����罸䌫嚙�
    socket.on('data', function(data){
        var role = roles.get(socket);
        var cmd = String(data);
        //���摮䀹�𦠜童撊����
        if(cmd == "left\r\n") role.x--;
        else if(cmd == "right\r\n") role.x++;
        else if(cmd == "up\r\n") role.y--;
        else if(cmd == "down\r\n") role.y++;

        //撉墧�擧窓
        for (let s of roles.keys()) {
            // �鼗撜啣�宷ocket�𨫢�𨉼蝜嗵�见洵嚙踝蕭��辷蕭, 撉墧�唳���䌫嚙賣�枏���𩣪蝏梹蛾閬�
            var ip = socket.remoteAddress;
            var id = socket.remotePort;
            var str = ip + ":" + id + " move to " + role.x + " " + role.y + "\n";
            s.write(str);
        }
    });

    //���嚙賢祚��祆仪��游葩
    socket.on('close',function(){
        roles.delete(socket)
    });
});

server.listen(8001);