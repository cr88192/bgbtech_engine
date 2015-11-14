/*
BTIC Meta Block.

Bits 0-3: Tag
Tag==0
	Bits 4-63: Metadata
	Bits 64-127: Alpha Block (64 bit)
	Bits 128-255: Color Block (128 bit)
Tag==1
	Bits 4-63: Metadata
	Bits 64-127: Alpha Block (64 bit)
	Bits 128-191: Color Block (64 bit)
	Bits 192-255: Reserved / Metadata

Alpha Block:
AlphaA>AlphaB:
	AlphaA(8), AlphaB(8), Indices(16x3).
	Index:
		0=A,			1=B,
		2=6/7A+1/7B,	3=5/7A+2/7B,
		4=4/7A+3/7B,	5=3/7A+4/7B,
		6=2/7A+5/7B,	7=1/7A+6/7B
AlphaA<=AlphaB:
	AlphaA(8), AlphaB(8), Tag(4), Meta(12), Indices(16x2).
	Index:
		0=A, 1=B, 2=2/3A+1/3B, 3=1/3A+2/3B
		Tag 0: Normal
		Tag 1: Special Color
		Tag 2: Skip/Translate, Meta=Y(6) X(6)

Tag=0 ColorBlock:
	0: Tag(4), Meta(12), ColorA(24), Color(24), Index(16x4)
		Colors are 24-bit RGB.
		Meta: Holds "logical block format" (or similar).
	1: Tag(4), ColorA(30), Color(30), Index(16x4)
		Colors are 3x10 bit float.
	Index:
		0=A, 1=14/15A+1/15B, ..., 14=1/15A+14/15B, 15=B

Tag=1 ColorBlock:
	ColorA>ColorB
		ColorA(16), ColorB(16), Index(16x2)
			Color=RGB565
			Index: 0=A, 1=B, 2=2/3A+1/3B, 3=1/3A+2/3B
	(ColorA<=ColorB) && (Alpha.Tag==0)
		ColorA(16), ColorB(16), Index(16x2)
			Index: 0=A, 1=B, 2=1/2A+1/2B, 3=Transparent
	(ColorA==ColorB) && (Alpha.Tag!=0)
		ColorA(16), ColorB(16), Tag(4), Meta(12), Index(16)
		Tag 0: Flat Color
		Tag 1: Index Color: Color=[ColorA(8), ColorB(8)]
		...
	(ColorA<ColorB) && (Alpha.Tag!=0)
		ColorA(16), ColorB(16), Tag(4), Meta(12), Index(16)
		Tag 0: Color Index 1bpp (16x1)
		Tag 1: Color Index 2x2 2bpp (4x2)


Metadata:
Generally codec specific.

Possible:
Tag2(4), Data(56)
Tag==0:
	Normal Block (Data is unused)
Tag==1:
	Skip/Translate
	Data holds motion vectors.

*/

#include <bgbbtj.h>

int BGBBTJ_BT1XMB_IndexAlphaY3(int val, int min, int max)
{
	int y, r, v;

	if(val==min)return(0);
	if(val==max)return(1);
//	if(val==0)return(6);
//	if(val==255)return(7);
	
	y=val-min;
	r=max-min;
	
	v=y*(256/(r+1));
//	v=(v+32)>>6;
	v=(v+16)>>5;
	if(v<0)v=0;
	if(v>5)v=5;
	return(v);
}

