#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libvecmath.h>

//#ifndef X86
#if 1

void WInt_Zero64(u16 *a)
	{ memset(a, 0, 8); }
void WInt_Zero128(u16 *a)
	{ memset(a, 0, 16); }
void WInt_Zero256(u16 *a)
	{ memset(a, 0, 32); }
void WInt_Zero512(u16 *a)
	{ memset(a, 0, 64); }
void WInt_Zero1024(u16 *a)
	{ memset(a, 0, 128); }

void WInt_Copy64(u16 *a, u16 *b)
	{ memcpy(b, a, 8); }
void WInt_Copy128(u16 *a, u16 *b)
	{ memcpy(b, a, 16); }
void WInt_Copy256(u16 *a, u16 *b)
	{ memcpy(b, a, 32); }
void WInt_Copy512(u16 *a, u16 *b)
	{ memcpy(b, a, 64); }
void WInt_Copy1024(u16 *a, u16 *b)
	{ memcpy(b, a, 128); }
#endif

void WInt_Copy64S(u16 *a, u16 *b, int sg)
	{ memcpy(b, a, 8); if(sg)WInt_Neg64(b); }
void WInt_Copy128S(u16 *a, u16 *b, int sg)
	{ memcpy(b, a, 16); if(sg)WInt_Neg128(b); }
void WInt_Copy256S(u16 *a, u16 *b, int sg)
	{ memcpy(b, a, 32); if(sg)WInt_Neg256(b); }
void WInt_Copy512S(u16 *a, u16 *b, int sg)
	{ memcpy(b, a, 64); if(sg)WInt_Neg512(b); }
void WInt_Copy1024S(u16 *a, u16 *b, int sg)
	{ memcpy(b, a, 128); if(sg)WInt_Neg1024(b); }

//#ifndef X86

#if 1
void WInt_Add64(u16 *a, u16 *b, u16 *c)
{
	unsigned int i;

	i=(int)a[0]+b[0];		c[0]=i;
	i=(i>>16)+((int)a[1]+b[1]);	c[1]=i;
	i=(i>>16)+((int)a[2]+b[2]);	c[2]=i;
	i=(i>>16)+((int)a[3]+b[3]);	c[3]=i;
}

void WInt_Sub64(u16 *a, u16 *b, u16 *c)
{
	unsigned int i;

	i=(int)a[0]-b[0];		c[0]=i;
	i=(i>>16)+((int)a[1]-b[1]);	c[1]=i;
	i=(i>>16)+((int)a[2]-b[2]);	c[2]=i;
	i=(i>>16)+((int)a[3]-b[3]);	c[3]=i;
}

void WInt_Add128(u16 *a, u16 *b, u16 *c)
{
	unsigned int i;

	i=(int)a[0]+b[0];		c[0]=i;
	i=(i>>16)+((int)a[1]+b[1]);	c[1]=i;
	i=(i>>16)+((int)a[2]+b[2]);	c[2]=i;
	i=(i>>16)+((int)a[3]+b[3]);	c[3]=i;
	i=(i>>16)+((int)a[4]+b[4]);	c[4]=i;
	i=(i>>16)+((int)a[5]+b[5]);	c[5]=i;
	i=(i>>16)+((int)a[6]+b[6]);	c[6]=i;
	i=(i>>16)+((int)a[7]+b[7]);	c[7]=i;
}

void WInt_Sub128(u16 *a, u16 *b, u16 *c)
{
	unsigned int i;

	i=(int)a[0]-b[0];		c[0]=i;
	i=(i>>16)+((int)a[1]-b[1]);	c[1]=i;
	i=(i>>16)+((int)a[2]-b[2]);	c[2]=i;
	i=(i>>16)+((int)a[3]-b[3]);	c[3]=i;
	i=(i>>16)+((int)a[4]-b[4]);	c[4]=i;
	i=(i>>16)+((int)a[5]-b[5]);	c[5]=i;
	i=(i>>16)+((int)a[6]-b[6]);	c[6]=i;
	i=(i>>16)+((int)a[7]-b[7]);	c[7]=i;
}

#endif

