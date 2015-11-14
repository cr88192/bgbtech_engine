#include <bgbbtj.h>

extern byte btic2b_zigzag[16];
extern byte btic2b_zigzag2[16];

void BTIC2B_TransIDCT4_Horiz(short *iblk, int *oblk)
{
	int ib0, ib1, ib2, ib3;
	int a, b, c, d;

	ib0=iblk[0];	ib1=iblk[1];
	ib2=iblk[2];	ib3=iblk[3];

	a=(ib0+ib2)*128;	b=(ib0-ib2)*128;
	c=ib1*167+ib3* 69;	d=ib1* 69-ib3*167;

	oblk[0]=a+c;
	oblk[1]=b+d;
	oblk[2]=b-d;
	oblk[3]=a-c;
}

void BTIC2B_TransIDCT4_Vert(int *iblk, int *oblk)
{
	int ib0, ib1, ib2, ib3;
	int a, b, c, d;

	ib0=iblk[0];	ib1=iblk[ 4];
	ib2=iblk[8];	ib3=iblk[12];

	a=(ib0+ib2)*128;	b=(ib0-ib2)*128;
	c=ib1*167+ib3* 69;	d=ib1* 69-ib3*167;

	oblk[ 0]=a+c;
	oblk[ 4]=b+d;
	oblk[ 8]=b-d;
	oblk[12]=a-c;
}

void BTIC2B_TransIDCT4(short *iblk, byte *oblk)
{
	int s[BTIC2B_DCTSZ2];
	int t[BTIC2B_DCTSZ2];
	int i, j;

	BTIC2B_TransIDCT4_Horiz(iblk+ 0, s+ 0);
	BTIC2B_TransIDCT4_Horiz(iblk+ 4, s+ 4);
	BTIC2B_TransIDCT4_Horiz(iblk+ 8, s+ 8);
	BTIC2B_TransIDCT4_Horiz(iblk+12, s+12);

	BTIC2B_TransIDCT4_Vert(s+0, t+0);
	BTIC2B_TransIDCT4_Vert(s+1, t+1);
	BTIC2B_TransIDCT4_Vert(s+2, t+2);
	BTIC2B_TransIDCT4_Vert(s+3, t+3);

	for(i=0; i<16; i++)
	{
		j=(t[i]>>16);
		oblk[i]=(j<0)?0:((j>255)?255:j);
	}
}

//Decoder

byte *BTIC2B_DecodeVLI(byte *cs, u64 *rval)
{
	int i;
	
	if(!cs)return(NULL);
	
	i=*cs++;
	if(!(i&0x80))
	{
		*rval=i; return(cs);
	}else if((i&0xC0)==0x80)
	{
		i=((i&0x3F)<<8)|(*cs++);
		*rval=i; return(cs);
	}else if((i&0xE0)==0xC0)
	{
		i=((i&0x1F)<<8)|(*cs++);
		i=(i<<8)|(*cs++);
		*rval=i; return(cs);
	}else if((i&0xF0)==0xE0)
	{
		i=((i&0x0F)<<8)|(*cs++);
		i=(i<<8)|(*cs++);
		i=(i<<8)|(*cs++);
		*rval=i; return(cs);
	}else if((i&0xF8)==0xF0)
	{
		i=((i&0x0F)<<8)|(*cs++);
		i=(i<<8)|(*cs++);
		i=(i<<8)|(*cs++);
		i=(i<<8)|(*cs++);
		*rval=i; return(cs);
	}
	
//	*(int *)-1=-1;
	*rval=0;
//	return(cs);
	return(NULL);
}

byte *BTIC2B_DecodeSVLI(byte *cs, s64 *rval)
{
	u64 li;
	s64 i;
	
	cs=BTGE_Voxel_DecodeVLI(cs, &li);
	
	i=(li&1)?(-(s64)((li+1)>>1)):((s64)(li>>1));
	
	*rval=i;
	return(cs);
}

u64 BTIC2BH_ReadVLI(BTIC2B_ImageContext *ctx)
{
	u64 v;
	
	ctx->cs=BTIC2B_DecodeVLI(ctx->cs, &v);
	return(v);
}

s64 BTIC2BH_ReadSVLI(BTIC2B_ImageContext *ctx)
{
	s64 v;
	
	ctx->cs=BTIC2B_DecodeSVLI(ctx->cs, &v);
	return(v);
}

