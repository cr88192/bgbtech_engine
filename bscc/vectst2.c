#include <stdio.h>
#include <complex.h>
#include <vecnf.h>


int bscc_main()
{
	float v0[4], v1[4];
	vec3 a, b;

	a=vec3(2,3,4);
	printf("A %f %f %f %f\n", a.x, a.y, a.z, a.w);

	*((vec3 *)v0)=a;

	printf("B %f %f %f %f\n", v0[0], v0[1], v0[2], v0[3]);

	v1[0]=v0[2];
	v1[1]=v0[1];
	v1[2]=v0[0];
	v1[3]=1;

	printf("C %f %f %f %f\n", v1[0], v1[1], v1[2], v1[3]);

	b=*((vec3 *)v1);
	printf("C %f %f %f %f\n", b.x, b.y, b.z, b.w);
}

int main()
{
	bscc_main();
}