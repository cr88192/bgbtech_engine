#include <bgbbtj.h>

extern const u32 btic2c_dbase[64];
extern const int btic2c_dextra[64];
extern const int btic2c_lbase[64];
extern const int btic2c_lextra[64];

extern const int btic3c_hilbert_idx[16];
extern const int btic3c_hilbert_xy[32];

int BGBBTJ_BTIC3A_DecodeInit()
{
	static int init=0;
	
	if(init)return(0);
	init=1;
	
	return(1);
}

BGBBTJ_API int BGBBTJ_BTIC3A_BlockIndexRaster(
	BTIC3A_ImageCtx *ctx, int bi, int *rx, int *ry)
{
	int mi, mx, my;
	
	mi=(bi>>4);
	my=mi/ctx->xsmb;
	mx=mi%ctx->xsmb;
}

BGBBTJ_API int BGBBTJ_BTIC3A_DecodeExtendedFormat(
	BTIC3A_ImageCtx *ctx, int bfmt)
{
	if((bfmt&0x70)==0x70)
	{
		switch(bfmt&15)
		{
		case 0x00: bfmt|=BTIC3AH_Read4Bits(ctx->bits)<<12; break;
		case 0x01: bfmt|=BTIC3AH_Read6Bits(ctx->bits)<<10; break;
		case 0x02: bfmt|=BTIC3AH_Read8Bits(ctx->bits)<< 8; break;
		case 0x04:	bfmt|=BTIC3AH_Read3Bits(ctx->bits)<<16;
					bfmt|=BTIC3AH_Read4Bits(ctx->bits)<<12;	break;
		case 0x05:	bfmt|=BTIC3AH_Read3Bits(ctx->bits)<<16;
					bfmt|=BTIC3AH_Read6Bits(ctx->bits)<<10; break;
		case 0x06:	bfmt|=BTIC3AH_Read3Bits(ctx->bits)<<16;
					bfmt|=BTIC3AH_Read8Bits(ctx->bits)<< 8; break;
		case 0x08:	bfmt|=BTIC3AH_Read5Bits(ctx->bits)<<16;
					bfmt|=BTIC3AH_Read4Bits(ctx->bits)<<12; break;
		case 0x09:	bfmt|=BTIC3AH_Read5Bits(ctx->bits)<<16;
					bfmt|=BTIC3AH_Read6Bits(ctx->bits)<<10; break;
		case 0x0A:	bfmt|=BTIC3AH_Read5Bits(ctx->bits)<<16;
					bfmt|=BTIC3AH_Read8Bits(ctx->bits)<< 8; break;
		case 0x0C:	bfmt|=BTIC3AH_Read8Bits(ctx->bits)<<16;
					bfmt|=BTIC3AH_Read4Bits(ctx->bits)<<12; break;
		case 0x0D:	bfmt|=BTIC3AH_Read8Bits(ctx->bits)<<16;
					bfmt|=BTIC3AH_Read6Bits(ctx->bits)<<10; break;
		case 0x0E:	bfmt|=BTIC3AH_Read8Bits(ctx->bits)<<16;
					bfmt|=BTIC3AH_Read8Bits(ctx->bits)<< 8; break;
		default:	break;
		}
	}
	return(bfmt);
}

