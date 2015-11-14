/*
Common Code shared between BTIC1C decoder backends.
*/

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

extern int bt1c_basedifftab5[16][32];

extern byte bt1c_hf12tob[4096];		//half-float (bits 4-15) to byte
extern u16 bt1c_btohf[256];			//byte to half-float

extern int bt1c_basedifftab[8][5];
extern u32 bt1c_blkidx_pixpat[256];	//1BPP -> RPZA Pixels

int BGBBTJ_BT1C_PredictDiffColorA(BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *blks, byte *ct, int xs1, int ys1, int stride)
{
	return(BGBBTJ_BT1C_PredictDiffColorModeA(ctx,
		blks, ct, xs1, ys1, stride, ctx->diff_m));
}

int BGBBTJ_BT1C_PredictDiffColorB(BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *blks, byte *ct, int xs1, int ys1, int stride)
{
	return(BGBBTJ_BT1C_PredictDiffColorModeB(ctx,
		blks, ct, xs1, ys1, stride, ctx->diff_m));
}

int BGBBTJ_BT1C_PredictDiffColorModeA(BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *blks, byte *ct, int xs1, int ys1, int stride, int mode)
{
	byte *ct0, *ct1, *ct2;
	int p0, p1, p2, p3;

	switch(mode)
	{
	case 0:
		p3=0x0000;
		break;
	case 1:
		ct2=ct-stride;
		if(ct2>=blks)p2=(ct2[0])|((ct2[1])<<8);
			else p2=0x0000;
		p3=p2;
		break;
	case 2:
		ct0=ct-xs1*stride-stride;
		ct1=ct-xs1*stride;
		ct2=ct-stride;
		if(ct0>=blks)p0=(ct0[0])|((ct0[1])<<8);
			else p0=0x0000;
		if(ct1>=blks)p1=(ct1[0])|((ct1[1])<<8);
			else p1=0x0000;
		if(ct2>=blks)p2=(ct2[0])|((ct2[1])<<8);
			else p2=0x0000;
		p3=BGBBTJ_BT1C_Paeth(p2, p1, p0);
		break;
	case 3:
		ct0=ct-xs1*stride-stride;
		ct1=ct-xs1*stride;
		ct2=ct-stride;
		if(ct0>=blks)p0=(ct0[0])|((ct0[1])<<8);
			else p0=0x0000;
		if(ct1>=blks)p1=(ct1[0])|((ct1[1])<<8);
			else p1=0x0000;
		if(ct2>=blks)p2=(ct2[0])|((ct2[1])<<8);
			else p2=0x0000;
		p3=BGBBTJ_BT1C_Paeth2(p2, p1, p0);
		break;
	case 4:
		p0=ctx->diff_lclra;
		p3=p0;
//		p3=BGBBTJ_BT1C_ConvRGB24ToRGB16(ctx, p0);
		break;
	default:
		p3=0; break;
	}

	return(p3);
}

int BGBBTJ_BT1C_PredictDiffColorModeB(BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *blks, byte *ct, int xs1, int ys1, int stride, int mode)
{
	byte *ct0, *ct1, *ct2;
	int p0, p1, p2, p3;

	switch(mode)
	{
	case 0:
		p3=0x0000;
		break;
	case 1:
		ct2=ct-stride;
		if(ct2>=blks)p2=(ct2[2])|((ct2[3])<<8);
			else p2=0x0000;
		p3=p2;
		break;
	case 2:
		ct0=ct-xs1*stride-stride;
		ct1=ct-xs1*stride;
		ct2=ct-stride;
		if(ct0>=blks)p0=(ct0[2])|((ct0[3])<<8);
			else p0=0x0000;
		if(ct1>=blks)p1=(ct1[2])|((ct1[3])<<8);
			else p1=0x0000;
		if(ct2>=blks)p2=(ct2[2])|((ct2[3])<<8);
			else p2=0x0000;
		p3=BGBBTJ_BT1C_Paeth(p2, p1, p0);
		break;
	case 3:
		ct0=ct-xs1*stride-stride;
		ct1=ct-xs1*stride;
		ct2=ct-stride;
		if(ct0>=blks)p0=(ct0[2])|((ct0[3])<<8);
			else p0=0x0000;
		if(ct1>=blks)p1=(ct1[2])|((ct1[3])<<8);
			else p1=0x0000;
		if(ct2>=blks)p2=(ct2[2])|((ct2[3])<<8);
			else p2=0x0000;
		p3=BGBBTJ_BT1C_Paeth2(p2, p1, p0);
		break;
	case 4:
		p0=ctx->diff_lclrb;
		p3=p0;
//		p3=BGBBTJ_BT1C_ConvRGB24ToRGB16(ctx, p0);
		break;
	default:
		p3=0; break;
	}

	return(p3);
}


