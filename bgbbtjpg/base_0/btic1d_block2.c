/*
Integer block-decoding logic.
*/

#include <bgbbtj.h>

void BGBBTJ_BT1D_DecodeBlockBX1AI_FillPlanesFlat(
	s32 *pxy, s32 *pxu, s32 *pxv, s32 *pxa,
	s32 cy, s32 cu, s32 cv, s32 ca)
{
	pxy[ 0]=pxy[ 1]=pxy[ 4]=pxy[ 5]=cy;
	pxy[ 2]=pxy[ 3]=pxy[ 6]=pxy[ 7]=cy;
	pxy[ 8]=pxy[ 9]=pxy[12]=pxy[13]=cy;
	pxy[10]=pxy[11]=pxy[14]=pxy[15]=cy;
	pxu[ 0]=pxu[ 1]=pxu[ 4]=pxu[ 5]=cu;
	pxu[ 2]=pxu[ 3]=pxu[ 6]=pxu[ 7]=cu;
	pxu[ 8]=pxu[ 9]=pxu[12]=pxu[13]=cu;
	pxu[10]=pxu[11]=pxu[14]=pxu[15]=cu;
	pxv[ 0]=pxv[ 1]=pxv[ 4]=pxv[ 5]=cv;
	pxv[ 2]=pxv[ 3]=pxv[ 6]=pxv[ 7]=cv;
	pxv[ 8]=pxv[ 9]=pxv[12]=pxv[13]=cv;
	pxv[10]=pxv[11]=pxv[14]=pxv[15]=cv;
	pxa[ 0]=pxa[ 1]=pxa[ 4]=pxa[ 5]=ca;
	pxa[ 2]=pxa[ 3]=pxa[ 6]=pxa[ 7]=ca;
	pxa[ 8]=pxa[ 9]=pxa[12]=pxa[13]=ca;
	pxa[10]=pxa[11]=pxa[14]=pxa[15]=ca;
}

void BGBBTJ_BT1D_DecodeBlockBX1AI_PlanesToRGBA(
	s32 *pxy, s32 *pxu, s32 *pxv, s32 *pxa,
	void *rgba, int xstride, int ystride, int clrs)
{
	byte *rgba_b;
	s16 *rgba_s;
	int ct0, ct1;
	int cy, cu, cv, cr, cg, cb, ca;
	int i, j, k, l;

#if 1
	if(clrs==BGBBTJ_JPG_RGBA)
	{
		rgba_b=rgba;
		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
		{
			k=(i*ystride)+(j*xstride);
			l=i*4+j;
			cy=pxy[l];	cu=pxu[l];
			cv=pxv[l];	ca=pxa[l];

			ct0=(cu>>1);
			ct1=(cv>>1);
			cg=cy+ct0;
			cr=cg-cu-ct1;
			cb=cg-cu+ct1;
			cr=cr>>4;	cg=cg>>4;
			cb=cb>>4;	ca=cb>>4;

#if 0
			cg=cy+(cu/2);
			cr=cg-cu-(cv/2);
			cb=cg-cu+(cv/2);

			cr=(cr+8)>>4;	cg=(cg+8)>>4;
			cb=(cb+8)>>4;	ca=(cb+8)>>4;
			
//			if((cr|cg|cb|ca)&(~255))
			{
				cr=(cr<0)?0:((cr>255)?255:cr);
				cg=(cg<0)?0:((cg>255)?255:cg);
				cb=(cb<0)?0:((cb>255)?255:cb);
				ca=(ca<0)?0:((ca>255)?255:ca);
			}
#endif

			rgba_b[k+0]=cr;	rgba_b[k+1]=cg;
			rgba_b[k+2]=cb; rgba_b[k+3]=ca;
		}
	}else if(clrs==BGBBTJ_JPG_BGRA)
	{
		rgba_b=rgba;
		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
		{
			k=(i*ystride)+(j*xstride);
			l=i*4+j;
			cy=pxy[l];	cu=pxu[l];
			cv=pxv[l];	ca=pxa[l];

			ct0=(cu>>1);
			ct1=(cv>>1);
			cg=cy+ct0;
			cr=cg-cu-ct1;
			cb=cg-cu+ct1;
			cr=cr>>4;	cg=cg>>4;
			cb=cb>>4;	ca=cb>>4;

#if 0
			cg=cy+(cu/2);
			cr=cg-cu-(cv/2);
			cb=cg-cu+(cv/2);

			cr=(cr+8)>>4;
			cg=(cg+8)>>4;
			cb=(cb+8)>>4;
			ca=(ca+8)>>4;
			
//			if((cr|cg|cb|ca)&(~255))
			{
				cr=(cr<0)?0:((cr>255)?255:cr);
				cg=(cg<0)?0:((cg>255)?255:cg);
				cb=(cb<0)?0:((cb>255)?255:cb);
				ca=(ca<0)?0:((ca>255)?255:ca);
			}
#endif

			rgba_b[k+2]=cr;	rgba_b[k+1]=cg;
			rgba_b[k+0]=cb; rgba_b[k+3]=ca;
		}
	}
#endif
}

