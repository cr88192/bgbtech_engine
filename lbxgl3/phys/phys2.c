//attempt rewrite of physics in a form capable of more complex forms of movement
//and more types of collision detection

#include <lbxgl2.h>

extern double lbxgl_physics_gravity[3];
extern double lbxgl_physics_ground;
extern double lbxgl_physics_sky;
extern double lbxgl_physics_friction;

static int lbxgl_physics_sunum=1;

void LBXGL_Physics2_BoxMove(double *bmins, double *bmaxs,
	double *start, double *end, double *mins, double *maxs)
{
	double mins2[3], maxs2[3];
	int i;

	VecND_Add(bmins, start, mins, 3);
	VecND_Add(bmaxs, start, maxs, 3);

	VecND_Add(bmins, end, mins2, 3);
	VecND_Add(bmaxs, end, maxs2, 3);

	for(i=0; i<3; i++)
	{
		if(mins2[i]<mins[i])mins[i]=mins2[i];
		if(maxs2[i]>maxs[i])maxs[i]=maxs2[i];
	}
}

void LBXGL_Physics2_SetupEntity(LBXGL_Entity *ent, float dt)
{
	LBXGL_Entity *cur;

	char *s;
	void *p;

	if(!ent->phys)ent->phys=kalloc(sizeof(LBXGL_PhysInfo));

	if(ent->phys->sunum==lbxgl_physics_sunum)
		return;
	ent->phys->sunum=lbxgl_physics_sunum;

	ent->phys->dt=dt;

	LBXGL_Entity_GetPropertyDVector(ent, "origin", ent->phys->org, 3);
	LBXGL_Entity_GetPropertyDVector(ent, "velocity", ent->phys->vel, 3);
	LBXGL_Entity_GetPropertyDVector(ent, "angles", ent->phys->ang, 3);
	LBXGL_Entity_GetPropertyDVector(ent, "avelocity",
		ent->phys->avel, 3);

	if(ent->rup)
	{
		LBXGL_Physics2_SetupEntity(ent->rup, dt);

		VecND_Add(ent->phys->org, ent->rup->phys->org,
			ent->phys->org, 3);
//		VecND_Add(ent->phys->vel, ent->rup->phys->vel,
//			ent->phys->vel, 3);

		VecND_Add(ent->phys->ang, ent->rup->phys->ang,
			ent->phys->ang, 3);
//		VecND_Add(ent->phys->avel, ent->rup->phys->avel,
//			ent->phys->avel, 3);
	}

	VecND_Copy(ent->phys->org, ent->phys->sorg, 3);
	VecND_Copy(ent->phys->vel, ent->phys->svel, 3);
	VecND_Copy(ent->phys->ang, ent->phys->sang, 3);
	VecND_Copy(ent->phys->avel, ent->phys->savel, 3);

/*
	cur=ent->rup;
	while(cur)
	{
		VecND_Add()
		cur=cur->rup;
	}
*/

	LBXGL_Entity_GetPropertyDVector(ent, "mins", ent->phys->mins, 3);
	LBXGL_Entity_GetPropertyDVector(ent, "maxs", ent->phys->maxs, 3);
	VecND_Copy(ent->phys->mins, ent->phys->rmins, 3);
	VecND_Copy(ent->phys->maxs, ent->phys->rmaxs, 3);

	s=LBXGL_Entity_GetPropertyString(ent, "movetype");
	if(!s)s="move_none";
	ent->phys->movestr=s;
	ent->phys->movetype=ObjType_StringCRC(s);

	s=LBXGL_Entity_GetPropertyString(ent, "solidtype");
	if(!s)s="solid_not";
	ent->phys->solidstr=s;
	ent->phys->solidtype=ObjType_StringCRC(s);

	p=LBXGL_Entity_GetProperty(ent, "mdl");
	ent->phys->model=p;
	ent->phys->modeltype=ObjType_GetTypeCRC(p);

	s=LBXGL_Entity_GetPropertyString(ent, "classname");
	if(!s || !*s)s="<none>";

//	kprint("phys2: %s %s %s\n", s, ent->phys->movestr, ent->phys->solidstr);

	if(ent->phys->solidtype==TYCRC_SOLID_OBB)
	{
		VecND_Scale(ent->phys->mins, 1.5, ent->phys->mins, 3);
		VecND_Scale(ent->phys->maxs, 1.5, ent->phys->maxs, 3);
	}

	if(ent->phys->solidtype==TYCRC_SOLID_SKEL)
		LBXGL_PhysSkel_SetupEntity(ent);
}

