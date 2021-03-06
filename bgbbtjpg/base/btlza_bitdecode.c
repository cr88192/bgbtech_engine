#include <bgbbtj.h>

int btlza_trans8[256]={
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

int btlza_dbase1[64]=
{
	   1,    2,    3,     4,     5,     7,     9,    13,
	  17,   25,   33,    49,    65,    97,   129,   193,
	 257,  385,  513,   769,  1025,  1537,  2049,  3073,
	4097, 6145, 8193, 12289, 16385, 24577, 32769, 49153,
	0x00010001, 0x00018001, 0x00020001, 0x00030001, 
	0x00040001, 0x00060001, 0x00080001, 0x000C0001, 
	0x00100001, 0x00180001, 0x00200001, 0x00300001, 
	0x00400001, 0x00600001, 0x00800001, 0x00C00001, 
	0x01000001, 0x01800001, 0x02000001, 0x03000001, 
	0x04000001, 0x06000001, 0x08000001, 0x0C000001, 
	0x10000001, 0x18000001, 0x20000001, 0x30000001, 
	0x40000001, 0x60000001, 0x80000001, 0xC0000001
};
int btlza_dextra1[64]=
{
	 0,  0,  0,  0,  1,  1,  2,  2,  3,  3,  4,  4,  5,  5,  6,  6,
	 7,  7,  8,  8,  9,  9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14,
	15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22,
	23, 23, 24, 24, 25, 25, 26, 26, 27, 27, 28, 28, 29, 29, 30, 30
};

int btlza_lbase1[32]={
		3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31,
		35, 43, 51, 59, 67, 83, 99, 115,
		131, 163, 195, 227, 258, 0, 0, 0};
int btlza_lextra1[32]={
		0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2,
		3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0, 0, 0, 0};
int btlza_lbase2[32]={
		3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31,
		35, 43, 51, 59, 67, 83, 99, 115,
		131, 163, 195, 227, 3, 0, 0, 0};
int btlza_lextra2[32]={
		0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2,
		3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 16, 0, 0, 0};

int btlza_lbase3[64]={
	  3,   4,   5,   6,   7,   8,   9,  10,
	 11,  13,  15,  17,  19,  23,  27,  31,
	 35,  43,  51,  59,  67,  83,  99, 115,
	131, 163, 195, 227,
	   259,   323,   387,   451,
	   515,   643,   771,   899,
	  1027,  1283,  1539,  1795,
	  2051,  2563,  3075,  3587,
	  4099,  5123,  6147,  7171,
	  8195, 10243, 12291, 14339,
	 16387, 20483, 24579, 28675,
	 32771, 40963, 49155, 57347,
	 65538,     0,     0,     0
};
int btlza_lextra3[64]={
	 0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1, 2, 2, 2, 2,
	 3,  3,  3,  3,  4,  4,  4,  4,  5,  5,  5,  5, 6, 6, 6, 6,
	 7,  7,  7,  7,  8,  8,  8,  8,  9,  9,  9,  9, 10, 10, 10, 10,
	11, 11, 11, 11, 12, 12, 12, 12, 13, 13, 13, 13,  0,  0,  0,  0
};


int BTLZA_BitDec_TransposeByte(int v)
{
	return(btlza_trans8[v]);
}

int BTLZA_BitDec_TransposeWord(int v)
{
	int i;
	i=btlza_trans8[(v>>8)&0xFF]|(btlza_trans8[v&0xFF]<<8);
	return(i);
}

int BTLZA_BitDec_SetupTable(byte *cl, int ncl,
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

//		printf("%d %d %d\n", i, j, cnt[i]);
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
				k=BTLZA_BitDec_TransposeByte(k2++);
				if(ti[k]==0xFFFF)ti[k]=i;
			}
		}else
		{
			k2=tc[i]>>(l-8);
			k=BTLZA_BitDec_TransposeByte(k2);
			if(ti[k]==0xFFFF)ti[k]=i;
		}

		k2=(tc[i])<<(16-l);
		k=BTLZA_BitDec_TransposeWord(k2++);
		tc[i]=k;
	}

	j=0;
	for(i=0; i<256; i++)if(ti[i]==0xFFFF)
	{
		printf("table bad index %d\n", i);
		j=-9;
	}
	return(j);
}

