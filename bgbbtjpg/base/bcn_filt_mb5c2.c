#include <bgbbtj.h>

#if 1
void BGBBTJ_BCn_EncodeBlockMB1CF_BGRA(byte *block,
	byte *rgba, int xstride, int ystride,
	byte *rgbastrt, byte *rgbaend)
{
	static char idxtaba[16]=
		{ 0,0,0,0, 0,0,2,2, 3,3,1,1, 1,1,1,1 };
	static char idxtabb[16]=
		{ 1,1,1,1, 1,1,3,3, 2,2,0,0, 0,0,0,0 };
	static char idxtabc[16]=
		{ 0,0,0,0, 0,0,2,2, 2,2,1,1, 1,1,1,1 };
	static char idxtabd[16]=
		{ 1,1,1,1, 1,1,2,2, 2,2,0,0, 0,0,0,0 };
	char *idxtab;
	int p0, p1, p2, p3, p4, p5, p6, p7;
	int l0, l1, l2, l3, l3a, l3b;
	int mcr, mcg, mcb, mca, mcy;
	int ncr, ncg, ncb, nca, ncy;
	int acr, acg, acb, aca, acy;
	int cr, cg, cb, ca, cy, cyr, cyg, cyb;
	int cr0, cg0, cb0, cy0;
	int cr1, cg1, cb1, cy1;
	int cr2, cg2, cb2, cy2;
	int cr3, cg3, cb3, cy3;
	int i, j, k, l;

	byte *rgba0, *rgba1, *rgba2, *rgba3;
	byte *rgba0e, *rgba1e, *rgba2e, *rgba3e;
	int mcr0, mcg0, mcb0, mcy0;
	int ncr0, ncg0, ncb0, ncy0;
	int mcr1, mcg1, mcb1, mcy1;
	int ncr1, ncg1, ncb1, ncy1;
	int mcr2, mcg2, mcb2, mcy2;
	int ncr2, ncg2, ncb2, ncy2;
	int mcr3, mcg3, mcb3, mcy3;
	int ncr3, ncg3, ncb3, ncy3;
	int l00, l01, l02, l03;
	int l10, l11, l12, l13;
	int l20, l21, l22, l23;
	int l30, l31, l32, l33;
	int cr00, cg00, cb00, cy00, cr01, cg01, cb01, cy01;
	int cr02, cg02, cb02, cy02, cr03, cg03, cb03, cy03;
	int cr10, cg10, cb10, cy10, cr11, cg11, cb11, cy11;
	int cr12, cg12, cb12, cy12, cr13, cg13, cb13, cy13;
	int cr20, cg20, cb20, cy20, cr21, cg21, cb21, cy21;
	int cr22, cg22, cb22, cy22, cr23, cg23, cb23, cy23;
	int cr30, cg30, cb30, cy30, cr31, cg31, cb31, cy31;
	int cr32, cg32, cb32, cy32, cr33, cg33, cb33, cy33;

#if 1
	rgba3=rgba+3*ystride+4*xstride;
//	if( (rgba<rgbastrt) || (rgba>rgbaend) ||
//		(rgba3<rgbastrt) || (rgba3>rgbaend))
	if( (rgba<rgbastrt) || (rgba3>rgbaend))
	{
		rgba0=rgba+(0*ystride);
		rgba1=rgba+(1*ystride);
		rgba2=rgba+(2*ystride);
		rgba3=rgba+(3*ystride);
		rgba0e=rgba0+(4*xstride);
		rgba1e=rgba1+(4*xstride);
		rgba2e=rgba2+(4*xstride);
		rgba3e=rgba3+(4*xstride);
		l0=0*xstride;		l1=1*xstride;
		l2=2*xstride;		l3=3*xstride;
		
		if((rgba0>=rgbastrt) && (rgba0e<=rgbaend))
		{
			cr00=rgba0[l0+2];	cg00=rgba0[l0+1]; cb00=rgba0[l0+0];
			cr01=rgba0[l1+2];	cg01=rgba0[l1+1]; cb01=rgba0[l1+0];
			cr02=rgba0[l2+2];	cg02=rgba0[l2+1]; cb02=rgba0[l2+0];
			cr03=rgba0[l3+2];	cg03=rgba0[l3+1]; cb03=rgba0[l3+0];
		}
		if((rgba1>=rgbastrt) && (rgba1e<=rgbaend))
		{
			cr10=rgba1[l0+2];	cg10=rgba1[l0+1]; cb10=rgba1[l0+0];
			cr11=rgba1[l1+2];	cg11=rgba1[l1+1]; cb11=rgba1[l1+0];
			cr12=rgba1[l2+2];	cg12=rgba1[l2+1]; cb12=rgba1[l2+0];
			cr13=rgba1[l3+2];	cg13=rgba1[l3+1]; cb13=rgba1[l3+0];
		}
		if((rgba2>=rgbastrt) && (rgba2e<=rgbaend))
		{
			cr20=rgba2[l0+2];	cg20=rgba2[l0+1]; cb20=rgba2[l0+0];
			cr21=rgba2[l1+2];	cg21=rgba2[l1+1]; cb21=rgba2[l1+0];
			cr22=rgba2[l2+2];	cg22=rgba2[l2+1]; cb22=rgba2[l2+0];
			cr23=rgba2[l3+2];	cg23=rgba2[l3+1]; cb23=rgba2[l3+0];
		}
		if((rgba3>=rgbastrt) && (rgba3e<=rgbaend))
		{
			cr30=rgba3[l0+2];	cg30=rgba3[l0+1]; cb30=rgba3[l0+0];
			cr31=rgba3[l1+2];	cg31=rgba3[l1+1]; cb31=rgba3[l1+0];
			cr32=rgba3[l2+2];	cg32=rgba3[l2+1]; cb32=rgba3[l2+0];
			cr33=rgba3[l3+2];	cg33=rgba3[l3+1]; cb33=rgba3[l3+0];
		}
	}else if(xstride==4)
	{
		rgba0=rgba+(0*ystride);
		rgba1=rgba+(1*ystride);
		rgba2=rgba+(2*ystride);
		rgba3=rgba+(3*ystride);

		cr00=rgba0[ 0+2];	cg00=rgba0[ 0+1]; cb00=rgba0[ 0+0];
		cr01=rgba0[ 4+2];	cg01=rgba0[ 4+1]; cb01=rgba0[ 4+0];
		cr02=rgba0[ 8+2];	cg02=rgba0[ 8+1]; cb02=rgba0[ 8+0];
		cr03=rgba0[12+2];	cg03=rgba0[12+1]; cb03=rgba0[12+0];
		cr10=rgba1[ 0+2];	cg10=rgba1[ 0+1]; cb10=rgba1[ 0+0];
		cr11=rgba1[ 4+2];	cg11=rgba1[ 4+1]; cb11=rgba1[ 4+0];
		cr12=rgba1[ 8+2];	cg12=rgba1[ 8+1]; cb12=rgba1[ 8+0];
		cr13=rgba1[12+2];	cg13=rgba1[12+1]; cb13=rgba1[12+0];
		cr20=rgba2[ 0+2];	cg20=rgba2[ 0+1]; cb20=rgba2[ 0+0];
		cr21=rgba2[ 4+2];	cg21=rgba2[ 4+1]; cb21=rgba2[ 4+0];
		cr22=rgba2[ 8+2];	cg22=rgba2[ 8+1]; cb22=rgba2[ 8+0];
		cr23=rgba2[12+2];	cg23=rgba2[12+1]; cb23=rgba2[12+0];
		cr30=rgba3[ 0+2];	cg30=rgba3[ 0+1]; cb30=rgba3[ 0+0];
		cr31=rgba3[ 4+2];	cg31=rgba3[ 4+1]; cb31=rgba3[ 4+0];
		cr32=rgba3[ 8+2];	cg32=rgba3[ 8+1]; cb32=rgba3[ 8+0];
		cr33=rgba3[12+2];	cg33=rgba3[12+1]; cb33=rgba3[12+0];
	}else
	{
		l00=0*ystride;		l10=1*ystride;
		l20=2*ystride;		l30=3*ystride;
		l01=l00+xstride;	l11=l10+xstride;
		l21=l20+xstride;	l31=l30+xstride;
		l02=l01+xstride;	l12=l11+xstride;
		l22=l21+xstride;	l32=l31+xstride;
		l03=l02+xstride;	l13=l12+xstride;
		l23=l22+xstride;	l33=l32+xstride;

		cr00=rgba[l00+2];	cg00=rgba[l00+1]; cb00=rgba[l00+0];
		cr01=rgba[l01+2];	cg01=rgba[l01+1]; cb01=rgba[l01+0];
		cr02=rgba[l02+2];	cg02=rgba[l02+1]; cb02=rgba[l02+0];
		cr03=rgba[l03+2];	cg03=rgba[l03+1]; cb03=rgba[l03+0];
		cr10=rgba[l10+2];	cg10=rgba[l10+1]; cb10=rgba[l10+0];
		cr11=rgba[l11+2];	cg11=rgba[l11+1]; cb11=rgba[l11+0];
		cr12=rgba[l12+2];	cg12=rgba[l12+1]; cb12=rgba[l12+0];
		cr13=rgba[l13+2];	cg13=rgba[l13+1]; cb13=rgba[l13+0];
		cr20=rgba[l20+2];	cg20=rgba[l20+1]; cb20=rgba[l20+0];
		cr21=rgba[l21+2];	cg21=rgba[l21+1]; cb21=rgba[l21+0];
		cr22=rgba[l22+2];	cg22=rgba[l22+1]; cb22=rgba[l22+0];
		cr23=rgba[l23+2];	cg23=rgba[l23+1]; cb23=rgba[l23+0];
		cr30=rgba[l30+2];	cg30=rgba[l30+1]; cb30=rgba[l30+0];
		cr31=rgba[l31+2];	cg31=rgba[l31+1]; cb31=rgba[l31+0];
		cr32=rgba[l32+2];	cg32=rgba[l32+1]; cb32=rgba[l32+0];
		cr33=rgba[l33+2];	cg33=rgba[l33+1]; cb33=rgba[l33+0];
	}

#if 0
	cy00=(cr00+(2*cg00)+cb00)>>2;	cy01=(cr01+(2*cg01)+cb01)>>2;
	cy02=(cr02+(2*cg02)+cb02)>>2;	cy03=(cr03+(2*cg03)+cb03)>>2;
	cy10=(cr10+(2*cg10)+cb10)>>2;	cy11=(cr11+(2*cg11)+cb11)>>2;
	cy12=(cr12+(2*cg12)+cb12)>>2;	cy13=(cr13+(2*cg13)+cb13)>>2;
	cy20=(cr20+(2*cg20)+cb20)>>2;	cy21=(cr21+(2*cg21)+cb21)>>2;
	cy22=(cr22+(2*cg22)+cb22)>>2;	cy23=(cr23+(2*cg23)+cb23)>>2;
	cy30=(cr30+(2*cg30)+cb30)>>2;	cy31=(cr31+(2*cg31)+cb31)>>2;
	cy32=(cr32+(2*cg32)+cb32)>>2;	cy33=(cr33+(2*cg33)+cb33)>>2;
#endif

#if 1
	cy00=(5*cr00+8*cg00+3*cb00)>>4;	cy01=(5*cr01+8*cg01+3*cb01)>>4;
	cy02=(5*cr02+8*cg02+3*cb02)>>4;	cy03=(5*cr03+8*cg03+3*cb03)>>4;
	cy10=(5*cr10+8*cg10+3*cb10)>>4;	cy11=(5*cr11+8*cg11+3*cb11)>>4;
	cy12=(5*cr12+8*cg12+3*cb12)>>4;	cy13=(5*cr13+8*cg13+3*cb13)>>4;
	cy20=(5*cr20+8*cg20+3*cb20)>>4;	cy21=(5*cr21+8*cg21+3*cb21)>>4;
	cy22=(5*cr22+8*cg22+3*cb22)>>4;	cy23=(5*cr23+8*cg23+3*cb23)>>4;
	cy30=(5*cr30+8*cg30+3*cb30)>>4;	cy31=(5*cr31+8*cg31+3*cb31)>>4;
	cy32=(5*cr32+8*cg32+3*cb32)>>4;	cy33=(5*cr33+8*cg33+3*cb33)>>4;
#endif

#if 0
	cy00=(3*cr00+4*cg00+cb00)>>3;	cy01=(3*cr01+4*cg01+cb01)>>3;
	cy02=(3*cr02+4*cg02+cb02)>>3;	cy03=(3*cr03+4*cg03+cb03)>>3;
	cy10=(3*cr10+4*cg10+cb10)>>3;	cy11=(3*cr11+4*cg11+cb11)>>3;
	cy12=(3*cr12+4*cg12+cb12)>>3;	cy13=(3*cr13+4*cg13+cb13)>>3;
	cy20=(3*cr20+4*cg20+cb20)>>3;	cy21=(3*cr21+4*cg21+cb21)>>3;
	cy22=(3*cr22+4*cg22+cb22)>>3;	cy23=(3*cr23+4*cg23+cb23)>>3;
	cy30=(3*cr30+4*cg30+cb30)>>3;	cy31=(3*cr31+4*cg31+cb31)>>3;
	cy32=(3*cr32+4*cg32+cb32)>>3;	cy33=(3*cr33+4*cg33+cb33)>>3;
#endif

#if 0
	if(1)			{ mcr0=cr00; mcg0=cg00; mcb0=cb00; mcy0=cy00; }
	if(cy01<mcy0)	{ mcr0=cr01; mcg0=cg01; mcb0=cb01; mcy0=cy01; }
	if(cy10<mcy0)	{ mcr0=cr10; mcg0=cg10; mcb0=cb10; mcy0=cy10; }
	if(cy11<mcy0)	{ mcr0=cr11; mcg0=cg11; mcb0=cb11; mcy0=cy11; }
	if(1)			{ mcr1=cr02; mcg1=cg02; mcb1=cb02; mcy1=cy02; }
	if(cy03<mcy1)	{ mcr1=cr03; mcg1=cg03; mcb1=cb03; mcy1=cy03; }
	if(cy12<mcy1)	{ mcr1=cr12; mcg1=cg12; mcb1=cb12; mcy1=cy12; }
	if(cy13<mcy1)	{ mcr1=cr13; mcg1=cg13; mcb1=cb13; mcy1=cy13; }
	if(1)			{ mcr2=cr20; mcg2=cg20; mcb2=cb20; mcy2=cy20; }
	if(cy21<mcy2)	{ mcr2=cr21; mcg2=cg21; mcb2=cb21; mcy2=cy21; }
	if(cy30<mcy2)	{ mcr2=cr30; mcg2=cg30; mcb2=cb30; mcy2=cy30; }
	if(cy31<mcy2)	{ mcr2=cr31; mcg2=cg31; mcb2=cb31; mcy2=cy31; }
	if(1)			{ mcr3=cr22; mcg3=cg22; mcb3=cb22; mcy3=cy22; }
	if(cy23<mcy3)	{ mcr3=cr23; mcg3=cg23; mcb3=cb23; mcy3=cy23; }
	if(cy32<mcy3)	{ mcr3=cr32; mcg3=cg32; mcb3=cb32; mcy3=cy32; }
	if(cy33<mcy3)	{ mcr3=cr33; mcg3=cg33; mcb3=cb33; mcy3=cy33; }

	if(1)			{ ncr0=cr00; ncg0=cg00; ncb0=cb00; ncy0=cy00; }
	if(cy01>ncy0)	{ ncr0=cr01; ncg0=cg01; ncb0=cb01; ncy0=cy01; }
	if(cy10>ncy0)	{ ncr0=cr10; ncg0=cg10; ncb0=cb10; ncy0=cy10; }
	if(cy11>ncy0)	{ ncr0=cr11; ncg0=cg11; ncb0=cb11; ncy0=cy11; }
	if(1)			{ ncr1=cr02; ncg1=cg02; ncb1=cb02; ncy1=cy02; }
	if(cy03>ncy1)	{ ncr1=cr03; ncg1=cg03; ncb1=cb03; ncy1=cy03; }
	if(cy12>ncy1)	{ ncr1=cr12; ncg1=cg12; ncb1=cb12; ncy1=cy12; }
	if(cy13>ncy1)	{ ncr1=cr13; ncg1=cg13; ncb1=cb13; ncy1=cy13; }
	if(1)			{ ncr2=cr20; ncg2=cg20; ncb2=cb20; ncy2=cy20; }
	if(cy21>ncy2)	{ ncr2=cr21; ncg2=cg21; ncb2=cb21; ncy2=cy21; }
	if(cy30>ncy2)	{ ncr2=cr30; ncg2=cg30; ncb2=cb30; ncy2=cy30; }
	if(cy31>ncy2)	{ ncr2=cr31; ncg2=cg31; ncb2=cb31; ncy2=cy31; }
	if(1)			{ ncr3=cr22; ncg3=cg22; ncb3=cb22; ncy3=cy22; }
	if(cy23>ncy3)	{ ncr3=cr23; ncg3=cg23; ncb3=cb23; ncy3=cy23; }
	if(cy32>ncy3)	{ ncr3=cr32; ncg3=cg32; ncb3=cb32; ncy3=cy32; }
	if(cy33>ncy3)	{ ncr3=cr33; ncg3=cg33; ncb3=cb33; ncy3=cy33; }

	if(1)			{ mcr=mcr0; mcg=mcg0; mcb=mcb0; mcy=mcy0; }
	if(mcy1<mcy)	{ mcr=mcr1; mcg=mcg1; mcb=mcb1; mcy=mcy1; }
	if(mcy2<mcy)	{ mcr=mcr2; mcg=mcg2; mcb=mcb2; mcy=mcy2; }
	if(mcy3<mcy)	{ mcr=mcr3; mcg=mcg3; mcb=mcb3; mcy=mcy3; }
	if(1)			{ ncr=ncr0; ncg=ncg0; ncb=ncb0; ncy=ncy0; }
	if(ncy1>ncy)	{ ncr=ncr1; ncg=ncg1; ncb=ncb1; ncy=ncy1; }
	if(ncy2>ncy)	{ ncr=ncr2; ncg=ncg2; ncb=ncb2; ncy=ncy2; }
	if(ncy3>ncy)	{ ncr=ncr3; ncg=ncg3; ncb=ncb3; ncy=ncy3; }
#endif

#if 0
	if(1)			{ mcr=cr00; mcg=cg00; mcb=cb00; mcy=cy00; }
	if(cy01<mcy)	{ mcr=cr01; mcg=cg01; mcb=cb01; mcy=cy01; }
	if(cy10<mcy)	{ mcr=cr10; mcg=cg10; mcb=cb10; mcy=cy10; }
	if(cy11<mcy)	{ mcr=cr11; mcg=cg11; mcb=cb11; mcy=cy11; }
	if(cy02<mcy)	{ mcr=cr02; mcg=cg02; mcb=cb02; mcy=cy02; }
	if(cy03<mcy)	{ mcr=cr03; mcg=cg03; mcb=cb03; mcy=cy03; }
	if(cy12<mcy)	{ mcr=cr12; mcg=cg12; mcb=cb12; mcy=cy12; }
	if(cy13<mcy)	{ mcr=cr13; mcg=cg13; mcb=cb13; mcy=cy13; }
	if(cy20<mcy)	{ mcr=cr20; mcg=cg20; mcb=cb20; mcy=cy20; }
	if(cy21<mcy)	{ mcr=cr21; mcg=cg21; mcb=cb21; mcy=cy21; }
	if(cy30<mcy)	{ mcr=cr30; mcg=cg30; mcb=cb30; mcy=cy30; }
	if(cy31<mcy)	{ mcr=cr31; mcg=cg31; mcb=cb31; mcy=cy31; }
	if(cy22<mcy)	{ mcr=cr22; mcg=cg22; mcb=cb22; mcy=cy22; }
	if(cy23<mcy)	{ mcr=cr23; mcg=cg23; mcb=cb23; mcy=cy23; }
	if(cy32<mcy)	{ mcr=cr32; mcg=cg32; mcb=cb32; mcy=cy32; }
	if(cy33<mcy)	{ mcr=cr33; mcg=cg33; mcb=cb33; mcy=cy33; }

	if(1)			{ ncr=cr00; ncg=cg00; ncb=cb00; ncy=cy00; }
	if(cy01>ncy)	{ ncr=cr01; ncg=cg01; ncb=cb01; ncy=cy01; }
	if(cy10>ncy)	{ ncr=cr10; ncg=cg10; ncb=cb10; ncy=cy10; }
	if(cy11>ncy)	{ ncr=cr11; ncg=cg11; ncb=cb11; ncy=cy11; }
	if(cy02>ncy)	{ ncr=cr02; ncg=cg02; ncb=cb02; ncy=cy02; }
	if(cy03>ncy)	{ ncr=cr03; ncg=cg03; ncb=cb03; ncy=cy03; }
	if(cy12>ncy)	{ ncr=cr12; ncg=cg12; ncb=cb12; ncy=cy12; }
	if(cy13>ncy)	{ ncr=cr13; ncg=cg13; ncb=cb13; ncy=cy13; }
	if(cy20>ncy)	{ ncr=cr20; ncg=cg20; ncb=cb20; ncy=cy20; }
	if(cy21>ncy)	{ ncr=cr21; ncg=cg21; ncb=cb21; ncy=cy21; }
	if(cy30>ncy)	{ ncr=cr30; ncg=cg30; ncb=cb30; ncy=cy30; }
	if(cy31>ncy)	{ ncr=cr31; ncg=cg31; ncb=cb31; ncy=cy31; }
	if(cy22>ncy)	{ ncr=cr22; ncg=cg22; ncb=cb22; ncy=cy22; }
	if(cy23>ncy)	{ ncr=cr23; ncg=cg23; ncb=cb23; ncy=cy23; }
	if(cy32>ncy)	{ ncr=cr32; ncg=cg32; ncb=cb32; ncy=cy32; }
	if(cy33>ncy)	{ ncr=cr33; ncg=cg33; ncb=cb33; ncy=cy33; }
#endif

#if 0
	// 00 01 02 03
	// 10 11 12 13
	// 20 21 22 23
	// 30 31 32 33
	
	//00, 33, 03, 30,
	//01, 32, 13, 20
	//02, 10, 31, 23
	//11, 22, 12, 21

	if(1)			{ mcr=cr00; mcg=cg00; mcb=cb00; mcy=cy00; }
	if(cy33<mcy)	{ mcr=cr33; mcg=cg33; mcb=cb33; mcy=cy33; }
	if(cy03<mcy)	{ mcr=cr03; mcg=cg03; mcb=cb03; mcy=cy03; }
	if(cy30<mcy)	{ mcr=cr30; mcg=cg30; mcb=cb30; mcy=cy30; }
	if(cy01<mcy)	{ mcr=cr01; mcg=cg01; mcb=cb01; mcy=cy01; }
	if(cy32<mcy)	{ mcr=cr32; mcg=cg32; mcb=cb32; mcy=cy32; }
	if(cy13<mcy)	{ mcr=cr13; mcg=cg13; mcb=cb13; mcy=cy13; }
	if(cy20<mcy)	{ mcr=cr20; mcg=cg20; mcb=cb20; mcy=cy20; }
	if(cy02<mcy)	{ mcr=cr02; mcg=cg02; mcb=cb02; mcy=cy02; }
	if(cy10<mcy)	{ mcr=cr10; mcg=cg10; mcb=cb10; mcy=cy10; }
	if(cy31<mcy)	{ mcr=cr31; mcg=cg31; mcb=cb31; mcy=cy31; }
	if(cy23<mcy)	{ mcr=cr23; mcg=cg23; mcb=cb23; mcy=cy23; }
	if(cy11<mcy)	{ mcr=cr11; mcg=cg11; mcb=cb11; mcy=cy11; }
	if(cy22<mcy)	{ mcr=cr22; mcg=cg22; mcb=cb22; mcy=cy22; }
	if(cy12<mcy)	{ mcr=cr12; mcg=cg12; mcb=cb12; mcy=cy12; }
	if(cy21<mcy)	{ mcr=cr21; mcg=cg21; mcb=cb21; mcy=cy21; }

	if(1)			{ ncr=cr00; ncg=cg00; ncb=cb00; ncy=cy00; }
	if(cy33>ncy)	{ ncr=cr33; ncg=cg33; ncb=cb33; ncy=cy33; }
	if(cy03>ncy)	{ ncr=cr03; ncg=cg03; ncb=cb03; ncy=cy03; }
	if(cy30>ncy)	{ ncr=cr30; ncg=cg30; ncb=cb30; ncy=cy30; }
	if(cy01>ncy)	{ ncr=cr01; ncg=cg01; ncb=cb01; ncy=cy01; }
	if(cy32>ncy)	{ ncr=cr32; ncg=cg32; ncb=cb32; ncy=cy32; }
	if(cy13>ncy)	{ ncr=cr13; ncg=cg13; ncb=cb13; ncy=cy13; }
	if(cy20>ncy)	{ ncr=cr20; ncg=cg20; ncb=cb20; ncy=cy20; }
	if(cy02>ncy)	{ ncr=cr02; ncg=cg02; ncb=cb02; ncy=cy02; }
	if(cy10>ncy)	{ ncr=cr10; ncg=cg10; ncb=cb10; ncy=cy10; }
	if(cy31>ncy)	{ ncr=cr31; ncg=cg31; ncb=cb31; ncy=cy31; }
	if(cy23>ncy)	{ ncr=cr23; ncg=cg23; ncb=cb23; ncy=cy23; }
	if(cy11>ncy)	{ ncr=cr11; ncg=cg11; ncb=cb11; ncy=cy11; }
	if(cy22>ncy)	{ ncr=cr22; ncg=cg22; ncb=cb22; ncy=cy22; }
	if(cy12>ncy)	{ ncr=cr12; ncg=cg12; ncb=cb12; ncy=cy12; }
	if(cy21>ncy)	{ ncr=cr21; ncg=cg21; ncb=cb21; ncy=cy21; }
#endif

#if 1
	// 00 01 02 03
	// 10 11 12 13
	// 20 21 22 23
	// 30 31 32 33
	
	//00, 33, 03, 30,
	//11, 22,
	//01, 32, 13, 20,
	//12, 21,
	//02, 10, 31, 23

	if(1)			{ mcr=cr00; mcg=cg00; mcb=cb00; mcy=cy00; }
	if(cy33<mcy)	{ mcr=cr33; mcg=cg33; mcb=cb33; mcy=cy33; }
	if(cy03<mcy)	{ mcr=cr03; mcg=cg03; mcb=cb03; mcy=cy03; }
	if(cy30<mcy)	{ mcr=cr30; mcg=cg30; mcb=cb30; mcy=cy30; }
	if(cy11<mcy)	{ mcr=cr11; mcg=cg11; mcb=cb11; mcy=cy11; }
	if(cy22<mcy)	{ mcr=cr22; mcg=cg22; mcb=cb22; mcy=cy22; }
	if(cy01<mcy)	{ mcr=cr01; mcg=cg01; mcb=cb01; mcy=cy01; }
	if(cy32<mcy)	{ mcr=cr32; mcg=cg32; mcb=cb32; mcy=cy32; }
	if(cy13<mcy)	{ mcr=cr13; mcg=cg13; mcb=cb13; mcy=cy13; }
	if(cy20<mcy)	{ mcr=cr20; mcg=cg20; mcb=cb20; mcy=cy20; }
	if(cy12<mcy)	{ mcr=cr12; mcg=cg12; mcb=cb12; mcy=cy12; }
	if(cy21<mcy)	{ mcr=cr21; mcg=cg21; mcb=cb21; mcy=cy21; }
	if(cy02<mcy)	{ mcr=cr02; mcg=cg02; mcb=cb02; mcy=cy02; }
	if(cy10<mcy)	{ mcr=cr10; mcg=cg10; mcb=cb10; mcy=cy10; }
	if(cy31<mcy)	{ mcr=cr31; mcg=cg31; mcb=cb31; mcy=cy31; }
	if(cy23<mcy)	{ mcr=cr23; mcg=cg23; mcb=cb23; mcy=cy23; }

	if(1)			{ ncr=cr00; ncg=cg00; ncb=cb00; ncy=cy00; }
	if(cy33>ncy)	{ ncr=cr33; ncg=cg33; ncb=cb33; ncy=cy33; }
	if(cy03>ncy)	{ ncr=cr03; ncg=cg03; ncb=cb03; ncy=cy03; }
	if(cy30>ncy)	{ ncr=cr30; ncg=cg30; ncb=cb30; ncy=cy30; }
	if(cy11>ncy)	{ ncr=cr11; ncg=cg11; ncb=cb11; ncy=cy11; }
	if(cy22>ncy)	{ ncr=cr22; ncg=cg22; ncb=cb22; ncy=cy22; }
	if(cy01>ncy)	{ ncr=cr01; ncg=cg01; ncb=cb01; ncy=cy01; }
	if(cy32>ncy)	{ ncr=cr32; ncg=cg32; ncb=cb32; ncy=cy32; }
	if(cy13>ncy)	{ ncr=cr13; ncg=cg13; ncb=cb13; ncy=cy13; }
	if(cy20>ncy)	{ ncr=cr20; ncg=cg20; ncb=cb20; ncy=cy20; }
	if(cy12>ncy)	{ ncr=cr12; ncg=cg12; ncb=cb12; ncy=cy12; }
	if(cy21>ncy)	{ ncr=cr21; ncg=cg21; ncb=cb21; ncy=cy21; }
	if(cy02>ncy)	{ ncr=cr02; ncg=cg02; ncb=cb02; ncy=cy02; }
	if(cy10>ncy)	{ ncr=cr10; ncg=cg10; ncb=cb10; ncy=cy10; }
	if(cy31>ncy)	{ ncr=cr31; ncg=cg31; ncb=cb31; ncy=cy31; }
	if(cy23>ncy)	{ ncr=cr23; ncg=cg23; ncb=cb23; ncy=cy23; }
#endif
	
#endif

	mca=255; nca=255;
	mcr2=mcr; mcg2=mcg; mcb2=mcb;
	ncr2=ncr; ncg2=ncg; ncb2=ncb;

//	aca=(mca+nca)>>1;			//Median Alpha
//	acy=(mcy+ncy)>>1;			//Median Luma
//	l0=49152/(nca-aca+1);		//Fix-Point Scale (Alpha)
//	l1=32768/(ncy-acy+1);		//Fix-Point Scale (Luma)
//	l2=65536-2048;

//	if((mcr==ncr) && (mcg==ncg) && (mcb==ncb) && (mca==nca))
	if((mcr2==ncr2) && (mcg2==ncg2) && (mcb2==ncb2))
	{
#if 1
		i=((mcr<<8)&0xF800) | ((mcg<<3)&0x07E0) | ((mcb>>3)&0x001F);
		block[ 0]=i; block[ 1]=i>>8;
		block[ 2]=i; block[ 3]=i>>8;
		block[ 4]=0; block[ 5]=0;
		block[ 6]=0; block[ 7]=0;
		block[ 8]=mcr2;	block[ 9]=mcg2;
		block[10]=mcb2;	block[11]=mca;
		block[12]=ncr2;	block[13]=ncg2;
		block[14]=ncb2;	block[15]=nca;
#endif	

//		if((block[0]==0x55) || (block[0]==0xAA))
//			block[0]^=32;
	}else
	{
		//Emit RGB Block (Opaque)

		acy=(mcy+ncy)>>1;					//Median Luma
		l1=(32768-6144)/(ncy-acy+1);		//Fix-Point Scale (Luma)
		l3a=65536-1024;
		l3b=65536+1024;

		i=((mcr<<8)&0xF800) | ((mcg<<3)&0x07E0) | ((mcb>>3)&0x001F);
		j=((ncr<<8)&0xF800) | ((ncg<<3)&0x07E0) | ((ncb>>3)&0x001F);
//		k=((mcr3<<11)&0xF800) | ((mcg3<<5)&0x07E0) | ((mcb3)&0x001F);
//		l=((ncr3<<11)&0xF800) | ((ncg3<<5)&0x07E0) | ((ncb3)&0x001F);

		if(j<i)
		{
			block[ 0]=i; block[ 1]=i>>8;
			block[ 2]=j; block[ 3]=j>>8;
			block[ 8]=mcr2;	block[ 9]=mcg2;
			block[10]=mcb2;	block[11]=mca;
			block[12]=ncr2;	block[13]=ncg2;
			block[14]=ncb2;	block[15]=nca;
			idxtab=idxtaba;
		}else if(i<j)
		{
			block[ 0]=j; block[ 1]=j>>8;
			block[ 2]=i; block[ 3]=i>>8;
			block[ 8]=ncr2;	block[ 9]=ncg2;
			block[10]=ncb2;	block[11]=nca;
			block[12]=mcr2;	block[13]=mcg2;
			block[14]=mcb2;	block[15]=mca;
			idxtab=idxtabb;
		}else
		{
			block[ 0]=i; block[ 1]=i>>8;
			block[ 2]=j; block[ 3]=j>>8;
			block[ 8]=mcr2;	block[ 9]=mcg2;
			block[10]=mcb2;	block[11]=mca;
			block[12]=ncr2;	block[13]=ncg2;
			block[14]=ncb2;	block[15]=nca;
			idxtab=idxtabc;
		}

//		if((block[0]==0x55) || (block[0]==0xAA))
//			block[0]^=32;

		p0=idxtab[((cy00-acy)*l1+l3a)>>13];
		p1=idxtab[((cy01-acy)*l1+l3b)>>13];
		p2=idxtab[((cy02-acy)*l1+l3a)>>13];
		p3=idxtab[((cy03-acy)*l1+l3b)>>13];
		block[4]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
		p0=idxtab[((cy10-acy)*l1+l3b)>>13];
		p1=idxtab[((cy11-acy)*l1+l3a)>>13];
		p2=idxtab[((cy12-acy)*l1+l3b)>>13];
		p3=idxtab[((cy13-acy)*l1+l3a)>>13];
		block[5]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
		p0=idxtab[((cy20-acy)*l1+l3a)>>13];
		p1=idxtab[((cy21-acy)*l1+l3b)>>13];
		p2=idxtab[((cy22-acy)*l1+l3a)>>13];
		p3=idxtab[((cy23-acy)*l1+l3b)>>13];
		block[6]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
		p0=idxtab[((cy30-acy)*l1+l3b)>>13];
		p1=idxtab[((cy31-acy)*l1+l3a)>>13];
		p2=idxtab[((cy32-acy)*l1+l3b)>>13];
		p3=idxtab[((cy33-acy)*l1+l3a)>>13];
		block[7]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
	}
	
	if((block[ 0]==0x55) || (block[ 0]==0xAA))
	{
		block[ 0]^=0x20;
//		block[ 0]^=0x01;
	}
}
#endif

