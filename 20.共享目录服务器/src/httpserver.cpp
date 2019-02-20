
#include "utils.hpp"
#include "threadpool.hpp"
#include <stdlib.h>

#define MAX_LISTEN 5
#define MAX_THREAD 5


//建立一个tcp服务器端程序，接收新连接
//为新连接组织一个线程池任务，添加到新线程池中
class HttpServer
{
private:
  int _serv_sock;
  //线程池对象，可以直接new一个线程池，从而来设置线程池的线程数
  ThreadPool* _tp;

private:
  //http任务的处理函数
  static bool HttpHandler(int sock)
  {
    RequestInfo info;
    HttpRequest req(sock);
    HttpResponse rsp(sock);

    //for ( ; ; )
    //{
    //  
    //  //这里对于服务器与客户端进行通信的时候，只需要一次就好了，所以就不需要进行死循环
    //  char buf[10240];
    //  memset(buf, 0, sizeof(buf));
    //  if (recv(sock, buf, sizeof(buf), 0) < 0)
    //  {
    //    std::cerr << "recv error!" << std::endl;
    //  }
    //  std::cout << "requst: " << buf << std::endl;
    //  
    //  char ouput_buf[1024];
    //  memset(ouput_buf, 0, sizeof(ouput_buf));
    //  const char* hello = "<h1>hello world</h1>";
    //  sprintf(ouput_buf, "HTTP/1.0 302 REDIRECT\nContent-Length:%lu\nLocation:https://www.taobao.com\n\n%s", strlen(hello), hello);
    //  send(sock, ouput_buf, sizeof(ouput_buf), 0);
    //}
    req.RecvHttpHeader(info);
    //接收http头部
    if (req.RecvHttpHeader(info) == false)
    {
      goto out;
    }
    //解析http头部
    if (req.ParseHttpHeader(info) == false)
    {
      goto out;
    }
    ////判断请求是否是CGI请求
    if (info.RequestIsCGI())
    {
      //若当前请求类型是CGI请求，则执行CGI响应
    //  rsp.CGIHandler(info);
    }
    //else 
    //{
    //  //若当前请求类型不是CGI请求，则执行文件列表或文件下载响应
    //  rsp.FileHandler(info);
    //}
    
    close(sock);
    return true;
out:
    rsp.ErrHandler(info);
    close(sock);
    return false;
  }

public:
  HttpServer()
    : _serv_sock(-1)
    , _tp(NULL)
  {}

  //tcp服务器socket的初始化，以及线程的初始化
  bool HttpServerInit(std::string ip, std::string port)
  {
    _serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    if (_serv_sock < 0)
    {
      LOG("sock error:%s\n", strerror(errno));
      return false;
    }
    int opt = 1;
    setsockopt(_serv_sock, SOL_SOCKET,SO_REUSEADDR);

    sockaddr_in lst_addr;
    lst_addr.sin_family = AF_INET;
    lst_addr.sin_port = htons(atoi(port.c_str()));
    lst_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    //绑定这个局域网的所有地址
    //lst_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    socklen_t len = sizeof(sockaddr_in);
    
    if ((bind(_serv_sock, (sockaddr*)&lst_addr, len) < 0))
    {
      LOG("bind error :%s\n", strerror(errno));
      close(_serv_sock);
      return false;
    }

    if (listen(_serv_sock, MAX_LISTEN) < 0)
    {
      LOG("listen errno :%s\n", strerror(errno));
      close(_serv_sock);
      return false;
    }

    _tp = new ThreadPool(MAX_THREAD);
    if (_tp == NULL)
    {
      LOG("thread pool malloc error!!\n");
      return false;
    }

    if (_tp->ThreadPoolInit() == false)
    {
      LOG("thread pool init error!!\n");
      return false;
    }
    return true;
  }

  //开始获取客户端新连接--创建任务--任务入队
  bool Start()
  {
      while (1)
      {
        sockaddr_in cli_addr;
        socklen_t len = sizeof(sockaddr_in);
        int new_sock = accept(_serv_sock, (sockaddr*)&cli_addr, &len);
        if (new_sock < 0)
        {
          LOG("accept error :%s\n", strerror(errno));
          return false;
        }
        std::cout << "new connect!" << std::endl;
        HttpTask ht;
        ht.SetHttpTask(new_sock, HttpHandler);
        _tp->PushTask(ht);
      }

      return true;
  }
};

