#include <bgbbtj.h>
#include <math.h>

extern const int pdjpg_zigzag[64];
extern const int pdjpg_zigzag2[64];

extern const u32 btic2c_dbase[64];
extern const int btic2c_dextra[64];
extern const int btic2c_lbase[64];
extern const int btic2c_lextra[64];

#if 1
void BTIC2C_TransDCT_Horiz(s32 *iblk, s32 *oblk)
{
	int ib[8];

	ib[0]=iblk[0];	ib[1]=iblk[1];
	ib[2]=iblk[2];	ib[3]=iblk[3];
	ib[4]=iblk[4];	ib[5]=iblk[5];
	ib[6]=iblk[6];	ib[7]=iblk[7];

	oblk[0]=ib[0]*91  +ib[1]*91  +ib[2]*91  +ib[3]*91  
			+ib[4]*91  +ib[5]*91  +ib[6]*91  +ib[7]*91;
	oblk[1]=ib[0]*126 +ib[1]*106 +ib[2]*71  +ib[3]*25  
			-ib[4]*25  -ib[5]*71  -ib[6]*106 -ib[7]*126;
	oblk[2]=ib[0]*118 +ib[1]*49  -ib[2]*49  -ib[3]*118 
			-ib[4]*118 -ib[5]*49  +ib[6]*49  +ib[7]*118;
	oblk[3]=ib[0]*106 -ib[1]*25  -ib[2]*126 -ib[3]*71  
			+ib[4]*71  +ib[5]*126 +ib[6]*25  -ib[7]*106;
	oblk[4]=ib[0]*91  -ib[1]*91  -ib[2]*91  +ib[3]*91  
			+ib[4]*91  -ib[5]*91  -ib[6]*91  +ib[7]*91;
	oblk[5]=ib[0]*71  -ib[1]*126 +ib[2]*25  +ib[3]*106 
			-ib[4]*106 -ib[5]*25  +ib[6]*126 -ib[7]*71;
	oblk[6]=ib[0]*49  -ib[1]*118 +ib[2]*118 -ib[3]*49  
			-ib[4]*49  +ib[5]*118 -ib[6]*118 +ib[7]*49;
	oblk[7]=ib[0]*25  -ib[1]*71  +ib[2]*106 -ib[3]*126 
			+ib[4]*126 -ib[5]*106 +ib[6]*71  -ib[7]*25;
}

void BTIC2C_TransDCT_Vert(s32 *iblk, s32 *oblk)
{
	oblk[ 0]=iblk[0]*91  +iblk[8]*91  
			+iblk[16]*91  +iblk[24]*91  
			+iblk[32]*91  +iblk[40]*91  
			+iblk[48]*91  +iblk[56]*91;
	oblk[ 8]=iblk[0]*126 +iblk[8]*106 
			+iblk[16]*71  +iblk[24]*25  
			-iblk[32]*25  -iblk[40]*71  
			-iblk[48]*106 -iblk[56]*126;
	oblk[16]=iblk[0]*118 +iblk[8]*49  
			-iblk[16]*49  -iblk[24]*118 
			-iblk[32]*118 -iblk[40]*49  
			+iblk[48]*49  +iblk[56]*118;
	oblk[24]=iblk[0]*106 -iblk[8]*25  
			-iblk[16]*126 -iblk[24]*71  
			+iblk[32]*71  +iblk[40]*126 
			+iblk[48]*25  -iblk[56]*106;
	oblk[32]=iblk[0]*91  -iblk[8]*91  
			-iblk[16]*91  +iblk[24]*91  
			+iblk[32]*91  -iblk[40]*91  
			-iblk[48]*91  +iblk[56]*91;
	oblk[40]=iblk[0]*71  -iblk[8]*126 
			+iblk[16]*25  +iblk[24]*106
			-iblk[32]*106 -iblk[40]*25  
			+iblk[48]*126 -iblk[56]*71;
	oblk[48]=iblk[0]*49  -iblk[8]*118
			+iblk[16]*118 -iblk[24]*49  
			-iblk[32]*49  +iblk[40]*118 
			-iblk[48]*118 +iblk[56]*49;
	oblk[56]=iblk[0]*25  -iblk[8]*71  
			+iblk[16]*106 -iblk[24]*126 
			+iblk[32]*126 -iblk[40]*106 
			+iblk[48]*71  -iblk[56]*25;
}

void BTIC2C_TransDCT(s32 *iblk, s32 *oblk)
{
	int s[BTIC2C_DCTSZ2];
	int t[BTIC2C_DCTSZ2];
	int i, j;

	BTIC2C_TransDCT_Horiz(iblk+0, s+0);
	BTIC2C_TransDCT_Horiz(iblk+8, s+8);
	BTIC2C_TransDCT_Horiz(iblk+16, s+16);
	BTIC2C_TransDCT_Horiz(iblk+24, s+24);
	BTIC2C_TransDCT_Horiz(iblk+32, s+32);
	BTIC2C_TransDCT_Horiz(iblk+40, s+40);
	BTIC2C_TransDCT_Horiz(iblk+48, s+48);
	BTIC2C_TransDCT_Horiz(iblk+56, s+56);

	BTIC2C_TransDCT_Vert(s+0, t+0);
	BTIC2C_TransDCT_Vert(s+1, t+1);
	BTIC2C_TransDCT_Vert(s+2, t+2);
	BTIC2C_TransDCT_Vert(s+3, t+3);
	BTIC2C_TransDCT_Vert(s+4, t+4);
	BTIC2C_TransDCT_Vert(s+5, t+5);
	BTIC2C_TransDCT_Vert(s+6, t+6);
	BTIC2C_TransDCT_Vert(s+7, t+7);

	for(i=0; i<64; i++)
		oblk[i]=t[i]>>16;
}
#endif

#if 1
void BTIC2C_TransWHT_Horiz(s32 *iblk, s32 *oblk)
{
	int ib0, ib1, ib2, ib3, ib4, ib5, ib6, ib7;
	int a0, a1, a2, a3, a4, a5, a6, a7;
	int b0, b1, b2, b3, b4, b5, b6, b7;
	int c0, c1, c2, c3, c4, c5, c6, c7;

	ib0=iblk[0];	ib1=iblk[1];
	ib2=iblk[2];	ib3=iblk[3];
	ib4=iblk[4];	ib5=iblk[5];
	ib6=iblk[6];	ib7=iblk[7];

	a0=ib0+ib4;			a1=ib1+ib5;
	a2=ib2+ib6;			a3=ib3+ib7;
	a4=(a0>>1)-ib4;		a5=(a1>>1)-ib5;
	a6=(a2>>1)-ib6;		a7=(a3>>1)-ib7;

	b0=a0+a2;
	b1=a1+a3;
	b2=(b0>>1)-a2;
	b3=(b1>>1)-a3;
	b4=a4+a6;
	b5=a5+a7;
	b6=(b4>>1)-a6;
	b7=(b5>>1)-a7;

	c0=b0+b1;
	c1=(c0>>1)-b1;
	c2=b2+b3;
	c3=(c2>>1)-b3;
	c4=b4+b5;
	c5=(c4>>1)-b5;
	c6=b6+b7;
	c7=(c6>>1)-b7;

	oblk[0]=c0;	oblk[1]=c4;
	oblk[2]=c6;	oblk[3]=c2;
	oblk[4]=c3;	oblk[5]=c7;
	oblk[6]=c5;	oblk[7]=c1;
}

void BTIC2C_TransWHT_Vert(s32 *iblk, s32 *oblk)
{
	int ib0, ib1, ib2, ib3, ib4, ib5, ib6, ib7;
	int a0, a1, a2, a3, a4, a5, a6, a7;
	int b0, b1, b2, b3, b4, b5, b6, b7;
	int c0, c1, c2, c3, c4, c5, c6, c7;

	ib0=iblk[8*0];	ib1=iblk[8*1];
	ib2=iblk[8*2];	ib3=iblk[8*3];
	ib4=iblk[8*4];	ib5=iblk[8*5];
	ib6=iblk[8*6];	ib7=iblk[8*7];

	a0=ib0+ib4;			a1=ib1+ib5;
	a2=ib2+ib6;			a3=ib3+ib7;
	a4=(a0>>1)-ib4;		a5=(a1>>1)-ib5;
	a6=(a2>>1)-ib6;		a7=(a3>>1)-ib7;

	b0=a0+a2;
	b1=a1+a3;
	b2=(b0>>1)-a2;
	b3=(b1>>1)-a3;
	b4=a4+a6;
	b5=a5+a7;
	b6=(b4>>1)-a6;
	b7=(b5>>1)-a7;

	c0=b0+b1;
	c1=(c0>>1)-b1;
	c2=b2+b3;
	c3=(c2>>1)-b3;
	c4=b4+b5;
	c5=(c4>>1)-b5;
	c6=b6+b7;
	c7=(c6>>1)-b7;

	oblk[8*0]=c0;	oblk[8*1]=c4;
	oblk[8*2]=c6;	oblk[8*3]=c2;
	oblk[8*4]=c3;	oblk[8*5]=c7;
	oblk[8*6]=c5;	oblk[8*7]=c1;
}

void BTIC2C_TransWHT(s32 *iblk, s32 *oblk)
{
	int s[BTIC2C_DCTSZ2];
	int t[BTIC2C_DCTSZ2];
	int i, j;

	BTIC2C_TransWHT_Horiz(iblk+0, s+0);
	BTIC2C_TransWHT_Horiz(iblk+8, s+8);
	BTIC2C_TransWHT_Horiz(iblk+16, s+16);
	BTIC2C_TransWHT_Horiz(iblk+24, s+24);
	BTIC2C_TransWHT_Horiz(iblk+32, s+32);
	BTIC2C_TransWHT_Horiz(iblk+40, s+40);
	BTIC2C_TransWHT_Horiz(iblk+48, s+48);
	BTIC2C_TransWHT_Horiz(iblk+56, s+56);

	BTIC2C_TransWHT_Vert(s+0, oblk+0);
	BTIC2C_TransWHT_Vert(s+1, oblk+1);
	BTIC2C_TransWHT_Vert(s+2, oblk+2);
	BTIC2C_TransWHT_Vert(s+3, oblk+3);
	BTIC2C_TransWHT_Vert(s+4, oblk+4);
	BTIC2C_TransWHT_Vert(s+5, oblk+5);
	BTIC2C_TransWHT_Vert(s+6, oblk+6);
	BTIC2C_TransWHT_Vert(s+7, oblk+7);
}
#endif

int BTIC2CH_QuantBlock(
	BTIC2C_Context *ctx, s32 *ibuf, s32 *obuf, int qid)
{
	int i, j, rs;
	
//	for(i=0; i<64; i++)
//		obuf[i]=ibuf[i]/ctx->img_qt[qid][i];

//	for(i=0; i<64; i++)
//		obuf[i]=(ibuf[i]*ctx->img_qtfp[qid][i])>>12;

	rs=1;
	for(i=0; i<64; i++)
	{
		j=(ibuf[i]*ctx->img_qtfp[qid][i]+2048)>>12;
		obuf[i]=j;
		if(i && j)
			{ rs=0; }
	}

	return(rs);
}

void BTIC2C_SetupQuantTabDivFP(BTIC2C_Context *ctx, int qid)
{
	int i;
	
	for(i=0; i<64; i++)
		ctx->img_qtfp[qid][i]=4096.0/ctx->img_qt[qid][i];
}

//Encoder

void BTIC2CH_WriteBit(BTIC2C_Context *ctx, int i)
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

void BTIC2CH_WriteNBits(BTIC2C_Context *ctx, int v, int n)
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

void BTIC2CH_WriteNBitsL(BTIC2C_Context *ctx, u64 i, int n)
{
	while(n>=16)
	{
		BTIC2CH_WriteNBits(ctx, i&65535, 16);
		i=i>>16;
		i-=16;
	}
	BTIC2CH_WriteNBits(ctx, i, n);
}

void BTIC2CH_Write2Bits(BTIC2C_Context *ctx, int i)
	{ BTIC2CH_WriteNBits(ctx, i, 2); }
void BTIC2CH_Write3Bits(BTIC2C_Context *ctx, int i)
	{ BTIC2CH_WriteNBits(ctx, i, 3); }
void BTIC2CH_Write4Bits(BTIC2C_Context *ctx, int i)
	{ BTIC2CH_WriteNBits(ctx, i, 4); }

void BTIC2CH_WriteRice(BTIC2C_Context *ctx, int v, int k)
{
	int j;
	
	j=v>>k;
	while(j--)
	 { BTIC2CH_WriteBit(ctx, 1); }
	BTIC2CH_WriteBit(ctx, 0);
	j=v&((1<<k)-1);
	BTIC2CH_WriteNBits(ctx, j, k);
}

int BTIC2CH_CountWriteRice(BTIC2C_Context *ctx, int v, int k)
{
	int j;
	j=v>>k;
	return(j+k);
}

