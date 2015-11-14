#include "bsde_internal.h"


void BSDE_SolidHull2_ProjectPointsLine(bsde_real *vecs, int nvecs,
	bsde_real *xform, bsde_real *norm, bsde_real *min, bsde_real *max)
{
	bsde_real pt[3];
	bsde_real m, n, f;
	int i;

	m=999999999.0;
	n=-999999999.0;
	
	for(i=0; i<nvecs; i++)
	{
		BSDE_Plane_TransformPoint(vecs+i*3, xform, pt);
		f=V3_DOT(pt, norm);
		if(f<m)m=f;
		if(f>n)n=f;
	}

	*min=m;
	*max=n;
}

int BSDE_SolidHull2_CheckCollide(
	bsde_real *anorm, bsde_real *bnorm,
	bsde_real *avecs, bsde_real *bvecs,
	int anf, int bnf, int anv, int bnv,
	bsde_real *axform, bsde_real *bxform)
{
	bsde_real norm[4];
	bsde_real m, n;
	int i;

	for(i=0; i<anf; i++)
	{
		BSDE_Plane_TransformNormal(anorm+i*4, axform, norm);
		BSDE_SolidHull2_ProjectPointsLine(bvecs, bnv,
			bxform, norm, &m, &n);
		if(m>=norm[3])return(0);
	}

	for(i=0; i<bnf; i++)
	{
		BSDE_Plane_TransformNormal(bnorm+i*4, bxform, norm);
		BSDE_SolidHull2_ProjectPointsLine(avecs, anv,
			axform, norm, &m, &n);
		if(m>=norm[3])return(0);
	}

	return(1);
}

