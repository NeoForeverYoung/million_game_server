#pragma once
using namespace std; 

class Conn {
public:
    enum TYPE {          //濞戝牊浼呯猾璇茬€�
        LISTEN = 1, 
        CLIENT = 2,
    }; 

    uint8_t type;
    int fd;
    uint32_t serviceId;

};