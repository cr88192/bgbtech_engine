#include <bgbgc.h>
#include <bgbdy.h>

#include <math.h>

/*
DYTF Packed Vectors

PackVec2: 2x Float28 (Shave 4 bits)
PackVec3: 3x Float18 / Float16
PackVec4: 4x Float14

0x71 / 0x72 / 0x73

Alternative:
PeckVec3:
  8 Bits = Exponent
  3 x 16 bits = Normalized Vector
  Each represents a value in the range -1..1
PackVec4:
  8 Bits = Exponent
  4 x 12 bits = Normalized Vector
  Each represents a value in the range -1..1


0x7800 PackVec2
0x7801 PackVec3
0x7802 PackVec4
0x7803 PackQuat

PackVec2:
	2 x Float24

PeckVec3:
  6 Bits = Exponent
  3 x 14 bits = Normalized Vector
  Each represents a value in the range -1..1
PackVec4:
  8 Bits = Exponent
  4 x 10 bits = Normalized Vector
  Each represents a value in the range -1..1
*/

static double bgbdy_log2(double v)
{
	return(log(v)*3.3219280948873623478703194294894);
//	3.3219280948873623478703194294894
//	return(log(v)/log(2.0));
}

static int bgbdy_log2i(double v)
{
	u32 t;
	int i;
	*(float *)(&t)=v;
	i=((t>>23)&255)-127;
	return(i);

//	return(log(v)*3.3219280948873623478703194294894);
//	3.3219280948873623478703194294894
//	return(log(v)/log(2.0));
}

BGBDY_API dytf dytfPackVec2(float x, float y)
{
	dytf t;
	u32 xa, ya;
	
	xa=*(u32 *)(&x);
	ya=*(u32 *)(&y);
	
//	xa=(xa+8)>>4;
//	ya=(ya+8)>>4;
//	t.v=0x7100000000000000ULL | (((u64)ya)<<28)|xa;

	xa=(xa+128)>>8;
	ya=(ya+128)>>8;
	t.v=0x7800000000000000ULL | (((u64)ya)<<24)|xa;

	return(t);
}

BGBDY_API dytf dytfPackVec3(float x, float y, float z)
{
	float mx, my, mz, m;
	dytf t;
	int xa, ya, za;
	float sc;
	int e;

	mx=fabs(x);
	my=fabs(y);
	mz=fabs(z);
	m=mx;
	if(my>m)m=my;
	if(mz>m)m=mz;

//	e=(int)(ceil(bgbdy_log2(m)));
	e=bgbdy_log2i(m);

	if(e>63)
	{
		t.v=0x7100000000000000ULL | (255LL<<48);
//		t.v=0x7801000000000000ULL | (63LL<<42);
	}else if(e>=0)
	{
#if 1
		sc=32767.0/(1<<e);
		xa=(int)(x*sc+0.5);
		ya=(int)(y*sc+0.5);
		za=(int)(z*sc+0.5);
		xa=(xa<(-32767))?(-32767):((xa>32767)?32767:xa);
		ya=(ya<(-32767))?(-32767):((ya>32767)?32767:ya);
		za=(za<(-32767))?(-32767):((za>32767)?32767:za);
		xa=xa&65535;
		ya=ya&65535;
		za=za&65535;
		t.v=0x7100000000000000ULL | (((u64)(e+128))<<48) |
			(((u64)za)<<32)|(((u64)ya)<<16)|xa;
#endif

#if 0
//		sc=(32764>>e);
//		sc=(8190>>e);
//		sc=8190.0/(1<<e);
		sc=8191.0/(1<<e);
		xa=((int)(x*sc+0.5))&16383;
		ya=((int)(y*sc+0.5))&16383;
		za=((int)(z*sc+0.5))&16383;
//		xa=(xa<(-8191))?(-8191):((xa>8191)?8191:xa);
//		ya=(ya<(-8191))?(-8191):((ya>8191)?8191:ya);
//		za=(za<(-8191))?(-8191):((za>8191)?8191:za);
		t.v=0x7801000000000000ULL | (((u64)(e+32))<<42) |
			(((u64)za)<<28)|(((u64)ya)<<14)|xa;
#endif
	}else if(e>(-64))
	{
#if 1
//		sc=32767.0/(1<<e);
		sc=32767.0*(1<<(-e));
		xa=(int)(x*sc+0.5);
		ya=(int)(y*sc+0.5);
		za=(int)(z*sc+0.5);
		xa=(xa<(-32767))?(-32767):((xa>32767)?32767:xa);
		ya=(ya<(-32767))?(-32767):((ya>32767)?32767:ya);
		za=(za<(-32767))?(-32767):((za>32767)?32767:za);
		xa=xa&65535;
		ya=ya&65535;
		za=za&65535;
		t.v=0x7100000000000000ULL | (((u64)(e+128))<<48) |
			(((u64)za)<<32)|(((u64)ya)<<16)|xa;
#endif

#if 0
//		sc=32764.0*(1<<(-e));
		sc=8190.0*(1<<(-e));
		xa=((int)(x*sc+0.5))&16383;
		ya=((int)(y*sc+0.5))&16383;
		za=((int)(z*sc+0.5))&16383;
//		t.v=0x7200000000000000ULL | (((u64)(e+128))<<48) |
//			(((u64)za)<<32)|(((u64)ya)<<16)|xa;
		t.v=0x7801000000000000ULL | (((u64)(e+32))<<42) |
			(((u64)za)<<28)|(((u64)ya)<<14)|xa;
#endif
	}else
	{
		t.v=0x7100000000000000ULL;
//		t.v=0x7801000000000000ULL;
	}
	return(t);

//	float18 xt, yt, zt;
//	u32 xa, ya, za;
//	xt=dtyEncodeFloat18(x);
//	yt=dtyEncodeFloat18(y);
//	zt=dtyEncodeFloat18(z);
//	t.v=0x7200000000000000ULL | (((u64)zt.v)<<36)|(((u64)yt.v)<<18)|xt.v;
}

