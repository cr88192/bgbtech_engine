/*
Plan: support BSP trees.
As a plus, BSP's wont really move, and each face can probably be treated (sort
of) like a plane.

Initially, checks can be approximate and based on axial projection.
One contact will be generated per face.
*/

#include "bsde_internal.h"

#define BSDE_BSP_SKIPSAME
#define BSDE_BSP_AUTOFLIP

void BSDE_SolidBSP_FaceExtents(BSDE_BSPFace *face, bsde_real *norm,
	bsde_real *min, bsde_real *max)
{
	bsde_real m, n, d;
	int i;

	m=BSDE_M_BIGVAL;
	n=-BSDE_M_BIGVAL;
	for(i=0; i<face->nvecs; i++)
	{
		d=V3_DOT(face->vecs+i*3, norm);
		m=(m<d)?m:d;
		n=(n>d)?n:d;
	}

	*min=m;
	*max=n;
}

int BSDE_SolidBSP_CheckFastMoveFace(BSDE_BSPFace *face,
	BSDE_Solid *obj, bsde_real *pos, bsde_real *org, bsde_real *norm, int max)
{
	bsde_real dv[3];
	bsde_real sd, ed, d, pd;
	bsde_real dm, dn;
	int i, j;

	BSDE_PlaneMoveExtents(obj, face->norm, &dm, &dn);
	if(dm>face->norm[3])return(0);
	if(dn<face->norm[3])return(0);

	for(i=0; i<face->nvecs; i++)
	{
		BSDE_PlaneMoveExtents(obj, face->evecs+i*4, &dm, &dn);

		d=face->evecs[i*4+3]-0.05;
		if(dm>=d)return(0);
	}

	pd=V3_DOT(obj->vel, face->norm);
	if(pd>0)pd=0;
	V3_ADDSCALE(obj->vel, face->norm, -pd*1.10, dv);
	V3_COPY(dv, obj->vel);
	V3_COPY(dv, obj->e_vel);

	//object might collide, forcibly stop
//	V3_ZERO(obj->vel);
//	V3_ZERO(obj->e_vel);
	V3_ZERO(obj->tvel);
	V3_ZERO(obj->e_tvel);

	V3_ZERO(obj->ivel);
	V3_ZERO(obj->itvel);

	V3_COPY(obj->org, obj->e_org);
	V4_COPY(obj->rot, obj->e_rot);
	BSDE_Mat4_Copy(obj->xform, obj->e_xform);

	return(0);
}

int BSDE_SolidBSP_CheckGenericMoveFace(BSDE_BSPFace *face,
	BSDE_Solid *obj, bsde_real *pos, bsde_real *org, bsde_real *norm, int max)
{
	bsde_real mpos[3], tv0[3], tv1[3];
	bsde_real sd, ed, d, pd;
	bsde_real dm, dn;
	int i, j;

	BSDE_PlaneMoveExtents(obj, face->norm, &dm, &dn);
	if((dm>face->norm[3]) && (dn>face->norm[3]))return(0);
	if((dm<face->norm[3]) && (dn<face->norm[3]))return(0);
	pd=face->norm[3]-dm;

#if 0
	for(i=0; i<(face->nvecs-1); i++)
	{
		V3_SUB(face->vecs+(i+1)*3, face->vecs+i*3, tv0);
		V3_CROSS(face->norm, tv0, tv1);
		V3_NORMALIZE(tv1, tv1);

		d=V3_DOT(face->vecs+i*3, tv1);
		BSDE_PlaneExtents(obj, tv1, &dm, &dn);

//		if((dm>d) && (dn>d))
		if((dm<d) && (dn<d))
			return(0);
	}
#endif

#if 1
	for(i=0; i<face->nvecs; i++)
	{
		BSDE_PlaneMoveExtents(obj, face->evecs+i*4, &dm, &dn);

		d=face->evecs[i*4+3]-0.05;
//		if((dm>d) && (dn>d))
//			return(0);
		if(dm>=d)return(0);
	}
#endif

	if(max>0)
	{
		d=V3_DOT(obj->org, face->norm);
		V3_ADDSCALE(obj->org, face->norm, -d-pd, org);
		org[3]=pd;
		V4_COPY(face->norm, norm);
//		printf("fn (%f %f %f %f)\n",
//			norm[0], norm[1], norm[2], norm[3]);
	}
	return(1);
}

