#include <bgbmid.h>

/*
Filters based of Band Transforms.

Idea:
Split sample into center/side channels;
Convert center/side into interpolated values (4bit);
Run these points through a frequency transform (DCT or WHT);
Map these onto bit-patterns.
*/

void BGBMID_BTAC_Band0WHT64(int *iblk, int *oblk, int dc)
{
	int tblk[64];
	int i0, i1, i2;
	int i, j, k;

	for(i=0; i<32; i++)
	{
		i0=iblk[i*2+0]-dc;
		i1=iblk[i*2+1]-dc;
		i2=i0+i1;
		tblk[i]=i2;
		oblk[32+i]=(i2>>1)-i1;
	}

	for(i=0; i<16; i++)
	{
		i0=tblk[i*2+0]; i1=tblk[i*2+1]; i2=i0+i1;
		tblk[i]=i2;	oblk[16+i]=(i2>>1)-i1;
	}

	for(i=0; i<8; i++)
	{
		i0=tblk[i*2+0]; i1=tblk[i*2+1]; i2=i0+i1;
		tblk[i]=i2;	oblk[8+i]=(i2>>1)-i1;
	}

	for(i=0; i<4; i++)
	{
		i0=tblk[i*2+0]; i1=tblk[i*2+1]; i2=i0+i1;
		tblk[i]=i2;	oblk[4+i]=(i2>>1)-i1;
	}

	for(i=0; i<2; i++)
	{
		i0=tblk[i*2+0]; i1=tblk[i*2+1]; i2=i0+i1;
		tblk[i]=i2;	oblk[2+i]=(i2>>1)-i1;
	}

	i0=tblk[0]; i1=tblk[1]; i2=i0+i1;
	oblk[0]=i2; oblk[1]=(i2>>1)-i1;
}

static int btac_band0_tcmbacc[64];
static int btac_band0_tsmbacc[64];

static int btac_band0_tcmbcnt[256];
static int btac_band0_tsmbcnt[256];

void BTAC_Band0_WriteNBits(void *ctx, int v, int n)
{
}

void BTAC_Band0_EncodeSymbol(void *ctx, int tab, int v)
{
}


int BTAC_Band0_BalanceTree_r(short *nodes, short *nlen,
	int root, int h, int ml)
{
	int h0, h1, h2, h3;
	int l0, l1, l2;

	if(root<0)return(0);

	h1=BTAC_Band0_BalanceTree_r(nodes, nlen, nodes[root*2+0], h+1, ml);
	h2=BTAC_Band0_BalanceTree_r(nodes, nlen, nodes[root*2+1], h+1, ml);
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

void BTAC_Band0_CalcLengths_r(short *nodes, byte *cl, int root, int h)
{
	if(root<0)
	{
		if(root==-513)return;
		cl[(-root)-1]=h;
		return;
	}

	BTAC_Band0_CalcLengths_r(nodes, cl, nodes[root*2+0], h+1);
	BTAC_Band0_CalcLengths_r(nodes, cl, nodes[root*2+1], h+1);
}

int BTAC_Band0_BuildLengths(int *stat, int nc, byte *cl, int ml)
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
		k=BTAC_Band0_BalanceTree_r(nodes, nlen, l, 0, ml);
	if(k>ml)return(-2);	//failed to balance tree

	BTAC_Band0_CalcLengths_r(nodes, cl, l, 0);
	return(0);
}


