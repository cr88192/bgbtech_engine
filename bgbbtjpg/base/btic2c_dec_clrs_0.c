#include <bgbbtj.h>

int BTIC2C_GetComponentPixel(BTIC2C_Context *ctx, int cn, int x, int y)
{
	int i, j, k, l;
	int id, w, h, n, v;

//	id=ctx->img_scn[cn];

//	i=x*ctx->img_schsc[cn];
//	j=y*ctx->img_scvsc[cn];
	i=(x*ctx->img_schsci[cn])>>16;
	j=(y*ctx->img_scvsci[cn])>>16;
	k=i>>3;
	l=j>>3;

	w=ctx->img_schs[cn];
//	h=ctx->img_scvs[cn];
	n=(l*w)+k;

	k=i&7;
	l=j&7;

	if(ctx->img_sjbuf[cn])
	{
		v=ctx->img_sjbuf[cn][n*64+(l*8+k)];
	}else
	{
		v=ctx->img_sibuf[cn][n*64+(l*8+k)];
	}
	return(v);
}

int BTIC2C_GetComponentPixelS420(BTIC2C_Context *ctx, int x, int y,
	int *ry0, int *ry1, int *ry2, int *ry3,
	int *ru, int *rv)
{
	int i, j, k, l;
	int t0, t1, w0, w1, n0, n1, x1, y1;

	x1=x>>1; y1=y>>1;

	w0=ctx->img_schs[0];
	w1=ctx->img_schs[1];

	i=x>>3; j=y>>3;
	k=x1>>3; l=y1>>3;
	n0=(j*w0)+i;
	n1=(l*w1)+k;

	i=x&7; j=y&7;
	k=x1&7; l=y1&7;
	t0=(n0<<6)+((j<<3)+i);
	t1=(n1<<6)+((l<<3)+k);

	if(ctx->img_sjbuf[0])
	{
		*ry0=ctx->img_sjbuf[0][t0];
		*ry1=ctx->img_sjbuf[0][t0+1];
		*ry2=ctx->img_sjbuf[0][t0+8];
		*ry3=ctx->img_sjbuf[0][t0+9];

		*ru=ctx->img_sjbuf[1][t1];
		*rv=ctx->img_sjbuf[2][t1];
	}else
	{
		*ry0=ctx->img_sibuf[0][t0];
		*ry1=ctx->img_sibuf[0][t0+1];
		*ry2=ctx->img_sibuf[0][t0+8];
		*ry3=ctx->img_sibuf[0][t0+9];

		*ru=ctx->img_sibuf[1][t1];
		*rv=ctx->img_sibuf[2][t1];
	}

	return(0);
}

int BTIC2C_GetComponentPixelS420A(BTIC2C_Context *ctx, int x, int y,
	int *ry0, int *ry1, int *ry2, int *ry3,
	int *ru, int *rv, int *ra)
{
	int i, j, k, l;
	int t0, t1, w0, w1, n0, n1, x1, y1;

	x1=x>>1; y1=y>>1;

	w0=ctx->img_schs[0];
	w1=ctx->img_schs[1];

	i=x>>3; j=y>>3;
	k=x1>>3; l=y1>>3;
	n0=(j*w0)+i;
	n1=(l*w1)+k;

	i=x&7; j=y&7;
	k=x1&7; l=y1&7;
	t0=(n0<<6)+((j<<3)+i);
	t1=(n1<<6)+((l<<3)+k);

	if(ctx->img_sjbuf[0])
	{
		*ry0=ctx->img_sjbuf[0][t0];
		*ry1=ctx->img_sjbuf[0][t0+1];
		*ry2=ctx->img_sjbuf[0][t0+8];
		*ry3=ctx->img_sjbuf[0][t0+9];

		*ru=ctx->img_sjbuf[1][t1];
		*rv=ctx->img_sjbuf[2][t1];
		*ra=ctx->img_sjbuf[3][t1];
	}else
	{
		*ry0=ctx->img_sibuf[0][t0];
		*ry1=ctx->img_sibuf[0][t0+1];
		*ry2=ctx->img_sibuf[0][t0+8];
		*ry3=ctx->img_sibuf[0][t0+9];

		*ru=ctx->img_sibuf[1][t1];
		*rv=ctx->img_sibuf[2][t1];
		*ra=ctx->img_sibuf[3][t1];
	}

	return(0);
}

void BTIC2C_DecodeColorTransformBlock(
	BTIC2C_Context *ctx, byte *obuf, int xo, int yo, int clrs)
{
	if(((xo+ctx->mbxs)>ctx->xs) || ((yo+ctx->mbys)>ctx->ys))
//	if(1)
	{
		BTIC2C_DecodeColorTransform_GenericBlock(
			ctx, obuf, xo, yo, clrs);
		return;
	}

	if(ctx->img_clrtrans==BTIC2C_CLRS_YCGCO)
	{
		if(ctx->img_is420)
		{
			if(ctx->img_alpha)
			{
				if((clrs==BGBBTJ_JPG_RGBA) || (clrs==BGBBTJ_JPG_RGBX))
				{
					BTIC2C_DecodeColorTransformYCgCo420A_Block_RGBA(
						ctx, obuf, xo, yo, clrs);
//					BTIC2C_DecodeColorTransformYCgCo420A_Block2_RGBA(
//						ctx, obuf, xo, yo, clrs);
					return;
				}else if((clrs==BGBBTJ_JPG_BGRA) || (clrs==BGBBTJ_JPG_BGRX))
				{
					BTIC2C_DecodeColorTransformYCgCo420A_Block_BGRA(
						ctx, obuf, xo, yo, clrs);
//					BTIC2C_DecodeColorTransformYCgCo420A_Block2_BGRA(
//						ctx, obuf, xo, yo, clrs);
					return;
				}else if(clrs==BGBBTJ_JPG_UYVY)
				{
					BTIC2C_DecodeColorTransformYCgCo420_Block_UYVY(
						ctx, obuf, xo, yo, clrs);
					return;
				}
			}else
			{
				if(clrs==BGBBTJ_JPG_UYVY)
				{
					BTIC2C_DecodeColorTransformYCgCo420_Block_UYVY(
						ctx, obuf, xo, yo, clrs);
					return;
				}
			}

			BTIC2C_DecodeColorTransformYCgCo420_Block(
				ctx, obuf, xo, yo, clrs);
			return;
		}
	}else if(ctx->img_clrtrans==BTIC2C_CLRS_RCT)
	{
		if(ctx->img_is420)
		{
			if(ctx->img_alpha)
			{
				if((clrs==BGBBTJ_JPG_RGBA) || (clrs==BGBBTJ_JPG_RGBX))
				{
					BTIC2C_DecodeColorTransformRCT420A_Block_RGBA(
						ctx, obuf, xo, yo, clrs);
					return;
				}else if((clrs==BGBBTJ_JPG_BGRA) || (clrs==BGBBTJ_JPG_BGRX))
				{
					BTIC2C_DecodeColorTransformRCT420A_Block_BGRA(
						ctx, obuf, xo, yo, clrs);
					return;
				}else if(clrs==BGBBTJ_JPG_UYVY)
				{
					BTIC2C_DecodeColorTransformRCT420_Block_UYVY(
						ctx, obuf, xo, yo, clrs);
					return;
				}
			}else
			{
				if(clrs==BGBBTJ_JPG_UYVY)
				{
					BTIC2C_DecodeColorTransformRCT420_Block_UYVY(
						ctx, obuf, xo, yo, clrs);
					return;
				}
			}

			BTIC2C_DecodeColorTransformRCT420_Block(
				ctx, obuf, xo, yo, clrs);
			return;
		}
	}else if(ctx->img_clrtrans==BTIC2C_CLRS_YCBCR)
	{
		if(ctx->img_is420)
		{
			if(clrs==BGBBTJ_JPG_UYVY)
			{
				BTIC2C_DecodeColorTransformYCbCr420_Block_UYVY(
					ctx, obuf, xo, yo, clrs);
				return;
			}
			BTIC2C_DecodeColorTransformYCbCr420_Block(
				ctx, obuf, xo, yo, clrs);
			return;
		}
	}
	
	if(((clrs>=BGBBTJ_JPG_BC1) && (clrs<=BGBBTJ_JPG_BC4A)) ||
		((clrs>=BGBBTJ_JPG_BC1_VF) && (clrs<=BGBBTJ_JPG_BC6_UVF)))
	{
		BTIC2C_DecodeColorTransform_BCnBlock(ctx, obuf, xo, yo, clrs);
		return;
	}
	
	BTIC2C_DecodeColorTransform_GenericBlock(
		ctx, obuf, xo, yo, clrs);
	return;
}

int BTIC2C_DecodeColorTransform_BCnBlock(BTIC2C_Context *ctx,
	byte *obuf, int xo, int yo, int clrs)
{
	byte tbuf8[16*16*4];
	byte *cs;
	int xs, ys, xs2, ys2, bn, xo2, yo2;
	int mbxs, mbys, mbxs2, mbys2;
	int i, j, k, l;

	xs=ctx->xs;			ys=ctx->ys;
	xs2=(xs+3)>>2;		ys2=(ys+3)>>2;
	xo2=xo>>2;			yo2=yo>>2;
//	bn=(yo>>2)*ys2+(xo>>2);

	mbxs=ctx->mbxs;		mbys=ctx->mbys;
	mbxs2=(mbxs+3)>>2;	mbys2=(mbys+3)>>2;

	if(clrs==BGBBTJ_JPG_BC7)
	{
		BTIC2C_DecodeColorTransform_GenericBlock2(ctx, tbuf8,
			xo, yo, 0, 0, mbxs, mbys, BGBBTJ_JPG_RGBA, 0);

		for(i=0; i<mbys2; i++)
			for(j=0; j<mbxs2; j++)
		{
			bn=((yo2+i)*xs2)+(xo2+j);
			cs=tbuf8+((i*4*mbxs+j*4)*4);
			BGBBTJ_BC7_EncodeBlock_Mode5(obuf+bn, cs, 4, mbxs*4);
		}
		return(0);
	}

	if(clrs==BGBBTJ_JPG_BC7_VF)
	{
		BTIC2C_DecodeColorTransform_GenericBlock2(ctx, tbuf8,
			xo, yo, 0, 0, mbxs, mbys, BGBBTJ_JPG_RGBA, 1);

		for(i=0; i<mbys2; i++)
			for(j=0; j<mbxs2; j++)
		{
//			bn=((yo2+i)*ys2)+(xo2+j);
			bn=((ys2-(yo2+i)-1)*xs2)+(xo2+j);
			cs=tbuf8+((i*4*mbxs+j*4)*4);
			BGBBTJ_BC7_EncodeBlock_Mode5(obuf+bn, cs, 4, mbxs*4);
		}
		return(0);
	}

	return(-1);
}

static int btic2c_hfloat2q8(u16 v)
{
	int i, e, m;
	
	e=(v>>10)&31;
	e-=15;
	m=(v&1023)|1024;

	if(e>=2)
	{
		i=m<<(e-2);
	}else
	{
		i=m>>(-e+3);
	}
	
	return(i);
}

int BTIC2C_DecodeColorTransform_GenericBlock(BTIC2C_Context *ctx,
	byte *obuf, int xo, int yo, int clrs)
{
	int cy, cu, cv, cr, cg, cb, ca;
	int x, y, xs, ys, mbxs, mbys;
	int i, j, k, l;

	xs=ctx->xs;			ys=ctx->ys;
	mbxs=ctx->mbxs;		mbys=ctx->mbys;
	for(i=0; i<mbys; i++)
		for(j=0; j<mbxs; j++)
	{
		x=xo+j; y=yo+i;
		if((x>=xs) || (y>=ys))
			continue;

		if(ctx->img_nc==4)
		{
			cy=BTIC2C_GetComponentPixel(ctx, 0, x, y);
			cu=BTIC2C_GetComponentPixel(ctx, 1, x, y);
			cv=BTIC2C_GetComponentPixel(ctx, 2, x, y);
			ca=BTIC2C_GetComponentPixel(ctx, 3, x, y);
		}else if(ctx->img_nc==3)
		{
			cy=BTIC2C_GetComponentPixel(ctx, 0, x, y);
			cu=BTIC2C_GetComponentPixel(ctx, 1, x, y);
			cv=BTIC2C_GetComponentPixel(ctx, 2, x, y);
			ca=255;
		}else if(ctx->img_nc==1)
		{
			cy=BTIC2C_GetComponentPixel(ctx, 0, x, y);
			cu=0; cv=0; ca=255;
		}

		switch(ctx->img_clrtrans)
		{
		case BTIC2C_CLRS_YCBCR:
//			cr=cy+1.40200*cv;
//			cg=cy-0.34414*cu-0.71414*cv;
//			cb=cy+1.77200*cu;
			cr=(256*cy       +359*cv+128)>>8;
			cg=(256*cy- 88*cu-183*cv+128)>>8;
			cb=(256*cy+454*cu       +128)>>8;
			break;
		case BTIC2C_CLRS_RCT:
			cg=cy-(cu+cv)>>2;
			cb=cg+cu;	cr=cg+cv;
			break;
		case BTIC2C_CLRS_YCGCO:
			k=(cu>>1);	l=(cv>>1);
			cg=cy+k;
			cr=cg-cu-l;
			cb=cg-cu+l;
			break;
		}

//		if(ctx->flip)
//			{ l=((y*xs)+x)*4; }
//		else
//			{ l=(((ys-1-y)*xs)+x)*4; }

		switch(clrs)
		{
		case BGBBTJ_JPG_RGBA:	case BGBBTJ_JPG_RGB:
		case BGBBTJ_JPG_BGRA:	case BGBBTJ_JPG_BGR:
		case BGBBTJ_JPG_RGBX:	case BGBBTJ_JPG_BGRX:
		case BGBBTJ_JPG_ARGB:	case BGBBTJ_JPG_ABGR:
		case BGBBTJ_JPG_UYVY:	case BGBBTJ_JPG_YUYV:
		case BGBBTJ_JPG_RGBA_LS16:
		case BGBBTJ_JPG_RGB_LS16:
			switch(ctx->clrs)
			{
			case BGBBTJ_JPG_RGBA:	case BGBBTJ_JPG_RGB:
			case BGBBTJ_JPG_BGRA:	case BGBBTJ_JPG_BGR:
			case BGBBTJ_JPG_ARGB:	case BGBBTJ_JPG_ABGR:
			case BGBBTJ_JPG_UYVY:	case BGBBTJ_JPG_YUYV:
			case BGBBTJ_JPG_RGBA_LS16:
			case BGBBTJ_JPG_RGB_LS16:
				break;
			case BGBBTJ_JPG_RGBA_Q11_4:
			case BGBBTJ_JPG_RGB_Q11_4:
				cr=cr>>4;	cg=cg>>4;
				cb=cb>>4;	ca=ca>>4;
				break;
			case BGBBTJ_JPG_RGBA_F16:
			case BGBBTJ_JPG_RGB_F16:
				cr=btic2c_hfloat2q8(cr);
				cg=btic2c_hfloat2q8(cg);
				cb=btic2c_hfloat2q8(cb);
				ca=btic2c_hfloat2q8(ca);
				break;
			case BGBBTJ_JPG_RGBA_F32:
			case BGBBTJ_JPG_RGB_F32:
				if(ctx->img_pbits==32)
				{
					cr=(*(float *)(&cr))*255;	cg=(*(float *)(&cg))*255;
					cb=(*(float *)(&cb))*255;	ca=(*(float *)(&ca))*255;
				}else
				{
					cr=btic2c_hfloat2q8(cr);	cg=btic2c_hfloat2q8(cg);
					cb=btic2c_hfloat2q8(cb);	ca=btic2c_hfloat2q8(ca);
				}
				break;
			default:
				break;
			}
			break;

		default:
			break;
		}

		if((clrs==BGBBTJ_JPG_RGBA) || (clrs==BGBBTJ_JPG_RGBX))
		{		
			if(ctx->flip)
				{ l=((y*xs)+x)*4; }
			else
				{ l=(((ys-1-y)*xs)+x)*4; }

			cr=(cr<0)?0:((cr>255)?255:cr);
			cg=(cg<0)?0:((cg>255)?255:cg);
			cb=(cb<0)?0:((cb>255)?255:cb);
			ca=(ca<0)?0:((ca>255)?255:ca);

			obuf[l+0]=cr;	obuf[l+1]=cg;
			obuf[l+2]=cb;	obuf[l+3]=ca;
		}else if((clrs==BGBBTJ_JPG_BGRA) || (clrs==BGBBTJ_JPG_BGRX))
		{
			if(ctx->flip)
				{ l=((y*xs)+x)*4; }
			else
				{ l=(((ys-1-y)*xs)+x)*4; }

			cr=(cr<0)?0:((cr>255)?255:cr);
			cg=(cg<0)?0:((cg>255)?255:cg);
			cb=(cb<0)?0:((cb>255)?255:cb);
			ca=(ca<0)?0:((ca>255)?255:ca);

			obuf[l+0]=cb;	obuf[l+1]=cg;
			obuf[l+2]=cr;	obuf[l+3]=ca;
		}else if(clrs==BGBBTJ_JPG_UYVY)
		{
			if(!ctx->flip)	{ l=((y*xs)+x)*2; }
				else		{ l=(((ys-1-y)*xs)+x)*2; }
			cy=(3*cr+4*cg+cb)>>3;
			if(x&1)			{ cu=((cr-cy)>>1)+128; }
				else		{ cu=((cb-cy)>>1)+128; }
			cy=(cy<0)?0:((cy>255)?255:cy);
			cu=(cu<0)?0:((cu>255)?255:cu);
			obuf[l+0]=cu;	obuf[l+1]=cy;
		}else if(clrs==BGBBTJ_JPG_YUYV)
		{
			if(!ctx->flip)	{ l=((y*xs)+x)*2; }
				else		{ l=(((ys-1-y)*xs)+x)*2; }
			cy=(3*cr+4*cg+cb)>>3;
			if(x&1)			{ cu=((cr-cy)>>1)+128; }
				else		{ cu=((cb-cy)>>1)+128; }
			cy=(cy<0)?0:((cy>255)?255:cy);
			cu=(cu<0)?0:((cu>255)?255:cu);
			obuf[l+0]=cy;	obuf[l+1]=cu;
		}
	}
	return(0);
}

