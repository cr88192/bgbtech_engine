#define BGBBTJ_BTIC1C_EFL_BLOCKLZ	1
#define BGBBTJ_BTIC1C_EFL_MIPMAP	2
#define BGBBTJ_BTIC1C_EFL_DIFFCLR	4
#define BGBBTJ_BTIC1C_EFL_BLOCKZI	8
#define BGBBTJ_BTIC1C_EFL_EXTMODES	16
#define BGBBTJ_BTIC1C_EFL_MOTION	32
#define BGBBTJ_BTIC1C_EFL_USEARITH	64		//use arithmetic coding

#define BGBBTJ_BTIC1C_EFL_BLKMB1	65536

#define BGBBTJ_BTIC1C_STFL_LBJPEG	1		//last block was JPEG

#define BGBBTJ_BTIC1C_QFL_BLOCKZI	BGBBTJ_QFL_NOSOI
#define BGBBTJ_BTIC1C_QFL_DIFFCLR	BGBBTJ_QFL_AUTODCT
#define BGBBTJ_BTIC1C_QFL_MOTION	BGBBTJ_QFL_BLOCKMOTION
#define BGBBTJ_BTIC1C_QFL_EXTMODES	BGBBTJ_QFL_ALTVLC
#define BGBBTJ_BTIC1C_QFL_INDEXCLR	BGBBTJ_QFL_410
#define BGBBTJ_BTIC1C_QFL_ALTJPEG	BGBBTJ_QFL_RDCT

//1000 0100 0001 0000
#define BGBBTJ_BTIC1C_DIFFBIAS565	0x8410

#define BGBBTJ_BTIC1C_DFL_NONE			0
#define BGBBTJ_BTIC1C_DFL_HASAX			1
#define BGBBTJ_BTIC1C_DFL_FLIPBLOCKS	2
#define BGBBTJ_BTIC1C_DFL_BC6H_UF		4
#define BGBBTJ_BTIC1C_DFL_RGB23B		8
#define BGBBTJ_BTIC1C_DFL_SCALEHDR2LDR	16
#define BGBBTJ_BTIC1C_DFL_YYYVECTOR		32

#define BGBBTJ_BTIC1C_DFL_FRAMESTICKY	\
	(BGBBTJ_BTIC1C_DFL_RGB23B|BGBBTJ_BTIC1C_DFL_SCALEHDR2LDR)

#define BGBBTJ_BTIC1C_CTRL_BLOCKMODE	0
#define BGBBTJ_BTIC1C_CTRL_VERSION		1
#define BGBBTJ_BTIC1C_CTRL_MINVERSION	2
#define BGBBTJ_BTIC1C_CTRL_SCALEHDR		3

#define BGBBTJ_BTIC1C_VERSION			0x10

#define BGBBTJ_BTIC1C_CTRFL_RGB23B		0

typedef struct BGBBTJ_BTIC1C_Context_s BGBBTJ_BTIC1C_Context;
typedef struct BGBBTJ_BTIC1C_LayerInfo_s BGBBTJ_BTIC1C_LayerInfo;
typedef struct BGBBTJ_BTIC1C_ImageInfo_s BGBBTJ_BTIC1C_ImageInfo;

struct BGBBTJ_BTIC1C_Context_s
{
s32 width;			//width of image
s32 height;			//height of image
int clrs;

byte *frm_css, *frm_cse;

int n_layer;
int n_image;
BGBBTJ_BTIC1C_LayerInfo *layer[64];
BGBBTJ_BTIC1C_ImageInfo *image[256];
// byte *ct;

BGBBTJ_BTIC1C_LayerInfo *cur_layer;
BGBBTJ_BTIC1C_ImageInfo *cur_img;

int imgtype;
int qflags;
int eflags;

bool flip;
};

struct BGBBTJ_BTIC1C_LayerInfo_s
{
s32 layerid;		//layer ID number
s32 xorg;			//X origin of layer (relative center)
s32 yorg;			//Y origin of layer (relative center)
s32 xsize;			//X size of layer (pixels)
s32 ysize;			//Y size of layer
s32 xcenter;		//X center of layer
s32 ycenter;		//Y center of layer
u32 flags;			//layer flags

BGBBTJ_BTIC1C_Context *ctx;
char *name;			//layer name
int n_image;
BGBBTJ_BTIC1C_ImageInfo *image[16];	//layer images
};

