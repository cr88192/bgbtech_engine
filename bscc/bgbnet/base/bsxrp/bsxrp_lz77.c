/**
  * Intermediate Data Format
  * Prior to Huffman coding, both LZ compressed and message-value data
  * will be represented in a 16-bit format. Within this format, the
  * low 12 bits will be used for payload, and the high 4 bits will hold
  * the word type.
  *
  * Word Types:
  * 0: Message Command Byte
  * 1: LZ77 Coded Byte
  * 2: LZ77 Run
  * 3: Value Payload Word
  * 4: Value Index
  * 5: Coded Integer
  *
  */

#include <bgbnet.h>

void BSXRP_UpdateWindowByte(BSXRP_StreamContext *ctx, byte *s)
{
	int h;

	h=(s[0]^(s[1]<<4)^(s[2]<<8))&0xFFF;
//	h=(s[0]^(s[1]<3)^(s[2]<<6))&0xFFF;
	ctx->lz_lbuf[ctx->lz_wpos]=ctx->lz_hash[h];
	ctx->lz_hash[h]=ctx->lz_wpos;
	ctx->lz_wbuf[ctx->lz_wpos]=*s;
//	ctx->lz_wpos++;
	ctx->lz_wpos=(ctx->lz_wpos+1)&65535;
}

void BSXRP_UpdateWindowString(BSXRP_StreamContext *ctx, byte *s, int l)
{
	int h;

	while(l--)
	{
		h=(s[0]^(s[1]<<4)^(s[2]<<8))&0xFFF;
//		h=(s[0]^(s[1]<<3)^(s[2]<<6))&0xFFF;
		ctx->lz_lbuf[ctx->lz_wpos]=ctx->lz_hash[h];
		ctx->lz_hash[h]=ctx->lz_wpos;
		ctx->lz_wbuf[ctx->lz_wpos]=*s++;
//		ctx->lz_wpos++;
		ctx->lz_wpos=(ctx->lz_wpos+1)&65535;
	}
}

int BSXRP_LookupString(BSXRP_StreamContext *ctx,
	byte *cs, byte *ce, int *rl, int *rd)
{
	byte *s, *t, *se;
	int i, j, l;
	int bl, bi, md, sd;

	se=cs+258;
	if(ce<se)se=ce;
	if((ce-cs)<3)return(0);

	md=ctx->lz_maxdist;
	sd=ctx->lz_sdepth;

	i=(cs[0]^(cs[1]<<4)^(cs[2]<<8))&0xFFF;
	i=ctx->lz_hash[i];

#if 1
	bl=0; bi=0; l=2;
	while(l && (sd--))
	{
		j=ctx->lz_wpos-i;
		if(j<=0)j+=65536;
		if(j>=md)break;

		if(i<(65536-258))
		{
			s=cs; t=ctx->lz_wbuf+i;
			while((s<se) && (*s==*t)) { s++; t++; }
			j=s-cs;
		}else
		{
			s=cs;
			for(j=0; ((*s++)==ctx->lz_wbuf[(i+j)&0xFFFF]) &&
				(s<se); j++);
		}

		if(j>bl)
		{
			bl=j; bi=i;

			if(bl>=258)break;
			if(bl>64)
			{
				if(sd>256)sd=256;
				if(bl>128)if(sd>128)sd=128;
				if(bl>192)
				{
					if(sd>64)sd=64;
					if(bl>224)if(sd>16)sd=16;
					if(bl>=256)if(sd>4)sd=4;
				}
			}
		}

		j=ctx->lz_lbuf[i];
		if(j>=i)l--;
		i=j;
	}
#endif

//	dyPrintf("ok\n");

	if(bl>2)
	{
		i=ctx->lz_wpos-bi;
		if(i<0)i+=65536;

		*rl=bl;
		*rd=i;

		return(1);
	}
	return(0);
}

