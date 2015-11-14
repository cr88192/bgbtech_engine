#include <bgbbtj.h>

static BGBBTJ_VidCodecCTX *btic_begin_decompress(int fcc,
	BGBBTJ_BMPInfoHeader *in, BGBBTJ_BMPInfoHeader *out)
{
	BGBBTJ_VidCodecCTX *ctx;
//	vfw_ctxinfo *info;
	int err;

	if(fcc!=RIFF_TAG_BTIC)
		return(NULL);

	fcc=in->biCompression;
	switch(fcc)
	{
	case BGBBTJ_FCC_bt1a:
		ctx=mbtc_begin_decompress(fcc, in, out);
		break;
	case BGBBTJ_FCC_bt1c:
		ctx=bt1c_begin_decompress(fcc, in, out);
		break;
	case BGBBTJ_FCC_bt1d:
		ctx=bt1d_begin_decompress(fcc, in, out);
		break;
	case BGBBTJ_FCC_bt2c:
		ctx=bt2c_begin_decompress(fcc, in, out);
		break;	
	default:
		ctx=NULL;
		break;
	}

	return(ctx);
}

static BGBBTJ_VidCodecCTX *btic_begin_compress(int fcc,
	BGBBTJ_BMPInfoHeader *in, BGBBTJ_BMPInfoHeader *out)
{
	BGBBTJ_VidCodecCTX *ctx;
//	vfw_ctxinfo *info;
	int err;

	if(fcc!=RIFF_TAG_BTIC)
		return(NULL);

	fcc=out->biCompression;
	switch(fcc)
	{
//	case BGBBTJ_FCC_bt1a:
//		ctx=mbtc_begin_compress(fcc, in, out);
//		break;
	case BGBBTJ_FCC_bt1c:
		ctx=bt1c_begin_compress(fcc, in, out);
		break;
	case BGBBTJ_FCC_bt1d:
		ctx=bt1d_begin_compress(fcc, in, out);
		break;
	case BGBBTJ_FCC_bt2c:
		ctx=bt2c_begin_compress(fcc, in, out);
		break;	
	default:
		ctx=NULL;
		break;
	}

	return(ctx);
}

int btic_decompress_query(int fcc,
	BGBBTJ_BMPInfoHeader *in, BGBBTJ_BMPInfoHeader *out)
{
	int err;

	if(fcc!=RIFF_TAG_BTIC)
		return(0);

	switch(in->biCompression)
	{
//	case BGBBTJ_FCC_bt1a:		err=1; break;
	case BGBBTJ_FCC_bt1c:		err=1; break;
	case BGBBTJ_FCC_bt1d:		err=1; break;
	case BGBBTJ_FCC_bt2c:		err=1; break;	
	default:				err=0; break;
	}
	return(err);
}

int btic_compress_query(int fcc,
	BGBBTJ_BMPInfoHeader *in, BGBBTJ_BMPInfoHeader *out)
{
	int err;

	if(fcc!=RIFF_TAG_BTIC)
		return(0);

	switch(out->biCompression)
	{
	case BGBBTJ_FCC_bt1a:		err=1; break;
	case BGBBTJ_FCC_bt1c:		err=1; break;
	case BGBBTJ_FCC_bt1d:		err=1; break;
	case BGBBTJ_FCC_bt2c:		err=1; break;	
	default:					err=0; break;
	}
	return(err);
}

int BGBBTJ_CodecBTIC_Init()
{
	BGBBTJ_VidCodec *codec;
	int i, j;
	short *sw;
	char buf[256];
	char *t;

	printf("BTIC: Init\n");

	codec=BGBBTJ_VidCodec_New();
	codec->begin_decompress=&btic_begin_decompress;
	codec->begin_compress=&btic_begin_compress;
	codec->decompress_query=&btic_decompress_query;
	codec->compress_query=&btic_compress_query;
	return(0);
}
