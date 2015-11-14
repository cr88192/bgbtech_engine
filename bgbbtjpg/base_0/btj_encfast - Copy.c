#include <bgbbtj.h>
#include <math.h>

#define DCTSZ	8
#define DCTSZ2	64

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

char *btj_jfe_marker[]={
"SOF0", "SOF1", "SOF2", "SOF3", "DHT", "SOF5", "SOF6", "SOF7",
"JPG", "SOF9", "SOF10", "SOF11", "DAC", "SOF13", "SOF14", "SOF15",
"RST0", "RST1", "RST2", "RST3", "RST4", "RST5", "RST6", "RST7",
"SOI", "EOI", "SOS", "DQT", "DNL", "DRI", "DHP", "EXP",
"APP0", "APP1", "APP2", "APP3", "APP4", "APP5", "APP6", "APP7", 
"APP8", "APP9", "APP10", "APP11", "APP12", "APP13", "APP14", "APP15", 
"JPG0", "JPG1", "JPG2", "JPG3", "JPG4", "JPG5", "JPG6", "JPG7", 
"JPG8", "JPG9", "JPG10", "JPG11", "JPG12", "JPG13", "COM", ""
};

byte btj_jfe_zigzag[64]={
 0,  1,  5,  6, 14, 15, 27, 28,
 2,  4,  7, 13, 16, 26, 29, 42,
 3,  8, 12, 17, 25, 30, 41, 43,
 9, 11, 18, 24, 31, 40, 44, 53,
10, 19, 23, 32, 39, 45, 52, 54,
20, 22, 33, 38, 46, 51, 55, 60,
21, 34, 37, 47, 50, 56, 59, 61,
35, 36, 48, 49, 57, 58, 62, 63
};

byte btj_jfe_zigzag2[64]={
 0,  1,  8, 16,  9,  2,  3, 10,
17, 24, 32, 25, 18, 11,  4,  5,
12, 19, 26, 33, 40, 48, 41, 34,
27, 20, 13,  6,  7, 14, 21, 28,
35, 42, 49, 56, 57, 50, 43, 36,
29, 22, 15, 23, 30, 37, 44, 51,
58, 59, 52, 45, 38, 31, 39, 46,
53, 60, 61, 54, 47, 55, 62, 63
};

ushort btj_jfeh_code[4][256];

byte btj_jfeh_len[4][256]={
{  6,  4,  3,  3,  2,  2,  3,  5,  7,  9, 10, 10, 10, 10, 10, 11,
 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
},
{  4,  2,  4,  3,  4,  5,  6,  7,  8, 13, 14, 16, 16, 15, 15, 15,
15,  3,  5,  7,  9, 12, 13, 15, 15, 15, 15, 15, 15, 15, 15, 15,
15,  5,  7, 10, 13, 14, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16,
16,  5,  8, 11, 13, 16, 15, 16, 16, 16, 16, 16, 16, 16, 16, 16,
11,  6, 10, 12, 15, 16, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
11,  6, 10, 15, 14, 15, 15, 11, 11, 11, 11, 11, 11, 11, 11, 11,
11,  7, 10, 13, 15, 15, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
11,  7, 10, 15, 16, 15, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
11,  7, 11, 14, 15, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
11,  5, 13, 15, 15, 11, 11, 11, 11, 11, 11, 11, 11, 16, 16, 16,
16,  8, 12, 15, 15, 15, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
16,  9, 12, 13, 15, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
16,  9, 14, 15, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
16, 10, 14, 15, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
16, 11, 15, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 15, 15,
10, 12, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 16,
},
{  2,  2,  2,  3,  4,  5,  8,  8,  9,  9,  8,  8,  8,  8,  9, 10,
 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
},
{  3,  2,  3,  3,  5,  7, 11, 14, 15, 16, 16, 16, 16, 16, 16, 16,
16,  3,  5,  8, 11, 12, 16, 16, 15, 15, 15, 15, 15, 15, 15, 15,
15,  4,  7, 11, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
15,  5,  9, 11, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
15,  6, 10, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
15,  6, 11, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
15,  7, 13, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
15,  7, 12, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
15,  8, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
15,  8, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
15,  9, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
15,  9, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
15, 10, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
15, 10, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
15, 12, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
11, 12, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 16,
}
};

byte *btj_jfeh_cs;	//current pos in bitstream (input)
byte *btj_jfeh_ct;	//current pos in bitstream (output)
uint btj_jfeh_win;	//bitstream window
int btj_jfeh_pos;	//bitstream offset
int btj_jfeh_isend;	//bitstream has broken (decoder)

byte btj_jfe_qt[4][64];
int btj_jfe_qtfp[4][64];


//Decoder Specific

