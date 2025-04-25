#pragma once
using namespace std;
#include <sys/epoll.h>
#include <memory>
#include "Conn.h"

class SocketWorker { 
private:
    //epoll鎻忚堪绗�
    int epollFd;
public:
    void Init();        //鍒濆�嬪寲
    void operator()();  //绾跨▼鍑芥暟
public:
    void AddEvent(int fd);
    void RemoveEvent(int fd);
    void ModifyEvent(int fd, bool epollOut);
private:
    void OnEvent(epoll_event ev);
    void OnAccept(shared_ptr<Conn> conn);
    void OnRW(shared_ptr<Conn> conn, bool r, bool w);
};