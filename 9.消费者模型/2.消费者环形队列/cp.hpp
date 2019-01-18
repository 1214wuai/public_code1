#include<iostream>
#include<vector>
#include<stdlib.h>
#include<semaphore.h>
#include<pthread.h>

#define NUM 16
class RingQueue{
  private:
    std::vector<int> q;
    int cap;
    sem_t data_sem;
    sem_t space_sem;
    int consume_step;
    int product_step;
  public:
    RingQueue(int _cap = NUM)
      :q(_cap)
       ,cap(_cap)
    {
      sem_init(&data_sem, 0, 0);
      sem_init(&space_sem, 0 ,cap);
      consume_step = 0;
      product_step = 0;
    }
    void PushDate(const int &data)
    {
      sem_wait(&space_sem);//P
      q[consume_step] = data;
      consume_step++;
      consume_step %=cap;
      sem_post(&data_sem);
    }
    void GetData(int &data)
    {
      sem_wait(&data_sem);
      data = q[product_step];
      product_step++;
      product_step %= cap;
      sem_post(&space_sem);
    }
    ~RingQueue()
    {
      sem_destroy(&data_sem);
      sem_destroy(&space_sem);
    }
};

