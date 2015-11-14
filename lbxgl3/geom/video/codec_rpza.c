#include <lbxgl2.h>
// #include <xvid.h>
#include <bgbbtj.h>

typedef struct {
// HIC hicd;
PDGL_BMPInfoHeader *ihead;
PDGL_BMPInfoHeader *ohead;
void *buffer;
void *vidCtx;
}vfw_ctxinfo;

int LBXGL_RPZA_DecompressFrame(LBXGL_AVI_Context *ctx,
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
			ctx->mjpg_cty=PDJPG_CTY_RGB;
	}

	if(!dst)return(0);

#if 0
	if(ctx->mjpg_cty!=PDJPG_CTY_RGB)
	{
		xs=ctx->bmihead->biWidth;
		ys=ctx->bmihead->biHeight;
		info=ctx->codec_ctx->data;

		if(!info->vidCtx)
			info->vidCtx=BTIC1_AllocContext();

		switch(ctx->mjpg_cty)
		{
		case PDJPG_CTY_RGBA_LUMA:
			if(!ctx->fdbuf_luma)
				ctx->fdbuf_luma=malloc(xs*ys*4);
			break;
		case PDJPG_CTY_RGBA_NORM:
			if(!ctx->fdbuf_norm)
				ctx->fdbuf_norm=malloc(xs*ys*4);
			break;
		case PDJPG_CTY_RGBA_NVSP:
			if(!ctx->fdbuf_norm)
				ctx->fdbuf_norm=malloc(xs*ys*4);
			if(!ctx->fdbuf_spec)
				ctx->fdbuf_spec=malloc(xs*ys*4);
			break;
		case PDJPG_CTY_RGBA_NVSPLU:
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
		
//		BTIC1_DecodeComponentCtx(
//			info->vidCtx, src, ssz,
//			ctx->fdbuf, ctx->fdbuf_norm,
//			ctx->fdbuf_spec, ctx->fdbuf_luma, 
//			&xs, &ys, ctx->mjpg_clrs);
		return(0);
	}
#endif

	xs=ctx->bmihead->biWidth;
	ys=ctx->bmihead->biHeight;
	info=ctx->codec_ctx->data;

	if(!info->vidCtx)
		info->vidCtx=BGBBTJ_RPZA_AllocContext();

//	BGBBTJ_BufPNG_DecodeOBuf(
//		src, ssz, dst, &xs, &ys);
	BGBBTJ_RPZA_DecodeFrameImage(info->vidCtx,
		src, dst, ssz, dsz, xs, ys, ctx->mjpg_clrs);

	return(0);
}

int LBXGL_BT1C_DecompressFrame(LBXGL_AVI_Context *ctx,
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
			ctx->mjpg_cty=PDJPG_CTY_RGB;
	}

	if(!dst)return(0);

	xs=ctx->bmihead->biWidth;
	ys=ctx->bmihead->biHeight;
	info=ctx->codec_ctx->data;

	if(!info->vidCtx)
		info->vidCtx=BGBBTJ_BT1C_AllocContext();

	BGBBTJ_BT1C_DecodeFrameImage(info->vidCtx,
		src, dst, ssz, dsz, xs, ys, ctx->mjpg_clrs);

	return(0);
}

int lbxgl_rpza_decompress_frame(LBXGL_VidCodecCTX *ctx,
	void *src, void *dst, int ssz, int dsz)
{
	vfw_ctxinfo *info;
	int err, i, j, k, ib, ob, xs, ys;
	byte *sbuf, *tbuf;

	info=ctx->data;

	if(!dst)return(0);

	if(!info->vidCtx)
		info->vidCtx=BGBBTJ_RPZA_AllocContext();

	i=1<<20;	//dummy...

	xs=info->ihead->biWidth;
	ys=info->ihead->biHeight;

	BGBBTJ_RPZA_DecodeFrameImage(info->vidCtx,
		src, dst, ssz, dsz, xs, ys, BGBBTJ_JPG_RGBA);

	return(0);
}

int lbxgl_bt1c_decompress_frame(LBXGL_VidCodecCTX *ctx,
	void *src, void *dst, int ssz, int dsz)
{
	vfw_ctxinfo *info;
	int err, i, j, k, ib, ob, xs, ys;
	byte *sbuf, *tbuf;

	info=ctx->data;

	if(!dst)return(0);

	if(!info->vidCtx)
		info->vidCtx=BGBBTJ_BT1C_AllocContext();

	i=1<<20;	//dummy...

	xs=info->ihead->biWidth;
	ys=info->ihead->biHeight;

	BGBBTJ_BT1C_DecodeFrameImage(info->vidCtx,
		src, dst, ssz, dsz, xs, ys, BGBBTJ_JPG_RGBA);

	return(0);
}

