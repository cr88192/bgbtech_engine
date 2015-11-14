/*
DXTn Fast Mip-Mapping
*/

#include <bgbbtj.h>

//0000 -> 00
//0001 -> 01
//0010 -> 01
//0011 -> 10
//0100 -> 01
//0101 -> 01
//0110 -> 10
//0111 -> 10
//1000 -> 01
//1001 -> 10
//1010 -> 10
//1011 -> 10
//1100 -> 10
//1101 -> 10
//1110 -> 11
//1111 -> 11

static int bgbbtj_bcnmip_horizlut4[16]=
{
0,1,1,2, 1,1,2,2, 1,2,2,2, 2,2,3,3
};

static int bgbbtj_bcnmip_vertilut4[16]=
{
0x00, 0x01, 0x02, 0x03,
0x10, 0x11, 0x12, 0x13,
0x20, 0x21, 0x22, 0x23,
0x30, 0x31, 0x32, 0x33,
};

static int bgbbtj_bcnmip_horizlut[256];
static int bgbbtj_bcnmip_vertilut[256];
static byte *bgbbtj_bcnmip_horzlut;
static byte *bgbbtj_bcnmip_vertlut;

void BGBBTJ_BCnMip_Init()
{
	static int init=0;
	int i, j;

	if(init)return;
	init=1;
	
	for(i=0; i<256; i++)
	{
		bgbbtj_bcnmip_horizlut[i]=
			(bgbbtj_bcnmip_horizlut4[i>>4]<<2)|
			 bgbbtj_bcnmip_horizlut4[i&15];

		bgbbtj_bcnmip_vertilut[i]=
			(bgbbtj_bcnmip_vertilut4[i>>4]<<8)|
			 bgbbtj_bcnmip_vertilut4[i&15];
	}
	
	bgbbtj_bcnmip_horzlut=malloc(65536);
	bgbbtj_bcnmip_vertlut=malloc(65536);

	for(i=0; i<65536; i++)
	{
		bgbbtj_bcnmip_horzlut[i]=
			(bgbbtj_bcnmip_horizlut[(i>>8)&255]<<4)|
			 bgbbtj_bcnmip_horizlut[(i>>0)&255];

		j=(	bgbbtj_bcnmip_vertilut[(i>>8)&255]<<2)|
			bgbbtj_bcnmip_vertilut[(i>>0)&255];
		bgbbtj_bcnmip_vertlut[i]=
			(bgbbtj_bcnmip_horizlut[(j>>8)&255]<<4)|
			 bgbbtj_bcnmip_horizlut[(j>>0)&255];
	}
}

void BGBBTJ_BCnMip_MergeBlockHorizDXT5B(byte *dst, byte *srca, byte *srcb)
{
	byte clr[16], clrb[8];
	int i, j;
	
	clr[ 0]=srca[1]&0xF8;
	clr[ 1]=((srca[1]<<5)|(srca[0]>>3))&0xFC;
	clr[ 2]=(srca[0]<<3)&0xF8;
	clr[ 4]=srca[3]&0xF8;
	clr[ 5]=((srca[3]<<5)|(srca[2]>>3))&0xFC;
	clr[ 6]=(srca[2]<<3)&0xF8;
	clr[ 8]=srcb[1]&0xF8;
	clr[ 9]=((srcb[1]<<5)|(srcb[0]>>3))&0xFC;
	clr[10]=(srcb[0]<<3)&0xF8;
	clr[12]=srcb[3]&0xF8;
	clr[13]=((srcb[3]<<5)|(srcb[2]>>3))&0xFC;
	clr[14]=(srcb[2]<<3)&0xF8;
	
	clrb[0]=(clr[0]+clr[ 8]+1)>>1;
	clrb[1]=(clr[1]+clr[ 9]+1)>>1;
	clrb[2]=(clr[2]+clr[10]+1)>>1;
	clrb[4]=(clr[4]+clr[12]+1)>>1;
	clrb[5]=(clr[5]+clr[13]+1)>>1;
	clrb[6]=(clr[6]+clr[14]+1)>>1;

	i=((clrb[0]<<8)&0xF800) | ((clrb[1]<<3)&0x07E0) | (clrb[2]>>3);
	j=((clrb[4]<<8)&0xF800) | ((clrb[5]<<3)&0x07E0) | (clrb[6]>>3);
	dst[0]=i;		dst[1]=i>>8;
	dst[2]=j;		dst[3]=j>>8;

	if(i==j)
	{
		dst[4]=0;	dst[5]=0;
		dst[6]=0;	dst[7]=0;
		return;
	}

//	dst[4]=(bgbbtj_bcnmip_horizlut[srca[4]]<<4)|
//		bgbbtj_bcnmip_horizlut[srcb[4]];
//	dst[5]=(bgbbtj_bcnmip_horizlut[srca[5]]<<4)|
//		bgbbtj_bcnmip_horizlut[srcb[5]];
//	dst[6]=(bgbbtj_bcnmip_horizlut[srca[6]]<<4)|
//		bgbbtj_bcnmip_horizlut[srcb[6]];
//	dst[7]=(bgbbtj_bcnmip_horizlut[srca[7]]<<4)|
//		bgbbtj_bcnmip_horizlut[srcb[7]];

	dst[4]=bgbbtj_bcnmip_horzlut[(srca[4]<<8)|srcb[4]];
	dst[5]=bgbbtj_bcnmip_horzlut[(srca[5]<<8)|srcb[5]];
	dst[6]=bgbbtj_bcnmip_horzlut[(srca[6]<<8)|srcb[6]];
	dst[7]=bgbbtj_bcnmip_horzlut[(srca[7]<<8)|srcb[7]];
}

