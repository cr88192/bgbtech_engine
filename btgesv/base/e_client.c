#include <btgesv.h>

BTGE_SEntity *btge_client_spawnent;

BTSV_API void BT_PreserveClientState(btEntity self)
{
	BTGE_SEntity *sent;

	sent=BTGE_SEnt_NewEntity(NULL, "info_player_saved");
	BT_FlattenClientState(self, sent);
	btge_client_spawnent=sent;
}

BTSV_API void BT_RestoreClientState(btEntity self)
{
	if(btge_client_spawnent)
	{
		BT_UnflattedClientState(self, btge_client_spawnent);
	}
}

BTSV_API void BT_UnflattedClientState(btEntity self, BTGE_SEntity *sent)
{
	float tv[4];
	btEntity ent;
	char *s;
	int i;

	s=BTGE_SEnt_GetStr(sent, "landmark");
	if(s)
	{
		ent=btFindTargets(s);
		if(ent)
		{
			BTGE_SEnt_GetVec3(sent, "movedir", tv);
			self->origin=v3dadd(ent->origin, vec3dvf(tv));

			BTGE_SEnt_GetVec3(sent, "angles", tv);
			self->angles=vec3vf(tv);

//			self->angles=ent->angles;
//			self->origin
		}
	}

	i=BTGE_SEnt_GetNum(sent, "flags");
	if(i>0)self->flags|=i;

	i=BTGE_SEnt_GetNum(sent, "itemflags");
	if(i>0)self->itemflags|=i;

	i=BTGE_SEnt_GetNum(sent, "weaponnum");
	if(i>0)
	{
		BT_PlayerWeaponChange(self, i);
	}

	i=BTGE_SEnt_GetNum(sent, "cur_hp");
	if(i>0)self->health=i;
	i=BTGE_SEnt_GetNum(sent, "cur_mp");
	if(i>0)self->curMP=i;
}

BTSV_API void BT_FlattenClientState(btEntity self, BTGE_SEntity *sent)
{
	BTGE_SEnt_SetStr(sent, "origin", btVec3ToStr(v3d2f(self->origin)));
	BTGE_SEnt_SetStr(sent, "angles", btVec3ToStr(self->angles));
	BTGE_SEnt_SetNum(sent, "flags", self->flags);
	BTGE_SEnt_SetNum(sent, "itemflags", self->itemflags);
	BTGE_SEnt_SetNum(sent, "weaponnum", self->weaponnum);

	BTGE_SEnt_SetNum(sent, "cur_hp", self->health);
	BTGE_SEnt_SetNum(sent, "cur_mp", self->curMP);

	if(self->landmark)
	{
		BTGE_SEnt_SetStr(sent, "landmark", self->landmark);
		BTGE_SEnt_SetStr(sent, "movedir", btVec3ToStr(self->movedir));
	}
}

BTSV_API bool BT_CheckAllowCheats(btEntity self)
{
	int i;
	if(btGameIsDeathmatch() || btGameIsCoop())
	{
		i=btCvarGetf("sv_cheats");
		if(i)return(true);
		btPrint(self, "This command requires sv_cheats to be set\n");
		return(false);
	}

	return(true);
}

