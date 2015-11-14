#include <bgbbtj.h>

extern int bgbbtj_bc7_weights2[4];
extern int bgbbtj_bc7_weights3[8];
extern int bgbbtj_bc7_weights4[16];

extern byte bgbbtj_bc7_partition2[64*16];

int BGBBTJ_BC6H_DecodeBlock_ReadMode(BGBBTJ_BitStream *bits)
{
	int i, j;

	i=BGBBTJ_BitsLE_Read2Bits(bits);
	if(i>1)
		{ i|=BGBBTJ_BitsLE_Read3Bits(bits)<<2; }
	return(i);
}

void BGBBTJ_BC6H_EncodeBlock_WriteMode(
	BGBBTJ_BitStream *bits, int mode)
{
	if(mode<2)
	{
		BGBBTJ_BitsLE_Write2Bits(bits, mode);
		return;
	}

	BGBBTJ_BitsLE_Write5Bits(bits, mode);
}

void BGBBTJ_BC6H_EncodeBlock_SplitMinMaxClrY(
	int *pxy,
	int *min_rgba, int *max_rgba, int *avg_rgba,
	int *rmcy, int *rncy, int *racy,
	int *rgba, int xstride, int ystride)
{
	int mcr, mcg, mcb, mca, mcy;
	int ncr, ncg, ncb, nca, ncy;
	int acr, acg, acb, aca, acy;
	int cr, cg, cb, ca, cy;
	int i, j, k, l;

	mca=0; nca=0;
	mcr=( 1<<16); mcg=( 1<<16); mcb=( 1<<16); mcy=( 1<<16);
	ncr=(-1<<16); ncg=(-1<<16); ncb=(-1<<16); ncy=(-1<<16);
	acr=  0; acg=  0; acb=  0; aca=  0; acy=  0;
	
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		k=(i*ystride)+(j*xstride);
		cr=rgba[k+0];	cg=rgba[k+1];
		cb=rgba[k+2];	ca=rgba[k+3];
		cy=(cr+(2*cg)+cb)>>2;		//Luma

		pxy[i*4+j]=cy;
//		pxy[j*4+i]=cy;

		//Find Mins/Maxs
		if(cy<mcy) { mcr=cr; mcg=cg; mcb=cb; mcy=cy; }
		if(cy>ncy) { ncr=cr; ncg=cg; ncb=cb; ncy=cy; }
		acr+=cr; acg+=cg;
		acb+=cb; acy+=cy;
	}
	
	min_rgba[0]=mcr;			min_rgba[1]=mcg;
	min_rgba[2]=mcb;			min_rgba[3]=mca;
	max_rgba[0]=ncr;			max_rgba[1]=ncg;
	max_rgba[2]=ncb;			max_rgba[3]=nca;
	avg_rgba[0]=(mcr+ncr)>>1;	avg_rgba[1]=(mcg+ncg)>>1;
	avg_rgba[2]=(mcb+ncb)>>1;	avg_rgba[3]=(mca+nca)>>1;
	
	*rmcy=mcy; *rncy=ncy; *racy=(mcy+ncy)>>1;
}

void BGBBTJ_BC6H_EncodeBlock_VecInvertPixels(
	int *pxv, int n, int min, int max)
{
	int i, j, k;
	
	for(i=0; i<n; i++)
		{ pxv[i]=max-(pxv[i]-min); }
}