int btj_jfe_xs, btj_jfe_ys;
byte btj_jfe_cid[16];
byte btj_jfe_ch[16];
byte btj_jfe_cv[16];
byte btj_jfe_qid[16];
int btj_jfe_cxi[16];
int btj_jfe_cyi[16];
int btj_jfe_nc;
int btj_jfe_chm;
int btj_jfe_chn;

int btj_jfe_scid[4];
int btj_jfe_scn[4];
short *btj_jfe_scbuf[4];
byte *btj_jfe_sibuf[4];

//Common

#if 1
void BTJ_JFE_TransDCT_Horiz(short *iblk, int *oblk)
{
	char ib[8];

	ib[0]=iblk[0]-128;	ib[1]=iblk[1]-128;
	ib[2]=iblk[2]-128;	ib[3]=iblk[3]-128;
	ib[4]=iblk[4]-128;	ib[5]=iblk[5]-128;
	ib[6]=iblk[6]-128;	ib[7]=iblk[7]-128;

	oblk[0]=ib[0]*91  +ib[1]*91  +ib[2]*91  +ib[3]*91  
			+ib[4]*91  +ib[5]*91  +ib[6]*91  +ib[7]*91;
	oblk[1]=ib[0]*126 +ib[1]*106 +ib[2]*71  +ib[3]*25  
			-ib[4]*25  -ib[5]*71  -ib[6]*106 -ib[7]*126;
	oblk[2]=ib[0]*118 +ib[1]*49  -ib[2]*49  -ib[3]*118 
			-ib[4]*118 -ib[5]*49  +ib[6]*49  +ib[7]*118;
	oblk[3]=ib[0]*106 -ib[1]*25  -ib[2]*126 -ib[3]*71  
			+ib[4]*71  +ib[5]*126 +ib[6]*25  -ib[7]*106;
	oblk[4]=ib[0]*91  -ib[1]*91  -ib[2]*91  +ib[3]*91  
			+ib[4]*91  -ib[5]*91  -ib[6]*91  +ib[7]*91;
	oblk[5]=ib[0]*71  -ib[1]*126 +ib[2]*25  +ib[3]*106 
			-ib[4]*106 -ib[5]*25  +ib[6]*126 -ib[7]*71;
	oblk[6]=ib[0]*49  -ib[1]*118 +ib[2]*118 -ib[3]*49  
			-ib[4]*49  +ib[5]*118 -ib[6]*118 +ib[7]*49;
	oblk[7]=ib[0]*25  -ib[1]*71  +ib[2]*106 -ib[3]*126 
			+ib[4]*126 -ib[5]*106 +ib[6]*71  -ib[7]*25;
}

void BTJ_JFE_TransDCT_Vert(int *iblk, int *oblk)
{
	oblk[ 0]=iblk[0]*91  +iblk[8]*91  +iblk[16]*91  +iblk[24]*91  +iblk[32]*91  +iblk[40]*91  +iblk[48]*91  +iblk[56]*91;
	oblk[ 8]=iblk[0]*126 +iblk[8]*106 +iblk[16]*71  +iblk[24]*25  -iblk[32]*25  -iblk[40]*71  -iblk[48]*106 -iblk[56]*126;
	oblk[16]=iblk[0]*118 +iblk[8]*49  -iblk[16]*49  -iblk[24]*118 -iblk[32]*118 -iblk[40]*49  +iblk[48]*49  +iblk[56]*118;
	oblk[24]=iblk[0]*106 -iblk[8]*25  -iblk[16]*126 -iblk[24]*71  +iblk[32]*71  +iblk[40]*126 +iblk[48]*25  -iblk[56]*106;
	oblk[32]=iblk[0]*91  -iblk[8]*91  -iblk[16]*91  +iblk[24]*91  +iblk[32]*91  -iblk[40]*91  -iblk[48]*91  +iblk[56]*91;
	oblk[40]=iblk[0]*71  -iblk[8]*126 +iblk[16]*25  +iblk[24]*106 -iblk[32]*106 -iblk[40]*25  +iblk[48]*126 -iblk[56]*71;
	oblk[48]=iblk[0]*49  -iblk[8]*118 +iblk[16]*118 -iblk[24]*49  -iblk[32]*49  +iblk[40]*118 -iblk[48]*118 +iblk[56]*49;
	oblk[56]=iblk[0]*25  -iblk[8]*71  +iblk[16]*106 -iblk[24]*126 +iblk[32]*126 -iblk[40]*106 +iblk[48]*71  -iblk[56]*25;
}

