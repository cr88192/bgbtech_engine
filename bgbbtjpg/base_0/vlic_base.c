/*
VLIC:
Video Layer Image Composition

Goal:
Set of utilities for offline / batch video composition tasks.

Performance may be relevant, but will not be the deciding factor for this.
Generally, calculations will be floating and/or fixed point.
 */

#include <bgbbtj.h>

BGBBTJ_API BTVLIC_Texture2D *BTVLIC_TextureFromRGBA(
	byte *rgba, int xs, int ys)
{
	BTVLIC_Texture2D *tmp;
	int i, n;
	
	tmp=gctalloc("btvlic_texture2d_t", sizeof(BTVLIC_Texture2D));

	tmp->buf=gcalloc(xs*ys*4*sizeof(s16));
	tmp->xs=xs;
	tmp->ys=ys;
	tmp->npot=((xs&(xs-1)) || (ys&(ys-1)))?1:0;

	if(rgba)
	{
		n=xs*ys*4;
		for(i=0; i<n; i++)
			{ tmp->buf[i]=rgba[i]*16; }
	}

	return(tmp);
}

BGBBTJ_API BTVLIC_Texture2D *BTVLIC_TextureFromSharedRGBA(
	byte *rgba, int xs, int ys)
{
	BTVLIC_Texture2D *tmp;
	int i, n;
	
	tmp=gctalloc("btvlic_texture2d_t", sizeof(BTVLIC_Texture2D));

//	tmp->buf=gcalloc(xs*ys*4*sizeof(s16));
	tmp->rgba=rgba;
	tmp->xs=xs;
	tmp->ys=ys;
	tmp->npot=((xs&(xs-1)) || (ys&(ys-1)))?1:0;

	return(tmp);
}

BGBBTJ_API BTVLIC_Texture2D *BTVLIC_TextureFromAVI(
	BGBBTJ_AVI_Context *avictx)
{
	BTVLIC_Texture2D *tmp;
	byte *fdbuf;
	int w, h;

	fdbuf=BGBBTJ_AVI_DecodeFrame(avictx);
	w=avictx->bmihead->biWidth;
	h=avictx->bmihead->biHeight;
	tmp=BTVLIC_TextureFromSharedRGBA(fdbuf, w, h);
	tmp->avictx=avictx;
	
	tmp->startDelay=0;
	tmp->playSpeed=1.0;
	tmp->isPaused=false;
	tmp->reqRestart=false;
	
	return(tmp);
}

BGBBTJ_API void BTVLIC_TextureToRGBA(BTVLIC_Texture2D *tex, byte *rgba)
{
	int i, j, n;
	
	n=tex->xs*tex->ys*4;
	for(i=0; i<n; i++)
	{
		j=tex->buf[i];
		j=(j+8)>>4;
		if(j<0)j=0;
		if(j>255)j=255;
		rgba[i]=j;
	}
}

BGBBTJ_API void BTVLIC_TextureToRGBA_Q11_4(BTVLIC_Texture2D *tex, s16 *rgba)
{
	int i, j, n;
	
	n=tex->xs*tex->ys*4;
	for(i=0; i<n; i++)
	{
		j=tex->buf[i];
		rgba[i]=j;
	}
}

BGBBTJ_API void BTVLIC_TextureUpdate(BTVLIC_Texture2D *tex, float dt)
{
	float dt1;

	if(tex->startDelay>0)
	{
		tex->startDelay-=dt;
		return;
	}

	if(tex->isPaused)
		return;

	if(tex->avictx)
	{
		if(tex->reqRestart)
		{
			BGBBTJ_AVI_Restart(tex->avictx);
			tex->reqRestart=false;
		}
		
		dt1=dt*tex->playSpeed;
		tex->rgba=BGBBTJ_AVI_FrameRaw(tex->avictx, dt1);
	}
}

BGBBTJ_API void BTVLIC_TextureUpdateSeek(BTVLIC_Texture2D *tex, float dt)
{
	float dt1;

	if(tex->avictx)
	{
		dt1=dt*tex->playSpeed;
		BGBBTJ_AVI_Seek(tex->avictx, dt1);
	}
}

BGBBTJ_API BTVLIC_DrawContext *BTVLIC_NewDrawContext(BTVLIC_Texture2D *tgt)
{
	BTVLIC_DrawContext *ctx;
	
	ctx=gctalloc("btvlic_drawcontext_t", sizeof(BTVLIC_DrawContext));
	ctx->tgt=tgt;
	return(ctx);
}

BGBBTJ_API BTVLIC_PixelValue BTVLIC_LerpPixel(
	BTVLIC_PixelValue a, BTVLIC_PixelValue b, int t)
{
	BTVLIC_PixelValue c;
	
	c.r=((a.r*(65536-t))+(b.r*t)+32768)>>16;
	c.g=((a.g*(65536-t))+(b.g*t)+32768)>>16;
	c.b=((a.b*(65536-t))+(b.b*t)+32768)>>16;
	c.a=((a.a*(65536-t))+(b.a*t)+32768)>>16;
	return(c);
}

BGBBTJ_API BTVLIC_PixelValue BTVLIC_LerpPixel2D(
	BTVLIC_PixelValue a, BTVLIC_PixelValue b,
	BTVLIC_PixelValue c, BTVLIC_PixelValue d, int tx, int ty)
{
	BTVLIC_PixelValue p0, p1, p2;
	
	p0=BTVLIC_LerpPixel(a, b, tx);
	p1=BTVLIC_LerpPixel(c, d, tx);
	p2=BTVLIC_LerpPixel(p0, p1, ty);
	return(p2);
}

BGBBTJ_API BTVLIC_PixelValueF BTVLIC_LerpPixelF(
	BTVLIC_PixelValueF a, BTVLIC_PixelValueF b, float t)
{
	BTVLIC_PixelValueF c;
	
	c.r=(a.r*(1.0-t))+(b.r*t);
	c.g=(a.g*(1.0-t))+(b.g*t);
	c.b=(a.b*(1.0-t))+(b.b*t);
	c.a=(a.a*(1.0-t))+(b.a*t);
	return(c);
}