void BGBBTJ_BT1XMB_EncodeBlockAlpha(byte *block,
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
	
	block[0]=max;
	block[1]=min;

	p0=BGBBTJ_BT1XMB_IndexAlphaY3(rgba[(0*xstride)+(0*ystride)], min, max);
	p1=BGBBTJ_BT1XMB_IndexAlphaY3(rgba[(1*xstride)+(0*ystride)], min, max);
	p2=BGBBTJ_BT1XMB_IndexAlphaY3(rgba[(2*xstride)+(0*ystride)], min, max);
	p3=BGBBTJ_BT1XMB_IndexAlphaY3(rgba[(3*xstride)+(0*ystride)], min, max);

	p4=BGBBTJ_BT1XMB_IndexAlphaY3(rgba[(0*xstride)+(1*ystride)], min, max);
	p5=BGBBTJ_BT1XMB_IndexAlphaY3(rgba[(1*xstride)+(1*ystride)], min, max);
	p6=BGBBTJ_BT1XMB_IndexAlphaY3(rgba[(2*xstride)+(1*ystride)], min, max);
	p7=BGBBTJ_BT1XMB_IndexAlphaY3(rgba[(3*xstride)+(1*ystride)], min, max);

	block[2]=p0|(p1<<3)|(p2<<6);
	block[3]=(p2>>2)|(p3<<1)|(p4<<4)|(p5<<7);
	block[4]=(p5>>1)|(p6<<2)|(p7<<5);
	
	p0=BGBBTJ_BT1XMB_IndexAlphaY3(rgba[(0*xstride)+(2*ystride)], min, max);
	p1=BGBBTJ_BT1XMB_IndexAlphaY3(rgba[(1*xstride)+(2*ystride)], min, max);
	p2=BGBBTJ_BT1XMB_IndexAlphaY3(rgba[(2*xstride)+(2*ystride)], min, max);
	p3=BGBBTJ_BT1XMB_IndexAlphaY3(rgba[(3*xstride)+(2*ystride)], min, max);

	p4=BGBBTJ_BT1XMB_IndexAlphaY3(rgba[(0*xstride)+(3*ystride)], min, max);
	p5=BGBBTJ_BT1XMB_IndexAlphaY3(rgba[(1*xstride)+(3*ystride)], min, max);
	p6=BGBBTJ_BT1XMB_IndexAlphaY3(rgba[(2*xstride)+(3*ystride)], min, max);
	p7=BGBBTJ_BT1XMB_IndexAlphaY3(rgba[(3*xstride)+(3*ystride)], min, max);
	
	block[5]=p0|(p1<<3)|(p2<<6);
	block[6]=(p2>>2)|(p3<<1)|(p4<<4)|(p5<<7);
	block[7]=(p5>>1)|(p6<<2)|(p7<<5);
}

void BGBBTJ_BT1XMB_EncodeBlockColor(byte *block,
	byte *rgba, int xstride, int ystride)
{
//	static const char idxtab[16]=
//		{ 1,1,1,1, 1,1,3,3, 2,2,0,0, 0,0,0,0 };
	static const char idxtab[32]=
		{ 15, 15, 15, 15, 15, 15, 15, 15,
		  15, 14, 13, 12, 11, 10,  9,  8,
		   7,  8,  5,  4,  3,  2,  1,  0,
		   0,  0,  0,  0,  0,  0,  0,  0 };
	byte pxa[16], pxy[16];
	int p0, p1, p2, p3, p4, p5, p6, p7;
	int l0, l1, l2, l3a, l3b;
	int mcr, mcg, mcb, mca, mcy;
	int ncr, ncg, ncb, nca, ncy;
	int acr, acg, acb, aca, acy;
	int cr, cg, cb, ca, cy;
	int i, j, k, l;
	
	mcr=255; mcg=255; mcb=255; mca=255; mcy=255;
	ncr=  0; ncg=  0; ncb=  0; nca=  0; ncy=  0;
	acr=  0; acg=  0; acb=  0; aca=  0; acy=  0;
	
	ca=255;
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		k=(i*ystride)+(j*xstride);
		cr=rgba[k+0];	cg=rgba[k+1];
		cb=rgba[k+2];
		cy=(cr+(2*cg)+cb)>>2;		//Luma

		pxa[i*4+j]=ca;
		pxy[i*4+j]=cy;

		//Find Mins/Maxs
		if(cy<mcy) { mcr=cr; mcg=cg; mcb=cb; mcy=cy; }
		if(cy>ncy) { ncr=cr; ncg=cg; ncb=cb; ncy=cy; }		
		acr+=cr; acg+=cg;
		acb+=cb; acy+=cy;
	}

	aca=(mca+nca)>>1;			//Median Alpha
	acy=(mcy+ncy)>>1;			//Median Luma
	l0=49152/(nca-aca+1);		//Fix-Point Scale (Alpha)
	l1=32768/(ncy-acy+1);		//Fix-Point Scale (Luma)
//	l2=4096;
//	l3=4096;
//	l2=-4096;
//	l3=-4096;
	l2=65536-2048;
//	l3a=65536-2048;
//	l3b=65536-2048;
//	l3b=65536;

