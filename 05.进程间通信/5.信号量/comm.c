#include"comm.h"
static int commSemSet(int nums,int flags)
{
  key_t key = ftok(PATHNAME,PROJ_ID);
  if(key<0){
    perror("ftok");
    return -1;
  }
  int semid = semget(key,nums,flags);
  if(semid<0){
    perror("semget");
    return -2;
  }
  return semid;
}
 int createSemSet(int nums)
{
  return commSemSet(nums,IPC_CREAT|IPC_EXCL|0666);
}

int initSem(int semid, int nums, int initval)
{
  union semun _un;
  _un.val = initval;
  if(semctl(semid,nums,SETVAL,_un)<0){
    perror("semctl");
    printf("初始化错误\n");
    return 0;
  }
  return 0;
}
int getSemSet(int nums)
{
  return commSemSet(nums,IPC_CREAT);
}
static int commPV(int semid,int who,int op)
{
  struct sembuf _sf;
  _sf.sem_num = who;
  _sf.sem_op = op;
  _sf.sem_flg = 0;
  if(semop(semid,&_sf, 1) < 0){
    perror("semcop");
    printf("PV错误\n");
    return -1;
  }
  return 0;
}
int P(int semid, int who)
{
  return commPV(semid,who,-1);
}
int V(int semid, int who)
{
  return commPV(semid,who,1);
}
int destroySemSet(int semid)
{
  if(semctl(semid,0,IPC_RMID)<0){
    perror("semctl");
    printf("删除错误\n");
    return -1;
  }
}

