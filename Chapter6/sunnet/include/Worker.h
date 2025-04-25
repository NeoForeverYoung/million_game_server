#pragma once
#include <thread> 
#include "Sunnet.h"
#include "Service.h"
class Sunnet;

using namespace std;

class Worker { 
public:
    int id;             //缂栧彿
    int eachNum;        //姣忔�″�勭悊澶氬皯鏉℃秷鎭�
    void operator()();  //绾跨▼鍑芥暟
private:
    //杈呭姪鍑芥暟
    void CheckAndPutGlobal(shared_ptr<Service> srv);
};