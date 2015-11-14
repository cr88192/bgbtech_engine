#include <bgbbtj.h>

#define BTIC1C_ENABLE_DIFFCLR

extern byte bt1c_blkmap1[256];
extern byte bt1c_blkmap2[256];
extern byte bt1c_blkmap3[256];
extern byte bt1c_blkmap4[256];

extern byte bt1c_blkalphap[256];
extern byte bt1c_blkalphamape1[4096];
extern byte bt1c_blkalphamape2[4096];

BGBBTJ_API int BGBBTJ_BT1CF_EncodeBlocksDXT1(
	BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *obuf, int osz, byte *blks, byte *lblks,
	int xs, int ys, int fl)
{
	return(BGBBTJ_BT1CF_EncodeBlocksDXTn(ctx,
		obuf, osz, blks, lblks, xs, ys, fl, 8));
}

BGBBTJ_API int BGBBTJ_BT1CF_EncodeBlocksDXT5(
	BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *obuf, int osz, byte *blks, byte *lblks,
	int xs, int ys, int fl)
{
	int i, j;
	
	i=BGBBTJ_BT1CF_EncodeBlocksDXTn(ctx,
		obuf, osz, blks+8, lblks?(lblks+8):NULL, xs, ys, fl, 16);
//	j=BGBBTJ_BT1C_EncodeBlocksBC4(ctx,
//		obuf+i, osz-i, blks, lblks, xs, ys,
//		fl, 16);
//		fl|BGBBTJ_BTIC1C_EFL_BLOCKLZ, 16);
//	return(i+j);
	return(i);
}

BGBBTJ_API int BGBBTJ_BT1CF_EncodeBlocksMB5C(
	BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *obuf, int osz, byte *blks, byte *lblks,
	int xs, int ys, int fl)
{
	int i, j;
	
	i=BGBBTJ_BT1CF_EncodeBlocksDXTn(ctx,
		obuf, osz, blks+8, lblks?(lblks+8):NULL, xs, ys, fl, 32);
//	j=BGBBTJ_BT1C_EncodeBlocksBC4(ctx,
//		obuf+i, osz-i, blks, lblks, xs, ys,
//		fl, 32);
	return(i);
//	return(i+j);
}