int BGBBTJ_BT1C_PredictDiffColorA24(BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *blks, byte *ct, int xs1, int ys1, int stride)
{
	return(BGBBTJ_BT1C_PredictDiffColorModeA24(ctx,
		blks, ct, xs1, ys1, stride, ctx->diff_m));
}

int BGBBTJ_BT1C_PredictDiffColorB24(BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *blks, byte *ct, int xs1, int ys1, int stride)
{
	return(BGBBTJ_BT1C_PredictDiffColorModeB24(ctx,
		blks, ct, xs1, ys1, stride, ctx->diff_m));
}

int BGBBTJ_BT1C_PredictDiffColorModeA24(BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *blks, byte *ct, int xs1, int ys1, int stride, int mode)
{
	byte *ct0, *ct1, *ct2;
	int p0, p1, p2, p3;

	switch(mode)
	{
	case 0:
		p3=0x000000;
		break;
	case 1:
		ct2=ct-stride;
		if(ct2>=blks)p2=(ct2[0])|((ct2[1])<<8);
			else p2=0x000000;
		p3=p2;
		break;
	case 2:
		ct0=ct-xs1*stride-stride;
		ct1=ct-xs1*stride;
		ct2=ct-stride;
		if(ct0>=blks)p0=(ct0[0])|((ct0[1])<<8);
			else p0=0x000000;
		if(ct1>=blks)p1=(ct1[0])|((ct1[1])<<8);
			else p1=0x000000;
		if(ct2>=blks)p2=(ct2[0])|((ct2[1])<<8);
			else p2=0x000000;
		p3=BGBBTJ_BT1C_Paeth(p2, p1, p0);
		break;
	case 3:
		ct0=ct-xs1*stride-stride;
		ct1=ct-xs1*stride;
		ct2=ct-stride;
		if(ct0>=blks)p0=(ct0[0])|((ct0[1])<<8);
			else p0=0x000000;
		if(ct1>=blks)p1=(ct1[0])|((ct1[1])<<8);
			else p1=0x000000;
		if(ct2>=blks)p2=(ct2[0])|((ct2[1])<<8);
			else p2=0x000000;
		p3=BGBBTJ_BT1C_Paeth2(p2, p1, p0);
		break;
	case 4:
		p3=ctx->diff_lclra;
//		p3=BGBBTJ_BT1C_ConvRGB24ToRGB16(ctx, p0);
		break;
	default:
		p3=0; break;
	}

	return(p3);
}

int BGBBTJ_BT1C_PredictDiffColorModeB24(BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *blks, byte *ct, int xs1, int ys1, int stride, int mode)
{
	byte *ct0, *ct1, *ct2;
	int p0, p1, p2, p3;

	switch(mode)
	{
	case 0:
		p3=0x000000;
		break;
	case 1:
		ct2=ct-stride;
		if(ct2>=blks)p2=(ct2[2])|((ct2[3])<<8);
			else p2=0x000000;
		p3=p2;
		break;
	case 2:
		ct0=ct-xs1*stride-stride;
		ct1=ct-xs1*stride;
		ct2=ct-stride;
		if(ct0>=blks)p0=(ct0[2])|((ct0[3])<<8);
			else p0=0x0000;
		if(ct1>=blks)p1=(ct1[2])|((ct1[3])<<8);
			else p1=0x0000;
		if(ct2>=blks)p2=(ct2[2])|((ct2[3])<<8);
			else p2=0x0000;
		p3=BGBBTJ_BT1C_Paeth(p2, p1, p0);
		break;
	case 3:
		ct0=ct-xs1*stride-stride;
		ct1=ct-xs1*stride;
		ct2=ct-stride;
		if(ct0>=blks)p0=(ct0[2])|((ct0[3])<<8);
			else p0=0x0000;
		if(ct1>=blks)p1=(ct1[2])|((ct1[3])<<8);
			else p1=0x0000;
		if(ct2>=blks)p2=(ct2[2])|((ct2[3])<<8);
			else p2=0x0000;
		p3=BGBBTJ_BT1C_Paeth2(p2, p1, p0);
		break;
	case 4:
		p3=ctx->diff_lclrb;
//		p3=BGBBTJ_BT1C_ConvRGB24ToRGB16(ctx, p0);
		break;
	default:
		p3=0; break;
	}

	return(p3);
}