BGBBTJ_API BTVLIC_PixelValueF BTVLIC_LerpPixel2DF(
	BTVLIC_PixelValueF a, BTVLIC_PixelValueF b,
	BTVLIC_PixelValueF c, BTVLIC_PixelValueF d,
	float tx, float ty)
{
	BTVLIC_PixelValueF p0, p1, p2;
	
	p0=BTVLIC_LerpPixelF(a, b, tx);
	p1=BTVLIC_LerpPixelF(c, d, tx);
	p2=BTVLIC_LerpPixelF(p0, p1, ty);
	return(p2);
}

BGBBTJ_API int BTVLIC_PixelRedFixed4(BTVLIC_PixelValue c)
	{ return(c.r); }
BGBBTJ_API int BTVLIC_PixelGreenFixed4(BTVLIC_PixelValue c)
	{ return(c.g); }
BGBBTJ_API int BTVLIC_PixelBlueFixed4(BTVLIC_PixelValue c)
	{ return(c.b); }
BGBBTJ_API int BTVLIC_PixelAlphaFixed4(BTVLIC_PixelValue c)
	{ return(c.a); }

BGBBTJ_API int BTVLIC_PixelRedNormalFixed16(BTVLIC_PixelValue c)
	{ return(c.r<<4); }
BGBBTJ_API int BTVLIC_PixelGreenNormalFixed16(BTVLIC_PixelValue c)
	{ return(c.g<<4); }
BGBBTJ_API int BTVLIC_PixelBlueNormalFixed16(BTVLIC_PixelValue c)
	{ return(c.b<<4); }
BGBBTJ_API int BTVLIC_PixelAlphaNormalFixed16(BTVLIC_PixelValue c)
	{ return(c.a<<4); }

BGBBTJ_API int BTVLIC_PixelAlphaClampNormalFixed16(BTVLIC_PixelValue c)
{
	int i;
	i=c.a<<4;
	if(i<0)i=0;
	if(i>65535)i=65535;
	return(i);
}

BGBBTJ_API BTVLIC_PixelValue BTVLIC_Texture2DRawPixel(
	BTVLIC_Texture2D *tex, int x, int y)
{
	BTVLIC_PixelValue t;
	int px, py;
	int k;

#if 0
	if(tex->npot)
	{
		px=x%tex->xs;
		py=y%tex->ys;
	}else
	{
		px=x&(tex->xs-1);
		py=y&(tex->ys-1);
	}
#endif

#if 1
	px=x; py=y;
	while(px>=tex->xs) { px-=tex->xs; }
	while(py>=tex->ys) { py-=tex->ys; }
	while(px<0) { px+=tex->xs; }
	while(py<0) { py+=tex->ys; }
#endif

	if(tex->buf)
	{
		k=(py*tex->xs+px)*4;
		t.r=tex->buf[k+0];	t.g=tex->buf[k+1];
		t.b=tex->buf[k+2];	t.a=tex->buf[k+3];
	}else if(tex->rgba)
	{
		k=(py*tex->xs+px)*4;
		t.r=tex->rgba[k+0]*16;	t.g=tex->rgba[k+1]*16;
		t.b=tex->rgba[k+2]*16;	t.a=tex->rgba[k+3]*16;
	}
	return(t);
}

BGBBTJ_API void BTVLIC_SetTexture2DRawPixel(
	BTVLIC_Texture2D *tex, int x, int y, BTVLIC_PixelValue clr)
{
//	BTVLIC_PixelValue t;
	int px, py;
	int i, j, k;

	if(tex->npot)
	{
		px=x%tex->xs;
		py=y%tex->ys;
	}else
	{
		px=x&(tex->xs-1);
		py=y&(tex->ys-1);
	}
	
	if(tex->buf)
	{
		k=(py*tex->xs+px)*4;
//		t.r=tex->buf[k+0];	t.g=tex->buf[k+1];
//		t.b=tex->buf[k+2];	t.a=tex->buf[k+3];
		tex->buf[k+0]=clr.r;	tex->buf[k+1]=clr.g;
		tex->buf[k+2]=clr.b;	tex->buf[k+3]=clr.a;
	}else if(tex->rgba)
	{
		k=(py*tex->xs+px)*4;
		j=(clr.r+8)>>4; tex->rgba[k+0]=(j<0)?0:((j>255)?255:j);
		j=(clr.g+8)>>4; tex->rgba[k+1]=(j<0)?0:((j>255)?255:j);
		j=(clr.b+8)>>4; tex->rgba[k+2]=(j<0)?0:((j>255)?255:j);
		j=(clr.a+8)>>4; tex->rgba[k+3]=(j<0)?0:((j>255)?255:j);
	}
//	return(t);
}

BGBBTJ_API BTVLIC_PixelValue BTVLIC_Texture2DInterpolatedPixel(
	BTVLIC_Texture2D *tex, int x, int y)
{
	BTVLIC_PixelValue p0, p1, p2, p3;
	BTVLIC_PixelValue d;
	int tx, ty;
	int px, py, rx, ry;
	
	tx=(x*tex->xs);
	ty=(y*tex->ys);
	px=tx>>16;
	py=ty>>16;
	rx=tx&65535;
	ry=ty&65535;
	
	p0=BTVLIC_Texture2DRawPixel(tex, px+0, py+0);
	p1=BTVLIC_Texture2DRawPixel(tex, px+1, py+0);
	p2=BTVLIC_Texture2DRawPixel(tex, px+0, py+1);
	p3=BTVLIC_Texture2DRawPixel(tex, px+1, py+1);
	
//	d=p0;
	
	d=BTVLIC_LerpPixel2D(p0, p1, p2, p3, rx, ry);
	return(d);
}

BGBBTJ_API BTVLIC_PixelValue BTVLIC_Texture2DNearestPixel(
	BTVLIC_Texture2D *tex, int x, int y)
{
	BTVLIC_PixelValue d;
	int tx, ty;
	int px, py;
	
	tx=(x*tex->xs);
	ty=(y*tex->ys);
	px=(tx+32768)>>16;
	py=(ty+32768)>>16;
	d=BTVLIC_Texture2DRawPixel(tex, px, py);
	return(d);
}

BGBBTJ_API BTVLIC_PixelValueF BTVLIC_PixelValueI2F(BTVLIC_PixelValue px)
{
	BTVLIC_PixelValueF t;
	t.r=px.r*(1.0/4080);
	t.g=px.g*(1.0/4080);
	t.b=px.b*(1.0/4080);
	t.a=px.a*(1.0/4080);
	return(t);
}

