#include"comm.h"
int main ()
{
  int msgid = CreatMsgQueue();
  char buf[1024];                                                   
  while(1){                                                       
    buf[0] = 0;                                                   
    RecvMsg(msgid,buf,CLIENT_TYPE);                         
    printf("client# %s\n");                                         
    printf("Please Enter# ");                           
    fflush(stdout);                                       
    ssize_t s = read(0,buf,sizeof(buf));                    
    if(s>0){                                                  
        buf[s-1] = 0;                                                    
        SenMsg(msgid, buf, SERVER_TYPE);
        printf("send done, wait recv...\n");
      }                                  
    }      
  DestroyMsgQueue(msgid);
  return 0;
}