int BTIC2C_DecodeColorTransform_GenericBlock2(BTIC2C_Context *ctx,
	byte *obuf, int ixo, int iyo,
	int oxo, int oyo, int oxs, int oys,
	int clrs, int flip)
{
	int cy, cu, cv, cr, cg, cb, ca;
	int x, y, xs, ys, mbxs, mbys;
	int i, j, k, l;

//	xs=ctx->xs;			ys=ctx->ys;
	xs=oxs;				ys=oys;
	mbxs=ctx->mbxs;		mbys=ctx->mbys;
	for(i=0; i<mbys; i++)
		for(j=0; j<mbxs; j++)
	{
		x=ixo+j; y=iyo+i;
		if((x>=xs) || (y>=ys))
			continue;

		if(ctx->img_nc==4)
		{
			cy=BTIC2C_GetComponentPixel(ctx, 0, x, y);
			cu=BTIC2C_GetComponentPixel(ctx, 1, x, y);
			cv=BTIC2C_GetComponentPixel(ctx, 2, x, y);
			ca=BTIC2C_GetComponentPixel(ctx, 3, x, y);
		}else if(ctx->img_nc==3)
		{
			cy=BTIC2C_GetComponentPixel(ctx, 0, x, y);
			cu=BTIC2C_GetComponentPixel(ctx, 1, x, y);
			cv=BTIC2C_GetComponentPixel(ctx, 2, x, y);
			ca=255;
		}else if(ctx->img_nc==1)
		{
			cy=BTIC2C_GetComponentPixel(ctx, 0, x, y);
			cu=0; cv=0; ca=255;
		}

		switch(ctx->img_clrtrans)
		{
		case BTIC2C_CLRS_YCBCR:
//			cr=cy+1.40200*cv;
//			cg=cy-0.34414*cu-0.71414*cv;
//			cb=cy+1.77200*cu;
			cr=(256*cy       +359*cv+128)>>8;
			cg=(256*cy- 88*cu-183*cv+128)>>8;
			cb=(256*cy+454*cu       +128)>>8;
			break;
		case BTIC2C_CLRS_RCT:
			cg=cy-(cu+cv)>>2;
			cb=cg+cu;	cr=cg+cv;
			break;
		case BTIC2C_CLRS_YCGCO:
			k=(cu>>1);	l=(cv>>1);
			cg=cy+k;
			cr=cg-cu-l;
			cb=cg-cu+l;
			break;
		}

//		if(ctx->flip)
//			{ l=((y*xs)+x)*4; }
//		else
//			{ l=(((ys-1-y)*xs)+x)*4; }

		switch(clrs)
		{
		case BGBBTJ_JPG_RGBA:	case BGBBTJ_JPG_RGB:
		case BGBBTJ_JPG_BGRA:	case BGBBTJ_JPG_BGR:
		case BGBBTJ_JPG_RGBX:	case BGBBTJ_JPG_BGRX:
		case BGBBTJ_JPG_ARGB:	case BGBBTJ_JPG_ABGR:
		case BGBBTJ_JPG_UYVY:	case BGBBTJ_JPG_YUYV:
		case BGBBTJ_JPG_RGBA_LS16:
		case BGBBTJ_JPG_RGB_LS16:
			switch(ctx->clrs)
			{
			case BGBBTJ_JPG_RGBA:	case BGBBTJ_JPG_RGB:
			case BGBBTJ_JPG_BGRA:	case BGBBTJ_JPG_BGR:
			case BGBBTJ_JPG_ARGB:	case BGBBTJ_JPG_ABGR:
			case BGBBTJ_JPG_UYVY:	case BGBBTJ_JPG_YUYV:
			case BGBBTJ_JPG_RGBA_LS16:
			case BGBBTJ_JPG_RGB_LS16:
				break;
			case BGBBTJ_JPG_RGBA_Q11_4:
			case BGBBTJ_JPG_RGB_Q11_4:
				cr=cr>>4;	cg=cg>>4;
				cb=cb>>4;	ca=ca>>4;
				break;
			case BGBBTJ_JPG_RGBA_F16:
			case BGBBTJ_JPG_RGB_F16:
				cr=btic2c_hfloat2q8(cr);
				cg=btic2c_hfloat2q8(cg);
				cb=btic2c_hfloat2q8(cb);
				ca=btic2c_hfloat2q8(ca);
				break;
			case BGBBTJ_JPG_RGBA_F32:
			case BGBBTJ_JPG_RGB_F32:
				if(ctx->img_pbits==32)
				{
					cr=(*(float *)(&cr))*255;	cg=(*(float *)(&cg))*255;
					cb=(*(float *)(&cb))*255;	ca=(*(float *)(&ca))*255;
				}else
				{
					cr=btic2c_hfloat2q8(cr);	cg=btic2c_hfloat2q8(cg);
					cb=btic2c_hfloat2q8(cb);	ca=btic2c_hfloat2q8(ca);
				}
				break;
			default:
				break;
			}
			break;

		default:
			break;
		}

		x=oxo+j; y=oyo+i;
		if((x>=xs) || (y>=ys))
			continue;

		if((clrs==BGBBTJ_JPG_RGBA) || (clrs==BGBBTJ_JPG_RGBX))
		{		
			if(flip)
				{ l=((y*oxs)+x)*4; }
			else
				{ l=(((oys-1-y)*oxs)+x)*4; }

			cr=(cr<0)?0:((cr>255)?255:cr);
			cg=(cg<0)?0:((cg>255)?255:cg);
			cb=(cb<0)?0:((cb>255)?255:cb);
			ca=(ca<0)?0:((ca>255)?255:ca);

			obuf[l+0]=cr;	obuf[l+1]=cg;
			obuf[l+2]=cb;	obuf[l+3]=ca;
		}else if((clrs==BGBBTJ_JPG_BGRA) || (clrs==BGBBTJ_JPG_BGRX))
		{
			if(flip)
				{ l=((y*oxs)+x)*4; }
			else
				{ l=(((oys-1-y)*oxs)+x)*4; }

			cr=(cr<0)?0:((cr>255)?255:cr);
			cg=(cg<0)?0:((cg>255)?255:cg);
			cb=(cb<0)?0:((cb>255)?255:cb);
			ca=(ca<0)?0:((ca>255)?255:ca);

			obuf[l+0]=cb;	obuf[l+1]=cg;
			obuf[l+2]=cr;	obuf[l+3]=ca;
		}else if(clrs==BGBBTJ_JPG_UYVY)
		{
			if(!flip)	{ l=((y*oxs)+x)*2; }
				else	{ l=(((oys-1-y)*oxs)+x)*2; }
			cy=(3*cr+4*cg+cb)>>3;
			if(x&1)		{ cu=((cr-cy)>>1)+128; }
				else	{ cu=((cb-cy)>>1)+128; }
			cy=(cy<0)?0:((cy>255)?255:cy);
			cu=(cu<0)?0:((cu>255)?255:cu);
			obuf[l+0]=cu;	obuf[l+1]=cy;
		}else if(clrs==BGBBTJ_JPG_YUYV)
		{
			if(!flip)	{ l=((y*oxs)+x)*2; }
				else	{ l=(((oys-1-y)*oxs)+x)*2; }
			cy=(3*cr+4*cg+cb)>>3;
			if(x&1)			{ cu=((cr-cy)>>1)+128; }
				else		{ cu=((cb-cy)>>1)+128; }
			cy=(cy<0)?0:((cy>255)?255:cy);
			cu=(cu<0)?0:((cu>255)?255:cu);
			obuf[l+0]=cy;	obuf[l+1]=cu;
		}
	}
	return(0);
}

int BTIC2C_DecodeColorTransform_DebugBlock(BTIC2C_Context *ctx,
	byte *obuf, int xo, int yo, int clrs)
{
	int cy, cu, cv, cr, cg, cb, ca;
	int x, y;
	int i, j, k, l;

	for(i=0; i<16; i++)
		for(j=0; j<16; j++)
	{
		x=xo+j; y=yo+i;

		if((x^y)&2)
		{
			cr=255; cg=0; cb=255; ca=255;
		}else
		{
			cr=0; cg=255; cb=255; ca=255;
		}

		if(ctx->flip)
			{ l=((y*ctx->xs)+x)*4; }
		else
		{
			k=ctx->ys-1-y;
			l=((k*ctx->xs)+x)*4;
		}

		if(clrs==BGBBTJ_JPG_RGBA)
		{
			cr=(cr<0)?0:((cr>255)?255:cr);
			cg=(cg<0)?0:((cg>255)?255:cg);
			cb=(cb<0)?0:((cb>255)?255:cb);

			obuf[l+0]=cr;	obuf[l+1]=cg;
			obuf[l+2]=cb;	obuf[l+3]=ca;
		}else if(clrs==BGBBTJ_JPG_BGRA)
		{
			cr=(cr<0)?0:((cr>255)?255:cr);
			cg=(cg<0)?0:((cg>255)?255:cg);
			cb=(cb<0)?0:((cb>255)?255:cb);

			obuf[l+0]=cb;	obuf[l+1]=cg;
			obuf[l+2]=cr;	obuf[l+3]=ca;
		}
	}
	return(0);
}


#define BT2C_CLRTRANS_SIMPLECLAMP255(cr)	\
	if((cr)&(~255)) (cr)=((cr)<0)?0:(((cr)>255)?255:(cr))

#define BT2C_CLRTRANS_SIMPLECLAMP255x3(cr, cg, cb)	\
	BT2C_CLRTRANS_SIMPLECLAMP255(cr); \
	BT2C_CLRTRANS_SIMPLECLAMP255(cg); \
	BT2C_CLRTRANS_SIMPLECLAMP255(cb)

#define BT2C_CLRTRANS_SIMPLECLAMP255x4(cr, cg, cb, ca)	\
	BT2C_CLRTRANS_SIMPLECLAMP255(cr); \
	BT2C_CLRTRANS_SIMPLECLAMP255(cg); \
	BT2C_CLRTRANS_SIMPLECLAMP255(cb); \
	BT2C_CLRTRANS_SIMPLECLAMP255(ca)

#define BT2C_CLRTRANS_SIMPLECLAMP255x4B(cr, cg, cb, ca)	\
		if(((cr)|(cg)|(cb)|(ca))&(~255))	\
			{ BT2C_CLRTRANS_SIMPLECLAMP255x4(cr, cg, cb, ca); }


#define BT2C_CLRTRANS_YCGCO2RGB(cy, cu, cv, cr, cg, cb)	\
	{	int tk, tl;					\
		tk=(cu>>1);	tl=(cv>>1);		\
		cg=cy+tk;					\
		cr=cg-cu-tl; cb=cg-cu+tl; }

#define BT2C_CLRTRANS_YCGCO2RGB420(		\
		cy0, cy1, cy2, cy3, cu, cv,		\
		cr0, cr1, cr2, cr3, 			\
		cg0, cg1, cg2, cg3,				\
		cb0, cb1, cb2, cb3)				\
	{	int l0, l1, l2, l3;				\
		int c0, c1;						\
		c1=cv;			c0=cu;			\
		l1=(c1>>1);		l0=(c0>>1);		\
		l2=c0+l1;	l3=c0-l1;			\
		cg0=cy0+l0; cg1=cy1+l0;			\
		cg2=cy2+l0;	cg3=cy3+l0;			\
		cr0=cg0-l2;	cr1=cg1-l2;			\
		cr2=cg2-l2;	cr3=cg3-l2;			\
		cb0=cg0-l3;	cb1=cg1-l3;			\
		cb2=cg2-l3;	cb3=cg3-l3; }