//	i=((mcr<<8)&0xF800) | ((mcg<<3)&0x07E0) | ((mcb>>3)&0x001F);
//	j=((ncr<<8)&0xF800) | ((ncg<<3)&0x07E0) | ((ncb>>3)&0x001F);
//		block[ 8]=j;	block[ 9]=j>>8;
//		block[10]=i;	block[11]=i>>8;

	block[0]=0;
	block[1]=0;
	block[2]=ncr;
	block[3]=ncg;
	block[4]=ncb;
	block[5]=mcr;
	block[6]=mcg;
	block[7]=mcb;

	l1=(32768-6144)/(ncy-acy+1);		//Fix-Point Scale (Luma)
	l3a=65536-256;
	l3b=65536+256;
//	l3a=65536-1024;
//	l3b=65536+1024;

	p0=idxtab[((pxy[ 0]-acy)*l1+l3a)>>12];
	p1=idxtab[((pxy[ 1]-acy)*l1+l3b)>>12];
	p2=idxtab[((pxy[ 2]-acy)*l1+l3a)>>12];
	p3=idxtab[((pxy[ 3]-acy)*l1+l3b)>>12];
	block[8]=(p1<<4)|p0;
	block[9]=(p3<<4)|p2;
	p0=idxtab[((pxy[ 4]-acy)*l1+l3b)>>12];
	p1=idxtab[((pxy[ 5]-acy)*l1+l3a)>>12];
	p2=idxtab[((pxy[ 6]-acy)*l1+l3b)>>12];
	p3=idxtab[((pxy[ 7]-acy)*l1+l3a)>>12];
	block[10]=(p1<<4)|p0;
	block[11]=(p3<<4)|p2;
	p0=idxtab[((pxy[ 8]-acy)*l1+l3a)>>12];
	p1=idxtab[((pxy[ 9]-acy)*l1+l3b)>>12];
	p2=idxtab[((pxy[10]-acy)*l1+l3a)>>12];
	p3=idxtab[((pxy[11]-acy)*l1+l3b)>>12];
	block[12]=(p1<<4)|p0;
	block[13]=(p3<<4)|p2;
	p0=idxtab[((pxy[12]-acy)*l1+l3b)>>12];
	p1=idxtab[((pxy[13]-acy)*l1+l3a)>>12];
	p2=idxtab[((pxy[14]-acy)*l1+l3b)>>12];
	p3=idxtab[((pxy[15]-acy)*l1+l3a)>>12];
	block[14]=(p1<<4)|p0;
	block[15]=(p3<<4)|p2;
}

void BGBBTJ_BT1XMB_EncodeBlock(byte *block,
	byte *rgba, int xstride, int ystride)
{
	bgbbtj_bt1c_memzero8(block);
	BGBBTJ_BT1XMB_EncodeBlockAlpha(block+ 8, rgba+3, xstride, ystride);
	BGBBTJ_BT1XMB_EncodeBlockColor(block+16, rgba+0, xstride, ystride);
}


void BGBBTJ_BT1XMB_DecodeBlockAlpha(byte *block,
	byte *rgba, int xstride, int ystride)
{
	byte val[8];
	int i, j, k, l;
	
	if(block[0]>block[1])
	{
		val[0]=block[0];
		val[1]=block[1];
		val[2]=(val[0]*219+val[1]* 37)>>8;
		val[3]=(val[0]*183+val[1]* 73)>>8;
		val[4]=(val[0]*146+val[1]*110)>>8;
		val[5]=(val[0]*110+val[1]*146)>>8;
		val[6]=(val[0]* 73+val[1]*183)>>8;
		val[7]=(val[0]* 37+val[1]*219)>>8;

		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
		{
			k=(i*4+j)*3;
			l=((block[2+(k>>3)]|(block[3+(k>>3)]<<8))>>(k&7))&7;
			k=i*ystride+j*xstride;
			rgba[k]=val[l];
		}
	}else
	{
		val[0]=block[0];
		val[1]=block[1];
		val[2]=(val[0]*11+val[1]* 5)>>4;
		val[3]=(val[0]* 5+val[1]*11)>>4;

		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
		{
			k=i*ystride+j*xstride;
			l=(block[4+i]>>(2*j))&3;
			rgba[k]=val[l];
		}
	}
}

