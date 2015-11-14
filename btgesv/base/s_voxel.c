#include <btgesv.h>

BTSV_API void BTSV_VoxelPower_UpdateVoxel_NoteBlock(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	BTGE_VoxelData *vox, int x, int y, int z)
{
	int pwr;
	int i, j, k;

	pwr=BTGE_VoxelPower_GetBlockAmbientPower(world, chk, vox, x, y, z);

	if((pwr>0) && !(vox->aux&8))
	{
		vox=BTGE_Voxel_GetWorldRegionVoxelDirty(world, chk->rgn, x, y, z);
		vox->aux|=8;
		BTSV_Voxel_NoteBlock_Play(world, vox, x, y, z);
	}

	if((pwr<=0) && (vox->aux&8))
	{
		vox=BTGE_Voxel_GetWorldRegionVoxelDirty(world, chk->rgn, x, y, z);
		vox->aux&=~8;
	}
}

BTSV_API void BTSV_VoxelPower_UpdateVoxel_TNT(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	BTGE_VoxelData *vox, int x, int y, int z)
{
	int pwr;
	int i, j, k;

	pwr=BTGE_VoxelPower_GetBlockAmbientPower(world, chk, vox, x, y, z);

	if((pwr>0) && !(vox->aux&8))
	{
		vox=BTGE_Voxel_GetWorldRegionVoxelDirty(world, chk->rgn, x, y, z);
		vox->aux|=8;
//		BTSV_Voxel_NoteBlock_Play(world, vox, x, y, z);
		BTSV_Voxel_TNT_Fire(world, vox, x, y, z);
	}

	if((pwr<=0) && (vox->aux&8))
	{
		vox=BTGE_Voxel_GetWorldRegionVoxelDirty(world, chk->rgn, x, y, z);
		vox->aux&=~8;
	}
}

BTSV_API void BTSV_VoxelPower_UpdateVoxel_CommandBlock(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	BTGE_VoxelData *vox, int x, int y, int z)
{
	int pwr;
	int i, j, k;

	pwr=BTGE_VoxelPower_GetBlockAmbientPower(world, chk, vox, x, y, z);

	if((pwr>0) && !(vox->aux&8))
	{
		vox=BTGE_Voxel_GetWorldRegionVoxelDirty(world, chk->rgn, x, y, z);
		vox->aux|=8;
		BTSV_Voxel_CommandBlock_Fire(world, vox, x, y, z);
//		BTSV_Voxel_TNT_Fire(world, vox, x, y, z);
	}

	if((pwr<=0) && (vox->aux&8))
	{
		vox=BTGE_Voxel_GetWorldRegionVoxelDirty(world, chk->rgn, x, y, z);
		vox->aux&=~8;
	}
}

BTSV_API void BTSV_VoxelPower_UpdateVoxel(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	BTGE_VoxelData *vox, int x, int y, int z)
{
	if((vox->type&VOX_TY_MASK)==VOX_TY_COMMAND_BLOCK)
	{
		BTSV_VoxelPower_UpdateVoxel_CommandBlock(world, chk, vox, x, y, z);
		return;
	}

	if((vox->type&VOX_TY_MASK)==VOX_TY_NOTE_BLOCK)
	{
		BTSV_VoxelPower_UpdateVoxel_NoteBlock(world, chk, vox, x, y, z);
		return;
	}

	if((vox->type&VOX_TY_MASK)==VOX_TY_TNT)
	{
		BTSV_VoxelPower_UpdateVoxel_TNT(world, chk, vox, x, y, z);
		return;
	}
	
	BTGE_VoxelPower_UpdateVoxel(world, chk, vox, x, y, z);
}

BTSV_API void BTSV_Voxel_NoteBlock_Play(
	BTGE_BrushWorld *world, BTGE_VoxelData *vox,
	int x, int y, int z)
{
	btEntity ent;
	char *s;
	dyt v;

	ent=btNewEntity();
	ent->origin=vec3d(x*32+16, y*32+16, z*32+16);
	btSetThink(ent, btDeleteEntity, 0.2);
	
	s="sound/world/klaxon1";
	
//	btSound(ent, BT_CHAN_VOICE, "sound/weapons/rocklx1a",
//		1.0, BT_ATTN_NORM);

	v=BTGE_Voxel_WorldGetIndexValue(world, x, y, z, vox->aux2);
	if(v && dystringp(v))
	{
		s=dystringv(v);
	}

	btSound(ent, BT_CHAN_VOICE, s,
		1.0, BT_ATTN_NORM);
}

