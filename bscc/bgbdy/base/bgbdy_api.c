#include <bgbgc.h>
#include <bgbdy.h>

#include <math.h>
// #include <complex.h>

BGBDY_API void dy_init(void *base)
{
	static int init=0;

	gc_init(base);

	if(init)return;
	init=1;

//	BGBGC_Init();
//	BGBGC_StackBase(base);
	BGBGC_InitDyLL();

	BGBDY_InitTypes();
//	BGBDY_InitFrames();
	BGBDY_InitArrayTypes();
	BGBDY_InitStringTypes();

	dyopr_init();
}

int dy_init2_i(int (*fcn)())
{
//	BGBGC_Init();
//	BGBGC_StackBase(base);
	BGBGC_InitDyLL();

	BGBDY_InitTypes();
//	BGBDY_InitFrames();

	dyopr_init();

	fcn();

	gc_deinit();
	return(0);
}

BGBDY_API void dy_init2(int (*fcn)())
{
	gc_init2(dy_init2_i, fcn);
}

BGBDY_API void dyPuts(char *str)
{
	gc_puts(str);
}

BGBDY_API void dyPrintf(char *str, ...)
{
	char buf[65536];
	va_list lst;
	int i;

	va_start(lst, str);
	vsprintf(buf, str, lst);
	va_end(lst);

	gc_puts(buf);
}

BGBDY_API void dyDPuts(char *str)
{
	gc_logputs(str);
}

BGBDY_API void dyDPrintf(char *str, ...)
{
	char buf[65536];
	va_list lst;
	int i;

	va_start(lst, str);
	vsprintf(buf, str, lst);
	va_end(lst);

	gc_logputs(buf);
}

BGBDY_API void dyAddLogHook(void (*fcn)(char *str))
	{ gcAddLogHook(fcn); }

BGBDY_API void dyAddTypeHook(char *(*fcn)(void *p))
	{ gcAddTypeHook(fcn); }

BGBDY_API char *dygettype(dyt p)
{
	char *ty;

	if(!p)return(NULL);

	ty=gcCheckPtrTypeCache((void *)p);
	if(ty)return(ty);

	ty=BGBDY_GetVType((byte *)p);
	if(ty)return(ty);

//	ty=BGBDY_GetTypeName(p);
	ty=gcgettype(p);
	return(ty);

//	ty=gcgettype(p);
//	if(ty)return(ty);
//	ty=BGBDY_GetVType(p);
//	if(ty)return(ty);
//	return(NULL);
}

BGBDY_API int dytypep(dyt p, char *ty)
{
	char *ty1;

	ty1=dygettype(p);

	if(!ty && !ty1)return(1);
	if(!ty || !ty1)return(0);

	if(!strcmp(ty1, ty))return(1);
	return(0);
}

BGBDY_API dyt dybool(int v)
{
	return(v?BGBDY_TRUE:BGBDY_FALSE);
}

BGBDY_API dyt dyint(int v)
{
	return(BGBDY_WrapInt(v));
}

BGBDY_API dyt dylong(s64 v)
{
	return(BGBDY_WrapInt64(v));
}

BGBDY_API dyt dylonglong(s64 v)
{
	return(BGBDY_WrapInt64(v));
}

BGBDY_API dyt dyfixint(int v)
{
	return(BGBDY_TryWrapInt64(v));
}

BGBDY_API dyt dyfixlong(s64 v)
{
	return(BGBDY_TryWrapInt64(v));
}

BGBDY_API dyt dychar(int v)
{
	return(BGBDY_WrapChar(v));
}

BGBDY_API dyt dyflonum(float v)
{
	return(BGBDY_WrapFlonum(v));
//	return(BGBDY_WrapFloat(v));
}

BGBDY_API dyt dyfloat(float v)
{
//	return(BGBDY_WrapFlonum(v));
	return(BGBDY_WrapFloat(v));
}

BGBDY_API dyt dydouble(double v)
{
	return(BGBDY_WrapDouble(v));
}

BGBDY_API dyt dyfcomplex(fcomplex v)
{
	dyt t;
//	t=BGBDY_New("_complex_t", sizeof(dcomplex));
//	BGBGC_MarkAtomic(t);
	t=gctatomic("_complex_t", sizeof(dcomplex));
	((dcomplex *)t)->r=v.r;
	((dcomplex *)t)->i=v.i;
	return(t);
}

BGBDY_API dyt dydcomplex(dcomplex v)
{
	dyt t;
//	t=BGBDY_New("_complex_t", sizeof(dcomplex));
//	BGBGC_MarkAtomic(t);
	t=gctatomic("_complex_t", sizeof(dcomplex));
	*((dcomplex *)t)=v;
	return(t);
}

BGBDY_API dyt dycomplex2(double r, double i)
{
	dyt t;
//	t=BGBDY_New("_complex_t", sizeof(dcomplex));
//	BGBGC_MarkAtomic(t);
	t=gctatomic("_complex_t", sizeof(dcomplex));
	((dcomplex *)t)->r=r;
	((dcomplex *)t)->i=i;
	return(t);
}

//BGBDY_API char *dystring(char *s)
BGBDY_API dyt dystring(char *s)
{
	if(!s)return(NULL);
	return(BGBDY_HashString(s));
}

