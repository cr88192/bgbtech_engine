#ifndef PD3D_MATH_H
#define PD3D_MATH_H

#define INLINE
typedef float bsde_real;

#define PD3D_M_PI		3.1415926535897932384626433832795
#define PD3D_M_BIGVAL		1000000000.0	//some large value
#define PD3D_M_SMALLVAL		0.0000000001	//some small value

#define DEG(x)		((x)*(PD3D_M_PI/180.0))


//macro varieties
#define V2F_COPY(a, b)		(b)[0]=(a)[0];	\
				(b)[1]=(a)[1];
#define V3F_COPY(a, b)		(b)[0]=(a)[0];	\
				(b)[1]=(a)[1];	\
				(b)[2]=(a)[2];
#define V4F_COPY(a, b)		(b)[0]=(a)[0];	\
				(b)[1]=(a)[1];	\
				(b)[2]=(a)[2];	\
				(b)[3]=(a)[3];

#define V2F_ADD(a, b, c)	(c)[0]=(a)[0]+(b)[0];	\
				(c)[1]=(a)[1]+(b)[1];
#define V3F_ADD(a, b, c)	(c)[0]=(a)[0]+(b)[0];	\
				(c)[1]=(a)[1]+(b)[1];	\
				(c)[2]=(a)[2]+(b)[2];
#define V4F_ADD(a, b, c)	(c)[0]=(a)[0]+(b)[0];	\
				(c)[1]=(a)[1]+(b)[1];	\
				(c)[2]=(a)[2]+(b)[2];	\
				(c)[3]=(a)[3]+(b)[3];

#define V2F_SUB(a, b, c)	(c)[0]=(a)[0]-(b)[0];	\
				(c)[1]=(a)[1]-(b)[1];
#define V3F_SUB(a, b, c)	(c)[0]=(a)[0]-(b)[0];	\
				(c)[1]=(a)[1]-(b)[1];	\
				(c)[2]=(a)[2]-(b)[2];
#define V4F_SUB(a, b, c)	(c)[0]=(a)[0]-(b)[0];	\
				(c)[1]=(a)[1]-(b)[1];	\
				(c)[2]=(a)[2]-(b)[2];	\
				(c)[3]=(a)[3]-(b)[3];

#define V2F_SCALE(a, b, c)	(c)[0]=(a)[0]*(b);	\
				(c)[1]=(a)[1]*(b);
#define V3F_SCALE(a, b, c)	(c)[0]=(a)[0]*(b);	\
				(c)[1]=(a)[1]*(b);	\
				(c)[2]=(a)[2]*(b);
#define V4F_SCALE(a, b, c)	(c)[0]=(a)[0]*(b);	\
				(c)[1]=(a)[1]*(b);	\
				(c)[2]=(a)[2]*(b);	\
				(c)[3]=(a)[3]*(b);

#define V2F_SCALEVEC(a, b, c)	(c)[0]=(a)[0]*(b)[0];	\
				(c)[1]=(a)[1]*(b)[1];
#define V3F_SCALEVEC(a, b, c)	(c)[0]=(a)[0]*(b)[0];	\
				(c)[1]=(a)[1]*(b)[1];	\
				(c)[2]=(a)[2]*(b)[2];
#define V4F_SCALEVEC(a, b, c)	(c)[0]=(a)[0]*(b)[0];	\
				(c)[1]=(a)[1]*(b)[1];	\
				(c)[2]=(a)[2]*(b)[2];	\
				(c)[3]=(a)[3]*(b)[3];

#define V2F_DOT(a, b)		((a)[0]*(b)[0]+(a)[1]*(b)[1])
#define V3F_DOT(a, b)		((a)[0]*(b)[0]+(a)[1]*(b)[1]+(a)[2]*(b)[2])
#define V4F_DOT(a, b)		((a)[0]*(b)[0]+(a)[1]*(b)[1]+ \
				(a)[2]*(b)[2]+(a)[3]*(b)[3])