void BTJ_JFE_TransDCT(short *iblk, short *oblk)
{
	int s[DCTSZ2];
	int t[DCTSZ2];
	int i, j;

	BTJ_JFE_TransDCT_Horiz(iblk+0, s+0);
	BTJ_JFE_TransDCT_Horiz(iblk+8, s+8);
	BTJ_JFE_TransDCT_Horiz(iblk+16, s+16);
	BTJ_JFE_TransDCT_Horiz(iblk+24, s+24);
	BTJ_JFE_TransDCT_Horiz(iblk+32, s+32);
	BTJ_JFE_TransDCT_Horiz(iblk+40, s+40);
	BTJ_JFE_TransDCT_Horiz(iblk+48, s+48);
	BTJ_JFE_TransDCT_Horiz(iblk+56, s+56);

	BTJ_JFE_TransDCT_Vert(s+0, t+0);
	BTJ_JFE_TransDCT_Vert(s+1, t+1);
	BTJ_JFE_TransDCT_Vert(s+2, t+2);
	BTJ_JFE_TransDCT_Vert(s+3, t+3);
	BTJ_JFE_TransDCT_Vert(s+4, t+4);
	BTJ_JFE_TransDCT_Vert(s+5, t+5);
	BTJ_JFE_TransDCT_Vert(s+6, t+6);
	BTJ_JFE_TransDCT_Vert(s+7, t+7);

	for(i=0; i<64; i++)
		oblk[i]=t[i]>>16;
}

#endif

void BTJ_JFEH_QuantBlock(short *ibuf, short *obuf, int qid)
{
	short *cs, *ct;
	int *qt;
	int i, j;

//	for(i=0; i<64; i++)
//		obuf[i]=ibuf[i]/btj_jfe_qt[qid][i];

//	for(i=0; i<64; i++)
//		obuf[i]=((ibuf[i]*btj_jfe_qtfp[qid][i])+2048)>>12;

	cs=ibuf; ct=obuf; qt=btj_jfe_qtfp[qid];
#if 1
	for(i=0; i<8; i++)
	{
		ct[0]=((cs[0]*qt[0])+2048)>>12;
		ct[1]=((cs[1]*qt[1])+2048)>>12;
		ct[2]=((cs[2]*qt[2])+2048)>>12;
		ct[3]=((cs[3]*qt[3])+2048)>>12;
		ct[4]=((cs[4]*qt[4])+2048)>>12;
		ct[5]=((cs[5]*qt[5])+2048)>>12;
		ct[6]=((cs[6]*qt[6])+2048)>>12;
		ct[7]=((cs[7]*qt[7])+2048)>>12;
		cs+=8; ct+=8; qt+=8;
	}
#endif

#if 0
	for(i=0; i<8; i++)
	{
		j=i*8;
		obuf[j+0]=((ibuf[j+0]*btj_jfe_qtfp[qid][j+0])+2048)>>12;
		obuf[j+1]=((ibuf[j+1]*btj_jfe_qtfp[qid][j+1])+2048)>>12;
		obuf[j+2]=((ibuf[j+2]*btj_jfe_qtfp[qid][j+2])+2048)>>12;
		obuf[j+3]=((ibuf[j+3]*btj_jfe_qtfp[qid][j+3])+2048)>>12;
		obuf[j+4]=((ibuf[j+4]*btj_jfe_qtfp[qid][j+4])+2048)>>12;
		obuf[j+5]=((ibuf[j+5]*btj_jfe_qtfp[qid][j+5])+2048)>>12;
		obuf[j+6]=((ibuf[j+6]*btj_jfe_qtfp[qid][j+6])+2048)>>12;
		obuf[j+7]=((ibuf[j+7]*btj_jfe_qtfp[qid][j+7])+2048)>>12;
	}
#endif
}

void BTJ_JFEH_SetupQuantTabDivFP(int qid)
{
	int i;
	
	for(i=0; i<64; i++)
		btj_jfe_qtfp[qid][i]=4096.0/btj_jfe_qt[qid][i];
}

//Encoder

#if 0
void BTJ_JFEH_WriteBit(int i)
{
	btj_jfeh_win|=i<<(31-btj_jfeh_pos);
	btj_jfeh_pos++;
	if(btj_jfeh_pos>=8)
	{
		i=(btj_jfeh_win>>24)&0xFF;
		*btj_jfeh_ct++=i;
		if(i==0xFF)*btj_jfeh_ct++=0x00;
		btj_jfeh_win<<=8;
		btj_jfeh_pos-=8;
	}
}

void BTJ_JFEH_WriteNBits(int i, int n)
{
	i&=(1<<n)-1;

	btj_jfeh_win|=i<<((32-n)-btj_jfeh_pos);
	btj_jfeh_pos+=n;
	while(btj_jfeh_pos>=8)
	{
		i=(btj_jfeh_win>>24)&0xFF;
		*btj_jfeh_ct++=i;
		if(i==0xFF)*btj_jfeh_ct++=0x00;
		btj_jfeh_win<<=8;
		btj_jfeh_pos-=8;
	}
}