BGBBTJ_API BTVLIC_PixelValue BTVLIC_PixelValueF2I(BTVLIC_PixelValueF px)
{
	BTVLIC_PixelValue t;
	t.r=px.r*4080;
	t.g=px.g*4080;
	t.b=px.b*4080;
	t.a=px.a*4080;
	return(t);
}

BGBBTJ_API BTVLIC_PixelValueF BTVLIC_Texture2DRawPixelF(
	BTVLIC_Texture2D *tex, int x, int y)
{
	BTVLIC_PixelValue px;
	px=BTVLIC_Texture2DRawPixel(tex, x, y);
	return(BTVLIC_PixelValueI2F(px));
}

BGBBTJ_API BTVLIC_PixelValueF BTVLIC_Texture2DInterpolatedPixelF(
	BTVLIC_Texture2D *tex, float x, float y)
{
	BTVLIC_PixelValue px;
	px=BTVLIC_Texture2DInterpolatedPixel(tex, x*65535+0.5, y*65535+0.5);
	return(BTVLIC_PixelValueI2F(px));
}

BGBBTJ_API BTVLIC_PixelValueF BTVLIC_Texture2DNearestPixelF(
	BTVLIC_Texture2D *tex, float x, float y)
{
	BTVLIC_PixelValue px;
	px=BTVLIC_Texture2DNearestPixel(tex, x*65535+0.5, y*65535+0.5);
	return(BTVLIC_PixelValueI2F(px));
}

BGBBTJ_API void BTVLIC_SetTexture2DRawPixelF(
	BTVLIC_Texture2D *tex, int x, int y, BTVLIC_PixelValueF clr)
{
	BTVLIC_SetTexture2DRawPixel(tex, x, y,
		BTVLIC_PixelValueF2I(clr));
}

BGBBTJ_API vec2 BTVLIC_Vec2TransMat3(vec2 v, BTVLIC_Mat3x3 mat)
{
	vec2 w;
	
	w=vec2(
		v2x(v)*mat.v00+v2y(v)*mat.v10+mat.v20,
		v2x(v)*mat.v01+v2y(v)*mat.v11+mat.v21);
	return(w);
}

BGBBTJ_API vec2 BTVLIC_Vec2TransIMat3(vec2 v, BTVLIC_Mat3x3 mat)
{
	float x, y;
	vec2 w;
	
	x=v2x(v)-mat.v20;	y=v2y(v)-mat.v21;
	w=vec2(x*mat.v00+y*mat.v01, x*mat.v10+y*mat.v11);
	return(w);
}

BGBBTJ_API vec2 BTVLIC_NVec2TransMat3(vec2 v, BTVLIC_Mat3x3 mat)
{
	vec2 w;
	
	w=vec2(
		v2x(v)*mat.v00+v2y(v)*mat.v10,
		v2x(v)*mat.v01+v2y(v)*mat.v11);
	return(w);
}

BGBBTJ_API BTVLIC_Mat3x3 BTVLIC_MatMultMat3(
	BTVLIC_Mat3x3 mata, BTVLIC_Mat3x3 matb)
{
	BTVLIC_Mat3x3 matc;
	
	matc.v00=(mata.v00*matb.v00)+(mata.v01*matb.v10)+(mata.v02*matb.v20);
	matc.v01=(mata.v00*matb.v01)+(mata.v01*matb.v11)+(mata.v02*matb.v21);
	matc.v02=(mata.v00*matb.v02)+(mata.v01*matb.v12)+(mata.v02*matb.v22);
	matc.v10=(mata.v10*matb.v00)+(mata.v11*matb.v10)+(mata.v12*matb.v20);
	matc.v11=(mata.v10*matb.v01)+(mata.v11*matb.v11)+(mata.v12*matb.v21);
	matc.v12=(mata.v10*matb.v02)+(mata.v11*matb.v12)+(mata.v12*matb.v22);
	matc.v20=(mata.v20*matb.v00)+(mata.v21*matb.v10)+(mata.v22*matb.v20);
	matc.v21=(mata.v20*matb.v01)+(mata.v21*matb.v11)+(mata.v22*matb.v21);
	matc.v22=(mata.v20*matb.v02)+(mata.v21*matb.v12)+(mata.v22*matb.v22);
	return(matc);
}

BGBBTJ_API BTVLIC_PixelValue BTVLIC_RenderDrawLayerPixel(
	BTVLIC_DrawContext *ctx, BTVLIC_DrawLayer *lyr, int x, int y)
{
	BTVLIC_PixelValue pix;
	int px, py, vx, vy;
	
	px=x-lyr->ox;
	py=y-lyr->oy;

	if((px<0) || (py<0) || (px>=lyr->xs) || (py>=lyr->ys))
	{
		pix.r=0;	pix.g=0;	pix.b=0;	pix.a=0;
		return(pix);
	}

	vx=(65536*px)/lyr->xs;
	vy=(65536*py)/lyr->ys;
	
	pix=BTVLIC_Texture2DInterpolatedPixel(lyr->tex, vx, vy);
	return(pix);
}

BGBBTJ_API BTVLIC_PixelValueF BTVLIC_RenderDrawLayerPixelF(
	BTVLIC_DrawContext *ctx, BTVLIC_DrawLayer *lyr, float x, float y)
{
	BTVLIC_PixelValueF pix, mpix;
	vec2 vc;
	float px, py, vx, vy;
	
	px=x-lyr->ox;
	py=y-lyr->oy;

	if((px<0) || (py<0) || (px>=lyr->xs) || (py>=lyr->ys))
	{
		pix.r=0;	pix.g=0;	pix.b=0;	pix.a=0;
		return(pix);
	}

	vx=px/lyr->xs;
	vy=py/lyr->ys;
	
	vc=BTVLIC_Vec2TransMat3(vec2(vx, vy), lyr->xform_tex);
	vx=v2x(vc); vy=v2y(vc);

	if((lyr->xs>lyr->tex->xs) && (lyr->ys>lyr->tex->ys))
	{
		pix=BTVLIC_Texture2DInterpolatedPixelF(lyr->tex, vx, vy);
	}else
	{
		pix=BTVLIC_Texture2DNearestPixelF(lyr->tex, vx, vy);
	}
	
	if(lyr->masktex)
	{
		mpix=BTVLIC_Texture2DNearestPixelF(lyr->tex, vx, vy);
		pix=BTVLIC_BlendLayerPixelABF(ctx, lyr,
			pix, mpix, pix, lyr->maskblend);
	}
	
	return(pix);
}

