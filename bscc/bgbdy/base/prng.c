#include <bgbgc.h>
#include <bgbdy.h>

volatile u32 bgbrng_mine[32];
int bgbrng_loop_start=0;

#ifdef MSVC

#ifdef X86_64
BGBDY_API u64 bgbrng_rdtsc()
{
	static void *fp=NULL;
	u64 v;

	if(!fp)
	{
		dyllAsmBegin();
		dyllAsmPrint("rdtsc\n");
		dyllAsmPrint("mov [rcx+0], eax\n");
		dyllAsmPrint("mov [rcx+4], edx\n");
		dyllAsmPrint("ret\n");
		fp=dyllAsmEnd();
		if(!fp)fp=UNDEFINED;
	}

	if(fp==UNDEFINED)
		return(0);
	((void(*)(u64 *))fp)(&v);

	return(v);
}
#endif

#ifdef X86
BGBDY_API u64 bgbrng_rdtsc()
{
	static void *fp=NULL;
	u64 v;

	if(!fp)
	{
		dyllAsmBegin();
		dyllAsmPrint("rdtsc\n");
		dyllAsmPrint("ret\n");
		fp=dyllAsmEnd();
		if(!fp)fp=UNDEFINED;
	}

	if(fp==UNDEFINED)
		return(0);
	v=((s64(*)())fp)();

	return(v);
}
#endif

#endif

#ifndef MSVC

#ifdef __i386__
BGBDY_API u64 bgbrng_rdtsc()
{
	u64 x;
	__asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));
	return(x);
}
#endif

#ifdef __x86_64__
BGBDY_API u64 bgbrng_rdtsc()
{
	u32 hi, lo;
	__asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
	return((((u64)hi)<<32)|lo);
}
#endif

#ifdef __powerpc__
BGBDY_API u64 bgbrng_rdtsc()
{
	u32 hi, lo, tmp;
	__asm__ volatile(
		"0:\n"
		"\tmftbu %0\n"
		"\tmftb  %1\n"
		"\tmftbu %2\n"
		"\tcmpw  %2,%0\n"
		"\tbne   0b\n"
		: "=r"(upper),"=r"(lower),"=r"(tmp)
		);
	return((((u64)hi)<<32)|lo);
}
#endif

#ifdef __arm__
BGBDY_API u64 bgbrng_rdtsc()
{
	u32 hi, lo;
//	__asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
	hi=clock(); lo=hi;
	return((((u64)hi)<<32)|lo);
}
#endif

#endif

void bgbrng_minemult()
{
	u64 h;
	int i;

	h=0;
	for(i=0; i<32; i++)
	{
//		h+=((u64)bgbrng_mine[i])*2147483647;
		h+=((u64)bgbrng_mine[i])*4294967291LL;
		bgbrng_mine[i]=(u32)h;
		h>>=32;
	}

	for(i=0; i<32; i++)
	{
		if(!h)break;
		h+=bgbrng_mine[i];
		bgbrng_mine[i]=(u32)h;
		h>>=32;
	}
}

void bgbrng_mineupdate()
{
	u64 g, h;

#if 0
	h=bgbrng_rdtsc();
	bgbrng_mine[0]^=(u32)h;
	bgbrng_mine[1]^=(u32)(h>>32);
#endif

#if 1
	g=bgbrng_rdtsc();
	h=((u64)bgbrng_mine[0])+((u32)g);
	bgbrng_mine[0]=(u32)h;
	h=(h>>32)+((u64)bgbrng_mine[1])+((u32)(g>>32));
	bgbrng_mine[1]=(u32)h;
	bgbrng_mine[2]+=(u32)(h>>32);
#endif

	bgbrng_minemult();
}

void bgbrng_minedump()
{
	FILE *fd;

	fd=fopen("randbits.bin", "wb");
	if(!fd)return;
	fwrite((void *)bgbrng_mine, 32, sizeof(u32), fd);
	fclose(fd);
}

void bgbrng_mineload()
{
	FILE *fd;

	fd=fopen("randbits.bin", "rb");
	if(!fd)return;
	fread((void *)bgbrng_mine, 32, sizeof(u32), fd);
	fclose(fd);
}

int bgbrng_loop(void *p)
{
	static int cnt=0;

	bgbrng_loop_start=1;
	while(1)
	{
		bgbrng_mineupdate();
		thSleep(100);

		if((cnt++)>=100)
		{
			bgbrng_minedump();
			cnt-=100;
		}
	}
	return(0);
}

int bgbrng_init()
{
	static int init=0;
	int i;

	if(init)return(0);
	init=1;

	printf("RNG Init\n");

	bgbrng_mineload();

	srand(bgbrng_rdtsc());
	for(i=0; i<32; i++)
		bgbrng_mine[i]^=(rand()<<16)|rand();

	for(i=0; i<32; i++)
	{
		bgbrng_mineupdate();
		thSleep(0);
	}

	bgbrng_minedump();

	thThread(bgbrng_loop, NULL);
	return(0);
}

BGBDY_API u32 bgbrng_genvalue()
{
	u64 h;
	int i;

	bgbrng_init();
	bgbrng_mineupdate();

	h=0;
	for(i=0; i<32; i++)
//		h+=((u64)bgbrng_mine[i])*2147483647;
//		h^=((u64)bgbrng_mine[i])*2147483647;
		h^=((u64)bgbrng_mine[i])*4294967291LL;
//	return(h);
	return(h>>32);
}

BGBDY_API u64 bgbrng_genvalue64()
{
	u64 h;
	h=bgbrng_genvalue();
	h=(h<<32)|bgbrng_genvalue();
	return(h);
}

