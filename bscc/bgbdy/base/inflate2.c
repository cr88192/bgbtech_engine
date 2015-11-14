/*
2006-03-17 Brendan G Bohannon

Single Buffer Inflater

Assumes all the input is available, and the output buffer is big enough to
store the entire decoded stream (may pass lengths as a sanity check, currently
unused).

The bitstream works by reading from the low end of the window, moving a pos up,
and shifting the window down. This is good up to 24 bits at a time.

Multi-bit elements are packed from the low bit up, starting in the low bit,
thus all that is needed is to shift and mask them.

Huffman codes are packed with the MSB in the LSB position of the byte, and are
thus transposed. This is an issue of constructing the decoder table.

This can technically handle Defltate64 as well.

2012-03-23:
Eliminate globals and use a context, goal being to support inflating in multiple threads.
*/

#include <bgbgc.h>
#include <bgbdy.h>

PDUNZ2_Context *pdunz2_freectx=NULL;
void *pdunz2_mutex=NULL;

void pdunz2_lock(void)
	{ thLockFastMutex(pdunz2_mutex); }
void pdunz2_unlock(void)
	{ thUnlockFastMutex(pdunz2_mutex); }

PDUNZ2_Context *PDUNZ2_AllocContext(void)
{
	PDUNZ2_Context *ctx;
	
	if(!pdunz2_mutex)
		{ pdunz2_mutex=thFastMutex(); }
	
	pdunz2_lock();
	if(pdunz2_freectx)
	{
		ctx=pdunz2_freectx;
		pdunz2_freectx=ctx->next;
		pdunz2_unlock();
		return(ctx);
	}
	pdunz2_unlock();

	ctx=gcalloc(sizeof(PDUNZ2_Context));
	return(ctx);
}

void PDUNZ2_FreeContext(PDUNZ2_Context *ctx)
{
	pdunz2_lock();
	ctx->next=pdunz2_freectx;
	pdunz2_freectx=ctx;
	pdunz2_unlock();
//	gcfree(ctx);
}


int PDUNZ2_ReadBit(PDUNZ2_Context *ctx)
{
	int i;

	i=(ctx->win>>ctx->pos)&1;
	ctx->pos++;
	if(ctx->pos>=8)
	{
		ctx->win=(ctx->win>>8)|((*ctx->cs++)<<24);
		ctx->pos-=8;
	}
	return(i);
}

int PDUNZ2_Read2Bits(PDUNZ2_Context *ctx)
{
	int i;
	i=(ctx->win>>ctx->pos)&3;
	ctx->pos+=2;
	if(ctx->pos>=8)
	{
		ctx->win=(ctx->win>>8)|((*ctx->cs++)<<24);
		ctx->pos-=8;
	}
	return(i);
}

int PDUNZ2_Read3Bits(PDUNZ2_Context *ctx)
{
	int i;
	i=(ctx->win>>ctx->pos)&7;
	ctx->pos+=3;
	if(ctx->pos>=8)
	{
		ctx->win=(ctx->win>>8)|((*ctx->cs++)<<24);
		ctx->pos-=8;
	}
	return(i);
}

int PDUNZ2_Read4Bits(PDUNZ2_Context *ctx)
{
	int i;
	i=(ctx->win>>ctx->pos)&15;
	ctx->pos+=4;
	if(ctx->pos>=8)
	{
		ctx->win=(ctx->win>>8)|((*ctx->cs++)<<24);
		ctx->pos-=8;
	}
	return(i);
}

int PDUNZ2_Read5Bits(PDUNZ2_Context *ctx)
{
	int i;
	i=(ctx->win>>ctx->pos)&31;
	ctx->pos+=5;
	if(ctx->pos>=8)
	{
		ctx->win=(ctx->win>>8)|((*ctx->cs++)<<24);
		ctx->pos-=8;
	}
	return(i);
}