void LBXGL_Physics2_FinishMove(LBXGL_Entity *ent)
{
	double org[3], vel[3];
	double ang[3], avel[3];

	if(ent->phys->solidtype==TYCRC_SOLID_SKEL)
		LBXGL_PhysSkel_FinishMove(ent);

	VecND_Copy(ent->phys->org, org, 3);
	VecND_Copy(ent->phys->vel, vel, 3);
	VecND_Copy(ent->phys->ang, ang, 3);
	VecND_Copy(ent->phys->avel, avel, 3);

	if(ent->rup)
	{
		VecND_Subtract(org, ent->rup->phys->sorg, org, 3);
//		VecND_Subtract(vel, ent->rup->phys->vel, vel, 3);
		VecND_Subtract(ang, ent->rup->phys->sang, ang, 3);
//		VecND_Subtract(avel, ent->rup->phys->avel, avel, 3);
	}

	if(VecND_Distance(ent->phys->sorg, ent->phys->org, 3)>0.000001)
		LBXGL_Entity_SetPropertyDVector(ent, "origin", org, 3);
	if(VecND_Distance(ent->phys->svel, ent->phys->vel, 3)>0.000001)
		LBXGL_Entity_SetPropertyDVector(ent, "velocity", vel, 3);

	if(VecND_Distance(ent->phys->sang, ent->phys->ang, 3)>0.000001)
		LBXGL_Entity_SetPropertyDVector(ent, "angles", ang, 3);
	if(VecND_Distance(ent->phys->savel, ent->phys->avel, 3)>0.000001)
		LBXGL_Entity_SetPropertyDVector(ent, "avelocity", avel, 3);
}


LBXGL_Entity *LBXGL_Physics2_QueryImpacts(LBXGL_Entity *ent, LBXGL_Entity *ents)
{
	LBXGL_Entity *cur, *fst, *lst;

	fst=NULL;
	lst=NULL;

	cur=ents;
	while(cur)
	{
		if((cur!=ent) && LBXGL_Physics_BoxCollideP(
			ent->phys->mmins, ent->phys->mmaxs,
			cur->phys->mmins, cur->phys->mmaxs))
		{
			if(lst)
			{
				cur->phys->chain=NULL;
				lst->phys->chain=cur;
				lst=cur;
			}else
			{
				cur->phys->chain=NULL;
				fst=cur;
				lst=cur;
			}
		}
		cur=cur->next;
	}

	return(fst);
}

LBXGL_Entity *LBXGL_Physics2_QueryVolume(double *mins, double *maxs,
	LBXGL_Entity *ents, int chl)
{
	LBXGL_Entity *cur, *fst, *lst;

	fst=NULL;
	lst=NULL;

	cur=ents;
	while(cur)
	{
		if(LBXGL_Physics_BoxCollideP(mins, maxs,
			cur->phys->mmins, cur->phys->mmaxs))
		{
			if(lst)
			{
				cur->phys->chains[chl]=NULL;
				lst->phys->chains[chl]=cur;
				lst=cur;
			}else
			{
				cur->phys->chains[chl]=NULL;
				fst=cur;
				lst=cur;
			}
		}
		cur=cur->next;
	}

	return(fst);
}

LBXGL_Entity *LBXGL_Physics2_SubQueryVolume(double *mins, double *maxs,
	LBXGL_Entity *ents, int schl, int dchl)
{
	LBXGL_Entity *cur, *fst, *lst;

	fst=NULL;
	lst=NULL;

	cur=ents;
	while(cur)
	{
		if(LBXGL_Physics_BoxCollideP(mins, maxs,
			cur->phys->mmins, cur->phys->mmaxs))
		{
			if(lst)
			{
				cur->phys->chains[dchl]=NULL;
				lst->phys->chains[dchl]=cur;
				lst=cur;
			}else
			{
				cur->phys->chains[dchl]=NULL;
				fst=cur;
				lst=cur;
			}
		}
		cur=cur->phys->chains[schl];
	}

	return(fst);
}

int LBXGL_Physics2_NOkAxis(LBXGL_Entity *ent1, LBXGL_Entity *ent2)
{
	double *aorg, *amins, *amaxs;
	double *borg, *bmins, *bmaxs;
	double ma[3], na[3], mb[3], nb[3];
	int i, j, k;

	aorg=ent1->phys->org;
	amins=ent1->phys->mins;
	amaxs=ent1->phys->maxs;

	borg=ent2->phys->org;
	bmins=ent2->phys->mins;
	bmaxs=ent2->phys->maxs;

	k=0;
	for(i=0; i<6; i++)
	{
		for(j=0; j<3; j++)
		{
			ma[j]=aorg[j]+amins[j];
			na[j]=aorg[j]+amaxs[j];
			mb[j]=borg[j]+bmins[j];
			nb[j]=borg[j]+bmaxs[j];
		}
		if(i<3)
		{
			na[i]=999999999;
			mb[i]=-999999999;
		}else
		{
			ma[i-3]=-999999999;
			nb[i-3]=999999999;
		}
		if(LBXGL_Physics_BoxCollideP(ma, na, mb, nb))k|=1<<i;
	}
	return(k);
}

int LBXGL_Physics2_NOkAxis2(
	double *aorg, double *amins, double *amaxs,
	double *borg, double *bmins, double *bmaxs)
{
	double ma[3], na[3], mb[3], nb[3];
	int i, j, k;

	k=0;
	for(i=0; i<6; i++)
	{
		for(j=0; j<3; j++)
		{
			ma[j]=aorg[j]+amins[j];
			na[j]=aorg[j]+amaxs[j];
			mb[j]=borg[j]+bmins[j];
			nb[j]=borg[j]+bmaxs[j];
		}
		if(i<3)
		{
			na[i]=999999999;
			mb[i]=-999999999;
		}else
		{
			ma[i-3]=-999999999;
			nb[i-3]=999999999;
		}
		if(LBXGL_Physics_BoxCollideP(ma, na, mb, nb))k|=1<<i;
	}
	return(k);
}

