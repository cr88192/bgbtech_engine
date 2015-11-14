/*
Filter for DXTn / BCn texture compression.

Pixel Block:
	A B C D
	E F G H
	I J K L
	M N O P

Color: 5:6:5
	Red: Bits 11-15
	Green: Bits 5-10
	Blue: Bits 0-4
	
DXT1 / BC1
color_0: Color
color_1: Color
pixels: bytes[4] (2 bpp)
	DCBA
	HGFE
	LKJI
	PONM
	
DXT5 / BC3
alpha_0: byte
alpha_1: byte
alphas: byte[6] (3bpp)
color_0: Color
color_1: Color
pixels: bytes[4]

DXT5 Alpha / BC4: 
	alpha_0: byte
	alpha_1: byte
	alphas: byte[6] (3bpp)

if(alpha_0<=alpha_1)
{
	0=alpha_0, 1=alpha_1;
	2-5=interpolated alphas;
	6=0, 7=255.
}else
{
	0=alpha_0, 1=alpha_1;
	2-7=interpolated alphas;
}

UVAY
	A holds Y.
	RG holds UV.
	B holds Alpha / Scale
		0-127: Alpha=2.0*B, Scale=1.0.
		128-255: Alpha=1.0, Scale=1.0-(2.0*B-1.0).

Assertion: For alpha-blending, reduced color precision is acceptable, 

*/

#include <bgbbtj.h>



#if 0
void BGBBTJ_BCn_EncodeBlockDXT5(byte *block,
	byte *rgba, int xstride, int ystride)
{
	static const char idxtab[16]=
		{ 0,0,0,0, 0,0,2,2, 3,3,1,1, 1,1,1,1 };
	static const char idxtab2[16]=
		{ 0,0,0,0, 2,2,3,3, 4,4,5,5, 1,1,1,1 };
	byte pxa[16], pxy[16];
	int p0, p1, p2, p3, p4, p5, p6, p7;
	int l0, l1, l2, l3;
	int mcr, mcg, mcb, mca, mcy;
	int ncr, ncg, ncb, nca, ncy;
	int aca, acy;
	int cr, cg, cb, ca, cy;
	int i, j, k, l;
	
	mcr=255; mcg=255; mcb=255; mca=255; mcy=255;
	ncr=  0; ncg=  0; ncb=  0; nca=  0; ncy=  0;
	
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		k=(i*ystride)+(j*xstride);
		cr=rgba[k+0];	cg=rgba[k+1];
		cb=rgba[k+2];	ca=rgba[k+3];
		cy=(cr+(2*cg)+cb)>>2;		//Luma

		pxa[i*4+j]=ca;
		pxy[i*4+j]=cy;

		//Find Mins/Maxs
		if(cy<mcy) { mcr=cr; mcg=cg; mcb=cb; mcy=cy; }
		if(cy>ncy) { ncr=cr; ncg=cg; ncb=cb; ncy=cy; }
		if(ca<mca) { mca=ca; }
		if(ca>nca) { nca=ca; }
	}

	aca=(mca+nca)>>1;			//Median Alpha
	acy=(mcy+ncy)>>1;			//Median Luma
	l0=49152/(nca-aca+1);		//Fix-Point Scale (Alpha)
	l1=32768/(ncy-acy+1);		//Fix-Point Scale (Luma)
//	l2=4096;
//	l3=4096;
//	l2=-4096;
//	l3=-4096;
	l2=-2048;
	l3=-2048;

	//Emit Y Block
	block[0]=mca;
	block[1]=nca;
	
	p0=idxtab2[(((pxa[ 0]-aca)*l0+l2)>>13)+8];
	p1=idxtab2[(((pxa[ 1]-aca)*l0+l2)>>13)+8];
	p2=idxtab2[(((pxa[ 2]-aca)*l0+l2)>>13)+8];
	p3=idxtab2[(((pxa[ 3]-aca)*l0+l2)>>13)+8];
	p4=idxtab2[(((pxa[ 4]-aca)*l0+l2)>>13)+8];
	p5=idxtab2[(((pxa[ 5]-aca)*l0+l2)>>13)+8];
	p6=idxtab2[(((pxa[ 6]-aca)*l0+l2)>>13)+8];
	p7=idxtab2[(((pxa[ 7]-aca)*l0+l2)>>13)+8];
	block[2]=p0|(p1<<3)|(p2<<6);
	block[3]=(p2>>2)|(p3<<1)|(p4<<4)|(p5<<7);
	block[4]=(p5>>1)|(p6<<2)|(p7<<5);
	p0=idxtab2[(((pxa[ 8]-aca)*l0+l2)>>13)+8];
	p1=idxtab2[(((pxa[ 9]-aca)*l0+l2)>>13)+8];
	p2=idxtab2[(((pxa[10]-aca)*l0+l2)>>13)+8];
	p3=idxtab2[(((pxa[11]-aca)*l0+l2)>>13)+8];
	p4=idxtab2[(((pxa[12]-aca)*l0+l2)>>13)+8];
	p5=idxtab2[(((pxa[13]-aca)*l0+l2)>>13)+8];
	p6=idxtab2[(((pxa[14]-aca)*l0+l2)>>13)+8];
	p7=idxtab2[(((pxa[15]-aca)*l0+l2)>>13)+8];
	block[5]=p0|(p1<<3)|(p2<<6);
	block[6]=(p2>>2)|(p3<<1)|(p4<<4)|(p5<<7);
	block[7]=(p5>>1)|(p6<<2)|(p7<<5);

	//Emit UVA Block
	i=((mcr<<8)&0xF800) | ((mcg<<3)&0x07E0) | ((mcb>>3)&0x001F);
	j=((ncr<<8)&0xF800) | ((ncg<<3)&0x07E0) | ((ncb>>3)&0x001F);
	block[ 8]=i;	block[ 9]=i>>8;
	block[10]=j;	block[11]=j>>8;

	p0=idxtab[(((pxy[ 0]-acy)*l1+l3)>>13)+8];
	p1=idxtab[(((pxy[ 1]-acy)*l1+l3)>>13)+8];
	p2=idxtab[(((pxy[ 2]-acy)*l1+l3)>>13)+8];
	p3=idxtab[(((pxy[ 3]-acy)*l1+l3)>>13)+8];
	block[12]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
	p0=idxtab[(((pxy[ 4]-acy)*l1+l3)>>13)+8];
	p1=idxtab[(((pxy[ 5]-acy)*l1+l3)>>13)+8];
	p2=idxtab[(((pxy[ 6]-acy)*l1+l3)>>13)+8];
	p3=idxtab[(((pxy[ 7]-acy)*l1+l3)>>13)+8];
	block[13]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
	p0=idxtab[(((pxy[ 8]-acy)*l1+l3)>>13)+8];
	p1=idxtab[(((pxy[ 9]-acy)*l1+l3)>>13)+8];
	p2=idxtab[(((pxy[10]-acy)*l1+l3)>>13)+8];
	p3=idxtab[(((pxy[11]-acy)*l1+l3)>>13)+8];
	block[14]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
	p0=idxtab[(((pxy[12]-acy)*l1+l3)>>13)+8];
	p1=idxtab[(((pxy[13]-acy)*l1+l3)>>13)+8];
	p2=idxtab[(((pxy[14]-acy)*l1+l3)>>13)+8];
	p3=idxtab[(((pxy[15]-acy)*l1+l3)>>13)+8];
	block[15]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
}
#endif

#if 1
void BGBBTJ_BCn_EncodeBlockDXT5(byte *block,
	byte *rgba, int xstride, int ystride)
{
	static const char idxtab[16]=
		{ 0,0,0,0, 0,0,2,2, 3,3,1,1, 1,1,1,1 };
	static const char idxtab2[16]=
		{ 0,0,0,0, 2,2,3,3, 4,4,5,5, 1,1,1,1 };
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
	
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		k=(i*ystride)+(j*xstride);
		cr=rgba[k+0];	cg=rgba[k+1];
		cb=rgba[k+2];	ca=rgba[k+3];
		cy=(cr+(2*cg)+cb)>>2;		//Luma

		pxa[i*4+j]=ca;
		pxy[i*4+j]=cy;

		//Find Mins/Maxs
		if(cy<mcy) { mcr=cr; mcg=cg; mcb=cb; mcy=cy; }
		if(cy>ncy) { ncr=cr; ncg=cg; ncb=cb; ncy=cy; }
		if(ca<mca) { mca=ca; }
		if(ca>nca) { nca=ca; }
		
		acr+=cr; acg+=cg;
		acb+=cb; acy+=cy;
	}

