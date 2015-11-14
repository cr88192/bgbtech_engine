#include <btgesv.h>

BTGE_BrushWorld *btge_world;
BTGE_SEntity *btge_client_spawnent;
BTGE_ClientInfo *btge_client;	//client list

float btge_time;		//world time
float btge_steptime;	//world step time
float btge_framedt;		//frame delta time
// btEntity btge_client;	//player

dyt btge_entity_GetSlot(dyt obj, dyt key)
{
	return(dyllGetStructSlotDynamic(
		obj, "BTGE_Entity_s", dysymbolv(key)));
}

dyt btge_entity_SetSlot(dyt obj, dyt key, dyt val)
{
	return(dyllSetStructSlotDynamic(
		obj, "BTGE_Entity_s", dysymbolv(key), val));
}

void BTGE_Entity_Init()
{
	static int init=0;

	if(init)return;
	init=1;

	dyTypeGetSlot("btge_entity_t", btge_entity_GetSlot);
	dyTypeSetSlot("btge_entity_t", btge_entity_SetSlot);

	btge_client_spawnent=NULL;

	btConsoleCmd("exec game/server.cfg");
	BTGE_DoExecCmds();
}

BTSV_API int BTGE_WorldTeardown(BTGE_BrushWorld *world)
{
	BTGE_BrushWorld *wrl0;
	int i;
	
	BTGE_BrushPhys_UnlinkWorld(world);
	BTGE_EntBSP_DestroyEntityBSP(world);
	
	wrl0=btge_world;
	btge_world=world;
	for(i=1; i<world->n_ents; i++)
		btFreeEntity(world->ents[i]);
	gcfree(world->ents);
	world->ents=NULL;
	world->n_ents=0;
	world->m_ents=0;
	btge_world=wrl0;
}

BTSV_API void BTSV_LoadMapFile(BTGE_BrushWorld *wrl, char *map)
{
	BTGE_Brush *bcur;
	BTGE_SEntity *scur;
	BTGE_SEntPair *kcur;

//	if(wrl->mapname && !strcmp(wrl->mapname, map))
//		return;

//	wrl->mapname=dystrdup(map);
	BTGE_BrushMap_LoadFile(wrl, map);

#if 0
	//just loaded map, avoid client pushing a bunch of deltas.

	bcur=wrl->brush;
	while(bcur)
	{
		bcur->flags&=~BTGE_BRFL_DIRTY;
		bcur=bcur->next;
	}

	scur=wrl->entity;
	while(scur)
	{
		kcur=scur->keys;
		while(kcur)
		{
			kcur->flags&=~BTGE_SEFL_DIRTY;
			kcur=kcur->next;
		}
		scur->flags&=~BTGE_SEFL_DIRTY;
		scur=scur->next;
	}
#endif

#if 1
	//alternate logic, force sending all deltas.

	bcur=wrl->brush;
	while(bcur)
	{
		bcur->flags|=BTGE_BRFL_DIRTY|BTGE_BRFL_NEW;
		bcur=bcur->next;
	}

#if 1
	scur=wrl->entity;
	while(scur)
	{
		kcur=scur->keys;
		while(kcur)
		{
			kcur->flags|=BTGE_SEFL_DIRTY;
			kcur=kcur->next;
		}
		scur->flags|=BTGE_SEFL_DIRTY|BTGE_SEFL_NEW;
		scur=scur->next;
	}
#endif
#endif
}

BTSV_API void BTSV_LoadMap(BTGE_BrushWorld *wrl, char *map)
{
	char tb[256];

//	btCvarSet("mapname", map);

	sprintf(tb, "maps/%s.map", map);
	BTSV_LoadMapFile(wrl, tb);
}

int BT_FindFreeEntityIndex()
{
	int i;

	if(!btge_world)
		return(-1);

	for(i=1; i<btge_world->n_ents; i++)
		if(!btge_world->ents[i])
			return(i);
	
	if(!btge_world->ents)
	{
		btge_world->ents=gcalloc(16384*sizeof(btEntity));
		btge_world->m_ents=16384;
		btge_world->n_ents=1;
	}

	if((btge_world->n_ents+1)>=btge_world->m_ents)
	{
		i=btge_world->m_ents;
		i=i+(i>>1);
		btge_world->ents=gcrealloc(btge_world->ents, i*sizeof(btEntity));
	}
	
	i=btge_world->n_ents++;
	return(i);	
}

BTSV_API btEntity btAllocEntity()
{
	btEntity ent;

	BTGE_Entity_Init();

	ent=gctalloc("btge_entity_t", sizeof(BTGE_Entity));
	return(ent);
}

BTSV_API btEntity btGetEntity(int idx)
{
	if(idx<=0)return(NULL);
	if(idx>=btge_world->n_ents)
		return(NULL);
	return(btge_world->ents[idx]);
}

BTSV_API btEntity btNewEntity()
{
	btEntity ent;
	int n;
	
	ent=btAllocEntity();
	n=BT_FindFreeEntityIndex();
	ent->entnum=n;
	btge_world->ents[n]=ent;
	return(ent);
}

BTSV_API void btFreeEntity(btEntity ent)
{
	if(!ent)return;

//	if(ent->eTrig)gcfree(ent);
//	if(ent->eAct)gcfree(ent);

	if(ent->vt)gcfree(ent->vt);
	if(ent->avt)gcfree(ent->avt);

	gcfree(ent);
}

BTSV_API void btDeleteEntity(btEntity ent)
{
	dyt t;

	if(!ent)return;
	if(ent->entnum<=0)return;

	t=dylist2s("delent", dyint(ent->entnum));
	BTGE_AddUpdateQueue(t);

	if(ent->light)
	{
		ent->light->flags|=BTGE_LFL_DISABLE|BTGE_LFL_CHANGED;
		t=dylist2s("dellight", dyint(ent->light->num));
		BTGE_AddUpdateQueue(t);
	}

	BTGE_EntPhys_UnlinkEntity(ent);

	if(ent->vt && ent->vt->deinit)
		ent->vt->deinit(ent);
	btge_world->ents[ent->entnum]=NULL;
	
	btFreeEntity(ent);
}

