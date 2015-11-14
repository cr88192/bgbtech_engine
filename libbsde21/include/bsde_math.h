#ifndef BSDE_MATH_H
#define BSDE_MATH_H

#if !defined(BSDE_REAL32) && !defined(BSDE_REAL64)
#define BSDE_REAL32
#endif

#ifdef BSDE_REAL32
typedef float bsde_real;
#endif

#ifdef BSDE_REAL64
typedef double bsde_real;
#endif

#ifndef INLINE
#define INLINE
#endif

#define BSDE_M_PI		3.1415926535897932384626433832795
#define BSDE_M_BIGVAL		1000000000.0	//some large value
#define BSDE_M_SMALLVAL		0.0000000001	//some small value

#define DEG(x)		((x)*(BSDE_M_PI/180.0))
#define MIN(x, y)	(((x)<(y))?(x):(y))
#define MAX(x, y)	(((x)>(y))?(x):(y))


//macro varieties
#define V2_COPY(a, b)		(b)[0]=(a)[0];	\
				(b)[1]=(a)[1];
#define V3_COPY(a, b)		(b)[0]=(a)[0];	\
				(b)[1]=(a)[1];	\
				(b)[2]=(a)[2];
#define V4_COPY(a, b)		(b)[0]=(a)[0];	\
				(b)[1]=(a)[1];	\
				(b)[2]=(a)[2];	\
				(b)[3]=(a)[3];

#define V2_ADD(a, b, c)	(c)[0]=(a)[0]+(b)[0];	\
				(c)[1]=(a)[1]+(b)[1];
#define V3_ADD(a, b, c)	(c)[0]=(a)[0]+(b)[0];	\
				(c)[1]=(a)[1]+(b)[1];	\
				(c)[2]=(a)[2]+(b)[2];
#define V4_ADD(a, b, c)	(c)[0]=(a)[0]+(b)[0];	\
				(c)[1]=(a)[1]+(b)[1];	\
				(c)[2]=(a)[2]+(b)[2];	\
				(c)[3]=(a)[3]+(b)[3];

#define V2_SUB(a, b, c)	(c)[0]=(a)[0]-(b)[0];	\
				(c)[1]=(a)[1]-(b)[1];
#define V3_SUB(a, b, c)	(c)[0]=(a)[0]-(b)[0];	\
				(c)[1]=(a)[1]-(b)[1];	\
				(c)[2]=(a)[2]-(b)[2];
#define V4_SUB(a, b, c)	(c)[0]=(a)[0]-(b)[0];	\
				(c)[1]=(a)[1]-(b)[1];	\
				(c)[2]=(a)[2]-(b)[2];	\
				(c)[3]=(a)[3]-(b)[3];

#define V2_SCALE(a, b, c)	(c)[0]=(a)[0]*(b);	\
				(c)[1]=(a)[1]*(b);
#define V3_SCALE(a, b, c)	(c)[0]=(a)[0]*(b);	\
				(c)[1]=(a)[1]*(b);	\
				(c)[2]=(a)[2]*(b);
#define V4_SCALE(a, b, c)	(c)[0]=(a)[0]*(b);	\
				(c)[1]=(a)[1]*(b);	\
				(c)[2]=(a)[2]*(b);	\
				(c)[3]=(a)[3]*(b);

#define V2_SCALEVEC(a, b, c)	(c)[0]=(a)[0]*(b)[0];	\
				(c)[1]=(a)[1]*(b)[1];
#define V3_SCALEVEC(a, b, c)	(c)[0]=(a)[0]*(b)[0];	\
				(c)[1]=(a)[1]*(b)[1];	\
				(c)[2]=(a)[2]*(b)[2];
#define V4_SCALEVEC(a, b, c)	(c)[0]=(a)[0]*(b)[0];	\
				(c)[1]=(a)[1]*(b)[1];	\
				(c)[2]=(a)[2]*(b)[2];	\
				(c)[3]=(a)[3]*(b)[3];

#define V2_DOT(a, b)		((a)[0]*(b)[0]+(a)[1]*(b)[1])
#define V3_DOT(a, b)		((a)[0]*(b)[0]+(a)[1]*(b)[1]+(a)[2]*(b)[2])
#define V4_DOT(a, b)		((a)[0]*(b)[0]+(a)[1]*(b)[1]+ \
				(a)[2]*(b)[2]+(a)[3]*(b)[3])

