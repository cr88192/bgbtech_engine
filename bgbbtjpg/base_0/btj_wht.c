#include <bgbbtj.h>

/*
Walsh Hadamard Transform
*/

void BGBBTJ_JPG_TransWHT_Stride4(int *iblk, int *oblk,
	int istride, int ostride)
{
	int ib0, ib1, ib2, ib3;
	int a0, a1, a2, a3;
	int b0, b1, b2, b3;

	ib0=iblk[istride*0];	ib1=iblk[istride*1];
	ib2=iblk[istride*2];	ib3=iblk[istride*3];

	a0=ib0+ib2;
	a1=ib1+ib3;
	a2=(a0>>1)-ib2;
	a3=(a1>>1)-ib3;

	b0=a0+a1;
	b1=(b0>>1)-a1;
	b2=a2+a3;
	b3=(b2>>1)-a3;

	oblk[ostride*0]=b0;
	oblk[ostride*1]=b2;
	oblk[ostride*2]=b3;
	oblk[ostride*3]=b1;
}

void BGBBTJ_JPG_TransIWHT_Stride4(int *iblk, int *oblk,
	int istride, int ostride)
{
	int ob0, ob1, ob2, ob3;
	int a0, a1, a2, a3;
	int b0, b1, b2, b3;

	b0=iblk[istride*0];
	b2=iblk[istride*1];
	b3=iblk[istride*2];
	b1=iblk[istride*3];

	a1=(b0>>1)-b1;
	a0=b0-a1;
	a3=(b2>>1)-b3;
	a2=b2-a3;

	ob2=(a0>>1)-a2;
	ob3=(a1>>1)-a3;
	ob0=a0-ob2;
	ob1=a1-ob3;

	oblk[ostride*0]=ob0;
	oblk[ostride*1]=ob1;
	oblk[ostride*2]=ob2;
	oblk[ostride*3]=ob3;
}

void BGBBTJ_JPG_TransWHT4Stride(int *iblk, int *oblk,
	int xstride, int ystride)
{
	int s[16], t[16];
	int i, j;

	BGBBTJ_JPG_TransWHT_Stride4(iblk+ystride*0, s+4*0, xstride, 1);
	BGBBTJ_JPG_TransWHT_Stride4(iblk+ystride*1, s+4*1, xstride, 1);
	BGBBTJ_JPG_TransWHT_Stride4(iblk+ystride*2, s+4*2, xstride, 1);
	BGBBTJ_JPG_TransWHT_Stride4(iblk+ystride*3, s+4*3, xstride, 1);

	BGBBTJ_JPG_TransWHT_Stride4(s+0, t+0, 4, 4);
	BGBBTJ_JPG_TransWHT_Stride4(s+1, t+1, 4, 4);
	BGBBTJ_JPG_TransWHT_Stride4(s+2, t+2, 4, 4);
	BGBBTJ_JPG_TransWHT_Stride4(s+3, t+3, 4, 4);

	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		oblk[i*ystride+j*xstride]=t[i*4+j];
	}
}

void BGBBTJ_JPG_TransIWHT4Stride(int *iblk, int *oblk,
	int xstride, int ystride)
{
	int s[16], t[16];
	int i, j;

	BGBBTJ_JPG_TransIWHT_Stride4(iblk+xstride*0, s+0, ystride, 4);
	BGBBTJ_JPG_TransIWHT_Stride4(iblk+xstride*1, s+1, ystride, 4);
	BGBBTJ_JPG_TransIWHT_Stride4(iblk+xstride*2, s+2, ystride, 4);
	BGBBTJ_JPG_TransIWHT_Stride4(iblk+xstride*3, s+3, ystride, 4);

	BGBBTJ_JPG_TransIWHT_Stride4(s+0*4, t+0*4, 1, 1);
	BGBBTJ_JPG_TransIWHT_Stride4(s+1*4, t+1*4, 1, 1);
	BGBBTJ_JPG_TransIWHT_Stride4(s+2*4, t+2*4, 1, 1);
	BGBBTJ_JPG_TransIWHT_Stride4(s+3*4, t+3*4, 1, 1);

	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		oblk[i*ystride+j*xstride]=t[i*4+j];
	}
}