u32 BTIC2BH_ReadVLT(BTIC2B_ImageContext *ctx)
{
	u32 t;
	int i;
	
	i=*ctx->cs++;
	if(!(i&0x80))
	{
		t=i;
	}else if((i&0xC0)==0x80)
	{
		t=(*ctx->cs++);
	}else if((i&0xE0)==0xC0)
	{
		t=*ctx->cs++;
		t|=(*ctx->cs++)<<8;
	}else if((i&0xF0)==0xE0)
	{
		t=*ctx->cs++;
		t|=(*ctx->cs++)<<8;
		t|=(*ctx->cs++)<<16;
	}else if((i&0xF8)==0xF0)
	{
		t=*ctx->cs++;
		t|=(*ctx->cs++)<<8;
		t|=(*ctx->cs++)<<16;
		t|=(*ctx->cs++)<<24;
	}

	return(t);
}

void BTIC2BH_PushSetInput(BTIC2B_ImageContext *ctx, byte *buf, int sz)
{
	int i;
	
	i=ctx->huff_stk_pos++;
	ctx->huff_stk_ct[i]=ctx->cs;
	ctx->huff_stk_cts[i]=ctx->css;
	ctx->huff_stk_cte[i]=ctx->cse;
	
	ctx->css=buf;
	ctx->cse=buf+sz;
	ctx->cs=buf;
}

int BTIC2BH_PopSetInput(BTIC2B_ImageContext *ctx)
{
	int sz;

	i=--ctx->huff_stk_pos;
	ctx->cs=ctx->huff_stk_ct[i];
	ctx->css=ctx->huff_stk_cts[i];
	ctx->cse=ctx->huff_stk_cte[i];
}

int BTIC2BH_NextByte(BTIC2B_ImageContext *ctx)
{
	int i, j;

	i=*btic2bh_cs++;
	return(i);
}

int BTIC2BH_InitReadBytes(BTIC2B_ImageContext *ctx, byte *buf, int sz)
{
	btic2bh_cs=buf;
	btic2bh_css=buf;
	btic2bh_cse=buf+sz;
	
	return(0);
}

int BTIC2BH_InitStream(BTIC2B_ImageContext *ctx, byte *buf, int sz)
{
	btic2bh_pos=0;
	btic2bh_isend=0;

	btic2bh_cs=buf;
	btic2bh_css=buf;
	btic2bh_cse=buf+sz;

	btic2bh_win=BTIC2BH_NextByte()<<24;
	btic2bh_win=(btic2bh_win>>8)|(BTIC2BH_NextByte(ctx)<<24);
	btic2bh_win=(btic2bh_win>>8)|(BTIC2BH_NextByte(ctx)<<24);
	btic2bh_win=(btic2bh_win>>8)|(BTIC2BH_NextByte(ctx)<<24);
	
	return(0);
}

int BTIC2BH_ReadBit(BTIC2B_ImageContext *ctx)
{
	int i;

	i=(btic2bh_win>>btic2bh_pos)&1;
	btic2bh_pos++;
	if(btic2bh_pos>=8)
	{
		btic2bh_win=(btic2bh_win>>8)|(BTIC2BH_NextByte(ctx)<<24);
		btic2bh_pos-=8;
	}
	return(i);
}

int BTIC2BH_Read2Bits(BTIC2B_ImageContext *ctx)
{
	int i;
	i=(ctx->bit_win>>ctx->bit_pos)&3;
	ctx->bit_pos+=2;
	if(ctx->bit_pos>=8)
	{
		ctx->bit_win=(ctx->bit_win>>8)|(BTIC2BH_ReadCodedByte(ctx)<<24);
		ctx->bit_pos-=8;
	}
	return(i);
}

int BTIC2BH_Read3Bits(BTIC2B_ImageContext *ctx)
{
	int i;
	i=(ctx->bit_win>>ctx->bit_pos)&7;
	ctx->bit_pos+=3;
	if(ctx->bit_pos>=8)
	{
		ctx->bit_win=(ctx->bit_win>>8)|(BTIC2BH_ReadCodedByte(ctx)<<24);
		ctx->bit_pos-=8;
	}
	return(i);
}

int BTIC2BH_Read4Bits(BTIC2B_ImageContext *ctx)
{
	int i;
	i=(ctx->bit_win>>ctx->bit_pos)&15;
	ctx->bit_pos+=4;
	if(ctx->bit_pos>=8)
	{
		ctx->bit_win=(ctx->bit_win>>8)|(BTIC2BH_ReadCodedByte(ctx)<<24);
		ctx->bit_pos-=8;
	}
	return(i);
}

int BTIC2BH_Read5Bits(BTIC2B_ImageContext *ctx)
{
	int i;
	i=(ctx->bit_win>>ctx->bit_pos)&31;
	ctx->bit_pos+=5;
	if(ctx->bit_pos>=8)
	{
		ctx->bit_win=(ctx->bit_win>>8)|(BTIC2BH_ReadCodedByte(ctx)<<24);
		ctx->bit_pos-=8;
	}
	return(i);
}