void BGBBTJ_BTIC3A_DecodeHandleColorDelta(
	BTIC3A_ImageCtx *ctx, int cdf)
{
	int dra, dga, dba, daa;
	int drb, dgb, dbb, dab;

//	if(fmt&0x70)
	if(cdf)
	{
//		switch((fmt>>4)&7)
		switch(cdf)
		{
		case 0:
			break;
		case 1:
			dra=BTIC3AH_Read2Bits(ctx->bits)-2;
			dga=BTIC3AH_Read3Bits(ctx->bits)-4;
			dba=BTIC3AH_Read2Bits(ctx->bits)-2;
			drb=BTIC3AH_Read2Bits(ctx->bits)-2;
			dgb=BTIC3AH_Read3Bits(ctx->bits)-4;
			dbb=BTIC3AH_Read2Bits(ctx->bits)-2;
			daa=0; dab=0;
			break;
		case 2:
			dra=BTIC3AH_Read3Bits(ctx->bits)-4;
			dga=BTIC3AH_Read4Bits(ctx->bits)-8;
			dba=BTIC3AH_Read3Bits(ctx->bits)-4;
			drb=BTIC3AH_Read3Bits(ctx->bits)-4;
			dgb=BTIC3AH_Read4Bits(ctx->bits)-8;
			dbb=BTIC3AH_Read3Bits(ctx->bits)-4;
			daa=0; dab=0;
			break;
		case 3:
			dra=BTIC3AH_Read5Bits(ctx->bits)-16;
			dga=BTIC3AH_Read6Bits(ctx->bits)-32;
			dba=BTIC3AH_Read5Bits(ctx->bits)-16;
			drb=BTIC3AH_Read5Bits(ctx->bits)-16;
			dgb=BTIC3AH_Read6Bits(ctx->bits)-32;
			dbb=BTIC3AH_Read5Bits(ctx->bits)-16;
			daa=0; dab=0;
			break;
		case 4:
			dra=BTIC3AH_Read8Bits(ctx->bits)-128;
			dga=BTIC3AH_Read8Bits(ctx->bits)-128;
			dba=BTIC3AH_Read8Bits(ctx->bits)-128;
			drb=BTIC3AH_Read8Bits(ctx->bits)-128;
			dgb=BTIC3AH_Read8Bits(ctx->bits)-128;
			dbb=BTIC3AH_Read8Bits(ctx->bits)-128;
			daa=0; dab=0;
			break;

		case 5:
			dga=BTIC3AH_Read3Bits(ctx->bits)-4;
			dgb=BTIC3AH_Read3Bits(ctx->bits)-4;
			dra=dga; dba=dga;
			drb=dgb; dbb=dgb;
			daa=0; dab=0;
			break;
		case 6:
			dga=BTIC3AH_Read5Bits(ctx->bits)-16;
			dgb=BTIC3AH_Read5Bits(ctx->bits)-16;
			dra=dga; dba=dga;
			drb=dgb; dbb=dgb;
			daa=0; dab=0;
			break;
		case 7:
			dga=BTIC3AH_Read8Bits(ctx->bits)-128;
			dgb=BTIC3AH_Read8Bits(ctx->bits)-128;
			dra=dga; dba=dga;
			drb=dgb; dbb=dgb;
			daa=0; dab=0;
			break;

		case 8:
			dra=BTIC3AH_Read2Bits(ctx->bits)-2;
			dga=BTIC3AH_Read3Bits(ctx->bits)-4;
			dba=BTIC3AH_Read2Bits(ctx->bits)-2;
			daa=BTIC3AH_Read3Bits(ctx->bits)-4;
			drb=BTIC3AH_Read2Bits(ctx->bits)-2;
			dgb=BTIC3AH_Read3Bits(ctx->bits)-4;
			dbb=BTIC3AH_Read2Bits(ctx->bits)-2;
			dab=BTIC3AH_Read3Bits(ctx->bits)-4;
			break;
		case 9:
			dra=BTIC3AH_Read3Bits(ctx->bits)-4;
			dga=BTIC3AH_Read4Bits(ctx->bits)-8;
			dba=BTIC3AH_Read3Bits(ctx->bits)-4;
			daa=BTIC3AH_Read4Bits(ctx->bits)-8;
			drb=BTIC3AH_Read3Bits(ctx->bits)-4;
			dgb=BTIC3AH_Read4Bits(ctx->bits)-8;
			dbb=BTIC3AH_Read3Bits(ctx->bits)-4;
			dab=BTIC3AH_Read4Bits(ctx->bits)-8;
			break;
		case 10:
			dra=BTIC3AH_Read5Bits(ctx->bits)-16;
			dga=BTIC3AH_Read6Bits(ctx->bits)-32;
			dba=BTIC3AH_Read5Bits(ctx->bits)-16;
			dga=BTIC3AH_Read6Bits(ctx->bits)-32;
			drb=BTIC3AH_Read5Bits(ctx->bits)-16;
			dgb=BTIC3AH_Read6Bits(ctx->bits)-32;
			dbb=BTIC3AH_Read5Bits(ctx->bits)-16;
			dga=BTIC3AH_Read6Bits(ctx->bits)-32;
			break;
		case 11:
			dra=BTIC3AH_Read8Bits(ctx->bits)-128;
			dga=BTIC3AH_Read8Bits(ctx->bits)-128;
			dba=BTIC3AH_Read8Bits(ctx->bits)-128;
			daa=BTIC3AH_Read8Bits(ctx->bits)-128;
			drb=BTIC3AH_Read8Bits(ctx->bits)-128;
			dgb=BTIC3AH_Read8Bits(ctx->bits)-128;
			dbb=BTIC3AH_Read8Bits(ctx->bits)-128;
			dab=BTIC3AH_Read8Bits(ctx->bits)-128;
			break;

		case 12:
			dga=BTIC3AH_Read2Bits(ctx->bits)-2;
			daa=BTIC3AH_Read2Bits(ctx->bits)-2;
			dgb=BTIC3AH_Read2Bits(ctx->bits)-2;
			dab=BTIC3AH_Read2Bits(ctx->bits)-2;
			dra=dga; dba=dga;
			drb=dgb; dbb=dgb;
			break;
		case 13:
			dga=BTIC3AH_Read3Bits(ctx->bits)-4;
			daa=BTIC3AH_Read3Bits(ctx->bits)-4;
			dgb=BTIC3AH_Read3Bits(ctx->bits)-4;
			dab=BTIC3AH_Read3Bits(ctx->bits)-4;
			dra=dga; dba=dga;
			drb=dgb; dbb=dgb;
			break;
		case 14:
			dga=BTIC3AH_Read5Bits(ctx->bits)-16;
			daa=BTIC3AH_Read5Bits(ctx->bits)-16;
			dgb=BTIC3AH_Read5Bits(ctx->bits)-16;
			dab=BTIC3AH_Read5Bits(ctx->bits)-16;
			dra=dga; dba=dga;
			drb=dgb; dbb=dgb;
			break;
		case 15:
			dga=BTIC3AH_Read8Bits(ctx->bits)-128;
			daa=BTIC3AH_Read8Bits(ctx->bits)-128;
			dgb=BTIC3AH_Read8Bits(ctx->bits)-128;
			dab=BTIC3AH_Read8Bits(ctx->bits)-128;
			dra=dga; dba=dga;
			drb=dgb; dbb=dgb;
			break;

		case 16:
			dra=BTIC3AH_Read2Bits(ctx->bits)-2;
			dga=BTIC3AH_Read3Bits(ctx->bits)-4;
			dba=BTIC3AH_Read2Bits(ctx->bits)-2;
			drb=dra; dgb=dga; dbb=dba;
			daa=0; dab=0;
			break;
		case 17:
			dra=BTIC3AH_Read5Bits(ctx->bits)-16;
			dga=BTIC3AH_Read6Bits(ctx->bits)-32;
			dba=BTIC3AH_Read5Bits(ctx->bits)-16;
			drb=dra; dgb=dga; dbb=dba;
			daa=0; dab=0;
			break;
		case 18:
			p0=BTIC3AH_Read3Bits(ctx->bits)-4;
			dra=p0; dga=p0; dba=p0;
			drb=p0; dgb=p0; dbb=p0;
			daa=0; dab=0;
			break;
		case 19:
			p0=BTIC3AH_Read5Bits(ctx->bits)-16;
			dra=p0; dga=p0; dba=p0;
			drb=p0; dgb=p0; dbb=p0;
			daa=0; dab=0;
			break;

		case 20:
			dra=BTIC3AH_Read2Bits(ctx->bits)-2;
			dga=BTIC3AH_Read3Bits(ctx->bits)-4;
			dba=BTIC3AH_Read2Bits(ctx->bits)-2;
			daa=BTIC3AH_Read3Bits(ctx->bits)-4;
			drb=dra; dgb=dga; dbb=dba; dab=daa;
			break;
		case 21:
			dra=BTIC3AH_Read5Bits(ctx->bits)-16;
			dga=BTIC3AH_Read6Bits(ctx->bits)-32;
			dba=BTIC3AH_Read5Bits(ctx->bits)-16;
			daa=BTIC3AH_Read6Bits(ctx->bits)-32;
			drb=dra; dgb=dga; dbb=dba; dab=daa;
			break;

		case 22:
			p0=BTIC3AH_Read3Bits(ctx->bits)-4;
			p1=BTIC3AH_Read3Bits(ctx->bits)-4;
			dra=p0; dga=p0; dba=p0;
			drb=p0; dgb=p0; dbb=p0;
			daa=p1; dab=p1;
			break;
		case 23:
			p0=BTIC3AH_Read5Bits(ctx->bits)-16;
			p1=BTIC3AH_Read5Bits(ctx->bits)-16;
			dra=p0; dga=p0; dba=p0;
			drb=p0; dgb=p0; dbb=p0;
			daa=p1; dab=p1;
			break;
			
		case 24:
			dra=BTIC3AH_DecodeSVLCB(ctx, 1);
			dga=BTIC3AH_DecodeSVLCB(ctx, 1);
			dba=BTIC3AH_DecodeSVLCB(ctx, 1);
			drb=BTIC3AH_DecodeSVLCB(ctx, 1);
			dgb=BTIC3AH_DecodeSVLCB(ctx, 1);
			dbb=BTIC3AH_DecodeSVLCB(ctx, 1);
			daa=0; dab=0;
			break;
		case 25:
			dra=BTIC3AH_DecodeSVLCB(ctx, 1);
			dga=BTIC3AH_DecodeSVLCB(ctx, 1);
			dba=BTIC3AH_DecodeSVLCB(ctx, 1);
			daa=BTIC3AH_DecodeSVLCB(ctx, 1);
			drb=BTIC3AH_DecodeSVLCB(ctx, 1);
			dgb=BTIC3AH_DecodeSVLCB(ctx, 1);
			dbb=BTIC3AH_DecodeSVLCB(ctx, 1);
			dab=BTIC3AH_DecodeSVLCB(ctx, 1);
			break;
		case 26:
			dga=BTIC3AH_DecodeSVLCB(ctx, 1);
			dgb=BTIC3AH_DecodeSVLCB(ctx, 1);
			dra=dga; dba=dga;
			drb=dgb; dbb=dgb;
			daa=0; dab=0;
			break;
		case 27:
			dga=BTIC3AH_DecodeSVLCB(ctx, 1);
			daa=BTIC3AH_DecodeSVLCB(ctx, 1);
			dgb=BTIC3AH_DecodeSVLCB(ctx, 1);
			dab=BTIC3AH_DecodeSVLCB(ctx, 1);
			dra=dga; dba=dga;
			drb=dgb; dbb=dgb;
			break;

		case 28:
			dra=BTIC3AH_DecodeSVLCB(ctx, 1);
			dga=BTIC3AH_DecodeSVLCB(ctx, 1);
			dba=BTIC3AH_DecodeSVLCB(ctx, 1);
			drb=dra; dgb=dga; dbb=dba;
			daa=0; dab=0;
			break;
		case 29:
			dra=BTIC3AH_DecodeSVLCB(ctx, 1);
			dga=BTIC3AH_DecodeSVLCB(ctx, 1);
			dba=BTIC3AH_DecodeSVLCB(ctx, 1);
			daa=BTIC3AH_DecodeSVLCB(ctx, 1);
			drb=dra; dgb=dga; dbb=dba; dba=daa;
			break;
		case 30:
			dga=BTIC3AH_DecodeSVLCB(ctx, 1);
			dra=dga; dba=dga;
			drb=dga; dgb=dga; dbb=dga;
			daa=0; dab=0;
			break;
		case 31:
			dga=BTIC3AH_DecodeSVLCB(ctx, 1);
			daa=BTIC3AH_DecodeSVLCB(ctx, 1);
			dra=dga; dba=dga;
			drb=dga; dbb=dga; dab=daa;
			break;

		default:
			break;
		}

		ctx->clra[0]+=dra*ctx->clrsc[0];
		ctx->clra[1]+=dga*ctx->clrsc[1];
		ctx->clra[2]+=dba*ctx->clrsc[2];
		ctx->clra[3]+=daa*ctx->clrsc[3];
		ctx->clrb[0]+=drb*ctx->clrsc[0];
		ctx->clrb[1]+=dgb*ctx->clrsc[1];
		ctx->clrb[2]+=dbb*ctx->clrsc[2];
		ctx->clrb[3]+=dab*ctx->clrsc[3];
	}
}

