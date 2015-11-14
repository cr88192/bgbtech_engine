#include <lbxgl2.h>

#ifndef RIFF_MAKETAG
#define RIFF_MAKETAG(a, b, c, d)	((a)+((b)<<8)+((c)<<16)+((d)<<24))
#endif

LBXGL_VidCodec *lbxgl_codecs=NULL;

LBXGL_VidCodec *LBXGL_VidCodec_New()
{
	LBXGL_VidCodec *tmp;

	tmp=gctalloc("lbxgl_vidcodec_t", sizeof(LBXGL_VidCodec));
	tmp->next=lbxgl_codecs;
	lbxgl_codecs=tmp;

	return(tmp);
}

LBXGL_VidCodecCTX *LBXGL_VidCodecCTX_New()
{
	LBXGL_VidCodecCTX *tmp;

	tmp=gctalloc("lbxgl_vidcodecctx_t", sizeof(LBXGL_VidCodecCTX));
	return(tmp);
}

LBXGL_VidCodecCTX *LBXGL_Codec_BeginDecompress(int fcc,
	PDGL_BMPInfoHeader *in, PDGL_BMPInfoHeader *out)
{
	LBXGL_VidCodec *cur;
	LBXGL_VidCodecCTX *tmp;
	int *cct;

	LBXGL_Codec_Init();

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
			if(*cct)
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
		if(!cur->fcc)
		{
			tmp=cur->begin_decompress(fcc, in, out);
			if(tmp)return(tmp);
		}
		cur=cur->next;
	}

	printf("Codec: Fail Codec %.4s\n", &fcc);

	return(NULL);
}

//default: base at upper left corner, rgba ordering, a=255
int LBXGL_Codec_DecompressFrame(LBXGL_VidCodecCTX *ctx,
	void *src, void *dst, int ssz, int dsz)
{
	if(!ctx->decompress_frame)return(-1);
	return(ctx->decompress_frame(ctx, src, dst, ssz, dsz));
}

int LBXGL_Codec_Init()
{
	static int init;

	if(init)return(0);
	init=1;

//	LBXGL_CodecMJPG_Init();
	LBXGL_CodecVFW_Init();
	LBXGL_CodecMJPG_Init();
	LBXGL_CodecMPNG_Init();
	LBXGL_CodecMBTC_Init();
	LBXGL_CodecRPZA_Init();

	return(1);
}
