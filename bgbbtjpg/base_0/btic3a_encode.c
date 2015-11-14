/*
First:
Convert source image into EncoderBlocks;
Quantize Blocks;
Serialize into buffer;
If I-Frame:
	Stat buffers;
	Rebuild and emit Huffman tables.
Entropy Enocode buffers.

*/

#include <bgbbtj.h>

extern const u32 btic2c_dbase[64];
extern const int btic2c_dextra[64];
extern const int btic2c_lbase[64];
extern const int btic2c_lextra[64];

extern const int btic3a_hilbert_idx[16];
extern const int btic3a_hilbert_xy[32];

void BTIC3A_EncodeBlock_SplitMinMaxClrYA_RGBA(
	byte *pxy, byte *pxa,
	int *min_rgba, int *max_rgba, int *avg_rgba,
	int *rmcy, int *rncy, int *racy,
	int *rmca, int *rnca, int *raca,
	byte *rgba, int xstride, int ystride)
{
	int mcr, mcg, mcb, mca, mcy;
	int ncr, ncg, ncb, nca, ncy;
	int acr, acg, acb, aca, acy;
	int cr, cg, cb, ca, cy;
	int i, j, k, l;

	mcr=255; mcg=255; mcb=255; mca=255; mcy=255;
	ncr=  0; ncg=  0; ncb=  0; nca=  0; ncy=  0;
	acr=  0; acg=  0; acb=  0; aca=  0; acy=  0;
	
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		k=(i*ystride)+(j*xstride);
		cr=rgba[k+0];	cg=rgba[k+1];
		cb=rgba[k+2];	ca=rgba[k+3];
		cy=(cr+(2*cg)+cb)>>2;		//Luma

		pxa[i*4+j]=ca;
		pxy[i*4+j]=cy;

//		pxa[j*4+i]=ca;
//		pxy[j*4+i]=cy;

		//Find Mins/Maxs
		if(cy<mcy) { mcr=cr; mcg=cg; mcb=cb; mcy=cy; }
		if(cy>ncy) { ncr=cr; ncg=cg; ncb=cb; ncy=cy; }
		if(ca<mca) { mca=ca; }
		if(ca>nca) { nca=ca; }
		
		acr+=cr; acg+=cg;
		acb+=cb; acy+=cy;
	}
	
	min_rgba[0]=mcr;			min_rgba[1]=mcg;
	min_rgba[2]=mcb;			min_rgba[3]=mca;
	max_rgba[0]=ncr;			max_rgba[1]=ncg;
	max_rgba[2]=ncb;			max_rgba[3]=nca;
	avg_rgba[0]=(mcr+ncr)>>1;	avg_rgba[1]=(mcg+ncg)>>1;
	avg_rgba[2]=(mcb+ncb)>>1;	avg_rgba[3]=(mca+nca)>>1;
	
	*rmcy=mcy; *rncy=ncy; *racy=(mcy+ncy)>>1;
	*rmca=mca; *rnca=nca; *raca=(mca+nca)>>1;
}

