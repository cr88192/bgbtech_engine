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

char *bgbbtj_gl_extensions=NULL;
char **bgbbtj_gl_extensions_xsa=NULL;
int *bgbbtj_gl_extensions_xsac;
int bgbbtj_gl_extensions_xsah[1024];
int bgbbtj_gl_extensions_nxsa;

BGBBTJ_API int BGBBTJ_SetGlExtensions(char *str)
{
	char tb[256];
	char **xsa;
	int *xsac;
	char *s, *t;
	int nxsa, mxsa;
	int i, j, k;

	bgbbtj_gl_extensions=str;
	
	mxsa=1024; nxsa=0;
	xsa=malloc(mxsa*sizeof(char *));
	
	s=str;
	while(*s)
	{
		if(*s<=' ')
		{
			while(*s && (*s<=' '))s++;
			continue;
		}
		
		t=tb;
		while(*s && (*s>' '))*t++=*s++;
		*t++=0;
		
		if((nxsa+1)>=mxsa)
		{
			mxsa=mxsa+(mxsa>>1);
			xsa=realloc(xsa, mxsa*sizeof(char *));
		}
		
		xsa[nxsa++]=strdup(tb);
	}

	xsa=realloc(xsa, nxsa*sizeof(char *));
	xsac=malloc(nxsa*sizeof(int));
	
	bgbbtj_gl_extensions_xsa=xsa;
	bgbbtj_gl_extensions_xsac=xsac;
	bgbbtj_gl_extensions_nxsa=nxsa;

	for(i=0; i<1024; i++)
		bgbbtj_gl_extensions_xsah[i]=-1;
		
	for(i=0; i<nxsa; i++)
	{
		s=xsa[i]; k=0;
		while(*s)
			{ k=(k*251)+(*s++); }
		k=((k*251)>>8)&1023;
		
		xsac[i]=bgbbtj_gl_extensions_xsah[k];
		bgbbtj_gl_extensions_xsah[k]=i;
	}
	
	return(0);
}

BGBBTJ_API int BGBBTJ_CheckGlExtension(char *str)
{
	char *s, *t;
	int i, j, k;

	if(!bgbbtj_gl_extensions)
		return(0);

	s=str; k=0;
	while(*s)
		{ k=(k*251)+(*s++); }
	k=((k*251)>>8)&1023;
	
	i=bgbbtj_gl_extensions_xsah[k];
	while(i>0)
	{
		if(!strcmp(bgbbtj_gl_extensions_xsa[i], str))
			return(1);
		i=bgbbtj_gl_extensions_xsac[i];
	}
	return(0);
}

BGBBTJ_API int BGBBTJ_BCn_RoundColorNearest5Bits(int v)
{
	int p0, p1, d0, d1, p;
	
	p0=(v>>3); p0=(p0<<3)|(p0>>2);
	p1=(v>>3)+1; p1=(p1<<3)|(p1>>2);
	d0=p0-v; d0=d0*d0;
	d1=p1-v; d1=d1*d1;
	p=(d0<d1)?p0:p1;
	p=(p<0)?0:((p>255)?255:p);
	return(p);
}

BGBBTJ_API int BGBBTJ_BCn_RoundColorNearest6Bits(int v)
{
	int p0, p1, d0, d1, p;
	
	p0=(v>>2); p0=(p0<<2)|(p0>>4);
	p1=(v>>2)+1; p1=(p1<<2)|(p1>>4);
	d0=p0-v; d0=d0*d0;
	d1=p1-v; d1=d1*d1;
	p=(d0<d1)?p0:p1;
	p=(p<0)?0:((p>255)?255:p);
	return(p);
}