#if 1
void BGBBTJ_BCn_EncodeBlockMB1CF2_BGRA(byte *block,
	byte *rgba, int xstride, int ystride,
	byte *rgbastrt, byte *rgbaend)
{
	static char idxtaba[16]=
		{ 0,0,0,0, 0,0,2,2, 3,3,1,1, 1,1,1,1 };
	static char idxtabb[16]=
		{ 1,1,1,1, 1,1,3,3, 2,2,0,0, 0,0,0,0 };
	static char idxtabc[16]=
		{ 0,0,0,0, 0,0,2,2, 2,2,1,1, 1,1,1,1 };
	static char idxtabd[16]=
		{ 1,1,1,1, 1,1,2,2, 2,2,0,0, 0,0,0,0 };
	byte pxy[16];
	char *idxtab;
	byte *rgba0, *rgba1, *rgba2, *rgba3;
	byte *rgba0e, *rgba1e, *rgba2e, *rgba3e;
	int p0, p1, p2, p3, p4, p5, p6, p7;
	int l0, l1, l2, l3, l3a, l3b;
	int mcr, mcg, mcb, mca, mcy;
	int ncr, ncg, ncb, nca, ncy;
	int acr, acg, acb, aca, acy;

	int mcr0, mcg0, mcb0, mcy0;
	int ncr0, ncg0, ncb0, ncy0;
	int mcr1, mcg1, mcb1, mcy1;
	int ncr1, ncg1, ncb1, ncy1;
	int mcr2, mcg2, mcb2, mcy2;
	int ncr2, ncg2, ncb2, ncy2;
	int mcr3, mcg3, mcb3, mcy3;
	int ncr3, ncg3, ncb3, ncy3;
	int acr0, acg0, acb0, acy0;
	int acr1, acg1, acb1, acy1;
	int acr2, acg2, acb2, acy2;
	int acr3, acg3, acb3, acy3;

	int cr, cg, cb, ca, cy, cyr, cyg, cyb;
//	int cr0, cg0, cb0, cy0;
//	int cr1, cg1, cb1, cy1;
//	int cr2, cg2, cb2, cy2;
//	int cr3, cg3, cb3, cy3;
	int l00, l01, l02, l03;
	int l10, l11, l12, l13;
	int l20, l21, l22, l23;
	int l30, l31, l32, l33;
	int cr00, cg00, cb00, cy00, cr01, cg01, cb01, cy01;
	int cr02, cg02, cb02, cy02, cr03, cg03, cb03, cy03;
	int cr10, cg10, cb10, cy10, cr11, cg11, cb11, cy11;
	int cr12, cg12, cb12, cy12, cr13, cg13, cb13, cy13;
	int cr20, cg20, cb20, cy20, cr21, cg21, cb21, cy21;
	int cr22, cg22, cb22, cy22, cr23, cg23, cb23, cy23;
	int cr30, cg30, cb30, cy30, cr31, cg31, cb31, cy31;
	int cr32, cg32, cb32, cy32, cr33, cg33, cb33, cy33;
//	int mcy0a, ncy0a, mcy0b, ncy0b;
//	int mcy0c, ncy0c, mcy0d, ncy0d;
	int use2x2;
	int i, j, k, l;

#if 1
	rgba3=rgba+3*ystride+4*xstride;
//	if( (rgba<rgbastrt) || (rgba>rgbaend) ||
//		(rgba3<rgbastrt) || (rgba3>rgbaend))
	if( (rgba<rgbastrt) || (rgba3>rgbaend))
	{
		rgba0=rgba+(0*ystride);
		rgba1=rgba+(1*ystride);
		rgba2=rgba+(2*ystride);
		rgba3=rgba+(3*ystride);
		rgba0e=rgba0+(4*xstride);
		rgba1e=rgba1+(4*xstride);
		rgba2e=rgba2+(4*xstride);
		rgba3e=rgba3+(4*xstride);
		l0=0*xstride;		l1=1*xstride;
		l2=2*xstride;		l3=3*xstride;
		
		if((rgba0>=rgbastrt) && (rgba0e<=rgbaend))
		{
			cr00=rgba0[l0+2];	cg00=rgba0[l0+1]; cb00=rgba0[l0+0];
			cr01=rgba0[l1+2];	cg01=rgba0[l1+1]; cb01=rgba0[l1+0];
			cr02=rgba0[l2+2];	cg02=rgba0[l2+1]; cb02=rgba0[l2+0];
			cr03=rgba0[l3+2];	cg03=rgba0[l3+1]; cb03=rgba0[l3+0];
		}
		if((rgba1>=rgbastrt) && (rgba1e<=rgbaend))
		{
			cr10=rgba1[l0+2];	cg10=rgba1[l0+1]; cb10=rgba1[l0+0];
			cr11=rgba1[l1+2];	cg11=rgba1[l1+1]; cb11=rgba1[l1+0];
			cr12=rgba1[l2+2];	cg12=rgba1[l2+1]; cb12=rgba1[l2+0];
			cr13=rgba1[l3+2];	cg13=rgba1[l3+1]; cb13=rgba1[l3+0];
		}
		if((rgba2>=rgbastrt) && (rgba2e<=rgbaend))
		{
			cr20=rgba2[l0+2];	cg20=rgba2[l0+1]; cb20=rgba2[l0+0];
			cr21=rgba2[l1+2];	cg21=rgba2[l1+1]; cb21=rgba2[l1+0];
			cr22=rgba2[l2+2];	cg22=rgba2[l2+1]; cb22=rgba2[l2+0];
			cr23=rgba2[l3+2];	cg23=rgba2[l3+1]; cb23=rgba2[l3+0];
		}
		if((rgba3>=rgbastrt) && (rgba3e<=rgbaend))
		{
			cr30=rgba3[l0+2];	cg30=rgba3[l0+1]; cb30=rgba3[l0+0];
			cr31=rgba3[l1+2];	cg31=rgba3[l1+1]; cb31=rgba3[l1+0];
			cr32=rgba3[l2+2];	cg32=rgba3[l2+1]; cb32=rgba3[l2+0];
			cr33=rgba3[l3+2];	cg33=rgba3[l3+1]; cb33=rgba3[l3+0];
		}
	}else if(xstride==4)
	{
		rgba0=rgba+(0*ystride);
		rgba1=rgba+(1*ystride);
		rgba2=rgba+(2*ystride);
		rgba3=rgba+(3*ystride);

		cr00=rgba0[ 0+2];	cg00=rgba0[ 0+1]; cb00=rgba0[ 0+0];
		cr01=rgba0[ 4+2];	cg01=rgba0[ 4+1]; cb01=rgba0[ 4+0];
		cr02=rgba0[ 8+2];	cg02=rgba0[ 8+1]; cb02=rgba0[ 8+0];
		cr03=rgba0[12+2];	cg03=rgba0[12+1]; cb03=rgba0[12+0];
		cr10=rgba1[ 0+2];	cg10=rgba1[ 0+1]; cb10=rgba1[ 0+0];
		cr11=rgba1[ 4+2];	cg11=rgba1[ 4+1]; cb11=rgba1[ 4+0];
		cr12=rgba1[ 8+2];	cg12=rgba1[ 8+1]; cb12=rgba1[ 8+0];
		cr13=rgba1[12+2];	cg13=rgba1[12+1]; cb13=rgba1[12+0];
		cr20=rgba2[ 0+2];	cg20=rgba2[ 0+1]; cb20=rgba2[ 0+0];
		cr21=rgba2[ 4+2];	cg21=rgba2[ 4+1]; cb21=rgba2[ 4+0];
		cr22=rgba2[ 8+2];	cg22=rgba2[ 8+1]; cb22=rgba2[ 8+0];
		cr23=rgba2[12+2];	cg23=rgba2[12+1]; cb23=rgba2[12+0];
		cr30=rgba3[ 0+2];	cg30=rgba3[ 0+1]; cb30=rgba3[ 0+0];
		cr31=rgba3[ 4+2];	cg31=rgba3[ 4+1]; cb31=rgba3[ 4+0];
		cr32=rgba3[ 8+2];	cg32=rgba3[ 8+1]; cb32=rgba3[ 8+0];
		cr33=rgba3[12+2];	cg33=rgba3[12+1]; cb33=rgba3[12+0];
	}else
	{
		l00=0*ystride;		l10=1*ystride;
		l20=2*ystride;		l30=3*ystride;
		l01=l00+xstride;	l11=l10+xstride;
		l21=l20+xstride;	l31=l30+xstride;
		l02=l01+xstride;	l12=l11+xstride;
		l22=l21+xstride;	l32=l31+xstride;
		l03=l02+xstride;	l13=l12+xstride;
		l23=l22+xstride;	l33=l32+xstride;

		cr00=rgba[l00+2];	cg00=rgba[l00+1]; cb00=rgba[l00+0];
		cr01=rgba[l01+2];	cg01=rgba[l01+1]; cb01=rgba[l01+0];
		cr02=rgba[l02+2];	cg02=rgba[l02+1]; cb02=rgba[l02+0];
		cr03=rgba[l03+2];	cg03=rgba[l03+1]; cb03=rgba[l03+0];
		cr10=rgba[l10+2];	cg10=rgba[l10+1]; cb10=rgba[l10+0];
		cr11=rgba[l11+2];	cg11=rgba[l11+1]; cb11=rgba[l11+0];
		cr12=rgba[l12+2];	cg12=rgba[l12+1]; cb12=rgba[l12+0];
		cr13=rgba[l13+2];	cg13=rgba[l13+1]; cb13=rgba[l13+0];
		cr20=rgba[l20+2];	cg20=rgba[l20+1]; cb20=rgba[l20+0];
		cr21=rgba[l21+2];	cg21=rgba[l21+1]; cb21=rgba[l21+0];
		cr22=rgba[l22+2];	cg22=rgba[l22+1]; cb22=rgba[l22+0];
		cr23=rgba[l23+2];	cg23=rgba[l23+1]; cb23=rgba[l23+0];
		cr30=rgba[l30+2];	cg30=rgba[l30+1]; cb30=rgba[l30+0];
		cr31=rgba[l31+2];	cg31=rgba[l31+1]; cb31=rgba[l31+0];
		cr32=rgba[l32+2];	cg32=rgba[l32+1]; cb32=rgba[l32+0];
		cr33=rgba[l33+2];	cg33=rgba[l33+1]; cb33=rgba[l33+0];
	}

#if 0
	cy00=(cr00+(2*cg00)+cb00)>>2;	cy01=(cr01+(2*cg01)+cb01)>>2;
	cy02=(cr02+(2*cg02)+cb02)>>2;	cy03=(cr03+(2*cg03)+cb03)>>2;
	cy10=(cr10+(2*cg10)+cb10)>>2;	cy11=(cr11+(2*cg11)+cb11)>>2;
	cy12=(cr12+(2*cg12)+cb12)>>2;	cy13=(cr13+(2*cg13)+cb13)>>2;
	cy20=(cr20+(2*cg20)+cb20)>>2;	cy21=(cr21+(2*cg21)+cb21)>>2;
	cy22=(cr22+(2*cg22)+cb22)>>2;	cy23=(cr23+(2*cg23)+cb23)>>2;
	cy30=(cr30+(2*cg30)+cb30)>>2;	cy31=(cr31+(2*cg31)+cb31)>>2;
	cy32=(cr32+(2*cg32)+cb32)>>2;	cy33=(cr33+(2*cg33)+cb33)>>2;
#endif

#if 1
	cy00=(5*cr00+8*cg00+3*cb00)>>4;	cy01=(5*cr01+8*cg01+3*cb01)>>4;
	cy02=(5*cr02+8*cg02+3*cb02)>>4;	cy03=(5*cr03+8*cg03+3*cb03)>>4;
	cy10=(5*cr10+8*cg10+3*cb10)>>4;	cy11=(5*cr11+8*cg11+3*cb11)>>4;
	cy12=(5*cr12+8*cg12+3*cb12)>>4;	cy13=(5*cr13+8*cg13+3*cb13)>>4;
	cy20=(5*cr20+8*cg20+3*cb20)>>4;	cy21=(5*cr21+8*cg21+3*cb21)>>4;
	cy22=(5*cr22+8*cg22+3*cb22)>>4;	cy23=(5*cr23+8*cg23+3*cb23)>>4;
	cy30=(5*cr30+8*cg30+3*cb30)>>4;	cy31=(5*cr31+8*cg31+3*cb31)>>4;
	cy32=(5*cr32+8*cg32+3*cb32)>>4;	cy33=(5*cr33+8*cg33+3*cb33)>>4;
#endif

	pxy[(0*4)+0]=cy00;	pxy[(0*4)+1]=cy01;
	pxy[(0*4)+2]=cy02;	pxy[(0*4)+3]=cy03;
	pxy[(1*4)+0]=cy10;	pxy[(1*4)+1]=cy11;
	pxy[(1*4)+2]=cy12;	pxy[(1*4)+3]=cy13;
	pxy[(2*4)+0]=cy20;	pxy[(2*4)+1]=cy21;
	pxy[(2*4)+2]=cy22;	pxy[(2*4)+3]=cy23;
	pxy[(3*4)+0]=cy30;	pxy[(3*4)+1]=cy31;
	pxy[(3*4)+2]=cy32;	pxy[(3*4)+3]=cy33;

	if(1)			{ mcr0=cr00; mcg0=cg00; mcb0=cb00; mcy0=cy00; }
	if(cy11<mcy0)	{ mcr0=cr11; mcg0=cg11; mcb0=cb11; mcy0=cy11; }
	if(cy01<mcy0)	{ mcr0=cr01; mcg0=cg01; mcb0=cb01; mcy0=cy01; }
	if(cy10<mcy0)	{ mcr0=cr10; mcg0=cg10; mcb0=cb10; mcy0=cy10; }
	if(1)			{ mcr1=cr02; mcg1=cg02; mcb1=cb02; mcy1=cy02; }
	if(cy13<mcy1)	{ mcr1=cr13; mcg1=cg13; mcb1=cb13; mcy1=cy13; }
	if(cy03<mcy1)	{ mcr1=cr03; mcg1=cg03; mcb1=cb03; mcy1=cy03; }
	if(cy12<mcy1)	{ mcr1=cr12; mcg1=cg12; mcb1=cb12; mcy1=cy12; }
	if(1)			{ mcr2=cr20; mcg2=cg20; mcb2=cb20; mcy2=cy20; }
	if(cy31<mcy2)	{ mcr2=cr31; mcg2=cg31; mcb2=cb31; mcy2=cy31; }
	if(cy21<mcy2)	{ mcr2=cr21; mcg2=cg21; mcb2=cb21; mcy2=cy21; }
	if(cy30<mcy2)	{ mcr2=cr30; mcg2=cg30; mcb2=cb30; mcy2=cy30; }
	if(1)			{ mcr3=cr22; mcg3=cg22; mcb3=cb22; mcy3=cy22; }
	if(cy33<mcy3)	{ mcr3=cr33; mcg3=cg33; mcb3=cb33; mcy3=cy33; }
	if(cy23<mcy3)	{ mcr3=cr23; mcg3=cg23; mcb3=cb23; mcy3=cy23; }
	if(cy32<mcy3)	{ mcr3=cr32; mcg3=cg32; mcb3=cb32; mcy3=cy32; }

	if(1)			{ ncr0=cr00; ncg0=cg00; ncb0=cb00; ncy0=cy00; }
	if(cy11>ncy0)	{ ncr0=cr11; ncg0=cg11; ncb0=cb11; ncy0=cy11; }
	if(cy01>ncy0)	{ ncr0=cr01; ncg0=cg01; ncb0=cb01; ncy0=cy01; }
	if(cy10>ncy0)	{ ncr0=cr10; ncg0=cg10; ncb0=cb10; ncy0=cy10; }
	if(1)			{ ncr1=cr02; ncg1=cg02; ncb1=cb02; ncy1=cy02; }
	if(cy13>ncy1)	{ ncr1=cr13; ncg1=cg13; ncb1=cb13; ncy1=cy13; }
	if(cy03>ncy1)	{ ncr1=cr03; ncg1=cg03; ncb1=cb03; ncy1=cy03; }
	if(cy12>ncy1)	{ ncr1=cr12; ncg1=cg12; ncb1=cb12; ncy1=cy12; }
	if(1)			{ ncr2=cr20; ncg2=cg20; ncb2=cb20; ncy2=cy20; }
	if(cy31>ncy2)	{ ncr2=cr31; ncg2=cg31; ncb2=cb31; ncy2=cy31; }
	if(cy21>ncy2)	{ ncr2=cr21; ncg2=cg21; ncb2=cb21; ncy2=cy21; }
	if(cy30>ncy2)	{ ncr2=cr30; ncg2=cg30; ncb2=cb30; ncy2=cy30; }
	if(1)			{ ncr3=cr22; ncg3=cg22; ncb3=cb22; ncy3=cy22; }
	if(cy33>ncy3)	{ ncr3=cr33; ncg3=cg33; ncb3=cb33; ncy3=cy33; }
	if(cy23>ncy3)	{ ncr3=cr23; ncg3=cg23; ncb3=cb23; ncy3=cy23; }
	if(cy32>ncy3)	{ ncr3=cr32; ncg3=cg32; ncb3=cb32; ncy3=cy32; }

	if(1)			{ mcr=mcr0; mcg=mcg0; mcb=mcb0; mcy=mcy0; }
	if(mcy3<mcy)	{ mcr=mcr3; mcg=mcg3; mcb=mcb3; mcy=mcy3; }
	if(mcy1<mcy)	{ mcr=mcr1; mcg=mcg1; mcb=mcb1; mcy=mcy1; }
	if(mcy2<mcy)	{ mcr=mcr2; mcg=mcg2; mcb=mcb2; mcy=mcy2; }
	if(1)			{ ncr=ncr0; ncg=ncg0; ncb=ncb0; ncy=ncy0; }
	if(ncy3>ncy)	{ ncr=ncr3; ncg=ncg3; ncb=ncb3; ncy=ncy3; }
	if(ncy1>ncy)	{ ncr=ncr1; ncg=ncg1; ncb=ncb1; ncy=ncy1; }
	if(ncy2>ncy)	{ ncr=ncr2; ncg=ncg2; ncb=ncb2; ncy=ncy2; }

//		if(cy0>ncy)	{ ncr=cr0; ncg=cg0; ncb=cb0; ncy=cy0; }
//		if(cy1>ncy)	{ ncr=cr1; ncg=cg1; ncb=cb1; ncy=cy1; }
//		if(cy2>ncy)	{ ncr=cr2; ncg=cg2; ncb=cb2; ncy=cy2; }
//		if(cy3>ncy)	{ ncr=cr3; ncg=cg3; ncb=cb3; ncy=cy3; }

//		acr+=cr0+cr1+cr2+cr3;
//		acg+=cg0+cg1+cg2+cg3;
//		acb+=cb0+cb1+cb2+cb3;
//		acy+=cy0+cy1+cy2+cy3;

#if 1
	acr0=(mcr0+ncr0)>>1;	acg0=(mcg0+ncg0)>>1;
	acb0=(mcb0+ncb0)>>1;	acy0=(mcy0+ncy0)>>1;
	acr1=(mcr1+ncr1)>>1;	acg1=(mcg1+ncg1)>>1;
	acb1=(mcb1+ncb1)>>1;	acy1=(mcy1+ncy1)>>1;
	acr2=(mcr2+ncr2)>>1;	acg2=(mcg2+ncg2)>>1;
	acb2=(mcb2+ncb2)>>1;	acy2=(mcy2+ncy2)>>1;
	acr3=(mcr3+ncr3)>>1;	acg3=(mcg3+ncg3)>>1;
	acb3=(mcb3+ncb3)>>1;	acy3=(mcy3+ncy3)>>1;
//	acr=(acr0+acr1+acr2+acr3)>>2;
//	acg=(acg0+acg1+acg2+acg3)>>2;
//	acb=(acb0+acb1+acb2+acb3)>>2;
//	acy=(acy0+acy1+acy2+acy3)>>2;
#endif

	acr=(mcr+ncr)>>1;	acg=(mcg+ncg)>>1;
	acb=(mcb+ncb)>>1;	acy=(mcy+ncy)>>1;

	use2x2=0;

#if 1
	cr=acr0-acr; cg=acg0-acg; cb=acb0-acb;
//	if((cr*cr+cg*cg+cb*cb)>768)use2x2=1;
//	if((cr*cr+cg*cg+cb*cb)>1536)use2x2=1;
//	if((cr*cr+cg*cg+cb*cb)>1024)use2x2=1;
	if((cr*cr+cg*cg+cb*cb)>4096)use2x2=1;
	cr=acr1-acr; cg=acg1-acg; cb=acb1-acb;
//	if((cr*cr+cg*cg+cb*cb)>768)use2x2=1;
//	if((cr*cr+cg*cg+cb*cb)>1536)use2x2=1;
//	if((cr*cr+cg*cg+cb*cb)>1024)use2x2=1;
	if((cr*cr+cg*cg+cb*cb)>4096)use2x2=1;
	cr=acr2-acr; cg=acg2-acg; cb=acb2-acb;
//	if((cr*cr+cg*cg+cb*cb)>768)use2x2=1;
//	if((cr*cr+cg*cg+cb*cb)>1536)use2x2=1;
//	if((cr*cr+cg*cg+cb*cb)>1024)use2x2=1;
	if((cr*cr+cg*cg+cb*cb)>4096)use2x2=1;
	cr=acr3-acr; cg=acg3-acg; cb=acb3-acb;
//	if((cr*cr+cg*cg+cb*cb)>768)use2x2=1;
//	if((cr*cr+cg*cg+cb*cb)>1536)use2x2=1;
//	if((cr*cr+cg*cg+cb*cb)>1024)use2x2=1;
	if((cr*cr+cg*cg+cb*cb)>4096)use2x2=1;
#endif

#if 0
	p2=acr-acg;		p3=acb-acg;

	p0=acr0-acg0;	p1=acb0-acg0;
	cr=p0-p2; cb=p1-p3;
//	if((cr*cr+cb*cb)>1024)use2x2=1;
//	if((cr*cr+cb*cb)>4096)use2x2=1;
	if((cr*cr+cb*cb)>16384)use2x2=1;

	p0=acr1-acg1;	p1=acb1-acg1;
	cr=p0-p2; cb=p1-p3;
//	if((cr*cr+cb*cb)>1024)use2x2=1;
//	if((cr*cr+cb*cb)>4096)use2x2=1;
	if((cr*cr+cb*cb)>16384)use2x2=1;

	p0=acr2-acg2;	p1=acb2-acg2;
	cr=p0-p2; cb=p1-p3;
//	if((cr*cr+cb*cb)>1024)use2x2=1;
//	if((cr*cr+cb*cb)>4096)use2x2=1;
	if((cr*cr+cb*cb)>16384)use2x2=1;

	p0=acr3-acg3;	p1=acb3-acg3;
	cr=p0-p2; cb=p1-p3;
//	if((cr*cr+cb*cb)>1024)use2x2=1;
//	if((cr*cr+cb*cb)>4096)use2x2=1;
	if((cr*cr+cb*cb)>16384)use2x2=1;
#endif

#endif

#if 1
	if(use2x2)
	{
		idxtab=idxtaba;

		p0=ncy0-mcy0;
		p1=ncy1-mcy1;
		p2=ncy2-mcy2;
		p3=ncy3-mcy3;
		p4=max(ncg0-mcg0, max(ncr0-mcr0, ncb0-mcb0));
		p5=max(ncg1-mcg1, max(ncr1-mcr1, ncb1-mcb1));
		p6=max(ncg2-mcg2, max(ncr2-mcr2, ncb2-mcb2));
		p7=max(ncg3-mcg3, max(ncr3-mcr3, ncb3-mcb3));
		p0=(p0+p4)>>1;
		p1=(p1+p5)>>1;
		p2=(p2+p6)>>1;
		p3=(p3+p7)>>1;

		block[ 0]=0x55; block[ 1]=0x00;
		block[ 2]=0xAB; block[ 3]=0x00;
		block[ 8]=acr0;	block[ 9]=acg0;
		block[10]=acb0;	block[11]=p0;
		block[12]=acr1;	block[13]=acg1;
		block[14]=acb1;	block[15]=p1;
		block[16]=acr2;	block[17]=acg2;
		block[18]=acb2;	block[19]=p2;
		block[20]=acr3;	block[21]=acg3;
		block[22]=acb3;	block[23]=p3;

		l0=(32768-6144)/(ncy0-acy0+1);
		l1=(32768-6144)/(ncy1-acy1+1);
		l2=(32768-6144)/(ncy2-acy2+1);
		l3=(32768-6144)/(ncy3-acy3+1);
		l3a=65536-1024;
		l3b=65536+1024;

		p0=idxtab[((pxy[ 0]-acy0)*l0+l3a)>>13];
		p1=idxtab[((pxy[ 1]-acy0)*l0+l3b)>>13];
		p2=idxtab[((pxy[ 2]-acy1)*l1+l3a)>>13];
		p3=idxtab[((pxy[ 3]-acy1)*l1+l3b)>>13];
		block[4]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
		p0=idxtab[((pxy[ 4]-acy0)*l0+l3b)>>13];
		p1=idxtab[((pxy[ 5]-acy0)*l0+l3a)>>13];
		p2=idxtab[((pxy[ 6]-acy1)*l1+l3b)>>13];
		p3=idxtab[((pxy[ 7]-acy1)*l1+l3a)>>13];
		block[5]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
		p0=idxtab[((pxy[ 8]-acy2)*l2+l3a)>>13];
		p1=idxtab[((pxy[ 9]-acy2)*l2+l3b)>>13];
		p2=idxtab[((pxy[10]-acy3)*l3+l3a)>>13];
		p3=idxtab[((pxy[11]-acy3)*l3+l3b)>>13];
		block[6]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
		p0=idxtab[((pxy[12]-acy2)*l2+l3b)>>13];
		p1=idxtab[((pxy[13]-acy2)*l2+l3a)>>13];
		p2=idxtab[((pxy[14]-acy3)*l3+l3b)>>13];
		p3=idxtab[((pxy[15]-acy3)*l3+l3a)>>13];
		block[7]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
		
		return;
	}
#endif

	mca=255; nca=255;

	mcr2=mcr; mcg2=mcg; mcb2=mcb;
	ncr2=ncr; ncg2=ncg; ncb2=ncb;

	aca=(mca+nca)>>1;			//Median Alpha
	acy=(mcy+ncy)>>1;			//Median Luma
	l0=49152/(nca-aca+1);		//Fix-Point Scale (Alpha)
	l1=32768/(ncy-acy+1);		//Fix-Point Scale (Luma)
	l2=65536-2048;

//	if((mcr==ncr) && (mcg==ncg) && (mcb==ncb) && (mca==nca))
	if((mcr2==ncr2) && (mcg2==ncg2) && (mcb2==ncb2))
	{	
#if 1
		i=((mcr<<8)&0xF800) | ((mcg<<3)&0x07E0) | ((mcb>>3)&0x001F);
		block[ 0]=i; block[ 1]=i>>8;
		block[ 2]=i; block[ 3]=i>>8;
		block[ 4]=0; block[ 5]=0;
		block[ 6]=0; block[ 7]=0;
		block[ 8]=mcr2;	block[ 9]=mcg2;
		block[10]=mcb2;	block[11]=mca;
		block[12]=ncr2;	block[13]=ncg2;
		block[14]=ncb2;	block[15]=nca;
#endif	
	}else
	{
		//Emit RGB Block (Opaque)

		l1=(32768-6144)/(ncy-acy+1);		//Fix-Point Scale (Luma)
		l3a=65536-1024;
		l3b=65536+1024;

		i=((mcr<<8)&0xF800) | ((mcg<<3)&0x07E0) | ((mcb>>3)&0x001F);
		j=((ncr<<8)&0xF800) | ((ncg<<3)&0x07E0) | ((ncb>>3)&0x001F);

		if(j<i)
		{
			block[ 0]=i; block[ 1]=i>>8;
			block[ 2]=j; block[ 3]=j>>8;
			block[ 8]=mcr2;	block[ 9]=mcg2;
			block[10]=mcb2;	block[11]=mca;
			block[12]=ncr2;	block[13]=ncg2;
			block[14]=ncb2;	block[15]=nca;
			idxtab=idxtaba;
		}else if(i<j)
		{
			block[ 0]=j; block[ 1]=j>>8;
			block[ 2]=i; block[ 3]=i>>8;
			block[ 8]=ncr2;	block[ 9]=ncg2;
			block[10]=ncb2;	block[11]=nca;
			block[12]=mcr2;	block[13]=mcg2;
			block[14]=mcb2;	block[15]=mca;
			idxtab=idxtabb;
		}else
		{
			block[ 0]=i; block[ 1]=i>>8;
			block[ 2]=j; block[ 3]=j>>8;
			block[ 8]=mcr2;	block[ 9]=mcg2;
			block[10]=mcb2;	block[11]=mca;
			block[12]=ncr2;	block[13]=ncg2;
			block[14]=ncb2;	block[15]=nca;
			idxtab=idxtabc;
		}

		p0=idxtab[((pxy[ 0]-acy)*l1+l3a)>>13];
		p1=idxtab[((pxy[ 1]-acy)*l1+l3b)>>13];
		p2=idxtab[((pxy[ 2]-acy)*l1+l3a)>>13];
		p3=idxtab[((pxy[ 3]-acy)*l1+l3b)>>13];
		block[4]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
		p0=idxtab[((pxy[ 4]-acy)*l1+l3b)>>13];
		p1=idxtab[((pxy[ 5]-acy)*l1+l3a)>>13];
		p2=idxtab[((pxy[ 6]-acy)*l1+l3b)>>13];
		p3=idxtab[((pxy[ 7]-acy)*l1+l3a)>>13];
		block[5]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
		p0=idxtab[((pxy[ 8]-acy)*l1+l3a)>>13];
		p1=idxtab[((pxy[ 9]-acy)*l1+l3b)>>13];
		p2=idxtab[((pxy[10]-acy)*l1+l3a)>>13];
		p3=idxtab[((pxy[11]-acy)*l1+l3b)>>13];
		block[6]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
		p0=idxtab[((pxy[12]-acy)*l1+l3b)>>13];
		p1=idxtab[((pxy[13]-acy)*l1+l3a)>>13];
		p2=idxtab[((pxy[14]-acy)*l1+l3b)>>13];
		p3=idxtab[((pxy[15]-acy)*l1+l3a)>>13];
		block[7]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
	}

	if((block[ 0]==0x55) || (block[ 0]==0xAA))
	{
		block[ 0]^=0x20;
//		block[ 0]^=0x01;
	}
}
#endif