BGBDY_API char *dystrdup(char *s)
{
	return((char *)(dystring(s)));
}

BGBDY_API char *dystrsym(char *s)
{
	return((char *)(dysymbol(s)));
}

BGBDY_API char *dystrkey(char *s)
{
	return((char *)(dykeyword(s)));
}

//BGBDY_API char *dysymbol(char *s)
BGBDY_API dyt dysymbol(char *s)
{
	if(!s)return(NULL);
	return(BGBDY_WrapSymbol(s));
}

//BGBDY_API char *dykeyword(char *s)
BGBDY_API dyt dykeyword(char *s)
{
	if(!s)return(NULL);
	return(BGBDY_WrapKeyword(s));
}

BGBDY_API dyt dysym(char *s)
{
	if(!s)return(NULL);
	return(BGBDY_WrapSymbol(s));
}

BGBDY_API dyt dyksym(char *s)
{
	if(!s)return(NULL);
	return(BGBDY_WrapKeyword(s));
}

BGBDY_API dyt dyFixedSymbol(char *s)
{
//	if(!s)return(NULL);
	return(BGBDY_WrapFixedSymbol(s));
}

//BGBDY_API u16 *dywstring(u16 *s)
BGBDY_API dyt dywstring(u16 *s)
{
//	u16 *t;
//	int i;

//	for(i=0; s[i]; i++);
//	t=BGBDY_New("_wstring_t", (i+1)*sizeof(u16));
//	BGBGC_MarkAtomic(t);
//	for(i=0; s[i]; i++)t[i]=s[i];
//	t[i]=0;
//	return(t);

	if(!s)return(NULL);
	return(BGBDY_HashString16(s));
}

//BGBDY_API u16 *dywstring2(char *str)
BGBDY_API dyt dywstring2(char *str)
{
	char *s;
	u16 *t, *b;
	int i, j;

	i=strlen(str);
	b=gcralloc((i+1)*sizeof(u16));

	s=str; t=b;
	while(*s)
	{
		i=BGBGC_ParseChar(&s);
		BGBGC_EmitChar16(&t, i);
	}
	*t++=0;

	return(dywstring(b));
}

BGBDY_API dyt dyustring(char *s)
{
	if(!s)return(NULL);
	return(BGBDY_WrapUString(s));
}

BGBDY_API dyt dyusym(char *s)
{
	if(!s)return(NULL);
	return(BGBDY_WrapUSymbol(s));
}


BGBDY_API int dyboolp(dyt p)
{
	if(p==BGBDY_TRUE)return(1);
	if(p==BGBDY_FALSE)return(1);
	return(0);
}

BGBDY_API int dyfixintp(dyt p)
{
//	return(dytypep(p, "_fixint_t"));
	return(BGBDY_IsFixnumFP((byte *)p));
}

BGBDY_API int dyintp(dyt p)
{
	char *ty;
	if(dyfixintp(p))return(1);

	ty=dygettype(p);
	if(!ty)return(0);
	if(!strcmp(ty, "_int_t"))return(1);
	if(!strcmp(ty, "_char_t"))return(1);
	return(0);

//	return(dytypep(p, "_int_t"));
//	return(dytypep(p, "_fixint_t") || dytypep(p, "_int_t"));
}

BGBDY_API int dylongp(dyt p)
{
	char *ty;
	if(dyfixintp(p))return(1);
	
	ty=dygettype(p);
	if(!ty)return(0);
	if(!strcmp(ty, "_int_t"))return(1);
	if(!strcmp(ty, "_long_t"))return(1);
	if(!strcmp(ty, "_char_t"))return(1);
//	if(!strcmp(ty, "_int128_t"))return(1);
	return(0);
}

BGBDY_API int dylonglongp(dyt p)
{
	char *ty;
	if(dyfixintp(p))return(1);
	
	ty=dygettype(p);
	if(!ty)return(0);
	if(!strcmp(ty, "_int_t"))return(1);
	if(!strcmp(ty, "_long_t"))return(1);
	if(!strcmp(ty, "_char_t"))return(1);
//	if(!strcmp(ty, "_int128_t"))return(1);
	return(0);
}

BGBDY_API int dylongx2p(dyt p)
{
	char *ty;
	if(dyfixintp(p))return(1);
	
	ty=dygettype(p);
	if(!ty)return(0);
	if(!strcmp(ty, "_int_t"))return(1);
	if(!strcmp(ty, "_long_t"))return(1);
	if(!strcmp(ty, "_char_t"))return(1);
	if(!strcmp(ty, "_int128_t"))return(1);
	return(0);
}

BGBDY_API int dycharp(dyt p)
{
	return(dytypep(p, "_char_t"));
//	return(BGBDY_IsFixnumFP((byte *)p));
}

BGBDY_API int dyflonump(dyt p)
{
//	return(dytypep(p, "_flonum_t"));
	return(BGBDY_IsFlonumFP((byte *)p));
}

BGBDY_API int dyfloatp(dyt p)
{
	if(dyflonump(p))return(1);
	return(dytypep(p, "_float_t"));
//	return(dytypep(p, "_float_t") || dytypep(p, "_flonum_t"));
}

