#include <bgbbtj.h>

/*
 0  1  2  3
 4  5  6  7
 8  9 10 11
12 13 14 15
*/

byte btic2b_zigzag[16]={
 0,  1,  5,  6,
 2,  4,  7, 12,
 3,  8, 11, 13,
 9, 10, 14, 15
};

byte btic2b_zigzag2[16]={
 0,  1,  4,  8,
 5,  2,  3,  6,
 9, 12, 13, 10,
 7, 11, 14, 15
};

#if 1
void BTIC2B_TransDCT_Horiz(byte *iblk, int *oblk)
{
	int ib0, ib1, ib2, ib3;
	int ib0p, ib1p, ib0n, ib1n;

	ib0=iblk[0];	ib1=iblk[1];
	ib2=iblk[2];	ib3=iblk[3];

	ib0p=ib0+ib3;	ib0n=ib0-ib3;
	ib1p=ib1+ib2;	ib1n=ib1-ib2;

	oblk[0]=(ib0p+ib1p)*128;
	oblk[2]=(ib0p-ib1p)*128;
	oblk[1]=ib0n*167+ib1n* 69;
	oblk[3]=ib0n* 69-ib1n*167;
}

void BTIC2B_TransDCT_Vert(int *iblk, int *oblk)
{
	int ib0, ib1, ib2, ib3;
	int ib0p, ib1p, ib0n, ib1n;

	ib0=iblk[0];	ib1=iblk[ 4];
	ib2=iblk[8];	ib3=iblk[12];

	ib0p=ib0+ib3;	ib0n=ib0-ib3;
	ib1p=ib1+ib2;	ib1n=ib1-ib2;

	oblk[ 0]=(ib0p+ib1p)*128;
	oblk[ 8]=(ib0p-ib1p)*128;
	oblk[ 4]=ib0n*167+ib1n* 69;
	oblk[12]=ib0n* 69-ib1n*167;
}

void BTIC2B_TransDCT(byte *iblk, short *oblk)
{
	int s[BTIC2B_DCTSZ2];
	int t[BTIC2B_DCTSZ2];
	int i, j;

	BTIC2B_TransDCT_Horiz(iblk+ 0, s+ 0);
	BTIC2B_TransDCT_Horiz(iblk+ 4, s+ 4);
	BTIC2B_TransDCT_Horiz(iblk+ 8, s+ 8);
	BTIC2B_TransDCT_Horiz(iblk+12, s+12);

	BTIC2B_TransDCT_Vert(s+0, t+0);
	BTIC2B_TransDCT_Vert(s+1, t+1);
	BTIC2B_TransDCT_Vert(s+2, t+2);
	BTIC2B_TransDCT_Vert(s+3, t+3);

	for(i=0; i<16; i++)
		oblk[i]=t[i]>>16;
}

#endif

byte *BTIC2BH_EmitVLI(byte *ct, u64 val)
{
	if(val<0)
	{
		*ct++=0;
		return(ct);
	}

	if(val<0x80)
	{
		*ct++=val;
		return(ct);
	}

	if(val<0x4000)
	{
		*ct++=0x80|(val>>8);
		*ct++=val&0xFF;
		return(ct);
	}

	if(val<0x200000)
	{
		*ct++=0xC0|(val>>16);
		*ct++=(val>>8)&0xFF;
		*ct++=val&0xFF;
		return(ct);
	}

	if(val<0x10000000)
	{
		*ct++=0xE0|(val>>24);
		*ct++=(val>>16)&0xFF;
		*ct++=(val>>8)&0xFF;
		*ct++=val&0xFF;
		return(ct);
	}

	if(val<0x800000000ULL)
	{
		*ct++=0xF0|(val>>32);
		*ct++=(val>>24)&0xFF;
		*ct++=(val>>16)&0xFF;
		*ct++=(val>>8)&0xFF;
		*ct++=val&0xFF;
		return(ct);
	}

	if(val<0x40000000000ULL)
	{
		*ct++=0xF8|(val>>40);
		*ct++=(val>>32)&0xFF;
		*ct++=(val>>24)&0xFF;
		*ct++=(val>>16)&0xFF;
		*ct++=(val>>8)&0xFF;
		*ct++=val&0xFF;
		return(ct);
	}

	if(val<0x2000000000000ULL)
	{
		*ct++=0xFC|(val>>48);
		*ct++=(val>>40)&0xFF;
		*ct++=(val>>32)&0xFF;
		*ct++=(val>>24)&0xFF;
		*ct++=(val>>16)&0xFF;
		*ct++=(val>>8)&0xFF;
		*ct++=val&0xFF;
		return(ct);
	}

	if(val<0x100000000000000ULL)
	{
		*ct++=0xFE|(val>>56);
		*ct++=(val>>48)&0xFF;
		*ct++=(val>>40)&0xFF;
		*ct++=(val>>32)&0xFF;
		*ct++=(val>>24)&0xFF;
		*ct++=(val>>16)&0xFF;
		*ct++=(val>>8)&0xFF;
		*ct++=val&0xFF;
		return(ct);
	}

	if(val<0x8000000000000000ULL)
	{
		*ct++=0xFF;
		*ct++=(val>>56)&0xFF;
		*ct++=(val>>48)&0xFF;
		*ct++=(val>>40)&0xFF;
		*ct++=(val>>32)&0xFF;
		*ct++=(val>>24)&0xFF;
		*ct++=(val>>16)&0xFF;
		*ct++=(val>>8)&0xFF;
		*ct++=val&0xFF;
		return(ct);
	}

	*ct++=0xFF;
	*ct++=0x80;
	*ct++=(val>>56)&0xFF;
	*ct++=(val>>48)&0xFF;
	*ct++=(val>>40)&0xFF;
	*ct++=(val>>32)&0xFF;
	*ct++=(val>>24)&0xFF;
	*ct++=(val>>16)&0xFF;
	*ct++=(val>>8)&0xFF;
	*ct++=val&0xFF;
	return(ct);
}

byte *BTIC2BH_EmitSVLI(byte *ct, s64 val)
{
	return(BTIC2BH_EmitVLI(ct,
		(val>=0)?(val<<1):(((-val)<<1)-1)));
}

byte *BTIC2BH_EmitString(byte *ct, char *str)
{
	memcpy(ct, str, strlen(str)+1);
	ct+=strlen(str)+1;
	return(ct);
}

void BTIC2B_WriteVLI(BTIC2B_ImageContext *ctx, u64 i)
{
	ctx->ct=BTIC2BH_EmitVLI(ctx->ct, i);
}

void BTIC2B_WriteSVLI(BTIC2B_ImageContext *ctx, s64 i)
{
	ctx->ct=BTIC2BH_EmitSVLI(ctx->ct, i);
}

