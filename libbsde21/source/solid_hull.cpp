/*
Convex Hulls

Each will contain:
A number of face normals defining the solid;
The number of points on each face;
A list of points for each face;
A list of points for the hull.

Normals: N=(x y z w)=(Nxyz w)
Where: w=O*Nxyz
 */

#include "bsde_internal.h"

void BSDE_SolidHull_ModifyHullFaces(BSDE_Solid *obj, bsde_real *norms, int n)
{
	bsde_real d;
	int i;

	if(obj->vecs) { free(obj->vecs); obj->vecs=NULL; }
	if(obj->faces) { free(obj->faces); obj->faces=NULL; }
	if(obj->facevecs) { free(obj->facevecs); obj->facevecs=NULL; }
	if(obj->norms)free(obj->norms);

	obj->norms=(bsde_real *)malloc(4*n*sizeof(bsde_real));
	obj->n_faces=n;
	for(i=0; i<n; i++)
	{
		V3_NORMALIZE(norms+i*4, obj->norms+i*4);
		obj->norms[i*4+3]=norms[i*4+3];
	}

	if((obj->mass>0) && (obj->volume>0))
		d=obj->mass/obj->volume;
		else d=1;

	BSDE_SolidHull_MakeHullFaces(obj);
	obj->mass=d*obj->volume;

	BSDE_BuildInertiaTensor(obj);
}

