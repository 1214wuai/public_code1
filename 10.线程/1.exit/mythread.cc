#include<iostream>
#include<sys/types.h>
#include<unistd.h>
#include<pthread.h>
#include<sys/syscall.h>

using namespace std;

void* thread1(void* arg)
{
  const char* thread_name = (const char*)arg;
  pid_t tid;
  tid = syscall(SYS_gettid);
  while(1){
    sleep(1);
    cout<<thread_name<<"running... pid: "<<getpid()<<"my thread id is: "<<pthread_self()<<"tid:"<<tid<<endl;
    //sleep(1);
  }
  return (void*)123;
}


void* thread2(void* arg)
{
  const char* thread_name = (const char*)arg;
  while(1){
    sleep(1);
    cout<<thread_name<<"running... pid: "<<getpid()<<"my thread id is: "<<pthread_self()<<endl;
    //sleep(1);
  }
  pthread_exit((void*)456);
}

int main()
{
  pthread_t tid;
  pthread_t _tid;
  pthread_create(&tid, NULL, thread1,(void*)"thread 1");
  pthread_create(&_tid, NULL, thread2,(void*)"thread 2");
  cout<<"main thread running... pid: "<<getpid()<<"tid: "<< tid<<"_tid"<<_tid<<endl;
  pid_t id;
  id = syscall(SYS_gettid);
  cout<<"tid:"<<endl;
  sleep(30);
  pthread_detach(tid);
  pthread_cancel(tid);

  int ret = pthread_join(tid, NULL);
  cout<<"new thread quit, code is: "<<ret<<endl;
}
