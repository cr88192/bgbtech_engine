#ifndef FOURCC
#define FOURCC(a, b, c, d) ((a)|((b)<<8)|((c)<<16)|((d)<<24))
#endif

#define BTIC1_FCC_LHDR	FOURCC('L', 'H', 'D', 'R')
#define BTIC1_FCC_LDAT	FOURCC('L', 'D', 'A', 'T')
#define BTIC1_FCC_LEND	FOURCC('L', 'E', 'N', 'D')
#define BTIC1_FCC_CLID	FOURCC('C', 'L', 'I', 'D')

#define BTIC1_FCC_THDR	FOURCC('T', 'H', 'D', 'R')
#define BTIC1_FCC_TDAT	FOURCC('T', 'D', 'A', 'T')
#define BTIC1_FCC_TLDA	FOURCC('T', 'L', 'D', 'A')
#define BTIC1_FCC_TEND	FOURCC('T', 'E', 'N', 'D')

#define BTIC1_FCC_TDAZ	FOURCC('T', 'D', 'A', 'Z')
#define BTIC1_FCC_TLDZ	FOURCC('T', 'L', 'D', 'Z')

#define BTIC1_FCC_BA1Z	FOURCC('B', 'A', '1', 'Z')


#define BGBBTJ_BTIC1_FILT_NONE			0
#define BGBBTJ_BTIC1_FILT_SCANFILT		1
#define BGBBTJ_BTIC1_FILT_BLOCKFILT		2
#define BGBBTJ_BTIC1_FILT_BLOCKPACK		3
#define BGBBTJ_BTIC1_FILT_BLOCKPACK2	4

#define BGBBTJ_BTIC1_LFL_IFRAME			1
#define BGBBTJ_BTIC1_LFL_PFRAME			2

#define BGBBTJ_BTIC1_MARK_APP0	0xE0
#define BGBBTJ_BTIC1_MARK_APP1	0xE1
#define BGBBTJ_BTIC1_MARK_APP2	0xE2
#define BGBBTJ_BTIC1_MARK_APP3	0xE3
#define BGBBTJ_BTIC1_MARK_APP4	0xE4
#define BGBBTJ_BTIC1_MARK_APP5	0xE5
#define BGBBTJ_BTIC1_MARK_APP6	0xE6
#define BGBBTJ_BTIC1_MARK_APP7	0xE7
#define BGBBTJ_BTIC1_MARK_APP8	0xE8
#define BGBBTJ_BTIC1_MARK_APP9	0xE9
#define BGBBTJ_BTIC1_MARK_APP10	0xEA
#define BGBBTJ_BTIC1_MARK_APP11	0xEB
#define BGBBTJ_BTIC1_MARK_APP12	0xEC
#define BGBBTJ_BTIC1_MARK_APP13	0xED
#define BGBBTJ_BTIC1_MARK_APP14	0xEE
#define BGBBTJ_BTIC1_MARK_APP15	0xEF

#define BGBBTJ_BTIC1_MARK_FMT0	0xF0
#define BGBBTJ_BTIC1_MARK_FMT1	0xF1
#define BGBBTJ_BTIC1_MARK_FMT2	0xF2
#define BGBBTJ_BTIC1_MARK_FMT3	0xF3
#define BGBBTJ_BTIC1_MARK_FMT4	0xF4
#define BGBBTJ_BTIC1_MARK_FMT5	0xF5
#define BGBBTJ_BTIC1_MARK_FMT6	0xF6
#define BGBBTJ_BTIC1_MARK_FMT7	0xF7
#define BGBBTJ_BTIC1_MARK_FMT8	0xF8
#define BGBBTJ_BTIC1_MARK_FMT9	0xF9
#define BGBBTJ_BTIC1_MARK_FMT10	0xFA
#define BGBBTJ_BTIC1_MARK_FMT11	0xFB
#define BGBBTJ_BTIC1_MARK_FMT12	0xFC
#define BGBBTJ_BTIC1_MARK_FMT13	0xFD
#define BGBBTJ_BTIC1_MARK_COM	0xFE
// #define BGBBTJ_BTIC1_MARK_ESC	0xFF

#define btic_malloc(sz)		btic_malloc_lln(sz, __FILE__, __LINE__)

typedef struct BGBBTJ_BTIC1_Context_s BGBBTJ_BTIC1_Context;
typedef struct BGBBTJ_BTIC1_LayerHeader_s BGBBTJ_BTIC1_LayerHeader;
typedef struct BGBBTJ_BTIC1_ImageHeader_s BGBBTJ_BTIC1_ImageHeader;

struct BGBBTJ_BTIC1_Context_s
{
byte *frm_css, *frm_cse;

int n_layer;
int n_image;
BGBBTJ_BTIC1_LayerHeader *layer[64];
BGBBTJ_BTIC1_ImageHeader *image[256];
// byte *ct;

BGBBTJ_BTIC1_LayerHeader *cur_layer;
BGBBTJ_BTIC1_ImageHeader *cur_img;

int imgtype;
int qflags;
};

struct BGBBTJ_BTIC1_LayerHeader_s
{
s32 layerid;		//layer ID number
s32 xorg;			//X origin of layer (relative center)
s32 yorg;			//Y origin of layer (relative center)
s32 xsize;			//X size of layer (pixels)
s32 ysize;			//Y size of layer
s32 xcenter;		//X center of layer
s32 ycenter;		//Y center of layer
u32 flags;			//layer flags

BGBBTJ_BTIC1_Context *ctx;
char *name;			//layer name
int n_image;
BGBBTJ_BTIC1_ImageHeader *image[16];	//layer images
};

struct BGBBTJ_BTIC1_ImageHeader_s
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

BGBBTJ_BTIC1_Context *ctx;			//owning context
BGBBTJ_BTIC1_LayerHeader *layer;	//owning layer
char *name;							//image component name

s32 width2;			//width of blocks
s32 height2;		//height of blocks
s32 nblocks;		//size of image in blocks

int ofs_pframe;		//offset of P-Frame base

int sz_data;		//size of image data
int sz_ldata;		//size of image data
int sz_fpdata;		//size of packed data
int sz_fpldata;		//size of packed literal data
int sz_rgba;		//size of RGBA data
byte *data;			//image data (unpacked)
byte *ldata;		//last image data (unpacked)
byte *fpdata;		//image data (filtered and packed)
byte *fpldata;		//image data (filtered and packed literal)
byte *rgba;			//image data (RGBA)
};

struct BGBBTJ_BTIC1_TagLayerHeader_Ds
{
char name[32];
btj_dword_sbe layerid;
btj_dword_sbe xorg;
btj_dword_sbe yorg;
btj_dword_sbe xsize;
btj_dword_sbe ysize;
btj_dword_sbe xcenter;
btj_dword_sbe ycenter;
btj_dword_ube flags;
};

struct BGBBTJ_BTIC1_CompLayerHeader_Ds
{
char name[8];
btj_dword_sbe layerid;
btj_dword_ube flags;
};

struct BGBBTJ_BTIC1_CompLayerID_Ds
{
btj_dword_sbe layerid;
btj_dword_ube flags;
};

struct BGBBTJ_BTIC1_ImageHeader_Ds
{
btj_dword_sbe width;
btj_dword_sbe height;
btj_word_ube imgtype;
byte mip_start;
byte mip_end;
byte filtmode;
byte clrtype;
byte pixtype;
};
