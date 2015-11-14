#include <bgbbtj.h>
#include <time.h>

typedef int (foo)(int);

foo *bar;
// {
//	return(0);
//}

#if 0
int main(int argc, char *argv[])
{
	char tb[256];
	byte *obuf, *tbuf, *o2buf;
	VFILE *fd;
	
	double er, eg, eb, ea;
	double ey, eu, ev;
	double f, g;
	int cr, cg, cb, ca;
	int pr, pg, pb, pa;
	int cy, cu, cv;
	int py, pu, pv;
	int qy, qu, qv;
	int mr, mg, mb;
	int my, mu, mv;
	int md, d, n, w, h;
	int t0, t1, t2, t3;
	int i, j, k, l;

#if 1
	fd=vffopen(argv[1], "rb");
	tbuf=vf_bufferin_sz(fd, &l);
	obuf=gcatomic(l+65536);
//	k=BTLZA_BitEnc_EncodeStreamXLvlZlTest(tbuf, obuf, l, l+65536, 5);
	k=BTLZA_BitEnc_EncodeStream32LvlZl(tbuf, obuf, l, l+65536, 5);
	printf("%d -> %d\n", l, k);
	
	o2buf=gcatomic(l+65536);
	
	t0=clock(); t1=t0; n=0;
	while((t1>=t0) && (t1<(t0+(30*CLOCKS_PER_SEC))))
	{
//		BTLZA_DecodeStreamZl(obuf, o2buf, k, l);
		PDUNZ_DecodeStreamZl(obuf, o2buf, k, l);
		t1=clock(); n++;
		
		t2=t1-t0;
//		printf("%d %d %f\r", n, t1-t0,
//			(((double)n)*l)/(t2/(double)CLOCKS_PER_SEC));
		printf("%d %d %f\r", n, t1-t0,
			(1.0/1000000)*(((double)n)*l)/(t2/(double)CLOCKS_PER_SEC));
	}
	printf("\n");
	
#endif

#if 0
	BGBBTJ_Float16_Init();

	for(i=0; i<256; i++)
	{
		f=i/16.0;
		g=f*M_PI;
		j=BGBBTJ_EncodeFloat16(f);
		k=BGBBTJ_EncodeFloat16(g);
		l=BGBBTJ_AddFloat16(j, k);
		ey=BGBBTJ_DecodeFloat16(l);
		
		printf("+: %f %f %f %f (%04X %04X %04X)\n", f, g, f+g, ey, j, k, l);
//		printf("%f %f %f %f\n", f, g, f+g, ey);
//		printf("\t %04X %04X %04X\n", j, k, l);

		l=BGBBTJ_SubFloat16(j, k);
		ey=BGBBTJ_DecodeFloat16(l);
		printf("-: %f %f %f %f (%04X %04X %04X)\n", f, g, f-g, ey, j, k, l);

		l=BGBBTJ_MulFloat16(j, k);
		ey=BGBBTJ_DecodeFloat16(l);
		printf("*: %f %f %f %f (%04X %04X %04X)\n", f, g, f*g, ey, j, k, l);

		l=BGBBTJ_DivFloat16(j, k);
		ey=BGBBTJ_DecodeFloat16(l);
		printf("/: %f %f %f %f (%04X %04X %04X)\n", f, g, f/g, ey, j, k, l);

		l=BGBBTJ_RcpFloat16(j);
		ey=BGBBTJ_DecodeFloat16(l);
		printf("/: %f %f %f (%04X %04X)\n", f, 1.0/f, ey, j, l);
	}

	for(i=0; i<128; i++)
	{
//		j=((i-0x40)<<7)+0x3C00;
		j=((i-0x38)<<7)+0x3C00;
		f=BGBBTJ_DecodeFloat16(j);
		printf("MF8(%02X) %f (%04X)\n", i, f, j);
	}
#endif

#if 0

#if 0
	for(i=0; i<8; i++)
	{
		py=(i&1)?255:0;
		pu=(i&2)?255:0;
		pv=(i&4)?255:0;
	
//		py=128;
	
		md=999999;
		for(cr=0; cr<256; cr++)
			for(cg=0; cg<256; cg++)
				for(cb=0; cb<256; cb++)
		{
			cy=(( 19595*cr + 38470*cg + 7471*cb + 32768)>>16)+0;
			cu=((-11056*cr - 21712*cg +32768*cb + 32768)>>16)+128;
			cv=(( 32768*cr - 27440*cg - 5328*cb + 32768)>>16)+128;
		
//			d=BGBBTJ_SuperMagenta8_YuvDist(cy, cu, cv);
//			d=BGBBTJ_SuperMagenta8_PointDist(cy, cu, cv, 255, 255, 255);
//			d=BGBBTJ_SuperMagenta8_PointDist(cy, cu, cv, 0, 0, 0);
			d=BGBBTJ_SuperMagenta8_PointDist(cy, cu, cv, py, pu, pv);
			if(d<md)
			{
				mr=cr; mg=cg; mb=cb;
				my=cy; mu=cu; mv=cv;
				md=d;
			}
		}
	
		printf("Min %d Dist=%5d, RGB=%3d,%3d,%3d, "
				"YUV=%3d,%3d,%3d, pYUV=%3d,%3d,%3d\n",
			i, md, mr, mg, mb,
			my, mu, mv, py, pu, pv);
	}
#endif

#if 0
	ey=0; eu=0; ev=0; ea=0; n=0;
	for(cy=0; cy<256; cy++)
	{
		cu=128; cv=128;

#if 1
		j=cy*cy+cu*cu+cv*cv;
		f=255.0/sqrt(j);
//		f=1.0/sqrt(cy/128.0);
//		f=1.5-0.5*(cy/128.0);
		py=cy*f;
		pu=cu*f;
		pv=cv*f;
#endif

//		g=py/148.0;
//		g=sqrt(py/148.0);
//		g=sqrt(py/128.0);
//		g=0.71+0.66*sqrt(py/148.0);
//		g=0.71+0.72*sqrt(py/180.0);
//		g=0.71+0.72*pow(py/180.0, 2);
//		g=1.0/(1.5-0.5*(cy/128.0));
//		g=g*g;
		qy=py*g;
		qu=pu*g;
		qv=pv*g;
		
		ey+=((double)(cy-qy))*(cy-qy);
		eu+=((double)(cu-qu))*(cu-qu);
		ev+=((double)(cv-qv))*(cv-qv);
		ea+=((double)(f-g))*(f-g);
//		ea+=((double)(f-g))*f;
		n++;
	}

	printf("Ey=%f Eu=%f Ev=%f Es=%f\n",
		sqrt(ey/n), sqrt(eu/n), sqrt(ev/n), sqrt(ea/n));
#endif

	er=0; eg=0; eb=0; ea=0; ey=0; n=0;
	for(cr=0; cr<256; cr++)
		for(cg=0; cg<256; cg++)
			for(cb=0; cb<256; cb++)
	{
#if 1
		cy=(( 19595*cr + 38470*cg + 7471*cb + 32768)>>16)+0;
		cu=((-11056*cr - 21712*cg +32768*cb + 32768)>>16)+128;
		cv=(( 32768*cr - 27440*cg - 5328*cb + 32768)>>16)+128;
//		ca=255;
//		ca=248;
		ca=200;

		py=(cy/4)*4;
		pu=(cu/4)*4;
		pv=(cv/4)*4;

		BGBBTJ_SuperMagenta8_ConvRGBA2YUV(
			cr, cg, cb, ca,
			&cy, &cu, &cv);

		BGBBTJ_SuperMagenta8_ConvYUV2RGBA(
			cy, cu, cv,
			&pr, &pg, &pb, &pa);

//		mu=pu-128; mv=pv-128;
//		pr=65536*py          +91881*mv;
//		pg=65536*py- 22554*mu-46802*mv;
//		pb=65536*py+116130*mu;
//		pr>>=16; pg>>=16; pb>>=16;

		pr=(pr<0)?0:((pr>255)?255:pr);
		pg=(pg<0)?0:((pg>255)?255:pg);
		pb=(pb<0)?0:((pb>255)?255:pb);

		py=(( 19595*pr + 38470*pg + 7471*pb + 32768)>>16)+0;
#endif

		er+=((double)(cr-pr))*(cr-pr);
		eg+=((double)(cg-pg))*(cg-pg);
		eb+=((double)(cb-pb))*(cb-pb);
		ea+=((double)(ca-pa))*(ca-pa);
		ey+=((double)(cy-py))*(cy-py);
		n++;
	}
	
	printf("Er=%f Eg=%f Eb=%f Ea=%f Ey=%f\n",
		sqrt(er/n), sqrt(eg/n),
		sqrt(eb/n), sqrt(ea/n),
		sqrt(ey/n));


#if 1
	er=0; eg=0; eb=0; ea=0; ey=0; n=0;
	for(cr=0; cr<256; cr++)
		for(cg=0; cg<256; cg++)
			for(cb=0; cb<256; cb++)
	{
		cy=(( 19595*cr + 38470*cg + 7471*cb + 32768)>>16)+0;

//		cu=(( 16384*cr + 16384*cg - 32768*cb + 32768)>>16)+128;
//		cv=((-32768*cr + 16384*cg + 16384*cb + 32768)>>16)+128;

//		cu=(( 32768*cr - 16384*cg - 16384*cb + 32768)>>16)+128;
//		cv=((-16384*cr - 16384*cg + 32768*cb + 32768)>>16)+128;

//		cu=(( 19595*cr + 38470*cg + 7471*cb + 32768)>>16)+0;
//		cu=(( 25297*cr + 32768*cg + 7471*cb + 32768)>>16)+0;
//		cu=(( 32768*cr + 25297*cg + 7471*cb + 32768)>>16)+0;
//		cv=((-11056*cr - 21712*cg +32768*cb + 32768)>>16)+128;

		cu=(( 22446*cr + 35619*cg + 7471*cb + 32768)>>16)+0;
		cv=((-11056*cr - 21712*cg +32768*cb + 32768)>>16)+128;

//		cu=(( 32768*cr + 25297*cg + 7471*cb + 32768)>>16)+0;
//		cv=((-21712*cr - 11056*cg +32768*cb + 32768)>>16)+128;

#if 0	
		mu=cu-128; mv=cv-128;
		pr=65536*mu;
		pg=32768*mu+32768*mv;
		pb=65536*mv;
		if(pg<0)pg=-pg;
#endif

#if 0
		mv=cv-128;
//		pr=65536*cu;
//		pg=65536*cu- 22554*mv;
		pr=65536*cu- 22554*mv;
		pg=65536*cu;
		pb=65536*cu+116130*mv;
#endif

#if 1
		mv=cv-128;
		pr=65536*cu;
		pg=65536*cu-116130*mv;
		pb=65536*cu+116130*mv;
#endif

		pr>>=16; pg>>=16; pb>>=16;
//		pr=(pr<0)?0:((pr>255)?255:pr);
//		pg=(pg<0)?0:((pg>255)?255:pg);
//		pb=(pb<0)?0:((pb>255)?255:pb);

		py=(( 19595*pr + 38470*pg + 7471*pb + 32768)>>16)+0;

		er+=((double)(cr-pr))*(cr-pr);
		eg+=((double)(cg-pg))*(cg-pg);
		eb+=((double)(cb-pb))*(cb-pb);
//		ea+=((double)(ca-pa))*(ca-pa);
		ey+=((double)(cy-py))*(cy-py);
		n++;
	}
	
	printf("Er=%f Eg=%f Eb=%f Ea=%f Ey=%f\n",
		sqrt(er/n), sqrt(eg/n),
		sqrt(eb/n), sqrt(ea/n),
		sqrt(ey/n));
#endif

#if 1
	w=256; h=256;
	tbuf=malloc(1<<22);
	obuf=malloc(w*h*4);
	
	for(cy=0; cy<256; cy++)
		for(cu=0; cu<256; cu++)
	{
#if 1
		mv=cu-128;
		pr=65536*cy;
//		pg=65536*cy- 22554*mv;
//		pg=65536*cy- 65536*mv;
//		pr=65536*cy- 22554*mv;
//		pr=65536*cy- 65536*mv;
//		pr=65536*cy-116130*mv;
//		pg=65536*cy;
//		pb=65536*cy+116130*mv;

		pr=65536*cy;
//		pg=65536*cy-65536*cu;
//		pb=65536*cu;
		pg=65536*cy-116130*mv;
		pb=65536*cy+116130*mv;

//		pg=65536*cy-116130*mv;
//		pb=65536*cy+116130*mv;
#endif

		pr>>=16; pg>>=16; pb>>=16;
		pr=(pr<0)?0:((pr>255)?255:pr);
		pg=(pg<0)?0:((pg>255)?255:pg);
		pb=(pb<0)?0:((pb>255)?255:pb);

		obuf[(cy*256+cu)*4+0]=pr;
		obuf[(cy*256+cu)*4+1]=pg;
		obuf[(cy*256+cu)*4+2]=pb;
		obuf[(cy*256+cu)*4+3]=255;
	}
#endif

#if 1
	i=BGBBTJ_BufPNG_Encode(tbuf, 1<<20, obuf, w, h);

	sprintf(tb, "%s_dump_yu2.png", "colortst0");
	fd=fopen(tb, "wb");
	if(fd)
	{
		fwrite(tbuf, 1, i, fd);
		fclose(fd);
	}
#endif

#endif

}
#endif

