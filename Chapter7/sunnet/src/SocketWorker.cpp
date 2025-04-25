#include "SocketWorker.h"
#include <iostream>
#include <unistd.h>
#include <assert.h>
#include <string.h>

#include <Sunnet.h>
#include <fcntl.h>
#include <sys/socket.h>

//鍒濆�嬪寲
void SocketWorker::Init() {
    cout << "SocketWorker Init" << endl;
    //鍒涘缓epoll
    epollFd = epoll_create(1024); // 杩斿洖鍊硷細闈炶礋鏁�:鎴愬姛鐨勬弿杩扮�︼紝-1澶辫触
    assert(epollFd > 0); 
}


void SocketWorker::OnAccept(shared_ptr<Conn> conn) {
    cout << "OnAccept fd:" << conn->fd << endl;
    //姝ラ��1锛歛ccept
    int clientFd = accept(conn->fd, NULL, NULL);
    if (clientFd < 0) {
        cout << "accept error" << endl;
    }
    //姝ラ��2锛氳�剧疆闈為樆濉�
    fcntl(clientFd, F_SETFL, O_NONBLOCK);
    //鍐欑紦鍐插尯澶у皬
    //unsigned long buffSize = 4294967295;
    //if(setsockopt(clientFd, SOL_SOCKET, SO_SNDBUFFORCE , &buffSize, sizeof(buffSize)) < 0){
    //    cout << "OnAccept setsockopt Fail " << strerror(errno) << endl;
    //}
    //姝ラ��3锛氭坊鍔犲埌绠＄悊缁撴瀯
    Sunnet::inst->AddConn(clientFd, conn->serviceId, Conn::TYPE::CLIENT);
    //姝ラ��4锛氭坊鍔犲埌epoll
    struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLET;
	ev.data.fd = clientFd;
	if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientFd, &ev) == -1) {
		cout << "OnAccept epoll_ctl Fail:" << strerror(errno) << endl;
	}
    //姝ラ��5锛氶€氱煡
    auto msg= make_shared<SocketAcceptMsg>();
    msg->type = BaseMsg::TYPE::SOCKET_ACCEPT;
    msg->listenFd = conn->fd;
    msg->clientFd = clientFd;
    Sunnet::inst->Send(conn->serviceId, msg);
}

void SocketWorker::OnRW(shared_ptr<Conn> conn, bool r, bool w) {
    cout << "OnRW fd:" << conn->fd << endl;
    auto msg= make_shared<SocketRWMsg>();
    msg->type = BaseMsg::TYPE::SOCKET_RW;
    msg->fd = conn->fd;
    msg->isRead = r;
    msg->isWrite = w;
    Sunnet::inst->Send(conn->serviceId, msg);
}



//澶勭悊浜嬩欢
void SocketWorker::OnEvent(epoll_event ev){
    int fd = ev.data.fd;
    auto conn = Sunnet::inst->GetConn(fd);
    if(conn == NULL){
        cout << "OnEvent error, conn == NULL" << endl;
        return;
    }
    //浜嬩欢绫诲瀷
    bool isRead = ev.events & EPOLLIN;
    bool isWrite = ev.events & EPOLLOUT;
    bool isError = ev.events & EPOLLERR;
    //鐩戝惉Socket
    if(conn->type == Conn::TYPE::LISTEN){
        if(isRead) {
            OnAccept(conn);
        }
    }
    //鏅�閫歋ocket
    else {
        if(isRead || isWrite) {
            OnRW(conn, isRead, isWrite);
        }
        if(isError){
            cout << "OnError fd:" << conn->fd << endl;
        }
    }
}

void SocketWorker::operator()() {
    while(true) {
        //闃诲�炵瓑寰�
        const int EVENT_SIZE = 64;
        struct epoll_event events[EVENT_SIZE];
	    int eventCount = epoll_wait(epollFd , events, EVENT_SIZE, -1);
        //鍙栧緱浜嬩欢
        for (int i=0; i<eventCount; i++) {
            epoll_event ev = events[i]; //褰撳墠瑕佸�勭悊鐨勪簨浠�
            OnEvent(ev);
        }
    }
}


//璺ㄧ嚎绋嬭皟鐢�
void SocketWorker::AddEvent(int fd) {
    cout << "AddEvent fd " << fd << endl;
    //娣诲姞鍒癳poll
    struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLET;
	ev.data.fd = fd;
	if (epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &ev) == -1) {
		cout << "AddEvent epoll_ctl Fail:" << strerror(errno) << endl;
	}
}

//璺ㄧ嚎绋嬭皟鐢�
void SocketWorker::RemoveEvent(int fd) {
    cout << "RemoveEvent fd " << fd << endl;
    epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, NULL);
}

//璺ㄧ嚎绋嬭皟鐢�
void SocketWorker::ModifyEvent(int fd, bool epollOut) {
    cout << "ModifyEvent fd " << fd << " " << epollOut << endl;
    struct epoll_event ev;
    ev.data.fd = fd;

    if(epollOut){
	    ev.events = EPOLLIN | EPOLLET | EPOLLOUT;
    }
    else
    {
        ev.events = EPOLLIN | EPOLLET ;
    }
    epoll_ctl(epollFd, EPOLL_CTL_MOD, fd, &ev);
}