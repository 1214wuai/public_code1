#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>
int main ()
{
 // umask(0);//创建文件时有文件屏蔽
  int fd = open("myfile.txt",O_RDONLY);//此时的file是write过的，里面存有数据（10行Hello bit）
  if(fd<0){
    printf("open error\n");
    return 1;
  }
  char buf[1024];
  ssize_t s = read(fd,buf,sizeof(buf)-1);//buf在C语言中以字符串形式呈现，必须以\0结尾
  if(s > 0){
    buf[s] =  0;//加上\0
    printf("%s",buf);
  }
  close(fd);
  return 0;
}
