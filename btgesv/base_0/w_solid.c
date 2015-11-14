#include <btgesv.h>

BTGE_BrushWorld *btge_world;
float btge_time;		//world time
float btge_steptime;	//world step time
float btge_framedt;		//frame delta time

// btEntity btge_tempSelf;	//hack...

/** Set the currently active world */
BTSV_API int btsvBindWorld(BTGE_BrushWorld *wrl)
{
	BTGE_BrushWorld_DrawBuildFast(wrl);
	btge_world=wrl;
	return(0);
}

BTSV_API BTGE_BrushWorld *btCurrentWorld(void)
{
	return(btge_world);
}

/** Return contents flags for a given brush list */
BTSV_API int btBrushListContents(BTGE_Brush *lst)
{
	BTGE_Brush *cur;
	int cont;
	
	cur=lst; cont=0;
	while(cur)
	{
		if(cur->flags&BTGE_BRFL_DEL)
		{
			cur=cur->chain;
			continue;
		}

		if(cur->flags&BTGE_BRFL_FLUID)
		{
			if((cur->flags&BTGE_TXFL_FLUID_MASK)==BTGE_TXFL_FLUID_WATER)
				{ cont|=BT_CONTENTS_WATER; }
			else if((cur->flags&BTGE_TXFL_FLUID_MASK)==BTGE_TXFL_FLUID_SLIME)
				{ cont|=BT_CONTENTS_SLIME; }
			else if((cur->flags&BTGE_TXFL_FLUID_MASK)==BTGE_TXFL_FLUID_LAVA)
				{ cont|=BT_CONTENTS_LAVA; }
			else if((cur->flags&BTGE_TXFL_FLUID_MASK)==BTGE_TXFL_FLUID_SEWER)
				{ cont|=BT_CONTENTS_WATER; }
			else { cont|=BT_CONTENTS_WATER; }	//generic fluid
			
			cur=cur->chain;
			continue;
		}

		if(cur->flags&BTGE_BRFL_EFFECT)
		{
			if((cur->flags&BTGE_TXFL_EFFECT_MASK)==BTGE_TXFL_EFFECT_FOG)
				{ cont|=BT_CONTENTS_MIST; }
			cur=cur->chain;
			continue;
		}

		if(cur->flags&BTGE_BRFL_NONSOLID)
		{
			cur=cur->chain;
			continue;
		}

		if((cur->flags&BTGE_TXFL_MATERIAL_MASK)==BTGE_TXFL_MATERIAL_SKY)
		{
			cont|=BT_CONTENTS_SKY;
			cur=cur->chain;
			continue;
		}

		if(cur->flags&BTGE_BRFL_ALPHA)
		{
//			if(cur->flags&BTGE_BRFL_NODRAW)
//			{
//				cont|=BT_CONTENTS_SKY;
//			}else
//				cont|=BT_CONTENTS_WINDOW;

			cont|=BT_CONTENTS_WINDOW;
			cur=cur->chain;
			continue;
		}

		if(cur->flags&BTGE_BRFL_NODRAW)
			cont|=BT_CONTENTS_WINDOW;

		//ugly, but probably true enough...
//		if(cur->flags&BTGE_BRFL_NODRAW)
//			cont|=BT_CONTENTS_SKY;

		cont|=BT_CONTENTS_SOLID;
		cur=cur->chain;
	}
	
	return(cont);
}

/** Return flags to indicate whatever may be in a given spot.
  * If present, forent indicates the entity doing the check,
  * which is then ignored when determining contents.
  */
BTSV_API int btPointContents(vec3 pt, btEntity forent)
{
	float io[4];
	BTGE_VoxelData *vox;
	BTGE_Brush *brush;
	btEntity ents;
	int cont;
	int i;
//	float *sv;

	BTGE_BrushWorld_DrawBuildFast(btge_world);
	
//	sv=pfvec3(pt);
	vfvec3(io, pt);
	brush=BTGE_BrushBSP_QueryTreeNode(btge_world->bsp, io, 0.1);
//	if(brush)return(BT_CONTENTS_SOLID);
	cont=btBrushListContents(brush);

	if(!(cont&BT_CONTENTS_SOLID))
	{
		vox=BTGE_Voxel_GetWorldVoxel(btge_world,
			io[0]/32, io[1]/32, io[2]/32);
//		if(vox && vox->type)
//			cont|=BT_CONTENTS_SOLID;

		if(vox && vox->type)
		{
			i=BTGE_Voxel_GetVoxelFlags(vox);
			cont|=btGetFlagContents(i);
		}
	}

	if(!(cont&BT_CONTENTS_SOLID))
	{
		ents=BT_CheckBoxEnts(pt, pt,
			BT_TRACE_BSPSOLID|BT_TRACE_MONSTERS, forent);
//		if(ents)
//			cont|=BT_CONTENTS_ORIGIN|BT_CONTENTS_SOLID;

		cont|=btEntityListBoxContents(ents, pt, pt);
	}

	return(cont);
}