BTSV_API bool btEntityP(btEntity ent)
{
	int i;

	if(!dytypep((dyt)ent, "btge_entity_t"))
		return(false);
	i=ent->entnum;
	if(btGetEntity(i)!=ent)
		return(false);
	return(true);
}

BTSV_API btEntity btLookupClientEntity(int clnum)
{
	btEntity cur;
	int i;

	for(i=1; i<btge_world->n_ents; i++)
	{
		cur=btge_world->ents[i];
		if(!cur)continue;
		
		if(!(cur->flags&BT_FL_CLIENT))
			continue;
		if(cur->clientnum==clnum)
			return(cur);
	}
	return(NULL);
}

BTSV_API float btTime()
	{ return(btge_time); }

BTSV_API float btDelayTime(float amt)
{
	if(amt<0)return(amt);
	return(btge_time+amt);
}

BTSV_API btEntity btGetCurrentClient(void)
{
	btEntity cur, lst;
	int i, n;

	lst=btGetCurrentClientList();

	cur=lst; n=0;
	while(cur) { n++; cur=cur->chain; }

	i=(n>0)?(rand()%n):0;
	cur=lst;
	while(cur && cur->chain && (i>0))
		{ i--; cur=cur->chain; }
	return(cur);

//	return(btge_client);
}

BTSV_API btEntity btGetCurrentClientList(void)
{
	btEntity cur, lst;
	int i;

	lst=NULL;
	for(i=1; i<btge_world->n_ents; i++)
	{
		cur=btge_world->ents[i];
		if(!cur)continue;
		
		if(!(cur->flags&BT_FL_CLIENT))
			continue;
		cur->chain=lst;
		lst=cur;
	}
	return(lst);
}

BTSV_API void btSound(btEntity self, int chan,
	char *sound, float vol, int attn)
{
	dyt t;
	
//	if(!sound || !(*sound))
	if(!sound)
		return;

	if(!(*sound))
	{
		if(!(chan&7))
			return;
	}
	
	t=dylist6s("sound",
		dyint(self->entnum), dyint(chan), dystring(sound),
		dyflonum(vol), dyint(attn));
	BTGE_AddUpdateQueue(t);
}

BTSV_API void btSpeak2(btEntity self, int chan,
	char *voice, char *ctrl, char *text, float vol, int attn)
{
	dyt t;
	
	if(!text || !(*text))
		return;
	
	t=dylist8s("speak",
		dyint(self->entnum), dyint(chan),
		dystring(voice), dystring(ctrl), dystring(text),
		dyflonum(vol), dyint(attn));
	BTGE_AddUpdateQueue(t);
}

BTSV_API void btSpeak(btEntity self, int chan,
	char *text, float vol, int attn)
{
	if(!text || !(*text))
		return;

	btSpeak2(self, chan, self->ttsVoice, self->ttsControl, text, vol, attn);
}

BTSV_API void btParticles(char *ty, int cnt, int clr,
	vec3 org, vec3 vel, vec3 svel)
{
	BTGE_ClientInfo *clcur;
	vec3 ro;
	dyt t;
	
#if 0
	t=dylist7s("particles",
		dylist2s("ty", dystring(ty)),
		dylist2s("cnt", dyint(cnt)),
		dylist2s("clr", dyint(clr)),
		BTGE_DeltaNamedVec3("org", org),
		BTGE_DeltaNamedVec3("vel", vel),
		BTGE_DeltaNamedVec3("svel", svel));
	BTGE_AddUpdateQueue(t);
#endif

	clcur=btge_client;
	while(clcur)
	{
		ro=vec3(
			clcur->reforg[0],
			clcur->reforg[1],
			clcur->reforg[2]);
	
		t=dylist7s("particles",
			dylist2s("ty", dystring(ty)),
			dylist2s("cnt", dyint(cnt)),
			dylist2s("clr", dyint(clr)),
			BTGE_DeltaNamedVec3("org", v3sub(org, ro)),
			BTGE_DeltaNamedVec3("vel", vel),
			BTGE_DeltaNamedVec3("svel", svel));

		BTGE_AddUpdateEntityQueue(clcur->entity, t);

		clcur=clcur->next;
	}

//	BTGE_AddUpdateEntityQueue(targ, t);
}

BTSV_API void btDPrint(char *str, ...)
{
	char tb[256];
	va_list lst;
	dyt t;
	
	va_start(lst, str);
	vsprintf(tb, str, lst);
	gc_puts(tb);
	t=dylist2s("dprint", dystring(tb));
	BTGE_AddUpdateQueue(t);
	va_end(lst);
}

BTSV_API void btCenterprint(btEntity targ, char *str, ...)
{
	char tb[256];
	va_list lst;
	dyt t;
	
	va_start(lst, str);
	vsprintf(tb, str, lst);
//	gc_puts(tb);
	va_end(lst);
	t=dylist2s("centerprint", dystring(tb));
//	BTGE_AddUpdateQueue(t);
	BTGE_AddUpdateEntityQueue(targ, t);
}

BTSV_API void btCenterprintBox(btEntity targ, char *str, ...)
{
	char tb[256];
	va_list lst;
	dyt t;
	
	va_start(lst, str);
	vsprintf(tb, str, lst);
//	gc_puts(tb);
	va_end(lst);
	t=dylist2s("centerprint_box", dystring(tb));
//	BTGE_AddUpdateQueue(t);
	BTGE_AddUpdateEntityQueue(targ, t);
}

BTSV_API void btCenterprintBoxName(btEntity targ, char *str, ...)
{
	char tb[256];
	va_list lst;
	dyt t;
	
	va_start(lst, str);
	vsprintf(tb, str, lst);
	va_end(lst);
	t=dylist2s("centerprint_boxname", dystring(tb));
	BTGE_AddUpdateEntityQueue(targ, t);
}

BTSV_API void btCenterprintBoxImage(btEntity targ, char *str, ...)
{
	char tb[256];
	va_list lst;
	dyt t;
	
	va_start(lst, str);
	vsprintf(tb, str, lst);
	va_end(lst);
	t=dylist2s("centerprint_boximg", dystring(tb));
	BTGE_AddUpdateEntityQueue(targ, t);
}