void BSDE_SolidBSP_ObbFaceIntersect(bsde_real *mins, bsde_real *maxs,
	bsde_real *pos, BSDE_BSPFace *face, bsde_real *io)
{
	bsde_real pts[64*3];
	bsde_real bo[3], org[3];
	bsde_real d, pd;
	int i, j, k, l, n;

//	BSDE_SolidOBB_GenPoints(mins, maxs, pos, pts);
	BSDE_SolidOBB_GenPoints4x(mins, maxs, pos, pts);

	V3_ZERO(bo);
	n=0;

	for(i=0; i<64; i++)
	{
		V3_COPY(pts+i*3, org);

		d=V3_NDOT(org, face->norm);
		if(d>0)continue;

		for(j=0; j<face->nvecs; j++)
		{
			d=V3_NDOT(org, face->evecs+j*4);
//			if(d>0)break;

#if 0
			if(d>0)
			{
//				V3_ADDSCALE(org, face->evecs+j*4, -d, org);

#if 1
				//point lies outside face volume
				pd=0;
				l=0;
				for(k=0; k<3; k++)
				{
					d=fabs(V3_DOT(pos+k*4, face->evecs+j*4));
					if(d>pd)
					{
						pd=d;
						l=k;
					}
				}
//				if(pd<0.33)continue;

				pd=V3_DOT(pos+l*4, face->evecs+j*4);
				d/=pd;
				V3_ADDSCALE(org, pos+l*4, -d, org);
#endif
			}
#endif
		}
//		if(j<face->nvecs)continue;

		V3_ADD(bo, org, bo);
		n++;
	}

	if(!n)n++;
	V3_SCALE(bo, 1.0/n, io);
}

int BSDE_SolidBSP_CheckObbMoveFace(BSDE_BSPFace *face,
	BSDE_Solid *obj, bsde_real *pos, bsde_real *org, bsde_real *norm, int max)
{
	bsde_real sd, ed, d, pd;
	bsde_real dm, dn;
	int i, j, k;

//	BSDE_PlaneExtents(obj, face->norm, &dm, &dn);
	BSDE_SolidOBB_PlaneExtents(obj->mins, obj->maxs, pos,
		face->norm, &dm, &dn);
//	BSDE_PlaneMoveExtents(obj, face->norm, &dm, &dn);
	if((dm>face->norm[3]) && (dn>face->norm[3]))return(0);
	if((dm<face->norm[3]) && (dn<face->norm[3]))return(0);
	pd=face->norm[3]-dm;

	for(i=0; i<face->nvecs; i++)
	{
//		BSDE_PlaneExtents(obj, face->evecs+i*4, &dm, &dn);
		BSDE_SolidOBB_PlaneExtents(obj->mins, obj->maxs, pos,
			face->evecs+i*4, &dm, &dn);
//		BSDE_PlaneMoveExtents(obj, face->evecs+i*4, &dm, &dn);
		d=face->evecs[i*4+3]-0.05;
		if(dm>=d)return(0);
	}

	for(i=0; i<3; i++)
	{
		BSDE_SolidBSP_FaceExtents(face, pos+i*4, &dm, &dn);
		d=V3_DOT(pos+3*4, pos+i*4);
		if(((dm-d)>obj->maxs[i]) || ((dn-d)<obj->mins[i]))
			return(0);
	}

	if(max<=0)return(1);	//no space for contact info

	//FIXME: make contact points be within face
	V4_COPY(face->norm, norm);
	BSDE_SolidOBB_ObbPlaneIntersect(
		obj->mins, obj->maxs, pos, norm, org);
//	BSDE_SolidBSP_ObbFaceIntersect(
//		obj->mins, obj->maxs, pos, face, org);
	d=V3_DOT(org, norm);
	org[3]=norm[3]-d;
//	org[3]=pd;
//	org[3]=10*pd;

	return(1);
}

int BSDE_SolidBSP_CheckSphereFace(BSDE_BSPFace *face,
	BSDE_Solid *obj, bsde_real *pos, bsde_real *org, bsde_real *norm, int max)
{
	bsde_real io[3], iv[3], d, r;
	int i;

	r=obj->radius*obj->radius;
	V3_COPY(pos+3*4, io);

	d=V3_NDOT(io, face->norm);
	V3_ADDSCALE(io, face->norm, -d, io);

	for(i=0; i<face->nvecs; i++)
	{
		d=V3_NDOT(io, face->evecs+i*4)-0.05;
		if(d>0) { V3_ADDSCALE(io, face->evecs+i*4, -d, io); }
	}

	d=V3_DIST(pos+3*4, io);
	if(d>=obj->radius)return(0);

	if(max>0)
	{
//		V4_COPY(face->norm, norm);
		V3_SUB(pos+3*4, io, norm);
		V3_NORMALIZE(norm, norm);
		norm[3]=V3_DOT(io, norm);

		V3_SUB(io, pos+3*4, iv);
		d=obj->radius/V3_LEN(iv);
		V3_SCALE(iv, d, iv);
		V3_ADD(pos+3*4, iv, org);
		org[3]=-V3_NDOT(org, face->norm);
	}

	return(1);
}

