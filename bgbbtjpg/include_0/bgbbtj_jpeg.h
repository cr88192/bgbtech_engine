#define	BGBBTJ_JPG_RGBA			0	//RGBA(32)
#define	BGBBTJ_JPG_RGB			1	//RGB(24)
#define	BGBBTJ_JPG_BGRA			3	//BGRA(32)
#define	BGBBTJ_JPG_BGR			4	//BGR(24)
#define	BGBBTJ_JPG_YUVA			5	//YUVA(32)
#define	BGBBTJ_JPG_YUV			6	//YUV(24)
#define	BGBBTJ_JPG_Y			7	//Y(8)
#define	BGBBTJ_JPG_YA			8	//YA(16)
#define	BGBBTJ_JPG_YUV422		9	//YYUV
#define	BGBBTJ_JPG_YUV420		10	//YYU/YYV (YYYYUV)

#define	BGBBTJ_JPG_YYYA			11
#define	BGBBTJ_JPG_ARGB			12
#define	BGBBTJ_JPG_ABGR			13
#define	BGBBTJ_JPG_UYVY			14	//UYVY Ordering (4:2:2)
#define	BGBBTJ_JPG_YUYV			15	//YUYV Ordering (4:2:2)

#define	BGBBTJ_JPG_BC1			16	//BC1 / DXT1
#define	BGBBTJ_JPG_BC2			17	//BC2 / DXT3
#define	BGBBTJ_JPG_BC3			18	//BC3 / DXT5
#define	BGBBTJ_JPG_BC4			19	//BC4
#define	BGBBTJ_JPG_BC5			20	//BC5
#define	BGBBTJ_JPG_BC6			21	//BC6H (Signed)
#define	BGBBTJ_JPG_BC7			22	//BC7
#define	BGBBTJ_JPG_BC1F			23	//BC1 / DXT1
#define	BGBBTJ_JPG_BC3F			24	//BC3 / DXT5
#define	BGBBTJ_JPG_BC1A			25	//BC1 / DXT1
#define	BGBBTJ_JPG_BC3_UVAY		26	//DXT5-UVAY
#define	BGBBTJ_JPG_BC7_SRGB		27	//BC7 (SRGBA)
#define	BGBBTJ_JPG_BC6_UF16		28	//BC6H (Unsigned)
#define	BGBBTJ_JPG_BC4A			29	//BC4 (Alpha Only)

#define	BGBBTJ_JPG_RGBA_Q11_4	32	//RGBA(64 bit, fixed Q11.4)
#define	BGBBTJ_JPG_RGB_Q11_4	33	//RGB(48 bit, fixed Q11.4)
#define	BGBBTJ_JPG_RGBA_F32		34	//RGBA(128 bit, float32)
#define	BGBBTJ_JPG_RGB_F32		35	//RGB(96 bit, float32)
#define	BGBBTJ_JPG_RGBA_F16		36	//RGBA(64 bit, float16)
#define	BGBBTJ_JPG_RGB_F16		37	//RGB(48 bit, float16)
#define	BGBBTJ_JPG_RGBA_LS16	38	//RGBA(64 bit, log-scale)
#define	BGBBTJ_JPG_RGB_LS16		39	//RGB(48 bit, log-scale)

#define	BGBBTJ_JPG_RGBA_VF		40	//RGBA (Vertical Flip)
#define	BGBBTJ_JPG_RGB_VF		41	//RGB (Vertical Flip)
#define	BGBBTJ_JPG_RGBA_F16_VF	42	//RGBA(64 bit, float16)
#define	BGBBTJ_JPG_RGB_F16_VF	43	//RGB(48 bit, float16)

#define	BGBBTJ_JPG_BC1_VF		48	//BC1 / DXT1 (Vertical Flip)
#define	BGBBTJ_JPG_BC2_VF		49	//BC2 / DXT3 (Vertical Flip)
#define	BGBBTJ_JPG_BC3_VF		50	//BC3 / DXT5 (Vertical Flip)
#define	BGBBTJ_JPG_BC4_VF		51	//BC4 (Vertical Flip)
#define	BGBBTJ_JPG_BC5_VF		52	//BC5 (Vertical Flip)
#define	BGBBTJ_JPG_BC6_VF		53	//BC6H (Signed) (Vertical Flip)
#define	BGBBTJ_JPG_BC7_VF		54	//BC7 (Vertical Flip)
#define	BGBBTJ_JPG_BC6_VF2		55	//BC6H (Signed) (Vertical Flip)
#define	BGBBTJ_JPG_BC7_VF2		56	//BC7 (Vertical Flip)