BTSV_API void btPrint(btEntity targ, char *str, ...)
{
	char tb[256];
	va_list lst;
	dyt t;
	
	va_start(lst, str);
	vsprintf(tb, str, lst);
	gc_puts(tb);
	va_end(lst);
	t=dylist2s("print", dystring(tb));
//	BTGE_AddUpdateQueue(t);
	BTGE_AddUpdateEntityQueue(targ, t);
}

BTSV_API void btViewFlags(btEntity targ, int flags)
{
	dyt t;	
	t=dylist2s("viewflags", dyint(flags));
//	BTGE_AddUpdateQueue(t);
	BTGE_AddUpdateEntityQueue(targ, t);
}

BTSV_API void btViewFlash(btEntity targ,
	int rgb, float time, float scale)
{
	dyt t;	
	t=dylist4s("viewflash", dyint(rgb),
		dyflonum(time), dyflonum(scale));
//	BTGE_AddUpdateQueue(t);
	BTGE_AddUpdateEntityQueue(targ, t);
}

BTSV_API void btViewMins(btEntity targ, vec3 v)
{
	dyt t;	

	t=dylist4s("view_mins",
		dyflonum(v3x(v)),
		dyflonum(v3y(v)),
		dyflonum(v3z(v)));
//	BTGE_AddUpdateQueue(t);
	BTGE_AddUpdateEntityQueue(targ, t);
}

BTSV_API void btViewMaxs(btEntity targ, vec3 v)
{
	dyt t;	

	t=dylist4s("view_maxs",
		dyflonum(v3x(v)),
		dyflonum(v3y(v)),
		dyflonum(v3z(v)));
//	BTGE_AddUpdateQueue(t);
	BTGE_AddUpdateEntityQueue(targ, t);
}

BTSV_API void btViewFOV(btEntity targ, int fov)
{
	dyt t;	
	t=dylist2s("view_fov", dyint(fov));
//	BTGE_AddUpdateQueue(t);
	BTGE_AddUpdateEntityQueue(targ, t);
}

BTSV_API void btViewMinMax(btEntity targ, vec3 mins, vec3 maxs)
{
	btViewMins(targ, mins);
	btViewMaxs(targ, maxs);
}

/** Send console command to a given client */
BTSV_API void btStuffCmd(btEntity targ, char *str, ...)
{
	char tb[256];
	va_list lst;
	dyt t;

	va_start(lst, str);
	vsprintf(tb, str, lst);
	va_end(lst);
	t=dylist2s("stuffcmd", dystring(tb));
//	BTGE_AddUpdateQueue(t);
	BTGE_AddUpdateEntityQueue(targ, t);
}

/** Send console commend to everyone */
BTSV_API void btBroadcastCmd(char *str, ...)
{
	char tb[256];
	va_list lst;
	dyt t;

	va_start(lst, str);
	vsprintf(tb, str, lst);
	va_end(lst);
	t=dylist2s("stuffcmd", dystring(tb));
	BTGE_AddUpdateQueue(t);
//	BTGE_ConsoleCmd(tb);
}

/** Execute console command on server */
BTSV_API void btConsoleCmd(char *str, ...)
{
	char tb[256];
	va_list lst;
	dyt t;

	va_start(lst, str);
	vsprintf(tb, str, lst);
	va_end(lst);
//	t=dylist2s("stuffcmd", dystring(tb));
//	BTGE_AddUpdateQueue(t);
	BTGE_ConsoleCmd(tb);
}

BTSV_API void btSetBGM(btEntity targ, char *name)
{
	dyt t;
	
	t=dylist2s("setbgm", dystring(name));
//	BTGE_AddUpdateQueue(t);
	BTGE_AddUpdateEntityQueue(targ, t);
}

BTSV_API void btSetClientCamera(btEntity client, btEntity targ)
{
	dyt t;
	
//	t=dylist2s("camera", dyint(targ->entnum));
//	BTGE_AddUpdateQueue(t);
}

BTSV_API void btSetScale(btEntity ent, float sc)
{
	int i, j, k;
	
	if((sc<=0) || (sc==1.0))
		{ ent->effects2&=~BT_EF2_SCALE_MASK; return; }
//	if(sc>256)
//		{ ent->effects2|=BT_EF2_SCALE_MASK; return; }

	ent->effects2&=~BT_EF2_SCALE_MASK;
	i=(log(sc)/log(2))+7;
	if(i<0)
		{ ent->effects2|=(1<<BT_EF2_SCALE_LSHIFT); return; }
	if(i>15)
		{ ent->effects2|=BT_EF2_SCALE_MASK; return; }
	j=(sc-pow(2, i-7))*16;
	k=(i<<4)|(j&15);
	ent->effects2|=(k<<BT_EF2_SCALE_LSHIFT);
}

BTSV_API int btEntityCheckLine(btEntity ent, vec3 v0, vec3 v1)
{
	float am[3], an[3];
	float bm[3], bn[3];
	BTGE_Brush *lst;
	int i;
	
	vfvec3(am, ent->absmin);
	vfvec3(an, ent->absmax);
	vfvec3(bm, v0);
	vfvec3(bn, v1);
	
	i=SolidAABB_BoxLineCollideP(am, an, bm, bn);
	if(!i)return(0);
	
	if(btGetBModel(ent))
	{
		vfvec3(bm, v3sub(v0, ent->origin));
		vfvec3(bn, v3sub(v1, ent->origin));

//		lst=BTGE_BrushBSP_BoxQueryTreeNode(btGetBModel(ent)->bsp, bm, bn);
		lst=BTGE_BrushBSP_Traceline(btGetBModel(ent)->bsp, NULL, bm, bn);
		if(!lst)return(0);

		i=btBrushListContents(lst);
		if(!(i&BT_CONTENTS_CHECKSOLID))
			return(0);
		return(1);
	}
	
	return(1);
}

BTSV_API int btEntityLineContents(btEntity ent, vec3 v0, vec3 v1)
{
	float am[3], an[3];
	float bm[3], bn[3];
	BTGE_Brush *lst;
	int i;
	
	vfvec3(am, ent->absmin);
	vfvec3(an, ent->absmax);
	vfvec3(bm, v0);
	vfvec3(bn, v1);
	
	i=SolidAABB_BoxLineCollideP(am, an, bm, bn);
	if(!i)return(0);
	
	if(btGetBModel(ent))
	{
		vfvec3(bm, v3sub(v0, ent->origin));
		vfvec3(bn, v3sub(v1, ent->origin));

//		lst=BTGE_BrushBSP_BoxQueryTreeNode(btGetBModel(ent)->bsp, bm, bn);
		lst=BTGE_BrushBSP_Traceline(btGetBModel(ent)->bsp, NULL, bm, bn);
		if(!lst)return(0);

		i=btBrushListContents(lst);
		return(i);
	}
	
	return(BT_CONTENTS_ORIGIN|BT_CONTENTS_SOLID);
}