u16 BGBBTJ_BT1C_LinearExtrapolate565(u16 clra, u16 clrb)
{
	int cr, cg, cb;
	u16 clrc;

	cr=2*(clrb&0xF800)-(clra&0xF800);
	cg=2*(clrb&0x07E0)-(clra&0x07E0);
	cb=2*(clrb&0x001F)-(clra&0x001F);
	
	if(cr<0)cr=0;
	if(cg<0)cg=0;
	if(cb<0)cb=0;
	if(cr&0x10000)cr=0xF800;
	if(cg&0x00800)cg=0x07E0;
	if(cb&0x00020)cb=0x001F;

	clrc=cr|cg|cb;

//	clrc=2*clrb-clra;
	return(clrc);
}

int BGBBTJ_BT1C_AddDiff15ToClr24(BGBBTJ_BTIC1C_ImageInfo *ctx,
	int diff, int clr, int sc)
{
	int i, cr, cg, cb, p;
	int dr, dg, db;
	
	cr=(clr&0xFF0000)>>16;
	cg=(clr&0x00FF00)>>8;
	cb=(clr&0x0000FF);

//	dr=(diff&0x7C00)>>10;
//	dg=(diff&0x03E0)>>5;
//	db=(diff&0x001F);

	dr=(diff>>10)&31;
	dg=(diff>> 5)&31;
	db=(diff    )&31;

	if(sc>0)
	{
		dr=bt1c_basedifftab5[sc][dr];
		dg=bt1c_basedifftab5[sc][dg];
		db=bt1c_basedifftab5[sc][db];
	}else
	{
		dr=dr-16;
		dg=dg-16;
		db=db-16;
	}

	cr=cr+dr;
	cg=cg+dg;
	cb=cb+db;

	cr=(cr>255)?255:((cr<0)?0:cr);
	cg=(cg>255)?255:((cg<0)?0:cg);
	cb=(cb>255)?255:((cb<0)?0:cb);

//	cr=cr|(cr>>5);
//	cg=cg|(cg>>5);
//	cb=cb|(cb>>5);

	i=(cr<<16)|(cg<<8)|cb;
	return(i);
}

int BGBBTJ_BT1C_ConvDiff15ToDiff24(BGBBTJ_BTIC1C_ImageInfo *ctx,
	int diff)
{
	int i, cr, cg, cb, p;
	
	cr=(diff&0x7C00)>>7;
	cg=(diff&0x03E0)>>2;
	cb=(diff&0x001F)<<3;

//	cr=cr|(cr>>5);
//	cg=cg|(cg>>5);
//	cb=cb|(cb>>5);

	i=(cr<<16)|(cg<<8)|cb;
	return(i);
}

int BGBBTJ_BT1C_AddPred24ToDiff15(BGBBTJ_BTIC1C_ImageInfo *ctx,
	int clr, int diff)
{
	int i, cr, cg, cb, p;
	int dr, dg, db;
	
	cr=(clr&0xFF0000)>>16;
	cg=(clr&0x00FF00)>>8;
	cb=(clr&0x0000FF);

	dr=(diff&0x7C00)>>10;
	dg=(diff&0x03E0)>>5;
	db=(diff&0x001F);

	if(ctx->diff_sc)
	{
		dr=bt1c_basedifftab5[ctx->diff_sc][dr];
		dg=bt1c_basedifftab5[ctx->diff_sc][dg];
		db=bt1c_basedifftab5[ctx->diff_sc][db];
	}else
	{
		dr=dr-16;
		dg=dg-16;
		db=db-16;
	}

	cr=cr+dr;
	cg=cg+dg;
	cb=cb+db;

	cr=(cr>255)?255:((cr<0)?0:cr);
	cg=(cg>255)?255:((cg<0)?0:cg);
	cb=(cb>255)?255:((cb<0)?0:cb);

//	cr=cr|(cr>>5);
//	cg=cg|(cg>>5);
//	cb=cb|(cb>>5);

	i=(cr<<16)|(cg<<8)|cb;
	return(i);
}

