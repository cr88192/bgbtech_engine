#include <bgbbtj.h>

extern byte bt1c_blkmap1[256];
extern byte bt1c_blkmap2[256];
extern byte bt1c_blkmap3[256];
extern byte bt1c_blkmap4[256];

extern byte bt1c_blkalphap[256];
extern byte bt1c_blkalphamape1[4096];
extern byte bt1c_blkalphamape2[4096];
extern u16 bt1c_difftab[8*256];		//differential color tables

extern u16 bt1c_blkidxcolor[256];		//default block index colors

BGBBTJ_API int BGBBTJ_BT1C_EncodeBlocksDXT1_LookupRun(
	byte *csc, byte *css, byte *cse,
	int *rlen, int *rdist, int fl)
{
	byte *cs, *cs1;
	int bi, bl, ml;
	int i, j, k;

	if(!(fl&BGBBTJ_BTIC1C_EFL_BLOCKLZ))
		{ *rlen=0; *rdist=0; return(0); }
		
	bi=0; bl=0; ml=256;
	cs=csc-8;
	while(cs>css)
	{
		i=(csc-cs)/8;
		if(i>=65536) { break; }
		else if(i>=4096) { ml=1; }
		else if(i>=1024) { ml=8; }
		else if(i>=256) { ml=32; }
		else if(i>=64) { ml=64; }
		else if(i>=16) { ml=128; }

		for(j=0; j<256; j++)
			if(memcmp(cs+j*8, csc+j*8, 8))
				break;
		if(j>bl) { bi=i; bl=j; }
		if(bl>=ml)break;
		cs-=8;
	}
	
	if(bl<16)
	{
//		if(bi>256)
//			return(0);
	}
	
	*rlen=bl;
	*rdist=bi;
	return(bl>0);
}

int BGBBTJ_BT1C_EncodeValueForDiffMode(
	BGBBTJ_BTIC1C_ImageInfo *ctx,
	int b, int d, int sc)
{
	static int sclt[8][4]=
	{
		{1,  2,  2, 0},
		{1,  3,  4, 0},
		{2,  4,  6, 0},
		{2,  6,  8, 0},
		{4,  8, 13, 0},
		{5, 13, 17, 0},
		{7, 17, 23, 0},
		{9, 23, 29, 0},
	};
	int i;

	i=2;
	if((d<0) && ((b-sclt[sc][0])>0))
	{
		i=1;
		if((d<=(-sclt[sc][1]) && ((b-sclt[sc][2])>0)))i=0;
	}else if((d>0) && ((b+sclt[sc][0])<31))
	{
		i=3;
		if((d>=(sclt[sc][1])) && ((b+sclt[sc][2])<31))i=4;
	}

	return(i);

#if 0
	int i;
	switch(sc)
	{
	case 0:
		i=2;
		if((d<0) && ((b-1)>=0))
		{
			i=1;
			if((d<=(-3) && ((b-4)>=0)))i=0;
			break;
		}
		if((d>0) && ((b+1)<31))
		{
			i=3;
			if((d>=( 3)) && ((b+4)<31))i=4;
			break;
		}
		break;
	case 1:
		i=2;
		if((d<0) && ((b-2)>=0))
		{
			i=1;
			if((d<=(-6) && ((b-8)>=0)))i=0;
			break;
		}
		if((d>0) && ((b+2)<31))
		{
			i=3;
			if((d>=( 6)) && ((b+8)<31))i=4;
			break;
		}
		break;
	case 2:
		i=2;
		if((d<0) && ((b-5)>=0))
		{
			i=1;
			if((d<=(-13) && ((b-17)>=0)))i=0;
			break;
		}
		if((d>0) && ((b+5)<31))
		{
			i=3;
			if((d>=( 13)) && ((b+17)<31))i=4;
			break;
		}
		break;
	case 3:
		i=2;
		if((d<0) && ((b-9)>=0))
		{
			i=1;
			if((d<=(-23) && ((b-29)>=0)))i=0;
			break;
		}
		if((d>0) && ((b+9)<31))
		{
			i=3;
			if((d>=( 23)) && ((b+29)<31))i=4;
			break;
		}
		break;
	default:
		i=2;
		break;
	}
	return(i);
#endif
}

int BGBBTJ_BT1C_EncodePredictDiffColor(
	BGBBTJ_BTIC1C_ImageInfo *ctx,
	int p, int v, int sc)
{
	int br, bg, bb;
	int cr, cg, cb;
	int dr, dg, db;
	int d2r, d2g, d2b;
	int i, j, k;

	if(ctx->diff_lclra==((u32)-1))
		return(-1);
	if(ctx->diff_lclrb==((u32)-1))
		return(-1);

	if(ctx->diff_lbmode!=0)
		return(-1);

//	p=BGBBTJ_BT1C_ConvRGB16ToRGB15(ctx, p);
	v=BGBBTJ_BT1C_ConvRGB16ToRGB15(ctx, v);

	br=(p>>10)&0x1F;
	bg=(p>> 5)&0x1F;
	bb=(p    )&0x1F;
	cr=(v>>10)&0x1F;
	cg=(v>> 5)&0x1F;
	cb=(v    )&0x1F;
	dr=cr-br;
	dg=cg-bg;
	db=cb-bb;
	
	d2r=BGBBTJ_BT1C_EncodeValueForDiffMode(ctx, br, dr, sc);
	d2g=BGBBTJ_BT1C_EncodeValueForDiffMode(ctx, bg, dg, sc);
	d2b=BGBBTJ_BT1C_EncodeValueForDiffMode(ctx, bb, db, sc);
	
	i=sc*256+((5*d2r+d2g)*5+d2b);
	j=p+bt1c_difftab[i];

	j=BGBBTJ_BT1C_ConvRGB15ToRGB16(ctx, j);
	
	return(j);
}

int BGBBTJ_BT1C_EncodePredictDiff(
	BGBBTJ_BTIC1C_ImageInfo *ctx,
	int p, int v, int sc)
{
	int br, bg, bb;
	int cr, cg, cb;
	int dr, dg, db;
	int d2r, d2g, d2b;
	int i, j, k;

//	p=BGBBTJ_BT1C_ConvRGB16ToRGB15(ctx, p);
	v=BGBBTJ_BT1C_ConvRGB16ToRGB15(ctx, v);

	br=(p>>10)&0x1F;
	bg=(p>> 5)&0x1F;
	bb=(p    )&0x1F;
	cr=(v>>10)&0x1F;
	cg=(v>> 5)&0x1F;
	cb=(v    )&0x1F;
	dr=cr-br;
	dg=cg-bg;
	db=cb-bb;
	
	d2r=BGBBTJ_BT1C_EncodeValueForDiffMode(ctx, br, dr, sc);
	d2g=BGBBTJ_BT1C_EncodeValueForDiffMode(ctx, bg, dg, sc);
	d2b=BGBBTJ_BT1C_EncodeValueForDiffMode(ctx, bb, db, sc);
	
	i=((5*d2r+d2g)*5+d2b);
	return(i);
}

int BGBBTJ_BT1C_EncodePredictDiffColorA(BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *blks, byte *cs,
	int xs1, int ys1, int fl, int stride, int sc)
{
	int p, v;
	int i, j, k;

	p=BGBBTJ_BT1C_PredictDiffColorModeA(ctx,
		blks, cs, xs1, ys1, stride, 4);
	v=cs[0]|(cs[1]<<8);
	i=BGBBTJ_BT1C_EncodePredictDiffColor(ctx, p, v, sc);
	return(i);
}

int BGBBTJ_BT1C_EncodePredictDiffColorB(
	BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *blks, byte *cs,
	int xs1, int ys1, int fl, int stride, int sc)
{
	int p, v;
	int i, j, k;

	p=BGBBTJ_BT1C_PredictDiffColorModeB(ctx,
		blks, cs, xs1, ys1, stride, 4);
	v=cs[2]|(cs[3]<<8);
	i=BGBBTJ_BT1C_EncodePredictDiffColor(ctx, p, v, sc);
	return(i);
}

int BGBBTJ_BT1C_EncodePredictDiffA(
	BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *blks, byte *cs,
	int xs1, int ys1, int fl, int stride, int sc)
{
	int p, v;
	int i, j, k;

	p=BGBBTJ_BT1C_PredictDiffColorModeA(ctx,
		blks, cs, xs1, ys1, stride, 4);
	v=cs[0]|(cs[1]<<8);
	i=BGBBTJ_BT1C_EncodePredictDiff(ctx, p, v, sc);
	return(i);
}

int BGBBTJ_BT1C_EncodePredictDiffB(
	BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *blks, byte *cs,
	int xs1, int ys1, int fl, int stride, int sc)
{
	int p, v;
	int i, j, k;

	p=BGBBTJ_BT1C_PredictDiffColorModeB(ctx, 
		blks, cs, xs1, ys1, stride, 4);
	v=cs[2]|(cs[3]<<8);
	i=BGBBTJ_BT1C_EncodePredictDiff(ctx, p, v, sc);
	return(i);
}

int BGBBTJ_BT1C_EncodeClearDiffForBlock(
	BGBBTJ_BTIC1C_ImageInfo *ctx)
{
	ctx->diff_lclra=(u32)-1;
	ctx->diff_lclrb=(u32)-1;
	ctx->diff_lbmode=-1;
	ctx->diff_sc=-1;
	return(0);
}

int BGBBTJ_BT1C_EncodeSetupDiffForBlock(
	BGBBTJ_BTIC1C_ImageInfo *ctx, byte *cs,
	int xs1, int ys1, int fl, int stride)
{
	u32 clra, clrb;
	int p, v;
	int i, j, k;

	if((cs[0]==0x55) || (cs[0]==0xAA))
	{
		BGBBTJ_BT1C_EncodeClearDiffForBlock(ctx);
		return(0);
	}

	if((cs[0]==0x55) || (cs[0]==0xAB))
	{
		BGBBTJ_BT1C_EncodeClearDiffForBlock(ctx);
		return(0);
	}

	if((cs[0]==0x55) || (cs[0]==0xAD))
	{
		BGBBTJ_BT1C_EncodeClearDiffForBlock(ctx);
		ctx->diff_lbmode=-1;
		return(0);
	}

	if(BGBBTJ_BT1C_CheckBlockColor_IsRGB23(ctx, cs))
	{
		clra=BGBBTJ_BT1C_GetBlockColorA_AsRGB24(ctx, cs);
		clrb=BGBBTJ_BT1C_GetBlockColorB_AsRGB24(ctx, cs);
		ctx->diff_lclra=clra;
		ctx->diff_lclrb=clrb;
		ctx->diff_lbmode=6;
	}else
	{
		clra=cs[0]|(cs[1]<<8);
		clrb=cs[2]|(cs[3]<<8);

		clra=BGBBTJ_BT1C_ConvRGB16ToRGB15(ctx, clra);
		clrb=BGBBTJ_BT1C_ConvRGB16ToRGB15(ctx, clrb);
		ctx->diff_lclra=clra;
		ctx->diff_lclrb=clrb;
		ctx->diff_lbmode=0;
	}
	return(0);
}

BGBBTJ_API int BGBBTJ_BT1C_EncodeBlocksDXT1(
	BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *obuf, int osz, byte *blks, byte *lblks,
	int xs, int ys, int fl)
{
	return(BGBBTJ_BT1C_EncodeBlocksDXTn(ctx,
		obuf, osz, blks, lblks, xs, ys, fl, 8));
}

BGBBTJ_API int BGBBTJ_BT1C_EncodeBlocksDXT5(
	BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *obuf, int osz, byte *blks, byte *lblks,
	int xs, int ys, int fl)
{
	int i, j;
	
	i=BGBBTJ_BT1C_EncodeBlocksDXTn(ctx,
		obuf, osz, blks+8, lblks?(lblks+8):NULL, xs, ys, fl, 16);
	j=BGBBTJ_BT1C_EncodeBlocksBC4(ctx,
		obuf+i, osz-i, blks, lblks, xs, ys,
		fl, 16);
//		fl|BGBBTJ_BTIC1C_EFL_BLOCKLZ, 16);
	return(i+j);
}

BGBBTJ_API int BGBBTJ_BT1C_EncodeBlocksMB5C(
	BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *obuf, int osz, byte *blks, byte *lblks,
	int xs, int ys, int fl)
{
	int i, j;
	
	if((ctx->clrs==BGBBTJ_JPG_RGB) ||
		(ctx->clrs==BGBBTJ_JPG_BGR) ||
		(ctx->clrs==BGBBTJ_JPG_RGBX) ||
		(ctx->clrs==BGBBTJ_JPG_BGRX))
	{
		i=BGBBTJ_BT1C_EncodeBlocksDXTn(ctx,
			obuf, osz, blks+8, lblks?(lblks+8):NULL, xs, ys, fl, 32);
		return(i);
	}
	
	i=BGBBTJ_BT1C_EncodeBlocksDXTn(ctx,
		obuf, osz, blks+8, lblks?(lblks+8):NULL, xs, ys, fl, 32);
	j=BGBBTJ_BT1C_EncodeBlocksBC4(ctx,
		obuf+i, osz-i, blks, lblks, xs, ys,
		fl, 32);
	return(i+j);
}

static int bt1c_stats[8];

BGBBTJ_API int BGBBTJ_BT1C_EncodeBlocksDXT1Z(
	BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *obuf, int osz, byte *blks, byte *lblks,
	int xs, int ys, int fl)
{
	byte *tbuf1, *tbuf2, *tbuf3;
	int sz1, sz2, sz3;
	int i, j, k;
	
	if(!(fl&BGBBTJ_BTIC1C_EFL_BLOCKZI))
	{
		sz1=BGBBTJ_BT1C_EncodeBlocksDXT1(ctx,
			obuf, osz, blks, lblks, xs, ys, fl);
		return(sz1);
	}
	
	if(!lblks)
	{
		printf("ZStat: Small=%d, NoDfl=%d, Dfl=%d, Dfl+A=%d JPG=%d\n",
			bt1c_stats[0], bt1c_stats[1],
			bt1c_stats[2], bt1c_stats[3],
			bt1c_stats[4]);
		for(i=0; i<8; i++)
			bt1c_stats[i]=0;
	}
	
	tbuf1=malloc(osz);
	tbuf2=malloc(osz);
	sz1=BGBBTJ_BT1C_EncodeBlocksDXT1(ctx,
		tbuf1, osz, blks, lblks, xs, ys, fl);
	
	if(sz1<256)
//	if(1)
	{
		bt1c_stats[0]++;
		memcpy(obuf, tbuf1, sz1);
		free(tbuf1);
		free(tbuf2);
		return(sz1);
	}

#if 0
//	sz2=PDZ2_EncodeStream64LvlZl(tbuf1, tbuf2+6, sz1, osz-6, 9);
//	sz2=PDZ2_EncodeStream64LvlZl(tbuf1, tbuf2+6, sz1, osz-6, 7);
//	sz2=PDZ2_EncodeStream64LvlZl(tbuf1, tbuf2+6, sz1, osz-6, 5);
//	sz2=PDZ2_EncodeStream64LvlZl(tbuf1, tbuf2+6, sz1, osz-6, 3);

	tbuf3=malloc(osz);
	sz3=PDZ2_EncodeStream64LvlZlTest(tbuf1, tbuf3, sz1, osz, 5);
	sz2=BGBBTJ_BitArith_EncodeTestData(tbuf3, sz3, tbuf2+6, osz-6);
	
	if((sz2>0) && (sz2<(0.85*sz1)) && (sz2<(0.85*sz3)))
	{
		bt1c_stats[3]++;
		sz2+=6;
		memcpy(obuf, tbuf2, sz2);
		obuf[0]=0xE3;			obuf[1]=(sz2>>16)&255;
		obuf[2]=(sz2>>8)&255;	obuf[3]=(sz2>> 0)&255;
		obuf[4]='Z';			obuf[5]='A';
		free(tbuf1);
		free(tbuf2);
		free(tbuf3);
		return(sz2);
	}
#if 0
	else
	{
		memcpy(obuf, tbuf1, sz1);
		free(tbuf1);
		free(tbuf2);
		free(tbuf3);
		return(sz1);
	}
#endif
	free(tbuf3);
#endif

#if 1
//	sz2=PDZ2_EncodeStream64LvlZl(tbuf1, tbuf2+6, sz1, osz-6, 9);
//	sz2=PDZ2_EncodeStream64LvlZl(tbuf1, tbuf2+6, sz1, osz-6, 7);
//	sz2=PDZ2_EncodeStream64LvlZl(tbuf1, tbuf2+6, sz1, osz-6, 5);
//	sz2=PDZ2_EncodeStream64LvlZl(tbuf1, tbuf2+6, sz1, osz-6, 3);
//	sz2=PDZ2_EncodeStream64LvlZlTest(tbuf1, tbuf2+6, sz1, osz-6, 3);
//	sz2=BTLZA_BitEnc_EncodeStreamXLvlZlTest(tbuf1, tbuf2+6, sz1, osz-6, 3);
	sz2=BTLZA_BitEnc_EncodeStreamXLvlZlTest(tbuf1, tbuf2+6, sz1, osz-6, 5);
	if((sz2>0) && (sz2<(0.85*sz1)))
	{
		bt1c_stats[2]++;
		sz2+=6;
		memcpy(obuf, tbuf2, sz2);
		obuf[0]=0xE3;			obuf[1]=(sz2>>16)&255;
		obuf[2]=(sz2>>8)&255;	obuf[3]=(sz2>> 0)&255;
		obuf[4]='Z';			obuf[5]='I';
		free(tbuf1);
		free(tbuf2);
		return(sz2);
	}else
	{
		bt1c_stats[1]++;
		memcpy(obuf, tbuf1, sz1);
		free(tbuf1);
		free(tbuf2);
		return(sz1);
	}
#endif
}

BGBBTJ_API int BGBBTJ_BT1C_EncodeBlocksDXT5Z(
	BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *obuf, int osz, byte *blks, byte *lblks,
	int xs, int ys, int fl)
{
	byte *tbuf1, *tbuf2;
	int sz1, sz2;
	
	if(!(fl&BGBBTJ_BTIC1C_EFL_BLOCKZI))
	{
		sz1=BGBBTJ_BT1C_EncodeBlocksDXT5(ctx,
			obuf, osz, blks, lblks, xs, ys, fl);
		return(sz1);
	}
	
	tbuf1=malloc(osz);
	tbuf2=malloc(osz);
	sz1=BGBBTJ_BT1C_EncodeBlocksDXT5(ctx,
		tbuf1, osz, blks, lblks, xs, ys, fl);
	
	if(sz1<256)
//	if(1)
	{
		memcpy(obuf, tbuf1, sz1);
		free(tbuf1);
		free(tbuf2);
		return(sz1);
	}
	
//	sz2=PDZ2_EncodeStream64LvlZl(tbuf1, tbuf2+6, sz1, osz-6, 9);
//	sz2=PDZ2_EncodeStream64LvlZl(tbuf1, tbuf2+6, sz1, osz-6, 5);
//	sz2=PDZ2_EncodeStream64LvlZl(tbuf1, tbuf2+6, sz1, osz-6, 3);
	sz2=BTLZA_BitEnc_EncodeStreamXLvlZlTest(tbuf1, tbuf2+6, sz1, osz-6, 3);
	sz2+=6;
	if(sz2<(0.85*sz1))
	{
		memcpy(obuf, tbuf2, sz2);
		obuf[0]=0xE3;			obuf[1]=(sz2>>16)&255;
		obuf[2]=(sz2>>8)&255;	obuf[3]=(sz2>> 0)&255;
		obuf[4]='Z';			obuf[5]='I';
		free(tbuf1);
		free(tbuf2);
		return(sz2);
	}else
	{
		memcpy(obuf, tbuf1, sz1);
		free(tbuf1);
		free(tbuf2);
		return(sz1);
	}
}

