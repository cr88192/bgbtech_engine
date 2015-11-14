#include <bgbmid.h>

extern const u32 btac2_dbase[64];
extern const int btac2_dextra[64];

extern const u32 btac2_lbase[64];
extern const int btac2_lextra[64];

extern const int btac2_lorder[];
extern const int btac2_lorder2[];

FILE *btac2_encode_dbglogfd=NULL;

void btac2_encode_dbgPrintLog(char *str, ...)
{
	va_list lst;
	
	if(!btac2_encode_dbglogfd)
		return;
	
	va_start(lst, str);
	vfprintf(btac2_encode_dbglogfd, str, lst);
	va_end(lst);
}

void BTAC2_Encode_WriteBit(BGBMID_BTAC2_Context *ctx, int i)
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

void BTAC2_Encode_WriteNBits(BGBMID_BTAC2_Context *ctx, int i, int n)
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

void BTAC2_Encode_WriteNBitsL(BGBMID_BTAC2_Context *ctx, u64 i, int n)
{
	while(n>=16)
	{
		BTAC2_Encode_WriteNBits(ctx, i&65535, 16);
		i=i>>16;
		i-=16;
	}
	BTAC2_Encode_WriteNBits(ctx, i, n);
}

void BTAC2_Encode_Write2Bits(BGBMID_BTAC2_Context *ctx, int i)
	{ BTAC2_Encode_WriteNBits(ctx, i, 2); }
void BTAC2_Encode_Write3Bits(BGBMID_BTAC2_Context *ctx, int i)
	{ BTAC2_Encode_WriteNBits(ctx, i, 3); }
void BTAC2_Encode_Write4Bits(BGBMID_BTAC2_Context *ctx, int i)
	{ BTAC2_Encode_WriteNBits(ctx, i, 4); }

void BTAC2_Encode_WriteRice(BGBMID_BTAC2_Context *ctx, int v, int k)
{
	int j;
	
	j=v>>k;
	while(j--)
	 { BTAC2_Encode_WriteBit(ctx, 1); }
	BTAC2_Encode_WriteBit(ctx, 0);
	j=v&((1<<k)-1);
	BTAC2_Encode_WriteNBits(ctx, j, k);
}

int BTAC2_Encode_CountWriteRice(BGBMID_BTAC2_Context *ctx, int v, int k)
{
	int j;
	j=v>>k;
	return(j+k);
}

void BTAC2_Encode_WriteSRice(BGBMID_BTAC2_Context *ctx, int v, int k)
{
	BTAC2_Encode_WriteRice(ctx, (v>=0)?(v<<1):(((-v)<<1)-1), k);
}

int BTAC2_Encode_CountWriteSRice(BGBMID_BTAC2_Context *ctx, int v, int k)
{
	return(BTAC2_Encode_CountWriteRice(ctx,
		(v>=0)?(v<<1):(((-v)<<1)-1), k));
}

void BTAC2_Encode_WriteExpRice(BGBMID_BTAC2_Context *ctx, int v, int k)
{
	int i, j;
	
	if(v<=0)
	{
		BTAC2_Encode_WriteRice(ctx, 0, k);
		return;
	}
	
	i=v; j=0;
	while(i>0) { i>>=1; j++; }
	BTAC2_Encode_WriteRice(ctx, j, k);
	j=v&((1<<k)-1);
	BTAC2_Encode_WriteNBitsL(ctx, j, i);
}

void BTAC2_Encode_WriteExpSRice(BGBMID_BTAC2_Context *ctx, int v, int k)
{
	BTAC2_Encode_WriteExpRice(ctx, (v>=0)?(v<<1):(((-v)<<1)-1), k);
}

void BTAC2_Encode_FlushBits(BGBMID_BTAC2_Context *ctx)
{
	int i;
	while(ctx->huff_pos>0)
	{
		i=ctx->huff_win&0xFF;
		*ctx->ct++=i;
		ctx->huff_win>>=8;
		ctx->huff_pos-=8;
	}
	
	ctx->huff_win=0;
	ctx->huff_pos=0;
}

#if 0
void BTAC2_Encode_PushSetOutput(BGBMID_BTAC2_Context *ctx,
	byte *obuf, int sz)
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

int BTAC2_Encode_PopSetOutput(BGBMID_BTAC2_Context *ctx)
{
	int sz;

	BTAC2_Encode_FlushBits();
	
	sz=ctx->ct-ctx->cts;

	i=--ctx->huff_stk_pos;
	ctx->ct=ctx->huff_stk_ct[i];
	ctx->cts=ctx->huff_stk_cts[i];
	ctx->cte=ctx->huff_stk_cte[i];
	
	return(sz);
}
#endif


int BTAC2_Encode_BalanceTree_r(short *nodes, short *nlen,
	int root, int h, int ml)
{
	int h0, h1, h2, h3;
	int l0, l1, l2;

	if(root<0)return(0);

	h1=BTAC2_Encode_BalanceTree_r(nodes, nlen, nodes[root*2+0], h+1, ml);
	h2=BTAC2_Encode_BalanceTree_r(nodes, nlen, nodes[root*2+1], h+1, ml);
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

void BTAC2_Encode_CalcLengths_r(short *nodes, byte *cl, int root, int h)
{
	if(root<0)
	{
		if(root==-513)return;
		cl[(-root)-1]=h;
		return;
	}

	BTAC2_Encode_CalcLengths_r(nodes, cl, nodes[root*2+0], h+1);
	BTAC2_Encode_CalcLengths_r(nodes, cl, nodes[root*2+1], h+1);
}

int BTAC2_Encode_BuildLengths(int *stat, int nc, byte *cl, int ml)
{
	short nodes[1024], nlen[512];
	short roots[512], clen[512];
	int cnts[512];
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
		k=BTAC2_Encode_BalanceTree_r(nodes, nlen, l, 0, ml);
	if(k>ml)return(-2);	//failed to balance tree

	BTAC2_Encode_CalcLengths_r(nodes, cl, l, 0);
	return(0);
}

int BTAC2_Encode_BuildLengthsAdjust(int *stat, int nc, byte *cl, int ml)
{
	int i, j;

	while(1)
	{
		j=BTAC2_Encode_BuildLengths(stat, nc, cl, ml);
		if(j<0)
			printf("BTAC2_Encode_BuildLengthsAdjust: Huff Fail %d\n");

		for(i=0; i<nc; i++)
			if(stat[i] && !cl[i])
				break;
		if(i>=nc)break;

		printf("BTAC2_Encode_BuildLengthsAdjust: Fiddle Adjust\n");
		for(i=0; i<nc; i++)
			stat[i]++;
		continue;
	}
	return(0);
}

//static int btac2_lorder[]={
//	16, 17, 18, 0, 8,7, 9,6, 10,5, 11,4, 12,3, 13,2, 14,1, 15, 19};
//static int btac2_lorder2[]={
//	3, 17, 15, 13, 11,9, 7,5, 4,6, 8,10, 12,14, 16,18, 0,1, 2, 19};

//int BTAC2_Decode_DecodeHufftabSymbol(BGBMID_BTAC2_Context *ctx)
//{
//	int i;
//	i=BTAC2_Decode_ReadRice(ctx, 2);
//	return(btac2_lorder[i]);
//}

void BTAC2_Encode_EncodeHufftabSymbol(BGBMID_BTAC2_Context *ctx,
	int v, int rk)
{
//	BTAC2_Encode_WriteRice(ctx, btac2_lorder2[v], 2);
	BTAC2_Encode_WriteRice(ctx, btac2_lorder2[v], rk);
}

int BTAC2_Encode_CountEncodeHufftabSymbol(
	BGBMID_BTAC2_Context *ctx, int v, int rk)
{
	return(BTAC2_Encode_CountWriteRice(ctx, btac2_lorder2[v], rk));
}

void BTAC2_Encode_StatLengths(BGBMID_BTAC2_Context *ctx,
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

void BTAC2_Encode_EncodeCodeLengths(BGBMID_BTAC2_Context *ctx,
	byte *cl, int nc, int rk)
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
			BTAC2_Encode_EncodeHufftabSymbol(ctx, 18, rk);
			BTAC2_Encode_WriteNBits(ctx, j-11, 7);
			i+=j;
			continue;
		}
		if(j>2)
		{
			BTAC2_Encode_EncodeHufftabSymbol(ctx, 17, rk);
			BTAC2_Encode_Write3Bits(ctx, j-3);
			i+=j;
			continue;
		}

		l=cl[i++];
		BTAC2_Encode_EncodeHufftabSymbol(ctx, l, rk);

		if((l<0) || (l>16))printf("problem: bad code length noted\n");
	}
	BTAC2_Encode_EncodeHufftabSymbol(ctx, 19, rk);
}

