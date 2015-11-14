#include <bgbbtj.h>

#include <math.h>

#define DCTSZ	8
#define DCTSZ2	64

static const int pdjpg_zigzag[64]={
 0,  1,  5,  6, 14, 15, 27, 28,
 2,  4,  7, 13, 16, 26, 29, 42,
 3,  8, 12, 17, 25, 30, 41, 43,
 9, 11, 18, 24, 31, 40, 44, 53,
10, 19, 23, 32, 39, 45, 52, 54,
20, 22, 33, 38, 46, 51, 55, 60,
21, 34, 37, 47, 50, 56, 59, 61,
35, 36, 48, 49, 57, 58, 62, 63
};

static const int pdjpg_zigzag2[64]={
 0,  1,  8, 16,  9,  2,  3, 10,
17, 24, 32, 25, 18, 11,  4,  5,
12, 19, 26, 33, 40, 48, 41, 34,
27, 20, 13,  6,  7, 14, 21, 28,
35, 42, 49, 56, 57, 50, 43, 36,
29, 22, 15, 23, 30, 37, 44, 51,
58, 59, 52, 45, 38, 31, 39, 46,
53, 60, 61, 54, 47, 55, 62, 63
};

const u32 btic2c_dbase[64]={
		   0,    1,    2,     3,     4,     6,     8,    12,
		  16,   24,   32,    48,    64,    96,   128,   192,
		 256,  384,  512,   768,  1024,  1536,  2048,  3072,
		4096, 6144, 8192, 12288, 16384, 24576, 32768, 49152,
		//(32-bit range)
		0x00010000, 0x00018000, 0x00020000, 0x00030000,
		0x00040000, 0x00060000, 0x00080000, 0x000C0000,
		0x00100000, 0x00180000, 0x00200000, 0x00300000,
		0x00400000, 0x00600000, 0x00800000, 0x00C00000,
		0x01000000, 0x01800000, 0x02000000, 0x03000000,
		0x04000000, 0x06000000, 0x08000000, 0x0C000000,
		0x10000000, 0x18000000, 0x20000000, 0x30000000,
		0x40000000, 0x60000000, 0x80000000, 0xC0000000,
		};

const int btic2c_dextra[64]={
		 0,  0,  0,  0,  1,  1,  2,  2,
		 3,  3,  4,  4,  5,  5,  6,  6,
		 7,  7,  8,  8,  9,  9, 10, 10,
		11, 11, 12, 12, 13, 13, 14, 14,
		//(32-bit range)
		15, 15, 16, 16, 17, 17, 18, 18,
		19, 19, 20, 20, 21, 21, 22, 22,
		23, 23, 24, 24, 25, 25, 26, 26,
		27, 27, 28, 28, 29, 29, 30, 30
		};

const int btic2c_lbase[64]={
		    0,     1,     2,     3,     4,     5,     6,      7,
		    8,    10,    12,    14,    16,    20,    24,     28,
		   32,    40,    48,    56,    64,    80,    96,    112,
		  128,   160,   192,   224,   256,   320,   384,    448,
		  512,   640,   768,   896,	 1024,  1280,  1536,   1792,
		 2048,  2560,  3072,  3584,  4096,  5120,  6144,   7168,
		 8192, 10240, 12288, 14336,	16384, 20480, 24576,  28672,
		32768, 40960, 49152, 57344,	65536, 81920, 98304, 114688
		};
const int btic2c_lextra[64]={
		 0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  2,  2,  2,  2,
		 3,  3,  3,  3,  4,  4,  4,  4,  5,  5,  5,  5,  6,  6,  6,  6,
		 7,  7,  7,  7,  8,  8,  8,  8,  9,  9,  9,  9, 10, 10, 10, 10,
		11, 11, 11, 11, 12, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 14};