static LBXGL_VidCodecCTX *lbxgl_rpza_begin_decompress(int fcc,
	PDGL_BMPInfoHeader *in, PDGL_BMPInfoHeader *out)
{
	LBXGL_VidCodecCTX *ctx;
	vfw_ctxinfo *info;
	int err;

//	if((fcc!=RIFF_TAG_rpza) && (fcc!=RIFF_TAG_bt1c))
//		return(NULL);

	if((fcc!=RIFF_TAG_rpza) && (fcc!=RIFF_TAG_azpr) &&
			(fcc!=RIFF_TAG_bt1c) && (fcc!=RIFF_TAG_c1tb) &&
			(fcc!=RIFF_TAG_BTIC))
		return(NULL);

	if(fcc==RIFF_TAG_BTIC)
	{
		if(in->biCompression!=RIFF_TAG_bt1c)
			return(NULL);
	}

	ctx=LBXGL_VidCodecCTX_New();
	info=gcalloc(sizeof(vfw_ctxinfo));
	ctx->data=info;

	info->ihead=gcalloc(sizeof(PDGL_BMPInfoHeader));
	memset(info->ihead, 0, sizeof(PDGL_BMPInfoHeader));
	info->ihead->biSize		= sizeof(PDGL_BMPInfoHeader);
	info->ihead->biWidth		= in->biWidth;
	info->ihead->biHeight		= in->biHeight;
	info->ihead->biPlanes		= in->biPlanes;
	info->ihead->biBitCount		= in->biBitCount;
	info->ihead->biCompression	= in->biCompression;
	info->ihead->biSizeImage	=
		in->biWidth*in->biHeight*in->biBitCount/8;

	info->ohead=gcalloc(sizeof(PDGL_BMPInfoHeader));
	memset(info->ohead, 0, sizeof(PDGL_BMPInfoHeader));
	info->ohead->biSize		= sizeof(PDGL_BMPInfoHeader);
	info->ohead->biWidth		= out->biWidth;
//	info->ohead->biHeight		= -out->biHeight;
	info->ohead->biHeight		= out->biHeight;
	info->ohead->biPlanes		= out->biPlanes;
	info->ohead->biBitCount		= out->biBitCount;
	info->ohead->biCompression	= out->biCompression;
	info->ihead->biSizeImage	=
		out->biWidth*out->biHeight*out->biBitCount/8;

//	info->buffer=malloc(out->width*out->height*out->bpp/8);

	ctx->decompress_frame=&lbxgl_rpza_decompress_frame;

	return(ctx);
}

static LBXGL_VidCodecCTX *lbxgl_bt1c_begin_decompress(int fcc,
	PDGL_BMPInfoHeader *in, PDGL_BMPInfoHeader *out)
{
	LBXGL_VidCodecCTX *ctx;
	vfw_ctxinfo *info;
	int err;

//	if((fcc!=RIFF_TAG_rpza) && (fcc!=RIFF_TAG_bt1c))
//		return(NULL);

	if((fcc!=RIFF_TAG_rpza) && (fcc!=RIFF_TAG_azpr) &&
			(fcc!=RIFF_TAG_bt1c) && (fcc!=RIFF_TAG_c1tb) &&
			(fcc!=RIFF_TAG_BTIC))
		return(NULL);

	if(fcc==RIFF_TAG_BTIC)
	{
		if(in->biCompression!=RIFF_TAG_bt1c)
			return(NULL);
	}

	ctx=LBXGL_VidCodecCTX_New();
	info=gcalloc(sizeof(vfw_ctxinfo));
	ctx->data=info;

	info->ihead=gcalloc(sizeof(PDGL_BMPInfoHeader));
	memset(info->ihead, 0, sizeof(PDGL_BMPInfoHeader));
	info->ihead->biSize		= sizeof(PDGL_BMPInfoHeader);
	info->ihead->biWidth		= in->biWidth;
	info->ihead->biHeight		= in->biHeight;
	info->ihead->biPlanes		= in->biPlanes;
	info->ihead->biBitCount		= in->biBitCount;
	info->ihead->biCompression	= in->biCompression;
	info->ihead->biSizeImage	=
		in->biWidth*in->biHeight*in->biBitCount/8;

	info->ohead=gcalloc(sizeof(PDGL_BMPInfoHeader));
	memset(info->ohead, 0, sizeof(PDGL_BMPInfoHeader));
	info->ohead->biSize		= sizeof(PDGL_BMPInfoHeader);
	info->ohead->biWidth		= out->biWidth;
//	info->ohead->biHeight		= -out->biHeight;
	info->ohead->biHeight		= out->biHeight;
	info->ohead->biPlanes		= out->biPlanes;
	info->ohead->biBitCount		= out->biBitCount;
	info->ohead->biCompression	= out->biCompression;
	info->ihead->biSizeImage	=
		out->biWidth*out->biHeight*out->biBitCount/8;

//	info->buffer=malloc(out->width*out->height*out->bpp/8);

	ctx->decompress_frame=&lbxgl_bt1c_decompress_frame;

	return(ctx);
}

int LBXGL_CodecRPZA_Init()
{
	LBXGL_VidCodec *codec;
	int i, j;
	short *sw;
	char buf[256];
	char *t;

	printf("RPZA: Init\n");

	codec=LBXGL_VidCodec_New();
	codec->begin_decompress=&lbxgl_rpza_begin_decompress;

	codec=LBXGL_VidCodec_New();
	codec->begin_decompress=&lbxgl_bt1c_begin_decompress;
	return(0);
}
