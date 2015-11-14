#ifndef VECNF_H
#define VECNF_H

#include <bgbgc_conf.h>
// #include <bgbgc.h>
#include <math.h>

#ifndef dytAlign
#if defined(_MSC_VER) || defined(_BGBMETA)
#define dytAlign(x) __declspec(align(x))
#else
#ifdef __GNUC__
// #define dytAlign(x) __attribute__((align(x)))
#define dytAlign(x)
#endif
#endif
#endif

#if defined(__BSCC) || defined(_BGBMETA)
#include <vecnf_bscc.h>
#else
#if defined(MSVC)
#if defined(WIN64) || (_M_IX86_FP>=1)
#include <vecnf_msvc.h>
//#include <vecnf_rawd.h>
#include <vecnf_msvcd.h>
#else
#include <vecnf_raw.h>
#include <vecnf_rawd.h>
#endif
#else
#include <vecnf_raw.h>
#include <vecnf_rawd.h>
#endif
#endif


#ifdef __BSCC
static vec3 q_rot_v3f(quat a, vec3 b)
	{ return(qmul(qmul(a, (quat)b), qconj(a))); }
static vec4 q_rot_v4f(quat a, vec4 b)
	{ return(qmul(qmul(a, (quat)b), qconj(a))); }
#else
static vec3 q_rot_v3f(quat a, vec3 b)
	{ return(qmul(qmul(a, b), qconj(a))); }
static vec4 q_rot_v4f(quat a, vec4 b)
	{ return(qmul(qmul(a, b), qconj(a))); }
#endif

static float v3ndot(vec3 a, vec4 b)
	{ return(v3dot(a, v4xyz(b))-v4w(b)); }
static vec4 v4plane(vec3 xyz, float dist)
	{ return(vec4(v3x(xyz),v3y(xyz),v3z(xyz), dist)); }

static double v3dndot(vec3d a, vec4d b)
	{ return(v3ddot(a, v4dxyz(b))-v4dw(b)); }
static vec4d v4dplane(vec3d xyz, double dist)
	{ return(vec4d(v3dx(xyz), v3dy(xyz), v3dz(xyz), dist)); }

static vec2 v2lerp(vec2 a, vec2 b, float t)
	{ return(v2add(v2scale(a, 1.0-t), v2scale(b, t))); }
static vec3 v3lerp(vec3 a, vec3 b, float t)
	{ return(v3add(v3scale(a, 1.0-t), v3scale(b, t))); }
static vec4 v4lerp(vec4 a, vec4 b, float t)
	{ return(v4add(v4scale(a, 1.0-t), v4scale(b, t))); }

static vec2 v2nlerp(vec2 a, vec2 b, float t)
	{ return(v2norm(v2add(v2scale(a, 1.0-t), v2scale(b, t)))); }
static vec3 v3nlerp(vec3 a, vec3 b, float t)
	{ return(v3norm(v3add(v3scale(a, 1.0-t), v3scale(b, t)))); }
static vec4 v4nlerp(vec4 a, vec4 b, float t)
	{ return(v4norm(v4add(v4scale(a, 1.0-t), v4scale(b, t)))); }

static quat qlerp(quat a, quat b, float t)
	{ return(qadd(qscale(a, 1.0-t), qscale(b, t))); }
static quat qnlerp(quat a, quat b, float t)
	{ return(qnorm(qadd(qscale(a, 1.0-t), qscale(b, t)))); }

static float qarg(quat q)
	{ return(acos(qreal(q)/qlen(q))); }

static vec2d v2f2d(vec2 v)
	{ return(vec2d(v2x(v), v2y(v))); }
static vec3d v3f2d(vec3 v)
	{ return(vec3d(v3x(v), v3y(v), v3z(v))); }
static vec4d v4f2d(vec4 v)
	{ return(vec4d(v4x(v), v4y(v), v4z(v), v4w(v))); }
static quatd qf2d(quat v)
	{ return(quatd(qw(v), qx(v), qy(v), qz(v))); }

static vec2 v2d2f(vec2d v)
	{ return(vec2(v2dx(v), v2dy(v))); }
static vec3 v3d2f(vec3d v)
	{ return(vec3(v3dx(v), v3dy(v), v3dz(v))); }
static vec4 v4d2f(vec4d v)
	{ return(vec4(v4dx(v), v4dy(v), v4dz(v), v4dw(v))); }
static quat qd2f(quatd v)
	{ return(quat(qdw(v), qdx(v), qdy(v), qdz(v))); }

#define qsgn(q)		qnorm(q)

#define qln(q)		dtyQuatLn(q)
#define qexp(q)		dtyQuatExp(q)
#define qpow(p, q)	dtyQuatExpQuatQuat(p, q)
#define qpowsq(p, q)	dtyQuatExpRealQuat(p, q)
#define qpowqs(p, q)	dtyQuatExpQuatReal(p, q)

#define qsin(q)		dtyQuatSin(q)
#define qcos(q)		dtyQuatCos(q)
#define qtan(q)		dtyQuatTan(q)
#define qcot(q)		dtyQuatCot(q)

#define qsinh(q)	dtyQuatSinh(q)
#define qcosh(q)	dtyQuatCosh(q)
#define qtanh(q)	dtyQuatTanh(q)
#define qcoth(q)	dtyQuatCoth(q)

