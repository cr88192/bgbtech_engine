#include <pdgl.h>

#ifdef GLES2

float *pdgl_drawprim_xyzbuf;
float *pdgl_drawprim_stbuf;
float *pdgl_drawprim_nbuf;
float *pdgl_drawprim_clrbuf;
// float *pdgl_drawprim_nxbuf;
// float *pdgl_drawprim_nybuf;

int pdgl_drawprim_nvec;
int pdgl_drawprim_mvec;

int pdgl_drawprim_prim;
int pdgl_drawprim_blend_src;
int pdgl_drawprim_blend_dst;

float pdgl_drawprim_sttmp[2];
float pdgl_drawprim_ntmp[4];
float pdgl_drawprim_clr[4];
// float pdgl_drawprim_nxtmp[4];
// float pdgl_drawprim_nytmp[4];

static int pdgl_drawprim_init=0;

float pdgl_drawprim_matrixstack[4][16][16];
int pdgl_drawprim_matrixstackpos[4]={-1, -1, -1, -1};
float pdgl_drawprim_matrix[4][16];
int pdgl_drawprim_matrixmode;

PDGL_API void pdglBegin(int prim)
{
//	PDGL_DrawPrim_Init();
	pdgl_drawprim_nvec=0;
	pdgl_drawprim_prim=prim;
}

PDGL_API void pdglEnd(void)
{
	PDGL_DrawPrim_DrawArrays(pdgl_drawprim_prim,
		pdgl_drawprim_xyzbuf, pdgl_drawprim_stbuf,
		pdgl_drawprim_nbuf, pdgl_drawprim_clrbuf,
		pdgl_drawprim_nvec);
}

void PDGL_DrawPrim_DrawArrays(
	int prim,
	float *xyz, float *st, float *norm, float *rgba,
	int nxyz)
{
	int i;

	i=4*sizeof(float);
	PDGL_DrawPrim_DrawArraysNormalTexRGB(
		prim, 0, nxyz,
		3, GL_FLOAT, i, xyz,
		2, GL_FLOAT, 0, st,
		3, GL_FLOAT, i, norm,
		4, GL_FLOAT, 0, rgba);

#if 0
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	i=4*sizeof(float);
	glVertexPointer(3, GL_FLOAT, i, xyz);
	glTexCoordPointer(2, GL_FLOAT, 0, st);
	glNormalPointer(GL_FLOAT, i, norm);
	glColorPointer(4, GL_FLOAT, 0, rgba);
	glDrawArrays(prim, 0, nxyz);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
#endif
}

void PDGL_DrawPrim_VertexCheckExpand()
{
	int i;

	if(pdgl_drawprim_nvec>=pdgl_drawprim_mvec)
	{
		i=pdgl_drawprim_mvec+(pdgl_drawprim_mvec>>1);
		pdgl_drawprim_xyzbuf=
			gcrealloc(pdgl_drawprim_xyzbuf, i*4*sizeof(float));
		pdgl_drawprim_stbuf=
			gcrealloc(pdgl_drawprim_stbuf, i*2*sizeof(float));
		pdgl_drawprim_nbuf=
			gcrealloc(pdgl_drawprim_nbuf, i*4*sizeof(float));
		pdgl_drawprim_clrbuf=
			gcrealloc(pdgl_drawprim_clrbuf, i*4*sizeof(float));
		pdgl_drawprim_mvec=i;

		//light
//		pdgl_drawprim_nxbuf=
//			gcrealloc(pdgl_drawprim_nxbuf, i*4*sizeof(float));
//		pdgl_drawprim_nybuf=
//			gcrealloc(pdgl_drawprim_nybuf, i*4*sizeof(float));
	}
}

PDGL_API void pdglVertex3fv(float *v)
{
	float *t;
	int i;

	PDGL_DrawPrim_VertexCheckExpand();

	t=pdgl_drawprim_stbuf+pdgl_drawprim_nvec*2;
	V2F_COPY(pdgl_drawprim_sttmp, t);

	t=pdgl_drawprim_nbuf+pdgl_drawprim_nvec*4;
	V4F_COPY(pdgl_drawprim_ntmp, t);

	t=pdgl_drawprim_xyzbuf+pdgl_drawprim_nvec*4;
	V3F_COPY(v, t); t[3]=0;

	t=pdgl_drawprim_clrbuf+pdgl_drawprim_nvec*4;
	V4F_COPY(pdgl_drawprim_clr, t);

#if 0
//	if(pdgl_drawprim_light)
	if(1)
	{
		t=pdgl_drawprim_nxbuf+pdgl_drawprim_nvec*4;
		V4F_COPY(pdgl_drawprim_nxtmp, t);

		t=pdgl_drawprim_nybuf+pdgl_drawprim_nvec*4;
		V4F_COPY(pdgl_drawprim_nytmp, t);
	}
#endif

	pdgl_drawprim_nvec++;
}

