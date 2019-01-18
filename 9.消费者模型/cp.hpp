#ifndef __CP_HPP__
#define __CP_HPP__

#include<iostream>
#include<queue>
#include<pthread.h>
#include<time.h>
#include<stdlib.h>


class BlockQueue{
private:
  std::queue<int> bq;
  int cap;
  pthread_mutex_t lock;
  pthread_cond_t cond_p;//条件变量,队列为空时，需要生产者
  pthread_cond_t cond_c;//队列满了，要消费者来消费
private:
  void LockQueue()
  {
    pthread_mutex_lock(&lock); 
  }
  void UnLockQueue()
  {
    pthread_mutex_unlock(&lock);
  }
  void ProductWait()
  {
    pthread_cond_wait(&cond_p,&lock);
  }
  void ConsumeWait()
  {
    pthread_cond_wait(&cond_c,&lock);
  }
  void NotifyProduct()
  {
    pthread_cond_signal(&cond_p);
  }
  void NotifyComsume()
  {
    pthread_cond_signal(&cond_c);
  }
  bool IsEmpty()
  {
    return (bq.size() ==  0 ? true : false);
  }
  bool IsFull()
  {
    return (bq.size() == 32? true : false);
  }
public:
  BlockQueue(int cap_):cap(cap_)
  {
    pthread_mutex_init(&lock,NULL);
    pthread_cond_init(&cond_p,NULL);
    pthread_cond_init(&cond_c,NULL);
  }
  void PushData(const int& data)
  {
    LockQueue();
    while(IsFull()){
      NotifyComsume();
      std::cout<< "queue full,notify consume data,product stop."<<std::endl;
      ProductWait();
    }
    bq.push(data);
    NotifyComsume();
    UnLockQueue();
  }
  void PopData(int &data)
  {
    LockQueue();
    while(IsEmpty()){
      NotifyProduct();
      std::cout<<"queue empty, notify product data, consume stop"<<std::endl;
      ConsumeWait();
    }
    data = bq.front();
    bq.pop();
    NotifyComsume();
    UnLockQueue();
  }
  ~BlockQueue()
  {
    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&cond_p);
    pthread_cond_destroy(&cond_c);
  }
};


#endif //
