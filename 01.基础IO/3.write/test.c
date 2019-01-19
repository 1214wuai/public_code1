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
  int i = 0;
  const char *msg = "Hello bit\n";
  while(i < 10){
    write(fd,msg,strlen(msg));//strlen()函数在C语言中跟linux中的值不同（在linux中少1）
    i++;
  }
  close(fd);
  return 0;
}

