#ifndef __SOCKS5_H__
#define __SOCKS5_H__

#include"Epoll.h"

class Socks5Server : public EpollServer
{
  public:
    Socks5Server(int port = 8001)
      :EpollServer(port)
    {}

    //验证及建立连接
    bool AuthHandle(int connectfd);
    int EstabishmentHandle(int connectfd);

    //重写虚函数
    virtual void ConnectEventHandle(int connectfd);
    virtual void ReadEventHandle(int connectfd);
    //不需要重写
    //virtual void WriteEventHandle(int connectfd);
};

#endif//SOCKS5