PDGL_API void pdglVertex3dv(double *v)
{
	float tv[3];
	tv[0]=v[0]; tv[1]=v[1]; tv[2]=v[2];
	pdglVertex3fv(tv);
}

PDGL_API void pdglTexCoord2fv(float *v)
{
	float *t;
	V2F_COPY(v, pdgl_drawprim_sttmp);
}

PDGL_API void pdglNormal3fv(float *v)
{
	float *t;

	V3F_COPY(v, pdgl_drawprim_ntmp);
	pdgl_drawprim_ntmp[3]=0;
}

PDGL_API void pdglNormal4fv(float *v)
{
	float *t;
	V4F_COPY(v, pdgl_drawprim_ntmp);
}

PDGL_API void pdglNormal3sbv(sbyte *v)
{
//	float *t;

	pdgl_drawprim_ntmp[0]=v[0]*(1.0/127.0);
	pdgl_drawprim_ntmp[1]=v[1]*(1.0/127.0);
	pdgl_drawprim_ntmp[2]=v[2]*(1.0/127.0);

//	V3F_COPY(v, pdgl_drawprim_ntmp);
	pdgl_drawprim_ntmp[3]=0;
}

PDGL_API void pdglNormal3bv(sbyte *v)
{
	float ntmp[3];

	ntmp[0]=v[0]*(1.0/127.0);
	ntmp[1]=v[1]*(1.0/127.0);
	ntmp[2]=v[2]*(1.0/127.0);
	pdglNormal3fv(ntmp);
}

PDGL_API void pdglColor3fv(float *v)
{
	V3F_COPY(v, pdgl_drawprim_clr);
//	glColor3fv(pdgl_drawprim_clr);
}

PDGL_API void pdglColor4fv(float *v)
{
	V4F_COPY(v, pdgl_drawprim_clr);
//	glColor4fv(pdgl_drawprim_clr);
}

PDGL_API void pdglColor4ubv(byte *v)
{
	pdgl_drawprim_clr[0]=v[0]*(1.0/255.0);
	pdgl_drawprim_clr[1]=v[1]*(1.0/255.0);
	pdgl_drawprim_clr[2]=v[2]*(1.0/255.0);
	pdgl_drawprim_clr[3]=v[3]*(1.0/255.0);

//	V4F_COPY(v, pdgl_drawprim_clr);
//	glColor4fv(pdgl_drawprim_clr);
}

PDGL_API void pdglColor3f(float r, float g, float b)
{
	pdgl_drawprim_clr[0]=r;
	pdgl_drawprim_clr[1]=g;
	pdgl_drawprim_clr[2]=b;
}

PDGL_API void pdglColor4f(float r, float g, float b, float a)
{
	pdgl_drawprim_clr[0]=r;
	pdgl_drawprim_clr[1]=g;
	pdgl_drawprim_clr[2]=b;
	pdgl_drawprim_clr[3]=a;
}

PDGL_API void pdglColor4ub(int r, int g, int b, int a)
{
	pdglColor4f(r/255.0, g/255.0, b/255.0, a/255.0);
}

PDGL_API void pdglTexCoord2f(float s, float t)
{
	pdgl_drawprim_sttmp[0]=s;
	pdgl_drawprim_sttmp[1]=t;
}

PDGL_API void pdglVertex2f(float x, float y)
{
	float tv[3];
	tv[0]=x; tv[1]=y; tv[2]=0;
	pdglVertex3fv(tv);
}

PDGL_API void pdglVertex3f(float x, float y, float z)
{
	float tv[3];
	tv[0]=x; tv[1]=y; tv[2]=z;
	pdglVertex3fv(tv);
}

PDGL_API void pdglNormal3f(float x, float y, float z)
{
	pdgl_drawprim_ntmp[0]=x;
	pdgl_drawprim_ntmp[1]=y;
	pdgl_drawprim_ntmp[2]=z;
	pdgl_drawprim_ntmp[3]=0;
}

PDGL_API void pdglColorMask(bool r, bool g, bool b, bool a)
{
	glColorMask(r, g, b, a);
}

PDGL_API void pdglEnableTexture2D(void)
{
//	glEnable(GL_TEXTURE_2D);
}

PDGL_API void pdglDisableTexture2D(void)
{
	//Fixme: bind white texture
//	glDisable(GL_TEXTURE_2D);
}

PDGL_API void pdglEnableLighting(void)
{
//	glEnable(GL_LIGHTING);
}

PDGL_API void pdglDisableLighting(void)
{
//	glDisable(GL_LIGHTING);
}


PDGL_API void pdglModelviewMatrix(void)
{
	pdgl_drawprim_matrixmode=0;
}

PDGL_API void pdglProjectionMatrix(void)
{
	pdgl_drawprim_matrixmode=1;
}

