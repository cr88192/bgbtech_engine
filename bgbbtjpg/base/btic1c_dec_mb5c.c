#include <bgbbtj.h>

extern byte bt1c_blkmap1[256];			//RPZA -> DXT1 Pixels (A, B)
extern byte bt1c_blkmap2[256];			//RPZA -> DXT1 Pixels (B, A)
extern byte bt1c_blkmap3[256];			//DXT1 -> RPZA Pixels (A, B)
extern byte bt1c_blkmap4[256];			//DXT1 -> RPZA Pixels (B, A)

extern u16 bt1c_blkalphamap1[256];		//DXT1 -> BC4 Pixels (Bit Alpha)
extern u16 bt1c_blkalphamap2[256];		//DXT1 -> BC4 Pixels (A, B)
extern u16 bt1c_blkalphamap3[256];		//RPZA -> BC4 Pixels (A, B)
extern u16 bt1c_blkalphamap4[256];		//RPZA -> BC4 Pixels (B, A)

extern byte bt1c_blkalphap[256];		//RPZA pixels (Bit Alpha) has alpha
extern byte bt1c_blkalphamape1[4096];	//BC4 -> RPZA Pixels (A, B)
extern byte bt1c_blkalphamape2[4096];	//BC4 -> RPZA Pixels (B, A)

extern u16 bt1c_blkidxcolor[256];		//default block index colors
extern u16 bt1c_blkidxmap1[256];		//1BPP -> RPZA Pixels
extern u32 bt1c_blkidxmap2[256];		//2x2 2BPP -> RPZA Pixels

extern u16 bt1c_difftab[16*256];		//differential color tables
extern u32 bt1c_difftab24[16*256];		//diff7 on RGB24

extern byte bt1c_hf12tob[4096];		//half-float (bits 4-15) to byte

extern int bt1c_basedifftab[8][5];
extern u32 bt1c_blkidx_pixpat[256];	//1BPP -> RPZA Pixels


void BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasicAX(
	byte *ct, u16 clra, u16 clrb, u32 pix, int dfl)
{
	int ca, cb, ca2, cb2;
	byte *csm;

	ca=clra; cb=clrb;
	ca2=BGBBTJ_BT1C_ConvRGB16ToRGB24(NULL, clra);
	cb2=BGBBTJ_BT1C_ConvRGB16ToRGB24(NULL, clrb);
	ca2=BGBBTJ_BT1C_SwapRGB24_RB(ca2);
	cb2=BGBBTJ_BT1C_SwapRGB24_RB(cb2);

//	ca=ca|0x0020;
//	cb=cb|0x0020;
//	ca=ca&0xFFDF;
//	cb=cb&0xFFDF;

	if(clra>clrb)
	{
		bgbbtj_bt1c_setule16(ct+ 8, ca);
		bgbbtj_bt1c_setule16(ct+10, cb);
		bgbbtj_bt1c_setule32(ct+16, ca2);
		bgbbtj_bt1c_setule32(ct+20, cb2);
		csm=bt1c_blkmap1;
	}else
	{
		bgbbtj_bt1c_setule16(ct+ 8, cb);
		bgbbtj_bt1c_setule16(ct+10, ca);
		bgbbtj_bt1c_setule32(ct+16, cb2);
		bgbbtj_bt1c_setule32(ct+20, ca2);
		csm=bt1c_blkmap2;
	}
	ct[12]=csm[(pix>>24)&255]; ct[13]=csm[(pix>>16)&255];
	ct[14]=csm[(pix>> 8)&255]; ct[15]=csm[(pix    )&255];
	
//	if((ct[8]==0x55) || (ct[8]==0xAA))
	if(ct[8]==0x55)
		{ ct[8]^=0x20; }
}

void BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic(
	byte *ct, u16 clra, u16 clrb, u32 pix, int dfl)
{
	if(!(dfl&BGBBTJ_BTIC1C_DFL_HASAX))
		{ ct[0]=255; ct[1]=255; }
	BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasicAX(
		ct, clra, clrb, pix, dfl);
}

void BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasicAlpha(
	byte *ct, u16 clra, u16 clrb, u32 pix, int dfl)
{
	if(!(dfl&BGBBTJ_BTIC1C_DFL_HASAX))
		{ ct[0]=255; ct[1]=255; }
	BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasicAX(
		ct, clra, clrb, pix, dfl);
}

void BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasicAX_RGB24(
	byte *ct, u32 clra, u32 clrb, u32 pix, int dfl)
{
	int ca, cb, ca2, cb2;
	byte *csm;

	ca=BGBBTJ_BT1C_ConvRGB24ToRGB16(NULL, clra);
	cb=BGBBTJ_BT1C_ConvRGB24ToRGB16(NULL, clrb);
	ca2=BGBBTJ_BT1C_SwapRGB24_RB(clra);
	cb2=BGBBTJ_BT1C_SwapRGB24_RB(clrb);

//	ca=ca|0x0020;
//	cb=cb|0x0020;
//	ca=ca&0xFFDF;
//	cb=cb&0xFFDF;

	if(ca>cb)
	{
		bgbbtj_bt1c_setule16(ct+ 8, ca);
		bgbbtj_bt1c_setule16(ct+10, cb);
		bgbbtj_bt1c_setule32(ct+16, ca2);
		bgbbtj_bt1c_setule32(ct+20, cb2);
		csm=bt1c_blkmap1;
	}else
	{
		bgbbtj_bt1c_setule16(ct+ 8, cb);
		bgbbtj_bt1c_setule16(ct+10, ca);
		bgbbtj_bt1c_setule32(ct+16, cb2);
		bgbbtj_bt1c_setule32(ct+20, ca2);
		csm=bt1c_blkmap2;
	}
	ct[12]=csm[(pix>>24)&255]; ct[13]=csm[(pix>>16)&255];
	ct[14]=csm[(pix>> 8)&255]; ct[15]=csm[(pix    )&255];

//	if((ct[8]==0x55) || (ct[8]==0xAA))
	if(ct[8]==0x55)
		{ ct[8]^=0x20; }
}

void BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic_RGB24(
	byte *ct, u32 clra, u32 clrb, u32 pix, int dfl)
{
	if(!(dfl&BGBBTJ_BTIC1C_DFL_HASAX))
		{ ct[0]=255; ct[1]=255; }
	BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasicAX_RGB24(
		ct, clra, clrb, pix, dfl);
}

void BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasicAlpha_RGB24(
	byte *ct, u32 clra, u32 clrb, u32 pix, int dfl)
{
	if(!(dfl&BGBBTJ_BTIC1C_DFL_HASAX))
		{ ct[0]=255; ct[1]=255; }
	BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasicAX_RGB24(
		ct, clra, clrb, pix, dfl);
}


void BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasicAX_RGB48(
	byte *ct, u64 clra, u64 clrb, u32 pix, int dfl)
{
	int ca, cb, ca2, cb2;
	byte *csm;

//	ca=BGBBTJ_BT1C_ConvRGB24ToRGB16(NULL, clra);
//	cb=BGBBTJ_BT1C_ConvRGB24ToRGB16(NULL, clrb);
//	ca2=BGBBTJ_BT1C_SwapRGB24_RB(clra);
//	cb2=BGBBTJ_BT1C_SwapRGB24_RB(clrb);
//	if(ca>cb)
//	{
//		bgbbtj_bt1c_setule16(ct+ 8, ca);
//		bgbbtj_bt1c_setule16(ct+10, cb);
//		bgbbtj_bt1c_setule32(ct+16, ca2);
//		bgbbtj_bt1c_setule32(ct+20, cb2);
//		csm=bt1c_blkmap1;
//	}else
//	{
//		bgbbtj_bt1c_setule16(ct+ 8, cb);
//		bgbbtj_bt1c_setule16(ct+10, ca);
//		bgbbtj_bt1c_setule32(ct+16, cb2);
//		bgbbtj_bt1c_setule32(ct+20, ca2);
//		csm=bt1c_blkmap2;
//	}

	csm=bt1c_blkmap1;

	ct[ 8]=0x55;	ct[ 9]=0x00;
	ct[10]=0xA9;	ct[11]=0x00;

	bgbbtj_bt1c_setule16(ct+16, (clra>>32)&65535);
	bgbbtj_bt1c_setule16(ct+18, (clra>>16)&65535);
	bgbbtj_bt1c_setule16(ct+20, (clra    )&65535);

	bgbbtj_bt1c_setule16(ct+24, (clrb>>32)&65535);
	bgbbtj_bt1c_setule16(ct+26, (clrb>>16)&65535);
	bgbbtj_bt1c_setule16(ct+28, (clrb    )&65535);

	ct[12]=csm[(pix>>24)&255]; ct[13]=csm[(pix>>16)&255];
	ct[14]=csm[(pix>> 8)&255]; ct[15]=csm[(pix    )&255];
}

void BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic_RGB48(
	byte *ct, u64 clra, u64 clrb, u32 pix, int dfl)
{
	if(!(dfl&BGBBTJ_BTIC1C_DFL_HASAX))
		{ ct[0]=255; ct[1]=255; }
	BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasicAX_RGB48(
		ct, clra, clrb, pix, dfl);
}


