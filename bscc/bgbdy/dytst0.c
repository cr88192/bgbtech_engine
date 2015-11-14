#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <bgbgc.h>
#include <bgbdy.h>

void *temp[4096];
s64 hash;

int garble(byte *p, int sz)
{
	while(sz--)
	{
		hash*=65521;
		*p++=(hash>>56)&0xFF;
	}
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
		if((t-it)>(30*CLOCKS_PER_SEC))break;

		i=rand();
//		i&=4095;
		i&=65535;

		p=gcalloc(i);
//		garble(p, i);

		i=rand();
		if(!(i&255))temp[((i*251)>>8)&0xFFF]=p;
	}
}

int entropytst()
{
	static int hist[4096];
	static int srt[4096];
	u64 *ub;
	FILE *fd;
	void *p;
	int i, j, k, it, t, lc, hc;
	u64 li, lj;
	double min, max, mid, lmid, hmid;
	double f, g;

#if 0
	li=bgbrng_rdtsc(); t=1<<22;
	for(i=0; i<t; i++)
	{
		thSleep(0);
		lj=bgbrng_rdtsc();
		j=lj-li; li=lj;

		if(j<0) { hist[4095]++; continue; }
		if(j>4093) { hist[4094]++; continue; }
		hist[j]++;
	}

	f=1.0/t;
	fd=fopen("hist0.txt", "wt");
	for(i=0; i<(4096/8); i++)
	{
		for(j=0; j<8; j++)
		{
//			fprintf(fd, "%.4f ", hist[i*8+j]*f);
			fprintf(fd, "%7d ", hist[i*8+j]);
		}
		fprintf(fd, "\n");
	}
	fclose(fd);

	f=1.0/t;
	fd=fopen("hist0.pcm", "wb");
	for(i=0; i<4096; i++)
	{
		j=hist[i]*(16384.0/t);
		fputc(j&0xFF, fd);
		fputc((j>>8)&0xFF, fd);
	}
	fclose(fd);

	for(i=0; i<4096; i++)srt[i]=i;

	for(i=0; i<4096; i++)
		for(j=i+1; j<4096; j++)
			if(hist[srt[j]]>hist[srt[i]])
				{ k=srt[i]; srt[i]=srt[j]; srt[j]=k; }

	for(i=0; i<16; i++)
	{
		lc=0; hc=0;
		for(j=0; j<=i; j++)lc+=hist[srt[j]];
		for(j=i+1; j<4096; j++)hc+=hist[srt[j]];

		printf("%2d: %7d %d/%d=%.4f,%.4f\n", i, hist[srt[i]],
			lc, hc, ((double)lc)/hc, ((double)hc)/lc);
	}

#endif

#if 0
	ub=malloc((1<<24)*sizeof(u64));
	li=bgbrng_rdtsc();
	for(i=0; i<(1<<24); i++)
	{
		thSleep(0);
		lj=bgbrng_rdtsc();

		if(lj<li)
			{ li=lj; i--; continue; }

		ub[i]=lj-li; li=lj;
	}

	min=1000000000.0; max=0; mid=0;
	for(i=0; i<(1<<24); i++)
	{
		li=ub[i];
		if(li<min)min=li;
		if(li>max)max=li;
		mid+=li;
	}
	mid/=1<<24;

	printf("%f %f %f\n", min, max, mid);

	lmid=0; hmid=0; lc=0; hc=0;
	for(i=0; i<(1<<24); i++)
	{
		li=ub[i];
		if(li<mid) { lmid+=li; lc++; }
		if(li>mid) { hmid+=li; hc++; }
	}
	lmid/=lc;
	hmid/=hc;

	printf("%f %f %f\n", lmid, mid, hmid);
	printf("%f %f\n", lc/(double)(1<<24), hc/(double)(1<<24));
#endif

#if 0
	fd=fopen("rdtsc0.pcm", "wb");
	for(i=0; i<(1<<24); i++)
	{
		li=ub[i];

//		f=(li-lmid)*(4096/lmid);
//		if(f<-32767)f=-32767;
//		if(f>32767)f=32767;

		f=li;
		if(f<0)continue;
		if(f>lmid)continue;

		j=f;
		fputc(j&0xFF, fd);
		fputc((j>>8)&0xFF, fd);
	}
	fclose(fd);
#endif
}