int PDUNZ2_ReadNBits(PDUNZ2_Context *ctx, int n)
{
	int i;
	i=(ctx->win>>ctx->pos)&((1<<n)-1);
	ctx->pos+=n;
	while(ctx->pos>=8)
	{
		ctx->win=(ctx->win>>8)|((*ctx->cs++)<<24);
		ctx->pos-=8;
	}
	return(i);
}

void PDUNZ2_SkipNBits(PDUNZ2_Context *ctx, int n)
{
	ctx->pos+=n;
	while(ctx->pos>=8)
	{
		ctx->win=(ctx->win>>8)|((*ctx->cs++)<<24);
		ctx->pos-=8;
	}
}

int PDUNZ2_PeekWord(PDUNZ2_Context *ctx)
{
	int i;
	i=(ctx->win>>ctx->pos)&65535;
	return(i);
}

void PDUNZ2_AlignByte(PDUNZ2_Context *ctx)
{
	while(ctx->pos>0)
	{
		ctx->win=(ctx->win>>8)|((*ctx->cs++)<<24);
		ctx->pos-=8;
	}
	ctx->pos=0;
}

int PDUNZ2_ReadAlignedByte(PDUNZ2_Context *ctx)
{
	int i;
	i=ctx->win&0xFF;
	ctx->win=(ctx->win>>8)|((*ctx->cs++)<<24);
	return(i);
}

int PDUNZ2_ReadAlignedWord(PDUNZ2_Context *ctx)
{
	int i;
	i=ctx->win&0xFFFF;
	ctx->win=(ctx->win>>16)|(ctx->cs[0]<<16)|(ctx->cs[1]<<24);
	ctx->cs+=2;
	return(i);
}

static int trans8[256]={
0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0,
0x10, 0x90, 0x50, 0xD0, 0x30, 0xB0, 0x70, 0xF0,
0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8,
0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8, 0x78, 0xF8,
0x04, 0x84, 0x44, 0xC4, 0x24, 0xA4, 0x64, 0xE4,
0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4,
0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC,
0x1C, 0x9C, 0x5C, 0xDC, 0x3C, 0xBC, 0x7C, 0xFC,
0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2,
0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2,
0x0A, 0x8A, 0x4A, 0xCA, 0x2A, 0xAA, 0x6A, 0xEA,
0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA,
0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6,
0x16, 0x96, 0x56, 0xD6, 0x36, 0xB6, 0x76, 0xF6,
0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE,
0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE,
0x01, 0x81, 0x41, 0xC1, 0x21, 0xA1, 0x61, 0xE1,
0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1,
0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9,
0x19, 0x99, 0x59, 0xD9, 0x39, 0xB9, 0x79, 0xF9,
0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5,
0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5,
0x0D, 0x8D, 0x4D, 0xCD, 0x2D, 0xAD, 0x6D, 0xED,
0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD,
0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3,
0x13, 0x93, 0x53, 0xD3, 0x33, 0xB3, 0x73, 0xF3,
0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB,
0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB,
0x07, 0x87, 0x47, 0xC7, 0x27, 0xA7, 0x67, 0xE7,
0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7,
0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF,
0x1F, 0x9F, 0x5F, 0xDF, 0x3F, 0xBF, 0x7F, 0xFF
};

int PDUNZ2_TransposeByte(int v)
{
	return(trans8[v]);
}

int PDUNZ2_TransposeWord(int v)
{
	int i;
	i=trans8[(v>>8)&0xFF]|(trans8[v&0xFF]<<8);
	return(i);
}

