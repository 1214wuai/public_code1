#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<fcntl.h>

#define FIFONAME "mypipe"

int main ()
{
  mkfifo(FIFONAME, 0644);//创建好管道文件
  int fd = open(FIFONAME,O_RDONLY);
  if(fd<0){
    return 1;
  }
  char buff[1024];
  while(1)
  {
    ssize_t s = read(fd,buff,sizeof(buff)-1);
    if(s>0){
      buff[s] = 0;
      printf("client# %s\n",buff);
    }else if(s==0){//s==0代表写端不写，并且关闭的文件描述符，此时读端读到文件末尾
      printf("client quit!server quit too \n");
      break;
      
    }else{
      break;
    }
  }
  close(fd);
  return 0;
}