int BSDE_SolidBSP_CheckCylinderFace(BSDE_BSPFace *face,
	BSDE_Solid *obj, bsde_real *pos, bsde_real *org, bsde_real *norm, int max)
{
	bsde_real v0[3], v1[3], pt[3];
	bsde_real io[3], iv[3], d, r, f;
	int i;

	V3_ADDSCALE(pos+3*4, pos+2*4, -obj->height*0.5, v0);
	V3_ADDSCALE(pos+3*4, pos+2*4, obj->height*0.5, v1);

	BSDE_Plane_NearestPointLinePlane(v0, v1, face->norm, pt);

	r=obj->radius*obj->radius;
	V3_COPY(pt, io);

	d=V3_NDOT(io, face->norm);
	V3_ADDSCALE(io, face->norm, -d, io);

	for(i=0; i<face->nvecs; i++)
	{
		d=V3_NDOT(io, face->evecs+i*4)-0.05;
		if(d>0) { V3_ADDSCALE(io, face->evecs+i*4, -d, io); }
	}

//	printf("cyl 0\n");

	if(obj->solidtype==BSDE_SOLID_CYLINDER)
	{
		f=fabs(V3_DOT(face->norm, pos+2*4));
		r=obj->radius*(1.1-f);
		d=V3_NDOT(pt, face->norm);
		if(d>r)return(0);

		if(V3_DIST(pt, io)>=obj->radius)return(0);
		if(max<=0)return(1);


#if 1
		V4_COPY(face->norm, norm);

//		f=V3_DOT(face->norm, pos+2*4);
//		V3_SCALEADDSCALE(face->norm, 1-fabs(f), pos+2*4, f, norm);
//		V3_NORMALIZE(norm, norm);
//		norm[3]=V3_DOT(io, norm);

		BSDE_SolidCylinder_NearestPoint(obj->height, obj->radius, 0,
			pos, io, org);
		f=V3_NDOT(org, face->norm);
		V3_ADDSCALE(org, face->norm, -f, org);

		org[3]=r-d-V3_NDOT(org, norm);
//		org[3]=r-V3_NDOT(org, norm);

//		V3_COPY(io, org);
//		V3_ADDSCALE(pt, face->norm, d-r, org);
//		org[3]=r-d;

#endif

#if 0
		V3_SUB(pt, io, norm);
		V3_NORMALIZE(norm, norm);
		norm[3]=V3_DOT(io, norm);

#if 1
		V3_SUB(io, pt, iv);
		d=r/V3_LEN(iv);
		V3_SCALE(iv, d, iv);
		V3_ADD(pt, iv, org);
		org[3]=-V3_NDOT(org, face->norm);
#else
		V3_SUB(io, pt, iv);
		d=obj->radius/V3_LEN(iv);
		V3_SCALE(iv, d, iv);
		V3_ADD(pt, iv, org);
		org[3]=-V3_NDOT(org, face->norm);
#endif
#endif

		return(1);
	}

	d=V3_DIST(pt, io);
	if(d>=obj->radius)return(0);

//	printf("cyl 1\n");

	if(max>0)
	{
//		V4_COPY(face->norm, norm);
		V3_SUB(pt, io, norm);
		V3_NORMALIZE(norm, norm);
		norm[3]=V3_DOT(io, norm);

		V3_SUB(io, pt, iv);
		d=obj->radius/V3_LEN(iv);
		V3_SCALE(iv, d, iv);
		V3_ADD(pt, iv, org);
		org[3]=-V3_NDOT(org, face->norm);
	}

	return(1);
}

int BSDE_SolidBSP_CheckHullFace(BSDE_BSPFace *face,
	BSDE_Solid *obj, bsde_real *pos, bsde_real *org, bsde_real *norm, int max)
{
	bsde_real sd, ed, d, pd;
	bsde_real dm, dn;
	int i, j, k;

//	BSDE_PlaneExtents(obj, face->norm, &dm, &dn);
	BSDE_PlaneMoveExtents(obj, face->norm, &dm, &dn);
	if((dm>face->norm[3]) && (dn>face->norm[3]))return(0);
	if((dm<face->norm[3]) && (dn<face->norm[3]))return(0);
	pd=face->norm[3]-dm;

	for(i=0; i<face->nvecs; i++)
	{
//		BSDE_PlaneExtents(obj, face->evecs+i*4, &dm, &dn);
		BSDE_PlaneMoveExtents(obj, face->evecs+i*4, &dm, &dn);
		d=face->evecs[i*4+3]-0.05;
		if(dm>=d)return(0);
	}

	for(i=0; i<obj->n_faces; i++)
	{
		BSDE_SolidBSP_FaceExtents(face, obj->norms+i*4, &dm, &dn);
		d=V3_DOT(pos+3*4, obj->norms+i*4);
		if((dm-d)>obj->norms[i*4+3])
			return(0);
	}

	if(max<=0)return(1);	//no space for contact info

	//FIXME: make contact points be within face
	V4_COPY(face->norm, norm);
	BSDE_SolidHull_HullPlaneIntersect(obj, pos, norm, org);
	d=V3_DOT(org, norm);
	org[3]=norm[3]-d;

	return(1);
}

