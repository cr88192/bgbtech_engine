#include <stdio.h>

typedef int s32;
typedef unsigned int u32;

typedef long long s64;
typedef unsigned long long u64;

int main()
{
	s64 a;
	u64 b;
	int i, j, k;

	for(i=0; i<64; i++)
	{
		j=rand()*rand()*rand();
		k=rand()*rand()*rand();

//		a=((s64)j)*((s32)k);
//		b=((u64)j)*((u32)k);

		a=((s64)((s32)j))*((s64)((s32)k));
		b=((u64)((u32)j))*((u64)((u32)k));

		j=(a>>32);
		k=(b>>32);

		printf("%08X %08X %d", j, k, j==k);
		printf("    %08X %08X %d\n",
			(u32)a, (u32)b, ((u32)a)==((u32)b));
	}
}