void BTIC2C_DecodeColorTransformYCgCo420_SubBlock4x4_NfRGBA(
	s32 *yb, s32 *ub, s32 *vb, s32 *ab, byte *obuf, int xs, int ys)
{
	int cr0, cr1, cr2, cr3;
	int cg0, cg1, cg2, cg3;
	int cb0, cb1, cb2, cb3;
	int l0, l1, l2, l3;
	int ca;

	ca=ab[0];
	BT2C_CLRTRANS_YCGCO2RGB420(
		yb[0], yb[1], yb[8], yb[9], ub[0], vb[0],
		cr0, cr1, cr2, cr3,
		cg0, cg1, cg2, cg3,
		cb0, cb1, cb2, cb3);
	BT2C_CLRTRANS_SIMPLECLAMP255x4B(cr0, cr1, cr2, cr3);
	BT2C_CLRTRANS_SIMPLECLAMP255x4B(cg0, cg1, cg2, cg3);
	BT2C_CLRTRANS_SIMPLECLAMP255x4B(cb0, cb1, cb2, cb3);
	BT2C_CLRTRANS_SIMPLECLAMP255(ca);
	l0=0*xs*4; l1=1*xs*4;
	obuf[l0+0]=cr0;	obuf[l0+1]=cg0;	obuf[l0+2]=cb0;	obuf[l0+3]=ca;
	obuf[l0+4]=cr1;	obuf[l0+5]=cg1;	obuf[l0+6]=cb1;	obuf[l0+7]=ca;
	obuf[l1+0]=cr2;	obuf[l1+1]=cg2;	obuf[l1+2]=cb2;	obuf[l1+3]=ca;
	obuf[l1+4]=cr3;	obuf[l1+5]=cg3;	obuf[l1+6]=cb3;	obuf[l1+7]=ca;

	ca=ab[1];
	BT2C_CLRTRANS_YCGCO2RGB420(
		yb[2], yb[3], yb[10], yb[11], ub[1], vb[1],
		cr0, cr1, cr2, cr3,
		cg0, cg1, cg2, cg3,
		cb0, cb1, cb2, cb3);
	BT2C_CLRTRANS_SIMPLECLAMP255x4B(cr0, cr1, cr2, cr3);
	BT2C_CLRTRANS_SIMPLECLAMP255x4B(cg0, cg1, cg2, cg3);
	BT2C_CLRTRANS_SIMPLECLAMP255x4B(cb0, cb1, cb2, cb3);
	BT2C_CLRTRANS_SIMPLECLAMP255(ca);
	l0=0*xs*4; l1=1*xs*4;
	obuf[l0+ 8]=cr0; obuf[l0+ 9]=cg0; obuf[l0+10]=cb0; obuf[l0+11]=ca;
	obuf[l0+12]=cr1; obuf[l0+13]=cg1; obuf[l0+14]=cb1; obuf[l0+15]=ca;
	obuf[l1+ 8]=cr2; obuf[l1+ 9]=cg2; obuf[l1+10]=cb2; obuf[l1+11]=ca;
	obuf[l1+12]=cr3; obuf[l1+13]=cg3; obuf[l1+14]=cb3; obuf[l1+15]=ca;

	ca=ab[8];
	BT2C_CLRTRANS_YCGCO2RGB420(
		yb[16], yb[17], yb[24], yb[25], ub[8], vb[8],
		cr0, cr1, cr2, cr3,
		cg0, cg1, cg2, cg3,
		cb0, cb1, cb2, cb3);
	BT2C_CLRTRANS_SIMPLECLAMP255x4B(cr0, cr1, cr2, cr3);
	BT2C_CLRTRANS_SIMPLECLAMP255x4B(cg0, cg1, cg2, cg3);
	BT2C_CLRTRANS_SIMPLECLAMP255x4B(cb0, cb1, cb2, cb3);
	BT2C_CLRTRANS_SIMPLECLAMP255(ca);
	l0=2*xs*4; l1=3*xs*4;
	obuf[l0+0]=cr0;	obuf[l0+1]=cg0;	obuf[l0+2]=cb0;	obuf[l0+3]=ca;
	obuf[l0+4]=cr1;	obuf[l0+5]=cg1;	obuf[l0+6]=cb1;	obuf[l0+7]=ca;
	obuf[l1+0]=cr2;	obuf[l1+1]=cg2;	obuf[l1+2]=cb2;	obuf[l1+3]=ca;
	obuf[l1+4]=cr3;	obuf[l1+5]=cg3;	obuf[l1+6]=cb3;	obuf[l1+7]=ca;

	ca=ab[9];
	BT2C_CLRTRANS_YCGCO2RGB420(
		yb[18], yb[19], yb[26], yb[27], ub[9], vb[9],
		cr0, cr1, cr2, cr3,
		cg0, cg1, cg2, cg3,
		cb0, cb1, cb2, cb3);
	BT2C_CLRTRANS_SIMPLECLAMP255x4B(cr0, cr1, cr2, cr3);
	BT2C_CLRTRANS_SIMPLECLAMP255x4B(cg0, cg1, cg2, cg3);
	BT2C_CLRTRANS_SIMPLECLAMP255x4B(cb0, cb1, cb2, cb3);
	BT2C_CLRTRANS_SIMPLECLAMP255(ca);
	l0=2*xs*4; l1=3*xs*4;
	obuf[l0+ 8]=cr0; obuf[l0+ 9]=cg0; obuf[l0+10]=cb0; obuf[l0+11]=ca;
	obuf[l0+12]=cr1; obuf[l0+13]=cg1; obuf[l0+14]=cb1; obuf[l0+15]=ca;
	obuf[l1+ 8]=cr2; obuf[l1+ 9]=cg2; obuf[l1+10]=cb2; obuf[l1+11]=ca;
	obuf[l1+12]=cr3; obuf[l1+13]=cg3; obuf[l1+14]=cb3; obuf[l1+15]=ca;
}

void BTIC2C_DecodeColorTransformYCgCo420_SubBlock8x8_NfRGBA(
	s32 *yb, s32 *ub, s32 *vb, s32 *ab, byte *obuf, int xs, int ys)
{
	BTIC2C_DecodeColorTransformYCgCo420_SubBlock4x4_NfRGBA(
		yb+0, ub+0, vb+0, ab+0, obuf+0, xs, ys);
	BTIC2C_DecodeColorTransformYCgCo420_SubBlock4x4_NfRGBA(
		yb+4, ub+2, vb+2, ab+2, obuf+16, xs, ys);
	BTIC2C_DecodeColorTransformYCgCo420_SubBlock4x4_NfRGBA(
		yb+32, ub+16, vb+16, ab+16, obuf+(4*xs*4)+0, xs, ys);
	BTIC2C_DecodeColorTransformYCgCo420_SubBlock4x4_NfRGBA(
		yb+36, ub+18, vb+18, ab+18, obuf+(4*xs*4)+16, xs, ys);
}

void BTIC2C_DecodeColorTransformYCgCo420_NfBlock2(
	BTIC2C_Context *ctx, byte *obuf, int xo, int yo, int clrs)
{
	s32 *si0, *si1, *si2, *si3;
	int t0, t1, w0, w1, n0, n1, x1, y1, xs, ys;
	int l0, l1, l2, l3;
	int i, j, k, l;

	w0=ctx->img_schs[0];
	w1=ctx->img_schs[1];
	si0=ctx->img_sibuf[0];
	si1=ctx->img_sibuf[1];
	si2=ctx->img_sibuf[2];
	si3=ctx->img_sibuf[3];
//	al=(ctx->img_alpha);

	x1=xo>>1; y1=yo>>1;
	l2=x1>>3; l3=y1>>3;
	n1=(l3*w1)+l2;

	l0=xo>>3; l1=yo>>3;
	xs=ctx->xs; ys=ctx->ys;

	n0=((l1+0)*w0)+(l0+0);
	BTIC2C_DecodeColorTransformYCgCo420_SubBlock8x8_NfRGBA(
		si0+n0*64, si1+n1*64, si2+n1*64, si3+n1*64,
		obuf+(yo+0)*xs+(0*4), ctx->xs, ctx->ys);
	n0=((l1+0)*w0)+(l0+1);
	BTIC2C_DecodeColorTransformYCgCo420_SubBlock8x8_NfRGBA(
		si0+n0*64, si1+n1*64+4, si2+n1*64+4, si3+n1*64+4,
		obuf+(yo+0)*xs+(8*4), ctx->xs, ctx->ys);

	n0=((l1+1)*w0)+(l0+0);
	BTIC2C_DecodeColorTransformYCgCo420_SubBlock8x8_NfRGBA(
		si0+n0*64, si1+n1*64+32, si2+n1*64+32, si3+n1*64+32,
		obuf+(yo+8)*xs+(0*4), ctx->xs, ctx->ys);
	n0=((l1+1)*w0)+(l0+1);
	BTIC2C_DecodeColorTransformYCgCo420_SubBlock8x8_NfRGBA(
		si0+n0*64, si1+n1*64+4, si2+n1*64+4, si3+n1*64+4,
		obuf+(yo+8)*xs+(8*4), ctx->xs, ctx->ys);
}

void BTIC2C_DecodeColorTransformYCgCo420_SubBlock4x4_RGBA(
	s32 *yb, s32 *ub, s32 *vb, s32 *ab, byte *obuf, int xs, int ys)
{
	int cr0, cr1, cr2, cr3;
	int cg0, cg1, cg2, cg3;
	int cb0, cb1, cb2, cb3;
	int l0, l1, l2, l3;
	int ca;

	ca=ab[0];
	BT2C_CLRTRANS_YCGCO2RGB420(
		yb[0], yb[1], yb[8], yb[9], ub[0], vb[0],
		cr0, cr1, cr2, cr3,
		cg0, cg1, cg2, cg3,
		cb0, cb1, cb2, cb3);
	BT2C_CLRTRANS_SIMPLECLAMP255x4B(cr0, cr1, cr2, cr3);
	BT2C_CLRTRANS_SIMPLECLAMP255x4B(cg0, cg1, cg2, cg3);
	BT2C_CLRTRANS_SIMPLECLAMP255x4B(cb0, cb1, cb2, cb3);
	BT2C_CLRTRANS_SIMPLECLAMP255(ca);
	l0=3*xs*4; l1=2*xs*4;
	obuf[l0+0]=cr0;	obuf[l0+1]=cg0;	obuf[l0+2]=cb0;	obuf[l0+3]=ca;
	obuf[l0+4]=cr1;	obuf[l0+5]=cg1;	obuf[l0+6]=cb1;	obuf[l0+7]=ca;
	obuf[l1+0]=cr2;	obuf[l1+1]=cg2;	obuf[l1+2]=cb2;	obuf[l1+3]=ca;
	obuf[l1+4]=cr3;	obuf[l1+5]=cg3;	obuf[l1+6]=cb3;	obuf[l1+7]=ca;

	ca=ab[1];
	BT2C_CLRTRANS_YCGCO2RGB420(
		yb[2], yb[3], yb[10], yb[11], ub[1], vb[1],
		cr0, cr1, cr2, cr3,
		cg0, cg1, cg2, cg3,
		cb0, cb1, cb2, cb3);
	BT2C_CLRTRANS_SIMPLECLAMP255x4B(cr0, cr1, cr2, cr3);
	BT2C_CLRTRANS_SIMPLECLAMP255x4B(cg0, cg1, cg2, cg3);
	BT2C_CLRTRANS_SIMPLECLAMP255x4B(cb0, cb1, cb2, cb3);
	BT2C_CLRTRANS_SIMPLECLAMP255(ca);
	l0=3*xs*4; l1=2*xs*4;
	obuf[l0+ 8]=cr0; obuf[l0+ 9]=cg0; obuf[l0+10]=cb0; obuf[l0+11]=ca;
	obuf[l0+12]=cr1; obuf[l0+13]=cg1; obuf[l0+14]=cb1; obuf[l0+15]=ca;
	obuf[l1+ 8]=cr2; obuf[l1+ 9]=cg2; obuf[l1+10]=cb2; obuf[l1+11]=ca;
	obuf[l1+12]=cr3; obuf[l1+13]=cg3; obuf[l1+14]=cb3; obuf[l1+15]=ca;

	ca=ab[8];
	BT2C_CLRTRANS_YCGCO2RGB420(
		yb[16], yb[17], yb[24], yb[25], ub[8], vb[8],
		cr0, cr1, cr2, cr3,
		cg0, cg1, cg2, cg3,
		cb0, cb1, cb2, cb3);
	BT2C_CLRTRANS_SIMPLECLAMP255x4B(cr0, cr1, cr2, cr3);
	BT2C_CLRTRANS_SIMPLECLAMP255x4B(cg0, cg1, cg2, cg3);
	BT2C_CLRTRANS_SIMPLECLAMP255x4B(cb0, cb1, cb2, cb3);
	BT2C_CLRTRANS_SIMPLECLAMP255(ca);
	l0=1*xs*4; l1=0*xs*4;
	obuf[l0+0]=cr0;	obuf[l0+1]=cg0;	obuf[l0+2]=cb0;	obuf[l0+3]=ca;
	obuf[l0+4]=cr1;	obuf[l0+5]=cg1;	obuf[l0+6]=cb1;	obuf[l0+7]=ca;
	obuf[l1+0]=cr2;	obuf[l1+1]=cg2;	obuf[l1+2]=cb2;	obuf[l1+3]=ca;
	obuf[l1+4]=cr3;	obuf[l1+5]=cg3;	obuf[l1+6]=cb3;	obuf[l1+7]=ca;

	ca=ab[9];
	BT2C_CLRTRANS_YCGCO2RGB420(
		yb[18], yb[19], yb[26], yb[27], ub[9], vb[9],
		cr0, cr1, cr2, cr3,
		cg0, cg1, cg2, cg3,
		cb0, cb1, cb2, cb3);
	BT2C_CLRTRANS_SIMPLECLAMP255x4B(cr0, cr1, cr2, cr3);
	BT2C_CLRTRANS_SIMPLECLAMP255x4B(cg0, cg1, cg2, cg3);
	BT2C_CLRTRANS_SIMPLECLAMP255x4B(cb0, cb1, cb2, cb3);
	BT2C_CLRTRANS_SIMPLECLAMP255(ca);
	l0=1*xs*4; l1=0*xs*4;
	obuf[l0+ 8]=cr0; obuf[l0+ 9]=cg0; obuf[l0+10]=cb0; obuf[l0+11]=ca;
	obuf[l0+12]=cr1; obuf[l0+13]=cg1; obuf[l0+14]=cb1; obuf[l0+15]=ca;
	obuf[l1+ 8]=cr2; obuf[l1+ 9]=cg2; obuf[l1+10]=cb2; obuf[l1+11]=ca;
	obuf[l1+12]=cr3; obuf[l1+13]=cg3; obuf[l1+14]=cb3; obuf[l1+15]=ca;
}

void BTIC2C_DecodeColorTransformYCgCo420_SubBlock8x8_RGBA(
	s32 *yb, s32 *ub, s32 *vb, s32 *ab, byte *obuf, int xs, int ys)
{
	BTIC2C_DecodeColorTransformYCgCo420_SubBlock4x4_RGBA(
		yb+0, ub+0, vb+0, ab+0, obuf+(4*xs*4)+0, xs, ys);
	BTIC2C_DecodeColorTransformYCgCo420_SubBlock4x4_RGBA(
		yb+4, ub+2, vb+2, ab+2, obuf+(4*xs*4)+16, xs, ys);
	BTIC2C_DecodeColorTransformYCgCo420_SubBlock4x4_RGBA(
		yb+32, ub+16, vb+16, ab+16, obuf+(0*xs*4)+0, xs, ys);
	BTIC2C_DecodeColorTransformYCgCo420_SubBlock4x4_RGBA(
		yb+36, ub+18, vb+18, ab+18, obuf+(0*xs*4)+16, xs, ys);
}

