#include <bgbnet.h>

BGBNET_API BSXRP_MuxBuffer *BSXRP_LookupMuxBufferRX(
	BSXRP_Context *ctx, int muxid)
{
	BSXRP_MuxBuffer *cur;

	cur=ctx->mux_rx;
	while(cur)
	{
		if(cur->muxid==muxid)
			return(cur);
		cur=cur->next;
	}
	return(NULL);
}

BGBNET_API BSXRP_MuxBuffer *BSXRP_LookupMuxBufferTX(
	BSXRP_Context *ctx, int muxid)
{
	BSXRP_MuxBuffer *cur;

	cur=ctx->mux_tx;
	while(cur)
	{
		if(cur->muxid==muxid)
			return(cur);
		cur=cur->next;
	}
	return(NULL);
}

BGBNET_API BSXRP_MuxBuffer *BSXRP_GetMuxBufferRX(
	BSXRP_Context *ctx, int muxid)
{
	BSXRP_MuxBuffer *tmp;

	if(muxid<=0)muxid=ctx->mux_rover++;

	tmp=BSXRP_LookupMuxBufferRX(ctx, muxid);
	if(tmp)return(tmp);
	
	tmp=gctalloc("bsxrp_muxbuffer_t", sizeof(BSXRP_MuxBuffer));
	tmp->muxid=muxid;
	tmp->next=ctx->mux_rx;
	ctx->mux_rx=tmp;
	return(tmp);
}

BGBNET_API BSXRP_MuxBuffer *BSXRP_GetMuxBufferTX(
	BSXRP_Context *ctx, int muxid)
{
	BSXRP_MuxBuffer *tmp;

	if(muxid<=0)muxid=ctx->mux_rover++;

	tmp=BSXRP_LookupMuxBufferTX(ctx, muxid);
	if(tmp)return(tmp);
	
	tmp=gctalloc("bsxrp_muxbuffer_t", sizeof(BSXRP_MuxBuffer));
	tmp->muxid=muxid;
	tmp->next=ctx->mux_tx;
	ctx->mux_tx=tmp;
	return(tmp);
}

void BSXRP_DecodeMultiplexData(BSXRP_Context *ctx,
	BSXRP_StreamContext *sctx)
{
	BSXRP_MuxBuffer *mux;
	int tag, muxid, offs, size;
	int i;

	tag=BSXRP_DecodeSInteger(sctx);
	muxid=BSXRP_DecodeSInteger(sctx);
	offs=BSXRP_DecodeSInteger(sctx);
	size=BSXRP_DecodeSInteger(sctx);

	if(tag==1)
	{
		mux=BSXRP_LookupMuxBufferRX(ctx, muxid);

		if(!mux->buffer || (offs+size)>mux->sz_buffer)
		{
			i=offs+size;
			mux->buffer=gcrealloc(mux->buffer, i);
			mux->sz_buffer=i;
		}

		sctx->obuf=mux->buffer+offs;
		sctx->ct=sctx->obuf;
		sctx->cte=sctx->obuf+size;
		BSXRP_DecodeBlockData(sctx);

		mux->offset=offs+size;
		
		BSXRP_SendMultiplexMessage(ctx, 2, muxid, offs, NULL, size);
		
		return;
	}

	if(tag==2)
	{
		mux=BSXRP_LookupMuxBufferTX(ctx, muxid);

		mux->ack_offset=offs+size;
	}

}

int BSXRP_BeginMessageEncodeBuffer(BSXRP_StreamContext *ctx)
{
	u16 *vbuf;
	byte *ct, *cte;
	int n;
	
	vbuf=malloc((1<<16)*sizeof(u16));
	ctx->val_ct=vbuf;
	ctx->val_cts=vbuf;
	ctx->val_cte=vbuf+(1<<16);
}

int BSXRP_EndMessageEncodeBuffer(BSXRP_StreamContext *ctx,
	int minitag, byte *obuf, int osz)
{
	u16 *vbuf;
	byte *ct, *cte;
	int n;
	
//	BSXRP_EncodeValueBuffer(ctx, val);

	vbuf=ctx->val_cts;

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

	BSXRP_Write4Bits(ctx, minitag);
	BSXRP_EncodeLZBuffer(ctx, vbuf, n);
	BSXRP_Write4Bits(ctx, 0);
	ct=BSXRP_FinishBitstreamWrite(ctx);
	ct=BSXRP_EmitTag(ct, 2);

	free(vbuf);

	return(ct-obuf);
}

int BSXRP_MultiplexEncodeChunkBuffer(
	BSXRP_StreamContext *ctx,
	int tag, int muxid, int offset, byte *ibuf, int isz,
	byte *obuf, int osz)
{
	int i;
	
	BSXRP_BeginMessageEncodeBuffer(ctx);

	BSXRP_ValueEncodeInteger(ctx, tag);
	BSXRP_ValueEncodeInteger(ctx, muxid);
	BSXRP_ValueEncodeInteger(ctx, offset);
	BSXRP_ValueEncodeInteger(ctx, isz);

	if(ibuf)
	{
		BSXRP_ValueEncodeByteData(ctx, ibuf, isz);
	}

	i=BSXRP_EndMessageEncodeBuffer(ctx, 3, obuf, osz);
	return(i);
}

BGBNET_API int BSXRP_SendMultiplexMessage(BSXRP_Context *ctx,
	int tag, int muxid, int offset, byte *ibuf, int isz)
{
	byte buf[65536];
//	byte *buf;
	int i, j, k;

	if(!ctx->tx)
	{
		ctx->tx=BSXRP_CreateStreamContext();
	}
	
//	buf=malloc(65536);
	i=BSXRP_MultiplexEncodeChunkBuffer(ctx->tx,
		tag, muxid, offset, ibuf, isz,
		buf, 65536);

//	dyPrintf("BSXRP_SendStreamMessage: %d -> %d\n", i, ct-buf);
	k=vfwrite(buf, 1, i, ctx->con->sock);
//	dyPrintf("BSXRP_SendStreamMessage: sent=%d\n", k);
//	free(buf);

	return(k);
}

void BSXRP_MultiplexThink(BSXRP_Context *ctx)
{
	BSXRP_MuxBuffer *cur;
	int i, j, k, l;

	cur=ctx->mux_tx; l=0;
	while(cur)
	{
		i=cur->offset-cur->ack_offset;
		l+=i;
//		if(cur->muxid==muxid)
//			return(cur);
		cur=cur->next;
	}
	
	ctx->mux_tx_pending=l;

	while(l<16384)
	{
		cur=ctx->mux_tx;
		while(cur && (l<16384))
		{
			i=cur->sz_buffer-cur->offset;
			if(i>1024)i=1024;
			BSXRP_SendMultiplexMessage(ctx, 1, cur->muxid,
				cur->offset, cur->buffer+cur->offset, i);
			cur->offset+=i;
			l+=i;
//			i=cur->offset-cur->ack_offset;
//			l+=i;
//			if(cur->muxid==muxid)
//				return(cur);
			cur=cur->next;
		}
	}

	ctx->mux_tx_pending=l;
}
