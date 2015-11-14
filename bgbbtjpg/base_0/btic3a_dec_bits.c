#include <bgbbtj.h>

extern const u32 btic2c_dbase[64];
extern const int btic2c_dextra[64];
extern const int btic2c_lbase[64];
extern const int btic2c_lextra[64];

extern const int btic3c_hilbert_idx[16];
extern const int btic3c_hilbert_xy[32];

BTIC3A_BitstreamCtx *BTIC3AH_AllocBitsteam()
{
	BTIC3A_BitstreamCtx *tmp;
	
	tmp=gctalloc("btic3a_bitstreamctx_t", sizeof(BTIC3A_BitstreamCtx));
	return(tmp);
}

void BTIC3A_FreeBitstream(BTIC3A_BitstreamCtx *ctx)
{
	gcfree(ctx);
}

int BTIC3AH_NextByte(BTIC3A_BitstreamCtx *ctx)
{
	int i, j;
	i=*ctx->huff_cs++;
	return(i);
}

int BTIC3AH_InitStreamRead(BTIC3A_BitstreamCtx *ctx, byte *buf, int sz)
{
	ctx->huff_pos=0;
	ctx->huff_isend=0;

	ctx->huff_cs=buf;
	ctx->huff_cse=buf+sz;
	ctx->huff_win=BTIC3AH_NextByte(ctx);
	ctx->huff_win=(ctx->huff_win<<8)|BTIC3AH_NextByte(ctx);
	ctx->huff_win=(ctx->huff_win<<8)|BTIC3AH_NextByte(ctx);
	ctx->huff_win=(ctx->huff_win<<8)|BTIC3AH_NextByte(ctx);
	return(0);
}

int BTIC3AH_ReadBit(BTIC3A_BitstreamCtx *ctx)
{
	int i;

	i=(ctx->huff_win>>(31-ctx->huff_pos))&1;
	ctx->huff_pos++;
	if(ctx->huff_pos>=8)
	{
		ctx->huff_win=(ctx->huff_win<<8)|BTIC3AH_NextByte(ctx);
		ctx->huff_pos-=8;
	}
	return(i);
}

int BTIC3AH_ReadNBits(BTIC3A_BitstreamCtx *ctx, int n)
{
	int i;

	i=(ctx->huff_win>>(32-n-ctx->huff_pos))&((1<<n)-1);
	ctx->huff_pos+=n;
	while(ctx->huff_pos>=8)
	{
		ctx->huff_win=(ctx->huff_win<<8)|BTIC3AH_NextByte(ctx);
		ctx->huff_pos-=8;
	}
	return(i);
}

int BTIC3AH_Read2Bits(BTIC3A_BitstreamCtx *ctx)
	{ return(BTIC3AH_ReadNBits(ctx, 2)); }
int BTIC3AH_Read3Bits(BTIC3A_BitstreamCtx *ctx)
	{ return(BTIC3AH_ReadNBits(ctx, 3)); }
int BTIC3AH_Read4Bits(BTIC3A_BitstreamCtx *ctx)
	{ return(BTIC3AH_ReadNBits(ctx, 4)); }
int BTIC3AH_Read5Bits(BTIC3A_BitstreamCtx *ctx)
	{ return(BTIC3AH_ReadNBits(ctx, 5)); }
int BTIC3AH_Read6Bits(BTIC3A_BitstreamCtx *ctx)
	{ return(BTIC3AH_ReadNBits(ctx, 6)); }
int BTIC3AH_Read7Bits(BTIC3A_BitstreamCtx *ctx)
	{ return(BTIC3AH_ReadNBits(ctx, 7)); }
int BTIC3AH_Read8Bits(BTIC3A_BitstreamCtx *ctx)
	{ return(BTIC3AH_ReadNBits(ctx, 8)); }

void BTIC3AH_SkipNBits(BTIC3A_BitstreamCtx *ctx, int n)
{
	ctx->huff_pos+=n;
	while(ctx->huff_pos>=8)
	{
		ctx->huff_win=(ctx->huff_win<<8)|BTIC3AH_NextByte(ctx);
		ctx->huff_pos-=8;
	}
}

int BTIC3AH_ReadRice(BTIC3A_BitstreamCtx *ctx, int n)
{
	int i, j;
	
	i=0;
	while(BTIC3AH_ReadBit(ctx))i++;
	j=BTIC3AH_ReadNBits(ctx, n);
	return((i<<n)|j);
}

int BTIC3AH_ReadSRice(BTIC3A_BitstreamCtx *ctx, int n)
{
	int i;
	i=BTIC3AH_ReadRice(ctx, n);
	i=(i&1)?(-((i+1)>>1)):(i>>1);
	return(i);
}