int BTAC2_Encode_CountEncodeCodeLengths(BGBMID_BTAC2_Context *ctx,
	byte *cl, int nc, int rk)
{
	int i, j, l, lc;

	i=0; l=-1; lc=0;
	while(i<nc)
	{
		for(j=0; ((i+j)<nc) && (cl[i+j]==l); j++);
		if(j>10)
		{
			//check for end of table
			if(((i+j)>=nc) && (l==0))
				break;
		
			if(j>138)j=138;
			lc+=BTAC2_Encode_CountEncodeHufftabSymbol(ctx, 18, rk);
			lc+=7;
			i+=j;
			continue;
		}
		if(j>2)
		{
			lc+=BTAC2_Encode_CountEncodeHufftabSymbol(ctx, 17, rk);
			lc+=3;
			i+=j;
			continue;
		}

		l=cl[i++];
		lc+=BTAC2_Encode_CountEncodeHufftabSymbol(ctx, l, rk);

		if((l<0) || (l>16))printf("problem: bad code length noted\n");
	}
	lc+=BTAC2_Encode_CountEncodeHufftabSymbol(ctx, 19, rk);
	return(lc);
}

int BTAC2_Encode_EncodeHuffTable(BGBMID_BTAC2_Context *ctx,
	int tab, byte *cl, int nc)
{
	int i, j, rk, lc;

	rk=2; lc=999999999;
	for(i=1; i<15; i++)
	{
		j=BTAC2_Encode_CountEncodeCodeLengths(ctx, cl, nc, i);
		if(j<lc) { rk=i; lc=j; }
	}

	printf("BTAC2_Encode_EncodeHuffTable: %d %d\n", rk, lc);

	BTAC2_Encode_WriteNBits(ctx, 1, 4);		//tsz
	BTAC2_Encode_WriteNBits(ctx, rk, 4);	//rice factor
	BTAC2_Encode_EncodeCodeLengths(ctx, cl, nc, rk);

#if 0
	j=BTIC2BH_SetupTable(cl, nc,
		ctx->huff_code[tab], ctx->huff_mask[tab], ctx->huff_len[tab],
		ctx->huff_idx[tab], ctx->huff_next[tab]);
	if(j<0)
	{
		printf("Encode Dynamic Literal/Length Table Problem "
			"Tab=%d, Code=%d\n", tab, j);
		return(j);
	}
#endif

	return(0);
}

byte *BTAC2_Encode_EmitBeginMarkTWOCC(
	BGBMID_BTAC2_Context *ctx, int a, int b)
{
	byte *pt;

	pt=ctx->ct;
	*ctx->ct++=0;
	*ctx->ct++=0;
	*ctx->ct++=a;
	*ctx->ct++=b;
	
	return(pt);
}

void BTAC2_Encode_EmitEndMarkTWOCC(
	BGBMID_BTAC2_Context *ctx, byte *pt)
{
	int i;
	BTAC2_Encode_FlushBits(ctx);
	
	i=ctx->ct-pt;
	pt[0]=i&255;
	pt[1]=(i>>8)&255;
	
	printf("BTAC2_Encode_EmitEndMarkTWOCC: %c%c %d\n", pt[2], pt[3], i);
}

byte *BTAC2_Encode_EmitTagMarkTWOCC(
	BGBMID_BTAC2_Context *ctx, int a, int b)
{
	byte *pt;

	pt=ctx->ct;
	*ctx->ct++=4;
	*ctx->ct++=0;
	*ctx->ct++=a;
	*ctx->ct++=b;
	
	return(pt);
}

byte *BTAC2_Encode_EmitBeginMarkFOURCC(
	BGBMID_BTAC2_Context *ctx, int a, int b, int c, int d)
{
	byte *pt;

	pt=ctx->ct;
	*ctx->ct++=2;	*ctx->ct++=0;
	*ctx->ct++=0;	*ctx->ct++=0;

	*ctx->ct++=0;	*ctx->ct++=0;
	*ctx->ct++=0;	*ctx->ct++=0;

	*ctx->ct++=a;	*ctx->ct++=b;
	*ctx->ct++=c;	*ctx->ct++=d;
	
	return(pt);
}

void BTAC2_Encode_EmitEndMarkFOURCC(
	BGBMID_BTAC2_Context *ctx, byte *pt)
{
	int i;
	BTAC2_Encode_FlushBits(ctx);
	
	i=ctx->ct-pt;
	pt[4]=i&255;
	pt[5]=(i>>8)&255;
	pt[6]=(i>>16)&255;
	pt[7]=(i>>24)&255;
	
	printf("BTAC2_Encode_EmitEndMarkFOURCC: %c%c%c%c %d\n",
		pt[8], pt[9], pt[10], pt[11], i);
}

byte *BTAC2_Encode_EmitTagMarkFOURCC(
	BGBMID_BTAC2_Context *ctx, int a, int b, int c, int d)
{
	byte *pt;

	pt=ctx->ct;
	*ctx->ct++=4;
	*ctx->ct++=0;
	*ctx->ct++=a;
	*ctx->ct++=b;
	
	return(pt);
}

void BTAC2_Encode_EmitFourCC(
	BGBMID_BTAC2_Context *ctx, int a, int b, int c, int d)
{
	*ctx->ct++=a;	*ctx->ct++=b;
	*ctx->ct++=c;	*ctx->ct++=d;
}

void BTAC2_Encode_EmitEightCC(
	BGBMID_BTAC2_Context *ctx,
	int a, int b, int c, int d,
	int e, int f, int g, int h)
{
	*ctx->ct++=a;	*ctx->ct++=b;
	*ctx->ct++=c;	*ctx->ct++=d;
	*ctx->ct++=e;	*ctx->ct++=f;
	*ctx->ct++=g;	*ctx->ct++=h;
}

void BTAC2_Encode_EmitBytes(
	BGBMID_BTAC2_Context *ctx, byte *data, int sz)
{
	if((ctx->ct+sz)>ctx->cte)
	{
		printf("BTAC2_Encode_EmitBytes: Overrun\n");
		return;
	}

	memcpy(ctx->ct, data, sz);
	ctx->ct+=sz;
}

void BTAC2_Encode_EmitByte(
	BGBMID_BTAC2_Context *ctx, int x)
{
	*ctx->ct++=x&255;
}

void BTAC2_Encode_EmitWord(
	BGBMID_BTAC2_Context *ctx, int x)
{
	*ctx->ct++=x&255;
	*ctx->ct++=(x>>8)&255;
}

void BTAC2_Encode_EmitDWord(
	BGBMID_BTAC2_Context *ctx, int x)
{
	*ctx->ct++=x&255;
	*ctx->ct++=(x>>8)&255;
	*ctx->ct++=(x>>16)&255;
	*ctx->ct++=(x>>24)&255;
}

void BTAC2_Encode_EmitQuantTabValue(BGBMID_BTAC2_Context *ctx,
	int val, int rk)
{
	int i, j, k;

	j=((val>=0)?(val<<1):(((-val)<<1)-1));
//	j=val;

	for(i=0; i<64; i++)
	{
		k=btac2_dbase[i]+(1<<btac2_dextra[i]);
		if((j>=btac2_dbase[i]) && (j<k))
		{
			BTAC2_Encode_WriteSRice(ctx, i, rk);
			BTAC2_Encode_WriteNBits(ctx,
				j-btac2_dbase[i],
				btac2_dextra[i]);
			break;
		}
	}
}

int BTAC2_Encode_CountEmitQuantTabValue(BGBMID_BTAC2_Context *ctx,
	int val, int rk)
{
	int i, j, k, l;

	j=((val>=0)?(val<<1):(((-val)<<1)-1));
//	j=val;

	for(i=0; i<64; i++)
	{
		k=btac2_dbase[i]+(1<<btac2_dextra[i]);
		if((j>=btac2_dbase[i]) && (j<k))
		{
			l=BTAC2_Encode_CountWriteSRice(ctx, i, rk);
			l+=btac2_dextra[i];
			return(l);
		}
	}
	
	return(999999);
}

void BTAC2_Encode_EmitQuantTabSz(BGBMID_BTAC2_Context *ctx,
	int *tab, int blen, int rk)
{
	int i, j, k, l;

	l=0;
	for(i=0; i<blen; i++)
	{
		j=tab[i];
		if(j==l)
		{
			for(k=i+1; k<blen; k++)
			{
				j=tab[k];
				if(j!=l)break;
			}
			j=k-i;
			
			printf("%d ", -j);
			BTAC2_Encode_WriteSRice(ctx, -j, rk);
			i=k-1;
			continue;
		}

		printf("%d(%d) ", j, j-l);
//		BTAC2_Encode_WriteSRice(ctx, j, rk);
		BTAC2_Encode_EmitQuantTabValue(ctx, j-l, rk);
		l=j;
	}
//	BTAC2_Encode_WriteSRice(ctx, 0, rk);
	printf("\n");
}