BTSV_API int btEntityCheckBox(btEntity ent, vec3 mins, vec3 maxs)
{
	float am[3], an[3];
	float bm[3], bn[3];
	BTGE_Brush *lst;
//	btEntity ents;
	int i;
	
	if(!ent)return(0);
	
	vfvec3(am, ent->absmin);
	vfvec3(an, ent->absmax);
	vfvec3(bm, mins);
	vfvec3(bn, maxs);
	
	i=SolidAABB_BoxCollideP(am, an, bm, bn);
	if(!i)return(0);
	
	if(btGetBModel(ent))
	{
		vfvec3(bm, v3sub(mins, ent->origin));
		vfvec3(bn, v3sub(maxs, ent->origin));
		lst=BTGE_BrushBSP_BoxQueryTreeNode(btGetBModel(ent)->bsp, bm, bn);
		if(!lst)return(0);

		i=btBrushListContents(lst);
		if(!(i&BT_CONTENTS_CHECKSOLID))
			return(0);
		return(1);
	}
	
	return(1);
}

BTSV_API int btEntityBoxContents(btEntity ent, vec3 mins, vec3 maxs)
{
	float am[3], an[3];
	float bm[3], bn[3];
	BTGE_Brush *lst;
//	btEntity ents;
	int i;
	
	if(!ent)return(0);
	
	vfvec3(am, ent->absmin);
	vfvec3(an, ent->absmax);
	vfvec3(bm, mins);
	vfvec3(bn, maxs);
	
	i=SolidAABB_BoxCollideP(am, an, bm, bn);
	if(!i)return(0);
	
	if(btGetBModel(ent))
	{
		vfvec3(bm, v3sub(mins, ent->origin));
		vfvec3(bn, v3sub(maxs, ent->origin));
		lst=BTGE_BrushBSP_BoxQueryTreeNode(btGetBModel(ent)->bsp, bm, bn);
		if(!lst)return(0);
		
		i=btBrushListContents(lst);
		return(i);
	}
	
	return(BT_CONTENTS_ORIGIN|BT_CONTENTS_SOLID);
}

BTSV_API int btEntityListBoxContents(btEntity lst, vec3 amins, vec3 amaxs)
{
	btEntity cur;
	int cont;
	
	if(!lst)return(0);
	
	cur=lst; cont=0;
	while(cur)
	{
		cont|=btEntityBoxContents(cur, amins, amaxs);
		cur=cur->chain;
	}
	
	return(cont);
}

BTSV_API float btEntityDistance(btEntity e0, btEntity e1)
{
	vec3 v0, v1;
	
	v0=btCalcCenter(e0);
	v1=btCalcCenter(e1);
	return(v3len(v3sub(v0, v1)));
}

BTSV_API float btSmallDistancePointBox(vec3 point, vec3 mins, vec3 maxs)
{
	float tvp[3], tvm[3], tvn[3], tn[4];
	vec3 org, dir;
	float f, g;

	org=v3scale(v3add(mins, maxs), 0.5);
	dir=v3norm(v3sub(org, point));

	vfvec3(tvm, mins);
	vfvec3(tvn, maxs);
	vfvec3(tvp, point);

	vfvec3(tn, dir);
	tn[3]=V3F_DOT(tvp, tn);
	
	Hull_BoxPlaneExtents(tvm, tvn, tn, &f, &g);
	return(f);
}

BTSV_API btEntity btFindRadius(vec3 origin, float radius)
{
	btEntity cur, lst;
	vec3 org;
	float rad, dist;
	int i;
	
	lst=NULL;
	for(i=1; i<btge_world->n_ents; i++)
	{
		cur=btge_world->ents[i];
		if(!cur)continue;
		org=btCalcCenter(cur);
		rad=btCalcRadius(cur);
		dist=v3len(v3sub(org, origin));
		if(dist>=(rad+radius))
			continue;
		cur->chain=lst;
		lst=cur;
	}
	
	return(lst);
}

BTSV_API btEntity btFindBox(vec3 mins, vec3 maxs)
{
	btEntity cur, lst;
	int i, j;

	lst=NULL;
	for(i=1; i<btge_world->n_ents; i++)
	{
		cur=btge_world->ents[i];
		if(!cur)continue;

		j=btEntityCheckBox(cur, mins, maxs);
		if(j)
		{
			cur->chain=lst;
			lst=cur;
		}
	}
	return(lst);
}

BTSV_API bool btGameIsSingle()
{
	if(btGameIsDeathmatch() || btGameIsCoop())
		return(false);
	return(true);
}

BTSV_API bool btGameIsCoop()
{
	int i;
	i=btCvarGetf("coop");
	if(i)return(true);
	return(false);
}

BTSV_API bool btGameIsDeathmatch()
{
	int i;
	i=btCvarGetf("deathmatch");
	if(i)return(true);
	return(false);
}

BTSV_API bool btGameIsNoMonsters()
{
	int i;
	i=btCvarGetf("sv_nomonsters");
	if(i)return(true);
	return(false);
}