BGBDY_API int dydoublep(dyt p)
{
//	return(dytypep(p, "_float_t"));
	return(dytypep(p, "_double_t"));
}

BGBDY_API int dyfixrealp(dyt p)
{
	return(BGBDY_IsFixRealFP((byte *)p));
}

BGBDY_API int dyrealp(dyt p)
{
	char *ty;

	if(dyfixintp(p))return(1);
	if(dyflonump(p))return(1);

	ty=dygettype(p);
	if(!ty)return(0);

	if(!strcmp(ty, "_int_t"))return(1);
	if(!strcmp(ty, "_long_t"))return(1);
	if(!strcmp(ty, "_float_t"))return(1);
	if(!strcmp(ty, "_double_t"))return(1);
	if(!strcmp(ty, "_char_t"))return(1);
//	if(!strcmp(ty, "_int128_t"))return(1);
//	if(!strcmp(ty, "_float128_t"))return(1);
	return(0);
}

BGBDY_API int dyrealx2p(dyt p)
{
	char *ty;

	if(dyfixintp(p))return(1);
	if(dyflonump(p))return(1);

	ty=dygettype(p);
	if(!ty)return(0);

	if(!strcmp(ty, "_int_t"))return(1);
	if(!strcmp(ty, "_long_t"))return(1);
	if(!strcmp(ty, "_float_t"))return(1);
	if(!strcmp(ty, "_double_t"))return(1);
	if(!strcmp(ty, "_char_t"))return(1);
	if(!strcmp(ty, "_int128_t"))return(1);
	if(!strcmp(ty, "_float128_t"))return(1);
	return(0);
}

BGBDY_API int dyfcomplexp(dyt p)
{
	return(dytypep(p, "_complex_t"));
}

BGBDY_API int dydcomplexp(dyt p)
{
	return(dytypep(p, "_complex_t"));
}

BGBDY_API int dycomplexp(dyt p)
{
	return(dytypep(p, "_complex_t"));
}

BGBDY_API int dySmallComplexp(dyt p)
{
	return(dyrealp(p) || dycomplexp(p));
}

BGBDY_API int dySmallQuatp(dyt p)
{
	return(dyrealp(p) || dycomplexp(p) || dyquatp(p));
}

BGBDY_API int dystringp(dyt p)
{
	return(dytypep(p, "_string_t"));
}

BGBDY_API int dysymbolp(dyt p)
{
	return(dytypep(p, "_symbol_t"));
}

BGBDY_API int dykeywordp(dyt p)
{
	return(dytypep(p, "_keyword_t"));
}

BGBDY_API int dywstringp(dyt p)
{
	return(dytypep(p, "_wstring_t"));
}

BGBDY_API int dyustringp(dyt p)
{
	return(dytypep(p, "_ustring_t"));
}

BGBDY_API int dyusymp(dyt p)
{
	return(dytypep(p, "_usymbol_t"));
}

BGBDY_API char *dystringv(dyt p)
{
	u16 *s;
	char *t, *b;
	int i, j;

	if(dytypep(p, "_string_t"))
		return((char *)p);
	if(dytypep(p, "_symbol_t"))
		return((char *)p);
	if(dytypep(p, "_keyword_t"))
		return((char *)p);
	if(dytypep(p, "_wstring_t"))
	{
		s=(u16 *)p; for(i=0; s[i]; i++);
//		b=gctatomic("_string_t", (i+1)*sizeof(char));
		b=gcralloc((i+1)*6);

//		t=b; for(i=0; s[i]; i++)BGBGC_EmitChar(&t, s[i]);

		t=b;
		while(*s)
		{
			i=BGBGC_ParseChar16(&s);
			BGBGC_EmitChar(&t, i);
		}

		*t++=0;
		return(dystrdup(b));
	}

	if(dytypep(p, "_ustring_t"))return(BGBDY_UnwrapUString(p));
	if(dytypep(p, "_usymbol_t"))return(BGBDY_UnwrapUSymbol(p));
	return(NULL);
}

BGBDY_API char *dysymbolv(dyt p)
{
	return(dystringv(p));
}

BGBDY_API char *dysymv(dyt p)
{
	return(dystringv(p));
}

BGBDY_API u16 *dywstringv(dyt p)
{
	char *s;
	u16 *t, *b;
	int i, j;

	if(dytypep(p, "_wstring_t"))
		return((u16 *)p);

	if(dytypep(p, "_string_t") ||
		dytypep(p, "_symbol_t"))
	{
		s=(char *)p;

		i=strlen(s);
		b=gcralloc((i+1)*sizeof(u16));

		t=b;
		while(*s)
		{
			i=BGBGC_ParseChar(&s);
			BGBGC_EmitChar16(&t, i);
		}
		*t++=0;

		return((u16 *)dywstring(b));
	}

	if(dytypep(p, "_ustring_t"))return((u16 *)dywstring2(BGBDY_UnwrapUString(p)));
	if(dytypep(p, "_usymbol_t"))return((u16 *)dywstring2(BGBDY_UnwrapUSymbol(p)));

	return(NULL);
}


BGBDY_API int dyintv(dyt p)
{
	return(BGBDY_UnwrapInt(p));
}

BGBDY_API s64 dylongv(dyt p)
{
	return(BGBDY_UnwrapInt64(p));
}