int BTAC2_Encode_CountQuantTabSz(BGBMID_BTAC2_Context *ctx,
	int *tab, int blen, int rk)
{
	int i, j, k, l, lc;

	lc=0; l=0;
	for(i=0; i<blen; i++)
	{
		j=tab[i];
		if(j==l)
		{
			for(k=i+1; k<blen; k++)
			{
				j=tab[k];
				if(j!=l)break;
			}
			j=k-i;
			
//			printf("%d ", -j);
			lc+=BTAC2_Encode_CountWriteSRice(ctx, -j, rk);
			i=k-1;
			continue;
		}

//		printf("%d ", j);
//		lc+=BTAC2_Encode_CountWriteSRice(ctx, j, rk);
		lc+=BTAC2_Encode_CountEmitQuantTabValue(ctx, j-l, rk);
		l=j;
	}
//	lc+=BTAC2_Encode_CountWriteSRice(ctx, 0, rk);
//	printf("\n");
	
	return(lc);
}

void BTAC2_Encode_EmitDQT(BGBMID_BTAC2_Context *ctx, int n)
{
	static char *tidx="0123456789abcdefghijklmnopqrstuvwxyz";
//	byte tbuf[1024];
	char tb[4];
	byte *pt;
	int sz, ts, rk, lc;
	int i, j, k, l;

	if(!ctx->qtab_min[n])
		return;

//	BTIC2BH_PushSetOutput(ctx, tbuf, 1024);
	
	rk=3; lc=999999999;
	
	for(i=1; i<16; i++)
	{
		j=BTAC2_Encode_CountQuantTabSz(ctx,
			ctx->qtab_min[n], ctx->btac_blklen, i);
		j+=BTAC2_Encode_CountQuantTabSz(ctx,
			ctx->qtab_max[n], ctx->btac_blklen, i);
		if(j<lc) { rk=i; lc=j; }
	}
	
	printf("BTAC2_Encode_EmitDQT: %d %d\n", rk, lc);
	
//	pt=BTAC2_Encode_EmitBeginMarkTWOCC(ctx, 'Q', '0'+n);
	pt=BTAC2_Encode_EmitBeginMarkTWOCC(ctx, 'Q', tidx[n]);
	
	ts=bgbmid_log2up(ctx->btac_blklen)-4;
	
	BTAC2_Encode_WriteNBits(ctx, ts, 4);		//table size
	BTAC2_Encode_WriteNBits(ctx, rk, 4);	//rice factor

	BTAC2_Encode_EmitQuantTabSz(ctx,
		ctx->qtab_min[n], ctx->btac_blklen, rk);
	BTAC2_Encode_EmitQuantTabSz(ctx,
		ctx->qtab_max[n], ctx->btac_blklen, rk);
	
	BTAC2_Encode_EmitEndMarkTWOCC(ctx, pt);
	
//	sz=BTIC2BH_PopSetOutput(ctx);
//	tb[0]='Q'; tb[1]='0'+n; tb[2]=0;
//	BTIC2B_WriteVLT(ctx, 0, tb);
//	BTIC2B_WriteVLI(ctx, sz);
//	BTIC2B_WriteBytes(ctx, tbuf, sz);
}

void BTAC2_Encode_EmitDHT(BGBMID_BTAC2_Context *ctx, int n)
{
	static char *tidx="0123456789abcdefghijklmnopqrstuvwxyz";
	byte *pt;

	if((n<0) || (n>35))
		return;
	if(!ctx->huff_len[n])
		return;

//	pt=BTAC2_Encode_EmitBeginMarkTWOCC(ctx, 'H', '0'+n);
	pt=BTAC2_Encode_EmitBeginMarkTWOCC(ctx, 'H', tidx[n]);
	BTAC2_Encode_EncodeHuffTable(ctx, n, ctx->huff_len[n], 256);
	BTAC2_Encode_EmitEndMarkTWOCC(ctx, pt);
}

void BTAC2_Encode_EncodeHeaderTables(BGBMID_BTAC2_Context *ctx)
{
	byte tbuf[65536];
	byte *pt;
	int i, j, k, sz;
	
	ctx->ct=tbuf;
	ctx->huff_win=0;
	ctx->huff_pos=0;
	
//	BTAC2_Encode_EmitTagMarkTWOCC(ctx, 'S', 'H');

	pt=BTAC2_Encode_EmitBeginMarkTWOCC(ctx, 'H', 'D');
	BTAC2_Encode_EmitEightCC(ctx, 'B', 'T', 'A', 'C', '2', 'A', 0, 0);
	BTAC2_Encode_EmitWord(ctx, ctx->btac_blklen);
	BTAC2_Encode_EmitWord(ctx, ctx->btac_blkbytes);
	BTAC2_Encode_EmitDWord(ctx, ctx->flags);
	BTAC2_Encode_EmitWord(ctx, 0);
	BTAC2_Encode_EmitByte(ctx, ctx->chan);
	BTAC2_Encode_EmitByte(ctx, ctx->bits);
	BTAC2_Encode_EmitDWord(ctx, ctx->rate);
	BTAC2_Encode_EmitDWord(ctx, ctx->len);
	BTAC2_Encode_EmitEndMarkTWOCC(ctx, pt);

	for(i=0; i<8; i++)
		BTAC2_Encode_EmitDQT(ctx, i);
	for(i=0; i<8; i++)
		BTAC2_Encode_EmitDHT(ctx, i);

	BTAC2_Encode_EmitTagMarkTWOCC(ctx, 'Z', 'Z');
	
	sz=ctx->ct-tbuf;
	
	ctx->headtab=gcatomic(sz);
	memcpy(ctx->headtab, tbuf, sz);
	ctx->sz_headtab=sz;
	
	printf("BTAC2_Encode_EncodeHeaderTables: %d\n", sz);
}

int BTAC2_Encode_CountEncodeSymbol(BGBMID_BTAC2_Context *ctx,
	int tab, int v)
{
	int i;
	i=ctx->huff_len[tab][v];
	if(!i) { i=999999; }
	return(i);
}

void BTAC2_Encode_EncodeSymbol(BGBMID_BTAC2_Context *ctx, int tab, int v)
{
	if(!ctx->huff_len[tab][v])
	{
		printf("BTAC2_Encode_EncodeSymbol: Bad Symbol %d %d\n", tab, v);
	}

	BTAC2_Encode_WriteNBits(ctx,
		ctx->huff_code[tab][v],
		ctx->huff_len[tab][v]);
}

int BTAC2_Encode_CountEncodeCoeffUValue(BGBMID_BTAC2_Context *ctx,
	int tab, int pfb, u32 val)
{
	int i, j, k, l;

	j=val; l=-1;
	for(i=0; i<64; i++)
	{
		k=btac2_dbase[i]+(1<<btac2_dextra[i]);
		if((j>=btac2_dbase[i]) && (j<k))
		{
			l=BTAC2_Encode_CountEncodeSymbol(ctx, tab, pfb+i);
			l+=btac2_dextra[i];
			break;
		}
	}
	return(l);
}

void BTAC2_Encode_EncodeCoeffUValue(BGBMID_BTAC2_Context *ctx,
	int tab, int pfb, u32 val)
{
	int i, j, k;

	j=val;
	for(i=0; i<64; i++)
	{
		k=btac2_dbase[i]+(1<<btac2_dextra[i]);
		if((j>=btac2_dbase[i]) && (j<k))
		{
			BTAC2_Encode_EncodeSymbol(ctx, tab, pfb+i);
			BTAC2_Encode_WriteNBits(ctx,
				j-btac2_dbase[i],
				btac2_dextra[i]);
			break;
		}
	}
}

void BTAC2_Encode_StatCoeffUValue(BGBMID_BTAC2_Context *ctx,
	int *stat, int pfb, u32 val)
{
	int i, j, k;

	j=val;
	for(i=0; i<64; i++)
	{
		k=btac2_dbase[i]+(1<<btac2_dextra[i]);
		if((j>=btac2_dbase[i]) && (j<k))
		{
			stat[pfb+i]++;
			break;
		}
	}
}


int BTAC2_Encode_CountEncodeLenUValue(BGBMID_BTAC2_Context *ctx,
	int tab, int pfb, u32 val)
{
	int i, j, k, l;

	j=val; l=-1;
	for(i=0; i<64; i++)
	{
		k=btac2_lbase[i]+(1<<btac2_lextra[i]);
		if((j>=btac2_lbase[i]) && (j<k))
		{
			l=BTAC2_Encode_CountEncodeSymbol(ctx, tab, pfb+i);
			l+=btac2_lextra[i];
			break;
		}
	}
	return(l);
}

