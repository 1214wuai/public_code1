#ifndef _COMM_H_
#define _COMM_H_ 

#include<stdio.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<string.h>
#include<unistd.h>
#define PATHNAME "."
#define PROJID 0x1214

#define SERVER_TYPE 1
#define CLIENT_TYPE 2

struct msgbuf{
  long mtype;
  char mtext[64];
};
//struct msgbuf{
// long mtyte;
//  char mtext[64];
//};

int CreatMsgQueue();
int OpenMsgQueue();
void DestroyMsgQueue(int msgid);
void SenMsg(int msgid, char msg[], int types);
void RecvMsg(int msgid, char msg[], int type);
#endif 
