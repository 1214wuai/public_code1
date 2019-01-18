#include<iostream>
#include<pthread.h>
#include<unistd.h>

using std::cout;
using std::endl;
pthread_mutex_t mutex;
static int ticket = 100;
void* thread_routine(void* arg)
{
  const char* thread_name = (char*)arg;
  while(1)
  {
    pthread_mutex_lock(&mutex);
    if(ticket>0)
    {
      usleep(1000);
      ticket--;
      cout<<thread_name<<" : "<<ticket<<endl;
      pthread_mutex_unlock(&mutex);
      sched_yield();
    }
    else{
      pthread_mutex_unlock(&mutex);
      break;
    }
  }
}
int main()
{
  pthread_t tid, tid1, tid2, tid3;
  //pthread_mutex_t mutex;
  pthread_create(&tid, NULL,thread_routine,(void*)"thread1");
  pthread_create(&tid1, NULL,thread_routine,(void*)"thread2");
  pthread_create(&tid2, NULL,thread_routine,(void*)"thread3");
  pthread_create(&tid3, NULL,thread_routine,(void*)"thread4");
  pthread_join(tid,NULL);
  pthread_join(tid1,NULL);
  pthread_join(tid2,NULL);
  pthread_join(tid3,NULL);
  pthread_mutex_destroy(&mutex);
  return 0;
}