void BTJ_JFEH_FlushBits()
{
	int i;
	while(btj_jfeh_pos>0)
	{
		i=(btj_jfeh_win>>24)&0xFF;
		*btj_jfeh_ct++=i;
		if(i==0xFF)*btj_jfeh_ct++=0x00;
		btj_jfeh_win<<=8;
		btj_jfeh_pos-=8;
	}
}
#endif

#if 1
void BTJ_JFEH_WriteBit(int i)
{
	btj_jfeh_pos--;
	btj_jfeh_win|=i<<btj_jfeh_pos;
	if(btj_jfeh_pos<=24)
	{
		i=(btj_jfeh_win>>24)&0xFF;
		*btj_jfeh_ct++=i;
		if(i==0xFF)*btj_jfeh_ct++=0x00;
		btj_jfeh_win<<=8;
		btj_jfeh_pos+=8;
	}
}

void BTJ_JFEH_WriteNBits(int i, int n)
{
	i&=(1<<n)-1;

	btj_jfeh_pos-=n;
	btj_jfeh_win|=i<<btj_jfeh_pos;
	while(btj_jfeh_pos<=24)
	{
		i=(btj_jfeh_win>>24)&0xFF;
		*btj_jfeh_ct++=i;
		if(i==0xFF)*btj_jfeh_ct++=0x00;
		btj_jfeh_win<<=8;
		btj_jfeh_pos+=8;
	}
}

void BTJ_JFEH_FlushBits()
{
	int i;
//	while(btj_jfeh_pos>0)
	while(btj_jfeh_pos<32)
	{
		i=(btj_jfeh_win>>24)&0xFF;
		*btj_jfeh_ct++=i;
		if(i==0xFF)*btj_jfeh_ct++=0x00;
		btj_jfeh_win<<=8;
		btj_jfeh_pos+=8;
	}
}
#endif

void BTJ_JFEH_EncodeSymbol(int tab, int v)
{
	BTJ_JFEH_WriteNBits(btj_jfeh_code[tab][v], btj_jfeh_len[tab][v]);
}

void BTJ_JFEH_EncodeVal(int tab, int z, int v)
{
	int i, j, k;

	if(!v) { BTJ_JFEH_EncodeSymbol(tab, z<<4); return; }

	if(v>0)
	{
		i=1;
		while(v>=(1<<i))i++;
		BTJ_JFEH_EncodeSymbol(tab, (z<<4)|i);
		BTJ_JFEH_WriteNBits(v, i);
		return;
	}

	i=1; j=-v; while(j>=(1<<i))i++;
	BTJ_JFEH_EncodeSymbol(tab, (z<<4)|i);

	k=(1<<i)-(j+1);
	BTJ_JFEH_WriteNBits(k, i);
	return;
}

int BTJ_JFEH_EncodeBlock(short *buf, int dctab, int actab)
{
	int i, j, k;

	BTJ_JFEH_EncodeVal(dctab, 0, buf[0]);

	for(i=1; i<64; i++)
	{
		j=buf[btj_jfe_zigzag2[i]];
		if(j)
		{
			BTJ_JFEH_EncodeVal(actab, 0, j);
			continue;
		}

		for(j=i+1; j<64; j++)
			if(buf[btj_jfe_zigzag2[j]])break;
		if(j>=64)
		{
			BTJ_JFEH_EncodeSymbol(actab, 0);
			break;
		}

		while((j-i)>15)
		{
			BTJ_JFEH_EncodeSymbol(actab, 15<<4);
			i+=16;
		}

		j-=i;
//		if(j>15)j=15;
		BTJ_JFEH_EncodeVal(actab, j, buf[btj_jfe_zigzag2[i+j]]);
		i+=j;
	}
}

int BTJ_JFE_GetImgBlk(short *blk, int xo, int yo,
	short *img, int xs, int ys)
{
	int i, j, k;

	for(i=0; i<DCTSZ; i++)for(j=0; j<DCTSZ; j++)
	{
		k=img[(i+yo)*xs+(j+xo)];
		blk[i*DCTSZ+j]=k;
	}
}

int BTJ_JFE_FilterImageDCT(short *ibuf, short *obuf, int xs, int ys)
{
	static short tblk[DCTSZ2], tblk2[DCTSZ2];
	static short blk[DCTSZ2];
	int i, j, k, l;

	k=0;
	for(i=0; i<(ys/DCTSZ); i++)
		for(j=0; j<(xs/DCTSZ); j++)
	{
		BTJ_JFE_GetImgBlk(blk, j*DCTSZ, i*DCTSZ, ibuf, xs, ys);
		BTJ_JFE_TransDCT(blk, obuf+k*DCTSZ2);
		k++;
	}
}