int LBXGL_Physics2_CheckCollideBBox(LBXGL_Entity *ent1, LBXGL_Entity *ent2)
{
//	int collide;
	int i, j;

	i=LBXGL_Physics_BoxCollideP(
		ent1->phys->mmins, ent1->phys->mmaxs,
		ent2->phys->mmins, ent2->phys->mmaxs);
	if(!i)return(0);

	i=LBXGL_Physics2_NOkAxis(ent1, ent2);
	if(!i)return(0);

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

//	if((i&1) && (vel[0]>0))vel[0]=0;
//	if((i&2) && (vel[1]>0))vel[1]=0;
//	if((i&4) && (vel[2]>0))vel[2]=0;
//	if((i&8) && (vel[0]<0))vel[0]=0;
//	if((i&16) && (vel[1]<0))vel[1]=0;
//	if((i&32) && (vel[2]<0))vel[2]=0;

	return(0);
}

int LBXGL_Physics2_CheckCollide(LBXGL_Entity *ent1, LBXGL_Entity *ent2)
{
	int collide;
	int i, j;

//	kprint("check collide %s %s\n",
//		LBXGL_Entity_GetPropertyString(ent1, "classname"),
//		LBXGL_Entity_GetPropertyString(ent2, "classname"));

	VecND_Zero(ent2->phys->inorm, 4);
	VecND_Zero(ent2->phys->iorg, 3);

	collide=0;
	switch(ent1->phys->solidtype)
	{
	case TYCRC_SOLID_NOT:
		collide=0;	//can't collide
		break;
	case TYCRC_SOLID_TRIGGER:
		switch(ent2->phys->solidtype)
		{
		case TYCRC_SOLID_NOT:
		case TYCRC_SOLID_TRIGGER:
		case TYCRC_SOLID_PRIM:
		case TYCRC_SOLID_BSP:
			collide=0;
			break;
		default:
			collide=1;	//very sensitive
			break;
		}
		break;

	case TYCRC_SOLID_SKEL:
	case TYCRC_SOLID_BBOX:
		switch(ent2->phys->solidtype)
		{
		case TYCRC_SOLID_NOT:
			collide=0;	//can't collide
			break;
		case TYCRC_SOLID_TRIGGER:
			collide=0;	//doesn't matter
			break;
		case TYCRC_SOLID_SKEL:
		case TYCRC_SOLID_BBOX:
			collide=0;
			collide=LBXGL_Physics2_CheckCollideBBox(ent1, ent2);
			break;

		case TYCRC_SOLID_PRIM:
		case TYCRC_SOLID_BSP:
			collide=LBXGL_Physics2_CheckCollidePrimBBox(ent2, ent1);
			break;

		case TYCRC_SOLID_OBB:
			collide=LBXGL_PhysSolid_CheckObbEnt(ent1, ent2);
			break;

		default:
			kprint("LBXGL_Physics2_CheckCollide: bad solidtype %s\n",
				ent2->phys->solidstr);
			break;
		}
		break;
	case TYCRC_SOLID_PRIM:
	case TYCRC_SOLID_BSP:
		switch(ent2->phys->solidtype)
		{
		case TYCRC_SOLID_NOT:
			collide=0;	//can't collide
			break;
		case TYCRC_SOLID_TRIGGER:
			collide=0;	//doesn't matter
			break;
		case TYCRC_SOLID_SKEL:
		case TYCRC_SOLID_BBOX:
			collide=LBXGL_Physics2_CheckCollidePrimBBox(ent1, ent2);
			break;
		case TYCRC_SOLID_OBB:
			collide=LBXGL_Physics2_CheckCollidePrimObb(ent1, ent2);
			break;
		case TYCRC_SOLID_PRIM:
		case TYCRC_SOLID_BSP:
			collide=0;	//leave for later
			break;
		default:
			kprint("LBXGL_Physics2_CheckCollide: bad solidtype %s\n",
				ent2->phys->solidstr);
			break;
		}
		break;

	case TYCRC_SOLID_OBB:
		switch(ent2->phys->solidtype)
		{
		case TYCRC_SOLID_NOT:
			collide=0;	//can't collide
			break;
		case TYCRC_SOLID_TRIGGER:
			collide=0;	//doesn't matter
			break;
		case TYCRC_SOLID_SKEL:
		case TYCRC_SOLID_BBOX:
		case TYCRC_SOLID_OBB:
			collide=LBXGL_PhysSolid_CheckObbEnt(ent1, ent2);
			break;

		case TYCRC_SOLID_PRIM:
		case TYCRC_SOLID_BSP:
//			collide=LBXGL_Physics2_CheckCollidePrimBBox(ent2, ent1);
			collide=LBXGL_Physics2_CheckCollidePrimObb(ent2, ent1);
			break;

		default:
			kprint("LBXGL_Physics2_CheckCollide: bad solidtype %s\n",
				ent2->phys->solidstr);
			break;
		}
		break;

	default:
		kprint("LBXGL_Physics2_CheckCollide: bad solidtype %s\n",
			ent1->phys->solidstr);
		break;
	}

	return(collide);
}