/*
MB2A
R,G,B,d, Pa,Pb,Pc,Pd

d==0: Flat or Special (RGB=Color)
  Pa==0: Flat
  Pa==1: 4x4x1bpp
    Pb=Diff
    Pc/Pd=Pixel Bits
  Pa==2: 2x2x2bpp
    Pb=Diff
    Pc=Pixel Bits
  Pa==3: Skip (No Translate)
  Pa==4: Skip+Translate
	Pc=Y Offset
	Pd=X Offset
d=1..255: Normal

*/

#if 1
void BGBBTJ_BCn_EncodeBlockMB2A(byte *block,
	byte *rgba, int xstride, int ystride,
	int dflat, int d2x2)
{
//	static const char idxtab[16]=
//		{ 1,1,1,1, 1,1,3,3, 2,2,0,0, 0,0,0,0 };
	static const char idxtab[16]=
		{ 0,0,0,0, 0,0,1,1, 2,2,3,3, 3,3,3,3 };
//	static const char idxtab2[16]=
//		{ 0,0,0,0, 2,2,3,3, 4,4,5,5, 1,1,1,1 };
	byte pxa[16], pxy[16];
	int p0, p1, p2, p3, p4, p5, p6, p7;
	int l0, l1, l2, l3a, l3b;
	int mcr, mcg, mcb, mca, mcy;
	int ncr, ncg, ncb, nca, ncy;
	int acr, acg, acb, aca, acy;
	int cr, cg, cb, ca, cy;
	int i, j, k, l;
	
	mcr=255; mcg=255; mcb=255; mca=255; mcy=255;
	ncr=  0; ncg=  0; ncb=  0; nca=  0; ncy=  0;
	acr=  0; acg=  0; acb=  0; aca=  0; acy=  0;
	
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		k=(i*ystride)+(j*xstride);
		cr=rgba[k+0];	cg=rgba[k+1];
		cb=rgba[k+2];	ca=rgba[k+3];
		cy=(cr+(2*cg)+cb)>>2;		//Luma

		pxa[i*4+j]=ca;
		pxy[i*4+j]=cy;

		//Find Mins/Maxs
		if(cy<mcy) { mcr=cr; mcg=cg; mcb=cb; mcy=cy; }
		if(cy>ncy) { ncr=cr; ncg=cg; ncb=cb; ncy=cy; }
		if(ca<mca) { mca=ca; }
		if(ca>nca) { nca=ca; }
		
		acr+=cr; acg+=cg;
		acb+=cb; acy+=cy;
	}

	acr=(acr+8)>>4;
	acg=(acg+8)>>4;
	acb=(acb+8)>>4;

	aca=(mca+nca)>>1;			//Median Alpha
	acy=(mcy+ncy)>>1;			//Median Luma
	l0=49152/(nca-aca+1);		//Fix-Point Scale (Alpha)
	l1=32768/(ncy-acy+1);		//Fix-Point Scale (Luma)
	l2=65536-2048;

	l1=(32768-6144)/(ncy-acy+1);		//Fix-Point Scale (Luma)
	l3a=65536-1024;
	l3b=65536+1024;

	i=ncy-acy;
