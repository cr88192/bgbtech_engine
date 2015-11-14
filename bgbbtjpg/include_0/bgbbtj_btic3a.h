#define BTIC3A_CLRFMT_RGBA8		1
#define BTIC3A_CLRFMT_RGBA565	2
#define BTIC3A_CLRFMT_RGBA16	3

#define BTIC3A_BFMT_ALPHABLOCK_MASK 0x00000F00
#define BTIC3A_BFMT_COLORBLOCK_MASK 0x0000F000
#define BTIC3A_BFMT_COLORDELTA_MASK 0x00FF0000

typedef struct BTIC3A_BitstreamCtx_s BTIC3A_BitstreamCtx;
typedef struct BTIC3A_ImageCtx_s BTIC3A_ImageCtx;

typedef struct
{
int bfmt;			//block format, bits 8-13: alphablock
u16 colorA[4];		//absolute color A
u16 colorB[4];		//absolute color B
byte dclrA[4];		//delta color A
byte dclrB[4];		//delta color B

byte colorBits[8];	//color indices, max 1-4 bits, 4-bits each
byte alphaBits[8];	//alpha indices, max 1-4 bits
}BTIC3A_EncoderBlock;


struct BTIC3A_BitstreamCtx_s
{
//Huffman
u16 huff_code[16*256];	//huffman code bits
u16 huff_mask[16*256];	//huffman code masks
u16 huff_next[16*256];	//huffman code next
byte huff_len[16*256];	//huffman code length
u16 huff_idx[16*256];	//huffman code masks

byte *huff_cs;			//current pos in bitstream (input)
byte *huff_ct;			//current pos in bitstream (output)
byte *huff_cse;			//current pos in bitstream (input)
byte *huff_cte;			//current pos in bitstream (output)
uint huff_win;			//bitstream window
int huff_pos;			//bitstream offset
int huff_isend;			//bitstream has broken (decoder)

int mv_rkx;				//Motion Vector Rk X
int mv_rky;				//Motion Vector Rk Y
};

struct BTIC3A_ImageCtx_s
{
BTIC3A_BitstreamCtx *bits;		//bitstream
int xs, ys;						//image size (pixels)
int xsb, ysb;					//image size (blocks)
int xsmb, ysmb;					//image size (macroblocks)
int ysmb_mip;					//logical Y size accounting for mip.
int ysmb_miplane;				//logical Y size accounting for mip and plane.

u16 clra[4];
u16 clrb[4];
u16 clrsc[4];
int clrfmt;						//color format
int imgtype;
int clrtype;
byte mipstrt;
byte mipend;
byte imgplane;
int imgflags;

byte *blks;						//block buffer
byte *lblks;					//last frame block buffer

// int sc_rky;				//Scale Vector Rk Y
// int sc_rka;				//Scale Vector Rk A

// u16 *enc_cmdbuf;
// int enc_szCmdbuf
// int enc_mszCmdbuf

BTIC3A_EncoderBlock *encBlks;
};
