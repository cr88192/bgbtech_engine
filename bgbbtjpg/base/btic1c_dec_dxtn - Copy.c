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

extern u16 bt1c_difftab[8*256];		//differential color tables

extern byte bt1c_hf12tob[4096];		//half-float (bits 4-15) to byte

extern int bt1c_basedifftab[8][5];
extern u32 bt1c_blkidx_pixpat[256];	//1BPP -> RPZA Pixels


int BGBBTJ_BT1C_PredictDiffColorA(BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *blks, byte *ct, int xs1, int ys1, int stride)
{
	return(BGBBTJ_BT1C_PredictDiffColorModeA(
		blks, ct, xs1, ys1, stride, ctx->diff_m));
}

int BGBBTJ_BT1C_PredictDiffColorB(BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *blks, byte *ct, int xs1, int ys1, int stride)
{
	return(BGBBTJ_BT1C_PredictDiffColorModeB(
		blks, ct, xs1, ys1, stride, ctx->diff_m));
}

int BGBBTJ_BT1C_PredictDiffColorModeA(
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
	}

	return(p3);
}

int BGBBTJ_BT1C_PredictDiffColorModeB(
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
	}

	return(p3);
}

int BGBBTJ_BT1C_ConvRGB23ToRGB15(u32 clr)
{
	int i;
	i=	((clr&0x3E0000)>>7)|
		((clr&0x007C00)>>5)|
		((clr&0x00007C)>>2);
	return(i);
}

int BGBBTJ_BT1C_ConvRGB31ToRGB15(u32 clr)
{
	int i;
	i=	((clr&0x3E000000)>>15)|
		((clr&0x007C0000)>>13)|
		((clr&0x00007C00)>>10);
	return(i);
}

int BGBBTJ_BT1C_ConvRGB31FUToRGB15(u32 clr)
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

int BGBBTJ_BT1C_ConvRGB31FSToRGB15(u32 clr)
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