//	if(acr<i)i=acr;
//	if(acg<i)i=acg;
//	if(acb<i)i=acb;
//	if((255-acr)<i)i=255-acr;
//	if((255-acg)<i)i=255-acg;
//	if((255-acb)<i)i=255-acb;
	
	block[0]=acr;	block[1]=acg;
	block[2]=acb;	block[3]=i;

//	if(i<8)
//	if(i<16)
	if(i<dflat)
	{
		block[3]=0;
		block[4]=0;
		block[5]=0;
		block[6]=0;
		block[7]=0;
		return;
	}

//	if(i<32)
	if(i<d2x2)
	{
		block[3]=0;
		block[4]=2;
		block[5]=i;
		block[6]=0;
		block[7]=0;

		p4=(pxy[ 0]+pxy[ 1]+pxy[ 4]+pxy[ 5])>>2;
		p5=(pxy[ 2]+pxy[ 3]+pxy[ 6]+pxy[ 7])>>2;
		p6=(pxy[ 8]+pxy[ 9]+pxy[12]+pxy[13])>>2;
		p7=(pxy[10]+pxy[11]+pxy[14]+pxy[15])>>2;

		p0=idxtab[((p4-acy)*l1+l3a)>>13];
		p1=idxtab[((p5-acy)*l1+l3b)>>13];
		p2=idxtab[((p6-acy)*l1+l3a)>>13];
		p3=idxtab[((p7-acy)*l1+l3b)>>13];

		block[6]=(p0<<6)|(p1<<4)|(p2<<2)|p3;

//		block[4]=(p0<<6)|(p0<<4)|(p1<<2)|p1;
//		block[5]=(p0<<6)|(p0<<4)|(p1<<2)|p1;
//		block[6]=(p2<<6)|(p2<<4)|(p3<<2)|p3;
//		block[7]=(p2<<6)|(p2<<4)|(p3<<2)|p3;
//		block[8]=(p0<<6)|(p1<<4)|(p2<<2)|p3;

//		block[4]=(p1<<6)|(p1<<4)|(p0<<2)|p0;
//		block[5]=(p1<<6)|(p1<<4)|(p0<<2)|p0;
//		block[6]=(p3<<6)|(p3<<4)|(p2<<2)|p2;
//		block[7]=(p3<<6)|(p3<<4)|(p2<<2)|p2;
		return;
	}

	p0=idxtab[((pxy[ 0]-acy)*l1+l3a)>>13];
	p1=idxtab[((pxy[ 1]-acy)*l1+l3b)>>13];
	p2=idxtab[((pxy[ 2]-acy)*l1+l3a)>>13];
	p3=idxtab[((pxy[ 3]-acy)*l1+l3b)>>13];
