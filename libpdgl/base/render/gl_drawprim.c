#include <pdgl.h>

#ifndef GLES2

#if !defined(APIENTRY)
#if defined(__WIN32__)
#define APIENTRY __stdcall
#else
#define APIENTRY
#endif
#endif

/* Miscellaneous */
  void (APIENTRY *pglClearIndex)(GLfloat c);
  void (APIENTRY *pglClearColor)(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
  void (APIENTRY *pglClear)(GLbitfield mask);
  void (APIENTRY *pglIndexMask)(GLuint mask);
  void (APIENTRY *pglColorMask)(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
  void (APIENTRY *pglAlphaFunc)(GLenum func, GLclampf ref);
  void (APIENTRY *pglBlendFunc)(GLenum sfactor, GLenum dfactor);
  void (APIENTRY *pglLogicOp)(GLenum opcode);
  void (APIENTRY *pglCullFace)(GLenum mode);
  void (APIENTRY *pglFrontFace)(GLenum mode);
  void (APIENTRY *pglPointSize)(GLfloat size);
  void (APIENTRY *pglLineWidth)(GLfloat width);
  void (APIENTRY *pglPolygonMode)(GLenum face, GLenum mode);
  void (APIENTRY *pglPolygonOffset)(GLfloat factor, GLfloat units);
  void (APIENTRY *pglScissor)(GLint x, GLint y, GLsizei width, GLsizei height);
  void (APIENTRY *pglClipPlane)(GLenum plane, const GLdouble *equation);
  void (APIENTRY *pglGetClipPlane)(GLenum plane, GLdouble *equation);
  void (APIENTRY *pglDrawBuffer)(GLenum mode);
  void (APIENTRY *pglReadBuffer)(GLenum mode);
  GLboolean (APIENTRY *pglIsEnabled)(GLenum cap);
  void (APIENTRY *pglEnableClientState)(GLenum cap);
  void (APIENTRY *pglDisableClientState)(GLenum cap);

  void (APIENTRY *pglGetFloatv)(GLenum pname, GLfloat *params);
  void (APIENTRY *pglGetIntegerv)(GLenum pname, GLint *params);

  void (APIENTRY *pglEnable)(GLenum cap);
  void (APIENTRY *pglDisable)(GLenum cap);

  void (APIENTRY *pglPushAttrib)(GLbitfield mask);
  void (APIENTRY *pglPopAttrib)(void);
  void (APIENTRY *pglPushClientAttrib)(GLbitfield mask);
  void (APIENTRY *pglPopClientAttrib)(void);

  GLint (APIENTRY *pglRenderMode)(GLenum mode);
  GLenum (APIENTRY *pglGetError)(void);
  const GLubyte* (APIENTRY *pglGetString)(GLenum name);

  void (APIENTRY *pglFinish)(void);
  void (APIENTRY *pglFlush)(void);
  void (APIENTRY *pglHint)(GLenum target, GLenum mode);


  void (APIENTRY *pglDeleteTextures)(GLsizei n,
		const GLuint *textures);
  void (APIENTRY *pglBindTexture)(GLenum target,
		GLuint texture);
  void (APIENTRY *pglColorMask)(GLboolean red,
		GLboolean green, GLboolean blue, GLboolean alpha);

  void (APIENTRY *pglReadPixels)( GLint x, GLint y,
		GLsizei width, GLsizei height, GLenum format,
		GLenum type, GLvoid *pixels );
  void (APIENTRY *pglDrawPixels)( GLsizei width, GLsizei height,
		GLenum format, GLenum type, const GLvoid *pixels );


/* Drawing Functions */
  void (APIENTRY *pglBegin)(GLenum mode);
  void (APIENTRY *pglEnd)(void);

  void (APIENTRY *pglVertex2f)(GLfloat x, GLfloat y);
  void (APIENTRY *pglVertex3f)(GLfloat x, GLfloat y, GLfloat z);
  void (APIENTRY *pglVertex4f)(GLfloat x, GLfloat y, GLfloat z, GLfloat w);

  void (APIENTRY *pglVertex2fv)(const GLfloat *v);
  void (APIENTRY *pglVertex3dv)(const GLdouble *v);
  void (APIENTRY *pglVertex3fv)(const GLfloat *v);

  void (APIENTRY *pglVertex4fv)(const GLfloat *v);

  void (APIENTRY *pglNormal3f)(GLfloat nx, GLfloat ny, GLfloat nz);

  void (APIENTRY *pglNormal3fv)(const GLfloat *v);

  void (APIENTRY *pglColor3f)(GLfloat red, GLfloat green, GLfloat blue);
  void (APIENTRY *pglColor4f)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
  void (APIENTRY *pglColor3fv)(const GLfloat *v);
  void (APIENTRY *pglColor4fv)(const GLfloat *v);
  void (APIENTRY *pglTexCoord1f)(GLfloat s);
  void (APIENTRY *pglTexCoord2f)(GLfloat s, GLfloat t);
  void (APIENTRY *pglTexCoord3f)(GLfloat s, GLfloat t, GLfloat r);
  void (APIENTRY *pglTexCoord4f)(GLfloat s, GLfloat t, GLfloat r, GLfloat q);
  void (APIENTRY *pglTexCoord1fv)(const GLfloat *v);
  void (APIENTRY *pglTexCoord2fv)(const GLfloat *v);
  void (APIENTRY *pglTexCoord3fv)(const GLfloat *v);
  void (APIENTRY *pglTexCoord4fv)(const GLfloat *v);

  void (APIENTRY *pglColor3fv)(const GLfloat *v);
  void (APIENTRY *pglColor4fv)(const GLfloat *v);
  void (APIENTRY *pglTexCoord2f)(GLfloat s, GLfloat t);

/* Depth Buffer */
  void (APIENTRY *pglClearDepth)(GLclampd depth);
  void (APIENTRY *pglDepthFunc)(GLenum func);
  void (APIENTRY *pglDepthMask)(GLboolean flag);
  void (APIENTRY *pglDepthRange)(GLclampd near_val, GLclampd far_val);

/* Transformation */
  void (APIENTRY *pglMatrixMode)(GLenum mode);
  void (APIENTRY *pglOrtho)(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val);
  void (APIENTRY *pglFrustum)(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val);
  void (APIENTRY *pglViewport)(GLint x, GLint y, GLsizei width, GLsizei height);
  void (APIENTRY *pglPushMatrix)(void);
  void (APIENTRY *pglPopMatrix)(void);
  void (APIENTRY *pglLoadIdentity)(void);
  void (APIENTRY *pglLoadMatrixf)(const GLfloat *m);
  void (APIENTRY *pglMultMatrixf)(const GLfloat *m);
  void (APIENTRY *pglRotatef)(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
  void (APIENTRY *pglScalef)(GLfloat x, GLfloat y, GLfloat z);
  void (APIENTRY *pglTranslatef)(GLfloat x, GLfloat y, GLfloat z);

/* Lighting */
  void (APIENTRY *pglShadeModel)(GLenum mode);
  void (APIENTRY *pglLightf)(GLenum light, GLenum pname, GLfloat param);
  void (APIENTRY *pglLighti)(GLenum light, GLenum pname, GLint param);
  void (APIENTRY *pglLightfv)(GLenum light, GLenum pname, const GLfloat *params);
  void (APIENTRY *pglLightiv)(GLenum light, GLenum pname, const GLint *params);
  void (APIENTRY *pglGetLightfv)(GLenum light, GLenum pname, GLfloat *params);
  void (APIENTRY *pglGetLightiv)(GLenum light, GLenum pname, GLint *params);
  void (APIENTRY *pglLightModelf)(GLenum pname, GLfloat param);
  void (APIENTRY *pglLightModeli)(GLenum pname, GLint param);
  void (APIENTRY *pglLightModelfv)(GLenum pname, const GLfloat *params);
  void (APIENTRY *pglLightModeliv)(GLenum pname, const GLint *params);
  void (APIENTRY *pglMaterialf)(GLenum face, GLenum pname, GLfloat param);
  void (APIENTRY *pglMateriali)(GLenum face, GLenum pname, GLint param);
  void (APIENTRY *pglMaterialfv)(GLenum face,
		GLenum pname, const GLfloat *params);
  void (APIENTRY *pglMaterialiv)(GLenum face,
		GLenum pname, const GLint *params);
  void (APIENTRY *pglGetMaterialfv)(GLenum face,
		GLenum pname, GLfloat *params);
  void (APIENTRY *pglGetMaterialiv)(GLenum face,
		GLenum pname, GLint *params);
  void (APIENTRY *pglColorMaterial)(GLenum face, GLenum mode);

/* Stenciling */
  void (APIENTRY *pglStencilFunc)(GLenum func,
		GLint ref, GLuint mask);
  void (APIENTRY *pglStencilMask)(GLuint mask);
  void (APIENTRY *pglStencilOp)(GLenum fail,
		GLenum zfail, GLenum zpass);
  void (APIENTRY *pglClearStencil)(GLint s);

  void (APIENTRY *pglTexEnvf)(GLenum target,
		GLenum pname, GLfloat param);
  void (APIENTRY *pglTexEnvi)(GLenum target,
		GLenum pname, GLint param);
  void (APIENTRY *pglTexEnvfv)(GLenum target,
		GLenum pname, const GLfloat *params);
  void (APIENTRY *pglTexEnviv)(GLenum target,
		GLenum pname, const GLint *params);
  void (APIENTRY *pglTexParameterf)(GLenum target,
		GLenum pname, GLfloat param);
  void (APIENTRY *pglTexParameteri)(GLenum target,
		GLenum pname, GLint param);
  void (APIENTRY *pglTexParameterfv)(GLenum target,
		GLenum pname, const GLfloat *params);
  void (APIENTRY *pglTexParameteriv)(GLenum target,
		GLenum pname, const GLint *params);

  void (APIENTRY *pglTexImage1D)(GLenum target,
		GLint level, GLint internalFormat, GLsizei width,
		GLint border, GLenum format, GLenum type,
		const GLvoid *pixels);
  void (APIENTRY *pglTexImage2D)(GLenum target,
		GLint level, GLint internalFormat, GLsizei width,
		GLsizei height, GLint border, GLenum format,
		GLenum type, const GLvoid *pixels);
  void (APIENTRY *pglGetTexImage)(GLenum target,
		GLint level, GLenum format, GLenum type, GLvoid *pixels);

  void (APIENTRY *pglCopyTexImage2D)(GLenum target, GLint level,
		GLenum internalformat, GLint x, GLint y, GLsizei width,
		GLsizei height, GLint border);

  void (APIENTRY *pglVertexPointer)(GLint size, GLenum type,
		GLsizei stride, const GLvoid *ptr);
  void (APIENTRY *pglNormalPointer)(GLenum type,
		GLsizei stride, const GLvoid *ptr);
  void (APIENTRY *pglColorPointer)(GLint size,
		GLenum type, GLsizei stride, const GLvoid *ptr);
  void (APIENTRY *pglIndexPointer)(GLenum type,
		GLsizei stride, const GLvoid *ptr);
  void (APIENTRY *pglTexCoordPointer)(GLint size,
		GLenum type, GLsizei stride, const GLvoid *ptr);
  void (APIENTRY *pglEdgeFlagPointer)(
		GLsizei stride, const GLvoid *ptr);
  void (APIENTRY *pglGetPointerv)(
		GLenum pname, GLvoid **params);
  void (APIENTRY *pglArrayElement)(GLint i);
  void (APIENTRY *pglDrawArrays)(GLenum mode,
		GLint first, GLsizei count);
  void (APIENTRY *pglDrawElements)(GLenum mode,
		GLsizei count, GLenum type, const GLvoid *indices);
  void (APIENTRY *pglInterleavedArrays)(GLenum format,
		GLsizei stride, const GLvoid *pointer);


PDGL_API void pdglPushAttrib(GLbitfield mask)
{
	if(pglPushAttrib)
		{ pglPushAttrib(mask); return; }
	pglPushAttrib=pdglGetProcAddress("glPushAttrib");
	pglPushAttrib(mask);
}

PDGL_API void pdglPopAttrib(void)
{
	if(pglPopAttrib)
		{ pglPopAttrib(); return; }
	pglPopAttrib=pdglGetProcAddress("glPopAttrib");
	pglPopAttrib();
}

PDGL_API void pdglPushClientAttrib(GLbitfield mask)
{
	if(pglPushClientAttrib)
		{ pglPushClientAttrib(mask); return; }
	pglPushClientAttrib=pdglGetProcAddress("glPushClientAttrib");
	pglPushClientAttrib(mask);
}

PDGL_API void pdglPopClientAttrib(void)
{
	if(pglPopClientAttrib)
		{ pglPopClientAttrib(); return; }
	pglPopClientAttrib=pdglGetProcAddress("glPopClientAttrib");
	pglPopClientAttrib();
}

PDGL_API void pdglStencilFunc(GLenum func,
		GLint ref, GLuint mask)
{
	if(pglStencilFunc)
		{ pglStencilFunc(func, ref, mask); return; }
	pglStencilFunc=pdglGetProcAddress("glStencilFunc");
	pglStencilFunc(func, ref, mask);
}

PDGL_API void pdglStencilMask(GLuint mask)
{
	if(pglStencilMask)
		{ pglStencilMask(mask); return; }
	pglStencilMask=pdglGetProcAddress("glStencilMask");
	pglStencilMask(mask);
}

PDGL_API void pdglStencilOp(GLenum fail,
		GLenum zfail, GLenum zpass)
{
	if(pglStencilOp)
		{ pglStencilOp(fail, zfail, zpass); return; }
	pglStencilOp=pdglGetProcAddress("glStencilOp");
	pglStencilOp(fail, zfail, zpass);
}

PDGL_API void pdglClearStencil(GLint s)
{
	if(pglClearStencil)
		{ pglClearStencil(s); return; }
	pglClearStencil=pdglGetProcAddress("glClearStencil");
	pglClearStencil(s);
}


PDGL_API GLenum pdglGetError(void)
{
	if(pglGetError)
		{ return(pglGetError()); }
	pglGetError=pdglGetProcAddress("glGetError");
	return(pglGetError());
}

PDGL_API const GLubyte *pdglGetString(GLenum name)
{
	if(pglGetString)
		{ return(pglGetString(name)); }
	pglGetString=pdglGetProcAddress("glGetString");
	return(pglGetString(name));
}

PDGL_API void pdglReadPixels( GLint x, GLint y,
	GLsizei width, GLsizei height, GLenum format,
	GLenum type, GLvoid *pixels)
{
	if(pglReadPixels)
	{
		pglReadPixels(x, y, width, height, format, type, pixels);
		return;
	}
	pglReadPixels=pdglGetProcAddress("glReadPixels");
	pglReadPixels(x, y, width, height, format, type, pixels);
}

PDGL_API void pdglDrawPixels( GLsizei width, GLsizei height,
	GLenum format, GLenum type, const GLvoid *pixels)
{
	if(pglDrawPixels)
		{ pglDrawPixels(width, height, format, type, pixels); return; }
	pglDrawPixels=pdglGetProcAddress("glDrawPixels");
	pglDrawPixels(width, height, format, type, pixels);
}


PDGL_API void pdglEnableClientState( GLenum cap )
{
	if(pglEnableClientState)
		{ pglEnableClientState(cap); return; }
	pglEnableClientState=pdglGetProcAddress("glEnableClientState");
	pglEnableClientState(cap);
}

PDGL_API void pdglDisableClientState( GLenum cap )
{
	if(pglDisableClientState)
		{ pglDisableClientState(cap); return; }
	pglDisableClientState=pdglGetProcAddress("glDisableClientState");
	pglDisableClientState(cap);
}


PDGL_API void pdglVertexPointer( GLint size,
	GLenum type, GLsizei stride, const GLvoid *ptr )
{
	if(pglVertexPointer)
		{ pglVertexPointer(size, type, stride, ptr); return; }
	pglVertexPointer=pdglGetProcAddress("glVertexPointer");
	pglVertexPointer(size, type, stride, ptr);
}

PDGL_API void pdglNormalPointer( GLenum type,
	GLsizei stride, const GLvoid *ptr )
{
	if(pglNormalPointer)
		{ pglNormalPointer(type, stride, ptr); return; }
	pglNormalPointer=pdglGetProcAddress("glNormalPointer");
	pglNormalPointer(type, stride, ptr);
}

PDGL_API void pdglColorPointer( GLint size,
	GLenum type, GLsizei stride, const GLvoid *ptr )
{
	if(pglColorPointer)
		{ pglColorPointer(size, type, stride, ptr); return; }
	pglColorPointer=pdglGetProcAddress("glColorPointer");
	pglColorPointer(size, type, stride, ptr);
}

PDGL_API void pdglIndexPointer( GLenum type,
	GLsizei stride, const GLvoid *ptr )
{
	if(pglIndexPointer)
		{ pglIndexPointer(type, stride, ptr); return; }
	pglIndexPointer=pdglGetProcAddress("glIndexPointer");
	pglIndexPointer(type, stride, ptr);
}

PDGL_API void pdglTexCoordPointer( GLint size,
	GLenum type, GLsizei stride, const GLvoid *ptr )
{
	if(pglTexCoordPointer)
		{ pglTexCoordPointer(size, type, stride, ptr); return; }
	pglTexCoordPointer=pdglGetProcAddress("glTexCoordPointer");
	pglTexCoordPointer(size, type, stride, ptr);
}

PDGL_API void pdglEdgeFlagPointer( GLsizei stride, const GLvoid *ptr )
{
	if(pglEdgeFlagPointer)
		{ pglEdgeFlagPointer(stride, ptr); return; }
	pglEdgeFlagPointer=pdglGetProcAddress("glEdgeFlagPointer");
	pglEdgeFlagPointer(stride, ptr);
}

PDGL_API void pdglGetPointerv( GLenum pname, GLvoid **params )
{
	if(pglGetPointerv)
		{ pglGetPointerv(pname, params); return; }
	pglGetPointerv=pdglGetProcAddress("glGetPointerv");
	pglGetPointerv(pname, params);
}

PDGL_API void pdglArrayElement( GLint i )
{
	if(pglArrayElement)
		{ pglArrayElement(i); return; }
	pglArrayElement=pdglGetProcAddress("glArrayElement");
	pglArrayElement(i);
}

PDGL_API void pdglDrawArrays( GLenum mode, GLint first, GLsizei count )
{
	if(pglDrawArrays)
		{ pglDrawArrays(mode, first, count); return; }
	pglDrawArrays=pdglGetProcAddress("glDrawArrays");
	pglDrawArrays(mode, first, count);
}

PDGL_API void pdglDrawElements( GLenum mode,
	GLsizei count, GLenum type, const GLvoid *indices )
{
	if(pglDrawElements)
		{ pglDrawElements(mode, count, type, indices); return; }
	pglDrawElements=pdglGetProcAddress("glDrawElements");
	pglDrawElements(mode, count, type, indices);
}

PDGL_API void pdglInterleavedArrays( GLenum format,
	GLsizei stride, const GLvoid *pointer )
{
	if(pglInterleavedArrays)
		{ pglInterleavedArrays(format, stride, pointer); return; }
	pglInterleavedArrays=pdglGetProcAddress("glInterleavedArrays");
	pglInterleavedArrays(format, stride, pointer);
}


PDGL_API void pdglBegin(int prim)
{
	if(pglBegin)
		{ pglBegin(prim); return; }
	pglBegin=pdglGetProcAddress("glBegin");
	pglBegin(prim);
}

PDGL_API void pdglEnd(void)
{
	if(pglEnd)
		{ pglEnd(); return; }
	pglEnd=pdglGetProcAddress("glEnd");
	pglEnd();
}


PDGL_API void pdglClear(int mode)
{
	if(pglClear)
		{ pglClear(mode); return; }
	pglClear=pdglGetProcAddress("glClear");
	pglClear(mode);
}

PDGL_API void pdglCullFace(int mode)
{
	if(pglCullFace)
		{ pglCullFace(mode); return; }
	pglCullFace=pdglGetProcAddress("glCullFace");
	pglCullFace(mode);
}

PDGL_API void pdglFrontFace(int mode)
{
	if(pglFrontFace)
		{ pglFrontFace(mode); return; }
	pglFrontFace=pdglGetProcAddress("glFrontFace");
	pglFrontFace(mode);
}

PDGL_API void pdglClearColor(float r, float g, float b, float a)
{
	if(pglClearColor)
		{ pglClearColor(r, g, b, a); return; }
	pglClearColor=pdglGetProcAddress("glClearColor");
	pglClearColor(r, g, b, a);
}

PDGL_API void pdglAlphaFunc(int sfac, int dfac)
{
	if(pglAlphaFunc)
		{ pglAlphaFunc(sfac, dfac); return; }
	pglAlphaFunc=pdglGetProcAddress("glAlphaFunc");
	pglAlphaFunc(sfac, dfac);
}

PDGL_API void pdglBlendFunc(int sfac, int dfac)
{
	if(pglBlendFunc)
		{ pglBlendFunc(sfac, dfac); return; }
	pglBlendFunc=pdglGetProcAddress("glBlendFunc");
	pglBlendFunc(sfac, dfac);
}

PDGL_API void pdglPolygonMode(int face, int mode)
{
	if(pglPolygonMode)
		{ pglPolygonMode(face, mode); return; }
	pglPolygonMode=pdglGetProcAddress("glPolygonMode");
	pglPolygonMode(face, mode);
}

PDGL_API void pdglHint(int target, int mode)
{
	if(pglHint)
		{ pglHint(target, mode); return; }
	pglHint=pdglGetProcAddress("glHint");
	pglHint(target, mode);
}


PDGL_API void pdglDeleteTextures(GLsizei n, const GLuint *textures)
{
	if(pglDeleteTextures)
		{ pglDeleteTextures(n, textures); return; }
	pglDeleteTextures=pdglGetProcAddress("glDeleteTextures");
	pglDeleteTextures(n, textures);
}

PDGL_API void pdglBindTexture(int target, int texture)
{
	if(pglBindTexture)
		{ pglBindTexture(target, texture); return; }
	pglBindTexture=pdglGetProcAddress("glBindTexture");
	pglBindTexture(target, texture);
}

PDGL_API void pdglDisable(int val)
{
	if(pglDisable)
		{ pglDisable(val); return; }
	pglDisable=pdglGetProcAddress("glDisable");
	pglDisable(val);
}

PDGL_API void pdglEnable(int val)
{
	if(pglEnable)
		{ pglEnable(val); return; }
	pglEnable=pdglGetProcAddress("glEnable");
	pglEnable(val);
}

PDGL_API void pdglFinish(void)
{
	if(pglFinish)
		{ pglFinish(); return; }
	pglFinish=pdglGetProcAddress("glFinish");
	pglFinish();
}

PDGL_API void pdglFlush(void)
{
	if(pglFlush)
		{ pglFlush(); return; }
	pglFlush=pdglGetProcAddress("glFlush");
	pglFlush();
}

PDGL_API void pdglColor3f(float r, float g, float b)
{
	if(pglColor3f)
		{ pglColor3f(r, g, b); return; }
	pglColor3f=pdglGetProcAddress("glColor3f");
	pglColor3f(r, g, b);
}

PDGL_API void pdglColor4f(float r, float g, float b, float a)
{
	if(pglColor4f)
		{ pglColor4f(r, g, b, a); return; }
	pglColor4f=pdglGetProcAddress("glColor4f");
	pglColor4f(r, g, b, a);
}

PDGL_API void pdglColor3fv(float *v)
{
	if(pglColor3fv)
		{ pglColor3fv(v); return; }
	pglColor3fv=pdglGetProcAddress("glColor3fv");
	pglColor3fv(v);
}

PDGL_API void pdglColor4fv(float *v)
{
	if(pglColor4fv)
		{ pglColor4fv(v); return; }
	pglColor4fv=pdglGetProcAddress("glColor4fv");
	pglColor4fv(v);
//	pdglColor4fv(v);
}

PDGL_API void pdglTexCoord2f(float s, float t)
{
	if(pglTexCoord2f)
		{ pglTexCoord2f(s, t); return; }
	pglTexCoord2f=pdglGetProcAddress("glTexCoord2f");
	pglTexCoord2f(s, t);
}

PDGL_API void pdglTexCoord2fv(float *v)
{
	if(pglTexCoord2fv)
		{ pglTexCoord2fv(v); return; }
	pglTexCoord2fv=pdglGetProcAddress("glTexCoord2fv");
	pglTexCoord2fv(v);
}

PDGL_API void pdglTexCoord3fv(float *v)
{
	if(pglTexCoord3fv)
		{ pglTexCoord3fv(v); return; }
	pglTexCoord3fv=pdglGetProcAddress("glTexCoord3fv");
	pglTexCoord3fv(v);
}

PDGL_API void pdglVertex2f(float x, float y)
{
	if(pglVertex2f)
		{ pglVertex2f(x, y); return; }
	pglVertex2f=pdglGetProcAddress("glVertex2f");
	pglVertex2f(x, y);
}

PDGL_API void pdglVertex3f(float x, float y, float z)
{
	if(pglVertex3f)
		{ pglVertex3f(x, y, z); return; }
	pglVertex3f=pdglGetProcAddress("glVertex3f");
	pglVertex3f(x, y, z);
}

PDGL_API void pdglVertex2fv(float *v)
{
	if(pglVertex2fv)
		{ pglVertex2fv(v); return; }
	pglVertex2fv=pdglGetProcAddress("glVertex2fv");
	pglVertex2fv(v);
}

PDGL_API void pdglVertex3fv(float *v)
{
	if(pglVertex3fv)
		{ pglVertex3fv(v); return; }
	pglVertex3fv=pdglGetProcAddress("glVertex3fv");
	pglVertex3fv(v);
}

PDGL_API void pdglVertex3dv(double *v)
{
	if(pglVertex3dv)
		{ pglVertex3dv(v); return; }
	pglVertex3dv=pdglGetProcAddress("glVertex3dv");
	pglVertex3dv(v);
}

PDGL_API void pdglNormal3f(float x, float y, float z)
{
	if(pglNormal3f)
		{ pglNormal3f(x, y, z); return; }
	pglNormal3f=pdglGetProcAddress("glNormal3f");
	pglNormal3f(x, y, z);
}

PDGL_API void pdglNormal3fv(float *v)
{
	if(pglNormal3fv)
		{ pglNormal3fv(v); return; }
	pglNormal3fv=pdglGetProcAddress("glNormal3fv");
	pglNormal3fv(v);
}


PDGL_API void pdglColorMask(bool r, bool g, bool b, bool a)
{
	if(pglColorMask)
		{ pglColorMask(r, g, b, a); return; }
	pglColorMask=pdglGetProcAddress("glColorMask");
	pglColorMask(r, g, b, a);
}



/* Lighting */
PDGL_API void pdglShadeModel(GLenum mode)
{
	if(pglShadeModel)
		{ pglShadeModel(mode); return; }
	pglShadeModel=pdglGetProcAddress("glShadeModel");
	pglShadeModel(mode);
}

PDGL_API void pdglLightf(GLenum light, GLenum pname, GLfloat param)
{
	if(pglLightf)
		{ pglLightf(light, pname, param); return; }
	pglLightf=pdglGetProcAddress("glLightf");
	pglLightf(light, pname, param);
}

PDGL_API void pdglLightfv(GLenum light,
	GLenum pname, const GLfloat *params)
{
	if(pglLightfv)
		{ pglLightfv(light, pname, params); return; }
	pglLightfv=pdglGetProcAddress("glLightfv");
	pglLightfv(light, pname, params);
}

PDGL_API void pdglLightModelf(GLenum pname, GLfloat param)
{
	if(pglLightModelf)
		{ pglLightModelf(pname, param); return; }
	pglLightModelf=pdglGetProcAddress("glLightModelf");
	pglLightModelf(pname, param);
}

PDGL_API void pdglLightModeli(GLenum pname, GLint param)
{
	if(pglLightModeli)
		{ pglLightModeli(pname, param); return; }
	pglLightModeli=pdglGetProcAddress("glLightModeli");
	pglLightModeli(pname, param);
}

PDGL_API void pdglLightModelfv(GLenum pname, const GLfloat *params)
{
	if(pglLightModelfv)
		{ pglLightModelfv(pname, params); return; }
	pglLightModelfv=pdglGetProcAddress("glLightModelfv");
	pglLightModelfv(pname, params);
}

PDGL_API void pdglMaterialf(GLenum face, GLenum pname, GLfloat param)
{
	if(pglMaterialf)
		{ pglMaterialf(face, pname, param); return; }
	pglMaterialf=pdglGetProcAddress("glMaterialf");
	pglMaterialf(face, pname, param);
}

PDGL_API void pdglMaterialfv(
	GLenum face, GLenum pname, const GLfloat *params)
{
	if(pglMaterialfv)
		{ pglMaterialfv(face, pname, params); return; }
	pglMaterialfv=pdglGetProcAddress("glMaterialfv");
	pglMaterialfv(face, pname, params);
}

PDGL_API void pdglGetMaterialfv(GLenum face,
	GLenum pname, GLfloat *params)
{
	if(pglGetMaterialfv)
		{ pglGetMaterialfv(face, pname, params); return; }
	pglGetMaterialfv=pdglGetProcAddress("glGetMaterialfv");
	pglGetMaterialfv(face, pname, params);
}

PDGL_API void pdglColorMaterial(GLenum face, GLenum mode)
{
	if(pglColorMaterial)
		{ pglColorMaterial(face, mode); return; }
	pglColorMaterial=pdglGetProcAddress("glColorMaterial");
	pglColorMaterial(face, mode);
}


PDGL_API void pdglTexEnvf(GLenum target, GLenum pname, GLfloat param)
{
	if(pglTexEnvf)
		{ pglTexEnvf(target, pname, param); return; }
	pglTexEnvf=pdglGetProcAddress("glTexEnvf");
	pglTexEnvf(target, pname, param);
}


PDGL_API void pdglClearDepth(float depth)
{
	if(pglClearDepth)
		{ pglClearDepth(depth); return; }
	pglClearDepth=pdglGetProcAddress("glClearDepth");
	pglClearDepth(depth);
}

PDGL_API void pdglDepthFunc(int func)
{
	if(pglDepthFunc)
		{ pglDepthFunc(func); return; }
	pglDepthFunc=pdglGetProcAddress("glDepthFunc");
	pglDepthFunc(func);
}

PDGL_API void pdglDepthMask(GLboolean flag)
{
	if(pglDepthMask)
		{ pglDepthMask(flag); return; }
	pglDepthMask=pdglGetProcAddress("glDepthMask");
	pglDepthMask(flag);
}

PDGL_API void pdglDepthRange(float nearv, float farv)
{
	if(pglDepthRange)
		{ pglDepthRange(nearv, farv); return; }
	pglDepthRange=pdglGetProcAddress("glDepthRange");
	pglDepthRange(nearv, farv);
}


PDGL_API void pdglMatrixMode(GLenum mode)
{
	if(pglMatrixMode)
		{ pglMatrixMode(mode); return; }
	pglMatrixMode=pdglGetProcAddress("glMatrixMode");
	pglMatrixMode(mode);
}

PDGL_API void pdglViewport(int x, int y, int width, int height)
{
	if(pglViewport)
		{ pglViewport(x, y, width, height); return; }
	pglViewport=pdglGetProcAddress("glViewport");
	pglViewport(x, y, width, height);
}

PDGL_API void pdglLoadIdentity(void)
{
	if(pglLoadIdentity)
		{ pglLoadIdentity(); return; }
	pglLoadIdentity=pdglGetProcAddress("glLoadIdentity");
	pglLoadIdentity();
}

PDGL_API void pdglPushMatrix(void)
{
	if(pglPushMatrix)
		{ pglPushMatrix(); return; }
	pglPushMatrix=pdglGetProcAddress("glPushMatrix");
	pglPushMatrix();
}

PDGL_API void pdglPopMatrix(void)
{
	if(pglPopMatrix)
		{ pglPopMatrix(); return; }
	pglPopMatrix=pdglGetProcAddress("glPopMatrix");
	pglPopMatrix();
}

PDGL_API void pdglMultMatrixf(float *mat)
{
	if(pglMultMatrixf)
		{ pglMultMatrixf(mat); return; }
	pglMultMatrixf=pdglGetProcAddress("glMultMatrixf");
	pglMultMatrixf(mat);
}

PDGL_API void pdglTranslatef(float x, float y, float z)
{
	if(pglTranslatef)
		{ pglTranslatef(x, y, z); return; }
	pglTranslatef=pdglGetProcAddress("glTranslatef");
	pglTranslatef(x, y, z);
}

PDGL_API void pdglScalef(float x, float y, float z)
{
	if(pglScalef)
		{ pglScalef(x, y, z); return; }
	pglScalef=pdglGetProcAddress("glScalef");
	pglScalef(x, y, z);
}

PDGL_API void pdglRotatef(float th, float nx, float ny, float nz)
{
	if(pglRotatef)
		{ pglRotatef(th, nx, ny, nz); return; }
	pglRotatef=pdglGetProcAddress("glRotatef");
	pglRotatef(th, nx, ny, nz);
}

PDGL_API void pdglOrtho(
	float mx, float nx,
	float my, float ny,
	float mz, float nz)
{
	if(pglOrtho)
		{ pglOrtho(mx, nx, my, ny, mz, nz); return; }
	pglOrtho=pdglGetProcAddress("glOrtho");
	pglOrtho(mx, nx, my, ny, mz, nz);
}

void pdglFrustum(
	float mx, float nx,
	float my, float ny,
	float mz, float nz)
{
	if(pglFrustum)
		{ pglFrustum(mx, nx, my, ny, mz, nz); return; }
	pglFrustum=pdglGetProcAddress("glFrustum");
	pglFrustum(mx, nx, my, ny, mz, nz);
}

PDGL_API void pdglTranslated(double x, double y, double z)
	{ pdglTranslatef(x, y, z); }


PDGL_API void pdglGetFloatv( GLenum pname, GLfloat *params )
{
	if(pglGetFloatv)
		{ pglGetFloatv(pname, params); return; }
	pglGetFloatv=pdglGetProcAddress("glGetFloatv");
	pglGetFloatv(pname, params);
}

PDGL_API void pdglGetIntegerv( GLenum pname, GLint *params )
{
	if(pglGetIntegerv)
		{ pglGetIntegerv(pname, params); return; }
	pglGetIntegerv=pdglGetProcAddress("glGetIntegerv");
	pglGetIntegerv(pname, params);
}


PDGL_API void pdglTexEnvi(GLenum target, GLenum pname, GLint param)
{
	if(pglTexEnvi)
		{ pglTexEnvi(target, pname, param); return; }
	pglTexEnvi=pdglGetProcAddress("glTexEnvi");
	pglTexEnvi(target, pname, param);
}

PDGL_API void pdglTexEnvfv(GLenum target,
	GLenum pname, const GLfloat *params)
{
	if(pglTexEnvfv)
		{ pglTexEnvfv(target, pname, params); return; }
	pglTexEnvfv=pdglGetProcAddress("glTexEnvfv");
	pglTexEnvfv(target, pname, params);
}

PDGL_API void pdglTexEnviv(GLenum target,
	GLenum pname, const GLint *params)
{
	if(pglTexEnviv)
		{ pglTexEnviv(target, pname, params); return; }
	pglTexEnviv=pdglGetProcAddress("glTexEnviv");
	pglTexEnviv(target, pname, params);
}

PDGL_API void pdglTexParameterf(GLenum target,
	GLenum pname, GLfloat param)
{
	if(pglTexParameterf)
		{ pglTexParameterf(target, pname, param); return; }
	pglTexParameterf=pdglGetProcAddress("glTexParameterf");
	pglTexParameterf(target, pname, param);
}

PDGL_API void pdglTexParameteri(GLenum target,
	GLenum pname, GLint param)
{
	if(pglTexParameteri)
		{ pglTexParameteri(target, pname, param); return; }
	pglTexParameteri=pdglGetProcAddress("glTexParameteri");
	pglTexParameteri(target, pname, param);
}

PDGL_API void pdglTexParameterfv(GLenum target,
	GLenum pname, const GLfloat *params)
{
	if(pglTexParameterfv)
		{ pglTexParameterfv(target, pname, params); return; }
	pglTexParameterfv=pdglGetProcAddress("glTexParameterfv");
	pglTexParameterfv(target, pname, params);
}

PDGL_API void pdglTexParameteriv(GLenum target,
	GLenum pname, const GLint *params)
{
	if(pglTexParameteriv)
		{ pglTexParameteriv(target, pname, params); return; }
	pglTexParameteriv=pdglGetProcAddress("glTexParameteriv");
	pglTexParameteriv(target, pname, params);
}

PDGL_API void pdglTexImage1D(GLenum target, GLint level,
	GLint internalFormat, GLsizei width, GLint border,
	GLenum format, GLenum type, const GLvoid *pixels)
{
	if(pglTexImage1D)
	{
		pglTexImage1D(target, level, internalFormat,
			width, border, format, type, pixels);
		return;
	}
	pglTexImage1D=pdglGetProcAddress("glTexImage1D");
	pglTexImage1D(target, level, internalFormat,
		width, border, format, type, pixels);
}

PDGL_API void pdglTexImage2D(GLenum target, GLint level,
	GLint internalFormat, GLsizei width, GLsizei height,
	GLint border, GLenum format, GLenum type, const GLvoid *pixels)
{
	if(pglTexImage2D)
	{
		pglTexImage2D(target, level, internalFormat,
			width, height, border, format, type, pixels);
		return;
	}
	pglTexImage2D=pdglGetProcAddress("glTexImage2D");
	pglTexImage2D(target, level, internalFormat,
		width, height, border, format, type, pixels);
}

PDGL_API void pdglGetTexImage(GLenum target, GLint level,
	GLenum format, GLenum type, GLvoid *pixels)
{
	if(pglGetTexImage)
		{ pglGetTexImage(target, level, format, type, pixels); return; }
	pglGetTexImage=pdglGetProcAddress("glGetTexImage");
	pglGetTexImage(target, level, format, type, pixels);
}

PDGL_API void pdglCopyTexImage2D( GLenum target, GLint level,
	GLenum internalformat, GLint x, GLint y,
	GLsizei width, GLsizei height, GLint border )
{
	if(pglCopyTexImage2D)
	{
		pglCopyTexImage2D(target, level, internalformat,
			x, y, width, height, border);
		return;
	}
	pglCopyTexImage2D=pdglGetProcAddress("glCopyTexImage2D");
	pglCopyTexImage2D(target, level, internalformat,
		x, y, width, height, border);
}




PDGL_API void pdglNormal3sbv(sbyte *v)
{
	float ntmp[3];

	ntmp[0]=v[0]*(1.0/127.0);
	ntmp[1]=v[1]*(1.0/127.0);
	ntmp[2]=v[2]*(1.0/127.0);
	pdglNormal3fv(ntmp);
}

PDGL_API void pdglNormal3bv(sbyte *v)
{
	float ntmp[3];

	ntmp[0]=v[0]*(1.0/127.0);
	ntmp[1]=v[1]*(1.0/127.0);
	ntmp[2]=v[2]*(1.0/127.0);
	pdglNormal3fv(ntmp);
}

PDGL_API void pdglColor4ubv(byte *v)
{
	float clr[4];
	clr[0]=v[0]*(1.0/255.0);
	clr[1]=v[1]*(1.0/255.0);
	clr[2]=v[2]*(1.0/255.0);
	clr[3]=v[3]*(1.0/255.0);
	pdglColor4fv(clr);
}

PDGL_API void pdglColor4ub(int r, int g, int b, int a)
	{ pdglColor4f(r/255.0, g/255.0, b/255.0, a/255.0); }

PDGL_API void pdglEnableTexture2D(void)
	{ pdglEnable(GL_TEXTURE_2D); }
PDGL_API void pdglDisableTexture2D(void)
	{ pdglDisable(GL_TEXTURE_2D); }
PDGL_API void pdglEnableLighting(void)
	{ pdglEnable(GL_LIGHTING); }
PDGL_API void pdglDisableLighting(void)
	{ pdglDisable(GL_LIGHTING); }
PDGL_API void pdglModelviewMatrix(void)
	{ pdglMatrixMode(GL_MODELVIEW); }
PDGL_API void pdglProjectionMatrix(void)
	{ pdglMatrixMode(GL_PROJECTION); }
PDGL_API void pdglGetModelviewMatrix(float *v)
	{ pdglGetFloatv(GL_MODELVIEW_MATRIX, v); }

PDGL_API void PDGL_Light0_ConstantAttenuation(float v)
	{ pdglLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, v); }
PDGL_API void PDGL_Light0_LinearAttenuation(float v)
	{ pdglLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, v); }
PDGL_API void PDGL_Light0_QuadraticAttenuation(float v)
	{ pdglLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, v); }