void BTAC2_Encode_EncodeLenUValue(BGBMID_BTAC2_Context *ctx,
	int tab, int pfb, u32 val)
{
	int i, j, k;

	j=val;
	for(i=0; i<64; i++)
	{
		k=btac2_lbase[i]+(1<<btac2_lextra[i]);
		if((j>=btac2_lbase[i]) && (j<k))
		{
			BTAC2_Encode_EncodeSymbol(ctx, tab, pfb+i);
			BTAC2_Encode_WriteNBits(ctx,
				j-btac2_lbase[i],
				btac2_lextra[i]);
			break;
		}
	}
}

void BTAC2_Encode_StatLenUValue(BGBMID_BTAC2_Context *ctx,
	int *stat, int pfb, u32 val)
{
	int i, j, k;

	j=val;
	for(i=0; i<64; i++)
	{
		k=btac2_lbase[i]+(1<<btac2_lextra[i]);
		if((j>=btac2_lbase[i]) && (j<k))
		{
			stat[pfb+i]++;
			break;
		}
	}
}


int BTAC2_Encode_CountEncodeVal(BGBMID_BTAC2_Context *ctx,
	int tab, int z, int v)
{
	int i, j, k;

	if(z<0) { *(int *)-1=-1; }

	v=((v>=0)?(v<<1):(((-v)<<1)-1));

	if((z>6) || (v>=65536))
	{
		k=BTAC2_Encode_CountEncodeLenUValue(ctx, tab, 7<<5, z);
		k+=BTAC2_Encode_CountEncodeCoeffUValue(ctx, tab, 0<<6, v);
		return(k);
	}

	k=BTAC2_Encode_CountEncodeCoeffUValue(ctx, tab, z<<5, v);
	return(k);
}

void BTAC2_Encode_EncodeVal(BGBMID_BTAC2_Context *ctx,
	int tab, int z, int v)
{
	int i, j, k;

	if(z<0) { *(int *)-1=-1; }

#if 1
	if((z!=0) && (v!=0))
		{ btac2_encode_dbgPrintLog("Z%dV%d ", z, v); }
	else if(v!=0)
		{ btac2_encode_dbgPrintLog("V%d ", v); }
	else if(z!=0)
		{ btac2_encode_dbgPrintLog("Z%d ", z); }
	else
		{ btac2_encode_dbgPrintLog("ZZ "); }
#endif

	v=((v>=0)?(v<<1):(((-v)<<1)-1));

	if((z>6) || (v>=65536))
	{
//		printf("BTAC2_Encode_EncodeVal: Trap %d %d\n", z, v);
	
		BTAC2_Encode_EncodeLenUValue(ctx, tab, 7<<5, z);
		BTAC2_Encode_EncodeCoeffUValue(ctx, tab, 0<<6, v);
		return;
	}

	BTAC2_Encode_EncodeCoeffUValue(ctx, tab, z<<5, v);
	return;
}

void BTAC2_Encode_StatVal(BGBMID_BTAC2_Context *ctx,
	int *stat, int z, int v)
{
	int i, j, k;

	if(z<0) { *(int *)-1=-1; }

	v=((v>=0)?(v<<1):(((-v)<<1)-1));

	if((z>6) || (v>=65536))
	{
		BTAC2_Encode_StatLenUValue(ctx, stat, 7<<5, z);
		BTAC2_Encode_StatCoeffUValue(ctx, stat, 0<<6, v);
		return;
	}

	BTAC2_Encode_StatCoeffUValue(ctx, stat, z<<5, v);
	return;
}

void BTAC2_Encode_EncodeDiffDC(BGBMID_BTAC2_Context *ctx,
	int tab, int v)
{
	v=((v>=0)?(v<<1):(((-v)<<1)-1));
	BTAC2_Encode_EncodeCoeffUValue(ctx, tab, 0, v);
}

int BTAC2_Encode_CountEncodeDiffDC(BGBMID_BTAC2_Context *ctx,
	int tab, int v)
{
	int l;

	v=((v>=0)?(v<<1):(((-v)<<1)-1));
	l=BTAC2_Encode_CountEncodeCoeffUValue(ctx, tab, 0, v);
	return(l);
}

void BTAC2_Encode_StatDiffDC(BGBMID_BTAC2_Context *ctx,
	int *stat, int v)
{
	int i, j, k;

	v=((v>=0)?(v<<1):(((-v)<<1)-1));
	BTAC2_Encode_StatCoeffUValue(ctx, stat, 0, v);
	return;
}


int BTAC2_Encode_CountEncodeBlock(BGBMID_BTAC2_Context *ctx,
	int *buf, int blen, int dctab, int actab)
{
	int i, j, k, l;

//	l=BTAC2_Encode_CountEncodeVal(ctx, dctab, 0, buf[0]);
	l=BTAC2_Encode_CountEncodeDiffDC(ctx, dctab, buf[0]);

	i=1;
	while(i<blen)
	{
		j=buf[i];
		if(j)
		{
			l+=BTAC2_Encode_CountEncodeVal(ctx, actab, 0, j);
			i++;
			continue;
		}

#if 1
		j=i+1;
		while(1)
		{
			if(j>=blen)
			{
				l+=BTAC2_Encode_CountEncodeVal(ctx, actab, 0, 0);
				return(l);
			}
			if(buf[j])break;
			j++;
		}
#endif

		k=j-i;
		l+=BTAC2_Encode_CountEncodeVal(ctx, actab, k, buf[j]);
		i=j+1;
	}
	return(l);
}

void BTAC2_Encode_EncodeBlock(BGBMID_BTAC2_Context *ctx,
	int *buf, int blen, int dctab, int actab)
{
	int i, j, k, l, pvf;

	btac2_encode_dbgPrintLog("BLK ");

//	BTAC2_Encode_EncodeVal(ctx, dctab, 0, buf[0]);
	BTAC2_Encode_EncodeDiffDC(ctx, dctab, buf[0]);

	i=1; l=0; pvf=0;
	while(i<blen)
	{
		j=buf[i];
		if(j)
		{
//			btac2_encode_dbgPrintLog("E%d", j-l);
//			btac2_encode_dbgPrintLog("E%d%c ", j-l,
//				(((j-l)*(j-l))<(j*j))?'P':'F');
//			pvf+=(((j-l)*(j-l))<(j*j))?1:-1;
//			pvf+=(j*j)-((j-l)*(j-l));

			BTAC2_Encode_EncodeVal(ctx, actab, 0, j);
			i++; l=j;
			continue;
		}

		j=i+1;
		while(1)
		{
			if(j>=blen)
			{
				BTAC2_Encode_EncodeVal(ctx, actab, 0, 0);

//				btac2_encode_dbgPrintLog("PVF=%d", pvf);
//				btac2_encode_dbgPrintLog("PVF%c=%d",
//					(pvf>0)?'p':'f', pvf);
				btac2_encode_dbgPrintLog("\n\n");
				return;
			}
			if(buf[j])break;
			j++;
		}

#if 0
		k=buf[j];
//		btac2_encode_dbgPrintLog("E%d%c ", k-l,
//			(((k-l)*(k-l))<(k*k))?'P':'F');
		btac2_encode_dbgPrintLog("E%d ", k-l);
		pvf+=(((k-l)*(k-l))<(k*k))?1:-1;
		l=k;
#endif

		k=j-i;
		BTAC2_Encode_EncodeVal(ctx, actab, k, buf[j]);
		i=j+1;
	}

//	btac2_encode_dbgPrintLog("PVF%c=%d", (pvf>0)?'p':'f', pvf);
	btac2_encode_dbgPrintLog("\n\n");
}

void BTAC2_Encode_StatBlock(BGBMID_BTAC2_Context *ctx,
	int *buf, int blen, int *dcstat, int *acstat)
{
	int i, j, k;

//	BTAC2_Encode_StatVal(ctx, dcstat, 0, buf[0]);
	BTAC2_Encode_StatDiffDC(ctx, dcstat, buf[0]);

	i=1;
	while(i<blen)
	{
		j=buf[i];
		if(j)
		{
			BTAC2_Encode_StatVal(ctx, acstat, 0, j);
			i++;
			continue;
		}

		j=i+1;
		while(1)
		{
			if(j>=blen)
			{
				BTAC2_Encode_StatVal(ctx, acstat, 0, 0);
				return;
			}
			if(buf[j])break;
			j++;
		}

		k=j-i;
		BTAC2_Encode_StatVal(ctx, acstat, k, buf[j]);
		i=j+1;
	}
}