void BGBBTJ_BC6H_EncodeBlock_Mode3(byte *block,
	int *rgba, int xstride, int ystride)
{
	static const char idxtab[32]=
		{  0,  0,  0,  0,  0,  0,  0,  0,
		   0,  1,  2,  3,  4,  5,  6,  7,
		   8,  9, 10, 11, 12, 13, 14, 15,
		  15, 15, 15, 15, 15, 15, 15, 15};
	
	BGBBTJ_BitStream bits;

	int pxy[16];
	int p0, p1, p2, p3, p4, p5, p6, p7;
	int l0, l1, l2, l3a, l3b;
	int min[4], mcy, mca;
	int max[4], ncy, nca;
	int avg[4], acy, aca;
	int acr, acg, acb;
	int cr, cg, cb, ca, cy;
	int i, j, k, l;

	BGBBTJ_BC6H_EncodeBlock_SplitMinMaxClrY(
		pxy, min, max, avg,
		&mcy, &ncy, &acy,
		rgba, xstride, ystride);

	BGBBTJ_BitsLE_ClearSetupWrite(&bits, block, 16);
	BGBBTJ_BitsLE_WriteBits(&bits, 3, 5);

	if(pxy[0]>=acy)
	{
		BGBBTJ_BC7_EncodeBlock_VecSwapRGBA(min, max);
		BGBBTJ_BC6H_EncodeBlock_VecInvertPixels(pxy, 16, mcy, ncy);
	}

	BGBBTJ_BitsLE_WriteBits(&bits, (min[0]+32)>>6, 10);
	BGBBTJ_BitsLE_WriteBits(&bits, (min[1]+32)>>6, 10);
	BGBBTJ_BitsLE_WriteBits(&bits, (min[2]+32)>>6, 10);
	BGBBTJ_BitsLE_WriteBits(&bits, (max[0]+32)>>6, 10);
	BGBBTJ_BitsLE_WriteBits(&bits, (max[1]+32)>>6, 10);
	BGBBTJ_BitsLE_WriteBits(&bits, (max[2]+32)>>6, 10);

//	l0=49152/(nca-aca+1);		//Fix-Point Scale (Alpha)
	l1=32768/(ncy-acy+1);		//Fix-Point Scale (Luma)
	l2=65536-2048;
	l3a=65536;

	p0=idxtab[((pxy[0]-acy)*l1+l3a)>>12];
	BGBBTJ_BitsLE_Write3Bits(&bits, p0);
	for(i=1; i<16; i++)
	{
		p0=idxtab[((pxy[i]-acy)*l1+l3a)>>12];
		BGBBTJ_BitsLE_Write4Bits(&bits, p0);
	}

	BGBBTJ_BitsLE_FlushBits(&bits);
}

void BGBBTJ_BC6HS_EncodeBlockUByte(byte *block,
	byte *rgba, int xstride, int ystride)
{
	int blkb[16*4];
	int i, j, k, l;
	
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		k=i*ystride+j*xstride; l=(i*4+j)*4;
		blkb[l+0]=BGBBTJ_EncodeFloat16((rgba[k+0])/255.0);
		blkb[l+1]=BGBBTJ_EncodeFloat16((rgba[k+1])/255.0);
		blkb[l+2]=BGBBTJ_EncodeFloat16((rgba[k+2])/255.0);
	}
	
	BGBBTJ_BC6H_EncodeBlock_Mode3(block, blkb, 4, 4*4);
}

void BGBBTJ_BC6HS_EncodeBlockSByte(byte *block,
	sbyte *rgba, int xstride, int ystride)
{
	int blkb[16*4];
	int i, j, k, l;
	
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		k=i*ystride+j*xstride; l=(i*4+j)*4;
		blkb[l+0]=BGBBTJ_EncodeFloat16((rgba[k+0])/127.0);
		blkb[l+1]=BGBBTJ_EncodeFloat16((rgba[k+1])/127.0);
		blkb[l+2]=BGBBTJ_EncodeFloat16((rgba[k+2])/127.0);
	}
	
	BGBBTJ_BC6H_EncodeBlock_Mode3(block, blkb, 4, 4*4);
}

void BGBBTJ_BC6HS_EncodeBlockFloat(byte *block,
	float *rgba, int xstride, int ystride)
{
	int blkb[16*4];
	int i, j, k, l;
	
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		k=i*ystride+j*xstride; l=(i*4+j)*4;
		blkb[l+0]=BGBBTJ_EncodeFloat16(rgba[k+0]);
		blkb[l+1]=BGBBTJ_EncodeFloat16(rgba[k+1]);
		blkb[l+2]=BGBBTJ_EncodeFloat16(rgba[k+2]);
	}
	
	BGBBTJ_BC6H_EncodeBlock_Mode3(block, blkb, 4, 4*4);
}

