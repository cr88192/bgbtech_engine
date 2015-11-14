#include <bgbbtj.h>

int BTIC2C_GetComponentPixel(BTIC2C_Context *ctx, int cn, int x, int y)
{
	int i, j, k, l;
	int id, w, h, n, v;

	i=(x*ctx->img_schsci[cn])>>16;
	j=(y*ctx->img_scvsci[cn])>>16;
	k=i>>3;
	l=j>>3;

	w=ctx->img_schs[cn];
	n=(l*w)+k;

	k=i&7;
	l=j&7;

	if(ctx->img_sjbuf[cn])
		{ v=ctx->img_sjbuf[cn][n*64+(l*8+k)]; }
	else
		{ v=ctx->img_sibuf[cn][n*64+(l*8+k)]; }
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

			if(ctx->img_alpha)
			{
				if((clrs==BGBBTJ_JPG_RGBA) || (clrs==BGBBTJ_JPG_RGBX))
				{
					BTIC2C_DecodeColorTransformYCbCr420_Block(
						ctx, obuf, xo, yo, clrs);
//					BTIC2C_DecodeColorTransformYCbCr420_Block_RGBA(
//						ctx, obuf, xo, yo, clrs);
					return;
				}else if((clrs==BGBBTJ_JPG_BGRA) || (clrs==BGBBTJ_JPG_BGRX))
				{
					BTIC2C_DecodeColorTransformYCbCr420_Block(
						ctx, obuf, xo, yo, clrs);
//					BTIC2C_DecodeColorTransformYCbCr420_Block_BGRA(
//						ctx, obuf, xo, yo, clrs);
					return;
				}
			}else
			{
				if((clrs==BGBBTJ_JPG_RGBA) || (clrs==BGBBTJ_JPG_RGBX))
				{
					BTIC2C_DecodeColorTransformYCbCr420_Block_RGBX(
						ctx, obuf, xo, yo, clrs);
					return;
				}else if((clrs==BGBBTJ_JPG_BGRA) || (clrs==BGBBTJ_JPG_BGRX))
				{
					BTIC2C_DecodeColorTransformYCbCr420_Block_BGRX(
						ctx, obuf, xo, yo, clrs);
					return;
				}
			}

//			BTIC2C_DecodeColorTransformYCbCr420_Block(
//				ctx, obuf, xo, yo, clrs);
//			return;
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
		}else if(clrs==BGBBTJ_JPG_RGB)
		{		
			if(ctx->flip)
				{ l=((y*xs)+x)*3; }
			else
				{ l=(((ys-1-y)*xs)+x)*3; }
			cr=(cr<0)?0:((cr>255)?255:cr);
			cg=(cg<0)?0:((cg>255)?255:cg);
			cb=(cb<0)?0:((cb>255)?255:cb);
			obuf[l+0]=cr; obuf[l+1]=cg; obuf[l+2]=cb;
		}else if(clrs==BGBBTJ_JPG_BGR)
		{
			if(ctx->flip)
				{ l=((y*xs)+x)*3; }
			else
				{ l=(((ys-1-y)*xs)+x)*3; }
			cr=(cr<0)?0:((cr>255)?255:cr);
			cg=(cg<0)?0:((cg>255)?255:cg);
			cb=(cb<0)?0:((cb>255)?255:cb);
			obuf[l+0]=cb; obuf[l+1]=cg; obuf[l+2]=cr;
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
			{ cr=255; cg=0; cb=255; ca=255; }
		else
			{ cr=0; cg=255; cb=255; ca=255; }

		if(ctx->flip)
			{ l=((y*ctx->xs)+x)*4; }
		else
			{ l=(((ctx->ys-1-y)*ctx->xs)+x)*4; }

		if(clrs==BGBBTJ_JPG_RGBA)
		{
			obuf[l+0]=cr;	obuf[l+1]=cg;
			obuf[l+2]=cb;	obuf[l+3]=ca;
		}else if(clrs==BGBBTJ_JPG_BGRA)
		{
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

#if 1
		if((clrs==BGBBTJ_JPG_RGBA) || (clrs==BGBBTJ_JPG_RGBX))
		{
			if(ctx->flip)
				{	l0=(((y+0)*xs)+x+0)*4;
					l2=(((y+1)*xs)+x+0)*4; }
			else
				{	l0=(((ys-1-y)*xs)+x+0)*4;
					l2=(((ys-2-y)*xs)+x+0)*4; }

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
		l0=(cu>>1); l1=(cv>>1);
		l2=cu+l1;	l3=cu-l1;
		cg0=cy0+l0; cg1=cy1+l0;
		cg2=cy2+l0;	cg3=cy3+l0;
		cr0=cg0-l2;	cr1=cg1-l2;
		cr2=cg2-l2;	cr3=cg3-l2;
		cb0=cg0-l3;	cb1=cg1-l3;
		cb2=cg2-l3;	cb3=cg3-l3;
#endif

		BT2C_CLRTRANS_SIMPLECLAMP255x4B(cr0, cr1, cr2, cr3);
		BT2C_CLRTRANS_SIMPLECLAMP255x4B(cg0, cg1, cg2, cg3);
		BT2C_CLRTRANS_SIMPLECLAMP255x4B(cb0, cb1, cb2, cb3);
		BT2C_CLRTRANS_SIMPLECLAMP255(ca);

		if(ctx->flip)
		{
			l0=(((y+0)*xs)+x+0)*4;
			l2=(((y+1)*xs)+x+0)*4;
		}
		else
		{
			l0=(((ys-1-y)*xs)+x+0)*4;
			l2=(((ys-2-y)*xs)+x+0)*4;
		}

		ob0=obuf+l0; ob1=obuf+l2;

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

		if(ctx->flip)
		{
			l0=(((y+0)*xs)+x+0)*4;
			l2=(((y+1)*xs)+x+0)*4;
		}
		else
		{
			l0=(((ys-1-y)*xs)+x+0)*4;
			l2=(((ys-2-y)*xs)+x+0)*4;
		}

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
			BT2C_CLRTRANS_SIMPLECLAMP255x4B(cr0, cr1, cr2, cr3);
			BT2C_CLRTRANS_SIMPLECLAMP255x4B(cg0, cg1, cg2, cg3);
			BT2C_CLRTRANS_SIMPLECLAMP255x4B(cb0, cb1, cb2, cb3);
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

		BT2C_CLRTRANS_SIMPLECLAMP255x4B(cr0, cr1, cr2, cr3);
		BT2C_CLRTRANS_SIMPLECLAMP255x4B(cg0, cg1, cg2, cg3);
		BT2C_CLRTRANS_SIMPLECLAMP255x4B(cb0, cb1, cb2, cb3);

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

		BT2C_CLRTRANS_SIMPLECLAMP255x4B(cr0, cr1, cr2, cr3);
		BT2C_CLRTRANS_SIMPLECLAMP255x4B(cg0, cg1, cg2, cg3);
		BT2C_CLRTRANS_SIMPLECLAMP255x4B(cb0, cb1, cb2, cb3);

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
	int l0, l1, l2, l3;
	int i, j, k, l;

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
			BT2C_CLRTRANS_SIMPLECLAMP255x4B(cr0, cr1, cr2, cr3);
			BT2C_CLRTRANS_SIMPLECLAMP255x4B(cg0, cg1, cg2, cg3);
			BT2C_CLRTRANS_SIMPLECLAMP255x4B(cb0, cb1, cb2, cb3);
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

void BTIC2C_DecodeColorTransformYCbCr420_Block_RGBX(
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
//		ca=al?si3[t1]:255;
		ca=255;

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
			BT2C_CLRTRANS_SIMPLECLAMP255x4B(cr0, cr1, cr2, cr3);
			BT2C_CLRTRANS_SIMPLECLAMP255x4B(cg0, cg1, cg2, cg3);
			BT2C_CLRTRANS_SIMPLECLAMP255x4B(cb0, cb1, cb2, cb3);
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

		ob0[0]=cr0; ob0[1]=cg0; ob0[2]=cb0; ob0[3]=ca;
		ob0[4]=cr1; ob0[5]=cg1; ob0[6]=cb1; ob0[7]=ca;
		ob1[0]=cr2; ob1[1]=cg2; ob1[2]=cb2; ob1[3]=ca;
		ob1[4]=cr3; ob1[5]=cg3; ob1[6]=cb3; ob1[7]=ca;
	}
}

void BTIC2C_DecodeColorTransformYCbCr420_Block_BGRX(
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
		ca=255;

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
			BT2C_CLRTRANS_SIMPLECLAMP255x4B(cr0, cr1, cr2, cr3);
			BT2C_CLRTRANS_SIMPLECLAMP255x4B(cg0, cg1, cg2, cg3);
			BT2C_CLRTRANS_SIMPLECLAMP255x4B(cb0, cb1, cb2, cb3);
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

		ob0[0]=cb0; ob0[1]=cg0; ob0[2]=cr0; ob0[3]=ca;
		ob0[4]=cb1; ob0[5]=cg1; ob0[6]=cr1; ob0[7]=ca;
		ob1[0]=cb2; ob1[1]=cg2; ob1[2]=cr2; ob1[3]=ca;
		ob1[4]=cb3; ob1[5]=cg3; ob1[6]=cr3; ob1[7]=ca;
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
	int l0, l1, l2, l3;
	int i, j, k, l;

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
}
#endif