BGBBTJ_API int BGBBTJ_BCn_TranscodeBlockBC1ToBC3(
	byte *sblk, byte *dblk)
{
	u32 v;
	int p0, p1, p2, p3, p4, p5, p6, p7;
	int i, j, k;
	
#if defined(X86) || defined(X86_64)
	i=((u16 *)sblk)[0];
	j=((u16 *)sblk)[1];
#else
	i=sblk[0]|(sblk[1]<<8);
	j=sblk[2]|(sblk[3]<<8);
#endif

	if(j<i)
	{
		//Opaque Block
#if defined(X86) || defined(X86_64)
		((u64 *)dblk)[0]=0x000000000000FFFFULL;
		((u64 *)dblk)[1]=*(u64 *)sblk;
#else
		dblk[0]=255;	dblk[1]=255;	dblk[2]=0;	dblk[3]=0;
		dblk[4]=0;		dblk[5]=0;		dblk[6]=0;	dblk[7]=0;
		memcpy(dblk+8, sblk, 8);
//		memcpy(dblk+i*16+8, sblk+i*8, 8);
#endif
	}else
	{
#if defined(X86) || defined(X86_64)
		v=((u32 *)sblk)[1];
		if(v==0xFFFFFFFF)
		{
			((u64 *)dblk)[0]=0x0000000000000000ULL;
			((u64 *)dblk)[1]=*(u64 *)sblk;
			return(0);
		}
#else
		v=sblk[4]|(sblk[5]<<8)|(sblk[6]<<16)|(sblk[7]<<24);
		if(v==0xFFFFFFFF)
		{
			memset(dblk, 0, 8);
			memcpy(dblk+8, sblk, 8);
		}
#endif

		//Block with Alpha
		dblk[0]=0;	dblk[1]=255;

		p0=((sblk[4]&0x03)!=0x03);
		p1=((sblk[4]&0x0C)!=0x0C);
		p2=((sblk[4]&0x30)!=0x30);
		p3=((sblk[4]&0xC0)!=0xC0);
		p4=((sblk[5]&0x03)!=0x03);
		p5=((sblk[5]&0x0C)!=0x0C);
		p6=((sblk[5]&0x30)!=0x30);
		p7=((sblk[5]&0xC0)!=0xC0);

		k=p0|(p1<<3)|(p2<<6)|(p3<<9)|
			(p4<<12)||(p5<<15)|(p6<<18)|(p7<<21);
		dblk[2]=k;		dblk[3]=k>>8;		dblk[4]=k>>16;

//		dblk[2]=p0|(p1<<3)|(p2<<6);
//		dblk[3]=(p2>>2)|(p3<<1)|(p4<<4)|(p5<<7);
//		dblk[4]=(p5>>1)|(p6<<2)|(p7<<5);

		p0=((sblk[6]&0x03)!=0x03);
		p1=((sblk[6]&0x0C)!=0x0C);
		p2=((sblk[6]&0x30)!=0x30);
		p3=((sblk[6]&0xC0)!=0xC0);
		p4=((sblk[7]&0x03)!=0x03);
		p5=((sblk[7]&0x0C)!=0x0C);
		p6=((sblk[7]&0x30)!=0x30);
		p7=((sblk[7]&0xC0)!=0xC0);

		k=p0|(p1<<3)|(p2<<6)|(p3<<9)|
			(p4<<12)||(p5<<15)|(p6<<18)|(p7<<21);
		dblk[5]=k;		dblk[6]=k>>8;		dblk[7]=k>>16;

//		dblk[5]=p0|(p1<<3)|(p2<<6);
//		dblk[6]=(p2>>2)|(p3<<1)|(p4<<4)|(p5<<7);
//		dblk[7]=(p5>>1)|(p6<<2)|(p7<<5);

		((u64 *)dblk)[1]=*(u64 *)sblk;
	}
	return(0);
}

BGBBTJ_API int BGBBTJ_BCn_TranscodeBC1ToBC3(
	byte *sblk, byte *dblk, int xs, int ys)
{
	byte *db, *sb;
	int i, n;
	
	n=((xs+3)/4)*((ys+3)/4);
	for(i=0; i<n; i++)
	{
		db=dblk+i*16;
		sb=sblk+i*8;

		BGBBTJ_BCn_TranscodeBlockBC1ToBC3(sb, db);

#if 0
#if defined(X86) || defined(X86_64)
//#if 0
		((u64 *)db)[0]=0x000000000000FFFFULL;
		((u64 *)db)[1]=*(u64 *)sb;
#else
		db[0]=255;	db[1]=255;		db[2]=0;	db[3]=0;
		db[4]=0;	db[5]=0;		db[6]=0;	db[7]=0;
		memcpy(db+8, sb, 8);
//		memcpy(dblk+i*16+8, sblk+i*8, 8);
#endif
#endif
	}
	return(0);
}

