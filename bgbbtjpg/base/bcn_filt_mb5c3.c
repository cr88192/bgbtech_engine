#if 1
void BGBBTJ_BCn_EncodeBlockMB1CF_CY_BGRA(byte *block,
	byte *rgba, int xstride, int ystride)
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
	byte pxy[16];
	int p0, p1, p2, p3, p4, p5, p6, p7;
	int l0, l1, l2, l3, l3a, l3b;
	int mcr, mcg, mcb, mca, mcy;
	int ncr, ncg, ncb, nca, ncy;
	int acr, acg, acb, aca, acy;

	int mcrc, mcgc, mcbc, mcyc;
	int ncrc, ncgc, ncbc, ncyc;
	int acrc, acgc, acbc, acyc;
	int mcry, mcgy, mcby, mcyy;
	int ncry, ncgy, ncby, ncyy;
	int acry, acgy, acby, acyy;

	int cr, cg, cb, ca, cy, cyr, cyg, cyb;
	int i, j, k, l;

	byte *rgba0, *rgba1, *rgba2, *rgba3;
	int l00, l01, l02, l03;
	int l10, l11, l12, l13;
	int l20, l21, l22, l23;
	int l30, l31, l32, l33;
	int cr00, cg00, cb00, cr01, cg01, cb01;
	int cr02, cg02, cb02, cr03, cg03, cb03;
	int cr10, cg10, cb10, cr11, cg11, cb11;
	int cr12, cg12, cb12, cr13, cg13, cb13;
	int cr20, cg20, cb20, cr21, cg21, cb21;
	int cr22, cg22, cb22, cr23, cg23, cb23;
	int cr30, cg30, cb30, cr31, cg31, cb31;
	int cr32, cg32, cb32, cr33, cg33, cb33;
	int cy00y, cy01y, cy02y, cy03y;
	int cy10y, cy11y, cy12y, cy13y;
	int cy20y, cy21y, cy22y, cy23y;
	int cy30y, cy31y, cy32y, cy33y;
	int cy00c, cy01c, cy02c, cy03c;
	int cy10c, cy11c, cy12c, cy13c;
	int cy20c, cy21c, cy22c, cy23c;
	int cy30c, cy31c, cy32c, cy33c;

#if 1
	if(xstride==4)
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
	cy00=(5*cr00+8*cg00+3*cb00)>>4;	cy01=(5*cr01+8*cg01+3*cb01)>>4;
	cy02=(5*cr02+8*cg02+3*cb02)>>4;	cy03=(5*cr03+8*cg03+3*cb03)>>4;
	cy10=(5*cr10+8*cg10+3*cb10)>>4;	cy11=(5*cr11+8*cg11+3*cb11)>>4;
	cy12=(5*cr12+8*cg12+3*cb12)>>4;	cy13=(5*cr13+8*cg13+3*cb13)>>4;
	cy20=(5*cr20+8*cg20+3*cb20)>>4;	cy21=(5*cr21+8*cg21+3*cb21)>>4;
	cy22=(5*cr22+8*cg22+3*cb22)>>4;	cy23=(5*cr23+8*cg23+3*cb23)>>4;
	cy30=(5*cr30+8*cg30+3*cb30)>>4;	cy31=(5*cr31+8*cg31+3*cb31)>>4;
	cy32=(5*cr32+8*cg32+3*cb32)>>4;	cy33=(5*cr33+8*cg33+3*cb33)>>4;
#endif

#if 1
	cy00y=(8*cr00+5*cg00+3*cb00)>>4;	cy01y=(8*cr01+5*cg01+3*cb01)>>4;
	cy02y=(8*cr02+5*cg02+3*cb02)>>4;	cy03y=(8*cr03+5*cg03+3*cb03)>>4;
	cy10y=(8*cr10+5*cg10+3*cb10)>>4;	cy11y=(8*cr11+5*cg11+3*cb11)>>4;
	cy12y=(8*cr12+5*cg12+3*cb12)>>4;	cy13y=(8*cr13+5*cg13+3*cb13)>>4;
	cy20y=(8*cr20+5*cg20+3*cb20)>>4;	cy21y=(8*cr21+5*cg21+3*cb21)>>4;
	cy22y=(8*cr22+5*cg22+3*cb22)>>4;	cy23y=(8*cr23+5*cg23+3*cb23)>>4;
	cy30y=(8*cr30+5*cg30+3*cb30)>>4;	cy31y=(8*cr31+5*cg31+3*cb31)>>4;
	cy32y=(8*cr32+5*cg32+3*cb32)>>4;	cy33y=(8*cr33+5*cg33+3*cb33)>>4;
