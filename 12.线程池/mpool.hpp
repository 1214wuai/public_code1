#ifndef __MPOOL_H__
#define __MPOOL_H__
#include<unistd.h>
#include<pthread.h>
#include<iostream>
#include<queue>
using namespace std;

typedef int (*cal_t)(int,int);//函数指针，参数是两个int，返回值是int
class Task{
  private:
    int number;
    int x;
    int y;
    cal_t handerTask;
  public:
    Task(int x_, int y_,cal_t handerTask_):x(x_),y(y_),handerTask(handerTask_)
  {}
    int Run()
    {
      return handerTask(x,y);
    }
    //回调函数
    ~Task()
    {}

};

class ThreadPool{
  private:
    int thread_nums;
    int idles;//闲置线程数目
    std::queue<Task> t_queue;//任务队列,临界资源
    pthread_mutex_t lock;
    pthread_cond_t cond;
  private:
    static void *thread_routine(void* arg)
    {
      ThreadPool *tp = (ThreadPool*)arg;//把当前对象传进来
      for(;;)
      {
        tp->LockQueue();
        while(tp->IsEmpty())
        {
          tp->IdleThread();//任务队列为空时，线程挂起
        }
        //此时线程被唤醒，有任务了
        Task t = tp->GetTask();//默认构造
        //处理任务之前释放锁
        tp->UnLockQueue();
        int ret = t.Run();
        cout<<"thread "<<pthread_self()<<" Result is: "<< ret<<endl;
      }
    }
    bool is_stop;
    void NotifyOneThread()
    {
      pthread_cond_signal(&cond);
    }
    void NotifyAllThread()
    {
      pthread_cond_broadcast(&cond);
    }
  public:
    ThreadPool(int num_):thread_nums(num_)
    {}
    void InitThreadPool()
    {
      pthread_mutex_init(&lock,NULL);
      pthread_cond_init(&cond,NULL);
      int i = 0;
      for(;i<thread_nums; i++)
      {
        pthread_t tid;
        pthread_create(&tid,NULL,thread_routine, (void*)this);
      }
    }
     void LockQueue()
    {
      pthread_mutex_lock(&lock);
    }
    void UnLockQueue()
    {
      pthread_mutex_unlock(&lock);
    }
    bool IsEmpty()
    {
      return t_queue.size()==0?true:false;
    }
    void IdleThread()
    {
      if(is_stop)
      {
        UnLockQueue();
        thread_nums--;
        pthread_exit((void*)0);
        cout<<"pthread: "<<pthread_self()<<" quit"<<endl;
        return;
      }
      pthread_cond_wait(&cond,&lock);
    }
    void AddTask(Task &t)
    {
      if(is_stop){
        UnLockQueue();
        return;//如果要退出，不再添加新任务，处理完原来的任务就退出
      }
      LockQueue();
      t_queue.push(t);
      NotifyOneThread();
      UnLockQueue();
    }
    Task GetTask()
    {
      Task t = t_queue.front();
      t_queue.pop();
      return t;

    }
    void Stop()
    {
      LockQueue();
      is_stop = true;
      UnLockQueue();
      while(thread_nums>0)
      {
        NotifyAllThread();
      }
    }
    ~ThreadPool()
    {
      pthread_mutex_destroy(&lock);
      pthread_cond_destroy(&cond);
    }
};

#endif
