#define	PDJPG_RGBA		0
#define	PDJPG_RGB		1
#define	PDJPG_BGRA		3
#define	PDJPG_BGR		4
#define	PDJPG_YUVA		5
#define	PDJPG_YUV		6
#define	PDJPG_Y			7
#define	PDJPG_YA		8
#define	PDJPG_YUV422	9	//YYUV
#define	PDJPG_YUV420	10	//YYU/YYV

#define	PDJPG_YYYA		11

#define	PDJPG_CLRS_YCBCR		0
#define	PDJPG_CLRS_ORCT			1
#define	PDJPG_CLRS_RGB			2
#define	PDJPG_CLRS_YCCK			3

#define	PDJPG_QFL_RDCT			256
#define	PDJPG_QFL_444			512
#define	PDJPG_QFL_ORCT			1024
#define	PDJPG_QFL_RGB			2048

#define	PDJPG_QFL_LOSSLESS		(PDJPG_QFL_RDCT|PDJPG_QFL_444| \
									PDJPG_QFL_ORCT)

#define	PDJPG_BCSFL_RDCT		1

#define	PDJPG_CTY_NONE			0
#define	PDJPG_CTY_RGB			3
#define	PDJPG_CTY_RGBA			4
#define	PDJPG_CTY_RGBA_LUMA		7
#define	PDJPG_CTY_RGBA_NORM		8
#define	PDJPG_CTY_RGBA_NVSP		12
#define	PDJPG_CTY_RGBA_NVSPLU	16

typedef struct PDJPG_Context_s PDJPG_Context;

typedef struct PDBTV0_Context_s PDBTV0_Context;

struct PDJPG_Context_s
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
byte jpg_rdct;			//image uses RDCT
byte jpg_clrtrans;		//color space transform
byte jpg_yuvw;			//image uses YUVW

u16 bcst_version;
u32 bcst_flags;
byte bcst_trans;
byte bcst_bpp;

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
};

struct PDBTV0_Context_s
{
byte *oldlayer[8];
};