#endif

#if 1
	cy00c=(3*cr00+5*cg00+8*cb00)>>4;	cy01c=(3*cr01+5*cg01+8*cb01)>>4;
	cy02c=(3*cr02+5*cg02+8*cb02)>>4;	cy03c=(3*cr03+5*cg03+8*cb03)>>4;
	cy10c=(3*cr10+5*cg10+8*cb10)>>4;	cy11c=(3*cr11+5*cg11+8*cb11)>>4;
	cy12c=(3*cr12+5*cg12+8*cb12)>>4;	cy13c=(3*cr13+5*cg13+8*cb13)>>4;
	cy20c=(3*cr20+5*cg20+8*cb20)>>4;	cy21c=(3*cr21+5*cg21+8*cb21)>>4;
	cy22c=(3*cr22+5*cg22+8*cb22)>>4;	cy23c=(3*cr23+5*cg23+8*cb23)>>4;
	cy30c=(3*cr30+5*cg30+8*cb30)>>4;	cy31c=(3*cr31+5*cg31+8*cb31)>>4;
	cy32c=(3*cr32+5*cg32+8*cb32)>>4;	cy33c=(3*cr33+5*cg33+8*cb33)>>4;
#endif

#if 0
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

#if 1
	if(1)			{ mcry=cr00; mcgy=cg00; mcby=cb00; mcyy=cy00y; }
	if(cy33y<mcyy)	{ mcry=cr33; mcgy=cg33; mcby=cb33; mcyy=cy33y; }
	if(cy03y<mcyy)	{ mcry=cr03; mcgy=cg03; mcby=cb03; mcyy=cy03y; }
	if(cy30y<mcyy)	{ mcry=cr30; mcgy=cg30; mcby=cb30; mcyy=cy30y; }
	if(cy11y<mcyy)	{ mcry=cr11; mcgy=cg11; mcby=cb11; mcyy=cy11y; }
	if(cy22y<mcyy)	{ mcry=cr22; mcgy=cg22; mcby=cb22; mcyy=cy22y; }
	if(cy01y<mcyy)	{ mcry=cr01; mcgy=cg01; mcby=cb01; mcyy=cy01y; }
	if(cy32y<mcyy)	{ mcry=cr32; mcgy=cg32; mcby=cb32; mcyy=cy32y; }
	if(cy13y<mcyy)	{ mcry=cr13; mcgy=cg13; mcby=cb13; mcyy=cy13y; }
	if(cy20y<mcyy)	{ mcry=cr20; mcgy=cg20; mcby=cb20; mcyy=cy20y; }
	if(cy12y<mcyy)	{ mcry=cr12; mcgy=cg12; mcby=cb12; mcyy=cy12y; }
	if(cy21y<mcyy)	{ mcry=cr21; mcgy=cg21; mcby=cb21; mcyy=cy21y; }
	if(cy02y<mcyy)	{ mcry=cr02; mcgy=cg02; mcby=cb02; mcyy=cy02y; }
	if(cy10y<mcyy)	{ mcry=cr10; mcgy=cg10; mcby=cb10; mcyy=cy10y; }
	if(cy31y<mcyy)	{ mcry=cr31; mcgy=cg31; mcby=cb31; mcyy=cy31y; }
	if(cy23y<mcyy)	{ mcry=cr23; mcgy=cg23; mcby=cb23; mcyy=cy23y; }

	if(1)			{ ncry=cr00; ncgy=cg00; ncby=cb00; ncyy=cy00y; }
	if(cy33y>ncyy)	{ ncry=cr33; ncgy=cg33; ncby=cb33; ncyy=cy33y; }
	if(cy03y>ncyy)	{ ncry=cr03; ncgy=cg03; ncby=cb03; ncyy=cy03y; }
	if(cy30y>ncyy)	{ ncry=cr30; ncgy=cg30; ncby=cb30; ncyy=cy30y; }
	if(cy11y>ncyy)	{ ncry=cr11; ncgy=cg11; ncby=cb11; ncyy=cy11y; }
	if(cy22y>ncyy)	{ ncry=cr22; ncgy=cg22; ncby=cb22; ncyy=cy22y; }
	if(cy01y>ncyy)	{ ncry=cr01; ncgy=cg01; ncby=cb01; ncyy=cy01y; }
	if(cy32y>ncyy)	{ ncry=cr32; ncgy=cg32; ncby=cb32; ncyy=cy32y; }
	if(cy13y>ncyy)	{ ncry=cr13; ncgy=cg13; ncby=cb13; ncyy=cy13y; }
	if(cy20y>ncyy)	{ ncry=cr20; ncgy=cg20; ncby=cb20; ncyy=cy20y; }
	if(cy12y>ncyy)	{ ncry=cr12; ncgy=cg12; ncby=cb12; ncyy=cy12y; }
	if(cy21y>ncyy)	{ ncry=cr21; ncgy=cg21; ncby=cb21; ncyy=cy21y; }
	if(cy02y>ncyy)	{ ncry=cr02; ncgy=cg02; ncby=cb02; ncyy=cy02y; }
	if(cy10y>ncyy)	{ ncry=cr10; ncgy=cg10; ncby=cb10; ncyy=cy10y; }
	if(cy31y>ncyy)	{ ncry=cr31; ncgy=cg31; ncby=cb31; ncyy=cy31y; }
	if(cy23y>ncyy)	{ ncry=cr23; ncgy=cg23; ncby=cb23; ncyy=cy23y; }