void BGBBTJ_BT1D_DecodeBlockBX1AI_PlanesToRGBA420(
	s32 *pxy, s32 *pxu, s32 *pxv, s32 *pxa,
	void *rgba, int xstride, int ystride, int clrs)
{
	byte *rgba_b;
	s16 *rgba_s;

	int cy, cu, cv, cr, cg, cb, ca;
	int cy0, cy1, cy2, cy3;
	int k0, k1, k2, k3;
	int l0, l1, l2, l3, l4;
	int cr0, cg0, cb0;
	int cr1, cg1, cb1;
	int cr2, cg2, cb2;
	int cr3, cg3, cb3;
	int i, j, k, l;

#if 1
	if(clrs==BGBBTJ_JPG_RGBA)
	{
		rgba_b=rgba;
		for(i=0; i<2; i++)
			for(j=0; j<2; j++)
		{
			k0=((i*2+0)*ystride)+((j*2+0)*xstride);
			k1=((i*2+0)*ystride)+((j*2+1)*xstride);
			k2=((i*2+1)*ystride)+((j*2+0)*xstride);
			k3=((i*2+1)*ystride)+((j*2+1)*xstride);
//			l=i*4+j;
			l0=(i*2+0)*4+(j*2+0);
			l1=(i*2+0)*4+(j*2+0);
			l2=(i*2+0)*4+(j*2+0);
			l3=(i*2+0)*4+(j*2+0);
			l4=i*2+j;

			cy0=pxy[l0];	cy1=pxy[l1];
			cy2=pxy[l2];	cy3=pxy[l3];
			cu=pxu[l4];		cv=pxv[l4];
			ca=pxa[l4];
//			cv=pxv[l];	ca=pxa[l];

			k=(cu/2);
			cg0=cy0+k; 	cg1=cy1+k;
			cg2=cy2+k;	cg3=cy3+k;
			l=cu+(cv/2);
			cr0=cg0-l;	cr1=cg1-l;
			cr2=cg2-l;	cr3=cg3-l;
			l=cu-(cv/2);
			cb0=cg0-l;	cb1=cg1-l;
			cb2=cg2-l;	cb3=cg3-l;

			cr0=(cr0+8)>>4;	cr1=(cr1+8)>>4;
			cr2=(cr2+8)>>4;	cr3=(cr3+8)>>4;
			cg0=(cg0+8)>>4;	cg1=(cg1+8)>>4;
			cg2=(cg2+8)>>4;	cg3=(cg3+8)>>4;
			cb0=(cb0+8)>>4;	cb1=(cb1+8)>>4;
			cb2=(cb2+8)>>4;	cb3=(cb3+8)>>4;
			ca=(ca+8)>>4;

			if((cr0|cr1|cr2|cr3)&(~255))
			{	cr0=(cr0<0)?0:((cr0>255)?255:cr0);
				cr1=(cr1<0)?0:((cr1>255)?255:cr1);
				cr2=(cr2<0)?0:((cr2>255)?255:cr2);
				cr3=(cr3<0)?0:((cr3>255)?255:cr3);	}
			if((cg0|cg1|cg2|cg3)&(~255))
			{	cg0=(cg0<0)?0:((cg0>255)?255:cg0);
				cg1=(cg1<0)?0:((cg1>255)?255:cg1);
				cg2=(cg2<0)?0:((cg2>255)?255:cg2);
				cg3=(cg3<0)?0:((cg3>255)?255:cg3);	}
			if((cb0|cb1|cb2|cb3)&(~255))
			{	cb0=(cb0<0)?0:((cb0>255)?255:cb0);
				cb1=(cb1<0)?0:((cb1>255)?255:cb1);
				cb2=(cb2<0)?0:((cb2>255)?255:cb2);
				cb3=(cb3<0)?0:((cb3>255)?255:cb3);	}

			rgba_b[k0+0]=cr0; rgba_b[k0+1]=cg0; rgba_b[k0+2]=cb0;
			rgba_b[k1+0]=cr1; rgba_b[k1+1]=cg1; rgba_b[k1+2]=cb1;
			rgba_b[k2+0]=cr2; rgba_b[k2+1]=cg2; rgba_b[k2+2]=cb2;
			rgba_b[k3+0]=cr3; rgba_b[k3+1]=cg3; rgba_b[k3+2]=cb3;

			ca=(ca<0)?0:((ca>255)?255:ca);
			rgba_b[k0+3]=ca;	rgba_b[k1+3]=ca;
			rgba_b[k2+3]=ca;	rgba_b[k3+3]=ca;
		}
	}else if(clrs==BGBBTJ_JPG_BGRA)
	{
		rgba_b=rgba;
		for(i=0; i<2; i++)
			for(j=0; j<2; j++)
		{
			k0=((i*2+0)*ystride)+((j*2+0)*xstride);
			k1=((i*2+0)*ystride)+((j*2+1)*xstride);
			k2=((i*2+1)*ystride)+((j*2+0)*xstride);
			k3=((i*2+1)*ystride)+((j*2+1)*xstride);
//			l=i*4+j;
			l0=(i*2+0)*4+(j*2+0);
			l1=(i*2+0)*4+(j*2+0);
			l2=(i*2+0)*4+(j*2+0);
			l3=(i*2+0)*4+(j*2+0);
			l4=i*2+j;

			cy0=pxy[l0];	cy1=pxy[l1];
			cy2=pxy[l2];	cy3=pxy[l3];
			cu=pxu[l4];		cv=pxv[l4];
			ca=pxa[l4];
//			cv=pxv[l];	ca=pxa[l];

			k=(cu/2);
			cg0=cy0+k; 	cg1=cy1+k;
			cg2=cy2+k;	cg3=cy3+k;
			l=cu+(cv/2);
			cr0=cg0-l;	cr1=cg1-l;
			cr2=cg2-l;	cr3=cg3-l;
			l=cu-(cv/2);
			cb0=cg0-l;	cb1=cg1-l;
			cb2=cg2-l;	cb3=cg3-l;

			cr0=(cr0+8)>>4;	cr1=(cr1+8)>>4;
			cr2=(cr2+8)>>4;	cr3=(cr3+8)>>4;
			cg0=(cg0+8)>>4;	cg1=(cg1+8)>>4;
			cg2=(cg2+8)>>4;	cg3=(cg3+8)>>4;
			cb0=(cb0+8)>>4;	cb1=(cb1+8)>>4;
			cb2=(cb2+8)>>4;	cb3=(cb3+8)>>4;
			ca=(ca+8)>>4;

			if((cr0|cr1|cr2|cr3)&(~255))
			{	cr0=(cr0<0)?0:((cr0>255)?255:cr0);
				cr1=(cr1<0)?0:((cr1>255)?255:cr1);
				cr2=(cr2<0)?0:((cr2>255)?255:cr2);
				cr3=(cr3<0)?0:((cr3>255)?255:cr3);	}
			if((cg0|cg1|cg2|cg3)&(~255))
			{	cg0=(cg0<0)?0:((cg0>255)?255:cg0);
				cg1=(cg1<0)?0:((cg1>255)?255:cg1);
				cg2=(cg2<0)?0:((cg2>255)?255:cg2);
				cg3=(cg3<0)?0:((cg3>255)?255:cg3);	}
			if((cb0|cb1|cb2|cb3)&(~255))
			{	cb0=(cb0<0)?0:((cb0>255)?255:cb0);
				cb1=(cb1<0)?0:((cb1>255)?255:cb1);
				cb2=(cb2<0)?0:((cb2>255)?255:cb2);
				cb3=(cb3<0)?0:((cb3>255)?255:cb3);	}

			rgba_b[k0+0]=cb0; rgba_b[k0+1]=cg0; rgba_b[k0+2]=cr0;
			rgba_b[k1+0]=cb1; rgba_b[k1+1]=cg1; rgba_b[k1+2]=cr1;
			rgba_b[k2+0]=cb2; rgba_b[k2+1]=cg2; rgba_b[k2+2]=cr2;
			rgba_b[k3+0]=cb3; rgba_b[k3+1]=cg3; rgba_b[k3+2]=cr3;

			ca=(ca<0)?0:((ca>255)?255:ca);
			rgba_b[k0+3]=ca;	rgba_b[k1+3]=ca;
			rgba_b[k2+3]=ca;	rgba_b[k3+3]=ca;
		}
	}
#endif
}

