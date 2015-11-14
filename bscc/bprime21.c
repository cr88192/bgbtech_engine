#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//int pl[(1<<16)];
//int pl[65536];
long long pl[65536];

int main()
{
	int np;
//	unsigned long long i, j, k;
	int i, j, k;

	np=0;

#if 1
	for(i=2; i<65536; i++)
	{
//		if(((i-1)>>16)!=(i>>16))fprintf(stderr, "%d\n", i);

#if 1
		for(j=0; j<np; j++)
		{
//			if((i%pl[j])==0)break;	//value is not prime
			if(!(i%pl[j]))break;	//value is not prime

			k=pl[j];
			if((k*k)>i)
			{
				pl[np++]=i;
				break;	//value is prime
			}
		}
#endif

		if(j<np)continue;

		pl[np++]=i;
	}
#endif

	printf("NP %d\n", np);

	for(i=0; i<(np/12); i++)
	{
		for(j=0; j<12; j++)
			printf(" %4d,", pl[i*12+j]);
		printf("\n");
	}
	for(j=0; j<(np%12); j++)
		printf(" %4d,", pl[i*12+j]);
	printf("\n");
}
