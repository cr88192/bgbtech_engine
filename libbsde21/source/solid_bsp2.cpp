/*
Plan: support movable BSP trees.
This is a near-duplication of BSP's, except that orientation needs to be taken
into account, which risks loosing speed.

Likely, this will be called by BSP code upon detecting semistatic or
non-static conditions.
*/

#include "bsde_internal.h"

#define BSDE_BSP_SKIPSAME
#define BSDE_BSP_AUTOFLIP

void BSDE_SolidBSP2_FaceExtents(BSDE_BSPFace *face, bsde_real *xform,
	bsde_real *norm, bsde_real *min, bsde_real *max)
{
	bsde_real tv[3];
	bsde_real m, n, d;
	int i;

	m=BSDE_M_BIGVAL;
	n=-BSDE_M_BIGVAL;
	for(i=0; i<face->nvecs; i++)
	{
		BSDE_Plane_TransformPoint(face->vecs+i*3, xform, tv);
		d=V3_DOT(tv, norm);
		m=(m<d)?m:d;
		n=(n>d)?n:d;
	}

	*min=m;
	*max=n;
}

int BSDE_SolidBSP2_CheckFastMoveFace(BSDE_BSPFace *face, bsde_real *xform,
	BSDE_Solid *obj, bsde_real *pos, bsde_real *org, bsde_real *norm, int max)
{
	bsde_real dv[3], tv[4], tv2[4];
	bsde_real sd, ed, d, pd;
	bsde_real dm, dn;
	int i, j;

	BSDE_Plane_TransformNormal(face->norm, xform, tv);
	BSDE_PlaneMoveExtents(obj, tv, &dm, &dn);
	if(dm>tv[3])return(0);
	if(dn<tv[3])return(0);

	for(i=0; i<face->nvecs; i++)
	{
		BSDE_Plane_TransformNormal(face->evecs+i*4, xform, tv2);
		BSDE_PlaneMoveExtents(obj, tv2, &dm, &dn);

		d=tv2[3]-0.05;
		if(dm>=d)return(0);
	}

	pd=V3_DOT(obj->vel, tv);
	if(pd>0)pd=0;
	V3_ADDSCALE(obj->vel, tv, -pd*1.10, dv);
	V3_COPY(dv, obj->vel);
	V3_COPY(dv, obj->e_vel);

	//object might collide, forcibly stop
	V3_ZERO(obj->tvel);
	V3_ZERO(obj->e_tvel);

	V3_ZERO(obj->ivel);
	V3_ZERO(obj->itvel);

	V3_COPY(obj->org, obj->e_org);
	V4_COPY(obj->rot, obj->e_rot);
	BSDE_Mat4_Copy(obj->xform, obj->e_xform);

	return(0);
}

int BSDE_SolidBSP2_CheckGenericMoveFace(BSDE_BSPFace *face, bsde_real *xform,
	BSDE_Solid *obj, bsde_real *pos, bsde_real *org, bsde_real *norm, int max)
{
	bsde_real tv[4], tv2[4];
	bsde_real mpos[3], tv0[3], tv1[3];
	bsde_real sd, ed, d, pd;
	bsde_real dm, dn;
	int i, j;

	BSDE_Plane_TransformNormal(face->norm, xform, tv);

	BSDE_PlaneMoveExtents(obj, tv, &dm, &dn);
	if((dm>tv[3]) && (dn>tv[3]))return(0);
	if((dm<tv[3]) && (dn<tv[3]))return(0);
	pd=tv[3]-dm;

	for(i=0; i<face->nvecs; i++)
	{
		BSDE_Plane_TransformNormal(face->evecs+i*4, xform, tv2);

		BSDE_PlaneMoveExtents(obj, tv2, &dm, &dn);
		d=tv2[3]-0.05;
		if(dm>=d)return(0);
	}

	if(max>0)
	{
		d=V3_DOT(obj->org, tv);
		V3_ADDSCALE(obj->org, tv, -d-pd, org);
		org[3]=pd;
		V4_COPY(tv, norm);
	}
	return(1);
}

void BSDE_SolidBSP2_ObbFaceIntersect(bsde_real *mins, bsde_real *maxs,
	bsde_real *pos, BSDE_BSPFace *face, bsde_real *xform, bsde_real *io)
{
	bsde_real pts[64*3];
	bsde_real bo[3], org[3], tv[4];
	bsde_real d, pd;
	int i, j, k, l, n;

	BSDE_Plane_TransformNormal(face->norm, xform, tv);

	BSDE_SolidOBB_GenPoints4x(mins, maxs, pos, pts);

	V3_ZERO(bo);
	n=0;

	for(i=0; i<64; i++)
	{
		V3_COPY(pts+i*3, org);

		d=V3_NDOT(org, tv);
		if(d>0)continue;

		V3_ADD(bo, org, bo);
		n++;
	}

	if(!n)n++;
	V3_SCALE(bo, 1.0/n, io);
}