void BTAC2_SplitCenterSideS16(
	s16 *iblk,
	int *tcblk, int *tsblk,
	int *rcm, int *rcn,
	int *rsm, int *rsn,
	int blen, int flags)
{
	int sm, sn, cm, cn, tl, tr, tc;
	int lm2, ln2, rm2, rn2;
	int cm2, cn2, ca2, ba;
	int p0, p1, p2, p3;
	int s0, s1;
	int i, j, k, l;

	cm= 99999; cn=-99999;
	sm= 99999; sn=-99999;
	
	ba=0;
	for(i=0; i<blen; i++)
	{
		if(flags&BGBMID_BTAC_EFL_HALFMONO)
		{
			tl=iblk[i*2+0];
			tr=tl;
		}else if(flags&BGBMID_BTAC_EFL_STEREO)
		{
			tl=iblk[i*2+0];
			tr=iblk[i*2+1];
		}else
		{
			tl=iblk[i];
			tr=tl;
		}

//		k=tl+tr;
//		l=(k>>1)-tr;

		k=(tl+tr)>>1;
		l=tl-k;
//		l=tl-tr;
//		l=((tl-k)+(k-tr))>>1;

//		l=(l<(-32768))?(-32768):((l<32768)?l:32767);
		if(tcblk) { tcblk[i]=k; }
		if(tsblk) { tsblk[i]=l; }
		ba+=l;
		if(k<cm) { cm=k; }
		if(k>cn) { cn=k; }
		if(l<sm) { sm=l; }
		if(l>sn) { sn=l; }
	}

	if(rcm && rcn) { *rcm=cm; *rcn=cn; }
	if(rsm && rsn) { *rsm=sm; *rsn=sn; }
}

void BTAC2_FilterEncodeBlock(s16 *iblk, byte *oblk,
	int tag, int blen, int flags)
{
}

void BTAC2_Common_PreQuantWHT(BGBMID_BTAC2_Context *ctx,
	int *iblk, int *oblk,
	int blen, int qtab, int qf)
{
	int i, j, k;
	
	for(i=0; i<blen; i++)
		oblk[i]=iblk[i];
	
#if 0
	if(qf>=32)
	{
		for(i=(blen/2); i<blen; i++)
			oblk[i]=0;
	}

	if(qf>=64)
	{
		for(i=(blen/4); i<blen; i++)
			oblk[i]=0;
	}
#endif
}

void BTAC2_FilterEncodeBlock0(BGBMID_BTAC2_Context *ctx,
	s16 *iblk, byte *oblk, int blen, int flags)
{
	int tcba[256], tsba[256];
	int tcbb[256], tsbb[256];
	int tcbc[256], tsbc[256];
	int tcma[256], tsma[256];
	int tcmb[256], tsmb[256];
	int cm, cn, sm, sn, tl, tr, tc;
	int cm2, cn2, ca2, ba, qf, qf0;
	int p0, p1, p2, p3;
	int s0, s1;
	int i, j, k, l;

	BTAC2_SplitCenterSideS16(
		iblk, tcba, tsba,
		&cm, &cn, &sm, &sn, blen, flags);

	BTAC2_Common_TransformWHT(tcba, tcma, blen, 0);
	BTAC2_Common_TransformWHT(tsba, tsma, blen, 0);
//	BTAC2_Common_TransformDCT(tcba, tcma, blen, 0);
//	BTAC2_Common_TransformDCT(tsba, tsma, blen, 0);

	qf0=-1;
	for(i=0; i<32; i++)
	{
//		qf=i*i;
		qf=(i*i)/4;
//		qf=(i*i)/16;
//		qf=(qf+(4*i))/2;
		if(qf==qf0)continue;
		qf0=qf;
		
		BTAC2_Common_PreQuantWHT(ctx, tcma, tcmb, blen, 0, qf);
		BTAC2_Common_QuantWHT(ctx, tcmb, tcmb, blen, 0, qf);
		l=BTAC2_Encode_CountEncodeBlock(ctx, tcmb, blen, 0, 1);

		if(flags&BGBMID_BTAC_EFL_STEREO)
		{
			BTAC2_Common_PreQuantWHT(ctx, tsma, tsmb, blen, 1, qf);
			BTAC2_Common_QuantWHT(ctx, tsmb, tsmb, blen, 1, qf);
			l+=BTAC2_Encode_CountEncodeBlock(ctx, tsmb, blen, 2, 3);
		}

		if((l+24)<=ctx->btac_blkbits)
			break;
	}

#if 1
//	for(k=0; k<4; k++)
	if(1)
	{
		BTAC2_Common_DequantWHT(ctx, tcmb, tcma, blen, 0, qf);
		BTAC2_Common_DequantWHT(ctx, tsmb, tsma, blen, 1, qf);
		BTAC2_Common_TransformIWHT(tcma, tcbb, blen, 0);
		BTAC2_Common_TransformIWHT(tsma, tsbb, blen, 0);
		for(j=0; j<blen; j++)
		{
			tcbc[j]=tcba[j]+(tcba[j]-tcbb[j]);
			tsbc[j]=tsba[j]+(tsba[j]-tsbb[j]);
		}

		BTAC2_Common_TransformWHT(tcbc, tcma, blen, 0);
		BTAC2_Common_TransformWHT(tsbc, tsma, blen, 0);

		while(qf<255)
		{
			BTAC2_Common_PreQuantWHT(ctx, tcma, tcmb, blen, 0, qf);
			BTAC2_Common_QuantWHT(ctx, tcmb, tcmb, blen, 0, qf);

			BTAC2_Common_PreQuantWHT(ctx, tsma, tsmb, blen, 1, qf);
			BTAC2_Common_QuantWHT(ctx, tsmb, tsmb, blen, 1, qf);

			l=BTAC2_Encode_CountEncodeBlock(ctx, tcmb, blen, 0, 1);
			l+=BTAC2_Encode_CountEncodeBlock(ctx, tsmb, blen, 2, 3);
			if((l+24)<=ctx->btac_blkbits)
				break;
			
			qf++;
		}
	}
#endif

//	if(i>=64)
	if(i>=32)
	{
//		printf("BTAC2_FilterEncodeBlock0: Fail Size Block\n");
		printf("BTAC2_FilterEncodeBlock0: Fail Size Block Qf=%d L=%d\n",
			qf, l);
	}else
	{
//		printf("BTAC2_FilterEncodeBlock0: Encode Qf=%d L=%d\n", qf, l);
//		btac2_encode_dbgPrintLog("BLK ");
		btac2_encode_dbgPrintLog("BTAC2_FilterEncodeBlock0: "
			"Encode Qf=%d L=%d\n", qf, l);
	}

	ctx->ct=oblk;
	ctx->cte=oblk+ctx->btac_blkbytes;
	ctx->huff_win=0;
	ctx->huff_pos=0;

	BTAC2_Encode_WriteNBits(ctx, 0, 4);		//tag
	BTAC2_Encode_WriteNBits(ctx, 0, 4);		//pad
	BTAC2_Encode_WriteNBits(ctx, qf, 8);	//quant

	BTAC2_Encode_EncodeBlock(ctx, tcmb, blen, 0, 1);
	if(flags&BGBMID_BTAC_EFL_STEREO)
		{ BTAC2_Encode_EncodeBlock(ctx, tsmb, blen, 2, 3); }

	BTAC2_Encode_FlushBits(ctx);

	if(ctx->ct>=ctx->cte)
	{
		printf("BTAC2_FilterEncodeBlock0: Overflowed Block\n");
	}
	
//	BTAC2_Encode_StatBlock(NULL, tcmb, btac_band0_tcmbcnt, btac_band0_tcmbcnt);
//	BTAC2_Encode_StatBlock(NULL, tsmb, btac_band0_tsmbcnt, btac_band0_tsmbcnt);
}