void BGBBTJ_BT1XMB_DecodeBlockColor0(byte *block,
	byte *rgba, int xstride, int ystride)
{
	byte clr[16*4];
	int i, j, k, l;

//	clr[ 0]=block[1]&0xF8;
//	clr[ 1]=((block[1]<<5)|(block[0]>>3))&0xFC;
//	clr[ 2]=(block[0]<<3)&0xF8;
//	clr[ 4]=block[3]&0xF8;
//	clr[ 5]=((block[3]<<5)|(block[2]>>3))&0xFC;
//	clr[ 6]=(block[2]<<3)&0xF8;	

	clr[ 0*4+0]=block[2];
	clr[ 0*4+1]=block[3];
	clr[ 0*4+2]=block[4];
	clr[15*4+0]=block[5];
	clr[15*4+1]=block[6];
	clr[15*4+2]=block[7];

	for(i=1; i<15; i++)
	{
		clr[i*4+0]=(clr[0]*(16-i)+clr[4]*i)>>4;
		clr[i*4+1]=(clr[1]*(16-i)+clr[5]*i)>>4;
		clr[i*4+2]=(clr[2]*(16-i)+clr[6]*i)>>4;
	}
	
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		k=i*ystride+j*xstride;
//		l=((block[4+i]>>(2*j))&3)*4;
		l=((block[8+((4*i+j)>>1)]>>(4*(j&1)))&15)*4;
		rgba[k+0]=clr[l+0];
		rgba[k+1]=clr[l+1];
		rgba[k+2]=clr[l+2];
	}
}

void BGBBTJ_BT1XMB_DecodeBlockColor1(byte *block,
	byte *rgba, int xstride, int ystride)
{
}

void BGBBTJ_BT1XMB_DecodeBlockColor(byte *block,
	byte *rgba, int xstride, int ystride)
{
	switch(block[0]&15)
	{
	case 0:
		BGBBTJ_BT1XMB_DecodeBlockColor0(block, rgba, xstride, ystride);
		break;
	default:
		break;
	}
}

void BGBBTJ_BT1XMB_DecodeBlockColorB0(byte *block,
	byte *rgba, int xstride, int ystride)
{
	byte clr[4*4];
	int i, j, k, l;
	
	i=block[0]|(block[1]<<8);
	j=block[2]|(block[3]<<8);

#if 1
	if(i==j)
	{
		k=(block[4]^block[5])|(block[5]^block[6])|(block[6]^block[7]);
		if(!k)
		{
			clr[0]=block[1]&0xF8;
			clr[1]=((block[1]<<5)|(block[0]>>3))&0xFC;
			clr[2]=(block[0]<<3)&0xF8;
			clr[3]=(block[4]==0xFF)?0:255;
			clr[0]|=(clr[0]>>5);
			clr[1]|=(clr[1]>>6);
			clr[2]|=(clr[2]>>5);
			
			for(i=0; i<4; i++)
				for(j=0; j<4; j++)
			{
				k=i*ystride+j*xstride;
				rgba[k+0]=clr[0];
				rgba[k+1]=clr[1];
				rgba[k+2]=clr[2];
				rgba[k+3]=clr[3];
			}
			return;
		}
	}
#endif

	if(i<=j)
	{
		clr[ 0]=block[1]&0xF8;
		clr[ 1]=((block[1]<<5)|(block[0]>>3))&0xFC;
		clr[ 2]=(block[0]<<3)&0xF8;
		clr[ 3]=255;
		clr[ 4]=block[3]&0xF8;
		clr[ 5]=((block[3]<<5)|(block[2]>>3))&0xFC;
		clr[ 6]=(block[2]<<3)&0xF8;	
		clr[ 7]=255;

		clr[ 0]|=(clr[ 0]>>5);
		clr[ 1]|=(clr[ 1]>>6);
		clr[ 2]|=(clr[ 2]>>5);
		clr[ 4]|=(clr[ 4]>>5);
		clr[ 5]|=(clr[ 5]>>6);
		clr[ 6]|=(clr[ 6]>>5);

		clr[ 8]=(clr[0]+clr[4])>>1;
		clr[ 9]=(clr[1]+clr[5])>>1;
		clr[10]=(clr[2]+clr[6])>>1;
		clr[11]=255;
		clr[12]=(clr[0]+clr[4])>>1;
		clr[13]=(clr[1]+clr[5])>>1;
		clr[14]=(clr[2]+clr[6])>>1;
		clr[15]=0;
	}else
	{
		clr[ 0]=block[1]&0xF8;
		clr[ 1]=((block[1]<<5)|(block[0]>>3))&0xFC;
		clr[ 2]=(block[0]<<3)&0xF8;
		clr[ 3]=255;
		clr[ 4]=block[3]&0xF8;
		clr[ 5]=((block[3]<<5)|(block[2]>>3))&0xFC;
		clr[ 6]=(block[2]<<3)&0xF8;	
		clr[ 7]=255;

		clr[ 0]|=(clr[ 0]>>5);
		clr[ 1]|=(clr[ 1]>>6);
		clr[ 2]|=(clr[ 2]>>5);
		clr[ 4]|=(clr[ 4]>>5);
		clr[ 5]|=(clr[ 5]>>6);
		clr[ 6]|=(clr[ 6]>>5);

		clr[ 8]=(clr[0]*11+clr[4]*5)>>4;
		clr[ 9]=(clr[1]*11+clr[5]*5)>>4;
		clr[10]=(clr[2]*11+clr[6]*5)>>4;
		clr[11]=255;
		clr[12]=(clr[0]*5+clr[4]*11)>>4;
		clr[13]=(clr[1]*5+clr[5]*11)>>4;
		clr[14]=(clr[2]*5+clr[6]*11)>>4;
		clr[15]=255;
	}
	
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		k=i*ystride+j*xstride;
		l=((block[4+i]>>(2*j))&3)*4;
		rgba[k+0]=clr[l+0];
		rgba[k+1]=clr[l+1];
		rgba[k+2]=clr[l+2];
		rgba[k+3]=clr[l+3];
	}
}

