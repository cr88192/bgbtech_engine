#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char byte;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef struct {
u32 a, b, c, d;
}iv128_t;

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

void bgbrng_ciph0_swap(byte *map, int i, int j)
	{ int k; k=map[i]; map[i]=map[j]; map[j]=k; }

void bgbrng_ciph0_swap2(byte *map, byte *imap, int i, int j)
{
	int k;
	k=map[i]; map[i]=map[j]; map[j]=k;
	imap[map[i]]=i; imap[map[j]]=j;
}

int bgbrng_ciph0_kindex(iv128_t *key)
{
	int k;

	*key=bgbrng_mulM107(*key);
	k=((key->d>>16)^(key->d>>24))&255;
	return(k);
}

void bgbrng_ciph0_enc(byte *ibuf, byte *obuf, int sz,
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

void bgbrng_ciph0_dec(byte *ibuf, byte *obuf, int sz,
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

iv128_t bgbrng_ciph0_genkey(char *str)
{
//	static u64 ma=0x0126EEB7, mb=0x00100375, mc=0xFFF0FFFB, md=0x31415927;
	iv128_t v;
	char *s;
	int i;

//	v.a=ma; v.b=mb; v.c=mc; v.d=md;
	v.a=0; v.b=0; v.c=0; v.d=0;

	s=str;
	while(*s)
	{
		v.a+=*s++;
		v=bgbrng_mulM107(v);
		v.a+=v.d; v.b+=v.a; v.c+=v.b;
	}

//	for(i=0; i<256; i++)
//	{
//		v=bgbrng_mulM107(v);
//		v.a+=v.d;
//	}

	return(v);
}

void bgbrng_ciph0_dummy(byte *ibuf, byte *obuf, int sz,
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
//		obuf[i]=j;
		k=bgbrng_ciph0_kindex(key);
		obuf[i]=k;
		bgbrng_ciph0_swap(map, j, k);
	}
}

int main()
{
	byte buf[256];
	byte tbuf[256];
	byte buf1[256];
	iv128_t v, tv;
	int i;

	for(i=0; i<256; i++)
		buf[i]=rand();

	strcpy(buf, "this is a test\n");

	v=bgbrng_ciph0_genkey("Atestkey");
	printf("%08X %08X %08X %08X\n", v.d, v.c, v.b, v.a);
	v=bgbrng_ciph0_genkey("Btestkey");
	printf("%08X %08X %08X %08X\n", v.d, v.c, v.b, v.a);
	v=bgbrng_ciph0_genkey("Ctestkey");
	printf("%08X %08X %08X %08X\n", v.d, v.c, v.b, v.a);
	v=bgbrng_ciph0_genkey("Dtestkey");
	printf("%08X %08X %08X %08X\n", v.d, v.c, v.b, v.a);

	v=bgbrng_ciph0_genkey("testkey");
	printf("%08X %08X %08X %08X\n", v.d, v.c, v.b, v.a);

	tv=v;
	bgbrng_ciph0_dummy(buf, tbuf, 256, &tv);

	printf("v: ");
	for(i=0; i<76/2; i++)
		printf("%02X", tbuf[i]);
	printf("\n");

	tv=v;
	bgbrng_ciph0_enc(buf, tbuf, 256, &tv);

	tv=v;
	bgbrng_ciph0_dec(tbuf, buf1, 256, &tv);

	printf("I: ");
	for(i=0; i<76/2; i++)
		printf("%02X", buf[i]);
	printf("\n");

	printf("C: ");
	for(i=0; i<76/2; i++)
		printf("%02X", tbuf[i]);
	printf("\n");

	printf("O: ");
	for(i=0; i<76/2; i++)
		printf("%02X", buf1[i]);
	printf("\n");
}

#if 0
int main()
{
	byte map[256], imap[256];
	int i, j, k, v;

	for(i=0; i<256; i++)map[i]=i;
	for(i=0; i<256; i++)imap[i]=i;

	for(i=0; i<256; i++)
	{
		j=rand()&255;
		bgbrng_ciph0_swap2(imap, map, i, j);
	}

	for(i=0; i<256; i++)
	{
		printf("%d: %d, %d\n", i, map[i], imap[i]);
	}


	for(i=0; i<16; i++)
	{
		j=rand()&255;
		k=rand()&255;

		v=map[j];
		bgbrng_ciph0_swap2(imap, map, j, k);

		printf("%d->%d->%d\n", j, v, imap[v]);
	}
}
#endif

