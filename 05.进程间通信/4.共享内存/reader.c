#include<stdio.h>
#include<sys/ipc.h>
#include<sys/types.h>
#include<sys/shm.h>
#include<unistd.h>
int main ()
{
  key_t k = ftok(".",0x1214);
  if(k<0){
    printf("ftok error\n");
    return 1;
  }
  int shmid = shmget(k,4096,IPC_CREAT);
  if(shmid<0){
    printf("shmget error\n");
    return 1;
  }
  char * buff =(char*) shmat(shmid,NULL,0);
  int i = 0;
  while(1){
    printf("%s\n",buff);
    sleep(1); 
  }
  shmdt(buff);

  return 0;

}