void BTJ_JFE_EmitDQT(int n)
{
	int i;

	*btj_jfeh_ct++=0xFF;
	*btj_jfeh_ct++=JPG_DQT;

	i=64+3;
	*btj_jfeh_ct++=i>>8;
	*btj_jfeh_ct++=i&0xFF;

	*btj_jfeh_ct++=n;
	for(i=0; i<64; i++)
		*btj_jfeh_ct++=btj_jfe_qt[n][btj_jfe_zigzag2[i]];
}

void BTJ_JFE_EmitSOF(int xs, int ys)
{
	int i;

	*btj_jfeh_ct++=0xFF;
	*btj_jfeh_ct++=JPG_SOF0;

	i=8+3*3;
	*btj_jfeh_ct++=i>>8;	//Lf
	*btj_jfeh_ct++=i&0xFF;

	*btj_jfeh_ct++=8; 	//P

	*btj_jfeh_ct++=ys>>8;	//Y
	*btj_jfeh_ct++=ys&0xFF;	//Y
	*btj_jfeh_ct++=xs>>8;	//X
	*btj_jfeh_ct++=xs&0xFF;	//X

	*btj_jfeh_ct++=3;	//Nf

	*btj_jfeh_ct++=1;	//Ci
	*btj_jfeh_ct++=0x22;	//Hi Vi
	*btj_jfeh_ct++=0;	//Tqi
	*btj_jfeh_ct++=2;	//Ci
	*btj_jfeh_ct++=0x11;	//Hi Vi
	*btj_jfeh_ct++=1;	//Tqi
	*btj_jfeh_ct++=3;	//Ci
	*btj_jfeh_ct++=0x11;	//Hi Vi
	*btj_jfeh_ct++=1;	//Tqi
}

void BTJ_JFE_EmitSOS()
{
	int i;

	*btj_jfeh_ct++=0xFF;
	*btj_jfeh_ct++=JPG_SOS;

	i=6+3*2;
	*btj_jfeh_ct++=i>>8;	//Lf
	*btj_jfeh_ct++=i&0xFF;

	*btj_jfeh_ct++=3; 	//Ns

	*btj_jfeh_ct++=1;	//Csi
	*btj_jfeh_ct++=0x00;	//Tdi Tai
	*btj_jfeh_ct++=2;	//Csi
	*btj_jfeh_ct++=0x11;	//Tdi Tai
	*btj_jfeh_ct++=3;	//Csi
	*btj_jfeh_ct++=0x11;	//Tdi Tai

	*btj_jfeh_ct++=0; 	//Ss
	*btj_jfeh_ct++=63; 	//Se
	*btj_jfeh_ct++=0x00; 	//Ah Al
}

void BTJ_JFE_EmitDHT(int tab)
{
	byte *p;
	int i, j, k;

	*btj_jfeh_ct++=0xFF;
	*btj_jfeh_ct++=JPG_DHT;

//	i=8+3*1;
	p=btj_jfeh_ct;
	*btj_jfeh_ct++=i>>8;	//Lf
	*btj_jfeh_ct++=i&0xFF;

	i=(tab/2)|((tab&1)<<4);
	*btj_jfeh_ct++=i; 	//Tc Th

//	tab<<=8;
	for(i=1; i<=16; i++)
	{
		k=0;
		for(j=0; j<256; j++)
			if(btj_jfeh_len[tab][j]==i)
				k++;
		*btj_jfeh_ct++=k; 	//Li
	}

	k=0;
	for(i=1; i<=16; i++)
	{
		k<<=1;
		for(j=0; j<256; j++)
			if(btj_jfeh_len[tab][j]==i)
		{
			*btj_jfeh_ct++=j; 	//Vi
			btj_jfeh_code[tab][j]=k++;
		}
	}

//	printf("DHT %04X\n", k);

	i=btj_jfeh_ct-p;
	p[0]=i>>8;	//Lf
	p[1]=i&0xFF;

#if 0
	printf("DHT Tab=%d\n", tab);

#if 0
	printf("{ ");
	
	for(i=0; i<32; i++)
	{
		for(j=0; j<8; j++)
		{
			printf("0x%04X, ", ctx->huff_code[tab|(i*8+j)]);
		}
		printf("\n");
	}
	printf("},\n");
#endif

	printf("{ ");
	
	for(i=0; i<16; i++)
	{
		for(j=0; j<16; j++)
		{
			printf("%2d, ", btj_jfeh_len[tab][i*16+j]);
		}
		printf("\n");
	}
	printf("},\n");

#endif
}

