#include <bgbmid.h>

#if 1
void BTAC2_TransDCT_Stride4(int *iblk, int *oblk,
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

void BTAC2_TransDCT4Stride(int *iblk, int *oblk,
	int xstride, int ystride)
{
	int s[16], t[16];
	int i, j;

	BTAC2_TransDCT_Stride4(iblk+ystride*0, s+4*0, xstride, 1);
	BTAC2_TransDCT_Stride4(iblk+ystride*1, s+4*1, xstride, 1);
	BTAC2_TransDCT_Stride4(iblk+ystride*2, s+4*2, xstride, 1);
	BTAC2_TransDCT_Stride4(iblk+ystride*3, s+4*3, xstride, 1);

	BTAC2_TransDCT_Stride4(s+0, t+0, 4, 4);
	BTAC2_TransDCT_Stride4(s+1, t+1, 4, 4);
	BTAC2_TransDCT_Stride4(s+2, t+2, 4, 4);
	BTAC2_TransDCT_Stride4(s+3, t+3, 4, 4);

	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		oblk[i*ystride+j*xstride]=(t[i*4+j]+32768)>>16;
	}
}

#endif

#if 1
void BTAC2_TransIDCT_Stride4(int *iblk, int *oblk,
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

void BTAC2_TransIDCT4Stride(int *iblk, int *oblk,
	int xstride, int ystride)
{
	int s[16], t[16];
	int i, j;

	BTAC2_TransIDCT_Stride4(iblk+ystride*0, s+4*0, xstride, 1);
	BTAC2_TransIDCT_Stride4(iblk+ystride*1, s+4*1, xstride, 1);
	BTAC2_TransIDCT_Stride4(iblk+ystride*2, s+4*2, xstride, 1);
	BTAC2_TransIDCT_Stride4(iblk+ystride*3, s+4*3, xstride, 1);

	BTAC2_TransIDCT_Stride4(s+0, t+0, 4, 4);
	BTAC2_TransIDCT_Stride4(s+1, t+1, 4, 4);
	BTAC2_TransIDCT_Stride4(s+2, t+2, 4, 4);
	BTAC2_TransIDCT_Stride4(s+3, t+3, 4, 4);

	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		oblk[i*ystride+j*xstride]=(t[i*4+j]+32768)>>16;
	}
}
#endif

