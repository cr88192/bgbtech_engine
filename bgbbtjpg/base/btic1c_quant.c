#include <bgbbtj.h>

BGBBTJ_API int BGBBTJ_BT1CQ_CalcColorRMSE(
	byte *rgb0, byte *rgb1)
{
//	int y0, y1, dy;
	int dr, dg, db, da;
	int d;

//	y0=(rgb0[0]+2*rgb0[1]+rgb0[2])>>2;
//	y1=(rgb1[0]+2*rgb1[1]+rgb1[2])>>2;
	dr=rgb0[0]-rgb1[0];
	dg=rgb0[1]-rgb1[1];
	db=rgb0[2]-rgb1[2];
	da=rgb0[3]-rgb1[3];
//	dy=y0-y1;
	
//	d=(dr*dr)+(dg*dg)+(db*db)+(da*da)+(dy*dy);
	d=(dr*dr)+2*(dg*dg)+(db*db)+(da*da);
//	d=d/5;
	return(d);
}

BGBBTJ_API double BGBBTJ_BT1CQ_CalcBlockRMSE(
	BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *blka, byte *blkb)
{
	byte rgba0[16*4];
	byte rgba1[16*4];
	int e;
	int i;
	
	if((blkb[0]==0x55) && (blkb[1]==0xAA) &&
		(blkb[2]==0xAA) && (blkb[3]==0x55))
	{
//		e=16*1024;
		e=16*32768;
		return(e);
	}
	
	BGBBTJ_BT1CQ_DecodeBlock(ctx, blka, rgba0);
	BGBBTJ_BT1CQ_DecodeBlock(ctx, blkb, rgba1);
	
	e=0;
	for(i=0; i<16; i++)
	{
		e+=BGBBTJ_BT1CQ_CalcColorRMSE(rgba0+i*4, rgba1+i*4);
	}
//	e=sqrt(e/(5*16));
	e=sqrt(e/5.0);
	return(e);
}

BGBBTJ_API void BGBBTJ_BT1CQ_CalcBlockTruncate(
	BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *blka, byte *blkb)
{
	blkb[0]=blka[0];	blkb[1]=blka[1];
	blkb[2]=blka[2];	blkb[3]=blka[3];
	blkb[4]=blka[4];	blkb[5]=blka[5];
	blkb[6]=blka[6];	blkb[7]=blka[7];
	
	if(ctx->eflags&BGBBTJ_BTIC1C_EFL_BLKMB1)
	{
		blkb[ 8]=0;	blkb[ 9]=0;
		blkb[10]=0;	blkb[11]=0;
		blkb[12]=0;	blkb[13]=0;
		blkb[14]=0;	blkb[15]=0;
	}
}

BGBBTJ_API void BGBBTJ_BT1CQ_CalcBlockAsFlat(
	BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *blka, byte *blkb)
{
	byte rgba0[16*4];
//	byte rgba1[16*4];
	int cr, cg, cb, ca;
	int cr2, cg2, cb2;
	int dr, dg, db;
	int i, j, k, l;

	if((blka[0]==blka[2]) && (blka[1]==blka[3]))
//	if(1)
	{
//		memcpy(blkb, blka, 8);
		BGBBTJ_BT1CQ_CopyBlock(ctx, blka, blkb);
		if((blkb[0]==0x55) || (blkb[0]==0xAA))
		{
//			blkb[0]^=1;
//			blkb[2]^=1;
			blkb[0]^=32;
			blkb[2]^=32;
		}
		return;
	}

	BGBBTJ_BT1CQ_DecodeBlock(ctx, blka, rgba0);
	
	cr=0; cg=0; cb=0; ca=0;
	for(i=0; i<16; i++)
	{
		cr+=rgba0[i*4+0];	cg+=rgba0[i*4+1];
		cb+=rgba0[i*4+2];	ca+=rgba0[i*4+3];
	}
	
	cr=cr/16;	cg=cg/16;
	cb=cb/16;	ca=ca/16;

	cr2=cr; cg2=cg; cb2=cb;
	cr=BGBBTJ_BCn_RoundColorNearest5Bits(cr);
	cg=BGBBTJ_BCn_RoundColorNearest6Bits(cg);
	cb=BGBBTJ_BCn_RoundColorNearest5Bits(cb);

	if(ca<128)
	{
		i=((cr<<8)&0xF800) | ((cg<<3)&0x07E0) | ((cb>>3)&0x001F);
		blkb[0]=i;		blkb[1]=i>>8;
		blkb[2]=i;		blkb[3]=i>>8;
		blkb[4]=255;	blkb[5]=255;
		blkb[6]=255;	blkb[7]=255;

		if(ctx->eflags&BGBBTJ_BTIC1C_EFL_BLKMB1)
		{
//			dr=cr2-cr; dg=cg2-cg; db=cb2-cb;
//			j=((dr<<11)&0xF800) | ((dg<<5)&0x07E0) | ((db)&0x001F);
//			blkb[ 8]=j;		blkb[ 9]=j>>8;
//			blkb[10]=j;		blkb[11]=j>>8;

			blkb[ 8]=cr2;	blkb[ 9]=cg2;
			blkb[10]=cb2;	blkb[11]=ca;
			blkb[12]=cr2;	blkb[13]=cg2;
			blkb[14]=cb2;	blkb[15]=ca;
		}
	}else
	{
		i=((cr<<8)&0xF800) | ((cg<<3)&0x07E0) | ((cb>>3)&0x001F);
		blkb[0]=i;	blkb[1]=i>>8;
		blkb[2]=i;	blkb[3]=i>>8;
		blkb[4]=0;	blkb[5]=0;
		blkb[6]=0;	blkb[7]=0;

		if(ctx->eflags&BGBBTJ_BTIC1C_EFL_BLKMB1)
		{
//			dr=cr2-cr; dg=cg2-cg; db=cb2-cb;
//			j=((dr<<11)&0xF800) | ((dg<<5)&0x07E0) | ((db)&0x001F);
//			blkb[ 8]=j;		blkb[ 9]=j>>8;
//			blkb[10]=j;		blkb[11]=j>>8;

			blkb[ 8]=cr2;	blkb[ 9]=cg2;
			blkb[10]=cb2;	blkb[11]=ca;
			blkb[12]=cr2;	blkb[13]=cg2;
			blkb[14]=cb2;	blkb[15]=ca;
		}
	}

	if((blkb[0]==0x55) || (blkb[0]==0xAA))
	{
//		blkb[0]^=1;
		blkb[0]^=32;
	}
}