int BGBBTJ_BT1C_CalcDiff15ColorA24(BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *blks, byte *ct, int xs1, int ys1, int stride, int diff)
{
	u32 p0, p1;
	p0=BGBBTJ_BT1C_PredictDiffColorA24(ctx, blks, ct, xs1, ys1, stride);
//	p1=BGBBTJ_BT1C_AddPred24ToDiff15(ctx, p0, diff);
	p1=BGBBTJ_BT1C_AddDiff15ToClr24(ctx, diff, p0, ctx->diff_sc);
	return(p1);
}

int BGBBTJ_BT1C_CalcDiff15ColorB24(BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *blks, byte *ct, int xs1, int ys1, int stride, int diff)
{
	u32 p0, p1;
	p0=BGBBTJ_BT1C_PredictDiffColorB24(ctx, blks, ct, xs1, ys1, stride);
//	p1=BGBBTJ_BT1C_AddPred24ToDiff15(ctx, p0, diff);
	p1=BGBBTJ_BT1C_AddDiff15ToClr24(ctx, diff, p0, ctx->diff_sc);
	return(p1);
}

int BGBBTJ_BT1C_ConvRGB23ToRGB15(BGBBTJ_BTIC1C_ImageInfo *ctx, u32 clr)
{
	int i;

	if(ctx->dflags&BGBBTJ_BTIC1C_DFL_RGB23B)
	{
		//xrrr-rrrr pggg-gggg gbbb-bbbb
		//xrrr-rr00 0ggg-gg00 0bbb-bb00
		//          xrrr-rrgg gggb-bbbb
		i=	((clr&0x7C0000)>>8)|
			((clr&0x007C00)>>5)|
			((clr&0x00007C)>>2);
	}else
	{
		//xprr-rrrr rggg-gggg gbbb-bbbb
		//xprr-rrr0 0ggg-gg00 0bbb-bb00
		//          xrrr-rrgg gggb-bbbb
		i=	((clr&0x3E0000)>>7)|
			((clr&0x007C00)>>5)|
			((clr&0x00007C)>>2);
	}
	return(i);
}

int BGBBTJ_BT1C_ConvRGB31ToRGB15(BGBBTJ_BTIC1C_ImageInfo *ctx, u32 clr)
{
	int i;
	i=	((clr&0x3E000000)>>15)|
		((clr&0x007C0000)>>13)|
		((clr&0x00007C00)>>10);
	return(i);
}

int BGBBTJ_BT1C_ConvRGB31FUToRGB15(BGBBTJ_BTIC1C_ImageInfo *ctx, u32 clr)
{
	int cr, cg, cb;
	int i;
	cr=(clr>>20)&1023;
	cg=(clr>>10)&1023;
	cb=(clr    )&1023;
	cr=(cr<<1)|((clr>>30)&1);
	cg=(cg<<1)|((clr>>30)&1);
	cb=(cb<<1)|((clr>>30)&1);
	cr=bt1c_hf12tob[cr];
	cg=bt1c_hf12tob[cg];
	cb=bt1c_hf12tob[cb];
	i=((cr&0xF8)<<7)|((cg&0xF8)<<2)|((cb&0xF8)>>3);
	return(i);
}

int BGBBTJ_BT1C_ConvRGB31FSToRGB15(BGBBTJ_BTIC1C_ImageInfo *ctx, u32 clr)
{
	int cr, cg, cb;
	int i;
	cr=(clr>>20)&1023;
	cg=(clr>>10)&1023;
	cb=(clr    )&1023;
	cr=(cr<<1)|((clr>>30)&1);
	cg=(cg<<1)|((clr>>30)&1);
	cb=(cb<<1)|((clr>>30)&1);
	cr=bt1c_hf12tob[cr<<1];
	cg=bt1c_hf12tob[cg<<1];
	cb=bt1c_hf12tob[cb<<1];
	i=((cr&0xF8)<<7)|((cg&0xF8)<<2)|((cb&0xF8)>>3);
	return(i);
}

int BGBBTJ_BT1C_ConvRGB23ToRGB16(BGBBTJ_BTIC1C_ImageInfo *ctx, u32 clr)
{
	int i;

	if(ctx->dflags&BGBBTJ_BTIC1C_DFL_RGB23B)
	{
		//xrrr-rrrr pggg-gggg gbbb-bbbb
		//xrrr-rr00 0ggg-gg00 0bbb-bb00
		//          rrrr-rggg gggb-bbbb
		i=	((clr&0x7C0000)>>7)|
			((clr&0x007C00)>>5)|
			((clr&0x00007C)>>2);
	}else
	{
		//xprr-rrrr rggg-gggg gbbb-bbbb
		//xprr-rrr0 0ggg-ggg0 0bbb-bb00
		//          rrrr-rggg gggb-bbbb
		i=	((clr&0x3E0000)>>6)|
			((clr&0x007E00)>>4)|
			((clr&0x00007C)>>2);
	}
	return(i);
}

