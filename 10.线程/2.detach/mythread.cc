#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>

void* thread_run(void* arg)
{
  pthread_detach(pthread_self());
  printf("%s\n", (char*)arg);
  sleep(5);
  return (void*)123;
}
int main()
{
  pthread_t tid ;
  if (pthread_create(&tid,NULL,thread_run,(void*)"thread1")!=0){
    return 1;
  }
  void* ret;
  int x = 0;
  sleep(1);
  pthread_cancel(tid);
  x = pthread_join(tid,&ret);
  if(x==0){
    printf("pthread wait success\n");
  }else{
    printf("pthread wait faiiled\n");
  }
  printf("%d",(int)ret);
  return x;
}