void BGBBTJ_BCn_DecodeBlockMB1C_4ClrRGB24(
	byte *block, byte *rgba, int xstride, int ystride, int flag)
{
	byte clra[4*4], clrd[4];
	byte clrb[4*4*4];
	int mcr, mcg, mcb, ncr, ncg, ncb;
	int cr, cg, cb;
	int p0, p1, p2, p3;
	int i, j, k, l;
	
	if(flag&1)
	{
		clra[0*4+2]=block[ 8];
		clra[0*4+1]=block[ 9];
		clra[0*4+0]=block[10];
		clra[0*4+3]=255;
		clra[1*4+2]=block[12];
		clra[1*4+1]=block[13];
		clra[1*4+0]=block[14];
		clra[1*4+3]=255;
		clra[2*4+2]=block[16];
		clra[2*4+1]=block[17];
		clra[2*4+0]=block[18];
		clra[2*4+3]=255;
		clra[3*4+2]=block[20];
		clra[3*4+1]=block[21];
		clra[3*4+0]=block[22];
		clra[3*4+3]=255;
	}else
	{
		clra[0*4+0]=block[ 8];
		clra[0*4+1]=block[ 9];
		clra[0*4+2]=block[10];
		clra[0*4+3]=255;
		clra[1*4+0]=block[12];
		clra[1*4+1]=block[13];
		clra[1*4+2]=block[14];
		clra[1*4+3]=255;
		clra[2*4+0]=block[16];
		clra[2*4+1]=block[17];
		clra[2*4+2]=block[18];
		clra[2*4+3]=255;
		clra[3*4+0]=block[20];
		clra[3*4+1]=block[21];
		clra[3*4+2]=block[22];
		clra[3*4+3]=255;
	}

	clrd[0]=block[11];
	clrd[1]=block[15];
	clrd[2]=block[19];
	clrd[3]=block[23];
	
	for(i=0; i<4; i++)
	{
#if 0
		clrb[i*16+0*4+0]=clra[i*4+0]+(clrd[i]>>1);
		clrb[i*16+0*4+1]=clra[i*4+1]+(clrd[i]>>1);
		clrb[i*16+0*4+2]=clra[i*4+2]+(clrd[i]>>1);
		clrb[i*16+1*4+0]=clra[i*4+0]-(clrd[i]>>1);
		clrb[i*16+1*4+1]=clra[i*4+1]-(clrd[i]>>1);
		clrb[i*16+1*4+2]=clra[i*4+2]-(clrd[i]>>1);
		if(clrb[i*16+0*4+0]>255)clrb[i*16+0*4+0]=255;
		if(clrb[i*16+0*4+1]>255)clrb[i*16+0*4+1]=255;
		if(clrb[i*16+0*4+2]>255)clrb[i*16+0*4+2]=255;
		if(clrb[i*16+1*4+0]<0)clrb[i*16+1*4+0]=0;
		if(clrb[i*16+1*4+1]<0)clrb[i*16+1*4+1]=0;
		if(clrb[i*16+1*4+2]<0)clrb[i*16+1*4+2]=0;
#endif
#if 0
		clrb[i*16+0*4+0]=clra[i*4+0]-(clrd[i]>>1);
		clrb[i*16+0*4+1]=clra[i*4+1]-(clrd[i]>>1);
		clrb[i*16+0*4+2]=clra[i*4+2]-(clrd[i]>>1);
		clrb[i*16+1*4+0]=clra[i*4+0]+(clrd[i]>>1);
		clrb[i*16+1*4+1]=clra[i*4+1]+(clrd[i]>>1);
		clrb[i*16+1*4+2]=clra[i*4+2]+(clrd[i]>>1);
		if(clrb[i*16+0*4+0]<  0)clrb[i*16+0*4+0]=0;
		if(clrb[i*16+0*4+1]<  0)clrb[i*16+0*4+1]=0;
		if(clrb[i*16+0*4+2]<  0)clrb[i*16+0*4+2]=0;
		if(clrb[i*16+1*4+0]>255)clrb[i*16+1*4+0]=255;
		if(clrb[i*16+1*4+1]>255)clrb[i*16+1*4+1]=255;
		if(clrb[i*16+1*4+2]>255)clrb[i*16+1*4+2]=255;
#endif
#if 1
		mcr=clra[i*4+0]-(clrd[i]>>1);
		mcg=clra[i*4+1]-(clrd[i]>>1);
		mcb=clra[i*4+2]-(clrd[i]>>1);
		ncr=clra[i*4+0]+(clrd[i]>>1);
		ncg=clra[i*4+1]+(clrd[i]>>1);
		ncb=clra[i*4+2]+(clrd[i]>>1);

//		mcr=clra[i*4+0]-clrd[i];
//		mcg=clra[i*4+1]-clrd[i];
//		mcb=clra[i*4+2]-clrd[i];
//		ncr=clra[i*4+0]+clrd[i];
//		ncg=clra[i*4+1]+clrd[i];
//		ncb=clra[i*4+2]+clrd[i];

//		cr=clra[i*4+0];		cg=clra[i*4+1];
//		cb=clra[i*4+2];		j=cr+cg+cb+1;
//		mcr=cr-clrd[i]*cr/j;
//		mcg=cg-clrd[i]*cg/j;
//		mcb=cb-clrd[i]*cb/j;
//		ncr=cr+clrd[i]*cr/j;
//		ncg=cg+clrd[i]*cg/j;
//		ncb=cb+clrd[i]*cb/j;

//		mcr=clra[i*4+0]-(clrd[i]>>1);
//		mcg=clra[i*4+1]-clrd[i];
//		mcb=clra[i*4+2]-(clrd[i]>>1);
//		ncr=clra[i*4+0]+(clrd[i]>>1);
//		ncg=clra[i*4+1]+clrd[i];
//		ncb=clra[i*4+2]+(clrd[i]>>1);

		if(mcr<  0)mcr=0;
		if(mcg<  0)mcg=0;
		if(mcb<  0)mcb=0;
		if(ncr>255)ncr=255;
		if(ncg>255)ncg=255;
		if(ncb>255)ncb=255;
		clrb[i*16+0*4+0]=mcr;
		clrb[i*16+0*4+1]=mcg;
		clrb[i*16+0*4+2]=mcb;
		clrb[i*16+1*4+0]=ncr;
		clrb[i*16+1*4+1]=ncg;
		clrb[i*16+1*4+2]=ncb;
#endif
		clrb[i*16+2*4+0]=(11*clrb[i*16+0*4+0]+ 5*clrb[i*16+1*4+0])>>4;
		clrb[i*16+2*4+1]=(11*clrb[i*16+0*4+1]+ 5*clrb[i*16+1*4+1])>>4;
		clrb[i*16+2*4+2]=(11*clrb[i*16+0*4+2]+ 5*clrb[i*16+1*4+2])>>4;
		clrb[i*16+3*4+0]=( 5*clrb[i*16+0*4+0]+11*clrb[i*16+1*4+0])>>4;
		clrb[i*16+3*4+1]=( 5*clrb[i*16+0*4+1]+11*clrb[i*16+1*4+1])>>4;
		clrb[i*16+3*4+2]=( 5*clrb[i*16+0*4+2]+11*clrb[i*16+1*4+2])>>4;
	}
	
	i=block[4];
//	i=block[7];
	p0=i&3;			p1=(i>>2)&3;
	p2=(i>>4)&3;	p3=(i>>6)&3;
	rgba[0*ystride+0*xstride+0]=clrb[0*16+p0*4+0];
	rgba[0*ystride+0*xstride+1]=clrb[0*16+p0*4+1];
	rgba[0*ystride+0*xstride+2]=clrb[0*16+p0*4+2];
	rgba[0*ystride+0*xstride+3]=255;
	rgba[0*ystride+1*xstride+0]=clrb[0*16+p1*4+0];
	rgba[0*ystride+1*xstride+1]=clrb[0*16+p1*4+1];
	rgba[0*ystride+1*xstride+2]=clrb[0*16+p1*4+2];
	rgba[0*ystride+1*xstride+3]=255;
	rgba[0*ystride+2*xstride+0]=clrb[1*16+p2*4+0];
	rgba[0*ystride+2*xstride+1]=clrb[1*16+p2*4+1];
	rgba[0*ystride+2*xstride+2]=clrb[1*16+p2*4+2];
	rgba[0*ystride+2*xstride+3]=255;
	rgba[0*ystride+3*xstride+0]=clrb[1*16+p3*4+0];
	rgba[0*ystride+3*xstride+1]=clrb[1*16+p3*4+1];
	rgba[0*ystride+3*xstride+2]=clrb[1*16+p3*4+2];
	rgba[0*ystride+3*xstride+3]=255;

	i=block[5];
//	i=block[6];
	p0=i&3;			p1=(i>>2)&3;
	p2=(i>>4)&3;	p3=(i>>6)&3;
	rgba[1*ystride+0*xstride+0]=clrb[0*16+p0*4+0];
	rgba[1*ystride+0*xstride+1]=clrb[0*16+p0*4+1];
	rgba[1*ystride+0*xstride+2]=clrb[0*16+p0*4+2];
	rgba[1*ystride+0*xstride+3]=255;
	rgba[1*ystride+1*xstride+0]=clrb[0*16+p1*4+0];
	rgba[1*ystride+1*xstride+1]=clrb[0*16+p1*4+1];
	rgba[1*ystride+1*xstride+2]=clrb[0*16+p1*4+2];
	rgba[1*ystride+1*xstride+3]=255;
	rgba[1*ystride+2*xstride+0]=clrb[1*16+p2*4+0];
	rgba[1*ystride+2*xstride+1]=clrb[1*16+p2*4+1];
	rgba[1*ystride+2*xstride+2]=clrb[1*16+p2*4+2];
	rgba[1*ystride+2*xstride+3]=255;
	rgba[1*ystride+3*xstride+0]=clrb[1*16+p3*4+0];
	rgba[1*ystride+3*xstride+1]=clrb[1*16+p3*4+1];
	rgba[1*ystride+3*xstride+2]=clrb[1*16+p3*4+2];
	rgba[1*ystride+3*xstride+3]=255;

	i=block[6];
//	i=block[5];
	p0=i&3;			p1=(i>>2)&3;
	p2=(i>>4)&3;	p3=(i>>6)&3;
	rgba[2*ystride+0*xstride+0]=clrb[2*16+p0*4+0];
	rgba[2*ystride+0*xstride+1]=clrb[2*16+p0*4+1];
	rgba[2*ystride+0*xstride+2]=clrb[2*16+p0*4+2];
	rgba[2*ystride+0*xstride+3]=255;
	rgba[2*ystride+1*xstride+0]=clrb[2*16+p1*4+0];
	rgba[2*ystride+1*xstride+1]=clrb[2*16+p1*4+1];
	rgba[2*ystride+1*xstride+2]=clrb[2*16+p1*4+2];
	rgba[2*ystride+1*xstride+3]=255;
	rgba[2*ystride+2*xstride+0]=clrb[3*16+p2*4+0];
	rgba[2*ystride+2*xstride+1]=clrb[3*16+p2*4+1];
	rgba[2*ystride+2*xstride+2]=clrb[3*16+p2*4+2];
	rgba[2*ystride+2*xstride+3]=255;
	rgba[2*ystride+3*xstride+0]=clrb[3*16+p3*4+0];
	rgba[2*ystride+3*xstride+1]=clrb[3*16+p3*4+1];
	rgba[2*ystride+3*xstride+2]=clrb[3*16+p3*4+2];
	rgba[2*ystride+3*xstride+3]=255;

	i=block[7];
//	i=block[4];
	p0=i&3;			p1=(i>>2)&3;
	p2=(i>>4)&3;	p3=(i>>6)&3;
	rgba[3*ystride+0*xstride+0]=clrb[2*16+p0*4+0];
	rgba[3*ystride+0*xstride+1]=clrb[2*16+p0*4+1];
	rgba[3*ystride+0*xstride+2]=clrb[2*16+p0*4+2];
	rgba[3*ystride+0*xstride+3]=255;
	rgba[3*ystride+1*xstride+0]=clrb[2*16+p1*4+0];
	rgba[3*ystride+1*xstride+1]=clrb[2*16+p1*4+1];
	rgba[3*ystride+1*xstride+2]=clrb[2*16+p1*4+2];
	rgba[3*ystride+1*xstride+3]=255;
	rgba[3*ystride+2*xstride+0]=clrb[3*16+p2*4+0];
	rgba[3*ystride+2*xstride+1]=clrb[3*16+p2*4+1];
	rgba[3*ystride+2*xstride+2]=clrb[3*16+p2*4+2];
	rgba[3*ystride+2*xstride+3]=255;
	rgba[3*ystride+3*xstride+0]=clrb[3*16+p3*4+0];
	rgba[3*ystride+3*xstride+1]=clrb[3*16+p3*4+1];
	rgba[3*ystride+3*xstride+2]=clrb[3*16+p3*4+2];
	rgba[3*ystride+3*xstride+3]=255;
}

