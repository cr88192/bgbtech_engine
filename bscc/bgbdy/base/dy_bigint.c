/*
BigInt:
Internal representation is an array of 16-bit values.
The first value will give the length of the array, and the remainder give the
value. The representation will be 2s complement and in low-high order.

 */

#include <bgbgc.h>
#include <bgbdy.h>

u16 *dybi_ralloc(int sz)
{
	static u16 *buf=NULL;
	static int rov;
	u16 *t;

	if(!buf)buf=malloc(16384*sizeof(u16));
	if((rov+sz)>16384)rov=0;

	t=buf+rov;
	rov+=sz;
	return(t);
}

void dybi_AddBI(u16 *dst, u16 *srca, u16 *srcb, int sz)
{
	int i, j;

	j=0;
	for(i=0; i<sz; i++)
	{
		j=j+(srca[i]+srcb[i]);
		dst[i]=(u16)j;
		j=j>>16;
	}
}

void dybi_SubBI(u16 *dst, u16 *srca, u16 *srcb, int sz)
{
	u32 vi;
	int i;

	vi=0;
	for(i=0; i<sz; i++)
	{
		vi=(vi+srca[i])-srcb[i];
		dst[i]=(u16)vi;
		vi=vi>>16;
	}
}

void dybi_ScaleBI(u16 *dst, u16 *src, u16 sc, int sz)
{
	u32 vi;
	int i;

	vi=0;
	for(i=0; i<sz; i++)
	{
		vi=vi+(src[i]*sc);
		dst[i]=(u16)vi;
		vi=vi>>16;
	}
}

void dybi_Scale1BI(u16 *dst, u16 *src, u16 sc, int sz)
{
	u32 vi;
	int i;

	vi=0;
	for(i=0; i<sz; i++)
	{
		vi=vi+(src[i]*sc);
		dst[i]=(u16)vi;
		vi=vi>>16;
	}
	dst[i]=(u16)vi;
}

void dybi_MulBI(u16 *dst, u16 *srca, u16 *srcb, int sz, int sza, int szb)
{
	u16 *bt;
	int i, j;

	bt=dybi_ralloc(sz);

	for(i=0; i<sz; i++)dst[i]=0x0000;
	for(j=0; j<sz; j++)bt[j]=0;

	for(i=0; i<szb; i++)
	{
		dybi_Scale1BI(bt, srca, srcb[i], sza);
		dybi_AddBI(dst+i, dst+i, bt, sz-i);
	}
}


int dySizeBigInt(dyt p)
	{ return(*(u16 *)p); }

int dyUnpackBigInt(dyt p, u16 *buf, int sz)
{
	u16 *s, *t;
	int i, j, n;

	s=(u16 *)p; n=*s++;
	if(n>sz)return(-1);
	t=buf; i=n;
	while(i--)*t++=*s++;

	i=sz-n;
	if((*(s-1))&0x8000) { while(i--)*t++=0xFFFF; }
		else { while(i--)*t++=0x0000; }

	return(n);
}

dyt dyPackBigInt(u16 *buf, int sz)
{
	u16 *b, *s, *t;
	int i, n;

	s=buf+(sz-1);
	while((s>buf) && ((*s==0xFFFF)||(*s==0x0000)))s--;
	n=s-buf; if(!n)n++;

	b=gctatomic("_bigint_t", (n+1)*sizeof(u16));
	t=b; *t++=n;
	s=buf; i=n;
	while(i--)*t++=*s++;
	return((dyt)b);
}

dyt dyAddBigInt(dyt a, dyt b)
{
	u16 *ba, *bb, *bc;
	int i, j, sz;

	i=dySizeBigInt(a);
	j=dySizeBigInt(b);
	sz=((i>j)?i:j)+1;

	ba=dybi_ralloc(sz);
	bb=dybi_ralloc(sz);
	bc=dybi_ralloc(sz);
	dyUnpackBigInt(a, ba, sz);
	dyUnpackBigInt(b, bb, sz);
	dybi_AddBI(bc, ba, bb, sz);

	return(dyPackBigInt(bc, sz));
}

dyt dySubBigInt(dyt a, dyt b)
{
	u16 *ba, *bb, *bc;
	int i, j, sz;

	i=dySizeBigInt(a);
	j=dySizeBigInt(b);
	sz=((i>j)?i:j)+1;

	ba=dybi_ralloc(sz);
	bb=dybi_ralloc(sz);
	bc=dybi_ralloc(sz);
	dyUnpackBigInt(a, ba, sz);
	dyUnpackBigInt(b, bb, sz);
	dybi_SubBI(bc, ba, bb, sz);

	return(dyPackBigInt(bc, sz));
}

dyt dyMulBigInt(dyt a, dyt b)
{
	u16 *ba, *bb, *bc;
	int i, j, sz, sza, szb;

	sza=dySizeBigInt(a);
	szb=dySizeBigInt(b);
	sz=sza+szb;

	ba=dybi_ralloc(sza);
	bb=dybi_ralloc(szb);
	bc=dybi_ralloc(sz);
	dyUnpackBigInt(a, ba, sza);
	dyUnpackBigInt(b, bb, szb);
	dybi_MulBI(bc, ba, bb, sz, sza, szb);

	return(dyPackBigInt(bc, sz));
}
