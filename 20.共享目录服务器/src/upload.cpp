#include "utils.hpp"

enum _boudry_type{
  BOUNDRY_NO = 0,
  BOUNDRY_FIRST,
  BOUNDRY_MIDDLE,
  BOUNDRY_LAST,
  BOUNDRY_BACK  //部分boundary
};
class Upload
{
  private:
    int _file_fd;
    int64_t content_len;
    std::string _file_name;
    std::string _f_boundry;
    std::string _m_boundry;
    std::string _l_boundry;
  private:
    int MAtchBoundry(char* buf, size_t blen, int* boundry_pos)
    {
      //----boundry
      //first_boundry:------boundry
      //middle_boundry:\r\n------boundry
      //last_boundry:  \r\n------boundry--
      //从起始位置匹配first_boundry
      if(!memcmp(buf, _f_boundry.c_str(), _f_boundry.length()))
      {
        *boundry_pos = 0;
        return BOUNDRY_FIRST;
      }

      //匹配middle和last
      //如果剩余长度大于boundary的长度就进行完全匹配，否则进行部分匹配
      for(size_t i = 0; i<blen; i++)
      {
        if((blen-i) >= _m_boundry.length())//字符串剩余长度大于boundary的长度，则全部匹配
        {
          if(!memcmp(buf+i, _m_boundry.c_str(), _m_boundry.length()))
          {
            *boundry_pos = i;
            return BOUNDRY_MIDDLE;
          }
          else if(!memcmp(buf+i, _l_boundry.c_str(), _l_boundry.length()))
          {
            *boundry_pos = i;
            return BOUNDRY_LAST;
          }
        }
        else
        {
          //剩余长度小于boundary长度，防止出现半个boundary，所以进行部分匹配
          //全部匹配，否则部分比配
          //int cmp_len = (blen -i) > _m_boundry.length() ? _m_boundry.length() : (blen -i);
          int cmp_len = blen - i; 
          if(!memcmp(buf+i, _m_boundry.c_str(), cmp_len))
          {
            *boundry_pos = i;
            return BOUNDRY_BACK;
          }
          else if(!memcmp(buf+i, _l_boundry.c_str(), cmp_len))
          {
           *boundry_pos = i;
            return BOUNDRY_BACK;
          }
        }
      }
      return BOUNDRY_NO;
    }

    bool GetFileName(char* buf, int* content_pos)
    {
      char* ptr = NULL;
      ptr = strstr(buf, "\r\n\r\n");
      if(ptr == NULL)
      {
        //头信息不完整
        *content_pos = 0;//前面的数据不能删
        return false;
      }
      *content_pos = ptr - buf + 4;
      std::string header;
      header.assign(buf,ptr - buf);

      std::string file_sep = "filename=\"";
      size_t pos =  header.find(file_sep);
      if(pos == std::string::npos)
      {
        return false;
      }
      std::string file;
      file = header.substr(pos + file_sep.length());
      pos = file.find("\"");
      if(pos == std::string::npos)
      {
        return false;
      }
      file.erase(pos);
      _file_name = WWWROOT;
      _file_name += "/";
      _file_name += file;
      fprintf(stderr, "upload file:[%s]\n", _file_name.c_str());
      return true;
    }

    bool CreatFile()
    {
      _file_fd = open(_file_name.c_str(), O_CREAT|O_WRONLY, 0664);
      if(_file_fd < 0)
      {

        fprintf(stderr, "open error:%s\n", strerror(errno));
        return false;
      }
      return true;
    }

    bool CloseFile()
    {
      if(_file_fd != -1)
      {
        close(_file_fd);
        _file_fd = -1;
      }
      return true;
    }
    bool WriteFile(char* buf, int len)
    {
      if(_file_fd != -1)
      {
        write(_file_fd, buf, len);
      }
      return true;
    }
  public:
    Upload():_file_fd(-1){}
    //初始化boundary信息
    bool InitUploadInfo()
    {
      umask(0);
      char* ptr = getenv("Content-Length");
      if(ptr == nullptr)
      {
        fprintf(stderr, "upload.cpp 144 :have no content-length!!\n");
        return false;
      }
      content_len = Utils::StrToDig(ptr);
      ptr = getenv("Content-Type");
      if(ptr == nullptr)
      {
        fprintf(stderr, "have no content-type!!\n");
        return false;
      }
      std::string boundry_sep = "boundary=";
      std::string content_type = ptr;
      size_t pos = content_type.find(boundry_sep);
      if(pos == std::string::npos)
      {
        fprintf(stderr, "content type have no boundry!!\n");
        return false;
      }
      std::string boundry;
      boundry = content_type.substr(pos+boundry_sep.length());
      _f_boundry = "--" + boundry;
      _m_boundry = "\r\n" + _f_boundry + "\r\n";
      _l_boundry = "\r\n" + _f_boundry + "--";


      //int64_t cont_len = Utils::StrToDig(content_len);
      return true;
    }

