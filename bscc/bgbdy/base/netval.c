/*--
Cat pdlib;BGBDY
Text
	objective:
	create code for managing builtin dynamic types.
	make use of pdlib type system as base.
--*/

#include <bgbgc.h>
#include <bgbdy.h>

#include <math.h>

#define NETVAL_RANGE	(1<<24)
#define NETVAL_MIN (0-(1<<23))
#define NETVAL_MAX ((1<<23)-1)
#define NETVAL_OFS (1<<23)

int netval_float28=0;
size_t netval_irange;
size_t netval_frange;

s64 netval_imin;
s64 netval_imax;
s64 netval_iofs;

byte *bgbdy_vintrange;
// byte *bgbdy_vintfrange;
byte *bgbdy_vffrange;
int bgbdy_bool[2]={0, 1};

byte *bgbdy_vustrange;
byte *bgbdy_vusyrange;

byte *bgbdy_vcharrange;

//void *bgbdy_symhash=NULL;
//void *bgbdy_kwhash=NULL;
//void *bgbdy_strhash=NULL;
//void *bgbdy_flhash=NULL;

void *bgbdy_symhash;
void *bgbdy_kwhash;
void *bgbdy_strhash;
void *bgbdy_strhash16;
void *bgbdy_flhash;

void *bgbdy_fbvt_s32=NULL;
void *bgbdy_fbvt_s64=NULL;
void *bgbdy_fbvt_s128=NULL;
void *bgbdy_fbvt_f32=NULL;
void *bgbdy_fbvt_f64=NULL;
void *bgbdy_fbvt_f128=NULL;

void *bgbdy_fbvt_vec2=NULL;
void *bgbdy_fbvt_vec3=NULL;
void *bgbdy_fbvt_vec4=NULL;
void *bgbdy_fbvt_quat=NULL;

void *bgbdy_fbvt_vec2d=NULL;
void *bgbdy_fbvt_vec3d=NULL;
void *bgbdy_fbvt_vec4d=NULL;
void *bgbdy_fbvt_quatd=NULL;

int BGBDY_IsPRange(byte *p, byte *addr, int rng)
{
	if(!((p-addr)&(~(rng-1))))
		return(1);
	return(0);
}

int BGBDY_IsFixnumFP(byte *p)
{
//	if(!((p-bgbdy_vintrange)&(~(NETVAL_RANGE-1))))
//		return(1);
	if(!((p-bgbdy_vintrange)&(~(netval_irange-1))))
		return(1);
	return(0);
}

int BGBDY_IsFlonumFP(byte *p)
{
	if(!((p-bgbdy_vffrange)&(~(netval_frange-1))))
		return(1);
	return(0);
}

int BGBDY_IsFixRealFP(byte *p)
{
	if(!((p-bgbdy_vintrange)&(~(netval_irange-1))))
		return(1);
	if(!((p-bgbdy_vffrange)&(~(netval_frange-1))))
		return(1);
	return(0);
}

char *BGBDY_GetVType(byte *p)
{
	char *s;

//	if(!((p-bgbdy_vintrange)&(~(NETVAL_RANGE-1))))
//		return("_fixint_t");

	if(!((p-bgbdy_vintrange)&(~(netval_irange-1))))
		return("_fixint_t");

	if(!((p-bgbdy_vffrange)&(~(netval_frange-1))))
		return("_flonum_t");

//	if(!((p-bgbdy_vintfrange)&(~(NETVAL_RANGE-1))))
//		return("_float_t");

	if(((void *)p)==BGBDY_TRUE)return("_bool_t");
	if(((void *)p)==BGBDY_FALSE)return("_bool_t");
	if(((void *)p)==UNDEFINED)return("_undefined_t");

	return(NULL);
}

char *BGBDY_HashSymbol(void **tab, char *ty, char *str)
{
	char **hash;
	char *s, *t;
	int i, j, k, hv;

	hash=(char **)*tab;
	if(!hash)
	{
//		hash=malloc(16384*sizeof(char *));
		hash=gcweak(65536*sizeof(char *));
//		hash=gcalloc(16384*sizeof(char *));
		memset(hash, 0, 65536*sizeof(char *));

		gclock(hash);

		*tab=(void *)hash;
	}

	s=str; i=0;
	if(*s)i=(*s++)<<8;
	while(*s)i=(i*251)+(*s++);
	hv=i;

	for(j=0; j<16; j++)
	{
		k=(i>>8)&0xFFFF;
//		if(!hash[k])break;
		if(!hash[k])continue;
		if(!strcmp(hash[k], str))return(hash[k]);
		i*=251;
	}

	i=hv;
	for(j=0; j<16; j++)
	{
		k=(i>>8)&0xFFFF;
		if(!hash[k])
		{
			t=gctalloc(ty, strlen(str)+1);
			strcpy(t, str);
			hash[k]=t;
			return(t);
		}
		i*=251;
	}

	printf("BGBDY_HashSymbol: hash full\n");
	*(int *)-1=-1;
	return(NULL);
}