void BTIC3A_EncBlock_EncodeRGBA(BTIC3A_EncoderBlock *block,
	byte *rgba, int xstride, int ystride)
{
	byte pxa[16], pxy[16];
	int l0, l1, l2, l3;
	int p0, p1;
	int i, j, k, l;

	BTIC3A_EncodeBlock_SplitMinMaxClrYA_RGBA(
		pxy, pxa, min, max, avg,
		&mcy, &ncy, &acy,
		&mca, &nca, &aca,
		rgba, xstride, ystride);
		
	block->colorA[0]=min[0];
	block->colorA[1]=min[1];
	block->colorA[2]=min[2];
	block->colorA[3]=min[3];

	block->colorB[0]=max[0];
	block->colorB[1]=max[1];
	block->colorB[2]=max[2];
	block->colorB[3]=max[3];

	l0=32768/(nca-aca+1);		//Fix-Point Scale (Alpha)
	l1=32768/(ncy-acy+1);		//Fix-Point Scale (Luma)
	l2=65536;

	ctx->bfmt=0;
	BTIC3A_EncBlock_SetFormat(block, (ncy>mcy)?15:0, (nca>mca)?15:0, 0);
	
	for(i=0; i<8; i++)
	{
		block->alphaBits[i]=0;
		block->colorBits[i]=0;
	}

	for(i=0; i<16; i++)
	{
		p0=((pxa[0]-aca)*l0+l2)>>12;
		if(p0<0)p0=0;
		if(p0>15)p0=15;

		p1=((pxy[0]-acy)*l1+l2)>>12;
		if(p1<0)p1=0;
		if(p1>15)p1=15;
		
		block->alphaBits[i>>1]|=p0<<((i&1)*4);
		block->colorBits[i>>1]|=p1<<((i&1)*4);
	}
}

void BTIC3A_EncBlock_SetFormat(BTIC3A_EncoderBlock *block,
	int cbf, int abf, int cdf)
{
	if(!(abf))
	{
		if(cdf<7)
		{
			block->bfmt=cbf|(cdf<<4);
			return;
		}

		if(cdf<8)
		{
			block->bfmt=0x74|(cbf<<12)|(cdf<<16);
			return;
		}
	
		if(cdf<32)
		{
			block->bfmt=0x78|(cbf<<12)|(cdf<<16);
			return;
		}
	}else
	{
		if(cdf<8)
		{
			block->bfmt=0x76|(abf<<8)|(cbf<<12)|(cdf<<16);
			return;
		}
	
		if(cdf<32)
		{
			block->bfmt=0x7A|(abf<<8)|(cbf<<12)|(cdf<<16);
			return;
		}
	}
}

void BTIC3A_EncBlock_GetFormat(BTIC3A_EncoderBlock *block,
	int *rcbf, int *rabf, int *rcdf)
{
	int fmt;
	int cbf, abf, cdf;

	fmt=block->bfmt;
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

	*rcbf=cbf;
	*rabf=abf;
	*rcdf=cdf;
}