void LBXGL_Physics2_PredictMove(LBXGL_Entity *ent, LBXGL_Entity *ents,
	float dt)
{
	double gdir[3], vel[3];
	double x, y;

	LBXGL_Physics2_CheckOnGround(ent, ents, dt);

	switch(ent->phys->movetype)
	{
	case TYCRC_MOVE_NONE:
		VecND_Copy(ent->phys->org, ent->phys->smove, 3);
		VecND_Copy(ent->phys->org, ent->phys->emove, 3);

		VecND_Copy(ent->phys->ang, ent->phys->sang, 3);
		VecND_AddScale(ent->phys->ang, ent->phys->avel, dt,
			ent->phys->eang, 3);
		break;
	case TYCRC_MOVE_TOSS:
		VecND_Copy(ent->phys->org, ent->phys->smove, 3);
		VecND_AddScale(ent->phys->org, ent->phys->vel, dt,
			ent->phys->emove, 3);
		if(!ent->phys->flags&LBXGL_PHYS_FLONGROUND)
			VecND_Add(ent->phys->vel, lbxgl_physics_gravity,
				ent->phys->vel, 3);

		VecND_Copy(ent->phys->ang, ent->phys->sang, 3);
		VecND_AddScale(ent->phys->ang, ent->phys->avel, dt,
			ent->phys->eang, 3);
		break;
	case TYCRC_MOVE_BOUNCE:
		VecND_Copy(ent->phys->org, ent->phys->smove, 3);
		VecND_AddScale(ent->phys->org, ent->phys->vel, dt,
			ent->phys->emove, 3);
		if(!ent->phys->flags&LBXGL_PHYS_FLONGROUND)
			VecND_Add(ent->phys->vel, lbxgl_physics_gravity,
				ent->phys->vel, 3);

		VecND_Copy(ent->phys->ang, ent->phys->sang, 3);
		VecND_AddScale(ent->phys->ang, ent->phys->avel, dt,
			ent->phys->eang, 3);
		break;
	case TYCRC_MOVE_FLY:
		VecND_Copy(ent->phys->org, ent->phys->smove, 3);
		VecND_AddScale(ent->phys->org, ent->phys->vel, dt,
			ent->phys->emove, 3);

		VecND_Copy(ent->phys->ang, ent->phys->sang, 3);
		VecND_AddScale(ent->phys->ang, ent->phys->avel, dt,
			ent->phys->eang, 3);
		break;
	case TYCRC_MOVE_SLIDE:
	case TYCRC_MOVE_SLIDERESIST:
		VecND_Copy(ent->phys->org, ent->phys->smove, 3);

		if(!(ent->phys->flags&LBXGL_PHYS_FLONGROUND))
			VecND_Add(ent->phys->vel, lbxgl_physics_gravity,
				ent->phys->vel, 3);

		if(ent->phys->flags&LBXGL_PHYS_FLONGROUND)
		{
			VecND_Normalize(lbxgl_physics_gravity, gdir, 3);

			y=VecND_Normalize(ent->phys->vel, vel, 3);

//			x=VecND_DotProduct(ent->phys->vel, gdir, 3);
			x=VecND_DotProduct(vel, gdir, 3);
//			if(x>0)
			VecND_AddScale(vel, gdir, -x, vel, 3);
			if(x<0)ent->phys->flags&=~LBXGL_PHYS_FLONGROUND;

			VecND_Normalize(vel, vel, 3);
			VecND_Scale(vel, y, ent->phys->vel, 3);

			x=1-(4*dt);
			if(x<0)x=0;
			VecND_ScaleAddScale(ent->phys->vel, x,
				ent->phys->groundent->phys->vel, 1-x,
				ent->phys->vel, 3);
		}

		VecND_AddScale(ent->phys->org, ent->phys->vel, dt,
			ent->phys->emove, 3);

		VecND_Copy(ent->phys->ang, ent->phys->sang, 3);
		VecND_AddScale(ent->phys->ang, ent->phys->avel, dt,
			ent->phys->eang, 3);
		break;

	case TYCRC_MOVE_PUSH:
		VecND_Copy(ent->phys->org, ent->phys->smove, 3);
		VecND_AddScale(ent->phys->org, ent->phys->vel, dt,
			ent->phys->emove, 3);

		VecND_Copy(ent->phys->ang, ent->phys->sang, 3);
		VecND_AddScale(ent->phys->ang, ent->phys->avel, dt,
			ent->phys->eang, 3);
		break;

	case TYCRC_MOVE_CHARACTER:
		VecND_Copy(ent->phys->org, ent->phys->smove, 3);
		VecND_Copy(ent->phys->org, ent->phys->emove, 3);

		VecND_Copy(ent->phys->ang, ent->phys->sang, 3);
		VecND_AddScale(ent->phys->ang, ent->phys->avel, dt,
			ent->phys->eang, 3);
		break;

	default:
		kprint("unknown movetype %s\n", ent->phys->movestr);
	}
	LBXGL_Physics2_BoxMove(
		ent->phys->mins, ent->phys->maxs,
		ent->phys->smove, ent->phys->emove,
		ent->phys->mmins, ent->phys->mmaxs);
}