int BSDE_SolidBSP_CheckTriMeshFace(BSDE_BSPFace *face,
	BSDE_Solid *obj, bsde_real *pos, bsde_real *org, bsde_real *norm, int max)
{
	bsde_real sd, ed, d, pd;
	bsde_real dm, dn;
	int i, j, k;

//	BSDE_SolidBRep_Predict(obj);

//	BSDE_PlaneMoveExtents(obj, face->norm, &dm, &dn);
	BSDE_SolidBRep_ProjectLine3(obj, face->norm, &dm, &dn);

	if((dm>face->norm[3]) && (dn>face->norm[3]))return(0);
	if((dm<face->norm[3]) && (dn<face->norm[3]))return(0);
	pd=face->norm[3]-dm;

	for(i=0; i<face->nvecs; i++)
	{
//		BSDE_PlaneExtents(obj, face->evecs+i*4, &dm, &dn);
//		BSDE_PlaneMoveExtents(obj, face->evecs+i*4, &dm, &dn);
		BSDE_SolidBRep_ProjectLine3(obj, face->evecs+i*4, &dm, &dn);
		d=face->evecs[i*4+3]-0.05;
		if(dm>=d)return(0);
	}

	if(max<=0)return(1);	//no space for contact info

	//FIXME: make contact points be within face
	V4_COPY(face->norm, norm);

	BSDE_SolidBRep_PlaneIntersect(obj, pos, norm, org);

	d=V3_DOT(org, norm);
	org[3]=norm[3]-d;

	return(1);
}

int BSDE_SolidBSP_CheckMoveFace(BSDE_BSPFace *face,
	BSDE_Solid *obj, bsde_real *pos, bsde_real *org, bsde_real *norm, int max)
{
	bsde_real dv[3];
	bsde_real m, n, o, p;
	bsde_real m2, n2, d, d0, d1, v;
	int i;

#if 0
	//speed checking
	BSDE_PlaneExtentsPos(obj, obj->xform, face->norm, &m, &n);
	BSDE_PlaneExtentsPos(obj, obj->e_xform, face->norm, &o, &p);
	m2=(m<o)?m:o;
	n2=(n>p)?n:p;

	if((m2>face->norm[3]) || (n2<face->norm[3]))
		return(0);

	m2=(m>o)?m:o;
	n2=(n<p)?n:p;

	d0=n-m;
	d1=p-o;
	d=(d0<d1)?d0:d1;
	v=fabs(o-m);

//	if(m2>=n2)
	if(v>=(d*0.15))
	{
		//object is moving fast
		printf("fast object (%f,%f) (%f %f)\n", m, n, o, p);

		i=BSDE_SolidBSP_CheckFastMoveFace(
			face, obj, pos, org, norm, max);
		return(i);
	}
#endif

	switch(obj->solidtype)
	{
	case BSDE_SOLID_AABB:
//		i=BSDE_SolidBSP_CheckGenericMoveFace(
//			face, obj, pos, org, norm, max);
//		break;
	case BSDE_SOLID_OBB:
		i=BSDE_SolidBSP_CheckObbMoveFace(
			face, obj, pos, org, norm, max);
		break;
	case BSDE_SOLID_SPHERE:
		i=BSDE_SolidBSP_CheckSphereFace(
			face, obj, pos, org, norm, max);
		break;
	case BSDE_SOLID_CYLINDER:
	case BSDE_SOLID_CCYLINDER:
		i=BSDE_SolidBSP_CheckCylinderFace(
			face, obj, pos, org, norm, max);
		break;
	case BSDE_SOLID_HULL:
		i=BSDE_SolidBSP_CheckHullFace(
			face, obj, pos, org, norm, max);
		break;
	case BSDE_SOLID_TRIMESH:
		i=BSDE_SolidBSP_CheckTriMeshFace(
			face, obj, pos, org, norm, max);
		break;
	default:
		i=BSDE_SolidBSP_CheckGenericMoveFace(
			face, obj, pos, org, norm, max);
		break;
	}
	return(i);
}

int BSDE_SolidBSP_CheckMoveLeaf(BSDE_BSPNode *node,
	BSDE_Solid *obj, bsde_real *pos, bsde_real *org, bsde_real *norm, int max)
{
	BSDE_BSPFace *cur;
	int i, j;

	i=0;
	cur=node->face;
	while(cur)
	{
		j=BSDE_SolidBSP_CheckMoveFace(cur, obj, pos,
			org+i*4, norm+i*4, max);
		i+=j;
		max-=j;
		cur=cur->next;
	}
	return(i);
}

