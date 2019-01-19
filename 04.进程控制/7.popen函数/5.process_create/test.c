#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

void process_create(void *func, void *argv)
{
  pid_t pid = fork();
  if(pid == 0)
  {
    ((int (*)())func) (((char**)argv)[0],(char**)argv);
  }
  else{
    int st;
    pid_t ret = wait(&st);
    if(ret == -1)
    {
      perror("wait");
      exit(-1);
    }
  }
  return ;
}

int main()
{
  char *argv[] = {"ls","-l",NULL};
  process_create(execvp,argv);

  return 0;
}
