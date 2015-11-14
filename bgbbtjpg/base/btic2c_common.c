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

const u64 btic2c_dbase2[128]={
		//(16-bit range)
		0x0000, 0x0001, 0x0002, 0x0003,	//0..3
		0x0004, 0x0006, 0x0008, 0x000C,
		0x0010, 0x0018, 0x0020, 0x0030,
		0x0040, 0x0060, 0x0080, 0x00C0,
		0x0100, 0x0180, 0x0200, 0x0300,
		0x0400, 0x0600, 0x0800, 0x0C00,
		0x1000, 0x1800, 0x2000, 0x3000,
		0x4000, 0x6000, 0x8000, 0xC000,	//28..31
		//(32-bit range)
		0x00010000, 0x00018000, 0x00020000, 0x00030000,	//32..35
		0x00040000, 0x00060000, 0x00080000, 0x000C0000,
		0x00100000, 0x00180000, 0x00200000, 0x00300000,
		0x00400000, 0x00600000, 0x00800000, 0x00C00000,
		0x01000000, 0x01800000, 0x02000000, 0x03000000,
		0x04000000, 0x06000000, 0x08000000, 0x0C000000,
		0x10000000, 0x18000000, 0x20000000, 0x30000000,
		0x40000000, 0x60000000, 0x80000000, 0xC0000000,	//60..63
		//(64-bit range)
		0x0000000100000000ULL, 0x0000000180000000ULL,	//64, 65
		0x0000000200000000ULL, 0x0000000300000000ULL,
		0x0000000400000000ULL, 0x0000000600000000ULL,
		0x0000000800000000ULL, 0x0000000C00000000ULL,
		0x0000001000000000ULL, 0x0000001800000000ULL,
		0x0000002000000000ULL, 0x0000003000000000ULL,
		0x0000004000000000ULL, 0x0000006000000000ULL,
		0x0000008000000000ULL, 0x000000C000000000ULL,
		0x0000010000000000ULL, 0x0000018000000000ULL,
		0x0000020000000000ULL, 0x0000030000000000ULL,
		0x0000040000000000ULL, 0x0000060000000000ULL,
		0x0000080000000000ULL, 0x00000C0000000000ULL,
		0x0000100000000000ULL, 0x0000180000000000ULL,
		0x0000200000000000ULL, 0x0000300000000000ULL,
		0x0000400000000000ULL, 0x0000600000000000ULL,
		0x0000800000000000ULL, 0x0000C00000000000ULL,	//94, 95
		0x0001000000000000ULL, 0x0001800000000000ULL,	//96, 97
		0x0002000000000000ULL, 0x0003000000000000ULL,
		0x0004000000000000ULL, 0x0006000000000000ULL,
		0x0008000000000000ULL, 0x000C000000000000ULL,
		0x0010000000000000ULL, 0x0018000000000000ULL,
		0x0020000000000000ULL, 0x0030000000000000ULL,
		0x0040000000000000ULL, 0x0060000000000000ULL,
		0x0080000000000000ULL, 0x00C0000000000000ULL,
		0x0100000000000000ULL, 0x0180000000000000ULL,
		0x0200000000000000ULL, 0x0300000000000000ULL,
		0x0400000000000000ULL, 0x0600000000000000ULL,
		0x0800000000000000ULL, 0x0C00000000000000ULL,
		0x1000000000000000ULL, 0x1800000000000000ULL,
		0x2000000000000000ULL, 0x3000000000000000ULL,
		0x4000000000000000ULL, 0x6000000000000000ULL,
		0x8000000000000000ULL, 0xC000000000000000ULL,	//126, 127
		};

const int btic2c_dextra[128]={
		//(16-bit range)
		 0,  0,  0,  0,  1,  1,  2,  2,
		 3,  3,  4,  4,  5,  5,  6,  6,
		 7,  7,  8,  8,  9,  9, 10, 10,
		11, 11, 12, 12, 13, 13, 14, 14,
		//(32-bit range)
		15, 15, 16, 16, 17, 17, 18, 18,
		19, 19, 20, 20, 21, 21, 22, 22,
		23, 23, 24, 24, 25, 25, 26, 26,
		27, 27, 28, 28, 29, 29, 30, 30,
		//(64-bit range)
		31, 31, 32, 32, 33, 33, 34, 34,
		35, 35, 36, 36, 37, 37, 38, 38,
		39, 39, 40, 40, 41, 41, 42, 42,
		43, 43, 44, 44, 45, 45, 46, 46,
		47, 47, 48, 48, 49, 49, 50, 50,
		51, 51, 52, 52, 53, 53, 54, 54,
		55, 55, 56, 56, 57, 57, 58, 58,
		59, 59, 60, 60, 61, 61, 62, 62
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

//		if(out->biCompression==RIFF_TAG_BGRA)
//			info->clrs=BGBBTJ_JPG_BGRA;

		if(out->biCompression==RIFF_TAG_RGBX)
		{
			info->clrs=(out->biBitCount==24)?
				BGBBTJ_JPG_RGB:BGBBTJ_JPG_RGBX;
		}

		if(out->biCompression==RIFF_TAG_RGBA)
		{
			info->clrs=(out->biBitCount==24)?
				BGBBTJ_JPG_RGB:BGBBTJ_JPG_RGBA;
		}

		if(out->biCompression==RIFF_TAG_BGRX)
		{
			info->clrs=(out->biBitCount==24)?
				BGBBTJ_JPG_BGR:BGBBTJ_JPG_BGRX;
		}

		if(out->biCompression==RIFF_TAG_BGRA)
		{
			info->clrs=(out->biBitCount==24)?
				BGBBTJ_JPG_BGR:BGBBTJ_JPG_BGRA;
		}

		if(out->biCompression==BGBBTJ_FCC_DXT1)
			{ info->clrs=BGBBTJ_JPG_DXT1; }
		if(out->biCompression==BGBBTJ_FCC_DXT5)
			{ info->clrs=BGBBTJ_JPG_DXT5; }
		if(out->biCompression==BGBBTJ_FCC_BC6H)
			{ info->clrs=BGBBTJ_JPG_BC6H; }
		if(out->biCompression==BGBBTJ_FCC_BC7)
			{ info->clrs=BGBBTJ_JPG_BC7; }
	}
	else
		{ info->clrs=BGBBTJ_JPG_RGBA; }

	info->flip=((short)(out->biHeight))<0;

	ctx->decompress_frame=&bt2c_decompress_frame;
	ctx->decompress_frame_clrs=&bt2c_decompress_frame_clrs;

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
