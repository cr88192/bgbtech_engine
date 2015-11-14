#include <windows.h>
#include <GL/GL.h>
#include <math.h>

#ifndef M_PI
#define M_PI		3.1415926535897932384626433832795
#endif

int Draw_SetSolid_2D(float aspect, int xo, int yo, int xs, int ys)
{
	glViewport (xo, yo, xs, ys);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity ();
	glOrtho (-1000, 1000, -1000/aspect, 1000/aspect, -99999, 99999);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity ();

	glDisable (GL_DEPTH_TEST);
	glDisable (GL_CULL_FACE);
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable (GL_TEXTURE_2D);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glColor4f (1,1,1,1);
}

int Draw_SetSolid2_2D(float aspect, float size,
	float *org, float *angles, int xo, int yo, int xs, int ys)
{
	float h2;

//	printf("%g %g %g\n", org[0], org[1], org[2]);
	h2=size/aspect;

	glViewport (xo, yo, xs, ys);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity ();

	glOrtho  (-size, size, -h2, h2, -999999999, 999999999);

	glRotatef (-angles[2],  0, 0, 1);
	glRotatef (-angles[0],  1, 0, 0);
	glRotatef (-angles[1],  0, 1, 0);
	glTranslated (-org[0],  -org[1],  -org[2]);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity ();

	glDisable (GL_DEPTH_TEST);
	glDisable (GL_CULL_FACE);
	glDisable(GL_ALPHA_TEST);

	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glShadeModel (GL_SMOOTH);

	glEnable (GL_TEXTURE_2D);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

//	glEnable(GL_STENCIL_TEST);
	glEnable(GL_SCISSOR_TEST);

	glColor4f (1,1,1,1);
}


int Draw_SetOrtho_3D(float aspect, float size,
	float *org, float *angles, int x, int y, int w, int h)
{
	float h2;

	glViewport (x, y, w, h);

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
}

int Draw_SetSolid3_2D(float lxs, float lys,
	float *org, int xo, int yo, int xs, int ys)
{
	glViewport (xo, yo, xs, ys);

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

	glColor4f (1,1,1,1);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
		GL_ACCUM_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

int Draw_SetPerspective_3D(float aspect, float fov, float d,
	float *org, float *angles, int xo, int yo, int xs, int ys)
{
	GLdouble xmin, xmax, ymin, ymax, dst;
	float pos2[16];

	glViewport (xo, yo, xs, ys);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity ();

//	if(leftcoords)

	dst=0.1;

	xmax = dst*tan(fov*(M_PI/360.0));
	xmin = -xmax;

	ymin = xmin/aspect;
	ymax = xmax/aspect;

//	glFrustum  (xmin, xmax, ymin, ymax, dst, 100000);
	glFrustum  (xmin, xmax, ymin, ymax, dst, 100000000);

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
}

int Draw_Square(int x, int y, int w, int h)
{
	glBegin(GL_QUADS);
	glVertex2f(x,	y);
	glVertex2f(x,	y+h);
	glVertex2f(x+w,	y+h);
	glVertex2f(x+w,	y);
	glEnd();

	return(0);
}

int Draw_FillSquare(int x, int y, int w, int h, float c[4])
{
	glDisable(GL_TEXTURE_2D);

	glColor4fv(c);

	glBegin(GL_QUADS);
	glVertex2f(x,	y);
	glVertex2f(x,	y+h);
	glVertex2f(x+w,	y+h);
	glVertex2f(x+w,	y);
	glEnd();

	glEnable(GL_TEXTURE_2D);
}

int Draw_FillSquareRGBA(int x, int y, int w, int h,
	int r, int g, int b, int a)
{
	glDisable(GL_TEXTURE_2D);

	glColor4f(r/255.0, g/255.0, b/255.0, a/255.0);

	glBegin(GL_QUADS);
	glVertex2f(x,	y);
	glVertex2f(x,	y+h);
	glVertex2f(x+w,	y+h);
	glVertex2f(x+w,	y);
	glEnd();

	glEnable(GL_TEXTURE_2D);
}

int Draw_Circle(int ox, int oy, int r)
{
	int i;
	float x, y;

	glBegin(GL_POLYGON);
	for(i=0; i<16; i++)
	{
		x=ox+(sin(M_PI*(i/8.0))*r);
		y=oy+(cos(M_PI*(i/8.0))*r);
		glVertex2f(x, y);
	}
	glEnd();

	return(0);
}

int Draw_FillCircle(int ox, int oy, int ra, int r, int g, int b, int a)
{
	glDisable(GL_TEXTURE_2D);
	glColor4f(r/255.0, g/255.0, b/255.0, a/255.0);

	Draw_Circle(ox, oy, ra);
	return(0);
}

int Draw_PictureCircle(int ox, int oy, int r, int texnum)
{
	int i;
	float a, sa, ca, x, y, s, t;

	glBegin(GL_POLYGON);
	for(i=0; i<16; i++)
	{
		a=M_PI*(i/8.0);
		sa=sin(a);
		ca=cos(a);
		s=(ca+1)*0.5;
		t=-(sa+1)*0.5;

		x=ox+(ca*r);
		y=oy+(sa*r);

		glTexCoord2f(s, t);
		glVertex2f(x, y);
	}
	glEnd();

	return(0);
}

int Draw_Bind(int num)
{
//	glBindTexture(GL_TEXTURE_2D, num);
	return(0);
}

int Draw_DisableTextures()
{
	glDisable(GL_TEXTURE_2D);
	return(0);
}

int Draw_EnableTextures()
{
	glEnable(GL_TEXTURE_2D);
	return(0);
}

int Draw_ColorRGBA(int r, int g, int b, int a)
{
	glColor4f(r/255.0, g/255.0, b/255.0, a/255.0);
	return(0);
}

int Draw_Picture(int x, int y, int w, int h, int num)
{
	float s1, s2;
	float t1, t2;

	s1=0;
	s2=1;
	t1=1;
	t2=0;

//	printf("tex=%d\n", num);

//	glBindTexture(GL_TEXTURE_2D, num);

	glBegin(GL_QUADS);
	glTexCoord2f(s1, t1);
	glVertex2f(x, y);
	glTexCoord2f(s1, t2);
	glVertex2f(x, y+h);
	glTexCoord2f(s2, t2);
	glVertex2f(x+w, y+h);
	glTexCoord2f(s2, t1);
	glVertex2f(x+w, y);
	glEnd();

	return(0);
}
