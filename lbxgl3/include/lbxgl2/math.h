#ifndef LBXGL_MATH_H
#define LBXGL_MATH_H

#define BSDE_M_PI		3.1415926535897932384626433832795
#define BSDE_M_BIGVAL		1000000000.0	//some large value
#define BSDE_M_SMALLVAL		0.0000000001	//some small value

#define DEG(x)		((x)*(BSDE_M_PI/180.0))


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
#define V2D_COPY(a, b)		V2F_COPY(a, b)
#define V3D_COPY(a, b)		V3F_COPY(a, b)
#define V4D_COPY(a, b)		V4F_COPY(a, b)
#define V2D_ADD(a, b, c)	V2F_ADD(a, b, c)
#define V3D_ADD(a, b, c)	V3F_ADD(a, b, c)
#define V4D_ADD(a, b, c)	V4F_ADD(a, b, c)

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

#define V2F_SET(a, x, y)	(a)[0]=x; (a)[1]=y;
#define V3F_SET(a, x, y, z)	(a)[0]=x; (a)[1]=y; (a)[2]=z;
#define V4F_SET(a, x, y, z, w)	(a)[0]=x; (a)[1]=y; (a)[2]=z; (a)[3]=w;

#define V2D_SET(a, x, y)	(a)[0]=x; (a)[1]=y;
#define V3D_SET(a, x, y, z)	(a)[0]=x; (a)[1]=y; (a)[2]=z;
#define V4D_SET(a, x, y, z, w)	(a)[0]=x; (a)[1]=y; (a)[2]=z; (a)[3]=w;

#define V2F_ADDCONST(a, k, b)	(b)[0]=(a)[0]+(k); (b)[1]=(a)[1]+(k);
#define V3F_ADDCONST(a, k, b)	(b)[0]=(a)[0]+(k); (b)[1]=(a)[1]+(k); \
				(b)[2]=(a)[2]+(k);
#define V4F_ADDCONST(a, k, b)	(b)[0]=(a)[0]+(k); (b)[1]=(a)[1]+(k); \
				(b)[2]=(a)[2]+(k); (b)[3]=(a)[3]+(k);


#define V2F_SUBCONST(a, k, b)	(b)[0]=(a)[0]-(k); (b)[1]=(a)[1]-(k);
#define V3F_SUBCONST(a, k, b)	(b)[0]=(a)[0]-(k); (b)[1]=(a)[1]-(k); \
				(b)[2]=(a)[2]-(k);
#define V4F_SUBCONST(a, k, b)	(b)[0]=(a)[0]-(k); (b)[1]=(a)[1]-(k); \
				(b)[2]=(a)[2]-(k); (b)[3]=(a)[3]-(k);


#define V2F_ADDSCALE(a, b, c, d)	(d)[0]=(a)[0]+(b)[0]*(c);	\
					(d)[1]=(a)[1]+(b)[1]*(c);
#define V3F_ADDSCALE(a, b, c, d)	(d)[0]=(a)[0]+(b)[0]*(c);	\
					(d)[1]=(a)[1]+(b)[1]*(c);	\
					(d)[2]=(a)[2]+(b)[2]*(c);
#define V4F_ADDSCALE(a, b, c, d)	(d)[0]=(a)[0]+(b)[0]*(c);	\
					(d)[1]=(a)[1]+(b)[1]*(c);	\
					(d)[2]=(a)[2]+(b)[2]*(c);	\
					(d)[3]=(a)[3]+(b)[3]*(c);

#define V2F_SCALEADDSCALE(a, b, c, d, e)	\
					(e)[0]=(a)[0]*(b)+(c)[0]*(d);	\
					(e)[1]=(a)[1]*(b)+(c)[1]*(d);
#define V3F_SCALEADDSCALE(a, b, c, d, e)	\
					(e)[0]=(a)[0]*(b)+(c)[0]*(d);	\
					(e)[1]=(a)[1]*(b)+(c)[1]*(d);	\
					(e)[2]=(a)[2]*(b)+(c)[2]*(d);