void BSDE_SolidHull_MakeHullFaces(BSDE_Solid *obj)
{
	static bsde_real vecbuf[64*64*3];
	static bsde_real pointbuf[64*3], pointbuf2[64*3];
	static int facebuf[64*64];
	static int facevbuf[64];
	int i, j, k, l, n, nf, nv;
	int p0, p1, p2;
	bsde_real *v0, *v1, *v2;
	bsde_real dv0[3], dv1[3], dv2[3];
	bsde_real f, g, h;

	n=0;
	nv=0;
	nf=0;

//	printf("build");
	for(i=0; i<obj->n_faces; i++)
	{
		BSDE_SolidHull_MakePlaneFace(obj->norms+(i*4), pointbuf);
		l=4;

		for(j=0; j<obj->n_faces; j++)
		{
			if(i==j)continue;

			l=BSDE_SolidHull_ClipFace(obj->norms+(j*4),
				pointbuf, pointbuf2, l);
			for(k=0; k<(l*3); k++)pointbuf[k]=pointbuf2[k];
		}
		if(!l)continue;

//		printf("%d:%d", i, l);

		for(j=0; j<l; j++)
		{
			for(k=0; k<nv; k++)
				if(V3_DIST(pointbuf+j*3,
					vecbuf+k*3)<0.00001)
						break;
			if(k==nv)
//			if(1)
			{
				V3_COPY(pointbuf+j*3, vecbuf+k*3);
				nv++;
			}

			facebuf[n++]=k;
		}

		facevbuf[nf]=l;
		nf++;
	}
//	printf("\n");

//	printf("facevecs");
//	for(i=0; i<nf; i++)printf(" %d:%d", i, facevbuf[i]);
//	printf("\n");

	if(nf<obj->n_faces)
	{
		printf("MakeHullFaces: %d faces clipped away\n",
			obj->faces-nf);
	}

//	obj->n_faces=nf;

	obj->n_vecs=nv;
	obj->vecs=(bsde_real *)malloc(nv*3*sizeof(bsde_real));
	memcpy(obj->vecs, vecbuf, nv*3*sizeof(bsde_real));

	obj->faces=(int *)malloc(n*sizeof(int));
	memcpy(obj->faces, facebuf, n*sizeof(int));

	obj->facevecs=(int *)malloc(nf*sizeof(int));
	memcpy(obj->facevecs, facevbuf, nf*sizeof(int));

	k=0;
	h=0;
	for(i=0; i<nf; i++)
	{
		l=facevbuf[i];
		for(j=1; j<(l-1); j++)
		{
			p0=facebuf[k];
			p1=facebuf[k+j];
			p2=facebuf[k+j+1];
			if((p0==p1) || (p1==p2) || (p2==p0))continue;

			v0=vecbuf+p0*3;
			v1=vecbuf+p1*3;
			v2=vecbuf+p2*3;

			V3_SUB(v1, v0, dv0);
			V3_SUB(v2, v0, dv1);

			V3_CROSS(dv0, dv1, dv2);
			f=V3_NORMALIZE(dv2, dv2);
			g=f*0.5;

			V3_CROSS(dv0, v0, dv2);
			f=V3_NORMALIZE(dv2, dv2);
			g*=f*0.5;

			V3_CROSS(dv1, v0, dv2);
			f=V3_NORMALIZE(dv2, dv2);
			g*=f*0.5;

			h+=g;
		}
		k+=l;
	}

	obj->volume=h;

	V3_SET(obj->mins, 999999, 999999, 999999);
	V3_SET(obj->maxs, -999999, -999999, -999999);
	for(i=0; i<nv; i++)
	{
		if(vecbuf[i*3+0]<obj->mins[0])obj->mins[0]=vecbuf[i*3+0];
		if(vecbuf[i*3+0]>obj->maxs[0])obj->maxs[0]=vecbuf[i*3+0];
		if(vecbuf[i*3+1]<obj->mins[1])obj->mins[1]=vecbuf[i*3+1];
		if(vecbuf[i*3+1]>obj->maxs[1])obj->maxs[1]=vecbuf[i*3+1];
		if(vecbuf[i*3+2]<obj->mins[2])obj->mins[2]=vecbuf[i*3+2];
		if(vecbuf[i*3+2]>obj->maxs[2])obj->maxs[2]=vecbuf[i*3+2];
	}

	f=0;
	for(i=0; i<nv; i++)
		{ g=V3_LEN(vecbuf+i*3); if(g>f)f=g; }
	obj->radius=f;

	obj->stateflags&=~BSDE_STATEFL_BBOX;
	if(obj->n_faces==6)
	{
		j=0;
		for(i=0; i<obj->n_faces; i++)
		{
			v0=obj->norms+i*4;
			if((v0[0]==1) && (v0[1]==0) && (v0[2]==0))j|=1;
			if((v0[0]==-1) && (v0[1]==0) && (v0[2]==0))j|=2;
			if((v0[0]==0) && (v0[1]==1) && (v0[2]==0))j|=4;
			if((v0[0]==0) && (v0[1]==-1) && (v0[2]==0))j|=8;
			if((v0[0]==0) && (v0[1]==0) && (v0[2]==1))j|=16;
			if((v0[0]==0) && (v0[1]==0) && (v0[2]==-1))j|=32;
		}

		if(j==63)obj->stateflags|=BSDE_STATEFL_BBOX;
	}
}

void BSDE_SolidHull_ComputeVolume(BSDE_Solid *obj)
{
	int i, j;
}

#if 0
void BSDE_SolidHull_ProjectLine(BSDE_Solid *obj,
	bsde_real *xform, bsde_real *norm, bsde_real *min, bsde_real *max)
{
	bsde_real pt[3];
	bsde_real m, n, f;
	int i;

	m=999999999.0;
	n=-999999999.0;
	
	for(i=0; i<obj->n_vecs; i++)
	{
		BSDE_Plane_TransformPoint(obj->vecs+i*3, xform, pt);
		f=V3_DOT(pt, norm);
		if(f<m)m=f;
		if(f>n)n=f;
	}

	*min=m;
	*max=n;
}
#endif

#if 1
void BSDE_SolidHull_ProjectLine(BSDE_Solid *obj,
	bsde_real *xform, bsde_real *norm, bsde_real *min, bsde_real *max)
{
	bsde_real pt[3], dir[4];
	bsde_real *v;
	bsde_real m, n, f;
	int i;

	m=999999999.0; n=-999999999.0;
	BSDE_Plane_TransformVectorT(norm, xform, dir);
	f=(norm[0]*xform[12])+(norm[1]*xform[13])+(norm[2]*xform[14]);
	dir[3]=-f;

	v=obj->vecs;
	for(i=0; i<obj->n_vecs; i++)
		{ f=V3_NDOT(v, dir); if(f<m)m=f; if(f>n)n=f; v+=3; }
	*min=m; *max=n;
}
#endif