void BTAC2_FilterStatBlock0(BGBMID_BTAC2_Context *ctx,
	s16 *iblk, int blen, int flags)
{
	int tcba[256], tsba[256];
	int tcma[256], tsma[256];
	int tcmb[256], tsmb[256];
	int cm, cn, sm, sn, tl, tr, tc;
	int cm2, cn2, ca2, ba;
	int p0, p1, p2, p3;
	int s0, s1;
	int i, j, k, l;

	BTAC2_SplitCenterSideS16(
		iblk, tcba, tsba,
		&cm, &cn, &sm, &sn, blen, flags);

	BTAC2_Common_TransformWHT(tcba, tcma, blen, 0);
	BTAC2_Common_TransformWHT(tsba, tsma, blen, 0);
//	BTAC2_Common_TransformDCT(tcba, tcma, blen, 0);
//	BTAC2_Common_TransformDCT(tsba, tsma, blen, 0);

	l=-1;
	for(i=0; i<32; i++)
	{
//		j=i*i;
		j=(i*i)/4;
//		j=(i*i)/16;
		if(j==l)continue;
		l=j;
		
		BTAC2_Common_PreQuantWHT(ctx, tcma, tcmb, blen, 0, j);
		BTAC2_Common_PreQuantWHT(ctx, tsma, tsmb, blen, 1, j);
		BTAC2_Common_QuantWHT(ctx, tcmb, tcmb, blen, 0, j);
		BTAC2_Common_QuantWHT(ctx, tsmb, tsmb, blen, 1, j);

#if 1
		l=BTAC2_Encode_CountEncodeBlock(ctx, tcmb, blen, 0, 1);
		l+=BTAC2_Encode_CountEncodeBlock(ctx, tsmb, blen, 2, 3);

		if(l>(8*ctx->btac_blkbits))
			{ i+=3; continue; }
		if(l>(4*ctx->btac_blkbits))
			{ i+=1; continue; }

//		if(l>ctx->btac_blkbits)
//			continue;

		if(l<=ctx->btac_blkbits)
		{
			BTAC2_Encode_StatBlock(ctx, tcmb, blen,
				ctx->huff_stat[0], ctx->huff_stat[1]);
			BTAC2_Encode_StatBlock(ctx, tsmb, blen,
				ctx->huff_stat[2], ctx->huff_stat[3]);
			continue;
		}
#endif

#if 0
		BTAC2_Encode_StatBlock(ctx, tcmb, blen,
			ctx->huff_stat[0], ctx->huff_stat[1]);
		BTAC2_Encode_StatBlock(ctx, tsmb, blen,
			ctx->huff_stat[2], ctx->huff_stat[3]);
#endif
	}
}

void BTAC2_FilterQStatBlock0(BGBMID_BTAC2_Context *ctx,
	double *qsec, double *qses,
	double *qdec, double *qdes,
	s16 *iblk, int blen, int flags)
{
	int tcba[256], tsba[256];
	int tcma[256], tsma[256];
	int tcmb[256], tsmb[256];
	int cm, cn, sm, sn, tl, tr, tc;
	float f, g, f0, g0, f1, g1;
	int s0, s1;
	int i, j, k, l;

	BTAC2_SplitCenterSideS16(
		iblk, tcba, tsba,
		&cm, &cn, &sm, &sn, blen, flags);

	BTAC2_Common_TransformWHT(tcba, tcma, blen, 0);
	BTAC2_Common_TransformWHT(tsba, tsma, blen, 0);
//	BTAC2_Common_TransformDCT(tcba, tcma, blen, 0);
//	BTAC2_Common_TransformDCT(tsba, tsma, blen, 0);

	f0=0; g0=0;
	for(i=0; i<blen; i++)
	{
		f1=tcma[i];
		qsec[i]+=f1*f1;
//		f=f1-f0;
//		qdec[i]+=f*f;
		qdec[i]+=f1*f0;

		g1=tsma[i];
		qses[i]+=g1*g1;
//		g=g1-g0;
//		qdes[i]+=g*g;
		qdes[i]+=g1*g0;
		
		f0=f1; g0=g1;
	}
}


void BTAC2_FilterEncodeStreamS16(
	BGBMID_BTAC2_Context *ctx,
	s16 *iblk, byte *oblk,
	int len, int flags)
{
	byte tcmbcl[256];
	byte tsmbcl[256];
	s16 tblk[256*2];
	float f, g, h;
	int totb, blen, bsz;
	int i, j, k, l, n, tag, ltag, rtag, fl2;
		
	blen=ctx->btac_blklen;
	bsz=ctx->btac_blkbytes;
	n=(len+blen-1)/blen;
	for(i=0; i<n; i++)
	{
		if(!(i&255))
			{ printf("Stats Pass %d/%d  \r", i, n); }

//		printf("BTAC2_FilterEncodeStreamS16: Blk=%d\n", i);

		fl2=flags;
		if((i>0) && i<(n-1))
			fl2|=BGBMID_BTAC_EFL_MIDSTREAM;

		if(flags&BGBMID_BTAC_EFL_STEREO)
		{
//			tag=BGBMID_BTAC_Check64BlockBestS16(iblk+i*2*64, flags);
			BTAC2_FilterEncodeBlock0(ctx,
				iblk+i*2*blen, oblk+i*bsz, blen, fl2);
		}else
		{
//			tag=BGBMID_BTAC_Check64BlockBestS16(iblk+i*64, flags);
			BTAC2_FilterEncodeBlock0(ctx,
				iblk+i*blen, oblk+i*bsz, blen, fl2);
		}
	}
	printf("\n");
}