#define V4F_SCALEADDSCALE(a, b, c, d, e)	\
					(e)[0]=(a)[0]*(b)+(c)[0]*(d);	\
					(e)[1]=(a)[1]*(b)+(c)[1]*(d);	\
					(e)[2]=(a)[2]*(b)+(c)[2]*(d);	\
					(e)[3]=(a)[3]*(B)+(c)[3]*(d);

#define V3F_NDOT(a, n)		((a)[0]*(n)[0]+(a)[1]*(n)[1]+ \
				(a)[2]*(n)[2]-(n)[3])
#define V3D_NDOT(a, n)		((a)[0]*(n)[0]+(a)[1]*(n)[1]+ \
				(a)[2]*(n)[2]-(n)[3])


#define V2D_COPY(a, b)		V2F_COPY(a, b)
#define V3D_COPY(a, b)		V3F_COPY(a, b)
#define V4D_COPY(a, b)		V4F_COPY(a, b)
#define V2D_ADD(a, b, c)	V2F_ADD(a, b, c)
#define V3D_ADD(a, b, c)	V3F_ADD(a, b, c)
#define V4D_ADD(a, b, c)	V4F_ADD(a, b, c)
#define V2D_SUB(a, b, c)	V2F_SUB(a, b, c)
#define V3D_SUB(a, b, c)	V3F_SUB(a, b, c)
#define V4D_SUB(a, b, c)	V4F_SUB(a, b, c)
#define V2D_SCALE(a, b, c)	V2F_SCALE(a, b, c)
#define V3D_SCALE(a, b, c)	V3F_SCALE(a, b, c)
#define V4D_SCALE(a, b, c)	V4F_SCALE(a, b, c)
#define V2D_SCALEVEC(a, b, c)	V2F_SCALEVEC(a, b, c)
#define V3D_SCALEVEC(a, b, c)	V3F_SCALEVEC(a, b, c)
#define V4D_SCALEVEC(a, b, c)	V4F_SCALEVEC(a, b, c)
#define V2D_DOT(a, b)		V2F_DOT(a, b)
#define V3D_DOT(a, b)		V3F_DOT(a, b)
#define V4D_DOT(a, b)		V4F_DOT(a, b)
#define V2D_CROSS(a, b)		V2F_CROSS(a, b)
#define V3D_CROSS(a, b, c)	V3F_CROSS(a, b, c)
#define V2D_LEN(a)		V2F_LEN(a)
#define V3D_LEN(a)		V3F_LEN(a)
#define V4D_LEN(a)		V4F_LEN(a)
#define V2D_DIST(a, b)		V2F_DIST(a, b)
#define V3D_DIST(a, b)		V3F_DIST(a, b)
#define V4D_DIST(a, b)		V4F_DIST(a, b)
#define V2D_ZERO(a)		V2F_ZERO(a)
#define V3D_ZERO(a)		V3F_ZERO(a)
#define V4D_ZERO(a)		V4F_ZERO(a)
#define V2D_CONST(a, k)		V2F_CONST(a, k)
#define V3D_CONST(a, k)		V3F_CONST(a, k)
#define V4D_CONST(a, k)		V4F_CONST(a, k)
#define V2D_ADDCONST(a, k, b)	V2F_ADDCONST(a, k, b)
#define V3D_ADDCONST(a, k, b)	V3F_ADDCONST(a, k, b)
#define V4D_ADDCONST(a, k, b)	V4F_ADDCONST(a, k, b)
#define V2D_SUBCONST(a, k, b)	V2F_SUBCONST(a, k, b)
#define V3D_SUBCONST(a, k, b)	V3F_SUBCONST(a, k, b)
#define V4D_SUBCONST(a, k, b)	V4F_SUBCONST(a, k, b)
#define V2D_ADDSCALE(a, b, c, d)	V2F_ADDSCALE(a, b, c, d)
#define V3D_ADDSCALE(a, b, c, d)	V3F_ADDSCALE(a, b, c, d)
#define V4D_ADDSCALE(a, b, c, d)	V4F_ADDSCALE(a, b, c, d)

