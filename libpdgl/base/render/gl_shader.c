#ifdef _WIN32
#define PDGL_INC_NOGLHDRS

#include <windows.h>

#include <GL/gl.h>

//#include <GL_2/glext.h>
//#include <GL_2/wglext.h>
// #include <GL_3/glcorearb.h>
#include <GL_3/glext.h>
#include <GL_3/wglext.h>

// #include <GL/gl.h>

#endif

#include <pdgl.h>
#include <math.h>

#ifndef GLES

#ifdef _WIN32
#include <windows.h>
#include <wingdi.h>
#include <commctrl.h>
#endif

#ifndef _WIN32
#include <GL_2/glext.h>
#endif

// #include <bgbccc.h>

char *pdgl_shader_name[1024];
int pdgl_shader_hdl[1024];
int pdgl_shader_cnt;
int pdgl_shader_cur;

typedef void (APIENTRYP PFNGLBINDTEXTUREPROC)	//AH:ignore
	(GLenum target, GLuint texture);

typedef void (APIENTRYP PFNGLDISABLEPROC) (GLenum cap);	//AH:ignore
typedef void (APIENTRYP PFNGLENABLEPROC) (GLenum cap);	//AH:ignore
typedef void (APIENTRYP PFNGLFINISHPROC)	//AH:ignore
	(void);
typedef void (APIENTRYP PFNGLFLUSHPROC)	//AH:ignore
	(void);

PFNGLBINDTEXTUREPROC		pglBindTexture;
PFNGLDISABLEPROC			pglDisable;
PFNGLENABLEPROC				pglEnable;
PFNGLFINISHPROC				pglFinish;

PFNGLATTACHSHADERPROC		pglAttachShader;
PFNGLBINDATTRIBLOCATIONPROC	pglBindAttribLocation;
PFNGLCOMPILESHADERPROC		pglCompileShader;
PFNGLCREATEPROGRAMPROC		pglCreateProgram;
PFNGLCREATESHADERPROC		pglCreateShader;
PFNGLDELETEPROGRAMPROC		pglDeleteProgram;
PFNGLDELETESHADERPROC		pglDeleteShader;
PFNGLDETACHSHADERPROC		pglDetachShader;

PFNGLGETSHADERIVPROC		pglGetShaderiv;
PFNGLGETSHADERINFOLOGPROC		pglGetShaderInfoLog;

PFNGLGETUNIFORMLOCATIONPROC		pglGetUniformLocation;
PFNGLGETUNIFORMFVPROC			pglGetUniformfv;
PFNGLGETUNIFORMIVPROC			pglGetUniformiv;
PFNGLGETVERTEXATTRIBDVPROC		pglGetVertexAttribdv;
PFNGLGETVERTEXATTRIBFVPROC		pglGetVertexAttribfv;
PFNGLGETVERTEXATTRIBIVPROC		pglGetVertexAttribuv;

PFNGLVERTEXATTRIBPOINTERPROC		pglVertexAttribPointer;
PFNGLGETVERTEXATTRIBPOINTERVPROC	pglGetVertexAttribPointerv;

PFNGLLINKPROGRAMPROC		pglLinkProgram;
PFNGLSHADERSOURCEPROC		pglShaderSource;
PFNGLUSEPROGRAMPROC		pglUseProgram;

PFNGLUNIFORM1FPROC		pglUniform1f;
PFNGLUNIFORM2FPROC		pglUniform2f;
PFNGLUNIFORM3FPROC		pglUniform3f;
PFNGLUNIFORM4FPROC		pglUniform4f;
PFNGLUNIFORM1IPROC		pglUniform1i;
PFNGLUNIFORM2IPROC		pglUniform2i;
PFNGLUNIFORM3IPROC		pglUniform3i;
PFNGLUNIFORM4IPROC		pglUniform4i;
PFNGLUNIFORM1FVPROC		pglUniform1fv;
PFNGLUNIFORM2FVPROC		pglUniform2fv;
PFNGLUNIFORM3FVPROC		pglUniform3fv;
PFNGLUNIFORM4FVPROC		pglUniform4fv;
PFNGLUNIFORM1IVPROC		pglUniform1iv;
PFNGLUNIFORM2IVPROC		pglUniform2iv;
PFNGLUNIFORM3IVPROC		pglUniform3iv;
PFNGLUNIFORM4IVPROC		pglUniform4iv;
PFNGLUNIFORMMATRIX2FVPROC	pglUniformMatrix2fv;
PFNGLUNIFORMMATRIX3FVPROC	pglUniformMatrix3fv;
PFNGLUNIFORMMATRIX4FVPROC	pglUniformMatrix4fv;

#ifdef _WIN32
PROC (*pdgl_wglGetProcAddress)(LPCSTR  lpszProc);
#endif

int pdglGlDriverIsGL()
{
	char *gllib;

	gllib=btCvarGet("gl_driver");
	if(!gllib)
		return(1);
	if(!strcmp(gllib, "opengl32"))
		return(1);
	return(0);
}

