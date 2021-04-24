#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void mysys(char* command)
{
	pid_t pid=fork();
	if(pid==0)
	{
		execl("/bin/sh","sh","-c",command,NULL);
		exit(0);
	}
	wait(NULL);
	return;
}
int main()
{
	printf("--------------------------------------------------------------------\n");
	mysys("echo HELLO WORLD");
	printf("--------------------------------------------------------------------\n");
	mysys("ls /");
	printf("--------------------------------------------------------------------\n");
	return 0;
}
