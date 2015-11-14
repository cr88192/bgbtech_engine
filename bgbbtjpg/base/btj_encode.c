#include <bgbbtj.h>

extern const int pdjpg_zigzag[64];
extern const int pdjpg_zigzag2[64];

extern float pdjpg_exptab[256];
extern float pdjpg_scltab[256];

//Encoder

void BGBBTJ_Huff_WriteBit(BGBBTJ_JPG_Context *ctx, int i)
{
	pdjhuff_win|=i<<(31-pdjhuff_pos);
	pdjhuff_pos++;
	if(pdjhuff_pos>=8)
	{
		i=(pdjhuff_win>>24)&0xFF;
		*pdjhuff_ct++=i;
		if(i==0xFF)*pdjhuff_ct++=0x00;
		pdjhuff_win<<=8;
		pdjhuff_pos-=8;
	}
}

void BGBBTJ_Huff_WriteNBits(BGBBTJ_JPG_Context *ctx, int v, int n)
{
	int i, j, k;

	v&=(1<<n)-1;

	j=pdjhuff_pos+n;
	k=pdjhuff_win|(v<<(32-j));
	while(j>=8)
	{
		i=(k>>24)&0xFF;
		*pdjhuff_ct++=i;
		if(i==0xFF)*pdjhuff_ct++=0x00;
		k<<=8;
		j-=8;
	}
	pdjhuff_pos=j;
	pdjhuff_win=k;

//	pdjhuff_win|=i<<((32-n)-pdjhuff_pos);
//	pdjhuff_pos+=n;
//	while(pdjhuff_pos>=8)
//	{
//		i=(pdjhuff_win>>24)&0xFF;
//		*pdjhuff_ct++=i;
//		if(i==0xFF)*pdjhuff_ct++=0x00;
//		pdjhuff_win<<=8;
//		pdjhuff_pos-=8;
//	}
}

void BGBBTJ_Huff_FlushBits(BGBBTJ_JPG_Context *ctx)
{
	int i;
	while(pdjhuff_pos>0)
	{
		i=(pdjhuff_win>>24)&0xFF;
		*pdjhuff_ct++=i;
		if(i==0xFF)*pdjhuff_ct++=0x00;
		pdjhuff_win<<=8;
		pdjhuff_pos-=8;
	}
}

void BGBBTJ_Huff_WriteString(BGBBTJ_JPG_Context *ctx, char *str)
{
	char *s;

	BGBBTJ_Huff_FlushBits(ctx);	
	s=str;
	while(*s)
		*pdjhuff_ct++=*s++;
	*pdjhuff_ct++=0;
}

int BGBBTJ_Huff_BalanceTree_r(short *nodes, short *nlen, int root, int h, int ml)
{
	int h0, h1, h2, h3;
	int l0, l1, l2;

	if(root<0)return(0);

	h1=BGBBTJ_Huff_BalanceTree_r(nodes, nlen, nodes[root*2+0], h+1, ml);
	h2=BGBBTJ_Huff_BalanceTree_r(nodes, nlen, nodes[root*2+1], h+1, ml);
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

void BGBBTJ_Huff_CalcLengths_r(short *nodes, byte *cl, int root, int h)
{
	if(root<0)
	{
		if(root==-513)return;
		cl[(-root)-1]=h;
		return;
	}

	BGBBTJ_Huff_CalcLengths_r(nodes, cl, nodes[root*2+0], h+1);
	BGBBTJ_Huff_CalcLengths_r(nodes, cl, nodes[root*2+1], h+1);
}

int BGBBTJ_Huff_BuildLengths(int *stat, int nc, byte *cl, int ml)
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
		k=BGBBTJ_Huff_BalanceTree_r(nodes, nlen, l, 0, ml);
	if(k>ml)return(-2);	//failed to balance tree

	BGBBTJ_Huff_CalcLengths_r(nodes, cl, l, 0);
	return(0);
}

int BGBBTJ_Huff_BuildLengthsAdjust(int *stat, int nc, byte *cl, int ml)
{
	int i, j;

	while(1)
	{
		j=BGBBTJ_Huff_BuildLengths(stat, nc, cl, ml);
		if(j<0)
			printf("BGBBTJ_Huff_BuildLengthsAdjust: Huff Fail %d\n");

		for(i=0; i<nc; i++)
			if(stat[i] && !cl[i])
				break;
		if(i>=nc)break;

		printf("BGBBTJ_Huff_BuildLengthsAdjust: Fiddle Adjust\n");
		for(i=0; i<nc; i++)
			stat[i]++;
		continue;
	}
	return(0);
}

void BGBBTJ_Huff_EncodeSymbol(BGBBTJ_JPG_Context *ctx, int tab, int v)
{
	tab<<=8;
	BGBBTJ_Huff_WriteNBits(ctx, ctx->huff_code[tab|v], ctx->huff_len[tab|v]);
}

void BGBBTJ_Huff_EncodeVal(BGBBTJ_JPG_Context *ctx, int tab, int z, int v)
{
	int i, j, k;

	if(!v) { BGBBTJ_Huff_EncodeSymbol(ctx, tab, z<<4); return; }

	if(v>0)
	{
		i=1; while(v>=(1<<i))i++;
		if(i>15)
		{
			printf("BGBBTJ_Huff_EncodeVal: Overflow A\n");
			i=15;
		}
		
		BGBBTJ_Huff_EncodeSymbol(ctx, tab, (z<<4)|i);
		BGBBTJ_Huff_WriteNBits(ctx, v, i);
		return;
	}

	i=1; j=-v; while(j>=(1<<i))i++;
	if(i>15)
	{
		printf("BGBBTJ_Huff_EncodeVal: Overflow B\n");
		i=15;
	}

	BGBBTJ_Huff_EncodeSymbol(ctx, tab, (z<<4)|i);

	k=(1<<i)-(j+1);
	BGBBTJ_Huff_WriteNBits(ctx, k, i);
	return;
}

void BGBBTJ_Huff_StatVal(int *stat, int z, int v)
{
	int i, j, k;

	if(!v) { stat[z<<4]++; return; }

	if(v>0)
	{
		i=1; while(v>=(1<<i))i++;
		if(i>15)i=15;
		stat[(z<<4)|i]++;
		return;
	}

	i=1; j=-v; while(j>=(1<<i))i++;
	if(i>15)i=15;
	stat[(z<<4)|i]++;
}

#if 1
int BGBBTJ_Huff_EncodeBlock(BGBBTJ_JPG_Context *ctx,
	short *buf, int dctab, int actab)
{
	int i, j, k;

	BGBBTJ_Huff_EncodeVal(ctx, dctab, 0, buf[0]);

	for(i=1; i<64; i++)
	{
		j=buf[pdjpg_zigzag2[i]];
		if(j)
		{
			BGBBTJ_Huff_EncodeVal(ctx, actab, 0, j);
			continue;
		}

//		if(buf[pdjpg_zigzag2[i]])
//		{
//			BGBBTJ_Huff_EncodeVal(ctx, actab, 0, buf[pdjpg_zigzag2[i]]);
//			continue;
//		}
		for(j=i; j<64; j++)
			if(buf[pdjpg_zigzag2[j]])break;
		if(j>=64)
		{
			BGBBTJ_Huff_EncodeSymbol(ctx, actab, 0);
			break;
		}

		j-=i; if(j>15)j=15;
		BGBBTJ_Huff_EncodeVal(ctx, actab, j, buf[pdjpg_zigzag2[i+j]]);
		i+=j;
	}
	return(0);
}