void WInt_Add256(u16 *a, u16 *b, u16 *c)
{
	unsigned int i;

	i=(int)a[0]+b[0];		c[0]=i;
	i=(i>>16)+((int)a[1]+b[1]);	c[1]=i;
	i=(i>>16)+((int)a[2]+b[2]);	c[2]=i;
	i=(i>>16)+((int)a[3]+b[3]);	c[3]=i;
	i=(i>>16)+((int)a[4]+b[4]);	c[4]=i;
	i=(i>>16)+((int)a[5]+b[5]);	c[5]=i;
	i=(i>>16)+((int)a[6]+b[6]);	c[6]=i;
	i=(i>>16)+((int)a[7]+b[7]);	c[7]=i;

	i=(i>>16)+((int)a[8]+b[8]);	c[8]=i;
	i=(i>>16)+((int)a[9]+b[9]);	c[9]=i;
	i=(i>>16)+((int)a[10]+b[10]);	c[10]=i;
	i=(i>>16)+((int)a[11]+b[11]);	c[11]=i;
	i=(i>>16)+((int)a[12]+b[12]);	c[12]=i;
	i=(i>>16)+((int)a[13]+b[13]);	c[13]=i;
	i=(i>>16)+((int)a[14]+b[14]);	c[14]=i;
	i=(i>>16)+((int)a[15]+b[15]);	c[15]=i;
}

void WInt_Sub256(u16 *a, u16 *b, u16 *c)
{
	unsigned int i;

	i=(int)a[0]-b[0];		c[0]=i;
	i=(i>>16)+((int)a[1]-b[1]);	c[1]=i;
	i=(i>>16)+((int)a[2]-b[2]);	c[2]=i;
	i=(i>>16)+((int)a[3]-b[3]);	c[3]=i;
	i=(i>>16)+((int)a[4]-b[4]);	c[4]=i;
	i=(i>>16)+((int)a[5]-b[5]);	c[5]=i;
	i=(i>>16)+((int)a[6]-b[6]);	c[6]=i;
	i=(i>>16)+((int)a[7]-b[7]);	c[7]=i;
	i=(i>>16)+((int)a[8]-b[8]);	c[8]=i;
	i=(i>>16)+((int)a[9]-b[9]);	c[9]=i;
	i=(i>>16)+((int)a[10]-b[10]);	c[10]=i;
	i=(i>>16)+((int)a[11]-b[11]);	c[11]=i;
	i=(i>>16)+((int)a[12]-b[12]);	c[12]=i;
	i=(i>>16)+((int)a[13]-b[13]);	c[13]=i;
	i=(i>>16)+((int)a[14]-b[14]);	c[14]=i;
	i=(i>>16)+((int)a[15]-b[15]);	c[15]=i;
}

void WInt_Add512(u16 *a, u16 *b, u16 *c)
{
	unsigned int t;
	int i;
	t=0;
	for(i=0; i<32; i++)	{ t=(t>>16)+((int)a[i]+b[i]); c[i]=t; }
}

void WInt_Sub512(u16 *a, u16 *b, u16 *c)
{
	unsigned int t;
	int i;
	t=0;
	for(i=0; i<32; i++)	{ t=(t>>16)+((int)a[i]-b[i]); c[i]=t; }
}

void WInt_Add1024(u16 *a, u16 *b, u16 *c)
{
	unsigned int t;
	int i;
	t=0;
	for(i=0; i<64; i++)	{ t=(t>>16)+((int)a[i]+b[i]); c[i]=t; }
}

void WInt_Sub1024(u16 *a, u16 *b, u16 *c)
{
	unsigned int t;
	int i;
	t=0;
	for(i=0; i<64; i++)	{ t=(t>>16)+((int)a[i]-b[i]); c[i]=t; }
}

void WInt_Inc64(u16 *a)
{
	unsigned int t;

	t=a[0];	t++;	a[0]=t;
	t>>=16;	if(!t)return;	t+=a[1];	a[1]=t;
	t>>=16;	if(!t)return;	t+=a[2];	a[2]=t;
	t>>=16;	if(!t)return;	t+=a[3];	a[3]=t;
}

void WInt_Inc128(u16 *a)
{
	unsigned int t;

	t=a[0];	t++;	a[0]=t;
	t>>=16;	if(!t)return;	t+=a[1];	a[1]=t;
	t>>=16;	if(!t)return;	t+=a[2];	a[2]=t;
	t>>=16;	if(!t)return;	t+=a[3];	a[3]=t;
	t>>=16;	if(!t)return;	t+=a[4];	a[4]=t;
	t>>=16;	if(!t)return;	t+=a[5];	a[5]=t;
	t>>=16;	if(!t)return;	t+=a[6];	a[6]=t;
	t>>=16;	if(!t)return;	t+=a[7];	a[7]=t;
}

