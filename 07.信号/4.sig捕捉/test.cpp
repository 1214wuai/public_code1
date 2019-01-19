#include<iostream>
#include<signal.h>
#include<unistd.h>

using namespace std;
void handler(int signo)
{
  cout << "catch a sig： "<< signo << endl; 
}
int main ()
{
  struct sigaction act, oact;
  act.sa_handler = handler;
  sigemptyset(&(act.sa_mask));
  //act.sa_flags = SA_RESETHAND ;//自定义函数只执行一次
  act.sa_flags = 0;
  sigaction(2, &act, &oact);

  while(1)
  {
     cout<< "hello world" << endl;
    sleep(1);
  }
  return 0;
}