BTSV_API void BTSV_Voxel_TNT_Fire(
	BTGE_BrushWorld *world, BTGE_VoxelData *vox,
	int x, int y, int z)
{
	btEntity ent;

	ent=btNewEntity();
	ent->origin=vec3d(x*32+16, y*32+16, z*32+16);
	
	ent->damage=120;
	ent->height=160;

	btSound(ent, BT_CHAN_VOICE, "sound/misc/fuse1_5s",
		1.0, BT_ATTN_NORM);
	
	btSetThink(ent, BT_GrenadeThink, 5);
	
//	btSound(ent, BT_CHAN_VOICE, "sound/weapons/rocklx1a",
//		1.0, BT_ATTN_NORM);

//	btSound(ent, BT_CHAN_VOICE, "sound/world/klaxon1",
//		1.0, BT_ATTN_NORM);
}

BTSV_API void BTSV_Voxel_CommandBlock_Fire(
	BTGE_BrushWorld *world, BTGE_VoxelData *vox,
	int x, int y, int z)
{
	btEntity ent;
	char *s;
	dyt v, o;

	ent=btNewEntity();
	ent->origin=vec3d(x*32+16, y*32+16, z*32+16);
	btSetThink(ent, btDeleteEntity, 0.2);
	
	s=NULL;
	
//	btSound(ent, BT_CHAN_VOICE, "sound/weapons/rocklx1a",
//		1.0, BT_ATTN_NORM);

	v=BTGE_Voxel_WorldGetIndexValue(world, x, y, z, vox->aux2);
	if(v && dystringp(v))
	{
		s=dystringv(v);
	}

	printf("BTSV_Voxel_CommandBlock_Fire: %p %s\n", v, s?s:"-");

	if(!s || !(*s))
		return;

	if(*s==';')
	{
		o=dyObject1("self", (dyt)ent);
		btScriptEvalObj(s+1, o);
		return;
	}

	BTGE_ConsoleCmd(s);

//	btEntityScriptEval(ent, s);

//	btSound(ent, BT_CHAN_VOICE, s,
//		1.0, BT_ATTN_NORM);
}

BTSV_API int BTSV_Voxel_SmallDamageEvent(
	BTGE_BrushWorld *world, BTGE_VoxelData *vox,
	int x, int y, int z, double *org,
	float radius, float damage, float dmg)
{
	BTGE_VoxelInfo *inf;

	if((vox->type&VOX_TY_MASK)==VOX_TY_NOTE_BLOCK)
	{
		vox=BTGE_Voxel_GetWorldVoxelDirty(world, x, y, z);
		vox->aux=(vox->aux&0xF8)|((vox->aux+1)&7);
		BTSV_Voxel_NoteBlock_Play(world, vox, x, y, z);

//		vox->aux^=8;
		return(0);
	}

}

BTSV_API void BTSV_Voxel_TouchEvent_PressurePlate(
	BTGE_BrushWorld *world, BTGE_VoxelData *vox,
	int x, int y, int z, int fl, btEntity ent)
{
	if(!(vox->aux&1))
	{
		vox=BTGE_Voxel_GetWorldVoxelDirty(world, x, y, z);
		vox->aux=1;
//		vox->type=VOX_TY_REDSTONE_TORCH0;
		vox->delay=10;
	}
}

BTSV_API void BTSV_Voxel_CheckFireBBoxTrigger(btEntity self,
	vec3 mins, vec3 maxs)
{
	vec3d amins, amaxs;

	amins=v3dadd(self->origin, v3f2d(mins));
	amaxs=v3dadd(self->origin, v3f2d(maxs));
	BTSV_Voxel_UpdateBoxTrigger(btCurrentWorld(), amins, amaxs, 0, self);
}