void *pdglGetProcAddress(char *name)
{
	char *gllib;
	void *ptr;
#ifdef _WIN32
	if(pdgl_wglGetProcAddress)
	{
		ptr=pdgl_wglGetProcAddress(name);
		if(ptr)return(ptr);
		ptr=dyllGetAddr(name);
		return(ptr);
	}
	
	gllib=btCvarGet("gl_driver");
	if(!gllib)gllib="opengl32";
	dyllLoadLibrary(gllib);
	
//	dyllLoadLibrary("opengl32");
	pdgl_wglGetProcAddress=dyllGetAddr("wglGetProcAddress");
	if(pdgl_wglGetProcAddress)
	{
		ptr=pdgl_wglGetProcAddress(name);
		if(ptr)return(ptr);
		ptr=dyllGetAddr(name);
		return(ptr);
//		return(pdgl_wglGetProcAddress(name));
	}
	return(dyllGetAddr(name));
//	return(NULL);
//	return(wglGetProcAddress(name));
#else
	return(dyllGetAddr(name));
#endif
}

PDGL_API int PDGL_InitShader()
{
	static int init=0;

	if(init)return(0);
	init=1;

	printf("PDGL_InitShader\n");

//	ccLoadLibrary("opengl32.dll");
//	ccLoadLibrary("opengl32");

//	pglAttachShader=ccGetAddr("glAttachShader");
//	pglBindAttribLocation=ccGetAddr("glBindAttribLocation");
//	pglCompileShader=ccGetAddr("glCompileShader");
//	pglCreateProgram=ccGetAddr("glCreateProgram");
//	pglCreateShader=ccGetAddr("glCreateShader");
//	pglDeleteProgram=ccGetAddr("glDeleteProgram");
//	pglDeleteShader=ccGetAddr("glDeleteShader");
//	pglDetachShader=ccGetAddr("glDetachShader");

//	pglGetShaderiv=ccGetAddr("glGetShaderiv");

//	pglLinkProgram=ccGetAddr("glLinkProgram");
//	pglShaderSource=ccGetAddr("glShaderSource");
//	pglUseProgram=ccGetAddr("glUseProgram");

//	pglBindTexture=pdglGetProcAddress("glBindTexture");
//	pglDisable=pdglGetProcAddress("glDisable");
//	pglEnable=pdglGetProcAddress("glEnable");
//	pglFinish=pdglGetProcAddress("glFinish");

	pglAttachShader=pdglGetProcAddress("glAttachShader");
	pglBindAttribLocation=pdglGetProcAddress("glBindAttribLocation");
	pglCompileShader=pdglGetProcAddress("glCompileShader");
	pglCreateProgram=pdglGetProcAddress("glCreateProgram");
	pglCreateShader=pdglGetProcAddress("glCreateShader");
	pglDeleteProgram=pdglGetProcAddress("glDeleteProgram");
	pglDeleteShader=pdglGetProcAddress("glDeleteShader");
	pglDetachShader=pdglGetProcAddress("glDetachShader");

	pglGetShaderiv=pdglGetProcAddress("glGetShaderiv");
	pglGetShaderInfoLog=pdglGetProcAddress("glGetShaderInfoLog");

	pglGetUniformLocation=pdglGetProcAddress("glGetUniformLocation");
	pglGetUniformfv=pdglGetProcAddress("glGetUniformfv");
	pglGetUniformiv=pdglGetProcAddress("glGetUniformiv");
	pglGetVertexAttribdv=pdglGetProcAddress("glGetVertexAttribdv");
	pglGetVertexAttribfv=pdglGetProcAddress("glGetVertexAttribfv");
	pglGetVertexAttribuv=pdglGetProcAddress("glGetVertexAttribuv");

	pglVertexAttribPointer=
		pdglGetProcAddress("glVertexAttribPointer");
	pglGetVertexAttribPointerv=
		pdglGetProcAddress("glGetVertexAttribPointerv");

	pglLinkProgram=pdglGetProcAddress("glLinkProgram");
	pglShaderSource=pdglGetProcAddress("glShaderSource");
	pglUseProgram=pdglGetProcAddress("glUseProgram");

	pglUniform1f=pdglGetProcAddress("glUniform1f");
	pglUniform2f=pdglGetProcAddress("glUniform2f");
	pglUniform3f=pdglGetProcAddress("glUniform3f");
	pglUniform4f=pdglGetProcAddress("glUniform4f");
	pglUniform1i=pdglGetProcAddress("glUniform1i");
	pglUniform2i=pdglGetProcAddress("glUniform2i");
	pglUniform3i=pdglGetProcAddress("glUniform3i");
	pglUniform4i=pdglGetProcAddress("glUniform4i");
	pglUniform1fv=pdglGetProcAddress("glUniform1fv");
	pglUniform2fv=pdglGetProcAddress("glUniform2fv");
	pglUniform3fv=pdglGetProcAddress("glUniform3fv");
	pglUniform4fv=pdglGetProcAddress("glUniform4fv");
	pglUniform1iv=pdglGetProcAddress("glUniform1iv");
	pglUniform2iv=pdglGetProcAddress("glUniform2iv");
	pglUniform3iv=pdglGetProcAddress("glUniform3iv");
	pglUniform4iv=pdglGetProcAddress("glUniform4iv");
	pglUniformMatrix2fv=pdglGetProcAddress("glUniformMatrix2fv");
	pglUniformMatrix3fv=pdglGetProcAddress("glUniformMatrix3fv");
	pglUniformMatrix4fv=pdglGetProcAddress("glUniformMatrix4fv");

	printf("PDGL_InitShader: %s\n", pglCreateShader?"OK":"Fail");

	pdgl_shader_cnt=0;

	return(1);
}