BGBBTJ_API int BGBBTJ_BT1CF_EncodeBlocksDXT5Z(
	BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *obuf, int osz, byte *blks, byte *lblks,
	int xs, int ys, int fl)
{
	byte *tbuf1, *tbuf2;
	int sz1, sz2;
	
//	if(!(fl&BGBBTJ_BTIC1C_EFL_BLOCKZI))
//	{
//		sz1=BGBBTJ_BT1CF_EncodeBlocksDXT5(ctx,
//			obuf, osz, blks, lblks, xs, ys, fl);
//		return(sz1);
//	}
	
	tbuf1=malloc(osz);
	tbuf2=malloc(osz);
	sz1=BGBBTJ_BT1CF_EncodeBlocksDXT5(ctx,
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
	sz2=BTLZA_BitEncF_EncodeStreamXLvlZl(tbuf1, tbuf2+6, sz1, osz-6, 3);
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

BGBBTJ_API int BGBBTJ_BT1CF_EncodeBlocksMB5CZ(
	BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *obuf, int osz, byte *blks, byte *lblks,
	int xs, int ys, int fl)
{
	byte *tbuf1, *tbuf2;
	int sz1, sz2;
	
//	if(!(fl&BGBBTJ_BTIC1C_EFL_BLOCKZI))
//	{
//		sz1=BGBBTJ_BT1CF_EncodeBlocksMB5C(ctx,
//			obuf, osz, blks, lblks, xs, ys, fl);
//		return(sz1);
//	}
	
	tbuf1=malloc(osz);
	tbuf2=malloc(osz);
	sz1=BGBBTJ_BT1CF_EncodeBlocksMB5C(ctx,
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
	sz2=BTLZA_BitEncF_EncodeStreamXLvlZl(tbuf1, tbuf2+6, sz1, osz-6, 3);
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

BGBBTJ_API int BGBBTJ_BT1CF_EncodeBlocksDXTn(
	BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *obuf, int osz, byte *blks, byte *lblks,
	int xs, int ys, int fl, int stride)
{
	byte tb[32];
	byte *cs, *cse, *csm, *csl, *csle;
	byte *ct, *cte, *ctb;
	int xs1, ys1, n, n1;
	int xs2, ys2, xs3, ys3;
	int i0, i1, i2, i3;
	int i4, i5, i6, i7;
	int bl, bd, lbjp;
	int i, j, k, l;
	
	BGBBTJ_BT1C_Init();

#ifdef BTIC1C_ENABLE_DIFFCLR
	fl|=BGBBTJ_BTIC1C_EFL_DIFFCLR;
#endif

	ctx->eflags=fl;
	ctx->eflags&=~BGBBTJ_BTIC1C_EFL_BLKMB1;
	if(stride==32)
		ctx->eflags|=BGBBTJ_BTIC1C_EFL_BLKMB1;

	xs1=(xs+3)/4;
	ys1=(ys+3)/4;
	n=xs1*ys1;
	
	cs=blks; cse=cs+n*stride;
	csl=lblks; csle=csl+n*stride;
	ct=obuf;
	
	lbjp=0;
	
	*ct++=0xE1;		*ct++=0x00;
	*ct++=0x00;		*ct++=0x00;

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
		
		//Controller: Version
		*ct++=0xEF;
		*ct++=0x81;
		*ct++=BGBBTJ_BTIC1C_VERSION;

#ifdef BTIC1C_ENABLE_DIFFCLR
#endif
	}

	ctx->curmode=0;
	
#ifdef BTIC1C_ENABLE_DIFFCLR
	ctx->diff_m=0;
	ctx->diff_sc=-1;
	ctx->diff_lsc=-1;

	ctx->diff_lclra=0;
	ctx->diff_lclrb=0;
	ctx->diff_lclrc=0;
	ctx->diff_lclrd=0;

	*ct++=0xF8;
	*ct++=0x8F;
	*ct++=0x04;
#endif

	while(cs<cse)
	{
#if 1
		if(lblks && !memcmp(cs, csl, 8))
		{
			if(ctx->eflags&BGBBTJ_BTIC1C_EFL_BLKMB1)
			{
				for(i=1; (i<8192) && ((cs+i*stride)<cse); i++)
				{
					if(memcmp(cs+i*stride, csl+i*stride, 16))
						break;
				}
			}else
			{
				for(i=1; (i<8192) && ((cs+i*stride)<cse); i++)
				{
					if(memcmp(cs+i*stride, csl+i*stride, 8))
						break;
				}
			}

			if(i>32)
			{
				j=i-1;
				*ct++=0xEE;
				*ct++=0x00+(j>>8);
				*ct++=j&255;
				cs+=i*stride; csl+=i*stride;
				continue;
			}

			*ct++=0x80+(i-1);
			cs+=i*stride; csl+=i*stride;
			continue;
		}
#endif

		if((ctx->eflags&BGBBTJ_BTIC1C_EFL_BLKMB1) &&
			(cs[0]==0x55) && (cs[2]==0xAB))
		{
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
			continue;
		}

		i0=cs[0]|(cs[1]<<8);
		i1=cs[2]|(cs[3]<<8);
//		BGBBTJ_BT1C_GetBlockColorsAB_AsRGB23(ctx, cs, &i4, &i5);
		BGBBTJ_BT1C_GetBlockColorsExtAB(ctx, cs, &i4, &i5);

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
//				BGBBTJ_BT1C_GetBlockColorsAB_AsRGB23(ctx,
//					cs+i*stride, &i6, &i7);
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

			if((i>0) && BGBBTJ_BT1C_CheckBlockColor_IsRGB23(ctx, cs))
			{
#ifdef BTIC1C_ENABLE_DIFFCLR
				if(ctx->eflags&BGBBTJ_BTIC1C_EFL_DIFFCLR)
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

				i2=BGBBTJ_BT1C_GetBlockColorA_AsRGB23(ctx, cs);
//				i3=BGBBTJ_BT1C_GetBlockColorB_AsRGB23(ctx, cs);
			
				ct=BGBBTJ_BT1C_EncodeBlocksDXTn_EmitModeEscapeColorOnly(
					ctx, ct, 6);
				*ct++=0xA0+(i-1);

				ct[0]=(i2>>16)&0xFF;
				ct[1]=(i2>>8)&0xFF;
				ct[2]=i2&0xFF;
				ct+=3;

//				j=((i0&0xFFC0)>>1)|(i0&0x1F);
//				ct[0]=(j>>8)&0xFF;	ct[1]=j&0xFF;
//				ct+=2;

#ifdef BTIC1C_ENABLE_DIFFCLR
				if(ctx->eflags&BGBBTJ_BTIC1C_EFL_DIFFCLR)
				{
					ctx->diff_lclra=BGBBTJ_BT1C_ConvRGB23ToRGB24(ctx, i2);
					ctx->diff_lclrb=ctx->diff_lclra;
				}
#endif

				cs+=i*stride; csl+=i*stride;
				continue;
			}

			if(i>0)
			{
				ct=BGBBTJ_BT1C_EncodeBlocksDXTn_EmitModeEscapeColorOnly(
					ctx, ct, 0);

				*ct++=0xA0+(i-1);

				j=((i0&0xFFC0)>>1)|(i0&0x1F);
				ct[0]=(j>>8)&0xFF;	ct[1]=j&0xFF;
				ct+=2;

#ifdef BTIC1C_ENABLE_DIFFCLR
				if(ctx->eflags&BGBBTJ_BTIC1C_EFL_DIFFCLR)
				{
					ctx->diff_lclra=BGBBTJ_BT1C_ConvRGB15ToRGB24(ctx, j);
					ctx->diff_lclrb=ctx->diff_lclra;
				}
#endif
			
				cs+=i*stride; csl+=i*stride;
				continue;
			}
		}
#endif

		if(i0>i1)
//		if(1)
		{
			csm=bt1c_blkmap3;
//			csm=bt1c_blkmap4;

			for(i=1; (i<32) && ((cs+i*stride)<cse); i++)
			{
				if(lblks && !memcmp(cs+i*stride, csl+i*stride, 8))
					break;
				i2=cs[i*stride+0]|(cs[i*stride+1]<<8);
				i3=cs[i*stride+2]|(cs[i*stride+3]<<8);
				if((i2!=i0) || (i3!=i1))
					break;
//				BGBBTJ_BT1C_GetBlockColorsAB_AsRGB23(ctx,
//					cs+i*stride, &i6, &i7);
				BGBBTJ_BT1C_GetBlockColorsExtAB(ctx,
					cs+i*stride, &i6, &i7);
				if((i6!=i4) || (i7!=i5))
					break;
			}

			if((i>0) && BGBBTJ_BT1C_CheckBlockColor_IsRGB23(ctx, cs))
			{
#ifdef BTIC1C_ENABLE_DIFFCLR
				if(ctx->eflags&BGBBTJ_BTIC1C_EFL_DIFFCLR)
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
//						if(ctx->diff_sc<15)
//						if(ctx->diff_sc<5)
//						if(ctx->diff_sc<3)
//						if(ctx->diff_sc<1)
//							ctx->diff_sc++;
					}
				}
#endif

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
				
#ifdef BTIC1C_ENABLE_DIFFCLR
				if(ctx->eflags&BGBBTJ_BTIC1C_EFL_DIFFCLR)
				{
					ctx->diff_lclra=BGBBTJ_BT1C_ConvRGB23ToRGB24(ctx, i2);
					ctx->diff_lclrb=BGBBTJ_BT1C_ConvRGB23ToRGB24(ctx, i3);
				}
#endif
				continue;
			}

			if(i>1)
//			if(0)
			{
				ct=BGBBTJ_BT1C_EncodeBlocksDXTn_EmitModeEscapeColorOnly(
					ctx, ct, 0);

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

#ifdef BTIC1C_ENABLE_DIFFCLR
				if(ctx->eflags&BGBBTJ_BTIC1C_EFL_DIFFCLR)
				{
					ctx->diff_lclra=BGBBTJ_BT1C_ConvRGB15ToRGB24(ctx, j);
					ctx->diff_lclrb=BGBBTJ_BT1C_ConvRGB15ToRGB24(ctx, k);
				}
#endif
				continue;
			}
			
			ct=BGBBTJ_BT1C_EncodeBlocksDXTn_EmitModeEscape(
				ctx, ct, 0);

			j=((i0&0xFFC0)>>1)|(i0&0x1F);
			k=((i1&0xFFC0)>>1)|(i1&0x1F);
			k|=0x8000;
		
			ct[0]=(j>>8)&0xFF;	ct[1]=j&0xFF;
			ct[2]=(k>>8)&0xFF;	ct[3]=k&0xFF;
			ct[4]=csm[cs[4]];	ct[5]=csm[cs[5]];
			ct[6]=csm[cs[6]];	ct[7]=csm[cs[7]];
			cs+=stride; csl+=stride; ct+=8;
			
#ifdef BTIC1C_ENABLE_DIFFCLR
			if(ctx->eflags&BGBBTJ_BTIC1C_EFL_DIFFCLR)
			{
				ctx->diff_lclra=BGBBTJ_BT1C_ConvRGB15ToRGB24(ctx, j);
				ctx->diff_lclrb=BGBBTJ_BT1C_ConvRGB15ToRGB24(ctx, k);
			}
#endif
			continue;
		}

		ctx->diff_sc=-1;

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
//				BGBBTJ_BT1C_GetBlockColorsAB_AsRGB23(ctx,
//					cs+i*stride, &i6, &i7);
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

#ifdef BTIC1C_ENABLE_DIFFCLR
				if(ctx->eflags&BGBBTJ_BTIC1C_EFL_DIFFCLR)
				{
					ctx->diff_lclra=BGBBTJ_BT1C_ConvRGB23ToRGB24(ctx, i2);
					ctx->diff_lclrb=BGBBTJ_BT1C_ConvRGB23ToRGB24(ctx, i3);
				}
#endif
				continue;
			}

#if 1
			if(i>1)
			{
				ct=BGBBTJ_BT1C_EncodeBlocksDXTn_EmitModeEscapeColorOnly(
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

#ifdef BTIC1C_ENABLE_DIFFCLR
				if(ctx->eflags&BGBBTJ_BTIC1C_EFL_DIFFCLR)
				{
					ctx->diff_lclra=BGBBTJ_BT1C_ConvRGB15ToRGB24(ctx, j);
					ctx->diff_lclrb=BGBBTJ_BT1C_ConvRGB15ToRGB24(ctx, k);
				}
#endif
				continue;
			}
#endif

			ct=BGBBTJ_BT1C_EncodeBlocksDXTn_EmitModeEscapeColorOnly(
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
			continue;
		}
	}

	i=ct-obuf;
	obuf[1]=(i>>16)&0xFF;
	obuf[2]=(i>>8)&0xFF;
	obuf[3]=(i>>0)&0xFF;

	return(i);
}