void BTIC3A_EncBlock_DecodeRGBA(BTIC3A_EncoderBlock *block,
	byte *rgba, int xstride, int ystride)
{
	byte clrtab[16*4];
	int cbf, abf, cdf;
	int clra, clrb;
	int l0, l1, l2, l3;
	int i, j, k, l, nc, na, bc, ba;

//	cbf=(fmt>>12)&15;
//	cdf=(fmt>>16)&255;
//	abf=(fmt>>8)&15;
	BTIC3A_EncBlock_GetFormat(block, &cbf, &abf, &cdf);

	if((cbf&0xC))
	{
		bc=((cbf&0x3)+1);
		nc=1<<bc;
		for(i=0; i<nc; i++)
		{
			clrtab[i*4+0]=(((nc-i-1)*block->colorA[0])+
				(i*block->colorB[0]))/(nc-1);
			clrtab[i*4+1]=(((nc-i-1)*block->colorA[1])+
				(i*block->colorB[1]))/(nc-1);
			clrtab[i*4+2]=(((nc-i-1)*block->colorA[2])+
				(i*block->colorB[2]))/(nc-1);
		}

		if((cbf&0xC)==0xC)
		{
			for(i=0; i<4; i++)
				for(j=0; j<4; j++)
			{
				l=i*4+j;
				k=(block->colorBits[l>>1]>>((i&1)*4))&(nc-1);
				l=i*ystride+j*xstride;
				rgba[l+0]=clrtab[k*4+0];
				rgba[l+1]=clrtab[k*4+1];
				rgba[l+2]=clrtab[k*4+2];
			}
		}else if((cbf&0xC)==0x8)
		{
			for(i=0; i<2; i++)
				for(j=0; j<2; j++)
			{
				l=i*2+j;
				k=(block->colorBits[l>>1]>>((i&1)*4))&(nc-1);
				l0=(i*2+0)*ystride+(j*2+0)*xstride;
				l1=(i*2+0)*ystride+(j*2+1)*xstride;
				l2=(i*2+1)*ystride+(j*2+0)*xstride;
				l3=(i*2+1)*ystride+(j*2+1)*xstride;
				rgba[l0+0]=clrtab[k*4+0]; rgba[l1+0]=clrtab[k*4+0];
				rgba[l2+0]=clrtab[k*4+0]; rgba[l3+0]=clrtab[k*4+0];
				rgba[l0+1]=clrtab[k*4+1]; rgba[l1+1]=clrtab[k*4+1];
				rgba[l2+1]=clrtab[k*4+1]; rgba[l3+1]=clrtab[k*4+1];
				rgba[l0+2]=clrtab[k*4+2]; rgba[l1+2]=clrtab[k*4+2];
				rgba[l2+2]=clrtab[k*4+2]; rgba[l3+2]=clrtab[k*4+2];
			}
		}else
		{
			for(i=0; i<4; i++)
				for(j=0; j<4; j++)
			{
				k=block->colorBits[0]&3;
				l=i*ystride+j*xstride;
				rgba[l+0]=clrtab[k*4+0];
				rgba[l+1]=clrtab[k*4+1];
				rgba[l+2]=clrtab[k*4+2];
			}
		}
	}else
	{
		bc=2; nc=4;
		for(i=0; i<4; i++)
		{
			clrtab[i*4+0]=(((3-i)*block->colorA[0])+
				(i*block->colorB[0]))/3;
			clrtab[i*4+1]=(((3-i)*block->colorA[1])+
				(i*block->colorB[1]))/3;
			clrtab[i*4+2]=(((3-i)*block->colorA[2])+
				(i*block->colorB[2]))/3;
		}
		
		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
		{
			k=block->colorBits[0]&3;
			l=i*ystride+j*xstride;
			rgba[l+0]=clrtab[k*4+0];
			rgba[l+1]=clrtab[k*4+1];
			rgba[l+2]=clrtab[k*4+2];
		}
	}

	if((abf&0xC))
	{
		ba=((abf&0x3)+1);
		na=1<<ba;
		for(i=0; i<na; i++)
		{
			clrtab[i*4+3]=(((nc-i-1)*block->colorA[3])+
				(i*block->colorB[3]))/(nc-1);
		}

		if((abf&0xC)==0xC)
		{
			for(i=0; i<4; i++)
				for(j=0; j<4; j++)
			{
				l=i*4+j;
				k=(block->alphaBits[l>>1]>>((i&1)*4))&(na-1);
				l=i*ystride+j*xstride;
				rgba[l+3]=clrtab[k*4+3];
			}
		}else if((abf&0xC)==0x8)
		{
			for(i=0; i<2; i++)
				for(j=0; j<2; j++)
			{
				l=i*2+j;
				k=(block->alphaBits[l>>1]>>((i&1)*4))&(nc-1);
				l0=(i*2+0)*ystride+(j*2+0)*xstride;
				l1=(i*2+0)*ystride+(j*2+1)*xstride;
				l2=(i*2+1)*ystride+(j*2+0)*xstride;
				l3=(i*2+1)*ystride+(j*2+1)*xstride;
				rgba[l0+3]=clrtab[k*4+3]; rgba[l1+3]=clrtab[k*4+3];
				rgba[l2+3]=clrtab[k*4+3]; rgba[l3+3]=clrtab[k*4+3];
			}
		}else
		{
			for(i=0; i<4; i++)
				for(j=0; j<4; j++)
			{
				k=block->alphaBits[0]&3;
				l=i*ystride+j*xstride;
				rgba[l+3]=clrtab[k*4+3];
			}
		}
	}else
	{
		bc=2; nc=4;
		for(i=0; i<4; i++)
		{
			clrtab[i*4+3]=(((3-i)*block->colorA[3])+
				(i*block->colorB[3]))/3;
		}

		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
		{
			k=block->alphaBits[0]&3;
			l=i*ystride+j*xstride;
			rgba[l+3]=clrtab[k*4+3];
		}
	}
}