void BGBBTJ_BCnMip_MergeBlockVertDXT5B(byte *dst, byte *srca, byte *srcb)
{
	byte clr[16], clrb[8];
	int i, j;
	
	clr[ 0]=srca[1]&0xF8;
	clr[ 1]=((srca[1]<<5)|(srca[0]>>3))&0xFC;
	clr[ 2]=(srca[0]<<3)&0xF8;
	clr[ 4]=srca[3]&0xF8;
	clr[ 5]=((srca[3]<<5)|(srca[2]>>3))&0xFC;
	clr[ 6]=(srca[2]<<3)&0xF8;
	clr[ 8]=srcb[1]&0xF8;
	clr[ 9]=((srcb[1]<<5)|(srcb[0]>>3))&0xFC;
	clr[10]=(srcb[0]<<3)&0xF8;
	clr[12]=srcb[3]&0xF8;
	clr[13]=((srcb[3]<<5)|(srcb[2]>>3))&0xFC;
	clr[14]=(srcb[2]<<3)&0xF8;
	
	clrb[0]=(clr[0]+clr[ 8]+1)>>1;
	clrb[1]=(clr[1]+clr[ 9]+1)>>1;
	clrb[2]=(clr[2]+clr[10]+1)>>1;
	clrb[4]=(clr[4]+clr[12]+1)>>1;
	clrb[5]=(clr[5]+clr[13]+1)>>1;
	clrb[6]=(clr[6]+clr[14]+1)>>1;

	i=((clrb[0]<<8)&0xF800) | ((clrb[1]<<3)&0x07E0) | (clrb[2]>>3);
	j=((clrb[4]<<8)&0xF800) | ((clrb[5]<<3)&0x07E0) | (clrb[6]>>3);
	dst[0]=i;		dst[1]=i>>8;
	dst[2]=j;		dst[3]=j>>8;

	if(i==j)
	{
		dst[4]=0;	dst[5]=0;
		dst[6]=0;	dst[7]=0;
		return;
	}

//	dst[4]=(bgbbtj_bcnmip_horizlut[srca[4]]<<4)|
//		bgbbtj_bcnmip_horizlut[srcb[4]];
//	dst[5]=(bgbbtj_bcnmip_horizlut[srca[5]]<<4)|
//		bgbbtj_bcnmip_horizlut[srcb[5]];
//	dst[6]=(bgbbtj_bcnmip_horizlut[srca[6]]<<4)|
//		bgbbtj_bcnmip_horizlut[srcb[6]];
//	dst[7]=(bgbbtj_bcnmip_horizlut[srca[7]]<<4)|
//		bgbbtj_bcnmip_horizlut[srcb[7]];

	dst[4]=bgbbtj_bcnmip_vertlut[(srca[4]<<8)|srcb[4]];
	dst[5]=bgbbtj_bcnmip_vertlut[(srca[5]<<8)|srcb[5]];
	dst[6]=bgbbtj_bcnmip_vertlut[(srca[6]<<8)|srcb[6]];
	dst[7]=bgbbtj_bcnmip_vertlut[(srca[7]<<8)|srcb[7]];
}