BGBDY_API dytf dytfPackVec4(float x, float y, float z, float w)
{
	float mx, my, mz, mw, m;
	dytf t;
	int xa, ya, za, wa;
	float sc;
	int e;

	mx=fabs(x);
	my=fabs(y);
	mz=fabs(z);
	mw=fabs(w);
	m=mx;
	if(my>m)m=my;
	if(mz>m)m=mz;
	if(mw>m)m=mw;

//	e=(int)(ceil(bgbdy_log2(m)));
	e=bgbdy_log2i(m);

	if(e>127)
	{
//		t.v=0x7802000000000000ULL | (255LL<<40);
		t.v=0x7200000000000000ULL | (255LL<<48);
	}else if(e>=0)
	{
#if 1
		sc=2047.0/(1<<e);
		xa=((int)(x*sc+0.5))&4095;
		ya=((int)(y*sc+0.5))&4095;
		za=((int)(z*sc+0.5))&4095;
		wa=((int)(w*sc+0.5))&4095;
		t.v=0x7200000000000000ULL | (((u64)(e+128))<<48) |
			(((u64)wa)<<36)|(((u64)za)<<24)|(((u64)ya)<<12)|xa;
#endif

#if 0
//		sc=(510>>e);
		sc=510.0/(1<<e);
		xa=((int)(x*sc+0.5))&1023;
		ya=((int)(y*sc+0.5))&1023;
		za=((int)(z*sc+0.5))&1023;
		wa=((int)(w*sc+0.5))&1023;
		t.v=0x7802000000000000ULL | (((u64)(e+128))<<40) |
			(((u64)wa)<<30)|(((u64)za)<<20)|(((u64)ya)<<10)|xa;
#endif
	}else if(e>(-128))
	{
#if 1
		sc=2047.0*(1<<(-e));
//		sc=2047.0/(1<<e);
		xa=((int)(x*sc+0.5))&4095;
		ya=((int)(y*sc+0.5))&4095;
		za=((int)(z*sc+0.5))&4095;
		wa=((int)(w*sc+0.5))&4095;
		t.v=0x7200000000000000ULL | (((u64)(e+128))<<48) |
			(((u64)wa)<<36)|(((u64)za)<<24)|(((u64)ya)<<12)|xa;
#endif

#if 0
		sc=510.0*(1<<(-e));
		xa=((int)(x*sc+0.5))&1023;
		ya=((int)(y*sc+0.5))&1023;
		za=((int)(z*sc+0.5))&1023;
		wa=((int)(w*sc+0.5))&1023;
		t.v=0x7802000000000000ULL | (((u64)(e+128))<<40) |
			(((u64)wa)<<30)|(((u64)za)<<20)|(((u64)ya)<<10)|xa;
#endif
	}else
	{
//		t.v=0x7802000000000000ULL;
		t.v=0x7200000000000000ULL;
	}
	return(t);

#if 0
	dytf t;
	float16 xt, yt, zt, wt;
	u32 xa, ya, za, wa;
	
	xt=dtyEncodeFloat16(x);
	yt=dtyEncodeFloat16(y);
	zt=dtyEncodeFloat16(z);
	wt=dtyEncodeFloat16(w);
	
	xa=(xt.v+1)>>2;
	ya=(yt.v+1)>>2;
	za=(zt.v+1)>>2;
	wa=(wt.v+1)>>2;
	
	t.v=0x7300000000000000ULL |
		(((u64)wa)<<42)|
		(((u64)za)<<28)|
		(ya<<14)|xa;
	return(t);
#endif
}