BTSV_API int btGetFlagContents(int flags)
{
	int cont;
	
	cont=0;

	if(flags&BTGE_TXFL_FLUID)
	{
		if((flags&BTGE_TXFL_FLUID_MASK)==BTGE_TXFL_FLUID_WATER)
			{ cont|=BT_CONTENTS_WATER; }
		else if((flags&BTGE_TXFL_FLUID_MASK)==BTGE_TXFL_FLUID_SLIME)
			{ cont|=BT_CONTENTS_SLIME; }
		else if((flags&BTGE_TXFL_FLUID_MASK)==BTGE_TXFL_FLUID_LAVA)
			{ cont|=BT_CONTENTS_LAVA; }
		else if((flags&BTGE_TXFL_FLUID_MASK)==BTGE_TXFL_FLUID_SEWER)
			{ cont|=BT_CONTENTS_WATER; }
		else { cont|=BT_CONTENTS_WATER; }	//generic fluid

		return(cont);
	}

	if((flags&BTGE_BRFL_EFFECT) &&
		(flags&BTGE_TXFL_EFFECT_MASK)!=BTGE_TXFL_EFFECT_VOXEL)
	{
		if((flags&BTGE_TXFL_EFFECT_MASK)==BTGE_TXFL_EFFECT_FOG)
			{ cont|=BT_CONTENTS_MIST; }
		return(cont);
	}

	if(flags&BTGE_TXFL_NONSOLID)
		return(0);

	if((flags&BTGE_TXFL_MATERIAL_MASK)==BTGE_TXFL_MATERIAL_SKY)
	{
		cont|=BT_CONTENTS_SKY;
		return(cont);
	}

	if(flags&BTGE_TXFL_ALPHA)
	{
		cont|=BT_CONTENTS_WINDOW;
		return(cont);
	}

	if(flags&BTGE_BRFL_NODRAW)
	{
		cont|=BT_CONTENTS_WINDOW;
		return(cont);
	}

	cont|=BT_CONTENTS_SOLID;
	return(cont);
}

BTSV_API int btRoundDown(float val)
{
	return(floor(val));
}

BTSV_API int btBoxContents(vec3 amins, vec3 amaxs, btEntity forent)
{
	float am[4], an[4];
	int ami[4], ani[4];
	BTGE_VoxelData *vox;
	BTGE_Brush *lst;
	btEntity ents;
	int i, j, k, l, l1;
	int cont;

	BTGE_BrushWorld_DrawBuildFast(btge_world);

	vfvec3(am, amins);
	vfvec3(an, amaxs);	
	lst=BTGE_BrushBSP_BoxQueryTreeNode(btge_world->bsp, am, an);
//	if(lst)return(BT_CONTENTS_SOLID);
	cont=btBrushListContents(lst);

	if(!(cont&BT_CONTENTS_SOLID))
	{
		ami[0]=btRoundDown(am[0]/32);
		ami[1]=btRoundDown(am[1]/32);
		ami[2]=btRoundDown(am[2]/32);
		ani[0]=btRoundDown(an[0]/32);
		ani[1]=btRoundDown(an[1]/32);
		ani[2]=btRoundDown(an[2]/32);
		
		for(i=ami[0]; i<=ani[0]; i++)
			for(j=ami[1]; j<=ani[1]; j++)
				for(k=ami[2]; k<=ani[2]; k++)
		{
			vox=BTGE_Voxel_GetWorldVoxel(btge_world, i, j, k);
			if(vox && vox->type)
			{
				l=BTGE_Voxel_GetVoxelFlags(vox);
				l1=BTGE_Voxel_GetVoxelFlags(vox);
				cont|=btGetFlagContents(l);
//				printf("btBoxContents: %08X %08X\n", l, cont);
				if(cont&BT_CONTENTS_SOLID)
					break;
			}
		}
	}

	if(!(cont&BT_CONTENTS_SOLID))
	{
		ents=BT_CheckBoxEnts(amins, amaxs,
			BT_TRACE_BSPSOLID|BT_TRACE_MONSTERS, forent);
//		if(ents)
//			cont|=BT_CONTENTS_ORIGIN|BT_CONTENTS_SOLID;
		cont|=btEntityListBoxContents(ents, amins, amaxs);
	}

	return(cont);
}

BTSV_API int btPointBoxContents(vec3 org, vec3 mins, vec3 maxs, btEntity forent)
{
	vec3 am, an;

	am=v3add(org, mins);
	an=v3add(org, maxs);
	return(btBoxContents(am, an, forent));
}

BTSV_API vec3 btBoxStepMoveFall(
	vec3 org, vec3 mins, vec3 maxs, float dt, btEntity self)
{
	vec3 org1, org2, vel;
	float f;
	int i, j;

	f=256*dt;
	vel=vec3(0, 0, -f);

	if(self)
	{
//		self->flags&=~BT_FL_INWATER;
		self->flags&=~(BT_FL_ONGROUND|BT_FL_INWATER);

		//flying entities don't fall
		if(self->flags&BT_FL_FLY)
			return(org);
			
		vel=v3scale(self->velocity, dt);
	}

//	f=64*dt;
//	f=256*dt;
//	org1=v3sub(org, vec3(0, 0, f));
	org1=v3add(org, vel);
	i=btPointBoxContents(org1, mins, maxs, self);
	if(i&BT_CONTENTS_PLAYERSOLID)
	{
		if(self)
		{
			if(self->flags&BT_FL_CLIENT)
			{
//				printf("client impact\n");
//				BT_ClientImpact(self, self->velocity);
				BT_ClientImpact(self,
					v3add(self->velocity,
						self->impulseVelocity));
			}
			
			self->flags|=BT_FL_ONGROUND;
			self->velocity=vec3(0,0,0);

			j=btPointContents(org, self);
			if(j&BT_CONTENTS_FLUID)
			{
				self->flags|=BT_FL_INWATER;
			}

		}
		return(org);
	}

	if(self && (self->flags&BT_FL_CLIENT))
	{
		org2=v3sub(org, vec3(0, 0, 2));
		i=btPointBoxContents(org2, mins, maxs, self);
		if(i&BT_CONTENTS_PLAYERSOLID)
		{
			self->flags|=BT_FL_ONGROUND;
		}
	}

	j=btPointContents(org, self);
	if(j&BT_CONTENTS_FLUID)
	{
		if(self)
		{
			self->flags|=BT_FL_INWATER;
			
			//swim entities don't sink
			if(self->flags&BT_FL_SWIM)
				return(org);

			f=1.0-dt;
			if(f<0)f=0;
			self->velocity=v3scale(self->velocity, f);
//			self->velocity=vec3(0,0,0);
		}

		//reduced falling speed
//		f=16*dt;
//		org1=v3sub(org, vec3(0, 0, f));

		if(!self)
			org1=v3addscale(org, vel, 0.25);
	}
	
	return(org1);
}