#define V2_CROSS(a, b)		((a)[0]*(b)[1]-(a)[1]*(b)[0])
#define V3_CROSS(a, b, c)	(c)[0]=((a)[1]*(b)[2])-((a)[2]*(b)[1]);	\
				(c)[1]=((a)[2]*(b)[0])-((a)[0]*(b)[2]);	\
				(c)[2]=((a)[0]*(b)[1])-((a)[1]*(b)[0]);

#define V2_LEN(a)		(sqrt(V2_DOT(a, a)))
#define V3_LEN(a)		(sqrt(V3_DOT(a, a)))
#define V4_LEN(a)		(sqrt(V4_DOT(a, a)))

#define V1F_DIST2(a, b)		(((a)-(b))*((a)-(b)))
#define V2_DIST2(a, b)		(V1F_DIST2((a)[0], (b)[0])+ \
				V1F_DIST2((a)[1], (b)[1]))
#define V3_DIST2(a, b)		(V1F_DIST2((a)[0], (b)[0])+ \
				V1F_DIST2((a)[1], (b)[1])+ \
				V1F_DIST2((a)[2], (b)[2]))
#define V4_DIST2(a, b)		(V1F_DIST2((a)[0], (b)[0])+ \
				V1F_DIST2((a)[1], (b)[1])+ \
				V1F_DIST2((a)[2], (b)[2])+ \
				V1F_DIST2((a)[3], (b)[3]))

#define V2_DIST(a, b)		(sqrt(V2_DIST2(a, b)))
#define V3_DIST(a, b)		(sqrt(V3_DIST2(a, b)))
#define V4_DIST(a, b)		(sqrt(V4_DIST2(a, b)))

#define V2_ZERO(a)		(a)[0]=0; (a)[1]=0;
#define V3_ZERO(a)		(a)[0]=0; (a)[1]=0; (a)[2]=0;
#define V4_ZERO(a)		(a)[0]=0; (a)[1]=0; (a)[2]=0; (a)[3]=0;


#define V2_CONST(a, k)		(a)[0]=k; (a)[1]=k;
#define V3_CONST(a, k)		(a)[0]=k; (a)[1]=k; (a)[2]=k;
#define V4_CONST(a, k)		(a)[0]=k; (a)[1]=k; (a)[2]=k; (a)[3]=k;

#define V2_SET(a, x, y)	(a)[0]=x; (a)[1]=y;
#define V3_SET(a, x, y, z)	(a)[0]=x; (a)[1]=y; (a)[2]=z;
#define V4_SET(a, x, y, z, w)	(a)[0]=x; (a)[1]=y; (a)[2]=z; (a)[3]=w;

#define V2_ADDCONST(a, k, b)	(b)[0]=(a)[0]+(k); (b)[1]=(a)[1]+(k);
#define V3_ADDCONST(a, k, b)	(b)[0]=(a)[0]+(k); (b)[1]=(a)[1]+(k); \
				(b)[2]=(a)[2]+(k);
#define V4_ADDCONST(a, k, b)	(b)[0]=(a)[0]+(k); (b)[1]=(a)[1]+(k); \
				(b)[2]=(a)[2]+(k); (b)[3]=(a)[3]+(k);


#define V2_SUBCONST(a, k, b)	(b)[0]=(a)[0]-(k); (b)[1]=(a)[1]-(k);
#define V3_SUBCONST(a, k, b)	(b)[0]=(a)[0]-(k); (b)[1]=(a)[1]-(k); \
				(b)[2]=(a)[2]-(k);
#define V4_SUBCONST(a, k, b)	(b)[0]=(a)[0]-(k); (b)[1]=(a)[1]-(k); \
				(b)[2]=(a)[2]-(k); (b)[3]=(a)[3]-(k);


#define V2_ADDSCALE(a, b, c, d)	(d)[0]=(a)[0]+(b)[0]*(c);	\
					(d)[1]=(a)[1]+(b)[1]*(c);
#define V3_ADDSCALE(a, b, c, d)	(d)[0]=(a)[0]+(b)[0]*(c);	\
					(d)[1]=(a)[1]+(b)[1]*(c);	\
					(d)[2]=(a)[2]+(b)[2]*(c);
#define V4_ADDSCALE(a, b, c, d)	(d)[0]=(a)[0]+(b)[0]*(c);	\
					(d)[1]=(a)[1]+(b)[1]*(c);	\
					(d)[2]=(a)[2]+(b)[2]*(c);	\
					(d)[3]=(a)[3]+(b)[3]*(c);