#if 1
	acr=(acr+8)>>4;
	acg=(acg+8)>>4;
	acb=(acb+8)>>4;
	acy=(acr+(2*acg)+acb)>>2;
	
	l=(256*mcy)/(acy+1);
	cr=(acr*l+128)>>8;
	cg=(acg*l+128)>>8;
	cb=(acb*l+128)>>8;
	
//	mcr=(11*mcr+5*cr+8)>>4;
//	mcg=(11*mcg+5*cg+8)>>4;
//	mcb=(11*mcb+5*cb+8)>>4;
//	mcr=(5*mcr+11*cr+8)>>4;
//	mcg=(5*mcg+11*cg+8)>>4;
//	mcb=(5*mcb+11*cb+8)>>4;
	mcr=(mcr+cr)>>1;
	mcg=(mcg+cg)>>1;
	mcb=(mcb+cb)>>1;
	

	l=(256*ncy)/(acy+1);
	cr=(acr*l+128)>>8;
	cg=(acg*l+128)>>8;
	cb=(acb*l+128)>>8;
	
//	ncr=(11*ncr+5*cr+8)>>4;
//	ncg=(11*ncg+5*cg+8)>>4;
//	ncb=(11*ncb+5*cb+8)>>4;
//	ncr=(5*ncr+11*cr+8)>>4;
//	ncg=(5*ncg+11*cg+8)>>4;
//	ncb=(5*ncb+11*cb+8)>>4;
	ncr=(ncr+cr)>>1;
	ncg=(ncg+cg)>>1;
	ncb=(ncb+cb)>>1;
#endif

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

	if(mca==nca)
	{
		block[0]=mca; block[1]=nca;
		block[2]=0;	block[3]=0;
		block[4]=0;	block[5]=0;
		block[6]=0;	block[7]=0;
	}else
	{
		//Emit Alpha Block
		block[0]=mca;
		block[1]=nca;
	
		p0=idxtab2[((pxa[ 0]-aca)*l0+l2)>>13];
		p1=idxtab2[((pxa[ 1]-aca)*l0+l2)>>13];
		p2=idxtab2[((pxa[ 2]-aca)*l0+l2)>>13];
		p3=idxtab2[((pxa[ 3]-aca)*l0+l2)>>13];
		p4=idxtab2[((pxa[ 4]-aca)*l0+l2)>>13];
		p5=idxtab2[((pxa[ 5]-aca)*l0+l2)>>13];
		p6=idxtab2[((pxa[ 6]-aca)*l0+l2)>>13];
		p7=idxtab2[((pxa[ 7]-aca)*l0+l2)>>13];
		block[2]=p0|(p1<<3)|(p2<<6);
		block[3]=(p2>>2)|(p3<<1)|(p4<<4)|(p5<<7);
		block[4]=(p5>>1)|(p6<<2)|(p7<<5);
		p0=idxtab2[((pxa[ 8]-aca)*l0+l2)>>13];
		p1=idxtab2[((pxa[ 9]-aca)*l0+l2)>>13];
		p2=idxtab2[((pxa[10]-aca)*l0+l2)>>13];
		p3=idxtab2[((pxa[11]-aca)*l0+l2)>>13];
		p4=idxtab2[((pxa[12]-aca)*l0+l2)>>13];
		p5=idxtab2[((pxa[13]-aca)*l0+l2)>>13];
		p6=idxtab2[((pxa[14]-aca)*l0+l2)>>13];
		p7=idxtab2[((pxa[15]-aca)*l0+l2)>>13];
		block[5]=p0|(p1<<3)|(p2<<6);
		block[6]=(p2>>2)|(p3<<1)|(p4<<4)|(p5<<7);
		block[7]=(p5>>1)|(p6<<2)|(p7<<5);
	}

	if((mcr==ncr) && (mcg==ncg) && (mcb==ncb))
	{
		//Flat Color Block
#if 1
		mcr=((mcr+3)>>3)<<3;
		mcg=((mcg+1)>>2)<<2;
		mcb=((mcb+3)>>3)<<3;
		ncr=((ncr+5)>>3)<<3;
		ncg=((ncg+3)>>2)<<2;
		ncb=((ncb+5)>>3)<<3;
		mcr=(mcr<0)?0:((mcr>255)?255:mcr);
		mcg=(mcg<0)?0:((mcg>255)?255:mcg);
		mcb=(mcb<0)?0:((mcb>255)?255:mcb);
		ncr=(ncr<0)?0:((ncr>255)?255:ncr);
		ncg=(ncg<0)?0:((ncg>255)?255:ncg);
		ncb=(ncb<0)?0:((ncb>255)?255:ncb);
#endif
		i=((mcr<<8)&0xF800) | ((mcg<<3)&0x07E0) | ((mcb>>3)&0x001F);
		j=((ncr<<8)&0xF800) | ((ncg<<3)&0x07E0) | ((ncb>>3)&0x001F);
		block[ 8]=i;	block[ 9]=i>>8;
		block[10]=j;	block[11]=j>>8;
		block[12]=(2<<6)|(3<<4)|(2<<2)|3;
		block[13]=(3<<6)|(2<<4)|(3<<2)|2;
		block[14]=(2<<6)|(3<<4)|(2<<2)|3;
		block[15]=(3<<6)|(2<<4)|(3<<2)|2;
	}else
	{
		//Emit RGB Block
#if 0
		mcr=((mcr+4)>>3)<<3;
		mcg=((mcg+2)>>2)<<2;
		mcb=((mcb+4)>>3)<<3;
		ncr=((ncr+4)>>3)<<3;
		ncg=((ncg+2)>>2)<<2;
		ncb=((ncb+4)>>3)<<3;
		mcr=(mcr<0)?0:((mcr>255)?255:mcr);
		mcg=(mcg<0)?0:((mcg>255)?255:mcg);
		mcb=(mcb<0)?0:((mcb>255)?255:mcb);
		ncr=(ncr<0)?0:((ncr>255)?255:ncr);
		ncg=(ncg<0)?0:((ncg>255)?255:ncg);
		ncb=(ncb<0)?0:((ncb>255)?255:ncb);
#endif

#if 1
		mcr=((mcr+3)>>3)<<3;
		mcg=((mcg+1)>>2)<<2;
		mcb=((mcb+3)>>3)<<3;
		ncr=((ncr+5)>>3)<<3;
		ncg=((ncg+3)>>2)<<2;
		ncb=((ncb+5)>>3)<<3;
		mcr=(mcr<0)?0:((mcr>255)?255:mcr);
		mcg=(mcg<0)?0:((mcg>255)?255:mcg);
		mcb=(mcb<0)?0:((mcb>255)?255:mcb);
		ncr=(ncr<0)?0:((ncr>255)?255:ncr);
		ncg=(ncg<0)?0:((ncg>255)?255:ncg);
		ncb=(ncb<0)?0:((ncb>255)?255:ncb);
#endif

//		mcy=(mcr+(2*mcg)+mcb)>>2;
//		ncy=(ncr+(2*ncg)+ncb)>>2;
//		acy=(mcy+ncy)>>1;			//Median Luma

//		l1=32768/(ncy-acy+1);		//Fix-Point Scale (Luma)
//		l1=(32768-8192)/(ncy-acy+1);		//Fix-Point Scale (Luma)
		l1=(32768-6144)/(ncy-acy+1);		//Fix-Point Scale (Luma)
//		l1=(16384)/(ncy-acy+1);		//Fix-Point Scale (Luma)

//		l3a=65536-2048;
//		l3b=65536-2048;
//		l3b=65536;
//		l3a=65536;
//		l3b=65536+2048;
		l3a=65536-1024;
		l3b=65536+1024;

		i=((mcr<<8)&0xF800) | ((mcg<<3)&0x07E0) | ((mcb>>3)&0x001F);
		j=((ncr<<8)&0xF800) | ((ncg<<3)&0x07E0) | ((ncb>>3)&0x001F);
		block[ 8]=i;	block[ 9]=i>>8;
		block[10]=j;	block[11]=j>>8;

		p0=idxtab[((pxy[ 0]-acy)*l1+l3a)>>13];
		p1=idxtab[((pxy[ 1]-acy)*l1+l3b)>>13];
		p2=idxtab[((pxy[ 2]-acy)*l1+l3a)>>13];
		p3=idxtab[((pxy[ 3]-acy)*l1+l3b)>>13];
		block[12]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
		p0=idxtab[((pxy[ 4]-acy)*l1+l3b)>>13];
		p1=idxtab[((pxy[ 5]-acy)*l1+l3a)>>13];
		p2=idxtab[((pxy[ 6]-acy)*l1+l3b)>>13];
		p3=idxtab[((pxy[ 7]-acy)*l1+l3a)>>13];
		block[13]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
		p0=idxtab[((pxy[ 8]-acy)*l1+l3a)>>13];
		p1=idxtab[((pxy[ 9]-acy)*l1+l3b)>>13];
		p2=idxtab[((pxy[10]-acy)*l1+l3a)>>13];
		p3=idxtab[((pxy[11]-acy)*l1+l3b)>>13];
		block[14]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
		p0=idxtab[((pxy[12]-acy)*l1+l3b)>>13];
		p1=idxtab[((pxy[13]-acy)*l1+l3a)>>13];
		p2=idxtab[((pxy[14]-acy)*l1+l3b)>>13];
		p3=idxtab[((pxy[15]-acy)*l1+l3a)>>13];
		block[15]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
	}
}
#endif