int BSDE_SolidBSP2_CheckObbMoveFace(BSDE_BSPFace *face, bsde_real *xform,
	BSDE_Solid *obj, bsde_real *pos, bsde_real *org, bsde_real *norm, int max)
{
	bsde_real tv[4], tv2[4];
	bsde_real sd, ed, d, pd;
	bsde_real dm, dn;
	int i, j, k;

	BSDE_Plane_TransformNormal(face->norm, xform, tv);

	BSDE_SolidOBB_PlaneExtents(obj->mins, obj->maxs, pos,
		tv, &dm, &dn);
	if((dm>tv[3]) && (dn>tv[3]))return(0);
	if((dm<tv[3]) && (dn<tv[3]))return(0);
	pd=tv[3]-dm;

	for(i=0; i<face->nvecs; i++)
	{
		BSDE_Plane_TransformNormal(face->evecs+i*4, xform, tv2);

		BSDE_SolidOBB_PlaneExtents(obj->mins, obj->maxs, pos,
			tv2, &dm, &dn);
		d=tv2[3]-0.05;
		if(dm>=d)return(0);
	}

	for(i=0; i<3; i++)
	{
		BSDE_SolidBSP2_FaceExtents(face, xform, pos+i*4, &dm, &dn);
		d=V3_DOT(pos+3*4, pos+i*4);
		if(((dm-d)>obj->maxs[i]) || ((dn-d)<obj->mins[i]))
			return(0);
	}

	if(max<=0)return(1);	//no space for contact info

	//FIXME: make contact points be within face
	V4_COPY(tv, norm);
	BSDE_SolidOBB_ObbPlaneIntersect(
		obj->mins, obj->maxs, pos, norm, org);
	d=V3_DOT(org, norm);
	org[3]=norm[3]-d;

	return(1);
}

int BSDE_SolidBSP2_CheckSphereFace(BSDE_BSPFace *face, bsde_real *xform,
	BSDE_Solid *obj, bsde_real *pos, bsde_real *org, bsde_real *norm, int max)
{
	bsde_real tv[4], tv2[4];
	bsde_real io[3], iv[3], d, r;
	int i;

	BSDE_Plane_TransformNormal(face->norm, xform, tv);

	r=obj->radius*obj->radius;
	V3_COPY(pos+3*4, io);

	d=V3_NDOT(io, tv);
	V3_ADDSCALE(io, tv, -d, io);

	for(i=0; i<face->nvecs; i++)
	{
		BSDE_Plane_TransformNormal(face->evecs+i*4, xform, tv2);
		d=V3_NDOT(io, tv2)-0.05;
		if(d>0) { V3_ADDSCALE(io, tv2, -d, io); }
	}

	d=V3_DIST(pos+3*4, io);
	if(d>=obj->radius)return(0);

	if(max>0)
	{
//		V4_COPY(tv, norm);
		V3_SUB(pos+3*4, io, norm);
		V3_NORMALIZE(norm, norm);
		norm[3]=V3_DOT(io, norm);

		V3_SUB(io, pos+3*4, iv);
		d=obj->radius/V3_LEN(iv);
		V3_SCALE(iv, d, iv);
		V3_ADD(pos+3*4, iv, org);
		org[3]=-V3_NDOT(org, tv);
	}

	return(1);
}

int BSDE_SolidBSP2_CheckHullFace(BSDE_BSPFace *face, bsde_real *xform,
	BSDE_Solid *obj, bsde_real *pos, bsde_real *org, bsde_real *norm, int max)
{
	bsde_real tv[4], tv2[4];
	bsde_real sd, ed, d, pd;
	bsde_real dm, dn;
	int i, j, k;

	BSDE_Plane_TransformNormal(face->norm, xform, tv);

	BSDE_PlaneMoveExtents(obj, tv, &dm, &dn);
	if((dm>tv[3]) && (dn>tv[3]))return(0);
	if((dm<tv[3]) && (dn<tv[3]))return(0);
	pd=tv[3]-dm;

	for(i=0; i<face->nvecs; i++)
	{
		BSDE_Plane_TransformNormal(face->evecs+i*4, xform, tv2);
		BSDE_PlaneMoveExtents(obj, tv2, &dm, &dn);
		d=tv2[3]-0.05;
		if(dm>=d)return(0);
	}

	for(i=0; i<obj->n_faces; i++)
	{
		BSDE_SolidBSP2_FaceExtents(face, xform,
			obj->norms+i*4, &dm, &dn);
		d=V3_DOT(pos+3*4, obj->norms+i*4);
		if((dm-d)>obj->norms[i*4+3])
			return(0);
	}

	if(max<=0)return(1);	//no space for contact info

	//FIXME: make contact points be within face
	V4_COPY(tv, norm);
	BSDE_SolidHull_HullPlaneIntersect(obj, pos, norm, org);
	d=V3_DOT(org, norm);
	org[3]=norm[3]-d;

	return(1);
}