//	block[4]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
	block[4]=(p0<<6)|(p1<<4)|(p2<<2)|p3;
	p0=idxtab[((pxy[ 4]-acy)*l1+l3b)>>13];
	p1=idxtab[((pxy[ 5]-acy)*l1+l3a)>>13];
	p2=idxtab[((pxy[ 6]-acy)*l1+l3b)>>13];
	p3=idxtab[((pxy[ 7]-acy)*l1+l3a)>>13];
//	block[5]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
	block[5]=(p0<<6)|(p1<<4)|(p2<<2)|p3;
	p0=idxtab[((pxy[ 8]-acy)*l1+l3a)>>13];
	p1=idxtab[((pxy[ 9]-acy)*l1+l3b)>>13];
	p2=idxtab[((pxy[10]-acy)*l1+l3a)>>13];
	p3=idxtab[((pxy[11]-acy)*l1+l3b)>>13];
//	block[6]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
	block[6]=(p0<<6)|(p1<<4)|(p2<<2)|p3;
	p0=idxtab[((pxy[12]-acy)*l1+l3b)>>13];
	p1=idxtab[((pxy[13]-acy)*l1+l3a)>>13];
	p2=idxtab[((pxy[14]-acy)*l1+l3b)>>13];
	p3=idxtab[((pxy[15]-acy)*l1+l3a)>>13];
//	block[7]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
	block[7]=(p0<<6)|(p1<<4)|(p2<<2)|p3;
}

void BGBBTJ_BCn_EncodeImageMB2A(byte *block,
	byte *rgba, int xs, int ys, int stride, int qf)
{
	int df, d2x2, qr;
	int xs1, ys1;
	int i, j;
	
	qr=100-(qf&127);
	if(qr<0)qr=0;
	
	df=qr*0.16;
//	df=qr*0.32;
	d2x2=qr*0.64;
	
//	df=8;
//	d2x2=16;
	
	xs1=xs>>2; ys1=ys>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		BGBBTJ_BCn_EncodeBlockMB2A(
			block+(i*xs1+j)*16,
			rgba+(i*4*xs+j*4)*stride,
			stride, xs*stride, df, d2x2);
	}
}

int clamp255(int i)
{
	return((i<0)?0:((i>255)?255:i));
//	return(i);
}

