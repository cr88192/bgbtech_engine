#include <bgbnet.h>

BSXRP_StreamContext *BSXRP_AllocStreamContext()
{
	BSXRP_StreamContext *ctx;
	
	ctx=gctalloc("bsxrp_streamcontext_t", sizeof(BSXRP_StreamContext));
	return(ctx);
}

BSXRP_StreamContext *BSXRP_CreateStreamContext()
{
	BSXRP_StreamContext *ctx;
	ctx=BSXRP_AllocStreamContext();
	BSXRP_DecodeHuffTableFixed(ctx, 0, 0);
	BSXRP_DecodeHuffTableFixed(ctx, 1, 1);
	BSXRP_DecodeHuffTableFixed(ctx, 2, 2);
	
	ctx->lz_maxdist=65536-4096;
	ctx->lz_sdepth=4096;
	
	return(ctx);
}

dyt BSXRP_ShiftBackMRU(BSXRP_StreamContext *ctx, int idx)
{
	dyt t;
	int i;

	t=ctx->val_mru[idx];
	for(i=idx; i>=1; i--)
		ctx->val_mru[i]=ctx->val_mru[i-1];
	ctx->val_mru[0]=NULL;
	return(t);
}

dyt BSXRP_ShiftIndexMTF(BSXRP_StreamContext *ctx, int idx)
{
	dyt t;
	int i;
	
	t=ctx->val_mru[idx];
	for(i=idx; i>=1; i--)
		ctx->val_mru[i]=ctx->val_mru[i-1];
	ctx->val_mru[0]=t;
	return(t);
}

int BSXRP_IndexValueMRU(BSXRP_StreamContext *ctx, dyt val)
{
	int i;

	if(!dyconsp(val))
	{
		for(i=0; i<ctx->val_szmru; i++)
		{
			if(val==ctx->val_mru[i])
				return(i);
		}
		return(-1);
	}

	for(i=0; i<ctx->val_szmru; i++)
	{
//		if(dyeqp(val, ctx->val_mru[i]))
		if(dyListEqualP(val, ctx->val_mru[i]))
//		if(dyeqvp(val, ctx->val_mru[i]))
			return(i);
	}
	return(-1);
}

void BSXRP_AddValueMRU(BSXRP_StreamContext *ctx, dyt val)
{
	dyt t;
	int idx;
	
	if(dyconsp(val))
		val=dyListCopyR(val);
	
	if(ctx->val_szmru<256)
	{
		idx=ctx->val_szmru++;
		BSXRP_ShiftBackMRU(ctx, idx);
		ctx->val_mru[0]=val;
	}else
	{
		idx=ctx->val_szmru;
		t=BSXRP_ShiftBackMRU(ctx, idx);
		ctx->val_mru[0]=val;
		if(dyconsp(t))
			dyFreeListR(t);
	}
	
}

#if 0
void BSXRP_EncodeValue(BSXRP_StreamContext *ctx, dyt val)
{
	int i;

	if(!val)
		{ BSXRP_EncodeSymbol(ctx, 0, 0x13); return; }
	if(val==UNDEFINED)
		{ BSXRP_EncodeSymbol(ctx, 0, 0x14); return; }
	if(val==BGBDY_TRUE)
		{ BSXRP_EncodeSymbol(ctx, 0, 0x11); return; }
	if(val==BGBDY_FALSE)
		{ BSXRP_EncodeSymbol(ctx, 0, 0x12); return; }

	i=BSXRP_IndexValueMRU(ctx, val);
	if(i>=0)
	{
		BSXRP_EncodeValueIndex(ctx, i);
		BSXRP_ShiftIndexMTF(ctx, i);
		return;
	}

	if(dyintp(val))
	{
		BSXRP_EncodeSymbol(ctx, 0, 0x05);
		BSXRP_EncodeSInteger(ctx, dyintv(val));
		BSXRP_AddValueMRU(ctx, val);
		return;
	}

	if(dysymbolp(val))
	{
		BSXRP_EncodeSymbol(ctx, 0, 0x02);
//		BSXRP_EncodeSInteger(ctx, dyintv(val));
		BSXRP_AddValueMRU(ctx, val);
		return;
	}
}
#endif

void BSXRP_ValueEncodeSymbol(BSXRP_StreamContext *ctx, int sym)
{
	*ctx->val_ct++=sym|0x0000;
}

void BSXRP_ValueEncodeIndex(BSXRP_StreamContext *ctx, int idx)
{
	*ctx->val_ct++=(idx&0x0FFF)|0x4000;
	*ctx->val_ct++=((idx>>12)&0x0FFF)|0x3000;
}

