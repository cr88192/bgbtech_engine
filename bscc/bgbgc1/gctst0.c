#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <bgbgc.h>

void *temp[4096];
s64 hash;

int garble(byte *p, int sz)
{
	if(!p)return(-1);
	while(sz--)
	{
		hash*=65521;
		*p++=(hash>>56)&0xFF;
	}
	return(0);
}

int heapstress()
{
	void *p;
	int i, j, k, t, it;

	hash=(s64)(&heapstress);
	for(i=0; i<1024; i++)hash*=65521;

	it=clock();
	while(1)
	{
		t=clock();
		if((t-it)<0)break;
		if((t-it)>(240*CLOCKS_PER_SEC))break;

		i=rand();
//		i&=4095;
//		i&=65535;
		i&=8191;

		p=gcalloc(i);
		garble(p, i);

		i=rand();
		if(!(i&255))temp[((i*251)>>8)&0xFFF]=p;
	}
}

int tst_minilock()
{
	static int seq=1;
	static volatile int lock;
	int i;

	i=seq++;
	while(1)
	{
		while(lock)thSleep(0);
		lock=seq;
//		thSleep(0);
		if(lock==seq)break;
	}
	lock=0;
}

int tst_mutex()
{
	void *mtx;
	int i, j, t, dt;

	t=clock();
//	for(i=0; i<(1<<20); i++)
//		bgbrng_genvalue();
		rand();
	dt=clock()-t;
	printf("%fs %d\n", dt/((double)CLOCKS_PER_SEC), dt);

	mtx=thMutex();

	t=clock();
	for(i=0; i<(1<<20); i++)
	{
		thLockMutex(mtx);
//		bgbrng_genvalue();
		rand();
		thUnlockMutex(mtx);
	}
	dt=clock()-t;
	printf("%fs %d\n", dt/((double)CLOCKS_PER_SEC), dt);

#if 0
	thInitQInt(&j);
	t=clock();
	for(i=0; i<(1<<20); i++)
	{
//		thLockMutex(mtx);
//		bgbrng_genvalue();
//		thSleep(0);
//		tst_minilock();
		thLockQInt(&j);
		rand();
		thUnlockQInt(&j);
//		thUnlockMutex(mtx);
	}
	dt=clock()-t;
	printf("%fs %d\n", dt/((double)CLOCKS_PER_SEC), dt);
#endif
}

int main_i(void *p)
{
	heapstress();

	gc_deinit();
	return(0);
}

int main(int argc, char *argv[])
{
//	dy_init2(main_i);
	gc_init2(main_i, NULL);
	return(0);

#if 0
	gc_init(&argc);

//	tst_dy0();
//	tst_dy1();

	heapstress();
//	entropytst();
//	rngtst();
//	tst_largeint();

//	tst_mutex();

	return(0);
#endif
}
