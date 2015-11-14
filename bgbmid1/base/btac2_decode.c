#include <bgbmid.h>

extern const u32 btac2_dbase[64];
extern const int btac2_dextra[64];

extern const u32 btac2_lbase[64];
extern const int btac2_lextra[64];

extern const int btac2_lorder[];
extern const int btac2_lorder2[];


void BTAC2_Decode_PushRead(BGBMID_BTAC2_Context *ctx)
{
	int i;

	i=ctx->stk_pos++;
	ctx->stk_cs[i]=ctx->cs;
	ctx->stk_css[i]=ctx->css;
	ctx->stk_cse[i]=ctx->cse;
}

void BTAC2_Decode_PopRead(BGBMID_BTAC2_Context *ctx)
{
	int i;

	i=--ctx->stk_pos;
	ctx->cs=ctx->stk_cs[i];
	ctx->css=ctx->stk_css[i];
	ctx->cse=ctx->stk_cse[i];
}

int BTAC2_Decode_NextByte(BGBMID_BTAC2_Context *ctx)
{
	int i, j;

	i=*ctx->cs++;
	return(i);
}

int BTAC2_Decode_InitReadBytes(BGBMID_BTAC2_Context *ctx, byte *buf, int sz)
{
	ctx->cs=buf;
	ctx->css=buf;
	ctx->cse=buf+sz;
	
	return(0);
}

int BTAC2_Decode_InitReadStream(BGBMID_BTAC2_Context *ctx, byte *buf, int sz)
{
	ctx->huff_pos=0;
//	btic2bh_isend=0;

	ctx->cs=buf;
	ctx->css=buf;
	ctx->cse=buf+sz;

	ctx->huff_win=BTAC2_Decode_NextByte(ctx)<<24;
	ctx->huff_win=(ctx->huff_win>>8)|(BTAC2_Decode_NextByte(ctx)<<24);
	ctx->huff_win=(ctx->huff_win>>8)|(BTAC2_Decode_NextByte(ctx)<<24);
	ctx->huff_win=(ctx->huff_win>>8)|(BTAC2_Decode_NextByte(ctx)<<24);
	
	return(0);
}

int BTAC2_Decode_ReadBit(BGBMID_BTAC2_Context *ctx)
{
	int i;

	i=(ctx->huff_win>>ctx->huff_pos)&1;
	ctx->huff_pos++;
	if(ctx->huff_pos>=8)
	{
		ctx->huff_win=(ctx->huff_win>>8)|
			(BTAC2_Decode_NextByte(ctx)<<24);
		ctx->huff_pos-=8;
	}
	return(i);
}

int BTAC2_Decode_Read2Bits(BGBMID_BTAC2_Context *ctx)
{
	int i;
	i=(ctx->huff_win>>ctx->huff_pos)&3;
	ctx->huff_pos+=2;
	if(ctx->huff_pos>=8)
	{
		ctx->huff_win=(ctx->huff_win>>8)|
			(BTAC2_Decode_NextByte(ctx)<<24);
		ctx->huff_pos-=8;
	}
	return(i);
}

int BTAC2_Decode_Read3Bits(BGBMID_BTAC2_Context *ctx)
{
	int i;
	i=(ctx->huff_win>>ctx->huff_pos)&7;
	ctx->huff_pos+=3;
	if(ctx->huff_pos>=8)
	{
		ctx->huff_win=(ctx->huff_win>>8)|
			(BTAC2_Decode_NextByte(ctx)<<24);
		ctx->huff_pos-=8;
	}
	return(i);
}

int BTAC2_Decode_Read4Bits(BGBMID_BTAC2_Context *ctx)
{
	int i;
	i=(ctx->huff_win>>ctx->huff_pos)&15;
	ctx->huff_pos+=4;
	if(ctx->huff_pos>=8)
	{
		ctx->huff_win=(ctx->huff_win>>8)|
			(BTAC2_Decode_NextByte(ctx)<<24);
		ctx->huff_pos-=8;
	}
	return(i);
}

int BTAC2_Decode_Read5Bits(BGBMID_BTAC2_Context *ctx)
{
	int i;
	i=(ctx->huff_win>>ctx->huff_pos)&31;
	ctx->huff_pos+=5;
	if(ctx->huff_pos>=8)
	{
		ctx->huff_win=(ctx->huff_win>>8)|
			(BTAC2_Decode_NextByte(ctx)<<24);
		ctx->huff_pos-=8;
	}
	return(i);
}