int BSDE_SolidBSP2_CheckMoveFace(BSDE_BSPFace *face, bsde_real *xform,
	BSDE_Solid *obj, bsde_real *pos, bsde_real *org, bsde_real *norm, int max)
{
	bsde_real dv[3], tv[4];
	bsde_real m, n, o, p;
	bsde_real m2, n2, d, d0, d1, v;
	int i;


	BSDE_Plane_TransformNormal(face->norm, xform, tv);

	//speed checking
	BSDE_PlaneExtentsPos(obj, obj->xform, tv, &m, &n);
	BSDE_PlaneExtentsPos(obj, obj->e_xform, tv, &o, &p);
	m2=(m<o)?m:o;
	n2=(n>p)?n:p;

	if((m2>tv[3]) || (n2<tv[3]))
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

		i=BSDE_SolidBSP2_CheckFastMoveFace(
			face, xform, obj, pos, org, norm, max);
		return(i);
	}

	switch(obj->solidtype)
	{
	case BSDE_SOLID_AABB:
	case BSDE_SOLID_OBB:
		i=BSDE_SolidBSP2_CheckObbMoveFace(
			face, xform, obj, pos, org, norm, max);
		break;
	case BSDE_SOLID_SPHERE:
		i=BSDE_SolidBSP2_CheckSphereFace(
			face, xform, obj, pos, org, norm, max);
		break;
	case BSDE_SOLID_HULL:
		i=BSDE_SolidBSP2_CheckHullFace(
			face, xform, obj, pos, org, norm, max);
		break;
	default:
		i=BSDE_SolidBSP2_CheckGenericMoveFace(
			face, xform, obj, pos, org, norm, max);
		break;
	}
	return(i);
}

int BSDE_SolidBSP2_CheckMoveLeaf(BSDE_BSPNode *node, bsde_real *xform,
	BSDE_Solid *obj, bsde_real *pos, bsde_real *org, bsde_real *norm, int max)
{
	BSDE_BSPFace *cur;
	int i, j;

	i=0;
	cur=node->face;
	while(cur)
	{
		j=BSDE_SolidBSP2_CheckMoveFace(cur, xform, obj, pos,
			org+i*4, norm+i*4, max);
		i+=j;
		max-=j;
		cur=cur->next;
	}
	return(i);
}

int BSDE_SolidBSP2_CheckMoveNode(BSDE_BSPNode *node, bsde_real *xform,
	BSDE_Solid *obj, bsde_real *pos, bsde_real *org, bsde_real *norm, int max)
{
	bsde_real tv[4];
	bsde_real dm, dn, sd, ed;
	int i;

	if(!node->lnode)
	{
		i=BSDE_SolidBSP2_CheckMoveLeaf(node, xform,
			obj, pos, org, norm, max);
		return(i);
	}

	BSDE_Plane_TransformNormal(node->norm, xform, tv);

	sd=V3_DOT(obj->org, tv)-node->norm[3];
	ed=V3_DOT(obj->e_org, tv)-node->norm[3];
	BSDE_PlaneFastMoveExtents(obj, tv, &dm, &dn);

	if((dm<=tv[3]) && (dn<=tv[3]))
	{
		i=BSDE_SolidBSP2_CheckMoveNode(node->lnode, xform, obj, pos,
			org, norm, max);
		return(i);
	}

	if((dm>=tv[3]) && (dn>=tv[3]))
//	if(dm>=tv[3])
	{
		i=BSDE_SolidBSP2_CheckMoveNode(node->rnode, xform, obj, pos,
			org, norm, max);
		return(i);
	}

	if(sd<0)
	{
		i=BSDE_SolidBSP2_CheckMoveNode(node->lnode, xform, obj, pos,
			org, norm, max);
		i+=BSDE_SolidBSP2_CheckMoveNode(node->rnode, xform, obj, pos,
			org+i*4, norm+i*4, max-i);
		return(i);
	}else
	{
		i=BSDE_SolidBSP2_CheckMoveNode(node->rnode, xform, obj, pos,
			org, norm, max);
		i+=BSDE_SolidBSP2_CheckMoveNode(node->lnode, xform, obj, pos,
			org+i*4, norm+i*4, max-i);
		return(i);
	}

	return(0);
}


