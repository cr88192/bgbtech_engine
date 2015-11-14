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

	ca2=BGBBTJ_BT1C_ConvRGB16ToRGB24(NULL, clra);
	cb2=BGBBTJ_BT1C_ConvRGB16ToRGB24(NULL, clrb);
	ca2=BGBBTJ_BT1C_SwapRGB24_RB(ca2);
	cb2=BGBBTJ_BT1C_SwapRGB24_RB(cb2);

	if(clra>clrb)
	{
		bgbbtj_bt1c_setule16(ct+ 8, clra);
		bgbbtj_bt1c_setule16(ct+10, clrb);
		bgbbtj_bt1c_setule32(ct+16, ca2);
		bgbbtj_bt1c_setule32(ct+20, cb2);
		csm=bt1c_blkmap1;
	}else
	{
		bgbbtj_bt1c_setule16(ct+ 8, clrb);
		bgbbtj_bt1c_setule16(ct+10, clra);
		bgbbtj_bt1c_setule32(ct+16, cb2);
		bgbbtj_bt1c_setule32(ct+20, ca2);
		csm=bt1c_blkmap2;
	}
	ct[12]=csm[(pix>>24)&255]; ct[13]=csm[(pix>>16)&255];
	ct[14]=csm[(pix>> 8)&255]; ct[15]=csm[(pix    )&255];
}

void BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic(
	byte *ct, u16 clra, u16 clrb, u32 pix, int dfl)
{
	ct[0]=255; ct[1]=255;
	BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasicAX(
		ct, clra, clrb, pix, dfl);
}

void BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasicAlpha(
	byte *ct, u16 clra, u16 clrb, u32 pix, int dfl)
{
	ct[0]=255; ct[1]=255;
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
}

void BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasic_RGB24(
	byte *ct, u32 clra, u32 clrb, u32 pix, int dfl)
{
	ct[0]=255; ct[1]=255;
	BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasicAX_RGB24(
		ct, clra, clrb, pix, dfl);
}

void BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasicAlpha_RGB24(
	byte *ct, u32 clra, u32 clrb, u32 pix, int dfl)
{
	ct[0]=255; ct[1]=255;
	BGBBTJ_BT1C_DecodeBlocksMB5C_EncBlockBasicAX_RGB24(
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
		j=BGBBTJ_BT1C_DecodeSingleColor565(ctx, &cs, mode);
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

//	diffm=0; diffsc=0;
//	ctx->diff_m=0; ctx->diff_sc=0;
	diffm=ctx->diff_m;
	diffsc=ctx->diff_sc;

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
			case 0xE0:
				if(mode!=ctx->modes[0])
					{ mode=ctx->modes[0]; nextmode=mode; }
				else
					{ ret=1; }
//				if(mode)
//					{ mode=0; nextmode=0; }
//				else
//					{ ret=1; }
				break;
			case 0xE1:
				mode=ctx->modes[1];
				nextmode=mode;
//				cs+=3;
				break;
			case 0xE2:
				mode=ctx->modes[2];
				nextmode=mode;
//				if((cs[0]>='A') && (cs[0]<='Z'))
//					{ ret=-1; }
//				j=(cs[1]<<8)|cs[2];
//				cs+=j-1;
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

				if((ct+j*32)>cte) { ret=-1; break; }
				if((ct-k*32)<blks) { ret=-1; break; }

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
					break;
				case 1:
					k=((j>>8)&31)+1;
					yo=((j>>4)&15)-8;
					xo=((j   )&15)-8;
					break;
				case 2:
					k=(j&8191)+1;
					yo=cs[0]-128;
					xo=cs[1]-128;
					cs+=2;
					break;
				default:
					xo=0; yo=0; k=0;
					break;
				}

//				if((ct+j*32)>cte) { ret=-1; break; }
//				if((ct-k*32)<blks) { ret=-1; break; }
				l=yo*xs1+xo;

				while(k--)
				{
					bgbbtj_bt1c_memcpy32(ct, csl+l*32);
					ct+=32; csl+=32; csa+=8;
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
						{ nextmode=l; mode=l; }
					break;
				case 5:
					k=j&31;
					l=(cs[0]<<8)|(cs[1]); cs+=2;
					ctx->optcntrl[k]=l;
					if(k==BGBBTJ_BTIC1C_CTRL_BLOCKMODE)
						{ nextmode=l; mode=l; }
					if(k==BGBBTJ_BTIC1C_CTRL_SCALEHDR)
						ctx->dflags|=BGBBTJ_BTIC1C_DFL_SCALEHDR2LDR;
					break;
				case 6:
					k=j&31;
					if(k<24)
					{
						nextmode=mode;
						mode=k;
						break;
					}
					if(k<28)
					{
						k=((k&3)<<8)|(*cs++);
						nextmode=mode;
						mode=k;
						break;
					}
					k=(cs[0]<<8)|(cs[1]); cs+=2;
					nextmode=mode;
					mode=k;
					break;
					
				default: break;
				}
				break;

			case 0xF0:
				switch(cs[0])
				{
				case 0xF0: cs++; mode=ctx->modes[0]; nextmode=mode; break;
				case 0xF1: cs++; mode=ctx->modes[1]; nextmode=mode; break;
				case 0xF2: cs++; mode=ctx->modes[2]; nextmode=mode; break;
				case 0xF3: cs++; mode=ctx->modes[3]; nextmode=mode; break;
				case 0xF4: cs++; mode=ctx->modes[4]; nextmode=mode; break;
				case 0xF5: cs++; mode=ctx->modes[5]; nextmode=mode; break;
				case 0xF6: cs++; mode=ctx->modes[6]; nextmode=mode; break;
				case 0xF7: cs++; mode=ctx->modes[7]; nextmode=mode; break;
				case 0xEF:
					if((cs[1]>=0xC0) && (cs[1]<0xD8))
					{
						k=cs[1]&31; cs+=2;
						nextmode=k; mode=mode;
						break;
					}
					if((cs[1]>=0xD8) && (cs[1]<0xDC))
					{
						k=((cs[1]&3)<<8)|cs[2]; cs+=3;
						nextmode=k; mode=mode;
						break;
					}
					nextmode=mode; mode=ctx->modes[0]; break;
				default: nextmode=mode; mode=ctx->modes[0]; break;
				}
				break;
			case 0xF1: nextmode=mode; mode=ctx->modes[1]; break;
			case 0xF2: nextmode=mode; mode=ctx->modes[2]; break;
			case 0xF3: nextmode=mode; mode=ctx->modes[3]; break;
			case 0xF4: nextmode=mode; mode=ctx->modes[4]; break;
			case 0xF5: nextmode=mode; mode=ctx->modes[5]; break;
			case 0xF6: nextmode=mode; mode=ctx->modes[6]; break;
			case 0xF7: nextmode=mode; mode=ctx->modes[7]; break;
			
			case 0xF8:
			case 0xF9:
			case 0xFA:
			case 0xFB:
				j=((i-0xF8)<<8)|(*cs++);
//				j=*cs++;
				switch(j)
				{
				case 0x81:
					for(j=0; j<256; j++)
						{ ctx->pal256[j]=bt1c_blkidxcolor[j]; }
					for(j=0; j<16; j++)
						{ ctx->pal16[j]=bt1c_blkidxcolor[j]; }
					for(j=0; j<256; j++)
						{ ctx->pat256[j]=bt1c_blkidx_pixpat[j]; }
					for(j=0; j<8; j++)
						{ ctx->modes[j]=j; }
					for(j=0; j<8; j++)
						{ ctx->amodes[j]=j; }
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
					}
					break;
				case 0x83:
					j=(cs[0]>>4)&15;
					k=(cs[0]&15)+1;
					for(; j<k; j++)
						{ ctx->pal16[j]=ctx->pal256[*cs++]; }
					break;
				case 0x84:
					j=(cs[0]>>4)&15;
					k=(cs[0]&15)+1;
					for(; j<k; j++)
					{
						l=(cs[0]<<8)|cs[1]; cs+=2;
						l=BGBBTJ_BT1C_ConvRGB15ToRGB24(ctx, l);
						ctx->pal16[j]=l;
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
//						l=	((l&0x3E0000)>>7)|
//							((l&0x007C00)>>5)|
//							((l&0x00007C)>>2);
						l=BGBBTJ_BT1C_ConvRGB23ToRGB24(ctx, l);
						ctx->pal256[j]=l;
					}
					break;
				case 0x87:
					j=*cs++;
					k=(*cs++)+1;
					for(; j<k; j++)
					{
						l=(cs[0]<<24)|(cs[1]<<16)|(cs[2]<<8)|cs[3];
						cs+=4;
//						l=	((l&0x3E000000)>>15)|
//							((l&0x007C0000)>>13)|
//							((l&0x00007C00)>>10);
//						l=BGBBTJ_BT1C_ConvRGB31FUToRGB24(ctx, l);
						l=(l>>8);
						ctx->pal256[j]=l;
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
					}
					break;

				case 0x8A:
					j=(cs[0]<<8)|cs[1]; cs+=2;
					ctx->modes[(j>>12)&7]=j&4095;
					break;

				case 0x8F:
					j=*cs++;
					diffm=j;
					ctx->diff_m=diffm;
					break;
				case 0x90: case 0x91:
				case 0x92: case 0x93:
				case 0x94: case 0x95:
				case 0x96: case 0x97:
				case 0x98: case 0x99:
				case 0x9A: case 0x9B:
				case 0x9C: case 0x9D:
				case 0x9E: case 0x9F:
					if(!diffm)
						diffm=1;
					diffsc=j-0x90;
					ctx->diff_m=diffm;
					ctx->diff_sc=diffsc;
					break;
				case 0xA0: case 0xA1:
				case 0xA2: case 0xA3:
				case 0xA4: case 0xA5:
				case 0xA6: case 0xA7:
					k=j-0xA0;
					ctx->clrdb_mode=k;
					break;
				default:
					ret=-1;
					break;
				}
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
						k=BGBBTJ_BT1C_PredictDiffColorA(ctx,
							blks, ct, xs1, ys1, 32);
						j=k+j;
					}else
					{
						j=(cs[0]<<24)|(cs[1]<<16)|(cs[2]<<8)|cs[3];
						cs+=4;
						l=j;
						if(mode==8)
							{ j=BGBBTJ_BT1C_ConvRGB31FUToRGB24(ctx, j); }
						else
							{ j=BGBBTJ_BT1C_ConvRGB31FSToRGB24(ctx, j); }
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
						l=BGBBTJ_BT1C_PredictDiffColorA(ctx,
							blks, ct, xs1, ys1, 32);
						j=l+j;
						l=BGBBTJ_BT1C_PredictDiffColorB(ctx,
							blks, ct, xs1, ys1, 32);
						k=l+k;
					}else
					{
						j=(cs[0]<<24)|(cs[1]<<16)|(cs[2]<<8)|cs[3];
						k=(cs[4]<<24)|(cs[5]<<16)|(cs[6]<<8)|cs[7];
						cs+=8;
						l=k;
						if(mode==8)
						{
							j=BGBBTJ_BT1C_ConvRGB31FUToRGB24(ctx, j);
							k=BGBBTJ_BT1C_ConvRGB31FUToRGB24(ctx, k);
						}
						else
						{
							j=BGBBTJ_BT1C_ConvRGB31FSToRGB24(ctx, j);
							k=BGBBTJ_BT1C_ConvRGB31FSToRGB24(ctx, k);
						}
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
						j=BGBBTJ_BT1C_ConvRGB31FUToRGB24(ctx, j);
						k=BGBBTJ_BT1C_ConvRGB31FUToRGB24(ctx, k);
					}
					else
					{
						j=BGBBTJ_BT1C_ConvRGB31FSToRGB24(ctx, j);
						k=BGBBTJ_BT1C_ConvRGB31FSToRGB24(ctx, k);
					}
					ctx->diff_lclra=j;
					ctx->diff_lclrb=k;

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
						j=k+j;
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
						j=l+j;
						l=BGBBTJ_BT1C_PredictDiffColorB24(ctx,
							blks, ct, xs1, ys1, 16);
						k=l+k;
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