PDGL_API void PDGL_Enable_Light0(void)
	{ pdglEnable(GL_LIGHT0); }

PDGL_API void PDGL_ColorMaterial_Ambient(float *v)
	{ pdglMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, v); }
PDGL_API void PDGL_ColorMaterial_Diffuse(float *v)
	{ pdglMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, v); }
PDGL_API void PDGL_ColorMaterial_Specular(float *v)
	{ pdglMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, v); }

PDGL_API void PDGL_ColorMaterial_Emission(float *v)
{
	pdglMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, v);
}

PDGL_API void PDGL_ColorMaterial_Shininess(float *v)
{
	pdglMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, v);
}


PDGL_API void PDGL_Light0_Ambient(float *v)
{
	pdglLightfv(GL_LIGHT0, GL_AMBIENT, v);
}

PDGL_API void PDGL_Light0_Diffuse(float *v)
{
	pdglLightfv(GL_LIGHT0, GL_DIFFUSE, v);
}

PDGL_API void PDGL_Light0_Specular(float *v)
{
	pdglLightfv(GL_LIGHT0, GL_SPECULAR, v);
}

PDGL_API void PDGL_Light0_Position(float *v)
{
	pdglLightfv(GL_LIGHT0, GL_POSITION, v);
}

PDGL_API void PDGL_Light0_SpotDirection(float *v)
{
	pdglLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, v);
}