BGBBTJ_API void BGBBTJ_BCn_EncodeImageDXT1(byte *block,
	byte *rgba, int xs, int ys, int stride)
{
	int xs1, ys1;
	int i, j;
	
	xs1=xs>>2; ys1=ys>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		BGBBTJ_BCn_EncodeBlockDXT1(
			block+(i*xs1+j)*8,
			rgba+(i*4*xs+j*4)*stride,
			stride, xs*stride, -4);
	}
}

BGBBTJ_API void BGBBTJ_BCn_EncodeImageDXT1A(byte *block,
	byte *rgba, int xs, int ys, int stride)
{
	int xs1, ys1;
	int i, j;
	
	xs1=xs>>2; ys1=ys>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		BGBBTJ_BCn_EncodeBlockDXT1(
			block+(i*xs1+j)*8,
			rgba+(i*4*xs+j*4)*stride,
			stride, xs*stride, 3);
	}
}

BGBBTJ_API void BGBBTJ_BCn_EncodeImageDXT1F(byte *block,
	byte *rgba, int xs, int ys, int stride)
{
	int xs1, ys1;
	int i, j;
	
	xs1=xs>>2; ys1=ys>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		BGBBTJ_BCn_EncodeBlockDXT1F(
			block+(i*xs1+j)*8,
			rgba+(i*4*xs+j*4)*stride,
			stride, xs*stride, 3);
	}
}

BGBBTJ_API void BGBBTJ_BCn_EncodeImageDXT5(byte *block,
	byte *rgba, int xs, int ys, int stride)
{
	int xs1, ys1;
	int i, j;
	
	xs1=xs>>2; ys1=ys>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
//		BGBBTJ_BCn_EncodeBlockBC4(
//			block+(i*xs1+j)*16,
//			rgba+(i*4*xs+j*4)*stride+3,
//			stride, xs*stride);
//		BGBBTJ_BCn_EncodeBlockDXT1(
//			block+(i*xs1+j)*16+8,
//			rgba+(i*4*xs+j*4)*stride,
//			stride, xs*stride, 4);

		BGBBTJ_BCn_EncodeBlockDXT5(
			block+(i*xs1+j)*16,
			rgba+(i*4*xs+j*4)*stride,
			stride, xs*stride);
	}
}

BGBBTJ_API void BGBBTJ_BCn_EncodeImageDXT5F(byte *block,
	byte *rgba, int xs, int ys, int stride)
{
	int xs1, ys1;
	int i, j;
	
	xs1=xs>>2; ys1=ys>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		BGBBTJ_BCn_EncodeBlockBC4(
			block+(i*xs1+j)*16,
			rgba+(i*4*xs+j*4)*stride+3,
			stride, xs*stride);
		BGBBTJ_BCn_EncodeBlockDXT1F(
			block+(i*xs1+j)*16+8,
			rgba+(i*4*xs+j*4)*stride,
			stride, xs*stride, 4);
	}
}

BGBBTJ_API void BGBBTJ_BCn_EncodeIdxImageAlphaDXT5(byte *block,
	byte *rgba, int xs, int ys, int stride, int idx)
{
	int xs1, ys1;
	int i, j;
	
	xs1=xs>>2; ys1=ys>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		BGBBTJ_BCn_EncodeBlockBC4(
			block+(i*xs1+j)*16,
			rgba+(i*4*xs+j*4)*stride+idx,
			stride, xs*stride);
//		BGBBTJ_BCn_EncodeBlockDXT1F(
//			block+(i*xs1+j)*16+8,
//			rgba+(i*4*xs+j*4)*stride,
//			stride, xs*stride, 4);
	}
}

BGBBTJ_API void BGBBTJ_BCn_FillImageAlphaDXT5(byte *block,
	int val, int xs, int ys)
{
	int xs1, ys1;
	int i, j;
	
	xs1=xs>>2; ys1=ys>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		BGBBTJ_BCn_FillBlockBC4(
			block+(i*xs1+j)*16, val);
	}
}

BGBBTJ_API int BGBBTJ_BCn_CheckEncodeImageAlpha(
	byte *rgba, int xs, int ys, int stride)
{
	int i, j;
	
	for(i=0; i<ys; i++)
		for(j=0; j<xs; j++)
	{
		if(rgba[(i*xs+j)*stride+3]!=255)
			return(1);
	}
	return(0);
}

BGBBTJ_API void BGBBTJ_BCn_EncodeImageDXTn(byte *block,
	byte *rgba, int xs, int ys, int stride, int fmt)
{
	switch(fmt)
	{
	case BGBBTJ_JPG_BC1:
		BGBBTJ_BCn_EncodeImageDXT1(block, rgba, xs, ys, stride);
//		BGBBTJ_BCn_EncodeImageDXT1F(block, rgba, xs, ys, stride);
		break;
	case BGBBTJ_JPG_BC3:
		BGBBTJ_BCn_EncodeImageDXT5(block, rgba, xs, ys, stride);
		break;
	case BGBBTJ_JPG_BC1F:
		BGBBTJ_BCn_EncodeImageDXT1F(block, rgba, xs, ys, stride);
		break;
	case BGBBTJ_JPG_BC3F:
		BGBBTJ_BCn_EncodeImageDXT5F(block, rgba, xs, ys, stride);
		break;
	case BGBBTJ_JPG_BC1A:
		BGBBTJ_BCn_EncodeImageDXT1A(block, rgba, xs, ys, stride);
//		BGBBTJ_BCn_EncodeImageDXT1F(block, rgba, xs, ys, stride);
		break;
	case BGBBTJ_JPG_BC3_UVAY:
		BGBBTJ_BCn_EncodeImageDXT5_UVAY(block, rgba, xs, ys, stride);
		break;
	}
}

BGBBTJ_API void BGBBTJ_BCn_EncodeImageAutoDXTn(byte *block,
	byte *rgba, int xs, int ys, int stride, int *rfmt)
{
	if(stride==3)
	{
		BGBBTJ_BCn_EncodeImageDXT1(block, rgba, xs, ys, stride);
//		BGBBTJ_BCn_EncodeImageDXT1F(block, rgba, xs, ys, stride);
		*rfmt=BGBBTJ_JPG_BC1;
		return;
	}

	if(!BGBBTJ_BCn_CheckEncodeImageAlpha(rgba, xs, ys, stride))
	{
#if 0
		BGBBTJ_BCn_EncodeImageDXT5_UVAY(block, rgba, xs, ys, stride);
		*rfmt=BGBBTJ_JPG_BC3_UVAY;
		return;
#endif

#if 0
		BGBBTJ_BCn_EncodeImageDXT5(block, rgba, xs, ys, stride);
		*rfmt=BGBBTJ_JPG_BC3;
		return;
#endif

		BGBBTJ_BCn_EncodeImageDXT1(block, rgba, xs, ys, stride);
//		BGBBTJ_BCn_EncodeImageDXT1F(block, rgba, xs, ys, stride);
		*rfmt=BGBBTJ_JPG_BC1;
		return;
	}

#if 0
	BGBBTJ_BCn_EncodeImageDXT5_UVAY(block, rgba, xs, ys, stride);
	*rfmt=BGBBTJ_JPG_BC3_UVAY;
	return;
#endif

	BGBBTJ_BCn_EncodeImageDXT5(block, rgba, xs, ys, stride);
	*rfmt=BGBBTJ_JPG_BC3;

//	BGBBTJ_BCn_EncodeImageDXT5_UVAY(block, rgba, xs, ys, stride);
//	*rfmt=BGBBTJ_JPG_BC3_UVAY;
}

