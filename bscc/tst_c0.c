#include <stdio.h>

int cmp(unsigned int x, unsigned int y)
{
	unsigned int z, fl;

	fl=0;
//	if(x==y)
//		fl|=64;
	
	z=(x>>1) - (y>>1);
//	if(!z)z+=(x&1)?1:-1;
	if(!z)
	{
//		if(x&1)z++;
//		if(y&1)z--;
		z+=x&1;
		z-=y&1;
		if(!z)fl|=64;
	}

	fl|=(z>>31)&1;
	fl|=(z>>23)&128;
	fl|=((fl>>4)^(fl<<3))&8;

	printf("%10u %10u %10u %02X\n", x, y, z, fl);

	return(fl);
}

int main()
{
	int i, j;
	
	for(i=-3; i<3; i++)
		for(j=-3; j<3; j++)
			cmp(i, j);
}