BTSV_API void BT_ClientConCmd(btEntity self, char **a)
{
	char tb[1024];
	btItem item;
	vec3 v0;
	char *s, *t;
	dyt v;
	int i, j, k;

	if(!self)
		return;

	if(!strcmp(a[0], "invul"))
	{
		if(!BT_CheckAllowCheats(self))return;
		self->flags^=BT_FL_INVULNERABLE;
		btPrint(self, "Invul: %s\n",
			(self->flags&BT_FL_INVULNERABLE)?"Enabled":"Disabled");
		return;
	}
	if(!strcmp(a[0], "notarget"))
	{
		if(!BT_CheckAllowCheats(self))return;
		self->flags^=BT_FL_NOTARGET;
		btPrint(self, "Notarget: %s\n",
			(self->flags&BT_FL_NOTARGET)?"Enabled":"Disabled");
		return;
	}
	if(!strcmp(a[0], "noclip"))
	{
		if(!BT_CheckAllowCheats(self))return;
		self->flags^=BT_FL_NOCLIP;
		btPrint(self, "Noclip: %s\n",
			(self->flags&BT_FL_NOCLIP)?"Enabled":"Disabled");
		return;
	}

	if(!strcmp(a[0], "fastmp"))
	{
		if(!BT_CheckAllowCheats(self))return;
		self->flags^=BT_FL_FASTMP;
		btPrint(self, "FastMP: %s\n",
			(self->flags&BT_FL_FASTMP)?"Enabled":"Disabled");
		return;
	}

	if(!strcmp(a[0], "railmove"))
	{
		if(!BT_CheckAllowCheats(self))return;
		self->flags2^=BT_FL2_RAILMOVE;
		btPrint(self, "Railmove: %s\n",
			(self->flags2&BT_FL2_RAILMOVE)?"Enabled":"Disabled");
		return;
	}

	if(!strcmp(a[0], "creative"))
	{
		if(!BT_CheckAllowCheats(self))return;
		self->flags^=BT_FL_CREATIVE;
		btPrint(self, "Creative Mode: %s\n",
			(self->flags&BT_FL_CREATIVE)?"Enabled":"Disabled");

		if(self->flags&BT_FL_CREATIVE)
			{ self->effects2|=BT_EF2_CAM_CREATIVE; }
		else { self->effects2&=~BT_EF2_CAM_CREATIVE; }

		return;
	}

	if(!strcmp(a[0], "guimode"))
	{
		if(!BT_CheckAllowCheats(self))return;
		self->flags2^=BT_FL2_GUIMODE;
		btPrint(self, "GUI Mode: %s\n",
			(self->flags2&BT_FL2_GUIMODE)?"Enabled":"Disabled");

		if(self->flags2&BT_FL2_GUIMODE)
			{ self->effects2|=BT_EF2_CAM_GUIMODE; }
		else { self->effects2&=~BT_EF2_CAM_GUIMODE; }

		return;
	}

	if(!strcmp(a[0], "clientmove"))
	{
		if(!BT_CheckAllowCheats(self))return;
		self->flags2^=BT_FL2_CLIENTMOVE;
		btPrint(self, "ClientMove Mode: %s\n",
			(self->flags2&BT_FL2_CLIENTMOVE)?"Enabled":"Disabled");

		if(self->flags2&BT_FL2_CLIENTMOVE)
			{ self->effects2|=BT_EF2_CAM_CLIENTMOVE; }
		else { self->effects2&=~BT_EF2_CAM_CLIENTMOVE; }

		return;
	}

	if(!strcmp(a[0], "camera"))
	{
		if(!BT_CheckAllowCheats(self))return;

		if(!a[1])
		{
			btPrint(self, "Usage: camera <cmd> [opts]\n");
			return;
		}

		if(!strcmp(a[1], "2D") || !strcmp(a[1], "2d"))
		{
			self->effects2^=BT_EF2_CAM_2D;
			btPrint(self, "Camera2D: %s\n",
				(self->effects2&BT_EF2_CAM_2D)?"Enabled":"Disabled");
			return;
		}

		if(!strcmp(a[1], "fixed_org"))
		{
			self->effects2^=BT_EF2_CAM_FIXED_ORG;
			btPrint(self, "Camera Fixed Origin: %s\n",
				(self->effects2&BT_EF2_CAM_FIXED_ORG)?"Enabled":"Disabled");
			return;
		}
		if(!strcmp(a[1], "fixed_rot"))
		{
			self->effects2^=BT_EF2_CAM_FIXED_ROT;
			btPrint(self, "Camera Fixed Rotation: %s\n",
				(self->effects2&BT_EF2_CAM_FIXED_ROT)?"Enabled":"Disabled");
			return;
		}

		if(!strcmp(a[1], "hflip"))
		{
			self->flags2^=BT_FL2_VIEWHFLIP;
			btPrint(self, "View Horizontal Flip: %s\n",
				(self->flags2&BT_FL2_VIEWHFLIP)?"Enabled":"Disabled");
		}

		if(!strcmp(a[1], "vflip"))
		{
			self->flags2^=BT_FL2_VIEWVFLIP;
			btPrint(self, "View Vertical Flip: %s\n",
				(self->flags2&BT_FL2_VIEWVFLIP)?"Enabled":"Disabled");
		}
	}

	if(!strcmp(a[0], "give"))
	{
		if(!BT_CheckAllowCheats(self))return;

		if(!a[1])
		{
			btPrint(self, "Usage: give <item>\n");
			return;
		}
		
		if(!strcmp(a[1], "all"))
		{
			self->itemflags|=BT_ITFL_GIVEALL;
			return;
		}

		item=btGetItem(a[1]);
		if(item)
		{
#if 0
			if(item->canPickup)
			{
				if(!item->canPickup(self, self))
					return;
			}else
			{
				if(!item_generic_canPickup(self, self))
					return;
			}
#endif

			btSound(self, BT_CHAN_ITEM, item->touchSound,
				1.0, BT_ATTN_NORM);

			if(item->touchThink)
				item->touchThink(self, item);

			return;
		}

//		v0=vec3vf(org);
		v0=v3d2f(self->origin);
		s=dystrdup(a[1]);
		btSpawnEntityBasic(s, v0, 0);

		return;
	}

	if(!strcmp(a[0], "placervox"))
	{
		if(!BT_CheckAllowCheats(self))return;

		if(!a[1])
		{
			btPrint(self, "Usage: placervox <name>\n");
			return;
		}

		if(!strcmp(a[1], "normal"))
		{
			self->placer_vox=(self->placer_vox&VOX_TY_MASK)|
				VOX_TYM_NORMAL;
			return;
		}

		if(!strcmp(a[1], "flexform"))
		{
			self->placer_vox=(self->placer_vox&VOX_TY_MASK)|
				VOX_TYM_FLEXFORM;
			return;
		}

		if(!strcmp(a[1], "aux"))
		{
			i=gcratoi(a[2]);
			self->placer_aux=(self->placer_aux&0xFFFFFF00)|i;
			return;
		}

		if(!strcmp(a[1], "aux2"))
		{
			i=gcratoi(a[2]);
			self->placer_aux=(self->placer_aux&0xFF0000FF)|(i<<8);
			return;
		}

		if(!strcmp(a[1], "value"))
		{
			if(!a[2])
			{
				self->placer_value=NULL;
				return;
			}
		
			s=a[2];
			if(s[strlen(s)+1]=='\"')
			{
				v=dystring(s);
			}else
			{
				v=dysParseFromString(s);
			}

			self->placer_value=v;
			return;
		}

		i=BTGE_Voxel_IndexVoxelName(a[1]);
		if(i>=0)
		{
			self->placer_vox=(self->placer_vox&VOX_TYM_MASK)|(i&VOX_TY_MASK);
		}

		return;
	}

	if(!strcmp(a[0], "speak"))
	{
		t=tb;
		for(i=1; a[i]; i++)
		{
			strcpy(t, a[i]);
			t+=strlen(t);
			*t++=' ';
		}
		*t++=0;

//		btSpeak(self, BT_CHAN_VOICE, tb,
//			1.0, BT_ATTN_NORM);
		btSpeak(self, BT_CHAN_VOICE, tb,
			1.0, BT_ATTN_LOW);
		return;
	}
}

