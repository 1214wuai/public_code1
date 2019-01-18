#include<stdio.h>                                               
#include<sys/wait.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>

int main ()
{                                                                                  
	pid_t id;
	id = fork();
	if(id<0){
		printf("%s fotk error\n",__FUNCTION__);
		return 1;
	 }else if(id==0){
		printf("child is run,pid is:%d\n",getpid());
		sleep(5);
		exit(257);
	}else{
		int status = 0;
		pid_t ret = waitpid(-1,&status,0);//阻塞式等待，5s
		printf("this is test for wait\n");
		if( WIFEXITED (status) && ret == id){//if(ret>0)
			printf("wait child 5s success,child return code is%d : %d\n",0xFF&(status>>8),WEXITSTATUS(status));//WEXITSTATUS(status)也就是退出码0xFF&&（status>>8）
		}else{
			printf("wait child failed,return.\n");
			return 1;
			}
	}
	return 0;
 }