void BTAC2_FilterStatStreamS16(
	BGBMID_BTAC2_Context *ctx, s16 *iblk, int len, int flags)
{
	static double bandtune[8]={0.5, 0.7, 1.0, 1.5, 2.0, 1.75, 1.5, 1.0};

	double tcmbqe[256];
	double tsmbqe[256];
	double tcmbdqe[256];
	double tsmbdqe[256];

	byte tcmbcl[256];
	byte tsmbcl[256];
	s16 tblk[256*2];
	float f, g, h, h0, h1;
	int totb, blen;
	int i, j, k, l, n, tag, ltag, rtag, fl2;
	
	for(i=0; i<4; i++)
	{
		if(!ctx->huff_stat[i])
			ctx->huff_stat[i]=gcalloc(256*sizeof(int));
			
		for(j=0; j<256; j++)
			ctx->huff_stat[i][j]=0;

		if(!ctx->huff_len[i])
			ctx->huff_len[i]=gcalloc(256*sizeof(byte));
			
		for(j=0; j<256; j++)
			ctx->huff_len[i][j]=8;
	}
	
	blen=ctx->btac_blklen;
	n=(len+blen-1)/blen;

	for(i=0; i<blen; i++)
	{
		tcmbqe[i]=0; tsmbqe[i]=0;
		tcmbdqe[i]=0; tsmbdqe[i]=0;
	}

	for(i=0; i<n; i++)
	{
		fl2=flags;
		if((i>0) && i<(n-1))
			fl2|=BGBMID_BTAC_EFL_MIDSTREAM;

		if(flags&BGBMID_BTAC_EFL_STEREO)
		{
//			tag=BGBMID_BTAC_Check64BlockBestS16(iblk+i*2*64, flags);
			BTAC2_FilterQStatBlock0(ctx,
				tcmbqe, tsmbqe,
				tcmbdqe, tsmbdqe,
				iblk+i*2*blen, blen, fl2);
		}else
		{
//			tag=BGBMID_BTAC_Check64BlockBestS16(iblk+i*64, flags);
			BTAC2_FilterQStatBlock0(ctx,
				tcmbqe, tsmbqe,
				tcmbdqe, tsmbdqe,
				iblk+i*blen, blen, fl2);
		}
	}

	f=0; g=0;
	for(i=0; i<256; i++)
		{ f+=tcmbqe[i]; g+=tsmbqe[i]; }

	for(i=0; i<256; i++)
	{
//		tcmbqe[i]*=1.0/((f/256)+1.0);
//		tsmbqe[i]*=1.0/((g/256)+1.0);

		tcmbqe[i]*=1.0/(sqrt(f/256)+1.0);
		tsmbqe[i]*=1.0/(sqrt(g/256)+1.0);
	}

	for(i=0; i<256; i++)
	{
		f=tcmbqe[i]/(double)n;
		f=sqrt(f);
		f=sqrt(f);
//		f=sqrt(f);

		g=tsmbqe[i]/(double)n;
		g=sqrt(g);
		g=sqrt(g);
//		g=sqrt(g);

//		f=1.0;
//		g=1.0;

//		h=0.5+(i/256.0);
//		h=h*h;

//		h=2.0*(i/256.0);
//		h=log(i+1.0)*0.18*(1.5*M_PI);
//		h=(i/256.0)*(1.5*M_PI);
//		h=bgbmid_log2up(i+1.0)*0.125*(1.5*M_PI);
//		h=2.0*(1.0-cos(h));
//		h=1.0*(1.0-cos(h));
//		h=bandtune[bgbmid_log2dn(i)];
//		h=h*h;
//		h=2.0*((1<<bgbmid_log2dn(i))/256.0);

		h0=1.5*((1<<bgbmid_log2dn(i))/256.0);
		h1=1.5*(i/256.0);
		h=0.5*(h0+h1);
		h=1.5*(h*h);
		
		h=0.25+h;

		f=f*h;

//		h=4.0*(i/256.0);
//		h=2.0*((1<<bgbmid_log2dn(i))/256.0);

		h0=1.5*((1<<bgbmid_log2dn(i))/256.0);
		h1=1.5*(i/256.0);
		h=0.5*(h0+h1);
		h=1.5*(h*h);

		h=0.25+h;
		g=g*h;

		ctx->qtab_max[0][i]=1+32768*f;
//		ctx->qtab_max[0][i]=1+65536/f;
//		ctx->qtab_max[0][i]=1+65536*f;
		ctx->qtab_max[0][i]=1+49152*f;

//		ctx->qtab_max[1][i]=1+32768*g;
//		ctx->qtab_max[1][i]=1+65536*g;
//		ctx->qtab_max[1][i]=1+131072*f;
		ctx->qtab_max[1][i]=1+49152*g;
	}

#if 0
	if(1)
	{
		for(i=0; i<16; i++)
		{
			for(j=0; j<16; j++)
			{
				f=tcmbqe[i*16+j]/(double)n;
				f=sqrt(f);
				f=sqrt(f);
				printf("%.1f ", f);
				h+=f;
			}
			printf("\n");
		}
	}
#endif

#if 0
	if(1)
	{
		printf("Stat E/dE:\n");
		h=0;
		for(i=0; i<32; i++)
		{
			for(j=0; j<8; j++)
			{
				f=tcmbqe[i*8+j];
				g=tcmbdqe[i*8+j];
//				printf("%.1f ", f/g);
//				printf("%4.1f ", g/(double)n);
//				printf("%4.1f ", bgbmid_ssqrt(g/(double)n));
//				printf("%1.4f ", g/(f*n));
				printf("%1.4f ", f/g);
				h+=g;
			}
			printf("\n");
		}
		printf("Corr=%f\n", bgbmid_ssqrt(h/(double)(256*n)));
		printf("\n");
	}
#endif

	printf("Stats Pass 1\n");

	for(i=0; i<n; i++)
	{
		if(!(i&255))
			{ printf("Stats Pass %d/%d  \r", i, n); }

		fl2=flags;
		if((i>0) && i<(n-1))
			fl2|=BGBMID_BTAC_EFL_MIDSTREAM;

		if(flags&BGBMID_BTAC_EFL_STEREO)
		{
//			tag=BGBMID_BTAC_Check64BlockBestS16(iblk+i*2*64, flags);
			BTAC2_FilterStatBlock0(ctx, iblk+i*2*blen, blen, fl2);
		}else
		{
//			tag=BGBMID_BTAC_Check64BlockBestS16(iblk+i*64, flags);
			BTAC2_FilterStatBlock0(ctx, iblk+i*blen, blen, fl2);
		}
	}
	printf("\n");

#if 1
	for(i=0; i<4; i++)
	{
		j=BTAC2_Encode_BuildLengthsAdjust(ctx->huff_stat[i], 256,
			tcmbcl, 16);
		for(j=0; j<256; j++)
		{
			ctx->huff_len[i][j]=tcmbcl[j];
			if(!tcmbcl[j])
				ctx->huff_len[i][j]=8;

			ctx->huff_stat[i][j]=0;
		}
	}

	printf("Stats Pass 2\n");

	for(i=0; i<n; i++)
	{
		if(!(i&255))
			{ printf("Stats Pass %d/%d  \r", i, n); }
	
		fl2=flags;
		if((i>0) && i<(n-1))
			fl2|=BGBMID_BTAC_EFL_MIDSTREAM;

		if(flags&BGBMID_BTAC_EFL_STEREO)
		{
//			tag=BGBMID_BTAC_Check64BlockBestS16(iblk+i*2*64, flags);
			BTAC2_FilterStatBlock0(ctx, iblk+i*2*blen, blen, fl2);
		}else
		{
//			tag=BGBMID_BTAC_Check64BlockBestS16(iblk+i*64, flags);
			BTAC2_FilterStatBlock0(ctx, iblk+i*blen, blen, fl2);
		}
	}
	printf("\n");
#endif

	for(k=0; k<4; k++)
	{
		printf("Stats %d\n", k);

		h=0;
		for(i=0; i<16; i++)
		{
			for(j=0; j<16; j++)
			{
				f=ctx->huff_stat[k][i*16+j]/(double)n;
				printf("%.1f ", f);
				h+=f;
			}
			printf("\n");
		}
		printf("%f\n", h*8);
		printf("\n");
	}

	for(k=0; k<4; k++)
	{
		if(!ctx->huff_code[k])
		{
			ctx->huff_code[k]=gcalloc(256*sizeof(u16));
			ctx->huff_mask[k]=gcalloc(256*sizeof(u16));
			ctx->huff_idx[k]=gcalloc(256*sizeof(u16));
			ctx->huff_next[k]=gcalloc(256*sizeof(u16));
		}

		if(!ctx->huff_len[k])
			ctx->huff_len[k]=gcalloc(256);

		for(j=0; j<256; j++)
			ctx->huff_len[k][j]=0;

//		j=BTAC2_Encode_BuildLengths(ctx->huff_stat[k], 256,
//			tcmbcl, 15);
		j=BTAC2_Encode_BuildLengthsAdjust(ctx->huff_stat[k], 256,
			tcmbcl, 16);
		if(j<0)
		{
			printf("Build Lengths Error %d, %d\n", k, j);
			
			//dummy table
			for(j=0; j<256; j++)
				tcmbcl[j]=8;
		}

		j=BTAC2_Common_SetupTable(tcmbcl, 256,
			ctx->huff_code[k], ctx->huff_mask[k], ctx->huff_len[k],
			ctx->huff_idx[k], ctx->huff_next[k]);
		if(j<0)printf("Setup Error %d, %d\n", k, j);
	}

	for(k=0; k<4; k++)
	{
		printf("SymLen %d\n", k);

		h=0;
		for(i=0; i<16; i++)
		{
			for(j=0; j<16; j++)
			{
				l=ctx->huff_len[k][i*16+j];
				printf("%2d ", l);
				h+=f;
			}
			printf("\n");
		}
//		printf("%f\n", h*8);
		printf("\n");
	}
	
#if 0
	BTAC2_Encode_BuildLengths(btac_band0_tcmbcnt, 256, tcmbcl, 15);
	BTAC2_Encode_BuildLengths(btac_band0_tsmbcnt, 256, tsmbcl, 15);

	totb=0;
	for(i=0; i<16; i++)
	{
		for(j=0; j<16; j++)
		{
			k=tcmbcl[i*16+j];
			totb+=(k*btac_band0_tcmbcnt[i*16+j])/n;
			printf("%2d ", k);
		}
		printf("\n");
	}
	printf("Avg Tot=%d\n", totb);
	printf("\n");

	totb=0;
	for(i=0; i<16; i++)
	{
		for(j=0; j<16; j++)
		{
			k=tsmbcl[i*16+j];
			totb+=(k*btac_band0_tsmbcnt[i*16+j])/n;
			printf("%2d ", k);
		}
		printf("\n");
	}
	printf("Avg Tot=%d\n", totb);
	printf("\n");

//	printf("Encode %d: ", n);
//	for(i=0; i<16; i++)
//		printf("%d ", tcnt[i]);
//	printf("\n");
#endif

}

void BTAC2_Encode_SetupTables(
	BGBMID_BTAC2_Context *ctx, int ch, int rt, int len, int flags)
{
	int s0, s1;
	int i, bl, bs;

//	bs=128;
//	bs=64;
	bs=96;
//	bs=192;

	ctx->btac_blklen=256;
	ctx->btac_blkbytes=bs;
	ctx->btac_blkbits=bs*8;
	
	ctx->chan=ch;
	ctx->bits=16;
	ctx->rate=rt;
	ctx->len=len;
	ctx->flags=flags;

	ctx->qtab_min[0]=gcalloc(ctx->btac_blklen*sizeof(int));
	ctx->qtab_max[0]=gcalloc(ctx->btac_blklen*sizeof(int));
	ctx->qtab_min[1]=gcalloc(ctx->btac_blklen*sizeof(int));
	ctx->qtab_max[1]=gcalloc(ctx->btac_blklen*sizeof(int));
	
	bl=ctx->btac_blklen;
	
	s0=32768;
//	s1=65536;
	s1=32768;
	
	for(i=0; i<bl; i++)
	{
		ctx->qtab_min[0][i]=1;
		ctx->qtab_min[1][i]=1;

		ctx->qtab_max[0][i]=s0;
		ctx->qtab_max[1][i]=s1;
	}

#if 0
	for(i=0; i<(bl>>1); i++)
	{
		ctx->qtab_max[0][i]=s0*0.8;
		ctx->qtab_max[1][i]=s1*0.8;
	}

	for(i=0; i<(bl>>2); i++)
	{
		ctx->qtab_max[0][i]=s0*0.7;
		ctx->qtab_max[1][i]=s1*0.7;
	}

	for(i=0; i<(bl>>3); i++)
	{
		ctx->qtab_max[0][i]=s0*0.6;
		ctx->qtab_max[1][i]=s1*0.6;
	}

	for(i=0; i<(bl>>4); i++)
	{
		ctx->qtab_max[0][i]=s0*0.5;
		ctx->qtab_max[1][i]=s1*0.5;
	}

	for(i=0; i<(bl>>5); i++)
	{
		ctx->qtab_max[0][i]=s0*0.4;
		ctx->qtab_max[1][i]=s1*0.4;
	}
#endif

}

