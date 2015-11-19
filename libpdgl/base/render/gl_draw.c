#include <pdgl.h>

#ifndef GLES
// #include <GL/gl.h>
// #include <math.h>

int pdgl_view_xo;
int pdgl_view_yo;
int pdgl_view_xs;
int pdgl_view_ys;

PDGL_API void PDGL_GetViewOrigin(int *rxo, int *ryo)
{
	*rxo=pdgl_view_xo;
	*ryo=pdgl_view_yo;
}

PDGL_API void PDGL_GetViewSize(int *rxs, int *rys)
{
	*rxs=pdgl_view_xs;
	*rys=pdgl_view_ys;
}

PDGL_API int Draw_SetSolid_2D(float aspect, int xo, int yo, int xs, int ys)
{
	pdgl_view_xo=xo;
	pdgl_view_yo=yo;
	pdgl_view_xs=xs;
	pdgl_view_ys=ys;

	pdglViewport(xo, yo, xs, ys);

	pdglMatrixMode(GL_PROJECTION);
	pdglLoadIdentity();
	pdglOrtho(-1000, 1000, -1000/aspect, 1000/aspect, -99999, 99999);

	pdglMatrixMode(GL_MODELVIEW);
	pdglLoadIdentity();

	pdglDisable(GL_DEPTH_TEST);
	pdglDisable(GL_CULL_FACE);
	pdglEnable(GL_BLEND);
	pdglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	pdglEnable(GL_TEXTURE_2D);
	pdglPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	pdglColor4f(1,1,1,1);
}

PDGL_API int Draw_SetSolid2_2D(float aspect, float size,
	float *org, float *angles, int xo, int yo, int xs, int ys)
{
	float h2;

//	printf("%g %g %g\n", org[0], org[1], org[2]);
	h2=size/aspect;

	pdgl_view_xo=xo;
	pdgl_view_yo=yo;
	pdgl_view_xs=xs;
	pdgl_view_ys=ys;

	pdglViewport(xo, yo, xs, ys);

	pdglMatrixMode(GL_PROJECTION);
	pdglLoadIdentity();

	pdglOrtho(-size, size, -h2, h2, -999999999, 999999999);

	pdglRotatef(-angles[2],  0, 0, 1);
	pdglRotatef(-angles[0],  1, 0, 0);
	pdglRotatef(-angles[1],  0, 1, 0);
	pdglTranslated(-org[0],  -org[1],  -org[2]);

	pdglMatrixMode(GL_MODELVIEW);
	pdglLoadIdentity();

	pdglDisable(GL_DEPTH_TEST);
	pdglDisable(GL_CULL_FACE);
	pdglDisable(GL_ALPHA_TEST);

	pdglEnable(GL_BLEND);
	pdglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	pdglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	pdglShadeModel(GL_SMOOTH);

	pdglEnable(GL_TEXTURE_2D);
	pdglPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

//	pdglEnable(GL_STENCIL_TEST);
	pdglEnable(GL_SCISSOR_TEST);

	pdglColor4f(1,1,1,1);
}


PDGL_API int Draw_SetOrtho_3D(float aspect, float size,
	float *org, float *angles, int x, int y, int w, int h)
{
	float h2;

	pdgl_view_xo=x;
	pdgl_view_yo=y;
	pdgl_view_xs=w;
	pdgl_view_ys=h;

	pdglViewport(x, y, w, h);

	pdglMatrixMode(GL_PROJECTION);
	pdglLoadIdentity();

	h2=size/aspect;
	pdglOrtho(-size, size, -h2, h2, -999999999, 999999999);

//	if(leftcoords)
//	pdglScalef(1, 1, -1);

	pdglRotatef(-angles[1],  0, 1, 0);
	pdglRotatef(-angles[0],  1, 0, 0);
	pdglRotatef(-angles[2],  0, 0, 1);
	pdglTranslatef(-org[0],  -org[1],  -org[2]);

	pdglDisable(GL_DEPTH_TEST);
	pdglDisable(GL_CULL_FACE);
	pdglEnable(GL_BLEND);
	pdglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	pdglDisable(GL_TEXTURE_2D);
	pdglPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	pdglColor4f(0,0,0,1);

	pdglMatrixMode(GL_MODELVIEW);
	pdglLoadIdentity();
}

