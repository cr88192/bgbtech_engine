#ifndef FOURCC
#define FOURCC(a, b, c, d) ((a)|((b)<<8)|((c)<<16)|((d)<<24))
#endif

#ifndef TWOCC
#define TWOCC(a, b) ((a)|((b)<<8))
#endif

#ifndef TWOCC_A
#define TWOCC_A(t) ((a)&0xFF)
#endif

#ifndef TWOCC_B
#define TWOCC_B(t) (((a)>>8)&0xFF)
#endif

#ifndef FOURCC_ABCD
#define FOURCC_ABCD
#define FOURCC_A(t) ((a)&0xFF)
#define FOURCC_B(t) (((a)>>8)&0xFF)
#define FOURCC_C(t) (((a)>>16)&0xFF)
#define FOURCC_D(t) (((a)>>24)&0xFF)
#endif

#define BTIC2B_DCTSZ	4
#define BTIC2B_DCTSZ2	16

typedef struct BTIC2B_ImageContext_s BTIC2B_ImageContext;

struct BTIC2B_ImageContext_s
{
//Huffman
u16 huff_code[8][256];	//huffman code bits
u16 huff_mask[8][256];	//huffman code masks
s16 huff_next[8][256];	//huffman code next
byte huff_len[8][256];	//huffman code length
s16 huff_idx[8][256];	//huffman code masks

byte *cs;			//current input position
byte *css;			//current input start
byte *cse;			//current input end

byte *ct;			//current output position
byte *cts;			//current output start
byte *cte;			//current output end

uint huff_win;			//bitstream window
int huff_pos;			//bitstream offset
int huff_isend;			//bitstream has broken (decoder)

byte *huff_stk_ct[16];		//current output position
byte *huff_stk_cts[16];		//current output start
byte *huff_stk_cte[16];		//current output end
byte huff_stk_pos;

//Common

byte qtab[16][256];		//quantization tables
int qtabfp[16][256];	//quantization tables (fixed point)

//Decoder Specific

int xs, ys;				//image size (raw pixels)
int xs2, ys2;			//image size (Y plane)
int xs3, ys3;			//image size (UV planes)
int xs2b, ys2b;			//image size (Y plane blocks)
int xs3b, ys3b;			//image size (UV plane blocks)

//image header components
byte ihc_cid[8];		//component ID
byte ihc_qid[8];		//quantization table
byte ihc_ch[8];			//Horizontal sample factor
byte ihc_cv[8];			//Vertical sample factor
byte ihc_cdt[8];		//DC Huffman Table
byte ihc_cat[8];		//AC Huffman Table
byte ihc_cbt[8];		//Block Type
byte ihc_cbs[8];		//Block Size

//calculated block-size
short ihc_pcbs[8];		//Block Size (Pixels)
short ihc_pcbs2[8];		//Block Size (Pixels^2)
byte ihc_pcbsh[8];		//Block Size Shift (Pixels)
byte ihc_pcbsh2[8];		//Block Size Shift (Pixels^2)

//calculated fields
int ihc_cxi[8];			//Component Image X Size
int ihc_cyi[8];			//Component Image Y Size
int ihc_cxib[8];		//Component Image X Size (Blocks)
int ihc_cyib[8];		//Component Image Y Size (Blocks)
int ihc_cbh[8];			//Component Blocks Horizontal
int ihc_cbv[8];			//Component Blocks Vertical
int ihc_cidx[8];		//Coefficient Index
int ihc_nc;				//Num Components
int ihc_chm;			//Max Horizontal Sampling
int ihc_cvm;			//Max Vertical Sampling

int im_mbxs;			//Macroblock X Size
int im_mbys;			//Macroblock Y Size
int im_imxs;			//Image Macroblock X Size
int im_imys;			//Image Macroblock Y Size

// int im_scid[8];
// int im_scn[8];
s32 *im_scbuf[8];		//DCT block buffer (normal)
s32 *im_sqbuf[8];		//DCT block buffer (quantized, zigzag)
s32 *im_sibuf[8];		//pixel block buffer
s32 *im_sdcbuf[8];		//DC buffer

int ihc_schs[8];		//scan horizontal size
int ihc_scvs[8];		//scan vertical size
int ihc_schsci[8];		//scan horizontal scale
int ihc_scvsci[8];		//scan vertical scale

byte *im_imgbuf;		//image buffer

byte is_420;			//image is 420
byte is_444;			//image is 444
byte is_410;			//image is 410
byte is_rdct;			//image uses RDCT
byte is_yuvw;			//image uses YUVW
byte clrtrans;		//color space transform

int imgtype;
u32 flags;
byte trans;
byte bpp;
byte pixtype;
int org_x;
int org_y;

int min_x;
int min_y;
int max_x;
int max_y;

//Encoder Specific
byte is_mono;
byte is_alpha;

// s16 *yb;		//Y pixel buffer
// s16 *ub;		//U pixel buffer
// s16 *vb;		//V pixel buffer
// s16 *wb;		//W pixel buffer
// s16 *ab;		//A pixel buffer

// s32 *ydb;		//Y DCT block buffer (raw)
// s32 *udb;		//U DCT block buffer (raw)
// s32 *vdb;		//V DCT block buffer (raw)
// s32 *wdb;		//W DCT block buffer (raw)
// s32 *adb;		//A DCT block buffer (raw)

// s32 *ydc;		//Y DCT block buffer (quantized, zigzag)
// s32 *udc;		//U DCT block buffer (quantized, zigzag)
// s32 *vdc;		//V DCT block buffer (quantized, zigzag)
// s32 *wdc;		//W DCT block buffer (quantized, zigzag)
// s32 *adc;		//A DCT block buffer (quantized, zigzag)

int lxs;
int lys;

int dcs[256];
int acs[256];
int dcsuv[256];
int acsuv[256];
};