int BTIC2BH_ReadNBits(BTIC2B_ImageContext *ctx, int n)
{
	int i;

	i=(btic2bh_win>>btic2bh_pos)&((1<<n)-1);
	btic2bh_pos+=n;
	while(btic2bh_pos>=8)
	{
		btic2bh_win=(btic2bh_win>>8)|(BTIC2BH_NextByte(ctx)<<24);
		btic2bh_pos-=8;
	}
	return(i);
}

u64 BTIC2BH_ReadNBitsL(BTIC2B_ImageContext *ctx, int n)
{
	u64 v;
	int i, j;
	
	v=0; i=n; j=0;
	while(i>16)
	{
		v|=((u64)BTIC2BH_ReadNBits(ctx, 16))<<j;
		i-=16; j+=16;
	}
	v|=((u64)BTIC2BH_ReadNBits(ctx, i))<<j;
	return(v);
}

void BTIC2BH_SkipNBits(BTIC2B_ImageContext *ctx, int n)
{
	btic2bh_pos+=n;
	while(btic2bh_pos>=8)
	{
		btic2bh_win=(btic2bh_win>>8)|(BTIC2BH_NextByte(ctx)<<24);
		btic2bh_pos-=8;
	}
}

int BTIC2BH_ReadRice(BTIC2B_ImageContext *ctx, int n)
{
	int i, j;
	
	i=0;
	while(BTIC2BH_ReadBit(ctx))i++;
	j=BTIC2BH_ReadNBits(ctx, n);
	return((i<<n)|j);
}

int BTIC2BH_PeekWord(BTIC2B_ImageContext *ctx)
{
	int i;
	i=(btic2bh_win>>btic2bh_pos)&65535;
	return(i);
}

int BTIC2BH_DecodeSymbol(BTIC2B_ImageContext *ctx, int tab)
{
	int i, j, k;

	tab<<=8;

	i=BTIC2BH_PeekWord(ctx);
	for(j=0; j<256; j++)
	{
		k=btic2bh_len[tab|j];
		if(!k)continue;

		if((i&((1<<k)-1))!=btic2bh_code[tab|j])
			continue;
		BTIC2BH_SkipNBits(ctx, k);
		return(j);
	}
	return(-1);
}


int BTIC2BH_DecodeHuffTableFixed(BTIC2BH_ImageContext *ctx,
	int tab, int tn)
{
	byte lcl[288];
	int hl;
	int i, j;

	switch(tn)
	{
	case 0:
		hl=256;
		for(i=0; i<256; i++)lcl[i]=8;
		break;
	case 1:
		hl=288;
		for(i=0; i<144; i++)lcl[i]=8;
		for(i=144; i<256; i++)lcl[i]=9;
		for(i=256; i<280; i++)lcl[i]=7;
		for(i=280; i<288; i++)lcl[i]=8;
		break;
	case 2:
		hl=256;
		for(i=0; i<16; i++)lcl[i]=5;
		for(i=16; i<32; i++)lcl[i]=6;
		for(i=32; i<64; i++)lcl[i]=8;
		for(i=64; i<128; i++)lcl[i]=9;
		for(i=128; i<256; i++)lcl[i]=11;
		break;
	default:
		hl=256;
		for(i=0; i<256; i++)lcl[i]=8;
		break;
	}

	j=BTIC2BH_SetupTable(lcl, hl,
		ctx->huff_code[tab], ctx->huff_mask[tab], ctx->huff_len[tab],
		ctx->huff_idx[tab], ctx->huff_next[tab]);
	if(j<0)
	{
		dyPrintf("Static Literal/Length Table Problem "
			"Tab=%d, Tn=%d, Code=%d\n", tab, tn, j);
		return(j);
	}

	return(0);
}

int BTIC2BH_DecodeCodeLengths(BTIC2B_ImageContext *ctx, byte *cl, int ncl)
{
	int i, j, k;

	for(i=0; i<ncl; i++)cl[i]=0;

	i=0; k=0;
	while(i<ncl)
	{
		j=BTIC2BH_DecodeHufftabSymbol(ctx);
		if(j<0)return(j);

		if(j<17)
			{ k=j; cl[i++]=j; continue; }

		if(j==17)
		{
			j=BTIC2BH_Read3Bits(ctx)+3;
			while(j--)cl[i++]=k;
			continue;
		}
		if(j==18)
		{
			j=BTIC2BH_ReadNBits(ctx, 7)+11;
			while(j--)cl[i++]=k;
			continue;
		}
		if(j==19)break;

		return(-7);
	}
	return(0);
}

