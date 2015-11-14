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

static const byte btj_jfe_zigzag[64]={
 0,  1,  5,  6, 14, 15, 27, 28,
 2,  4,  7, 13, 16, 26, 29, 42,
 3,  8, 12, 17, 25, 30, 41, 43,
 9, 11, 18, 24, 31, 40, 44, 53,
10, 19, 23, 32, 39, 45, 52, 54,
20, 22, 33, 38, 46, 51, 55, 60,
21, 34, 37, 47, 50, 56, 59, 61,
35, 36, 48, 49, 57, 58, 62, 63
};

static const byte btj_jfe_zigzag2[64]={
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

static const byte btj_jfeh_len[4][256]={
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

#if 1
void BTJ_JFE_TransDCT_Horiz(short *iblk, int *oblk)
{
	int ib0, ib1, ib2, ib3, ib4, ib5, ib6, ib7;
	int ib0p7, ib1p6, ib2p5, ib3p4;
	int ib0n7, ib1n6, ib2n5, ib3n4;
	int ib0p7n3n4, ib1p6n2n5;

	ib0=iblk[0]-128;	ib1=iblk[1]-128;
	ib2=iblk[2]-128;	ib3=iblk[3]-128;
	ib4=iblk[4]-128;	ib5=iblk[5]-128;
	ib6=iblk[6]-128;	ib7=iblk[7]-128;

	ib0p7=ib0+ib7;	ib1p6=ib1+ib6;
	ib2p5=ib2+ib5;	ib3p4=ib3+ib4;
	ib0n7=ib0-ib7;	ib1n6=ib1-ib6;
	ib2n5=ib2-ib5;	ib3n4=ib3-ib4;

	oblk[0]=(ib0p7+ib1p6+ib2p5+ib3p4)*91;
	oblk[4]=(ib0p7-ib1p6-ib2p5+ib3p4)*91;

	ib0p7n3n4=ib0p7-ib3p4;
	ib1p6n2n5=ib1p6-ib2p5;
	oblk[2]=ib0p7n3n4*118 +ib1p6n2n5* 49;
	oblk[6]=ib0p7n3n4* 49 -ib1p6n2n5*118;

	oblk[1]=ib0n7*126 +ib1n6*106 +ib2n5* 71 +ib3n4* 25;
	oblk[3]=ib0n7*106 -ib1n6* 25 -ib2n5*126 -ib3n4* 71;
	oblk[5]=ib0n7* 71 -ib1n6*126 +ib2n5* 25 +ib3n4*106;
	oblk[7]=ib0n7* 25 -ib1n6* 71 +ib2n5*106 -ib3n4*126;
}

void BTJ_JFE_TransDCT_Vert2(int *iblk, short *oblk)
{
	int ib0, ib1, ib2, ib3, ib4, ib5, ib6, ib7;
	int ob0, ob1, ob2, ob3, ob4, ob5, ob6, ob7;
	int ib0p7, ib1p6, ib2p5, ib3p4;
	int ib0n7, ib1n6, ib2n5, ib3n4;
	int ib0p7n3n4, ib1p6n2n5;

	ib0=iblk[ 0];	ib1=iblk[ 8];
	ib2=iblk[16];	ib3=iblk[24];
	ib4=iblk[32];	ib5=iblk[40];
	ib6=iblk[48];	ib7=iblk[56];

	ib0p7=ib0+ib7;	ib1p6=ib1+ib6;
	ib2p5=ib2+ib5;	ib3p4=ib3+ib4;
	ib0n7=ib0-ib7;	ib1n6=ib1-ib6;
	ib2n5=ib2-ib5;	ib3n4=ib3-ib4;

	ob0=(ib0p7+ib1p6+ib2p5+ib3p4)*91;
	ob4=(ib0p7-ib1p6-ib2p5+ib3p4)*91;

	ib0p7n3n4=ib0p7-ib3p4;
	ib1p6n2n5=ib1p6-ib2p5;
	ob2=ib0p7n3n4*118 +ib1p6n2n5* 49;
	ob6=ib0p7n3n4* 49 -ib1p6n2n5*118;

	ob1=ib0n7*126 +ib1n6*106 +ib2n5* 71 +ib3n4* 25;
	ob3=ib0n7*106 -ib1n6* 25 -ib2n5*126 -ib3n4* 71;
	ob5=ib0n7* 71 -ib1n6*126 +ib2n5* 25 +ib3n4*106;
	ob7=ib0n7* 25 -ib1n6* 71 +ib2n5*106 -ib3n4*126;

	oblk[ 0]=(ob0+32768)>>16;	oblk[ 8]=(ob1+32768)>>16;
	oblk[16]=(ob2+32768)>>16;	oblk[24]=(ob3+32768)>>16;
	oblk[32]=(ob4+32768)>>16;	oblk[40]=(ob5+32768)>>16;
	oblk[48]=(ob6+32768)>>16;	oblk[56]=(ob7+32768)>>16;
}

void BTJ_JFE_TransDCT(short *iblk, short *oblk)
{
	int s[DCTSZ2];
	int i, j;

	BTJ_JFE_TransDCT_Horiz(iblk+ 0, s+ 0);
	BTJ_JFE_TransDCT_Horiz(iblk+ 8, s+ 8);
	BTJ_JFE_TransDCT_Horiz(iblk+16, s+16);
	BTJ_JFE_TransDCT_Horiz(iblk+24, s+24);
	BTJ_JFE_TransDCT_Horiz(iblk+32, s+32);
	BTJ_JFE_TransDCT_Horiz(iblk+40, s+40);
	BTJ_JFE_TransDCT_Horiz(iblk+48, s+48);
	BTJ_JFE_TransDCT_Horiz(iblk+56, s+56);

	BTJ_JFE_TransDCT_Vert2(s+0, oblk+0);
	BTJ_JFE_TransDCT_Vert2(s+1, oblk+1);
	BTJ_JFE_TransDCT_Vert2(s+2, oblk+2);
	BTJ_JFE_TransDCT_Vert2(s+3, oblk+3);
	BTJ_JFE_TransDCT_Vert2(s+4, oblk+4);
	BTJ_JFE_TransDCT_Vert2(s+5, oblk+5);
	BTJ_JFE_TransDCT_Vert2(s+6, oblk+6);
	BTJ_JFE_TransDCT_Vert2(s+7, oblk+7);
}
#endif

#if 1
void BTJ_JFEH_QuantBlock(BGBBTJ_JFE_Context *ctx,
	short *ibuf, short *obuf, int qid)
{
	short tbuf[64];
	short *cs, *ct;
	int *qt;
	byte *zt;
	int i, j;

#if 0
	//HACK: DC Range Trap
	if((ibuf[0]>=2048) || (ibuf[0]<=-2048))
	{
		i=ibuf[0];
		if(ibuf[0]>=2048)ibuf[0]=2048;
		if(ibuf[0]<=-2048)ibuf[0]=-2048;
		printf("BTJ_JFEH_QuantBlock: DC Range Trap %d\n", i);
	}
#endif

#if 1
	qt=ctx->qtfp[qid];

//	0,  1,  5,  6, 14, 15, 27, 28,
	tbuf[ 0]=((ibuf[ 0]*qt[ 0])+2048)>>12;
	tbuf[ 1]=((ibuf[ 1]*qt[ 1])+2048)>>12;
	tbuf[ 5]=((ibuf[ 2]*qt[ 2])+2048)>>12;
	tbuf[ 6]=((ibuf[ 3]*qt[ 3])+2048)>>12;
	tbuf[14]=((ibuf[ 4]*qt[ 4])+2048)>>12;
	tbuf[15]=((ibuf[ 5]*qt[ 5])+2048)>>12;
	tbuf[27]=((ibuf[ 6]*qt[ 6])+2048)>>12;
	tbuf[28]=((ibuf[ 7]*qt[ 7])+2048)>>12;

//	2,  4,  7, 13, 16, 26, 29, 42,
	tbuf[ 2]=((ibuf[ 8]*qt[ 8])+2048)>>12;
	tbuf[ 4]=((ibuf[ 9]*qt[ 9])+2048)>>12;
	tbuf[ 7]=((ibuf[10]*qt[10])+2048)>>12;
	tbuf[13]=((ibuf[11]*qt[11])+2048)>>12;
	tbuf[16]=((ibuf[12]*qt[12])+2048)>>12;
	tbuf[26]=((ibuf[13]*qt[13])+2048)>>12;
	tbuf[29]=((ibuf[14]*qt[14])+2048)>>12;
	tbuf[42]=((ibuf[15]*qt[15])+2048)>>12;

//	3,  8, 12, 17, 25, 30, 41, 43,
	tbuf[ 3]=((ibuf[16]*qt[16])+2048)>>12;
	tbuf[ 8]=((ibuf[17]*qt[17])+2048)>>12;
	tbuf[12]=((ibuf[18]*qt[18])+2048)>>12;
	tbuf[17]=((ibuf[19]*qt[19])+2048)>>12;
	tbuf[25]=((ibuf[20]*qt[20])+2048)>>12;
	tbuf[30]=((ibuf[21]*qt[21])+2048)>>12;
	tbuf[41]=((ibuf[22]*qt[22])+2048)>>12;
	tbuf[43]=((ibuf[23]*qt[23])+2048)>>12;

//	9, 11, 18, 24, 31, 40, 44, 53,
	tbuf[ 9]=((ibuf[24]*qt[24])+2048)>>12;
	tbuf[11]=((ibuf[25]*qt[25])+2048)>>12;
	tbuf[18]=((ibuf[26]*qt[26])+2048)>>12;
	tbuf[24]=((ibuf[27]*qt[27])+2048)>>12;
	tbuf[31]=((ibuf[28]*qt[28])+2048)>>12;
	tbuf[40]=((ibuf[29]*qt[29])+2048)>>12;
	tbuf[44]=((ibuf[30]*qt[30])+2048)>>12;
	tbuf[53]=((ibuf[31]*qt[31])+2048)>>12;

//	10, 19, 23, 32, 39, 45, 52, 54,
	tbuf[10]=((ibuf[32]*qt[32])+2048)>>12;
	tbuf[19]=((ibuf[33]*qt[33])+2048)>>12;
	tbuf[23]=((ibuf[34]*qt[34])+2048)>>12;
	tbuf[32]=((ibuf[35]*qt[35])+2048)>>12;
	tbuf[39]=((ibuf[36]*qt[36])+2048)>>12;
	tbuf[45]=((ibuf[37]*qt[37])+2048)>>12;
	tbuf[52]=((ibuf[38]*qt[38])+2048)>>12;
	tbuf[54]=((ibuf[39]*qt[39])+2048)>>12;

//	20, 22, 33, 38, 46, 51, 55, 60,
	tbuf[20]=((ibuf[40]*qt[40])+2048)>>12;
	tbuf[22]=((ibuf[41]*qt[41])+2048)>>12;
	tbuf[33]=((ibuf[42]*qt[42])+2048)>>12;
	tbuf[38]=((ibuf[43]*qt[43])+2048)>>12;
	tbuf[46]=((ibuf[44]*qt[44])+2048)>>12;
	tbuf[51]=((ibuf[45]*qt[45])+2048)>>12;
	tbuf[55]=((ibuf[46]*qt[46])+2048)>>12;
	tbuf[60]=((ibuf[47]*qt[47])+2048)>>12;

//	21, 34, 37, 47, 50, 56, 59, 61,
	tbuf[21]=((ibuf[48]*qt[48])+2048)>>12;
	tbuf[34]=((ibuf[49]*qt[49])+2048)>>12;
	tbuf[37]=((ibuf[50]*qt[50])+2048)>>12;
	tbuf[47]=((ibuf[51]*qt[51])+2048)>>12;
	tbuf[50]=((ibuf[52]*qt[52])+2048)>>12;
	tbuf[56]=((ibuf[53]*qt[53])+2048)>>12;
	tbuf[59]=((ibuf[54]*qt[54])+2048)>>12;
	tbuf[61]=((ibuf[55]*qt[55])+2048)>>12;

//	35, 36, 48, 49, 57, 58, 62, 63
	tbuf[35]=((ibuf[56]*qt[56])+2048)>>12;
	tbuf[36]=((ibuf[57]*qt[57])+2048)>>12;
	tbuf[48]=((ibuf[58]*qt[58])+2048)>>12;
	tbuf[49]=((ibuf[59]*qt[59])+2048)>>12;
	tbuf[57]=((ibuf[60]*qt[60])+2048)>>12;
	tbuf[58]=((ibuf[61]*qt[61])+2048)>>12;
	tbuf[62]=((ibuf[62]*qt[62])+2048)>>12;
	tbuf[63]=((ibuf[63]*qt[63])+2048)>>12;

	memcpy(obuf, tbuf, 64*sizeof(short));
#endif

#if 0
	cs=ibuf; qt=ctx->qtfp[qid]; zt=btj_jfe_zigzag;
	for(i=0; i<8; i++)
	{
		tbuf[zt[0]]=((cs[0]*qt[0])+2048)>>12;
		tbuf[zt[1]]=((cs[1]*qt[1])+2048)>>12;
		tbuf[zt[2]]=((cs[2]*qt[2])+2048)>>12;
		tbuf[zt[3]]=((cs[3]*qt[3])+2048)>>12;
		tbuf[zt[4]]=((cs[4]*qt[4])+2048)>>12;
		tbuf[zt[5]]=((cs[5]*qt[5])+2048)>>12;
		tbuf[zt[6]]=((cs[6]*qt[6])+2048)>>12;
		tbuf[zt[7]]=((cs[7]*qt[7])+2048)>>12;
		cs+=8; qt+=8; zt+=8;
	}
	memcpy(obuf, tbuf, 64*sizeof(short));
#endif
}
#endif

void BTJ_JFEH_SetupQuantTabDivFP(BGBBTJ_JFE_Context *ctx, int qid)
{
	int i;
	
	for(i=0; i<64; i++)
		ctx->qtfp[qid][i]=4096.0/ctx->qt[qid][i]+0.5;
}

//Encoder
#if 1
void BTJ_JFEH_WriteBit(BGBBTJ_JFE_Context *ctx, int i)
{
	ctx->pos--;
	ctx->win|=i<<ctx->pos;
	if(ctx->pos<=24)
	{
		i=(ctx->win>>24)&0xFF;
		*ctx->ct++=i;
		if(i==0xFF)*ctx->ct++=0x00;
		ctx->win<<=8;
		ctx->pos+=8;
	}
}

void BTJ_JFEH_WriteNBits(BGBBTJ_JFE_Context *ctx, int i, int n)
{
//	i&=(1<<n)-1;

	ctx->pos-=n;
	ctx->win|=i<<ctx->pos;
	while(ctx->pos<=24)
	{
		i=(ctx->win>>24)&0xFF;
		*ctx->ct++=i;
		if(i==0xFF)*ctx->ct++=0x00;
		ctx->win<<=8;
		ctx->pos+=8;
	}
}

void BTJ_JFEH_FlushBits(BGBBTJ_JFE_Context *ctx)
{
	int i;
	while(ctx->pos<32)
	{
		i=(ctx->win>>24)&0xFF;
		*ctx->ct++=i;
		if(i==0xFF)*ctx->ct++=0x00;
		ctx->win<<=8;
		ctx->pos+=8;
	}
}
#endif

void BTJ_JFEH_EncodeSymbol(BGBBTJ_JFE_Context *ctx, int tab, int v)
{
	BTJ_JFEH_WriteNBits(ctx, btj_jfeh_code[tab][v], btj_jfeh_len[tab][v]);
}

void BTJ_JFEH_EncodeVal(BGBBTJ_JFE_Context *ctx, int tab, int z, int v)
{
	int i, j, k;

	if(!v) { BTJ_JFEH_EncodeSymbol(ctx, tab, z<<4); return; }

	if(v>0)
	{
		i=1;
		while(v>=(1<<i))i++;
		BTJ_JFEH_EncodeSymbol(ctx, tab, (z<<4)|i);
		BTJ_JFEH_WriteNBits(ctx, v, i);
		return;
	}

	i=1; j=-v; while(j>=(1<<i))i++;
	BTJ_JFEH_EncodeSymbol(ctx, tab, (z<<4)|i);

	k=(1<<i)-(j+1);
	BTJ_JFEH_WriteNBits(ctx, k, i);
	return;
}

void BTJ_JFEH_EncodeBlock(BGBBTJ_JFE_Context *ctx,
	short *buf, int dctab, int actab)
{
	int i, j, k;

	BTJ_JFEH_EncodeVal(ctx, dctab, 0, buf[0]);

	i=1;
	while(i<64)
	{
		j=buf[i];
		if(j)
		{
			BTJ_JFEH_EncodeVal(ctx, actab, 0, j);
			i++;
			continue;
		}

		j=i+1;
		while(1)
		{
#if 1
			if(j<56)
			{
				if(buf[j])break;
				if(buf[++j])break;
				if(buf[++j])break;
				if(buf[++j])break;
				if(buf[++j])break;
				if(buf[++j])break;
				if(buf[++j])break;
				if(buf[++j])break;
				continue;
			}else
			{
				while(1)
				{
					if(j>=64)
					{
						BTJ_JFEH_EncodeSymbol(ctx, actab, 0);
						return;
					}
					if(buf[j])break;
					j++;
				}
				break;
			}
#endif

#if 0
			if(j>=64)
			{
				BTJ_JFEH_EncodeSymbol(ctx, actab, 0);
				return;
			}
			if(buf[j])break;
			j++;
#endif
		}

		k=j-i;
//		while((j-i)>15)
		while(k>15)
		{
			BTJ_JFEH_EncodeSymbol(ctx, actab, 15<<4);
			i+=16; k=j-i;
		}

//		k=j-i;
		BTJ_JFEH_EncodeVal(ctx, actab, k, buf[j]);
		i=j+1;
	}
}

void BTJ_JFE_GetImgBlk2Y_RGB(short *blk, int xo, int yo,
	byte *img, int xs, int ys, int xs2, int ys2, int ssz)
{
	byte *ib;
	short *ob;
	int sx0, sx1, sx2, sx3, sx4, sx5, sx6, sx7;
	int i, j, k;

	if(((xo+8)>xs) || ((ys-yo-8)<0))
	{
		//goes off edge of image
		for(i=0; i<8; i++)
			for(j=0; j<8; j++)
		{
			k=(ys-(yo+i)-1);
			if(((xo+j)>=xs) || (k<0))
				{ blk[i*8+j]=0; continue; }
			ib=img+((k*xs)+xo+j)*ssz;
			blk[i*8+j]=(19595*ib[0] + 38470*ib[1] +
				7471*ib[2] + 32768)>>16;
		}

		return;
	}

	if(ssz==4)
	{
		for(i=0; i<8; i++)
		{
			ib=img+(((ys-(yo+i)-1)*xs)+xo)*ssz;
			ob=blk+(i*8);
			ob[0]=(19595*ib[ 0] + 38470*ib[ 1] + 7471*ib[ 2] + 32768)>>16;
			ob[1]=(19595*ib[ 4] + 38470*ib[ 5] + 7471*ib[ 6] + 32768)>>16;
			ob[2]=(19595*ib[ 8] + 38470*ib[ 9] + 7471*ib[10] + 32768)>>16;
			ob[3]=(19595*ib[12] + 38470*ib[13] + 7471*ib[14] + 32768)>>16;
			ob[4]=(19595*ib[16] + 38470*ib[17] + 7471*ib[18] + 32768)>>16;
			ob[5]=(19595*ib[20] + 38470*ib[21] + 7471*ib[22] + 32768)>>16;
			ob[6]=(19595*ib[24] + 38470*ib[25] + 7471*ib[26] + 32768)>>16;
			ob[7]=(19595*ib[28] + 38470*ib[29] + 7471*ib[30] + 32768)>>16;
		}
	}else
	{
		sx0=0;     sx1=ssz;   sx2=2*ssz; sx3=3*ssz;
		sx4=4*ssz; sx5=5*ssz; sx6=6*ssz; sx7=7*ssz;
		for(i=0; i<8; i++)
		{
			ib=img+(((ys-(yo+i)-1)*xs)+xo)*ssz;
			ob=blk+(i*8);
			ob[0]=(19595*ib[sx0+0] + 38470*ib[sx0+1] + 7471*ib[sx0+2] + 32768)>>16;
			ob[1]=(19595*ib[sx1+0] + 38470*ib[sx1+1] + 7471*ib[sx1+2] + 32768)>>16;
			ob[2]=(19595*ib[sx2+0] + 38470*ib[sx2+1] + 7471*ib[sx2+2] + 32768)>>16;
			ob[3]=(19595*ib[sx3+0] + 38470*ib[sx3+1] + 7471*ib[sx3+2] + 32768)>>16;
			ob[4]=(19595*ib[sx4+0] + 38470*ib[sx4+1] + 7471*ib[sx4+2] + 32768)>>16;
			ob[5]=(19595*ib[sx5+0] + 38470*ib[sx5+1] + 7471*ib[sx5+2] + 32768)>>16;
			ob[6]=(19595*ib[sx6+0] + 38470*ib[sx6+1] + 7471*ib[sx6+2] + 32768)>>16;
			ob[7]=(19595*ib[sx7+0] + 38470*ib[sx7+1] + 7471*ib[sx7+2] + 32768)>>16;
		}
	}
}

void BTJ_JFE_GetImgBlk2Y_BGR(short *blk, int xo, int yo,
	byte *img, int xs, int ys, int xs2, int ys2, int ssz)
{
	byte *ib;
	short *ob;
	int sx0, sx1, sx2, sx3, sx4, sx5, sx6, sx7;
	int i, j, k;

	if(((xo+8)>xs) || ((ys-yo-8)<0))
	{
		//goes off edge of image
		for(i=0; i<8; i++)
			for(j=0; j<8; j++)
		{
			k=(ys-(yo+i)-1);
			if(((xo+j)>=xs) || (k<0))
				{ blk[i*8+j]=0; continue; }
			ib=img+((k*xs)+xo+j)*ssz;
			blk[i*8+j]=(19595*ib[2] + 38470*ib[1] +
				7471*ib[0] + 32768)>>16;
		}

		return;
	}

	if(ssz==4)
	{
		for(i=0; i<8; i++)
		{
			ib=img+(((ys-(yo+i)-1)*xs)+xo)*ssz;
			ob=blk+(i*8);
			ob[0]=(19595*ib[ 2] + 38470*ib[ 1] + 7471*ib[ 0] + 32768)>>16;
			ob[1]=(19595*ib[ 6] + 38470*ib[ 5] + 7471*ib[ 4] + 32768)>>16;
			ob[2]=(19595*ib[10] + 38470*ib[ 9] + 7471*ib[ 8] + 32768)>>16;
			ob[3]=(19595*ib[14] + 38470*ib[13] + 7471*ib[12] + 32768)>>16;
			ob[4]=(19595*ib[18] + 38470*ib[17] + 7471*ib[16] + 32768)>>16;
			ob[5]=(19595*ib[22] + 38470*ib[21] + 7471*ib[20] + 32768)>>16;
			ob[6]=(19595*ib[26] + 38470*ib[25] + 7471*ib[24] + 32768)>>16;
			ob[7]=(19595*ib[30] + 38470*ib[29] + 7471*ib[28] + 32768)>>16;
		}
	}else
	{
		sx0=0;     sx1=ssz;   sx2=2*ssz; sx3=3*ssz;
		sx4=4*ssz; sx5=5*ssz; sx6=6*ssz; sx7=7*ssz;
		for(i=0; i<8; i++)
		{
			ib=img+(((ys-(yo+i)-1)*xs)+xo)*ssz;
			ob=blk+(i*8);
			ob[0]=(19595*ib[sx0+2] + 38470*ib[sx0+1] + 
				7471*ib[sx0+0] + 32768)>>16;
			ob[1]=(19595*ib[sx1+2] + 38470*ib[sx1+1] + 
				7471*ib[sx1+0] + 32768)>>16;
			ob[2]=(19595*ib[sx2+2] + 38470*ib[sx2+1] + 
				7471*ib[sx2+0] + 32768)>>16;
			ob[3]=(19595*ib[sx3+2] + 38470*ib[sx3+1] + 
				7471*ib[sx3+0] + 32768)>>16;
			ob[4]=(19595*ib[sx4+2] + 38470*ib[sx4+1] + 
				7471*ib[sx4+0] + 32768)>>16;
			ob[5]=(19595*ib[sx5+2] + 38470*ib[sx5+1] + 
				7471*ib[sx5+0] + 32768)>>16;
			ob[6]=(19595*ib[sx6+2] + 38470*ib[sx6+1] + 
				7471*ib[sx6+0] + 32768)>>16;
			ob[7]=(19595*ib[sx7+2] + 38470*ib[sx7+1] + 
				7471*ib[sx7+0] + 32768)>>16;
		}
	}
}

void BTJ_JFE_GetImgBlk2UV_RGB(
	short *ublk, short *vblk, int xo, int yo,
	byte *img, int xs, int ys, int xs2, int ys2, int ssz)
{
	byte *ib0, *ib1;
	short *obu, *obv;
	int cr, cg, cb, cu, cv;
	int i, j, k;

//	if(((xo+16)>xs) || ((2*ys2-yo-1)>ys))
	if(((xo+16)>xs) || ((ys-yo-16)<0))
	{
		//goes off edge of image
		for(i=0; i<8; i++)
			for(j=0; j<8; j++)
		{
			if((xo+2*j+1)>=xs)
				{ ublk[i*8+j]=0; vblk[i*8+j]=0; continue; }
			k=(ys-(yo+i*2+1)-1);
//			if(k>=ys)
			if(k<0)
				{ ublk[i*8+j]=0; vblk[i*8+j]=0; continue; }

			ib0=img+(((ys-(yo+i*2+0)-1)*xs)+xo+j*2)*ssz;
			ib1=img+(((ys-(yo+i*2+1)-1)*xs)+xo+j*2)*ssz;
			obu=ublk+(i*8)+j; obv=vblk+(i*8)+j;

			cr=(ib0[0]+ib0[ssz+0]+ib1[0]+ib1[ssz+0])>>2;
			cg=(ib0[1]+ib0[ssz+1]+ib1[1]+ib1[ssz+1])>>2;
			cb=(ib0[2]+ib0[ssz+2]+ib1[2]+ib1[ssz+2])>>2;
			cu=-11056*cr -21712*cg +32768*cb;
			cv= 32768*cr -27440*cg - 5328*cb;
			obu[0]=((cu+32768)>>16)+128;
			obv[0]=((cv+32768)>>16)+128;
		}

		return;
	}

	if(ssz==4)
	{
		for(i=0; i<8; i++)
		{
			ib0=img+(((ys-(yo+i*2+0)-1)*xs)+xo)*ssz;
			ib1=img+(((ys-(yo+i*2+1)-1)*xs)+xo)*ssz;
			obu=ublk+(i*8); obv=vblk+(i*8);

			for(j=0; j<2; j++)
			{
				cr=(ib0[0]+ib0[4]+ib1[0]+ib1[4])>>2;
				cg=(ib0[1]+ib0[5]+ib1[1]+ib1[5])>>2;
				cb=(ib0[2]+ib0[6]+ib1[2]+ib1[6])>>2;
				cu=-11056*cr -21712*cg +32768*cb;
				cv= 32768*cr -27440*cg - 5328*cb;
				obu[0]=((cu+32768)>>16)+128;
				obv[0]=((cv+32768)>>16)+128;

				cr=(ib0[ 8]+ib0[12]+ib1[ 8]+ib1[12])>>2;
				cg=(ib0[ 9]+ib0[13]+ib1[ 9]+ib1[13])>>2;
				cb=(ib0[10]+ib0[14]+ib1[10]+ib1[14])>>2;
				cu=-11056*cr -21712*cg +32768*cb;
				cv= 32768*cr -27440*cg - 5328*cb;
				obu[1]=((cu+32768)>>16)+128;
				obv[1]=((cv+32768)>>16)+128;

				cr=(ib0[16]+ib0[20]+ib1[16]+ib1[20])>>2;
				cg=(ib0[17]+ib0[21]+ib1[17]+ib1[21])>>2;
				cb=(ib0[18]+ib0[22]+ib1[18]+ib1[22])>>2;
				cu=-11056*cr -21712*cg +32768*cb;
				cv= 32768*cr -27440*cg - 5328*cb;
				obu[2]=((cu+32768)>>16)+128;
				obv[2]=((cv+32768)>>16)+128;

				cr=(ib0[24]+ib0[28]+ib1[24]+ib1[28])>>2;
				cg=(ib0[25]+ib0[29]+ib1[25]+ib1[29])>>2;
				cb=(ib0[26]+ib0[30]+ib1[26]+ib1[30])>>2;
				cu=-11056*cr -21712*cg +32768*cb;
				cv= 32768*cr -27440*cg - 5328*cb;
				obu[3]=((cu+32768)>>16)+128;
				obv[3]=((cv+32768)>>16)+128;

				ib0+=32; ib1+=32; obu+=4; obv+=4;
			}
		}
	}else
	{
		for(i=0; i<8; i++)
		{
			ib0=img+(((ys-(yo+i*2+0)-1)*xs)+xo)*ssz;
			ib1=img+(((ys-(yo+i*2+1)-1)*xs)+xo)*ssz;
			obu=ublk+(i*8); obv=vblk+(i*8);

			for(j=0; j<8; j++)
			{
				cr=(ib0[0]+ib0[ssz+0]+ib1[0]+ib1[ssz+0])>>2;
				cg=(ib0[1]+ib0[ssz+1]+ib1[1]+ib1[ssz+1])>>2;
				cb=(ib0[2]+ib0[ssz+2]+ib1[2]+ib1[ssz+2])>>2;
				cu=-11056*cr -21712*cg +32768*cb;
				cv= 32768*cr -27440*cg - 5328*cb;
				obu[0]=((cu+32768)>>16)+128;
				obv[0]=((cv+32768)>>16)+128;
				ib0+=2*ssz; ib1+=2*ssz; obu++; obv++;
			}
		}
	}
}

void BTJ_JFE_GetImgBlk2UV_BGR(
	short *ublk, short *vblk, int xo, int yo,
	byte *img, int xs, int ys, int xs2, int ys2, int ssz)
{
	byte *ib0, *ib1;
	short *obu, *obv;
	int cr, cg, cb, cu, cv;
	int i, j, k;

//	if(((xo+16)>xs) || ((2*ys2-yo-1)>ys))
	if(((xo+16)>xs) || ((ys-yo-16)<0))
	{
		//goes off edge of image
		for(i=0; i<8; i++)
			for(j=0; j<8; j++)
		{
			k=(ys-(yo+i*2+1)-1);
			if(((xo+2*j+1)>=xs) || (k<0))
				{ ublk[i*8+j]=0; vblk[i*8+j]=0; continue; }

			ib0=img+(((ys-(yo+i*2+0)-1)*xs)+xo+j*2)*ssz;
			ib1=img+(((ys-(yo+i*2+1)-1)*xs)+xo+j*2)*ssz;
			obu=ublk+(i*8)+j; obv=vblk+(i*8)+j;

			cb=(ib0[0]+ib0[ssz+0]+ib1[0]+ib1[ssz+0])>>2;
			cg=(ib0[1]+ib0[ssz+1]+ib1[1]+ib1[ssz+1])>>2;
			cr=(ib0[2]+ib0[ssz+2]+ib1[2]+ib1[ssz+2])>>2;
			cu=-11056*cr -21712*cg +32768*cb;
			cv= 32768*cr -27440*cg - 5328*cb;
			obu[0]=((cu+32768)>>16)+128;
			obv[0]=((cv+32768)>>16)+128;
		}

		return;
	}

	if(ssz==4)
	{
		for(i=0; i<8; i++)
		{
			ib0=img+(((ys-(yo+i*2+0)-1)*xs)+xo)*ssz;
			ib1=img+(((ys-(yo+i*2+1)-1)*xs)+xo)*ssz;
			obu=ublk+(i*8); obv=vblk+(i*8);

			for(j=0; j<2; j++)
			{
				cb=(ib0[0]+ib0[4]+ib1[0]+ib1[4])>>2;
				cg=(ib0[1]+ib0[5]+ib1[1]+ib1[5])>>2;
				cr=(ib0[2]+ib0[6]+ib1[2]+ib1[6])>>2;
				cu=-11056*cr -21712*cg +32768*cb;
				cv= 32768*cr -27440*cg - 5328*cb;
				obu[0]=((cu+32768)>>16)+128;
				obv[0]=((cv+32768)>>16)+128;

				cb=(ib0[ 8]+ib0[12]+ib1[ 8]+ib1[12])>>2;
				cg=(ib0[ 9]+ib0[13]+ib1[ 9]+ib1[13])>>2;
				cr=(ib0[10]+ib0[14]+ib1[10]+ib1[14])>>2;
				cu=-11056*cr -21712*cg +32768*cb;
				cv= 32768*cr -27440*cg - 5328*cb;
				obu[1]=((cu+32768)>>16)+128;
				obv[1]=((cv+32768)>>16)+128;

				cb=(ib0[16]+ib0[20]+ib1[16]+ib1[20])>>2;
				cg=(ib0[17]+ib0[21]+ib1[17]+ib1[21])>>2;
				cr=(ib0[18]+ib0[22]+ib1[18]+ib1[22])>>2;
				cu=-11056*cr -21712*cg +32768*cb;
				cv= 32768*cr -27440*cg - 5328*cb;
				obu[2]=((cu+32768)>>16)+128;
				obv[2]=((cv+32768)>>16)+128;

				cb=(ib0[24]+ib0[28]+ib1[24]+ib1[28])>>2;
				cg=(ib0[25]+ib0[29]+ib1[25]+ib1[29])>>2;
				cr=(ib0[26]+ib0[30]+ib1[26]+ib1[30])>>2;
				cu=-11056*cr -21712*cg +32768*cb;
				cv= 32768*cr -27440*cg - 5328*cb;
				obu[3]=((cu+32768)>>16)+128;
				obv[3]=((cv+32768)>>16)+128;

				ib0+=32; ib1+=32; obu+=4; obv+=4;
			}
		}
	}else
	{
		for(i=0; i<8; i++)
		{
			ib0=img+(((ys-(yo+i*2+0)-1)*xs)+xo)*ssz;
			ib1=img+(((ys-(yo+i*2+1)-1)*xs)+xo)*ssz;
			obu=ublk+(i*8); obv=vblk+(i*8);

			for(j=0; j<8; j++)
			{
				cb=(ib0[0]+ib0[ssz+0]+ib1[0]+ib1[ssz+0])>>2;
				cg=(ib0[1]+ib0[ssz+1]+ib1[1]+ib1[ssz+1])>>2;
				cr=(ib0[2]+ib0[ssz+2]+ib1[2]+ib1[ssz+2])>>2;
				cu=-11056*cr -21712*cg +32768*cb;
				cv= 32768*cr -27440*cg - 5328*cb;
				obu[0]=((cu+32768)>>16)+128;
				obv[0]=((cv+32768)>>16)+128;
				ib0+=2*ssz; ib1+=2*ssz; obu++; obv++;
			}
		}
	}
}

void BTJ_JFE_FilterImageDCT2(byte *ibuf,
	short *ybuf, short *ubuf, short *vbuf, int xs, int ys,
	int qf, int pf)
{
	short ublk[DCTSZ2], vblk[DCTSZ2];
	short blk[DCTSZ2];
	int xs2, ys2, xs3, ys3, ssz;
	int xs2n, ys2n, xs3n, ys3n;
	int x2, y2;
	int i, j, k, l;

	xs2=((xs+7)/8)*8;
	ys2=((ys+7)/8)*8;
	xs3=((xs+15)/16)*8;
	ys3=((ys+15)/16)*8;

	xs2n=xs2/8;
	ys2n=ys2/8;

	xs3n=xs3/8;
	ys3n=ys3/8;

#if 1
	if((pf==BGBBTJ_JPG_RGBA) || (pf==BGBBTJ_JPG_RGB))
	{
		ssz=4;
		if(pf==BGBBTJ_JPG_RGB)ssz=3;
		k=0;
		for(i=0; i<ys3n; i++)
			for(j=0; j<xs3n; j++)
		{
			x2=j*2; y2=i*2;
			l=(y2+0)*xs2n+x2+0;
			BTJ_JFE_GetImgBlk2Y_RGB(blk, x2*DCTSZ, y2*DCTSZ,
				ibuf, xs, ys, xs2, ys2, ssz);
			BTJ_JFE_TransDCT(blk, ybuf+l*DCTSZ2);
			l++;
			BTJ_JFE_GetImgBlk2Y_RGB(blk, (x2+1)*DCTSZ, y2*DCTSZ,
				ibuf, xs, ys, xs2, ys2, ssz);
			BTJ_JFE_TransDCT(blk, ybuf+l*DCTSZ2);
			l=(y2+1)*xs2n+x2+0;
			BTJ_JFE_GetImgBlk2Y_RGB(blk, x2*DCTSZ, (y2+1)*DCTSZ,
				ibuf, xs, ys, xs2, ys2, ssz);
			BTJ_JFE_TransDCT(blk, ybuf+l*DCTSZ2);
			l++;
			BTJ_JFE_GetImgBlk2Y_RGB(blk, (x2+1)*DCTSZ, (y2+1)*DCTSZ,
				ibuf, xs, ys, xs2, ys2, ssz);
			BTJ_JFE_TransDCT(blk, ybuf+l*DCTSZ2);

			BTJ_JFE_GetImgBlk2UV_RGB(ublk, vblk,
				x2*DCTSZ, y2*DCTSZ, ibuf, xs, ys, xs3, ys3, ssz);
			BTJ_JFE_TransDCT(ublk, ubuf+k*DCTSZ2);
			BTJ_JFE_TransDCT(vblk, vbuf+k*DCTSZ2);
			k++;
		}
		
		return;
	}

	if((pf==BGBBTJ_JPG_BGRA) || (pf==BGBBTJ_JPG_BGR))
	{
		ssz=4;
		if(pf==BGBBTJ_JPG_BGR)ssz=3;
		k=0;
		for(i=0; i<ys3n; i++)
			for(j=0; j<xs3n; j++)
		{
			x2=j*2; y2=i*2;
			l=(y2+0)*xs2n+x2+0;
			BTJ_JFE_GetImgBlk2Y_BGR(blk, x2*DCTSZ, y2*DCTSZ,
				ibuf, xs, ys, xs2, ys2, ssz);
			BTJ_JFE_TransDCT(blk, ybuf+l*DCTSZ2);
			l++;
			BTJ_JFE_GetImgBlk2Y_BGR(blk, (x2+1)*DCTSZ, y2*DCTSZ,
				ibuf, xs, ys, xs2, ys2, ssz);
			BTJ_JFE_TransDCT(blk, ybuf+l*DCTSZ2);
			l=(y2+1)*xs2n+x2+0;
			BTJ_JFE_GetImgBlk2Y_BGR(blk, x2*DCTSZ, (y2+1)*DCTSZ,
				ibuf, xs, ys, xs2, ys2, ssz);
			BTJ_JFE_TransDCT(blk, ybuf+l*DCTSZ2);
			l++;
			BTJ_JFE_GetImgBlk2Y_BGR(blk, (x2+1)*DCTSZ, (y2+1)*DCTSZ,
				ibuf, xs, ys, xs2, ys2, ssz);
			BTJ_JFE_TransDCT(blk, ybuf+l*DCTSZ2);

			BTJ_JFE_GetImgBlk2UV_BGR(ublk, vblk,
				x2*DCTSZ, y2*DCTSZ, ibuf, xs, ys, xs3, ys3, ssz);
			BTJ_JFE_TransDCT(ublk, ubuf+k*DCTSZ2);
			BTJ_JFE_TransDCT(vblk, vbuf+k*DCTSZ2);
			k++;
		}
		
		return;
	}
#endif

	if((pf==BGBBTJ_JPG_RGBA) || (pf==BGBBTJ_JPG_BGRA) ||
		(pf==BGBBTJ_JPG_RGB) || (pf==BGBBTJ_JPG_BGR))
	{
		ssz=4;
		if((pf==BGBBTJ_JPG_RGB) || (pf==BGBBTJ_JPG_BGR))ssz=3;
	
		k=0;
		for(i=0; i<(ys2/DCTSZ); i++)
			for(j=0; j<(xs2/DCTSZ); j++)
		{
			BTJ_JFE_GetImgBlk2Y_RGB(blk, j*DCTSZ, i*DCTSZ,
				ibuf, xs, ys, xs2, ys2, ssz);
			BTJ_JFE_TransDCT(blk, ybuf+k*DCTSZ2);
			k++;
		}
	}else if((pf==BGBBTJ_JPG_ARGB) || (pf==BGBBTJ_JPG_ABGR))
	{
		ssz=4;
		k=0;
		for(i=0; i<(ys2/DCTSZ); i++)
			for(j=0; j<(xs2/DCTSZ); j++)
		{
			BTJ_JFE_GetImgBlk2Y_RGB(blk, j*DCTSZ, i*DCTSZ,
				ibuf+1, xs, ys, xs2, ys2, ssz);
			BTJ_JFE_TransDCT(blk, ybuf+k*DCTSZ2);
			k++;
		}
	}

	if((pf==BGBBTJ_JPG_RGBA) || (pf==BGBBTJ_JPG_RGB))
	{
		ssz=4;
		if(pf==BGBBTJ_JPG_RGB)ssz=3;

		k=0;
		for(i=0; i<(ys3/DCTSZ); i++)
			for(j=0; j<(xs3/DCTSZ); j++)
		{
			BTJ_JFE_GetImgBlk2UV_RGB(ublk, vblk,
				2*j*DCTSZ, 2*i*DCTSZ, ibuf, xs, ys, xs3, ys3, ssz);
			BTJ_JFE_TransDCT(ublk, ubuf+k*DCTSZ2);
			BTJ_JFE_TransDCT(vblk, vbuf+k*DCTSZ2);
			k++;
		}
	}else if((pf==BGBBTJ_JPG_BGRA) || (pf==BGBBTJ_JPG_BGR))
	{
		ssz=4;
		if(pf==BGBBTJ_JPG_BGR)ssz=3;

		k=0;
		for(i=0; i<(ys3/DCTSZ); i++)
			for(j=0; j<(xs3/DCTSZ); j++)
		{
			BTJ_JFE_GetImgBlk2UV_BGR(ublk, vblk,
				2*j*DCTSZ, 2*i*DCTSZ, ibuf, xs, ys, xs3, ys3, ssz);
			BTJ_JFE_TransDCT(ublk, ubuf+k*DCTSZ2);
			BTJ_JFE_TransDCT(vblk, vbuf+k*DCTSZ2);
			k++;
		}
	}else if(pf==BGBBTJ_JPG_ARGB)
	{
		ssz=4; k=0;
		for(i=0; i<(ys3/DCTSZ); i++)
			for(j=0; j<(xs3/DCTSZ); j++)
		{
			BTJ_JFE_GetImgBlk2UV_RGB(ublk, vblk,
				2*j*DCTSZ, 2*i*DCTSZ, ibuf+1, xs, ys, xs3, ys3, ssz);
			BTJ_JFE_TransDCT(ublk, ubuf+k*DCTSZ2);
			BTJ_JFE_TransDCT(vblk, vbuf+k*DCTSZ2);
			k++;
		}
	}else if(pf==BGBBTJ_JPG_ABGR)
	{
		ssz=4; k=0;
		for(i=0; i<(ys3/DCTSZ); i++)
			for(j=0; j<(xs3/DCTSZ); j++)
		{
			BTJ_JFE_GetImgBlk2UV_BGR(ublk, vblk,
				2*j*DCTSZ, 2*i*DCTSZ, ibuf+1, xs, ys, xs3, ys3, ssz);
			BTJ_JFE_TransDCT(ublk, ubuf+k*DCTSZ2);
			BTJ_JFE_TransDCT(vblk, vbuf+k*DCTSZ2);
			k++;
		}
	}
}


void BTJ_JFE_EmitDQT(BGBBTJ_JFE_Context *ctx, int n)
{
	int i;

	*ctx->ct++=0xFF;
	*ctx->ct++=JPG_DQT;

	i=64+3;
	*ctx->ct++=i>>8;
	*ctx->ct++=i&0xFF;

	*ctx->ct++=n;
	for(i=0; i<64; i++)
		*ctx->ct++=ctx->qt[n][btj_jfe_zigzag2[i]];
}

void BTJ_JFE_EmitSOF(BGBBTJ_JFE_Context *ctx, int xs, int ys)
{
	int i;

	*ctx->ct++=0xFF;
	*ctx->ct++=JPG_SOF0;

	i=8+3*3;
	*ctx->ct++=i>>8;	//Lf
	*ctx->ct++=i&0xFF;

	*ctx->ct++=8; 	//P

	*ctx->ct++=ys>>8;	//Y
	*ctx->ct++=ys&0xFF;	//Y
	*ctx->ct++=xs>>8;	//X
	*ctx->ct++=xs&0xFF;	//X

	*ctx->ct++=3;	//Nf

	*ctx->ct++=1;	//Ci
	*ctx->ct++=0x22;	//Hi Vi
	*ctx->ct++=0;	//Tqi
	*ctx->ct++=2;	//Ci
	*ctx->ct++=0x11;	//Hi Vi
	*ctx->ct++=1;	//Tqi
	*ctx->ct++=3;	//Ci
	*ctx->ct++=0x11;	//Hi Vi
	*ctx->ct++=1;	//Tqi
}

void BTJ_JFE_EmitSOS(BGBBTJ_JFE_Context *ctx)
{
	int i;

	*ctx->ct++=0xFF;
	*ctx->ct++=JPG_SOS;

	i=6+3*2;
	*ctx->ct++=i>>8;	//Lf
	*ctx->ct++=i&0xFF;

	*ctx->ct++=3; 	//Ns

	*ctx->ct++=1;	//Csi
	*ctx->ct++=0x00;	//Tdi Tai
	*ctx->ct++=2;	//Csi
	*ctx->ct++=0x11;	//Tdi Tai
	*ctx->ct++=3;	//Csi
	*ctx->ct++=0x11;	//Tdi Tai

	*ctx->ct++=0; 	//Ss
	*ctx->ct++=63; 	//Se
	*ctx->ct++=0x00; 	//Ah Al
}

void BTJ_JFE_EmitDHT(BGBBTJ_JFE_Context *ctx, int tab)
{
	byte *p;
	int i, j, k;

	*ctx->ct++=0xFF;
	*ctx->ct++=JPG_DHT;

	p=ctx->ct;
	*ctx->ct++=0;	//Lf
	*ctx->ct++=0;

	i=(tab/2)|((tab&1)<<4);
	*ctx->ct++=i; 	//Tc Th

//	tab<<=8;
	for(i=1; i<=16; i++)
	{
		k=0;
		for(j=0; j<256; j++)
			if(btj_jfeh_len[tab][j]==i)
				k++;
		*ctx->ct++=k; 	//Li
	}

	k=0;
	for(i=1; i<=16; i++)
	{
		k<<=1;
		for(j=0; j<256; j++)
			if(btj_jfeh_len[tab][j]==i)
		{
			*ctx->ct++=j; 	//Vi
			btj_jfeh_code[tab][j]=k++;
		}
	}

//	printf("DHT %04X\n", k);

	i=ctx->ct-p;
	p[0]=i>>8;	//Lf
	p[1]=i&0xFF;
}

void BTJ_JFE_EmitDHT2(BGBBTJ_JFE_Context *ctx)
{
	static int init=0;
	static byte *tab=NULL;
	static int tabsz;
	byte *ct0;
	int i;
	
	if(!init)
	{
		ct0=ctx->ct;
		BTJ_JFE_EmitDHT(ctx, 0);
		BTJ_JFE_EmitDHT(ctx, 1);
		BTJ_JFE_EmitDHT(ctx, 2);
		BTJ_JFE_EmitDHT(ctx, 3);
		i=ctx->ct-ct0;
		tab=malloc(i);
		tabsz=i;
		memcpy(tab, ct0, i);
		init=1;
		return;
	}
	
	memcpy(ctx->ct, tab, tabsz);
	ctx->ct+=tabsz;
}

static const int pdjpg_ijg_qtab_y[64] = {
16, 11, 10, 16,  24,  40,  51,  61,
12, 12, 14, 19,  26,  58,  60,  55,
14, 13, 16, 24,  40,  57,  69,  56,
14, 17, 22, 29,  51,  87,  80,  62,
18, 22, 37, 56,  68, 109, 103,  77,
24, 35, 55, 64,  81, 104, 113,  92,
49, 64, 78, 87, 103, 121, 120, 101,
72, 92, 95, 98, 112, 100, 103,  99};

static const int pdjpg_ijg_qtab_uv[64] = {
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


BGBBTJ_API BGBBTJ_JFE_Context *BTJ_JFE_AllocContext(void)
{
	BGBBTJ_JFE_Context *ctx;
	
	ctx=gcalloc(sizeof(BGBBTJ_JFE_Context));
	return(ctx);
}

BGBBTJ_API void BTJ_JFE_FreeContext(
	BGBBTJ_JFE_Context *ctx)
{
	gcfree(ctx);
}

BGBBTJ_API void BTJ_JFE_FlushEncodeFast(
	BGBBTJ_JFE_Context *ctx)
{
}

BGBBTJ_API int BTJ_JFE_EncodeFast(byte *ibuf, byte *obuf,
	int xs, int ys, int qf, int pf)
{
	static BGBBTJ_JFE_Context cctx;
//	memset(&cctx, 0, sizeof(BGBBTJ_JFE_Context));
	return(BTJ_JFE_EncodeFastCtx(&cctx, ibuf, obuf,
		xs, ys, qf, pf));
}

BGBBTJ_API int BTJ_JFE_EncodeFast2(byte *ibuf, byte *obuf,
	int xs, int ys, int qf, int pf)
{
	BGBBTJ_JFE_Context cctx;
	int i;
	
	memset(&cctx, 0, sizeof(BGBBTJ_JFE_Context));
	i=BTJ_JFE_EncodeFastCtx(&cctx, ibuf, obuf,
		xs, ys, qf, pf);
	return(i);
}

BGBBTJ_API int BTJ_JFE_EncodeFastCtx(
	BGBBTJ_JFE_Context *ctx,
	byte *ibuf, byte *obuf,
	int xs, int ys, int qf, int pf)
{
	short *tp;
	int xs2, ys2, xs3, ys3;
	int xs2b, ys2b, xs3b, ys3b;
	int i, j, k, l, n;

	xs2=((xs+7)/8)*8;
	ys2=((ys+7)/8)*8;
	xs3=((xs+15)/16)*8;
	ys3=((ys+15)/16)*8;

	if(!ctx->ydb || (xs!=ctx->lxs) || (ys!=ctx->lys))
	{
		if(ctx->ydb)
		{
			free(ctx->ydb);
			free(ctx->udb);
			free(ctx->vdb);
		}

		ctx->ydb=malloc((xs2+8)*(ys2+16)*sizeof(short));
		ctx->udb=malloc((xs3+8)*(ys3+8)*sizeof(short));
		ctx->vdb=malloc((xs3+8)*(ys3+8)*sizeof(short));

		ctx->lxs=xs;
		ctx->lys=ys;
	}

	BTJ_JFE_FilterImageDCT2(ibuf,
		ctx->ydb, ctx->udb, ctx->vdb, xs, ys, qf, pf);

	if(qf!=ctx->lqf)
	{
		BTJ_JFE_MakeQuantTabFastIJG_Y(ctx->qt[0], (qf&255)/100.0);
		BTJ_JFE_MakeQuantTabFastIJG_UV(ctx->qt[1], (qf&255)/100.0);
		BTJ_JFEH_SetupQuantTabDivFP(ctx, 0);
		BTJ_JFEH_SetupQuantTabDivFP(ctx, 1);
		ctx->lqf=qf;
	}

	xs2b=xs2/8; ys2b=ys2/8;
	xs3b=xs3/8; ys3b=ys3/8;

	l=0;
	for(i=0; i<ys3b; i++)
		for(j=0; j<xs3b; j++)
	{
		tp=ctx->ydb+((i*2+0)*xs2b+j*2+0)*64;
		BTJ_JFEH_QuantBlock(ctx, tp, tp, 0);
		tp[0]-=l; l=tp[0]+l;

		tp=ctx->ydb+((i*2+0)*xs2b+j*2+1)*64;
		BTJ_JFEH_QuantBlock(ctx, tp, tp, 0);
		tp[0]-=l; l=tp[0]+l;

		tp=ctx->ydb+((i*2+1)*xs2b+j*2+0)*64;
		BTJ_JFEH_QuantBlock(ctx, tp, tp, 0);
		tp[0]-=l; l=tp[0]+l;

		tp=ctx->ydb+((i*2+1)*xs2b+j*2+1)*64;
		BTJ_JFEH_QuantBlock(ctx, tp, tp, 0);
		tp[0]-=l; l=tp[0]+l;
	}

	j=xs3b*ys3b; k=0; l=0;
	for(i=0; i<j; i++)
	{
		BTJ_JFEH_QuantBlock(ctx, ctx->udb+i*64, ctx->udb+i*64, 1);
		BTJ_JFEH_QuantBlock(ctx, ctx->vdb+i*64, ctx->vdb+i*64, 1);
		ctx->udb[i*64+0]-=k; k=ctx->udb[i*64+0]+k;
		ctx->vdb[i*64+0]-=l; l=ctx->vdb[i*64+0]+l;
	}

	ctx->ct=obuf;
	ctx->win=0;
	ctx->pos=32;

	*ctx->ct++=0xFF;
	*ctx->ct++=JPG_SOI;

	BTJ_JFE_EmitDQT(ctx, 0);
	BTJ_JFE_EmitDQT(ctx, 1);

	BTJ_JFE_EmitSOF(ctx, xs, ys);

	BTJ_JFE_EmitDHT2(ctx);

//	BTJ_JFE_EmitDHT(ctx, 0);
//	BTJ_JFE_EmitDHT(ctx, 1);
//	BTJ_JFE_EmitDHT(ctx, 2);
//	BTJ_JFE_EmitDHT(ctx, 3);

	BTJ_JFE_EmitSOS(ctx);

	for(i=0; i<ys3b; i++)
		for(j=0; j<xs3b; j++)
	{
		BTJ_JFEH_EncodeBlock(ctx, ctx->ydb+((i*2+0)*xs2b+j*2+0)*64, 0, 1);
		BTJ_JFEH_EncodeBlock(ctx, ctx->ydb+((i*2+0)*xs2b+j*2+1)*64, 0, 1);
		BTJ_JFEH_EncodeBlock(ctx, ctx->ydb+((i*2+1)*xs2b+j*2+0)*64, 0, 1);
		BTJ_JFEH_EncodeBlock(ctx, ctx->ydb+((i*2+1)*xs2b+j*2+1)*64, 0, 1);

		k=i*xs3b+j;
		BTJ_JFEH_EncodeBlock(ctx, ctx->udb+k*64, 2, 3);
		BTJ_JFEH_EncodeBlock(ctx, ctx->vdb+k*64, 2, 3);
	}

	BTJ_JFEH_FlushBits(ctx);

	*ctx->ct++=0xFF;
	*ctx->ct++=JPG_EOI;

	i=ctx->ct-obuf;

	return(i);
}
