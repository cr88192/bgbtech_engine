#include <bgbbtj.h>

extern const u32 btic2c_dbase[64];
extern const int btic2c_dextra[64];
extern const int btic2c_lbase[64];
extern const int btic2c_lextra[64];

int BTIC3AH_InitStreamWrite(BTIC3A_BitstreamCtx *ctx, byte *buf, int sz)
{
	ctx->huff_pos=0;
	ctx->huff_isend=0;
	ctx->huff_win=0;
	ctx->huff_ct=buf;
	ctx->huff_cte=buf+sz;
	return(0);
}

void BTIC3AH_WriteBit(BTIC3A_BitstreamCtx *ctx, int i)
{
	ctx->huff_win|=i<<(31-ctx->huff_pos);
	ctx->huff_pos++;
	if(ctx->huff_pos>=8)
	{
		i=(ctx->huff_win>>24)&0xFF;
		*ctx->huff_ct++=i;
//		if(i==0xFF)*ctx->huff_ct++=0x00;
		ctx->huff_win<<=8;
		ctx->huff_pos-=8;
	}
}

void BTIC3AH_WriteNBits(BTIC3A_BitstreamCtx *ctx, int v, int n)
{
	int i, j, k;

	v&=(1<<n)-1;

	j=ctx->huff_pos+n;
	k=ctx->huff_win|(v<<(32-j));
	while(j>=8)
	{
		i=(k>>24)&0xFF;
		*ctx->huff_ct++=i;
//		if(i==0xFF)*ctx->huff_ct++=0x00;
		k<<=8;
		j-=8;
	}
	ctx->huff_pos=j;
	ctx->huff_win=k;
}

void BTIC3AH_WriteNBitsL(BTIC3A_BitstreamCtx *ctx, u64 i, int n)
{
	while(n>=16)
	{
		BTIC3AH_WriteNBits(ctx, i&65535, 16);
		i=i>>16;
		i-=16;
	}
	BTIC3AH_WriteNBits(ctx, i, n);
}

void BTIC3AH_Write2Bits(BTIC3A_BitstreamCtx *ctx, int i)
	{ BTIC3AH_WriteNBits(ctx, i, 2); }
void BTIC3AH_Write3Bits(BTIC3A_BitstreamCtx *ctx, int i)
	{ BTIC3AH_WriteNBits(ctx, i, 3); }
void BTIC3AH_Write4Bits(BTIC3A_BitstreamCtx *ctx, int i)
	{ BTIC3AH_WriteNBits(ctx, i, 4); }

void BTIC3AH_WriteRice(BTIC3A_BitstreamCtx *ctx, int v, int k)
{
	int j;
	
	j=v>>k;
	while(j--)
	 { BTIC3AH_WriteBit(ctx, 1); }
	BTIC3AH_WriteBit(ctx, 0);
	j=v&((1<<k)-1);
	BTIC3AH_WriteNBits(ctx, j, k);
}

int BTIC3AH_CountWriteRice(BTIC3A_BitstreamCtx *ctx, int v, int k)
{
	int j;
	j=v>>k;
	return(j+k);
}

void BTIC3AH_WriteSRice(BTIC3A_BitstreamCtx *ctx, int v, int k)
{
	BTIC3AH_WriteRice(ctx, (v>=0)?(v<<1):(((-v)<<1)-1), k);
}

int BTIC3AH_CountWriteSRice(BTIC3A_BitstreamCtx *ctx, int v, int k)
{
	return(BTIC3AH_CountWriteRice(ctx,
		(v>=0)?(v<<1):(((-v)<<1)-1), k));
}

void BTIC3AH_WriteExpRice(BTIC3A_BitstreamCtx *ctx, int v, int k)
{
	int i, j;
	
	if(v<=0)
	{
		BTIC3AH_WriteRice(ctx, 0, k);
		return;
	}
	
	i=v; j=0;
	while(i>0) { i>>=1; j++; }
	BTIC3AH_WriteRice(ctx, j, k);
	j=v&((1<<k)-1);
	BTIC3AH_WriteNBitsL(ctx, j, i);
}

void BTIC3AH_WriteExpSRice(BTIC3A_BitstreamCtx *ctx, int v, int k)
{
	BTIC3AH_WriteExpRice(ctx, (v>=0)?(v<<1):(((-v)<<1)-1), k);
}

