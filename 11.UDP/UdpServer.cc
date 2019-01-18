#include"UdpServer.hpp"

void Usage(std::string proc_)
{
  std::cout<<"Usage: "<<"server_ip server_port"<<std::endl;
}

int main(int argc, char* argv[])
{
  if(argc != 3)
  {
    Usage(argv[0]);
    exit(0);
  }
  UdpServer *sp = new UdpServer(argv[1],atoi(argv[2]));
  sp->InitServer();
  sp->Start();
  delete sp;
  return 0;
}

