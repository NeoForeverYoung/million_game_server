#include "LuaAPI.h"
#include "stdint.h"
#include "Sunnet.h"
#include <unistd.h>
#include <string.h>
#include <iostream>

//濞夈劌鍞絃ua濡�鈥虫健
void LuaAPI::Register(lua_State *luaState) {
    
    static luaL_Reg lualibs[] = {
        { "NewService", NewService },
        { "KillService", KillService },
        { "Send", Send },

        { "Listen", Listen },
        { "CloseConn", CloseConn },
        { "Write", Write },
        { NULL, NULL }
    };

    luaL_newlib (luaState, lualibs);
    lua_setglobal(luaState, "sunnet");
}


//瀵�鈧�閸氾拷閺傜増婀囬崝锟�
int LuaAPI::NewService(lua_State *luaState) {
    //閸欏倹鏆熸稉锟介弫锟�
    int num = lua_gettop(luaState);//閼惧嘲褰囬崣鍌涙殶閻ㄥ嫪閲滈弫锟�
    //閸欏倹鏆�1閿涙碍婀囬崝锛勮��閸�锟�
    if(lua_isstring(luaState, 1) == 0){  //1:閺勶拷 0:娑撳秵妲�
        lua_pushinteger(luaState, -1);
        return 1;
    }
    size_t len = 0;
    const char *type = lua_tolstring(luaState, 1, &len);
    char * newstr = new char[len+1]; //閸氬酣娼伴崝鐕�0 
    newstr[len] = '\0';
    memcpy(newstr, type, len);
    auto t = make_shared<string>(newstr);
    //婢跺嫮鎮�
    uint32_t id = Sunnet::inst->NewService(t);
    //鏉╂柨娲栭崐锟�
    lua_pushinteger(luaState, id);
    return 1;
}

int LuaAPI::KillService(lua_State *luaState) {
    //閸欏倹鏆�
    int num = lua_gettop(luaState);//閼惧嘲褰囬崣鍌涙殶閻ㄥ嫪閲滈弫锟�
    if(lua_isinteger(luaState, 1) == 0) {
        return 0;
    }
    int id = lua_tointeger(luaState, 1);
    //婢跺嫮鎮�
    Sunnet::inst->KillService(id);
    //鏉╂柨娲栭崐锟�
    //閿涘牊妫ら敍锟�
    return 0;
}

//閸欐垿鈧�浣圭Х閹�锟�
int LuaAPI::Send(lua_State *luaState) {
    //閸欏倹鏆熼幀缁樻殶
    int num = lua_gettop(luaState);
    if(num != 3) {
        cout << "Send fail, num err" << endl;
        return 0;
    }
    //閸欏倹鏆�1:閹存垶妲哥拫锟�
    if(lua_isinteger(luaState, 1) == 0) {
        cout << "Send fail, arg1 err" << endl;
        return 0;
    }
    int source = lua_tointeger(luaState, 1);
    //閸欏倹鏆�2:閸欐垿鈧�浣虹舶鐠嬶拷
    if(lua_isinteger(luaState, 2) == 0) {
        cout << "Send fail, arg2 err" << endl;
        return 0;
    }
    int toId = lua_tointeger(luaState, 2);
    //閸欏倹鏆�3:閸欐垿鈧�浣烘畱閸愬懎锟斤拷
    if(lua_isstring(luaState, 3) == 0){
        cout << "Send fail, arg3 err" << endl;
        return 0;
    }
    size_t len = 0;
    const char *buff = lua_tolstring(luaState, 3, &len);
    char * newstr = new char[len];
    memcpy(newstr, buff, len);
    //婢跺嫮鎮�
    auto msg= make_shared<ServiceMsg>();
    msg->type = BaseMsg::TYPE::SERVICE;
    msg->source = source;
    msg->buff = shared_ptr<char>(newstr);
    msg->size = len;
    Sunnet::inst->Send(toId, msg);
    //鏉╂柨娲栭崐锟�
    //閿涘牊妫ら敍锟�
    return 0;
}

//瀵�鈧�閸氾拷缂冩垹绮堕惄鎴濇儔
int LuaAPI::Listen(lua_State *luaState){
    //閸欏倹鏆熸稉锟介弫锟�
    int num = lua_gettop(luaState);
    //閸欏倹鏆�1閿涙氨锟斤拷閸欙拷
    if(lua_isinteger(luaState, 1) == 0) {
        lua_pushinteger(luaState, -1);
        return 1;
    }
    int port = lua_tointeger(luaState, 1);
    //閸欏倹鏆�2閿涙碍婀囬崝锟絀d
    if(lua_isinteger(luaState, 2) == 0) {
        lua_pushinteger(luaState, -1);
        return 1;
    }
    int id = lua_tointeger(luaState, 2);
    //婢跺嫮鎮�
    int fd = Sunnet::inst->Listen(port, id);
    //鏉╂柨娲栭崐锟�
    lua_pushinteger(luaState, fd);
    return 1;
}

//閸忔娊妫存潻鐐村复
int LuaAPI::CloseConn(lua_State *luaState){
    //閸欏倹鏆熸稉锟介弫锟�
    int num = lua_gettop(luaState);
    //閸欏倹鏆�1閿涙瓲d
    if(lua_isinteger(luaState, 1) == 0) {
        return 0;
    }
    int fd = lua_tointeger(luaState, 1);
    //婢跺嫮鎮�
    Sunnet::inst->CloseConn(fd);
    //鏉╂柨娲栭崐锟�
    //閿涘牊妫ら敍锟�
    return 0;
}

//閸愭瑥锟芥�佸复鐎涳拷
int LuaAPI::Write(lua_State *luaState){
    //閸欏倹鏆熸稉锟介弫锟�
    int num = lua_gettop(luaState);
    //閸欏倹鏆�1閿涙瓲d
    if(lua_isinteger(luaState, 1) == 0) {
        lua_pushinteger(luaState, -1);
        return 1;
    }
    int fd = lua_tointeger(luaState, 1);
    //閸欏倹鏆�2閿涙瓬uff
    if(lua_isstring(luaState, 2) == 0){
        lua_pushinteger(luaState, -1);
        return 1;
    }
    size_t len = 0;
    const char *buff = lua_tolstring(luaState, 2, &len);
    //婢跺嫮鎮�
    int r = write(fd, buff, len);
    //鏉╂柨娲栭崐锟�
    lua_pushinteger(luaState, r);
    return 1;
}