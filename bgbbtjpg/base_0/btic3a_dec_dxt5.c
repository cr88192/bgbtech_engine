#include <bgbbtj.h>

extern const u32 btic2c_dbase[64];
extern const int btic2c_dextra[64];
extern const int btic2c_lbase[64];
extern const int btic2c_lextra[64];

extern const int btic3c_hilbert_idx[16];
extern const int btic3c_hilbert_xy[32];

BGBBTJ_API void BGBBTJ_BTIC3A_DecodeBlocksDXT5_BlockFormat(
	BTIC3A_ImageCtx *ctx, byte *block, int fmt)
{
	static int idx2[4]={0, 2, 3, 1};
	static int idx3[8]={0,0, 2,2, 3,3, 1,1};
	static int idx4[16]={0,0,0,0, 2,2,2,2, 3,3,3,3, 1,1,1,1};

	static int idxb1[16]={
		0x00, 0x40, 0x10, 0x50,
		0x04, 0x44, 0x14, 0x54,
		0x01, 0x41, 0x11, 0x51,
		0x05, 0x45, 0x15, 0x55};

	int cbf, abf, cdf;
	int clra, clrb;
	int dra, dga, dba, daa;
	int drb, dgb, dbb, dab;
	int p0, p1, p2, p3;
	int q0, q1, q2, q3;
	int i, j, k, l;

	if((fmt&0x70)==0x70)
	{
		cbf=(fmt>>12)&15;
		cdf=(fmt>>16)&255;
		switch(fmt&3)
		{
		case 0: abf=3; break;
		case 1: abf=((fmt>>12)&12)|((fmt>>10)&3); break;
		case 2: abf=(fmt>>8)&15; break;
		default: break;
		}
	}else
	{
		cbf=fmt&15;
		abf=3;
		cdf=(fmt>>4)&7;
	}

	BGBBTJ_BTIC3A_DecodeHandleColorDelta(ctx, cdf);

	if(abf==3)
	{
		i=ctx->clrb[3];
		block[0]=i; block[1]=i;
		block[2]=0;	block[3]=0;
		block[4]=0;	block[5]=0;
		block[6]=0;	block[7]=0;
	}else
	{
		switch(abf)
		{
		case 0:
			i=ctx->clra[3];
			block[0]=i; block[1]=i;
			block[2]=0;	block[3]=0;
			block[4]=0;	block[5]=0;
			block[6]=0;	block[7]=0;
			break;
		case 1:
			i=(11*ctx->clra[3]+5*ctx->clrb[3])>>4;
			block[0]=i; block[1]=i;
			block[2]=0;	block[3]=0;
			block[4]=0;	block[5]=0;
			block[6]=0;	block[7]=0;
			break;
		case 2:
			i=(5*ctx->clra[3]+11*ctx->clrb[3])>>4;
			block[0]=i; block[1]=i;
			block[2]=0;	block[3]=0;
			block[4]=0;	block[5]=0;
			block[6]=0;	block[7]=0;
			break;
		case 3:
			i=ctx->clrb[3];
			block[0]=i; block[1]=i;
			block[2]=0;	block[3]=0;
			block[4]=0;	block[5]=0;
			block[6]=0;	block[7]=0;
			break;

		default:
			i=ctx->clrb[3];
			block[0]=i; block[1]=i;
			block[2]=0;	block[3]=0;
			block[4]=0;	block[5]=0;
			block[6]=0;	block[7]=0;
			break;
		}
	}

//	if(!(fmt&0x70))
	if(1)
	{
//		block[0]=ctx->clrb[3];
//		block[1]=ctx->clrb[3];
//		block[2]=0;	block[3]=0;
//		block[4]=0;	block[5]=0;
//		block[6]=0;	block[7]=0;

		clra=	((ctx->clra[0]<<8)&0xF800) |
				((ctx->clra[1]<<3)&0x07E0) |
				((ctx->clra[2]>>3)&0x001F);
		clrb=	((ctx->clrb[0]<<8)&0xF800) |
				((ctx->clrb[1]<<3)&0x07E0) |
				((ctx->clrb[2]>>3)&0x001F);

		block[ 8]=clra;	block[ 9]=clra>>8;
		block[10]=clrb;	block[11]=clrb>>8;

		switch(cbf)
		{
		case 0:
			block[12]=0x00;	block[13]=0x00;
			block[14]=0x00;	block[15]=0x00;
			break;
		case 1:
			block[12]=0xAA;	block[13]=0xAA;
			block[14]=0xAA;	block[15]=0xAA;
			break;
		case 2:
			block[12]=0xFF;	block[13]=0xFF;
			block[14]=0xFF;	block[15]=0xFF;
			break;
		case 3:
			block[12]=0x55;	block[13]=0x55;
			block[14]=0x55;	block[15]=0x55;
			break;

		case 4:
			i=BTIC3AH_ReadBit(ctx->bits);
			j=i?0x55:0x00;
			block[12]=j;	block[13]=j;
			block[14]=j;	block[15]=j;
			break;
		case 5:
			i=BTIC3AH_Read2Bits(ctx->bits);
			j=idx2[i]; j=j|(j<<2); j=j|(j<<4);
			block[12]=j;	block[13]=j;
			block[14]=j;	block[15]=j;
			break;
		case 6:
			i=BTIC3AH_Read3Bits(ctx->bits);
			j=idx3[i]; j=j|(j<<2); j=j|(j<<4);
			block[12]=j;	block[13]=j;
			block[14]=j;	block[15]=j;
			break;
		case 7:
			i=BTIC3AH_Read4Bits(ctx->bits);
			j=idx4[i]; j=j|(j<<2); j=j|(j<<4);
			block[12]=j;	block[13]=j;
			block[14]=j;	block[15]=j;
			break;

		case 8:
			p0=BTIC3AH_ReadBit(ctx->bits);
			p1=BTIC3AH_ReadBit(ctx->bits);
			p2=BTIC3AH_ReadBit(ctx->bits);
			p3=BTIC3AH_ReadBit(ctx->bits);
			i=p0|(p0<<2)|(p1<<4)|(p1<<6);
			j=p2|(p2<<2)|(p3<<4)|(p3<<6);
			block[12]=i;	block[13]=i;
			block[14]=j;	block[15]=j;
			break;
		case 9:
			p0=idx2[BTIC3AH_Read2Bits(ctx->bits)];
			p1=idx2[BTIC3AH_Read2Bits(ctx->bits)];
			p2=idx2[BTIC3AH_Read2Bits(ctx->bits)];
			p3=idx2[BTIC3AH_Read2Bits(ctx->bits)];
			i=p0|(p0<<2)|(p1<<4)|(p1<<6);
			j=p2|(p2<<2)|(p3<<4)|(p3<<6);
			block[12]=i;	block[13]=i;
			block[14]=j;	block[15]=j;
			break;
		case 10:
			p0=idx3[BTIC3AH_Read3Bits(ctx->bits)];
			p1=idx3[BTIC3AH_Read3Bits(ctx->bits)];
			p2=idx3[BTIC3AH_Read3Bits(ctx->bits)];
			p3=idx3[BTIC3AH_Read3Bits(ctx->bits)];
			i=p0|(p0<<2)|(p1<<4)|(p1<<6);
			j=p2|(p2<<2)|(p3<<4)|(p3<<6);
			block[12]=i;	block[13]=i;
			block[14]=j;	block[15]=j;
			break;
		case 11:
			p0=idx4[BTIC3AH_Read4Bits(ctx->bits)];
			p1=idx4[BTIC3AH_Read4Bits(ctx->bits)];
			p2=idx4[BTIC3AH_Read4Bits(ctx->bits)];
			p3=idx4[BTIC3AH_Read4Bits(ctx->bits)];
			i=p0|(p0<<2)|(p1<<4)|(p1<<6);
			j=p2|(p2<<2)|(p3<<4)|(p3<<6);
			block[12]=i;	block[13]=i;
			block[14]=j;	block[15]=j;
			break;

		case 12:
			p0=BTIC3AH_Read8Bits(ctx->bits);
			p1=BTIC3AH_Read8Bits(ctx->bits);
			block[12]=idxb1[(p0>>4)&15];
			block[13]=idxb1[(p0   )&15];
			block[14]=idxb1[(p1>>4)&15];
			block[15]=idxb1[(p1   )&15];

//			p0=BTIC3AH_Read4Bits(ctx->bits);
//			p1=BTIC3AH_Read4Bits(ctx->bits);
//			p2=BTIC3AH_Read4Bits(ctx->bits);
//			p3=BTIC3AH_Read4Bits(ctx->bits);
//			block[12]=idxb1[p0];
//			block[13]=idxb1[p1];
//			block[14]=idxb1[p2];
//			block[15]=idxb1[p3];
			break;
		case 13:
			p0=BTIC3AH_Read8Bits(ctx->bits);
			p1=BTIC3AH_Read8Bits(ctx->bits);
			p2=BTIC3AH_Read8Bits(ctx->bits);
			p3=BTIC3AH_Read8Bits(ctx->bits);
			q0=	(idx2[(p0>>6)&3]   )|(idx2[(p0>>4)&3]<<2)|
				(idx2[(p0>>2)&3]<<4)|(idx2[(p0   )&3]<<6);
			q1=	(idx2[(p1>>6)&3]   )|(idx2[(p1>>4)&3]<<2)|
				(idx2[(p1>>2)&3]<<4)|(idx2[(p1   )&3]<<6);
			q2=	(idx2[(p2>>6)&3]   )|(idx2[(p2>>4)&3]<<2)|
				(idx2[(p2>>2)&3]<<4)|(idx2[(p2   )&3]<<6);
			q3=	(idx2[(p3>>6)&3]   )|(idx2[(p3>>4)&3]<<2)|
				(idx2[(p3>>2)&3]<<4)|(idx2[(p3   )&3]<<6);
			block[12]=q0;	block[13]=q1;
			block[14]=q2;	block[15]=q3;
			break;

		case 14:
			p0=BTIC3AH_ReadNBits(ctx->bits, 12);
			p1=BTIC3AH_ReadNBits(ctx->bits, 12);
			p2=BTIC3AH_ReadNBits(ctx->bits, 12);
			p3=BTIC3AH_ReadNBits(ctx->bits, 12);
			q0=	(idx3[(p0>>9)&7]   )|(idx3[(p0>>6)&7]<<2)|
				(idx3[(p0>>3)&7]<<4)|(idx3[(p0   )&7]<<6);
			q1=	(idx3[(p1>>9)&7]   )|(idx3[(p1>>6)&7]<<2)|
				(idx3[(p1>>3)&7]<<4)|(idx3[(p1   )&7]<<6);
			q2=	(idx3[(p2>>9)&7]   )|(idx3[(p2>>6)&7]<<2)|
				(idx3[(p2>>3)&7]<<4)|(idx3[(p2   )&7]<<6);
			q3=	(idx3[(p3>>9)&7]   )|(idx3[(p3>>6)&7]<<2)|
				(idx3[(p3>>3)&7]<<4)|(idx3[(p3   )&7]<<6);
			block[12]=q0;	block[13]=q1;
			block[14]=q2;	block[15]=q3;
			break;
		case 15:
			p0=BTIC3AH_ReadNBits(ctx->bits, 16);
			p1=BTIC3AH_ReadNBits(ctx->bits, 16);
			p2=BTIC3AH_ReadNBits(ctx->bits, 16);
			p3=BTIC3AH_ReadNBits(ctx->bits, 16);
			q0=	(idx4[(p0>>12)&15]   )|(idx4[(p0>>8)&15]<<2)|
				(idx4[(p0>> 4)&15]<<4)|(idx4[(p0   )&15]<<6);
			q1=	(idx4[(p1>>12)&15]   )|(idx4[(p1>>8)&15]<<2)|
				(idx4[(p1>> 4)&15]<<4)|(idx4[(p1   )&15]<<6);
			q2=	(idx4[(p2>>12)&15]   )|(idx4[(p2>>8)&15]<<2)|
				(idx4[(p2>> 4)&15]<<4)|(idx4[(p2   )&15]<<6);
			q3=	(idx4[(p3>>12)&15]   )|(idx4[(p3>>8)&15]<<2)|
				(idx4[(p3>> 4)&15]<<4)|(idx4[(p3   )&15]<<6);
			block[12]=q0;	block[13]=q1;
			block[14]=q2;	block[15]=q3;
			break;
		}
		return;
	}
}