class HttpResponse
{
    //文件请求（完成文件下载/列表功能）接口
    //CGI请求接口
    private:
        int _cli_sock;
        //ETag: "inode-fsize-mtime"\r\n每个文件的inode都是唯一的,文件大小，最后一次修改的时间
        std::string _etag;//是否被修改
        std::string _mtime;//最后一次修改时间
        std::string _date; //系统响应时间
        //std::string _cont_len;//客户端请求正文大小
    public:
		HttpResponse(int sock):_cli_sock(sock){}
        bool InitResponse(RequestInfo req_info)//初始化一些请求的响应信息 
        {
          //req_info.st.st_size;
          //req_info.st.st_ino;
          //req_info.st.st_mtime;
          
          //Last-Modified:
          Utils::DigitToStr(req_info._st._st_mtime,_mtime);
          
          //ETag:
          Utils::MakeETag(int64_t size, int64_t ino, int64_t mtime);
          
          //Date:
          time_t t = time(NULL);
          Utils::;TimeToGMT(t,_date);
          return true;
        }

        //发送头部
        bool SendData(std::string& buf)
        {
          
          if(send(_cli_sock, buf.c_str(), buf.length(), 0) < 0)
          {
            return false;
          }
          return true;
        }

        //发送正文
        bool SendCData(std::string& buf)
        {
          if(buf.empty())
          {
            return SendData("0\r\n");
          }

          std::stringstream ss;
          ss<<std::hex<<buf.length()<< "\r\n";
          
          SendData(ss.c_str());
          ss.clear();
          
          SendData(buf);
          SendData("\r\n");

          return true;
        }
        bool ProcessFile(std::string &file)//文件下载功能
        {

          return true;
        }
        bool ProcessList(std::string &path)//文件列表功能
        {
          //组织头部
          //首行
          //Content_Type：text/html\r\n
          //ETag: \r\n
          //Date: \r\n
          //Transfer-Encoding: chunked\r\n 块传输
          //Connection: close\r\n\r\n
          
          //正文：
          //每一个目录的文件都要组织一个html标签信息
          std::string rsp_header;

          rsp_header = info._version + " 200 OK\r\n";
          rsp_header += "Content-Type:text/html\r\n";
          rsp_header += "Connection: close\r\n";
          if(info._version == "HTTP/1.1")
          {
            rsp_header += "Transfer_Encoding: chunked\r\n";
          }
          rsp_header += "ETag: " + _etag + "\r\n";
          rsp_header += "Date: " + ;
          rsp_header += "";
          SendData(rsp_header);

          std::string rsp_body;
          rsp_body = "<html<head>>";
          rsp_body += "<title>" + info._path_info + "</title>";
          rsp_body += "<meta charset = 'UTF-8'>";

          while(1)
          {
            std::string file_html;
            //获取目录下的每一个文件，组织html信息，chunke传输
            SendCdata(file_html);
          }
          rsp_body = "</body>"
      return true;
        }
		bool ProcessCGI(std::string &file)//cgi请求处理

        {

          return true;
        }

		//真正对外的接口
		bool ErrHandler(RequestInfo &info)//处理错误响应
		{
			std::string rsp_header;
			//首行
			//
			//
			//
      rsp_header = info._version + " " +info._err_code + " ";
      rsp_header += Utils::GetErrDesc(info._err_code)+ "\r\n";

      time_t t = time(NULL);
      std::string gmt;
      Utils::TimeToGMT(time_t t,gmt);
      rsp_header += "Date: " +gmt + "\r\n";

      std::string rsp_body;
      rsp_body = "<html><body><h1>" + info._err_code;
      rsp_body += "<h1><body></html>";

      rsp_body += "Content_Length: " + cont_len + 
		}

    bool FileIsDir()
    {
      return true;
    }

		bool CGIHandler(RequestInfo &info)
		{
			InitResponse(info);//初始化CGI响应信息
			ProcessCGI(info);
      FileIsDir(info)
		}
		bool FileHandler(RequestInfo &info)
		{
			InitResponse(info);//初始化文件响应信息
			if (DIR)
			{
				ProcessList(info);//执行文件列表展示响应
			}
			else
			{
				ProcessFile(info);//执行文件下载响应
			}
		}

};


void Usage(const std::string proc)
{
  std::cout << "Usage: " << proc << " ip port" << std::endl;
}

int main(int argc, char* argv[])
{
  if (argc != 3)
  {
    Usage(argv[0]);
    exit(1);
  }
  HttpServer server;
  server.HttpServerInit(argv[1], argv[2]);
  server.Start();
  return 0;
}