int BTLZA_BitDec_SetupStatic(BGBBTJ_BTLZA_Context *ctx)
{
	byte lcl[288], dcl[32];
	int hl, hd;
	int i, j;

	hl=288;
	for(i=0; i<144; i++)lcl[i]=8;
	for(i=144; i<256; i++)lcl[i]=9;
	for(i=256; i<280; i++)lcl[i]=7;
	for(i=280; i<288; i++)lcl[i]=8;

	hd=32;
	for(i=0; i<32; i++)dcl[i]=5;

	j=BTLZA_BitDec_SetupTable(lcl, hl,
		ctx->bs_ltab_code, ctx->bs_ltab_mask, ctx->bs_ltab_len,
		ctx->bs_ltab_idx, ctx->bs_ltab_next);
	if(j<0)
	{
		printf("Static Literal/Length Table Problem %d\n", j);
		return(j);
	}

	j=BTLZA_BitDec_SetupTable(dcl, hd,
		ctx->bs_dtab_code, ctx->bs_dtab_mask, ctx->bs_dtab_len,
		ctx->bs_dtab_idx, ctx->bs_dtab_next);
	if(j<0)
	{
		printf("Static Distance Table Problem %d\n", j);
		return(j);
	}

	return(0);
}

int BTLZA_BitDec_DecodeSymbol(BGBBTJ_BTLZA_Context *ctx)
	{ return(ctx->BS_DecodeSymbol(ctx)); }
int BTLZA_BitDec_DecodeDistanceSymbol(BGBBTJ_BTLZA_Context *ctx)
	{ return(ctx->BS_DecodeDistanceSymbol(ctx)); }

int BTLZA_BitDec_DecodeSymbolBasic(BGBBTJ_BTLZA_Context *ctx)
{
	int i, j;

	i=BTLZA_BitDec_PeekWord(ctx);
//	i=(ctx->bs_win>>ctx->bs_pos)&65535;
	j=ctx->bs_ltab_idx[i&0xFF];

	if((i&ctx->bs_ltab_mask[j])==ctx->bs_ltab_code[j])
	{
		BTLZA_BitDec_SkipNBits(ctx, ctx->bs_ltab_len[j]);
		return(j);
	}else
	{
		j=ctx->bs_ltab_next[j];
	}

	while((i&ctx->bs_ltab_mask[j])!=ctx->bs_ltab_code[j])
		j=ctx->bs_ltab_next[j];

	BTLZA_BitDec_SkipNBits(ctx, ctx->bs_ltab_len[j]);
	return(j);
}

int BTLZA_BitDec_DecodeDistanceSymbolBasic(BGBBTJ_BTLZA_Context *ctx)
{
	int i, j;

	i=BTLZA_BitDec_PeekWord(ctx);
//	i=(ctx->bs_win>>ctx->bs_pos)&65535;
	j=ctx->bs_dtab_idx[i&0xFF];

	if((i&ctx->bs_dtab_mask[j])==ctx->bs_dtab_code[j])
	{
		BTLZA_BitDec_SkipNBits(ctx, ctx->bs_dtab_len[j]);
		return(j);
	}else
	{
		j=ctx->bs_dtab_next[j];
	}

	while((i&ctx->bs_dtab_mask[j])!=ctx->bs_dtab_code[j])
		j=ctx->bs_dtab_next[j];

	BTLZA_BitDec_SkipNBits(ctx, ctx->bs_dtab_len[j]);
	return(j);
}

int BTLZA_BitDec_DecodeSymbolBasic2(BGBBTJ_BTLZA_Context *ctx)
{
	int i, j;

//	i=BTLZA_BitDec_PeekWord(ctx);
	i=(ctx->bs_win>>ctx->bs_pos)&65535;
	j=ctx->bs_ltab_idx[i&0xFF];

	if((i&ctx->bs_ltab_mask[j])==ctx->bs_ltab_code[j])
	{
		BTLZA_BitDec_SkipNBitsBasic(ctx, ctx->bs_ltab_len[j]);
		return(j);
	}else
	{
		j=ctx->bs_ltab_next[j];
	}

	while((i&ctx->bs_ltab_mask[j])!=ctx->bs_ltab_code[j])
		j=ctx->bs_ltab_next[j];

	BTLZA_BitDec_SkipNBitsBasic(ctx, ctx->bs_ltab_len[j]);
	return(j);
}

int BTLZA_BitDec_DecodeDistanceSymbolBasic2(BGBBTJ_BTLZA_Context *ctx)
{
	int i, j;

//	i=BTLZA_BitDec_PeekWord(ctx);
	i=(ctx->bs_win>>ctx->bs_pos)&65535;
	j=ctx->bs_dtab_idx[i&0xFF];

	if((i&ctx->bs_dtab_mask[j])==ctx->bs_dtab_code[j])
	{
		BTLZA_BitDec_SkipNBitsBasic(ctx, ctx->bs_dtab_len[j]);
		return(j);
	}else
	{
		j=ctx->bs_dtab_next[j];
	}

	while((i&ctx->bs_dtab_mask[j])!=ctx->bs_dtab_code[j])
		j=ctx->bs_dtab_next[j];

	BTLZA_BitDec_SkipNBitsBasic(ctx, ctx->bs_dtab_len[j]);
	return(j);
}

