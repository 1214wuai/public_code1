#include"TcpServer.hpp"
void Usage(std::string argv)
{
  std::cout<<argv<<" local_port"<<std::endl;
}

int main(int argc, char* argv[])
{
  if(argc != 2)
  {
    Usage(argv[0]);
    exit(0);
  }
  int port = atoi(argv[1]);
  Server *sp = new Server(port);
  sp->InitServer();
  sp->Run();
  delete sp;

  return 0;
}