void BGBBTJ_BT1C_FillColorDeblockMB5C(
	BGBBTJ_BTIC1C_ImageInfo *ctx, 
	byte *blks, byte *ct, u16 clr, int idx,
	int xs1, int ys1, int stride, int mode, int dfl)
{
	BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic(ct, clr, clr, 0, dfl);
}

void BGBBTJ_BT1C_DecodeBlockColorArrayMB5C(
	BGBBTJ_BTIC1C_ImageInfo *ctx, 
	byte *blks, byte *ct, byte *csa, byte **rcs, int num,
	int xs1, int ys1, int stride, int mode, int dfl)
{
	byte *cs;
	int i, j, k;
	
	cs=*rcs;
	for(i=0; i<num; i++)
	{
		j=BGBBTJ_BT1C_DecodeSingleColor565(ctx, &cs,
			mode, BGBBTJ_BTIC1C_EFL_DIFFCLR);
		BGBBTJ_BT1C_FillColorDeblockMB5C(ctx,
			blks, ct, j, 0, xs1, ys1, stride,
			ctx->clrdb_mode, dfl);
		ct+=stride;
	}
	*rcs=cs;
}

void BGBBTJ_BT1C_DecodeRawBlockMB5C(
	BGBBTJ_BTIC1C_ImageInfo *ctx, 
	byte *blks, byte *ct, byte *csa, byte **rcs, int num,
	int xs1, int ys1, int stride, int mode, int dfl)
{
	u32 aclr[16], bclr[4];
	byte pixt[16*4], bpixt[4*4];
	byte *cs, *csm;
	int blt;
	int i, j, k;
	
	cs=*rcs;
//	memset(ct, 0, 32);
	memset(ct+8, 0, 24);
	switch(mode)
	{
	case 0: case 4:
	case 5: case 14:
		if(!(cs[2]&0x80) && (cs[4]&0x80))
		{
			j=(cs[0]<<8)|(cs[1]);
			bclr[0]=BGBBTJ_BT1C_ConvRGB15ToRGB24(ctx, j);
			j=(cs[2]<<8)|(cs[3]);
			bclr[1]=BGBBTJ_BT1C_ConvRGB15ToRGB24(ctx, j);
			j=(cs[4]<<8)|(cs[5]);
			bclr[2]=BGBBTJ_BT1C_ConvRGB15ToRGB24(ctx, j);
			j=(cs[6]<<8)|(cs[7]);
			bclr[3]=BGBBTJ_BT1C_ConvRGB15ToRGB24(ctx, j);
			cs+=8;
			blt=1;
			break;
		}
		
		for(i=0; i<16; i++)
		{
			j=(cs[0]<<8)|(cs[1]);
			k=BGBBTJ_BT1C_ConvRGB15ToRGB24(ctx, j);
			aclr[i]=k;
			cs+=2;
		}
		blt=0;
//		cs+=32;
		break;
	case 6:		case 10:
	case 11:	case 12:
	case 13:
		if(!(cs[3]&0x80) && (cs[6]&0x80))
		{
			j=(cs[0]<<16)|(cs[1]<<8)|(cs[2]);
			bclr[0]=BGBBTJ_BT1C_ConvRGB23ToRGB24(ctx, j);
			j=(cs[3]<<16)|(cs[4]<<8)|(cs[5]);
			bclr[1]=BGBBTJ_BT1C_ConvRGB23ToRGB24(ctx, j);
			j=(cs[6]<<16)|(cs[7]<<8)|(cs[8]);
			bclr[2]=BGBBTJ_BT1C_ConvRGB23ToRGB24(ctx, j);
			j=(cs[9]<<16)|(cs[10]<<8)|(cs[11]);
			bclr[3]=BGBBTJ_BT1C_ConvRGB23ToRGB24(ctx, j);
			cs+=12;
			blt=1;
			break;
		}
		
		for(i=0; i<16; i++)
		{
			j=(cs[0]<<16)|(cs[1]<<8)|(cs[2]);
			k=BGBBTJ_BT1C_ConvRGB23ToRGB24(ctx, j);
			aclr[i]=k;
			cs+=3;
		}
		blt=0;
//		cs+=48;
		break;

	case 7:
		if(!(cs[4]&0x80) && (cs[8]&0x80))
		{
			j=(cs[ 0]<<24)|(cs[ 1]<<16)|(cs[ 2]<<8)|(cs[ 3]);
			bclr[0]=BGBBTJ_BT1C_ConvRGB31FSToRGB24(ctx, j);
			j=(cs[ 4]<<24)|(cs[ 5]<<16)|(cs[ 6]<<8)|(cs[ 7]);
			bclr[1]=BGBBTJ_BT1C_ConvRGB31FSToRGB24(ctx, j);
			j=(cs[ 8]<<24)|(cs[ 9]<<16)|(cs[10]<<8)|(cs[11]);
			bclr[2]=BGBBTJ_BT1C_ConvRGB31FSToRGB24(ctx, j);
			j=(cs[12]<<24)|(cs[13]<<16)|(cs[14]<<8)|(cs[15]);
			bclr[3]=BGBBTJ_BT1C_ConvRGB31FSToRGB24(ctx, j);
			cs+=16;
			blt=1;
			break;
		}
		
		for(i=0; i<16; i++)
		{
			j=(cs[0]<<24)|(cs[1]<<16)|(cs[2]<<8)|(cs[3]);
			k=BGBBTJ_BT1C_ConvRGB31FSToRGB24(ctx, j);
			aclr[i]=k;
			cs+=4;
		}
		blt=0;
//		cs+=48;
		break;
	case 8:
		if(!(cs[4]&0x80) && (cs[8]&0x80))
		{
			j=(cs[ 0]<<24)|(cs[ 1]<<16)|(cs[ 2]<<8)|(cs[ 3]);
			bclr[0]=BGBBTJ_BT1C_ConvRGB31FUToRGB24(ctx, j);
			j=(cs[ 4]<<24)|(cs[ 5]<<16)|(cs[ 6]<<8)|(cs[ 7]);
			bclr[1]=BGBBTJ_BT1C_ConvRGB31FUToRGB24(ctx, j);
			j=(cs[ 8]<<24)|(cs[ 9]<<16)|(cs[10]<<8)|(cs[11]);
			bclr[2]=BGBBTJ_BT1C_ConvRGB31FUToRGB24(ctx, j);
			j=(cs[12]<<24)|(cs[13]<<16)|(cs[14]<<8)|(cs[15]);
			bclr[3]=BGBBTJ_BT1C_ConvRGB31FUToRGB24(ctx, j);
			cs+=16;
			blt=1;
			break;
		}
		
		for(i=0; i<16; i++)
		{
			j=(cs[0]<<24)|(cs[1]<<16)|(cs[2]<<8)|(cs[3]);
			k=BGBBTJ_BT1C_ConvRGB31FUToRGB24(ctx, j);
			aclr[i]=k;
			cs+=4;
		}
		blt=0;
//		cs+=48;
		break;

	default:
		blt=0;
		break;
	}

	if(!blt)
	{
#if defined(X86) || defined(X86_64)
		BGBBTJ_BCn_EncodeBlockMB1CF_BGRA(ct+8, (byte *)(&aclr), 4, 16,
			(byte *)(&aclr), ((byte *)(&aclr))+16*4);
#else
		for(i=0; i<16; i++)
		{
			pixt[i*4+0]=(aclr[i]>>16)&255;
			pixt[i*4+1]=(aclr[i]>>8)&255;
			pixt[i*4+2]=(aclr[i]>>0)&255;
			pixt[i*4+3]=(aclr[i]>>24)&255;
		}
		BGBBTJ_BCn_EncodeBlockMB1C(ct+8, pixt, 4, 16,
			pixt, pixt+16*4);
//		BGBBTJ_BCn_EncodeBlockMB1CF2(ct+8, pixt, 4, 16,
//			pixt, pixt+16*4);
#endif
		*rcs=cs;
		return;
	}

	for(i=0; i<4; i++)
	{
		bpixt[i*4+0]=(bclr[i]>>16)&255;
		bpixt[i*4+1]=(bclr[i]>>8)&255;
		bpixt[i*4+2]=(bclr[i]>>0)&255;
		bpixt[i*4+3]=(bclr[i]>>24)&255;
	}

	i=*cs;
	switch((i>>5)&7)
	{
	case 4:
		ct[ 8]=0x55;	ct[ 9]=0x00;
		ct[10]=0xAC;	ct[11]=i&31;
		ct[16]=bpixt[ 0];	ct[17]=bpixt[ 1];
		ct[18]=bpixt[ 2];	ct[19]=bpixt[ 3];
		ct[20]=bpixt[ 4];	ct[21]=bpixt[ 5];
		ct[22]=bpixt[ 6];	ct[23]=bpixt[ 7];
		ct[24]=bpixt[ 8];	ct[25]=bpixt[ 9];
		ct[26]=bpixt[10];	ct[27]=bpixt[11];
		ct[28]=bpixt[12];	ct[29]=bpixt[13];
		ct[30]=bpixt[14];	ct[31]=bpixt[15];
		break;
	case 5:
		ct[ 8]=0x55;	ct[ 9]=0x00;
		ct[10]=0xAB;	ct[11]=i&31;
		ct[16]=bpixt[ 0];	ct[17]=bpixt[ 1];
		ct[18]=bpixt[ 2];	// ct[19]=bpixt[ 3];
		ct[20]=bpixt[ 4];	ct[21]=bpixt[ 5];
		ct[22]=bpixt[ 6];	// ct[23]=bpixt[ 7];
		ct[24]=bpixt[ 8];	ct[25]=bpixt[ 9];
		ct[26]=bpixt[10];	// ct[27]=bpixt[11];
		ct[28]=bpixt[12];	ct[29]=bpixt[13];
		ct[30]=bpixt[14];	// ct[31]=bpixt[15];
		
		i=(cs[0]<<16)|(cs[1]<<8)|(cs[2]);
		cs+=3;
		ct[19]=((i>>15)&31)<<3;
		ct[23]=((i>>10)&31)<<3;
		ct[25]=((i>>5)&31)<<3;
		ct[29]=((i>>0)&31)<<3;

		csm=bt1c_blkmap1;
		ct[12]=csm[cs[0]]; ct[13]=csm[cs[1]];
		ct[14]=csm[cs[2]]; ct[15]=csm[cs[3]];
		cs+=4;
		break;
	case 6:
		ct[ 8]=0x55;	ct[ 9]=0x00;
		ct[10]=0xAB;	ct[11]=i&31;
		ct[16]=bpixt[ 0];	ct[17]=bpixt[ 1];
		ct[18]=bpixt[ 2];	// ct[19]=bpixt[ 3];
		ct[20]=bpixt[ 4];	ct[21]=bpixt[ 5];
		ct[22]=bpixt[ 6];	// ct[23]=bpixt[ 7];
		ct[24]=bpixt[ 8];	ct[25]=bpixt[ 9];
		ct[26]=bpixt[10];	// ct[27]=bpixt[11];
		ct[28]=bpixt[12];	ct[29]=bpixt[13];
		ct[30]=bpixt[14];	// ct[31]=bpixt[15];

		switch(i&31)
		{
		case 0:
			ct[19]=cs[1];	ct[23]=cs[2];
			ct[27]=cs[3];	ct[31]=cs[4];
			cs+=5;
			break;
		case 1:
		case 2:
//			ct[11]=cs[2];	ct[15]=cs[3];
//			ct[19]=cs[4];	ct[23]=cs[5];
			ct[19]=cs[1];	ct[23]=cs[2];
			ct[27]=cs[3];	ct[31]=cs[4];
			ct[ 9]=cs[5];
			cs+=6;
			break;
		case 16:
		case 17:
			break;
		case 19:
			ct[9]=cs[1];
			cs+=2;
			break;
		}

		csm=bt1c_blkmap1;
		ct[12]=csm[cs[0]]; ct[13]=csm[cs[1]];
		ct[14]=csm[cs[2]]; ct[15]=csm[cs[3]];
		cs+=4;
		
		break;
	}

	*rcs=cs;
}

