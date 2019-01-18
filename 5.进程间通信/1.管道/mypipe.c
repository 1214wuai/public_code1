#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<stdlib.h>
int main ()
{
  int fds[2];
  if(pipe(fds)<0){
    printf("pipe error\n");
    return 1;
  }
  //printf("fds[0]: %d, fds[1]: %d\n", fds[0], fds[1]);
  pid_t id = fork();
  if(id==0)
  {
    //child  w
    close(fds[0]);
    const char *str = "I am a student \n";
    int i = 0;
    while(1)
    {
      write(fds[1],str,strlen(str));
      printf("child write done! : %d\n",i++);
      //sleep(1);
    }
    close(fds[1]);
    exit(0);
  }else{
    //parent  r
    close(fds[1]);
    sleep(3);
    close(fds[0]);
    char buff[1024];
    int status; 
    waitpid(id,&status,0);
    printf("%d\n",status&0x7F);
    return 0;
  }
}