void BTIC2CH_WriteSRice(BTIC2C_Context *ctx, int v, int k)
{
	BTIC2CH_WriteRice(ctx, (v>=0)?(v<<1):(((-v)<<1)-1), k);
}

int BTIC2CH_CountWriteSRice(BTIC2C_Context *ctx, int v, int k)
{
	return(BTIC2CH_CountWriteRice(ctx,
		(v>=0)?(v<<1):(((-v)<<1)-1), k));
}

void BTIC2CH_WriteExpRice(BTIC2C_Context *ctx, int v, int k)
{
	int i, j;
	
	if(v<=0)
	{
		BTIC2CH_WriteRice(ctx, 0, k);
		return;
	}
	
	i=v; j=0;
	while(i>0) { i>>=1; j++; }
	BTIC2CH_WriteRice(ctx, j, k);
	j=v&((1<<k)-1);
	BTIC2CH_WriteNBitsL(ctx, j, i);
}

void BTIC2CH_WriteExpSRice(BTIC2C_Context *ctx, int v, int k)
{
	BTIC2CH_WriteExpRice(ctx, (v>=0)?(v<<1):(((-v)<<1)-1), k);
}

void BTIC2CH_WriteAdRice(BTIC2C_Context *ctx, int v, int *rk)
{
	int i, j, k, p;
	
	k=*rk;
	p=v>>k; j=p;
	while(j--)
	 { BTIC2CH_WriteBit(ctx, 1); }
	BTIC2CH_WriteBit(ctx, 0);
	j=v&((1<<k)-1);
	BTIC2CH_WriteNBits(ctx, j, k);
	
	if(!p && (k>0))k--;
	if(p>1)
	{
		j=p;
		while(j>1)
			{ k++; j>>=1; }
	}
	*rk=k;
}

void BTIC2CH_WriteAdSRice(BTIC2C_Context *ctx, int v, int *rk)
{
	BTIC2CH_WriteAdRice(ctx, (v>=0)?(v<<1):(((-v)<<1)-1), rk);
}

void BTIC2CH_FlushBits(BTIC2C_Context *ctx)
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

void BTIC2CH_WriteString(BTIC2C_Context *ctx, char *str)
{
	char *s;

	BTIC2CH_FlushBits(ctx);	
	s=str;
	while(*s)
		*ctx->huff_ct++=*s++;
	*ctx->huff_ct++=0;
}