void BGBBTJ_BCn_DecodeBlockMB1C(
	byte *block, byte *rgba, int xstride, int ystride, int flag)
{
	byte clr[4*4];
	int dr, dg, db;
	int i, j, k, l;
	
	if(block[0]==0x55)
	{
		if(block[2]==0xAB)
		{
			BGBBTJ_BCn_DecodeBlockMB1C_4ClrRGB24(
				block, rgba, xstride, ystride, flag);
			return;
		}
	}
	
	i=block[0]|(block[1]<<8);
	j=block[2]|(block[3]<<8);

#if 1
	if(i==j)
	{
		k=(block[4]^block[5])|(block[5]^block[6])|(block[6]^block[7]);
		if(!k)
		{
			clr[0]=block[1]&0xF8;
			clr[1]=((block[1]<<5)|(block[0]>>3))&0xFC;
			clr[2]=(block[0]<<3)&0xF8;
			clr[3]=(block[4]==0xFF)?0:255;
			clr[0]|=(clr[0]>>5);
			clr[1]|=(clr[1]>>6);
			clr[2]|=(clr[2]>>5);

#if 1
			if(block[8]|block[9]|block[10]|block[11]|
				block[12]|block[13]|block[14]|block[15])
			{
				clr[0]=block[ 8];
				clr[1]=block[ 9];
				clr[2]=block[10];
			}
#endif

			for(i=0; i<4; i++)
				for(j=0; j<4; j++)
			{
				k=i*ystride+j*xstride;
				rgba[k+0]=clr[0];
				rgba[k+1]=clr[1];
				rgba[k+2]=clr[2];
				rgba[k+3]=clr[3];
			}
			return;
		}
	}
#endif

	if(block[8]|block[9]|block[10]|block[11]|
		block[12]|block[13]|block[14]|block[15])
	{
		clr[0]=block[ 8];		clr[1]=block[ 9];
		clr[2]=block[10];		clr[4]=block[12];
		clr[5]=block[13];		clr[6]=block[14];
	}else
	{
		clr[ 0]=block[1]&0xF8;
		clr[ 1]=((block[1]<<5)|(block[0]>>3))&0xFC;
		clr[ 2]=(block[0]<<3)&0xF8;
		clr[ 3]=255;
		clr[ 4]=block[3]&0xF8;
		clr[ 5]=((block[3]<<5)|(block[2]>>3))&0xFC;
		clr[ 6]=(block[2]<<3)&0xF8;	
		clr[ 7]=255;

		clr[ 0]|=(clr[ 0]>>5);		clr[ 1]|=(clr[ 1]>>6);
		clr[ 2]|=(clr[ 2]>>5);		clr[ 4]|=(clr[ 4]>>5);
		clr[ 5]|=(clr[ 5]>>6);		clr[ 6]|=(clr[ 6]>>5);
	}

	if(i<=j)
	{
		clr[ 8]=(clr[0]+clr[4])>>1;
		clr[ 9]=(clr[1]+clr[5])>>1;
		clr[10]=(clr[2]+clr[6])>>1;
		clr[11]=255;
		clr[12]=(clr[0]+clr[4])>>1;
		clr[13]=(clr[1]+clr[5])>>1;
		clr[14]=(clr[2]+clr[6])>>1;
		clr[15]=0;
	}else
	{
		clr[ 8]=(clr[0]*11+clr[4]*5)>>4;
		clr[ 9]=(clr[1]*11+clr[5]*5)>>4;
		clr[10]=(clr[2]*11+clr[6]*5)>>4;
		clr[11]=255;
		clr[12]=(clr[0]*5+clr[4]*11)>>4;
		clr[13]=(clr[1]*5+clr[5]*11)>>4;
		clr[14]=(clr[2]*5+clr[6]*11)>>4;
		clr[15]=255;
	}
	
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		k=i*ystride+j*xstride;
		l=((block[4+i]>>(2*j))&3)*4;
		rgba[k+0]=clr[l+0];
		rgba[k+1]=clr[l+1];
		rgba[k+2]=clr[l+2];
		rgba[k+3]=clr[l+3];
	}
}