int BSDE_SolidHull2_CalcCollide(
	BSDE_Solid *aobj, BSDE_Solid *bobj,
	bsde_real *anorm, bsde_real *bnorm,
	bsde_real *avecs, bsde_real *bvecs,
	int anf, int bnf, int anv, int bnv,
	bsde_real *axform, bsde_real *bxform,
	bsde_real *org, bsde_real *norm, bsde_real *dist)
{
	static bsde_real pts0[16*4], pts1[16*4];
	bsde_real dir[4], dir1[4], dir2[4], bn[4], bo[3];
	bsde_real ldir[4], lorg[3];
	bsde_real m, n, bd, d0, d1, d2, d3;
	bsde_real f;
	int i, j, k;

	V3_ZERO(bn);
	bd=999999;
//	bd=0;

//	printf("m0\n");

	V3_SUB(axform+3*4, bxform+3*4, ldir);
	V3_NORMALIZE(ldir, ldir);

#if 0
	BSDE_SolidHull2_ProjectPointsLine(avecs, anv, axform, ldir, &d0, &d1);
	BSDE_SolidHull2_ProjectPointsLine(bvecs, bnv, bxform, ldir, &d2, &d3);
	m=(d0>d2)?d0:d2; n=(d1<d3)?d1:d3;
	ldir[3]=(m+n)/2;
	V4_COPY(ldir, bn); bd=ldir[3]-m;
#endif

#if 1
	for(i=0; i<anf; i++)
	{
//		printf("N (%g %g %g %g)\n",
//			anorm[i*4+0], anorm[i*4+1],
//			anorm[i*4+2], anorm[i*4+3]);

		BSDE_Plane_TransformNormal(anorm+i*4, axform, dir);
		BSDE_SolidHull2_ProjectPointsLine(bvecs, bnv,
			bxform, dir, &m, &n);
		if(m>dir[3])return(0);
//		if(n<dir[3])continue;
//		if((dir[3]-m)<0.01)continue;

//		f=(dir[3]-m)*(2.0-fabs(V3_DOT(bn, norm)));
		f=dir[3]-m;
		if(f<bd)
//		if((dir[3]-m)<bd)
		{
//			V4_COPY(dir, bn);
			V4_SCALE(dir, -1, bn);
			bd=dir[3]-m;
		}
	}

	for(i=0; i<bnf; i++)
	{
		BSDE_Plane_TransformNormal(bnorm+i*4, bxform, dir);
		BSDE_SolidHull2_ProjectPointsLine(avecs, anv,
			axform, dir, &m, &n);
		if(m>dir[3])return(0);
//		if(n<dir[3])continue;
//		if((dir[3]-m)<0.01)continue;

//		f=(dir[3]-m)*(2.0-fabs(V3_DOT(bn, norm)));
		f=dir[3]-m;
		if(f<bd)
//		if((dir[3]-m)<bd)
		{
//			V4_SCALE(dir, -1, bn);
			V4_COPY(dir, bn);
			bd=dir[3]-m;
		}
	}

#if 0
	for(i=0; i<anf; i++)
		for(j=0; j<bnf; j++)
	{
		BSDE_Plane_TransformNormal(anorm+i*4, axform, dir1);
		BSDE_Plane_TransformNormal(bnorm+j*4, bxform, dir2);
		V3_CROSS(dir1, dir2, dir);
		bd=V3_NORMALIZE(dir, dir);
		if(bd<0.01)continue;

		BSDE_SolidHull2_ProjectPointsLine(avecs, anv,
			axform, dir, &d0, &d1);
		BSDE_SolidHull2_ProjectPointsLine(bvecs, bnv,
			bxform, dir, &d2, &d3);

		m=(d0>d2)?d0:d2;
		n=(d1<d3)?d1:d3;
		if(m>n)return(0);

		dir[3]=(m+n)/2;

		if((n-m)<bd)
		{
//			V4_COPY(dir, bn);
			V4_SCALE(dir, -1, bn);
			bd=n-m;
		}
	}
#endif
#endif

//	printf("m1\n");

	BSDE_SolidHull_MakePlaneFace(bn, pts0);
	j=4;

//	for(i=0; i<j; i++)
//		printf("\t(%g %g %g)\n", pts0[i*3+0], pts0[i*3+1], pts0[i*3+2]);

	for(i=0; i<anf; i++)
	{
		BSDE_Plane_TransformNormal(anorm+i*4, axform, dir);
		j=BSDE_SolidHull_ClipFace(dir, pts0, pts1, j);
		for(k=0; k<(j*3); k++)pts0[k]=pts1[k];
	}

	for(i=0; i<bnf; i++)
	{
		BSDE_Plane_TransformNormal(bnorm+i*4, bxform, dir);
		j=BSDE_SolidHull_ClipFace(dir, pts0, pts1, j);
		for(k=0; k<(j*3); k++)pts0[k]=pts1[k];
	}

#if 1
	if((j>0) && (bd>0.15))
	{
//		printf("A\n");

		V3_ZERO(lorg);
		for(k=0; k<j; k++) { V3_ADD(lorg, pts0+k*3, lorg); }
		V3_SCALE(lorg, 1.0/j, lorg)

		V3_SUB(axform+3*4, lorg, dir1);
//		V3_NORMALIZE(dir1, dir1);

		V3_SUB(lorg, bxform+3*4, dir2);
//		V3_NORMALIZE(dir2, dir2);

		V3_ADD(dir1, dir2, dir);

		if(aobj && bobj)
		{
			V3_SUB(bobj->vel, aobj->vel, dir1);
//			V3_SUB(aobj->vel, bobj->vel, dir1);
			V3_ADDSCALE(dir, dir1, 10, dir);
		}

		V3_NORMALIZE(dir, dir);
		dir[3]=0;

		printf("A %f %f %f\n", dir[0], dir[1], dir[2]);

		BSDE_SolidHull2_ProjectPointsLine(avecs, anv,
			axform, dir, &d0, &d1);
		BSDE_SolidHull2_ProjectPointsLine(bvecs, bnv,
			bxform, dir, &d2, &d3);

		m=(d0>d2)?d0:d2;
		n=(d1<d3)?d1:d3;
		if(m>n)return(0);	//somehow, no collision

		bd=(m+n)/2-V3_DOT(bxform+3*4, dir);
		V3_ADDSCALE(bxform+3*4, dir, bd, org);
		V3_COPY(dir, norm);
		*dist=n-m;
		return(1);
	}
#endif

#if 1
//	if(!j || (bd>0.1))
//	if(!j)
//	if(0)
//	if(bd>0.15)
//	if(bd>0.20)
//	if(bd>0.01)
	if(bd>0.25)
//	if(bd>0.10)
	{

		printf("B\n");

		V3_ZERO(dir1); V3_ZERO(dir2);
		for(i=0; i<anv; i++)
			{ V3_ADD(dir1, avecs+i*3, dir1); }
		for(i=0; i<bnv; i++)
			{ V3_ADD(dir2, bvecs+i*3, dir2); }
		V3_SCALE(dir1, 1.0/anv, dir1);
		V3_SCALE(dir2, 1.0/bnv, dir2);

		V3_SUB(dir1, dir2, dir);
//		V3_SUB(dir2, dir1, dir);

//		V3_SUB(axform+3*4, bxform+3*4, dir);
//		V3_SUB(axform+3*4, bxform+3*4, dir);

		if(aobj && bobj)
		{
			V3_SUB(bobj->vel, aobj->vel, dir1);
//			V3_SUB(aobj->vel, bobj->vel, dir1);
			V3_ADDSCALE(dir, dir1, 10, dir);
		}

		V3_NORMALIZE(dir, dir);
		dir[3]=0;

		BSDE_SolidHull2_ProjectPointsLine(avecs, anv,
			axform, dir, &d0, &d1);
		BSDE_SolidHull2_ProjectPointsLine(bvecs, bnv,
			bxform, dir, &d2, &d3);

		m=(d0>d2)?d0:d2;
		n=(d1<d3)?d1:d3;
		if(m>n)return(0);	//somehow, no collision

		bd=(m+n)/2-V3_DOT(bxform+3*4, dir);
		V3_ADDSCALE(bxform+3*4, dir, bd, org);
		V3_COPY(dir, norm);
//		V3_SCALE(dir, -1, norm);
		*dist=n-m;
		return(1);
	}
#endif

/*
	if(!j)
	{
		printf("clipped away\n");
		return(0);
	}
*/


	for(i=0; i<j; i++)
	{
		V3_COPY(pts0+i*3, org+i*4);
		org[i*4+3]=fabs(V3_NDOT(pts0+i*3, bn));
	}
	V3_COPY(bn, norm);
//	V3_SCALE(bn, -1, norm);
	*dist=bd;

	return(j);
}