int BTLZA_BitDec_DecodeSymbolReadBit(BGBBTJ_BTLZA_Context *ctx)
{
	int i, j, k, l;

	i=0;
	for(l=1; l<16; l++)
	{
		i|=BTLZA_BitDec_ReadLiteralBit(ctx)<<(l-1);
		j=ctx->bs_ltab_idx[i&0xFF];
		while(j!=0xFFFF)
		{
			if(ctx->bs_ltab_len[j]>l)
				{ j=ctx->bs_ltab_next[j]; continue; }
			if((i&ctx->bs_ltab_mask[j])!=ctx->bs_ltab_code[j])
				{ j=ctx->bs_ltab_next[j]; continue; }
			break;
		}		
		if(j!=0xFFFF)
			break;
	}
	return(j);
}

int BTLZA_BitDec_DecodeDistanceSymbolReadBit(BGBBTJ_BTLZA_Context *ctx)
{
	int i, j, k, l;

	i=0;
	for(l=1; l<16; l++)
	{
		i|=BTLZA_BitDec_ReadDistanceBit(ctx)<<(l-1);
		j=ctx->bs_dtab_idx[i&0xFF];
		while(j!=0xFFFF)
		{
			if(ctx->bs_dtab_len[j]>l)
				{ j=ctx->bs_dtab_next[j]; continue; }
			if((i&ctx->bs_dtab_mask[j])!=ctx->bs_dtab_code[j])
				{ j=ctx->bs_dtab_next[j]; continue; }
			break;
		}		
		if(j!=0xFFFF)
			break;
	}
	return(j);
}

int BTLZA_BitDec_DecodeDistance(BGBBTJ_BTLZA_Context *ctx)
{

	int i, j, k;

//	i=BTLZA_BitDec_PeekWord(ctx);
//	j=ctx->bs_dtab_idx[i&0xFF];

//	while((i&ctx->bs_dtab_mask[j])!=ctx->bs_dtab_code[j])
//		j=ctx->bs_dtab_next[j];
//	BTLZA_BitDec_SkipNBits(ctx, ctx->bs_dtab_len[j]);

	j=BTLZA_BitDec_DecodeDistanceSymbol(ctx);
	k=btlza_dbase1[j]+BTLZA_BitDec_ReadExtraNBits(ctx, btlza_dextra1[j]);

//	k=btlza_dbase1[j]+BTLZA_BitDec_ReadNBits(ctx, btlza_dextra1[j]);

	return(k);
}

int BTLZA_BitDec_DecodeCodeLengths(BGBBTJ_BTLZA_Context *ctx,
	byte *cl, int ncl)
{
	int i, j, k;

	i=0; k=0;
	while(i<ncl)
	{
		j=BTLZA_BitDec_DecodeSymbol(ctx);
		if(j<0)return(j);

		if(j<16)
		{
			k=j;
			cl[i++]=j;
			continue;
		}

		if(j==16)
		{
			j=BTLZA_BitDec_Read2Bits(ctx)+3;
			while(j--)cl[i++]=k;
			continue;
		}

		if(j==17)
		{
			j=BTLZA_BitDec_Read3Bits(ctx)+3;
			while(j--)cl[i++]=0;
			k=0;
			continue;
		}
		if(j==18)
		{
			j=BTLZA_BitDec_ReadNBits(ctx, 7)+11;
			while(j--)cl[i++]=0;
			k=0;
			continue;
		}

		return(-7);
	}
	return(0);
}