BGBBTJ_API int BGBBTJ_BCn_TranscodeBC1ToBC3B(
	byte *sblk, byte *dblk, int xs, int ys)
{
	byte *db, *sb;
	int i, n;
	
	n=((xs+3)/4)*((ys+3)/4);
	for(i=0; i<n; i++)
	{
		db=dblk+i*16;
		sb=sblk+i*16+8;

		BGBBTJ_BCn_TranscodeBlockBC1ToBC3(sb, db);
	}
	return(0);
}

BGBBTJ_API int BGBBTJ_BCn_TranscodeBC1ToBC3B_Opaque(
	byte *sblk, byte *dblk, int xs, int ys)
{
	byte *db, *sb;
	int i, n;
	
	n=((xs+3)/4)*((ys+3)/4);
	for(i=0; i<n; i++)
	{
		db=dblk+i*16;
		sb=sblk+i*16+8;

#if defined(X86) || defined(X86_64)
//#if 0
		((u64 *)db)[0]=0x000000000000FFFFULL;
		((u64 *)db)[1]=*(u64 *)sb;
#else
		db[0]=255;	db[1]=255;		db[2]=0;	db[3]=0;
		db[4]=0;	db[5]=0;		db[6]=0;	db[7]=0;
		memcpy(db+8, sb, 8);
//		memcpy(dblk+i*16+8, sblk+i*8, 8);
#endif

//		BGBBTJ_BCn_TranscodeBlockBC1ToBC3(sb, db);
	}
	return(0);
}

BGBBTJ_API int BGBBTJ_BCn_CheckEncodeImageAlpha(
	byte *rgba, int xs, int ys, int stride)
{
	int i, j;
	
	if(!rgba)return(0);
	
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

	case BGBBTJ_JPG_BC6:
		BGBBTJ_BC6HS_EncodeImageUByte(block, rgba, xs, ys, stride);
		break;
	case BGBBTJ_JPG_BC7:
//		BGBBTJ_BC7_EncodeImageBC7(block, rgba, xs, ys, stride);
		BGBBTJ_BC7_EncodeImageBestBC7(block, rgba, xs, ys, stride);
		break;
	}
}

BGBBTJ_API int BGBBTJ_BCn_CheckGlSupportS3TC(void)
{
	static int ret=-1;

	if(ret>=0)
		return(ret);

	ret=0;
	if(BGBBTJ_CheckGlExtension("GL_EXT_texture_compression_s3tc"))
		ret=1;
	return(ret);
}

BGBBTJ_API int BGBBTJ_BCn_CheckGlSupportBPTC(void)
{
	static int ret=-1;

	if(ret>=0)
		return(ret);

	ret=0;
	if(BGBBTJ_CheckGlExtension("GL_ARB_texture_compression_bptc"))
		ret=1;
	return(ret);
}

BGBBTJ_API int BGBBTJ_BCn_CheckGlSupportAny(void)
{
	if(BGBBTJ_BCn_CheckGlSupportS3TC() ||
		BGBBTJ_BCn_CheckGlSupportBPTC())
			return(1);
	return(0);
}