BGBBTJ_API int BGBBTJ_BT1C_DecodeBlocksMB5C_I(
	BGBBTJ_BTIC1C_ImageInfo *ctx, byte *ibuf, int isz,
	byte *blks, byte *lblks, byte *ablks,
	int xs, int ys, int dfl)
{
	byte tb[32];
	byte *cs, *cse, *csm, *csl, *csa;
	byte *ct, *cte, *ctb;
	u64 clraw, clrbw;
	int diffm, diffsc;
	int xs1, ys1, n, n1, ret, mode, nextmode;
	int xo, yo, clra, clrb;
	int i, j, k, l;
	
	BGBBTJ_BT1C_Init();
	
	xs1=(xs+3)/4;
	ys1=(ys+3)/4;
	n=xs1*ys1;
	
	ctb=tb;
	cs=ibuf; cse=cs+isz;
	ct=blks; cte=ct+32*n;
	csl=lblks; csa=ablks;
	if(!csl)csl=blks;
	if(!csa)csa=blks;
//	cs+=4;

	if(ibuf[0]==0xE1)
	{
		l=(cs[1]<<16)|(cs[2]<<8)|cs[3];
		cs=ibuf+4; cse=ibuf+l;
	}

	dfl|=ctx->dflags&BGBBTJ_BTIC1C_DFL_FRAMESTICKY;
	ctx->dflags=dfl;

	diffm=0; diffsc=0;
	ctx->diff_m=0; ctx->diff_sc=0;
//	diffm=ctx->diff_m;
//	diffsc=ctx->diff_sc;

	ctx->diff_lclra=0;
	ctx->diff_lclrb=0;
	ctx->diff_lclrc=0;
	ctx->diff_lclrd=0;

//	ret=0; mode=0; nextmode=0;
	ret=0; mode=ctx->modes[0]; nextmode=mode;
	while((cs<cse) && (ct<cte) && !ret)
	{
		i=*cs++;
		switch(i&0xE0)
		{
		case 0x80:
			if(ct!=csl)
			{
				j=(i&31)+1;
				while(j--)
				{
					bgbbtj_bt1c_memcpy32(ct, csl);
					ct+=32; csl+=32; csa+=8;
				}
			}else
			{
				n1=(i&31)+1;
				ct+=n1*32; csl+=n1*32; csa+=n1*8;
			}
			break;			
		case 0xE0:
			switch(i)
			{
			case 0xE0: case 0xE1: case 0xE2: case 0xE3:
			case 0xE4: case 0xE5: case 0xE6: case 0xE7:
			case 0xE8: case 0xE9: case 0xEA: case 0xEB:
			case 0xEC:
			case 0xEF:

			case 0xF0: case 0xF1: case 0xF2: case 0xF3:
			case 0xF4: case 0xF5: case 0xF6: case 0xF7:
				ctx->curmode=mode;
				ctx->nextmode=nextmode;
				ret=BGBBTJ_BT1C_DecodeBlocksCommandBasic(ctx, &cs, i);
				mode=ctx->curmode;
				nextmode=ctx->nextmode;
				break;
				
			case 0xED: case 0xEE:
				ret=BGBBTJ_BT1C_DecodeBlocksCommandBlockCopy32A8(
					ctx, &cs, &ct, &csl, &csa, i);
				break;

			case 0xF8:
			case 0xF9:
			case 0xFA:
			case 0xFB:
				j=((i-0xF8)<<8)|(*cs++);
				ctx->curmode=mode;
				ctx->nextmode=nextmode;
				ret=BGBBTJ_BT1C_DecodeBlocksCommandExt(ctx, &cs, j);
				diffm=ctx->diff_m;
				diffsc=ctx->diff_sc;
				mode=ctx->curmode;
				nextmode=ctx->nextmode;
				break;
			
			case 0xFC:
			case 0xFD:
			case 0xFE:
			case 0xFF:
				j=((i-0xFC)<<8)|(*cs++);
				if((j>=0x80) && (j<=0x9F))
				{
					n1=(j&31)+1;
					BGBBTJ_BT1C_DecodeBlockColorArrayMB5C(ctx,
						blks, ct, csa, &cs, n1,
						xs1, ys1, 32, mode, dfl);
					ct+=n1*32; csl+=n1*32; csa+=n1*8;
					break;
				}
				break;
			default:
				ret=-1;
				break;
			}
			break;

		default:
			switch(mode)
			{
			case 0:
				switch(i&0xE0)
				{
				case 0xA0:
					if(diffm && (cs[0]&0x80))
					{
						j=*cs++;
						j=bt1c_difftab[(diffsc*256)+(j&127)];
						k=BGBBTJ_BT1C_PredictDiffColorA(ctx,
							blks, ct, xs1, ys1, 32);
						j=k+j;
					}else
					{
						j=(cs[0]<<8)|cs[1];
						cs+=2;
						l=j;
					}

					ctx->diff_lclra=j;
					ctx->diff_lclrb=j;
					j=((j&0x7FE0)<<1)|(j&0x1F);
					j|=(j>>5)&0x20;
					
					if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy8(ct, csa);
							BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic(
								ct, j, j, 0x00000000, dfl);
							ct+=32; csl+=32; csa+=8;
						}
					}else
					{
						if(l&0x8000)
						{
							BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasicAlpha(
								ctb, j, j, 0xFFFFFFFFU, dfl);
						}else
						{
							BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic(
								ctb, j, j, 0x00000000, dfl);
						}

						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy32(ct, ctb);
							ct+=32; csl+=32; csa+=8;
						}
					}
					break;
				case 0xC0:
					if(diffm && (cs[0]&0x80))
					{
						j=*cs++;
						k=*cs++;
						j=bt1c_difftab[(diffsc*256)+(j&127)];
						k=bt1c_difftab[(diffsc*256)+(k&127)];
						l=BGBBTJ_BT1C_PredictDiffColorA(ctx,
							blks, ct, xs1, ys1, 32);
						j=l+j;
						l=BGBBTJ_BT1C_PredictDiffColorB(ctx,
							blks, ct, xs1, ys1, 32);
						k=l+k;

						ctx->diff_lclra=j;
						ctx->diff_lclrb=k;

						j=((j&0x7FE0)<<1)|(j&0x1F);
						k=((k&0x7FE0)<<1)|(k&0x1F);
						j|=(j>>5)&0x20;
						k|=(k>>5)&0x20;

						clra=j; clrb=k;

						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							k=(cs[0]<<24)|(cs[1]<<16)|(cs[2]<<8)|(cs[3]);
							cs+=4;
							if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
								{ bgbbtj_bt1c_memcpy8(ct, csa); }
							BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic(
								ct, clra, clrb, k, dfl);
							ct+=32; csl+=32; csa+=8;
						}
						break;
					}
					
					j=(cs[0]<<8)|cs[1];
					k=(cs[2]<<8)|cs[3];
					cs+=4;
					ctx->diff_lclra=j;
					ctx->diff_lclrb=k;

					l=k;
					j=((j&0x7FE0)<<1)|(j&0x1F);
					k=((k&0x7FE0)<<1)|(k&0x1F);
					j|=(j>>5)&0x20;
					k|=(k>>5)&0x20;

					clra=j; clrb=k;
					
					n1=(i&31)+1;
					for(i=0; i<n1; i++)
					{
						k=(cs[0]<<24)|(cs[1]<<16)|(cs[2]<<8)|(cs[3]);
						cs+=4;
						if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
							{ bgbbtj_bt1c_memcpy8(ct, csa); }
						if(l&0x8000)
						{
							BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasicAlpha(
								ct, clra, clrb, k, dfl);
						}else
						{
							BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic(
								ct, clra, clrb, k, dfl);
						}
						ct+=32; csl+=32; csa+=8;
					}
					break;

				default:
					if(cs[1]&0x80)
					{
						cs--;
						j=(cs[0]<<8)|cs[1];
						k=(cs[2]<<8)|cs[3];
						cs+=4;
						ctx->diff_lclra=j;
						ctx->diff_lclrb=k;

						j=((j&0x7FE0)<<1)|(j&0x1F);
						k=((k&0x7FE0)<<1)|(k&0x1F);
						j|=(j>>5)&0x20;
						k|=(k>>5)&0x20;

						clra=j; clrb=k;
						k=(cs[0]<<24)|(cs[1]<<16)|(cs[2]<<8)|(cs[3]);
						cs+=4;
						if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
							{ bgbbtj_bt1c_memcpy8(ct, csa); }
						BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic(
							ct, clra, clrb, k, dfl);
						ct+=32; csl+=32; csa+=8;
					}else
					{
//						memset(ctb, 0, 32);
						//dummy...
						cs--;
						BGBBTJ_BT1C_DecodeRawBlockMB5C(ctx,
							blks, ct, csa, &cs, 1,
							xs1, ys1, 32, mode, dfl);
//						cs+=31;
//						bgbbtj_bt1c_memcpy32(ct, ctb);
						ct+=32; csl+=32; csa+=8;
					}
					break;
				}
				break;
			case 1:
				switch(i&0xE0)
				{
				case 0xA0:
					j=ctx->pal256[*cs++];
					ctx->diff_lclra=j;
					ctx->diff_lclrb=j;
//					j=((j&0x7FE0)<<1)|(j&0x1F);
//					j|=(j>>5)&0x20;
					if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy8(ct, csa);
							BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic_RGB24(
								ct, j, j, 0, dfl);
							ct+=32; csl+=32; csa+=8;
						}
					}else
					{
						BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic_RGB24(
							ctb, j, j, 0, dfl);
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy32(ct, ctb);
							ct+=32; csl+=32; csa+=8;
						}
					}
					break;
				case 0xC0:
					j=ctx->pal256[cs[0]];
					k=ctx->pal256[cs[1]];
					cs+=2;