PDGL_API void PDGL_Light0_SpotCutoff(float v)
{
	pdglLightf(GL_LIGHT0, GL_SPOT_CUTOFF, v);
}

PDGL_API void PDGL_Light0_SpotExponent(float v)
{
	pdglLightf(GL_LIGHT0, GL_SPOT_EXPONENT, v);
}


PDGL_API void PDGL_DrawPrim_DrawArraysSimpleFlat(
	int prim, int base, int nxyz,
	int xyzsize, int xyztype, int xyzstep, void *xyz)
{
	pdglEnableClientState(GL_VERTEX_ARRAY);
	pdglVertexPointer(xyzsize, xyztype, xyzstep, xyz);
	pdglDrawArrays(prim, base, nxyz);
	pdglDisableClientState(GL_VERTEX_ARRAY);
}

PDGL_API void PDGL_DrawPrim_DrawArraysNormalFlat(
	int prim, int base, int nxyz,
	int xyzsize, int xyztype, int xyzstep, void *xyz, 
	int normsize, int normtype, int normstep, void *norm)
{
	pdglEnableClientState(GL_VERTEX_ARRAY);
	pdglEnableClientState(GL_NORMAL_ARRAY);
	pdglVertexPointer(xyzsize, xyztype, xyzstep, xyz);
	pdglNormalPointer(normtype, normstep, norm);
	pdglDrawArrays(prim, base, nxyz);
	pdglDisableClientState(GL_VERTEX_ARRAY);
	pdglDisableClientState(GL_NORMAL_ARRAY);
}