#define	BGBBTJ_JPG_MB5C			57	//MB5C
#define	BGBBTJ_JPG_MB5C_VF		58	//BC7 (Vertical Flip)

#define	BGBBTJ_JPG_BC6_UVF		60	//BC6H (Unsigned)

#define	BGBBTJ_JPG_DXT1			BGBBTJ_JPG_BC1
#define	BGBBTJ_JPG_DXT3			BGBBTJ_JPG_BC2
#define	BGBBTJ_JPG_DXT5			BGBBTJ_JPG_BC3
#define	BGBBTJ_JPG_DXT1F		BGBBTJ_JPG_BC1F
#define	BGBBTJ_JPG_DXT5F		BGBBTJ_JPG_BC3F
#define	BGBBTJ_JPG_DXT5_UVAY	BGBBTJ_JPG_BC3_UVAY
#define	BGBBTJ_JPG_DXT1A		BGBBTJ_JPG_BC1A
#define	BGBBTJ_JPG_BC6H			BGBBTJ_JPG_BC6
#define	BGBBTJ_JPG_BC6H_SF16	BGBBTJ_JPG_BC6
#define	BGBBTJ_JPG_BC6H_UF16	BGBBTJ_JPG_BC6_UF16

#define	BGBBTJ_JPG_BC6H_VF		BGBBTJ_JPG_BC6_VF
#define	BGBBTJ_JPG_BC6H_SF16_VF	BGBBTJ_JPG_BC6_VF
// #define	BGBBTJ_JPG_BC6H_UF16_VF	BGBBTJ_JPG_BC6_UF16

#define	BGBBTJ_JPG_CLRS_YCBCR		0
#define	BGBBTJ_JPG_CLRS_ORCT		1
#define	BGBBTJ_JPG_CLRS_RGB			2
#define	BGBBTJ_JPG_CLRS_YCCK		3

#define	BGBBTJ_JPG_QFL_RDCT			256
#define	BGBBTJ_JPG_QFL_444			512
#define	BGBBTJ_JPG_QFL_ORCT			1024
#define	BGBBTJ_JPG_QFL_RGB			2048

#define	BGBBTJ_QFL_RDCT				256
#define	BGBBTJ_QFL_444				512
#define	BGBBTJ_QFL_ORCT				1024
#define	BGBBTJ_QFL_RGB				2048

//features, to be implemented in C version
#define	BGBBTJ_QFL_NOSOI			4096
#define	BGBBTJ_QFL_LAYER_HIDDEN		8192
#define	BGBBTJ_QFL_MEGABLOCK		16384
#define	BGBBTJ_QFL_ALTVLC			32768
#define	BGBBTJ_QFL_ALPHACYAN		65536
#define	BGBBTJ_QFL_RANGECODER		131072
#define	BGBBTJ_QFL_AUTODCT			262144

#define	BGBBTJ_QFL_BITALPHA			524288		//1bpp inline alpha

#define	BGBBTJ_QFL_IFRAME			1048576
#define	BGBBTJ_QFL_PFRAME			2097152
#define	BGBBTJ_QFL_SUBLAYER			4194304
#define	BGBBTJ_QFL_TAGBASELAYER		8388608

#define	BGBBTJ_QFL_410				(1<<24)		//use 4:1:0 subsampling
#define	BGBBTJ_QFL_BATCHENCODE		(1<<25)		//doing offline encoding
#define	BGBBTJ_QFL_BLOCKINDEX		(1<<26)		//enable block indexing
#define	BGBBTJ_QFL_MIPMAP			(1<<27)		//use mipmaps
#define	BGBBTJ_QFL_BLOCKMOTION		(1<<28)		//use mipmaps

#define	BGBBTJ_JPG_QFL_LOSSLESS		(BGBBTJ_JPG_QFL_RDCT|BGBBTJ_JPG_QFL_444| \
									BGBBTJ_JPG_QFL_ORCT)

#define	BGBBTJ_JPG_BCSFL_RDCT		1

#define	BGBBTJ_JPG_CTY_NONE			0
#define	BGBBTJ_JPG_CTY_RGB			3
#define	BGBBTJ_JPG_CTY_RGBA			4
#define	BGBBTJ_JPG_CTY_RGBA_LUMA	7
#define	BGBBTJ_JPG_CTY_RGBA_NORM	8
#define	BGBBTJ_JPG_CTY_RGBA_NVSP	12
#define	BGBBTJ_JPG_CTY_RGBA_NVSPLU	16


#define DCTSZ	8
#define DCTSZ2	64

//typedef unsigned char byte;
//typedef unsigned short ushort;
//typedef unsigned int uint;

