/*
MetaBlock 2
	AlphaBlock
	ColorBlock
	ExtColorBlock
	MetaBlock

AlphaBlock: BC4
ColorBlock: DXT1-like (normally)
ExtColorBlock: Pair of 32-bit extended colors.
MetaBlock: TDB

Used if stride==32.

ColorBlock:
(ColorA>ColorB)&&(ExtColorA||ExtColorB)
	Logical 8.8.8 or 10.10.10 colors.

Special Colors:
The bytes within a color-block may be used to encode special information.
	55 aa 55 bb: Index Color Block (256 color)
	55 xx 7z yy: 4-color block (RGB15)
		ExtColorBlock: 4x RGB15 (Color A/B/C/D)
		zyyxx: 4x 5-bits, max-min

	55 xx A8 yy: Motion Vector
		xx/yy: Delta
	55 xx A9 xx: RGB31F
		ExtColorBlock+Meta: 2x RGB64F
	55 xx AA xx: Undefined/Invalid Block
	55 xx AB pp: 4-color RGB24
		ExtColor+Meta: 4x RGB24,D
	55 xx AC yy: 4-color Interpolate
		ExtColor+Meta: 4x RGB24,D
		yy=Interpolate Mode
*/

#include <bgbbtj.h>

int bt1c_basedifftab5[16][32];		//diff5 -> delta
char bt1c_basedifftab5enc[16][512];	//8-bits -> diff5

