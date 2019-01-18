#ifndef __MPOOL_H__
#define __MPOOL_H__

#include<iostream>
#include<queue>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>

#define MAX_THREAD 3
#define MAX_QUEUE 10
class MyTask{
  private:
    int data;
  public:
    MyTask(){}
    ~MyTask(){}
    void SetData(int _data)
    {
      data = _data;
    }

    void Run()
    {
      int t = rand()%5;
      printf("thread:%lu sleep %d sec, execute data:%d\n", pthread_self(), t, data);
      sleep(t);
    }
};

class MyThreadPool{
  private:
    int max_thread;
    int max_queue;
    int cur_thread;
    int keep_running;
    
    pthread_t *tid;
    std::queue<MyTask*> task_list;
    pthread_mutex_t mutex;
    pthread_cond_t empty;
    pthread_cond_t full;
  private:
    void ThreadLock()
    {
      pthread_mutex_lock(&mutex);
    }
    void ThreadUnlock()
    {
      pthread_mutex_unlock(&mutex);
    }
    void ConsumerWait()
    {
      pthread_cond_wait(&empty,&mutex);
    }
    void ConsumerNotify()
    {
      pthread_cond_signal(&empty);
    }
    void ProductWait()
    {
      pthread_cond_wait(&full,&mutex);
    }
    void ProductNotify()
    {
      pthread_cond_signal(&full);
    }
    bool ThreadIsRunning()
    {
      return (keep_running == 0 ? false:true);
    }
    void ThreadExit()
    {
      cur_thread--;
      printf("thread:%lu exit\n", pthread_self());
      ProductNotify();
      pthread_exit(NULL);
    }
    bool QueueEmpty()
    {
      return (task_list.size() == 0 ? true :false);
    }
    bool QueueFull()
    {
      return (task_list.size() == max_queue  ? true : false);
    }
    void PopTask(MyTask **task)
    {
      *task = task_list.front();
      task_list.pop();
      return;
    }
    void PushTask(MyTask *task)
    {
      task_list.push(task);
    }

    static void *thread_routine(void* arg)
    {
      MyThreadPool *pthreadpool = (MyThreadPool*)arg;
      while(1)
      {
        pthreadpool->ThreadLock();
        while(pthreadpool->QueueEmpty() && pthreadpool->ThreadIsRunning()){
          pthreadpool->ConsumerWait();
        }
        if(!pthreadpool->ThreadIsRunning() && pthreadpool->QueueEmpty()){
          pthreadpool->ThreadUnlock();
          pthreadpool->ThreadExit();
        }
        MyTask *task;
        pthreadpool->PopTask(&task);
        pthreadpool->ProductNotify();
        pthreadpool->ThreadUnlock();
        task->Run();
      }
      return NULL;
    }
  public:
    MyThreadPool(int _max_thread = MAX_THREAD, int _max_queue = MAX_QUEUE)
      :max_thread(_max_thread)
       ,max_queue(_max_queue)
       ,cur_thread(_max_thread)
       ,keep_running(1)
    {
     // int i = 0;
      printf("create thread:%d-%d\n", max_thread,max_queue);
      tid = (pthread_t*)malloc(sizeof(pthread_t)*_max_thread);
      pthread_mutex_init(&mutex,NULL);
      pthread_cond_init(&full,NULL);
      pthread_cond_init(&empty,NULL);
      for(int i = 0; i <_max_thread; i++){
        pthread_create(&tid[i],NULL,thread_routine, (void*)this);
        pthread_detach(tid[i]);
      }
  }

  bool AddTaskToPool(MyTask *task)
  {
    ThreadLock();
    while(QueueFull()){
      ProductWait();
    }
    PushTask(task);
    printf("add task to pool\n");
    ConsumerNotify();
    return true;
  }
  void StopThreadPool()
  {
    if(keep_running == 0)
    {
      return;
    }
    ThreadLock();
    keep_running = 0;
    while(cur_thread > 0)
    {
      ProductWait();
    }
    ThreadUnlock();
  }
  ~MyThreadPool()
  {
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&full);
    pthread_cond_destroy(&empty);
  }
};


#endif