u32 BGBBTJ_BT1C_ConvRGB15ToRGB24(BGBBTJ_BTIC1C_ImageInfo *ctx, u16 clr)
{
	int i, cr, cg, cb, p;
	
	cr=(clr&0x7C00)>>7;
	cg=(clr&0x03E0)>>2;
	cb=(clr&0x001F)<<3;

	cr=cr|(cr>>5);
	cg=cg|(cg>>5);
	cb=cb|(cb>>5);

	i=(cr<<16)|(cg<<8)|cb;
	return(i);
}

u32 BGBBTJ_BT1C_ConvRGB16ToRGB24(BGBBTJ_BTIC1C_ImageInfo *ctx, u16 clr)
{
	int i, cr, cg, cb, p;
	
	cr=(clr&0xF800)>>8;
	cg=(clr&0x07E0)>>3;
	cb=(clr&0x001F)<<3;

	cr=cr|(cr>>5);
	cg=cg|(cg>>6);
	cb=cb|(cb>>5);

	i=(cr<<16)|(cg<<8)|cb;
	return(i);
}

u32 BGBBTJ_BT1C_ConvRGB23ToRGB24(BGBBTJ_BTIC1C_ImageInfo *ctx, u32 clr)
{
	int i, cr, cg, cb, p;
	
	if(ctx->dflags&BGBBTJ_BTIC1C_DFL_RGB23B)
	{
		i=(clr<<1)|((clr>>15)&1);
	}else
	{
		cr=(clr&0x3F8000)>>14;
		cg=(clr&0x007F80)>>7;
		cb=(clr&0x00007F)<<1;
		p=(clr>>22)&1;
		cr|=p; cb|=p;
		i=(cr<<16)|(cg<<8)|cb;
	}
	return(i);
}

u32 BGBBTJ_BT1C_ConvRGB31FUToRGB24(BGBBTJ_BTIC1C_ImageInfo *ctx, u32 clr)
{
	int cr, cg, cb;
	int i;
	cr=(clr>>20)&1023;
	cg=(clr>>10)&1023;
	cb=(clr    )&1023;
	cr=(cr<<1)|((clr>>30)&1);
	cg=(cg<<1)|((clr>>30)&1);
	cb=(cb<<1)|((clr>>30)&1);
	if(ctx && (ctx->dflags&BGBBTJ_BTIC1C_DFL_SCALEHDR2LDR))
	{
		i=ctx->optcntrl[BGBBTJ_BTIC1C_CTRL_SCALEHDR];
		cr=BGBBTJ_MulFloat16F(cr, i);
		cg=BGBBTJ_MulFloat16F(cg, i);
		cb=BGBBTJ_MulFloat16F(cb, i);
	}
	cr=bt1c_hf12tob[cr];
	cg=bt1c_hf12tob[cg];
	cb=bt1c_hf12tob[cb];
	i=(cr<<16)|(cg<<8)|cb;
	return(i);
}

u32 BGBBTJ_BT1C_ConvRGB31FSToRGB24(BGBBTJ_BTIC1C_ImageInfo *ctx, u32 clr)
{
	int cr, cg, cb;
	int i;
	cr=(clr>>20)&1023;
	cg=(clr>>10)&1023;
	cb=(clr    )&1023;
	cr=(cr<<1)|((clr>>30)&1);
	cg=(cg<<1)|((clr>>30)&1);
	cb=(cb<<1)|((clr>>30)&1);
	if(ctx && (ctx->dflags&BGBBTJ_BTIC1C_DFL_SCALEHDR2LDR))
	{
		i=ctx->optcntrl[BGBBTJ_BTIC1C_CTRL_SCALEHDR];
		cr=BGBBTJ_MulFloat16F(cr, i);
		cg=BGBBTJ_MulFloat16F(cg, i);
		cb=BGBBTJ_MulFloat16F(cb, i);
	}
	cr=bt1c_hf12tob[cr<<1];
	cg=bt1c_hf12tob[cg<<1];
	cb=bt1c_hf12tob[cb<<1];
	i=(cr<<16)|(cg<<8)|cb;
	return(i);
}