char *BGBDY_HashSymbolQuick(void **tab, char *ty, char *str)
{
	char **hash;
	char *s, *t;
	int i, j;

	hash=(char **)*tab;
	if(!hash)
	{
//		hash=malloc(16384*sizeof(void *));
		hash=gcweak(16384*sizeof(void *));
//		hash=gcalloc(16384*sizeof(void *));
		memset(hash, 0, 16384*sizeof(char *));

		gclock(hash);

		*tab=(void *)hash;
	}

	s=str; i=0;
	if(*s)i=(*s++)<<8;
	while(*s)i=(i*251)+(*s++);

//	i=((i*251)>>8)&0x3FFF;
	i=(i>>8)&0x3FFF;
	t=hash[i];
	if(t && !strcmp(t, str))return(t);

	t=gctalloc(ty, strlen(str)+1);
	strcpy(t, str);
	hash[i]=t;

	return(t);
}

int bgbdy_wstrlen(u16 *str)
{
	int i;
	for(i=0; str[i]; i++);
	return(i);
}

int bgbdy_wstrcmp(u16 *s0, u16 *s1)
{
	while(*s0 && (*s0 == *s1)) { s0++; s1++; }
	if((*s0)<(*s1))return(-1);
	if((*s0)>(*s1))return(1);
	return(0);
}

void bgbdy_wstrcpy(u16 *sdst, u16 *ssrc)
{
	while(*ssrc)*sdst++=*ssrc++;
	*sdst++=*ssrc++;
}

u16 *BGBDY_HashSymbol16Quick(void **tab, char *ty, u16 *str)
{
	u16 **hash;
	u16 *s, *t;
	int i, j;

	hash=(u16 **)(*tab);
	if(!hash)
	{
//		hash=malloc(4096*sizeof(void *));
//		hash=gcweak(4096*sizeof(void *));
		hash=gcalloc(16384*sizeof(void *));
		memset(hash, 0, 16384*sizeof(void *));

		gclock(hash);

		*tab=(void *)hash;
	}

	s=str; i=0;
	if(*s)i=(*s++)<<8;
	while(*s)i=(i*251)+(*s++);

//	i=((i*251)>>8)&0x3FFF;
	i=(i>>8)&0x3FFF;
	t=hash[i];
	if(t && !bgbdy_wstrcmp(t, str))return(t);

	t=gctalloc(ty, bgbdy_wstrlen(str)+1);
	bgbdy_wstrcpy(t, str);
	hash[i]=t;

	return(t);
}

BGBDY_API void *BGBDY_WrapIntFast(int i)
{
//	i=(i+NETVAL_OFS)&(NETVAL_RANGE-1);
	i=(i+netval_iofs)&(netval_irange-1);
	return(bgbdy_vintrange+i);
}

BGBDY_API int BGBDY_UnwrapIntFast(void *a)
{
//	return(((long)a-(long)bgbdy_vintrange)+NETVAL_MIN);
//	return(((long)a-(long)bgbdy_vintrange)+netval_imin);
	return(((byte *)a-(byte *)bgbdy_vintrange)+netval_imin);
}

BGBDY_API s32 *BGBDY_AllocInt32(void)
{
	void *p;
	
	if(bgbdy_fbvt_s32)
	{
		p=bgbdy_fbvt_s32;
		bgbdy_fbvt_s32=*(void **)p;
		return(p);
	}
	
	p=gctatomic("_int_t", sizeof(f32));
	return(p);
}

BGBDY_API s64 *BGBDY_AllocInt64(void)
{
	void *p;

	if(bgbdy_fbvt_s64)
	{
		p=bgbdy_fbvt_s64;
		bgbdy_fbvt_s64=*(void **)p;
		return(p);
	}

	p=gctatomic("_long_t", sizeof(s64));
	return(p);
}

BGBDY_API f32 *BGBDY_AllocFloat32(void)
{
	void *p;

	if(bgbdy_fbvt_f32)
	{
		p=bgbdy_fbvt_f32;
		bgbdy_fbvt_f32=*(void **)p;
		return(p);
	}

	p=gctatomic("_float_t", sizeof(f32));
	return(p);
}

BGBDY_API f64 *BGBDY_AllocFloat64(void)
{
	void *p;

	if(bgbdy_fbvt_f64)
	{
		p=bgbdy_fbvt_f64;
		bgbdy_fbvt_f64=*(void **)p;
		return(p);
	}

	p=gctatomic("_double_t", sizeof(f64));
	return(p);
}

BGBDY_API s128 *BGBDY_AllocInt128(void)
{
	void *p;

	if(bgbdy_fbvt_s128)
	{
		p=bgbdy_fbvt_s128;
		bgbdy_fbvt_s128=*(void **)p;
		return(p);
	}

	p=gctatomic("_int128_t", sizeof(s128));
	return(p);
}