int BSDE_SolidBSP_CheckMoveNode(BSDE_BSPNode *node,
	BSDE_Solid *obj, bsde_real *pos, bsde_real *org, bsde_real *norm, int max)
{
	bsde_real dm, dn, sd, ed;
	int i;

	if(!node->lnode)
	{
		i=BSDE_SolidBSP_CheckMoveLeaf(node, obj, pos, org, norm, max);
		return(i);
	}

	sd=V3_DOT(obj->org, node->norm)-node->norm[3];
	ed=V3_DOT(obj->e_org, node->norm)-node->norm[3];
//	BSDE_PlaneExtents(obj, node->norm, &dm, &dn);
	BSDE_PlaneFastMoveExtents(obj, node->norm, &dm, &dn);

	if((dm<=node->norm[3]) && (dn<=node->norm[3]))
//	if(dn<=node->norm[3])
	{
		i=BSDE_SolidBSP_CheckMoveNode(node->lnode, obj, pos,
			org, norm, max);
		return(i);
	}

	if((dm>=node->norm[3]) && (dn>=node->norm[3]))
//	if(dm>=node->norm[3])
	{
		i=BSDE_SolidBSP_CheckMoveNode(node->rnode, obj, pos,
			org, norm, max);
		return(i);
	}

	if(sd<0)
	{
		i=BSDE_SolidBSP_CheckMoveNode(node->lnode, obj, pos,
			org, norm, max);
		i+=BSDE_SolidBSP_CheckMoveNode(node->rnode, obj, pos,
			org+i*4, norm+i*4, max-i);
		return(i);
	}else
	{
		i=BSDE_SolidBSP_CheckMoveNode(node->rnode, obj, pos,
			org, norm, max);
		i+=BSDE_SolidBSP_CheckMoveNode(node->lnode, obj, pos,
			org+i*4, norm+i*4, max-i);
		return(i);
	}

	return(0);
}


int BSDE_SolidBSP_CheckCollide(
	BSDE_Solid *aobj, bsde_real *apos, BSDE_Solid *bobj, bsde_real *bpos)
{
	static bsde_real org[4*64], norm[4*64];
	BSDE_Solid *tobj;
	bsde_real tv[4], f;
	bsde_real *v0;
	int flip;
	int i, j;

	if(bobj->solidtype==BSDE_SOLID_BSP)
	{
		if(aobj->solidtype==BSDE_SOLID_BSP)
			return(0);
		tobj=aobj; aobj=bobj; bobj=tobj;
		v0=apos; apos=bpos; bpos=v0;
		flip=1;
	}else flip=0;

	i=BSDE_SolidBSP_CheckMoveNode((BSDE_BSPNode *)aobj->data,
		bobj, bpos, org, norm, 0);
//	printf("BSDE_SolidBSP_CheckCollide: %d contacts\n", i);
	return(i!=0);
//	return(1);
}

int BSDE_SolidBSP_CollideMultiContact(
	BSDE_World *world,
	BSDE_Solid *aobj, bsde_real *apos,
	BSDE_Solid *bobj, bsde_real *bpos,
	BSDE_Contact **lst)
{
	static bsde_real org[4*64], norm[4*64];
	BSDE_Solid *tobj;
	bsde_real tv[4], f, g;
	bsde_real *v0;
	int flip;
	int i, j, k, n;

//	printf("BSDE_SolidBSP_CollideMultiContact:\n");

	if(bobj->solidtype==BSDE_SOLID_BSP)
	{
		if(aobj->solidtype==BSDE_SOLID_BSP)
			return(0);
		tobj=aobj; aobj=bobj; bobj=tobj;
		v0=apos; apos=bpos; bpos=v0;
		flip=1;
	}else flip=0;

	//detect identity
	k=0;
	for(i=0; i<4; i++)for(j=0; j<4; j++)
		k|=(int)((i==j)?(apos[i*4+j]!=1):(apos[i*4+j]!=0));

//	if(!k && (aobj->moveflags&BSDE_MOVEFL_STATIC))
//	if(1)
	if(!k)
	{
		i=BSDE_SolidBSP_CheckMoveNode((BSDE_BSPNode *)aobj->data,
			bobj, bpos, org, norm, 64);
		if(i>64)i=64;
	}else
	{
		i=BSDE_SolidBSP2_CheckMoveNode((BSDE_BSPNode *)aobj->data, apos,
			bobj, bpos, org, norm, 64);
		if(i>64)i=64;
	}

//	if(i)printf("BSDE_SolidBSP_CollideMultiContact: %d contacts\n", i);

	f=flip?-1:1;
//	f=flip?1:-1;
	n=0;
	for(j=0; j<i; j++)
	{
#ifdef BSDE_BSP_SKIPSAME
		for(k=0; k<j; k++)
			if(V3_DOT(norm+k*4, norm+j*4)>0.95)
				break;
		if(k<j)continue;
#endif

#ifdef BSDE_BSP_AUTOFLIP
		g=V3_DOT(bobj->org, norm+j*4)-norm[j*4+3];
		f=(g<0)?-1:1;
#endif

		V4_SCALE(norm+j*4, f, tv);
		lst[n++]=BSDE_MakeContact(world, bobj, aobj,
			org+j*4, tv, org[j*4+3]);
//		printf("(%f %f %f)\n", tv[0], tv[1], tv[2]);
	}
	return(n);
}