int bt2c_decompress_frame(BGBBTJ_VidCodecCTX *ctx,
	void *src, void *dst, int ssz, int dsz)
{
	BTIC2C_Context *info;
	int xs, ys;
	
	info=ctx->data;
	xs=info->xs;
	ys=info->ys;

	return(BTIC2C_DecodeCtx(
		info, src, dst, ssz, dsz,
		&xs, &ys, info->clrs));
}

int bt2c_decompress_frame_clrs(BGBBTJ_VidCodecCTX *ctx,
	void *src, void *dst, int ssz, int dsz, int clrs)
{
	BTIC2C_Context *info;
	int xs, ys;
	
	info=ctx->data;
	xs=info->xs;
	ys=info->ys;

	return(BTIC2C_DecodeCtx(
		info, src, dst, ssz, dsz,
		&xs, &ys, clrs));
}

int bt2c_compress_frame(BGBBTJ_VidCodecCTX *ctx,
	void *src, void *dst, int dsz, int qfl, int clrs, int *rfl)
{
	BTIC2C_Context *info;
	int sz;
	
	info=ctx->data;

	sz=BTIC2C_EncodeCtx(ctx->data, src, dst, dsz,
		info->xs, info->ys, qfl, clrs);
	*rfl=qfl;
	return(sz);
}

BGBBTJ_VidCodecCTX *bt2c_begin_decompress(int fcc,
	BGBBTJ_BMPInfoHeader *in, BGBBTJ_BMPInfoHeader *out)
{
	BGBBTJ_VidCodecCTX *ctx;
	BTIC2C_Context *info;
	int err;

	if((fcc!=BGBBTJ_FCC_bt2c) && (in->biCompression!=BGBBTJ_FCC_bt2c))
		return(NULL);

	ctx=BGBBTJ_VidCodecCTX_New();
//	info=gcalloc(sizeof(vfw_ctxinfo));
	info=BTIC2C_AllocContext();
	ctx->data=info;

//	info->width=abs(in->biWidth);
//	info->height=abs(in->biHeight);
	info->xs=abs(in->biWidth);
	info->ys=abs(in->biHeight);

	if(out)
	{
		info->clrs=(out->biBitCount==24)?
			BGBBTJ_JPG_RGB:BGBBTJ_JPG_RGBA;
			
		if(out->biCompression==RIFF_TAG_BGRA)
			info->clrs=BGBBTJ_JPG_BGRA;
	}
	else
		{ info->clrs=BGBBTJ_JPG_RGBA; }

	info->flip=((short)(out->biHeight))<0;

	ctx->decompress_frame=&bt2c_decompress_frame;

	return(ctx);
}

BGBBTJ_VidCodecCTX *bt2c_begin_compress(int fcc,
	BGBBTJ_BMPInfoHeader *in, BGBBTJ_BMPInfoHeader *out)
{
	BGBBTJ_VidCodecCTX *ctx;
	BTIC2C_Context *info;
	int err;

	if((fcc!=BGBBTJ_FCC_bt2c) && (in->biCompression!=BGBBTJ_FCC_bt2c))
		return(NULL);

	ctx=BGBBTJ_VidCodecCTX_New();
//	info=gcalloc(sizeof(vfw_ctxinfo));
	info=BTIC2C_AllocContext();
	ctx->data=info;

//	info->width=in->biWidth;
//	info->height=in->biHeight;
	info->xs=in->biWidth;
	info->ys=in->biHeight;
	info->clrs=BGBBTJ_JPG_RGBA;

	if(in && (in->biBitCount==24))
		{ info->clrs=BGBBTJ_JPG_RGB; }

	ctx->compress_frame=&bt2c_compress_frame;

	return(ctx);
}

int BGBBTJ_CodecBT2C_Init()
{
	BGBBTJ_VidCodec *codec;
	int i, j;
	short *sw;
	char buf[256];
	char *t;

	printf("BT2C: Init\n");

	codec=BGBBTJ_VidCodec_New();
	codec->begin_decompress=&bt2c_begin_decompress;
	codec->begin_compress=&bt2c_begin_compress;
	return(0);
}