void BGBBTJ_BTIC3A_DecodeExtendedCommand(
	BTIC3A_ImageCtx *ctx, int cmdop)
{
	int cr, cg, cb, ca;

	switch(cmdop)
	{
	case 0x00:
		break;
	case 0x01:
		switch(ctx->clrfmt)
		{
		case BTIC3A_CLRFMT_RGBA8:
			ctx->clra[0]=BTIC3AH_Read8Bits(ctx->bits);
			ctx->clra[1]=BTIC3AH_Read8Bits(ctx->bits);
			ctx->clra[2]=BTIC3AH_Read8Bits(ctx->bits);
			break;
		case BTIC3A_CLRFMT_RGBA565:
			cr=BTIC3AH_Read5Bits(ctx->bits);
			cg=BTIC3AH_Read6Bits(ctx->bits);
			cb=BTIC3AH_Read5Bits(ctx->bits);
			cr=(cr<<3)|(cr>>2);		cg=(cg<<2)|(cg>>4);
			cb=(cb<<3)|(cb>>2);		ctx->clra[0]=cr;
			ctx->clra[1]=cg;		ctx->clra[2]=cb;
			break;
		case BTIC3A_CLRFMT_RGBA16:
			ctx->clra[0]=BTIC3AH_Read16Bits(ctx->bits);
			ctx->clra[1]=BTIC3AH_Read16Bits(ctx->bits);
			ctx->clra[2]=BTIC3AH_Read16Bits(ctx->bits);
			break;
		default:
			break;
		}
		break;
	case 0x02:
		switch(ctx->clrfmt)
		{
		case BTIC3A_CLRFMT_RGBA8:
			ctx->clrb[0]=BTIC3AH_Read8Bits(ctx->bits);
			ctx->clrb[1]=BTIC3AH_Read8Bits(ctx->bits);
			ctx->clrb[2]=BTIC3AH_Read8Bits(ctx->bits);
			break;
		case BTIC3A_CLRFMT_RGBA565:
			cr=BTIC3AH_Read5Bits(ctx->bits);
			cg=BTIC3AH_Read6Bits(ctx->bits);
			cb=BTIC3AH_Read5Bits(ctx->bits);
			cr=(cr<<3)|(cr>>2);		cg=(cg<<2)|(cg>>4);
			cb=(cb<<3)|(cb>>2);		ctx->clrb[0]=cr;
			ctx->clrb[1]=cg;		ctx->clrb[2]=cb;
			break;
		case BTIC3A_CLRFMT_RGBA16:
			ctx->clrb[0]=BTIC3AH_Read16Bits(ctx->bits);
			ctx->clrb[1]=BTIC3AH_Read16Bits(ctx->bits);
			ctx->clrb[2]=BTIC3AH_Read16Bits(ctx->bits);
			break;
		default:
			break;
		}
		break;
	case 0x03:
		switch(ctx->clrfmt)
		{
		case BTIC3A_CLRFMT_RGBA8:
			ctx->clra[0]=BTIC3AH_Read8Bits(ctx->bits);
			ctx->clra[1]=BTIC3AH_Read8Bits(ctx->bits);
			ctx->clra[2]=BTIC3AH_Read8Bits(ctx->bits);
			ctx->clra[3]=BTIC3AH_Read8Bits(ctx->bits);
			break;
		case BTIC3A_CLRFMT_RGBA565:
			cr=BTIC3AH_Read5Bits(ctx->bits);
			cg=BTIC3AH_Read6Bits(ctx->bits);
			cb=BTIC3AH_Read5Bits(ctx->bits);
			ca=BTIC3AH_Read8Bits(ctx->bits);
			cr=(cr<<3)|(cr>>2);	cg=(cg<<2)|(cg>>4);
			cb=(cb<<3)|(cb>>2);
			ctx->clra[0]=cr;	ctx->clra[1]=cg;
			ctx->clra[2]=cb;	ctx->clra[3]=ca;
			break;
		case BTIC3A_CLRFMT_RGBA16:
			ctx->clra[0]=BTIC3AH_Read16Bits(ctx->bits);
			ctx->clra[1]=BTIC3AH_Read16Bits(ctx->bits);
			ctx->clra[2]=BTIC3AH_Read16Bits(ctx->bits);
			ctx->clra[3]=BTIC3AH_Read16Bits(ctx->bits);
			break;
		default:
			break;
		}
		break;
	case 0x04:
		switch(ctx->clrfmt)
		{
		case BTIC3A_CLRFMT_RGBA8:
			ctx->clrb[0]=BTIC3AH_Read8Bits(ctx->bits);
			ctx->clrb[1]=BTIC3AH_Read8Bits(ctx->bits);
			ctx->clrb[2]=BTIC3AH_Read8Bits(ctx->bits);
			ctx->clrb[3]=BTIC3AH_Read8Bits(ctx->bits);
			break;
		case BTIC3A_CLRFMT_RGBA565:
			cr=BTIC3AH_Read5Bits(ctx->bits);
			cg=BTIC3AH_Read6Bits(ctx->bits);
			cb=BTIC3AH_Read5Bits(ctx->bits);
			ca=BTIC3AH_Read8Bits(ctx->bits);
			cr=(cr<<3)|(cr>>2);	cg=(cg<<2)|(cg>>4);
			cb=(cb<<3)|(cb>>2);
			ctx->clrb[0]=cr;	ctx->clrb[1]=cg;
			ctx->clrb[2]=cb;	ctx->clrb[3]=ca;
			break;
		case BTIC3A_CLRFMT_RGBA16:
			ctx->clrb[0]=BTIC3AH_Read16Bits(ctx->bits);
			ctx->clrb[1]=BTIC3AH_Read16Bits(ctx->bits);
			ctx->clrb[2]=BTIC3AH_Read16Bits(ctx->bits);
			ctx->clrb[3]=BTIC3AH_Read16Bits(ctx->bits);
			break;
		default:
			break;
		}
		break;
	case 0x05:
//		ctx->clrsc[0]=BTIC3AH_ReadAdSRice(ctx, &ctx->sc_rky);
//		ctx->clrsc[1]=BTIC3AH_ReadAdSRice(ctx, &ctx->sc_rky);
//		ctx->clrsc[2]=BTIC3AH_ReadAdSRice(ctx, &ctx->sc_rky);
		cr=BTIC3AH_DecodeSVLCB(ctx, 1);
		cg=BTIC3AH_DecodeSVLCB(ctx, 1);
		cb=BTIC3AH_DecodeSVLCB(ctx, 1);
		ctx->clrsc[0]=cr;
		ctx->clrsc[1]=cg;
		ctx->clrsc[2]=cb;
		break;
	case 0x06:
//		ctx->clrsc[0]=BTIC3AH_ReadAdSRice(ctx, &ctx->sc_rky);
//		ctx->clrsc[1]=BTIC3AH_ReadAdSRice(ctx, &ctx->sc_rky);
//		ctx->clrsc[2]=BTIC3AH_ReadAdSRice(ctx, &ctx->sc_rky);
//		ctx->clrsc[3]=BTIC3AH_ReadAdSRice(ctx, &ctx->sc_rka);
		cr=BTIC3AH_DecodeSVLCB(ctx, 1);
		cg=BTIC3AH_DecodeSVLCB(ctx, 1);
		cb=BTIC3AH_DecodeSVLCB(ctx, 1);
		ca=BTIC3AH_DecodeSVLCB(ctx, 1);
		ctx->clrsc[0]=cr;	ctx->clrsc[1]=cg;
		ctx->clrsc[2]=cb;	ctx->clrsc[2]=ca;
		break;
	case 0x07:
//		cg=BTIC3AH_ReadAdSRice(ctx, &ctx->sc_rky);
//		ca=BTIC3AH_ReadAdSRice(ctx, &ctx->sc_rka);
//		ctx->clrsc[0]=cg;	ctx->clrsc[1]=cg;
//		ctx->clrsc[2]=cg;	ctx->clrsc[3]=ca;
		cg=BTIC3AH_DecodeSVLCB(ctx, 1);
		ca=BTIC3AH_DecodeSVLCB(ctx, 1);
		ctx->clrsc[0]=cg;	ctx->clrsc[1]=cg;
		ctx->clrsc[2]=cg;	ctx->clrsc[2]=ca;
		break;

	case 0x08:
		ctx->clra[0]=0;		ctx->clra[1]=0;
		ctx->clra[2]=0;		ctx->clra[3]=0;
		ctx->clrb[0]=255;	ctx->clrb[1]=255;
		ctx->clrb[2]=255;	ctx->clrb[3]=255;

		i=BTIC3AH_Read4Bits(ctx->bits);
		switch(i)
		{
		case 0:		case 1:
			ctx->clrfmt=BTIC3A_CLRFMT_RGBA565;
			ctx->clrsc[0]=8;	ctx->clrsc[1]=4;
			ctx->clrsc[2]=8;	ctx->clrsc[3]=1;
			break;
		case 2:		case 3:
			ctx->clrfmt=BTIC3A_CLRFMT_RGBA8;
			ctx->clrsc[0]=1;	ctx->clrsc[1]=1;
			ctx->clrsc[2]=1;	ctx->clrsc[3]=1;
			break;
		case 4:		case 5:
			ctx->clrfmt=BTIC3A_CLRFMT_RGBA16;
			ctx->clrsc[0]=1;	ctx->clrsc[1]=1;
			ctx->clrsc[2]=1;	ctx->clrsc[3]=1;
			ctx->clrb[0]=0x4000;	ctx->clrb[1]=0x4000;
			ctx->clrb[2]=0x4000;	ctx->clrb[3]=0x4000;
			break;
		default:
			break;
		}
		break;

	default:
		break;
	}
}