int BTIC2CH_BalanceTree_r(short *nodes, short *nlen,
	int root, int h, int ml)
{
	int h0, h1, h2, h3;
	int l0, l1, l2;

	if(root<0)return(0);

	h1=BTIC2CH_BalanceTree_r(nodes, nlen, nodes[root*2+0], h+1, ml);
	h2=BTIC2CH_BalanceTree_r(nodes, nlen, nodes[root*2+1], h+1, ml);
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

void BTIC2CH_CalcLengths_r(
	short *nodes, byte *cl, int root, int h)
{
	if(root<0)
	{
		if(root==-513)return;
		cl[(-root)-1]=h;
		return;
	}

	BTIC2CH_CalcLengths_r(nodes, cl, nodes[root*2+0], h+1);
	BTIC2CH_CalcLengths_r(nodes, cl, nodes[root*2+1], h+1);
}

int BTIC2CH_BuildLengths(
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
		k=BTIC2CH_BalanceTree_r(nodes, nlen, l, 0, ml);
	if(k>ml)return(-2);	//failed to balance tree

	BTIC2CH_CalcLengths_r(nodes, cl, l, 0);
	return(0);
}

int BTIC2CH_BuildLengthsAdjust(
	int *stat, int nc, byte *cl, int ml)
{
	int i, j;

	while(1)
	{
		j=BTIC2CH_BuildLengths(stat, nc, cl, ml);
		if(j<0)
			printf("BTIC2CH_BuildLengthsAdjust: Huff Fail %d\n");

		for(i=0; i<nc; i++)
			if(stat[i] && !cl[i])
				break;
		if(i>=nc)break;

		printf("BTIC2CH_BuildLengthsAdjust: Fiddle Adjust\n");
		for(i=0; i<nc; i++)
			stat[i]++;
		continue;
	}
	return(0);
}

void BTIC2CH_EncodeSymbol(BTIC2C_Context *ctx, int tab, int v)
{
	int t;

	t=tab<<8;
	BTIC2CH_WriteNBits(ctx, ctx->huff_code[t|v], ctx->huff_len[t|v]);

	ctx->cnt_hsn[tab]++;
	ctx->cnt_hsb[tab]+=ctx->huff_len[t|v];
}

void BTIC2CH_EncodeCoeffUValue(BTIC2C_Context *ctx,
	int tab, int z, u32 val)
{
	int i, j, k;

	j=val;
	for(i=0; i<64; i++)
	{
		k=btic2c_dbase[i]+(1<<btic2c_dextra[i]);
		if((j>=btic2c_dbase[i]) && (j<k))
		{
			BTIC2CH_EncodeSymbol(ctx, tab, i|(z<<5));
			BTIC2CH_WriteNBits(ctx, j-btic2c_dbase[i], btic2c_dextra[i]);

			ctx->cnt_hdn[tab]++;
			ctx->cnt_hdb[tab]+=btic2c_dextra[i];
			break;
		}
	}
}

void BTIC2CH_StatCoeffUValue(BTIC2C_Context *ctx,
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

void BTIC2CH_EncodeLenUValue(BTIC2C_Context *ctx,
	int tab, int z, u32 val)
{
	int i, j, k;

	j=val;
	for(i=0; i<64; i++)
	{
		k=btic2c_lbase[i]+(1<<btic2c_lextra[i]);
		if((j>=btic2c_lbase[i]) && (j<k))
		{
			BTIC2CH_EncodeSymbol(ctx, tab, i|(z<<5));
			BTIC2CH_WriteNBits(ctx, j-btic2c_lbase[i], btic2c_lextra[i]);

			ctx->cnt_hln[tab]++;
			ctx->cnt_hlb[tab]+=btic2c_lextra[i];
			break;
		}
	}
}

void BTIC2CH_StatLenUValue(BTIC2C_Context *ctx,
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

void BTIC2CH_EncodeVal(BTIC2C_Context *ctx,
	int tab, int tab2, int z, int v)
{
	int i, j, k;

	if(!v && (z<7)) { BTIC2CH_EncodeSymbol(ctx, tab, z<<5); return; }

#if 0
	if(v>0)
	{
		i=1; while(v>=(1<<i))i++;
		BTIC2CH_EncodeSymbol(ctx, tab, (z<<4)|i);
		BTIC2CH_WriteNBits(ctx, v, i);
		return;
	}

	i=1; j=v; while(j>=(1<<i))i++;
	BTIC2CH_EncodeSymbol(ctx, tab, (z<<4)|i);

	k=(1<<i)-(j+1);
	BTIC2CH_WriteNBits(ctx, k, i);
#endif

	j=(v<0)?(((-v)<<1)-1):(v<<1);
	if((z>=7) || (j>=65536))
	{
		BTIC2CH_EncodeLenUValue(ctx, tab, 7, z);
		BTIC2CH_EncodeCoeffUValue(ctx, tab2, 0, j);
	}else
	{
		BTIC2CH_EncodeCoeffUValue(ctx, tab, z, j);
	}

	return;
}

void BTIC2CH_StatVal(BTIC2C_Context *ctx,
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
		BTIC2CH_StatLenUValue(ctx, stat, 7, z);
		BTIC2CH_StatCoeffUValue(ctx, stat2, 0, j);
	}else
	{
		BTIC2CH_StatCoeffUValue(ctx, stat, z, j);
	}
}

void BTIC2CH_EncodeDiffDC(BTIC2C_Context *ctx,
	int tab, int tab2, int v)
{
	int j;
	j=(v<0)?(((-v)<<1)-1):(v<<1);
	BTIC2CH_EncodeCoeffUValue(ctx, tab, 0, j);
}

void BTIC2CH_StatDiffDC(BTIC2C_Context *ctx,
	int *stat, int *stat2, int v)
{
	int j;
	j=(v<0)?(((-v)<<1)-1):(v<<1);
	BTIC2CH_StatCoeffUValue(ctx, stat, 0, j);
}

int BTIC2CH_EncodeBlockMode(BTIC2C_Context *ctx,
	byte *bmbuf, int cn, int dctab, int actab)
{
	int i, j;

	if(bmbuf[0]!=ctx->blk_mode[cn])
	{
		i=bmbuf[0];
		j=(i<0)?(((-i)<<1)-1):(i<<1);
		BTIC2CH_EncodeLenUValue(ctx, dctab, 4, 2);
		BTIC2CH_EncodeCoeffUValue(ctx, dctab, 5, j);
		ctx->blk_mode[cn]=i;
	}
	return(0);
}

int BTIC2CH_StatBlockMode(BTIC2C_Context *ctx,
	byte *bmbuf, int cn, int *dcstat, int *acstat)
{
	int i, j;

	if(bmbuf[0]!=ctx->blk_mode[cn])
	{
		i=bmbuf[0];
		j=(i<0)?(((-i)<<1)-1):(i<<1);
		BTIC2CH_StatLenUValue(ctx, dcstat, 4, 2);
		BTIC2CH_StatCoeffUValue(ctx, dcstat, 5, j);
		ctx->blk_mode[cn]=i;
	}
	return(0);
}

#if 1
int BTIC2CH_EncodeBlock(BTIC2C_Context *ctx,
	s32 *buf, int dctab, int actab, int dc2tab, int ac2tab)
{
	int l0, l1, l2, l3;
	int c, n, lm, ln;
	int i, j, k, l;

//	BTIC2CH_EncodeVal(ctx, dctab, 0, buf[0]);
	BTIC2CH_EncodeDiffDC(ctx, dctab, dc2tab, buf[0]);

	for(i=1; i<64; i++)
	{
#if 0
		if(buf[pdjpg_zigzag2[i]])
		{
			BTIC2CH_EncodeVal(ctx, actab, ac2tab, 0, buf[pdjpg_zigzag2[i]]);
			continue;
		}
#endif

		for(j=i; j<64; j++)
			if(buf[pdjpg_zigzag2[j]])break;
		if(j>=64)
		{
			BTIC2CH_EncodeSymbol(ctx, actab, 0);
			break;
		}

#if 1
		c=0; lm=99; ln=0;
//		for(k=j; k<61; k++)
//		for(k=j; k<64; k++)
		for(k=j; k<63; k++)
		{
			if(k>(j+10))
				{ break; }
			l0=buf[pdjpg_zigzag2[k+0]];
			l1=buf[pdjpg_zigzag2[k+1]];
//			l2=buf[pdjpg_zigzag2[k+2]];
//			l3=buf[pdjpg_zigzag2[k+3]];
//			if(!l0 && !l1 && !l2)
//			if(!l0 && !l1)
			if(!l0)
				{ break; }
			l=(l0<0)?(((-l0)<<1)-1):(l0<<1);
			if(l>=8)
				{ break; }
//			if(l>=4)
//				{ break; }
			if(l>c)
				{ c=l; }
			if(l<lm) { lm=l; }
			if(l>ln) { ln=l; }
		}
		n=k-j;
		if(n>=3)
		{
			if(n>10)n=10;
//			c=(c>3)?3:(c>1)?2:1;
//			c=(c>3)?3:(c>2)?2:1;

			if(lm>0)	{ c=(ln>3)?3:(ln>2)?2:1; }
			else		{ c=(ln>3)?3:2; }

			BTIC2CH_EncodeLenUValue(ctx, actab, 7, j-i);
//			BTIC2CH_EncodeSymbol(ctx, actab, (6<<5)|((c-1)<<3)|(n-3));
			BTIC2CH_EncodeSymbol(ctx, ac2tab, (6<<5)|((c-1)<<3)|(n-3));

			for(k=0; k<n; k++)
			{
				l=buf[pdjpg_zigzag2[j+k]];
				l=(l<0)?(((-l)<<1)-1):(l<<1);
				BTIC2CH_WriteNBits(ctx, l, c);
			}

			ctx->cnt_hvn[ac2tab]++;
			ctx->cnt_hvb[ac2tab]+=n*c;

//			BTIC2CH_EncodeCoeffUValue(ctx, tab, 0, j);
			i=j+n-1;
			continue;
		}
#endif


		j-=i;
//		if(j>15)j=15;
		BTIC2CH_EncodeVal(ctx, actab, ac2tab, j, buf[pdjpg_zigzag2[i+j]]);
		i+=j;
	}
	return(0);
}

int BTIC2CH_StatBlock(BTIC2C_Context *ctx, s32 *buf,
	int *dcstat, int *acstat,
	int *dc2stat, int *ac2stat)
{
	int l0, l1, l2, l3;
	int c, n, lm, ln;
	int i, j, k, l;

//	BTIC2CH_StatVal(ctx, dcstat, 0, buf[0]);
	BTIC2CH_StatDiffDC(ctx, dcstat, dc2stat, buf[0]);

	for(i=1; i<64; i++)
	{
#if 0
		if(buf[pdjpg_zigzag2[i]])
		{
			BTIC2CH_StatVal(ctx, acstat, ac2stat,
				0, buf[pdjpg_zigzag2[i]]);
			continue;
		}
#endif

		for(j=i; j<64; j++)
			if(buf[pdjpg_zigzag2[j]])break;
		if(j>=64)
		{
			acstat[0]++;
			break;
		}

#if 1
		c=0; lm=99; ln=0;
//		for(k=j; k<61; k++)
//		for(k=j; k<64; k++)
		for(k=j; k<63; k++)
		{
			if(k>(j+10))
				{ break; }
			l0=buf[pdjpg_zigzag2[k+0]];
			l1=buf[pdjpg_zigzag2[k+1]];
//			l2=buf[pdjpg_zigzag2[k+2]];
//			l3=buf[pdjpg_zigzag2[k+3]];
//			if(!l0 && !l1 && !l2)
//			if(!l0 && !l1)
			if(!l0)
				{ break; }
			l=(l0<0)?(((-l0)<<1)-1):(l0<<1);
			if(l>=8)
				{ break; }
//			if(l>=4)
//				{ break; }
			if(l>c)
				{ c=l; }
			if(l<lm) { lm=l; }
			if(l>ln) { ln=l; }
		}
		n=k-j;
		if(n>=3)
		{
			if(n>10)n=10;
//			c=(c>3)?3:(c>1)?2:1;

			if(lm>0)	{ c=(ln>3)?3:(ln>2)?2:1; }
			else		{ c=(ln>3)?3:2; }

			BTIC2CH_StatLenUValue(ctx, acstat, 7, j-i);
//			BTIC2CH_EncodeSymbol(ctx, actab, (6<<5)|((c-1)<<3)|(n-3));
//			acstat[(6<<5)|((c-1)<<3)|(n-3)]++;
			ac2stat[(6<<5)|((c-1)<<3)|(n-3)]++;

#if 0
			for(k=0; k<n; k++)
			{
				l=buf[pdjpg_zigzag2[j+k]];
				l=(l<0)?(((-l)<<1)-1):(l<<1);
//				BTIC2CH_WriteNBits(ctx, l, c);
			}
#endif

//			BTIC2CH_EncodeCoeffUValue(ctx, tab, 0, j);
			i=j+n-1;
			continue;
		}
#endif

		j-=i;
//		if(j>15)j=15;
		BTIC2CH_StatVal(ctx, acstat, ac2stat,
			j, buf[pdjpg_zigzag2[i+j]]);
		i+=j;
	}
	return(0);
}
#endif

#if defined(X86) || defined(X86_64)
#define memcpy8(dst, src)	(*(double *)(dst)=*(double *)(src))
#else
#define memcpy8(dst, src)	memcpy(dst, src, 8)
#endif

#if 0
// int BTIC2C_GetImgBlk(char *blk, int xo, int yo, byte *img, int xs, int ys)
{
	byte *cs;
	char *ct;
	int i, j, k;

	cs=img+(yo*xs+xo); ct=blk;
	memcpy8(ct, cs); ct+=8; cs+=xs;
	memcpy8(ct, cs); ct+=8; cs+=xs;
	memcpy8(ct, cs); ct+=8; cs+=xs;
	memcpy8(ct, cs); ct+=8; cs+=xs;
	memcpy8(ct, cs); ct+=8; cs+=xs;
	memcpy8(ct, cs); ct+=8; cs+=xs;
	memcpy8(ct, cs); ct+=8; cs+=xs;
	memcpy8(ct, cs); // ct+=8; cs+=xs;
}
#endif

#if 1
void BTIC2C_GetImgBlk(s32 *blk, int xo, int yo, s32 *img, int xs, int ys)
{
	s32 *cs;
	s32 *ct;
	int i, j, k;

	cs=img+(yo*xs+xo); ct=blk;
	memcpy(ct, cs, 32); ct+=8; cs+=xs;
	memcpy(ct, cs, 32); ct+=8; cs+=xs;
	memcpy(ct, cs, 32); ct+=8; cs+=xs;
	memcpy(ct, cs, 32); ct+=8; cs+=xs;
	memcpy(ct, cs, 32); ct+=8; cs+=xs;
	memcpy(ct, cs, 32); ct+=8; cs+=xs;
	memcpy(ct, cs, 32); ct+=8; cs+=xs;
	memcpy(ct, cs, 32); // ct+=8; cs+=xs;
}
#endif

#if 1
void BTIC2C_PutImgBlk(s32 *blk, int xo, int yo, s32 *img, int xs, int ys)
{
	s32 *cs;
	s32 *ct;
	int i, j, k;

	cs=img+(yo*xs+xo); ct=blk;
	memcpy(cs, ct, 32); ct+=8; cs+=xs;
	memcpy(cs, ct, 32); ct+=8; cs+=xs;
	memcpy(cs, ct, 32); ct+=8; cs+=xs;
	memcpy(cs, ct, 32); ct+=8; cs+=xs;
	memcpy(cs, ct, 32); ct+=8; cs+=xs;
	memcpy(cs, ct, 32); ct+=8; cs+=xs;
	memcpy(cs, ct, 32); ct+=8; cs+=xs;
	memcpy(cs, ct, 32); // ct+=8; cs+=xs;
}
#endif

#if 1
int BTIC2C_FilterImageDCT(BTIC2C_Context *ctx,
	s32 *ibuf, s32 *obuf, int xs, int ys)
{
//	static short tblk[BTIC2C_DCTSZ2], tblk2[BTIC2C_DCTSZ2];
//	static char blk[BTIC2C_DCTSZ2];
	s32 tblk[BTIC2C_DCTSZ2], tblk2[BTIC2C_DCTSZ2];
	s32 blk[BTIC2C_DCTSZ2];
	int i, j, k, l;

	k=0;
	for(i=0; i<(ys/BTIC2C_DCTSZ); i++)
		for(j=0; j<(xs/BTIC2C_DCTSZ); j++)
	{
		BTIC2C_GetImgBlk(blk, j*BTIC2C_DCTSZ, i*BTIC2C_DCTSZ, ibuf, xs, ys);
		BTIC2C_TransDCT(blk, obuf+k*BTIC2C_DCTSZ2);
		k++;
	}
	return(0);
}
#endif

int BTIC2C_FilterImageWHT(BTIC2C_Context *ctx,
	s32 *ibuf, s32 *obuf, int xs, int ys)
{
//	static short tblk[BTIC2C_DCTSZ2], tblk2[BTIC2C_DCTSZ2];
//	static char blk[BTIC2C_DCTSZ2];
	s32 tblk[BTIC2C_DCTSZ2], tblk2[BTIC2C_DCTSZ2];
	s32 blk[BTIC2C_DCTSZ2];
	int i, j, k, l;

	k=0;
	for(i=0; i<(ys/BTIC2C_DCTSZ); i++)
		for(j=0; j<(xs/BTIC2C_DCTSZ); j++)
	{
		BTIC2C_GetImgBlk(blk, j*BTIC2C_DCTSZ, i*BTIC2C_DCTSZ, ibuf, xs, ys);
		BTIC2C_TransWHT(blk, obuf+k*BTIC2C_DCTSZ2);
		k++;
	}
	return(0);
}

int BTIC2C_FilterImageBLKT(BTIC2C_Context *ctx,
	s32 *ibuf, s32 *obuf, int xs, int ys)
{
	if(ctx->img_blktrans==BTIC2C_BLKT_DCT)
	{
		return(BTIC2C_FilterImageDCT(ctx,
			ibuf, obuf, xs, ys));
	}

	if(ctx->img_blktrans==BTIC2C_BLKT_WHT)
	{
		return(BTIC2C_FilterImageWHT(ctx,
			ibuf, obuf, xs, ys));
	}
	
	return(-1);
}

int BTIC2C_FilterImageDeltaBLKT(BTIC2C_Context *ctx,
	s32 *ibuf, s32 *libuf,
	s32 *obuf, byte *bmbuf, int qid, int xs, int ys)
{
	if(ctx->img_blktrans==BTIC2C_BLKT_DCT)
	{
		return(BTIC2C_FilterImageDeltaDCT(ctx,
			ibuf, libuf, obuf, bmbuf, qid, xs, ys));
	}

	if(ctx->img_blktrans==BTIC2C_BLKT_WHT)
	{
		return(BTIC2C_FilterImageDeltaWHT(ctx,
			ibuf, libuf, obuf, bmbuf, qid, xs, ys));
	}
}

int BTIC2C_FilterImageDeltaWHT(BTIC2C_Context *ctx,
	s32 *ibuf, s32 *libuf,
	s32 *obuf, byte *bmbuf, int qid, int xs, int ys)
{
//	static short tblk[BTIC2C_DCTSZ2], tblk2[BTIC2C_DCTSZ2];
//	static char blk[BTIC2C_DCTSZ2];
	s32 tblk[BTIC2C_DCTSZ2], tblk2[BTIC2C_DCTSZ2];
	s32 blk[BTIC2C_DCTSZ2], lblk[BTIC2C_DCTSZ2];
	s32 tqblk[BTIC2C_DCTSZ2], tqblk2[BTIC2C_DCTSZ2];
	int xi, yi, da, db, ea, eb, ba, bb;
	int i, j, k, l;

	k=0;
	for(i=0; i<(ys/BTIC2C_DCTSZ); i++)
		for(j=0; j<(xs/BTIC2C_DCTSZ); j++)
	{
//		bmbuf[k*4+0]=0;

		bmbuf[k*4+0]=1;
		bmbuf[k*4+1]=0;
		bmbuf[k*4+2]=0;
		bmbuf[k*4+3]=0;
	
		BTIC2C_GetImgBlk(blk, j*BTIC2C_DCTSZ, i*BTIC2C_DCTSZ,
			ibuf, xs, ys);
		BTIC2C_GetImgBlk(lblk, j*BTIC2C_DCTSZ, i*BTIC2C_DCTSZ,
			libuf, xs, ys);
		
		ea=0; eb=0;
		for(yi=0; yi<8; yi++)
			for(xi=0; xi<8; xi++)
		{
			tblk[yi*8+xi]=blk[yi*8+xi]-lblk[yi*8+xi];
//			da=blk[yi*8+xi];
//			db=tblk[yi*8+xi];
//			ea+=da*da;
//			eb+=db*db;
		}

		ba=0; bb=0;
		for(yi=0; yi<8; yi++)
			for(xi=0; xi<8; xi++)
		{
			ba+=blk[yi*8+xi];
			bb+=tblk[yi*8+xi];
		}
		ba/=64; bb/=64;

		for(yi=0; yi<8; yi++)
			for(xi=0; xi<8; xi++)
		{
			da=blk[yi*8+xi]-ba;
			db=tblk[yi*8+xi]-bb;
			ea+=da*da;
			eb+=db*db;
		}

#if 1
		/* Detect and handle skip-blocks */
		BTIC2C_TransWHT(tblk, tblk2);
		BTIC2CH_QuantBlock(ctx, tblk2, tqblk, qid);
		for(l=0; l<64; l++)
			if(tqblk[l])
				break;
		if(l>=64)
		{
			BTIC2C_TransWHT(blk, obuf+k*BTIC2C_DCTSZ2);
			
			/* Skip Blocks only have DC */
			for(l=1; l<64; l++)
				obuf[k*BTIC2C_DCTSZ2+l]=0;

			bmbuf[k*4+0]=BTIC2C_BLKM_SKIP;
			k++;
			continue;
		}
#endif

//		if(eb<ea)
		if(0)
		{
			BTIC2C_TransWHT(tblk, obuf+k*BTIC2C_DCTSZ2);
			bmbuf[k*4+0]=BTIC2C_BLKM_ADD;
		}else
		{
			BTIC2C_TransWHT(blk, obuf+k*BTIC2C_DCTSZ2);
			bmbuf[k*4+0]=BTIC2C_BLKM_REPLACE;
		}
		k++;
	}
	return(0);
}

int BTIC2C_FilterImageDeltaIWHT(BTIC2C_Context *ctx,
	s32 *imgbuf, s32 *limgbuf,
	s32 *blkbuf, byte *bmbuf, int qid, int xs, int ys)
{
//	static short tblk[BTIC2C_DCTSZ2], tblk2[BTIC2C_DCTSZ2];
//	static char blk[BTIC2C_DCTSZ2];
	s32 tblk[BTIC2C_DCTSZ2], tblk2[BTIC2C_DCTSZ2];
	s32 blk[BTIC2C_DCTSZ2], lblk[BTIC2C_DCTSZ2];
	int xi, yi;
	int i, j, k, l;

	k=0;
	for(i=0; i<(ys/BTIC2C_DCTSZ); i++)
		for(j=0; j<(xs/BTIC2C_DCTSZ); j++)
	{
//		bmbuf[k*4+0]=0;

//		bmbuf[k*4+0]=1;
//		bmbuf[k*4+1]=0;
//		bmbuf[k*4+2]=0;
//		bmbuf[k*4+3]=0;

		if(bmbuf[k*4+0]==BTIC2C_BLKM_SKIP)
			{ k++; continue; }

//		BTIC2C_GetImgBlk(blk, j*BTIC2C_DCTSZ, i*BTIC2C_DCTSZ,
//			ibuf, xs, ys);
		BTIC2C_GetImgBlk(lblk, j*BTIC2C_DCTSZ, i*BTIC2C_DCTSZ,
			limgbuf, xs, ys);
		
//		for(yi=0; yi<8; yi++)
//			for(xi=0; xi<8; xi++)
//				{ blk[yi*8+xi]-=lblk[yi*8+xi]; }
		
//		BTIC2C_TransWHT(blk, obuf+k*BTIC2C_DCTSZ2);
		BTIC2C_TransIWHT(blkbuf+k*BTIC2C_DCTSZ2, blk);

		if(bmbuf[k*4+0]==BTIC2C_BLKM_REPLACE)
		{
			BTIC2C_PutImgBlk(blk, j*BTIC2C_DCTSZ, i*BTIC2C_DCTSZ,
				imgbuf, xs, ys);
		}else if(bmbuf[k*4+0]==BTIC2C_BLKM_ADD)
		{
			for(yi=0; yi<8; yi++)
				for(xi=0; xi<8; xi++)
					{ blk[yi*8+xi]+=lblk[yi*8+xi]; }
			BTIC2C_PutImgBlk(blk, j*BTIC2C_DCTSZ, i*BTIC2C_DCTSZ,
				imgbuf, xs, ys);
		}

		k++;
	}
	return(0);
}

void BTIC2C_EmitQuantTabValue(BTIC2C_Context *ctx,
	int val, int *rk)
{
	int i, j, k;

	j=((val>=0)?(val<<1):(((-val)<<1)-1));
//	j=val;

	for(i=0; i<64; i++)
	{
		k=btic2c_lbase[i]+(1<<btic2c_lextra[i]);
		if((j>=btic2c_lbase[i]) && (j<k))
		{
			BTIC2CH_WriteAdSRice(ctx, i, rk);
			BTIC2CH_WriteNBits(ctx,
				j-btic2c_lbase[i],
				btic2c_lextra[i]);
			break;
		}
	}
}

void BTIC2C_EmitQuantTabSz(BTIC2C_Context *ctx,
	u16 *tab, int blen, int *rk)
{
	int i, j, k, l;

	l=0;
	for(i=0; i<blen; i++)
	{
//		j=tab[i];
		j=tab[pdjpg_zigzag2[i]];
		if(j==l)
		{
			for(k=i+1; k<blen; k++)
			{
//				j=tab[k];
				j=tab[pdjpg_zigzag2[k]];
				if(j!=l)break;
			}
			j=k-i;
			
//			printf("%d ", -j);
			BTIC2CH_WriteAdSRice(ctx, -j, rk);
			i=k-1;
			continue;
		}

//		printf("%d(%d) ", j, j-l);
//		BTIC2C_WriteSRice(ctx, j, rk);
		BTIC2C_EmitQuantTabValue(ctx, j-l, rk);
		l=j;
	}
//	BTAC2_Encode_WriteSRice(ctx, 0, rk);
//	printf("\n");
}

void BTIC2C_EmitDQT(BTIC2C_Context *ctx, int n)
{
	byte *ct0;
	int i, j, k;

	ct0=ctx->huff_ct;
	*ctx->huff_ct++=0xE3;
	*ctx->huff_ct++=0x00;
	*ctx->huff_ct++=0x00;
	*ctx->huff_ct++=0x00;
	*ctx->huff_ct++='Q';
	*ctx->huff_ct++='0'+n;

	k=4;
	*ctx->huff_ct++=0x10;
	ctx->huff_win=0;
	ctx->huff_pos=0;
	BTIC2C_EmitQuantTabSz(ctx, ctx->img_qt[n], 64, &k);
	BTIC2CH_FlushBits(ctx);

//	*ctx->huff_ct++=n;
//	for(i=0; i<64; i++)
//		*ctx->huff_ct++=ctx->img_qt[n][pdjpg_zigzag2[i]];
		
	i=ctx->huff_ct-ct0;
	ct0[1]=(i>>16)&255;
	ct0[2]=(i>> 8)&255;
	ct0[3]=(i>> 0)&255;
}

void BTIC2C_EmitSOF(BTIC2C_Context *ctx, int xs, int ys)
{
	byte *ct0;
	int i, ns, bts, clrs;

	bts=8;
	clrs=ctx->clrs;
	switch(clrs)
	{
	case BGBBTJ_JPG_RGBA_Q11_4:
	case BGBBTJ_JPG_RGBA_F32:
	case BGBBTJ_JPG_RGBA_F16:
	case BGBBTJ_JPG_RGBA_LS16:
		bts=16; break;
	case BGBBTJ_JPG_RGB_Q11_4:
	case BGBBTJ_JPG_RGB_F32:
	case BGBBTJ_JPG_RGB_F16:
	case BGBBTJ_JPG_RGB_LS16:
		bts=16; break;
	default:
		break;
	}

	ct0=ctx->huff_ct;
	*ctx->huff_ct++=0xE3;
	*ctx->huff_ct++=0x00;
	*ctx->huff_ct++=0x00;
	*ctx->huff_ct++=0x00;
	*ctx->huff_ct++='S';
	*ctx->huff_ct++='F';

//	i=8+(ctx->img_mono?1:3)*3;
//	*ctx->huff_ct++=i>>8;	//Lf
//	*ctx->huff_ct++=i&0xFF;

	*ctx->huff_ct++=8; 			//P (bits)
	*ctx->huff_ct++=ctx->clrs;	//ClrS

	*ctx->huff_ct++=ys>>8;		//Y
	*ctx->huff_ct++=ys&0xFF;	//Y
	*ctx->huff_ct++=xs>>8;		//X
	*ctx->huff_ct++=xs&0xFF;	//X

	if(ctx->img_mono)
	{
		ns=3;
		if(ctx->img_alpha)ns++;

		*ctx->huff_ct++=ns;		//Nf

		*ctx->huff_ct++=1;		//Ci
		*ctx->huff_ct++=0x11;	//Hi Vi
		*ctx->huff_ct++=0;		//Tqi
		if(ctx->img_alpha)
		{
			*ctx->huff_ct++=4;		//Ci
			*ctx->huff_ct++=0x11;	//Hi Vi
			*ctx->huff_ct++=2;		//Tqi
		}
	}else
	{
		ns=3;
		if(ctx->img_alpha)ns++;
	
		*ctx->huff_ct++=ns;		//Nf

		*ctx->huff_ct++=1;		//Ci
		*ctx->huff_ct++=0x22;	//Hi Vi
		*ctx->huff_ct++=0;		//Tqi
		*ctx->huff_ct++=2;		//Ci
		*ctx->huff_ct++=0x11;	//Hi Vi
		*ctx->huff_ct++=1;		//Tqi
		*ctx->huff_ct++=3;		//Ci
		*ctx->huff_ct++=0x11;	//Hi Vi
		*ctx->huff_ct++=1;		//Tqi
		if(ctx->img_alpha)
		{
			*ctx->huff_ct++=4;		//Ci
			*ctx->huff_ct++=0x11;	//Hi Vi
			*ctx->huff_ct++=2;		//Tqi
		}
	}
	
	*ctx->huff_ct++=0x01;					//Tag
	*ctx->huff_ct++=0x02;					//Length
	*ctx->huff_ct++=ctx->img_clrtrans;		//Color Transform
	*ctx->huff_ct++=ctx->img_blktrans;		//Block Transform

	i=ctx->huff_ct-ct0;
	ct0[1]=(i>>16)&255;
	ct0[2]=(i>> 8)&255;
	ct0[3]=(i>> 0)&255;
}

void BTIC2C_EmitSOS(BTIC2C_Context *ctx)
{
	byte *ct0;
	int i, ns;

	ct0=ctx->huff_ct;
	*ctx->huff_ct++=0xE3;
	*ctx->huff_ct++=0x00;
	*ctx->huff_ct++=0x00;
	*ctx->huff_ct++=0x00;
	*ctx->huff_ct++='S';
	*ctx->huff_ct++='S';

	if(!ctx->img_mono)
	{
		ns=3;
		if(ctx->img_alpha)ns++;
	
		*ctx->huff_ct++=ns; 	//Ns

#if 0
		*ctx->huff_ct++=1;		//Csi
		*ctx->huff_ct++=0x01;	//Tdi Tai
		*ctx->huff_ct++=2;		//Csi
		*ctx->huff_ct++=0x23;	//Tdi Tai
		*ctx->huff_ct++=3;		//Csi
		*ctx->huff_ct++=0x23;	//Tdi Tai
		if(ctx->img_alpha)
		{
			*ctx->huff_ct++=4;		//Csi
			*ctx->huff_ct++=0x45;	//Tdi Tai
		}
#endif

#if 1
		*ctx->huff_ct++=16|1;	//Csi
		*ctx->huff_ct++=0x01;	//Tdi Tai
		*ctx->huff_ct++=0x06;	//Td2i Ta2i
		*ctx->huff_ct++=16|2;	//Csi
		*ctx->huff_ct++=0x23;	//Tdi Tai
		*ctx->huff_ct++=0x27;	//Td2i Ta2i
		*ctx->huff_ct++=16|3;	//Csi
		*ctx->huff_ct++=0x23;	//Tdi Tai
		*ctx->huff_ct++=0x27;	//Td2i Ta2i
		if(ctx->img_alpha)
		{
			*ctx->huff_ct++=16|4;	//Csi
			*ctx->huff_ct++=0x45;	//Tdi Tai
			*ctx->huff_ct++=0x48;	//Td2i Ta2i
		}
#endif
	}else
	{
		ns=1;
		if(ctx->img_alpha)ns++;

		*ctx->huff_ct++=ns; 	//Ns

//		*ctx->huff_ct++=1;		//Csi
//		*ctx->huff_ct++=0x01;	//Tdi Tai

		*ctx->huff_ct++=16|1;	//Csi
		*ctx->huff_ct++=0x01;	//Tdi Tai
		*ctx->huff_ct++=0x06;	//Tdi Tai

		if(ctx->img_alpha)
		{
//			*ctx->huff_ct++=4;		//Csi
//			*ctx->huff_ct++=0x45;	//Tdi Tai

			*ctx->huff_ct++=16|4;	//Csi
			*ctx->huff_ct++=0x45;	//Tdi Tai
			*ctx->huff_ct++=0x48;	//Tdi Tai
		}
	}

	*ctx->huff_ct++=0; 		//Ss
	*ctx->huff_ct++=63; 	//Se
	*ctx->huff_ct++=0x00; 	//Ah Al

	i=ctx->huff_ct-ct0;
	ct0[1]=(i>>16)&255;
	ct0[2]=(i>> 8)&255;
	ct0[3]=(i>> 0)&255;
}

#if 1
static int btic2c_lorder[]={
	16, 17, 18, 0, 8,7, 9,6, 10,5, 11,4, 12,3, 13,2, 14,1, 15, 19};
static int btic2c_lorder2[]={
	3, 17, 15, 13, 11,9, 7,5, 4,6, 8,10, 12,14, 16,18, 0,1, 2, 19};

void BTIC2C_EncodeHufftabSymbol(BTIC2C_Context *ctx,
	int v, int *rk)
{
	BTIC2CH_WriteAdRice(ctx, btic2c_lorder2[v], rk);
//	BTIC2CH_WriteRice(ctx, btic2c_lorder2[v], 2);
}

void BTIC2C_EncodeCodeLengths(BTIC2C_Context *ctx,
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
			BTIC2C_EncodeHufftabSymbol(ctx, 18, rk);
			BTIC2CH_WriteNBits(ctx, j-11, 7);
			i+=j;
			continue;
		}
		if(j>2)
		{
			BTIC2C_EncodeHufftabSymbol(ctx, 17, rk);
			BTIC2CH_Write3Bits(ctx, j-3);
			i+=j;
			continue;
		}

		l=cl[i++];
		BTIC2C_EncodeHufftabSymbol(ctx, l, rk);

		if((l<0) || (l>16))printf("problem: bad code length noted\n");
	}
	BTIC2C_EncodeHufftabSymbol(ctx, 19, rk);
//	printf("\n");
}