#define V2_SCALEADDSCALE(a, b, c, d, e)	\
					(e)[0]=(a)[0]*(b)+(c)[0]*(d);	\
					(e)[1]=(a)[1]*(b)+(c)[1]*(d);
#define V3_SCALEADDSCALE(a, b, c, d, e)	\
					(e)[0]=(a)[0]*(b)+(c)[0]*(d);	\
					(e)[1]=(a)[1]*(b)+(c)[1]*(d);	\
					(e)[2]=(a)[2]*(b)+(c)[2]*(d);
#define V4_SCALEADDSCALE(a, b, c, d, e)	\
					(e)[0]=(a)[0]*(b)+(c)[0]*(d);	\
					(e)[1]=(a)[1]*(b)+(c)[1]*(d);	\
					(e)[2]=(a)[2]*(b)+(c)[2]*(d);	\
					(e)[3]=(a)[3]*(B)+(c)[3]*(d);

#include "bsde_math_dot.h"

#define V2_SUM(a)		((a)[0]+(a)[1])
#define V3_SUM(a)		((a)[0]+(a)[1]+(a)[2])
#define V4_SUM(a)		((a)[0]+(a)[1]+(a)[2]+(a)[3])

#define V2_INITMINS(a)		V2_CONST(a, BSDE_M_BIGVAL)
#define V2_INITMAXS(a)		V2_CONST(a, -BSDE_M_BIGVAL)
#define V3_INITMINS(a)		V3_CONST(a, BSDE_M_BIGVAL)
#define V3_INITMAXS(a)		V3_CONST(a, -BSDE_M_BIGVAL)
#define V4_INITMINS(a)		V4_CONST(a, BSDE_M_BIGVAL)
#define V4_INITMAXS(a)		V4_CONST(a, -BSDE_M_BIGVAL)

#define V2_INITBOX(m, n)	V2_INITMINS(m) V2_INITMAXS(n)
#define V3_INITBOX(m, n)	V3_INITMINS(m) V3_INITMAXS(n)
#define V4_INITBOX(m, n)	V4_INITMINS(m) V4_INITMAXS(n)

#define V2_MIN(a, b, c)	(c)[0]=MIN((a)[0], (b)[0]); \
				(c)[1]=MIN((a)[1], (b)[1]);
#define V3_MIN(a, b, c)	(c)[0]=MIN((a)[0], (b)[0]); \
				(c)[1]=MIN((a)[1], (b)[1]); \
				(c)[2]=MIN((a)[2], (b)[2]);
#define V4_MIN(a, b, c)	(c)[0]=MIN((a)[0], (b)[0]); \
				(c)[1]=MIN((a)[1], (b)[1]); \
				(c)[2]=MIN((a)[2], (b)[2]); \
				(c)[3]=MIN((a)[3], (b)[3]);
#define V2_MAX(a, b, c)	(c)[0]=MAX((a)[0], (b)[0]); \
				(c)[1]=MAX((a)[1], (b)[1]);
#define V3_MAX(a, b, c)	(c)[0]=MAX((a)[0], (b)[0]); \
				(c)[1]=MAX((a)[1], (b)[1]); \
				(c)[2]=MAX((a)[2], (b)[2]);
#define V4_MAX(a, b, c)	(c)[0]=MAX((a)[0], (b)[0]); \
				(c)[1]=MAX((a)[1], (b)[1]); \
				(c)[2]=MAX((a)[2], (b)[2]); \
				(c)[3]=MAX((a)[3], (b)[3]);

static INLINE bsde_real V2_NORMALIZE(bsde_real *a, bsde_real *b)
	{bsde_real f, g; f=V2_LEN(a); g=(f==0)?1:(1.0/f);
	V2_SCALE(a, g, b); return(f);}

static INLINE bsde_real V3_NORMALIZE(bsde_real *a, bsde_real *b)
	{bsde_real f, g; f=V3_LEN(a); g=(f==0)?1:(1.0/f);
	V3_SCALE(a, g, b); return(f);}

static INLINE bsde_real V4_NORMALIZE(bsde_real *a, bsde_real *b)
	{bsde_real f, g; f=V4_LEN(a); g=(f==0)?1:(1.0/f);
	V4_SCALE(a, g, b); return(f);}

#endif
