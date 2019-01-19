#include"cp.hpp"
using namespace std;

void* consumer(void * arg)
{
  RingQueue *rqp = (RingQueue*)arg;
  int data;
  for(;;)
  {
    rqp->GetData(data);
    cout<<"Consume data done : "<< data<<endl;
    //sleep(1);
  }
}

void* producter(void* arg)
{
  RingQueue *rqp = (RingQueue*)arg;
  srand((unsigned long)time(NULL));
  for(;;)
  {
    int data = rand()%100;
    rqp->PushDate(data);
    cout<<"Product data done : "<<data<<endl;

  }
}
int main()
{
  pthread_t c, p;
  RingQueue rq;
  pthread_create(&c,NULL,consumer,(void*)&rq);
  pthread_create(&p,NULL,producter,(void*)&rq);

  pthread_join(c,NULL);
  pthread_join(p,NULL);
  return 0;
}