BGBBTJ_API int BGBBTJ_BT1C_EncodeBlocksMB5CZ(
	BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *obuf, int osz, byte *blks, byte *lblks,
	int xs, int ys, int fl)
{
	byte *tbuf1, *tbuf2;
	int sz1, sz2;
	
	if(!(fl&BGBBTJ_BTIC1C_EFL_BLOCKZI))
	{
		sz1=BGBBTJ_BT1C_EncodeBlocksMB5C(ctx,
			obuf, osz, blks, lblks, xs, ys, fl);
		return(sz1);
	}
	
	tbuf1=malloc(osz);
	tbuf2=malloc(osz);
	sz1=BGBBTJ_BT1C_EncodeBlocksMB5C(ctx,
		tbuf1, osz, blks, lblks, xs, ys, fl);
	
	if(sz1<256)
//	if(1)
	{
		memcpy(obuf, tbuf1, sz1);
		free(tbuf1);
		free(tbuf2);
		return(sz1);
	}
	
//	sz2=PDZ2_EncodeStream64LvlZl(tbuf1, tbuf2+6, sz1, osz-6, 9);
//	sz2=PDZ2_EncodeStream64LvlZl(tbuf1, tbuf2+6, sz1, osz-6, 5);
//	sz2=PDZ2_EncodeStream64LvlZl(tbuf1, tbuf2+6, sz1, osz-6, 3);
	sz2=BTLZA_BitEnc_EncodeStreamXLvlZlTest(tbuf1, tbuf2+6, sz1, osz-6, 3);
	sz2+=6;
	if(sz2<(0.85*sz1))
	{
		memcpy(obuf, tbuf2, sz2);
		obuf[0]=0xE3;			obuf[1]=(sz2>>16)&255;
		obuf[2]=(sz2>>8)&255;	obuf[3]=(sz2>> 0)&255;
		obuf[4]='Z';			obuf[5]='I';
		free(tbuf1);
		free(tbuf2);
		return(sz2);
	}else
	{
		memcpy(obuf, tbuf1, sz1);
		free(tbuf1);
		free(tbuf2);
		return(sz1);
	}
}

int BGBBTJ_BT1C_EncodeBlocksDXTn_CheckModeColorsCompatible(
	BGBBTJ_BTIC1C_ImageInfo *ctx, int dmode, int smode)
{
	int i;

	switch(dmode)
	{
	case 0: case 4: case 5: case 14:
		switch(smode)
		{
		case 0: case 4: case 5: case 14:
			i=1; break;
		default: i=0; break;
		}
		break;
	case 1: case 2: case 3:
		switch(smode)
		{
		case 1: case 2: case 3:
			i=1; break;
		default: i=0; break;
		}
		break;
	case 6: case 10: case 11: case 12:
		switch(smode)
		{
		case 6: case 10: case 11: case 12:
			i=1; break;
		default: i=0; break;
		}
		break;
	case 9: case 15:
		switch(smode)
		{
		case 9: case 15:
			i=1; break;
		default: i=0; break;
		}
		break;
	default:
		i=0;
		break;
	}
	return(i);
}

byte *BGBBTJ_BT1C_EncodeBlocksDXTn_EmitModeEscapeColorOnly(
	BGBBTJ_BTIC1C_ImageInfo *ctx, byte *ct, int mode)
{
	int i;

	//current mode, no prefix needed
	if(mode==ctx->curmode)
	{
		ctx->modecnt[mode]++;
		return(ct);
	}

	i=BGBBTJ_BT1C_EncodeBlocksDXTn_CheckModeColorsCompatible(
		ctx, mode, ctx->curmode);

	//modes have compatible colors, no prefix needed.
	if(i)
	{
		ctx->modecnt[mode]++;
		return(ct);
	}

	//emit mode escapes
	ct=BGBBTJ_BT1C_EncodeBlocksDXTn_EmitModeEscape(ctx, ct, mode);
	return(ct);
}

byte *BGBBTJ_BT1C_EncodeBlocksDXTn_EmitModeEscape(
	BGBBTJ_BTIC1C_ImageInfo *ctx, byte *ct, int mode)
{
	int i, j, k, l;

	ctx->modecnt[mode]++;

	//current mode, no prefix needed
	if(mode==ctx->curmode)
		{ return(ct); }

	i=BGBBTJ_BT1C_EncodeBlocksDXTn_CheckModeColorsCompatible(
		ctx, mode, ctx->curmode);
	if(!i)
	{
		ctx->diff_lclra=(u32)-1;
		ctx->diff_lclrb=(u32)-1;
	}

	//check if one of the in-use modes.
	for(i=0; i<8; i++)
	{
		if(ctx->modes[i]==mode)
		{
			if((ctx->curmode<0) ||
				(ctx->modecnt[mode]>ctx->modecnt[ctx->curmode]))
//			if(0)
			{
				if((i<3) && ((i>0) || (ctx->curmode>0)))
				{
					*ct++=0xE0+i;
					ctx->curmode=mode;
					return(ct);
				}
				
				//switch mode
				*ct++=0xF0;
				*ct++=0xF0+i;
				ctx->curmode=mode;
				return(ct);
			}
		
			//use mode
			*ct++=0xF0+i;
			return(ct);
		}
	}

#if 1
	i=1;
	for(j=1; j<8; j++)
	{
		if(ctx->modecnt[ctx->modes[j]]<ctx->modecnt[ctx->modes[i]])
			i=j;
	}

	if(ctx->modecnt[mode]>ctx->modecnt[ctx->modes[i]])
	{
		if((ctx->curmode<0) ||
			(ctx->modecnt[mode]>ctx->modecnt[ctx->curmode]))
		{
			//Set preset and switch mode
			ctx->modes[i]=mode;
			*ct++=0xEC;
			*ct++=0x30|i;
			*ct++=mode;
			ctx->curmode=mode;
			return(ct);
		}

		//Set preset and use as prefix
		ctx->modes[i]=mode;
		*ct++=0xEC;
		*ct++=0x28|i;
		*ct++=mode;
		return(ct);

#if 0
		//assign mode to preset
		j=(i<<12)|mode;
		*ct++=0xF8;
		*ct++=0x8A;
		*ct++=(j>>8)&255;
		*ct++=j&255;
		//check and switch mode
		if((ctx->curmode<0) ||
			(ctx->modecnt[mode]>ctx->modecnt[ctx->curmode]))
		{
			if((i<3) && ((i>0) || (ctx->curmode>0)))
			{
				*ct++=0xE0+i;
				ctx->curmode=mode;
				return(ct);
			}

			//switch mode
			*ct++=0xF0;
			*ct++=0xF0+i;
			ctx->curmode=mode;
			return(ct);
		}

		//use mode
		*ct++=0xF0+i;
		return(ct);
#endif
	}
#endif

#if 0
	//see if the mode can be made in-use
	for(i=1; i<8; i++)
	{
		if(ctx->modecnt[mode]>ctx->modecnt[ctx->modes[i]])
		{
			//assign mode to preset
			j=(i<<12)|mode;
			*ct++=0xF8;
			*ct++=0x8A;
			*ct++=(j>>8)&255;
			*ct++=j&255;

			//check and switch mode
			if((ctx->curmode<0) ||
				(ctx->modecnt[mode]>ctx->modecnt[ctx->curmode]))
			{
				//switch mode
				*ct++=0xF0;
				*ct++=0xF0+i;
				ctx->curmode=mode;
				return(ct);
			}

			//use mode
			*ct++=0xF0+i;
			return(ct);
		}
	}
#endif

	if(mode<24)
	{
		*ct++=0xEF;
		*ct++=0xC0|mode;
		return(ct);
	}

	if(mode<1024)
	{
		*ct++=0xEF;
		*ct++=0xD8+(mode>>8);
		*ct++=mode&255;
		return(ct);
	}

	*ct++=0xEF;
	*ct++=0xDC;
	*ct++=(mode>>8)&255;
	*ct++=mode&255;
	return(ct);
}

byte *BGBBTJ_BT1C_EncodeBlocksDXTn_EmitAlphaModeEscape(
	BGBBTJ_BTIC1C_ImageInfo *ctx, byte *ct, int mode)
{
	int i, j, k, l;

	ctx->amodecnt[mode]++;

	//current mode, no prefix needed
	if(mode==ctx->curmode)
		{ return(ct); }

	//check if one of the in-use modes.
	for(i=0; i<8; i++)
	{
		if(ctx->amodes[i]==mode)
		{
			if((ctx->curmode<0) ||
				(ctx->amodecnt[mode]>ctx->amodecnt[ctx->curmode]))
			{
//				if(i<3)
				if((i<3) && ((i>0) || (ctx->curmode>0)))
				{
					*ct++=0xE0+i;
					ctx->curmode=mode;
					return(ct);
				}
				
				//switch mode
				*ct++=0xF0;
				*ct++=0xF0+i;
				ctx->curmode=mode;
				return(ct);
			}
		
			//use mode
			*ct++=0xF0+i;
			return(ct);
		}
	}

#if 1
	//see if the mode can be made in-use
	for(i=1; i<8; i++)
	{
		if(ctx->amodecnt[mode]>ctx->amodecnt[ctx->modes[i]])
		{
			//assign mode to preset
			ctx->amodes[i]=mode;
			j=(i<<12)|mode;
			*ct++=0xF8;
			*ct++=0x8A;
			*ct++=(j>>8)&255;
			*ct++=j&255;

			//check and switch mode
			if((ctx->curmode<0) ||
				(ctx->amodecnt[mode]>ctx->amodecnt[ctx->curmode]))
			{
				//switch mode
				*ct++=0xF0;
				*ct++=0xF0+i;
				ctx->curmode=mode;
				return(ct);
			}

			//use mode
			*ct++=0xF0+i;
			return(ct);
		}
	}
#endif

	if(mode<24)
	{
		*ct++=0xEF;
		*ct++=0xC0+mode;
		return(ct);
	}

	if(mode<1024)
	{
		*ct++=0xEF;
		*ct++=0xD8+(mode>>8);
		*ct++=mode&255;
		return(ct);
	}

	*ct++=0xEF;
	*ct++=0xDC;
	*ct++=(mode>>8)&255;
	*ct++=mode&255;
	return(ct);
}