BTSV_API btEntity btFindSpawnpoint()
{
	btEntity cur, lst;
	vec3 org;
	float rad, dist;
	int i, n;

	//check for restoring from savegame
	lst=NULL;
	for(i=1; i<btge_world->n_ents; i++)
	{
		cur=btge_world->ents[i];
		if(!cur)continue;
		if(!cur->classname)continue;
		
		if(!strcmp(cur->classname, "info_player_saved"))
			{ cur->chain=lst; lst=cur; continue; }
	}

	if(lst)
	{
		//saved points override other spawnpoints
		btge_client_spawnent=lst->sent;
		return(lst);
	}
	
	//check for qualifying spawn locations...
	lst=NULL;
	for(i=1; i<btge_world->n_ents; i++)
	{
		cur=btge_world->ents[i];
		if(!cur)continue;
		if(!cur->classname)continue;

		if(btGameIsSingle())
		{
			if(!strcmp(cur->classname, "info_player_start"))
				{ cur->chain=lst; lst=cur; continue; }
			if(!strcmp(cur->classname, "info_player_start2d"))
				{ cur->chain=lst; lst=cur; continue; }
			if(!strcmp(cur->classname, "info_player_fixed"))
				{ cur->chain=lst; lst=cur; continue; }
			if(!strcmp(cur->classname, "info_player_fixed2d"))
				{ cur->chain=lst; lst=cur; continue; }
		}

		if(btGameIsCoop() &&
			!strcmp(cur->classname, "info_player_coop"))
				{ cur->chain=lst; lst=cur; continue; }
		if(btGameIsDeathmatch() &&
			!strcmp(cur->classname, "info_player_deathmatch"))
				{ cur->chain=lst; lst=cur; continue; }

		if(btGameIsDeathmatch() &&
			!strcmp(cur->classname, "info_player_team1"))
				{ cur->chain=lst; lst=cur; continue; }
		if(btGameIsDeathmatch() &&
			!strcmp(cur->classname, "info_player_team2"))
				{ cur->chain=lst; lst=cur; continue; }
	}
	
	if(!lst)
	{
		//couldn't find any good ones, look for any spawn locations
		for(i=1; i<btge_world->n_ents; i++)
		{
			cur=btge_world->ents[i];
			if(!cur)continue;
			if(!cur->classname)continue;

			if(!strcmp(cur->classname, "info_player_start"))
				{ cur->chain=lst; lst=cur; continue; }
			if(!strcmp(cur->classname, "info_player_start2d"))
				{ cur->chain=lst; lst=cur; continue; }
			if(!strcmp(cur->classname, "info_player_fixed"))
				{ cur->chain=lst; lst=cur; continue; }
			if(!strcmp(cur->classname, "info_player_fixed2d"))
				{ cur->chain=lst; lst=cur; continue; }

			if(!strcmp(cur->classname, "info_player_coop"))
				{ cur->chain=lst; lst=cur; continue; }
			if(!strcmp(cur->classname, "info_player_deathmatch"))
				{ cur->chain=lst; lst=cur; continue; }
			if(!strcmp(cur->classname, "info_player_team1"))
				{ cur->chain=lst; lst=cur; continue; }
			if(!strcmp(cur->classname, "info_player_team2"))
				{ cur->chain=lst; lst=cur; continue; }
		}
	}
	
	if(!lst)return(NULL);
	
	cur=lst; n=0;
	while(cur) { n++; cur=cur->chain; }
	
	printf("btFindSpawnpoint: %d spawnpoints\n", n);
	
	i=btRandom()*n;
	cur=lst;
	while(cur->chain && i) { i--; cur=cur->chain; }
	
	return(cur);
}

BTSV_API void btEntUse(btEntity self, btEntity other)
{
	if(self->dy_this)
		dyCall2(self->dy_this, "use", (dyt)self, (dyt)other);

	if(self->f_use)
		self->f_use(self, other);
	if(self->vt && self->vt->use)
		self->vt->use(self, other);
}

BTSV_API bool btEntTouch(btEntity self, btEntity other)
{
	if(self->dy_this)
		dyCall2(self->dy_this, "touch", (dyt)self, (dyt)other);

	if(self->vt && self->vt->touch)
	{
		self->vt->touch(self, other);
		return(true);
	}
	return(false);
}

BTSV_API bool btEntBlocked(btEntity self, btEntity other)
{
	if(self->dy_this)
		dyCall2(self->dy_this, "blocked", (dyt)self, (dyt)other);

	if(self->vt && self->vt->blocked)
	{
		self->vt->blocked(self, other);
		return(true);
	}
	return(false);
}

BTSV_API bool btEntPain(btEntity self, btEntity other, float damage)
{
	if(self->dy_this)
		dyCall3(self->dy_this, "pain",
			(dyt)self, (dyt)other, dyfloat(damage));

	if(self->vt && self->vt->pain)
	{
		self->vt->pain(self, other, damage);
		return(true);
	}
	return(false);
}

BTSV_API bool btEntDie(btEntity self, btEntity other, float damage)
{
	if(self->dy_this)
		dyCall3(self->dy_this, "die",
			(dyt)self, (dyt)other, dyfloat(damage));

	if(self->vt && self->vt->die)
	{
		self->vt->die(self, other, damage);
		return(true);
	}
	return(false);
}

BTSV_API bool btEntStand(btEntity self)
{
	if(self->dy_this)
		dyCall1(self->dy_this, "stand", (dyt)self);

	if(self->avt && self->avt->stand)
	{
		self->avt->stand(self);
		return(true);
	}
	return(false);
}

BTSV_API bool btEntWalk(btEntity self)
{
	if(self->dy_this)
		dyCall1(self->dy_this, "walk", (dyt)self);

//	*(int *)-1;

	if(self->avt && self->avt->walk)
	{
		self->avt->walk(self);
		return(true);
	}
	return(false);
}

BTSV_API bool btEntRun(btEntity self)
{
	if(self->dy_this)
		dyCall1(self->dy_this, "run", (dyt)self);

	if(self->avt && self->avt->run)
	{
		self->avt->run(self);
		return(true);
	}
	return(false);
}

BTSV_API bool btEntMissile(btEntity self)
{
	if(self->dy_this)
		dyCall1(self->dy_this, "missile", (dyt)self);

	if(self->avt && self->avt->missile)
	{
		self->avt->missile(self);
		return(true);
	}
	return(false);
}

BTSV_API bool btEntMelee(btEntity self)
{
	if(self->dy_this)
		dyCall1(self->dy_this, "melee", (dyt)self);

	if(self->avt && self->avt->melee)
	{
		self->avt->melee(self);
		return(true);
	}
	return(false);
}

BTSV_API bool btEntHasMissileP(btEntity self)
{
	dyt p;
	if(self->dy_this)
	{
		p=dyGet(self->dy_this, "missile");
		if(p && (p!=UNDEFINED))
			return(true);
	}

	if(self->avt && self->avt->missile)
		return(true);
	return(false);
}

