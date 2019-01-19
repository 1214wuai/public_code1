#pragma once

#include<string.h>
#include<iostream>
#include<string>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<strings.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<signal.h>

class Sock{
  private:
    std::string ip;
    int port;
    int listen_sock;
  public:
    Sock(const std::string &ip_,const int &port_):ip(ip_),port(port_),listen_sock(-1)
  {}
    void Socket()
    {
      listen_sock = socket(AF_INET, SOCK_STREAM, 0);
      if(listen_sock<0)
      {
        std::cerr<<"sock error"<<std::endl;
        exit(2);
      }
    }

    void Bind()
    {
      struct sockaddr_in local;
      bzero(&local,sizeof(local));
      local.sin_family = AF_INET;
      local.sin_port = htons(port);
      //local.sin_addr.s_addr = inet_addr(ip.c_str());
      //假设电脑有两台电脑，就有两个ip，此时绑定只能绑定一个
      //只能向一个ip地址发数据
      //
      
      local.sin_addr.s_addr = htonl((INADDR_ANY));//实际上式全0
      
      //此时就可以不需要ip，减少工作量
      //但是客户端一定要有ip

      if(bind(listen_sock, (struct sockaddr*)&local, sizeof(local))<0)
      {
        std::cerr<<"bind error"<<std::endl;
        exit(3);
      }
    }

    void Listen()
    {
      if(listen(listen_sock, 5)<0)
      {
        std::cerr<<"listen error"<<std::endl;
        exit(4);
      }
    }

    int Accept()
    {
      struct sockaddr_in peer;
      socklen_t len = sizeof(peer);
      int sock = accept(listen_sock,(struct sockaddr*)&peer, &len );
      if(sock < 0)
      {
        std::cerr<<"accept error"<<std::endl;
        return -1;
      }
      return sock;
    }
    ~Sock()
    {
      close(listen_sock);
    }
};

class Server{
  private:
    Sock sock;
  public:
    Server(const std::string &ip_, const int &port_):sock(ip_, port_)
  {}

    void InitServer()
    {
      signal(SIGCHLD, SIG_IGN);//忽略子进程退出的信号
      sock.Socket();
      sock.Bind();
      sock.Listen();
    }
    
    void Serverce(int sock_)
    {
      char buf[1024];
      for(;;)
      {
        ssize_t s = read(sock_, buf, sizeof(buf)-1);
        if(s>0)
        {
          buf[s] = 0;
          std::cout<<buf<<std::endl;
          write(sock_,buf,strlen(buf));
        }
        else if(s == 0)
        {
          std::cout<<"client is quit..."<<std::endl;
          break;
        }
        else{
          std::cerr<<"read error"<<std::endl;
        }
      }
      close(sock_);
    }
    void Run()
    {
      for(;;)
      {
        int new_sock = sock.Accept();//获得新连接
        if(new_sock < 0)
        {
          continue;//获取失败，重新去获取
        }
        std::cout<<"Get a New Client..."<<std::endl;
        //Serverce(new_sock);//获取成功，进行服务,此时只能服务一个，后续客户会被阻塞
        pid_t id = fork();
        if(id == 0)
        {
          Serverce(new_sock);
          exit(0);
        }
        //父进程必须关闭new_sock
        close(new_sock);
      }
    }
    ~Server()
    {}

};
