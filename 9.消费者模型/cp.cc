#include"cp.hpp"
const int num = 32;//队列的容量
using namespace std;

void* consume_routine(void* arg)
{
  BlockQueue *q = (BlockQueue*)arg;
  int data;
  for(;;){
    q->PopData(data);
    cout<<"comsume done, data is:"<<data<<endl;
  }
}

void* product_routine(void* arg)
{
  BlockQueue *q = (BlockQueue*)arg;
  srand((unsigned long)time(NULL));
  for(;;){
    int data = rand()%100+1;
    q->PushData(data);    
    cout<<"product done, data is: "<<data<<endl;
  }
}
int main()
{
  BlockQueue q = BlockQueue(num);
  pthread_t c, p;
  pthread_create(&c,NULL,consume_routine, (void*)&q);
  pthread_create(&p,NULL,product_routine, (void*)&q);

  pthread_join(c,NULL);
  pthread_join(p,NULL);
  return 0;
}