BMID_API void BTAC2_TestFilterStreamS16(
	s16 *iblk, byte *oblk, int len, int flags)
{
	BGBMID_BTAC2_Context *ctx;
	int i, bl, bs;
	
	bs=128;
	bs=256;
	
	ctx=BTAC2_Common_AllocContext();
	ctx->btac_blklen=256;
	ctx->btac_blkbytes=bs;
	ctx->btac_blkbits=bs*8;
	
//	ctx->chan=(flags&BGBMID_BTAC_EFL_STEREO)?2:1;
//	ctx->bits=16;
//	ctx->rate=44100;
//	ctx->len=len;
	
	BTAC2_Encode_SetupTables(ctx,
		(flags&BGBMID_BTAC_EFL_STEREO)?2:1, 44100, len, flags);

	BTAC2_FilterStatStreamS16(ctx, iblk, len, flags);
	BTAC2_FilterEncodeStreamS16(ctx, iblk, oblk, len, flags);
	
	BTAC2_Encode_EncodeHeaderTables(ctx);
}

BMID_API int BTAC2_Encode(short *ibuf,
	int ch, int rt, int len, int flags,
	byte *obuf, int obsz)
{
	BGBMID_BTAC2_Context *ctx;
	byte *pt;
	int i, bl, bn, bsz, sz;
	
	if(!btac2_encode_dbglogfd)
		btac2_encode_dbglogfd=fopen("btac2_enclog.txt", "wt");
	
	ctx=BTAC2_Common_AllocContext();
	ctx->btac_blklen=256;
	ctx->btac_blkbytes=64;
	ctx->btac_blkbits=64*8;
	
//	ctx->chan=(flags&BGBMID_BTAC_EFL_STEREO)?2:1;
//	ctx->bits=16;
//	ctx->rate=44100;
//	ctx->len=len;
	
	BTAC2_Encode_SetupTables(ctx,
		ch, rt, len, flags);

	bl=ctx->btac_blklen;
	bn=(len+(bl-1))/bl;
	bsz=ctx->btac_blkbytes;
	
	ctx->data=gcatomic(bn*bsz);
	ctx->sz_data=bn*bsz;

	BTAC2_FilterStatStreamS16(ctx, ibuf, len, flags);
	BTAC2_FilterEncodeStreamS16(ctx, ibuf, ctx->data, len, flags);
	
	BTAC2_Encode_EncodeHeaderTables(ctx);
		
	ctx->ct=obuf;
	ctx->cte=obuf+obsz;

	BTAC2_Encode_EmitEightCC(ctx, 'B', 'T', 'A', 'C', '2', 'A', 0x0D, 0x0A);
	BTAC2_Encode_EmitDWord(ctx, 0);
	BTAC2_Encode_EmitDWord(ctx, 0);

	pt=BTAC2_Encode_EmitBeginMarkFOURCC(ctx, 'H', 'E', 'A', 'D');
	BTAC2_Encode_EmitBytes(ctx, ctx->headtab, ctx->sz_headtab);
	BTAC2_Encode_EmitEndMarkFOURCC(ctx, pt);

	pt=BTAC2_Encode_EmitBeginMarkFOURCC(ctx, 'D', 'A', 'T', 'A');
	BTAC2_Encode_EmitBytes(ctx, ctx->data, ctx->sz_data);
	BTAC2_Encode_EmitEndMarkFOURCC(ctx, pt);
	
	sz=ctx->ct-obuf;
	obuf[8]=sz&255;			obuf[9]=(sz>>8)&255;
	obuf[10]=(sz>>16)&255;	obuf[11]=(sz>>24)&255;
	
	return(sz);
}

#if 0
BMID_API void BGBMID_BTAC_Band0Stat64StreamS16(
	s16 *iblk, byte *oblk, int len, int flags)
{
	byte tcmbcl[256];
	byte tsmbcl[256];
	s16 tblk[64*2];
	int tcnt[16];
	float f, g, h;
	int totb;
	int i, j, k, n, tag, ltag, rtag, fl2;
	
	for(i=0; i<16; i++)
		{ tcnt[i]=0; }

	for(i=0; i<64; i++)
	{
		btac_band0_tcmbacc[i]=0;
		btac_band0_tsmbacc[i]=0;
	}

	for(i=0; i<256; i++)
	{
		btac_band0_tcmbcnt[i]=0;
		btac_band0_tsmbcnt[i]=0;
	}
	
	n=(len+63)/64;
	for(i=0; i<n; i++)
	{
		fl2=flags;
		if((i>0) && i<(n-1))
			fl2|=BGBMID_BTAC_EFL_MIDSTREAM;

		if((flags&BGBMID_BTAC_EFL_STEREO) && (flags&BGBMID_BTAC_EFL_HALFMONO))
		{
			ltag=0; rtag=0;
			BGBMID_BTAC_Encode64BlockBand0(
				iblk+i*2*64+0, oblk+i*64+ 0, ltag, fl2);
			BGBMID_BTAC_Encode64BlockBand0(
				iblk+i*2*64+1, oblk+i*64+32, rtag, fl2);

			tcnt[ltag]++;
			tcnt[rtag]++;
		}else if(flags&BGBMID_BTAC_EFL_STEREO)
		{
			tag=0;
//			tag=BGBMID_BTAC_Check64BlockBestS16(iblk+i*2*64, flags);
			BGBMID_BTAC_Encode64BlockBand0(iblk+i*2*64, oblk+i*32, tag, fl2);
			tcnt[tag]++;
		}else
		{
			tag=0;
//			tag=BGBMID_BTAC_Check64BlockBestS16(iblk+i*64, flags);
			BGBMID_BTAC_Encode64BlockBand0(iblk+i*64, oblk+i*32, tag, fl2);
			tcnt[tag]++;
		}
	}

//	printf("\n");

	for(i=0; i<8; i++)
	{
		for(j=0; j<8; j++)
		{
			printf("%.2f ", btac_band0_tcmbacc[i*8+j]/((double)n));
		}
		printf("\n");
	}
	printf("\n");

	for(i=0; i<8; i++)
	{
		for(j=0; j<8; j++)
		{
			printf("%.2f ", btac_band0_tsmbacc[i*8+j]/((double)n));
		}
		printf("\n");
	}
	printf("\n");

	h=0;
	for(i=0; i<16; i++)
	{
		for(j=0; j<16; j++)
		{
			f=btac_band0_tcmbcnt[i*16+j]/(double)n;
			printf("%.1f ", f);
			h+=f;
		}
		printf("\n");
	}
	printf("%f\n", h*8);
	printf("\n");

	h=0;
	for(i=0; i<16; i++)
	{
		for(j=0; j<16; j++)
		{
			f=btac_band0_tsmbcnt[i*16+j]/(double)n;
			printf("%.1f ", f);
			h+=f;
		}
		printf("\n");
	}
	printf("%f\n", h*8);
	printf("\n");
	
	BTAC2_Encode_BuildLengths(btac_band0_tcmbcnt, 256, tcmbcl, 15);
	BTAC2_Encode_BuildLengths(btac_band0_tsmbcnt, 256, tsmbcl, 15);

	totb=0;
	for(i=0; i<16; i++)
	{
		for(j=0; j<16; j++)
		{
			k=tcmbcl[i*16+j];
			totb+=(k*btac_band0_tcmbcnt[i*16+j])/n;
			printf("%2d ", k);
		}
		printf("\n");
	}
	printf("Avg Tot=%d\n", totb);
	printf("\n");

	totb=0;
	for(i=0; i<16; i++)
	{
		for(j=0; j<16; j++)
		{
			k=tsmbcl[i*16+j];
			totb+=(k*btac_band0_tsmbcnt[i*16+j])/n;
			printf("%2d ", k);
		}
		printf("\n");
	}
	printf("Avg Tot=%d\n", totb);
	printf("\n");

//	printf("Encode %d: ", n);
//	for(i=0; i<16; i++)
//		printf("%d ", tcnt[i]);
//	printf("\n");
}
#endif
