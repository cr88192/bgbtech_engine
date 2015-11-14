#include <bgbbtj.h>

/*

IDCT4
 128  167  128   69
 128   69 -128 -167
 128  -69 -128  167
 128 -167  128  -69

DCT4
 128  128  128  128
 167   69  -69 -167
 128 -128 -128  128
  69 -167  167  -69

IDCT8
  91  126  118  106   91   71   49   25
  91  106   49  -25  -91 -126 -118  -71
  91   71  -49 -126  -91   25  118  106
  91   25 -118  -71   91  106  -49 -126
  91  -25 -118   71   91 -106  -49  126
  91  -71  -49  126  -91  -25  118 -106
  91 -106   49   25  -91  126 -118   71
  91 -126  118 -106   91  -71   49  -25

DCT8
  91   91   91   91   91   91   91   91
 126  106   71   25  -25  -71 -106 -126
 118   49  -49 -118 -118  -49   49  118
 106  -25 -126  -71   71  126   25 -106
  91  -91  -91   91   91  -91  -91   91
  71 -126   25  106 -106  -25  126  -71
  49 -118  118  -49  -49  118 -118   49
  25  -71  106 -126  126 -106   71  -25

IDCT16
 64  90  89  87  84  80  75  70  64  57  50  43  35  26  18   9
 64  87  75  57  35   9 -18 -43 -64 -80 -89 -90 -84 -70 -50 -26
 64  80  50   9 -35 -70 -89 -87 -64 -26  18  57  84  90  75  43
 64  70  18 -43 -84 -87 -50   9  64  90  75  26 -35 -80 -89 -57
 64  57 -18 -80 -84 -26  50  90  64  -9 -75 -87 -35  43  89  70
 64  43 -50 -90 -35  57  89  26 -64 -87 -18  70  84   9 -75 -80
 64  26 -75 -70  35  90  18 -80 -64  43  89   9 -84 -57  50  87
 64   9 -89 -26  84  43 -75 -57  64  70 -50 -80  35  87 -18 -90
 64  -9 -89  26  84 -43 -75  57  64 -70 -50  80  35 -87 -18  90
 64 -26 -75  70  35 -90  18  80 -64 -43  89  -9 -84  57  50 -87
 64 -43 -50  90 -35 -57  89 -26 -64  87 -18 -70  84  -9 -75  80
 64 -57 -18  80 -84  26  50 -90  64   9 -75  87 -35 -43  89 -70
 64 -70  18  43 -84  87 -50  -9  64 -90  75 -26 -35  80 -89  57
 64 -80  50  -9 -35  70 -89  87 -64  26  18 -57  84 -90  75 -43
 64 -87  75 -57  35  -9 -18  43 -64  80 -89  90 -84  70 -50  26
 64 -90  89 -87  84 -80  75 -70  64 -57  50 -43  35 -26  18  -9

DCT16
 64  64  64  64  64  64  64  64  64  64  64  64  64  64  64  64
 90  87  80  70  57  43  26   9  -9 -26 -43 -57 -70 -80 -87 -90
 89  75  50  18 -18 -50 -75 -89 -89 -75 -50 -18  18  50  75  89
 87  57   9 -43 -80 -90 -70 -26  26  70  90  80  43  -9 -57 -87
 84  35 -35 -84 -84 -35  35  84  84  35 -35 -84 -84 -35  35  84
 80   9 -70 -87 -26  57  90  43 -43 -90 -57  26  87  70  -9 -80
 75 -18 -89 -50  50  89  18 -75 -75  18  89  50 -50 -89 -18  75
 70 -43 -87   9  90  26 -80 -57  57  80 -26 -90  -9  87  43 -70
 64 -64 -64  64  64 -64 -64  64  64 -64 -64  64  64 -64 -64  64
 57 -80 -26  90  -9 -87  43  70 -70 -43  87   9 -90  26  80 -57
 50 -89  18  75 -75 -18  89 -50 -50  89 -18 -75  75  18 -89  50
 43 -90  57  26 -87  70   9 -80  80  -9 -70  87 -26 -57  90 -43
 35 -84  84 -35 -35  84 -84  35  35 -84  84 -35 -35  84 -84  35
 26 -70  90 -80  43   9 -57  87 -87  57  -9 -43  80 -90  70 -26
 18 -50  75 -89  89 -75  50 -18 -18  50 -75  89 -89  75 -50  18
  9 -26  43 -57  70 -80  87 -90  90 -87  80 -70  57 -43  26  -9

*/

#if 1
void BGBBTJ_JPG_TransDCT_Stride4(int *iblk, int *oblk,
	int istride, int ostride)
{
	int ib0, ib1, ib2, ib3;
	int ib0p, ib1p, ib0n, ib1n;

// DCT4
//  128  128  128  128
//  167   69  -69 -167
//  128 -128 -128  128
//   69 -167  167  -69

	ib0=iblk[istride*0];	ib1=iblk[istride*1];
	ib2=iblk[istride*2];	ib3=iblk[istride*3];

	ib0p=ib0+ib3;	ib0n=ib0-ib3;
	ib1p=ib1+ib2;	ib1n=ib1-ib2;

	oblk[ostride*0]=(ib0p+ib1p)*128;
	oblk[ostride*2]=(ib0p-ib1p)*128;
	oblk[ostride*1]=ib0n*167+ib1n* 69;
	oblk[ostride*3]=ib0n* 69-ib1n*167;

//	oblk[ostride*0]=ib0*128 +ib1*128 +ib2*128 +ib3*128;
//	oblk[ostride*1]=ib0*167 +ib1* 69 -ib2* 69 -ib3*167;
//	oblk[ostride*2]=ib0*128 -ib1*128 -ib2*128 +ib3*128;
//	oblk[ostride*3]=ib0* 69 -ib1*167 +ib2*167 -ib3* 69;
}

void BGBBTJ_JPG_TransDCT4Stride(int *iblk, int *oblk,
	int xstride, int ystride)
{
	int s[16], t[16];
	int i, j;

	BGBBTJ_JPG_TransDCT_Stride4(iblk+ystride*0, s+4*0, xstride, 1);
	BGBBTJ_JPG_TransDCT_Stride4(iblk+ystride*1, s+4*1, xstride, 1);
	BGBBTJ_JPG_TransDCT_Stride4(iblk+ystride*2, s+4*2, xstride, 1);
	BGBBTJ_JPG_TransDCT_Stride4(iblk+ystride*3, s+4*3, xstride, 1);

	BGBBTJ_JPG_TransDCT_Stride4(s+0, t+0, 4, 4);
	BGBBTJ_JPG_TransDCT_Stride4(s+1, t+1, 4, 4);
	BGBBTJ_JPG_TransDCT_Stride4(s+2, t+2, 4, 4);
	BGBBTJ_JPG_TransDCT_Stride4(s+3, t+3, 4, 4);

	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		oblk[i*ystride+j*xstride]=(t[i*4+j]+32768)>>16;
	}
}

#endif

