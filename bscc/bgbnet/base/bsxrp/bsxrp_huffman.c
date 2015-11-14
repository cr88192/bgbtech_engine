#include <bgbnet.h>

int BSXRP_BalanceTree_r(short *nodes, short *nlen, int root, int h, int ml)
{
	int h0, h1, h2, h3;
	int l0, l1, l2;

	if(root<0)
	{
//		dyPrintf("L");
		return(0);
	}

//	dyPrintf("{");

	h1=BSXRP_BalanceTree_r(nodes, nlen, nodes[root*2+0], h+1, ml);
	h2=BSXRP_BalanceTree_r(nodes, nlen, nodes[root*2+1], h+1, ml);
	h0=((h1>h2)?h1:h2)+1;
	nlen[root]=h0;

	if((h+h0)<=ml)	//depth limit not exceeded
	{
//		dyPrintf("}");
		return(h0);
	}

	//ok, so part of the tree is too deep
	if((h1+1)<h2)
	{
		l0=nodes[root*2+1];
//		if(l0<0)return(h0);	//can't rebalance leaves

		l1=nodes[l0*2+1];
		nodes[l0*2+1]=nodes[l0*2+0];
		nodes[l0*2+0]=nodes[root*2+0];
		nodes[root*2+0]=l0;
		nodes[root*2+1]=l1;
	}else if((h2+1)<h1)
	{
		l0=nodes[root*2+0];
//		if(l0<0)return(h0);	//can't rebalance leaves

		l1=nodes[l0*2+0];
		nodes[l0*2+0]=nodes[l0*2+1];
		nodes[l0*2+1]=nodes[root*2+1];
		nodes[root*2+0]=l1;
		nodes[root*2+1]=l0;
	}else
	{
//		dyPrintf("bal}");
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

//	dyPrintf("rebal}");

	return(h0);
}

void BSXRP_CalcLengths_r(short *nodes, byte *cl, int root, int h)
{
	if(root<0)
	{
		cl[(-root)-1]=h;
		return;
	}

	BSXRP_CalcLengths_r(nodes, cl, nodes[root*2+0], h+1);
	BSXRP_CalcLengths_r(nodes, cl, nodes[root*2+1], h+1);
}

void BSXRP_BuildLengths(int *stat, int nc, byte *cl, int ml)
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

	for(i=0; i<nc; i++)cl[i]=0;
	if(!nr)
	{
//		dyPrintf("empty tree\n");
		return;
	}

//	if(nr==1)
//	{
//		cl[(-roots[0])-1]=1;
//		return;
//	}

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

//		dyPrintf("%d %d %d\n", cnts[j], cnts[k], cnts[nr]);

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
		k=BSXRP_BalanceTree_r(nodes, nlen, l, 0, ml);
	if(k>ml)
	{
		dyPrintf("tree balance failure\n");
		dyPrintf("tree depth %d, org %d, %d nodes\n", k, j, nn);
	}

	BSXRP_CalcLengths_r(nodes, cl, l, 0);
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

int BSXRP_TransposeByte(int v)
{
	return(trans8[v]);
}

int BSXRP_TransposeWord(int v)
{
	int i;
	i=trans8[(v>>8)&0xFF]|(trans8[v&0xFF]<<8);
	return(i);
}

int BSXRP_SetupTable(byte *cl, int ncl,
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

//		dyPrintf("%d %d %d\n", i, j, cnt[i]);
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
				k=BSXRP_TransposeByte(k2++);
				if(ti[k]==0xFFFF)ti[k]=i;
			}
		}else
		{
			k2=tc[i]>>(l-8);
			k=BSXRP_TransposeByte(k2);
			if(ti[k]==0xFFFF)ti[k]=i;
		}

		k2=(tc[i])<<(16-l);
		k=BSXRP_TransposeWord(k2++);
		tc[i]=k;
	}

	j=0;
	for(i=0; i<256; i++)if(ti[i]==0xFFFF)
	{
		dyPrintf("table bad index %d\n", i);
		j=-9;
	}
	return(j);
}

//HACK: fiddle with table and try again if symbols are missing.
void BSXRP_HuffmanLengthSanityAdjust(BSXRP_StreamContext *ctx, int tab)
{
	int i;

	for(i=0; i<384; i++)
		if(ctx->huff_stat[tab][i] && !ctx->huff_len[tab][i])
			break;
	if(i>=384)return;

	for(i=0; i<384; i++)
		if(ctx->huff_stat[tab][i] && !ctx->huff_len[tab][i])
			ctx->huff_stat[tab][i]++;

	for(i=0; i<64; i++)
		ctx->huff_stat[tab][i]++;
	BSXRP_BuildLengths(ctx->huff_stat[tab], 384, ctx->huff_len[tab], 16);
}