void BGBBTJ_JPG_TransWHT_Stride8(int *iblk, int *oblk,
	int istride, int ostride)
{
	int ib0, ib1, ib2, ib3, ib4, ib5, ib6, ib7;
	int a0, a1, a2, a3, a4, a5, a6, a7;
	int b0, b1, b2, b3, b4, b5, b6, b7;
	int c0, c1, c2, c3, c4, c5, c6, c7;

	ib0=iblk[istride*0];	ib1=iblk[istride*1];
	ib2=iblk[istride*2];	ib3=iblk[istride*3];
	ib4=iblk[istride*4];	ib5=iblk[istride*5];
	ib6=iblk[istride*6];	ib7=iblk[istride*7];

	a0=ib0+ib4;			a1=ib1+ib5;
	a2=ib2+ib6;			a3=ib3+ib7;
	a4=(a0>>1)-ib4;		a5=(a1>>1)-ib5;
	a6=(a2>>1)-ib6;		a7=(a3>>1)-ib7;

	b0=a0+a2;
	b1=a1+a3;
	b2=(b0>>1)-a2;
	b3=(b1>>1)-a3;
	b4=a4+a6;
	b5=a5+a7;
	b6=(b4>>1)-a6;
	b7=(b5>>1)-a7;

	c0=b0+b1;
	c1=(c0>>1)-b1;
	c2=b2+b3;
	c3=(c2>>1)-b3;
	c4=b4+b5;
	c5=(c4>>1)-b5;
	c6=b6+b7;
	c7=(c6>>1)-b7;

	oblk[ostride*0]=c0;
	oblk[ostride*1]=c4;
	oblk[ostride*2]=c6;
	oblk[ostride*3]=c2;
	oblk[ostride*4]=c3;
	oblk[ostride*5]=c7;
	oblk[ostride*6]=c5;
	oblk[ostride*7]=c1;
}

void BGBBTJ_JPG_TransIWHT_Stride8(int *iblk, int *oblk,
	int istride, int ostride)
{
	int ob0, ob1, ob2, ob3, ob4, ob5, ob6, ob7;
	int a0, a1, a2, a3, a4, a5, a6, a7;
	int b0, b1, b2, b3, b4, b5, b6, b7;
	int c0, c1, c2, c3, c4, c5, c6, c7;

	c0=iblk[istride*0];
	c4=iblk[istride*1];
	c6=iblk[istride*2];
	c2=iblk[istride*3];
	c3=iblk[istride*4];
	c7=iblk[istride*5];
	c5=iblk[istride*6];
	c1=iblk[istride*7];

	b1=(c0>>1)-c1;
	b0=c0-b1;
	b3=(c2>>1)-c3;
	b2=c2-b3;
	b5=(c4>>1)-c5;
	b4=c4-b5;
	b7=(c6>>1)-c7;
	b6=c6-b7;

	a2=(b0>>1)-b2;
	a3=(b1>>1)-b3;
	a0=b0-a2;
	a1=b1-a3;
	a6=(b4>>1)-b6;
	a7=(b5>>1)-b7;
	a4=b4-a6;
	a5=b5-a7;

	ob4=(a0>>1)-a4;
	ob5=(a1>>1)-a5;
	ob6=(a2>>1)-a6;
	ob7=(a3>>1)-a7;
	ob0=a0-ob4;
	ob1=a1-ob5;
	ob2=a2-ob6;
	ob3=a3-ob7;

	oblk[ostride*0]=ob0;
	oblk[ostride*1]=ob1;
	oblk[ostride*2]=ob2;
	oblk[ostride*3]=ob3;
	oblk[ostride*4]=ob4;
	oblk[ostride*5]=ob5;
	oblk[ostride*6]=ob6;
	oblk[ostride*7]=ob7;
}