void BTIC2C_DecodeColorTransformYCgCo420A_Block2_RGBA(
	BTIC2C_Context *ctx, byte *obuf, int xo, int yo, int clrs)
{
	s32 *si0, *si1, *si2, *si3;
	int t0, t1, w0, w1, n0, n1, x1, y1, xs, ys, yb;
	int l0, l1, l2, l3;
	int i, j, k, l;

	w0=ctx->img_schs[0];
	w1=ctx->img_schs[1];
	si0=ctx->img_sibuf[0];
	si1=ctx->img_sibuf[1];
	si2=ctx->img_sibuf[2];
	si3=ctx->img_sibuf[3];
//	al=(ctx->img_alpha);

	x1=xo>>1; y1=yo>>1;
	l2=x1>>3; l3=y1>>3;
	n1=(l3*w1)+l2;

	l0=xo>>3; l1=yo>>3;
	xs=ctx->xs; ys=ctx->ys;
	yb=ys-yo-16;

	n0=((l1+0)*w0)+(l0+0);
	BTIC2C_DecodeColorTransformYCgCo420_SubBlock8x8_RGBA(
		si0+n0*64, si1+n1*64, si2+n1*64, si3+n1*64,
		obuf+(yb+8)*xs*4+((xo+0)*4), ctx->xs, ctx->ys);
	n0=((l1+0)*w0)+(l0+1);
	BTIC2C_DecodeColorTransformYCgCo420_SubBlock8x8_RGBA(
		si0+n0*64, si1+n1*64+4, si2+n1*64+4, si3+n1*64+4,
		obuf+(yb+8)*xs*4+((xo+8)*4), ctx->xs, ctx->ys);

	n0=((l1+1)*w0)+(l0+0);
	BTIC2C_DecodeColorTransformYCgCo420_SubBlock8x8_RGBA(
		si0+n0*64, si1+n1*64+32+0, si2+n1*64+32+0, si3+n1*64+32+0,
		obuf+(yb+0)*xs*4+((xo+0)*4), ctx->xs, ctx->ys);
	n0=((l1+1)*w0)+(l0+1);
	BTIC2C_DecodeColorTransformYCgCo420_SubBlock8x8_RGBA(
		si0+n0*64, si1+n1*64+32+4, si2+n1*64+32+4, si3+n1*64+32+4,
		obuf+(yb+0)*xs*4+((xo+8)*4), ctx->xs, ctx->ys);
}

void BTIC2C_DecodeColorTransformYCgCo420_SubBlock4x4_BGRA(
	s32 *yb, s32 *ub, s32 *vb, s32 *ab, byte *obuf, int xs, int ys)
{
	int cr0, cr1, cr2, cr3;
	int cg0, cg1, cg2, cg3;
	int cb0, cb1, cb2, cb3;
	int l0, l1, l2, l3;
	int ca;

	ca=ab[0];
	BT2C_CLRTRANS_YCGCO2RGB420(
		yb[0], yb[1], yb[8], yb[9], ub[0], vb[0],
		cr0, cr1, cr2, cr3,
		cg0, cg1, cg2, cg3,
		cb0, cb1, cb2, cb3);
	BT2C_CLRTRANS_SIMPLECLAMP255x4B(cr0, cr1, cr2, cr3);
	BT2C_CLRTRANS_SIMPLECLAMP255x4B(cg0, cg1, cg2, cg3);
	BT2C_CLRTRANS_SIMPLECLAMP255x4B(cb0, cb1, cb2, cb3);
	BT2C_CLRTRANS_SIMPLECLAMP255(ca);
	l0=3*xs*4; l1=2*xs*4;
	obuf[l0+0]=cb0;	obuf[l0+1]=cg0;	obuf[l0+2]=cr0;	obuf[l0+3]=ca;
	obuf[l0+4]=cb1;	obuf[l0+5]=cg1;	obuf[l0+6]=cr1;	obuf[l0+7]=ca;
	obuf[l1+0]=cb2;	obuf[l1+1]=cg2;	obuf[l1+2]=cr2;	obuf[l1+3]=ca;
	obuf[l1+4]=cb3;	obuf[l1+5]=cg3;	obuf[l1+6]=cr3;	obuf[l1+7]=ca;

	ca=ab[1];
	BT2C_CLRTRANS_YCGCO2RGB420(
		yb[2], yb[3], yb[10], yb[11], ub[1], vb[1],
		cr0, cr1, cr2, cr3,
		cg0, cg1, cg2, cg3,
		cb0, cb1, cb2, cb3);
	BT2C_CLRTRANS_SIMPLECLAMP255x4B(cr0, cr1, cr2, cr3);
	BT2C_CLRTRANS_SIMPLECLAMP255x4B(cg0, cg1, cg2, cg3);
	BT2C_CLRTRANS_SIMPLECLAMP255x4B(cb0, cb1, cb2, cb3);
	BT2C_CLRTRANS_SIMPLECLAMP255(ca);
	l0=3*xs*4; l1=2*xs*4;
	obuf[l0+ 8]=cb0; obuf[l0+ 9]=cg0; obuf[l0+10]=cr0; obuf[l0+11]=ca;
	obuf[l0+12]=cb1; obuf[l0+13]=cg1; obuf[l0+14]=cr1; obuf[l0+15]=ca;
	obuf[l1+ 8]=cb2; obuf[l1+ 9]=cg2; obuf[l1+10]=cr2; obuf[l1+11]=ca;
	obuf[l1+12]=cb3; obuf[l1+13]=cg3; obuf[l1+14]=cr3; obuf[l1+15]=ca;

	ca=ab[8];
	BT2C_CLRTRANS_YCGCO2RGB420(
		yb[16], yb[17], yb[24], yb[25], ub[8], vb[8],
		cr0, cr1, cr2, cr3,
		cg0, cg1, cg2, cg3,
		cb0, cb1, cb2, cb3);
	BT2C_CLRTRANS_SIMPLECLAMP255x4B(cr0, cr1, cr2, cr3);
	BT2C_CLRTRANS_SIMPLECLAMP255x4B(cg0, cg1, cg2, cg3);
	BT2C_CLRTRANS_SIMPLECLAMP255x4B(cb0, cb1, cb2, cb3);
	BT2C_CLRTRANS_SIMPLECLAMP255(ca);
	l0=1*xs*4; l1=0*xs*4;
	obuf[l0+0]=cb0;	obuf[l0+1]=cg0;	obuf[l0+2]=cr0;	obuf[l0+3]=ca;
	obuf[l0+4]=cb1;	obuf[l0+5]=cg1;	obuf[l0+6]=cr1;	obuf[l0+7]=ca;
	obuf[l1+0]=cb2;	obuf[l1+1]=cg2;	obuf[l1+2]=cr2;	obuf[l1+3]=ca;
	obuf[l1+4]=cb3;	obuf[l1+5]=cg3;	obuf[l1+6]=cr3;	obuf[l1+7]=ca;

	ca=ab[9];
	BT2C_CLRTRANS_YCGCO2RGB420(
		yb[18], yb[19], yb[26], yb[27], ub[9], vb[9],
		cr0, cr1, cr2, cr3,
		cg0, cg1, cg2, cg3,
		cb0, cb1, cb2, cb3);
	BT2C_CLRTRANS_SIMPLECLAMP255x4B(cr0, cr1, cr2, cr3);
	BT2C_CLRTRANS_SIMPLECLAMP255x4B(cg0, cg1, cg2, cg3);
	BT2C_CLRTRANS_SIMPLECLAMP255x4B(cb0, cb1, cb2, cb3);
	BT2C_CLRTRANS_SIMPLECLAMP255(ca);
	l0=1*xs*4; l1=0*xs*4;
	obuf[l0+ 8]=cb0; obuf[l0+ 9]=cg0; obuf[l0+10]=cr0; obuf[l0+11]=ca;
	obuf[l0+12]=cb1; obuf[l0+13]=cg1; obuf[l0+14]=cr1; obuf[l0+15]=ca;
	obuf[l1+ 8]=cb2; obuf[l1+ 9]=cg2; obuf[l1+10]=cr2; obuf[l1+11]=ca;
	obuf[l1+12]=cb3; obuf[l1+13]=cg3; obuf[l1+14]=cr3; obuf[l1+15]=ca;
}

void BTIC2C_DecodeColorTransformYCgCo420_SubBlock8x8_BGRA(
	s32 *yb, s32 *ub, s32 *vb, s32 *ab, byte *obuf, int xs, int ys)
{
	BTIC2C_DecodeColorTransformYCgCo420_SubBlock4x4_BGRA(
		yb+ 0, ub+ 0, vb+ 0, ab+ 0, obuf+(4*xs*4)+ 0, xs, ys);
	BTIC2C_DecodeColorTransformYCgCo420_SubBlock4x4_BGRA(
		yb+ 4, ub+ 2, vb+ 2, ab+ 2, obuf+(4*xs*4)+16, xs, ys);
	BTIC2C_DecodeColorTransformYCgCo420_SubBlock4x4_BGRA(
		yb+32, ub+16, vb+16, ab+16, obuf+(0*xs*4)+ 0, xs, ys);
	BTIC2C_DecodeColorTransformYCgCo420_SubBlock4x4_BGRA(
		yb+36, ub+18, vb+18, ab+18, obuf+(0*xs*4)+16, xs, ys);
}

void BTIC2C_DecodeColorTransformYCgCo420A_Block2_BGRA(
	BTIC2C_Context *ctx, byte *obuf, int xo, int yo, int clrs)
{
	s32 *si0, *si1, *si2, *si3;
	int t0, t1, w0, w1, n0, n1, x1, y1, xs, ys, yb;
	int l0, l1, l2, l3;
	int i, j, k, l;

	w0=ctx->img_schs[0];
	w1=ctx->img_schs[1];
	si0=ctx->img_sibuf[0];
	si1=ctx->img_sibuf[1];
	si2=ctx->img_sibuf[2];
	si3=ctx->img_sibuf[3];
//	al=(ctx->img_alpha);

	x1=xo>>1; y1=yo>>1;
	l2=x1>>3; l3=y1>>3;
	n1=(l3*w1)+l2;

	l0=xo>>3; l1=yo>>3;
	xs=ctx->xs; ys=ctx->ys;
	yb=ys-yo-16;

	n0=((l1+0)*w0)+(l0+0);
	BTIC2C_DecodeColorTransformYCgCo420_SubBlock8x8_BGRA(
		si0+n0*64, si1+n1*64+0, si2+n1*64+0, si3+n1*64,
		obuf+(yb+8)*xs*4+((xo+0)*4), ctx->xs, ctx->ys);
	n0=((l1+0)*w0)+(l0+1);
	BTIC2C_DecodeColorTransformYCgCo420_SubBlock8x8_BGRA(
		si0+n0*64, si1+n1*64+4, si2+n1*64+4, si3+n1*64+4,
		obuf+(yb+8)*xs*4+((xo+8)*4), ctx->xs, ctx->ys);

	n0=((l1+1)*w0)+(l0+0);
	BTIC2C_DecodeColorTransformYCgCo420_SubBlock8x8_BGRA(
		si0+n0*64, si1+n1*64+32+0, si2+n1*64+32+0, si3+n1*64+32+0,
		obuf+(yb+0)*xs*4+((xo+0)*4), ctx->xs, ctx->ys);
	n0=((l1+1)*w0)+(l0+1);
	BTIC2C_DecodeColorTransformYCgCo420_SubBlock8x8_BGRA(
		si0+n0*64, si1+n1*64+32+4, si2+n1*64+32+4, si3+n1*64+32+4,
		obuf+(yb+0)*xs*4+((xo+8)*4), ctx->xs, ctx->ys);
}