void BGBBTJ_BT1CQ_CalcBlockAs2x2(
	BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *blka, byte *blkb)
{
//	static byte halfidx[16]=
//	{ 0, 2, 2, 2,
//	  2, 1, 3, 3,
//	  2, 3, 2, 2,
//	  2, 3, 2, 3};
	static byte halfidx[16]=
	{ 0x0, 0xA, 0xA, 0xA,
	  0xA, 0x5, 0xF, 0xF,
	  0xA, 0xF, 0xA, 0xA,
	  0xA, 0xF, 0xA, 0xF};
	
	blkb[0]=blka[0]; blkb[1]=blka[1];
	blkb[2]=blka[2]; blkb[3]=blka[3];
	
	blkb[4]=halfidx[blka[4]&15]|(halfidx[blka[4]>>4]<<4);
	blkb[5]=halfidx[blka[5]&15]|(halfidx[blka[5]>>4]<<4);
	blkb[5]=(blkb[4]&0xCC)|(blkb[5]&0x33);
	blkb[4]=halfidx[blkb[5]&15]|(halfidx[blkb[5]>>4]<<4);
	blkb[5]=blkb[4];

	blkb[6]=halfidx[blka[6]&15]|(halfidx[blka[6]>>4]<<4);
	blkb[7]=halfidx[blka[7]&15]|(halfidx[blka[7]>>4]<<4);
	blkb[7]=(blkb[6]&0xCC)|(blkb[7]&0x33);
	blkb[6]=halfidx[blkb[7]&15]|(halfidx[blkb[7]>>4]<<4);
	blkb[7]=blkb[6];
	
//	blkb[4]=halfidx[blka[4]&15]|(halfidx[blka[4]>>4]<<4);
//	blkb[5]=halfidx[blka[5]&15]|(halfidx[blka[5]>>4]<<4);
//	blkb[6]=halfidx[blka[6]&15]|(halfidx[blka[6]>>4]<<4);
//	blkb[7]=halfidx[blka[7]&15]|(halfidx[blka[7]>>4]<<4);
//	blkb[5]=blkb[4];
//	blkb[7]=blkb[6];

	if(ctx->eflags&BGBBTJ_BTIC1C_EFL_BLKMB1)
	{
		blkb[ 8]=blka[ 8];	blkb[ 9]=blka[ 9];
		blkb[10]=blka[10];	blkb[11]=blka[11];
		blkb[12]=blka[12];	blkb[13]=blka[13];
		blkb[14]=blka[14];	blkb[15]=blka[15];
	}
}

void BGBBTJ_BT1CQ_CalcBlockAs1bpp(
	BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *blka, byte *blkb)
{
//	static byte halfidx[16]=
//	{ 0, 2, 2, 2,
//	  2, 1, 3, 3,
//	  2, 3, 2, 2,
//	  2, 3, 2, 3};
	static byte halfidx[16]=
	{ 0x0, 0x1, 0x0, 0x1,
	  0x4, 0x5, 0x4, 0x5,
	  0x0, 0x1, 0x0, 0x1,
	  0x4, 0x5, 0x4, 0x5};
	
	blkb[0]=blka[0]; blkb[1]=blka[1];
	blkb[2]=blka[2]; blkb[3]=blka[3];
	
	blkb[4]=halfidx[blka[4]&15]|(halfidx[blka[4]>>4]<<4);
	blkb[5]=halfidx[blka[5]&15]|(halfidx[blka[5]>>4]<<4);
	blkb[6]=halfidx[blka[6]&15]|(halfidx[blka[6]>>4]<<4);
	blkb[7]=halfidx[blka[7]&15]|(halfidx[blka[7]>>4]<<4);

	if(ctx->eflags&BGBBTJ_BTIC1C_EFL_BLKMB1)
	{
		blkb[ 8]=blka[ 8];	blkb[ 9]=blka[ 9];
		blkb[10]=blka[10];	blkb[11]=blka[11];
		blkb[12]=blka[12];	blkb[13]=blka[13];
		blkb[14]=blka[14];	blkb[15]=blka[15];
	}
}