BGBBTJ_API void BGBBTJ_BCn_DecodeImageDXTn(byte *block,
	byte *rgba, int xs, int ys, int stride, int fmt)
{
	switch(fmt)
	{
	case BGBBTJ_JPG_BC1:
		BGBBTJ_BCn_DecodeImageDXT1(block, rgba, xs, ys, stride);
//		BGBBTJ_BCn_EncodeImageDXT1F(block, rgba, xs, ys, stride);
		break;
	case BGBBTJ_JPG_BC3:
		BGBBTJ_BCn_DecodeImageDXT5(block, rgba, xs, ys, stride);
		break;
	case BGBBTJ_JPG_BC1F:
		BGBBTJ_BCn_DecodeImageDXT1(block, rgba, xs, ys, stride);
		break;
	case BGBBTJ_JPG_BC3F:
		BGBBTJ_BCn_DecodeImageDXT5(block, rgba, xs, ys, stride);
		break;
	case BGBBTJ_JPG_BC3_UVAY:
		BGBBTJ_BCn_DecodeImageDXT5_UVAY(block, rgba, xs, ys, stride);
		break;
	}
}


void BGBBTJ_BCn_EncodeBlockDXT1F_YUV420(byte *block,
	short *pxy, short *pxu, short *pxv)
{
	static const char idxtab[16]=
		{ 0,0,0,0, 0,0,2,2, 3,3,1,1, 1,1,1,1 };

	int p0, p1, p2, p3;
	int mcy, ncy, acy;
	int acr, acg, acb;
	int cy, cu, cv;
	int cra, cga, cba;
	int crb, cgb, cbb;
	int u1, v1, l0, l1, l2;
	int i, j, k;

	cu=(pxu[0]+pxu[1]+pxu[8]+pxu[9])>>2;
	cv=(pxv[0]+pxv[1]+pxv[8]+pxv[9])>>2;

#if 0
	mcy=255; ncy=0;
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		k=pxy[i*8+j];
		if(k<mcy)mcy=k;
		if(k>ncy)ncy=k;
	}
#endif

#if 1
	mcy=pxy[0];	ncy=mcy;
	i=pxy[ 1];	j=(i-mcy)>>24; k=(ncy-i)>>24;
	mcy=(j&i)|((~j)&mcy); ncy=(k&i)|((~k)&ncy);
	i=pxy[ 2];	j=(i-mcy)>>24; k=(ncy-i)>>24;
	mcy=(j&i)|((~j)&mcy); ncy=(k&i)|((~k)&ncy);
	i=pxy[ 3];	j=(i-mcy)>>24; k=(ncy-i)>>24;
	mcy=(j&i)|((~j)&mcy); ncy=(k&i)|((~k)&ncy);
	i=pxy[ 8];	j=(i-mcy)>>24; k=(ncy-i)>>24;
	mcy=(j&i)|((~j)&mcy); ncy=(k&i)|((~k)&ncy);
	i=pxy[ 9];	j=(i-mcy)>>24; k=(ncy-i)>>24;
	mcy=(j&i)|((~j)&mcy); ncy=(k&i)|((~k)&ncy);
	i=pxy[10];	j=(i-mcy)>>24; k=(ncy-i)>>24;
	mcy=(j&i)|((~j)&mcy); ncy=(k&i)|((~k)&ncy);
	i=pxy[11];	j=(i-mcy)>>24; k=(ncy-i)>>24;
	mcy=(j&i)|((~j)&mcy); ncy=(k&i)|((~k)&ncy);
	i=pxy[16];	j=(i-mcy)>>24; k=(ncy-i)>>24;
	mcy=(j&i)|((~j)&mcy); ncy=(k&i)|((~k)&ncy);
	i=pxy[17];	j=(i-mcy)>>24; k=(ncy-i)>>24;
	mcy=(j&i)|((~j)&mcy); ncy=(k&i)|((~k)&ncy);
	i=pxy[18];	j=(i-mcy)>>24; k=(ncy-i)>>24;
	mcy=(j&i)|((~j)&mcy); ncy=(k&i)|((~k)&ncy);
	i=pxy[19];	j=(i-mcy)>>24; k=(ncy-i)>>24;
	mcy=(j&i)|((~j)&mcy); ncy=(k&i)|((~k)&ncy);
	i=pxy[24];	j=(i-mcy)>>24; k=(ncy-i)>>24;
	mcy=(j&i)|((~j)&mcy); ncy=(k&i)|((~k)&ncy);
	i=pxy[25];	j=(i-mcy)>>24; k=(ncy-i)>>24;
	mcy=(j&i)|((~j)&mcy); ncy=(k&i)|((~k)&ncy);
	i=pxy[26];	j=(i-mcy)>>24; k=(ncy-i)>>24;
	mcy=(j&i)|((~j)&mcy); ncy=(k&i)|((~k)&ncy);
	i=pxy[27];	j=(i-mcy)>>24; k=(ncy-i)>>24;
	mcy=(j&i)|((~j)&mcy); ncy=(k&i)|((~k)&ncy);
#endif

	acy=(mcy+ncy)>>1;

	u1=cu-128; v1=cv-128;
//	l0=91881*v1;
//	l1=-22554*u1-46802*v1;
//	l2=116130*u1;

	l0=359*v1;
	l1=-88*u1-183*v1;
	l2=454*u1;

	cra=(256*mcy+l0)>>8;	cga=(256*mcy+l1)>>8;	cba=(256*mcy+l2)>>8;
	crb=(256*ncy+l0)>>8;	cgb=(256*ncy+l1)>>8;	cbb=(256*ncy+l2)>>8;
	if((cra|cga|cba|crb|cgb|cbb)&(~255))
	{
		cra=(cra<0)?0:((cra>255)?255:cra);
		cga=(cga<0)?0:((cga>255)?255:cga);
		cba=(cba<0)?0:((cba>255)?255:cba);
		crb=(crb<0)?0:((crb>255)?255:crb);
		cgb=(cgb<0)?0:((cgb>255)?255:cgb);
		cbb=(cbb<0)?0:((cbb>255)?255:cbb);
	}

	i=((cra<<8)&0xF800) | ((cga<<3)&0x07E0) | (cba>>3);
	j=((crb<<8)&0xF800) | ((cgb<<3)&0x07E0) | (cbb>>3);
	block[0]=i;		block[1]=i>>8;
	block[2]=j;		block[3]=j>>8;

	if(i==j)
	{
		block[4]=0;	block[5]=0;
		block[6]=0;	block[7]=0;
		return;
	}

#if 1
//	l0=(256+64)/(ncy-mcy+1);
//	l0=(256+64)/(ncy-acy+1);
//	l0=65536/(ncy-acy+1);
	l0=32768/(ncy-acy+1);

	p0=idxtab[(((pxy[24]-acy)*l0)>>13)+8];
	p1=idxtab[(((pxy[25]-acy)*l0)>>13)+8];
	p2=idxtab[(((pxy[26]-acy)*l0)>>13)+8];
	p3=idxtab[(((pxy[27]-acy)*l0)>>13)+8];
	block[4]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
	p0=idxtab[(((pxy[16]-acy)*l0)>>13)+8];
	p1=idxtab[(((pxy[17]-acy)*l0)>>13)+8];
	p2=idxtab[(((pxy[18]-acy)*l0)>>13)+8];
	p3=idxtab[(((pxy[19]-acy)*l0)>>13)+8];
	block[5]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
	p0=idxtab[(((pxy[ 8]-acy)*l0)>>13)+8];
	p1=idxtab[(((pxy[ 9]-acy)*l0)>>13)+8];
	p2=idxtab[(((pxy[10]-acy)*l0)>>13)+8];
	p3=idxtab[(((pxy[11]-acy)*l0)>>13)+8];
	block[6]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
	p0=idxtab[(((pxy[ 0]-acy)*l0)>>13)+8];
	p1=idxtab[(((pxy[ 1]-acy)*l0)>>13)+8];
	p2=idxtab[(((pxy[ 2]-acy)*l0)>>13)+8];
	p3=idxtab[(((pxy[ 3]-acy)*l0)>>13)+8];
	block[7]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
#endif

#if 0
	l0=256/(ncy-mcy+1);
	for(i=0; i<4; i++)
	{
		p0=idxtab[(((pxy[(3-i)*8+0]-acy)*l0)>>13)+8];
		p1=idxtab[(((pxy[(3-i)*8+1]-acy)*l0)>>13)+8];
		p2=idxtab[(((pxy[(3-i)*8+2]-acy)*l0)>>13)+8];
		p3=idxtab[(((pxy[(3-i)*8+3]-acy)*l0)>>13)+8];
		block[4+i]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
	}