void BGBBTJ_BT1C_FillColorDeblock(
	BGBBTJ_BTIC1C_ImageInfo *ctx, 
	byte *blks, byte *ct, byte *ctc, int idx,
	int xs1, int ys1, int stride, int mode)
{
	byte *ct0, *ct1, *ct2;
	int p0, p1, p2, p3, p4, p5;

	if(!mode)
	{
		bgbbtj_bt1c_memcpy8(ct, ctc);
		return;
	}
	
	ct0=ct-xs1*stride-stride;
	ct1=ct-xs1*stride;
	ct2=ct-stride;

//	p0=0; p1=0; p2=0;

	p3=(ctc[0])|((ctc[1])<<8);
	p0=p3; p1=p3; p2=p3;
	
	if(ct0>=blks)
	{
		//1111 0111 1101 1111
		p4=(ct0[0])|((ct0[1])<<8);
		p5=(ct0[2])|((ct0[3])<<8);
		p4=p4&0xF7DF; p5=p5&0xF7DF;
		p0=(p4+p5)>>1;
//		p0=(ct0[2])|((ct0[3])<<8);
//		p0=BGBBTJ_BT1C_ColorCloserAB(p4, p5, p3);
		p0=BGBBTJ_BT1C_ColorCloserABC(p4, p5, p0, p3);
	}
	if(ct1>=blks)
	{
		p4=(ct1[0])|((ct1[1])<<8);
		p5=(ct1[2])|((ct1[3])<<8);
		p4=p4&0xF7DF; p5=p5&0xF7DF;
		p1=(p4+p5)>>1;
//		p1=(ct1[2])|((ct1[3])<<8);
//		p1=BGBBTJ_BT1C_ColorCloserAB(p4, p5, p3);
		p1=BGBBTJ_BT1C_ColorCloserABC(p4, p5, p1, p3);
	}
	if(ct2>=blks)
	{
		p4=(ct2[0])|((ct2[1])<<8);
		p5=(ct2[2])|((ct2[3])<<8);
		p4=p4&0xF7DF; p5=p5&0xF7DF;
		p2=(p4+p5)>>1;
//		p2=(ct2[2])|((ct2[3])<<8);
//		p2=BGBBTJ_BT1C_ColorCloserAB(p4, p5, p3);
		p2=BGBBTJ_BT1C_ColorCloserABC(p4, p5, p2, p3);
	}

//	p3=(ctc[0])|((ctc[1])<<8);

	switch(mode)
	{
	case 0:
		bgbbtj_bt1c_memcpy8(ct, ctc);
		break;
	case 1:
		if(!idx)
		{
			//00 10 11 01 -> 78
			bgbbtj_bt1c_setule16(ct+0, p2);
			bgbbtj_bt1c_setule16(ct+2, p3);
			bgbbtj_bt1c_setule32(ct+4, 0x78787878);
			break;
		}
		bgbbtj_bt1c_memcpy8(ct, ctc);
		break;
	case 2:
		//00 10 00 10 -> 88
		//11 10 11 10 -> BB
		//10 11 10 11 -> DD
		//01 11 01 11 -> EE
		bgbbtj_bt1c_setule16(ct+0, p1);
		bgbbtj_bt1c_setule16(ct+2, p3);
		bgbbtj_bt1c_setule32(ct+4, 0x88BBDDEE);
		break;
	case 3:
		if(!idx)
		{
			//00 00 10 10 -> A0
			//00 10 11 11 -> F8
			//10 10 11 01 -> 7A
			//11 11 01 01 -> 5F
			bgbbtj_bt1c_setule16(ct+0, p0);
			bgbbtj_bt1c_setule16(ct+2, p3);
			bgbbtj_bt1c_setule32(ct+4, 0xA0F87A5F);
			break;
		}
		bgbbtj_bt1c_setule16(ct+0, p1);
		bgbbtj_bt1c_setule16(ct+2, p3);
		bgbbtj_bt1c_setule32(ct+4, 0x88BBDDEE);
		break;
	case 4:
		//00 00 10 10 -> A0
		//00 10 11 11 -> F8
		//10 10 11 01 -> 7A
		//11 11 01 01 -> 5F
		p4=BGBBTJ_BT1C_Paeth(p2, p1, p0);
		bgbbtj_bt1c_setule16(ct+0, p4);
		bgbbtj_bt1c_setule16(ct+2, p3);
		bgbbtj_bt1c_setule32(ct+4, 0xA0F87A5F);
		break;
	case 5:
//		p4=2*p3-p2;
		p4=BGBBTJ_BT1C_LinearExtrapolate565(p2, p3);
		if(!idx)
		{
			//00 10 11 01 -> 78
			bgbbtj_bt1c_setule16(ct+0, p2);
			bgbbtj_bt1c_setule16(ct+2, p4);
			bgbbtj_bt1c_setule32(ct+4, 0x78787878);
			break;
		}
		bgbbtj_bt1c_memcpy8(ct, ctc);
		break;
	case 6:
		//00 10 00 10 -> 88
		//11 10 11 10 -> BB
		//10 11 10 11 -> DD
		//01 11 01 11 -> EE
//		p4=2*p3-p1;
		p4=BGBBTJ_BT1C_LinearExtrapolate565(p1, p3);
		bgbbtj_bt1c_setule16(ct+0, p1);
		bgbbtj_bt1c_setule16(ct+2, p4);
		bgbbtj_bt1c_setule32(ct+4, 0x88BBDDEE);
		break;
	case 7:
		if(!idx)
		{
			//00 00 10 10 -> A0
			//00 10 11 11 -> F8
			//10 10 11 01 -> 7A
			//11 11 01 01 -> 5F
//			p4=2*p3-p0;
			p4=BGBBTJ_BT1C_LinearExtrapolate565(p0, p3);
			bgbbtj_bt1c_setule16(ct+0, p0);
			bgbbtj_bt1c_setule16(ct+2, p4);
			bgbbtj_bt1c_setule32(ct+4, 0xA0F87A5F);
			break;
		}
//		p4=2*p3-p1;
		p4=BGBBTJ_BT1C_LinearExtrapolate565(p1, p3);
		bgbbtj_bt1c_setule16(ct+0, p1);
		bgbbtj_bt1c_setule16(ct+2, p4);
		bgbbtj_bt1c_setule32(ct+4, 0x88BBDDEE);
		break;

	default:
		bgbbtj_bt1c_memcpy8(ct, ctc);
		break;
	}
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
		i=(cs[0]<<8)|cs[1];
		cs+=2;
		break;
	case 1:
	case 3:
		i=ctx->pal256[*cs++];
		break;
	case 2:
		i=ctx->pal16[*cs++];
		break;
	case 6:
		i=(cs[0]<<16)|(cs[1]<<8)|cs[2];
		cs+=3;
		i=	((i&0x3E0000)>>7)|
			((i&0x007C00)>>5)|
			((i&0x00007C)>>2);
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
		i=(cs[0]<<8)|cs[1];
		i=((i&0x7FE0)<<1)|(i&0x1F);
		i|=(i>>5)&0x20;
		cs+=2;
		break;
	case 1:
	case 3:
		i=ctx->pal256[*cs++];
//		i=(cs[0]<<8)|cs[1];
		i=((i&0x7FE0)<<1)|(i&0x1F);
		i|=(i>>5)&0x20;
		break;
	case 2:
		i=ctx->pal16[*cs++];
//		i=(cs[0]<<8)|cs[1];
		i=((i&0x7FE0)<<1)|(i&0x1F);
		i|=(i>>5)&0x20;
		break;
	case 6:
		i=(cs[0]<<16)|(cs[1]<<8)|cs[2];
		cs+=3;
		i=	((i&0x3E0000)>>7)|
			((i&0x007C00)>>5)|
			((i&0x00007C)>>2);
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

void BGBBTJ_BT1C_DecodeBlockColorArrayDXTn(
	BGBBTJ_BTIC1C_ImageInfo *ctx, 
	byte *blks, byte *ct, byte **rcs, int num,
	int xs1, int ys1, int stride, int mode)
{
	byte ctb[16];
	byte *cs;
	int i, j, k;
	
	cs=*rcs;
	for(i=0; i<num; i++)
	{
		j=BGBBTJ_BT1C_DecodeSingleColor565(ctx, &cs, mode);
		bgbbtj_bt1c_setule16(ctb+0, j);
		bgbbtj_bt1c_setule16(ctb+2, j);
		bgbbtj_bt1c_setule32(ctb+4, 0x00000000);
		BGBBTJ_BT1C_FillColorDeblock(ctx,
			blks, ct, ctb, 0, xs1, ys1, stride,
			ctx->clrdb_mode);
		ct+=stride;
	}
	*rcs=cs;
}

BGBBTJ_API int BGBBTJ_BT1C_DecodeBlocksDXTn(
	BGBBTJ_BTIC1C_ImageInfo *ctx, byte *ibuf, int isz,
	byte *blks, byte *lblks,
	int xs, int ys, int stride)
{
	byte tb[16];
	byte *cs, *cse, *csm, *csl;
	byte *ct, *cte, *ctb;
	int diffm, diffsc;
	int xs1, ys1, n, n1, ret, mode, nextmode;
	int xo, yo;
	int i, j, k, l;
	
	BGBBTJ_BT1C_Init();
	
	xs1=(xs+3)/4;
	ys1=(ys+3)/4;
	n=xs1*ys1;
	
	ctb=tb;
	cs=ibuf; cse=cs+isz;
	ct=blks; cte=ct+stride*n;
	csl=lblks;
	if(!csl)csl=blks;
//	cs+=4;

	if(ibuf[0]==0xE1)
	{
		l=(cs[1]<<16)|(cs[2]<<8)|cs[3];
		cs=ibuf+4; cse=ibuf+l;
	}
	
	diffm=0; diffsc=0;
	ctx->diff_m=0; ctx->diff_sc=0;

	ret=0; mode=0; nextmode=0;
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
					bgbbtj_bt1c_memcpy8(ct, csl);
					ct+=stride; csl+=stride;
				}
			}else
			{
				n1=(i&31)+1;
				ct+=n1*stride; csl+=n1*stride;
			}
			break;			
		case 0xE0:
			switch(i)
			{
			case 0xE0:
				if(mode)
					{ mode=0; nextmode=0; }
				else
					{ ret=1; }
				break;
			case 0xE1:
				cs+=3;
				break;
			case 0xE2:
				if((cs[0]>='A') && (cs[0]<='Z'))
					{ ret=-1; }
				j=(cs[1]<<8)|cs[2];
				cs+=j-1;
				break;
			case 0xE3:
			case 0xE4:
				if((cs[3]>='A') && (cs[1]<='Z'))
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

#if 0
			case 0xE8:
				bgbbtj_bt1c_memcpy8(ct, ct-stride);
				ct+=stride; csl+=stride;
				break;
			case 0xE9:
				j=(*cs++)+1;
				while(j--)
				{
					bgbbtj_bt1c_memcpy8(ct, ct-stride);
					ct+=stride; csl+=stride;
				}
				break;
			case 0xEA:
				j=(*cs++)+1;
				if((ct-j*stride)<blks) { ret=-1; break; }
				bgbbtj_bt1c_memcpy8(ct, ct-j*stride);
				ct+=stride; csl+=stride;
				break;
			case 0xEB:
				j=(cs[0]<<8)+cs[1]+1;
				cs+=2;
				if((ct-j*stride)<blks) { ret=-1; break; }
				bgbbtj_bt1c_memcpy8(ct, ct-j*stride);
				ct+=stride; csl+=stride;
				break;
			case 0xEC:
				j=(*cs++)+1;
				k=(*cs++)+1;
				if((ct+j*stride)>cte) { ret=-1; break; }
				if((ct-k*stride)<blks) { ret=-1; break; }
				while(j--)
				{
					bgbbtj_bt1c_memcpy8(ct, ct-k*stride);
					ct+=stride; csl+=stride;
				}
				break;
			case 0xED:
				j=(cs[0])+1;
				k=(cs[1]<<8)+cs[2]+1;
				cs+=3;

				if((ct+j*stride)>cte) { ret=-1; break; }
				if((ct-k*stride)<blks) { ret=-1; break; }

				while(j--)
				{
					bgbbtj_bt1c_memcpy8(ct, ct-k*stride);
					ct+=stride; csl+=stride;
				}
				break;
#endif

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

				if((ct+j*stride)>cte) { ret=-1; break; }
				if((ct-k*stride)<blks) { ret=-1; break; }

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

//				if((ct+j*stride)>cte) { ret=-1; break; }
//				if((ct-k*stride)<blks) { ret=-1; break; }
				l=yo*xs1+xo;

				while(k--)
				{
					bgbbtj_bt1c_memcpy8(ct, csl+l*stride);
					ct+=stride; csl+=stride;
				}
				break;

			case 0xEF:
				j=*cs++;
				switch((j>>5)&7)
				{
				case 0:
					k=j&31;
					ctx->optfl[k>>3]|=(1<<(k&7));
					break;
				case 1:
					k=j&31;
					ctx->optfl[k>>3]&=~(1<<(k&7));
					break;
				case 2:
					k=((j&31)<<8)|(*cs++);
					ctx->optfl[k>>3]|=(1<<(k&7));
					break;
				case 3:
					k=((j&31)<<8)|(*cs++);
					ctx->optfl[k>>3]&=~(1<<(k&7));
					break;
				case 4:
					k=j&31;
					l=*cs++;
					ctx->optcntrl[k]=l;
					break;
				case 5:
					k=j&31;
					l=(cs[0]<<8)|(cs[1]); cs+=2;
					ctx->optcntrl[k]=l;
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
				case 0xF0: cs++; mode=0; nextmode=0; break;
//				case 0xF1: cs++; mode=1; nextmode=1; break;
//				case 0xF2: cs++; mode=2; nextmode=2; break;
//				case 0xF3: cs++; mode=3; nextmode=3; break;
//				case 0xF4: cs++; mode=4; nextmode=4; break;
//				case 0xF5: cs++; mode=5; nextmode=5; break;
//				case 0xF6: cs++; mode=6; nextmode=6; break;
//				case 0xF7: cs++; mode=7; nextmode=7; break;
				case 0xF1: cs++; mode=ctx->modes[1]; nextmode=mode; break;
				case 0xF2: cs++; mode=ctx->modes[2]; nextmode=mode; break;
				case 0xF3: cs++; mode=ctx->modes[3]; nextmode=mode; break;
				case 0xF4: cs++; mode=ctx->modes[4]; nextmode=mode; break;
				case 0xF5: cs++; mode=ctx->modes[5]; nextmode=mode; break;
				case 0xF6: cs++; mode=ctx->modes[6]; nextmode=mode; break;
				case 0xF7: cs++; mode=ctx->modes[7]; nextmode=mode; break;
				default: nextmode=mode; mode=0; break;
				}
				break;
//			case 0xF1: nextmode=mode; mode=1; break;
//			case 0xF2: nextmode=mode; mode=2; break;
//			case 0xF3: nextmode=mode; mode=3; break;
//			case 0xF4: nextmode=mode; mode=4; break;
//			case 0xF5: nextmode=mode; mode=5; break;
//			case 0xF6: nextmode=mode; mode=6; break;
//			case 0xF7: nextmode=mode; mode=7; break;

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
//				j=*cs++;
				j=((i-0xF8)<<8)|(*cs++);
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
					break;
				case 0x82:
					j=*cs++;
					k=(*cs++)+1;
					for(; j<k; j++)
					{
						l=(cs[0]<<8)|cs[1];
						cs+=2;
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
						l=	((l&0x3E0000)>>7)|
							((l&0x007C00)>>5)|
							((l&0x00007C)>>2);
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
						l=	((l&0x3E000000)>>15)|
							((l&0x007C0000)>>13)|
							((l&0x00007C00)>>10);
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
						l=BGBBTJ_BT1C_ConvRGB31FSToRGB15(l);
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
						l=BGBBTJ_BT1C_ConvRGB31FUToRGB15(l);
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
					BGBBTJ_BT1C_DecodeBlockColorArrayDXTn(ctx,
						blks, ct, &cs, n1,
						xs1, ys1, stride, mode);
					ct+=n1*stride; csl+=n1*stride;
					break;
				}
				break;
			default:
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
							blks, ct, xs1, ys1, stride);
						j=k+j;
						l=0;