struct BGBBTJ_BTIC1C_ImageInfo_s
{
s32 layerid;		//image layer ID number
s32 width;			//width of image
s32 height;			//height of image
u32 flags;			//image layer flags
int imgtype;		//image type
byte mip_start;		//first (biggest) mip-level
byte mip_end;		//last (smallest) mip-level
byte filtmode;		//filter mode
byte clrtype;		//colorspace type
byte pixtype;		//pixel type
int eflags;			//encode time flags
int stflags;		//status flags
int dflags;			//decoder flags
int clrs;			//colorspace

BGBBTJ_BTIC1C_Context *ctx;			//owning context
BGBBTJ_BTIC1C_LayerInfo *layer;		//owning layer
char *name;							//image component name

s32 width2;			//width of blocks
s32 height2;		//height of blocks
s32 nblocks;		//size of image in blocks (base)
s32 nblocksmip;		//size of image in blocks (base+mipmap)

int ofs_pframe;		//offset of P-Frame base

int sz_data;		//size of image data
int sz_ldata;		//size of image data
int sz_fpdata;		//size of packed data
int sz_fpldata;		//size of packed literal data
int sz_rgba;		//size of RGBA data
int sz_tmrgba;		//size of RGBA data
byte *data;			//image data (unpacked)
byte *ldata;		//last image data (unpacked)
byte *fpdata;		//image data (filtered and packed)
byte *fpldata;		//image data (filtered and packed literal)
byte *rgba;			//image data (RGBA)
byte *lrgba;		//last image data (RGBA)
s16 *tmm_rgba;		//tone-map min image data (RGBA)
s16 *tmn_rgba;		//tone-map max image data (RGBA)

float *flrgba;		//image data (RGBA, float)

byte *adata;		//image alpha data (unpacked)
byte *ladata;		//last image alpha data (unpacked)

u32 pal256[256];	//256-color palette (RGB24)
u32 pal16[16];		//16 color palette (RGB24)
u32 pat256[256];	//pattern table
u16 modes[8];		//modes table
byte optfl[256];	//features flags
u32 optcntrl[32];	//controllers

u16 pal256s[256];	//256-color palette (RGB16)
u16 pal16s[16];		//16 color palette (RGB16)
u64 pal256w[256];	//256-color palette (RGB48)
u64 pal16w[16];		//16 color palette (RGB48)

u16 amodes[8];		//alpha modes table

int diff_m;			//differential mode
int diff_sc;		//differential scale
int clrdb_mode;		//color deblock mode
u32 diff_lclra;		//last color A
u32 diff_lclrb;		//last color B
u32 diff_lclrc;		//last color C
u32 diff_lclrd;		//last color D
int diff_lsc;		//last differential scale

u64 diff_lclraw;	//last color A (RGB48)
u64 diff_lclrbw;	//last color B (RGB48)
int diff_lbmode;	//last block mode

//Encoder / Extented Palette
u32 *pal_clr;		//palette colors
u16 *pal_index;		//palette index table
u32 *pal_chain;		//palette chain table
int pal_nclr;

int curmode;
int nextmode;
int modecnt[256];
int amodecnt[256];
byte cnt_jpg_inhibit;

BGBBTJ_JPG_Context *jpg_ctx;
BTIC2C_Context *bt2c_ctx;

byte *tbuf1;
byte *tbuf2;
int sz_tbuf1;
int sz_tbuf2;
};


typedef struct BGBBTJ_BTIC1C_TmpPalColor_s BGBBTJ_BTIC1C_TmpPalColor;
typedef struct BGBBTJ_BTIC1C_TmpPalNode_s BGBBTJ_BTIC1C_TmpPalNode;

struct BGBBTJ_BTIC1C_TmpPalColor_s
{
BGBBTJ_BTIC1C_TmpPalColor *next;
u16 rgb[4];
int cnt;
};

struct BGBBTJ_BTIC1C_TmpPalNode_s
{
BGBBTJ_BTIC1C_TmpPalNode *next;
BGBBTJ_BTIC1C_TmpPalColor *clrs;
BGBBTJ_BTIC1C_TmpPalNode *child[8];
u16 crgb[4];		//center RGB
float trgb[4];		//total RGB
int rgbcnt;			//RGB count
int tcnt;			//temp count
int nclrs;			//number of unique colors
};

#if defined(X86) || defined(X86_64) || defined(ARM)
#define bgbbtj_bt1c_memcpy4(dst, src)	\
	(*(u32 *)(dst)=*(u32 *)(src))
#define bgbbtj_bt1c_memcpy8(dst, src)	\
	(*(u64 *)(dst)=*(u64 *)(src))
#define bgbbtj_bt1c_memcpy16(dst, src)	\
	(((u64 *)(dst))[0]=((u64 *)(src))[0], \
	 ((u64 *)(dst))[1]=((u64 *)(src))[1])
#define bgbbtj_bt1c_memcpy32(dst, src)	\
	(((u64 *)(dst))[0]=((u64 *)(src))[0], \
	 ((u64 *)(dst))[1]=((u64 *)(src))[1], \
	 ((u64 *)(dst))[2]=((u64 *)(src))[2], \
	 ((u64 *)(dst))[3]=((u64 *)(src))[3])