//					j=((j&0x7FE0)<<1)|(j&0x1F);
//					k=((k&0x7FE0)<<1)|(k&0x1F);
//					j|=(j>>5)&0x20;
//					k|=(k>>5)&0x20;

					ctx->diff_lclra=j;
					ctx->diff_lclrb=k;

					clra=j; clrb=k;
					n1=(i&31)+1;
					for(i=0; i<n1; i++)
					{
						j=bt1c_blkidxmap1[cs[0]];
						k=bt1c_blkidxmap1[cs[1]];
						cs+=2;
						if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
							{ bgbbtj_bt1c_memcpy8(ct, csa); }
						BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic_RGB24(
							ct, clra, clrb, (j<<16)|k, dfl);
						ct+=32; csl+=32; csa+=8;
					}
					break;
				default:
					cs--;
					j=ctx->pal256[cs[0]];
					k=ctx->pal256[cs[1]];
					cs+=2;
//					j=((j&0x7FE0)<<1)|(j&0x1F);
//					k=((k&0x7FE0)<<1)|(k&0x1F);
//					j|=(j>>5)&0x20;
//					k|=(k>>5)&0x20;

					ctx->diff_lclra=j;
					ctx->diff_lclrb=k;

					clra=j; clrb=k;
					j=bt1c_blkidxmap1[cs[0]];
					k=bt1c_blkidxmap1[cs[1]];
					cs+=2;
					if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
						{ bgbbtj_bt1c_memcpy8(ct, csa); }
					BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic_RGB24(
						ct, clra, clrb, (j<<16)|k, dfl);
					ct+=32; csl+=32; csa+=8;
					break;
				}
				break;
			case 2:
				switch(i&0xE0)
				{
				case 0xA0:
					j=ctx->pal256[*cs++];
					ctx->diff_lclra=j;
					ctx->diff_lclrb=j;
//					j=((j&0x7FE0)<<1)|(j&0x1F);
//					j|=(j>>5)&0x20;
					if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy8(ct, csa);
							BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic_RGB24(
								ct, j, j, 0, dfl);
							ct+=32; csl+=32; csa+=8;
						}
					}else
					{
						BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic_RGB24(
							ctb, j, j, 0, dfl);
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy32(ct, ctb);
							ct+=32; csl+=32; csa+=8;
						}
					}
					break;
				case 0xC0:
					j=ctx->pal256[cs[0]];
					k=ctx->pal256[cs[1]];
					cs+=2;
//					j=((j&0x7FE0)<<1)|(j&0x1F);
//					k=((k&0x7FE0)<<1)|(k&0x1F);
//					j|=(j>>5)&0x20;
//					k|=(k>>5)&0x20;

					ctx->diff_lclra=j;
					ctx->diff_lclrb=k;

					clra=j; clrb=k;
					n1=(i&31)+1;
					for(i=0; i<n1; i++)
					{
						j=bt1c_blkidxmap2[*cs++];
						if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
							{ bgbbtj_bt1c_memcpy8(ct, csa); }
						BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic_RGB24(
							ct, clra, clrb, j, dfl);
						ct+=32; csl+=32; csa+=8;
					}
					break;
				default:
					cs--;
					j=ctx->pal256[cs[0]];
					k=ctx->pal256[cs[1]];
					cs+=2;
//					j=((j&0x7FE0)<<1)|(j&0x1F);
//					k=((k&0x7FE0)<<1)|(k&0x1F);
//					j|=(j>>5)&0x20;
//					k|=(k>>5)&0x20;
					ctx->diff_lclra=j;
					ctx->diff_lclrb=k;
					clra=j; clrb=k;
					j=bt1c_blkidxmap2[*cs++];
					if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
						{ bgbbtj_bt1c_memcpy8(ct, csa); }
					BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic_RGB24(
						ct, clra, clrb, j, dfl);
					ct+=32; csl+=32; csa+=8;
					break;
				}
				break;
			case 3:
				switch(i&0xE0)
				{
				case 0xA0:
					j=ctx->pal256[*cs++];
//					j=((j&0x7FE0)<<1)|(j&0x1F);
//					j|=(j>>5)&0x20;
					ctx->diff_lclra=j;
					ctx->diff_lclrb=j;

					if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy8(ct, csa);
							BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic_RGB24(
								ct, j, j, 0, dfl);
							ct+=32; csl+=32; csa+=8;
						}
					}else
					{
						BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic_RGB24(
							ctb, j, j, 0, dfl);
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy32(ct, ctb);
							ct+=32; csl+=32; csa+=8;
						}
					}
					break;
				case 0xC0:
					j=ctx->pal256[cs[0]];
					k=ctx->pal256[cs[1]];
					cs+=2;
//					j=((j&0x7FE0)<<1)|(j&0x1F);
//					k=((k&0x7FE0)<<1)|(k&0x1F);
//					j|=(j>>5)&0x20;
//					k|=(k>>5)&0x20;
					ctx->diff_lclra=j;
					ctx->diff_lclrb=k;

					clra=j; clrb=k;
					n1=(i&31)+1;
					for(i=0; i<n1; i++)
					{
						j=ctx->pat256[*cs++];
						if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
							{ bgbbtj_bt1c_memcpy8(ct, csa); }
						BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic_RGB24(
							ct, clra, clrb, j, dfl);
						ct+=32; csl+=32; csa+=8;
					}
					break;
				default:
					cs--;
					j=ctx->pal256[cs[0]];
					k=ctx->pal256[cs[1]];
					cs+=2;
//					j=((j&0x7FE0)<<1)|(j&0x1F);
//					k=((k&0x7FE0)<<1)|(k&0x1F);
//					j|=(j>>5)&0x20;
//					k|=(k>>5)&0x20;
					ctx->diff_lclra=j;
					ctx->diff_lclrb=k;
					clra=j; clrb=k;
					j=ctx->pat256[*cs++];
					if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
						{ bgbbtj_bt1c_memcpy8(ct, csa); }
					BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic_RGB24(
						ct, clra, clrb, j, dfl);
					ct+=32; csl+=32; csa+=8;
					break;
				}
				break;