BGBBTJ_API BTVLIC_PixelValueF BTVLIC_BlendLayerPixelABF(
	BTVLIC_DrawContext *ctx, BTVLIC_DrawLayer *lyr,
	BTVLIC_PixelValueF src, BTVLIC_PixelValueF dst,
	BTVLIC_PixelValueF clr, int blend)
{
	BTVLIC_PixelValueF d;
	
	switch(blend)
	{
	case BTVLIC_BLEND_ZERO:
		d.r=0;	d.g=0;
		d.b=0;	d.a=0;
		break;
	case BTVLIC_BLEND_ONE:
		d=clr;
		break;
	case BTVLIC_BLEND_SRC_COLOR:
		d.r=clr.r*src.r;	d.g=clr.g*src.g;
		d.b=clr.b*src.b;	d.a=clr.a*src.a;
		break;
	case BTVLIC_BLEND_DST_COLOR:
		d.r=clr.r*dst.r;	d.g=clr.g*dst.g;
		d.b=clr.b*dst.b;	d.a=clr.a*dst.a;
		break;

	case BTVLIC_BLEND_SRC_RED:
		d.r=clr.r*src.r;	d.g=clr.g*src.r;
		d.b=clr.b*src.r;	d.a=clr.a*src.r;
		break;
	case BTVLIC_BLEND_DST_RED:
		d.r=clr.r*dst.r;	d.g=clr.g*dst.r;
		d.b=clr.b*dst.r;	d.a=clr.a*dst.r;
		break;
	case BTVLIC_BLEND_SRC_GREEN:
		d.r=clr.r*src.g;	d.g=clr.g*src.g;
		d.b=clr.b*src.g;	d.a=clr.a*src.g;
		break;
	case BTVLIC_BLEND_DST_GREEN:
		d.r=clr.r*dst.g;	d.g=clr.g*dst.g;
		d.b=clr.b*dst.g;	d.a=clr.a*dst.g;
		break;
	case BTVLIC_BLEND_SRC_BLUE:
		d.r=clr.r*src.b;	d.g=clr.g*src.b;
		d.b=clr.b*src.b;	d.a=clr.a*src.b;
		break;
	case BTVLIC_BLEND_DST_BLUE:
		d.r=clr.r*dst.b;	d.g=clr.g*dst.b;
		d.b=clr.b*dst.b;	d.a=clr.a*dst.b;
		break;
	case BTVLIC_BLEND_SRC_ALPHA:
		d.r=clr.r*src.a;	d.g=clr.g*src.a;
		d.b=clr.b*src.a;	d.a=clr.a*src.a;
		break;
	case BTVLIC_BLEND_DST_ALPHA:
		d.r=clr.r*dst.a;	d.g=clr.g*dst.a;
		d.b=clr.b*dst.a;	d.a=clr.a*dst.a;
		break;

	case BTVLIC_BLEND_ONE_MINUS_SRC_COLOR:
		d.r=clr.r*(1.0-src.r);	d.g=clr.g*(1.0-src.g);
		d.b=clr.b*(1.0-src.b);	d.a=clr.a*(1.0-src.a);
		break;
	case BTVLIC_BLEND_ONE_MINUS_DST_COLOR:
		d.r=clr.r*(1.0-dst.r);	d.g=clr.g*(1.0-dst.g);
		d.b=clr.b*(1.0-dst.b);	d.a=clr.a*(1.0-dst.a);
		break;

	case BTVLIC_BLEND_ONE_MINUS_SRC_RED:
		d.r=clr.r*(1.0-src.r);	d.g=clr.g*(1.0-src.r);
		d.b=clr.b*(1.0-src.r);	d.a=clr.a*(1.0-src.r);
		break;
	case BTVLIC_BLEND_ONE_MINUS_DST_RED:
		d.r=clr.r*(1.0-dst.r);	d.g=clr.g*(1.0-dst.r);
		d.b=clr.b*(1.0-dst.r);	d.a=clr.a*(1.0-dst.r);
		break;
	case BTVLIC_BLEND_ONE_MINUS_SRC_GREEN:
		d.r=clr.r*(1.0-src.g);	d.g=clr.g*(1.0-src.g);
		d.b=clr.b*(1.0-src.g);	d.a=clr.a*(1.0-src.g);
		break;
	case BTVLIC_BLEND_ONE_MINUS_DST_GREEN:
		d.r=clr.r*(1.0-dst.g);	d.g=clr.g*(1.0-dst.g);
		d.b=clr.b*(1.0-dst.g);	d.a=clr.a*(1.0-dst.g);
		break;
	case BTVLIC_BLEND_ONE_MINUS_SRC_BLUE:
		d.r=clr.r*(1.0-src.b);	d.g=clr.g*(1.0-src.b);
		d.b=clr.b*(1.0-src.b);	d.a=clr.a*(1.0-src.b);
		break;
	case BTVLIC_BLEND_ONE_MINUS_DST_BLUE:
		d.r=clr.r*(1.0-dst.b);	d.g=clr.g*(1.0-dst.b);
		d.b=clr.b*(1.0-dst.b);	d.a=clr.a*(1.0-dst.b);
		break;

	case BTVLIC_BLEND_ONE_MINUS_SRC_ALPHA:
		d.r=clr.r*(1.0-src.a);	d.g=clr.g*(1.0-src.a);
		d.b=clr.b*(1.0-src.a);	d.a=clr.a*(1.0-src.a);
		break;
	case BTVLIC_BLEND_ONE_MINUS_DST_ALPHA:
		d.r=clr.r*(1.0-dst.a);	d.g=clr.g*(1.0-dst.a);
		d.b=clr.b*(1.0-dst.a);	d.a=clr.a*(1.0-dst.a);
		break;
	}
	return(d);
}