BTSV_API vec3 btBoxStepMoveSlide(vec3 org,
	vec3 mins, vec3 maxs, vec3 ivel, float dt, vec3 org1,
	btEntity self)
{
	vec3 org2;
	int i;

	if(fabs(v3y(ivel))>fabs(v3x(ivel)))
	{
		org2=vec3(v3x(org), v3y(org1), v3z(org1));
		i=btPointBoxContents(org2, mins, maxs, self);
		if(!(i&BT_CONTENTS_PLAYERSOLID))
			return(org2);
//		if(!i)return(btBoxStepMoveFall(org2, mins, maxs, dt));

		org2=vec3(v3x(org1), v3y(org), v3z(org1));
		i=btPointBoxContents(org2, mins, maxs, self);
		if(!(i&BT_CONTENTS_PLAYERSOLID))
			return(org2);
//		if(!i)return(btBoxStepMoveFall(org2, mins, maxs, dt));
	}else
	{
		org2=vec3(v3x(org1), v3y(org), v3z(org1));
		i=btPointBoxContents(org2, mins, maxs, self);
		if(!(i&BT_CONTENTS_PLAYERSOLID))
			return(org2);
//		if(!i)return(btBoxStepMoveFall(org2, mins, maxs, dt));

		org2=vec3(v3x(org), v3y(org1), v3z(org1));
		i=btPointBoxContents(org2, mins, maxs, self);
		if(!(i&BT_CONTENTS_PLAYERSOLID))
			return(org2);
//		if(!i)return(btBoxStepMoveFall(org2, mins, maxs, dt));
	}
	
	return(org1);
}

BTSV_API vec3 btBoxStepMove(vec3 org, vec3 mins, vec3 maxs,
	vec3 ivel, float dt, btEntity self)
{
	vec3 org1;

#if 1
	if(self)
	{
		self->flags&=~(BT_FL_SIMPLEMOVE|BT_FL_OFFGROUND|BT_FL_JUMPMOVE);
		self->impulseVelocity=ivel;
		
		if(self->flags&BT_FL_CLIENT)
		{
//			self->flags&=~(BT_FL_ONGROUND);
		}
	}

	if(self && (self->flags&BT_FL_NOCLIP))
	{
		org1=v3add(org, v3scale(ivel, dt));
		return(org1);
	}
#endif

	org1=btBoxStepMoveI(org, mins, maxs, ivel, dt, self);
	return(org1);
}