#if 1
			case 4:
			case 5:
			case 14:
				switch(i&0xE0)
				{
				case 0xA0:
					if(cs[0]&0x80)
					{
						j=*cs++;
						j=bt1c_difftab[(diffsc*256)+(j&127)];
						k=BGBBTJ_BT1C_PredictDiffColorA(ctx,
							blks, ct, xs1, ys1, 32);
						j=k+j;
					}else
					{
						j=(cs[0]<<8)|cs[1]; cs+=2;
					}

					ctx->diff_lclra=j;
					ctx->diff_lclrb=j;

					j=((j&0x7FE0)<<1)|(j&0x1F);
					j|=(j>>5)&0x20;

					if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy8(ct, csa);
							BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic(
								ct, j, j, 0, dfl);
							ct+=32; csl+=32; csa+=8;
						}
					}else
					{
						BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic(
							ctb, j, j, 0, dfl);
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy32(ct, ctb);
							ct+=32; csl+=32; csa+=8;
						}
					}
					break;
				case 0xC0:
					if(cs[0]&0x80)
					{
						j=*cs++; k=*cs++;
						j=bt1c_difftab[(diffsc*256)+(j&127)];
						k=bt1c_difftab[(diffsc*256)+(k&127)];
						l=BGBBTJ_BT1C_PredictDiffColorA(ctx,
							blks, ct, xs1, ys1, 32);
						j=l+j;
						l=BGBBTJ_BT1C_PredictDiffColorB(ctx,
							blks, ct, xs1, ys1, 32);
						k=l+k;
					}else
					{
						j=(cs[0]<<8)|cs[1]; cs+=2;
						k=(cs[0]<<8)|cs[1]; cs+=2;
					}

					ctx->diff_lclra=j;
					ctx->diff_lclrb=k;

					j=((j&0x7FE0)<<1)|(j&0x1F);
					k=((k&0x7FE0)<<1)|(k&0x1F);
					j|=(j>>5)&0x20;
					k|=(k>>5)&0x20;

					clra=j; clrb=k;
					if(mode==4)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							j=bt1c_blkidxmap1[cs[0]];
							k=bt1c_blkidxmap1[cs[1]];
							cs+=2;
							if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
								{ bgbbtj_bt1c_memcpy8(ct, csa); }
							BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic(
								ct, clra, clrb, (j<<16)|k, dfl);
							ct+=32; csl+=32; csa+=8;
						}
					}else if(mode==5)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							j=bt1c_blkidxmap2[*cs++];
							if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
								{ bgbbtj_bt1c_memcpy8(ct, csa); }
							BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic(
								ct, clra, clrb, j, dfl);
							ct+=32; csl+=32; csa+=8;
						}
					}else if(mode==14)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							j=ctx->pat256[*cs++];
							if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
								{ bgbbtj_bt1c_memcpy8(ct, csa); }
							BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic(
								ct, clra, clrb, j, dfl);
							ct+=32; csl+=32; csa+=8;
						}
					}
					break;
				default:
					if((ctx->optcntrl[1]>0) && !(cs[1]&0x80))
					{
						cs--;
						BGBBTJ_BT1C_DecodeRawBlockMB5C(ctx,
							blks, ct, csa, &cs, 1,
							xs1, ys1, 32, mode, dfl);
						ct+=32; csl+=32; csa+=8;
						break;
					}

					cs--;
					j=(cs[0]<<8)|cs[1]; cs+=2;
					k=(cs[0]<<8)|cs[1]; cs+=2;
					ctx->diff_lclra=j;
					ctx->diff_lclrb=k;
					j=((j&0x7FE0)<<1)|(j&0x1F);
					k=((k&0x7FE0)<<1)|(k&0x1F);
					j|=(j>>5)&0x20;
					k|=(k>>5)&0x20;

					clra=j; clrb=k;
					if(mode==4)
					{
						j=bt1c_blkidxmap1[cs[0]];
						k=bt1c_blkidxmap1[cs[1]];
						cs+=2;
						if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
							{ bgbbtj_bt1c_memcpy8(ct, csa); }
						BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic(
							ct, clra, clrb, (j<<16)|k, dfl);
						ct+=32; csl+=32; csa+=8;
					}else if(mode==5)
					{
						j=bt1c_blkidxmap2[*cs++];
						if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
							{ bgbbtj_bt1c_memcpy8(ct, csa); }
						BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic(
							ct, clra, clrb, j, dfl);
						ct+=32; csl+=32; csa+=8;
					}else if(mode==14)
					{
						j=ctx->pat256[*cs++];
						if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
							{ bgbbtj_bt1c_memcpy8(ct, csa); }
						BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic(
							ct, clra, clrb, j, dfl);
						ct+=32; csl+=32; csa+=8;
					}
					break;
				}
				break;
#endif

#if 1
			case 6:
				switch(i&0xE0)
				{
				case 0xA0:
					if(diffm && (cs[0]&0x80))
					{
						j=(cs[0]<<8)|cs[1];
						cs+=2;

//						j=BGBBTJ_BT1C_ConvDiff15ToDiff24(ctx, j);

//						j=*cs++;
//						j=bt1c_difftab[(diffsc*256)+(j&127)];
//						k=BGBBTJ_BT1C_PredictDiffColorA(ctx,
//							blks, ct, xs1, ys1, 32);
//						j=k+j;
//						k=BGBBTJ_BT1C_ConvRGB16ToRGB24(ctx, j);
	
						j=BGBBTJ_BT1C_CalcDiff15ColorA24(ctx,
							blks, ct, xs1, ys1, 32, j);
					
						ctx->diff_lclra=j;
						ctx->diff_lclrb=j;
					}else
					{
						j=(cs[0]<<16)|(cs[1]<<8)|cs[2];
						cs+=3;
						l=j;
						j=BGBBTJ_BT1C_ConvRGB23ToRGB24(ctx, j);
						ctx->diff_lclra=j;
						ctx->diff_lclrb=j;
					}

					if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy8(ct, csa);
							BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic_RGB24(
								ct, j, j, 0, dfl);
							ct+=32; csl+=32; csa+=8;
						}
					}else
					{
						BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic_RGB24(
							ctb, j, j, 0, dfl);
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy32(ct, ctb);
							ct+=32; csl+=32; csa+=8;
						}
					}
					break;
				case 0xC0:
					if(diffm && (cs[0]&0x80))
					{
						j=(cs[0]<<8)|cs[1];
						k=(cs[2]<<8)|cs[3];
						cs+=4;
//						j=bt1c_difftab[(diffsc*256)+(j&127)];
//						k=bt1c_difftab[(diffsc*256)+(k&127)];
//						l=BGBBTJ_BT1C_PredictDiffColorA(ctx,
//							blks, ct, xs1, ys1, 32);
//						j=l+j;
//						l=BGBBTJ_BT1C_PredictDiffColorB(ctx,
//							blks, ct, xs1, ys1, 32);
//						k=l+k;

						j=BGBBTJ_BT1C_CalcDiff15ColorA24(ctx,
							blks, ct, xs1, ys1, 32, j);
						k=BGBBTJ_BT1C_CalcDiff15ColorB24(ctx,
							blks, ct, xs1, ys1, 32, k);
					
						ctx->diff_lclra=j;
						ctx->diff_lclrb=k;
					}else
					{
						j=(cs[0]<<16)|(cs[1]<<8)|cs[2];
						k=(cs[3]<<16)|(cs[4]<<8)|cs[5];
						cs+=6;
						l=k;
						j=BGBBTJ_BT1C_ConvRGB23ToRGB24(ctx, j);
						k=BGBBTJ_BT1C_ConvRGB23ToRGB24(ctx, k);
						ctx->diff_lclra=j;
						ctx->diff_lclrb=k;
					}

					clra=j; clrb=k;

					n1=(i&31)+1;
					for(i=0; i<n1; i++)
					{
						j=(cs[0]<<24)|(cs[1]<<16)|(cs[2]<<8)|(cs[3]);
						cs+=4;
						if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
							{ bgbbtj_bt1c_memcpy8(ct, csa); }
						BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic_RGB24(
							ct, clra, clrb, j, dfl);
						ct+=32; csl+=32; csa+=8;
					}
					break;

				default:
//					if(!(cs[2]&0x80))
					if((ctx->optcntrl[1]>0) && !(cs[2]&0x80))
					{
						cs--;
						BGBBTJ_BT1C_DecodeRawBlockMB5C(ctx,
							blks, ct, csa, &cs, 1,
							xs1, ys1, 32, mode, dfl);
						ct+=32; csl+=32; csa+=8;
						break;
					}

					cs--;
					j=(cs[0]<<16)|(cs[1]<<8)|cs[2];
					k=(cs[3]<<16)|(cs[4]<<8)|cs[5];
					cs+=6;
					j=BGBBTJ_BT1C_ConvRGB23ToRGB24(ctx, j);
					k=BGBBTJ_BT1C_ConvRGB23ToRGB24(ctx, k);
					ctx->diff_lclra=j;
					ctx->diff_lclrb=k;