#define V2D_SCALEADDSCALE(a, b, c, d, e)	\
	V2F_SCALEADDSCALE(a, b, c, d, e)
#define V3D_SCALEADDSCALE(a, b, c, d, e)	\
	V3F_SCALEADDSCALE(a, b, c, d, e)
#define V4D_SCALEADDSCALE(a, b, c, d, e)	\
	V4F_SCALEADDSCALE(a, b, c, d, e)

#define V2F_SUM(a)		((a)[0]+(a)[1])
#define V3F_SUM(a)		((a)[0]+(a)[1]+(a)[2])
#define V4F_SUM(a)		((a)[0]+(a)[1]+(a)[2]+(a)[3])


static inline float V2F_NORMALIZE(float *a, float *b)
	{float f, g; f=V2F_LEN(a); g=(f==0)?1:(1.0/f);
	V2F_SCALE(a, g, b); return(f);}

static inline float V3F_NORMALIZE(float *a, float *b)
	{float f, g; f=V3F_LEN(a); g=(f==0)?1:(1.0/f);
	V3F_SCALE(a, g, b); return(f);}

static inline float V4F_NORMALIZE(float *a, float *b)
	{float f, g; f=V4F_LEN(a); g=(f==0)?1:(1.0/f);
	V4F_SCALE(a, g, b); return(f);}

static inline double V2D_NORMALIZE(double *a, double *b)
	{double f, g; f=V2F_LEN(a); g=(f==0)?1:(1.0/f);
	V2F_SCALE(a, g, b); return(f);}
static inline double V3D_NORMALIZE(double *a, double *b)
	{double f, g; f=V3F_LEN(a); g=(f==0)?1:(1.0/f);
	V3F_SCALE(a, g, b); return(f);}
static inline double V4D_NORMALIZE(double *a, double *b)
	{double f, g; f=V4F_LEN(a); g=(f==0)?1:(1.0/f);
	V4F_SCALE(a, g, b); return(f);}

float *VecNF_AllocVec(int sz);
double *VecND_AllocVec(int sz);

static inline float *VEC2(float x, float y)
{
	float *tmp;
	tmp=VecNF_AllocVec(2);
	tmp[0]=x; tmp[1]=y;
	return(tmp);
}

static inline float *VEC3(float x, float y, float z)
{
	float *tmp;
	tmp=VecNF_AllocVec(3);
	tmp[0]=x; tmp[1]=y; tmp[2]=z;
	return(tmp);
}

static inline float *VEC4(float x, float y, float z, float w)
{
	float *tmp;
	tmp=VecNF_AllocVec(4);
	tmp[0]=x; tmp[1]=y; tmp[2]=z; tmp[3]=w;
	return(tmp);
}

static inline double *VEC2D(double x, double y)
{
	double *tmp;
	tmp=VecND_AllocVec(2);
	tmp[0]=x; tmp[1]=y;
	return(tmp);
}

static inline double *VEC3D(double x, double y, double z)
{
	double *tmp;
	tmp=VecND_AllocVec(3);
	tmp[0]=x; tmp[1]=y; tmp[2]=z;
	return(tmp);
}

static inline double *VEC4D(double x, double y, double z, double w)
{
	double *tmp;
	tmp=VecND_AllocVec(4);
	tmp[0]=x; tmp[1]=y; tmp[2]=z; tmp[3]=w;
	return(tmp);
}

static inline float *VA2(float *v0, float *v1)
{
	float *v=VecNF_AllocVec(2);
	v[0]=v0[0]+v1[0]; v[1]=v0[1]+v1[1];
	return(v);
}