PDGL_API int Draw_SetSolid3_2D(float lxs, float lys,
	float *org, int xo, int yo, int xs, int ys)
{
	pdgl_view_xo=xo;
	pdgl_view_yo=yo;
	pdgl_view_xs=xs;
	pdgl_view_ys=ys;

	pdglViewport(xo, yo, xs, ys);

	pdglMatrixMode(GL_PROJECTION);
	pdglLoadIdentity();

	pdglOrtho(0, lxs, 0, lys, 0, 999999999);
	pdglTranslated(-org[0],  -org[1],  -org[2]);

	pdglMatrixMode(GL_MODELVIEW);
	pdglLoadIdentity();

	pdglEnable(GL_DEPTH_TEST);
	pdglDisable(GL_CULL_FACE);
	pdglDisable(GL_ALPHA_TEST);

	pdglDepthFunc(GL_LEQUAL);

	pdglEnable(GL_BLEND);
	pdglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	pdglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	pdglShadeModel(GL_SMOOTH);

	pdglEnable(GL_TEXTURE_2D);
	pdglPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

//	pdglEnable(GL_STENCIL_TEST);
//	pdglEnable(GL_SCISSOR_TEST);

	pdglColor4f(1,1,1,1);

	pdglClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
		GL_ACCUM_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

PDGL_API int Draw_SetSolid4_2D(
	float *mins, float *maxs,
	float *org, float *ang, int xo, int yo, int xs, int ys)
{
	pdgl_view_xo=xo;
	pdgl_view_yo=yo;
	pdgl_view_xs=xs;
	pdgl_view_ys=ys;

	pdglViewport(xo, yo, xs, ys);

	pdglMatrixMode(GL_PROJECTION);
	pdglLoadIdentity();

	pdglOrtho(mins[0], maxs[0], mins[1], maxs[1], mins[2], maxs[2]);

	pdglRotatef(-ang[1],  0, 1, 0);
	pdglRotatef(-ang[0],  1, 0, 0);
	pdglRotatef(-ang[2],  0, 0, 1);
	pdglTranslatef(-org[0],  -org[1],  -org[2]);

	pdglMatrixMode(GL_MODELVIEW);
	pdglLoadIdentity();

	pdglEnable(GL_DEPTH_TEST);
//	pdglDisable(GL_CULL_FACE);
	pdglDisable(GL_ALPHA_TEST);

	pdglEnable(GL_CULL_FACE);
	pdglCullFace(GL_FRONT);
	pdglFrontFace(GL_CCW);

	pdglDepthFunc(GL_LEQUAL);

	pdglEnable(GL_BLEND);
	pdglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	pdglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	pdglShadeModel(GL_SMOOTH);

	pdglEnable(GL_TEXTURE_2D);
	pdglPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

//	pdglEnable(GL_STENCIL_TEST);
//	pdglEnable(GL_SCISSOR_TEST);

	pdglColor4f(1,1,1,1);

	pdglClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
		GL_ACCUM_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

PDGL_API int Draw_SetPerspective_3D(float aspect, float fov, float d,
	float *org, float *angles, int xo, int yo, int xs, int ys)
{
	GLdouble xmin, xmax, ymin, ymax, dst;
	float pos2[16];

	pdgl_view_xo=xo;
	pdgl_view_yo=yo;
	pdgl_view_xs=xs;
	pdgl_view_ys=ys;

	pdglViewport(xo, yo, xs, ys);

	pdglMatrixMode(GL_PROJECTION);
	pdglLoadIdentity();

//	if(leftcoords)

	dst=0.1;

	xmax=dst*tan(fov*(M_PI/360.0));
	xmin=-xmax;

	ymin=xmin/aspect;
	ymax=xmax/aspect;

	pdglFrustum(xmin, xmax, ymin, ymax, dst, 100000);
//	pdglFrustum(xmin, xmax, ymin, ymax, dst, 100000000);
//	pdglFrustum(xmin, xmax, ymin, ymax, dst, 1000000);

//	pdglScalef(1, 1, -1);

	pdglTranslatef(0, 0, -d);

	pdglRotatef(-angles[1],  0, 1, 0);
	pdglRotatef(-angles[0],  1, 0, 0);
	pdglRotatef(-angles[2],  0, 0, 1);
	pdglTranslatef(-org[0],  -org[1],  -org[2]);

//	MatN_SwapOrder(pos, pos2, 4);
//	pdglMultMatrixd(pos2);

	pdglMatrixMode(GL_MODELVIEW);
	pdglLoadIdentity();

	pdglEnable(GL_DEPTH_TEST);
	pdglDepthFunc(GL_LESS);

	pdglEnable(GL_CULL_FACE);
	pdglCullFace(GL_FRONT);
	pdglFrontFace(GL_CCW);

	pdglDisable(GL_SCISSOR_TEST);
	pdglDisable(GL_ALPHA_TEST);

//	pdglDisable(GL_CULL_FACE);

	pdglEnable(GL_BLEND);
	pdglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	pdglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	pdglShadeModel(GL_SMOOTH);

	pdglEnable(GL_TEXTURE_2D);
	pdglPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	pdglColor4f(0,0,0,1);

	pdglHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}

PDGL_API int Draw_SetPerspective2_3D(float aspect, float fov, float width,
	float *org, float *rot, int xo, int yo, int xs, int ys)
{
	GLdouble xmin, xmax, ymin, ymax, dst;
	float pos[16];

	pdgl_view_xo=xo;
	pdgl_view_yo=yo;
	pdgl_view_xs=xs;
	pdgl_view_ys=ys;

	pdglViewport(xo, yo, xs, ys);

	pdglMatrixMode(GL_PROJECTION);
	pdglLoadIdentity();

	dst=0.1;

//	xmax=dst*tan(fov*(M_PI/360.0));
//	xmin=-xmax;

	xmax=width/2;
	xmin=-xmax;

	ymin=xmin/aspect;
	ymax=xmax/aspect;

	dst=xmax/tan(fov*(M_PI/360.0));

	pdglFrustum(xmin, xmax, ymin, ymax, dst, 100000);
//	pdglFrustum(xmin, xmax, ymin, ymax, dst, 100000000);

//	pdglScalef(1, 1, -1);
	pdglTranslatef(0, 0, -dst);

//	Mat3to4F_Transpose(rot, pos);

	pos[0]=rot[0]; 	pos[1]=rot[3];	pos[2]=rot[6];	pos[3]=0;
	pos[4]=rot[1];	pos[5]=rot[4];	pos[6]=rot[7];	pos[7]=0;
	pos[8]=rot[2];	pos[9]=rot[5];	pos[10]=rot[8];	pos[11]=0;
	pos[12]=0;	pos[13]=0;	pos[14]=0;	pos[15]=1;

	pdglRotatef(-90,  1, 0, 0);

	pdglMultMatrixf(pos);
	pdglTranslatef(-org[0],  -org[1],  -org[2]);


	pdglMatrixMode(GL_MODELVIEW);
	pdglLoadIdentity();

	pdglEnable(GL_DEPTH_TEST);
	pdglDepthFunc(GL_LESS);

	pdglEnable(GL_CULL_FACE);
	pdglCullFace(GL_FRONT);
	pdglFrontFace(GL_CCW);

	pdglDisable(GL_SCISSOR_TEST);
	pdglDisable(GL_ALPHA_TEST);

//	pdglDisable(GL_CULL_FACE);

	pdglEnable(GL_BLEND);
	pdglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	pdglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	pdglShadeModel(GL_SMOOTH);

	pdglEnable(GL_TEXTURE_2D);
	pdglPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	pdglColor4f(0,0,0,1);

	pdglHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}

PDGL_API int Draw_Square(int x, int y, int w, int h)
{
	pdglBegin(GL_QUADS);
	pdglVertex2f(x,	y);
	pdglVertex2f(x,	y+h);
	pdglVertex2f(x+w,	y+h);
	pdglVertex2f(x+w,	y);
	pdglEnd();

	return(0);
}

PDGL_API int Draw_FillSquare(int x, int y, int w, int h, float c[4])
{
	pdglDisable(GL_TEXTURE_2D);

	pdglColor4fv(c);

	pdglBegin(GL_QUADS);
	pdglVertex2f(x,	y);
	pdglVertex2f(x,	y+h);
	pdglVertex2f(x+w,	y+h);
	pdglVertex2f(x+w,	y);
	pdglEnd();

	pdglEnable(GL_TEXTURE_2D);
}

PDGL_API int Draw_FillSquareRGBA(int x, int y, int w, int h,
	int r, int g, int b, int a)
{
	pdglDisable(GL_TEXTURE_2D);

	pdglColor4f(r/255.0, g/255.0, b/255.0, a/255.0);

	pdglBegin(GL_QUADS);
	pdglVertex2f(x,	y);
	pdglVertex2f(x,	y+h);
	pdglVertex2f(x+w,	y+h);
	pdglVertex2f(x+w,	y);
	pdglEnd();

	pdglEnable(GL_TEXTURE_2D);
}

PDGL_API int Draw_Circle(int ox, int oy, int r)
{
	int i;
	float x, y;

	pdglBegin(GL_POLYGON);
	for(i=0; i<16; i++)
	{
		x=ox+(sin(M_PI*(i/8.0))*r);
		y=oy+(cos(M_PI*(i/8.0))*r);
		pdglVertex2f(x, y);
	}
	pdglEnd();

	return(0);
}

PDGL_API int Draw_FillCircle(int ox, int oy, int ra, int r, int g, int b, int a)
{
	pdglDisable(GL_TEXTURE_2D);
	pdglColor4f(r/255.0, g/255.0, b/255.0, a/255.0);

	Draw_Circle(ox, oy, ra);
	return(0);
}

PDGL_API int Draw_PictureCircle(int ox, int oy, int r, int texnum)
{
	int i;
	float a, sa, ca, x, y, s, t;

	pdglBegin(GL_POLYGON);
	for(i=0; i<16; i++)
	{
		a=M_PI*(i/8.0);
		sa=sin(a);
		ca=cos(a);
		s=(ca+1)*0.5;
		t=-(sa+1)*0.5;

		x=ox+(ca*r);
		y=oy+(sa*r);

		pdglTexCoord2f(s, t);
		pdglVertex2f(x, y);
	}
	pdglEnd();

	return(0);
}

PDGL_API int Draw_Bind(int num)
{
	pdglBindTexture(GL_TEXTURE_2D, num);
	return(0);
}

PDGL_API int Draw_DisableTextures()
{
	pdglDisable(GL_TEXTURE_2D);
	return(0);
}

PDGL_API int Draw_EnableTextures()
{
	pdglEnable(GL_TEXTURE_2D);
	return(0);
}

PDGL_API int Draw_ColorRGBA(int r, int g, int b, int a)
{
	pdglColor4f(r/255.0, g/255.0, b/255.0, a/255.0);
	return(0);
}

PDGL_API int Draw_Picture(int x, int y, int w, int h, int num)
{
	float s1, s2;
	float t1, t2;

	s1=0;
	s2=1;
	t1=1;
	t2=0;

//	printf("tex=%d\n", num);

	pdglBindTexture(GL_TEXTURE_2D, num);

	pdglBegin(GL_QUADS);
	pdglTexCoord2f(s1, t1);
	pdglVertex2f(x, y);
	pdglTexCoord2f(s1, t2);
	pdglVertex2f(x, y+h);
	pdglTexCoord2f(s2, t2);
	pdglVertex2f(x+w, y+h);
	pdglTexCoord2f(s2, t1);
	pdglVertex2f(x+w, y);
	pdglEnd();

	return(0);
}

#endif
