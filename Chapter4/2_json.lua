local skynet = require "skynet"
local cjson = require "cjson"

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

    return cmd, msg
end


--瑙ｇ爜娴嬭瘯
function test2()
    local buff = [[{"_cmd":"enter","playerid":101,"x":10,"y":20,"size":1}]]
    local isok, msg = pcall(cjson.decode, buff)
    if isok then
        print(msg._cmd)  -- enter
        print(msg.playerid) -- 101.0
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
    test1()
	--test2()
	--test3()
end)