BGBDY_API f128 *BGBDY_AllocFloat128(void)
{
	void *p;

	if(bgbdy_fbvt_f128)
	{
		p=bgbdy_fbvt_f128;
		bgbdy_fbvt_f128=*(void **)p;
		return(p);
	}

	p=gctatomic("_float128_t", sizeof(f128));
	return(p);
}

BGBDY_API float *BGBDY_AllocVec2(void)
{
	void *p;

	if(bgbdy_fbvt_vec2)
	{
		p=bgbdy_fbvt_vec2;
		bgbdy_fbvt_vec2=*(void **)p;
		return(p);
	}

	p=gctatomic("_vec2_t", 4*sizeof(float));
	return(p);
}

BGBDY_API float *BGBDY_AllocVec3(void)
{
	void *p;

	if(bgbdy_fbvt_vec3)
	{
		p=bgbdy_fbvt_vec3;
		bgbdy_fbvt_vec3=*(void **)p;
		return(p);
	}

	p=gctatomic("_vec3_t", 4*sizeof(float));
	return(p);
}

BGBDY_API float *BGBDY_AllocVec4(void)
{
	void *p;

	if(bgbdy_fbvt_vec4)
	{
		p=bgbdy_fbvt_vec4;
		bgbdy_fbvt_vec4=*(void **)p;
		return(p);
	}

	p=gctatomic("_vec4_t", 4*sizeof(float));
	return(p);
}

BGBDY_API float *BGBDY_AllocQuat(void)
{
	void *p;

	if(bgbdy_fbvt_quat)
	{
		p=bgbdy_fbvt_quat;
		bgbdy_fbvt_quat=*(void **)p;
		return(p);
	}

	p=gctatomic("_quat_t", 4*sizeof(float));
	return(p);
}

BGBDY_API double *BGBDY_AllocVec2D(void)
{
	void *p;

	if(bgbdy_fbvt_vec2d)
	{
		p=bgbdy_fbvt_vec2d;
		bgbdy_fbvt_vec2d=*(void **)p;
		return(p);
	}

	p=gctatomic("_vec2d_t", 4*sizeof(double));
	return(p);
}

BGBDY_API double *BGBDY_AllocVec3D(void)
{
	void *p;

	if(bgbdy_fbvt_vec3d)
	{
		p=bgbdy_fbvt_vec3d;
		bgbdy_fbvt_vec3d=*(void **)p;
		return(p);
	}

	p=gctatomic("_vec3d_t", 4*sizeof(double));
	return(p);
}

BGBDY_API double *BGBDY_AllocVec4D(void)
{
	void *p;

	if(bgbdy_fbvt_vec4d)
	{
		p=bgbdy_fbvt_vec4d;
		bgbdy_fbvt_vec4d=*(void **)p;
		return(p);
	}

	p=gctatomic("_vec4d_t", 4*sizeof(double));
	return(p);
}

BGBDY_API double *BGBDY_AllocQuatD(void)
{
	void *p;

	if(bgbdy_fbvt_quatd)
	{
		p=bgbdy_fbvt_quatd;
		bgbdy_fbvt_quatd=*(void **)p;
		return(p);
	}

	p=gctatomic("_quatd_t", 4*sizeof(double));
	return(p);
}

BGBDY_API void BGBDY_FreeInt32(s32 *p)
{
	*(void **)p=bgbdy_fbvt_s32;
	bgbdy_fbvt_s32=p;
}

BGBDY_API void BGBDY_FreeInt64(s64 *p)
{
	*(void **)p=bgbdy_fbvt_s64;
	bgbdy_fbvt_s64=p;
}

BGBDY_API void BGBDY_FreeFloat32(f32 *p)
{
	*(void **)p=bgbdy_fbvt_f32;
	bgbdy_fbvt_f32=p;
}

BGBDY_API void BGBDY_FreeFloat64(f64 *p)
{
	*(void **)p=bgbdy_fbvt_f64;
	bgbdy_fbvt_f64=p;
}

BGBDY_API void BGBDY_FreeInt128(s128 *p)
{
	*(void **)p=bgbdy_fbvt_s128;
	bgbdy_fbvt_s128=p;
}

BGBDY_API void BGBDY_FreeFloat128(f128 *p)
{
	*(void **)p=bgbdy_fbvt_f128;
	bgbdy_fbvt_f128=p;
}

BGBDY_API void BGBDY_FreeVec2(float *p)
{
	*(void **)p=bgbdy_fbvt_vec2;
	bgbdy_fbvt_vec2=p;
}

BGBDY_API void BGBDY_FreeVec3(float *p)
{
	*(void **)p=bgbdy_fbvt_vec3;
	bgbdy_fbvt_vec3=p;
}

BGBDY_API void BGBDY_FreeVec4(float *p)
{
	*(void **)p=bgbdy_fbvt_vec4;
	bgbdy_fbvt_vec4=p;
}