BGBBTJ_API int BGBBTJ_BTIC3A_DecodeBlocksDXT5(BTIC3A_ImageCtx *ctx,
	byte *ibuf, int isz, byte *blks, byte *lblks,
	int xs, int ys)
{
	byte tb[16];
	byte *cs, *cse, *csm, *csl;
	byte *ct, *cte, *ctb;
	int xs1, ys1, n, n1, op, bfmt, ret;
	int bi, mx, my, bx, by;
	int mvx, mvy;
	int i, j, k, l;
	
	BGBBTJ_BTIC3A_DecodeInit();
	
	xs1=(xs+3)/4;
	ys1=(ys+3)/4;
	n=xs1*ys1;
	
	ctb=tb;
	
	BTIC3AH_InitStreamRead(ctx->bits, cs, isz);
	
	ret=0; bi=0; mx=0; my=0; bx=0; by=0;
	while(bi<n)
	{
		op=BTIC3AH_DecodeSymbol(ctx->bits, 0);
		switch(op&0xE0)
		{
		case 0x80:
			n1=BTIC3AH_ReadVLCA(ctx->bits, op&31);
			for(i=0; i<n1; i++)
			{
				csl=lblks+(by*xs1+bx)*16;
				ct=blks+(by*xs1+bx)*16;
				bgbbtj_rpza_memcpy16(ct, csl);
				BGBBTJ_BTIC3A_NextBlockIndexRaster(ctx, &bi,
					&mx, &my, &bx, &by);
			}
			break;
		case 0xA0:
			n1=BTIC3AH_ReadVLCA(ctx->bits, op&31);
			BTIC3AH_ReadMotionVector(ctx->bits, &mvx, &mvy);
			for(i=0; i<n1; i++)
			{
				csl=lblks+((by+mvy)*xs1+(bx+mvx))*stride;
				ct=blks+(by*xs1+bx)*stride;
				bgbbtj_rpza_memcpy16(ct, csl);

				BGBBTJ_BTIC3A_NextBlockIndexRaster(ctx, &bi,
					&mx, &my, &bx, &by);
			}
			break;

		case 0xC0:
			n1=BTIC3AH_ReadVLCA(ctx->bits, op&31);
			bfmt=BTIC3AH_DecodeSymbol(ctx->bits, 0);
			if((bfmt&0x70)==0x70)
			{
				bfmt=BGBBTJ_BTIC3A_DecodeExtendedFormat(ctx, bfmt);
			}else
			{
				if(!(bfmt&0x0C))
				{
					BGBBTJ_BTIC3A_DecodeBlocksDXTn_BlockFormat(ctx,
						ctb, bfmt);

					for(i=0; i<n1; i++)
					{
						ct=blks+(by*xs1+bx)*16;
						bgbbtj_rpza_memcpy16(ct, ctb);
						BGBBTJ_BTIC3A_NextBlockIndexRaster(ctx, &bi,
							&mx, &my, &bx, &by);
					}
					break;
				}
			}

			for(i=0; i<n1; i++)
			{
				ct=blks+(by*xs1+bx)*16;
				BGBBTJ_BTIC3A_DecodeBlocksDXTn_BlockFormat(ctx, ct, bfmt);
				BGBBTJ_BTIC3A_NextBlockIndexRaster(ctx, &bi,
					&mx, &my, &bx, &by);
			}
			break;

		case 0xE0:
			i=BTIC3AH_ReadVLCA(ctx->bits, op&31);
			BGBBTJ_BTIC3A_DecodeExtendedCommand(ctx, i);
			break;

		default:
			ct=blks+(by*xs1+bx)*16;
			bfmt=BGBBTJ_BTIC3A_DecodeExtendedFormat(ctx, op);
			BGBBTJ_BTIC3A_DecodeBlocksDXTn_BlockFormat(ctx, ct, bfmt);
			BGBBTJ_BTIC3A_NextBlockIndexRaster(ctx, &bi,
				&mx, &my, &bx, &by);
			break;
		}
	}
	
	if(ret)return(ret);
	return(cs-ibuf);
}