#if 1
int BSDE_SolidHull2_CollideOBB(
	bsde_real *amins, bsde_real *amaxs,
	bsde_real *bmins, bsde_real *bmaxs,
	bsde_real *axform, bsde_real *bxform,
	bsde_real *rorg, bsde_real *rnorm, bsde_real *rdist)
{
	bsde_real apts[8*3], bpts[8*3];
	bsde_real ant[6*4], bnt[6*4];
	bsde_real dir[4];
	bsde_real d0, d1, d2, d3, m, n, d;
	int i;

	BSDE_SolidOBB3_GenPointsLocal(amins, amaxs, apts);
	BSDE_SolidOBB3_GenPointsLocal(bmins, bmaxs, bpts);
	BSDE_SolidOBB3_GenNormsLocal(amins, amaxs, ant);
	BSDE_SolidOBB3_GenNormsLocal(bmins, bmaxs, bnt);


	if(0)
	{
		V3_SUB(axform+3*4, bxform+3*4, dir);
		V3_NORMALIZE(dir, dir);
		dir[3]=0;

		BSDE_SolidHull2_ProjectPointsLine(apts, 8,
			axform, dir, &d0, &d1);
		BSDE_SolidHull2_ProjectPointsLine(bpts, 8,
			bxform, dir, &d2, &d3);

		m=(d0>d2)?d0:d2;
		n=(d1<d3)?d1:d3;
		if(m>n)return(0);	//somehow, no collision

		d=(m+n)/2-V3_DOT(bxform+3*4, dir);
		V3_ADDSCALE(bxform+3*4, dir, d, rorg);
		V3_COPY(dir, rnorm);
//		V3_SCALE(dir, -1, rnorm);
		*rdist=n-m;
		return(1);
	}

	i=BSDE_SolidHull2_CalcCollide(NULL, NULL,
		ant, bnt, apts, bpts, 6, 6, 8, 8, 
		axform, bxform, rorg, rnorm, rdist);

//	printf("Collide OBB %d (%g %g %g) %g\n", i,
//		rnorm[0], rnorm[1], rnorm[2], rdist);

	return(i);
}
#endif



