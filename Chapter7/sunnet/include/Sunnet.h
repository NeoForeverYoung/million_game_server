#pragma once
#include <vector>
#include "Worker.h"
#include "Service.h"
#include <unordered_map>
#include "SocketWorker.h"
#include "Conn.h"

class Worker;

class Sunnet {
public:
    //��埈�𡁶極
    static Sunnet* inst;
public:
    //��见墙��祉𤗈�黸���嚙�
    Sunnet();
    //��埝��嚙賢洩撖脤�噼滓蝝烐耨嚙�
    void Start();
    //蝏𥕦�羓���仪��荔蕭嚙�
    void Wait();
    //瞉抒�脣㧜���撊�憪�
    uint32_t NewService(shared_ptr<string> type);
    void KillService(uint32_t id);     //瘚惩㧥瑼粹��撊�憪罸㞾嚙賢捂颲怎�罸錬嚙�
    //��蹱ề��砌蝴蝘琿𦀩嚙�
    void Send(uint32_t toId, shared_ptr<BaseMsg> msg);
    //��譌�亦𧋦�����瑕𧖣�礶撊�蝬�
    shared_ptr<Service> PopGlobalQueue();
    void PushGlobalQueue(shared_ptr<Service> srv);
    //������隡鞉童皛���𡒊�见洵��穃秣���蝝蹱�惩麢隡鞉童皛���𡒊�见洎��罸錬嚙賡�𨥈蕭
    void WorkerWait();
    //瘚惩䴴蟡渡��嚙�
    shared_ptr<BaseMsg> MakeMsg(uint32_t source, char* buff, int len);
    //瞉抒�脣㧜��䕘蕭Conn
    int AddConn(int fd, uint32_t id, Conn::TYPE type);
    shared_ptr<Conn> GetConn(int fd);
    bool RemoveConn(int fd);
    //蝻���删眸�仪��游葩�礶撊�蝬娪鑬��亙�偦�𥕦𨯂�袿��条�瘀蕭慦鬉ad write��𨥈蕭
    int Listen(uint32_t port, uint32_t serviceId);
    void CloseConn(uint32_t fd);
    //��萇麯嚙賣�晽ent�鑬��亙��
    void ModifyEvent(int fd, bool epollOut);
private:
    //摰詻�斤�𠉛遝頝兩䲰
    int WORKER_NUM = 3;              //摰詻�斤�𠉛遝頝兩䲰���撣桃�䠷鰵撊������𨥈蕭
    vector<Worker*> workers;         //worker��萇�����
    vector<thread*> workerThreads;   //蝏曇楊�䲰
    //Socket蝏曇楊�䲰
    SocketWorker* socketWorker;
    thread* socketThread;
    //���撊�憪罸�埝�����
    unordered_map<uint32_t, shared_ptr<Service>> services;
    uint32_t maxId = 0;              //�����祆噤嚙瘢D
    pthread_rwlock_t servicesLock;   //���霂脣�㯄𪃭嚙�
    //��譌�亦𧋦�����瑕𧖣
    queue<shared_ptr<Service>> globalQueue;
    int globalLen = 0;               //�����瑕𧖣����𡆇摰�
    pthread_spinlock_t globalLock;   //�𪃭嚙�
    //瘚潭�䭾飾��𨅯掃��𣈯鰐嚙�
    pthread_mutex_t sleepMtx;
    pthread_cond_t sleepCond;
    int sleepCount = 0;        //瘚潭�䭾飾摰詻�斤�𠉛遝頝兩䲰���嚙� 
    //Conn��埝�����
    unordered_map<uint32_t, shared_ptr<Conn>> conns;
    pthread_rwlock_t connsLock;   //���霂脣�㯄𪃭嚙�

private:
    //撖栽�祇�𡄯蕭摰詻�斤�𠉛遝頝兩䲰
    void StartWorker();
    //��𠺶�匧�见捂��斤�𠉛遝頝兩䲰
    void CheckAndWeakUp();
    //撖栽�祇�𡁶鱏ocket蝏曇楊�䲰
    void StartSocket();
    //�鼗撜啣�����撊�憪�
    shared_ptr<Service> GetService(uint32_t id);
};