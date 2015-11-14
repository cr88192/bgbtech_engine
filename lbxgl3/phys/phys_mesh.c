/*
Mesh-related physics stuff.
 */

#include <lbxgl2.h>

int LBXGL_Physics2_LinePlaneIntersect(double *start, double *end, double *norm,
	double *point)
{
	double dir[3], x;

	if(VecND_DotProduct(norm, norm, 3)==0)
	{
		VecND_Copy(start, point, 3);
		return(-1);
	}

	//calc direction
	VecND_Subtract(end, start, dir, 3);

	x=VecND_DotProduct(dir, norm, 3);
	x=(x<0)?-x:x;
	VecND_Scale(dir, 1/x, dir, 3);

	//calc intersection
	x=VecND_DotProduct(start, norm, 3)-norm[3];
	x=(x<0)?-x:x;
	VecND_AddScale(start, dir, x, point, 3);

	return(0);
}

int LBXGL_Physics2_CheckLinePlane(double *start, double *end, double *norm)
{
	double x;
	int i;

	if(VecND_DotProduct(norm, norm, 3)==0)
		return(0);

	i=0;
	x=VecND_DotProduct(start, norm, 3)-norm[3];
	if(x>=0)i|=1;
		else i|=2;

	x=VecND_DotProduct(end, norm, 3)-norm[3];
	if(x>=0)i|=1;
		else i|=2;

	return(i);
}

int LBXGL_Physics2_ClipLinePlane(double *start, double *end, double *norm)
{
	int i;

	i=LBXGL_Physics2_CheckLinePlane(start, end, norm);
	if(i!=3)return(i);

	LBXGL_Physics2_LinePlaneIntersect(start, end, norm, end);
	return(i);
}

int LBXGL_Physics2_CheckPlaneBox(double *norm, double *mins, double *maxs)
{
	double d0, d1;
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

	i=0;
	if(d0>=norm[3])i|=1;	//>=dist
	if(d1<norm[3])i|=2;	//<dist

	if(!i)
	{
		kprint("LBXGL_Physics2_CheckPlaneBox: fail (%lf %lf %lf %lf)\n",
			norm[0], norm[1], norm[2], norm[3]);
		*(int *)-1=-1;
	}

	return(i);
}

int LBXGL_Physics2_CheckPrimBBox(LBXGL_PolyPrim *prim, double *org,
	double *smove, double *emove, double *vel,
	double *mins, double *maxs,
	double *inorm, double *nd)
{
	LBXGL_Poly *cur;
	double norm[4];
	int i, c, r;
	double cog[3], d, d2, rnd;
	double rnorm[4], rmins[3], rmaxs[3];

	VecND_Zero(rnorm, 4);

/*
	for(i=0; i<3; i++)
	{
		rmins[i]=mins[i]-org[i];
		rmaxs[i]=maxs[i]-org[i];

		cog[i]=(rmins[i]+rmaxs[i])/2;
	} 
*/

	for(i=0; i<3; i++)
		cog[i]=(mins[i]+maxs[i])/2;

	rnd=*nd;
//	nd= 999999999;
//	nd=-999999999;

	c=2;
	cur=prim->poly;
	while(cur)
	{
		LBXGL_CalcPolyNormal(cur);
		VecNF2D_Copy(cur->normal, norm, 4);
		norm[3]+=VecND_DotProduct(org, norm, 3);

		i=LBXGL_Physics2_CheckPlaneBox(norm, mins, maxs);

		//"poor choice"
		if((i==3) && (rnd>100000000))
		{
			r=1;
			VecND_Copy(norm, rnorm, 4);
		}
#if 1
		d=VecND_DotProduct(norm, smove, 3)-norm[3];
		d2=VecND_DotProduct(norm, vel, 3);
//		if(d<0)d=-d;
		if((d>=0) && (d<rnd) && (d2<0))
		{
			r=1;
			VecND_Copy(norm, rnorm, 4);
			rnd=d;
		}
#endif
		c&=i;
		cur=cur->next;
	}

	if(c && r)
	{
		VecND_Copy(rnorm, inorm, 4);
		*nd=rnd;
	}

	return(c);
}