void BT_ClientInventoryClick(btEntity self, char *str)
{
	int i;

	i=BTGE_Voxel_IndexVoxelName(str);
	if(i>=0)
	{
		self->placer_vox=(self->placer_vox&VOX_TYM_MASK)|(i&VOX_TY_MASK);
	}
}

//called every frame for clients
void BT_ClientThink(btEntity self)
{
	float tv0[3];
	vec3d org0, org1;
	vec3 dir;
	float dt, f, g, d;
	char *s;
	int i, ofl;

	self->effects2&=~BT_EF2_CAM_WEAPONFLIP;

	if(self->flags2&BT_FL2_VIEWHFLIP)
	{
		self->effects2^=BT_EF2_CAM_WEAPONFLIP;
		self->effects2|=BT_EF2_CAM_HFLIP;
	}
	else
		{ self->effects2&=~BT_EF2_CAM_HFLIP;	}

	if(self->flags2&BT_FL2_VIEWVFLIP)
		{ self->effects2|=BT_EF2_CAM_VFLIP;	}
	else
		{ self->effects2&=~BT_EF2_CAM_VFLIP;	}

	btViewFlags(self, self->effects2);
	
	if(self->effects2&BT_EF2_CAM_2D)
	{
		btSetViewModel(self, "");

		if(v3dist(self->viewMins, self->viewMaxs)>1)
		{
			btViewMinMax(self, self->viewMins, self->viewMaxs);
		}else
		{
//			btViewMinMax(self, vec3(-400, -300, -4096), vec3(400, 300, 0));
			btViewMinMax(self, vec3(-400, -300, 0), vec3(400, 300, 4096));
		}
	}

	tv0[0]=v3dx(self->origin)-self->client->reforg[0];
	tv0[1]=v3dy(self->origin)-self->client->reforg[1];
	tv0[2]=v3dz(self->origin)-self->client->reforg[2];
	f=self->client->reforg[0]+
		self->client->reforg[1]+
		self->client->reforg[2];
	
//	if(V3F_LEN(tv0)>1024)
//	if((V3F_LEN(tv0)>8192) ||
//		((f!=0) && (v3len(self->origin)<8192)))

	d=16384;
	if((fabs(tv0[0])>d) ||
		(fabs(tv0[1])>d) ||
		(fabs(tv0[2])>d) ||
		((f!=0) && (v3dlen(self->origin)<d)))
	{
		self->client->reforg[0]=floor(v3dx(self->origin)/32)*32;
		self->client->reforg[1]=floor(v3dy(self->origin)/32)*32;
		self->client->reforg[2]=floor(v3dz(self->origin)/32)*32;

		if(abs(self->client->reforg[0])<d)
			self->client->reforg[0]=0;
		if(abs(self->client->reforg[1])<d)
			self->client->reforg[1]=0;
		if(abs(self->client->reforg[2])<d)
			self->client->reforg[2]=0;

		printf("Client %d Ref-Origin %d,%d,%d\n",
			self->client->clnum,
			(int)(self->client->reforg[0]),
			(int)(self->client->reforg[1]),
			(int)(self->client->reforg[2]));
	}

#if 0
	if(v3len(self->origin)<8192)
	{
		self->client->reforg[0]=0;
		self->client->reforg[1]=0;
		self->client->reforg[2]=0;

//		printf("Client %d Ref-Oirigin Reset\n",
//			self->client->clnum);
	}
#endif

#if 1
	if(self->effects2&BT_EF2_CAM_FORCEANGLE)
	{
//		if(self->oldeffects2&BT_EF2_CAM_FORCEANGLE)
		if(self->pauseTime<btTime())
		{
			//FORCEANGLE only applies for a limited time
			self->effects2&=~BT_EF2_CAM_FORCEANGLE;
		}
	}
#endif	

	if(self->health<=0)
	{
		if(self->impulseFlags)
		{
			BT_PlayerImpulse(self, self->impulseFlags);
			self->impulseFlags=0;
		}

		return;
	}

	if(self->flags&BT_FL_NOCLIP)
	{
		//cancel out any velocity when noclip
		self->velocity=vec3(0,0,0);
	}

	if(!(self->effects2&BT_EF2_CAM_FIXED_ORG))
	{
//		if(btCheckOnGround(self->origin, self->mins, self->maxs, self))
//			self->flags|=BT_FL_ONGROUND;

		if(v3z(self->impulseVelocity)>0)
		{
//			printf("Player Jump %p, %08X\n", self, self->flags);
			
			if(self->flags&BT_FL_ONGROUND)
			{
				s="sound/player/jump1";
				btSound(self, BT_CHAN_BODY, s, 1.0, BT_ATTN_NORM);
//				btSpeak(self, BT_CHAN_BODY, "jump", 1.0, BT_ATTN_NORM);

				self->flags&=~BT_FL_ONGROUND;
			}
		}

//		if((self->flags2&BT_FL2_RAILMOVE) && BT_EntityOnRailP(self))
		if(self->flags2&BT_FL2_RAILMOVE)
		{
			self->mins=vec3(-12, -12,  16);

//			dir=v3addscale(self->velocity,
//				self->impulseVelocity, btStepTime());
			dir=v3addscale(self->velocity,
				self->impulseVelocity, 0.5);

#if 0
			f=v3len(dir);
				
			org1=v3daddscale(self->origin,
				v3f2d(dir), btStepTime());
			org1=BTGE_VoxelRail_AlignPointRail2(
				btCurrentWorld(), org1,
				self->mins, self->maxs);
			dir=v3d2f(v3dsub(org1, self->origin));
//			dir=v3scale(dir, 1.0/btStepTime());
//			dir=v3add(self->velocity, dir);
//			f=v3len(dir);
//			if(f>300)
//				{ dir=v3scale(dir, 300/f); }

			dir=v3norm(dir);
			if(f>400)f=400;
			dir=v3scale(dir, f);
#endif

			self->velocity=dir;
		}else
		{
			self->mins=vec3(-12, -12,  0);

			ofl=self->flags;
			org0=self->origin;
	
			self->origin=btBoxStepMove(
				self->origin, self->mins, self->maxs,
				self->impulseVelocity, btStepTime(), self);
			BT_CheckFireBoxTrigger(self);

			f=v3ddist(self->origin, org0);
			self->moveDist+=f;

			if(self->flags&BT_FL_ONGROUND)
			{
//				printf("Player Onground %p, %08X\n", self, self->flags);
				if(ofl&BT_FL_ONGROUND)
				{
					f=v3ddist(self->origin, org0);
					self->walkDist+=f;
					self->stepDist+=f;
				
					if(self->stepDist>128)
					{
						s="sound/player/step1";
						btSound(self, BT_CHAN_BODY, s, 1.0, BT_ATTN_NORM);
						self->stepDist-=128;
					}
				}
			}
		}

	}

	if(self->impulseFlags)
	{
		BT_PlayerImpulse(self, self->impulseFlags);
		self->impulseFlags=0;
	}

	if(!BT_QuadFinishedP(self) && BT_QuadFinishedInP(self, 0.15))
	{
		btSound(self, BT_CHAN_ITEM, "sound/items/damage2",
			1.0, BT_ATTN_NORM);
	}

	if(!BT_EnviroFinishedP(self) && BT_EnviroFinishedInP(self, 0.15))
	{
//		btSound(self, BT_CHAN_ITEM, "sound/items/damage2",
//			1.0, BT_ATTN_NORM);
	}

	i=btPointContents(v3dadd(self->origin, v3f2d(self->vieworg)), self);
	if(!(i&BT_CONTENTS_FLUID) || !BT_EnviroFinishedP(self))
	{
		BT_AirFinished(self, 10);
		self->flags2&=~BT_FL2_STOPPORTAL;
	}

	if(BT_PainFinishedP(self) && BT_EnviroFinishedP(self))
	{
		i=btPointBoxContents(self->origin,
			self->mins, self->maxs, self);
#if 0
		if((i&BT_CONTENTS_FLUID_MASK)==BT_CONTENTS_FLUID_PORTAL)
		{
			BT_PainFinished(self, 1.0);
//			BT_Damage(self, NULL, 10, BT_MOD_SLIME);
			BTSV_Voxel_CheckFireBBoxPortal(self, self->mins, self->maxs);
		}else
#endif
		if(i&BT_CONTENTS_LAVA)
		{
			BT_PainFinished(self, 0.25);
			BT_Damage(self, NULL, 10, BT_MOD_LAVA);
		}else
		if(i&BT_CONTENTS_SLIME)
		{
			BT_PainFinished(self, 1.0);
			BT_Damage(self, NULL, 10, BT_MOD_SLIME);
		}else
		if((i&BT_CONTENTS_FLUID) && BT_AirFinishedP(self))
		{
			BT_PainFinished(self, 1.0);
			BT_Damage(self, NULL, 10, BT_MOD_WATER);
		}

	}


	dt=btStepTime();

	if(self->maxHealth>0)
	{
		if(self->health<self->maxHealth)
		{
//			self->health+=dt*5;

			f=dt*2;
			if(BT_PainFinishedTimeP(self, 5))f*=4;
			self->health+=f;
		}
		if(self->health>(2*self->maxHealth))
			self->health-=dt*5;
	}

	if(self->maxMP>0)
	{
		if(self->curMP<self->maxMP)
		{
//			self->curMP+=dt;

//			f=dt*5;
			f=dt*2;
			if(BT_AttackFinishedTimeP(self, 15))f*=4;
			if(self->flags&BT_FL_FASTMP)f*=8;
			self->curMP+=f;
		}
//		if(self->curMP>(2*self->maxMP))
		if(self->curMP>(8*self->maxMP))
			self->curMP-=0.5*dt;
	}
}