int BGBBTJ_Huff_StatBlock(short *buf, int *dcstat, int *acstat)
{
	int i, j, k;

	BGBBTJ_Huff_StatVal(dcstat, 0, buf[0]);
	for(i=1; i<64; i++)
	{
		if(buf[pdjpg_zigzag2[i]])
		{
			BGBBTJ_Huff_StatVal(acstat, 0, buf[pdjpg_zigzag2[i]]);
			continue;
		}
		for(j=i; j<64; j++)
			if(buf[pdjpg_zigzag2[j]])break;
		if(j>=64)
		{
			acstat[0]++;
			break;
		}

		j-=i; if(j>15)j=15;
		BGBBTJ_Huff_StatVal(acstat, j, buf[pdjpg_zigzag2[i+j]]);
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

#define memcpy16(dst, src)	memcpy(dst, src, 16)

#if 0
int BGBBTJ_JPG_GetImgBlk(char *blk, int xo, int yo, byte *img, int xs, int ys)
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
	return(0);
}
#endif

#if 1
int BGBBTJ_JPG_GetImgBlk16(
	short *blk, int xo, int yo,
	short *img, int xs, int ys)
{
	short *cs;
	short *ct;
	int i, j, k;

	cs=img+(yo*xs+xo); ct=blk;
	memcpy16(ct, cs); ct+=8; cs+=xs;
	memcpy16(ct, cs); ct+=8; cs+=xs;
	memcpy16(ct, cs); ct+=8; cs+=xs;
	memcpy16(ct, cs); ct+=8; cs+=xs;
	memcpy16(ct, cs); ct+=8; cs+=xs;
	memcpy16(ct, cs); ct+=8; cs+=xs;
	memcpy16(ct, cs); ct+=8; cs+=xs;
	memcpy16(ct, cs);
	return(0);
}
#endif

// int BGBBTJ_JPG_FilterImageDCT(byte *ibuf, short *obuf, int xs, int ys)
int BGBBTJ_JPG_FilterImageDCT(short *ibuf, short *obuf,
	int xs, int ys, int dcbias)
{
//	static short tblk[DCTSZ2], tblk2[DCTSZ2];
//	static char blk[DCTSZ2];
	short tblk[DCTSZ2], tblk2[DCTSZ2];
//	char blk[DCTSZ2];
	short blk[DCTSZ2];
	int i, j, k, l, xn, yn;

	xn=(xs/DCTSZ);
	yn=(ys/DCTSZ);
	k=0;
	for(i=0; i<yn; i++)
		for(j=0; j<xn; j++)
	{
//		BGBBTJ_JPG_GetImgBlk(blk, j*DCTSZ, i*DCTSZ, ibuf, xs, ys);
		BGBBTJ_JPG_GetImgBlk16(blk, j*DCTSZ, i*DCTSZ, ibuf, xs, ys);
		BGBBTJ_JPG_TransDCT(blk, obuf+k*DCTSZ2, dcbias);
//		BGBBTJ_JPG_TransQDCT(blk, obuf+k*DCTSZ2, dcbias);
		k++;
	}
	return(0);
}

//int BGBBTJ_JPG_FilterImageRDCT(byte *ibuf, short *obuf, int xs, int ys)
int BGBBTJ_JPG_FilterImageRDCT(short *ibuf, short *obuf,
	int xs, int ys, int dcbias)
{
//	static short tblk[DCTSZ2], tblk2[DCTSZ2];
//	static char blk[DCTSZ2];
	short tblk[DCTSZ2], tblk2[DCTSZ2];
//	char blk[DCTSZ2];
	short blk[DCTSZ2];
	int i, j, k, l;

	k=0;
	for(i=0; i<(ys/DCTSZ); i++)
		for(j=0; j<(xs/DCTSZ); j++)
	{
//		BGBBTJ_JPG_GetImgBlk(blk, j*DCTSZ, i*DCTSZ, ibuf, xs, ys);
		BGBBTJ_JPG_GetImgBlk16(blk, j*DCTSZ, i*DCTSZ, ibuf, xs, ys);
		BGBBTJ_JPG_TransRDCT(blk, obuf+k*DCTSZ2, dcbias);
		k++;
	}
	return(0);
}

void BGBBTJ_JPG_EmitDQT(BGBBTJ_JPG_Context *ctx, int n)
{
	int i;

	*pdjhuff_ct++=0xFF;
	*pdjhuff_ct++=JPG_DQT;

	i=64+3;
	*pdjhuff_ct++=i>>8;
	*pdjhuff_ct++=i&0xFF;

	*pdjhuff_ct++=n;
	for(i=0; i<64; i++)
		*pdjhuff_ct++=ctx->jpg_qt[n][pdjpg_zigzag2[i]];
}

void BGBBTJ_JPG_EmitSOF(BGBBTJ_JPG_Context *ctx, int xs, int ys)
{
	byte *p;
	int i;

	*pdjhuff_ct++=0xFF;
	*pdjhuff_ct++=JPG_SOF0;

	i=8+(ctx->jpg_mono?1:3)*3;
	p=pdjhuff_ct;
	*pdjhuff_ct++=i>>8;	//Lf
	*pdjhuff_ct++=i&0xFF;

	*pdjhuff_ct++=8; 	//P

	*pdjhuff_ct++=ys>>8;	//Y
	*pdjhuff_ct++=ys&0xFF;	//Y
	*pdjhuff_ct++=xs>>8;	//X
	*pdjhuff_ct++=xs&0xFF;	//X

	if(ctx->jpg_mono)
	{
		*pdjhuff_ct++=1;	//Nf

		*pdjhuff_ct++=1;	//Ci
		*pdjhuff_ct++=0x11;	//Hi Vi
		*pdjhuff_ct++=0;	//Tqi
	}else if(ctx->jpg_is420)
	{
		if(!ctx->jpg_yuvw)
		{
			//YUV 4:2:0
		
			*pdjhuff_ct++=3;	//Nf

			*pdjhuff_ct++=1;	//Ci
			*pdjhuff_ct++=0x22;	//Hi Vi
			*pdjhuff_ct++=0;	//Tqi
			*pdjhuff_ct++=2;	//Ci
			*pdjhuff_ct++=0x11;	//Hi Vi
			*pdjhuff_ct++=1;	//Tqi
			*pdjhuff_ct++=3;	//Ci
			*pdjhuff_ct++=0x11;	//Hi Vi
			*pdjhuff_ct++=1;	//Tqi
		}else
		{
			//YUVW 4:2:0

			*pdjhuff_ct++=4;	//Nf

			*pdjhuff_ct++=1;	//Ci
			*pdjhuff_ct++=0x22;	//Hi Vi
			*pdjhuff_ct++=0;	//Tqi
			*pdjhuff_ct++=2;	//Ci
			*pdjhuff_ct++=0x11;	//Hi Vi
			*pdjhuff_ct++=1;	//Tqi
			*pdjhuff_ct++=3;	//Ci
			*pdjhuff_ct++=0x11;	//Hi Vi
			*pdjhuff_ct++=1;	//Tqi
			*pdjhuff_ct++=4;	//Ci
			*pdjhuff_ct++=0x11;	//Hi Vi
			*pdjhuff_ct++=1;	//Tqi
		}
	}else if(ctx->jpg_is410)
	{
		if(!ctx->jpg_yuvw)
		{
			//YUV 4:1:0
		
			*pdjhuff_ct++=3;	//Nf

			*pdjhuff_ct++=1;	//Ci
			*pdjhuff_ct++=0x44;	//Hi Vi
			*pdjhuff_ct++=0;	//Tqi
			*pdjhuff_ct++=2;	//Ci
			*pdjhuff_ct++=0x11;	//Hi Vi
			*pdjhuff_ct++=1;	//Tqi
			*pdjhuff_ct++=3;	//Ci
			*pdjhuff_ct++=0x11;	//Hi Vi
			*pdjhuff_ct++=1;	//Tqi
		}else
		{
			//YUVW 4:1:0

			*pdjhuff_ct++=4;	//Nf

			*pdjhuff_ct++=1;	//Ci
			*pdjhuff_ct++=0x44;	//Hi Vi
			*pdjhuff_ct++=0;	//Tqi
			*pdjhuff_ct++=2;	//Ci
			*pdjhuff_ct++=0x11;	//Hi Vi
			*pdjhuff_ct++=1;	//Tqi
			*pdjhuff_ct++=3;	//Ci
			*pdjhuff_ct++=0x11;	//Hi Vi
			*pdjhuff_ct++=1;	//Tqi
			*pdjhuff_ct++=4;	//Ci
			*pdjhuff_ct++=0x11;	//Hi Vi
			*pdjhuff_ct++=1;	//Tqi
		}
	}else
	{
		if(!ctx->jpg_yuvw)
		{
			//YUV 4:4:4

			*pdjhuff_ct++=3;	//Nf

			*pdjhuff_ct++=1;	//Ci
			*pdjhuff_ct++=0x11;	//Hi Vi
			*pdjhuff_ct++=0;	//Tqi
			*pdjhuff_ct++=2;	//Ci
			*pdjhuff_ct++=0x11;	//Hi Vi
			*pdjhuff_ct++=1;	//Tqi
			*pdjhuff_ct++=3;	//Ci
			*pdjhuff_ct++=0x11;	//Hi Vi
			*pdjhuff_ct++=1;	//Tqi
		}else
		{
			//YUVW 4:4:4

			*pdjhuff_ct++=4;	//Nf

			*pdjhuff_ct++=1;	//Ci
			*pdjhuff_ct++=0x11;	//Hi Vi
			*pdjhuff_ct++=0;	//Tqi
			*pdjhuff_ct++=2;	//Ci
			*pdjhuff_ct++=0x11;	//Hi Vi
			*pdjhuff_ct++=1;	//Tqi
			*pdjhuff_ct++=3;	//Ci
			*pdjhuff_ct++=0x11;	//Hi Vi
			*pdjhuff_ct++=1;	//Tqi
			*pdjhuff_ct++=4;	//Ci
			*pdjhuff_ct++=0x11;	//Hi Vi
			*pdjhuff_ct++=1;	//Tqi
		}
	}

	i=pdjhuff_ct-p;
	p[0]=i>>8;	//Lf
	p[1]=i&0xFF;
}

void BGBBTJ_JPG_EmitSOS(BGBBTJ_JPG_Context *ctx)
{
	byte *p;
	int i;

	*pdjhuff_ct++=0xFF;
	*pdjhuff_ct++=JPG_SOS;

	i=6+(ctx->jpg_mono?1:3)*2;
	p=pdjhuff_ct;
	*pdjhuff_ct++=i>>8;	//Lf
	*pdjhuff_ct++=i&0xFF;

	if(ctx->jpg_yuvw)
	{
		*pdjhuff_ct++=4; 	//Ns

		*pdjhuff_ct++=1;	//Csi
		*pdjhuff_ct++=0x00;	//Tdi Tai
		*pdjhuff_ct++=2;	//Csi
		*pdjhuff_ct++=0x11;	//Tdi Tai
		*pdjhuff_ct++=3;	//Csi
		*pdjhuff_ct++=0x11;	//Tdi Tai
		*pdjhuff_ct++=4;	//Csi
		*pdjhuff_ct++=0x11;	//Tdi Tai
	}else if(!ctx->jpg_mono)
	{
		*pdjhuff_ct++=3; 	//Ns

		*pdjhuff_ct++=1;	//Csi
		*pdjhuff_ct++=0x00;	//Tdi Tai
		*pdjhuff_ct++=2;	//Csi
		*pdjhuff_ct++=0x11;	//Tdi Tai
		*pdjhuff_ct++=3;	//Csi
		*pdjhuff_ct++=0x11;	//Tdi Tai
	}else
	{
		*pdjhuff_ct++=1; 	//Ns

		*pdjhuff_ct++=1;	//Csi
		*pdjhuff_ct++=0x00;	//Tdi Tai
	}

	*pdjhuff_ct++=0; 	//Ss
	*pdjhuff_ct++=63; 	//Se
	*pdjhuff_ct++=0x00; 	//Ah Al

	i=pdjhuff_ct-p;
	p[0]=i>>8;	//Lf
	p[1]=i&0xFF;
}

void BGBBTJ_JPG_EmitDHT(BGBBTJ_JPG_Context *ctx, int tab)
{
	byte *p;
	int i, j, k;

	*pdjhuff_ct++=0xFF;
	*pdjhuff_ct++=JPG_DHT;

	i=8+3*1;	//temp value
	p=pdjhuff_ct;
	*pdjhuff_ct++=i>>8;	//Lf
	*pdjhuff_ct++=i&0xFF;

	i=(tab/2)|((tab&1)<<4);
	*pdjhuff_ct++=i; 	//Tc Th

	tab<<=8;
	for(i=1; i<=16; i++)
	{
		k=0;
		for(j=0; j<256; j++)
			if(ctx->huff_len[tab|j]==i)
				k++;
		*pdjhuff_ct++=k; 	//Li
	}

	k=0;
	for(i=1; i<=16; i++)
	{
		k<<=1;
		for(j=0; j<256; j++)
			if(ctx->huff_len[tab|j]==i)
		{
			*pdjhuff_ct++=j; 	//Vi
			ctx->huff_code[tab|j]=k++;
		}
	}

	if(k>=65536)
		printf("BGBBTJ_JPG_EmitDHT: Error %04X\n", k);

//	printf("DHT %04X\n", k);

	i=pdjhuff_ct-p;
	p[0]=i>>8;	//Lf
	p[1]=i&0xFF;
	
#if 0
	printf("DHT Tab=%d\n", tab>>8);

#if 0
	printf("{ ");
	
	for(i=0; i<32; i++)
	{
		for(j=0; j<8; j++)
		{
			printf("0x%04X, ", ctx->huff_code[tab|(i*8+j)]);
		}
		printf("\n");
	}
	printf("},\n");
#endif

	printf("{ ");
	
	for(i=0; i<16; i++)
	{
		for(j=0; j<16; j++)
		{
			printf("%2d, ", ctx->huff_len[tab|(i*16+j)]);
		}
		printf("\n");
	}
	printf("},\n");

#endif
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

void BGBBTJ_JPG_MakeQuantTabFastIJG_Y(
	short *inv, int cnt, byte *tab, float qf)
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

void BGBBTJ_JPG_MakeQuantTabFastIJG_UV(
	short *inv, int cnt, byte *tab, float qf)
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

void BGBBTJ_JPG_MakeQuantTabInput(short *inv, int cnt, byte *tab, float q)
{
	BGBBTJ_JPG_MakeQuantTabInput_AA(inv, cnt, tab, q);
}

void BGBBTJ_JPG_MakeQuantTabInputY(short *inv, int cnt, byte *tab, float q)
{
	byte taba[64], tabb[64];
	int i;

	//BGBBTJ_JPG_MakeQuantTabInput_AA(inv, cnt, tab, q);
	//BGBBTJ_JPG_MakeQuantTabFastIJG_UV(inv, cnt, tab, q);

	BGBBTJ_JPG_MakeQuantTabInput_AA(inv, cnt, taba, q);
	BGBBTJ_JPG_MakeQuantTabFastIJG_UV(inv, cnt, tabb, q);
	for (i = 0; i < 64; i++)
		tab[i] = (taba[i] + tabb[i]) / 2;
}

void BGBBTJ_JPG_MakeQuantTabInputUV(short *inv, int cnt, byte *tab, float q)
{
	byte taba[64], tabb[64];
	int i;

	//BGBBTJ_JPG_MakeQuantTabInput_AA(inv, cnt, tab, q);
	//BGBBTJ_JPG_MakeQuantTabFastIJG_UV(inv, cnt, tab, q);

	BGBBTJ_JPG_MakeQuantTabInput_AA(inv, cnt, taba, q);
	BGBBTJ_JPG_MakeQuantTabFastIJG_UV(inv, cnt, tabb, q);
	for (i = 0; i < 64; i++)
		tab[i] = (taba[i] + tabb[i]) / 2;
}

void BGBBTJ_JPG_MakeQuantTabInput_AA(short *in, int cnt, byte *tab, float q)
{
//	static double deltas[DCTSZ2];
	double deltas[DCTSZ2];
	double f, g, te;
	int i, j, k;

	if(q>=1)
	{
		for(i=0; i<DCTSZ2; i++)
			tab[i]=1;
		return;
	}

	for(j=0; j<DCTSZ2; j++)
		deltas[j]=0;

	for(i=0; i<cnt; i++)
		for(j=0; j<DCTSZ2; j++)
	{
		f=in[i*DCTSZ2+j];
		deltas[j]+=f*f;
	}
	for(j=0; j<DCTSZ2; j++)
	{
		deltas[j]=sqrt(deltas[j]/cnt);
//		deltas[j]/=cnt;
	}

	te=0;
	for(j=0; j<DCTSZ2; j++)
		te+=deltas[j];

	for(i=0; i<DCTSZ; i++)
	{
		for(j=0; j<DCTSZ; j++)
		{
			f=deltas[i*DCTSZ+j];

			f=f/te;
			if(f<0.00001)f=0.00001;
			g=(1.0-q);
			f=10*g*g*g/f;
			k=f;
			if(k<1)k=1;
			if(k>255)k=255;

//			printf("%f/%f %d\n", deltas[i*DCTSZ+j], te, k);

//			if(f<1)f=1;
//			if(f>255)f=255;
			tab[i*DCTSZ+j]=k;
		}
	}

#if 1
	if(tab[DCTSZ2-3]>1)tab[DCTSZ2-3]*=0.75;
	if(tab[DCTSZ2-2]>1)tab[DCTSZ2-2]*=0.75;
	if(tab[DCTSZ2-1]>1)tab[DCTSZ2-1]*=0.5;
#endif
}

void BGBBTJ_JPG_MakeQuantTabInputFast(
	short *in, int cnt, byte *tab, float q)
{
	int i;
//	BGBBTJ_JPG_MakeQuantTabInput(in, cnt, tab, q);

	i=cnt/16;
	if(i<1)i=1;
	BGBBTJ_JPG_MakeQuantTabInputY(in, i, tab, q);
}

void BGBBTJ_JPG_EmitMarkerBCST(BGBBTJ_JPG_Context *ctx)
{
	byte *lfp;
	int i, j, k;

	if(ctx->jpg_rdct)
	{
		//Emit libjpeg SERM marker
		*pdjhuff_ct++=0xFF;
		*pdjhuff_ct++=JPG_APP9;

		lfp=pdjhuff_ct;
		*pdjhuff_ct++=0x00;	//length
		*pdjhuff_ct++=0x00;
		*pdjhuff_ct++='J';
		*pdjhuff_ct++='P';
		*pdjhuff_ct++='S';
		*pdjhuff_ct++='E';
		*pdjhuff_ct++='R';
		*pdjhuff_ct++='M';
		*pdjhuff_ct++=0;

		i=pdjhuff_ct-lfp;
		lfp[0]=i>>8;	//length
		lfp[1]=i&0xFF;
	}

	if(ctx->jpg_clrtrans==BGBBTJ_JPG_CLRS_RGB)
	{
		*pdjhuff_ct++=0xFF;
		*pdjhuff_ct++=JPG_APP14;

		lfp=pdjhuff_ct;
		*pdjhuff_ct++=0x00;	//length
		*pdjhuff_ct++=0x00;

		*pdjhuff_ct++='A';
		*pdjhuff_ct++='d';
		*pdjhuff_ct++='o';
		*pdjhuff_ct++='b';
		*pdjhuff_ct++='e';

		i=100;
		*pdjhuff_ct++=(i>>8)&0xFF;
		*pdjhuff_ct++=i&0xFF;

		i=0;
		*pdjhuff_ct++=(i>>24)&0xFF;
		*pdjhuff_ct++=(i>>16)&0xFF;
		*pdjhuff_ct++=(i>>8)&0xFF;
		*pdjhuff_ct++=i&0xFF;

		*pdjhuff_ct++=0;	//RGB
//		*pdjhuff_ct++=ctx->bcst_bpp;

		i=pdjhuff_ct-lfp;
		lfp[0]=i>>8;	//length
		lfp[1]=i&0xFF;
	}

	ctx->bcst_version=0x0100;
	ctx->bcst_flags=0;
	ctx->bcst_trans=ctx->jpg_clrtrans;
	ctx->bcst_bpp=8;

	if(ctx->jpg_rdct)
		ctx->bcst_flags|=BGBBTJ_JPG_BCSFL_RDCT;

	//check if not to bother with marker...
	if(!ctx->bcst_flags && !ctx->bcst_trans)
	{
//		printf("BGBBTJ_JPG_EmitMarkerBCST: No Emit\n");
		return;
	}

//	printf("BGBBTJ_JPG_EmitMarkerBCST: Emit\n");

	*pdjhuff_ct++=0xFF;
	*pdjhuff_ct++=JPG_APP12;

	lfp=pdjhuff_ct;
	*pdjhuff_ct++=0x00;	//length
	*pdjhuff_ct++=0x00;

	*pdjhuff_ct++='B';
	*pdjhuff_ct++='C';
	*pdjhuff_ct++='S';
	*pdjhuff_ct++='T';

	*pdjhuff_ct++=(ctx->bcst_version>>8)&0xFF;
	*pdjhuff_ct++=ctx->bcst_version&0xFF;

	*pdjhuff_ct++=ctx->bcst_trans;
	*pdjhuff_ct++=ctx->bcst_bpp;

	*pdjhuff_ct++=(ctx->bcst_flags>>24)&0xFF;
	*pdjhuff_ct++=(ctx->bcst_flags>>16)&0xFF;
	*pdjhuff_ct++=(ctx->bcst_flags>>8)&0xFF;
	*pdjhuff_ct++=ctx->bcst_flags&0xFF;


	i=pdjhuff_ct-lfp;
	lfp[0]=i>>8;	//length
	lfp[1]=i&0xFF;
}

void BGBBTJ_JPG_EmitMarkerInlineAlpha(BGBBTJ_JPG_Context *ctx)
{
	if(ctx->badb)
	{
		pdjhuff_ct=BTIC1_EmitLumpAPP12Z(
			pdjhuff_ct, BTIC1_FCC_BA1Z,
			ctx->badb, ctx->sz_badb);
	}
}

BGBBTJ_API int BGBBTJ_JPG_EncodeCtx_Prepare(
	BGBBTJ_JPG_Context *ctx, int xs, int ys, int qfl, int pf)
{
	int xs2, ys2, xs3, ys3;
	int cr, cg, cb, cy, cu, cv;
	int i, j, k, l, qf;

	qf=qfl&255;

	ctx->jpg_is420=1;

	if((qf==110) || (qf==111) || (qfl&BGBBTJ_JPG_QFL_444) ||
			(qf==102) || (qf==103))
		ctx->jpg_is420=0;

	if(qfl&BGBBTJ_QFL_410)
	{
		ctx->jpg_is420=0;
		ctx->jpg_is410=1;
	}

//	ctx->jpg_is444=0;

	if(ctx->jpg_is420)
	{
//		xs2=((xs+7)/8)*8;
//		ys2=((ys+7)/8)*8;
//		xs3=(xs2+1)/2;
//		ys3=(ys2+1)/2;

		xs2=((xs+15)/16)*16;
		ys2=((ys+15)/16)*16;

		xs3=((xs+15)/16)*8;
		ys3=((ys+15)/16)*8;
		ctx->jpg_is444=0;
		ctx->jpg_is410=0;
//		printf("BGBBTJ_JPG_EncodeCtx_Prepare: 4:2:0 %dx%d\n", xs, ys);
	}else if(ctx->jpg_is410)
	{
		xs2=((xs+7)/8)*8;
		ys2=((ys+7)/8)*8;
//		xs3=(xs2+1)/2;
//		ys3=(ys2+1)/2;

		xs3=((xs+31)/32)*8;
		ys3=((ys+31)/32)*8;
		ctx->jpg_is444=0;
		ctx->jpg_is420=0;
//		printf("BGBBTJ_JPG_EncodeCtx_Prepare: 4:1:0 %dx%d\n", xs, ys);
	}else
	{
		xs2=((xs+7)/8)*8;
		ys2=((ys+7)/8)*8;
		xs3=xs2;
		ys3=ys2;
		ctx->jpg_is444=1;
		ctx->jpg_is410=0;
		ctx->jpg_is420=0;
//		printf("BGBBTJ_JPG_EncodeCtx_Prepare: 4:4:4 %dx%d\n", xs, ys);
	}
	
	ctx->xs=xs;
	ctx->ys=ys;
	ctx->xs2=xs2;
	ctx->ys2=ys2;
	ctx->xs3=xs3;
	ctx->ys3=ys3;

	ctx->jpg_mono=0;
	ctx->jpg_yuvw=0;
	ctx->jpg_tabcacheframe=0;
	ctx->xs=xs;
	ctx->ys=ys;

	if(pf==BGBBTJ_JPG_YYYA)
		ctx->jpg_mono=1;

	ctx->jpg_rdct=0;
	if((qf==110) || (qf==111) || (qfl&BGBBTJ_JPG_QFL_RDCT))
		ctx->jpg_rdct=1;

	ctx->jpg_clrtrans=0;
	if((qf==110) || (qfl&BGBBTJ_JPG_QFL_ORCT) || (qf==101) || (qf==103))
		ctx->jpg_clrtrans=BGBBTJ_JPG_CLRS_ORCT;
	if((qf==111) || (qfl&BGBBTJ_JPG_QFL_RGB) || (qf==104))
		ctx->jpg_clrtrans=BGBBTJ_JPG_CLRS_RGB;

	if(!(qfl&BGBBTJ_QFL_PFRAME) && !(qfl&BGBBTJ_QFL_SUBLAYER))
//	if(1)
	{
		//full quality
		for(i=0; i<64; i++)ctx->jpg_qt[0][i]=1;
		for(i=0; i<64; i++)ctx->jpg_qt[1][i]=1;

		//dummy huffman tables
		for(i=0; i<256; i++)ctx->huff_len[0*256+i]=0;
		for(i=0; i<256; i++)ctx->huff_len[1*256+i]=0;
		for(i=0; i<256; i++)ctx->huff_len[2*256+i]=0;
		for(i=0; i<256; i++)ctx->huff_len[3*256+i]=0;

		for(i=0; i<16; i++)ctx->huff_len[0*256+i]=4;
		for(i=0; i<16; i++)ctx->huff_len[2*256+i]=4;
		for(i=0; i<255; i++)ctx->huff_len[1*256+i]=8;
		for(i=0; i<255; i++)ctx->huff_len[3*256+i]=8;

//		for(i=0; i<255; i++)ctx->huff_len[0*256+i]=8;
//		for(i=0; i<255; i++)ctx->huff_len[2*256+i]=8;
	}


//	ctx->huff_len[1*256+0]=7;

//	printf("M0\n");

	//make sure intermediate buffers are present and the correct size
	if(!ctx->yb || (xs!=ctx->lxs) || (ys!=ctx->lys))
	{
		if(ctx->yb)
		{
			free(ctx->yb);
			free(ctx->ub);
			free(ctx->vb);
			free(ctx->ydb);
			free(ctx->udb);
			free(ctx->vdb);
		}

		//W channel is rare, and so is handled specially
		if(ctx->wb)
		{
			free(ctx->wb);
			free(ctx->wdb);
		}

//		if(ctx->bab)
//		{
//			free(ctx->bab);
//			ctx->bab=NULL;
//		}
		
		if(ctx->badb)
		{
			free(ctx->badb);
			ctx->badb=NULL;
		}

//		ctx->yb=malloc(xs2*ys2);
//		ctx->ub=malloc(xs2*ys2);
//		ctx->vb=malloc(xs2*ys2);

//		ctx->yb=malloc(xs2*ys2*sizeof(short));
//		ctx->ub=malloc(xs2*ys2*sizeof(short));
//		ctx->vb=malloc(xs2*ys2*sizeof(short));

		ctx->yb=malloc((xs2+16)*(ys2+16)*sizeof(short));
		ctx->ub=malloc((xs2+8)*(ys2+8)*sizeof(short));
		ctx->vb=malloc((xs2+8)*(ys2+8)*sizeof(short));

		ctx->ydb=malloc((xs2+16)*(ys2+16)*sizeof(short));
//		ctx->udb=malloc((xs3+8)*(ys3+8)*sizeof(short));
//		ctx->vdb=malloc((xs3+8)*(ys3+8)*sizeof(short));
		ctx->udb=malloc((xs2+8)*(ys2+8)*sizeof(short));
		ctx->vdb=malloc((xs2+8)*(ys2+8)*sizeof(short));

		if(ctx->jpg_yuvw)
		{
			ctx->wb=malloc((xs2+8)*(ys2+8)*sizeof(short));
			ctx->wdb=malloc((xs2+8)*(ys2+8)*sizeof(short));
		}

		ctx->lxs=xs;
		ctx->lys=ys;
	}

	if(qfl&BGBBTJ_QFL_BITALPHA)
	{
//		if(!ctx->bab)
//		{
//			i=(xs2/8)*(ys2/8)*8;
//			ctx->bab=malloc(i);
//			memset(ctx->bab, 0, i);
//		}

		if(!ctx->badb)
		{
			i=(xs2/8)*(ys2/8)*8;
			j=((xs2+8)/8)*((ys2+8)/8)*8;
			ctx->badb=malloc(j);
			ctx->sz_badb=i;
			memset(ctx->badb, 0, i);
		}
	}

//	memset(ctx->yb, 128, xs2*ys2);
//	memset(ctx->ub, 128, xs2*ys2);
//	memset(ctx->vb, 128, xs2*ys2);

	memset(ctx->yb, 128, xs2*ys2*sizeof(short));
	memset(ctx->ub, 128, xs2*ys2*sizeof(short));
	memset(ctx->vb, 128, xs2*ys2*sizeof(short));

	memset(ctx->ydb, 0, xs2*(ys2+8)*sizeof(short));
	memset(ctx->udb, 0, xs3*(ys3+8)*sizeof(short));
	memset(ctx->vdb, 0, xs3*(ys3+8)*sizeof(short));

	if(ctx->jpg_yuvw)
	{
		memset(ctx->wb, 128, xs2*ys2*sizeof(short));
		memset(ctx->wdb, 0, xs2*(ys2+8)*sizeof(short));
	}
	return(0);
}

BGBBTJ_API int BGBBTJ_JPG_EncodeCtx_ColorTransform(
	BGBBTJ_JPG_Context *ctx, byte *ibuf, int xs, int ys, int pf)
{
	int xs2, ys2, xs3, ys3;
	int cr, cg, cb, ca, cy, cu, cv, cw;
	int i, j, k, l, n, qf;

	xs2=ctx->xs2;
	ys2=ctx->ys2;
	xs3=ctx->xs3;
	ys3=ctx->ys3;

	if((xs!=xs2) || (ys!=ys2))
	{
		n=xs2*ys2;
		for(i=0; i<n; i++)
			{ ctx->yb[i]=128; }
//		memset(ctx->yb, 128, xs2*ys2);
	}
	if((xs!=xs3) || (ys!=ys3))
	{
		n=xs3*ys3;
		for(i=0; i<n; i++)
			{ ctx->ub[i]=128; ctx->vb[i]=128; }
		if(ctx->wb)
		{
			for(i=0; i<n; i++)
				{ ctx->wb[i]=128; }
		}

//		memset(ctx->ub, 128, xs3*ys3);
//		memset(ctx->vb, 128, xs3*ys3);
//		if(ctx->wb)
//			{ memset(ctx->wb, 128, xs3*ys3); }
	}

	for(i=0; i<ys; i++)
		for(j=0; j<xs; j++)
	{
		k=ys-(i+1);
		cr=ibuf[(k*xs+j)*4+0];
		cg=ibuf[(k*xs+j)*4+1];
		cb=ibuf[(k*xs+j)*4+2];
		ca=ibuf[(k*xs+j)*4+2];

//		if(qf==110)
		if(ctx->jpg_clrtrans==BGBBTJ_JPG_CLRS_YCBCR)
		{
			cy=0.299*cr	+0.587*cg	+0.114*cb;
			cu=-0.1687*cr	-0.3313*cg	+0.5*cb 	+128;
			cv=0.5*cr	-0.4187*cg	-0.0813*cb	+128;
			cw=ca;

			cy=(cy<0)?0:((cy>255)?255:cy);
			cu=(cu<0)?0:((cu>255)?255:cu);
			cv=(cv<0)?0:((cv>255)?255:cv);
		}else if(ctx->jpg_clrtrans==BGBBTJ_JPG_CLRS_ORCT)
		{
//			cy=(cr+2*cg+cb)/4;
			cy=cg+(cb+cr-2*cg)/4;
			cu=(cb-cg)+128;
			cv=(cr-cg)+128;
			cw=ca;
//			k=cy-(cu+cv-256)/4;
//			cy+=cg-k;
//			if(k!=cg)cy++;

//			cu=cb-cr;
//			cv=cg-cr-0.5*cu;
//			cy=cg-0.5*cv;
//			cu+=128; cv+=128;
		}else if(ctx->jpg_clrtrans==BGBBTJ_JPG_CLRS_RGB)
		{
			cy=cr;
			cu=cg;
			cv=cb;
			cw=ca;
		}else
		{
			cy=0.299*cr	+0.587*cg	+0.114*cb;
			cu=-0.1687*cr	-0.3313*cg	+0.5*cb 	+128;
			cv=0.5*cr	-0.4187*cg	-0.0813*cb	+128;
			cw=ca;
		}

//		cu=128; cv=128;

//		cy=(cy<0)?0:((cy>255)?255:cy);
//		cu=(cu<0)?0:((cu>255)?255:cu);
//		cv=(cv<0)?0:((cv>255)?255:cv);

		ctx->yb[i*xs2+j]=cy;
		ctx->ub[i*xs2+j]=cu;
		ctx->vb[i*xs2+j]=cv;
		if(ctx->jpg_yuvw)
			ctx->wb[i*xs2+j]=cw;

		if(ctx->badb)
		{
			k=(i>>3)*(xs2>>3)+(j>>3);
			if(ca>=128)
			{
				ctx->badb[k*8+(i&7)]|=128>>(j&7);
			}
		}
	}
	return(0);
}

BGBBTJ_API int BGBBTJ_JPG_EncodeCtx_ColorTransform16(
	BGBBTJ_JPG_Context *ctx, short *ibuf, int xs, int ys, int pf)
{
	int xs2, ys2, xs3, ys3;
	int cr, cg, cb, ca, cy, cu, cv, cw;
	int i, j, k, l, qf;

	xs2=ctx->xs2;
	ys2=ctx->ys2;
	xs3=ctx->xs3;
	ys3=ctx->ys3;

	for(i=0; i<ys; i++)
		for(j=0; j<xs; j++)
	{
		k=ys-(i+1);
		cr=ibuf[(k*xs+j)*4+0];
		cg=ibuf[(k*xs+j)*4+1];
		cb=ibuf[(k*xs+j)*4+2];
		ca=ibuf[(k*xs+j)*4+2];

//		if(qf==110)
		if(ctx->jpg_clrtrans==BGBBTJ_JPG_CLRS_YCBCR)
		{
			cy=0.299*cr	+0.587*cg	+0.114*cb;
			cu=-0.1687*cr	-0.3313*cg	+0.5*cb 	+128;
			cv=0.5*cr	-0.4187*cg	-0.0813*cb	+128;
			cw=ca;

			//cy=(cy<0)?0:((cy>255)?255:cy);
			//cu=(cu<0)?0:((cu>255)?255:cu);
			//cv=(cv<0)?0:((cv>255)?255:cv);
		}else if(ctx->jpg_clrtrans==BGBBTJ_JPG_CLRS_ORCT)
		{
//			cy=(cr+2*cg+cb)/4;
			cy=cg+(cb+cr-2*cg)/4;
			cu=(cb-cg)+128;
			cv=(cr-cg)+128;
			cw=ca;
		}else if(ctx->jpg_clrtrans==BGBBTJ_JPG_CLRS_RGB)
		{
			cy=cr; cu=cg; cv=cb; cw=ca;
		}else
		{
			cy=0.299*cr	+0.587*cg	+0.114*cb;
			cu=-0.1687*cr	-0.3313*cg	+0.5*cb 	+128;
			cv=0.5*cr	-0.4187*cg	-0.0813*cb	+128;
			cw=ca;
		}

//		cu=128; cv=128;

//		cy=(cy<0)?0:((cy>255)?255:cy);
//		cu=(cu<0)?0:((cu>255)?255:cu);
//		cv=(cv<0)?0:((cv>255)?255:cv);

		ctx->yb[i*xs2+j]=cy;
		ctx->ub[i*xs2+j]=cu;
		ctx->vb[i*xs2+j]=cv;
		if(ctx->jpg_yuvw)
			ctx->wb[i*xs2+j]=cw;

		if(ctx->badb)
		{
			k=(i>>3)*(xs2>>3)+(j>>3);
			if(ca>=128)
			{
				ctx->badb[k*8+(i&7)]|=128>>(j&7);
			}
		}
	}
	return(0);
}

BGBBTJ_API int BGBBTJ_JPG_EncodeCtx_DownSample(BGBBTJ_JPG_Context *ctx)
{
	int xs2, ys2, xs3, ys3;
	int i, j, k, l;

	xs2=ctx->xs2;
	ys2=ctx->ys2;
	xs3=ctx->xs3;
	ys3=ctx->ys3;

//	if((xs3!=xs2) || (ys3!=ys2))
	if(ctx->jpg_is420)
	{
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

			if(ctx->jpg_yuvw)
			{
				k=ctx->wb[(i*2)*xs2+j*2]+
					ctx->wb[(i*2)*xs2+j*2+1]+
					ctx->wb[(i*2+1)*xs2+j*2]+
					ctx->wb[(i*2+1)*xs2+j*2+1];
				ctx->wb[i*xs3+j]=k/4;
			}
		}
	}

	if(ctx->jpg_is410)
	{
		for(i=0; i<ys3; i++)
			for(j=0; j<xs3; j++)
		{
			k=	ctx->ub[(i*4+0)*xs2+j*4+0]+ctx->ub[(i*4+0)*xs2+j*4+1]+
				ctx->ub[(i*4+0)*xs2+j*4+2]+ctx->ub[(i*4+0)*xs2+j*4+3]+
				ctx->ub[(i*4+1)*xs2+j*4+0]+ctx->ub[(i*4+1)*xs2+j*4+1]+
				ctx->ub[(i*4+1)*xs2+j*4+2]+ctx->ub[(i*4+1)*xs2+j*4+3]+
				ctx->ub[(i*4+2)*xs2+j*4+0]+ctx->ub[(i*4+2)*xs2+j*4+1]+
				ctx->ub[(i*4+2)*xs2+j*4+2]+ctx->ub[(i*4+2)*xs2+j*4+3]+
				ctx->ub[(i*4+3)*xs2+j*4+0]+ctx->ub[(i*4+3)*xs2+j*4+1]+
				ctx->ub[(i*4+3)*xs2+j*4+2]+ctx->ub[(i*4+3)*xs2+j*4+3];
//			ctx->ub[i*xs3+j]=(k+8)>>4;
			ctx->ub[i*xs3+j]=k>>4;

			k=	ctx->vb[(i*4+0)*xs2+j*4+0]+ctx->vb[(i*4+0)*xs2+j*4+1]+
				ctx->vb[(i*4+0)*xs2+j*4+2]+ctx->vb[(i*4+0)*xs2+j*4+3]+
				ctx->vb[(i*4+1)*xs2+j*4+0]+ctx->vb[(i*4+1)*xs2+j*4+1]+
				ctx->vb[(i*4+1)*xs2+j*4+2]+ctx->vb[(i*4+1)*xs2+j*4+3]+
				ctx->vb[(i*4+2)*xs2+j*4+0]+ctx->vb[(i*4+2)*xs2+j*4+1]+
				ctx->vb[(i*4+2)*xs2+j*4+2]+ctx->vb[(i*4+2)*xs2+j*4+3]+
				ctx->vb[(i*4+3)*xs2+j*4+0]+ctx->vb[(i*4+3)*xs2+j*4+1]+
				ctx->vb[(i*4+3)*xs2+j*4+2]+ctx->vb[(i*4+3)*xs2+j*4+3];
//			ctx->vb[i*xs3+j]=(k+8)>>4;
			ctx->vb[i*xs3+j]=k>>4;

			if(ctx->jpg_yuvw)
			{
				k=	ctx->wb[(i*4+0)*xs2+j*4+0]+ctx->wb[(i*4+0)*xs2+j*4+1]+
					ctx->wb[(i*4+0)*xs2+j*4+2]+ctx->wb[(i*4+0)*xs2+j*4+3]+
					ctx->wb[(i*4+1)*xs2+j*4+0]+ctx->wb[(i*4+1)*xs2+j*4+1]+
					ctx->wb[(i*4+1)*xs2+j*4+2]+ctx->wb[(i*4+1)*xs2+j*4+3]+
					ctx->wb[(i*4+2)*xs2+j*4+0]+ctx->wb[(i*4+2)*xs2+j*4+1]+
					ctx->wb[(i*4+2)*xs2+j*4+2]+ctx->wb[(i*4+2)*xs2+j*4+3]+
					ctx->wb[(i*4+3)*xs2+j*4+0]+ctx->wb[(i*4+3)*xs2+j*4+1]+
					ctx->wb[(i*4+3)*xs2+j*4+2]+ctx->wb[(i*4+3)*xs2+j*4+3];
//				ctx->wb[i*xs3+j]=(k+8)>>4;
				ctx->wb[i*xs3+j]=k>>4;
			}
		}
	}
	return(0);
}

BGBBTJ_API int BGBBTJ_JPG_EncodeCtx(
	BGBBTJ_JPG_Context *ctx, byte *ibuf, byte *obuf, int xs, int ys,
	int qfl, int pf)
{
	int i;
	BGBBTJ_JPG_EncodeCtx_Prepare(ctx, xs, ys, qfl, pf);
	BGBBTJ_JPG_EncodeCtx_ColorTransform(ctx, ibuf, xs, ys, pf);
	BGBBTJ_JPG_EncodeCtx_DownSample(ctx);
	i=BGBBTJ_JPG_EncodeCtx_Inner(ctx, obuf, xs, ys, qfl);
	return(i);
}

BGBBTJ_API int BGBBTJ_JPG_EncodeCtxSS(
	BGBBTJ_JPG_Context *ctx, short *ibuf, byte *obuf, int xs, int ys,
	int qfl, int pf)
{
	int i;
	BGBBTJ_JPG_EncodeCtx_Prepare(ctx, xs, ys, qfl, pf);
	BGBBTJ_JPG_EncodeCtx_ColorTransform16(ctx, ibuf, xs, ys, pf);
	BGBBTJ_JPG_EncodeCtx_DownSample(ctx);
	i=BGBBTJ_JPG_EncodeCtx_Inner(ctx, obuf, xs, ys, qfl);
	return(i);
}

BGBBTJ_API int BGBBTJ_JPG_EncodeCtx_Inner(
	BGBBTJ_JPG_Context *ctx, byte *obuf, int xs, int ys, int qfl)
{
//	static byte *yb=NULL, *ub, *vb;
//	static short *ydb=NULL, *udb, *vdb;
//	static int lxs=0, lys=0;

	int dcs[256], acs[256];
	int dcsuv[256], acsuv[256];
	short *tp;
	int xs2, ys2, xs3, ys3, qf;
	int cr, cg, cb, cy, cu, cv;
	int i, j, k, l, m, n;

	qf=qfl&255;

//	qfl&=~BGBBTJ_QFL_PFRAME;
//	qfl&=~BGBBTJ_QFL_IFRAME;

	xs2=ctx->xs2;
	ys2=ctx->ys2;
	xs3=ctx->xs3;
	ys3=ctx->ys3;

//	printf("M1\n");

//	if(qf==110)
	if(ctx->jpg_rdct)
	{
//		printf("BGBBTJ_JPG_EncodeCtx: Lossless Detect\n");
	
		BGBBTJ_JPG_FilterImageRDCT(ctx->yb, ctx->ydb, xs2, ys2, 128);
		BGBBTJ_JPG_FilterImageRDCT(ctx->ub, ctx->udb, xs3, ys3, 128);
		BGBBTJ_JPG_FilterImageRDCT(ctx->vb, ctx->vdb, xs3, ys3, 128);
	}else
	{
		BGBBTJ_JPG_FilterImageDCT(ctx->yb, ctx->ydb, xs2, ys2, 128);
		BGBBTJ_JPG_FilterImageDCT(ctx->ub, ctx->udb, xs3, ys3, 128);
		BGBBTJ_JPG_FilterImageDCT(ctx->vb, ctx->vdb, xs3, ys3, 128);
	}

	if(!(qfl&BGBBTJ_QFL_PFRAME) && !(qfl&BGBBTJ_QFL_SUBLAYER))
	{
		j=(xs2/8)*(ys2/8);
		BGBBTJ_JPG_MakeQuantTabInput(ctx->ydb, j, ctx->jpg_qt[0], qf/100.0);

		j=(xs3/8)*(ys3/8);
		BGBBTJ_JPG_MakeQuantTabInput(ctx->udb, j, ctx->jpg_qt[1], qf/100.0);
		BGBBTJ_JPG_MakeQuantTabInput(ctx->vdb, j, ctx->jpg_qt[2], qf/100.0);
		for(i=0; i<64; i++)
		{
			ctx->jpg_qt[1][i]=(ctx->jpg_qt[1][i]+ctx->jpg_qt[2][i])/2;
		}

		BGBBTJ_JPG_SetupQuantTabDivFP(ctx, 0);
		BGBBTJ_JPG_SetupQuantTabDivFP(ctx, 1);
	}

//	free(yb);
//	free(ub);
//	free(vb);


//	if(ctx->jpg_mono || !ctx->jpg_is420)
	if(ctx->jpg_mono || ctx->jpg_is444)
	{
		j=(xs2/8)*(ys2/8); k=0;
		for(i=0; i<j; i++)
		{
			BGBBTJ_Huff_QuantBlock(ctx, ctx->ydb+i*64, ctx->ydb+i*64, 0);
			ctx->ydb[i*64+0]-=k; k=ctx->ydb[i*64+0]+k;
		}
	}

	if(!ctx->jpg_mono && ctx->jpg_is420)
	{
		l=0;
		for(i=0; i<=(ys3/8); i++)
			for(j=0; j<(xs3/8); j++)
		{
			tp=ctx->ydb+((i*2+0)*(xs2/8)+j*2+0)*64;
			BGBBTJ_Huff_QuantBlock(ctx, tp, tp, 0);
			tp[0]-=l; l=tp[0]+l;

			tp=ctx->ydb+((i*2+0)*(xs2/8)+j*2+1)*64;
			BGBBTJ_Huff_QuantBlock(ctx, tp, tp, 0);
			tp[0]-=l; l=tp[0]+l;

			tp=ctx->ydb+((i*2+1)*(xs2/8)+j*2+0)*64;
			BGBBTJ_Huff_QuantBlock(ctx, tp, tp, 0);
			tp[0]-=l; l=tp[0]+l;

			tp=ctx->ydb+((i*2+1)*(xs2/8)+j*2+1)*64;
			BGBBTJ_Huff_QuantBlock(ctx, tp, tp, 0);
			tp[0]-=l; l=tp[0]+l;
		}
	}

	if(ctx->jpg_is410)
	{
		m=0;
		for(i=0; i<=(ys3/8); i++)
			for(j=0; j<(xs3/8); j++)
		{
			for(k=0; k<4; k++)
				for(l=0; l<4; l++)
			{
				tp=ctx->ydb+((i*4+k)*(xs2/8)+j*4+l)*64;
				BGBBTJ_Huff_QuantBlock(ctx, tp, tp, 0);
				tp[0]-=m; m=tp[0]+m;
			}
		}
	}

#if 0
	if(!ctx->jpg_mono && !ctx->jpg_is420)
	{
		l=0;
		for(i=0; i<=(ys3/8); i++)
			for(j=0; j<(xs3/8); j++)
		{
			BGBBTJ_Huff_QuantBlock(ctx, ctx->ydb+i*64, ctx->ydb+i*64, 1);
			ctx->ydb[i*64+0]-=l; l=ctx->ydb[i*64+0]+l;
		}
	}
#endif

	j=(xs3/8)*(ys3/8); k=0; l=0;
	for(i=0; i<j; i++)
	{
		BGBBTJ_Huff_QuantBlock(ctx, ctx->udb+i*64, ctx->udb+i*64, 1);
		BGBBTJ_Huff_QuantBlock(ctx, ctx->vdb+i*64, ctx->vdb+i*64, 1);
		ctx->udb[i*64+0]-=k; k=ctx->udb[i*64+0]+k;
		ctx->vdb[i*64+0]-=l; l=ctx->vdb[i*64+0]+l;
	}

	if(ctx->jpg_yuvw)
	{
		j=(xs3/8)*(ys3/8); k=0;
		for(i=0; i<j; i++)
		{
			BGBBTJ_Huff_QuantBlock(ctx, ctx->wdb+i*64, ctx->wdb+i*64, 1);
			ctx->wdb[i*64+0]-=k; k=ctx->wdb[i*64+0]+k;
		}
	}

//	printf("M2\n");

	if(!(qfl&BGBBTJ_QFL_PFRAME) && !(qfl&BGBBTJ_QFL_SUBLAYER))
	{
		if((qfl&BGBBTJ_QFL_IFRAME) || (qfl&BGBBTJ_QFL_TAGBASELAYER))
//		if(0)
		{
			for(i=0; i<16; i++)dcs[i]=1;
			for(i=16; i<256; i++)dcs[i]=0;
			for(i=0; i<256; i++)acs[i]=1;

			for(i=0; i<16; i++)dcsuv[i]=1;
			for(i=16; i<256; i++)dcsuv[i]=0;
			for(i=0; i<256; i++)acsuv[i]=1;
		}else
		{
			for(i=0; i<256; i++)dcs[i]=0;
			for(i=0; i<256; i++)acs[i]=0;
			for(i=0; i<256; i++)dcsuv[i]=0;
			for(i=0; i<256; i++)acsuv[i]=0;
		}

		j=(xs2/8)*(ys2/8);
		k=(xs3/8)*(ys3/8);
		for(i=0; i<j; i++)BGBBTJ_Huff_StatBlock(ctx->ydb+i*64, dcs, acs);
		for(i=0; i<k; i++)BGBBTJ_Huff_StatBlock(ctx->udb+i*64, dcsuv, acsuv);
		for(i=0; i<k; i++)BGBBTJ_Huff_StatBlock(ctx->vdb+i*64, dcsuv, acsuv);
	
		if(ctx->jpg_yuvw)
			{ for(i=0; i<k; i++)BGBBTJ_Huff_StatBlock(
				ctx->wdb+i*64, dcsuv, acsuv); }

//		BGBBTJ_Huff_BuildLengths(dcs, 256, ctx->huff_len+0*256, 16);
//		BGBBTJ_Huff_BuildLengths(acs, 256, ctx->huff_len+1*256, 16);
//		BGBBTJ_Huff_BuildLengths(dcsuv, 256, ctx->huff_len+2*256, 16);
//		BGBBTJ_Huff_BuildLengths(acsuv, 256, ctx->huff_len+3*256, 16);

		BGBBTJ_Huff_BuildLengthsAdjust(dcs, 256, ctx->huff_len+0*256, 16);
		BGBBTJ_Huff_BuildLengthsAdjust(acs, 256, ctx->huff_len+1*256, 16);
		BGBBTJ_Huff_BuildLengthsAdjust(dcsuv, 256, ctx->huff_len+2*256, 16);
		BGBBTJ_Huff_BuildLengthsAdjust(acsuv, 256, ctx->huff_len+3*256, 16);
	}

	pdjhuff_ct=obuf;
	pdjhuff_win=0;
	pdjhuff_pos=0;

	if(!(qfl&BGBBTJ_QFL_NOSOI))
	{
		*pdjhuff_ct++=0xFF;
		*pdjhuff_ct++=JPG_SOI;
	}

	BGBBTJ_JPG_EmitMarkerBCST(ctx);

	if(!(qfl&BGBBTJ_QFL_PFRAME) && !(qfl&BGBBTJ_QFL_SUBLAYER))
//	if(1)
	{
		BGBBTJ_JPG_EmitDQT(ctx, 0);
		if(!ctx->jpg_mono)BGBBTJ_JPG_EmitDQT(ctx, 1);
	}

	BGBBTJ_JPG_EmitSOF(ctx, xs, ys);

	BGBBTJ_JPG_EmitMarkerInlineAlpha(ctx);

	if(!(qfl&BGBBTJ_QFL_PFRAME) && !(qfl&BGBBTJ_QFL_SUBLAYER))
//	if(1)
	{
		BGBBTJ_JPG_EmitDHT(ctx, 0);
		BGBBTJ_JPG_EmitDHT(ctx, 1);
		if(!ctx->jpg_mono)
		{
			BGBBTJ_JPG_EmitDHT(ctx, 2);
			BGBBTJ_JPG_EmitDHT(ctx, 3);
		}
	}

	BGBBTJ_JPG_EmitSOS(ctx);

	pdjhuff_win=0;
	pdjhuff_pos=0;

	if(ctx->jpg_mono)
	{
		j=(xs2/8)*(ys2/8);
		for(i=0; i<j; i++)BGBBTJ_Huff_EncodeBlock(ctx, ctx->ydb+i*64, 0, 1);
	}else if(ctx->jpg_is420)
	{
		for(i=0; i<=(ys3/8); i++)
			for(j=0; j<(xs3/8); j++)
		{
			BGBBTJ_Huff_EncodeBlock(ctx,
				ctx->ydb+((i*2+0)*(xs2/8)+j*2+0)*64, 0, 1);
			BGBBTJ_Huff_EncodeBlock(ctx,
				ctx->ydb+((i*2+0)*(xs2/8)+j*2+1)*64, 0, 1);
			BGBBTJ_Huff_EncodeBlock(ctx,
				ctx->ydb+((i*2+1)*(xs2/8)+j*2+0)*64, 0, 1);
			BGBBTJ_Huff_EncodeBlock(ctx,
				ctx->ydb+((i*2+1)*(xs2/8)+j*2+1)*64, 0, 1);

			k=i*(xs3/8)+j;
			BGBBTJ_Huff_EncodeBlock(ctx, ctx->udb+k*64, 2, 3);
			BGBBTJ_Huff_EncodeBlock(ctx, ctx->vdb+k*64, 2, 3);
			if(ctx->jpg_yuvw)
				{ BGBBTJ_Huff_EncodeBlock(ctx, ctx->wdb+k*64, 2, 3); }
		}
	}else if(ctx->jpg_is410)
	{
		for(i=0; i<=(ys3/8); i++)
			for(j=0; j<(xs3/8); j++)
		{
			for(k=0; k<4; k++)
				for(l=0; l<4; l++)
			{
				BGBBTJ_Huff_EncodeBlock(ctx,
					ctx->ydb+((i*4+k)*(xs2/8)+j*4+l)*64, 0, 1);
			}

			k=i*(xs3/8)+j;
			BGBBTJ_Huff_EncodeBlock(ctx, ctx->udb+k*64, 2, 3);
			BGBBTJ_Huff_EncodeBlock(ctx, ctx->vdb+k*64, 2, 3);
			if(ctx->jpg_yuvw)
				{ BGBBTJ_Huff_EncodeBlock(ctx, ctx->wdb+k*64, 2, 3); }
		}
	}else
	{
		j=(xs2/8)*(ys2/8);
		for(i=0; i<j; i++)
		{
			BGBBTJ_Huff_EncodeBlock(ctx, ctx->ydb+i*64, 0, 1);
			BGBBTJ_Huff_EncodeBlock(ctx, ctx->udb+i*64, 2, 3);
			BGBBTJ_Huff_EncodeBlock(ctx, ctx->vdb+i*64, 2, 3);
			if(ctx->jpg_yuvw)
				{ BGBBTJ_Huff_EncodeBlock(ctx, ctx->wdb+i*64, 2, 3); }
		}

#if 0
		for(i=0; i<=(ys3/8); i++)
			for(j=0; j<(xs3/8); j++)
		{
			k=i*(xs3/8)+j;
			BGBBTJ_Huff_EncodeBlock(ctx, ctx->ydb+k*64, 0, 1);
			BGBBTJ_Huff_EncodeBlock(ctx, ctx->udb+k*64, 2, 3);
			BGBBTJ_Huff_EncodeBlock(ctx, ctx->vdb+k*64, 2, 3);
		}
#endif
	}

	BGBBTJ_Huff_FlushBits(ctx);

	if(!(qfl&BGBBTJ_QFL_NOSOI))
	{
		*pdjhuff_ct++=0xFF;
		*pdjhuff_ct++=JPG_EOI;
	}

//	free(ydb);
//	free(udb);
//	free(vdb);

//	printf("M3\n");

	i=pdjhuff_ct-obuf;

	return(i);
}

BGBBTJ_API BGBBTJ_JPG_Context *BGBBTJ_JPG_AllocContext()
{
	BGBBTJ_JPG_Context *ctx;

	BGBBTJ_JPG_Init();
	ctx=malloc(sizeof(BGBBTJ_JPG_Context));
	memset(ctx, 0, sizeof(BGBBTJ_JPG_Context));
	return(ctx);
}

BGBBTJ_API void BGBBTJ_JPG_FreeContext(BGBBTJ_JPG_Context *ctx)
{
	if(ctx->yb)
	{
		free(ctx->yb);
		free(ctx->ub);
		free(ctx->vb);
		free(ctx->ydb);
		free(ctx->udb);
		free(ctx->vdb);
	}

	if(ctx->jpg_imgbuf)
		free(ctx->jpg_imgbuf);

	if(ctx->imgbuf_rgba)
		free(ctx->imgbuf_rgba);
	if(ctx->imgbuf_norm)
		free(ctx->imgbuf_norm);
	if(ctx->imgbuf_spec)
		free(ctx->imgbuf_spec);
	if(ctx->imgbuf_luma)
		free(ctx->imgbuf_luma);
	if(ctx->imgbuf_lebe)
		free(ctx->imgbuf_lebe);

	free(ctx);
}

BGBBTJ_API int BGBBTJ_JPG_Encode(
	byte *ibuf, byte *obuf, int xs, int ys, int qf)
{
	BGBBTJ_JPG_Context *ctx;
	int sz;
	
	ctx=BGBBTJ_JPG_AllocContext();
	sz=BGBBTJ_JPG_EncodeCtx(ctx, ibuf, obuf, xs, ys, qf, 0);
	BGBBTJ_JPG_FreeContext(ctx);
	return(sz);
}

BGBBTJ_API void BGBBTJ_JPG_FlushEncodeFast(BGBBTJ_JPG_Context *ctx)
{
	if(!ctx)return;
	ctx->jpg_tabcacheframe=0;	//force rebuild
	ctx->oldAlphaClr=0;
}

BGBBTJ_API void BGBBTJ_JPG_SetContextAlphaColor(BGBBTJ_JPG_Context *ctx,
	int cr, int cg, int cb, int ca)
{
	int cy, cu, cv;
	if(!ctx)return;

	if(ca<1)
	{
		ctx->alphaClr=0;
		return;
	}

	ctx->alphaClr=cr+(cg<<8)+(cb<<16)+(ca<<24);

	cy= 19595*cr +38470*cg + 7471*cb;
	cu=-11056*cr -21712*cg +32768*cb;
	cv= 32768*cr -27440*cg - 5328*cb;
	ctx->alphaClrY=cy>>16;
	ctx->alphaClrU=(cu>>16)+128;
	ctx->alphaClrV=(cv>>16)+128;
	ctx->alphaClrA=ca;
}

#if 0
//static void pdjpg_getPixel2_RGBA(byte *rgb,
//		int *ra, int *ga, int *ba,
//		int *rb, int *gb, int *bb)
//{
//	*ra=rgb[0]; *ga=rgb[1]; *ba=rgb[2];
//	*ra=rgb[4]; *ga=rgb[5]; *ba=rgb[6];
//}
#endif

void BGBBTJ_JPG_ConvertImageYUV(BGBBTJ_JPG_Context *ctx,
	byte *ibuf, int xs, int ys, int pf,
	int xs2, int ys2, int xs3, int ys3)
{
//	void (*getPixel2)(byte *rgb,
//		int *ra, int *ga, int *ba,
//		int *rb, int *gb, int *bb);
	byte *cs, *cse;
	short *cty, *ctu, *ctv;
	byte *cs1, *cs2;
	short *cty1, *cty2;

	int cr, cg, cb, ca, cy, cu, cv;

	int cra, cga, cba, caa, cya, cua, cva;
	int crb, cgb, cbb, cab, cyb, cub, cvb;
	int crc, cgc, cbc, cac, cyc, cuc, cvc;
	int crd, cgd, cbd, cad, cyd, cud, cvd;
	int psz, psz2;

	int i, j, k, l, n;

	if((pf==BGBBTJ_JPG_RGBA) && !ctx->alphaClr)
	{
		n=(ys+1)/2;
		for(i=0; i<n; i++)
		{
			k=((ys-(2*i+1))*xs)*4;
			cs1=ibuf+k; cse=cs1+xs*4;
			l=((ys-(2*i+2))*xs)*4;
			cs2=ibuf+l;

			cty1=ctx->yb+(2*i+0)*xs2;
			cty2=ctx->yb+(2*i+1)*xs2;

			l=i*xs3;
			ctu=ctx->ub+l;
			ctv=ctx->vb+l;

			while(cs1<cse)
			{
				cra=*(cs1+0); cga=*(cs1+1); cba=*(cs1+2);
				crb=*(cs1+4); cgb=*(cs1+5); cbb=*(cs1+6);
				crc=*(cs2+0); cgc=*(cs2+1); cbc=*(cs2+2);
				crd=*(cs2+4); cgd=*(cs2+5); cbd=*(cs2+6);

				cya=19595*cra + 38470*cga + 7471*cba;
				cyb=19595*crb + 38470*cgb + 7471*cbb;
				cyc=19595*crc + 38470*cgc + 7471*cbc;
				cyd=19595*crd + 38470*cgd + 7471*cbd;
				*cty1++=cya>>16;
				*cty1++=cyb>>16;
				*cty2++=cyc>>16;
				*cty2++=cyd>>16;

				cr=(cra+crb+crc+crd)>>2;
				cg=(cga+cgb+cgc+cgd)>>2;
				cb=(cba+cbb+cbc+cbd)>>2;

				cu=-11056*cr -21712*cg +32768*cb;
				cv= 32768*cr -27440*cg - 5328*cb;
				*ctu++=(cu>>16)+128;
				*ctv++=(cv>>16)+128;

				cs1+=8; cs2+=8;
			}
		}
	}else
	if((pf==BGBBTJ_JPG_RGBA) || (pf==BGBBTJ_JPG_RGB) ||
		(pf==BGBBTJ_JPG_BGRA) || (pf==BGBBTJ_JPG_BGR))
	{
		switch(pf)
		{
		case BGBBTJ_JPG_RGBA: case BGBBTJ_JPG_BGRA:
			psz=4; break;
		case BGBBTJ_JPG_RGB: case BGBBTJ_JPG_BGR:
			psz=3; break;
		default: psz=4; break;
		}
		psz2=psz*2;

		n=(ys+1)/2;
		for(i=0; i<n; i++)
		{
			k=((ys-(2*i+1))*xs)*psz;
			cs1=ibuf+k; cse=cs1+xs*psz;
			l=((ys-(2*i+2))*xs)*psz;
			cs2=ibuf+l;

			cty1=ctx->yb+(2*i+0)*xs2;
			cty2=ctx->yb+(2*i+1)*xs2;

			l=i*xs3;
			ctu=ctx->ub+l;
			ctv=ctx->vb+l;

			while(cs1<cse)
			{
//				getPixel2(cs1, &cra, &cga, &cba, &crb, &cgb, &cbb);
//				getPixel2(cs2, &crc, &cgc, &cbc, &crd, &cgd, &cbd);

				switch(pf)
				{
				case BGBBTJ_JPG_RGBA:
					cra=*(cs1+0); cga=*(cs1+1); cba=*(cs1+2); caa=*(cs1+3);
					crb=*(cs1+4); cgb=*(cs1+5); cbb=*(cs1+6); cab=*(cs1+7);
					crc=*(cs2+0); cgc=*(cs2+1); cbc=*(cs2+2); cac=*(cs2+3);
					crd=*(cs2+4); cgd=*(cs2+5); cbd=*(cs2+6); cad=*(cs2+7);
					psz=8; break;
				case BGBBTJ_JPG_BGRA:
					cra=*(cs1+2); cga=*(cs1+1); cba=*(cs1+0); caa=*(cs1+3);
					crb=*(cs1+6); cgb=*(cs1+5); cbb=*(cs1+4); cab=*(cs1+7);
					crc=*(cs2+2); cgc=*(cs2+1); cbc=*(cs2+0); cac=*(cs2+3);
					crd=*(cs2+6); cgd=*(cs2+5); cbd=*(cs2+4); cad=*(cs2+7);
					psz=8; break;
				case BGBBTJ_JPG_RGB:
					cra=*(cs1+0); cga=*(cs1+1); cba=*(cs1+2); caa=255;
					crb=*(cs1+3); cgb=*(cs1+4); cbb=*(cs1+5); cab=255;
					crc=*(cs2+0); cgc=*(cs2+1); cbc=*(cs2+2); cac=255;
					crd=*(cs2+3); cgd=*(cs2+4); cbd=*(cs2+5); cad=255;
					psz=6; break;
				case BGBBTJ_JPG_BGR:
					cra=*(cs1+2); cga=*(cs1+1); cba=*(cs1+0); caa=255;
					crb=*(cs1+5); cgb=*(cs1+4); cbb=*(cs1+3); cab=255;
					crc=*(cs2+2); cgc=*(cs2+1); cbc=*(cs2+0); cac=255;
					crd=*(cs2+5); cgd=*(cs2+4); cbd=*(cs2+3); cad=255;
					psz=6; break;
				}

				if(ctx->alphaClr)
				{
					ca=(caa+cab+cac+cad)>>2;
					if(ca<ctx->alphaClrA)
					{
						cy=ctx->alphaClrY;
						cu=ctx->alphaClrU;
						cv=ctx->alphaClrV;
						*cty1++=cy; *cty1++=cy;
						*cty2++=cy; *cty2++=cy;
						*ctu++=cu; *ctv++=cv;
						continue;
					}
				}

				cya=19595*cra + 38470*cga + 7471*cba;
				cyb=19595*crb + 38470*cgb + 7471*cbb;
				cyc=19595*crc + 38470*cgc + 7471*cbc;
				cyd=19595*crd + 38470*cgd + 7471*cbd;
				*cty1++=cya>>16;
				*cty1++=cyb>>16;
				*cty2++=cyc>>16;
				*cty2++=cyd>>16;

				cr=(cra+crb+crc+crd)>>2;
				cg=(cga+cgb+cgc+cgd)>>2;
				cb=(cba+cbb+cbc+cbd)>>2;

				cu=-11056*cr -21712*cg +32768*cb;
				cv= 32768*cr -27440*cg - 5328*cb;
				*ctu++=(cu>>16)+128;
				*ctv++=(cv>>16)+128;

				cs1+=psz2; cs2+=psz2;
			}
		}
	}else
	{
		switch(pf)
		{
		case BGBBTJ_JPG_YUVA: psz=4; psz2=8; break;
		case BGBBTJ_JPG_YUV: psz=3; psz2=6; break;
		case BGBBTJ_JPG_YUV422: psz=2; psz2=4; break;
		case BGBBTJ_JPG_YUV420: psz=3; psz2=3; break;
		case BGBBTJ_JPG_YA: psz=2; psz2=4; break;
		case BGBBTJ_JPG_Y: psz=1; psz2=2; break;
		case BGBBTJ_JPG_YYYA: psz=4; psz2=8; break;
		default: psz=4; psz2=8; break;
		}

		n=(ys+1)/2;
		for(i=0; i<n; i++)
		{
			if(pf==BGBBTJ_JPG_YUV420)
			{
				k=(((ys-(2*i+1))*xs2)*psz2)>>1;
				cs1=ibuf+k; cse=cs1+((xs*psz2)>>1);
				l=(((ys-(2*i+2))*xs2)*psz2)>>1;
				cs2=ibuf+l;
			}else
			{
				k=((ys-(2*i+1))*xs)*psz;
				cs1=ibuf+k; cse=cs1+xs*psz;
				l=((ys-(2*i+2))*xs)*psz;
				cs2=ibuf+l;
			}

			cty1=ctx->yb+(2*i+0)*xs2;
			cty2=ctx->yb+(2*i+1)*xs2;

			l=i*xs3;
			ctu=ctx->ub+l;
			ctv=ctx->vb+l;

			while(cs1<cse)
			{
//				getPixel2(cs1, &cra, &cga, &cba, &crb, &cgb, &cbb);
//				getPixel2(cs2, &crc, &cgc, &cbc, &crd, &cgd, &cbd);

				switch(pf)
				{
				case BGBBTJ_JPG_YUVA:
					cya=*(cs1+0); cua=*(cs1+1); cva=*(cs1+2);
					cyb=*(cs1+4); cub=*(cs1+5); cvb=*(cs1+6);
					cyc=*(cs2+0); cuc=*(cs2+1); cvc=*(cs2+2);
					cyd=*(cs2+4); cud=*(cs2+5); cvd=*(cs2+6);
					cu=(cua+cub+cuc+cud)>>2;
					cv=(cva+cvb+cvc+cvd)>>2;
					break;
				case BGBBTJ_JPG_YUV:
					cya=*(cs1+0); cua=*(cs1+1); cva=*(cs1+2);
					cyb=*(cs1+3); cub=*(cs1+4); cvb=*(cs1+5);
					cyc=*(cs2+0); cuc=*(cs2+1); cvc=*(cs2+2);
					cyd=*(cs2+3); cud=*(cs2+4); cvd=*(cs2+5);
					cu=(cua+cub+cuc+cud)>>2;
					cv=(cva+cvb+cvc+cvd)>>2;
					break;
				case BGBBTJ_JPG_YUV422:
					cya=*(cs1+0); cua=*(cs1+1); cyb=*(cs1+2); cva=*(cs1+3);
					cyc=*(cs2+0); cuc=*(cs2+1); cyd=*(cs2+2); cvc=*(cs2+3);
					cu=(cua+cuc)>>1; cv=(cva+cvc)>>1;
					break;
				case BGBBTJ_JPG_YUV420:
					cya=*(cs1+0); cyb=*(cs1+1); cu=*(cs1+2);
					cyc=*(cs2+0); cyd=*(cs2+1); cv=*(cs2+2);
					break;
				case BGBBTJ_JPG_YA:
					cya=*(cs1+0); cyb=*(cs1+2);
					cyc=*(cs2+0); cyd=*(cs2+2);
					cu=0; cv=0;
					break;
				case BGBBTJ_JPG_Y:
					cya=*(cs1+0); cyb=*(cs1+1);
					cyc=*(cs2+0); cyd=*(cs2+1);
					cu=0; cv=0;
					break;

				case BGBBTJ_JPG_YYYA:
					cya=*(cs1+1); cyb=*(cs1+5);
					cyc=*(cs2+1); cyd=*(cs2+5);
					cu=0; cv=0;
					break;
				}

				*cty1++=cya; *cty1++=cyb;
				*cty2++=cyc; *cty2++=cyd;
				*ctu++=cu; *ctv++=cv;

				cs1+=psz2; cs2+=psz2;
			}
		}
	}
}

BGBBTJ_API int BGBBTJ_JPG_EncodeFastCtx(BGBBTJ_JPG_Context *ctx,
	byte *ibuf, byte *obuf, int xs, int ys, int qf, int pf)
{
	char tb[256];
	short *tp;
//	byte *cs, *cse, *cty, *ctu, *ctv;
	int xs2, ys2, xs3, ys3;
//	int cr, cg, cb, cy, cu, cv;

//	int cra, cga, cba, cya;
//	int crb, cgb, cbb, cyb;
//	int crc, cgc, cbc, cyc;
//	int crd, cgd, cbd, cyd;
//	byte *cs1, *cs2, *cty1, *cty2;
	byte *ctt;

	int i, j, k, l, n;

	xs2=((xs+7)/8)*8;
	ys2=((ys+7)/8)*8;
	xs3=((xs+15)/16)*8;
	ys3=((ys+15)/16)*8;

	ctx->jpg_mono=0;

	if(qf&BGBBTJ_QFL_IFRAME)
	{
		ctx->jpg_tabcacheframe=0;
	}

	if(qf&BGBBTJ_QFL_PFRAME)
	{
//		ctx->jpg_tabcacheframe=16;
		ctx->jpg_tabcacheframe=8;
	}

	if(!ctx->jpg_tabcacheframe)
	{
		//full quality
		for(i=0; i<64; i++)ctx->jpg_qt[0][i]=1;
		for(i=0; i<64; i++)ctx->jpg_qt[1][i]=1;
	}

	if(!ctx->yb || (xs!=ctx->lxs) || (ys!=ctx->lys))
	{
		if(ctx->yb)
		{
			free(ctx->yb);
			free(ctx->ub);
			free(ctx->vb);
			free(ctx->ydb);
			free(ctx->udb);
			free(ctx->vdb);
		}

		ctx->yb=malloc(xs2*ys2);
		ctx->ub=malloc(xs2*ys2);
		ctx->vb=malloc(xs2*ys2);

		ctx->ydb=malloc((xs2+8)*(ys2+16)*sizeof(short));
		ctx->udb=malloc((xs3+8)*(ys3+8)*sizeof(short));
		ctx->vdb=malloc((xs3+8)*(ys3+8)*sizeof(short));

		ctx->lxs=xs;
		ctx->lys=ys;

		memset(ctx->yb, 128, xs2*ys2);
		memset(ctx->ub, 128, xs2*ys2);
		memset(ctx->vb, 128, xs2*ys2);

		memset(ctx->ydb, 0, xs2*(ys2+8)*sizeof(short));
		memset(ctx->udb, 0, xs3*(ys3+8)*sizeof(short));
		memset(ctx->vdb, 0, xs3*(ys3+8)*sizeof(short));

		ctx->jpg_tabcacheframe=0;	//force rebuild
	}

	BGBBTJ_JPG_ConvertImageYUV(ctx,
		ibuf, xs, ys, pf,
		xs2, ys2, xs3, ys3);

	BGBBTJ_JPG_FilterImageDCT(ctx->yb, ctx->ydb, xs2, ys2, 128);
	BGBBTJ_JPG_FilterImageDCT(ctx->ub, ctx->udb, xs3, ys3, 128);
	BGBBTJ_JPG_FilterImageDCT(ctx->vb, ctx->vdb, xs3, ys3, 128);

	if(ctx->jpg_tabcacheframe<=0)
	{
		j=(xs2/8)*(ys2/8);
		BGBBTJ_JPG_MakeQuantTabInputFast(
			ctx->ydb, j, ctx->jpg_qt[0], qf/100.0);

		j=(xs3/8)*(ys3/8);
		BGBBTJ_JPG_MakeQuantTabInputFast(
			ctx->udb, j, ctx->jpg_qt[1], qf/100.0);
		BGBBTJ_JPG_MakeQuantTabInputFast(
			ctx->vdb, j, ctx->jpg_qt[2], qf/100.0);
		for(i=0; i<64; i++)
			ctx->jpg_qt[1][i]=(ctx->jpg_qt[1][i]+ctx->jpg_qt[2][i])/2;

		BGBBTJ_JPG_SetupQuantTabDivFP(ctx, 0);
		BGBBTJ_JPG_SetupQuantTabDivFP(ctx, 1);
	}

	if(ctx->jpg_mono)
	{
		j=(xs2/8)*(ys2/8); k=0;
		for(i=0; i<j; i++)
		{
			BGBBTJ_Huff_QuantBlock(ctx, ctx->ydb+i*64, ctx->ydb+i*64, 0);
			ctx->ydb[i*64+0]-=k;
			k=ctx->ydb[i*64+0]+k;
		}
	}

	if(!ctx->jpg_mono)
	{
		l=0;
		for(i=0; i<=(ys3/8); i++)
			for(j=0; j<(xs3/8); j++)
		{
			tp=ctx->ydb+((i*2+0)*(xs2/8)+j*2+0)*64;
			BGBBTJ_Huff_QuantBlock(ctx, tp, tp, 0);
			tp[0]-=l; l=tp[0]+l;

			tp=ctx->ydb+((i*2+0)*(xs2/8)+j*2+1)*64;
			BGBBTJ_Huff_QuantBlock(ctx, tp, tp, 0);
			tp[0]-=l; l=tp[0]+l;

			tp=ctx->ydb+((i*2+1)*(xs2/8)+j*2+0)*64;
			BGBBTJ_Huff_QuantBlock(ctx, tp, tp, 0);
			tp[0]-=l; l=tp[0]+l;

			tp=ctx->ydb+((i*2+1)*(xs2/8)+j*2+1)*64;
			BGBBTJ_Huff_QuantBlock(ctx, tp, tp, 0);
			tp[0]-=l; l=tp[0]+l;
		}
	}

	j=(xs3/8)*(ys3/8); k=0; l=0;
	for(i=0; i<j; i++)
	{
		BGBBTJ_Huff_QuantBlock(ctx, ctx->udb+i*64, ctx->udb+i*64, 1);
		BGBBTJ_Huff_QuantBlock(ctx, ctx->vdb+i*64, ctx->vdb+i*64, 1);
		ctx->udb[i*64+0]-=k; k=ctx->udb[i*64+0]+k;
		ctx->vdb[i*64+0]-=l; l=ctx->vdb[i*64+0]+l;
	}

//	printf("M2\n");

	if(ctx->jpg_tabcacheframe<=0)
	{
		for(i=0; i<256; i++)ctx->dcs[i]=1;
		for(i=0; i<256; i++)ctx->acs[i]=1;
		for(i=0; i<256; i++)ctx->dcsuv[i]=1;
		for(i=0; i<256; i++)ctx->acsuv[i]=1;

		j=(xs2/8)*(ys2/8);
		k=(xs3/8)*(ys3/8);
		for(i=0; i<j; i++)
			BGBBTJ_Huff_StatBlock(ctx->ydb+i*64, ctx->dcs, ctx->acs);
		for(i=0; i<k; i++)
			BGBBTJ_Huff_StatBlock(ctx->udb+i*64, ctx->dcsuv, ctx->acsuv);
		for(i=0; i<k; i++)
			BGBBTJ_Huff_StatBlock(ctx->vdb+i*64, ctx->dcsuv, ctx->acsuv);

//		BGBBTJ_Huff_BuildLengths(ctx->dcs, 256, ctx->huff_len+0*256, 16);
//		BGBBTJ_Huff_BuildLengths(ctx->acs, 256, ctx->huff_len+1*256, 16);
//		BGBBTJ_Huff_BuildLengths(ctx->dcsuv, 256, ctx->huff_len+2*256, 16);
//		BGBBTJ_Huff_BuildLengths(ctx->acsuv, 256, ctx->huff_len+3*256, 16);

		BGBBTJ_Huff_BuildLengthsAdjust(
			ctx->dcs, 256, ctx->huff_len+0*256, 16);
		BGBBTJ_Huff_BuildLengthsAdjust(
			ctx->acs, 256, ctx->huff_len+1*256, 16);
		BGBBTJ_Huff_BuildLengthsAdjust(
			ctx->dcsuv, 256, ctx->huff_len+2*256, 16);
		BGBBTJ_Huff_BuildLengthsAdjust(
			ctx->acsuv, 256, ctx->huff_len+3*256, 16);
	}

	pdjhuff_ct=obuf;
	pdjhuff_win=0;
	pdjhuff_pos=0;

	*pdjhuff_ct++=0xFF;
	*pdjhuff_ct++=JPG_SOI;

	if(ctx->alphaClr && (ctx->alphaClr!=ctx->oldAlphaClr))
	{
		sprintf(tb, "%d %d %d %d",
			(ctx->alphaClr&0xFF),
			((ctx->alphaClr>>8)&0xFF),
			((ctx->alphaClr>>16)&0xFF),
			((ctx->alphaClr>>24)&0xFF));
	
		*pdjhuff_ct++=0xFF;
		*pdjhuff_ct++=JPG_APP11;
		ctt=pdjhuff_ct;
		*pdjhuff_ct++=0x00;
		*pdjhuff_ct++=0x00;
		BGBBTJ_Huff_WriteString(ctx, "AlphaColor");
		BGBBTJ_Huff_WriteString(ctx, tb);
		i=(pdjhuff_ct-ctt);
		ctt[0]=(i>>8)&0xFF; ctt[1]=i&0xFF;
	}


	if(ctx->jpg_tabcacheframe<=0)
	{
		BGBBTJ_JPG_EmitDQT(ctx, 0);
		if(!ctx->jpg_mono)BGBBTJ_JPG_EmitDQT(ctx, 1);
	}

	BGBBTJ_JPG_EmitSOF(ctx, xs, ys);

//	if(ctx->jpg_tabcacheframe<=0)
	if(1)
	{
		BGBBTJ_JPG_EmitDHT(ctx, 0);
		BGBBTJ_JPG_EmitDHT(ctx, 1);
		if(!ctx->jpg_mono)
		{
			BGBBTJ_JPG_EmitDHT(ctx, 2);
			BGBBTJ_JPG_EmitDHT(ctx, 3);
		}
	}

	BGBBTJ_JPG_EmitSOS(ctx);

	pdjhuff_win=0;
	pdjhuff_pos=0;

	if(ctx->jpg_mono)
	{
		j=(xs2/8)*(ys2/8);
		for(i=0; i<j; i++)BGBBTJ_Huff_EncodeBlock(ctx, ctx->ydb+i*64, 0, 1);
	}else
	{
		for(i=0; i<=((ys3)/8); i++)
			for(j=0; j<(xs3/8); j++)
		{
			BGBBTJ_Huff_EncodeBlock(ctx,
				ctx->ydb+((i*2+0)*(xs2/8)+j*2+0)*64, 0, 1);
			BGBBTJ_Huff_EncodeBlock(ctx,
				ctx->ydb+((i*2+0)*(xs2/8)+j*2+1)*64, 0, 1);
			BGBBTJ_Huff_EncodeBlock(ctx,
				ctx->ydb+((i*2+1)*(xs2/8)+j*2+0)*64, 0, 1);
			BGBBTJ_Huff_EncodeBlock(ctx,
				ctx->ydb+((i*2+1)*(xs2/8)+j*2+1)*64, 0, 1);

			k=i*(xs3/8)+j;
			BGBBTJ_Huff_EncodeBlock(ctx, ctx->udb+k*64, 2, 3);
			BGBBTJ_Huff_EncodeBlock(ctx, ctx->vdb+k*64, 2, 3);
		}
	}

	BGBBTJ_Huff_FlushBits(ctx);

	*pdjhuff_ct++=0xFF;
	*pdjhuff_ct++=JPG_EOI;

	if(ctx->jpg_tabcacheframe<=0)
	{
//		ctx->jpg_tabcacheframe=16;
		ctx->jpg_tabcacheframe=8;
	}else
	{
		ctx->jpg_tabcacheframe--;
	}

//	printf("M3\n");

	i=pdjhuff_ct-obuf;

	return(i);
}

BGBBTJ_API int BGBBTJ_JPG_EncodeFast(
	byte *ibuf, byte *obuf, int xs, int ys, int qf)
{
	static BGBBTJ_JPG_Context *ctx=NULL;
	int sz;
	
	if(!ctx)ctx=BGBBTJ_JPG_AllocContext();
	sz=BGBBTJ_JPG_EncodeFastCtx(ctx, ibuf, obuf, xs, ys, qf, 0);
//	BGBBTJ_JPG_FreeContext(ctx);
	return(sz);
}

byte *BGBBTJ_JPG_EmitComponentLayer(BGBBTJ_JPG_Context *ctx,
	byte *ct, char *name)
{
	byte *ctt;
	int i;

	pdjhuff_ct=ct; pdjhuff_win=0; pdjhuff_pos=0;
	*pdjhuff_ct++=0xFF;
	*pdjhuff_ct++=JPG_APP11;
	ctt=pdjhuff_ct;
	*pdjhuff_ct++=0x00;
	*pdjhuff_ct++=0x00;
	BGBBTJ_Huff_WriteString(ctx, "CompLayer");
	BGBBTJ_Huff_WriteString(ctx, name);
	i=pdjhuff_ct-ctt;
	ctt[0]=(i>>8)&0xFF; ctt[1]=i&0xFF;
	return(pdjhuff_ct);
}

byte *BGBBTJ_JPG_EmitMarkerJFIF(BGBBTJ_JPG_Context *ctx, byte *ct)
{
	byte *ctt;
	int i;

	if(ctx->jpg_clrtrans!=BGBBTJ_JPG_CLRS_YCBCR)
		return(ct);

	pdjhuff_ct=ct; pdjhuff_win=0; pdjhuff_pos=0;
	*pdjhuff_ct++=0xFF;
	*pdjhuff_ct++=JPG_APP0;
	ctt=pdjhuff_ct;
	*pdjhuff_ct++=0x00;
	*pdjhuff_ct++=0x00;
	BGBBTJ_Huff_WriteString(ctx, "JFIF");

	*pdjhuff_ct++=0x01;		//version high
	*pdjhuff_ct++=0x02;		//version low

	*pdjhuff_ct++=0x00;		//no units

	*pdjhuff_ct++=0x00;		//X density
	*pdjhuff_ct++=0x01;

	*pdjhuff_ct++=0x00;		//Y density
	*pdjhuff_ct++=0x01;

	*pdjhuff_ct++=0x00;		//thumbnail
	*pdjhuff_ct++=0x00;

//	BGBBTJ_Huff_WriteString(ctx, name);
	i=pdjhuff_ct-ctt;
	ctt[0]=(i>>8)&0xFF; ctt[1]=i&0xFF;
	return(pdjhuff_ct);
}

BGBBTJ_API int BGBBTJ_JPG_EscapeEncodeBuffer(byte *ibuf, int isz,
	byte *obuf, int osz)
{
	byte *cs, *ct, *cse, *cte;
	
	cs=ibuf; cse=ibuf+isz;
	ct=obuf; cte=obuf+osz;
	
	while((cs<cse) && (ct<cte))
	{
		if(*cs==0xFF)
		{
			cs++;
			*ct++=0xFF;
			*ct++=0x00;
			continue;
		}
		
		*ct++=*cs++;
	}
	
	if(ct>=cte)return(-1);
	return(ct-obuf);
}

BGBBTJ_API int BGBBTJ_JPG_EscapeEncodeSingleBuffer(byte *buf, int sz)
{
	byte *tbuf;
	int i;
	
	tbuf=malloc(sz*2);
	i=BGBBTJ_JPG_EscapeEncodeBuffer(buf, sz, tbuf, sz*2);
	if(i<0)return(i);
	memcpy(buf, tbuf, i);
	free(tbuf);
	return(i);
}

BGBBTJ_API int BGBBTJ_JPG_EscapeDecodeBuffer(byte *ibuf, int isz,
	byte *obuf, int osz)
{
	byte *cs, *ct, *cse, *cte;
	
	cs=ibuf; cse=ibuf+isz;
	ct=obuf; cte=obuf+osz;
	
	while((cs<cse) && (ct<cte))
	{
		if((cs[0]==0xFF) && (cs[1]==0x00))
			{ cs+=2; *ct++=0xFF; continue; }
		*ct++=*cs++;
	}
	if(ct>=cte)return(-1);
	return(ct-obuf);
}

BGBBTJ_API int BGBBTJ_JPG_EscapeDecodeSingleBuffer(byte *buf, int sz)
{
	byte *cs, *ct, *cse, *cte;
	
	cs=buf; cse=buf+sz;
	ct=buf; cte=buf+sz;
	
	while((cs<cse) && (ct<cte))
	{
		if((cs[0]==0xFF) && (cs[1]==0x00))
			{ cs+=2; *ct++=0xFF; continue; }
		*ct++=*cs++;
	}
	if(ct>=cte)return(-1);
	return(ct-buf);
}

BGBBTJ_API int BGBBTJ_JPG_EncodeLDatCtx(
	BGBBTJ_JPG_Context *ctx, byte *ibuf, byte *obuf,
	int xs, int ys, int qf, int pf)
{
	byte *tbuf, *cts;
	int i, j, sz, tsz;
	
//	if(!ctx)ctx=BGBBTJ_JPG_AllocContext();

	tbuf=malloc(1<<24);
	sz=BGBBTJ_JPG_EncodeCtx(ctx, ibuf, tbuf, xs, ys, qf, pf);

	sz=BGBBTJ_JPG_EscapeEncodeSingleBuffer(tbuf, sz);

	cts=tbuf; tsz=sz;
	pdjhuff_ct=obuf; pdjhuff_win=0; pdjhuff_pos=0;
	while(tsz>=65528)
	{
		i=65529;
		if(cts[i-1]==0xFF)i--;
		j=i+6;
		*pdjhuff_ct++=0xFF;
		*pdjhuff_ct++=JPG_APP12;
		*pdjhuff_ct++=(j>>8)&0xFF;
		*pdjhuff_ct++=j&0xFF;

		*pdjhuff_ct++='L';
		*pdjhuff_ct++='D';
		*pdjhuff_ct++='A';
		*pdjhuff_ct++='T';
		
		memcpy(pdjhuff_ct, cts, i);
		cts+=i; pdjhuff_ct+=i;
		tsz-=i;
	}
	
	if((tsz>0) && (tsz<65528))
	{
		i=tsz+6;
		*pdjhuff_ct++=0xFF;
		*pdjhuff_ct++=JPG_APP12;
		*pdjhuff_ct++=(i>>8)&0xFF;
		*pdjhuff_ct++=i&0xFF;

		*pdjhuff_ct++='L';
		*pdjhuff_ct++='D';
		*pdjhuff_ct++='A';
		*pdjhuff_ct++='T';
		
		memcpy(pdjhuff_ct, cts, tsz);
		pdjhuff_ct+=tsz;
	}

	sz=pdjhuff_ct-obuf;
	free(tbuf);
	
	return(sz);
}

BGBBTJ_API int BGBBTJ_JPG_EncodeBaseCtx(
	BGBBTJ_JPG_Context *ctx, byte *ibuf, byte *obuf,
	int xs, int ys, int qf, int pf)
{
	byte *tbuf, *cs, *cse, *ct;
	int sz;

	tbuf=malloc(1<<24);
	sz=BGBBTJ_JPG_EncodeCtx(ctx, ibuf, tbuf, xs, ys, qf, pf);
	
	cs=tbuf; cse=tbuf+sz; ct=obuf;
	if((cs[0]==0xFF) && (cs[1]==JPG_SOI))
		cs+=2;
	while(cs<cse)
	{
		if((cs[0]==0xFF) && (cs[1]==JPG_EOI))
			break;
		*ct++=*cs++;
	}
	
	free(tbuf);
	return(ct-obuf);
}

BGBBTJ_API int BGBBTJ_JPG_EncodeComponentCtx(
	BGBBTJ_JPG_Context *ctx,
	byte *rgba, byte *norm, byte *spec, byte *luma,
	byte *obuf, int xs, int ys, int qf)
{
	return(BGBBTJ_JPG_EncodeComponentCtxI(ctx,
		rgba, norm, spec, luma, NULL,
		obuf, xs, ys, qf));
}

BGBBTJ_API int BGBBTJ_JPG_EncodeComponentCtxI(
	BGBBTJ_JPG_Context *ctx,
	byte *rgba, byte *norm, byte *spec, byte *luma, byte *lebe,
	byte *obuf, int xs, int ys, int qf)
{
	byte *tbuf;
	byte *ct;
	int i, j, k, n, qf1;

	if(!rgba)return(-1);

	n=xs*ys;
	tbuf=malloc(xs*ys*4);

	ct=obuf;

	*ct++=0xFF;
	*ct++=JPG_SOI;

	ct=BGBBTJ_JPG_EmitMarkerJFIF(ctx, ct);

	qf1=qf|BGBBTJ_QFL_TAGBASELAYER;

	ct=BGBBTJ_JPG_EmitComponentLayer(ctx, ct, "RGB");
//	i=BGBBTJ_JPG_EncodeCtx(ctx, rgba, ct, xs, ys, qf, 0);
	i=BGBBTJ_JPG_EncodeBaseCtx(ctx, rgba, ct, xs, ys, qf1, 0);
	if(i<0) { free(tbuf); return(i); }
	ct+=i;
	
//	if((ct[-2]==0xFF) && (ct[-2]==JPG_EOI))
//		{ ct-=2; }
	
	qf1=qf|BGBBTJ_QFL_SUBLAYER;
	qf1&=~BGBBTJ_QFL_BITALPHA;
	
	if(norm)
	{
		for(i=0; i<n; i++)
		{
			tbuf[i*4+0]=norm[i*4+0];
			tbuf[i*4+1]=norm[i*4+2];
			tbuf[i*4+2]=norm[i*4+1];
			tbuf[i*4+3]=255;
		}

		ct=BGBBTJ_JPG_EmitComponentLayer(ctx, ct, "XYZ");
		i=BGBBTJ_JPG_EncodeLDatCtx(ctx, tbuf, ct, xs, ys, qf1, 0);
		if(i<0) { free(tbuf); return(i); }
		ct+=i;
	}

	if(spec)
	{
		ct=BGBBTJ_JPG_EmitComponentLayer(ctx, ct, "SpRGB");
		i=BGBBTJ_JPG_EncodeLDatCtx(ctx, spec, ct, xs, ys, qf1, 0);
		if(i<0) { free(tbuf); return(i); }
		ct+=i;
	}

	if(norm || spec)
	{
		for(i=0; i<n; i++)
		{
			j=rgba[i*4+3];
			tbuf[i*4+0]=norm?norm[i*4+3]:j;
			tbuf[i*4+1]=rgba[i*4+3];
			tbuf[i*4+2]=spec?spec[i*4+3]:j;
			tbuf[i*4+3]=255;
		}

		ct=BGBBTJ_JPG_EmitComponentLayer(ctx, ct, "DASe");
		i=BGBBTJ_JPG_EncodeLDatCtx(ctx, tbuf, ct, xs, ys, qf1, 0);
		if(i<0) { free(tbuf); return(i); }
		ct+=i;
	}else
	{
		k=0;
		for(i=0; i<n; i++)
		{
			j=rgba[i*4+3];
			if(j!=255)k=1;
			tbuf[i*4+0]=j;
			tbuf[i*4+1]=j;
			tbuf[i*4+2]=j;
			tbuf[i*4+3]=255;
		}

		if(k)
		{
			ct=BGBBTJ_JPG_EmitComponentLayer(ctx, ct, "Alpha");
			i=BGBBTJ_JPG_EncodeLDatCtx(ctx, tbuf,
				ct, xs, ys, qf1, BGBBTJ_JPG_YYYA);
			if(i<0) { free(tbuf); return(i); }
			ct+=i;
		}
	}

	if(luma)
	{
		ct=BGBBTJ_JPG_EmitComponentLayer(ctx, ct, "LuRGB");
		i=BGBBTJ_JPG_EncodeLDatCtx(ctx, luma, ct, xs, ys, qf1, 0);
		if(i<0) { free(tbuf); return(i); }
		ct+=i;
	}

	if(lebe)
	{
		ct=BGBBTJ_JPG_EmitComponentLayer(ctx, ct, "LeBe");
		i=BGBBTJ_JPG_EncodeLDatCtx(ctx, lebe, ct, xs, ys, qf1, 0);
		if(i<0) { free(tbuf); return(i); }
		ct+=i;
	}

	*ct++=0xFF;
	*ct++=JPG_EOI;

	free(tbuf);
	return(ct-obuf);
}

BGBBTJ_API int BGBBTJ_JPG_EncodeComponent(
	byte *rgba, byte *norm, byte *spec, byte *luma,
	byte *obuf, int xs, int ys, int qf)
{
	BGBBTJ_JPG_Context *ctx;
	int sz;
	
	ctx=BGBBTJ_JPG_AllocContext();
	sz=BGBBTJ_JPG_EncodeComponentCtx(ctx,
		rgba, norm, spec, luma,
		obuf, xs, ys, qf);
	BGBBTJ_JPG_FreeContext(ctx);
	return(sz);
}

float pdjpg_ssqr(float f)
	{ return((f>=0)?(f*f):(-f*f)); }
float pdjpg_ssqrt(float f)
	{ return((f>=0)?sqrt(f):(-sqrt(-f))); }

int BGBBTJ_JPG_CalcComponentValueExponent(float val)
{
	float f, g;
	int i, j;

	if(val>=0)
	{
		f=log(val)/log(2);
		i=pdjpg_ssqrt(f*240)+0.5;
		if(i>63)i=63;
		if(i<(-63))i=-63;
		j=192+i;
		return(j);
	}else
	{
		f=log(-val)/log(2);
		i=pdjpg_ssqrt(f*240)+0.5;
		if(i>63)i=63;
		if(i<(-63))i=-63;
		j=63-i;
		return(j);
	}
}

void BGBBTJ_JPG_EncodeFloatPixel(float *ipix, byte *opix, byte *oexp)
{
	float f, g;
	int i, j, k;

	if((ipix[0]>=0) && (ipix[0]<=1) &&
		(ipix[1]>=0) && (ipix[1]<=1) &&
		(ipix[2]>=0) && (ipix[2]<=1))
	{
		*oexp=192;
		opix[0]=ipix[0]*255;
		opix[1]=ipix[1]*255;
		opix[2]=ipix[2]*255;
		return;
	}
	
	f=ipix[1];
	if(fabs(ipix[0])>fabs(f))f=ipix[0];
	if(fabs(ipix[2])>fabs(f))f=ipix[2];
	
	if(f>=0)
	{
		f=log(f)/log(2);
		i=pdjpg_ssqrt(f*240)+0.5;
		if(i>63)i=63;
		if(i<(-63))i=-63;
		j=192+i;
		g=pdjpg_scltab[j];

		*oexp=j;
		f=255/g;
		k=ipix[0]*f; k=(k<0)?0:((k<=255)?k:255); opix[0]=k;
		k=ipix[1]*f; k=(k<0)?0:((k<=255)?k:255); opix[1]=k;
		k=ipix[2]*f; k=(k<0)?0:((k<=255)?k:255); opix[2]=k;
		return;
	}else
	{
		f=log(-f)/log(2);
		i=pdjpg_ssqrt(f*240)+0.5;
		if(i>63)i=63;
		if(i<(-63))i=-63;
		j=63-i;
		g=pdjpg_scltab[j];

		*oexp=j;
		f=255/g;
		k=ipix[0]*f; k=(k<0)?0:((k<=255)?k:255); opix[0]=k;
		k=ipix[1]*f; k=(k<0)?0:((k<=255)?k:255); opix[1]=k;
		k=ipix[2]*f; k=(k<0)?0:((k<=255)?k:255); opix[2]=k;
		return;
	}
}

BGBBTJ_API int BGBBTJ_JPG_EncodeFloatComponentCtx(BGBBTJ_JPG_Context *ctx,
	float *rgba, float *norm, float *spec, float *luma,
	byte *obuf, int xs, int ys, int qf)
{
	byte *trgba, *tnorm, *tspec, *tluma, *tlebe;
	int i, j, n;
	float f, g;

	n=xs*ys;
	if(!ctx->imgbuf_rgba)
		ctx->imgbuf_rgba=malloc(4*n);
	if(!ctx->imgbuf_norm)
		ctx->imgbuf_norm=malloc(4*n);
	if(!ctx->imgbuf_spec)
		ctx->imgbuf_spec=malloc(4*n);
	if(!ctx->imgbuf_luma)
		ctx->imgbuf_luma=malloc(4*n);

	if(!ctx->imgbuf_lebe)
	{
		ctx->imgbuf_lebe=malloc(4*n);
		memset(ctx->imgbuf_lebe, 192, 4*n);
	}
	
	trgba=ctx->imgbuf_rgba;
	tnorm=ctx->imgbuf_norm;
	tspec=ctx->imgbuf_spec;
	tluma=ctx->imgbuf_luma;
	tlebe=ctx->imgbuf_lebe;
	
	if(rgba)
	{
		for(i=0; i<n; i++)
		{
			BGBBTJ_JPG_EncodeFloatPixel(rgba, trgba+i*4, tlebe+i*4+0);
		}
	}

	if(norm)
	{
		for(i=0; i<n; i++)
		{
//			f=(1.0/256)*pdjpg_scltab[tlebe[i*4+0]];
			j=BGBBTJ_JPG_CalcComponentValueExponent(norm[i*4+3]);
			f=255/pdjpg_scltab[j];
			tnorm[i*4+0]=norm[i*4+0]*255;
			tnorm[i*4+1]=norm[i*4+1]*255;
			tnorm[i*4+2]=norm[i*4+2]*255;
			tnorm[i*4+3]=norm[i*4+3]*f;
			tlebe[i*4+3]=j;
		}
	}

	if(spec)
	{
		for(i=0; i<n; i++)
		{
			tspec[i*4+0]=spec[i*4+0]*255;
			tspec[i*4+1]=spec[i*4+1]*255;
			tspec[i*4+2]=spec[i*4+2]*255;
			tspec[i*4+3]=spec[i*4+3]*255;
		}
	}

	if(luma)
	{
		for(i=0; i<n; i++)
		{
			BGBBTJ_JPG_EncodeFloatPixel(luma, tluma+i*4, tlebe+i*4+1);
		}
	}

	i=BGBBTJ_JPG_EncodeComponentCtxI(ctx,
		trgba, tnorm, tspec, tluma, tlebe,
		obuf, xs, ys, qf);
	return(i);
}
