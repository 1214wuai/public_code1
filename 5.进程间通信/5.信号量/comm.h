#ifndef _COMM_H_
#define _COMM_H_ 
#include<stdio.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/sem.h>
#include<unistd.h>
#define PATHNAME "."
#define PROJ_ID 0x666
union semun{
  int val;
  struct semid_ds *buf;
  unsigned short *array;
  struct seminfo *_buf;
};

int createSemSet(int nums);
int initSem(int semid, int nums, int initval);
int getSemSet(int nums);
int P(int semid, int who);
int V(int semid, int who);
int destroySemSet(int semid);

#endif