BGBDY_API dytf dytfPackQuat(float w, float x, float y, float z)
{
	float mx, my, mz, mw, m;
	dytf t;
	int xa, ya, za, wa;
	float sc;
	int e;

	mx=fabs(x);
	my=fabs(y);
	mz=fabs(z);
	mw=fabs(w);
	m=mx;
	if(my>m)m=my;
	if(mz>m)m=mz;
	if(mw>m)m=mw;

//	e=(int)(ceil(bgbdy_log2(m)));
	e=bgbdy_log2i(m);

	if(e>127)
	{
		t.v=0x7803000000000000ULL | (255LL<<40);
	}else if(e>=0)
	{
//		sc=(510>>e);
		sc=510.0/(1<<e);
		xa=((int)(x*sc+0.5))&1023;
		ya=((int)(y*sc+0.5))&1023;
		za=((int)(z*sc+0.5))&1023;
		wa=((int)(w*sc+0.5))&1023;
		t.v=0x7803000000000000ULL | (((u64)(e+128))<<40) |
			(((u64)wa)<<30)|(((u64)za)<<20)|(((u64)ya)<<10)|xa;
	}else if(e>(-128))
	{
		sc=510.0*(1<<(-e));
		xa=((int)(x*sc+0.5))&1023;
		ya=((int)(y*sc+0.5))&1023;
		za=((int)(z*sc+0.5))&1023;
		wa=((int)(w*sc+0.5))&1023;
		t.v=0x7803000000000000ULL | (((u64)(e+128))<<40) |
			(((u64)wa)<<30)|(((u64)za)<<20)|(((u64)ya)<<10)|xa;
	}else
	{
		t.v=0x7803000000000000ULL;
	}
	return(t);
}

BGBDY_API dytf dytfPackVec2fv(float *v)
	{ return(dytfPackVec2(v[0], v[1])); }
BGBDY_API dytf dytfPackVec3fv(float *v)
	{ return(dytfPackVec3(v[0], v[1], v[2])); }
BGBDY_API dytf dytfPackVec4fv(float *v)
	{ return(dytfPackVec4(v[0], v[1], v[2], v[3])); }
BGBDY_API dytf dytfPackQuatfv(float *v)
	{ return(dytfPackVec4(v[3], v[0], v[1], v[2])); }

BGBDY_API dytf dytfPackVec2w(vec2 v)
	{ return(dytfPackVec2(v2x(v), v2y(v))); }
BGBDY_API dytf dytfPackVec3w(vec3 v)
	{ return(dytfPackVec3(v3x(v), v3y(v), v3z(v))); }
BGBDY_API dytf dytfPackVec4w(vec4 v)
	{ return(dytfPackVec4(v4x(v), v4y(v), v4z(v), v4w(v))); }
BGBDY_API dytf dytfPackQuatw(quat v)
	{ return(dytfPackQuat(v4w(v), v4x(v), v4y(v), v4z(v))); }


BGBDY_API void dytfPackVec2vfv(dytf p, float *fv)
{
	u32 xa, ya;
	
	xa=(u32)((p.v)<<8);
	ya=((u32)(p.v>>16))&0xFFFFFF00UL;
	fv[0]=*(float *)(&xa);
	fv[1]=*(float *)(&ya);
}

