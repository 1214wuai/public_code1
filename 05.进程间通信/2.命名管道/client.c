#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>
#define FIFONAME "mypipe"

int main ()
{
  //客户端不需要再创建管道
  int fd = open(FIFONAME,O_WRONLY);
  if(fd<0){
    return 1;
  }
  char buff[1024];
  while(1)
  {
    printf("Please Enter your Message To Server#");
    fflush(stdout);
    ssize_t s = read(0,buff,sizeof(buff)-1);
    buff[s-1] = 0;
    write(fd,buff,strlen(buff));
  }
  close(fd);
  return 0;
}