BGBDY_API s64 dylonglongv(dyt p)
{
	return(BGBDY_UnwrapInt64(p));
}

BGBDY_API float dyfloatv(dyt p)
{
	return(BGBDY_UnwrapFloat(p));
}

BGBDY_API double dydoublev(dyt p)
{
	return(BGBDY_UnwrapFloat(p));
}


BGBDY_API double dyrealv(dyt p)
{
	return(BGBDY_UnwrapFloat(p));
}

BGBDY_API dcomplex dycomplexv(dyt p)
{
	if(dycomplexp(p))
		return(*((dcomplex *)p));
	return(dtyDComplex(dyrealv(p), 0));
}

BGBDY_API fcomplex dyfcomplexv(dyt p)
{
	fcomplex t;
	if(dycomplexp(p))
	{
		t.r=((dcomplex *)p)->r;
		t.i=((dcomplex *)p)->i;
		return(t);
	}
	t.r=dyrealv(p); t.i=0;
	return(t);
}

BGBDY_API dcomplex dydcomplexv(dyt p)
{
	if(dycomplexp(p))
		return(*((dcomplex *)p));
	return(dtyDComplex(dyrealv(p), 0));
}

BGBDY_API double dycreal(dyt p)
{
	if(dycomplexp(p))return(((dcomplex *)p)->r);
	return(dyrealv(p));
}

BGBDY_API double dycimag(dyt p)
{
	if(dycomplexp(p))return(((dcomplex *)p)->i);
	return(dyrealv(p));
}


BGBDY_API dyt dyvec2(float x, float y)
{
	dyt p;
	float *fa;

//	p=gctatomic("_vec2_t", 4*sizeof(float));
	p=(dyt)BGBDY_AllocVec2();
	fa=(float *)p;
	fa[0]=x; fa[1]=y; fa[2]=0; fa[3]=0;

	return(p);
}

BGBDY_API dyt dyvec3(float x, float y, float z)
{
	dyt p;
	float *fa;

//	p=gctatomic("_vec3_t", 4*sizeof(float));
	p=(dyt)BGBDY_AllocVec3();
	fa=(float *)p;
	fa[0]=x; fa[1]=y; fa[2]=z; fa[3]=0;

	return(p);
}

BGBDY_API dyt dyvec4(float x, float y, float z, float w)
{
	dyt p;
	float *fa;

//	p=gctatomic("_vec4_t", 4*sizeof(float));
	p=(dyt)BGBDY_AllocVec4();
	fa=(float *)p;
	fa[0]=x; fa[1]=y; fa[2]=z; fa[3]=w;

	return(p);
}

BGBDY_API dyt dyquat(float w, float x, float y, float z)
{
	dyt p;
	float *fa;

//	p=gctatomic("_quat_t", 4*sizeof(float));
	p=(dyt)BGBDY_AllocQuat();
	fa=(float *)p;
	fa[0]=x; fa[1]=y; fa[2]=z; fa[3]=w;

	return(p);
}


BGBDY_API dyt dyvec2d(double x, double y)
{
	double *fa;
	fa=BGBDY_AllocVec2D();
	fa[0]=x; fa[1]=y; fa[2]=0; fa[3]=0;
	return((dyt)fa);
}

BGBDY_API dyt dyvec3d(double x, double y, double z)
{
	double *fa;
	fa=BGBDY_AllocVec3D();
	fa[0]=x; fa[1]=y; fa[2]=z; fa[3]=0;
	return((dyt)fa);
}

BGBDY_API dyt dyvec4d(double x, double y, double z, double w)
{
	double *fa;
	fa=BGBDY_AllocVec4D();
	fa[0]=x; fa[1]=y; fa[2]=z; fa[3]=w;
	return((dyt)fa);
}

BGBDY_API dyt dyquatd(double w, double x, double y, double z)
{
	double *fa;
	fa=BGBDY_AllocQuatD();
	fa[0]=x; fa[1]=y; fa[2]=z; fa[3]=w;
	return((dyt)fa);
}


BGBDY_API dyt dyvec2fv(float *v)
	{ return(dyvec2(v[0], v[1])); }
BGBDY_API dyt dyvec3fv(float *v)
	{ return(dyvec3(v[0], v[1], v[2])); }
BGBDY_API dyt dyvec4fv(float *v)
	{ return(dyvec4(v[0], v[1], v[2], v[3])); }
BGBDY_API dyt dyquatfv(float *v)
	{ return(dyquat(v[0], v[1], v[2], v[3])); }

BGBDY_API dyt dyvec2dv(double *v)
	{ return(dyvec2d(v[0], v[1])); }
BGBDY_API dyt dyvec3dv(double *v)
	{ return(dyvec3d(v[0], v[1], v[2])); }
BGBDY_API dyt dyvec4dv(double *v)
	{ return(dyvec4d(v[0], v[1], v[2], v[3])); }
BGBDY_API dyt dyquatdv(double *v)
	{ return(dyquatd(v[3], v[0], v[1], v[2])); }

BGBDY_API dyt dywvec2(vec2 v)
	{ return(dyvec2(v2x(v), v2y(v))); }
BGBDY_API dyt dywvec3(vec3 v)
	{ return(dyvec3(v3x(v), v3y(v), v3z(v))); }