void BGBBTJ_BT1D_DecodeBlockBX1AI_FlatToRGBA(
	s32 cy, s32 cu, s32 cv, s32 ca,
	void *rgba, int xstride, int ystride, int clrs)
{
	byte *rgba_b;
	s16 *rgba_s;
	s32 *rgba_d;
	s32 cr, cg, cb;
	s32 ctv;
	int i, j, k, l;

 	cg=cy+(cu/2);
	cr=cg-cu-(cv/2);
	cb=cg-cu+(cv/2);

	switch(clrs)
	{
	case BGBBTJ_JPG_RGBA:
		cr=(cr+8)>>4;	cg=(cg+8)>>4;
		cb=(cb+8)>>4;	ca=(cb+8)>>4;
		cr=(cr<0)?0:((cr>255)?255:cr);
		cg=(cg<0)?0:((cg>255)?255:cg);
		cb=(cb<0)?0:((cb>255)?255:cb);
		ca=(ca<0)?0:((ca>255)?255:ca);

#ifdef X86
		rgba_d=rgba;
		ctv=cr|(cg<<8)|(cb<<16)|(ca<<24);
		for(i=0; i<4; i++)
		{
			k=(i*ystride)>>2;
			rgba_d[k+0]=ctv;	rgba_d[k+1]=ctv;
			rgba_d[k+2]=ctv;	rgba_d[k+3]=ctv;
		}
#else
		rgba_b=rgba;
		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
		{
			k=(i*ystride)+(j*xstride);
			rgba_b[k+0]=cr;	rgba_b[k+1]=cg;
			rgba_b[k+2]=cb; rgba_b[k+3]=ca;
		}
#endif
		break;
	case BGBBTJ_JPG_RGB:
		cr=(cr+8)>>4;	cg=(cg+8)>>4;
		cb=(cb+8)>>4;
		cr=(cr<0)?0:((cr>255)?255:cr);
		cg=(cg<0)?0:((cg>255)?255:cg);
		cb=(cb<0)?0:((cb>255)?255:cb);

		rgba_b=rgba;
		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
		{
			k=(i*ystride)+(j*xstride);
			rgba_b[k+0]=cr;	rgba_b[k+1]=cg;
			rgba_b[k+2]=cb;
		}
		break;
	case BGBBTJ_JPG_BGRA:
		cr=(cr+8)>>4;	cg=(cg+8)>>4;
		cb=(cb+8)>>4;	ca=(ca+8)>>4;
		cr=(cr<0)?0:((cr>255)?255:cr);
		cg=(cg<0)?0:((cg>255)?255:cg);
		cb=(cb<0)?0:((cb>255)?255:cb);
		ca=(ca<0)?0:((ca>255)?255:ca);
	
		rgba_b=rgba;
		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
		{
			k=(i*ystride)+(j*xstride);
			rgba_b[k+0]=cb;	rgba_b[k+1]=cg;
			rgba_b[k+2]=cr; rgba_b[k+3]=ca;
		}
		break;
	case BGBBTJ_JPG_BGR:
		cr=(cr+8)>>4;	cg=(cg+8)>>4;	cb=(cb+8)>>4;
		cr=(cr<0)?0:((cr>255)?255:cr);
		cg=(cg<0)?0:((cg>255)?255:cg);
		cb=(cb<0)?0:((cb>255)?255:cb);
	
		rgba_b=rgba;
		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
		{
			k=(i*ystride)+(j*xstride);
			rgba_b[k+0]=cb;	rgba_b[k+1]=cg; rgba_b[k+2]=cr;
		}
		break;
	case BGBBTJ_JPG_RGBA_Q11_4:
	case BGBBTJ_JPG_RGB_Q11_4:
		rgba_s=rgba;
		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
		{
			k=(i*ystride)+(j*xstride);
			rgba_s[k+0]=cr;	rgba_s[k+1]=cg;
			rgba_s[k+2]=cb;
			if(clrs==BGBBTJ_JPG_RGBA)rgba_s[k+3]=ca;
		}
		break;
	default:
		break;
	}

#if 0
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		k=(i*ystride)+(j*xstride);
		rgba[k+0]=cr;	rgba[k+1]=cg;
		rgba[k+2]=cb;	rgba[k+3]=ca;
	}
#endif
}