int BSXRP_LZCompressBuffer(BSXRP_StreamContext *ctx,
	byte *ibuf, u16 *obuf, int isz, int osz)
{
	byte *cs, *ce;
	u16 *ct, *cte;
	int l, d, l2, d2, mi, ni;
	int mp, n, pi, pj;
	int i, j;

	cs=ibuf; ce=ibuf+isz;
	ct=obuf; cte=obuf+osz;

	mp=0; n=0; pi=ctx->lz_wpos; pj=0;
	mi=BSXRP_LookupString(ctx, cs, ce, &l, &d);
	while(cs<ce)
	{
		j=((ce-cs)>=260)?260:(ce-cs);
		for(i=pj; i<j; i++)
			{ ctx->lz_wbuf[pi]=cs[i]; pi=(pi+1)&0xFFFF; pj++; }

		ni=BSXRP_LookupString(ctx, cs+1, ce, &l2, &d2);
		if(mi && ni && (l2>(l+1)))mi=0;

		if(mi)
		{
//			*mt|=1<<(mp++);
//			if(mp>=8) { mt++; mp-=8; }
			n++;

			*ct++=0x2000|((l-3)&0x0FFF);
			*ct++=0x3000|(d&0x0FFF);
			*ct++=0x3000|((d>>12)&0x0FFF);
//			*ct++=d&0xFF;
//			*ct++=(d>>8)&0xFF;

			BSXRP_UpdateWindowString(ctx, cs, l);
			cs+=l;
			pj-=l;

			j=((ce-cs)>=260)?260:(ce-cs);
			for(i=pj; i<j; i++)
				{ ctx->lz_wbuf[pi]=cs[i];
				pi=(pi+1)&0xFFFF; pj++; }

			mi=BSXRP_LookupString(ctx, cs, ce, &l, &d);
			continue;
		}

		n++;

		BSXRP_UpdateWindowByte(ctx, cs);
		*ct++=0x1000|(*cs++);
		pj--;

		mi=ni; l=l2; d=d2+1;
	}

	*ct++=0x1100;

#if 1
	if((ct-obuf)>osz)
	{
		dyPrintf("Deflate: LZ Buffer Overflow\n");
		*(int *)-1=-1;
	}
#endif

	return(ct-obuf);
//	return(n);
}

static int lbase[]={
	0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007,
	0x0008, 0x000A, 0x000C, 0x000E, 0x0010, 0x0014, 0x0018, 0x001C,
	0x0020, 0x0028, 0x0030, 0x0038, 0x0040, 0x0050, 0x0060, 0x0070,
	0x0080, 0x00A0, 0x00C0, 0x00E0, 0x0100, 0x0140, 0x0180, 0x01C0,
	0x0200, 0x0280, 0x0300, 0x0380, 0x0400, 0x0500, 0x0600, 0x0700,
	0x0800, 0x0A00, 0x0C00, 0x0E00, 0x1000, 0x1400, 0x1800, 0x1C00,
	0x2000, 0x2800, 0x3000, 0x3800, 0x4000, 0x5000, 0x6000, 0x7000,
	0x8000, 0xA000, 0xC000, 0xE000};
static int lextra[]={
	 0,  0,  0,  0,  0,  0,  0,  0,
	 1,  1,  1,  1,  2,  2,  2,  2,
	 3,  3,  3,  3,  4,  4,  4,  4,
	 5,  5,  5,  5,  6,  6,  6,  6,
	 7,  7,  7,  7,  8,  8,  8,  8,
	 9,  9,  9,  9, 10, 10, 10, 10,
	11, 11, 11, 11, 12, 12, 12, 12,
	13, 13, 13, 13};

void BSXRP_StatLZRun(BSXRP_StreamContext *ctx,
	u16 *cs, int *lstat, int *dstat)
{
	int i, j, k;

	j=(cs[0]&0x0FFF)+3-2;
	for(i=0; i<48; i++)
	{
		k=lbase[i]+(1<<lextra[i]);
		if((j>=lbase[i]) && (j<k))
		{
			lstat[256+i]++;
			break;
		}
	}

	j=(cs[1]&0x0FFF)|((cs[2]&0x0FFF)<<12);
	for(i=0; i<60; i++)
	{
		k=lbase[i]+(1<<lextra[i]);
		if((j>=lbase[i]) && (j<k))
		{
			dstat[i]++;
			break;
		}
	}
}

void BSXRP_StatLZBuffer(BSXRP_StreamContext *ctx,
	u16 *tbuf, int n,
	int *cstat, int *lstat, int *dstat)
{
	u16 *s, *se;
	u64 lk;
	int i, j, k;

	for(i=0; i<384; i++)cstat[i]=0;
	for(i=0; i<384; i++)lstat[i]=0;
	for(i=0; i<384; i++)dstat[i]=0;

	s=tbuf; se=tbuf+n;
//	for(i=0; i<n; i++)
	while(s<se)
	{
		if(((*s)&0xF000)==0x5000)	//Coded Integer
		{
			lk=s[0]&0x0FFF;
			for(i=1; (s[i]&0xF000)==0x3000; i++)
				lk|=((u64)(s[i]&0x0FFF))<<(i*12);
			k=BSXRP_StatUIntPrefix(ctx, lk);
			dstat[k]++;
			s+=i;
			continue;
		}

		if(((*s)&0xF000)==0x4000) //Value Index
		{
			j=(s[0]&0x0FFF)|((s[1]&0x0FFF)<<12);
			k=BSXRP_StatUIntPrefix(ctx, j);
			cstat[128+k]++;
			s+=2;
		}

		if(((*s)&0xF000)==0x2000)	//LZ Run
		{
			BSXRP_StatLZRun(ctx, s, lstat, dstat);
			s+=3; continue;
		}

		if(((*s)&0xF000)==0x1000) //LZ byte
			{ lstat[(*s++)&0x0FFF]++; continue; }
		if(((*s)&0xF000)==0x0000) //Command
			{ cstat[(*s++)&0x0FFF]++; continue; }
	}
//	lstat[256]++;
}

