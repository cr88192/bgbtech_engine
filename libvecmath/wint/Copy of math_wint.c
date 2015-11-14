#include <libvecmath.h>


void WInt_Add64(ushort *a, ushort *b, ushort *c)
{
	unsigned int i;

	i=(int)a[0]+b[0];		c[0]=i;
	i=(i>>16)+((int)a[1]+b[1]);	c[1]=i;
	i=(i>>16)+((int)a[2]+b[2]);	c[2]=i;
	i=(i>>16)+((int)a[3]+b[3]);	c[3]=i;
}

void WInt_Sub64(ushort *a, ushort *b, ushort *c)
{
	unsigned int i;

	i=(int)a[0]-b[0];		c[0]=i;
	i=(i>>16)+((int)a[1]-b[1]);	c[1]=i;
	i=(i>>16)+((int)a[2]-b[2]);	c[2]=i;
	i=(i>>16)+((int)a[3]-b[3]);	c[3]=i;
}

void WInt_Add128(ushort *a, ushort *b, ushort *c)
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

void WInt_Sub128(ushort *a, ushort *b, ushort *c)
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

void WInt_Add256(ushort *a, ushort *b, ushort *c)
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

void WInt_Sub256(ushort *a, ushort *b, ushort *c)
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

void WInt_Add512(ushort *a, ushort *b, ushort *c)
{
	unsigned int a;
	int i;
	a=0;
	for(i=0; i<32; i++)	{ a=(a>>16)+((int)a[i]+b[i]); c[i]=a; }
}

void WInt_Sub512(ushort *a, ushort *b, ushort *c)
{
	unsigned int a;
	int i;
	a=0;
	for(i=0; i<32; i++)	{ a=(a>>16)+((int)a[i]-b[i]); c[i]=a; }
}

void WInt_Add1024(ushort *a, ushort *b, ushort *c)
{
	unsigned int a;
	int i;
	a=0;
	for(i=0; i<64; i++)	{ a=(a>>16)+((int)a[i]+b[i]); c[i]=a; }
}

void WInt_Sub1024(ushort *a, ushort *b, ushort *c)
{
	unsigned int a;
	int i;
	a=0;
	for(i=0; i<64; i++)	{ a=(a>>16)+((int)a[i]-b[i]); c[i]=a; }
}

void WInt_Add1024(ushort *a, ushort *b, ushort *c)
{
	unsigned int a;
	int i;

	a=0;
	for(i=0; i<32; i++)	{ a=(a>>16)+((int)a[i]+b[i]); c[i]=a; }
}

void WInt_Sub1024(ushort *a, ushort *b, ushort *c)
{
	unsigned int a;
	int i;

	a=0;
	for(i=0; i<16; i++)	{ a=(a>>16)+((int)a[i]-b[i]); c[i]=a; }
}


void WInt_Mul64(ushort *a, ushort *b, ushort *c)
{
	unsigned int i;

	i=(int)a[0]*b[0];
	c[0]=i;

	i=(i>>16)+((int)a[1]*b[0])+((int)a[0]*b[1]);
	c[1]=i;

	i=(i>>16)+((int)a[2]*b[0])+((int)a[1]*b[1])+((int)a[0]*b[2]);
	c[2]=i;

	i=(i>>16)+((int)a[3]*b[0])+((int)a[2]*b[1])+
		((int)a[1]*b[2])+((int)a[0]*b[3]);
	c[3]=i;

	i=(i>>16)+((int)a[3]*b[1])+((int)a[2]*b[2])+((int)a[1]*b[3]);
	c[4]=i;

	i=(i>>16)+((int)a[3]*b[2])+((int)a[2]*b[3]);
	c[5]=i;

	i=(i>>16)+((int)a[3]*b[6]);
	c[6]=i;
	c[7]=i>>16;
}