void BGBBTJ_BT1D_DecodeBlockBX1AI(byte *block,
	void *rgba, int xstride, int ystride, int clrs)
{
	s32 pty[8], ptu[8], ptv[8], pta[8];
	s32 pxy[16], pxu[16], pxv[16], pxa[16];
	s32 pxu1[4], pxv1[4], pxa1[4];
	s32 cy, cu, cv, cr, cg, cb, ca;
	float f, g, h;
	int myi, mui, mvi, nyi, nui, nvi, nai;
	int bm, emi, eni;
	int i0, i1, i2, i3;
	int j0, j1, j2, j3;
	int p0, p1, p2, p3;
	int i, j, k, l;
	
//	bm=((block[0]>>5)&0x07)|((block[4]>>2)&0x38);
	bm=((block[0]>>2)&0x38)|((block[4]>>5)&0x07);

//	if((bm>=32) && (bm<48))
//		{ return; }

	if(bm>=32)
	{
		/* Skip Blocks */
		if(bm<48)
			{ return; }
		if(block[0]&0x10)
			{ return; }
	}

	i0=(block[0]<<24)|(block[1]<<16)|(block[2]<<8)|(block[3]);
	i1=(block[4]<<24)|(block[5]<<16)|(block[6]<<8)|(block[7]);
//	i2=(block[8]<<24)|(block[9]<<16)|(block[10]<<8)|(block[11]);
//	i3=(block[12]<<24)|(block[13]<<16)|(block[14]<<8)|(block[15]);
	j0=(block[ 8]<<8)|(block[ 9]);	j1=(block[10]<<8)|(block[11]);
	j2=(block[12]<<8)|(block[13]);	j3=(block[14]<<8)|(block[15]);
	
#if 0
	if((bm>=32) && (bm<48))
	{
		/* Skip Block */
	}else
#endif
	if(bm==0)
	{
		myi=(i0>>18)&2047;	mui=(i0>>9)&511;	mvi=(i0>>0)&511;
		nyi=(i1>>18)&2047;	nui=(i1>>9)&511;	nvi=(i1>>0)&511;
		
		pty[0]=(4082*myi+1024)>>11; pty[7]=(4082*nyi+1024)>>11;
		pty[1]=((219*pty[0])+( 37*pty[7])+128)>>8;
		pty[2]=((183*pty[0])+( 73*pty[7])+128)>>8;
		pty[3]=((146*pty[0])+(110*pty[7])+128)>>8;
		pty[4]=((110*pty[0])+(146*pty[7])+128)>>8;
		pty[5]=(( 73*pty[0])+(183*pty[7])+128)>>8;
		pty[6]=(( 37*pty[0])+(219*pty[7])+128)>>8;

		ptu[0]=(4096*(mui-256)+128)>>8; ptu[3]=(4096*(nui-256)+128)>>8;
		ptv[0]=(4096*(mvi-256)+128)>>8; ptv[3]=(4096*(nvi-256)+128)>>8;
		ptu[1]=((11*ptu[0])+( 5*ptu[3])+8)>>4;
		ptu[2]=(( 5*ptu[0])+(11*ptu[3])+8)>>4;
		ptv[1]=((11*ptv[0])+( 5*ptv[3])+8)>>4;
		ptv[2]=(( 5*ptv[0])+(11*ptv[3])+8)>>4;
		
		pxy[ 0]=pty[(j0>>13)&7];	pxy[ 1]=pty[(j0>>10)&7];
		pxy[ 4]=pty[(j0>> 7)&7];	pxy[ 5]=pty[(j0>> 4)&7];
		pxy[ 2]=pty[(j1>>13)&7];	pxy[ 3]=pty[(j1>>10)&7];
		pxy[ 6]=pty[(j1>> 7)&7];	pxy[ 7]=pty[(j1>> 4)&7];
		pxy[ 8]=pty[(j2>>13)&7];	pxy[ 9]=pty[(j2>>10)&7];
		pxy[12]=pty[(j2>> 7)&7];	pxy[13]=pty[(j2>> 4)&7];
		pxy[10]=pty[(j3>>13)&7];	pxy[11]=pty[(j3>>10)&7];
		pxy[14]=pty[(j3>> 7)&7];	pxy[15]=pty[(j3>> 4)&7];
		pxu1[0]=ptu[(j0>> 2)&3];	pxv1[0]=ptv[(j0    )&3];
		pxu1[1]=ptu[(j1>> 2)&3];	pxv1[1]=ptv[(j1    )&3];
		pxu1[2]=ptu[(j2>> 2)&3];	pxv1[2]=ptv[(j2    )&3];
		pxu1[3]=ptu[(j3>> 2)&3];	pxv1[3]=ptv[(j3    )&3];
		pxa1[0]=4080;	pxa1[1]=4080;
		pxa1[2]=4080;	pxa1[3]=4080;

//		pxu[ 0]=pxu[ 1]=pxu[ 4]=pxu[ 5]=pxu1[0];
//		pxv[ 0]=pxv[ 1]=pxv[ 4]=pxv[ 5]=pxv1[0];
//		pxu[ 2]=pxu[ 3]=pxu[ 6]=pxu[ 7]=pxu1[1];
//		pxv[ 2]=pxv[ 3]=pxv[ 6]=pxv[ 7]=pxv1[1];
//		pxu[ 8]=pxu[ 9]=pxu[12]=pxu[13]=pxu1[2];
//		pxv[ 8]=pxv[ 9]=pxv[12]=pxv[13]=pxv1[2];
//		pxu[10]=pxu[11]=pxu[14]=pxu[15]=pxu1[3];
//		pxv[10]=pxv[11]=pxv[14]=pxv[15]=pxv1[3];
//		pxa[ 0]=pxa[ 1]=pxa[ 4]=pxa[ 5]=4080;
//		pxa[ 2]=pxa[ 3]=pxa[ 6]=pxa[ 7]=4080;
//		pxa[ 8]=pxa[ 9]=pxa[12]=pxa[13]=4080;
//		pxa[10]=pxa[11]=pxa[14]=pxa[15]=4080;

		BGBBTJ_BT1D_DecodeBlockBX1AI_PlanesToRGBA420(
			pxy, pxu1, pxv1, pxa1, rgba, xstride, ystride, clrs);
		
//		BGBBTJ_BT1D_DecodeBlockBX1AI_PlanesToRGBA(
//			pxy, pxu, pxv, pxa, rgba, xstride, ystride);
	}else if(bm==1)
	{
		myi=(i0>>16)&511;	mui=(i0>>8)&255;	mvi=(i0>>0)&255;
		nyi=(i1>>16)&511;	nui=(i1>>8)&255;	nvi=(i1>>0)&255;
		emi=(i0>>25)&15;	eni=(i1>>25)&15;
//		f=pow(2.0, emi-7);
//		g=pow(2.0, eni-7);
		
		pty[0]=(((4096*(myi-256)+128)>>8)<<emi)>>8;
		pty[7]=(((4096*(nyi-256)+128)>>8)<<eni)>>8;
		pty[1]=((219*pty[0])+( 37*pty[7])+128)>>8;
		pty[2]=((183*pty[0])+( 73*pty[7])+128)>>8;
		pty[3]=((146*pty[0])+(110*pty[7])+128)>>8;
		pty[4]=((110*pty[0])+(146*pty[7])+128)>>8;
		pty[5]=(( 73*pty[0])+(183*pty[7])+128)>>8;
		pty[6]=(( 37*pty[0])+(219*pty[7])+128)>>8;

		ptu[0]=(((4112*(mui-128)+64)>>7)<<emi)>>8;
		ptu[3]=(((4112*(nui-128)+64)>>7)<<eni)>>8;
		ptv[0]=(((4112*(mvi-128)+64)>>7)<<emi)>>8;
		ptv[3]=(((4112*(nvi-128)+64)>>7)<<eni)>>8;

//		ptu[0]=4096*(mui-128)/127.0*f;
//		ptu[3]=4096*(nui-128)/127.0*g;
//		ptv[0]=4096*(mvi-128)/127.0*f;
//		ptv[3]=4096*(nvi-128)/127.0*g;
		ptu[1]=((11*ptu[0])+( 5*ptu[3])+8)>>4;
		ptu[2]=(( 5*ptu[0])+(11*ptu[3])+8)>>4;
		ptv[1]=((11*ptv[0])+( 5*ptv[3])+8)>>4;
		ptv[2]=(( 5*ptv[0])+(11*ptv[3])+8)>>4;
		
		pxy[ 0]=pty[(j0>>13)&7];	pxy[ 1]=pty[(j0>>10)&7];
		pxy[ 4]=pty[(j0>> 7)&7];	pxy[ 5]=pty[(j0>> 4)&7];
		pxy[ 2]=pty[(j1>>13)&7];	pxy[ 3]=pty[(j1>>10)&7];
		pxy[ 6]=pty[(j1>> 7)&7];	pxy[ 7]=pty[(j1>> 4)&7];
		pxy[ 8]=pty[(j2>>13)&7];	pxy[ 9]=pty[(j2>>10)&7];
		pxy[12]=pty[(j2>> 7)&7];	pxy[13]=pty[(j2>> 4)&7];
		pxy[10]=pty[(j3>>13)&7];	pxy[11]=pty[(j3>>10)&7];
		pxy[14]=pty[(j3>> 7)&7];	pxy[15]=pty[(j3>> 4)&7];
		pxu1[0]=ptu[(j0>> 2)&3];	pxv1[0]=ptv[(j0    )&3];
		pxu1[1]=ptu[(j1>> 2)&3];	pxv1[1]=ptv[(j1    )&3];
		pxu1[2]=ptu[(j2>> 2)&3];	pxv1[2]=ptv[(j2    )&3];
		pxu1[3]=ptu[(j3>> 2)&3];	pxv1[3]=ptv[(j3    )&3];
		pxa1[0]=4080;	pxa1[1]=4080;
		pxa1[2]=4080;	pxa1[3]=4080;
		
//		pxu[ 0]=pxu[ 1]=pxu[ 4]=pxu[ 5]=pxu1[0];
//		pxv[ 0]=pxv[ 1]=pxv[ 4]=pxv[ 5]=pxv1[0];
//		pxu[ 2]=pxu[ 3]=pxu[ 6]=pxu[ 7]=pxu1[1];
//		pxv[ 2]=pxv[ 3]=pxv[ 6]=pxv[ 7]=pxv1[1];
//		pxu[ 8]=pxu[ 9]=pxu[12]=pxu[13]=pxu1[2];
//		pxv[ 8]=pxv[ 9]=pxv[12]=pxv[13]=pxv1[2];
//		pxu[10]=pxu[11]=pxu[14]=pxu[15]=pxu1[3];
//		pxv[10]=pxv[11]=pxv[14]=pxv[15]=pxv1[3];
//		pxa[ 0]=pxa[ 1]=pxa[ 4]=pxa[ 5]=4080;
//		pxa[ 2]=pxa[ 3]=pxa[ 6]=pxa[ 7]=4080;
//		pxa[ 8]=pxa[ 9]=pxa[12]=pxa[13]=4080;
//		pxa[10]=pxa[11]=pxa[14]=pxa[15]=4080;

		BGBBTJ_BT1D_DecodeBlockBX1AI_PlanesToRGBA420(
			pxy, pxu1, pxv1, pxa1, rgba, xstride, ystride, clrs);

//		BGBBTJ_BT1D_DecodeBlockBX1AI_PlanesToRGBA(
//			pxy, pxu, pxv, pxa, rgba, xstride, ystride);
	}else if(bm==2)
	{
		myi=(i0>>18)&2047;	mui=(i0>>9)&511;	mvi=(i0>>0)&511;
		nyi=(i1>>18)&2047;	nui=(i1>>9)&511;	nvi=(i1>>0)&511;
		
		pty[0]=(4082*myi+1024)>>11;
		pty[7]=(4082*nyi+1024)>>11;
		pty[1]=((219*pty[0])+( 37*pty[7])+128)>>8;
		pty[2]=((183*pty[0])+( 73*pty[7])+128)>>8;
		pty[3]=((146*pty[0])+(110*pty[7])+128)>>8;
		pty[4]=((110*pty[0])+(146*pty[7])+128)>>8;
		pty[5]=(( 73*pty[0])+(183*pty[7])+128)>>8;
		pty[6]=(( 37*pty[0])+(219*pty[7])+128)>>8;

		ptu[0]=(4096*(mui-256)+128)>>8;
		ptu[3]=(4096*(nui-256)+128)>>8;
		ptv[0]=(4096*(mvi-256)+128)>>8;
		ptv[3]=(4096*(nvi-256)+128)>>8;
		ptu[1]=((219*ptu[0])+( 37*ptu[7])+128)>>8;
		ptu[2]=((183*ptu[0])+( 73*ptu[7])+128)>>8;
		ptu[3]=((146*ptu[0])+(110*ptu[7])+128)>>8;
		ptu[4]=((110*ptu[0])+(146*ptu[7])+128)>>8;
		ptu[5]=(( 73*ptu[0])+(183*ptu[7])+128)>>8;
		ptu[6]=(( 37*ptu[0])+(219*ptu[7])+128)>>8;
		ptv[1]=((219*ptv[0])+( 37*ptv[7])+128)>>8;
		ptv[2]=((183*ptv[0])+( 73*ptv[7])+128)>>8;
		ptv[3]=((146*ptv[0])+(110*ptv[7])+128)>>8;
		ptv[4]=((110*ptv[0])+(146*ptv[7])+128)>>8;
		ptv[5]=(( 73*ptv[0])+(183*ptv[7])+128)>>8;
		ptv[6]=(( 37*ptv[0])+(219*ptv[7])+128)>>8;
		
		p0=(j0>>13)&7;	p1=(j0>>10)&7;	p2=(j0>> 7)&7;	p3=(j0>> 4)&7;
		pxy[ 0]=pty[p0]; pxy[ 1]=pty[p1]; pxy[ 4]=pty[p2]; pxy[ 5]=pty[p3];
		pxu[ 0]=ptu[p0]; pxu[ 1]=ptu[p1]; pxu[ 4]=ptu[p2]; pxu[ 5]=ptu[p3];
		pxv[ 0]=ptv[p0]; pxv[ 1]=ptv[p1]; pxv[ 4]=ptv[p2]; pxv[ 5]=ptv[p3];
		p0=(j1>>13)&7;	p1=(j1>>10)&7;	p2=(j1>> 7)&7;	p3=(j1>> 4)&7;
		pxy[ 2]=pty[p0]; pxy[ 3]=pty[p1]; pxy[ 6]=pty[p2]; pxy[ 7]=pty[p3];
		pxu[ 2]=ptu[p0]; pxu[ 3]=ptu[p1]; pxu[ 6]=ptu[p2]; pxu[ 7]=ptu[p3];
		pxv[ 2]=ptv[p0]; pxv[ 3]=ptv[p1]; pxv[ 6]=ptv[p2]; pxv[ 7]=ptv[p3];
		p0=(j2>>13)&7;	p1=(j2>>10)&7;	p2=(j2>> 7)&7;	p3=(j2>> 4)&7;
		pxy[ 8]=pty[p0]; pxy[ 9]=pty[p1]; pxy[12]=pty[p2]; pxy[13]=pty[p3];
		pxu[ 8]=ptu[p0]; pxu[ 9]=ptu[p1]; pxu[12]=ptu[p2]; pxu[13]=ptu[p3];
		pxv[ 8]=ptv[p0]; pxv[ 9]=ptv[p1]; pxv[12]=ptv[p2]; pxv[13]=ptv[p3];
		p0=(j3>>13)&7;	p1=(j3>>10)&7;	p2=(j3>> 7)&7;	p3=(j3>> 4)&7;
		pxy[10]=pty[p0]; pxy[11]=pty[p1]; pxy[14]=pty[p2]; pxy[15]=pty[p3];
		pxu[10]=ptu[p0]; pxu[11]=ptu[p1]; pxu[14]=ptu[p2]; pxu[15]=ptu[p3];
		pxv[10]=ptv[p0]; pxv[11]=ptv[p1]; pxv[14]=ptv[p2]; pxv[15]=ptv[p3];

		pxa1[0]=(4352*(j0&15)+8)>>4;		pxa1[1]=(4352*(j1&15)+8)>>4;
		pxa1[2]=(4352*(j2&15)+8)>>4;		pxa1[3]=(4352*(j3&15)+8)>>4;
		pxa[ 0]=pxa[ 1]=pxa[ 4]=pxa[ 5]=pxa1[0];
		pxa[ 2]=pxa[ 3]=pxa[ 6]=pxa[ 7]=pxa1[1];
		pxa[ 8]=pxa[ 9]=pxa[12]=pxa[13]=pxa1[2];
		pxa[10]=pxa[11]=pxa[14]=pxa[15]=pxa1[3];

		BGBBTJ_BT1D_DecodeBlockBX1AI_PlanesToRGBA(
			pxy, pxu, pxv, pxa, rgba, xstride, ystride, clrs);
	}else if(bm==3)
	{
		myi=(i0>>16)&511;	mui=(i0>>8)&255;	mvi=(i0>>0)&255;
		nyi=(i1>>16)&511;	nui=(i1>>8)&255;	nvi=(i1>>0)&255;
		emi=(i0>>25)&15;	eni=(i1>>25)&15;
//		f=pow(2.0, emi-7);
//		g=pow(2.0, eni-7);
		
		pty[0]=(((4096*(myi-256)+128)>>8)<<emi)>>8;
		pty[7]=(((4096*(nyi-256)+128)>>8)<<eni)>>8;
//		pty[0]=((myi-256)/255.0)*f; pty[7]=((nyi-256)/255.0)*g;
		pty[1]=((219*pty[0])+( 37*pty[7])+128)>>8;
		pty[2]=((183*pty[0])+( 73*pty[7])+128)>>8;
		pty[3]=((146*pty[0])+(110*pty[7])+128)>>8;
		pty[4]=((110*pty[0])+(146*pty[7])+128)>>8;
		pty[5]=(( 73*pty[0])+(183*pty[7])+128)>>8;
		pty[6]=(( 37*pty[0])+(219*pty[7])+128)>>8;

		ptu[0]=(((4112*(mui-128)+64)>>7)<<emi)>>8;
		ptu[3]=(((4112*(nui-128)+64)>>7)<<eni)>>8;
		ptv[0]=(((4112*(mvi-128)+64)>>7)<<emi)>>8;
		ptv[3]=(((4112*(nvi-128)+64)>>7)<<eni)>>8;
//		ptu[0]=((mui-128)/127.0)*f; ptu[7]=((nui-128)/127.0)*g;
//		ptv[0]=((mvi-128)/127.0)*f; ptv[7]=((nvi-128)/127.0)*g;
		ptu[1]=((219*ptu[0])+( 37*ptu[7])+128)>>8;
		ptu[2]=((183*ptu[0])+( 73*ptu[7])+128)>>8;
		ptu[3]=((146*ptu[0])+(110*ptu[7])+128)>>8;
		ptu[4]=((110*ptu[0])+(146*ptu[7])+128)>>8;
		ptu[5]=(( 73*ptu[0])+(183*ptu[7])+128)>>8;
		ptu[6]=(( 37*ptu[0])+(219*ptu[7])+128)>>8;
		ptv[1]=((219*ptv[0])+( 37*ptv[7])+128)>>8;
		ptv[2]=((183*ptv[0])+( 73*ptv[7])+128)>>8;
		ptv[3]=((146*ptv[0])+(110*ptv[7])+128)>>8;
		ptv[4]=((110*ptv[0])+(146*ptv[7])+128)>>8;
		ptv[5]=(( 73*ptv[0])+(183*ptv[7])+128)>>8;
		ptv[6]=(( 37*ptv[0])+(219*ptv[7])+128)>>8;
		
		p0=(j0>>13)&7;	p1=(j0>>10)&7;	p2=(j0>> 7)&7;	p3=(j0>> 4)&7;
		pxy[ 0]=pty[p0]; pxy[ 1]=pty[p1]; pxy[ 4]=pty[p2]; pxy[ 5]=pty[p3];
		pxu[ 0]=ptu[p0]; pxu[ 1]=ptu[p1]; pxu[ 4]=ptu[p2]; pxu[ 5]=ptu[p3];
		pxv[ 0]=ptv[p0]; pxv[ 1]=ptv[p1]; pxv[ 4]=ptv[p2]; pxv[ 5]=ptv[p3];
		p0=(j1>>13)&7;	p1=(j1>>10)&7;	p2=(j1>> 7)&7;	p3=(j1>> 4)&7;
		pxy[ 2]=pty[p0]; pxy[ 3]=pty[p1]; pxy[ 6]=pty[p2]; pxy[ 7]=pty[p3];
		pxu[ 2]=ptu[p0]; pxu[ 3]=ptu[p1]; pxu[ 6]=ptu[p2]; pxu[ 7]=ptu[p3];
		pxv[ 2]=ptv[p0]; pxv[ 3]=ptv[p1]; pxv[ 6]=ptv[p2]; pxv[ 7]=ptv[p3];
		p0=(j2>>13)&7;	p1=(j2>>10)&7;	p2=(j2>> 7)&7;	p3=(j2>> 4)&7;
		pxy[ 8]=pty[p0]; pxy[ 9]=pty[p1]; pxy[12]=pty[p2]; pxy[13]=pty[p3];
		pxu[ 8]=ptu[p0]; pxu[ 9]=ptu[p1]; pxu[12]=ptu[p2]; pxu[13]=ptu[p3];
		pxv[ 8]=ptv[p0]; pxv[ 9]=ptv[p1]; pxv[12]=ptv[p2]; pxv[13]=ptv[p3];
		p0=(j3>>13)&7;	p1=(j3>>10)&7;	p2=(j3>> 7)&7;	p3=(j3>> 4)&7;
		pxy[10]=pty[p0]; pxy[11]=pty[p1]; pxy[14]=pty[p2]; pxy[15]=pty[p3];
		pxu[10]=ptu[p0]; pxu[11]=ptu[p1]; pxu[14]=ptu[p2]; pxu[15]=ptu[p3];
		pxv[10]=ptv[p0]; pxv[11]=ptv[p1]; pxv[14]=ptv[p2]; pxv[15]=ptv[p3];

		pxa1[0]=(4352*(j0&15)+8)>>4;		pxa1[1]=(4352*(j1&15)+8)>>4;
		pxa1[2]=(4352*(j2&15)+8)>>4;		pxa1[3]=(4352*(j3&15)+8)>>4;
		pxa[ 0]=pxa[ 1]=pxa[ 4]=pxa[ 5]=pxa1[0];
		pxa[ 2]=pxa[ 3]=pxa[ 6]=pxa[ 7]=pxa1[1];
		pxa[ 8]=pxa[ 9]=pxa[12]=pxa[13]=pxa1[2];
		pxa[10]=pxa[11]=pxa[14]=pxa[15]=pxa1[3];

		BGBBTJ_BT1D_DecodeBlockBX1AI_PlanesToRGBA(
			pxy, pxu, pxv, pxa, rgba, xstride, ystride, clrs);
	}else if((bm>=48) && (bm<56))
	{
		//Skip
		if(block[0]&0x10)
			return;

		//Flat Color
		if(bm==48)
		{
			//LDR Flat Color
			nyi=(i1>>18)&2047;	nui=(i1>>9)&511;	nvi=(i1>>0)&511;
//			cy=nyi/2047.0;
//			cu=(nui-256)/255.0;
//			cv=(nvi-256)/255.0;

			cy=(4082*nyi+1024)>>11;
			cu=(4096*(nui-256)+128)>>8;
			cv=(4096*(nvi-256)+128)>>8;

			BGBBTJ_BT1D_DecodeBlockBX1AI_FlatToRGBA(
				cy, cu, cv, 4080, rgba, xstride, ystride, clrs);
		}else if(bm==49)
		{
			//HDR Flat Color
			nyi=(i1>>16)&511;	nui=(i1>>8)&255;	nvi=(i1>>0)&255;
			eni=(i1>>25)&15;
//			f=pow(2.0, eni-7);

//			cy=f*((nyi-256)/255.0);
//			cu=f*((nui-128)/127.0);
//			cv=f*((nvi-128)/127.0);
			cy=(((4096*(nyi-256)+128)>>8)<<eni)>>8;
			cu=(((4112*(nui-128)+64)>>7)<<eni)>>8;
			cv=(((4112*(nvi-128)+64)>>7)<<eni)>>8;

			BGBBTJ_BT1D_DecodeBlockBX1AI_FlatToRGBA(
				cy, cu, cv, 4080, rgba, xstride, ystride, clrs);
		}else if(bm==50)
		{
			//LDR Flat Color + Alpha
			nai=(i1>>24)&31;
			nyi=(i1>>16)&255;	nui=(i1>>8)&255;	nvi=(i1>>0)&255;

//			cy=nyi/255.0;
//			cu=(nui-128)/127.0;
//			cv=(nvi-128)/127.0;
//			ca=nai/31.0;
			cy=(4096*nyi+128)>>8;
			cu=(4112*(nui-128)+64)>>7;
			cv=(4112*(nvi-128)+64)>>7;
			ca=(4212*nai+16)>>5;

			BGBBTJ_BT1D_DecodeBlockBX1AI_FlatToRGBA(
				cy, cu, cv, ca, rgba, xstride, ystride, clrs);
		}else if(bm==51)
		{
			//HDR Flat Color + Alpha
			nyi=(i1>>17)&255;	nui=(i1>>11)&63;	nvi=(i1>>5)&63;
			nai=(i1>>0)&31; eni=(i1>>25)&15;
//			f=pow(2.0, eni-7);

//			cy=f*((nyi-128)/127.0);
//			cu=f*((nui-32)/31.0);
//			cv=f*((nvi-32)/31.0);
//			ca=nai/31.0;

			cy=(4112*(nyi-128)+64)>>7;
			cu=(4212*(nui-128)+16)>>5;
			cv=(4212*(nvi-128)+16)>>5;
			ca=(4212*nai+16)>>5;
			cy=(cy<<eni)>>8;
			cu=(cu<<eni)>>8;
			cv=(cv<<eni)>>8;

			BGBBTJ_BT1D_DecodeBlockBX1AI_FlatToRGBA(
				cy, cu, cv, ca, rgba, xstride, ystride, clrs);
		}else
		{
			//Half-Width Flat Color
			nai=(i1>>30)&1;
			nyi=(i1>>24)&63;	nui=(i1>>20)&15;	nvi=(i1>>16)&15;
//			cy=nyi/63.0;
//			cu=(nui-8)/7.0;
//			cv=(nvi-8)/7.0;
			ca=nai?0.0:1.0;

			cy=(4145*nyi+32)>>6;
			cu=(4663*(nui-8)+4)>>3;
			cv=(4663*(nvi-8)+4)>>3;

			BGBBTJ_BT1D_DecodeBlockBX1AI_FlatToRGBA(
				cy, cu, cv, ca, rgba, xstride, ystride, clrs);
		}
	}else if(bm==63)
	{
		//LDR Half-Width
		
		myi=(i1>>22)&63;	mui=(i1>>12)&15;	mvi=(i1>>8)&15;
		nyi=(i1>>16)&63;	nui=(i1>> 4)&15;	nvi=(i1>>0)&15;
		nai=(i1>>28)&1;
		
		if(nai)
		{
			pty[0]=(4145*myi+32)>>6;
			ptu[0]=(4663*(mui-8)+4)>>3;
			ptv[0]=(4663*(mvi-8)+4)>>3;
			pty[2]=(4145*nyi+32)>>6;
			ptu[2]=(4663*(nui-8)+4)>>3;
			ptv[2]=(4663*(nvi-8)+4)>>3;

//			pty[0]=myi/63.0; pty[2]=nyi/63.0;
//			ptu[0]=(mui-8)/7.0; ptu[2]=(nui-8)/7.0;
//			ptv[0]=(mvi-8)/7.0; ptv[2]=(nvi-8)/7.0;
			pty[1]=(pty[0]+pty[2]+1)>>1;
			ptu[1]=(ptu[0]+ptu[2]+1)>>1;
			ptv[1]=(ptv[0]+ptv[2]+1)>>1;
			pta[0]=4080;	pta[1]=4080;	pta[2]=4080;
			pty[3]=pty[1];	ptu[3]=ptu[1];
			ptv[3]=ptv[1];	pta[3]=0;
		}else
		{
			pty[0]=(4145*myi+32)>>6;
			ptu[0]=(4663*(mui-8)+4)>>3;
			ptv[0]=(4663*(mvi-8)+4)>>3;
			pty[3]=(4145*nyi+32)>>6;
			ptu[3]=(4663*(nui-8)+4)>>3;
			ptv[3]=(4663*(nvi-8)+4)>>3;

//			pty[0]=myi/63.0; pty[3]=nyi/63.0;
//			ptu[0]=(mui-8)/7.0; ptu[3]=(nui-8)/7.0;
//			ptv[0]=(mvi-8)/7.0; ptv[3]=(nvi-8)/7.0;
			pty[1]=((11*pty[0])+( 5*pty[3])+8)>>4;
			pty[2]=(( 5*pty[0])+(11*pty[3])+8)>>4;
			ptu[1]=((11*ptu[0])+( 5*ptu[3])+8)>>4;
			ptu[2]=(( 5*ptu[0])+(11*ptu[3])+8)>>4;
			ptv[1]=((11*ptv[0])+( 5*ptv[3])+8)>>4;
			ptv[2]=(( 5*ptv[0])+(11*ptv[3])+8)>>4;
			pta[0]=4080;	pta[1]=4080;
			pta[2]=4080;	pta[3]=4080;
		}
		
		p0=(j0>>14)&3;	p1=(j0>>12)&3;	p2=(j0>>10)&3;	p3=(j0>> 8)&3;
		pxy[ 0]=pty[p0]; pxy[ 1]=pty[p1]; pxy[ 4]=pty[p2]; pxy[ 5]=pty[p3];
		pxu[ 0]=ptu[p0]; pxu[ 1]=ptu[p1]; pxu[ 4]=ptu[p2]; pxu[ 5]=ptu[p3];
		pxv[ 0]=ptv[p0]; pxv[ 1]=ptv[p1]; pxv[ 4]=ptv[p2]; pxv[ 5]=ptv[p3];
		pxa[ 0]=pta[p0]; pxa[ 1]=pta[p1]; pxa[ 4]=pta[p2]; pxa[ 5]=pta[p3];
		p0=(j0>> 6)&3;	p1=(j0>> 4)&3;	p2=(j0>> 2)&3;	p3=(j0>> 0)&3;
		pxy[ 2]=pty[p0]; pxy[ 3]=pty[p1]; pxy[ 6]=pty[p2]; pxy[ 7]=pty[p3];
		pxu[ 2]=ptu[p0]; pxu[ 3]=ptu[p1]; pxu[ 6]=ptu[p2]; pxu[ 7]=ptu[p3];
		pxv[ 2]=ptv[p0]; pxv[ 3]=ptv[p1]; pxv[ 6]=ptv[p2]; pxv[ 7]=ptv[p3];
		pxa[ 2]=pta[p0]; pxa[ 3]=pta[p1]; pxa[ 6]=pta[p2]; pxa[ 7]=pta[p3];
		p0=(j1>>14)&3;	p1=(j1>>12)&3;	p2=(j1>>10)&3;	p3=(j1>> 8)&3;
		pxy[ 8]=pty[p0]; pxy[ 9]=pty[p1]; pxy[12]=pty[p2]; pxy[13]=pty[p3];
		pxu[ 8]=ptu[p0]; pxu[ 9]=ptu[p1]; pxu[12]=ptu[p2]; pxu[13]=ptu[p3];
		pxv[ 8]=ptv[p0]; pxv[ 9]=ptv[p1]; pxv[12]=ptv[p2]; pxv[13]=ptv[p3];
		pxa[ 8]=pta[p0]; pxa[ 9]=pta[p1]; pxa[12]=pta[p2]; pxa[13]=pta[p3];
		p0=(j1>> 6)&3;	p1=(j1>> 4)&3;	p2=(j1>> 2)&3;	p3=(j1>> 0)&3;
		pxy[10]=pty[p0]; pxy[11]=pty[p1]; pxy[14]=pty[p2]; pxy[15]=pty[p3];
		pxu[10]=ptu[p0]; pxu[11]=ptu[p1]; pxu[14]=ptu[p2]; pxu[15]=ptu[p3];
		pxv[10]=ptv[p0]; pxv[11]=ptv[p1]; pxv[14]=ptv[p2]; pxv[15]=ptv[p3];
		pxa[10]=pta[p0]; pxa[11]=pta[p1]; pxa[14]=pta[p2]; pxa[15]=pta[p3];
		
		BGBBTJ_BT1D_DecodeBlockBX1AI_PlanesToRGBA(
			pxy, pxu, pxv, pxa, rgba, xstride, ystride, clrs);
	}
}


