#include <bgbbtj.h>

int BGBBTJ_BCn_IndexBC4Y(int val, int min, int max)
{
	int y, r, v;

	if(val==min)return(0);
	if(val==max)return(1);
	if(val==0)return(6);
	if(val==255)return(7);
	
	y=val-min;
	r=max-min;
	
	v=y*(256/(r+1));
	v=(v+32)>>6;
	if(v<0)v=0;
	if(v>3)v=3;
	return(v+2);
}

void BGBBTJ_BCn_FillBlockBC4(byte *block, int val)
{
	block[0]=val; block[1]=val;
	block[2]=0;	block[3]=0;	block[4]=0;
	block[5]=0;	block[6]=0;	block[7]=0;
	return;
}

void BGBBTJ_BCn_EncodeBlockBC4(byte *block,
	byte *rgba, int xstride, int ystride)
{
	int min, max, avg;
	int cy;
	int p0, p1, p2, p3, p4, p5, p6, p7;
	int i, j;

	min=255; max=0; avg=0;
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		cy=rgba[(i*ystride)+(j*xstride)+0];
		if(cy<min)min=cy;
		if(cy>max)max=cy;
		avg+=cy;
	}
	avg=(avg+8)>>4;
	
	if(min==max)
	{
		block[0]=min;
		block[1]=max;
		block[2]=0;	block[3]=0;	block[4]=0;
		block[5]=0;	block[6]=0;	block[7]=0;
		return;
	}
	
	block[0]=min;
	block[1]=max;

	p0=BGBBTJ_BCn_IndexBC4Y(rgba[(0*xstride)+(0*ystride)], min, max);
	p1=BGBBTJ_BCn_IndexBC4Y(rgba[(1*xstride)+(0*ystride)], min, max);
	p2=BGBBTJ_BCn_IndexBC4Y(rgba[(2*xstride)+(0*ystride)], min, max);
	p3=BGBBTJ_BCn_IndexBC4Y(rgba[(3*xstride)+(0*ystride)], min, max);

	p4=BGBBTJ_BCn_IndexBC4Y(rgba[(0*xstride)+(1*ystride)], min, max);
	p5=BGBBTJ_BCn_IndexBC4Y(rgba[(1*xstride)+(1*ystride)], min, max);
	p6=BGBBTJ_BCn_IndexBC4Y(rgba[(2*xstride)+(1*ystride)], min, max);
	p7=BGBBTJ_BCn_IndexBC4Y(rgba[(3*xstride)+(1*ystride)], min, max);

	block[2]=p0|(p1<<3)|(p2<<6);
	block[3]=(p2>>2)|(p3<<1)|(p4<<4)|(p5<<7);
	block[4]=(p5>>1)|(p6<<2)|(p7<<5);
	
//	block[2]=(p7<<5)|(p6<<2)|(p5>>1);
//	block[3]=(p5<<7)|(p4<<4)|(p3<<1)|(p2>>2);
//	block[4]=(p2<<6)|(p1<<3)|p0;

//	block[4]=(p7<<5)|(p6<<2)|(p5>>1);
//	block[3]=(p5<<7)|(p4<<4)|(p3<<1)|(p2>>2);
//	block[2]=(p2<<6)|(p1<<3)|p0;

	p0=BGBBTJ_BCn_IndexBC4Y(rgba[(0*xstride)+(2*ystride)], min, max);
	p1=BGBBTJ_BCn_IndexBC4Y(rgba[(1*xstride)+(2*ystride)], min, max);
	p2=BGBBTJ_BCn_IndexBC4Y(rgba[(2*xstride)+(2*ystride)], min, max);
	p3=BGBBTJ_BCn_IndexBC4Y(rgba[(3*xstride)+(2*ystride)], min, max);

	p4=BGBBTJ_BCn_IndexBC4Y(rgba[(0*xstride)+(3*ystride)], min, max);
	p5=BGBBTJ_BCn_IndexBC4Y(rgba[(1*xstride)+(3*ystride)], min, max);
	p6=BGBBTJ_BCn_IndexBC4Y(rgba[(2*xstride)+(3*ystride)], min, max);
	p7=BGBBTJ_BCn_IndexBC4Y(rgba[(3*xstride)+(3*ystride)], min, max);
	
	block[5]=p0|(p1<<3)|(p2<<6);
	block[6]=(p2>>2)|(p3<<1)|(p4<<4)|(p5<<7);
	block[7]=(p5>>1)|(p6<<2)|(p7<<5);