int BTLZA_BitDec_DecodeHeader(BGBBTJ_BTLZA_Context *ctx)
{
	static int lorder[]={
		16, 17, 18, 0, 8,7, 9,6, 10,5, 11,4, 12,3, 13,2, 14,1, 15};

	byte hcl[32], lcl[512], dcl[32];
	int hl, hd, hc;
	int i, j;

	hl=BTLZA_BitDec_ReadNBits(ctx, 5)+257;
	hd=BTLZA_BitDec_ReadNBits(ctx, 5)+1;
	hc=BTLZA_BitDec_ReadNBits(ctx, 4)+4;

//	printf("%d %d %d\n", hl, hd, hc);

	for(i=0; i<32; i++)hcl[i]=0;
	for(i=0; i<hc; i++)
		hcl[lorder[i]]=BTLZA_BitDec_ReadNBits(ctx, 3);

//	for(i=0; i<20; i++)printf("%d:%d ", i, hcl[i]);
//	printf("\n");

	j=BTLZA_BitDec_SetupTable(hcl, 32,
		ctx->bs_ltab_code, ctx->bs_ltab_mask, ctx->bs_ltab_len,
		ctx->bs_ltab_idx, ctx->bs_ltab_next);
	if(j<0)
	{
		printf("Codes Table Problem %d\n", j);
		for(i=0; i<20; i++)printf("%d:%d ", i, hcl[i]);
		printf("\n");
		return(j);
	}

	i=BTLZA_BitDec_DecodeCodeLengths(ctx, lcl, hl);
	if(i<0)return(i);
	i=BTLZA_BitDec_DecodeCodeLengths(ctx, dcl, hd);
	if(i<0)return(i);

//	for(i=0; i<hl; i++)
//	{
//		j=lcl[i];
//		if(j)printf("%d:%d ", i, j);
//	}
//	printf("\n");

//	for(i=0; i<hd; i++)printf("%d:%d ", i, dcl[i]);
//	printf("\n");

	j=BTLZA_BitDec_SetupTable(lcl, hl,
		ctx->bs_ltab_code, ctx->bs_ltab_mask, ctx->bs_ltab_len,
		ctx->bs_ltab_idx, ctx->bs_ltab_next);
	if(j<0)
	{
		printf("Literal/Length Table Problem %d\n", j);
		for(i=0; i<hl; i++)printf("%d:%d ", i, lcl[i]);
		printf("\n");
		return(j);
	}

	j=BTLZA_BitDec_SetupTable(dcl, hd,
		ctx->bs_dtab_code, ctx->bs_dtab_mask, ctx->bs_dtab_len,
		ctx->bs_dtab_idx, ctx->bs_dtab_next);
	if(j<0)
	{
		printf("Distance Table Problem %d\n", j);
		for(i=0; i<hd; i++)printf("%d:%d ", i, dcl[i]);
		printf("\n");
		return(j);
	}

	return(0);
}


int BTLZA_BitDec_DecodeHeaderBTLZH(BGBBTJ_BTLZA_Context *ctx)
{
	static int lorder[]={
		16, 17, 18, 0, 8,7, 9,6, 10,5, 11,4, 12,3, 13,2, 14,1, 15};

	byte hcl[64], lcl[512], dcl[64];
	int hl, hd, hc, hr;
	int i, j;

	hr=BTLZA_BitDec_ReadNBits(ctx, 3);
	if(hr!=0)return(-1);
	
	hl=BTLZA_BitDec_ReadNBits(ctx, 7)+257;
	hd=BTLZA_BitDec_ReadNBits(ctx, 6)+1;
	hc=BTLZA_BitDec_ReadNBits(ctx, 5)+4;

//	printf("%d %d %d\n", hl, hd, hc);

	for(i=0; i<64; i++)hcl[i]=0;
	for(i=0; i<hc; i++)
		hcl[lorder[i]]=BTLZA_BitDec_ReadNBits(ctx, 3);

//	for(i=0; i<20; i++)printf("%d:%d ", i, hcl[i]);
//	printf("\n");

	j=BTLZA_BitDec_SetupTable(hcl, 64,
		ctx->bs_ltab_code, ctx->bs_ltab_mask, ctx->bs_ltab_len,
		ctx->bs_ltab_idx, ctx->bs_ltab_next);
	if(j<0)
	{
		printf("Codes Table Problem %d\n", j);
		for(i=0; i<20; i++)printf("%d:%d ", i, hcl[i]);
		printf("\n");
		return(j);
	}

	i=BTLZA_BitDec_DecodeCodeLengths(ctx, lcl, hl);
	if(i<0)return(i);
	i=BTLZA_BitDec_DecodeCodeLengths(ctx, dcl, hd);
	if(i<0)return(i);

//	for(i=0; i<hl; i++)
//	{
//		j=lcl[i];
//		if(j)printf("%d:%d ", i, j);
//	}
//	printf("\n");

//	for(i=0; i<hd; i++)printf("%d:%d ", i, dcl[i]);
//	printf("\n");

	j=BTLZA_BitDec_SetupTable(lcl, hl,
		ctx->bs_ltab_code, ctx->bs_ltab_mask, ctx->bs_ltab_len,
		ctx->bs_ltab_idx, ctx->bs_ltab_next);
	if(j<0)
	{
		printf("Literal/Length Table Problem %d\n", j);
		for(i=0; i<hl; i++)printf("%d:%d ", i, lcl[i]);
		printf("\n");
		return(j);
	}

	j=BTLZA_BitDec_SetupTable(dcl, hd,
		ctx->bs_dtab_code, ctx->bs_dtab_mask, ctx->bs_dtab_len,
		ctx->bs_dtab_idx, ctx->bs_dtab_next);
	if(j<0)
	{
		printf("Distance Table Problem %d\n", j);
		for(i=0; i<hd; i++)printf("%d:%d ", i, dcl[i]);
		printf("\n");
		return(j);
	}

	return(0);
}

