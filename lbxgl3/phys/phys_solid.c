/*
Physics stuff related to "solids".
A solid will be an arbitrarily oriented model defined in terms of brushes.

Represent each as arrays of pre-transformed planes, with sets of counts for
the planes for each brush.


Also possible is "solids" based on BRep models.
 */

#include <lbxgl2.h>


//BREP

#if 0
// int LBXGL_PhysSolid_CheckBrush(float *a, int na,
	float *bv, byte *bvc, int nb,
	float *io)
{
	float *v0, *v1;

	float d;
	int i, j, k, l, f;

#if 0
	v0=a;
	for(i=0; i<na; i++)
	{
		f=0;
		v1=bv;

		for(j=0; j<nb; j++)
			for(k=0; k<bvc[j]; k++)
		{
			d=0;
			for(l=0; l<3; l++)d+=v0[l]*v1[l];
			if(d<v0[3])f=1;

			v1+=3;
		}
		if(!f)return(0);
		v0+=4;
	}
#endif

	v0=bv;
	for(i=0; i<nb; i++)
		for(j=0; j<bvc[i]; j++)
	{
		f=1;
		v1=a;
		for(k=0; k<na; k++)
		{
			d=0;
			for(l=0; l<3; l++)d+=v0[l]*v1[l];
			if(d>=v1[3])f=0;

			v1+=4;
		}
		if(f)
		{
			VecNF_Copy(v0, io, 3);
			return(1);
		}
		v0+=3;
	}
	return(0);
}
#endif

//a/na: planes, b/nb: vertices
int LBXGL_PhysSolid_CheckBrush(float *a, int na, float *b, int nb,
	float *io)
{
	float *v0, *v1;

	float d;
	int i, j, k, l, f;

	v0=b;
	for(i=0; i<nb; i++)
	{
		f=1;
		v1=a;
		for(k=0; k<na; k++)
		{
			d=0;
			for(l=0; l<3; l++)d+=v0[l]*v1[l];
			if(d>=v1[3])f=0;

			v1+=4;
		}
		if(f)
		{
			VecNF_Copy(v0, io, 3);
			return(1);
		}
		v0+=3;
	}
	return(0);
}


int LBXGL_PhysSolid_CheckLineTriangle(float *v0, float *v1, float *v2,
	float *vs, float *ve)
{
}

int LBXGL_PhysSolid_CheckTriangleTriangle(
	float *u0, float *u1, float *u2,
	float *v0, float *v1, float *v2,
	float *io)
{
	float du0[3], du1[3], du2[4];
	float dv0[3], dv1[3], dv2[4];
	float iv[3];
	float d0, d1, d2;

	float m0, n0, m1, n1;

	VecNF_Subtract(u1, u0, du0, 3);
	VecNF_Subtract(u2, u0, du1, 3);
	Vec3F_CrossProduct(du0, du1, du2);
	du2[3]=VecNF_DotProduct(du2, u0, 3);

	d0=VecNF_DotProduct(du2, v0, 3)-du2[3];
	d1=VecNF_DotProduct(du2, v0, 3)-du2[3];
	d2=VecNF_DotProduct(du2, v0, 3)-du2[3];

	if(((d0*d1)>0) && ((d0*d2)>0))
		return(0);


	VecNF_Subtract(v1, v0, dv0, 3);
	VecNF_Subtract(v2, v0, dv1, 3);
	Vec3F_CrossProduct(dv0, dv1, dv2);
	dv2[3]=VecNF_DotProduct(dv2, v0, 3);

	d0=VecNF_DotProduct(dv2, u0, 3)-dv2[3];
	d1=VecNF_DotProduct(dv2, u0, 3)-dv2[3];
	d2=VecNF_DotProduct(dv2, u0, 3)-dv2[3];

	if(((d0*d1)>0) && ((d0*d2)>0))
		return(0);


	Vec3F_CrossProduct(du2, dv2, iv);
	VecNF_Normalize(iv, iv, 3);


	d0=VecNF_DotProduct(iv, u0, 3);
	d1=VecNF_DotProduct(iv, u1, 3);
	d2=VecNF_DotProduct(iv, u2, 3);
	m0=(d0<d1)?((d0<d2)?d0:d2):((d1<d2)?d1:d2);
	n0=(d0>d1)?((d0>d2)?d0:d2):((d1>d2)?d1:d2);

	d0=VecNF_DotProduct(iv, v0, 3);
	d1=VecNF_DotProduct(iv, v1, 3);
	d2=VecNF_DotProduct(iv, v2, 3);
	m1=(d0<d1)?((d0<d2)?d0:d2):((d1<d2)?d1:d2);
	n1=(d0>d1)?((d0>d2)?d0:d2):((d1>d2)?d1:d2);

	if((m0>n1) || (m1>n0))
		return(0);

	if(io)VecNF_Scale(iv, (m0+n0+m1+n1)/4.0, io, 3);

	return(1);
}

