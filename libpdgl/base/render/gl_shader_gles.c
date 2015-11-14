#include <pdgl.h>

#ifdef GLES2

char *pdgl_shader_name[1024];
int pdgl_shader_hdl[1024];
int pdgl_shader_cnt;
int pdgl_shader_cur;

int pdgl_shader_default;
int pdgl_shader_flat;

void *pdglGetProcAddress(char *name)
{
	return(dyllGetAddr(name));
}

PDGL_API int PDGL_InitShader()
{
	static int init=0;

	if(init)return(0);
	init=1;

	printf("PDGL_InitShader\n");
	pdgl_shader_cnt=0;

	pdgl_shader_default=PDGL_LoadShader("bgb_default");
	pdgl_shader_flat=PDGL_LoadShader("bgb_flat");

	return(1);
}

PDGL_API int PDGL_LookupShaderSource(char *name, int ty)
{
	char tb[256];
	int i, j;

	sprintf(tb, "shaders_gles/%s.%s", name,
		(ty==GL_VERTEX_SHADER)?"vert":"frag");

	for(i=0; i<pdgl_shader_cnt; i++)
		if(!strcmp(pdgl_shader_name[i], tb))
			return(pdgl_shader_hdl[i]);
	return(-1);
}

PDGL_API int PDGL_LoadShaderSource(char *name, int ty)
{
	char tb[4096];
	VFILE *fd;
	char *buf;
	int i, j;

	if((ty!=GL_VERTEX_SHADER) && (ty!=GL_FRAGMENT_SHADER))
		return(-1);

	i=PDGL_LookupShaderSource(name, ty);
	if(i>0)return(i);

	sprintf(tb, "shaders_gles/%s.%s", name,
		(ty==GL_VERTEX_SHADER)?"vert":"frag");
	fd=vffopen(tb, "rb");
	if(!fd)return(-1);
	buf=vf_bufferin(fd);
	vfclose(fd);

	i=glCreateShader(ty);
	glShaderSource(i, 1, (const char **)(&buf), NULL);
	glCompileShader(i);

	glGetShaderiv(i, GL_COMPILE_STATUS, &j);
	if(j==GL_FALSE)
	{
		printf("Fail compile shader %s\n", tb);
		
		glGetShaderInfoLog(i, 4096, &j, tb);
		printf("%s\n", tb);
		
		return(-1);
	}

	j=pdgl_shader_cnt++;
	pdgl_shader_name[j]=strdup(tb);
	pdgl_shader_hdl[j]=i;

	return(i);
}


PDGL_API int PDGL_LookupShader(char *name)
{
	int i, j;

	for(i=0; i<pdgl_shader_cnt; i++)
		if(!strcmp(pdgl_shader_name[i], name))
			return(pdgl_shader_hdl[i]);
	return(-1);
}

PDGL_API int PDGL_LoadShader(char *name)
{
	int i, j, k;

	PDGL_InitShader();

	i=PDGL_LookupShader(name);
	if(i>0)return(i);

	i=PDGL_LoadShaderSource(name, GL_VERTEX_SHADER);
	j=PDGL_LoadShaderSource(name, GL_FRAGMENT_SHADER);

	if((i<0) || (j<0))
		return(-1);

	k=glCreateProgram();
	glAttachShader(k, i);
	glAttachShader(k, j);
	glLinkProgram(k);

	i=pdgl_shader_cnt++;
	pdgl_shader_name[i]=strdup(name);
	pdgl_shader_hdl[i]=k;

	return(k);
}

PDGL_API int PDGL_BindShader(int num)
{
	if(num<=0)
	{
		pdgl_shader_cur=0;
		glUseProgram(0);
		return(0);
	}

	pdgl_shader_cur=num;
	glUseProgram(num);
	return(0);
}

PDGL_API int PDGL_UnbindShader()
{
	PDGL_BindShader(0);
	return(0);
}


PDGL_API int pdglGetUniformLocation(int shader, char *name)
{
	int i;
	i=glGetUniformLocation(shader, name);
	return(i);
}

PDGL_API int PDGL_CheckForUniform(char *name)
{
	int i;
	i=pdglGetUniformLocation(pdgl_shader_cur, name);
	if(i<0)return(0);
	return(1);
}

PDGL_API int PDGL_Uniform1f(char *name, float x)
{
	int i;
	i=glGetUniformLocation(pdgl_shader_cur, name);
	if(i<0)return(0); glUniform1f(i, x);
	return(1);
}

PDGL_API int PDGL_Uniform2f(char *name, float x, float y)
{
	int i;
	i=glGetUniformLocation(pdgl_shader_cur, name);
	if(i<0)return(0); glUniform2f(i, x, y);
	return(1);
}

PDGL_API int PDGL_Uniform3f(char *name, float x, float y, float z)
{
	int i;
	i=glGetUniformLocation(pdgl_shader_cur, name);
	if(i<0)return(0); glUniform3f(i, x, y, z);
	return(1);
}