void BGBBTJ_BCn_DecodeBlockMB2A(byte *block,
	byte *rgba, int xstride, int ystride)
{
	byte clr[4*4];
	int cd, bt, pxb;
	int i, j, k, l;
	
	i=block[3];
	cd=i; bt=0;
	
	if(i==0)
	{
		if(block[4]==0)
		{
			for(i=0; i<4; i++)
				for(j=0; j<4; j++)
			{
				k=i*ystride+j*xstride;
				rgba[k+0]=block[0];
				rgba[k+1]=block[1];
				rgba[k+2]=block[2];
				rgba[k+3]=255;
			}
			return;
		}	

		if(block[4]==1)
			{ cd=block[5]; bt=1; }
		if(block[4]==2)
			{ cd=block[5]; bt=2; }
	}

	clr[ 0]=clamp255(block[0]-cd);
	clr[ 1]=clamp255(block[1]-cd);
	clr[ 2]=clamp255(block[2]-cd);
	clr[ 3]=255;
	clr[12]=clamp255(block[0]+cd);
	clr[13]=clamp255(block[1]+cd);
	clr[14]=clamp255(block[2]+cd);	
	clr[15]=255;

	clr[ 4]=(clr[0]*11+clr[12]*5)>>4;
	clr[ 5]=(clr[1]*11+clr[13]*5)>>4;
	clr[ 6]=(clr[2]*11+clr[14]*5)>>4;
	clr[ 7]=255;
	clr[ 8]=(clr[0]*5+clr[12]*11)>>4;
	clr[ 9]=(clr[1]*5+clr[13]*11)>>4;
	clr[10]=(clr[2]*5+clr[14]*11)>>4;
	clr[11]=255;
	
	if(bt==0)
	{
		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
		{
			k=i*ystride+j*xstride;
			l=((block[4+i]>>(6-2*j))&3)*4;
			rgba[k+0]=clr[l+0];
			rgba[k+1]=clr[l+1];
			rgba[k+2]=clr[l+2];
			rgba[k+3]=clr[l+3];
		}
	}else if(bt==1)
	{
		pxb=(block[6]<<8)|block[7];
		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
		{
			k=i*ystride+j*xstride;
			l=(pxb>>((3-i)*4+(3-j)))&1;
			l=(l?3:0)*4;
			rgba[k+0]=clr[l+0];
			rgba[k+1]=clr[l+1];
			rgba[k+2]=clr[l+2];
			rgba[k+3]=clr[l+3];
		}
	}else if(bt==2)
	{
		pxb=block[6];
		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
		{
			k=i*ystride+j*xstride;
			l=2*(i>>1)+(j>>1);
			l=((pxb>>(6-2*l))&3)*4;
			rgba[k+0]=clr[l+0];
			rgba[k+1]=clr[l+1];
			rgba[k+2]=clr[l+2];
			rgba[k+3]=clr[l+3];
		}
	}
}

void BGBBTJ_BCn_DecodeImageMB2A(byte *block,
	byte *rgba, int xs, int ys, int stride)
{
	int xs1, ys1;
	int i, j;
	
	xs1=xs>>2; ys1=ys>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		BGBBTJ_BCn_DecodeBlockMB2A(
			block+(i*xs1+j)*16,
			rgba+(i*4*xs+j*4)*stride,
			stride, xs*stride);
	}
}

int BGBBTJ_BCn_CheckBlockDistMB2A(
	byte *blocka, byte *blockb, int err)
{
	int cra, cga, cba, caa;
	int crb, cgb, cbb, cab;
	int dr, dg, db, da;
	int dy;
	
	cra=blocka[0];	cga=blocka[1];
	cba=blocka[2];	caa=blocka[3];
	crb=blockb[0];	cgb=blockb[1];
	cbb=blockb[2];	cab=blockb[3];

	if(!caa && blocka[4])
		caa=blocka[5];
	if(!cba && blockb[4])
		cba=blockb[5];

	dr=cra-crb; if(dr<0)dr=-dr;
	dg=cga-cgb; if(dg<0)dg=-dg;
	db=cba-cbb; if(db<0)db=-db;
	da=caa-cab; if(da<0)da=-da;
	
	dy=(0.35*dr)+(0.5*dg)+(0.15*db);
	if(dy>err)
		return(0);
	if((dy+caa+cab)<err)
		return(1);
		
	return(1);
}

int PickColorVector(int cr, int cg, int cb)
{
	double vr, vg, vb, cd;
	int cm;

	vr=cr/255.0;	vg=cg/255.0;	vb=cb/255.0;
	cd=sqrt((vr*vr)+(vg*vg)+(vb*vb));
	if(cd>0)
	{
		cr=cr/cd;
		cg=cg/cd;
		cb=cb/cd;
	}
	
	cm=0;
//	if(cr>180)cm|=1;
//	if(cg>180)cm|=2;
//	if(cb>180)cm|=4;

	if(cr>154)cm|=1;
	if(cg>154)cm|=2;
	if(cb>154)cm|=4;

//	if(cr>167)cm|=1;
//	if(cg>167)cm|=2;
//	if(cb>167)cm|=4;

//	if(cr>160)cm|=1;
//	if(cg>160)cm|=2;
//	if(cb>160)cm|=4;

//	if(cr>192)cm|=8;
//	if(cg>192)cm|=8;
//	if(cb>192)cm|=8;

//	if(cr>180)cm|=8;
//	if(cg>180)cm|=8;
//	if(cb>180)cm|=8;

	if(cr>167)cm|=8;
	if(cg>167)cm|=8;
	if(cb>167)cm|=8;
	
//	if(!cm)cm=7;

	return(cm);
}