//check 2 transformed brep solids
//av: vertices, at: triangles, na: count
int LBXGL_PhysSolid_CheckBrep(
	float *av, int *at, int na,
	float *bv, int *bt, int nb,
	float *io)
{
	int i, j, k;

	for(i=0; i<na; i++)
		for(j=0; j<nb; j++)
	{
		k=LBXGL_PhysSolid_CheckTriangleTriangle(
			&av[at[i*3+0]*3], &av[at[i*3+1]*3], &av[at[i*3+2]*3],
			&bv[bt[j*3+0]*3], &bv[bt[j*3+1]*3], &bv[bt[j*3+2]*3],
			io);
		if(k)return(1);
	}
	return(0);
}


//OBB

int LBXGL_PhysSolid_CheckObbPlane(float *mins, float *maxs,
	float *xform, float *norm, float *io)
{
	float w[3];

	float xwa, ywa, zwa;
	float m, n, d;
	int i;

	w[0]=VecNF_DotProduct(xform+(0*4), norm, 3);
	w[1]=VecNF_DotProduct(xform+(1*4), norm, 3);
	w[2]=VecNF_DotProduct(xform+(2*4), norm, 3);

	xwa=xform[0*4+3]*norm[0];
	ywa=xform[1*4+3]*norm[1];
	zwa=xform[2*4+3]*norm[2];

	m=999999999;
	n=-999999999;
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
#if 0
		d=mins[i]*w[i];
		if(d<m)m=d;
		if(d>n)n=d;

		d=maxs[i]*w[i];
		if(d<m)m=d;
		if(d>n)n=d;
#endif
	}
	m+=xwa+ywa+zwa-norm[3];
	n+=xwa+ywa+zwa-norm[3];

	i=0;
	if(m<0)i|=2;
	if(n>=0)i|=1;

	if(io && (i==3))
		LBXGL_PhysSolid_ObbPlaneIntersect(
			mins, maxs, xform, norm, io);

	return(i);
}

int LBXGL_PhysSolid_ObbPlaneIntersect(float *mins, float *maxs,
	float *xform, float *norm, float *io)
{
	float dx[3], dy[3], dz[3], dorg[3], dc[3];

	dc[0]=(mins[0]+maxs[0])*0.5;
	dc[1]=(mins[1]+maxs[1])*0.5;
	dc[2]=(mins[2]+maxs[2])*0.5;

	dorg[0]=xform[0*4+3];
	dorg[1]=xform[0*4+3];
	dorg[2]=xform[0*4+3];
	VecNF_ProjectPointOnPlane2(dorg, dorg, norm, 3);

	VecNF_ProjectPointOnPlane(dx, xform+(0*4), norm, 3);
	VecNF_ProjectPointOnPlane(dy, xform+(1*4), norm, 3);
	VecNF_ProjectPointOnPlane(dz, xform+(2*4), norm, 3);


	io[0]=dorg[0]+(dc[0]*dx[0])+(dc[1]*dy[0])+(dc[2]*dz[0]);
	io[1]=dorg[1]+(dc[0]*dx[1])+(dc[1]*dy[1])+(dc[2]*dz[1]);
	io[2]=dorg[2]+(dc[0]*dx[2])+(dc[1]*dy[2])+(dc[2]*dz[2]);

	return(0);
}