BTSV_API void BTSV_Voxel_UpdateBoxTrigger(
	BTGE_BrushWorld *world, vec3d amins, vec3d amaxs, int fl, btEntity ent)
{
	double am[4], an[4];
	int ami[4], ani[4];
	BTGE_VoxelData *vox;
	BTGE_VoxelInfo *inf;
	float f, g, br;
	int i, j, k, l0, l1, ty;
	
	vdvec3d(am, amins);
	vdvec3d(an, amaxs);

//	ami[0]=am[0]/32; ami[1]=am[1]/32; ami[2]=am[2]/32;
//	ani[0]=an[0]/32+0.99; ani[1]=an[1]/32+0.99; ani[2]=an[2]/32+0.99;

//	ami[0]=am[0]/32-1; ami[1]=am[1]/32-1; ami[2]=am[2]/32-1;
//	ani[0]=an[0]/32+1; ani[1]=an[1]/32+1; ani[2]=an[2]/32+1;

	ami[0]=floor(am[0]/32); ami[1]=floor(am[1]/32); ami[2]=floor(am[2]/32);
	ani[0]=floor(an[0]/32); ani[1]=floor(an[1]/32); ani[2]=floor(an[2]/32);
//	ani[0]=ceil(an[0]/32); ani[1]=ceil(an[1]/32); ani[2]=ceil(an[2]/32);

	for(i=ami[0]; i<=ani[0]; i++)
		for(j=ami[1]; j<=ani[1]; j++)
			for(k=ami[2]; k<=ani[2]; k++)
	{
		vox=BTGE_Voxel_GetWorldVoxel(world, i, j, k);
		if(vox && vox->type)
		{
			ty=vox->type&VOX_TY_MASK;

			inf=BTGE_Voxel_GetWorldVoxelInfo(world, vox, i, j, k);
			if(!inf)continue;
			
			if(inf->TouchEvent)
			{
				inf->TouchEvent(world, vox, i, j, k, fl, ent);
			}

//			if(ty==VOX_TY_PORTAL)
//			{
//				BTSV_Voxel_TouchEvent_Portal(world, vox, i, j, k, fl, ent);
//			}

//			vox=BTGE_Voxel_GetWorldVoxelDirty(world, i, j, k);
//			if(!vox || !(vox->type&VOX_TY_MASK))
//			{
//				//there was a block here, but it's gone now...
//				continue;
//			}
		}
	}
}

BTSV_API void BTSV_Voxel_TouchEvent_Portal(
	BTGE_BrushWorld *world, BTGE_VoxelData *vox,
	int x, int y, int z, int fl, btEntity self)
{
	vec3d sorg, eorg, dorg, mid;

	char *s;
	dyt v;

//	if(ent->flags2&BT_FL2_STOPPORTAL)
//		return;

	if(!BT_PortalFinishedP(self))
		return;

	v=BTGE_Voxel_WorldGetIndexValue(world, x, y, z, vox->aux2);
	if(v && dystringp(v))
		{ s=dystringv(v); }
	else { s=NULL; }

	if(!s)
		{ s=world->vox_terr->portal_map[vox->aux&15]; }

	if(s)
	{
		if(!(self->flags&BT_FL_CLIENT))
		{
			BT_PortalFinished(self, 5);
			return;
		}
	
		btDPrint("changelevel_use: %s\n", s);
		btConsoleCmd("map \"%s\"\n", s);
	}

	BT_PortalFinished(self, 5);

	mid=v3f2d(v3scale(v3add(self->mins, self->maxs), 0.5));
	sorg=v3dadd(self->origin, mid);
//	eorg=v3dadd(dorg, mid);

	dorg=vec3d(-x*32, -y*32, (z+16)*32);
	dorg=BTSV_Voxel_FindEntitySpawnPosition(self, dorg);

	btParticleTeleportExplosion(sorg);
//	btParticleTeleportExplosion(eorg);

	btSound(self, BT_CHAN_VOICE, "sound/misc/tele1",
		1.0, BT_ATTN_NORM);

	btCenterprint(self, "Portal");
	self->flags2|=BT_FL2_STOPPORTAL;

	self->origin=dorg;
//	self->angles=other->angles;
	self->flags&=~BT_FL_ONGROUND;
	if(self->flags&BT_FL_CLIENT)
		self->pauseTime=btDelayTime(0.1);
}

BTSV_API void BTSV_Voxel_CheckFireBBoxPortal(btEntity self,
	vec3 mins, vec3 maxs)
{
//	btCenterprint(self, "Portal");
}

BTSV_API vec3d BTSV_Voxel_FindEntitySpawnPosition(btEntity self, vec3d sorg)
{
	BTGE_BrushWorld *world;
	int x, y, z;
	int i;
	
	world=btCurrentWorld();
	x=floor(v3dx(sorg)/32);
	y=floor(v3dy(sorg)/32);
	z=floor(v3dz(sorg)/32);
	
	i=BTGE_Voxel_FindNearestSpawnBlockType(world, x, y, z, &x, &y, &z,
		VOX_TYS_ANYSOLIDBLOCK, 256);
	if(i)
	{
		return(vec3d(x*32+16, y*32+16, z*32+48));
	}
	return(self->origin);
}
