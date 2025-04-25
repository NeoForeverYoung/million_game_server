#include "Service.h"
#include "Sunnet.h"
#include <iostream>
#include <unistd.h>
#include <string.h>
#include "LuaAPI.h"

//��见墙��祉𤗈�黸���嚙�
Service::Service() {
    //��埝��嚙賢洩撖脤𪃭嚙�
    pthread_spin_init(&queueLock, PTHREAD_PROCESS_PRIVATE);//�䥥戭祆����坔�𥟇�罸��憭厩��瘨𥪜�嘥偺��𡜐蕭��𥕦革kynet�𤁗摨萘�鮋錬��扳��
    pthread_spin_init(&inGlobalLock, PTHREAD_PROCESS_PRIVATE);
}

//��𧢲�剔�舫�𤏸𥁒���
Service::~Service(){
    pthread_spin_destroy(&queueLock);
    pthread_spin_destroy(&inGlobalLock);
}

//�縧��穃��憡穃�煺��
void Service::PushMsg(shared_ptr<BaseMsg> msg) {
    pthread_spin_lock(&queueLock);
    {
        msgQueue.push(msg);
    }
    pthread_spin_unlock(&queueLock);
}

//��蹱𡵆�黆憡穃�煺��
shared_ptr<BaseMsg> Service::PopMsg() {
    shared_ptr<BaseMsg> msg = NULL;
    //��蹱僙蝡湧�争��蝘琿𦀩嚙�
    pthread_spin_lock(&queueLock);
    {
        if (!msgQueue.empty()) { 
            msg =  msgQueue.front();
            msgQueue.pop();
        }
    }
    pthread_spin_unlock(&queueLock);
    return msg;
}

//瞉嗅𤅷��𦠜�𣏾�祇�争��蝘琿𦀩嚙賡�𥕦��蝜煾摚��聆�祇𡢿�𥈡��䜘�行孛��𡄯��嚙賢𤅷���
bool Service::ProcessMsg() {
    shared_ptr<BaseMsg> msg = PopMsg();
    if(msg) {
        OnMsg(msg);
        return true;
    }
    else {
        return false;
    }
} 

//瞉嗅𤅷��𨂾��争��蝘琿𦀩嚙賡�𥕦��蝜煾摚��聆�祇𡢿�𥈡��䜘�行孛��𡄯��嚙賢𤅷���
void Service::ProcessMsgs(int max) {
    for(int i=0; i<max; i++){
        bool succ = ProcessMsg();
        if(!succ){
            break;
        }
    }
}

//��埝�条�㯄��撊�憪罸�𡁜漱�閖�辷蕭
void Service::OnInit() {
    cout << "[" << id <<"] OnInit"  << endl;
    //����踎蝻尞ua��𤩺鬼憳䠷��嚙�
    luaState = luaL_newstate();
    luaL_openlibs(luaState); 
    //憡剹�亙𤪖Sunnet蝏航搇蝎態PI
    LuaAPI::Register(luaState);
    //�𦄡��嚙賢迭ua����𣺹甈�
    string filename = "../service/" + *type + "/init.lua";
    int isok = luaL_dofile(luaState, filename.data());
    if(isok == 1){ //�縝��砍�𥟇仪�𪃭瘣㚚�𢠃𡢿韐�0��𥕦掃��𤑳�僐�亙熣瘨橒蕭1.
         cout << "run lua fail:" << lua_tostring(luaState, -1) << endl;
    }
    //��见�芣袿Lua��𤏸𥁒���
    lua_getglobal(luaState, "OnInit"); 
    lua_pushinteger(luaState, id); 
    isok = lua_pcall(luaState, 1, 0, 0);
    if(isok != 0){ //�縝��砍�𥟇仪�𪃭瘣㚚�𢠃𡢿韐�0��𥕦掃�����埝�𤾸𥈡��䜘�乓�𤑳�琜蕭.
         cout << "call lua OnInit fail " << lua_tostring(luaState, -1) << endl;
    }
}

//���頝箏�𣬚�嫘ǜ��𤤿�䈑蕭������撋�
void Service::OnSocketData(int fd, const char* buff, int len) {
    //��见�芣袿Lua��𤏸𥁒���
    lua_getglobal(luaState, "OnSocketData"); 
    lua_pushinteger(luaState, fd); 
    lua_pushlstring(luaState, buff,len); 
    int isok = lua_pcall(luaState, 2, 0, 0);
    if(isok != 0){ //�縝��砍�𥟇仪�𪃭瘣㚚�𢠃𡢿韐�0��𥕦掃�����埝�𤾸𥈡��䜘�乓�𤑳�琜蕭.
         cout << "call lua OnSocketData fail " << lua_tostring(luaState, -1) << endl;
    }
}

//瞈�璆�撣渡�𥟇��敶脤�琜蕭
void Service::OnSocketWritable(int fd) {
    cout << "OnSocketWritable " << fd << endl;
    auto w = writers[fd];
    w->OnWriteable();
}