static int pdjpg_ijg_qtab_y[64] = {
16, 11, 10, 16,  24,  40,  51,  61,
12, 12, 14, 19,  26,  58,  60,  55,
14, 13, 16, 24,  40,  57,  69,  56,
14, 17, 22, 29,  51,  87,  80,  62,
18, 22, 37, 56,  68, 109, 103,  77,
24, 35, 55, 64,  81, 104, 113,  92,
49, 64, 78, 87, 103, 121, 120, 101,
72, 92, 95, 98, 112, 100, 103,  99};

static int pdjpg_ijg_qtab_uv[64] = {
99, 99, 99, 99, 99, 99, 99, 99,
99, 99, 99, 99, 99, 99, 99, 99,
99, 99, 99, 99, 99, 99, 99, 99,
99, 99, 99, 99, 99, 99, 99, 99,
47, 66, 99, 99, 99, 99, 99, 99,
24, 26, 56, 99, 99, 99, 99, 99,
18, 21, 26, 66, 99, 99, 99, 99,
17, 18, 24, 47, 99, 99, 99, 99};

void BTJ_JFE_MakeQuantTabFastIJG_Y(byte *tab, float qf)
{
	double s, q;
	int i, j;

	q=(qf*100);
	s=(q<50)?5000/q:(200-2*q);
	for(i=0; i<64; i++)
	{
		j=(int)((s * pdjpg_ijg_qtab_y[i]+50)/100);
		j=(j<1)?1:((j<256)?j:255);
		tab[i]=j;
	}
}

void BTJ_JFE_MakeQuantTabFastIJG_UV(byte *tab, float qf)
{
	double s, q;
	int i, j;

	q=(qf*100);
	s=(q<50)?5000/q:(200-2*q);
	for(i=0; i<64; i++)
	{
		j=(int)((s*pdjpg_ijg_qtab_uv[i]+50)/100);
		j=(j<1)?1:((j<256)?j:255);
		tab[i]=j;
	}
}