static inline float *VA3(float *v0, float *v1)
{
	float *v=VecNF_AllocVec(3);
	v[0]=v0[0]+v1[0]; v[1]=v0[1]+v1[1]; v[2]=v0[2]+v1[2];
	return(v);
}
static inline float *VA4(float *v0, float *v1)
{
	float *v=VecNF_AllocVec(4);
	v[0]=v0[0]+v1[0]; v[1]=v0[1]+v1[1];
	v[2]=v0[2]+v1[2]; v[3]=v0[3]+v1[3];
	return(v);
}

static inline float *VS2(float *v0, float *v1)
{
	float *v=VecNF_AllocVec(2);
	v[0]=v0[0]-v1[0]; v[1]=v0[1]-v1[1];
	return(v);
}
static inline float *VS3(float *v0, float *v1)
{
	float *v=VecNF_AllocVec(3);
	v[0]=v0[0]-v1[0]; v[1]=v0[1]-v1[1]; v[2]=v0[2]-v1[2];
	return(v);
}
static inline float *VS4(float *v0, float *v1)
{
	float *v=VecNF_AllocVec(4);
	v[0]=v0[0]-v1[0]; v[1]=v0[1]-v1[1];
	v[2]=v0[2]-v1[2]; v[3]=v0[3]-v1[3];
	return(v);
}

static inline float *VM2(float *v0, float f)
{
	float *v=VecNF_AllocVec(2);
	v[0]=v0[0]*f; v[1]=v0[1]*f;
	return(v);
}
static inline float *VM3(float *v0, float f)
{
	float *v=VecNF_AllocVec(3);
	v[0]=v0[0]*f; v[1]=v0[1]*f; v[2]=v0[2]*f;
	return(v);
}
static inline float *VM4(float *v0, float f)
{
	float *v=VecNF_AllocVec(4);
	v[0]=v0[0]*f; v[1]=v0[1]*f;
	v[2]=v0[2]*f; v[3]=v0[3]*f;
	return(v);
}


static inline float VD2(float *v0, float *v1)
{
	return((v0[0]*v1[0])+(v0[1]*v1[1]));
}
static inline float VD3(float *v0, float *v1)
{
	return((v0[0]*v1[0])+(v0[1]*v1[1])+(v0[2]*v1[2]));
}
static inline float VD4(float *v0, float *v1)
{
	return((v0[0]*v1[0])+(v0[1]*v1[1])+(v0[2]*v1[2])+(v0[3]*v1[3]));
}


static inline float VX2(float *v0, float *v1)
{
	return((v0[0]*v1[1])-(v1[0]*v0[1]));
}

static inline float *VX3(float *v0, float *v1)
{
	float *v=VecNF_AllocVec(3);
	v[0]=(v0[1]*v1[2])-(v0[2]*v1[1]);
	v[1]=(v0[2]*v1[0])-(v0[0]*v1[2]);
	v[2]=(v0[0]*v1[1])-(v0[1]*v1[0]);
	return(v);
}

static inline float VL2(float *v)
{
	return(sqrt((v[0]*v[0])+(v[1]*v[1])));
}
static inline float VL3(float *v)
{
	return(sqrt((v[0]*v[0])+(v[1]*v[1])+(v[2]*v[2])));
}
static inline float VL4(float *v)
{
	return(sqrt((v[0]*v[0])+(v[1]*v[1])+(v[2]*v[2])+(v[3]*v[3])));
}

static inline void VC2(float *d, float *s)
{
	d[0]=s[0]; d[1]=s[1];
}
static inline void VC3(float *d, float *s)
{
	d[0]=s[0]; d[1]=s[1]; d[2]=s[2];
}
static inline void VC4(float *d, float *s)
{
	d[0]=s[0]; d[1]=s[1]; d[2]=s[2]; d[3]=s[3];
}

#endif