BGBDY_API void BGBDY_FreeQuat(float *p)
{
	*(void **)p=bgbdy_fbvt_quat;
	bgbdy_fbvt_quat=p;
}

BGBDY_API void BGBDY_FreeVec2D(double *p)
{
	*(void **)p=bgbdy_fbvt_vec2d;
	bgbdy_fbvt_vec2d=p;
}

BGBDY_API void BGBDY_FreeVec3D(double *p)
{
	*(void **)p=bgbdy_fbvt_vec3d;
	bgbdy_fbvt_vec3d=p;
}

BGBDY_API void BGBDY_FreeVec4D(double *p)
{
	*(void **)p=bgbdy_fbvt_vec4d;
	bgbdy_fbvt_vec4d=p;
}

BGBDY_API void BGBDY_FreeQuatD(double *p)
{
	*(void **)p=bgbdy_fbvt_quatd;
	bgbdy_fbvt_quatd=p;
}

void *BGBDY_WrapInt(int i)
{
	int *p;

	BGBDY_InitTypes();

//	if((i>=NETVAL_MIN) && (i<=NETVAL_MAX))
//		return(bgbdy_vintrange+i+NETVAL_OFS);

	if((i>=netval_imin) && (i<=netval_imax))
		return(bgbdy_vintrange+i+netval_iofs);

//	p=gctalloc("_int_t", sizeof(int));
	p=BGBDY_AllocInt32();
	*p=i;

	return((void *)p);
}

void *BGBDY_WrapInt64(s64 i)
{
	s64 *p;

	BGBDY_InitTypes();

//	if((i>=NETVAL_MIN) && (i<=NETVAL_MAX))
//		return(bgbdy_vintrange+i+NETVAL_OFS);

//	if((i>=netval_imin) && (i<=netval_imax))
//		return(bgbdy_vintrange+i+netval_iofs);

	if((i>=(-(1LL<<31))) && (i<=((1LL<<31)-1)))
		return(BGBDY_WrapInt((int)i));

//	p=gctalloc("_long_t", sizeof(s64));
	p=BGBDY_AllocInt64();
	*p=i;

	return((void *)p);
}

void *BGBDY_TryWrapInt64(s64 i)
{
#ifdef X86_64
	if((i>=netval_imin) && (i<=netval_imax))
		return(bgbdy_vintrange+i+netval_iofs);
#else
	if((i>=netval_imin) && (i<=netval_imax))
		return(bgbdy_vintrange+((int)i)+netval_iofs);
#endif
	return(NULL);
}

void *BGBDY_WrapChar(int i)
{
	BGBDY_InitTypes();
	if((i>=0) && (i<=65535))
		return(bgbdy_vcharrange+i);
	return(BGBDY_WrapInt(i));
}

void *BGBDY_HashString(char *s)
{
	char *t;
	t=BGBDY_HashSymbolQuick(&bgbdy_strhash, "_string_t", s);
	return(t);
}

void *BGBDY_WrapSymbol(char *s)
{
	char *t;
	t=BGBDY_HashSymbol(&bgbdy_symhash, "_symbol_t", s);
	return(t);
}

void *BGBDY_WrapKeyword(char *s)
{
	char *t;
	t=BGBDY_HashSymbol(&bgbdy_kwhash, "_keyword_t", s);
	return(t);
}

void *BGBDY_HashString16(u16 *s)
{
	u16 *t;
	t=BGBDY_HashSymbol16Quick(&bgbdy_strhash16, "_wstring_t", s);
	return(t);
}

void *BGBDY_WrapFixedSymbol(char *s)
{
	static char *hash_src[256];
	static char *hash_dst[256];
	char *t;
	int i;
	
	i=((((nlint)s)*65521)>>16)&255;
	if(hash_src[i]==s)
		return(hash_dst[i]);
	
	t=BGBDY_HashSymbol(&bgbdy_symhash, "_symbol_t", s);
	hash_src[i]=s;
	hash_dst[i]=t;
	return(t);
}


int BGBDY_EncUStrChar(int i)
{
	if((i>='A') && (i<='Z'))return(i-'A');
	if((i>='a') && (i<='z'))return(i-'a'+26);
	if((i>='0') && (i<='9'))return(i-'0'+52);
	if(i=='_')return(62);
	if(!i)return(63);
	return(-1);
}

int BGBDY_DecUStrChar(int i)
{
	if((i>= 0) && (i<26))return(i+'A');
	if((i>=26) && (i<52))return((i-26)+'a');
	if((i>=52) && (i<62))return((i-52)+'0');
	if(i==62)return('_');
	if(i==63)return(0);
	return(-1);
}