#endif

#if 0
	p0=-((acy-pxy[ 0])>>16);
	p1=-((acy-pxy[ 1])>>16);
	p2=-((acy-pxy[ 2])>>16);
	p3=-((acy-pxy[ 3])>>16);
	block[7]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
	p0=-((acy-pxy[ 8])>>16);
	p1=-((acy-pxy[ 9])>>16);
	p2=-((acy-pxy[10])>>16);
	p3=-((acy-pxy[11])>>16);
	block[6]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
	p0=-((acy-pxy[16])>>16);
	p1=-((acy-pxy[17])>>16);
	p2=-((acy-pxy[18])>>16);
	p3=-((acy-pxy[19])>>16);
	block[5]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
	p0=-((acy-pxy[24])>>16);
	p1=-((acy-pxy[25])>>16);
	p2=-((acy-pxy[26])>>16);
	p3=-((acy-pxy[27])>>16);
	block[4]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
#endif
}

BGBBTJ_API void BGBBTJ_BCn_EncodeImageDXT1F_YUV(byte *block,
	byte *rgba, int xs, int ys, int stride)
{
	int xs1, ys1;
	int i, j;
	
	xs1=xs>>2; ys1=ys>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		BGBBTJ_BCn_EncodeBlockDXT1F_YUV420(
			block+(i*xs1+j)*8,
			(short *)(rgba+(i*xs1+j)*16),
			(short *)(rgba+(i*xs1+j)*16),
			(short *)(rgba+(i*xs1+j)*16));
	}
}

#if 0
void BGBBTJ_BCn_EncodeBlockDXT5_UVAY(byte *block,
	byte *rgba, int xstride, int ystride, int nclr)
{
	static const char idxtab[16]=
		{ 0,0,0,0, 0,0,2,2, 3,3,1,1, 1,1,1,1 };
	static const char idxtab2[16]=
		{ 0,0,0,0, 2,2,3,3, 4,4,5,5, 1,1,1,1 };
	short pxy[16], pxu[16], pxv[16], pxa[16], pxz[16];
//	int peuva[3], pxuva[3], pwuva[3];
	int p0, p1, p2, p3, p4, p5, p6, p7;
	int l0, l1;
	int mcy, mcu, mcv, mca, mcz;
	int ncy, ncu, ncv, nca, ncz;
	int acy, acu, acv, aca, acz;
	int cr, cg, cb, ca;
	int cy, cu, cv, cz, czb;
	int i, j, k, l;
	
	mcy=255; mcu=255; mcv=255; mca=255; mcz=255;
	ncy=  0; ncu=  0; ncv=  0; nca=  0; ncz=  0;
	acy=  0; acu=  0; acv=  0; aca=  0; acz=  0;
	czb=  0;
	
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		k=(i*ystride)+(j*xstride);
		cr=rgba[k+0];	cg=rgba[k+1];
		cb=rgba[k+2];	ca=rgba[k+3];
//		cy=(cr+(2*cg)+cb)>>2;		//Y (Luma)
		cy=cg+(cb+cr-2*cg)>>2;		//Y (Luma)
		cu=((cb-cg)>>1)+128;
		cv=((cr-cg)>>1)+128;
//		ca=(ca>>1)+128;
		ca=(ca>>1);

#if 0
//		if(ca>=252)
		if(ca>=120)
		{
			l0=(cu>=128)?(cu-128):(128-cu);
			l1=(cv>=128)?(cv-128):(128-cv);
			l=(l0>=l1)?l0:l1;
//			l=128/(l+1);
//			l=(l>>2)<<2;
			l=64/(l+1);
			if(!l)l=1;
			
			cu=(cu-128)*l+128;
			cv=(cv-128)*l+128;
//			ca=128+(128/l);
//			ca=128-(128/l);
//			ca=(128/l);
			ca=256-(128/l);
		}
#endif

//		ca=255-ca;

//		cz=(cu+(2*cv)+ca)>>2;		//Z (Chroma+Alpha)
//		cz=((3*cu)+(4*cv)+ca)>>3;	//Z (Chroma+Alpha)
		cz=((256-cu)+(2*cv)+ca)>>2;		//Z (Chroma+Alpha)

		czb+=(cu-128)*(cv-128);

//		cy=(cy<0)?0:((cy>255)?255:cy);
//		cu=(cu<0)?0:((cu>255)?255:cu);
//		cv=(cv<0)?0:((cv>255)?255:cv);
//		ca=(ca<0)?0:((ca>255)?255:ca);
//		cz=(cz<0)?0:((cz>255)?255:cz);

#if 0
		if((cy+32)>=ca)
			{ cy=(224*ca)>>8; }
#endif

		pxy[i*4+j]=cy;
		pxz[i*4+j]=cz;

		pxu[i*4+j]=cu;
		pxv[i*4+j]=cv;
		pxa[i*4+j]=ca;

#if 0
		//Energy Balance (UVA)
		peuva[0]+=(cu-128)*(cu-128);
		peuva[1]+=(cv-128)*(cv-128);
		peuva[2]+=(ca-128)*(ca-128);
		//Correlation (UVA)
		pxuva[0]+=(cu-128)*(cv-128);
		pxuva[1]+=(cv-128)*(ca-128);
		pxuva[2]+=(cu-128)*(ca-128);
#endif

		//Find Mins/Maxs YUVA
		if(cz<mcz) { mcu=cu; mcv=cv; mca=ca; mcz=cz; }
		if(cz>ncz) { ncu=cu; ncv=cv; nca=ca; ncz=cz; }
		if(cy<mcy) { mcy=cy; }
		if(cy>ncy) { ncy=cy; }

#if 0
		if(cu<mcu) { mcu=cu; }
		if(cu>ncu) { ncu=cu; }
		if(cv<mcv) { mcv=cv; }
		if(cv>ncv) { ncv=cv; }
		if(ca<mca) { mca=ca; }
		if(ca>nca) { nca=ca; }
		if(cz<mcz) { mcz=cz; }
		if(cz>ncz) { ncz=cz; }
#endif

		//For Average YUVA
		acy+=cy;	acu+=cu;
		acv+=cv;	aca+=ca;
	}
	
	//Calculate Average YUVA
	acy=acy>>4;	acu=acu>>4;
	acv=acv>>4;	aca=aca>>4;

//	if(pxuva[0]<0) { k=mcu; mcu=ncu; ncu=k; }
//	if(pxuva[1]<0) { k=mca; mca=nca; nca=k; }

#if 0
	k=peuva[0]+peuva[1]+peuva[2];
	l=(256*peuva[0])/k;
	pwuva[0]=(pxuva[0]>>28)*l;
	l=(256*peuva[1])/k;
	pwuva[1]=l;
	l=(256*peuva[2])/k;
	pwuva[2]=(pxuva[2]>>28)*l;

	if(pxuva[0]<0) { k=mcu; mcu=ncu; ncu=k; }
	if(pxuva[1]<0) { k=mca; mca=nca; nca=k; }

	mcz=255; ncz=0;
	for(i=0; i<16; i++)
	{
		cu=pxu[i]; cv=pxv[i]; ca=pxv[i];
		cz=(cu*pwuva[0]+cv*pwuva[1]+cv*pwuva[2])>>8;
		pxz[i]=cz;
		if(cz<mcz) { mcz=cz; }
		if(cz>ncz) { ncz=cz; }
	}

#endif

#if 0
	if(czb<0)
//	if(czb>0)
	{
		k=mcu; mcu=ncu; ncu=k;
		mcz=255; ncz=0;
		for(i=0; i<16; i++)
		{
			cu=256-pxu[i];
			cv=pxv[i];
			ca=pxv[i];
			cz=(l+(2*cv)+ca)>>2;	//Z (Chroma+Alpha)
//			cz=((4*l)+(3*cv)+ca)>>3;	//Z (Chroma+Alpha)
			pxz[i]=cz;
			if(cz<mcz) { mcz=cz; }
			if(cz>ncz) { ncz=cz; }
		}
	}
#endif

	mcu=((mcu+4)>>3)<<3;
	mcv=((mcv+2)>>2)<<2;
	ncu=((ncu+4)>>3)<<3;
	ncv=((ncv+2)>>2)<<2;

//	mcu=(((mcu-128)>>3)<<3)+128;
//	mcv=(((mcv-128)>>2)<<2)+128;
//	ncu=(((ncu-128)>>3)<<3)+128;
//	ncv=(((ncv-128)>>2)<<2)+128;