vec3 btBoxStepMoveI(vec3 org, vec3 mins, vec3 maxs,
	vec3 ivel, float dt, btEntity self)
{
	vec3 norg, org1, org2, org3;
	float f;
	int i, j;

	if(dt>0.5)
	{
		return(org);
	}

	if(dt>(1.0/24))
	{
		f=dt*0.5;
		org1=btBoxStepMoveI(org, mins, maxs, ivel, f, self);

		if(self && (self->flags&BT_FL_JUMPMOVE))
			return(btBoxStepMoveFall(org1, mins, maxs, f, self));

		org2=btBoxStepMoveI(org1, mins, maxs, ivel, f, self);
		return(org2);
	}

#if 0
	if(self)
	{
		self->flags&=~(BT_FL_SIMPLEMOVE|BT_FL_OFFGROUND|BT_FL_JUMPMOVE);
		self->impulseVelocity=ivel;
		
		if(self->flags&BT_FL_CLIENT)
		{
//			self->flags&=~(BT_FL_ONGROUND);
		}
	}

	if(self && (self->flags&BT_FL_NOCLIP))
	{
		org1=v3add(org, v3scale(ivel, dt));
		return(org1);
	}
#endif

	//can't move if dead
	if(self && (self->health<0))
	{
		org1=btBoxStepMoveFall(org, mins, maxs, dt, self);
		return(org1);
	}

	org1=v3add(org, v3scale(ivel, dt));

//	f=64*dt;
//	org2=v3sub(org1, vec3(0, 0, f));

	i=btPointBoxContents(org1, mins, maxs, self);
//	j=btPointBoxContents(org2, mins, maxs);
//	if(!i)
	if(!(i&BT_CONTENTS_PLAYERSOLID))
	{
		if(self)self->flags|=BT_FL_SIMPLEMOVE;
		org2=btBoxStepMoveFall(org1, mins, maxs, dt, self);

#if 0
		if(self && (self->flags&BT_FL_MONSTER) &&
			!(self->flags&(BT_FL_ONGROUND|BT_FL_FLY|BT_FL_SWIM)))
		{
			//ok, not onground, assume walked off a ledge
			self->flags&=~BT_FL_SIMPLEMOVE;
			org2=btBoxStepMoveFall(org, mins, maxs, dt, self);
		}
#endif

#if 1
		if(self && (self->flags&BT_FL_MONSTER) &&
			!(self->flags&(BT_FL_FLY|BT_FL_SWIM)))
		{
//			j=btCheckGroundDist(org1, mins, maxs, 16, self);
//			j=btCheckGroundDist(org1, mins, maxs, 24, self);
//			j=btCheckGroundDistPts(org1, mins, maxs, 24, self);
			j=btCheckGroundDistPts(org1, mins, maxs, 36, self);
			if(!j)
			{
				printf("btBoxStepMove: %d Ledge Detect Handle\n",
					self->entnum);
				self->flags&=~BT_FL_SIMPLEMOVE;
				org2=btBoxStepMoveFall(org, mins, maxs, dt, self);
			}
		}
#endif

		return(org2);
	}

	if((fabs(v3x(ivel))>fabs(v3z(ivel))) &&
		(fabs(v3y(ivel))>fabs(v3z(ivel))))
	{
		//assume sliding along ground
		org1=vec3(v3x(org1), v3y(org1), v3z(org));
		i=btPointBoxContents(org1, mins, maxs, self);
//		if(!i)return(org1);
		if(!(i&BT_CONTENTS_PLAYERSOLID))
		{
			if(self)self->flags|=BT_FL_SIMPLEMOVE;
			return(btBoxStepMoveFall(org1, mins, maxs, dt, self));
//			return(org1);
		}
	}

	for(j=0; j<4; j++)
	{
		org2=vec3(v3x(org1), v3y(org1), v3z(org1)+(j+1)*4);
		i=btPointBoxContents(org2, mins, maxs, self);
//		if(!i)return(org2);
		if(!(i&BT_CONTENTS_PLAYERSOLID))
		{
			if(self)self->flags|=BT_FL_SIMPLEMOVE;
			return(btBoxStepMoveFall(org2, mins, maxs, dt, self));
//			return(org2);
		}
	}

	if(self && (self->flags&BT_FL_MONSTER))
	{
		org2=vec3(v3x(org1), v3y(org1), v3z(org1)+34);
		i=btPointBoxContents(org2, mins, maxs, self);
//		if(!i)return(org2);
		if(!(i&BT_CONTENTS_PLAYERSOLID))
		{
			printf("btBoxStepMove: %d Jump Move, flags=%08X\n",
				self->entnum, self->flags);
			self->flags|=BT_FL_JUMPMOVE;
			return(btBoxStepMoveFall(org, mins, maxs, dt, self));
		}
	}

#if 0
	if(self && (self->flags&BT_FL_MONSTER) &&
		!(self->flags&(BT_FL_FLY|BT_FL_SWIM)))
	{
//		printf("btBoxStepMove: %d Compound Move\n",
//			self->entnum);
		return(btBoxStepMoveFall(org, mins, maxs, dt, self));
	}
#endif

	org2=btBoxStepMoveSlide(org, mins, maxs, ivel, dt, org1, self);
	i=btPointBoxContents(org2, mins, maxs, self);
//	if(!i)return(org2);
	if(!(i&BT_CONTENTS_PLAYERSOLID))
	{
		if(self)self->flags|=BT_FL_SIMPLEMOVE;
		return(btBoxStepMoveFall(org2, mins, maxs, dt, self));
	}

	for(j=0; j<4; j++)
	{
		org2=vec3(v3x(org1), v3y(org1), v3z(org1)+(j+1)*4);
//		i=btPointBoxContents(org2, mins, maxs);
//		if(!i)return(org2);
		org2=btBoxStepMoveSlide(org, mins, maxs, ivel, dt, org2, self);
		i=btPointBoxContents(org2, mins, maxs, self);
		if(!(i&BT_CONTENTS_PLAYERSOLID))
		{
			if(self)self->flags|=BT_FL_SIMPLEMOVE;
			return(btBoxStepMoveFall(org2, mins, maxs, dt, self));
		}
	}
	
	return(btBoxStepMoveFall(org, mins, maxs, dt, self));
//	return(org);
}

BTSV_API btEntity BT_TracelineEnts(BTGE_Trace *trace,
	vec3 spos, vec3 epos, int flags, btEntity forent)
{
	btEntity cur, lst;
	int i, j;
	
	lst=NULL;
	for(i=1; i<btge_world->n_ents; i++)
	{
		cur=btge_world->ents[i];
		if(!cur)continue;
		if(forent)
		{
			if(cur==forent)continue;
			if(cur->owner==forent)continue;
			if(cur==forent->owner)continue;
			if(cur->owner && cur->owner==forent->owner)
				continue;
		}

		if(cur->solidtype==BT_SOLID_NOT)
		{
			//check ignore non-solid
			if(!(flags&BT_TRACE_NONSOLID))
				continue;
		}
		
		if(cur->solidtype==BT_SOLID_TRIGGER)
		{
			//check ignore triggers
			if(!(flags&BT_TRACE_TRIGGER))
				continue;
		}

		if((cur->solidtype==BT_SOLID_BBOX) ||
			(cur->solidtype==BT_SOLID_SLIDEBOX))
		{
			//check ignore monsters
			if(!(flags&BT_TRACE_MONSTERS))
				continue;
		}

		if(cur->solidtype==BT_SOLID_BSP)
		{
			//check ignore triggers
			if(!(flags&BT_TRACE_BSPSOLID))
				continue;
		}

		j=btEntityLineContents(cur, spos, epos);

//		j=btEntityCheckLine(cur, spos, epos);
//		if(j)
		if(j&BT_CONTENTS_CHECKSOLID)
		{
			cur->chain=lst;
			lst=cur;
		}
	}
	return(lst);
}

