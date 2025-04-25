#pragma once
#include <queue>
#include <thread>
#include "Msg.h"
#include "ConnWriter.h"
#include <unordered_map>

extern "C"  {  
    #include "lua.h"  
    #include "lauxlib.h"
    #include "lualib.h"  
}  

using namespace std;

class Service {
public:
    //娑撶儤鏅ラ悳鍥╀紥濞茬粯鈧�褎鏂侀崷鈺瑄blic

    //閸烇拷娑撯偓id
    uint32_t id;
    //缁�璇茬€�
    shared_ptr<string> type;
    // 閺勶拷閸氾附锟斤絽婀�闁�鈧�閸戯拷
    bool isExiting = false;
    //濞戝牊浼呴崚妤勩€�
    queue<shared_ptr<BaseMsg>> msgQueue;
    pthread_spinlock_t queueLock;
    //閺嶅洩锟界増妲搁崥锕€婀�閸忋劌鐪�闂冪喎鍨�  true:閸︺劑妲﹂崚妞捐厬閿涘本鍨ㄥ�濓絽婀�婢跺嫮鎮�
    bool inGlobal = false;
    pthread_spinlock_t inGlobalLock;
    //娑撴艾濮熼柅鏄忕帆閿涘牅绮庡ù瀣�鐦�娴ｈ法鏁ら敍锟�
    unordered_map<int, shared_ptr<ConnWriter>> writers;
public:       
    //閺嬪嫰鈧�鐘叉嫲閺嬫劖鐎�閸戣姤鏆�
    Service();
    ~Service();
    //閸ョ偠鐨熼崙鑺ユ殶閿涘牏绱�閸愭瑦婀囬崝锟犫偓鏄忕帆閿涳拷
    void OnInit();
    void OnMsg(shared_ptr<BaseMsg> msg);
    void OnExit();
    //閹绘帒鍙嗗☉鍫熶紖
    void PushMsg(shared_ptr<BaseMsg> msg);
    //閹笛嗭拷灞剧Х閹�锟�
    bool ProcessMsg();
    void ProcessMsgs(int max);  
    //閸忋劌鐪�闂冪喎鍨�
    void SetInGlobal(bool isIn);
private:
    //Lua閾忔碍瀚欓張锟�
    lua_State *luaState;
private:
    //閸欐牕鍤�娑撯偓閺夆剝绉烽幁锟�
    shared_ptr<BaseMsg> PopMsg();
    //濞戝牊浼呮径鍕�鎮婇弬瑙勭《
    void OnServiceMsg(shared_ptr<ServiceMsg> msg);
    void OnAcceptMsg(shared_ptr<SocketAcceptMsg> msg);
    void OnRWMsg(shared_ptr<SocketRWMsg> msg);
    void OnSocketData(int fd, const char* buff, int len);
    void OnSocketWritable(int fd);
    void OnSocketClose(int fd);
};