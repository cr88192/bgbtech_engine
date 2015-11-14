#include "bsde_internal.h"

int BSDE_SolidOBB3_CollideNormal(
	bsde_real *amins, bsde_real *amaxs, bsde_real *apos,
	bsde_real *bmins, bsde_real *bmaxs, bsde_real *bpos,
	bsde_real *norm, bsde_real *dist)
{
	bsde_real ext[4];
	bsde_real bn[4], v0[3], v1[3];
	bsde_real d0, d1, d, bd, f;
	int i, j, k;

	V4_ZERO(bn);
	bd=99999999.0;

	for(i=0; i<3; i++)
	{
		BSDE_SolidOBB_ObbPlaneExtents(amins, amaxs, apos,
			apos+i*4, ext+0);
		BSDE_SolidOBB_ObbPlaneExtents(bmins, bmaxs, bpos,
			apos+i*4, ext+2);
		d0=(ext[0]>ext[2])?ext[0]:ext[2];
		d1=(ext[1]<ext[3])?ext[1]:ext[3];

		d=d1-d0;
		if(d<0)return(0);


		j=(V3_DOT(apos+3*4, apos+i*4) >
			V3_DOT(bpos+3*4, apos+i*4));
		if(d<bd)
		{
			V3_SCALE(apos+i*4, j?1:-1, bn);
			bn[4]=j?amaxs[i]:amins[i];
			bd=d;
		}
	}

	for(i=0; i<3; i++)
	{
		BSDE_SolidOBB_ObbPlaneExtents(amins, amaxs, apos,
			bpos+i*4, ext+0);
		BSDE_SolidOBB_ObbPlaneExtents(bmins, bmaxs, bpos,
			bpos+i*4, ext+2);
		d0=(ext[0]>ext[2])?ext[0]:ext[2];
		d1=(ext[1]<ext[3])?ext[1]:ext[3];

		d=d1-d0;
		if(d<0)return(0);


		j=(V3_DOT(apos+3*4, bpos+i*4) >
			V3_DOT(bpos+3*4, bpos+i*4));
		if(d<bd)
		{
			V3_SCALE(bpos+i*4, j?1:-1, bn);
			bn[4]=j?bmaxs[i]:bmins[i];
			bd=d;
		}
	}

#if 1
	for(i=0; i<3; i++)
		for(j=0; j<3; j++)
	{
		V3_CROSS(apos+i*4, bpos+j*4, v0);
		V3_NORMALIZE(v0, v0);

		BSDE_SolidOBB_ObbPlaneExtents(amins, amaxs, apos, v0, ext+0);
		BSDE_SolidOBB_ObbPlaneExtents(bmins, bmaxs, bpos, v0, ext+2);
		d0=(ext[0]>ext[2])?ext[0]:ext[2];
		d1=(ext[1]<ext[3])?ext[1]:ext[3];

		d=d1-d0;
		if(d<0)return(0);

		k=(V3_DOT(apos+3*4, v0) >
			V3_DOT(bpos+3*4, v0));
		if(d<bd)
		{
			V3_SCALE(v0, k?1:-1, bn);
			bn[4]=0; //not really a plane
			bd=d;
		}
	}
#endif

	V4_COPY(bn, norm);
	*dist=bd;
	return(1);
}

void BSDE_SolidOBB3_GenPoints(
	bsde_real *mins, bsde_real *maxs, bsde_real *pos, bsde_real *pts)
{
	bsde_real v[3];
	int i, n;

	for(i=0; i<8; i++)
	{
		v[0]=(i&1)?maxs[0]:mins[0];
		v[1]=(i&2)?maxs[1]:mins[1];
		v[2]=(i&4)?maxs[2]:mins[2];
		BSDE_Plane_TransformPoint(v, pos, pts+i*3);
	}
}

void BSDE_SolidOBB3_GenPointsLocal(
	bsde_real *mins, bsde_real *maxs, bsde_real *pts)
{
	bsde_real v[3];
	int i, n;

	for(i=0; i<8; i++)
	{
		v[0]=(i&1)?maxs[0]:mins[0];
		v[1]=(i&2)?maxs[1]:mins[1];
		v[2]=(i&4)?maxs[2]:mins[2];
		V3_COPY(v, pts+i*3);
	}
}

void BSDE_SolidOBB3_GenNormsLocal(
	bsde_real *mins, bsde_real *maxs, bsde_real *norms)
{
	V4_SET(norms+0*4, -1,  0,  0, -mins[0]);
	V4_SET(norms+1*4,  1,  0,  0,  maxs[0]);
	V4_SET(norms+2*4,  0, -1,  0, -mins[1]);
	V4_SET(norms+3*4,  0,  1,  0,  maxs[1]);
	V4_SET(norms+4*4,  0,  0, -1, -mins[2]);
	V4_SET(norms+5*4,  0,  0,  1,  maxs[2]);
}


