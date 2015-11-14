#include <bgbbtj.h>

#if 1
// void BGBBTJ_JPG_TransDCT_Horiz(byte *iblk, int *oblk)
void BGBBTJ_JPG_TransDCT_Horiz(short *iblk, int *oblk, int dcbias)
{
	int ib[8];

//	ib[0]=iblk[0]-128;	ib[1]=iblk[1]-128;
//	ib[2]=iblk[2]-128;	ib[3]=iblk[3]-128;
//	ib[4]=iblk[4]-128;	ib[5]=iblk[5]-128;
//	ib[6]=iblk[6]-128;	ib[7]=iblk[7]-128;

	ib[0]=iblk[0]-dcbias;	ib[1]=iblk[1]-dcbias;
	ib[2]=iblk[2]-dcbias;	ib[3]=iblk[3]-dcbias;
	ib[4]=iblk[4]-dcbias;	ib[5]=iblk[5]-dcbias;
	ib[6]=iblk[6]-dcbias;	ib[7]=iblk[7]-dcbias;

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

void BGBBTJ_JPG_TransDCT_Vert(int *iblk, int *oblk)
{
	oblk[ 0]=iblk[0]*91  +iblk[8]*91  
			+iblk[16]*91  +iblk[24]*91  
			+iblk[32]*91  +iblk[40]*91  
			+iblk[48]*91  +iblk[56]*91;
	oblk[ 8]=iblk[0]*126 +iblk[8]*106 
			+iblk[16]*71  +iblk[24]*25  
			-iblk[32]*25  -iblk[40]*71  
			-iblk[48]*106 -iblk[56]*126;
	oblk[16]=iblk[0]*118 +iblk[8]*49  
			-iblk[16]*49  -iblk[24]*118 
			-iblk[32]*118 -iblk[40]*49  
			+iblk[48]*49  +iblk[56]*118;
	oblk[24]=iblk[0]*106 -iblk[8]*25  
			-iblk[16]*126 -iblk[24]*71  
			+iblk[32]*71  +iblk[40]*126 
			+iblk[48]*25  -iblk[56]*106;
	oblk[32]=iblk[0]*91  -iblk[8]*91  
			-iblk[16]*91  +iblk[24]*91  
			+iblk[32]*91  -iblk[40]*91  
			-iblk[48]*91  +iblk[56]*91;
	oblk[40]=iblk[0]*71  -iblk[8]*126 
			+iblk[16]*25  +iblk[24]*106
			-iblk[32]*106 -iblk[40]*25  
			+iblk[48]*126 -iblk[56]*71;
	oblk[48]=iblk[0]*49  -iblk[8]*118
			+iblk[16]*118 -iblk[24]*49  
			-iblk[32]*49  +iblk[40]*118 
			-iblk[48]*118 +iblk[56]*49;
	oblk[56]=iblk[0]*25  -iblk[8]*71  
			+iblk[16]*106 -iblk[24]*126 
			+iblk[32]*126 -iblk[40]*106 
			+iblk[48]*71  -iblk[56]*25;
}

// void BGBBTJ_JPG_TransDCT(byte *iblk, short *oblk)
void BGBBTJ_JPG_TransDCT(short *iblk, short *oblk, int dcbias)
{
	int s[DCTSZ2];
	int t[DCTSZ2];
	int i, j;

	BGBBTJ_JPG_TransDCT_Horiz(iblk+0, s+0, dcbias);
	BGBBTJ_JPG_TransDCT_Horiz(iblk+8, s+8, dcbias);
	BGBBTJ_JPG_TransDCT_Horiz(iblk+16, s+16, dcbias);
	BGBBTJ_JPG_TransDCT_Horiz(iblk+24, s+24, dcbias);
	BGBBTJ_JPG_TransDCT_Horiz(iblk+32, s+32, dcbias);
	BGBBTJ_JPG_TransDCT_Horiz(iblk+40, s+40, dcbias);
	BGBBTJ_JPG_TransDCT_Horiz(iblk+48, s+48, dcbias);
	BGBBTJ_JPG_TransDCT_Horiz(iblk+56, s+56, dcbias);

	BGBBTJ_JPG_TransDCT_Vert(s+0, t+0);
	BGBBTJ_JPG_TransDCT_Vert(s+1, t+1);
	BGBBTJ_JPG_TransDCT_Vert(s+2, t+2);
	BGBBTJ_JPG_TransDCT_Vert(s+3, t+3);
	BGBBTJ_JPG_TransDCT_Vert(s+4, t+4);
	BGBBTJ_JPG_TransDCT_Vert(s+5, t+5);
	BGBBTJ_JPG_TransDCT_Vert(s+6, t+6);
	BGBBTJ_JPG_TransDCT_Vert(s+7, t+7);

	for(i=0; i<64; i++)
		oblk[i]=t[i]>>16;
}

#endif

#if 0
void BGBBTJ_JPG_TransIDCT_Horiz(short *iblk, int *oblk)
{
	int t0, t1, t2, t3, t4, t5, t6, t7;
	int tt0, tt1, tt2, tt3;
	int t10, t11, t12, t13, t14, t15, t16, t17;
	int tt10, tt11, tt12, tt13;
	int z1, z2;

	t0=iblk[0]+iblk[7];	t1=iblk[1]+iblk[6];
	t2=iblk[2]+iblk[5];	t3=iblk[3]+iblk[4];
	t4=iblk[0]-iblk[7];	t5=iblk[1]-iblk[6];
	t6=iblk[2]-iblk[5];	t7=iblk[3]-iblk[4];
	t10=t0+t3;	t11=t1+t2;
	t12=t0-t3;	t13=t1-t2;

	t14=t4+t7;	t15=t5+t6;
	t16=t4+t6;	t17=t5+t7;
	
	z1=(t12+t13)*139;	z2=(t16+t17)*301;

	tt0=t4*384;			tt1=t5*787;
	tt2=t6*526;			tt3=t7*77;
	tt10=t14*-230;		tt11=t15*-656;
	tt12=t16*-100+z2;	tt13=t17*-502+z2;

	oblk[0]=(t10+t11)*256;
	oblk[4]=(t10-t11)*256;
	oblk[2]=(z1+t12* 196);
	oblk[6]=(z1+t13*-473);

	oblk[1]=tt0+tt10+tt12;
	oblk[3]=tt1+tt11+tt13;
	oblk[5]=tt2+tt11+tt12;
	oblk[7]=tt3+tt10+tt13;
}
#endif

#if 1
void BGBBTJ_JPG_TransIDCT_Horiz(short *iblk, int *oblk)
{
//	int i0, i1, i2, i3, i4, i5, i6, i7;
	int a, b, c, d, e, f;
	int i, j, k, l;
	int m, n, o, p;

#if 0
	i0=iblk[0];	i1=iblk[1];	i2=iblk[2];	i3=iblk[3];
	i4=iblk[4];	i5=iblk[5];	i6=iblk[6];	i7=iblk[7];
	a=i0* 91;	b=i4* 91;
	c=i2*118;	d=i2* 49;
	e=i6*118;	f=i6* 49;
	i=a+c+b+f;	j=a+d-b-e;	k=a-d-b+e;	l=a-c+b-f;
	m=i1*126+i3*106+i5* 71+i7* 25;
	n=i1*106-i3* 25-i5*126-i7* 71;
	o=i1* 71-i3*126+i5* 25+i7*106;
	p=i1* 25-i3* 71+i5*106-i7*126;
	oblk[0]=i+m;	oblk[1]=j+n;	oblk[2]=k+o;	oblk[3]=l+p;
	oblk[4]=l-p;	oblk[5]=k-o;	oblk[6]=j-n;	oblk[7]=i-m;
#endif

#if 1
	a=iblk[0]* 91;	b=iblk[4]* 91;
	c=iblk[2]*118;	d=iblk[2]* 49;
	e=iblk[6]*118;	f=iblk[6]* 49;
	i=a+c+b+f;	j=a+d-b-e;	k=a-d-b+e;	l=a-c+b-f;
	m=iblk[1]*126+iblk[3]*106+iblk[5]* 71+iblk[7]* 25;
	n=iblk[1]*106-iblk[3]* 25-iblk[5]*126-iblk[7]* 71;
	o=iblk[1]* 71-iblk[3]*126+iblk[5]* 25+iblk[7]*106;
	p=iblk[1]* 25-iblk[3]* 71+iblk[5]*106-iblk[7]*126;
	oblk[0]=i+m;	oblk[1]=j+n;	oblk[2]=k+o;	oblk[3]=l+p;
	oblk[4]=l-p;	oblk[5]=k-o;	oblk[6]=j-n;	oblk[7]=i-m;
#endif
}

void BGBBTJ_JPG_TransIDCT_Vert(int *iblk, int *oblk)
{
	int a, b, c, d, e, f;
	int i, j, k, l;
	int m, n, o, p;

	a=iblk[ 0]* 91;	b=iblk[32]*91;
	c=iblk[16]*118;	d=iblk[16]*49;
	e=iblk[48]*118;	f=iblk[48]*49;
	i=a+c+b+f;	j=a+d-b-e;	k=a-d-b+e;	l=a-c+b-f;
	m=iblk[ 8]*126+iblk[24]*106+iblk[40]* 71+iblk[56]* 25;
	n=iblk[ 8]*106-iblk[24]* 25-iblk[40]*126-iblk[56]* 71;
	o=iblk[ 8]* 71-iblk[24]*126+iblk[40]* 25+iblk[56]*106;
	p=iblk[ 8]* 25-iblk[24]* 71+iblk[40]*106-iblk[56]*126;
	oblk[ 0]=i+m;	oblk[ 8]=j+n;	oblk[16]=k+o;	oblk[24]=l+p;
	oblk[32]=l-p;	oblk[40]=k-o;	oblk[48]=j-n;	oblk[56]=i-m;
}

void BGBBTJ_JPG_TransIDCT_Vert2(int *iblk, short *oblk, int dcbias)
{
	int a, b, c, d, e, f;
	int i, j, k, l;
	int m, n, o, p;

	a=iblk[ 0]* 91;	b=iblk[32]*91;
	c=iblk[16]*118;	d=iblk[16]*49;
	e=iblk[48]*118;	f=iblk[48]*49;
	i=a+c+b+f;	j=a+d-b-e;	k=a-d-b+e;	l=a-c+b-f;
	m=iblk[ 8]*126+iblk[24]*106+iblk[40]* 71+iblk[56]* 25;
	n=iblk[ 8]*106-iblk[24]* 25-iblk[40]*126-iblk[56]* 71;
	o=iblk[ 8]* 71-iblk[24]*126+iblk[40]* 25+iblk[56]*106;
	p=iblk[ 8]* 25-iblk[24]* 71+iblk[40]*106-iblk[56]*126;
	oblk[ 0]=((i+m)>>16)+dcbias;	oblk[ 8]=((j+n)>>16)+dcbias;
	oblk[16]=((k+o)>>16)+dcbias;	oblk[24]=((l+p)>>16)+dcbias;
	oblk[32]=((l-p)>>16)+dcbias;	oblk[40]=((k-o)>>16)+dcbias;
	oblk[48]=((j-n)>>16)+dcbias;	oblk[56]=((i-m)>>16)+dcbias;
}

/** Determine whether the DCT block is a flat color (all AC coefficients are zero). */
int BGBBTJ_JPG_DetectFlatIDCT(short *iblk)
{
	int i;

	if(iblk[ 1])return(0);
	if(iblk[ 8])return(0);

//	if(iblk[ 1]|iblk[8])return(0);
	if(iblk[19]|iblk[9])return(0);
	if(iblk[ 2]|iblk[3]|iblk[10])	return(0);
	if(iblk[17]|iblk[24]|iblk[32]|iblk[25])	return(0);
	if(iblk[18]|iblk[11]|iblk[ 4]|iblk[ 5])	return(0);

	if(	iblk[12]|iblk[19]|iblk[26]|iblk[33]|
		iblk[40]|iblk[48]|iblk[41]|iblk[34])
			return(0);
	if(	iblk[27]|iblk[20]|iblk[13]|iblk[ 6]|
		iblk[ 7]|iblk[14]|iblk[21]|iblk[28])
			return(0);

	if(	iblk[35]|iblk[42]|iblk[49]|iblk[56]|
		iblk[57]|iblk[50]|iblk[43]|iblk[36]|
		iblk[29]|iblk[22]|iblk[15]|iblk[23]|
		iblk[30]|iblk[37]|iblk[44]|iblk[51])
			return(0);
	if(	iblk[58]|iblk[59]|iblk[52]|iblk[45]|
		iblk[38]|iblk[31]|iblk[39]|iblk[46]|
		iblk[53]|iblk[60]|iblk[61]|iblk[54]|
		iblk[47]|iblk[55]|iblk[62]|iblk[63])
			return(0);
	return(1);
}

// void BGBBTJ_JPG_TransIDCT(short *iblk, byte *oblk)
void BGBBTJ_JPG_TransIDCT(short *iblk, short *oblk,
	int dcbias, int rtmode)
{
	int s[DCTSZ2];
	int t[DCTSZ2];
	short *ob, *obe;
	int i, j, k;

#if 1
//	if(BGBBTJ_JPG_DetectFlatIDCT(iblk))
	if(rtmode==2)
	{
		/* If DCT block is a flat color, fill directly */
		k=((iblk[0]*8281+32768)>>16)+dcbias;
//		k=((iblk[0]*8192+32768)>>16)+dcbias;
		ob=oblk; obe=ob+64;
//		for(i=0; i<8; i++)
		while(ob<obe)
		{
			ob[ 0]=k; ob[ 1]=k; ob[ 2]=k; ob[ 3]=k;
			ob[ 4]=k; ob[ 5]=k; ob[ 6]=k; ob[ 7]=k;
			ob[ 8]=k; ob[ 9]=k; ob[10]=k; ob[11]=k;
			ob[12]=k; ob[13]=k; ob[14]=k; ob[15]=k;
			ob+=16;
		}
		return;
	}
#endif

#if 0
//	i=iblk[1]+iblk[8]+iblk[16]+iblk[9];
	i=iblk[1]+iblk[8];
	if(!i)
	{
		k=iblk[1]+iblk[2]+iblk[3]+iblk[4]+
			iblk[5]+iblk[6]+iblk[7];
		for(i=1; (i<8) && !k; i++)
		{
			j=i*8;
			k+=iblk[j+0]+iblk[j+1]+iblk[j+2]+iblk[j+3]+
				iblk[j+4]+iblk[j+5]+iblk[j+6]+iblk[j+7];
		}
		if(!k)
		{
			k=((iblk[0]*8281)>>16)+dcbias;

			for(i=0; i<8; i++)
			{
				j=i*8;
				oblk[j+0]=k; oblk[j+1]=k;
				oblk[j+2]=k; oblk[j+3]=k;
				oblk[j+4]=k; oblk[j+5]=k;
				oblk[j+6]=k; oblk[j+7]=k;
			}
			return;
		}
	}
#endif

	BGBBTJ_JPG_TransIDCT_Horiz(iblk+0, s+0);
	BGBBTJ_JPG_TransIDCT_Horiz(iblk+8, s+8);
	BGBBTJ_JPG_TransIDCT_Horiz(iblk+16, s+16);
	BGBBTJ_JPG_TransIDCT_Horiz(iblk+24, s+24);
	BGBBTJ_JPG_TransIDCT_Horiz(iblk+32, s+32);
	BGBBTJ_JPG_TransIDCT_Horiz(iblk+40, s+40);
	BGBBTJ_JPG_TransIDCT_Horiz(iblk+48, s+48);
	BGBBTJ_JPG_TransIDCT_Horiz(iblk+56, s+56);

#if 1
	BGBBTJ_JPG_TransIDCT_Vert2(s+0, oblk+0, dcbias);
	BGBBTJ_JPG_TransIDCT_Vert2(s+1, oblk+1, dcbias);
	BGBBTJ_JPG_TransIDCT_Vert2(s+2, oblk+2, dcbias);
	BGBBTJ_JPG_TransIDCT_Vert2(s+3, oblk+3, dcbias);
	BGBBTJ_JPG_TransIDCT_Vert2(s+4, oblk+4, dcbias);
	BGBBTJ_JPG_TransIDCT_Vert2(s+5, oblk+5, dcbias);
	BGBBTJ_JPG_TransIDCT_Vert2(s+6, oblk+6, dcbias);
	BGBBTJ_JPG_TransIDCT_Vert2(s+7, oblk+7, dcbias);
#endif

#if 0
	BGBBTJ_JPG_TransIDCT_Vert(s+0, t+0);
	BGBBTJ_JPG_TransIDCT_Vert(s+1, t+1);
	BGBBTJ_JPG_TransIDCT_Vert(s+2, t+2);
	BGBBTJ_JPG_TransIDCT_Vert(s+3, t+3);
	BGBBTJ_JPG_TransIDCT_Vert(s+4, t+4);
	BGBBTJ_JPG_TransIDCT_Vert(s+5, t+5);
	BGBBTJ_JPG_TransIDCT_Vert(s+6, t+6);
	BGBBTJ_JPG_TransIDCT_Vert(s+7, t+7);
#endif

#if 0
	for(i=0; i<8; i++)
	{
		j=i*8;
		oblk[j+0]=(t[j+0]>>16)+dcbias;
		oblk[j+1]=(t[j+1]>>16)+dcbias;
		oblk[j+2]=(t[j+2]>>16)+dcbias;
		oblk[j+3]=(t[j+3]>>16)+dcbias;
		oblk[j+4]=(t[j+4]>>16)+dcbias;
		oblk[j+5]=(t[j+5]>>16)+dcbias;
		oblk[j+6]=(t[j+6]>>16)+dcbias;
		oblk[j+7]=(t[j+7]>>16)+dcbias;
	}
#endif

#if 0
	for(i=0; i<64; i++)
	{
		j=(t[i]>>16)+dcbias;
//		if(j&(~255))j=(j<0)?0:((j>255)?255:j);
		oblk[i]=j;
//		oblk[i]=(j<0)?0:((j>255)?255:j);
	}
#endif
}

void BGBBTJ_JPG_TransIDCT16(short *iblk, short *oblk, int dcbias)
{
	int s[DCTSZ2];
	int t[DCTSZ2];
	int i, j;

	BGBBTJ_JPG_TransIDCT_Horiz(iblk+0, s+0);
	BGBBTJ_JPG_TransIDCT_Horiz(iblk+8, s+8);
	BGBBTJ_JPG_TransIDCT_Horiz(iblk+16, s+16);
	BGBBTJ_JPG_TransIDCT_Horiz(iblk+24, s+24);
	BGBBTJ_JPG_TransIDCT_Horiz(iblk+32, s+32);
	BGBBTJ_JPG_TransIDCT_Horiz(iblk+40, s+40);
	BGBBTJ_JPG_TransIDCT_Horiz(iblk+48, s+48);
	BGBBTJ_JPG_TransIDCT_Horiz(iblk+56, s+56);

	for(i=0; i<DCTSZ2; i++)
		s[i]>>=4;

	BGBBTJ_JPG_TransIDCT_Vert(s+0, t+0);
	BGBBTJ_JPG_TransIDCT_Vert(s+1, t+1);
	BGBBTJ_JPG_TransIDCT_Vert(s+2, t+2);
	BGBBTJ_JPG_TransIDCT_Vert(s+3, t+3);
	BGBBTJ_JPG_TransIDCT_Vert(s+4, t+4);
	BGBBTJ_JPG_TransIDCT_Vert(s+5, t+5);
	BGBBTJ_JPG_TransIDCT_Vert(s+6, t+6);
	BGBBTJ_JPG_TransIDCT_Vert(s+7, t+7);

	for(i=0; i<64; i++)
	{
		j=(t[i]>>12)+dcbias;
//		if(j&(~255))j=(j<0)?0:((j>255)?255:j);
		oblk[i]=j;
	}
}

#endif


#if 0
void BGBBTJ_JPG_TransIDCT_Horiz(short *iblk, int *oblk)
{
	int a, b, c, d, e, f;
	int i, j, k, l;
	int m, n, o, p;

	a=iblk[0]*6;	b=iblk[4]*6;
	c=iblk[2]*7;	d=iblk[2]*3;
	e=iblk[6]*7;	f=iblk[6]*3;
	i=a+c+b+f;	j=a+d-b-e;	k=a-d-b+e;	l=a-c+b-f;
	m=iblk[1]*8+iblk[3]*7+iblk[5]*4+iblk[7]*2;
	n=iblk[1]*7-iblk[3]*2-iblk[5]*8-iblk[7]*4;
	o=iblk[1]*4-iblk[3]*8+iblk[5]*2+iblk[7]*7;
	p=iblk[1]*2-iblk[3]*4+iblk[5]*7-iblk[7]*8;
	oblk[0]=i+m;	oblk[1]=j+n;	oblk[2]=k+o;	oblk[3]=l+p;
	oblk[4]=l-p;	oblk[5]=k-o;	oblk[6]=j-n;	oblk[7]=i-m;
}

void BGBBTJ_JPG_TransIDCT_Vert(int *iblk, int *oblk)
{
	int a, b, c, d, e, f;
	int i, j, k, l;
	int m, n, o, p;

	a=iblk[ 0]*6;	b=iblk[32]*6;
	c=iblk[16]*7;	d=iblk[16]*3;
	e=iblk[48]*7;	f=iblk[48]*3;
	i=a+c+b+f;	j=a+d-b-e;	k=a-d-b+e;	l=a-c+b-f;
	m=iblk[ 8]*8+iblk[24]*7+iblk[40]*4+iblk[56]*2;
	n=iblk[ 8]*7-iblk[24]*2-iblk[40]*8-iblk[56]*4;
	o=iblk[ 8]*4-iblk[24]*8+iblk[40]*2+iblk[56]*7;
	p=iblk[ 8]*2-iblk[24]*4+iblk[40]*7-iblk[56]*8;
	oblk[ 0]=i+m;	oblk[ 8]=j+n;	oblk[16]=k+o;	oblk[24]=l+p;
	oblk[32]=l-p;	oblk[40]=k-o;	oblk[48]=j-n;	oblk[56]=i-m;
}

void BGBBTJ_JPG_TransIDCT_Vert2(int *iblk, short *oblk, int dcbias)
{
	int a, b, c, d, e, f;
	int i, j, k, l;
	int m, n, o, p;

	a=iblk[ 0]*6;	b=iblk[32]*6;
	c=iblk[16]*7;	d=iblk[16]*3;
	e=iblk[48]*7;	f=iblk[48]*3;
	i=a+c+b+f;	j=a+d-b-e;	k=a-d-b+e;	l=a-c+b-f;
	m=iblk[ 8]*8+iblk[24]*7+iblk[40]*4+iblk[56]*2;
	n=iblk[ 8]*7-iblk[24]*2-iblk[40]*8-iblk[56]*4;
	o=iblk[ 8]*4-iblk[24]*8+iblk[40]*2+iblk[56]*7;
	p=iblk[ 8]*2-iblk[24]*4+iblk[40]*7-iblk[56]*8;
	oblk[ 0]=((i+m)>>8)+dcbias;	oblk[ 8]=((j+n)>>8)+dcbias;
	oblk[16]=((k+o)>>8)+dcbias;	oblk[24]=((l+p)>>8)+dcbias;
	oblk[32]=((l-p)>>8)+dcbias;	oblk[40]=((k-o)>>8)+dcbias;
	oblk[48]=((j-n)>>8)+dcbias;	oblk[56]=((i-m)>>8)+dcbias;
}

// void BGBBTJ_JPG_TransIDCT(short *iblk, byte *oblk)
void BGBBTJ_JPG_TransIDCT(short *iblk, short *oblk,
	int dcbias, int rtmode)
{
	int s[DCTSZ2];
	int t[DCTSZ2];
	int i, j, k;

	BGBBTJ_JPG_TransIDCT_Horiz(iblk+0, s+0);
	BGBBTJ_JPG_TransIDCT_Horiz(iblk+8, s+8);
	BGBBTJ_JPG_TransIDCT_Horiz(iblk+16, s+16);
	BGBBTJ_JPG_TransIDCT_Horiz(iblk+24, s+24);
	BGBBTJ_JPG_TransIDCT_Horiz(iblk+32, s+32);
	BGBBTJ_JPG_TransIDCT_Horiz(iblk+40, s+40);
	BGBBTJ_JPG_TransIDCT_Horiz(iblk+48, s+48);
	BGBBTJ_JPG_TransIDCT_Horiz(iblk+56, s+56);

#if 0
	BGBBTJ_JPG_TransIDCT_Vert(s+0, t+0);
	BGBBTJ_JPG_TransIDCT_Vert(s+1, t+1);
	BGBBTJ_JPG_TransIDCT_Vert(s+2, t+2);
	BGBBTJ_JPG_TransIDCT_Vert(s+3, t+3);
	BGBBTJ_JPG_TransIDCT_Vert(s+4, t+4);
	BGBBTJ_JPG_TransIDCT_Vert(s+5, t+5);
	BGBBTJ_JPG_TransIDCT_Vert(s+6, t+6);
	BGBBTJ_JPG_TransIDCT_Vert(s+7, t+7);
#endif

#if 1
	BGBBTJ_JPG_TransIDCT_Vert2(s+0, oblk+0, dcbias);
	BGBBTJ_JPG_TransIDCT_Vert2(s+1, oblk+1, dcbias);
	BGBBTJ_JPG_TransIDCT_Vert2(s+2, oblk+2, dcbias);
	BGBBTJ_JPG_TransIDCT_Vert2(s+3, oblk+3, dcbias);
	BGBBTJ_JPG_TransIDCT_Vert2(s+4, oblk+4, dcbias);
	BGBBTJ_JPG_TransIDCT_Vert2(s+5, oblk+5, dcbias);
	BGBBTJ_JPG_TransIDCT_Vert2(s+6, oblk+6, dcbias);
	BGBBTJ_JPG_TransIDCT_Vert2(s+7, oblk+7, dcbias);
#endif
}
#endif


#define FLTOFIX14(x)	((int)((x)*8192+0.5))
#define FIX14TOI(x)	(((x)+4096)>>13)

#if 0
//RDCT: Integer Reversible DCT (Lossless)

// void BGBBTJ_JPG_TransRDCT_Horiz(byte *iblk, int *oblk)
void BGBBTJ_JPG_TransRDCT_Horiz(short *iblk, int *oblk, int dcbias)
{
	int ib[8];

	ib[0]=iblk[2]-dcbias;	ib[1]=iblk[7]-dcbias;
	ib[2]=iblk[4]-dcbias;	ib[3]=iblk[3]-dcbias;
	ib[4]=iblk[6]-dcbias;	ib[5]=iblk[0]-dcbias;
	ib[6]=iblk[1]-dcbias;	ib[7]=iblk[5]-dcbias;

	ib[7]=(-ib[7])+FIX14TOI(
		ib[0]*FLTOFIX14(1.1648)+		ib[1]*FLTOFIX14(-2.8234)+
		ib[2]*FLTOFIX14(0.5375)+		ib[3]*FLTOFIX14(-0.6058)+
		ib[4]*FLTOFIX14(1.2228)+		ib[5]*FLTOFIX14(-0.3805)+
		ib[6]*FLTOFIX14(0.0288));
	ib[0]=ib[0]+FIX14TOI(
		ib[1]*FLTOFIX14(-1.1129)+		ib[2]*FLTOFIX14(0.0570)+
		ib[3]*FLTOFIX14(-0.4712)+		ib[4]*FLTOFIX14(0.1029)+
		ib[5]*FLTOFIX14(0.0156)+		ib[6]*FLTOFIX14(-0.4486)+
		ib[7]*FLTOFIX14(-0.4619));
	ib[1]=ib[1]+FIX14TOI(
		ib[0]*FLTOFIX14(-0.0685)+		ib[2]*FLTOFIX14(0.2708)+
		ib[3]*FLTOFIX14(-0.2708)+		ib[4]*FLTOFIX14(-0.2235)+
		ib[5]*FLTOFIX14(0.2568)+		ib[6]*FLTOFIX14(-0.3205)+
		ib[7]*FLTOFIX14(0.3841));
	ib[2]=ib[2]+FIX14TOI(
		ib[0]*FLTOFIX14(-0.0364)+		ib[1]*FLTOFIX14(-1.7104)+
		ib[3]*FLTOFIX14(-1.0000)+		ib[4]*FLTOFIX14(0.3066)+
		ib[5]*FLTOFIX14(0.6671)+		ib[6]*FLTOFIX14(-0.5953)+
		ib[7]*FLTOFIX14(0.2039));
	ib[3]=ib[3]+FIX14TOI(
		ib[0]*FLTOFIX14(0.7957)+		ib[1]*FLTOFIX14(0.9664)+
		ib[2]*FLTOFIX14(0.4439)+		ib[4]*FLTOFIX14(0.6173)+
		ib[5]*FLTOFIX14(-0.1422)+		ib[6]*FLTOFIX14(1.0378)+
		ib[7]*FLTOFIX14(-0.1700));
	ib[4]=ib[4]+FIX14TOI(
		ib[0]*FLTOFIX14(0.4591)+		ib[1]*FLTOFIX14(0.4108)+
		ib[2]*FLTOFIX14(-0.2073)+		ib[3]*FLTOFIX14(-1.0824)+
		ib[5]*FLTOFIX14(0.7071)+		ib[6]*FLTOFIX14(0.8873)+
		ib[7]*FLTOFIX14(-0.2517));
	ib[5]=ib[5]+FIX14TOI(
		ib[0]*FLTOFIX14(-0.6573)+		ib[1]*FLTOFIX14(0.5810)+
		ib[2]*FLTOFIX14(-0.2931)+		ib[3]*FLTOFIX14(-0.5307)+
		ib[4]*FLTOFIX14(-0.8730)+		ib[6]*FLTOFIX14(-0.1594)+
		ib[7]*FLTOFIX14(-0.3560));
	ib[6]=ib[6]+FIX14TOI(
		ib[0]*FLTOFIX14(1.0024)+		ib[1]*FLTOFIX14(-0.7180)+
		ib[2]*FLTOFIX14(-0.0928)+		ib[3]*FLTOFIX14(-0.0318)+
		ib[4]*FLTOFIX14(0.4170)+		ib[5]*FLTOFIX14(1.1665)+
		ib[7]*FLTOFIX14(0.4904));
	ib[7]=ib[7]+FIX14TOI(
		ib[0]*FLTOFIX14(1.1020)+		ib[1]*FLTOFIX14(-2.0306)+
		ib[2]*FLTOFIX14(-0.3881)+		ib[3]*FLTOFIX14(0.6561)+
		ib[4]*FLTOFIX14(1.2405)+		ib[5]*FLTOFIX14(1.6577)+
		ib[6]*FLTOFIX14(-1.1914));
	oblk[0]=ib[3];	oblk[1]=ib[6];
	oblk[2]=ib[4];	oblk[3]=ib[2];
	oblk[4]=ib[5];	oblk[5]=ib[7];
	oblk[6]=ib[0];	oblk[7]=ib[1];
}

void BGBBTJ_JPG_TransRDCT_Vert(int *iblk, int *oblk)
{
	int ib[8];

	ib[0]=iblk[16];	ib[1]=iblk[56];
	ib[2]=iblk[32];	ib[3]=iblk[24];
	ib[4]=iblk[48];	ib[5]=iblk[ 0];
	ib[6]=iblk[ 8];	ib[7]=iblk[40];

	ib[7]=(-ib[7])+FIX14TOI(
		ib[0]*FLTOFIX14(1.1648)+		ib[1]*FLTOFIX14(-2.8234)+
		ib[2]*FLTOFIX14(0.5375)+		ib[3]*FLTOFIX14(-0.6058)+
		ib[4]*FLTOFIX14(1.2228)+		ib[5]*FLTOFIX14(-0.3805)+
		ib[6]*FLTOFIX14(0.0288));
	ib[0]=ib[0]+FIX14TOI(
		ib[1]*FLTOFIX14(-1.1129)+		ib[2]*FLTOFIX14(0.0570)+
		ib[3]*FLTOFIX14(-0.4712)+		ib[4]*FLTOFIX14(0.1029)+
		ib[5]*FLTOFIX14(0.0156)+		ib[6]*FLTOFIX14(-0.4486)+
		ib[7]*FLTOFIX14(-0.4619));
	ib[1]=ib[1]+FIX14TOI(
		ib[0]*FLTOFIX14(-0.0685)+		ib[2]*FLTOFIX14(0.2708)+
		ib[3]*FLTOFIX14(-0.2708)+		ib[4]*FLTOFIX14(-0.2235)+
		ib[5]*FLTOFIX14(0.2568)+		ib[6]*FLTOFIX14(-0.3205)+
		ib[7]*FLTOFIX14(0.3841));
	ib[2]=ib[2]+FIX14TOI(
		ib[0]*FLTOFIX14(-0.0364)+		ib[1]*FLTOFIX14(-1.7104)+
		ib[3]*FLTOFIX14(-1.0000)+		ib[4]*FLTOFIX14(0.3066)+
		ib[5]*FLTOFIX14(0.6671)+		ib[6]*FLTOFIX14(-0.5953)+
		ib[7]*FLTOFIX14(0.2039));
	ib[3]=ib[3]+FIX14TOI(
		ib[0]*FLTOFIX14(0.7957)+		ib[1]*FLTOFIX14(0.9664)+
		ib[2]*FLTOFIX14(0.4439)+		ib[4]*FLTOFIX14(0.6173)+
		ib[5]*FLTOFIX14(-0.1422)+		ib[6]*FLTOFIX14(1.0378)+
		ib[7]*FLTOFIX14(-0.1700));
	ib[4]=ib[4]+FIX14TOI(
		ib[0]*FLTOFIX14(0.4591)+		ib[1]*FLTOFIX14(0.4108)+
		ib[2]*FLTOFIX14(-0.2073)+		ib[3]*FLTOFIX14(-1.0824)+
		ib[5]*FLTOFIX14(0.7071)+		ib[6]*FLTOFIX14(0.8873)+
		ib[7]*FLTOFIX14(-0.2517));
	ib[5]=ib[5]+FIX14TOI(
		ib[0]*FLTOFIX14(-0.6573)+		ib[1]*FLTOFIX14(0.5810)+
		ib[2]*FLTOFIX14(-0.2931)+		ib[3]*FLTOFIX14(-0.5307)+
		ib[4]*FLTOFIX14(-0.8730)+		ib[6]*FLTOFIX14(-0.1594)+
		ib[7]*FLTOFIX14(-0.3560));
	ib[6]=ib[6]+FIX14TOI(
		ib[0]*FLTOFIX14(1.0024)+		ib[1]*FLTOFIX14(-0.7180)+
		ib[2]*FLTOFIX14(-0.0928)+		ib[3]*FLTOFIX14(-0.0318)+
		ib[4]*FLTOFIX14(0.4170)+		ib[5]*FLTOFIX14(1.1665)+
		ib[7]*FLTOFIX14(0.4904));
	ib[7]=ib[7]+FIX14TOI(
		ib[0]*FLTOFIX14(1.1020)+		ib[1]*FLTOFIX14(-2.0306)+
		ib[2]*FLTOFIX14(-0.3881)+		ib[3]*FLTOFIX14(0.6561)+
		ib[4]*FLTOFIX14(1.2405)+		ib[5]*FLTOFIX14(1.6577)+
		ib[6]*FLTOFIX14(-1.1914));
	oblk[0]=ib[3];	oblk[8]=ib[6];
	oblk[16]=ib[4];	oblk[24]=ib[2];
	oblk[32]=ib[5];	oblk[40]=ib[7];
	oblk[48]=ib[0];	oblk[56]=ib[1];
}

// void BGBBTJ_JPG_TransRDCT(byte *iblk, short *oblk)
void BGBBTJ_JPG_TransRDCT(short *iblk, short *oblk, int dcbias)
{
	int s[DCTSZ2];
	int t[DCTSZ2];
	int i, j;

	BGBBTJ_JPG_TransRDCT_Horiz(iblk+0, s+0, dcbias);
	BGBBTJ_JPG_TransRDCT_Horiz(iblk+8, s+8, dcbias);
	BGBBTJ_JPG_TransRDCT_Horiz(iblk+16, s+16, dcbias);
	BGBBTJ_JPG_TransRDCT_Horiz(iblk+24, s+24, dcbias);
	BGBBTJ_JPG_TransRDCT_Horiz(iblk+32, s+32, dcbias);
	BGBBTJ_JPG_TransRDCT_Horiz(iblk+40, s+40, dcbias);
	BGBBTJ_JPG_TransRDCT_Horiz(iblk+48, s+48, dcbias);
	BGBBTJ_JPG_TransRDCT_Horiz(iblk+56, s+56, dcbias);

	BGBBTJ_JPG_TransRDCT_Vert(s+0, t+0);
	BGBBTJ_JPG_TransRDCT_Vert(s+1, t+1);
	BGBBTJ_JPG_TransRDCT_Vert(s+2, t+2);
	BGBBTJ_JPG_TransRDCT_Vert(s+3, t+3);
	BGBBTJ_JPG_TransRDCT_Vert(s+4, t+4);
	BGBBTJ_JPG_TransRDCT_Vert(s+5, t+5);
	BGBBTJ_JPG_TransRDCT_Vert(s+6, t+6);
	BGBBTJ_JPG_TransRDCT_Vert(s+7, t+7);

	for(i=0; i<64; i++)
		oblk[i]=t[i];
}
#endif

#if 0
#undef FLTOFIX14
#undef FIX14TOI
#define FLTOFIX14(x)	((int)((x)*16384+0.5))
#define FIX14TOI(x)	(((x)+8192)>>14)
#endif

#if 0
void BGBBTJ_JPG_TransIRDCT_Horiz(int *iblk, int *oblk)
{
	int ib[8];

	ib[0]=iblk[6];	ib[1]=iblk[7];
	ib[2]=iblk[3];	ib[3]=iblk[0];
	ib[4]=iblk[2];	ib[5]=iblk[4];
	ib[6]=iblk[1];	ib[7]=iblk[5];

	ib[7]=ib[7]-FIX14TOI(
		ib[0]*FLTOFIX14(1.1020)+		ib[1]*FLTOFIX14(-2.0306)+
		ib[2]*FLTOFIX14(-0.3881)+		ib[3]*FLTOFIX14(0.6561)+
		ib[4]*FLTOFIX14(1.2405)+		ib[5]*FLTOFIX14(1.6577)+
		ib[6]*FLTOFIX14(-1.1914));
	ib[6]=ib[6]-FIX14TOI(
		ib[0]*FLTOFIX14(1.0024)+		ib[1]*FLTOFIX14(-0.7180)+
		ib[2]*FLTOFIX14(-0.0928)+		ib[3]*FLTOFIX14(-0.0318)+
		ib[4]*FLTOFIX14(0.4170)+		ib[5]*FLTOFIX14(1.1665)+
		ib[7]*FLTOFIX14(0.4904));
	ib[5]=ib[5]-FIX14TOI(
		ib[0]*FLTOFIX14(-0.6573)+		ib[1]*FLTOFIX14(0.5810)+
		ib[2]*FLTOFIX14(-0.2931)+		ib[3]*FLTOFIX14(-0.5307)+
		ib[4]*FLTOFIX14(-0.8730)+		ib[6]*FLTOFIX14(-0.1594)+
		ib[7]*FLTOFIX14(-0.3560));
	ib[4]=ib[4]-FIX14TOI(
		ib[0]*FLTOFIX14(0.4591)+		ib[1]*FLTOFIX14(0.4108)+
		ib[2]*FLTOFIX14(-0.2073)+		ib[3]*FLTOFIX14(-1.0824)+
		ib[5]*FLTOFIX14(0.7071)+		ib[6]*FLTOFIX14(0.8873)+
		ib[7]*FLTOFIX14(-0.2517));
	ib[3]=ib[3]-FIX14TOI(
		ib[0]*FLTOFIX14(0.7957)+		ib[1]*FLTOFIX14(0.9664)+
		ib[2]*FLTOFIX14(0.4439)+		ib[4]*FLTOFIX14(0.6173)+
		ib[5]*FLTOFIX14(-0.1422)+		ib[6]*FLTOFIX14(1.0378)+
		ib[7]*FLTOFIX14(-0.1700));
	ib[2]=ib[2]-FIX14TOI(
		ib[0]*FLTOFIX14(-0.0364)+		ib[1]*FLTOFIX14(-1.7104)+
		ib[3]*FLTOFIX14(-1.0000)+		ib[4]*FLTOFIX14(0.3066)+
		ib[5]*FLTOFIX14(0.6671)+		ib[6]*FLTOFIX14(-0.5953)+
		ib[7]*FLTOFIX14(0.2039));
	ib[1]=ib[1]-FIX14TOI(
		ib[0]*FLTOFIX14(-0.0685)+		ib[2]*FLTOFIX14(0.2708)+
		ib[3]*FLTOFIX14(-0.2708)+		ib[4]*FLTOFIX14(-0.2235)+
		ib[5]*FLTOFIX14(0.2568)+		ib[6]*FLTOFIX14(-0.3205)+
		ib[7]*FLTOFIX14(0.3841));
	ib[0]=ib[0]-FIX14TOI(
		ib[1]*FLTOFIX14(-1.1129)+		ib[2]*FLTOFIX14(0.0570)+
		ib[3]*FLTOFIX14(-0.4712)+		ib[4]*FLTOFIX14(0.1029)+
		ib[5]*FLTOFIX14(0.0156)+		ib[6]*FLTOFIX14(-0.4486)+
		ib[7]*FLTOFIX14(-0.4619));
	ib[7]=-ib[7]+FIX14TOI(
		ib[0]*FLTOFIX14(1.1648)+		ib[1]*FLTOFIX14(-2.8234)+
		ib[2]*FLTOFIX14(0.5375)+		ib[3]*FLTOFIX14(-0.6058)+
		ib[4]*FLTOFIX14(1.2228)+		ib[5]*FLTOFIX14(-0.3805)+
		ib[6]*FLTOFIX14(0.0288));

	oblk[0]=ib[5];	oblk[1]=ib[6];
	oblk[2]=ib[0];	oblk[3]=ib[3];
	oblk[4]=ib[2];	oblk[5]=ib[7];
	oblk[6]=ib[4];	oblk[7]=ib[1];
}

void BGBBTJ_JPG_TransIRDCT_Vert(short *iblk, int *oblk)
{
	int ib[8];

	ib[0]=iblk[48];	ib[1]=iblk[56];
	ib[2]=iblk[24];	ib[3]=iblk[ 0];
	ib[4]=iblk[16];	ib[5]=iblk[32];
	ib[6]=iblk[ 8];	ib[7]=iblk[40];

	ib[7]=ib[7]-FIX14TOI(
		ib[0]*FLTOFIX14(1.1020)+		ib[1]*FLTOFIX14(-2.0306)+
		ib[2]*FLTOFIX14(-0.3881)+		ib[3]*FLTOFIX14(0.6561)+
		ib[4]*FLTOFIX14(1.2405)+		ib[5]*FLTOFIX14(1.6577)+
		ib[6]*FLTOFIX14(-1.1914));
	ib[6]=ib[6]-FIX14TOI(
		ib[0]*FLTOFIX14(1.0024)+		ib[1]*FLTOFIX14(-0.7180)+
		ib[2]*FLTOFIX14(-0.0928)+		ib[3]*FLTOFIX14(-0.0318)+
		ib[4]*FLTOFIX14(0.4170)+		ib[5]*FLTOFIX14(1.1665)+
		ib[7]*FLTOFIX14(0.4904));
	ib[5]=ib[5]-FIX14TOI(
		ib[0]*FLTOFIX14(-0.6573)+		ib[1]*FLTOFIX14(0.5810)+
		ib[2]*FLTOFIX14(-0.2931)+		ib[3]*FLTOFIX14(-0.5307)+
		ib[4]*FLTOFIX14(-0.8730)+		ib[6]*FLTOFIX14(-0.1594)+
		ib[7]*FLTOFIX14(-0.3560));
	ib[4]=ib[4]-FIX14TOI(
		ib[0]*FLTOFIX14(0.4591)+		ib[1]*FLTOFIX14(0.4108)+
		ib[2]*FLTOFIX14(-0.2073)+		ib[3]*FLTOFIX14(-1.0824)+
		ib[5]*FLTOFIX14(0.7071)+		ib[6]*FLTOFIX14(0.8873)+
		ib[7]*FLTOFIX14(-0.2517));
	ib[3]=ib[3]-FIX14TOI(
		ib[0]*FLTOFIX14(0.7957)+		ib[1]*FLTOFIX14(0.9664)+
		ib[2]*FLTOFIX14(0.4439)+		ib[4]*FLTOFIX14(0.6173)+
		ib[5]*FLTOFIX14(-0.1422)+		ib[6]*FLTOFIX14(1.0378)+
		ib[7]*FLTOFIX14(-0.1700));
	ib[2]=ib[2]-FIX14TOI(
		ib[0]*FLTOFIX14(-0.0364)+		ib[1]*FLTOFIX14(-1.7104)+
		ib[3]*FLTOFIX14(-1.0000)+		ib[4]*FLTOFIX14(0.3066)+
		ib[5]*FLTOFIX14(0.6671)+		ib[6]*FLTOFIX14(-0.5953)+
		ib[7]*FLTOFIX14(0.2039));
	ib[1]=ib[1]-FIX14TOI(
		ib[0]*FLTOFIX14(-0.0685)+		ib[2]*FLTOFIX14(0.2708)+
		ib[3]*FLTOFIX14(-0.2708)+		ib[4]*FLTOFIX14(-0.2235)+
		ib[5]*FLTOFIX14(0.2568)+		ib[6]*FLTOFIX14(-0.3205)+
		ib[7]*FLTOFIX14(0.3841));
	ib[0]=ib[0]-FIX14TOI(
		ib[1]*FLTOFIX14(-1.1129)+		ib[2]*FLTOFIX14(0.0570)+
		ib[3]*FLTOFIX14(-0.4712)+		ib[4]*FLTOFIX14(0.1029)+
		ib[5]*FLTOFIX14(0.0156)+		ib[6]*FLTOFIX14(-0.4486)+
		ib[7]*FLTOFIX14(-0.4619));
	ib[7]=-ib[7]+FIX14TOI(
		ib[0]*FLTOFIX14(1.1648)+		ib[1]*FLTOFIX14(-2.8234)+
		ib[2]*FLTOFIX14(0.5375)+		ib[3]*FLTOFIX14(-0.6058)+
		ib[4]*FLTOFIX14(1.2228)+		ib[5]*FLTOFIX14(-0.3805)+
		ib[6]*FLTOFIX14(0.0288));

	oblk[ 0]=ib[5];	oblk[ 8]=ib[6];
	oblk[16]=ib[0];	oblk[24]=ib[3];
	oblk[32]=ib[2];	oblk[40]=ib[7];
	oblk[48]=ib[4];	oblk[56]=ib[1];
}

#if 0
// void BGBBTJ_JPG_TransIRDCT(short *iblk, byte *oblk)
void BGBBTJ_JPG_TransIRDCT(short *iblk, short *oblk,
	int dcbias, int rtmode)
{
	int s[DCTSZ2];
	int t[DCTSZ2];
	int i, j;

	BGBBTJ_JPG_TransIRDCT_Vert(iblk+0, s+0);
	BGBBTJ_JPG_TransIRDCT_Vert(iblk+1, s+1);
	BGBBTJ_JPG_TransIRDCT_Vert(iblk+2, s+2);
	BGBBTJ_JPG_TransIRDCT_Vert(iblk+3, s+3);
	BGBBTJ_JPG_TransIRDCT_Vert(iblk+4, s+4);
	BGBBTJ_JPG_TransIRDCT_Vert(iblk+5, s+5);
	BGBBTJ_JPG_TransIRDCT_Vert(iblk+6, s+6);
	BGBBTJ_JPG_TransIRDCT_Vert(iblk+7, s+7);

	BGBBTJ_JPG_TransIRDCT_Horiz(s+0, t+0);
	BGBBTJ_JPG_TransIRDCT_Horiz(s+8, t+8);
	BGBBTJ_JPG_TransIRDCT_Horiz(s+16, t+16);
	BGBBTJ_JPG_TransIRDCT_Horiz(s+24, t+24);
	BGBBTJ_JPG_TransIRDCT_Horiz(s+32, t+32);
	BGBBTJ_JPG_TransIRDCT_Horiz(s+40, t+40);
	BGBBTJ_JPG_TransIRDCT_Horiz(s+48, t+48);
	BGBBTJ_JPG_TransIRDCT_Horiz(s+56, t+56);

	for(i=0; i<64; i++)
	{
		j=(t[i])+dcbias;
//		if(j&(~255))j=(j<0)?0:((j>255)?255:j);
		oblk[i]=j;
	}
}
#endif

#endif

void BGBBTJ_JPG_TransRDCT_Ref(
	int iblk0, int iblk1, int iblk2, int iblk3,
	int iblk4, int iblk5, int iblk6, int iblk7,
	int *oblk0, int *oblk1, int *oblk2, int *oblk3,
	int *oblk4, int *oblk5, int *oblk6, int *oblk7)
{
	int ib0, ib1, ib2, ib3, ib4, ib5, ib6, ib7;

	ib0=iblk2; ib1=iblk7;
	ib2=iblk4; ib3=iblk3;
	ib4=iblk6; ib5=iblk0;
	ib6=iblk1; ib7=iblk5;

	//9542 -23129 4403 -4963 10017 -3117 236
	//-9117 467 -3860 843 128 -3675 -3784
	//-561 2218 -2218 -1831 2104 -2626 3147
	//-298 -14012 -8192 2512 5465 -4877 1670
	//6518 7917 3636 5057 -1165 8502 -1393
	//3761 3365 -1698 -8867 5793 7269 -2062
	//-5385 4760 -2401 -4347 -7152 -1306 -2916
	//8212 -5882 -760 -261 3416 9556 4017
	//9028 -16635 -3179 5375 10162 13580 -9760

#if 1
	ib7=(-ib7) + (((ib0*9542 + ib1*-23129 +
		ib2*4403 + ib3*-4963 + ib4*10017 +
		ib5*-3117 + ib6*236) + 4096) >> 13);
	ib0=ib0 + (((ib1*-9117 + ib2*467 +
		ib3*-3860 + ib4*843 + ib5*128 +
		ib6*-3675 + ib7*-3784) + 4096) >> 13);
	ib1=ib1 + (((ib0*-561 + ib2*2218 +
		ib3*-2218 + ib4*-1831 + ib5*2104 +
		ib6*-2626 + ib7*3147) + 4096) >> 13);
	ib2=ib2 + (((ib0*-298 + ib1*-14012 +
		ib3*-8192 + ib4*2512 + ib5*5465 +
		ib6*-4877 + ib7*1670) + 4096) >> 13);
	ib3=ib3 + (((ib0*6518 + ib1*7917 +
		ib2*3636 + ib4*5057 + ib5*-1165 +
		ib6*8502 + ib7*-1393) + 4096) >> 13);
	ib4=ib4 + (((ib0*3761 + ib1*3365 +
		ib2*-1698 + ib3*-8867 + ib5*5793 +
		ib6*7269 + ib7*-2062) + 4096) >> 13);
	ib5=ib5 + (((ib0*-5385 + ib1*4760 +
		ib2*-2401 + ib3*-4347 + ib4*-7152 +
		ib6*-1306 + ib7*-2916) + 4096) >> 13);
	ib6=ib6 + (((ib0*8212 + ib1*-5882 +
		ib2*-760 + ib3*-261 + ib4*3416 +
		ib5*9556 + ib7*4017) + 4096) >> 13);
	ib7=ib7 + (((ib0*9028 + ib1*-16635 +
		ib2*-3179 + ib3*5375 + ib4*10162 +
		ib5*13580 + ib6*-9760) + 4096) >> 13);
#endif

	*oblk0=ib3; *oblk1=ib6;
	*oblk2=ib4; *oblk3=ib2;
	*oblk4=ib5; *oblk5=ib7;
	*oblk6=ib0; *oblk7=ib1;
}

void BGBBTJ_JPG_TransIRDCT_Ref(
	int iblk0, int iblk1, int iblk2, int iblk3,
	int iblk4, int iblk5, int iblk6, int iblk7,
	int *oblk0, int *oblk1, int *oblk2, int *oblk3,
	int *oblk4, int *oblk5, int *oblk6, int *oblk7)
{
	int ib0, ib1, ib2, ib3, ib4, ib5, ib6, ib7;

	ib0=iblk6; ib1=iblk7;
	ib2=iblk3; ib3=iblk0;
	ib4=iblk2; ib5=iblk4;
	ib6=iblk1; ib7=iblk5;

	//9028 -16635 -3179 5375 10162 13580 -9760
	//8212 -5882 -760 -261 3416 9556 4017
	//-5385 4760 -2401 -4347 -7152 -1306 -2916
	//3761 3365 -1698 -8867 5793 7269 -2062
	//6518 7917 3636 5057 -1165 8502 -1393
	//-298 -14012 -8192 2512 5465 -4877 1670
	//-561 2218 -2218 -1831 2104 -2626 3147
	//-9117 467 -3860 843 128 -3675 -3784
	//9542 -23129 4403 -4963 10017 -3117 236

#if 1
	ib7=ib7 - (((ib0*9028 + ib1*-16635 +
		ib2*-3179 + ib3*5375 + ib4*10162 +
		ib5*13580 + ib6*-9760) + 4096) >> 13);
	ib6=ib6 - (((ib0*8212 + ib1*-5882 +
		ib2*-760 + ib3*-261 + ib4*3416 +
		ib5*9556 + ib7*4017) + 4096) >> 13);
	ib5=ib5 - (((ib0*-5385 + ib1*4760 +
		ib2*-2401 + ib3*-4347 + ib4*-7152 +
		ib6*-1306 + ib7*-2916) + 4096) >> 13);
	ib4=ib4 - (((ib0*3761 + ib1*3365 +
		ib2*-1698 + ib3*-8867 + ib5*5793 +
		ib6*7269 + ib7*-2062) + 4096) >> 13);
	ib3=ib3 - (((ib0*6518 + ib1*7917 +
		ib2*3636 + ib4*5057 + ib5*-1165 +
		ib6*8502 + ib7*-1393) + 4096) >> 13);
	ib2=ib2 - (((ib0*-298 + ib1*-14012 +
		ib3*-8192 + ib4*2512 + ib5*5465 +
		ib6*-4877 + ib7*1670) + 4096) >> 13);
	ib1=ib1 - (((ib0*-561 + ib2*2218 +
		ib3*-2218 + ib4*-1831 + ib5*2104 +
		ib6*-2626 + ib7*3147) + 4096) >> 13);
	ib0=ib0 - (((ib1*-9117 + ib2*467 +
		ib3*-3860 + ib4*843 + ib5*128 +
		ib6*-3675 + ib7*-3784) + 4096) >> 13);
	ib7=-ib7 + (((ib0*9542 + ib1*-23129 +
		ib2*4403 + ib3*-4963 + ib4*10017 +
		ib5*-3117 + ib6*236) + 4096) >> 13);
#endif

	*oblk0=ib5; *oblk1=ib6;
	*oblk2=ib0; *oblk3=ib3;
	*oblk4=ib2; *oblk5=ib7;
	*oblk6=ib4; *oblk7=ib1;
}

void BGBBTJ_JPG_TransRDCT(short *iblk, short *oblk, int dcbias)
{
	int s00, s01, s02, s03, s04, s05, s06, s07;
	int s10, s11, s12, s13, s14, s15, s16, s17;
	int s20, s21, s22, s23, s24, s25, s26, s27;
	int s30, s31, s32, s33, s34, s35, s36, s37;
	int s40, s41, s42, s43, s44, s45, s46, s47;
	int s50, s51, s52, s53, s54, s55, s56, s57;
	int s60, s61, s62, s63, s64, s65, s66, s67;
	int s70, s71, s72, s73, s74, s75, s76, s77;
	int t00, t01, t02, t03, t04, t05, t06, t07;
	int t10, t11, t12, t13, t14, t15, t16, t17;
	int t20, t21, t22, t23, t24, t25, t26, t27;
	int t30, t31, t32, t33, t34, t35, t36, t37;
	int t40, t41, t42, t43, t44, t45, t46, t47;
	int t50, t51, t52, t53, t54, t55, t56, t57;
	int t60, t61, t62, t63, t64, t65, t66, t67;
	int t70, t71, t72, t73, t74, t75, t76, t77;
	//int i;

	BGBBTJ_JPG_TransRDCT_Ref(
		iblk[ 0] - dcbias, iblk[ 1] - dcbias,
		iblk[ 2] - dcbias, iblk[ 3] - dcbias,
		iblk[ 4] - dcbias, iblk[ 5] - dcbias,
		iblk[ 6] - dcbias, iblk[ 7] - dcbias,
		&s00, &s01, &s02, &s03,
		&s04, &s05, &s06, &s07);
	BGBBTJ_JPG_TransRDCT_Ref(
		iblk[ 8] - dcbias, iblk[ 9] - dcbias,
		iblk[10] - dcbias, iblk[11] - dcbias,
		iblk[12] - dcbias, iblk[13] - dcbias,
		iblk[14] - dcbias, iblk[15] - dcbias,
		&s10, &s11, &s12, &s13,
		&s14, &s15, &s16, &s17);
	BGBBTJ_JPG_TransRDCT_Ref(
		iblk[16] - dcbias, iblk[17] - dcbias,
		iblk[18] - dcbias, iblk[19] - dcbias,
		iblk[20] - dcbias, iblk[21] - dcbias,
		iblk[22] - dcbias, iblk[23] - dcbias,
		&s20, &s21, &s22, &s23,
		&s24, &s25, &s26, &s27);
	BGBBTJ_JPG_TransRDCT_Ref(
		iblk[24] - dcbias, iblk[25] - dcbias,
		iblk[26] - dcbias, iblk[27] - dcbias,
		iblk[28] - dcbias, iblk[29] - dcbias,
		iblk[30] - dcbias, iblk[31] - dcbias,
		&s30, &s31, &s32, &s33, 
		&s34, &s35, &s36, &s37);
	BGBBTJ_JPG_TransRDCT_Ref(
		iblk[32] - dcbias, iblk[33] - dcbias,
		iblk[34] - dcbias, iblk[35] - dcbias,
		iblk[36] - dcbias, iblk[37] - dcbias,
		iblk[38] - dcbias, iblk[39] - dcbias,
		&s40, &s41, &s42, &s43,
		&s44, &s45, &s46, &s47);
	BGBBTJ_JPG_TransRDCT_Ref(
		iblk[40] - dcbias, iblk[41] - dcbias,
		iblk[42] - dcbias, iblk[43] - dcbias,
		iblk[44] - dcbias, iblk[45] - dcbias,
		iblk[46] - dcbias, iblk[47] - dcbias,
		&s50, &s51, &s52, &s53,
		&s54, &s55, &s56, &s57);
	BGBBTJ_JPG_TransRDCT_Ref(
		iblk[48] - dcbias, iblk[49] - dcbias,
		iblk[50] - dcbias, iblk[51] - dcbias,
		iblk[52] - dcbias, iblk[53] - dcbias,
		iblk[54] - dcbias, iblk[55] - dcbias,
		&s60, &s61, &s62, &s63,
		&s64, &s65, &s66, &s67);
	BGBBTJ_JPG_TransRDCT_Ref(
		iblk[56] - dcbias, iblk[57] - dcbias,
		iblk[58] - dcbias, iblk[59] - dcbias,
		iblk[60] - dcbias, iblk[61] - dcbias,
		iblk[62] - dcbias, iblk[63] - dcbias,
		&s70, &s71, &s72, &s73,
		&s74, &s75, &s76, &s77);

	BGBBTJ_JPG_TransRDCT_Ref(s00, s10, s20, s30, s40, s50, s60, s70,
		&t00, &t10, &t20, &t30, &t40, &t50, &t60, &t70);
	BGBBTJ_JPG_TransRDCT_Ref(s01, s11, s21, s31, s41, s51, s61, s71,
		&t01, &t11, &t21, &t31, &t41, &t51, &t61, &t71);
	BGBBTJ_JPG_TransRDCT_Ref(s02, s12, s22, s32, s42, s52, s62, s72,
		&t02, &t12, &t22, &t32, &t42, &t52, &t62, &t72);
	BGBBTJ_JPG_TransRDCT_Ref(s03, s13, s23, s33, s43, s53, s63, s73,
		&t03, &t13, &t23, &t33, &t43, &t53, &t63, &t73);
	BGBBTJ_JPG_TransRDCT_Ref(s04, s14, s24, s34, s44, s54, s64, s74,
		&t04, &t14, &t24, &t34, &t44, &t54, &t64, &t74);
	BGBBTJ_JPG_TransRDCT_Ref(s05, s15, s25, s35, s45, s55, s65, s75,
		&t05, &t15, &t25, &t35, &t45, &t55, &t65, &t75);
	BGBBTJ_JPG_TransRDCT_Ref(s06, s16, s26, s36, s46, s56, s66, s76,
		&t06, &t16, &t26, &t36, &t46, &t56, &t66, &t76);
	BGBBTJ_JPG_TransRDCT_Ref(s07, s17, s27, s37, s47, s57, s67, s77,
		&t07, &t17, &t27, &t37, &t47, &t57, &t67, &t77);

	oblk[ 0]=t00; oblk[ 1]=t01; oblk[ 2]=t02; oblk[ 3]=t03;
	oblk[ 4]=t04; oblk[ 5]=t05; oblk[ 6]=t06; oblk[ 7]=t07;
	oblk[ 8]=t10; oblk[ 9]=t11; oblk[10]=t12; oblk[11]=t13;
	oblk[12]=t14; oblk[13]=t15; oblk[14]=t16; oblk[15]=t17;
	oblk[16]=t20; oblk[17]=t21; oblk[18]=t22; oblk[19]=t23;
	oblk[20]=t24; oblk[21]=t25; oblk[22]=t26; oblk[23]=t27;
	oblk[24]=t30; oblk[25]=t31; oblk[26]=t32; oblk[27]=t33;
	oblk[28]=t34; oblk[29]=t35; oblk[30]=t36; oblk[31]=t37;
	oblk[32]=t40; oblk[33]=t41; oblk[34]=t42; oblk[35]=t43;
	oblk[36]=t44; oblk[37]=t45; oblk[38]=t46; oblk[39]=t47;
	oblk[40]=t50; oblk[41]=t51; oblk[42]=t52; oblk[43]=t53;
	oblk[44]=t54; oblk[45]=t55; oblk[46]=t56; oblk[47]=t57;
	oblk[48]=t60; oblk[49]=t61; oblk[50]=t62; oblk[51]=t63;
	oblk[52]=t64; oblk[53]=t65; oblk[54]=t66; oblk[55]=t67;
	oblk[56]=t70; oblk[57]=t71; oblk[58]=t72; oblk[59]=t73;
	oblk[60]=t74; oblk[61]=t75; oblk[62]=t76; oblk[63]=t77;
}

void BGBBTJ_JPG_TransIRDCT(short *iblk, short *oblk,
	int dcbias, int rtmode)
{
	int s00, s01, s02, s03, s04, s05, s06, s07;
	int s10, s11, s12, s13, s14, s15, s16, s17;
	int s20, s21, s22, s23, s24, s25, s26, s27;
	int s30, s31, s32, s33, s34, s35, s36, s37;
	int s40, s41, s42, s43, s44, s45, s46, s47;
	int s50, s51, s52, s53, s54, s55, s56, s57;
	int s60, s61, s62, s63, s64, s65, s66, s67;
	int s70, s71, s72, s73, s74, s75, s76, s77;
	int t00, t01, t02, t03, t04, t05, t06, t07;
	int t10, t11, t12, t13, t14, t15, t16, t17;
	int t20, t21, t22, t23, t24, t25, t26, t27;
	int t30, t31, t32, t33, t34, t35, t36, t37;
	int t40, t41, t42, t43, t44, t45, t46, t47;
	int t50, t51, t52, t53, t54, t55, t56, t57;
	int t60, t61, t62, t63, t64, t65, t66, t67;
	int t70, t71, t72, t73, t74, t75, t76, t77;

	BGBBTJ_JPG_TransIRDCT_Ref(
		iblk[ 0], iblk[ 8], iblk[16], iblk[24],
		iblk[32], iblk[40], iblk[48], iblk[56],
		&s00, &s10, &s20, &s30, &s40, &s50, &s60, &s70);
	BGBBTJ_JPG_TransIRDCT_Ref(
		iblk[ 1], iblk[ 9], iblk[17], iblk[25],
		iblk[33], iblk[41], iblk[49], iblk[57],
		&s01, &s11, &s21, &s31, &s41, &s51, &s61, &s71);
	BGBBTJ_JPG_TransIRDCT_Ref(
		iblk[ 2], iblk[10], iblk[18], iblk[26],
		iblk[34], iblk[42], iblk[50], iblk[58],
		&s02, &s12, &s22, &s32, &s42, &s52, &s62, &s72);
	BGBBTJ_JPG_TransIRDCT_Ref(
		iblk[ 3], iblk[11], iblk[19], iblk[27],
		iblk[35], iblk[43], iblk[51], iblk[59],
		&s03, &s13, &s23, &s33, &s43, &s53, &s63, &s73);
	BGBBTJ_JPG_TransIRDCT_Ref(
		iblk[ 4], iblk[12], iblk[20], iblk[28],
		iblk[36], iblk[44], iblk[52], iblk[60],
		&s04, &s14, &s24, &s34, &s44, &s54, &s64, &s74);
	BGBBTJ_JPG_TransIRDCT_Ref(
		iblk[ 5], iblk[13], iblk[21], iblk[29],
		iblk[37], iblk[45], iblk[53], iblk[61],
		&s05, &s15, &s25, &s35, &s45, &s55, &s65, &s75);
	BGBBTJ_JPG_TransIRDCT_Ref(
		iblk[ 6], iblk[14], iblk[22], iblk[30],
		iblk[38], iblk[46], iblk[54], iblk[62],
		&s06, &s16, &s26, &s36, &s46, &s56, &s66, &s76);
	BGBBTJ_JPG_TransIRDCT_Ref(
		iblk[ 7], iblk[15], iblk[23], iblk[31],
		iblk[39], iblk[47], iblk[55], iblk[63],
		&s07, &s17, &s27, &s37, &s47, &s57, &s67, &s77);

	BGBBTJ_JPG_TransIRDCT_Ref(s00, s01, s02, s03, s04, s05, s06, s07,
		&t00, &t01, &t02, &t03, &t04, &t05, &t06, &t07);
	BGBBTJ_JPG_TransIRDCT_Ref(s10, s11, s12, s13, s14, s15, s16, s17,
		&t10, &t11, &t12, &t13, &t14, &t15, &t16, &t17);
	BGBBTJ_JPG_TransIRDCT_Ref(s20, s21, s22, s23, s24, s25, s26, s27,
		&t20, &t21, &t22, &t23, &t24, &t25, &t26, &t27);
	BGBBTJ_JPG_TransIRDCT_Ref(s30, s31, s32, s33, s34, s35, s36, s37,
		&t30, &t31, &t32, &t33, &t34, &t35, &t36, &t37);
	BGBBTJ_JPG_TransIRDCT_Ref(s40, s41, s42, s43, s44, s45, s46, s47,
		&t40, &t41, &t42, &t43, &t44, &t45, &t46, &t47);
	BGBBTJ_JPG_TransIRDCT_Ref(s50, s51, s52, s53, s54, s55, s56, s57,
		&t50, &t51, &t52, &t53, &t54, &t55, &t56, &t57);
	BGBBTJ_JPG_TransIRDCT_Ref(s60, s61, s62, s63, s64, s65, s66, s67,
		&t60, &t61, &t62, &t63, &t64, &t65, &t66, &t67);
	BGBBTJ_JPG_TransIRDCT_Ref(s70, s71, s72, s73, s74, s75, s76, s77,
		&t70, &t71, &t72, &t73, &t74, &t75, &t76, &t77);

	oblk[ 0]=t00 + dcbias; oblk[ 1]=t01 + dcbias;
	oblk[ 2]=t02 + dcbias; oblk[ 3]=t03 + dcbias;
	oblk[ 4]=t04 + dcbias; oblk[ 5]=t05 + dcbias;
	oblk[ 6]=t06 + dcbias; oblk[ 7]=t07 + dcbias;
	oblk[ 8]=t10 + dcbias; oblk[ 9]=t11 + dcbias;
	oblk[10]=t12 + dcbias; oblk[11]=t13 + dcbias;
	oblk[12]=t14 + dcbias; oblk[13]=t15 + dcbias;
	oblk[14]=t16 + dcbias; oblk[15]=t17 + dcbias;
	oblk[16]=t20 + dcbias; oblk[17]=t21 + dcbias;
	oblk[18]=t22 + dcbias; oblk[19]=t23 + dcbias;
	oblk[20]=t24 + dcbias; oblk[21]=t25 + dcbias;
	oblk[22]=t26 + dcbias; oblk[23]=t27 + dcbias;
	oblk[24]=t30 + dcbias; oblk[25]=t31 + dcbias;
	oblk[26]=t32 + dcbias; oblk[27]=t33 + dcbias;
	oblk[28]=t34 + dcbias; oblk[29]=t35 + dcbias;
	oblk[30]=t36 + dcbias; oblk[31]=t37 + dcbias;
	oblk[32]=t40 + dcbias; oblk[33]=t41 + dcbias;
	oblk[34]=t42 + dcbias; oblk[35]=t43 + dcbias;
	oblk[36]=t44 + dcbias; oblk[37]=t45 + dcbias;
	oblk[38]=t46 + dcbias; oblk[39]=t47 + dcbias;
	oblk[40]=t50 + dcbias; oblk[41]=t51 + dcbias;
	oblk[42]=t52 + dcbias; oblk[43]=t53 + dcbias;
	oblk[44]=t54 + dcbias; oblk[45]=t55 + dcbias;
	oblk[46]=t56 + dcbias; oblk[47]=t57 + dcbias;
	oblk[48]=t60 + dcbias; oblk[49]=t61 + dcbias;
	oblk[50]=t62 + dcbias; oblk[51]=t63 + dcbias;
	oblk[52]=t64 + dcbias; oblk[53]=t65 + dcbias;
	oblk[54]=t66 + dcbias; oblk[55]=t67 + dcbias;
	oblk[56]=t70 + dcbias; oblk[57]=t71 + dcbias;
	oblk[58]=t72 + dcbias; oblk[59]=t73 + dcbias;
	oblk[60]=t74 + dcbias; oblk[61]=t75 + dcbias;
	oblk[62]=t76 + dcbias; oblk[63]=t77 + dcbias;
}

#if 0
//HAD: Hadamard Transform (Lossless)

void BGBBTJ_JPG_TransHAD_Horiz(byte *iblk, int *oblk)
{
	int ia[8];
	int ib[8];
	int ic[8];
	int id[8];

	ib[0]=iblk[0]-128;	ib[1]=iblk[1]-128;
	ib[2]=iblk[2]-128;	ib[3]=iblk[3]-128;
	ib[4]=iblk[4]-128;	ib[5]=iblk[5]-128;
	ib[6]=iblk[6]-128;	ib[7]=iblk[7]-128;

	ia[0]=ib[0]+ib[4];
	ia[1]=ib[1]+ib[5];
	ia[2]=ib[2]+ib[6];
	ia[3]=ib[3]+ib[7];
	ia[4]=(ia[0]>>1)-ib[4];
	ia[5]=(ia[1]>>1)-ib[5];
	ia[6]=(ia[2]>>1)-ib[6];
	ia[7]=(ia[3]>>1)-ib[7];

	ic[0]=ia[0]+ia[2];
	ic[1]=ia[1]+ia[3];
	ic[2]=(ic[0]>>1)-ia[2];
	ic[3]=(ic[1]>>1)-ia[3];
	ic[4]=ia[4]+ia[6];
	ic[5]=ia[5]+ia[7];
	ic[6]=(ic[4]>>1)-ia[6];
	ic[7]=(ic[5]>>1)-ia[7];

	id[0]=ic[0]+ic[1];
	id[1]=(id[0]>>1)-ic[1];
	id[2]=ic[2]+ic[3];
	id[3]=(id[2]>>1)-ic[3];
	id[4]=ic[4]+ic[5];
	id[5]=(id[4]>>1)-ic[5];
	id[6]=ic[6]+ic[7];
	id[7]=(id[6]>>1)-ic[7];

	oblk[0]=id[0];	oblk[1]=id[4];
	oblk[2]=id[6];	oblk[3]=id[2];
	oblk[4]=id[3];	oblk[5]=id[7];
	oblk[6]=id[5];	oblk[7]=id[1];
}

void BGBBTJ_JPG_TransHAD_Vert(int *iblk, int *oblk)
{
	int ib[8];

	ib[0]=iblk[16];	ib[1]=iblk[56];
	ib[2]=iblk[32];	ib[3]=iblk[24];
	ib[4]=iblk[48];	ib[5]=iblk[ 0];
	ib[6]=iblk[ 8];	ib[7]=iblk[40];

	oblk[0]=ib[3];	oblk[8]=ib[6];
	oblk[16]=ib[4];	oblk[24]=ib[2];
	oblk[32]=ib[5];	oblk[40]=ib[7];
	oblk[48]=ib[0];	oblk[56]=ib[1];
}

void BGBBTJ_JPG_TransHAD(byte *iblk, short *oblk)
{
	int s[DCTSZ2];
	int t[DCTSZ2];
	int i, j;

	BGBBTJ_JPG_TransHAD_Horiz(iblk+0, s+0);
	BGBBTJ_JPG_TransHAD_Horiz(iblk+8, s+8);
	BGBBTJ_JPG_TransHAD_Horiz(iblk+16, s+16);
	BGBBTJ_JPG_TransHAD_Horiz(iblk+24, s+24);
	BGBBTJ_JPG_TransHAD_Horiz(iblk+32, s+32);
	BGBBTJ_JPG_TransHAD_Horiz(iblk+40, s+40);
	BGBBTJ_JPG_TransHAD_Horiz(iblk+48, s+48);
	BGBBTJ_JPG_TransRDCT_Horiz(iblk+56, s+56);

	BGBBTJ_JPG_TransRDCT_Vert(s+0, t+0);
	BGBBTJ_JPG_TransRDCT_Vert(s+1, t+1);
	BGBBTJ_JPG_TransRDCT_Vert(s+2, t+2);
	BGBBTJ_JPG_TransRDCT_Vert(s+3, t+3);
	BGBBTJ_JPG_TransRDCT_Vert(s+4, t+4);
	BGBBTJ_JPG_TransRDCT_Vert(s+5, t+5);
	BGBBTJ_JPG_TransRDCT_Vert(s+6, t+6);
	BGBBTJ_JPG_TransRDCT_Vert(s+7, t+7);

	for(i=0; i<64; i++)
		oblk[i]=t[i];
}
#endif