BGBBTJ_API int BGBBTJ_BT1CF_EncodeImageFast(
	BGBBTJ_BTIC1C_Context *ctx, byte *rgba, byte *obuf, int osz,
	int xs, int ys, int qf, int clrs)
{
	return(BGBBTJ_BT1CF_EncodeImage(
		ctx, rgba, obuf, osz, xs, ys, qf, clrs));
}

BGBBTJ_API int BGBBTJ_BT1CF_EncodeImage(
	BGBBTJ_BTIC1C_Context *ctx, byte *rgba, byte *obuf, int osz,
	int xs, int ys, int qf, int clrs)
{
	BGBBTJ_BTIC1C_ImageInfo *hdr;
	int sz;

	hdr=ctx->image[0];
	
	sz=BGBBTJ_BT1CF_EncodeImage2(ctx, hdr,
		rgba, obuf, osz, xs, ys, qf, clrs);
	return(sz);
}

BGBBTJ_API int BGBBTJ_BT1CF_EncodeImage2(
	BGBBTJ_BTIC1C_Context *ctx, BGBBTJ_BTIC1C_ImageInfo *hdr,
	byte *rgba, byte *obuf, int osz,
	int xs, int ys, int qf, int clrs)
{
	s16 *rgba2;
	byte *ldat, *ct, *ct0;
	int xs1, ys1;
	int xs2, ys2;
	int xs3, ys3;
	int cr, cg, cb, ca;
	int sz, bsz, efl;
	int x1, y1, x2, y2;
	int i, j, k, l, n;
	
	hdr->width=xs;
	hdr->height=ys;
	hdr->flags=qf;
//	hdr->clrs=clrs;

	hdr->width2=(xs+3)/4;
	hdr->height2=(ys+3)/4;
	hdr->nblocks=hdr->width2*hdr->height2;
	hdr->nblocksmip=hdr->nblocks;

//	efl=0;
	efl=BGBBTJ_BTIC1C_EFL_EXTMODES;

	if(!hdr->rgba)
	{
//		i=(4*hdr->width2)*(4*hdr->height2)*4;
//		if(qf&BGBBTJ_QFL_MIPMAP)i*=2;
		i=hdr->nblocksmip*16*4;
		hdr->rgba=gcatomic(i);
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
	if(clrs==BGBBTJ_JPG_RGBA)
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
						hdr->rgba[k+0]=cr; hdr->rgba[k+1]=cg;
						hdr->rgba[k+2]=cb; hdr->rgba[k+3]=ca;
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
						hdr->rgba[k+0]=cr; hdr->rgba[k+1]=cg;
						hdr->rgba[k+2]=cb; hdr->rgba[k+3]=ca;
					}
				}
			}
		}
		