int BTLZA_BitDec_DecodeStaticHeader2(BGBBTJ_BTLZA_Context *ctx)
{
	byte lcl[384], dcl[64];
	int hl, hd, tl, td;
	int i, j;

	tl=BTLZA_BitDec_ReadNBits(ctx, 3);
	td=BTLZA_BitDec_ReadNBits(ctx, 2);

	switch(tl)
	{
	case 0:
		hl=288;
		for(i=0; i<144; i++)lcl[i]=8;
		for(i=144; i<256; i++)lcl[i]=9;
		for(i=256; i<280; i++)lcl[i]=7;
		for(i=280; i<288; i++)lcl[i]=8;
		break;
	case 1:
		hl=384;
		for(i=0; i<127; i++)lcl[i]=8;
		for(i=128; i<384; i++)lcl[i]=9;
		break;
	case 2:
		hl=384;
		for(i=0; i<255; i++)lcl[i]=9;
		for(i=256; i<384; i++)lcl[i]=8;
		break;
	default:
		return(-1);
		break;
	}

	switch(td)
	{
	case 0:
		hd=64;
		for(i=0; i<64; i++)dcl[i]=6;
		break;
	case 1:
		hd=32;
		for(i=0; i<32; i++)dcl[i]=5;
		break;
	case 2:
		hd=16;
		for(i=0; i<16; i++)dcl[i]=4;
		break;
	case 3:
		hd=8;
		for(i=0; i<8; i++)dcl[i]=3;
		break;
	default:
		break;
	}

	j=BTLZA_BitDec_SetupTable(lcl, hl,
		ctx->bs_ltab_code, ctx->bs_ltab_mask, ctx->bs_ltab_len,
		ctx->bs_ltab_idx, ctx->bs_ltab_next);
	if(j<0)
	{
		printf("Static2 Literal/Length Table Problem %d\n", j);
		return(j);
	}

	j=BTLZA_BitDec_SetupTable(dcl, hd,
		ctx->bs_dtab_code, ctx->bs_dtab_mask, ctx->bs_dtab_len,
		ctx->bs_dtab_idx, ctx->bs_dtab_next);
	if(j<0)
	{
		printf("Static2 Distance Table Problem %d\n", j);
		return(j);
	}

	return(0);
}