void BGBBTJ_BCn_DecodeBlockMB1C_Pal24(
	byte *block,
	byte *rgba, int xstride, int ystride, u32 *pal)
{
	byte clr[4*4];
	int i, j, k, l;
	
	i=block[0]|(block[1]<<8);
	j=block[2]|(block[3]<<8);

#if 1
	if(i==j)
	{
		k=(block[4]^block[5])|(block[5]^block[6])|(block[6]^block[7]);
		if(!k)
		{
			l=(i>>8)&255;
			clr[0]=(pal[l]>>16)&255;
			clr[1]=(pal[l]>>8)&255;
			clr[2]=(pal[l]>>0)&255;
			clr[3]=(block[4]==0xFF)?0:255;
			
			for(i=0; i<4; i++)
				for(j=0; j<4; j++)
			{
				k=i*ystride+j*xstride;
				rgba[k+0]=clr[0];
				rgba[k+1]=clr[1];
				rgba[k+2]=clr[2];
				rgba[k+3]=clr[3];
			}
			return;
		}
	}
#endif

	if(i<=j)
	{
		clr[ 0]=(pal[(i>>8)&255]>>16)&255;
		clr[ 1]=(pal[(i>>8)&255]>>8)&255;
		clr[ 2]=(pal[(i>>8)&255]>>0)&255;
		clr[ 3]=255;
		clr[ 4]=(pal[(j>>8)&255]>>16)&255;
		clr[ 5]=(pal[(j>>8)&255]>>8)&255;
		clr[ 6]=(pal[(j>>8)&255]>>0)&255;
		clr[ 7]=255;
		clr[ 8]=(clr[0]+clr[4])>>1;
		clr[ 9]=(clr[1]+clr[5])>>1;
		clr[10]=(clr[2]+clr[6])>>1;
		clr[11]=255;
		clr[12]=(clr[0]+clr[4])>>1;
		clr[13]=(clr[1]+clr[5])>>1;
		clr[14]=(clr[2]+clr[6])>>1;
		clr[15]=0;
	}else
	{
		clr[ 0]=(pal[(i>>8)&255]>>16)&255;
		clr[ 1]=(pal[(i>>8)&255]>>8)&255;
		clr[ 2]=(pal[(i>>8)&255]>>0)&255;
		clr[ 3]=255;
		clr[ 4]=(pal[(j>>8)&255]>>16)&255;
		clr[ 5]=(pal[(j>>8)&255]>>8)&255;
		clr[ 6]=(pal[(j>>8)&255]>>0)&255;
		clr[ 7]=255;
		clr[ 8]=(clr[0]*11+clr[4]*5)>>4;
		clr[ 9]=(clr[1]*11+clr[5]*5)>>4;
		clr[10]=(clr[2]*11+clr[6]*5)>>4;
		clr[11]=255;
		clr[12]=(clr[0]*5+clr[4]*11)>>4;
		clr[13]=(clr[1]*5+clr[5]*11)>>4;
		clr[14]=(clr[2]*5+clr[6]*11)>>4;
		clr[15]=255;
	}
	
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		k=i*ystride+j*xstride;
		l=((block[4+i]>>(2*j))&3)*4;
		rgba[k+0]=clr[l+0];
		rgba[k+1]=clr[l+1];
		rgba[k+2]=clr[l+2];
		rgba[k+3]=clr[l+3];
	}
}

