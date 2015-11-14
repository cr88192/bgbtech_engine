#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
	char buf[256];
	FILE *fd;
	unsigned int l;
	int i, j, k;

	fd=fopen(argv[1], "rb");

	l=fgetc(fd);
	l|=fgetc(fd)<<8;
	l|=fgetc(fd)<<16;
	l|=fgetc(fd)<<24;
	while(!feof(fd))
	{
		if(l==0x04034b50)
		{
			printf("local @ %X\n", ftell(fd));

			fseek(fd, 22, 1);

			i=fgetc(fd);
			i|=fgetc(fd)<<8;

			fgetc(fd);
			fgetc(fd);

			memset(buf, 0, 256);
			fread(buf, 1, i, fd);
			printf("%s\n", buf);
		}
		l=(l>>8)|(fgetc(fd)<<24);
	}
}
