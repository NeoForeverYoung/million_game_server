#pragma once
#include <vector>
#include "Worker.h"
#include "Service.h"
#include <unordered_map>

class Worker;

class Sunnet {
public:
    //閸楁洑绶�
    static Sunnet* inst;
public:
    //閺嬪嫰鈧�鐘插毐閺侊拷
    Sunnet();
    //閸掓繂锟藉��瀵查獮璺虹磻婵�锟�
    void Start();
    //缁涘�婄窡鏉╂劘锟斤拷
    void Wait();
    //婢х偛鍨归張宥呭��
    uint32_t NewService(shared_ptr<string> type);
    void KillService(uint32_t id);     //娴犲懘妾洪張宥呭�熼懛锟藉�歌精鐨熼悽锟�
    //閸欐垿鈧�浣圭Х閹�锟�
    void Send(uint32_t toId, shared_ptr<BaseMsg> msg);
    //閸忋劌鐪�闂冪喎鍨�閹垮秳缍�
    shared_ptr<Service> PopGlobalQueue();
    void PushGlobalQueue(shared_ptr<Service> srv);
    //鐠佲晛浼愭担婊呭殠缁嬪��鐡戝�板拑绱欐禒鍛�浼愭担婊呭殠缁嬪��鐨熼悽锟介敍锟�
    void WorkerWait();
    //娴犲懏绁寸拠锟�
    shared_ptr<BaseMsg> MakeMsg(uint32_t source, char* buff, int len);
private:
    //瀹搞儰缍旂痪璺ㄢ柤
    int WORKER_NUM = 3;              //瀹搞儰缍旂痪璺ㄢ柤閺佸府绱欓柊宥囩枂閿涳拷
    vector<Worker*> workers;         //worker鐎电�呰杽
    vector<thread*> workerThreads;   //缁捐法鈻�
    //閺堝秴濮熼崚妤勩€�
    unordered_map<uint32_t, shared_ptr<Service>> services;
    uint32_t maxId = 0;              //閺堚偓婢讹拷ID
    pthread_rwlock_t servicesLock;   //鐠囪�插晸闁匡拷
    //閸忋劌鐪�闂冪喎鍨�
    queue<shared_ptr<Service>> globalQueue;
    int globalLen = 0;               //闂冪喎鍨�闂€鍨�瀹�
    pthread_spinlock_t globalLock;   //闁匡拷
    //娴兼垹婀㈤崪灞芥暅闁憋拷
    pthread_mutex_t sleepMtx;
    pthread_cond_t sleepCond;
    int sleepCount = 0;        //娴兼垹婀㈠�搞儰缍旂痪璺ㄢ柤閺侊拷 

private:
    //瀵�鈧�閸氾拷瀹搞儰缍旂痪璺ㄢ柤
    void StartWorker();
    //閸炪倝鍟嬪�搞儰缍旂痪璺ㄢ柤
    void CheckAndWeakUp();
    //閼惧嘲褰囬張宥呭��
    shared_ptr<Service> GetService(uint32_t id);
};