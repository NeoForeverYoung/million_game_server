#include "Sunnet.h"
#include <iostream>
#include <assert.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

using namespace std;

//閸楁洑绶�
Sunnet* Sunnet::inst;
Sunnet::Sunnet(){
    inst = this;
}

//瀵�鈧�閸氱椇orker缁捐法鈻�
void Sunnet::StartWorker() {
    for (int i = 0; i < WORKER_NUM; i++) {
        cout << "start worker thread:" << i << endl;
        //閸掓稑缂撶痪璺ㄢ柤鐎电�呰杽
        Worker* worker = new Worker();
        worker->id = i;
        worker->eachNum = 2 << i;
        //閸掓稑缂撶痪璺ㄢ柤
        thread* wt = new thread(*worker);
        //濞ｈ�插�為崚鏉垮灙鐞涳拷
        workers.push_back(worker);
        workerThreads.push_back(wt);
    }
}

//瀵�鈧�閸氱柇ocket缁捐法鈻�
void Sunnet::StartSocket() {
    //閸掓稑缂撶痪璺ㄢ柤鐎电�呰杽
    socketWorker = new SocketWorker();
    //閸掓繂锟藉��瀵�
    socketWorker->Init();
    //閸掓稑缂撶痪璺ㄢ柤
    socketThread = new thread(*socketWorker);
}

//瀵�鈧�閸氾拷缁�鑽ょ埠
void Sunnet::Start() {
    cout << "Hello Sunnet" << endl;
    //韫囩晫鏆怱IGPIPE娣団€冲娇
    signal(SIGPIPE, SIG_IGN);
    //闁匡拷
    pthread_rwlock_init(&servicesLock, NULL);
    assert(pthread_rwlock_init(&connsLock, NULL)==0);
    pthread_spin_init(&globalLock, PTHREAD_PROCESS_PRIVATE);
    pthread_cond_init(&sleepCond, NULL);
    pthread_mutex_init(&sleepMtx, NULL);    
    //瀵�鈧�閸氱柣orker
    StartWorker();
    //瀵�鈧�閸氱柇ocket缁捐法鈻�
    StartSocket();
}

//缁涘�婄窡
void Sunnet::Wait() {
    if( workerThreads[0]) {
        workerThreads[0]->join();
    }
}

//閺傛澘缂撻張宥呭��
uint32_t Sunnet::NewService(shared_ptr<string> type) {
    auto srv = make_shared<Service>();
    srv->type = type;
    pthread_rwlock_wrlock(&servicesLock);
    {
        srv->id = maxId; 
        maxId++;
        services.emplace(srv->id, srv);
    }
    pthread_rwlock_unlock(&servicesLock);
    srv->OnInit(); //閸掓繂锟藉��瀵�
    return srv->id;
}

//閻㈢湈d閺屻儲澹橀張宥呭��
shared_ptr<Service> Sunnet::GetService(uint32_t id) {
    shared_ptr<Service> srv = NULL;
    pthread_rwlock_rdlock(&servicesLock);
    {
        unordered_map<uint32_t, shared_ptr<Service>>::iterator iter = services.find (id);
        if (iter != services.end()){
            srv = iter->second;
        }
    }
    pthread_rwlock_unlock(&servicesLock);
    return srv;
}

//閸掔娀娅庨張宥呭��
//閸欙拷閼崇氮ervice閼凤拷瀹歌精鐨熼懛锟藉�告唻绱濋崶鐘辫礋srv->OnExit閵嗕够rv->isExiting娑撳秴濮為柨锟�
void Sunnet::KillService(uint32_t id) {
    shared_ptr<Service> srv = GetService(id);
    if(!srv){
        return;
    }
    //闁�鈧�閸戝搫澧�
    srv->OnExit();
    srv->isExiting = true;
    //閸掔姴鍨�鐞涳拷
    pthread_rwlock_wrlock(&servicesLock);
    {
        services.erase(id);
    }
    pthread_rwlock_unlock(&servicesLock);
}


//閸欐垿鈧�浣圭Х閹�锟�
void Sunnet::Send(uint32_t toId, shared_ptr<BaseMsg> msg){
    shared_ptr<Service> toSrv = GetService(toId);
    if(!toSrv){
        cout << "Send fail, toSrv not exist toId:" << toId << endl;
        return;
    }
    toSrv->PushMsg(msg);
    //濡�鈧�閺屻儱鑻熼弨鎯у弳閸忋劌鐪�闂冪喎鍨�
    //娑撹櫣缂夌亸蹇庡�嶉悾灞藉隘閻忓灚妞块幒褍鍩楅敍宀€鐗�閸у繐鐨濈憗鍛�鈧�锟�
    bool hasPush = false;
    pthread_spin_lock(&toSrv->inGlobalLock);
    {
        if(!toSrv->inGlobal) {
            PushGlobalQueue(toSrv);
            toSrv->inGlobal = true;
            hasPush = true;
        }
    }
    pthread_spin_unlock(&toSrv->inGlobalLock);
    //閸炪倛鎹ｆ潻娑氣柤閿涘奔绗夐弨鎯ф躬娑撳�告櫕閸栨椽鍣烽棃锟�
    if(hasPush) {
        CheckAndWeakUp();
    }
}

