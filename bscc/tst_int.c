#include <stdio.h>

int main()
{
//	unsigned char a, b;
//	int c;

	unsigned int a, b;
	long long c;
	
//	a=254; b=3;
	a=(1ULL<<32)-255;
	b=312;
	c=a+b;
	printf("%u %d\n", (unsigned int)c, (int)(c>>32));
}
