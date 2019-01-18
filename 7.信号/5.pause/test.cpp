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
  //act.sa_handler = handler;
  //act.sa_handler = SIG_IGN;
  act.sa_handler = SIG_DFL;
  sigemptyset(&(act.sa_mask));
  act.sa_flags = 0;
  sigaction(2, &act, &oact);
  int ret = pause();
  cout<< ret << endl;
  return 0;
}