void BTLZA_BitDec_MemCpy(byte *dst, byte *src, int len)
{
	byte *s, *t;
#ifdef X86_64
	s64 d;
#else
	int d;
#endif
	int i, j, k, l;

#if defined(X86) || defined(X86_64) || defined(ARM)
	s=src; t=dst; i=len;
	d=dst-src;
#if 1
	if(d>=16)
	{
		while(i>=16)
		{
//			((double *)t)[0]=((double *)s)[0];
//			((double *)t)[1]=((double *)s)[1];
			((s64 *)t)[0]=((s64 *)s)[0];
			((s64 *)t)[1]=((s64 *)s)[1];
			t+=16; s+=16; i-=16;
		}
		if(i>=8)
//			{ *((double *)t)++=*((double *)s)++; i-=8; }
//			{ *((s64 *)t)++=*((s64 *)s)++; i-=8; }
			{ *((s64 *)t)=*((s64 *)s); t+=8; s+=8; i-=8; }
		if(i>=4)
//			{ *((int *)t)++=*((int *)s)++; i-=4; }
			{ *((int *)t)=*((int *)s); t+=4; s+=4; i-=4; }
		if(i>=2)
			{ *t++=*s++; *t++=*s++; i-=2; }
		if(i) { *t++=*s++; }
	}else
	{
		while(i>=16)
		{
			t[ 0]=s[ 0];	t[ 1]=s[ 1];
			t[ 2]=s[ 2];	t[ 3]=s[ 3];
			t[ 4]=s[ 4];	t[ 5]=s[ 5];
			t[ 6]=s[ 6];	t[ 7]=s[ 7];
			t[ 8]=s[ 8];	t[ 9]=s[ 9];
			t[10]=s[10];	t[11]=s[11];
			t[12]=s[12];	t[13]=s[13];
			t[14]=s[14];	t[15]=s[15];
			t+=16; s+=16; i-=16;
		}
		if(i>=8)
		{
			t[ 0]=s[ 0];	t[ 1]=s[ 1];
			t[ 2]=s[ 2];	t[ 3]=s[ 3];
			t[ 4]=s[ 4];	t[ 5]=s[ 5];
			t[ 6]=s[ 6];	t[ 7]=s[ 7];
			t+=8; s+=8; i-=8;
		}
		if(i>=4)
		{
			t[ 0]=s[ 0];	t[ 1]=s[ 1];
			t[ 2]=s[ 2];	t[ 3]=s[ 3];
			t+=4; s+=4; i-=4;
		}
		if(i>=2)
		{
			*t++=*s++;	*t++=*s++;
			i-=2;
		}
		if(i)*t++=*s++;
//		while(i--)*t++=*s++;
	}
#endif
#if 0
	if(d>=16)
	{
		while(i>=16)
		{
			((double *)t)[0]=((double *)s)[0];
			((double *)t)[1]=((double *)s)[1];
//			((s64 *)t)[0]=((s64 *)s)[0];
//			((s64 *)t)[1]=((s64 *)s)[1];
			t+=16; s+=16; i-=16;
		}
		if(i>=8)
			{ *((double *)t)++=*((double *)s)++; i-=8; }
		if(i>=4)
			{ *((int *)t)++=*((int *)s)++; i-=4; }
		if(i>=2)
			{ *t++=*s++; *t++=*s++; i-=2; }
		if(i) { *t++=*s++; }
	}else if(0) // if(i>=32)
	{
		t[ 0]=s[ 0];	t[ 1]=s[ 1];
		t[ 2]=s[ 2];	t[ 3]=s[ 3];
		t[ 4]=s[ 4];	t[ 5]=s[ 5];
		t[ 6]=s[ 6];	t[ 7]=s[ 7];
		t[ 8]=s[ 8];	t[ 9]=s[ 9];
		t[10]=s[10];	t[11]=s[11];
		t[12]=s[12];	t[13]=s[13];
		t[14]=s[14];	t[15]=s[15];
	
		while(i>=16)
		{
			((double *)t)[0]=((double *)s)[0];
			((double *)t)[1]=((double *)s)[1];
//			((s64 *)t)[0]=((s64 *)s)[0];
//			((s64 *)t)[1]=((s64 *)s)[1];
			t+=16; s+=16; i-=16;
		}
		if(i>=8)
			{ *((double *)t)++=*((double *)s)++; i-=8; }
		if(i>=4)
			{ *((int *)t)++=*((int *)s)++; i-=4; }
		if(i>=2)
			{ *t++=*s++; *t++=*s++; i-=2; }
		if(i) { *t++=*s++; }
	}else if(k>=8)
	{
		while(i>=8)
			{ *((double *)t)++=*((double *)s)++; i-=8; }
		if(i>=4)
			{ *((int *)t)++=*((int *)s)++; i-=4; }
		if(i>=2)
			{ *t++=*s++; *t++=*s++; i-=2; }
		if(i) { *t++=*s++; }
	}else if(k>=4)
	{
		while(i>=4)
			{ *((int *)t)++=*((int *)s)++; i-=4; }
		if(i>=2)
			{ *t++=*s++; *t++=*s++; i-=2; }
		if(i) { *t++=*s++; }
	}else
	{
		while(i--)*t++=*s++;
	}
#endif
#else
	s=src; t=dst; i=len;
	while(i--)*t++=*s++;
#endif
}

int BTLZA_BitDec_DecodeRun(BGBBTJ_BTLZA_Context *ctx, int sym)
{
	int *lbase, *lextra;
	char *s, *t;
	int i, j, k, l;

	if(sym<318)
	{
		i=sym-257;
		j=ctx->lbase[i]+BTLZA_BitDec_ReadNBits(ctx, ctx->lextra[i]);
		k=BTLZA_BitDec_DecodeDistance(ctx);
		ctx->lz_lastdist=k;
		ctx->lz_lastrun=j;
		s=ctx->ct-k;
		i=j;
//		while(i--)*ctx->ct++=*s++;
		BTLZA_BitDec_MemCpy(ctx->ct, s, i);
		ctx->ct+=i;

		return(0);
	}

	if(sym<334)
	{
		i=sym-318;
		j=btlza_dbase1[i]+BTLZA_BitDec_ReadExtraNBits(ctx, btlza_dextra1[i]);
		
		switch(j)
		{
		case 1:		/* Prior Length and Distance */
			k=ctx->lz_lastdist;
			l=ctx->lz_lastrun;
			s=ctx->ct-k; i=l;
//			while(i--)*ctx->ct++=*s++;
			BTLZA_BitDec_MemCpy(ctx->ct, s, i);
			ctx->ct+=i;
			break;
		case 2:		/* Prior Length */
			k=BTLZA_BitDec_DecodeDistance(ctx);
			ctx->lz_lastdist=k;
			l=ctx->lz_lastrun;
			s=ctx->ct-k; i=l;
//			while(i--)*ctx->ct++=*s++;
			BTLZA_BitDec_MemCpy(ctx->ct, s, i);
			ctx->ct+=i;
			break;
		case 3:		/* Prior Distance */
			i=BTLZA_BitDec_DecodeSymbol(ctx);
			if(i<0)return(i);
			i=i-257;
			l=ctx->lbase[i]+BTLZA_BitDec_ReadNBits(ctx, ctx->lextra[i]);
			ctx->lz_lastrun=l;
			k=ctx->lz_lastdist;
			s=ctx->ct-k; i=l;
//			while(i--)*ctx->ct++=*s++;
			BTLZA_BitDec_MemCpy(ctx->ct, s, i);
			ctx->ct+=i;
			break;
		default:
			return(-1);
			break;
		}
		return(0);
	}

	return(-1);
}