int BTAC2_Decode_ReadNBits(BGBMID_BTAC2_Context *ctx, int n)
{
	int i;

	i=(ctx->huff_win>>ctx->huff_pos)&((1<<n)-1);
	ctx->huff_pos+=n;
	while(ctx->huff_pos>=8)
	{
		ctx->huff_win=(ctx->huff_win>>8)|(BTAC2_Decode_NextByte(ctx)<<24);
		ctx->huff_pos-=8;
	}
	return(i);
}

u64 BTAC2_Decode_ReadNBitsL(BGBMID_BTAC2_Context *ctx, int n)
{
	u64 v;
	int i, j;
	
	v=0; i=n; j=0;
	while(i>16)
	{
		v|=((u64)BTAC2_Decode_ReadNBits(ctx, 16))<<j;
		i-=16; j+=16;
	}
	v|=((u64)BTAC2_Decode_ReadNBits(ctx, i))<<j;
	return(v);
}

void BTAC2_Decode_SkipNBits(BGBMID_BTAC2_Context *ctx, int n)
{
	ctx->huff_pos+=n;
	while(ctx->huff_pos>=8)
	{
		ctx->huff_win=(ctx->huff_win>>8)|(BTAC2_Decode_NextByte(ctx)<<24);
		ctx->huff_pos-=8;
	}
}

int BTAC2_Decode_ReadRice(BGBMID_BTAC2_Context *ctx, int n)
{
	int i, j;
	
	i=0;
	while(BTAC2_Decode_ReadBit(ctx))i++;
	j=BTAC2_Decode_ReadNBits(ctx, n);
	return((i<<n)|j);
}

int BTAC2_Decode_ReadSRice(BGBMID_BTAC2_Context *ctx, int n)
{
	int i;
	i=BTAC2_Decode_ReadRice(ctx, n);
	i=(i&1)?(-((i+1)>>1)):(i>>1);
	return(i);
}

int BTAC2_Decode_PeekWord(BGBMID_BTAC2_Context *ctx)
{
	int i;
	i=(ctx->huff_win>>ctx->huff_pos)&65535;
	return(i);
}

int BTAC2_Decode_DecodeSymbol(BGBMID_BTAC2_Context *ctx, int tab)
{
	int i, j, k;

//	tab<<=8;

	i=BTAC2_Decode_PeekWord(ctx);
	for(j=0; j<256; j++)
	{
		k=ctx->huff_len[tab][j];
		if(!k)continue;

		if((i&((1<<k)-1))!=ctx->huff_code[tab][j])
			continue;
		BTAC2_Decode_SkipNBits(ctx, k);
		return(j);
	}
	return(-1);
}

int BTAC2_Decode_DecodeHufftabSymbol(BGBMID_BTAC2_Context *ctx, int rk)
{
	int i;
	i=BTAC2_Decode_ReadRice(ctx, rk);
	return(btac2_lorder[i]);
}

