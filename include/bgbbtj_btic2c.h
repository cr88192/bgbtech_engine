#define BTIC2C_DCTSZ	8		//DCT Size
#define BTIC2C_DCTSZ2	64		//DCT Size Squared

#define BTIC2C_DCTSZ1_2	4		//1/2 DCT Size
#define BTIC2C_DCTSZ1_4	2		//1/4 DCT Size

#define BTIC2C_CLRS_YCBCR	0
#define BTIC2C_CLRS_RCT		1
#define BTIC2C_CLRS_YCGCO	2

#define BTIC2C_BLKT_DCT		0
#define BTIC2C_BLKT_WHT		1
#define BTIC2C_BLKT_RDCT	2

#define BTIC2C_BLKM_REPLACE		0
#define BTIC2C_BLKM_ADD			1
#define BTIC2C_BLKM_MOVEADD		2
#define BTIC2C_BLKM_SKIP		3
#define BTIC2C_BLKM_MOVESKIP	4

// typedef struct BTIC2C_Context_s BTIC2C_Context;

struct BTIC2C_Context_s
{
//Huffman
u16 huff_code[16*256];	//huffman code bits
u16 huff_mask[16*256];	//huffman code masks
u16 huff_next[16*256];	//huffman code next
byte huff_len[16*256];	//huffman code length
u16 huff_idx[16*256];	//huffman code masks
// u16 huff_idx12[8*4096];	//12-bit huffman code masks

byte *huff_cs;			//current pos in bitstream (input)
byte *huff_ct;			//current pos in bitstream (output)
uint huff_win;			//bitstream window
int huff_pos;			//bitstream offset
int huff_isend;			//bitstream has broken (decoder)

int (*Huff_NextByte)(BTIC2C_Context *ctx);
void (*Huff_EmitByte)(BTIC2C_Context *ctx, int i);

//Arithmetic Coder

byte *ari_cs;			//current pos in bitstream (input)
byte *ari_ct;			//current pos in bitstream (output)
byte *ari_cse;			//current end pos in bitstream (input)
byte *ari_cte;			//current end pos in bitstream (output)

u32 ari_rmin;			//window lower range
u32 ari_rmax;			//window upper range
u32 ari_rval;			//window decode value

int ari_wctx;			//window context
byte *ari_model;		//probability model

int ari_ctxbits;		//context bits
int ari_ctxmask;		//context mask

//Common

u16 img_qt[4][64];		//quantization tables
int img_qtfp[4][64];	//quantization tables (fixed point)

//Decoder Specific

int xs, ys;				//image size (pixels)
int xs2, ys2;			//image size (Y plane)
int xs3, ys3;			//image size (UV plane)
int wmb, hmb;			//image size (in macroblocks)
int mbxs, mbys;			//macroblock size (pixels)
int qfl;				//quality and flags
int clrs;				//image pixel transform

byte img_cid[16];		//component IDs
byte img_ch[16];		//component horizontal factor
byte img_cv[16];		//component vertical factor
byte img_qid[16];		//component quantization table
int img_cxi[16];		//component image X size
int img_cyi[16];		//component image Y size
int img_nc;				//num components
int img_chm;			//max horizontal sampling
int img_chn;			//max vertical sampling

int img_scid[4];
int img_scn[4];
s32 *img_scbuf[4];		//image blocks buffers
s32 *img_sibuf[4];		//image plane (16/24 bits)
s16 *img_sjbuf[4];		//image plane (8/12 bits)
s32 *img_lsibuf[4];		//image plane (16/24 bits)
s16 *img_lsjbuf[4];		//image plane (8/12 bits)

int img_schs[4];		//scan horizontal size
int img_scvs[4];		//scan vertical size
// float img_schsc[4];		//scan horizontal scale
// float img_scvsc[4];		//scan vertical scale

int img_schsci[4];		//scan horizontal scale (fixed point)
int img_scvsci[4];		//scan vertical scale (fixed point)

byte img_cdt[4];		//coded DC Table
byte img_cat[4];		//codec AC Table
byte img_cd2t[4];		//coded DC Table 2
byte img_ca2t[4];		//coded AC Table 2
// byte img_csn[4];
byte img_ns;
byte img_pbits;
byte img_pclrs;

// byte *img_imgbuf;		//image buffer

byte img_is420;			//image is 420
byte img_is444;			//image is 444
// byte img_rdct;			//image uses RDCT
byte img_clrtrans;		//color space transform
byte img_blktrans;		//block transform

u16 bcst_version;
u32 bcst_flags;
byte bcst_trans;
byte bcst_bpp;

int skip_cnt;			//skip macroblocks count
byte *skip_map;			//bitmap of skipped blocks
byte blk_mode[8];		//current block mode
s16 blk_move[16];		//current block movement

// u16 img_app14_dctversion;
// u16 img_app14_flags0;
// u16 img_app14_flags1;
// byte img_app14_trans;

//Encoder Specific
bool img_mono;
byte img_tabcacheframe;		//cache table frame count.
bool img_alpha;
bool flip;

s32 *yb, *ub, *vb, *ab;			//YUVA plane buffers
s32 *lyb, *lub, *lvb, *lab;		//last YUVA plane buffers
s32 *ydb, *udb, *vdb, *adb;		//YUVA block buffers
byte *ybm, *ubm, *vbm, *abm;	//YUVA block modes (Mode, ?, X, Y)
int lxs;
int lys;

//current symbol stats
int dcsy[256];
int acsy[256];
int acs2y[256];
int dcsuv[256];
int acsuv[256];
int acs2uv[256];
int dcsa[256];
int acsa[256];
int acs2a[256];

//stats for various uses of bits per-table
int cnt_hsn[16];	//per-table symbol counts
int cnt_hsb[16];	//per-table bit counts
int cnt_hdn[16];	//per-table coeff counts
int cnt_hdb[16];	//per-table coeff extra-bit counts
int cnt_hln[16];	//per-table len counts
int cnt_hlb[16];	//per-table len extra-bit counts
int cnt_hvn[16];	//per-table vector counts
int cnt_hvb[16];	//per-table vector extra-bit counts

//aggregate P-frame counts
int apcnt_hsn[16];	//per-table symbol counts
int apcnt_hsb[16];	//per-table bit counts
int apcnt_hdn[16];	//per-table coeff counts
int apcnt_hdb[16];	//per-table coeff extra-bit counts
int apcnt_hln[16];	//per-table len counts
int apcnt_hlb[16];	//per-table len extra-bit counts
int apcnt_hvn[16];	//per-table vector counts
int apcnt_hvb[16];	//per-table vector extra-bit counts
};