void BTIC3A_EncBlock_CalcBlockRMSE(
	BTIC3A_EncoderBlock *blocka,
	BTIC3A_EncoderBlock *blockb)
{
	byte pxa[16*4];
	byte pxb[16*4];
	float f, g;
	int i, j, k;
	
	BTIC3A_EncBlock_DecodeRGBA(blocka, pxa, 4, 4*4);
	BTIC3A_EncBlock_DecodeRGBA(blockb, pxb, 4, 4*4);
	
	f=0;
	for(i=0; i<(16*4); i++)
	{
		j=pxa[i]-pxb[i];
		f+=j*j;
	}
	f=sqrt(f/(16*4));
	return(f);
}

void BTIC3A_Encode_TransformFromRGBA(BTIC3A_ImageCtx *ctx,
	byte *rgba, int xs, int ys, int clrs)
{
	int xs1, ys1, xs2, ys2;
	int i, j, k, l;

	ctx->xs=xs;
	ctx->ys=ys;

	xs1=(xs+15)/16;
	ys1=(ys+15)/16;
	ctx->xsmb=xs1;
	ctx->ysmb=ys1;

	xs2=xs1*4;
	ys2=ys1*4;
	ctx->xsb=xs2;
	ctx->ysb=ys2;

	if(!ctx->encBlks)
		{ ctx->encBlks=gcatomic(xs2*ys2*sizeof(BTIC3A_EncoderBlock)); }

	for(i=0; i<ys2; i++)
		for(j=0; j<xs2; j++)
	{
		BTIC3A_EncBlock_EncodeRGBA(
			ctx->encBlks+(i*xs2+j),
			rgba+(i*4*xs+j*4)*stride,
			4, xs*4);
	}
}

int BTIC3A_Encode_CheckDeltaBits(int d, int b)
{
	int i;
	
	if(!b)
		{ return(!d); }
	
	i=d+(1<<(b-1));
	if((i<0) || (i>=(1<<b)))
		return(0);
	return(1);
}

int BTIC3A_Encode_CheckDeltaBits2(int da, int db, int b)
{
	if(!BTIC3A_Encode_CheckDeltaBits(da, b))
		return(0);
	if(!BTIC3A_Encode_CheckDeltaBits(db, b))
		return(0);
	return(1);
}

int BTIC3A_Encode_CheckDeltaBitsRGB(
	int dr, int dg, int db,
	int br, int bg, int bb)
{
	if(!BTIC3A_Encode_CheckDeltaBits(dr, br))
		return(0);
	if(!BTIC3A_Encode_CheckDeltaBits(dg, bg))
		return(0);
	if(!BTIC3A_Encode_CheckDeltaBits(db, bb))
		return(0);
	return(1);
}

int BTIC3A_Encode_CheckDeltaBitsRGB2(
	int dra, int dga, int dba,
	int drb, int dgb, int dbb,
	int br, int bg, int bb)
{
	if(!BTIC3A_Encode_CheckDeltaBits(dra, br))
		return(0);
	if(!BTIC3A_Encode_CheckDeltaBits(dga, bg))
		return(0);
	if(!BTIC3A_Encode_CheckDeltaBits(dba, bb))
		return(0);
	if(!BTIC3A_Encode_CheckDeltaBits(drb, br))
		return(0);
	if(!BTIC3A_Encode_CheckDeltaBits(dgb, bg))
		return(0);
	if(!BTIC3A_Encode_CheckDeltaBits(dbb, bb))
		return(0);
	return(1);
}

