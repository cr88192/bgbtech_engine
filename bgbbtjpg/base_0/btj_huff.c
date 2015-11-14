#include <bgbbtj.h>

extern const int pdjpg_zigzag[64];
extern const int pdjpg_zigzag2[64];

int BGBBTJ_Huff_NextByte(BGBBTJ_JPG_Context *ctx)
{
	int i, j;

//	if(pdjhuff_isend)return(0xFF);
	if(pdjhuff_isend)return(0x00);

	i=*pdjhuff_cs++;
	if(i==0xFF)
	{
		j=*pdjhuff_cs++;
		if(j)
		{
//			printf("BGBBTJ_Huff_NextByte: Bitstream Marker %02X\n", j);
			pdjhuff_isend=1;
			return(0x00);
		}
	}
	return(i);
}

int BGBBTJ_Huff_InitStream(BGBBTJ_JPG_Context *ctx, byte *buf)
{
	pdjhuff_pos=0;
	pdjhuff_isend=0;

	pdjhuff_cs=buf;
	pdjhuff_win=BGBBTJ_Huff_NextByte(ctx);
	pdjhuff_win=(pdjhuff_win<<8)|BGBBTJ_Huff_NextByte(ctx);
	pdjhuff_win=(pdjhuff_win<<8)|BGBBTJ_Huff_NextByte(ctx);
	pdjhuff_win=(pdjhuff_win<<8)|BGBBTJ_Huff_NextByte(ctx);
	return(0);
}

int BGBBTJ_Huff_ReadBit(BGBBTJ_JPG_Context *ctx)
{
	int i;

	i=(pdjhuff_win>>(31-pdjhuff_pos))&1;
	pdjhuff_pos++;
	if(pdjhuff_pos>=8)
	{
		pdjhuff_win=(pdjhuff_win<<8)|BGBBTJ_Huff_NextByte(ctx);
		pdjhuff_pos-=8;
	}
	return(i);
}

int BGBBTJ_Huff_ReadNBits(BGBBTJ_JPG_Context *ctx, int n)
{
	int i;

	if(n<=0)return(0);

	i=(pdjhuff_win>>(32-n-pdjhuff_pos))&((1<<n)-1);
	pdjhuff_pos+=n;
	while(pdjhuff_pos>=8)
	{
		pdjhuff_win=(pdjhuff_win<<8)|BGBBTJ_Huff_NextByte(ctx);
		pdjhuff_pos-=8;
	}
	return(i);
}

void BGBBTJ_Huff_SkipNBits(BGBBTJ_JPG_Context *ctx, int n)
{
	pdjhuff_pos+=n;
	while(pdjhuff_pos>=8)
	{
		pdjhuff_win=(pdjhuff_win<<8)|BGBBTJ_Huff_NextByte(ctx);
		pdjhuff_pos-=8;
	}
}

int BGBBTJ_Huff_PeekWord(BGBBTJ_JPG_Context *ctx)
{
	int i;
	i=(pdjhuff_win>>(16-pdjhuff_pos))&65535;
	return(i);
}

#if 1
int BGBBTJ_Huff_DecodeSymbol(BGBBTJ_JPG_Context *ctx, int tab)
{
	int i, j, k, l;

//	i=BGBBTJ_Huff_PeekWord(ctx);
//	i=(pdjhuff_win>>(16-pdjhuff_pos))&65535;
//	i=(u16)(pdjhuff_win>>(16-pdjhuff_pos));

	i=(byte)(pdjhuff_win>>(24-pdjhuff_pos));
	
	tab<<=8;
//	j=ctx->huff_idx[tab|k];
//	j=ctx->huff_idx[tab|(i>>8)];
	j=ctx->huff_idx[tab|i];

#if 1
//	l=tab|j;
//	k=ctx->huff_len[l];
	k=ctx->huff_len[tab|j];
//	if(0)
	if(k<=8)
//	if(k<8)
	{
		pdjhuff_pos+=k;
//		while(pdjhuff_pos>=8)
		if(pdjhuff_pos>=8)
		{
			pdjhuff_win=(pdjhuff_win<<8)|BGBBTJ_Huff_NextByte(ctx);
			pdjhuff_pos-=8;
		}

//		BGBBTJ_Huff_SkipNBits(ctx, k);
		return(j);
	}
#endif

	i=(u16)(pdjhuff_win>>(16-pdjhuff_pos));
	j=ctx->huff_idx[tab|(i>>8)];

	while(1)
	{
		l=tab|j;
		k=ctx->huff_len[l];
		if(!k)
		{
//			printf("BGBBTJ_Huff_DecodeSymbol: Trap A\n");
			break;
		}

#if 1
		if((i>>(16-k))!=ctx->huff_code[l])
//		if((k>=8) && (i>>(16-k))!=ctx->huff_code[l])
		{
			j=ctx->huff_next[l];
//			if(!j)
			if(j<0)
			{
//				printf("BGBBTJ_Huff_DecodeSymbol: Trap B\n");
				break;
			}
			continue;
		}
#endif

		pdjhuff_pos+=k;
		while(pdjhuff_pos>=8)
//		if(pdjhuff_pos>=8)
		{
			pdjhuff_win=(pdjhuff_win<<8)|BGBBTJ_Huff_NextByte(ctx);
			pdjhuff_pos-=8;
		}

//		BGBBTJ_Huff_SkipNBits(ctx, k);
		return(j);
	}

#if 0
	i=BGBBTJ_Huff_PeekWord(ctx);
	for(j=0; j<256; j++)
	{
		k=ctx->huff_len[tab|j];
		if(!k)continue;

		if((i>>(16-k))!=ctx->huff_code[tab|j])
			continue;
		BGBBTJ_Huff_SkipNBits(ctx, k);
		return(j);
	}
#endif

	return(-1);
}
#endif


