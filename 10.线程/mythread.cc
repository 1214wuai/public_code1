#include<iostream>
#include<sys/types.h>
#include<unistd.h>
#include<pthread.h>

using namespace std;

void* thread_routine(void* arg)
{
  const char* thread_name = (const char*)arg;
  cout<<thread_name<<"running... pid: "<<getpid()<<"my thread id is: "<<pthread_self()<<endl;
  sleep(5);
  return (void*)123;
}
int main()
{
  pthread_t tid;
  pthread_create(&tid, NULL, thread_routine,(void*)"thread 1");
  cout<<"main thread running... pid: "<<getpid()<<"tid: "<< tid<<endl;
  sleep(3);
  pthread_detach(tid);
  pthread_cancel(tid);
  void* ret;
  int x = 0;
  x=pthread_join(tid,&ret);
  cout<<"new thread quit, code is: "<<(int)ret<<endl;
  cout<<x<<endl;
}
