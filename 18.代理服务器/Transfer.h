#ifndef __TRANSFER_H__
#define __TRANSFER_H__

#include"Epoll.h"

class TransferServer : public EpollServer
{
  public:
    TransferServer(const char* socks5ServerIp, int socks5ServerPort,int selfport = 8888):EpollServer(selfport)
  {
    //父类在初始化列表初始化
    memset(&_socks5addr,0,sizeof(struct sockaddr_in));
    _socks5addr.sin_family = AF_INET;
    _socks5addr.sin_port = htons(socks5ServerPort);
    _socks5addr.sin_addr.s_addr = inet_addr(socks5ServerIp);
  }
    virtual void ConnectEventHandle(int connectfd);
    virtual void ReadEventHandle(int connectfd);
    //不需要重写
    //virtual void WriteEventHandle(int connectfd);
  protected:
    struct sockaddr_in _socks5addr;
};

#endif//__TRANSFER_H__