int BSDE_SolidBSP_TraceFace2(BSDE_BSPFace *face, bsde_real *sp, bsde_real *ep,
	bsde_real *io)
{
	bsde_real pt[3], dv[3], dn[4];
	bsde_real f, g;
	int i, j;

	f=V3_NDOT(sp, face->norm);
	g=V3_NDOT(ep, face->norm);

	if((f<=0) && (g<=0))return(0);
	if((f>=0) && (g>=0))return(0);

	BSDE_Plane_LinePlaneIntersect(sp, ep, face->norm, pt);
	for(i=0; i<face->nvecs; i++)
	{
		f=V3_NDOT(pt, face->evecs+i*4);
		if(f>=0)return(0);
	}

	V3_COPY(pt, io);
	return(1);
}

BSDE_BSPFace *BSDE_SolidBSP_TraceFaces2(BSDE_BSPFace *face,
	bsde_real *sp, bsde_real *ep, bsde_real *io)
{
	BSDE_BSPFace *cur;
	int i;

	cur=face;
	while(cur)
	{
		i=BSDE_SolidBSP_TraceFace2(cur, sp, ep, io);
		if(i)return(cur);
		cur=cur->next;
	}
	return(NULL);
}

BSDE_BSPFace *BSDE_SolidBSP_TraceNode2(BSDE_BSPNode *node,
	bsde_real *sp, bsde_real *ep, bsde_real *io)
{
	BSDE_BSPFace *tmp;
	bsde_real f, g;
	int i, j;

	if(!node->lnode)
	{
		tmp=BSDE_SolidBSP_TraceFaces2(node->face, sp, ep, io);
		return(tmp);
	}

	f=V3_NDOT(sp, node->norm);
	g=V3_NDOT(ep, node->norm);

	if((f<=0) && (g<=0))
	{
		tmp=BSDE_SolidBSP_TraceNode2(node->lnode, sp, ep, io);
		return(tmp);
	}
	if((f>=0) && (g>=0))
	{
		tmp=BSDE_SolidBSP_TraceNode2(node->rnode, sp, ep, io);
		return(tmp);
	}

	if(f<g)
	{
		tmp=BSDE_SolidBSP_TraceNode2(node->lnode, sp, ep, io);
		if(tmp)return(tmp);

		tmp=BSDE_SolidBSP_TraceFaces2(node->face, sp, ep, io);
		if(tmp)return(tmp);

		tmp=BSDE_SolidBSP_TraceNode2(node->rnode, sp, ep, io);
		return(tmp);
	}else
	{
		tmp=BSDE_SolidBSP_TraceNode2(node->rnode, sp, ep, io);
		if(tmp)return(tmp);

		tmp=BSDE_SolidBSP_TraceFaces2(node->face, sp, ep, io);
		if(tmp)return(tmp);

		tmp=BSDE_SolidBSP_TraceNode2(node->lnode, sp, ep, io);
		return(tmp);
	}
}

int BSDE_SolidBSP_LineCollide(BSDE_Solid *obj, bsde_real *xform,
	bsde_real *isp, bsde_real *iep)
{
	bsde_real sp[3], ep[3], io[4];
	BSDE_BSPFace *tmp;

	BSDE_Plane_TransformPointLocal(isp, obj->xform, sp);
	BSDE_Plane_TransformPointLocal(iep, obj->xform, ep);

	tmp=BSDE_SolidBSP_TraceNode2((BSDE_BSPNode *)obj->data, sp, ep, io);
	if(tmp)return(1);
	return(0);
}


void BSDE_SolidBSP_DestroyFace(BSDE_BSPFace *face)
{
	if(face->vecs)free(face->vecs);
	if(face->evecs)free(face->evecs);
	free(face);
}

void BSDE_SolidBSP_DestroyFaces(BSDE_BSPFace *face)
{
	BSDE_BSPFace *cur, *nxt;

	cur=face;
	while(cur)
	{
		nxt=cur->next;
		BSDE_SolidBSP_DestroyFace(cur);
		cur=nxt;
	}
}

void BSDE_SolidBSP_DestroyNode(BSDE_BSPNode *node)
{
	if(!node)return;

	BSDE_SolidBSP_DestroyNode(node->lnode);
	BSDE_SolidBSP_DestroyNode(node->rnode);
	BSDE_SolidBSP_DestroyFaces(node->face);
	free(node);
}

