local skynet = require "skynet"
local cjson = require "cjson"


local pb = require "protobuf"



local mysql = require "skynet.db.mysql"
local db


local playerdata = {
    baseinfo = {},  --鍩烘湰淇℃伅
    bag = {},       --鑳屽寘
    task = {},      --浠诲姟
    friend = {},    --鏈嬪弸
    mail = {},      --閭�浠�
    achieve = {},   --鎴愬氨
    title = {},     --绉板彿
    --鈥︹€�
}

function test7()
    pb.register_file("./storage/playerdata.pb")
    local sql = string.format("select * from baseinfo where playerid = 109")
    local res = db:query(sql)


    local data = res[1].data
    print("data len:"..string.len(data))
    --璇诲彇
    local udata = pb.decode("playerdata.BaseInfo", data)
    if not udata then
        print("error")
        return false
    end
    local playerdata = udata

    print("coin:"..playerdata.coin)
    print("name:"..playerdata.name)
    print("time:"..playerdata.last_login_time)
    print("skin:"..playerdata.skin)
end


function test6()
    pb.register_file("./storage/playerdata.pb")
    --鍒涜��
    local playerdata = {
        playerid = 109,
        coin = 97,
        name = "Tiny",
        level = 3,
        last_login_time = os.time(),
    }
    local data = pb.encode("playerdata.BaseInfo", playerdata)
    print("data len:"..string.len(data))
    local sql = string.format("insert into baseinfo (playerid, data) values (%d, %s)", 109, mysql.quote_sql_str(data))
    local res = db:query(sql)
    if res.err then
        print("error:"..res.err)
    else
        print("ok")
    end
end

--杩炴帴閮ㄥ垎鐣ワ紝璇诲彇鐜╁�舵暟鎹�
function test5()
    local playerdata = {}
    local res = db:query("select * from player where playerid = 105")
    if not res or not res[1] then
        print("loading error")
        return false
    end
    playerdata.coin = res[1].coin
    playerdata.name = res[1].name
    playerdata.last_login_time = res[1].last_login_time

    if res[1].skin then
        playerdata.skin = res[1].skin
    else
        playerdata.skin = 1
    end

    print("coin:"..playerdata.coin)
    print("name:"..playerdata.name)
    print("time:"..playerdata.last_login_time)
    print("skin:"..playerdata.skin)
end



--protobuf缂栫爜瑙ｇ爜
function test4()
    pb.register_file("./proto/login.pb")
    --缂栫爜
    local msg = {
        id = 101,
        pw = "123456",
    }
    local buff = pb.encode("login.Login", msg)
    print("len:"..string.len(buff))
    --瑙ｇ爜
    local umsg = pb.decode("login.Login", buff)
    if umsg then
        print("id:"..umsg.id)
        print("pw:"..umsg.pw)
    else
        print("error")
    end
end


function json_pack(cmd, msg)
    msg._cmd = cmd
    local body = cjson.encode(msg)    --鍗忚��浣撳瓧鑺傛祦
	local namelen = string.len(cmd)   --鍗忚��鍚嶉暱搴�
    local bodylen = string.len(body)  --鍗忚��浣撻暱搴�
	local len = namelen + bodylen + 2 --鍗忚��鎬婚暱搴�
	local format = string.format("> i2 i2 c%d c%d", namelen, bodylen)
	local buff = string.pack(format, len, namelen, cmd, body)
    return buff
end

function json_unpack(buff)
	local len = string.len(buff)
	local namelen_format = string.format("> i2 c%d", len-2)
    local namelen, other = string.unpack(namelen_format, buff)
    local bodylen = len-2-namelen
	local format = string.format("> c%d c%d", namelen, bodylen)
	local cmd, bodybuff = string.unpack(format, other)

    local isok, msg = pcall(cjson.decode, bodybuff)
    if not isok or not msg or not msg._cmd or not cmd == msg._cmd then
        print("error")
        return
    end

    return msg._cmd, msg
end

--鍗忚��娴嬭瘯
function test3()
    local msg = {
        _cmd = "playerinfo",
        coin = 100,
        bag = {
            [1] = {1001,1},  --鍊氬ぉ鍓�*1
            [2] = {1005,5}   --鑽夎嵂*5
        },
    }
    --缂栫爜
    local buff_with_len = json_pack("playerinfo", msg)
    local len = string.len(buff_with_len)
    print("len:"..len)
    print(buff_with_len)
    --瑙ｇ爜
    local format = string.format(">i2 c%d", len-2)
    local _, buff = string.unpack(format, buff_with_len)
    local cmd, umsg = json_unpack(buff)
    print("cmd:"..cmd)
    print("coin:"..umsg.coin)
    print("sword:"..umsg.bag[1][2])
end

--瑙ｇ爜娴嬭瘯
function test2()
    local buff = [[{"_cmd":"enter","playerid":101,"x":10,"y":20,"size":1}]]
    local isok, msg = pcall(cjson.decode, buff)
    if isok then
        print(msg._cmd)
        print(msg.playerid)
    else
        print("error")
    end
end

--缂栫爜娴嬭瘯
function test1()
    local msg = {
        _cmd = "balllist",
        balls = {
            [1] = {id=102, x=10, y=20, size=1},
            [2] = {id=103, x=10, y=30, size=2},
        }
    }
    local buff = cjson.encode(msg)
    print(buff)
end

skynet.start(function()
    --杩炴帴
	db=mysql.connect({
		host="39.100.116.101",
		port=3306,
		database="playerdata",
		user="root",
		password="12345678aB+",
		max_packet_size = 1024 * 1024,
		on_connect = nil
    })
    

    test7()
end)