void BGBBTJ_BCnMip_MergeBlockDXT5C(byte *dst,
	byte *srca, byte *srcb,
	byte *srcc, byte *srcd)
{
	static const char idxtab[16]=
		{ 0,0,0,0, 0,0,2,2, 3,3,1,1, 1,1,1,1 };

	byte *srcn[4];
	short clrn[4][8];
	short clyn[4][4];
	short pxyn[4][16];
	short clr[8];
	short pxy[16];
	int mcy, ncy, acy;
	int mcr, mcg, mcb;
	int ncr, ncg, ncb;
	int p0, p1, p2, p3;
	int l0, l1, l2, l3;
	int i, j, k, l;
	
	srcn[0]=srca; srcn[1]=srcb;
	srcn[2]=srcc; srcn[3]=srcd;
	mcy=512; ncy=-512;
	for(i=0; i<4; i++)
	{
		clrn[i][0]=srcn[i][1]&0xF8 + 4;
		clrn[i][1]=((srcn[i][1]<<5)|(srcn[i][0]>>3))&0xFC + 2;
		clrn[i][2]=(srcn[i][0]<<3)&0xF8 + 4;
		clrn[i][3]=(clrn[i][0]+2*clrn[i][1]+clrn[i][2]+2)>>2;
		clrn[i][4]=srcn[i][3]&0xF8 + 4;
		clrn[i][5]=((srcn[i][3]<<5)|(srcn[i][2]>>3))&0xFC + 2;
		clrn[i][6]=(srcn[i][2]<<3)&0xF8 + 4;
		clrn[i][7]=(clrn[i][4]+2*clrn[i][5]+clrn[i][6]+2)>>2;

		if(clrn[i][3]<mcy)
		{
			clr[0]=clrn[i][0];
			clr[1]=clrn[i][1];
			clr[2]=clrn[i][2];
			mcy=clrn[i][3];
		}

		if(clrn[i][7]>ncy)
		{
			clr[4]=clrn[i][4];
			clr[5]=clrn[i][5];
			clr[6]=clrn[i][6];
			ncy=clrn[i][7];
		}

		clyn[i][0]=clrn[i][3];
		clyn[i][1]=clrn[i][7];
		clyn[i][2]=(11*clyn[i][0]+ 5*clyn[i][1]+8)>>4;
		clyn[i][3]=( 5*clyn[i][0]+11*clyn[i][1]+8)>>4;

		j=srcn[i][4];
		pxyn[i][ 0]=clyn[i][(j>>6)&3];
		pxyn[i][ 1]=clyn[i][(j>>4)&3];
		pxyn[i][ 2]=clyn[i][(j>>2)&3];
		pxyn[i][ 3]=clyn[i][(j>>0)&3];
		j=srcn[i][5];
		pxyn[i][ 4]=clyn[i][(j>>6)&3];
		pxyn[i][ 5]=clyn[i][(j>>4)&3];
		pxyn[i][ 6]=clyn[i][(j>>2)&3];
		pxyn[i][ 7]=clyn[i][(j>>0)&3];
		j=srcn[i][6];
		pxyn[i][ 8]=clyn[i][(j>>6)&3];
		pxyn[i][ 9]=clyn[i][(j>>4)&3];
		pxyn[i][10]=clyn[i][(j>>2)&3];
		pxyn[i][11]=clyn[i][(j>>0)&3];
		j=srcn[i][7];
		pxyn[i][12]=clyn[i][(j>>6)&3];
		pxyn[i][13]=clyn[i][(j>>4)&3];
		pxyn[i][14]=clyn[i][(j>>2)&3];
		pxyn[i][15]=clyn[i][(j>>0)&3];
	}

#if 1
	mcr=(clrn[0][0]+clrn[1][0]+clrn[2][0]+clrn[3][0]+2)>>2;
	mcg=(clrn[0][1]+clrn[1][1]+clrn[2][1]+clrn[3][1]+2)>>2;
	mcb=(clrn[0][2]+clrn[1][2]+clrn[2][2]+clrn[3][2]+2)>>2;

	ncr=(clrn[0][4]+clrn[1][4]+clrn[2][4]+clrn[3][4]+2)>>2;
	ncg=(clrn[0][5]+clrn[1][5]+clrn[2][5]+clrn[3][5]+2)>>2;
	ncb=(clrn[0][6]+clrn[1][6]+clrn[2][6]+clrn[3][6]+2)>>2;
	
	l0=(mcr+2*mcg+mcb+2)>>2;
	l1=(ncr+2*ncg+ncb+2)>>2;
	if(l0) { l2=(256*mcy)/l0; }
	if(l1) { l3=(256*ncy)/l1; }
	
	mcr=(mcr*l2+128)>>8;
	mcg=(mcg*l2+128)>>8;
	mcb=(mcb*l2+128)>>8;
	ncr=(ncr*l3+128)>>8;
	ncg=(ncg*l3+128)>>8;
	ncb=(ncb*l3+128)>>8;
#endif

//	mcr=clr[0]; mcg=clr[1]; mcb=clr[2];
//	ncr=clr[4]; ncg=clr[5]; ncb=clr[6];

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

	i=((mcr<<8)&0xF800) | ((mcg<<3)&0x07E0) | ((mcb>>3)&0x001F);
	j=((ncr<<8)&0xF800) | ((ncg<<3)&0x07E0) | ((ncb>>3)&0x001F);

//	i=((clr[0]<<8)&0xF800) | ((clr[1]<<3)&0x07E0) | (clr[2]>>3);
//	j=((clr[4]<<8)&0xF800) | ((clr[5]<<3)&0x07E0) | (clr[6]>>3);
	dst[0]=i;		dst[1]=i>>8;
	dst[2]=j;		dst[3]=j>>8;

	if(i==j)
	{
		dst[4]=0;	dst[5]=0;
		dst[6]=0;	dst[7]=0;
		return;
	}
	
	for(i=0; i<2; i++)
		for(j=0; j<2; j++)
	{
		k=i*8+j*2; l=i*2+j;
		pxy[k+0]=(pxyn[l][ 0]+pxyn[l][ 1]+pxyn[l][ 4]+pxyn[l][ 5])>>2;
		pxy[k+1]=(pxyn[l][ 2]+pxyn[l][ 3]+pxyn[l][ 6]+pxyn[l][ 7])>>2;
		pxy[k+4]=(pxyn[l][ 8]+pxyn[l][ 9]+pxyn[l][12]+pxyn[l][13])>>2;
		pxy[k+5]=(pxyn[l][10]+pxyn[l][11]+pxyn[l][14]+pxyn[l][15])>>2;
	}
	
#if 0
	pxy[ 0]=(pxyn[0][ 0]+pxyn[0][ 1]+pxyn[0][ 4]+pxyn[0][ 5])>>2;
	pxy[ 1]=(pxyn[0][ 2]+pxyn[0][ 3]+pxyn[0][ 6]+pxyn[0][ 7])>>2;
	pxy[ 2]=(pxyn[1][ 0]+pxyn[1][ 1]+pxyn[1][ 4]+pxyn[1][ 5])>>2;
	pxy[ 3]=(pxyn[1][ 2]+pxyn[1][ 3]+pxyn[1][ 6]+pxyn[1][ 7])>>2;
	pxy[ 4]=(pxyn[0][ 8]+pxyn[0][ 9]+pxyn[0][ 8]+pxyn[0][ 9])>>2;
	pxy[ 5]=(pxyn[0][10]+pxyn[0][11]+pxyn[0][14]+pxyn[0][15])>>2;
	pxy[ 6]=(pxyn[1][ 8]+pxyn[1][ 9]+pxyn[1][10]+pxyn[1][11])>>2;
	pxy[ 7]=(pxyn[1][10]+pxyn[1][11]+pxyn[1][14]+pxyn[1][15])>>2;
	pxy[ 8]=(pxyn[2][ 0]+pxyn[2][ 1]+pxyn[2][ 4]+pxyn[2][ 5])>>2;
	pxy[ 9]=(pxyn[2][ 2]+pxyn[2][ 3]+pxyn[2][ 6]+pxyn[2][ 7])>>2;
	pxy[10]=(pxyn[3][ 0]+pxyn[3][ 1]+pxyn[3][ 4]+pxyn[3][ 5])>>2;
	pxy[11]=(pxyn[3][ 2]+pxyn[3][ 3]+pxyn[3][ 6]+pxyn[3][ 7])>>2;
	pxy[12]=(pxyn[2][ 8]+pxyn[2][ 9]+pxyn[2][ 8]+pxyn[2][ 9])>>2;
	pxy[13]=(pxyn[2][10]+pxyn[2][11]+pxyn[2][14]+pxyn[2][15])>>2;
	pxy[14]=(pxyn[3][ 8]+pxyn[3][ 9]+pxyn[3][10]+pxyn[3][11])>>2;
	pxy[15]=(pxyn[3][10]+pxyn[3][11]+pxyn[3][14]+pxyn[3][15])>>2;
#endif

	acy=(mcy+ncy)>>1;			//Median Luma
	l1=32768/(ncy-acy+1);		//Fix-Point Scale (Luma)
	l3=-2048;

	p0=idxtab[(((pxy[ 0]-acy)*l1+l3)>>13)+8];
	p1=idxtab[(((pxy[ 1]-acy)*l1+l3)>>13)+8];
	p2=idxtab[(((pxy[ 2]-acy)*l1+l3)>>13)+8];
	p3=idxtab[(((pxy[ 3]-acy)*l1+l3)>>13)+8];
	dst[4]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
	p0=idxtab[(((pxy[ 4]-acy)*l1+l3)>>13)+8];
	p1=idxtab[(((pxy[ 5]-acy)*l1+l3)>>13)+8];
	p2=idxtab[(((pxy[ 6]-acy)*l1+l3)>>13)+8];
	p3=idxtab[(((pxy[ 7]-acy)*l1+l3)>>13)+8];
	dst[5]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
	p0=idxtab[(((pxy[ 8]-acy)*l1+l3)>>13)+8];
	p1=idxtab[(((pxy[ 9]-acy)*l1+l3)>>13)+8];
	p2=idxtab[(((pxy[10]-acy)*l1+l3)>>13)+8];
	p3=idxtab[(((pxy[11]-acy)*l1+l3)>>13)+8];
	dst[6]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
	p0=idxtab[(((pxy[12]-acy)*l1+l3)>>13)+8];
	p1=idxtab[(((pxy[13]-acy)*l1+l3)>>13)+8];
	p2=idxtab[(((pxy[14]-acy)*l1+l3)>>13)+8];
	p3=idxtab[(((pxy[15]-acy)*l1+l3)>>13)+8];
	dst[7]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
}