void BTIC2B_WriteVLT(BTIC2B_ImageContext *ctx, int fl, char *str)
{
	char *s;
	u64 v;

	s=str; v=fl;
	while(*s) { v=(v<<8)|(*s++); }
	BTIC2B_WriteVLI(ctx, v);
}

void BTIC2B_WriteBytes(BTIC2B_ImageContext *ctx, byte *buf, int sz)
{
	memcpy(ctx->ct, buf, sz);
	ctx->ct+=sz;
}

void BTIC2B_WriteVLT_FCC(BTIC2B_ImageContext *ctx, int fl, u32 fcc)
{
	char *s;
	u64 v;

//	s=str; v=fl;
//	while(*s) { v=(v<<8)|(*s++); }

	v=(fl<<32)|(FOURCC_A(fcc)<<24)|(FOURCC_B(fcc)<<16)|
		(FOURCC_C(fcc)<<8)||FOURCC_D(fcc);

	BTIC2B_WriteVLI(ctx, v);
}

//Encoder

void BTIC2BH_WriteBit(BTIC2B_ImageContext *ctx, int i)
{
	ctx->huff_win|=i<<ctx->huff_pos;
	ctx->huff_pos++;
	if(ctx->huff_pos>=8)
	{
		i=ctx->huff_win&0xFF;
		*ctx->ct++=i;
		ctx->huff_win>>=8;
		ctx->huff_pos-=8;
	}
}

void BTIC2BH_WriteNBits(BTIC2B_ImageContext *ctx, int i, int n)
{
//	i&=(1<<n)-1;

	ctx->huff_win|=i<<ctx->huff_pos;
	ctx->huff_pos+=n;
	while(ctx->huff_pos>=8)
	{
		i=ctx->huff_win&0xFF;
		*ctx->ct++=i;
		ctx->huff_win>>=8;
		ctx->huff_pos-=8;
	}
}

void BTIC2BH_WriteRice(BTIC2B_ImageContext *ctx, int v, int k)
{
	int j;
	
	j=v>>k;
	while(j--)
	 { BTIC2BH_WriteBit(1); }
	BTIC2BH_WriteBit(0);
	j=v&((1<<k)-1);
	BTIC2BH_WriteNBits(j, k);
}

void BTIC2BH_WriteSRice(BTIC2B_ImageContext *ctx, int v, int k)
{
	BTIC2BH_WriteRice(ctx, (v>=0)?(v<<1):(((-v)<<1)-1), k);
}

void BTIC2BH_WriteExpRice(BTIC2B_ImageContext *ctx, int v, int k)
{
	int i, j;
	
	if(v<=0)
	{
		BTIC2BH_WriteRice(ctx, 0, k);
		return;
	}
	
	i=v; j=0;
	while(i>0) { i>>=1; j++; }
	BTIC2BH_WriteRice(ctx, j, k);
	j=v&((1<<k)-1);
	BTIC2BH_WriteNBitsL(ctx, j, i);
}

void BTIC2BH_WriteExpSRice(BTIC2B_ImageContext *ctx, int v, int k)
{
	BTIC2BH_WriteExpRice((v>=0)?(v<<1):(((-v)<<1)-1), k);
}

void BTIC2BH_FlushBits(BTIC2B_ImageContext *ctx)
{
	int i;
	while(ctx->huff_pos>0)
	{
		i=ctx->huff_win&0xFF;
		*ctx->ct++=i;
		ctx->huff_win>>=8;
		ctx->huff_pos-=8;
	}
}

void BTIC2BH_PushSetOutput(BTIC2B_ImageContext *ctx, byte *obuf, int sz)
{
	int i;
	
	i=ctx->huff_stk_pos++;
	ctx->huff_stk_ct[i]=ctx->ct;
	ctx->huff_stk_cts[i]=ctx->cts;
	ctx->huff_stk_cte[i]=ctx->cte;
	
	ctx->cts=obuf;
	ctx->cte=obuf+sz;
	ctx->ct=obuf;
}

int BTIC2BH_PopSetOutput(BTIC2B_ImageContext *ctx)
{
	int sz;

	BTIC2BH_FlushBits();
	
	sz=ctx->ct-ctx->cts;

	i=--ctx->huff_stk_pos;
	ctx->ct=ctx->huff_stk_ct[i];
	ctx->cts=ctx->huff_stk_cts[i];
	ctx->cte=ctx->huff_stk_cte[i];
	
	return(sz);
}

void BTIC2BH_PushBeginLumpFcc(BTIC2B_ImageContext *ctx, u32 fcc)
{
	BTIC2B_WriteVLT_FCC(ctx, 0, fcc);
	BTIC2B_WriteVLT_FCC(ctx, 0, FOURCC(' ', ' ', ' ', ' '));
	BTIC2BH_PushSetOutput(ctx, ctx->ct, ctx->cte-ctx->ct);
}

void BTIC2BH_PopEndLumpFcc(BTIC2B_ImageContext *ctx)
{
	int i;
	
	i=BTIC2BH_PopSetOutput(ctx);
	ctx->ct[-1]=i&0xFF;
	ctx->ct[-2]=(i>>8)&0xFF;
	ctx->ct[-3]=(i>>8)&0xFF;
	ctx->ct[-4]=(i>>8)&0xFF;
	ctx->ct+=i;
}

int BTIC2BH_BalanceTree_r(short *nodes, short *nlen,
	int root, int h, int ml)
{
	int h0, h1, h2, h3;
	int l0, l1, l2;

	if(root<0)return(0);

	h1=BTIC2BH_BalanceTree_r(nodes, nlen, nodes[root*2+0], h+1, ml);
	h2=BTIC2BH_BalanceTree_r(nodes, nlen, nodes[root*2+1], h+1, ml);
	h0=((h1>h2)?h1:h2)+1;
	nlen[root]=h0;

	if((h+h0)<=ml)	//depth limit not exceeded
		return(h0);

	//ok, so part of the tree is too deep
	if((h1+1)<h2)
	{
		l0=nodes[root*2+1];
		l1=nodes[l0*2+1];
		nodes[l0*2+1]=nodes[l0*2+0];
		nodes[l0*2+0]=nodes[root*2+0];
		nodes[root*2+0]=l0;
		nodes[root*2+1]=l1;
	}else if((h2+1)<h1)
	{
		l0=nodes[root*2+0];
		l1=nodes[l0*2+0];
		nodes[l0*2+0]=nodes[l0*2+1];
		nodes[l0*2+1]=nodes[root*2+1];
		nodes[root*2+0]=l1;
		nodes[root*2+1]=l0;
	}else
	{
		//rotating would be ineffective or would make things worse...
		return(h0);
	}

	//recalc depth of modified sub-tree
	l1=nodes[l0*2+0];
	l2=nodes[l0*2+1];
	h1=(l1<0)?0:nlen[l1];
	h2=(l2<0)?0:nlen[l2];
	h3=((h1>h2)?h1:h2)+1;
	nlen[l0]=h3;

	//recalc height of root node
	l1=nodes[root*2+0];
	l2=nodes[root*2+1];
	h1=(l1<0)?0:nlen[l1];
	h2=(l2<0)?0:nlen[l2];
	h0=((h1>h2)?h1:h2)+1;
	nlen[root]=h0;

	return(h0);
}