#if 1
void BGBBTJ_JPG_TransIDCT_Stride4(int *iblk, int *oblk,
	int istride, int ostride)
{
	int ib0, ib1, ib2, ib3;
	int a, b, c, d;

// IDCT4
//  128  167  128   69
//  128   69 -128 -167
//  128  -69 -128  167
//  128 -167  128  -69

	ib0=iblk[istride*0];	ib1=iblk[istride*1];
	ib2=iblk[istride*2];	ib3=iblk[istride*3];

	a=(ib0+ib2)*128;	b=(ib0-ib2)*128;
	c=ib1*167+ib3* 69;	d=ib1* 69-ib3*167;

	oblk[ostride*0]=a+c;
	oblk[ostride*1]=b+d;
	oblk[ostride*2]=b-d;
	oblk[ostride*3]=a-c;

//	oblk[ostride*0]=ib0*128 +ib1*167 +ib2*128 +ib3* 69;
//	oblk[ostride*1]=ib0*128 +ib1* 69 -ib2*128 -ib3*167;
//	oblk[ostride*2]=ib0*128 -ib1* 69 -ib2*128 +ib3*167;
//	oblk[ostride*3]=ib0*128 -ib1*167 +ib2*128 -ib3* 69;
}

void BGBBTJ_JPG_TransIDCT4Stride(int *iblk, int *oblk,
	int xstride, int ystride)
{
	int s[16], t[16];
	int i, j;

	BGBBTJ_JPG_TransIDCT_Stride4(iblk+ystride*0, s+4*0, xstride, 1);
	BGBBTJ_JPG_TransIDCT_Stride4(iblk+ystride*1, s+4*1, xstride, 1);
	BGBBTJ_JPG_TransIDCT_Stride4(iblk+ystride*2, s+4*2, xstride, 1);
	BGBBTJ_JPG_TransIDCT_Stride4(iblk+ystride*3, s+4*3, xstride, 1);

	BGBBTJ_JPG_TransIDCT_Stride4(s+0, t+0, 4, 4);
	BGBBTJ_JPG_TransIDCT_Stride4(s+1, t+1, 4, 4);
	BGBBTJ_JPG_TransIDCT_Stride4(s+2, t+2, 4, 4);
	BGBBTJ_JPG_TransIDCT_Stride4(s+3, t+3, 4, 4);

	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		oblk[i*ystride+j*xstride]=(t[i*4+j]+32768)>>16;
	}
}
#endif

#if 1
void BGBBTJ_JPG_TransDCT_Sride8(int *iblk, int *oblk,
	int istride, int ostride)
{
	int ib[8], ob[8];

	ib[0]=iblk[istride*0];	ib[1]=iblk[istride*1];
	ib[2]=iblk[istride*2];	ib[3]=iblk[istride*3];
	ib[4]=iblk[istride*4];	ib[5]=iblk[istride*5];
	ib[6]=iblk[istride*6];	ib[7]=iblk[istride*7];

	ob[0]=ib[0]*91  +ib[1]*91  +ib[2]*91  +ib[3]*91  
			+ib[4]*91  +ib[5]*91  +ib[6]*91  +ib[7]*91;
	ob[1]=ib[0]*126 +ib[1]*106 +ib[2]*71  +ib[3]*25  
			-ib[4]*25  -ib[5]*71  -ib[6]*106 -ib[7]*126;
	ob[2]=ib[0]*118 +ib[1]*49  -ib[2]*49  -ib[3]*118 
			-ib[4]*118 -ib[5]*49  +ib[6]*49  +ib[7]*118;
	ob[3]=ib[0]*106 -ib[1]*25  -ib[2]*126 -ib[3]*71  
			+ib[4]*71  +ib[5]*126 +ib[6]*25  -ib[7]*106;
	ob[4]=ib[0]*91  -ib[1]*91  -ib[2]*91  +ib[3]*91  
			+ib[4]*91  -ib[5]*91  -ib[6]*91  +ib[7]*91;
	ob[5]=ib[0]*71  -ib[1]*126 +ib[2]*25  +ib[3]*106 
			-ib[4]*106 -ib[5]*25  +ib[6]*126 -ib[7]*71;
	ob[6]=ib[0]*49  -ib[1]*118 +ib[2]*118 -ib[3]*49  
			-ib[4]*49  +ib[5]*118 -ib[6]*118 +ib[7]*49;
	ob[7]=ib[0]*25  -ib[1]*71  +ib[2]*106 -ib[3]*126 
			+ib[4]*126 -ib[5]*106 +ib[6]*71  -ib[7]*25;
			
	oblk[ostride*0]=ob[0];
	oblk[ostride*1]=ob[1];
	oblk[ostride*2]=ob[2];
	oblk[ostride*3]=ob[3];
	oblk[ostride*4]=ob[4];
	oblk[ostride*5]=ob[5];
	oblk[ostride*6]=ob[6];
	oblk[ostride*7]=ob[7];
}

void BGBBTJ_JPG_TransDCT8Stride(int *iblk, int *oblk,
	int xstride, int ystride)
{
	int s[64], t[64];
	int i, j;

	BGBBTJ_JPG_TransDCT_Sride8(iblk+ystride*0, s+8*0, xstride, 1);
	BGBBTJ_JPG_TransDCT_Sride8(iblk+ystride*1, s+8*1, xstride, 1);
	BGBBTJ_JPG_TransDCT_Sride8(iblk+ystride*2, s+8*2, xstride, 1);
	BGBBTJ_JPG_TransDCT_Sride8(iblk+ystride*3, s+8*3, xstride, 1);
	BGBBTJ_JPG_TransDCT_Sride8(iblk+ystride*4, s+8*4, xstride, 1);
	BGBBTJ_JPG_TransDCT_Sride8(iblk+ystride*5, s+8*5, xstride, 1);
	BGBBTJ_JPG_TransDCT_Sride8(iblk+ystride*6, s+8*6, xstride, 1);
	BGBBTJ_JPG_TransDCT_Sride8(iblk+ystride*7, s+8*7, xstride, 1);

	BGBBTJ_JPG_TransDCT_Sride8(s+0, t+0, 8, 8);
	BGBBTJ_JPG_TransDCT_Sride8(s+1, t+1, 8, 8);
	BGBBTJ_JPG_TransDCT_Sride8(s+2, t+2, 8, 8);
	BGBBTJ_JPG_TransDCT_Sride8(s+3, t+3, 8, 8);
	BGBBTJ_JPG_TransDCT_Sride8(s+4, t+4, 8, 8);
	BGBBTJ_JPG_TransDCT_Sride8(s+5, t+5, 8, 8);
	BGBBTJ_JPG_TransDCT_Sride8(s+6, t+6, 8, 8);
	BGBBTJ_JPG_TransDCT_Sride8(s+7, t+7, 8, 8);

	for(i=0; i<64; i++)
		oblk[(i>>3)*ystride+(i&7)*xstride]=(t[i]+32768)>>16;
}
#endif