int BGBDY_EncodeUChars(char *s)
{
	int i, i0, i1, i2, i3;

	i0=BGBDY_EncUStrChar(s[0]);
	i1=BGBDY_EncUStrChar(s[1]);
	i2=BGBDY_EncUStrChar(s[2]);
	i3=BGBDY_EncUStrChar(s[3]);

	if(i0==63)return(63);
	if((i0>=0) && (i1==63))return(i0+(63<<6));
	if((i0>=0) && (i1>=0) && (i2==63))return(i0+(i1<<6)+(63<<12));
	if((i0>=0) && (i1>=0) && (i2>=0) && (i3==63))
		return(i0+(i1<<6)+(i2<<12)+(63<<18));
	if((i0>=0) && (i1>=0) && (i2>=0) && (i3>=0))
		return(i0+(i1<<6)+(i2<<12)+(i3<<18));

	if(!(s[1]) || !(s[2]))
	{
		i=63+(((byte)(s[0]))<<8)+(((byte)(s[1]))<<16);
		return(i);
	}
	return(-1);
}

char *BGBDY_DecodeUChars(int v)
{
	char buf[5];

	if((v&63)==64)
	{
		buf[0]=(char)(v>>8);
		buf[1]=(char)(v>>16);
		buf[2]=0;
		return(gcrstrdup(buf));
	}

	buf[0]=BGBDY_DecUStrChar(v&63);
	buf[1]=BGBDY_DecUStrChar((v>>6)&63);
	buf[2]=BGBDY_DecUStrChar((v>>12)&63);
	buf[3]=BGBDY_DecUStrChar((v>>18)&63);
	buf[4]=0;
	return(gcrstrdup(buf));
}

void *BGBDY_WrapUString(char *s)
{
	int i;

	i=BGBDY_EncodeUChars(s);
	if(i>=0)return(bgbdy_vustrange+i);
	return(BGBDY_HashString(s));
}

void *BGBDY_WrapUSymbol(char *s)
{
	int i;

	i=BGBDY_EncodeUChars(s);
	if(i>=0)return(bgbdy_vusyrange+i);
	return(BGBDY_HashString(s));
}

char *BGBDY_UnwrapUString(void *p)
{
	int i;
	i=((byte *)p)-bgbdy_vustrange;
	return(BGBDY_DecodeUChars(i));
}

char *BGBDY_UnwrapUSymbol(void *p)
{
	int i;
	i=((byte *)p)-bgbdy_vusyrange;
	return(BGBDY_DecodeUChars(i));
}

#ifdef X86_64
BGBDY_API void *BGBDY_WrapMiniFloat(double f)
{
	u64 l;
	*(double *)(&l)=f;
	return(bgbdy_vffrange+(l>>16));
}

BGBDY_API double BGBDY_UnwrapMiniFloat(void *p)
{
	u64 l;
	l=((byte *)p)-bgbdy_vffrange;
	l<<=16;
	return(*(double *)(&l));
}
#else

// #define BGBDY_F28_BIAS	24
#define BGBDY_F28_BIAS	32

double bgbdy_pow2(int i)
{
	if(i>0)return(1<<i);
	if(i<0)return(1.0/(1<<(-i)));
	return(1);
}

#if 0
void *BGBDY_WrapMiniFloat28(double f)
{
	int i, j;
	u32 k;

	//S.7.20, bias=32

	if(f<0) { f=-f; k|=0x8000000; } else k=0;
	i=floor(log(f)/log(2));
	if(i<(-(BGBDY_F28_BIAS+12)))return(bgbdy_vffrange+0);
	if(i<(-BGBDY_F28_BIAS))i=-BGBDY_F28_BIAS;
	if(i>=(128-BGBDY_F28_BIAS))return(NULL);
	j=(u32)((f/bgbdy_pow2(i)-1)*1048576.0+0.5);
	if(j>0xFFFFF)j=0xFFFFF;
	k|=((i+BGBDY_F28_BIAS)<<20)|j;
	return(bgbdy_vffrange+k);
}

double BGBDY_UnwrapMiniFloat28(void *p)
{
	double f;
	u32 i;
	int j;

	i=((byte *)p)-bgbdy_vffrange;
	f=(1+((i&0xFFFFF)/1048576.0))*bgbdy_pow2(((i>>20)&127)-BGBDY_F28_BIAS);
	if(i&0x8000000)f=-f;
	return(f);
}
#endif

#if 1
void *BGBDY_WrapMiniFloat28(double v)
{
	int i, j;
	float f;
	u32 k;

	f=v; k=((*(u32 *)(&f))+8)>>4;
//	*(float *)(&k)=f;
//	k=(k+7)>>4;
	return(bgbdy_vffrange+k);
}

double BGBDY_UnwrapMiniFloat28(void *p)
{
	double f;
	u32 i;
	int j;

	i=((byte *)p)-bgbdy_vffrange;
	i=i<<4;
	f=*(float *)(&i);
	return(f);
}
#endif