void BSXRP_ValueEncodeInteger(BSXRP_StreamContext *ctx, s64 val)
{
	u64 v;
	
	v=(val>=0)?(((u64)val)<<1):((((u64)(-val))<<1)-1);

	*ctx->val_ct++=(v&0x0FFF)|0x5000;
//	*ctx->val_ct++=((v>>12)&0x0FFF)|0x3000;
//	*ctx->val_ct++=((v>>24)&0x0FFF)|0x3000;

	v>>=12;
	while(v)
	{
		*ctx->val_ct++=(v&0x0FFF)|0x3000;
		v>>=12;
	}
}

void BSXRP_ValueEncodeReal(BSXRP_StreamContext *ctx, double val)
{
	u64 ulv;
	s64 lm, le;
	
	ulv=*(u64 *)(&val);
	
	if(!ulv)
	{
		BSXRP_ValueEncodeInteger(ctx, 0);
		BSXRP_ValueEncodeInteger(ctx, 0);
		return;
	}
	
	lm=ulv&0x000FFFFFFFFFFFFFULL;		//mantissa
	lm|=0x0010000000000000ULL;			//hidden bit
	le=((ulv>>52)&0x7FF)-1023-52;		//base exponent
	
	//sign bit
	if(ulv&0x8000000000000000ULL)
		lm=-lm;
	
	//remove needless 0's
	while(!(lm&1))
		{ lm>>=1; le++; }

	BSXRP_ValueEncodeInteger(ctx, lm);
	BSXRP_ValueEncodeInteger(ctx, le);
}

void BSXRP_ValueEncodeReal32(BSXRP_StreamContext *ctx, float val)
{
	u32 uv;
	s64 lm, le;
	
	uv=*(u32 *)(&val);
	
	if(!uv)
	{
		BSXRP_ValueEncodeInteger(ctx, 0);
		BSXRP_ValueEncodeInteger(ctx, 0);
		return;
	}
	
	lm=uv&0x007FFFFF;		//mantissa
	lm|=0x00800000;			//hidden bit
	le=((uv>>23)&0xFF)-127-23;		//base exponent
	
	//sign bit
	if(uv&0x80000000)
		lm=-lm;
	
	//remove needless 0's
	while(!(lm&1))
		{ lm>>=1; le++; }

//	dyPrintf("BSXRP_ValueEncodeReal32: %f %d,%d\n",
//		val, (int)lm, (int)le);

	BSXRP_ValueEncodeInteger(ctx, lm);
	BSXRP_ValueEncodeInteger(ctx, le);
}

void BSXRP_ValueEncodeString(BSXRP_StreamContext *ctx, char *str)
{
	int i;
	i=BSXRP_LZCompressBuffer(ctx, str, ctx->val_ct,
		strlen(str), ctx->val_cte-ctx->val_ct);
	ctx->val_ct+=i;
}

void BSXRP_ValueEncodeByteData(BSXRP_StreamContext *ctx,
	byte *buf, int sz)
{
	int i;
	i=BSXRP_LZCompressBuffer(ctx, buf, ctx->val_ct,
		sz, ctx->val_cte-ctx->val_ct);
	ctx->val_ct+=i;
}

void BSXRP_EncodeValueList(BSXRP_StreamContext *ctx, dyt lst)
{
	dyt c;
	
	c=lst;
	while(dyconsp(c))
		c=dycdr(c);

	if(c)
	{
		BSXRP_ValueEncodeSymbol(ctx, 0x08);
		c=lst;
		while(dyconsp(c))
			{ BSXRP_EncodeValueBuffer(ctx, dycar(c)); c=dycdr(c); }
		BSXRP_EncodeValueBuffer(ctx, c);
		BSXRP_ValueEncodeSymbol(ctx, 0x00);
		return;
	}

	BSXRP_ValueEncodeSymbol(ctx, 0x01);
	c=lst;
	while(dyconsp(c))
		{ BSXRP_EncodeValueBuffer(ctx, dycar(c)); c=dycdr(c); }
	BSXRP_ValueEncodeSymbol(ctx, 0x00);
}

void BSXRP_EncodeValueArray(BSXRP_StreamContext *ctx, dyt val)
{
	dyt t;
	int i, n;
	
	n=dyarraysz(val);

	BSXRP_ValueEncodeSymbol(ctx, 0x09);
	for(i=0; i<n; i++)
	{
		t=dyarrayidx(val, i);
		BSXRP_EncodeValueBuffer(ctx, t);
	}
	BSXRP_ValueEncodeSymbol(ctx, 0x00);
}