BGBBTJ_API int BGBBTJ_BCn_CheckGlSupportColorFormat(int fmt)
{
	int ret;

	switch(fmt)
	{
	case BGBBTJ_JPG_BC1:
	case BGBBTJ_JPG_BC1F:
	case BGBBTJ_JPG_BC2:
	case BGBBTJ_JPG_BC3:
	case BGBBTJ_JPG_BC3F:
		ret=0;
		if(BGBBTJ_BCn_CheckGlSupportS3TC())
			ret=1;
		break;
	case BGBBTJ_JPG_BC3_UVAY:
		ret=0;
		if(BGBBTJ_BCn_CheckGlSupportS3TC())
			ret=1;
//		cmp=GL_COMPRESSED_RGBA_S3TC_DXT5_EXT; blksz=16;
		break;
	case BGBBTJ_JPG_BC6H_SF16:
	case BGBBTJ_JPG_BC6H_UF16:
	case BGBBTJ_JPG_BC7:
	case BGBBTJ_JPG_BC7_SRGB:
		ret=0;
		if(BGBBTJ_BCn_CheckGlSupportBPTC())
			ret=1;
		break;
	default:
		ret=0;
		break;
	}
	return(ret);
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

#if 1
	if(BGBBTJ_CheckGlExtension("GL_ARB_texture_compression_bptc"))
	{
//		BGBBTJ_BC7_EncodeImageBC7(block, rgba, xs, ys, stride);
		BGBBTJ_BC7_EncodeImageBestBC7(block, rgba, xs, ys, stride);
		*rfmt=BGBBTJ_JPG_BC7;
		return;
	}
#endif

	if(!BGBBTJ_BCn_CheckEncodeImageAlpha(rgba, xs, ys, stride))
	{
#if 0
		BGBBTJ_BC6HS_EncodeImageUByte(block, rgba, xs, ys, stride);
		*rfmt=BGBBTJ_JPG_BC6;
		return;
#endif

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
	case BGBBTJ_JPG_BC1A:
		BGBBTJ_BCn_DecodeImageDXT1(block, rgba, xs, ys, stride);
		break;
	case BGBBTJ_JPG_BC3F:
		BGBBTJ_BCn_DecodeImageDXT5(block, rgba, xs, ys, stride);
		break;
	case BGBBTJ_JPG_BC3_UVAY:
		BGBBTJ_BCn_DecodeImageDXT5_UVAY(block, rgba, xs, ys, stride);
		break;

	case BGBBTJ_JPG_BC7:
		BGBBTJ_BC7_DecodeImageBC7(block, rgba, xs, ys, stride);
		break;
	}
}

void BGBBTJ_BCn_DecodeBlockDXT1(byte *block,
	byte *rgba, int xstride, int ystride, int nclr)
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
//		clr[12]=0;
//		clr[13]=0;
//		clr[14]=0;
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

void BGBBTJ_BCn_DecodeBlockDXT1_BGR(byte *block,
	byte *rgba, int xstride, int ystride, int nclr)
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
			clr[ 0]=block[1]&0xF8;
			clr[ 1]=((block[1]<<5)|(block[0]>>3))&0xFC;
			clr[ 2]=(block[0]<<3)&0xF8;
			clr[ 3]=(block[4]==0xFF)?0:255;
			clr[ 0]|=(clr[ 0]>>5);
			clr[ 1]|=(clr[ 1]>>6);
			clr[ 2]|=(clr[ 2]>>5);
		
			for(i=0; i<4; i++)
				for(j=0; j<4; j++)
			{
				k=i*ystride+j*xstride;
				rgba[k+0]=clr[2];
				rgba[k+1]=clr[1];
				rgba[k+2]=clr[0];
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

//		clr[ 8]=(clr[0]+clr[4])>>1;
//		clr[ 9]=(clr[1]+clr[5])>>1;
//		clr[10]=(clr[2]+clr[6])>>1;
		clr[11]=255;
//		clr[12]=0;
//		clr[13]=0;
//		clr[14]=0;
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
		rgba[k+0]=clr[l+2];
		rgba[k+1]=clr[l+1];
		rgba[k+2]=clr[l+0];
		rgba[k+3]=clr[l+3];
	}
}

