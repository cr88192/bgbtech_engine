#include <pdgl.h>

#ifdef GLES2

PDGL_API int Draw_SetSolid_2D(float aspect, int xo, int yo, int xs, int ys)
{
	glViewport (xo, yo, xs, ys);

//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity ();
//	glOrtho (-1000, 1000, -1000/aspect, 1000/aspect, -99999, 99999);

//	glMatrixMode(GL_MODELVIEW);
//	glLoadIdentity ();

//	glDisable (GL_DEPTH_TEST);
//	glDisable (GL_CULL_FACE);
//	glEnable (GL_BLEND);
//	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//	glEnable (GL_TEXTURE_2D);
//	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	pdglColor4f (1,1,1,1);
}

PDGL_API int Draw_SetSolid2_2D(float aspect, float size,
	float *org, float *angles, int xo, int yo, int xs, int ys)
{
	float h2;

//	printf("%g %g %g\n", org[0], org[1], org[2]);
	h2=size/aspect;

	glViewport (xo, yo, xs, ys);

//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity ();

//	glOrtho  (-size, size, -h2, h2, -999999999, 999999999);

//	glRotatef (-angles[2],  0, 0, 1);
//	glRotatef (-angles[0],  1, 0, 0);
//	glRotatef (-angles[1],  0, 1, 0);
//	glTranslated (-org[0],  -org[1],  -org[2]);

//	glMatrixMode(GL_MODELVIEW);
//	glLoadIdentity ();

//	glDisable (GL_DEPTH_TEST);
//	glDisable (GL_CULL_FACE);
//	glDisable(GL_ALPHA_TEST);

//	glEnable (GL_BLEND);
//	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
//	glShadeModel (GL_SMOOTH);

//	glEnable (GL_TEXTURE_2D);
//	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

//	glEnable(GL_STENCIL_TEST);
//	glEnable(GL_SCISSOR_TEST);

	pdglColor4f (1,1,1,1);
}


PDGL_API int Draw_SetOrtho_3D(float aspect, float size,
	float *org, float *angles, int x, int y, int w, int h)
{
	float h2;

	glViewport (x, y, w, h);

#if 0
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity ();

	h2=size/aspect;
	glOrtho  (-size, size, -h2, h2, -999999999, 999999999);

//	if(leftcoords)
//	glScalef(1, 1, -1);

	glRotatef (-angles[1],  0, 1, 0);
	glRotatef (-angles[0],  1, 0, 0);
	glRotatef (-angles[2],  0, 0, 1);
	glTranslatef (-org[0],  -org[1],  -org[2]);

	glDisable (GL_DEPTH_TEST);
	glDisable (GL_CULL_FACE);
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDisable (GL_TEXTURE_2D);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glColor4f (0,0,0,1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity ();
#endif
}

PDGL_API int Draw_SetSolid3_2D(float lxs, float lys,
	float *org, int xo, int yo, int xs, int ys)
{
	glViewport (xo, yo, xs, ys);

#if 0
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity ();

	glOrtho  (0, lxs, 0, lys, 0, 999999999);
	glTranslated (-org[0],  -org[1],  -org[2]);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_ALPHA_TEST);

	glDepthFunc(GL_LEQUAL);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glShadeModel(GL_SMOOTH);

	glEnable(GL_TEXTURE_2D);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

//	glEnable(GL_STENCIL_TEST);
//	glEnable(GL_SCISSOR_TEST);

#endif

	pdglColor4f (1,1,1,1);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
		GL_STENCIL_BUFFER_BIT);
}

PDGL_API int Draw_SetSolid4_2D(
	float *mins, float *maxs,
	float *org, float *ang, int xo, int yo, int xs, int ys)
{
	glViewport (xo, yo, xs, ys);

#if 0
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity ();

	glOrtho  (mins[0], maxs[0], mins[1], maxs[1], mins[2], maxs[2]);

	glRotatef (-ang[1],  0, 1, 0);
	glRotatef (-ang[0],  1, 0, 0);
	glRotatef (-ang[2],  0, 0, 1);
	glTranslatef (-org[0],  -org[1],  -org[2]);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_DEPTH_TEST);
//	glDisable(GL_CULL_FACE);
	glDisable(GL_ALPHA_TEST);

	glEnable (GL_CULL_FACE);
	glCullFace (GL_FRONT);
	glFrontFace (GL_CCW);

	glDepthFunc(GL_LEQUAL);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glShadeModel(GL_SMOOTH);

	glEnable(GL_TEXTURE_2D);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

//	glEnable(GL_STENCIL_TEST);
//	glEnable(GL_SCISSOR_TEST);
#endif

	pdglColor4f (1,1,1,1);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
		GL_STENCIL_BUFFER_BIT);
}

