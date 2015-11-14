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

int BGBBTJ_MJPEG_DecompressFrameClrs(BGBBTJ_AVI_Context *ctx,
	void *src, void *dst, int ssz, int dsz, int clrs)
{
	ctx->mjpg_clrs=clrs;
	return(BGBBTJ_MJPEG_DecompressFrame(ctx,
		src, dst, ssz, dsz));
}

int BGBBTJ_MJPEG_DecompressFrame(BGBBTJ_AVI_Context *ctx,
	void *src, void *dst, int ssz, int dsz)
{
	vfw_ctxinfo *info;
	byte *sbuf, *tbuf;
	int xs, ys;

	if(!ctx->mjpg_cty)
	{
//		ctx->mjpg_cty=BGBBTJ_JPG_DecodeComponentType(src, ssz);
		ctx->mjpg_cty=BGBBTJ_JPG_DecodeComponentType(src, ssz);
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
//			info->vidCtx=BGBBTJ_JPG_AllocContext();
			info->vidCtx=BGBBTJ_JPG_AllocContext();

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
		
		BGBBTJ_JPG_DecodeComponentCtxClrs(
			info->vidCtx, src, ssz, ctx->mjpg_clrs,
//		BGBBTJ_JPG_DecodeComponentCtx(info->vidCtx, src, ssz,
//		BGBBTJ_JPG_DecodeComponentCtx(info->vidCtx, src, ssz,
			ctx->fdbuf, ctx->fdbuf_norm,
			ctx->fdbuf_spec, ctx->fdbuf_luma, 
			&xs, &ys);
		return(0);
	}

	xs=ctx->bmihead->biWidth;
	ys=ctx->bmihead->biHeight;
	info=ctx->codec_ctx->data;

	if(!info->vidCtx)
//		info->vidCtx=BGBBTJ_JPG_AllocContext();
		info->vidCtx=BGBBTJ_JPG_AllocContext();

	tbuf=BGBBTJ_JPG_DecodeCtxClrs(
		info->vidCtx, src, ssz, ctx->mjpg_clrs,
		&xs, &ys);
	if(tbuf)
		memcpy(dst, tbuf, xs*ys*4);

//	mjpg_decompress_frame(ctx->codec_ctx, src, dst, ssz, dsz);
	return(0);
}

int mjpg_decompress_frame(BGBBTJ_VidCodecCTX *ctx,
	void *src, void *dst, int ssz, int dsz)
{
	vfw_ctxinfo *info;
	int err, i, j, k, ib, ob, xs, ys;
	byte *sbuf, *tbuf;

	info=ctx->data;

	if(!dst)return(0);

	if(!info->vidCtx)
//		info->vidCtx=BGBBTJ_JPG_AllocContext();
		info->vidCtx=BGBBTJ_JPG_AllocContext();

	i=1<<20;	//dummy...
//	tbuf=BGBBTJ_JPG_Decode(src, ssz, &xs, &ys);
//	tbuf=BGBBTJ_JPG_DecodeCtx(info->vidCtx, src, ssz, &xs, &ys);
	tbuf=BGBBTJ_JPG_DecodeCtx(info->vidCtx, src, ssz, &xs, &ys);
	if(!tbuf)
	{
//		printf("MJPG: bad frame\n");
//		sbuf=src;
//		for(i=0; i<32; i++)printf("%02X ", sbuf[i]);
//		printf("\n");
		return(-1);
	}
	
//	printf("%d %d\n", xs, ys);
	
	memcpy(dst, tbuf, xs*ys*4);
//	BGBBTJ_JPG_Free(tbuf);

	return(0);
}

static BGBBTJ_VidCodecCTX *mjpg_begin_decompress(int fcc,
	BGBBTJ_BMPInfoHeader *in, BGBBTJ_BMPInfoHeader *out)
{
	BGBBTJ_VidCodecCTX *ctx;
	vfw_ctxinfo *info;
	int err;

	if((fcc!=RIFF_TAG_MJPG) && (fcc!=RIFF_TAG_MBTJ) &&
			(fcc!=RIFF_TAG_GPJM))
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

	ctx->decompress_frame=&mjpg_decompress_frame;

	return(ctx);
}

int BGBBTJ_CodecMJPG_Init()
{
	BGBBTJ_VidCodec *codec;
	int i, j;
	short *sw;
	char buf[256];
	char *t;

	printf("MJPG: Init\n");

	codec=BGBBTJ_VidCodec_New();
	codec->begin_decompress=&mjpg_begin_decompress;
	return(0);
}