void BTIC2BH_CalcLengths_r(short *nodes, byte *cl, int root, int h)
{
	if(root<0)
	{
		if(root==-513)return;
		cl[(-root)-1]=h;
		return;
	}

	BTIC2BH_CalcLengths_r(nodes, cl, nodes[root*2+0], h+1);
	BTIC2BH_CalcLengths_r(nodes, cl, nodes[root*2+1], h+1);
}

int BTIC2BH_BuildLengths(int *stat, int nc, byte *cl, int ml)
{
	static short nodes[1024], nlen[512];
	static short roots[512], clen[512];
	static int cnts[512];
	int nr, nn;
	int i, j, k, l;

	nr=0; nn=0;
	for(i=0; i<nc; i++)
	{
		if(!stat[i])continue;
		roots[nr]=-(i+1);
		cnts[nr]=stat[i];
		clen[nr]=0;
		nr++;
	}

	roots[nr]=-513;
	cnts[nr]=0;
	clen[nr]=0;
	nr++;

	for(i=0; i<nc; i++)cl[i]=0;
	if(!nr)return(-1);

	while(nr>1)
	{
		if(cnts[0]>=cnts[1]) { j=0; k=1; }
			else { j=1; k=0; }
		for(i=2; i<nr; i++)
		{
			if(cnts[i]<=cnts[k])
			{
				j=k; k=i;
				continue;
			}
			if(cnts[i]<=cnts[j])
			{
				j=i;
				continue;
			}
		}

		nlen[nn]=((clen[j]>clen[k])?clen[j]:clen[k])+1;
		nodes[nn*2+0]=roots[j];
		nodes[nn*2+1]=roots[k];

		roots[nr]=nn;
		cnts[nr]=cnts[j]+cnts[k];
		clen[nr]=nlen[nn];

//		printf("%d %d %d\n", cnts[j], cnts[k], cnts[nr]);

		nn++; nr++;

		l=0;
		for(i=0; i<nr; i++)
		{
			if((i==j) || (i==k))continue;
			roots[l]=roots[i];
			cnts[l]=cnts[i];
			clen[l]=clen[i];
			l++;
		}
		nr=l;
	}

	l=roots[0];
	j=clen[0];
	k=j;

	i=4;
	while((i--) && (k>ml))
		k=BTIC2BH_BalanceTree_r(nodes, nlen, l, 0, ml);
	if(k>ml)return(-2);	//failed to balance tree

	BTIC2BH_CalcLengths_r(nodes, cl, l, 0);
	return(0);
}



static int bsxrp_lorder[]={
	16, 17, 18, 0, 8,7, 9,6, 10,5, 11,4, 12,3, 13,2, 14,1, 15, 19};
static int bsxrp_lorder2[]={
	3, 17, 15, 13, 11,9, 7,5, 4,6, 8,10, 12,14, 16,18, 0,1, 2, 19};

int BTIC2BH_DecodeHufftabSymbol(BTIC2BH_StreamContext *ctx)
{
	int i;
	i=BTIC2BH_ReadRice(ctx, 2);
	return(bsxrp_lorder[i]);
}

void BTIC2BH_EncodeHufftabSymbol(BTIC2BH_StreamContext *ctx, int v)
{
	BTIC2BH_WriteRice(ctx, bsxrp_lorder2[v], 2);
}

void BTIC2BH_StatLengths(BTIC2BH_StreamContext *ctx,
	byte *cl, int nc, int *stat)
{
	int i, j, l;

	l=-1;
	for(i=0; i<nc;)
	{
		for(j=0; ((i+j)<nc) && (cl[i+j]==0); j++);
		if(j>138)j=138;
		if(j>10) { stat[18]++; i+=j; l=0; continue; }
		if(j>2) { stat[17]++; i+=j; l=0; continue; }

		for(j=0; ((i+j)<nc) && (cl[i+j]==l); j++);
		if(j>6)j=6;
		if(j>2) { stat[16]++; i+=j; continue; }

		l=cl[i++];
		stat[l]++;
	}
}

void BTIC2BH_EncodeCodeLengths(BTIC2BH_StreamContext *ctx, byte *cl, int nc)
{
	int i, j, l;

	i=0; l=-1;
	while(i<nc)
	{
		for(j=0; ((i+j)<nc) && (cl[i+j]==l); j++);
		if(j>10)
		{
			//check for end of table
			if(((i+j)>=nc) && (l==0))
				break;
		
			if(j>138)j=138;
			BTIC2BH_EncodeHufftabSymbol(ctx, 18);
			BTIC2BH_WriteNBits(ctx, j-11, 7);
			i+=j;
			continue;
		}
		if(j>2)
		{
			BTIC2BH_EncodeHufftabSymbol(ctx, 17);
			BTIC2BH_Write3Bits(ctx, j-3);
			i+=j;
			continue;
		}

		l=cl[i++];
		BTIC2BH_EncodeHufftabSymbol(ctx, l);

		if((l<0) || (l>16))printf("problem: bad code length noted\n");
	}
	BTIC2BH_EncodeHufftabSymbol(ctx, 19);
}


int BTIC2BH_EncodeHuffTable(BTIC2BH_StreamContext *ctx,
	int tab, byte *cl, int nc)
{
	int i, j;

	BTIC2BH_WriteNBits(ctx, 1, 4);
	BTIC2BH_WriteNBits(ctx, 2, 4);
	BTIC2BH_EncodeCodeLengths(ctx, cl, nc);

	j=BTIC2BH_SetupTable(cl, nc,
		ctx->huff_code[tab], ctx->huff_mask[tab], ctx->huff_len[tab],
		ctx->huff_idx[tab], ctx->huff_next[tab]);
	if(j<0)
	{
		printf("Encode Dynamic Literal/Length Table Problem "
			"Tab=%d, Code=%d\n", tab, j);
		return(j);
	}

	return(0);
}

int BTIC2BH_CheckEncodeHuffTable(BTIC2BH_StreamContext *ctx,
	int tab, byte *cl, int nc)
{
	int i;

	for(i=0; i<nc; i++)
		if(cl[i])return(1);
	return(0);
}