void BGBBTJ_BTIC3A_NextBlockIndexRaster(
	BTIC3A_ImageCtx *ctx, int *rbi,
	int *rmx, int *rmy
	int *rbx, int *rby)
{
	int bi, bj, mi, mx, my, hi;

	bi=*rbi;
	bj=bi+1;
//	if(((bi+1)&(~15)) == (bi&(~15)))
	if(!((bi^bj)&(~15)))
	{
		*rbi=bj;
		hi=bj&15;
		*rbx=((*rmx)<<2)+btic3c_hilbert_xy[hi*2+0];
		*rby=((*rmy)<<2)+btic3c_hilbert_xy[hi*2+1];
		return;
	}

	*rbi=bj;
	
	mi=(bj>>4);
	my=mi/ctx->xsmb;
	mx=mi%ctx->xsmb;
	
	*rmx=mx;
	*rmy=my;

	hi=bj&15;
	*rbx=(mx<<2)+btic3c_hilbert_xy[hi*2+0];
	*rby=(my<<2)+btic3c_hilbert_xy[hi*2+1];
}

int BTIC3A_MarkerImgData(BTIC3A_Context *ctx, byte *buf, int len,
	byte *obuf, int clrs)
{
	if(!ctx->blks)
		{ ctx->blks=gcatomic(ctx->xsb*ctx->ysb*16); }
	if(!ctx->lblks)
		{ ctx->lblks=gcatomic(ctx->xsb*ctx->ysb*16); }

	//reset to start-of-frame defaults
	ctx->clra[0]=0;		ctx->clra[1]=0;
	ctx->clra[2]=0;		ctx->clra[3]=0;
	ctx->clrb[0]=255;	ctx->clrb[1]=255;
	ctx->clrb[2]=255;	ctx->clrb[3]=255;
	ctx->clrsc[0]=1;	ctx->clrsc[1]=1;
	ctx->clrsc[2]=1;	ctx->clrsc[3]=1;
	ctx->bits->mv_rkx=4;
	ctx->bits->mv_rky=4;
	ctx->sc_rky=4;	ctx->sc_rka=4;
	
	switch(ctx->imgtype)
	{
	case BGBBTJ_JPG_RGBA:
	case BGBBTJ_JPG_RGB:
	case BGBBTJ_JPG_BC7:
	case BGBBTJ_JPG_BC7_SRGB:
		ctx->clrfmt=BTIC3A_CLRFMT_RGBA8;
		ctx->clrsc[0]=1;	ctx->clrsc[1]=1;
		ctx->clrsc[2]=1;	ctx->clrsc[3]=1;
		break;
	case BGBBTJ_JPG_DXT1:
	case BGBBTJ_JPG_DXT3:
	case BGBBTJ_JPG_DXT5:
		ctx->clrfmt=BTIC3A_CLRFMT_RGBA565;
		ctx->clrsc[0]=8;	ctx->clrsc[1]=4;
		ctx->clrsc[2]=8;	ctx->clrsc[3]=1;
		break;
	case BGBBTJ_JPG_RGBA_F16:
	case BGBBTJ_JPG_RGB_F16:
	case BGBBTJ_JPG_BC6H_SF16:
	case BGBBTJ_JPG_BC6H_UF16:
		ctx->clrfmt=BTIC3A_CLRFMT_RGBA16;
		ctx->clrsc[0]=1;	ctx->clrsc[1]=1;
		ctx->clrsc[2]=1;	ctx->clrsc[3]=1;
		ctx->clrb[0]=0x4000;	ctx->clrb[1]=0x4000;
		ctx->clrb[2]=0x4000;	ctx->clrb[3]=0x4000;
		break;
	default:
		ctx->clrfmt=BTIC3A_CLRFMT_RGBA8;
		ctx->clrsc[0]=1;	ctx->clrsc[1]=1;
		ctx->clrsc[2]=1;	ctx->clrsc[3]=1;
		break;
	}

	switch(clrs)
	{
	case BGBBTJ_JPG_RGBA:
		i=BGBBTJ_BTIC3A_DecodeBlocksDXT5(ctx, buf, len,
			ctx->blks, ctx->lblks, ctx->xs, ctx->ys);
		BGBBTJ_BCn_FlipImageDXT5(ctx->blks, ctx->lblks, xs, ys);
		BGBBTJ_BCn_DecodeImageDXT5(ctx->lblks, dst, xs, ys, 4);
//		BGBBTJ_BCn_DecodeImageDXT5(hdr->data, dst, xs, ys, 4);
		memcpy(ctx->lblks, ctx->blks, ctx->xsb*ctx->ysb*16);
		break;
	case BGBBTJ_JPG_RGB:
		i=BGBBTJ_BTIC3A_DecodeBlocksDXT1(ctx, buf, len,
			ctx->blks, ctx->lblks, ctx->xs, ctx->ys);
		BGBBTJ_BCn_FlipImageDXT1(ctx->blks, ctx->lblks, xs, ys, 8);
		BGBBTJ_BCn_DecodeImageDXT1(ctx->lblks, dst, xs, ys, 3);
		memcpy(ctx->lblks, ctx->blks, ctx->xsb*ctx->ysb*8);
		break;
	case BGBBTJ_JPG_BC1:
	case BGBBTJ_JPG_BC1F:
	case BGBBTJ_JPG_BC1A:
		i=BGBBTJ_BTIC3A_DecodeBlocksDXT1(ctx, buf, len,
			ctx->blks, ctx->lblks, ctx->xs, ctx->ys);
		BGBBTJ_BCn_FlipImageDXT1(ctx->blks, dst, xs, ys, 8);
		memcpy(ctx->lblks, ctx->blks, ctx->xsb*ctx->ysb*8);
		break;
	case BGBBTJ_JPG_BC3:
	case BGBBTJ_JPG_BC3F:
		i=BGBBTJ_BTIC3A_DecodeBlocksDXT5(ctx, buf, len,
			ctx->blks, ctx->lblks, ctx->xs, ctx->ys);
		BGBBTJ_BCn_FlipImageDXT5(ctx->blks, dst, xs, ys);
		memcpy(ctx->lblks, ctx->blks, ctx->xsb*ctx->ysb*16);
		break;
	}

	return(i);
}