//		BGBBTJ_Tex_Dither15(hdr->rgba, hdr->rgba, xs1, ys1);
	}
#endif

//	ldat=hdr->ldata;
//	if(qf&BGBBTJ_QFL_IFRAME)
//		ldat=NULL;

	ldat=NULL;
	if(qf&BGBBTJ_QFL_PFRAME)
		ldat=hdr->ldata;

	switch(clrs)
	{
	case BGBBTJ_JPG_RGBA:
			BGBBTJ_BCn_EncodeImageDXT5B(hdr->fpdata, hdr->rgba, xs1, ys1, 4);
			BGBBTJ_BCn_FlipImageDXT5(hdr->fpdata, hdr->data, xs1, ys1);
			sz=BGBBTJ_BT1CF_EncodeBlocksDXT5(hdr, obuf, osz,
				hdr->data, ldat, xs1, ys1, 0);
			bsz=16;
		break;
	case BGBBTJ_JPG_RGB:
		BGBBTJ_BCn_EncodeImageDXT1A(hdr->fpdata, rgba, xs, ys, 3);
//		BGBBTJ_BCn_EncodeImageDXT1F(hdr->fpdata, rgba, xs, ys, 3);
		BGBBTJ_BCn_FlipImageDXT1(hdr->fpdata, hdr->data, xs, ys, 8);
		sz=BGBBTJ_BT1CF_EncodeBlocksDXT1(hdr, obuf, osz,
			hdr->data, ldat, xs, ys, 0);
		bsz=8;
		break;
	case BGBBTJ_JPG_BGRA:
#if 1
//		BGBBTJ_BCn_EncodeImageMB1CFB_BGRA(hdr->fpdata, rgba, xs, ys, 4);
//		BGBBTJ_BCn_FlipImageMB5C(hdr->fpdata, hdr->data, xs, ys);
		BGBBTJ_BCn_EncodeImageMB1CFB_FlipBGRA(hdr->data, rgba, xs, ys, 4);
//		sz=BGBBTJ_BT1CF_EncodeBlocksMB5C(hdr, obuf, osz,
//			hdr->data, ldat, xs, ys, efl);
		sz=BGBBTJ_BT1CF_EncodeBlocksMB5CZ(hdr, obuf, osz,
			hdr->data, ldat, xs, ys, efl);
		bsz=32;
#endif
		break;
	case BGBBTJ_JPG_BGR:
//		BGBBTJ_BCn_EncodeImageDXT1F_BGRA(hdr->fpdata, rgba, xs, ys, 3);
		BGBBTJ_BCn_EncodeImageDXT1A_BGRA(hdr->fpdata, rgba, xs, ys, 3);
		BGBBTJ_BCn_FlipImageDXT1(hdr->fpdata, hdr->data, xs, ys, 8);
		sz=BGBBTJ_BT1CF_EncodeBlocksDXT1(hdr, obuf, osz,
			hdr->data, ldat, xs, ys, 0);
		bsz=8;
		break;

	default:
		break;
	}
	
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