BGBBTJ_API int BGBBTJ_BT1C_EncodeBlocksDXTn(
	BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *obuf, int osz, byte *blks, byte *lblks,
	int xs, int ys, int fl, int stride)
{
	static const int mvtab[32]=
	{
	-1, -1,	 0, -1,	 1, -1,  1,  0,
	 1,  1,  0,  1,	-1,  1, -1,  0,
	-2,  0,  0, -2,	 2,  0,  0,  2,
	-3,  0,  0, -3,	 3,  0,  0,  3,
	};

	static int stats[256];

	byte tb[16];
	byte *cs, *cse, *csm, *csl, *csle;
	byte *ct, *cte, *ctb, *lct0;
	byte *cs1, *cs2;
	int xs1, ys1, n, n1;
	int xs2, ys2, xs3, ys3;
	int p0, p1, p2, p3, p4, p5, p6, p7;
	int i0, i1, i2, i3;
	int i4, i5, i6, i7;
	int pa0, pa1, pa2, pa3;
	int pb0, pb1, pb2, pb3;
	int is1bpp, is2x2, as1bpp, as2x2;
	int bx, by, bk;
	int bl, bd, lbjp;
	int i, j, k, l;
	
	BGBBTJ_BT1C_Init();

	ctx->eflags=fl;
	ctx->eflags&=~BGBBTJ_BTIC1C_EFL_BLKMB1;
	if(stride==32)
		ctx->eflags|=BGBBTJ_BTIC1C_EFL_BLKMB1;

	xs1=(xs+3)/4;
	ys1=(ys+3)/4;
	n=xs1*ys1;
	
	if(fl&BGBBTJ_BTIC1C_EFL_MIPMAP)
	{
		xs2=xs; ys2=ys; n=0;
		while((xs2>1) || (ys2>1))
		{
			if(!xs2)xs2++;
			if(!ys2)ys2++;
			xs3=(xs2+3)/4;
			ys3=(ys2+3)/4;
			n+=xs3*ys3;
			xs2>>=1; ys2>>=1;
		}
	}

	cs=blks; cse=cs+n*stride;
	csl=lblks; csle=csl+n*stride;
	ct=obuf;

	lct0=ct;

	*ct++=0xE1;		*ct++=0x00;
	*ct++=0x00;		*ct++=0x00;

	ctx->diff_m=0;
	ctx->diff_sc=-1;
	ctx->diff_lsc=-1;

//	if(fl&BGBBTJ_BTIC1C_EFL_DIFFCLR)
//		{ *ct++=0xF8; *ct++=0x8F; *ct++=0x02;
//		  *ct++=0xF8; *ct++=0x90; }

//	if(fl&BGBBTJ_BTIC1C_EFL_DIFFCLR)
//		{ *ct++=0xF8; *ct++=0x8F; *ct++=0x03;
//		  *ct++=0xF8; *ct++=0x90; }

//	if(fl&BGBBTJ_BTIC1C_EFL_DIFFCLR)
	if(1)
		{ *ct++=0xF8; *ct++=0x8F; *ct++=0x04;
		  *ct++=0xF8; *ct++=0x90; }

//	if(fl&BGBBTJ_BTIC1C_EFL_DIFFCLR)
//		{ *ct++=0xF8; *ct++=0x8F; *ct++=0x01;
//		  *ct++=0xF8; *ct++=0x90; }

	lbjp=(ctx->stflags&BGBBTJ_BTIC1C_STFL_LBJPEG);

	if(ctx->pal_clr && (!lblks || lbjp))
	{
		*ct++=0xF8; *ct++=0x86;
		*ct++=0x00; *ct++=0xFF;

		for(i=0; i<256; i++)
		{
			j=ctx->pal_clr[i];
			p0=(j>>16)&0xFF;
			p1=(j>> 8)&0xFF;
			p2=(j    )&0xFF;
			p3=((p0&p1)&1)|((p1&p2)&1)|((p0&p2)&1);
			
//			k=(p3<<22)|(p0<<15)|(p1<<7)|p2;
			k=(p3<<22)|
				((p0&0xFE)<<14)|
				((p1&0xFF)<< 7)|
				((p2&0xFE)>> 1);
			*ct++=(k>>16)&255;
			*ct++=(k>> 8)&255;
			*ct++=(k>> 0)&255;
		}
	}

	ctx->curmode=ctx->modes[0];
//	ctx->diff_lclra=(u32)-1;
//	ctx->diff_lclrb=(u32)-1;
	ctx->diff_lclra=0;
	ctx->diff_lclrb=0;
	ctx->diff_lclrc=0;
	ctx->diff_lclrd=0;

#if 0
	if((fl&BGBBTJ_BTIC1C_EFL_EXTMODES) && (!lblks || lbjp))
	{
		for(i=0; i<8; i++)
			{ ctx->modes[i]=i; }
		for(i=0; i<256; i++)
			{ ctx->modecnt[i]=0; }

		for(i=0; i<8; i++)
		{
			j=(i<<12)|i;
			*ct++=0xF8;
			*ct++=0x8A;
			*ct++=(j>>8)&255;
			*ct++=j&255;
		}

//		*ct++=0xF0;
//		*ct++=0xF0;
		ctx->curmode=-1;
	}
#endif

#if 1
	if((fl&BGBBTJ_BTIC1C_EFL_EXTMODES) && (!lblks || lbjp))
	{
		for(i=0; i<8; i++)
			{ ctx->modes[i]=i; }
		for(i=0; i<256; i++)
			{ ctx->modecnt[i]=0; }

		*ct++=0xF8;
		*ct++=0x8B;
		ctx->curmode=0;
	}
#endif

//	ctx->curmode=0;
//	ctx->curmode=ctx->modes[0];

	if(fl&BGBBTJ_BTIC1C_EFL_EXTMODES)
	{
//		*ct++=0xF8; *ct++=0xA2;
//		*ct++=0xF8; *ct++=0xA3;
	}

//	for(i=0; i<256; i++)
//		stats[i]=0;

	while(cs<cse)
	{
		if(lct0[0]<0xE0)
		{
			if(lct0[0]<0x80)
				{ stats[0]++; }
			else
				{ stats[lct0[0]&0xE0]++; }
		}
		else
			{ stats[lct0[0]]++; }

		lct0=ct;

#if 0
		if(lblks && !memcmp(cs, csl, 8))
		{
			for(i=1; (i<32) && ((cs+i*stride)<cse); i++)
			{
				if(memcmp(cs+i*stride, csl+i*stride, 8))
					break;
			}

			*ct++=0x80+(i-1);
			cs+=i*stride; csl+=i*stride;
			continue;
		}
#endif

		if(fl&BGBBTJ_BTIC1C_EFL_EXTMODES)
		{
#if 1
			if(lblks && !memcmp(cs, csl, 8))
			{
				for(i=1; (i<8192) && ((cs+i*stride)<cse); i++)
				{
					if(memcmp(cs+i*stride, csl+i*stride, 8))
						break;
				}

				if(i>32)
				{
					j=i-1;
					*ct++=0xEE;
					*ct++=0x00+(j>>8);
					*ct++=j&255;
					cs+=i*stride; csl+=i*stride;
					BGBBTJ_BT1C_EncodeClearDiffForBlock(ctx);
					continue;
				}

				*ct++=0x80+(i-1);
				cs+=i*stride; csl+=i*stride;
				BGBBTJ_BT1C_EncodeClearDiffForBlock(ctx);
				continue;
			}
#endif
		}else
		{
#if 1
			if(lblks && !memcmp(cs, csl, 8))
			{
				for(i=1; (i<32) && ((cs+i*stride)<cse); i++)
				{
					if(memcmp(cs+i*stride, csl+i*stride, 8))
						break;
				}

				*ct++=0x80+(i-1);
				cs+=i*stride; csl+=i*stride;
				BGBBTJ_BT1C_EncodeClearDiffForBlock(ctx);
				continue;
			}
#endif
		}
		
		if(lblks && (fl&BGBBTJ_BTIC1C_EFL_MOTION))
		{
			bx=0; by=0; bk=0;
#if 0
			for(i=-2; i<=2; i++)
			{
				for(j=-2; j<=2; j++)
				{
					cs1=csl+(i*xs1*stride)+j*stride;
					if(cs1<lblks)continue;
					if(cs1>=csle)continue;

					k=0;
					if(!memcmp(cs, cs1, 8))
					{
						for(k=1; (k<32) && ((cs+k*stride)<cse); k++)
						{
							if(memcmp(cs+k*stride, cs1+k*stride, 8))
								break;
						}
					}
					if(k>bk)
						{ bx=j; by=i; bk=k; }
				}
			}
#endif

#if 1
			for(i=0; i<16; i++)
			{
				cs1=csl+
					(mvtab[i*2+1]*xs1*stride)+
					(mvtab[i*2+0]*stride);
				if(cs1<lblks)continue;
				if(cs1>=csle)continue;

				k=0;
				if(!memcmp(cs, cs1, 8))
				{
					for(k=1; (k<32) && ((cs+k*stride)<cse); k++)
					{
						if(memcmp(cs+k*stride, cs1+k*stride, 8))
							break;
					}
					if(k>bk)
						{ bx=mvtab[i*2+0]; by=mvtab[i*2+1]; bk=k; }
				}
			}
#endif

			if(bk>0)
			{
				if(bk>32)bk=32;
			
				*ct++=0xEE;
				*ct++=0x20+(bk-1);
				*ct++=((by+8)<<4)|(bx+8);

				cs+=bk*stride; csl+=bk*stride;
				BGBBTJ_BT1C_EncodeClearDiffForBlock(ctx);
				continue;
			}
		}

		i0=cs[0]|(cs[1]<<8);
		i1=cs[2]|(cs[3]<<8);
		BGBBTJ_BT1C_GetBlockColorsExtAB(ctx, cs, &i4, &i5);

		is1bpp=0;
		is2x2=0;

		if(fl&BGBBTJ_BTIC1C_EFL_EXTMODES)
		{
			is1bpp=(!(cs[4]&0xAA)) && (!(cs[5]&0xAA)) &&
				(!(cs[6]&0xAA)) && (!(cs[7]&0xAA));
			is2x2=((cs[4]==cs[5]) && (cs[6]==cs[7]) &&
				(((cs[4]>>2)&0x33)==(cs[4]&0x33)) &&
				(((cs[6]>>2)&0x33)==(cs[7]&0x33)));

			if(is1bpp)
			{
				p0=((cs[4]&1)<<3)|(cs[4]&4)|((cs[4]&16)>>3)|((cs[4]&64)>>6);
				p1=((cs[5]&1)<<3)|(cs[5]&4)|((cs[5]&16)>>3)|((cs[5]&64)>>6);
				p2=((cs[6]&1)<<3)|(cs[6]&4)|((cs[6]&16)>>3)|((cs[6]&64)>>6);
				p3=((cs[7]&1)<<3)|(cs[7]&4)|((cs[7]&16)>>3)|((cs[7]&64)>>6);
				as1bpp=(p0<<12)|(p1<<8)|(p2<<4)|p3;
			}

			if(is2x2)
			{
				p0=((cs[4]&0x03)<<2)|((cs[4]&0x30)>>4);
				p1=((cs[6]&0x03)<<2)|((cs[6]&0x30)>>4);
				as2x2=(p0<<4)|p1;
			}

#if 0			
			//for now...
			if(BGBBTJ_BT1C_CheckBlockColor_IsRGB23(ctx, cs))
			{
				is1bpp=false;
				is2x2=false;
			}
#endif
		}

#if 0
		if(BGBBTJ_BT1C_EncodeBlocksDXT1_LookupRun(
			cs, blks, cse, &bl, &bd, fl))
//		if((i0!=i1) && BGBBTJ_BT1C_EncodeBlocksDXT1_LookupRun(
//			cs, blks, cse, &bl, &bd, fl))
		{
			if((bl==1) && (bd==1))
				{ *ct++=0xE8; cs+=bl*stride; csl+=bl*stride; continue; }
			if((bl<=256) && (bd==1))
				{ *ct++=0xE9; *ct++=bl-1;
				cs+=bl*stride; csl+=bl*stride; continue; }
			if((bl==1) && (bd<=256))
				{ *ct++=0xEA; *ct++=bd-1;
				cs+=bl*stride; csl+=bl*stride; continue; }
			if((bl==1) && (bd<=65536))
			{
				j=bd-1; *ct++=0xEB;
				*ct++=(j>>8)&255; *ct++=j&255;
				cs+=bl*stride; csl+=bl*stride; continue;
			}
			if((bl<=256) && (bd<=256))
				{ *ct++=0xEC; *ct++=bl-1; *ct++=bd-1;
				cs+=bl*stride; csl+=bl*stride; continue; }
			if((bl<=256) && (bd<=65536))
			{
				*ct++=0xED; *ct++=bl-1;
				j=bd-1; *ct++=(j>>8)&255; *ct++=j&255;
				cs+=bl*stride; csl+=bl*stride; continue;
			}
		}
#endif

		if((ctx->eflags&BGBBTJ_BTIC1C_EFL_BLKMB1) &&
			(cs[0]==0x55) && (cs[2]==0xAB))
		{
			BGBBTJ_BT1C_EncodeSetupDiffForBlock(
				ctx, cs, xs1, ys1, fl, stride);

			csm=bt1c_blkmap3;

//			ct=BGBBTJ_BT1C_EncodeBlocksDXTn_EmitModeEscapeColorOnly(
//				ctx, ct, 6);

			i0=BGBBTJ_BT1C_GetBlockColorA_AsRGB23(ctx, cs);
			i1=BGBBTJ_BT1C_GetBlockColorB_AsRGB23(ctx, cs);
			i2=BGBBTJ_BT1C_GetBlockColorC_AsRGB23(ctx, cs);
			i3=BGBBTJ_BT1C_GetBlockColorD_AsRGB23(ctx, cs);
			
			i2|=0x800000;
			
			ct=BGBBTJ_BT1C_EncodeBlocksDXTn_EmitModeEscape(
				ctx, ct, 6);

//			n1=i;
//			if(i>1)
//				{ *ct++=0xC0+(i-1); }

			ct[0]=(i0>>16)&0xFF;
			ct[1]=(i0>>8)&0xFF;
			ct[2]=i0&0xFF;
			ct+=3;
			ct[0]=(i1>>16)&0xFF;
			ct[1]=(i1>>8)&0xFF;
			ct[2]=i1&0xFF;
			ct+=3;
			ct[0]=(i2>>16)&0xFF;
			ct[1]=(i2>>8)&0xFF;
			ct[2]=i2&0xFF;
			ct+=3;
			ct[0]=(i3>>16)&0xFF;
			ct[1]=(i3>>8)&0xFF;
			ct[2]=i3&0xFF;
			ct+=3;
			
			*ct++=0xC0;
			
			ct[0]=cs[11];
			ct[1]=cs[15];
			ct[2]=cs[19];
			ct[3]=cs[23];
			ct+=4;
			
			ct[0]=csm[cs[4]]; ct[1]=csm[cs[5]];
			ct[2]=csm[cs[6]]; ct[3]=csm[cs[7]];
			ct+=4;

			ctx->diff_sc=-1;

			cs+=stride; csl+=stride;
//			BGBBTJ_BT1C_EncodeSetupDiffForBlock(
//				ctx, cs-stride, xs1, ys1, fl, stride);
			continue;
		}

		if((ctx->eflags&BGBBTJ_BTIC1C_EFL_BLKMB1) &&
			(cs[0]==0x55) && (cs[2]==0xAD))
		{
			BGBBTJ_BT1C_EncodeSetupDiffForBlock(
				ctx, cs, xs1, ys1, fl, stride);

			csm=bt1c_blkmap3;

			i0=BGBBTJ_BT1C_GetBlockColorA_AsRGB31FS(ctx, cs);
			i1=BGBBTJ_BT1C_GetBlockColorB_AsRGB31FS(ctx, cs);
			i2=BGBBTJ_BT1C_GetBlockColorC_AsRGB31FS(ctx, cs);
			i3=BGBBTJ_BT1C_GetBlockColorD_AsRGB31FS(ctx, cs);
			
			i2|=0x80000000;
			
			ct=BGBBTJ_BT1C_EncodeBlocksDXTn_EmitModeEscape(
				ctx, ct, 7);

			ct[0]=(i0>>24)&0xFF;
			ct[1]=(i0>>16)&0xFF;
			ct[2]=(i0>> 8)&0xFF;
			ct[3]=(i0    )&0xFF;
			ct+=4;
			ct[0]=(i1>>24)&0xFF;
			ct[1]=(i1>>16)&0xFF;
			ct[2]=(i1>> 8)&0xFF;
			ct[3]=(i1    )&0xFF;
			ct+=4;
			ct[0]=(i2>>24)&0xFF;
			ct[1]=(i2>>16)&0xFF;
			ct[2]=(i2>> 8)&0xFF;
			ct[3]=(i2    )&0xFF;
			ct+=4;
			ct[0]=(i3>>24)&0xFF;
			ct[1]=(i3>>16)&0xFF;
			ct[2]=(i3>> 8)&0xFF;
			ct[3]=(i3    )&0xFF;
			ct+=4;
			
//			*ct++=0xC0;
			*ct++=0xD2;
			*ct++=cs[1];
						
			ct[0]=csm[cs[4]]; ct[1]=csm[cs[5]];
			ct[2]=csm[cs[6]]; ct[3]=csm[cs[7]];
			ct+=4;

			cs+=stride; csl+=stride;
//			BGBBTJ_BT1C_EncodeSetupDiffForBlock(
//				ctx, cs-stride, xs1, ys1, fl, stride);
			continue;
		}


#if 1
		//Floating-Point Color Blocks
		if((fl&BGBBTJ_BTIC1C_EFL_EXTMODES) &&
			(cs[0]==0x55) && (cs[2]==0xA9))
		{
			i0=BGBBTJ_BT1C_GetBlockColorA_AsRGB31FS(ctx, cs);
			i1=BGBBTJ_BT1C_GetBlockColorB_AsRGB31FS(ctx, cs);

			if(i0==i1)
			{
				for(i=0; (i<32) && ((cs+i*stride)<cse); i++)
				{
					if(lblks && !memcmp(cs+i*stride, csl+i*stride, 8))
						break;
					j=i*stride;
					i2=BGBBTJ_BT1C_GetBlockColorA_AsRGB31FS(ctx, cs+j);
					i3=BGBBTJ_BT1C_GetBlockColorB_AsRGB31FS(ctx, cs+j);
					if((i2!=i0) || (i2!=i3))
						break;
				}
				if(i>0)
				{
					BGBBTJ_BT1C_EncodeSetupDiffForBlock(
						ctx, cs, xs1, ys1, fl, stride);

					if(BGBBTJ_BT1C_BlockColorAB_FloatUnsignedP(ctx, cs))
					{
						i0=BGBBTJ_BT1C_GetBlockColorA_AsRGB31FU(ctx, cs);
						i1=BGBBTJ_BT1C_GetBlockColorB_AsRGB31FU(ctx, cs);
						l=8;
					}else
					{
						l=7;
					}
					
					ct=BGBBTJ_BT1C_EncodeBlocksDXTn_EmitModeEscapeColorOnly(
						ctx, ct, l);
					*ct++=0xA0+(i-1);
					*ct++=(i0>>24)&0xFF;
					*ct++=(i0>>16)&0xFF;
					*ct++=(i0>> 8)&0xFF;
					*ct++=(i0    )&0xFF;

//					*ct++=(i0>>8)&0xFF;
					cs+=i*stride; csl+=i*stride;
//					BGBBTJ_BT1C_EncodeSetupDiffForBlock(
//						ctx, cs-stride, xs1, ys1, fl, stride);
					continue;
				}
			}

			csm=bt1c_blkmap3;

			for(i=1; (i<32) && ((cs+i*stride)<cse); i++)
			{
				if(lblks && !memcmp(cs+i*stride, csl+i*stride, 8))
					break;

				j=i*stride;
				i2=BGBBTJ_BT1C_GetBlockColorA_AsRGB31FS(ctx, cs+j);
				i3=BGBBTJ_BT1C_GetBlockColorB_AsRGB31FS(ctx, cs+j);
				if((i2!=i0) || (i3!=i1))
					break;

				if(is2x2)
				{
					k=((cs[j+4]==cs[j+5]) && (cs[j+6]==cs[j+7]) &&
						(((cs[j+4]>>2)&0x33)==(cs[j+4]&0x33)) &&
						(((cs[j+6]>>2)&0x33)==(cs[j+7]&0x33)));
					if(!k)break;
				}else if(is1bpp)
				{
					k=(!(cs[j+4]&0xAA)) && (!(cs[j+5]&0xAA)) &&
						(!(cs[j+6]&0xAA)) && (!(cs[j+7]&0xAA));
					if(!k)break;
				}
			}

#if 0
			if(is2x2)
			{
				if((i>1) || (((i0>>8)&0xFF)>=128))
				{
					BGBBTJ_BT1C_EncodeSetupDiffForBlock(
						ctx, cs, xs1, ys1, fl, stride);

					ct=BGBBTJ_BT1C_EncodeBlocksDXTn_EmitModeEscape(
						ctx, ct, 2);

					n1=i;
					*ct++=0xC0+(i-1);
					ct[0]=(i0>>8)&0xFF;
					ct[1]=(i1>>8)&0xFF;
					ct+=2;

					for(i=0; i<n1; i++)
					{
						p0=(cs[4]&0x03)|((cs[4]&0x30)>>2);
						p1=(cs[6]&0x03)|((cs[6]&0x30)>>2);
						p2=(p1<<4)|p0;
						*ct++=csm[p2];
						cs+=stride; csl+=stride;
					}
//					BGBBTJ_BT1C_EncodeSetupDiffForBlock(
//						ctx, cs-stride, xs1, ys1, fl, stride);
					continue;
				}

				BGBBTJ_BT1C_EncodeSetupDiffForBlock(
					ctx, cs, xs1, ys1, fl, stride);

				ct=BGBBTJ_BT1C_EncodeBlocksDXTn_EmitModeEscape(
					ctx, ct, 2);

				ct[0]=(i0>>8)&0xFF;
				ct[1]=(i1>>8)&0xFF;
				ct+=2;
				p0=(cs[4]&0x03)|((cs[4]&0x30)>>2);
				p1=(cs[6]&0x03)|((cs[6]&0x30)>>2);
				p2=(p1<<4)|p0;
				*ct++=csm[p2];
				cs+=stride; csl+=stride;
				continue;
			}
#endif

#if 0
			if(is1bpp)
			{
				if((i>1) || (((i0>>8)&0xFF)>=128))
				{
					BGBBTJ_BT1C_EncodeSetupDiffForBlock(
						ctx, cs, xs1, ys1, fl, stride);

					n1=i;
					ct=BGBBTJ_BT1C_EncodeBlocksDXTn_EmitModeEscape(
						ctx, ct, 1);
					*ct++=0xC0+(i-1);
					ct[0]=(i0>>8)&0xFF;
					ct[1]=(i1>>8)&0xFF;
					ct+=2;

					for(i=0; i<n1; i++)
					{
						p0=((cs[4]&1)<<3)|(cs[4]&4)|
							((cs[4]&16)>>3)|((cs[4]&64)>>6);
						p1=((cs[5]&1)<<3)|(cs[5]&4)|
							((cs[5]&16)>>3)|((cs[5]&64)>>6);
						p2=((cs[6]&1)<<3)|(cs[6]&4)|
							((cs[6]&16)>>3)|((cs[6]&64)>>6);
						p3=((cs[7]&1)<<3)|(cs[7]&4)|
							((cs[7]&16)>>3)|((cs[7]&64)>>6);
						p4=(p0<<12)|(p1<<8)|(p2<<4)|p3;

						*ct++=(p4>>8)&255;
						*ct++=p4&255;
						cs+=stride; csl+=stride;
					}
//					BGBBTJ_BT1C_EncodeSetupDiffForBlock(
//						ctx, cs-stride, xs1, ys1, fl, stride);
					continue;
				}

				BGBBTJ_BT1C_EncodeSetupDiffForBlock(
					ctx, cs, xs1, ys1, fl, stride);

				ct=BGBBTJ_BT1C_EncodeBlocksDXTn_EmitModeEscape(
					ctx, ct, 1);
				ct[0]=(i0>>8)&0xFF;
				ct[1]=(i1>>8)&0xFF;
				ct+=2;

				p0=((cs[4]&1)<<3)|(cs[4]&4)|
					((cs[4]&16)>>3)|((cs[4]&64)>>6);
				p1=((cs[5]&1)<<3)|(cs[5]&4)|
					((cs[5]&16)>>3)|((cs[5]&64)>>6);
				p2=((cs[6]&1)<<3)|(cs[6]&4)|
					((cs[6]&16)>>3)|((cs[6]&64)>>6);
				p3=((cs[7]&1)<<3)|(cs[7]&4)|
					((cs[7]&16)>>3)|((cs[7]&64)>>6);
				p4=(p0<<12)|(p1<<8)|(p2<<4)|p3;

				*ct++=(p4>>8)&255;
				*ct++=p4&255;
				cs+=stride; csl+=stride;
				continue;
			}
#endif

#if 1
			if(BGBBTJ_BT1C_BlockColorAB_FloatUnsignedP(ctx, cs))
			{
				i0=BGBBTJ_BT1C_GetBlockColorA_AsRGB31FU(ctx, cs);
				i1=BGBBTJ_BT1C_GetBlockColorB_AsRGB31FU(ctx, cs);
				l=8;
			}else
			{
				l=7;
			}

			BGBBTJ_BT1C_EncodeSetupDiffForBlock(
				ctx, cs, xs1, ys1, fl, stride);
			
			ct=BGBBTJ_BT1C_EncodeBlocksDXTn_EmitModeEscapeColorOnly(
				ctx, ct, l);

			n1=i;
			if(i>1)
				{ *ct++=0xC0+(i-1); }
			else
				{ i1|=0x80000000; }

			*ct++=(i0>>24)&0xFF;
			*ct++=(i0>>16)&0xFF;
			*ct++=(i0>> 8)&0xFF;
			*ct++=(i0    )&0xFF;

			*ct++=(i1>>24)&0xFF;
			*ct++=(i1>>16)&0xFF;
			*ct++=(i1>> 8)&0xFF;
			*ct++=(i1    )&0xFF;

			for(i=0; i<n1; i++)
			{
				ct[0]=csm[cs[4]]; ct[1]=csm[cs[5]];
				ct[2]=csm[cs[6]]; ct[3]=csm[cs[7]];
				cs+=stride; csl+=stride; ct+=4;
			}
//			BGBBTJ_BT1C_EncodeSetupDiffForBlock(
//				ctx, cs-stride, xs1, ys1, fl, stride);
			continue;
#endif
		}
#endif

		//Indexed Color Blocks
		if((fl&BGBBTJ_BTIC1C_EFL_EXTMODES) &&
//			ctx->pal_clr &&
			(cs[0]==0x55) && (cs[2]==0x55))
		{
			if(i0==i1)
			{
				for(i=0; (i<32) && ((cs+i*stride)<cse); i++)
				{
					if(lblks && !memcmp(cs+i*stride, csl+i*stride, 8))
						break;
					i2=cs[i*stride+0]|(cs[i*stride+1]<<8);
					i3=cs[i*stride+2]|(cs[i*stride+3]<<8);
					if((i2!=i0) || (i2!=i3))
						break;
				}
				if(i>0)
				{
					BGBBTJ_BT1C_EncodeSetupDiffForBlock(
						ctx, cs, xs1, ys1, fl, stride);

					ct=BGBBTJ_BT1C_EncodeBlocksDXTn_EmitModeEscapeColorOnly(
						ctx, ct, 1);
//					*ct++=0xF1;
					*ct++=0xA0+(i-1);
					*ct++=(i0>>8)&0xFF;
					cs+=i*stride; csl+=i*stride;
//					BGBBTJ_BT1C_EncodeSetupDiffForBlock(
//						ctx, cs-stride, xs1, ys1, fl, stride);
					continue;
				}
			}

			if(1)
			{
				csm=bt1c_blkmap3;

				for(i=1; (i<32) && ((cs+i*stride)<cse); i++)
				{
					if(lblks && !memcmp(cs+i*stride, csl+i*stride, 8))
						break;

					j=i*stride;
					i2=cs[j+0]|(cs[j+1]<<8);
					i3=cs[j+2]|(cs[j+3]<<8);
					if((i2!=i0) || (i3!=i1))
						break;

					if(is2x2)
					{
						k=((cs[j+4]==cs[j+5]) && (cs[j+6]==cs[j+7]) &&
							(((cs[j+4]>>2)&0x33)==(cs[j+4]&0x33)) &&
							(((cs[j+6]>>2)&0x33)==(cs[j+7]&0x33)));
						if(!k)break;
					}else if(is1bpp)
					{
						k=(!(cs[j+4]&0xAA)) && (!(cs[j+5]&0xAA)) &&
							(!(cs[j+6]&0xAA)) && (!(cs[j+7]&0xAA));
						if(!k)break;
					}
				}

#if 1
				if(is2x2)
				{
					if((i>1) || (((i0>>8)&0xFF)>=128))
					{
						BGBBTJ_BT1C_EncodeSetupDiffForBlock(
							ctx, cs, xs1, ys1, fl, stride);

						ct=BGBBTJ_BT1C_EncodeBlocksDXTn_EmitModeEscape(
							ctx, ct, 2);

						n1=i;
//						*ct++=0xF3;
						*ct++=0xC0+(i-1);
						ct[0]=(i0>>8)&0xFF;
						ct[1]=(i1>>8)&0xFF;
						ct+=2;

						for(i=0; i<n1; i++)
						{
							p0=(cs[4]&0x03)|((cs[4]&0x30)>>2);
							p1=(cs[6]&0x03)|((cs[6]&0x30)>>2);
							p2=(p1<<4)|p0;
							*ct++=csm[p2];
							cs+=stride; csl+=stride;
						}
//						BGBBTJ_BT1C_EncodeSetupDiffForBlock(
//							ctx, cs-stride, xs1, ys1, fl, stride);
						continue;
					}

					ct=BGBBTJ_BT1C_EncodeBlocksDXTn_EmitModeEscape(
						ctx, ct, 2);

//					*ct++=0xF3;
					ct[0]=(i0>>8)&0xFF;
					ct[1]=(i1>>8)&0xFF;
					ct+=2;
					p0=(cs[4]&0x03)|((cs[4]&0x30)>>2);
					p1=(cs[6]&0x03)|((cs[6]&0x30)>>2);
					p2=(p1<<4)|p0;
					*ct++=csm[p2];
					cs+=stride; csl+=stride;
					BGBBTJ_BT1C_EncodeSetupDiffForBlock(
						ctx, cs-stride, xs1, ys1, fl, stride);
					continue;
				}
#endif

//				if(is1bpp)
				if(1)
				{
					if((i>1) || (((i0>>8)&0xFF)>=128))
					{
						BGBBTJ_BT1C_EncodeSetupDiffForBlock(
							ctx, cs, xs1, ys1, fl, stride);

						n1=i;
//						*ct++=0xF1;
						ct=BGBBTJ_BT1C_EncodeBlocksDXTn_EmitModeEscape(
							ctx, ct, 1);
						*ct++=0xC0+(i-1);
						ct[0]=(i0>>8)&0xFF;
						ct[1]=(i1>>8)&0xFF;
						ct+=2;

						for(i=0; i<n1; i++)
						{
							p0=((cs[4]&1)<<3)|(cs[4]&4)|
								((cs[4]&16)>>3)|((cs[4]&64)>>6);
							p1=((cs[5]&1)<<3)|(cs[5]&4)|
								((cs[5]&16)>>3)|((cs[5]&64)>>6);
							p2=((cs[6]&1)<<3)|(cs[6]&4)|
								((cs[6]&16)>>3)|((cs[6]&64)>>6);
							p3=((cs[7]&1)<<3)|(cs[7]&4)|
								((cs[7]&16)>>3)|((cs[7]&64)>>6);
							p4=(p0<<12)|(p1<<8)|(p2<<4)|p3;

							*ct++=(p4>>8)&255;
							*ct++=p4&255;
							cs+=stride; csl+=stride;
						}
//						BGBBTJ_BT1C_EncodeSetupDiffForBlock(
//							ctx, cs-stride, xs1, ys1, fl, stride);
						continue;
					}

					BGBBTJ_BT1C_EncodeSetupDiffForBlock(
						ctx, cs, xs1, ys1, fl, stride);

//					*ct++=0xF1;
					ct=BGBBTJ_BT1C_EncodeBlocksDXTn_EmitModeEscape(
						ctx, ct, 1);
					ct[0]=(i0>>8)&0xFF;
					ct[1]=(i1>>8)&0xFF;
					ct+=2;

					p0=((cs[4]&1)<<3)|(cs[4]&4)|
						((cs[4]&16)>>3)|((cs[4]&64)>>6);
					p1=((cs[5]&1)<<3)|(cs[5]&4)|
						((cs[5]&16)>>3)|((cs[5]&64)>>6);
					p2=((cs[6]&1)<<3)|(cs[6]&4)|
						((cs[6]&16)>>3)|((cs[6]&64)>>6);
					p3=((cs[7]&1)<<3)|(cs[7]&4)|
						((cs[7]&16)>>3)|((cs[7]&64)>>6);
					p4=(p0<<12)|(p1<<8)|(p2<<4)|p3;

					*ct++=(p4>>8)&255;
					*ct++=p4&255;
					cs+=stride; csl+=stride;
//					BGBBTJ_BT1C_EncodeSetupDiffForBlock(
//						ctx, cs-stride, xs1, ys1, fl, stride);
					continue;
				}

#if 0
				if(i>1)
				{
					n1=i;
					*ct++=0xC0+(i-1);

					j=((i0&0xFFC0)>>1)|(i0&0x1F);
					k=((i1&0xFFC0)>>1)|(i1&0x1F);
		
					ct[0]=(j>>8)&0xFF;	ct[1]=j&0xFF;
					ct[2]=(k>>8)&0xFF;	ct[3]=k&0xFF;
					ct+=4;
				
					for(i=0; i<n1; i++)
					{
						ct[0]=csm[cs[4]]; ct[1]=csm[cs[5]];
						ct[2]=csm[cs[6]]; ct[3]=csm[cs[7]];
						cs+=stride; csl+=stride; ct+=4;
					}
					continue;
				}
			
				j=((i0&0xFFC0)>>1)|(i0&0x1F);
				k=((i1&0xFFC0)>>1)|(i1&0x1F);
				k|=0x8000;
		
				ct[0]=(j>>8)&0xFF;	ct[1]=j&0xFF;
				ct[2]=(k>>8)&0xFF;	ct[3]=k&0xFF;
				ct[4]=csm[cs[4]];	ct[5]=csm[cs[5]];
				ct[6]=csm[cs[6]];	ct[7]=csm[cs[7]];
				cs+=stride; csl+=stride; ct+=8;
				continue;
#endif
			}
		}

//		if(i0<=i1)
		if((i0<=i1) && !(ctx->eflags&BGBBTJ_BTIC1C_EFL_BLKMB1))
		{
			//fully transparent blocks
			if(	(cs[4]==0xFF) && (cs[5]==0xFF) &&
				(cs[6]==0xFF) && (cs[7]==0xFF))
			{
				for(i=1; (i<32) && ((cs+i*stride)<cse); i++)
				{
					if(lblks && !memcmp(cs+i*stride, csl+i*stride, 8))
						break;
					if(memcmp(cs+i*stride, cs, 8))
						break;
					//i2=cs[i*8*0]|(cs[i*8+1]<<8);
					//i3=cs[i*8+2]|(cs[i*8+3]<<8);
					//if((i2!=i0) || (i2!=i3))
					//	break;
				}

				BGBBTJ_BT1C_EncodeSetupDiffForBlock(
					ctx, cs, xs1, ys1, fl, stride);
			
				ct=BGBBTJ_BT1C_EncodeBlocksDXTn_EmitModeEscapeColorOnly(
					ctx, ct, 0);
				*ct++=0xA0+(i-1);

				j=((i0&0xFFC0)>>1)|(i0&0x1F);
				j|=0x8000;
				ct[0]=(j>>8)&0xFF;	ct[1]=j&0xFF;
				ct+=2;
				cs+=i*stride; csl+=i*stride;
//				BGBBTJ_BT1C_EncodeSetupDiffForBlock(
//					ctx, cs-stride, xs1, ys1, fl, stride);
				continue;
			}
		}

//		if(fl&BGBBTJ_BTIC1C_EFL_DIFFCLR)
		if(0)
		{
			l=-1;
			pa0=BGBBTJ_BT1C_EncodePredictDiffColorA(
				ctx, blks, cs, xs1, ys1, fl, stride, ctx->diff_sc);
			pb0=BGBBTJ_BT1C_EncodePredictDiffColorB(
				ctx, blks, cs, xs1, ys1, fl, stride, ctx->diff_sc);

			if((i0==pa0) && (i1==pb0))
			{
				l=ctx->diff_sc;
			}else
			{
				l=-1;
				for(i=0; i<8; i++)
				{
					pa0=BGBBTJ_BT1C_EncodePredictDiffColorA(
						ctx, blks, cs, xs1, ys1, fl, stride, i);
					pb0=BGBBTJ_BT1C_EncodePredictDiffColorB(
						ctx, blks, cs, xs1, ys1, fl, stride, i);
					if((i0==pa0) && (i1==pb0))
						{ l=i; break; }
				}
			}

#if 1
			if((i0==i1) && (l>=0))
			{
				for(i=0; (i<32) && ((cs+i*stride)<cse); i++)
				{
					if(lblks && !memcmp(cs+i*stride, csl+i*stride, 8))
						break;
					i2=cs[i*stride+0]|(cs[i*stride+1]<<8);
					i3=cs[i*stride+2]|(cs[i*stride+3]<<8);
					if((i2!=i0) || (i2!=i3))
						break;
					if(	bt1c_blkalphap[cs[i*stride+4]] |
						bt1c_blkalphap[cs[i*stride+5]] |
						bt1c_blkalphap[cs[i*stride+6]] |
						bt1c_blkalphap[cs[i*stride+7]])
							break;
				}

				if(i>0)
				{
					if(l!=ctx->diff_sc)
						{ *ct++=0xF8; *ct++=0x90+l; ctx->diff_sc=l; }
					ct=BGBBTJ_BT1C_EncodeBlocksDXTn_EmitModeEscapeColorOnly(
						ctx, ct, 0);
					*ct++=0xA0+(i-1);

					j=BGBBTJ_BT1C_EncodePredictDiffA(
						ctx, blks, cs, xs1, ys1, fl, stride, l);
					*ct++=j|0x80;

					BGBBTJ_BT1C_EncodeSetupDiffForBlock(
						ctx, cs, xs1, ys1, fl, stride);

					cs+=i*stride; csl+=i*stride;
//					BGBBTJ_BT1C_EncodeSetupDiffForBlock(
//						ctx, cs-stride, xs1, ys1, fl, stride);
					continue;
				}
			}

			if((i0>i1) && (l>=0))
			{
				csm=bt1c_blkmap3;
//				csm=bt1c_blkmap4;

				for(i=1; (i<32) && ((cs+i*stride)<cse); i++)
				{
					if(lblks && !memcmp(cs+i*stride, csl+i*stride, 8))
						break;

					j=i*stride;
					i2=cs[j+0]|(cs[j+1]<<8);
					i3=cs[j+2]|(cs[j+3]<<8);
					if((i2!=i0) || (i3!=i1))
						break;
					if(is2x2)
					{
						k=((cs[j+4]==cs[j+5]) && (cs[j+6]==cs[j+7]) &&
							(((cs[j+4]>>2)&0x33)==(cs[j+4]&0x33)) &&
							(((cs[j+6]>>2)&0x33)==(cs[j+7]&0x33)));
						if(!k)break;
					}
				}

				if(is2x2)
				{
					if(l!=ctx->diff_sc)
						{ *ct++=0xF8; *ct++=0x90+l; ctx->diff_sc=l; }

					n1=i;
					ct=BGBBTJ_BT1C_EncodeBlocksDXTn_EmitModeEscape(
						ctx, ct, 5);
					*ct++=0xC0+(i-1);

					j=BGBBTJ_BT1C_EncodePredictDiffA(
						ctx, blks, cs, xs1, ys1, fl, stride, l);
					k=BGBBTJ_BT1C_EncodePredictDiffB(
						ctx, blks, cs, xs1, ys1, fl, stride, l);
					*ct++=j|0x80;
					*ct++=k|0x80;

					BGBBTJ_BT1C_EncodeSetupDiffForBlock(
						ctx, cs, xs1, ys1, fl, stride);

					for(i=0; i<n1; i++)
					{
						p0=(cs[4]&0x03)|((cs[4]&0x30)>>2);
						p1=(cs[6]&0x03)|((cs[6]&0x30)>>2);
//						p2=(p0<<4)|p1;
						p2=(p1<<4)|p0;
						*ct++=csm[p2];
						cs+=stride; csl+=stride;
					}
//					BGBBTJ_BT1C_EncodeSetupDiffForBlock(
//						ctx, cs-stride, xs1, ys1, fl, stride);
					continue;
				}
			
				if(1)
				{
//					BGBBTJ_BT1C_EncodeSetupDiffForBlock(
//						ctx, cs, xs1, ys1, fl, stride);

					if(l!=ctx->diff_sc)
						{ *ct++=0xF8; *ct++=0x90+l; ctx->diff_sc=l; }

					n1=i;
					ct=BGBBTJ_BT1C_EncodeBlocksDXTn_EmitModeEscape(
						ctx, ct, 0);
					*ct++=0xC0+(i-1);

					j=BGBBTJ_BT1C_EncodePredictDiffA(
						ctx, blks, cs, xs1, ys1, fl, stride, l);
					k=BGBBTJ_BT1C_EncodePredictDiffB(
						ctx, blks, cs, xs1, ys1, fl, stride, l);
					*ct++=j|0x80;
					*ct++=k|0x80;

					BGBBTJ_BT1C_EncodeSetupDiffForBlock(
						ctx, cs, xs1, ys1, fl, stride);

					for(i=0; i<n1; i++)
					{
						ct[0]=csm[cs[4]]; ct[1]=csm[cs[5]];
						ct[2]=csm[cs[6]]; ct[3]=csm[cs[7]];
						cs+=stride; csl+=stride; ct+=4;
					}
//					BGBBTJ_BT1C_EncodeSetupDiffForBlock(
//						ctx, cs-stride, xs1, ys1, fl, stride);
					continue;
				}
			}
#endif
		}

#if 1
		if(i0==i1)
//		if(0)
		{
			for(i=0; (i<32) && ((cs+i*stride)<cse); i++)
			{
				if(lblks && !memcmp(cs+i*stride, csl+i*stride, 8))
					break;
//				if(memcmp(cs+i*8, cs, 8))
//					break;
				i2=cs[i*stride+0]|(cs[i*stride+1]<<8);
				i3=cs[i*stride+2]|(cs[i*stride+3]<<8);
				if((i2!=i0) || (i2!=i3))
					break;
				BGBBTJ_BT1C_GetBlockColorsExtAB(ctx,
					cs+i*stride, &i6, &i7);
				if((i6!=i4) || (i6!=i7))
					break;
				if(	bt1c_blkalphap[cs[i*stride+4]] |
					bt1c_blkalphap[cs[i*stride+5]] |
					bt1c_blkalphap[cs[i*stride+6]] |
					bt1c_blkalphap[cs[i*stride+7]])
						break;
			}

//			if((i>0) && (fl&BGBBTJ_BTIC1C_EFL_EXTMODES) &&
//				(ctx->eflags&BGBBTJ_BTIC1C_EFL_BLKMB1) &&
//				BGBBTJ_BT1C_CheckBlockColor_IsRGB23(ctx, cs))
			if((i>0) && BGBBTJ_BT1C_CheckBlockColor_IsRGB23(ctx, cs))
			{
// #ifdef BTIC1C_ENABLE_DIFFCLR
#if 1
				if(ctx->eflags&BGBBTJ_BTIC1C_EFL_DIFFCLR)
//				if(1)
				{
					i2=BGBBTJ_BT1C_GetColorA_RGB23AsDiff15(ctx, cs);
					if(i2>=0)
					{
						if(ctx->diff_sc!=ctx->diff_lsc)
						{
							*ct++=0xF8;
							*ct++=0x90+ctx->diff_sc;
							ctx->diff_lsc=ctx->diff_sc;
						}
				
						ct=BGBBTJ_BT1C_EncodeBlocksDXTn_EmitModeEscape(
							ctx, ct, 6);

						n1=i;
						*ct++=0xA0+(i-1);
						i2|=0x8000;

						ct[0]=(i2>>8)&0xFF;
						ct[1]=i2&0xFF;
						ct+=2;
						
						ctx->diff_lclra=BGBBTJ_BT1C_AddDiff15ToClr24(
							ctx, i2, ctx->diff_lclra, ctx->diff_sc);
						ctx->diff_lclrb=ctx->diff_lclra;
						ctx->diff_lbmode=6;
						
						if((i2&0x7FFF)==0x4210)
						{
							if(ctx->diff_sc>0)
								ctx->diff_sc--;
						}
						
						cs+=i*stride; csl+=i*stride;
						continue;
					}else
					{
						if(ctx->diff_sc<0)
							ctx->diff_sc=0;
//						if(ctx->diff_sc<15)
//						if(ctx->diff_sc<5)
//						if(ctx->diff_sc<3)
//						if(ctx->diff_sc<1)
//							ctx->diff_sc++;
					}
				}
#endif

				BGBBTJ_BT1C_EncodeSetupDiffForBlock(
					ctx, cs, xs1, ys1, fl, stride);

				i2=BGBBTJ_BT1C_GetBlockColorA_AsRGB23(ctx, cs);
//				i3=BGBBTJ_BT1C_GetBlockColorB_AsRGB23(ctx, cs);
			
				ct=BGBBTJ_BT1C_EncodeBlocksDXTn_EmitModeEscapeColorOnly(
					ctx, ct, 6);
				*ct++=0xA0+(i-1);

				ct[0]=(i2>>16)&0xFF;
				ct[1]=(i2>>8)&0xFF;
				ct[2]=i2&0xFF;
				ct+=3;

				cs+=i*stride; csl+=i*stride;
//				BGBBTJ_BT1C_EncodeSetupDiffForBlock(
//					ctx, cs-stride, xs1, ys1, fl, stride);
				continue;
			}

			if(i>0)
			{
				BGBBTJ_BT1C_EncodeSetupDiffForBlock(
					ctx, cs, xs1, ys1, fl, stride);

				ct=BGBBTJ_BT1C_EncodeBlocksDXTn_EmitModeEscape(
					ctx, ct, 0);
				*ct++=0xA0+(i-1);

				j=((i0&0xFFC0)>>1)|(i0&0x1F);
				ct[0]=(j>>8)&0xFF;	ct[1]=j&0xFF;
				ct+=2;
			
				cs+=i*stride; csl+=i*stride;
//				BGBBTJ_BT1C_EncodeSetupDiffForBlock(
//					ctx, cs-stride, xs1, ys1, fl, stride);
				continue;
			}
		}
#endif

//		if(i0>i1)
//		if((i0>i1) || (cs[0]==0x55))
//		if(1)
		if((i0>i1) || (ctx->eflags&BGBBTJ_BTIC1C_EFL_BLKMB1))
		{
			csm=bt1c_blkmap3;
//			csm=bt1c_blkmap4;

			for(i=1; (i<32) && ((cs+i*stride)<cse); i++)
			{
				if(lblks && !memcmp(cs+i*stride, csl+i*stride, 8))
					break;

				j=i*stride;
				i2=cs[j+0]|(cs[j+1]<<8);
				i3=cs[j+2]|(cs[j+3]<<8);
				if((i2!=i0) || (i3!=i1))
					break;
				BGBBTJ_BT1C_GetBlockColorsExtAB(ctx,
					cs+i*stride, &i6, &i7);
				if((i6!=i4) || (i7!=i5))
					break;

				if(is2x2)
				{
					k=((cs[j+4]==cs[j+5]) && (cs[j+6]==cs[j+7]) &&
						(((cs[j+4]>>2)&0x33)==(cs[j+4]&0x33)) &&
						(((cs[j+6]>>2)&0x33)==(cs[j+7]&0x33)));
					if(!k)break;
				}else if(is1bpp)
				{
//					k=((cs[j+4]==cs[j+5]) && (cs[j+6]==cs[j+7]) &&
//						(((cs[j+4]>>2)&0x33)==(cs[j+4]&0x33)) &&
//						(((cs[j+6]>>2)&0x33)==(cs[j+7]&0x33)));
					k=(!(cs[j+4]&0xAA)) && (!(cs[j+5]&0xAA)) &&
						(!(cs[j+6]&0xAA)) && (!(cs[j+7]&0xAA));
					if(!k)break;
				}
			}

#if 1
			if((i>0) && is2x2 &&
				BGBBTJ_BT1C_CheckBlockColor_IsRGB23(ctx, cs))
			{
// #ifdef BTIC1C_ENABLE_DIFFCLR
#if 1
				if(ctx->eflags&BGBBTJ_BTIC1C_EFL_DIFFCLR)
//				if(1)
				{
					i2=BGBBTJ_BT1C_GetColorA_RGB23AsDiff15(ctx, cs);
					i3=BGBBTJ_BT1C_GetColorB_RGB23AsDiff15(ctx, cs);
					
					if((i2>=0) && (i3>=0))
					{
						if(ctx->diff_sc!=ctx->diff_lsc)
						{
							*ct++=0xF8;
							*ct++=0x90+ctx->diff_sc;
							ctx->diff_lsc=ctx->diff_sc;
						}
				
						ct=BGBBTJ_BT1C_EncodeBlocksDXTn_EmitModeEscape(
							ctx, ct, 11);

						n1=i;
						*ct++=0xC0+(i-1);
						i2|=0x8000;

						ct[0]=(i2>>8)&0xFF;
						ct[1]=i2&0xFF;
						ct+=2;
						ct[0]=(i3>>8)&0xFF;
						ct[1]=i3&0xFF;
						ct+=2;
			
						for(i=0; i<n1; i++)
						{
							p0=(cs[4]&0x03)|((cs[4]&0x30)>>2);
							p1=(cs[6]&0x03)|((cs[6]&0x30)>>2);
							p2=(p1<<4)|p0;
							*ct++=csm[p2];
							cs+=stride; csl+=stride;
						}
						
						ctx->diff_lclra=BGBBTJ_BT1C_AddDiff15ToClr24(
							ctx, i2, ctx->diff_lclra, ctx->diff_sc);
						ctx->diff_lclrb=BGBBTJ_BT1C_AddDiff15ToClr24(
							ctx, i3, ctx->diff_lclrb, ctx->diff_sc);
						ctx->diff_lbmode=6;
						
						if(((i2&0x7FFF)==0x4210) && (i3==0x4210))
						{
							if(ctx->diff_sc>0)
								ctx->diff_sc--;
						}
						
						continue;
					}else
					{
						if(ctx->diff_sc<0)
							ctx->diff_sc=0;
					}
				}
#endif

				BGBBTJ_BT1C_EncodeSetupDiffForBlock(
					ctx, cs, xs1, ys1, fl, stride);

				i2=BGBBTJ_BT1C_GetBlockColorA_AsRGB23(ctx, cs);
				i3=BGBBTJ_BT1C_GetBlockColorB_AsRGB23(ctx, cs);
			
				ct=BGBBTJ_BT1C_EncodeBlocksDXTn_EmitModeEscape(
					ctx, ct, 11);

				n1=i;
				if(i>1)
					{ *ct++=0xC0+(i-1); }
				else
					{ i3|=0x800000; }

				ct[0]=(i2>>16)&0xFF;
				ct[1]=(i2>> 8)&0xFF;
				ct[2]=(i2    )&0xFF;
				ct+=3;

				ct[0]=(i3>>16)&0xFF;
				ct[1]=(i3>> 8)&0xFF;
				ct[2]=(i3    )&0xFF;
				ct+=3;
			
				for(i=0; i<n1; i++)
				{
					p0=(cs[4]&0x03)|((cs[4]&0x30)>>2);
					p1=(cs[6]&0x03)|((cs[6]&0x30)>>2);
					p2=(p1<<4)|p0;
					*ct++=csm[p2];
					cs+=stride; csl+=stride;
				}
//				BGBBTJ_BT1C_EncodeSetupDiffForBlock(
//					ctx, cs-stride, xs1, ys1, fl, stride);
				continue;
			}

			if((i>0) && is1bpp &&
				BGBBTJ_BT1C_CheckBlockColor_IsRGB23(ctx, cs))
			{
// #ifdef BTIC1C_ENABLE_DIFFCLR
#if 1
				if(ctx->eflags&BGBBTJ_BTIC1C_EFL_DIFFCLR)
//				if(1)
				{
					i2=BGBBTJ_BT1C_GetColorA_RGB23AsDiff15(ctx, cs);
					i3=BGBBTJ_BT1C_GetColorB_RGB23AsDiff15(ctx, cs);
					
					if((i2>=0) && (i3>=0))
					{
						if(ctx->diff_sc!=ctx->diff_lsc)
						{
							*ct++=0xF8;
							*ct++=0x90+ctx->diff_sc;
							ctx->diff_lsc=ctx->diff_sc;
						}
				
						ct=BGBBTJ_BT1C_EncodeBlocksDXTn_EmitModeEscape(
							ctx, ct, 10);

						n1=i;
						*ct++=0xC0+(i-1);
						i2|=0x8000;

						ct[0]=(i2>>8)&0xFF;
						ct[1]=i2&0xFF;
						ct+=2;
						ct[0]=(i3>>8)&0xFF;
						ct[1]=i3&0xFF;
						ct+=2;
			
						for(i=0; i<n1; i++)
						{
							p0=((cs[4]&1)<<3)|(cs[4]&4)|
								((cs[4]&16)>>3)|((cs[4]&64)>>6);
							p1=((cs[5]&1)<<3)|(cs[5]&4)|
								((cs[5]&16)>>3)|((cs[5]&64)>>6);
							p2=((cs[6]&1)<<3)|(cs[6]&4)|
								((cs[6]&16)>>3)|((cs[6]&64)>>6);
							p3=((cs[7]&1)<<3)|(cs[7]&4)|
								((cs[7]&16)>>3)|((cs[7]&64)>>6);
							p4=(p0<<12)|(p1<<8)|(p2<<4)|p3;

							*ct++=(p4>>8)&255;
							*ct++=p4&255;

							cs+=stride; csl+=stride;
						}
						
						ctx->diff_lclra=BGBBTJ_BT1C_AddDiff15ToClr24(
							ctx, i2, ctx->diff_lclra, ctx->diff_sc);
						ctx->diff_lclrb=BGBBTJ_BT1C_AddDiff15ToClr24(
							ctx, i3, ctx->diff_lclrb, ctx->diff_sc);
						ctx->diff_lbmode=6;
						
						if(((i2&0x7FFF)==0x4210) && (i3==0x4210))
						{
							if(ctx->diff_sc>0)
								ctx->diff_sc--;
						}
						
						continue;
					}else
					{
						if(ctx->diff_sc<0)
							ctx->diff_sc=0;
					}
				}
#endif

				BGBBTJ_BT1C_EncodeSetupDiffForBlock(
					ctx, cs, xs1, ys1, fl, stride);

				i2=BGBBTJ_BT1C_GetBlockColorA_AsRGB23(ctx, cs);
				i3=BGBBTJ_BT1C_GetBlockColorB_AsRGB23(ctx, cs);
			
				ct=BGBBTJ_BT1C_EncodeBlocksDXTn_EmitModeEscape(
					ctx, ct, 10);

				n1=i;
				if(i>1)
					{ *ct++=0xC0+(i-1); }
				else
					{ i3|=0x800000; }

				ct[0]=(i2>>16)&0xFF;
				ct[1]=(i2>> 8)&0xFF;
				ct[2]=i2&0xFF;
				ct+=3;

				ct[0]=(i3>>16)&0xFF;
				ct[1]=(i3>> 8)&0xFF;
				ct[2]=(i3    )&0xFF;
				ct+=3;
			
				for(i=0; i<n1; i++)
				{
					p0=((cs[4]&1)<<3)|(cs[4]&4)|
						((cs[4]&16)>>3)|((cs[4]&64)>>6);
					p1=((cs[5]&1)<<3)|(cs[5]&4)|
						((cs[5]&16)>>3)|((cs[5]&64)>>6);
					p2=((cs[6]&1)<<3)|(cs[6]&4)|
						((cs[6]&16)>>3)|((cs[6]&64)>>6);
					p3=((cs[7]&1)<<3)|(cs[7]&4)|
						((cs[7]&16)>>3)|((cs[7]&64)>>6);
					p4=(p0<<12)|(p1<<8)|(p2<<4)|p3;

					*ct++=(p4>>8)&255;
					*ct++=p4&255;

					cs+=stride; csl+=stride;
				}
//				BGBBTJ_BT1C_EncodeSetupDiffForBlock(
//					ctx, cs-stride, xs1, ys1, fl, stride);
				continue;
			}
#endif

#if 1
//			if(is2x2)
			if(is2x2 && !BGBBTJ_BT1C_CheckBlockColor_IsRGB23(ctx, cs))
			{
				if(i>1)
//				if(1)
				{
					BGBBTJ_BT1C_EncodeSetupDiffForBlock(
						ctx, cs, xs1, ys1, fl, stride);

					n1=i;
//					*ct++=0xF5;
					ct=BGBBTJ_BT1C_EncodeBlocksDXTn_EmitModeEscape(
						ctx, ct, 5);
					*ct++=0xC0+(i-1);

					j=((i0&0xFFC0)>>1)|(i0&0x1F);
					k=((i1&0xFFC0)>>1)|(i1&0x1F);
					ct[0]=(j>>8)&0xFF;	ct[1]=j&0xFF;
					ct[2]=(k>>8)&0xFF;	ct[3]=k&0xFF;
					ct+=4;

					for(i=0; i<n1; i++)
					{
						p0=(cs[4]&0x03)|((cs[4]&0x30)>>2);
						p1=(cs[6]&0x03)|((cs[6]&0x30)>>2);
//						p2=(p0<<4)|p1;
						p2=(p1<<4)|p0;
						*ct++=csm[p2];
						cs+=stride; csl+=stride;
					}
//					BGBBTJ_BT1C_EncodeSetupDiffForBlock(
//						ctx, cs-stride, xs1, ys1, fl, stride);
					continue;
				}

#if 1
				BGBBTJ_BT1C_EncodeSetupDiffForBlock(
					ctx, cs, xs1, ys1, fl, stride);

//				*ct++=0xF5;
				ct=BGBBTJ_BT1C_EncodeBlocksDXTn_EmitModeEscape(
					ctx, ct, 5);

				j=((i0&0xFFC0)>>1)|(i0&0x1F);
				k=((i1&0xFFC0)>>1)|(i1&0x1F);
				k|=0x8000;
				
				ct[0]=(j>>8)&0xFF;	ct[1]=j&0xFF;
				ct[2]=(k>>8)&0xFF;	ct[3]=k&0xFF;
				ct+=4;

				p0=(cs[4]&0x03)|((cs[4]&0x30)>>2);
				p1=(cs[6]&0x03)|((cs[6]&0x30)>>2);
//				p2=(p0<<4)|p1;
				p2=(p1<<4)|p0;
				*ct++=csm[p2];
				cs+=stride; csl+=stride;
//				BGBBTJ_BT1C_EncodeSetupDiffForBlock(
//					ctx, cs-stride, xs1, ys1, fl, stride);
				continue;
#endif
			}
#endif

#if 1
//			if(is1bpp)
			if(is1bpp && !BGBBTJ_BT1C_CheckBlockColor_IsRGB23(ctx, cs))
			{
				if(i>1)
//				if(1)
				{
					BGBBTJ_BT1C_EncodeSetupDiffForBlock(
						ctx, cs, xs1, ys1, fl, stride);

					n1=i;
//					*ct++=0xF4;
					ct=BGBBTJ_BT1C_EncodeBlocksDXTn_EmitModeEscape(
						ctx, ct, 4);
					*ct++=0xC0+(i-1);

					j=((i0&0xFFC0)>>1)|(i0&0x1F);
					k=((i1&0xFFC0)>>1)|(i1&0x1F);
					k|=0x8000;
					
					ct[0]=(j>>8)&0xFF;	ct[1]=j&0xFF;
					ct[2]=(k>>8)&0xFF;	ct[3]=k&0xFF;
					ct+=4;

					for(i=0; i<n1; i++)
					{
						p0=((cs[4]&1)<<3)|(cs[4]&4)|
							((cs[4]&16)>>3)|((cs[4]&64)>>6);
						p1=((cs[5]&1)<<3)|(cs[5]&4)|
							((cs[5]&16)>>3)|((cs[5]&64)>>6);
						p2=((cs[6]&1)<<3)|(cs[6]&4)|
							((cs[6]&16)>>3)|((cs[6]&64)>>6);
						p3=((cs[7]&1)<<3)|(cs[7]&4)|
							((cs[7]&16)>>3)|((cs[7]&64)>>6);
						p4=(p0<<12)|(p1<<8)|(p2<<4)|p3;

						*ct++=(p4>>8)&255;
						*ct++=p4&255;

//						p0=(cs[4]&0x03)|((cs[4]&0x30)>>2);
//						p1=(cs[6]&0x03)|((cs[6]&0x30)>>2);
//						p2=(p0<<4)|p1;
//						p2=(p1<<4)|p0;
//						*ct++=csm[p2];
						cs+=stride; csl+=stride;
					}
//					BGBBTJ_BT1C_EncodeSetupDiffForBlock(
//						ctx, cs-stride, xs1, ys1, fl, stride);
					continue;
				}

#if 1
				BGBBTJ_BT1C_EncodeSetupDiffForBlock(
					ctx, cs, xs1, ys1, fl, stride);

//				*ct++=0xF4;
				ct=BGBBTJ_BT1C_EncodeBlocksDXTn_EmitModeEscape(
					ctx, ct, 4);

				j=((i0&0xFFC0)>>1)|(i0&0x1F);
				k=((i1&0xFFC0)>>1)|(i1&0x1F);
				k|=0x8000;
				
				ct[0]=(j>>8)&0xFF;	ct[1]=j&0xFF;
				ct[2]=(k>>8)&0xFF;	ct[3]=k&0xFF;
				ct+=4;

				p0=((cs[4]&1)<<3)|(cs[4]&4)|
					((cs[4]&16)>>3)|((cs[4]&64)>>6);
				p1=((cs[5]&1)<<3)|(cs[5]&4)|
					((cs[5]&16)>>3)|((cs[5]&64)>>6);
				p2=((cs[6]&1)<<3)|(cs[6]&4)|
					((cs[6]&16)>>3)|((cs[6]&64)>>6);
				p3=((cs[7]&1)<<3)|(cs[7]&4)|
					((cs[7]&16)>>3)|((cs[7]&64)>>6);
				p4=(p0<<12)|(p1<<8)|(p2<<4)|p3;

				*ct++=(p4>>8)&255;
				*ct++=p4&255;

//				p0=(cs[4]&0x03)|((cs[4]&0x30)>>2);
//				p1=(cs[6]&0x03)|((cs[6]&0x30)>>2);
//				p2=(p0<<4)|p1;
//				p2=(p1<<4)|p0;
//				*ct++=csm[p2];
				cs+=stride; csl+=stride;
//				BGBBTJ_BT1C_EncodeSetupDiffForBlock(
//					ctx, cs-stride, xs1, ys1, fl, stride);
				continue;
#endif
			}
#endif

			if((i>0) && BGBBTJ_BT1C_CheckBlockColor_IsRGB23(ctx, cs))
			{
// #ifdef BTIC1C_ENABLE_DIFFCLR
#if 1
				if(ctx->eflags&BGBBTJ_BTIC1C_EFL_DIFFCLR)
//				if(1)
				{
					i2=BGBBTJ_BT1C_GetColorA_RGB23AsDiff15(ctx, cs);
					i3=BGBBTJ_BT1C_GetColorB_RGB23AsDiff15(ctx, cs);
					
					if((i2>=0) && (i3>=0))
					{
						if(ctx->diff_sc!=ctx->diff_lsc)
						{
							*ct++=0xF8;
							*ct++=0x90+ctx->diff_sc;
							ctx->diff_lsc=ctx->diff_sc;
						}
				
						ct=BGBBTJ_BT1C_EncodeBlocksDXTn_EmitModeEscape(
							ctx, ct, 6);

						n1=i;
						*ct++=0xC0+(i-1);
						i2|=0x8000;

						ct[0]=(i2>>8)&0xFF;
						ct[1]=i2&0xFF;
						ct+=2;
						ct[0]=(i3>>8)&0xFF;
						ct[1]=i3&0xFF;
						ct+=2;
			
						for(i=0; i<n1; i++)
						{
							ct[0]=csm[cs[4]]; ct[1]=csm[cs[5]];
							ct[2]=csm[cs[6]]; ct[3]=csm[cs[7]];
							cs+=stride; csl+=stride; ct+=4;
						}
						
						ctx->diff_lclra=BGBBTJ_BT1C_AddDiff15ToClr24(
							ctx, i2, ctx->diff_lclra, ctx->diff_sc);
						ctx->diff_lclrb=BGBBTJ_BT1C_AddDiff15ToClr24(
							ctx, i3, ctx->diff_lclrb, ctx->diff_sc);
						ctx->diff_lbmode=6;
						
						if(((i2&0x7FFF)==0x4210) && (i3==0x4210))
						{
							if(ctx->diff_sc>0)
								ctx->diff_sc--;
						}
						
						continue;
					}else
					{
						if(ctx->diff_sc<0)
							ctx->diff_sc=0;
					}
				}
#endif

				BGBBTJ_BT1C_EncodeSetupDiffForBlock(
					ctx, cs, xs1, ys1, fl, stride);
				i2=BGBBTJ_BT1C_GetBlockColorA_AsRGB23(ctx, cs);
				i3=BGBBTJ_BT1C_GetBlockColorB_AsRGB23(ctx, cs);
			
				ct=BGBBTJ_BT1C_EncodeBlocksDXTn_EmitModeEscape(
					ctx, ct, 6);

				n1=i;
				if(i>1)
					{ *ct++=0xC0+(i-1); }
				else
					{ i3|=0x800000; }

				ct[0]=(i2>>16)&0xFF;
				ct[1]=(i2>>8)&0xFF;
				ct[2]=i2&0xFF;
				ct+=3;

				ct[0]=(i3>>16)&0xFF;
				ct[1]=(i3>>8)&0xFF;
				ct[2]=i3&0xFF;
				ct+=3;
			
				for(i=0; i<n1; i++)
				{
					ct[0]=csm[cs[4]]; ct[1]=csm[cs[5]];
					ct[2]=csm[cs[6]]; ct[3]=csm[cs[7]];
					cs+=stride; csl+=stride; ct+=4;
				}
//				BGBBTJ_BT1C_EncodeSetupDiffForBlock(
//					ctx, cs-stride, xs1, ys1, fl, stride);
				continue;
			}

			if(i>1)
//			if(0)
			{
				BGBBTJ_BT1C_EncodeSetupDiffForBlock(
					ctx, cs, xs1, ys1, fl, stride);

				n1=i;
				ct=BGBBTJ_BT1C_EncodeBlocksDXTn_EmitModeEscape(
					ctx, ct, 0);
				*ct++=0xC0+(i-1);

				j=((i0&0xFFC0)>>1)|(i0&0x1F);
				k=((i1&0xFFC0)>>1)|(i1&0x1F);
		
				ct[0]=(j>>8)&0xFF;	ct[1]=j&0xFF;
				ct[2]=(k>>8)&0xFF;	ct[3]=k&0xFF;
				ct+=4;
				
				for(i=0; i<n1; i++)
				{
					ct[0]=csm[cs[4]]; ct[1]=csm[cs[5]];
					ct[2]=csm[cs[6]]; ct[3]=csm[cs[7]];
					cs+=stride; csl+=stride; ct+=4;
				}
//				BGBBTJ_BT1C_EncodeSetupDiffForBlock(
//					ctx, cs-stride, xs1, ys1, fl, stride);
				continue;
			}
			
//			csm=bt1c_blkmap3;

			BGBBTJ_BT1C_EncodeSetupDiffForBlock(
				ctx, cs, xs1, ys1, fl, stride);

			ct=BGBBTJ_BT1C_EncodeBlocksDXTn_EmitModeEscape(
				ctx, ct, 0);
//			*ct++=0xC0;
			j=((i0&0xFFC0)>>1)|(i0&0x1F);
			k=((i1&0xFFC0)>>1)|(i1&0x1F);
			k|=0x8000;
		
			ct[0]=(j>>8)&0xFF;	ct[1]=j&0xFF;
			ct[2]=(k>>8)&0xFF;	ct[3]=k&0xFF;
			ct[4]=csm[cs[4]];	ct[5]=csm[cs[5]];
			ct[6]=csm[cs[6]];	ct[7]=csm[cs[7]];
			cs+=stride; csl+=stride; ct+=8;
//			BGBBTJ_BT1C_EncodeSetupDiffForBlock(
//				ctx, cs-stride, xs1, ys1, fl, stride);
			continue;
		}


		BGBBTJ_BT1C_EncodeClearDiffForBlock(ctx);

		if(i0<=i1)
		{
			csm=bt1c_blkmap4;
//			csm=bt1c_blkmap3;

			for(i=1; (i<32) && ((cs+i*stride)<cse); i++)
			{
				if(lblks && !memcmp(cs+i*stride, csl+i*stride, 8))
					break;
				i2=cs[i*stride+0]|(cs[i*stride+1]<<8);
				i3=cs[i*stride+2]|(cs[i*stride+3]<<8);
				if((i2!=i0) || (i3!=i1))
					break;
				BGBBTJ_BT1C_GetBlockColorsExtAB(ctx,
					cs+i*stride, &i6, &i7);
				if((i6!=i4) || (i7!=i5))
					break;
			}

			if((i>0) && BGBBTJ_BT1C_CheckBlockColor_IsRGB23(ctx, cs))
			{
				i2=BGBBTJ_BT1C_GetBlockColorA_AsRGB23(ctx, cs);
				i3=BGBBTJ_BT1C_GetBlockColorB_AsRGB23(ctx, cs);
				i3|=0x800000;
				ct=BGBBTJ_BT1C_EncodeBlocksDXTn_EmitModeEscape(
					ctx, ct, 6);

				n1=i;
				if(i>1)
					{ *ct++=0xC0+(i-1); }
				else
					{ i3|=0x800000; }

				ct[0]=(i2>>16)&0xFF;
				ct[1]=(i2>>8)&0xFF;
				ct[2]=i2&0xFF;
				ct+=3;

				ct[0]=(i3>>16)&0xFF;
				ct[1]=(i3>>8)&0xFF;
				ct[2]=i3&0xFF;
				ct+=3;
			
				for(i=0; i<n1; i++)
				{
					ct[0]=csm[cs[4]]; ct[1]=csm[cs[5]];
					ct[2]=csm[cs[6]]; ct[3]=csm[cs[7]];
					cs+=stride; csl+=stride; ct+=4;
				}
//				BGBBTJ_BT1C_EncodeSetupDiffForBlock(
//					ctx, cs-stride, xs1, ys1, fl, stride);
				continue;
			}

#if 1
			if(i>1)
			{
				ct=BGBBTJ_BT1C_EncodeBlocksDXTn_EmitModeEscape(
					ctx, ct, 0);

				n1=i;
				*ct++=0xC0+(i-1);

				j=((i0&0xFFC0)>>1)|(i0&0x1F);
				k=((i1&0xFFC0)>>1)|(i1&0x1F);
				k|=0x8000;

				ct[0]=(j>>8)&0xFF;	ct[1]=j&0xFF;
				ct[2]=(k>>8)&0xFF;	ct[3]=k&0xFF;
				ct+=4;
				
				for(i=0; i<n1; i++)
				{
					ct[0]=csm[cs[4]]; ct[1]=csm[cs[5]];
					ct[2]=csm[cs[6]]; ct[3]=csm[cs[7]];
					cs+=stride; csl+=stride; ct+=4;
				}
//				BGBBTJ_BT1C_EncodeSetupDiffForBlock(
//					ctx, cs-stride, xs1, ys1, fl, stride);
				continue;
			}
#endif

			ct=BGBBTJ_BT1C_EncodeBlocksDXTn_EmitModeEscape(
				ctx, ct, 0);
			
			*ct++=0xC0;
			j=((i0&0xFFC0)>>1)|(i0&0x1F);
			k=((i1&0xFFC0)>>1)|(i1&0x1F);
			k|=0x8000;
		
			ct[0]=(j>>8)&0xFF;	ct[1]=j&0xFF;
			ct[2]=(k>>8)&0xFF;	ct[3]=k&0xFF;
			ct[4]=csm[cs[4]];	ct[5]=csm[cs[5]];
			ct[6]=csm[cs[6]];	ct[7]=csm[cs[7]];
			cs+=stride; csl+=stride; ct+=8;
//			BGBBTJ_BT1C_EncodeSetupDiffForBlock(
//				ctx, cs-stride, xs1, ys1, fl, stride);
			continue;
		}
	}

	i=ct-obuf;
	obuf[1]=(i>>16)&0xFF;
	obuf[2]=(i>>8)&0xFF;
	obuf[3]=(i>>0)&0xFF;

//	if(1)
	if(!lblks)
	{
		for(j=0; j<256; j++)
		{
			if(!stats[j])continue;
			printf("%02X:%d ", j, stats[j]);
		}
		printf("\n");

		for(j=0; j<256; j++)
			stats[j]=0;
	}

	return(i);
}

