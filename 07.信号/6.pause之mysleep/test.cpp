#include<iostream>
#include<signal.h>
#include<unistd.h>
using namespace std;
void handler(int signo)
{;}

int MySleep(int sec)
{
  struct sigaction act,oact;
  act.sa_handler = handler;
  sigemptyset(&act.sa_mask);
  sigaction(SIGALRM, &act, &oact);//注册信号处理函数
  alarm(sec);
  pause();
  sigaction(SIGALRM,&oact,NULL);//恢复之前信号处理的过程
  int ret = alarm(0);//清空闹钟
  return ret;
  
}
int main ()
{
  while(1){
  MySleep(1);
  cout << "wake up" <<endl;
 }
  return 0;
}