PDGL_API void pdglLoadIdentity()
{
//	PDGL_DrawPrim_Init();
	Mat4F_Identity(pdgl_drawprim_matrix[pdgl_drawprim_matrixmode]);
//	glMatrixMode(GL_MODELVIEW_MATRIX);
//	glLoadIdentity();
}

PDGL_API void pdglPushMatrix(void)
{
	int i, j;

//	PDGL_DrawPrim_Init();

	if(pdgl_drawprim_matrixstackpos[pdgl_drawprim_matrixmode]<0)
	{
		Mat4F_Identity(pdgl_drawprim_matrix[pdgl_drawprim_matrixmode]);
		pdgl_drawprim_matrixstackpos[pdgl_drawprim_matrixmode]=0;
	}

	if(pdgl_drawprim_matrixstackpos[pdgl_drawprim_matrixmode]>=16)
	{
		*(int *)-1=-1;
	}

	i=pdgl_drawprim_matrixstackpos[pdgl_drawprim_matrixmode]++;
	for(j=0; j<16; j++)
		pdgl_drawprim_matrixstack[pdgl_drawprim_matrixmode][i][j]=
			pdgl_drawprim_matrix[pdgl_drawprim_matrixmode][j];

//	glMatrixMode(GL_MODELVIEW_MATRIX);
//	glPushMatrix();
}

PDGL_API void pdglPopMatrix(void)
{
	int i, j;

//	PDGL_DrawPrim_Init();

	i=--pdgl_drawprim_matrixstackpos[pdgl_drawprim_matrixmode];
	if(i<0)
	{
		Mat4F_Identity(pdgl_drawprim_matrix[pdgl_drawprim_matrixmode]);
		pdgl_drawprim_matrixstackpos[pdgl_drawprim_matrixmode]=0;
		return;
	}

	for(j=0; j<16; j++)
		pdgl_drawprim_matrix[pdgl_drawprim_matrixmode][j]=
			pdgl_drawprim_matrixstack[pdgl_drawprim_matrixmode][i][j];

//	glMatrixMode(GL_MODELVIEW_MATRIX);
//	glPopMatrix();
}

PDGL_API void pdglMultMatrixf(float *mat)
{
	float tmat[16];

	Mat4F_MatMult(pdgl_drawprim_matrix[pdgl_drawprim_matrixmode], mat, tmat);
	Mat4F_Copy(tmat, pdgl_drawprim_matrix[pdgl_drawprim_matrixmode]);

//	glMatrixMode(GL_MODELVIEW_MATRIX);
//	glMultMatrixf(mat);
}

PDGL_API void pdglTranslatef(float x, float y, float z)
{
	float tmat[16];

	Mat4F_Identity(tmat);
	tmat[12]=x; tmat[13]=y; tmat[14]=z;
	pdglMultMatrixf(tmat);
}

PDGL_API void pdglScalef(float x, float y, float z)
{
	float tmat[16];

	Mat4F_Identity(tmat);
	tmat[0]=x; tmat[5]=y; tmat[10]=z;
	pdglMultMatrixf(tmat);
}

PDGL_API void pdglRotatef(float th, float nx, float ny, float nz)
{
//	glRotatef(th, nx, ny, nz);
}

PDGL_API void pdglGetModelviewMatrix(float *v)
{
//	glGetFloatv(GL_MODELVIEW_MATRIX, v);
}

PDGL_API void pdglOrtho(
	float mx, float nx,
	float my, float ny,
	float mz, float nz)
{
//	glOrtho(mx, nx, my, ny, mz, nz);
}



PDGL_API void PDGL_Light0_ConstantAttenuation(float v)
{
//	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, v);
}

PDGL_API void PDGL_Light0_LinearAttenuation(float v)
{
//	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, v);
}

PDGL_API void PDGL_Light0_QuadraticAttenuation(float v)
{
//	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, v);
}

PDGL_API void PDGL_Enable_Light0(void)
{
//	glEnable(GL_LIGHT0);
}

PDGL_API void PDGL_ColorMaterial_Ambient(float *v)
{
//	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, v);
}

PDGL_API void PDGL_ColorMaterial_Diffuse(float *v)
{
//	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, v);
}

PDGL_API void PDGL_ColorMaterial_Specular(float *v)
{
//	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, v);
}

PDGL_API void PDGL_ColorMaterial_Emission(float *v)
{
//	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, v);
}

PDGL_API void PDGL_ColorMaterial_Shininess(float *v)
{
//	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, v);
}


PDGL_API void PDGL_Light0_Ambient(float *v)
{
//	glLightfv(GL_LIGHT0, GL_AMBIENT, v);
}

PDGL_API void PDGL_Light0_Diffuse(float *v)
{
//	glLightfv(GL_LIGHT0, GL_DIFFUSE, v);
}