void BSDE_SolidHull_Box(BSDE_Solid *obj,
	bsde_real *xform, bsde_real *mins, bsde_real *maxs)
{
	bsde_real pt[3];
	int i, j;

	V3_CONST(mins, BSDE_M_BIGVAL);
	V3_CONST(maxs, -BSDE_M_BIGVAL);
	
	for(i=0; i<obj->n_vecs; i++)
	{
		BSDE_Plane_TransformPoint(obj->vecs+i*3, xform, pt);
		for(j=0; j<3; j++)
		{
			mins[j]=(mins[j]<pt[j])?mins[j]:pt[j];
			maxs[j]=(maxs[j]>pt[j])?maxs[j]:pt[j];
		}
	}
}

void BSDE_SolidHull_BoxMove(BSDE_Solid *obj,
	bsde_real *s_xform, bsde_real *e_xform, bsde_real *mins, bsde_real *maxs)
{
	bsde_real m0[3], n0[3], m1[3], n1[3];

	BSDE_SolidHull_Box(obj, s_xform, m0, n0);
	BSDE_SolidHull_Box(obj, e_xform, m1, n1);
	mins[0]=(m0[0]<m1[0])?m0[0]:m1[0];
	maxs[0]=(n0[0]>n1[0])?n0[0]:n1[0];
	mins[1]=(m0[1]<m1[1])?m0[1]:m1[1];
	maxs[1]=(n0[1]>n1[1])?n0[1]:n1[1];
	mins[2]=(m0[2]<m1[2])?m0[2]:m1[2];
	maxs[2]=(n0[2]>n1[2])?n0[2]:n1[2];
}

int BSDE_SolidHull_PointCollide(BSDE_Solid *obj, bsde_real *xform,
	bsde_real *org)
{
	bsde_real norm[4];
	bsde_real f;
	int i;

	for(i=0; i<obj->n_faces; i++)
	{
		BSDE_Plane_TransformNormal(obj->norms+i*4, xform, norm);
		f=V3_NDOT(org, norm);
		if(f>0)return(0);
	}
	return(1);
}

int BSDE_SolidHull_LineCollide(BSDE_Solid *obj, bsde_real *xform,
	bsde_real *sp, bsde_real *ep)
{
	bsde_real na[16*4];
	bsde_real *nv;
	bsde_real f, g;
	int i;

	for(i=0; i<obj->n_faces; i++)
		BSDE_Plane_TransformNormal(obj->norms+i*4, xform, na+i*4);

	for(i=0; i<obj->n_faces; i++)
	{
		nv=na+i*4; f=V3_NDOT(sp, nv); g=V3_NDOT(ep, nv);
		if((f>=0) && (g>=0))return(0);
		if(f>0)BSDE_Plane_LinePlaneIntersect(sp, ep, nv, sp);
		if(g>0)BSDE_Plane_LinePlaneIntersect(sp, ep, nv, ep);
	}

	for(i=0; i<obj->n_faces; i++)
	{
		nv=na+i*4; f=V3_NDOT(sp, nv); g=V3_NDOT(ep, nv);
		if((f>=0) && (g>=0))return(0);
	}

	return(1);
}

void BSDE_SolidHull_NearestPoint(BSDE_Solid *obj, bsde_real *xform,
	bsde_real *org, bsde_real *pt)
{
	bsde_real norm[4];
	bsde_real f;
	int i;

	V3_COPY(org, pt);
	for(i=0; i<obj->n_faces; i++)
	{
		BSDE_Plane_TransformNormal(obj->norms+i*4, xform, norm);
		f=V3_NDOT(pt, norm);
		if(f>0)
		{
			V3_ADDSCALE(pt, norm, -f, pt);
		}
	}
}