void BSXRP_EncodeValueByteArray(BSXRP_StreamContext *ctx, dyt val)
{
	byte *buf;
	dyt t;
	int i, sz;
	
	sz=dycArraySz((dycArray)val);
	buf=dycArrayV((dycArray)val);

	BSXRP_ValueEncodeSymbol(ctx, 0x0A);
	BSXRP_ValueEncodeInteger(ctx, sz);
	BSXRP_ValueEncodeByteData(ctx, buf, sz);
//	BSXRP_ValueEncodeSymbol(ctx, 0x00);
}

void BSXRP_EncodeValueByteArrayNoMRU(BSXRP_StreamContext *ctx, dyt val)
{
	byte *buf;
	dyt t;
	int i, sz;
	
	sz=dycArraySz((dycArray)val);
	buf=dycArrayV((dycArray)val);

	BSXRP_ValueEncodeSymbol(ctx, 0x0B);
	BSXRP_ValueEncodeInteger(ctx, sz);
	BSXRP_ValueEncodeByteData(ctx, buf, sz);
//	BSXRP_ValueEncodeSymbol(ctx, 0x00);
}

void BSXRP_EncodeValueBuffer(BSXRP_StreamContext *ctx, dyt val)
{
	int i;

	if(!val)
		{ BSXRP_ValueEncodeSymbol(ctx, 0x10); return; }
	if(val==UNDEFINED)
		{ BSXRP_ValueEncodeSymbol(ctx, 0x14); return; }
	if(val==BGBDY_TRUE)
		{ BSXRP_ValueEncodeSymbol(ctx, 0x11); return; }
	if(val==BGBDY_FALSE)
		{ BSXRP_ValueEncodeSymbol(ctx, 0x12); return; }

	i=BSXRP_IndexValueMRU(ctx, val);
	if(i>=0)
//	if(0)
	{
		BSXRP_ValueEncodeIndex(ctx, i);
		BSXRP_ShiftIndexMTF(ctx, i);
		return;
	}

	if(dyconsp(val))
	{
		BSXRP_EncodeValueList(ctx, val);
		BSXRP_AddValueMRU(ctx, val);
		return;
	}

	if(dyarrayp(val))
	{
		BSXRP_EncodeValueArray(ctx, val);
		BSXRP_AddValueMRU(ctx, val);
		return;
	}

	if(dycArraySigP(val, "h"))
	{
		BSXRP_EncodeValueByteArray(ctx, val);
		BSXRP_AddValueMRU(ctx, val);
		return;
	}

	if(dysymbolp(val))
	{
		BSXRP_ValueEncodeSymbol(ctx, 0x02);
		BSXRP_ValueEncodeString(ctx, dysymbolv(val));
		BSXRP_AddValueMRU(ctx, val);
		return;
	}
	if(dystringp(val))
	{
		BSXRP_ValueEncodeSymbol(ctx, 0x03);
		BSXRP_ValueEncodeString(ctx, dysymbolv(val));
		BSXRP_AddValueMRU(ctx, val);
		return;
	}
	if(dykeywordp(val))
	{
		BSXRP_ValueEncodeSymbol(ctx, 0x04);
		BSXRP_ValueEncodeString(ctx, dysymbolv(val));
		BSXRP_AddValueMRU(ctx, val);
		return;
	}

	if(dyintp(val))
	{
		BSXRP_ValueEncodeSymbol(ctx, 0x05);
//		BSXRP_EncodeSInteger(ctx, dyintv(val));
		BSXRP_ValueEncodeInteger(ctx, dyintv(val));
		BSXRP_AddValueMRU(ctx, val);
		return;
	}

	if(dyflonump(val))
	{
		BSXRP_ValueEncodeSymbol(ctx, 0x06);
		BSXRP_ValueEncodeReal32(ctx, dyfloatv(val));
		BSXRP_AddValueMRU(ctx, val);
		return;
	}

	if(dyfloatp(val) || dydoublep(val))
	{
		BSXRP_ValueEncodeSymbol(ctx, 0x07);
		BSXRP_ValueEncodeReal(ctx, dydoublev(val));
		BSXRP_AddValueMRU(ctx, val);
		return;
	}
}

byte *BSXRP_EmitTag(byte *ct, int tag)
	{ *ct++=0x7F; *ct++=0xFF; *ct++=0xFE; *ct++=tag; return(ct); }