//��𤩺𡂝璉湔仪��游葩��橒蕭
void Service::OnSocketClose(int fd) {
    writers.erase(fd);
    cout << "OnSocketClose " << fd << endl;

    //��见�芣袿Lua��𤏸𥁒���
    lua_getglobal(luaState, "OnSocketClose"); 
    lua_pushinteger(luaState, fd); 
    int isok = lua_pcall(luaState, 1, 0, 0);
    if(isok != 0){ //�縝��砍�𥟇仪�𪃭瘣㚚�𢠃𡢿韐�0��𥕦掃�����埝�𤾸𥈡��䜘�乓�𤑳�琜蕭.
         cout << "call lua OnSocketClose fail " << lua_tostring(luaState, -1) << endl;
    }
}

//���頝箏�屸�𤩺𨑳蝎祇��撊�憪罸�蹱�墧蔥�𨫢�𡠺蝘琿𦀩嚙�
void Service::OnServiceMsg(shared_ptr<ServiceMsg> msg) {
    //��见�芣袿Lua��𤏸𥁒���
    lua_getglobal(luaState, "OnServiceMsg"); 
    lua_pushinteger(luaState, msg->source); 
    lua_pushlstring(luaState, msg->buff.get(), msg->size); 
    int isok = lua_pcall(luaState, 2, 0, 0);
    if(isok != 0){ //�縝��砍�𥟇仪�𪃭瘣㚚�𢠃𡢿韐�0��𥕦掃�����埝�𤾸𥈡��䜘�乓�𤑳�琜蕭.
         cout << "call lua OnServiceMsg fail " << lua_tostring(luaState, -1) << endl;
    }
}

//�����𣬚�偦鑬嚙�
void Service::OnAcceptMsg(shared_ptr<SocketAcceptMsg> msg) {
    cout << "OnAcceptMsg " << msg->clientFd << endl;
    auto w = make_shared<ConnWriter>();
    w->fd = msg->clientFd;
    writers.emplace(msg->clientFd, w);

    //��见�芣袿Lua��𤏸𥁒���
    lua_getglobal(luaState, "OnAcceptMsg"); 
    lua_pushinteger(luaState, msg->listenFd); 
    lua_pushinteger(luaState, msg->clientFd); 
    int isok = lua_pcall(luaState, 2, 0, 0);
    if(isok != 0){ //�縝��砍�𥟇仪�𪃭瘣㚚�𢠃𡢿韐�0��𥕦掃�����埝�𤾸𥈡��䜘�乓�𤑳�琜蕭.
         cout << "call lua OnAcceptMsg fail " << lua_tostring(luaState, -1) << endl;
    }
}

//瞈�璆�撣渡�𥟇��敶脩��霂脣蔡��琜蕭
void Service::OnRWMsg(shared_ptr<SocketRWMsg> msg) {
    int fd = msg->fd;
    //��辷蕭���嚙�
    if(msg->isRead) {
        const int BUFFSIZE = 512;
        char buff[BUFFSIZE];
        int len = 0;
        do {
            len = read(fd, &buff, BUFFSIZE);
            if(len > 0){
                OnSocketData(fd, buff, len);
            }
        }while(len == BUFFSIZE);

        if(len <= 0 && errno != EAGAIN) {
            if(Sunnet::inst->GetConn(fd)) {
                OnSocketClose(fd);
                Sunnet::inst->CloseConn(fd);
            }
        }
    }
    //��辷蕭��鞉�嗵�坔�剹�血贋憡𢞖��皝�else��𨥈蕭
    if(msg->isWrite) {
        if(Sunnet::inst->GetConn(fd)){
            OnSocketWritable(fd);
        }
    }
}



//���頝箏��憡穃�煺�������匾閖�辷蕭
void Service::OnMsg(shared_ptr<BaseMsg> msg) {
    //SERVICE
    if(msg->type == BaseMsg::TYPE::SERVICE) {
        auto m = dynamic_pointer_cast<ServiceMsg>(msg);
        OnServiceMsg(m);
    }
    //SOCKET_ACCEPT
    else if(msg->type == BaseMsg::TYPE::SOCKET_ACCEPT) {
        auto m = dynamic_pointer_cast<SocketAcceptMsg>(msg);
        OnAcceptMsg(m);
    }
    //SOCKET_RW
    else if(msg->type == BaseMsg::TYPE::SOCKET_RW) {
        auto m = dynamic_pointer_cast<SocketRWMsg>(msg);
        OnRWMsg(m);
    }
}


//�𢥫��祇�𤑳�䀹����婙��璊���辷��敶�
void Service::OnExit() {
    cout << "[" << id <<"] OnExit"  << endl;
    //��见�芣袿Lua��𤏸𥁒���
    lua_getglobal(luaState, "OnExit"); 
    int isok = lua_pcall(luaState, 0, 0, 0);
    if(isok != 0){ //�縝��砍�𥟇仪�𪃭瘣㚚�𢠃𡢿韐�0��𥕦掃�����埝�𤾸𥈡��䜘�乓�𤑳�琜蕭.
         cout << "call lua OnExit fail " << 
            lua_tostring(luaState, -1) << endl;
    }
    //��𤩺𡂝璉惻ua��𤩺鬼憳䠷��嚙�
    lua_close(luaState);
}

void Service::SetInGlobal(bool isIn) {
    pthread_spin_lock(&inGlobalLock);
    {
        inGlobal = isIn;
    }
    pthread_spin_unlock(&inGlobalLock);
}