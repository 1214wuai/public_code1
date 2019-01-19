#include<stdio.h>
#include<sys/ipc.h>
#include<sys/types.h>
#include<sys/shm.h>
#include<unistd.h>
#include<string.h>
int main ()
{
  key_t k = ftok(".",0x1214);
  if(k<0){
    printf("ftok error\n");
    return 1;
  }
  int shmid = shmget(k,4096,IPC_CREAT|IPC_EXCL|0666);
  if(shmid<0){
    printf("shmget error\n");
    return 1;
  }
  
  
  char * buf =(char*)shmat(shmid,NULL,0);
  memset(buf,'\0',4096);
  int i = 0;
  while(i<26){
    buf[i] = 'A'+i;
    i++;
    sleep(1);
  }
  shmdt(buf);
  shmctl(shmid,IPC_RMID,NULL);
  return 0;
}