int BTIC2BH_DecodeHuffTable(BTIC2B_ImageContext *ctx, int tab)
{
	byte lcl[512];
	int ht, hk, hl;
	int i, j, k;

	ht=BTIC2BH_ReadNBits(ctx, 4);
	hk=BTIC2BH_ReadNBits(ctx, 4);
	
	if(ht==0)
	{
		i=BTIC2BH_DecodeHuffTableFixed(ctx, tab, hk);
		return(i);
	}

	if(ht!=1)
	{
		dyPrintf("Bad Huffman Table Type %d\n", ht);
		return(-1);
	}

	BTIC2BH_DecodeCodeLengths(ctx, lcl, 256);
	j=BTIC2BH_SetupTable(lcl, 256,
		ctx->huff_code[tab], ctx->huff_mask[tab], ctx->huff_len[tab],
		ctx->huff_idx[tab], ctx->huff_next[tab]);
	if(j<0)
	{
		dyPrintf("Decode Dynamic Literal/Length Table Problem "
			"Tab=%d, Code=%d\n", tab, j);
		return(j);
	}

	return(0);
}



int BTIC2BH_DecodeCoeffUValueBits(
	BTIC2B_ImageContext *ctx, int sym)
{
	static u32 dbase[]={
		   0,    1,    2,     3,     4,     6,     8,    12,
		  16,   24,   32,    48,    64,    96,   128,   192,
		 256,  384,  512,   768,  1024,  1536,  2048,  3072,
		4096, 6144, 8192, 12288, 16384, 24576, 32768, 49152,
		//(32-bit range)
		0x00001000, 0x00001800, 0x00002000, 0x00003000,
		0x00004000, 0x00006000, 0x00008000, 0x0000C000,
		0x00010000, 0x00018000, 0x00020000, 0x00030000,
		0x00040000, 0x00060000, 0x00080000, 0x000C0000,
		0x00100000, 0x00180000, 0x00200000, 0x00300000,
		0x00400000, 0x00600000, 0x00800000, 0x00C00000,
		0x01000000, 0x01800000, 0x02000000, 0x03000000,
		0x04000000, 0x06000000, 0x08000000, 0x0C000000,
		0x10000000, 0x18000000, 0x20000000, 0x30000000,
		0x40000000, 0x60000000, 0x80000000, 0xC0000000,
		};
	static int dextra[64]={
		 0,  0,  0,  0,  1,  1,  2,  2,
		 3,  3,  4,  4,  5,  5,  6,  6,
		 7,  7,  8,  8,  9,  9, 10, 10,
		11, 11, 12, 12, 13, 13, 14, 14,
		//(32-bit range)
		15, 15, 16, 16, 17, 17, 18, 18,
		19, 19, 20, 20, 21, 21, 22, 22,
		23, 23, 24, 24, 25, 25, 26, 26,
		27, 27, 28, 28, 29, 29, 30, 30
		};

	int i, j;
	i=dbase[sym]+BSXRP_ReadNBits(ctx, dextra[sym]);
	return(i);
}

int BTIC2BH_DecodeCoeffValueBits(
	BTIC2B_ImageContext *ctx, int sym)
{
	int i;
	i=BTIC2BH_DecodeCoeffUValueBits(ctx, sym);
	i=(i&1)?(-((i+1)>>1)):(i>>1);
	return(i);
}

int BTIC2BH_DecodeTagBits(
	BTIC2B_ImageContext *ctx, int sym)
{
	static int lbase[32]={
		  0,   1,   2,   3,   4,   5,   6,   7,
		  8,  10,  12,  14,  16,  20,  24,  28,
		 32,  40,  48,  56,  64,  80,  96, 112,
		128, 160, 192, 224, 256, 320, 384, 448};
	static int lextra[32]={
		0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2,
		3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6};

	int i, j;
	i=lbase[sym]+BSXRP_ReadNBits(ctx, lextra[sym]);
	return(i);
}

int BTIC2BH_DecodeDiffDC(BTIC2B_ImageContext *ctx, int tab)
{
	int i, j;

	i=BTIC2BH_DecodeSymbol(tab);
	j=BTIC2BH_DecodeCoeffValueBits(ctx, i&31);
	return(j);
}