void BSXRP_EncodeSymbol(BSXRP_StreamContext *ctx, int tab, int v)
{
	if(!ctx->huff_len[tab][v])
		dyPrintf("BSXRP_EncodeSymbol: no symbol Tab=%d, Sym=%d\n", tab, v);

	BSXRP_WriteNBits(ctx, ctx->huff_code[tab][v],
		ctx->huff_len[tab][v]);
}

int BSXRP_DecodeSymbol(BSXRP_StreamContext *ctx, int tab)
{
	int i, j;

	i=BSXRP_PeekWord(ctx);
	j=ctx->huff_idx[tab][i&0xFF];

	while((i&ctx->huff_mask[tab][j])!=ctx->huff_code[tab][j])
		j=ctx->huff_next[tab][j];

	BSXRP_SkipNBits(ctx, ctx->huff_len[tab][j]);
	return(j);
}

int BSXRP_DecodeHuffTableFixed(BSXRP_StreamContext *ctx,
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

	j=BSXRP_SetupTable(lcl, hl,
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

static int bsxrp_lorder[]={
	16, 17, 18, 0, 8,7, 9,6, 10,5, 11,4, 12,3, 13,2, 14,1, 15, 19};
static int bsxrp_lorder2[]={
	3, 17, 15, 13, 11,9, 7,5, 4,6, 8,10, 12,14, 16,18, 0,1, 2, 19};

int BSXRP_DecodeHufftabSymbol(BSXRP_StreamContext *ctx)
{
	int i;
	i=BSXRP_ReadRice(ctx, 2);
	return(bsxrp_lorder[i]);
}

void BSXRP_EncodeHufftabSymbol(BSXRP_StreamContext *ctx, int v)
{
	BSXRP_WriteRice(ctx, bsxrp_lorder2[v], 2);
}

void BSXRP_StatLengths(BSXRP_StreamContext *ctx,
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

int BSXRP_DecodeCodeLengths(BSXRP_StreamContext *ctx, byte *cl, int ncl)
{
	int i, j, k;

	for(i=0; i<ncl; i++)cl[i]=0;

	i=0; k=0;
	while(i<ncl)
	{
		j=BSXRP_DecodeHufftabSymbol(ctx);
		if(j<0)return(j);

		if(j<17)
			{ k=j; cl[i++]=j; continue; }

		if(j==17)
		{
			j=BSXRP_Read3Bits(ctx)+3;
			while(j--)cl[i++]=k;
			continue;
		}
		if(j==18)
		{
			j=BSXRP_ReadNBits(ctx, 7)+11;
			while(j--)cl[i++]=k;
			continue;
		}
		if(j==19)break;

		return(-7);
	}
	return(0);
}

void BSXRP_EncodeCodeLengths(BSXRP_StreamContext *ctx, byte *cl, int nc)
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
			BSXRP_EncodeHufftabSymbol(ctx, 18);
			BSXRP_WriteNBits(ctx, j-11, 7);
			i+=j;
			continue;
		}
		if(j>2)
		{
			BSXRP_EncodeHufftabSymbol(ctx, 17);
			BSXRP_Write3Bits(ctx, j-3);
			i+=j;
			continue;
		}

		l=cl[i++];
		BSXRP_EncodeHufftabSymbol(ctx, l);

//		dyPrintf("L%d ", l);

		if((l<0) || (l>16))printf("problem: bad code length noted\n");
	}
	BSXRP_EncodeHufftabSymbol(ctx, 19);
//	dyPrintf("\n");
}