int BTLZA_BitDec_DecodeBlockData(BGBBTJ_BTLZA_Context *ctx)
{
	int i;

	if(ctx->bs_flags&BGBBTJ_ZFL_ISBTLZH)
		{ ctx->lbase=btlza_lbase3; ctx->lextra=btlza_lextra3; }
	else if(ctx->bs_flags&BGBBTJ_ZFL_DEFLATE64)
		{ ctx->lbase=btlza_lbase2; ctx->lextra=btlza_lextra2; }
	else
		{ ctx->lbase=btlza_lbase1; ctx->lextra=btlza_lextra1; }

	while(1)
	{
		i=BTLZA_BitDec_DecodeSymbol(ctx);
		if(i<0)return(i);

		if(i<256)
		{
			*ctx->ct++=i;
			continue;
		}
		if(i==256)break;

		i=BTLZA_BitDec_DecodeRun(ctx, i);
		if(i<0)return(i);
	}

	return(0);
}

int BTLZA_BitDec_AlignBitsEnd(BGBBTJ_BTLZA_Context *ctx)
{
	int i, j;

	//N/A in arithmetic mode
	if(ctx->rmax)
		return(0);

	while(ctx->bs_pos<=24)
	{
		ctx->cs--;
		ctx->bs_pos+=8;
		ctx->bs_win<<=8;
	}
	return(0);
}

int BTLZA_BitDec_DecodeHeaderBTArith(BGBBTJ_BTLZA_Context *ctx)
{
	int hl, hd, hc, he, hr;
	int i, j;

	hr=BTLZA_BitDec_Read3Bits(ctx);
	if(hr==0)
	{
		//Switch to Byte mode
		BTLZA_BitArith_SetupEndDecode(ctx);
		if(ctx->bs_flags&BGBBTJ_ZFL_ESCAPE_FF)
			{ ctx->BS_ReadByte=BTLZA_BitDec_ReadByteEscape; }
		else
			{ ctx->BS_ReadByte=BTLZA_BitDec_ReadByteBasic; }
		return(0);
	}
	
	if(hr==1)
	{
		//Switch to Arithmetic Mode
		hl=BTLZA_BitDec_ReadNBits(ctx, 4)+12;
//		he=BTLZA_BitDec_ReadBit(ctx);
		hd=BTLZA_BitDec_ReadNBits(ctx, 5);

		BTLZA_BitDec_AlignBitsEnd(ctx);

		BTLZA_BitArith_SetupRawContextBits(ctx, hl);
		BTLZA_BitArith_SetupBeginDecode(ctx);
		ctx->BS_ReadByte=BTLZA_BitDec_ReadByteArithLE;
		ctx->BS_DecodeSymbol=BTLZA_BitDec_DecodeSymbolBasic;
		ctx->BS_DecodeDistanceSymbol=BTLZA_BitDec_DecodeDistanceSymbolBasic;

		ctx->bs_win=0;
		ctx->bs_pos=32;
		BTLZA_BitDec_ReadAdjust(ctx);

//		ctx->BS_ReadByte=he?
//			BTLZA_BitDec_ReadByteArithLE:
//			BTLZA_BitDec_ReadByteArithBE;
		return(0);
	}
	
	return(-1);
}