PDGL_API int PDGL_LookupShaderSource(char *name, int ty)
{
	char tb[256];
	int i, j;

	sprintf(tb, "shaders/%s.%s", name,
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

	if(!pglShaderSource)
		return(-1);

	if((ty!=GL_VERTEX_SHADER) && (ty!=GL_FRAGMENT_SHADER))
		return(-1);

	i=PDGL_LookupShaderSource(name, ty);
	if(i>0)return(i);

	sprintf(tb, "shaders/%s.%s", name,
		(ty==GL_VERTEX_SHADER)?"vert":"frag");
//	fd=vffopen(tb, "rb");
	fd=btcl_vffopen(tb, "rb");
	if(!fd)return(-1);
	buf=vf_bufferin(fd);
	vfclose(fd);

	i=pglCreateShader(ty);
	pglShaderSource(i, 1, (const char **)(&buf), NULL);
	pglCompileShader(i);

	pglGetShaderiv(i, GL_COMPILE_STATUS, &j);
	if(j==GL_FALSE)
	{
		printf("Fail compile shader %s\n", tb);
		
		pglGetShaderInfoLog(i, 4096, &j, tb);
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

	if(!pglCreateProgram || !pglLinkProgram)
		return(-1);

	i=PDGL_LoadShaderSource(name, GL_VERTEX_SHADER);
	j=PDGL_LoadShaderSource(name, GL_FRAGMENT_SHADER);

	if((i<0) || (j<0))
		return(-1);

	k=pglCreateProgram();
	pglAttachShader(k, i);
	pglAttachShader(k, j);
	pglLinkProgram(k);

	i=pdgl_shader_cnt++;
	pdgl_shader_name[i]=strdup(name);
	pdgl_shader_hdl[i]=k;

	return(k);
}

PDGL_API int PDGL_BindShader(int num)
{
	if(pdgl_shader_cur==num)
		return(0);

	if(!pglUseProgram)
		return(-1);
	if(num<=0)
	{
		pdgl_shader_cur=0;
		pglUseProgram(0);
		return(0);
	}

	pdgl_shader_cur=num;
	pglUseProgram(num);
	return(0);
}

PDGL_API int PDGL_UnbindShader()
{
	PDGL_BindShader(0);
	return(0);
}

PDGL_API int PDGL_CheckGlExtension(char *str)
{
	return(BGBBTJ_CheckGlExtension(str));
}

#ifndef GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX
#define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 0x9048
#endif

#ifndef GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX
#define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049
#endif

PDGL_API int PDGL_GetFreeVideoMemory()
{
	int mem[]={-1,-1,-1,-1};

	if(PDGL_CheckGlExtension("GL_NVX_gpu_memory_info"))
	{
		pdglGetIntegerv(GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX,
			&mem[0]);
//		printf("PDGL_GetFreeVideoMemory: NVidia\n");
	}

	if(PDGL_CheckGlExtension("GL_ATI_meminfo"))
	{
		pdglGetIntegerv(GL_TEXTURE_FREE_MEMORY_ATI,
			mem);
//		printf("PDGL_GetFreeVideoMemory: ATI/AMD\n");
	}

	return(mem[0]);
}


PDGL_API int pdglGetUniformLocation(int shader, char *name)
{
	int i;
//	return(pdglGetAttribLocation(pdgl_shader_cur, name));

	if(!pglGetUniformLocation)return(-1);
	i=pglGetUniformLocation(shader, name);
	return(i);
}

PDGL_API int pdglGetUniformLocationF(int shader, char *name)
{
	static int hash_sdr[4096];
	static char *hash_name[4096];
	static int hash_var[4096];
	char *s;
	int i, hi;
//	return(pdglGetAttribLocation(pdgl_shader_cur, name));

//	hi=shader*4093; s=name;
//	while(*s) { hi=(hi*4093)+(*s++); }

	hi=((shader*4093)+((nlint)name))*4093;
	hi=(hi>>12)&4095;
	
	if((hash_sdr[hi]==shader) && (hash_name[hi]==name))
//	if((hash_sdr[hi]==shader) && hash_name[hi] &&
//		!strcmp(hash_name[hi], name))
	{
		return(hash_var[hi]);
	}

	if(!pglGetUniformLocation)return(-1);
	i=pglGetUniformLocation(shader, name);
	
	if(i>0)
//	if(i>=0)
	{
		hash_sdr[hi]=shader;
		hash_name[hi]=name;
		hash_var[hi]=i;
	}

	return(i);
}

PDGL_API void PDGL_ErrorStatusUniform(char *name)
{
	int i;

	i=pdglGetError();
	while(i!=GL_NO_ERROR)
	{
//		printf("PDGL_ErrorStatusUniform: Var=%s Error 0x%04X\n", name, i);
//		*(int *)-1=-1;
		i=pdglGetError();
	}
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
	if(!pglGetUniformLocation)return(-1);
	i=pdglGetUniformLocationF(pdgl_shader_cur, name);
//	i=pglGetUniformLocation(pdgl_shader_cur, name);
	if(i<0)return(0); pglUniform1f(i, x);
	PDGL_ErrorStatusUniform(name);
	return(1);
}

PDGL_API int PDGL_Uniform2f(char *name, float x, float y)
{
	int i;
	if(!pglGetUniformLocation)return(-1);
//	i=pglGetUniformLocation(pdgl_shader_cur, name);
	i=pdglGetUniformLocationF(pdgl_shader_cur, name);
	if(i<0)return(0); pglUniform2f(i, x, y);
	PDGL_ErrorStatusUniform(name);
	return(1);
}

PDGL_API int PDGL_Uniform3f(char *name, float x, float y, float z)
{
	int i;
	if(!pglGetUniformLocation)return(-1);
//	i=pglGetUniformLocation(pdgl_shader_cur, name);
	i=pdglGetUniformLocationF(pdgl_shader_cur, name);
	if(i<0)return(0); pglUniform3f(i, x, y, z);
	PDGL_ErrorStatusUniform(name);
	return(1);
}

PDGL_API int PDGL_Uniform4f(char *name, float x, float y, float z, float w)
{
	int i;
	if(!pglGetUniformLocation)return(-1);
//	i=pglGetUniformLocation(pdgl_shader_cur, name);
	i=pdglGetUniformLocationF(pdgl_shader_cur, name);
	if(i<0)return(0); pglUniform4f(i, x, y, z, w);
	PDGL_ErrorStatusUniform(name);
	return(1);
}


PDGL_API int PDGL_Uniform1i(char *name, int x)
{
	int i;
	if(!pglGetUniformLocation)return(-1);
//	i=pglGetUniformLocation(pdgl_shader_cur, name);
	i=pdglGetUniformLocationF(pdgl_shader_cur, name);
	if(i<0)return(0); pglUniform1i(i, x);
	PDGL_ErrorStatusUniform(name);
	return(1);
}

PDGL_API int PDGL_Uniform2i(char *name, int x, int y)
{
	int i;
	if(!pglGetUniformLocation)return(-1);
//	i=pglGetUniformLocation(pdgl_shader_cur, name);
	i=pdglGetUniformLocationF(pdgl_shader_cur, name);
	if(i<0)return(0); pglUniform2i(i, x, y);
	PDGL_ErrorStatusUniform(name);
	return(1);
}

PDGL_API int PDGL_Uniform3i(char *name, int x, int y, int z)
{
	int i;
	if(!pglGetUniformLocation)return(-1);
//	i=pglGetUniformLocation(pdgl_shader_cur, name);
	i=pdglGetUniformLocationF(pdgl_shader_cur, name);
	if(i<0)return(0); pglUniform3i(i, x, y, z);
	PDGL_ErrorStatusUniform(name);
	return(1);
}

PDGL_API int PDGL_Uniform4i(char *name, int x, int y, int z, int w)
{
	int i;
	if(!pglGetUniformLocation)return(-1);
//	i=pglGetUniformLocation(pdgl_shader_cur, name);
	i=pdglGetUniformLocationF(pdgl_shader_cur, name);
	if(i<0)return(0); pglUniform4i(i, x, y, z, w);
	PDGL_ErrorStatusUniform(name);
	return(1);
}

PDGL_API int PDGL_Uniform1fv(char *name, int cnt, float *pf)
{
	int i;
	if(!pglGetUniformLocation)return(-1);
//	i=pglGetUniformLocation(pdgl_shader_cur, name);
	i=pdglGetUniformLocationF(pdgl_shader_cur, name);
	if(i<0)return(0); pglUniform1fv(i, cnt, pf);
	PDGL_ErrorStatusUniform(name);
	return(1);
}

PDGL_API int PDGL_Uniform2fv(char *name, int cnt, float *pf)
{
	int i;
	if(!pglGetUniformLocation)return(-1);
//	i=pglGetUniformLocation(pdgl_shader_cur, name);
	i=pdglGetUniformLocationF(pdgl_shader_cur, name);
	if(i<0)return(0); pglUniform2fv(i, cnt, pf);
	PDGL_ErrorStatusUniform(name);
	return(1);
}

PDGL_API int PDGL_Uniform3fv(char *name, int cnt, float *pf)
{
	int i;
	if(!pglGetUniformLocation)return(-1);
//	i=pglGetUniformLocation(pdgl_shader_cur, name);
	i=pdglGetUniformLocationF(pdgl_shader_cur, name);
	if(i<0)return(0); pglUniform3fv(i, cnt, pf);
	PDGL_ErrorStatusUniform(name);
	return(1);
}

PDGL_API int PDGL_Uniform4fv(char *name, int cnt, float *pf)
{
	int i;
	if(!pglGetUniformLocation)return(-1);
//	i=pglGetUniformLocation(pdgl_shader_cur, name);
	i=pdglGetUniformLocationF(pdgl_shader_cur, name);
	if(i<0)return(0); pglUniform4fv(i, cnt, pf);
	PDGL_ErrorStatusUniform(name);
	return(1);
}


#if 1
PDGL_API void pdglUniform1f(int var, float x)
	{ if(pglUniform1f)pglUniform1f(var, x); }
PDGL_API void pdglUniform2f(int var, float x, float y)
	{ if(pglUniform2f)pglUniform2f(var, x, y); }
PDGL_API void pdglUniform3f(int var, float x, float y, float z)
	{ if(pglUniform3f)pglUniform3f(var, x, y, z); }
PDGL_API void pdglUniform4f(int var, float x, float y, float z, float w)
	{ if(pglUniform4f)pglUniform4f(var, x, y, z, w); }

PDGL_API void pdglUniform1i(int var, int x)
	{ if(pglUniform1i)pglUniform1i(var, x); }
PDGL_API void pdglUniform2i(int var, int x, int y)
	{ if(pglUniform2i)pglUniform2i(var, x, y); }
PDGL_API void pdglUniform3i(int var, int x, int y, int z)
	{ if(pglUniform3i)pglUniform3i(var, x, y, z); }
PDGL_API void pdglUniform4i(int var, int x, int y, int z, int w)
	{ if(pglUniform4i)pglUniform4i(var, x, y, z, w); }

PDGL_API void pdglUniform1fv(int var, int cnt, float *pf)
	{ if(pglUniform1fv)pglUniform1fv(var, cnt, pf); }
PDGL_API void pdglUniform2fv(int var, int cnt, float *pf)
	{ if(pglUniform2fv)pglUniform2fv(var, cnt, pf); }
PDGL_API void pdglUniform3fv(int var, int cnt, float *pf)
	{ if(pglUniform3fv)pglUniform3fv(var, cnt, pf); }
PDGL_API void pdglUniform4fv(int var, int cnt, float *pf)
	{ if(pglUniform4fv)pglUniform4fv(var, cnt, pf); }
#endif

PDGL_API int pdglVertexAttrib1f(int index, float v0)
{
	static PFNGLVERTEXATTRIB1FPROC fcn=NULL;
	static int set=0;

	if(!fcn)
	{
		if(set)return(-1); set=1;
		fcn=pdglGetProcAddress("glVertexAttrib1f");
		if(!fcn)return(-1);
	}

	fcn(index, v0);
	return(0);
}

PDGL_API int pdglVertexAttrib2f(int index, float v0, float v1)
{
	static PFNGLVERTEXATTRIB2FPROC fcn=NULL;
	static int set=0;

	if(!fcn)
	{
		if(set)return(-1); set=1;
		fcn=pdglGetProcAddress("glVertexAttrib2f");
		if(!fcn)return(-1);
	}

	fcn(index, v0, v1);
	return(0);
}

PDGL_API int pdglVertexAttrib3f(int index, float v0, float v1, float v2)
{
	static PFNGLVERTEXATTRIB3FPROC fcn=NULL;
	static int set=0;

	if(!fcn)
	{
		if(set)return(-1); set=1;
		fcn=pdglGetProcAddress("glVertexAttrib3f");
		if(!fcn)return(-1);
	}

	fcn(index, v0, v1, v2);
	return(0);
}

PDGL_API int pdglVertexAttrib4f(int index,
	float v0, float v1, float v2, float v3)
{
	static PFNGLVERTEXATTRIB4FPROC fcn=NULL;
	static int set=0;

	if(!fcn)
	{
		if(set)return(-1); set=1;
		fcn=pdglGetProcAddress("glVertexAttrib4f");
		if(!fcn)return(-1);
	}

	fcn(index, v0, v1, v2, v3);
	return(0);
}

PDGL_API int pdglVertexAttrib1fv(int index, float *v)
{
	static PFNGLVERTEXATTRIB1FVPROC fcn=NULL;
	static int set=0;

	if(!fcn)
	{
		if(set)return(-1); set=1;
		fcn=pdglGetProcAddress("glVertexAttrib1fv");
		if(!fcn)return(-1);
	}

	fcn(index, v);
	return(0);
}

PDGL_API int pdglVertexAttrib2fv(int index, float *v)
{
	static PFNGLVERTEXATTRIB2FVPROC fcn=NULL;
	static int set=0;

	if(!fcn)
	{
		if(set)return(-1); set=1;
		fcn=pdglGetProcAddress("glVertexAttrib2fv");
		if(!fcn)return(-1);
	}

	fcn(index, v);
	return(0);
}

PDGL_API int pdglVertexAttrib3fv(int index, float *v)
{
	static PFNGLVERTEXATTRIB3FVPROC fcn=NULL;
	static int set=0;

	if(!fcn)
	{
		if(set)return(-1); set=1;
		fcn=pdglGetProcAddress("glVertexAttrib3fv");
		if(!fcn)return(-1);
	}

	fcn(index, v);
	return(0);
}

PDGL_API int pdglVertexAttrib4fv(int index, float *v)
{
	static PFNGLVERTEXATTRIB4FVPROC fcn=NULL;
	static int set=0;

	if(!fcn)
	{
		if(set)return(-1); set=1;
		fcn=pdglGetProcAddress("glVertexAttrib4fv");
		if(!fcn)return(-1);
	}

	fcn(index, v);
	return(0);
}



PDGL_API int pdglGetAttribLocation(int program, char *name)
{
	static PFNGLGETATTRIBLOCATIONPROC fcn=NULL;
	static int set=0;

	if(!fcn)
	{
		if(set)return(-1); set=1;
		fcn=pdglGetProcAddress("glGetAttribLocation");
		if(!fcn)return(-1);
	}

	return(fcn(program, name));
}

PDGL_API int PDGL_GetAttribLocation(char *name)
{
	return(pdglGetAttribLocation(pdgl_shader_cur, name));
}

PDGL_API int pdglVertexAttribPointer(
	int index, int size, int type, int normalized, int stride,
	void *pointer)
{
	if(!pglVertexAttribPointer)return(-1);
	pglVertexAttribPointer(index, size, type,
		normalized, stride, pointer);
	return(0);
}

PDGL_API int pdglEnableVertexAttribArray(int idx)
{
	static PFNGLENABLEVERTEXATTRIBARRAYPROC fcn=NULL;
	static int set=0;

	if(!fcn)
	{
		if(set)return(-1); set=1;
		fcn=pdglGetProcAddress("glEnableVertexAttribArray");
		if(!fcn)return(-1);
	}

	fcn(idx);
}

PDGL_API int pdglDisableVertexAttribArray(int idx)
{
	static PFNGLDISABLEVERTEXATTRIBARRAYPROC fcn=NULL;
	static int set=0;

	if(!fcn)
	{
		if(set)return(-1); set=1;
		fcn=pdglGetProcAddress("glDisableVertexAttribArray");
		if(!fcn)return(-1);
	}

	fcn(idx);
}

PDGL_API int pdglActiveTexture(int idx)
{
	static PFNGLACTIVETEXTUREPROC fcn=NULL;
	static int set=0;

	if(!fcn)
	{
		if(set)return(-1); set=1;
		fcn=pdglGetProcAddress("glActiveTexture");
		if(!fcn)return(-1);
	}

	fcn(GL_TEXTURE0+idx);
}

#ifndef _WIN32
  typedef void (APIENTRYP PFNGLMULTITEXCOORD1FPROC) (
	GLenum target, GLfloat s);
  typedef void (APIENTRYP PFNGLMULTITEXCOORD2FPROC) (
	GLenum target, GLfloat s, GLfloat t);
  typedef void (APIENTRYP PFNGLMULTITEXCOORD3FPROC) (
	GLenum target, GLfloat s, GLfloat t, GLfloat r);
  typedef void (APIENTRYP PFNGLMULTITEXCOORD4FPROC) (
	GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
  typedef void (APIENTRYP PFNGLMULTITEXCOORD1FVPROC) (
	GLenum target, const GLfloat *v);
  typedef void (APIENTRYP PFNGLMULTITEXCOORD2FVPROC) (
	GLenum target, const GLfloat *v);
  typedef void (APIENTRYP PFNGLMULTITEXCOORD3FVPROC) (
	GLenum target, const GLfloat *v);
  typedef void (APIENTRYP PFNGLMULTITEXCOORD4FVPROC) (
	GLenum target, const GLfloat *v);
#endif

PDGL_API int pdglMultiTexCoord1f(int idx, float s)
{
	static PFNGLMULTITEXCOORD1FPROC fcn=NULL;
	static int set=0;

	if(!fcn)
	{
		if(set)return(-1); set=1;
		fcn=pdglGetProcAddress("glMultiTexCoord1f");
		if(!fcn)return(-1);
	}

	fcn(GL_TEXTURE0+idx, s);
	return(0);
}

PDGL_API int pdglMultiTexCoord2f(int idx, float s, float t)
{
	static PFNGLMULTITEXCOORD2FPROC fcn=NULL;
	static int set=0;

	if(!fcn)
	{
		if(set)return(-1); set=1;
		fcn=pdglGetProcAddress("glMultiTexCoord2f");
		if(!fcn)return(-1);
	}

	fcn(GL_TEXTURE0+idx, s, t);
	return(0);
}

PDGL_API int pdglMultiTexCoord3f(int idx, float s, float t, float p)
{
	static PFNGLMULTITEXCOORD3FPROC fcn=NULL;
	static int set=0;

	if(!fcn)
	{
		if(set)return(-1); set=1;
		fcn=pdglGetProcAddress("glMultiTexCoord3f");
		if(!fcn)return(-1);
	}

	fcn(GL_TEXTURE0+idx, s, t, p);
	return(0);
}

PDGL_API int pdglMultiTexCoord4f(int idx, float s, float t, float p, float q)
{
	static PFNGLMULTITEXCOORD4FPROC fcn=NULL;
	static int set=0;

	if(!fcn)
	{
		if(set)return(-1); set=1;
		fcn=pdglGetProcAddress("glMultiTexCoord4f");
		if(!fcn)return(-1);
	}

	fcn(GL_TEXTURE0+idx, s, t, p, q);
	return(0);
}


PDGL_API int pdglMultiTexCoord1fv(int idx, float *v)
{
	static PFNGLMULTITEXCOORD1FVPROC fcn=NULL;
	static int set=0;

	if(!fcn)
	{
		if(set)return(-1); set=1;
		fcn=pdglGetProcAddress("glMultiTexCoord1fv");
		if(!fcn)return(-1);
	}

	fcn(GL_TEXTURE0+idx, v);
	return(0);
}

PDGL_API int pdglMultiTexCoord2fv(int idx, float *v)
{
	static PFNGLMULTITEXCOORD2FVPROC fcn=NULL;
	static int set=0;

	if(!fcn)
	{
		if(set)return(-1); set=1;
		fcn=pdglGetProcAddress("glMultiTexCoord2fv");
		if(!fcn)return(-1);
	}

	fcn(GL_TEXTURE0+idx, v);
	return(0);
}

PDGL_API int pdglMultiTexCoord3fv(int idx, float *v)
{
	static PFNGLMULTITEXCOORD3FVPROC fcn=NULL;
	static int set=0;

	if(!fcn)
	{
		if(set)return(-1); set=1;
		fcn=pdglGetProcAddress("glMultiTexCoord3fv");
		if(!fcn)return(-1);
	}

	fcn(GL_TEXTURE0+idx, v);
	return(0);
}

PDGL_API int pdglMultiTexCoord4fv(int idx, float *v)
{
	static PFNGLMULTITEXCOORD4FVPROC fcn=NULL;
	static int set=0;

	if(!fcn)
	{
		if(set)return(-1); set=1;
		fcn=pdglGetProcAddress("glMultiTexCoord4fv");
		if(!fcn)return(-1);
	}

	fcn(GL_TEXTURE0+idx, v);
	return(0);
}

PDGL_API int pdglBeginQuery(int tgt, int id)
{
	static PFNGLBEGINQUERYPROC fcn=NULL;
	static int set=0;

	if(!fcn)
	{
		if(set)return(-1); set=1;
		fcn=pdglGetProcAddress("glBeginQuery");
		if(!fcn)return(-1);
	}

	fcn(tgt, id);
	return(0);
}

PDGL_API int pdglEndQuery(int tgt)
{
	static PFNGLENDQUERYPROC fcn=NULL;
	static int set=0;

	if(!fcn)
	{
		if(set)return(-1); set=1;
		fcn=pdglGetProcAddress("glEndQuery");
		if(!fcn)return(-1);
	}

	fcn(tgt);
	return(0);
}

PDGL_API int pdglGetQueryObjectiv(int id, int pname, int *params)
{
	static PFNGLGETQUERYOBJECTIVPROC fcn=NULL;
	static int set=0;

	if(!fcn)
	{
		if(set)return(-1); set=1;
		fcn=pdglGetProcAddress("glGetQueryObjectiv");
		if(!fcn)return(-1);
	}

	fcn(id, pname, params);
	return(0);
}

PDGL_API int pdglGetQueryObjectuiv(int id, int pname, uint *params)
{
	static PFNGLGETQUERYOBJECTUIVPROC fcn=NULL;
	static int set=0;

	if(!fcn)
	{
		if(set)return(-1); set=1;
		fcn=pdglGetProcAddress("glGetQueryObjectuiv");
		if(!fcn)return(-1);
	}

	fcn(id, pname, params);
	return(0);
}


PDGL_API int pdglGenFramebuffers(int n, int *ids)
{
	static PFNGLGENFRAMEBUFFERSEXTPROC fcn=NULL;
	static int set=0;

	if(!fcn)
	{
		if(set)return(-1); set=1;
		fcn=pdglGetProcAddress("glGenFramebuffersEXT");
		if(!fcn)return(-1);
	}

	fcn(n, ids);
	return(0);
}

PDGL_API int pdglGenRenderbuffers(int n, int *ids)
{
	static PFNGLGENRENDERBUFFERSEXTPROC fcn=NULL;
	static int set=0;

	if(!fcn)
	{
		if(set)return(-1); set=1;
		fcn=pdglGetProcAddress("glGenRenderbuffersEXT");
		if(!fcn)return(-1);
	}

	fcn(n, ids);
	return(0);
}

PDGL_API int pdglBindFramebuffer(int target, int framebuffer)
{
	static PFNGLBINDFRAMEBUFFEREXTPROC fcn=NULL;
	static int set=0;

	if(!fcn)
	{
		if(set)return(-1); set=1;
		fcn=pdglGetProcAddress("glBindFramebufferEXT");
		if(!fcn)return(-1);
	}

	fcn(target, framebuffer);
	return(0);
}

PDGL_API int pdglBindRenderbuffer(int target, int framebuffer)
{
	static PFNGLBINDRENDERBUFFEREXTPROC fcn=NULL;
	static int set=0;

	if(!fcn)
	{
		if(set)return(-1); set=1;
		fcn=pdglGetProcAddress("glBindRenderbufferEXT");
		if(!fcn)return(-1);
	}

	fcn(target, framebuffer);
	return(0);
}

PDGL_API int pdglRenderbufferStorage(int target,
	int internalformat, int width, int height)
{
	static PFNGLRENDERBUFFERSTORAGEEXTPROC fcn=NULL;
	static int set=0;

	if(!fcn)
	{
		if(set)return(-1); set=1;
		fcn=pdglGetProcAddress("glRenderbufferStorageEXT");
		if(!fcn)return(-1);
	}

	fcn(target, internalformat, width, height);
	return(0);
}

PDGL_API int pdglFramebufferTexture2D(
	int target, int attachment, int textarget, int texture, int level)
{
	static PFNGLFRAMEBUFFERTEXTURE2DEXTPROC fcn=NULL;
	static int set=0;

	if(!fcn)
	{
		if(set)return(-1); set=1;
		fcn=pdglGetProcAddress("glFramebufferTexture2DEXT");
		if(!fcn)return(-1);
	}

	fcn(target, attachment, textarget, texture, level);
	return(0);
}

PDGL_API int pdglFramebufferRenderbuffer(
	int target, int attachment, int textarget, int texture)
{
	static PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC fcn=NULL;
	static int set=0;

	if(!fcn)
	{
		if(set)return(-1); set=1;
		fcn=pdglGetProcAddress("glFramebufferRenderbufferEXT");
		if(!fcn)return(-1);
	}

	fcn(target, attachment, textarget, texture);
	return(0);
}

PDGL_API int pdglCheckFramebufferStatus(int target)
{
	static PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC fcn=NULL;
	static int set=0;
	int i;

	if(!fcn)
	{
		if(set)return(-1); set=1;
		fcn=pdglGetProcAddress("glCheckFramebufferStatusEXT");
		if(!fcn)return(-1);
	}

	i=fcn(target);
	return(i);
}

PDGL_API int pdglDrawBuffers(int n, int *ids)
{
	static PFNGLDRAWBUFFERSPROC fcn=NULL;
	static int set=0;

	if(!fcn)
	{
		if(set)return(-1); set=1;
		fcn=pdglGetProcAddress("glDrawBuffers");
		if(!fcn)return(-1);
	}

	fcn(n, ids);
	return(0);
}


PDGL_API int pdglGenBuffers(int n, int *ids)
{
	static PFNGLGENBUFFERSPROC fcn=NULL;
	static int set=0;
	int i;

	if(!fcn)
	{
		if(set)
		{
			for(i=0; i<n; i++)
				ids[i]=0;
			return(-1);
		}
		set=1;
		fcn=pdglGetProcAddress("glGenBuffers");
		if(!fcn)
		{
			for(i=0; i<n; i++)
				ids[i]=0;
			return(-1);
		}
	}

	fcn(n, ids);
	return(0);
}

PDGL_API int pdglDeleteBuffers(int n, int *ids)
{
	static PFNGLGENBUFFERSPROC fcn=NULL;
	static int set=0;

	if(!fcn)
	{
		if(set)return(-1); set=1;
		fcn=pdglGetProcAddress("glDeleteBuffers");
		if(!fcn)return(-1);
	}

	fcn(n, ids);
	return(0);
}

PDGL_API int pdglBindBuffer(int target, int buffer)
{
	static PFNGLBINDBUFFERPROC fcn=NULL;
	static int set=0;

	if(!fcn)
	{
		if(set)
		{
			*(int *)-1=-1;
			return(-1);
		}
		set=1;
		fcn=pdglGetProcAddress("glBindBuffer");
		if(!fcn)
		{
			*(int *)-1=-1;
			return(-1);
		}
	}

	fcn(target, buffer);
	return(0);
}

PDGL_API int pdglBufferData(
	int target, int size,
	const void *data, int usage)
{
	static PFNGLBUFFERDATAPROC fcn=NULL;
	static int set=0;

	if(!fcn)
	{
		if(set)return(-1); set=1;
		fcn=pdglGetProcAddress("glBufferData");
		if(!fcn)return(-1);
	}

	fcn(target, size, data, usage);
	return(0);
}

PDGL_API int pdglBufferSubData(
	int target, int offset, int size,
	const void *data)
{
	static PFNGLBUFFERSUBDATAPROC fcn=NULL;
	static int set=0;

	if(!fcn)
	{
		if(set)return(-1); set=1;
		fcn=pdglGetProcAddress("glBufferSubData");
		if(!fcn)return(-1);
	}

	fcn(target, offset, size, data);
	return(0);
}

PDGL_API void *pdglMapBuffer(int target, int access)
{
	static PFNGLMAPBUFFERPROC fcn=NULL;
	static int set=0;

	if(!fcn)
	{
		if(set)return(NULL); set=1;
		fcn=pdglGetProcAddress("glMapBuffer");
		if(!fcn)return(NULL);
	}

	return(fcn(target, access));
}

PDGL_API bool pdglUnmapBuffer(int target)
{
	static PFNGLUNMAPBUFFERPROC fcn=NULL;
	static int set=0;

	if(!fcn)
	{
		if(set)return(false); set=1;
		fcn=pdglGetProcAddress("glUnmapBuffer");
		if(!fcn)return(false);
	}

	return(fcn(target));
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
	static PFNGLCOMPRESSEDTEXIMAGE2DPROC fcn=NULL;
	static int set=0;

	if(!fcn)
	{
		if(set)return(-1); set=1;
		fcn=pdglGetProcAddress("glCompressedTexImage2D");
		if(!fcn)return(-1);
	}

	fcn(target, level, internalformat, width, height,
		border, imageSize, data);
	return(0);
}

#endif