BTSV_API bool btEntHasMeleeP(btEntity self)
{
	dyt p;
	if(self->dy_this)
	{
		p=dyGet(self->dy_this, "melee");
		if(p && (p!=UNDEFINED))
			return(true);
	}

	if(self->avt && self->avt->melee)
		return(true);
	return(false);
}

BTSV_API bool btEntPreThink(btEntity self)
{
	if(self->dy_this)
		dyCall1(self->dy_this, "prethink", (dyt)self);

	if(self->vt && self->vt->prethink)
	{
		self->vt->prethink(self);
		return(true);
	}
	return(false);
}


//void btEntMelee(btEntity self)
//	{ if(self->avt->melee)self->avt->melee(self); }

BTSV_API void btSetThink(btEntity self, btge_think fcn, float dt)
{
	self->f_think=fcn;
	self->dy_think=NULL;
	self->nextthink=btDelayTime(dt);
}

BTSV_API void btSetDyThink(btEntity self, dyt fcn, float dt)
{
	self->dy_think=fcn;
	self->f_think=NULL;
	self->nextthink=btDelayTime(dt);
}

BTSV_API btEntity btFindTargets(char *targetname)
{
	btEntity cur, lst;
	int i;
	
	if(!targetname)
		return(NULL);
	
	lst=NULL;
	for(i=1; i<btge_world->n_ents; i++)
	{
		cur=btge_world->ents[i];
		if(!cur)continue;
		if(!cur->targetname)
			continue;
		if(strcmp(cur->targetname, targetname))
			continue;
		cur->chain=lst;
		lst=cur;
	}
	
	return(lst);
}

BTSV_API btEntity btFindClients()
{
	btEntity cur, lst;
	int i;
	
	lst=NULL;
	for(i=1; i<btge_world->n_ents; i++)
	{
		cur=btge_world->ents[i];
		if(!cur)continue;
		if(!(cur->flags&BT_FL_CLIENT))
			continue;
		cur->chain=lst;
		lst=cur;
	}
	
	return(lst);
}

BTSV_API btEntity btFindClassname(char *classname)
{
	btEntity cur, lst;
	int i;
	
	if(!classname)
		return(NULL);
	
	lst=NULL;
	for(i=1; i<btge_world->n_ents; i++)
	{
		cur=btge_world->ents[i];
		if(!cur)continue;
		if(!cur->classname)
			continue;
		if(strcmp(cur->classname, classname))
			continue;
		cur->chain=lst;
		lst=cur;
	}
	
	return(lst);
}

void BT_SetMovedir(btEntity self)
{
	float f, g;

	f=btCurrentYaw(self);
	g=btCurrentPitch(self);
	if(f==-1)
		{ self->movedir=vec3(0,0,1); }
	else if(f==-2)
		{ self->movedir=vec3(0,0,-1); }
	else
	{
		f=f*(M_PI/180);
		g=g*(M_PI/180);
		self->movedir=vec3(cos(f)*cos(g), sin(f)*cos(g), sin(g));
	}
}

void BT_InitTrigger(btEntity self)
{
	self->solidtype=BT_SOLID_TRIGGER;
	self->movetype=BT_MOVE_NONE;
	btSetModel(self, self->model);
}

BTSV_API void BT_CalcMove(btEntity self,
	vec3 dest, float speed, btge_think fcn)
{
	vec3 dir;
	float len, dt, dt1, hz;

	if(speed<=0)return;
	
	self->f_think1=fcn;
	self->f_think=BT_CalcMoveDone;
	self->finalDest=dest;
	self->moveSpeed=speed;
	
	if(v3dist(self->origin, dest)<1)
	{
		self->velocity=vec3(0,0,0);
		self->nextthink=btDelayTime(0.1);
		return;
	}
	
	hz=1.0/btge_framedt;
	
	dir=v3sub(dest, self->origin);
	len=v3len(dir);
	dt=len/speed;
//	dt=floor(dt*10)/10;
	//pad up to avoid overshoot
//	dt1=ceil(dt*10)/10;
	dt1=ceil(dt*hz)/hz;
	
	//HACK: overcompensate slightly for platforms
	if(!strcmp(self->classname, "func_plat"))
	{
//		dt1=floor(dt*10)/10;
		dt1=floor(dt*hz)/hz;
//		dt1-=0.2;
	}
	
	if(dt<0.1)
	{
		self->velocity=vec3(0,0,0);
		self->nextthink=btDelayTime(0.1);
		return;
	}
	
	self->nextthink=btDelayTime(dt);
	self->velocity=v3scale(dir, 1.0/dt1);
//	self->velocity=v3scale(dir, speed);
}

BTSV_API void BT_CalcMoveDy(btEntity self,
	vec3 dest, float speed, dyt fcn)
{
	self->dy_think1=fcn;
	BT_CalcMove(self, dest, speed, NULL);
}

void BT_CalcMoveDone(btEntity self)
{
	btSetOrigin(self, self->finalDest);
	self->velocity=vec3(0,0,0);
//	self->nextthink=btDelayTime(-1);
	self->f_think=NULL;

	if(self->dy_think1)
	{
		dyApplyMethod1(self->dy_this,
			self->dy_think1, (dyt)self);
	}else if(self->f_think1)
	{
		self->f_think1(self);
	}
}

void BT_CalcMoveResume(btEntity self)
{
	BT_CalcMove(self, self->finalDest, self->moveSpeed, self->f_think1);
}

void BT_CalcMoveBlocked(btEntity self)
{
	//limit to pushmove entities
	if(self->f_think==BT_CalcMoveDone)
	{
		self->velocity=vec3(0,0,0);
		self->f_think=BT_CalcMoveResume;
		self->nextthink=btDelayTime(0.1);
	}
}

void BT_DelayThink(btEntity self)
{
	BT_UseTargets(self, self->enemy);
	btDeleteEntity(self);
}

BTSV_API void BT_DelayUseTargets(btEntity self,
	btEntity other, float delay)
{
	btEntity tmp;

	tmp=btNewEntityTriggerInit(NULL, NULL);
	tmp->classname="DelayedUse";
	tmp->f_think=BT_DelayThink;
	tmp->nextthink=btDelayTime(delay);
	tmp->enemy=other;
	tmp->message=self->message;
	tmp->target=self->target;
	tmp->killtarget=self->killtarget;
	tmp->use_eval=self->use_eval;
}

