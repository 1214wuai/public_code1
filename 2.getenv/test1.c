#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<stdlib.h>
int main (int argc, char *argv[], char *env[])
{
	printf("%s\n",getenv("PATH"));
	printf("%s\n",getenv("SHELL"));
	return 0;
}