void *BGBDY_WrapMiniFloat24(double f)
{
	int i, j;
	u32 k;

	//S.7.16, bias=24

	if(f<0) { f=-f; k|=0x800000; } else k=0;
//	i=ceil(log2(f));
	i=ceil(log(f)/log(2));
//	if(i<(-40))return(bgbdy_vintfrange+NETVAL_OFS);
	if(i<(-40))return(bgbdy_vffrange);
	if(i<(-24))i=-24;
	if(i>=104)return(NULL);
	j=(u32)((f/pow(2, i))*65536.0+0.5);
	if(j>0xFFFF)j=0xFFFF;
	k|=((i+24)<<16)|j;
	return(bgbdy_vffrange+k);
}

double BGBDY_UnwrapMiniFloat24(void *p)
{
	double f;
	u32 i;

	i=((byte *)p)-bgbdy_vffrange;
	f=((i&0xFFFF)/65536.0)*pow(2, ((i>>16)&127)-24);
	if(i&0x800000)f=-f;
	return(f);
}

BGBDY_API void *BGBDY_WrapMiniFloat(double f)
{
	if(netval_float28)
		return(BGBDY_WrapMiniFloat28(f));
	return(BGBDY_WrapMiniFloat24(f));
}

BGBDY_API double BGBDY_UnwrapMiniFloat(void *p)
{
	if(netval_float28)
		return(BGBDY_UnwrapMiniFloat28(p));
	return(BGBDY_UnwrapMiniFloat24(p));
}
#endif

void *BGBDY_WrapFlonum(double f)
{
	double *pd;
	void *p;
	int i;

	BGBDY_InitTypes();

	p=BGBDY_WrapMiniFloat(f);
	if(p)return(p);

//	i=(int)f;
//	if(i==f)if((i>=NETVAL_MIN) && (i<=NETVAL_MAX))
//		return(bgbdy_vintfrange+i+NETVAL_OFS);

//	p=BGBDY_WrapMiniFloat(f);
//	if(p)return(p);

	pd=gctalloc("_float_t", sizeof(double));
	*pd=f;
	return((void *)pd);
}

void *BGBDY_WrapFloat(double f)
{
	double *p;
	int i;

	BGBDY_InitTypes();

//	i=(int)f;
//	if(i==f)if((i>=NETVAL_MIN) && (i<=NETVAL_MAX))
//		return(bgbdy_vintfrange+i+NETVAL_OFS);

#ifdef X86_64
	p=BGBDY_WrapMiniFloat(f);
	if(p)return(p);
#endif

//#else

#if 0
	if(netval_float28)
	{
		p=BGBDY_WrapMiniFloat28(f);
		if(p)return(p);
	}

//	i=(int)f;
//	if(i==f)if((i>=NETVAL_MIN) && (i<=NETVAL_MAX))
//		return(bgbdy_vintfrange+i+NETVAL_OFS);
#endif

	p=gctalloc("_float_t", sizeof(double));
	*p=f;
	return((void *)p);
}

void *BGBDY_WrapDouble(double f)
{
	double *p;

	p=gctalloc("_double_t", sizeof(double));
	*p=f;

	return((void *)p);
}


void *BGBDY_WrapArray(void *a)
{
	void **ip, **op;
	int i;

	if(!a)
	{
		op=gctalloc("_array_t", 1*sizeof(void *));
		return((void *)op);
	}

	ip=(void **)a;
	for(i=0; ip[i]; i++);

	op=gctalloc("_array_t", (i+1)*sizeof(void *));
	for(i=0; ip[i]; i++)op[i]=ip[i];
	op[i++]=NULL;

	return((void *)op);
}

/*--
Cat pdlib;BGBDY
Form
	void *BGBDY_WrapDArray(void *a);
Description
	Wraps a dynamic array.
--*/
void *BGBDY_WrapDArray(void *a)
{
	void *p, *p2;
	int i;

	p=BGBDY_WrapArray(a);
	p2=gctalloc("_d_array_t", sizeof(void *));
	*(void **)p2=p;

	return(p2);
}

/*--
Cat pdlib;BGBDY
Form
	void *BGBDY_WrapNDArray(void *a, int n);
Description
	Wraps a dynamic array with a given args count.
--*/
void *BGBDY_WrapNDArray(void *a, int n)
{
	void **ip, **op;
	void *p;
	int i;

	ip=(void **)a;
	op=gctalloc("_array_t", (n+1)*sizeof(void *));
	for(i=0; i<n; i++)op[i]=ip[i];
	op[i]=NULL;

	p=gctalloc("_d_array_t", sizeof(void *));
	*(void **)p=(void *)op;

	return(p);
}

int BGBDY_GetDArraySize(void *arr)
{
	int i;

	i=(gcGetSize(*(void **)arr)/sizeof(void *))-1;
	return(i);
}

void *BGBDY_GetDArrayIndex(void *arr, int idx)
{
	void **a;
	int l;

	if(idx<0)return(NULL);

	l=BGBDY_GetDArraySize(arr);
	if(idx>=l)return(NULL);

	a=(void **)(*(void **)arr);
	return(a[idx]);
}