BTSV_API void BT_UseTargets(btEntity self, btEntity other)
{
	btEntity tmpArr[256];
	btEntity tmp, cur, nxt;
	int i, n;

	if(self->delay>0)
	{
		BT_DelayUseTargets(self, other, self->delay);
		return;
	}
	
	if(self->message && (other->flags&BT_FL_CLIENT))
	{
		btCenterprint(other, self->message);
		btSound(other, BT_CHAN_VOICE, "sound/misc/talk.wav", 1, BT_ATTN_NORM);

//		btSpeak2(other, BT_CHAN_VOICE,
//			"female0", NULL, self->message,
//			1.0, BT_ATTN_NORM);
	}

	if(self->target)
	{
		printf("BT_UseTargets: %p %p %s\n", self, other, self->target);
	
		cur=btFindTargets(self->target); n=0;
		while(cur)
		{
			tmpArr[n++]=cur;
			cur=cur->chain;
		}
		
		for(i=0; i<n; i++)
			btEntUse(tmpArr[i], other);
	}

	if(self->killtarget)
	{
		printf("BT_UseTargets: killtarget %p %p %s\n",
			self, other, self->killtarget);

		cur=btFindTargets(self->killtarget); n=0;
		while(cur)
		{
			tmpArr[n++]=cur;
			cur=cur->chain;
		}

		for(i=0; i<n; i++)
			btDeleteEntity(tmpArr[i]);
	}

	if(self->use_eval)
	{
		btScriptEval(self->use_eval);
	}
}

/** Set basic ending time for attack */
BTSV_API void BT_AttackFinished(btEntity self, float delay)
	{ self->attackFinished=btDelayTime(delay); }

/** Check if the last attack has already finished */
BTSV_API int BT_AttackFinishedP(btEntity self)
	{ return(self->attackFinished<btTime()); }

/** Check if the last attack has finished a certain time ago */
BTSV_API int BT_AttackFinishedTimeP(btEntity self, float delay)
	{ return((self->attackFinished+delay)<btTime()); }

/** Set rapid-fire finished time for attacks */
BTSV_API void BT_AttackFinishedRapid(btEntity self, float delay)
{
	float dt1;
	
	//check if the prior attack finished <= 1 frame ago
	dt1=btTime()-self->attackFinished;
	if((dt1>0) && (dt1<=btge_framedt))
	{
		//set next attack to directly follow prior finishing
		//this can help reduce delays with rapid-fire weapons
		self->attackFinished=self->attackFinished+delay;
		return;
	}
	
	//otherwise, do usual thing
	self->attackFinished=btDelayTime(delay);
}

BTSV_API void BT_PainFinished(btEntity self, float delay)
	{ self->painFinished=btDelayTime(delay); }
BTSV_API int BT_PainFinishedP(btEntity self)
	{ return(self->painFinished<btTime()); }

BTSV_API int BT_PainFinishedTimeP(btEntity self, float delay)
	{ return((self->painFinished+delay)<btTime()); }

BTSV_API void BT_AirFinished(btEntity self, float delay)
	{ self->airFinished=btDelayTime(delay); }
BTSV_API int BT_AirFinishedP(btEntity self)
	{ return(self->airFinished<btTime()); }

BTSV_API void btParticleExplosion(vec3 org)
{
	btParticles("flame", 1500, BT_RGB_WHITE, org, vec3(0,0,0), vec3(250,250,250));
	btParticles("flame", 500, BT_RGB_WHITE, org, vec3(0,0,0), vec3(25,25,25));
	btParticles("smoke_light", 100, BT_RGB_WHITE, org, vec3(0,0,0), vec3(25,25,25));
}

BTSV_API void btParticleBloodExplosion(vec3 org)
{
	btParticles("generic_light_fall", 1500, BT_RGB_RED_DARK,
		org, vec3(0,0,0), vec3(250,250,250));
	btParticles("generic_fall", 250, BT_RGB_RED_DARK,
		org, vec3(0,0,75), vec3(50,50,50));

	btParticles("gib_fall", 100, BT_RGB_WHITE,
		org, vec3(0,0,50), vec3(25,25,25));
}

BTSV_API void btParticleRobotExplosion(vec3 org)
{
	btParticles("flame", 1500, BT_RGB_WHITE, org, vec3(0,0,0), vec3(250,250,250));
	btParticles("flame", 500, BT_RGB_WHITE, org, vec3(0,0,0), vec3(25,25,25));

	btParticles("generic_light_fall", 1500, BT_RGB_GREEN_DARK,
		org, vec3(0,0,0), vec3(250,250,250));
	btParticles("generic_fall", 250, BT_RGB_GREEN_DARK,
		org, vec3(0,0,75), vec3(50,50,50));

	btParticles("metgib_fall", 100, BT_RGB_GREEN_DARK,
		org, vec3(0,0,50), vec3(25,25,25));
}

BTSV_API void btParticleTeleportExplosion(vec3 org)
{
	btParticles("generic_light_fall", 1000, BT_RGB_BLUE_DARK,
		org, vec3(0,0,0), vec3(250,250,250));
	btParticles("generic_fall", 750, BT_RGB_BLUE_DARK,
		org, vec3(0,0,75), vec3(50,50,50));
	btParticles("smoke_light", 100, BT_RGB_BLUE, org, vec3(0,0,0), vec3(25,25,25));

//	btParticles("gib_fall", 100, BT_RGB_WHITE,
//		org, vec3(0,0,50), vec3(25,25,25));
}

BTSV_API void btParticleSparks(vec3 org)
{
//	btParticles("flame", 1500, BT_RGB_WHITE, org, vec3(0,0,0), vec3(250,250,250));
	btParticles("flame", 100, BT_RGB_WHITE, org, vec3(0,0,0), vec3(25,25,25));
//	btParticles("smoke_light", 100, BT_RGB_WHITE, org, vec3(0,0,0), vec3(25,25,25));
}

BTSV_API int BT_ExplodeRemoveSelf(btEntity self)
{
	btSetModel(self, NULL);
	btSetThink(self, btDeleteEntity, 0.1);
//	btDeleteEntity(self);
	return(0);
}

