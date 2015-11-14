#include <bgbbtj.h>

BGBBTJ_API int BGBBTJ_SplitHdrBlockToneMap_FixedQ3_11(
	s16 *rgba, s16 *tmrgba, s16 *tnrgba, byte *orgba,
	int xs, int ys, int qfl)
{
	int xs1, ys1, xs2, ys2;
	int x1, y1, x2, y2;
	int c0, c1, c2, c3;
	int cr, cg, cb, ca;
	int mr, mg, mb, ma;
	int nr, ng, nb, na;
	int rr, rg, rb, ra;
	int pr, pg, pb, pa;
	int i, j, k;

	xs1=(xs+3)/4;
	ys1=(ys+3)/4;

#if 0
	for(y1=0; y1<ys; y1++)
		for(x1=0; x1<xs; x1++)
	{
		cr=rgba[(y1*xs+x1)*4+0];
		cg=rgba[(y1*xs+x1)*4+1];
		cb=rgba[(y1*xs+x1)*4+2];
		ca=rgba[(y1*xs+x1)*4+3];

		pr=cr>>4;		pg=cg>>4;
		pb=cb>>4;		pa=ca>>4;
			
		pr=(pr<0)?0:((pr>255)?255:pr);
		pg=(pg<0)?0:((pg>255)?255:pg);
		pb=(pb<0)?0:((pb>255)?255:pb);
		pa=(pa<0)?0:((pa>255)?255:pa);

		orgba[(y1*xs+x1)*4+0]=pr;
		orgba[(y1*xs+x1)*4+1]=pg;
		orgba[(y1*xs+x1)*4+2]=pb;
		orgba[(y1*xs+x1)*4+3]=pa;
	}
	return;
#endif

	for(y1=0; y1<ys1; y1++)
		for(x1=0; x1<xs1; x1++)
	{
		mr=32767;	mg=32767;	mb=32767;	ma=32767;
		nr=-32767;	ng=-32767;	nb=-32767;	na=-32767;
		for(y2=0; y2<4; y2++)
			for(x2=0; x2<4; x2++)
		{
			cr=rgba[((y1*4+y2)*xs+(x1*4+x2))*4+0];
			cg=rgba[((y1*4+y2)*xs+(x1*4+x2))*4+1];
			cb=rgba[((y1*4+y2)*xs+(x1*4+x2))*4+2];
			ca=rgba[((y1*4+y2)*xs+(x1*4+x2))*4+3];
			
			if(cr<mr) { mr=cr; }
			if(cg<mg) { mg=cg; }
			if(cb<mb) { mb=cb; }
			if(ca<ma) { ma=ca; }

			if(cr>nr) { nr=cr; }
			if(cg>ng) { ng=cg; }
			if(cb>nb) { nb=cb; }
			if(ca>na) { na=ca; }
		}
	
		c0=(mr+2*mg+mb)/4;
		c1=(nr+2*ng+nb)/4;

//		c0=0; c1=255*16;

//		if(c0>(32*16))c0=32*16;
//		if(c1<(224*16))c1=224*16;
		if(c0>(0*16))c0=0*16;
		if(c1<(255*16))c1=255*16;

//		if((c1-c0)<(8*16))
//			{ c0-=4*16; c1+=4*16; }

//		if((c1-c0)<(32*16))
//			{ c0-=16*16; c1+=16*16; }

#if 1
		if((c1-c0)<(64*16))
		{
			if(c0>(128*16))
				{ c0-=64*16; }
			else if(c1<(128*16))
				{ c1+=64*16; }
			else
				{ c0-=32*16; c1+=32*16; }
		}
#endif

		if(c0<mr)mr=(3*c0+mr)/4;
		if(c0<mg)mg=(3*c0+mg)/4;
		if(c0<mb)mb=(3*c0+mb)/4;

		if(c1>nr)nr=(3*c1+nr)/4;
		if(c1>ng)ng=(3*c1+ng)/4;
		if(c1>nb)nb=(3*c1+nb)/4;
		
//		if(ma>(252*16))ma=(252*16);
//		if(na<(4*16))na=(4*16);

		if((na-ma)<(8*16))
			{ ma-=4*16; na+=4*16; }

		tmrgba[(y1*xs1+x1)*4+0]=mr;		tmrgba[(y1*xs1+x1)*4+1]=mg;
		tmrgba[(y1*xs1+x1)*4+2]=mb;		tmrgba[(y1*xs1+x1)*4+3]=ma;
		tnrgba[(y1*xs1+x1)*4+0]=nr;		tnrgba[(y1*xs1+x1)*4+1]=ng;
		tnrgba[(y1*xs1+x1)*4+2]=nb;		tnrgba[(y1*xs1+x1)*4+3]=na;
	}

	for(i=0; i<4; i++)
	{
		for(y1=0; y1<(ys1-1); y1++)
			for(x1=0; x1<(xs1-1); x1++)
				for(k=0; k<4; k++)
		{
			c0=tmrgba[((y1+0)*xs1+(x1+0))*4+k];
			c1=tmrgba[((y1+0)*xs1+(x1+1))*4+k];
			c2=tmrgba[((y1+1)*xs1+(x1+0))*4+k];
			c3=tmrgba[((y1+1)*xs1+(x1+1))*4+k];
		
			c1=(c0+c1+c2+c3)>>2;
			if(c1<c0)c0=c1;
			tmrgba[((y1+0)*xs1+(x1+0))*4+k]=c0;

			c0=tnrgba[((y1+0)*xs1+(x1+0))*4+k];
			c1=tnrgba[((y1+0)*xs1+(x1+1))*4+k];
			c2=tnrgba[((y1+1)*xs1+(x1+0))*4+k];
			c3=tnrgba[((y1+1)*xs1+(x1+1))*4+k];
		
			c1=(c0+c1+c2+c3)>>2;
			if(c1>c0)c0=c1;
			tnrgba[((y1+0)*xs1+(x1+0))*4+k]=c0;
		}
	}
	
	for(y1=0; y1<ys1; y1++)
		for(x1=0; x1<xs1; x1++)
	{
		mr=tmrgba[(y1*xs1+x1)*4+0];
		mg=tmrgba[(y1*xs1+x1)*4+1];
		mb=tmrgba[(y1*xs1+x1)*4+2];
		ma=tmrgba[(y1*xs1+x1)*4+3];

		nr=tnrgba[(y1*xs1+x1)*4+0];
		ng=tnrgba[(y1*xs1+x1)*4+1];
		nb=tnrgba[(y1*xs1+x1)*4+2];
		na=tnrgba[(y1*xs1+x1)*4+3];
		
		rr=nr-mr;	rg=ng-mg;
		rb=nb-mb;	ra=na-ma;
		for(y2=0; y2<4; y2++)
			for(x2=0; x2<4; x2++)
		{
			cr=rgba[((y1*4+y2)*xs+(x1*4+x2))*4+0];
			cg=rgba[((y1*4+y2)*xs+(x1*4+x2))*4+1];
			cb=rgba[((y1*4+y2)*xs+(x1*4+x2))*4+2];
			ca=rgba[((y1*4+y2)*xs+(x1*4+x2))*4+3];
			
			pr=(256*(cr-mr))/rr;
			pg=(256*(cg-mg))/rg;
			pb=(256*(cb-mb))/rb;
			pa=(256*(ca-ma))/ra;

//			pr=cr>>4;
//			pg=cg>>4;
//			pb=cb>>4;
//			pa=ca>>4;
			
			pr=(pr<0)?0:((pr>255)?255:pr);
			pg=(pg<0)?0:((pg>255)?255:pg);
			pb=(pb<0)?0:((pb>255)?255:pb);
			pa=(pa<0)?0:((pa>255)?255:pa);

			orgba[((y1*4+y2)*xs+(x1*4+x2))*4+0]=pr;
			orgba[((y1*4+y2)*xs+(x1*4+x2))*4+1]=pg;
			orgba[((y1*4+y2)*xs+(x1*4+x2))*4+2]=pb;
			orgba[((y1*4+y2)*xs+(x1*4+x2))*4+3]=pa;
		}
	}
	return(0);
}