//					*(int *)-1=-1;

					clra=j; clrb=k;
					j=(cs[0]<<24)|(cs[1]<<16)|(cs[2]<<8)|(cs[3]);
					cs+=4;
					if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
						{ bgbbtj_bt1c_memcpy8(ct, csa); }
					BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic_RGB24(
						ct, clra, clrb, j, dfl);
					ct+=32; csl+=32; csa+=8;
					break;
				}
				break;
#endif

#if 1
			case 7:
			case 8:
				switch(i&0xE0)
				{
				case 0xA0:
					if(diffm && (cs[0]&0x80))
					{
						j=(cs[0]<<8)|cs[1];
						cs+=2;

						clraw=BGBBTJ_BT1C_CalcDiff15ColorA48(ctx,
							blks, ct, xs1, ys1, 16, j);
						ctx->diff_lclraw=clraw;
						ctx->diff_lclrbw=clraw;
					}else
					{
						j=(cs[0]<<24)|(cs[1]<<16)|(cs[2]<<8)|cs[3];
						cs+=4;
						l=j;
						if(mode==8)
						{
							clraw=BGBBTJ_BT1C_ConvRGB31FUToRGB48F(ctx, j);
						}
						else
						{
							clraw=BGBBTJ_BT1C_ConvRGB31FSToRGB48F(ctx, j);
						}
						ctx->diff_lclraw=clraw;
						ctx->diff_lclrbw=clraw;
					}
					
//					j=BGBBTJ_BT1C_ConvRGB48FToRGB24(ctx, clraw);

					if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy8(ct, csa);
							BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic_RGB48(
								ct, clraw, clraw, 0, dfl);
							ct+=32; csl+=32; csa+=8;
						}
					}else
					{
						BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic_RGB48(
							ctb, clraw, clraw, 0, dfl);
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy32(ct, ctb);
							ct+=32; csl+=32; csa+=8;
						}
					}
					break;
				case 0xC0:
					if(diffm && (cs[0]&0x80))
					{
						j=(cs[0]<<8)|cs[1];
						k=(cs[2]<<8)|cs[3];
						cs+=4;
						clraw=BGBBTJ_BT1C_CalcDiff15ColorA48(ctx,
							blks, ct, xs1, ys1, 16, j);
						clrbw=BGBBTJ_BT1C_CalcDiff15ColorB48(ctx,
							blks, ct, xs1, ys1, 16, k);
						ctx->diff_lclraw=clraw;
						ctx->diff_lclrbw=clrbw;
					}else
					{
						j=(cs[0]<<24)|(cs[1]<<16)|(cs[2]<<8)|cs[3];
						k=(cs[4]<<24)|(cs[5]<<16)|(cs[6]<<8)|cs[7];
						cs+=8;
						l=k;
						if(mode==8)
						{
							clraw=BGBBTJ_BT1C_ConvRGB31FUToRGB48F(ctx, j);
							clrbw=BGBBTJ_BT1C_ConvRGB31FUToRGB48F(ctx, k);
						}
						else
						{
							clraw=BGBBTJ_BT1C_ConvRGB31FSToRGB48F(ctx, j);
							clrbw=BGBBTJ_BT1C_ConvRGB31FSToRGB48F(ctx, k);
						}
						ctx->diff_lclraw=clraw;
						ctx->diff_lclrbw=clrbw;
					}

//					j=BGBBTJ_BT1C_ConvRGB48FToRGB24(ctx, clraw);
//					k=BGBBTJ_BT1C_ConvRGB48FToRGB24(ctx, clrbw);
//					clra=j; clrb=k;

					n1=(i&31)+1;
					for(i=0; i<n1; i++)
					{
						j=(cs[0]<<24)|(cs[1]<<16)|(cs[2]<<8)|(cs[3]);
						cs+=4;
						if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
							{ bgbbtj_bt1c_memcpy8(ct, csa); }
						BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic_RGB48(
							ct, clraw, clrbw, j, dfl);
						ct+=32; csl+=32; csa+=8;
					}
					break;

				default:
//					if(!(cs[3]&0x80))
					if((ctx->optcntrl[1]>0) && !(cs[3]&0x80))
					{
						cs--;
						BGBBTJ_BT1C_DecodeRawBlockMB5C(ctx,
							blks, ct, csa, &cs, 1,
							xs1, ys1, 32, mode, dfl);
						ct+=32; csl+=32; csa+=8;
						break;
					}

					cs--;
					j=(cs[0]<<24)|(cs[1]<<16)|(cs[2]<<8)|cs[3];
					k=(cs[0]<<24)|(cs[1]<<16)|(cs[2]<<8)|cs[3];
					cs+=8;
					if(mode==8)
					{
						clraw=BGBBTJ_BT1C_ConvRGB31FUToRGB48F(ctx, j);
						clrbw=BGBBTJ_BT1C_ConvRGB31FUToRGB48F(ctx, k);
					}
					else
					{
						clraw=BGBBTJ_BT1C_ConvRGB31FSToRGB48F(ctx, j);
						clrbw=BGBBTJ_BT1C_ConvRGB31FSToRGB48F(ctx, k);
					}
					ctx->diff_lclraw=clraw;
					ctx->diff_lclrbw=clrbw;

//					j=BGBBTJ_BT1C_ConvRGB48FToRGB24(ctx, clraw);
//					k=BGBBTJ_BT1C_ConvRGB48FToRGB24(ctx, clrbw);

					clra=j; clrb=k;
					j=(cs[0]<<24)|(cs[1]<<16)|(cs[2]<<8)|(cs[3]);
					cs+=4;
					if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
						{ bgbbtj_bt1c_memcpy8(ct, csa); }
					BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic_RGB48(
						ct, clraw, clrbw, j, dfl);
					ct+=32; csl+=32; csa+=8;
					break;
				}
				break;
#endif

#if 1
			case 9:
				switch(i&0xE0)
				{
				case 0xA0:
					j=ctx->pal16[(*cs++)&15];
					if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy8(ct, csa);
							BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic_RGB24(
								ct, j, j, 0, dfl);
							ct+=32; csl+=32; csa+=8;
						}
					}else
					{
						BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic_RGB24(
							ctb, j, j, 0, dfl);
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy16(ct, ctb);
							ct+=32; csl+=32; csa+=8;
						}
					}
					break;
				case 0xC0:
					j=ctx->pal16[(cs[0]>>4)&15];
					k=ctx->pal16[(cs[0]>>0)&15];
					cs++;

					clra=j; clrb=k;
					n1=(i&31)+1;
					for(i=0; i<n1; i++)
					{
						j=bt1c_blkidxmap1[cs[0]];
						k=bt1c_blkidxmap1[cs[1]];
						cs+=2;
						if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
							{ bgbbtj_bt1c_memcpy8(ct, csa); }
						BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic_RGB24(
							ct, clra, clrb, (j<<16)|k, dfl);
						ct+=32; csl+=32; csa+=8;
					}
					break;
				default:
					cs--;
					j=ctx->pal16[(cs[0]>>4)&15];
					k=ctx->pal16[(cs[0]>>0)&15];
					cs++;
					
					clra=j; clrb=k;
					j=bt1c_blkidxmap1[cs[0]];
					k=bt1c_blkidxmap1[cs[1]];
					cs+=2;
					if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
						{ bgbbtj_bt1c_memcpy8(ct, csa); }
					BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic_RGB24(
						ct, clra, clrb, (j<<16)|k, dfl);
					ct+=32; csl+=32; csa+=8;
					break;
				}
				break;
#endif