int BTLZA_BitDec_DecodeBlock(BGBBTJ_BTLZA_Context *ctx)
{
	int fi, ty, ty2, l, nl;
	int i;

	fi=BTLZA_BitDec_ReadBit(ctx);
	ty=BTLZA_BitDec_Read2Bits(ctx);

//	printf("blk %d %d\n", fi, ty);

	switch(ty)
	{
	case 0:
		BTLZA_BitDec_AlignByte(ctx);
		l=BTLZA_BitDec_ReadAlignedWord(ctx);
		nl=(~BTLZA_BitDec_ReadAlignedWord(ctx))&0xFFFF;
		if(l!=nl)
			{ return(-3); }

		while(l--)
		{
			i=BTLZA_BitDec_ReadAlignedByte(ctx);
			*ctx->ct++=i;
		}
		break;
	case 1:
		i=BTLZA_BitDec_SetupStatic(ctx);
		if(i<0)return(i);
		i=BTLZA_BitDec_DecodeBlockData(ctx);
		if(i<0)return(i);
		break;
	case 2:
		i=BTLZA_BitDec_DecodeHeader(ctx);
		if(i<0)return(i);
		i=BTLZA_BitDec_DecodeBlockData(ctx);
		if(i<0)return(i);
		break;

	case 3:
		ty2=BTLZA_BitDec_Read3Bits(ctx);
		if(ty2==7)
		{
			if(ctx->bs_flags&BGBBTJ_ZFL_ESCBTLZA)
				return(2);
			return(-2);
		}

		if(ty2==0)
		{
			i=BTLZA_BitDec_ReadNBits(ctx, 6);
			l=btlza_dbase1[i]+BTLZA_BitDec_ReadNBits(ctx, btlza_dextra1[i]);
			while(l--)
			{
				i=BTLZA_BitDec_ReadNBits(ctx, 8);
				*ctx->ct++=i;
			}
			break;
		}
			
		if(ty2==1)
		{
			i=BTLZA_BitDec_DecodeHeaderBTArith(ctx);
			if(i<0)return(i);
			break;
		}
			
		if(ty2==2)
		{
			i=BTLZA_BitDec_DecodeHeaderBTLZH(ctx);
			if(i<0)return(i);
			ctx->bs_flags|=BGBBTJ_ZFL_ISBTLZH;
			i=BTLZA_BitDec_DecodeBlockData(ctx);
			ctx->bs_flags&=~BGBBTJ_ZFL_ISBTLZH;
			if(i<0)return(i);
			break;
		}

		if(ty2==3)
		{
			i=BTLZA_BitDec_DecodeStaticHeader2(ctx);
			if(i<0)return(i);
			i=BTLZA_BitDec_DecodeBlockData(ctx);
			if(i<0)return(i);
			break;
		}

//		if(ty2==3)
//		{
//			i=BTLZA_Decode_DecodeBlockData(ctx);
//			if(i<0)return(i);
//			break;
//		}

		return(-2);
		break;

	default:
		return(-2);
		break;
	}

	return(fi);
}

int BTLZA_BitDec_DecodeStream(BGBBTJ_BTLZA_Context *ctx,
	byte *ibuf, byte *obuf, int isz, int osz)
{
	return(BTLZA_BitDec_DecodeStreamSz(ctx, ibuf, obuf, isz, osz, NULL, 0));
}

int BTLZA_BitDec_DecodeStreamSz(BGBBTJ_BTLZA_Context *ctx,
	byte *ibuf, byte *obuf, int isz,
	int osz, int *rosz, int flags)
{
	int i, k;

	if(flags&BGBBTJ_ZFL_ESCAPE_FF)
	{
		ctx->BS_ReadByte=BTLZA_BitDec_ReadByteEscape;
	}else
	{
		ctx->BS_ReadByte=BTLZA_BitDec_ReadByteBasic;
	}
	
	ctx->BS_DecodeSymbol=BTLZA_BitDec_DecodeSymbolBasic2;
	ctx->BS_DecodeDistanceSymbol=BTLZA_BitDec_DecodeDistanceSymbolBasic2;
	ctx->BS_ReadExtraNBits=BTLZA_BitDec_ReadExtraNBitsBasic;
	ctx->BS_ReadNBits=BTLZA_BitDec_ReadNBitsBasic;
	ctx->BS_SkipNBits=BTLZA_BitDec_SkipNBitsBasic;
	ctx->BS_ReadBit=BTLZA_BitDec_ReadBitBasic;

	ctx->ibuf=ibuf;
	ctx->obuf=obuf;
//	ctx->cs=ibuf+4;
	ctx->cs=ibuf;
	ctx->ct=obuf;
	ctx->cse=ibuf+isz;
	ctx->cte=obuf+osz;

	ctx->bs_win=(ctx->bs_win>>8)|(BTLZA_BitDec_ReadByte(ctx)<<24);
	ctx->bs_win=(ctx->bs_win>>8)|(BTLZA_BitDec_ReadByte(ctx)<<24);
	ctx->bs_win=(ctx->bs_win>>8)|(BTLZA_BitDec_ReadByte(ctx)<<24);
	ctx->bs_win=(ctx->bs_win>>8)|(BTLZA_BitDec_ReadByte(ctx)<<24);
	ctx->bs_pos=0;
	
	ctx->bs_flags=flags;

	k=0;
	while(1)
	{
		i=BTLZA_BitDec_DecodeBlock(ctx);
		if(i<0)
		{
			if(rosz)
				{ *rosz=ctx->ct-ctx->obuf; }
			return(i);
		}
		if(i)break;
	}

	i=ctx->bs_pos;
	while(i<=24)
	{
		ctx->cs--;
		i+=8;
	}

	if(rosz)
		*rosz=ctx->ct-ctx->obuf;

	i=ctx->cs-ibuf;
	return(i);
}