void BSDE_SolidOBB3_ObbBestNormalPoint(
	bsde_real *mins, bsde_real *maxs, bsde_real *pos,
	bsde_real *org, bsde_real *norm)
{
	bsde_real bn[3];
	bsde_real n[3], o[3];
	bsde_real d, f, bd;
	int i;

	V3_COPY(pos+12, o);

	V3_ZERO(bn);
	bd=99999999.0;

	for(i=0; i<3; i++)
	{
		V3_COPY(pos+(i*4), n);
		d=V3_DOT(org, n)-V3_DOT(o, n);

		f=d-mins[i];
		if(f<bd)
		{
			V3_SCALE(n, -1, bn);
			bd=f;
		}

		f=maxs[i]-d;
		if(f<bd)
		{
			V3_SCALE(n, 1, bn);
			bd=f;
		}
	}

	V3_NORMALIZE(bn, norm);
}

int BSDE_SolidOBB3_CheckPointCollide(
	bsde_real *mins, bsde_real *maxs, bsde_real *pos, bsde_real *pt)
{
	bsde_real r[3], v[3];

	V3_SUB(pt, pos+3*4, r);
	v[0]=V3_DOT(r, pos+0*3);
	v[1]=V3_DOT(r, pos+1*3);
	v[2]=V3_DOT(r, pos+2*3);
	if((v[0]<mins[0]) || (v[0]>maxs[0]))return(0);
	if((v[1]<mins[1]) || (v[1]>maxs[1]))return(0);
	if((v[2]<mins[2]) || (v[2]>maxs[2]))return(0);
	return(1);
}


int BSDE_SolidOBB3_CollideContact(
	bsde_real *amins, bsde_real *amaxs, bsde_real *apos,
	bsde_real *bmins, bsde_real *bmaxs, bsde_real *bpos,
	bsde_real *rorg, bsde_real *rnorm, bsde_real *rdist)
{
	bsde_real ext[4];
	bsde_real norm[4], ndist;
	bsde_real bp[3], bd, d, d0, d1;
	bsde_real *v0, *v1, f, g, tv0[3], tv1[3];
	int i, j, k;

	i=BSDE_SolidOBB3_CollideNormal(amins, amaxs, apos,
		bmins, bmaxs, bpos, norm, &ndist);
	if(!i)return(0);

	//else, generate a push in the general direction
	V3_SUB(apos+3*4, bpos+3*4, norm);
	V3_NORMALIZE(norm, norm);

	BSDE_SolidOBB_ObbPlaneExtents(amins, amaxs, apos, norm, ext+0);
	BSDE_SolidOBB_ObbPlaneExtents(bmins, bmaxs, bpos, norm, ext+2);
	d0=(ext[0]>ext[2])?ext[0]:ext[2];
	d1=(ext[1]<ext[3])?ext[1]:ext[3];
	if(d0>=d1)return(0);	//somehow, no collision

	d=(d0+d1)/2-V3_DOT(bpos+3*4, norm);
	V3_ADDSCALE(bpos+3*4, norm, d, rorg);
	V3_COPY(norm, rnorm);
	*rdist=d1-d0;
	return(1);
}


int BSDE_SolidOBB3_CollideAABBContact(
	bsde_real *amins, bsde_real *amaxs, bsde_real *apos,
	bsde_real *bmins, bsde_real *bmaxs, bsde_real *bpos,
	bsde_real *rorg, bsde_real *rnorm, bsde_real *rdist)
{
	bsde_real ext[4];
	bsde_real norm[4];
	bsde_real bd, d, d0, d1;
	int i, j, k;

	i=BSDE_SolidOBB3_CollideNormal(amins, amaxs, apos,
		bmins, bmaxs, bpos, norm, &d);
	if(!i)return(0);

	bd=999999999;
	V4_ZERO(norm);
	for(i=0; i<3; i++)
	{
		ext[0]=apos[3*4+i]+amins[i];
		ext[1]=apos[3*4+i]+amaxs[i];
		BSDE_SolidOBB_ObbPlaneExtents(bmins, bmaxs, bpos,
			apos+i*4, ext+2);
		d=V3_DOT(bpos+3*4, apos+i*4);

		if((ext[2]<ext[0]) && (ext[3]>ext[0]) && (d<ext[0]))
		{
			V3_ADDSCALE(norm, apos+i*4, d-ext[0], norm);
		}

		if((ext[2]<ext[1]) && (ext[3]>ext[1]) && (d>ext[1]))
		{
			V3_ADDSCALE(norm, apos+i*4, d-ext[1], norm);
		}
	}

	V3_NORMALIZE(norm, norm);

	BSDE_SolidOBB_ObbPlaneExtents(amins, amaxs, apos, norm, ext+0);
	BSDE_SolidOBB_ObbPlaneExtents(bmins, bmaxs, bpos, norm, ext+2);
	d0=(ext[0]>ext[2])?ext[0]:ext[2];
	d1=(ext[1]<ext[3])?ext[1]:ext[3];
	norm[3]=ext[1];

	BSDE_SolidOBB_ObbPlaneIntersect(bmins, bmaxs, bpos, norm, rorg);
	V3_SCALE(norm, -1, rnorm);
	*rdist=d1-d0;
	return(1);
}