int BTIC3A_Encode_CheckDeltaBitsRGBA2(
	int dra, int dga, int dba, int daa,
	int drb, int dgb, int dbb, int dab,
	int br, int bg, int bb, int ba)
{
	if(!BTIC3A_Encode_CheckDeltaBits(dra, br))
		return(0);
	if(!BTIC3A_Encode_CheckDeltaBits(dga, bg))
		return(0);
	if(!BTIC3A_Encode_CheckDeltaBits(dba, bb))
		return(0);
	if(!BTIC3A_Encode_CheckDeltaBits(daa, ba))
		return(0);
	if(!BTIC3A_Encode_CheckDeltaBits(drb, br))
		return(0);
	if(!BTIC3A_Encode_CheckDeltaBits(dgb, bg))
		return(0);
	if(!BTIC3A_Encode_CheckDeltaBits(dbb, bb))
		return(0);
	if(!BTIC3A_Encode_CheckDeltaBits(dab, ba))
		return(0);
	return(1);
}

int BTIC3A_Encode_CheckDeltaBitsYA2(
	int dya, int daa,
	int dyb, int dab,
	int by, int ba)
{
	if(!BTIC3A_Encode_CheckDeltaBits(dya, by))
		return(0);
	if(!BTIC3A_Encode_CheckDeltaBits(daa, ba))
		return(0);
	if(!BTIC3A_Encode_CheckDeltaBits(dyb, by))
		return(0);
	if(!BTIC3A_Encode_CheckDeltaBits(dab, ba))
		return(0);
	return(1);
}