void BT_ClientKilled(btEntity self, float dmg, int mod)
{
	static char *snds[]={
		"sound/player/death1",
		"sound/player/death2",
		"sound/player/death3",
		"sound/player/death4",
		"sound/player/death5"};
	char *s;

//	self->solidtype=BT_SOLID_NOT;
//	self->movetype=BT_MOVE_TOSS;
//	self->velocity=vec3(0,0,0);
	
	if(self->health>(-1))
		self->health=-1;

	self->effects2|=BT_EF2_ISDEAD;
	
	btSetBBox(self, vec3(-8,-8,-8), vec3(8,8,8));
	btSetOrigin(self, v3dadd(self->origin, v3f2d(self->vieworg)));
	self->vieworg=vec3(0,0,0);

	s="sound/misc/udeath";
	s=snds[rand()%5];
	
	if(mod==BT_MOD_WATER)
		s="sound/player/h2odeath";
	
	btParticleBloodExplosion(self->origin);
	btSound(self, BT_CHAN_VOICE, s,
		1.0, BT_ATTN_NORM);

	btSetModel(self, NULL);
	btSetViewModel(self, NULL);

	btViewFlags(self, self->effects2);

	BT_AttackFinished(self, 5);
}

void BT_ClientPain(btEntity self, float dmg, int mod)
{
	static char *snds[]={
		"sound/player/pain1",
		"sound/player/pain2",
		"sound/player/pain3",
		"sound/player/pain4",
		"sound/player/pain5",
		"sound/player/pain6"};
	static char *snds_drown[]={
		"sound/player/drown1",
		"sound/player/drown2"};
	char *s;

	if(!BT_InvulFinishedP(self))
	{
		s="sound/player/nopain1";
		btSound(self, BT_CHAN_VOICE, s, 1.0, BT_ATTN_NORM);
		return;
	}

	//make sure we have at least "some" pain frames
	if(BT_PainFinishedP(self))
		BT_PainFinished(self, 0.1);

	s=snds[rand()%5];
	if(mod==BT_MOD_WATER)
		s=snds_drown[rand()%2];

//	if(mod==BT_MOD_LAVA)
//		s="sound/player/pain6";

	if(mod==BT_MOD_IMPACT)
		s=NULL;

	btSound(self, BT_CHAN_VOICE, s, 1.0, BT_ATTN_NORM);

	if(mod==BT_MOD_WATER)
		btViewFlash(self, BT_RGB_BLUE_MID, 0.25, 1);

	if(mod!=BT_MOD_WATER)
		btViewFlash(self, BT_RGB_RED_MID, 0.25, 3);
}

void BT_ClientImpact(btEntity self, vec3 vel)
{
	static char *snds_land[]={
		"sound/player/land",
		"sound/player/land1"};

	float speed, dmg;
	char *s;

	s=snds_land[rand()%2];
	
//	speed=v3len(vel);
	speed=-v3z(vel);
	dmg=speed/100;

	if(dmg<4)return;

	if(dmg<7)
	{
		btSound(self, BT_CHAN_VOICE, s, 1.0, BT_ATTN_NORM);
		return;
	}
	
	s="sound/player/land2";
	btSound(self, BT_CHAN_VOICE, s, 1.0, BT_ATTN_NORM);

	BT_Damage(self, NULL, dmg, BT_MOD_IMPACT);
}

void BT_ClientRespawn(btEntity self)
{
	char *s;

	if(btGameIsDeathmatch() || btGameIsCoop())
	{
		btRespawnPlayer(self);
		return;
	}

	s=btCvarGet("mapname");
	btConsoleCmd("map %s", s);
}

/** Client has just connected */
void BT_ClientConnected(btEntity self)
{
	btClientEvalScript(self, "game_sv/ClientStuff.bs");
}