int PDUNZ2_SetupTable(byte *cl, int ncl,
	u16 *tc, u16 *tm, byte *tl,
	u16 *ti, u16 *tn)
{
	int cnt[16], nc[16], nn[16], nf[16];
	int i, j, k, k2, l, ff;

	for(i=0; i<16; i++) { cnt[i]=0; nc[i]=0; nn[i]=-1; nf[i]=-1; }
	for(i=0; i<256; i++)ti[i]=0xFFFF;

	for(i=0; i<ncl; i++)cnt[cl[i]]++;
	cnt[0]=0;

#if 1
	j=cl[0];
	for(i=15; i>=1; i--)
		if(cnt[i])break;
	if(j>i)j=i;
	if(!i)return(-12);	//no codes

	j=1;
	for(i=1; i<16; i++)
	{
		j<<=1;
		j-=cnt[i];

//		printf("%d %d %d\n", i, j, cnt[i]);
		if(j<0)return(-10);	//over subscribed
	}
	if((j>0) && ((ncl-cnt[0])!=1))
		return(-11);	//incomplete set
#endif

	j=0;
	for(i=1; i<16; i++)
	{
		j=(j+cnt[i-1])<<1;
		nc[i]=j;
	}

	for(i=0; i<ncl; i++)
	{
		l=cl[i];
		if(!l)
		{
			tl[i]=cl[i];
			continue;
		}

		tl[i]=cl[i];
		tm[i]=(1<<tl[i])-1;
		tc[i]=nc[l]++;

		if(nn[l]>=0)
		{
			tn[nn[l]]=i;
			nn[l]=i;
		}else
		{
			nf[l]=i;
			nn[l]=i;
		}
	}

	j=-1; ff=-1;
	for(i=1; i<16; i++)
	{
		if(nf[i]>=0)
		{
			if(ff<0)ff=nf[i];
			if(j>=0)tn[j]=nf[i];
			j=nn[i];
		}
	}
	if(j>=0)tn[j]=0xFFFF;

	for(i=ff; i!=0xFFFF; i=tn[i])
	{
		l=cl[i];
		if(l<=8)
		{
			k2=tc[i]<<(8-l);
			j=1<<(8-l);
			while(j--)
			{
				k=PDUNZ2_TransposeByte(k2++);
				if(ti[k]==0xFFFF)ti[k]=i;
			}
		}else
		{
			k2=tc[i]>>(l-8);
			k=PDUNZ2_TransposeByte(k2);
			if(ti[k]==0xFFFF)ti[k]=i;
		}

		k2=(tc[i])<<(16-l);
		k=PDUNZ2_TransposeWord(k2++);
		tc[i]=k;
	}

	j=0;
	for(i=0; i<256; i++)if(ti[i]==0xFFFF)
	{
		printf("table bad index %d\n", i);
		j=-9;
	}
	return(j);
}

int PDUNZ2_SetupStatic(PDUNZ2_Context *ctx)
{
	byte lcl[288], dcl[32];
	int hl, hd;
	int i, j;

	hl=288;
	for(i=0; i<144; i++)lcl[i]=8;
	for(i=144; i<256; i++)lcl[i]=9;
	for(i=256; i<280; i++)lcl[i]=7;
	for(i=280; i<288; i++)lcl[i]=8;

	hd=32;
	for(i=0; i<32; i++)dcl[i]=5;

	j=PDUNZ2_SetupTable(lcl, hl,
		ctx->ltab_code, ctx->ltab_mask, ctx->ltab_len,
		ctx->ltab_idx, ctx->ltab_next);
	if(j<0)
	{
		printf("Static Literal/Length Table Problem %d\n", j);
		return(j);
	}

	j=PDUNZ2_SetupTable(dcl, hd,
		ctx->dtab_code, ctx->dtab_mask, ctx->dtab_len,
		ctx->dtab_idx, ctx->dtab_next);
	if(j<0)
	{
		printf("Static Distance Table Problem %d\n", j);
		return(j);
	}

	return(0);
}

int PDUNZ2_DecodeSymbol(PDUNZ2_Context *ctx)
{
	int i, j;

	i=PDUNZ2_PeekWord(ctx);
	j=ctx->ltab_idx[i&0xFF];

	while((i&ctx->ltab_mask[j])!=ctx->ltab_code[j])
		j=ctx->ltab_next[j];

	PDUNZ2_SkipNBits(ctx, ctx->ltab_len[j]);
	return(j);
}