u64 BGBBTJ_BT1C_ConvRGB31FUToRGB48F(BGBBTJ_BTIC1C_ImageInfo *ctx, u32 clr)
{
	int cr, cg, cb;
	u64 i;
	cr=(clr>>20)&1023;
	cg=(clr>>10)&1023;
	cb=(clr    )&1023;
	cr=(cr<<1)|((clr>>30)&1);
	cg=(cg<<1)|((clr>>30)&1);
	cb=(cb<<1)|((clr>>30)&1);
	cr=(cr<<3)|(cr>>9);
	cg=(cg<<3)|(cg>>9);
	cb=(cb<<3)|(cb>>9);
	i=(((u64)cr)<<32)|(cg<<16)|cb;
	return(i);
}

u64 BGBBTJ_BT1C_ConvRGB31FSToRGB48F(BGBBTJ_BTIC1C_ImageInfo *ctx, u32 clr)
{
	int cr, cg, cb;
	u64 i;
	cr=(clr>>20)&1023;
	cg=(clr>>10)&1023;
	cb=(clr    )&1023;
	cr=(cr<<1)|((clr>>30)&1);
	cg=(cg<<1)|((clr>>30)&1);
	cb=(cb<<1)|((clr>>30)&1);
	cr=(cr<<4)|(cr>>8);
	cg=(cg<<4)|(cg>>8);
	cb=(cb<<4)|(cb>>8);
	i=(((u64)cr)<<32)|(cg<<16)|cb;
	return(i);
}

u64 BGBBTJ_BT1C_ConvRGB24ToRGB48F(BGBBTJ_BTIC1C_ImageInfo *ctx, u32 clr)
{
	int cr, cg, cb;
	u64 i;
	cr=(clr>>16)&1023;
	cg=(clr>> 8)&1023;
	cb=(clr    )&1023;
	cr=bt1c_btohf[cr];
	cg=bt1c_btohf[cg];
	cb=bt1c_btohf[cb];
	i=(((u64)cr)<<32)|(cg<<16)|cb;
	return(i);
}

u64 BGBBTJ_BT1C_ConvRGB15ToRGB48F(BGBBTJ_BTIC1C_ImageInfo *ctx, u16 clr)
{
	return(BGBBTJ_BT1C_ConvRGB24ToRGB48F(ctx,
		BGBBTJ_BT1C_ConvRGB15ToRGB24(ctx, clr)));
}

u64 BGBBTJ_BT1C_ConvRGB16ToRGB48F(BGBBTJ_BTIC1C_ImageInfo *ctx, u16 clr)
{
	return(BGBBTJ_BT1C_ConvRGB24ToRGB48F(ctx,
		BGBBTJ_BT1C_ConvRGB16ToRGB24(ctx, clr)));
}

u64 BGBBTJ_BT1C_ConvRGB23ToRGB48F(BGBBTJ_BTIC1C_ImageInfo *ctx, u32 clr)
{
	return(BGBBTJ_BT1C_ConvRGB24ToRGB48F(ctx,
		BGBBTJ_BT1C_ConvRGB23ToRGB24(ctx, clr)));
}

u32 BGBBTJ_BT1C_ConvRGB24ToRGB23(BGBBTJ_BTIC1C_ImageInfo *ctx, u32 clr)
{
	int i;
	i=	((clr&0x0000FE)>>1)|
		((clr&0x00FF00)>>1)|
		((clr&0xFE0000)>>2)|
		(((clr&1)&((clr>>16)&1))<<22);
	return(i);
}

int BGBBTJ_BT1C_ConvRGB24ToRGB16(BGBBTJ_BTIC1C_ImageInfo *ctx, u32 clr)
{
	int i;
	//rrrr-rrrr gggg-gggg bbbb-bbbb
	//rrrr-r000 gggg-gg00 bbbb-b000
	//          rrrr-rggg gggb-bbbb
	i=	((clr&0xF80000)>>8)|
		((clr&0x00FC00)>>5)|
		((clr&0x0000F8)>>3);
	return(i);
}

u32 BGBBTJ_BT1C_SwapRGB24_RB(u32 clr)
{
	u32 i;
	i=(clr&0xFF00FF00)|((clr>>16)&255)|((clr&255)<<16);
	return(i);
}