int BSDE_SolidHull2_CalcCollideStatic(
	BSDE_Solid *aobj, BSDE_Solid *bobj,
	bsde_real *anorm, bsde_real *bnorm,
	bsde_real *avecs, bsde_real *bvecs,
	int anf, int bnf, int anv, int bnv,
	bsde_real *axform, bsde_real *bxform,
	bsde_real *org, bsde_real *norm, bsde_real *dist)
{
	static bsde_real pts0[64*4], pts1[64*4];
	bsde_real dir[4], dir1[4], dir2[4], bn[4], bo[4];
	bsde_real ldir[4], lorg[4];
	bsde_real m, n, bd, d0, d1, d2, d3;
	bsde_real f;
	int i, j, k;

	V4_ZERO(bn);
	bd=999999;

//	V3_SCALE(bobj->vel, -1, bn);
//	V3_COPY(bobj->vel, bn);
//	V3_NORMALIZE(bn, bn);
//	bd=5;
//	bd=999999;

//	BSDE_SolidHull2_ProjectPointsLine(bvecs, bnv,
//		bxform, bn, &m, &n);
//	bn[3]=0;


	for(i=0; i<anf; i++)
	{
//		printf("N (%g %g %g %g)\n",
//			anorm[i*4+0], anorm[i*4+1],
//			anorm[i*4+2], anorm[i*4+3]);

		BSDE_Plane_TransformNormal(anorm+i*4, axform, dir);
		BSDE_SolidHull2_ProjectPointsLine(bvecs, bnv,
			bxform, dir, &m, &n);
		if(m>dir[3])return(0);
//		if(n<dir[3])continue;
//		if((dir[3]-m)<0.01)continue;

//		f=V3_NDOT(bxform+3*4, dir);
		f=V3_NDOT(bobj->org, dir);
		if(f<0)continue;

		f=dir[3]-m;
		if(f<bd)
		{
//			V4_COPY(dir, bn);
			V4_SCALE(dir, -1, bn);
			bd=f;
		}
	}

	if(bd>999990)
	{
		printf("no plane\n");

//		V3_SCALE(bobj->vel, -1, bn);
//		V3_COPY(bobj->vel, bn);

		V3_SUB(bobj->org, bobj->e_org, bn);
		V3_NORMALIZE(bn, bn);

		printf("Fake %f %f %f\n", bn[0], bn[1], bn[2]);

		BSDE_SolidHull2_ProjectPointsLine(avecs, anv,
			axform, bn, &d0, &d1);
		BSDE_SolidHull2_ProjectPointsLine(bvecs, bnv,
			bxform, bn, &d2, &d3);

		m=(d0>d2)?d0:d2;
		n=(d1<d3)?d1:d3;
		bd=n-m;

		bn[3]=n;

		f=V3_NDOT(bobj->org, bn);
		V3_ADDSCALE(bobj->org, bn, -f-bd, org);
		org[3]=bd;

		V3_COPY(bn, norm);
		*dist=bd;

		return(1);
	}

	BSDE_SolidHull_MakePlaneFace(bn, pts0);
	j=4;

//	for(i=0; i<j; i++)
//		printf("\t(%g %g %g)\n", pts0[i*3+0], pts0[i*3+1], pts0[i*3+2]);

#if 0
	for(i=0; i<anf; i++)
	{
		BSDE_Plane_TransformNormal(anorm+i*4, axform, dir);
		j=BSDE_SolidHull_ClipFace(dir, pts0, pts1, j);
		for(k=0; k<(j*3); k++)pts0[k]=pts1[k];
	}
#endif

	for(i=0; i<bnf; i++)
	{
		BSDE_Plane_TransformNormal(bnorm+i*4, bxform, dir);
		j=BSDE_SolidHull_ClipFace(dir, pts0, pts1, j);
		for(k=0; k<(j*3); k++)pts0[k]=pts1[k];
	}

	if(!j)
	{
		printf("clipped away\n");

		f=V3_NDOT(bobj->org, bn);
		V3_ADDSCALE(bobj->org, bn, -f, org);
		org[3]=bd;

		V3_COPY(bn, norm);
//		V3_SCALE(bn, -1, norm);
		*dist=bd;

		return(1);

		return(0);
	}

	for(i=0; i<j; i++)
	{
		V3_COPY(pts0+i*3, org+i*4);
//		org[i*4+3]=fabs(V3_NDOT(pts0+i*3, bn));
		org[i*4+3]=bd;
	}
	V3_COPY(bn, norm);
//	V3_SCALE(bn, -1, norm);
	*dist=bd;

	return(j);
}