int BSXRP_MessageEncodeValueBuffer(
	BSXRP_StreamContext *ctx, dyt val, byte *obuf, int osz)
{
	u16 *vbuf;
	byte *ct, *cte;
	int n;
	
	vbuf=malloc((1<<16)*sizeof(u16));
	ctx->val_ct=vbuf;
	ctx->val_cts=vbuf;
	ctx->val_cte=vbuf+(1<<16);

	BSXRP_EncodeValueBuffer(ctx, val);
	n=ctx->val_ct-vbuf;
	
	BSXRP_StatLZBuffer(ctx, vbuf, n,
		ctx->huff_stat[0], ctx->huff_stat[1], ctx->huff_stat[2]);
	
	BSXRP_BuildLengths(ctx->huff_stat[0], 384, ctx->huff_len[0], 16);
	BSXRP_BuildLengths(ctx->huff_stat[1], 384, ctx->huff_len[1], 16);
	BSXRP_BuildLengths(ctx->huff_stat[2], 384, ctx->huff_len[2], 16);
	
	BSXRP_HuffmanLengthSanityAdjust(ctx, 0);
	BSXRP_HuffmanLengthSanityAdjust(ctx, 1);
	BSXRP_HuffmanLengthSanityAdjust(ctx, 2);
	
	ct=obuf; cte=obuf+osz;

#if 0
	ct=BSXRP_EmitTag(ct, 4);
	BSXRP_SetupBitstreamWrite(ctx, ct, cte);
	BSXRP_EncodeHuffTable(ctx, 0, ctx->huff_len[0], 384);
	ct=BSXRP_FinishBitstreamWrite(ctx);

	ct=BSXRP_EmitTag(ct, 5);
	BSXRP_SetupBitstreamWrite(ctx, ct, cte);
	BSXRP_EncodeHuffTable(ctx, 1, ctx->huff_len[1], 384);
	ct=BSXRP_FinishBitstreamWrite(ctx);

	ct=BSXRP_EmitTag(ct, 6);
	BSXRP_SetupBitstreamWrite(ctx, ct, cte);
	BSXRP_EncodeHuffTable(ctx, 2, ctx->huff_len[2], 384);
	ct=BSXRP_FinishBitstreamWrite(ctx);

	ct=BSXRP_EmitTag(ct, 1);
	BSXRP_SetupBitstreamWrite(ctx, ct, cte);
	BSXRP_EncodeLZBuffer(ctx, vbuf, n);
	ct=BSXRP_FinishBitstreamWrite(ctx);
	ct=BSXRP_EmitTag(ct, 2);
#endif	

	ct=BSXRP_EmitTag(ct, 1);
	BSXRP_SetupBitstreamWrite(ctx, ct, cte);

	if(BSXRP_CheckEncodeHuffTable(ctx, 0, ctx->huff_len[0], 384))
	{
		BSXRP_Write4Bits(ctx, 4);
		BSXRP_EncodeHuffTable(ctx, 0, ctx->huff_len[0], 384);
	}
	if(BSXRP_CheckEncodeHuffTable(ctx, 1, ctx->huff_len[1], 384))
	{
		BSXRP_Write4Bits(ctx, 5);
		BSXRP_EncodeHuffTable(ctx, 1, ctx->huff_len[1], 384);
	}
	if(BSXRP_CheckEncodeHuffTable(ctx, 2, ctx->huff_len[2], 384))
	{
		BSXRP_Write4Bits(ctx, 6);
		BSXRP_EncodeHuffTable(ctx, 2, ctx->huff_len[2], 384);
	}

	BSXRP_Write4Bits(ctx, 1);
	BSXRP_EncodeLZBuffer(ctx, vbuf, n);
	BSXRP_Write4Bits(ctx, 0);
	ct=BSXRP_FinishBitstreamWrite(ctx);
	ct=BSXRP_EmitTag(ct, 2);

	free(vbuf);

	return(ct-obuf);
}


double BSXRP_DecodeMessageReal(BSXRP_StreamContext *ctx)
{
	double v;
	s64 lm, le;
	
	lm=BSXRP_DecodeSInteger(ctx);
	le=BSXRP_DecodeSInteger(ctx);
	v=((double)lm)*pow(2.0, ((int)le));
	
//	dyPrintf("BSXRP_DecodeMessageReal: %d,%d %f\n",
//		(int)lm, (int)le, v);
	
	return(v);
}

char *BSXRP_DecodeMessageStringData(BSXRP_StreamContext *ctx)
{
	char tbuf[4096];

	ctx->obuf=tbuf;
	ctx->ct=tbuf;
	ctx->cte=tbuf+4092;
	BSXRP_DecodeBlockData(ctx);
	*ctx->ct++=0;
	
	return(gcrstrdup(tbuf));
}