int BGBDY_SetDArrayIndex(void *arr, int idx, void *val)
{
	void **a, **b;
	int i, l;

	if(idx<0)return(-1);

	a=(void **)(*(void **)arr);

	l=BGBDY_GetDArraySize(arr);
	if(idx>=l)
	{
		b=gctalloc("_array_t", (idx+2)*sizeof(void *));
		for(i=0; i<l; i++)b[i]=a[i];
		*(void **)arr=(void *)b;

		gcfree(a);
		a=b;
	}

	a[idx]=val;

	return(0);
}


int BGBDY_UnwrapInt(void *a)
{
	char *ty;

//	if(BGBDY_IsPRange(a, bgbdy_vintrange, NETVAL_RANGE))
//		return(((long)a-(long)bgbdy_vintrange)+NETVAL_MIN);
//	if(BGBDY_IsPRange(a, bgbdy_vintfrange, NETVAL_RANGE))
//		return(((long)a-(long)bgbdy_vintfrange)+NETVAL_MIN);
//	if(BGBDY_IsPRange(a, bgbdy_vffrange, NETVAL_RANGE))
//		return((((long)a-(long)bgbdy_vffrange)-NETVAL_OFS)/256.0);

	if(BGBDY_IsPRange(a, bgbdy_vintrange, netval_irange))
		return(((long)a-(long)bgbdy_vintrange)+netval_imin);

	if(BGBDY_IsPRange(a, bgbdy_vffrange, netval_frange))
		return(BGBDY_UnwrapMiniFloat(a));

	if(BGBDY_IsPRange(a, bgbdy_vcharrange, 65536))
		return((long)a-(long)bgbdy_vcharrange);

	if(a==BGBDY_NULL)return(0);
	if(a==BGBDY_TRUE)return(1);
	if(a==BGBDY_FALSE)return(0);

//	ty=BGBDY_GetTypeName(a);
	ty=gcgettype(a);
	if(!ty)return(0);
	if(!strcmp(ty, "_int_t"))return(*(int *)a);
	if(!strcmp(ty, "_long_t"))return(*(s64 *)a);
	if(!strcmp(ty, "_float_t"))return(*(double *)a);
	if(!strcmp(ty, "_double_t"))return(*(double *)a);

	if(!strcmp(ty, "_int128_t"))
		return(iv128_s64v(*(iv128_t *)a));
	if(!strcmp(ty, "_float128_t"))
		return(fv128_toDouble(*(fv128_t *)a));

	if(dyllTypeboxPtrP((dyt)a))
		{ return((nlint)dyllTypeboxPtrData((dyt)a)); }

	return(0);
}

s64 BGBDY_UnwrapInt64(void *a)
{
	char *ty;

//	if(BGBDY_IsPRange(a, bgbdy_vintrange, NETVAL_RANGE))
//		return(((long)a-(long)bgbdy_vintrange)+NETVAL_MIN);
//	if(BGBDY_IsPRange(a, bgbdy_vintfrange, NETVAL_RANGE))
//		return(((long)a-(long)bgbdy_vintfrange)+NETVAL_MIN);
//	if(BGBDY_IsPRange(a, bgbdy_vffrange, NETVAL_RANGE))
//		return((((long)a-(long)bgbdy_vffrange)-NETVAL_OFS)/256.0);

	if(BGBDY_IsPRange(a, bgbdy_vintrange, netval_irange))
		return(((long)a-(long)bgbdy_vintrange)+netval_imin);

	if(BGBDY_IsPRange(a, bgbdy_vffrange, netval_frange))
		return(BGBDY_UnwrapMiniFloat(a));

	if(BGBDY_IsPRange(a, bgbdy_vcharrange, 65536))
		return((long)a-(long)bgbdy_vcharrange);

	if(a==BGBDY_NULL)return(0);
	if(a==BGBDY_TRUE)return(1);
	if(a==BGBDY_FALSE)return(0);

//	ty=BGBDY_GetTypeName(a);
	ty=gcgettype(a);
	if(!ty)return(0);
	if(!strcmp(ty, "_int_t"))return(*(int *)a);
	if(!strcmp(ty, "_long_t"))return(*(s64 *)a);
	if(!strcmp(ty, "_float_t"))return(*(double *)a);
	if(!strcmp(ty, "_double_t"))return(*(double *)a);

	if(!strcmp(ty, "_int128_t"))
		return(iv128_s64v(*(iv128_t *)a));
	if(!strcmp(ty, "_float128_t"))
		return(fv128_toDouble(*(fv128_t *)a));

	if(dyllTypeboxPtrP((dyt)a))
		{ return((nlint)dyllTypeboxPtrData((dyt)a)); }

	return(0);
}

