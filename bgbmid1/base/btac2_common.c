#include <bgbmid.h>

/*
BTAC2 (0.2.0)
or: BTAC2A

Partially redesigned BTAC, intended for a similar use case, but hopefully 
increasing the audio quality for the bitrate.

Will use CBR and power-of-2 sample counts for blocks, but will not require 
the use of power-of-2 sizes for encoded blocks. Both parameters will be 
variable.

Like in BTAC, each block will self-identify its format via a 4-bit tag.
Unlike BTAC, blocks will use entropy-coded contents.


Bitstream:
Will use LSB-first bit-packing.
Values will be Huffman coded.

Coded Block:
	4 bits: block type (0)
	4 bits: Table-Index Index
	8 bits: Quantization Factor
	Center VLC-Block
	[ Side VLC Block ]
	
Will use Huffman tables 0/1 for Center, and 2/3 for Side.
Center will use Quantization Table 0, and Side will use Quantization Table 1.
The blocks will use the WHT (Walsh Hadamard Transform).
Center will be Left+Right/2, and Side will be Left-Center.

	
VLC:
Will use a VLC scheme similar to that of JPEG, where the high bits will encode a zero-prefix count, and the low-bits will encode the coefficient.

AC Will use a Z3.V5 scheme, with Z:
Z=0-6: Zero Count, V=AC Value (VLCA)
Z=7: Escape, V=Zero Count (VLCB)
	T2.W6: T=Tag, W=Value
		T=0, W=AC Value (VLCA)
		T=1, W=Command (VLCB)

The DC coefficient will use T2.V6:
T=0: W=DC Value
T=1: W=Command (VLCB)

VLCA (Prefix, Range, Extra):
 0- 3,   0-  3, 0
 4/ 5,   4-  7, 1
 6/ 7,   8- 15, 2
 8/ 9,  16- 31, 3
10/11,  32- 63, 4
12/13,  64-127, 5
14/15, 128-255, 6
16/17, 256-511, 7
...

VLCB (Prefix, Range, Extra):
 0- 7,   0-  7, 0
 8-11,   8- 15, 1
12-15,  16- 31, 2
16-19,  32- 63, 3
20-23,  64-127, 4
24-27, 128-255, 5
28-31, 256-511, 6
...

DC and AC coefficients will be sign-folded.
0, -1, 1, -2, 2, ...


TLV:
Used for header data.

len:WORD, tag:TWOCC
0:WORD, tag:TWOCC, len:DWORD
1:WORD, len:WORD, tag:FOURCC
2:WORD, flags:WORD, len:DWORD, tag:FOURCC
3:WORD, flags:WORD, len:QWORD, tag:FOURCC

Main TLV:

'HEAD', BTAC2 Headers
'DATA', BTAC2 Data

Headers:

HD: Main Header
	magic:BYTE[6]="BTAC2A"
	version:WORD	//format version, 0x0000
	blklen:WORD		//block length (samples)
	blksz:WORD		//block size (bytes)
	flags:DWORD		//encoding flags
	_pad:WORD;		//pad, must be 0
	chan:BYTE;		//channel count
	bits:BYTE;		//sample bits
	rate:DWORD;		//sample rate
	length:DWORD;	//sample length
	
Flags:
	BGBTAC_EFL_16BIT		1		//Sample is 16 bit
	BGBTAC_EFL_UNSIGNED		2		//Sample is unsigned
	BGBTAC_EFL_LOOP			4		//Sample should loop
	BGBTAC_EFL_BIDIRLOOP	8		//Sample loops back and forth
	BGBTAC_EFL_REVERSE		16		//Sample plays in reverse
	-
	BGBTAC_EFL_STEREO		256		//Sample is stereo
	BGBTAC_EFL_CENTERMONO	512		//Encoded as a center mono channel
	BGBTAC_EFL_HALFMONO		1024	//Sample uses half-mono
	BGBTAC_EFL_MIDSTREAM	2048	//(Internal)
	BGBTAC_EFL_MIPMAP		4096	//(Internal)

If STEREO is set, then the sample encodes both a left and right channels, otherwise it is mono. For stereo, CENTERMONO indicates that it is encoded as if it were mono, with a simple center channel. HALFMONO indicates split encoding for the left/right channels, rather than joint-stereo, with each channel being given its own coded block.

If STEREO is clear, then only a single channel is encoded. HALFMONO indicates that we are only encoding a left or right channel, in which case CENTERMONO indicates which channel we are encoding (left if clear, right if set).


H0-H9: Huffman Tables 0-9
Ha-Hz: Huffman Tables 10-35

4 bits: Table Type
	0=Fixed, 1=Dynamic, 2-15=Reserved
4 bits: Rice Factor

Huffman-Table symbols are encoded with a permutation:
	16, 17, 18, 0, 8,7, 9,6, 10,5, 11,4, 12,3, 13,2, 14,1, 15, 19

Where symbol values are as such:
	0: Non-Coded Symbol
	1-16: Symbol Code Length
	17: +3 Bits, encode a run of the prior length.
	18: +7 Bits, encode a run of the prior length.
	19: End of Table

Q0-Q9: Quantization Tables 0-9
Qa-Qz: Quantization Tables 10-35

Consist of min/max pairs of quantization tables (these are interpolated per-block).

4 bits: Table Size (log2-4)
	Gives the number of coefficients in the table, where:
	0=16, 1=32, 2=64, 3=128, 4=256, ...
4 bits: Rice Factor

This is followed by the min and max quantization tables.
Each table consists of an collection of signed Rice-coded values, where values >0 are literal values, and values <0 indicate runs of the prior value.

'TX': Table Index Table

Consists of an array of 64-bit index descriptors:
R0(4) H0(6) H1(6) H2(6) H3(6) Q0(6) Q1(6) PAD(24)


Top Level File Header:
magic:BYTE[8]="BTAC2A\r\n";		//magic sequence
size:DWORD						//file payload size
check:DWORD;					//Adler32 checksum (optional)
... TLV Data ...

*/

