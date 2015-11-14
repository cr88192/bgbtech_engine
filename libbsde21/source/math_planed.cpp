#include "bsde_internal.h"

void BSDE_Plane_LinePlaneIntersect(bsde_real *start, bsde_real *end,
	bsde_real *norm, bsde_real *point)
{
	bsde_real dir[3], x;

	//calc direction
	V3_SUB(end, start, dir);

	x=V3_DOT(dir, norm);
	x=(x<0)?-x:x;
	V3_SCALE(dir, 1/x, dir);

	//calc intersection
	x=V3_NDOT(start, norm);
	x=(x<0)?-x:x;
	V3_ADDSCALE(start, dir, x, point);
}

int BSDE_Plane_CheckLinePlane(bsde_real *start, bsde_real *end, bsde_real *norm)
{
	bsde_real x;
	int i;

	i=0;
	x=V3_NDOT(start, norm);
	if(x>=0)i|=1; else i|=2;

	x=V3_NDOT(end, norm);
	if(x>=0)i|=1; else i|=2;

	return(i);
}

int BSDE_Plane_ClipLinePlane(bsde_real *start, bsde_real *end,
	bsde_real *norm, bsde_real *end2)
{
	int i;

	i=BSDE_Plane_CheckLinePlane(start, end, norm);
	if(i!=3)return(i);

	BSDE_Plane_LinePlaneIntersect(start, end, norm, end2);
	return(i);
}

void BSDE_Plane_NearestPointLinePlane(bsde_real *start, bsde_real *end,
	bsde_real *norm, bsde_real *pt)
{
	bsde_real d0, d1;
	int i;

	i=BSDE_Plane_CheckLinePlane(start, end, norm);
	if(i==3)
	{
		BSDE_Plane_LinePlaneIntersect(start, end, norm, pt);
		return;
	}

	d0=V3_NDOT(start, norm);
	d1=V3_NDOT(end, norm);

	if(fabs(d0-d1)<0.001)
	{
		V3_SCALEADDSCALE(start, 0.5, end, 0.5, pt);
	}else if(fabs(d0)<fabs(d1))
	{
		V3_COPY(start, pt);
	}else
	{
		V3_COPY(end, pt);
	}
}

int BSDE_Plane_CheckPlaneBox(bsde_real *norm, bsde_real *mins, bsde_real *maxs)
{
	bsde_real d0, d1;
	int i;

	d0=0;
	d1=0;
	for(i=0; i<3; i++)
	{
		if(norm[i]>0)
		{
			d0+=maxs[i]*norm[i];
			d1+=mins[i]*norm[i];
		}else
		{
			d0+=mins[i]*norm[i];
			d1+=maxs[i]*norm[i];
		}
	}

	i=0;
	if(d0>=norm[3])i|=1;	//>=dist
	if(d1<norm[3])i|=2;	//<dist

	return(i);
}

void BSDE_Plane_TransformVector(bsde_real *dir, bsde_real *xform, bsde_real *v)
{
	v[0]=(dir[0]*xform[0])+(dir[1]*xform[4])+(dir[2]*xform[8]);
	v[1]=(dir[0]*xform[1])+(dir[1]*xform[5])+(dir[2]*xform[9]);
	v[2]=(dir[0]*xform[2])+(dir[1]*xform[6])+(dir[2]*xform[10]);
}

void BSDE_Plane_TransformVectorT(bsde_real *dir, bsde_real *xform, bsde_real *v)
{
	v[0]=(dir[0]*xform[0])+(dir[1]*xform[1])+(dir[2]*xform[2]);
	v[1]=(dir[0]*xform[4])+(dir[1]*xform[5])+(dir[2]*xform[6]);
	v[2]=(dir[0]*xform[8])+(dir[1]*xform[9])+(dir[2]*xform[10]);
}