#if 0
// BGBBTJ_API byte *BGBBTJ_BT1C_EncodeToneMapSpan0(
	byte *ct, int osz, s16 *rgba0, s16 *rgba1,
	int xs, int stride)
{
	s16 *csa, *csb;
	int pxa, pxb, pxc, pxd;
	int p, p0, p1;
	int i, j, k;
	
	csa=rgba0;
	csb=rgba1;
	
	pxa=0;
	pxc=0;
	for(i=0; i<xs; i++)
	{
		pxb=*csa; pxd=*csb;
//		p0=pxb+pxc-pxa;
//		p1=(3*pxb+3*pxc+2*pxa)>>3;
//		p1=(pxb+pxc)>>1;
//		p=(p0+p1)>>1;

		p=(3*pxb+3*pxc-2*pxa)>>2;
	}
}
#endif

BGBBTJ_API int BGBBTJ_BT1C_EncodeToneMapImage(
	byte *obuf, int osz, s16 *rgba,
	int xs, int ys, int qfl, int stride)
{
	byte *ct;
	int i0, i1, i2, i3;
	int i, j, k, l, zc;
	
	ct=obuf;
	for(i=0; i<4; i++)
	{
		l=0; zc=0;
		for(j=0; j<ys; j++)
			for(k=0; k<xs; k++)
		{
			i0=rgba[(j*xs+k)*4+i];
			i1=i0-l;
			l=i0;
			if(!i1) { zc++; continue; }
			
			i2=(i1<0)?(((-i1)<<1)-1):(i1<<1);
			
			if(zc>31)
			{
				*ct++=0xFF;
				ct=BGBBTJ_EmitSVLI(ct, zc);
				ct=BGBBTJ_EmitVLI(ct, i2);
			}else if((zc>6) || (i2>31))
			{
				*ct++=0xE0|zc;
				ct=BGBBTJ_EmitVLI(ct, i2);
			}else
			{
				*ct++=(zc<<5)|i2;
			}
		}
		
		if(zc>31)
		{
			*ct++=0xFF;
			ct=BGBBTJ_EmitSVLI(ct, zc);
			ct=BGBBTJ_EmitVLI(ct, 0);
		}else if(zc>6)
		{
			*ct++=0xE0|zc;
			ct=BGBBTJ_EmitVLI(ct, 0);
		}
	}
	
	return(ct-obuf);
}