u16 BGBBTJ_BT1C_DecodeSingleColor555(
	BGBBTJ_BTIC1C_ImageInfo *ctx, byte **rcs, int mode)
{
	byte *cs;
	int i;

	cs=*rcs;
	switch(mode)
	{
	case 0:
	case 4:
	case 5:
	case 14:
		i=(cs[0]<<8)|cs[1];
		cs+=2;
		break;
	case 1:
	case 2:
	case 3:
		i=ctx->pal256[*cs++];
		break;
	case 6:
	case 10:
	case 11:
	case 12:
	case 13:
		i=(cs[0]<<16)|(cs[1]<<8)|cs[2];
		cs+=3;
		i=	((i&0x3E0000)>>7)|
			((i&0x007C00)>>5)|
			((i&0x00007C)>>2);
		break;
	case 9:
	case 15:
		i=ctx->pal16[*cs++];
		break;
	default:
		i=0; break;
	}
	
	*rcs=cs;
	return(i);
}

u16 BGBBTJ_BT1C_DecodeSingleColor565(
	BGBBTJ_BTIC1C_ImageInfo *ctx, byte **rcs, int mode)
{
	byte *cs;
	int i;

	cs=*rcs;
	switch(mode)
	{
	case 0:
	case 4:
	case 5:
	case 14:
		i=(cs[0]<<8)|cs[1];
		i=((i&0x7FE0)<<1)|(i&0x1F);
		i|=(i>>5)&0x20;
		cs+=2;
		break;
	case 1:
	case 2:
	case 3:
		i=ctx->pal256[*cs++];
//		i=(cs[0]<<8)|cs[1];
		i=((i&0x7FE0)<<1)|(i&0x1F);
		i|=(i>>5)&0x20;
		break;
	case 6:
	case 10:
	case 11:
	case 12:
	case 13:
		i=(cs[0]<<16)|(cs[1]<<8)|cs[2];
		cs+=3;
		i=	((i&0x3E0000)>>7)|
			((i&0x007C00)>>5)|
			((i&0x00007C)>>2);
		i=((i&0x7FE0)<<1)|(i&0x1F);
		i|=(i>>5)&0x20;
		break;
	case 9:
	case 15:
		i=ctx->pal16[*cs++];
//		i=(cs[0]<<8)|cs[1];
		i=((i&0x7FE0)<<1)|(i&0x1F);
		i|=(i>>5)&0x20;
		break;
	default:
		i=0;
		break;
	}
	
	*rcs=cs;
	return(i);
}


u64 BGBBTJ_BT1C_PredictDiffColorA48(BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *blks, byte *ct, int xs1, int ys1, int stride)
{
	return(BGBBTJ_BT1C_PredictDiffColorModeA48(ctx,
		blks, ct, xs1, ys1, stride, ctx->diff_m));
}

u64 BGBBTJ_BT1C_PredictDiffColorB48(BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *blks, byte *ct, int xs1, int ys1, int stride)
{
	return(BGBBTJ_BT1C_PredictDiffColorModeB48(ctx,
		blks, ct, xs1, ys1, stride, ctx->diff_m));
}

u64 BGBBTJ_BT1C_PredictDiffColorModeA48(BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *blks, byte *ct, int xs1, int ys1, int stride, int mode)
{
	byte *ct0, *ct1, *ct2;
	u64 p0, p1, p2, p3;

	switch(mode)
	{
	case 0:
		p3=0x000000;
		break;
	case 1:
	case 2:
	case 3:
		p3=0x000000;
		break;
	case 4:
		p3=ctx->diff_lclraw;
		break;
	default:
		p3=0; break;
	}

	return(p3);
}

u64 BGBBTJ_BT1C_PredictDiffColorModeB48(BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *blks, byte *ct, int xs1, int ys1, int stride, int mode)
{
	byte *ct0, *ct1, *ct2;
	u64 p0, p1, p2, p3;

	switch(mode)
	{
	case 0:
		p3=0x000000;
		break;
	case 1:
	case 2:
	case 3:
		p3=0x000000;
		break;
	case 4:
		p3=ctx->diff_lclrbw;
		break;
	default:
		p3=0; break;
	}

	return(p3);
}