BGBDY_API u32 bgbrng_genvalue_fast()
{
	static u64 h=0;
	static int i=0;

	if(!i)
	{
		h=bgbrng_genvalue64();
		i=256;
	}

	h*=4294967291LL;
	i--;

	return(h>>32);
}


iv128_t bgbrng_mulM107(iv128_t a)
{
	//000007FF FFFFFFFF FFFFFFFF FFFFFFFF
	static u64 m0=0xFFFFFFFF, m1=0x000007FF;
	iv128_t c;
	u64 li;

	li=a.a*m0; c.a=li;
	li=(li>>32) + a.a*m0 + a.b*m0; c.b=li;
	li=(li>>32) + a.a*m0 + a.b*m0 + a.c*m0; c.c=li;
	li=(li>>32) + a.a*m1 + a.b*m0 + a.c*m0 + a.d*m0; c.d=li;
	return(c);
}

iv128_t bgbrng_mulP96(iv128_t a)
{
	//000007FF FFFFFFFF FFFFFFFF FFFFFFFF
	//79228162514231123546292096409
	//281474976710597 4294967291 65521=
	//79210028534740905188068355767
	//FF8A000000000D99
	static u64 ma=0x0126EEB7, mb=0x00100375, mc=0xFFF0FFFB, md=0x00000000;
	iv128_t c;
	u64 li;

	li=a.a*ma; c.a=li;
	li=(li>>32) + a.a*mb + a.b*ma; c.b=li;
	li=(li>>32) + a.a*mc + a.b*mb + a.c*ma; c.c=li;
	li=(li>>32) + a.a*md + a.b*mc + a.c*mb + a.d*ma; c.d=li;
	return(c);
}

BGBDY_API int bgbrng_rand(iv128_t *a)
{
//	*a=bgbrng_mulM107(*a);
	*a=bgbrng_mulP96(*a);
	return(a->d);
}


//random value between 0 and 1
BGBDY_API double bgbrng_randf()
{
	double f;

	f=((double)(bgbrng_genvalue_fast()&0xFFFFFF))/16777215.0;
	return(f);
}

//random value between -1 and 1
BGBDY_API double bgbrng_randg()
{
	return(2*bgbrng_randf()-1);
}

BGBDY_API float bgbrng_ssqrt(float f)
	{ if(f<0)return(-sqrt(-f)); return(sqrt(f)); }
BGBDY_API float bgbrng_ssqr(float f)
	{ if(f<0)return(-(f*f)); return(f*f); }

BGBDY_API float bgbrng_nfrand()
	{ return(bgbrng_randf()); }
BGBDY_API float bgbrng_sfrand()
	{ return(bgbrng_randf()*2-1.0); }
//float bgbrng_sqrrand()
//	{ return(sqrt(bgbrng_randf()*65536)/256); }
//float bgbrng_ssqrrand()
//	{ return(bgbrng_ssqrt(bgbrng_sfrand()*65536)/256); }

BGBDY_API float bgbrng_sqrrand()
	{ return(bgbrng_ssqr(bgbrng_nfrand())); }
BGBDY_API float bgbrng_ssqrrand()
	{ return(bgbrng_ssqr(bgbrng_sfrand())); }

BGBDY_API float bgbrng_logrand()
	{ return((log(bgbrng_nfrand()*65535+1)/log(2))/16); }


BGBDY_API void bgbrng_ciph0_swap(byte *map, int i, int j)
	{ int k; k=map[i]; map[i]=map[j]; map[j]=k; }

BGBDY_API void bgbrng_ciph0_swap2(byte *map, byte *imap, int i, int j)
{
	int k;
	k=map[i]; map[i]=map[j]; map[j]=k;
	imap[map[i]]=i; imap[map[j]]=j;
}

BGBDY_API int bgbrng_ciph0_kindex(iv128_t *key)
{
	int k;

	*key=bgbrng_mulM107(*key);
	k=((key->d>>16)^(key->d>>24))&255;
	return(k);
}

BGBDY_API void bgbrng_ciph0_enc(byte *ibuf, byte *obuf, int sz,
	iv128_t *key)
{
	byte map[256], imap[256];
	int i, j, k;

	for(i=0; i<256; i++)map[i]=i;
	for(i=0; i<256; i++)imap[i]=i;

	for(i=0; i<256; i++)
	{
		j=bgbrng_ciph0_kindex(key);
		bgbrng_ciph0_swap2(imap, map, i, j);
	}

	for(i=0; i<sz; i++)
	{
		j=ibuf[i];
		obuf[i]=map[j];
		k=bgbrng_ciph0_kindex(key);
		bgbrng_ciph0_swap2(imap, map, j, k);
	}
}

BGBDY_API void bgbrng_ciph0_dec(byte *ibuf, byte *obuf, int sz,
	iv128_t *key)
{
	byte map[256];
	int i, j, k;

	for(i=0; i<256; i++)map[i]=i;

	for(i=0; i<256; i++)
	{
		j=bgbrng_ciph0_kindex(key);
		bgbrng_ciph0_swap(map, i, j);
	}

	for(i=0; i<sz; i++)
	{
		j=map[ibuf[i]];
		obuf[i]=j;
		k=bgbrng_ciph0_kindex(key);
		bgbrng_ciph0_swap(map, j, k);
	}
}

BGBDY_API iv128_t bgbrng_ciph0_genkey(char *str)
{
//	static u64 ma=0x0126EEB7, mb=0x00100375, mc=0xFFF0FFFB, md=0x31415927;
	iv128_t v;
	char *s;

//	v.a=ma; v.b=mb; v.c=mc; v.d=md;
	v.a=0; v.b=0; v.c=0; v.d=0;

	s=str;
	while(*s)
	{
		v.a+=*s++;
		v=bgbrng_mulM107(v);
		v.a+=v.d; v.b+=v.a; v.c+=v.b;
	}
	return(v);
}