#if 0
						bgbbtj_bt1c_setule16(ctb+0, j);
						bgbbtj_bt1c_setule16(ctb+2, j);
						bgbbtj_bt1c_setule32(ctb+4, 0x00000000);

						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy8(ct, ctb);
							ct+=stride; csl+=stride;
						}
						break;
#endif
					}else
					{
						j=(cs[0]<<8)|cs[1];
						cs+=2;
						l=j;
						j=((j&0x7FE0)<<1)|(j&0x1F);
						j|=(j>>5)&0x20;
					}

					if(l&0x8000)
					{
//						ctb[0]=j&0xFF;	ctb[1]=(j>>8)&0xFF;
//						ctb[2]=j&0xFF;	ctb[3]=(j>>8)&0xFF;
//						ctb[4]=0xFF; ctb[5]=0xFF;
//						ctb[6]=0xFF; ctb[7]=0xFF;
						bgbbtj_bt1c_setule16(ctb+0, j);
						bgbbtj_bt1c_setule16(ctb+2, j);
						bgbbtj_bt1c_setule32(ctb+4, 0xFFFFFFFFU);
					}else
					{
//						ctb[0]=j&0xFF;	ctb[1]=(j>>8)&0xFF;
//						ctb[2]=j&0xFF;	ctb[3]=(j>>8)&0xFF;
//						ctb[4]=0; ctb[5]=0;
//						ctb[6]=0; ctb[7]=0;
						bgbbtj_bt1c_setule16(ctb+0, j);
						bgbbtj_bt1c_setule16(ctb+2, j);
						bgbbtj_bt1c_setule32(ctb+4, 0x00000000);
					}

					if(ctx->clrdb_mode)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							BGBBTJ_BT1C_FillColorDeblock(ctx,
								blks, ct, ctb, i, xs1, ys1, stride,
								ctx->clrdb_mode);