BGBBTJ_API int BGBBTJ_BT1C_EncodeToneMap(
	byte *obuf, int osz, s16 *tmrgba, s16 *tnrgba,
	int xs, int ys, int qfl, int stride)
{
	byte *ct, *cte, *ctb, *ct0;
	int xs1, ys1;
	int i, j, k, l;

	xs1=(xs+3)/4;
	ys1=(ys+3)/4;

	ct=obuf;
	
	ct0=ct;
	*ct++=0xE3;		*ct++=0x00;
	*ct++=0x00;		*ct++=0x00;
	*ct++='T';		*ct++='M';
	ct+=BGBBTJ_BT1C_EncodeToneMapImage(ct, osz-(ct-obuf), tmrgba,
		xs1, ys1, qfl, stride);
	i=ct-ct0;
	if(i>6)
	{
		ct0[1]=(i>>16)&255;
		ct0[2]=(i>>8)&255;
		ct0[3]=(i>>0)&255;
	}else
	{
		ct=ct0;
	}

	ct0=ct;
	*ct++=0xE3;		*ct++=0x00;
	*ct++=0x00;		*ct++=0x00;
	*ct++='T';		*ct++='N';
	ct+=BGBBTJ_BT1C_EncodeToneMapImage(ct, osz-(ct-obuf), tnrgba,
		xs1, ys1, qfl, stride);
	i=ct-ct0;
	if(i>6)
	{
		ct0[1]=(i>>16)&255;
		ct0[2]=(i>>8)&255;
		ct0[3]=(i>>0)&255;
	}else
	{
		ct=ct0;
	}

	return(ct-obuf);
}