#if 1
void BTIC2C_DecodeColorTransformYCgCo420_Block(
	BTIC2C_Context *ctx, byte *obuf, int xo, int yo, int clrs)
{
//	s32 *si0, *si1, *si2, *si3;
	s16 *si0, *si1, *si2, *si3;
	byte *ob0, *ob1;
	int t0, t1, w0, w1, n0, n1, x1, y1;
	int al, x, y, xs, ys;
	int cy, cu, cv, cr, cg, cb, ca;
	int cu1, cv1, cu2, cv2;
	int cy0, cy1, cy2, cy3;
	int cr0, cg0, cb0, ca0;
	int cr1, cg1, cb1, ca1;
	int cr2, cg2, cb2, ca2;
	int cr3, cg3, cb3, ca3;
	int l0, l1, l2, l3;
	int i, j, k, l;

#if 1
	w0=ctx->img_schs[0];
	w1=ctx->img_schs[1];
//	si0=ctx->img_sibuf[0];
//	si1=ctx->img_sibuf[1];
//	si2=ctx->img_sibuf[2];
//	si3=ctx->img_sibuf[3];
	si0=ctx->img_sjbuf[0];
	si1=ctx->img_sjbuf[1];
	si2=ctx->img_sjbuf[2];
	si3=ctx->img_sjbuf[3];
	al=(ctx->img_alpha);
	xs=ctx->xs;
	ys=ctx->ys;

	for(i=0; i<16; i+=2)
		for(j=0; j<16; j+=2)
	{
		x=xo+j; y=yo+i;
		x1=x>>1; y1=y>>1;
		l0=x>>3; l1=y>>3;
		l2=x1>>3; l3=y1>>3;
		n0=(l1*w0)+l0;
		n1=(l3*w1)+l2;

		l0=x&7; l1=y&7;
		l2=x1&7; l3=y1&7;
		t0=(n0<<6)+((l1<<3)+l0);
		t1=(n1<<6)+((l3<<3)+l2);

		cy0=si0[t0  ];	cy1=si0[t0+1];
		cy2=si0[t0+8];	cy3=si0[t0+9];
		cu=si1[t1];		cv=si2[t1];
		ca=al?si3[t1]:255;

#if 1
		l0=(cu>>1);
		cg0=cy0+l0; cg1=cy1+l0;
		cg2=cy2+l0;	cg3=cy3+l0;
		l1=cu+(cv>>1);
		cr0=cg0-l1;	cr1=cg1-l1;
		cr2=cg2-l1;	cr3=cg3-l1;
		l2=cu-(cv>>1);
		cb0=cg0-l2;	cb1=cg1-l2;
		cb2=cg2-l2;	cb3=cg3-l2;
#endif

		BT2C_CLRTRANS_SIMPLECLAMP255x4B(cr0, cr1, cr2, cr3);
		BT2C_CLRTRANS_SIMPLECLAMP255x4B(cg0, cg1, cg2, cg3);
		BT2C_CLRTRANS_SIMPLECLAMP255x4B(cb0, cb1, cb2, cb3);
//		BT2C_CLRTRANS_SIMPLECLAMP255x4B(ca0, ca1, ca2, ca3);

#if 0
		if((cr0|cr1|cr2|cr3)&(~255))
		{
			if(cr0&(~255))cr0=(cr0<0)?0:((cr0>255)?255:cr0);
			if(cr1&(~255))cr1=(cr1<0)?0:((cr1>255)?255:cr1);
			if(cr2&(~255))cr2=(cr2<0)?0:((cr2>255)?255:cr2);
			if(cr3&(~255))cr3=(cr3<0)?0:((cr3>255)?255:cr3);
		}

		if((cg0|cg1|cg2|cg3)&(~255))
		{
			if(cg0&(~255))cg0=(cg0<0)?0:((cg0>255)?255:cg0);
			if(cg1&(~255))cg1=(cg1<0)?0:((cg1>255)?255:cg1);
			if(cg2&(~255))cg2=(cg2<0)?0:((cg2>255)?255:cg2);
			if(cg3&(~255))cg3=(cg3<0)?0:((cg3>255)?255:cg3);
		}

		if((cb0|cb1|cb2|cb3)&(~255))
		{
			if(cb0&(~255))cb0=(cb0<0)?0:((cb0>255)?255:cb0);
			if(cb1&(~255))cb1=(cb1<0)?0:((cb1>255)?255:cb1);
			if(cb2&(~255))cb2=(cb2<0)?0:((cb2>255)?255:cb2);
			if(cb3&(~255))cb3=(cb3<0)?0:((cb3>255)?255:cb3);
		}
#endif

#if 1
		if((clrs==BGBBTJ_JPG_RGBA) || (clrs==BGBBTJ_JPG_RGBX))
		{
			if(ctx->flip)
				{	l0=(((y+0)*xs)+x+0)*4;
					l2=(((y+1)*xs)+x+0)*4; }
			else
				{	l0=(((ys-1-y)*xs)+x+0)*4;
					l2=(((ys-2-y)*xs)+x+0)*4; }

//			l0=(((ys-1-y)*xs)+x+0)*4;
//			l2=(((ys-2-y)*xs)+x+0)*4;
			ob0=obuf+l0; ob1=obuf+l2;

			ob0[0]=cr0; ob0[1]=cg0; ob0[2]=cb0; ob0[3]=ca;
			ob0[4]=cr1; ob0[5]=cg1; ob0[6]=cb1; ob0[7]=ca;
			ob1[0]=cr2; ob1[1]=cg2; ob1[2]=cb2; ob1[3]=ca;
			ob1[4]=cr3; ob1[5]=cg3; ob1[6]=cb3; ob1[7]=ca;
		}else if((clrs==BGBBTJ_JPG_BGRA) || (clrs==BGBBTJ_JPG_BGRX))
		{
			if(ctx->flip)
				{	l0=(((y+0)*xs)+x+0)*4;
					l2=(((y+1)*xs)+x+0)*4; }
			else
				{	l0=(((ys-1-y)*xs)+x+0)*4;
					l2=(((ys-2-y)*xs)+x+0)*4; }

//			l0=(((ys-1-y)*xs)+x+0)*4;
//			l2=(((ys-2-y)*xs)+x+0)*4;
			ob0=obuf+l0; ob1=obuf+l2;

			ob0[0]=cb0; ob0[1]=cg0; ob0[2]=cr0; ob0[3]=ca;
			ob0[4]=cb1; ob0[5]=cg1; ob0[6]=cr1; ob0[7]=ca;
			ob1[0]=cb2; ob1[1]=cg2; ob1[2]=cr2; ob1[3]=ca;
			ob1[4]=cb3; ob1[5]=cg3; ob1[6]=cr3; ob1[7]=ca;
		}else if(clrs==BGBBTJ_JPG_UYVY)
		{
			if(ctx->flip)
				{	l0=(((y+0)*xs)+x+0)*2;
					l2=(((y+1)*xs)+x+0)*2; }
			else
				{	l0=(((ys-1-y)*xs)+x+0)*2;
					l2=(((ys-2-y)*xs)+x+0)*2; }

//			l0=(((ys-1-y)*xs)+x+0)*2;
//			l2=(((ys-2-y)*xs)+x+0)*2;
			ob0=obuf+l0; ob1=obuf+l2;
			
			cu1=(((cb0-cg0)+(cb1-cg1))>>2)+128;
			cv1=(((cr0-cg0)+(cr1-cg1))>>2)+128;
			cu2=(((cb2-cg2)+(cb3-cg3))>>2)+128;
			cv2=(((cr2-cg2)+(cr3-cg3))>>2)+128;
			ob0[0]=cu1; ob0[1]=cy0; ob0[2]=cv1; ob0[3]=cy1;
			ob1[0]=cu2; ob1[1]=cy2; ob1[2]=cv2; ob1[3]=cy3;
		}else if(clrs==BGBBTJ_JPG_YUYV)
		{
			if(ctx->flip)
				{	l0=(((y+0)*xs)+x+0)*2;
					l2=(((y+1)*xs)+x+0)*2; }
			else
				{	l0=(((ys-1-y)*xs)+x+0)*2;
					l2=(((ys-2-y)*xs)+x+0)*2; }

//			l0=(((ys-1-y)*xs)+x+0)*2;
//			l2=(((ys-2-y)*xs)+x+0)*2;
			ob0=obuf+l0; ob1=obuf+l2;
			
			cu1=(((cb0-cg0)+(cb1-cg1))>>2)+128;
			cv1=(((cr0-cg0)+(cr1-cg1))>>2)+128;
			cu2=(((cb2-cg2)+(cb3-cg3))>>2)+128;
			cv2=(((cr2-cg2)+(cr3-cg3))>>2)+128;
			ob0[0]=cy0; ob0[1]=cu1; ob0[2]=cy1; ob0[3]=cv1;
			ob1[0]=cy2; ob1[1]=cu2; ob1[2]=cy3; ob1[3]=cv2;
		}
#endif
	}
#endif
}

void BTIC2C_DecodeColorTransformYCgCo420A_Block_RGBA(
	BTIC2C_Context *ctx, byte *obuf, int xo, int yo, int clrs)
{
//	s32 *si0, *si1, *si2, *si3;
	s16 *si0, *si1, *si2, *si3;
	byte *ob0, *ob1;
	int t0, t1, w0, w1, n0, n1, x1, y1;
	int al, x, y, xs, ys;
	int cy, cu, cv, cr, cg, cb, ca;
	int cy0, cy1, cy2, cy3;
	int cr0, cg0, cb0, ca0;
	int cr1, cg1, cb1, ca1;
	int cr2, cg2, cb2, ca2;
	int cr3, cg3, cb3, ca3;
	int l0, l1, l2, l3;
	int i, j, k, l;

#if 1
	w0=ctx->img_schs[0];
	w1=ctx->img_schs[1];
//	si0=ctx->img_sibuf[0];
//	si1=ctx->img_sibuf[1];
//	si2=ctx->img_sibuf[2];
//	si3=ctx->img_sibuf[3];

	si0=ctx->img_sjbuf[0];
	si1=ctx->img_sjbuf[1];
	si2=ctx->img_sjbuf[2];
	si3=ctx->img_sjbuf[3];
	xs=ctx->xs;
	ys=ctx->ys;

//	l2=xo>>4; l3=yo>>4;
//	n1=(l3*w1)+l2;

	for(i=0; i<16; i+=2)
		for(j=0; j<16; j+=2)
	{
		x=xo+j; y=yo+i;
		x1=x>>1; y1=y>>1;
		l0=x>>3; l1=y>>3;
		l2=x1>>3; l3=y1>>3;
		n0=(l1*w0)+l0;
		n1=(l3*w1)+l2;

		l0=x&7; l1=y&7;
		l2=x1&7; l3=y1&7;
		t0=(n0<<6)+((l1<<3)+l0);
		t1=(n1<<6)+((l3<<3)+l2);

		cy0=si0[t0  ];	cy1=si0[t0+1];
		cy2=si0[t0+8];	cy3=si0[t0+9];
		cu=si1[t1];		cv=si2[t1];
		ca=si3[t1];

#if 1
		l0=(cu>>1); l1=(cv>>1);
		l2=cu+l1;	l3=cu-l1;
		cg0=cy0+l0; cg1=cy1+l0;
		cg2=cy2+l0;	cg3=cy3+l0;
		cr0=cg0-l2;	cr1=cg1-l2;
		cr2=cg2-l2;	cr3=cg3-l2;
		cb0=cg0-l3;	cb1=cg1-l3;
		cb2=cg2-l3;	cb3=cg3-l3;
#endif

#if 0
		l0=(cu>>1);
		cg0=cy0+l0; cg1=cy1+l0;
		cg2=cy2+l0;	cg3=cy3+l0;
		l1=cu+(cv>>1);
		cr0=cg0-l1;	cr1=cg1-l1;
		cr2=cg2-l1;	cr3=cg3-l1;
		l2=cu-(cv>>1);
		cb0=cg0-l2;	cb1=cg1-l2;
		cb2=cg2-l2;	cb3=cg3-l2;
#endif

		BT2C_CLRTRANS_SIMPLECLAMP255x4B(cr0, cr1, cr2, cr3);
		BT2C_CLRTRANS_SIMPLECLAMP255x4B(cg0, cg1, cg2, cg3);
		BT2C_CLRTRANS_SIMPLECLAMP255x4B(cb0, cb1, cb2, cb3);
//		BT2C_CLRTRANS_SIMPLECLAMP255x4B(ca0, ca1, ca2, ca3);
		BT2C_CLRTRANS_SIMPLECLAMP255(ca);

#if 0
		if((cr0|cr1|cr2|cr3)&(~255))
		{
			cr0=(cr0|((-(cr0>>8))>>31))&(~(cr0>>31));
			cr1=(cr1|((-(cr1>>8))>>31))&(~(cr1>>31));
			cr2=(cr2|((-(cr2>>8))>>31))&(~(cr2>>31));
			cr3=(cr3|((-(cr3>>8))>>31))&(~(cr3>>31));
		}
		if((cg0|cg1|cg2|cg3)&(~255))
		{
			cg0=(cg0|((-(cg0>>8))>>31))&(~(cg0>>31));
			cg1=(cg1|((-(cg1>>8))>>31))&(~(cg1>>31));
			cg2=(cg2|((-(cg2>>8))>>31))&(~(cg2>>31));
			cg3=(cg3|((-(cg3>>8))>>31))&(~(cg3>>31));
		}
		if((cb0|cb1|cb2|cb3)&(~255))
		{
			cb0=(cb0|((-(cb0>>8))>>31))&(~(cb0>>31));
			cb1=(cb1|((-(cb1>>8))>>31))&(~(cb1>>31));
			cb2=(cb2|((-(cb2>>8))>>31))&(~(cb2>>31));
			cb3=(cb3|((-(cb3>>8))>>31))&(~(cb3>>31));
		}
#endif

#if 0
		if((cr0|cr1|cr2|cr3)&(~255))
		{
			if(cr0&(~255))cr0=(cr0<0)?0:((cr0>255)?255:cr0);
			if(cr1&(~255))cr1=(cr1<0)?0:((cr1>255)?255:cr1);
			if(cr2&(~255))cr2=(cr2<0)?0:((cr2>255)?255:cr2);
			if(cr3&(~255))cr3=(cr3<0)?0:((cr3>255)?255:cr3);
		}

		if((cg0|cg1|cg2|cg3)&(~255))
		{
			if(cg0&(~255))cg0=(cg0<0)?0:((cg0>255)?255:cg0);
			if(cg1&(~255))cg1=(cg1<0)?0:((cg1>255)?255:cg1);
			if(cg2&(~255))cg2=(cg2<0)?0:((cg2>255)?255:cg2);
			if(cg3&(~255))cg3=(cg3<0)?0:((cg3>255)?255:cg3);
		}

		if((cb0|cb1|cb2|cb3)&(~255))
		{
			if(cb0&(~255))cb0=(cb0<0)?0:((cb0>255)?255:cb0);
			if(cb1&(~255))cb1=(cb1<0)?0:((cb1>255)?255:cb1);
			if(cb2&(~255))cb2=(cb2<0)?0:((cb2>255)?255:cb2);
			if(cb3&(~255))cb3=(cb3<0)?0:((cb3>255)?255:cb3);
		}
#endif

		if(ctx->flip)
		{
//			l=((y*xs)+x)*4;
			l0=(((y+0)*xs)+x+0)*4;
			l2=(((y+1)*xs)+x+0)*4;
		}
		else
		{
//			l=(((ys-1-y)*xs)+x)*4;
			k=ys-1-y;
			l0=((k*xs)+x+0)*4;
			k=ys-2-y;
			l2=((k*xs)+x+0)*4;
		}

//		k=ys-1-y;
//		l0=((k*xs)+x+0)*4;
//		k=ys-2-y;
//		l2=((k*xs)+x+0)*4;
		ob0=obuf+l0; ob1=obuf+l2;

//		ca0=cr0|(((byte)cg0)<<8)|(((byte)cb0)<<16)|(((byte)ca)<<24);
//		ca1=cr1|(((byte)cg1)<<8)|(((byte)cb1)<<16)|(((byte)ca)<<24);
//		ca2=cr2|(((byte)cg2)<<8)|(((byte)cb2)<<16)|(((byte)ca)<<24);
//		ca3=cr3|(((byte)cg3)<<8)|(((byte)cb3)<<16)|(((byte)ca)<<24);
		
//		((u32 *)ob0)[0]=ca0;		((u32 *)ob0)[1]=ca1;
//		((u32 *)ob1)[0]=ca2;		((u32 *)ob1)[1]=ca3;

//		k=ctx->ys-1-y;
//		l0=((k*ctx->xs)+x+0)*4;
//		k=ctx->ys-2-y;
//		l2=((k*ctx->xs)+x+0)*4;
//		ob0=obuf+l0; ob1=obuf+l2;

		ob0[0]=cr0; ob0[1]=cg0; ob0[2]=cb0; ob0[3]=ca;
		ob0[4]=cr1; ob0[5]=cg1; ob0[6]=cb1; ob0[7]=ca;
		ob1[0]=cr2; ob1[1]=cg2; ob1[2]=cb2; ob1[3]=ca;
		ob1[4]=cr3; ob1[5]=cg3; ob1[6]=cb3; ob1[7]=ca;
	}
#endif
}