BTSV_API btEntity BT_CheckBoxEnts(
	vec3 mins, vec3 maxs, int flags, btEntity forent)
{
	btEntity cur, lst;
	int i, j;

	lst=BTGE_EntBSP_CheckBoxEntsR(mins, maxs, flags, forent,
		NULL, btge_world->entsNodeRoot);
	return(lst);

#if 0
//	printf("BT_CheckBoxEnts\n");
	
	lst=NULL;
	for(i=1; i<btge_world->n_ents; i++)
	{
		cur=btge_world->ents[i];
		if(!cur)continue;
		if(forent)
		{
			if(cur==forent)continue;
			if(cur->owner==forent)continue;
			if(cur==forent->owner)continue;
		}

		if(cur->solidtype==BT_SOLID_NOT)
		{
			//check ignore non-solid
			if(!(flags&BT_TRACE_NONSOLID))
				continue;
		}
		
		if(cur->solidtype==BT_SOLID_TRIGGER)
		{
			//check ignore triggers
			if(!(flags&BT_TRACE_TRIGGER))
				continue;
		}

		if((cur->solidtype==BT_SOLID_BBOX) ||
			(cur->solidtype==BT_SOLID_SLIDEBOX))
		{
			//check ignore monsters
			if(!(flags&BT_TRACE_MONSTERS))
				continue;
		}

		if(cur->solidtype==BT_SOLID_BSP)
		{
			//check ignore triggers
			if(!(flags&BT_TRACE_BSPSOLID))
				continue;
		}

//		printf("BT_CheckBoxEnts: %p\n", cur);

//		j=btEntityCheckLine(cur, spos, epos);
		j=btEntityCheckBox(cur, mins, maxs);
		if(j)
		{
			cur->chain=lst;
			lst=cur;
		}
	}
//	printf("BT_CheckBoxEnts: OK %p\n", lst);
	return(lst);
#endif
}

BTSV_API void BT_CheckFireBBoxTrigger(btEntity self,
	vec3 mins, vec3 maxs)
{
	btEntity tmpArr[256];
	btEntity lst, cur, nxt;
	vec3 amins, amaxs;
	int i, n;
	
	amins=v3add(self->origin, mins);
	amaxs=v3add(self->origin, maxs);
	lst=BT_CheckBoxEnts(amins, amaxs, BT_TRACE_TRIGGER, self);
	cur=lst; n=0;
	
	n=0;
	while(cur)
	{
		tmpArr[n++]=cur;
		cur=cur->chain;
	}
		
	for(i=0; i<n; i++)
	{
		if(btEntityP(tmpArr[i]))
			btEntTouch(tmpArr[i], self);
		if(btEntityP(self))
			btEntTouch(self, tmpArr[i]);
	}
}

BTSV_API void BT_CheckFireBoxTrigger(btEntity self)
{
	BT_CheckFireBBoxTrigger(self, self->mins, self->maxs);
}

BTSV_API void BT_CheckFirePadBoxTrigger(btEntity self)
{
	vec3 mins, maxs;
	mins=v3add(self->mins, vec3(-8,-8,-8));
	maxs=v3add(self->maxs, vec3( 8, 8, 8));
	BT_CheckFireBBoxTrigger(self, mins, maxs);
}

/**
 * Traceline from start to end position.
 * flags indicates to ignore monsters or other things.
 * forent is an entity which will be ignored in the trace.
 */
BTSV_API int btTraceline(BTGE_Trace *trace,
	vec3 spos, vec3 epos, int flags, btEntity forent)
{
	float tv0[3], tv1[3], tv2[3];
	BTGE_Brush *brush, *blst, *blste, *bcur, *bnxt;
	BTGE_VoxelData *vox;
	btEntity ents, cur, best;
	float *sv, *ev;
	float dist, f, g;
	int i;

	BTGE_BrushWorld_DrawBuildFast(btge_world);

	memset(trace, 0, sizeof(BTGE_Trace));

	vfvec3(tv0, spos);
	vfvec3(tv1, epos);
	dist=V3F_DIST(tv0, tv1);
	if(dist<=0.01)dist=0.01;

	trace->fraction=1;

	if(!(flags&BT_TRACE_CONT_NOSOLID))
	{
		i=btPointContents(spos, forent);
		if(i&BT_CONTENTS_SOLID)
		{
			trace->fraction=0;
			trace->flags|=BT_TRACE_STARTSOLID;
			return(TRUE);
		}
	
//		brush=BTGE_BrushBSP_TracelineFirst(btge_world->bsp, tv0, tv1);
		brush=BTGE_BrushBSP_Traceline(btge_world->bsp, NULL, tv0, tv1);
		
		bcur=brush; blst=NULL; blste=NULL;
		while(bcur)
		{
			bnxt=bcur->chain;
			if(bcur->flags&BTGE_BRFL_ALPHA)
			{
				if(flags&BT_TRACE_CONT_NOWINDOW)
					{ bcur=bnxt; continue; }
			}
			if(bcur->flags&BTGE_BRFL_FLUID)
			{
				if(flags&BT_TRACE_CONT_NOFLUID)
					{ bcur=bnxt; continue; }
			}

			bcur->chain=NULL;
			if(blste) { blste->chain=bcur; blste=bcur; }
			else { blst=bcur; blste=bcur; }

			bcur=bnxt;
		}
		
		trace->brush=blst;
		trace->contents=btBrushListContents(blst);
	}

	if(flags&(BT_TRACE_MONSTERS|BT_TRACE_CLIP|
		BT_TRACE_TRIGGER|BT_TRACE_NONSOLID|BT_TRACE_BSPSOLID))
	{
		if(trace->brush)
		{
			//if looking for monsters and hit geometry
			//clip line to the intersected brush
#if 0
			BTGE_Brush_CheckLineBrushClip(trace->brush,
				tv0, tv1, tv2, NULL);
			epos=vec3vf(tv2);
			trace->org=epos;
#endif
			
#if 1
			bcur=trace->brush;
			while(bcur)
			{
				BTGE_Brush_CheckLineBrushClip(trace->brush,
					tv0, tv1, tv2, NULL);
				V3F_COPY(tv2, tv1);
				bcur=bcur->chain;
			}
			epos=vec3vf(tv2);
			trace->org=epos;
#endif

			f=V3F_DIST(tv2, tv0);
			trace->fraction=f/dist;
		}
	}else
	{
		//no exact endpoint, but still need a fraction
		if(trace->brush)
			trace->fraction=0.99;
	}
	
	if(flags&(BT_TRACE_MONSTERS|
		BT_TRACE_TRIGGER|BT_TRACE_NONSOLID|BT_TRACE_BSPSOLID))
	{
		if(trace->brush)
		{
			//if looking for monsters and hit geometry
			//clip line to the intersected brush
			BTGE_Brush_CheckLineBrushClip(trace->brush,
				tv0, tv1, tv2, NULL);
			epos=vec3vf(tv2);
		}

		ents=BT_TracelineEnts(trace, spos, epos, flags, forent);
		trace->ent=ents;
	}

	vox=BTGE_Voxel_TraceLineVoxel(btge_world, spos, epos, 0, tv2);
	
	if(trace->brush || trace->ent || vox)
	{
		//hit something
		//but, fraction needs to be < 1 for contacts
		if(trace->fraction>=1.0)
			trace->fraction=0.99;
		return(TRUE);
	}
	
	
//	trace->fraction=1;
	return(FALSE);
}

