#include "bsde_internal.h"

//OBB

int BSDE_SolidOBB_CheckObbPlane(bsde_real *mins, bsde_real *maxs,
	bsde_real *xform, bsde_real *norm, bsde_real *io)
{
	bsde_real w[3];

	bsde_real xwa, ywa, zwa;
	bsde_real m, n;
	int i;

	w[0]=V3_DOT(xform+(0*4), norm);
	w[1]=V3_DOT(xform+(1*4), norm);
	w[2]=V3_DOT(xform+(2*4), norm);

	xwa=xform[3*4+0]*norm[0];
	ywa=xform[3*4+1]*norm[1];
	zwa=xform[3*4+2]*norm[2];

	m=0;
	n=0;
	for(i=0; i<3; i++)
	{
		if(w[i]>0)
		{
			m+=mins[i]*w[i];
			n+=maxs[i]*w[i];
		}else
		{
			m+=maxs[i]*w[i];
			n+=mins[i]*w[i];
		}
	}
	m+=xwa+ywa+zwa-norm[3];
	n+=xwa+ywa+zwa-norm[3];

	i=0;
	if(m<0)i|=2;
	if(n>=0)i|=1;

	if(!i)
	{
		printf("BSDE_SolidOBB_CheckObbPlane: fail\n");

		printf("[(%f %f %f) (%f %f %f)]\n",
			mins[0], mins[1], mins[2],
			maxs[0], maxs[1], maxs[2]);

		printf("(%f %f %f %f)\n",
			norm[0], norm[1], norm[2], norm[3]);

		printf("%f %f\n", m, n);
	}

	if(io && (i==3))
		BSDE_SolidOBB_ObbPlaneIntersect(
			mins, maxs, xform, norm, io);

	return(i);
}

int BSDE_SolidOBB_ObbPlaneIntersect(bsde_real *mins, bsde_real *maxs,
	bsde_real *xform, bsde_real *norm, bsde_real *io)
{
	bsde_real pts[8*3];
	bsde_real bo[3];
	bsde_real d;
	int i, j;

	BSDE_SolidOBB_GenPoints(mins, maxs, xform, pts);

	V3_ZERO(bo);
	j=0;

	for(i=0; i<8; i++)
	{
		d=V3_NDOT(pts+i*3, norm);
		if(d<0)
		{
			V3_ADD(bo, pts+i*3, bo);
			j++;
		}
	}

	if(!j)j++;
	V3_SCALE(bo, 1.0/j, io);

	return(0);
}


int BSDE_SolidOBB_ObbPlaneExtents(bsde_real *mins, bsde_real *maxs,
	bsde_real *xform, bsde_real *norm, bsde_real *ext)
{
	bsde_real w[3];

	bsde_real xwa, ywa, zwa;
	bsde_real m, n;
	int i;

	w[0]=V3_DOT(xform+(0*4), norm);
	w[1]=V3_DOT(xform+(1*4), norm);
	w[2]=V3_DOT(xform+(2*4), norm);

	xwa=xform[3*4+0]*norm[0];
	ywa=xform[3*4+1]*norm[1];
	zwa=xform[3*4+2]*norm[2];

	m=0;
	n=0;

	for(i=0; i<3; i++)
	{
		if(w[i]>0)
		{
			m+=mins[i]*w[i];
			n+=maxs[i]*w[i];
		}else
		{
			m+=maxs[i]*w[i];
			n+=mins[i]*w[i];
		}
	}
	m+=xwa+ywa+zwa;
	n+=xwa+ywa+zwa;

	ext[0]=m;
	ext[1]=n;

	return(0);
}

void BSDE_SolidOBB_PlaneExtents(bsde_real *mins, bsde_real *maxs,
	bsde_real *xform, bsde_real *norm, bsde_real *min, bsde_real *max)
{
	bsde_real w[3];
	bsde_real m, n, d;
	int i;

	w[0]=V3_DOT(xform+(0*4), norm);
	w[1]=V3_DOT(xform+(1*4), norm);
	w[2]=V3_DOT(xform+(2*4), norm);
	d=V3_DOT(xform+3*4, norm);

	m=0;
	n=0;
	for(i=0; i<3; i++)
	{
		if(w[i]>0)
		{
			m+=mins[i]*w[i];
			n+=maxs[i]*w[i];
		}else
		{
			m+=maxs[i]*w[i];
			n+=mins[i]*w[i];
		}
	}
	*min=m+d;
	*max=n+d;
}