int BTIC2BH_DecodeBlock(BTIC2B_ImageContext *ctx,
	short *buf, int dctab, int actab,
	int ni, int nn)
{
	int i, j, k;

	for(i=1; i<16; i++)buf[i]=0;

	buf[0]+=BTIC2BH_DecodeDiffDC(dctab);
	for(i=1; i<16; i++)
	{
		j=BTIC2BH_DecodeSymbol(actab);
		if(!j)break;

		k=j>>5;
		if(k>=6)
		{
			i+=BTIC2BH_DecodeTagBits(ctx, j&31);
//			k=BTIC2BH_DecodeDiffDC(ctx, actab);
			j=BTIC2BH_DecodeSymbol(actab);
			k=BTIC2BH_DecodeCoeffValueBits(ctx, j&63);
			buf[i]=k;
			continue;
		}

		i+=k;
		if(!(j&31))continue;
		k=BTIC2BH_DecodeCoeffValueBits(ctx, j&31);
		buf[i]=k;
	}

	if(i>16)printf("bad dct block1 %02X %d/%d\n", j, ni, nn);

	return(0);
}

void BTIC2BH_DequantBlock(BTIC2B_ImageContext *ctx,
	short *ibuf, short *obuf, int qid)
{
	int i, j;
	for(i=0; i<16; i++)
	{
		obuf[btic2b_zigzag2[i]]=
			ibuf[i]*btic2b_qt[qid][btic2b_zigzag2[i]];
	}
}

int BTIC2B_MarkerIDAT(BTIC2B_ImageContext *ctx, byte *buf, int sz)
{
	static s32 dbuf[8*16];
	byte ch[8], cv[8], qid[8];
	byte step[8];
	int xi[8], yi[8], wi[8], hi[8];
	byte cdt[8], cat[8];
	int mxs, mys;

	int l0, l1, p2, l3;
	int pa, pb, pc, pd;
	int i, j, k, l, i1, j1, k1, l1;
	int w, h, x, y, n, ns;

	ns=ctx->ihc_nc;
	for(i=0; i<ns; i++)
	{
//		printf("%d %d %d\n", s[0], s[1]>>4, s[1]&15);

		qiq[i]=ctx->ihc_qid[i];
		ch[i]=ctx->ihc_ch[i];
		cv[i]=ctx->ihc_cv[i];
		cdt[i]=ctx->ihc_cdt[i];
		cat[i]=ctx->ihc_cat[i];

		w=ctx->ihc_cxib[i];
		h=ctx->ihc_cyib[i];
		wi[i]=w;
		hi[i]=h;

		n=(w+1)*(h+2);
//		ctx->im_scid[i]=qid[0];
//		ctx->im_scn[i]=j;
		ctx->im_scbuf[i]=malloc(n*ctx->ihc_pcbs2[i]*sizeof(s32));
		ctx->im_sdcbuf[i]=malloc(n*sizeof(s32));
		ctx->im_sibuf[i]=malloc(n*ctx->ihc_pcbs2[i]*sizeof(s16));

//		memset(btic2b_sibuf[i], 0, n*64);
	}

	mxs=ctx->im_imxs;
	mys=ctx->im_imys;
	n=mxs*mys;

	BTIC2BH_PushSetInput(ctx, buf, sz);
	BTIC2BH_InitStream(ibuf, sz);

	for(i=0; i<(8*16); i++)dbuf[i]=0;

	for(i=0; i<n; i++)
	{
		for(j=0; j<ns; j++)
			for(k=0; k<cv[j]; k++)
				for(l=0; l<ch[j]; l++)
		{
			y=(i/mxs)*cv[j]+k;
			x=(i%mxs)*ch[j]+l;
			k1=(y*wi[j])+x;

			pa=0; pb=0; pc=0;
			if(x>0)
				{ pa=ctx->im_sdcbuf[j][(y*wi[j])+(x-1)]; }
			if(y>0)
				{ pb=ctx->im_sdcbuf[j][((y-1)*wi[j])+x]; }
			if((x>0) && (y>0))
				{ pc=ctx->im_sdcbuf[j][((y-1)*wi[j])+(x-1)]; }

			pd=BTIC2B_Paeth(pa, pb, pc);
			dbuf[j*16+0]=pd;
			
			BTIC2BH_DecodeBlock(dbuf+j*16,
				cdt[j]*2+0, cat[j]*2+1, i, n);
			ctx->im_sdcbuf[j][k1]=dbuf[j*16+0];
			
			BTIC2BH_DequantBlock(dbuf+j*16,
				ctx->im_scbuf[j]+k1*16, qid[j]);
			BTIC2B_TransIDCT(ctx->im_scbuf[j]+k1*16,
				btic2b_sibuf[j]+k1*16);
		}
	}
	BTIC2BH_PopSetInput(ctx);
	
	return(0);
}