void BTIC3A_Encode_EncodeImageBlockHeader(BTIC3A_ImageCtx *ctx,
	BTIC3A_EncoderBlock *block)
{
	s16 cda[4], cdb[4];
	int fmt;
	int cbf, abf, cdf;

	BTIC3A_EncBlock_GetFormat(block, &cbf, &abf, &cdf);

	cda[0]=block->colorA[0]-ctx->clra[0];
	cda[1]=block->colorA[1]-ctx->clra[1];
	cda[2]=block->colorA[2]-ctx->clra[2];
	cda[3]=block->colorA[3]-ctx->clra[3];

	cdb[0]=block->colorB[0]-ctx->clrb[0];
	cdb[1]=block->colorB[1]-ctx->clrb[1];
	cdb[2]=block->colorB[2]-ctx->clrb[2];
	cdb[3]=block->colorB[3]-ctx->clrb[3];
	
	cda[0]=(cda[0]+(ctx->clrsc[0]>>1))/ctx->clrsc[0];
	cda[1]=(cda[1]+(ctx->clrsc[1]>>1))/ctx->clrsc[1];
	cda[2]=(cda[2]+(ctx->clrsc[2]>>1))/ctx->clrsc[2];
	cda[3]=(cda[3]+(ctx->clrsc[3]>>1))/ctx->clrsc[3];

	cdb[0]=(cdb[0]+(ctx->clrsc[0]>>1))/ctx->clrsc[0];
	cdb[1]=(cdb[1]+(ctx->clrsc[1]>>1))/ctx->clrsc[1];
	cdb[2]=(cdb[2]+(ctx->clrsc[2]>>1))/ctx->clrsc[2];
	cdb[3]=(cdb[3]+(ctx->clrsc[3]>>1))/ctx->clrsc[3];

	block->dclrA[0]=cda[0];
	block->dclrA[1]=cda[1];
	block->dclrA[2]=cda[2];
	block->dclrA[3]=cda[3];

	block->dclrB[0]=cdb[0];
	block->dclrB[1]=cdb[1];
	block->dclrB[2]=cdb[2];
	block->dclrB[3]=cdb[3];

	if(!cda[3] && !cdb[3])
	{
		if((dra[0]==dra[1]) && (dra[1]==dra[2]) &&
			(drb[0]==drb[1]) && (drb[1]==drb[2]))
		{
			if(BTIC3A_Encode_CheckDeltaBits2(dra[1], drb[1], 0))
				{ cdf=0; }
			else if(BTIC3A_Encode_CheckDeltaBits2(dra[1], drb[1], 3))
				{ cdf=5; }
			else if(BTIC3A_Encode_CheckDeltaBits2(dra[1], drb[1], 5))
				{ cdf=6; }
			else if(BTIC3A_Encode_CheckDeltaBits2(dra[1], drb[1], 8))
				{ cdf=7; }
			else { cdf=26; }
		}else
		{
			if(BTIC3A_Encode_CheckDeltaBitsRGB2(
				cda[0], cda[1], cda[2], cdb[0], cdb[1], cdb[2], 0, 0, 0))
					{ cdf=0; }
			else if(BTIC3A_Encode_CheckDeltaBitsRGB2(
				cda[0], cda[1], cda[2], cdb[0], cdb[1], cdb[2], 2, 3, 2))
					{ cdf=1; }
			else if(BTIC3A_Encode_CheckDeltaBitsRGB2(
				cda[0], cda[1], cda[2], cdb[0], cdb[1], cdb[2], 3, 4, 3))
					{ cdf=2; }
			else if(BTIC3A_Encode_CheckDeltaBitsRGB2(
				cda[0], cda[1], cda[2], cdb[0], cdb[1], cdb[2], 5, 6, 5))
					{ cdf=3; }
			else if(BTIC3A_Encode_CheckDeltaBitsRGB2(
				cda[0], cda[1], cda[2], cdb[0], cdb[1], cdb[2], 8, 8, 8))
					{ cdf=4; }
			else { cdf=24; }
		}
	}else
	{
		if((dra[0]==dra[1]) && (dra[1]==dra[2]) &&
			(drb[0]==drb[1]) && (drb[1]==drb[2]))
		{
			if(BTIC3A_Encode_CheckDeltaBitsYA2(
					dra[1], dra[3], drb[1], drb[3], 0, 0))
				{ cdf=0; }
			else if(BTIC3A_Encode_CheckDeltaBitsYA2(
					dra[1], dra[3], drb[1], drb[3], 2, 2))
				{ cdf=12; }
			else if(BTIC3A_Encode_CheckDeltaBitsYA2(
					dra[1], dra[3], drb[1], drb[3], 3, 3))
				{ cdf=13; }
			else if(BTIC3A_Encode_CheckDeltaBitsYA2(
					dra[1], dra[3], drb[1], drb[3], 5, 5))
				{ cdf=14; }
			else if(BTIC3A_Encode_CheckDeltaBitsYA2(
					dra[1], dra[3], drb[1], drb[3], 8, 8))
				{ cdf=15; }
			else { cdf=27; }
		}else
		{
			if(BTIC3A_Encode_CheckDeltaBitsRGBA2(
				cda[0], cda[1], cda[2], cda[3],
				cdb[0], cdb[1], cdb[2], cdb[3],
				0, 0, 0, 0))
					{ cdf=0; }
			else if(BTIC3A_Encode_CheckDeltaBitsRGBA2(
				cda[0], cda[1], cda[2], cda[3],
				cdb[0], cdb[1], cdb[2], cdb[3],
				2, 3, 2, 3))
					{ cdf=8; }
			else if(BTIC3A_Encode_CheckDeltaBitsRGBA2(
				cda[0], cda[1], cda[2], cda[3],
				cdb[0], cdb[1], cdb[2], cdb[3],
				3, 4, 3, 4))
					{ cdf=9; }
			else if(BTIC3A_Encode_CheckDeltaBitsRGBA2(
				cda[0], cda[1], cda[2], cda[3],
				cdb[0], cdb[1], cdb[2], cdb[3],
				5, 6, 5, 6))
					{ cdf=10; }
			else if(BTIC3A_Encode_CheckDeltaBitsRGBA2(
				cda[0], cda[1], cda[2], cda[3],
				cdb[0], cdb[1], cdb[2], cdb[3],
				8, 8, 8, 8))
					{ cdf=11; }
			else { cdf=25; }
		}
	}

	BTIC3A_EncBlock_SetFormat(block, cbf, abf, cdf);

	fmt=block->bfmt;
	if((fmt&0x70)==0x70)
	{
		BTIC3AH_EncodeSymbol(ctx->bits, 0, fmt&255);

		cbf=(fmt>>12)&15;
		cdf=(fmt>>16)&255;

		switch((fmt>>2)&3)
		{
		case 0:
			break;
		case 1:
			BTIC3AH_Write3Bits(ctx, cdf&7);
			break;
		case 2:
			BTIC3AH_Write5Bits(ctx, cdf&31);
			break;
		default:
			break;
		}

		switch(fmt&3)
		{
		case 0:
			abf=3;
			BTIC3AH_Write4Bits(ctx, (fmt>>12)&15);
			break;
		case 1:
			abf=((fmt>>12)&12)|((fmt>>10)&3);
			BTIC3AH_Write6Bits(ctx, (fmt>>10)&63);
			break;
		case 2:
			BTIC3AH_Write8Bits(ctx, (fmt>>8)&255);
			abf=(fmt>>8)&15;
			break;
		default: break;
		}
	}else
	{
		BTIC3AH_EncodeSymbol(ctx->bits, 0, fmt&255);
		cbf=fmt&15;
		abf=3;
		cdf=(fmt>>4)&7;
	}
}