#if 1
#if 0
void BGBBTJ_JPG_TransIDCT_Sride8(int *iblk, int *oblk,
	int istride, int ostride)
{
	int ib[8], ob[8];

	ib[0]=iblk[istride*0];	ib[1]=iblk[istride*1];
	ib[2]=iblk[istride*2];	ib[3]=iblk[istride*3];
	ib[4]=iblk[istride*4];	ib[5]=iblk[istride*5];
	ib[6]=iblk[istride*6];	ib[7]=iblk[istride*7];

//IDCT8
//  91  126  118  106   91   71   49   25
//  91  106   49  -25  -91 -126 -118  -71
//  91   71  -49 -126  -91   25  118  106
//  91   25 -118  -71   91  106  -49 -126
//  91  -25 -118   71   91 -106  -49  126
//  91  -71  -49  126  -91  -25  118 -106
//  91 -106   49   25  -91  126 -118   71
//  91 -126  118 -106   91  -71   49  -25

	ob[0]=ib[0]*91+ib[1]*91+ib[2]*91+ib[3]*91+ib[4]*91+ib[5]*91+ib[6]*91+ib[7]*91;
	ob[1]=ib[0]*126+ib[1]*106+ib[2]*71+ib[3]*25-ib[4]*25-ib[5]*71-ib[6]*106 -ib[7]*126;
	ob[2]=ib[0]*118+ib[1]*49-ib[2]*49-ib[3]*118-ib[4]*118-ib[5]*49+ib[6]*49  +ib[7]*118;
	ob[3]=ib[0]*106-ib[1]*25-ib[2]*126-ib[3]*71+ib[4]*71+ib[5]*126+ib[6]*25  -ib[7]*106;
	ob[4]=ib[0]*91-ib[1]*91-ib[2]*91+ib[3]*91+ib[4]*91-ib[5]*91-ib[6]*91  +ib[7]*91;
	ob[5]=ib[0]*71-ib[1]*126+ib[2]*25+ib[3]*106-ib[4]*106-ib[5]*25+ib[6]*126 -ib[7]*71;
	ob[6]=ib[0]*49-ib[1]*118+ib[2]*118-ib[3]*49-ib[4]*49+ib[5]*118-ib[6]*118 +ib[7]*49;
	ob[7]=ib[0]*25-ib[1]*71+ib[2]*106-ib[3]*126+ib[4]*126-ib[5]*106+ib[6]*71  -ib[7]*25;
			
	oblk[ostride*0]=ob[0];	oblk[ostride*1]=ob[1];
	oblk[ostride*2]=ob[2];	oblk[ostride*3]=ob[3];
	oblk[ostride*4]=ob[4];	oblk[ostride*5]=ob[5];
	oblk[ostride*6]=ob[6];	oblk[ostride*7]=ob[7];
}
#endif

#if 1
void BGBBTJ_JPG_TransIDCT_Sride8(int *iblk, int *oblk,
	int istride, int ostride)
{
	int ib0, ib1, ib2, ib3, ib4, ib5, ib6, ib7;
	int a, b, c, d, e, f;
	int i, j, k, l;
	int m, n, o, p;

	ib0=iblk[istride*0];	ib1=iblk[istride*1];
	ib2=iblk[istride*2];	ib3=iblk[istride*3];
	ib4=iblk[istride*4];	ib5=iblk[istride*5];
	ib6=iblk[istride*6];	ib7=iblk[istride*7];

	a=ib0* 91;	b=ib4*91;
	c=ib2*118;	d=ib2*49;
	e=ib6*118;	f=ib6*49;
	i=a+c+b+f;	j=a+d-b-e;	k=a-d-b+e;	l=a-c+b-f;
	m=ib1*126+ib3*106+ib5* 71+ib7* 25;
	n=ib1*106-ib3* 25-ib5*126-ib7* 71;
	o=ib1* 71-ib3*126+ib5* 25+ib7*106;
	p=ib1* 25-ib3* 71+ib5*106-ib7*126;
	oblk[ostride*0]=i+m;	oblk[ostride*1]=j+n;
	oblk[ostride*2]=k+o;	oblk[ostride*3]=l+p;
	oblk[ostride*4]=l-p;	oblk[ostride*5]=k-o;
	oblk[ostride*6]=j-n;	oblk[ostride*7]=i-m;
}
#endif

void BGBBTJ_JPG_TransIDCT8Stride(int *iblk, int *oblk,
	int xstride, int ystride)
{
	int s[64], t[64];
	int i, j;

	BGBBTJ_JPG_TransIDCT_Sride8(iblk+ystride*0, s+8*0, xstride, 1);
	BGBBTJ_JPG_TransIDCT_Sride8(iblk+ystride*1, s+8*1, xstride, 1);
	BGBBTJ_JPG_TransIDCT_Sride8(iblk+ystride*2, s+8*2, xstride, 1);
	BGBBTJ_JPG_TransIDCT_Sride8(iblk+ystride*3, s+8*3, xstride, 1);
	BGBBTJ_JPG_TransIDCT_Sride8(iblk+ystride*4, s+8*4, xstride, 1);
	BGBBTJ_JPG_TransIDCT_Sride8(iblk+ystride*5, s+8*5, xstride, 1);
	BGBBTJ_JPG_TransIDCT_Sride8(iblk+ystride*6, s+8*6, xstride, 1);
	BGBBTJ_JPG_TransIDCT_Sride8(iblk+ystride*7, s+8*7, xstride, 1);

	BGBBTJ_JPG_TransIDCT_Sride8(s+0, t+0, 8, 8);
	BGBBTJ_JPG_TransIDCT_Sride8(s+1, t+1, 8, 8);
	BGBBTJ_JPG_TransIDCT_Sride8(s+2, t+2, 8, 8);
	BGBBTJ_JPG_TransIDCT_Sride8(s+3, t+3, 8, 8);
	BGBBTJ_JPG_TransIDCT_Sride8(s+4, t+4, 8, 8);
	BGBBTJ_JPG_TransIDCT_Sride8(s+5, t+5, 8, 8);
	BGBBTJ_JPG_TransIDCT_Sride8(s+6, t+6, 8, 8);
	BGBBTJ_JPG_TransIDCT_Sride8(s+7, t+7, 8, 8);

	for(i=0; i<64; i++)
		oblk[(i>>3)*ystride+(i&7)*xstride]=(t[i]+32768)>>16;
}
#endif