int PDUNZ2_DecodeDistance(PDUNZ2_Context *ctx)
{
	static int dbase[]={
		1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193,
		257, 385, 513, 769, 1025, 1537, 2049, 3073,
		4097, 6145, 8193, 12289, 16385, 24577, 32769, 49153};
	static int dextra[]={
		0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6,
		7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14};

	int i, j, k;

	i=PDUNZ2_PeekWord(ctx);
	j=ctx->dtab_idx[i&0xFF];

	while((i&ctx->dtab_mask[j])!=ctx->dtab_code[j])
		j=ctx->dtab_next[j];

	PDUNZ2_SkipNBits(ctx, ctx->dtab_len[j]);
	k=dbase[j]+PDUNZ2_ReadNBits(ctx, dextra[j]);

	return(k);
}

int PDUNZ2_DecodeCodeLengths(PDUNZ2_Context *ctx, byte *cl, int ncl)
{
	int i, j, k;

	i=0; k=0;
	while(i<ncl)
	{
		j=PDUNZ2_DecodeSymbol(ctx);
		if(j<0)return(j);

		if(j<16)
		{
			k=j;
			cl[i++]=j;
			continue;
		}

		if(j==16)
		{
			j=PDUNZ2_Read2Bits(ctx)+3;
			while(j--)cl[i++]=k;
			continue;
		}

		if(j==17)
		{
			j=PDUNZ2_Read3Bits(ctx)+3;
			while(j--)cl[i++]=0;
			k=0;
			continue;
		}
		if(j==18)
		{
			j=PDUNZ2_ReadNBits(ctx, 7)+11;
			while(j--)cl[i++]=0;
			k=0;
			continue;
		}

		return(-7);
	}
	return(0);
}

int PDUNZ2_DecodeHeader(PDUNZ2_Context *ctx)
{
	static int lorder[]={
		16, 17, 18, 0, 8,7, 9,6, 10,5, 11,4, 12,3, 13,2, 14,1, 15};

	byte hcl[32], lcl[512], dcl[32];
	int hl, hd, hc;
	int i, j;

	hl=PDUNZ2_ReadNBits(ctx, 5)+257;
	hd=PDUNZ2_ReadNBits(ctx, 5)+1;
	hc=PDUNZ2_ReadNBits(ctx, 4)+4;

//	printf("%d %d %d\n", hl, hd, hc);

	for(i=0; i<32; i++)hcl[i]=0;
	for(i=0; i<hc; i++)
		hcl[lorder[i]]=PDUNZ2_ReadNBits(ctx, 3);

//	for(i=0; i<20; i++)printf("%d:%d ", i, hcl[i]);
//	printf("\n");

	j=PDUNZ2_SetupTable(hcl, 32,
		ctx->ltab_code, ctx->ltab_mask, ctx->ltab_len,
		ctx->ltab_idx, ctx->ltab_next);
	if(j<0)
	{
		printf("Codes Table Problem %d\n", j);
		for(i=0; i<20; i++)printf("%d:%d ", i, hcl[i]);
		printf("\n");
		return(j);
	}

	i=PDUNZ2_DecodeCodeLengths(ctx, lcl, hl);
	if(i<0)return(i);
	i=PDUNZ2_DecodeCodeLengths(ctx, dcl, hd);
	if(i<0)return(i);

//	for(i=0; i<hl; i++)
//	{
//		j=lcl[i];
//		if(j)printf("%d:%d ", i, j);
//	}
//	printf("\n");

//	for(i=0; i<hd; i++)printf("%d:%d ", i, dcl[i]);
//	printf("\n");

	j=PDUNZ2_SetupTable(lcl, hl,
		ctx->ltab_code, ctx->ltab_mask, ctx->ltab_len,
		ctx->ltab_idx, ctx->ltab_next);
	if(j<0)
	{
		printf("Literal/Length Table Problem %d\n", j);
		for(i=0; i<hl; i++)printf("%d:%d ", i, lcl[i]);
		printf("\n");
		return(j);
	}

	j=PDUNZ2_SetupTable(dcl, hd,
		ctx->dtab_code, ctx->dtab_mask, ctx->dtab_len,
		ctx->dtab_idx, ctx->dtab_next);
	if(j<0)
	{
		printf("Distance Table Problem %d\n", j);
		for(i=0; i<hd; i++)printf("%d:%d ", i, dcl[i]);
		printf("\n");
		return(j);
	}

	return(0);
}