void WInt_Inc256(u16 *a)
{
	unsigned int t;

	t=a[0];	t++;	a[0]=t;
	t>>=16;	if(!t)return;	t+=a[ 1];	a[ 1]=t;
	t>>=16;	if(!t)return;	t+=a[ 2];	a[ 2]=t;
	t>>=16;	if(!t)return;	t+=a[ 3];	a[ 3]=t;
	t>>=16;	if(!t)return;	t+=a[ 4];	a[ 4]=t;
	t>>=16;	if(!t)return;	t+=a[ 5];	a[ 5]=t;
	t>>=16;	if(!t)return;	t+=a[ 6];	a[ 6]=t;
	t>>=16;	if(!t)return;	t+=a[ 7];	a[ 7]=t;
	t>>=16;	if(!t)return;	t+=a[ 8];	a[ 8]=t;
	t>>=16;	if(!t)return;	t+=a[ 9];	a[ 9]=t;
	t>>=16;	if(!t)return;	t+=a[10];	a[10]=t;
	t>>=16;	if(!t)return;	t+=a[11];	a[11]=t;
	t>>=16;	if(!t)return;	t+=a[12];	a[12]=t;
	t>>=16;	if(!t)return;	t+=a[13];	a[13]=t;
	t>>=16;	if(!t)return;	t+=a[14];	a[14]=t;
	t>>=16;	if(!t)return;	t+=a[15];	a[15]=t;
}

void WInt_Inc512(u16 *a)
{
	unsigned int t;
	int i;

	t=a[0];	t++;	a[0]=t;
	for(i=1; i<32; i++)
	{
		t>>=16;	if(!t)break;
		t+=a[i]; a[i]=t;
	}
}

void WInt_Inc1024(u16 *a)
{
	unsigned int t;
	int i;

	t=a[0];	t++;	a[0]=t;
	for(i=1; i<64; i++)
	{
		t>>=16;	if(!t)break;
		t+=a[i]; a[i]=t;
	}
}

int WInt_CmpEq64(u16 *a, u16 *b)
{
	if(a[3]!=b[3])return(0);
	if(a[2]!=b[2])return(0);
	if(a[1]!=b[1])return(0);
	if(a[0]!=b[0])return(0);
	return(1);
}

int WInt_CmpEq128(u16 *a, u16 *b)
{
	if(a[7]!=b[7])return(0);
	if(a[6]!=b[6])return(0);
	if(a[5]!=b[5])return(0);
	if(a[4]!=b[4])return(0);
	if(a[3]!=b[3])return(0);
	if(a[2]!=b[2])return(0);
	if(a[1]!=b[1])return(0);
	if(a[0]!=b[0])return(0);
	return(1);
}

int WInt_CmpEq256(u16 *a, u16 *b)
{
	if(!WInt_CmpEq128(a+8, b+8))return(0);
	return(WInt_CmpEq128(a, b));
}

int WInt_CmpEq512(u16 *a, u16 *b)
{
	if(!WInt_CmpEq256(a+16, b+16))return(0);
	return(WInt_CmpEq256(a, b));
}

int WInt_CmpEq1024(u16 *a, u16 *b)
{
	if(!WInt_CmpEq512(a+32, b+32))return(0);
	return(WInt_CmpEq512(a, b));
}

int WInt_CmpBE64(u16 *a, u16 *b)
{
	if(a[3]>b[3])return(0);
	if(a[2]>b[2])return(0);
	if(a[1]>b[1])return(0);
	if(a[0]>b[0])return(0);
	return(1);
}

int WInt_CmpBE128(u16 *a, u16 *b)
{
	if(!WInt_CmpBE64(a+4, b+4))return(0);
	return(WInt_CmpBE64(a, b));
}

int WInt_CmpBE256(u16 *a, u16 *b)
{
	if(!WInt_CmpBE128(a+8, b+8))return(0);
	return(WInt_CmpBE128(a, b));
}

int WInt_CmpBE512(u16 *a, u16 *b)
{
	if(!WInt_CmpBE256(a+16, b+16))return(0);
	return(WInt_CmpBE256(a, b));
}

int WInt_CmpBE1024(u16 *a, u16 *b)
{
	if(!WInt_CmpBE512(a+32, b+32))return(0);
	return(WInt_CmpBE512(a, b));
}


int WInt_CmpB64(u16 *a, u16 *b)
{
	if(a[3]>b[3])return(0);
	if(a[2]>b[2])return(0);
	if(a[1]>b[1])return(0);
	if(a[0]>=b[0])return(0);
	return(1);
}

int WInt_CmpB128(u16 *a, u16 *b)
{
	if(!WInt_CmpBE64(a+4, b+4))return(0);
	return(WInt_CmpB64(a, b));
}

int WInt_CmpB256(u16 *a, u16 *b)
{
	if(!WInt_CmpBE128(a+8, b+8))return(0);
	return(WInt_CmpB128(a, b));
}

