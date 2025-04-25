#pragma once
#include <memory>
using namespace std; 

//濞戝牊浼呴崺铏硅��
class BaseMsg {
public:
    enum TYPE {          //濞戝牊浼呯猾璇茬€�
        SERVICE = 1, 
    }; 
    uint8_t type;           //濞戝牊浼呯猾璇茬€�
    char load[999999]{};    //閻�銊ょ艾濡�鈧�濞村��鍞寸€涙ɑ纭犲�曪拷
    virtual ~BaseMsg(){};
};

//閺堝秴濮熼梻瀛樼Х閹�锟�
class ServiceMsg : public BaseMsg  {
public: 
    uint32_t source;        //濞戝牊浼呴崣鎴︹偓浣规煙
    shared_ptr<char> buff;  //濞戝牊浼呴崘鍛�锟斤拷
    size_t size;            //濞戝牊浼呴崘鍛�锟界懓銇囩亸锟�
};

