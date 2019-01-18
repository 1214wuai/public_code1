#pragma once
#include<iostream>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<cassert>
#include<string>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;

class Sock{
  public:
    Sock(std::string &ip_, int&port_):ip(ip_), port(port_)
  {}
    void Socket()
    {
      sock = socket(AF_INET,SOCK_DGRAM,0);
      if(sock<0)
      {
        std::cerr<<"socket error"<<std::endl;
        exit(2);
      }
    }
    void Bind()
    {
      struct sockaddr_in local_;
      local_.sin_family = AF_INET;
      local_.sin_port = htons(port);
      local_.sin_addr.s_addr = inet_addr(ip.c_str());
      if(bind(sock, (struct sockaddr*)&local_,sizeof(local_))<0)
      {
        std::cerr<<"bing error"<<std::endl;
        exit(3);
      }
    }
    
    void Recv(std::string &str_, struct sockaddr_in &peer, socklen_t &len)
    {
      char buf[1024];
      len = sizeof(sockaddr_in);
      ssize_t s = recvfrom(sock,buf,sizeof(buf)-0,0,(struct sockaddr*)&peer, &len);
      if(s>0)
      {
        buf[s] = 0;
        str_ = buf;//C语言的字符串转成C++，所以提前要保证buf以\0结尾
      }
    }
    void send(std::string &str_, struct sockaddr_in &peer, socklen_t &len)
    {
      sendto(sock, str_.c_str(), str_.size(), 0, (struct sockaddr*)&peer, len);
    }
    ~Sock()
    {
      close(sock);
    }
  private:
    int sock;
    std::string ip;
    int port;
};

class UdpServer{
  private:
    Sock sock;
  public:
    UdpServer(std::string ip_, int port_):sock(ip_,port_)
  {}
    void InitServer()
    {
      sock.Socket();//创建
      sock.Bind();//绑定
    }
    void Start()
    {
      struct sockaddr_in peer;
      socklen_t len;
      std::string message;
      for(;;)
      {
        sock.Recv(message,peer,len);
        std::cout<<"["<<inet_ntoa(peer.sin_addr)<<":"<<ntohs(peer.sin_port)<<"]#"<<message<<std::endl;
        
        message+=" server";//受到消息时，服务器加一句server，再发出去
        
        sock.send(message,peer,len);//收什么，发什么
        std::cout<<"server echo success"<<std::endl;
      }
    }
    ~UdpServer()
    {

    }
};