//							bgbbtj_bt1c_memcpy8(ct, ctb);
							ct+=stride; csl+=stride;
						}
					}else
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy8(ct, ctb);
							ct+=stride; csl+=stride;
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
							blks, ct, xs1, ys1, stride);
						j=l+j;
						l=BGBBTJ_BT1C_PredictDiffColorB(ctx,
							blks, ct, xs1, ys1, stride);
						k=l+k;

						bgbbtj_bt1c_setule16(ctb+0, j);
						bgbbtj_bt1c_setule16(ctb+2, k);
						csm=bt1c_blkmap1;

						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							ctb[4]=csm[cs[0]]; ctb[5]=csm[cs[1]];
							ctb[6]=csm[cs[2]]; ctb[7]=csm[cs[3]];
							cs+=4;
							bgbbtj_bt1c_memcpy8(ct, ctb);
							ct+=stride; csl+=stride;
						}
						break;
					}
					
					j=(cs[0]<<8)|cs[1];
					k=(cs[2]<<8)|cs[3];
					cs+=4;
					l=k;
					j=((j&0x7FE0)<<1)|(j&0x1F);
					k=((k&0x7FE0)<<1)|(k&0x1F);
					j|=(j>>5)&0x20;
					k|=(k>>5)&0x20;

					if(l&0x8000)
					{
						if(j<=k)
						{
							bgbbtj_bt1c_setule16(ctb+0, j);
							bgbbtj_bt1c_setule16(ctb+2, k);
							csm=bt1c_blkmap1;
						}else
						{
							bgbbtj_bt1c_setule16(ctb+0, k);
							bgbbtj_bt1c_setule16(ctb+2, j);
							csm=bt1c_blkmap2;
						}
					}else
					{
						if(j>k)
						{
							bgbbtj_bt1c_setule16(ctb+0, j);
							bgbbtj_bt1c_setule16(ctb+2, k);
							csm=bt1c_blkmap1;
						}else
						{
							bgbbtj_bt1c_setule16(ctb+0, k);
							bgbbtj_bt1c_setule16(ctb+2, j);
							csm=bt1c_blkmap2;
						}
					}

					n1=(i&31)+1;
					for(i=0; i<n1; i++)
					{
						ctb[4]=csm[cs[0]]; ctb[5]=csm[cs[1]];
						ctb[6]=csm[cs[2]]; ctb[7]=csm[cs[3]];
						cs+=4;
						bgbbtj_bt1c_memcpy8(ct, ctb);
						ct+=stride; csl+=stride;
					}
					break;

				default:
					if(cs[1]&0x80)
					{
						cs--;
						j=(cs[0]<<8)|cs[1];
						k=(cs[2]<<8)|cs[3];
						cs+=4;
						j=((j&0x7FE0)<<1)|(j&0x1F);
						k=((k&0x7FE0)<<1)|(k&0x1F);
						j|=(j>>5)&0x20;
						k|=(k>>5)&0x20;

						if(j>k)
						{
//							ctb[0]=j&0xFF;	ctb[1]=(j>>8)&0xFF;
//							ctb[2]=k&0xFF;	ctb[3]=(k>>8)&0xFF;
							bgbbtj_bt1c_setule16(ctb+0, j);
							bgbbtj_bt1c_setule16(ctb+2, k);
							csm=bt1c_blkmap1;
						}else
						{
//							ctb[0]=k&0xFF;	ctb[1]=(k>>8)&0xFF;
//							ctb[2]=j&0xFF;	ctb[3]=(j>>8)&0xFF;
							bgbbtj_bt1c_setule16(ctb+0, k);
							bgbbtj_bt1c_setule16(ctb+2, j);
							csm=bt1c_blkmap2;
						}

						ctb[4]=csm[cs[0]]; ctb[5]=csm[cs[1]];
						ctb[6]=csm[cs[2]]; ctb[7]=csm[cs[3]];
						cs+=4;
						bgbbtj_bt1c_memcpy8(ct, ctb);
						ct+=stride; csl+=stride;
					}else
					{
						memset(ctb, 0, 8);
						//dummy...
						cs+=31;
						bgbbtj_bt1c_memcpy8(ct, ctb);
						ct+=stride; csl+=stride;
					}
					break;
				}
				break;
			case 1:
				switch(i&0xE0)
				{
				case 0xA0:
					j=ctx->pal256[*cs++];
					j=((j&0x7FE0)<<1)|(j&0x1F);
					j|=(j>>5)&0x20;

					ctb[0]=j&0xFF;	ctb[1]=(j>>8)&0xFF;
					ctb[2]=j&0xFF;	ctb[3]=(j>>8)&0xFF;
					ctb[4]=0; ctb[5]=0;
					ctb[6]=0; ctb[7]=0;

					if(ctx->clrdb_mode)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							BGBBTJ_BT1C_FillColorDeblock(ctx,
								blks, ct, ctb, i, xs1, ys1, stride,
								ctx->clrdb_mode);
//							bgbbtj_bt1c_memcpy8(ct, ctb);
							ct+=stride; csl+=stride;
						}
					}else
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy8(ct, ctb);
							ct+=stride; csl+=stride;
						}
					}
					break;
				case 0xC0:
					j=ctx->pal256[cs[0]];
					k=ctx->pal256[cs[1]];
					cs+=2;
					j=((j&0x7FE0)<<1)|(j&0x1F);
					k=((k&0x7FE0)<<1)|(k&0x1F);
					j|=(j>>5)&0x20;
					k|=(k>>5)&0x20;

					ctb[0]=j&0xFF;	ctb[1]=(j>>8)&0xFF;
					ctb[2]=k&0xFF;	ctb[3]=(k>>8)&0xFF;
					csm=bt1c_blkmap1;

					n1=(i&31)+1;
					for(i=0; i<n1; i++)
					{
						j=bt1c_blkidxmap1[cs[0]];
						k=bt1c_blkidxmap1[cs[1]];
						cs+=2;

						ctb[4]=csm[(j>>8)&255]; ctb[5]=csm[j&255];
						ctb[6]=csm[(k>>8)&255]; ctb[7]=csm[k&255];
						bgbbtj_bt1c_memcpy8(ct, ctb);
						ct+=stride; csl+=stride;
					}
					break;
				default:
					cs--;
					j=ctx->pal256[cs[0]];
					k=ctx->pal256[cs[1]];
					cs+=2;
					j=((j&0x7FE0)<<1)|(j&0x1F);
					k=((k&0x7FE0)<<1)|(k&0x1F);
					j|=(j>>5)&0x20;
					k|=(k>>5)&0x20;

					ctb[0]=j&0xFF;	ctb[1]=(j>>8)&0xFF;
					ctb[2]=k&0xFF;	ctb[3]=(k>>8)&0xFF;
					csm=bt1c_blkmap1;

					j=bt1c_blkidxmap1[cs[0]];
					k=bt1c_blkidxmap1[cs[1]];
					cs+=2;

					ctb[4]=csm[(j>>8)&255]; ctb[5]=csm[j&255];
					ctb[6]=csm[(k>>8)&255]; ctb[7]=csm[k&255];
					bgbbtj_bt1c_memcpy8(ct, ctb);
					ct+=stride; csl+=stride;

					break;
				}
				break;
			case 2:
				switch(i&0xE0)
				{
				case 0xA0:
					j=ctx->pal16[(*cs++)&15];
					j=((j&0x7FE0)<<1)|(j&0x1F);
					j|=(j>>5)&0x20;

					ctb[0]=j&0xFF;	ctb[1]=(j>>8)&0xFF;
					ctb[2]=j&0xFF;	ctb[3]=(j>>8)&0xFF;
					ctb[4]=0; ctb[5]=0;
					ctb[6]=0; ctb[7]=0;

					if(ctx->clrdb_mode)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							BGBBTJ_BT1C_FillColorDeblock(ctx,
								blks, ct, ctb, i, xs1, ys1, stride,
								ctx->clrdb_mode);
//							bgbbtj_bt1c_memcpy8(ct, ctb);
							ct+=stride; csl+=stride;
						}
					}else
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy8(ct, ctb);
							ct+=stride; csl+=stride;
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

					ctb[0]=j&0xFF;	ctb[1]=(j>>8)&0xFF;
					ctb[2]=k&0xFF;	ctb[3]=(k>>8)&0xFF;
					csm=bt1c_blkmap1;

					n1=(i&31)+1;
					for(i=0; i<n1; i++)
					{
						j=ctx->pat256[*cs++];
						ctb[4]=csm[(j>>24)&255]; ctb[5]=csm[(j>>16)&255];
						ctb[6]=csm[(j>> 8)&255]; ctb[7]=csm[(j    )&255];
						bgbbtj_bt1c_memcpy8(ct, ctb);
						ct+=stride; csl+=stride;
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

					ctb[0]=j&0xFF;	ctb[1]=(j>>8)&0xFF;
					ctb[2]=k&0xFF;	ctb[3]=(k>>8)&0xFF;
					csm=bt1c_blkmap1;

					j=ctx->pat256[*cs++];
					ctb[4]=csm[(j>>24)&255]; ctb[5]=csm[(j>>16)&255];
					ctb[6]=csm[(j>> 8)&255]; ctb[7]=csm[(j    )&255];
					bgbbtj_bt1c_memcpy8(ct, ctb);
					ct+=stride; csl+=stride;

					break;
				}
				break;
			case 3:
				switch(i&0xE0)
				{
				case 0xA0:
					j=ctx->pal256[*cs++];
					j=((j&0x7FE0)<<1)|(j&0x1F);
					j|=(j>>5)&0x20;

					ctb[0]=j&0xFF;	ctb[1]=(j>>8)&0xFF;
					ctb[2]=j&0xFF;	ctb[3]=(j>>8)&0xFF;
					ctb[4]=0; ctb[5]=0;
					ctb[6]=0; ctb[7]=0;

					if(ctx->clrdb_mode)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							BGBBTJ_BT1C_FillColorDeblock(ctx,
								blks, ct, ctb, i, xs1, ys1, stride,
								ctx->clrdb_mode);
//							bgbbtj_bt1c_memcpy8(ct, ctb);
							ct+=stride; csl+=stride;
						}
					}else
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy8(ct, ctb);
							ct+=stride; csl+=stride;
						}
					}
					break;
				case 0xC0:
					j=ctx->pal256[cs[0]];
					k=ctx->pal256[cs[1]];
					cs+=2;
					j=((j&0x7FE0)<<1)|(j&0x1F);
					k=((k&0x7FE0)<<1)|(k&0x1F);
					j|=(j>>5)&0x20;
					k|=(k>>5)&0x20;

					ctb[0]=j&0xFF;	ctb[1]=(j>>8)&0xFF;
					ctb[2]=k&0xFF;	ctb[3]=(k>>8)&0xFF;
					csm=bt1c_blkmap1;

					n1=(i&31)+1;
					for(i=0; i<n1; i++)
					{
						j=ctx->pat256[*cs++];
						ctb[4]=csm[(j>>24)&255]; ctb[5]=csm[(j>>16)&255];
						ctb[6]=csm[(j>> 8)&255]; ctb[7]=csm[(j    )&255];
						bgbbtj_bt1c_memcpy8(ct, ctb);
						ct+=stride; csl+=stride;
					}
					break;
				default:
					cs--;
					j=ctx->pal256[cs[0]];
					k=ctx->pal256[cs[1]];
					cs+=2;
					j=((j&0x7FE0)<<1)|(j&0x1F);
					k=((k&0x7FE0)<<1)|(k&0x1F);
					j|=(j>>5)&0x20;
					k|=(k>>5)&0x20;

					ctb[0]=j&0xFF;	ctb[1]=(j>>8)&0xFF;
					ctb[2]=k&0xFF;	ctb[3]=(k>>8)&0xFF;
					csm=bt1c_blkmap1;

					j=ctx->pat256[*cs++];
					ctb[4]=csm[(j>>24)&255]; ctb[5]=csm[(j>>16)&255];
					ctb[6]=csm[(j>> 8)&255]; ctb[7]=csm[(j    )&255];
					bgbbtj_bt1c_memcpy8(ct, ctb);
					ct+=stride; csl+=stride;

					break;
				}
				break;