BGBDY_API dyt dywvec4(vec4 v)
	{ return(dyvec4(v4x(v), v4y(v), v4z(v), v4w(v))); }
BGBDY_API dyt dywquat(quat v)
	{ return(dyquat(qw(v), qx(v), qy(v), qz(v))); }

BGBDY_API dyt dywvec2d(vec2d v)
		{ return(dyvec2d(v2dx(v), v2dy(v))); }
BGBDY_API dyt dywvec3d(vec3d v)
	{ return(dyvec3d(v3dx(v), v3dy(v), v3dz(v))); }
BGBDY_API dyt dywvec4d(vec4d v)
	{ return(dyvec4d(v4dx(v), v4dy(v), v4dz(v), v4dw(v))); }
BGBDY_API dyt dywquatd(quatd v)
	{ return(dyquatd(qdw(v), qdx(v), qdy(v), qdz(v))); }

BGBDY_API int dyvec2p(dyt p)	{ return(dytypep(p, "_vec2_t")); }
BGBDY_API int dyvec3p(dyt p)	{ return(dytypep(p, "_vec3_t")); }
BGBDY_API int dyvec4p(dyt p)	{ return(dytypep(p, "_vec4_t")); }
BGBDY_API int dyquatp(dyt p)	{ return(dytypep(p, "_quat_t")); }

BGBDY_API int dyvec2dp(dyt p)	{ return(dytypep(p, "_vec2d_t")); }
BGBDY_API int dyvec3dp(dyt p)	{ return(dytypep(p, "_vec3d_t")); }
BGBDY_API int dyvec4dp(dyt p)	{ return(dytypep(p, "_vec4d_t")); }
BGBDY_API int dyquatdp(dyt p)	{ return(dytypep(p, "_quatd_t")); }

BGBDY_API int dyvec2sdp(dyt p)
	{ return(dytypep(p, "_vec2d_t") || dytypep(p, "_vec2_t")); }
BGBDY_API int dyvec3sdp(dyt p)
	{ return(dytypep(p, "_vec3d_t") || dytypep(p, "_vec3_t")); }
BGBDY_API int dyvec4sdp(dyt p)
	{ return(dytypep(p, "_vec4d_t") || dytypep(p, "_vec4_t")); }
BGBDY_API int dyquatsdp(dyt p)
	{ return(dytypep(p, "_quatd_t") || dytypep(p, "_quat_t")); }

BGBDY_API void dyvecv2fv(dyt p, float *v)
{
	if(dytypep(p, "_vec2_t") || dytypep(p, "_vec3_t") ||
		dytypep(p, "_vec4_t") || dytypep(p, "_quat_t"))
		{ v[0]=((float *)p)[0]; v[1]=((float *)p)[1]; return; }
	if(dytypep(p, "_vec2d_t") || dytypep(p, "_vec3d_t") ||
		dytypep(p, "_vec4d_t") || dytypep(p, "_quatd_t"))
		{ v[0]=((double *)p)[0]; v[1]=((double *)p)[1]; return; }
	v[0]=0; v[1]=0;
	return;
}

BGBDY_API void dyvecv3fv(dyt p, float *v)
{
	if(dytypep(p, "_vec2_t"))
		{ v[0]=((float *)p)[0]; v[1]=((float *)p)[1]; v[2]=0; return; }
	if(dytypep(p, "_vec3_t") || dytypep(p, "_vec4_t") ||
			dytypep(p, "_quat_t"))
		{ v[0]=((float *)p)[0]; v[1]=((float *)p)[1];
			v[2]=((float *)p)[2]; return; }
	if(dytypep(p, "_vec2d_t"))
		{ v[0]=((double *)p)[0]; v[1]=((double *)p)[1]; v[2]=0; return; }
	if(dytypep(p, "_vec3d_t") || dytypep(p, "_vec4d_t") ||
			dytypep(p, "_quatd_t"))
		{ v[0]=((double *)p)[0]; v[1]=((double *)p)[1];
			v[2]=((double *)p)[2]; return; }
	v[0]=0; v[1]=0; v[2]=0;
	return;
}

BGBDY_API void dyvecv4fv(dyt p, float *v)
{
	if(dytypep(p, "_vec2_t"))
		{ v[0]=((float *)p)[0]; v[1]=((float *)p)[1];
			v[2]=0; v[3]=0; return; }
	if(dytypep(p, "_vec3_t"))
		{ v[0]=((float *)p)[0]; v[1]=((float *)p)[1];
			v[2]=((float *)p)[2]; v[3]=0; return; }
	if(dytypep(p, "_vec4_t") || dytypep(p, "_quat_t"))
		{ v[0]=((float *)p)[0]; v[1]=((float *)p)[1];
			v[2]=((float *)p)[2]; v[3]=((float *)p)[3]; return; }

	if(dytypep(p, "_vec2d_t"))
		{ v[0]=((double *)p)[0]; v[1]=((double *)p)[1];
			v[2]=0; v[3]=0; return; }
	if(dytypep(p, "_vec3d_t"))
		{ v[0]=((double *)p)[0]; v[1]=((double *)p)[1];
			v[2]=((double *)p)[2]; v[3]=0; return; }
	if(dytypep(p, "_vec4d_t") || dytypep(p, "_quatd_t"))
		{ v[0]=((double *)p)[0]; v[1]=((double *)p)[1];
			v[2]=((double *)p)[2]; v[3]=((double *)p)[3]; return; }

	v[0]=0; v[1]=0; v[2]=0; v[3]=0;
	return;
}