int WInt_CmpB512(u16 *a, u16 *b)
{
	if(!WInt_CmpBE256(a+16, b+16))return(0);
	return(WInt_CmpB256(a, b));
}

int WInt_CmpB1024(u16 *a, u16 *b)
{
	if(!WInt_CmpBE512(a+32, b+32))return(0);
	return(WInt_CmpB512(a, b));
}

int WInt_CmpA64(u16 *a, u16 *b)
	{ return(!WInt_CmpBE64(a, b)); }
int WInt_CmpA128(u16 *a, u16 *b)
	{ return(!WInt_CmpBE128(a, b)); }
int WInt_CmpA256(u16 *a, u16 *b)
	{ return(!WInt_CmpBE256(a, b)); }
int WInt_CmpA512(u16 *a, u16 *b)
	{ return(!WInt_CmpBE512(a, b)); }
int WInt_CmpA1024(u16 *a, u16 *b)
	{ return(!WInt_CmpBE1024(a, b)); }


void WInt_Neg64(u16 *a)
{
	u32 *ap;

	ap=(u32 *)a;
	ap[0]=~ap[0];	ap[1]=~ap[1];
	WInt_Inc64(a);
}

void WInt_Neg128(u16 *a)
{
	u32 *ap;

	ap=(u32 *)a;
	ap[0]=~ap[0];	ap[1]=~ap[1];	ap[2]=~ap[2];	ap[3]=~ap[3];
	WInt_Inc128(a);
}

void WInt_Neg256(u16 *a)
{
	u32 *ap;

	ap=(u32 *)a;
	ap[0]=~ap[0];	ap[1]=~ap[1];	ap[2]=~ap[2];	ap[3]=~ap[3];
	ap[4]=~ap[4];	ap[5]=~ap[5];	ap[6]=~ap[6];	ap[7]=~ap[7];
	WInt_Inc256(a);
}

void WInt_Neg512(u16 *a)
{
	u32 *ap;
	int i;

	ap=(u32 *)a;
	for(i=0; i<16; i++)ap[i]=~ap[i];
	WInt_Inc512(a);
}

void WInt_Neg1024(u16 *a)
{
	u32 *ap;
	int i;

	ap=(u32 *)a;
	for(i=0; i<32; i++)ap[i]=~ap[i];
	WInt_Inc1024(a);
}


int WInt_Abs64(u16 *a, u16 *b)
{
	if(a[3]&0x8000)
	{
		b[0]=~a[0];	b[1]=~a[1];	b[2]=~a[2];	b[3]=~a[3];
		WInt_Inc64(b);
		return(1);
	}

	b[0]=a[0];	b[1]=a[1];	b[2]=a[2];	b[3]=a[3];
	return(0);
}

int WInt_Abs128(u16 *a, u16 *b)
{
	if(a[7]&0x8000)
	{
		b[0]=~a[0];	b[1]=~a[1];	b[2]=~a[2];	b[3]=~a[3];
		b[4]=~a[4];	b[5]=~a[5];	b[6]=~a[6];	b[7]=~a[7];
		WInt_Inc128(b);
		return(1);
	}

	b[0]=a[0];	b[1]=a[1];	b[2]=a[2];	b[3]=a[3];
	b[4]=a[4];	b[5]=a[5];	b[6]=a[6];	b[7]=a[7];
	return(0);
}

int WInt_Abs256(u16 *a, u16 *b)
{
	if(a[15]&0x8000)
	{
		b[ 0]=~a[ 0];	b[ 1]=~a[ 1];	b[ 2]=~a[ 2];	b[ 3]=~a[ 3];
		b[ 4]=~a[ 4];	b[ 5]=~a[ 5];	b[ 6]=~a[ 6];	b[ 7]=~a[ 7];
		b[ 8]=~a[ 8];	b[ 9]=~a[ 9];	b[10]=~a[10];	b[11]=~a[11];
		b[12]=~a[12];	b[13]=~a[13];	b[14]=~a[14];	b[15]=~a[15];
		WInt_Inc256(b);
		return(1);
	}

	b[ 0]=a[ 0];	b[ 1]=a[ 1];	b[ 2]=a[ 2];	b[ 3]=a[ 3];
	b[ 4]=a[ 4];	b[ 5]=a[ 5];	b[ 6]=a[ 6];	b[ 7]=a[ 7];
	b[ 8]=a[ 8];	b[ 9]=a[ 9];	b[10]=a[10];	b[11]=a[11];
	b[12]=a[12];	b[13]=a[13];	b[14]=a[14];	b[15]=a[15];
	return(0);
}