int rngtst()
{
	FILE *fd;
	int i, j, k;

#if 0
	for(i=0; i<16; i++)
	{
		j=bgbrng_genvalue();
		printf("%08X ", j);
	}
#endif

#if 0
	fd=fopen("rand0.bin", "wb");
	for(i=0; i<(1<<24); i++)
	{
		j=bgbrng_genvalue();
		fputc(j&0xFF, fd);
	}
	fclose(fd);
#endif
}

int tst_largeint()
{
	iv128_t val;

//	val=atoxl_iv("79228162514231123546292096409");
	val=atoxl_iv("79210028534740905188068355767");
	printf("%08X %08X %08X %08X\n", val.d, val.c, val.b, val.a);
}

int tst_dy0()
{
	dyt p, c, l;
	int i, j, k;
	double f, g, h;
	float f1;

	p=dyfloat(M_PI);
	f=M_PI;
	f1=M_PI;

	for(i=0; i<256; i++)
	{
		g=dyfloatv(p);
		printf("%f\t%f\t%f\t%f\t%f\n", f, f1, g, f-f1, f-g);

		h=1.06000+sin((M_PI/8)*i)*0.5;

		f*=h;
		f1*=h;
		g*=h;
		p=dyfloat(g);
	}
}

int tst_dy1()
{
	dyt p;

	p=dycons(dysymbol("test"), dycons(dyint(3), dyint(4)));

//	p=dylist3s("test", dyint(3), dyint(4));
	p=dylist3s("test", dyfloat(3), dyint(4));

	dyPrint(p);

	*(int *)-1=-1;
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
	dyt mtx;
	int i, j, t, dt;

	t=clock();
	for(i=0; i<(1<<20); i++)
		bgbrng_genvalue();
//		rand();
	dt=clock()-t;
	printf("%fs %d\n", dt/((double)CLOCKS_PER_SEC), dt);

	mtx=thMutex();

	t=clock();
	for(i=0; i<(1<<20); i++)
	{
		thLockMutex(mtx);
		bgbrng_genvalue();
//		rand();
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

#if 0
int tst_sql()
{
	dyt db;

	db=dyllSqlOpenDB(NULL);

	dyllSqlExecPrint(db, "create table foo "
		"(name text, desc text, idx integer);");
	dyllSqlExecPrint(db, "insert into foo values('Foo', 'Example A', 9);");
	dyllSqlExecPrint(db, "insert into foo values('Bar', 'Example B', 10);");
	dyllSqlExecPrint(db, "insert into foo values('Baz', 'Example C', 11);");
	dyllSqlExecPrintTable(db, "select * from foo;");

	dyllSqlCloseDB(db);
}
#endif

int bench_typechk_val(dyt p)
{
	int i, t, dt, n;
	double dtf;

	t=clock();
	n=100000000;
	for(i=0; i<n; i++)
		{ dytypep(p, "_foo_bar_t"); }
	dt=clock()-t;
	dtf=dt/((double)CLOCKS_PER_SEC);
	printf("%fs %d %fns\n", dtf, dt, (dtf/n)*1000000000.0);
}

int bench_typechk()
{
	dyt p;

	printf("Typecheck speed-tests\n");

	p=dyint(3);
	printf("Fixnum: %p\n", p);
	bench_typechk_val(p);

	p=dycons(NULL, NULL);
	printf("Cons: %p\n", p);
	bench_typechk_val(p);

	p=gctalloc("_baz_t", 256);
	printf("Heap: %p\n", p);
	bench_typechk_val(p);

	p=(dyt)(&p);
	printf("Non-heap: %p\n", p);
	bench_typechk_val(p);
}

int main(int argc, char *argv[])
{
	dy_init(&argc);

//	tst_dy0();
//	tst_dy1();

//	heapstress();
//	entropytst();
//	rngtst();
//	tst_largeint();

//	tst_mutex();

//	tst_sql();

	bench_typechk();

	return(0);
}