int BSDE_SolidHull_CheckCollide(BSDE_Solid *aobj, BSDE_Solid *bobj,
	bsde_real *axform, bsde_real *bxform)
{
	bsde_real norm[4];
	bsde_real m, n;
	int i;

	if(aobj->stateflags&BSDE_STATEFL_BBOX)
	{
		if(bobj->stateflags&BSDE_STATEFL_BBOX)
		{
			i=BSDE_SolidOBB_CheckCollide(
				aobj->mins, aobj->maxs, axform,
				bobj->mins, bobj->maxs, bxform);
			return(i);
		}

		i=BSDE_SolidHull_CheckObbCollide(bobj, aobj, bxform, axform);
		return(i);
	}
	if(bobj->stateflags&BSDE_STATEFL_BBOX)
	{
		i=BSDE_SolidHull_CheckObbCollide(aobj, bobj, axform, bxform);
		return(i);
	}

	for(i=0; i<aobj->n_faces; i++)
	{
		BSDE_Plane_TransformNormal(aobj->norms+i*4, axform, norm);
		BSDE_SolidHull_ProjectLine(bobj, bxform, norm, &m, &n);
		if(m>=norm[3])return(0);
	}

	for(i=0; i<bobj->n_faces; i++)
	{
		BSDE_Plane_TransformNormal(bobj->norms+i*4, bxform, norm);
		BSDE_SolidHull_ProjectLine(aobj, axform, norm, &m, &n);
		if(m>=norm[3])return(0);
	}

	return(1);
}

int BSDE_SolidHull_CheckObbCollide(BSDE_Solid *aobj, BSDE_Solid *bobj,
	bsde_real *axform, bsde_real *bxform)
{
	bsde_real norm[4], ext[4];
	bsde_real m, n;
	int i;

//	if(V3_DIST(axform+12, bxform+12)>=(aobj->radius+bobj->radius))
//	{
//		printf("Sphere Chk: %f %f\n", aobj->radius, bobj->radius);
//		return(0);
//	}

	if(!BSDE_SolidAABB_BoxCollideP(
		aobj->mmins, aobj->mmaxs, bobj->mmins, bobj->mmaxs))
			return(0);

	for(i=0; i<aobj->n_faces; i++)
	{
		BSDE_Plane_TransformNormal(aobj->norms+i*4, axform, norm);
		BSDE_SolidOBB_ObbPlaneExtents(bobj->mins, bobj->maxs,
			bxform, norm, ext);
		if(ext[0]>=norm[3])return(0);
	}

	for(i=0; i<3; i++)
	{
		V3_COPY(bxform+i*4, norm);
		BSDE_SolidHull_ProjectLine(aobj, axform, norm, &m, &n);
		BSDE_SolidOBB_ObbPlaneExtents(bobj->mins, bobj->maxs,
			bxform, norm, ext);
		if((ext[0]>=n) || (m>=ext[1]))
			return(0);
	}

	return(1);
}

int BSDE_SolidHull_CheckSphereCollide(BSDE_Solid *aobj, BSDE_Solid *bobj,
	bsde_real *axform, bsde_real *bxform, bsde_real *aorg, bsde_real *borg)
{
	bsde_real norm[4], ext[4];
	bsde_real pt[3];
	bsde_real m, n;
	int i;

	BSDE_SolidHull_NearestPoint(aobj, axform, borg, pt);
	if(V3_DIST(borg, pt)>=bobj->radius)
		return(0);

	return(1);
}

