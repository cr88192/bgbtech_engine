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

extern s16 bt1c_difftaby8[16][128];	//diff7 on RGB24

extern int bt1c_basedifftab5[16][32];

extern byte bt1c_hf12tob[4096];		//half-float (bits 4-15) to byte
extern u16 bt1c_btohf[256];			//byte to half-float

extern int bt1c_basedifftab[8][5];
extern u32 bt1c_blkidx_pixpat[256];	//1BPP -> RPZA Pixels

int BGBBTJ_BT1C_ResetImageDefaults(BGBBTJ_BTIC1C_ImageInfo *ctx)
{
	int i, j, k, l;

	for(j=0; j<256; j++)
	{
		k=bt1c_blkidxcolor[j];
		k=BGBBTJ_BT1C_ConvRGB15ToRGB24(ctx, k);
		ctx->pal256[j]=k;
		l=BGBBTJ_BT1C_ConvRGB24ToRGB48F(ctx, k);
		ctx->pal256w[j]=l;
		l=BGBBTJ_BT1C_ConvRGB24ToRGB16(ctx, k);
		ctx->pal256s[j]=l;
//		ctx->pal256[j]=bt1c_blkidxcolor[j];
	}
	for(j=0; j<16; j++)
	{
		ctx->pal16[j]=ctx->pal256[j];
		ctx->pal16w[j]=ctx->pal256w[j];
		ctx->pal16s[j]=ctx->pal256s[j];
	}
	for(j=0; j<256; j++)
		{ ctx->pat256[j]=bt1c_blkidx_pixpat[j]; }
	for(j=0; j<8; j++)
		{ ctx->modes[j]=j; }
	for(j=0; j<8; j++)
		{ ctx->amodes[j]=j; }
	ctx->optcntrl[0]=0;
	ctx->optcntrl[1]=0;
	ctx->optcntrl[2]=0;
	ctx->optcntrl[3]=0x3C00;
	ctx->optcntrl[4]=0xFFFFFF;
	ctx->optcntrl[5]=0xFFFFFF;
	
	return(0);
}

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

	cr=(cr+dr)&255;
	cg=(cg+dg)&255;
	cb=(cb+db)&255;

//	cr=cr+dr;
//	cg=cg+dg;
//	cb=cb+db;

//	cr=(cr>255)?255:((cr<0)?0:cr);
//	cg=(cg>255)?255:((cg<0)?0:cg);
//	cb=(cb>255)?255:((cb<0)?0:cb);

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

	cr=(cr+dr)&255;
	cg=(cg+dg)&255;
	cb=(cb+db)&255;

//	cr=cr+dr;
//	cg=cg+dg;
//	cb=cb+db;

//	cr=(cr>255)?255:((cr<0)?0:cr);
//	cg=(cg>255)?255:((cg<0)?0:cg);
//	cb=(cb>255)?255:((cb<0)?0:cb);

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

u16 BGBBTJ_BT1C_ConvRGB16ToRGB15(BGBBTJ_BTIC1C_ImageInfo *ctx, u16 clr)
{
	int i, cr, cg, cb, p;

	cr=(clr&0xF800)>>8;
	cg=(clr&0x07E0)>>3;
	cb=(clr&0x001F)<<3;

//	cr=cr|(cr>>5);
//	cg=cg|(cg>>6);
//	cb=cb|(cb>>5);

	i=	((cr<<7)&0x7C00)|
		((cg<<2)&0x03E0)|
		((cb>>3)&0x001F);
	return(i);
}

u16 BGBBTJ_BT1C_ConvRGB15ToRGB16(BGBBTJ_BTIC1C_ImageInfo *ctx, u16 clr)
{
	int i, cr, cg, cb, p;

	cr=(clr&0x7C00)>>7;
	cg=(clr&0x03E0)>>2;
	cb=(clr&0x001F)<<3;

//	cr=cr|(cr>>5);
//	cg=cg|(cg>>6);
//	cb=cb|(cb>>5);

	i=	((cr<<8)&0xF800)|
		((cg<<3)&0x07E0)|
		((cb>>3)&0x001F);
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
		i=  ((clr&0x3F8000)<<2)|
			((clr&0x007FFF)<<1);
//		i|=((((s32)clr)<<1)>>31)&0x010001;
		p=(clr>>22)&1;
		i|=p|(p<<16);

//		cr=(clr&0x3F8000)>>14;
//		cg=(clr&0x007F80)>>7;
//		cb=(clr&0x00007F)<<1;
//		p=(clr>>22)&1;
//		cr|=p; cb|=p;
//		i=(cr<<16)|(cg<<8)|cb;
	}
	return(i);
}

