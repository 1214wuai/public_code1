#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
int main()
{
  int status = 0;
  status = system("ls -la");
  if(status == -1){
    perror("system()");
      exit(1);
  }
  if(WIFEXITED(status) != 0)//正常退出
  {
    if(WEXITSTATUS(status) == 0)//操作正确
      printf("run command success\n");
    else{//操作错误
      printf("run command fail and exit code is %d\n",WEXITSTATUS(status));
    }
  }
  else{
    printf("exit code is %d\n",WEXITSTATUS(status));
  }
  return 0;
}