int LBXGL_Physics2_HandleImpact(LBXGL_Entity *ent1, LBXGL_Entity *ent2)
{
	double dir[3], gdir[3], vel[3];
	double x, y;

	VecND_Normalize(lbxgl_physics_gravity, gdir, 3);
	switch(ent1->phys->movetype)
	{
	case TYCRC_MOVE_NONE:
		break;
	case TYCRC_MOVE_TOSS:
		//hit something and stop
		VecND_Copy(ent1->phys->smove, ent1->phys->emove, 3);
		VecND_Zero(ent1->phys->vel, 3);
		break;

	case TYCRC_MOVE_BOUNCE:
		//hit something and bounce
		VecND_Copy(ent1->phys->smove, ent1->phys->emove, 3);
		if(VecND_DotProduct(ent2->phys->inorm, ent2->phys->inorm, 3)!=0)
		{
			x=VecND_Normalize(ent1->phys->vel, ent1->phys->vel, 3);
			VecND_Add(ent1->phys->vel, ent2->phys->inorm, dir, 3);
			VecND_Scale(dir, x*0.8, ent1->phys->vel, 3);
		}
		break;
	case TYCRC_MOVE_FLY:
		//hit something and slide
		if(VecND_DotProduct(ent2->phys->inorm, ent2->phys->inorm, 3)!=0)
		{
			x=VecND_DotProduct(ent1->phys->vel, ent2->phys->inorm, 3);
			VecND_AddScale(ent1->phys->vel, ent2->phys->inorm, -x,
				dir, 3);

			VecND_AddScale(ent1->phys->org, dir, ent1->phys->dt,
				ent1->phys->emove, 3);
		}else
		{
			//or failing that: stop
			VecND_Copy(ent1->phys->smove, ent1->phys->emove, 3);
		}
		break;
	case TYCRC_MOVE_SLIDE:
		//hit something and slide
		if(VecND_DotProduct(ent2->phys->inorm, ent2->phys->inorm, 3)!=0)
//		if(0)
		{
//			kprint("collision normal: (%lf %lf %lf %lf)\n",
//				ent2->phys->inorm[0], ent2->phys->inorm[1],
//				ent2->phys->inorm[2], ent2->phys->inorm[3]);

			VecND_Scale(ent2->phys->inorm, -1, dir, 3);

			VecND_Normalize(lbxgl_physics_gravity, gdir, 3);
			x=VecND_DotProduct(gdir, dir, 3);
/*
//			if(x>0.67)
			if(0)
			{
				ent1->phys->flags|=LBXGL_PHYS_FLONGROUND;
				VecND_Copy(ent1->phys->vel, dir, 3);
			}else
*/


			y=VecND_Normalize(ent1->phys->vel, vel, 3);

			if(!(ent1->phys->flags&LBXGL_PHYS_FLONGROUND))
			{
				x=VecND_DotProduct(vel, dir, 3);
				if(x>0)VecND_AddScale(vel, dir, -x,
					vel, 3);
			}

			if(ent1->phys->flags&LBXGL_PHYS_FLONGROUND)
			{
				x=VecND_DotProduct(vel, gdir, 3);
//				if(x>0)
				VecND_AddScale(vel, gdir, -x,
					vel, 3);
//				if(x<0)ent1->phys->flags&=
//					~LBXGL_PHYS_FLONGROUND;
			}

			VecND_Normalize(vel, vel, 3);
			VecND_Scale(vel, y, vel, 3);

			x=1-(5*ent1->phys->dt);
			if(x<0)x=0;
			VecND_Scale(vel, x, ent1->phys->vel, 3);

			VecND_AddScale(ent1->phys->org, vel, ent1->phys->dt,
				ent1->phys->emove, 3);
		}else
		{
			//or failing that: stop
			VecND_Copy(ent1->phys->smove, ent1->phys->emove, 3);
			VecND_Zero(ent1->phys->vel, 3);
		}
		break;
	case TYCRC_MOVE_SLIDERESIST:
		//hit something and slide with friction
//		if(VecND_DotProduct(ent2->phys->inorm, ent2->phys->inorm, 3)!=0)
		if(0)
		{
			x=VecND_DotProduct(ent1->phys->vel, ent2->phys->inorm, 3);
			VecND_AddScale(ent1->phys->vel, ent2->phys->inorm, -x,
				dir, 3);

			VecND_Scale(dir, 1-(0.9*ent1->phys->dt),
				ent1->phys->vel, 3);

			VecND_AddScale(ent1->phys->org, dir, ent1->phys->dt,
				ent1->phys->emove, 3);
		}else
		{
			//or failing that: stop
			VecND_Copy(ent1->phys->smove, ent1->phys->emove, 3);
		}
		break;
	case TYCRC_MOVE_PUSH:
//		kprint("push\n");

//		VecND_AddScale(ent2->phys->smove, ent1->phys->vel,
//			ent1->phys->dt*1.1, ent2->phys->smove, 3);
//		VecND_AddScale(ent2->phys->emove, ent1->phys->vel,
//			ent1->phys->dt*1.1, ent2->phys->emove, 3);

//		VecND_Add(ent2->phys->vel, ent1->phys->vel,
//			ent2->phys->vel, 3);

		VecND_Scale(ent1->phys->inorm, 1, dir, 3);
		VecND_AddScale(ent2->phys->smove, dir,
			ent1->phys->dt*1.1, ent2->phys->smove, 3);
		VecND_AddScale(ent2->phys->emove, dir,
			ent1->phys->dt*1.1, ent2->phys->emove, 3);

/*
		x=VecND_DotProduct(ent2->phys->vel, dir, 3);
		if(x<0)VecND_AddScale(ent2->phys->vel, dir, -x,
			ent2->phys->vel, 3);

		VecND_AddScale(ent2->phys->smove, ent2->phys->vel,
			ent2->phys->dt, ent2->phys->emove, 3);
*/

		//hit something and stop
//		VecND_Copy(ent1->phys->smove, ent1->phys->emove, 3);
//		VecND_Zero(ent1->phys->vel, 3);
		break;

	case TYCRC_MOVE_CHARACTER:
		//hit something and stop
		VecND_Copy(ent1->phys->smove, ent1->phys->emove, 3);
		VecND_Zero(ent1->phys->vel, 3);
		break;
	}
	LBXGL_Physics2_BoxMove(
		ent1->phys->mins, ent1->phys->maxs,
		ent1->phys->smove, ent1->phys->emove,
		ent1->phys->mmins, ent1->phys->mmaxs);

	LBXGL_Physics2_BoxMove(
		ent2->phys->mins, ent2->phys->maxs,
		ent2->phys->smove, ent2->phys->emove,
		ent2->phys->mmins, ent2->phys->mmaxs);

	if(ent1->phys->solidtype==TYCRC_SOLID_SKEL)
		LBXGL_PhysSkel_HandleImpact(ent1, ent2);

	return(0);
}

