#include"mpool.hpp"

#define NUM 5

int add(int x,int y)
{
  return x+y;
}
int main()
{

  ThreadPool *tp = new ThreadPool(NUM);
  
  tp->InitThreadPool();
  int count = 1;
  for(;;)
  {
    sleep(1);
    Task t(count, count-1,add);
    tp->AddTask(t);
    count++;
  }
  return 0;
}