int BSDE_SolidOBB_ObbPlaneMoveExtents(bsde_real *mins, bsde_real *maxs,
	bsde_real *sxform, bsde_real *exform, bsde_real *norm, bsde_real *ext)
{
	bsde_real ext2[4];

	BSDE_SolidOBB_ObbPlaneExtents(mins, maxs, sxform, norm, ext2+0);
	BSDE_SolidOBB_ObbPlaneExtents(mins, maxs, exform, norm, ext2+2);

	ext[0]=(ext2[0]<ext2[2])?ext2[0]:ext2[2];
	ext[1]=(ext2[1]>ext2[3])?ext2[1]:ext2[3];

	return(0);
}

int BSDE_SolidOBB_CheckObbMovePlane(bsde_real *mins, bsde_real *maxs,
	bsde_real *sxform, bsde_real *exform, bsde_real *norm, bsde_real *io)
{
	bsde_real ext[2];
	int i;

	BSDE_SolidOBB_ObbPlaneMoveExtents(mins, maxs, sxform, exform,
		norm, ext);

	i=0;
	if((ext[0]-norm[3])<0)i|=2;
	if((ext[1]-norm[3])>=0)i|=1;

	if(io && (i==3))
		BSDE_SolidOBB_ObbPlaneIntersect(
			mins, maxs, exform, norm, io);

	return(i);
}

int BSDE_SolidOBB_CheckCollide(
	bsde_real *amins, bsde_real *amaxs, bsde_real *apos,
	bsde_real *bmins, bsde_real *bmaxs, bsde_real *bpos)
{
	int i, j;
	bsde_real ext[8], dir[3];
	bsde_real omins[3], omaxs[3];

	for(i=0; i<3; i++)
	{
		BSDE_SolidOBB_ObbPlaneExtents(amins, amaxs, apos,
			apos+i*4, ext+0);
		BSDE_SolidOBB_ObbPlaneExtents(bmins, bmaxs, bpos,
			apos+i*4, ext+2);
		if((ext[0]>=ext[3]) || (ext[2]>=ext[1]))
			return(0);
	}

	for(i=0; i<3; i++)
	{
		BSDE_SolidOBB_ObbPlaneExtents(amins, amaxs, apos,
			bpos+i*4, ext+0);
		BSDE_SolidOBB_ObbPlaneExtents(bmins, bmaxs, bpos,
			bpos+i*4, ext+2);
		if((ext[0]>=ext[3]) || (ext[2]>=ext[1]))
			return(0);

		omins[i]=(ext[0]>ext[2])?ext[0]:ext[2];	//greater min
		omaxs[i]=(ext[1]<ext[3])?ext[1]:ext[3];	//lesser max
	}

	for(i=0; i<3; i++)
		for(j=0; j<3; j++)
	{
		V3_CROSS(apos+i*4, bpos+j*4, dir);

		BSDE_SolidOBB_ObbPlaneExtents(amins, amaxs, apos,
			bpos+i*4, ext+0);
		BSDE_SolidOBB_ObbPlaneExtents(bmins, bmaxs, bpos,
			bpos+i*4, ext+2);
		if((ext[0]>=ext[3]) || (ext[2]>=ext[1]))
			return(0);
	}

	return(1);
}

