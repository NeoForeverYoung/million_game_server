#include "Service.h"
#include "Sunnet.h"
#include <iostream>

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
} 

//���頝箏��憡穃�煺�������匾閖�辷蕭
void Service::OnMsg(shared_ptr<BaseMsg> msg) {
    //憡游洎�糓�錬嚙�
    if(msg->type == BaseMsg::TYPE::SERVICE) {
        auto m = dynamic_pointer_cast<ServiceMsg>(msg);
        cout << "[" << id <<"] OnMsg " << m->buff << endl;

        auto msgRet = Sunnet::inst->MakeMsg(id, 
            new char[9999999] { 'p', 'i', 'n', 'g', '\0' }, 9999999);

        Sunnet::inst->Send(m->source, msgRet);
    }
    else {
        cout << "[" << id <<"] OnMsg"  << endl;
    }
}

//�𢥫��祇�𤑳�䀹����婙��璊���辷��敶�
void Service::OnExit() {
    cout << "[" << id <<"] OnExit"  << endl;
}

void Service::SetInGlobal(bool isIn) {
    pthread_spin_lock(&inGlobalLock);
    {
        inGlobal = isIn;
    }
    pthread_spin_unlock(&inGlobalLock);
}