BGBBTJ_API int BGBBTJ_BT1C_EncodeBlocksBC4(
	BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *obuf, int osz, byte *blks, byte *lblks,
	int xs, int ys, int fl, int stride)
{
	byte tb[16];
	byte *cs, *cse, *csm, *csl;
	byte *ct, *cte, *ctb;
	int xs1, ys1, n, n1;
	int xs2, ys2, xs3, ys3;
	int i0, i1, i2, i3;
	int bl, bd;
	int i, j, k, l;
	
	BGBBTJ_BT1C_Init();

	ctx->eflags=fl;
	ctx->eflags&=~BGBBTJ_BTIC1C_EFL_BLKMB1;
	if(stride==32)
		ctx->eflags|=BGBBTJ_BTIC1C_EFL_BLKMB1;

	xs1=(xs+3)/4;
	ys1=(ys+3)/4;
	n=xs1*ys1;

	if(fl&BGBBTJ_BTIC1C_EFL_MIPMAP)
	{
		xs2=xs; ys2=ys; n=0;
		while((xs2>1) || (ys2>1))
		{
			if(!xs2)xs2++;
			if(!ys2)ys2++;
			xs3=(xs2+3)/4;
			ys3=(ys2+3)/4;
			n+=xs3*ys3;
			xs2>>=1; ys2>>=1;
		}
	}
	
	cs=blks; cse=cs+n*stride; csl=lblks;
	ct=obuf;
	
	*ct++=0xE3;		*ct++=0x00;
	*ct++=0x00;		*ct++=0x00;
	*ct++='A';		*ct++='X';

	ctx->curmode=ctx->modes[0];
	
//	if((fl&BGBBTJ_BTIC1C_EFL_EXTMODES) && (!lblks || lbjp))
	if((fl&BGBBTJ_BTIC1C_EFL_EXTMODES) && !lblks)
	{
		for(i=0; i<8; i++)
			{ ctx->amodes[i]=i; }
		for(i=0; i<256; i++)
			{ ctx->amodecnt[i]=0; }

		for(i=0; i<8; i++)
		{
			j=(i<<12)|i;
			*ct++=0xF8;
			*ct++=0x8A;
			*ct++=(j>>8)&255;
			*ct++=j&255;
		}

		ctx->curmode=-1;
//		*ct++=0xF0;
//		*ct++=0xF0;
	}

//	ctx->curmode=0;
//	ctx->curmode=ctx->modes[0];

	while(cs<cse)
	{
		if(lblks && !memcmp(cs, csl, 8))
		{
			for(i=1; (i<32) && ((cs+i*stride)<cse); i++)
			{
				if(memcmp(cs+i*stride, csl+i*stride, 8))
					break;
			}

			*ct++=0x80+(i-1);
			cs+=i*stride; csl+=i*stride;
			continue;
		}

		i0=cs[0];
		i1=cs[1];

		if((ctx->eflags&BGBBTJ_BTIC1C_EFL_BLKMB1) &&
			((i0&0xF0)==0x80) && ((i1&0xF0)==0x70))
		{
			i0=(cs[0]<<8)|cs[2];
			i1=(cs[1]<<8)|cs[3];
			i0=(i0<<4)&65535;
			i1=(i1<<4)&65535;
			i0=i0|(i0>>12);
			i1=i1|(i1>>12);

			if(i0==i1)
			{
				for(i=0; (i<32) && ((cs+i*stride)<cse); i++)
				{
					if(memcmp(cs+i*stride, cs, 8))
						break;
				}
			
				if(i>0)
				{
					*ct++=0xA0+(i-1);

					j=i0>>1;
					ct[0]=(j>>8)&0xFF;	ct[1]=j&0xFF;
					ct+=2;
			
					cs+=i*stride; csl+=i*stride;
					continue;
				}
			}

			for(i=1; (i<32) && ((cs+i*stride)<cse); i++)
			{
//				i2=cs[i*stride+0];
//				i3=cs[i*stride+1];
				j=i*stride;
				i2=(cs[j+0]<<8)|cs[j+2];
				i3=(cs[j+1]<<8)|cs[j+3];
				i2=(i2<<4)&65535;
				i3=(i3<<4)&65535;
				i2=i2|(i2>>12);
				i3=i3|(i3>>12);

				if((i2!=i0) || (i3!=i1))
					break;
			}
			
			j=i0>>1;
			k=i1>>1;

			csm=bt1c_blkmap3;

			ct=BGBBTJ_BT1C_EncodeBlocksDXTn_EmitModeEscape(
				ctx, ct, 3);

			n1=i;
			if(i>1)
			{
				*ct++=0xC0+(i-1);
			}else
			{
				k|=0x8000;
			}

			ct[0]=(j>>8)&0xFF;	ct[1]=j&0xFF;
			ct[2]=(k>>8)&0xFF;	ct[3]=k&0xFF;
			ct+=4;

			for(i=0; i<n1; i++)
			{
				ct[0]=csm[cs[4]];	ct[1]=csm[cs[5]];
				ct[2]=csm[cs[6]];	ct[3]=csm[cs[7]];
				cs+=stride; csl+=stride; ct+=4;
			}
			continue;
		}

#if 0
		if(BGBBTJ_BT1C_EncodeBlocksDXT1_LookupRun(
			cs, blks, cse, &bl, &bd, fl))
		{
			if((bl==1) && (bd==1))
				{ *ct++=0xE8; cs+=bl*stride; csl+=bl*stride; continue; }
			if((bl<=256) && (bd==1))
				{ *ct++=0xE9; *ct++=bl-1;
				cs+=bl*stride; csl+=bl*stride; continue; }
			if((bl==1) && (bd<=256))
				{ *ct++=0xEA; *ct++=bd-1;
				cs+=bl*stride; csl+=bl*stride; continue; }
			if((bl==1) && (bd<=65536))
			{
				j=bd-1; *ct++=0xEB;
				*ct++=(j>>8)&255; *ct++=j&255;
				cs+=bl*stride; csl+=bl*stride; continue;
			}
			if((bl<=256) && (bd<=256))
				{ *ct++=0xEC; *ct++=bl-1; *ct++=bd-1;
				cs+=bl*stride; csl+=bl*stride; continue; }
			if((bl<=256) && (bd<=65536))
			{
				*ct++=0xED; *ct++=bl-1;
				j=bd-1; *ct++=(j>>8)&255; *ct++=j&255;
				cs+=bl*stride; csl+=bl*stride; continue;
			}
		}
#endif

		if(i0==i1)
		{
			for(i=0; (i<32) && ((cs+i*stride)<cse); i++)
			{
				if(memcmp(cs+i*stride, cs, 8))
					break;
			}
			
			if(i>0)
			{
				*ct++=0xA0+(i-1);

//				j=((i0&0xFFC0)>>1)|(i0&0x1F);
				j=i0;
				*ct++=j>>1;

//				ct[0]=(j>>8)&0xFF;	ct[1]=j&0xFF;
//				ct+=2;
			
				cs+=i*stride; csl+=i*stride;
				continue;
			}
		}

//		if(i0>=i1)
		if(1)
		{
			if(i0>i1)
				{ csm=bt1c_blkalphamape2; }
			else
				{ csm=bt1c_blkalphamape1; }

			for(i=1; (i<32) && ((cs+i*stride)<cse); i++)
			{
				i2=cs[i*stride+0];
				i3=cs[i*stride+1];
				if((i2!=i0) || (i3!=i1))
					break;
			}
			
			if(i>1)
//			if(0)
			{
				n1=i;
				*ct++=0xC0+(i-1);

				j=i0; k=i1;
//				ct[0]=(j>>8)&0xFF;	ct[1]=j&0xFF;
//				ct[2]=(k>>8)&0xFF;	ct[3]=k&0xFF;
//				ct+=4;

				ct[0]=(j>>1)&0x7F;
				ct[1]=(k>>1)&0x7F;
				ct+=2;

//				ct[0]=(j>>8)&0xFF;	ct[1]=j&0xFF; ct[2]=k;
//				ct+=3;
				
				for(i=0; i<n1; i++)
				{
					j=cs[2]|(cs[3]<<8)|(cs[4]<<16);
					k=cs[5]|(cs[6]<<8)|(cs[7]<<16);
					ct[0]=csm[j&0xFFF]; ct[1]=csm[(j>>12)&0xFFF];
					ct[2]=csm[k&0xFFF]; ct[3]=csm[(k>>12)&0xFFF];
					cs+=stride; csl+=stride; ct+=4;
				}
				continue;
			}
			
			j=i0; k=i1;
//			k|=0x8000;
		
//			ct[0]=(j>>8)&0xFF;	ct[1]=j&0xFF;
//			ct[2]=(k>>8)&0xFF;	ct[3]=k&0xFF;
//			ct[0]=(j>>8)&0xFF;	ct[1]=j&0xFF; ct[2]=k;
//			ct+=3;

			ct[0]=((j>>1)&0x7F)|0x00;
			ct[1]=((k>>1)&0x7F)|0x80;
			ct+=2;

			j=cs[2]|(cs[3]<<8)|(cs[4]<<16);
			k=cs[5]|(cs[6]<<8)|(cs[7]<<16);
			ct[0]=csm[j&0xFFF];	ct[1]=csm[(j>>12)&0xFFF];
			ct[2]=csm[k&0xFFF];	ct[3]=csm[(k>>12)&0xFFF];
			cs+=stride; csl+=stride; ct+=4;
			continue;
		}
	}

	i=ct-obuf;
	obuf[1]=(i>>16)&0xFF;
	obuf[2]=(i>>8)&0xFF;
	obuf[3]=(i>>0)&0xFF;

	return(i);
}

