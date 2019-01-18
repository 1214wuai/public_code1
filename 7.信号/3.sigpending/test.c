#include<stdio.h>
#include<signal.h>
#include<unistd.h>

void printsigset(sigset_t *set)
{
  int i = 1;
  for(;i<=31;i++){
    if(sigismember(set,i)){//判断指定信号是否在目标集和中
        putchar('1');
    }else{
        putchar('0');
    }
  }
  puts("");
}

int main ()
{
  sigset_t s,p;
  sigemptyset(&s);//定义信号集对象，并清空初始化
  sigaddset(&s,SIGINT);//添加2号信号
  sigprocmask(SIG_BLOCK,&s,NULL);//设置阻塞信号集，阻塞二号信号
  while(1){
    sigpending(&p);//获取未决信号
    printsigset(&p);
    sleep(1);
  }
  return 0;
}