BGBBTJ_API void BGBBTJ_BT1D_DecodeImageBX1AI(byte *block,
	byte *rgba, int xs, int ys, int stride, int clrs)
{
//	float tblk[16*4];
	float bgr, bgg, bgb;
	float cr, cg, cb, ca;
	float f, g;
	int xs1, ys1;
	int i, j, k, l;
	int i0, i1, i2, i3;
	int l0, l1, stp;
	
//	clrs=(stride>3)?BGBBTJ_JPG_RGBA:BGBBTJ_JPG_RGB;
	switch(clrs)
	{
	case BGBBTJ_JPG_RGBA:
	case BGBBTJ_JPG_RGB:
		stp=1; break;
	case BGBBTJ_JPG_RGBA_Q11_4:
	case BGBBTJ_JPG_RGB_Q11_4:
		stp=2; break;
	case BGBBTJ_JPG_RGBA_F32:
	case BGBBTJ_JPG_RGB_F32:
		stp=4; break;
	default:
		stp=1; break;
	}
	
	xs1=xs>>2; ys1=ys>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		BGBBTJ_BT1D_DecodeBlockBX1AI(
			block+(i*xs1+j)*16,
			rgba+(i*4*xs+j*4)*stride*stp,
			stride, xs*stride,
			clrs);
	}
}