#if 0
void BGBBTJ_BCn_DecodeBlockMB1C_BGRA(
	byte *block, byte *rgba, int xstride, int ystride, int flag)
{
	byte clr[4*4];
	int dr, dg, db;
	int p0, p1, p2, p3;
	int i, j, k, l;
	
	if(block[0]==0x55)
	{
		if(block[2]==0xAB)
		{
			BGBBTJ_BCn_DecodeBlockMB1C_4ClrRGB24(
				block, rgba, xstride, ystride, flag|1);
			return;
		}
	}
	
#if defined(X86) || defined(X86_64)
//	i=block[0]|(block[1]<<8);
//	j=block[2]|(block[3]<<8);
//	i=*(int *)(block+ 8);
//	j=*(int *)(block+12);

	i=*(u16 *)(block+0);
	j=*(u16 *)(block+2);
#else
	i=block[0]|(block[1]<<8);
	j=block[2]|(block[3]<<8);
#endif

#if 1
	if(i==j)
	{
#if defined(X86) || defined(X86_64)
		k=*(int *)(block+4);
#else
		k=(block[4]^block[5])|(block[5]^block[6])|(block[6]^block[7]);
#endif
		if(!k)
		{
			clr[2]=block[1]&0xF8;
			clr[1]=((block[1]<<5)|(block[0]>>3))&0xFC;
			clr[0]=(block[0]<<3)&0xF8;
			clr[3]=(block[4]==0xFF)?0:255;
			clr[2]|=(clr[0]>>5);
			clr[1]|=(clr[1]>>6);
			clr[0]|=(clr[2]>>5);

#if 1
			if(block[8]|block[9]|block[10]|block[11]|
				block[12]|block[13]|block[14]|block[15])
			{
				clr[2]=block[ 8];
				clr[1]=block[ 9];
				clr[0]=block[10];
			}
#endif

#if defined(X86) || defined(X86_64)
			if(xstride==4)
			{
				j=*(int *)clr;
				for(i=0; i<4; i++)
				{
					k=i*ystride;
					((int *)(rgba+k))[0]=j;
					((int *)(rgba+k))[1]=j;
					((int *)(rgba+k))[2]=j;
					((int *)(rgba+k))[3]=j;
				}
				return;
			}
#endif

			for(i=0; i<4; i++)
				for(j=0; j<4; j++)
			{
				k=i*ystride+j*xstride;
				rgba[k+0]=clr[0];
				rgba[k+1]=clr[1];
				rgba[k+2]=clr[2];
				rgba[k+3]=clr[3];
			}
			return;
		}
	}
#endif

#if defined(X86) || defined(X86_64)
	if(*(s64 *)(block+8))
#else
	if(block[8]|block[9]|block[10]|block[11]|
		block[12]|block[13]|block[14]|block[15])
#endif
	{
		clr[2]=block[ 8];		clr[1]=block[ 9];
		clr[0]=block[10];		clr[6]=block[12];
		clr[5]=block[13];		clr[4]=block[14];
	}else
	{
		clr[ 2]=block[1]&0xF8;
		clr[ 1]=((block[1]<<5)|(block[0]>>3))&0xFC;
		clr[ 0]=(block[0]<<3)&0xF8;
		clr[ 3]=255;
		clr[ 6]=block[3]&0xF8;
		clr[ 5]=((block[3]<<5)|(block[2]>>3))&0xFC;
		clr[ 4]=(block[2]<<3)&0xF8;	
		clr[ 7]=255;

		clr[ 0]|=(clr[ 0]>>5);		clr[ 1]|=(clr[ 1]>>6);
		clr[ 2]|=(clr[ 2]>>5);		clr[ 4]|=(clr[ 4]>>5);
		clr[ 5]|=(clr[ 5]>>6);		clr[ 6]|=(clr[ 6]>>5);
	}

//	if(i<=j)
	if(0)
	{
		clr[ 8]=(clr[0]+clr[4])>>1;
		clr[ 9]=(clr[1]+clr[5])>>1;
		clr[10]=(clr[2]+clr[6])>>1;
		clr[11]=255;
		clr[12]=(clr[0]+clr[4])>>1;
		clr[13]=(clr[1]+clr[5])>>1;
		clr[14]=(clr[2]+clr[6])>>1;
		clr[15]=0;
	}else
	{
		clr[ 8]=(clr[0]*11+clr[4]*5)>>4;
		clr[ 9]=(clr[1]*11+clr[5]*5)>>4;
		clr[10]=(clr[2]*11+clr[6]*5)>>4;
		clr[11]=255;
		clr[12]=(clr[0]*5+clr[4]*11)>>4;
		clr[13]=(clr[1]*5+clr[5]*11)>>4;
		clr[14]=(clr[2]*5+clr[6]*11)>>4;
		clr[15]=255;
	}

#if defined(X86) || defined(X86_64)
	if(xstride==4)
	{
		for(i=0; i<4; i++)
		{
			j=block[4+i];
			k=i*ystride;
			p0=(j>>(2*0))&3;	p1=(j>>(2*1))&3;
			p2=(j>>(2*2))&3;	p3=(j>>(2*3))&3;
			((int *)(rgba+k))[0]=((int *)clr)[p0];
			((int *)(rgba+k))[1]=((int *)clr)[p1];
			((int *)(rgba+k))[2]=((int *)clr)[p2];
			((int *)(rgba+k))[3]=((int *)clr)[p3];
		}
		return;
	}
#endif

	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		k=i*ystride+j*xstride;
		l=((block[4+i]>>(2*j))&3)*4;
		rgba[k+0]=clr[l+0];
		rgba[k+1]=clr[l+1];
		rgba[k+2]=clr[l+2];
		rgba[k+3]=clr[l+3];
	}
}
#endif

