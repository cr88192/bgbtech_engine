//#ifndef __BSCC
#if 1

#include <stdio.h>
#include <time.h>

#endif

#ifndef CLOCKS_PER_SEC
#define CLOCKS_PER_SEC	60
#endif

#define	EXPR(a, b)	((a)*(a))+((b)*(b))

#ifndef PF
#define PF rf_
#endif

#ifndef _BGBMETA
//#if 0
#define FN(a) rf_##a
#else
#define FN_I(a, b) a ## b
#define FN(a) FN_I(PF, a)
#endif

int FN(fib)(int x)
{
	if(x>2)return(FN(fib)(x-1)+FN(fib)(x-2));
	return(1);
}

#if 1
long long FN(lfib)(long long x)
{
	if(x>2)return(FN(lfib)(x-1)+FN(lfib)(x-2));
	return(1);
}

double FN(dfib)(double x)
{
	if(x>2)return(FN(dfib)(x-1)+FN(dfib)(x-2));
	return(1);
}

int FN(ltst)(int x)
{
	int i, j;

	j=0;
	for(i=0; i<x; i++)j++;

	return(j);
}

typedef struct foo_s {
int x, y;
}foo;

int FN(stst)(foo *st)
{
	return(st->x+st->y);
}

int FN(stst1)()
{
	foo st;
	int i;
	st.x=3; st.y=4;
	i=FN(stst)(&st);
	printf("stst %d\n", i);
	return(0);
}

#endif

//int arrtst[]={1, 2, 3, 4};

int FN(hash_str)(char *s)
{
	int i;

	i=0;
	while(*s)i=i*251+(*s++);
	return(i&0xFFF);
}

int FN(bscc_main)()
{
	long long li;
	int i, t, dt;

	printf("test\n");

	t=clock();
	i=FN(fib)(36);
	dt=clock()-t;

	printf("fibtst %d %d\n", i, dt);

	i=FN(hash_str)("foobarbaz");
	printf("hashtst %d\n", i);

//	printf("hashtst %d\n", hash_str("foobarbaz"));

	FN(stst1)();

	li=0;
//	li=i;
	if(!li)printf("LI-Z\n");
//	t=i;
//	li=0;

	return(0);
}

#if 0
int main()
{
	int i, t, dt;

	t=clock();

//	EXPR(3, 4);

	i=fib(36);
//	i=ltst(100000000);

	dt=clock()-t;
	printf("%d %fms\n", i, (dt*1000.0)/CLOCKS_PER_SEC);

	printf("hashtst %d\n", hash_str("foobarbaz"));
}
#endif
