#include<stdio.h>
int main(int argc, char *argv[], char * env[])
{
	int i = 0;
	for(;env[i];i++)
	{
		printf("%d->%s\n",i,env[i]);
	}
//获得当前进程的环境变量
//environ 二级指针也可以找到环境变量
//getenv函数
//char *getenv(const char *name);参数传SHELL，返回的是/bin/bash 参数如果是PATH，返回的是遗传路径
//putenv 传入“PATH=.......”

}
