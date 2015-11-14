#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>

float vec_dot1(const float* a, const float* b, int len)
{
    float f;
    int i;
    f=0;
    for(i=0; i<len; i++)f+=a[i]*b[i];
    return(f);
}

float vec_dot2(const float* a, const float* b, int len)
{
    float f;
    f=0;
    while(len--)f+=a[len]*b[len];
    return(f);
}

int bscc_main()
{
	float v0[64], v1[64];
	int i, j;

	i=clock();

	for(i=0; j<10000000; j++)
		vec_dot1(v0, v1, 64);

	i=clock()-i;
	printf("%f\n", i/(float)CLOCKS_PER_SEC);


	i=clock();

	for(i=0; j<10000000; j++)
		vec_dot2(v0, v1, 64);

	i=clock()-i;
	printf("%f\n", i/(float)CLOCKS_PER_SEC);
}

int main()
{
	bscc_main();
}
