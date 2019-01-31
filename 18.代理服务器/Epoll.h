#ifndef __EPOLL_H__
#define __EPOLL_H__

#include"Header.h"

class EpollServer
{
  public:
    EpollServer(int port)//ip不用传，监听本机
      :_port(port)
      ,_listenfd(-1)
      ,_eventfd(-1)
    {}

    virtual ~EpollServer()
    {
      if(_listenfd != -1)
        close(_listenfd);
      //eventfd
    }

    void RemoveConnect(int fd);
    void SendInLoop(int fd, const char* buf, size_t len);
    void Forwarding(Channel* clientChannel, Channel* serverChannel, 
        bool recvDecrypt, bool sendEncry);
    
    
    //启动服务，开始监听
    void Start();

    //事件循环
    void EventLoop();

    virtual void ConnectEventHandle(int fd) = 0;
    virtual void ReadEventHAndle(int fd) = 0;
    virtual void WriteEventHandle(int fd) = 0;
  private:
    //防拷贝
    EpollServer(const EpollServer&);
    EpollServer& operator=(const EpollSever&);

  protected:
    int _port; //端口
    int _listenfd; //监听套接字

    int _eventfd;//事件描述符
    static const size_t _MAX_EVENT;//最大事件数
    map<int, Connect*> _connectMap;
};


#endif//__EPOLL_H__