//	mcu=(11*mcu+5*acu)>>4;
//	mcv=(11*mcv+5*acv)>>4;
//	mca=(11*mca+5*aca)>>4;
//	ncu=(11*ncu+5*acu)>>4;
//	ncv=(11*ncv+5*acv)>>4;
//	nca=(11*nca+5*aca)>>4;

//	mcu=(mcu<128)?(((mcu+7)>>3)<<3):mcu;
//	mcv=(mcv<128)?(((mcv+3)>>2)<<2):mcv;

	mcy=(mcy<0)?0:((mcy>255)?255:mcy);
	mcu=(mcu<0)?0:((mcu>255)?255:mcu);
	mcv=(mcv<0)?0:((mcv>255)?255:mcv);
	mca=(mca<0)?0:((mca>255)?255:mca);
	mcz=(mcz<0)?0:((mcz>255)?255:mcz);
	ncy=(ncy<0)?0:((ncy>255)?255:ncy);
	ncu=(ncu<0)?0:((ncu>255)?255:ncu);
	ncv=(ncv<0)?0:((ncv>255)?255:ncv);
	nca=(nca<0)?0:((nca>255)?255:nca);
	ncz=(ncz<0)?0:((ncz>255)?255:ncz);

//	if(mcy<0)mcy=0;
//	if(mcu<0)mcu=0;
//	if(mcv<0)mcv=0;
//	if(mca<0)mca=0;
//	if(mcz<0)mcz=0;
//	if(ncy>255)ncy=255;
//	if(ncu>255)ncu=255;
//	if(ncv>255)ncv=255;
//	if(nca>255)nca=255;
//	if(ncz>255)ncz=255;

	acy=(mcy+ncy)>>1;			//Median Y
	acz=(mcz+ncz)>>1;			//Median Z
//	l0=(256+128)/(ncy-acy+1);	//Fix-Point Scale (Y)
//	l1=(256+64)/(ncz-acz+1);	//Fix-Point Scale (Z)
//	l0=(ncy>acy)?(32768/(ncy-acy)):0;		//Fix-Point Scale (Y)
//	l1=(ncz>acz)?(32768/(ncz-acz)):0;		//Fix-Point Scale (Z)
//	l0=32768/(ncy-acy+1);		//Fix-Point Scale (Y)
	l0=49152/(ncy-acy+1);		//Fix-Point Scale (Y)
	l1=32768/(ncz-acz+1);		//Fix-Point Scale (Z)

	//Emit Y Block
	block[0]=mcy;
	block[1]=ncy;
	
	p0=idxtab2[(((pxy[ 0]-acy)*l0)>>13)+8];
	p1=idxtab2[(((pxy[ 1]-acy)*l0)>>13)+8];
	p2=idxtab2[(((pxy[ 2]-acy)*l0)>>13)+8];
	p3=idxtab2[(((pxy[ 3]-acy)*l0)>>13)+8];
	p4=idxtab2[(((pxy[ 4]-acy)*l0)>>13)+8];
	p5=idxtab2[(((pxy[ 5]-acy)*l0)>>13)+8];
	p6=idxtab2[(((pxy[ 6]-acy)*l0)>>13)+8];
	p7=idxtab2[(((pxy[ 7]-acy)*l0)>>13)+8];
	block[2]=p0|(p1<<3)|(p2<<6);
	block[3]=(p2>>2)|(p3<<1)|(p4<<4)|(p5<<7);
	block[4]=(p5>>1)|(p6<<2)|(p7<<5);
	p0=idxtab2[(((pxy[ 8]-acy)*l0)>>13)+8];
	p1=idxtab2[(((pxy[ 9]-acy)*l0)>>13)+8];
	p2=idxtab2[(((pxy[10]-acy)*l0)>>13)+8];
	p3=idxtab2[(((pxy[11]-acy)*l0)>>13)+8];
	p4=idxtab2[(((pxy[12]-acy)*l0)>>13)+8];
	p5=idxtab2[(((pxy[13]-acy)*l0)>>13)+8];
	p6=idxtab2[(((pxy[14]-acy)*l0)>>13)+8];
	p7=idxtab2[(((pxy[15]-acy)*l0)>>13)+8];
	block[5]=p0|(p1<<3)|(p2<<6);
	block[6]=(p2>>2)|(p3<<1)|(p4<<4)|(p5<<7);
	block[7]=(p5>>1)|(p6<<2)|(p7<<5);

//	if(czb<0) { k=mcu; mcu=ncu; ncu=k; }

	//Emit UVA Block
	i=((mcu<<8)&0xF800) | ((mcv<<3)&0x07E0) | ((mca>>3)&0x001F);
	j=((ncu<<8)&0xF800) | ((ncv<<3)&0x07E0) | ((nca>>3)&0x001F);
	block[ 8]=i;	block[ 9]=i>>8;
	block[10]=j;	block[11]=j>>8;

	p0=idxtab[(((pxz[ 0]-acz)*l1)>>13)+8];
	p1=idxtab[(((pxz[ 1]-acz)*l1)>>13)+8];
	p2=idxtab[(((pxz[ 2]-acz)*l1)>>13)+8];
	p3=idxtab[(((pxz[ 3]-acz)*l1)>>13)+8];
	block[12]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
	p0=idxtab[(((pxz[ 4]-acz)*l1)>>13)+8];
	p1=idxtab[(((pxz[ 5]-acz)*l1)>>13)+8];
	p2=idxtab[(((pxz[ 6]-acz)*l1)>>13)+8];
	p3=idxtab[(((pxz[ 7]-acz)*l1)>>13)+8];
	block[13]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
	p0=idxtab[(((pxz[ 8]-acz)*l1)>>13)+8];
	p1=idxtab[(((pxz[ 9]-acz)*l1)>>13)+8];
	p2=idxtab[(((pxz[10]-acz)*l1)>>13)+8];
	p3=idxtab[(((pxz[11]-acz)*l1)>>13)+8];
	block[14]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
	p0=idxtab[(((pxz[12]-acz)*l1)>>13)+8];
	p1=idxtab[(((pxz[13]-acz)*l1)>>13)+8];
	p2=idxtab[(((pxz[14]-acz)*l1)>>13)+8];
	p3=idxtab[(((pxz[15]-acz)*l1)>>13)+8];
	block[15]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
}
#endif