int BSDE_SolidHull_CheckCylinderCollide(BSDE_Solid *aobj, BSDE_Solid *bobj,
	bsde_real *axform, bsde_real *bxform, bsde_real *aorg, bsde_real *borg)
{
	bsde_real f;
	bsde_real pt[3], pt2[3];
	bsde_real m, n;
	int i;

	f=V3_DOT(aorg, bxform+2*4)-V3_DOT(borg, bxform+2*4);

	if(f>(bobj->height*0.5))f=(bobj->height*0.5);
	if(f<(-(bobj->height*0.5)))f=-(bobj->height*0.5);

	pt[0]=borg[0]+f*bxform[2*4+0];
	pt[1]=borg[1]+f*bxform[2*4+1];
	pt[2]=borg[2]+f*bxform[2*4+2];

	BSDE_SolidHull_NearestPoint(aobj, axform, pt, pt2);
	if(V3_DIST(pt, pt2)>=bobj->radius)
		return(0);

	if(bobj->solidtype==BSDE_SOLID_CYLINDER)
	{
		f=V3_DOT(pt2, bxform+2*4)-V3_DOT(borg, bxform+2*4);
		if(f>(bobj->height*0.5))return(0);
		if(f<(-(bobj->height*0.5)))return(0);
	}

	return(1);
}

int BSDE_SolidHull_CheckTriMeshCollide(BSDE_Solid *aobj, BSDE_Solid *bobj,
	bsde_real *axform, bsde_real *bxform)
{
	static bsde_real norms[64*4];
	bsde_real *v0, *v1, *v2;
	bsde_real norm[4], v[3];
	bsde_real m, n;
	int i, j;

//	printf("mesh/hull\n");

	BSDE_SolidBRep_Predict(bobj);

	for(i=0; i<aobj->n_faces; i++)
		BSDE_Plane_TransformNormal(
			aobj->norms+i*4, axform, norms+i*4);

	//try to rule out by seperating axis
	for(i=0; i<aobj->n_faces; i++)
	{
		BSDE_SolidBRep_ProjectLine3(bobj, norms+i*4, &m, &n);
		if(m>=norms[i*4+3])return(0);
	}

//	printf("mesh/hull 1\n");

	return(1);

	//verify that at least 1 face is inside the hull
	for(i=0; i<bobj->n_faces; i++)
	{
		v0=bobj->e_vecs+bobj->faces[i*3+0]*3;
		v1=bobj->e_vecs+bobj->faces[i*3+0]*3;
		v2=bobj->e_vecs+bobj->faces[i*3+0]*3;

		for(j=0; j<aobj->n_faces; j++)
		{
			v[0]=V3_NDOT(v0, norms+i*4);
			v[1]=V3_NDOT(v1, norms+i*4);
			v[2]=V3_NDOT(v2, norms+i*4);

			if((v[0]>=0) && (v[1]>=0) && (v[2]>=0))
				break;
		}

		if(j==aobj->n_faces)
			return(1);
	}

	return(0);
}

int BSDE_SolidHull_CollidePoints(BSDE_Solid *obj,
	bsde_real *ipts, bsde_real *opts, int npts)
{
	bsde_real f;
	int i, j, k, l;

	l=0;
	for(i=0; i<npts; i++)
	{
		k=0;
		for(j=0; j<obj->n_faces; j++)
		{
			f=V3_NDOT(ipts+i*3, obj->norms+j*4);
			if(f>0)
			{
				k=1;
				break;
			}
		}
		if(k)continue;

		V3_COPY(ipts+i*3, opts+l*3);
		l++;
	}

	return(l);
}

void BSDE_SolidHull_CollideOrigin(BSDE_Solid *aobj, BSDE_Solid *bobj,
	bsde_real *org)
{
	bsde_real vecs[256*3];
	int i, j;

	i=BSDE_SolidHull_CollidePoints(aobj,
		bobj->vecs, vecs, bobj->n_vecs);
	j=BSDE_SolidHull_CollidePoints(bobj,
		aobj->vecs, vecs+i*3, aobj->n_vecs);

	i=i+j;
	V3_ZERO(org);
	for(j=0; j<i; j++)
	{
		V3_ADD(org, vecs+j*3, org);
	}
	V3_SCALE(org, 1.0/i, org);
}