void BTIC3A_Encode_EncodeImageBlockData(BTIC3A_ImageCtx *ctx,
	BTIC3A_EncoderBlock *block)
{
	int fmt;
	int cbf, abf, cdf;
	int l0, l1, l2, l3;
	int i, j, k, l, nc, na, bc, ba;

	BTIC3A_EncBlock_GetFormat(block, &cbf, &abf, &cdf);

	if((cbf&0xC))
	{
		bc=((cbf&0x3)+1);
		nc=1<<bc;

		if((cbf&0xC)==0xC)
		{
			for(i=0; i<4; i++)
				for(j=0; j<4; j++)
			{
				l=i*4+j;
				k=(block->colorBits[l>>1]>>((i&1)*4))&(nc-1);
				BTIC3AH_WriteNBits(ctx, k, bc);
			}
		}else if((cbf&0xC)==0x8)
		{
			for(i=0; i<2; i++)
				for(j=0; j<2; j++)
			{
				l=i*2+j;
				k=(block->colorBits[l>>1]>>((i&1)*4))&(nc-1);
				BTIC3AH_WriteNBits(ctx, k, bc);
			}
		}else
		{
			k=block->colorBits[0]&(nc-1);
			BTIC3AH_WriteNBits(ctx, k, bc);
		}
	}

	if((abf&0xC))
	{
		ba=((abf&0x3)+1);
		na=1<<ba;

		if((abf&0xC)==0xC)
		{
			for(i=0; i<4; i++)
				for(j=0; j<4; j++)
			{
				l=i*4+j;
				k=(block->alphaBits[l>>1]>>((i&1)*4))&(na-1);
				BTIC3AH_WriteNBits(ctx, k, bc);
			}
		}else if((abf&0xC)==0x8)
		{
			for(i=0; i<2; i++)
				for(j=0; j<2; j++)
			{
				l=i*2+j;
				k=(block->alphaBits[l>>1]>>((i&1)*4))&(nc-1);
				BTIC3AH_WriteNBits(ctx, k, bc);
			}
		}else
		{
			k=block->alphaBits[0]&3;
			BTIC3AH_WriteNBits(ctx, k, bc);
		}
	}
}

void BTIC3A_Encode_EncodeImageBlocks(BTIC3A_ImageCtx *ctx,
	byte *rgba, int xs, int ys, int clrs)
{
}