void BGBBTJ_BT1XMB_DecodeBlockColorB1(byte *block,
	byte *rgba, int xstride, int ystride)
{
	byte clr[4*4];
	int i0, i1, i2, i3;
	int i, j, k, l, t;
	
	i=block[0]|(block[1]<<8);
	j=block[2]|(block[3]<<8);

#if 1
	if(i==j)
	{
		k=(block[4]^block[5])|(block[5]^block[6])|(block[6]^block[7]);
		if(!k)
		{
			clr[0]=block[1]&0xF8;
			clr[1]=((block[1]<<5)|(block[0]>>3))&0xFC;
			clr[2]=(block[0]<<3)&0xF8;
			clr[3]=(block[4]==0xFF)?0:255;
			clr[0]|=(clr[0]>>5);
			clr[1]|=(clr[1]>>6);
			clr[2]|=(clr[2]>>5);
			
			for(i=0; i<4; i++)
				for(j=0; j<4; j++)
			{
				k=i*ystride+j*xstride;
				rgba[k+0]=clr[0];
				rgba[k+1]=clr[1];
				rgba[k+2]=clr[2];
				rgba[k+3]=clr[3];
			}
			return;
		}
	}
#endif

	if(i<=j)
	{
		clr[ 0]=block[1]&0xF8;
		clr[ 1]=((block[1]<<5)|(block[0]>>3))&0xFC;
		clr[ 2]=(block[0]<<3)&0xF8;
		clr[ 3]=255;
		clr[ 4]=block[3]&0xF8;
		clr[ 5]=((block[3]<<5)|(block[2]>>3))&0xFC;
		clr[ 6]=(block[2]<<3)&0xF8;	
		clr[ 7]=255;

		clr[ 0]|=(clr[ 0]>>5);
		clr[ 1]|=(clr[ 1]>>6);
		clr[ 2]|=(clr[ 2]>>5);
		clr[ 4]|=(clr[ 4]>>5);
		clr[ 5]|=(clr[ 5]>>6);
		clr[ 6]|=(clr[ 6]>>5);

		clr[ 8]=(clr[0]+clr[4])>>1;
		clr[ 9]=(clr[1]+clr[5])>>1;
		clr[10]=(clr[2]+clr[6])>>1;
		clr[11]=255;
		clr[12]=(clr[0]+clr[4])>>1;
		clr[13]=(clr[1]+clr[5])>>1;
		clr[14]=(clr[2]+clr[6])>>1;
		clr[15]=0;
		
		t=block[4]&15;
		switch(t)
		{
		case 0:
			i0=block[6]|(block[7]<<8);
			for(i=0; i<4; i++)
				for(j=0; j<4; j++)
			{
				k=i*ystride+j*xstride;
				l=((i0>>(i*4+j))&1)*4;
				rgba[k+0]=clr[l+0];
				rgba[k+1]=clr[l+1];
				rgba[k+2]=clr[l+2];
				rgba[k+3]=clr[l+3];
			}
			break;
		case 1:
			l=block[6];
			for(i=0; i<2; i++)
				for(j=0; j<2; j++)
			{
				i0=(i*2+0)*ystride+(j*2+0)*xstride;
				i1=(i*2+0)*ystride+(j*2+1)*xstride;
				i2=(i*2+1)*ystride+(j*2+0)*xstride;
				i3=(i*2+1)*ystride+(j*2+1)*xstride;
				k=((l>>(i*4+j))&1)*4;
				rgba[i0+0]=clr[k+0];	rgba[i0+1]=clr[k+1];
				rgba[i0+2]=clr[k+2];	rgba[i0+3]=clr[k+3];
				rgba[i1+0]=clr[k+0];	rgba[i1+1]=clr[k+1];
				rgba[i1+2]=clr[k+2];	rgba[i1+3]=clr[k+3];
				rgba[i2+0]=clr[k+0];	rgba[i2+1]=clr[k+1];
				rgba[i2+2]=clr[k+2];	rgba[i2+3]=clr[k+3];
				rgba[i3+0]=clr[k+0];	rgba[i3+1]=clr[k+1];
				rgba[i3+2]=clr[k+2];	rgba[i3+3]=clr[k+3];
			}
			break;
		default:
			break;
		}
	}else
	{
		clr[ 0]=block[1]&0xF8;
		clr[ 1]=((block[1]<<5)|(block[0]>>3))&0xFC;
		clr[ 2]=(block[0]<<3)&0xF8;
		clr[ 3]=255;
		clr[ 4]=block[3]&0xF8;
		clr[ 5]=((block[3]<<5)|(block[2]>>3))&0xFC;
		clr[ 6]=(block[2]<<3)&0xF8;	
		clr[ 7]=255;

		clr[ 0]|=(clr[ 0]>>5);
		clr[ 1]|=(clr[ 1]>>6);
		clr[ 2]|=(clr[ 2]>>5);
		clr[ 4]|=(clr[ 4]>>5);
		clr[ 5]|=(clr[ 5]>>6);
		clr[ 6]|=(clr[ 6]>>5);

		clr[ 8]=(clr[0]*11+clr[4]*5)>>4;
		clr[ 9]=(clr[1]*11+clr[5]*5)>>4;
		clr[10]=(clr[2]*11+clr[6]*5)>>4;
		clr[11]=255;
		clr[12]=(clr[0]*5+clr[4]*11)>>4;
		clr[13]=(clr[1]*5+clr[5]*11)>>4;
		clr[14]=(clr[2]*5+clr[6]*11)>>4;
		clr[15]=255;

		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
		{
			k=i*ystride+j*xstride;
			l=((block[4+i]>>(2*j))&3)*4;
			rgba[k+0]=clr[l+0];
			rgba[k+1]=clr[l+1];
			rgba[k+2]=clr[l+2];
			rgba[k+3]=clr[l+3];
		}
	}
}