int BTIC2C_EncodeHuffTable(BTIC2C_Context *ctx,
	int tab, byte *cl, int nc)
{
	int i, j, rk, lc;

	BTIC2CH_WriteNBits(ctx, 1, 4);		//tsz
	BTIC2CH_WriteNBits(ctx, 0, 4);		//rice factor
	
	rk=4;
	BTIC2C_EncodeCodeLengths(ctx, cl, nc, &rk);

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

void BTIC2C_EmitDHT(BTIC2C_Context *ctx, int tab)
{
	byte *ct0;
	int i, j, k, t;

	ct0=ctx->huff_ct;
	*ctx->huff_ct++=0xE3;
	*ctx->huff_ct++=0x00;
	*ctx->huff_ct++=0x00;
	*ctx->huff_ct++=0x00;
	*ctx->huff_ct++='H';
	*ctx->huff_ct++='0'+tab;

#if 1
	t=tab<<8;

	ctx->huff_win=0;
	ctx->huff_pos=0;

//	*ctx->huff_ct++=0x10;	//TtTb
	BTIC2C_EncodeHuffTable(ctx, tab, ctx->huff_len+t, 256);
	BTIC2CH_FlushBits(ctx);

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
		printf("BTIC2C_EmitDHT: Error %04X\n", k);
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
		printf("BTIC2C_EmitDHT: Error %04X\n", k);
#endif

	i=ctx->huff_ct-ct0;
	ct0[1]=(i>>16)&255;
	ct0[2]=(i>> 8)&255;
	ct0[3]=(i>> 0)&255;
}

void BTIC2C_MakeQuantTabInput(s32 *in, int cnt, u16 *tab, float q)
{
//	static double deltas[BTIC2C_DCTSZ2];
	double deltas[BTIC2C_DCTSZ2];
	double f, g, te;
	int i, j, k;

	for(j=0; j<BTIC2C_DCTSZ2; j++)
		deltas[j]=0;

	for(i=0; i<cnt; i++)
		for(j=0; j<BTIC2C_DCTSZ2; j++)
			deltas[j]+=fabs(in[i*BTIC2C_DCTSZ2+j]);
	for(j=0; j<BTIC2C_DCTSZ2; j++)
		deltas[j]/=cnt;

	te=0;
	for(j=0; j<BTIC2C_DCTSZ2; j++)
		te+=deltas[j];

	for(i=0; i<BTIC2C_DCTSZ; i++)
	{
		for(j=0; j<BTIC2C_DCTSZ; j++)
		{
			f=deltas[i*BTIC2C_DCTSZ+j];

			f=f/te;
			if(f<0.00001)f=0.00001;
			g=(1.0-q);
			f=10*g*g*g/f;
			k=f;
			if(k<1)k=1;
			if(k>255)k=255;

//			printf("%f/%f %d\n", deltas[i*BTIC2C_DCTSZ+j], te, k);

//			if(f<1)f=1;
//			if(f>255)f=255;
			tab[i*BTIC2C_DCTSZ+j]=k;
		}
	}

#if 1
	if(tab[BTIC2C_DCTSZ2-3]>1)tab[BTIC2C_DCTSZ2-3]*=0.75;
	if(tab[BTIC2C_DCTSZ2-2]>1)tab[BTIC2C_DCTSZ2-2]*=0.75;
	if(tab[BTIC2C_DCTSZ2-1]>1)tab[BTIC2C_DCTSZ2-1]*=0.5;
#endif
}

void BTIC2C_MakeQuantTabInputFast(
	s32 *in, int cnt, u16 *tab, float q)
{
	int i;
//	BTIC2C_MakeQuantTabInput(in, cnt, tab, q);

	i=cnt/16;
	if(i<1)i=1;
	BTIC2C_MakeQuantTabInput(in, i, tab, q);
}

static int pdjpg_ijg_qtab_y[64] = {
16, 11, 10, 16,  24,  40,  51,  61,
12, 12, 14, 19,  26,  58,  60,  55,
14, 13, 16, 24,  40,  57,  69,  56,
14, 17, 22, 29,  51,  87,  80,  62,
18, 22, 37, 56,  68, 109, 103,  77,
24, 35, 55, 64,  81, 104, 113,  92,
49, 64, 78, 87, 103, 121, 120, 101,
72, 92, 95, 98, 112, 100, 103,  99};

static int pdjpg_ijg_qtab_uv[64] = {
99, 99, 99, 99, 99, 99, 99, 99,
99, 99, 99, 99, 99, 99, 99, 99,
99, 99, 99, 99, 99, 99, 99, 99,
99, 99, 99, 99, 99, 99, 99, 99,
47, 66, 99, 99, 99, 99, 99, 99,
24, 26, 56, 99, 99, 99, 99, 99,
18, 21, 26, 66, 99, 99, 99, 99,
17, 18, 24, 47, 99, 99, 99, 99};

void BTIC2C_MakeQuantTabFastIJG_Y(
	s32 *inv, int cnt, u16 *tab, float qf)
{
	double s, q;
	int i, j;

	q = (qf * 100);
	s = (q < 50) ? 5000 / q : (200 - 2 * q);
	for (i = 0; i < 64; i++)
	{
		j = (int)((s * pdjpg_ijg_qtab_y[i] + 50) / 100);
		j = (j < 1) ? 1 : ((j < 256) ? j : 255);
		tab[i] = j;
	}
}

void BTIC2C_MakeQuantTabFastIJG_UV(
	s32 *inv, int cnt, u16 *tab, float qf)
{
	double s, q;
	int i, j;

	q = (qf * 100);
	s = (q < 50) ? 5000 / q : (200 - 2 * q);
	for (i = 0; i < 64; i++)
	{
		j = (int)((s * pdjpg_ijg_qtab_y[i] + 50) / 100);
		j = (j < 1) ? 1 : ((j < 256) ? j : 255);
		tab[i] = j;
	}
}

int BTIC2C_ColorTransform(
	BTIC2C_Context *ctx, byte *ibuf,
	int xs, int ys, int qf, int pf)
{
	int xs2, ys2, xs3, ys3;
	int cr, cg, cb, cy, cu, cv, ca;
	int i, j, k, l;

	xs2=ctx->xs2;	ys2=ctx->ys2;
	xs3=ctx->xs3;	ys3=ctx->ys3;

	for(i=0; i<ys; i++)
		for(j=0; j<xs; j++)
	{
		k=ys-(i+1);
		switch(pf)
		{
		case BGBBTJ_JPG_RGBA:
			l=(k*xs+j)*4;
			cr=ibuf[l+0];	cg=ibuf[l+1];
			cb=ibuf[l+2];	ca=ibuf[l+3];
			if(ca<224) { ctx->img_alpha=1; }
			break;
		case BGBBTJ_JPG_RGB:
			l=(k*xs+j)*3;
			cr=ibuf[l+0];	cg=ibuf[l+1];
			cb=ibuf[l+2];	ca=255;
			break;
		case BGBBTJ_JPG_BGRA:
			l=(k*xs+j)*4;
			cr=ibuf[l+2];	cg=ibuf[l+1];
			cb=ibuf[l+0];	ca=ibuf[l+3];
			if(ca<224) { ctx->img_alpha=1; }
			break;
		case BGBBTJ_JPG_BGR:
			l=(k*xs+j)*3;
			cr=ibuf[l+2];	cg=ibuf[l+1];
			cb=ibuf[l+0];	ca=255;
			break;
		case BGBBTJ_JPG_RGBA_Q11_4:
			l=(k*xs+j)*4;
			cr=((s16 *)ibuf)[l+2];	cg=((s16 *)ibuf)[l+1];
			cb=((s16 *)ibuf)[l+0];	ca=((s16 *)ibuf)[l+3];
			if((ca<3584) || (ca>4096))
				{ ctx->img_alpha=1; }
			break;
		case BGBBTJ_JPG_RGBA_F16:
			l=(k*xs+j)*4;
			cr=((s16 *)ibuf)[l+2];	cg=((s16 *)ibuf)[l+1];
			cb=((s16 *)ibuf)[l+0];	ca=((s16 *)ibuf)[l+3];
			if((ca<0x3B00) || (ca>0x3C00))
				{ ctx->img_alpha=1; }
			break;
		case BGBBTJ_JPG_RGBA_LS16:
			l=(k*xs+j)*4;
			cr=((s16 *)ibuf)[l+2];	cg=((s16 *)ibuf)[l+1];
			cb=((s16 *)ibuf)[l+0];	ca=((s16 *)ibuf)[l+3];
			if((ca<224) || (ca>256)) { ctx->img_alpha=1; }
			break;
//		case BGBBTJ_JPG_RGB_Q11_4:
//		case BGBBTJ_JPG_RGB_F16:
//		case BGBBTJ_JPG_RGB_LS16:
//			l=(k*xs+j)*3;
//			cr=((s16 *)ibuf)[l+2];	cg=((s16 *)ibuf)[l+1];
//			cb=((s16 *)ibuf)[l+0];	ca=0x3C00;
//			break;
		}

//		cy=0.299*cr	+0.587*cg	+0.114*cb;
//		cu=-0.1687*cr	-0.3313*cg	+0.5*cb 	+128;
//		cv=0.5*cr	-0.4187*cg	-0.0813*cb	+128;

		if(ctx->img_clrtrans==BTIC2C_CLRS_RCT)
		{
			cy=(cr+(2*cg)+cb)>>2;		//Luma
			cu=cb-cg;
			cv=cr-cg;
		}else if(ctx->img_clrtrans==BTIC2C_CLRS_YCGCO)
		{
			cy=(cr+(2*cg)+cb)>>2;		//Luma
			cu=(2*cg-cr-cb)>>1;
			cv=cb-cr;
		}else
		{
			cy= 0.2990*cr +0.5870*cg +0.1140*cb;
			cu=-0.1687*cr -0.3313*cg +0.5000*cb;
			cv= 0.5000*cr -0.4187*cg -0.0813*cb;
		}

		
//		if(ca<224) { ctx->img_alpha=1; }

//		cu=128; cv=128;

//		cy=(cy<0)?0:((cy>255)?255:cy);
//		cu=(cu<0)?0:((cu>255)?255:cu);
//		cv=(cv<0)?0:((cv>255)?255:cv);

		ctx->yb[i*xs2+j]=cy;
		ctx->ub[i*xs2+j]=cu;
		ctx->vb[i*xs2+j]=cv;
		ctx->ab[i*xs2+j]=ca;
	}

	for(i=0; i<ys3; i++)
		for(j=0; j<xs3; j++)
	{
		k=ctx->ub[(i*2)*xs2+j*2]+
			ctx->ub[(i*2)*xs2+j*2+1]+
			ctx->ub[(i*2+1)*xs2+j*2]+
			ctx->ub[(i*2+1)*xs2+j*2+1];
		ctx->ub[i*xs3+j]=k/4;

		k=ctx->vb[(i*2)*xs2+j*2]+
			ctx->vb[(i*2)*xs2+j*2+1]+
			ctx->vb[(i*2+1)*xs2+j*2]+
			ctx->vb[(i*2+1)*xs2+j*2+1];
		ctx->vb[i*xs3+j]=k/4;
	}

	if(ctx->img_alpha)
	{
		for(i=0; i<ys3; i++)
			for(j=0; j<xs3; j++)
		{
			k=ctx->ab[(i*2)*xs2+j*2]+
				ctx->ab[(i*2)*xs2+j*2+1]+
				ctx->ab[(i*2+1)*xs2+j*2]+
				ctx->ab[(i*2+1)*xs2+j*2+1];
			ctx->ab[i*xs3+j]=k/4;
		}
	}
	return(0);
}

int BTIC2C_EncodeCtx(
	BTIC2C_Context *ctx, byte *ibuf, byte *obuf, int osz,
	int xs, int ys, int qf, int pf)
{
//	int dcs[256], acs[256];
//	int dcsuv[256], acsuv[256];
	byte *ct0;
	s32 *tp;
	int xs2, ys2, xs3, ys3, wmb, hmb;
	int cr, cg, cb, cy, cu, cv, ca;
	int ly, lu, lv, la;
	int skr, skrs;
	int i, j, k, l;

//	qf&=~BGBBTJ_QFL_PFRAME;

	xs2=((xs+7)/8)*8;
	ys2=((ys+7)/8)*8;

	xs3=((xs+15)/16)*8;
	ys3=((ys+15)/16)*8;
	
	wmb=((xs+15)/16);
	hmb=((ys+15)/16);

	ctx->img_mono=0;
	ctx->img_alpha=0;
	ctx->img_tabcacheframe=0;
	ctx->xs=xs;
	ctx->ys=ys;
	ctx->xs2=xs2;
	ctx->ys2=ys2;
	ctx->xs3=xs3;
	ctx->ys3=ys3;
	ctx->wmb=wmb;
	ctx->hmb=hmb;
	
	ctx->qfl=qf;
	ctx->clrs=pf;

	if(pf==BGBBTJ_JPG_YYYA)
		ctx->img_mono=1;

	ctx->img_clrtrans=BTIC2C_CLRS_YCGCO;
//	ctx->img_clrtrans=BTIC2C_CLRS_RCT;
	ctx->img_blktrans=BTIC2C_BLKT_WHT;

#if 0
	if(!(qf&BGBBTJ_QFL_PFRAME))
	{
		//full quality
		for(i=0; i<64; i++)ctx->img_qt[0][i]=1;
		for(i=0; i<64; i++)ctx->img_qt[1][i]=1;

		//dummy huffman tables
		for(i=0; i<256; i++)ctx->huff_len[0*256+i]=0;
		for(i=0; i<256; i++)ctx->huff_len[1*256+i]=0;
		for(i=0; i<256; i++)ctx->huff_len[2*256+i]=0;
		for(i=0; i<256; i++)ctx->huff_len[3*256+i]=0;

		for(i=0; i<16; i++)ctx->huff_len[0*256+i]=4;
		for(i=0; i<16; i++)ctx->huff_len[2*256+i]=4;
		for(i=0; i<256; i++)ctx->huff_len[1*256+i]=8;
		for(i=0; i<256; i++)ctx->huff_len[3*256+i]=8;

//		for(i=0; i<255; i++)ctx->huff_len[0*256+i]=8;
//		for(i=0; i<255; i++)ctx->huff_len[2*256+i]=8;
	}
#endif

//	ctx->huff_len[1*256+0]=7;

//	printf("M0\n");

	if(!ctx->yb || (xs!=ctx->lxs) || (ys!=ctx->lys))
	{
		if(ctx->yb)
		{
			free(ctx->yb);
			free(ctx->ub);
			free(ctx->vb);
			free(ctx->ab);
			free(ctx->ydb);
			free(ctx->udb);
			free(ctx->vdb);
			free(ctx->adb);
		}

		if(ctx->lyb)
		{
			free(ctx->lyb);
			free(ctx->lub);
			free(ctx->lvb);
			free(ctx->lab);
		}
		
		if(ctx->skip_map)
		{
			free(ctx->skip_map);
			ctx->skip_map=NULL;
		}

		ctx->yb=malloc(xs2*ys2*sizeof(s32));
		ctx->ub=malloc(xs2*ys2*sizeof(s32));
		ctx->vb=malloc(xs2*ys2*sizeof(s32));
		ctx->ab=malloc(xs2*ys2*sizeof(s32));

		ctx->lyb=malloc(xs2*ys2*sizeof(s32));
		ctx->lub=malloc(xs2*ys2*sizeof(s32));
		ctx->lvb=malloc(xs2*ys2*sizeof(s32));
		ctx->lab=malloc(xs2*ys2*sizeof(s32));

		ctx->ydb=malloc((xs2+8)*(ys2+8)*sizeof(s32));
		ctx->udb=malloc((xs3+8)*(ys3+8)*sizeof(s32));
		ctx->vdb=malloc((xs3+8)*(ys3+8)*sizeof(s32));
		ctx->adb=malloc((xs3+8)*(ys3+8)*sizeof(s32));

		ctx->lxs=xs;
		ctx->lys=ys;

		memset(ctx->yb, 0, xs2*ys2*sizeof(s32));
		memset(ctx->ub, 0, xs2*ys2*sizeof(s32));
		memset(ctx->vb, 0, xs2*ys2*sizeof(s32));
		memset(ctx->ab, 0, xs2*ys2*sizeof(s32));

		memset(ctx->lyb, 0, xs2*ys2*sizeof(s32));
		memset(ctx->lub, 0, xs2*ys2*sizeof(s32));
		memset(ctx->lvb, 0, xs2*ys2*sizeof(s32));
		memset(ctx->lab, 0, xs2*ys2*sizeof(s32));

		memset(ctx->ydb, 0, xs2*(ys2+8)*sizeof(s32));
		memset(ctx->udb, 0, xs3*(ys3+8)*sizeof(s32));
		memset(ctx->vdb, 0, xs3*(ys3+8)*sizeof(s32));
		memset(ctx->adb, 0, xs3*(ys3+8)*sizeof(s32));
	}

//	if(!(qf&BGBBTJ_QFL_PFRAME))
	if(1)
	{
		if(!ctx->ybm)
		{
			i=(xs2/8)*(ys2/8);
			j=(xs3/8)*(ys3/8);
			ctx->ybm=malloc(i*4);
			ctx->ubm=malloc(j*4);
			ctx->vbm=malloc(j*4);
			ctx->abm=malloc(j*4);
		}

//		memcpy(ctx->lyb, ctx->yb, xs2*ys2*sizeof(s32));
//		memcpy(ctx->lub, ctx->ub, xs3*ys3*sizeof(s32));
//		memcpy(ctx->lvb, ctx->vb, xs3*ys3*sizeof(s32));
//		memcpy(ctx->lab, ctx->ab, xs3*ys3*sizeof(s32));

		i=wmb*hmb;
		j=(i+7)/8;
		if(!ctx->skip_map)
			{ ctx->skip_map=malloc(j); }
		memset(ctx->skip_map, 255, j);

		i=(xs2/8)*(ys2/8);
		j=(xs3/8)*(ys3/8);
		memset(ctx->ybm, 0, i*4);
		memset(ctx->ubm, 0, j*4);
		memset(ctx->vbm, 0, j*4);
		memset(ctx->abm, 0, j*4);
	}

	BTIC2C_ColorTransform(ctx, ibuf, xs, ys, qf, pf);

//	printf("M1\n");

	if(!(qf&BGBBTJ_QFL_PFRAME))
	{
		BTIC2C_FilterImageBLKT(ctx, ctx->yb, ctx->ydb, xs2, ys2);
		BTIC2C_FilterImageBLKT(ctx, ctx->ub, ctx->udb, xs3, ys3);
		BTIC2C_FilterImageBLKT(ctx, ctx->vb, ctx->vdb, xs3, ys3);
		if(ctx->img_alpha)
			{ BTIC2C_FilterImageBLKT(ctx, ctx->ab, ctx->adb, xs3, ys3); }

//		BTIC2C_FilterImageWHT(ctx, ctx->yb, ctx->ydb, xs2, ys2);
//		BTIC2C_FilterImageWHT(ctx, ctx->ub, ctx->udb, xs3, ys3);
//		BTIC2C_FilterImageWHT(ctx, ctx->vb, ctx->vdb, xs3, ys3);
//		if(ctx->img_alpha)
//			{ BTIC2C_FilterImageWHT(ctx, ctx->ab, ctx->adb, xs3, ys3); }
	}else
	{
		BTIC2C_FilterImageDeltaBLKT(ctx, ctx->yb, ctx->lyb,
			ctx->ydb, ctx->ybm, 0, xs2, ys2);
		BTIC2C_FilterImageDeltaBLKT(ctx, ctx->ub, ctx->lub,
			ctx->udb, ctx->ubm, 1, xs3, ys3);
		BTIC2C_FilterImageDeltaBLKT(ctx, ctx->vb, ctx->lvb,
			ctx->vdb, ctx->vbm, 1, xs3, ys3);
		if(ctx->img_alpha)
		{
			BTIC2C_FilterImageDeltaBLKT(ctx, ctx->ab, ctx->lab,
				ctx->adb, ctx->abm, 2, xs3, ys3);
		}
	}

	if(!(qf&BGBBTJ_QFL_PFRAME))
	{
		j=(xs2/8)*(ys2/8);
//		BTIC2C_MakeQuantTabInput(ctx->ydb, j,
//			ctx->img_qt[0], (qf&255)/100.0);
		BTIC2C_MakeQuantTabFastIJG_Y(ctx->ydb, j,
			ctx->img_qt[0], (qf&255)/100.0);

		j=(xs3/8)*(ys3/8);
//		BTIC2C_MakeQuantTabInput(ctx->udb, j,
//			ctx->img_qt[1], (qf&255)/100.0);
//		BTIC2C_MakeQuantTabInput(ctx->vdb, j,
//			ctx->img_qt[2], (qf&255)/100.0);
		BTIC2C_MakeQuantTabFastIJG_UV(ctx->udb, j,
			ctx->img_qt[1], (qf&255)/100.0);
		BTIC2C_MakeQuantTabFastIJG_UV(ctx->vdb, j,
			ctx->img_qt[2], (qf&255)/100.0);

		for(i=0; i<64; i++)
		{
			ctx->img_qt[1][i]=
				(ctx->img_qt[1][i]+ctx->img_qt[2][i])/2;
		}

		BTIC2C_SetupQuantTabDivFP(ctx, 0);
		BTIC2C_SetupQuantTabDivFP(ctx, 1);

		if(ctx->img_alpha)
		{
			j=(xs3/8)*(ys3/8);
//			BTIC2C_MakeQuantTabInput(ctx->adb, j,
//				ctx->img_qt[2], (qf&255)/100.0);
			BTIC2C_MakeQuantTabFastIJG_Y(ctx->adb, j,
				ctx->img_qt[2], (qf&255)/100.0);

			BTIC2C_SetupQuantTabDivFP(ctx, 2);
		}
	}

#if 1
	if(1)
	{
		ly=0; lu=0; lv=0; la=0;
		for(i=0; i<(ys3/8); i++)
			for(j=0; j<(xs3/8); j++)
		{
			skrs=1;
			cy=ly; cu=lu; cv=lv; ca=la;
			
			k=((i*2+0)*(xs2/8)+j*2+0);
			tp=ctx->ydb+k*64;
			skr=BTIC2CH_QuantBlock(ctx, tp, tp, 0);
			if(ctx->ybm[k*4+0]==BTIC2C_BLKM_SKIP)
				{ tp[0]=ly; }
			tp[0]-=ly; ly=tp[0]+ly;
//			if((ctx->ybm[k*4+0]!=BTIC2C_BLKM_SKIP) && (ly || tp[0] || !skr))
			if(ctx->ybm[k*4+0]!=BTIC2C_BLKM_SKIP)
				{ skrs=0; }

			k=((i*2+0)*(xs2/8)+j*2+1);
			tp=ctx->ydb+k*64;
			skr=BTIC2CH_QuantBlock(ctx, tp, tp, 0);
			if(ctx->ybm[k*4+0]==BTIC2C_BLKM_SKIP)
				{ tp[0]=ly; }
			tp[0]-=ly; ly=tp[0]+ly;
//			if((ctx->ybm[k*4+0]!=BTIC2C_BLKM_SKIP) && (ly || tp[0] || !skr))
			if(ctx->ybm[k*4+0]!=BTIC2C_BLKM_SKIP)
				{ skrs=0; }

			k=((i*2+1)*(xs2/8)+j*2+0);
			tp=ctx->ydb+k*64;
			skr=BTIC2CH_QuantBlock(ctx, tp, tp, 0);
			if(ctx->ybm[k*4+0]==BTIC2C_BLKM_SKIP)
				{ tp[0]=ly; }
			tp[0]-=ly; ly=tp[0]+ly;
//			if((ctx->ybm[k*4+0]!=BTIC2C_BLKM_SKIP) && (ly || tp[0] || !skr))
			if(ctx->ybm[k*4+0]!=BTIC2C_BLKM_SKIP)
				{ skrs=0; }

			k=((i*2+1)*(xs2/8)+j*2+1);
			tp=ctx->ydb+k*64;
			skr=BTIC2CH_QuantBlock(ctx, tp, tp, 0);
			if(ctx->ybm[k*4+0]==BTIC2C_BLKM_SKIP)
				{ tp[0]=ly; }
			tp[0]-=ly; ly=tp[0]+ly;
//			if((ctx->ybm[k*4+0]!=BTIC2C_BLKM_SKIP) && (ly || tp[0] || !skr))
			if(ctx->ybm[k*4+0]!=BTIC2C_BLKM_SKIP)
				{ skrs=0; }


			k=(i*(xs3/8)+j);
			tp=ctx->udb+k*64;
			skr=BTIC2CH_QuantBlock(ctx, tp, tp, 1);
			if(ctx->ubm[k*4+0]==BTIC2C_BLKM_SKIP)
				{ tp[0]=lu; }
			tp[0]-=lu; lu=tp[0]+lu;
//			if((ctx->ubm[k*4+0]!=BTIC2C_BLKM_SKIP) && (lu || tp[0] || !skr))
			if(ctx->ubm[k*4+0]!=BTIC2C_BLKM_SKIP)
				{ skrs=0; }

			k=(i*(xs3/8)+j);
			tp=ctx->vdb+k*64;
			skr=BTIC2CH_QuantBlock(ctx, tp, tp, 1);
			if(ctx->vbm[k*4+0]==BTIC2C_BLKM_SKIP)
				{ tp[0]=lv; }
			tp[0]-=lv; lv=tp[0]+lv;
//			if((ctx->vbm[k*4+0]!=BTIC2C_BLKM_SKIP) && (lv || tp[0] || !skr))
			if(ctx->vbm[k*4+0]!=BTIC2C_BLKM_SKIP)
				{ skrs=0; }

			if(ctx->img_alpha)
			{
				k=(i*(xs3/8)+j);
				tp=ctx->adb+k*64;
				skr=BTIC2CH_QuantBlock(ctx, tp, tp, 2);
				if(ctx->abm[k*4+0]==BTIC2C_BLKM_SKIP)
					{ tp[0]=la; }
				tp[0]-=la; la=tp[0]+la;
//				if((ctx->abm[k*4+0]!=BTIC2C_BLKM_SKIP) &&
//						(la || tp[0] || !skr))
				if(ctx->abm[k*4+0]!=BTIC2C_BLKM_SKIP)
					{ skrs=0; }
			}

//			skrs=0;

			if(!skrs)
			{
				k=(i*(xs3/8)+j);
				ctx->skip_map[k>>3]&=~(1<<(k&7));
			}else
			{
				ly=cy;	lu=cu;
				lv=cv;	la=ca;
			}
		}
	}
#endif

#if 0
	if(ctx->img_mono)
	{
		j=(xs2/8)*(ys2/8); k=0;
		for(i=0; i<j; i++)
		{
			tp=ctx->ydb+i*64;
			skrs=BTIC2CH_QuantBlock(ctx, tp, tp, 0);
			ctx->ydb[i*64+0]-=k; k=ctx->ydb[i*64+0]+k;
			if((ctx->ybm[i*4+0]!=BTIC2C_BLKM_SKIP) && (k || tp[0] || !skrs))
				{ ctx->skip_map[i>>3]&=~(1<<(i&7)); }
		}
	}else
	{
		l=0;
		for(i=0; i<(ys3/8); i++)
			for(j=0; j<(xs3/8); j++)
		{
			skrs=1;
			
			k=((i*2+0)*(xs2/8)+j*2+0);
			tp=ctx->ydb+k*64;
			skr=BTIC2CH_QuantBlock(ctx, tp, tp, 0);
			tp[0]-=l; l=tp[0]+l;
			if((ctx->ybm[k*4+0]!=BTIC2C_BLKM_SKIP) && (l || tp[0] || !skr))
				{ skrs=0; }

			k=((i*2+0)*(xs2/8)+j*2+1);
			tp=ctx->ydb+k*64;
			skr=BTIC2CH_QuantBlock(ctx, tp, tp, 0);
			tp[0]-=l; l=tp[0]+l;
			if((ctx->ybm[k*4+0]!=BTIC2C_BLKM_SKIP) && (l || tp[0] || !skr))
				{ skrs=0; }

			k=((i*2+1)*(xs2/8)+j*2+0);
			tp=ctx->ydb+k*64;
			skr=BTIC2CH_QuantBlock(ctx, tp, tp, 0);
			tp[0]-=l; l=tp[0]+l;
			if((ctx->ybm[k*4+0]!=BTIC2C_BLKM_SKIP) && (l || tp[0] || !skr))
				{ skrs=0; }

			k=((i*2+1)*(xs2/8)+j*2+1);
			tp=ctx->ydb+k*64;
			skr=BTIC2CH_QuantBlock(ctx, tp, tp, 0);
			tp[0]-=l; l=tp[0]+l;
			if((ctx->ybm[k*4+0]!=BTIC2C_BLKM_SKIP) && (l || tp[0] || !skr))
				{ skrs=0; }

			skrs=0;

			if(!skrs)
			{
				k=(i*(xs3/8)+j);
				ctx->skip_map[k>>3]&=~(1<<(k&7));
			}
		}
	}

	j=(xs3/8)*(ys3/8); k=0; l=0;
	for(i=0; i<j; i++)
	{
		skrs=1;

		skr=BTIC2CH_QuantBlock(ctx, ctx->udb+i*64, ctx->udb+i*64, 1);
		ctx->udb[i*64+0]-=k; k=ctx->udb[i*64+0]+k;
		if((ctx->ubm[i*4+0]!=BTIC2C_BLKM_SKIP) &&
				(k || ctx->udb[i*64+0] || !skr))
			{ skrs=0; }

		skr=BTIC2CH_QuantBlock(ctx, ctx->vdb+i*64, ctx->vdb+i*64, 1);
		ctx->vdb[i*64+0]-=l; l=ctx->vdb[i*64+0]+l;
		if((ctx->vbm[i*4+0]!=BTIC2C_BLKM_SKIP) &&
				(k || ctx->vdb[i*64+0] || !skr))
			{ skrs=0; }

		if(!skrs) { ctx->skip_map[i>>3]&=~(1<<(i&7)); }
	}

	if(ctx->img_alpha)
	{
		j=(xs3/8)*(ys3/8); k=0;
		for(i=0; i<j; i++)
		{
			skrs=BTIC2CH_QuantBlock(ctx, ctx->adb+i*64, ctx->adb+i*64, 2);
			ctx->adb[i*64+0]-=k; k=ctx->adb[i*64+0]+k;

			if((ctx->abm[i*4+0]!=BTIC2C_BLKM_SKIP) && (k || !skrs))
				{ ctx->skip_map[i>>3]&=~(1<<(i&7)); }
		}
	}
#endif

//	printf("M2\n");

	if(!(qf&BGBBTJ_QFL_PFRAME))
	{
#if 1
		for(i=0; i<256; i++)
		{
//			ctx->dcsy[i]=1;
//			ctx->acsy[i]=1;
//			ctx->dcsuv[i]=1;
//			ctx->acsuv[i]=1;
//			ctx->dcsa[i]=1;
//			ctx->acsa[i]=1;

#if 0
			ctx->dcsy[i]>>=1;
			ctx->acsy[i]>>=1;
			ctx->dcsuv[i]>>=1;
			ctx->acsuv[i]>>=1;
			ctx->dcsa[i]>>=1;
			ctx->acsa[i]>>=1;
#endif

			ctx->dcsy[i]+=1;
			ctx->acsy[i]+=1;
			ctx->acs2y[i]+=1;
			ctx->dcsuv[i]+=1;
			ctx->acsuv[i]+=1;
			ctx->acs2uv[i]+=1;
			ctx->dcsa[i]+=1;
			ctx->acsa[i]+=1;
			ctx->acs2a[i]+=1;
		}
#endif

		// pretend 1/2 image of skip blocks is present
//		j=(wmb*hmb)/2;
//		ctx->dcsy[0x81]+=j;
//		ctx->dcsy[0x00]+=j;

		for(i=0; i<8; i++)
			ctx->blk_mode[i]=0;
		ctx->blk_move[0]=0;
		ctx->blk_move[1]=0;

		j=(xs2/8)*(ys2/8);
		k=(xs3/8)*(ys3/8);
		for(i=0; i<j; i++)
		{
			BTIC2CH_StatBlockMode(ctx, ctx->ybm+i*4, 0,
				ctx->dcsy, ctx->acsy);
			BTIC2CH_StatBlock(ctx, ctx->ydb+i*64,
				ctx->dcsy, ctx->acsy, ctx->dcsy, ctx->acs2y);
		}
		for(i=0; i<k; i++)
		{
			BTIC2CH_StatBlockMode(ctx, ctx->ubm+i*4, 1,
				ctx->dcsuv, ctx->acsuv);
			BTIC2CH_StatBlock(ctx, ctx->udb+i*64,
				ctx->dcsuv, ctx->acsuv, ctx->dcsuv, ctx->acs2uv);
			BTIC2CH_StatBlockMode(ctx, ctx->vbm+i*4, 2,
				ctx->dcsuv, ctx->acsuv);
			BTIC2CH_StatBlock(ctx, ctx->vdb+i*64,
				ctx->dcsuv, ctx->acsuv, ctx->dcsuv, ctx->acs2uv);
		}
		if(ctx->img_alpha)
		{
			for(i=0; i<k; i++)
			{
				BTIC2CH_StatBlockMode(ctx, ctx->abm+i*4, 3,
					ctx->dcsa, ctx->acsa);
				BTIC2CH_StatBlock(ctx, ctx->vdb+i*64,
					ctx->dcsa, ctx->acsa, ctx->dcsa, ctx->acs2a);
			}
		}

//		BTIC2CH_BuildLengths(dcs, 256, ctx->huff_len+0*256, 16);
//		BTIC2CH_BuildLengths(acs, 256, ctx->huff_len+1*256, 16);
//		BTIC2CH_BuildLengths(dcsuv, 256, ctx->huff_len+2*256, 16);
//		BTIC2CH_BuildLengths(acsuv, 256, ctx->huff_len+3*256, 16);

		BTIC2CH_BuildLengthsAdjust(ctx->dcsy, 256, ctx->huff_len+0*256, 16);
		BTIC2CH_BuildLengthsAdjust(ctx->acsy, 256, ctx->huff_len+1*256, 16);
		BTIC2CH_BuildLengthsAdjust(ctx->dcsuv, 256, ctx->huff_len+2*256, 16);
		BTIC2CH_BuildLengthsAdjust(ctx->acsuv, 256, ctx->huff_len+3*256, 16);

		BTIC2CH_BuildLengthsAdjust(ctx->acs2y, 256,
			ctx->huff_len+6*256, 16);
		BTIC2CH_BuildLengthsAdjust(ctx->acs2uv, 256,
			ctx->huff_len+7*256, 16);

		if(ctx->img_alpha)
		{
			BTIC2CH_BuildLengthsAdjust(ctx->dcsa, 256,
				ctx->huff_len+4*256, 16);
			BTIC2CH_BuildLengthsAdjust(ctx->acsa, 256,
				ctx->huff_len+5*256, 16);
			BTIC2CH_BuildLengthsAdjust(ctx->acs2a, 256,
				ctx->huff_len+8*256, 16);
		}

#if 1
		for(i=0; i<256; i++)
		{
			ctx->dcsy[i]>>=1;
			ctx->acsy[i]>>=1;
			ctx->acs2y[i]>>=1;
			ctx->dcsuv[i]>>=1;
			ctx->acsuv[i]>>=1;
			ctx->acs2uv[i]>>=1;
			ctx->dcsa[i]>>=1;
			ctx->acsa[i]>>=1;
			ctx->acs2a[i]>>=1;
		}
#endif
	}else
	{
#if 1
		for(i=0; i<256; i++)
		{
			ctx->dcsy[i]>>=1;
			ctx->acsy[i]>>=1;
			ctx->acs2y[i]>>=1;
			ctx->dcsuv[i]>>=1;
			ctx->acsuv[i]>>=1;
			ctx->acs2uv[i]>>=1;
			ctx->dcsa[i]>>=1;
			ctx->acsa[i]>>=1;
			ctx->acs2a[i]>>=1;
		}
#endif

		// pretend 1/2 image of skip blocks is present
//		j=(wmb*hmb)/2;
//		ctx->dcsy[0x81]+=j;
//		ctx->dcsy[0x00]+=j;

		for(i=0; i<8; i++)
			ctx->blk_mode[i]=0;
		ctx->blk_move[0]=0;
		ctx->blk_move[1]=0;

		j=(xs2/8)*(ys2/8);
		k=(xs3/8)*(ys3/8);
		for(i=0; i<j; i++)
		{
			BTIC2CH_StatBlockMode(ctx, ctx->ybm+i*4, 0,
				ctx->dcsy, ctx->acsy);
			BTIC2CH_StatBlock(ctx, ctx->ydb+i*64,
				ctx->dcsy, ctx->acsy, ctx->dcsy, ctx->acs2y);
		}
		for(i=0; i<k; i++)
		{
			BTIC2CH_StatBlockMode(ctx, ctx->ubm+i*4, 1,
				ctx->dcsuv, ctx->acsuv);
			BTIC2CH_StatBlock(ctx, ctx->udb+i*64,
				ctx->dcsuv, ctx->acsuv, ctx->dcsuv, ctx->acs2uv);
			BTIC2CH_StatBlockMode(ctx, ctx->vbm+i*4, 2,
				ctx->dcsuv, ctx->acsuv);
			BTIC2CH_StatBlock(ctx, ctx->vdb+i*64,
				ctx->dcsuv, ctx->acsuv, ctx->dcsuv, ctx->acs2uv);
		}
		if(ctx->img_alpha)
		{
			for(i=0; i<k; i++)
			{
				BTIC2CH_StatBlockMode(ctx, ctx->abm+i*4, 3,
					ctx->dcsa, ctx->acsa);
				BTIC2CH_StatBlock(ctx, ctx->vdb+i*64,
					ctx->dcsa, ctx->acsa, ctx->dcsa, ctx->acs2a);
			}
		}

#if 0
		for(i=0; i<256; i++)
		{
			ctx->dcsy[i]>>=1;
			ctx->acsy[i]>>=1;
			ctx->dcsuv[i]>>=1;
			ctx->acsuv[i]>>=1;
			ctx->dcsa[i]>>=1;
			ctx->acsa[i]>>=1;
		}
#endif
	}

	ctx->huff_ct=obuf;
	ctx->huff_win=0;
	ctx->huff_pos=0;

//	*ctx->huff_ct++=0xFF;
//	*ctx->huff_ct++=JPG_SOI;

	if(!(qf&BGBBTJ_QFL_PFRAME))
	{
		BTIC2C_EmitDQT(ctx, 0);
		if(!ctx->img_mono)
			BTIC2C_EmitDQT(ctx, 1);
		if(ctx->img_alpha)
			BTIC2C_EmitDQT(ctx, 2);

		BTIC2C_EmitSOF(ctx, xs, ys);

		BTIC2C_EmitDHT(ctx, 0);
		BTIC2C_EmitDHT(ctx, 1);
		BTIC2C_EmitDHT(ctx, 6);
		if(!ctx->img_mono)
		{
			BTIC2C_EmitDHT(ctx, 2);
			BTIC2C_EmitDHT(ctx, 3);
			BTIC2C_EmitDHT(ctx, 7);
		}
		if(ctx->img_alpha)
		{
			BTIC2C_EmitDHT(ctx, 4);
			BTIC2C_EmitDHT(ctx, 5);
			BTIC2C_EmitDHT(ctx, 8);
		}

		BTIC2C_EmitSOS(ctx);
	}

	ct0=ctx->huff_ct;
	*ctx->huff_ct++=0xE1;
	*ctx->huff_ct++=0x00;
	*ctx->huff_ct++=0x00;
	*ctx->huff_ct++=0x00;

	ctx->huff_win=0;
	ctx->huff_pos=0;

	for(i=0; i<16; i++)
	{
		ctx->cnt_hsn[i]=0;
		ctx->cnt_hsb[i]=0;
		ctx->cnt_hdn[i]=0;
		ctx->cnt_hdb[i]=0;
		ctx->cnt_hln[i]=0;
		ctx->cnt_hlb[i]=0;
		ctx->cnt_hvn[i]=0;
		ctx->cnt_hvb[i]=0;
	}

	for(i=0; i<8; i++)
		ctx->blk_mode[i]=0;
	ctx->blk_move[0]=0;
	ctx->blk_move[1]=0;

	if(ctx->img_mono)
	{
		j=(xs2/8)*(ys2/8);
		for(i=0; i<j; i++)
		{
			BTIC2CH_EncodeBlockMode(ctx, ctx->ybm+i*4, 0, 0, 1);
//			BTIC2CH_EncodeBlock(ctx, ctx->ydb+i*64, 0, 1);
//			BTIC2CH_EncodeBlock(ctx, ctx->ydb+i*64, 0, 1, 0, 1);
			BTIC2CH_EncodeBlock(ctx, ctx->ydb+i*64, 0, 1, 0, 6);
		}
	}else
	{
		for(i=0; i<(ys3/8); i++)
			for(j=0; j<(xs3/8); j++)
		{
			if(qf&BGBBTJ_QFL_PFRAME)
			{
				k=i*wmb+j;
				if(ctx->skip_map[k>>3]&(1<<(k&7)))
				{
					BTIC2CH_EncodeSymbol(ctx, 0, 0x81);
					BTIC2CH_EncodeSymbol(ctx, 0, 0x00);

					ctx->dcsy[0x81]++;
					ctx->dcsy[0x00]++;
					continue;
				}
			}

			k=((i*2+0)*(xs2/8)+j*2+0);
			BTIC2CH_EncodeBlockMode(ctx, ctx->ybm+k*4, 0, 0, 1);
//			BTIC2CH_EncodeBlock(ctx, ctx->ydb+k*64, 0, 1, 0, 1);
			BTIC2CH_EncodeBlock(ctx, ctx->ydb+k*64, 0, 1, 0, 6);

			k=((i*2+0)*(xs2/8)+j*2+1);
			BTIC2CH_EncodeBlockMode(ctx, ctx->ybm+k*4, 0, 0, 1);
//			BTIC2CH_EncodeBlock(ctx, ctx->ydb+k*64, 0, 1, 0, 1);
			BTIC2CH_EncodeBlock(ctx, ctx->ydb+k*64, 0, 1, 0, 6);

			k=((i*2+1)*(xs2/8)+j*2+0);
			BTIC2CH_EncodeBlockMode(ctx, ctx->ybm+k*4, 0, 0, 1);
//			BTIC2CH_EncodeBlock(ctx, ctx->ydb+k*64, 0, 1, 0, 1);
			BTIC2CH_EncodeBlock(ctx, ctx->ydb+k*64, 0, 1, 0, 6);

			k=((i*2+1)*(xs2/8)+j*2+1);
			BTIC2CH_EncodeBlockMode(ctx, ctx->ybm+k*4, 0, 0, 1);
//			BTIC2CH_EncodeBlock(ctx, ctx->ydb+k*64, 0, 1, 0, 1);
			BTIC2CH_EncodeBlock(ctx, ctx->ydb+k*64, 0, 1, 0, 6);

			k=i*(xs3/8)+j;
			BTIC2CH_EncodeBlockMode(ctx, ctx->ubm+k*4, 1, 2, 3);
//			BTIC2CH_EncodeBlock(ctx, ctx->udb+k*64, 2, 3, 2, 3);
			BTIC2CH_EncodeBlock(ctx, ctx->udb+k*64, 2, 3, 2, 7);

			BTIC2CH_EncodeBlockMode(ctx, ctx->vbm+k*4, 2, 2, 3);
//			BTIC2CH_EncodeBlock(ctx, ctx->vdb+k*64, 2, 3, 2, 3);
			BTIC2CH_EncodeBlock(ctx, ctx->vdb+k*64, 2, 3, 2, 7);

			if(ctx->img_alpha)
			{
				BTIC2CH_EncodeBlockMode(ctx, ctx->abm+k*4, 3, 4, 5);
//				BTIC2CH_EncodeBlock(ctx, ctx->adb+k*64, 4, 5, 4, 5);
				BTIC2CH_EncodeBlock(ctx, ctx->adb+k*64, 4, 5, 4, 8);
			}
		}
	}

	if(0)
	{
		for(i=0; i<16; i++)
		{
			j=ctx->cnt_hsn[i]+ctx->cnt_hdn[i]+
				ctx->cnt_hln[i]+ctx->cnt_hvn[i];
			k=ctx->cnt_hsb[i]+ctx->cnt_hdb[i]+
				ctx->cnt_hlb[i]+ctx->cnt_hvb[i];
				
			if(!j)continue;

			printf("Ht=%d(N%d B%d A%.1f) ",
				i, j, k, ((double)k)/j);
			
			if(ctx->cnt_hsn[i])
			{
				printf("S(N%d B%d A%.1f) ",
					ctx->cnt_hsn[i], ctx->cnt_hsb[i],
					((double)ctx->cnt_hsb[i])/ctx->cnt_hsn[i]);
			}
			if(ctx->cnt_hdn[i])
			{
				printf("D(N%d B%d A%.1f) ",
					ctx->cnt_hdn[i], ctx->cnt_hdb[i],
					((double)ctx->cnt_hdb[i])/ctx->cnt_hdn[i]);
			}
			if(ctx->cnt_hln[i])
			{
				printf("L(N%d B%d A%.1f) ",
					ctx->cnt_hln[i], ctx->cnt_hlb[i],
					((double)ctx->cnt_hlb[i])/ctx->cnt_hln[i]);
			}
			if(ctx->cnt_hvn[i])
			{
				printf("V(N%d B%d A%.1f) ",
					ctx->cnt_hvn[i], ctx->cnt_hvb[i],
					((double)ctx->cnt_hvb[i])/ctx->cnt_hvn[i]);
			}
			printf("\n");
		}
	}

	if(1)
	{
#if 1
		if(ctx->img_mono)
		{
			j=(xs2/8)*(ys2/8); k=0;
			for(i=0; i<j; i++)
			{
				tp=ctx->ydb+i*64;
				BTIC2CH_DequantBlock(ctx, tp, tp, 0);
				ctx->ydb[i*64+0]+=k; k=ctx->ydb[i*64+0];
			}
		}else
		{
			l=0;
			for(i=0; i<(ys3/8); i++)
				for(j=0; j<(xs3/8); j++)
			{
				tp=ctx->ydb+((i*2+0)*(xs2/8)+j*2+0)*64;
				BTIC2CH_DequantBlock(ctx, tp, tp, 0);
				tp[0]+=l; l=tp[0];

				tp=ctx->ydb+((i*2+0)*(xs2/8)+j*2+1)*64;
				BTIC2CH_DequantBlock(ctx, tp, tp, 0);
				tp[0]+=l; l=tp[0];

				tp=ctx->ydb+((i*2+1)*(xs2/8)+j*2+0)*64;
				BTIC2CH_DequantBlock(ctx, tp, tp, 0);
				tp[0]+=l; l=tp[0];

				tp=ctx->ydb+((i*2+1)*(xs2/8)+j*2+1)*64;
				BTIC2CH_DequantBlock(ctx, tp, tp, 0);
				tp[0]+=l; l=tp[0];
			}
		}

		j=(xs3/8)*(ys3/8); k=0; l=0;
		for(i=0; i<j; i++)
		{
			BTIC2CH_DequantBlock(ctx, ctx->udb+i*64, ctx->udb+i*64, 1);
			ctx->udb[i*64+0]+=k; k=ctx->udb[i*64+0];

			BTIC2CH_DequantBlock(ctx, ctx->vdb+i*64, ctx->vdb+i*64, 1);
			ctx->vdb[i*64+0]+=l; l=ctx->vdb[i*64+0];
		}

		if(ctx->img_alpha)
		{
			j=(xs3/8)*(ys3/8); k=0;
			for(i=0; i<j; i++)
			{
				BTIC2CH_DequantBlock(ctx, ctx->adb+i*64, ctx->adb+i*64, 2);
				ctx->adb[i*64+0]+=k; k=ctx->adb[i*64+0];
			}
		}
#endif

		BTIC2C_FilterImageDeltaIWHT(ctx, ctx->lyb, ctx->lyb,
			ctx->ydb, ctx->ybm, 0, xs2, ys2);
		BTIC2C_FilterImageDeltaIWHT(ctx, ctx->lub, ctx->lub,
			ctx->udb, ctx->ubm, 1, xs3, ys3);
		BTIC2C_FilterImageDeltaIWHT(ctx, ctx->lvb, ctx->lvb,
			ctx->vdb, ctx->vbm, 1, xs3, ys3);
		BTIC2C_FilterImageDeltaIWHT(ctx, ctx->lab, ctx->lab,
			ctx->adb, ctx->abm, 2, xs3, ys3);
	}

	BTIC2CH_FlushBits(ctx);

	i=ctx->huff_ct-ct0;
	ct0[1]=(i>>16)&255;
	ct0[2]=(i>> 8)&255;
	ct0[3]=(i>> 0)&255;

	i=ctx->huff_ct-obuf;

	return(i);
}

BTIC2C_Context *BTIC2C_AllocContext()
{
	BTIC2C_Context *ctx;
	
	ctx=malloc(sizeof(BTIC2C_Context));
	memset(ctx, 0, sizeof(BTIC2C_Context));
	return(ctx);
}

void BTIC2C_FreeContext(BTIC2C_Context *ctx)
{
	int i;

	if(ctx->yb)
	{
		free(ctx->yb);
		free(ctx->ub);
		free(ctx->vb);
		free(ctx->ab);
		free(ctx->ydb);
		free(ctx->udb);
		free(ctx->vdb);
		free(ctx->adb);
	}

//	if(ctx->img_imgbuf)
//		free(ctx->img_imgbuf);

#if 1
	for(i=0; i<4; i++)
	{
		if(ctx->img_scbuf[i])free(ctx->img_scbuf[i]);
		if(ctx->img_sibuf[i])free(ctx->img_sibuf[i]);
		if(ctx->img_sjbuf[i])free(ctx->img_sjbuf[i]);
		ctx->img_scbuf[i]=NULL;
		ctx->img_sibuf[i]=NULL;
		ctx->img_sjbuf[i]=NULL;
	}
#endif

	free(ctx);
}

int BTIC2C_Encode(
	byte *ibuf, byte *obuf, int xs, int ys, int qf, int pf)
{
	BTIC2C_Context *ctx;
	int sz;
	
	ctx=BTIC2C_AllocContext();
	sz=BTIC2C_EncodeCtx(ctx, ibuf, obuf, 1<<20, xs, ys, qf, pf);
	BTIC2C_FreeContext(ctx);
	return(sz);
}

void BTIC2C_FlushEncodeFast(BTIC2C_Context *ctx)
{
	if(!ctx)return;
	ctx->img_tabcacheframe=0;	//force rebuild
//	ctx->oldAlphaClr=0;
}