int LBXGL_Physics2_HandlePlaneImpact(LBXGL_Entity *ent1, double *gnorm)
{
	double dir[3];
	double x;

	switch(ent1->phys->movetype)
	{
	case TYCRC_MOVE_NONE:
		break;
	case TYCRC_MOVE_TOSS:
		//hit something and stop
		VecND_Copy(ent1->phys->smove, ent1->phys->emove, 3);
		VecND_Zero(ent1->phys->vel, 3);
		break;

	case TYCRC_MOVE_BOUNCE:
		//hit something and bounce
		VecND_Copy(ent1->phys->smove, ent1->phys->emove, 3);

		x=VecND_Normalize(ent1->phys->vel, ent1->phys->vel, 3);
		VecND_Copy(ent1->phys->vel, dir, 3);
		dir[3]=1;
		VecND_Normalize(dir, dir, 3);

		VecND_Scale(dir, x*0.8, ent1->phys->vel, 3);
		break;
	case TYCRC_MOVE_FLY:
		//hit something and slide
		x=VecND_DotProduct(ent1->phys->vel, gnorm, 3);
		VecND_AddScale(ent1->phys->vel, gnorm, -x,
			dir, 3);

		VecND_AddScale(ent1->phys->org, dir, ent1->phys->dt,
			ent1->phys->emove, 3);
		break;
	case TYCRC_MOVE_SLIDE:
		//hit something and slide
		VecND_Scale(gnorm, -1, dir, 3);

		x=VecND_DotProduct(ent1->phys->vel, dir, 3);
		VecND_AddScale(ent1->phys->vel, dir, -x,
			dir, 3);
		VecND_Copy(dir, ent1->phys->vel, 3);

/*
		x=1-(5*ent1->phys->dt);
		if(x<0)x=0;
		VecND_Scale(dir, x, ent1->phys->vel, 3);
*/

		VecND_AddScale(ent1->phys->org, dir, ent1->phys->dt,
			ent1->phys->emove, 3);
		break;
	case TYCRC_MOVE_SLIDERESIST:
		//hit something and slide with friction
		x=VecND_DotProduct(ent1->phys->vel, gnorm, 3);
		VecND_AddScale(ent1->phys->vel, gnorm, -x,
			dir, 3);

		VecND_Scale(dir, 1-(0.9*ent1->phys->dt),
			ent1->phys->vel, 3);
		VecND_AddScale(ent1->phys->org, dir, ent1->phys->dt,
			ent1->phys->emove, 3);

		break;
	case TYCRC_MOVE_CHARACTER:
		//hit something and stop
		VecND_Copy(ent1->phys->smove, ent1->phys->emove, 3);
		VecND_Zero(ent1->phys->vel, 3);
		break;
	}

	LBXGL_Physics2_BoxMove(
		ent1->phys->mins, ent1->phys->maxs,
		ent1->phys->smove, ent1->phys->emove,
		ent1->phys->mmins, ent1->phys->mmaxs);

	if(ent1->phys->solidtype==TYCRC_SOLID_SKEL)
		LBXGL_PhysSkel_HandlePlaneImpact(ent1, gnorm);

	return(0);
}

int LBXGL_Physics2_BlockImpact(LBXGL_Entity *ent)
{
	VecND_Copy(ent->phys->smove, ent->phys->emove, 3);
	VecND_Zero(ent->phys->vel, 3);

	LBXGL_Physics2_BoxMove(
		ent->phys->mins, ent->phys->maxs,
		ent->phys->smove, ent->phys->emove,
		ent->phys->mmins, ent->phys->mmaxs);

	return(0);
}

int LBXGL_Physics2_CheckOnGround(LBXGL_Entity *ent,
	LBXGL_Entity *ents, float dt)
{
	LBXGL_Entity *cur, *tmp, *nxt;
	LBXGL_Entity *fst, *lst;
	double osmove[3], oemove[3], f;
	double gnorm[4];
	int i, j, k;

	VecND_Copy(ent->phys->org, osmove, 3);
	VecND_Copy(ent->phys->org, oemove, 3);
	oemove[2]-=0.1;
	LBXGL_Physics2_BoxMove(
		ent->phys->mins, ent->phys->maxs,
		osmove, oemove,
		ent->phys->mmins, ent->phys->mmaxs);

	ent->phys->flags&=~LBXGL_PHYS_FLONGROUND;

	tmp=LBXGL_Physics2_QueryImpacts(ent, ents);
	cur=tmp;
	while(cur)
	{
		i=LBXGL_Physics2_CheckCollide(ent, cur);
		nxt=cur->phys->chain;

		if(i)
		{
			ent->phys->flags|=LBXGL_PHYS_FLONGROUND;
			ent->phys->groundent=cur;
			break;
		}

		cur=cur->phys->chain;
	}

//	ent->phys->flags|=LBXGL_PHYS_FLONGROUND;

	return(0);
}

