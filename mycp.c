#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
char buf[2048];
int main(int argc,char** argv)
{
	if(argc<2)
	{
		printf("no i/o files.\n");
		return 0;
	}
	int src=open(argv[1],O_RDONLY);
	int drt=open(argv[2],O_RDWR|O_CREAT|O_TRUNC,S_IWUSR|S_IRUSR|S_IWGRP|S_IRGRP|S_IWOTH|S_IROTH);
	if(src<0)
	{
		printf("failed to open %s.\n",argv[1]);
		return 0;
	}
	if(drt<0)
	{
		printf("failed to open %s.\n",argv[2]);
		return 0;
	}
	int len;
	while((len=read(src,buf,2047))>0)
		write(drt,buf,len);
	close(src);
	close(drt);
	return 0;
}