PDGL_API int Draw_SetPerspective_3D(float aspect, float fov, float d,
	float *org, float *angles, int xo, int yo, int xs, int ys)
{
	double xmin, xmax, ymin, ymax, dst;
	float pos2[16];

	glViewport (xo, yo, xs, ys);

#if 0
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity ();

//	if(leftcoords)

	dst=0.1;

	xmax = dst*tan(fov*(M_PI/360.0));
	xmin = -xmax;

	ymin = xmin/aspect;
	ymax = xmax/aspect;

	glFrustum  (xmin, xmax, ymin, ymax, dst, 100000);
//	glFrustum  (xmin, xmax, ymin, ymax, dst, 100000000);
//	glFrustum  (xmin, xmax, ymin, ymax, dst, 1000000);

//	glScalef(1, 1, -1);

	glTranslatef(0, 0, -d);

	glRotatef (-angles[1],  0, 1, 0);
	glRotatef (-angles[0],  1, 0, 0);
	glRotatef (-angles[2],  0, 0, 1);
	glTranslatef (-org[0],  -org[1],  -org[2]);

//	MatN_SwapOrder(pos, pos2, 4);
//	glMultMatrixd(pos2);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity ();

	glEnable (GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable (GL_CULL_FACE);
	glCullFace (GL_FRONT);
	glFrontFace (GL_CCW);

	glDisable(GL_SCISSOR_TEST);
	glDisable(GL_ALPHA_TEST);

//	glDisable (GL_CULL_FACE);

	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glShadeModel (GL_SMOOTH);

	glEnable (GL_TEXTURE_2D);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glColor4f (0,0,0,1);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
#endif

}

PDGL_API int Draw_SetPerspective2_3D(float aspect, float fov, float width,
	float *org, float *rot, int xo, int yo, int xs, int ys)
{
	double xmin, xmax, ymin, ymax, dst;
	float pos[16];

	glViewport (xo, yo, xs, ys);

#if 0
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity ();

	dst=0.1;

//	xmax = dst*tan(fov*(M_PI/360.0));
//	xmin = -xmax;

	xmax = width/2;
	xmin = -xmax;

	ymin = xmin/aspect;
	ymax = xmax/aspect;

	dst = xmax/tan(fov*(M_PI/360.0));

//	glFrustum  (xmin, xmax, ymin, ymax, dst, 100000);
	glFrustum  (xmin, xmax, ymin, ymax, dst, 100000000);

//	glScalef(1, 1, -1);
	glTranslatef(0, 0, -dst);

//	Mat3to4F_Transpose(rot, pos);

	pos[0]=rot[0]; 	pos[1]=rot[3];	pos[2]=rot[6];	pos[3]=0;
	pos[4]=rot[1];	pos[5]=rot[4];	pos[6]=rot[7];	pos[7]=0;
	pos[8]=rot[2];	pos[9]=rot[5];	pos[10]=rot[8];	pos[11]=0;
	pos[12]=0;	pos[13]=0;	pos[14]=0;	pos[15]=1;

	glRotatef (-90,  1, 0, 0);

	glMultMatrixf(pos);
	glTranslatef (-org[0],  -org[1],  -org[2]);


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity ();

	glEnable (GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable (GL_CULL_FACE);
	glCullFace (GL_FRONT);
	glFrontFace (GL_CCW);

	glDisable(GL_SCISSOR_TEST);
	glDisable(GL_ALPHA_TEST);

//	glDisable (GL_CULL_FACE);

	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glShadeModel (GL_SMOOTH);

	glEnable (GL_TEXTURE_2D);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glColor4f (0,0,0,1);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
#endif

}

PDGL_API int Draw_Square(int x, int y, int w, int h)
{
	pdglBegin(PDGL_QUADS);
	pdglVertex2f(x,	y);
	pdglVertex2f(x,	y+h);
	pdglVertex2f(x+w,	y+h);
	pdglVertex2f(x+w,	y);
	pdglEnd();

	return(0);
}

PDGL_API int Draw_FillSquare(int x, int y, int w, int h, float c[4])
{
	pdglDisableTexture2D();

	pdglColor4fv(c);

	pdglBegin(PDGL_QUADS);
	pdglVertex2f(x,	y);
	pdglVertex2f(x,	y+h);
	pdglVertex2f(x+w,	y+h);
	pdglVertex2f(x+w,	y);
	pdglEnd();

	pdglEnableTexture2D();
}

PDGL_API int Draw_FillSquareRGBA(int x, int y, int w, int h,
	int r, int g, int b, int a)
{
	pdglDisableTexture2D();

	pdglColor4f(r/255.0, g/255.0, b/255.0, a/255.0);

	pdglBegin(PDGL_QUADS);
	pdglVertex2f(x,	y);
	pdglVertex2f(x,	y+h);
	pdglVertex2f(x+w,	y+h);
	pdglVertex2f(x+w,	y);
	pdglEnd();

	pdglEnableTexture2D();
}

PDGL_API int Draw_Circle(int ox, int oy, int r)
{
	int i;
	float x, y;

	pdglBegin(PDGL_POLYGON);
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
	pdglDisableTexture2D();
	pdglColor4f(r/255.0, g/255.0, b/255.0, a/255.0);

	Draw_Circle(ox, oy, ra);
	return(0);
}

PDGL_API int Draw_PictureCircle(int ox, int oy, int r, int texnum)
{
	int i;
	float a, sa, ca, x, y, s, t;

	pdglBegin(PDGL_POLYGON);
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
	glBindTexture(GL_TEXTURE_2D, num);
	return(0);
}

PDGL_API int Draw_DisableTextures()
{
	pdglDisableTexture2D();
	return(0);
}

PDGL_API int Draw_EnableTextures()
{
	pdglEnableTexture2D();
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

	glBindTexture(GL_TEXTURE_2D, num);

	pdglBegin(PDGL_QUADS);
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