BGBBTJ_API void BGBBTJ_BT1CQ_CalcBlockAsPriorColor(
	BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *blka, byte *blkb, byte *blkc)
{
	blkc[0]=blka[0];	blkc[1]=blka[1];
	blkc[2]=blka[2];	blkc[3]=blka[3];
	blkc[4]=blkb[4];	blkc[5]=blkb[5];
	blkc[6]=blkb[6];	blkc[7]=blkb[7];

	if(ctx->eflags&BGBBTJ_BTIC1C_EFL_BLKMB1)
	{
		blkc[ 8]=blka[ 8];	blkc[ 9]=blka[ 9];
		blkc[10]=blka[10];	blkc[11]=blka[11];
		blkc[12]=blka[12];	blkc[13]=blka[13];
		blkc[14]=blka[14];	blkc[15]=blka[15];
	}
}

BGBBTJ_API void BGBBTJ_BT1CQ_CalcBlockAsPriorFlatColor(
	BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *blka, byte *blkb, byte *blkc)
{
	if(((blka[0]==0x55) && (blka[2]==0xAA)) ||
		((blkb[0]==0x55) && (blkb[2]==0xAA)))
	{
		blkc[0]=0x55; blkc[1]=0xAA;
		blkc[2]=0xAA; blkc[3]=0x55;
		blkc[4]=0x55; blkc[5]=0xAA;
		blkc[6]=0xAA; blkc[7]=0x55;
		return;
	}
	
	blkc[0]=blka[0];	blkc[1]=blka[1];
	blkc[2]=blka[0];	blkc[3]=blka[1];
	blkc[4]=blkb[4];	blkc[5]=blkb[5];
	blkc[6]=blkb[6];	blkc[7]=blkb[7];
	
	if(ctx->eflags&BGBBTJ_BTIC1C_EFL_BLKMB1)
	{
		blkc[ 8]=blka[ 8];	blkc[ 9]=blka[ 9];
		blkc[10]=blka[10];	blkc[11]=blka[11];
		blkc[12]=blka[ 8];	blkc[13]=blka[ 9];
		blkc[14]=blka[10];	blkc[15]=blka[11];
	}
}

BGBBTJ_API void BGBBTJ_BT1CQ_CalcBlockAsDiffScale(
	BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *oblks, byte *ct, int xs1, int ys1,
	byte *blka, byte *blkb, int fl, int stride, int sc)
{
	int ca, cb;

	if((ctx->diff_lclra==((u32)-1)) &&
		(ctx->diff_lclrb==((u32)-1)))
	{
		BGBBTJ_BT1CQ_ThrashBlock(blkb);
		return;
	}

	ct[0]=blka[0];	ct[1]=blka[1];
	ct[2]=blka[2];	ct[3]=blka[3];

	ca=BGBBTJ_BT1C_EncodePredictDiffColorA(
		ctx, oblks, ct, xs1, ys1,
		fl, stride, sc);
	cb=BGBBTJ_BT1C_EncodePredictDiffColorB(
		ctx, oblks, ct, xs1, ys1,
		fl, stride, sc);

	if((ca<0) || (cb<0))
	{
		BGBBTJ_BT1CQ_ThrashBlock(blkb);
		return;
	}

	blkb[0]=ca&255;		blkb[1]=(ca>>8)&255;
	blkb[2]=cb&255;		blkb[3]=(cb>>8)&255;
//	blkb[0]=blka[0];	blkb[1]=blka[1];
//	blkb[2]=blka[0];	blkb[3]=blka[1];
	blkb[4]=blka[4];	blkb[5]=blka[5];
	blkb[6]=blka[6];	blkb[7]=blka[7];

	if(ctx->eflags&BGBBTJ_BTIC1C_EFL_BLKMB1)
	{
		blkb[ 8]=0;	blkb[ 9]=0;
		blkb[10]=0;	blkb[11]=0;
		blkb[12]=0;	blkb[13]=0;
		blkb[14]=0;	blkb[15]=0;

//		blkb[ 8]=blka[ 8];	blkb[ 9]=blka[ 9];
//		blkb[10]=blka[10];	blkb[11]=blka[11];
//		blkb[12]=blka[12];	blkb[13]=blka[13];
//		blkb[14]=blka[14];	blkb[15]=blka[15];
	}
}