#if 0
int BTAC2_Decode_DecodeHuffTableFixed(BGBMID_BTAC2_Context *ctx,
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

	j=BTAC2_Decode_SetupTable(lcl, hl,
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
#endif

int BTAC2_Decode_DecodeCodeLengths(BGBMID_BTAC2_Context *ctx,
	byte *cl, int ncl, int rk)
{
	int i, j, k;

	for(i=0; i<ncl; i++)cl[i]=0;

	i=0; k=0;
	while(i<ncl)
	{
		j=BTAC2_Decode_DecodeHufftabSymbol(ctx, rk);
		if(j<0)return(j);

		if(j<17)
			{ k=j; cl[i++]=j; continue; }

		if(j==17)
		{
			j=BTAC2_Decode_Read3Bits(ctx)+3;
			while(j--)cl[i++]=k;
			continue;
		}
		if(j==18)
		{
			j=BTAC2_Decode_ReadNBits(ctx, 7)+11;
			while(j--)cl[i++]=k;
			continue;
		}
		if(j==19)break;

		return(-7);
	}
	return(0);
}

int BTAC2_Decode_DecodeHuffTable(BGBMID_BTAC2_Context *ctx, int tab)
{
	byte lcl[512];
	int ht, hk, hl;
	int i, j, k;

	if(!ctx->huff_code[tab])
		{ ctx->huff_code[tab]=gcatomic(256*sizeof(u16)); }
	if(!ctx->huff_mask[tab])
		{ ctx->huff_mask[tab]=gcatomic(256*sizeof(u16)); }
	if(!ctx->huff_len[tab])
		{ ctx->huff_len[tab]=gcatomic(256*sizeof(byte)); }
	if(!ctx->huff_idx[tab])
		{ ctx->huff_idx[tab]=gcatomic(256*sizeof(u16)); }
	if(!ctx->huff_next[tab])
		{ ctx->huff_next[tab]=gcatomic(256*sizeof(u16)); }

	ht=BTAC2_Decode_ReadNBits(ctx, 4);
	hk=BTAC2_Decode_ReadNBits(ctx, 4);

#if 0	
	if(ht==0)
	{
		i=BTAC2_Decode_DecodeHuffTableFixed(ctx, tab, hk);
		return(i);
	}
#endif

	if(ht!=1)
	{
		dyPrintf("Bad Huffman Table Type %d\n", ht);
		return(-1);
	}

	BTAC2_Decode_DecodeCodeLengths(ctx, lcl, 256, hk);
	j=BTAC2_Common_SetupTable(lcl, 256,
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

int BTAC2_Decode_DecodeQuantTable(BGBMID_BTAC2_Context *ctx, int tab)
{
	int ts, rk;
	int i, j, k, l, n;

	ts=BTAC2_Decode_ReadNBits(ctx, 4);
	rk=BTAC2_Decode_ReadNBits(ctx, 4);

	n=16<<ts;

	if(ctx->qtab_min[tab])
		{ gcfree(ctx->qtab_min[tab]); }
	if(ctx->qtab_max[tab])
		{ gcfree(ctx->qtab_max[tab]); }
	ctx->qtab_min[tab]=gcatomic(n*sizeof(int));
	ctx->qtab_max[tab]=gcatomic(n*sizeof(int));

	l=0;
	for(i=0; i<n; i++)
	{
		j=BTAC2_Decode_ReadSRice(ctx, rk);
		if(j<0)
		{
			k=i+(-j);
			for(j=i; (j<k) && (j<n); j++)
				ctx->qtab_min[tab][j]=l;
			i=k-1;
			continue;
		}
		
//		l=j;
		k=btac2_dbase[j]+BTAC2_Decode_ReadNBits(ctx, btac2_dextra[j]);
		k=(k&1)?(-((k+1)>>1)):(k>>1);
		l=l+k;

		ctx->qtab_min[tab][i]=l;
	}

	l=0;
	for(i=0; i<n; i++)
	{
		j=BTAC2_Decode_ReadSRice(ctx, rk);
		if(j<0)
		{
			k=i+(-j);
			for(j=i; (j<k) && (j<n); j++)
				ctx->qtab_max[tab][j]=l;
			i=k-1;
			continue;
		}

//		l=j;

		k=btac2_dbase[j]+BTAC2_Decode_ReadNBits(ctx, btac2_dextra[j]);
		k=(k&1)?(-((k+1)>>1)):(k>>1);
		l=l+k;

		ctx->qtab_max[tab][i]=l;
	}
	return(0);
}


int BTAC2_Decode_DecodeCoeffUValueBits(
	BGBMID_BTAC2_Context *ctx, int sym)
{
	int i, j;
	i=btac2_dbase[sym]+BTAC2_Decode_ReadNBits(ctx, btac2_dextra[sym]);
	return(i);
}

int BTAC2_Decode_DecodeCoeffValueBits(
	BGBMID_BTAC2_Context *ctx, int sym)
{
	int i;
	i=BTAC2_Decode_DecodeCoeffUValueBits(ctx, sym);
	i=(i&1)?(-((i+1)>>1)):(i>>1);
	return(i);
}

int BTAC2_Decode_DecodeTagBits(
	BGBMID_BTAC2_Context *ctx, int sym)
{
	int i, j;
//	i=lbase[sym]+BTAC2_Decode_ReadNBits(ctx, lextra[sym]);
	i=btac2_lbase[sym]+BTAC2_Decode_ReadNBits(ctx, btac2_lextra[sym]);
	return(i);
}

int BTAC2_Decode_DecodeDiffDC(BGBMID_BTAC2_Context *ctx, int tab)
{
	int i, j;

	i=BTAC2_Decode_DecodeSymbol(ctx, tab);
	j=BTAC2_Decode_DecodeCoeffValueBits(ctx, i&63);
	return(j);
}

int BTAC2_Decode_DecodeBlock(BGBMID_BTAC2_Context *ctx,
	int *buf, int blen, int dctab, int actab)
{
	int i, j, k;

	for(i=1; i<blen; i++)buf[i]=0;

	buf[0]=BTAC2_Decode_DecodeDiffDC(ctx, dctab);
	for(i=1; i<blen; i++)
	{
		j=BTAC2_Decode_DecodeSymbol(ctx, actab);
		if(!j)break;

		k=j>>5;
		if(k>=7)
		{
			i+=BTAC2_Decode_DecodeTagBits(ctx, j&31);
//			k=BTIC2BH_DecodeDiffDC(ctx, actab);
			j=BTAC2_Decode_DecodeSymbol(ctx, actab);
			k=BTAC2_Decode_DecodeCoeffValueBits(ctx, j&63);
			buf[i]=k;
			continue;
		}

		i+=k;
		if(i>=blen)break;
		
		if(!(j&31))continue;
		k=BTAC2_Decode_DecodeCoeffValueBits(ctx, j&31);
		buf[i]=k;
	}

	if(i>blen)
	{
		printf("bad block1 %d %02X\n", i, j);
		return(-1);
	}

	return(0);
}

void BTAC2_Decode_DecodeMarker(BGBMID_BTAC2_Context *ctx,
	int *rtag, int *rlen, int *rflags)
{
	int i, j, k;
	
	i=ctx->cs[0] | (ctx->cs[1]<<8);
	j=ctx->cs[2] | (ctx->cs[3]<<8);
	ctx->cs+=4;
	
	if(i>=4)
	{
		*rtag=j;
		*rlen=i-4;
		*rflags=0;
		return;
	}
	
	if(i==0)
	{
		i=ctx->cs[0] | (ctx->cs[1]<<8) |
			(ctx->cs[2]<<16) | (ctx->cs[3]<<24);
		ctx->cs+=4;
		*rtag=j;	*rlen=i-8;	*rflags=0;
		return;
	}

	if(i==1)
	{
		j=ctx->cs[0] | (ctx->cs[1]<<8) |
			(ctx->cs[2]<<16) | (ctx->cs[3]<<24);
		ctx->cs+=4;
		*rtag=j;	*rlen=i-8;	*rflags=0;
		return;
	}

	if(i==2)
	{
		k=j;
		
		i=ctx->cs[0] | (ctx->cs[1]<<8) |
			(ctx->cs[2]<<16) | (ctx->cs[3]<<24);
		ctx->cs+=4;

		j=ctx->cs[0] | (ctx->cs[1]<<8) |
			(ctx->cs[2]<<16) | (ctx->cs[3]<<24);
		ctx->cs+=4;

		*rtag=j;	*rlen=i-12;	*rflags=k;
		return;
	}

	if(i==3)
	{
		k=j;
		
		i=ctx->cs[0] | (ctx->cs[1]<<8) |
			(ctx->cs[2]<<16) | (ctx->cs[3]<<24);
		ctx->cs+=8;

		j=ctx->cs[0] | (ctx->cs[1]<<8) |
			(ctx->cs[2]<<16) | (ctx->cs[3]<<24);
		ctx->cs+=4;

		*rtag=j;	*rlen=i-16;	*rflags=k;
		return;
	}
}

void BTAC2_Decode_SeekPastMarker(BGBMID_BTAC2_Context *ctx,
	int tag, int len, int flags)
{
	ctx->cs+=len;
}

void BTAC2_Decode_DecodeHeaderTables(
	BGBMID_BTAC2_Context *ctx, byte *ibuf, int isz)
{
	byte *cs0;
	int tag, len, fl;
	int i, j, k, n;

	BTAC2_Decode_PushRead(ctx);
	BTAC2_Decode_InitReadBytes(ctx, ibuf, isz);

	while(ctx->cs<ctx->cse)
	{
		BTAC2_Decode_DecodeMarker(ctx, &tag, &len, &fl);
		if(tag==BTAC2_TWOCC('Z', 'Z'))
			{ BTAC2_Decode_SeekPastMarker(ctx, tag, len, fl); break; }

		if(tag==BTAC2_TWOCC('H', 'D'))
		{
			ctx->btac_blklen=ctx->cs[8]|(ctx->cs[9]<<8);
			ctx->btac_blkbytes=ctx->cs[10]|(ctx->cs[11]<<8);
			ctx->btac_blkbits=ctx->btac_blkbytes*8;

			ctx->flags=ctx->cs[12]|(ctx->cs[13]<<8)|
				(ctx->cs[14]<<16)|(ctx->cs[15]<<24);
			ctx->chan=ctx->cs[18];
			ctx->bits=ctx->cs[19];
			ctx->rate=ctx->cs[20]|(ctx->cs[21]<<8)|
				(ctx->cs[22]<<16)|(ctx->cs[23]<<24);
			ctx->len=ctx->cs[24]|(ctx->cs[25]<<8)|
				(ctx->cs[26]<<16)|(ctx->cs[27]<<24);

			BTAC2_Decode_SeekPastMarker(ctx, tag, len, fl);
			continue;
		}
			
		if((BTAC2_TWOCC_A(tag)=='H') &&
			(BTAC2_TWOCC_B(tag)>='0') && (BTAC2_TWOCC_B(tag)<='7'))
		{
			BTAC2_Decode_PushRead(ctx);
			BTAC2_Decode_InitReadStream(ctx, ctx->cs, len);
			BTAC2_Decode_DecodeHuffTable(ctx, BTAC2_TWOCC_B(tag)-'0');
			BTAC2_Decode_PopRead(ctx);
			BTAC2_Decode_SeekPastMarker(ctx, tag, len, fl);
			continue;
		}

		if((BTAC2_TWOCC_A(tag)=='Q') &&
			(BTAC2_TWOCC_B(tag)>='0') && (BTAC2_TWOCC_B(tag)<='7'))
		{
			BTAC2_Decode_PushRead(ctx);
			BTAC2_Decode_InitReadStream(ctx, ctx->cs, len);
			BTAC2_Decode_DecodeQuantTable(ctx, BTAC2_TWOCC_B(tag)-'0');
			BTAC2_Decode_PopRead(ctx);
			BTAC2_Decode_SeekPastMarker(ctx, tag, len, fl);
			continue;
		}

		if(tag==BTAC2_TWOCC('T', 'X'))
		{
			n=(len+7)/8;
			ctx->tabidx=gcatomic(n*sizeof(u64));
			ctx->n_tabidx=n;
			
			for(i=0; i<n; i++)
			{
				ctx->tabidx[i]=
					((u64)ctx->cs[i*8+0]    )|((u64)ctx->cs[i*8+1]<< 8)|
					((u64)ctx->cs[i*8+2]<<16)|((u64)ctx->cs[i*8+3]<<24)|
					((u64)ctx->cs[i*8+4]<<32)|((u64)ctx->cs[i*8+5]<<40)|
					((u64)ctx->cs[i*8+6]<<48)|((u64)ctx->cs[i*8+7]<<56);
			}

			BTAC2_Decode_SeekPastMarker(ctx, tag, len, fl);
			continue;
		}
			
		BTAC2_Decode_SeekPastMarker(ctx, tag, len, fl);
	}
	BTAC2_Decode_PopRead(ctx);
}

void BTAC2_MergeCenterSideS16(
	BGBMID_BTAC2_Context *ctx, s16 *oblk,
	int *tcblk, int *tsblk,
	int blen, int flags)
{
	int i0, i1, i2, i3;
	int i, j, k;

	if((flags&BGBMID_BTAC_EFL_STEREO) &&
		!(flags&BGBMID_BTAC_EFL_HALFMONO))
	{
		if((ctx->flags&BGBMID_BTAC_EFL_STEREO) &&
			!(ctx->flags&(BGBMID_BTAC_EFL_CENTERMONO|
				BGBMID_BTAC_EFL_HALFMONO)))
		{
			for(i=0; i<blen; i++)
			{
				i0=tcblk[i];
				i1=tsblk[i];
//				i3=(i0>>1)-i1;
//				i2=i0-i3;

//				i3=2*i0-i1;
//				i2=i3+i1;

				i2=i0+i1;
				i3=i0-i1;

//				i2=i0-i1;
//				i3=i0+i1;

				if(i2<-32768)i2=-32768;
				if(i2>32767)i2=32767;
				if(i3<-32768)i3=-32768;
				if(i3>32767)i3=32767;
				oblk[i*2+0]=i2;
				oblk[i*2+1]=i3;
			}
		}else
		{
			for(i=0; i<blen; i++)
			{
				i0=tcblk[i];
				if(i0<-32768)i0=-32768;
				if(i0>32767)i0=32767;
				oblk[i*2+0]=i0;
				oblk[i*2+1]=i0;
			}
		}
	}else if((flags&BGBMID_BTAC_EFL_STEREO) &&
		(flags&BGBMID_BTAC_EFL_HALFMONO))
	{
		if(flags&BGBMID_BTAC_EFL_CENTERMONO)
			oblk++;
		
		if((ctx->flags&BGBMID_BTAC_EFL_STEREO) &&
			!(ctx->flags&(BGBMID_BTAC_EFL_CENTERMONO|
				BGBMID_BTAC_EFL_HALFMONO)))
		{
			for(i=0; i<blen; i++)
			{
				i0=tcblk[i]>>1;
				if(i0<-32768)i0=-32768;
				if(i0>32767)i0=32767;
				oblk[i*2+0]=i0;
			}
		}else
		{
			for(i=0; i<blen; i++)
			{
				i0=tcblk[i];
				if(i0<-32768)i0=-32768;
				if(i0>32767)i0=32767;
				oblk[i*2+0]=i0;
			}
		}
	}else
	{
		if((ctx->flags&BGBMID_BTAC_EFL_STEREO) &&
			!(ctx->flags&(BGBMID_BTAC_EFL_CENTERMONO|
				BGBMID_BTAC_EFL_HALFMONO)))
		{
			for(i=0; i<blen; i++)
			{
				i0=tcblk[i]>>1;
				if(i0<-32768)i0=-32768;
				if(i0>32767)i0=32767;
				oblk[i]=i0;
			}
		}else
		{
			for(i=0; i<blen; i++)
			{
				i0=tcblk[i];
				if(i0<-32768)i0=-32768;
				if(i0>32767)i0=32767;
				oblk[i]=i0;
			}
		}
	}
}

int BTAC2_Decode_FilterDecodeBlock(
	BGBMID_BTAC2_Context *ctx, byte *ibuf, s16 *oblk, int flags)
{
	int tcma[256], tsma[256];
	int tcmb[256], tsmb[256];
	int h0, h1, h2, h3, q0, q1;
	int ts, rk, qf, bl;
	int i, j, l, n, ret;

	bl=ctx->btac_blklen;
	ret=0;
	
	BTAC2_Decode_PushRead(ctx);
	BTAC2_Decode_InitReadStream(ctx, ibuf, ctx->btac_blkbytes);

	ts=BTAC2_Decode_ReadNBits(ctx, 4);
	rk=BTAC2_Decode_ReadNBits(ctx, 4);
	qf=BTAC2_Decode_ReadNBits(ctx, 8);
	
	if(ctx->tabidx)
	{
		h0=(ctx->tabidx[rk]>>4)&63;
		h1=(ctx->tabidx[rk]>>10)&63;
		h2=(ctx->tabidx[rk]>>16)&63;
		h3=(ctx->tabidx[rk]>>22)&63;
		q0=(ctx->tabidx[rk]>>28)&63;
		q0=(ctx->tabidx[rk]>>34)&63;
	}else
		{ h0=0; h1=1; h2=2; h3=3; q0=0; q1=1; }
	
	if(ts==0)
	{
		i=BTAC2_Decode_DecodeBlock(ctx, tcma, bl, h0, h1);
		if(i<0) { ret=-1; }
		
		BTAC2_Common_DequantWHT(ctx, tcma, tcmb, bl, q0, qf);
		BTAC2_Common_TransformIWHT(tcmb, tcmb, bl, 0);
//		BTAC2_Common_TransformIDCT(tcmb, tcmb, bl, 0);

		if((ctx->flags&BGBMID_BTAC_EFL_STEREO)&&
			!(ctx->flags&(BGBMID_BTAC_EFL_CENTERMONO|
				BGBMID_BTAC_EFL_HALFMONO)))
		{
			i=BTAC2_Decode_DecodeBlock(ctx, tsma, bl, h2, h3);
			if(i<0) { ret=-1; }
		
			BTAC2_Common_DequantWHT(ctx, tsma, tsmb, bl, q1, qf);
			BTAC2_Common_TransformIWHT(tsmb, tsmb, bl, 0);
//			BTAC2_Common_TransformIDCT(tsmb, tsmb, bl, 0);
		}

		BTAC2_MergeCenterSideS16(ctx, oblk, tcmb, tsmb, bl, flags);
	}

	BTAC2_Decode_PopRead(ctx);
	return(ret);
}

void BTAC2_Decode_DecodeBody(
	BGBMID_BTAC2_Context *ctx, byte *ibuf, int isz)
{
	int tag, len, fl;
	int i, j, k;

	BTAC2_Decode_PushRead(ctx);
	BTAC2_Decode_InitReadBytes(ctx, ibuf, isz);
	while(ctx->cs<ctx->cse)
	{
		BTAC2_Decode_DecodeMarker(ctx, &tag, &len, &fl);
//		if(tag==BTAC2_TWOCC('Z', 'Z'))
//			{ BTAC2_Decode_SeekPastMarker(ctx, tag, len, fl); break; }

		if(tag==BTAC2_FOURCC('H', 'E', 'A', 'D'))
		{
			BTAC2_Decode_DecodeHeaderTables(ctx, ctx->cs, len);
			BTAC2_Decode_SeekPastMarker(ctx, tag, len, fl);
			continue;
		}

		if(tag==BTAC2_FOURCC('D', 'A', 'T', 'A'))
		{
			if(ctx->data)
				{ gcfree(ctx->data); ctx->data=NULL; }
			ctx->data=gcatomic(len);
			memcpy(ctx->data, ctx->cs, len);
			BTAC2_Decode_SeekPastMarker(ctx, tag, len, fl);
			continue;
		}
			
		BTAC2_Decode_SeekPastMarker(ctx, tag, len, fl);
	}
	BTAC2_Decode_PopRead(ctx);
}


BMID_API void BTAC2_Sample_DecodeBlock(BGBMID_Sample *self,
	int blkid, s16 *obuf, int flags)
{
	BGBMID_BTAC2_Context *ctx;
	
	ctx=self->codecdata;
	BTAC2_Decode_FilterDecodeBlock(ctx,
		ctx->data + (blkid*ctx->btac_blkbytes),
		obuf, flags);
}

BMID_API void BTAC2_Sample_DecodeStream(BGBMID_Sample *self,
	s16 *obuf, int base, int len, int flags)
{
	BGBMID_BTAC2_Context *ctx;
	int i, j, ch;

	ch=(flags&BGBMID_BTAC_EFL_STEREO)?2:1;

	ctx=self->codecdata;
//	ch=ctx->chan;
//	if(ch>1)flags|=BGBMID_BTAC_EFL_STEREO;

	for(i=0; i<len; i++)
	{
		j=BTAC2_Decode_FilterDecodeBlock(ctx,
			ctx->data + ((base+i)*ctx->btac_blkbytes),
			obuf + (i*ctx->btac_blklen*ch), flags);

		if(j<0)
		{
			printf("BTAC2_Sample_DecodeStream: Error %d %d/%d\n",
				j, i, len);
		}
	}
}

BMID_API BGBMID_Sample *BGBMID_BTAC2_DecodeSample(byte *ibuf, int isz)
{
	BGBMID_Sample *head;
	BGBMID_BTAC2_Context *ctx;

	if(memcmp(ibuf, "BTAC2A", 6))
		return(NULL);

	head=BGBMID_BTAC_CreateSample(0, 0, 0, 0, 0);

	ctx=BTAC2_Common_AllocContext();
	BTAC2_Decode_DecodeBody(ctx, ibuf+16, isz-16);
	
	head->codecdata=ctx;
	head->DecodeBlock=BTAC2_Sample_DecodeBlock;
	
	head->chan=ctx->chan;
	head->bits=ctx->bits;
	head->len=ctx->len;
	head->rate=ctx->rate;
	head->flags=ctx->flags;
	head->blk_samples=bgbmid_log2up(ctx->btac_blklen);
	head->blk_size=bgbmid_log2up(ctx->btac_blkbytes);

	return(head);
}