int LBXGL_PhysSolid_ObbPlaneExtents(float *mins, float *maxs,
	float *xform, float *norm, float *ext)
{
	float w[3];

	float xwa, ywa, zwa;
	float m, n, d;
	int i;

	w[0]=VecNF_DotProduct(xform+(0*4), norm, 3);
	w[1]=VecNF_DotProduct(xform+(1*4), norm, 3);
	w[2]=VecNF_DotProduct(xform+(2*4), norm, 3);

	xwa=xform[0*4+3]*norm[0];
	ywa=xform[1*4+3]*norm[1];
	zwa=xform[2*4+3]*norm[2];

//	m=999999999;
//	n=-999999999;
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

#if 0
		d=mins[i]*w[i];
		if(d<m)m=d;
		if(d>n)n=d;

		d=maxs[i]*w[i];
		if(d<m)m=d;
		if(d>n)n=d;
#endif
	}
	m+=xwa+ywa+zwa;
	n+=xwa+ywa+zwa;

	ext[0]=m;
	ext[1]=n;

	return(0);
}

int LBXGL_PhysSolid_ObbPlaneMoveExtents(float *mins, float *maxs,
	float *sxform, float *exform, float *norm, float *ext)
{
	float ext2[4];

	LBXGL_PhysSolid_ObbPlaneExtents(mins, maxs, sxform, norm, ext2+0);
	LBXGL_PhysSolid_ObbPlaneExtents(mins, maxs, exform, norm, ext2+2);

	ext[0]=(ext2[0]<ext2[2])?ext2[0]:ext2[2];
	ext[1]=(ext2[1]>ext2[3])?ext2[1]:ext2[3];

	return(0);
}

int LBXGL_PhysSolid_CheckObbMovePlane(float *mins, float *maxs,
	float *sxform, float *exform, float *norm, float *io)
{
	float ext[2];
	int i;

	LBXGL_PhysSolid_ObbPlaneMoveExtents(mins, maxs, sxform, exform,
		norm, ext);

	i=0;
	if((ext[0]-norm[3])<0)i|=2;
	if((ext[1]-norm[3])>=0)i|=1;

	if(io && (i==3))
		LBXGL_PhysSolid_ObbPlaneIntersect(
			mins, maxs, exform, norm, io);

	return(i);
}

