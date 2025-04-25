#include <iostream>
#include <unistd.h>
#include "Worker.h"
#include "Service.h"
using namespace std;

//闁�锝勭昂鐠嬪儜unnet閻ㄥ嫰鈧�姘崇箖娴肩姴寮�閺佹媽袙閸愶拷
//閻樿埖鈧�浣规Ц娑撳秴婀�闂冪喎鍨�娑擄拷閿涘疅lobal=true
void Worker::CheckAndPutGlobal(shared_ptr<Service> srv) {
    //闁�鈧�閸戣桨鑵戦敍鍫濆涧閼冲€熷殰瀹歌精鐨熼柅鈧�閸戠尨绱漣sExiting娑撳秳绱扮痪璺ㄢ柤閸愯尙鐛婇敍锟�
    if(srv->isExiting){ 
        return; 
    }

    pthread_spin_lock(&srv->queueLock);
    {
        //闁插秵鏌婇弨鎯ф礀閸忋劌鐪�闂冪喎鍨�
        if(!srv->msgQueue.empty()) {
            //濮濄倖妞俿rv->inGlobal娑撯偓鐎规碍妲竧rue
            Sunnet::inst->PushGlobalQueue(srv);
        }
        //娑撳秴婀�闂冪喎鍨�娑擄拷閿涘矂鍣哥拋绶刵Global
        else {
            srv->SetInGlobal(false);
        }
    }
    pthread_spin_unlock(&srv->queueLock);
}



//缁捐法鈻奸崙鑺ユ殶
void Worker::operator()() {
    while(true) {
        shared_ptr<Service> srv = Sunnet::inst->PopGlobalQueue();
        if(!srv){
            Sunnet::inst->WorkerWait();
        }
        else{
            srv->ProcessMsgs(eachNum);
            CheckAndPutGlobal(srv);
        }
    }
}