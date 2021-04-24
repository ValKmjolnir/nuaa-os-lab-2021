#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
char buf[1024];
char** split(char* str)
{
	char** exec=(char**)malloc(256*sizeof(char**));
	int index=0;
	char* p=strtok(str," ");
	while(p)
	{
		exec[index]=(char*)malloc(strlen(p)+1);
		strcpy(exec[index],p);
		++index;
		p=strtok(NULL," ");
	}
	exec[index]=NULL;
	return exec;
}
void delete(char** exec)
{
	for(int i=0;exec[i];++i)
		free(exec[i]);
	free(exec);
	return;
}
void mysys(char** exec)
{
	pid_t pid=fork();
	if(pid==0)
	{
		if(execvp(exec[0],exec)<0)
			perror("command not found");
		exit(0);
	}
	else
		wait(NULL);
	return;
}
int main()
{
	while(1)
	{
		write(1,"$ ",2);
		memset(buf,0,sizeof(buf));
		int len=read(0,buf,1024);
		if(buf[0]=='\n')
			continue;
		buf[len-1]='\0';
		char** exec=split(buf);
		if(!strcmp(exec[0],"exit"))
			break;
		else if(!strcmp(exec[0],"cd"))
			chdir(exec[1]?exec[1]:"/home/user");
		else
			mysys(exec);
		delete(exec);
	}
	return 0;
}
