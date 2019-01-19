#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
int main()
{
	char *const argv[] = {"ps","-ef",NULL};
	char *const envp[] = {"PATH=/bin:/usr/bin", "TERM=console",NULL};

	pid_t pid = fork();
	if(pid==0){
		//execve("/bin/ps",argv,envp);//带e的，需要自己组装环境变量
		//execl("/bin/ps","ps","-ef",NULL);
		//execlp("ps","ps","-ef",NULL);//带p的，可以使用环境变量PATH,无需写全路径
		execle("/bin/ps","ps","-ef",NULL,envp);//带e的，需要自己组装环境变量
		//execv("/bin/ps",argv);
		//execvp("ps",argv);//带p的，可以使用环境变量 
		sleep(2);
		exit(0);
	}else{
		sleep(3);
	}
	return 0;
}