void BSDE_Plane_TransformNormal(bsde_real *dir, bsde_real *xform, bsde_real *norm)
{
#ifdef BSDE_REAL32
	vec4 v;
	bsde_real f;

	v=v4scaleadd3(
		vec4vf(xform+0), dir[0],
		vec4vf(xform+4), dir[1],
		vec4vf(xform+8), dir[2]);
	f=v3dot(v, vec4vf(xform+12));
	vfvec4(norm, v);
	norm[3]=dir[3]+f;
#else
	bsde_real f;

//	BSDE_Plane_TransformVector(dir, xform, norm);

	norm[0]=(dir[0]*xform[0])+(dir[1]*xform[4])+(dir[2]*xform[8]);
	norm[1]=(dir[0]*xform[1])+(dir[1]*xform[5])+(dir[2]*xform[9]);
	norm[2]=(dir[0]*xform[2])+(dir[1]*xform[6])+(dir[2]*xform[10]);
	f=(norm[0]*xform[12])+(norm[1]*xform[13])+(norm[2]*xform[14]);
	norm[3]=dir[3]+f;
#endif
}

void BSDE_Plane_TransformNormalT(bsde_real *dir, bsde_real *xform, bsde_real *norm)
{
	bsde_real f;

//	BSDE_Plane_TransformVectorT(dir, xform, norm);
	norm[0]=(dir[0]*xform[0])+(dir[1]*xform[1])+(dir[2]*xform[2]);
	norm[1]=(dir[0]*xform[4])+(dir[1]*xform[5])+(dir[2]*xform[6]);
	norm[2]=(dir[0]*xform[8])+(dir[1]*xform[9])+(dir[2]*xform[10]);
	f=(dir[0]*xform[12])+(dir[1]*xform[13])+(dir[2]*xform[14]);
	norm[3]=dir[3]-f;
}

void BSDE_Plane_TransformNormalN(bsde_real *dir, bsde_real *xform, bsde_real *norm)
{
	bsde_real f;

//	BSDE_Plane_TransformVector(dir, xform, norm);
	norm[0]=(dir[0]*xform[0])+(dir[1]*xform[4])+(dir[2]*xform[8]);
	norm[1]=(dir[0]*xform[1])+(dir[1]*xform[5])+(dir[2]*xform[9]);
	norm[2]=(dir[0]*xform[2])+(dir[1]*xform[6])+(dir[2]*xform[10]);
	f=(norm[0]*xform[12])+(norm[1]*xform[13])+(norm[2]*xform[14]);
	norm[3]=dir[3]-f;
}

void BSDE_Plane_TransformNormalNT(bsde_real *dir, bsde_real *xform, bsde_real *norm)
{
	bsde_real f;

//	BSDE_Plane_TransformVectorT(dir, xform, norm);
	norm[0]=(dir[0]*xform[0])+(dir[1]*xform[1])+(dir[2]*xform[2]);
	norm[1]=(dir[0]*xform[4])+(dir[1]*xform[5])+(dir[2]*xform[6]);
	norm[2]=(dir[0]*xform[8])+(dir[1]*xform[9])+(dir[2]*xform[10]);
	f=(norm[0]*xform[12])+(norm[1]*xform[13])+(norm[2]*xform[14]);
	norm[3]=dir[3]-f;
}

void BSDE_Plane_TransformPoint(bsde_real *point, bsde_real *xform, bsde_real *org)
{
	bsde_real f;

	org[0]=(point[0]*xform[0])+(point[1]*xform[4])+
		(point[2]*xform[8])+xform[12];
	org[1]=(point[0]*xform[1])+(point[1]*xform[5])+
		(point[2]*xform[9])+xform[13];
	org[2]=(point[0]*xform[2])+(point[1]*xform[6])+
		(point[2]*xform[10])+xform[14];
}

void BSDE_Plane_TransformPointT(bsde_real *point, bsde_real *xform, bsde_real *org)
{
	bsde_real f;

	org[0]=(point[0]*xform[0])+(point[1]*xform[1])+
		(point[2]*xform[2])+xform[12];
	org[1]=(point[0]*xform[4])+(point[1]*xform[5])+
		(point[2]*xform[6])+xform[13];
	org[2]=(point[0]*xform[8])+(point[1]*xform[9])+
		(point[2]*xform[10])+xform[14];
}