int BTIC3AH_ReadAdRice(BTIC3A_BitstreamCtx *ctx, int *rk)
{
	int v, p;
	int i, j, k;
	
	k=*rk;
	if(k<0)
	{
		i=BTIC3AH_ReadRice(ctx, -k);
		return(i);
	}
	
	p=0;
	while(BTIC3AH_ReadBit(ctx))p++;
	i=BTIC3AH_ReadNBits(ctx, k);
	v=(p<<k)|i;

	if(!p && (k>0))k--;
	if(p>1)
	{
		j=p;
		while(j>1)
			{ k++; j>>=1; }
	}
	*rk=k;

	return(v);
}

int BTIC3AH_ReadAdSRice(BTIC3A_BitstreamCtx *ctx, int *rk)
{
	int i;
	i=BTIC3AH_ReadAdRice(ctx, rk);
	i=(i&1)?(-((i+1)>>1)):(i>>1);
	return(i);
}

int BTIC3AH_PeekWord(BTIC3A_BitstreamCtx *ctx)
{
	int i;
	i=(ctx->huff_win>>(16-ctx->huff_pos))&65535;
	return(i);
}

int BTIC3AH_DecodeSymbol(BTIC3A_BitstreamCtx *ctx, int tab)
{
	int i, j, k, l, t;

	i=(byte)(pdjhuff_win>>(24-pdjhuff_pos));
	
	t=tab<<8;
	j=ctx->huff_idx[t|i];
	k=ctx->huff_len[t|j];
	if(k<=8)
	{
		pdjhuff_pos+=k;
		if(pdjhuff_pos>=8)
		{
			pdjhuff_win=(pdjhuff_win<<8)|BTIC3AH_NextByte(ctx);
			pdjhuff_pos-=8;
		}
		return(j);
	}

	i=(u16)(pdjhuff_win>>(16-pdjhuff_pos));
	j=ctx->huff_idx[t|(i>>8)];

	while(1)
	{
		l=t|j;
		k=ctx->huff_len[l];

		if((i>>(16-k))!=ctx->huff_code[l])
		{
			j=ctx->huff_next[l];
			continue;
		}

		pdjhuff_pos+=k;
		while(pdjhuff_pos>=8)
		{
			pdjhuff_win=(pdjhuff_win<<8)|BTIC3AH_NextByte(ctx);
			pdjhuff_pos-=8;
		}
		return(j);
	}

	return(-1);
}

int BTIC3AH_ReadVLCA(BTIC3A_BitstreamCtx *ctx, int pfx)
{
	int i;
	i=btic2c_dbase[pfx]+BTIC2CH_ReadNBits(ctx, btic2c_dextra[pfx]);
	return(i);
}

int BTIC3AH_ReadVLCB(BTIC3A_BitstreamCtx *ctx, int pfx)
{
	int i;
	i=btic2c_lbase[pfx]+BTIC2CH_ReadNBits(ctx, btic2c_lextra[pfx]);
	return(i);
}

int BTIC3AH_ReadSVLCA(BTIC3A_BitstreamCtx *ctx, int pfx)
{
	int i;
	i=BTIC3AH_ReadVLCA(ctx, pfx);
	i=(i&1)?(-((i+1)>>1)):(i>>1);
	return(i);
}

int BTIC3AH_ReadSVLCB(BTIC3A_BitstreamCtx *ctx, int pfx)
{
	int i;
	i=BTIC3AH_ReadVLCB(ctx, pfx);
	i=(i&1)?(-((i+1)>>1)):(i>>1);
	return(i);
}

int BTIC3AH_DecodeVLCA(BTIC3A_BitstreamCtx *ctx, int tab)
{
	int i, j;
	j=BTIC3AH_DecodeSymbol(ctx, tab);
	i=btic2c_dbase[j]+BTIC2CH_ReadNBits(ctx, btic2c_dextra[j]);
	return(i);
}

int BTIC3AH_DecodeVLCB(BTIC3A_BitstreamCtx *ctx, int tab)
{
	int i, j;
	j=BTIC3AH_DecodeSymbol(ctx, tab);
	i=btic2c_lbase[j]+BTIC2CH_ReadNBits(ctx, btic2c_lextra[j]);
	return(i);
}

int BTIC3AH_DecodeSVLCA(BTIC3A_BitstreamCtx *ctx, int tab)
{
	int i;
	i=BTIC3AH_DecodeVLCA(ctx, tab);
	i=(i&1)?(-((i+1)>>1)):(i>>1);
	return(i);
}

int BTIC3AH_DecodeSVLCB(BTIC3A_BitstreamCtx *ctx, int tab)
{
	int i;
	i=BTIC3AH_DecodeVLCB(ctx, tab);
	i=(i&1)?(-((i+1)>>1)):(i>>1);
	return(i);
}

#if 1
static int btic2c_lorder[]={
	16, 17, 18, 0, 8,7, 9,6, 10,5, 11,4, 12,3, 13,2, 14,1, 15, 19};

int BTIC3A_DecodeHufftabSymbol(BTIC3A_BitstreamCtx *ctx, int *rk)
{
	int i;
	i=BTIC3AH_ReadAdRice(ctx, rk);
//	i=BTIC3AH_ReadRice(ctx, 2);
	return(btic2c_lorder[i]);
}