int WInt_Abs512(u16 *a, u16 *b)
{
	int i;
	if(a[31]&0x8000)
	{
		for(i=0; i<32; i++)b[i]=~a[i];
		WInt_Inc512(b);
		return(1);
	}

	for(i=0; i<32; i++)b[i]=a[i];
	return(0);
}

int WInt_Abs1024(u16 *a, u16 *b)
{
	int i;
	if(a[63]&0x8000)
	{
		for(i=0; i<64; i++)b[i]=~a[i];
		WInt_Inc1024(b);
		return(1);
	}

	for(i=0; i<64; i++)b[i]=a[i];
	return(0);
}



void WInt_MulW(u16 *a, u16 *b, u16 *c, int w)
{
	unsigned int t;
	int i, j, k, w2;

	w2=w*2;
	for(i=0; i<w; i++)
		for(j=0; j<w; j++)
	{
		t=((int)a[i])*b[j];
		for(k=i+j; k<w2; k++)
		{
			t+=c[k];
			c[k]=t;
			t>>=16;
		}
	}
}

//#ifndef X86
#if 1

void WInt_Mul64(u16 *a, u16 *b, u16 *c)
{
	WInt_MulW(a, b, c, 4);
}

void WInt_Mul128(u16 *a, u16 *b, u16 *c)
{
	WInt_MulW(a, b, c, 8);
}

#endif

void WInt_Mul256(u16 *a, u16 *b, u16 *c)
{
	WInt_MulW(a, b, c, 16);
}

void WInt_Mul512(u16 *a, u16 *b, u16 *c)
{
	WInt_MulW(a, b, c, 32);
}

void WInt_Mul1024(u16 *a, u16 *b, u16 *c)
{
	WInt_MulW(a, b, c, 64);
}

int WInt_Shr64_1(u16 *a)
{
	unsigned int i, j;

	i=a[0] | (a[1]<<16);	j=i&1;
	a[0]=i>>1;
	i=(i>>16) | (a[2]<<16);
	a[1]=i>>1;
	i=(i>>16) | (a[3]<<16);
	a[2]=i>>1;
	a[3]=i>>17;
	return(i);
}

int WInt_Shr128_1(u16 *a)
{
	int i, j;
	i=WInt_Shr64_1(a+4);
	j=WInt_Shr64_1(a);
	if(i)a[3]|=0x8000;
	return(j);
}

int WInt_Shr256_1(u16 *a)
{
	int i, j;
	i=WInt_Shr128_1(a+8);
	j=WInt_Shr128_1(a);
	if(i)a[7]|=0x8000;
	return(j);
}

int WInt_Shr512_1(u16 *a)
{
	int i, j;
	i=WInt_Shr256_1(a+16);
	j=WInt_Shr256_1(a);
	if(i)a[15]|=0x8000;
	return(j);
}

int WInt_Shr1024_1(u16 *a)
{
	int i, j;
	i=WInt_Shr128_1(a+32);
	j=WInt_Shr128_1(a);
	if(i)a[31]|=0x8000;
	return(j);
}


void WInt_Div64(u16 *a, u16 *b, u16 *c, u16 *d)
{
	u16 rx[8], dx[8], qx[4];
	int i, sg;

	sg=WInt_Abs64(a, rx);		WInt_Zero64(rx+4);
	sg^=WInt_Abs64(b, dx+4);	WInt_Zero64(dx);
	WInt_Zero64(qx);

	for(i=0; i<64; i++)
	{
		WInt_Shr128_1(dx);
		if(WInt_CmpBE128(dx, rx))
		{
			WInt_Sub128(rx, dx, rx);
			qx[i>>4]|=1<<(i&15);
		}
	}

	if(c)WInt_Copy64S(qx, c, sg);
	if(d)WInt_Copy64(rx, d);
}

void WInt_Div128(u16 *a, u16 *b, u16 *c, u16 *d)
{
	u16 rx[16], dx[16], qx[8];
	int i, sg;

	sg=WInt_Abs128(a, rx);		WInt_Zero128(rx+8);
	sg^=WInt_Abs128(b, dx+8);	WInt_Zero128(dx);
	WInt_Zero128(qx);

	for(i=0; i<128; i++)
	{
		WInt_Shr256_1(dx);
		if(WInt_CmpBE256(dx, rx))
		{
			WInt_Sub256(rx, dx, rx);
			qx[i>>4]|=1<<(i&15);
		}
	}

	if(c)WInt_Copy128S(qx, c, sg);
	if(d)WInt_Copy128(rx, d);
}