#define V2F_CROSS(a, b)		((a)[0]*(b)[1]-(a)[1]*(b)[0])
#define V3F_CROSS(a, b, c)	(c)[0]=((a)[1]*(b)[2])-((a)[2]*(b)[1]);	\
				(c)[1]=((a)[2]*(b)[0])-((a)[0]*(b)[2]);	\
				(c)[2]=((a)[0]*(b)[1])-((a)[1]*(b)[0]);

#define V2F_LEN(a)		(sqrt(V2F_DOT(a, a)))
#define V3F_LEN(a)		(sqrt(V3F_DOT(a, a)))
#define V4F_LEN(a)		(sqrt(V4F_DOT(a, a)))

#define V1F_DIST2(a, b)		(((a)-(b))*((a)-(b)))
#define V2F_DIST2(a, b)		(V1F_DIST2((a)[0], (b)[0])+ \
				V1F_DIST2((a)[1], (b)[1]))
#define V3F_DIST2(a, b)		(V1F_DIST2((a)[0], (b)[0])+ \
				V1F_DIST2((a)[1], (b)[1])+ \
				V1F_DIST2((a)[2], (b)[2]))
#define V4F_DIST2(a, b)		(V1F_DIST2((a)[0], (b)[0])+ \
				V1F_DIST2((a)[1], (b)[1])+ \
				V1F_DIST2((a)[2], (b)[2])+ \
				V1F_DIST2((a)[3], (b)[3]))

#define V2F_DIST(a, b)		(sqrt(V2F_DIST2(a, b)))
#define V3F_DIST(a, b)		(sqrt(V3F_DIST2(a, b)))
#define V4F_DIST(a, b)		(sqrt(V4F_DIST2(a, b)))


#define V2F_ZERO(a)		(a)[0]=0; (a)[1]=0;


#define V3F_ZERO(a)		(a)[0]=0; (a)[1]=0; (a)[2]=0;


#define V4F_ZERO(a)		(a)[0]=0; (a)[1]=0; (a)[2]=0; (a)[3]=0;
 
#define V2F_CONST(a, k)		(a)[0]=k; (a)[1]=k;


#define V3F_CONST(a, k)		(a)[0]=k; (a)[1]=k; (a)[2]=k;


#define V4F_CONST(a, k)		(a)[0]=k; (a)[1]=k; (a)[2]=k; (a)[3]=k;


#define V2F_ADDCONST(a, k)	(a)[0]+=k; (a)[1]+=k;


#define V3F_ADDCONST(a, k)	(a)[0]+=k; (a)[1]+=k; (a)[2]+=k;


#define V4F_ADDCONST(a, k)	(a)[0]+=k; (a)[1]+=k; (a)[2]+=k; (a)[3]+=k;


#define V2F_SUBCONST(a, k)	(a)[0]-=k; (a)[1]-=k;


#define V3F_SUBCONST(a, k)	(a)[0]-=k; (a)[1]-=k; (a)[2]-=k;


#define V4F_SUBCONST(a, k)	(a)[0]-=k; (a)[1]-=k; (a)[2]-=k; (a)[3]-=k;

#define V2F_ADDSCALE(a, b, c, d)	(d)[0]=(a)[0]+(b)[0]*(c);	\
					(d)[1]=(a)[1]+(b)[1]*(c);
#define V3F_ADDSCALE(a, b, c, d)	(d)[0]=(a)[0]+(b)[0]*(c);	\
					(d)[1]=(a)[1]+(b)[1]*(c);	\
					(d)[2]=(a)[2]+(b)[2]*(c);
#define V4F_ADDSCALE(a, b, c, d)	(d)[0]=(a)[0]+(b)[0]*(c);	\
					(d)[1]=(a)[1]+(b)[1]*(c);	\
					(d)[2]=(a)[2]+(b)[2]*(c);	\
					(d)[3]=(a)[3]+(b)[3]*(c);

#define V2F_SCALEADDSCALE(a, b, c, d, e) \
					(e)[0]=(a)[0]*(b)+(c)[0]*(d);	\
					(e)[1]=(a)[1]*(b)+(c)[1]*(d);