/**
  * determine if origin and bbox represent a place a flying entity can be
  */
BTSV_API int btCheckFlySpot(vec3 org, vec3 mins, vec3 maxs, btEntity forent)
{
	static int nostand=
		BT_CONTENTS_WATER|BT_CONTENTS_SLIME|
		BT_CONTENTS_LAVA|BT_CONTENTS_SKY;
	vec3 org1;
	int i;
	
	//can't fly in solids or fluids
	i=btPointBoxContents(org, mins, maxs, forent);
	if(i&BT_CONTENTS_SOLID)	//in solid
		return(0);
	if(i&nostand)			//in a bad place
		return(0);

	return(1);
}

/**
  * determine if origin and bbox represent a place a flying entity can be
  */
BTSV_API int btCheckSwimSpot(vec3 org, vec3 mins, vec3 maxs, btEntity forent)
{
	static int noswim=
		BT_CONTENTS_SOLID|BT_CONTENTS_SLIME|
		BT_CONTENTS_LAVA|BT_CONTENTS_SKY;
	vec3 org1;
	int i;
	
	//can't fly in solids or fluids
	i=btPointBoxContents(org, mins, maxs, forent);
	if(!(i&BT_CONTENTS_WATER))	//need water to swim...
		return(0);
	if(i&noswim)return(0);		//can't swim in some things
	return(1);
}

/**
  * Determine if origin and bbox represent a place an entity can stand.
  * Dist determines how far to look downwards for ground.
  * This version will check points, and is only true if all of the points
  * happen to align with the ground.
  */
BTSV_API int btCheckGroundDistPts(vec3 org, vec3 mins, vec3 maxs,
	float dist, btEntity forent)
{
	BTGE_Trace tr;
	vec3 org1, org2;
	int i;

	if(!btCheckGroundDist(org, mins, maxs, dist, forent))
		return(0);

	org1=v3add(org, vec3(0, 0, v3z(mins)-dist));
//	i=btPointBoxContents(org1, mins, maxs, forent);
//	if(!i)

	i=btTraceline(&tr, org, org1,
		BT_TRACE_MONSTERS|BT_TRACE_BSPSOLID, forent);
	if(!i)return(0);
//	if(tr.fraction>=1.0)return(0);

#if 0
	org2=v3add(org1, vec3(v3x(mins), v3y(mins), 0));
	i=btTraceline(&tr, org, org2,
		BT_TRACE_MONSTERS|BT_TRACE_BSPSOLID, forent);
	if(!i)return(0);

	org2=v3add(org1, vec3(v3x(maxs), v3y(mins), 0));
	i=btTraceline(&tr, org, org2,
		BT_TRACE_MONSTERS|BT_TRACE_BSPSOLID, forent);
	if(!i)return(0);

	org2=v3add(org1, vec3(v3x(mins), v3y(maxs), 0));
	i=btTraceline(&tr, org, org2,
		BT_TRACE_MONSTERS|BT_TRACE_BSPSOLID, forent);
	if(!i)return(0);

	org2=v3add(org1, vec3(v3x(maxs), v3y(maxs), 0));
	i=btTraceline(&tr, org, org2,
		BT_TRACE_MONSTERS|BT_TRACE_BSPSOLID, forent);
	if(!i)return(0);
#endif

	return(1);
}

/**
  * determine if origin and bbox represent a place an entity can stand
  * dist determines how far to look downwards for ground
  */
BTSV_API int btCheckGroundDist(vec3 org, vec3 mins, vec3 maxs,
	float dist, btEntity forent)
{
	static int nostand=
		BT_CONTENTS_WATER|BT_CONTENTS_SLIME|
		BT_CONTENTS_LAVA|BT_CONTENTS_SKY;
//	static int nostand2=
//		BT_CONTENTS_SLIME|
//		BT_CONTENTS_LAVA|BT_CONTENTS_SKY;
	vec3 org1;
	int i;
	
	//can't stand in solids or fluids
	i=btPointBoxContents(org, mins, maxs, forent);
	if(i&BT_CONTENTS_SOLID)	//in solid
	{
		if(forent)printf("btCheckGroundDist: %d In Solid\n",
			forent->entnum);
		return(0);
	}
	if(i&nostand)			//in a bad place
	{
		if(forent)printf("btCheckGroundDist: %d Bad Spot\n",
			forent->entnum);
		return(0);
	}

	if((dist>4) && btCheckGroundDist2(org, mins, maxs, 4, forent));
		return(1);
	if((dist>8) && btCheckGroundDist2(org, mins, maxs, 8, forent));
		return(1);
	
	i=btCheckGroundDist2(org, mins, maxs, dist, forent);
	return(i);
}