PDGL_API int PDGL_Uniform4f(char *name, float x, float y, float z, float w)
{
	int i;
	i=glGetUniformLocation(pdgl_shader_cur, name);
	if(i<0)return(0); glUniform4f(i, x, y, z, w);
	return(1);
}


PDGL_API int PDGL_Uniform1i(char *name, int x)
{
	int i;
	i=glGetUniformLocation(pdgl_shader_cur, name);
	if(i<0)return(0); glUniform1i(i, x);
	return(1);
}

PDGL_API int PDGL_Uniform2i(char *name, int x, int y)
{
	int i;
	i=glGetUniformLocation(pdgl_shader_cur, name);
	if(i<0)return(0); glUniform2i(i, x, y);
	return(1);
}

PDGL_API int PDGL_Uniform3i(char *name, int x, int y, int z)
{
	int i;
	i=glGetUniformLocation(pdgl_shader_cur, name);
	if(i<0)return(0); glUniform3i(i, x, y, z);
	return(1);
}

PDGL_API int PDGL_Uniform4i(char *name, int x, int y, int z, int w)
{
	int i;
	i=glGetUniformLocation(pdgl_shader_cur, name);
	if(i<0)return(0); glUniform4i(i, x, y, z, w);
	return(1);
}

PDGL_API int PDGL_Uniform1fv(char *name, int cnt, float *pf)
{
	int i;
	i=glGetUniformLocation(pdgl_shader_cur, name);
	if(i<0)return(0); glUniform1fv(i, cnt, pf);
	return(1);
}

PDGL_API int PDGL_Uniform2fv(char *name, int cnt, float *pf)
{
	int i;
	i=glGetUniformLocation(pdgl_shader_cur, name);
	if(i<0)return(0); glUniform2fv(i, cnt, pf);
	return(1);
}

PDGL_API int PDGL_Uniform3fv(char *name, int cnt, float *pf)
{
	int i;
	i=glGetUniformLocation(pdgl_shader_cur, name);
	if(i<0)return(0); glUniform3fv(i, cnt, pf);
	return(1);
}

PDGL_API int PDGL_Uniform4fv(char *name, int cnt, float *pf)
{
	int i;
	i=glGetUniformLocation(pdgl_shader_cur, name);
	if(i<0)return(0); glUniform4fv(i, cnt, pf);
	return(1);
}

PDGL_API void pdglUniform1f(int var, float x)
	{ glUniform1f(var, x); }
PDGL_API void pdglUniform2f(int var, float x, float y)
	{ glUniform2f(var, x, y); }
PDGL_API void pdglUniform3f(int var, float x, float y, float z)
	{ glUniform3f(var, x, y, z); }
PDGL_API void pdglUniform4f(int var, float x, float y, float z, float w)
	{ glUniform4f(var, x, y, z, w); }

PDGL_API void pdglUniform1i(int var, int x)
	{ glUniform1i(var, x); }
PDGL_API void pdglUniform2i(int var, int x, int y)
	{ glUniform2i(var, x, y); }
PDGL_API void pdglUniform3i(int var, int x, int y, int z)
	{ glUniform3i(var, x, y, z); }
PDGL_API void pdglUniform4i(int var, int x, int y, int z, int w)
	{ glUniform4i(var, x, y, z, w); }

PDGL_API void pdglUniform1fv(int var, int cnt, float *pf)
	{ glUniform1fv(var, cnt, pf); }
PDGL_API void pdglUniform2fv(int var, int cnt, float *pf)
	{ glUniform2fv(var, cnt, pf); }
PDGL_API void pdglUniform3fv(int var, int cnt, float *pf)
	{ glUniform3fv(var, cnt, pf); }
PDGL_API void pdglUniform4fv(int var, int cnt, float *pf)
	{ glUniform4fv(var, cnt, pf); }

PDGL_API int pdglVertexAttrib1f(int index, float v0)
{
	glVertexAttrib1f(index, v0);
	return(0);
}

PDGL_API int pdglVertexAttrib2f(int index, float v0, float v1)
{
	glVertexAttrib2f(index, v0, v1);
	return(0);
}

PDGL_API int pdglVertexAttrib3f(int index, float v0, float v1, float v2)
{
	glVertexAttrib3f(index, v0, v1, v2);
	return(0);
}

PDGL_API int pdglVertexAttrib4f(int index,
	float v0, float v1, float v2, float v3)
{
	glVertexAttrib4f(index, v0, v1, v2, v3);
	return(0);
}

PDGL_API int pdglVertexAttrib1fv(int index, float *v)
{
	glVertexAttrib1fv(index, v);
	return(0);
}

PDGL_API int pdglVertexAttrib2fv(int index, float *v)
{
	glVertexAttrib2fv(index, v);
	return(0);
}

PDGL_API int pdglVertexAttrib3fv(int index, float *v)
{
	glVertexAttrib3fv(index, v);
	return(0);
}

PDGL_API int pdglVertexAttrib4fv(int index, float *v)
{
	glVertexAttrib4fv(index, v);
	return(0);
}