int BSDE_SolidOBB_CheckObbMove(
	bsde_real *amins, bsde_real *amaxs, bsde_real *aspos, bsde_real *aepos,
	bsde_real *bmins, bsde_real *bmaxs, bsde_real *bspos, bsde_real *bepos,
	bsde_real *org)
{
	int i;
	bsde_real ext[8];
	bsde_real omins[3], omaxs[3], iorg[3];

#if 0
	for(i=0; i<3; i++)
	{
		BSDE_SolidOBB_ObbPlaneExtents(amins, amaxs, aspos,
			aspos+i*4, ext+0);
		BSDE_SolidOBB_ObbPlaneExtents(bmins, bmaxs, bspos,
			aspos+i*4, ext+4);
	}
#endif

	for(i=0; i<3; i++)
	{
		BSDE_SolidOBB_ObbPlaneExtents(amins, amaxs, aepos,
			aepos+i*4, ext+0);
		BSDE_SolidOBB_ObbPlaneExtents(bmins, bmaxs, bepos,
			aepos+i*4, ext+2);
		if((ext[0]>ext[3]) || (ext[2]>ext[1]))
			return(0);
	}

	for(i=0; i<3; i++)
	{
		BSDE_SolidOBB_ObbPlaneExtents(amins, amaxs, aepos,
			bepos+i*4, ext+0);
		BSDE_SolidOBB_ObbPlaneExtents(bmins, bmaxs, bepos,
			bepos+i*4, ext+2);
		if((ext[0]>ext[3]) || (ext[2]>ext[1]))
			return(0);

		omins[i]=(ext[0]>ext[2])?ext[0]:ext[2];	//greater min
		omaxs[i]=(ext[1]<ext[3])?ext[1]:ext[3];	//lesser max
	}


	for(i=0; i<3; i++)
		iorg[i]=(omins[i]+omaxs[i])*0.5;
	for(i=0; i<3; i++)
	{
		org[i]=iorg[0]*bepos[0*4+i]+
			iorg[1]*bepos[1*4+i]+
			iorg[2]*bepos[2*4+i]+
			bepos[3*4+i];
	}

	return(1);
}

int BSDE_SolidOBB_ObbImpactNormal(
	bsde_real *amins, bsde_real *amaxs, bsde_real *aspos, bsde_real *aepos,
	bsde_real *bmins, bsde_real *bmaxs, bsde_real *bspos, bsde_real *bepos,
	bsde_real *vel, bsde_real *norm)
{
	bsde_real rv[3], rn[3];
	bsde_real ext[2];
	int i;

	rv[0]=V3_DOT(vel, bepos+0*4);
	rv[1]=V3_DOT(vel, bepos+1*4);
	rv[2]=V3_DOT(vel, bepos+2*4);

	if(fabs(rv[0])>fabs(rv[1]))
	{
		if(fabs(rv[0])>fabs(rv[2]))
		{
			rn[0]=(rv[0]>0)?1:-1;
			rn[1]=0;
			rn[2]=0;
		}else
		{
			rn[0]=0;
			rn[1]=0;
			rn[2]=(rv[2]>0)?1:-1;
		}
	}else
	{
		if(fabs(rv[1])>fabs(rv[2]))
		{
			rn[0]=0;
			rn[1]=(rv[1]>0)?1:-1;
			rn[2]=0;
		}else
		{
			rn[0]=0;
			rn[1]=0;
			rn[2]=(rv[2]>0)?1:-1;
		}
	}

	for(i=0; i<3; i++)
	{
		norm[i]=rn[0]*bepos[0*4+i]+
			rn[1]*bepos[1*4+i]+
			rn[2]*bepos[2*4+i];
	}

	BSDE_SolidOBB_ObbPlaneExtents(bmins, bmaxs, bepos,
		norm, ext);
	norm[3]=ext[0];	//near extent

	return(0);
}

void BSDE_SolidOBB_Box(bsde_real *mins, bsde_real *maxs, bsde_real *pos,
	bsde_real *bmins, bsde_real *bmaxs)
{
	bsde_real norm[3];
	bsde_real ext[2];
	int i;

	for(i=0; i<3; i++)
	{
		V3_ZERO(norm);
		norm[i]=1;

		BSDE_SolidOBB_ObbPlaneExtents(mins, maxs, pos,
			norm, ext);
		bmins[i]=ext[0];
		bmaxs[i]=ext[1];
	}
}

void BSDE_SolidOBB_BoxMove(
	bsde_real *mins, bsde_real *maxs, bsde_real *spos, bsde_real *epos,
	bsde_real *bmins, bsde_real *bmaxs)
{
	bsde_real norm[3];
	bsde_real ext[2];
	int i;

	for(i=0; i<3; i++)
	{
		V3_ZERO(norm);
		norm[i]=1;

		BSDE_SolidOBB_ObbPlaneExtents(mins, maxs, spos,
			norm, ext);
		bmins[i]=ext[0];
		bmaxs[i]=ext[1];

		BSDE_SolidOBB_ObbPlaneExtents(mins, maxs, epos,
			norm, ext);
		if(ext[0]<bmins[i])bmins[i]=ext[0];
		if(ext[1]>bmaxs[i])bmaxs[i]=ext[1];
	}
}