BTSV_API int btCheckGroundDist2(vec3 org, vec3 mins, vec3 maxs,
	float dist, btEntity forent)
{
	static int nostand=
		BT_CONTENTS_WATER|BT_CONTENTS_SLIME|
		BT_CONTENTS_LAVA|BT_CONTENTS_SKY;
	vec3 org1;
	int i;

	org1=v3sub(org, vec3(0, 0, dist));
	i=btPointBoxContents(org1, mins, maxs, forent);
	if(!(i&BT_CONTENTS_SOLID))	//nothing underfoot
	{
		if(forent)printf("btCheckGroundDist: %d Empty Space (%s) (%s)\n",
			forent->entnum, btVec3ToStr(org), btVec3ToStr(org1));
		return(0);
	}
	if(i&nostand)				//on something bad
	{
		if(forent)printf("btCheckGroundDist: %d Bad Ground\n",
			forent->entnum);
		return(0);
	}

	//ok, can stand here
	return(1);
}

/** Determine if the location is on the ground */
BTSV_API int btCheckOnGround(vec3 org, vec3 mins, vec3 maxs,
	btEntity forent)
{
	return(btCheckGroundDist(org, mins, maxs, 1, forent));
}

BTSV_API vec3 btCalcGroundFloorPoint(vec3 org, vec3 mins, vec3 maxs, 
	btEntity forent)
{
	int i;
	if(!btCheckGroundDist(org, mins, maxs, 24, forent))
		return(org);
	
	while(!btCheckGroundDist(org, mins, maxs, 4, forent))
		org=v3sub(org, vec3(0,0,3.5));
	while(!btCheckGroundDist(org, mins, maxs, 1, forent))
		org=v3sub(org, vec3(0,0,0.5));
	return(org);
}

BTSV_API vec3 btYawVector(float yaw)
{
	float f;
	f=yaw*(M_PI/180);
	return(vec3(cos(f), sin(f), 0));
}

BTSV_API float btCurrentYaw(btEntity self)
	{ return(v3z(self->angles)); }
BTSV_API float btCurrentPitch(btEntity self)
	{ return(v3x(self->angles)); }

BTSV_API void btSetYaw(btEntity self, float ny)
{
	self->angles=vec3(v3x(self->angles), v3y(self->angles), ny);
}

BTSV_API float btAngleMod(float v)
{
	while(v>=360)v-=360;
	while(v<0)v+=360;
	return(v);
}

BTSV_API float btAngleMod2(float v)
{
	while(v>=180)v-=360;
	while(v<-180)v+=360;
	return(v);
}

BTSV_API vec3 btAngleMod3(vec3 ang)
{
	return(vec3(
		btAngleMod2(v3x(ang)),
		btAngleMod2(v3y(ang)),
		btAngleMod(v3z(ang))));
}

BTSV_API vec3 btVecToAngles(vec3 dir)
{
	float yaw, pitch;
	
	yaw=btVecToYaw(dir);
	pitch=asin(v3z(dir))*(180/M_PI);
	return(vec3(pitch, 0, yaw));
}

BTSV_API float btVecToYaw(vec3 dir)
{
	float f;
	f=atan2(v3y(dir), v3x(dir))*(180/M_PI);
	f=btAngleMod(f);
	return(f);
}

void BT_WalkmovePointAdjGround(btEntity self, vec3 org)
{
	vec3 org1;

	org1=btCalcGroundFloorPoint(org, self->mins, self->maxs, self);
	if(btCheckOnGround(org1, self->mins, self->maxs, self))
	{
		//managed to align with ground
		self->origin=org1;
	}else
	{
		//ok, failed to realign, so just found in-air
		self->flags&=~BT_FL_ONGROUND;
		self->origin=org1;
	}
}

BTSV_API void btDropToFloor(btEntity self)
{
	BT_WalkmovePointAdjGround(self, self->origin);
}

BTSV_API int btWalkmove(btEntity self, float yaw, float dist)
{
	vec3 org, dir, org1;
	int i;

	if(dist>0)
	{
//		printf("btWalkmove: %d %f, %d %d\n", (int)yaw, dist,
//			(int)self->idealYaw, (int)btCurrentYaw(self));
	}

#if 1
	if(self->flags&BT_FL_SCRIPTED)
	{
		//predict and update flags as usual

//		dir=BT_TargetDirection(self, self->goalEntity);
//		dir=v3scale(dir, dist/btge_framedt);
		dir=v3scale(btYawVector(yaw), dist/btge_framedt);

		org=btBoxStepMove(self->origin, self->mins, self->maxs,
			dir, btge_framedt, self);
		if(self->flags&BT_FL_SIMPLEMOVE)
		{
			self->origin=org;
			return(1);
		}

		//but force the movement
		org=v3add(self->origin, v3scale(dir, btge_framedt));
		self->origin=org;
		return(1);
	}
#endif

#if 1
//	btge_tempSelf=self;
	dir=v3scale(btYawVector(yaw), dist/btge_framedt);
	org=btBoxStepMove(self->origin, self->mins, self->maxs,
		dir, btge_framedt, self);
	self->origin=org;
//	btge_tempSelf=NULL;
	if(self->flags&BT_FL_SIMPLEMOVE)
		return(1);
	return(0);
#endif

#if 0
	//calculate desired spot
	dir=v3scale(btYawVector(yaw), dist);
	org=v3add(self->origin, dir);

	if(self->flags&BT_FL_FLY)
	{
		if(btCheckFlySpot(org, self->mins, self->maxs, self))
		{
			self->origin=org;
			return(1);
		}
		return(0);
	}

	if(self->flags&BT_FL_SWIM)
	{
		if(btCheckSwimSpot(org, self->mins, self->maxs, self))
		{
			self->origin=org;
			return(1);
		}
		return(0);
	}

	if(btCheckOnGround(org, self->mins, self->maxs, self))
	{
		//ok, no problem
		self->origin=org;
		return(1);
	}

	if(btCheckGroundDist(org, self->mins, self->maxs, 12, self))
	{
		//ok, but no longer on-ground (gone off a step or similar)
		BT_WalkmovePointAdjGround(self, org);
//		self->flags&=~BT_FL_ONGROUND;
//		self->origin=org;
		return(1);
	}
	
	org1=v3add(org, vec3(0,0,8));
	if(btCheckGroundDist(org1, self->mins, self->maxs, 12, self))
	{
		//ok, but went up a step or similar
		BT_WalkmovePointAdjGround(self, org1);
		return(1);
	}
	
	//can't go there
	return(0);
#endif
}