int LBXGL_PhysSolid_CheckObbMove(
	float *amins, float *amaxs, float *aspos, float *aepos,
	float *bmins, float *bmaxs, float *bspos, float *bepos,
	float *org)
{
	int i;
	float ext[8];
	float omins[3], omaxs[3], iorg[3];

#if 0
	for(i=0; i<3; i++)
	{
		LBXGL_PhysSolid_ObbPlaneExtents(amins, amaxs, aspos,
			aspos+i*4, ext+0);
		LBXGL_PhysSolid_ObbPlaneExtents(bmins, bmaxs, bspos,
			aspos+i*4, ext+4);
	}
#endif

	for(i=0; i<3; i++)
	{
		LBXGL_PhysSolid_ObbPlaneExtents(amins, amaxs, aepos,
			aepos+i*4, ext+0);
		LBXGL_PhysSolid_ObbPlaneExtents(bmins, bmaxs, bepos,
			aepos+i*4, ext+2);
		if((ext[0]>ext[3]) || (ext[2]>ext[1]))
			return(0);
	}

	for(i=0; i<3; i++)
	{
		LBXGL_PhysSolid_ObbPlaneExtents(amins, amaxs, aepos,
			bepos+i*4, ext+0);
		LBXGL_PhysSolid_ObbPlaneExtents(bmins, bmaxs, bepos,
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
			bepos[i*4+3];
	}

	return(1);
}

int LBXGL_PhysSolid_ObbImpactNormal(
	float *amins, float *amaxs, float *aspos, float *aepos,
	float *bmins, float *bmaxs, float *bspos, float *bepos,
	float *vel, float *norm)
{
	float rv[3], rn[3];
	float ext[2];
	int i;

	rv[0]=VecNF_DotProduct(vel, bepos+0*4, 3);
	rv[1]=VecNF_DotProduct(vel, bepos+1*4, 3);
	rv[2]=VecNF_DotProduct(vel, bepos+2*4, 3);

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

	LBXGL_PhysSolid_ObbPlaneExtents(bmins, bmaxs, bepos,
		norm, ext);
	norm[3]=ext[0];	//near extent

	return(0);
}

int LBXGL_PhysSolid_CheckObbPlaneD(double *mins, double *maxs,
	double *xform, double *norm, double *io)
{
	float rmins[3], rmaxs[3];
	float rxform[16];
	float rnorm[4];
	float rorg[3];
	int i;

	VecND2F_Copy(mins, rmins, 3);
	VecND2F_Copy(maxs, rmaxs, 3);
	VecND2F_Copy(xform, rxform, 16);
	VecND2F_Copy(norm, rnorm, 4);

	i=LBXGL_PhysSolid_CheckObbPlane(rmins, rmaxs, rxform, rnorm,
		io?rorg:NULL);

	if(i && io)VecNF2D_Copy(rorg, io, 3);
	return(i);
}

int LBXGL_PhysSolid_CheckObbEnt(LBXGL_Entity *ent1, LBXGL_Entity *ent2)
{
	float axform[16], bxform[16];
	float amins[3], amaxs[3];
	float bmins[3], bmaxs[3];
	float org1[3], org2[3];
	float ang1[3], ang2[3];
	float norm[4], org[3], vel[3];
	int i, j;

	VecND2F_Copy(ent1->phys->emove, org1, 3);
	VecND2F_Copy(ent2->phys->emove, org2, 3);
	VecND2F_Copy(ent1->phys->eang, ang1, 3);
	VecND2F_Copy(ent2->phys->eang, ang2, 3);
	VecND2F_Copy(ent1->phys->rmins, amins, 3);
	VecND2F_Copy(ent1->phys->rmaxs, amaxs, 3);
	VecND2F_Copy(ent2->phys->rmins, bmins, 3);
	VecND2F_Copy(ent2->phys->rmaxs, bmaxs, 3);

	MatNF_FillIdentity(axform, 4);
	MatNF_FillIdentity(bxform, 4);

	Vec16F_TranslateMatrix(axform, org1, axform);
	Vec16F_TranslateMatrix(bxform, org2, bxform);

	if(ent1->phys->solidtype==TYCRC_SOLID_OBB)
		Vec16F_RotateMatrix(axform, ang1, axform);
	if(ent2->phys->solidtype==TYCRC_SOLID_OBB)
		Vec16F_RotateMatrix(bxform, ang2, bxform);

	i=LBXGL_PhysSolid_CheckObbMove(amins, amaxs, axform, axform,
		bmins, bmaxs, bxform, bxform, org);

	if(i)
	{
		VecND2F_Copy(ent1->phys->vel, vel, 3);
		LBXGL_PhysSolid_ObbImpactNormal(
			amins, amaxs, axform, axform,
			bmins, bmaxs, bxform, bxform,
			vel, norm);
		VecNF2D_Copy(org, ent2->phys->iorg, 3);
		VecNF2D_Copy(norm, ent2->phys->inorm, 4);

		VecND2F_Copy(ent2->phys->vel, vel, 3);
		LBXGL_PhysSolid_ObbImpactNormal(
			bmins, bmaxs, bxform, bxform,
			amins, amaxs, axform, axform,
			vel, norm);
		VecNF2D_Copy(org, ent1->phys->iorg, 3);
		VecNF2D_Copy(norm, ent1->phys->inorm, 4);
	}

#if 0
	VecND_Zero(ent2->phys->inorm, 4);

	for(j=0; j<3; j++)if(i&(1<<j))
	{
		if(ent1->phys->vel[j]>0)
		{
			ent2->phys->inorm[j]=-1;
			return(1);
		}
	}

	for(j=0; j<3; j++)if(i&(8<<j))
	{
		if(ent1->phys->vel[j]<0)
		{
			ent2->phys->inorm[j]=1;
			return(1);
		}
	}
#endif

	return(i);
}