int BSDE_SolidOBB_ObbPointCollide(
	bsde_real *mins, bsde_real *maxs, bsde_real *pos, bsde_real *org)
{
	bsde_real d;
	int i;

	for(i=0; i<3; i++)
	{
		d=V3_DOT(org, pos+(i*4))-pos[3*4+i];
		if(d<mins[i])return(0);
		if(d>maxs[i])return(0);
	}
	return(1);
}

void BSDE_SolidOBB_GenPoints(
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

void BSDE_SolidOBB_GenPoints4x(
	bsde_real *mins, bsde_real *maxs, bsde_real *pos, bsde_real *pts)
{
	bsde_real v[3], d[3];
	int i, j, k, n;

	d[0]=(maxs[0]-mins[0])/3.0;
	d[1]=(maxs[1]-mins[1])/3.0;
	d[2]=(maxs[2]-mins[2])/3.0;

	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
			for(k=0; k<4; k++)
	{
		v[0]=mins[0]+d[0]*i;
		v[1]=mins[1]+d[1]*j;
		v[2]=mins[2]+d[2]*k;
		BSDE_Plane_TransformPoint(v, pos, pts+(i*16+j*4+k)*3);
	}
}

void BSDE_SolidOBB_GenPointsNx(
	bsde_real *mins, bsde_real *maxs, bsde_real *pos, bsde_real *pts, int n)
{
	bsde_real v[3], d[3];
	int i, j, k, n2;

	d[0]=(maxs[0]-mins[0])/(n-1);
	d[1]=(maxs[1]-mins[1])/(n-1);
	d[2]=(maxs[2]-mins[2])/(n-1);

	n2=n*n;
	for(i=0; i<n; i++)
		for(j=0; j<n; j++)
			for(k=0; k<n; k++)
	{
		v[0]=mins[0]+d[0]*i;
		v[1]=mins[1]+d[1]*j;
		v[2]=mins[2]+d[2]*k;
		BSDE_Plane_TransformPoint(v, pos, pts+(i*n2+j*n+k)*3);
	}
}

void BSDE_SolidOBB_GenPointsCubeNx(
	bsde_real *mins, bsde_real *maxs, bsde_real *pts, int n)
{
	bsde_real v[3], d[3];
	int i, j, k, n2;

	d[0]=(maxs[0]-mins[0])/(n-1);
	d[1]=(maxs[1]-mins[1])/(n-1);
	d[2]=(maxs[2]-mins[2])/(n-1);

	n2=n*n;
	for(i=0; i<n; i++)
		for(j=0; j<n; j++)
			for(k=0; k<n; k++)
	{
		v[0]=mins[0]+d[0]*i;
		v[1]=mins[1]+d[1]*j;
		v[2]=mins[2]+d[2]*k;
		V3_COPY(v, pts+(i*n2+j*n+k)*3);
	}
}

int BSDE_SolidOBB_ClipPoints(
	bsde_real *mins, bsde_real *maxs, bsde_real *pos,
	bsde_real *ipts, bsde_real *opts, int npts)
{
	int i, j, k;

	k=0;
	for(i=0; i<npts; i++)
	{
		j=BSDE_SolidOBB_ObbPointCollide(
			mins, maxs, pos, ipts+i*3);

		if(j)
		{
			V3_COPY(ipts+i*3, opts+k*3);
			k++;
		}
	}
	return(k);
}

void BSDE_SolidOBB_ObbNearestPoint(
	bsde_real *mins, bsde_real *maxs, bsde_real *pos,
	bsde_real *org, bsde_real *pt)
{
	bsde_real n[3], o[3];
	bsde_real d;
	int i;

	V3_COPY(org, pt);
	V3_COPY(pos+12, o);

	for(i=0; i<3; i++)
	{
		V3_COPY(pos+(i*4), n);
		d=V3_DOT(pt, n)-V3_DOT(o, n);
		if(d<mins[i])
		{
			V3_ADDSCALE(pt, n, mins[i]-d, pt);
		}
		if(d>maxs[i])
		{
			V3_ADDSCALE(pt, n, maxs[i]-d, pt);
		}
	}
}
