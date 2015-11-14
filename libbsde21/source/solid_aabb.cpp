#include "bsde_internal.h"

//Generic AABB stuff

void BSDE_SolidAABB_BoxMove(bsde_real *bmins, bsde_real *bmaxs,
	bsde_real *start, bsde_real *end, bsde_real *mins, bsde_real *maxs)
{
	bsde_real mins2[3], maxs2[3];
	int i;

	V3_ADD(bmins, start, mins);
	V3_ADD(bmaxs, start, maxs);
	V3_ADD(bmins, end, mins2);
	V3_ADD(bmaxs, end, maxs2);

	for(i=0; i<3; i++)
	{
		if(mins2[i]<mins[i])mins[i]=mins2[i];
		if(maxs2[i]>maxs[i])maxs[i]=maxs2[i];
	}
}

int BSDE_SolidAABB_BoxCollideP(bsde_real *ma, bsde_real *na, bsde_real *mb, bsde_real *nb)
{
	if(na[0]<=mb[0])return(0);
	if(na[1]<=mb[1])return(0);
	if(na[2]<=mb[2])return(0);
	if(nb[0]<=ma[0])return(0);
	if(nb[1]<=ma[1])return(0);
	if(nb[2]<=ma[2])return(0);

	return(1);
}

int BSDE_SolidAABB_BoxCollideAxis(bsde_real *ma, bsde_real *na, bsde_real *mb, bsde_real *nb)
{
	int i;

	i=0;
	if((na[0]>mb[0]) && !(ma[0]>=nb[0]))i|=1;
	if((na[1]>mb[1]) && !(ma[1]>=nb[1]))i|=2;
	if((na[2]>mb[2]) && !(ma[2]>=nb[2]))i|=4;

	if((nb[0]>ma[0]) && !(mb[0]>=na[0]))i|=8;
	if((nb[1]>ma[1]) && !(mb[1]>=na[1]))i|=16;
	if((nb[2]>ma[2]) && !(mb[2]>=na[2]))i|=32;

	return(i);
}

int BSDE_SolidAABB_PointInBoxP(bsde_real *p, bsde_real *min, bsde_real *max)
{
	if(p[0]<=min[0])return(0);
	if(p[0]>=max[0])return(0);
	if(p[1]<=min[1])return(0);
	if(p[1]>=max[1])return(0);
	if(p[2]<=min[2])return(0);
	if(p[2]>=max[2])return(0);

	return(1);
}

void BSDE_SolidAABB_BoxIntersection(bsde_real *ma, bsde_real *na,
	bsde_real *mb, bsde_real *nb, bsde_real *mc, bsde_real *nc)
{
	mc[0]=(ma[0]>mb[0])?ma[0]:mb[0];
	nc[0]=(na[0]<nb[0])?na[0]:nb[0];
	mc[1]=(ma[1]>mb[1])?ma[1]:mb[1];
	nc[1]=(na[1]<nb[1])?na[1]:nb[1];
	mc[2]=(ma[2]>mb[2])?ma[2]:mb[2];
	nc[2]=(na[2]<nb[2])?na[2]:nb[2];
}

int BSDE_SolidAABB_BoxCollideContact(bsde_real *ma, bsde_real *na,
	bsde_real *mb, bsde_real *nb, bsde_real *org, bsde_real *norm, bsde_real *dist)
{
	bsde_real mc[3], nc[3];
	bsde_real bd, d;
	int i, bi;

	BSDE_SolidAABB_BoxIntersection(ma, na, mb, nb, mc, nc);
	bd=99999999.0;
	bi=-1;

	for(i=0; i<3; i++)
	{
		d=nc[i]-mc[i];
		if(d<0)return(0);	//does not collide
//		if(d<bd)
		if((0<d) && (d<bd))
		{
			bi=i;
			bd=d;
		}
	}
	if(bi<0)
	{
		bi=2;
		bd=0;
	}

	org[0]=(mc[0]+nc[0])*0.5;
	org[1]=(mc[1]+nc[1])*0.5;
	org[2]=(mc[2]+nc[2])*0.5;

	V3_ZERO(norm);
	i=(ma[bi]<mb[bi])?-1:1;
	norm[bi]=i;
	*dist=bd;

	return(1);
}

void BSDE_SolidAABB_BoxPlaneExtents(bsde_real *mins, bsde_real *maxs,
	bsde_real *norm, bsde_real *min, bsde_real *max)
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

	*min=d1;
	*max=d0;
}

int BSDE_SolidAABB_BoxLineCollideP(bsde_real *min, bsde_real *max,
	bsde_real *sp, bsde_real *ep)
{
	bsde_real n[4];

	//eliminate if non-overlap
	if((sp[0]<min[0]) && (ep[0]<min[0]))return(0);
	if((sp[1]<min[1]) && (ep[1]<min[1]))return(0);
	if((sp[2]<min[2]) && (ep[2]<min[2]))return(0);
	if((sp[0]>max[0]) && (ep[0]>max[0]))return(0);
	if((sp[1]>max[1]) && (ep[1]>max[1]))return(0);
	if((sp[2]>max[2]) && (ep[2]>max[2]))return(0);

	n[0]=1; n[1]=0; n[2]=0; n[3]=min[0];
	if(sp[0]<min[0])BSDE_Plane_LinePlaneIntersect(sp, ep, n, sp);
	if(ep[0]<min[0])BSDE_Plane_LinePlaneIntersect(sp, ep, n, ep);

	n[0]=1; n[1]=0; n[2]=0; n[3]=max[0];
	if(sp[0]>max[0])BSDE_Plane_LinePlaneIntersect(sp, ep, n, sp);
	if(ep[0]>max[0])BSDE_Plane_LinePlaneIntersect(sp, ep, n, ep);

	if((sp[1]<min[1]) && (ep[1]<min[1]))return(0);
	if((sp[2]<min[2]) && (ep[2]<min[2]))return(0);
	if((sp[1]>max[1]) && (ep[1]>max[1]))return(0);
	if((sp[2]>max[2]) && (ep[2]>max[2]))return(0);

	n[0]=0; n[1]=1; n[2]=0; n[3]=min[1];
	if(sp[1]<min[1])BSDE_Plane_LinePlaneIntersect(sp, ep, n, sp);
	if(ep[1]<min[1])BSDE_Plane_LinePlaneIntersect(sp, ep, n, ep);

	n[0]=0; n[1]=1; n[2]=0; n[3]=max[1];
	if(sp[1]>max[1])BSDE_Plane_LinePlaneIntersect(sp, ep, n, sp);
	if(ep[1]>max[1])BSDE_Plane_LinePlaneIntersect(sp, ep, n, ep);

	if((sp[2]<min[2]) && (ep[2]<min[2]))return(0);
	if((sp[2]>max[2]) && (ep[2]>max[2]))return(0);

	return(1);
}