int BTIC3A_DecodeCodeLengths(BTIC3A_BitstreamCtx *ctx,
	byte *cl, int ncl, int *rk)
{
	int i, j, k;

	for(i=0; i<ncl; i++)cl[i]=0;

	i=0; k=0;
	while(i<ncl)
	{
		j=BTIC3A_DecodeHufftabSymbol(ctx, rk);
		if(j<0)return(j);

		if(j<17)
			{ k=j; cl[i++]=j; continue; }

		if(j==17)
		{
			j=BTIC3AH_Read3Bits(ctx)+3;
			while(j--)cl[i++]=k;
			continue;
		}
		if(j==18)
		{
			j=BTIC3AH_ReadNBits(ctx, 7)+11;
			while(j--)cl[i++]=k;
			continue;
		}
		if(j==19)break;

		return(-7);
	}
	return(0);
}
#endif

int BTIC3AH_SetupTabUniform8(BTIC3A_BitstreamCtx *ctx, int tn)
{
	int i, j, k, l;

	for(i=0; i<256; i++)
	{
		ctx->huff_len[tn*256+i]=8;
		ctx->huff_next[tn*256+i]=-1;
		ctx->huff_idx[tn*256+i]=i;
	}
	return(0);
}

int BTIC3AH_MarkerDHT(BTIC3A_BitstreamCtx *ctx, byte *buf, int tn, int len)
{
	byte *cnt, *s;
	int i, j, k, l;

	for(i=0; i<256; i++)
	{
		ctx->huff_len[tn*256+i]=0;
		ctx->huff_next[tn*256+i]=-1;
		ctx->huff_idx[tn*256+i]=-1;
	}

//	for(i=0; i<4096; i++)
//		{ ctx->huff_idx12[tn*4096+i]=-1; }

	s=buf;
	
	i=*s++;
	
//	if(i==0x10)
	if((i&0xF0)==0x10)
	{
		if(i&15)
			{ k=-(i&15); }
		else
			{ k=4; }

		BTIC3AH_InitStreamRead(ctx, s);
		BTIC3A_DecodeCodeLengths(ctx, ctx->huff_len+tn*256, 256, &k);

		k=0; l=0;
		for(i=1; i<=16; i++)
		{
			k<<=1;
			for(j=0; j<256; j++)
			{
				if(ctx->huff_len[tn*256+j]==i)
				{
					ctx->huff_code[tn*256+j]=k++;
//					ctx->huff_len[tn*256+j]=i;
					ctx->huff_mask[tn*256+j]=(1<<i)-1;
					l=i;
				}
			}
		}
	}else if(!i)
	{
		cnt=s; s+=16;
		k=0; l=0;
		for(i=0; i<16; i++)
		{
			k<<=1;
			for(j=0; j<cnt[i]; j++)
			{
				ctx->huff_code[tn*256+(*s)]=k++;
				ctx->huff_len[tn*256+(*s)]=i+1;
				ctx->huff_mask[tn*256+(*s)]=(1<<(i+1))-1;
				l=i+1;
				s++;
			}
		}
//		printf("%04X %d\n", k, l);
	}

#if 1
	for(i=0; i<256; i++)
	{
		l=ctx->huff_len[tn*256+i];
		if(!l)continue;
		j=ctx->huff_code[tn*256+i];

#if 1
		if(l<=8)
		{
			j=j<<(8-l);
			k=1<<(8-l);
			while((k--) && (j<256))
			{
				ctx->huff_idx[tn*256+j]=i;
				j++;
			}
		}else
		{
			j=j>>(l-8);
			ctx->huff_next[tn*256+i]=
				ctx->huff_idx[tn*256+j];
			ctx->huff_idx[tn*256+j]=i;
		}
#endif

#if 0
		if(l<=12)
		{
			j=j<<(12-l);
			k=1<<(12-l);
			while((k--) && (j<4096))
			{
				ctx->huff_idx12[tn*4096+j]=i;
				j++;
			}
		}else
		{
			j=j>>(l-12);
			ctx->huff_next[tn*256+i]=
				ctx->huff_idx12[tn*4096+j];
			ctx->huff_idx12[tn*4096+j]=i;
		}
#endif
	}
#endif

	return(0);
}


int BTIC3AH_ReadMotionVector(BTIC3A_BitstreamCtx *ctx,
	int *rvx, int *rvy)
{
	int rk;
//	rk=BTIC3AH_ReadRice(ctx, 1);
//	*rvx=BTIC3AH_ReadAdSRice(ctx, &ctx->mv_rkx);
//	*rvy=BTIC3AH_ReadAdSRice(ctx, &ctx->mv_rky);

	*rvx=BTIC3AH_DecodeSVLCB(ctx, 1);
	*rvy=BTIC3AH_DecodeSVLCB(ctx, 1);
}
