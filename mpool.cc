#include"mpool.hpp"
 int main()
{
  MyTask task[10];
  int i;
  MyThreadPool pool;
  for(i = 0;i<10;i++)

  {
    task[i].SetData(i);
    pool.AddTaskToPool(&task[i]);
  }
  pool.StopThreadPool();
  return 0;
}