//	block[5]=(p7<<5)|(p6<<2)|(p5>>1);
//	block[6]=(p5<<7)|(p4<<4)|(p3<<1)|(p2>>2);
//	block[7]=(p2<<6)|(p1<<3)|p0;

//	block[7]=(p7<<5)|(p6<<2)|(p5>>1);
//	block[6]=(p5<<7)|(p4<<4)|(p3<<1)|(p2>>2);
//	block[5]=(p2<<6)|(p1<<3)|p0;
}


int BGBBTJ_BCn_IndexBC4YL(int val, int min, int max)
{
	int y, r, v;

	if(val==min)return(0);
	if(val==max)return(1);
	if(val==0)return(6);
	if(val==255)return(7);
	
	v=(min+max)>>1;
	return((val>v)?4:3);
	
//	y=val-min;
//	r=max-min;
	
//	v=y*(256/(r+1));
//	v=(v+32)>>6;
//	v&=~1;
//	if(v<0)v=0;
//	if(v>3)v=3;
//	return(v+2);
}

void BGBBTJ_BCn_EncodeBlockBC4L(byte *block,
	byte *rgba, int xstride, int ystride)
{
	int min, max, avg;
	int cy;
	int p0, p1, p2, p3, p4, p5, p6, p7;
	int i, j;

	min=255; max=0; avg=0;
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		cy=rgba[(i*ystride)+(j*xstride)+0];
		if(cy<min)min=cy;
		if(cy>max)max=cy;
		avg+=cy;
	}
	avg=(avg+8)>>4;
	
	if(min==max)
	{
		block[0]=min;
		block[1]=max;
		block[2]=0;	block[3]=0;	block[4]=0;
		block[5]=0;	block[6]=0;	block[7]=0;
		return;
	}
	
	block[0]=min;
	block[1]=max;

	p0=BGBBTJ_BCn_IndexBC4YL(rgba[(0*xstride)+(0*ystride)], min, max);
	p1=BGBBTJ_BCn_IndexBC4YL(rgba[(1*xstride)+(0*ystride)], min, max);
	p2=BGBBTJ_BCn_IndexBC4YL(rgba[(2*xstride)+(0*ystride)], min, max);
	p3=BGBBTJ_BCn_IndexBC4YL(rgba[(3*xstride)+(0*ystride)], min, max);

	p4=BGBBTJ_BCn_IndexBC4YL(rgba[(0*xstride)+(1*ystride)], min, max);
	p5=BGBBTJ_BCn_IndexBC4YL(rgba[(1*xstride)+(1*ystride)], min, max);
	p6=BGBBTJ_BCn_IndexBC4YL(rgba[(2*xstride)+(1*ystride)], min, max);
	p7=BGBBTJ_BCn_IndexBC4YL(rgba[(3*xstride)+(1*ystride)], min, max);

	block[2]=p0|(p1<<3)|(p2<<6);
	block[3]=(p2>>2)|(p3<<1)|(p4<<4)|(p5<<7);
	block[4]=(p5>>1)|(p6<<2)|(p7<<5);

	p0=BGBBTJ_BCn_IndexBC4YL(rgba[(0*xstride)+(2*ystride)], min, max);
	p1=BGBBTJ_BCn_IndexBC4YL(rgba[(1*xstride)+(2*ystride)], min, max);
	p2=BGBBTJ_BCn_IndexBC4YL(rgba[(2*xstride)+(2*ystride)], min, max);
	p3=BGBBTJ_BCn_IndexBC4YL(rgba[(3*xstride)+(2*ystride)], min, max);

	p4=BGBBTJ_BCn_IndexBC4YL(rgba[(0*xstride)+(3*ystride)], min, max);
	p5=BGBBTJ_BCn_IndexBC4YL(rgba[(1*xstride)+(3*ystride)], min, max);
	p6=BGBBTJ_BCn_IndexBC4YL(rgba[(2*xstride)+(3*ystride)], min, max);
	p7=BGBBTJ_BCn_IndexBC4YL(rgba[(3*xstride)+(3*ystride)], min, max);
	
	block[5]=p0|(p1<<3)|(p2<<6);
	block[6]=(p2>>2)|(p3<<1)|(p4<<4)|(p5<<7);
	block[7]=(p5>>1)|(p6<<2)|(p7<<5);
}

void BGBBTJ_BCn_EncodeBlockBC4Y8x8(byte *block, short *y)
{
	byte yb[16];
	int i, j, k;
	
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		k=y[i*8+j];
		if(k<0)k=0;
		if(k>255)k=255;
		yb[(3-i)*4+j]=k;
	}

	BGBBTJ_BCn_EncodeBlockBC4L(block, yb, 1, 4);
}