#define qasin(q)	dtyQuatASin(q)
#define qacos(q)	dtyQuatACos(q)
#define qatan(q)	dtyQuatATan(q)
#define qacot(q)	dtyQuatACot(q)


#define qslerp(p, q, t)	dtyQuatSlerp(p, q, t)

// #ifdef __cplusplus
#if 0

static vec2 operator=(vec2 &a, float *b)	{ a=vec2vf(b); return(a); }
static vec3 operator=(vec3 &a, float *b)	{ a=vec3vf(b); return(a); }
static vec4 operator=(vec4 &a, float *b)	{ a=vec4vf(b); return(a); }
static quat operator=(quat &a, float *b)	{ a=quatvf(b); return(a); }

static vec2 operator=(float &*a, vec2 b)
	{ vfvec2(a, b); return(b); }
static vec3 operator=(float &*a, vec3 b)
	{ vfvec3(a, b); return(b); }
static vec4 operator=(float &*a, vec4 b)
	{ vfvec4(a, b); return(b); }
static quat operator=(float &*a, quat b)
	{ vfquat(a, b); return(b); }

static vec2 operator+(vec2 a, vec2 b)	{ return(v2add(a, b)); }
static vec2 operator-(vec2 a, vec2 b)	{ return(v2sub(a, b)); }
static vec3 operator+(vec3 a, vec3 b)	{ return(v3add(a, b)); }
static vec3 operator-(vec3 a, vec3 b)	{ return(v3sub(a, b)); }
static vec4 operator+(vec4 a, vec4 b)	{ return(v4add(a, b)); }
static vec4 operator-(vec4 a, vec4 b)	{ return(v4sub(a, b)); }
static quat operator+(quat a, quat b)	{ return(qadd(a, b)); }
static quat operator-(quat a, quat b)	{ return(qsub(a, b)); }

static float operator*(vec2 a, vec2 b)	{ return(v2dot(a, b)); }
static float operator*(vec3 a, vec3 b)	{ return(v3dot(a, b)); }
static float operator*(vec4 a, vec4 b)	{ return(v4dot(a, b)); }

static vec2 operator*(vec2 a, float b)	{ return(v2scale(a, b)); }
static vec3 operator*(vec3 a, float b)	{ return(v3scale(a, b)); }
static vec4 operator*(vec4 a, float b)	{ return(v4scale(a, b)); }
static quat operator*(quat a, float b)	{ return(qscale(a, b)); }

static vec2 operator*(float a, vec2 b)	{ return(v2scale(b, a)); }
static vec3 operator*(float a, vec3 b)	{ return(v3scale(b, a)); }
static vec4 operator*(float a, vec4 b)	{ return(v4scale(b, a)); }
static quat operator*(float a, quat b)	{ return(qscale(b, a)); }

static vec2 operator/(vec2 a, float b)	{ return(v2scale(a, 1.0/b)); }
static vec3 operator/(vec3 a, float b)	{ return(v3scale(a, 1.0/b)); }
static vec4 operator/(vec4 a, float b)	{ return(v4scale(a, 1.0/b)); }
static quat operator/(quat a, float b)	{ return(qscale(a, 1.0/b)); }

static vec2 operator/(float a, vec2 b)	{ return(v2scale(b, 1.0/a)); }
static vec3 operator/(float a, vec3 b)	{ return(v3scale(b, 1.0/a)); }
static vec4 operator/(float a, vec4 b)	{ return(v4scale(b, 1.0/a)); }
static quat operator/(float a, quat b)	{ return(qscale(b, 1.0/a)); }

static quat operator*(quat a, quat b)	{ return(qmul(a, b)); }
static quat operator/(quat a, quat b)	{ return(qdiv(a, b)); }
static quat operator|(quat a, quat b)	{ return(qrdiv(a, b)); }

static float operator%(vec2 a, vec2 b)	{ return(v2cross(a, b)); }
static vec3 operator%(vec3 a, vec3 b)	{ return(v3cross(a, b)); }
static vec4 operator%(vec4 a, vec4 b)	{ return(v4cross(a, b)); }
static quat operator%(quat a, quat b)	{ return(qcross(a, b)); }

static vec2 operator^(vec2 a, vec2 b)	{ return(v2mul(a, b)); }
static vec3 operator^(vec3 a, vec3 b)	{ return(v3mul(a, b)); }
static vec4 operator^(vec4 a, vec4 b)	{ return(v4mul(a, b)); }

static quat operator^(quat a, quat b)	{ return(qexp(a, b)); }
static quat operator^(quat a, float b)	{ return(qexpqs(a, b)); }
static quat operator^(float a, quat b)	{ return(qexpsq(a, b)); }

// static vec2 operator*(vec2 a, vec2 b)	{ return(v2mul(a, b)); }
// static vec2 operator/(vec2 a, vec2 b)	{ return(v2div(a, b)); }
// static vec3 operator*(vec3 a, vec3 b)	{ return(v3mul(a, b)); }
// static vec3 operator/(vec3 a, vec3 b)	{ return(v3div(a, b)); }
// static vec4 operator*(vec4 a, vec4 b)	{ return(v4mul(a, b)); }
// static vec4 operator/(vec4 a, vec4 b)	{ return(v4div(a, b)); }

#endif

#endif
