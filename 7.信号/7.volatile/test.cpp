#include<stdio.h>
#include<signal.h>

 int flag = 0;

void handler(int signo)
{
  flag = 1;
  printf("change flag to 1\n");
}
int main ()
{
  signal(2,handler);
  while(!flag);
  printf("proc done...\n");
  return 0;
}
