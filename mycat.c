#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
char buf[2048];
int main(int argc,char** argv)
{
	if(argc<1)
	{
		printf("no input file.\n");
		return 0;
	}
	int fd=open(argv[1],O_RDONLY);
	if(fd<0)
	{
		printf("cannot open file %s.\n",argv[1]);
		return 0;
	}
	while(read(fd,buf,2048)>0)
		printf("%s",buf);
	close(fd);
	printf("\n");
	return 0;
}