#if 1
			case 4:
			case 5:
				switch(i&0xE0)
				{
				case 0xA0:
					if(cs[0]&0x80)
					{
						j=*cs++;
						j=bt1c_difftab[(diffsc*256)+(j&127)];
						k=BGBBTJ_BT1C_PredictDiffColorA(ctx,
							blks, ct, xs1, ys1, stride);
						j=k+j;
					}else
					{
						j=(cs[0]<<8)|cs[1]; cs+=2;
						j=((j&0x7FE0)<<1)|(j&0x1F);
						j|=(j>>5)&0x20;
					}
					
					bgbbtj_bt1c_setule16(ctb+0, j);
					bgbbtj_bt1c_setule16(ctb+2, j);
					bgbbtj_bt1c_setule32(ctb+4, 0x00000000);

					if(ctx->clrdb_mode)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							BGBBTJ_BT1C_FillColorDeblock(ctx,
								blks, ct, ctb, i, xs1, ys1, stride,
								ctx->clrdb_mode);
//							bgbbtj_bt1c_memcpy8(ct, ctb);
							ct+=stride; csl+=stride;
						}
					}else
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy8(ct, ctb);
							ct+=stride; csl+=stride;
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
							blks, ct, xs1, ys1, stride);
						j=l+j;
						l=BGBBTJ_BT1C_PredictDiffColorB(ctx,
							blks, ct, xs1, ys1, stride);
						k=l+k;
					}else
					{
						j=(cs[0]<<8)|cs[1]; cs+=2;
						k=(cs[0]<<8)|cs[1]; cs+=2;
						j=((j&0x7FE0)<<1)|(j&0x1F);
						k=((k&0x7FE0)<<1)|(k&0x1F);
						j|=(j>>5)&0x20;
						k|=(k>>5)&0x20;
					}

					bgbbtj_bt1c_setule16(ctb+0, j);
					bgbbtj_bt1c_setule16(ctb+2, k);
					csm=bt1c_blkmap1;

					if(mode==4)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							j=bt1c_blkidxmap1[cs[0]];
							k=bt1c_blkidxmap1[cs[1]];
							cs+=2;
							ctb[4]=csm[(j>>8)&255]; ctb[5]=csm[j&255];
							ctb[6]=csm[(k>>8)&255]; ctb[7]=csm[k&255];
							bgbbtj_bt1c_memcpy8(ct, ctb);
							ct+=stride; csl+=stride;
						}
					}else if(mode==5)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							j=bt1c_blkidxmap2[*cs++];
							ctb[4]=csm[(j>>24)&255];
							ctb[5]=csm[(j>>16)&255];
							ctb[6]=csm[(j>> 8)&255];
							ctb[7]=csm[j&255];
							bgbbtj_bt1c_memcpy8(ct, ctb);
							ct+=stride; csl+=stride;
						}
					}
					break;
				default:
					cs--;
//					if(cs[2]&0x80)
//					if(diffm)
					if(0)
					{
						j=*cs++; k=*cs++;
						j=bt1c_difftab[(diffsc*256)+(j&127)];
						k=bt1c_difftab[(diffsc*256)+(k&127)];
						l=BGBBTJ_BT1C_PredictDiffColorA(ctx,
							blks, ct, xs1, ys1, stride);
						j=l+j;
						l=BGBBTJ_BT1C_PredictDiffColorB(ctx,
							blks, ct, xs1, ys1, stride);
						k=l+k;
					}else
					{
						j=(cs[0]<<8)|cs[1]; cs+=2;
						k=(cs[0]<<8)|cs[1]; cs+=2;
						j=((j&0x7FE0)<<1)|(j&0x1F);
						k=((k&0x7FE0)<<1)|(k&0x1F);
						j|=(j>>5)&0x20;
						k|=(k>>5)&0x20;
					}

					bgbbtj_bt1c_setule16(ctb+0, j);
					bgbbtj_bt1c_setule16(ctb+2, k);
					csm=bt1c_blkmap1;

					if(mode==4)
					{
						j=bt1c_blkidxmap1[cs[0]];
						k=bt1c_blkidxmap1[cs[1]];
						cs+=2;
						ctb[4]=csm[(j>>8)&255]; ctb[5]=csm[j&255];
						ctb[6]=csm[(k>>8)&255]; ctb[7]=csm[k&255];
						bgbbtj_bt1c_memcpy8(ct, ctb);
						ct+=stride; csl+=stride;
					}else if(mode==5)
					{
						j=bt1c_blkidxmap2[*cs++];
						ctb[4]=csm[(j>>24)&255];
						ctb[5]=csm[(j>>16)&255];
						ctb[6]=csm[(j>> 8)&255];
						ctb[7]=csm[(j    )&255];
						bgbbtj_bt1c_memcpy8(ct, ctb);
						ct+=stride; csl+=stride;
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

//						j=*cs++;
//						j=bt1c_difftab[(diffsc*256)+(j&127)];
						k=BGBBTJ_BT1C_PredictDiffColorA(ctx,
							blks, ct, xs1, ys1, stride);
						j=k+j;
					}else
					{
						j=(cs[0]<<16)|(cs[1]<<8)|cs[2];
						cs+=3;
						l=j;
						j=BGBBTJ_BT1C_ConvRGB23ToRGB15(j);
						j=((j&0x7FE0)<<1)|(j&0x1F);
						j|=(j>>5)&0x20;
					}

					bgbbtj_bt1c_setule16(ctb+0, j);
					bgbbtj_bt1c_setule16(ctb+2, j);
					bgbbtj_bt1c_setule32(ctb+4, 0x00000000);

					if(ctx->clrdb_mode)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							BGBBTJ_BT1C_FillColorDeblock(ctx,
								blks, ct, ctb, i, xs1, ys1, stride,
								ctx->clrdb_mode);
