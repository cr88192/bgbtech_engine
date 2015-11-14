#include <pdgl.h>

#ifndef GLES2

PDGL_API void pdglBegin(int prim)
	{ glBegin(prim); }
PDGL_API void pdglEnd(void)
	{ glEnd(); }
PDGL_API void pdglVertex2fv(float *v)
	{ glVertex2fv(v); }
PDGL_API void pdglVertex3fv(float *v)
	{ glVertex3fv(v); }

PDGL_API void pdglVertex3dv(double *v)
	{ glVertex3dv(v); }

PDGL_API void pdglTexCoord2fv(float *v)
	{ glTexCoord2fv(v); }

PDGL_API void pdglNormal3fv(float *v)
	{ glNormal3fv(v); }

PDGL_API void pdglNormal3sbv(sbyte *v)
{
	float ntmp[3];

	ntmp[0]=v[0]*(1.0/127.0);
	ntmp[1]=v[1]*(1.0/127.0);
	ntmp[2]=v[2]*(1.0/127.0);
	glNormal3fv(ntmp);
}

PDGL_API void pdglNormal3bv(sbyte *v)
{
	float ntmp[3];

	ntmp[0]=v[0]*(1.0/127.0);
	ntmp[1]=v[1]*(1.0/127.0);
	ntmp[2]=v[2]*(1.0/127.0);
	glNormal3fv(ntmp);
}

PDGL_API void pdglColor3fv(float *v)
{
	glColor3fv(v);
}

PDGL_API void pdglColor4fv(float *v)
{
	glColor4fv(v);
}

PDGL_API void pdglColor4ubv(byte *v)
{
	float clr[4];
	clr[0]=v[0]*(1.0/255.0);
	clr[1]=v[1]*(1.0/255.0);
	clr[2]=v[2]*(1.0/255.0);
	clr[3]=v[3]*(1.0/255.0);
	glColor4fv(clr);
}

PDGL_API void pdglColor3f(float r, float g, float b)
{
	glColor3f(r, g, b);
}

PDGL_API void pdglColor4f(float r, float g, float b, float a)
{
	glColor4f(r, g, b, a);
}

PDGL_API void pdglColor4ub(int r, int g, int b, int a)
{
	glColor4f(r/255.0, g/255.0, b/255.0, a/255.0);
}

PDGL_API void pdglTexCoord2f(float s, float t)
{
	glTexCoord2f(s, t);
}

PDGL_API void pdglVertex2f(float x, float y)
{
	glVertex2f(x, y);
}

PDGL_API void pdglVertex3f(float x, float y, float z)
{
	glVertex3f(x, y, z);
}

PDGL_API void pdglNormal3f(float x, float y, float z)
{
	glNormal3f(x, y, z);
}

PDGL_API void pdglColorMask(bool r, bool g, bool b, bool a)
{
	glColorMask(r, g, b, a);
}

PDGL_API void pdglEnableTexture2D(void)
{
	glEnable(GL_TEXTURE_2D);
}

PDGL_API void pdglDisableTexture2D(void)
{
	glDisable(GL_TEXTURE_2D);
}

PDGL_API void pdglEnableLighting(void)
{
	glEnable(GL_LIGHTING);
}

PDGL_API void pdglDisableLighting(void)
{
	glDisable(GL_LIGHTING);
}


PDGL_API void pdglModelviewMatrix(void)
{
	glMatrixMode(GL_MODELVIEW);
}

PDGL_API void pdglProjectionMatrix(void)
{
	glMatrixMode(GL_PROJECTION);
}

PDGL_API void pdglLoadIdentity(void)
{
	glLoadIdentity();
}

PDGL_API void pdglPushMatrix(void)
{
	glPushMatrix();
}

PDGL_API void pdglPopMatrix(void)
{
	glPopMatrix();
}

PDGL_API void pdglMultMatrixf(float *mat)
{
	glMultMatrixf(mat);
}

PDGL_API void pdglTranslatef(float x, float y, float z)
{
	glTranslatef(x, y, z);
}

PDGL_API void pdglScalef(float x, float y, float z)
{
	glScalef(x, y, z);
}

PDGL_API void pdglRotatef(float th, float nx, float ny, float nz)
{
	glRotatef(th, nx, ny, nz);
}

PDGL_API void pdglGetModelviewMatrix(float *v)
{
	glGetFloatv(GL_MODELVIEW_MATRIX, v);
}

PDGL_API void pdglOrtho(
	float mx, float nx,
	float my, float ny,
	float mz, float nz)
{
	glOrtho(mx, nx, my, ny, mz, nz);
}

PDGL_API void PDGL_Light0_ConstantAttenuation(float v)
{
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, v);
}

PDGL_API void PDGL_Light0_LinearAttenuation(float v)
{
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, v);
}

PDGL_API void PDGL_Light0_QuadraticAttenuation(float v)
{
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, v);
}

PDGL_API void PDGL_Enable_Light0(void)
{
	glEnable(GL_LIGHT0);
}

PDGL_API void PDGL_ColorMaterial_Ambient(float *v)
{
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, v);
}

PDGL_API void PDGL_ColorMaterial_Diffuse(float *v)
{
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, v);
}

PDGL_API void PDGL_ColorMaterial_Specular(float *v)
{
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, v);
}

