#include"Epoll.h"

//设置非阻塞
void SetNonblocking(int sfd)
{
  int flags,s;
  flags = fcntl(sfd, F_GETFL, 0);
  if(flags == -1)
  {
    ErrorLog("SetNonblocking:F_GETFL");
  }
  flags |= O_NONBLOCK;
  s = fcntl(sfd, F_SETFL, flags);
  if(s == -1)
  {
    ErrorLog("SetNonblocking:F_SETFL");
  }
}

//操作event事件
void OpEvent(int fd, int events, int how, int line)
{
  struct epoll_event event;
  event.events = events;
  event.data.fd = fd;
  if(epoll_ctl(_events, how,fd,&event) == -1)
  {
    ErrorLog("epoll_ctl.fd:%d.line:%d", fd, how, line);
  }
}

void EpollServer::Start()
{
  _listenfd = socket(AF_INET, SOCK_STREAM, 0);
  if(_listenfd < 0)
  {
    ErrorLog("Create socket");
    return;
  }

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(_port);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  if(bind(_listenfd, (struct sockaddr*)&addr, sizeof(addr))< 0)
  {
    ErrorLog("bind");
    return;
  }
  if(listen(_listenfd, 100000) < 0)
  {
    ErrorLog("listen");
    return;
  }

  //开始事件循环
  
  _eventfd = epoll_create(_MAX_EVENT);
  if(_eventfd == -1)
  {
    ErrorLog("epoll_create");
    return;
  }

  //struct epoll_event event;
  //event.events = EPOLLIN;
  //event.data.fd = _eventfd;
  //if(epoll_ctl(_eventfd, EPOLL_CTL_ADD, _listenfd, EPOLLIN) == -1)
  //{
  //  ErrorLog("EPOLL_CTL_ADD listenfd EPOLLIN");
  //  return;
  //}
  EventLoop();
}

void EpollServer::EventLoop()
{
  //添加_listenfd事件
  SetNonblocking(_listenfd);
  OpEvent(_listenfd, EPOLLIN, EPOLL_CTL_ADD, __LINE__);

  //事件列表
  struct epoll_event events[_MAX_EVENT];
  while(1)
  {
    int size = epoll_wait(_eventfd, events,sizeof(events),-1);
    if(size == -1)
    {
      ErrorLog("epoll_wait");
      break;
    }

    int i = 0;
    for(i = 0; i<size; ++i)
    {
      if(events[i].data.fd == _listenfd)//监听到,有事件过来
      {
        struct sockaddr_in client;
        socklen_t len = sizeof(client);
        int connectfd = accept(_listenfd, (struct sockaddr*)&client, &len); 
        if(connectfd < 0)
        {
          ErrorLog("accept");
          continue;
        }
        TraceLog("client connect");
        ConnectEventHandle(connectfd);
      }
      else if(events[i].events & (EPOLLIN | EPOLLPRI | EPOLLDHUP))//读事件，独立的整形，1248....events里用一个位来标识，非0就是真
      {
        ReadEventHAndle(events[i].data.fd);
      }
      else if(events[i].eents & EPOLLOUT)//写事件
      {
        WriteEventHandle(events[i].data.fd);
      }
      else
      {
        ErrorLog("events error");
      }
    }
  }
}
