
#include "ConnWriter.h"
#include <unistd.h>
#include <Sunnet.h>
#include <iostream>
#include <string.h>
#include <sys/socket.h>

void ConnWriter::EntireWriteWhenEmpty(shared_ptr<char> buff, streamsize len) {
    char* s = buff.get() ;
    //��卝�剁蕭撣桃敦>=0, -1&&EAGAIN, -1&&EINTR, -1&&��𤩺𨑳蝎�
    streamsize n = write(fd, s, len);
    if(n < 0 && errno == EINTR) { }; //瘚惩䴴敶��鰐��滨�条�蓥蝴��鮋緵嚙�
    cout << "EntireWriteWhenEmpty write n=" << n << endl;
    //�㴓�麢���1-1��𥟇鬲��誯鱓��亙�梶�對蕭
    if(n >= 0 && n == len) {
        return;
    }
    //�㴓�麢���1-2��𥟇鬲��𤘪�𣏾�祇鱓��亙�𡡞�𥕦�笔𠸊憡𢞖�喳�㯄�譌�舐��
    if( (n > 0 && n < len) || (n < 0 && errno == EAGAIN) ) {
        auto obj = make_shared<WriteObject>();
        obj->start = n;
        obj->buff = buff;
        obj->len = len;
        objs.push_back(obj);
        Sunnet::inst->ModifyEvent(fd, true);
        return;
    }
    //�㴓�麢���1-3��𥟇停皝⊿𨫢�𤧚敶��錬��潭�羓��嚙�
    cout << "EntireWrite write error " <<  endl;
}

void ConnWriter::EntireWriteWhenNotEmpty(shared_ptr<char> buff, streamsize len) {
    auto obj = make_shared<WriteObject>();
    obj->start = 0;
    obj->buff = buff;
    obj->len = len;
    objs.push_back(obj);
}

void ConnWriter::EntireWrite(shared_ptr<char> buff, streamsize len) {
    if(isClosing){
        cout << "EntireWrite fail, because isClosing" << endl;
        return;
    }
    //�㴓�麢���1��𥟇鬼������憭羓�罸�鞉�誩��������撋���𥕦掃��𤤿�𤩺�羓糓��鞉�誩��
    if(objs.empty()) {
        EntireWriteWhenEmpty(buff, len);
    }
    //�㴓�麢���2��𥟇鬼皝�撖啣麢��㯄�譌�行�罸䌫嚙賡�𥕦掛��𢠃�𠉛𤗈��屸��嚙賜�𧶏蕭
    else{
        EntireWriteWhenNotEmpty(buff, len);
    }
}


//�仪�𪃭瘣㚚�𠺪蕭:���嚙賡�𡄯���𨯵���撖豢�煾�鞉�誩��瘚𨅯�閧契��㚁蕭
bool ConnWriter::WriteFrontObj() {
    //憡𢞖�喟�罸�鞉�鞉�罸䌫嚙�
    if(objs.empty()) {
        return false;
    }
    //�鼗撜啣��蝏梹蕭瘨𣏾�祇�㚁蕭
    auto obj = objs.front();

    //��卝�剁蕭撣桃敦>=0, -1&&EAGAIN, -1&&EINTR, -1&&��𤩺𨑳蝎�
    char* s = obj->buff.get() + obj->start;
    int len = obj->len - obj->start;
    int n = write(fd, s, len);
    cout << "WriteFrontObj write n=" << n << endl;
    if(n < 0 && errno == EINTR) { }; //瘚惩䴴敶��鰐��滨�条�蓥蝴��鮋緵嚙�
    //�㴓�麢���1-1��𥟇鬲��誯鱓��亙�梶�對蕭
    if(n >= 0 && n == len) {
        objs.pop_front(); //��烐揪瑽�
        return true;
    }
    //�㴓�麢���1-2��𥟇鬲��𤘪�𣏾�祇鱓��亙�𡡞�𥕦�笔𠸊憡𢞖�喳�㯄�譌�舐��
    if( (n > 0 && n < len) || (n < 0 && errno == EAGAIN) ) {
        obj->start += n;
        return false;
    }
    //�㴓�麢���1-3��𥟇停皝⊿𨫢�𤧚敶��錬��潭�羓��嚙�
    cout << "EntireWrite write error " << endl;
}

void ConnWriter::OnWriteable() {
    auto conn = Sunnet::inst->GetConn(fd);
    if(conn == NULL){ //�仪��游葩摰豢�鍦𧃍���嚙�
        return;
    }

    while(WriteFrontObj()){
        //撖堆蕭��頣蕭
    }
    
    if(objs.empty()) {
        Sunnet::inst->ModifyEvent(fd, false);

        if(isClosing) {
            //�𢥫瘞梁�⊿��撊�憪罸�𥕦掛嚙賬��嚙賢𤧚���瘨枏��瑽賊𢥫瘞望袿��𧢲鬼蝖�
            //���儮𤫟ad瘚䲷���� Bad file descriptor�𦅚��拇��
            cout << "linger close conn" << endl;
            shutdown(fd, SHUT_RD);
            auto msg= make_shared<SocketRWMsg>();
            msg->type = BaseMsg::TYPE::SOCKET_RW;
            msg->fd = conn->fd;
            msg->isRead = true;
            Sunnet::inst->Send(conn->serviceId, msg);
        }
    }
}

void ConnWriter::LingerClose(){
    if(isClosing){
        return;
    }
    isClosing = true;
    if(objs.empty()) {
        Sunnet::inst->CloseConn(fd);
        return;
    }
}