void WInt_Mul128(ushort *a, ushort *b, ushort *c)
{
	unsigned int i;

	i=(int)a[0]*b[0];
	c[0]=i;

	i=(i>>16)+((int)a[1]*b[0])+((int)a[0]*b[1]);
	c[1]=i;

	i=(i>>16)+((int)a[2]*b[0])+((int)a[1]*b[1])+((int)a[0]*b[2]);
	c[2]=i;

	i=(i>>16)+((int)a[3]*b[0])+((int)a[2]*b[1])+
		((int)a[1]*b[2])+((int)a[0]*b[3]);
	c[3]=i;

	i=(i>>16)+((int)a[4]*b[0])+((int)a[3]*b[1])+((int)a[2]*b[2])+
		((int)a[1]*b[3])+((int)a[0]*b[4]);
	c[4]=i;

	i=(i>>16)+	((int)a[5]*b[0])+	((int)a[4]*b[1])+
			((int)a[3]*b[2])+	((int)a[2]*b[3])+
			((int)a[1]*b[4])+	((int)a[0]*b[5]);
	c[5]=i;

	i=(i>>16)+	((int)a[6]*b[0])+	((int)a[5]*b[1])+
			((int)a[4]*b[2])+	((int)a[3]*b[3])+
			((int)a[2]*b[4])+	((int)a[1]*b[5])+
			((int)a[0]*b[6]);
	c[6]=i;

	i=(i>>16)+	((int)a[7]*b[0])+	((int)a[6]*b[1])+
			((int)a[5]*b[2])+	((int)a[4]*b[3])+
			((int)a[3]*b[4])+	((int)a[2]*b[5])+
			((int)a[1]*b[6])+	((int)a[0]*b[7]);
	c[7]=i;

	i=(i>>16)+	((int)a[7]*b[1])+	((int)a[6]*b[2])+
			((int)a[5]*b[3])+	((int)a[4]*b[4])+
			((int)a[3]*b[5])+	((int)a[2]*b[6])+
			((int)a[1]*b[7]);
	c[8]=i;

	i=(i>>16)+	((int)a[7]*b[2])+	((int)a[6]*b[3])+
			((int)a[5]*b[4])+	((int)a[4]*b[5])+
			((int)a[3]*b[6])+	((int)a[2]*b[7]);
	c[9]=i;

	i=(i>>16)+	((int)a[7]*b[3])+	((int)a[6]*b[4])+
			((int)a[5]*b[5])+	((int)a[4]*b[6])+
			((int)a[3]*b[7]);
	c[10]=i;

	i=(i>>16)+	((int)a[7]*b[4])+	((int)a[6]*b[5])+
			((int)a[5]*b[6])+	((int)a[4]*b[7]);
	c[11]=i;

	i=(i>>16)+	((int)a[7]*b[5])+	((int)a[6]*b[6])+
			((int)a[5]*b[7]);
	c[12]=i;

	i=(i>>16)+	((int)a[7]*b[6])+	((int)a[6]*b[7]);
	c[13]=i;

	i=(i>>16)+	((int)a[7]*b[7]);
	c[14]=i;

	c[15]=i>>16;
}