BGBDY_API void dyquatvfv(dyt p, float *v)
	{ dyvecv4fv(p, v); }

BGBDY_API void dyvecv2dv(dyt p, double *v)
{
	if(dytypep(p, "_vec2_t") || dytypep(p, "_vec3_t") ||
		dytypep(p, "_vec4_t") || dytypep(p, "_quat_t"))
		{ v[0]=((float *)p)[0]; v[1]=((float *)p)[1]; return; }
	if(dytypep(p, "_vec2d_t") || dytypep(p, "_vec3d_t") ||
		dytypep(p, "_vec4d_t") || dytypep(p, "_quatd_t"))
		{ v[0]=((double *)p)[0]; v[1]=((double *)p)[1]; return; }
	v[0]=0; v[1]=0;
	return;
}

BGBDY_API void dyvecv3dv(dyt p, double *v)
{
	if(dytypep(p, "_vec2_t"))
		{ v[0]=((float *)p)[0]; v[1]=((float *)p)[1]; v[2]=0; return; }
	if(dytypep(p, "_vec3_t") || dytypep(p, "_vec4_t") ||
			dytypep(p, "_quat_t"))
		{ v[0]=((float *)p)[0]; v[1]=((float *)p)[1];
			v[2]=((float *)p)[2]; return; }
	if(dytypep(p, "_vec2d_t"))
		{ v[0]=((double *)p)[0]; v[1]=((double *)p)[1]; v[2]=0; return; }
	if(dytypep(p, "_vec3d_t") || dytypep(p, "_vec4d_t") ||
			dytypep(p, "_quatd_t"))
		{ v[0]=((double *)p)[0]; v[1]=((double *)p)[1];
			v[2]=((double *)p)[2]; return; }
	v[0]=0; v[1]=0; v[2]=0;
	return;
}

BGBDY_API void dyvecv4dv(dyt p, double *v)
{
	if(dytypep(p, "_vec2_t"))
		{ v[0]=((float *)p)[0]; v[1]=((float *)p)[1];
			v[2]=0; v[3]=0; return; }
	if(dytypep(p, "_vec3_t"))
		{ v[0]=((float *)p)[0]; v[1]=((float *)p)[1];
			v[2]=((float *)p)[2]; v[3]=0; return; }
	if(dytypep(p, "_vec4_t") || dytypep(p, "_quat_t"))
		{ v[0]=((float *)p)[0]; v[1]=((float *)p)[1];
			v[2]=((float *)p)[2]; v[3]=((float *)p)[3]; return; }

	if(dytypep(p, "_vec2d_t"))
		{ v[0]=((double *)p)[0]; v[1]=((double *)p)[1];
			v[2]=0; v[3]=0; return; }
	if(dytypep(p, "_vec3d_t"))
		{ v[0]=((double *)p)[0]; v[1]=((double *)p)[1];
			v[2]=((double *)p)[2]; v[3]=0; return; }
	if(dytypep(p, "_vec4d_t") || dytypep(p, "_quatd_t"))
		{ v[0]=((double *)p)[0]; v[1]=((double *)p)[1];
			v[2]=((double *)p)[2]; v[3]=((double *)p)[3]; return; }

	v[0]=0; v[1]=0; v[2]=0; v[3]=0;
	return;
}

BGBDY_API void dyquatvdv(dyt p, double *v)
	{ dyvecv4dv(p, v); }

BGBDY_API vec2 dyvec2v(dyt p)
{
	double *da;
	float *fa;

	fa=(float *)p;
	if(dytypep(p, "_vec2_t"))return(vec2(fa[0], fa[1]));
	if(dytypep(p, "_vec3_t"))return(vec2(fa[0], fa[1]));
	if(dytypep(p, "_vec4_t"))return(vec2(fa[0], fa[1]));
	if(dytypep(p, "_quat_t"))return(vec2(fa[0], fa[1]));

	da=(double *)p;
	if(dytypep(p, "_vec2d_t"))return(vec2(da[0], da[1]));
	if(dytypep(p, "_vec3d_t"))return(vec2(da[0], da[1]));
	if(dytypep(p, "_vec4d_t"))return(vec2(da[0], da[1]));
	if(dytypep(p, "_quatd_t"))return(vec2(da[0], da[1]));

	return(vec2(0, 0));
}

BGBDY_API vec3 dyvec3v(dyt p)
{
	double *da;
	float *fa;

	fa=(float *)p;
	if(dytypep(p, "_vec2_t"))return(vec3(fa[0], fa[1], 0));
	if(dytypep(p, "_vec3_t"))return(vec3(fa[0], fa[1], fa[2]));
	if(dytypep(p, "_vec4_t"))return(vec3(fa[0], fa[1], fa[2]));
	if(dytypep(p, "_quat_t"))return(vec3(fa[0], fa[1], fa[2]));

	da=(double *)p;
	if(dytypep(p, "_vec2d_t"))return(vec3(da[0], da[1], 0));
	if(dytypep(p, "_vec3d_t"))return(vec3(da[0], da[1], da[2]));
	if(dytypep(p, "_vec4d_t"))return(vec3(da[0], da[1], da[2]));
	if(dytypep(p, "_quatd_t"))return(vec3(da[0], da[1], da[2]));

	return(vec3(0, 0, 0));
}