void BGBBTJ_BC6HU_EncodeBlockFloat(byte *block,
	float *rgba, int xstride, int ystride)
{
	int blkb[16*4];
	int i, j, k, l;
	
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		k=i*ystride+j*xstride; l=(i*4+j)*4;
		blkb[l+0]=BGBBTJ_EncodeFloat16(rgba[k+0])<<1;
		blkb[l+1]=BGBBTJ_EncodeFloat16(rgba[k+1])<<1;
		blkb[l+2]=BGBBTJ_EncodeFloat16(rgba[k+2])<<1;
	}
	
	BGBBTJ_BC6H_EncodeBlock_Mode3(block, blkb, 4, 4*4);
}

void BGBBTJ_BC6HS_EncodeBlockUByteExp(byte *block,
	byte *rgba, int xstride, int ystride)
{
	int blkb[16*4];
	float f, g;
	int i, j, k, l;
	
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		k=i*ystride+j*xstride; l=(i*4+j)*4;
		f=pow(2, rgba[k+2]-64)/255.0;
		blkb[l+0]=BGBBTJ_EncodeFloat16((rgba[k+0])*f);
		blkb[l+1]=BGBBTJ_EncodeFloat16((rgba[k+1])*f);
		blkb[l+2]=BGBBTJ_EncodeFloat16((rgba[k+2])*f);
	}
	
	BGBBTJ_BC6H_EncodeBlock_Mode3(block, blkb, 4, 4*4);
}


void BGBBTJ_BC6HS_EncodeBlockHFloat(byte *block,
	u16 *rgba, int xstride, int ystride)
{
	int blkb[16*4];
	float f, g;
	int i, j, k, l;
	
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		k=i*ystride+j*xstride; l=(i*4+j)*4;
		blkb[l+0]=rgba[k+0];
		blkb[l+1]=rgba[k+1];
		blkb[l+2]=rgba[k+2];
	}
	
	BGBBTJ_BC6H_EncodeBlock_Mode3(block, blkb, 4, 4*4);
}

void BGBBTJ_BC6HU_EncodeBlockHFloat(byte *block,
	u16 *rgba, int xstride, int ystride)
{
	int blkb[16*4];
	float f, g;
	int i, j, k, l;
	
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		k=i*ystride+j*xstride; l=(i*4+j)*4;
		blkb[l+0]=rgba[k+0]<<1;
		blkb[l+1]=rgba[k+1]<<1;
		blkb[l+2]=rgba[k+2]<<1;
	}
	
	BGBBTJ_BC6H_EncodeBlock_Mode3(block, blkb, 4, 4*4);
}


BGBBTJ_API void BGBBTJ_BC6HS_EncodeImageUByte(byte *block,
	byte *rgba, int xs, int ys, int stride)
{
	int xs1, ys1;
	int i, j;
	
	BGBBTJ_Float16_Init();
	
	xs1=xs>>2; ys1=ys>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		BGBBTJ_BC6HS_EncodeBlockUByte(
			block+(i*xs1+j)*16,
			rgba+(i*4*xs+j*4)*stride,
			stride, xs*stride);
	}
}

BGBBTJ_API void BGBBTJ_BC6HS_EncodeImageSByte(byte *block,
	sbyte *rgba, int xs, int ys, int stride)
{
	int xs1, ys1;
	int i, j;
	
	BGBBTJ_Float16_Init();

	xs1=xs>>2; ys1=ys>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		BGBBTJ_BC6HS_EncodeBlockSByte(
			block+(i*xs1+j)*16,
			rgba+(i*4*xs+j*4)*stride,
			stride, xs*stride);
	}
}

BGBBTJ_API void BGBBTJ_BC6HS_EncodeImageFloat(byte *block,
	float *rgba, int xs, int ys, int stride)
{
	int xs1, ys1;
	int i, j;

	BGBBTJ_Float16_Init();
	
	xs1=xs>>2; ys1=ys>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		BGBBTJ_BC6HS_EncodeBlockFloat(
			block+(i*xs1+j)*16,
			rgba+(i*4*xs+j*4)*stride,
			stride, xs*stride);
	}
}