PDGL_API int pdglGetAttribLocation(int program, char *name)
{
	return(glGetAttribLocation(program, name));
}

PDGL_API int PDGL_GetAttribLocation(char *name)
{
	return(pdglGetAttribLocation(pdgl_shader_cur, name));
}

PDGL_API int pdglVertexAttribPointer(
	int index, int size, int type, int normalized, int stride,
	void *pointer)
{
	glVertexAttribPointer(index, size, type,
		normalized, stride, pointer);
	return(0);
}

PDGL_API int pdglEnableVertexAttribArray(int idx)
{
	glEnableVertexAttribArray(idx);
	return(0);
}

PDGL_API int pdglDisableVertexAttribArray(int idx)
{
	glDisableVertexAttribArray(idx);
	return(0);
}

PDGL_API int pdglActiveTexture(int idx)
{
	glActiveTexture(GL_TEXTURE0+idx);
	return(0);
}

PDGL_API int pdglMultiTexCoord1f(int idx, float s)
{
//	glMultiTexCoord1f(GL_TEXTURE0+idx, s);
	return(0);
}

PDGL_API int pdglMultiTexCoord2f(int idx, float s, float t)
{
//	glMultiTexCoord2f(GL_TEXTURE0+idx, s, t);
	return(0);
}

PDGL_API int pdglMultiTexCoord3f(int idx, float s, float t, float p)
{
//	glMultiTexCoord3f(GL_TEXTURE0+idx, s, t, p);
	return(0);
}

PDGL_API int pdglMultiTexCoord4f(int idx, float s, float t, float p, float q)
{
//	glMultiTexCoord4f(GL_TEXTURE0+idx, s, t, p, q);
	return(0);
}


PDGL_API int pdglMultiTexCoord1fv(int idx, float *v)
{
//	glMultiTexCoord1fv(GL_TEXTURE0+idx, v);
	return(0);
}

PDGL_API int pdglMultiTexCoord2fv(int idx, float *v)
{
//	glMultiTexCoord2fv(GL_TEXTURE0+idx, v);
	return(0);
}

PDGL_API int pdglMultiTexCoord3fv(int idx, float *v)
{
//	glMultiTexCoord3fv(GL_TEXTURE0+idx, v);
	return(0);
}

PDGL_API int pdglMultiTexCoord4fv(int idx, float *v)
{
//	glMultiTexCoord4fv(GL_TEXTURE0+idx, v);
	return(0);
}

PDGL_API int pdglBeginQuery(int tgt, int id)
{
//	glBeginQuery(tgt, id);
	return(0);
}

PDGL_API int pdglEndQuery(int tgt)
{
//	glEndQuery(tgt);
	return(0);
}

PDGL_API int pdglGetQueryObjectiv(int id, int pname, int *params)
{
//	glGetQueryObjectiv(id, pname, params);
	return(0);
}

PDGL_API int pdglGetQueryObjectuiv(int id, int pname, uint *params)
{
//	glGetQueryObjectuiv(id, pname, params);
	return(0);
}


PDGL_API int pdglGenFramebuffers(int n, int *ids)
{
//	glGenFramebuffersEXT(n, ids);
	return(0);
}

PDGL_API int pdglGenRenderbuffers(int n, int *ids)
{
//	glGenRenderbuffersEXT(n, ids);
	return(0);
}

PDGL_API int pdglBindFramebuffer(int target, int framebuffer)
{
//	glBindFramebufferEXT(target, framebuffer);
	return(0);
}

PDGL_API int pdglBindRenderbuffer(int target, int framebuffer)
{
//	glBindRenderbufferEXT(target, framebuffer);
	return(0);
}

PDGL_API int pdglRenderbufferStorage(int target,
	int internalformat, int width, int height)
{
//	glRenderbufferStorageEXT(target, internalformat, width, height);
	return(0);
}

PDGL_API int pdglFramebufferTexture2D(
	int target, int attachment, int textarget, int texture, int level)
{
//	glFramebufferTexture2DEXT(target, attachment, textarget, texture, level);
	return(0);
}

PDGL_API int pdglFramebufferRenderbuffer(
	int target, int attachment, int textarget, int texture)
{
//	glFramebufferRenderbufferEXT(target, attachment, textarget, texture);
	return(0);
}

PDGL_API int pdglCheckFramebufferStatus(int target)
{
	int i;

	i=-1;
//	i=glCheckFramebufferStatusEXT(target);
	return(i);
}

PDGL_API int pdglDrawBuffers(int n, int *ids)
{
//	glDrawBuffers(n, ids);
	return(0);
}

PDGL_API int pdglCompressedTexImage2D(
	int		target,
	int		level,
	int		internalformat,
	int		width,
	int		height,
	int		border,
	int		imageSize,
	const void *data)
{
	glCompressedTexImage2D(target, level, internalformat, width, height,
		border, imageSize, data);
	return(0);
}

#endif