#define JPG_SOF0	0xC0
#define JPG_SOF1	0xC1
#define JPG_SOF2	0xC2
#define JPG_SOF3	0xC3
#define JPG_DHT		0xC4
#define JPG_SOF5	0xC5
#define JPG_SOF6	0xC6
#define JPG_SOF7	0xC7
#define JPG_JPG		0xC8
#define JPG_SOF9	0xC9
#define JPG_SOF10	0xCA
#define JPG_SOF11	0xCB
#define JPG_DAC		0xCC
#define JPG_SOF13	0xCD
#define JPG_SOF14	0xCE
#define JPG_SOF15	0xCF

#define JPG_RST0	0xD0
#define JPG_RST1	0xD1
#define JPG_RST2	0xD2
#define JPG_RST3	0xD3
#define JPG_RST4	0xD4
#define JPG_RST5	0xD5
#define JPG_RST6	0xD6
#define JPG_RST7	0xD7

#define JPG_SOI		0xD8
#define JPG_EOI		0xD9
#define JPG_SOS		0xDA
#define JPG_DQT		0xDB
#define JPG_DNL		0xDC
#define JPG_DRI		0xDD
#define JPG_DHP		0xDE
#define JPG_EXP		0xDF

#define JPG_APP0	0xE0
#define JPG_APP1	0xE1
#define JPG_APP2	0xE2
#define JPG_APP3	0xE3
#define JPG_APP4	0xE4
#define JPG_APP5	0xE5
#define JPG_APP6	0xE6
#define JPG_APP7	0xE7
#define JPG_APP8	0xE8
#define JPG_APP9	0xE9
#define JPG_APP10	0xEA
#define JPG_APP11	0xEB
#define JPG_APP12	0xEC
#define JPG_APP13	0xED
#define JPG_APP14	0xEE
#define JPG_APP15	0xEF

#define JPG_JPG0	0xF0
#define JPG_JPG1	0xF1
#define JPG_JPG2	0xF2
#define JPG_JPG3	0xF3
#define JPG_JPG4	0xF4
#define JPG_JPG5	0xF5
#define JPG_JPG6	0xF6
#define JPG_JPG7	0xF7
#define JPG_JPG8	0xF8
#define JPG_JPG9	0xF9
#define JPG_JPG10	0xFA
#define JPG_JPG11	0xFB
#define JPG_JPG12	0xFC
#define JPG_JPG13	0xFD
#define JPG_COM		0xFE

#if 1
#define pdjhuff_cs		ctx->huff_cs
#define pdjhuff_ct		ctx->huff_ct
#define pdjhuff_win		ctx->huff_win
#define pdjhuff_pos		ctx->huff_pos
#define pdjhuff_isend	ctx->huff_isend
#endif


typedef struct BGBBTJ_JPG_Context_s BGBBTJ_JPG_Context;
typedef struct BGBBTJ_JPG_TagLayer_s BGBBTJ_JPG_TagLayer;
typedef struct BGBBTJ_JPG_CompLayer_s BGBBTJ_JPG_CompLayer;
typedef struct BGBBTJ_JPG_LayerInfo_s BGBBTJ_JPG_LayerInfo;

typedef struct PDBTV0_Context_s PDBTV0_Context;
typedef struct BGBBTJ_JFE_Context_s BGBBTJ_JFE_Context;

