#include<iostream>
#include<string>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<stdlib.h>

void Usage(std::string proc_)
{
  std::cout<<"Usage:"<< proc_<<"server_ip server_port"<<std::endl;
}

int main(int argc, char*argv[])

{
  if(argc != 3)
  {
    Usage(argv[0]);
    exit(1);
  }
  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  if(sock<0)
  {
    std::cout<<"socket error"<<std::endl;
    exit(2);
  }
  std::string message;//发
  char buf[1024];

  struct sockaddr_in server;
  bzero(&server,sizeof(server));
  server.sin_family = AF_INET;
  server.sin_port = htons(atoi(argv[2]));//端口号，本来是char*,要转成网络序列使用htons
  server.sin_addr.s_addr = inet_addr(argv[1]);//inet_addr 将字符串风格的ip地质转换成四字节网络序列的地址
  
  for(;;)
  {
    socklen_t len = sizeof(server);
    std::cout<<"Please Enter#";
    std::cin>>message;
    sendto(sock,message.c_str(),message.size(),0,(struct sockaddr*)&server,len); 
    ssize_t x =recvfrom(sock,buf,sizeof(buf)-1,0, (struct sockaddr*)&server,&len);
    buf[x] = 0;
    std::cout<<"server echo#"<<buf<<std::endl;
  
  }
  return 0;
}