#define V3F_SCALEADDSCALE(a, b, c, d, e) \
					(e)[0]=(a)[0]*(b)+(c)[0]*(d);	\
					(e)[1]=(a)[1]*(b)+(c)[1]*(d);	\
					(e)[2]=(a)[2]*(b)+(c)[2]*(d);
#define V4F_SCALEADDSCALE(a, b, c, d, e) \
					(e)[0]=(a)[0]*(b)+(c)[0]*(d);	\
					(e)[1]=(a)[1]*(b)+(c)[1]*(d);	\
					(e)[2]=(a)[2]*(b)+(c)[2]*(d);	\
					(e)[3]=(a)[3]*(b)+(c)[3]*(d);

#include "bsde_math_dot.h"

static INLINE bsde_real V2F_NORMALIZE(bsde_real *a, bsde_real *b)
	{bsde_real f, g; f=V2F_LEN(a); g=(f==0)?1:(1.0/f);
	V2F_SCALE(a, g, b); return(f);}

static INLINE bsde_real V3F_NORMALIZE(bsde_real *a, bsde_real *b)
	{bsde_real f, g; f=V3F_LEN(a); g=(f==0)?1:(1.0/f);
	V3F_SCALE(a, g, b); return(f);}

static INLINE bsde_real V4F_NORMALIZE(bsde_real *a, bsde_real *b)
	{bsde_real f, g; f=V4F_LEN(a); g=(f==0)?1:(1.0/f);
	V4F_SCALE(a, g, b); return(f);}

static INLINE bsde_real V2D_NORMALIZE(bsde_real *a, bsde_real *b)
	{bsde_real f, g; f=V2F_LEN(a); g=(f==0)?1:(1.0/f);
	V2F_SCALE(a, g, b); return(f);}
static INLINE bsde_real V3D_NORMALIZE(bsde_real *a, bsde_real *b)
	{bsde_real f, g; f=V3F_LEN(a); g=(f==0)?1:(1.0/f);
	V3F_SCALE(a, g, b); return(f);}
static INLINE bsde_real V4D_NORMALIZE(bsde_real *a, bsde_real *b)
	{bsde_real f, g; f=V4F_LEN(a); g=(f==0)?1:(1.0/f);
	V4F_SCALE(a, g, b); return(f);}


bsde_real *PD3D_Vec_AllocVec(int n);

static INLINE bsde_real *VEC2(bsde_real x, bsde_real y)
{
	bsde_real *v=PD3D_Vec_AllocVec(2);
	v[0]=x; v[1]=y;
	return(v);
}
static INLINE bsde_real *VEC3(bsde_real x, bsde_real y, bsde_real z)
{
	bsde_real *v=PD3D_Vec_AllocVec(3);
	v[0]=x; v[1]=y; v[2]=z;
	return(v);
}
static INLINE bsde_real *VEC4(bsde_real x, bsde_real y, bsde_real z, bsde_real w)
{
	bsde_real *v=PD3D_Vec_AllocVec(4);
	v[0]=x; v[1]=y; v[2]=z; v[3]=w;
	return(v);
}


static INLINE bsde_real *VA2(bsde_real *v0, bsde_real *v1)
{
	bsde_real *v=PD3D_Vec_AllocVec(2);
	v[0]=v0[0]+v1[0]; v[1]=v0[1]+v1[1];
	return(v);
}
static INLINE bsde_real *VA3(bsde_real *v0, bsde_real *v1)
{
	bsde_real *v=PD3D_Vec_AllocVec(3);
	v[0]=v0[0]+v1[0]; v[1]=v0[1]+v1[1]; v[2]=v0[2]+v1[2];
	return(v);
}
static INLINE bsde_real *VA4(bsde_real *v0, bsde_real *v1)
{
	bsde_real *v=PD3D_Vec_AllocVec(4);
	v[0]=v0[0]+v1[0]; v[1]=v0[1]+v1[1];
	v[2]=v0[2]+v1[2]; v[3]=v0[3]+v1[3];
	return(v);
}