u32 BGBBTJ_BT1C_ConvYYY7ToRGB24(BGBBTJ_BTIC1C_ImageInfo *ctx, int clr)
{
	int cv, cr, cg, cb;
	int i, j;

//	if(ctx->dflags&BGBBTJ_BTIC1C_DFL_YYYVECTOR)
	if(ctx->optcntrl[4]!=0xFFFFFF)
	{
		j=(clr<<1)&255;
		j=j|(j>>7);

		cv=ctx->optcntrl[4];
		cr=(cv>>16)&255;
		cg=(cv>> 8)&255;
		cb=(cv    )&255;
		cr=(cr*j+255)>>8;
		cg=(cg*j+255)>>8;
		cb=(cb*j+255)>>8;

		i=(cr<<16)|(cg<<8)|cb;
	}else
	{
		j=(clr<<1)&255;
		j=j|(j>>7);
		i=(j<<16)|(j<<8)|j;
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
		cr=BGBBTJ_MulFloat16F(cr, i<<4)>>4;
		cg=BGBBTJ_MulFloat16F(cg, i<<4)>>4;
		cb=BGBBTJ_MulFloat16F(cb, i<<4)>>4;
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
		cr=BGBBTJ_MulFloat16F(cr, i<<5)>>5;
		cg=BGBBTJ_MulFloat16F(cg, i<<5)>>5;
		cb=BGBBTJ_MulFloat16F(cb, i<<5)>>5;
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

u32 BGBBTJ_BT1C_ConvRGB48FToRGB24(BGBBTJ_BTIC1C_ImageInfo *ctx, u64 clr)
{
	int cr, cg, cb;
	int i;

	cr=(clr>>32)&65535;
	cg=(clr>>16)&65535;
	cb=(clr    )&65535;
	if(ctx && (ctx->dflags&BGBBTJ_BTIC1C_DFL_SCALEHDR2LDR))
	{
		i=ctx->optcntrl[BGBBTJ_BTIC1C_CTRL_SCALEHDR];
		cr=BGBBTJ_MulFloat16F(cr, i);
		cg=BGBBTJ_MulFloat16F(cg, i);
		cb=BGBBTJ_MulFloat16F(cb, i);
	}
	cr=bt1c_hf12tob[cr>>4];
	cg=bt1c_hf12tob[cg>>4];
	cb=bt1c_hf12tob[cb>>4];
	i=(cr<<16)|(cg<<8)|cb;
	return(i);
}

u32 BGBBTJ_BT1C_ConvRGB48FToRGB16(BGBBTJ_BTIC1C_ImageInfo *ctx, u64 clr)
{
	return(BGBBTJ_BT1C_ConvRGB24ToRGB16(ctx,
		BGBBTJ_BT1C_ConvRGB48FToRGB24(ctx, clr)));
}

int BGBBTJ_BT1C_AddPred24ToDiff7Y(BGBBTJ_BTIC1C_ImageInfo *ctx,
	int clr, int diff)
{
	int i, cr, cg, cb, p;
	int dr, dg, db;
	int dvr, dvg, dvb;
	
//	cr=(clr&0xFF0000)>>16;
//	cg=(clr&0x00FF00)>>8;
//	cb=(clr&0x0000FF);
	cr=(clr>>16)&255;
	cg=(clr>> 8)&255;
	cb=(clr    )&255;

	if(ctx->diff_sc)
	{
		i=diff&127;
		dr=bt1c_difftaby8[ctx->diff_sc][i];
		dg=bt1c_difftaby8[ctx->diff_sc][i];
		db=bt1c_difftaby8[ctx->diff_sc][i];
	}else
	{
		dg=diff-16;
		dr=dg; db=dg;
	}

//	if(ctx->dflags&BGBBTJ_BTIC1C_DFL_YYYVECTOR)
	if(ctx->optcntrl[5]!=0xFFFFFF)
	{
		i=ctx->optcntrl[5];
		
		dvr=(i>>16)&255;
		dvg=(i>> 8)&255;
		dvb=(i    )&255;

		dr=(dvr*dr+255)>>8;
		dg=(dvg*dg+255)>>8;
		db=(dvb*db+255)>>8;
	}

	cr=(cr+dr)&255;
	cg=(cg+dg)&255;
	cb=(cb+db)&255;

	i=(cr<<16)|(cg<<8)|cb;
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
	BGBBTJ_BTIC1C_ImageInfo *ctx, byte **rcs, int mode, int flag)
{
	byte *cs;
	int i, j, k;

	cs=*rcs;
	switch(mode)
	{
	case 0:
	case 4:
	case 5:
	case 14:
		if((cs[0]&0x80) && ctx->diff_m && (flag&BGBBTJ_BTIC1C_EFL_DIFFCLR))
		{
			j=*cs++;
			j=bt1c_difftab[(ctx->diff_sc*256)+(j&127)];
			k=ctx->diff_lclra;
			i=k+j;
			ctx->diff_lclra=i;
			ctx->diff_lclrb=i;
			break;
		}
		i=(cs[0]<<8)|cs[1];
		i=((i&0x7FE0)<<1)|(i&0x1F);
		i|=(i>>5)&0x20;
		cs+=2;
		break;
	case 1:
	case 2:
	case 3:
		i=ctx->pal256[*cs++];
		ctx->diff_lclra=i;
		ctx->diff_lclrb=i;
//		i=(cs[0]<<8)|cs[1];
		i=((i&0x7FE0)<<1)|(i&0x1F);
		i|=(i>>5)&0x20;
		break;
	case 6:
	case 10:
	case 11:
	case 12:
//	case 13:
		if((cs[0]&0x80) && ctx->diff_m && (flag&BGBBTJ_BTIC1C_EFL_DIFFCLR))
		{
			j=(cs[0]<<8)|cs[1];
			cs+=2;
			k=ctx->diff_lclra;
			i=BGBBTJ_BT1C_AddDiff15ToClr24(ctx, j, k, ctx->diff_sc);
			ctx->diff_lclra=i;
			ctx->diff_lclrb=i;
			i=	((i&0xF80000)>>8)|
				((i&0x00FC00)>>5)|
				((i&0x0000F8)>>3);
			break;
		}
		i=(cs[0]<<16)|(cs[1]<<8)|cs[2];
		cs+=3;
		i=  ((i&0x3F8000)<<2)|
			((i&0x007FFF)<<1);
		ctx->diff_lclra=i;
		ctx->diff_lclrb=i;
		i=	((i&0xF80000)>>8)|
			((i&0x00FC00)>>5)|
			((i&0x0000F8)>>3);
		break;
	case 13:
		if((cs[0]&0x80) && ctx->diff_m && (flag&BGBBTJ_BTIC1C_EFL_DIFFCLR))
		{
			j=*cs++;
			j=bt1c_difftab24[(ctx->diff_sc*256)+(j&127)];
			k=ctx->diff_lclra;
			i=bgbbtj_bt1c_addrgb24(k, j);
			ctx->diff_lclra=i;
			ctx->diff_lclrb=i;
			i=	((i&0xF80000)>>8)|
				((i&0x00FC00)>>5)|
				((i&0x0000F8)>>3);
			break;
		}
		i=(cs[0]<<16)|(cs[1]<<8)|cs[2];
		cs+=3;
		i=  ((i&0x3F8000)<<2)|
			((i&0x007FFF)<<1);
		ctx->diff_lclra=i;
		ctx->diff_lclrb=i;
		i=	((i&0xF80000)>>8)|
			((i&0x00FC00)>>5)|
			((i&0x0000F8)>>3);
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

int BGBBTJ_BT1C_DecodeBlocksCommandBlockCopy8N(
	BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte **rcs, byte **rct,
	byte **rcsl, int cmd, int stride)
{
	byte *cs, *ct, *csl;
	int ret, xo, yo, yshr;
	int i, j, k, l;

	ret=0;
	cs=*rcs; ct=*rct;
	csl=*rcsl;
	switch(cmd)
	{
#if 1
	case 0xED:
		j=*cs++;
		switch((j>>5)&7)
		{
		case 0:
			j=(j&31)+1;
			k=((cs[0]<<8)|(cs[1]))+1; cs+=2;
			break;
		case 1:
			j=(j&31)+1;
			k=(*cs++)+1;
			break;
		case 2:
			k=(((j&31)<<8)|(*cs++))+1;
			j=1;
			break;
		case 3:
			k=(j&31)+1;
			j=1;
			break;
		case 4:
			j=(((j&31)<<8)|(*cs++))+1;
			k=1;
			break;
		case 5:
			j=(j&31)+1;
			k=1;
			break;					
		default:
			j=0; k=0;
			ret=-1;
			break;
		}

//		if((ct+j*stride)>cte) { ret=-1; break; }
//		if((ct-k*stride)<blks) { ret=-1; break; }

		while(j--)
		{
			bgbbtj_bt1c_memcpy8(ct, ct-k*stride);
			ct+=stride; csl+=stride;
		}
		break;
#endif

	case 0xEE:
		j=(cs[0]<<8)+cs[1];
		cs+=2;

		switch((j>>13)&7)
		{
		case 0:
			k=(j&8191)+1;
			xo=0; yo=0; yshr=0;
			break;
		case 1:
			k=((j>>8)&31)+1;
			yo=((j>>4)&15)-8;
			xo=((j   )&15)-8;
			yshr=0;
			break;
		case 2:
			k=(j&8191)+1;
			yo=cs[0]-128;
			xo=cs[1]-128;
			yshr=0;
			cs+=2;
			break;
		case 3:
			j=(j<<8)|(*cs++);
			k=   ((j>>16)&31)+1;
			yshr=((j>>12)&15);
			yo=  ((j>> 6)&63)-32;
			xo=  ((j    )&63)-32;
			break;
		default:
			xo=0; yo=0; k=0;
			break;
		}

//		if((ct+j*stride)>cte) { ret=-1; break; }
//		if((ct-k*stride)<blks) { ret=-1; break; }
//		l=yo*xs1+xo;
//		l=yo*ctx->width2+xo;

		if(yshr)
		{
			l=ctx->width2>>yshr;
			if(!l)l=1;
			l=yo*l+xo;
		}else
		{
			l=yo*ctx->width2+xo;
		}

		while(k--)
		{
			bgbbtj_bt1c_memcpy8(ct, csl+l*stride);
			ct+=stride; csl+=stride;
		}
		break;

	default:
		ret=-1;
		break;
	}
	*rcs=cs;	*rct=ct;
	*rcsl=csl;
	return(ret);
}

int BGBBTJ_BT1C_DecodeBlocksCommandBlockCopy16(
	BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte **rcs, byte **rct,
	byte **rcsl, int cmd)
{
	byte *cs, *ct, *csl;
	int ret, xo, yo, yshr;
	int i, j, k, l;

	ret=0;
	cs=*rcs; ct=*rct;
	csl=*rcsl;
	switch(cmd)
	{
#if 1
	case 0xED:
		j=*cs++;
		switch((j>>5)&7)
		{
		case 0:
			j=(j&31)+1;
			k=((cs[0]<<8)|(cs[1]))+1; cs+=2;
			break;
		case 1:
			j=(j&31)+1;
			k=(*cs++)+1;
			break;
		case 2:
			k=(((j&31)<<8)|(*cs++))+1;
			j=1;
			break;
		case 3:
			k=(j&31)+1;
			j=1;
			break;
		case 4:
			j=(((j&31)<<8)|(*cs++))+1;
			k=1;
			break;
		case 5:
			j=(j&31)+1;
			k=1;
			break;					
		default:
			j=0; k=0;
			ret=-1;
			break;
		}

//		if((ct+j*16)>cte) { ret=-1; break; }
//		if((ct-k*16)<blks) { ret=-1; break; }

		while(j--)
		{
			bgbbtj_bt1c_memcpy16(ct, ct-k*16);
			ct+=16; csl+=16;
		}
		break;
#endif

	case 0xEE:
		j=(cs[0]<<8)+cs[1];
		cs+=2;

		switch((j>>13)&7)
		{
		case 0:
			k=(j&8191)+1;
			xo=0; yo=0;
			yshr=0;
			break;
		case 1:
			k=((j>>8)&31)+1;
			yo=((j>>4)&15)-8;
			xo=((j   )&15)-8;
			yshr=0;
			break;
		case 2:
			k=(j&8191)+1;
			yo=cs[0]-128;
			xo=cs[1]-128;
			yshr=0;
			cs+=2;
			break;
		case 3:
			j=(j<<8)|(*cs++);
			k=   ((j>>16)&31)+1;
			yshr=((j>>12)&15);
			yo=  ((j>> 6)&63)-32;
			xo=  ((j    )&63)-32;
			break;
		default:
			xo=0; yo=0; k=0;
			yshr=0;
			break;
		}

//		if((ct+j*16)>cte) { ret=-1; break; }
//		if((ct-k*16)<blks) { ret=-1; break; }
//		l=yo*xs1+xo;
//		l=yo*ctx->width2+xo;

		if(yshr)
		{
			l=ctx->width2>>yshr;
			if(!l)l=1;
			l=yo*l+xo;
		}else
		{
			l=yo*ctx->width2+xo;
		}

		while(k--)
		{
			bgbbtj_bt1c_memcpy16(ct, csl+l*16);
			ct+=16; csl+=16;
		}
		break;

	default:
		ret=-1;
		break;
	}
	*rcs=cs;	*rct=ct;
	*rcsl=csl;
	return(ret);
}

int BGBBTJ_BT1C_DecodeBlocksCommandBlockCopy16A8(
	BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte **rcs, byte **rct,
	byte **rcsl, byte **rcsa,
	int cmd)
{
	byte *cs, *ct, *csl, *csa;
	int ret, xo, yo, yshr;
	int i, j, k, l;

	ret=0;
	cs=*rcs; ct=*rct;
	csl=*rcsl; csa=*rcsa;
	switch(cmd)
	{
#if 1
	case 0xED:
		j=*cs++;
		switch((j>>5)&7)
		{
		case 0:
			j=(j&31)+1;
			k=((cs[0]<<8)|(cs[1]))+1; cs+=2;
			break;
		case 1:
			j=(j&31)+1;
			k=(*cs++)+1;
			break;
		case 2:
			k=(((j&31)<<8)|(*cs++))+1;
			j=1;
			break;
		case 3:
			k=(j&31)+1;
			j=1;
			break;
		case 4:
			j=(((j&31)<<8)|(*cs++))+1;
			k=1;
			break;
		case 5:
			j=(j&31)+1;
			k=1;
			break;					
		default:
			j=0; k=0;
			ret=-1;
			break;
		}

//		if((ct+j*16)>cte) { ret=-1; break; }
//		if((ct-k*16)<blks) { ret=-1; break; }

		while(j--)
		{
			bgbbtj_bt1c_memcpy16(ct, ct-k*16);
			ct+=16; csl+=16; csa+=8;
		}
		break;
#endif

	case 0xEE:
		j=(cs[0]<<8)+cs[1];
		cs+=2;

		switch((j>>13)&7)
		{
		case 0:
			k=(j&8191)+1;
			xo=0; yo=0;
			yshr=0;
			break;
		case 1:
			k=((j>>8)&31)+1;
			yo=((j>>4)&15)-8;
			xo=((j   )&15)-8;
			yshr=0;
			break;
		case 2:
			k=(j&8191)+1;
			yo=cs[0]-128;
			xo=cs[1]-128;
			yshr=0;
			cs+=2;
			break;
		case 3:
			j=(j<<8)|(*cs++);
			k=   ((j>>16)&31)+1;
			yshr=((j>>12)&15);
			yo=  ((j>> 6)&63)-32;
			xo=  ((j    )&63)-32;
			break;
		default:
			xo=0; yo=0; k=0;
			yshr=0;
			break;
		}

//		if((ct+j*16)>cte) { ret=-1; break; }
//		if((ct-k*16)<blks) { ret=-1; break; }
//		l=yo*xs1+xo;
//		l=yo*ctx->width2+xo;

		if(yshr)
		{
			l=ctx->width2>>yshr;
			if(!l)l=1;
			l=yo*l+xo;
		}else
		{
			l=yo*ctx->width2+xo;
		}

		while(k--)
		{
			bgbbtj_bt1c_memcpy16(ct, csl+l*16);
			ct+=16; csl+=16; csa+=8;
		}
		break;

	default:
		ret=-1;
		break;
	}
	*rcs=cs;	*rct=ct;
	*rcsl=csl;	*rcsa=csa;
	return(ret);
}

int BGBBTJ_BT1C_DecodeBlocksCommandBlockCopy32A8(
	BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte **rcs, byte **rct,
	byte **rcsl, byte **rcsa,
	int cmd)
{
	byte *cs, *ct, *csl, *csa;
	int ret, xo, yo, yshr;
	int i, j, k, l;

	ret=0;
	cs=*rcs; ct=*rct;
	csl=*rcsl; csa=*rcsa;
	switch(cmd)
	{
#if 1
	case 0xED:
		j=*cs++;
		switch((j>>5)&7)
		{
		case 0:
			j=(j&31)+1;
			k=((cs[0]<<8)|(cs[1]))+1; cs+=2;
			break;
		case 1:
			j=(j&31)+1;
			k=(*cs++)+1;
			break;
		case 2:
			k=(((j&31)<<8)|(*cs++))+1;
			j=1;
			break;
		case 3:
			k=(j&31)+1;
			j=1;
			break;
		case 4:
			j=(((j&31)<<8)|(*cs++))+1;
			k=1;
			break;
		case 5:
			j=(j&31)+1;
			k=1;
			break;					
		default:
			j=0; k=0;
			ret=-1;
			break;
		}

//		if((ct+j*32)>cte) { ret=-1; break; }
//		if((ct-k*32)<blks) { ret=-1; break; }

		while(j--)
		{
			bgbbtj_bt1c_memcpy32(ct, ct-k*32);
			ct+=32; csl+=32; csa+=8;
		}
		break;
#endif

	case 0xEE:
		j=(cs[0]<<8)+cs[1];
		cs+=2;

		switch((j>>13)&7)
		{
		case 0:
			k=(j&8191)+1;
			xo=0; yo=0;
			yshr=0;
			break;
		case 1:
			k=((j>>8)&31)+1;
			yo=((j>>4)&15)-8;
			xo=((j   )&15)-8;
			yshr=0;
			break;
		case 2:
			k=(j&8191)+1;
			yo=cs[0]-128;
			xo=cs[1]-128;
			yshr=0;
			cs+=2;
			break;
		case 3:
			j=(j<<8)|(*cs++);
			k=   ((j>>16)&31)+1;
			yshr=((j>>12)&15);
			yo=  ((j>> 6)&63)-32;
			xo=  ((j    )&63)-32;
			break;
		default:
			xo=0; yo=0; k=0;
			yshr=0;
			break;
		}

//		if((ct+j*32)>cte) { ret=-1; break; }
//		if((ct-k*32)<blks) { ret=-1; break; }
//		l=yo*xs1+xo;
//		l=yo*ctx->width2+xo;

		if(yshr)
		{
			l=ctx->width2>>yshr;
			if(!l)l=1;
			l=yo*l+xo;
		}else
		{
			l=yo*ctx->width2+xo;
		}

		while(k--)
		{
			bgbbtj_bt1c_memcpy32(ct, csl+l*32);
			ct+=32; csl+=32; csa+=8;
		}
		break;

	default:
		ret=-1;
		break;
	}
	*rcs=cs;	*rct=ct;
	*rcsl=csl;	*rcsa=csa;
	return(ret);
}

int BGBBTJ_BT1C_DecodeBlocksCommandBasic(
	BGBBTJ_BTIC1C_ImageInfo *ctx, byte **rcs, int cmd)
{
	byte *cs;
	int ret;
	int i, j, k, l;

	ret=0;
	cs=*rcs;
	switch(cmd)
	{
	case 0xE0:
		if(ctx->curmode!=ctx->modes[0])
		{
			ctx->curmode=ctx->modes[0];
			ctx->nextmode=ctx->curmode;
		}
		else
		{
			ret=1;
		}
		break;
	case 0xE1:
		ctx->curmode=ctx->modes[1];
		ctx->nextmode=ctx->curmode;
		break;
	case 0xE2:
		ctx->curmode=ctx->modes[2];
		ctx->nextmode=ctx->curmode;
		break;
	case 0xE3:
	case 0xE4:
		if((cs[3]>='A') && (cs[3]<='Z'))
			{ ret=-1; }
		j=(cs[0]<<16)|(cs[1]<<8)|cs[2];
		cs+=j-1;
		break;
	case 0xE5:
		if((cs[1]>='A') && (cs[1]<='Z'))
			{ ret=-1; }
		j=cs[0];
		cs+=j-1;
		break;

	case 0xEC:
		j=*cs++;
		switch((j>>5)&7)
		{
		case 0:
			k=j&31;
			if(k<24)
			{
				ctx->curmode=k;
				ctx->nextmode=ctx->curmode;
				break;
			}
			if(k<28)
			{
				k=((k&3)<<8)|(*cs++);
				ctx->curmode=k;
				ctx->nextmode=ctx->curmode;
				break;
			}
			k=(cs[0]<<8)|(cs[1]); cs+=2;
			ctx->curmode=k;
			ctx->nextmode=ctx->curmode;
			break;

		case 1:
			l=*cs++;
			switch((j>>3)&3)
			{
			case 0:
				ctx->modes[j&7]=l;
				break;
			case 1:
				ctx->modes[j&7]=l;
				ctx->nextmode=ctx->curmode;
				ctx->curmode=l;
				break;
			case 2:
				ctx->modes[j&7]=l;
				ctx->curmode=l;
				ctx->nextmode=ctx->curmode;
				break;
			default:
				ret=-1;
				break;
			}
			break;

		case 2:
			k=((j&31)<<8)|(*cs++);
			l=*cs++;
			switch((k>>10)&7)
			{
			case 0: break;
			case 1: l=(s8)l; break;
			default: break;
			}
			ctx->optcntrl[k&1023]=l;
			break;
		case 3:
			k=((j&31)<<8)|(*cs++);
			l=(cs[0]<<8)|(cs[1]); cs+=2;
			switch((k>>10)&7)
			{
			case 0: break;
			case 1: l=(s16)l; break;
			default: break;
			}
			ctx->optcntrl[k&1023]=l;
			break;
		case 4:
			k=((j&31)<<8)|(*cs++);
			l=(cs[0]<<8)|(cs[1]<<8)|(cs[2]); cs+=3;
			switch((k>>10)&7)
			{
			case 0: break;
			case 1: l=(((s32)l)<<8)>>8; break;
			default: break;
			}
			ctx->optcntrl[k&1023]=l;
			break;
		case 5:
			k=((j&31)<<8)|(*cs++);
			l=(cs[0]<<24)|(cs[1]<<16)|(cs[2]<<8)|(cs[3]); cs+=4;
			switch((k>>10)&7)
			{
			case 0: break;
			case 1: l=(s32)l; break;
			default: break;
			}
			ctx->optcntrl[k&1023]=l;
			break;
			
		default:
			ret=-1;
			break;
		}
		break;

	case 0xEF:
		j=*cs++;
		switch((j>>5)&7)
		{
		case 0:
			k=j&31;
			ctx->optfl[k>>3]|=(1<<(k&7));
			if(k==BGBBTJ_BTIC1C_CTRFL_RGB23B)
				ctx->dflags|=BGBBTJ_BTIC1C_DFL_RGB23B;
			break;
		case 1:
			k=j&31;
			ctx->optfl[k>>3]&=~(1<<(k&7));
			if(k==BGBBTJ_BTIC1C_CTRFL_RGB23B)
				ctx->dflags&=~BGBBTJ_BTIC1C_DFL_RGB23B;
			break;
		case 2:
			k=((j&31)<<8)|(*cs++);
			ctx->optfl[k>>3]|=(1<<(k&7));
			if(k==BGBBTJ_BTIC1C_CTRFL_RGB23B)
				ctx->dflags|=BGBBTJ_BTIC1C_DFL_RGB23B;
			break;
		case 3:
			k=((j&31)<<8)|(*cs++);
			ctx->optfl[k>>3]&=~(1<<(k&7));
			if(k==BGBBTJ_BTIC1C_CTRFL_RGB23B)
				ctx->dflags&=~BGBBTJ_BTIC1C_DFL_RGB23B;
			break;
		case 4:
			k=j&31;
			l=*cs++;
			ctx->optcntrl[k]=l;
			if(k==BGBBTJ_BTIC1C_CTRL_BLOCKMODE)
				{ ctx->nextmode=l; ctx->curmode=l; }
			break;
		case 5:
			k=j&31;
			l=(cs[0]<<8)|(cs[1]); cs+=2;
			ctx->optcntrl[k]=l;
			if(k==BGBBTJ_BTIC1C_CTRL_BLOCKMODE)
				{ ctx->nextmode=l; ctx->curmode=l; }
			if(k==BGBBTJ_BTIC1C_CTRL_SCALEHDR)
				ctx->dflags|=BGBBTJ_BTIC1C_DFL_SCALEHDR2LDR;
			break;
		case 6:
			k=j&31;
			if(k<24)
			{
				ctx->nextmode=ctx->curmode;
				ctx->curmode=k;
				break;
			}
			if(k<28)
			{
				k=((k&3)<<8)|(*cs++);
				ctx->nextmode=ctx->curmode;
				ctx->curmode=k;
				break;
			}
			k=(cs[0]<<8)|(cs[1]); cs+=2;
			ctx->nextmode=ctx->curmode;
			ctx->curmode=k;
			break;
			
		default:
			ret=-1;
			break;
		}
		break;

	case 0xF0:
		i=cs[0];
		switch(cs[0])
		{
		case 0xF0: case 0xF1:
		case 0xF2: case 0xF3:
		case 0xF4: case 0xF5:
		case 0xF6: case 0xF7:
			cs++;
			ctx->curmode=ctx->modes[i-0xF0];
			ctx->nextmode=ctx->curmode;
			break;
		case 0xEF:
			if((cs[1]>=0xC0) && (cs[1]<0xD8))
			{
				k=cs[1]&31; cs+=2;
				ctx->nextmode=k;
				ctx->curmode=ctx->curmode;
				break;
			}
			if((cs[1]>=0xD8) && (cs[1]<0xDC))
			{
				k=((cs[1]&3)<<8)|cs[2]; cs+=3;
				ctx->nextmode=k;
				ctx->curmode=ctx->curmode;
				break;
			}
			ctx->nextmode=ctx->curmode;
			ctx->curmode=ctx->modes[0];
			break;
		default:
			ctx->nextmode=ctx->curmode;
			ctx->curmode=ctx->modes[0];
			break;
		}
		break;
	case 0xF1: case 0xF2:
	case 0xF3: case 0xF4:
	case 0xF5: case 0xF6:
	case 0xF7:
		ctx->nextmode=ctx->curmode;
		ctx->curmode=ctx->modes[cmd-0xF0];
		break;

	default:
		ret=-1;
		break;
	}
	*rcs=cs;
	return(ret);
}

int  BGBBTJ_BT1C_DecodeBlocksCommandExt(
	BGBBTJ_BTIC1C_ImageInfo *ctx, byte **rcs, int cmd)
{
	byte *cs;
	int ret;
	int i, j, k, l;

	ret=0;
	cs=*rcs;
	switch(cmd)
	{
	case 0x81:
		BGBBTJ_BT1C_ResetImageDefaults(ctx);
		break;
	case 0x82:
		j=*cs++;
		k=(*cs++)+1;
		for(; j<k; j++)
		{
			l=(cs[0]<<8)|cs[1];
			cs+=2;
			l=BGBBTJ_BT1C_ConvRGB15ToRGB24(ctx, l);
			ctx->pal256[j]=l;
			ctx->pal256w[j]=
				BGBBTJ_BT1C_ConvRGB24ToRGB48F(ctx, l);
			ctx->pal256s[j]=
				BGBBTJ_BT1C_ConvRGB24ToRGB16(ctx, l);
		}
		break;
	case 0x83:
		j=(cs[0]>>4)&15;
		k=(cs[0]&15)+1;
		for(; j<k; j++)
		{
			l=*cs++;
			ctx->pal16[j]=ctx->pal256[l];
			ctx->pal16w[j]=ctx->pal256w[l];
			ctx->pal16s[j]=ctx->pal256s[l];
		}
		break;
	case 0x84:
		j=(cs[0]>>4)&15;
		k=(cs[0]&15)+1;
		for(; j<k; j++)
		{
			l=(cs[0]<<8)|cs[1]; cs+=2;
			l=BGBBTJ_BT1C_ConvRGB15ToRGB24(ctx, l);
			ctx->pal16[j]=l;
			ctx->pal16w[j]=
				BGBBTJ_BT1C_ConvRGB24ToRGB48F(ctx, l);
			ctx->pal16s[j]=
				BGBBTJ_BT1C_ConvRGB24ToRGB16(ctx, l);
		}
		break;
	case 0x85:
		j=*cs++;
		k=(*cs++)+1;
		for(; j<k; j++)
		{
			l=(cs[0]<<24)|(cs[1]<<16)|(cs[2]<<8)|cs[3];
			cs+=4;
			ctx->pat256[j]=l;
		}
		break;
	case 0x86:
		j=*cs++;
		k=(*cs++)+1;
		for(; j<k; j++)
		{
			l=(cs[0]<<16)|(cs[1]<<8)|cs[2];
			cs+=3;
			l=BGBBTJ_BT1C_ConvRGB23ToRGB24(ctx, l);
			ctx->pal256[j]=l;
			ctx->pal256w[j]=
				BGBBTJ_BT1C_ConvRGB24ToRGB48F(ctx, l);
			ctx->pal256s[j]=
				BGBBTJ_BT1C_ConvRGB24ToRGB16(ctx, l);
		}
		break;
	case 0x87:
		j=*cs++;
		k=(*cs++)+1;
		for(; j<k; j++)
		{
			l=(cs[0]<<24)|(cs[1]<<16)|(cs[2]<<8)|cs[3];
			cs+=4;
			l=(l>>8);
			ctx->pal256[j]=l;
			ctx->pal256w[j]=
				BGBBTJ_BT1C_ConvRGB24ToRGB48F(ctx, l);
			ctx->pal256s[j]=
				BGBBTJ_BT1C_ConvRGB24ToRGB16(ctx, l);
		}
		break;
	case 0x88:
		j=*cs++;
		k=(*cs++)+1;
		for(; j<k; j++)
		{
			l=(cs[0]<<24)|(cs[1]<<16)|(cs[2]<<8)|cs[3];
			cs+=4;
			l=BGBBTJ_BT1C_ConvRGB31FSToRGB24(ctx, l);
			ctx->pal256[j]=l;
			ctx->pal256w[j]=
				BGBBTJ_BT1C_ConvRGB31FSToRGB48F(ctx, l);
			ctx->pal256s[j]=
				BGBBTJ_BT1C_ConvRGB24ToRGB16(ctx, l);
		}
		break;
	case 0x89:
		j=*cs++;
		k=(*cs++)+1;
		for(; j<k; j++)
		{
			l=(cs[0]<<24)|(cs[1]<<16)|(cs[2]<<8)|cs[3];
			cs+=4;
			l=BGBBTJ_BT1C_ConvRGB31FUToRGB24(ctx, l);
			ctx->pal256[j]=l;
			ctx->pal256w[j]=
				BGBBTJ_BT1C_ConvRGB31FUToRGB48F(ctx, l);
			ctx->pal256s[j]=
				BGBBTJ_BT1C_ConvRGB24ToRGB16(ctx, l);
		}
		break;
	case 0x8A:
		j=(cs[0]<<8)|cs[1]; cs+=2;
		ctx->modes[(j>>12)&7]=j&4095;
		break;
	case 0x8B:
		for(j=0; j<8; j++)
		{
			ctx->modes[j]=j;
		}
		ctx->curmode=0;
		ctx->nextmode=0;
		break;

	case 0x8C:
		j=(cs[0]<<8)|cs[1]; cs+=2;
		if((j>>10)>32)
		{
			break;
		}
		ret=-1;
		break;

	case 0x8F:
		j=*cs++;
		ctx->diff_m=j;
		break;
	case 0x90: case 0x91:
	case 0x92: case 0x93:
	case 0x94: case 0x95:
	case 0x96: case 0x97:
	case 0x98: case 0x99:
	case 0x9A: case 0x9B:
	case 0x9C: case 0x9D:
	case 0x9E: case 0x9F:
		if(!ctx->diff_m)
			ctx->diff_m=1;
		ctx->diff_sc=cmd-0x90;
		break;
	case 0xA0: case 0xA1:
	case 0xA2: case 0xA3:
	case 0xA4: case 0xA5:
	case 0xA6: case 0xA7:
		k=cmd-0xA0;
		ctx->clrdb_mode=k;
		break;
	default:
		ret=-1;
		break;
	}
	*rcs=cs;
	return(ret);
}