int BTIC2B_MarkerIHDR(BTIC2B_ImageContext *ctx, byte *buf, int sz)
{
	byte *cnt, *s;
	int bs, bt, mbxs, mbys;
	int i, j, k, l, m, n;

	BTIC2BH_PushSetInput(ctx, buf, sz);
	BTIC2BH_InitStream(ctx, buf, sz);

	ctx->xs=BTIC2BH_ReadNBitsL(ctx, 24);
	ctx->ys=BTIC2BH_ReadNBitsL(ctx, 24);

	BTIC2BH_ReadNBits(ctx, 8);
	BTIC2BH_ReadNBits(ctx, 8);

	ctx->flags=BTIC2BH_ReadNBitsL(ctx, 32);

	ctx->imgtype=BTIC2BH_ReadNBits(ctx, 8);
	ctx->bpp=BTIC2BH_ReadNBits(ctx, 8);
	ctx->pixtype=BTIC2BH_ReadNBits(ctx, 4);
	ctx->ihc_nc=BTIC2BH_ReadNBits(ctx, 4);

	m=0; n=0;
	for(i=0; i<ctx->ihc_nc; i++)
	{
		ctx->ihc_cid[i]=BTIC2BH_ReadNBits(ctx, 4);
		ctx->ihc_qid[i]=BTIC2BH_ReadNBits(ctx, 4);
		ctx->ihc_ch[i]=BTIC2BH_ReadNBits(ctx, 4);
		ctx->ihc_cv[i]=BTIC2BH_ReadNBits(ctx, 4);
		ctx->ihc_cdt[i]=BTIC2BH_ReadNBits(ctx, 4);
		ctx->ihc_cat[i]=BTIC2BH_ReadNBits(ctx, 4);
		ctx->ihc_cbt[i]=BTIC2BH_ReadNBits(ctx, 4);
		ctx->ihc_cbs[i]=BTIC2BH_ReadNBits(ctx, 4);

		if(btic2b_ch[i]>m)m=btic2b_ch[i];
		if(btic2b_cv[i]>n)n=btic2b_cv[i];
	}

	ctx->ihc_chm=m;
	ctx->ihc_cvm=n;

	for(i=0; i<8; i++)
		ctx->ihc_cidx[i]=-1;

	bs=0; bt=0; mbxs=0; mbys=0;
	for(i=0; i<ctx->ihc_nc; i++)
	{
		ctx->ihc_cxi[i]=(ctx->ihc_ch[i]*ctx->xs)/m;
		ctx->ihc_cyi[i]=(ctx->ihc_cv[i]*ctx->ys)/n;
		
		j=ctx->ihc_cid[i];
		ctx->ihc_cidx[j]=i;
		
		bt=ctx->ihc_cbt[i];
		bs=ctx->ihc_cbs[i];
		
		j=1<<(bs+2);
		ctx->ihc_pcbs[i]=j;
		ctx->ihc_pcbs2[i]=j*j;
		ctx->ihc_pcbsh[i]=bs+2;
		ctx->ihc_pcbsh2[i]=2*(bs+2);
		
		ctx->ihc_cxib[i]=(ctx->ihc_cxi[i]+j-1)/j;
		ctx->ihc_cyib[i]=(ctx->ihc_cyi[i]+j-1)/j;
		
//		k=(ctx->ihc_ch[i]*j)/m;
//		l=(ctx->ihc_cv[i]*j)/n;
		k=(m*j)/ctx->ihc_ch[i];
		l=(n*j)/ctx->ihc_cv[i];

		if(k>mbxs)mbxs=k;
		if(l>mbys)mbys=l;
	}

	ctx->im_mbxs=mbxs;
	ctx->im_mbys=mbys;
	ctx->im_imxs=(ctx->xs+mbxs-1)/mbxs;
	ctx->im_imys=(ctx->ys+mbys-1)/mbys;

	for(i=0; i<ctx->ihc_nc; i++)
	{
		ctx->ihc_cbh[i]=mbxs/ctx->ihc_pcbs[i];
		ctx->ihc_cbv[i]=mbys/ctx->ihc_pcbs[i];
	}

	BTIC2BH_PopSetInput(ctx);

	return(0);
}