LBXGL_Entity *LBXGL_Physics2_MoveTimeExtents(LBXGL_Entity *ent,
	LBXGL_Entity *ents, float dt)
{
	LBXGL_Entity *cur, *tmp, *nxt;
	LBXGL_Entity *fst, *lst;
	double osmove[3], oemove[3], f;
	double gnorm[4];
	int i, j, k;

/*
	if(ent->phys->solidtype==TYCRC_SOLID_SKEL)
	{
		tmp=LBXGL_PhysSkel_MoveTimeExtents(ent, ents, dt);
		return(tmp);
	}
*/

	if((ent->phys->movetype==TYCRC_MOVE_NONE) &&
		(ent->phys->solidtype!=TYCRC_SOLID_TRIGGER))
		return(NULL);

//	if(!(rand()%4))ent->phys->flags&=~LBXGL_PHYS_FLONGROUND;

	//kludge to detect onground status

	VecND_Zero(gnorm, 4);
	gnorm[2]=1;
	gnorm[3]=lbxgl_physics_ground;

	LBXGL_Physics2_BoxMove(
		ent->phys->mins, ent->phys->maxs,
		ent->phys->smove, ent->phys->emove,
		ent->phys->mmins, ent->phys->mmaxs);

	tmp=LBXGL_Physics2_QueryImpacts(ent, ents);
/*
	if(!tmp)
	{
		kprint("phys2: %s no block\n",
			LBXGL_Entity_GetPropertyString(ent, "classname"));

		VecND_Copy(ent->phys->emove, ent->phys->org, 3);
		LBXGL_Physics2_FinishMove(ent);
		return(NULL);
	}
*/

	fst=NULL;
	lst=NULL;

	cur=tmp;
	while(cur)
	{
		i=LBXGL_Physics2_CheckCollide(ent, cur);
		nxt=cur->phys->chain;

//		if(i)kprint("collide %s %s\n",
//			LBXGL_Entity_GetPropertyString(ent, "classname"),
//			LBXGL_Entity_GetPropertyString(cur, "classname"));

		if(i && (ent->phys->movetype!=TYCRC_MOVE_PUSH))
		{
			j=16;
			while(i && (j>0))
			{
				//stairs hack
				f=cur->phys->inorm[2];
				if(i && VecND_DotProduct(cur->phys->inorm, cur->phys->inorm, 3)
					&& (f>-0.7) && (f <0.7))
				{
					VecND_Copy(ent->phys->smove, osmove, 3);
					VecND_Copy(ent->phys->emove, oemove, 3);

					for(k=1; k<10; k++)
					{
						VecND_Copy(osmove, ent->phys->smove, 3);
						VecND_Copy(oemove, ent->phys->emove, 3);

						ent->phys->smove[2]+=k*0.10;
						ent->phys->emove[2]+=k*0.10;

						LBXGL_Physics2_BoxMove(
							ent->phys->mins, ent->phys->maxs,
							ent->phys->smove, ent->phys->emove,
							ent->phys->mmins, ent->phys->mmaxs);

						i=LBXGL_Physics2_CheckCollide(ent, cur);
						if(!i)break;
					}
					if(k==10)
					{
						VecND_Copy(osmove, ent->phys->smove, 3);
						VecND_Copy(oemove, ent->phys->emove, 3);

						LBXGL_Physics2_BoxMove(
							ent->phys->mins, ent->phys->maxs,
							ent->phys->smove, ent->phys->emove,
							ent->phys->mmins, ent->phys->mmaxs);
						i=LBXGL_Physics2_CheckCollide(ent, cur);
					}
					if(!i)continue;
				}
				//end stairs hack

				LBXGL_Physics2_HandleImpact(ent, cur);
				i=LBXGL_Physics2_CheckCollide(ent, cur);
				j--;
			}
//			if(!j)kprint("phys2: %s stuck\n",
//				LBXGL_Entity_GetPropertyString(ent, "classname"));

			i=1;
		}else if(i && (ent->phys->movetype==TYCRC_MOVE_PUSH))
		{
			j=16;
			while(i && (j>0))
			{
				LBXGL_Physics2_HandleImpact(ent, cur);
				i=LBXGL_Physics2_CheckCollide(ent, cur);
				j--;
			}
			i=1;
		}

		if(i)
		{
			if(lst)
			{
				cur->phys->chain=NULL;
				lst->phys->chain=cur;
				lst=cur;
			}else
			{
				cur->phys->chain=NULL;
				fst=cur;
				lst=cur;
			}
		}

//		cur=cur->phys->chain;
		cur=nxt;
	}
	i=LBXGL_Physics2_CheckPlaneBox(gnorm, ent->phys->mmins, ent->phys->mmaxs);
	if(i&2)LBXGL_Physics2_HandlePlaneImpact(ent, gnorm);

#if 0
	//if didn't do good enough trying to deal with collisions,
	//just block movement
	if(ent->phys->movetype!=TYCRC_MOVE_PUSH)
	{
		tmp=LBXGL_Physics2_QueryImpacts(ent, ents);
		cur=tmp;
		while(cur)
		{
			i=LBXGL_Physics2_CheckCollide(ent, cur);
			nxt=cur->phys->chain;

			if(i)LBXGL_Physics2_BlockImpact(ent);

//			cur=cur->phys->chain;
			cur=nxt;
		}

		i=LBXGL_Physics2_CheckPlaneBox(gnorm,
			ent->phys->mmins, ent->phys->mmaxs);
		if(i&2)LBXGL_Physics2_BlockImpact(ent);
	}
#endif

	return(fst);
}