#if 1
void BGBBTJ_JPG_TransDCT_Sride16(int *iblk, int *oblk,
	int istride, int ostride)
{
	int ib0, ib1, ib2, ib3, ib4, ib5, ib6, ib7;
	int ob0, ob1, ob2, ob3, ob4, ob5, ob6, ob7;
	int ib8, ib9, ib10, ib11, ib12, ib13, ib14, ib15;
	int ob8, ob9, ob10, ob11, ob12, ob13, ob14, ob15;
	int ib0p, ib1p, ib2p, ib3p, ib4p, ib5p, ib6p, ib7p;
	int ib0n, ib1n, ib2n, ib3n, ib4n, ib5n, ib6n, ib7n;
	int ib0pp, ib1pp, ib2pp, ib3pp;
	int ib0pn, ib1pn, ib2pn, ib3pn;

	ib0 =iblk[istride* 0];	ib1 =iblk[istride* 1];
	ib2 =iblk[istride* 2];	ib3 =iblk[istride* 3];
	ib4 =iblk[istride* 4];	ib5 =iblk[istride* 5];
	ib6 =iblk[istride* 6];	ib7 =iblk[istride* 7];
	ib8 =iblk[istride* 8];	ib9 =iblk[istride* 9];
	ib10=iblk[istride*10];	ib11=iblk[istride*11];
	ib12=iblk[istride*12];	ib13=iblk[istride*13];
	ib14=iblk[istride*14];	ib15=iblk[istride*15];

	ib0p=ib0+ib15;	ib0n=ib0-ib15;
	ib1p=ib0+ib14;	ib1n=ib0-ib14;
	ib2p=ib0+ib13;	ib2n=ib0-ib13;
	ib3p=ib0+ib12;	ib3n=ib0-ib12;
	ib4p=ib0+ib11;	ib4n=ib0-ib11;
	ib5p=ib0+ib10;	ib5n=ib0-ib10;
	ib6p =ib0+ib9;	ib6n=ib0-ib9;
	ib7p =ib0+ib8;	ib7n=ib0-ib8;

	ib0pp=ib0p+ib7p;	ib0pn=ib0p-ib7p;
	ib1pp=ib1p+ib6p;	ib1pn=ib1p-ib6p;
	ib2pp=ib2p+ib5p;	ib2pn=ib2p-ib5p;
	ib3pp=ib3p+ib4p;	ib3pn=ib3p-ib4p;

#if 1
	ob0 =(ib0pp+ib1pp+ib2pp+ib3pp)*64;
 	ob8 =(ib0pp-ib1pp-ib2pp+ib3pp)*64;

 	ob2 =ib0pn*89+ib1pn*75+ib2pn*50+ib3pn*18;
 	ob4 =ib0pp*84+ib1pp*35-ib2pp*35-ib3pp*84;
 	ob6 =ib0pn*75-ib1pn*18-ib2pn*89-ib3pn*50;
 	ob10=ib0pn*50-ib1pn*89+ib2pn*18+ib3pn*75;
 	ob12=ib0pp*35-ib1pp*84+ib2pp*84-ib3pp*35;
 	ob14=ib0pn*18-ib1pn*50+ib2pn*75-ib3pn*89;

	ob1 =ib0n*90+ib1n*87+ib2n*80+ib3n*70+ib4n*57+ib5n*43+ib6n*26+ib7n* 9;
	ob3 =ib0n*87+ib1n*57+ib2n* 9-ib3n*43-ib4n*80-ib5n*90-ib6n*70-ib7n*26;
 	ob5 =ib0n*80+ib1n* 9-ib2n*70-ib3n*87-ib4n*26+ib5n*57+ib6n*90+ib7n*43;
 	ob7 =ib0n*70-ib1n*43-ib2n*87+ib3n* 9+ib4n*90+ib5n*26-ib6n*80-ib7n*57;
 	ob9 =ib0n*57-ib1n*80-ib2n*26+ib3n*90-ib4n* 9-ib5n*87+ib6n*43+ib7n*70;
 	ob11=ib0n*43-ib1n*90+ib2n*57+ib3n*26-ib4n*87+ib5n*70+ib6n* 9-ib7n*80;
 	ob13=ib0n*26-ib1n*70+ib2n*90-ib3n*80+ib4n*43+ib5n* 9-ib6n*57+ib7n*87;
 	ob15=ib0n* 9-ib1n*26+ib2n*43-ib3n*57+ib4n*70-ib5n*80+ib6n*87-ib7n*90;
#endif

#if 0
	ob0 =ib0p*64+ib1p*64+ib2p*64+ib3p*64+ib4p*64+ib5p*64+ib6p*64+ib7p*64;
	ob1 =ib0n*90+ib1n*87+ib2n*80+ib3n*70+ib4n*57+ib5n*43+ib6n*26+ib7n* 9;
 	ob2 =ib0p*89+ib1p*75+ib2p*50+ib3p*18-ib4p*18-ib5p*50-ib6p*75-ib7p*89;
	ob3 =ib0n*87+ib1n*57+ib2n* 9-ib3n*43-ib4n*80-ib5n*90-ib6n*70-ib7n*26;
 	ob4 =ib0p*84+ib1p*35-ib2p*35-ib3p*84-ib4p*84-ib5p*35+ib6p*35+ib7p*84;
 	ob5 =ib0n*80+ib1n* 9-ib2n*70-ib3n*87-ib4n*26+ib5n*57+ib6n*90+ib7n*43;
 	ob6 =ib0p*75-ib1p*18-ib2p*89-ib3p*50+ib4p*50+ib5p*89+ib6p*18-ib7p*75
 	ob7 =ib0n*70-ib1n*43-ib2n*87+ib3n* 9+ib4n*90+ib5n*26-ib6n*80-ib7n*57;
 	ob8 =ib0p*64-ib1p*64-ib2p*64+ib3p*64+ib4p*64-ib5p*64-ib6p*64+ib7p*64;
 	ob9 =ib0n*57-ib1n*80-ib2n*26+ib3n*90-ib4n* 9-ib5n*87+ib6n*43+ib7n*70;
 	ob10=ib0p*50-ib1p*89+ib2p*18+ib3p*75-ib4p*75-ib5p*18+ib6p*89-ib7p*50;
 	ob11=ib0n*43-ib1n*90+ib2n*57+ib3n*26-ib4n*87+ib5n*70+ib6n* 9-ib7n*80;
 	ob12=ib0p*35-ib1p*84+ib2p*84-ib3p*35-ib4p*35+ib5p*84-ib6p*84+ib7p*35;
 	ob13=ib0n*26-ib1n*70+ib2n*90-ib3n*80+ib4n*43+ib5n* 9-ib6n*57+ib7n*87;
 	ob14=ib0p*18-ib1p*50+ib2p*75-ib3p*89+ib4p*89-ib5p*75+ib6p*50-ib7p*18;
 	ob15=ib0n* 9-ib1n*26+ib2n*43-ib3n*57+ib4n*70-ib5n*80+ib6n*87-ib7n*90;
#endif

//	ob0 =ib0*64+ib1*64+ib2*64+ib3*64+ib4*64+ib5*64+ib6*64+ib7*64+ib8*64+ib9*64+ib10*64+ib11*64+ib12*64+ib13*64+ib14*64+ib15*64;
//	ob1 =ib0*90+ib1*87+ib2*80+ib3*70+ib4*57+ib5*43+ib6*26+ib7* 9-ib8* 9-ib9*26-ib10*43-ib11*57-ib12*70-ib13*80-ib14*87-ib15*90;
// 	ob2 =ib0*89+ib1*75+ib2*50+ib3*18-ib4*18-ib5*50-ib6*75-ib7*89-ib8*89-ib9*75-ib10*50-ib11*18+ib12*18+ib13*50+ib14*75+ib15*89;
//	ob3 =ib0*87+ib1*57+ib2* 9-ib3*43-ib4*80-ib5*90-ib6*70-ib7*26+ib8*26+ib9*70+ib10*90+ib11*80+ib12*43-ib13* 9-ib14*57-ib15*87;
// 	ob4 =ib0*84+ib1*35-ib2*35-ib3*84-ib4*84-ib5*35+ib6*35+ib7*84+ib8*84+ib9*35-ib10*35-ib11*84-ib12*84-ib13*35+ib14*35+ib15*84;
// 	ob5 =ib0*80+ib1* 9-ib2*70-ib3*87-ib4*26+ib5*57+ib6*90+ib7*43-ib8*43-ib9*90-ib10*57+ib11*26+ib12*87+ib13*70-ib14* 9-ib15*80;
// 	ob6 =ib0*75-ib1*18-ib2*89-ib3*50+ib4*50+ib5*89+ib6*18-ib7*75-ib8*75+ib9*18+ib10*89+ib11*50-ib12*50-ib13*89-ib14*18+ib15*75;
// 	ob7 =ib0*70-ib1*43-ib2*87+ib3* 9+ib4*90+ib5*26-ib6*80-ib7*57+ib8*57+ib9*80-ib10*26-ib11*90-ib12* 9+ib13*87+ib14*43-ib15*70;
// 	ob8 =ib0*64-ib1*64-ib2*64+ib3*64+ib4*64-ib5*64-ib6*64+ib7*64+ib8*64-ib9*64-ib10*64+ib11*64+ib12*64-ib13*64-ib14*64+ib15*64;
// 	ob9 =ib0*57-ib1*80-ib2*26+ib3*90-ib4* 9-ib5*87+ib6*43+ib7*70-ib8*70-ib9*43+ib10*87+ib11* 9-ib12*90+ib13*26+ib14*80-ib15*57;
// 	ob10=ib0*50-ib1*89+ib2*18+ib3*75-ib4*75-ib5*18+ib6*89-ib7*50-ib8*50+ib9*89-ib10*18-ib11*75+ib12*75+ib13*18-ib14*89+ib15*50;
// 	ob11=ib0*43-ib1*90+ib2*57+ib3*26-ib4*87+ib5*70+ib6* 9-ib7*80+ib8*80-ib9* 9-ib10*70+ib11*87-ib12*26-ib13*57+ib14*90-ib15*43;
// 	ob12=ib0*35-ib1*84+ib2*84-ib3*35-ib4*35+ib5*84-ib6*84+ib7*35+ib8*35-ib9*84+ib10*84-ib11*35-ib12*35+ib13*84-ib14*84+ib15*35;
// 	ob13=ib0*26-ib1*70+ib2*90-ib3*80+ib4*43+ib5* 9-ib6*57+ib7*87-ib8*87+ib9*57-ib10* 9-ib11*43+ib12*80-ib13*90+ib14*70-ib15*26;
// 	ob14=ib0*18-ib1*50+ib2*75-ib3*89+ib4*89-ib5*75+ib6*50-ib7*18-ib8*18+ib9*50-ib10*75+ib11*89-ib12*89+ib13*75-ib14*50+ib15*18;
// 	ob15=ib0* 9-ib1*26+ib2*43-ib3*57+ib4*70-ib5*80+ib6*87-ib7*90+ib8*90-ib9*87+ib10*80-ib11*70+ib12*57-ib13*43+ib14*26-ib15* 9;

	oblk[ostride* 0]=ob0;	oblk[ostride* 1]=ob1;
	oblk[ostride* 2]=ob2;	oblk[ostride* 3]=ob3;
	oblk[ostride* 4]=ob4;	oblk[ostride* 5]=ob5;
	oblk[ostride* 6]=ob6;	oblk[ostride* 7]=ob7;
	oblk[ostride* 8]=ob8;	oblk[ostride* 9]=ob9;
	oblk[ostride*10]=ob10;	oblk[ostride*11]=ob11;
	oblk[ostride*12]=ob12;	oblk[ostride*13]=ob13;
	oblk[ostride*14]=ob14;	oblk[ostride*15]=ob15;

#if 0
//	ib[ 0]=iblk[istride* 0];	ib[ 1]=iblk[istride* 1];
//	ib[ 2]=iblk[istride* 2];	ib[ 3]=iblk[istride* 3];
//	ib[ 4]=iblk[istride* 4];	ib[ 5]=iblk[istride* 5];
//	ib[ 6]=iblk[istride* 6];	ib[ 7]=iblk[istride* 7];
//	ib[ 8]=iblk[istride* 8];	ib[ 9]=iblk[istride* 9];
//	ib[10]=iblk[istride*10];	ib[11]=iblk[istride*11];
//	ib[12]=iblk[istride*12];	ib[13]=iblk[istride*13];
//	ib[14]=iblk[istride*14];	ib[15]=iblk[istride*15];

	ob[ 0]=ib[0]*64+ib[1]*64+ib[2]*64+ib[3]*64+ib[4]*64+ib[5]*64+ib[6]*64+ib[7]*64+ib[8]*64+ib[9]*64+ib[10]*64+ib[11]*64+ib[12]*64+ib[13]*64+ib[14]*64+ib[15]*64;
	ob[ 1]=ib[0]*90+ib[1]*87+ib[2]*80+ib[3]*70+ib[4]*57+ib[5]*43+ib[6]*26+ib[7]* 9-ib[8]* 9-ib[9]*26-ib[10]*43-ib[11]*57-ib[12]*70-ib[13]*80-ib[14]*87-ib[15]*90;
 	ob[ 2]=ib[0]*89+ib[1]*75+ib[2]*50+ib[3]*18-ib[4]*18-ib[5]*50-ib[6]*75-ib[7]*89-ib[8]*89-ib[9]*75-ib[10]*50-ib[11]*18+ib[12]*18+ib[13]*50+ib[14]*75+ib[15]*89;
 	ob[ 3]=ib[0]*87+ib[1]*57+ib[2]* 9-ib[3]*43-ib[4]*80-ib[5]*90-ib[6]*70-ib[7]*26+ib[8]*26+ib[9]*70+ib[10]*90+ib[11]*80+ib[12]*43-ib[13]* 9-ib[14]*57-ib[15]*87;
 	ob[ 4]=ib[0]*84+ib[1]*35-ib[2]*35-ib[3]*84-ib[4]*84-ib[5]*35+ib[6]*35+ib[7]*84+ib[8]*84+ib[9]*35-ib[10]*35-ib[11]*84-ib[12]*84-ib[13]*35+ib[14]*35+ib[15]*84;
 	ob[ 5]=ib[0]*80+ib[1]* 9-ib[2]*70-ib[3]*87-ib[4]*26+ib[5]*57+ib[6]*90+ib[7]*43-ib[8]*43-ib[9]*90-ib[10]*57+ib[11]*26+ib[12]*87+ib[13]*70-ib[14]* 9-ib[15]*80;
 	ob[ 6]=ib[0]*75-ib[1]*18-ib[2]*89-ib[3]*50+ib[4]*50+ib[5]*89+ib[6]*18-ib[7]*75-ib[8]*75+ib[9]*18+ib[10]*89+ib[11]*50-ib[12]*50-ib[13]*89-ib[14]*18+ib[15]*75;
 	ob[ 7]=ib[0]*70-ib[1]*43-ib[2]*87+ib[3]* 9+ib[4]*90+ib[5]*26-ib[6]*80-ib[7]*57+ib[8]*57+ib[9]*80-ib[10]*26-ib[11]*90-ib[12]* 9+ib[13]*87+ib[14]*43-ib[15]*70;
 	ob[ 8]=ib[0]*64-ib[1]*64-ib[2]*64+ib[3]*64+ib[4]*64-ib[5]*64-ib[6]*64+ib[7]*64+ib[8]*64-ib[9]*64-ib[10]*64+ib[11]*64+ib[12]*64-ib[13]*64-ib[14]*64+ib[15]*64;
 	ob[ 9]=ib[0]*57-ib[1]*80-ib[2]*26+ib[3]*90-ib[4]* 9-ib[5]*87+ib[6]*43+ib[7]*70-ib[8]*70-ib[9]*43+ib[10]*87+ib[11]* 9-ib[12]*90+ib[13]*26+ib[14]*80-ib[15]*57;
 	ob[10]=ib[0]*50-ib[1]*89+ib[2]*18+ib[3]*75-ib[4]*75-ib[5]*18+ib[6]*89-ib[7]*50-ib[8]*50+ib[9]*89-ib[10]*18-ib[11]*75+ib[12]*75+ib[13]*18-ib[14]*89+ib[15]*50;
 	ob[11]=ib[0]*43-ib[1]*90+ib[2]*57+ib[3]*26-ib[4]*87+ib[5]*70+ib[6]* 9-ib[7]*80+ib[8]*80-ib[9]* 9-ib[10]*70+ib[11]*87-ib[12]*26-ib[13]*57+ib[14]*90-ib[15]*43;
 	ob[12]=ib[0]*35-ib[1]*84+ib[2]*84-ib[3]*35-ib[4]*35+ib[5]*84-ib[6]*84+ib[7]*35+ib[8]*35-ib[9]*84+ib[10]*84-ib[11]*35-ib[12]*35+ib[13]*84-ib[14]*84+ib[15]*35;
 	ob[13]=ib[0]*26-ib[1]*70+ib[2]*90-ib[3]*80+ib[4]*43+ib[5]* 9-ib[6]*57+ib[7]*87-ib[8]*87+ib[9]*57-ib[10]* 9-ib[11]*43+ib[12]*80-ib[13]*90+ib[14]*70-ib[15]*26;
 	ob[14]=ib[0]*18-ib[1]*50+ib[2]*75-ib[3]*89+ib[4]*89-ib[5]*75+ib[6]*50-ib[7]*18-ib[8]*18+ib[9]*50-ib[10]*75+ib[11]*89-ib[12]*89+ib[13]*75-ib[14]*50+ib[15]*18;
  	ob[15]=ib[0]* 9-ib[1]*26+ib[2]*43-ib[3]*57+ib[4]*70-ib[5]*80+ib[6]*87-ib[7]*90+ib[8]*90-ib[9]*87+ib[10]*80-ib[11]*70+ib[12]*57-ib[13]*43+ib[14]*26-ib[15]* 9;
			
	oblk[ostride* 0]=ob[ 0];	oblk[ostride* 1]=ob[ 1];
	oblk[ostride* 2]=ob[ 2];	oblk[ostride* 3]=ob[ 3];
	oblk[ostride* 4]=ob[ 4];	oblk[ostride* 5]=ob[ 5];
	oblk[ostride* 6]=ob[ 6];	oblk[ostride* 7]=ob[ 7];
	oblk[ostride* 8]=ob[ 8];	oblk[ostride* 9]=ob[ 9];
	oblk[ostride*10]=ob[10];	oblk[ostride*11]=ob[11];
	oblk[ostride*12]=ob[12];	oblk[ostride*13]=ob[13];
	oblk[ostride*14]=ob[14];	oblk[ostride*15]=ob[15];
#endif
}

