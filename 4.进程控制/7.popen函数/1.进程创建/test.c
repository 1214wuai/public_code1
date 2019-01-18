#include<stdlib.h>                                                                 
#include<stdio.h>
#include<unistd.h>
int glob = 10;
int main()
{
	pid_t pid=vfork();
	if(pid==0)
	{
		glob = 30;
		printf("I am child , pid: %d, glob = %d, &glob %d\n",getpid(),glob,&glob);
		sleep(3);
		exit(0);
	}else{
		printf("I am parent , pid: %d, glob = %d, &glob %d\n",getpid(),glob,&glob);
		sleep(1);
	}
	sleep(1);
}	