BGBBTJ_API BGBBTJ_BTIC1C_Context *BGBBTJ_BT1C_AllocContext()
{
	BGBBTJ_BTIC1C_Context *ctx;
	BGBBTJ_BTIC1C_LayerInfo *lyr;
	BGBBTJ_BTIC1C_ImageInfo *hdr;

	ctx=gctalloc("bt1c_context_t", sizeof(BGBBTJ_BTIC1C_Context));
	lyr=gctalloc("bt1c_layerinfo_t", sizeof(BGBBTJ_BTIC1C_LayerInfo));
	hdr=gctalloc("bt1c_imageinfo_t", sizeof(BGBBTJ_BTIC1C_ImageInfo));

	hdr->ctx=ctx;
	hdr->layer=lyr;

	lyr->image[0]=hdr;
	lyr->n_image=1;
	lyr->ctx=ctx;

	ctx->layer[0]=lyr;
	ctx->image[0]=hdr;
	ctx->n_layer=1;
	ctx->n_image=1;
	
	return(ctx);
}

BGBBTJ_API int BGBBTJ_BT1C_EncodeImageFast(
	BGBBTJ_BTIC1C_Context *ctx, byte *rgba, byte *obuf, int osz,
	int xs, int ys, int qf, int clrs)
{
	return(BGBBTJ_BT1C_EncodeImage(
		ctx, rgba, obuf, osz, xs, ys, qf, clrs));
}