#endif

#if 1
	if(1)			{ mcrc=cr00; mcgc=cg00; mcbc=cb00; mcyc=cy00c; }
	if(cy33c<mcyc)	{ mcrc=cr33; mcgc=cg33; mcbc=cb33; mcyc=cy33c; }
	if(cy03c<mcyc)	{ mcrc=cr03; mcgc=cg03; mcbc=cb03; mcyc=cy03c; }
	if(cy30c<mcyc)	{ mcrc=cr30; mcgc=cg30; mcbc=cb30; mcyc=cy30c; }
	if(cy11c<mcyc)	{ mcrc=cr11; mcgc=cg11; mcbc=cb11; mcyc=cy11c; }
	if(cy22c<mcyc)	{ mcrc=cr22; mcgc=cg22; mcbc=cb22; mcyc=cy22c; }
	if(cy01c<mcyc)	{ mcrc=cr01; mcgc=cg01; mcbc=cb01; mcyc=cy01c; }
	if(cy32c<mcyc)	{ mcrc=cr32; mcgc=cg32; mcbc=cb32; mcyc=cy32c; }
	if(cy13c<mcyc)	{ mcrc=cr13; mcgc=cg13; mcbc=cb13; mcyc=cy13c; }
	if(cy20c<mcyc)	{ mcrc=cr20; mcgc=cg20; mcbc=cb20; mcyc=cy20c; }
	if(cy12c<mcyc)	{ mcrc=cr12; mcgc=cg12; mcbc=cb12; mcyc=cy12c; }
	if(cy21c<mcyc)	{ mcrc=cr21; mcgc=cg21; mcbc=cb21; mcyc=cy21c; }
	if(cy02c<mcyc)	{ mcrc=cr02; mcgc=cg02; mcbc=cb02; mcyc=cy02c; }
	if(cy10c<mcyc)	{ mcrc=cr10; mcgc=cg10; mcbc=cb10; mcyc=cy10c; }
	if(cy31c<mcyc)	{ mcrc=cr31; mcgc=cg31; mcbc=cb31; mcyc=cy31c; }
	if(cy23c<mcyc)	{ mcrc=cr23; mcgc=cg23; mcbc=cb23; mcyc=cy23c; }

	if(1)			{ ncrc=cr00; ncgc=cg00; ncbc=cb00; ncyc=cy00c; }
	if(cy33c>ncyc)	{ ncrc=cr33; ncgc=cg33; ncbc=cb33; ncyc=cy33c; }
	if(cy03c>ncyc)	{ ncrc=cr03; ncgc=cg03; ncbc=cb03; ncyc=cy03c; }
	if(cy30c>ncyc)	{ ncrc=cr30; ncgc=cg30; ncbc=cb30; ncyc=cy30c; }
	if(cy11c>ncyc)	{ ncrc=cr11; ncgc=cg11; ncbc=cb11; ncyc=cy11c; }
	if(cy22c>ncyc)	{ ncrc=cr22; ncgc=cg22; ncbc=cb22; ncyc=cy22c; }
	if(cy01c>ncyc)	{ ncrc=cr01; ncgc=cg01; ncbc=cb01; ncyc=cy01c; }
	if(cy32c>ncyc)	{ ncrc=cr32; ncgc=cg32; ncbc=cb32; ncyc=cy32c; }
	if(cy13c>ncyc)	{ ncrc=cr13; ncgc=cg13; ncbc=cb13; ncyc=cy13c; }
	if(cy20c>ncyc)	{ ncrc=cr20; ncgc=cg20; ncbc=cb20; ncyc=cy20c; }
	if(cy12c>ncyc)	{ ncrc=cr12; ncgc=cg12; ncbc=cb12; ncyc=cy12c; }
	if(cy21c>ncyc)	{ ncrc=cr21; ncgc=cg21; ncbc=cb21; ncyc=cy21c; }
	if(cy02c>ncyc)	{ ncrc=cr02; ncgc=cg02; ncbc=cb02; ncyc=cy02c; }
	if(cy10c>ncyc)	{ ncrc=cr10; ncgc=cg10; ncbc=cb10; ncyc=cy10c; }
	if(cy31c>ncyc)	{ ncrc=cr31; ncgc=cg31; ncbc=cb31; ncyc=cy31c; }
	if(cy23c>ncyc)	{ ncrc=cr23; ncgc=cg23; ncbc=cb23; ncyc=cy23c; }