int BGBBTJ_BT1XMB_GetBlockAlphaTag(byte *block)
{
	if(block[0]>block[1])
		return(0);
	return(block[2]&15);
}

void BGBBTJ_BT1XMB_DecodeBlock(byte *block,
	byte *rgba, int xstride, int ystride)
{
	switch(block[0]&15)
	{
	case 0:
		BGBBTJ_BT1XMB_DecodeBlockAlpha(block+ 8, rgba+3, xstride, ystride);
		BGBBTJ_BT1XMB_DecodeBlockColor(block+16, rgba+0, xstride, ystride);
		break;
	case 1:
		BGBBTJ_BT1XMB_DecodeBlockAlpha(block+ 8, rgba+3, xstride, ystride);
		if(BGBBTJ_BT1XMB_GetBlockAlphaTag(block+ 8))
		{
			BGBBTJ_BT1XMB_DecodeBlockColorB1(block+16,
				rgba+0, xstride, ystride);
		}else
		{
			BGBBTJ_BT1XMB_DecodeBlockColorB0(block+16,
				rgba+0, xstride, ystride);
		}
		break;
	default:
		break;
	}
}


int BGBBTJ_BT1XMB_CheckBlockSkipP(byte *blk, byte *lblk)
{
	if(!memcmp(blk, lblk, 16))
		return(1);
	return(0);
}

int BGBBTJ_BT1XMB_CheckBlockTranslateP(byte *blk, byte *lblk,
	int x, int y)
{
	return(0);
}

int BGBBTJ_BT1XMB_GetBlockTranslate(byte *block, int *rx, int *ry)
{
	return(0);
}