int BGBBTJ_BT1F_QuantizeBlocks(
	byte *oblks, byte *iblks, byte *lblks,
	int xs, int ys, int qfl, int stride)
{
	int cr, cg, cb, ca;
	int pr, pg, pb, pa;
	int mr, mg, mb, ma;

	int md, d, n, w, h, w2, h2, lq, qr;
	int t0, t1, t2, t3;
	int i, j, k, l;

	w2=xs>>2;
	h2=ys>>2;
	n=w2*h2;

	qr=100-(qfl&127);
	if(qr<0)qr=0;
	
//	lq=0.16*qr;
	lq=0.32*qr;
	if(lq<1)lq=1;

	pr=0; pg=0; pb=0; pa=0;
	for(i=0; i<n; i++)
	{
		l=i*stride;

		if(lblks)
		{
			//skip
			if(BGBBTJ_BCn_CheckBlockDistMB2A(
				iblks+l, lblks+l, lq))
			{
				oblks[l+0]=pr;
				oblks[l+1]=pg;
				oblks[l+2]=pb;
				oblks[l+3]=0;
				oblks[l+4]=3;
				oblks[l+5]=pa;
				oblks[l+6]=0;
				oblks[l+7]=0;
				continue;
			}
		}

		cr=iblks[l+0];		cg=iblks[l+1];
		cb=iblks[l+2];		ca=iblks[l+3];
		if((iblks[l+3]==0) && (iblks[l+4]!=0))
			ca=iblks[l+5];
		
		mr=cr-pr;	mg=cg-pg;
		mb=cb-pb;	ma=ca-pa;

//		mr=(mr/8)*8;	mg=(mg/8)*8;
//		mb=(mb/8)*8;	ma=(ma/8)*8;

		mr=(mr/lq)*lq;	mg=(mg/lq)*lq;
		mb=(mb/lq)*lq;	ma=(ma/lq)*lq;

		pr=pr+mr;	pg=pg+mg;	pb=pb+mb;	pa=pa+ma;

		if(iblks[l+3]==0)
		{
			oblks[l+0]=pr;
			oblks[l+1]=pg;
			oblks[l+2]=pb;
			oblks[l+3]=0;
			oblks[l+4]=iblks[l+4];
			oblks[l+5]=pa;
			oblks[l+6]=iblks[l+6];
			oblks[l+7]=iblks[l+7];
		}else
		{
			oblks[l+0]=pr;
			oblks[l+1]=pg;
			oblks[l+2]=pb;
			oblks[l+3]=pa;
			oblks[l+4]=iblks[l+4];
			oblks[l+5]=iblks[l+5];
			oblks[l+6]=iblks[l+6];
			oblks[l+7]=iblks[l+7];
		}
	}

	return(0);
}

void BGBBTJ_BT1F_EncodeBlocks_EmitUIntParam(
	byte **rct, int var, int val)
{
	byte *ct;
	
	ct=*rct;
	if((var<32) && (val<32))
	{
		*ct++=0xC4+(var>>3);
		*ct++=(var<<5)|val;
		*rct=ct;
		return;
	}

	if((var<64) && (val<1024))
	{
		*ct++=0xC8;
		*ct++=(var<<2)|(val>>8);
		*ct++=val;
		*rct=ct;
		return;
	}
}