void BSDE_Plane_TransformPointN(bsde_real *point, bsde_real *xform, bsde_real *org)
{
	bsde_real f;

	org[0]=(point[0]*xform[0])+(point[1]*xform[4])+
		(point[2]*xform[8])-xform[12];
	org[1]=(point[0]*xform[1])+(point[1]*xform[5])+
		(point[2]*xform[9])-xform[13];
	org[2]=(point[0]*xform[2])+(point[1]*xform[6])+
		(point[2]*xform[10])-xform[14];
}

void BSDE_Plane_TransformPointNT(bsde_real *point, bsde_real *xform, bsde_real *org)
{
	bsde_real f;

	org[0]=(point[0]*xform[0])+(point[1]*xform[1])+
		(point[2]*xform[2])-xform[12];
	org[1]=(point[0]*xform[4])+(point[1]*xform[5])+
		(point[2]*xform[6])-xform[13];
	org[2]=(point[0]*xform[8])+(point[1]*xform[9])+
		(point[2]*xform[10])-xform[14];
}

void BSDE_Plane_TransformPointLocal(bsde_real *point, bsde_real *xform, bsde_real *org)
{
	bsde_real pt[3], f;

	V3_SUB(point, xform+3*4, pt);
	org[0]=(pt[0]*xform[0])+(pt[1]*xform[1])+(pt[2]*xform[2]);
	org[1]=(pt[0]*xform[4])+(pt[1]*xform[5])+(pt[2]*xform[6]);
	org[2]=(pt[0]*xform[8])+(pt[1]*xform[9])+(pt[2]*xform[10]);

#if 0
	BSDE_Plane_TransformVectorT(xform+3*4, xform, dv);
	org[0]= (point[0]*xform[0]) +(point[1]*xform[1])+
		(point[2]*xform[2]) -dv[0];
	org[1]= (point[0]*xform[4]) +(point[1]*xform[5])+
		(point[2]*xform[6]) -dv[1];
	org[2]= (point[0]*xform[8]) +(point[1]*xform[9])+
		(point[2]*xform[10])-dv[2];
#endif
}

void BSDE_Plane_AdjacentNormals(bsde_real *n, bsde_real *x, bsde_real *y)
{
	bsde_real f, g;

	V3_ZERO(x);
	V3_ZERO(y);

	if(fabs(n[0])>fabs(n[1]))
	{
		if(fabs(n[0])>fabs(n[2]))
		{
			x[1]=1;
			y[2]=1;
		}else
		{
			x[0]=1;
			y[1]=1;
		}
	}else
	{
		if(fabs(n[1])>fabs(n[2]))
		{
			x[0]=1;
			y[2]=1;
		}else
		{
			x[0]=1;
			y[1]=1;
		}
	}

	f=V3_DOT(x, n);
	g=V3_DOT(y, n);
	V3_ADDSCALE(x, n, -f, x);
	V3_ADDSCALE(y, n, -g, y);

	V3_NORMALIZE(x, x);
	V3_NORMALIZE(y, y);
}

void BSDE_Plane_AdjacentNormals2(bsde_real *norm, bsde_real *udir, bsde_real *vdir)
{
	bsde_real f, g;

	V3_ZERO(udir);
	V3_ZERO(vdir);

	//UxV=N

	if(fabs(norm[0])>fabs(norm[1]))
	{
		if(fabs(norm[0])>fabs(norm[2]))
		{
			udir[(norm[0]>0)?1:2]=1; 
			vdir[(norm[0]>0)?2:1]=1;
		}else
		{
			udir[(norm[2]>0)?0:1]=1; 
			vdir[(norm[2]>0)?1:0]=1;
		}
	}else
	{
		if(fabs(norm[1])>fabs(norm[2]))
		{
			udir[(norm[1]>0)?2:0]=1; 
			vdir[(norm[1]>0)?0:2]=1;
		}else
		{
			udir[(norm[2]>0)?0:1]=1; 
			vdir[(norm[2]>0)?1:0]=1;
		}
	}

	f=V3_DOT(udir, norm);
	g=V3_DOT(vdir, norm);
	V3_ADDSCALE(udir, norm, -f, udir);
	V3_ADDSCALE(vdir, norm, -g, vdir);

	V3_NORMALIZE(udir, udir);
	V3_NORMALIZE(vdir, vdir);
}