void BGBBTJ_BCnMip_MergeBlockDXT5B(byte *dst,
	byte *srca, byte *srcb,
	byte *srcc, byte *srcd)
{
//	byte tmpa[8], tmpb[8];

//	BGBBTJ_BCnMip_MergeBlockHorizDXT5B(tmpa, srca, srcb);
//	BGBBTJ_BCnMip_MergeBlockHorizDXT5B(tmpb, srcc, srcd);
//	BGBBTJ_BCnMip_MergeBlockVertDXT5B(dst, tmpa, tmpb);

	BGBBTJ_BCnMip_MergeBlockDXT5C(dst, srca, srcb, srcc, srcd);
}

BGBBTJ_API void BGBBTJ_BCnMip_MergeDXT5B(
	byte *dst, byte *src, int xs, int ys, int stride)
{
	byte *srca, *srcb, *srcc, *srcd, *dsta;
	int xs2, ys2;
	int xs3, ys3;
	int i, j, k;

	if((xs<=4) || (ys<=4))
	{
		BGBBTJ_BCnMip_MergeBlockDXT5B(dst, src, src, src, src);
		return;
	}

	if((xs<=8) || (ys<=8))
	{
		srca=src+0*stride; srcb=src+1*stride;
		srcc=src+2*stride; srcd=src+3*stride;
		BGBBTJ_BCnMip_MergeBlockDXT5B(dst, srca, srcb, srcc, srcd);
		return;
	}

	xs2=xs>>2; ys2=ys>>2;
	xs3=xs>>3; ys3=ys>>3;

	for(i=0; i<ys3; i++)
		for(j=0; j<xs3; j++)
	{
		k=(i*2+0)*xs2+(j*2+0); srca=src+k*stride;
		k=(i*2+0)*xs2+(j*2+1); srcb=src+k*stride;
		k=(i*2+1)*xs2+(j*2+0); srcc=src+k*stride;
		k=(i*2+1)*xs2+(j*2+1); srcd=src+k*stride;
		k=i*xs3+j; dsta=dst+k*stride;
		BGBBTJ_BCnMip_MergeBlockDXT5B(dsta, srca, srcb, srcc, srcd);
	}
}

BGBBTJ_API void BGBBTJ_BCnMip_MergeDXT5(
	byte *dst, byte *src, int xs, int ys)
{
	BGBBTJ_BCnMip_Init();

	BGBBTJ_BCnMip_MergeDXT5B(dst+8, src+8, xs, ys, 16);
}