int BSXRP_DecodeHuffTable(BSXRP_StreamContext *ctx, int tab)
{
	byte lcl[512];
	int ht, hk, hl;
	int i, j, k;

	ht=BSXRP_ReadNBits(ctx, 4);
	hk=BSXRP_ReadNBits(ctx, 4);
	
	if(ht==0)
	{
		i=BSXRP_DecodeHuffTableFixed(ctx, tab, hk);
		return(i);
	}

	if(ht!=1)
	{
		dyPrintf("Bad Huffman Table Type %d\n", ht);
		return(-1);
	}

	BSXRP_DecodeCodeLengths(ctx, lcl, 384);
	j=BSXRP_SetupTable(lcl, 384,
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

int BSXRP_EncodeHuffTable(BSXRP_StreamContext *ctx,
	int tab, byte *cl, int nc)
{
	int i, j;

	BSXRP_WriteNBits(ctx, 1, 4);
	BSXRP_WriteNBits(ctx, 2, 4);
	BSXRP_EncodeCodeLengths(ctx, cl, nc);

	j=BSXRP_SetupTable(cl, nc,
		ctx->huff_code[tab], ctx->huff_mask[tab], ctx->huff_len[tab],
		ctx->huff_idx[tab], ctx->huff_next[tab]);
	if(j<0)
	{
		dyPrintf("Encode Dynamic Literal/Length Table Problem "
			"Tab=%d, Code=%d\n", tab, j);
		return(j);
	}

	return(0);
}

int BSXRP_CheckEncodeHuffTable(BSXRP_StreamContext *ctx,
	int tab, byte *cl, int nc)
{
	int i;

	for(i=0; i<nc; i++)
		if(cl[i])return(1);
	return(0);
}

static u64 bsxrp_ibase[]={
	//16 bit range
	0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007,
	0x0008, 0x000A, 0x000C, 0x000E,
	
	0x0010, 0x0014, 0x0018, 0x001C, 0x0020, 0x0028, 0x0030, 0x0038,
	0x0040, 0x0050, 0x0060, 0x0070, 0x0080, 0x00A0, 0x00C0, 0x00E0,
	0x0100, 0x0140, 0x0180, 0x01C0, 0x0200, 0x0280, 0x0300, 0x0380,
	0x0400, 0x0500, 0x0600, 0x0700, 0x0800, 0x0A00, 0x0C00, 0x0E00,
	0x1000, 0x1400, 0x1800, 0x1C00, 0x2000, 0x2800, 0x3000, 0x3800,
	0x4000, 0x5000, 0x6000, 0x7000, 0x8000, 0xA000, 0xC000, 0xE000,
	
	//32-bit range
	0x00010000, 0x00014000, 0x00018000, 0x0001C000,
	0x00020000, 0x00028000, 0x00030000, 0x00038000,
	0x00040000, 0x00050000, 0x00060000, 0x00070000,
	0x00080000, 0x000A0000, 0x000C0000, 0x000E0000,

	0x00100000, 0x00140000, 0x00180000, 0x001C0000,
	0x00200000, 0x00280000, 0x00300000, 0x00380000,
	0x00400000, 0x00500000, 0x00600000, 0x00700000,
	0x00800000, 0x00A00000, 0x00C00000, 0x00E00000,

	0x01000000, 0x01400000, 0x01800000, 0x01C00000,
	0x02000000, 0x02800000, 0x03000000, 0x03800000,
	0x04000000, 0x05000000, 0x06000000, 0x07000000,
	0x08000000, 0x0A000000, 0x0C000000, 0x0E000000,

	0x10000000, 0x14000000, 0x18000000, 0x1C000000,
	0x20000000, 0x28000000, 0x30000000, 0x38000000,
	0x40000000, 0x50000000, 0x60000000, 0x70000000,
	0x80000000, 0xA0000000, 0xC0000000, 0xE0000000,

	//64 bit range
	0x0000000100000000, 0x0000000140000000,
	0x0000000180000000, 0x00000001C0000000,
	0x0000000200000000, 0x0000000280000000,
	0x0000000300000000, 0x0000000380000000,
	0x0000000400000000, 0x0000000500000000,
	0x0000000600000000, 0x0000000700000000,
	0x0000000800000000, 0x0000000A00000000,
	0x0000000C00000000, 0x0000000E00000000,

	0x0000001000000000, 0x0000001400000000,
	0x0000001800000000, 0x0000001C00000000,
	0x0000002000000000, 0x0000002800000000,
	0x0000003000000000, 0x0000003800000000,
	0x0000004000000000, 0x0000005000000000,
	0x0000006000000000, 0x0000007000000000,
	0x0000008000000000, 0x000000A000000000,
	0x000000C000000000, 0x000000E000000000,

	0x0000010000000000, 0x0000014000000000,
	0x0000018000000000, 0x000001C000000000,
	0x0000020000000000, 0x0000028000000000,
	0x0000030000000000, 0x0000038000000000,
	0x0000040000000000, 0x0000050000000000,
	0x0000060000000000, 0x0000070000000000,
	0x0000080000000000, 0x00000A0000000000,
	0x00000C0000000000, 0x00000E0000000000,

	0x0000100000000000, 0x0000140000000000,
	0x0000180000000000, 0x00001C0000000000,
	0x0000200000000000, 0x0000280000000000,
	0x0000300000000000, 0x0000380000000000,
	0x0000400000000000, 0x0000500000000000,
	0x0000600000000000, 0x0000700000000000,
	0x0000800000000000, 0x0000A00000000000,
	0x0000C00000000000, 0x0000E00000000000,

	0x0001000000000000, 0x0001400000000000,
	0x0001800000000000, 0x0001C00000000000,
	0x0002000000000000, 0x0002800000000000,
	0x0003000000000000, 0x0003800000000000,
	0x0004000000000000, 0x0005000000000000,
	0x0006000000000000, 0x0007000000000000,
	0x0008000000000000, 0x000A000000000000,
	0x000C000000000000, 0x000E000000000000,

	0x0010000000000000, 0x0014000000000000,
	0x0018000000000000, 0x001C000000000000,
	0x0020000000000000, 0x0028000000000000,
	0x0030000000000000, 0x0038000000000000,
	0x0040000000000000, 0x0050000000000000,
	0x0060000000000000, 0x0070000000000000,
	0x0080000000000000, 0x00A0000000000000,
	0x00C0000000000000, 0x00E0000000000000,

	0x0100000000000000, 0x0140000000000000,
	0x0180000000000000, 0x01C0000000000000,
	0x0200000000000000, 0x0280000000000000,
	0x0300000000000000, 0x0380000000000000,
	0x0400000000000000, 0x0500000000000000,
	0x0600000000000000, 0x0700000000000000,
	0x0800000000000000, 0x0A00000000000000,
	0x0C00000000000000, 0x0E00000000000000,

	0x1000000000000000, 0x1400000000000000,
	0x1800000000000000, 0x1C00000000000000,
	0x2000000000000000, 0x2800000000000000,
	0x3000000000000000, 0x3800000000000000,
	0x4000000000000000, 0x5000000000000000,
	0x6000000000000000, 0x7000000000000000,
	0x8000000000000000LL, 0xA000000000000000LL,
	0xC000000000000000LL, 0xE000000000000000LL,
	};

static int bsxrp_iextra[]={
		 0,  0,  0,  0,   0,  0,  0,  0,
		 1,  1,  1,  1,
		 2,  2,  2,  2,   3,  3,  3,  3,
		 4,  4,  4,  4,   5,  5,  5,  5,
		 6,  6,  6,  6,   7,  7,  7,  7,
		 8,  8,  8,  8,   9,  9,  9,  9,
		10, 10, 10, 10,  11, 11, 11, 11,
		12, 12, 12, 12,  13, 13, 13, 13,
		
		//32 bit range
		14, 14, 14, 14,  15, 15, 15, 15,
		16, 16, 16, 16,  17, 17, 17, 17,

		18, 18, 18, 18,  19, 19, 19, 19,
		20, 20, 20, 20,  21, 21, 21, 21,

		22, 22, 22, 22,  23, 23, 23, 23,
		24, 24, 24, 24,  25, 25, 25, 25,

		26, 26, 26, 26,  27, 27, 27, 27,
		28, 28, 28, 28,  29, 29, 29, 29,
		
		//64 bit range
		30, 30, 30, 30,  31, 31, 31, 31,
		32, 32, 32, 32,  33, 33, 33, 33,

		34, 34, 34, 34,  35, 35, 35, 35,
		36, 36, 36, 36,  37, 37, 37, 37,

		38, 38, 38, 38,  39, 39, 39, 39,
		40, 40, 40, 40,  41, 41, 41, 41,

		42, 42, 42, 42,  43, 43, 43, 43,
		44, 44, 44, 44,  45, 45, 45, 45,

		46, 46, 46, 46,  47, 47, 47, 47,
		48, 48, 48, 48,  49, 49, 49, 49,

		50, 50, 50, 50,  51, 51, 51, 51,
		52, 52, 52, 52,  53, 53, 53, 53,

		54, 54, 54, 54,  55, 55, 55, 55,
		56, 56, 56, 56,  57, 57, 57, 57,

		58, 58, 58, 58,  59, 59, 59, 59,
		60, 60, 60, 60,  61, 61, 61, 61,
	};

void BSXRP_EncodeUInteger(BSXRP_StreamContext *ctx, u64 val)
{
//	static int dbase[]={
//		1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193,
//		257, 385, 513, 769, 1025, 1537, 2049, 3073,
//		4097, 6145, 8193, 12289, 16385, 24577, 32769, 49153};
//	static int dextra[]={
//		0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6,
//		7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14};

	u64 lj, lk;
	int i;

	lj=val;
	for(i=0; i<252; i++)
	{
		lk=bsxrp_ibase[i]+(1ULL<<bsxrp_iextra[i]);
		if((val>=bsxrp_ibase[i]) && (lj<lk))
		{
			BSXRP_EncodeSymbol(ctx, 2, i);
			BSXRP_WriteNBitsL(ctx, lj-bsxrp_ibase[i], bsxrp_iextra[i]);
			break;
		}
	}
}

u64 BSXRP_DecodeUInteger(BSXRP_StreamContext *ctx)
{
	u64 lk;
	int i;

	i=BSXRP_DecodeSymbol(ctx, 2);
	lk=bsxrp_ibase[i]+BSXRP_ReadNBitsL(ctx, bsxrp_iextra[i]);
	return(lk);
}

void BSXRP_EncodeSInteger(BSXRP_StreamContext *ctx, s64 val)
{
	u64 v;
	
	v=(val>=0)?(((u64)val)<<1):((((u64)(-val))<<1)-1);
	BSXRP_EncodeUInteger(ctx, v);
}

s64 BSXRP_DecodeSInteger(BSXRP_StreamContext *ctx)
{
	u64 lj;
	s64 v;
	
	lj=BSXRP_DecodeUInteger(ctx);
	v=(lj&1)?(-((s64)((lj+1)>>1))):(lj>>1);
	return(v);
}

int BSXRP_StatUIntPrefix(BSXRP_StreamContext *ctx, u64 val)
{
	u64 lj, lk;
	int i;

	lj=val;
	for(i=0; i<252; i++)
	{
		lk=bsxrp_ibase[i]+(1ULL<<bsxrp_iextra[i]);
		if((val>=bsxrp_ibase[i]) && (lj<lk))
			return(i);
	}
	return(-1);
}

int BSXRP_StatSIntPrefix(BSXRP_StreamContext *ctx, u64 val)
{
	u64 v;
	v=(val>=0)?(((u64)val)<<1):((((u64)(-val))<<1)-1);
	return(BSXRP_StatUIntPrefix(ctx, v));
}

void BSXRP_EncodeDistance(BSXRP_StreamContext *ctx, int val)
{
	BSXRP_EncodeUInteger(ctx, val);
}

int BSXRP_DecodeDistance(BSXRP_StreamContext *ctx)
{
	return((int)BSXRP_DecodeUInteger(ctx));
}

void BSXRP_EncodeValueIndex(BSXRP_StreamContext *ctx, int val)
{
	u64 lj, lk;
	int i;

	lj=val;
	for(i=0; i<124; i++)
	{
		lk=bsxrp_ibase[i]+(1ULL<<bsxrp_iextra[i]);
		if((val>=bsxrp_ibase[i]) && (lj<lk))
		{
			BSXRP_EncodeSymbol(ctx, 0, 128+i);
			BSXRP_WriteNBitsL(ctx, lj-bsxrp_ibase[i], bsxrp_iextra[i]);
			break;
		}
	}
}

u64 BSXRP_DecodeValueIndex(BSXRP_StreamContext *ctx, int pf)
{
	u64 lk;
	int i;

	i=pf-128;
	lk=bsxrp_ibase[i]+BSXRP_ReadNBitsL(ctx, bsxrp_iextra[i]);
	return(lk);
}

#if 0
int BSXRP_DecodeDistance(BSXRP_StreamContext *ctx)
{
	static int dbase[]={
		1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193,
		257, 385, 513, 769, 1025, 1537, 2049, 3073,
		4097, 6145, 8193, 12289, 16385, 24577, 32769, 49153};
	static int dextra[]={
		0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6,
		7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14};

	int i, j, k;

	i=BSXRP_PeekWord();
	j=pdunz_dtab_idx[i&0xFF];

	while((i&pdunz_dtab_mask[j])!=pdunz_dtab_code[j])
		j=pdunz_dtab_next[j];

	BSXRP_SkipNBits(pdunz_dtab_len[j]);
	k=dbase[j]+BSXRP_ReadNBits(dextra[j]);

	return(k);
}
#endif