#if 0
int BGBBTJ_Huff_DecodeSymbol(BGBBTJ_JPG_Context *ctx, int tab)
{
	int i, j, k, l;

	tab<<=8;

#if 0
	i=BGBBTJ_Huff_PeekWord(ctx);
	j=ctx->huff_idx[tab|(i>>8)];
	while(1)
	{
		l=tab|j;
		k=ctx->huff_len[l];
//		if(!k)break;

		if((i>>(16-k))!=ctx->huff_code[l])
		{
			j=ctx->huff_next[l];
			if(!j)break;
			continue;
		}
		BGBBTJ_Huff_SkipNBits(ctx, k);
		return(j);
	}
#endif

#if 1
	i=BGBBTJ_Huff_PeekWord(ctx);
	for(j=0; j<256; j++)
	{
		k=ctx->huff_len[tab|j];
		if(!k)continue;

		if((i>>(16-k))!=ctx->huff_code[tab|j])
			continue;
		BGBBTJ_Huff_SkipNBits(ctx, k);
		return(j);
	}
#endif

	return(-1);
}
#endif

int BGBBTJ_Huff_DecodeDiffDC(BGBBTJ_JPG_Context *ctx, int tab)
{
	int i, j;

	i=BGBBTJ_Huff_DecodeSymbol(ctx, tab);
	j=BGBBTJ_Huff_ReadNBits(ctx, i);

	if(!(j&(1<<(i-1))))
		j=(-1<<i)+j+1;
	return(j);
}

int BGBBTJ_Huff_DecodeBlock(BGBBTJ_JPG_Context *ctx,
	short *buf, int dctab, int actab,
	int ni, int nn)
{
	int i, j, k;

//	for(i=1; i<64; i++)buf[i]=0;
	memset(buf+1, 0, 63*sizeof(short));

	buf[0]+=BGBBTJ_Huff_DecodeDiffDC(ctx, dctab);
	for(i=1; i<64; i++)
	{
		j=BGBBTJ_Huff_DecodeSymbol(ctx, actab);
		if(j<=0)
		{
			if(j<0)
			{
				printf("bad dct block1 %02X(%d) %d/%d\n", j, j, ni, nn);
				return(-1);
			}
			break;
		}

		i+=(j>>4)&15;	//preceding 0's
		if(!(j&15))continue;
		if(i>=64)
		{
//			printf("bad dct block3 %02X(%d) len=%d %d/%d\n",
//				j, j, i, ni, nn);
			break;
		}

		j&=15;
		k=BGBBTJ_Huff_ReadNBits(ctx, j);
		if(!(k&(1<<(j-1))))
			k=(-1<<j)+k+1;

		j=pdjpg_zigzag2[i];
		buf[j]=k;
	}

#if 0
	for(j=i+1; j<64; j++)
	{
//		buf[i]=0;
		buf[pdjpg_zigzag2[j]]=0;
	}
#endif

	if(i>64)
	{
		printf("bad dct block2 %02X(%d) len=%d %d/%d\n", j, j, i, ni, nn);
		return(-1);
	}

	return(0);
}

void BGBBTJ_Huff_QuantBlock(
	BGBBTJ_JPG_Context *ctx, short *ibuf, short *obuf, int qid)
{
	int i, j;
	
//	for(i=0; i<64; i++)
//		obuf[i]=ibuf[i]/ctx->jpg_qt[qid][i];

	for(i=0; i<64; i++)
		obuf[i]=((ibuf[i]*ctx->jpg_qtfp[qid][i])+2048)>>12;
}

void BGBBTJ_Huff_DequantBlock(
	BGBBTJ_JPG_Context *ctx, short *ibuf, short *obuf, int qid)
{
	short *ib, *ob;
	byte *qt, *qt1;
	int i, j;
//	for(i=0; i<64; i++)
//		obuf[i]=ibuf[i]*ctx->jpg_qt[qid][i];

	qt=ctx->jpg_qt[qid];
	for(i=0; i<8; i++)
	{
		j=i*8; qt1=qt+j; ib=ibuf+j; ob=obuf+j;
		ob[0]=ib[0]*qt1[0];
		ob[1]=ib[1]*qt1[1];
		ob[2]=ib[2]*qt1[2];
		ob[3]=ib[3]*qt1[3];
		ob[4]=ib[4]*qt1[4];
		ob[5]=ib[5]*qt1[5];
		ob[6]=ib[6]*qt1[6];
		ob[7]=ib[7]*qt1[7];
	}
}