#if 1
void BGBBTJ_BCn_EncodeBlockDXT5_UVAY(byte *block,
	byte *rgba, int xstride, int ystride, int nclr)
{
	static const char idxtab[16]=
		{ 0,0,0,0, 0,0,2,2, 3,3,1,1, 1,1,1,1 };
//	static const char idxtab2[16]=
//		{ 0,0,0,0, 2,2,3,3, 4,4,5,5, 1,1,1,1 };
	static const char idxtab2[16]=
		{ 1,1,7,7, 6,6,5,5, 4,4,3,3, 2,2,0,0 };
	short pxy[16], pxu[16], pxv[16], pxa[16], pxz[16];
//	int peuva[3], pxuva[3], pwuva[3];
	int p0, p1, p2, p3, p4, p5, p6, p7;
	int l0, l1, l2a, l2b, l3a, l3b;
	int mcy, mcu, mcv, mca, mcz;
	int ncy, ncu, ncv, nca, ncz;
	int acy, acu, acv, aca, acz;
	int cr, cg, cb, ca;
	int cy, cu, cv, cz, czb;
	int i, j, k, l;
	
	mcy=255; mcu=255; mcv=255; mca=255; mcz=255;
	ncy=  0; ncu=  0; ncv=  0; nca=  0; ncz=  0;
	acy=  0; acu=  0; acv=  0; aca=  0; acz=  0;
	czb=  0;
	
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		k=(i*ystride)+(j*xstride);
		cr=rgba[k+0];	cg=rgba[k+1];
		cb=rgba[k+2];	ca=rgba[k+3];

#if 0
//		cy=(cr+(2*cg)+cb)>>2;		//Y (Luma)
		cy=cg+((cb+cr-2*cg)>>2);	//Y (Luma)
		cu=((cb-cg)>>1)+128;
		cv=((cr-cg)>>1)+128;
//		ca=(ca>>1)+128;
		ca=(ca>>1);
#endif

#if 1
		cy=cg-((2*cg-cb-cr)>>2);
//		cy=cg+((cb+cr-2*cg)>>2);	//Y (Luma)
//		cu=((cb-cr)>>1)+128;
//		cv=((2*cg-cb-cr)>>2)+128;

		cu=(cb-cr)+128;
		cv=((2*cg-cb-cr)>>1)+128;
		ca=(ca>>1);
#endif

#if 1
//		if(ca>=252)
		if(ca>=120)
		{
			l0=(cu>=128)?(cu-128):(128-cu);
			l1=(cv>=128)?(cv-128):(128-cv);
			l=(l0>=l1)?l0:l1;
			
			if(l<64)
			{
//				l=128/(l+1);
//				l=(l>>2)<<2;
				l=64/(l+1);
				if(!l)l=1;

//				k=((128/l+2)&0xFC);
				k=((128/l+4)&0xF8);
//				k=(128/l);
				if(!k)k=1;
				l=128/k;
				if(!l)l=1;
			
				cu=(cu-128)*l+128;
				cv=(cv-128)*l+128;
//				ca=128+(128/l);
//				ca=128-(128/l);
//				ca=(128/l);
//				ca=256-(128/l);
				ca=256-(64/l);
			}else if(l<128)
			{
				ca=192;
			}else
			{
				cu=((cu-128)>>1)+128;
				cv=((cv-128)>>1)+128;
				ca=128;
			}
		}
#endif

		cz=(cu+(2*cv)+ca)>>2;		//Z (Chroma+Alpha)
//		cz=((3*cu)+(4*cv)+ca)>>3;	//Z (Chroma+Alpha)
//		cz=((256-cu)+(2*cv)+ca)>>2;		//Z (Chroma+Alpha)

		czb+=(cu-128)*(cv-128);

		pxy[i*4+j]=cy;
		pxz[i*4+j]=cz;

		pxu[i*4+j]=cu;
		pxv[i*4+j]=cv;
		pxa[i*4+j]=ca;

		//Find Mins/Maxs YUVA
		if(cz<mcz) { mcu=cu; mcv=cv; mca=ca; mcz=cz; }
		if(cz>ncz) { ncu=cu; ncv=cv; nca=ca; ncz=cz; }
		if(cy<mcy) { mcy=cy; }
		if(cy>ncy) { ncy=cy; }

		//For Average YUVA
		acy+=cy;	acu+=cu;
		acv+=cv;	aca+=ca;
	}
	
	//Calculate Average YUVA
//	acy=acy>>4;	acu=acu>>4;
//	acv=acv>>4;	aca=aca>>4;

	acy=(acy+8)>>4;	acu=(acu+8)>>4;
	acv=(acv+8)>>4;	aca=(aca+8)>>4;

#if 1
	mcu=(mcu*13+acu*3)>>4;
	mcv=(mcv*13+acv*3)>>4;
	mca=(mca*13+aca*3)>>4;

	ncu=(ncu*13+acu*3)>>4;
	ncv=(ncv*13+acv*3)>>4;
	nca=(nca*13+aca*3)>>4;
#endif

#if 0
	acz=(acu+(2*acv)+aca)>>2;
	
	l=(256*mcz)/(acz+1);
	cu=(acu*l+128)>>8;
	cv=(acv*l+128)>>8;
	ca=(aca*l+128)>>8;
	
	mcu=(mcu*3+cu)>>2;
	mcv=(mcv*3+cv)>>2;
	mca=(mca*3+ca)>>2;

	l=(256*ncz)/(acz+1);
	cu=(acu*l+128)>>8;
	cv=(acv*l+128)>>8;
	ca=(aca*l+128)>>8;
	
	ncu=(ncu*3+cu)>>2;
	ncv=(ncv*3+cv)>>2;
	nca=(nca*3+ca)>>2;
#endif

	mcu=((mcu+4)>>3)<<3;
	mcv=((mcv+2)>>2)<<2;
	mca=((mca+4)>>3)<<3;
	ncu=((ncu+4)>>3)<<3;
	ncv=((ncv+2)>>2)<<2;
	nca=((nca+4)>>3)<<3;

//	mcu=((mcu+3)>>3)<<3;
//	mcv=((mcv+1)>>2)<<2;
//	ncu=((ncu+5)>>3)<<3;
//	ncv=((ncv+3)>>2)<<2;

	mcy=(mcy<0)?0:((mcy>255)?255:mcy);
	mcu=(mcu<0)?0:((mcu>255)?255:mcu);
	mcv=(mcv<0)?0:((mcv>255)?255:mcv);
	mca=(mca<0)?0:((mca>255)?255:mca);
	mcz=(mcz<0)?0:((mcz>255)?255:mcz);
	ncy=(ncy<0)?0:((ncy>255)?255:ncy);
	ncu=(ncu<0)?0:((ncu>255)?255:ncu);
	ncv=(ncv<0)?0:((ncv>255)?255:ncv);
	nca=(nca<0)?0:((nca>255)?255:nca);
	ncz=(ncz<0)?0:((ncz>255)?255:ncz);

	acy=(mcy+ncy)>>1;			//Median Y
	acz=(mcz+ncz)>>1;			//Median Z
//	l0=49152/(ncy-acy+1);		//Fix-Point Scale (Y)
//	l1=32768/(ncz-acz+1);		//Fix-Point Scale (Z)

//	l0=(49152-8192)/(ncy-acy+1);		//Fix-Point Scale (Y)
//	l0=(49152-1024)/(ncy-acy+1);		//Fix-Point Scale (Y)
//	l0=(49152-12288)/(ncy-acy+1);		//Fix-Point Scale (Y)
//	l0=(49152-6144)/(ncy-acy+1);		//Fix-Point Scale (Y)
//	l0=(49152-6144)/(ncy-acy+1);		//Fix-Point Scale (Y)

	l0=65536/(ncy-acy+1);				//Fix-Point Scale (Y)

//	l1=(32768-6144)/(ncz-acz+1);		//Fix-Point Scale (Z)
//	l1=(32768-4096)/(ncz-acz+1);		//Fix-Point Scale (Z)
	l1=(32768-8192)/(ncz-acz+1);		//Fix-Point Scale (Z)

	l2a=65536-1024;
	l2b=65536+1024;

	l3a=65536-1024;
	l3b=65536+1024;

	//Emit Y Block
//	block[0]=mcy;
//	block[1]=ncy;
	block[0]=ncy;
	block[1]=mcy;
	
	p0=idxtab2[(((pxy[ 0]-acy)*l0+l2a)>>13)];
	p1=idxtab2[(((pxy[ 1]-acy)*l0+l2b)>>13)];
	p2=idxtab2[(((pxy[ 2]-acy)*l0+l2a)>>13)];
	p3=idxtab2[(((pxy[ 3]-acy)*l0+l2b)>>13)];
	p4=idxtab2[(((pxy[ 4]-acy)*l0+l2b)>>13)];
	p5=idxtab2[(((pxy[ 5]-acy)*l0+l2a)>>13)];
	p6=idxtab2[(((pxy[ 6]-acy)*l0+l2b)>>13)];
	p7=idxtab2[(((pxy[ 7]-acy)*l0+l2a)>>13)];
	block[2]=p0|(p1<<3)|(p2<<6);
	block[3]=(p2>>2)|(p3<<1)|(p4<<4)|(p5<<7);
	block[4]=(p5>>1)|(p6<<2)|(p7<<5);
	p0=idxtab2[(((pxy[ 8]-acy)*l0+l2a)>>13)];
	p1=idxtab2[(((pxy[ 9]-acy)*l0+l2b)>>13)];
	p2=idxtab2[(((pxy[10]-acy)*l0+l2a)>>13)];
	p3=idxtab2[(((pxy[11]-acy)*l0+l2b)>>13)];
	p4=idxtab2[(((pxy[12]-acy)*l0+l2b)>>13)];
	p5=idxtab2[(((pxy[13]-acy)*l0+l2a)>>13)];
	p6=idxtab2[(((pxy[14]-acy)*l0+l2b)>>13)];
	p7=idxtab2[(((pxy[15]-acy)*l0+l2a)>>13)];
	block[5]=p0|(p1<<3)|(p2<<6);
	block[6]=(p2>>2)|(p3<<1)|(p4<<4)|(p5<<7);
	block[7]=(p5>>1)|(p6<<2)|(p7<<5);

	//Emit UVA Block
	i=((mcu<<8)&0xF800) | ((mcv<<3)&0x07E0) | ((mca>>3)&0x001F);
	j=((ncu<<8)&0xF800) | ((ncv<<3)&0x07E0) | ((nca>>3)&0x001F);
	block[ 8]=i;	block[ 9]=i>>8;
	block[10]=j;	block[11]=j>>8;

	p0=idxtab[(((pxz[ 0]-acz)*l1+l3a)>>13)];
	p1=idxtab[(((pxz[ 1]-acz)*l1+l3b)>>13)];
	p2=idxtab[(((pxz[ 2]-acz)*l1+l3a)>>13)];
	p3=idxtab[(((pxz[ 3]-acz)*l1+l3b)>>13)];
	block[12]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
	p0=idxtab[(((pxz[ 4]-acz)*l1+l3b)>>13)];
	p1=idxtab[(((pxz[ 5]-acz)*l1+l3a)>>13)];
	p2=idxtab[(((pxz[ 6]-acz)*l1+l3b)>>13)];
	p3=idxtab[(((pxz[ 7]-acz)*l1+l3a)>>13)];
	block[13]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
	p0=idxtab[(((pxz[ 8]-acz)*l1+l3a)>>13)];
	p1=idxtab[(((pxz[ 9]-acz)*l1+l3b)>>13)];
	p2=idxtab[(((pxz[10]-acz)*l1+l3a)>>13)];
	p3=idxtab[(((pxz[11]-acz)*l1+l3b)>>13)];
	block[14]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
	p0=idxtab[(((pxz[12]-acz)*l1+l3b)>>13)];
	p1=idxtab[(((pxz[13]-acz)*l1+l3a)>>13)];
	p2=idxtab[(((pxz[14]-acz)*l1+l3b)>>13)];
	p3=idxtab[(((pxz[15]-acz)*l1+l3a)>>13)];
	block[15]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
}
#endif

