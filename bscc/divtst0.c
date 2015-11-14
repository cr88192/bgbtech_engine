#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	float a, b, c, e, f;
	int i;
	
	a=3; b=4;
	a=5; b=6;
	
	f=1;
	while((b*f)>1)
	{
		printf("%f %f\n", f, b*f);
		f*=0.5;
	}
	c=a*f;
	for(i=0; i<10; i++)
	{
		e=(a-b*c);
		printf("%f %f %f %f\n", a, b, c, e);
		c=c+e*f;
	}
}