#if 1
void BGBBTJ_BCn_DecodeBlockMB1C_BGRA(
	byte *block, byte *rgba, int xstride, int ystride, int flag)
{
	byte clr[4*4];
	int *trgb;
	int dr, dg, db;
	int p0, p1, p2, p3;
	int i, j, k, l;
	
	if(block[0]==0x55)
	{
		if(block[2]==0xAB)
		{
			BGBBTJ_BCn_DecodeBlockMB1C_4ClrRGB24(
				block, rgba, xstride, ystride, flag|1);
			return;
		}
	}
	
#if defined(X86) || defined(X86_64) || defined(ARM)
//	i=block[0]|(block[1]<<8);
//	j=block[2]|(block[3]<<8);
//	i=*(int *)(block+ 8);
//	j=*(int *)(block+12);

	i=*(u16 *)(block+0);
	j=*(u16 *)(block+2);
#else
	i=block[0]|(block[1]<<8);
	j=block[2]|(block[3]<<8);
#endif

#if 1
	if(i==j)
	{
#if defined(X86) || defined(X86_64) || defined(ARM)
		k=*(int *)(block+4);
		if(!k || (k==-1))
#else
		k=(block[4]^block[5])|(block[5]^block[6])|(block[6]^block[7]);
		if(!k)
#endif
		{
			clr[2]=block[1]&0xF8;
			clr[1]=((block[1]<<5)|(block[0]>>3))&0xFC;
			clr[0]=(block[0]<<3)&0xF8;
			clr[3]=(block[4]==0xFF)?0:255;
			clr[2]|=(clr[0]>>5);
			clr[1]|=(clr[1]>>6);
			clr[0]|=(clr[2]>>5);

#if 1
#if defined(X86) || defined(X86_64) || defined(ARM)
			if(*(u64 *)(block+8))
#else
			if(block[8]|block[9]|block[10]|block[11]|
				block[12]|block[13]|block[14]|block[15])
#endif
			{
				clr[2]=block[ 8];
				clr[1]=block[ 9];
				clr[0]=block[10];
			}
#endif

#if defined(X86) || defined(X86_64) || defined(ARM)
			if(xstride==4)
			{
				j=*(int *)clr;
#if 1
				trgb=(int *)(rgba+0*ystride);
				trgb[0]=j;	trgb[1]=j;
				trgb[2]=j;	trgb[3]=j;
				trgb=(int *)(rgba+1*ystride);
				trgb[0]=j;	trgb[1]=j;
				trgb[2]=j;	trgb[3]=j;
				trgb=(int *)(rgba+2*ystride);
				trgb[0]=j;	trgb[1]=j;
				trgb[2]=j;	trgb[3]=j;
				trgb=(int *)(rgba+3*ystride);
				trgb[0]=j;	trgb[1]=j;
				trgb[2]=j;	trgb[3]=j;
#endif
#if 0
				for(i=0; i<4; i++)
				{
					k=i*ystride;
					((int *)(rgba+k))[0]=j;
					((int *)(rgba+k))[1]=j;
					((int *)(rgba+k))[2]=j;
					((int *)(rgba+k))[3]=j;
				}
#endif
				return;
			}
#endif

			for(i=0; i<4; i++)
				for(j=0; j<4; j++)
			{
				k=i*ystride+j*xstride;
				rgba[k+0]=clr[0];
				rgba[k+1]=clr[1];
				rgba[k+2]=clr[2];
				rgba[k+3]=clr[3];
			}
			return;
		}
	}
#endif

// #if defined(X86) || defined(X86_64)
#if defined(X86) || defined(X86_64) || defined(ARM)
	if(*(s64 *)(block+8))
#else
	if(block[8]|block[9]|block[10]|block[11]|
		block[12]|block[13]|block[14]|block[15])
#endif
	{
		clr[2]=block[ 8];		clr[1]=block[ 9];
		clr[0]=block[10];		clr[6]=block[12];
		clr[5]=block[13];		clr[4]=block[14];
		clr[ 3]=255;			clr[ 7]=255;
	}else
	{
		clr[ 2]=block[1]&0xF8;
		clr[ 1]=((block[1]<<5)|(block[0]>>3))&0xFC;
		clr[ 0]=(block[0]<<3)&0xF8;
		clr[ 3]=255;
		clr[ 6]=block[3]&0xF8;
		clr[ 5]=((block[3]<<5)|(block[2]>>3))&0xFC;
		clr[ 4]=(block[2]<<3)&0xF8;	
		clr[ 7]=255;

		clr[ 0]|=(clr[ 0]>>5);		clr[ 1]|=(clr[ 1]>>6);
		clr[ 2]|=(clr[ 2]>>5);		clr[ 4]|=(clr[ 4]>>5);
		clr[ 5]|=(clr[ 5]>>6);		clr[ 6]|=(clr[ 6]>>5);
	}

//	if(i<=j)
	if(0)
	{
		clr[ 8]=(clr[0]+clr[4])>>1;
		clr[ 9]=(clr[1]+clr[5])>>1;
		clr[10]=(clr[2]+clr[6])>>1;
		clr[11]=255;
		clr[12]=(clr[0]+clr[4])>>1;
		clr[13]=(clr[1]+clr[5])>>1;
		clr[14]=(clr[2]+clr[6])>>1;
		clr[15]=0;
	}else
	{
		clr[ 8]=(clr[0]*11+clr[4]*5)>>4;
		clr[ 9]=(clr[1]*11+clr[5]*5)>>4;
		clr[10]=(clr[2]*11+clr[6]*5)>>4;
		clr[11]=255;
		clr[12]=(clr[0]*5+clr[4]*11)>>4;
		clr[13]=(clr[1]*5+clr[5]*11)>>4;
		clr[14]=(clr[2]*5+clr[6]*11)>>4;
		clr[15]=255;
	}

// #if defined(X86) || defined(X86_64)
#if defined(X86) || defined(X86_64) || defined(ARM)
	if(xstride==4)
	{
#if 0
		for(i=0; i<4; i++)
		{
			j=block[4+i];
			k=i*ystride;
			p0=(j>>(2*0))&3;	p1=(j>>(2*1))&3;
			p2=(j>>(2*2))&3;	p3=(j>>(2*3))&3;
			((int *)(rgba+k))[0]=((int *)clr)[p0];
			((int *)(rgba+k))[1]=((int *)clr)[p1];
			((int *)(rgba+k))[2]=((int *)clr)[p2];
			((int *)(rgba+k))[3]=((int *)clr)[p3];
		}
#endif

		j=block[4+0];
		trgb=(int *)(rgba+0*ystride);
		p0=(j>>(2*0))&3;	p1=(j>>(2*1))&3;
		p2=(j>>(2*2))&3;	p3=(j>>(2*3))&3;
		trgb[0]=((int *)clr)[p0];
		trgb[1]=((int *)clr)[p1];
		trgb[2]=((int *)clr)[p2];
		trgb[3]=((int *)clr)[p3];

		j=block[4+1];
		trgb=(int *)(rgba+1*ystride);
		p0=(j>>(2*0))&3;	p1=(j>>(2*1))&3;
		p2=(j>>(2*2))&3;	p3=(j>>(2*3))&3;
		trgb[0]=((int *)clr)[p0];
		trgb[1]=((int *)clr)[p1];
		trgb[2]=((int *)clr)[p2];
		trgb[3]=((int *)clr)[p3];

		j=block[4+2];
		trgb=(int *)(rgba+2*ystride);
		p0=(j>>(2*0))&3;	p1=(j>>(2*1))&3;
		p2=(j>>(2*2))&3;	p3=(j>>(2*3))&3;
		trgb[0]=((int *)clr)[p0];
		trgb[1]=((int *)clr)[p1];
		trgb[2]=((int *)clr)[p2];
		trgb[3]=((int *)clr)[p3];

		j=block[4+3];
		trgb=(int *)(rgba+3*ystride);
		p0=(j>>(2*0))&3;	p1=(j>>(2*1))&3;
		p2=(j>>(2*2))&3;	p3=(j>>(2*3))&3;
		trgb[0]=((int *)clr)[p0];
		trgb[1]=((int *)clr)[p1];
		trgb[2]=((int *)clr)[p2];
		trgb[3]=((int *)clr)[p3];

		return;
	}
#endif

	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		k=i*ystride+j*xstride;
		l=((block[4+i]>>(2*j))&3)*4;
		rgba[k+0]=clr[l+0];
		rgba[k+1]=clr[l+1];
		rgba[k+2]=clr[l+2];
		rgba[k+3]=clr[l+3];
	}
}
#endif

