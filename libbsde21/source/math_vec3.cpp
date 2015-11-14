#include "bsde_internal.h"

void BSDE_Vec3F_ProjectPointOnPlane(bsde_real *d, bsde_real *v, bsde_real *z)
{
	bsde_real f;
	f=V3_DOT(v, z);
	V3_ADDSCALE(v, z, -f, d);
}

void BSDE_Vec3F_ProjectPointOnPlane2(bsde_real *d, bsde_real *v, bsde_real *z, bsde_real zn)
{
	bsde_real f;
	f=V3_DOT(v, z)-zn;
	V3_ADDSCALE(v, z, -f, d);
}

void BSDE_Vec3_ProjectPointOnPlane(bsde_real *d, bsde_real *v, bsde_real *z)
{
	bsde_real f;
	f=V3_DOT(v, z);
	V3_ADDSCALE(v, z, -f, d);
}

void BSDE_Vec3_ProjectPointOnPlane2(bsde_real *d, bsde_real *v, bsde_real *z, bsde_real zn)
{
	bsde_real f;
	f=V3_DOT(v, z)-zn;
	V3_ADDSCALE(v, z, -f, d);
}

bsde_real BSDE_Vec3F_DistLine(bsde_real *pt, bsde_real *v0, bsde_real *v1)
{
	bsde_real nv[3], tv0[3], tv1[3];

	V3_SUB(v1, v0, nv);
	V3_NORMALIZE(nv, nv);

	V3_ADDSCALE(v0, nv, -V3_DOT(v0, nv), tv0);
	V3_ADDSCALE(pt, nv, -V3_DOT(pt, nv), tv1);

	return(V3_DIST(tv0, tv1));
}

bsde_real BSDE_Vec3F_DistLineSeg(bsde_real *pt, bsde_real *v0, bsde_real *v1)
{
	bsde_real nv[3], tv0[3], tv1[3];
	bsde_real m, n, d;

	V3_SUB(v1, v0, nv);
	V3_NORMALIZE(nv, nv);

	m=V3_DOT(v0, nv);
	n=V3_DOT(v1, nv);
	d=V3_DOT(pt, nv);

	if(d<m)return(V3_DIST(pt, v0));
	if(d>n)return(V3_DIST(pt, v1));

	V3_ADDSCALE(v0, nv, -V3_DOT(v0, nv), tv0);
	V3_ADDSCALE(pt, nv, -V3_DOT(pt, nv), tv1);

	return(V3_DIST(tv0, tv1));
}

bsde_real BSDE_Vec3_DistLine(bsde_real *pt, bsde_real *v0, bsde_real *v1)
{
	bsde_real nv[3], tv0[3], tv1[3];

	V3_SUB(v1, v0, nv);
	V3_NORMALIZE(nv, nv);

	V3_ADDSCALE(v0, nv, -V3_DOT(v0, nv), tv0);
	V3_ADDSCALE(pt, nv, -V3_DOT(pt, nv), tv1);

	return(V3_DIST(tv0, tv1));
}

bsde_real BSDE_Vec3_DistLineSeg(bsde_real *pt, bsde_real *v0, bsde_real *v1)
{
	bsde_real nv[3], tv0[3], tv1[3];
	bsde_real m, n, d;

	V3_SUB(v1, v0, nv);
	V3_NORMALIZE(nv, nv);

	m=V3_DOT(v0, nv);
	n=V3_DOT(v1, nv);
	d=V3_DOT(pt, nv);

	if(d<m)return(V3_DIST(pt, v0));
	if(d>n)return(V3_DIST(pt, v1));

	V3_ADDSCALE(v0, nv, -V3_DOT(v0, nv), tv0);
	V3_ADDSCALE(pt, nv, -V3_DOT(pt, nv), tv1);

	return(V3_DIST(tv0, tv1));
}