int PDUNZ2_DecodeRun(PDUNZ2_Context *ctx, int sym)
{
	static int lbase[]={
		3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31,
		35, 43, 51, 59, 67, 83, 99, 115,
		131, 163, 195, 227, 258, 0, 0, 0};
	static int lextra[]={
		0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2,
		3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0, 0, 0, 0};
	char *s;
	int i, j, k;

	i=sym-257;
	j=lbase[i]+PDUNZ2_ReadNBits(ctx, lextra[i]);
	k=PDUNZ2_DecodeDistance(ctx);
	s=ctx->ct-k;
	i=j;
	while(i--)*ctx->ct++=*s++;

	return(0);
}

int PDUNZ2_DecodeBlockData(PDUNZ2_Context *ctx)
{
	int i;

	while(1)
	{
		i=PDUNZ2_DecodeSymbol(ctx);
		if(i<0)return(i);

		if(i<256)
		{
			*ctx->ct++=i;
			continue;
		}
		if(i==256)break;

		i=PDUNZ2_DecodeRun(ctx, i);
		if(i<0)return(i);
	}

	return(0);
}

int PDUNZ2_DecodeBlock(PDUNZ2_Context *ctx)
{
	int fi, ty, l, nl;
	int i;

	fi=PDUNZ2_ReadBit(ctx);
	ty=PDUNZ2_Read2Bits(ctx);

//	printf("blk %d %d\n", fi, ty);

	switch(ty)
	{
	case 0:
		PDUNZ2_AlignByte(ctx);
		l=PDUNZ2_ReadAlignedWord(ctx);
		nl=(~PDUNZ2_ReadAlignedWord(ctx))&0xFFFF;
		if(l!=nl)
		{
//			printf("%04X %04X(%04X)\n", l, nl, (~nl)&0xFFFF);
			return(-3);
		}

		while(l--)
		{
			i=PDUNZ2_ReadAlignedByte(ctx);
			*ctx->ct++=i;
		}
		break;
	case 1:
		i=PDUNZ2_SetupStatic(ctx);
		if(i<0)return(i);
		i=PDUNZ2_DecodeBlockData(ctx);
		if(i<0)return(i);
		break;
	case 2:
		i=PDUNZ2_DecodeHeader(ctx);
		if(i<0)return(i);
		i=PDUNZ2_DecodeBlockData(ctx);
		if(i<0)return(i);
		break;

	case 3:
	default:
		return(-2);
		break;
	}

	return(fi);
}

int PDUNZ2_DecodeStreamCtx(PDUNZ2_Context *ctx, 
	byte *ibuf, byte *obuf, int isz, int osz)
{
	int i, k;

	ctx->ibuf=ibuf;
	ctx->obuf=obuf;
	ctx->cs=ibuf+4;
	ctx->ct=obuf;
	ctx->cse=ibuf+isz;
	ctx->ce=obuf+osz;

	ctx->win=ibuf[0]|(ibuf[1]<<8)|(ibuf[2]<<16)|(ibuf[3]<<24);
	ctx->pos=0;

	k=0;
	while(1)
	{
		i=PDUNZ2_DecodeBlock(ctx);
		if(i<0)return(i);
		if(i)break;
	}

	i=ctx->pos;
	while(i<=24)
	{
		ctx->cs--;
		i+=8;
	}

	i=ctx->cs-ibuf;
	return(i);
}

int PDUNZ2_DecodeStream(byte *ibuf, byte *obuf, int isz, int osz)
{
	PDUNZ2_Context *ctx;
	int i;
	
//	ctx=gcalloc(sizeof(PDUNZ2_Context));
	ctx=PDUNZ2_AllocContext();
	i=PDUNZ2_DecodeStreamCtx(ctx, ibuf, obuf, isz, osz);
	PDUNZ2_FreeContext(ctx);
//	gcfree(ctx);
	return(i);
}