BSDE_BSPFace *BSDE_SolidBSP_BuildFace(BSDE_World *world,
	int **arrp, bsde_real **varrp, bsde_real *mins, bsde_real *maxs)
{
	bsde_real vbuf[32*3];
	bsde_real tv0[3], tv1[3];
	BSDE_BSPFace *tmp;
	bsde_real *varr, f;
	int *arr;
	int i, j, n, ns;

	tmp=(BSDE_BSPFace *)malloc(sizeof(BSDE_BSPFace));
	memset(tmp, 0, sizeof(BSDE_BSPFace));

	n=0;
	ns=0;
	arr=*arrp;
	varr=*varrp;
	while(*arr!=BSDE_TAG_END)
	{
		if(*arr==BSDE_TAG_NORMAL)
		{
			arr++;
			if(!ns)
			{
				V4_COPY(varr, tmp->norm);
				tmp->norm[3]*=world->sc_len;
			}
			ns=1;
			varr+=4;
			continue;
		}

		if(*arr==BSDE_TAG_VERTEX)
		{
			for(i=0; i<n; i++)
				if(V3_DIST(varr, vbuf+i*3)<0.01)
					break;
			if(i<n)	//near collinear points
			{
				arr++;
				varr+=4;
				continue;
			}

			arr++;
			V3_COPY(varr, vbuf+n*3);
//			V3_SCALE(varr, world->sc_len, vbuf+n*3);
			varr+=4;
			n++;
			continue;
		}

		BSDE_ArraySkipUnknown(&arr, &varr);
		break;
	}
	arr++;

#if 0
	if(!ns)
	{
		//normal has not been set, calculate from vertices
		V3_SUB(vbuf, vbuf+3, tv0);
		V3_SUB(vbuf+6, vbuf+3, tv1);
		V3_CROSS(tv0, tv1, tmp->norm);
		V3_NORMALIZE(tmp->norm, tmp->norm);
		tmp->norm[3]=V3_DOT(vbuf, tmp->norm);
	}
#endif

	tmp->vecs=(bsde_real *)malloc(n*3*sizeof(bsde_real));
	for(i=0; i<n; i++)for(j=0; j<3; j++)
	{
		f=vbuf[i*3+j]*world->sc_len;
		mins[j]=(mins[j]<f)?mins[j]:f;
		maxs[j]=(maxs[j]>f)?maxs[j]:f;
		tmp->vecs[i*3+j]=f;
	}

	if(!ns)
	{
		//normal has not been set, calculate from vertices
		V3_SUB(tmp->vecs, tmp->vecs+3, tv0);
		V3_SUB(tmp->vecs+6, tmp->vecs+3, tv1);
		V3_CROSS(tv0, tv1, tmp->norm);
		V3_NORMALIZE(tmp->norm, tmp->norm);
		tmp->norm[3]=V3_DOT(tmp->vecs, tmp->norm);
	}

	tmp->evecs=(bsde_real *)malloc(n*4*sizeof(bsde_real));
	for(i=0; i<n; i++)
	{
		V3_SUB(tmp->vecs+((i+1)%n)*3, tmp->vecs+i*3, tv0);
		V3_CROSS(tmp->norm, tv0, tv1);
		V3_NORMALIZE(tv1, tmp->evecs+i*4);
		tmp->evecs[i*4+3]=V3_DOT(tmp->vecs+i*3, tmp->evecs+i*4);
	}

	tmp->nvecs=n;

	*arrp=arr;
	*varrp=varr;
	return(tmp);
}

BSDE_BSPNode *BSDE_SolidBSP_BuildLeaf(BSDE_World *world,
	int **arrp, bsde_real **varrp, bsde_real *mins, bsde_real *maxs)
{
	int *arr;
	bsde_real *varr;
	BSDE_BSPNode *tmp;
	BSDE_BSPFace *tf;
	int i;

	tmp=(BSDE_BSPNode *)malloc(sizeof(BSDE_BSPNode));
	memset(tmp, 0, sizeof(BSDE_BSPNode));
	V3_CONST(tmp->mins, BSDE_M_BIGVAL);
	V3_CONST(tmp->maxs, -BSDE_M_BIGVAL);

	arr=*arrp;
	varr=*varrp;
	while(*arr!=BSDE_TAG_END)
	{
		if((arr[0]==BSDE_TAG_BEGIN) && (arr[1]==BSDE_BSP_FACE))
		{
			arr+=2;
			tf=BSDE_SolidBSP_BuildFace(world,
				&arr, &varr, tmp->mins, tmp->maxs);
			tf->next=tmp->face;
			tmp->face=tf;
			continue;
		}

		BSDE_ArraySkipUnknown(&arr, &varr);
		break;
	}
	arr++;

	for(i=0; i<3; i++)
	{
		mins[i]=(mins[i]<tmp->mins[i])?mins[i]:tmp->mins[i];
		maxs[i]=(maxs[i]>tmp->maxs[i])?maxs[i]:tmp->maxs[i];
	}

	*arrp=arr;
	*varrp=varr;
	return(tmp);
}