BGBBTJ_API void BGBBTJ_BT1CQ_CalcBlockAsIndexColor(
	BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *blka, byte *blkb)
{
	int i, j;

	if(!ctx->pal_clr)
	{
		BGBBTJ_BT1CQ_ThrashBlock(blkb);
		return;
	}
	
	if((blka[0]==0x55) && (blka[2]==0x55))
	{
		blkb[0]=blka[0];	blkb[1]=blka[1];
		blkb[2]=blka[2];	blkb[3]=blka[3];
		blkb[4]=blka[4];	blkb[5]=blka[5];
		blkb[6]=blka[6];	blkb[7]=blka[7];
		return;
	}
	
	i=blka[0]|(blka[1]<<8);
	j=blka[2]|(blka[3]<<8);
	i=BGBBTJ_BT1X_LookupColorIndex555_RGB565P24(
		i, ctx->pal_clr, ctx->pal_nclr,
		ctx->pal_index, ctx->pal_chain);
	j=BGBBTJ_BT1X_LookupColorIndex555_RGB565P24(
		j, ctx->pal_clr, ctx->pal_nclr,
		ctx->pal_index, ctx->pal_chain);
	
	blkb[0]=0x55;	blkb[1]=i;
	blkb[2]=0x55;	blkb[3]=j;	
	blkb[4]=blka[4];	blkb[5]=blka[5];
	blkb[6]=blka[6];	blkb[7]=blka[7];
}

void BGBBTJ_BT1CQ_CalcBlockAsIndexColor1bpp(
	BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *blka, byte *blkb)
{
	byte tblk[16];
	
	BGBBTJ_BT1CQ_CalcBlockAsIndexColor(ctx, blka, tblk);
	BGBBTJ_BT1CQ_CalcBlockAs1bpp(ctx, tblk, blkb);
}

void BGBBTJ_BT1CQ_CalcBlockAsIndexColor2x2(
	BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *blka, byte *blkb)
{
	byte tblk[16];
	
	BGBBTJ_BT1CQ_CalcBlockAsIndexColor(ctx, blka, tblk);
	BGBBTJ_BT1CQ_CalcBlockAs2x2(ctx, tblk, blkb);
}

void BGBBTJ_BT1CQ_CalcBlockAsIndexColorFlat(
	BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *blka, byte *blkb)
{
	byte tblk[16];
	BGBBTJ_BT1CQ_CalcBlockAsFlat(ctx, blka, tblk);
	BGBBTJ_BT1CQ_CalcBlockAsIndexColor(ctx, tblk, blkb);
}

BGBBTJ_API void BGBBTJ_BT1CQ_ThrashBlock(byte *blk)
{
	blk[0]=0x55; blk[1]=0xAA;
	blk[2]=0xAA; blk[3]=0x55;
	blk[4]=0x55; blk[5]=0xAA;
	blk[6]=0xAA; blk[7]=0x55;
}

void BGBBTJ_BT1CQ_CopyBlock(
	BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *blka, byte *blkb)
{
	if(ctx->eflags&BGBBTJ_BTIC1C_EFL_BLKMB1)
	{
//		memcpy(blkb, blka, 16);
		memcpy(blkb, blka, 24);
	}else
	{
		memcpy(blkb, blka, 8);
	}
}

BGBBTJ_API int BGBBTJ_BT1C_QuantizeBlocksDXTn(
	BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *oblks, byte *blks, byte *lblks,
	int xs, int ys, int qfl, int stride)
{
	static const int mvtab[32]=
	{
	-1, -1,	 0, -1,	 1, -1,  1,  0,
	 1,  1,  0,  1,	-1,  1, -1,  0,
	-2,  0,  0, -2,	 2,  0,  0,  2,
	-3,  0,  0, -3,	 3,  0,  0,  3,
	};

	byte blkwin[256*32];
	byte tblk[32], tblk2[32], tlblk[32];
	byte aiblk[32], ablk[32];
	byte *cblk, *ciblk, *clblk, *coblk, *cwblk, *cwblk2;
	byte *csl, *csle, *cs1, *bcs1;
	double e, be;
	double rmse, crmse;
	double f, g;
	u32 seed;
	byte blkrov;
	int xs1, ys1;
	int xs2, ys2;
	int xs3, ys3;
	int bx, by;
	int i, j, k, l, n, csc;
	
	rmse=100-(qfl&255);
//	rmse=(rmse*rmse)/100;
//	rmse=(rmse*rmse)/80.0;
//	rmse=pow(rmse, 1.5)/10.0;
//	rmse=pow(rmse, 1.5)/20.0;
	rmse=pow(rmse, 1.5)/40.0;
//	rmse=1;
	blkrov=0;
	
	qfl&=~BGBBTJ_BTIC1C_QFL_DIFFCLR;
	
	ctx->eflags&=~BGBBTJ_BTIC1C_EFL_BLKMB1;
	if(stride==32)
		ctx->eflags|=BGBBTJ_BTIC1C_EFL_BLKMB1;
	
//	if(rmse<4.0)
	if(rmse<1.0)
	{
//		if(oblks!=blks)
		if(1)
		{
			xs1=(xs+3)>>2; ys1=(ys+3)>>2;
			for(i=0; i<ys1; i++)
				for(j=0; j<xs1; j++)
			{
				cblk=blks+(i*xs1+j)*stride;
				coblk=oblks+(i*xs1+j)*stride;
				
				if((stride!=32) && ((cblk[0]==0x55) || (cblk[0]==0xAA)))
				{
//					memcpy(ablk, cblk, 8);
					BGBBTJ_BT1CQ_CopyBlock(ctx, cblk, ablk);
//					ablk[0]^=1;
					ablk[0]^=32;
//					memcpy(coblk, ablk, 8);
					BGBBTJ_BT1CQ_CopyBlock(ctx, ablk, coblk);
					continue;
				}
				
//				memcpy(coblk, cblk, 8);
				BGBBTJ_BT1CQ_CopyBlock(ctx, cblk, coblk);
			}
		}
		return(0);
	}
	
	for(i=0; i<256; i++)
		{ BGBBTJ_BT1CQ_ThrashBlock(blkwin+i*32); }
	
	BGBBTJ_BT1CQ_ThrashBlock(tlblk);
	xs1=(xs+3)>>2; ys1=(ys+3)>>2;
	n=xs1*ys1; csc=0;
	
	if(qfl&BGBBTJ_QFL_MIPMAP)
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
	}

	BGBBTJ_BT1C_EncodeClearDiffForBlock(ctx);

	csl=lblks; csle=csl+n*stride;