BGBBTJ_API int BGBBTJ_BT1C_EncodeImage(
	BGBBTJ_BTIC1C_Context *ctx, byte *rgba, byte *obuf, int osz,
	int xs, int ys, int qf, int clrs)
{
	BGBBTJ_BTIC1C_ImageInfo *hdr;
	int sz;

	hdr=ctx->image[0];
	
	sz=BGBBTJ_BT1C_EncodeImage2(ctx, hdr,
		rgba, obuf, osz, xs, ys, qf, clrs);
	return(sz);
}

BGBBTJ_API int BGBBTJ_BT1C_EncodeImage2(
	BGBBTJ_BTIC1C_Context *ctx, BGBBTJ_BTIC1C_ImageInfo *hdr,
	byte *rgba, byte *obuf, int osz,
	int xs, int ys, int qf, int clrs)
{
	byte tpal_rgb[256*4];
	s16 *rgba2;
	byte *ldat, *ct, *ct0;
	byte *tbuf;
	int xs1, ys1;
	int xs2, ys2;
	int xs3, ys3;
	int lcr, lcg, lcb, lca, lcy;
	int ccr, ccg, ccb, cca, ccy;
	int dcr, dcg, dcb, dca, dcy;
	int cr, cg, cb, ca;
	int sz, bsz, efl, sz2, lsc;
	int x1, y1, x2, y2;
	int i, j, k, l, n;
	
	hdr->width=xs;
	hdr->height=ys;
	hdr->flags=qf;
	hdr->clrs=clrs;

	efl=0;
	if(qf&BGBBTJ_BTIC1C_QFL_DIFFCLR)
		efl|=BGBBTJ_BTIC1C_EFL_DIFFCLR;
	if(qf&BGBBTJ_BTIC1C_QFL_BLOCKZI)
		efl|=BGBBTJ_BTIC1C_EFL_BLOCKZI;
	if(qf&BGBBTJ_BTIC1C_QFL_EXTMODES)
		efl|=BGBBTJ_BTIC1C_EFL_EXTMODES;
	if(qf&BGBBTJ_BTIC1C_QFL_MOTION)
		efl|=BGBBTJ_BTIC1C_EFL_MOTION;

	hdr->width2=(xs+3)/4;
	hdr->height2=(ys+3)/4;
	hdr->nblocks=hdr->width2*hdr->height2;
	hdr->nblocksmip=hdr->nblocks;

	if(qf&BGBBTJ_QFL_MIPMAP)
	{
		xs2=xs; ys2=ys; n=0;
		while((xs2>0) || (ys2>0))
		{
			if(!xs2)xs2++;
			if(!ys2)ys2++;
			xs3=(xs2+3)/4;
			ys3=(ys2+3)/4;
			n+=xs3*ys3;
			xs2>>=1; ys2>>=1;
		}
		hdr->nblocksmip=n;
	}

	if(!hdr->rgba)
	{
//		i=(4*hdr->width2)*(4*hdr->height2)*4;
//		if(qf&BGBBTJ_QFL_MIPMAP)i*=2;
		i=hdr->nblocksmip*16*4;
		hdr->rgba=gcatomic(i);
		hdr->lrgba=gcatomic(i);
		hdr->sz_rgba=i;
	}
	
	if(!hdr->data)
	{
//		i=hdr->nblocks*16;
		i=hdr->nblocksmip*32;
		hdr->data=gcatomic(i);
		hdr->sz_data=i;
	}

#if 1
	if(!hdr->fpdata)
	{
//		i=hdr->nblocks*16;
		i=hdr->nblocksmip*32;
		hdr->fpdata=gcatomic(i);
		hdr->sz_fpdata=i;
	}
#endif

	xs1=hdr->width2*4;
	ys1=hdr->height2*4;

#if 1
	if(clrs==BGBBTJ_JPG_RGB)
	{
		for(i=0; i<ys; i++)
			for(j=0; j<xs; j++)
		{
			k=(i*xs+j)*3;
			cr=rgba[k+0];		cg=rgba[k+1];
			cb=rgba[k+2];		ca=255;

			k=(i*xs1+j)*4;
//			hdr->rgba[k+0]=cr; hdr->rgba[k+1]=cg;
//			hdr->rgba[k+2]=cb; hdr->rgba[k+3]=ca;

			hdr->rgba[k+0]=cb; hdr->rgba[k+1]=cg;
			hdr->rgba[k+2]=cr; hdr->rgba[k+3]=ca;
		}
	}

	if((clrs==BGBBTJ_JPG_RGBA) || (clrs==BGBBTJ_JPG_RGBX))
	{
		xs2=hdr->width2;
		ys2=hdr->height2;
		for(y1=0; y1<ys2; y1++)
			for(x1=0; x1<xs2; x1++)
		{
			for(k=0; k<4; k++)
			{
				for(l=0; l<4; l++)
				{
					ca=rgba[((y1*4+k)*xs+(x1*4+l))*4+3];
					if(ca>0)break;
				}
				if(l<4)break;
			}
			if(k<4)
			{
				for(y2=0; y2<4; y2++)
				{
					for(x2=0; x2<4; x2++)
					{
						j=((y1*4+y2)*xs+(x1*4+x2))*4;
						k=((y1*4+y2)*xs1+(x1*4+x2))*4;
						cr=rgba[j+0]; cg=rgba[j+1];
						cb=rgba[j+2]; ca=rgba[j+3];
//						hdr->rgba[k+0]=cr; hdr->rgba[k+1]=cg;
//						hdr->rgba[k+2]=cb; hdr->rgba[k+3]=ca;
						hdr->rgba[k+0]=cb; hdr->rgba[k+1]=cg;
						hdr->rgba[k+2]=cr; hdr->rgba[k+3]=ca;
					}
				}
			}else
			{
				for(y2=0; y2<4; y2++)
				{
					for(x2=0; x2<4; x2++)
					{
						k=((y1*4+y2)*xs1+(x1*4+x2))*4;
						cr=255; cg=0; cb=255; ca=0;
//						hdr->rgba[k+0]=cr; hdr->rgba[k+1]=cg;
//						hdr->rgba[k+2]=cb; hdr->rgba[k+3]=ca;

						hdr->rgba[k+0]=cb; hdr->rgba[k+1]=cg;
						hdr->rgba[k+2]=cr; hdr->rgba[k+3]=ca;
					}
				}
			}
		}
		
//		BGBBTJ_Tex_Dither15(hdr->rgba, hdr->rgba, xs1, ys1);
	}
#endif

#if 1
	if(clrs==BGBBTJ_JPG_BGR)
	{
		for(i=0; i<ys; i++)
			for(j=0; j<xs; j++)
		{
			k=(i*xs+j)*3;
			cr=rgba[k+2];		cg=rgba[k+1];
			cb=rgba[k+0];		ca=255;

			k=(i*xs1+j)*4;
//			hdr->rgba[k+0]=cr; hdr->rgba[k+1]=cg;
//			hdr->rgba[k+2]=cb; hdr->rgba[k+3]=ca;

			hdr->rgba[k+0]=cb; hdr->rgba[k+1]=cg;
			hdr->rgba[k+2]=cr; hdr->rgba[k+3]=ca;
		}
	}

	if(clrs==BGBBTJ_JPG_BGRX)
	{
		for(i=0; i<ys; i++)
			for(j=0; j<xs; j++)
		{
			k=(i*xs+j)*4;
			cr=rgba[k+2];		cg=rgba[k+1];
			cb=rgba[k+0];		ca=255;

			k=(i*xs1+j)*4;
//			hdr->rgba[k+0]=cr; hdr->rgba[k+1]=cg;
//			hdr->rgba[k+2]=cb; hdr->rgba[k+3]=ca;

			hdr->rgba[k+0]=cb; hdr->rgba[k+1]=cg;
			hdr->rgba[k+2]=cr; hdr->rgba[k+3]=ca;
		}
	}

//	if((clrs==BGBBTJ_JPG_BGRA) || (clrs==BGBBTJ_JPG_BGRX))
	if(clrs==BGBBTJ_JPG_BGRA)
	{
		xs2=hdr->width2;
		ys2=hdr->height2;
		for(y1=0; y1<ys2; y1++)
			for(x1=0; x1<xs2; x1++)
		{
			for(k=0; k<4; k++)
			{
				for(l=0; l<4; l++)
				{
					ca=rgba[((y1*4+k)*xs+(x1*4+l))*4+3];
					if(ca>0)break;
				}
				if(l<4)break;
			}
			if(k<4)
			{
				for(y2=0; y2<4; y2++)
				{
					for(x2=0; x2<4; x2++)
					{
						j=((y1*4+y2)*xs+(x1*4+x2))*4;
						k=((y1*4+y2)*xs1+(x1*4+x2))*4;
						cr=rgba[j+2]; cg=rgba[j+1];
						cb=rgba[j+0]; ca=rgba[j+3];
//						hdr->rgba[k+0]=cr; hdr->rgba[k+1]=cg;
//						hdr->rgba[k+2]=cb; hdr->rgba[k+3]=ca;

						hdr->rgba[k+0]=cb; hdr->rgba[k+1]=cg;
						hdr->rgba[k+2]=cr; hdr->rgba[k+3]=ca;
					}
				}
			}else
			{
				for(y2=0; y2<4; y2++)
				{
					for(x2=0; x2<4; x2++)
					{
						k=((y1*4+y2)*xs1+(x1*4+x2))*4;
						cr=255; cg=0; cb=255; ca=0;
//						hdr->rgba[k+0]=cr; hdr->rgba[k+1]=cg;
//						hdr->rgba[k+2]=cb; hdr->rgba[k+3]=ca;

						hdr->rgba[k+0]=cb; hdr->rgba[k+1]=cg;
						hdr->rgba[k+2]=cr; hdr->rgba[k+3]=ca;
					}
				}
			}
		}
		
//		BGBBTJ_Tex_Dither15(hdr->rgba, hdr->rgba, xs1, ys1);
	}
#endif

#if 0
	if(clrs==BGBBTJ_JPG_RGBA)
	{
		for(i=0; i<ys; i++)
			for(j=0; j<xs; j++)
		{
			BGBBTJ_BlendMagentaEncodeColor(
				rgba+((i*xs+j)*4),
				hdr->rgba+((i*xs1+j)*4));
		
#if 0
			cr=rgba[(i*xs+j)*4+0];		cg=rgba[(i*xs+j)*4+1];
			cb=rgba[(i*xs+j)*4+2];		ca=rgba[(i*xs+j)*4+3];
		
			l=255-ca;
			k=255-((l*l)>>8);
//			k=(ca*ca)>>8;
		
//			if(ca<224)
//			if(k<224)
			if(k<252)
			{
//				cr=((cr*k)+(0*(256-k)))>>8;
//				cg=((cg*k)+(255*(256-k)))>>8;
//				cb=((cb*k)+(255*(256-k)))>>8;

				cr=((cr*k)+(255*(256-k)))>>8;
				cg=((cg*k)+(0*(256-k)))>>8;
				cb=((cb*k)+(255*(256-k)))>>8;

#if 0
				if(((i>>6)^(j>>6))&1)
				{
					cr=((cr*k)+(255*(256-k)))>>8;
					cg=((cg*k)+(0*(256-k)))>>8;
					cb=((cb*k)+(255*(256-k)))>>8;
				}else
				{
					cr=((cr*k)+(0*(256-k)))>>8;
					cg=((cg*k)+(255*(256-k)))>>8;
					cb=((cb*k)+(255*(256-k)))>>8;
				}
#endif
			}

			hdr->rgba[(i*xs+j)*4+0]=cr;
			hdr->rgba[(i*xs+j)*4+1]=cg;
			hdr->rgba[(i*xs+j)*4+2]=cb;
			hdr->rgba[(i*xs+j)*4+3]=ca;
#endif
		}
	}
#endif

#if 0
	if(clrs==BGBBTJ_JPG_RGBA_Q11_4)
	{
		if(!hdr->tmm_rgba)
		{
//			i=(4*hdr->width2)*(4*hdr->height2)*4;
//			if(qf&BGBBTJ_QFL_MIPMAP)i*=2;
			i=hdr->nblocksmip*4*sizeof(s16);
			hdr->tmm_rgba=gcatomic(i);
			hdr->tmn_rgba=gcatomic(i);
			hdr->sz_tmrgba=i;
		}

		BGBBTJ_SplitHdrBlockToneMap_FixedQ3_11(
			(s16 *)rgba, hdr->tmm_rgba, hdr->tmn_rgba,
			hdr->rgba, xs, ys, qf);
	}
#endif

#if 1
//	if((clrs==BGBBTJ_JPG_RGBA) || (clrs==BGBBTJ_JPG_BGRA))
	if((clrs==BGBBTJ_JPG_RGBA) || (clrs==BGBBTJ_JPG_BGRA) ||
		(clrs==BGBBTJ_JPG_RGBX) || (clrs==BGBBTJ_JPG_BGRX) ||
		(clrs==BGBBTJ_JPG_RGB) || (clrs==BGBBTJ_JPG_BGR))
	{
//		lsc=(qf&255)*2.56;
		lsc=(qf&255)*3.42;
	
		for(i=0; i<ys1; i++)
			for(j=0; j<xs1; j++)
		{
			k=(i*xs1+j)*4;
			lcr=hdr->lrgba[k+0]; lcg=hdr->lrgba[k+1];
			lcb=hdr->lrgba[k+2]; // lca=hdr->lrgba[k+3];
			ccr=hdr->rgba[k+0]; ccg=hdr->rgba[k+1];
			ccb=hdr->rgba[k+2]; // cca=hdr->rgba[k+3];
			
			dcr=ccr-lcr;	dcg=ccg-lcg;
			dcb=ccb-lcb;	// dca=cca-lca;
//			dcy=(dcr*dcr)+(dcg*dcg)+(dcb*dcb)+(dca*dca);
			dcy=(dcr*dcr)+(dcg*dcg)+(dcb*dcb);
			
//			l=dcy/256;
//			l=dcy/64;
//			l=dcy/16;
//			l=dcy/4;
//			l=dcy/4096;
//			l=dcy*16;
			l=dcy*4;
//			l=dcy*2;
			l=(l*lsc)>>8;
//			l=(l<0)?0:((l>256)?256:l);
			l=(l<64)?64:((l>256)?256:l);
//			l=(l<32)?32:((l>256)?256:l);
//			l=(l<128)?128:((l>256)?256:l);
						
			cr=(((256-l)*lcr)+(l*ccr))>>8;
			cg=(((256-l)*lcg)+(l*ccg))>>8;
			cb=(((256-l)*lcb)+(l*ccb))>>8;
//			cr=(((256-l)*lcr)+(l*ccr))>>8;

			hdr->rgba[k+0]=cr;
			hdr->rgba[k+1]=cg;
			hdr->rgba[k+2]=cb;
		}
	}
#endif

#if 1
	if(qf&BGBBTJ_QFL_MIPMAP)
	{
		xs2=xs1; ys2=ys1; ct0=hdr->rgba;
		while((xs2>1) || (ys2>1))
		{
			if(!xs2)xs2++;
			if(!ys2)ys2++;
			xs3=(xs2+3)/4;
			ys3=(ys2+3)/4;
//			n+=xs3*ys3;
//			ct=ct0+(xs3*ys3)*4;
			ct=ct0+(xs2*ys2)*4;
			BGBBTJ_Tex_HalfSampleDest(ct0, ct, xs2, ys2);
			xs2>>=1; ys2>>=1;
			ct0=ct;
		}
	}
#endif

	if((qf&BGBBTJ_BTIC1C_QFL_INDEXCLR) && (qf&BGBBTJ_QFL_IFRAME) &&
		((clrs==BGBBTJ_JPG_RGBA) || (clrs==BGBBTJ_JPG_RGBX) ||
		 (clrs==BGBBTJ_JPG_RGB) ||
		 (clrs==BGBBTJ_JPG_BGRA) || (clrs==BGBBTJ_JPG_BGRX) ||
		 (clrs==BGBBTJ_JPG_BGR)))
	{
		if(!hdr->pal_clr)
		{
			hdr->pal_nclr=256;
			hdr->pal_clr=gcatomic(hdr->pal_nclr*sizeof(u32));
			hdr->pal_index=gcatomic((32*32*32)*sizeof(u16));
			hdr->pal_chain=gcatomic(2*hdr->pal_nclr*sizeof(u32));
		}

#if 0	
		if(clrs==BGBBTJ_JPG_RGBA)
		{
			BGBBTJ_BT1X_GeneratePaletteRGB24(hdr->rgba, xs1*ys1, 4,
				tpal_rgb, hdr->pal_nclr, 4);
		}else if(clrs==BGBBTJ_JPG_RGB)
		{
			BGBBTJ_BT1X_GeneratePaletteRGB24(hdr->rgba, xs1*ys1, 3,
				tpal_rgb, hdr->pal_nclr, 4);
		}else if(clrs==BGBBTJ_JPG_BGRA)
		{
			BGBBTJ_BT1X_GeneratePaletteBGR24(hdr->rgba, xs1*ys1, 4,
				tpal_rgb, hdr->pal_nclr, 4);
		}else if(clrs==BGBBTJ_JPG_BGR)
		{
			BGBBTJ_BT1X_GeneratePaletteBGR24(hdr->rgba, xs1*ys1, 3,
				tpal_rgb, hdr->pal_nclr, 4);
		}
#endif

		BGBBTJ_BT1X_GeneratePaletteBGR24(hdr->rgba, xs1*ys1, 4,
			tpal_rgb, hdr->pal_nclr, 4);

#if 0
		for(i=0; i<256; i++)
		{
			BGBBTJ_BT1X_UnpackColor555_RGB24(
				bt1c_blkidxcolor[i], tpal_rgb+i*4);
//			tpal_rgb[i*4+0]=
		}
#endif

		BGBBTJ_BT1X_GeneratePaletteIndex555_RGB24(
			tpal_rgb, hdr->pal_nclr, 4, hdr->pal_index,
			hdr->pal_chain, 2*hdr->pal_nclr);
		for(i=0; i<hdr->pal_nclr; i++)
		{
			j=	(tpal_rgb[i*4+0]<<16) |
				(tpal_rgb[i*4+1]<<8) |
				(tpal_rgb[i*4+2]);
			hdr->pal_clr[i]=j;
		}
	}
	
//	ldat=hdr->ldata;
//	if(qf&BGBBTJ_QFL_IFRAME)
//		ldat=NULL;

	ldat=NULL;
	if(qf&BGBBTJ_QFL_PFRAME)
		ldat=hdr->ldata;

	switch(clrs)
	{
	case BGBBTJ_JPG_RGBA:
	case BGBBTJ_JPG_BGRA:
	case BGBBTJ_JPG_RGBX:
	case BGBBTJ_JPG_BGRX:
		if(qf&BGBBTJ_QFL_MIPMAP)
		{
			efl|=BGBBTJ_BTIC1C_EFL_MIPMAP;
		
			BGBBTJ_BCn_EncodeImageDXT5B_Mip(hdr->fpdata,
				hdr->rgba, xs1, ys1, 4);
			BGBBTJ_BCn_FlipImageDXT5_Mip(hdr->fpdata, hdr->data, xs1, ys1);
			BGBBTJ_BT1C_QuantizeBlocksDXT5(hdr, hdr->data, hdr->data, ldat,
				xs1, ys1, qf);
//			sz=BGBBTJ_BT1C_EncodeBlocksDXT5(obuf, osz,
//				hdr->data, ldat, xs, ys);
			sz=BGBBTJ_BT1C_EncodeBlocksDXT5Z(hdr, obuf, osz,
				hdr->data, ldat, xs1, ys1, efl);
			bsz=16;
		}else
		{
#if 0
			BGBBTJ_BCn_EncodeImageDXT1A(hdr->fpdata, rgba, xs, ys, 4);
//			BGBBTJ_BCn_EncodeImageDXT1F(hdr->fpdata, rgba, xs, ys, 4);
			BGBBTJ_BCn_FlipImageDXT1(hdr->fpdata, hdr->data, xs, ys, 8);
			sz=BGBBTJ_BT1C_EncodeBlocksDXT1(hdr, obuf, osz,
				hdr->data, ldat, xs, ys);
			bsz=8;
#endif
#if 0
			BGBBTJ_BCn_EncodeImageDXT5B(hdr->fpdata, hdr->rgba, xs1, ys1, 4);
//			BGBBTJ_BCn_EncodeImageDXT5(hdr->fpdata, hdr->rgba, xs, ys, 4);
//			BGBBTJ_BCn_EncodeImageDXT5(hdr->fpdata, rgba, xs, ys, 4);
//			BGBBTJ_BCn_EncodeImageDXT1F(hdr->fpdata, rgba, xs, ys, 4);
			BGBBTJ_BCn_FlipImageDXT5(hdr->fpdata, hdr->data, xs1, ys1);
			BGBBTJ_BT1C_QuantizeBlocksDXT5(hdr, hdr->data, hdr->data, ldat,
				xs1, ys1, qf);
//			sz=BGBBTJ_BT1C_EncodeBlocksDXT5(obuf, osz,
//				hdr->data, ldat, xs, ys);
			sz=BGBBTJ_BT1C_EncodeBlocksDXT5Z(hdr, obuf, osz,
				hdr->data, ldat, xs1, ys1, efl);
			bsz=16;
#endif

#if 1
			BGBBTJ_BCn_EncodeImageMB1CF_FlipBGRA(
				hdr->data, hdr->rgba, xs1, ys1, 4);

//			BGBBTJ_BCn_EncodeImageMB5C(hdr->fpdata, hdr->rgba, xs1, ys1, 4);
//			BGBBTJ_BCn_FlipImageMB5C(hdr->fpdata, hdr->data, xs1, ys1);

			BGBBTJ_BT1C_QuantizeBlocksMB5C(hdr, hdr->data, hdr->data, ldat,
				xs1, ys1, qf);
			sz=BGBBTJ_BT1C_EncodeBlocksMB5CZ(hdr, obuf, osz,
				hdr->data, ldat, xs1, ys1, efl);
			bsz=32;
#endif
		}
		break;

	case BGBBTJ_JPG_RGB:
	case BGBBTJ_JPG_BGR:
#if 0
		BGBBTJ_BCn_EncodeImageDXT1A(hdr->fpdata, hdr->rgba, xs, ys, 4);
		BGBBTJ_BCn_FlipImageDXT1(hdr->fpdata, hdr->data, xs, ys, 8);
		BGBBTJ_BT1C_QuantizeBlocksDXT1(hdr, hdr->data, hdr->data, ldat,
			xs, ys, qf);
		sz=BGBBTJ_BT1C_EncodeBlocksDXT1Z(hdr, obuf, osz,
			hdr->data, ldat, xs, ys, efl);
		bsz=8;

//		BGBBTJ_BCn_EncodeImageDXT1A(hdr->fpdata, hdr->rgba, xs, ys, 4);
#endif
#if 1
		BGBBTJ_BCn_EncodeImageMB1CF_FlipBGRA(
			hdr->data, hdr->rgba, xs1, ys1, 4);

//		BGBBTJ_BCn_EncodeImageMB5C_BGRA(hdr->fpdata, hdr->rgba, xs1, ys1, 4);
//		BGBBTJ_BCn_EncodeImageMB5C(hdr->fpdata, hdr->rgba, xs1, ys1, 4);
//		BGBBTJ_BCn_FlipImageMB5C(hdr->fpdata, hdr->data, xs1, ys1);
		BGBBTJ_BT1C_QuantizeBlocksMB5C(hdr, hdr->data, hdr->data, ldat,
			xs1, ys1, qf);
		sz=BGBBTJ_BT1C_EncodeBlocksMB5CZ(hdr, obuf, osz,
			hdr->data, ldat, xs1, ys1, efl);
		bsz=32;
#endif
		break;

	case BGBBTJ_JPG_RGBA_F16:
		BGBBTJ_BCn_EncodeImageMB5C_F16(hdr->fpdata, (u16 *)rgba, xs, ys, 4);
		BGBBTJ_BCn_FlipImageMB5C(hdr->fpdata, hdr->data, xs, ys);
//		BGBBTJ_BT1C_QuantizeBlocksMB5C(hdr, hdr->data, hdr->data, ldat,
//			xs, ys, qf);
		sz=BGBBTJ_BT1C_EncodeBlocksMB5CZ(hdr, obuf, osz,
			hdr->data, ldat, xs, ys, efl);
		bsz=32;
		break;
	case BGBBTJ_JPG_RGB_F16:
		BGBBTJ_BCn_EncodeImageMB1C_F16(hdr->fpdata, (u16 *)rgba, xs, ys, 3);
		BGBBTJ_BCn_FlipImageMB5C(hdr->fpdata, hdr->data, xs, ys);
//		BGBBTJ_BT1C_QuantizeBlocksMB5C(hdr, hdr->data, hdr->data, ldat,
//			xs, ys, qf);
		sz=BGBBTJ_BT1C_EncodeBlocksMB5CZ(hdr, obuf, osz,
			hdr->data, ldat, xs, ys, efl);
		bsz=32;
		break;
	case BGBBTJ_JPG_RGBX_F16:
		BGBBTJ_BCn_EncodeImageMB1C_F16(hdr->fpdata, (u16 *)rgba, xs, ys, 4);
		BGBBTJ_BCn_FlipImageMB5C(hdr->fpdata, hdr->data, xs, ys);
//		BGBBTJ_BT1C_QuantizeBlocksMB5C(hdr, hdr->data, hdr->data, ldat,
//			xs, ys, qf);
		sz=BGBBTJ_BT1C_EncodeBlocksMB5CZ(hdr, obuf, osz,
			hdr->data, ldat, xs, ys, efl);
		bsz=32;
		break;

#if 0
	case BGBBTJ_JPG_BGRA_F16:
		BGBBTJ_BCn_EncodeImageMB5C_F16_BGRA(hdr->fpdata, rgba, xs, ys, 4);
		BGBBTJ_BCn_FlipImageMB5C(hdr->fpdata, hdr->data, xs, ys);
//		BGBBTJ_BT1C_QuantizeBlocksMB5C(hdr, hdr->data, hdr->data, ldat,
//			xs, ys, qf);
		sz=BGBBTJ_BT1C_EncodeBlocksMB5CZ(hdr, obuf, osz,
			hdr->data, ldat, xs, ys, efl);
		bsz=32;
		break;
	case BGBBTJ_JPG_BGR_F16:
		BGBBTJ_BCn_EncodeImageMB1C_F16_BGRA(hdr->fpdata, rgba, xs, ys, 3);
		BGBBTJ_BCn_FlipImageMB5C(hdr->fpdata, hdr->data, xs, ys);
//		BGBBTJ_BT1C_QuantizeBlocksMB5C(hdr, hdr->data, hdr->data, ldat,
//			xs, ys, qf);
		sz=BGBBTJ_BT1C_EncodeBlocksMB5CZ(hdr, obuf, osz,
			hdr->data, ldat, xs, ys, efl);
		bsz=32;
		break;
	case BGBBTJ_JPG_BGRX_F16:
		BGBBTJ_BCn_EncodeImageMB1C_F16_BGRA(hdr->fpdata, rgba, xs, ys, 4);
		BGBBTJ_BCn_FlipImageMB5C(hdr->fpdata, hdr->data, xs, ys);
//		BGBBTJ_BT1C_QuantizeBlocksMB5C(hdr, hdr->data, hdr->data, ldat,
//			xs, ys, qf);
		sz=BGBBTJ_BT1C_EncodeBlocksMB5CZ(hdr, obuf, osz,
			hdr->data, ldat, xs, ys, efl);
		bsz=32;
		break;
#endif

#if 0
	case BGBBTJ_JPG_RGB:
		BGBBTJ_BCn_EncodeImageDXT1A(hdr->fpdata, rgba, xs, ys, 3);
//		BGBBTJ_BCn_EncodeImageDXT1F(hdr->fpdata, rgba, xs, ys, 3);
		BGBBTJ_BCn_FlipImageDXT1(hdr->fpdata, hdr->data, xs, ys, 8);
		BGBBTJ_BT1C_QuantizeBlocksDXT1(hdr, hdr->data, hdr->data, ldat,
			xs, ys, qf);
		sz=BGBBTJ_BT1C_EncodeBlocksDXT1Z(hdr, obuf, osz,
			hdr->data, ldat, xs, ys, efl);
		bsz=8;
		break;
	case BGBBTJ_JPG_BGRA:
#if 0
		BGBBTJ_BCn_EncodeImageDXT1A_BGRA(hdr->fpdata, rgba, xs, ys, 4);
		BGBBTJ_BCn_FlipImageDXT1(hdr->fpdata, hdr->data, xs, ys, 8);
		BGBBTJ_BT1C_QuantizeBlocksDXT1(hdr, hdr->data, hdr->data, ldat,
			xs, ys, qf);
		sz=BGBBTJ_BT1C_EncodeBlocksDXT1Z(hdr, obuf, osz,
			hdr->data, ldat, xs, ys, efl);
		bsz=8;
#endif
#if 0
		BGBBTJ_BCn_EncodeImageMB5C_BGRA(hdr->fpdata, rgba, xs, ys, 4);
		BGBBTJ_BCn_FlipImageMB5C(hdr->fpdata, hdr->data, xs, ys);
		BGBBTJ_BT1C_QuantizeBlocksMB5C(hdr, hdr->data, hdr->data, ldat,
			xs, ys, qf);
		sz=BGBBTJ_BT1C_EncodeBlocksMB5CZ(hdr, obuf, osz,
			hdr->data, ldat, xs, ys, efl);
		bsz=32;
#endif
#if 1
		BGBBTJ_BCn_EncodeImageMB5C(hdr->fpdata, hdr->rgba, xs1, ys1, 4);
		BGBBTJ_BCn_FlipImageMB5C(hdr->fpdata, hdr->data, xs1, ys1);
		BGBBTJ_BT1C_QuantizeBlocksMB5C(hdr, hdr->data, hdr->data, ldat,
			xs1, ys1, qf);
		sz=BGBBTJ_BT1C_EncodeBlocksMB5CZ(hdr, obuf, osz,
			hdr->data, ldat, xs1, ys1, efl);
		bsz=32;
#endif
		break;
	case BGBBTJ_JPG_BGR:
//		BGBBTJ_BCn_EncodeImageDXT1F_BGRA(hdr->fpdata, rgba, xs, ys, 3);
		BGBBTJ_BCn_EncodeImageDXT1A_BGRA(hdr->fpdata, rgba, xs, ys, 3);
		BGBBTJ_BCn_FlipImageDXT1(hdr->fpdata, hdr->data, xs, ys, 8);
		BGBBTJ_BT1C_QuantizeBlocksDXT1(hdr, hdr->data, hdr->data, ldat,
			xs, ys, qf);
		sz=BGBBTJ_BT1C_EncodeBlocksDXT1Z(hdr, obuf, osz,
			hdr->data, ldat, xs, ys, efl);
		bsz=8;
		break;
#endif
#if 0
	case BGBBTJ_JPG_RGBA_Q11_4:
		BGBBTJ_BCn_EncodeImageDXT5B(hdr->fpdata, hdr->rgba, xs1, ys1, 4);
		BGBBTJ_BCn_FlipImageDXT5(hdr->fpdata, hdr->data, xs1, ys1);
		BGBBTJ_BT1C_QuantizeBlocksDXT5(hdr, hdr->data, hdr->data, ldat,
			xs1, ys1, qf);
		sz=BGBBTJ_BT1C_EncodeBlocksDXT5Z(hdr, obuf, osz,
			hdr->data, ldat, xs1, ys1, efl);
		i=BGBBTJ_BT1C_EncodeToneMap(obuf+sz, osz-sz,
			hdr->tmm_rgba, hdr->tmn_rgba, xs1, ys1, qf, 4);
		if(i>0)sz+=i;
		bsz=16;
		break;
#endif
	default:
		break;
	}

#if 1
	hdr->stflags&=~BGBBTJ_BTIC1C_STFL_LBJPEG;
//	if(qf&BGBBTJ_BTIC1C_QFL_ALTJPEG)
//	if((qf&BGBBTJ_BTIC1C_QFL_ALTJPEG) && !(xs1&15) && !(ys1&15))
//	if((qf&BGBBTJ_BTIC1C_QFL_ALTJPEG) &&
//		!(xs1&15) && !(ys1&15) && !(qf&BGBBTJ_QFL_PFRAME))
	if((qf&BGBBTJ_BTIC1C_QFL_ALTJPEG) && !(xs1&15) && !(ys1&15) &&
		(!(qf&BGBBTJ_QFL_PFRAME) || (hdr->cnt_jpg_inhibit<=0)))
	{
		tbuf=malloc(1<<24);
		sz2=BGBBTJ_JPG_EncodeComponent(hdr->rgba, NULL, NULL, NULL,
			tbuf, xs1, ys1, qf&255);
//		sz2=BTJ_JFE_EncodeFast(hdr->rgba, tbuf,
//			xs1, ys1, qf&255,
//			BGBBTJ_JPG_RGBA);
		
		if((sz2+256)<sz)
		{
			if(qf&BGBBTJ_QFL_PFRAME)
				hdr->cnt_jpg_inhibit=16;
			hdr->stflags|=BGBBTJ_BTIC1C_STFL_LBJPEG;
			bt1c_stats[4]++;

			memcpy(obuf+6, tbuf, sz2);
			sz2=sz2+6;

			obuf[0]=0xE3;			obuf[1]=(sz2>>16)&255;
			obuf[2]=(sz2>>8)&255;	obuf[3]=(sz2>> 0)&255;
			obuf[4]='J';			obuf[5]='P';
			ct=obuf+sz2;
			
			sz=ct-obuf;
		}else
		{
			if(hdr->cnt_jpg_inhibit>0)
				hdr->cnt_jpg_inhibit--;
		}
		
		free(tbuf);
	}else
	{
		if(hdr->cnt_jpg_inhibit>0)
			hdr->cnt_jpg_inhibit--;
	}
#endif

#if 1
	if(!ldat)
	{
		ct=obuf+sz;

		ct0=ct;
		*ct++=0xE5;		*ct++=0x00;
		*ct++='H';		*ct++='D';

		*ct++=(xs>>8)&255;	*ct++=(xs>>0)&255;
		*ct++=(ys>>8)&255;	*ct++=(ys>>0)&255;
		*ct++=0x18;			*ct++=clrs;
		*ct++=0x00;			*ct++=0x00;
		*ct++=(qf>>24)&255;	*ct++=(qf>>16)&255;
		*ct++=(qf>>8)&255;	*ct++=(qf>>0)&255;
		
		ct0[1]=ct-ct0;
	
		sz=ct-obuf;
	}
#endif

	if(hdr->lrgba)
		{ memcpy(hdr->lrgba, hdr->rgba, hdr->sz_rgba); }

#if 1
	if(!hdr->ldata)
	{
//		i=hdr->nblocks*16;
		i=hdr->nblocksmip*32;
		hdr->ldata=gcatomic(i);
		hdr->sz_ldata=i;
	}
//	memcpy(hdr->ldata, hdr->data, hdr->nblocks*bsz);
	memcpy(hdr->ldata, hdr->data, hdr->nblocksmip*bsz);
#endif

	return(sz);
}