BGBBTJ_API BTVLIC_PixelValueF BTVLIC_BlendLayerPixelF(
	BTVLIC_DrawContext *ctx, BTVLIC_DrawLayer *lyr,
	BTVLIC_PixelValueF a, BTVLIC_PixelValueF b)
{
	BTVLIC_PixelValueF c, d;
	float alpha;
	
	alpha=lyr->alpha/4080.0;
	
	switch(lyr->blend)
	{
	case BTVLIC_BLEND_NORMAL:
		c=BTVLIC_LerpPixelF(a, b, b.a*alpha);
		break;
	case BTVLIC_BLEND_ADD:
		d.r=a.r+b.r;	d.g=a.g+b.g;
		d.b=a.b+b.b;	//d.a=a.a+b.a;
		d.a=b.a;
		c=BTVLIC_LerpPixelF(a, d, b.a*alpha);
		break;
	case BTVLIC_BLEND_SUBTRACT:
		d.r=a.r-b.r;	d.g=a.g-b.g;
		d.b=a.b-b.b;	//d.a=a.a-b.a;
		d.a=b.a;
		c=BTVLIC_LerpPixelF(a, d, b.a*alpha);
		break;
	case BTVLIC_BLEND_MULTIPLY:
		d.r=a.r*b.r;	d.g=a.g*b.g;
		d.b=a.b*b.b;	//d.a=a.a*b.a;
		d.a=b.a;
		c=BTVLIC_LerpPixelF(a, d, b.a*alpha);
		break;
	case BTVLIC_BLEND_DIVIDE:
		d.r=a.r/b.r;	d.g=a.g/b.g;
		d.b=a.b/b.b;	//d.a=a.a/b.a;
		d.a=b.a;
		c=BTVLIC_LerpPixelF(a, d, b.a*alpha);
		break;
	case BTVLIC_BLEND_SCREEN:
		d.r=1.0-(1.0-a.r)*(1.0-b.r);	d.g=1.0-(1.0-a.g)*(1.0-b.g);
		d.b=1.0-(1.0-a.b)*(1.0-b.b);	//d.a=1.0-(1.0-a.a)*(1.0-b.a);
		d.a=b.a;
		c=BTVLIC_LerpPixelF(a, d, b.a*alpha);
		break;
	case BTVLIC_BLEND_OVERLAY:
		if(a.r<0.5)
				{ d.r=2.0*a.r*b.r; }
		else	{ d.r=1.0-2.0*(1.0-a.r)*(1.0-b.r); }
		if(a.g<0.5)
				{ d.g=2.0*a.g*b.g; }
		else	{ d.g=1.0-2.0*(1.0-a.g)*(1.0-b.g); }
		if(a.b<0.5)
				{ d.b=2.0*a.b*b.b; }
		else	{ d.b=1.0-2.0*(1.0-a.b)*(1.0-b.b); }
//		if(a.a<0.5)
//				{ d.a=2.0*a.a*b.a; }
//		else	{ d.a=1.0-2.0*(1.0-a.a)*(1.0-b.a); }
		d.a=b.a;
		c=BTVLIC_LerpPixelF(a, d, b.a*alpha);
		break;
	case BTVLIC_BLEND_DIFFERENCE:
		d.r=abs(a.r-b.r);	d.g=abs(a.g-b.g);
		d.b=abs(a.b-b.b);	//d.a=abs(a.a-b.a);
		d.a=b.a;
		c=BTVLIC_LerpPixelF(a, d, b.a*alpha);
		break;
	case BTVLIC_BLEND_DARKEN:
		d.r=min(a.r, b.r);	d.g=min(a.g, b.g);
		d.b=min(a.b, b.b);	//d.a=min(a.a, b.a);
		d.a=b.a;
		c=BTVLIC_LerpPixelF(a, d, b.a*alpha);
		break;
	case BTVLIC_BLEND_LIGHTEN:
		d.r=max(a.r, b.r);	d.g=max(a.g, b.g);
		d.b=max(a.b, b.b);	//d.a=max(a.a, b.a);
		d.a=b.a;
		c=BTVLIC_LerpPixelF(a, d, b.a*alpha);
		break;
	case BTVLIC_BLEND_COLOR_DODGE:
		d.r=a.r/(1.0-b.r);	d.g=a.g/(1.0-b.g);
		d.b=a.b/(1.0-b.b);	//d.a=a.a/(1.0-b.a);
		d.a=b.a;
		c=BTVLIC_LerpPixelF(a, d, b.a*alpha);
		break;
	case BTVLIC_BLEND_COLOR_BURN:
		d.r=1.0-(1.0-a.r)/b.r;	d.g=1.0-(1.0-a.g)/b.g;
		d.b=1.0-(1.0-a.b)/b.b;	//d.a=1.0-(1.0-a.a)/b.a;
		d.a=b.a;
		c=BTVLIC_LerpPixelF(a, d, b.a*alpha);
		break;

	case BTVLIC_BLEND_INVERT:
		d.r=1.0-b.r;	d.g=1.0-b.g;
		d.b=1.0-b.b;	//d.a=a.a-b.a;
		d.a=b.a;
		c=BTVLIC_LerpPixelF(a, d, b.a*alpha);
		break;

	case BTVLIC_BLEND_ZERO:
	case BTVLIC_BLEND_ONE:
	case BTVLIC_BLEND_SRC_COLOR:
	case BTVLIC_BLEND_DST_COLOR:
	case BTVLIC_BLEND_SRC_ALPHA:
	case BTVLIC_BLEND_DST_ALPHA:
	case BTVLIC_BLEND_ONE_MINUS_SRC_COLOR:
	case BTVLIC_BLEND_ONE_MINUS_DST_COLOR:
	case BTVLIC_BLEND_ONE_MINUS_SRC_ALPHA:
	case BTVLIC_BLEND_ONE_MINUS_DST_ALPHA:

	case BTVLIC_BLEND_SRC_RED:
	case BTVLIC_BLEND_DST_RED:
	case BTVLIC_BLEND_SRC_GREEN:
	case BTVLIC_BLEND_DST_GREEN:
	case BTVLIC_BLEND_SRC_BLUE:
	case BTVLIC_BLEND_DST_BLUE:
	case BTVLIC_BLEND_ONE_MINUS_SRC_RED:
	case BTVLIC_BLEND_ONE_MINUS_DST_RED:
	case BTVLIC_BLEND_ONE_MINUS_SRC_GREEN:
	case BTVLIC_BLEND_ONE_MINUS_DST_GREEN:
	case BTVLIC_BLEND_ONE_MINUS_SRC_BLUE:
	case BTVLIC_BLEND_ONE_MINUS_DST_BLUE:

		c=BTVLIC_BlendLayerPixelABF(ctx, lyr, b, a, a, lyr->blend2);
		d=BTVLIC_BlendLayerPixelABF(ctx, lyr, b, a, b, lyr->blend);
		d.r=c.r+d.r;	d.g=c.g+d.g;
		d.b=c.b+d.b;	d.a=c.a+d.a;
		c=BTVLIC_LerpPixelF(a, d, alpha);
		break;

	default:
		c=BTVLIC_LerpPixelF(a, b, b.a*alpha);
		break;
	}
	return(c);
}

