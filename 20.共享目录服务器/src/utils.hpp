#ifndef __M_UTILLS_H__
#define __M_UTILLS_H__

#include <iostream>
#include <string.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/stat.h>

#define LOG(...) do{\
        fprintf(stdout, __VA_ARGS__);\
    }while(0)

#define MAX_HTTPHDR 4096

//包含HttpRequest解析出来的请求信息
class RequestInfo
{
public:
  //请求方法
  std::string _method;
  //协议版本
  std::string _version;
  //资源路径
  std::string _path_info;
  //资源实际路径
  std::string _path_phys;
  //查询字符串
  std::string _query_string;
  //头部当中的键值对
  std::unordered_map<std::string, std::string> _hdr_list;
  //获取文件信息
  struct stat _st;
public:
  std::string _err_code;
public:
  void SetError(const std::string& code)
  {
    _err_code = code;
  }

  bool RequestIsCGI()
  {
    return true;
  }
};

//http数据的接收接口
//http数据的解析接口
//对外提供能够获取处理结构的接口
class HttpRequest
{
private:
  int _cli_sock;
  std::string _http_header;
  RequestInfo _req_info;

public:
  HttpRequest(int sock)
    : _cli_sock(sock)
  {}

  //接收http请求头
  bool RecvHttpHeader(RequestInfo& info)
  {
    //定义一个设置http头部最大值
    char tmp[MAX_HTTPHDR];
    while (1)
    {
      //预先读取，不从缓存区中把数据拿出来
      int ret = recv(_cli_sock, tmp, MAX_HTTPHDR, MSG_PEEK);
      //读取出错，或者对端关闭连接
      if (ret <= 0)
      {
        //EINTR表示这次操作被信号打断，EAGAIN表示当前缓存区没有数据
        if (errno == EINTR || errno == EAGAIN)
        {
          continue;
        }
        info.SetError("500");
        return false;
      }
      //ptr为NULL表示tmp里面没有\r\n\r\n
      char* ptr = strstr(tmp, "\r\n\r\n");
      //当读了MAX_HTTPHDR这么多的字节，但是还是没有把头部读完，说明头部过长了
      if ((ptr == NULL) && (ret == MAX_HTTPHDR))
      {
        info.SetError("413");
        return false;
      }
      //当读的字节小于这么多，并且没有空行出现，说明数据还没有从发送端发送完毕，所以接收缓存区，需要等待一下再次读取数据
      else if ((ptr == NULL) && (ret < MAX_HTTPHDR))
      {
        usleep(1000);
        continue;
      }

      int hdr_len = ptr - tmp;
      _http_header.assign(tmp, hdr_len);
      recv(_cli_sock, tmp, hdr_len + 4, 0);
      LOG("header:%s\n", tmp);
      break;
    }

    return true;
  }

  //解析http请求头
  bool ParseHttpHeader(RequestInfo& info);
  //向外提供解析结果
  RequestInfo& GetRequestInfo(); 
};

//文件请求接口(完成文件下载/列表功能)接口
//CGI请求接口
class HttpResponse
{
private:
  int _cli_sock;
  //表明这个文件是否修改过
  std::string _etag;
  //最后一次修改时间
  std::string _mtime;
  //文件长度
  std::string _cont_len;

public:
  HttpResponse(int sock)
    : _cli_sock(sock)
  {}

  //初始化的一些请求响应信息
  bool InitResponse(RequestInfo req_info);
  //文件下载功能
  bool ProcessFile(RequestInfo& info);
  //文件列表功能
  bool ProcessList(RequestInfo& info);
  //cgi请求的处理
  bool ProcessCGI(RequestInfo& info);
  //处理出错响应
  bool ErrHandler(RequestInfo& info);
  bool CGIHandler(RequestInfo& info)
  {
    //初始化CGI信息
    InitResponse(info);
    //执行CGI响应
    ProcessCGI(info);
    return true;
  }

  bool FileHandler(RequestInfo& info)
  {
    //初始化文件响应信息
    InitResponse(info);
    //执行文件列表展示响应
   // if (DIR)
   // {
   //   ProcessList();
   // }
   // //执行文件下载响应
   // else 
   // {
   //   ProcessFile(info);
   // }
    return true;
  }
};

#endif //__M_UTILLS_H__