int BSDE_SolidHull_ContactHull(BSDE_Solid *aobj, BSDE_Solid *bobj,
	bsde_real *axform, bsde_real *bxform, bsde_real *aorg, bsde_real *borg,
	bsde_real *org, bsde_real *norm, bsde_real *dist)
{
	static bsde_real anv[64*4], bnv[64*4];

	bsde_real bn[4], bo[3], tn[4], pt[3];
	bsde_real bd;
	bsde_real m, n, o, p, d, f;
	int i, j, k;

	i=BSDE_SolidHull2_CalcCollide(aobj, bobj,
		aobj->norms, bobj->norms,
		aobj->vecs, bobj->vecs, aobj->n_faces, bobj->n_faces,
		aobj->n_vecs, bobj->n_vecs,
		axform, bxform, org, norm, dist);
	return(i);

	V3_SUB(aorg, borg, bn);
	V3_NORMALIZE(bn, norm);

	BSDE_PlaneExtents(aobj, norm, &m, &n);
	BSDE_PlaneExtents(bobj, norm, &o, &p);
	m=(m>o)?m:o; n=(n<p)?n:p;

	d=V3_DOT(borg, norm);
	V3_ADDSCALE(borg, norm, ((m+n)/2)-d, org);
	*dist=n-m;
	return(1);
}

int BSDE_SolidHull_ContactBox(BSDE_Solid *aobj, BSDE_Solid *bobj,
	bsde_real *axform, bsde_real *bxform, bsde_real *aorg, bsde_real *borg,
	bsde_real *org, bsde_real *norm, bsde_real *dist)
{
	static bsde_real bpts[8*4];
	static bsde_real bnv[6*4];

	bsde_real bn[4], bo[3], tn[4], pt[3];
	bsde_real bd;
	bsde_real m, n, o, p, d, f;
	int i, j, k;

	BSDE_SolidOBB3_GenPointsLocal(bobj->mins, bobj->maxs, bpts);
	BSDE_SolidOBB3_GenNormsLocal(bobj->mins, bobj->maxs, bnv);

	if(aobj->moveflags&(BSDE_MOVEFL_STATIC|BSDE_MOVEFL_SEMISTATIC))
	{
		i=BSDE_SolidHull2_CalcCollideStatic(aobj, bobj,
			aobj->norms, bnv,
			aobj->vecs, bpts, aobj->n_faces, 6,
			aobj->n_vecs, 8,
			axform, bxform, org, norm, dist);
		return(i);
	}

	i=BSDE_SolidHull2_CalcCollide(aobj, bobj,
		aobj->norms, bnv,
		aobj->vecs, bpts, aobj->n_faces, 6,
		aobj->n_vecs, 8,
		axform, bxform, org, norm, dist);
	return(i);

#if 0
	V3_SUB(aorg, borg, bn);
	V3_NORMALIZE(bn, norm);

	BSDE_PlaneExtents(aobj, norm, &m, &n);
	BSDE_PlaneExtents(bobj, norm, &o, &p);
	m=(m>o)?m:o; n=(n<p)?n:p;

	d=V3_DOT(borg, norm);
	V3_ADDSCALE(borg, norm, ((m+n)/2)-d, org);
	*dist=n-m;
	return(1);
#endif
}

int BSDE_SolidHull_ContactSphere(BSDE_Solid *aobj, BSDE_Solid *bobj,
	bsde_real *axform, bsde_real *bxform, bsde_real *aorg, bsde_real *borg,
	bsde_real *org, bsde_real *norm, bsde_real *dist)
{
	static bsde_real bpts[8*4];
	static bsde_real bnv[6*4];

	bsde_real bn[4], bo[3], tn[4], pt[3];
	bsde_real bd;
	bsde_real m, n, o, p, d, f;
	int i, j, k;

	if(aobj->moveflags&(BSDE_MOVEFL_STATIC|BSDE_MOVEFL_SEMISTATIC))
	{
//		printf("Sphere Static Hukk\n");

		i=BSDE_SolidHull2_CalcCollideStaticSphere(aobj, bobj,
			aobj->norms, aobj->vecs, aobj->n_faces,
			aobj->n_vecs, axform, bxform, org, norm, dist);
		return(i);
	}

#if 1
	V3_SUB(aorg, borg, bn);
	V3_NORMALIZE(bn, norm);

	BSDE_PlaneExtents(aobj, norm, &m, &n);
	BSDE_PlaneExtents(bobj, norm, &o, &p);
	m=(m>o)?m:o; n=(n<p)?n:p;

	d=V3_DOT(borg, norm);
	V3_ADDSCALE(borg, norm, ((m+n)/2)-d, org);
	*dist=n-m;
	return(1);
#endif
}

