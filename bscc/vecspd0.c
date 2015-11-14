#include <stdio.h>
#include <complex.h>
#include <time.h>

#ifdef __BSCC
#include <vecnf.h>
#endif


#ifdef __BSCC
int bscc_main()
{
	float v0[4], v1[4], v2[4];
	vec3 a, b, c;
	float f;
	int i, j, t;

	a=vec3(2,3,4);
	b=a;

	t=clock();
	for(i=0; i<100000000; i++)
		b+=a;
	j=clock()-t;
	printf("VA %d\n", j);

	t=clock();
	for(i=0; i<100000000; i++)
		c+=a%b;
	j=clock()-t;
	printf("VC %d\n", j);

	t=clock();
	for(i=0; i<100000000; i++)
		f+=a^b;
	j=clock()-t;
	printf("VD %d\n", j);


	t=clock();
	for(i=0; i<100000000; i++)
	{
		v0[0]=v0[0]+v1[0];
		v0[1]=v0[1]+v1[1];
		v0[2]=v0[2]+v1[2];
	}
	j=clock()-t;
	printf("VA2 %d\n", j);

	t=clock();
	for(i=0; i<100000000; i++)
	{
		v2[0]=v0[1]*v1[2]-v0[2]*v1[1];
		v2[1]=v0[2]*v1[0]-v0[0]*v1[2];
		v2[2]=v0[0]*v1[1]-v0[1]*v1[0];
	}
	j=clock()-t;
	printf("VC2 %d\n", j);

	t=clock();
	for(i=0; i<100000000; i++)
	{
		f+=v0[0]*v1[0]+v0[1]*v1[1]+v0[2]*v1[2];
	}
	j=clock()-t;
	printf("VD2 %d\n", j);
}
#endif

#ifndef __BSCC

int main()
{
	float v0[4], v1[4], v2[4];
	float f;
	int i, j, t;

	t=clock();
	for(i=0; i<100000000; i++)
	{
		v0[0]=v0[0]+v1[0];
		v0[1]=v0[1]+v1[1];
		v0[2]=v0[2]+v1[2];
	}
	j=clock()-t;
	printf("VA2 %d\n", j);

	t=clock();
	for(i=0; i<100000000; i++)
	{
		v2[0]=v0[1]*v1[2]-v0[2]*v1[1];
		v2[1]=v0[2]*v1[0]-v0[0]*v1[2];
		v2[2]=v0[0]*v1[1]-v0[1]*v1[0];
	}
	j=clock()-t;
	printf("VC2 %d\n", j);

	t=clock();
	for(i=0; i<100000000; i++)
	{
		f+=v0[0]*v1[0]+v0[1]*v1[1]+v0[2]*v1[2];
	}
	j=clock()-t;
	printf("VD2 %d\n", j);
}

#endif