BSDE_BSPNode *BSDE_SolidBSP_BuildNode(BSDE_World *world,
	int **arrp, bsde_real **varrp,
	bsde_real *mins, bsde_real *maxs)
{
	int *arr;
	bsde_real *varr;
	BSDE_BSPNode *tmp, *tn;
	int i;

	tmp=(BSDE_BSPNode *)malloc(sizeof(BSDE_BSPNode));
	memset(tmp, 0, sizeof(BSDE_BSPNode));
	V3_CONST(tmp->mins, BSDE_M_BIGVAL);
	V3_CONST(tmp->maxs, -BSDE_M_BIGVAL);

	arr=*arrp;
	varr=*varrp;
	while(*arr!=BSDE_TAG_END)
	{
		if(*arr==BSDE_TAG_NORMAL)
		{
			arr++;
			V4_COPY(varr, tmp->norm);
			tmp->norm[3]*=world->sc_len;
			varr+=4;
			continue;
		}

		if((arr[0]==BSDE_TAG_BEGIN) && (arr[1]==BSDE_BSP_NODE))
		{
			arr+=2;
			tn=BSDE_SolidBSP_BuildNode(world,
				&arr, &varr, tmp->mins, tmp->maxs);
			if(!tmp->lnode)tmp->lnode=tn;
			else if(!tmp->rnode)tmp->rnode=tn;
			continue;
		}
		if((arr[0]==BSDE_TAG_BEGIN) && (arr[1]==BSDE_BSP_LEAF))
		{
			arr+=2;
			tn=BSDE_SolidBSP_BuildLeaf(world,
				&arr, &varr, tmp->mins, tmp->maxs);
			if(!tmp->lnode)tmp->lnode=tn;
			else if(!tmp->rnode)tmp->rnode=tn;
			continue;
		}

		BSDE_ArraySkipUnknown(&arr, &varr);
		break;
	}
	arr++;

	for(i=0; i<3; i++)
	{
		mins[i]=(mins[i]<tmp->mins[i])?mins[i]:tmp->mins[i];
		maxs[i]=(maxs[i]>tmp->maxs[i])?maxs[i]:tmp->maxs[i];
	}

	*arrp=arr;
	*varrp=varr;
	return(tmp);
}

BSDE_Solid *BSDE_SolidBSP_BuildArray(BSDE_World *world,
	int **arrp, bsde_real **varrp)
{
	bsde_real mins[3], maxs[3];
	BSDE_Solid *tmp;
	BSDE_BSPNode *root;
	bsde_real *varr;
	int *arr;

	printf("BSDE_SolidBSP_BuildArray:\n");

	V3_CONST(mins, BSDE_M_BIGVAL);
	V3_CONST(maxs, -BSDE_M_BIGVAL);

	arr=*arrp;
	varr=*varrp;
	root=NULL;
	while(*arr!=BSDE_TAG_END)
	{
		if((arr[0]==BSDE_TAG_BEGIN) && (arr[1]==BSDE_BSP_NODE))
		{
			arr+=2;
			if(root)printf("BSDE_SolidBSP_BuildArray: "
				"multi-root tree\n");
			root=BSDE_SolidBSP_BuildNode(world, &arr, &varr, mins, maxs);
			continue;
		}
		if((arr[0]==BSDE_TAG_BEGIN) && (arr[1]==BSDE_BSP_LEAF))
		{
			arr+=2;
			if(root)printf("BSDE_SolidBSP_BuildArray: "
				"multi-root tree\n");
			root=BSDE_SolidBSP_BuildLeaf(world, &arr, &varr, mins, maxs);
			continue;
		}

		BSDE_ArraySkipUnknown(&arr, &varr);
		break;
	}
	arr++;

	if(!root)
	{
		*arrp=arr;
		*varrp=varr;
		return(NULL);
	}

	tmp=(BSDE_Solid *)malloc(sizeof(BSDE_Solid));
	memset(tmp, 0, sizeof(BSDE_Solid));
	tmp->solidtype=BSDE_SOLID_BSP;
	BSDE_Quat_Identity(tmp->rot);
	tmp->data=root;

	V3_COPY(mins, tmp->mins);
	V3_COPY(maxs, tmp->maxs);

	printf("bsp bbox (%g %g %g) (%g %g %g)\n",
		mins[0], mins[1], mins[2],
		maxs[0], maxs[1], maxs[2]);

	tmp->elastic=BSDE_M_DEF_ELASTIC;
	tmp->friction=BSDE_M_DEF_FRICTION;
	tmp->volume=(maxs[0]-mins[0])*(maxs[1]-mins[1])*(maxs[2]-mins[2]);
	tmp->mass=100000*tmp->volume;
	BSDE_BuildBoxInertiaTensor(tmp);

	printf("BSDE_SolidBSP_BuildArray: OK\n");

	*arrp=arr;
	*varrp=varr;
	return(tmp);
}