BGBDY_API vec4 dyvec4v(dyt p)
{
	double *da;
	float *fa;

	fa=(float *)p;
	if(dytypep(p, "_vec2_t"))return(vec4(fa[0], fa[1], 0, 0));
	if(dytypep(p, "_vec3_t"))return(vec4(fa[0], fa[1], fa[2], 0));
	if(dytypep(p, "_vec4_t"))return(vec4(fa[0], fa[1], fa[2], fa[3]));
	if(dytypep(p, "_quat_t"))return(vec4(fa[0], fa[1], fa[2], fa[3]));

	da=(double *)p;
	if(dytypep(p, "_vec2d_t"))return(vec4(da[0], da[1], 0, 0));
	if(dytypep(p, "_vec3d_t"))return(vec4(da[0], da[1], da[2], 0));
	if(dytypep(p, "_vec4d_t"))return(vec4(da[0], da[1], da[2], da[3]));
	if(dytypep(p, "_quatd_t"))return(vec4(da[0], da[1], da[2], da[3]));

	return(vec4(0, 0, 0, 0));
}

BGBDY_API quat dyquatv(dyt p)
{
	double *da;
	float *fa;

	fa=(float *)p;
	if(dytypep(p, "_vec2_t"))return(quat(0, fa[0], fa[1], 0));
	if(dytypep(p, "_vec3_t"))return(quat(0, fa[0], fa[1], fa[2]));
	if(dytypep(p, "_vec4_t"))return(quat(fa[3], fa[0], fa[1], fa[2]));
	if(dytypep(p, "_quat_t"))return(quat(fa[3], fa[0], fa[1], fa[2]));

	da=(double *)p;
	if(dytypep(p, "_vec2d_t"))return(quat(0, da[0], da[1], 0));
	if(dytypep(p, "_vec3d_t"))return(quat(0, da[0], da[1], da[2]));
	if(dytypep(p, "_vec4d_t"))return(quat(da[3], da[0], da[1], da[2]));
	if(dytypep(p, "_quatd_t"))return(quat(da[3], da[0], da[1], da[2]));

	if(dyrealp(p))
	{
		return(quat(dyrealv(p), 0, 0, 0));
	}

	if(dycomplexp(p))
	{
		return(quat(dycreal(p), dycimag(p), 0, 0));
	}
	
	return(quat(0, 0, 0, 0));
}

BGBDY_API vec2d dyvec2vd(dyt p)
{
	double *da;
	float *fa;

	fa=(float *)p;
	if(dytypep(p, "_vec2_t"))return(vec2d(fa[0], fa[1]));
	if(dytypep(p, "_vec3_t"))return(vec2d(fa[0], fa[1]));
	if(dytypep(p, "_vec4_t"))return(vec2d(fa[0], fa[1]));
	if(dytypep(p, "_quat_t"))return(vec2d(fa[0], fa[1]));

	da=(double *)p;
	if(dytypep(p, "_vec2d_t"))return(vec2d(da[0], da[1]));
	if(dytypep(p, "_vec3d_t"))return(vec2d(da[0], da[1]));
	if(dytypep(p, "_vec4d_t"))return(vec2d(da[0], da[1]));
	if(dytypep(p, "_quatd_t"))return(vec2d(da[0], da[1]));

	return(vec2d(0, 0));
}

BGBDY_API vec3d dyvec3vd(dyt p)
{
	double *da;
	float *fa;

	fa=(float *)p;
	if(dytypep(p, "_vec2_t"))return(vec3d(fa[0], fa[1], 0));
	if(dytypep(p, "_vec3_t"))return(vec3d(fa[0], fa[1], fa[2]));
	if(dytypep(p, "_vec4_t"))return(vec3d(fa[0], fa[1], fa[2]));
	if(dytypep(p, "_quat_t"))return(vec3d(fa[0], fa[1], fa[2]));

	da=(double *)p;
	if(dytypep(p, "_vec2d_t"))return(vec3d(da[0], da[1], 0));
	if(dytypep(p, "_vec3d_t"))return(vec3d(da[0], da[1], da[2]));
	if(dytypep(p, "_vec4d_t"))return(vec3d(da[0], da[1], da[2]));
	if(dytypep(p, "_quatd_t"))return(vec3d(da[0], da[1], da[2]));

	return(vec3d(0, 0, 0));
}