void BGBBTJ_JPG_TransDCT16Stride(int *iblk, int *oblk,
	int xstride, int ystride)
{
	int s[256], t[256];
	int i, j;

	BGBBTJ_JPG_TransDCT_Sride16(iblk+ystride* 0, s+16* 0, xstride, 1);
	BGBBTJ_JPG_TransDCT_Sride16(iblk+ystride* 1, s+16* 1, xstride, 1);
	BGBBTJ_JPG_TransDCT_Sride16(iblk+ystride* 2, s+16* 2, xstride, 1);
	BGBBTJ_JPG_TransDCT_Sride16(iblk+ystride* 3, s+16* 3, xstride, 1);
	BGBBTJ_JPG_TransDCT_Sride16(iblk+ystride* 4, s+16* 4, xstride, 1);
	BGBBTJ_JPG_TransDCT_Sride16(iblk+ystride* 5, s+16* 5, xstride, 1);
	BGBBTJ_JPG_TransDCT_Sride16(iblk+ystride* 6, s+16* 6, xstride, 1);
	BGBBTJ_JPG_TransDCT_Sride16(iblk+ystride* 7, s+16* 7, xstride, 1);
	BGBBTJ_JPG_TransDCT_Sride16(iblk+ystride* 8, s+16* 8, xstride, 1);
	BGBBTJ_JPG_TransDCT_Sride16(iblk+ystride* 9, s+16* 9, xstride, 1);
	BGBBTJ_JPG_TransDCT_Sride16(iblk+ystride*10, s+16*10, xstride, 1);
	BGBBTJ_JPG_TransDCT_Sride16(iblk+ystride*11, s+16*11, xstride, 1);
	BGBBTJ_JPG_TransDCT_Sride16(iblk+ystride*12, s+16*12, xstride, 1);
	BGBBTJ_JPG_TransDCT_Sride16(iblk+ystride*13, s+16*13, xstride, 1);
	BGBBTJ_JPG_TransDCT_Sride16(iblk+ystride*14, s+16*14, xstride, 1);
	BGBBTJ_JPG_TransDCT_Sride16(iblk+ystride*15, s+16*15, xstride, 1);

	BGBBTJ_JPG_TransDCT_Sride16(s+ 0, t+ 0, 16, 16);
	BGBBTJ_JPG_TransDCT_Sride16(s+ 1, t+ 1, 16, 16);
	BGBBTJ_JPG_TransDCT_Sride16(s+ 2, t+ 2, 16, 16);
	BGBBTJ_JPG_TransDCT_Sride16(s+ 3, t+ 3, 16, 16);
	BGBBTJ_JPG_TransDCT_Sride16(s+ 4, t+ 4, 16, 16);
	BGBBTJ_JPG_TransDCT_Sride16(s+ 5, t+ 5, 16, 16);
	BGBBTJ_JPG_TransDCT_Sride16(s+ 6, t+ 6, 16, 16);
	BGBBTJ_JPG_TransDCT_Sride16(s+ 7, t+ 7, 16, 16);
	BGBBTJ_JPG_TransDCT_Sride16(s+ 8, t+ 8, 16, 16);
	BGBBTJ_JPG_TransDCT_Sride16(s+ 9, t+ 9, 16, 16);
	BGBBTJ_JPG_TransDCT_Sride16(s+10, t+10, 16, 16);
	BGBBTJ_JPG_TransDCT_Sride16(s+11, t+11, 16, 16);
	BGBBTJ_JPG_TransDCT_Sride16(s+12, t+12, 16, 16);
	BGBBTJ_JPG_TransDCT_Sride16(s+13, t+13, 16, 16);
	BGBBTJ_JPG_TransDCT_Sride16(s+14, t+14, 16, 16);
	BGBBTJ_JPG_TransDCT_Sride16(s+15, t+15, 16, 16);

	for(i=0; i<256; i++)
		oblk[(i>>4)*ystride+(i&15)*xstride]=(t[i]+32768)>>16;
}
#endif


