#include<iostream>
#include<unistd.h>
#include<string.h>
#include<iostream>
#include<string>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<strings.h>
#include<arpa/inet.h>

void Usage(std::string proc)
{
  std::cout<<"Usage: "<<proc<<"server_ip server_port"<<std::endl;
}
int main(int argc, char* argv[])
{
  if(argc != 3)
  {
    Usage(argv[0]);
    exit(1);
  }
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  //客户端可以绑定，但是不建议绑定，可能绑定的端口号被其他服务占用
  //客户端也不需要监听
  //也不要accept
  
  //需要建立连接
  struct sockaddr_in peer;
  socklen_t len = sizeof(peer);//长度
  bzero(&peer,sizeof(peer));//初始化为0
  peer.sin_family = AF_INET;
  peer.sin_port = htons(atoi(argv[2]));
  peer.sin_addr.s_addr = inet_addr(argv[1]);
  
  connect(sock, (struct sockaddr*)&peer, len);
  
  std::string message;
  for(;;)
  {
    std::cout<<"Please Enter# ";
    std::cin>>message;
    char buf[1024];
    write(sock, message.c_str(), message.size());
    ssize_t s = read(sock, buf, sizeof(buf)-1);
    if(s>0)
    {
      buf[s] = 0;
      std::cout<<"server echo# "<< buf <<std::endl;
    }
      
  }

  return 0;
}