//							bgbbtj_bt1c_memcpy8(ct, ctb);
							ct+=stride; csl+=stride;
						}
					}else
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy8(ct, ctb);
							ct+=stride; csl+=stride;
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
						l=BGBBTJ_BT1C_PredictDiffColorA(ctx,
							blks, ct, xs1, ys1, stride);
						j=l+j;
						l=BGBBTJ_BT1C_PredictDiffColorB(ctx,
							blks, ct, xs1, ys1, stride);
						k=l+k;
					}else
					{
						j=(cs[0]<<16)|(cs[1]<<8)|cs[2];
						k=(cs[3]<<16)|(cs[4]<<8)|cs[5];
						cs+=6;
						l=k;
						j=BGBBTJ_BT1C_ConvRGB23ToRGB15(j);
						k=BGBBTJ_BT1C_ConvRGB23ToRGB15(k);
						j=((j&0x7FE0)<<1)|(j&0x1F);
						k=((k&0x7FE0)<<1)|(k&0x1F);
						j|=(j>>5)&0x20;
						k|=(k>>5)&0x20;
					}

					if(j>k)
					{
						bgbbtj_bt1c_setule16(ctb+0, j);
						bgbbtj_bt1c_setule16(ctb+2, k);
						csm=bt1c_blkmap1;
					}else
					{
						bgbbtj_bt1c_setule16(ctb+0, k);
						bgbbtj_bt1c_setule16(ctb+2, j);
						csm=bt1c_blkmap2;
					}

					n1=(i&31)+1;
					for(i=0; i<n1; i++)
					{
						ctb[4]=csm[cs[0]]; ctb[5]=csm[cs[1]];
						ctb[6]=csm[cs[2]]; ctb[7]=csm[cs[3]];
						cs+=4;
						bgbbtj_bt1c_memcpy8(ct, ctb);
						ct+=stride; csl+=stride;
					}
					break;

				default:
					cs--;
					j=(cs[0]<<16)|(cs[1]<<8)|cs[2];
					k=(cs[3]<<16)|(cs[4]<<8)|cs[5];
					cs+=6;
					j=BGBBTJ_BT1C_ConvRGB23ToRGB15(j);
					k=BGBBTJ_BT1C_ConvRGB23ToRGB15(k);
					j=((j&0x7FE0)<<1)|(j&0x1F);
					k=((k&0x7FE0)<<1)|(k&0x1F);
					j|=(j>>5)&0x20;
					k|=(k>>5)&0x20;

					if(j>k)
					{
						bgbbtj_bt1c_setule16(ctb+0, j);
						bgbbtj_bt1c_setule16(ctb+2, k);
						csm=bt1c_blkmap1;
					}else
					{
						bgbbtj_bt1c_setule16(ctb+0, k);
						bgbbtj_bt1c_setule16(ctb+2, j);
						csm=bt1c_blkmap2;
					}

					ctb[4]=csm[cs[0]]; ctb[5]=csm[cs[1]];
					ctb[6]=csm[cs[2]]; ctb[7]=csm[cs[3]];
					cs+=4;
					bgbbtj_bt1c_memcpy8(ct, ctb);
					ct+=stride; csl+=stride;
					break;
				}
				break;
#endif

#if 1
			case 7:
				switch(i&0xE0)
				{
				case 0xA0:
					if(diffm && (cs[0]&0x80))
					{
						j=(cs[0]<<8)|cs[1];
						cs+=2;

//						j=*cs++;
//						j=bt1c_difftab[(diffsc*256)+(j&127)];
						k=BGBBTJ_BT1C_PredictDiffColorA(ctx,
							blks, ct, xs1, ys1, stride);
						j=k+j;
					}else
					{
						j=(cs[0]<<24)|(cs[1]<<16)|(cs[2]<<8)|cs[3];
						cs+=4;
						l=j;
						j=BGBBTJ_BT1C_ConvRGB31FSToRGB15(j);
						j=((j&0x7FE0)<<1)|(j&0x1F);
						j|=(j>>5)&0x20;
					}

					bgbbtj_bt1c_setule16(ctb+0, j);
					bgbbtj_bt1c_setule16(ctb+2, j);
					bgbbtj_bt1c_setule32(ctb+4, 0x00000000);

					if(ctx->clrdb_mode)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							BGBBTJ_BT1C_FillColorDeblock(ctx,
								blks, ct, ctb, i, xs1, ys1, stride,
								ctx->clrdb_mode);
//							bgbbtj_bt1c_memcpy8(ct, ctb);
							ct+=stride; csl+=stride;
						}
					}else
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy8(ct, ctb);
							ct+=stride; csl+=stride;
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
						l=BGBBTJ_BT1C_PredictDiffColorA(ctx,
							blks, ct, xs1, ys1, stride);
						j=l+j;
						l=BGBBTJ_BT1C_PredictDiffColorB(ctx,
							blks, ct, xs1, ys1, stride);
						k=l+k;
					}else
					{
						j=(cs[0]<<24)|(cs[1]<<16)|(cs[2]<<8)|cs[3];
						k=(cs[4]<<24)|(cs[5]<<16)|(cs[6]<<8)|cs[7];
						cs+=8;
						l=k;
						j=BGBBTJ_BT1C_ConvRGB31FSToRGB15(j);
						k=BGBBTJ_BT1C_ConvRGB31FSToRGB15(k);
						j=((j&0x7FE0)<<1)|(j&0x1F);
						k=((k&0x7FE0)<<1)|(k&0x1F);
						j|=(j>>5)&0x20;
						k|=(k>>5)&0x20;
					}

					if(j>k)
					{
						bgbbtj_bt1c_setule16(ctb+0, j);
						bgbbtj_bt1c_setule16(ctb+2, k);
						csm=bt1c_blkmap1;
					}else
					{
						bgbbtj_bt1c_setule16(ctb+0, k);
						bgbbtj_bt1c_setule16(ctb+2, j);
						csm=bt1c_blkmap2;
					}

					n1=(i&31)+1;
					for(i=0; i<n1; i++)
					{
						ctb[4]=csm[cs[0]]; ctb[5]=csm[cs[1]];
						ctb[6]=csm[cs[2]]; ctb[7]=csm[cs[3]];
						cs+=4;
						bgbbtj_bt1c_memcpy8(ct, ctb);
						ct+=stride; csl+=stride;
					}
					break;

				default:
					cs--;
					j=(cs[0]<<24)|(cs[1]<<16)|(cs[2]<<8)|cs[3];
					k=(cs[0]<<24)|(cs[1]<<16)|(cs[2]<<8)|cs[3];
					cs+=8;
					j=BGBBTJ_BT1C_ConvRGB31FSToRGB15(j);
					k=BGBBTJ_BT1C_ConvRGB31FSToRGB15(k);
					j=((j&0x7FE0)<<1)|(j&0x1F);
					k=((k&0x7FE0)<<1)|(k&0x1F);
					j|=(j>>5)&0x20;
					k|=(k>>5)&0x20;

					if(j>k)
					{
						bgbbtj_bt1c_setule16(ctb+0, j);
						bgbbtj_bt1c_setule16(ctb+2, k);
						csm=bt1c_blkmap1;
					}else
					{
						bgbbtj_bt1c_setule16(ctb+0, k);
						bgbbtj_bt1c_setule16(ctb+2, j);
						csm=bt1c_blkmap2;
					}

					ctb[4]=csm[cs[0]]; ctb[5]=csm[cs[1]];
					ctb[6]=csm[cs[2]]; ctb[7]=csm[cs[3]];
					cs+=4;
					bgbbtj_bt1c_memcpy8(ct, ctb);
					ct+=stride; csl+=stride;
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