int BTIC3A_MarkerDHT(BTIC3A_Context *ctx, byte *buf, int tn, int len)
	{ return(BTIC3AH_MarkerDHT(ctx->bits, buf, tn, len)); }

int BTIC3A_MarkerHead(BTIC3A_Context *ctx, byte *buf, int len)
{
	int fcc, ity, cty;
	int mips, mipe, imgpl, imgver;
	int imgid, imgseq;
	int xs, ys, xs1, ys1, xs2, ys2, xs3, ys3, fl;
	int n;
	int i, j, k, l;
	
	if(	(buf[0]!='b') || (buf[1]!='t') ||
		(buf[2]!='3') || (buf[3]!='a'))
	{
		return(-1);
	}
	
//	fcc=(buf[4]<<24)|(buf[5]<<16)|(buf[ 6]<<8)|(buf[ 7]);
	xs=(buf[4]<<24)|(buf[5]<<16)|(buf[ 6]<<8)|(buf[ 7]);
	ys=(buf[8]<<24)|(buf[9]<<16)|(buf[10]<<8)|(buf[11]);
	ity=(buf[12]<<8)|buf[13];
	cty=(buf[14]<<8)|buf[15];
	mips=buf[16];
	mipe=buf[17];
	imgpl=buf[18];
	imgver=buf[19];
	fl=(buf[20]<<24)|(buf[21]<<16)|(buf[22]<<8)|(buf[23]);
	imgid=(buf[24]<<8)|buf[25];
	imgseq=(buf[26]<<8)|buf[27];

	ctx->xs=xs;
	ctx->ys=ys;
	ctx->imgtype=ity;
	ctx->clrtype=cty;
	ctx->mipstrt=mips;
	ctx->mipend=mipe;
	ctx->imgplane=imgpl;
	ctx->imgflags=fl;
	
	xs1=(xs+15)/16;
	ys1=(ys+15)/16;
	ctx->xsmb=xs1;
	ctx->ysmb=ys1;
	ctx->xsb=xs1*4;
	ctx->ysb=ys1*4;

	xs2=xs; ys2=ys; n=0;
	for(i=mips; i<=mipe; i++)
	{
		xs3=(xs2+15)/16;
		ys3=(ys2+15)/16;
		n+=xs3*ys3;
		xs2=(xs2+1)>>1;
		ys2=(ys2+1)>>1;
	}
	
	j=(n+(xs1-1))/xs1;
	ctx->ysmb_mip=j;
	ctx->ysmb_miplane=j*imgpl;

//	return(BTIC3AH_MarkerDHT(ctx->bits, buf, tn, len));
}