#if 1
			case 10:
			case 11:
			case 12:
				switch(i&0xE0)
				{
				case 0xA0:
					if(cs[0]&0x80)
					{
						j=(cs[0]<<8)|cs[1];
						cs+=2;
						j=BGBBTJ_BT1C_CalcDiff15ColorA24(ctx,
							blks, ct, xs1, ys1, 32, j);
					
						ctx->diff_lclra=j;
						ctx->diff_lclrb=j;
					}else
					{
						j=(cs[0]<<16)|(cs[1]<<8)|cs[2];
						cs+=3;
						l=j;
						j=BGBBTJ_BT1C_ConvRGB23ToRGB24(ctx, j);
						ctx->diff_lclra=j;
						ctx->diff_lclrb=j;
					}
					
					if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy8(ct, csa);
							BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic_RGB24(
								ct, j, j, 0, dfl);
							ct+=32; csl+=32; csa+=8;
						}
					}else
					{
						BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic_RGB24(
							ctb, j, j, 0, dfl);
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy16(ct, ctb);
							ct+=32; csl+=32; csa+=8;
						}
					}
					break;
				case 0xC0:
					if(cs[0]&0x80)
					{
						j=(cs[0]<<8)|cs[1];
						k=(cs[2]<<8)|cs[3];
						cs+=4;
						j=BGBBTJ_BT1C_CalcDiff15ColorA24(ctx,
							blks, ct, xs1, ys1, 32, j);
						k=BGBBTJ_BT1C_CalcDiff15ColorB24(ctx,
							blks, ct, xs1, ys1, 32, k);
						ctx->diff_lclra=j;
						ctx->diff_lclrb=k;
					}else
					{
						j=(cs[0]<<16)|(cs[1]<<8)|cs[2];
						k=(cs[3]<<16)|(cs[4]<<8)|cs[5];
						cs+=6;
						l=k;
						j=BGBBTJ_BT1C_ConvRGB23ToRGB24(ctx, j);
						k=BGBBTJ_BT1C_ConvRGB23ToRGB24(ctx, k);
						ctx->diff_lclra=j;
						ctx->diff_lclrb=k;
					}

					clra=j; clrb=k;
					if(mode==10)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							j=bt1c_blkidxmap1[cs[0]];
							k=bt1c_blkidxmap1[cs[1]];
							cs+=2;
							if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
								{ bgbbtj_bt1c_memcpy8(ct, csa); }
							BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic_RGB24(
								ct, clra, clrb, (j<<16)|k, dfl);
							ct+=32; csl+=32; csa+=8;
						}
					}else if(mode==11)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							j=bt1c_blkidxmap2[*cs++];
							if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
								{ bgbbtj_bt1c_memcpy8(ct, csa); }
							BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic_RGB24(
								ct, clra, clrb, j, dfl);
							ct+=32; csl+=32; csa+=8;
						}
					}else if(mode==12)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							j=ctx->pat256[*cs++];
							if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
								{ bgbbtj_bt1c_memcpy8(ct, csa); }
							BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic_RGB24(
								ct, clra, clrb, j, dfl);
							ct+=32; csl+=32; csa+=8;
						}
					}
					break;
				default:
					if((ctx->optcntrl[1]>0) && !(cs[2]&0x80))
					{
						cs--;
						BGBBTJ_BT1C_DecodeRawBlockMB5C(ctx,
							blks, ct, csa, &cs, 1,
							xs1, ys1, 32, mode, dfl);
						ct+=32; csl+=32; csa+=8;
						break;
					}

					cs--;
					j=(cs[0]<<16)|(cs[1]<<8)|cs[2];
					k=(cs[3]<<16)|(cs[4]<<8)|cs[5];
					cs+=6;
					j=BGBBTJ_BT1C_ConvRGB23ToRGB24(ctx, j);
					k=BGBBTJ_BT1C_ConvRGB23ToRGB24(ctx, k);
					ctx->diff_lclra=j;
					ctx->diff_lclrb=k;

					clra=j; clrb=k;
					if(mode==10)
					{
						j=bt1c_blkidxmap1[cs[0]];
						k=bt1c_blkidxmap1[cs[1]];
						cs+=2;
						if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
							{ bgbbtj_bt1c_memcpy8(ct, csa); }
						BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic_RGB24(
							ct, clra, clrb, (j<<16)|k, dfl);
						ct+=32; csl+=32; csa+=8;
					}else if(mode==11)
					{
						j=bt1c_blkidxmap2[*cs++];
						if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
							{ bgbbtj_bt1c_memcpy8(ct, csa); }
						BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic_RGB24(
							ct, clra, clrb, j, dfl);
						ct+=32; csl+=32; csa+=8;
					}else if(mode==12)
					{
						j=ctx->pat256[*cs++];
						if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
							{ bgbbtj_bt1c_memcpy8(ct, csa); }
						BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic_RGB24(
							ct, clra, clrb, j, dfl);
						ct+=32; csl+=32; csa+=8;
					}
					break;
				}
				break;
#endif

#if 1
			case 13:
				switch(i&0xE0)
				{
				case 0xA0:
					if(cs[0]&0x80)
					{
						j=*cs++;
						j=bt1c_difftab24[(diffsc*256)+(j&127)];
						k=BGBBTJ_BT1C_PredictDiffColorA24(ctx,
							blks, ct, xs1, ys1, 16);
//						j=k+j;
						j=bgbbtj_bt1c_addrgb24(k, j);
//						j=bt1c_difftab[(diffsc*256)+(j&127)];
//						j=BGBBTJ_BT1C_CalcDiff15ColorA24(ctx,
//							blks, ct, xs1, ys1, 32, j);
						ctx->diff_lclra=j;
						ctx->diff_lclrb=j;
					}else
					{
						j=(cs[0]<<16)|(cs[1]<<8)|cs[2];
						cs+=3;
						l=j;
						j=BGBBTJ_BT1C_ConvRGB23ToRGB24(ctx, j);
						ctx->diff_lclra=j;
						ctx->diff_lclrb=j;
					}

					if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy8(ct, csa);
							BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic_RGB24(
								ct, j, j, 0, dfl);
							ct+=32; csl+=32; csa+=8;
						}
					}else
					{
						BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic_RGB24(
							ctb, j, j, 0, dfl);
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy16(ct, ctb);
							ct+=32; csl+=32; csa+=8;
						}
					}
					break;
				case 0xC0:
					if(cs[0]&0x80)
					{
						j=cs[0]; k=cs[1]; cs+=2;
						j=bt1c_difftab24[(diffsc*256)+(j&127)];
						k=bt1c_difftab24[(diffsc*256)+(k&127)];
						l=BGBBTJ_BT1C_PredictDiffColorA24(ctx,
							blks, ct, xs1, ys1, 16);
//						j=l+j;
						j=bgbbtj_bt1c_addrgb24(l, j);
						l=BGBBTJ_BT1C_PredictDiffColorB24(ctx,
							blks, ct, xs1, ys1, 16);
//						k=l+k;
						k=bgbbtj_bt1c_addrgb24(l, k);
//						j=bt1c_difftab[(diffsc*256)+(j&127)];
//						k=bt1c_difftab[(diffsc*256)+(k&127)];
//						j=BGBBTJ_BT1C_CalcDiff15ColorA24(ctx,
//							blks, ct, xs1, ys1, 32, j);
//						k=BGBBTJ_BT1C_CalcDiff15ColorB24(ctx,
//							blks, ct, xs1, ys1, 32, k);
						ctx->diff_lclra=j;
						ctx->diff_lclrb=k;
					}else
					{
						j=(cs[0]<<16)|(cs[1]<<8)|cs[2];
						k=(cs[3]<<16)|(cs[4]<<8)|cs[5];
						cs+=6;
						l=k;
						j=BGBBTJ_BT1C_ConvRGB23ToRGB24(ctx, j);
						k=BGBBTJ_BT1C_ConvRGB23ToRGB24(ctx, k);
						ctx->diff_lclra=j;
						ctx->diff_lclrb=k;
					}

					clra=j; clrb=k;
					n1=(i&31)+1;
					for(i=0; i<n1; i++)
					{
						j=bt1c_blkidxmap2[*cs++];
						if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
							{ bgbbtj_bt1c_memcpy8(ct, csa); }
						BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic_RGB24(
							ct, clra, clrb, j, dfl);
						ct+=32; csl+=32; csa+=8;
					}
					break;
				default:
					if((ctx->optcntrl[1]>0) && !(cs[2]&0x80))
					{
						cs--;
						BGBBTJ_BT1C_DecodeRawBlockMB5C(ctx,
							blks, ct, csa, &cs, 1,
							xs1, ys1, 16, mode, dfl);
						ct+=32; csl+=32; csa+=8;
						break;
					}

					cs--;
					j=(cs[0]<<16)|(cs[1]<<8)|cs[2];
					k=(cs[3]<<16)|(cs[4]<<8)|cs[5];
					cs+=6;
					j=BGBBTJ_BT1C_ConvRGB23ToRGB24(ctx, j);
					k=BGBBTJ_BT1C_ConvRGB23ToRGB24(ctx, k);
					ctx->diff_lclra=j;
					ctx->diff_lclrb=k;

					clra=j; clrb=k;
					j=bt1c_blkidxmap2[*cs++];
					if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
						{ bgbbtj_bt1c_memcpy8(ct, csa); }
					BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic_RGB24(
						ct, clra, clrb, j, dfl);
					ct+=32; csl+=32; csa+=8;
					break;
				}
				break;
#endif

#if 1
			case 15:
				switch(i&0xE0)
				{
				case 0xA0:
					j=ctx->pal16[(*cs++)&15];
					j=((j&0x7FE0)<<1)|(j&0x1F);
					j|=(j>>5)&0x20;
					if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy8(ct, csa);
							BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic(
								ct, j, j, 0, dfl);
							ct+=32; csl+=32; csa+=8;
						}
					}else
					{
						BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic(
							ctb, j, j, 0, dfl);
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy32(ct, ctb);
							ct+=32; csl+=32; csa+=8;
						}
					}
					break;
				case 0xC0:
					j=ctx->pal16[(cs[0]>>4)&15];
					k=ctx->pal16[(cs[0]>>0)&15];
					cs++;
					j=((j&0x7FE0)<<1)|(j&0x1F);
					k=((k&0x7FE0)<<1)|(k&0x1F);
					j|=(j>>5)&0x20;
					k|=(k>>5)&0x20;

					clra=j; clrb=k;
					n1=(i&31)+1;
					for(i=0; i<n1; i++)
					{
						j=ctx->pat256[*cs++];
						if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
							{ bgbbtj_bt1c_memcpy8(ct, csa); }
						BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic(
							ct, clra, clrb, j, dfl);
						ct+=32; csl+=32; csa+=8;
					}
					break;
				default:
					cs--;
					j=ctx->pal16[(cs[0]>>4)&15];
					k=ctx->pal16[(cs[0]>>0)&15];
					cs++;
					j=((j&0x7FE0)<<1)|(j&0x1F);
					k=((k&0x7FE0)<<1)|(k&0x1F);
					j|=(j>>5)&0x20;
					k|=(k>>5)&0x20;

					clra=j; clrb=k;
					j=ctx->pat256[*cs++];
					if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
						{ bgbbtj_bt1c_memcpy8(ct, csa); }
					BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic(
						ct, clra, clrb, j, dfl);
					ct+=32; csl+=32; csa+=8;
					break;
				}
				break;