BGBBTJ_API void BTVLIC_RenderContextLayer(
	BTVLIC_DrawContext *ctx, BTVLIC_DrawLayer *lyr)
{
//	BTVLIC_PixelValue p0, p1, p2;
	BTVLIC_PixelValueF p0, p1, p2;
	vec2 vc0, vc1, vc2;
	int mx, my, nx, ny;
	int x, y;

	vc0=BTVLIC_Vec2TransMat3(vec2(lyr->ox, lyr->oy), lyr->xform_base);
	vc1=BTVLIC_NVec2TransMat3(vec2(lyr->xs, lyr->ys), lyr->xform_base);

	mx=min(v2x(vc0), v2x(vc0)+v2x(vc1));
	my=min(v2y(vc0), v2y(vc0)+v2y(vc1));
	nx=max(v2x(vc0), v2x(vc0)+v2x(vc1));
	ny=max(v2y(vc0), v2y(vc0)+v2y(vc1));

	mx=max(0, mx);
	my=max(0, my);
	nx=min(ctx->tgt->xs, nx);
	ny=min(ctx->tgt->ys, ny);

//	mx=max(0, lyr->ox);
//	my=max(0, lyr->oy);
//	nx=min(ctx->tgt->xs, lyr->ox+lyr->xs);
//	ny=min(ctx->tgt->ys, lyr->oy+lyr->ys);
	
	for(y=my; y<ny; y++)
		for(x=mx; x<nx; x++)
	{
#if 0
		p0=BTVLIC_RenderDrawLayerPixel(ctx, lyr, x, y);
		p1=BTVLIC_Texture2DRawPixel(ctx->tgt, x, y);
		p2=BTVLIC_LerpPixel(p1, p0,
			BTVLIC_PixelAlphaClampNormalFixed16(p0));
		BTVLIC_SetTexture2DRawPixel(ctx->tgt, x, y, p2);
#endif

#if 1
		vc2=BTVLIC_Vec2TransIMat3(vec2(x, y), lyr->xform_base);
//		vc2=BTVLIC_Vec2TransMat3(vc2, lyr->xform_tex);

		p0=BTVLIC_RenderDrawLayerPixelF(ctx, lyr, v2x(vc2), v2y(vc2));

//		p0=BTVLIC_RenderDrawLayerPixelF(ctx, lyr, x, y);
		p1=BTVLIC_Texture2DRawPixelF(ctx->tgt, x, y);
//		p2=BTVLIC_LerpPixelF(p1, p0, p0.a);
		p2=BTVLIC_BlendLayerPixelF(ctx, lyr, p1, p0);
		BTVLIC_SetTexture2DRawPixelF(ctx->tgt, x, y, p2);
#endif
	}
}

BGBBTJ_API void BTVLIC_RenderContext(BTVLIC_DrawContext *ctx)
{
	BTVLIC_PixelValue px0;
	int i, j, k;
	
	px0.r=255*16;	px0.g=0*16;
	px0.b=255*16;	px0.a=0*16;
	
	for(i=0; i<ctx->tgt->ys; i++)
		for(j=0; j<ctx->tgt->xs; j++)
	{
		BTVLIC_SetTexture2DRawPixel(ctx->tgt, j, i, px0);
	}
	
	for(i=0; i<ctx->n_layer; i++)
	{
		BTVLIC_RenderContextLayer(ctx, ctx->layer[i]);
	}
}

BGBBTJ_API void BTVLIC_UpdateContextLayer(
	BTVLIC_DrawContext *ctx, BTVLIC_DrawLayer *lyr, float dt)
{
	BTVLIC_TransformDrawLayerBase(ctx, lyr,
		lyr->xfbase_xvel*dt, lyr->xfbase_yvel*dt, lyr->xfbase_avel*dt);
	BTVLIC_TransformDrawLayerTex(ctx, lyr,
		lyr->xftex_xvel*dt, lyr->xftex_yvel*dt, lyr->xftex_avel*dt);
}

BGBBTJ_API void BTVLIC_UpdateContext(BTVLIC_DrawContext *ctx, float dt)
{
	int i, j, k;
	
	for(i=0; i<ctx->n_tex; i++)
	{
		BTVLIC_TextureUpdate(ctx->tex[i], dt);

//		BTVLIC_RenderContextLayer(ctx, ctx->layer[i]);
	}
	
	for(i=0; i<ctx->n_layer; i++)
	{
		BTVLIC_UpdateContextLayer(ctx, ctx->layer[i], dt);
	}
}

BGBBTJ_API BTVLIC_Texture2D *BTVLIC_LookupTexture(
	BTVLIC_DrawContext *ctx, char *name)
{
	int i, j, k;
	
	for(i=0; i<ctx->n_tex; i++)
	{
		if(!strcmp(ctx->tex[i]->name, name))
			return(ctx->tex[i]);
	}
	return(NULL);
}

BGBBTJ_API BTVLIC_DrawLayer *BTVLIC_LookupDrawLayer(
	BTVLIC_DrawContext *ctx, char *name)
{
	int i, j, k;
	
	for(i=0; i<ctx->n_layer; i++)
	{
		if(!strcmp(ctx->layer[i]->name, name))
			return(ctx->layer[i]);
	}
	return(NULL);
}

BTVLIC_Mat3x3 BTVLIC_Mat3Identity()
{
	BTVLIC_Mat3x3 t;
	t.v00=1;	t.v01=0;	t.v02=0;
	t.v10=0;	t.v11=1;	t.v12=0;
	t.v20=0;	t.v21=0;	t.v22=1;
	return(t);
}