int LBXGL_Physics2_CheckCollidePrimBBox(LBXGL_Entity *ent1, LBXGL_Entity *ent2)
{
	LBXGL_PolyGroup *grp;
	LBXGL_PolyPrim *cur;
	double nd;
	int collide;

	VecND_Zero(ent1->phys->inorm, 3);
	ent1->phys->inorm[2]=1;
	nd=999999999;

	collide=0;
	switch(ent1->phys->modeltype)
	{
	case TYCRC_LBXGL_POLYGROUP_T:
		grp=ent1->phys->model;
		cur=grp->prim;
		while(cur)
		{
			collide|=LBXGL_Physics2_CheckPrimBBox(cur, ent1->phys->emove,
				ent2->phys->smove, ent2->phys->emove,
				ent2->phys->vel,
				ent2->phys->mmins, ent2->phys->mmaxs,
				ent1->phys->inorm, &nd);
			LBXGL_Physics2_LinePlaneIntersect(
				ent2->phys->smove, ent2->phys->emove,
				ent1->phys->inorm, ent1->phys->iorg);
			cur=cur->next;
		}
		break;
	case TYCRC_LBXGL_POLYPRIM_T:
		cur=ent1->phys->model;
		collide=LBXGL_Physics2_CheckPrimBBox(cur, ent1->phys->emove,
			ent2->phys->smove, ent2->phys->emove, ent2->phys->vel,
			ent2->phys->mmins, ent2->phys->mmaxs,
			ent1->phys->inorm, &nd);
		LBXGL_Physics2_LinePlaneIntersect(
			ent2->phys->smove, ent2->phys->emove,
			ent1->phys->inorm, ent1->phys->iorg);
		break;

	case TYCRC_LBXGL_BSPMODEL_T:
		collide=LBXGL_BspPhys_CheckBoxMoveModelD(ent1->phys->model,
			ent1->phys->emove,
			ent2->phys->smove, ent2->phys->emove,
			ent2->phys->mmins, ent2->phys->mmaxs,
			ent1->phys->inorm);
		LBXGL_Physics2_LinePlaneIntersect(
			ent2->phys->smove, ent2->phys->emove,
			ent1->phys->inorm, ent1->phys->iorg);
		break;

	default:
		kprint("LBXGL_Physics2_CheckCollidePrimBBox: bad modeltype %s\n",
			ObjType_GetTypeName(ent1->phys->model));
		*(int *)-1=-1;
		break;
	}
	return(collide);
}

int LBXGL_Physics2_CheckCollidePrimObb(LBXGL_Entity *ent1, LBXGL_Entity *ent2)
{
	LBXGL_PolyGroup *grp;
	LBXGL_PolyPrim *cur;
	double nd;
	int collide;

	double sxform[16], exform[16], tmat0[16], tmat1[16];
	int i, j;

	MatND_FillIdentity(tmat0, 4);
	Vec16D_TranslateMatrix(tmat0, ent2->phys->smove, tmat1);
	Vec16D_RotateMatrix(tmat1, ent2->phys->sang, sxform);

	MatND_FillIdentity(tmat0, 4);
	Vec16D_TranslateMatrix(tmat0, ent2->phys->emove, tmat1);
	Vec16D_RotateMatrix(tmat1, ent2->phys->eang, exform);


	VecND_Zero(ent1->phys->inorm, 3);
	ent1->phys->inorm[2]=1;
	nd=999999999;

	collide=0;
	switch(ent1->phys->modeltype)
	{
#if 1
	case TYCRC_LBXGL_POLYGROUP_T:
		grp=ent1->phys->model;
		cur=grp->prim;
		while(cur)
		{
			collide|=LBXGL_Physics2_CheckPrimBBox(cur, ent1->phys->emove,
				ent2->phys->smove, ent2->phys->emove,
				ent2->phys->vel,
				ent2->phys->mmins, ent2->phys->mmaxs,
				ent1->phys->inorm, &nd);
			LBXGL_Physics2_LinePlaneIntersect(
				ent2->phys->smove, ent2->phys->emove,
				ent1->phys->inorm, ent1->phys->iorg);
			cur=cur->next;
		}
		break;
	case TYCRC_LBXGL_POLYPRIM_T:
		cur=ent1->phys->model;
		collide=LBXGL_Physics2_CheckPrimBBox(cur, ent1->phys->emove,
			ent2->phys->smove, ent2->phys->emove, ent2->phys->vel,
			ent2->phys->mmins, ent2->phys->mmaxs,
			ent1->phys->inorm, &nd);
		LBXGL_Physics2_LinePlaneIntersect(
			ent2->phys->smove, ent2->phys->emove,
			ent1->phys->inorm, ent1->phys->iorg);
		break;
#endif

	case TYCRC_LBXGL_BSPMODEL_T:
		collide=LBXGL_BspPhys_CheckObbMoveModelD(ent1->phys->model,
			ent1->phys->emove,
			sxform, exform,
			ent2->phys->rmins, ent2->phys->rmaxs,
			ent1->phys->iorg, ent1->phys->inorm);
		break;

	default:
		kprint("LBXGL_Physics2_CheckCollidePrimObb: bad modeltype %s\n",
			ObjType_GetTypeName(ent1->phys->model));
		*(int *)-1=-1;
		break;
	}
	return(collide);
}