dyt BSXRP_DecodeMessageByteArray(BSXRP_StreamContext *ctx)
{
//	char tbuf[4096];
	dycArray arr;
	byte *buf;
	int sz;

	sz=BSXRP_DecodeSInteger(ctx);

	arr=dycNewArray("h", sz+4);
	buf=dycArrayV(arr);

	ctx->obuf=buf;
	ctx->ct=buf;
	ctx->cte=buf+(sz+4);
	BSXRP_DecodeBlockData(ctx);
	*ctx->ct++=0;
	
	return((dyt)arr);
}

dyt BSXRP_DecodeMessageList(BSXRP_StreamContext *ctx)
{
	dyt t, l;
	
	l=NULL;
	while(1)
	{
		t=BSXRP_DecodeMessageValue(ctx);
		if(t==BGBDY_AUXNULL)break;
		l=dycons(t, l);
	}
	
	l=dynreverse(l);
	return(l);
}

dyt BSXRP_DecodeMessageDotList(BSXRP_StreamContext *ctx)
{
	dyt t, v, l;
	
	l=NULL;
	while(1)
	{
		t=BSXRP_DecodeMessageValue(ctx);
		if(t==BGBDY_AUXNULL)break;
		l=dycons(t, l);
	}
	
	v=dycar(l);
	t=l;
	l=dynreverse(dycdr(l));
	l=dynappend(l, v);
	dyFreeCons(t);

	return(l);
}

dyt BSXRP_DecodeMessageArray(BSXRP_StreamContext *ctx)
{
	dyt t, l;
	
	l=BSXRP_DecodeMessageList(ctx);
	t=dyListToArray(l);
	dyFreeList(l);
	return(t);
}

dyt BSXRP_DecodeMessageValue(BSXRP_StreamContext *ctx)
{
	s64 li;
	double f;
	dyt v;
	int op;
	
	op=BSXRP_DecodeSymbol(ctx, 0);
	
	if((op>=128) && (op<192))
	{
		li=BSXRP_DecodeValueIndex(ctx, op);
		v=BSXRP_ShiftIndexMTF(ctx, li);
		if(dyconsp(v))
			v=dyListCopyR(v);
		return(v);
	}
	
	switch(op)
	{
	case 0x00:
		v=BGBDY_AUXNULL;
		break;
	case 0x01:
		v=BSXRP_DecodeMessageList(ctx);
		BSXRP_AddValueMRU(ctx, v);
		break;
	case 0x02:
		v=dysymbol(BSXRP_DecodeMessageStringData(ctx));
		BSXRP_AddValueMRU(ctx, v);
		break;
	case 0x03:
		v=dystring(BSXRP_DecodeMessageStringData(ctx));
		BSXRP_AddValueMRU(ctx, v);
		break;
	case 0x04:
		v=dykeyword(BSXRP_DecodeMessageStringData(ctx));
		BSXRP_AddValueMRU(ctx, v);
		break;
	case 0x05:
		li=BSXRP_DecodeSInteger(ctx);
		v=dyint(li);
		BSXRP_AddValueMRU(ctx, v);
		break;
	case 0x06:
		f=BSXRP_DecodeMessageReal(ctx);
		v=dyflonum(f);
		BSXRP_AddValueMRU(ctx, v);
		break;
	case 0x07:
		f=BSXRP_DecodeMessageReal(ctx);
		v=dydouble(f);
		BSXRP_AddValueMRU(ctx, v);
		break;
	case 0x08:
		v=BSXRP_DecodeMessageDotList(ctx);
		BSXRP_AddValueMRU(ctx, v);
		break;
	case 0x09:
		v=BSXRP_DecodeMessageArray(ctx);
		BSXRP_AddValueMRU(ctx, v);
		break;
	case 0x0A:
		v=BSXRP_DecodeMessageByteArray(ctx);
		BSXRP_AddValueMRU(ctx, v);
		break;
	case 0x0B:
		v=BSXRP_DecodeMessageByteArray(ctx);
		break;

	case 0x10: v=BGBDY_NULL; break;
	case 0x11: v=BGBDY_TRUE; break;
	case 0x12: v=BGBDY_FALSE; break;
	case 0x13: v=BGBDY_NULL; break;
	case 0x14: v=UNDEFINED; break;

	case 0x15: li=BSXRP_DecodeSInteger(ctx); v=NULL; break;
	case 0x16: li=BSXRP_DecodeSInteger(ctx); v=NULL; break;
	
	default:
		dyPrintf("BSXRP_DecodeMessageValue: Bad Value Tag %02X", op);
		v=UNDEFINED;
		break;
	}
	
	return(v);
}