int BSDE_SolidHull2_CalcCollideStaticSphere(
	BSDE_Solid *aobj, BSDE_Solid *bobj,
	bsde_real *anorm, bsde_real *avecs, int anf, int anv,
	bsde_real *axform, bsde_real *bxform,
	bsde_real *org, bsde_real *norm, bsde_real *dist)
{
	static bsde_real pts0[64*4], pts1[64*4];
	bsde_real dir[4], dir1[4], dir2[4], bn[4], bo[3];
	bsde_real ldir[4], lorg[3];
	bsde_real m, n, o, p, d, bd, d0, d1, d2, d3;
	bsde_real f;
	int i, j, k;

	V4_ZERO(bn);
	bd=999999;


	for(i=0; i<anf; i++)
	{
		BSDE_Plane_TransformNormal(anorm+i*4, axform, dir);

		f=V3_DOT(bxform+12, dir);
		m=f-bobj->radius; n=f+bobj->radius;

		if(m>dir[3])return(0);

		f=V3_NDOT(bxform+3*4, dir);
//		f=V3_NDOT(bobj->org, dir);
		if(f<0)continue;

		f=dir[3]-m;
		if(f<bd)
		{
//			V4_COPY(dir, bn);
			V4_SCALE(dir, -1, bn);
			bd=f;
		}
	}

	if(bd>999990)
	{
		printf("no plane (Sph)\n");

		BSDE_SolidHull_NearestPoint(aobj, axform, bxform+12, bo);
		if(V3_DIST(bxform+12, bo)<0.001)V3_COPY(axform+12, bo);

//		V3_SUB(borg, bo, dir);
		V3_SUB(bo, bxform+3*4, dir);
		V3_NORMALIZE(dir, dir);
//		dir[3]=V3_DOT(bo, dir);

		BSDE_PlaneExtents(aobj, dir, &m, &n);
		BSDE_PlaneExtents(bobj, dir, &o, &p);
		m=(m>o)?m:o; n=(n<p)?n:p;

		d=V3_DOT(bxform+3*4, dir);
		V3_ADDSCALE(bxform+3*4, dir, ((m+n)/2)-d, org);
		*dist=n-m;
		return(1);
	}

	V3_ADDSCALE(bxform+3*4, bn, -bobj->radius, org);
	org[3]=bd;

	V3_COPY(bn, norm);
//	V3_SCALE(bn, -1, norm);
	*dist=bd;

	return(1);
}
