// #include <lbxgl2.h>
// #include <xvid.h>
#include <bgbbtj.h>

typedef struct {
// HIC hicd;
BGBBTJ_BMPInfoHeader *ihead;
BGBBTJ_BMPInfoHeader *ohead;
void *buffer;
void *vidCtx;
}vfw_ctxinfo;

int BGBBTJ_MBTIC_DecompressFrame(BGBBTJ_AVI_Context *ctx,
	void *src, void *dst, int ssz, int dsz)
{
	vfw_ctxinfo *info;
	byte *sbuf, *tbuf;
	int xs, ys;

	if(!ctx->mjpg_cty)
	{
//		ctx->mjpg_cty=PDJPG_DecodeComponentType(src, ssz);
//		ctx->mjpg_cty=BGBBTJ_JPG_DecodeComponentType(src, ssz);
		if(!ctx->mjpg_cty)
			ctx->mjpg_cty=BGBBTJ_JPG_CTY_RGB;
	}

	if(!dst)return(0);

	if(ctx->mjpg_cty!=BGBBTJ_JPG_CTY_RGB)
	{
		xs=ctx->bmihead->biWidth;
		ys=ctx->bmihead->biHeight;
		info=ctx->codec_ctx->data;

		if(!info->vidCtx)
			info->vidCtx=BTIC1_AllocContext();

		switch(ctx->mjpg_cty)
		{
		case BGBBTJ_JPG_CTY_RGBA_LUMA:
			if(!ctx->fdbuf_luma)
				ctx->fdbuf_luma=malloc(xs*ys*4);
			break;
		case BGBBTJ_JPG_CTY_RGBA_NORM:
			if(!ctx->fdbuf_norm)
				ctx->fdbuf_norm=malloc(xs*ys*4);
			break;
		case BGBBTJ_JPG_CTY_RGBA_NVSP:
			if(!ctx->fdbuf_norm)
				ctx->fdbuf_norm=malloc(xs*ys*4);
			if(!ctx->fdbuf_spec)
				ctx->fdbuf_spec=malloc(xs*ys*4);
			break;
		case BGBBTJ_JPG_CTY_RGBA_NVSPLU:
			if(!ctx->fdbuf_norm)
				ctx->fdbuf_norm=malloc(xs*ys*4);
			if(!ctx->fdbuf_spec)
				ctx->fdbuf_spec=malloc(xs*ys*4);
			if(!ctx->fdbuf_luma)
				ctx->fdbuf_luma=malloc(xs*ys*4);
			break;
		default:
			break;
		}
		
		BTIC1_DecodeComponentCtx(
			info->vidCtx, src, ssz,
			ctx->fdbuf, ctx->fdbuf_norm,
			ctx->fdbuf_spec, ctx->fdbuf_luma, 
			&xs, &ys, ctx->mjpg_clrs);
		return(0);
	}

	xs=ctx->bmihead->biWidth;
	ys=ctx->bmihead->biHeight;
	info=ctx->codec_ctx->data;

	if(!info->vidCtx)
		info->vidCtx=BTIC1_AllocContext();

	BTIC1_DecodeComponentCtx(
		info->vidCtx, src, ssz,
		dst, NULL, NULL, NULL,
		&xs, &ys, ctx->mjpg_clrs);

//	tbuf=BGBBTJ_JPG_DecodeCtxClrs(
//		info->vidCtx, src, ssz, ctx->mjpg_clrs,
//		&xs, &ys);
//	if(tbuf)
//		memcpy(dst, tbuf, xs*ys*4);

//	mjpg_decompress_frame(ctx->codec_ctx, src, dst, ssz, dsz);
	return(0);
}

int mbtc_decompress_frame(BGBBTJ_VidCodecCTX *ctx,
	void *src, void *dst, int ssz, int dsz)
{
	vfw_ctxinfo *info;
	int err, i, j, k, ib, ob, xs, ys;
	byte *sbuf, *tbuf;

	info=ctx->data;

	if(!dst)return(0);

	if(!info->vidCtx)
//		info->vidCtx=PDJPG_AllocContext();
		info->vidCtx=BTIC1_AllocContext();

	i=1<<20;	//dummy...

	BTIC1_DecodeComponentCtx(
		info->vidCtx, src, ssz,
		dst, NULL, NULL, NULL,
		&xs, &ys, 0);

	return(0);
}

BGBBTJ_VidCodecCTX *mbtc_begin_decompress(int fcc,
	BGBBTJ_BMPInfoHeader *in, BGBBTJ_BMPInfoHeader *out)
{
	BGBBTJ_VidCodecCTX *ctx;
	vfw_ctxinfo *info;
	int err;

	if(fcc!=RIFF_TAG_MBTC)
		return(NULL);

	ctx=BGBBTJ_VidCodecCTX_New();
	info=gcalloc(sizeof(vfw_ctxinfo));
	ctx->data=info;

	info->ihead=gcalloc(sizeof(BGBBTJ_BMPInfoHeader));
	memset(info->ihead, 0, sizeof(BGBBTJ_BMPInfoHeader));
	info->ihead->biSize		= sizeof(BGBBTJ_BMPInfoHeader);
	info->ihead->biWidth		= in->biWidth;
	info->ihead->biHeight		= in->biHeight;
	info->ihead->biPlanes		= in->biPlanes;
	info->ihead->biBitCount		= in->biBitCount;
	info->ihead->biCompression	= in->biCompression;
	info->ihead->biSizeImage	=
		in->biWidth*in->biHeight*in->biBitCount/8;

	info->ohead=gcalloc(sizeof(BGBBTJ_BMPInfoHeader));
	memset(info->ohead, 0, sizeof(BGBBTJ_BMPInfoHeader));
	info->ohead->biSize		= sizeof(BGBBTJ_BMPInfoHeader);
	info->ohead->biWidth		= out->biWidth;
//	info->ohead->biHeight		= -out->biHeight;
	info->ohead->biHeight		= out->biHeight;
	info->ohead->biPlanes		= out->biPlanes;
	info->ohead->biBitCount		= out->biBitCount;
	info->ohead->biCompression	= out->biCompression;
	info->ihead->biSizeImage	=
		out->biWidth*out->biHeight*out->biBitCount/8;

//	info->buffer=malloc(out->width*out->height*out->bpp/8);

	ctx->decompress_frame=&mbtc_decompress_frame;

	return(ctx);
}

int BGBBTJ_CodecMBTC_Init()
{
	BGBBTJ_VidCodec *codec;
	int i, j;
	short *sw;
	char buf[256];
	char *t;

	printf("MBTC: Init\n");

	codec=BGBBTJ_VidCodec_New();
	codec->begin_decompress=&mbtc_begin_decompress;
	return(0);
}