BGBBTJ_API void BGBBTJ_BCn_DecodeImageMB5C(byte *block,
	byte *rgba, int xs, int ys, int stride)
{
	int xs1, ys1;
	int i, j;
	
	xs1=xs>>2; ys1=ys>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
//		BGBBTJ_BCn_DecodeBlockBC4(
//			block+(i*xs1+j)*32,
//			rgba+(i*4*xs+j*4)*stride+3,
//			stride, xs*stride);
		BGBBTJ_BCn_DecodeBlockMB1C(
			block+(i*xs1+j)*32+8,
			rgba+(i*4*xs+j*4)*stride,
			stride, xs*stride, 0);
		BGBBTJ_BCn_DecodeBlockBC4B(
			block+(i*xs1+j)*32,
			rgba+(i*4*xs+j*4)*stride+3,
			stride, xs*stride);
	}
}

BGBBTJ_API void BGBBTJ_BCn_DecodeImageMB5CB(byte *block,
	byte *rgba, int xs, int ys, int stride)
{
	int xs1, ys1;
	int i, j;
	
	xs1=xs>>2; ys1=ys>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
//		BGBBTJ_BCn_DecodeBlockBC4(
//			block+(i*xs1+j)*32,
//			rgba+(i*4*xs+j*4)*stride+3,
//			stride, xs*stride);
		BGBBTJ_BCn_DecodeBlockMB1C(
			block+(i*xs1+j)*32+8,
			rgba+(i*4*xs+j*4)*stride,
			stride, xs*stride, 0);
	}
}

BGBBTJ_API void BGBBTJ_BCn_DecodeImageMB5C_BGRA(byte *block,
	byte *rgba, int xs, int ys, int stride)
{
	int xs1, ys1;
	int i, j;
	
	xs1=xs>>2; ys1=ys>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
//		BGBBTJ_BCn_DecodeBlockBC4(
//			block+(i*xs1+j)*32,
//			rgba+(i*4*xs+j*4)*stride+3,
//			stride, xs*stride);
		BGBBTJ_BCn_DecodeBlockMB1C_BGRA(
			block+(i*xs1+j)*32+8,
			rgba+(i*4*xs+j*4)*stride,
			stride, xs*stride, 0);
		BGBBTJ_BCn_DecodeBlockBC4B(
			block+(i*xs1+j)*32,
			rgba+(i*4*xs+j*4)*stride+3,
			stride, xs*stride);
	}
}

BGBBTJ_API void BGBBTJ_BCn_DecodeImageMB5CB_BGRA(byte *block,
	byte *rgba, int xs, int ys, int stride)
{
	int xs1, ys1;
	int i, j;
	
	xs1=xs>>2; ys1=ys>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
//		BGBBTJ_BCn_DecodeBlockBC4(
//			block+(i*xs1+j)*32,
//			rgba+(i*4*xs+j*4)*stride+3,
//			stride, xs*stride);
		BGBBTJ_BCn_DecodeBlockMB1C_BGRA(
			block+(i*xs1+j)*32+8,
			rgba+(i*4*xs+j*4)*stride,
			stride, xs*stride, 0);
	}
}


BGBBTJ_API void BGBBTJ_BCn_FlipBlockMB5C(
	byte *iblock, byte *oblock)
{
	byte *cs, *ct;
	int i, j, k, l;

	cs=iblock;
	ct=oblock;

	k=cs[2]|(cs[3]<<8)|(cs[4]<<16);
	l=cs[5]|(cs[6]<<8)|(cs[7]<<16);
	k=(k>>12)|((k&4095)<<12);
	l=(l>>12)|((l&4095)<<12);
		
	ct[ 0]=cs[ 0];			ct[ 1]=cs[ 1];
	ct[ 2]=l&255;			ct[ 3]=(l>>8)&255;
	ct[ 4]=(l>>16)&255;		ct[ 5]=k&255;
	ct[ 6]=(k>>8)&255;		ct[ 7]=(k>>16)&255;

	bgbbtj_bt1c_memcpy4(ct+8, cs+8);
	ct[12]=cs[15];			ct[13]=cs[14];
	ct[14]=cs[13];			ct[15]=cs[12];

	if((cs[8]==0x55) && (cs[10]==0xAB))
	{
		bgbbtj_bt1c_memcpy8(ct+16, cs+24);
		bgbbtj_bt1c_memcpy8(ct+24, cs+16);
	}else
	{
		bgbbtj_bt1c_memcpy16(ct+16, cs+16);
	}
}

BGBBTJ_API void BGBBTJ_BCn_DecodeImageMB5C_FlipBGRA(byte *block,
	byte *rgba, int xs, int ys, int stride)
{
	byte tblk[32];
	int xs1, ys1;
	int i, j;
	
	xs1=xs>>2; ys1=ys>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
//		BGBBTJ_BCn_FlipBlockMB5C(block+(i*xs1+j)*32, tblk);
		BGBBTJ_BCn_FlipBlockMB5C(block+((ys1-i-1)*xs1+j)*32, tblk);
		BGBBTJ_BCn_DecodeBlockMB1C_BGRA(
			tblk+8,
			rgba+(i*4*xs+j*4)*stride,
			stride, xs*stride, 0);
		BGBBTJ_BCn_DecodeBlockBC4B(
			tblk,
			rgba+(i*4*xs+j*4)*stride+3,
			stride, xs*stride);
	}
}

BGBBTJ_API void BGBBTJ_BCn_DecodeImageMB5CB_FlipBGRA(byte *block,
	byte *rgba, int xs, int ys, int stride)
{
	byte tblk[32];
	int xs1, ys1;
	int i, j;
	
	xs1=xs>>2; ys1=ys>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
//		BGBBTJ_BCn_FlipBlockMB5C(block+(i*xs1+j)*32, tblk);
		BGBBTJ_BCn_FlipBlockMB5C(block+((ys1-i-1)*xs1+j)*32, tblk);
		BGBBTJ_BCn_DecodeBlockMB1C_BGRA(
			tblk+8,
			rgba+(i*4*xs+j*4)*stride,
			stride, xs*stride, 0);
	}
}