    //对正文进行处理，将文件数据进行存储
    bool ProcessUpload()
    {
      //tlen：当前已经读取的长度
      //blen：buffer长度
      int64_t tlen = 0, blen = 0;
      char buf[MAX_BUFF];
      while(tlen < content_len)
      {
        //从管道中将数据读取出来
        int len = read(0, buf+blen,MAX_BUFF-blen);
        blen += len;//当前buf中数据的长度
        int boundry_pos;
        int content_pos;
        int flag = MAtchBoundry(buf, blen, &boundry_pos);
        if(flag == BOUNDRY_FIRST)
        {
          fprintf(stderr, "[In Boundry_First]\n");
          //1.从boundary头汇总获取文件名
          //2.若获取文件名成功，则创建文件，打开文件
          //3.将头信息从buf中移除，剩下的数据进行下一步匹配
          if(GetFileName(buf, &content_pos))//如果获取到了文件名
          {
            CreatFile();
            blen -= content_pos;
            memmove(buf,buf+content_pos, blen);//把文件数据向前移动，就把之前的first_boundry等数据覆盖了
            memset(buf+blen, 0, content_pos);
            fprintf(stderr, "[In BOUNDRY_FIRST去除分隔符和内容->buf:%s]", buf);
          }
          else
          {
            //接收到了first boundary，但是没有匹配到文件名，就把boundary干掉
            //blen -=  _f_boundry.length();
            //memmove(buf, buf + _f_boundry.length(), blen);
            blen -= boundry_pos;
            memmove(buf, buf+boundry_pos, blen);
            memset(buf+blen, 0 , boundry_pos);
          }

        }
        while(1)
        {
          flag = MAtchBoundry(buf, blen, &boundry_pos);
          if(flag != BOUNDRY_MIDDLE)
          {
              break;//没有匹配到middle_boundry就跳出循环
          }
          //匹配成功
          //1.将boundary之前的数据写入文件，将数据从buf中移除
          //2.关闭文件
          //3.看boundary头中是否有文件名，剩下的流程同first_boundry一样

          //如果有文件就打开，进行写入；没有文件就不进行写入直接将数据去除
          WriteFile(buf, boundry_pos);//boundry_pos指向数据的最后，
          CloseFile();
          blen -= boundry_pos;//将文件数去除去
          memmove(buf, buf + content_pos, blen);
          memset(buf + blen, 0, boundry_pos);
          
          if(GetFileName(buf, &content_pos))
          {
            CreatFile();
            memmove(buf,buf+content_pos, blen - content_pos);//把文件数据向前移动，就把之前的first_boundry等数据覆盖了
            memset(buf + blen, 0, content_pos);
          }
          else
          {
            if(content_pos == 0)
            {
              break;
            } 
            //没有找到名字或者名字后面的"
            //没有匹配成功就把boundry去除，防止下次进入再找者一个boundry
            blen -= _m_boundry.length();
            memmove(buf, buf+_m_boundry.length(), blen);
            memset(buf+blen, 0, _m_boundry.length());
          }
        }

        flag = MAtchBoundry(buf, blen, &boundry_pos);
        if(flag == BOUNDRY_LAST)
        {
          //last_boundry匹配成功
          //1.将boundary之前的数据集写入文件
          //2.关闭文件
          //3.上传文件处理完毕
          WriteFile(buf, boundry_pos);
          CloseFile();
          return true;
        
        }
        flag = MAtchBoundry(buf, blen, &boundry_pos);
        if(flag == BOUNDRY_BACK)
        {
          //将类似boundary位置之前的数据写入文件
          //1.移除之前的数据
          //2.剩下的数据不懂，重新接受数据，补全后匹配

          WriteFile(buf, boundry_pos);//boundry_pos指向数据的最后，
          blen -= boundry_pos ;
          memmove(buf, buf + content_pos, blen);
          memset(buf+blen, 0, boundry_pos);
          
        }
        flag = MAtchBoundry(buf, blen, &boundry_pos);
        if(flag == BOUNDRY_NO)
        {
          //直接将buf中所有写入文件
          WriteFile(buf, boundry_pos);//boundry_pos指向数据的最后，
          blen = 0;
        }
        tlen += len;
      }
      return true;
    }
};

int main()
{
  //缓冲区的数据没有读完，会向对方发送一个连接重置的报文
  Upload upload;
  std::string rsp_body;
  if(upload.InitUploadInfo() == false)
  {
    return 0;
  }
  if(upload.ProcessUpload() == false)
  {
    rsp_body = "<html><body><h1><FALSE!></h1></body></html>";
  }
  else
  {
    rsp_body = "<html><body><h1><SUCCESS!></h1></body></html>";
  }
  std::cout<<rsp_body;
  fflush(stdout);
  return 0;
}