int BGBBTJ_BT1XMB_IsFlatColorP(byte *blk)
{
	int i0, i1;

	if(BGBBTJ_BT1XMB_GetColorB_IsRGB565P(blk))
	{
		i0=BGBBTJ_BT1XMB_GetColorA_As565(blk);
		i1=BGBBTJ_BT1XMB_GetColorB_As565(blk);
		return(i0==i1);
	}

	if(BGBBTJ_BT1XMB_GetColorB_IsRGB787P(blk))
	{
		i0=BGBBTJ_BT1XMB_GetColorA_As787(blk);
		i1=BGBBTJ_BT1XMB_GetColorB_As787(blk);
		return(i0==i1);
	}
	
	return(0);
}

int BGBBTJ_BT1XMB_IsSimpleOpaqueP(byte *blk)
{
	return(1);
}

int BGBBTJ_BT1XMB_Is1BppP(byte *blk)
{
	int i;
	if((blk[0]&15)==1)
	{
		i=	(!(blk[16+4]&0xAA)) && (!(blk[16+5]&0xAA)) &&
			(!(blk[16+6]&0xAA)) && (!(blk[16+7]&0xAA));
		return(i);
	}
	return(0);
}

int BGBBTJ_BT1XMB_Is2x2P(byte *blk)
{
	int i;
	if((blk[0]&15)==1)
	{
		i=	 ((blk[16+4]==blk[16+5]) &&
			  (blk[16+6]==blk[16+7]) &&
			(((blk[16+4]>>2)&0x33)==(blk[16+4]&0x33)) &&
			(((blk[16+6]>>2)&0x33)==(blk[16+7]&0x33)));
		return(i);
	}
	return(0);
}

int BGBBTJ_BT1XMB_As1Bpp(byte *blk)
{
	int p0, p1, p2, p3, p4;
	if((blk[0]&15)==1)
	{
		p0= ((blk[16+4]& 1)<<3)| (blk[16+4]& 4)|
			((blk[16+4]&16)>>3)|((blk[16+4]&64)>>6);
		p1= ((blk[16+5]& 1)<<3)| (blk[16+5]& 4)|
			((blk[16+5]&16)>>3)|((blk[16+5]&64)>>6);
		p2= ((blk[16+6]& 1)<<3)| (blk[16+6]& 4)|
			((blk[16+6]&16)>>3)|((blk[16+6]&64)>>6);
		p3= ((blk[16+7]& 1)<<3)| (blk[16+7]& 4)|
			((blk[16+7]&16)>>3)|((blk[16+7]&64)>>6);
		p4=(p0<<12)|(p1<<8)|(p2<<4)|p3;
		return(p4);
	}
	return(0);
}

int BGBBTJ_BT1XMB_As2x2x2bpp(byte *blk)
{
	int p0, p1, p2, p3, p4;
	if((blk[0]&15)==1)
	{
		p0=((blk[16+4]&0x03)<<2)|((blk[16+4]&0x30)>>4);
		p1=((blk[16+6]&0x03)<<2)|((blk[16+6]&0x30)>>4);
		p2=(p0<<4)|p1;
		return(p2);
	}
	return(0);
}

u32 BGBBTJ_BT1XMB_As4x4x2bpp(byte *blk)
{
	u32 p0;
	if((blk[0]&15)==1)
	{
		p0=	(blk[16+4]    )|(blk[16+5]<< 8)|
			(blk[16+6]<<16)|(blk[16+7]<<24);
		return(p0);
	}
	return(0);
}

int BGBBTJ_BT1XMB_FullyTransparentP(byte *blk)
{
	u32 p0;

	if((blk[0]&15)==1)
	{
		p0=	(blk[16+4]    )|(blk[16+5]<< 8)|
			(blk[16+6]<<16)|(blk[16+7]<<24);
		if(p0==0xFFFFFFFF)
			return(1);
		if((blk[8+0]==0) && (blk[8+1]==0))
			return(1);
		return(0);
	}

	if((blk[0]&15)==0)
	{
		if((blk[8+0]==0) && (blk[8+1]==0))
			return(1);
		return(0);
	}

	return(0);
}

u16 BGBBTJ_BT1XMB_GetColorA_As565(byte *blk)
{
	int i, j, k;

	if((blk[0]&15)==1)
		{ return(blk[16]|(blk[17]<<8)); }
	if((blk[0]&15)==0)
	{
		if((blk[16]&15)==0)
		{
			i=	((blk[16+2]<<8)&0xF800) |
				((blk[16+3]<<3)&0x07E0) |
				((blk[16+4]>>3)&0x001F);
			return(i);
		}
	}
	return(0);
}