void BTIC2C_DecodeColorTransformYCgCo420A_Block_BGRA(
	BTIC2C_Context *ctx, byte *obuf, int xo, int yo, int clrs)
{
//	s32 *si0, *si1, *si2, *si3;
	s16 *si0, *si1, *si2, *si3;
	byte *ob0, *ob1;
	int t0, t1, w0, w1, n0, n1, x1, y1;
	int al, x, y, xs, ys;
	int cy, cu, cv, cr, cg, cb, ca;
	int cy0, cy1, cy2, cy3;
	int cr0, cg0, cb0, ca0;
	int cr1, cg1, cb1, ca1;
	int cr2, cg2, cb2, ca2;
	int cr3, cg3, cb3, ca3;
	int l0, l1, l2, l3;
	int i, j, k, l;

#if 1
	w0=ctx->img_schs[0];
	w1=ctx->img_schs[1];
//	si0=ctx->img_sibuf[0];
//	si1=ctx->img_sibuf[1];
//	si2=ctx->img_sibuf[2];
//	si3=ctx->img_sibuf[3];

	si0=ctx->img_sjbuf[0];
	si1=ctx->img_sjbuf[1];
	si2=ctx->img_sjbuf[2];
	si3=ctx->img_sjbuf[3];
	xs=ctx->xs;
	ys=ctx->ys;

	for(i=0; i<16; i+=2)
		for(j=0; j<16; j+=2)
	{
		x=xo+j; y=yo+i;
		x1=x>>1; y1=y>>1;
		l0=x>>3; l1=y>>3;
		l2=x1>>3; l3=y1>>3;
		n0=(l1*w0)+l0;
		n1=(l3*w1)+l2;

		l0=x&7; l1=y&7;
		l2=x1&7; l3=y1&7;
		t0=(n0<<6)+((l1<<3)+l0);
		t1=(n1<<6)+((l3<<3)+l2);

		cy0=si0[t0  ];	cy1=si0[t0+1];
		cy2=si0[t0+8];	cy3=si0[t0+9];
		cu=si1[t1];		cv=si2[t1];
		ca=si3[t1];

#if 1
		l0=(cu>>1);
		cg0=cy0+l0; cg1=cy1+l0;
		cg2=cy2+l0;	cg3=cy3+l0;
		l1=cu+(cv>>1);
		cr0=cg0-l1;	cr1=cg1-l1;
		cr2=cg2-l1;	cr3=cg3-l1;
		l2=cu-(cv>>1);
		cb0=cg0-l2;	cb1=cg1-l2;
		cb2=cg2-l2;	cb3=cg3-l2;
#endif

		BT2C_CLRTRANS_SIMPLECLAMP255x4B(cr0, cr1, cr2, cr3);
		BT2C_CLRTRANS_SIMPLECLAMP255x4B(cg0, cg1, cg2, cg3);
		BT2C_CLRTRANS_SIMPLECLAMP255x4B(cb0, cb1, cb2, cb3);
//		BT2C_CLRTRANS_SIMPLECLAMP255x4B(ca0, ca1, ca2, ca3);
		BT2C_CLRTRANS_SIMPLECLAMP255(ca);

#if 0
		if((cr0|cr1|cr2|cr3)&(~255))
		{
			if(cr0&(~255))cr0=(cr0<0)?0:((cr0>255)?255:cr0);
			if(cr1&(~255))cr1=(cr1<0)?0:((cr1>255)?255:cr1);
			if(cr2&(~255))cr2=(cr2<0)?0:((cr2>255)?255:cr2);
			if(cr3&(~255))cr3=(cr3<0)?0:((cr3>255)?255:cr3);
		}

		if((cg0|cg1|cg2|cg3)&(~255))
		{
			if(cg0&(~255))cg0=(cg0<0)?0:((cg0>255)?255:cg0);
			if(cg1&(~255))cg1=(cg1<0)?0:((cg1>255)?255:cg1);
			if(cg2&(~255))cg2=(cg2<0)?0:((cg2>255)?255:cg2);
			if(cg3&(~255))cg3=(cg3<0)?0:((cg3>255)?255:cg3);
		}

		if((cb0|cb1|cb2|cb3)&(~255))
		{
			if(cb0&(~255))cb0=(cb0<0)?0:((cb0>255)?255:cb0);
			if(cb1&(~255))cb1=(cb1<0)?0:((cb1>255)?255:cb1);
			if(cb2&(~255))cb2=(cb2<0)?0:((cb2>255)?255:cb2);
			if(cb3&(~255))cb3=(cb3<0)?0:((cb3>255)?255:cb3);
		}
#endif

		if(ctx->flip)
		{
//			l=((y*xs)+x)*4;
			l0=(((y+0)*xs)+x+0)*4;
			l2=(((y+1)*xs)+x+0)*4;
		}
		else
		{
//			l=(((ys-1-y)*xs)+x)*4;
			k=ys-1-y;
			l0=((k*xs)+x+0)*4;
			k=ys-2-y;
			l2=((k*xs)+x+0)*4;
		}

//		k=ys-1-y;
//		l0=((k*xs)+x+0)*4;
//		k=ys-2-y;
//		l2=((k*xs)+x+0)*4;

		ob0=obuf+l0; ob1=obuf+l2;

		ob0[0]=cb0; ob0[1]=cg0; ob0[2]=cr0; ob0[3]=ca;
		ob0[4]=cb1; ob0[5]=cg1; ob0[6]=cr1; ob0[7]=ca;
		ob1[0]=cb2; ob1[1]=cg2; ob1[2]=cr2; ob1[3]=ca;
		ob1[4]=cb3; ob1[5]=cg3; ob1[6]=cr3; ob1[7]=ca;
	}
#endif
}

void BTIC2C_DecodeColorTransformYCgCo420_Block_UYVY(
	BTIC2C_Context *ctx, byte *obuf, int xo, int yo, int clrs)
{
	s16 *si0, *si1, *si2, *si3;
	byte *ob0, *ob1;
	int t0, t1, w0, w1, n0, n1, x1, y1;
	int al, x, y, xs, ys;
	int cy, cu, cv, cr, cg, cb, ca;
	int cu1, cv1, cu2, cv2;
	int cy0, cy1, cy2, cy3;
	int cr0, cg0, cb0, ca0;
	int cr1, cg1, cb1, ca1;
	int cr2, cg2, cb2, ca2;
	int cr3, cg3, cb3, ca3;
	int l0, l1, l2, l3;
	int i, j, k, l;

#if 1
	w0=ctx->img_schs[0];
	w1=ctx->img_schs[1];

	si0=ctx->img_sjbuf[0];
	si1=ctx->img_sjbuf[1];
	si2=ctx->img_sjbuf[2];
	si3=ctx->img_sjbuf[3];
	xs=ctx->xs;
	ys=ctx->ys;

	for(i=0; i<16; i+=2)
		for(j=0; j<16; j+=2)
	{
		x=xo+j; y=yo+i;
		x1=x>>1; y1=y>>1;
		l0=x>>3; l1=y>>3;
		l2=x1>>3; l3=y1>>3;
		n0=(l1*w0)+l0;
		n1=(l3*w1)+l2;

		l0=x&7; l1=y&7;
		l2=x1&7; l3=y1&7;
		t0=(n0<<6)+((l1<<3)+l0);
		t1=(n1<<6)+((l3<<3)+l2);

		cy0=si0[t0  ];	cy1=si0[t0+1];
		cy2=si0[t0+8];	cy3=si0[t0+9];
		cu=si1[t1];		cv=si2[t1];
		ca=si3[t1];

#if 1
		l0=(cu>>1);
		cg0=cy0+l0; cg1=cy1+l0;
		cg2=cy2+l0;	cg3=cy3+l0;
		l1=cu+(cv>>1);
		cr0=cg0-l1;	cr1=cg1-l1;
		cr2=cg2-l1;	cr3=cg3-l1;
		l2=cu-(cv>>1);
		cb0=cg0-l2;	cb1=cg1-l2;
		cb2=cg2-l2;	cb3=cg3-l2;
#endif

		BT2C_CLRTRANS_SIMPLECLAMP255x4B(cr0, cr1, cr2, cr3);
		BT2C_CLRTRANS_SIMPLECLAMP255x4B(cg0, cg1, cg2, cg3);
		BT2C_CLRTRANS_SIMPLECLAMP255x4B(cb0, cb1, cb2, cb3);
		BT2C_CLRTRANS_SIMPLECLAMP255(ca);

		k=ys-1-y;
		l0=((k*xs)+x+0)*2;
		k=ys-2-y;
		l2=((k*xs)+x+0)*2;
		ob0=obuf+l0; ob1=obuf+l2;

		cu1=((cb0-cg0)>>1)+128;
		cv1=((cr1-cg1)>>1)+128;
		cu2=((cb2-cg2)>>1)+128;
		cv2=((cr3-cg3)>>1)+128;
		ob0[0]=cu1; ob0[1]=cy0; ob0[2]=cv1; ob0[3]=cy1;
		ob1[0]=cu2; ob1[1]=cy2; ob1[2]=cv2; ob1[3]=cy3;
	}
#endif
}

void BTIC2C_DecodeColorTransformRCT420_Block(
	BTIC2C_Context *ctx, byte *obuf, int xo, int yo, int clrs)
{
//	s32 *si0, *si1, *si2, *si3;
	s16 *si0, *si1, *si2, *si3;
	byte *ob0, *ob1;
	int t0, t1, w0, w1, n0, n1, x1, y1;
	int al, x, y;
	int cy, cu, cv, cr, cg, cb, ca, cu1, cv1;
	int cy0, cy1, cy2, cy3;
	int cr0, cg0, cb0, ca0;
	int cr1, cg1, cb1, ca1;
	int cr2, cg2, cb2, ca2;
	int cr3, cg3, cb3, ca3;
	int l0, l1, l2, l3;
	int i, j, k, l;

	w0=ctx->img_schs[0];
	w1=ctx->img_schs[1];
//	si0=ctx->img_sibuf[0];
//	si1=ctx->img_sibuf[1];
//	si2=ctx->img_sibuf[2];
//	si3=ctx->img_sibuf[3];
	si0=ctx->img_sjbuf[0];
	si1=ctx->img_sjbuf[1];
	si2=ctx->img_sjbuf[2];
	si3=ctx->img_sjbuf[3];
	al=(ctx->img_alpha);

	for(i=0; i<16; i+=2)
		for(j=0; j<16; j+=2)
	{
		x=xo+j; y=yo+i;
		x1=x>>1; y1=y>>1;
		l0=x>>3; l1=y>>3;
		l2=x1>>3; l3=y1>>3;
		n0=(l1*w0)+l0;
		n1=(l3*w1)+l2;

		l0=x&7; l1=y&7;
		l2=x1&7; l3=y1&7;
		t0=(n0<<6)+((l1<<3)+l0);
		t1=(n1<<6)+((l3<<3)+l2);

		cy0=si0[t0  ];	cy1=si0[t0+1];
		cy2=si0[t0+8];	cy3=si0[t0+9];
		cu=si1[t1];		cv=si2[t1];
		ca=al?si3[t1]:255;

		l0=(cu+cv)>>2;
		cg0=cy0-l0;		cg1=cy1-l0;
		cg2=cy2-l0;		cg3=cy3-l0;
		cb0=cg0+cu;		cb1=cg1+cu;
		cb2=cg2+cu;		cb3=cg3+cu;
		cr0=cg0+cv;		cr1=cg1+cv;
		cr2=cg2+cv;		cr3=cg3+cv;

		if((clrs==BGBBTJ_JPG_RGBA) || (clrs==BGBBTJ_JPG_BGRA) ||
			(clrs==BGBBTJ_JPG_RGBX) || (clrs==BGBBTJ_JPG_BGRX))
		{
			if((cr0|cr1|cr2|cr3)&(~255))
			{
				if(cr0&(~255))cr0=(cr0<0)?0:((cr0>255)?255:cr0);
				if(cr1&(~255))cr1=(cr1<0)?0:((cr1>255)?255:cr1);
				if(cr2&(~255))cr2=(cr2<0)?0:((cr2>255)?255:cr2);
				if(cr3&(~255))cr3=(cr3<0)?0:((cr3>255)?255:cr3);
			}

			if((cg0|cg1|cg2|cg3)&(~255))
			{
				if(cg0&(~255))cg0=(cg0<0)?0:((cg0>255)?255:cg0);
				if(cg1&(~255))cg1=(cg1<0)?0:((cg1>255)?255:cg1);
				if(cg2&(~255))cg2=(cg2<0)?0:((cg2>255)?255:cg2);
				if(cg3&(~255))cg3=(cg3<0)?0:((cg3>255)?255:cg3);
			}

			if((cb0|cb1|cb2|cb3)&(~255))
			{
				if(cb0&(~255))cb0=(cb0<0)?0:((cb0>255)?255:cb0);
				if(cb1&(~255))cb1=(cb1<0)?0:((cb1>255)?255:cb1);
				if(cb2&(~255))cb2=(cb2<0)?0:((cb2>255)?255:cb2);
				if(cb3&(~255))cb3=(cb3<0)?0:((cb3>255)?255:cb3);
			}
		}

		k=ctx->ys-1-y;
		l0=((k*ctx->xs)+x+0)*4;
		k=ctx->ys-2-y;
		l2=((k*ctx->xs)+x+0)*4;
		ob0=obuf+l0; ob1=obuf+l2;

		if((clrs==BGBBTJ_JPG_RGBA) || (clrs==BGBBTJ_JPG_RGBX))
		{
			ob0[0]=cr0; ob0[1]=cg0; ob0[2]=cb0; ob0[3]=ca;
			ob0[4]=cr1; ob0[5]=cg1; ob0[6]=cb1; ob0[7]=ca;
			ob1[0]=cr2; ob1[1]=cg2; ob1[2]=cb2; ob1[3]=ca;
			ob1[4]=cr3; ob1[5]=cg3; ob1[6]=cb3; ob1[7]=ca;
		}else if((clrs==BGBBTJ_JPG_BGRA) || (clrs==BGBBTJ_JPG_BGRX))
		{
			ob0[0]=cb0; ob0[1]=cg0; ob0[2]=cr0; ob0[3]=ca;
			ob0[4]=cb1; ob0[5]=cg1; ob0[6]=cr1; ob0[7]=ca;
			ob1[0]=cb2; ob1[1]=cg2; ob1[2]=cr2; ob1[3]=ca;
			ob1[4]=cb3; ob1[5]=cg3; ob1[6]=cr3; ob1[7]=ca;
		}
	}
}

void BTIC2C_DecodeColorTransformRCT420A_Block_RGBA(
	BTIC2C_Context *ctx, byte *obuf, int xo, int yo, int clrs)
{
//	s32 *si0, *si1, *si2, *si3;
	s16 *si0, *si1, *si2, *si3;
	byte *ob0, *ob1;
	int t0, t1, w0, w1, n0, n1, x1, y1;
	int al, x, y;
	int cy, cu, cv, cr, cg, cb, ca;
	int cy0, cy1, cy2, cy3;
	int cr0, cg0, cb0, ca0;
	int cr1, cg1, cb1, ca1;
	int cr2, cg2, cb2, ca2;
	int cr3, cg3, cb3, ca3;
	int l0, l1, l2, l3;
	int i, j, k, l;

#if 1
	w0=ctx->img_schs[0];
	w1=ctx->img_schs[1];
//	si0=ctx->img_sibuf[0];
//	si1=ctx->img_sibuf[1];
//	si2=ctx->img_sibuf[2];
//	si3=ctx->img_sibuf[3];
	si0=ctx->img_sjbuf[0];
	si1=ctx->img_sjbuf[1];
	si2=ctx->img_sjbuf[2];
	si3=ctx->img_sjbuf[3];

	for(i=0; i<16; i+=2)
		for(j=0; j<16; j+=2)
	{
		x=xo+j; y=yo+i;
		x1=x>>1; y1=y>>1;
		l0=x>>3; l1=y>>3;
		l2=x1>>3; l3=y1>>3;
		n0=(l1*w0)+l0;
		n1=(l3*w1)+l2;

		l0=x&7; l1=y&7;
		l2=x1&7; l3=y1&7;
		t0=(n0<<6)+((l1<<3)+l0);
		t1=(n1<<6)+((l3<<3)+l2);

		cy0=si0[t0  ];	cy1=si0[t0+1];
		cy2=si0[t0+8];	cy3=si0[t0+9];
		cu=si1[t1];		cv=si2[t1];
		ca=si3[t1];

		l0=(cu+cv)>>2;
		cg0=cy0-l0;		cg1=cy1-l0;
		cg2=cy2-l0;		cg3=cy3-l0;
		cb0=cg0+cu;		cb1=cg1+cu;
		cb2=cg2+cu;		cb3=cg3+cu;
		cr0=cg0+cv;		cr1=cg1+cv;
		cr2=cg2+cv;		cr3=cg3+cv;

#if 1
		if((cr0|cr1|cr2|cr3)&(~255))
		{
			if(cr0&(~255))cr0=(cr0<0)?0:((cr0>255)?255:cr0);
			if(cr1&(~255))cr1=(cr1<0)?0:((cr1>255)?255:cr1);
			if(cr2&(~255))cr2=(cr2<0)?0:((cr2>255)?255:cr2);
			if(cr3&(~255))cr3=(cr3<0)?0:((cr3>255)?255:cr3);
		}

		if((cg0|cg1|cg2|cg3)&(~255))
		{
			if(cg0&(~255))cg0=(cg0<0)?0:((cg0>255)?255:cg0);
			if(cg1&(~255))cg1=(cg1<0)?0:((cg1>255)?255:cg1);
			if(cg2&(~255))cg2=(cg2<0)?0:((cg2>255)?255:cg2);
			if(cg3&(~255))cg3=(cg3<0)?0:((cg3>255)?255:cg3);
		}

		if((cb0|cb1|cb2|cb3)&(~255))
		{
			if(cb0&(~255))cb0=(cb0<0)?0:((cb0>255)?255:cb0);
			if(cb1&(~255))cb1=(cb1<0)?0:((cb1>255)?255:cb1);
			if(cb2&(~255))cb2=(cb2<0)?0:((cb2>255)?255:cb2);
			if(cb3&(~255))cb3=(cb3<0)?0:((cb3>255)?255:cb3);
		}
#endif

		k=ctx->ys-1-y;
		l0=((k*ctx->xs)+x+0)*4;
		k=ctx->ys-2-y;
		l2=((k*ctx->xs)+x+0)*4;
		ob0=obuf+l0; ob1=obuf+l2;

		ob0[0]=cr0; ob0[1]=cg0; ob0[2]=cb0; ob0[3]=ca;
		ob0[4]=cr1; ob0[5]=cg1; ob0[6]=cb1; ob0[7]=ca;
		ob1[0]=cr2; ob1[1]=cg2; ob1[2]=cb2; ob1[3]=ca;
		ob1[4]=cr3; ob1[5]=cg3; ob1[6]=cb3; ob1[7]=ca;
	}
#endif
}

