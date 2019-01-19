#include"TcpServer.hpp"
void Usage(std::string argv)
{
  std::cout<<argv<<" local_ip local_port"<<std::endl;
}

int main(int argc, char* argv[])
{
  if(argc != 3)
  {
    Usage(argv[0]);
    exit(0);
  }
  std::string ip = argv[1];
  int port = atoi(argv[2]);
  Server *sp = new Server(ip, port);
  sp->InitServer();
  sp->Run();
  delete sp;

  return 0;
}