BGBDY_API void dytfPackVec3vfv(dytf p, float *fv)
{
	float sc;
	int e, xa, ya, za;

	if(((int)(p.v>>48))==0x7801)
	{
		xa=(int)((p.v>> 0)&16383);
		ya=(int)((p.v>>14)&16383);
		za=(int)((p.v>>28)&16383);
		if(xa&8192)xa|=(-1)<<14;
		if(ya&8192)ya|=(-1)<<14;
		if(za&8192)za|=(-1)<<14;

		e=((int)((p.v>>42)&63))-32;
		sc=pow(2, e-13)*(8192.0/8191.0);
		fv[0]=sc*xa;
		fv[1]=sc*ya;
		fv[2]=sc*za;
	}

	if(((int)(p.v>>56))==0x71)
	{
		xa=(int)((p.v>> 0)&65535);
		ya=(int)((p.v>>16)&65535);
		za=(int)((p.v>>32)&65535);
		if(xa&32768)xa|=(-1)<<16;
		if(ya&32768)ya|=(-1)<<16;
		if(za&32768)za|=(-1)<<16;

		e=((int)((p.v>>48)&255))-128;
		sc=pow(2, e-15)*(32768.0/32767.0);
		fv[0]=sc*xa;
		fv[1]=sc*ya;
		fv[2]=sc*za;
	}
}

BGBDY_API void dytfPackVec4vfv(dytf p, float *fv)
{
	float sc;
	int e, xa, ya, za, wa;

	if(((int)(p.v>>48))==0x7802)
	{
		xa=(int)((p.v>> 0)&1023);
		ya=(int)((p.v>>10)&1023);
		za=(int)((p.v>>20)&1023);
		wa=(int)((p.v>>30)&1023);
		if(xa&512)xa|=(-1)<<10;
		if(ya&512)ya|=(-1)<<10;
		if(za&512)za|=(-1)<<10;
		if(wa&512)wa|=(-1)<<10;

		e=((int)((p.v>>40)&255))-128;
		sc=pow(2, e-9)*(512.0/510.0);
		fv[0]=sc*xa;
		fv[1]=sc*ya;
		fv[2]=sc*za;
		fv[3]=sc*wa;
		
		return;
	}

	if(((int)(p.v>>56))==0x72)
	{
		xa=(int)((p.v>> 0)&4095);
		ya=(int)((p.v>>12)&4095);
		za=(int)((p.v>>24)&4095);
		wa=(int)((p.v>>36)&4095);
		if(xa&2048)xa|=(-1)<<16;
		if(ya&2048)ya|=(-1)<<16;
		if(za&2048)za|=(-1)<<16;
		if(wa&2048)wa|=(-1)<<16;

		e=((int)((p.v>>48)&255))-128;
		sc=pow(2, e-15)*(2048.0/2047.0);
		fv[0]=sc*xa;
		fv[1]=sc*ya;
		fv[2]=sc*za;
		fv[3]=sc*wa;
	}
}

BGBDY_API void dytfPackQuatvfv(dytf p, float *fv)
{
	dytfPackVec4vfv(p, fv);
}

BGBDY_API vec2 dytfPackVec2v(dytf p)
{
	float tv[4];
	dytfPackVec2vfv(p, tv);
	return(vec2(tv[0], tv[1]));
}

BGBDY_API vec3 dytfPackVec3v(dytf p)
{
	float tv[4];
	dytfPackVec3vfv(p, tv);
	return(vec3(tv[0], tv[1], tv[2]));
}

BGBDY_API vec4 dytfPackVec4v(dytf p)
{
	float tv[4];
	dytfPackVec4vfv(p, tv);
	return(vec4(tv[0], tv[1], tv[2], tv[3]));
}

BGBDY_API quat dytfPackQuatv(dytf p)
{
	float tv[4];
	dytfPackQuatvfv(p, tv);
	return(quat(tv[3], tv[0], tv[1], tv[2]));
}


BGBDY_API bool dytfPackVec2p(dytf p)
	{ return(((int)(p.v>>48))==0x7800); }
BGBDY_API bool dytfPackVec3p(dytf p)
	{ return(((int)(p.v>>48))==0x7801); }
BGBDY_API bool dytfPackVec4p(dytf p)
	{ return(((int)(p.v>>48))==0x7802); }
BGBDY_API bool dytfPackQuatp(dytf p)
	{ return(((int)(p.v>>48))==0x7803); }