PDGL_API void PDGL_ColorMaterial_Emission(float *v)
{
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, v);
}

PDGL_API void PDGL_ColorMaterial_Shininess(float *v)
{
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, v);
}


PDGL_API void PDGL_Light0_Ambient(float *v)
{
	glLightfv(GL_LIGHT0, GL_AMBIENT, v);
}

PDGL_API void PDGL_Light0_Diffuse(float *v)
{
	glLightfv(GL_LIGHT0, GL_DIFFUSE, v);
}

PDGL_API void PDGL_Light0_Specular(float *v)
{
	glLightfv(GL_LIGHT0, GL_SPECULAR, v);
}

PDGL_API void PDGL_Light0_Position(float *v)
{
	glLightfv(GL_LIGHT0, GL_POSITION, v);
}

PDGL_API void PDGL_Light0_SpotDirection(float *v)
{
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, v);
}

PDGL_API void PDGL_Light0_SpotCutoff(float v)
{
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, v);
}

PDGL_API void PDGL_Light0_SpotExponent(float v)
{
	glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, v);
}


PDGL_API void PDGL_DrawPrim_DrawArraysSimpleFlat(
	int prim, int base, int nxyz,
	int xyzsize, int xyztype, int xyzstep, void *xyz)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(xyzsize, xyztype, xyzstep, xyz);
	glDrawArrays(prim, base, nxyz);
	glDisableClientState(GL_VERTEX_ARRAY);
}

PDGL_API void PDGL_DrawPrim_DrawArraysNormalFlat(
	int prim, int base, int nxyz,
	int xyzsize, int xyztype, int xyzstep, void *xyz, 
	int normsize, int normtype, int normstep, void *norm)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glVertexPointer(xyzsize, xyztype, xyzstep, xyz);
	glNormalPointer(normtype, normstep, norm);
	glDrawArrays(prim, base, nxyz);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
}

PDGL_API void PDGL_DrawPrim_DrawArraysNormalFlatRGB(
	int prim, int base, int nxyz,
	int xyzsize, int xyztype, int xyzstep, void *xyz, 
	int normsize, int normtype, int normstep, void *norm,
	int rgbsize, int rgbtype, int rgbstep, void *rgb)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(xyzsize, xyztype, xyzstep, xyz);
	glNormalPointer(normtype, normstep, norm);
	glColorPointer(rgbsize, rgbtype, rgbstep, rgb);
	glDrawArrays(prim, base, nxyz);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
}

PDGL_API void PDGL_DrawPrim_DrawArraysNormalTex(
	int prim, int base, int nxyz,
	int xyzsize, int xyztype, int xyzstep, void *xyz, 
	int stsize, int sttype, int ststep, void *st, 
	int normsize, int normtype, int normstep, void *norm)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glVertexPointer(xyzsize, xyztype, xyzstep, xyz);
	glTexCoordPointer(stsize, sttype, ststep, st);
	glNormalPointer(normtype, normstep, norm);
	glDrawArrays(prim, base, nxyz);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
}

PDGL_API void PDGL_DrawPrim_DrawArraysNormalTexRGB(
	int prim, int base, int nxyz,
	int xyzsize, int xyztype, int xyzstep, void *xyz, 
	int stsize, int sttype, int ststep, void *st, 
	int normsize, int normtype, int normstep, void *norm,
	int rgbsize, int rgbtype, int rgbstep, void *rgb)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(xyzsize, xyztype, xyzstep, xyz);
	glTexCoordPointer(stsize, sttype, ststep, st);
	glNormalPointer(normtype, normstep, norm);
	glColorPointer(rgbsize, rgbtype, rgbstep, rgb);
	glDrawArrays(prim, base, nxyz);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
}

PDGL_API void PDGL_DrawPrim_DrawArraysTexRGB(
	int prim, int base, int nxyz,
	int xyzsize, int xyztype, int xyzstep, void *xyz, 
	int stsize, int sttype, int ststep, void *st, 
	int rgbsize, int rgbtype, int rgbstep, void *rgb)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(xyzsize, xyztype, xyzstep, xyz);
	glTexCoordPointer(stsize, sttype, ststep, st);
	glColorPointer(rgbsize, rgbtype, rgbstep, rgb);
	glDrawArrays(prim, base, nxyz);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
}

void pdgl_worker_begin(BGBGC_WorkerInfo *info)
{
	GfxDrv_SetupThreadShareLists();
}

void pdgl_worker_end(BGBGC_WorkerInfo *info)
{
	GfxDrv_TeardownThreadShareLists();
}

BGBGC_WorkerInfo *pdgl_worker_spawn(void *data)
{
	BGBGC_WorkerInfo *inf;
	inf=gcalloc(sizeof(BGBGC_WorkerInfo));
	inf->begin=pdgl_worker_begin;
	inf->end=pdgl_worker_end;
	return(inf);
}

PDGL_API BGBGC_Job *pdglAddJob(void *(*fcn)(void *data), void *data)
{
	static int workid=0;
	BGBGC_Job *job;
	
	*(int *)-1=-1;
	
	if(!workid)
	{
		workid=BGBGC_RegisterSpawnWorker(pdgl_worker_spawn);
	}
	
	job=thAddJobId(fcn, data, workid);
	return(job);
}


#endif