u16 BGBBTJ_BT1XMB_GetColorB_As565(byte *blk)
{
	int i, j, k;

	if((blk[0]&15)==1)
		{ return(blk[18]|(blk[19]<<8)); }
	if((blk[0]&15)==0)
	{
		if((blk[16]&15)==0)
		{
			i=	((blk[16+5]<<8)&0xF800) |
				((blk[16+6]<<3)&0x07E0) |
				((blk[16+7]>>3)&0x001F);
			return(i);
		}
	}
	return(0);
}

u32 BGBBTJ_BT1XMB_GetColorA_As787(byte *blk)
{
	int cr, cg, cb;
	int i, j, k;

	if((blk[0]&15)==1)
	{
		i=blk[16]|(blk[17]<<8);
		cr=(i>>11)&31; cr=(cr<<3)|(cr>>2);
		cg=(i>> 5)&63; cg=(cg<<2)|(cg>>4);
		cb=(i>>11)&31; cb=(cb<<3)|(cb>>2);
		j=((cr&1)&(cg&1))|((cg&1)&(cb&1));
		i=((cr&0xFE)<<14)|(cg<<7)|(cb>>1)|(j<<22);
		return(i);
	}

	if((blk[0]&15)==0)
	{
		if((blk[16]&15)==0)
		{
			cr=blk[16+2]; cg=blk[16+3]; cb=blk[16+4];
			j=((cr&1)&(cg&1))|((cg&1)&(cb&1));
			i=((cr&0xFE)<<14)|(cg<<7)|(cb>>1)|(j<<22);
			return(i);
		}
	}

	return(0);
}

u32 BGBBTJ_BT1XMB_GetColorB_As787(byte *blk)
{
	int cr, cg, cb;
	int i, j, k;

	if((blk[0]&15)==1)
	{
		i=blk[18]|(blk[19]<<8);
		cr=(i>>11)&31; cr=(cr<<3)|(cr>>2);
		cg=(i>> 5)&63; cg=(cg<<2)|(cg>>4);
		cb=(i>>11)&31; cb=(cb<<3)|(cb>>2);
		j=((cr&1)&(cg&1))|((cg&1)&(cb&1));
		i=((cr&0xFE)<<14)|(cg<<7)|(cb>>1)|(j<<22);
		return(i);
	}

	if((blk[0]&15)==0)
	{
		if((blk[16]&15)==0)
		{
			cr=blk[16+5]; cg=blk[16+6]; cb=blk[16+7];
			j=((cr&1)&(cg&1))|((cg&1)&(cb&1));
			i=((cr&0xFE)<<14)|(cg<<7)|(cb>>1)|(j<<22);
			return(i);
		}
	}

	return(0);
}

bool BGBBTJ_BT1XMB_GetColorB_IsRGB565P(byte *blk)
{
	if((blk[0]&15)==1)
		return(1);
	return(0);
}

bool BGBBTJ_BT1XMB_GetColorB_IsRGB787P(byte *blk)
{
	if((blk[0]&15)==0)
	{
		if((blk[16]&15)==0)
			return(1);
		return(0);
	}
	return(0);
}

int BGBBTJ_BT1XMB_IsColorMatchP(byte *blka, byte *blkb)
{
	int i0, i1, i2, i3;

	if(((blka[0]&15)==1) && ((blkb[0]&15)==1))
	{
		i0=BGBBTJ_BT1XMB_GetColorA_As565(blka);
		i1=BGBBTJ_BT1XMB_GetColorB_As565(blka);
		i2=BGBBTJ_BT1XMB_GetColorA_As565(blkb);
		i3=BGBBTJ_BT1XMB_GetColorB_As565(blkb);
		return((i0==i2) && (i1==i3));
	}

	if(((blka[0]&15)==0) && ((blkb[0]&15)==0))
	{
		if(((blka[16]&15)==0) && ((blkb[16]&15)==0))
		{
			i0=BGBBTJ_BT1XMB_GetColorA_As787(blka);
			i1=BGBBTJ_BT1XMB_GetColorB_As787(blka);
			i2=BGBBTJ_BT1XMB_GetColorA_As787(blkb);
			i3=BGBBTJ_BT1XMB_GetColorB_As787(blkb);
			return((i0==i2) && (i1==i3));
		}
	}
	
	return(0);
}