int BSDE_SolidBSP2_CheckCollide(
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

	i=BSDE_SolidBSP2_CheckMoveNode((BSDE_BSPNode *)aobj->data, apos,
		bobj, bpos, org, norm, 0);
	return(i!=0);
//	return(1);
}

int BSDE_SolidBSP2_CollideMultiContact(
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

//	printf("BSDE_SolidBSP2_CollideMultiContact:\n");

	if(bobj->solidtype==BSDE_SOLID_BSP)
	{
		if(aobj->solidtype==BSDE_SOLID_BSP)
			return(0);
		tobj=aobj; aobj=bobj; bobj=tobj;
		v0=apos; apos=bpos; bpos=v0;
		flip=1;
	}else flip=0;

	i=BSDE_SolidBSP2_CheckMoveNode((BSDE_BSPNode *)aobj->data, apos,
		bobj, bpos, org, norm, 64);
	if(i>64)i=64;

//	if(i)printf("BSDE_SolidBSP2_CollideMultiContact: %d contacts\n", i);

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



int BSDE_SolidBSP2_CheckIsecBoxFace(BSDE_BSPFace *face, bsde_real *xform,
	bsde_real *mins, bsde_real *maxs, bsde_real *pos)
{
	bsde_real tv[4], tv2[4];
	bsde_real sd, ed, d, pd;
	bsde_real dm, dn;
	int i, j, k;

	BSDE_Plane_TransformNormal(face->norm, xform, tv);

	BSDE_SolidOBB_PlaneExtents(mins, maxs, pos, tv, &dm, &dn);
	if((dm>tv[3]) && (dn>tv[3]))return(0);
	if((dm<tv[3]) && (dn<tv[3]))return(0);
	pd=tv[3]-dm;

	for(i=0; i<face->nvecs; i++)
	{
		BSDE_Plane_TransformNormal(face->evecs+i*4, xform, tv2);

		BSDE_SolidOBB_PlaneExtents(mins, maxs, pos,
			tv2, &dm, &dn);
		d=tv2[3]-0.05;
		if(dm>=d)return(0);
	}

	for(i=0; i<3; i++)
	{
		BSDE_SolidBSP2_FaceExtents(face, xform, pos+i*4, &dm, &dn);
		d=V3_DOT(pos+3*4, pos+i*4);
		if(((dm-d)>maxs[i]) || ((dn-d)<mins[i]))
			return(0);
	}

	return(1);
}

int BSDE_SolidBSP2_CheckIsecBoxLeaf(BSDE_BSPNode *node, bsde_real *xform,
	bsde_real *mins, bsde_real *maxs, bsde_real *pos)
{
	BSDE_BSPFace *cur;
	int i, j;

	i=0;
	cur=node->face;
	while(cur)
	{
		i=BSDE_SolidBSP2_CheckIsecBoxFace(cur, xform, mins, maxs, pos);
		if(i)return(i);
		cur=cur->next;
	}
	return(0);
}

int BSDE_SolidBSP2_CheckIsecBoxNode(BSDE_BSPNode *node, bsde_real *xform,
	bsde_real *mins, bsde_real *maxs, bsde_real *pos)
{
	bsde_real tv[4];
	bsde_real dm, dn, sd, ed;
	int i;

	if(!node->lnode)
	{
		i=BSDE_SolidBSP2_CheckIsecBoxLeaf(node, xform,
			mins, maxs, pos);
		return(i);
	}

	BSDE_Plane_TransformNormal(node->norm, xform, tv);
	BSDE_SolidOBB_PlaneExtents(mins, maxs, pos, tv, &dm, &dn);

	if((dm<=tv[3]) && (dn<=tv[3]))
	{
		i=BSDE_SolidBSP2_CheckIsecBoxNode(node->lnode, xform,
			mins, maxs, pos);
		return(i);
	}

	if((dm>=tv[3]) && (dn>=tv[3]))
	{
		i=BSDE_SolidBSP2_CheckIsecBoxNode(node->rnode, xform,
			mins, maxs, pos);
		return(i);
	}

	i=BSDE_SolidBSP2_CheckIsecBoxNode(node->lnode, xform, mins, maxs, pos);
	if(i)return(i);
	i=BSDE_SolidBSP2_CheckIsecBoxNode(node->rnode, xform, mins, maxs, pos);
	if(i)return(i);

	return(0);
}

int BSDE_SolidBSP2_BoxCollide(BSDE_Solid *obj, bsde_real *xform,
	bsde_real *mins, bsde_real *maxs, bsde_real *pos)
{
	int i;
	i=BSDE_SolidBSP2_CheckIsecBoxNode((BSDE_BSPNode *)obj->data,
		xform, mins, maxs, pos);
	return(i);
}