#endif

	if((ncyc-mcyc)>(ncyy-mcyy))
	{
		mcr=mcrc;	mcg=mcgc;		mcb=mcbc;	mcy=mcyc;
		ncr=ncrc;	ncg=ncgc;		ncb=ncbc;	ncy=ncyc;
		pxy[ 0]=cy00c;	pxy[ 1]=cy01c;	pxy[ 2]=cy02c;	pxy[ 3]=cy03c;
		pxy[ 4]=cy10c;	pxy[ 5]=cy11c;	pxy[ 6]=cy12c;	pxy[ 7]=cy13c;
		pxy[ 8]=cy20c;	pxy[ 9]=cy21c;	pxy[10]=cy22c;	pxy[11]=cy23c;
		pxy[12]=cy30c;	pxy[13]=cy31c;	pxy[14]=cy32c;	pxy[15]=cy33c;
	}else
	{
		mcr=mcry;	mcg=mcgy;		mcb=mcby;	mcy=mcyy;
		ncr=ncry;	ncg=ncgy;		ncb=ncby;	ncy=ncyy;
		pxy[ 0]=cy00y;	pxy[ 1]=cy01y;	pxy[ 2]=cy02y;	pxy[ 3]=cy03y;
		pxy[ 4]=cy10y;	pxy[ 5]=cy11y;	pxy[ 6]=cy12y;	pxy[ 7]=cy13y;
		pxy[ 8]=cy20y;	pxy[ 9]=cy21y;	pxy[10]=cy22y;	pxy[11]=cy23y;
		pxy[12]=cy30y;	pxy[13]=cy31y;	pxy[14]=cy32y;	pxy[15]=cy33y;
	}

#endif

	mca=255; nca=255;
	mcr2=mcr; mcg2=mcg; mcb2=mcb;
	ncr2=ncr; ncg2=ncg; ncb2=ncb;

//	if((mcr==ncr) && (mcg==ncg) && (mcb==ncb) && (mca==nca))
	if((mcr2==ncr2) && (mcg2==ncg2) && (mcb2==ncb2))
	{
		i=((mcr<<8)&0xF800) | ((mcg<<3)&0x07E0) | ((mcb>>3)&0x001F);
		block[ 0]=i; block[ 1]=i>>8;
		block[ 2]=i; block[ 3]=i>>8;
		block[ 4]=0; block[ 5]=0;
		block[ 6]=0; block[ 7]=0;
		block[ 8]=mcr2;	block[ 9]=mcg2;
		block[10]=mcb2;	block[11]=mca;
		block[12]=ncr2;	block[13]=ncg2;
		block[14]=ncb2;	block[15]=nca;
	}else
	{
		//Emit RGB Block (Opaque)

		acy=(mcy+ncy)>>1;					//Median Luma
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

#if 1
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
#endif

#if 0
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
#endif
	}
	
	if((block[ 0]==0x55) || (block[ 0]==0xAA))
		{ block[ 0]^=0x20; }
}
#endif