int BTIC2B_MarkerDQT(BTIC2B_ImageContext *ctx, byte *buf, int sz, int tab)
{
	byte *s, *se;
	int ts, rk;
	int i, j, l, n;

	BTIC2BH_PushSetInput(ctx, buf, sz);
	BTIC2BH_InitStream(ctx, buf, sz);

	ts=BTIC2BH_ReadNBits(ctx, 4);
	rk=BTIC2BH_ReadNBits(ctx, 4);

	n=16;
	if(ts==0)n=16;
	if(ts==1)n=64;
	if(ts==2)n=256;

	l=-1;
	for(i=0; i<n; i++)
	{
		j=BTIC2BH_ReadSRice(ctx, rk);
		if(j<0)
		{
			k=i+(-j);
			for(j=i; j<k; j++)
				ctx->qtab[tab][btic2b_zigzag2[j]]=l;
			i=k-1;
			continue;
		}
		btic2b_qt[tab][btic2b_zigzag2[i]]=j;
	}

	BTIC2BH_PopSetInput(ctx);

	return(0);
}

int BTIC2B_MarkerDHT(BTIC2B_ImageContext *ctx, byte *buf, int sz, int tab)
{
	BTIC2BH_PushSetInput(ctx, buf, sz);
	BTIC2BH_InitStream(ctx, buf, sz);
	BTIC2BH_DecodeHuffTable(ctx, tab);
	BTIC2BH_PopSetInput(ctx);

	return(0);
}

int BTIC2B_CheckHasComponent(BTIC2B_ImageContext *ctx, int cn)
{
	int i;
	i=ctx->im_cidx[cn];
	return(i>=0);
}

int BTIC2B_CheckHasComponent2(BTIC2B_ImageContext *ctx,
	int cn0, int cn1)
{
	return(BTIC2B_CheckHasComponent(ctx, cn0) &&
		BTIC2B_CheckHasComponent(ctx, cn1));
}

int BTIC2B_CheckHasComponent3(BTIC2B_ImageContext *ctx,
	int cn0, int cn1, int cn2)
{
	return(
		BTIC2B_CheckHasComponent(ctx, cn0) &&
		BTIC2B_CheckHasComponent(ctx, cn1) &&
		BTIC2B_CheckHasComponent(ctx, cn2));
}

int BTIC2B_CheckHasComponent4(BTIC2B_ImageContext *ctx,
	int cn0, int cn1, int cn2, int cn3)
{
	return(
		BTIC2B_CheckHasComponent(ctx, cn0) &&
		BTIC2B_CheckHasComponent(ctx, cn1) &&
		BTIC2B_CheckHasComponent(ctx, cn2) &&
		BTIC2B_CheckHasComponent(ctx, cn3));
}

int BTIC2B_GetComponentPixel(BTIC2B_ImageContext *ctx,
	int cn, int x, int y)
{
	int i, j, k, l;
	int id, w, h, n, v, bs, bsh, bsh2;

//	id=ctx->im_scn[cn];
	id=ctx->im_cidx[cn];

	bs=ctx->ihc_pcbs[id];
	bsh=ctx->ihc_pcbsh[id];
	bsh2=bsh<<1;

	i=(ctx->ihc_ch[id]*x)/ctx->ihc_chm;
	j=(ctx->ihc_cv[id]*y)/ctx->ihc_cvm;
//	k=i>>2;
//	l=j>>2;
	k=i>>bsh;
	l=j>>bsh;

//	w=(ctx->ihc_cxi[id]+ctx->ihc_pcbs[id]-1)/ctx->ihc_pcbs[id];
//	h=(ctx->ihc_cyi[id]+ctx->ihc_pcbs[id]-1)/ctx->ihc_pcbs[id];
	w=ctx->ihc_cxib[id];
	h=ctx->ihc_cyib[id];
	n=(l*w)+k;

	k=i&(bs-1);
	l=j&(bs-1);

//	v=btic2b_sibuf[cn][n*16+(l*4+k)];
	v=btic2b_sibuf[id][(n<<bsh2)+((l<<bsh)+k)];
	return(v);
}

