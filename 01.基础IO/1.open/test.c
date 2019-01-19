#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>
int main ()
{
  umask(0);//创建文件时有文件屏蔽
  int fd = open("myfile.txt",O_WRONLY | O_CREAT,0644);
  if(fd<0){
    printf("open error\n");
    return 1;
  }
  close(fd);
  return 0;
}