void BGBBTJ_BCn_DecodeBlockDXT5(byte *block,
	byte *rgba, int xstride, int ystride, int nclr)
{
	byte clr[4*4];
	int i, j, k, l;

#if 1
	i=(block[0]^block[2])|(block[1]^block[3]);
	if(!i)
	{
		clr[ 0]=block[1]&0xF8;
		clr[ 1]=((block[1]<<5)|(block[0]>>3))&0xFC;
		clr[ 2]=(block[0]<<3)&0xF8;
		clr[ 3]=(block[4]==0xFF)?0:255;
		clr[ 0]|=(clr[ 0]>>5);
		clr[ 1]|=(clr[ 1]>>6);
		clr[ 2]|=(clr[ 2]>>5);
		
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
#endif

	clr[ 0]=block[1]&0xF8;
	clr[ 1]=((block[1]<<5)|(block[0]>>3))&0xFC;
	clr[ 2]=(block[0]<<3)&0xF8;
	clr[ 4]=block[3]&0xF8;
	clr[ 5]=((block[3]<<5)|(block[2]>>3))&0xFC;
	clr[ 6]=(block[2]<<3)&0xF8;	

	clr[ 0]|=(clr[ 0]>>5);
	clr[ 1]|=(clr[ 1]>>6);
	clr[ 2]|=(clr[ 2]>>5);
	clr[ 4]|=(clr[ 4]>>5);
	clr[ 5]|=(clr[ 5]>>6);
	clr[ 6]|=(clr[ 6]>>5);

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

void BGBBTJ_BCn_DecodeBlockDXT5_BGRA(byte *block,
	byte *rgba, int xstride, int ystride, int nclr)
{
	byte clr[4*4];
	int i, j, k, l;

#if 1
	i=(block[0]^block[2])|(block[1]^block[3]);
	if(!i)
	{
		clr[ 0]=block[1]&0xF8;
		clr[ 1]=((block[1]<<5)|(block[0]>>3))&0xFC;
		clr[ 2]=(block[0]<<3)&0xF8;
		clr[ 3]=(block[4]==0xFF)?0:255;
		clr[ 0]|=(clr[ 0]>>5);
		clr[ 1]|=(clr[ 1]>>6);
		clr[ 2]|=(clr[ 2]>>5);
		
		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
		{
			k=i*ystride+j*xstride;
			rgba[k+0]=clr[2];
			rgba[k+1]=clr[1];
			rgba[k+2]=clr[0];
			rgba[k+3]=clr[3];
		}
		return;
	}
#endif

	clr[ 0]=block[1]&0xF8;
	clr[ 1]=((block[1]<<5)|(block[0]>>3))&0xFC;
	clr[ 2]=(block[0]<<3)&0xF8;
	clr[ 4]=block[3]&0xF8;
	clr[ 5]=((block[3]<<5)|(block[2]>>3))&0xFC;
	clr[ 6]=(block[2]<<3)&0xF8;	

	clr[ 0]|=(clr[ 0]>>5);
	clr[ 1]|=(clr[ 1]>>6);
	clr[ 2]|=(clr[ 2]>>5);
	clr[ 4]|=(clr[ 4]>>5);
	clr[ 5]|=(clr[ 5]>>6);
	clr[ 6]|=(clr[ 6]>>5);

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
		rgba[k+0]=clr[l+2];
		rgba[k+1]=clr[l+1];
		rgba[k+2]=clr[l+0];
	}
}

void BGBBTJ_BCn_DecodeBlockBC4(byte *block,
	byte *rgba, int xstride, int ystride)
{
	byte val[8];
	int i, j, k, l;

	if(block[0]==block[1])
	{
//			k=(i*4+j)*3;
//			l=((block[2+(k>>3)]|(block[3+(k>>3)]<<8))>>(k&7))&7;
		l=block[0];

#if 1
		for(i=0; i<4; i++)
		{
			k=i*ystride;
			rgba[k+0*xstride]=l;
			rgba[k+1*xstride]=l;
			rgba[k+2*xstride]=l;
			rgba[k+3*xstride]=l;
		}
#endif

#if 0
		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
		{
			k=i*ystride+j*xstride;
			rgba[k]=l;
		}
#endif
		return;
	}
	
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

void BGBBTJ_BCn_DecodeBlockBC4B(byte *block,
	byte *rgba, int xstride, int ystride)
{
	byte val[8];
	int a0, a1;
	int i, j, k, l;

	a0=block[0];
	a1=block[1];
	if(a0==a1)
	{
		if(a0==255)
			return;

		for(i=0; i<4; i++)
		{
			k=i*ystride;
			rgba[k+0*xstride]=a0;
			rgba[k+1*xstride]=a0;
			rgba[k+2*xstride]=a0;
			rgba[k+3*xstride]=a0;
		}
		return;
	}
	
	if(a0>a1)
	{
		val[0]=a0;
		val[1]=a1;
		val[2]=(a0*219+a1* 37)>>8;
		val[3]=(a0*183+a1* 73)>>8;
		val[4]=(a0*146+a1*110)>>8;
		val[5]=(a0*110+a1*146)>>8;
		val[6]=(a0* 73+a1*183)>>8;
		val[7]=(a0* 37+a1*219)>>8;
	}else
	{
		val[0]=a0;
		val[1]=a1;
		val[2]=(a0*205+a1* 51)>>8;
		val[3]=(a0*154+a1*102)>>8;
		val[4]=(a0*102+a1*154)>>8;
		val[5]=(a0* 51+a1*205)>>8;
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

BGBBTJ_API void BGBBTJ_BCn_DecodeImageDXT1_BGR(byte *block,
	byte *rgba, int xs, int ys, int stride)
{
	int xs1, ys1;
	int i, j;
	
	xs1=xs>>2; ys1=ys>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		BGBBTJ_BCn_DecodeBlockDXT1_BGR(
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

BGBBTJ_API void BGBBTJ_BCn_DecodeImageDXT5_BGRA(byte *block,
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
		BGBBTJ_BCn_DecodeBlockDXT5_BGRA(
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

BGBBTJ_API void BGBBTJ_BCn_FlipImageDXT1(byte *iblk,
	byte *oblk, int xs, int ys, int stride)
{
	byte *cs, *ct;
	int xs1, ys1;
	int i, j, k, n;

	xs1=(xs+3)/4;
	ys1=(ys+3)/4;
	n=xs1*ys1;

	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		cs=iblk+((ys1-i-1)*xs1+j)*stride;
		ct=oblk+(i*xs1+j)*stride;
		ct[0]=cs[0];	ct[1]=cs[1];
		ct[2]=cs[2];	ct[3]=cs[3];
		ct[4]=cs[7];	ct[5]=cs[6];
		ct[6]=cs[5];	ct[7]=cs[4];
	}
}

BGBBTJ_API void BGBBTJ_BCn_FlipImageDXT5(byte *iblk,
	byte *oblk, int xs, int ys)
{
	byte *cs, *ct;
	int xs1, ys1;
	int i, j, k, l, n;

	xs1=(xs+3)/4;
	ys1=(ys+3)/4;
	n=xs1*ys1;

	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		cs=iblk+((ys1-i-1)*xs1+j)*16;
		ct=oblk+(i*xs1+j)*16;
		
		k=cs[2]|(cs[3]<<8)|(cs[4]<<16);
		l=cs[5]|(cs[6]<<8)|(cs[7]<<16);
		k=(k>>12)|((k&4095)<<12);
		l=(l>>12)|((l&4095)<<12);
		
		ct[ 0]=cs[ 0];			ct[ 1]=cs[ 1];
		ct[ 2]=l&255;			ct[ 3]=(l>>8)&255;
		ct[ 4]=(l>>16)&255;		ct[ 5]=k&255;
		ct[ 6]=(k>>8)&255;		ct[ 7]=(k>>16)&255;
		ct[ 8]=cs[ 8];			ct[ 9]=cs[ 9];
		ct[10]=cs[10];			ct[11]=cs[11];
		ct[12]=cs[15];			ct[13]=cs[14];
		ct[14]=cs[13];			ct[15]=cs[12];
	}
}

BGBBTJ_API void BGBBTJ_BCn_FlipImageDXT5_Mip(
	byte *iblk, byte *oblk, int xs, int ys)
{
	byte *ct, *ct1;
	byte *cs, *cs1;
	int xs1, ys1;
	int xs2, ys2;
	int xs3, ys3;
	
	xs1=xs; ys1=ys;

	xs2=xs1; ys2=ys1; cs=iblk; ct=oblk;
	while((xs2>0) || (ys2>0))
	{
		if(!xs2)xs2++;
		if(!ys2)ys2++;
		xs3=(xs2+3)/4;
		ys3=(ys2+3)/4;
//		n+=xs3*ys3;
		cs1=cs+(xs3*ys3)*16;
		ct1=ct+(xs3*ys3)*16;
//		BGBBTJ_Tex_HalfSampleDest(ct0, ct, xs2, ys2);
		BGBBTJ_BCn_FlipImageDXT5(cs, ct, xs2, ys2);
		xs2>>=1; ys2>>=1;
		cs=cs1; ct=ct1;
	}
}