void BGBBTJ_BT1CQ_DecodeBlockDXT1(
	BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *block,
	byte *rgba, int xstride, int ystride, int nclr)
{
	byte clr[4*4];
	int dr, dg, db;
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

#if 1
			if(ctx->eflags&BGBBTJ_BTIC1C_EFL_BLKMB1)
			{
//				k=block[ 8]|(block[ 9]<<8);
//				dr=(k>>11)&31; if(dr&0x10)dr|=(-1)<<5;
//				dg=(k>>5)&63; if(dg&0x20)dg|=(-1)<<6;
//				db=(k)&31; if(db&0x10)db|=(-1)<<5;
//				clr[0]+=dr;
//				clr[1]+=dg;
//				clr[2]+=db;

				if(block[8]|block[9]|block[10]|block[11]|
					block[12]|block[13]|block[14]|block[15])
				{
					clr[0]=block[ 8];
					clr[1]=block[ 9];
					clr[2]=block[10];
				}
			}
#endif

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

#if 1
	if(ctx->eflags&BGBBTJ_BTIC1C_EFL_BLKMB1)
	{
#if 0
		k=block[ 8]|(block[ 9]<<8);
		l=block[10]|(block[11]<<8);
		dr=(k>>11)&31; if(dr&0x10)dr|=(-1)<<5;
		dg=(k>>5)&63; if(dg&0x20)dg|=(-1)<<6;
		db=(k)&31; if(db&0x10)db|=(-1)<<5;
		clr[0]+=dr;
		clr[1]+=dg;
		clr[2]+=db;

		dr=(l>>11)&31; if(dr&0x10)dr|=(-1)<<5;
		dg=(l>>5)&63; if(dg&0x20)dg|=(-1)<<6;
		db=(l)&31; if(db&0x10)db|=(-1)<<5;
		clr[4]+=dr;
		clr[5]+=dg;
		clr[6]+=db;
#endif

		if(block[8]|block[9]|block[10]|block[11]|
			block[12]|block[13]|block[14]|block[15])
		{
			clr[0]=block[ 8];		clr[1]=block[ 9];
			clr[2]=block[10];		clr[4]=block[12];
			clr[5]=block[13];		clr[6]=block[14];
		}
	}
#endif

	if(i<=j)
	{
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

void BGBBTJ_BT1CQ_DecodeBlockDXT1_Pal24(
	BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *block,
	byte *rgba, int xstride, int ystride, u32 *pal)
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
			l=(i>>8)&255;
			clr[0]=(pal[l]>>16)&255;
			clr[1]=(pal[l]>>8)&255;
			clr[2]=(pal[l]>>0)&255;
			clr[3]=(block[4]==0xFF)?0:255;
			
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
		clr[ 0]=(pal[(i>>8)&255]>>16)&255;
		clr[ 1]=(pal[(i>>8)&255]>>8)&255;
		clr[ 2]=(pal[(i>>8)&255]>>0)&255;
		clr[ 3]=255;
		clr[ 4]=(pal[(j>>8)&255]>>16)&255;
		clr[ 5]=(pal[(j>>8)&255]>>8)&255;
		clr[ 6]=(pal[(j>>8)&255]>>0)&255;
		clr[ 7]=255;
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
		clr[ 0]=(pal[(i>>8)&255]>>16)&255;
		clr[ 1]=(pal[(i>>8)&255]>>8)&255;
		clr[ 2]=(pal[(i>>8)&255]>>0)&255;
		clr[ 3]=255;
		clr[ 4]=(pal[(j>>8)&255]>>16)&255;
		clr[ 5]=(pal[(j>>8)&255]>>8)&255;
		clr[ 6]=(pal[(j>>8)&255]>>0)&255;
		clr[ 7]=255;
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

int BGBBTJ_BT1CQ_DecodeBlock(
	BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *iblk, byte *orgba)
{
	int l0, l1, l2, l3;
	int p0, p1, p2, p3;
	int i, j, k;

//	BGBBTJ_BCn_DecodeBlockDXT1(
//		iblk, orgba, 4, 16, 4);

	if(ctx->pal_clr && (iblk[0]==0x55) && (iblk[2]==0x55))
	{
		BGBBTJ_BT1CQ_DecodeBlockDXT1_Pal24(
			ctx, iblk, orgba, 4, 16, ctx->pal_clr);
	}else
	{
		BGBBTJ_BT1CQ_DecodeBlockDXT1(
			ctx, iblk, orgba, 4, 16, 4);
	}
	
	for(i=0; i<3; i++)
		for(j=0; j<3; j++)
	{
		l0=(i*4+j+0)*4;	l1=(i*4+j+1)*4;
		l2=(i*4+j+4)*4;	l3=(i*4+j+5)*4;
//		p0=(32*orgba[l0+0])+(12*orgba[l1+0])+(12*orgba[l2+0])+(8*orgba[l3+0]);
//		p1=(32*orgba[l0+1])+(12*orgba[l1+1])+(12*orgba[l2+1])+(8*orgba[l3+1]);
//		p2=(32*orgba[l0+2])+(12*orgba[l1+2])+(12*orgba[l2+2])+(8*orgba[l3+2]);
//		p3=(32*orgba[l0+3])+(12*orgba[l1+3])+(12*orgba[l2+3])+(8*orgba[l3+3]);
		p0= (26*orgba[l0+0])+(14*orgba[l1+0])+
			(14*orgba[l2+0])+(10*orgba[l3+0]);
		p1= (26*orgba[l0+1])+(14*orgba[l1+1])+
			(14*orgba[l2+1])+(10*orgba[l3+1]);
		p2= (26*orgba[l0+2])+(14*orgba[l1+2])+
			(14*orgba[l2+2])+(10*orgba[l3+2]);
		p3= (26*orgba[l0+3])+(14*orgba[l1+3])+
			(14*orgba[l2+3])+(10*orgba[l3+3]);
		orgba[l0+0]=p0>>6;
		orgba[l0+1]=p1>>6;
		orgba[l0+2]=p2>>6;
		orgba[l0+3]=p3>>6;
	}
	
	return(0);
}

bool BGBBTJ_BT1C_CheckBlockColor_IsRGB23(
	BGBBTJ_BTIC1C_ImageInfo *ctx, byte *blk)
{
	if(!(ctx->eflags&BGBBTJ_BTIC1C_EFL_EXTMODES))
		return(false);
	if(!(ctx->eflags&BGBBTJ_BTIC1C_EFL_BLKMB1))
		return(false);
	
	if((blk[0]==0x55) || (blk[0]==0xAA))
		return(false);

//	if((!blk[8]) && (!blk[9]) && (!blk[10]) && (!blk[11]))
//		return(false);
//	if( (!blk[ 8]) && (!blk[ 9]) && (!blk[10]) && (!blk[11]) &&
//		(!blk[12]) && (!blk[13]) && (!blk[14]) && (!blk[15]))
	if(!(blk[ 8]|blk[ 9]|blk[10]|blk[11]|
		 blk[12]|blk[13]|blk[14]|blk[15]))
		return(false);

//	return(false);

	return(true);
}

int BGBBTJ_BT1C_GetBlockColorA_AsRGB24(
	BGBBTJ_BTIC1C_ImageInfo *ctx, byte *blk)
{
	int cr, cg, cb;
	int dr, dg, db;
	int i0, i1, i2;

#if 0
	i0=blk[0]|(blk[1]<<8);
	i1=blk[8]|(blk[9]<<8);
	cr=(i0&0xF800)>>8;
	cg=(i0&0x07E0)>>3;
	cb=(i0&0x001F)<<3;
	cr=cr|(cr>>5);
	cg=cg|(cg>>6);
	cb=cb|(cb>>5);

	dr=(i1&0xF800)>>11;
	dg=(i1&0x07E0)>>5;
	db=(i1&0x001F)<<0;
	
	cr+=dr;
	cg+=dg;
	cb+=db;

	cr=(cr<0)?0:((cr>255)?255:cr);
	cg=(cg<0)?0:((cg>255)?255:cg);
	cb=(cb<0)?0:((cb>255)?255:cb);
#endif

	cr=blk[ 8];	cg=blk[ 9];	cb=blk[10];

	i2=(cr<<16)|(cg<<8)|(cb);
	return(i2);
}

int BGBBTJ_BT1C_GetBlockColorB_AsRGB24(
	BGBBTJ_BTIC1C_ImageInfo *ctx, byte *blk)
{
	int cr, cg, cb;
	int dr, dg, db;
	int i0, i1, i2;

#if 0	
	i0=blk[ 2]|(blk[ 3]<<8);
	i1=blk[10]|(blk[11]<<8);
	cr=(i0&0xF800)>>8;
	cg=(i0&0x07E0)>>3;
	cb=(i0&0x001F)<<3;
	cr=cr|(cr>>5);
	cg=cg|(cg>>6);
	cb=cb|(cb>>5);

	dr=(i1&0xF800)>>11;
	dg=(i1&0x07E0)>>5;
	db=(i1&0x001F)<<0;
	
	cr+=dr;
	cg+=dg;
	cb+=db;

	cr=(cr<0)?0:((cr>255)?255:cr);
	cg=(cg<0)?0:((cg>255)?255:cg);
	cb=(cb<0)?0:((cb>255)?255:cb);
#endif

	cr=blk[12];	cg=blk[13];	cb=blk[14];

	i2=(cr<<16)|(cg<<8)|(cb);
	return(i2);
}

int BGBBTJ_BT1C_GetBlockColorC_AsRGB24(
	BGBBTJ_BTIC1C_ImageInfo *ctx, byte *blk)
{
	int cr, cg, cb;
//	int dr, dg, db;
	int i0, i1, i2;

	cr=blk[16];	cg=blk[17];	cb=blk[18];
	i2=(cr<<16)|(cg<<8)|(cb);
	return(i2);
}

int BGBBTJ_BT1C_GetBlockColorD_AsRGB24(
	BGBBTJ_BTIC1C_ImageInfo *ctx, byte *blk)
{
	int cr, cg, cb;
//	int dr, dg, db;
	int i0, i1, i2;

	cr=blk[20];	cg=blk[21];	cb=blk[22];
	i2=(cr<<16)|(cg<<8)|(cb);
	return(i2);
}

int BGBBTJ_BT1C_GetBlockColorA_AsRGB23(
	BGBBTJ_BTIC1C_ImageInfo *ctx, byte *blk)
{
	int i, j, k, l;
	
	i=BGBBTJ_BT1C_GetBlockColorA_AsRGB24(ctx, blk);
	j=	((i&0x0000FE)>>1)|
		((i&0x00FF00)>>1)|
		((i&0xFE0000)>>2)|
		(((i&1)&((i>>16)&1))<<22);
	return(j);
}

int BGBBTJ_BT1C_GetBlockColorB_AsRGB23(
	BGBBTJ_BTIC1C_ImageInfo *ctx, byte *blk)
{
	int i, j, k, l;
	
	i=BGBBTJ_BT1C_GetBlockColorB_AsRGB24(ctx, blk);
	j=	((i&0x0000FE)>>1)|
		((i&0x00FF00)>>1)|
		((i&0xFE0000)>>2)|
		(((i&1)&((i>>16)&1))<<22);
	return(j);
}

int BGBBTJ_BT1C_GetBlockColorC_AsRGB23(
	BGBBTJ_BTIC1C_ImageInfo *ctx, byte *blk)
{
	int i, j, k, l;
	
	i=BGBBTJ_BT1C_GetBlockColorC_AsRGB24(ctx, blk);
	j=	((i&0x0000FE)>>1)|
		((i&0x00FF00)>>1)|
		((i&0xFE0000)>>2)|
		(((i&1)&((i>>16)&1))<<22);
	return(j);
}

int BGBBTJ_BT1C_GetBlockColorD_AsRGB23(
	BGBBTJ_BTIC1C_ImageInfo *ctx, byte *blk)
{
	int i, j, k, l;
	
	i=BGBBTJ_BT1C_GetBlockColorD_AsRGB24(ctx, blk);
	j=	((i&0x0000FE)>>1)|
		((i&0x00FF00)>>1)|
		((i&0xFE0000)>>2)|
		(((i&1)&((i>>16)&1))<<22);
	return(j);
}

void BGBBTJ_BT1C_GetBlockColorsAB_AsRGB23(
	BGBBTJ_BTIC1C_ImageInfo *ctx, byte *blk,
	int *rclra, int *rclrb)
{
	int i0, i1, i2, i3;

	if(	(ctx->eflags&BGBBTJ_BTIC1C_EFL_EXTMODES) &&
		(ctx->eflags&BGBBTJ_BTIC1C_EFL_BLKMB1) &&
//		!((blk[0]==0x55) || (blk[0]==0xAA)) &&
		(blk[ 8]|blk[ 9]|blk[10]|blk[11]|
		 blk[12]|blk[13]|blk[14]|blk[15]))
	{
		*rclra=BGBBTJ_BT1C_GetBlockColorA_AsRGB23(ctx, blk);
		*rclrb=BGBBTJ_BT1C_GetBlockColorB_AsRGB23(ctx, blk);
		return;
	}
	
	i0=blk[0]|(blk[1]<<8);
	i1=blk[2]|(blk[3]<<8);
	i2=BGBBTJ_BT1C_ConvRGB16ToRGB24(ctx, i0);
	i3=BGBBTJ_BT1C_ConvRGB16ToRGB24(ctx, i1);
	*rclra=BGBBTJ_BT1C_ConvRGB24ToRGB23(ctx, i2);
	*rclrb=BGBBTJ_BT1C_ConvRGB24ToRGB23(ctx, i3);
}

void BGBBTJ_BT1C_GetBlockColorsExtAB(
	BGBBTJ_BTIC1C_ImageInfo *ctx, byte *blk,
	int *rclra, int *rclrb)
{
	if(	(ctx->eflags&BGBBTJ_BTIC1C_EFL_EXTMODES) &&
		(ctx->eflags&BGBBTJ_BTIC1C_EFL_BLKMB1))
	{
#if defined(X86)||defined(X86_64)
		*rclra=*(int *)(blk+ 8);
		*rclrb=*(int *)(blk+12);
#else
		*rclra=(blk[ 8]<<16)|(blk[ 9]<<8)|(blk[10])|(blk[11]<<24);
		*rclrb=(blk[12]<<16)|(blk[13]<<8)|(blk[14])|(blk[15]<<24);
#endif
		return;
	}
	*rclra=0;
	*rclrb=0;
}


int BGBBTJ_BT1C_GetColorRGB24AsDiff15(
	BGBBTJ_BTIC1C_ImageInfo *ctx, int clr, int lclr, int sc)
{
	int i, j, k, l;
	int dr, dg, db;
	int dr2, dg2, db2;
	
	dr=((clr>>16)&255) - ((lclr>>16)&255);
	dg=((clr>> 8)&255) - ((lclr>> 8)&255);
	db=((clr>> 0)&255) - ((lclr>> 0)&255);
	
	dr2=bt1c_basedifftab5enc[sc][dr+256];
	dg2=bt1c_basedifftab5enc[sc][dg+256];
	db2=bt1c_basedifftab5enc[sc][db+256];
	
	if((dr2<0) || (dg2<0) || (db2<0))
		return(-2);
	
	j=(dr2<<10)|(dg2<<5)|db2;
	return(j);
	
//	j=	((i&0x0000FE)>>1)|
//		((i&0x00FF00)>>1)|
//		((i&0xFE0000)>>2)|
//		(((i&1)&((i>>16)&1))<<22);
//	return(j);
}

int BGBBTJ_BT1C_GetColorA_RGB23AsDiff15(
	BGBBTJ_BTIC1C_ImageInfo *ctx, byte *blk)
{
	int i, j, k, l;

	if(!(ctx->eflags&BGBBTJ_BTIC1C_EFL_EXTMODES) ||
		!(ctx->eflags&BGBBTJ_BTIC1C_EFL_DIFFCLR))
			return(-1);

	if(ctx->diff_sc<0)
		return(-1);
//	if(ctx->diff_lclra<0)
//		return(-1);
	
	i=BGBBTJ_BT1C_GetBlockColorA_AsRGB24(ctx, blk);
	j=BGBBTJ_BT1C_GetColorRGB24AsDiff15(ctx,
		i, ctx->diff_lclra, ctx->diff_sc);
	return(j);
}

int BGBBTJ_BT1C_GetColorB_RGB23AsDiff15(
	BGBBTJ_BTIC1C_ImageInfo *ctx, byte *blk)
{
	int i, j, k, l;

	if(!(ctx->eflags&BGBBTJ_BTIC1C_EFL_EXTMODES) ||
		!(ctx->eflags&BGBBTJ_BTIC1C_EFL_DIFFCLR))
			return(-1);

	if(ctx->diff_sc<0)
		return(-1);
//	if(ctx->diff_lclrb<0)
//		return(-1);
	
	i=BGBBTJ_BT1C_GetBlockColorB_AsRGB24(ctx, blk);
	j=BGBBTJ_BT1C_GetColorRGB24AsDiff15(ctx,
		i, ctx->diff_lclrb, ctx->diff_sc);
	return(j);
}