BGBBTJ_API int BGBBTJ_BT1C_DecodeBlocksBC4(BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *ibuf, int isz, byte *blks, byte *lblks,
	int xs, int ys, int stride)
{
	byte tb[16];
	byte *cs, *cse;
	u16 *csm;
	byte *ct, *cte, *ctb;
	int xs1, ys1, n, n1, ret;
	int i, j, k, l;
	
	BGBBTJ_BT1C_Init();
	
	xs1=(xs+3)/4;
	ys1=(ys+3)/4;
	n=xs1*ys1;
	
	ctb=tb;
	cs=ibuf; cse=cs+isz;
	ct=blks; cte=ct+stride*n;
	ret=0;

	if(ibuf[0]==0xE3)
	{
		l=(cs[1]<<16)|(cs[2]<<8)|cs[3];
		cs=ibuf+6; cse=ibuf+l;
	}
	
	while((cs<cse) && (ct<cte) && !ret)
	{
		i=*cs++;
		switch(i&0xE0)
		{
		case 0x80:
			n1=(i&31)+1;
			ct+=n1*stride;
			break;
		case 0xA0:
			j=*cs++;
			l=j;

			if(j&0x80)
				{ j=(j<<8)|(*cs++); }
			else
				{ j=j<<1; j|=(j>>7)&1; }

			ctb[0]=j&0xFF;	ctb[1]=j&0xFF;
			ctb[2]=0x00;	ctb[3]=0x00;
			ctb[4]=0x00;	ctb[5]=0x00;
			ctb[6]=0x00;	ctb[7]=0x00;

			n1=(i&31)+1;
			for(i=0; i<n1; i++)
			{
				bgbbtj_bt1c_memcpy8(ct, ctb);
				ct+=stride;
			}
			break;
		case 0xC0:
			if(cs[0]&0x80)
			{
				j=(cs[0]<<8)|cs[1];
				k=(cs[2]<<8)|cs[3];
				cs+=4;
			}else
			{
				j=cs[0]<<1;
				k=cs[1]<<1;
				j|=(j>>7)&1;
				k|=(k>>7)&1;
				cs+=2;
			}
			l=k;

//			if(j<=k)
			if(k<=j)
			{
				ctb[0]=j&0xFF;	ctb[1]=k&0xFF;
				csm=bt1c_blkalphamap3;
			}else
			{
				ctb[0]=k&0xFF;	ctb[1]=j&0xFF;
				csm=bt1c_blkalphamap4;
			}

			n1=(i&31)+1;
			for(i=0; i<n1; i++)
			{
				j=csm[cs[0]]|(csm[cs[1]]<<12);
				k=csm[cs[2]]|(csm[cs[3]]<<12);
				ctb[2]=j&0xFF; ctb[3]=(j>>8)&0xFF; ctb[4]=(j>>16)&0xFF;
				ctb[5]=k&0xFF; ctb[6]=(k>>8)&0xFF; ctb[7]=(k>>16)&0xFF;
				cs+=4;
				bgbbtj_bt1c_memcpy8(ct, ctb);
				ct+=stride;
			}
			break;
			
		case 0xE0:
			switch(i)
			{
			case 0xE0:
				break;
			case 0xE1:
				cs+=3;
				break;
			case 0xE8:
				bgbbtj_bt1c_memcpy8(ct, ct-stride);
				ct+=stride;
				break;
			case 0xE9:
				j=(*cs++)+1;
				while(j--)
				{
					bgbbtj_bt1c_memcpy8(ct, ct-stride);
					ct+=stride;
				}
				break;
			case 0xEA:
				j=(*cs++)+1;
				if((ct-j*stride)<blks) { ret=-1; break; }
				bgbbtj_bt1c_memcpy8(ct, ct-j*stride);
				ct+=stride;
				break;
			case 0xEB:
				j=(cs[0]<<8)+cs[1]+1;
				cs+=2;
				if((ct-j*stride)<blks) { ret=-1; break; }
				bgbbtj_bt1c_memcpy8(ct, ct-j*stride);
				ct+=stride;
				break;
			case 0xEC:
				j=(*cs++)+1;
				k=(*cs++)+1;
				if((ct+j*stride)>cte) { ret=-1; break; }
				if((ct-k*stride)<blks) { ret=-1; break; }
				while(j--)
				{
					bgbbtj_bt1c_memcpy8(ct, ct-k*stride);
					ct+=stride;
				}
				break;
			case 0xED:
				j=(cs[0])+1;
				k=(cs[1]<<8)+cs[2]+1;
				cs+=3;
				if((ct+j*stride)>cte) { ret=-1; break; }
				if((ct-k*stride)<blks) { ret=-1; break; }
				while(j--)
				{
					bgbbtj_bt1c_memcpy8(ct, ct-k*stride);
					ct+=stride;
				}
				break;
			default:
				ret=-1;
				break;
			}
			break;

		default:
			j=i;
			k=*cs++;
			
			j=(j&0x7F)<<1;
			k=(k&0x7F)<<1;
			j|=(j>>7)&1;
			k|=(k>>7)&1;
			
//			j=cs[0]<<1;
//			k=cs[1]<<1;
//			cs+=2;

//			if(j<=k)
			if(k<=j)
			{
				ctb[0]=j&0xFF;	ctb[1]=k&0xFF;
				csm=bt1c_blkalphamap3;
			}else
			{
				ctb[0]=k&0xFF;	ctb[1]=j&0xFF;
				csm=bt1c_blkalphamap4;
			}

			j=csm[cs[0]]|(csm[cs[1]]<<12);
			k=csm[cs[2]]|(csm[cs[3]]<<12);
			ctb[2]=j&0xFF; ctb[3]=(j>>8)&0xFF; ctb[4]=(j>>16)&0xFF;
			ctb[5]=k&0xFF; ctb[6]=(k>>8)&0xFF; ctb[7]=(k>>16)&0xFF;
			cs+=4;
			bgbbtj_bt1c_memcpy8(ct, ctb);
			ct+=stride;
			break;
		}
	}
	
	if(ret)return(ret);
	return(cs-ibuf);
}