void BTIC2BH_EncodeSymbol(BTIC2B_ImageContext *ctx, int tab, int v)
{
	tab<<=8;
	BTIC2BH_WriteNBits(ctx, ctx->huff_code[tab|v], ctx->huff_len[tab|v]);
}

const u32 btic2b_dbase[]={
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
const int btic2b_dextra[64]={
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

const int btic2b_lbase[32]={
		  0,   1,   2,   3,   4,   5,   6,   7,
		  8,  10,  12,  14,  16,  20,  24,  28,
		 32,  40,  48,  56,  64,  80,  96, 112,
		128, 160, 192, 224, 256, 320, 384, 448};
const int btic2b_lextra[32]={
		0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2,
		3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6};

void BTIC2BH_EncodeCoeffUValue(BTIC2B_ImageContext *ctx,
	int tab, int z, u32 val)
{
	int i, j, k;

	j=val;
	for(i=0; i<64; i++)
	{
		k=btic2b_dbase[i]+(1<<btic2b_dextra[i]);
		if((j>=dbase[i]) && (j<k))
		{
			BTIC2BH_EncodeSymbol(ctx, tab, i|(z<<5));
			BTIC2BH_WriteNBits(ctx, j-btic2b_dbase[i], btic2b_dextra[i]);
			break;
		}
	}
}

void BTIC2BH_StatCoeffUValue(BTIC2B_ImageContext *ctx,
	int *stat, int z, u32 val)
{
	int i, j, k;

	j=val;
	for(i=0; i<64; i++)
	{
		k=btic2b_dbase[i]+(1<<btic2b_dextra[i]);
		if((j>=dbase[i]) && (j<k))
		{
			stat[i|(z<<5)]++;
			break;
		}
	}
}

void BTIC2BH_EncodeCoeffValue(BTIC2B_ImageContext *ctx,
	int tab, int z, int val)
{
	BTIC2BH_EncodeCoeffUValue(ctx,
		tab, z, (val>=0)?(val<<1):(((-val)<<1)-1));
}

void BTIC2BH_StatCoeffValue(BTIC2B_ImageContext *ctx,
	int *stat, int z, int val)
{
	BTIC2BH_StatCoeffUValue(ctx,
		stat, z, (val>=0)?(val<<1):(((-val)<<1)-1));
}


void BTIC2BH_EncodeTagUValue(BTIC2B_ImageContext *ctx,
	int tab, int z, int val)
{

	int i, j, k;

	j=val;
	for(i=0; i<32; i++)
	{
		k=btic2b_lbase[i]+(1<<btic2b_lextra[i]);
		if((j>=lbase[i]) && (j<k))
		{
			BTIC2BH_EncodeSymbol(ctx, i|(z<<5));
			BTIC2BH_WriteNBits(ctx, j-btic2b_lbase[i], btic2b_lextra[i]);
			break;
		}
	}
}

void BTIC2BH_StatTagUValue(BTIC2B_ImageContext *ctx,
	int *stat, int z, int val)
{
	int i, j, k;

	j=val;
	for(i=0; i<32; i++)
	{
		k=btic2b_lbase[i]+(1<<btic2b_lextra[i]);
		if((j>=btic2b_lbase[i]) && (j<k))
		{
			stat[i|(z<<5)]++;
			break;
		}
	}
}

void BTIC2BH_EncodeTagSValue(BTIC2B_ImageContext *ctx,
	int tab, int z, int val)
{
	BTIC2BH_EncodeTagUValue(ctx,
		tab, z, (val>=0)?(val<<1):(((-val)<<1)-1));
}

void BTIC2BH_StatTagSValue(BTIC2B_ImageContext *ctx,
	int *stat, int z, int val)
{
	BTIC2BH_StatTagUValue(ctx,
		stat, z, (val>=0)?(val<<1):(((-val)<<1)-1));
}

void BTIC2BH_EncodeVal(BTIC2B_ImageContext *ctx,
	int tab, int z, int v)
{
	int i, j, k;

	if((z>=6) || (v>=32768) || (v<-32768))
	{
		BTIC2BH_EncodeTagUValue(ctx, tab, 6, z);
		BTIC2BH_EncodeCoeffValue(ctx, tab, 0, v);
		return;
	}

	if(!v) { BTIC2BH_EncodeSymbol(ctx, tab, z<<5); return; }

	BTIC2BH_EncodeCoeffValue(ctx, tab, z, v);
	return;
}

void BTIC2BH_StatVal(BTIC2B_ImageContext *ctx,
	int *stat, int z, int v)
{
	int i, j, k;

	if((z>=6) || (v>=32768) || (v<-32768))
	{
		BTIC2BH_StatTagUValue(ctx, stat, 6, z);
		BTIC2BH_StatCoeffValue(ctx, stat, 0, v);
		return;
	}

	if(!v) { stat[z<<5]++; return; }
	BTIC2BH_StatCoeffValue(ctx, stat, z, v);
}

int BTIC2BH_EncodeBlock(BTIC2B_ImageContext *ctx,
	short *buf, int dctab, int actab)
{
	int i, j, k;

	BTIC2BH_EncodeVal(ctx, dctab, 0, buf[0]);

	for(i=1; i<16; i++)
	{
		for(j=i; j<16; j++)
			if(buf[j])break;
		if(j>=16)
		{
			BTIC2BH_EncodeSymbol(ctx, actab, 0);
			break;
		}

		BTIC2BH_EncodeVal(ctx, actab, j-i, buf[j]);
		i=j;
	}
}

int BTIC2BH_StatBlock(BTIC2B_ImageContext *ctx,
	short *buf, int *dcstat, int *acstat)
{
	int i, j, k;

	BTIC2BH_StatVal(ctx, dcstat, 0, buf[0]);
	for(i=1; i<16; i++)
	{
		for(j=i; j<16; j++)
			if(buf[j])break;
		if(j>=16)
		{
			acstat[0]++;
			break;
		}

		BTIC2BH_StatVal(ctx, acstat, j-i, buf[j]);
		i=j;
	}
}

void BTIC2BH_QuantBlock(short *ibuf, short *obuf, int qid)
{
	int i, j;
	for(i=0; i<16; i++)
	{
//		obuf[i]=ibuf[i]/ctx->qtab[qid][i];
		obuf[i]=ibuf[btic2b_zigzag2[j]]/
			ctx->qtab[qid][btic2b_zigzag2[j]];
	}
}

int BTIC2B_GetImgBlk(char *blk, int xo, int yo, byte *img, int xs, int ys)
{
	int i, j, k;

	for(i=0; i<BTIC2B_DCTSZ; i++)
		for(j=0; j<BTIC2B_DCTSZ; j++)
	{
		k=img[(i+yo)*xs+(j+xo)];
		blk[i*BTIC2B_DCTSZ+j]=k;
	}
}

int BTIC2B_FilterImageDCT(byte *ibuf, short *obuf, int xs, int ys)
{
	static short tblk[BTIC2B_DCTSZ2], tblk2[BTIC2B_DCTSZ2];
	static char blk[BTIC2B_DCTSZ2];
	int i, j, k, l;

	k=0;
	for(i=0; i<(ys/BTIC2B_DCTSZ); i++)
		for(j=0; j<(xs/BTIC2B_DCTSZ); j++)
	{
		BTIC2B_GetImgBlk(blk, j*BTIC2B_DCTSZ, i*BTIC2B_DCTSZ, ibuf, xs, ys);
		BTIC2B_TransDCT(blk, obuf+k*BTIC2B_DCTSZ2);
		k++;
	}
}

void BTIC2B_EmitDQT(BTIC2B_ImageContext *ctx, int n)
{
	byte tbuf[1024];
	char tb[4];
	int i, j, k, k, sz;

	BTIC2BH_PushSetOutput(ctx, tbuf, 1024);
	
	BTIC2BH_WriteNBits(ctx, 0, 4);	//table size
	BTIC2BH_WriteNBits(ctx, 3, 4);	//rice factor

	l=-1;
	for(i=0; i<16; i++)
	{
		j=ctx->qtab[n][btic2b_zigzag2[i]];
		if(j==l)
		{
			for(k=i+1; k<16; k++)
			{
				j=ctx->qtab[n][btic2b_zigzag2[k]];
				if(j!=l)break;
			}
			j=k-i;
			
			BTIC2BH_WriteSRice(ctx, -j, 3);
			i=k-1;
			continue;
		}

		BTIC2BH_WriteSRice(ctx, j, 3);
		l=j;
	}
	BTIC2BH_WriteSRice(ctx, 0, 3);
	
	sz=BTIC2BH_PopSetOutput(ctx);

	tb[0]='Q'; tb[1]='0'+n; tb[2]=0;
	BTIC2B_WriteVLT(ctx, 0, tb);
	BTIC2B_WriteVLI(ctx, sz);
	BTIC2B_WriteBytes(ctx, tbuf, sz);
}

void BTIC2B_EmitIHDR(BTIC2B_ImageContext *ctx, int xs, int ys)
{
	byte tbuf[1024];
	char tb[4];
	int i, j, k, k, sz, nf;

	BTIC2BH_PushSetOutput(ctx, tbuf, 1024);
	
	BTIC2BH_WriteNBitsL(ctx, xs, 24);	//X Size
	BTIC2BH_WriteNBitsL(ctx, ys, 24);	//Y Size

	BTIC2BH_WriteNBits(ctx, 0, 8);		//Mip Start
	BTIC2BH_WriteNBits(ctx, 0, 8);		//Mip End

	BTIC2BH_WriteNBitsL(ctx, 0, 32);		//Flags

	BTIC2BH_WriteNBits(ctx, 3, 8);		//Image Type
	BTIC2BH_WriteNBits(ctx, 8, 8);		//Pixel Bits
	BTIC2BH_WriteNBits(ctx, 0, 4);		//Pixel Type

//	nf=ctx->ihc_nc;
	nf=0;
	for(i=0; i<ctx->ihc_nc; i++)
		{ if(ctx->ihc_cid[i])nf++; }

	BTIC2BH_WriteNBits(ctx, nf, 4);		//Num Components

	for(i=0; i<ctx->ihc_nc; i++)
	{
		if(!ctx->ihc_cid[i])
			continue;
	
		BTIC2BH_WriteNBits(ctx, ctx->ihc_cid[i], 4);	//Ci
		BTIC2BH_WriteNBits(ctx, ctx->ihc_qid[i], 4);	//Tqi
		BTIC2BH_WriteNBits(ctx, ctx->ihc_ch[i], 4);		//Hi
		BTIC2BH_WriteNBits(ctx, ctx->ihc_cv[i], 4);		//Vi
		BTIC2BH_WriteNBits(ctx, ctx->ihc_cdt[i], 4);	//Tdi
		BTIC2BH_WriteNBits(ctx, ctx->ihc_cat[i], 4);	//Tai
		BTIC2BH_WriteNBits(ctx, ctx->ihc_cbt[i], 4);	//Block Type
		BTIC2BH_WriteNBits(ctx, ctx->ihc_cbs[i], 4);	//Block Size
	}

	sz=BTIC2BH_PopSetOutput(ctx);

	BTIC2B_WriteVLT(ctx, 0, "IH");
	BTIC2B_WriteVLI(ctx, sz);
	BTIC2B_WriteBytes(ctx, tbuf, sz);
}

void BTIC2B_EmitDHT(BTIC2B_ImageContext *ctx, int tab)
{
	byte tbuf[1024];
	char tb[4];
	int i, j, k, k, sz;

	BTIC2BH_PushSetOutput(ctx, tbuf, 1024);
	BTIC2BH_EncodeHuffTable(ctx, tab, ctx->huff_len+tab*256, 256);
	sz=BTIC2BH_PopSetOutput(ctx);

	tb[0]='H'; tb[1]='0'+n; tb[2]=0;
	BTIC2B_WriteVLT(ctx, 0, tb);
	BTIC2B_WriteVLI(ctx, sz);
	BTIC2B_WriteBytes(ctx, tbuf, sz);
}

void BTIC2B_MakeQuantTabInput(BTIC2B_ImageContext *ctx,
	short *in, int cnt, byte *tab, float q)
{
	static double deltas[BTIC2B_DCTSZ2];
	double f, g, te;
	int i, j, k;

	for(j=0; j<BTIC2B_DCTSZ2; j++)
		deltas[j]=0;

	for(i=0; i<cnt; i++)
		for(j=0; j<BTIC2B_DCTSZ2; j++)
			deltas[j]+=fabs(in[i*BTIC2B_DCTSZ2+j]);
	for(j=0; j<BTIC2B_DCTSZ2; j++)
		deltas[j]/=cnt;

	te=0;
	for(j=0; j<BTIC2B_DCTSZ2; j++)
		te+=deltas[j];

	for(i=0; i<BTIC2B_DCTSZ; i++)
	{
		for(j=0; j<BTIC2B_DCTSZ; j++)
		{
			f=deltas[i*BTIC2B_DCTSZ+j];

			f=f/te;
			if(f<0.00001)f=0.00001;
			g=(1.0-q);
			f=10*g*g*g/f;
			k=f;
			if(k<1)k=1;
			if(k>255)k=255;

//			printf("%f/%f %d\n", deltas[i*BTIC2B_DCTSZ+j], te, k);

//			if(f<1)f=1;
//			if(f>255)f=255;
			tab[i*BTIC2B_DCTSZ+j]=k;
		}
	}

#if 1
	if(tab[BTIC2B_DCTSZ2-3]>1)tab[BTIC2B_DCTSZ2-3]*=0.75;
	if(tab[BTIC2B_DCTSZ2-2]>1)tab[BTIC2B_DCTSZ2-2]*=0.75;
	if(tab[BTIC2B_DCTSZ2-1]>1)tab[BTIC2B_DCTSZ2-1]*=0.5;
#endif
}

PDGL_API int BTIC2B_SetupEncode(BTIC2B_ImageContext *ctx,
	byte *ibuf, int xs, int ys, int qf)
{
	int i, j, k, n;

	ctx->xs2b=(xs+BTIC2B_DCTSZ-1)/BTIC2B_DCTSZ;
	ctx->ys2b=(ys+BTIC2B_DCTSZ-1)/BTIC2B_DCTSZ;
	ctx->xs3b=(xs+(2*BTIC2B_DCTSZ-1))/(2*BTIC2B_DCTSZ);
	ctx->ys3b=(ys+(2*BTIC2B_DCTSZ-1))/(2*BTIC2B_DCTSZ);

	ctx->xs2=ctx->xs2b*BTIC2B_DCTSZ;
	ctx->ys2=ctx->ys2b*BTIC2B_DCTSZ;
	ctx->xs3=ctx->xs3b*BTIC2B_DCTSZ;
	ctx->ys3=ctx->ys3b*BTIC2B_DCTSZ;

	ctx->ihc_cxi[0]=ctx->xs2;
	ctx->ihc_cyi[0]=ctx->ys2;
	ctx->ihc_cxi[1]=ctx->xs3;
	ctx->ihc_cyi[1]=ctx->ys3;
	ctx->ihc_cxi[2]=ctx->xs3;
	ctx->ihc_cyi[2]=ctx->ys3;
	ctx->ihc_cxi[3]=ctx->xs2;
	ctx->ihc_cyi[3]=ctx->ys2;

	ctx->is_mono=0;
	ctx->is_420=1;
	ctx->is_alpha=0;

	n=xs*ys;
	for(i=0; i<n; i++)
	{
		j=ibuf[i*4+3];
		if(j<224)ctx->is_alpha=1;
	}

	//full quality
	for(i=0; i<BTIC2B_DCTSZ2; i++)ctx->qtab[0][i]=1;
	for(i=0; i<BTIC2B_DCTSZ2; i++)ctx->qtab[1][i]=1;
	for(i=0; i<BTIC2B_DCTSZ2; i++)ctx->qtab[2][i]=1;
	for(i=0; i<BTIC2B_DCTSZ2; i++)ctx->qtab[3][i]=1;

	//dummy huffman tables
	for(i=0; i<256; i++)ctx->huff_len[0][i]=0;
	for(i=0; i<256; i++)ctx->huff_len[1][i]=0;
	for(i=0; i<256; i++)ctx->huff_len[2][i]=0;
	for(i=0; i<256; i++)ctx->huff_len[3][i]=0;
	for(i=0; i<256; i++)ctx->huff_len[4][i]=0;
	for(i=0; i<256; i++)ctx->huff_len[5][i]=0;

	for(i=0; i<16; i++)ctx->huff_len[0][i]=4;
	for(i=0; i<16; i++)ctx->huff_len[2][i]=4;
	for(i=0; i<16; i++)ctx->huff_len[4][i]=4;
	for(i=0; i<255; i++)ctx->huff_len[1][i]=8;
	for(i=0; i<255; i++)ctx->huff_len[3][i]=8;
	for(i=0; i<255; i++)ctx->huff_len[5][i]=8;

	if((xs!=ctx->lxs) || (ys!=ctx->lys))
	{
		for(i=0; i<8; i++)
		{
			if(ctx->im_scbuf[i])
			{
				free(ctx->im_scbuf[i]);
				ctx->im_scbuf[i]=NULL;
			}

			if(ctx->im_sqbuf[i])
			{
				free(ctx->im_sqbuf[i]);
				ctx->im_sqbuf[i]=NULL;
			}

			if(ctx->im_sibuf[i])
			{
				free(ctx->im_sibuf[i]);
				ctx->im_sibuf[i]=NULL;
			}

			if(ctx->im_sdcbuf[i])
			{
				free(ctx->im_sdcbuf[i]);
				ctx->im_sdcbuf[i]=NULL;
			}
		}

		ctx->im_sibuf[0]=malloc(xs2*ys2*sizeof(s16));
		ctx->im_sibuf[1]=malloc(xs2*ys2*sizeof(s16));
		ctx->im_sibuf[2]=malloc(xs2*ys2*sizeof(s16));
		ctx->im_sibuf[3]=malloc(xs2*ys2*sizeof(s16));

		ctx->im_scbuf[0]=malloc((xs2+8)*(ys2+8)*sizeof(s32));
		ctx->im_scbuf[1]=malloc((xs3+8)*(ys3+8)*sizeof(s32));
		ctx->im_scbuf[2]=malloc((xs3+8)*(ys3+8)*sizeof(s32));
		ctx->im_scbuf[3]=malloc((xs3+8)*(ys3+8)*sizeof(s32));

		ctx->im_sqbuf[0]=malloc((xs2+8)*(ys2+8)*sizeof(s32));
		ctx->im_sqbuf[1]=malloc((xs3+8)*(ys3+8)*sizeof(s32));
		ctx->im_sqbuf[2]=malloc((xs3+8)*(ys3+8)*sizeof(s32));
		ctx->im_sqbuf[3]=malloc((xs3+8)*(ys3+8)*sizeof(s32));

		ctx->lxs=xs;
		ctx->lys=ys;
	}
	
	if(ctx->is_420)
	{
		i=0;
		ctx->ihc_cid[i]=1;
		ctx->ihc_qid[i]=0;
		ctx->ihc_ch[i]=2;
		ctx->ihc_cv[i]=2;
		ctx->ihc_cdt[i]=0;
		ctx->ihc_cat[i]=1;
		ctx->ihc_cbt[i]=0;
		ctx->ihc_cbs[i]=0;

		if(!ctx->is_mono)
		{
			i++;
			ctx->ihc_cid[i]=2;
			ctx->ihc_qid[i]=1;
			ctx->ihc_ch[i]=1;
			ctx->ihc_cv[i]=1;
			ctx->ihc_cdt[i]=2;
			ctx->ihc_cat[i]=3;
			ctx->ihc_cbt[i]=0;
			ctx->ihc_cbs[i]=0;

			i++;
			ctx->ihc_cid[i]=3;
			ctx->ihc_qid[i]=1;
			ctx->ihc_ch[i]=1;
			ctx->ihc_cv[i]=1;
			ctx->ihc_cdt[i]=2;
			ctx->ihc_cat[i]=3;
			ctx->ihc_cbt[i]=0;
			ctx->ihc_cbs[i]=0;
		}else
		{
			i++;
			ctx->ihc_cid[i]=0;

			i++;
			ctx->ihc_cid[i]=0;
		}
		
		if(ctx->is_alpha)
		{
			i++;
			ctx->ihc_cid[i]=5;
			ctx->ihc_qid[i]=0;
			ctx->ihc_ch[i]=2;
			ctx->ihc_cv[i]=2;
			ctx->ihc_cdt[i]=4;
			ctx->ihc_cat[i]=5;
			ctx->ihc_cbt[i]=0;
			ctx->ihc_cbs[i]=0;
		}
		
		ctx->chm=2;
		ctx->cvm=2;
		ctx->ihc_nc=i;
	}
}

PDGL_API int BTIC2B_EncodeColorTransform(BTIC2B_ImageContext *ctx,
	byte *ibuf, int xs, int ys, int qf)
{
	int xs2, xs3;
	int cr, cg, cb, ca;
	int i, j, k;

	xs2=ctx->xs2;
	xs3=ctx->xs3;
	
	for(i=0; i<ys; i++)
		for(j=0; j<xs; j++)
	{
		k=ys-(i+1);
		cr=ibuf[(k*xs+j)*4+0];
		cg=ibuf[(k*xs+j)*4+1];
		cb=ibuf[(k*xs+j)*4+2];
		ca=ibuf[(k*xs+j)*4+3];

		cy=cg-((2*cg-cb-cr)>>2);
		cu=cb-cr;
		cv=(2*cg-cb-cr)>>1;

		ctx->sibuf[0][i*xs2+j]=cy;
		ctx->sibuf[1][i*xs2+j]=cu;
		ctx->sibuf[2][i*xs2+j]=cv;
		ctx->sibuf[3][i*xs2+j]=ca;
	}

	for(i=0; i<ys3; i++)
		for(j=0; j<xs3; j++)
	{
		k=	ctx->sibuf[1][(i*2  )*xs2+j*2  ]+
			ctx->sibuf[1][(i*2  )*xs2+j*2+1]+
			ctx->sibuf[1][(i*2+1)*xs2+j*2  ]+
			ctx->sibuf[1][(i*2+1)*xs2+j*2+1];
		ctx->sibuf[1][i*xs3+j]=k/4;

		k=	ctx->sibuf[2][(i*2  )*xs2+j*2  ]+
			ctx->sibuf[2][(i*2  )*xs2+j*2+1]+
			ctx->sibuf[2][(i*2+1)*xs2+j*2  ]+
			ctx->sibuf[2][(i*2+1)*xs2+j*2+1];
		ctx->sibuf[2][i*xs3+j]=k/4;
	}
}

PDGL_API int BTIC2B_EncodeQuantizePlane(
	BTIC2B_ImageContext *ctx, int pn)
{
	int xsb, ysb;
	int pa, pb, pc, pd;
	int i, j, k, l;

	xsb=(ctx->ihc_cxi[pn]+BTIC2B_DCTSZ-1)/BTIC2B_DCTSZ;
	ysb=(ctx->ihc_cyi[pn]+BTIC2B_DCTSZ-1)/BTIC2B_DCTSZ;

	for(i=0; i<xsb; i++)
		for(j=0; j<ysb; j++)
	{
		pa=(j>0)?ctx->im_sdcbuf[pn][((i  )*xsb+j-1)]:0;
		pb=(i>0)?ctx->im_sdcbuf[pn][((i-1)*xsb+j  )]:0;
		pc=((i>0)&&(j>0))?ctx->im_sdcbuf[0][((i-1)*xsb+j-1)]:0;
		pd=BTIC2B_Paeth(pa, pb, pc);
		
		k=i*xsb+j;
		BTIC2BH_QuantBlock(
			ctx->im_scbuf[pn]+k*BTIC2B_DCTSZ2,
			ctx->im_sqbuf[pn]+k*BTIC2B_DCTSZ2, 0);
		l=ctx->im_sqbuf[pn][k*BTIC2B_DCTSZ2+0];
		ctx->im_sdcbuf[pn][k]=l;
		ctx->im_sqbuf[pn][k*BTIC2B_DCTSZ2+0]=l-pd;
	}
}

PDGL_API int BTIC2B_EncodeComponentImage(BTIC2B_ImageContext *ctx,
	byte *obuf, int osz, byte *ibuf, int xs, int ys, int qf)
{
	static int dcs[256], acs[256];
	static int dcsuv[256], acsuv[256];
	static int dcsa[256], acsa[256];
	short *tp;
//	int xs2, ys2, xs3, ys3;
	int pa, pb, pc, pd;
	int i, j, k, l;

	BTIC2B_SetupEncode(ctx, ibuf, xs, ys, qf);
	BTIC2B_EncodeColorTransform(ctx, ibuf, xs, ys, qf);

	BTIC2B_FilterImageDCT(ctx->sibuf[0], ctx->scbuf[0],
		ctx->ihc_cxi[0], ctx->ihc_cyi[0]);
	if(!ctx->is_mono)
	{
		BTIC2B_FilterImageDCT(ctx->sibuf[1], ctx->scbuf[1],
			ctx->ihc_cxi[1], ctx->ihc_cyi[1]);
		BTIC2B_FilterImageDCT(ctx->sibuf[2], ctx->scbuf[2],
			ctx->ihc_cxi[2], ctx->ihc_cyi[2]);
	}
	if(ctx->is_alpha)
	{
		BTIC2B_FilterImageDCT(ctx->sibuf[3], ctx->scbuf[3],
			ctx->ihc_cxi[3], ctx->ihc_cyi[3]);
	}

	j=ctx->xs2b*ctx->ys2b;
	BTIC2B_MakeQuantTabInput(ctx->scbuf[0], j, ctx->qtab[0], (qf&255)/100.0);

	j=ctx->xs3b*ctx->ys3b;
	BTIC2B_MakeQuantTabInput(ctx->scbuf[1], j, ctx->qtab[1], (qf&255)/100.0);
	BTIC2B_MakeQuantTabInput(ctx->scbuf[2], j, ctx->qtab[2], (qf&255)/100.0);
	for(i=0; i<16; i++)
	{
		ctx->qtab[1][i]=
			(ctx->qtab[1][i]+ctx->qtab[2][i])/2;
	}

	if(ctx->is_alpha)
	{
		BTIC2B_MakeQuantTabInput(ctx->scbuf[3], j,
			ctx->qtab[2], (qf&255)/100.0);
	}

	BTIC2B_EncodeQuantizePlane(ctx, 0);
	if(!ctx->is_mono)
	{
		BTIC2B_EncodeQuantizePlane(ctx, 1);
		BTIC2B_EncodeQuantizePlane(ctx, 2);
	}
	if(ctx->is_alpha)
		{ BTIC2B_EncodeQuantizePlane(ctx, 3); }

	for(i=0; i<256; i++)dcs[i]=0;
	for(i=0; i<256; i++)acs[i]=0;
	for(i=0; i<256; i++)dcsuv[i]=0;
	for(i=0; i<256; i++)acsuv[i]=0;
	for(i=0; i<256; i++)dcsa[i]=0;
	for(i=0; i<256; i++)acsa[i]=0;

	j=ctx->xs2b*ctx->ys2b;
	k=ctx->xs3b*ctx->ys3b;
	for(i=0; i<j; i++)
	{
		BTIC2BH_StatBlock(ctx, ctx->sqbuf[0]+i*BTIC2B_DCTSZ2, dcs, acs);
		if(ctx->is_alpha)
			{ BTIC2BH_StatBlock(ctx, 
				ctx->sqbuf[3]+i*BTIC2B_DCTSZ2, dcsa, acsa); }
	}
	for(i=0; i<k; i++)
	{
		BTIC2BH_StatBlock(ctx, ctx->sqbuf[1]+i*BTIC2B_DCTSZ2, dcsuv, acsuv);
		BTIC2BH_StatBlock(ctx, ctx->sqbuf[2]+i*BTIC2B_DCTSZ2, dcsuv, acsuv);
	}

	BTIC2BH_BuildLengths(dcs, 256, ctx->huff_len+0*256, 16);
	BTIC2BH_BuildLengths(acs, 256, ctx->huff_len+1*256, 16);
	if(!ctx->is_mono)
	{
		BTIC2BH_BuildLengths(dcsuv, 256, ctx->huff_len+2*256, 16);
		BTIC2BH_BuildLengths(acsuv, 256, ctx->huff_len+3*256, 16);
	}
	if(ctx->is_alpha)
	{
		BTIC2BH_BuildLengths(dcsa, 256, ctx->huff_len+4*256, 16);
		BTIC2BH_BuildLengths(acsa, 256, ctx->huff_len+5*256, 16);
	}

//	ctx->ct=obuf;
//	ctx->huff_win=0;
//	ctx->huff_pos=0;

	BTIC2BH_PushSetOutput(ctx, obuf, osz);
	BTIC2BH_PushBeginLumpFcc(ctx, FOURCC('C', 'I', 'M', 'G'));

	BTIC2B_EmitDQT(ctx, 0);
	if(!ctx->is_mono)BTIC2B_EmitDQT(ctx, 1);
	if(ctx->is_alpha)BTIC2B_EmitDQT(ctx, 1);

	BTIC2B_EmitSOF(ctx, xs, ys);

	BTIC2B_EmitDHT(ctx, 0);
	BTIC2B_EmitDHT(ctx, 1);
	if(!ctx->is_mono)
	{
		BTIC2B_EmitDHT(ctx, 2);
		BTIC2B_EmitDHT(ctx, 3);
	}
	if(ctx->is_alpha)
	{
		BTIC2B_EmitDHT(ctx, 4);
		BTIC2B_EmitDHT(ctx, 5);
	}

	BTIC2B_EmitSOS();

	if(ctx->is_mono)
	{
		j=ctx->xs2b*ctx->ys2b;
		for(i=0; i<j; i++)
		{
			BTIC2BH_EncodeBlock(ctx,
				ctx->sqbuf[0]+i*BTIC2B_DCTSZ2, 0, 1);
			if(ctx->is_alpha)
			{
				BTIC2BH_EncodeBlock(ctx,
					ctx->sqbuf[3]+i*BTIC2B_DCTSZ2, 4, 5);
			}
		}
	}else if(ctx->is_420)
	{
		for(i=0; i<ctx->ys3b; i++)
			for(j=0; j<ctx->xs3b; j++)
		{
			k=((i*2+0)*ctx->xs2b+j*2+0);
			BTIC2BH_EncodeBlock(ctx, ctx->sqbuf[0]+k*BTIC2B_DCTSZ2, 0, 1);
			k=((i*2+0)*ctx->xs2b+j*2+1);
			BTIC2BH_EncodeBlock(ctx, ctx->sqbuf[0]+k*BTIC2B_DCTSZ2, 0, 1);
			k=((i*2+1)*ctx->xs2b+j*2+0);
			BTIC2BH_EncodeBlock(ctx, ctx->sqbuf[0]+k*BTIC2B_DCTSZ2, 0, 1);
			k=((i*2+1)*ctx->xs2b+j*2+1);
			BTIC2BH_EncodeBlock(ctx, ctx->sqbuf[0]+k*BTIC2B_DCTSZ2, 0, 1);

			k=i*ctx->xs3b+j;
			BTIC2BH_EncodeBlock(ctx, ctx->sqbuf[1]+k*BTIC2B_DCTSZ2, 2, 3);
			BTIC2BH_EncodeBlock(ctx, ctx->sqbuf[2]+k*BTIC2B_DCTSZ2, 2, 3);

			if(ctx->is_alpha)
			{
				k=((i*2+0)*ctx->xs2b+j*2+0);
				BTIC2BH_EncodeBlock(ctx,
					ctx->sqbuf[3]+k*BTIC2B_DCTSZ2, 4, 5);
				k=((i*2+0)*ctx->xs2b+j*2+1);
				BTIC2BH_EncodeBlock(ctx,
					ctx->sqbuf[3]+k*BTIC2B_DCTSZ2, 4, 5);
				k=((i*2+1)*ctx->xs2b+j*2+0);
				BTIC2BH_EncodeBlock(ctx,
					ctx->sqbuf[3]+k*BTIC2B_DCTSZ2, 4, 5);
				k=((i*2+1)*ctx->xs2b+j*2+1);
				BTIC2BH_EncodeBlock(ctx,
					ctx->sqbuf[3]+k*BTIC2B_DCTSZ2, 4, 5);
			}
		}
	}

	BTIC2BH_FlushBits(ctx);

	BTIC2BH_PushEndLumpFcc(ctx);
	i=BTIC2BH_PopSetOutput(ctx);
	return(i);
}