#if 1
void BGBBTJ_JPG_TransIDCT_Sride16(int *iblk, int *oblk,
	int istride, int ostride)
{
//	int ib[16], ob[16];

	int ib0, ib1, ib2, ib3, ib4, ib5, ib6, ib7;
	int ob0, ob1, ob2, ob3, ob4, ob5, ob6, ob7;
	int ib8, ib9, ib10, ib11, ib12, ib13, ib14, ib15;
	int ob8, ob9, ob10, ob11, ob12, ob13, ob14, ob15;

	ib0 =iblk[istride* 0];	ib1 =iblk[istride* 1];
	ib2 =iblk[istride* 2];	ib3 =iblk[istride* 3];
	ib4 =iblk[istride* 4];	ib5 =iblk[istride* 5];
	ib6 =iblk[istride* 6];	ib7 =iblk[istride* 7];
	ib8 =iblk[istride* 8];	ib9 =iblk[istride* 9];
	ib10=iblk[istride*10];	ib11=iblk[istride*11];
	ib12=iblk[istride*12];	ib13=iblk[istride*13];
	ib14=iblk[istride*14];	ib15=iblk[istride*15];

	ob0 =ib0*64+ib1*90+ib2*89+ib3*87+ib4*84+ib5*80+ib6*75+ib7*70+ib8*64+ib9*57+ib10*50+ib11*43+ib12*35+ib13*26+ib14*18+ib15* 9;
 	ob1 =ib0*64+ib1*87+ib2*75+ib3*57+ib4*35+ib5* 9-ib6*18-ib7*43-ib8*64-ib9*80-ib10*89-ib11*90-ib12*84-ib13*70-ib14*50-ib15*26;
 	ob2 =ib0*64+ib1*80+ib2*50+ib3* 9-ib4*35-ib5*70-ib6*89-ib7*87-ib8*64-ib9*26+ib10*18+ib11*57+ib12*84+ib13*90+ib14*75+ib15*43;
 	ob3 =ib0*64+ib1*70+ib2*18-ib3*43-ib4*84-ib5*87-ib6*50+ib7* 9+ib8*64+ib9*90+ib10*75+ib11*26-ib12*35-ib13*80-ib14*89-ib15*57;
 	ob4 =ib0*64+ib1*57-ib2*18-ib3*80-ib4*84-ib5*26+ib6*50+ib7*90+ib8*64-ib9* 9-ib10*75-ib11*87-ib12*35+ib13*43+ib14*89+ib15*70;
 	ob5 =ib0*64+ib1*43-ib2*50-ib3*90-ib4*35+ib5*57+ib6*89+ib7*26-ib8*64-ib9*87-ib10*18+ib11*70+ib12*84+ib13* 9-ib14*75-ib15*80;
 	ob6 =ib0*64+ib1*26-ib2*75-ib3*70+ib4*35+ib5*90+ib6*18-ib7*80-ib8*64+ib9*43+ib10*89+ib11* 9-ib12*84-ib13*57+ib14*50+ib15*87;
 	ob7 =ib0*64+ib1* 9-ib2*89-ib3*26+ib4*84+ib5*43-ib6*75-ib7*57+ib8*64+ib9*70-ib10*50-ib11*80+ib12*35+ib13*87-ib14*18-ib15*90;
 	ob8 =ib0*64-ib1* 9-ib2*89+ib3*26+ib4*84-ib5*43-ib6*75+ib7*57+ib8*64-ib9*70-ib10*50+ib11*80+ib12*35-ib13*87-ib14*18+ib15*90;
 	ob9 =ib0*64-ib1*26-ib2*75+ib3*70+ib4*35-ib5*90+ib6*18+ib7*80-ib8*64-ib9*43+ib10*89-ib11* 9-ib12*84+ib13*57+ib14*50-ib15*87;
 	ob10=ib0*64-ib1*43-ib2*50+ib3*90-ib4*35-ib5*57+ib6*89-ib7*26-ib8*64+ib9*87-ib10*18-ib11*70+ib12*84-ib13* 9-ib14*75+ib15*80;
 	ob11=ib0*64-ib1*57-ib2*18+ib3*80-ib4*84+ib5*26+ib6*50-ib7*90+ib8*64+ib9* 9-ib10*75+ib11*87-ib12*35-ib13*43+ib14*89-ib15*70;
 	ob12=ib0*64-ib1*70+ib2*18+ib3*43-ib4*84+ib5*87-ib6*50-ib7* 9+ib8*64-ib9*90+ib10*75-ib11*26-ib12*35+ib13*80-ib14*89+ib15*57;
 	ob13=ib0*64-ib1*80+ib2*50-ib3* 9-ib4*35+ib5*70-ib6*89+ib7*87-ib8*64+ib9*26+ib10*18-ib11*57+ib12*84-ib13*90+ib14*75-ib15*43;
 	ob14=ib0*64-ib1*87+ib2*75-ib3*57+ib4*35-ib5* 9-ib6*18+ib7*43-ib8*64+ib9*80-ib10*89+ib11*90-ib12*84+ib13*70-ib14*50+ib15*26;
 	ob15=ib0*64-ib1*90+ib2*89-ib3*87+ib4*84-ib5*80+ib6*75-ib7*70+ib8*64-ib9*57+ib10*50-ib11*43+ib12*35-ib13*26+ib14*18-ib15* 9;

//	ob[ 0]=ib0*64+ib[1]*64+ib[2]*64+ib[3]*64+ib[4]*64+ib[5]*64+ib[6]*64+ib[7]*64+ib[8]*64+ib[9]*64+ib[10]*64+ib[11]*64+ib[12]*64+ib[13]*64+ib[14]*64+ib[15]*64;
//	ob[ 1]=ib0*90+ib[1]*87+ib[2]*80+ib[3]*70+ib[4]*57+ib[5]*43+ib[6]*26+ib[7]* 9-ib[8]* 9-ib[9]*26-ib[10]*43-ib[11]*57-ib[12]*70-ib[13]*80-ib[14]*87-ib[15]*90;
// 	ob[ 2]=ib0*89+ib[1]*75+ib[2]*50+ib[3]*18-ib[4]*18-ib[5]*50-ib[6]*75-ib[7]*89-ib[8]*89-ib[9]*75-ib[10]*50-ib[11]*18+ib[12]*18+ib[13]*50+ib[14]*75+ib[15]*89;
// 	ob[ 3]=ib0*87+ib[1]*57+ib[2]* 9-ib[3]*43-ib[4]*80-ib[5]*90-ib[6]*70-ib[7]*26+ib[8]*26+ib[9]*70+ib[10]*90+ib[11]*80+ib[12]*43-ib[13]* 9-ib[14]*57-ib[15]*87;
// 	ob[ 4]=ib0*84+ib[1]*35-ib[2]*35-ib[3]*84-ib[4]*84-ib[5]*35+ib[6]*35+ib[7]*84+ib[8]*84+ib[9]*35-ib[10]*35-ib[11]*84-ib[12]*84-ib[13]*35+ib[14]*35+ib[15]*84;
// 	ob[ 5]=ib0*80+ib[1]* 9-ib[2]*70-ib[3]*87-ib[4]*26+ib[5]*57+ib[6]*90+ib[7]*43-ib[8]*43-ib[9]*90-ib[10]*57+ib[11]*26+ib[12]*87+ib[13]*70-ib[14]* 9-ib[15]*80;
// 	ob[ 6]=ib0*75-ib[1]*18-ib[2]*89-ib[3]*50+ib[4]*50+ib[5]*89+ib[6]*18-ib[7]*75-ib[8]*75+ib[9]*18+ib[10]*89+ib[11]*50-ib[12]*50-ib[13]*89-ib[14]*18+ib[15]*75;
// 	ob[ 7]=ib0*70-ib[1]*43-ib[2]*87+ib[3]* 9+ib[4]*90+ib[5]*26-ib[6]*80-ib[7]*57+ib[8]*57+ib[9]*80-ib[10]*26-ib[11]*90-ib[12]* 9+ib[13]*87+ib[14]*43-ib[15]*70;
// 	ob[ 8]=ib0*64-ib[1]*64-ib[2]*64+ib[3]*64+ib[4]*64-ib[5]*64-ib[6]*64+ib[7]*64+ib[8]*64-ib[9]*64-ib[10]*64+ib[11]*64+ib[12]*64-ib[13]*64-ib[14]*64+ib[15]*64;
// 	ob[ 9]=ib0*57-ib[1]*80-ib[2]*26+ib[3]*90-ib[4]* 9-ib[5]*87+ib[6]*43+ib[7]*70-ib[8]*70-ib[9]*43+ib[10]*87+ib[11]* 9-ib[12]*90+ib[13]*26+ib[14]*80-ib[15]*57;
// 	ob[10]=ib0*50-ib[1]*89+ib[2]*18+ib[3]*75-ib[4]*75-ib[5]*18+ib[6]*89-ib[7]*50-ib[8]*50+ib[9]*89-ib[10]*18-ib[11]*75+ib[12]*75+ib[13]*18-ib[14]*89+ib[15]*50;
// 	ob[11]=ib0*43-ib[1]*90+ib[2]*57+ib[3]*26-ib[4]*87+ib[5]*70+ib[6]* 9-ib[7]*80+ib[8]*80-ib[9]* 9-ib[10]*70+ib[11]*87-ib[12]*26-ib[13]*57+ib[14]*90-ib[15]*43;
// 	ob[12]=ib0*35-ib[1]*84+ib[2]*84-ib[3]*35-ib[4]*35+ib[5]*84-ib[6]*84+ib[7]*35+ib[8]*35-ib[9]*84+ib[10]*84-ib[11]*35-ib[12]*35+ib[13]*84-ib[14]*84+ib[15]*35;
// 	ob[13]=ib0*26-ib[1]*70+ib[2]*90-ib[3]*80+ib[4]*43+ib[5]* 9-ib[6]*57+ib[7]*87-ib[8]*87+ib[9]*57-ib[10]* 9-ib[11]*43+ib[12]*80-ib[13]*90+ib[14]*70-ib[15]*26;
// 	ob[14]=ib0*18-ib[1]*50+ib[2]*75-ib[3]*89+ib[4]*89-ib[5]*75+ib[6]*50-ib[7]*18-ib[8]*18+ib[9]*50-ib[10]*75+ib[11]*89-ib[12]*89+ib[13]*75-ib[14]*50+ib[15]*18;
// 	ob[15]=ib0* 9-ib[1]*26+ib[2]*43-ib[3]*57+ib[4]*70-ib[5]*80+ib[6]*87-ib[7]*90+ib[8]*90-ib[9]*87+ib[10]*80-ib[11]*70+ib[12]*57-ib[13]*43+ib[14]*26-ib[15]* 9;
			
	oblk[ostride* 0]=ob0;	oblk[ostride* 1]=ob1;
	oblk[ostride* 2]=ob2;	oblk[ostride* 3]=ob3;
	oblk[ostride* 4]=ob4;	oblk[ostride* 5]=ob5;
	oblk[ostride* 6]=ob6;	oblk[ostride* 7]=ob7;
	oblk[ostride* 8]=ob8;	oblk[ostride* 9]=ob9;
	oblk[ostride*10]=ob10;	oblk[ostride*11]=ob11;
	oblk[ostride*12]=ob12;	oblk[ostride*13]=ob13;
	oblk[ostride*14]=ob14;	oblk[ostride*15]=ob15;
}