const u32 btac2_dbase[64]={
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
		0x40000000, 0x60000000, 0x80000000, 0xC0000000
		};

const int btac2_dextra[64]={
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

const int btac2_lbase[64]={
		  0,   1,   2,   3,   4,   5,   6,   7,
		  8,  10,  12,  14,  16,  20,  24,  28,
		 32,  40,  48,  56,  64,  80,  96, 112,
		128, 160, 192, 224, 256, 320, 384, 448,

		0x000200, 0x000280, 0x000300, 0x000380,
		0x000400, 0x000500, 0x000600, 0x000700,
		0x000800, 0x000A00, 0x000C00, 0x000E00,
		0x001000, 0x001400, 0x001800, 0x001C00,
		0x002000, 0x002800, 0x003000, 0x003800,
		0x004000, 0x005000, 0x006000, 0x007000,
		0x008000, 0x00A000, 0x00C000, 0x00E000,
		0x010000, 0x014000, 0x018000, 0x01C000
		};

const int btac2_lextra[64]={
		 0,  0,  0,  0,  0,  0,  0,  0,  1, 1, 1, 1,  2,  2,  2,  2,
		 3,  3,  3,  3,  4,  4,  4,  4,  5, 5, 5, 5,  6,  6,  6,  6,
		 7,  7,  7,  7,  8,  8,  8,  8,  9, 9, 9, 9, 10, 10, 10, 10,
		11, 11, 11, 11, 12, 12, 12, 12, 13,13,13,13, 14, 14, 14, 14};

const u64 btac2_ibase[]={
	//16 bit range
	0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007,
	0x0008, 0x000A, 0x000C, 0x000E,
	
	0x0010, 0x0014, 0x0018, 0x001C, 0x0020, 0x0028, 0x0030, 0x0038,
	0x0040, 0x0050, 0x0060, 0x0070, 0x0080, 0x00A0, 0x00C0, 0x00E0,
	0x0100, 0x0140, 0x0180, 0x01C0, 0x0200, 0x0280, 0x0300, 0x0380,
	0x0400, 0x0500, 0x0600, 0x0700, 0x0800, 0x0A00, 0x0C00, 0x0E00,
	0x1000, 0x1400, 0x1800, 0x1C00, 0x2000, 0x2800, 0x3000, 0x3800,
	0x4000, 0x5000, 0x6000, 0x7000, 0x8000, 0xA000, 0xC000, 0xE000,
	
	//32-bit range
	0x00010000, 0x00014000, 0x00018000, 0x0001C000,
	0x00020000, 0x00028000, 0x00030000, 0x00038000,
	0x00040000, 0x00050000, 0x00060000, 0x00070000,
	0x00080000, 0x000A0000, 0x000C0000, 0x000E0000,

	0x00100000, 0x00140000, 0x00180000, 0x001C0000,
	0x00200000, 0x00280000, 0x00300000, 0x00380000,
	0x00400000, 0x00500000, 0x00600000, 0x00700000,
	0x00800000, 0x00A00000, 0x00C00000, 0x00E00000,

	0x01000000, 0x01400000, 0x01800000, 0x01C00000,
	0x02000000, 0x02800000, 0x03000000, 0x03800000,
	0x04000000, 0x05000000, 0x06000000, 0x07000000,
	0x08000000, 0x0A000000, 0x0C000000, 0x0E000000,

	0x10000000, 0x14000000, 0x18000000, 0x1C000000,
	0x20000000, 0x28000000, 0x30000000, 0x38000000,
	0x40000000, 0x50000000, 0x60000000, 0x70000000,
	0x80000000, 0xA0000000, 0xC0000000, 0xE0000000,

	//64 bit range
	0x0000000100000000, 0x0000000140000000,
	0x0000000180000000, 0x00000001C0000000,
	0x0000000200000000, 0x0000000280000000,
	0x0000000300000000, 0x0000000380000000,
	0x0000000400000000, 0x0000000500000000,
	0x0000000600000000, 0x0000000700000000,
	0x0000000800000000, 0x0000000A00000000,
	0x0000000C00000000, 0x0000000E00000000,

	0x0000001000000000, 0x0000001400000000,
	0x0000001800000000, 0x0000001C00000000,
	0x0000002000000000, 0x0000002800000000,
	0x0000003000000000, 0x0000003800000000,
	0x0000004000000000, 0x0000005000000000,
	0x0000006000000000, 0x0000007000000000,
	0x0000008000000000, 0x000000A000000000,
	0x000000C000000000, 0x000000E000000000,

	0x0000010000000000, 0x0000014000000000,
	0x0000018000000000, 0x000001C000000000,
	0x0000020000000000, 0x0000028000000000,
	0x0000030000000000, 0x0000038000000000,
	0x0000040000000000, 0x0000050000000000,
	0x0000060000000000, 0x0000070000000000,
	0x0000080000000000, 0x00000A0000000000,
	0x00000C0000000000, 0x00000E0000000000,

	0x0000100000000000, 0x0000140000000000,
	0x0000180000000000, 0x00001C0000000000,
	0x0000200000000000, 0x0000280000000000,
	0x0000300000000000, 0x0000380000000000,
	0x0000400000000000, 0x0000500000000000,
	0x0000600000000000, 0x0000700000000000,
	0x0000800000000000, 0x0000A00000000000,
	0x0000C00000000000, 0x0000E00000000000,

	0x0001000000000000, 0x0001400000000000,
	0x0001800000000000, 0x0001C00000000000,
	0x0002000000000000, 0x0002800000000000,
	0x0003000000000000, 0x0003800000000000,
	0x0004000000000000, 0x0005000000000000,
	0x0006000000000000, 0x0007000000000000,
	0x0008000000000000, 0x000A000000000000,
	0x000C000000000000, 0x000E000000000000,

	0x0010000000000000, 0x0014000000000000,
	0x0018000000000000, 0x001C000000000000,
	0x0020000000000000, 0x0028000000000000,
	0x0030000000000000, 0x0038000000000000,
	0x0040000000000000, 0x0050000000000000,
	0x0060000000000000, 0x0070000000000000,
	0x0080000000000000, 0x00A0000000000000,
	0x00C0000000000000, 0x00E0000000000000,

	0x0100000000000000, 0x0140000000000000,
	0x0180000000000000, 0x01C0000000000000,
	0x0200000000000000, 0x0280000000000000,
	0x0300000000000000, 0x0380000000000000,
	0x0400000000000000, 0x0500000000000000,
	0x0600000000000000, 0x0700000000000000,
	0x0800000000000000, 0x0A00000000000000,
	0x0C00000000000000, 0x0E00000000000000,

	0x1000000000000000, 0x1400000000000000,
	0x1800000000000000, 0x1C00000000000000,
	0x2000000000000000, 0x2800000000000000,
	0x3000000000000000, 0x3800000000000000,
	0x4000000000000000, 0x5000000000000000,
	0x6000000000000000, 0x7000000000000000,
	0x8000000000000000LL, 0xA000000000000000LL,
	0xC000000000000000LL, 0xE000000000000000LL,
	};

const int btac2_iextra[]={
		 0,  0,  0,  0,   0,  0,  0,  0,
		 1,  1,  1,  1,
		 2,  2,  2,  2,   3,  3,  3,  3,
		 4,  4,  4,  4,   5,  5,  5,  5,
		 6,  6,  6,  6,   7,  7,  7,  7,
		 8,  8,  8,  8,   9,  9,  9,  9,
		10, 10, 10, 10,  11, 11, 11, 11,
		12, 12, 12, 12,  13, 13, 13, 13,
		
		//32 bit range
		14, 14, 14, 14,  15, 15, 15, 15,
		16, 16, 16, 16,  17, 17, 17, 17,

		18, 18, 18, 18,  19, 19, 19, 19,
		20, 20, 20, 20,  21, 21, 21, 21,

		22, 22, 22, 22,  23, 23, 23, 23,
		24, 24, 24, 24,  25, 25, 25, 25,

		26, 26, 26, 26,  27, 27, 27, 27,
		28, 28, 28, 28,  29, 29, 29, 29,
		
		//64 bit range
		30, 30, 30, 30,  31, 31, 31, 31,
		32, 32, 32, 32,  33, 33, 33, 33,

		34, 34, 34, 34,  35, 35, 35, 35,
		36, 36, 36, 36,  37, 37, 37, 37,

		38, 38, 38, 38,  39, 39, 39, 39,
		40, 40, 40, 40,  41, 41, 41, 41,

		42, 42, 42, 42,  43, 43, 43, 43,
		44, 44, 44, 44,  45, 45, 45, 45,

		46, 46, 46, 46,  47, 47, 47, 47,
		48, 48, 48, 48,  49, 49, 49, 49,

		50, 50, 50, 50,  51, 51, 51, 51,
		52, 52, 52, 52,  53, 53, 53, 53,

		54, 54, 54, 54,  55, 55, 55, 55,
		56, 56, 56, 56,  57, 57, 57, 57,

		58, 58, 58, 58,  59, 59, 59, 59,
		60, 60, 60, 60,  61, 61, 61, 61,
	};


const int btac2_lorder[]={
	16, 17, 18, 0, 8,7, 9,6, 10,5, 11,4, 12,3, 13,2, 14,1, 15, 19};
const int btac2_lorder2[]={
	3, 17, 15, 13, 11,9, 7,5, 4,6, 8,10, 12,14, 16,18, 0,1, 2, 19};


void BTAC2_Common_TransformWHT(int *iblk, int *oblk, int cnt, int dc)
{
	int tblk[1024];
	int i0, i1, i2;
	int i, j, k, n;

	n=cnt>>1;
	for(i=0; i<n; i++)
	{
		i0=iblk[i*2+0]-dc;
		i1=iblk[i*2+1]-dc;
		i2=i0+i1;
		tblk[i]=i2;
		oblk[n+i]=(i2>>1)-i1;
	}

	n=n>>1;
	while(n>1)
	{
		for(i=0; i<n; i++)
		{
			i0=tblk[i*2+0]; i1=tblk[i*2+1]; i2=i0+i1;
			tblk[i]=i2;	oblk[n+i]=(i2>>1)-i1;
		}
		n=n>>1;
	}

	i0=tblk[0]; i1=tblk[1]; i2=i0+i1;
	oblk[0]=i2; oblk[1]=(i2>>1)-i1;
}

void BTAC2_Common_TransformIWHT(int *iblk, int *oblk, int cnt, int dc)
{
	int tba[1024];
	int tbb[1024];
	int i0, i1, i2, i3;
	int i, j, k, n;
	
	tba[0]=iblk[0];
	n=1;
	while(n<cnt)
	{
		for(i=0; i<n; i++)
		{
			i0=tba[i];
			i1=iblk[n+i];
			i3=(i0>>1)-i1;
			i2=i0-i3;
			tbb[i*2+0]=i2;
			tbb[i*2+1]=i3;
		}
		
		n=n<<1;
		for(i=0; i<n; i++)
		{
			tba[i]=tbb[i];
		}
	}

	for(i=0; i<n; i++)
	{
		oblk[i]=tba[i]+dc;
	}
}

void BTAC2_Common_QuantWHT(BGBMID_BTAC2_Context *ctx,
	int *iblk, int *oblk,
	int blen, int qtab, int qf)
{
	int s0, sm, sn;
	int i, j, k;
	
	for(i=0; i<blen; i++)
	{
		sm=ctx->qtab_min[qtab][i];
		sn=ctx->qtab_max[qtab][i];
		s0=sn-sm;
		j=sm+((s0*qf+128)>>8);
//		j=sm+((s0*qf)>>8);
		if(j<sm)j=sm;
		if(j>sn)j=sn;
		
		oblk[i]=iblk[i]/j;
//		oblk[i]=(iblk[i]/(float)j)+0.5;
	}
}

void BTAC2_Common_DequantWHT(BGBMID_BTAC2_Context *ctx,
	int *iblk, int *oblk,
	int blen, int qtab, int qf)
{
	int s0, sm, sn;
	int i, j, k;
	
	for(i=0; i<blen; i++)
	{
		sm=ctx->qtab_min[qtab][i];
		sn=ctx->qtab_max[qtab][i];
		s0=sn-sm;
		j=sm+((s0*qf+128)>>8);
//		j=sm+((s0*qf)>>8);
		if(j<sm)j=sm;
		if(j>sn)j=sn;
		
		oblk[i]=iblk[i]*j;
	}
}

BGBMID_BTAC2_Context *BTAC2_Common_AllocContext()
{
	BGBMID_BTAC2_Context *tmp;
	
	tmp=gctalloc("bgbmid_btac2_context_t", sizeof(BGBMID_BTAC2_Context));
	return(tmp);
}

static int trans8[256]={
0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0,
0x10, 0x90, 0x50, 0xD0, 0x30, 0xB0, 0x70, 0xF0,
0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8,
0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8, 0x78, 0xF8,
0x04, 0x84, 0x44, 0xC4, 0x24, 0xA4, 0x64, 0xE4,
0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4,
0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC,
0x1C, 0x9C, 0x5C, 0xDC, 0x3C, 0xBC, 0x7C, 0xFC,
0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2,
0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2,
0x0A, 0x8A, 0x4A, 0xCA, 0x2A, 0xAA, 0x6A, 0xEA,
0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA,
0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6,
0x16, 0x96, 0x56, 0xD6, 0x36, 0xB6, 0x76, 0xF6,
0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE,
0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE,
0x01, 0x81, 0x41, 0xC1, 0x21, 0xA1, 0x61, 0xE1,
0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1,
0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9,
0x19, 0x99, 0x59, 0xD9, 0x39, 0xB9, 0x79, 0xF9,
0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5,
0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5,
0x0D, 0x8D, 0x4D, 0xCD, 0x2D, 0xAD, 0x6D, 0xED,
0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD,
0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3,
0x13, 0x93, 0x53, 0xD3, 0x33, 0xB3, 0x73, 0xF3,
0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB,
0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB,
0x07, 0x87, 0x47, 0xC7, 0x27, 0xA7, 0x67, 0xE7,
0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7,
0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF,
0x1F, 0x9F, 0x5F, 0xDF, 0x3F, 0xBF, 0x7F, 0xFF
};

int BTAC2_Common_TransposeByte(int v)
{
	return(trans8[v]);
}

int BTAC2_Common_TransposeWord(int v)
{
	int i;
	i=trans8[(v>>8)&0xFF]|(trans8[v&0xFF]<<8);
	return(i);
}

int BTAC2_Common_SetupTable(byte *cl, int ncl,
	u16 *tc, u16 *tm, byte *tl,
	u16 *ti, u16 *tn)
{
	int cnt[18], nc[18], nn[18], nf[18];
	int i, j, k, k2, l, ff;

	for(i=0; i<=16; i++) { cnt[i]=0; nc[i]=0; nn[i]=-1; nf[i]=-1; }
	for(i=0; i<256; i++)ti[i]=0xFFFF;

	for(i=0; i<ncl; i++)cnt[cl[i]]++;
	cnt[0]=0;

#if 1
	j=cl[0];
//	for(i=15; i>=1; i--)
	for(i=16; i>=1; i--)
		if(cnt[i])break;
	if(j>i)j=i;
	if(!i)return(-12);	//no codes

	j=1;
//	for(i=1; i<16; i++)
	for(i=1; i<=16; i++)
	{
		j<<=1;
		j-=cnt[i];

//		printf("%d %d %d\n", i, j, cnt[i]);
		if(j<0)return(-10);	//over subscribed
	}
//	if((j>0) && ((ncl-cnt[0])!=1))
//		return(-11);	//incomplete set
#endif

	j=0;
//	for(i=1; i<16; i++)
	for(i=1; i<=16; i++)
	{
		j=(j+cnt[i-1])<<1;
		nc[i]=j;
	}

	for(i=0; i<ncl; i++)
	{
		l=cl[i];
		if(!l)
		{
			tl[i]=0;
			continue;
		}

		tl[i]=cl[i];
		tm[i]=(1<<tl[i])-1;
		tc[i]=nc[l]++;

		if(nn[l]>=0)
		{
			tn[nn[l]]=i;
			nn[l]=i;
		}else
		{
			nf[l]=i;
			nn[l]=i;
		}
	}

	j=-1; ff=-1;
//	for(i=1; i<16; i++)
	for(i=1; i<=16; i++)
	{
		if(nf[i]>=0)
		{
			if(ff<0)ff=nf[i];
			if(j>=0)tn[j]=nf[i];
			j=nn[i];
		}
	}
	if(j>=0)tn[j]=0xFFFF;

	for(i=ff; i!=0xFFFF; i=tn[i])
	{
		l=cl[i];
		if(l<=8)
		{
			k2=tc[i]<<(8-l);
			j=1<<(8-l);
			while(j--)
			{
				k=BTAC2_Common_TransposeByte(k2++);
				if(ti[k]==0xFFFF)ti[k]=i;
			}
		}else
		{
			k2=tc[i]>>(l-8);
			k=BTAC2_Common_TransposeByte(k2);
			if(ti[k]==0xFFFF)ti[k]=i;
		}

		k2=(tc[i])<<(16-l);
		k=BTAC2_Common_TransposeWord(k2++);
		tc[i]=k;
	}

	j=0;

#if 0
	for(i=0; i<256; i++)if(ti[i]==0xFFFF)
	{
		printf("table bad index %d\n", i);
		j=-9;
	}
#endif

	return(j);
}
