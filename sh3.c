#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#define REINPUT  0x01
#define REOUTPUT 0x02

#define MAXARGC 16
#define MAXCOMM 256
struct command
{
	int argc;
	char* argv[MAXARGC];
	char* input;
	char* output;
};
struct command commands[MAXCOMM];

void cmdinit()
{
	for(int i=0;i<MAXCOMM;++i)
	{
		commands[i].argc=0;
		commands[i].argv[0]=NULL;
		commands[i].input=NULL;
		commands[i].output=NULL;
	}
	return;
}

void cmdclear()
{
	for(int i=0;commands[i].argc;++i)
	{
		commands[i].argc=0;
		for(int j=0;j<MAXARGC && commands[i].argv[j];++j)
		{
			free(commands[i].argv[j]);
			commands[i].argv[j]=NULL;
		}
		if(commands[i].input)
		{
			free(commands[i].input);
			commands[i].input=NULL;
		}
		if(commands[i].output)
		{
			free(commands[i].output);
			commands[i].output=NULL;
		}
	}
	return;
}

void parse_command(int line,char* cmd)
{
	int index=0;
	char* p=strtok(cmd," ");
	while(p)
	{
		if(index>=MAXARGC-1)
			break;
		commands[line].argv[index]=(char*)malloc(strlen(p)+1);
		strcpy(commands[line].argv[index],p);
		++index;
		p=strtok(NULL," ");
	}
	commands[line].argc=index;
	commands[line].argv[index]=NULL;
	return;
}

int parse_commands(char* line)
{
	int cnum=0;
	char* buf[MAXCOMM];
	char* p=strtok(line,"|");
	while(p)
	{
		if(cnum>=MAXCOMM)
			break;
		buf[cnum]=(char*)malloc(strlen(p)+1);
		strcpy(buf[cnum],p);
		++cnum;
		p=strtok(NULL,"|");
	}
	for(int i=0;i<cnum;++i)
	{
		parse_command(i,buf[i]);
		free(buf[i]);
	}
	return cnum;
}

void cmdexec(int index)
{
	unsigned char redirect=0;
	for(int i=0;commands[index].argv[i];++i)
	{
		if(commands[index].argv[i][0]=='>')
		{
			commands[index].output=(char*)malloc(strlen(commands[index].argv[i]));
			strcpy(
				commands[index].output,
				commands[index].argv[i]+1
				);
			free(commands[index].argv[i]);
			for(int j=i;j<commands[index].argc;++j)
				commands[index].argv[j]=commands[index].argv[j+1];
			--i;
			--commands[index].argc;
			redirect|=REOUTPUT;
		}
		else if(commands[index].argv[i][0]=='<')
		{
			commands[index].input=(char*)malloc(strlen(commands[index].argv[i]));
			strcpy(
				commands[index].input,
				commands[index].argv[i]+1
				);
			free(commands[index].argv[i]);
			for(int j=i;j<commands[index].argc;++j)
				commands[index].argv[j]=commands[index].argv[j+1];
			--i;
			--commands[index].argc;
			redirect|=REINPUT;
		}
	}
	if(redirect&REOUTPUT)
	{
		int fd=open(commands[index].output,O_CREAT|O_RDWR,0666);
		dup2(fd,1);
		close(fd);
	}
	if(redirect&REINPUT)
	{
		int fd=open(commands[index].input,O_RDWR|O_ASYNC);
		dup2(fd,0);
		close(fd);
	}
	if(execvp(commands[index].argv[0],commands[index].argv)<0)
		perror("command not found");
	exit(0);
	return;
}

void cmds_exec(int cmd_size)
{
	if(!cmd_size)
		return;
	if(cmd_size==1)
		cmdexec(0);
	
	int fd[2];
	char buf[1024];
	pipe(fd);
	pid_t pid=fork();
	if(pid==0)
	{
		dup2(fd[1],1);
		close(fd[0]);
		close(fd[1]);
		cmds_exec(cmd_size-1);
	}
	dup2(fd[0],0);
	close(fd[0]);
	close(fd[1]);

	cmdexec(cmd_size-1);
	return;
}

void mysys(int cmd_size)
{
	if(!cmd_size)
		return;

	pid_t pid=fork();
	if(pid==0)
		cmds_exec(cmd_size);
	wait(NULL);
	return;
}
int main()
{
	cmdinit();
	char buf[1024];
	while(1)
	{
		write(1,"$ ",2);
		memset(buf,0,sizeof(buf));
		int len=read(0,buf,1024);
		if(buf[0]=='\n')
			continue;
		buf[len-1]='\0';
		int cmd_size=parse_commands(buf);
		if(!strcmp(commands[0].argv[0],"exit"))
			break;
		else if(!strcmp(commands[0].argv[0],"cd"))
			chdir(commands[0].argv[1]?commands[0].argv[1]:"/home/user");
		else
			mysys(cmd_size);
		cmdclear();
	}
	return 0;
}
