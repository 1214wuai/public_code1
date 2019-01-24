#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<fcntl.h>
#include<stdlib.h>
#include<pthread.h>
#include<string.h>
#include<sys/sendfile.h>
#include<unistd.h>
#include<sys/stat.h>

#define MAX 1024
int getLine(int sock, char line[], int num)
{
  char c = 'x';
  int i = 0;
  while(c != '\n' && i< num -1)
  {
    ssize_t s = recv(sock,&c,1,0);
    if(s>0){
      //\r\n, \r, \n->\n
      if(c == '\r')
      {
        recv(sock,&c,1,MSG_PEEK);
        if(c=='\n')
        {
          recv(sock,&c,1,0);
        }
        else{
          c = '\n';
        }
      }
      line[i++] = c;
    }
    else{
      break;
    }
  }
  line[i] = 0;
  return i;
}

void clearHeader(int sock)
{
  char line[MAX];
  do{
    getLine(sock, line,MAX);
  }while(strcmp(line,"\n")!=0);

}

void echoError(int sock, int statusCode)
{
  switch(statusCode){
    case 404:
      //show_404();
      break;
    default:
      break;
  }
}

int echoHtml(int sock, char* path, int size)
{
  char line[MAX];
  clearHeader(sock);
  int fd = open(path,O_RDONLY);
  if(fd < 0)
  {
    return 404;
  }

  char stuff
  sprintf(line, "http/1.0 200 OK\r\n");
  send(sock,line,strlen(line),0);
  sprintf(line, "Content-Type: text/html\r\n");
  send(sock,line,strlen(line),0);
  sprintf(line, "Content-Length: %d\r\n", size);
  send(sock,line,strlen(line),0);
  sprintf(line, "\r\n");
  send(sock,line,strlen(line),0);
  sendfile(sock,fd,NULL,size);

}
void* handlerRequest(void* arg)
{
  int sock = (int)arg;
  char line[MAX];
  char method[MAX/16];
  char url[MAX];
  int i = 0;
  int j = 0;
  int statusCode = 200;//状态码
  int cgi = 0;
  char* query_string = NULL;//url参数
  char path[MAX];//真正意义上访问的资源
  getLine(sock,line, MAX);
 //首行中的方法 
  while(i<sizeof(method)-1 && j<sizeof(line) && !isspace(line[j]))
  {
    method[i] = line[j];
    i++,j++;
  }
  method[i] = '\0';
  if(strcmp(method, "GET")==0)
  {}
  else if(strcmp(method, "POST")==0)
  {
    //POST方法需要cgi，正文传参
    cgi = 1;
  }
  else
  {
    clearHeader(sock);//清空报头
    statusCode = 404;
    goto end;
  }

  while(j<sizeof(line) && isspace(line[j]))
  {
    j++;
  }
  //提取有效内容
  i = 0;
  while(i<sizeof(url)-1 && j<sizeof(line) && !isspace(line[j]))
  {
    url[i] = line[j];
    i++,j++;
  }
  url[i] = '\0';
  printf("method: %s, url: %s\n",method,url);
  //判断url中是否有参数
  if(strcmp(method,"GET") == 0)
  {
    query_string = url;
    while(*query_string != '\0')
    {
      if(*query_string == '?')
      {
        cgi = 1;
        *query_string = '\0';
        query_string++;
        break;
      }
      query_string++;
    }
  }
  // method, url, query_string
  // POST方法没有query_string,GET方法不传参也没有query_string
  sprintf(path,"web/%s",url);//a/b/c.html            // /给首页
  if(path[strlen(path)-1] == '/')// /访问首页
  {
    strcat(path,"index.html");
  }

  struct stat st;
  if(stat(path,&st)<0)
  {
    clearHeader(sock);
    statusCode = 404;
    goto end;
  }
  else
  {
    //是否是目录
    if(S_ISDIR(st.st_mode)){
      strcat(path,"/index.html");
    }
    //是否是cgi
    if(cgi)
    {}
    else{
      statusCode = echoHtml(sock,path, st.st_size);
    }
  }
end:
  if(statusCode != 200)
  {
    echoError(sock, statusCode);
  }
  close(sock);
}

int startup(char* ip, int port)
{
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if(sock < 0)
  {
    perror("socket error");
    exit(2);
  }
  struct sockaddr_in local;
  local.sin_family = AF_INET;
  local.sin_addr.s_addr = inet_addr(ip);
  local.sin_port = htons(port);

  if(bind(sock,(struct sockaddr*)&local, sizeof(local))<0)
  {
    perror("bind");
    exit(3);
  }

  if(listen(sock,5)<0)
  {
    perror("listen");
    exit(4);
  }

  return sock;
}
// ./myTomcat ip 8080
int main(int argc, char*argv[])
{
  if(argc != 3)
  {
    return 0;
  }
  int listen_sock = startup(argv[1], atoi(argv[2]));
  for(;;)
  {
    struct sockaddr_in peer;
    socklen_t len = sizeof(peer);
    int sock = accept(listen_sock, (struct sockaddr*)&peer,&len);
    if(sock<0)
    {
      continue;
    }
    printf("get a new link\n");
    pthread_t tid;
    pthread_create(&tid, NULL, handlerRequest,(void*)sock);
    pthread_detach(tid);
  }
}
