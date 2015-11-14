#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

float fakesqrt(float f)
{
	int i;
	i=*(int *)(&f);
	i=((i-0x3F800000)>>1)+0x3F800000;
	return(*(float *)(&i));
}

int main()
{
	float f, g;
	int i, j, k;
	
	for(i=0; i<10; i++)
	{
		f=(i*M_PI); f=pow(1.5, f);
		printf("%f: %f %f\n", f, sqrt(f), fakesqrt(f));
	}
}