BGBDY_API vec4d dyvec4vd(dyt p)
{
	double *da;
	float *fa;

	fa=(float *)p;
	if(dytypep(p, "_vec2_t"))return(vec4d(fa[0], fa[1], 0, 0));
	if(dytypep(p, "_vec3_t"))return(vec4d(fa[0], fa[1], fa[2], 0));
	if(dytypep(p, "_vec4_t"))return(vec4d(fa[0], fa[1], fa[2], fa[3]));
	if(dytypep(p, "_quat_t"))return(vec4d(fa[0], fa[1], fa[2], fa[3]));

	da=(double *)p;
	if(dytypep(p, "_vec2d_t"))return(vec4d(da[0], da[1], 0, 0));
	if(dytypep(p, "_vec3d_t"))return(vec4d(da[0], da[1], da[2], 0));
	if(dytypep(p, "_vec4d_t"))return(vec4d(da[0], da[1], da[2], da[3]));
	if(dytypep(p, "_quatd_t"))return(vec4d(da[0], da[1], da[2], da[3]));

	return(vec4d(0, 0, 0, 0));
}

BGBDY_API quatd dyquatvd(dyt p)
{
	double *da;
	float *fa;

	fa=(float *)p;
	if(dytypep(p, "_vec2_t"))return(quatd(0, fa[0], fa[1], 0));
	if(dytypep(p, "_vec3_t"))return(quatd(0, fa[0], fa[1], fa[2]));
	if(dytypep(p, "_vec4_t"))return(quatd(fa[3], fa[0], fa[1], fa[2]));
	if(dytypep(p, "_quat_t"))return(quatd(fa[3], fa[0], fa[1], fa[2]));

	da=(double *)p;
	if(dytypep(p, "_vec2d_t"))return(quatd(0, da[0], da[1], 0));
	if(dytypep(p, "_vec3d_t"))return(quatd(0, da[0], da[1], da[2]));
	if(dytypep(p, "_vec4d_t"))return(quatd(da[3], da[0], da[1], da[2]));
	if(dytypep(p, "_quatd_t"))return(quatd(da[3], da[0], da[1], da[2]));
	
	if(dyrealp(p))
	{
		return(quatd(dyrealv(p), 0, 0, 0));
	}

	if(dycomplexp(p))
	{
		return(quatd(dycreal(p), dycimag(p), 0, 0));
	}
	
	return(quatd(0, 0, 0, 0));
}


typedef struct DYAttr_S {
dyt var;
dyt val;
}DYAttr;

BGBDY_API dyt dyattr(dyt var, dyt val)
{
	DYAttr *tmp;
	tmp=gctalloc("_attr_t", sizeof(DYAttr));
	tmp->var=var;
	tmp->val=val;
	return((dyt)tmp);
}

BGBDY_API int dyattrp(dyt p)
{
	return(dytypep(p, "_attr_t"));
}

BGBDY_API dyt dyattrvar(dyt p)
{
	return(((DYAttr *)p)->var);
}

BGBDY_API dyt dyattrval(dyt p)
{
	return(((DYAttr *)p)->val);
}

struct DYHash_S {
char **var;
dyt *val;
int cnt;
};

BGBDY_API dyt dyhash(int cnt)
{
	struct DYHash_S *tmp;

	if(cnt<64)cnt=64;

	tmp=gctalloc("_hash_t", sizeof(struct DYHash_S));
	tmp->cnt=cnt;
	gcSet(tmp->var, gcalloc(cnt*sizeof(char *)));
	gcSet(tmp->val, gcalloc(cnt*sizeof(dyt)));

	return((dyt)tmp);
}

BGBDY_API int dyhashp(dyt p)
{
	return(dytypep(p, "_hash_t"));
}

BGBDY_API dyt dyhashget(dyt p, char *str)
{
	struct DYHash_S *hash;
	int hi;
	char *s;
	int i, j, k;

	if(!p || !str)return(NULL);

	hash=(struct DYHash_S *)p;

	hi=0; s=str;
	while(*s)hi=hi*251+(*s++);

	i=hi;
	for(j=0; j<16; j++)
	{
		i=(i*251)&0x3FFFFFFF; k=(i>>8)%hash->cnt;
		if(!hash->var[k])return(NULL);
		if(!strcmp(hash->var[k], str))
			return(hash->val[k]);
	}
	return(NULL);
}

BGBDY_API dyt dyhashset(dyt obj, char *var, dyt val)
{
	struct DYHash_S *hash;
	char **vart;
	dyt *valt;
	dyt p;
	int hi, cnt;
	char *s;
	int i, j, k;

	if(!obj || !var)return(NULL);

	hash=(struct DYHash_S *)obj;

	hi=0; s=var;
	while(*s)hi=hi*251+(*s++);

	i=hi;
	for(j=0; j<16; j++)
	{
		i=(i*251)&0x3FFFFFFF; k=(i>>8)%hash->cnt;
		if(!hash->var[k])
		{
			gcSet(hash->var[k], dysymbol(var));
			gcSet(hash->val[k], val);
			return(NULL);
		}
		if(!strcmp(hash->var[k], var))
		{
			p=hash->val[k];
			gcSet(hash->val[k], val);
			return(p);
		}
	}

	//hash is full, expand
	gcSet(vart, hash->var);
	gcSet(valt, hash->val);
	cnt=hash->cnt;

	hash->cnt=cnt+(cnt>>1);
	gcSet(hash->var, gcalloc(hash->cnt*sizeof(char *)));
	gcSet(hash->val, gcalloc(hash->cnt*sizeof(dyt)));

	for(i=0; i<cnt; i++)
		dyhashset(obj, vart[i], valt[i]);

	p=dyhashset(obj, var, val);
	return(p);
}