void BGBBTJ_JPG_TransWHT8Stride(int *iblk, int *oblk,
	int xstride, int ystride)
{
	int s[64], t[64];
	int i, j;

	BGBBTJ_JPG_TransWHT_Stride8(iblk+ystride*0, s+8*0, xstride, 1);
	BGBBTJ_JPG_TransWHT_Stride8(iblk+ystride*1, s+8*1, xstride, 1);
	BGBBTJ_JPG_TransWHT_Stride8(iblk+ystride*2, s+8*2, xstride, 1);
	BGBBTJ_JPG_TransWHT_Stride8(iblk+ystride*3, s+8*3, xstride, 1);
	BGBBTJ_JPG_TransWHT_Stride8(iblk+ystride*4, s+8*4, xstride, 1);
	BGBBTJ_JPG_TransWHT_Stride8(iblk+ystride*5, s+8*5, xstride, 1);
	BGBBTJ_JPG_TransWHT_Stride8(iblk+ystride*6, s+8*6, xstride, 1);
	BGBBTJ_JPG_TransWHT_Stride8(iblk+ystride*7, s+8*7, xstride, 1);

	BGBBTJ_JPG_TransWHT_Stride8(s+0, t+0, 8, 8);
	BGBBTJ_JPG_TransWHT_Stride8(s+1, t+1, 8, 8);
	BGBBTJ_JPG_TransWHT_Stride8(s+2, t+2, 8, 8);
	BGBBTJ_JPG_TransWHT_Stride8(s+3, t+3, 8, 8);
	BGBBTJ_JPG_TransWHT_Stride8(s+4, t+4, 8, 8);
	BGBBTJ_JPG_TransWHT_Stride8(s+5, t+5, 8, 8);
	BGBBTJ_JPG_TransWHT_Stride8(s+6, t+6, 8, 8);
	BGBBTJ_JPG_TransWHT_Stride8(s+7, t+7, 8, 8);

	for(i=0; i<8; i++)
		for(j=0; j<8; j++)
	{
		oblk[i*ystride+j*xstride]=t[i*8+j];
	}
}

void BGBBTJ_JPG_TransIWHT8Stride(int *iblk, int *oblk,
	int xstride, int ystride)
{
	int s[64], t[64];
	int i, j;

	BGBBTJ_JPG_TransIWHT_Stride8(iblk+xstride*0, s+0, ystride, 8);
	BGBBTJ_JPG_TransIWHT_Stride8(iblk+xstride*1, s+1, ystride, 8);
	BGBBTJ_JPG_TransIWHT_Stride8(iblk+xstride*2, s+2, ystride, 8);
	BGBBTJ_JPG_TransIWHT_Stride8(iblk+xstride*3, s+3, ystride, 8);
	BGBBTJ_JPG_TransIWHT_Stride8(iblk+xstride*4, s+4, ystride, 8);
	BGBBTJ_JPG_TransIWHT_Stride8(iblk+xstride*5, s+5, ystride, 8);
	BGBBTJ_JPG_TransIWHT_Stride8(iblk+xstride*6, s+6, ystride, 8);
	BGBBTJ_JPG_TransIWHT_Stride8(iblk+xstride*7, s+7, ystride, 8);

	BGBBTJ_JPG_TransIWHT_Stride8(s+0*8, t+0*8, 1, 1);
	BGBBTJ_JPG_TransIWHT_Stride8(s+1*8, t+1*8, 1, 1);
	BGBBTJ_JPG_TransIWHT_Stride8(s+2*8, t+2*8, 1, 1);
	BGBBTJ_JPG_TransIWHT_Stride8(s+3*8, t+3*8, 1, 1);
	BGBBTJ_JPG_TransIWHT_Stride8(s+4*8, t+4*8, 1, 1);
	BGBBTJ_JPG_TransIWHT_Stride8(s+5*8, t+5*8, 1, 1);
	BGBBTJ_JPG_TransIWHT_Stride8(s+6*8, t+6*8, 1, 1);
	BGBBTJ_JPG_TransIWHT_Stride8(s+7*8, t+7*8, 1, 1);

	for(i=0; i<8; i++)
		for(j=0; j<8; j++)
	{
		oblk[i*ystride+j*xstride]=t[i*8+j];
	}
}