void BSXRP_EncodeLZRun(BSXRP_StreamContext *ctx, u16 *cs)
{
	int i, j, k;

	j=(cs[0]&0x0FFF)+3-2;
	for(i=0; i<48; i++)
	{
		k=lbase[i]+(1<<lextra[i]);
		if((j>=lbase[i]) && (j<k))
		{
			BSXRP_EncodeSymbol(ctx, 1, 256+i);
			BSXRP_WriteNBits(ctx, j-lbase[i], lextra[i]);
			break;
		}
	}

	j=(cs[1]&0x0FFF)|((cs[2]&0x0FFF)<<12);
	for(i=0; i<60; i++)
	{
		k=lbase[i]+(1<<lextra[i]);
		if((j>=lbase[i]) && (j<k))
		{
			BSXRP_EncodeSymbol(ctx, 2, i);
			BSXRP_WriteNBits(ctx, j-lbase[i], lextra[i]);
			break;
		}
	}
}

void BSXRP_EncodeLZBuffer(BSXRP_StreamContext *ctx,
	u16 *tbuf, int n)
{
	u16 *s, *se;
	u64 lk;
	int i, j;

	s=tbuf; se=s+n;
	while(s<se)
	{
		if(((*s)&0xF000)==0x5000)	//Coded Integer
		{
//			lk=(s[0]&0x0FFF)|((s[1]&0x0FFF)<<12)|((s[2]&0x0FFF)<<24);
			lk=s[0]&0x0FFF;
			for(i=1; (s[i]&0xF000)==0x3000; i++)
				lk|=((u64)(s[i]&0x0FFF))<<(i*12);
			BSXRP_EncodeUInteger(ctx, lk);
//			BSXRP_EncodeLZRun(ctx, s);
			s+=i;
			continue;
		}

		if(((*s)&0xF000)==0x4000)	//Value Index
		{
			i=(s[0]&0x0FFF)|((s[1]&0x0FFF)<<12);
			BSXRP_EncodeValueIndex(ctx, i);
//			BSXRP_EncodeLZRun(ctx, s);
			s+=2;
			continue;
		}

		//0x3000: Value Payload

		if(((*s)&0xF000)==0x2000)		//LZ Run
		{
			BSXRP_EncodeLZRun(ctx, s);
			s+=3;
			continue;
		}

		if(((*s)&0xF000)==0x1000)		//LZ Data Byte
		{
			BSXRP_EncodeSymbol(ctx, 1, (*s++)&0x0FFF);
			continue;
		}

		if(((*s)&0xF000)==0x0000)		//Command Byte
		{
			BSXRP_EncodeSymbol(ctx, 0, (*s++)&0x0FFF);
			continue;
		}
	}
//	BSXRP_EncodeSymbol(ctx, 1, 256);
}

int BSXRP_DecodeRun(BSXRP_StreamContext *ctx, int sym)
{
//	static int lbase[]={
//		3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31,
//		35, 43, 51, 59, 67, 83, 99, 115,
//		131, 163, 195, 227, 258, 0, 0, 0};
//	static int lextra[]={
//		0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2,
//		3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0, 0, 0, 0};

	char *s;
	int i, j, k, l, si, ti;

	i=sym-256;
	j=lbase[i]+BSXRP_ReadNBits(ctx, lextra[i])+2;
	k=BSXRP_DecodeDistance(ctx);
//	s=ctx->ct-k;
//	i=j;
//	while(i--)*ctx->ct++=*s++;

	si=ctx->lz_wpos-k;
	if(si<0)si+=65536;
	ti=ctx->lz_wpos;
	i=j;
	while(i--)
	{
		l=ctx->lz_wbuf[si];
		ctx->lz_wbuf[ti]=l;
		*ctx->ct++=l;
		si=(si+1)&65535;
		ti=(ti+1)&65535;
	}
	ctx->lz_wpos=ti;

	return(0);
}

int BSXRP_DecodeBlockData(BSXRP_StreamContext *ctx)
{
	int i, j;

	while(ctx->ct<ctx->cte)
	{
		i=BSXRP_DecodeSymbol(ctx, 1);
		if(i<0)return(i);

		if(i<256)
		{
			j=ctx->lz_wpos;
			ctx->lz_wpos=(j+1)&65535;
			ctx->lz_wbuf[j]=i;
			*ctx->ct++=i;
			continue;
		}
		if(i==256)break;

		i=BSXRP_DecodeRun(ctx, i);
		if(i<0)return(i);
	}

	return(0);
}

