#include "bsde_internal.h"

int BSDE_SolidSphere_CollideContact(
	bsde_real arad, bsde_real *aorg, bsde_real brad, bsde_real *borg,
	bsde_real *org, bsde_real *norm, bsde_real *dist)
{
	bsde_real dv[3];
	bsde_real l, f;

	V3_SUB(aorg, borg, dv);
	l=V3_NORMALIZE(dv, dv);
	if(l>=(arad+brad))return(0);

//	l=V3_DOT(dv, dv);
//	f=arad*arad+brad*brad;
//	if(l>=f)return(0);

	l=V3_NORMALIZE(dv, dv);

	if(org)
	{
		f=(brad+(l-arad))*0.5;
		if(f<0)f=0;

		V3_ADDSCALE(borg, dv, f, org);
		V3_COPY(dv, norm);
		*dist=(arad+brad)-l;
	}

	return(1);
}

#if 0
int BSDE_SolidSphere_CollideObbContact(
	bsde_real arad, bsde_real *aorg,
	bsde_real *bmins, bsde_real *bmaxs, bsde_real *pos,
	bsde_real *org, bsde_real *norm, bsde_real *dist)
{
	bsde_real pt[3], dv[3];
	bsde_real d;

	BSDE_SolidOBB_ObbNearestPoint(bmins, bmaxs, pos, aorg, pt);
	V3_SUB(aorg, pt, dv);
	d=V3_NORMALIZE(dv, dv);
	if(d<0.0001)
	{
		dv[2]=1;
		pt[2]-=arad;
	}

	if(d>=arad)return(0);

	if(org)
	{
		V3_COPY(pt, org);
		V3_COPY(dv, norm);
		*dist=arad-d;
	}

	return(1);
}
#endif

int BSDE_SolidSphere_CollideObbContact(
	bsde_real arad, bsde_real *aorg,
	bsde_real *bmins, bsde_real *bmaxs, bsde_real *bpos,
	bsde_real *org, bsde_real *norm, bsde_real *dist)
{
	bsde_real pt[3], dv[3];
	bsde_real ext[4];
	bsde_real bn[3];
	bsde_real d0, d1, d, bd, bo;
	int i, j;

	BSDE_SolidOBB_ObbNearestPoint(bmins, bmaxs, bpos, aorg, pt);
	V3_SUB(aorg, pt, dv);
	d=V3_NORMALIZE(dv, dv);
	if(d<0.0001)
	{
		dv[2]=1;
		pt[2]-=arad;
	}

	if(d>=arad)return(0);

	V3_COPY(dv, bn);
	bd=arad-d;

	for(i=0; i<3; i++)
	{
		ext[0]=V3_DOT(aorg, bpos+i*4)-arad;
		ext[1]=V3_DOT(aorg, bpos+i*4)+arad;
		BSDE_SolidOBB_ObbPlaneExtents(bmins, bmaxs, bpos,
			bpos+i*4, ext+2);
		d0=(ext[0]>ext[2])?ext[0]:ext[2];
		d1=(ext[1]<ext[3])?ext[1]:ext[3];

		d=d1-d0;
		if(d<0)return(0);

		if(d<bd)
		{
			V3_COPY(bpos+i*4, bn);
			bd=d;
		}
	}

	if(org)
	{
		V3_COPY(pt, org);
		V3_NORMALIZE(bn, bn);
		V3_COPY(bn, norm);
		*dist=bd;
	}
	return(1);
}