u64 BGBBTJ_BT1C_AddDiff15ToClr48(BGBBTJ_BTIC1C_ImageInfo *ctx,
	int diff, u64 clr, int sc)
{
	u64 li;
	int cr, cg, cb, p;
	int dr, dg, db;
	
	cr=(clr&0xFFFF00000000)>>32;
	cg=(clr&0x0000FFFF0000)>>16;
	cb=(clr&0x00000000FFFF);

	dr=(diff>>10)&31;
	dg=(diff>> 5)&31;
	db=(diff    )&31;

	if(sc>0)
	{
		dr=bt1c_basedifftab5[sc][dr];
		dg=bt1c_basedifftab5[sc][dg];
		db=bt1c_basedifftab5[sc][db];
	}else
	{
		dr=dr-16;
		dg=dg-16;
		db=db-16;
	}

	cr=cr+dr;
	cg=cg+dg;
	cb=cb+db;

	cr=(u16)cr;
	cg=(u16)cg;
	cb=(u16)cb;

//	cr=(cr>255)?255:((cr<0)?0:cr);
//	cg=(cg>255)?255:((cg<0)?0:cg);
//	cb=(cb>255)?255:((cb<0)?0:cb);

//	cr=cr|(cr>>5);
//	cg=cg|(cg>>5);
//	cb=cb|(cb>>5);

	li=(((u64)cr)<<32)|(cg<<16)|cb;
	return(li);
}

u64 BGBBTJ_BT1C_ConvDiff15ToDiff48(BGBBTJ_BTIC1C_ImageInfo *ctx,
	int diff)
{
	int cr, cg, cb, p;
	u64 li;
	
	cr=(diff>>10)&31;
	cg=(diff>> 5)&31;
	cb=(diff    )&31;

//	cr=(diff&0x7C00)>>7;
//	cg=(diff&0x03E0)>>2;
//	cb=(diff&0x001F)<<3;

//	cr=cr|(cr>>5);
//	cg=cg|(cg>>5);
//	cb=cb|(cb>>5);

	li=(((u64)cr)<<32)|(cg<<16)|cb;
	return(li);
}

u64 BGBBTJ_BT1C_AddPred48ToDiff15(BGBBTJ_BTIC1C_ImageInfo *ctx,
	u64 clr, int diff)
{
	int i, cr, cg, cb, p;
	int dr, dg, db;
	u64 li;
	
	cr=(clr&0xFFFF00000000LL)>>32;
	cg=(clr&0x0000FFFF0000LL)>>16;
	cb=(clr&0x00000000FFFFLL);

	dr=(diff&0x7C00)>>10;
	dg=(diff&0x03E0)>> 5;
	db=(diff&0x001F);

	if(ctx->diff_sc)
	{
		dr=bt1c_basedifftab5[ctx->diff_sc][dr];
		dg=bt1c_basedifftab5[ctx->diff_sc][dg];
		db=bt1c_basedifftab5[ctx->diff_sc][db];
	}else
	{
		dr=dr-16;
		dg=dg-16;
		db=db-16;
	}

	cr=cr+dr;
	cg=cg+dg;
	cb=cb+db;

//	cr=(cr>255)?255:((cr<0)?0:cr);
//	cg=(cg>255)?255:((cg<0)?0:cg);
//	cb=(cb>255)?255:((cb<0)?0:cb);

	cr=cr&65535;
	cg=cg&65535;
	cb=cb&65535;

//	cr=cr|(cr>>5);
//	cg=cg|(cg>>5);
//	cb=cb|(cb>>5);

	li=(((u64)cr)<<32)|(cg<<16)|cb;
	return(li);
}

u64 BGBBTJ_BT1C_CalcDiff15ColorA48(BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *blks, byte *ct, int xs1, int ys1, int stride, int diff)
{
	u64 p0, p1;
	p0=BGBBTJ_BT1C_PredictDiffColorA48(ctx, blks, ct, xs1, ys1, stride);
//	p1=BGBBTJ_BT1C_AddPred24ToDiff15(ctx, p0, diff);
	p1=BGBBTJ_BT1C_AddDiff15ToClr48(ctx, diff, p0, ctx->diff_sc);
	return(p1);
}

u64 BGBBTJ_BT1C_CalcDiff15ColorB48(BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *blks, byte *ct, int xs1, int ys1, int stride, int diff)
{
	u64 p0, p1;
	p0=BGBBTJ_BT1C_PredictDiffColorB48(ctx, blks, ct, xs1, ys1, stride);
//	p1=BGBBTJ_BT1C_AddPred24ToDiff15(ctx, p0, diff);
	p1=BGBBTJ_BT1C_AddDiff15ToClr48(ctx, diff, p0, ctx->diff_sc);
	return(p1);
}