//瀵�鐟板毉閸忋劌鐪�闂冪喎鍨�
shared_ptr<Service> Sunnet::PopGlobalQueue(){
    shared_ptr<Service> srv = NULL;
    pthread_spin_lock(&globalLock);
    {
        if (!globalQueue.empty()) {
            srv = globalQueue.front();
            globalQueue.pop();
            globalLen--;
        }
    }
    pthread_spin_unlock(&globalLock);
    return srv;
}

//閹绘帒鍙嗛崗銊ョ湰闂冪喎鍨�
void Sunnet::PushGlobalQueue(shared_ptr<Service> srv){
    pthread_spin_lock(&globalLock);
    {
        globalQueue.push(srv);
        globalLen++;
    }
    pthread_spin_unlock(&globalLock);
}


//娴犲懏绁寸拠鏇犳暏閿涘異uff妞よ崵鏁眓ew娴溠呮晸
shared_ptr<BaseMsg> Sunnet::MakeMsg(uint32_t source, char* buff, int len) {
    auto msg= make_shared<ServiceMsg>();
    msg->type = BaseMsg::TYPE::SERVICE;
    msg->source = source;
    //閸╃儤婀扮猾璇茬€烽惃鍕�锟界�呰杽濞屸剝婀侀弸鎰�鐎�閸戣姤鏆�
    //閹碘偓娴犮儱娲栭弨璺虹唨閺堬拷缁�璇茬€风紒鍕�鍨氶惃鍕�鏆熺紒鍕�鈹栭梻瀵告暏delete 閸滐拷 delete[]闁�钘夊讲娴狅拷
    //閺冪娀娓堕柌宥嗘煀閺嬫劖鐎�閺傝�勭《
    msg->buff = shared_ptr<char>(buff);
    msg->size = len;
    return msg;
}

//Worker缁捐法鈻肩拫鍐�鏁ら敍宀冪箻閸忋儰绱ら惇锟�
void Sunnet::WorkerWait(){
    pthread_mutex_lock(&sleepMtx);
    sleepCount++;
    pthread_cond_wait(&sleepCond, &sleepMtx);
    sleepCount--;
    pthread_mutex_unlock(&sleepMtx); 
}


//濡�鈧�閺屻儱鑻熼崬銈夊晪缁捐法鈻�
void Sunnet::CheckAndWeakUp(){
    //unsafe
    if(sleepCount == 0) {
        return;
    }
    if( WORKER_NUM - sleepCount <= globalLen ) {
        cout << "weakup" << endl; 
        pthread_cond_signal(&sleepCond);
    }
}


//濞ｈ�插�炴潻鐐村复
int Sunnet::AddConn(int fd, uint32_t id, Conn::TYPE type) {
    auto conn = make_shared<Conn>();
    conn->fd = fd;
    conn->serviceId = id;
    conn->type = type;
    pthread_rwlock_wrlock(&connsLock);
    {
        conns.emplace(fd, conn);
    }
    pthread_rwlock_unlock(&connsLock);
    return fd;
}

//閻㈢湈d閺屻儲澹樻潻鐐村复
shared_ptr<Conn> Sunnet::GetConn(int fd) {
    shared_ptr<Conn> conn = NULL;
    pthread_rwlock_rdlock(&connsLock);
    {
        unordered_map<uint32_t, shared_ptr<Conn>>::iterator iter = conns.find (fd);
        if (iter != conns.end()){
            conn = iter->second;
        }
    }
    pthread_rwlock_unlock(&connsLock);
    return conn;
}

//閸掔娀娅庢潻鐐村复
bool Sunnet::RemoveConn(int fd) {
    int result;
    pthread_rwlock_wrlock(&connsLock);
    {
        result = conns.erase(fd);
    }
    pthread_rwlock_unlock(&connsLock);
    return result == 1;
}

int Sunnet::Listen(uint32_t port, uint32_t serviceId) {
    //閸掓稑缂揝ocket
    int listenFd = socket(AF_INET, SOCK_STREAM, 0);
    if(listenFd <= 0){
        cout << "listen error, listenFd <= 0" << endl;
        return -1;
    }
    fcntl(listenFd, F_SETFL, O_NONBLOCK);
    //閸掓稑缂撻崷鏉挎絻缂佹挻鐎�
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    //bind
    int r = bind(listenFd, (struct sockaddr*)&addr, sizeof(addr));
    if( r == -1){
        cout << "listen error, bind fail" << endl;
        return -1;
    }
    //listen
    r = listen(listenFd, 64); //see
    if(r < 0){
        return -1;
    }
    //濞ｈ�插�為崚鎵�锟斤紕鎮婄紒鎾寸€�
    AddConn(listenFd, serviceId, Conn::TYPE::LISTEN);
    //Epoll娴滃�╂�㈤敍鍫ｆ硶缁捐法鈻奸敍锟�
    socketWorker->AddEvent(listenFd);
    return listenFd;
}


void Sunnet::CloseConn(uint32_t fd) {
    //閸掔娀娅庣粻锛勬倞缂佹挻鐎�
    bool succ = RemoveConn(fd);
    //閸忔娊妫�
    close(fd);
    //Epoll娴滃�╂�㈤敍鍫ｆ硶缁捐法鈻奸敍锟�
    if(succ) {
        socketWorker->RemoveEvent(fd);
    }
}

void Sunnet::ModifyEvent(int fd, bool epollOut) {
    socketWorker->ModifyEvent(fd, epollOut);
}