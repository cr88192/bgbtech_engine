#include <bgbbtj.h>

#ifndef RIFF_MAKETAG
#define RIFF_MAKETAG(a, b, c, d)	((a)+((b)<<8)+((c)<<16)+((d)<<24))
#endif

BGBBTJ_VidCodec *lbxgl_codecs=NULL;

BGBBTJ_VidCodec *BGBBTJ_VidCodec_New()
{
	BGBBTJ_VidCodec *tmp;

	tmp=gctalloc("lbxgl_vidcodec_t", sizeof(BGBBTJ_VidCodec));
	tmp->next=lbxgl_codecs;
	lbxgl_codecs=tmp;

	return(tmp);
}

BGBBTJ_VidCodecCTX *BGBBTJ_VidCodecCTX_New()
{
	BGBBTJ_VidCodecCTX *tmp;

	tmp=gctalloc("lbxgl_vidcodecctx_t", sizeof(BGBBTJ_VidCodecCTX));
	return(tmp);
}

void BGBBTJ_VidCodecCTX_Destroy(BGBBTJ_VidCodecCTX *ctx)
{
}

int BGBBTJ_Codec_DecompressQuery(int fcc,
	BGBBTJ_BMPInfoHeader *in, BGBBTJ_BMPInfoHeader *out)
{
	BGBBTJ_VidCodec *cur;
	int i;

	BGBBTJ_Codec_Init();

	cur=lbxgl_codecs;
	while(cur)
	{
		if(cur->decompress_query)
		{
			i=cur->decompress_query(fcc, in, out);
			if(i>0)return(i);
		}
		cur=cur->next;
	}
	return(0);
}

int BGBBTJ_Codec_CompressQuery(int fcc,
	BGBBTJ_BMPInfoHeader *in, BGBBTJ_BMPInfoHeader *out)
{
	BGBBTJ_VidCodec *cur;
	int i;

	BGBBTJ_Codec_Init();

	cur=lbxgl_codecs;
	while(cur)
	{
		if(cur->compress_query)
		{
			i=cur->compress_query(fcc, in, out);
			if(i>0)return(i);
		}
		cur=cur->next;
	}
	return(0);
}

BGBBTJ_VidCodecCTX *BGBBTJ_Codec_BeginDecompress(int fcc,
	BGBBTJ_BMPInfoHeader *in, BGBBTJ_BMPInfoHeader *out)
{
	BGBBTJ_VidCodec *cur;
	BGBBTJ_VidCodecCTX *tmp;
	int *cct;

	BGBBTJ_Codec_Init();

	cur=lbxgl_codecs;
	while(cur)
	{
		if(cur->fcc)
		{
			cct=cur->fcc;
			while(*cct)
			{
				if(fcc==*cct)break;
				cct++;
			}
			if(*cct && cur->begin_decompress)
			{
				tmp=cur->begin_decompress(fcc, in, out);
				if(tmp)return(tmp);
			}
		}
		cur=cur->next;
	}

	cur=lbxgl_codecs;
	while(cur)
	{
		if(!cur->fcc && cur->begin_decompress)
		{
			tmp=cur->begin_decompress(fcc, in, out);
			if(tmp)return(tmp);
		}
		cur=cur->next;
	}

	printf("Codec: Decode Fail Codec %.4s\n", &fcc);

	return(NULL);
}

BGBBTJ_VidCodecCTX *BGBBTJ_Codec_BeginCompress(int fcc,
	BGBBTJ_BMPInfoHeader *in, BGBBTJ_BMPInfoHeader *out)
{
	BGBBTJ_VidCodec *cur;
	BGBBTJ_VidCodecCTX *tmp;
	int *cct;

	BGBBTJ_Codec_Init();

	cur=lbxgl_codecs;
	while(cur)
	{
		if(cur->fcc)
		{
			cct=cur->fcc;
			while(*cct)
			{
				if(fcc==*cct)break;
				cct++;
			}
			if(*cct && cur->begin_compress)
			{
				tmp=cur->begin_compress(fcc, in, out);
				if(tmp)return(tmp);
			}
		}
		cur=cur->next;
	}

	cur=lbxgl_codecs;
	while(cur)
	{
		if(!cur->fcc && cur->begin_compress)
		{
			tmp=cur->begin_compress(fcc, in, out);
			if(tmp)return(tmp);
		}
		cur=cur->next;
	}

	printf("Codec: Encode Fail Codec %.4s/%.4s \n",
		&fcc, &out->biCompression);
	return(NULL);
}

//default: base at upper left corner, rgba ordering, a=255
int BGBBTJ_Codec_DecompressFrame(BGBBTJ_VidCodecCTX *ctx,
	void *src, void *dst, int ssz, int dsz)
{
	if(!ctx->decompress_frame)return(-1);
	return(ctx->decompress_frame(ctx, src, dst, ssz, dsz));
}

int BGBBTJ_Codec_DecompressFrameClrs(BGBBTJ_VidCodecCTX *ctx,
	void *src, void *dst, int ssz, int dsz, int clrs)
{
	if(!ctx->decompress_frame_clrs)return(-1);
	return(ctx->decompress_frame_clrs(ctx, src, dst, ssz, dsz, clrs));
}

int BGBBTJ_Codec_DecompressFrame2(BGBBTJ_VidCodecCTX *ctx,
	void *src, void *dst, int ssz, int dsz, int clrs)
{
	if(ctx->decompress_frame_clrs)
	{
		return(ctx->decompress_frame_clrs(ctx,
			src, dst, ssz, dsz, clrs));
	}

	if(ctx->decompress_frame)
	{
		return(ctx->decompress_frame(ctx,
			src, dst, ssz, dsz));
	}

	return(-1);
}

//default: base at upper left corner, rgba ordering, a=255
int BGBBTJ_Codec_CompressFrame(BGBBTJ_VidCodecCTX *ctx,
	void *src, void *dst, int dsz, int qfl, int clrs, int *rfl)
{
	if(!ctx->compress_frame)return(-1);
	return(ctx->compress_frame(ctx, src, dst, dsz, qfl, clrs, rfl));
}

int BGBBTJ_VidCodecCTX_EndCompress(BGBBTJ_VidCodecCTX *ctx)
{
	if(!ctx->end_compress)return(-1);
	return(ctx->end_compress(ctx));
}

int BGBBTJ_VidCodecCTX_EndDecompress(BGBBTJ_VidCodecCTX *ctx)
{
	if(!ctx->end_decompress)return(-1);
	return(ctx->end_decompress(ctx));
}

int BGBBTJ_Codec_Init()
{
	static int init;

	if(init)return(0);
	init=1;

//	BGBBTJ_CodecMJPG_Init();
	BGBBTJ_CodecVFW_Init();
	BGBBTJ_CodecMJPG_Init();
	BGBBTJ_CodecMPNG_Init();
	BGBBTJ_CodecMBTC_Init();
	BGBBTJ_CodecTHEO_Init();
	BGBBTJ_CodecRPZA_Init();
	BGBBTJ_CodecBT1C_Init();
	BGBBTJ_CodecBT1D_Init();
	BGBBTJ_CodecBT2C_Init();
	BGBBTJ_CodecBTIC_Init();

	return(1);
}