struct BGBBTJ_JPG_Context_s
{
//Huffman
u16 huff_code[8*256];	//huffman code bits
u16 huff_mask[8*256];	//huffman code masks
s16 huff_next[8*256];	//huffman code next
byte huff_len[8*256];	//huffman code length
s16 huff_idx[8*256];	//huffman code masks

byte *huff_cs;			//current pos in bitstream (input)
byte *huff_ct;			//current pos in bitstream (output)
uint huff_win;			//bitstream window
int huff_pos;			//bitstream offset
int huff_isend;			//bitstream has broken (decoder)

//Common

byte jpg_qt[4][64];		//quantization tables
int jpg_qtfp[4][64];	//quantization tables (fixed point)

int alphaClr;			//color used for alpha (RGBA)
byte alphaClrY;
byte alphaClrU;
byte alphaClrV;
byte alphaClrA;
int oldAlphaClr;		//used to detect change

//Decoder Specific

int xs, ys;		//image size (raw image)
int xs2, ys2;	//image size (Y plane)
int xs3, ys3;	//image size (UV planes)


byte jpg_cid[16];
byte jpg_ch[16];
byte jpg_cv[16];
byte jpg_qid[16];
int jpg_cxi[16];
int jpg_cyi[16];
int jpg_nc;
int jpg_chm;
int jpg_chn;

int jpg_scid[4];
int jpg_scn[4];
short *jpg_scbuf[4];
// byte *jpg_sibuf[4];
short *jpg_sibuf[4];

int jpg_schs[4];		//scan horizontal size
int jpg_scvs[4];		//scan vertical size
// float jpg_schsc[4];		//scan horizontal scale
// float jpg_scvsc[4];		//scan vertical scale

int jpg_schsci[4];		//scan horizontal scale
int jpg_scvsci[4];		//scan vertical scale

byte *jpg_imgbuf;		//image buffer

byte jpg_is420;			//image is 420
byte jpg_is444;			//image is 444
byte jpg_is410;			//image is 410
byte jpg_rdct;			//image uses RDCT
byte jpg_clrtrans;		//color space transform
byte jpg_yuvw;			//image uses YUVW

u16 bcst_version;
u32 bcst_flags;
byte bcst_trans;
byte bcst_bpp;
byte bcst_alpha;
byte bcst_blend;
int bcst_orgx;
int bcst_orgy;

int bcst_minx;
int bcst_miny;
int bcst_maxx;
int bcst_maxy;

byte *imgbuf_rgba;		//RGBA image buffer
byte *imgbuf_norm;		//XYZD image buffer
byte *imgbuf_spec;		//Spec image buffer
byte *imgbuf_luma;		//Luma image buffer
byte *imgbuf_lebe;		//LeBe image buffer

// u16 jpg_app14_dctversion;
// u16 jpg_app14_flags0;
// u16 jpg_app14_flags1;
// byte jpg_app14_trans;

//Encoder Specific
int jpg_mono;
int jpg_tabcacheframe;	//cache table frame count.

//byte *yb;
//byte *ub;
//byte *vb;
//byte *wb;

short *yb;		//Y pixel buffer
short *ub;		//U pixel buffer
short *vb;		//V pixel buffer
short *wb;		//W pixel buffer

short *ydb;		//Y DCT block buffer
short *udb;		//U DCT block buffer
short *vdb;		//V DCT block buffer
short *wdb;		//W DCT block buffer
int lxs;
int lys;

int dcs[256];
int acs[256];
int dcsuv[256];
int acsuv[256];

// byte *bab;		//bit alpha buffer
byte *badb;		//bit alpha block buffer
int sz_badb;

//layers / sub-layers
BGBBTJ_JPG_CompLayer *image[256];
BGBBTJ_JPG_TagLayer *layer[64];
int n_image;
int n_layer;
};

struct BGBBTJ_JPG_TagLayer_s
{
s32 layerid;		//layer ID number
s32 xorg;			//X origin of layer (relative center)
s32 yorg;			//Y origin of layer (relative center)
s32 xsize;			//X size of layer (pixels)
s32 ysize;			//Y size of layer
s32 xcenter;		//X center of layer
s32 ycenter;		//Y center of layer
u32 flags;			//layer flags

BGBBTJ_JPG_Context *ctx;
char *name;			//layer name
int n_image;
BGBBTJ_JPG_Context *image[16];	//layer images
};

struct BGBBTJ_JPG_CompLayer_s
{
BGBBTJ_JPG_Context *pctx;	//parent context
BGBBTJ_JPG_Context *ictx;	//image context
char *name;					//comp layer name
s32 layerid;
u32 flags;					//layer flags
};

struct BGBBTJ_JPG_LayerInfo_s
{
int flags;
byte alpha;
byte blend;
int orgx;
int orgy;
int xs;
int ys;

int minx;
int miny;
int maxx;
int maxy;
};

struct PDBTV0_Context_s
{
byte *oldlayer[8];
};


struct BGBBTJ_JFE_Context_s
{
// byte *cs;	//current pos in bitstream (input)
byte *ct;	//current pos in bitstream (output)
uint win;	//bitstream window
int pos;	//bitstream offset

byte qt[4][64];
int qtfp[4][64];

short *ydb, *udb, *vdb;
int lxs, lys, lqf;
};

typedef struct BGBBTJ_ArithContext_s BGBBTJ_ArithContext;

struct BGBBTJ_ArithContext_s
{
byte *cs;	//current pos in bitstream (input)
byte *ct;	//current pos in bitstream (output)
byte *cse;	//current end pos in bitstream (input)
byte *cte;	//current end pos in bitstream (output)

u32 rmin;		//window lower range
u32 rmax;		//window upper range
u32 rval;		//window decode value
// u32 range;		//window decode range

int wctx;
byte *model;	//pairs of counts
int ctxbits;	//context bits
int ctxmask;	//context mask
};