byte *BGBBTJ_BT1C_EmitCompLayerHeader(
	BGBBTJ_BTIC1C_Context *ctx, BGBBTJ_BTIC1C_ImageInfo *img,
	byte *cts, byte *cte)
{
	byte *ct;
	char *s;
	
	ct=cts;
	*ct++=0xE5;		*ct++=0x00;
	*ct++='C';		*ct++='L';

	s=img->name;
	while(*s) { *ct++=*s++; }
	*ct++=0;
	*ct++=0;
	cts[1]=ct-cts;
	
	return(ct);
}

byte *BGBBTJ_BT1C_EmitTagLayerHeader(
	BGBBTJ_BTIC1C_Context *ctx, BGBBTJ_BTIC1C_LayerInfo *img,
	byte *cts, byte *cte)
{
	byte *ct;
	char *s;
	
	ct=cts;
	*ct++=0xE5;		*ct++=0x00;
	*ct++='T';		*ct++='L';

	s=img->name;
	while(*s) { *ct++=*s++; }
	*ct++=0;
	*ct++=0;
	cts[1]=ct-cts;
	
	return(ct);
}

BGBBTJ_API byte *BGBBTJ_BT1C_EmitLayerImage(
	BGBBTJ_BTIC1C_Context *ctx, BGBBTJ_BTIC1C_ImageInfo *img,
	byte *ctt, byte *cte, int qfl)
{
	byte *ldat;
	byte *ct;
	int i, bsz;

	ldat=NULL;
	if(qfl&BGBBTJ_QFL_PFRAME)
		ldat=img->ldata;

	ct=BGBBTJ_BT1C_EmitCompLayerHeader(ctx, img, ctt, cte);
	switch(img->imgtype)
	{
	case BGBBTJ_JPG_DXT1:
	case BGBBTJ_JPG_DXT1F:
	case BGBBTJ_JPG_DXT1A:
		i=BGBBTJ_BT1C_EncodeBlocksDXT1(img, ct, cte-ct, img->data, ldat,
			img->width, img->height, 0);
		ct+=i; bsz=8;
		break;
	case BGBBTJ_JPG_DXT5:
	case BGBBTJ_JPG_DXT5F:
		i=BGBBTJ_BT1C_EncodeBlocksDXT5(img, ct, cte-ct, img->data, ldat,
			img->width, img->height, 0);
		ct+=i; bsz=16;
		break;
	default:
		break;
	}

#if 1
	if(!img->ldata)
	{
		i=img->nblocks*16;
		img->ldata=gcatomic(i);
		img->sz_ldata=i;
	}
	memcpy(img->ldata, img->data, img->nblocks*bsz);
#endif
	
	return(ct);
//	i=BGBBTJ_BT1C_EncodeImage(img, norm, ct, cte-ct, xs, ys, qf, clrs);
}