PDGL_API void PDGL_DrawPrim_DrawArraysNormalFlatRGB(
	int prim, int base, int nxyz,
	int xyzsize, int xyztype, int xyzstep, void *xyz, 
	int normsize, int normtype, int normstep, void *norm,
	int rgbsize, int rgbtype, int rgbstep, void *rgb)
{
	pdglEnableClientState(GL_VERTEX_ARRAY);
	pdglEnableClientState(GL_NORMAL_ARRAY);
	pdglEnableClientState(GL_COLOR_ARRAY);
	pdglVertexPointer(xyzsize, xyztype, xyzstep, xyz);
	pdglNormalPointer(normtype, normstep, norm);
	pdglColorPointer(rgbsize, rgbtype, rgbstep, rgb);
	pdglDrawArrays(prim, base, nxyz);
	pdglDisableClientState(GL_VERTEX_ARRAY);
	pdglDisableClientState(GL_NORMAL_ARRAY);
	pdglDisableClientState(GL_COLOR_ARRAY);
}

PDGL_API void PDGL_DrawPrim_DrawArraysNormalTex(
	int prim, int base, int nxyz,
	int xyzsize, int xyztype, int xyzstep, void *xyz, 
	int stsize, int sttype, int ststep, void *st, 
	int normsize, int normtype, int normstep, void *norm)
{
	pdglEnableClientState(GL_VERTEX_ARRAY);
	pdglEnableClientState(GL_TEXTURE_COORD_ARRAY);
	pdglEnableClientState(GL_NORMAL_ARRAY);
	pdglVertexPointer(xyzsize, xyztype, xyzstep, xyz);
	pdglTexCoordPointer(stsize, sttype, ststep, st);
	pdglNormalPointer(normtype, normstep, norm);
	pdglDrawArrays(prim, base, nxyz);
	pdglDisableClientState(GL_VERTEX_ARRAY);
	pdglDisableClientState(GL_TEXTURE_COORD_ARRAY);
	pdglDisableClientState(GL_NORMAL_ARRAY);
}