BTSV_API int BT_EntityExplode(btEntity self)
{
	if(!self || (self==UNDEFINED))
		return(-1);

	self->solidtype=BT_SOLID_NOT;
	self->movetype=BT_MOVE_NONE;
	self->velocity=vec3(0,0,0);

	btParticleExplosion(self->origin);
	btSound(self, BT_CHAN_VOICE, "sound/weapons/rocklx1a",
		1.0, BT_ATTN_NORM);

	btSetModel(self, NULL);
	btSetThink(self, btDeleteEntity, 0.2);
//	btSetThink(self, BT_ExplodeRemoveSelf, 0.1);
	return(0);
}

BTSV_API void btBecomeExplosion(btEntity self)
	{ BT_EntityExplode(self); }

BTSV_API int BT_EntityBloodExplode(btEntity self)
{
	self->solidtype=BT_SOLID_NOT;
	self->movetype=BT_MOVE_NONE;
	self->velocity=vec3(0,0,0);

	btParticleBloodExplosion(self->origin);
	btSound(self, BT_CHAN_VOICE, "sound/misc/udeath",
		1.0, BT_ATTN_NORM);

	btSetModel(self, NULL);
	btSetThink(self, btDeleteEntity, 0.2);
//	btSetThink(self, BT_ExplodeRemoveSelf, 0.1);
	return(0);
}

BTSV_API int BT_EntitySparksExplode(btEntity self)
{
	self->solidtype=BT_SOLID_NOT;
	self->movetype=BT_MOVE_NONE;
	self->velocity=vec3(0,0,0);

	btParticleSparks(self->origin);
	btSound(self, BT_CHAN_VOICE, "sound/weapons/lashit",
		1.0, BT_ATTN_NORM);

	btSetModel(self, NULL);
	btSetThink(self, btDeleteEntity, 0.1);
//	btSetThink(self, BT_ExplodeRemoveSelf, 0.1);
	return(0);
}

BTSV_API int BT_EntitySmallSparksExplode(btEntity self)
{
	self->solidtype=BT_SOLID_NOT;
	self->movetype=BT_MOVE_NONE;
	self->velocity=vec3(0,0,0);

//	btParticleSparks(self->origin);
//	btParticles("flame", 25, BT_RGB_WHITE, self->origin,
//		vec3(0,0,0), vec3(10,10,10));
	btParticles("generic_fall", 25, BT_RGB_YELLOW,
		self->origin, vec3(0,0,25), vec3(10,10,10));

	btSound(self, BT_CHAN_VOICE, "sound/weapons/lashit",
		1.0, BT_ATTN_NORM);

	btSetModel(self, NULL);
	btSetThink(self, btDeleteEntity, 0.1);
//	btSetThink(self, BT_ExplodeRemoveSelf, 0.1);
	return(0);
}

BTSV_API int BT_EntityRobotExplode(btEntity self)
{
	self->solidtype=BT_SOLID_NOT;
	self->movetype=BT_MOVE_NONE;
	self->velocity=vec3(0,0,0);

//	btParticleSparks(self->origin);
//	btSound(self, BT_CHAN_VOICE, "sound/weapons/lashit",
//		1.0, BT_ATTN_NORM);
	btParticleRobotExplosion(self->origin);
	btSound(self, BT_CHAN_VOICE, "sound/weapons/rocklx1a",
		1.0, BT_ATTN_NORM);

	btSetModel(self, NULL);
	btSetThink(self, btDeleteEntity, 0.1);
//	btSetThink(self, BT_ExplodeRemoveSelf, 0.1);
	return(0);
}

BTSV_API void BT_DisintegrationThink(btEntity self)
{
	int i;

	self->count--;
	if(self->count<=0)
	{
		btParticles("square_light_rise", 100, BT_RGB_VIOLET,
			self->origin, vec3(0,0,0), vec3(50,50,50));

		btSetModel(self, NULL);
		btSetThink(self, btDeleteEntity, 0.1);
		return;
	}
	
	i=self->count;
	if(i>15)i=15;
	
	self->effects&=~BT_EFF_ALPHA_MASK;
	self->effects|=i<<BT_EFF_ALPHA_SHIFT;

	btParticles("square_light_rise", 25, BT_RGB_VIOLET,
		self->origin, vec3(0,0,0), vec3(50,50,50));
	
	btSetThink(self, BT_DisintegrationThink, 0.1);
}

BTSV_API int BT_EntityDisintegration(btEntity self)
{
	self->solidtype=BT_SOLID_NOT;
//	self->movetype=BT_MOVE_NONE;
//	self->velocity=vec3(0,0,0);

//	btParticleSparks(self->origin);
//	btSound(self, BT_CHAN_VOICE, "sound/weapons/lashit",
//		1.0, BT_ATTN_NORM);

	self->count=16;
	btSetThink(self, BT_DisintegrationThink, 0.1);
}



BTSV_API btLight btGetEntityLight(btEntity self)
{
	if(self->light)
		return(self->light);
	self->light=BTGE_AllocServerLight(btge_world);
	return(self->light);
}

BTSV_API void btFreeEntityLight(btEntity self)
{
	if(!self->light)
		return;
	BTGE_FreeServerLight(btge_world, self->light);
	self->light=NULL;
}

BTSV_API void btLightSetOrigin(btLight light, vec3 org)
{
	vfvec3(light->org, org);
}

BTSV_API void btLightSetDirection(btLight light, vec3 dir)
{
	vfvec3(light->dir, dir);
}

BTSV_API void btLightSetTarget(btLight light, vec3 org)
{
	float org1[3];
	vfvec3(org1, org);
	V3F_SUB(org1, light->org, light->dir);
}

BTSV_API void btLightSetColor(btLight light, vec3 clr)
{
	vfvec3(light->clr, clr);
}

BTSV_API void btLightSetColori(btLight light, int clr)
{
	light->clr[0]=(clr&BT_RGB_RED)/((float)BT_RGB_RED);
	light->clr[1]=(clr&BT_RGB_GREEN)/((float)BT_RGB_GREEN);
	light->clr[2]=(clr&BT_RGB_BLUE)/((float)BT_RGB_BLUE);
}