void BTIC2C_DecodeColorTransformRCT420A_Block_BGRA(
	BTIC2C_Context *ctx, byte *obuf, int xo, int yo, int clrs)
{
//	s32 *si0, *si1, *si2, *si3;
	s16 *si0, *si1, *si2, *si3;
	byte *ob0, *ob1;
	int t0, t1, w0, w1, n0, n1, x1, y1;
	int al, x, y;
	int cy, cu, cv, cr, cg, cb, ca;
	int cy0, cy1, cy2, cy3;
	int cr0, cg0, cb0, ca0;
	int cr1, cg1, cb1, ca1;
	int cr2, cg2, cb2, ca2;
	int cr3, cg3, cb3, ca3;
	int l0, l1, l2, l3;
	int i, j, k, l;

#if 1
	w0=ctx->img_schs[0];
	w1=ctx->img_schs[1];
//	si0=ctx->img_sibuf[0];
//	si1=ctx->img_sibuf[1];
//	si2=ctx->img_sibuf[2];
//	si3=ctx->img_sibuf[3];
	si0=ctx->img_sjbuf[0];
	si1=ctx->img_sjbuf[1];
	si2=ctx->img_sjbuf[2];
	si3=ctx->img_sjbuf[3];

	for(i=0; i<16; i+=2)
		for(j=0; j<16; j+=2)
	{
		x=xo+j; y=yo+i;
		x1=x>>1; y1=y>>1;
		l0=x>>3; l1=y>>3;
		l2=x1>>3; l3=y1>>3;
		n0=(l1*w0)+l0;
		n1=(l3*w1)+l2;

		l0=x&7; l1=y&7;
		l2=x1&7; l3=y1&7;
		t0=(n0<<6)+((l1<<3)+l0);
		t1=(n1<<6)+((l3<<3)+l2);

		cy0=si0[t0  ];	cy1=si0[t0+1];
		cy2=si0[t0+8];	cy3=si0[t0+9];
		cu=si1[t1];		cv=si2[t1];
		ca=si3[t1];

		l0=(cu+cv)>>2;
		cg0=cy0-l0;		cg1=cy1-l0;
		cg2=cy2-l0;		cg3=cy3-l0;
		cb0=cg0+cu;		cb1=cg1+cu;
		cb2=cg2+cu;		cb3=cg3+cu;
		cr0=cg0+cv;		cr1=cg1+cv;
		cr2=cg2+cv;		cr3=cg3+cv;

#if 1
		if((cr0|cr1|cr2|cr3)&(~255))
		{
			if(cr0&(~255))cr0=(cr0<0)?0:((cr0>255)?255:cr0);
			if(cr1&(~255))cr1=(cr1<0)?0:((cr1>255)?255:cr1);
			if(cr2&(~255))cr2=(cr2<0)?0:((cr2>255)?255:cr2);
			if(cr3&(~255))cr3=(cr3<0)?0:((cr3>255)?255:cr3);
		}

		if((cg0|cg1|cg2|cg3)&(~255))
		{
			if(cg0&(~255))cg0=(cg0<0)?0:((cg0>255)?255:cg0);
			if(cg1&(~255))cg1=(cg1<0)?0:((cg1>255)?255:cg1);
			if(cg2&(~255))cg2=(cg2<0)?0:((cg2>255)?255:cg2);
			if(cg3&(~255))cg3=(cg3<0)?0:((cg3>255)?255:cg3);
		}

		if((cb0|cb1|cb2|cb3)&(~255))
		{
			if(cb0&(~255))cb0=(cb0<0)?0:((cb0>255)?255:cb0);
			if(cb1&(~255))cb1=(cb1<0)?0:((cb1>255)?255:cb1);
			if(cb2&(~255))cb2=(cb2<0)?0:((cb2>255)?255:cb2);
			if(cb3&(~255))cb3=(cb3<0)?0:((cb3>255)?255:cb3);
		}
#endif

		k=ctx->ys-1-y;
		l0=((k*ctx->xs)+x+0)*4;
		k=ctx->ys-2-y;
		l2=((k*ctx->xs)+x+0)*4;
		ob0=obuf+l0; ob1=obuf+l2;

		ob0[0]=cb0; ob0[1]=cg0; ob0[2]=cr0; ob0[3]=ca;
		ob0[4]=cb1; ob0[5]=cg1; ob0[6]=cr1; ob0[7]=ca;
		ob1[0]=cb2; ob1[1]=cg2; ob1[2]=cr2; ob1[3]=ca;
		ob1[4]=cb3; ob1[5]=cg3; ob1[6]=cr3; ob1[7]=ca;
	}
#endif
}

void BTIC2C_DecodeColorTransformRCT420_Block_UYVY(
	BTIC2C_Context *ctx, byte *obuf, int xo, int yo, int clrs)
{
	s16 *si0, *si1, *si2, *si3;
	byte *ob0, *ob1;
	int t0, t1, w0, w1, n0, n1, x1, y1;
	int al, x, y, xs, ys;
	int cy, cu, cv, cr, cg, cb, ca;
	int cu1, cv1, cu2, cv2;
	int cy0, cy1, cy2, cy3;
	int cr0, cg0, cb0, ca0;
	int cr1, cg1, cb1, ca1;
	int cr2, cg2, cb2, ca2;
	int cr3, cg3, cb3, ca3;
	int l0, l1, l2, l3;
	int i, j, k, l;

#if 1
	w0=ctx->img_schs[0];
	w1=ctx->img_schs[1];

	si0=ctx->img_sjbuf[0];
	si1=ctx->img_sjbuf[1];
	si2=ctx->img_sjbuf[2];
	si3=ctx->img_sjbuf[3];
	xs=ctx->xs;
	ys=ctx->ys;

	for(i=0; i<16; i+=2)
		for(j=0; j<16; j+=2)
	{
		x=xo+j; y=yo+i;
		x1=x>>1; y1=y>>1;
		l0=x>>3; l1=y>>3;
		l2=x1>>3; l3=y1>>3;
		n0=(l1*w0)+l0;
		n1=(l3*w1)+l2;

		l0=x&7; l1=y&7;
		l2=x1&7; l3=y1&7;
		t0=(n0<<6)+((l1<<3)+l0);
		t1=(n1<<6)+((l3<<3)+l2);

		cy0=si0[t0  ];	cy1=si0[t0+1];
		cy2=si0[t0+8];	cy3=si0[t0+9];
		cu=si1[t1];		cv=si2[t1];
		cu=(cu>>1)+128;	cv=(cv>>1)+128;

		BT2C_CLRTRANS_SIMPLECLAMP255x4B(cy0, cy1, cy2, cy3);
		BT2C_CLRTRANS_SIMPLECLAMP255(cu);
		BT2C_CLRTRANS_SIMPLECLAMP255(cv);

		l0=(((ys-1-y)*xs)+x+0)*2;
		l2=(((ys-2-y)*xs)+x+0)*2;
		ob0=obuf+l0;		ob1=obuf+l2;
		ob0[0]=cu; ob0[1]=cy0; ob0[2]=cv; ob0[3]=cy1;
		ob1[0]=cu; ob1[1]=cy2; ob1[2]=cv; ob1[3]=cy3;
	}
#endif
}


void BTIC2C_DecodeColorTransformYCbCr420_Block(
	BTIC2C_Context *ctx, byte *obuf, int xo, int yo, int clrs)
{
	s16 *si0, *si1, *si2, *si3;
	byte *ob0, *ob1;
	int t0, t1, w0, w1, n0, n1, x1, y1;
	int al, x, y;
	int cy, cu, cv, cr, cg, cb, ca, cu1, cv1;
	int cy0, cy1, cy2, cy3;
	int cr0, cg0, cb0, ca0;
	int cr1, cg1, cb1, ca1;
	int cr2, cg2, cb2, ca2;
	int cr3, cg3, cb3, ca3;
	int l0, l1, l2, l3;
	int i, j, k, l;

	w0=ctx->img_schs[0];
	w1=ctx->img_schs[1];
	si0=ctx->img_sjbuf[0];
	si1=ctx->img_sjbuf[1];
	si2=ctx->img_sjbuf[2];
	si3=ctx->img_sjbuf[3];
	al=(ctx->img_alpha);

	for(i=0; i<16; i+=2)
		for(j=0; j<16; j+=2)
	{
		x=xo+j; y=yo+i;
		x1=x>>1; y1=y>>1;
		l0=x>>3; l1=y>>3;
		l2=x1>>3; l3=y1>>3;
		n0=(l1*w0)+l0;
		n1=(l3*w1)+l2;

		l0=x&7; l1=y&7;
		l2=x1&7; l3=y1&7;
		t0=(n0<<6)+((l1<<3)+l0);
		t1=(n1<<6)+((l3<<3)+l2);

		cy0=si0[t0  ];	cy1=si0[t0+1];
		cy2=si0[t0+8];	cy3=si0[t0+9];
		cu=si1[t1];		cv=si2[t1];
		ca=al?si3[t1]:255;

//		l0=(cu+cv)>>2;
//		cg0=cy0-l0;		cg1=cy1-l0;
//		cg2=cy2-l0;		cg3=cy3-l0;
//		cb0=cg0+cu;		cb1=cg1+cu;
//		cb2=cg2+cu;		cb3=cg3+cu;
//		cr0=cg0+cv;		cr1=cg1+cv;
//		cr2=cg2+cv;		cr3=cg3+cv;

//		cu1=cu-128; cv1=cv-128;
//		l0=91881*cv1;
//		l1=-22554*cu1-46802*cv1;
//		l2=116130*cu1;
		l0=359*cv;
		l1=-88*cu-183*cv;
		l2=454*cu;
		ca0=(cy0<<8)+128;		ca1=(cy1<<8)+128;
		ca2=(cy2<<8)+128;		ca3=(cy3<<8)+128;
		cr0=(ca0+l0)>>8;		cg0=(ca0+l1)>>8;
		cb0=(ca0+l2)>>8;		cr1=(ca1+l0)>>8;
		cg1=(ca1+l1)>>8;		cb1=(ca1+l2)>>8;
		cr2=(ca2+l0)>>8;		cg2=(ca2+l1)>>8;
		cb2=(ca2+l2)>>8;		cr3=(ca3+l0)>>8;
		cg3=(ca3+l1)>>8;		cb3=(ca3+l2)>>8;

		if((clrs==BGBBTJ_JPG_RGBA) || (clrs==BGBBTJ_JPG_BGRA) ||
			(clrs==BGBBTJ_JPG_RGBX) || (clrs==BGBBTJ_JPG_BGRX))
		{
			if((cr0|cr1|cr2|cr3)&(~255))
			{
				if(cr0&(~255))cr0=(cr0<0)?0:((cr0>255)?255:cr0);
				if(cr1&(~255))cr1=(cr1<0)?0:((cr1>255)?255:cr1);
				if(cr2&(~255))cr2=(cr2<0)?0:((cr2>255)?255:cr2);
				if(cr3&(~255))cr3=(cr3<0)?0:((cr3>255)?255:cr3);
			}

			if((cg0|cg1|cg2|cg3)&(~255))
			{
				if(cg0&(~255))cg0=(cg0<0)?0:((cg0>255)?255:cg0);
				if(cg1&(~255))cg1=(cg1<0)?0:((cg1>255)?255:cg1);
				if(cg2&(~255))cg2=(cg2<0)?0:((cg2>255)?255:cg2);
				if(cg3&(~255))cg3=(cg3<0)?0:((cg3>255)?255:cg3);
			}

			if((cb0|cb1|cb2|cb3)&(~255))
			{
				if(cb0&(~255))cb0=(cb0<0)?0:((cb0>255)?255:cb0);
				if(cb1&(~255))cb1=(cb1<0)?0:((cb1>255)?255:cb1);
				if(cb2&(~255))cb2=(cb2<0)?0:((cb2>255)?255:cb2);
				if(cb3&(~255))cb3=(cb3<0)?0:((cb3>255)?255:cb3);
			}
		}

		if(ctx->flip)
		{
			l0=(((y+0)*ctx->xs)+x+0)*4;
			l2=(((y+1)*ctx->xs)+x+0)*4;
		}else
		{
			k=ctx->ys-1-y;
			l0=((k*ctx->xs)+x+0)*4;
			k=ctx->ys-2-y;
			l2=((k*ctx->xs)+x+0)*4;
		}
		ob0=obuf+l0; ob1=obuf+l2;

		if((clrs==BGBBTJ_JPG_RGBA) || (clrs==BGBBTJ_JPG_RGBX))
		{
			ob0[0]=cr0; ob0[1]=cg0; ob0[2]=cb0; ob0[3]=ca;
			ob0[4]=cr1; ob0[5]=cg1; ob0[6]=cb1; ob0[7]=ca;
			ob1[0]=cr2; ob1[1]=cg2; ob1[2]=cb2; ob1[3]=ca;
			ob1[4]=cr3; ob1[5]=cg3; ob1[6]=cb3; ob1[7]=ca;
		}else if((clrs==BGBBTJ_JPG_BGRA) || (clrs==BGBBTJ_JPG_BGRX))
		{
			ob0[0]=cb0; ob0[1]=cg0; ob0[2]=cr0; ob0[3]=ca;
			ob0[4]=cb1; ob0[5]=cg1; ob0[6]=cr1; ob0[7]=ca;
			ob1[0]=cb2; ob1[1]=cg2; ob1[2]=cr2; ob1[3]=ca;
			ob1[4]=cb3; ob1[5]=cg3; ob1[6]=cr3; ob1[7]=ca;
		}
	}
}