int BSDE_SolidHull_ContactGeneric(BSDE_Solid *aobj, BSDE_Solid *bobj,
	bsde_real *axform, bsde_real *bxform, bsde_real *aorg, bsde_real *borg,
	bsde_real *org, bsde_real *dir, bsde_real *dist)
{
	bsde_real norm[4], bn[4], bo[3];
	bsde_real bd;
	bsde_real m, n, o, p, d, f;
	int i;

#if 0
	if(bobj->solidtype==BSDE_SOLID_SPHERE)
	{
		BSDE_SolidHull_NearestPoint(aobj, axform, borg, bo);
		if(V3_DIST(borg, bo)<0.001)V3_COPY(aorg, bo);

//		V3_SUB(borg, bo, dir);
		V3_SUB(bo, borg, dir);
		V3_NORMALIZE(dir, dir);
//		dir[3]=V3_DOT(bo, dir);

		BSDE_PlaneExtents(aobj, dir, &m, &n);
		BSDE_PlaneExtents(bobj, dir, &o, &p);
		m=(m>o)?m:o; n=(n<p)?n:p;

		d=V3_DOT(borg, dir);
		V3_ADDSCALE(borg, dir, ((m+n)/2)-d, org);
		*dist=n-m;
		return(1);

#if 0
		V3_SUB(bo, borg, bn);
		d=bobj->radius/V3_LEN(bn);
//		V3_SCALE(bn, d, bn);
//		V3_ADD(borg, bn, org);
		V3_ADDSCALE(borg, bn, d, org);

		BSDE_PlaneExtents(aobj, dir, &m, &n);
		*dist=V3_DOT(org, dir)-n;

		return(1);
#endif
	}
#endif

	if(bobj->solidtype==BSDE_SOLID_SPHERE)
	{
		i=BSDE_SolidHull_ContactSphere(aobj, bobj,
			axform, bxform, aorg, borg,
			org, dir, dist);
		return(i);
	}

	if((bobj->solidtype==BSDE_SOLID_AABB) ||
		(bobj->solidtype==BSDE_SOLID_OBB))
	{
		i=BSDE_SolidHull_ContactBox(aobj, bobj,
			axform, bxform, aorg, borg,
			org, dir, dist);
		return(i);
	}

	V3_SUB(aorg, borg, bn);
	V3_NORMALIZE(bn, dir);

	BSDE_PlaneExtents(aobj, dir, &m, &n);
	BSDE_PlaneExtents(bobj, dir, &o, &p);
	m=(m>o)?m:o; n=(n<p)?n:p;

	d=V3_DOT(borg, dir);
	V3_ADDSCALE(borg, dir, ((m+n)/2)-d, org);
	*dist=n-m;
	return(1);

}

void BSDE_SolidHull_HullPlaneIntersect(BSDE_Solid *obj, bsde_real *pos,
	bsde_real *norm, bsde_real *io)
{
	bsde_real bo[3], pt[3];
	bsde_real d;
	int i, j;

	V3_ZERO(bo);
	j=0;

	for(i=0; i<obj->n_vecs; i++)
	{
		BSDE_Plane_TransformPoint(obj->vecs+i*3, pos, pt);
		d=V3_NDOT(pt, norm);
		if(d<0)
		{
			V3_ADD(bo, pt, bo);
			j++;
		}
	}

	if(!j)j++;
	V3_SCALE(bo, 1.0/j, io);
}
