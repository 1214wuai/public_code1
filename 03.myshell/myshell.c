#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>

#define MAX 1024
#define NUM 16

int main()
{

	char *myargv[NUM];
	char cmd[MAX];
	printf("[shabi@bogon 10_12]$ ");
	fgets(cmd,sizeof(cmd),stdin);
	cmd[strlen(cmd)-1] = '\0';
	int i = 0;
	myargv[i++] = strtok(cmd, " ");//ls -a -l -i
	char *ret = NULL;
	while(ret = strtok(NULL," "))
	{
		myargv[i++] = ret;
	}
	myargv[i]=NULL;
	pid_t id = fork();
	if(id == 0)//child
	{
		execvp(myargv[0],myargv);
		exit(1);
	}
	else
	{
		waitpid(id,NULL,0);
	}
	return 0;
}
