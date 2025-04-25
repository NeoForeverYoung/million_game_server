#pragma once
#include <list>
#include <stdint.h>
#include <memory>
using namespace std; 

class WriteObject {
public:
    streamsize start;
    streamsize len;
    shared_ptr<char> buff;
};

class ConnWriter {
public:
    int fd;
private:
    //鏄�鍚︽�ｅ湪鍏抽棴
    bool isClosing = false;
    list<shared_ptr<WriteObject>> objs;  //鍙屽悜閾捐〃
public:
    void EntireWrite(shared_ptr<char> buff, streamsize len);
    void LingerClose(); //鍏ㄩ儴鍙戝畬瀹屽啀鍏抽棴
    void OnWriteable();
private:
    void EntireWriteWhenEmpty(shared_ptr<char> buff, streamsize len);
    void EntireWriteWhenNotEmpty(shared_ptr<char> buff, streamsize len);
    bool WriteFrontObj();
};