const u32 btac_band0_dbase[]={
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
const int btac_band0_dextra[64]={
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

void BTAC_Band0_EncodeCoeffUValue(void *ctx,
	int tab, int z, u32 val)
{
	int i, j, k;

	j=val;
	for(i=0; i<64; i++)
	{
		k=btac_band0_dbase[i]+(1<<btac_band0_dextra[i]);
		if((j>=btac_band0_dbase[i]) && (j<k))
		{
			BTAC_Band0_EncodeSymbol(ctx, tab, i|(z<<5));
			BTAC_Band0_WriteNBits(ctx,
				j-btac_band0_dbase[i],
				btac_band0_dextra[i]);
			break;
		}
	}
}

void BTAC_Band0_StatCoeffUValue(void *ctx,
	int *stat, int z, u32 val)
{
	int i, j, k;

	j=val;
	for(i=0; i<64; i++)
	{
		k=btac_band0_dbase[i]+(1<<btac_band0_dextra[i]);
		if((j>=btac_band0_dbase[i]) && (j<k))
		{
			stat[i|(z<<5)]++;
			break;
		}
	}
}

#if 0
  void BTJ_JFEH_EncodeVal(void *ctx, int tab, int z, int v)
{
	int i, j, k;

	if(!v) { BTJ_JFEH_EncodeSymbol(ctx, tab, z<<4); return; }

	if(v>0)
	{
		i=1;
		while(v>=(1<<i))i++;
		BTJ_JFEH_EncodeSymbol(ctx, tab, (z<<4)|i);
		BTJ_JFEH_WriteNBits(ctx, v, i);
		return;
	}

	i=1; j=-v; while(j>=(1<<i))i++;
	BTJ_JFEH_EncodeSymbol(ctx, tab, (z<<4)|i);

	k=(1<<i)-(j+1);
	BTJ_JFEH_WriteNBits(ctx, k, i);
	return;
}

void BTAC_Band0_EncodeBlock(void *ctx,
	int *buf, int dctab, int actab)
{
	int i, j, k;

	BTJ_JFEH_EncodeVal(ctx, dctab, 0, buf[0]);

	i=1;
	while(i<64)
	{
		j=buf[i];
		if(j)
		{
			BTJ_JFEH_EncodeVal(ctx, actab, 0, j);
			i++;
			continue;
		}

		j=i+1;
		while(1)
		{
#if 1
			if(j>=64)
			{
				BTJ_JFEH_EncodeSymbol(ctx, actab, 0);
				return;
			}
			if(buf[j])break;
			j++;
#endif
		}

		k=j-i;
//		while((j-i)>15)
		while(k>15)
		{
			BTJ_JFEH_EncodeSymbol(ctx, actab, 15<<4);
			i+=16; k=j-i;
		}

//		k=j-i;
		BTJ_JFEH_EncodeVal(ctx, actab, k, buf[j]);
		i=j+1;
	}
}
#endif

void BTAC_Band0_StatVal(void *ctx, int *stat, int z, int v)
{
	int i, j, k;

	if(!v)
	{
		if(z>5)
		{
			BTAC_Band0_StatCoeffUValue(ctx, stat, 6, z);
			return;
		}
		stat[z<<5]++;
		return;
	}

	if(z>5)
	{
		BTAC_Band0_StatCoeffUValue(ctx, stat, 6, z);
		v=((v>=0)?(v<<1):(((-v)<<1)-1));
		BTAC_Band0_StatCoeffUValue(ctx, stat, 0, v);
		return;
	}

	v=((v>=0)?(v<<1):(((-v)<<1)-1));
	BTAC_Band0_StatCoeffUValue(ctx, stat, z, v);

//	i=1;
//	while(v>=(1<<i))i++;
//	tab[(z<<4)|i]++;

//	BTJ_JFEH_EncodeSymbol(ctx, tab, (z<<4)|i);
//	BTJ_JFEH_WriteNBits(ctx, v, i);
	return;
}

void BTAC_Band0_StatBlock(void *ctx,
	int *buf, int *dcstat, int *acstat)
{
	int i, j, k;

	BTAC_Band0_StatVal(ctx, dcstat, 0, buf[0]);

	i=1;
	while(i<64)
	{
		j=buf[i];
		if(j)
		{
			BTAC_Band0_StatVal(ctx, acstat, 0, j);
			i++;
			continue;
		}

		j=i+1;
		while(1)
		{
#if 1
			if(j>=64)
			{
				BTAC_Band0_StatVal(ctx, acstat, 0, 0);
				return;
			}
			if(buf[j])break;
			j++;
#endif
		}

#if 0
		k=j-i;
//		while((j-i)>15)
		while(k>7)
		{
			acstat[7<<5]++;
			i+=8; k=j-i;
		}
#endif

		k=j-i;
		BTAC_Band0_StatVal(ctx, acstat, k, buf[j]);
		i=j+1;
	}
}

void BGBMID_BTAC_Encode64BlockBand0(s16 *iblk, byte *oblk,
	int tag, int flags)
{
	s16 tcblk[64], tcpan[64];
	int tcma[64], tsma[64];
	int tcmb[64], tsmb[64];
	int cm, cn, sm, sn, tl, tr, tc;
	int cm2, cn2, ca2, ba;
	int p0, p1, p2, p3;
	int s0, s1;
	int i, j, k;

	BGBMID_BTAC_Encode64BlockHeaderSideS16(
		iblk, oblk, tcblk, tcpan,
		&cm, &cn, &sm, &sn, flags);

	for(i=0; i<64; i++)
	{
		cm2= 99999; cn2=-99999;
		tc=tcblk[i];

		if(cn!=cm)
			{ s0=268435456/(cn-cm); }
		else { s0=0; }
		p0=(s0*(tc-cm)+8388608)>>24;
		p0=(p0<0)?0:((p0<16)?p0:15);
		tcma[i]=p0;

		s1=(cn-cm);
		p1=((s1*p0+8)>>4)+cm;

		tcmb[i]=p1;
	}

	for(i=0; i<64; i++)
	{
		cm2= 99999; cn2=-99999;
		tc=tcpan[i];
//		tc=(tcpan[i*4+0]+tcpan[i*4+1]+tcpan[i*4+2]+tcpan[i*4+3])>>2;

		if(sn!=sm)
			{ s0=268435456/(sn-sm); }
		else { s0=0; }
		p0=(s0*(tc-sm)+8388608)>>24;
		p0=(p0<0)?0:((p0<16)?p0:15);
		tsma[i]=p0;

		s1=(sn-sm);
		p1=((s1*p0+8)>>4)+sm;
		tsmb[i]=p1;
	}
	
	BGBMID_BTAC_Band0WHT64(tcma, tcmb, 8);
	BGBMID_BTAC_Band0WHT64(tsma, tsmb, 8);
	
	for(i=0; i<64; i++)
	{
		btac_band0_tcmbacc[i]+=tcmb[i];
		btac_band0_tsmbacc[i]+=tsmb[i];
	}
	
	BTAC_Band0_StatBlock(NULL, tcmb, btac_band0_tcmbcnt, btac_band0_tcmbcnt);
	BTAC_Band0_StatBlock(NULL, tsmb, btac_band0_tsmbcnt, btac_band0_tsmbcnt);
}



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
	
	BTAC_Band0_BuildLengths(btac_band0_tcmbcnt, 256, tcmbcl, 15);
	BTAC_Band0_BuildLengths(btac_band0_tsmbcnt, 256, tsmbcl, 15);

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