#define bgbbtj_bt1c_memzero4(dst)	\
	(*(u32 *)(dst)=0)
#define bgbbtj_bt1c_memzero8(dst)	\
	(*(u64 *)(dst)=0)
#define bgbbtj_bt1c_memzero16(dst)	\
	(((u64 *)(dst))[0]=0, ((u64 *)(dst))[1]=0)
#define bgbbtj_bt1c_memzero32(dst)	\
	(((u64 *)(dst))[0]=0, ((u64 *)(dst))[1]=0, \
	 ((u64 *)(dst))[2]=0, ((u64 *)(dst))[3]=0)

#define bgbbtj_bt1c_setule16(dst, val)	\
	(*(u16 *)(dst)=(val))
#define bgbbtj_bt1c_setsle16(dst, val)	\
	(*(s16 *)(dst)=(val))
#define bgbbtj_bt1c_setule32(dst, val)	\
	(*(u32 *)(dst)=(val))
#define bgbbtj_bt1c_setsle32(dst, val)	\
	(*(s32 *)(dst)=(val))

#else

static void bgbbtj_bt1c_memcpy4(void *dst, void *src)
{
#if defined(X86) || defined(X86_64) || defined(ARM)
	*(u32 *)dst=*(u32 *)src;
#else
	memcpy(dst, src, 4);
#endif
}

static void bgbbtj_bt1c_memcpy8(void *dst, void *src)
{
#if defined(X86) || defined(X86_64) || defined(ARM)
	*(u64 *)dst=*(u64 *)src;
#else
	memcpy(dst, src, 8);
#endif
}

static void bgbbtj_bt1c_memcpy16(void *dst, void *src)
{
#if defined(X86) || defined(X86_64) || defined(ARM)
	((u64 *)dst)[0]=((u64 *)src)[0];
	((u64 *)dst)[1]=((u64 *)src)[1];
#else
	memcpy(dst, src, 16);
#endif
}

static void bgbbtj_bt1c_memcpy32(void *dst, void *src)
{
#if defined(X86) || defined(X86_64) || defined(ARM)
	((u64 *)dst)[0]=((u64 *)src)[0];
	((u64 *)dst)[1]=((u64 *)src)[1];
	((u64 *)dst)[2]=((u64 *)src)[2];
	((u64 *)dst)[3]=((u64 *)src)[3];
#else
	memcpy(dst, src, 32);
#endif
}

static void bgbbtj_bt1c_memzero4(void *dst)
{
#if defined(X86) || defined(X86_64) || defined(ARM)
	*(u32 *)dst=0;
#else
	memset(dst, 0, 8);
#endif
}

static void bgbbtj_bt1c_memzero8(void *dst)
{
#if defined(X86) || defined(X86_64) || defined(ARM)
	*(u64 *)dst=0;
#else
	memset(dst, 0, 8);
#endif
}

static void bgbbtj_bt1c_memzero16(void *dst)
{
#if defined(X86) || defined(X86_64) || defined(ARM)
	((u64 *)dst)[0]=0;
	((u64 *)dst)[1]=0;
#else
	memset(dst, 0, 16);
#endif
}

static void bgbbtj_bt1c_memzero32(void *dst)
{
#if defined(X86) || defined(X86_64) || defined(ARM)
	((u64 *)dst)[0]=0;
	((u64 *)dst)[1]=0;
	((u64 *)dst)[2]=0;
	((u64 *)dst)[3]=0;
#else
	memset(dst, 0, 32);
#endif
}

static void bgbbtj_bt1c_setule16(byte *dst, u16 val)
	{ dst[0]=val&255; dst[1]=(val>>8)&255; }
static void bgbbtj_bt1c_setsle16(byte *dst, s16 val)
	{ dst[0]=val&255; dst[1]=(val>>8)&255; }
static void bgbbtj_bt1c_setule32(byte *dst, u32 val)
{	dst[0]=val&255;			dst[1]=(val>>8)&255;
	dst[2]=(val>>16)&255;	dst[3]=(val>>24)&255;	}
static void bgbbtj_bt1c_setsle32(byte *dst, s32 val)
{	dst[0]=val&255;			dst[1]=(val>>8)&255;
	dst[2]=(val>>16)&255;	dst[3]=(val>>24)&255;	}
#endif

static u32 bgbbtj_bt1c_addrgb24(u32 a, u32 b)
{
	int tr, tg, tb;
	u32 c;

//	c=((a+b)&0xFF0000)|((a+b)&0x00FF00)|((a+b)&0x0000FF);

	tr=((a>>16)+(b>>16))&255;
	tg=((a>> 8)+(b>> 8))&255;
	tb=((a    )+(b    ))&255;
	c=(tr<<16)|(tg<<8)|tb;
	return(c);
}