#if 1
void BTAC2_TransDCT_Stride8(int *iblk, int *oblk,
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

void BTAC2_TransDCT8Stride(int *iblk, int *oblk,
	int xstride, int ystride)
{
	int s[64], t[64];
	int i, j;

	BTAC2_TransDCT_Stride8(iblk+ystride*0, s+8*0, xstride, 1);
	BTAC2_TransDCT_Stride8(iblk+ystride*1, s+8*1, xstride, 1);
	BTAC2_TransDCT_Stride8(iblk+ystride*2, s+8*2, xstride, 1);
	BTAC2_TransDCT_Stride8(iblk+ystride*3, s+8*3, xstride, 1);
	BTAC2_TransDCT_Stride8(iblk+ystride*4, s+8*4, xstride, 1);
	BTAC2_TransDCT_Stride8(iblk+ystride*5, s+8*5, xstride, 1);
	BTAC2_TransDCT_Stride8(iblk+ystride*6, s+8*6, xstride, 1);
	BTAC2_TransDCT_Stride8(iblk+ystride*7, s+8*7, xstride, 1);

	BTAC2_TransDCT_Stride8(s+0, t+0, 8, 8);
	BTAC2_TransDCT_Stride8(s+1, t+1, 8, 8);
	BTAC2_TransDCT_Stride8(s+2, t+2, 8, 8);
	BTAC2_TransDCT_Stride8(s+3, t+3, 8, 8);
	BTAC2_TransDCT_Stride8(s+4, t+4, 8, 8);
	BTAC2_TransDCT_Stride8(s+5, t+5, 8, 8);
	BTAC2_TransDCT_Stride8(s+6, t+6, 8, 8);
	BTAC2_TransDCT_Stride8(s+7, t+7, 8, 8);

	for(i=0; i<64; i++)
		oblk[(i>>3)*ystride+(i&7)*xstride]=(t[i]+32768)>>16;
}
#endif

#if 1
#if 0
void BTAC2_TransIDCT_Stride8(int *iblk, int *oblk,
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
void BTAC2_TransIDCT_Stride8(int *iblk, int *oblk,
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

void BTAC2_TransIDCT8Stride(int *iblk, int *oblk,
	int xstride, int ystride)
{
	int s[64], t[64];
	int i, j;

	BTAC2_TransIDCT_Stride8(iblk+ystride*0, s+8*0, xstride, 1);
	BTAC2_TransIDCT_Stride8(iblk+ystride*1, s+8*1, xstride, 1);
	BTAC2_TransIDCT_Stride8(iblk+ystride*2, s+8*2, xstride, 1);
	BTAC2_TransIDCT_Stride8(iblk+ystride*3, s+8*3, xstride, 1);
	BTAC2_TransIDCT_Stride8(iblk+ystride*4, s+8*4, xstride, 1);
	BTAC2_TransIDCT_Stride8(iblk+ystride*5, s+8*5, xstride, 1);
	BTAC2_TransIDCT_Stride8(iblk+ystride*6, s+8*6, xstride, 1);
	BTAC2_TransIDCT_Stride8(iblk+ystride*7, s+8*7, xstride, 1);

	BTAC2_TransIDCT_Stride8(s+0, t+0, 8, 8);
	BTAC2_TransIDCT_Stride8(s+1, t+1, 8, 8);
	BTAC2_TransIDCT_Stride8(s+2, t+2, 8, 8);
	BTAC2_TransIDCT_Stride8(s+3, t+3, 8, 8);
	BTAC2_TransIDCT_Stride8(s+4, t+4, 8, 8);
	BTAC2_TransIDCT_Stride8(s+5, t+5, 8, 8);
	BTAC2_TransIDCT_Stride8(s+6, t+6, 8, 8);
	BTAC2_TransIDCT_Stride8(s+7, t+7, 8, 8);

	for(i=0; i<64; i++)
		oblk[(i>>3)*ystride+(i&7)*xstride]=(t[i]+32768)>>16;
}
#endif


#if 1
void BTAC2_TransDCT_Stride16(s64 *iblk, s64 *oblk,
	int istride, int ostride)
{
	s64 ib0, ib1, ib2, ib3, ib4, ib5, ib6, ib7;
	s64 ob0, ob1, ob2, ob3, ob4, ob5, ob6, ob7;
	s64 ib8, ib9, ib10, ib11, ib12, ib13, ib14, ib15;
	s64 ob8, ob9, ob10, ob11, ob12, ob13, ob14, ob15;
	s64 ib0p, ib1p, ib2p, ib3p, ib4p, ib5p, ib6p, ib7p;
	s64 ib0n, ib1n, ib2n, ib3n, ib4n, ib5n, ib6n, ib7n;
	s64 ib0pp, ib1pp, ib2pp, ib3pp;
	s64 ib0pn, ib1pn, ib2pn, ib3pn;

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
 	ob6 =ib0p*75-ib1p*18-ib2p*89-ib3p*50+ib4p*50+ib5p*89+ib6p*18-ib7p*75;
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

	oblk[ostride* 0]=ob0;	oblk[ostride* 1]=ob1;
	oblk[ostride* 2]=ob2;	oblk[ostride* 3]=ob3;
	oblk[ostride* 4]=ob4;	oblk[ostride* 5]=ob5;
	oblk[ostride* 6]=ob6;	oblk[ostride* 7]=ob7;
	oblk[ostride* 8]=ob8;	oblk[ostride* 9]=ob9;
	oblk[ostride*10]=ob10;	oblk[ostride*11]=ob11;
	oblk[ostride*12]=ob12;	oblk[ostride*13]=ob13;
	oblk[ostride*14]=ob14;	oblk[ostride*15]=ob15;
}

void BTAC2_TransDCT16Stride(int *iblk, int *oblk,
	int xstride, int ystride)
{
	s64 s[256], t[256];
	int i, j;

	for(i=0; i<256; i++)
		s[i]=iblk[(i>>4)*ystride+(i&15)*xstride];

	BTAC2_TransDCT_Stride16(s+16* 0, s+16* 0, 1, 1);
	BTAC2_TransDCT_Stride16(s+16* 1, s+16* 1, 1, 1);
	BTAC2_TransDCT_Stride16(s+16* 2, s+16* 2, 1, 1);
	BTAC2_TransDCT_Stride16(s+16* 3, s+16* 3, 1, 1);
	BTAC2_TransDCT_Stride16(s+16* 4, s+16* 4, 1, 1);
	BTAC2_TransDCT_Stride16(s+16* 5, s+16* 5, 1, 1);
	BTAC2_TransDCT_Stride16(s+16* 6, s+16* 6, 1, 1);
	BTAC2_TransDCT_Stride16(s+16* 7, s+16* 7, 1, 1);
	BTAC2_TransDCT_Stride16(s+16* 8, s+16* 8, 1, 1);
	BTAC2_TransDCT_Stride16(s+16* 9, s+16* 9, 1, 1);
	BTAC2_TransDCT_Stride16(s+16*10, s+16*10, 1, 1);
	BTAC2_TransDCT_Stride16(s+16*11, s+16*11, 1, 1);
	BTAC2_TransDCT_Stride16(s+16*12, s+16*12, 1, 1);
	BTAC2_TransDCT_Stride16(s+16*13, s+16*13, 1, 1);
	BTAC2_TransDCT_Stride16(s+16*14, s+16*14, 1, 1);
	BTAC2_TransDCT_Stride16(s+16*15, s+16*15, 1, 1);

	BTAC2_TransDCT_Stride16(s+ 0, t+ 0, 16, 16);
	BTAC2_TransDCT_Stride16(s+ 1, t+ 1, 16, 16);
	BTAC2_TransDCT_Stride16(s+ 2, t+ 2, 16, 16);
	BTAC2_TransDCT_Stride16(s+ 3, t+ 3, 16, 16);
	BTAC2_TransDCT_Stride16(s+ 4, t+ 4, 16, 16);
	BTAC2_TransDCT_Stride16(s+ 5, t+ 5, 16, 16);
	BTAC2_TransDCT_Stride16(s+ 6, t+ 6, 16, 16);
	BTAC2_TransDCT_Stride16(s+ 7, t+ 7, 16, 16);
	BTAC2_TransDCT_Stride16(s+ 8, t+ 8, 16, 16);
	BTAC2_TransDCT_Stride16(s+ 9, t+ 9, 16, 16);
	BTAC2_TransDCT_Stride16(s+10, t+10, 16, 16);
	BTAC2_TransDCT_Stride16(s+11, t+11, 16, 16);
	BTAC2_TransDCT_Stride16(s+12, t+12, 16, 16);
	BTAC2_TransDCT_Stride16(s+13, t+13, 16, 16);
	BTAC2_TransDCT_Stride16(s+14, t+14, 16, 16);
	BTAC2_TransDCT_Stride16(s+15, t+15, 16, 16);

	for(i=0; i<256; i++)
		oblk[(i>>4)*ystride+(i&15)*xstride]=(t[i]+32768)>>16;
}
#endif


#if 1
void BTAC2_TransIDCT_Stride16(s64 *iblk, s64 *oblk,
	int istride, int ostride)
{
//	int ib[16], ob[16];

	s64 ib0, ib1, ib2, ib3, ib4, ib5, ib6, ib7;
	s64 ob0, ob1, ob2, ob3, ob4, ob5, ob6, ob7;
	s64 ib8, ib9, ib10, ib11, ib12, ib13, ib14, ib15;
	s64 ob8, ob9, ob10, ob11, ob12, ob13, ob14, ob15;

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
			
	oblk[ostride* 0]=ob0;	oblk[ostride* 1]=ob1;
	oblk[ostride* 2]=ob2;	oblk[ostride* 3]=ob3;
	oblk[ostride* 4]=ob4;	oblk[ostride* 5]=ob5;
	oblk[ostride* 6]=ob6;	oblk[ostride* 7]=ob7;
	oblk[ostride* 8]=ob8;	oblk[ostride* 9]=ob9;
	oblk[ostride*10]=ob10;	oblk[ostride*11]=ob11;
	oblk[ostride*12]=ob12;	oblk[ostride*13]=ob13;
	oblk[ostride*14]=ob14;	oblk[ostride*15]=ob15;
}

void BTAC2_TransIDCT16Stride(int *iblk, int *oblk,
	int xstride, int ystride)
{
	s64 s[256], t[256];
	int i, j;

	for(i=0; i<256; i++)
		s[i]=iblk[(i>>4)*ystride+(i&15)*xstride];

	BTAC2_TransIDCT_Stride16(s+ 0, s+ 0, 16, 16);
	BTAC2_TransIDCT_Stride16(s+ 1, s+ 1, 16, 16);
	BTAC2_TransIDCT_Stride16(s+ 2, s+ 2, 16, 16);
	BTAC2_TransIDCT_Stride16(s+ 3, s+ 3, 16, 16);
	BTAC2_TransIDCT_Stride16(s+ 4, s+ 4, 16, 16);
	BTAC2_TransIDCT_Stride16(s+ 5, s+ 5, 16, 16);
	BTAC2_TransIDCT_Stride16(s+ 6, s+ 6, 16, 16);
	BTAC2_TransIDCT_Stride16(s+ 7, s+ 7, 16, 16);
	BTAC2_TransIDCT_Stride16(s+ 8, s+ 8, 16, 16);
	BTAC2_TransIDCT_Stride16(s+ 9, s+ 9, 16, 16);
	BTAC2_TransIDCT_Stride16(s+10, s+10, 16, 16);
	BTAC2_TransIDCT_Stride16(s+11, s+11, 16, 16);
	BTAC2_TransIDCT_Stride16(s+12, s+12, 16, 16);
	BTAC2_TransIDCT_Stride16(s+13, s+13, 16, 16);
	BTAC2_TransIDCT_Stride16(s+14, s+14, 16, 16);
	BTAC2_TransIDCT_Stride16(s+15, s+15, 16, 16);

	BTAC2_TransIDCT_Stride16(s+16* 0, t+16* 0, 1, 1);
	BTAC2_TransIDCT_Stride16(s+16* 1, t+16* 1, 1, 1);
	BTAC2_TransIDCT_Stride16(s+16* 2, t+16* 2, 1, 1);
	BTAC2_TransIDCT_Stride16(s+16* 3, t+16* 3, 1, 1);
	BTAC2_TransIDCT_Stride16(s+16* 4, t+16* 4, 1, 1);
	BTAC2_TransIDCT_Stride16(s+16* 5, t+16* 5, 1, 1);
	BTAC2_TransIDCT_Stride16(s+16* 6, t+16* 6, 1, 1);
	BTAC2_TransIDCT_Stride16(s+16* 7, t+16* 7, 1, 1);
	BTAC2_TransIDCT_Stride16(s+16* 8, t+16* 8, 1, 1);
	BTAC2_TransIDCT_Stride16(s+16* 9, t+16* 9, 1, 1);
	BTAC2_TransIDCT_Stride16(s+16*10, t+16*10, 1, 1);
	BTAC2_TransIDCT_Stride16(s+16*11, t+16*11, 1, 1);
	BTAC2_TransIDCT_Stride16(s+16*12, t+16*12, 1, 1);
	BTAC2_TransIDCT_Stride16(s+16*13, t+16*13, 1, 1);
	BTAC2_TransIDCT_Stride16(s+16*14, t+16*14, 1, 1);
	BTAC2_TransIDCT_Stride16(s+16*15, t+16*15, 1, 1);

	for(i=0; i<256; i++)
		oblk[(i>>4)*ystride+(i&15)*xstride]=(t[i]+32768)>>16;
}
#endif

#if 0
void BTAC2_TransDCT256(int *iblk, int *oblk)
{
	int s[256], t[256];
	int i, j;

	BTAC2_TransDCT_Stride16(iblk+16* 0, s+1* 0, 1, 16);
	BTAC2_TransDCT_Stride16(iblk+16* 1, s+1* 1, 1, 16);
	BTAC2_TransDCT_Stride16(iblk+16* 2, s+1* 2, 1, 16);
	BTAC2_TransDCT_Stride16(iblk+16* 3, s+1* 3, 1, 16);
	BTAC2_TransDCT_Stride16(iblk+16* 4, s+1* 4, 1, 16);
	BTAC2_TransDCT_Stride16(iblk+16* 5, s+1* 5, 1, 16);
	BTAC2_TransDCT_Stride16(iblk+16* 6, s+1* 6, 1, 16);
	BTAC2_TransDCT_Stride16(iblk+16* 7, s+1* 7, 1, 16);
	BTAC2_TransDCT_Stride16(iblk+16* 8, s+1* 8, 1, 16);
	BTAC2_TransDCT_Stride16(iblk+16* 9, s+1* 9, 1, 16);
	BTAC2_TransDCT_Stride16(iblk+16*10, s+1*10, 1, 16);
	BTAC2_TransDCT_Stride16(iblk+16*11, s+1*11, 1, 16);
	BTAC2_TransDCT_Stride16(iblk+16*12, s+1*12, 1, 16);
	BTAC2_TransDCT_Stride16(iblk+16*13, s+1*13, 1, 16);
	BTAC2_TransDCT_Stride16(iblk+16*14, s+1*14, 1, 16);
	BTAC2_TransDCT_Stride16(iblk+16*15, s+1*15, 1, 16);

	BTAC2_TransDCT_Stride16(s, s, 1, 1);

	for(i=0; i<16; i++)
		oblk[i]=(s[i]+32768)>>16;
	for(i=16; i<256; i++)
		oblk[i]=(s[i]+127)>>8;
}

void BTAC2_TransIDCT256(int *iblk, int *oblk)
{
	int s[256], t[256];
	int i, j;

	BTAC2_TransIDCT_Stride16(s, t, 1, 1);

	BTAC2_TransIDCT_Stride16(iblk+ystride* 0, s+1* 0, 16, 1);
	BTAC2_TransIDCT_Stride16(iblk+ystride* 1, s+1* 1, xstride, 1);
	BTAC2_TransIDCT_Stride16(iblk+ystride* 2, s+1* 2, xstride, 1);
	BTAC2_TransIDCT_Stride16(iblk+ystride* 3, s+1* 3, xstride, 1);
	BTAC2_TransIDCT_Stride16(iblk+ystride* 4, s+1* 4, xstride, 1);
	BTAC2_TransIDCT_Stride16(iblk+ystride* 5, s+1* 5, xstride, 1);
	BTAC2_TransIDCT_Stride16(iblk+ystride* 6, s+1* 6, xstride, 1);
	BTAC2_TransIDCT_Stride16(iblk+ystride* 7, s+1* 7, xstride, 1);
	BTAC2_TransIDCT_Stride16(iblk+ystride* 8, s+1* 8, xstride, 1);
	BTAC2_TransIDCT_Stride16(iblk+ystride* 9, s+1* 9, xstride, 1);
	BTAC2_TransIDCT_Stride16(iblk+ystride*10, s+1*10, xstride, 1);
	BTAC2_TransIDCT_Stride16(iblk+ystride*11, s+1*11, xstride, 1);
	BTAC2_TransIDCT_Stride16(iblk+ystride*12, s+1*12, xstride, 1);
	BTAC2_TransIDCT_Stride16(iblk+ystride*13, s+1*13, xstride, 1);
	BTAC2_TransIDCT_Stride16(iblk+ystride*14, s+1*14, xstride, 1);
	BTAC2_TransIDCT_Stride16(iblk+ystride*15, s+1*15, xstride, 1);

	for(i=0; i<256; i++)
		oblk[(i>>4)*ystride+(i&15)*xstride]=(t[i]+32768)>>16;
}
#endif

void BTAC2_Common_TransformDCT(int *iblk, int *oblk, int cnt, int dc)
{
	BTAC2_TransDCT16Stride(iblk, oblk, 1, 16);
}

void BTAC2_Common_TransformIDCT(int *iblk, int *oblk, int cnt, int dc)
{
	BTAC2_TransIDCT16Stride(iblk, oblk, 1, 16);
}