PDGL_API void PDGL_DrawPrim_DrawArraysNormalTexRGB(
	int prim, int base, int nxyz,
	int xyzsize, int xyztype, int xyzstep, void *xyz, 
	int stsize, int sttype, int ststep, void *st, 
	int normsize, int normtype, int normstep, void *norm,
	int rgbsize, int rgbtype, int rgbstep, void *rgb)
{
	pdglEnableClientState(GL_VERTEX_ARRAY);
	pdglEnableClientState(GL_TEXTURE_COORD_ARRAY);
	pdglEnableClientState(GL_NORMAL_ARRAY);
	pdglEnableClientState(GL_COLOR_ARRAY);
	pdglVertexPointer(xyzsize, xyztype, xyzstep, xyz);
	pdglTexCoordPointer(stsize, sttype, ststep, st);
	pdglNormalPointer(normtype, normstep, norm);
	pdglColorPointer(rgbsize, rgbtype, rgbstep, rgb);
	pdglDrawArrays(prim, base, nxyz);
	pdglDisableClientState(GL_VERTEX_ARRAY);
	pdglDisableClientState(GL_TEXTURE_COORD_ARRAY);
	pdglDisableClientState(GL_NORMAL_ARRAY);
	pdglDisableClientState(GL_COLOR_ARRAY);
}

PDGL_API void PDGL_DrawPrim_DrawArraysTexRGB(
	int prim, int base, int nxyz,
	int xyzsize, int xyztype, int xyzstep, void *xyz, 
	int stsize, int sttype, int ststep, void *st, 
	int rgbsize, int rgbtype, int rgbstep, void *rgb)
{
	pdglEnableClientState(GL_VERTEX_ARRAY);
	pdglEnableClientState(GL_TEXTURE_COORD_ARRAY);
	pdglEnableClientState(GL_COLOR_ARRAY);
	pdglVertexPointer(xyzsize, xyztype, xyzstep, xyz);
	pdglTexCoordPointer(stsize, sttype, ststep, st);
	pdglColorPointer(rgbsize, rgbtype, rgbstep, rgb);
	pdglDrawArrays(prim, base, nxyz);
	pdglDisableClientState(GL_VERTEX_ARRAY);
	pdglDisableClientState(GL_TEXTURE_COORD_ARRAY);
	pdglDisableClientState(GL_COLOR_ARRAY);
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