int BGBDY_UnwrapBool(void *a)
{
//	if(!a)return(0);

	if(a==BGBDY_NULL)return(0);
	if(a==BGBDY_TRUE)return(1);
	if(a==BGBDY_FALSE)return(0);

	if(gctypep(a, "_int_t"))return(BGBDY_UnwrapInt(a));
//	if(gctypep(a, "_bool_t"))return(*(int *)a);
	if(gctypep(a, "_float_t"))return(BGBDY_UnwrapFloat(a));
	if(gctypep(a, "_double_t"))return(BGBDY_UnwrapFloat(a));
	return(1);
}

double BGBDY_UnwrapFloat(void *a)
{
	char *ty;

//	if(BGBDY_IsPRange(a, bgbdy_vintrange, NETVAL_RANGE))
//		return(((long)a-(long)bgbdy_vintrange)-NETVAL_OFS);
//	if(BGBDY_IsPRange(a, bgbdy_vintfrange, NETVAL_RANGE))
//		return(((long)a-(long)bgbdy_vintfrange)-NETVAL_OFS);
//	if(BGBDY_IsPRange(a, bgbdy_vffrange, NETVAL_RANGE))
//		return((((long)a-(long)bgbdy_vffrange)-NETVAL_OFS)/256.0);

	if(BGBDY_IsPRange(a, bgbdy_vintrange, netval_irange))
		return(((long)a-(long)bgbdy_vintrange)+netval_imin);

	if(BGBDY_IsPRange(a, bgbdy_vffrange, netval_frange))
		return(BGBDY_UnwrapMiniFloat(a));

	if(BGBDY_IsPRange(a, bgbdy_vcharrange, 65536))
		return((long)a-(long)bgbdy_vcharrange);

	if(a==BGBDY_NULL)return(0);
	if(a==BGBDY_TRUE)return(1);
	if(a==BGBDY_FALSE)return(0);

//	ty=BGBDY_GetTypeName(a);
	ty=gcgettype(a);
	if(!ty)return(0);

	if(!strcmp(ty, "_float_t"))return(*(double *)a);
	if(!strcmp(ty, "_double_t"))return(*(double *)a);
	if(!strcmp(ty, "_int_t"))return(*(int *)a);
	if(!strcmp(ty, "_long_t"))return(*(s64 *)a);

	if(!strcmp(ty, "_int128_t"))
		return(iv128_s64v(*(iv128_t *)a));
	if(!strcmp(ty, "_float128_t"))
		return(fv128_toDouble(*(fv128_t *)a));

	return(0);
}

void **BGBDY_UnwrapArray(void *a)
{
	return((void **)a);
}

void **BGBDY_UnwrapDArray(void *a)
{
	return((void **)(*(void **)a));
}


int BGBDY_IsNumber(void *p)
{
	char *ty;
//	ty=BGBDY_GetTypeName(p);
	ty=gcgettype(p);
	if(!ty)return(0);
	if(!strcmp(ty, "_int_t"))return(1);
	if(!strcmp(ty, "_long_t"))return(1);
	if(!strcmp(ty, "_float_t"))return(1);
	if(!strcmp(ty, "_double_t"))return(1);
	return(0);
}

static int BGBDY_InitSetupFixint(int bits)
{
	netval_irange=1<<bits;
	netval_imin=(0-(1<<(bits-1)));
	netval_imax=((1<<(bits-1))-1);
	netval_iofs=1<<(bits-1);
	return(0);
}

int BGBDY_InitTypes()
{
	static int init=0;

	if(init)return(0);
	init=1;

//	bgbdy_vintrange=BGBGC_SpaceAlloc(24);
//	bgbdy_vintfrange=BGBGC_SpaceAlloc(24);


#if 0
	bgbdy_vintrange=gcllSpaceNew("_fixint_t", 24);
//	bgbdy_vintfrange=gcllSpaceNew("_float_t", 24);
	BGBDY_InitSetupFixint(24);
#endif


#ifdef X86_64
	bgbdy_vffrange=gcllSpaceNew("_flonum_t", 48);
	netval_frange=1LL<<48;

	bgbdy_vintrange=gcllSpaceNew("_fixint_t", 48);
	BGBDY_InitSetupFixint(48);
#else
	if(gcllSpaceAllocSafeP(28))
	{
		netval_float28=1;
		netval_frange=1<<28;
		bgbdy_vffrange=gcllSpaceNew("_flonum_t", 28);
	}else
	{
		netval_float28=0;
		bgbdy_vffrange=gcllSpaceNew("_flonum_t", 24);
		netval_frange=1<<24;
	}

	if(gcllSpaceAllocSafeP(28))
	{
		bgbdy_vintrange=gcllSpaceNew("_fixint_t", 28);
		BGBDY_InitSetupFixint(28);
	}else
	{
		bgbdy_vintrange=gcllSpaceNew("_fixint_t", 24);
		BGBDY_InitSetupFixint(24);
	}
#endif

	bgbdy_vustrange=gcllSpaceNew("_ustring_t", 24);
	bgbdy_vusyrange=gcllSpaceNew("_usymbol_t", 24);

	bgbdy_vcharrange=gcllSpaceNew("_char_t", 16);

	return(1);
}
