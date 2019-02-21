#include "utils.hpp"
#include "threadpool.hpp"
#include <stdlib.h>
#include <signal.h>

#define MAX_LISTEN 5
#define MAX_THREAD 5



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
    std::string _filesize;//文件大小
    std::string _mime;//文件类型
    //std::string _cont_len;//客户端请求正文大小
  public:
    HttpResponse(int sock):_cli_sock(sock){}
    bool InitResponse(RequestInfo req_info)//初始化一些请求的响应信息 
    {
      //req_info.st.st_size;
      //req_info.st.st_ino;
      //req_info.st.st_mtime;

      //Last-Modified:
      Utils::TimeToGMT(req_info._st.st_mtime,_mtime);

      //ETag:
      Utils::MakeETag( req_info._st.st_ino, req_info._st.st_size, req_info._st.st_mtime, _etag);

      //Date:
      time_t t = time(NULL);
      Utils::TimeToGMT(t, _date);

      //fszie;
      Utils::DigitToStr(req_info._st.st_size,_filesize);
      
      //mime
      Utils::GetMime(req_info._method,_mime);
      return true;
    }

    //发送头部
    bool SendData(std::string buf)
    {

      if(send(_cli_sock, buf.c_str(), buf.length(), 0) < 0)
      {
        return false;
      }
      return true;
    }

    //发送正文
    bool SendCData(const std::string& buf)
    {
      //发送hello word
      //05\r\n
      //hello word\r\n
      //最后一个分块
      //0\r\n\r\n
      if(buf.empty())
      {
        //return SendData("0\r\n");
        return SendData("0\r\n\r\n");
      }

      std::stringstream ss;
      ss<<std::hex<<buf.length()<< "\r\n";

      SendData(ss.str());
      ss.clear();

      SendData(buf);
      SendData("\r\n");

      return true;
    }

    bool ProcessFile(RequestInfo &info)//文件下载功能
    {

      //下载时断网，来网了继续下载，此时就是断点续传
      std::string rsp_header;

      rsp_header = info._version + " 200 OK\r\n";
      rsp_header += "Content-Type: " + _mime + "\r\n";
      rsp_header += "Connection: close\r\n";
      rsp_header += "Content-Length: " + _filesize + "\r\n";
      rsp_header += "ETag: " + _etag + "\r\n";
      rsp_header += "Last-Modified: " + _mtime + "\r\n";
      rsp_header += "Date: " + _date + "\r\n\r\n";
      SendData(rsp_header);

      int fd = open(info._path_phys.c_str(), O_RDONLY);
      if(fd < 0)
      {
        info._err_code = "400";
        ErrHandler(info);
        return false;
      }
      int rlen = 0;
      char tmp[MAX_BUFF];
      while((rlen = read(fd, tmp, MAX_BUFF)) > 0)
      {
        send(_cli_sock, tmp, rlen, 0);
        tmp[rlen] = '\0';
        SendData(tmp);
      }
      close(fd);
      return true;
    }
    bool ProcessList(RequestInfo &info)//文件列表功能
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
        rsp_header += "Transfer-Encoding: chunked\r\n";//分块传输，每发送一块数据之前都会告诉对方这个数据多长
      }
      rsp_header += "ETag: " + _etag + "\r\n";
      rsp_header += "Last-Modified: " + _mtime + "\r\n";
      rsp_header += "Date: " + _date + "\r\n\r\n";
      SendData(rsp_header);

      std::string rsp_body;
      rsp_body = "<html><head>";
      rsp_body += "<title>" + info._path_info + "</title>";
      rsp_body += "<meta charset='UTF-8'>";
      rsp_body += "</head><body>";
      rsp_body += "<h1>Index of" + info._path_info+"</h1><hr /><or>";
      rsp_body += "<form action='/upload' method='POST' enctype='multipart/from-data'>";
      rsp_body += "<input type='file' name='FileUpLoad' />";
      rsp_body += "<input type='submit' value='上传' />";
      SendCData(rsp_body);
      // while(1)
      //std::string file_html;

      std::string path = info._path_phys;
      struct dirent** p_dirent = NULL;
      //scandir函数,的哥参数，第二个参数：三级指针，第三个参数：filter过滤掉“.”给1，不过滤给0，第四个参数来进行排序
      //获取目录下的每一个文件，组织html信息，chunke传输
      //readdir函数
      // file_html = "<li>";
      int num = scandir(info._path_phys.c_str(), &p_dirent, 0, alphasort);
      for(int i = 0; i< num; i++)
      {
        std::string file;
        std::string file_html;
        file = info._path_phys + p_dirent[i]->d_name;
        struct stat st;
        if(stat(file.c_str(), &st) < 0)
          continue;

        std::string mtime;
        std::string mime;
        Utils::GetMime(p_dirent[i]->d_name, mime);
        std::string fsize;
        Utils::DigitToStr(st.st_size / 1024, fsize);
        Utils::TimeToGMT(st.st_mtime, mtime);
        file_html += "<li><strong><a href='"+ info._path_info;
        file_html += p_dirent[i]->d_name;
        file_html += "'>";
        file_html += p_dirent[i]->d_name;
        file_html += "</a></strong>";
        file_html += "<br /><small>";
        file_html += "modifued: " + mtime + "<br />";
        file_html += mime + " - " += fsize + " kbytes";
        file_html += "<br /><br /></small></li>";

        //file_html += "</small>" + info._err_code + "</li>";
        SendCData(file_html);
      }
      rsp_body = "</ol><hr /></body></html>";
      SendCData(rsp_body);
      //进行分块发送的时候告诉
      SendCData("");
      return true;
    }		
    bool ProcessCGI(RequestInfo &info)//cgi请求处理
    {
      //使用外部程序完成CGI处理---文件上传
      //将http头信息和正文全部交给子进程处理
      //使用环境变量传递头信息
      //创建管道传递正文顺序
      //使用管道接收cgi程序的处理结果
      //流程：创建管道，创建子进程，设置子进程环境变量，程序替换
      int in[2];//向子进程传递正文数据
      int out[2];//从子进程读取处理结果
      if(pipe(in) || pipe(out))
      {
        info._err_code = "500";
        ErrHandler(info);
        return false;
      }
      int pid = fork();
      if(pid < 0)
      {
        info._err_code = "500";
        ErrHandler(info);
        return false;
      }
      else if(pid == 0)
      {
        //setenv函数，设置环境变量，第三个参数给非0，覆盖
        setenv("METHOD", info._method.c_str(), 1);
        setenv("VERSION", info._version.c_str(), 1);
        setenv("PATH_INFO", info._path_info.c_str(),1);//上传不需要实际物理路径
        setenv("QUERY_STRING", info._query_string.c_str(), 1);
        for(auto it = info._hdr_list.begin(); it != info._hdr_list.end(); it++)
        {
          setenv(it->first.c_str(), it->second.c_str(), 1);
        }
        close(in[1]);
        close(out[0]);
        dup2(in[0],0);//子进程将从标准输入读取正文数据
        dup2(out[1],1);//子进程直接打印处理结果传递给父进程
        execl(info._path_phys.c_str(), info._path_phys.c_str(), NULL);
        exit(0);
      }
      //父进程接下来就是对管道进行操作
      //1.通过in管道传递正文数据给子进程
      //2.通过out管道读取子进程的处理结果直到返回0
      //3.将数据处理结果组织http数据，响应给客户端
      close(in[0]);
      close(out[1]);
      //1
      auto it = info._hdr_list.find("Content-Length");
      if(it!=info._hdr_list.end())
      {
        char buf[MAX_BUFF] = {0};
        int64_t content_len = Utils::StrToDig(it->second);
        int rlen = recv(_cli_sock, buf, MAX_BUFF, 0);

      }
      //每一个目录的文件都要组织一个html标签信息
      //2
      //3
      std::string rsp_header;

      rsp_header = info._version + " 200 OK\r\n";
      rsp_header += "Content-Type:text/html\r\n";
      rsp_header += "Connection: close\r\n";
      if(info._version == "HTTP/1.1")
      {
        rsp_header += "Transfer-Encoding: chunked\r\n";//分块传输，每发送一块数据之前都会告诉对方这个数据多长
      }
      rsp_header += "ETag: " + _etag + "\r\n";
      rsp_header += "Last-Modified: " + _mtime + "\r\n";
      rsp_header += "Data: " + _date + "\r\n\r\n";
      SendData(rsp_header);
      while(1)
      {
        char buf[MAX_BUFF] = {0};
        int rlen = read(out[0], buf, MAX_BUFF);
        if(rlen == 0)
          break;
        send(_cli_sock, buf, rlen, 0);
      }

      return true;
    }

    //真正对外的接口
    bool ErrHandler(RequestInfo &info)//处理错误响应
    {
      std::string rsp_header;
      std::string rsp_body;
      //首行 协议版本 状态码 状态描述
      //头部
      //空行
      //正文
      rsp_header = info._version + " " +info._err_code + " ";
      rsp_header += Utils::GetErrDesc(info._err_code)+ "\r\n";

      time_t t = time(NULL);
      std::string gmt;
      Utils::TimeToGMT(t,gmt);
      rsp_header += "Date: " +gmt + "\r\n";

      std::string cont_len;
      rsp_body = "<html><body><h1>" + info._err_code;
      rsp_body += "<h1><body></html>";
      Utils::DigitToStr(rsp_body.length(), cont_len);
      rsp_body += "Content_Length: " + cont_len + "\r\n\r\n";

      std::cout<<"\r\n\r\n";
      std::cout<<rsp_header<<std::endl;
      std::cout<<rsp_body<<std::endl;
      std::cout<<"\n\n\n\n";



      send(_cli_sock, rsp_header.c_str(), rsp_header.length(), 0);
      send(_cli_sock, rsp_body.c_str(), rsp_body.length(), 0);
      return true;
    }

    bool FileIsDir(RequestInfo &info)
    {
      std::string path = info._path_info;
      if(info._st.st_mode & S_IFDIR)
      {
        if(path[path.length() -1] != '/')
          info._path_info.push_back('/');
        return true;
      }
      if(path[path.length() -1] != '/')
        info._path_info.push_back('/');
      return false;
    }

    bool CGIHandler(RequestInfo &info)
    {
      InitResponse(info);//初始化CGI响应信息
      ProcessCGI(info);
      // FileIsDir(info)
    }
    bool FileHandler(RequestInfo &info)
    {
      InitResponse(info);//初始化文件响应信息
      if (FileIsDir(info))
      {
        ProcessList(info);//执行文件列表展示响应
      }
      else
      {
        ProcessFile(info);//执行文件下载响应
      }
      return true;
    }

};
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
    //req.RecvHttpHeader(info);
    
    
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
      rsp.CGIHandler(info);
    }
    else 
    {
      //若当前请求类型不是CGI请求，则执行文件列表或文件下载响应
      rsp.FileHandler(info);
    }
    
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

    //地址复用
    int opt = 1;
    setsockopt(_serv_sock, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));

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
  signal(SIGPIPE, SIG_IGN);//重要，连接
  HttpServer server;
  if(server.HttpServerInit(argv[1], argv[2]) == false)
  {
    return -1;
  }

 if(server.Start() == false)
 {
   return -1;
 }
  return 0;
}