BGBBTJ_API BTVLIC_DrawLayer *BTVLIC_NewDrawLayerContext(
	BTVLIC_DrawContext *ctx, char *name,
	BTVLIC_Texture2D *tex,
	int ox, int oy, int xs, int ys)
{
	BTVLIC_DrawLayer *tmp;
	int i, n;
	
	tmp=gctalloc("btvlic_drawlayer_t", sizeof(BTVLIC_DrawLayer));

//	tmp->buf=gcalloc(xs*ys*sizeof(s16));
//	tmp->rgba=rgba;
	tmp->ox=ox;
	tmp->oy=oy;
	tmp->xs=xs;
	tmp->ys=ys;
	
	tmp->name=dystrdup(name);
	tmp->tex=tex;
//	tmp->ctx=ctx;

	tmp->xform_base=BTVLIC_Mat3Identity();
	tmp->xform_tex=BTVLIC_Mat3Identity();
	tmp->alpha=4080;

	return(tmp);
	
}

BGBBTJ_API int BTVLIC_BlendFuncForName(BTVLIC_DrawContext *ctx, char *name)
{
	int blend;

	blend=-1;
	if(!strcmp(name, "normal"))
		{ blend=BTVLIC_BLEND_NORMAL; }
	else if(!strcmp(name, "add"))
		{ blend=BTVLIC_BLEND_ADD; }
	else if(!strcmp(name, "subtract"))
		{ blend=BTVLIC_BLEND_SUBTRACT; }
	else if(!strcmp(name, "multiply"))
		{ blend=BTVLIC_BLEND_MULTIPLY; }
	else if(!strcmp(name, "divide"))
		{ blend=BTVLIC_BLEND_DIVIDE; }
	else if(!strcmp(name, "screen"))
		{ blend=BTVLIC_BLEND_SCREEN; }
	else if(!strcmp(name, "overlay"))
		{ blend=BTVLIC_BLEND_OVERLAY; }
	else if(!strcmp(name, "difference"))
		{ blend=BTVLIC_BLEND_DIFFERENCE; }
	else if(!strcmp(name, "darken"))
		{ blend=BTVLIC_BLEND_DARKEN; }
	else if(!strcmp(name, "lighten"))
		{ blend=BTVLIC_BLEND_LIGHTEN; }
	else if(!strcmp(name, "color_dodge"))
		{ blend=BTVLIC_BLEND_COLOR_DODGE; }
	else if(!strcmp(name, "color_burn"))
		{ blend=BTVLIC_BLEND_COLOR_BURN; }
	else if(!strcmp(name, "invert"))
		{ blend=BTVLIC_BLEND_INVERT; }
	if(blend>=0)return(blend);

	if(!strcmp(name, "zero"))
		{ blend=BTVLIC_BLEND_ZERO; }
	else if(!strcmp(name, "one"))
		{ blend=BTVLIC_BLEND_ONE; }
	else if(!strcmp(name, "src_color"))
		{ blend=BTVLIC_BLEND_SRC_COLOR; }
	else if(!strcmp(name, "dst_color"))
		{ blend=BTVLIC_BLEND_DST_COLOR; }
	else if(!strcmp(name, "src_alpha"))
		{ blend=BTVLIC_BLEND_SRC_ALPHA; }
	else if(!strcmp(name, "dst_alpha"))
		{ blend=BTVLIC_BLEND_DST_ALPHA; }
	else if(!strcmp(name, "one_minus_src_color"))
		{ blend=BTVLIC_BLEND_ONE_MINUS_SRC_COLOR; }
	else if(!strcmp(name, "one_minus_dst_color"))
		{ blend=BTVLIC_BLEND_ONE_MINUS_DST_COLOR; }
	else if(!strcmp(name, "one_minus_src_alpha"))
		{ blend=BTVLIC_BLEND_ONE_MINUS_SRC_ALPHA; }
	else if(!strcmp(name, "one_minus_dst_alpha"))
		{ blend=BTVLIC_BLEND_ONE_MINUS_DST_ALPHA; }
	else if(!strcmp(name, "src_red"))
		{ blend=BTVLIC_BLEND_SRC_RED; }
	else if(!strcmp(name, "dst_red"))
		{ blend=BTVLIC_BLEND_DST_RED; }
	else if(!strcmp(name, "src_green"))
		{ blend=BTVLIC_BLEND_SRC_GREEN; }
	else if(!strcmp(name, "dst_green"))
		{ blend=BTVLIC_BLEND_DST_GREEN; }
	else if(!strcmp(name, "src_blue"))
		{ blend=BTVLIC_BLEND_SRC_BLUE; }
	else if(!strcmp(name, "dst_blue"))
		{ blend=BTVLIC_BLEND_DST_BLUE; }
	else if(!strcmp(name, "one_minus_src_red"))
		{ blend=BTVLIC_BLEND_ONE_MINUS_SRC_RED; }
	else if(!strcmp(name, "one_minus_dst_red"))
		{ blend=BTVLIC_BLEND_ONE_MINUS_DST_RED; }
	else if(!strcmp(name, "one_minus_src_green"))
		{ blend=BTVLIC_BLEND_ONE_MINUS_SRC_GREEN; }
	else if(!strcmp(name, "one_minus_dst_green"))
		{ blend=BTVLIC_BLEND_ONE_MINUS_DST_GREEN; }
	else if(!strcmp(name, "one_minus_src_blue"))
		{ blend=BTVLIC_BLEND_ONE_MINUS_SRC_BLUE; }
	else if(!strcmp(name, "one_minus_dst_blue"))
		{ blend=BTVLIC_BLEND_ONE_MINUS_DST_BLUE; }

	return(blend);
}

BGBBTJ_API void BTVLIC_TransformDrawLayerBase(
	BTVLIC_DrawContext *ctx, BTVLIC_DrawLayer *lyr,
	float x, float y, float ang)
{
	BTVLIC_Mat3x3 mat;
	float a;
	
	a=ang*(M_PI/180.0);
	
	mat.v00=cos(a);		mat.v01=-sin(a);	mat.v02=0;
	mat.v10=sin(a);		mat.v11=cos(a);		mat.v12=0;
	mat.v20=x;			mat.v21=y;			mat.v22=1;
	lyr->xform_base=BTVLIC_MatMultMat3(lyr->xform_base, mat);
}