int BTIC3A_DecodeCtx(BTIC3A_Context *ctx,
	byte *ibuf, byte *obuf, int isz, int osz,
	int *rxs, int *rys, int clrs)
{
	byte *cs, *cse;
	int i, j, k, l;

	if(!ctx->bits)
	{
		ctx->bits=BTIC3AH_AllocBitsteam();
		BTIC3AH_SetupTabUniform8(ctx->bits, 0);
		BTIC3AH_SetupTabUniform8(ctx->bits, 1);
		BTIC3AH_SetupTabUniform8(ctx->bits, 2);
		BTIC3AH_SetupTabUniform8(ctx->bits, 3);
	}

	cs=ibuf; cse=ibuf+isz;
	while(cs<cse)
	{
		if(*cs==0xE0)
			break;

		if(*cs==0xE1)
		{
			l=(cs[1]<<16)|(cs[2]<<8)|(cs[3]);
			BTIC3A_MarkerImgData(ctx, cs+4, l-4, obuf, clrs);
			cs+=l; continue;
		}	

		if(*cs==0xE3)
		{
			l=(cs[1]<<16)|(cs[2]<<8)|(cs[3]);

			if((cs[4]=='H') && (cs[5]=='E'))
			{	BTIC2C_MarkerHead(ctx, cs+6, l-6);
				cs+=l; continue; }
			
//			if((cs[4]=='Q') && (cs[5]>='0') && (cs[5]<='9'))
//			{	BTIC2C_MarkerDQT(ctx, cs+6, cs[5]-'0', l-6);
//				cs+=l; continue; }
			if((cs[4]=='H') && (cs[5]>='0') && (cs[5]<='9'))
			{	BTIC3A_MarkerDHT(ctx, cs+6, cs[5]-'0', l-6);
				cs+=l; continue; }
//			if((cs[4]=='S') && (cs[5]=='F'))
//			{	BTIC2C_MarkerSOF0(ctx, cs+6, l-6);
//				cs+=l; continue; }
//			if((cs[4]=='S') && (cs[5]=='S'))
//			{	BTIC2C_MarkerSOS(ctx, cs+6, l-6);
//				cs+=l; continue; }
			cs+=l; continue;
		}

		if(*cs==0xE4)
		{
			l=(cs[1]<<16)|(cs[2]<<8)|(cs[3]);
			cs+=l; continue;
		}

		if(*cs==0xE5)
		{
			l=cs[1];

			if((cs[4]=='H') && (cs[5]=='E'))
			{	BTIC2C_MarkerHead(ctx, cs+4, l-4);
				cs+=l; continue; }

//			if((cs[2]=='Q') && (cs[3]>='0') && (cs[3]<='9'))
//			{	BTIC2C_MarkerDQT(ctx, cs+4, cs[3]-'0', l-4);
//				cs+=l; continue; }
			if((cs[2]=='H') && (cs[3]>='0') && (cs[3]<='9'))
			{	BTIC3A_MarkerDHT(ctx, cs+4, cs[3]-'0', l-4);
				cs+=l; continue; }
//			if((cs[2]=='S') && (cs[3]=='F'))
//			{	BTIC2C_MarkerSOF0(ctx, cs+4, l-4);
//				cs+=l; continue; }
//			if((cs[2]=='S') && (cs[3]=='S'))
//			{	BTIC2C_MarkerSOS(ctx, cs+4, l-4);
//				cs+=l; continue; }
			cs+=l; continue;
		}
	}

	*rxs=ctx->xs;
	*rys=ctx->ys;
	return(0);
}

BTIC3A_ImageCtx *BTIC3A_AllocImageContext()
{
	BTIC3A_ImageCtx *ctx;
	ctx=gctalloc("btic3a_imagectx_t", sizeof(BTIC3A_ImageCtx));
	return(ctx);
}

void BTIC3A_FreeImageContext(BTIC3A_ImageCtx *ctx)
{
	if(ctx->bits)
		{ BTIC3A_FreeBitstream(ctx->bits); }

	if(ctx->blks)
		{ gcfree(ctx->blks); }
	if(ctx->lblks)
		{ gcfree(ctx->lblks); }

	gcfree(ctx);
}
