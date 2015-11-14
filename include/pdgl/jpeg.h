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

#define	PDJPG_CTY_NONE			0
#define	PDJPG_CTY_RGB			3
#define	PDJPG_CTY_RGBA			4
#define	PDJPG_CTY_RGBA_LUMA		7
#define	PDJPG_CTY_RGBA_NORM		8
#define	PDJPG_CTY_RGBA_NVSP		12
#define	PDJPG_CTY_RGBA_NVSPLU	16

typedef struct PDJPG_Context_s PDJPG_Context;

struct PDJPG_Context_s
{
//Huffman
ushort huff_code[8*256];	//huffman code ts
u16 huff_mask[8*256];	//huffman code masks
u16 huff_next[8*256];	//huffman code next
byte huff_len[8*256];
u16 huff_idx[8*256];	//huffman code masks

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

int xs, ys;
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
byte *jpg_sibuf[4];

int jpg_schs[4];		//scan horizontal size
int jpg_scvs[4];		//scan vertical size
// float jpg_schsc[4];		//scan horizontal scale
// float jpg_scvsc[4];		//scan vertical scale

int jpg_schsci[4];		//scan horizontal scale
int jpg_scvsci[4];		//scan vertical scale

byte *jpg_imgbuf;		//image buffer

int jpg_is420;

//Encoder Specific
int jpg_mono;
int jpg_tabcacheframe;	//cache table frame count.

byte *yb;
byte *ub;
byte *vb;
short *ydb;
short *udb;
short *vdb;
int lxs;
int lys;

int dcs[256];
int acs[256];
int dcsuv[256];
int acsuv[256];
};