void BTIC2C_DecodeColorTransformYCbCr420_Block_UYVY(
	BTIC2C_Context *ctx, byte *obuf, int xo, int yo, int clrs)
{
	s16 *si0, *si1, *si2, *si3;
	byte *ob0, *ob1;
	int t0, t1, w0, w1, n0, n1, x1, y1;
	int al, x, y, xs, ys;
	int cy, cu, cv, cr, cg, cb, ca;
	int cu1, cv1, cu2, cv2;
	int cy0, cy1, cy2, cy3;
	int cr0, cg0, cb0, ca0;
	int cr1, cg1, cb1, ca1;
	int cr2, cg2, cb2, ca2;
	int cr3, cg3, cb3, ca3;
	int l0, l1, l2, l3;
	int i, j, k, l;

#if 1
	w0=ctx->img_schs[0];
	w1=ctx->img_schs[1];

	si0=ctx->img_sjbuf[0];
	si1=ctx->img_sjbuf[1];
	si2=ctx->img_sjbuf[2];
	si3=ctx->img_sjbuf[3];
	xs=ctx->xs;
	ys=ctx->ys;

	for(i=0; i<16; i+=2)
		for(j=0; j<16; j+=2)
	{
		x=xo+j; y=yo+i;
		x1=x>>1; y1=y>>1;
		l0=x>>3; l1=y>>3;
		l2=x1>>3; l3=y1>>3;
		n0=(l1*w0)+l0;
		n1=(l3*w1)+l2;

		l0=x&7; l1=y&7;
		l2=x1&7; l3=y1&7;
		t0=(n0<<6)+((l1<<3)+l0);
		t1=(n1<<6)+((l3<<3)+l2);

		cy0=si0[t0  ];	cy1=si0[t0+1];
		cy2=si0[t0+8];	cy3=si0[t0+9];
		cu=si1[t1]+128;	cv=si2[t1]+128;

		BT2C_CLRTRANS_SIMPLECLAMP255x4B(cy0, cy1, cy2, cy3);
		BT2C_CLRTRANS_SIMPLECLAMP255(cu);
		BT2C_CLRTRANS_SIMPLECLAMP255(cv);

		l0=(((ys-1-y)*xs)+x+0)*2;
		l2=(((ys-2-y)*xs)+x+0)*2;
		ob0=obuf+l0;		ob1=obuf+l2;
		ob0[0]=cu; ob0[1]=cy0; ob0[2]=cv; ob0[3]=cy1;
		ob1[0]=cu; ob1[1]=cy2; ob1[2]=cv; ob1[3]=cy3;
	}
#endif
}



int BTIC2C_DecodeColorTransformYCgCo420(
	BTIC2C_Context *ctx, byte *obuf, int clrs)
{
	int i, j;

	for(i=0; i<ctx->ys; i+=16)
		for(j=0; j<ctx->xs; j+=16)
	{
#if 0
		if((ctx->img_alpha) && (clrs==BGBBTJ_JPG_RGBA))
		{
			BTIC2C_DecodeColorTransformYCgCo420A_Block_RGBA(
				ctx, obuf, j, i, clrs);
			continue;
		}
#endif

		BTIC2C_DecodeColorTransformYCgCo420_Block(ctx, obuf, j, i, clrs);
	}
	return(0);
}
#endif

#if 0
int BTIC2C_DecodeColorTransformYCbCr420(
	BTIC2C_Context *ctx, byte *obuf, int clrs)
{
	s32 *si0, *si1, *si2, *si3;
	byte *ob0, *ob1;
	int t0, t1, w0, w1, n0, n1, x1, y1;
	int al, x, y;
	int cy, cu, cv, cr, cg, cb, ca, cu1, cv1;
	int cy0, cy1, cy2, cy3;
	int cr0, cg0, cb0, ca0;
	int cr1, cg1, cb1, ca1;
	int cr2, cg2, cb2, ca2;
	int cr3, cg3, cb3, ca3;
	int l0, l1, l2, l3;
	int i, j, k, l;

#if 1
	w0=ctx->img_schs[0];
	w1=ctx->img_schs[1];
	si0=ctx->img_sibuf[0];
	si1=ctx->img_sibuf[1];
	si2=ctx->img_sibuf[2];
	si3=ctx->img_sibuf[3];
	al=(ctx->img_alpha);

	for(i=0; i<ctx->ys; i+=2)
		for(j=0; j<ctx->xs; j+=2)
	{
		x=j; y=i;
		x1=x>>1; y1=y>>1;
		l0=x>>3; l1=y>>3;
		l2=x1>>3; l3=y1>>3;
		n0=(l1*w0)+l0;
		n1=(l3*w1)+l2;

		l0=x&7; l1=y&7;
		l2=x1&7; l3=y1&7;
		t0=(n0<<6)+((l1<<3)+l0);
		t1=(n1<<6)+((l3<<3)+l2);

		cy0=si0[t0  ];	cy1=si0[t0+1];
		cy2=si0[t0+8];	cy3=si0[t0+9];
		cu=si1[t1];		cv=si2[t1];
		ca=al?si3[t1]:255;

#if 1
		l0=(cu>>1);
		cg0=cy0+l0; cg1=cy1+l0;
		cg2=cy2+l0;	cg3=cy3+l0;
		l1=cu+(cv>>1);
		cr0=cg0-l1;	cr1=cg1-l1;
		cr2=cg2-l1;	cr3=cg3-l1;
		l2=cu-(cv>>1);
		cb0=cg0-l2;	cb1=cg1-l2;
		cb2=cg2-l2;	cb3=cg3-l2;
#endif

#if 1
		if((cr0|cr1|cr2|cr3)&(~255))
		{
			if(cr0&(~255))cr0=(cr0<0)?0:((cr0>255)?255:cr0);
			if(cr1&(~255))cr1=(cr1<0)?0:((cr1>255)?255:cr1);
			if(cr2&(~255))cr2=(cr2<0)?0:((cr2>255)?255:cr2);
			if(cr3&(~255))cr3=(cr3<0)?0:((cr3>255)?255:cr3);
		}

		if((cg0|cg1|cg2|cg3)&(~255))
		{
			if(cg0&(~255))cg0=(cg0<0)?0:((cg0>255)?255:cg0);
			if(cg1&(~255))cg1=(cg1<0)?0:((cg1>255)?255:cg1);
			if(cg2&(~255))cg2=(cg2<0)?0:((cg2>255)?255:cg2);
			if(cg3&(~255))cg3=(cg3<0)?0:((cg3>255)?255:cg3);
		}

		if((cb0|cb1|cb2|cb3)&(~255))
		{
			if(cb0&(~255))cb0=(cb0<0)?0:((cb0>255)?255:cb0);
			if(cb1&(~255))cb1=(cb1<0)?0:((cb1>255)?255:cb1);
			if(cb2&(~255))cb2=(cb2<0)?0:((cb2>255)?255:cb2);
			if(cb3&(~255))cb3=(cb3<0)?0:((cb3>255)?255:cb3);
		}
#endif

		k=ctx->ys-1-i;
		l0=((k*ctx->xs)+j+0)*4;
		k=ctx->ys-2-i;
		l2=((k*ctx->xs)+j+0)*4;
		ob0=obuf+l0; ob1=obuf+l2;

#if 1
		if(clrs==BGBBTJ_JPG_RGBA)
		{
			ob0[0]=cr0; ob0[1]=cg0; ob0[2]=cb0; ob0[3]=ca;
			ob0[4]=cr1; ob0[5]=cg1; ob0[6]=cb1; ob0[7]=ca;
			ob1[0]=cr2; ob1[1]=cg2; ob1[2]=cb2; ob1[3]=ca;
			ob1[4]=cr3; ob1[5]=cg3; ob1[6]=cb3; ob1[7]=ca;
		}else if(clrs==BGBBTJ_JPG_BGRA)
		{
			ob0[0]=cb0; ob0[1]=cg0; ob0[2]=cr0; ob0[3]=ca;
			ob0[4]=cb1; ob0[5]=cg1; ob0[6]=cr1; ob0[7]=ca;
			ob1[0]=cb2; ob1[1]=cg2; ob1[2]=cr2; ob1[3]=ca;
			ob1[4]=cb3; ob1[5]=cg3; ob1[6]=cr3; ob1[7]=ca;
		}
#endif
	}
#endif

	return(0);
}
#endif

#if 0
int BTIC2C_DecodeColorTransformYCbCr420(
	BTIC2C_Context *ctx, byte *obuf, int clrs)
{
	int y, u, v, r, g, b, a, u1, v1;
	int y0, y1, y2, y3;
	int r0, g0, b0, a0;
	int r1, g1, b1, a1;
	int r2, g2, b2, a2;
	int r3, g3, b3, a3;
	int l0, l1, l2, l3;
	int i, j, k, l;

#if 1
	for(i=0; i<ctx->ys; i+=2)
		for(j=0; j<ctx->xs; j+=2)
	{
		if(ctx->img_alpha)
		{
			BTIC2C_GetComponentPixelS420A(ctx, j, i,
				&y0, &y1, &y2, &y3, &u, &v, &a);
		}else
		{
			BTIC2C_GetComponentPixelS420(ctx, j, i,
				&y0, &y1, &y2, &y3, &u, &v);
			a=255;
		}

#if 0
		k=(u/2);
		g0=y0+k; 	g1=y1+k;
		g2=y2+k;	g3=y3+k;
		l=u+(v/2);
		r0=g0-l;	r1=g1-l;
		r2=g2-l;	r3=g3-l;
		l=u-(v/2);
		b0=g0-l;	b1=g1-l;
		b2=g2-l;	b3=g3-l;
#endif

#if 1
		k=(u>>1);
		g0=y0+k; 	g1=y1+k;
		g2=y2+k;	g3=y3+k;
		l=u+(v>>1);
		r0=g0-l;	r1=g1-l;
		r2=g2-l;	r3=g3-l;
		l=u-(v>>1);
		b0=g0-l;	b1=g1-l;
		b2=g2-l;	b3=g3-l;
#endif

#if 0
		u1=u-128; v1=v-128;
		l0=91881*v1;
		l1=-22554*u1-46802*v1;
		l2=116130*u1;

		y0<<=16; y1<<=16; y2<<=16; y3<<=16;
		r0=y0+l0; g0=y0+l1; b0=y0+l2;
		r1=y1+l0; g1=y1+l1; b1=y1+l2;
		r2=y2+l0; g2=y2+l1; b2=y2+l2;
		r3=y3+l0; g3=y3+l1; b3=y3+l2;

		r0>>=16; g0>>=16; b0>>=16;
		r1>>=16; g1>>=16; b1>>=16;
		r2>>=16; g2>>=16; b2>>=16;
		r3>>=16; g3>>=16; b3>>=16;
#endif

#if 1
		if((r0|r1|r2|r3)&(~255))
		{
			if(r0&(~255))r0=(r0<0)?0:((r0>255)?255:r0);
			if(r1&(~255))r1=(r1<0)?0:((r1>255)?255:r1);
			if(r2&(~255))r2=(r2<0)?0:((r2>255)?255:r2);
			if(r3&(~255))r3=(r3<0)?0:((r3>255)?255:r3);
		}

		if((g0|g1|g2|g3)&(~255))
		{
			if(g0&(~255))g0=(g0<0)?0:((g0>255)?255:g0);
			if(g1&(~255))g1=(g1<0)?0:((g1>255)?255:g1);
			if(g2&(~255))g2=(g2<0)?0:((g2>255)?255:g2);
			if(g3&(~255))g3=(g3<0)?0:((g3>255)?255:g3);
		}

		if((b0|b1|b2|b3)&(~255))
		{
			if(b0&(~255))b0=(b0<0)?0:((b0>255)?255:b0);
			if(b1&(~255))b1=(b1<0)?0:((b1>255)?255:b1);
			if(b2&(~255))b2=(b2<0)?0:((b2>255)?255:b2);
			if(b3&(~255))b3=(b3<0)?0:((b3>255)?255:b3);
		}
#endif

		k=ctx->ys-1-i;
		l0=((k*ctx->xs)+j+0)*4;
		k=ctx->ys-2-i;
		l2=((k*ctx->xs)+j+0)*4;

		if(clrs==BGBBTJ_JPG_RGBA)
		{
			obuf[l0+0]=r0; obuf[l0+1]=g0; obuf[l0+2]=b0; obuf[l0+3]=a;
			obuf[l0+4]=r1; obuf[l0+5]=g1; obuf[l0+6]=b1; obuf[l0+7]=a;
			obuf[l2+0]=r2; obuf[l2+1]=g2; obuf[l2+2]=b2; obuf[l2+3]=a;
			obuf[l2+4]=r3; obuf[l2+5]=g3; obuf[l2+6]=b3; obuf[l2+7]=a;
		}else if(clrs==BGBBTJ_JPG_BGRA)
		{
			obuf[l0+0]=b0; obuf[l0+1]=g0; obuf[l0+2]=r0; obuf[l0+3]=a;
			obuf[l0+4]=b1; obuf[l0+5]=g1; obuf[l0+6]=r1; obuf[l0+7]=a;
			obuf[l2+0]=b2; obuf[l2+1]=g2; obuf[l2+2]=r2; obuf[l2+3]=a;
			obuf[l2+4]=b3; obuf[l2+5]=g3; obuf[l2+6]=r3; obuf[l2+7]=a;
		}
	}
#endif

	return(0);
}
#endif

int BTIC2C_DecodeColorTransform(BTIC2C_Context *ctx,
	byte *obuf, int clrs)
{
	int y, u, v, r, g, b, a;
	int i, j, k, l;

	if(ctx->img_is420)
	{
		BTIC2C_DecodeColorTransformYCgCo420(ctx, obuf, clrs);
		return(0);
	}

	for(i=0; i<ctx->ys; i++)
		for(j=0; j<ctx->xs; j++)
	{
		if(ctx->img_nc==4)
		{
			y=BTIC2C_GetComponentPixel(ctx, 0, j, i);
			u=BTIC2C_GetComponentPixel(ctx, 1, j, i);
			v=BTIC2C_GetComponentPixel(ctx, 2, j, i);
			a=BTIC2C_GetComponentPixel(ctx, 3, j, i);

//			r=y+1.40200*(v-128);
//			g=y-0.34414*(u-128)-0.71414*(v-128);
//			b=y+1.77200*(u-128);

//			u=0; v=0;

			k=(u>>1);
			l=(v>>1);
			g=y+k;
			r=g-u-l;
			b=g-u+l;

			r=(r<0)?0:((r>255)?255:r);
			g=(g<0)?0:((g>255)?255:g);
			b=(b<0)?0:((b>255)?255:b);
		}else if(ctx->img_nc==3)
		{
			y=BTIC2C_GetComponentPixel(ctx, 0, j, i);
			u=BTIC2C_GetComponentPixel(ctx, 1, j, i);
			v=BTIC2C_GetComponentPixel(ctx, 2, j, i);
			a=255;

//			r=y+1.40200*(v-128);
//			g=y-0.34414*(u-128)-0.71414*(v-128);
//			b=y+1.77200*(u-128);

			k=(u>>1);
			l=(v>>1);
			g=y+k;
			r=g-u-l;
			b=g-u+l;

			r=(r<0)?0:((r>255)?255:r);
			g=(g<0)?0:((g>255)?255:g);
			b=(b<0)?0:((b>255)?255:b);
		}else if(ctx->img_nc==1)
		{
			y=BTIC2C_GetComponentPixel(ctx, 0, j, i);
			r=y; g=y; b=y; a=255;
		}

		if(ctx->flip)
			{ l=((i*ctx->xs)+j)*4; }
		else
		{
			k=ctx->ys-1-i;
			l=((k*ctx->xs)+j)*4;
		}

		if(clrs==BGBBTJ_JPG_RGBA)
		{
			obuf[l+0]=r;	obuf[l+1]=g;
			obuf[l+2]=b;	obuf[l+3]=a;
		}else if(clrs==BGBBTJ_JPG_BGRA)
		{
			obuf[l+0]=b;	obuf[l+1]=g;
			obuf[l+2]=r;	obuf[l+3]=a;
		}
	}
	return(0);
}