//	for(i=0; i<ys1; i++)
//		for(j=0; j<xs1; j++)
	for(i=0; i<n; i++)
	{
//		cblk=blks+(i*xs1+j)*stride;
//		coblk=oblks+(i*xs1+j)*stride;
		cblk=blks+i*stride;
		ciblk=cblk;
		coblk=oblks+i*stride;

//		memcpy(aiblk, ciblk, 8);
//		memcpy(ablk, ciblk, 8);
		BGBBTJ_BT1CQ_CopyBlock(ctx, ciblk, aiblk);
		BGBBTJ_BT1CQ_CopyBlock(ctx, ciblk, ablk);

		cblk=aiblk;
		crmse=rmse;

		//detect/avoid special cases
//		if((aiblk[0]==0x55) || (aiblk[0]==0xAA))
		if((stride!=32) && ((aiblk[0]==0x55) || (aiblk[0]==0xAA)))
			{ aiblk[0]^=1; }
//		if((aiblk[2]==0x55) || (aiblk[2]==0xAA))
//			{ aiblk[2]^=1; }

//		if((ablk[0]==0x55) || (ablk[0]==0xAA))
		if((stride!=32) && ((ablk[0]==0x55) || (ablk[0]==0xAA)))
			{ ablk[0]^=1; }

#if 1
		if(lblks)
		{
			seed=seed*65521+1;
			f=((seed>>16)&255)/255.0;
//			crmse=rmse*1.5;
//			crmse=rmse*2.0;
//			crmse=rmse*1.5+f;
//			crmse=rmse*1.0+2*f;
			crmse=rmse*1.5+2.5*f;
		}
#endif

		//is block sufficiently similar to last-frame block?
		if(lblks)
		{
//			clblk=lblks+(i*xs1+j)*stride;
			clblk=lblks+i*stride;

			e=BGBBTJ_BT1CQ_CalcBlockRMSE(ctx, cblk, clblk);
			if(e<=crmse)
			{
				BGBBTJ_BT1CQ_ThrashBlock(tlblk);
//				memcpy(coblk, clblk, 8);
				BGBBTJ_BT1CQ_CopyBlock(ctx, clblk, coblk);
				BGBBTJ_BT1C_EncodeClearDiffForBlock(ctx);
				continue;
			}
		}

		if(lblks && (qfl&BGBBTJ_BTIC1C_QFL_MOTION))
		{
			clblk=lblks+i*stride;
			bcs1=clblk;

			bx=0; by=0; be=999999999.0;
#if 0
			for(j=-2; j<=2; j++)
			{
				for(k=-2; k<=2; k++)
				{
					cs1=clblk+(j*xs1*stride)+k*stride;
					if(cs1<lblks)continue;
					if(cs1>=csle)continue;

					e=BGBBTJ_BT1CQ_CalcBlockRMSE(ctx, cblk, cs1);
					if(e<=be)
						{ bx=k; by=j; be=e; bcs1=cs1; }
				}
			}
#endif

#if 1
			for(j=0; j<16; j++)
			{
				cs1=clblk+
					(mvtab[j*2+1]*xs1*stride)+
					mvtab[j*2+0]*stride;
				if(cs1<lblks)continue;
				if(cs1>=csle)continue;

				e=BGBBTJ_BT1CQ_CalcBlockRMSE(ctx, cblk, cs1);
				if(e<=be)
					{ bx=mvtab[j*2+0]; by=mvtab[j*2+1]; be=e; bcs1=cs1; }
			}
#endif

			if(be<=crmse)
			{
				BGBBTJ_BT1CQ_ThrashBlock(tlblk);
//				memcpy(coblk, bcs1, 8);
				BGBBTJ_BT1CQ_CopyBlock(ctx, bcs1, coblk);
				BGBBTJ_BT1C_EncodeClearDiffForBlock(ctx);
				continue;
			}
		}

#if 1
		if(lblks)
		{
			seed=seed*65521+1;
			f=((seed>>16)&255)/255.0;
//			crmse=rmse*1.5;
//			crmse=rmse*2.0;
//			crmse=rmse*1.5+f;
			crmse=crmse*1.5+f;
		}
#endif

		if(qfl&BGBBTJ_BTIC1C_QFL_DIFFCLR)
		{
			BGBBTJ_BT1CQ_CalcBlockAsDiffScale(ctx,
				oblks, coblk, xs1, ys1,
				cblk, tblk, qfl, stride, csc);
			e=BGBBTJ_BT1CQ_CalcBlockRMSE(ctx, cblk, tblk);
			if(e<=crmse)
			{
//				memcpy(ablk, tblk, 8);
				BGBBTJ_BT1CQ_CopyBlock(ctx, tblk, ablk);
				cblk=ablk;
//				continue;
			}
			
			if(cblk!=ablk)
			{
				for(j=0; j<8; j++)
				{
					BGBBTJ_BT1CQ_CalcBlockAsDiffScale(ctx,
						oblks, coblk, xs1, ys1,
						cblk, tblk, qfl, stride, 0);
					e=BGBBTJ_BT1CQ_CalcBlockRMSE(ctx, cblk, tblk);
					if(e<=crmse)
					{
//						memcpy(ablk, tblk, 8);
						BGBBTJ_BT1CQ_CopyBlock(ctx, tblk, ablk);
						cblk=ablk; csc=0;
						break;
					}
				}
			}
		}
		
#if 1
		//can block be replaced with a flat color?
		if(qfl&BGBBTJ_BTIC1C_QFL_DIFFCLR)
//		if(0)
		{
			//are we using differential colors?
			//try to quantize to a differential color
			BGBBTJ_BT1CQ_CalcBlockAsFlat(ctx, ciblk, tblk2);
			BGBBTJ_BT1CQ_CalcBlockAsDiffScale(ctx,
				oblks, coblk, xs1, ys1,
				tblk2, tblk, qfl, stride, csc);
				
//			tblk[4]=0; tblk[5]=0;
//			tblk[6]=0; tblk[7]=0;
//			e=BGBBTJ_BT1CQ_CalcBlockRMSE(ciblk, tblk);
//			if(e<=rmse)
//			{
//				memcpy(ablk, tblk2, 8);
//				cblk=ablk;
//			}
		}else
		{
			BGBBTJ_BT1CQ_CalcBlockAsFlat(ctx, cblk, tblk);
		}
		e=BGBBTJ_BT1CQ_CalcBlockRMSE(ctx, cblk, tblk);
//		e=BGBBTJ_BT1CQ_CalcBlockRMSE(ctx, aiblk, tblk);
		if(e<=crmse)
		{
			//can we reuse last block's color
			BGBBTJ_BT1CQ_CalcBlockAsPriorFlatColor(
				ctx, tlblk, tblk, tblk2);
			e=BGBBTJ_BT1CQ_CalcBlockRMSE(ctx, cblk, tblk2);
//			e=BGBBTJ_BT1CQ_CalcBlockRMSE(ctx, aiblk, tblk2);
			if(e<=crmse)
			{
//				memcpy(coblk, tblk2, 8);
				BGBBTJ_BT1CQ_CopyBlock(ctx, tblk2, coblk);
				BGBBTJ_BT1C_EncodeSetupDiffForBlock(
					ctx, coblk, xs1, ys1, qfl, stride);
				continue;
			}

			//can we truncate extended color data?
			BGBBTJ_BT1CQ_CalcBlockTruncate(ctx, tblk, tblk2);
			e=BGBBTJ_BT1CQ_CalcBlockRMSE(ctx, cblk, tblk2);
//			if(e<=crmse)
//			if((e<=crmse) && (crmse>=64))
			if((e<=crmse) && (crmse>=128))
			{
				BGBBTJ_BT1CQ_CopyBlock(ctx, tblk2, coblk);
				BGBBTJ_BT1C_EncodeSetupDiffForBlock(
					ctx, coblk, xs1, ys1, qfl, stride);
				continue;
			}

//			BGBBTJ_BT1CQ_ThrashBlock(tlblk);
//			memcpy(tlblk, tblk, 8);
//			memcpy(coblk, tblk, 8);
			BGBBTJ_BT1CQ_CopyBlock(ctx, tblk, tlblk);
			BGBBTJ_BT1CQ_CopyBlock(ctx, tblk, coblk);
			BGBBTJ_BT1C_EncodeSetupDiffForBlock(
				ctx, coblk, xs1, ys1, qfl, stride);
			continue;
		}
#endif

		//Check Indexed Color Blocks
		if(ctx->pal_clr && (qfl&BGBBTJ_BTIC1C_QFL_EXTMODES))
		{
			//can block be replaced with a flat indexed color?
			BGBBTJ_BT1CQ_CalcBlockAsIndexColorFlat(ctx, cblk, tblk);
			e=BGBBTJ_BT1CQ_CalcBlockRMSE(ctx, cblk, tblk);
			if(e<=crmse)
			{
				//can we reuse last block's color
				BGBBTJ_BT1CQ_CalcBlockAsPriorFlatColor(
					ctx, tlblk, tblk, tblk2);
				e=BGBBTJ_BT1CQ_CalcBlockRMSE(ctx, cblk, tblk2);
				if(e<=crmse)
				{
//					memcpy(coblk, tblk2, 8);
					BGBBTJ_BT1CQ_CopyBlock(ctx, tblk2, coblk);
					BGBBTJ_BT1C_EncodeSetupDiffForBlock(
						ctx, coblk, xs1, ys1, qfl, stride);
					continue;
				}
//				memcpy(tlblk, tblk, 8); memcpy(coblk, tblk, 8);
				BGBBTJ_BT1CQ_CopyBlock(ctx, tblk, tlblk);
				BGBBTJ_BT1CQ_CopyBlock(ctx, tblk, coblk);
				BGBBTJ_BT1C_EncodeSetupDiffForBlock(
					ctx, coblk, xs1, ys1, qfl, stride);
				continue;
			}

#if 1
			BGBBTJ_BT1CQ_CalcBlockAsIndexColor2x2(ctx, cblk, tblk);
			e=BGBBTJ_BT1CQ_CalcBlockRMSE(ctx, cblk, tblk);
			if(e<=crmse)
			{
				//can we reuse last block's color
				BGBBTJ_BT1CQ_CalcBlockAsPriorColor(ctx, tlblk, tblk, tblk2);
				e=BGBBTJ_BT1CQ_CalcBlockRMSE(ctx, cblk, tblk2);
				if(e<=crmse)
				{
//					memcpy(ablk, tblk2, 8);
					BGBBTJ_BT1CQ_CopyBlock(ctx, tblk2, ablk);
					cblk=ablk;
				}
				else
				{
//					memcpy(ablk, tblk, 8);
					BGBBTJ_BT1CQ_CopyBlock(ctx, tblk, ablk);
					cblk=ablk;
				}

#if 1
//				memcpy(tlblk, cblk, 8);
//				memcpy(coblk, cblk, 8);
				BGBBTJ_BT1CQ_CopyBlock(ctx, cblk, tlblk);
				BGBBTJ_BT1CQ_CopyBlock(ctx, cblk, coblk);
				BGBBTJ_BT1C_EncodeSetupDiffForBlock(
					ctx, coblk, xs1, ys1, qfl, stride);
				continue;
#endif
			}
#endif

			BGBBTJ_BT1CQ_CalcBlockAsIndexColor1bpp(ctx, cblk, tblk);
			e=BGBBTJ_BT1CQ_CalcBlockRMSE(ctx, cblk, tblk);
			if(e<=crmse)
//			if(1)
			{
				//can we reuse last block's color
				BGBBTJ_BT1CQ_CalcBlockAsPriorColor(ctx, tlblk, tblk, tblk2);
				e=BGBBTJ_BT1CQ_CalcBlockRMSE(ctx, cblk, tblk2);
				if(e<=crmse)
				{
//					memcpy(ablk, tblk2, 8);
					BGBBTJ_BT1CQ_CopyBlock(ctx, tblk2, ablk);
					cblk=ablk;
				}
				else
				{
//					memcpy(ablk, tblk, 8);
					BGBBTJ_BT1CQ_CopyBlock(ctx, tblk, ablk);
					cblk=ablk;
				}

#if 1
//				memcpy(tlblk, cblk, 8);
//				memcpy(coblk, cblk, 8);
				BGBBTJ_BT1CQ_CopyBlock(ctx, cblk, tlblk);
				BGBBTJ_BT1CQ_CopyBlock(ctx, cblk, coblk);
				BGBBTJ_BT1C_EncodeSetupDiffForBlock(
					ctx, coblk, xs1, ys1, qfl, stride);
				continue;
#endif
			}
			
#if 0
			memcpy(tlblk, cblk, 8);
			memcpy(coblk, cblk, 8);
			continue;
#endif
		}

#if 1
		if(qfl&BGBBTJ_BTIC1C_QFL_EXTMODES)
		{
			BGBBTJ_BT1CQ_CalcBlockAs2x2(ctx, cblk, tblk);
			e=BGBBTJ_BT1CQ_CalcBlockRMSE(ctx, cblk, tblk);
			if(e<=crmse)
//			if(1)
			{
				//can we reuse last block's color
//				BGBBTJ_BT1CQ_CalcBlockAsPriorFlatColor(tlblk, tblk, tblk2);
				BGBBTJ_BT1CQ_CalcBlockAsPriorColor(ctx, tlblk, tblk, tblk2);
				e=BGBBTJ_BT1CQ_CalcBlockRMSE(ctx, cblk, tblk2);
//				e=BGBBTJ_BT1CQ_CalcBlockRMSE(aiblk, tblk2);
				if(e<=crmse)
				{
//					memcpy(ablk, tblk2, 8);
					BGBBTJ_BT1CQ_CopyBlock(ctx, tblk2, ablk);
					cblk=ablk;
				}else
				{
//					memcpy(ablk, tblk, 8);
					BGBBTJ_BT1CQ_CopyBlock(ctx, tblk, ablk);
					cblk=ablk;
				}
			}

#if 1
			BGBBTJ_BT1CQ_CalcBlockAs1bpp(ctx, cblk, tblk);
			e=BGBBTJ_BT1CQ_CalcBlockRMSE(ctx, cblk, tblk);
			if(e<=crmse)
			{
				//can we reuse last block's color
//				BGBBTJ_BT1CQ_CalcBlockAsPriorFlatColor(tlblk, tblk, tblk2);
				BGBBTJ_BT1CQ_CalcBlockAsPriorColor(ctx, tlblk, tblk, tblk2);
				e=BGBBTJ_BT1CQ_CalcBlockRMSE(ctx, cblk, tblk2);
//				e=BGBBTJ_BT1CQ_CalcBlockRMSE(ctx, aiblk, tblk2);
				if(e<=crmse)
				{
//					memcpy(coblk, tblk2, 8);
//					continue;
//					memcpy(ablk, tblk2, 8);
					BGBBTJ_BT1CQ_CopyBlock(ctx, tblk2, ablk);
					cblk=ablk;
				}else
				{
//					memcpy(ablk, tblk, 8);
					BGBBTJ_BT1CQ_CopyBlock(ctx, tblk, ablk);
					cblk=ablk;
				}

//				BGBBTJ_BT1CQ_ThrashBlock(tlblk);
//				memcpy(tlblk, tblk, 8);
//				memcpy(coblk, tblk, 8);
//				continue;
			}
#endif
		}
#endif

		//can we truncate extended color data?
		BGBBTJ_BT1CQ_CalcBlockTruncate(ctx, cblk, tblk);
		e=BGBBTJ_BT1CQ_CalcBlockRMSE(ctx, cblk, tblk);
//		if(e<=crmse)
		if((e<=crmse) && (crmse>=128))
		{
			BGBBTJ_BT1CQ_CopyBlock(ctx, tblk, ablk);
			cblk=ablk;
		}

		//can we reuse last block's color
		BGBBTJ_BT1CQ_CalcBlockAsPriorColor(ctx, tlblk, cblk, tblk);
		e=BGBBTJ_BT1CQ_CalcBlockRMSE(ctx, cblk, tblk);
//		e=BGBBTJ_BT1CQ_CalcBlockRMSE(ctx, aiblk, tblk);
		if(e<=crmse)
		{
//			memcpy(coblk, tblk, 8);
			BGBBTJ_BT1CQ_CopyBlock(ctx, tblk, coblk);
			BGBBTJ_BT1C_EncodeSetupDiffForBlock(
				ctx, coblk, xs1, ys1, qfl, stride);
			continue;
		}
		
		//can we replace the block with another recently seen block?
		if(qfl&BGBBTJ_QFL_BLOCKINDEX)
		{
//			for(k=0; k<256; k++)
			for(k=0; k<4; k++)
			{
				cwblk=blkwin+((blkrov+k)&255)*32;
				e=BGBBTJ_BT1CQ_CalcBlockRMSE(ctx, cblk, cwblk);
//				e=BGBBTJ_BT1CQ_CalcBlockRMSE(ctx, aiblk, cwblk);
				if(e<=rmse)break;
			}
			
//			if(k<256)
			if(k<4)
			{
				blkrov=(blkrov-1)&255;
				cwblk2=blkwin+blkrov*32;
//				memcpy(cwblk2, cwblk, 8);				
//				memcpy(coblk, cwblk, 8);
				BGBBTJ_BT1CQ_CopyBlock(ctx, cwblk, cwblk2);
				BGBBTJ_BT1CQ_CopyBlock(ctx, cwblk, coblk);
				BGBBTJ_BT1C_EncodeSetupDiffForBlock(
					ctx, coblk, xs1, ys1, qfl, stride);
				continue;
			}

			blkrov=(blkrov-1)&255;
			cwblk2=blkwin+blkrov*32;
//			memcpy(cwblk2, cblk, 8);
			BGBBTJ_BT1CQ_CopyBlock(ctx, cblk, cwblk2);
		}

//		memcpy(tlblk, cblk, 8);
//		memcpy(coblk, cblk, 8);
		BGBBTJ_BT1CQ_CopyBlock(ctx, cblk, tlblk);
		BGBBTJ_BT1CQ_CopyBlock(ctx, cblk, coblk);
		BGBBTJ_BT1C_EncodeSetupDiffForBlock(
			ctx, coblk, xs1, ys1, qfl, stride);
	}
	return(0);
}


BGBBTJ_API int BGBBTJ_BT1C_QuantizeBlocksDXT5(
	BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *oblks, byte *blks, byte *lblks,
	int xs, int ys, int qfl)
{
	return(BGBBTJ_BT1C_QuantizeBlocksDXTn(ctx,
		oblks+8, blks+8, lblks?(lblks+8):NULL, xs, ys, qfl, 16));
}

BGBBTJ_API int BGBBTJ_BT1C_QuantizeBlocksDXT1(
	BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *oblks, byte *blks, byte *lblks,
	int xs, int ys, int qfl)
{
	return(BGBBTJ_BT1C_QuantizeBlocksDXTn(ctx,
		oblks, blks, lblks, xs, ys, qfl, 8));
}

BGBBTJ_API int BGBBTJ_BT1C_QuantizeBlocksMB5C(
	BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *oblks, byte *blks, byte *lblks,
	int xs, int ys, int qfl)
{
	return(BGBBTJ_BT1C_QuantizeBlocksDXTn(ctx,
		oblks+8, blks+8, lblks?(lblks+8):NULL, xs, ys, qfl, 32));
}