BGBBTJ_API int BTJ_JFE_EncodeFast(byte *ibuf, byte *obuf,
	int xs, int ys, int qf)
{
	static short *yb=NULL, *ub, *vb;
	static short *ydb=NULL, *udb, *vdb;
	static int lxs=0, lys=0;

	static int dcs[256], acs[256];
	static int dcsuv[256], acsuv[256];
	short *tp;

	short *ctu, *ctv;
	byte *cs1, *cs2, *cse;
	short *cty1, *cty2;

	int xs2, ys2, xs3, ys3;
	int cr, cg, cb, cy, cu, cv;
	int cra, cga, cba, crb, cgb, cbb;
	int crc, cgc, cbc, crd, cgd, cbd;
	int cya, cyb, cyc, cyd;
	int i, j, k, l, n;

	xs2=((xs+7)/8)*8;
	ys2=((ys+7)/8)*8;
	xs3=((xs+15)/16)*8;
	ys3=((ys+15)/16)*8;

	//full quality
	for(i=0; i<64; i++)btj_jfe_qt[0][i]=1;
	for(i=0; i<64; i++)btj_jfe_qt[1][i]=1;


	if(!yb || (xs!=lxs) || (ys!=lys))
	{
		if(yb)
		{
			free(yb);
			free(ub);
			free(vb);
			free(ydb);
			free(udb);
			free(vdb);
		}

		yb=malloc(xs2*ys2*sizeof(short));
		ub=malloc(xs2*ys2*sizeof(short));
		vb=malloc(xs2*ys2*sizeof(short));

		ydb=malloc((xs2+8)*(ys2+16)*sizeof(short));
		udb=malloc((xs3+8)*(ys3+8)*sizeof(short));
		vdb=malloc((xs3+8)*(ys3+8)*sizeof(short));

		lxs=xs;
		lys=ys;
	}

//	memset(yb, 128, xs2*ys2);
//	memset(ub, 128, xs2*ys2);
//	memset(vb, 128, xs2*ys2);

//	memset(ydb, 0, xs2*(ys2+8)*sizeof(short));
//	memset(udb, 0, xs3*(ys3+8)*sizeof(short));
//	memset(vdb, 0, xs3*(ys3+8)*sizeof(short));

#if 0
	for(i=0; i<ys3; i++)
		for(j=0; j<xs3; j++)
	{
		k=((ys-(2*i+1))*xs+j*2+0)*4;
		cra=ibuf[k+0]; cga=ibuf[k+1]; cba=ibuf[k+2];
		k=((ys-(2*i+1))*xs+j*2+1)*4;
		crb=ibuf[k+0]; cgb=ibuf[k+1]; cbb=ibuf[k+2];
		k=((ys-(2*i+2))*xs+j*2+0)*4;
		crc=ibuf[k+0]; cgc=ibuf[k+1]; cbc=ibuf[k+2];
		k=((ys-(2*i+2))*xs+j*2+1)*4;
		crd=ibuf[k+0]; cgd=ibuf[k+1]; cbd=ibuf[k+2];

		cya=( 76*cra +150*cga + 29*cba)>>8;
		cyb=( 76*crb +150*cgb + 29*cbb)>>8;
		cyc=( 76*crc +150*cgc + 29*cbc)>>8;
		cyd=( 76*crd +150*cgd + 29*cbd)>>8;

		cr=(cra+crb+crc+crd)>>2;
		cg=(cga+cgb+cgc+cgd)>>2;
		cb=(cba+cbb+cbc+cbd)>>2;
		cu=-43*cr - 84*cg +128*cb;
		cv=128*cr -107*cg - 20*cb;
		cu=(cu>>8)+128; cv=(cv>>8)+128;

		yb[(2*i+0)*xs2+(j*2+0)]=cya;
		yb[(2*i+0)*xs2+(j*2+1)]=cyb;
		yb[(2*i+1)*xs2+(j*2+0)]=cyc;
		yb[(2*i+1)*xs2+(j*2+1)]=cyd;

		ub[i*xs3+j]=cu;
		vb[i*xs3+j]=cv;
	}
#endif

#if 1
	n=(ys+1)/2;
	for(i=0; i<n; i++)
	{
		k=((ys-(2*i+1))*xs)*4;
		cs1=ibuf+k; cse=cs1+xs*4;
		l=((ys-(2*i+2))*xs)*4;
		cs2=ibuf+l;

		cty1=yb+(2*i+0)*xs2;
		cty2=yb+(2*i+1)*xs2;

		l=i*xs3;
		ctu=ub+l;
		ctv=vb+l;

		while(cs1<cse)
		{
			cra=*(cs1+0); cga=*(cs1+1); cba=*(cs1+2);
			crb=*(cs1+4); cgb=*(cs1+5); cbb=*(cs1+6);
			crc=*(cs2+0); cgc=*(cs2+1); cbc=*(cs2+2);
			crd=*(cs2+4); cgd=*(cs2+5); cbd=*(cs2+6);

			cya=19595*cra + 38470*cga + 7471*cba;
			cyb=19595*crb + 38470*cgb + 7471*cbb;
			cyc=19595*crc + 38470*cgc + 7471*cbc;
			cyd=19595*crd + 38470*cgd + 7471*cbd;
			*cty1++=cya>>16;
			*cty1++=cyb>>16;
			*cty2++=cyc>>16;
			*cty2++=cyd>>16;

			cr=(cra+crb+crc+crd)>>2;
			cg=(cga+cgb+cgc+cgd)>>2;
			cb=(cba+cbb+cbc+cbd)>>2;

			cu=-11056*cr -21712*cg +32768*cb;
			cv= 32768*cr -27440*cg - 5328*cb;
			*ctu++=(cu>>16)+128;
			*ctv++=(cv>>16)+128;

			cs1+=8; cs2+=8;
		}
	}
#endif

	BTJ_JFE_FilterImageDCT(yb, ydb, xs2, ys2);
	BTJ_JFE_FilterImageDCT(ub, udb, xs3, ys3);
	BTJ_JFE_FilterImageDCT(vb, vdb, xs3, ys3);

	BTJ_JFE_MakeQuantTabFastIJG_Y(btj_jfe_qt[0], qf/100.0);
	BTJ_JFE_MakeQuantTabFastIJG_UV(btj_jfe_qt[1], qf/100.0);

	BTJ_JFEH_SetupQuantTabDivFP(0);
	BTJ_JFEH_SetupQuantTabDivFP(1);

	l=0;
	for(i=0; i<=(ys3/8); i++)
		for(j=0; j<(xs3/8); j++)
	{
		tp=ydb+((i*2+0)*(xs2/8)+j*2+0)*64;
		BTJ_JFEH_QuantBlock(tp, tp, 0);
		tp[0]-=l; l=tp[0]+l;

		tp=ydb+((i*2+0)*(xs2/8)+j*2+1)*64;
		BTJ_JFEH_QuantBlock(tp, tp, 0);
		tp[0]-=l; l=tp[0]+l;

		tp=ydb+((i*2+1)*(xs2/8)+j*2+0)*64;
		BTJ_JFEH_QuantBlock(tp, tp, 0);
		tp[0]-=l; l=tp[0]+l;

		tp=ydb+((i*2+1)*(xs2/8)+j*2+1)*64;
		BTJ_JFEH_QuantBlock(tp, tp, 0);
		tp[0]-=l; l=tp[0]+l;
	}

	j=(xs3/8)*(ys3/8); k=0; l=0;
	for(i=0; i<j; i++)
	{
		BTJ_JFEH_QuantBlock(udb+i*64, udb+i*64, 1);
		BTJ_JFEH_QuantBlock(vdb+i*64, vdb+i*64, 1);
		udb[i*64+0]-=k; k=udb[i*64+0]+k;
		vdb[i*64+0]-=l; l=vdb[i*64+0]+l;
	}

//	printf("M2\n");

#if 0
	for(i=0; i<256; i++)dcs[i]=0;
	for(i=0; i<256; i++)acs[i]=0;
	for(i=0; i<256; i++)dcsuv[i]=0;
	for(i=0; i<256; i++)acsuv[i]=0;

	for(i=0; i<16; i++)dcs[i]=1;
	for(i=0; i<16; i++)dcsuv[i]=1;

//	for(i=0; i<256; i++)dcs[i]=1;
	for(i=0; i<256; i++)acs[i]=1;
//	for(i=0; i<256; i++)dcsuv[i]=1;
	for(i=0; i<256; i++)acsuv[i]=1;

	j=(xs2/8)*(ys2/8);
	k=(xs3/8)*(ys3/8);
	for(i=0; i<j; i++)BGBBTJ_Huff_StatBlock(ydb+i*64, dcs, acs);
	for(i=0; i<k; i++)BGBBTJ_Huff_StatBlock(udb+i*64, dcsuv, acsuv);
	for(i=0; i<k; i++)BGBBTJ_Huff_StatBlock(vdb+i*64, dcsuv, acsuv);
	
//	if(ctx->jpg_yuvw)
//		{ for(i=0; i<k; i++)BGBBTJ_Huff_StatBlock(ctx->wdb+i*64,
//			dcsuv, acsuv); }

//	BGBBTJ_Huff_BuildLengths(dcs, 256, ctx->huff_len+0*256, 16);
//	BGBBTJ_Huff_BuildLengths(acs, 256, ctx->huff_len+1*256, 16);
//	BGBBTJ_Huff_BuildLengths(dcsuv, 256, ctx->huff_len+2*256, 16);
//	BGBBTJ_Huff_BuildLengths(acsuv, 256, ctx->huff_len+3*256, 16);

//	BGBBTJ_Huff_BuildLengthsAdjust(dcs, 256, btj_jfeh_len[0], 16);
//	BGBBTJ_Huff_BuildLengthsAdjust(acs, 256, btj_jfeh_len[1], 16);
//	BGBBTJ_Huff_BuildLengthsAdjust(dcsuv, 256, btj_jfeh_len[2], 16);
//	BGBBTJ_Huff_BuildLengthsAdjust(acsuv, 256, btj_jfeh_len[3], 16);

	BGBBTJ_Huff_BuildLengthsAdjust(dcs, 256, btj_jfeh_len[0], 15);
	BGBBTJ_Huff_BuildLengthsAdjust(acs, 256, btj_jfeh_len[1], 15);
	BGBBTJ_Huff_BuildLengthsAdjust(dcsuv, 256, btj_jfeh_len[2], 15);
	BGBBTJ_Huff_BuildLengthsAdjust(acsuv, 256, btj_jfeh_len[3], 15);
#endif

	btj_jfeh_ct=obuf;
	btj_jfeh_win=0;
//	btj_jfeh_pos=0;
	btj_jfeh_pos=32;

	*btj_jfeh_ct++=0xFF;
	*btj_jfeh_ct++=JPG_SOI;

	BTJ_JFE_EmitDQT(0);
	BTJ_JFE_EmitDQT(1);

	BTJ_JFE_EmitSOF(xs, ys);

	BTJ_JFE_EmitDHT(0);
	BTJ_JFE_EmitDHT(1);
	BTJ_JFE_EmitDHT(2);
	BTJ_JFE_EmitDHT(3);

	BTJ_JFE_EmitSOS();

	for(i=0; i<=(ys3/8); i++)
		for(j=0; j<(xs3/8); j++)
	{
		BTJ_JFEH_EncodeBlock(ydb+((i*2+0)*(xs2/8)+j*2+0)*64, 0, 1);
		BTJ_JFEH_EncodeBlock(ydb+((i*2+0)*(xs2/8)+j*2+1)*64, 0, 1);
		BTJ_JFEH_EncodeBlock(ydb+((i*2+1)*(xs2/8)+j*2+0)*64, 0, 1);
		BTJ_JFEH_EncodeBlock(ydb+((i*2+1)*(xs2/8)+j*2+1)*64, 0, 1);

		k=i*(xs3/8)+j;
		BTJ_JFEH_EncodeBlock(udb+k*64, 2, 3);
		BTJ_JFEH_EncodeBlock(vdb+k*64, 2, 3);
	}

	BTJ_JFEH_FlushBits();

	*btj_jfeh_ct++=0xFF;
	*btj_jfeh_ct++=JPG_EOI;

	i=btj_jfeh_ct-obuf;

	return(i);
}