#endif

#if 1
			case 16:
			case 17:
			case 18:
				switch(i&0xE0)
				{
				case 0xA0:
					if(cs[0]&0x80)
					{
						j=*cs++;
						k=ctx->diff_lclra;
						j=BGBBTJ_BT1C_AddPred24ToDiff7Y(ctx, k, j);
						ctx->diff_lclra=j;
						ctx->diff_lclrb=j;
					}else
					{
						j=*cs++;
						j=BGBBTJ_BT1C_ConvYYY7ToRGB24(ctx, j);
						ctx->diff_lclra=j;
						ctx->diff_lclrb=j;
					}
					
					if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy8(ct, csa);
							BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic_RGB24(
								ct, j, j, 0, dfl);
							ct+=32; csl+=32; csa+=8;
						}
					}else
					{
						BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic_RGB24(
							ctb, j, j, 0, dfl);
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy16(ct, ctb);
							ct+=32; csl+=32; csa+=8;
						}
					}
					break;
				case 0xC0:
					if(cs[0]&0x80)
					{
						j=cs[0]; k=cs[1]; cs+=2;
						l=ctx->diff_lclra;
						j=BGBBTJ_BT1C_AddPred24ToDiff7Y(ctx, l, j);
						l=ctx->diff_lclrb;
						k=BGBBTJ_BT1C_AddPred24ToDiff7Y(ctx, l, k);

						ctx->diff_lclra=j;
						ctx->diff_lclrb=k;
					}else
					{
						j=cs[0]; k=cs[1]; cs+=2;
						j=BGBBTJ_BT1C_ConvYYY7ToRGB24(ctx, j);
						k=BGBBTJ_BT1C_ConvYYY7ToRGB24(ctx, k);
						ctx->diff_lclra=j;
						ctx->diff_lclrb=k;
					}

					clra=j; clrb=k;
					if(mode==16)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							j=(cs[0]<<24)|(cs[1]<<16)|
								(cs[2]<<8)|(cs[3]);
							cs+=4;
							if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
								{ bgbbtj_bt1c_memcpy8(ct, csa); }
							BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic_RGB24(
								ct, clra, clrb, j, dfl);
							ct+=32; csl+=32; csa+=8;
						}
					}else if(mode==17)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							j=bt1c_blkidxmap1[cs[0]];
							k=bt1c_blkidxmap1[cs[1]];
							cs+=2;
							if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
								{ bgbbtj_bt1c_memcpy8(ct, csa); }
							BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic_RGB24(
								ct, clra, clrb, (j<<16)|k, dfl);
							ct+=32; csl+=32; csa+=8;
						}
					}else if(mode==18)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							j=bt1c_blkidxmap2[*cs++];
							if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
								{ bgbbtj_bt1c_memcpy8(ct, csa); }
							BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic_RGB24(
								ct, clra, clrb, j, dfl);
							ct+=32; csl+=32; csa+=8;
						}
					}
					break;
				default:
					if((ctx->optcntrl[1]>0) && !(cs[0]&0x80))
					{
						cs--;
						BGBBTJ_BT1C_DecodeRawBlockMB5C(ctx,
							blks, ct, csa, &cs, 1,
							xs1, ys1, 32, mode, dfl);
						ct+=32; csl+=32; csa+=8;
						break;
					}

					cs--;
					j=cs[0]; k=cs[1]; cs+=2;
					j=BGBBTJ_BT1C_ConvYYY7ToRGB24(ctx, j);
					k=BGBBTJ_BT1C_ConvYYY7ToRGB24(ctx, k);
					ctx->diff_lclra=j;
					ctx->diff_lclrb=k;

					clra=j; clrb=k;
					if(mode==16)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							j=(cs[0]<<24)|(cs[1]<<16)|
								(cs[2]<<8)|(cs[3]);
							cs+=4;
							if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
								{ bgbbtj_bt1c_memcpy8(ct, csa); }
							BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic_RGB24(
								ct, clra, clrb, j, dfl);
							ct+=32; csl+=32; csa+=8;
						}
					}else if(mode==17)
					{
						j=bt1c_blkidxmap1[cs[0]];
						k=bt1c_blkidxmap1[cs[1]];
						cs+=2;
						if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
							{ bgbbtj_bt1c_memcpy8(ct, csa); }
						BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic_RGB24(
							ct, clra, clrb, (j<<16)|k, dfl);
						ct+=32; csl+=32; csa+=8;
					}else if(mode==18)
					{
						j=bt1c_blkidxmap2[*cs++];
						if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
							{ bgbbtj_bt1c_memcpy8(ct, csa); }
						BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic_RGB24(
							ct, clra, clrb, j, dfl);
						ct+=32; csl+=32; csa+=8;
					}
					break;
				}
				break;
#endif

			default:
				break;
			}
			mode=nextmode;
			break;
		}
	}
	
	if(ret)return(ret);

	return(cs-ibuf);
}

BGBBTJ_API int BGBBTJ_BT1C_DecodeBlocksMB5C(BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *ibuf, int isz, byte *blks, byte *lblks, int xs, int ys, int dfl)
{
	byte *cs, *csl;
	byte *tbuf;
	int xs1, ys1, xs2, ys2;
	int i, j, k, n;

	xs1=(xs+3)/4;
	ys1=(ys+3)/4;
	n=xs1*ys1;

	if(ibuf[0]==0xE1)
	{
		i=(ibuf[1]<<16)|(ibuf[2]<<8)|(ibuf[3]<<0);
		if((ibuf[i+0]==0xE3) && (ibuf[i+4]=='A') && (ibuf[i+5]=='X'))
		{
			j=BGBBTJ_BT1C_DecodeBlocksBC4(ctx, ibuf+i, isz-i,
				ctx->adata, ctx->ladata, xs, ys, 8);
//			memcpy(ctx->ladata, ctx->adata, ctx->nblocks*8);
//			memcpy(blks, ctx->adata, ctx->nblocks*32);

			BGBBTJ_BT1C_DecodeBlocksMB5C_I(ctx, ibuf, isz,
				blks, lblks, ctx->adata, xs, ys,
				dfl|BGBBTJ_BTIC1C_DFL_HASAX);

//			j=BGBBTJ_BT1C_DecodeBlocksBC4(ctx, ibuf+i, isz-i,
//				blks, lblks, xs, ys, 32);
//			BGBBTJ_BT1C_DecodeBlocksMB5C_I(ctx, ibuf, isz,
//				blks, lblks, xs, ys, dfl|BGBBTJ_BTIC1C_DFL_HASAX);
			cs=ibuf+i+j;
			k=cs-ibuf;
		}else
		{
			i=BGBBTJ_BT1C_DecodeBlocksMB5C_I(ctx, ibuf, isz,
				blks, NULL, NULL, xs, ys, dfl);
			cs=ibuf+i;
			k=cs-ibuf;
		}
	}else if((ibuf[0]==0xE3) && (ibuf[4]=='Z') && (ibuf[5]=='I'))
	{
		i=(ibuf[1]<<16)|(ibuf[2]<<8)|(ibuf[3]<<0);
		j=((xs+3)>>2)*((ys+3)>>2)*3*32; k=0;

		BGBBTJ_BT1C_CheckSizeTBuf1(ctx, j);
		tbuf=ctx->tbuf1;
		
		BTLZA_DecodeStreamSzZl(ibuf+6, tbuf, i-6, j, &k, 0);
		BGBBTJ_BT1C_DecodeBlocksMB5C(ctx, tbuf, k, blks, lblks, xs, ys, dfl);
		k=i;
	}else if((ibuf[0]==0xE3) && (ibuf[4]=='J') && (ibuf[5]=='P'))
	{
		i=(ibuf[1]<<16)|(ibuf[2]<<8)|(ibuf[3]<<0);
		j=((xs+3)>>2)*((ys+3)>>2)*3*32; k=0;
		if(!ctx->jpg_ctx)
			ctx->jpg_ctx=BGBBTJ_JPG_AllocContext();
//		BGBBTJ_JPG_DecodeBufferClrsCtx(
//			ctx->jpg_ctx, ibuf+6, i-6,
//			blks, BGBBTJ_JPG_BC3_VF, &xs2, &ys2);
		BGBBTJ_JPG_DecodeBufferClrsCtx(
			ctx->jpg_ctx, ibuf+6, i-6,
			blks, BGBBTJ_JPG_MB5C_VF, &xs2, &ys2);
//		BGBBTJ_JPG_DecodeBufferClrs(ibuf+6, i-6,
//			blks, BGBBTJ_JPG_MB5C_VF, &xs2, &ys2);
		k=i;
		cs=ibuf+i;
	}else
	{
		return(-1);
	}
	return(k);
}