static INLINE bsde_real *VS2(bsde_real *v0, bsde_real *v1)
{
	bsde_real *v=PD3D_Vec_AllocVec(2);
	v[0]=v0[0]-v1[0]; v[1]=v0[1]-v1[1];
	return(v);
}
static INLINE bsde_real *VS3(bsde_real *v0, bsde_real *v1)
{
	bsde_real *v=PD3D_Vec_AllocVec(3);
	v[0]=v0[0]-v1[0]; v[1]=v0[1]-v1[1]; v[2]=v0[2]-v1[2];
	return(v);
}
static INLINE bsde_real *VS4(bsde_real *v0, bsde_real *v1)
{
	bsde_real *v=PD3D_Vec_AllocVec(4);
	v[0]=v0[0]-v1[0]; v[1]=v0[1]-v1[1];
	v[2]=v0[2]-v1[2]; v[3]=v0[3]-v1[3];
	return(v);
}

static INLINE bsde_real *VM2(bsde_real *v0, bsde_real f)
{
	bsde_real *v=PD3D_Vec_AllocVec(2);
	v[0]=v0[0]*f; v[1]=v0[1]*f;
	return(v);
}
static INLINE bsde_real *VM3(bsde_real *v0, bsde_real f)
{
	bsde_real *v=PD3D_Vec_AllocVec(3);
	v[0]=v0[0]*f; v[1]=v0[1]*f; v[2]=v0[2]*f;
	return(v);
}
static INLINE bsde_real *VM4(bsde_real *v0, bsde_real f)
{
	bsde_real *v=PD3D_Vec_AllocVec(4);
	v[0]=v0[0]*f; v[1]=v0[1]*f;
	v[2]=v0[2]*f; v[3]=v0[3]*f;
	return(v);
}


static INLINE bsde_real VD2(bsde_real *v0, bsde_real *v1)
{
	return((v0[0]*v1[0])+(v0[1]*v1[1]));
}
static INLINE bsde_real VD3(bsde_real *v0, bsde_real *v1)
{
	return((v0[0]*v1[0])+(v0[1]*v1[1])+(v0[2]*v1[2]));
}
static INLINE bsde_real VD4(bsde_real *v0, bsde_real *v1)
{
	return((v0[0]*v1[0])+(v0[1]*v1[1])+(v0[2]*v1[2])+(v0[3]*v1[3]));
}


static INLINE bsde_real VX2(bsde_real *v0, bsde_real *v1)
{
	return((v0[0]*v1[1])-(v1[0]*v0[1]));
}

static INLINE bsde_real *VX3(bsde_real *v0, bsde_real *v1)
{
	bsde_real *v=PD3D_Vec_AllocVec(3);
	v[0]=(v0[1]*v1[2])-(v0[2]*v1[1]);
	v[1]=(v0[2]*v1[0])-(v0[0]*v1[2]);
	v[2]=(v0[0]*v1[1])-(v0[1]*v1[0]);
	return(v);
}

static INLINE bsde_real VL2(bsde_real *v)
{
	return(sqrt((v[0]*v[0])+(v[1]*v[1])));
}
static INLINE bsde_real VL3(bsde_real *v)
{
	return(sqrt((v[0]*v[0])+(v[1]*v[1])+(v[2]*v[2])));
}
static INLINE bsde_real VL4(bsde_real *v)
{
	return(sqrt((v[0]*v[0])+(v[1]*v[1])+(v[2]*v[2])+(v[3]*v[3])));
}

static INLINE void VC2(bsde_real *d, bsde_real *s)
{
	d[0]=s[0]; d[1]=s[1];
}
static INLINE void VC3(bsde_real *d, bsde_real *s)
{
	d[0]=s[0]; d[1]=s[1]; d[2]=s[2];
}
static INLINE void VC4(bsde_real *d, bsde_real *s)
{
	d[0]=s[0]; d[1]=s[1]; d[2]=s[2]; d[3]=s[3];
}

#endif