void BGBBTJ_JPG_TransIDCT16Stride(int *iblk, int *oblk,
	int xstride, int ystride)
{
	int s[256], t[256];
	int i, j;

	BGBBTJ_JPG_TransIDCT_Sride16(iblk+ystride* 0, s+16* 0, xstride, 1);
	BGBBTJ_JPG_TransIDCT_Sride16(iblk+ystride* 1, s+16* 1, xstride, 1);
	BGBBTJ_JPG_TransIDCT_Sride16(iblk+ystride* 2, s+16* 2, xstride, 1);
	BGBBTJ_JPG_TransIDCT_Sride16(iblk+ystride* 3, s+16* 3, xstride, 1);
	BGBBTJ_JPG_TransIDCT_Sride16(iblk+ystride* 4, s+16* 4, xstride, 1);
	BGBBTJ_JPG_TransIDCT_Sride16(iblk+ystride* 5, s+16* 5, xstride, 1);
	BGBBTJ_JPG_TransIDCT_Sride16(iblk+ystride* 6, s+16* 6, xstride, 1);
	BGBBTJ_JPG_TransIDCT_Sride16(iblk+ystride* 7, s+16* 7, xstride, 1);
	BGBBTJ_JPG_TransIDCT_Sride16(iblk+ystride* 8, s+16* 8, xstride, 1);
	BGBBTJ_JPG_TransIDCT_Sride16(iblk+ystride* 9, s+16* 9, xstride, 1);
	BGBBTJ_JPG_TransIDCT_Sride16(iblk+ystride*10, s+16*10, xstride, 1);
	BGBBTJ_JPG_TransIDCT_Sride16(iblk+ystride*11, s+16*11, xstride, 1);
	BGBBTJ_JPG_TransIDCT_Sride16(iblk+ystride*12, s+16*12, xstride, 1);
	BGBBTJ_JPG_TransIDCT_Sride16(iblk+ystride*13, s+16*13, xstride, 1);
	BGBBTJ_JPG_TransIDCT_Sride16(iblk+ystride*14, s+16*14, xstride, 1);
	BGBBTJ_JPG_TransIDCT_Sride16(iblk+ystride*15, s+16*15, xstride, 1);

	BGBBTJ_JPG_TransIDCT_Sride16(s+ 0, t+ 0, 16, 16);
	BGBBTJ_JPG_TransIDCT_Sride16(s+ 1, t+ 1, 16, 16);
	BGBBTJ_JPG_TransIDCT_Sride16(s+ 2, t+ 2, 16, 16);
	BGBBTJ_JPG_TransIDCT_Sride16(s+ 3, t+ 3, 16, 16);
	BGBBTJ_JPG_TransIDCT_Sride16(s+ 4, t+ 4, 16, 16);
	BGBBTJ_JPG_TransIDCT_Sride16(s+ 5, t+ 5, 16, 16);
	BGBBTJ_JPG_TransIDCT_Sride16(s+ 6, t+ 6, 16, 16);
	BGBBTJ_JPG_TransIDCT_Sride16(s+ 7, t+ 7, 16, 16);
	BGBBTJ_JPG_TransIDCT_Sride16(s+ 8, t+ 8, 16, 16);
	BGBBTJ_JPG_TransIDCT_Sride16(s+ 9, t+ 9, 16, 16);
	BGBBTJ_JPG_TransIDCT_Sride16(s+10, t+10, 16, 16);
	BGBBTJ_JPG_TransIDCT_Sride16(s+11, t+11, 16, 16);
	BGBBTJ_JPG_TransIDCT_Sride16(s+12, t+12, 16, 16);
	BGBBTJ_JPG_TransIDCT_Sride16(s+13, t+13, 16, 16);
	BGBBTJ_JPG_TransIDCT_Sride16(s+14, t+14, 16, 16);
	BGBBTJ_JPG_TransIDCT_Sride16(s+15, t+15, 16, 16);

	for(i=0; i<256; i++)
		oblk[(i>>4)*ystride+(i&15)*xstride]=(t[i]+32768)>>16;
}
#endif