PDGL_API void PDGL_Light0_Specular(float *v)
{
//	glLightfv(GL_LIGHT0, GL_SPECULAR, v);
}

PDGL_API void PDGL_Light0_Position(float *v)
{
//	glLightfv(GL_LIGHT0, GL_POSITION, v);
}

PDGL_API void PDGL_Light0_SpotDirection(float *v)
{
//	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, v);
}

PDGL_API void PDGL_Light0_SpotCutoff(float v)
{
//	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, v);
}

PDGL_API void PDGL_Light0_SpotExponent(float v)
{
//	glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, v);
}


PDGL_API void PDGL_DrawPrim_DrawArraysSimpleFlat(
	int prim, int base, int nxyz,
	int xyzsize, int xyztype, int xyzstep, void *xyz)
{
//	glEnableClientState(GL_VERTEX_ARRAY);
//	glVertexPointer(xyzsize, xyztype, xyzstep, xyz);
//	glDrawArrays(prim, base, nxyz);
//	glDisableClientState(GL_VERTEX_ARRAY);
}

PDGL_API void PDGL_DrawPrim_DrawArraysNormalFlat(
	int prim, int base, int nxyz,
	int xyzsize, int xyztype, int xyzstep, void *xyz, 
	int normsize, int normtype, int normstep, void *norm)
{
//	glEnableClientState(GL_VERTEX_ARRAY);
//	glEnableClientState(GL_NORMAL_ARRAY);
//	glVertexPointer(xyzsize, xyztype, xyzstep, xyz);
//	glNormalPointer(normtype, normstep, norm);
//	glDrawArrays(prim, base, nxyz);
//	glDisableClientState(GL_VERTEX_ARRAY);
//	glDisableClientState(GL_NORMAL_ARRAY);
}

PDGL_API void PDGL_DrawPrim_DrawArraysNormalFlatRGB(
	int prim, int base, int nxyz,
	int xyzsize, int xyztype, int xyzstep, void *xyz, 
	int normsize, int normtype, int normstep, void *norm,
	int rgbsize, int rgbtype, int rgbstep, void *rgb)
{
//	glEnableClientState(GL_VERTEX_ARRAY);
//	glEnableClientState(GL_NORMAL_ARRAY);
//	glEnableClientState(GL_COLOR_ARRAY);
//	glVertexPointer(xyzsize, xyztype, xyzstep, xyz);
//	glNormalPointer(normtype, normstep, norm);
//	glColorPointer(rgbsize, rgbtype, rgbstep, rgb);
//	glDrawArrays(prim, base, nxyz);
//	glDisableClientState(GL_VERTEX_ARRAY);
//	glDisableClientState(GL_NORMAL_ARRAY);
//	glDisableClientState(GL_COLOR_ARRAY);
}

PDGL_API void PDGL_DrawPrim_DrawArraysNormalTex(
	int prim, int base, int nxyz,
	int xyzsize, int xyztype, int xyzstep, void *xyz, 
	int stsize, int sttype, int ststep, void *st, 
	int normsize, int normtype, int normstep, void *norm)
{
//	glEnableClientState(GL_VERTEX_ARRAY);
//	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
//	glEnableClientState(GL_NORMAL_ARRAY);
//	glVertexPointer(xyzsize, xyztype, xyzstep, xyz);
//	glTexCoordPointer(stsize, sttype, ststep, st);
//	glNormalPointer(normtype, normstep, norm);
//	glDrawArrays(prim, base, nxyz);
//	glDisableClientState(GL_VERTEX_ARRAY);
//	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
//	glDisableClientState(GL_NORMAL_ARRAY);
}

PDGL_API void PDGL_DrawPrim_DrawArraysNormalTexRGB(
	int prim, int base, int nxyz,
	int xyzsize, int xyztype, int xyzstep, void *xyz, 
	int stsize, int sttype, int ststep, void *st, 
	int normsize, int normtype, int normstep, void *norm,
	int rgbsize, int rgbtype, int rgbstep, void *rgb)
{
//	glEnableClientState(GL_VERTEX_ARRAY);
//	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
//	glEnableClientState(GL_NORMAL_ARRAY);
//	glEnableClientState(GL_COLOR_ARRAY);
//	glVertexPointer(xyzsize, xyztype, xyzstep, xyz);
//	glTexCoordPointer(stsize, sttype, ststep, st);
//	glNormalPointer(normtype, normstep, norm);
//	glColorPointer(rgbsize, rgbtype, rgbstep, rgb);
//	glDrawArrays(prim, base, nxyz);
//	glDisableClientState(GL_VERTEX_ARRAY);
//	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
//	glDisableClientState(GL_NORMAL_ARRAY);
//	glDisableClientState(GL_COLOR_ARRAY);
}

#endif
