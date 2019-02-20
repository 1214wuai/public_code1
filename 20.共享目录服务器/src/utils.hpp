#ifndef __M_UTILLS_H__
#define __M_UTILLS_H__

//#include<limits.h>
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
#include<time.h>

#define MAX_PATH 256 
#define LOG(...) do{\
        fprintf(stdout, __VA_ARGS__);\
    }while(0)

#define MAX_HTTPHDR 4096

std::unordered_map<std::string, std::string> g_err_desc = {
	{"200", "OK"},
	{"400", "Bad Request"},
	{"403", "Forbiden"},
	{"404", "NotFound"},
	{"405", "Method "},
	{"413","ka"},
	{"500", "ja "}
}

class Utils{                                                                                     
 public:                                                                                        
static int Split(std::string& src, const std::string &seg, std::vector<std::string> &list)      
{                                                                                                
	int num = 0;                                                                                   
	size_t  idx = 0;                                                                               
	size_t pos = 0;                                                                                
	while(idx < src.length())                                                                      
	{                                                                                              
		pos = src.find(seg, idx);                                                                    
			if(pos == std::string::npos)                                                                 
				break;                                                                                     
			list.push_back(src.substr(idx,pos-idx))                                                      
			num++;                                                                                       
			idx = pos+seg.length();                                                                      
	}                                                                                              
	if(idx < src.length())                                                                         
	{                                                                                              
		list.push_back(src.substr(idx));                                                             
		num++;                                                                                       
	}                                                                                              
	return num;                                                                                    
}

static const std::string GetErrDesc(const std::string &code)
{
  auto it = g_err_desc.find(code);
  if(it == g_err_desc.end())
  {
    return "UNknow";
  }
  return it->second;
}
static void TimeToGMT(time_t t, std::string &gmt)
{
  struct tm *mt = rmtime(t);
  char tmp[128]={0};
  int len;
  len = strftime(tmp,"%a, %d, %b, %Y, %H,:%M:%S GMT", mt);
  gmt.assign(tmp,len);
}

static void DigiToStr(int64_t num, std::string &str)
{
  std::stringstream ss;
  ss<<num;
  str=ss.str();
}

static int64_t StrToDig()
{

}

static void 
};

//包含HttpRequest解析出来的请求信息
class RequestInfo
{
public:
  std::string _method;//请求方法
  std::string _version;//协议版本
  std::string _path_info;//资源路径
  std::string _path_phys;//资源实际路径  
  std::string _query_string;//查询字符串 
  std::unordered_map<std::string, std::string> _hdr_list;//头部当中的键值对  
  struct stat _st; //获取文件信息 
public:
  std::string _err_code;
public:
  void SetError(const std::string& code)
  {
    _err_code = code;
  }

  bool RequestIsCGI()
  {
    if((_method == "GET" && !_query_string.empty()) || (_method == "POST"))
    {
      return true;
    }
    return false;
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

  //解析首行
  bool PathIsLegal(std::string &path, RequestInfo &info)
  {
    std::string file = "WWWROOT" +path;
    //stat函数，通过路径获取文件信息
    if(stat(path.c_str(), (&info)->_st) < 0)
    {
      info._err_code = "404";
      return false;
    }
    char tmp[MAX_PATH] = {0};
    realpath(path.c_str(), tmp);
   info._path_phys = tmp;//物理路径
    if(info._path_phys.find("WWWROOT") == std::string::npos)
    {
      info._err_code = "403";
      return false;
    }
    //如果路径不再WWW下面

    return true;
  }

  //解析首行
  bool ParseFirstLine(std::string &line, RequestInfo &info)
  {
    std::vector<std::string> line_list;
    if(Utils::Split(_http_header, " ", line_list) != 3)
    {
      info._err_code = "400";
      return false;
    }
    std::string url;
    info._method = line_list[0];
    url = line_list[1];
    info._version = line_list[2];
    if(info._method != "GET" && info._method != "POST" && info._method != "HEAD")
    {
      info._err_code = "405";
      return false;
    }
    if(info._version != "HTTP/0.9" && info._version != "HTTP/1.0" && info._version != "HTTP/1.0")
    {
      info._err_code = "400";
      return false;
    }

    size_t pos;
    pos=url.find("?");
    if(pos == std::string::npos)
    {
      info._path_info = url;
    }else{
      info._path_info = url.substr(0,pos);
      info._query_string = url.substr(pos + 1);
        //realpath函数，将相对路径转换成绝对路径，发生错误就是段错误
    }

    return PathIsLegal(info._path_info, info);
  }

  //解析http请求头
  bool ParseHttpHeader(RequestInfo &info)
  {
    //http请求头解析
    //请求方法 URL 协议版本
    //key:val\r\nkey:val
    //解析首行，其余大放到哈希表里
    std::vector<std::string> hdr_list;
    Utils:: Split(_http_header, "\r\n",hdr_list);//分割
    if(ParseFirstLine(hdr_list[0],info) == false)
    {
      return false;
    }
    hdr_list.erase(hdr_list.begin());
    for (size_t i = 0; i<hdr_list.size(); i++)
    {
      size_t pos = hdr_list[i].find(": ");
      info._hdr_list[hdr_list[i].substr(pos)] = hdr_list[i].substr(pos+2);
    }

    for(auto it=info._hdr_list.begin();it!=info._hdr_list.end();it++)
    {
      std::cout<<"["<<it->first<<"]=["<<it->second<<std::endl;
    }
   // for(size_t i = 0; i<hdr_list.size(); i++)
   // {
   //   std::cot<<hdr_list[i]<<std::endl;
   // }
    return true;
  }

  
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