int BGBBTJ_BT1F_EncodeBlocks(
	byte *iblks, byte *lblks,
	byte **rclrct, byte **rpixct, byte **rcmdct,
	int xs, int ys, int qfl, int stride)
{
	byte *clrct, *pixct, *cmdct;
	byte *cs, *cse, *cs1, *cs2;

	int cr, cg, cb, ca;
	int pr, pg, pb, pa, pv;
	int mr, mg, mb, ma;
	int cr0, cg0, cb0, ca0;
	int cr1, cg1, cb1, ca1;
	int md, d, n, w, h, w2, h2, lq, qr;
	int t0, t1, t2, t3;
	int i, j, k, l;

	w2=xs>>2;
	h2=ys>>2;
	n=w2*h2;

	qr=100-(qfl&127);
	if(qr<0)qr=0;
	
//	lq=0.16*qr;
	lq=0.32*qr;
	if(lq<1)lq=1;

	cs=iblks; cse=iblks+(n*stride);
	clrct=*rclrct;
	pixct=*rpixct;
	cmdct=*rcmdct;

	BGBBTJ_BT1F_EncodeBlocks_EmitUIntParam(&cmdct, 2, lq);

//	*cmdct++=0xC4;
//	*cmdct++=(2<<5)|lq;

#if 1
	pr=0; pg=0; pb=0; pa=0; pv=0;
//	cs=bbuf; cse=bbuf+(n*16);
	while(cs<cse)
	{
		if((cs[3]==0) && (cs[4]==3))
		{
			for(i=1; i<32; i++)
			{
				cs1=cs+i*16;
				if(cs1>cse)break;
				
				cr1=cs1[0]; cg1=cs1[1];
				cb1=cs1[2]; ca1=cs1[3];
				if((ca1!=0) || (cs1[4]!=3))
					break;
			}

			*cmdct++=0x80+(i-1);
			
			cs+=i*16;
			continue;
		}

		cr=cs[0];		cg=cs[1];
		cb=cs[2];		ca=cs[3];
		if((ca==0) && (cs[4]!=0))
			ca=cs[5];

		mr=cr-pr;	mg=cg-pg;
		mb=cb-pb;	ma=ca-pa;
//		mr=mr/4;	mg=mg/4;
//		mb=mb/4;	ma=ma/4;
		mr=mr/lq;	mg=mg/lq;
		mb=mb/lq;	ma=ma/lq;

		if(!pv)
		{
			*cmdct++=0x12;
			*clrct++=cr;	*clrct++=cg;
			*clrct++=cb;	*clrct++=ca;
			pr=cr;	pg=cg;
			pb=cb;	pa=ca;
			pv=1;
		}else if(mr|mg|mb|ma)
		{
			*cmdct++=0x02;
			*clrct++=mr;	*clrct++=mg;
			*clrct++=mb;	*clrct++=ma;
			pr=pr+(mr*lq);	pg=pg+(mg*lq);
			pb=pb+(mb*lq);	pa=pa+(ma*lq);
		}

		if((cs[3]==0) && (cs[4]==0))
		{
			for(i=1; i<16; i++)
			{
				cs1=cs+i*16;
				if(cs1>cse)break;
				
				cr1=cs1[0]; cg1=cs1[1];
				cb1=cs1[2]; ca1=cs1[3];
				if((ca1!=0) || (cs1[4]!=0))
					break;

				mr=cr1-cr;	mg=cg1-cg;
				mb=cb1-cb;
				if(mr || mg || mb)
					break;
			}

			*cmdct++=0x20+(i-1);
			
			cs+=i*16;
			continue;
		}

//		if(ca<32)
		if((cs[3]==0) && (cs[4]==2))
		{
			for(i=1; i<16; i++)
			{
				cs1=cs+i*16;
				if(cs1>cse)break;
				
				cr1=cs1[0]; cg1=cs1[1];
				cb1=cs1[2]; ca1=cs1[3];
				if((ca1!=0) || (cs1[4]!=2))
					break;

				mr=cr1-cr;	mg=cg1-cg;
				mb=cb1-cb;
				if(mr||mg||mb)
					break;
			}

			*cmdct++=0x30+(i-1);
			for(j=0; j<i; j++)
				*pixct++=cs[j*16+6];
			
			cs+=i*16;
			continue;
		}

		if((cs[3]==0) && (cs[4]==1))
		{
			for(i=1; i<16; i++)
			{
				cs1=cs+i*16;
				if(cs1>cse)break;
				
				cr1=cs1[0]; cg1=cs1[1];
				cb1=cs1[2]; ca1=cs1[3];
				if((ca1!=0) || (cs1[4]!=1))
					break;

				mr=cr1-cr;	mg=cg1-cg;
				mb=cb1-cb;
				if(mr||mg||mb)
					break;
			}

			*cmdct++=0x40+(i-1);
			for(j=0; j<i; j++)
			{
				*pixct++=cs[j*16+6];
				*pixct++=cs[j*16+7];
			}
			
			cs+=i*16;
			continue;
		}

		for(i=1; i<16; i++)
		{
			cs1=cs+i*16;
			if(cs1>cse)break;
			
			cr1=cs1[0]; cg1=cs1[1];
			cb1=cs1[2]; ca1=cs1[3];
			if(ca1==0)break;

			mr=cr1-cr;	mg=cg1-cg;
			mb=cb1-cb;	ma=ca1-ca;
			if(mr||mg||mb||ma)
				break;
		}

		*cmdct++=0x50+(i-1);
		for(j=0; j<i; j++)
		{
			*pixct++=cs[j*16+4];
			*pixct++=cs[j*16+5];
			*pixct++=cs[j*16+6];
			*pixct++=cs[j*16+7];
		}
		cs+=i*16;
		continue;
	}
#endif

	*rclrct=clrct;
	*rpixct=pixct;
	*rcmdct=cmdct;
	return(0);
}

int BGBBTJ_BT1F_EncodeImageBlocks(
	byte *obuf, byte *iblks, byte *lblks,
	byte *clrbuf, byte *pixbuf,
	byte *cmdbuf, byte *tmpbuf,
	int xs, int ys, int qfl, int stride)
{
	byte *clrct, *pixct, *cmdct;
	byte *ct;
	int i, j, k;

	clrct=clrbuf;
	pixct=pixbuf;
	cmdct=cmdbuf;
	ct=obuf;

	BGBBTJ_BT1F_EncodeBlocks(iblks, lblks,
		&clrct, &pixct, &cmdct,
		xs, ys, qfl, stride);

	i=0;
	*ct++=0xE4;		*ct++=i>>16;
	*ct++=i>>8;		*ct++=i;
	*ct++='B';		*ct++='T';
	*ct++='1';		*ct++='F';

	i=BTLZA_BitEnc_EncodeStreamXLvlZlc(
		cmdbuf, tmpbuf, cmdct-cmdbuf, 1<<24, 9);
	*ct++=0xE3;		*ct++=i>>16;
	*ct++=i>>8;		*ct++=i;
	*ct++='Z';		*ct++='B';
	memcpy(ct, tmpbuf, i);
	ct+=i;
	j=cmdct-cmdbuf;
	printf("CmdBuf %d/%d\n", i, j);

	i=BTLZA_BitEnc_EncodeStreamXLvlZlc(
		clrbuf, tmpbuf, clrct-clrbuf, 1<<24, 9);
	*ct++=0xE3;		*ct++=i>>16;
	*ct++=i>>8;		*ct++=i;
	*ct++='Z';		*ct++='C';
	memcpy(ct, tmpbuf, i);
	ct+=i;
	j=clrct-clrbuf;
	printf("ClrBuf %d/%d\n", i, j);

	i=BTLZA_BitEnc_EncodeStreamXLvlZlc(
		pixbuf, tmpbuf, pixct-pixbuf, 1<<24, 9);
	*ct++=0xE3;		*ct++=i>>16;
	*ct++=i>>8;		*ct++=i;
	*ct++='Z';		*ct++='P';
	memcpy(ct, tmpbuf, i);
	ct+=i;
	j=pixct-pixbuf;
	printf("PixBuf %d/%d\n", i, j);
	
	i=ct-obuf;
	obuf[1]=i>>16;
	obuf[2]=i>>8;
	obuf[3]=i;
	
	j=3*6+8;
	j+=cmdct-cmdbuf;
	j+=clrct-clrbuf;
	j+=pixct-pixbuf;
	printf("FrameSize %d/%d\n", i, j);

	return(i);
}