void WInt_Mul256(ushort *a, ushort *b, ushort *c)
{
	unsigned int i;

	i=(int)a[0]*b[0];
	c[0]=i;

	i=(i>>16)+((int)a[1]*b[0])+((int)a[0]*b[1]);
	c[1]=i;

	i=(i>>16)+((int)a[2]*b[0])+((int)a[1]*b[1])+((int)a[0]*b[2]);
	c[2]=i;

	i=(i>>16)+((int)a[3]*b[0])+((int)a[2]*b[1])+
		((int)a[1]*b[2])+((int)a[0]*b[3]);
	c[3]=i;

	i=(i>>16)+((int)a[4]*b[0])+((int)a[3]*b[1])+((int)a[2]*b[2])+
		((int)a[1]*b[3])+((int)a[0]*b[4]);
	c[4]=i;

	i=(i>>16)+	((int)a[5]*b[0])+	((int)a[4]*b[1])+
			((int)a[3]*b[2])+	((int)a[2]*b[3])+
			((int)a[1]*b[4])+	((int)a[0]*b[5]);
	c[5]=i;

	i=(i>>16)+	((int)a[6]*b[0])+	((int)a[5]*b[1])+
			((int)a[4]*b[2])+	((int)a[3]*b[3])+
			((int)a[2]*b[4])+	((int)a[1]*b[5])+
			((int)a[0]*b[6]);
	c[6]=i;

	i=(i>>16)+	((int)a[7]*b[0])+	((int)a[6]*b[1])+
			((int)a[5]*b[2])+	((int)a[4]*b[3])+
			((int)a[3]*b[4])+	((int)a[2]*b[5])+
			((int)a[1]*b[6])+	((int)a[0]*b[7]);
	c[7]=i;


	i=(i>>16)+	((int)a[8]*b[0])+	((int)a[7]*b[1])+
			((int)a[6]*b[2])+	((int)a[5]*b[3])+
			((int)a[4]*b[4])+	((int)a[3]*b[5])+
			((int)a[2]*b[6])+	((int)a[1]*b[7])+
			((int)a[0]*b[8]);
	c[8]=i;

	i=(i>>16)+	((int)a[9]*b[0])+	((int)a[8]*b[1])+
			((int)a[7]*b[2])+	((int)a[6]*b[3])+
			((int)a[5]*b[4])+	((int)a[4]*b[5])+
			((int)a[3]*b[6])+	((int)a[2]*b[7])+
			((int)a[1]*b[8])+	((int)a[0]*b[9]);
	c[9]=i;

	i=(i>>16)+	((int)a[10]*b[ 0])+	((int)a[ 9]*b[ 1])+
			((int)a[ 8]*b[ 2])+	((int)a[ 7]*b[ 3])+
			((int)a[ 6]*b[ 4])+	((int)a[ 5]*b[ 5])+
			((int)a[ 4]*b[ 6])+	((int)a[ 3]*b[ 7])+
			((int)a[ 2]*b[ 8])+	((int)a[ 1]*b[ 9])+
			((int)a[ 0]*b[10]);
	c[10]=i;

	i=(i>>16)+	((int)a[11]*b[ 0])+	((int)a[10]*b[ 1])+
			((int)a[ 9]*b[ 2])+	((int)a[ 8]*b[ 3])+
			((int)a[ 7]*b[ 4])+	((int)a[ 6]*b[ 5])+
			((int)a[ 5]*b[ 6])+	((int)a[ 4]*b[ 7])+
			((int)a[ 3]*b[ 8])+	((int)a[ 2]*b[ 9])+
			((int)a[ 1]*b[10])+	((int)a[ 0]*b[11]);
	c[11]=i;

	i=(i>>16)+	((int)a[12]*b[ 0])+	((int)a[11]*b[ 1])+
			((int)a[10]*b[ 2])+	((int)a[ 9]*b[ 3])+
			((int)a[ 8]*b[ 4])+	((int)a[ 7]*b[ 5])+
			((int)a[ 6]*b[ 6])+	((int)a[ 5]*b[ 7])+
			((int)a[ 4]*b[ 8])+	((int)a[ 3]*b[ 9])+
			((int)a[ 2]*b[10])+	((int)a[ 1]*b[11])+
			((int)a[ 0]*b[12]);
	c[12]=i;

	i=(i>>16)+	((int)a[13]*b[ 0])+	((int)a[12]*b[ 1])+
			((int)a[11]*b[ 2])+	((int)a[10]*b[ 3])+
			((int)a[ 9]*b[ 4])+	((int)a[ 8]*b[ 5])+
			((int)a[ 7]*b[ 6])+	((int)a[ 6]*b[ 7])+
			((int)a[ 5]*b[ 8])+	((int)a[ 4]*b[ 9])+
			((int)a[ 3]*b[10])+	((int)a[ 2]*b[11])+
			((int)a[ 1]*b[12])+	((int)a[ 0]*b[13]);
	c[13]=i;

	i=(i>>16)+	((int)a[14]*b[ 0])+	((int)a[13]*b[ 1])+
			((int)a[12]*b[ 2])+	((int)a[11]*b[ 3])+
			((int)a[10]*b[ 4])+	((int)a[ 9]*b[ 5])+
			((int)a[ 8]*b[ 6])+	((int)a[ 7]*b[ 7])+
			((int)a[ 6]*b[ 8])+	((int)a[ 5]*b[ 9])+
			((int)a[ 4]*b[10])+	((int)a[ 3]*b[11])+
			((int)a[ 2]*b[12])+	((int)a[ 1]*b[13])+
			((int)a[ 0]*b[14]);
	c[14]=i;

	i=(i>>16)+	((int)a[15]*b[ 0])+	((int)a[14]*b[ 1])+
			((int)a[13]*b[ 2])+	((int)a[12]*b[ 3])+
			((int)a[11]*b[ 4])+	((int)a[10]*b[ 5])+
			((int)a[ 9]*b[ 6])+	((int)a[ 8]*b[ 7])+
			((int)a[ 7]*b[ 8])+	((int)a[ 6]*b[ 9])+
			((int)a[ 5]*b[10])+	((int)a[ 4]*b[11])+
			((int)a[ 3]*b[12])+	((int)a[ 2]*b[13])+
			((int)a[ 1]*b[14])+	((int)a[ 0]*b[15]);
	c[15]=i;

	i=(i>>16)+	((int)a[15]*b[ 1])+	((int)a[14]*b[ 2])+
			((int)a[13]*b[ 3])+	((int)a[12]*b[ 4])+
			((int)a[11]*b[ 5])+	((int)a[10]*b[ 6])+
			((int)a[ 9]*b[ 7])+	((int)a[ 8]*b[ 8])+
			((int)a[ 7]*b[ 9])+	((int)a[ 6]*b[10])+
			((int)a[ 5]*b[11])+	((int)a[ 4]*b[12])+
			((int)a[ 3]*b[13])+	((int)a[ 2]*b[14])+
			((int)a[ 1]*b[15]);
	c[16]=i;

	i=(i>>16)+	((int)a[15]*b[ 2])+	((int)a[14]*b[ 3])+
			((int)a[13]*b[ 4])+	((int)a[12]*b[ 5])+
			((int)a[11]*b[ 6])+	((int)a[10]*b[ 7])+
			((int)a[ 9]*b[ 8])+	((int)a[ 8]*b[ 9])+
			((int)a[ 7]*b[10])+	((int)a[ 6]*b[11])+
			((int)a[ 5]*b[12])+	((int)a[ 4]*b[13])+
			((int)a[ 3]*b[14])+	((int)a[ 2]*b[15]);
	c[17]=i;

	i=(i>>16)+	((int)a[15]*b[ 3])+	((int)a[14]*b[ 4])+
			((int)a[13]*b[ 5])+	((int)a[12]*b[ 6])+
			((int)a[11]*b[ 7])+	((int)a[10]*b[ 8])+
			((int)a[ 9]*b[ 9])+	((int)a[ 8]*b[10])+
			((int)a[ 7]*b[11])+	((int)a[ 6]*b[12])+
			((int)a[ 5]*b[13])+	((int)a[ 4]*b[14])+
			((int)a[ 3]*b[15]);
	c[18]=i;

	i=(i>>16)+	((int)a[15]*b[ 4])+	((int)a[14]*b[ 5])+
			((int)a[13]*b[ 6])+	((int)a[12]*b[ 7])+
			((int)a[11]*b[ 8])+	((int)a[10]*b[ 9])+
			((int)a[ 9]*b[10])+	((int)a[ 8]*b[11])+
			((int)a[ 7]*b[12])+	((int)a[ 6]*b[13])+
			((int)a[ 5]*b[14])+	((int)a[ 4]*b[15]);
	c[19]=i;

	i=(i>>16)+	((int)a[15]*b[ 5])+	((int)a[14]*b[ 6])+
			((int)a[13]*b[ 7])+	((int)a[12]*b[ 8])+
			((int)a[11]*b[ 9])+	((int)a[10]*b[10])+
			((int)a[ 9]*b[11])+	((int)a[ 8]*b[12])+
			((int)a[ 7]*b[13])+	((int)a[ 6]*b[14])+
			((int)a[ 5]*b[15]);
	c[20]=i;

	i=(i>>16)+	((int)a[15]*b[ 6])+	((int)a[14]*b[ 7])+
			((int)a[13]*b[ 8])+	((int)a[12]*b[ 9])+
			((int)a[11]*b[10])+	((int)a[10]*b[11])+
			((int)a[ 9]*b[12])+	((int)a[ 8]*b[13])+
			((int)a[ 7]*b[14])+	((int)a[ 6]*b[15]);
	c[21]=i;

	i=(i>>16)+	((int)a[15]*b[ 7])+	((int)a[14]*b[ 8])+
			((int)a[13]*b[ 9])+	((int)a[12]*b[10])+
			((int)a[11]*b[11])+	((int)a[10]*b[12])+
			((int)a[ 9]*b[13])+	((int)a[ 8]*b[14])+
			((int)a[ 7]*b[15]);
	c[22]=i;

	i=(i>>16)+	((int)a[15]*b[ 8])+	((int)a[14]*b[ 9])+
			((int)a[13]*b[10])+	((int)a[12]*b[11])+
			((int)a[11]*b[12])+	((int)a[10]*b[13])+
			((int)a[ 9]*b[14])+	((int)a[ 8]*b[15]);
	c[23]=i;

	i=(i>>16)+	((int)a[15]*b[ 9])+	((int)a[14]*b[10])+
			((int)a[13]*b[11])+	((int)a[12]*b[12])+
			((int)a[11]*b[13])+	((int)a[10]*b[14])+
			((int)a[ 9]*b[15]);
	c[24]=i;

	i=(i>>16)+	((int)a[15]*b[10])+	((int)a[14]*b[11])+
			((int)a[13]*b[12])+	((int)a[12]*b[13])+
			((int)a[11]*b[14])+	((int)a[10]*b[15]);
	c[25]=i;

	i=(i>>16)+	((int)a[15]*b[11])+	((int)a[14]*b[12])+
			((int)a[13]*b[13])+	((int)a[12]*b[14])+
			((int)a[11]*b[15]);
	c[26]=i;

	i=(i>>16)+	((int)a[15]*b[12])+	((int)a[14]*b[13])+
			((int)a[13]*b[14])+	((int)a[12]*b[15]);
	c[27]=i;

	i=(i>>16)+	((int)a[15]*b[13])+	((int)a[14]*b[14])+
			((int)a[13]*b[15]);
	c[28]=i;

	i=(i>>16)+	((int)a[15]*b[14])+	((int)a[14]*b[15]);
	c[29]=i;

	i=(i>>16)+((int)a[15]*b[15]);
	c[30]=i;
	c[31]=i>>16;
}

void WInt_Mul512(ushort *a, ushort *b, ushort *c)
{
	unsigned int a;
	int i, j, k;

	a=0;
	for(i=0; i<32; i++)
	{
		for(j=0; j<=i; j++)
			a+=((int)a[j])*b[i-j];
		c[i]=a;	a>>=16;
	}
	for(; i<64; i++)
	{
		k=64-i;
		for(j=0; j<i; j++)
			a+=((int)a[32-k+j])*b[31-j];
		c[i]=a;	a>>=16;
	}
}

void WInt_Mul1024(ushort *a, ushort *b, ushort *c)
{
	unsigned int a;
	int i, j, k;

	a=0;
	for(i=0; i<64; i++)
	{
		for(j=0; j<=i; j++)
			a+=((int)a[j])*b[i-j];
		c[i]=a;	a>>=16;
	}
	for(; i<128; i++)
	{
		k=128-i;
		for(j=0; j<i; j++)
			a+=((int)a[64-k+j])*b[63-j];
		c[i]=a;	a>>=16;
	}
}