/** Calculate the physical center-point of an entity,
  * which is not necessarily at its origin.
  */
BTSV_API vec3 btCalcCenter(btEntity self)
{
	vec3 org, mid;
	mid=v3scale(v3add(self->mins, self->maxs), 0.5);
	org=v3add(self->origin, mid);
	return(org);
}

/** Calculate the bounding radius for an entity from its center. */
BTSV_API float btCalcRadius(btEntity self)
{
	vec3 org;
	float rad;
//	org=btCalcCenter(self);
	org=v3scale(v3add(self->mins, self->maxs), 0.5);
	rad=v3len(v3sub(self->maxs, org));
	return(rad);
}

/** Current frame step time */
BTSV_API float btStepTime()
{
	return(btge_framedt);
}

/** Current frame step in 0.1s units */
BTSV_API float btRelStepTime()
{
	return(btge_framedt/0.1);
}

/** A random number in the range 0 to 1 */
BTSV_API float btRandom()
{
	return(rand()/((float)RAND_MAX));
}

/** A random number in the range -1 to 1 */
BTSV_API float btSRandom()
{
	return(btRandom()*2-1);
}

BTSV_API float btRangeRandom(float min, float max)
{
	return(min+btRandom()*(max-min));
}

BTSV_API int btRangeRandomInt(int min, int max)
{
	int i;

	i=btRangeRandom(min, max+0.5);
	if(i>max)i=max;
	return(i);
}

/** Move an entity to a given origin.
  * Should be used instead of setting origin directly.
  */
BTSV_API void btSetOrigin(btEntity self, vec3 org)
{
	self->origin=org;
	self->flags|=BT_FL_SET_POSITION;
}

/** Set angles for an entity.
  * Should be used instead of setting angles directly.
  */
BTSV_API void btSetAngles(btEntity self, vec3 ang)
{
	self->angles=ang;
	self->flags|=BT_FL_SET_POSITION;
}

/** Set angles2 for an entity.
  * Should be used instead of setting angles2 directly.
  */
BTSV_API void btSetAngles2(btEntity self, vec3 ang)
{
	self->angles2=ang;
	self->flags|=BT_FL_SET_POSITION;
}

/** Set the angles for an entity.
  * Should be used instead of setting angles directly.
  */
BTSV_API void btSetRotation(btEntity self, quat rot)
{
	self->rot=rot;
	self->flags|=BT_FL_SET_POSITION;
}

/** Set the angles for an entity.
  * Should be used instead of setting angles directly.
  */
BTSV_API void btSetVelocity(btEntity self, vec3 vel)
{
	self->velocity=vel;
	self->flags|=BT_FL_SET_POSITION;
}

/** Set the solidtype and movetype for an entity.
  * Should be used instead of setting these fields directly.
  * May force re-synchronizing physics.
  */
BTSV_API void btSetSolid(btEntity self, int solidtype, int movetype)
{
	self->solidtype=solidtype;
	self->movetype=movetype;
	self->flags|=BT_FL_SET_SOLID;
}

/** Set the bbox for an entity.
  * Should be used instead of setting the bbox directly.
  */
BTSV_API void btSetBBox(btEntity self, vec3 mins, vec3 maxs)
{
	self->mins=mins;
	self->maxs=maxs;
	self->flags|=BT_FL_SET_SOLID;
}

/** Set the radius for an entity.
  * Should be used instead of setting the radius directly.
  * Specific To: Sphere solidtype.
  */
BTSV_API void btSetRadius(btEntity self, float radius)
{
	self->radius=radius;
	self->flags|=BT_FL_SET_SOLID;
}

/** Set the height and radius for an entity.
  * Should be used instead of setting the bbox directly.
  * Specific To: Cylinder/Capsule solidtypes.
  */
BTSV_API void btSetHeightRadius(btEntity self,
	float height, float radius)
{
	self->height=height;
	self->radius=radius;
	self->flags|=BT_FL_SET_SOLID;
}

/** Set the current model for an entity.
  * Should be used instead of setting model directly.
  */
BTSV_API void btSetModel(btEntity self, char *str)
{
	self->model=dystrdup(str);
}

/** Set the current weapon model for an entity.
  * Should be used instead of setting weapon model directly.
  */
BTSV_API void btSetViewModel(btEntity self, char *str)
{
	self->weaponmodel=dystrdup(str);
}