void BTIC3AH_WriteAdRice(BTIC3A_BitstreamCtx *ctx, int v, int *rk)
{
	int i, j, k, p;
	
	k=*rk;
	p=v>>k; j=p;
	while(j--)
	 { BTIC3AH_WriteBit(ctx, 1); }
	BTIC3AH_WriteBit(ctx, 0);
	j=v&((1<<k)-1);
	BTIC3AH_WriteNBits(ctx, j, k);
	
	if(!p && (k>0))k--;
	if(p>1)
	{
		j=p;
		while(j>1)
			{ k++; j>>=1; }
	}
	*rk=k;
}

void BTIC3AH_WriteAdSRice(BTIC3A_BitstreamCtx *ctx, int v, int *rk)
{
	BTIC3AH_WriteAdRice(ctx, (v>=0)?(v<<1):(((-v)<<1)-1), rk);
}

void BTIC3AH_FlushBits(BTIC3A_BitstreamCtx *ctx)
{
	int i;
	while(ctx->huff_pos>0)
	{
		i=(ctx->huff_win>>24)&0xFF;
		*ctx->huff_ct++=i;
//		if(i==0xFF)*ctx->huff_ct++=0x00;
		ctx->huff_win<<=8;
		ctx->huff_pos-=8;
	}
}

void BTIC3AH_WriteString(BTIC3A_BitstreamCtx *ctx, char *str)
{
	char *s;

	BTIC3AH_FlushBits(ctx);	
	s=str;
	while(*s)
		*ctx->huff_ct++=*s++;
	*ctx->huff_ct++=0;
}