BGBBTJ_API byte *BGBBTJ_BT1C_EmitTagLayer(
	BGBBTJ_BTIC1C_Context *ctx, BGBBTJ_BTIC1C_LayerInfo *layer,
	byte *ctt, byte *cte, int qfl)
{
	byte *ct;
	int i;
	
	ct=ctt;
	ct=BGBBTJ_BT1C_EmitTagLayerHeader(ctx, layer, ctt, cte);
	for(i=0; i<layer->n_image; i++)
	{
		ct=BGBBTJ_BT1C_EmitLayerImage(ctx,
			layer->image[i], ct, cte, qfl);
	}
	return(ct);
}

#if 0
// BGBBTJ_API int BGBBTJ_BT1C_EncodeLayerComponentImage(
	BGBBTJ_BTIC1C_Context *ctx, BGBBTJ_BTIC1C_LayerInfo *layer,
	byte *rgba, byte *norm, byte *spec, byte *luma,
	byte *obuf, int osz,
	int xs, int ys, int qf, int clrs)
{
	BGBBTJ_BTIC1C_ImageInfo *img;
	byte *ct, *cte;

	ct=obuf; cte=obuf+osz;

	img=BTIC1_GetLayerImage(ctx, layer, "RGBA");
	ct=BGBBTJ_BT1C_EmitCompLayerHeader(ctx, img, ct, cte);
	BGBBTJ_BT1C_EncodeImage(ctx, img, rgba,
		ct, cte-ct, xs, ys, qf, clrs);

	if(norm)
	{
		img=BTIC1_GetLayerImage(ctx, layer, "XYZD");
		ct=BGBBTJ_BT1C_EmitCompLayerHeader(ctx, img, ct, cte);
		BGBBTJ_BT1C_EncodeImage(ctx, img, norm,
			ct, cte-ct, xs, ys, qf, clrs);
	}

	if(spec)
	{
		img=BTIC1_GetLayerImage(ctx, layer, "SpRGB");
		ct=BGBBTJ_BT1C_EmitCompLayerHeader(ctx, img, ct, cte);
		BGBBTJ_BT1C_EncodeImage(ctx, img, spec,
			ct, cte-ct, xs, ys, qf, clrs);
	}

	if(luma)
	{
		img=BTIC1_GetLayerImage(ctx, layer, "LuRGB");
		ct=BGBBTJ_BT1C_EmitCompLayerHeader(ctx, img, ct, cte);
		BGBBTJ_BT1C_EncodeImage(ctx, img, luma,
			ct, cte-ct, xs, ys, qf, clrs);
	}
	
	return(ct-obuf);
}
#endif
