#include <stdio.h>

//unsigned char buf[1024];

int func()
{
	return(0);
}

int main()
{
//	unsigned char buf[1024];
	unsigned char *buf;

	void (*fp)();
	int *p;

	p=&func;
//	*p=-1;

	buf=malloc(64);
	buf[0]=0xC3;

	fp=buf;
	fp();

	printf("OK\n");
}