LBXGL_Entity *LBXGL_Physics2_BlockMoveExtents(LBXGL_Entity *ent,
	LBXGL_Entity *ents, float dt)
{
	LBXGL_Entity *cur, *tmp, *nxt;
	LBXGL_Entity *fst, *lst;
	double osmove[3], oemove[3], f;
	double gnorm[4];
	int i, j, k;

	if((ent->phys->movetype==TYCRC_MOVE_NONE) &&
		(ent->phys->solidtype!=TYCRC_SOLID_TRIGGER))
		return(NULL);

	if(!(rand()%4))ent->phys->flags&=~LBXGL_PHYS_FLONGROUND;

	VecND_Zero(gnorm, 4);
	gnorm[2]=1;
	gnorm[3]=lbxgl_physics_ground;

	//if didn't do good enough trying to deal with collisions,
	//just block movement
	if(ent->phys->movetype!=TYCRC_MOVE_PUSH)
	{
		tmp=LBXGL_Physics2_QueryImpacts(ent, ents);
		cur=tmp;
		while(cur)
		{
			i=LBXGL_Physics2_CheckCollide(ent, cur);
			nxt=cur->phys->chain;

			if(i)LBXGL_Physics2_BlockImpact(ent);

//			cur=cur->phys->chain;
			cur=nxt;
		}

		i=LBXGL_Physics2_CheckPlaneBox(gnorm,
			ent->phys->mmins, ent->phys->mmaxs);
		if(i&2)LBXGL_Physics2_BlockImpact(ent);
	}

	return(fst);
}

int LBXGL_Physics2_Step(LBXGL_Entity *ents, float dt)
{
	LBXGL_Entity *cur, *cur2, *tmp;
	double f;
	void *a[4];
	void *p;
	void *dtp;
	int i;

	dtp=NetVal_WrapFloat(dt);

	lbxgl_physics_sunum++;

//	kprint("phys frame %f\n", dt);

	cur=ents;
	while(cur)
	{
		LBXGL_Physics2_SetupEntity(cur, dt);
		cur=cur->next;
	}

	cur=ents;
	while(cur)
	{
		a[0]=dtp;
		a[1]=NULL;
		LBXGL_Entity_SendMessage(cur, "premove", a);

		f=LBXGL_Entity_GetPropertyFloat(cur, "thinktime");
		if(f>0)
		{
			f-=dt;
			LBXGL_Entity_SetPropertyFloat(cur, "thinktime", f);

			if(f<=0)
			{
				a[0]=NULL;
				LBXGL_Entity_SendMessage(cur, "think", a);
			}
		}

		LBXGL_Physics2_PredictMove(cur, ents, dt);

		cur=cur->next;
	}

#if 1
	cur=ents;
	while(cur)
	{
		tmp=LBXGL_Physics2_MoveTimeExtents(cur, ents, dt);

		cur2=tmp;
		while(cur2)
		{
			a[0]=cur2;
			a[1]=NULL;
			LBXGL_Entity_SendMessage(cur, "touch", a);

			a[0]=cur;
			a[1]=NULL;
			LBXGL_Entity_SendMessage(cur2, "touch", a);

			cur2=cur2->phys->chain;
		}

#if 0
		if(tmp)
		{
			i=LBXGL_Entity_GetPropertyBool(cur, "touching");
			if(!i)
			{
				a[0]=tmp;
				a[1]=NULL;
				LBXGL_Entity_SendMessage(cur, "touch", a);
				LBXGL_Entity_SetPropertyBool(cur, "touching", 1);
			}

			i=LBXGL_Entity_GetPropertyBool(tmp, "touching");
			if(!i)
			{
				a[0]=cur;
				a[1]=NULL;
				LBXGL_Entity_SendMessage(tmp, "touch", a);
				LBXGL_Entity_SetPropertyBool(tmp, "touching", 1);
			}
		}else
		{
			LBXGL_Entity_SetPropertyBool(cur, "touching", 0);
		}
#endif

		a[0]=dtp;
		a[1]=NULL;
		LBXGL_Entity_SendMessage(cur, "postmove", a);
		cur=cur->next;
	}
#endif

	cur=ents;
	while(cur)
	{
		LBXGL_Physics2_BlockMoveExtents(cur, ents, dt);
		cur=cur->next;
	}

	cur=ents;
	while(cur)
	{
		VecND_Copy(cur->phys->emove, cur->phys->org, 3);
		LBXGL_Physics2_FinishMove(cur);
		cur=cur->next;
	}

	return(0);
}

int LBXGL_Physics2_Init()
{
}