BGBBTJ_API void BGBBTJ_BT1D_DecodeImageBX1AI_RGBA32(byte *block,
	byte *rgba, int xs, int ys, int stride)
{
//	float tblk[16*4];
	float bgr, bgg, bgb;
	float cr, cg, cb, ca;
	float f, g;
	int xs1, ys1;
	int i, j, k, l;
	int i0, i1, i2, i3;
	int l0, l1, clrs;
	
	clrs=(stride>3)?BGBBTJ_JPG_RGBA:BGBBTJ_JPG_RGB;
	
	xs1=xs>>2; ys1=ys>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		BGBBTJ_BT1D_DecodeBlockBX1AI(
			block+(i*xs1+j)*16,
			rgba+(i*4*xs+j*4)*stride,
			stride, xs*stride,
			clrs);
	}
}

BGBBTJ_API void BGBBTJ_BT1D_DecodeImageBX1AI_BGRA32(byte *block,
	byte *rgba, int xs, int ys, int stride)
{
//	float tblk[16*4];
	float bgr, bgg, bgb;
	float cr, cg, cb, ca;
	float f, g;
	int xs1, ys1;
	int i, j, k, l;
	int i0, i1, i2, i3;
	int l0, l1, clrs;
	
	clrs=(stride>3)?BGBBTJ_JPG_BGRA:BGBBTJ_JPG_BGR;
	
	xs1=xs>>2; ys1=ys>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		BGBBTJ_BT1D_DecodeBlockBX1AI(
			block+(i*xs1+j)*16,
			rgba+(i*4*xs+j*4)*stride,
			stride, xs*stride,
			clrs);
	}
}