int main(int argc, char *argv[])
{
	char tb[256];
	byte *ibuf, *obuf, *tbuf, *o2buf;
	byte *clrb, *pixb, *cmdb;
	byte *clrct, *pixct, *cmdct;
	byte *cs, *cse, *cs1, *cs2;
	byte *bbuf;
	VFILE *fd;
	char *str;
	
	double er, eg, eb, ea;
	double ey, eu, ev;
	double f, g;
	int cr, cg, cb, ca;
	int pr, pg, pb, pa;
	int cr0, cg0, cb0, ca0;
	int cr1, cg1, cb1, ca1;
	int cy, cu, cv;
	int py, pu, pv;
	int qy, qu, qv;
	int mr, mg, mb, ma;
	int my, mu, mv;
	int md, d, n, w, h, w2, h2;
	int t0, t1, t2, t3;
	int i, j, k, l;

	str="tst2_big/DSC00057_3.png";
	ibuf=BGBBTJ_PNG_LoadRaw(str, &w, &h);

	tbuf=malloc(1<<24);

	obuf=malloc(w*h*4);
//	memcpy(obuf, ibuf, w*h*4);

	w2=w>>2; h2=h>>2; n=w2*h2;
	bbuf=malloc(w2*h2*16);

	clrb=malloc(n*4);
	pixb=malloc(n*4);
	cmdb=malloc(n*4);
	clrct=clrb;
	pixct=pixb;
	cmdct=cmdb;

	BGBBTJ_BCn_EncodeImageMB2A(bbuf, ibuf, w, h, 4, 30);

	BGBBTJ_BT1F_QuantizeBlocks(bbuf, bbuf, NULL, w, h, 30, 16);

//	BGBBTJ_BT1F_EncodeBlocks(bbuf, NULL,
//		&clrct, &pixct, &cmdct,
//		w, h, 90, 16);

	BGBBTJ_BT1F_EncodeImageBlocks(tbuf, bbuf, NULL,
		clrb, pixb, cmdb, tbuf+(1<<23), w, h, 30, 16);

//	t0=BTLZA_BitEnc_EncodeStreamXLvl(clrb, tbuf, n*4, 1<<24, 9);
//	t1=BTLZA_BitEnc_EncodeStreamXLvl(pixb, tbuf, n*4, 1<<24, 9);
//	printf("Sz=%d, SzClrB=%d, SzPixB=%d, SzRaw=%d\n", t0+t1, t0, t1, n*8);

//	t3=(clrct-clrb)+(pixct-pixb)+(cmdct-cmdb);
//	t0=BTLZA_BitEnc_EncodeStreamXLvl(clrb, tbuf, clrct-clrb, 1<<24, 9);
//	t1=BTLZA_BitEnc_EncodeStreamXLvl(pixb, tbuf, pixct-pixb, 1<<24, 9);
//	t2=BTLZA_BitEnc_EncodeStreamXLvl(cmdb, tbuf, cmdct-cmdb, 1<<24, 9);
//	printf("Sz=%d, SzClrB=%d/%d, SzPixB=%d/%d, SzCmdB=%d/%d, SzRaw=%d\n",
//		t0+t1+t2,
//		t0, clrct-clrb,
//		t1, pixct-pixb,
//		t2, cmdct-cmdb,
//		t3);

	BGBBTJ_BCn_DecodeImageMB2A(bbuf, obuf, w, h, 4);

#if 0
	for(i=0; i<h; i++)
		for(j=0; j<w; j++)
	{
		k=(i*w+j)*4;
		cr=ibuf[k+0];	cg=ibuf[k+1];
		cb=ibuf[k+2];	ca=ibuf[k+3];

//		cy=(cr+2*cg+cb)>>2;
		cy=0.35*cr+0.50*cg+0.15*cb;

		cv=PickColorVector(cr, cg, cb);
		
//		cv=7;
		if(cv&8)
		{
//			mr=(cv&1)?255:128;
//			mg=(cv&2)?255:128;
//			mb=(cv&4)?255:128;

//			mr=(cv&1)?255:192;
//			mg=(cv&2)?255:192;
//			mb=(cv&4)?255:192;

			mr=(cv&1)?255:160;
			mg=(cv&2)?255:160;
			mb=(cv&4)?255:160;
		}else if(cv)
		{
//			mr=(cv&1)?255:192;
//			mg=(cv&2)?255:192;
//			mb=(cv&4)?255:192;

//			mr=(cv&1)?255:224;
//			mg=(cv&2)?255:224;
//			mb=(cv&4)?255:224;

			mr=(cv&1)?255:208;
			mg=(cv&2)?255:208;
			mb=(cv&4)?255:208;
		}else
		{
			mr=224;
			mg=224;
			mb=224;
		}

		cr=(mr*cy)>>8;
		cg=(mg*cy)>>8;
		cb=(mb*cy)>>8;

		obuf[k+0]=cr;	obuf[k+1]=cg;
		obuf[k+2]=cb;	obuf[k+3]=ca;
	}
#endif

#if 1
	i=BGBBTJ_BufPNG_Encode(tbuf, 1<<24, obuf, w, h);

	sprintf(tb, "%s_dump_mcv1.png", "colortst0");
	fd=fopen(tb, "wb");
	if(fd)
	{
		fwrite(tbuf, 1, i, fd);
		fclose(fd);
	}
#endif
}
#endif