BGBBTJ_API void BGBBTJ_BCn_EncodeImageDXT5_UVAY(byte *block,
	byte *rgba, int xs, int ys, int stride)
{
	int xs1, ys1;
	int i, j;
	
	xs1=xs>>2; ys1=ys>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		BGBBTJ_BCn_EncodeBlockDXT5_UVAY(
			block+(i*xs1+j)*16,
			rgba+(i*4*xs+j*4)*stride,
			stride, xs*stride, 4);
	}
}

void BGBBTJ_BCn_DecodeBlockDXT1(byte *block,
	byte *rgba, int xstride, int ystride, int nclr)
{
	byte clr[4*4];
	int i, j, k, l;
	
	clr[ 0]=block[1]&0xF8;
	clr[ 1]=((block[1]<<5)|(block[0]>>3))&0xFC;
	clr[ 2]=(block[0]<<3)&0xF8;
	clr[ 3]=255;
	clr[ 4]=block[3]&0xF8;
	clr[ 5]=((block[3]<<5)|(block[2]>>3))&0xFC;
	clr[ 6]=(block[2]<<3)&0xF8;	
	clr[ 7]=255;
	clr[ 8]=(clr[0]+clr[4])>>2;
	clr[ 9]=(clr[1]+clr[5])>>2;
	clr[10]=(clr[2]+clr[6])>>2;
	clr[11]=255;
	clr[12]=0;
	clr[13]=0;
	clr[14]=0;
	clr[15]=0;
	
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

void BGBBTJ_BCn_DecodeBlockDXT5(byte *block,
	byte *rgba, int xstride, int ystride, int nclr)
{
	byte clr[4*4];
	int i, j, k, l;
	
	clr[ 0]=block[1]&0xF8;
	clr[ 1]=((block[1]<<5)|(block[0]>>3))&0xFC;
	clr[ 2]=(block[0]<<3)&0xF8;
	clr[ 4]=block[3]&0xF8;
	clr[ 5]=((block[3]<<5)|(block[2]>>3))&0xFC;
	clr[ 6]=(block[2]<<3)&0xF8;	
	clr[ 8]=(clr[0]*11+clr[4]*5)>>4;
	clr[ 9]=(clr[1]*11+clr[5]*5)>>4;
	clr[10]=(clr[2]*11+clr[6]*5)>>4;
	clr[12]=(clr[0]*5+clr[4]*11)>>4;
	clr[13]=(clr[1]*5+clr[5]*11)>>4;
	clr[14]=(clr[2]*5+clr[6]*11)>>4;
	
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		k=i*ystride+j*xstride;
		l=((block[4+i]>>(2*j))&3)*4;
		rgba[k+0]=clr[l+0];
		rgba[k+1]=clr[l+1];
		rgba[k+2]=clr[l+2];
	}
}

void BGBBTJ_BCn_DecodeBlockBC4(byte *block,
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
//		val[2]=(val[0]*213+val[1]*43)>>8;
//		val[3]=(val[0]*171+val[1]*85)>>8;
//		val[4]=(val[0]*85+val[1]*171)>>8;
//		val[5]=(val[0]*43+val[1]*213)>>8;
//		val[6]=0;
//		val[7]=255;
	}else
	{
		val[0]=block[0];
		val[1]=block[1];
		val[2]=(val[0]*205+val[1]* 51)>>8;
		val[3]=(val[0]*154+val[1]*102)>>8;
		val[4]=(val[0]*102+val[1]*154)>>8;
		val[5]=(val[0]* 51+val[1]*205)>>8;
//		val[2]=(val[0]*213+val[1]*43)>>8;
//		val[3]=(val[0]*171+val[1]*85)>>8;
//		val[4]=(val[0]*85+val[1]*171)>>8;
//		val[5]=(val[0]*43+val[1]*213)>>8;
		val[6]=0;
		val[7]=255;
	}

	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		k=(i*4+j)*3;
		l=((block[2+(k>>3)]|(block[3+(k>>3)]<<8))>>(k&7))&7;
		k=i*ystride+j*xstride;
		rgba[k]=val[l];
	}
}

BGBBTJ_API void BGBBTJ_BCn_DecodeImageDXT1(byte *block,
	byte *rgba, int xs, int ys, int stride)
{
	int xs1, ys1;
	int i, j;
	
	xs1=xs>>2; ys1=ys>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		BGBBTJ_BCn_DecodeBlockDXT1(
			block+(i*xs1+j)*8,
			rgba+(i*4*xs+j*4)*stride,
			stride, xs*stride, 4);
	}
}

BGBBTJ_API void BGBBTJ_BCn_DecodeImageDXT5(byte *block,
	byte *rgba, int xs, int ys, int stride)
{
	int xs1, ys1;
	int i, j;
	
	xs1=xs>>2; ys1=ys>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		BGBBTJ_BCn_DecodeBlockBC4(
			block+(i*xs1+j)*16,
			rgba+(i*4*xs+j*4)*stride+3,
			stride, xs*stride);
		BGBBTJ_BCn_DecodeBlockDXT5(
			block+(i*xs1+j)*16+8,
			rgba+(i*4*xs+j*4)*stride,
			stride, xs*stride, 4);
	}
}

BGBBTJ_API void BGBBTJ_BCn_DecodeImageDXT5_UVAY(byte *block,
	byte *rgba, int xs, int ys, int stride)
{
	byte *pix;
	int cy, cu, cv, ca, csc;
	int dr, dg, db, da;
	int i, j, n;

	BGBBTJ_BCn_DecodeImageDXT5(block, rgba, xs, ys, stride);
	
	n=xs*ys;
	for(i=0; i<n; i++)
	{
		pix=rgba+i*stride;

		cy=pix[3];
		cu=pix[0];
		cv=pix[1];
		ca=pix[2];

		if(ca>=128)
		{
			da=255;
//			csc=256-(ca-128)*2;
			csc=256-(ca-192)*4;
		}else
		{
			csc=256;
			da=ca*2;
		}

		cu=(((cu-128)*csc+128)>>8)+128;
		cv=(((cv-128)*csc+128)>>8)+128;

//		cu=128; cv=128;

#if 0
		dg=cy-(cu+cv-256)/2;
//		dg=cy-(cu+cv-256)/2.0;
		db=dg+2*(cu-128);
		dr=dg+2*(cv-128);
#endif

#if 0
		dg=cy+(cv-128);
		dr=dg-(cv-128)*2-(cu-128);
		db=dg-(cv-128)*2+(cu-128);
#endif

#if 1
		dg=cy+((cv-128)>>1);
		dr=dg-(cv-128)-((cu-128)>>1);
		db=dg-(cv-128)+((cu-128)>>1);
#endif

		if(dr<0)dr=0;
		if(dg<0)dg=0;
		if(db<0)db=0;
		if(dr>255)dr=255;
		if(dg>255)dg=255;
		if(db>255)db=255;
		
		pix[0]=dr;		pix[1]=dg;
		pix[2]=db;		pix[3]=da;
	}
}