BGBBTJ_API int BGBBTJ_BT1C_DecodeBlocksDXT1(BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *ibuf, int isz, byte *blks, byte *lblks, int xs, int ys)
{
	byte *tbuf, *tbuf2;
	int i, j, k, sz1;

	if(ibuf[0]==0xE1)
	{
		i=(ibuf[1]<<16)|(ibuf[2]<<8)|(ibuf[3]<<0);
		if((ibuf[i+0]==0xE3) && (ibuf[i+4]=='A') && (ibuf[i+5]=='X'))
		{
			BGBBTJ_BT1C_DecodeBlocksDXTn(ctx, ibuf, isz,
				blks, lblks, xs, ys, 8);
			j=(ibuf[i+1]<<16)|(ibuf[i+2]<<8)|(ibuf[i+3]<<0);
			k=i+j;
		}else
		{
			k=BGBBTJ_BT1C_DecodeBlocksDXTn(ctx, ibuf, isz,
				blks, lblks, xs, ys, 8);
		}
	}else if((ibuf[0]==0xE3) && (ibuf[4]=='Z') && (ibuf[5]=='I'))
	{
		i=(ibuf[1]<<16)|(ibuf[2]<<8)|(ibuf[3]<<0);
		j=((xs+3)>>2)*((ys+3)>>2)*3*16; k=0;
		
		tbuf=malloc(j);
		BGBBTJ_BT1C_Lock();
//		PDUNZ_DecodeStreamSz(ibuf+8, tbuf, i-8, j, &k, 0);
//		PDUNZ_DecodeStreamSzZl(ibuf+6, tbuf, i-6, j, &k, 0);
		BTLZA_DecodeStreamSzZl(ibuf+6, tbuf, i-6, j, &k, 0);
		BGBBTJ_BT1C_Unlock();
		BGBBTJ_BT1C_DecodeBlocksDXT1(ctx, tbuf, k, blks, lblks, xs, ys);
		free(tbuf);
		k=i;

//		cs=ibuf+i;
	}else if((ibuf[0]==0xE3) && (ibuf[4]=='Z') && (ibuf[5]=='A'))
	{
		i=(ibuf[1]<<16)|(ibuf[2]<<8)|(ibuf[3]<<0);
		j=((xs+3)>>2)*((ys+3)>>2)*3*16; k=0;
		
		tbuf=malloc(j);		tbuf2=malloc(j);
		BGBBTJ_BT1C_Lock();
		sz1=BGBBTJ_BitArith_DecodeData(ibuf+6, i-6, tbuf2, j);
//		PDUNZ_DecodeStreamSzZl(tbuf2, tbuf, sz1, j, &k, 0);
		BTLZA_DecodeStreamSzZl(tbuf2, tbuf, sz1, j, &k, 0);
		BGBBTJ_BT1C_Unlock();
		BGBBTJ_BT1C_DecodeBlocksDXT1(ctx, tbuf, k, blks, lblks, xs, ys);
		free(tbuf);			 free(tbuf2);
		k=i;
//		cs=ibuf+i;
	}else
	{
		return(-1);
	}
	return(k);
}

BGBBTJ_API int BGBBTJ_BT1C_DecodeBlocksDXT5(BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *ibuf, int isz, byte *blks, byte *lblks, int xs, int ys)
{
	byte *cs, *csl;
	byte *tbuf, *tbuf2;
	int xs1, ys1;
	int i, j, k, n, sz1;

	xs1=(xs+3)/4;
	ys1=(ys+3)/4;
	n=xs1*ys1;

	if(ibuf[0]==0xE1)
	{
		i=(ibuf[1]<<16)|(ibuf[2]<<8)|(ibuf[3]<<0);
		if((ibuf[i+0]==0xE3) && (ibuf[i+4]=='A') && (ibuf[i+5]=='X'))
//		if(0)
		{
			BGBBTJ_BT1C_DecodeBlocksDXTn(ctx, ibuf, isz,
				blks+8, lblks?(lblks+8):NULL, xs, ys, 16);
			j=BGBBTJ_BT1C_DecodeBlocksBC4(ctx, ibuf+i, isz-i,
				blks, lblks, xs, ys, 16);
			cs=ibuf+i+j;
		}else
		{
//			i=BGBBTJ_BT1C_DecodeBlocksDXTn(ctx, ibuf, isz,
//				blks+n*8, lblks, xs, ys, 8);
			BGBBTJ_BT1C_DecodeBlocksDXTn(ctx, ibuf, isz,
				blks+8, lblks?(lblks+8):NULL, xs, ys, 16);
//			BGBBTJ_BCn_TranscodeBC1ToBC3(blks+n*8, blks, xs, ys);
			BGBBTJ_BCn_TranscodeBC1ToBC3B(blks, blks, xs, ys);

//			i=BGBBTJ_BT1C_DecodeBlocksDXT5_NoAX(ctx, ibuf, isz, blks, xs, ys);
			cs=ibuf+i;
		}
	}else if((ibuf[0]==0xE3) && (ibuf[4]=='Z') && (ibuf[5]=='I'))
	{
		i=(ibuf[1]<<16)|(ibuf[2]<<8)|(ibuf[3]<<0);
		j=((xs+3)>>2)*((ys+3)>>2)*3*16; k=0;
		
		tbuf=malloc(j);
		BGBBTJ_BT1C_Lock();
//		PDUNZ_DecodeStreamSz(ibuf+8, tbuf, i-8, j, &k, 0);
//		PDUNZ_DecodeStreamSzZl(ibuf+6, tbuf, i-6, j, &k, 0);
		BTLZA_DecodeStreamSzZl(ibuf+6, tbuf, i-6, j, &k, 0);
		BGBBTJ_BT1C_Unlock();
		BGBBTJ_BT1C_DecodeBlocksDXT5(ctx, tbuf, k, blks, lblks, xs, ys);
		free(tbuf);
		k=i;

		cs=ibuf+i;
	}else if((ibuf[0]==0xE3) && (ibuf[4]=='Z') && (ibuf[5]=='A'))
	{
		i=(ibuf[1]<<16)|(ibuf[2]<<8)|(ibuf[3]<<0);
		j=((xs+3)>>2)*((ys+3)>>2)*3*16; k=0;
		
		tbuf=malloc(j);		tbuf2=malloc(j);
		BGBBTJ_BT1C_Lock();
		sz1=BGBBTJ_BitArith_DecodeData(ibuf+6, i-6, tbuf2, j);
//		PDUNZ_DecodeStreamSzZl(tbuf2, tbuf, sz1, j, &k, 0);
		BTLZA_DecodeStreamSzZl(tbuf2, tbuf, sz1, j, &k, 0);
		BGBBTJ_BT1C_Unlock();
		BGBBTJ_BT1C_DecodeBlocksDXT5(ctx, tbuf, k, blks, lblks, xs, ys);
		free(tbuf);			 free(tbuf2);
		k=i;
		cs=ibuf+i;
	}else
	{
		return(-1);
	}
	return(cs-ibuf);
}