int BTIC3AH_BalanceTree_r(short *nodes, short *nlen,
	int root, int h, int ml)
{
	int h0, h1, h2, h3;
	int l0, l1, l2;

	if(root<0)return(0);

	h1=BTIC3AH_BalanceTree_r(nodes, nlen, nodes[root*2+0], h+1, ml);
	h2=BTIC3AH_BalanceTree_r(nodes, nlen, nodes[root*2+1], h+1, ml);
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

void BTIC3AH_CalcLengths_r(
	short *nodes, byte *cl, int root, int h)
{
	if(root<0)
	{
		if(root==-513)return;
		cl[(-root)-1]=h;
		return;
	}

	BTIC3AH_CalcLengths_r(nodes, cl, nodes[root*2+0], h+1);
	BTIC3AH_CalcLengths_r(nodes, cl, nodes[root*2+1], h+1);
}

int BTIC3AH_BuildLengths(
	int *stat, int nc, byte *cl, int ml)
{
//	static short nodes[1024], nlen[512];
//	static short roots[512], clen[512];
//	static int cnts[512];

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
		k=BTIC3AH_BalanceTree_r(nodes, nlen, l, 0, ml);
	if(k>ml)return(-2);	//failed to balance tree

	BTIC3AH_CalcLengths_r(nodes, cl, l, 0);
	return(0);
}

int BTIC3AH_BuildLengthsAdjust(
	int *stat, int nc, byte *cl, int ml)
{
	int i, j;

	while(1)
	{
		j=BTIC3AH_BuildLengths(stat, nc, cl, ml);
		if(j<0)
			printf("BTIC3AH_BuildLengthsAdjust: Huff Fail %d\n");

		for(i=0; i<nc; i++)
			if(stat[i] && !cl[i])
				break;
		if(i>=nc)break;

		printf("BTIC3AH_BuildLengthsAdjust: Fiddle Adjust\n");
		for(i=0; i<nc; i++)
			stat[i]++;
		continue;
	}
	return(0);
}

void BTIC3AH_EncodeSymbol(BTIC3A_BitstreamCtx *ctx, int tab, int v)
{
	int t;

	t=tab<<8;
	BTIC3AH_WriteNBits(ctx, ctx->huff_code[t|v], ctx->huff_len[t|v]);

	ctx->cnt_hsn[tab]++;
	ctx->cnt_hsb[tab]+=ctx->huff_len[t|v];
}

void BTIC3AH_EncodeCoeffUValue(BTIC3A_BitstreamCtx *ctx,
	int tab, int z, u32 val)
{
	int i, j, k;

	j=val;
	for(i=0; i<64; i++)
	{
		k=btic2c_dbase[i]+(1<<btic2c_dextra[i]);
		if((j>=btic2c_dbase[i]) && (j<k))
		{
			BTIC3AH_EncodeSymbol(ctx, tab, i|(z<<5));
			BTIC3AH_WriteNBits(ctx, j-btic2c_dbase[i], btic2c_dextra[i]);

			ctx->cnt_hdn[tab]++;
			ctx->cnt_hdb[tab]+=btic2c_dextra[i];
			break;
		}
	}
}

void BTIC3AH_StatCoeffUValue(BTIC3A_BitstreamCtx *ctx,
	int *stat, int z, u32 val)
{
	int i, j, k;

	j=val;
	for(i=0; i<64; i++)
	{
		k=btic2c_dbase[i]+(1<<btic2c_dextra[i]);
		if((j>=btic2c_dbase[i]) && (j<k))
		{
			stat[i|(z<<5)]++;
			break;
		}
	}
}

void BTIC3AH_EncodeLenUValue(BTIC3A_BitstreamCtx *ctx,
	int tab, int z, u32 val)
{
	int i, j, k;

	j=val;
	for(i=0; i<64; i++)
	{
		k=btic2c_lbase[i]+(1<<btic2c_lextra[i]);
		if((j>=btic2c_lbase[i]) && (j<k))
		{
			BTIC3AH_EncodeSymbol(ctx, tab, i|(z<<5));
			BTIC3AH_WriteNBits(ctx, j-btic2c_lbase[i], btic2c_lextra[i]);

			ctx->cnt_hln[tab]++;
			ctx->cnt_hlb[tab]+=btic2c_lextra[i];
			break;
		}
	}
}

void BTIC3AH_StatLenUValue(BTIC3A_BitstreamCtx *ctx,
	int *stat, int z, u32 val)
{
	int i, j, k;

	j=val;
	for(i=0; i<64; i++)
	{
		k=btic2c_lbase[i]+(1<<btic2c_lextra[i]);
		if((j>=btic2c_lbase[i]) && (j<k))
		{
			stat[i|(z<<5)]++;
			break;
		}
	}
}

void BTIC3AH_EncodeVal(BTIC3A_BitstreamCtx *ctx,
	int tab, int tab2, int z, int v)
{
	int i, j, k;

	if(!v && (z<7)) { BTIC3AH_EncodeSymbol(ctx, tab, z<<5); return; }

#if 0
	if(v>0)
	{
		i=1; while(v>=(1<<i))i++;
		BTIC3AH_EncodeSymbol(ctx, tab, (z<<4)|i);
		BTIC3AH_WriteNBits(ctx, v, i);
		return;
	}

	i=1; j=v; while(j>=(1<<i))i++;
	BTIC3AH_EncodeSymbol(ctx, tab, (z<<4)|i);

	k=(1<<i)-(j+1);
	BTIC3AH_WriteNBits(ctx, k, i);
#endif

	j=(v<0)?(((-v)<<1)-1):(v<<1);
	if((z>=7) || (j>=65536))
	{
		BTIC3AH_EncodeLenUValue(ctx, tab, 7, z);
		BTIC3AH_EncodeCoeffUValue(ctx, tab2, 0, j);
	}else
	{
		BTIC3AH_EncodeCoeffUValue(ctx, tab, z, j);
	}

	return;
}

void BTIC3AH_StatVal(BTIC3A_BitstreamCtx *ctx,
	int *stat, int *stat2, int z, int v)
{
	int i, j, k;

	if(!v && (z<7)) { stat[z<<5]++; return; }

#if 0
	if(v>0)
	{
		i=1; while(v>=(1<<i))i++;
		stat[(z<<5)|i]++;
		return;
	}

	i=1; j=-v; while(j>=(1<<i))i++;
	stat[(z<<5)|i]++;
#endif

	j=(v<0)?(((-v)<<1)-1):(v<<1);
	if((z>=7) || (j>=65536))
	{
		BTIC3AH_StatLenUValue(ctx, stat, 7, z);
		BTIC3AH_StatCoeffUValue(ctx, stat2, 0, j);
	}else
	{
		BTIC3AH_StatCoeffUValue(ctx, stat, z, j);
	}
}

void BTIC3AH_EncodeDiffDC(BTIC3A_BitstreamCtx *ctx,
	int tab, int tab2, int v)
{
	int j;
	j=(v<0)?(((-v)<<1)-1):(v<<1);
	BTIC3AH_EncodeCoeffUValue(ctx, tab, 0, j);
}

void BTIC3AH_StatDiffDC(BTIC3A_BitstreamCtx *ctx,
	int *stat, int *stat2, int v)
{
	int j;
	j=(v<0)?(((-v)<<1)-1):(v<<1);
	BTIC3AH_StatCoeffUValue(ctx, stat, 0, j);
}

#if 1
static int btic2c_lorder[]={
	16, 17, 18, 0, 8,7, 9,6, 10,5, 11,4, 12,3, 13,2, 14,1, 15, 19};
static int btic2c_lorder2[]={
	3, 17, 15, 13, 11,9, 7,5, 4,6, 8,10, 12,14, 16,18, 0,1, 2, 19};

void BTIC3A_EncodeHufftabSymbol(BTIC3A_BitstreamCtx *ctx,
	int v, int *rk)
{
	BTIC3AH_WriteAdRice(ctx, btic2c_lorder2[v], rk);
//	BTIC3AH_WriteRice(ctx, btic2c_lorder2[v], 2);
}

void BTIC3A_EncodeCodeLengths(BTIC3A_BitstreamCtx *ctx,
	byte *cl, int nc, int *rk)
{
	int i, j, l, lk;

	i=0; l=-1; lk=*rk;
	while(i<nc)
	{
		if(lk!=*rk)
		{
			lk=*rk;
//			printf("%d ", lk);
		}
	
		for(j=0; ((i+j)<nc) && (cl[i+j]==l); j++);
		if(j>10)
		{
			//check for end of table
			if(((i+j)>=nc) && (l==0))
				break;
		
			if(j>138)j=138;
			BTIC3A_EncodeHufftabSymbol(ctx, 18, rk);
			BTIC3AH_WriteNBits(ctx, j-11, 7);
			i+=j;
			continue;
		}
		if(j>2)
		{
			BTIC3A_EncodeHufftabSymbol(ctx, 17, rk);
			BTIC3AH_Write3Bits(ctx, j-3);
			i+=j;
			continue;
		}

		l=cl[i++];
		BTIC3A_EncodeHufftabSymbol(ctx, l, rk);

		if((l<0) || (l>16))printf("problem: bad code length noted\n");
	}
	BTIC3A_EncodeHufftabSymbol(ctx, 19, rk);
//	printf("\n");
}

int BTIC3AH_EncodeHuffTable(BTIC3A_BitstreamCtx *ctx,
	int tab, byte *cl, int nc)
{
	int i, j, rk, lc;

	BTIC3AH_WriteNBits(ctx, 1, 4);		//tsz
	BTIC3AH_WriteNBits(ctx, 0, 4);		//rice factor
	
	rk=4;
	BTIC3A_EncodeCodeLengths(ctx, cl, nc, &rk);

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
#endif

int BTIC3AH_EncodeDHT(BTIC3A_BitstreamCtx *ctx,
	byte *obuf, int osz, int tab)
{
	byte *ct0;
	int i, j, k, t;

	ct0=ctx->huff_ct;
//	*ctx->huff_ct++=0xE3;
//	*ctx->huff_ct++=0x00;
//	*ctx->huff_ct++=0x00;
//	*ctx->huff_ct++=0x00;
//	*ctx->huff_ct++='H';
//	*ctx->huff_ct++='0'+tab;

#if 1
	t=tab<<8;

	ctx->huff_win=0;
	ctx->huff_pos=0;

//	*ctx->huff_ct++=0x10;	//TtTb
	BTIC3AH_InitStreamWrite(ctx, obuf, osz);
	BTIC3A_EncodeHuffTable(ctx, tab, ctx->huff_len+t, 256);
	BTIC3AH_FlushBits(ctx);

	k=0;
	for(i=1; i<=16; i++)
	{
		k<<=1;
		for(j=0; j<256; j++)
			if(ctx->huff_len[t|j]==i)
		{
			ctx->huff_code[t|j]=k++;
		}
	}

	if(k>=65536)
		printf("BTIC3A_EmitDHT: Error %04X\n", k);
#endif

#if 0
	*ctx->huff_ct++=0x00;	//TtTb

	tab<<=8;
	for(i=1; i<=16; i++)
	{
		k=0;
		for(j=0; j<256; j++)
			if(ctx->huff_len[tab|j]==i)
				k++;
		*ctx->huff_ct++=k; 	//Li
	}

	k=0;
	for(i=1; i<=16; i++)
	{
		k<<=1;
		for(j=0; j<256; j++)
			if(ctx->huff_len[tab|j]==i)
		{
			*ctx->huff_ct++=j; 	//Vi
			ctx->huff_code[tab|j]=k++;
		}
	}

	if(k>=65536)
		printf("BTIC3A_EmitDHT: Error %04X\n", k);
#endif

//	i=ctx->huff_ct-ct0;
//	ct0[1]=(i>>16)&255;
//	ct0[2]=(i>> 8)&255;
//	ct0[3]=(i>> 0)&255;

	i=ctx->huff_ct-obuf;
	ctx->huff_ct=ct0;

	return(i);
}