void BTIC2B_DecodeColorConvertImage(BTIC2B_ImageContext *ctx)
{
	int y, u, v, r, g, b, a;
	int i, j, k, l;

	if(!ctx->im_imgbuf)
	{
		ctx->im_imgbuf=malloc((ctx->xs+1)*(ctx->ys+1)*4);
		memset(ctx->im_imgbuf, 0xFF, ctx->xs*ctx->ys*4);
	}

	if(BTIC2B_CheckHasComponent4(ctx, 1, 2, 3, 5))
	{
		for(i=0; i<ctx->ys; i++)
			for(j=0; j<ctx->xs; j++)
		{
			y=BTIC2B_GetComponentPixel(1, j, i);
			u=BTIC2B_GetComponentPixel(2, j, i);
			v=BTIC2B_GetComponentPixel(3, j, i);
			a=BTIC2B_GetComponentPixel(5, j, i);

			g=y+(v>>1);
			r=g-v-(u>>1)
			b=g-v+(u>>1)

			r=(r<0)?0:((r>255)?255:r);
			g=(g<0)?0:((g>255)?255:g);
			b=(b<0)?0:((b>255)?255:b);

			k=ctx->ys-1-i;
			l=((k*ctx->xs)+j)*4;

			ctx->im_imgbuf[l+0]=r;
			ctx->im_imgbuf[l+1]=g;
			ctx->im_imgbuf[l+2]=b;
			ctx->im_imgbuf[l+3]=a;
		}
	}else if(BTIC2B_CheckHasComponent3(ctx, 1, 2, 3))
	{
		for(i=0; i<ctx->ys; i++)
			for(j=0; j<ctx->xs; j++)
		{
			y=BTIC2B_GetComponentPixel(1, j, i);
			u=BTIC2B_GetComponentPixel(2, j, i);
			v=BTIC2B_GetComponentPixel(3, j, i);
			a=255;

			g=y+(v>>1);
			r=g-v-(u>>1)
			b=g-v+(u>>1)

			r=(r<0)?0:((r>255)?255:r);
			g=(g<0)?0:((g>255)?255:g);
			b=(b<0)?0:((b>255)?255:b);

			k=ctx->ys-1-i;
			l=((k*ctx->xs)+j)*4;

			ctx->im_imgbuf[l+0]=r;
			ctx->im_imgbuf[l+1]=g;
			ctx->im_imgbuf[l+2]=b;
			ctx->im_imgbuf[l+3]=a;
		}
	}

	for(i=0; i<ctx->ys; i++)
		for(j=0; j<ctx->xs; j++)
	{
		if(BTIC2B_CheckHasComponent2(ctx, 1, 5))
		{
			y=BTIC2B_GetComponentPixel(1, j, i);
			a=BTIC2B_GetComponentPixel(5, j, i);
			r=y; g=y; b=y;
		}else if(BTIC2B_CheckHasComponent(ctx, 1))
		{
			y=BTIC2B_GetComponentPixel(1, j, i);
			r=y; g=y; b=y;
		}

		k=ctx->ys-1-i;
		l=((k*ctx->xs)+j)*4;

		ctx->im_imgbuf[l+0]=r;
		ctx->im_imgbuf[l+1]=g;
		ctx->im_imgbuf[l+2]=b;
		ctx->im_imgbuf[l+3]=a;
	}
}

byte *BTIC2B_DecodeComponentImage(BTIC2B_ImageContext *ctx,
	byte *ibuf, int sz, int *xs, int *ys)
{
	byte *cs, *cse;
	byte *obuf;
	u32 tag;
	int len;
	int i, j, k, l;

	BTIC2BH_PushSetInput(ctx, ibuf, sz);
	BTIC2BH_InitReadBytes(ctx, ibuf, sz);

	while(ctx->cs<ctx->cse)
	{
		tag=BTIC2BH_ReadVLT(ctx);
		len=BTIC2BH_ReadVLI(ctx);

		if(tag==FOURCC('C', 'I', 'M', 'G'))
		{
			BTIC2B_DecodeComponentImage(ctx, ctx->cs, len, xs, ys);
			ctx->cs+=len;
			break;
		}

		if((TWOCC_A(tag)=='H') &&
			(TWOCC_B(tag)>='0') && (TWOCC_B(tag)<='9'))
		{
			BTIC2B_MarkerDHT(ctx, ctx->cs, len, TWOCC_B(tag)-'0');
			ctx->cs+=len;
			continue;
		}

		if((TWOCC_A(tag)=='H') &&
			(TWOCC_B(tag)>='A') && (TWOCC_B(tag)<='F'))
		{
			BTIC2B_MarkerDHT(ctx, ctx->cs, len, TWOCC_B(tag)-'A'+10);
			ctx->cs+=len;
			continue;
		}

		if((TWOCC_A(tag)=='Q') &&
			(TWOCC_B(tag)>='0') && (TWOCC_B(tag)<='9'))
		{
			BTIC2B_MarkerDQT(ctx, ctx->cs, len, TWOCC_B(tag)-'0');
			ctx->cs+=len;
			continue;
		}

		if(tag==TWOCC('I', 'D'))
		{
			BTIC2B_MarkerIDAT(ctx, ctx->cs, len);
			ctx->cs+=len;
			continue;
		}
	
		ctx->cs+=len;
		continue;
	}

	BTIC2BH_PopSetInput(ctx);

	if(xs)*xs=ctx->xs;
	if(ys)*ys=ctx->ys;
	return(ctx->cs);
}