BGBBTJ_API void BTVLIC_TransformDrawLayerTex(
	BTVLIC_DrawContext *ctx, BTVLIC_DrawLayer *lyr,
	float x, float y, float ang)
{
	BTVLIC_Mat3x3 mat;
	float a;
	
	a=ang*(M_PI/180.0);
	
	mat.v00=cos(a);		mat.v01=-sin(a);	mat.v02=0;
	mat.v10=sin(a);		mat.v11=cos(a);		mat.v12=0;
	mat.v20=x;			mat.v21=y;			mat.v22=1;
	lyr->xform_tex=BTVLIC_MatMultMat3(lyr->xform_tex, mat);
}

BGBBTJ_API void BTVLIC_HandleCommand(BTVLIC_DrawContext *ctx, char **args)
{
	BTVLIC_Texture2D *tex;
	BGBBTJ_AVI_Context *avi;
	BTVLIC_DrawLayer *lyr;
	byte *buf;
	int w, h;
	int i, j, k;

	if(!strcmp(args[0], "tex_avi"))
	{
		avi=BGBBTJ_AVI_LoadAVI(args[2]);
//		vstat=BGBBTJ_AVI_GetStats(ictx);
//		w=avi->bmihead->biWidth;
//		h=avi->bmihead->biHeight;
		tex=BTVLIC_TextureFromAVI(avi);
		tex->name=dystrdup(args[1]);
		
		i=ctx->n_tex++;
		ctx->tex[i]=tex;
		return;
	}

	if(!strcmp(args[0], "tex_png"))
	{
		buf=BGBBTJ_PNG_LoadRaw(args[2], &w, &h);

		tex=BTVLIC_TextureFromRGBA(buf, w, h);
		tex->name=dystrdup(args[1]);
		
		i=ctx->n_tex++;
		ctx->tex[i]=tex;
		return;
	}

	if(!strcmp(args[0], "tex_startDelay"))
	{
		tex=BTVLIC_LookupTexture(ctx, args[1]);
		if(tex)
			{ tex->startDelay=atof(args[2]); }
	}

	if(!strcmp(args[0], "tex_playSpeed"))
	{
		tex=BTVLIC_LookupTexture(ctx, args[1]);
		if(tex)
			{ tex->playSpeed=atof(args[2]); }
	}

	if(!strcmp(args[0], "tex_pause"))
	{
		tex=BTVLIC_LookupTexture(ctx, args[1]);
		if(tex)
		{
			if(args[2])
				{ tex->isPaused=(atoi(args[2])!=0); }
			else
				{ tex->isPaused=true; }
		}
	}

	if(!strcmp(args[0], "tex_restart"))
	{
		tex=BTVLIC_LookupTexture(ctx, args[1]);
		if(tex) { tex->reqRestart=true; }
	}

	if(!strcmp(args[0], "tex_seek"))
	{
		tex=BTVLIC_LookupTexture(ctx, args[1]);
		if(tex)
			{ BTVLIC_TextureUpdateSeek(tex, atof(args[2])); }
	}

	if(!strcmp(args[0], "add_layer"))
	{
		tex=BTVLIC_LookupTexture(ctx, args[2]);
		lyr=BTVLIC_NewDrawLayerContext(ctx, args[1], tex,
			atoi(args[3]), atoi(args[4]),
			atoi(args[5]), atoi(args[6]));
		i=ctx->n_layer++;
		ctx->layer[i]=lyr;
		return;
	}

	if(!strcmp(args[0], "delete_layer"))
	{
		j=0;
		for(i=0; i<ctx->n_layer; i++)
		{
			if(!strcmp(ctx->layer[i]->name, args[1]))
			{
//				return(ctx->layer[i]);
				continue;
			}
			ctx->layer[j++]=ctx->layer[i];
		}
		ctx->n_layer=j;
		return;
	}

	if(!strcmp(args[0], "layer_addMask"))
	{
		lyr=BTVLIC_LookupDrawLayer(ctx, args[1]);
		tex=BTVLIC_LookupTexture(ctx, args[2]);
		lyr->masktex=tex;
		lyr->maskblend=BTVLIC_BlendFuncForName(ctx, args[3]);
		return;
	}

	if(!strcmp(args[0], "layer_blend"))
	{
		lyr=BTVLIC_LookupDrawLayer(ctx, args[1]);
		if(args[3])
		{
			lyr->blend=BTVLIC_BlendFuncForName(ctx, args[2]);
			lyr->blend2=BTVLIC_BlendFuncForName(ctx, args[3]);
		}else
		{
			lyr->blend=BTVLIC_BlendFuncForName(ctx, args[2]);
		}
		return;
	}

	if(!strcmp(args[0], "layer_alpha"))
	{
		lyr=BTVLIC_LookupDrawLayer(ctx, args[1]);
		lyr->alpha=atof(args[2])*16.0+0.5;
		return;
	}

	if(!strcmp(args[0], "layer_transformBase"))
	{
		lyr=BTVLIC_LookupDrawLayer(ctx, args[1]);
		if(lyr)
		{
			BTVLIC_TransformDrawLayerBase(ctx, lyr,
				atof(args[2]), atof(args[3]), atof(args[4]));
		}
	}

	if(!strcmp(args[0], "layer_transformTex"))
	{
		lyr=BTVLIC_LookupDrawLayer(ctx, args[1]);
		if(lyr)
		{
			BTVLIC_TransformDrawLayerTex(ctx, lyr,
				atof(args[2]), atof(args[3]), atof(args[4]));
		}
	}

	if(!strcmp(args[0], "layer_transformResetBase"))
	{
		lyr=BTVLIC_LookupDrawLayer(ctx, args[1]);
		lyr->xform_base=BTVLIC_Mat3Identity();
	}

	if(!strcmp(args[0], "layer_transformResetTex"))
	{
		lyr=BTVLIC_LookupDrawLayer(ctx, args[1]);
		lyr->xform_tex=BTVLIC_Mat3Identity();
	}

	if(!strcmp(args[0], "layer_transformBaseVel"))
	{
		lyr=BTVLIC_LookupDrawLayer(ctx, args[1]);
		lyr->xfbase_xvel=atof(args[2]);
		lyr->xfbase_yvel=atof(args[3]);
		lyr->xfbase_avel=atof(args[4]);
	}

	if(!strcmp(args[0], "layer_transformTexVel"))
	{
		lyr=BTVLIC_LookupDrawLayer(ctx, args[1]);
		lyr->xftex_xvel=atof(args[2]);
		lyr->xftex_yvel=atof(args[3]);
		lyr->xftex_avel=atof(args[4]);
	}
}
