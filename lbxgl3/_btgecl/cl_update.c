#include <lbxgl2.h>
#include <btgecl.h>

BTCL_World *btcl_world;
extern PDGLUI_InputState *lbxgl_state;
extern PDGLUI_Camera *lbxgl_cam;
LBXGL_BrushWorld *btcl_brushWorld;
float btcl_dt;

/** return predicted server time */
float btclTime(BTCL_World *wrl)
	{ return(wrl->sv_time); }
float btclMsgTime(BTCL_World *wrl)
	{ return(wrl->msg_time); }

float btclFrameTime(BTCL_World *wrl)
	{ return(btcl_dt); }

float btclClientTime()
{
	float f;
	f=PDGL_TimeMS()/1000.0;
	return(f);
}

LBXGL_API BTCL_World *BTCL_NewWorld()
{
	BTCL_World *tmp;
	
	tmp=gctalloc("btcl_world_t", sizeof(BTCL_World));
	return(tmp);
}

LBXGL_API void btclBindWorld(BTCL_World *wrl)
{
	btcl_world=wrl;
}

LBXGL_API void btclBindBrushWorld(LBXGL_BrushWorld *wrl)
{
	btcl_brushWorld=wrl;
}

LBXGL_API void BTCL_StepWorld(BTCL_World *wrl, float dt)
{
	btcl_dt=dt;
//	wrl->
}

LBXGL_API void BTCL_LoadMapFile(BTCL_World *wrl, char *map)
{
	if(wrl->mapname && !strcmp(wrl->mapname, map))
		return;

	wrl->mapname=dystrdup(map);
	LBXGL_BrushMap_LoadFile(btcl_brushWorld, map);
}

LBXGL_API void BTCL_LoadMap(BTCL_World *wrl, char *map)
{
	char tb[256];

//	btCvarSet("mapname", map);

	sprintf(tb, "maps/%s.map", map);
	BTCL_LoadMapFile(wrl, tb);
}

LBXGL_API void BTCL_FlushWorld(BTCL_World *wrl)
{
	BTCL_Entity *cur, *nxt;
	BTCL_Light *lcur, *lnxt;

	PDGL_Sound_StopPlaying(-1);	//stop all sounds
	wrl->cprint_msg=NULL;
	
	wrl->mapname=NULL;
	wrl->bgm_name=NULL;
	wrl->bgm_id=0;
	wrl->camnum=0;

	cur=wrl->ents;
	while(cur)
	{
		nxt=cur->next;
		if(cur->bone_org)gcfree(cur->bone_org);
		if(cur->bone_rot)gcfree(cur->bone_rot);
		gcfree(cur);
		cur=nxt;
	}
	wrl->ents=NULL;
	
	lcur=wrl->lights;
	while(lcur)
	{
		lnxt=lcur->next;
		gcfree(lcur);
		lcur=lnxt;
	}
	wrl->lights=NULL;
}

BTCL_Entity *BTCL_LookupEntity(BTCL_World *wrl, int entnum)
{
	BTCL_Entity *cur;
	
	cur=wrl->ents;
	while(cur)
	{
		if(cur->entnum==entnum)
			return(cur);
		cur=cur->next;
	}
	return(NULL);
}

BTCL_Entity *BTCL_GetEntity(BTCL_World *wrl, int entnum)
{
	BTCL_Entity *ent;

	ent=BTCL_LookupEntity(wrl, entnum);
	if(ent)return(ent);
	
	ent=gctalloc("btcl_entity_t", sizeof(BTCL_Entity));
	ent->entnum=entnum;
	ent->flags|=BTCL_EFL_DIRTY;

	ent->next=wrl->ents;
	wrl->ents=ent;
	
	return(ent);
}

void BTCL_DeleteEntity(BTCL_World *wrl, int entnum)
{
	BTCL_Entity *cur, *lst;
	
	cur=wrl->ents; lst=NULL;
	while(cur)
	{
		if(cur->entnum==entnum)
		{
			if(lst)
				{ lst->next=cur->next; }
			else wrl->ents=cur->next;
			
			BTCL_FlushEntity(wrl, cur);
			gcfree(cur);
			return;
		}
		lst=cur; cur=cur->next;
	}
	return;
}


BTCL_Light *BTCL_LookupLight(BTCL_World *wrl, int num)
{
	BTCL_Light *cur;
	
	cur=wrl->lights;
	while(cur)
	{
		if(cur->num==num)
			return(cur);
		cur=cur->next;
	}
	return(NULL);
}

BTCL_Light *BTCL_GetLight(BTCL_World *wrl, int num)
{
	BTCL_Light *tmp;

	tmp=BTCL_LookupLight(wrl, num);
	if(tmp)return(tmp);
	
	tmp=gctalloc("btcl_light_t", sizeof(BTCL_Light));
	tmp->num=num;
	
	tmp->next=wrl->lights;
	wrl->lights=tmp;
	
	return(tmp);
}

void BTCL_DecodeMsgVec(float *v, int nv, dyt msg)
{
	dyt c, t;
	int i;

	c=dycdr(msg);
	for(i=0; i<nv; i++)
	{
		if(!c)break;
		t=dycar(c);
		c=dycdr(c);
		if(!t)continue;
		v[i]=dyfloatv(t);
	}
}

void BTCL_DecodeMsgVec2(float *v, dyt msg)
	{ BTCL_DecodeMsgVec(v, 2, msg); }
void BTCL_DecodeMsgVec3(float *v, dyt msg)
	{ BTCL_DecodeMsgVec(v, 3, msg); }
void BTCL_DecodeMsgVec4(float *v, dyt msg)
	{ BTCL_DecodeMsgVec(v, 4, msg); }

void BTCL_DecodeBoneDeltaAttr(BTCL_World *wrl, BTCL_Entity *ent,
	int bone, dyt msg)
{
	dyt v;
	char *s;
	
	v=dycar(msg);
	if(v==dysym("org"))
		{ BTCL_DecodeMsgVec3(ent->bone_org+(bone*3), msg); return; }
//	if(v==dysym("vel"))
//		{ BTCL_DecodeMsgVec3(ent->vel, msg); return; }

	if(v==dysym("rot"))
		{ BTCL_DecodeMsgVec4(ent->bone_rot+(bone*4), msg); return; }
}

void BTCL_DecodeBoneDelta(BTCL_World *wrl, BTCL_Entity *ent, dyt msg)
{
//	BTCL_Light *ent;
	dyt c;
	int i, j;

//	printf("BTCL_DecodeLDelta: ");
//	dysPrintln(msg);

	i=dyintv(dycadr(msg));
	if(i<=0)return;
//	ent=BTCL_GetLight(wrl, i);
	
	if(!ent->bone_org)
	{
		j=16;
		while(j<=i)j=j+(j>>1);
		ent->bone_org=gcalloc(j*3*sizeof(float));
		ent->bone_rot=gcalloc(j*4*sizeof(float));
		ent->n_bones=j;
		ent->bone_mask=0;
	}
	
	if(i>=ent->n_bones)
	{
		j=ent->n_bones;
		while(j<=i)j=j+(j>>1);
		ent->bone_org=gcrealloc(ent->bone_org, j*3*sizeof(float));
		ent->bone_rot=gcrealloc(ent->bone_rot, j*4*sizeof(float));
		ent->n_bones=j;
	}
	
	ent->bone_mask|=(1<<i);
	
//	ent->time=btclMsgTime(wrl);
	
	c=dycddr(msg);
	while(c)
	{
		BTCL_DecodeBoneDeltaAttr(wrl, ent, i, dycar(c));
		c=dycdr(c);
	}
}

void BTCL_DecodeDeltaAttr(BTCL_World *wrl, BTCL_Entity *ent, dyt msg)
{
	dyt v;
	char *s;
	int i;
	
	v=dycar(msg);
	if(v==dysym("org"))
		{ BTCL_DecodeMsgVec3(ent->org, msg); return; }
	if(v==dysym("vel"))
		{ BTCL_DecodeMsgVec3(ent->vel, msg); return; }

	if(v==dysym("ang"))
		{ V4F_ZERO(ent->rot); BTCL_DecodeMsgVec3(ent->ang, msg); return; }
	if(v==dysym("rot"))
		{ V3F_ZERO(ent->ang); BTCL_DecodeMsgVec4(ent->rot, msg); return; }

	if(v==dysym("mins"))
		{ BTCL_DecodeMsgVec3(ent->mins, msg); return; }
	if(v==dysym("maxs"))
		{ BTCL_DecodeMsgVec3(ent->maxs, msg); return; }
	if(v==dysym("vorg"))
		{ BTCL_DecodeMsgVec3(ent->vorg, msg); return; }

	if(v==dysym("mdl"))
	{
//		ent->lmdl=ent->mdl;
		ent->mdl=dystringv(dycadr(msg));
		return;
	}
	if(v==dysym("wmdl"))
	{
//		ent->lwmdl=ent->wmdl;
		ent->wmdl=dystringv(dycadr(msg));
		return;
	}

	if(v==dysym("anim"))
		{ ent->anim=dystringv(dycadr(msg)); return; }
	if(v==dysym("wanim"))
		{ ent->wanim=dystringv(dycadr(msg)); return; }

	if(v==dysym("frm"))
		{ ent->frm=dyintv(dycadr(msg)); return; }
	if(v==dysym("wfrm"))
		{ ent->frm=dyintv(dycadr(msg)); return; }
	if(v==dysym("eff"))
		{ ent->effects=dyintv(dycadr(msg)); return; }
	if(v==dysym("eff2"))
		{ ent->effects2=dyintv(dycadr(msg)); return; }

	if(v==dysym("bone"))
		{ BTCL_DecodeBoneDelta(wrl, ent, msg); return; }

	if(v==dysym("delbone"))
	{
		i=dyintv(dycadr(msg));
		if(i<0) { ent->bone_mask=0; return; }
		ent->bone_mask&=~(1<<i);
		return;
	}
}

void BTCL_DecodeDelta(BTCL_World *wrl, dyt msg)
{
	BTCL_Entity *ent;
	dyt c;
	int i;

	i=dyintv(dycadr(msg));
	if(i<=0)return;
	ent=BTCL_GetEntity(wrl, i);

	ent->time3=ent->time2;
	V3F_COPY(ent->org2, ent->org3);

	ent->time2=ent->time;
	V3F_COPY(ent->org, ent->org2);
	V3F_COPY(ent->vel, ent->vel2);
	V3F_COPY(ent->ang, ent->ang2);
	V4F_COPY(ent->rot, ent->rot2);

	ent->time=btclMsgTime(wrl);
	ent->flags|=BTCL_EFL_DIRTY;

	c=dycddr(msg);
	while(c)
	{
		BTCL_DecodeDeltaAttr(wrl, ent, dycar(c));
		c=dycdr(c);
	}
	
	if(ent->time2<=0)
	{
		ent->time2=ent->time;
		V3F_COPY(ent->org, ent->org2);
		V3F_COPY(ent->vel, ent->vel2);
		V3F_COPY(ent->ang, ent->ang2);
		V4F_COPY(ent->rot, ent->rot2);
		ent->time3=ent->time2;
		V3F_COPY(ent->org2, ent->org3);
	}
}

void BTCL_DecodeLDeltaAttr(BTCL_World *wrl, BTCL_Light *ent, dyt msg)
{
	dyt v;
	
	v=dycar(msg);
	if(v==dysym("org"))
		{ BTCL_DecodeMsgVec3(ent->org, msg); return; }
	if(v==dysym("dir"))
		{ BTCL_DecodeMsgVec3(ent->dir, msg); return; }
	if(v==dysym("clr"))
		{ BTCL_DecodeMsgVec3(ent->clr, msg); return; }
	if(v==dysym("val"))
		{ BTCL_DecodeMsgVec2(ent->val, msg); return; }
	if(v==dysym("coeff"))
		{ BTCL_DecodeMsgVec3(ent->coeff, msg); return; }
	if(v==dysym("ang"))
		{ ent->angle=dyfloatv(dycadr(msg)); return; }

	if(v==dysym("mins"))
		{ BTCL_DecodeMsgVec3(ent->mins, msg); return; }
	if(v==dysym("mins"))
		{ BTCL_DecodeMsgVec3(ent->maxs, msg); return; }
	if(v==dysym("boxval"))
		{ BTCL_DecodeMsgVec3(ent->boxval, msg); return; }

	if(v==dysym("texname"))
		{ ent->texname=dystringv(dycadr(msg)); return; }

	if(v==dysym("fl"))
		{ ent->flags=dyintv(dycadr(msg)); return; }
	if(v==dysym("st"))
		{ ent->style=dyintv(dycadr(msg)); return; }
//	if(v==dysym("eff"))
//		{ ent->effects=dyintv(dycadr(msg)); return; }
}

void BTCL_DecodeLDelta(BTCL_World *wrl, dyt msg)
{
	BTCL_Light *ent;
	dyt c;
	int i;

	printf("BTCL_DecodeLDelta: ");
	dysPrintln(msg);

	i=dyintv(dycadr(msg));
	if(i<=0)return;
	ent=BTCL_GetLight(wrl, i);
	
//	ent->time=btclMsgTime(wrl);
	
	c=dycddr(msg);
	while(c)
	{
		BTCL_DecodeLDeltaAttr(wrl, ent, dycar(c));
		c=dycdr(c);
	}
}

void BTCL_DecodeSound(BTCL_World *wrl, dyt msg)
{
	float io[4], iv[4];
	BTCL_Entity *ent;
	char *name;
	dyt c;
	float vol;
	int chan, attn;
	int i, j, fl;

	i=dyintv(dycadr(msg));
	if(i<=0)return;
	ent=BTCL_GetEntity(wrl, i);
	
	ent->time=btclMsgTime(wrl);
	ent->flags|=BTCL_EFL_DIRTY;
	
	chan=dyintv(dycaddr(msg));
	name=dystringv(dycadddr(msg));
	vol=dyfloatv(dycaddddr(msg));
	attn=dyintv(dycadddddr(msg));

	fl=0;
	if(chan&BT_CHAN_AMBIENT)fl|=PDGL_SOUND_LOOP;
	if(attn==BT_ATTN_NONE)fl|=PDGL_SOUND_NOATTN;
	if(attn==BT_ATTN_STATIC)fl|=PDGL_SOUND_HIGHATTN;
	if(attn==BT_ATTN_LOW)fl|=PDGL_SOUND_LOWATTN;

#if 0
	printf("%s (%.2f %.2f %.2f) (%.2f %.2f %.2f)\n"
		"\t(%.2f %.2f %.2f) (%.2f %.2f %.2f)\n", name,
		ent->org[0], ent->org[1], ent->org[2],
		ent->vel[0], ent->vel[1], ent->vel[2],
		ent->mins[0], ent->mins[1], ent->mins[2],
		ent->maxs[0], ent->maxs[1], ent->maxs[2]);
#endif

	io[0]=ent->org[0]+(ent->mins[0]+ent->maxs[0])*0.5;
	io[1]=ent->org[1]+(ent->mins[1]+ent->maxs[1])*0.5;
	io[2]=ent->org[2]+(ent->mins[2]+ent->maxs[2])*0.5;

	V3F_SCALE(io, 1.0/40, io);
//	V3F_SCALE(ent->org, 1.0/40, io);
	V3F_SCALE(ent->vel, 1.0/40, iv);
	if(!(*name) || (*name=='-'))
		{ i=0; }
	else
		{ i=PDGL_Sound_PlayPointSample(name, fl, vol, 1.0, io, iv); }
	if(chan&7)
	{
		//stop prior sound on a given channel
		if(ent->s_chan[chan&7]>0)
			PDGL_Sound_StopPlaying(ent->s_chan[chan&7]);
		ent->s_chan[chan&7]=i;
	}
}

void BTCL_DecodeSpeak(BTCL_World *wrl, dyt msg)
{
	char tb[256];
	float io[4], iv[4];
	BTCL_Entity *ent;
	char *text, *voice, *ctrl, *str;
	dyt c;
	float vol;
	int chan, attn;
	int i, j, fl;

	i=dyintv(dycadr(msg));
	if(i<=0)return;
	ent=BTCL_GetEntity(wrl, i);
	
	ent->time=btclMsgTime(wrl);
	ent->flags|=BTCL_EFL_DIRTY;
	
	chan=dyintv(dycaddr(msg));
	voice=dystringv(dycadddr(msg));
	ctrl=dystringv(dycaddddr(msg));
	text=dystringv(dycadddddr(msg));
	vol=dyfloatv(dycaddddddr(msg));
	attn=dyintv(dycadddddddr(msg));

	fl=0;
	if(chan&BT_CHAN_AMBIENT)fl|=PDGL_SOUND_LOOP;
	if(attn==BT_ATTN_NONE)fl|=PDGL_SOUND_NOATTN;
	if(attn==BT_ATTN_STATIC)fl|=PDGL_SOUND_HIGHATTN;
	if(attn==BT_ATTN_LOW)fl|=PDGL_SOUND_LOWATTN;

#if 0
	printf("%s (%.2f %.2f %.2f) (%.2f %.2f %.2f)\n"
		"\t(%.2f %.2f %.2f) (%.2f %.2f %.2f)\n", name,
		ent->org[0], ent->org[1], ent->org[2],
		ent->vel[0], ent->vel[1], ent->vel[2],
		ent->mins[0], ent->mins[1], ent->mins[2],
		ent->maxs[0], ent->maxs[1], ent->maxs[2]);
#endif

	io[0]=ent->org[0]+(ent->mins[0]+ent->maxs[0])*0.5;
	io[1]=ent->org[1]+(ent->mins[1]+ent->maxs[1])*0.5;
	io[2]=ent->org[2]+(ent->mins[2]+ent->maxs[2])*0.5;

	str=text;
	if(ctrl)
	{
		sprintf(tb, "%s %s", ctrl, text);
		str=tb;
	}

	V3F_SCALE(io, 1.0/40, io);
//	V3F_SCALE(ent->org, 1.0/40, io);
	V3F_SCALE(ent->vel, 1.0/40, iv);
//	i=PDGL_Sound_PlayPointSample(name, fl, vol, 1.0, io, iv);
//	i=PDGL_Sound_PlayPointSample(name, fl, vol, 1.0, io, iv);
	i=PDGL_Voice_PointSpeak(voice, str, fl, vol, 1.0, io, iv);

	if(chan&7)
	{
		//stop prior sound on a given channel
		if(ent->s_chan[chan&7]>0)
			PDGL_Sound_StopPlaying(ent->s_chan[chan&7]);
		ent->s_chan[chan&7]=i;
	}
}


void BTCL_DecodeParticleEffect(BTCL_World *wrl, dyt msg)
{
	float org[3], vel[3], svel[3];
	dyt c, l, v;
	char *ty;
	int cnt, clr;
	int i;

	V3F_ZERO(org);
	V3F_ZERO(vel);
	V3F_ZERO(svel);

	cnt=100;
	ty="flame";

	c=dycdr(msg);
	while(c)
	{
		l=dycar(c);
		v=dycar(l);
		if(v==dysym("ty"))
			{ ty=dystringv(dycadr(l)); }
		else if(v==dysym("org"))
			{ BTCL_DecodeMsgVec3(org, l); }
		else if(v==dysym("vel"))
			{ BTCL_DecodeMsgVec3(vel, l); }
		else if(v==dysym("svel"))
			{ BTCL_DecodeMsgVec3(svel, l); }
		else if(v==dysym("cnt"))
			{ cnt=dyintv(dycadr(l)); }
		else if(v==dysym("clr"))
			{ clr=dyintv(dycadr(l)); }
//		if(v==dysym("st"))
//			{ ent->style=dyintv(dycadr(l)); }

		c=dycdr(c);
	}
	
	LBXGL_Particle_SpawnSpread(ty, cnt, clr, org, vel, svel);
}

void BTCL_DecodeUpdateMsg(BTCL_World *wrl, dyt msg)
{
	char *s;
	dyt v;
	float f, g, h;
	float dt, clt;
	
	if(!dyconsp(msg))
	{
		printf("Bad Message: ");
		dysPrintln(msg);
		return;
	}
	
	v=dycar(msg);
	if(v==dysym("delta"))
		{ BTCL_DecodeDelta(wrl, msg); return; }

	if(v==dysym("delent"))
		{ BTCL_DeleteEntity(wrl, dyintv(dycadr(msg))); return; }

	if(v==dysym("ldelta"))
		{ BTCL_DecodeLDelta(wrl, msg); return; }

	if(v==dysym("wdelta"))
		{ BTCL_DecodeWDelta(wrl, msg); return; }

	if(v==dysym("sound"))
		{ BTCL_DecodeSound(wrl, msg); return; }

	if(v==dysym("speak"))
		{ BTCL_DecodeSpeak(wrl, msg); return; }

	if(v==dysym("particles"))
		{ BTCL_DecodeParticleEffect(wrl, msg); return; }

	if(v==dysym("time"))
	{
//		BTCL_DecodeDelta(wrl, msg);
		f=dyfloatv(dycadr(msg));	//server time
		g=dyfloatv(dycaddr(msg));	//client time
		
		dt=g-btclClientTime();
		if(dt<0)dt=0;
		if(dt>1)dt=1;
		//estimate delay from server
		
		wrl->msg_time=f;			//timestamp of new message
		wrl->msg_cltime=g;			//client timestamp of new message
		wrl->msg_estdt=dt;			//estimated delay

		wrl->sv_time=f+(dt*0.5);	//estimated time on server
		
		//calculate time adjustment
		wrl->sv_dtadjust=wrl->sv_time-btclClientTime();
		return;
	}

	if(v==dysym("camera"))
	{
		wrl->camnum=dyintv(dycadr(msg));
		printf("camera: %d\n", wrl->camnum);
		PDGLUI_HideMenu(NULL);	//avoid lingering menus
		return;
	}

	if(v==dysym("centerprint"))
	{
		wrl->cprint_msg=dystringv(dycadr(msg));
		wrl->cprint_time=btclClientTime()+5;

//		wrl->cprint_boxmsg=dystringv(dycadr(msg));

//		printf("centerprint: %s\n", wrl->cprint_msg);
		return;
	}

	if(v==dysym("centerprint_box"))
	{
		wrl->cprint_boxmsg=dystringv(dycadr(msg));
		if(!(*(wrl->cprint_boxmsg)))
			wrl->cprint_boxmsg=NULL;
//		wrl->cprint_time=btclClientTime()+5;
//		printf("centerprint: %s\n", wrl->cprint_msg);
		return;
	}

	if(v==dysym("viewflags"))
	{
		wrl->viewflags=dyintv(dycadr(msg));
		return;
	}

	if(v==dysym("view_mins"))
		{ BTCL_DecodeMsgVec3(wrl->view_mins, msg); return; }
	if(v==dysym("view_maxs"))
		{ BTCL_DecodeMsgVec3(wrl->view_maxs, msg); return; }
	if(v==dysym("view_fov"))
		{ wrl->view_fov=dyfloatv(dycadr(msg)); return; }

	if(v==dysym("viewflash"))
	{
		wrl->flash_color=dyintv(dycadr(msg));
		wrl->flash_time=dyfloatv(dycaddr(msg));
		wrl->flash_scale=dyfloatv(dycadddr(msg));
		return;
	}

	if(v==dysym("setbgm"))
	{
		if(wrl->bgm_id>0)
			PDGL_Sound_StopPlaying(wrl->bgm_id);
		wrl->bgm_name=dystringv(dycadr(msg));
		printf("Set BGM %s\n", wrl->bgm_name);
		wrl->bgm_id=PDGL_Sound_PlaySample(wrl->bgm_name,
			PDGL_SOUND_LOOP, 1.0);
	}

	if(v==dysym("stuffcmd"))
	{
		ConCmds_FilteredHandler(dystringv(dycadr(msg)));
//		wrl->cprint_msg=dystringv(dycadr(msg));
//		wrl->cprint_time=btclClientTime()+5;
//		printf("centerprint: %s\n", wrl->cprint_msg);
		return;
	}
	
	if(v==dysym("mapname"))
	{
		BTCL_LoadMap(wrl, dystringv(dycadr(msg)));
		return;
	}

	if(v==dysym("hp"))
	{
		wrl->curHP=dyintv(dycadr(msg));
		wrl->maxHP=dyintv(dycaddr(msg));
		return;
	}

	if(v==dysym("mp"))
	{
		wrl->curMP=dyintv(dycadr(msg));
		wrl->maxMP=dyintv(dycaddr(msg));
		return;
	}

	if(v==dysym("inven"))
	{
		wrl->curMP=dyintv(dycadr(msg));
		wrl->maxMP=dyintv(dycaddr(msg));
		return;
	}
}

void BTCL_DecodeWDelta(BTCL_World *wrl, dyt msg)
{
	dyt c;
	int i;

	//don't decode world deltas if we don't have a camera
//	if(!wrl->camnum)
//		return;

	c=dycdr(msg);
	while(dyconsp(c))
	{
		BTCL_DecodeUpdateMsg(wrl, dycar(c));
		c=dycdr(c);
	}
}

LBXGL_API void BTCL_DecodeUpdate(BTCL_World *wrl, dyt msg)
{
	dyt c;
	int i;

	if(!msg)return;

//	c=dycdr(msg);
	c=msg;
	while(dyconsp(c))
	{
		BTCL_DecodeUpdateMsg(wrl, dycar(c));
		c=dycdr(c);
	}
}

void BTCL_FlushEntity(BTCL_World *wrl, BTCL_Entity *ent)
{
	LBXGL_Light *dl;

	if(ent->mdl || ent->wmdl ||
		(ent->effects&BT_EFF_BEAM))
	{
		LBXGL_BrushWorld_DeleteModelStateEntNum(
			btcl_brushWorld, ent->entnum);
		ent->mdl=NULL;
		ent->wmdl=NULL;
	}

	if(ent->effects&BT_EFF_GLOW_MASK)
	{
		dl=lbxWorldGetLight(btcl_brushWorld, ent->entnum|BT_LNUM_GLOW);
		dl->flags|=LBXGL_LFL_DISABLE;
	}
}

void BTCL_PredictEntFrame(BTCL_World *wrl, BTCL_Entity *ent)
{
	float ia[3], io[3], iv[3], iv1[3], iv2[3];
	PDGLUI_InputState *state;
	PDGLUI_Camera *cam;
	float f, g, h, dt;

	dt=btclTime(wrl)-ent->time;
	if(dt<0)dt=0;

#if 1
	f=ent->time - ent->time2;
	g=ent->time2 - ent->time3;
	if(f<=0)f=1;
	if(g<=0)g=1;
	V3F_SUB(ent->org, ent->org2, iv1);
	V3F_SUB(ent->org2, ent->org3, iv2);
	V3F_SCALE(iv1, 1.0/f, iv1);
	V3F_SCALE(iv2, 1.0/g, iv2);

//	V3F_ADDSCALE(iv1, iv2, -dt, iv);

	V3F_COPY(iv1, iv);

//	h=1.0+dt;
//	V3F_SCALEADDSCALE(iv1, h, iv2, 1.0-h, iv);
#endif

#if 1
	if(ent->entnum==wrl->camnum)
	{
		state=PDGLUI_GetState();
		cam=PDGLUI_GetCamera();

//		if((fabs(cam->ivel[0])+fabs(cam->ivel[1]))<0.1)
//			{ iv[0]=0; iv[1]=0; }
		V3F_SCALE(cam->ivel, 1.0/state->dt_f, iv1);
//		iv[0]=iv1[0];
//		iv[1]=iv1[1];
		iv1[2]=iv[2];

//		iv[0]=min(iv[0], iv1[0]);
//		iv[1]=min(iv[1], iv1[1]);

		iv[0]=(fabs(iv[0])<fabs(iv1[0]))?iv[0]:iv1[0];
		iv[1]=(fabs(iv[1])<fabs(iv1[1]))?iv[1]:iv1[1];

//		V3F_ADDSCALE(cam->org, iv1, state->dt_f, io);
		V3F_ADDSCALE(cam->org, iv, state->dt_f, io);
		V3F_SUB(io, ent->vorg, io);
	}
#endif

	if((dt<0.25) && !(ent->effects2&BT_EF2_NOLERP))
	{
//		V3F_ADDSCALE(ent->org, ent->vel, dt, ent->p_org);
		V3F_ADDSCALE(ent->org, iv, dt, ent->p_org);
	}else
	{
		V3F_COPY(ent->org, ent->p_org);
	}

#if 1
	if(ent->entnum==wrl->camnum)
	{
		V3F_SCALEADDSCALE(ent->p_org, 0.5, io, 0.5, ent->p_org);
	}
#endif

	if(V4F_LEN(ent->rot)>0.8)
	{
		V4F_COPY(ent->rot, ent->p_rot);
	}else
	{
		V3F_COPY(ent->ang, ent->p_ang);
//		V3F_SCALE(ent->ang, (M_PI/180), ia);
		ia[0]=ent->ang[0]*(M_PI/180);
		ia[1]=ent->ang[1]*(M_PI/180);
		ia[2]=ent->ang[2]*(M_PI/180);
		QuatF_FromAngles(ia, ent->p_rot);
//		QuatF_FromAngles(ent->ang, ent->p_rot);
	}
}

void BTCL_PredictEntWFrame(BTCL_World *wrl, BTCL_Entity *ent)
{
	PDGLUI_Camera *cam;
	float ia[3], iv[3], iv1[3], iv2[3];
	float f, g, h, dt;

#if 0
	dt=btclTime(wrl)-ent->time;
	if(dt<0)dt=0;

//	f=ent->time - ent->time2;
//	if(f<=0)f=1;
//	V3F_SUB(ent->org, ent->org2, iv);
//	V3F_SCALE(iv, 1.0/f, iv);

#if 1
	f=ent->time - ent->time2;
	g=ent->time2 - ent->time3;
	if(f<=0)f=1;
	if(g<=0)g=1;
	V3F_SUB(ent->org, ent->org2, iv1);
	V3F_SUB(ent->org2, ent->org3, iv2);
	V3F_SCALE(iv1, 1.0/f, iv1);
	V3F_SCALE(iv2, 1.0/g, iv2);

//	V3F_ADDSCALE(iv1, iv2, -dt, iv);

	h=1.0+dt;
	V3F_SCALEADDSCALE(iv1, h, iv2, 1.0-h, iv);
#endif

	if(dt<0.25)
	{
//		V3F_ADDSCALE(ent->org, ent->vel, dt, ent->p_org);
		V3F_ADDSCALE(ent->org, iv, dt, ent->p_org);
	}else
	{
		V3F_COPY(ent->org, ent->p_org);
	}
#endif

//	BTCL_PredictEntFrame(wrl, ent);

	cam=PDGLUI_GetCamera();

//	ent->p_org[2]+=48;
	V3F_ADD(ent->p_org, ent->vorg, ent->p_org);

	if(V4F_LEN(ent->rot)>0.8)
	{
		V4F_COPY(ent->rot, ent->p_rot);
	}else
	{
		V3F_COPY(ent->ang, ent->p_ang);
//		V3F_SCALE(ent->ang, (M_PI/180), ia);
//		ia[0]=(-ent->ang[0]-90)*(M_PI/180);
		ia[0]=(-ent->ang[0])*(M_PI/180);
		ia[1]=ent->ang[1]*(M_PI/180);
		ia[2]=ent->ang[2]*(M_PI/180);
		QuatF_FromAngles(ia, ent->p_rot);
//		QuatF_FromAngles(ent->ang, ent->p_rot);
	}
}

void BTCL_SetupClientEntFrameMdl(BTCL_World *wrl, BTCL_Entity *ent)
{
	LBXGL_ModelState *mdl;
	void *ptr;
	float v0[4];
	int i;

	if(wrl->camnum<=0)
		return;

	if(!ent->mdl)
	{
		if(ent->lmdl)
		{
			LBXGL_BrushWorld_DeleteModelStateEntNum(
				btcl_brushWorld, ent->entnum);
			ent->lmdl=NULL;
		}
		return;
	}
	
	i=atoi(ent->mdl);
	if(i>0)
	{
		mdl=LBXGL_BrushWorld_LookupModelStateMdlNum(btcl_brushWorld, i);
		if(!mdl)ent->flags|=BTCL_EFL_DIRTY;
	}else
	{
		if(ent->mdl!=ent->lmdl)
			LBXGL_BrushWorld_DeleteModelStateEntNum(
				btcl_brushWorld, ent->entnum);

		mdl=LBXGL_BrushWorld_LookupModelStateEntNum(
			btcl_brushWorld, ent->entnum);

		if(!mdl)
		{
			ent->lmdl=ent->mdl;
			
			if(!strcmp(ent->mdl, "$laser"))
			{
//				ptr=LBXGL_LaserEffect_Create(ent->org, ent->vel, ent->ang, )
			}else
			{
				mdl=LBXGL_Mdl_LoadModelInstance(ent->mdl);				
				LBXGL_BrushWorld_AddModelStateEntNum(
					btcl_brushWorld, mdl, ent->entnum);

				if(!mdl)
					ent->mdl=NULL;
			}
		}
	}

	if(!mdl)return;

	BTCL_PredictEntFrame(wrl, ent);
	V3F_COPY(ent->p_org, mdl->org);	
	QuatF_To3Matrix(ent->p_rot, mdl->rot);
//	Mat3F_Identity(mdl->rot);

	if(ent->anim)
			LBXGL_Mdl_SetAnimFrame(mdl, ent->anim, ent->frm);
	else	LBXGL_Mdl_SetFrame(mdl, ent->frm);

//	if(ent->effects&BT_EFF_GLOW_MASK)
	if(ent->effects&BT_EFF_NOSHADOW)
		mdl->flags|=LBXGL_TXFL_NOSHADOW;
}

void BTCL_SetupClientEntFrameWMdl(BTCL_World *wrl, BTCL_Entity *ent)
{
	LBXGL_ModelState *mdl;
	float v0[4];
	int i;

	if(wrl->camnum<=0)
		return;

	if(!ent->wmdl)
	{
		if(ent->lmdl)
		{
			LBXGL_BrushWorld_DeleteModelStateEntNum(
				btcl_brushWorld, ent->entnum);
			ent->lmdl=NULL;
		}
		return;
	}
	
	i=atoi(ent->wmdl);
	if(i>0)
	{
		mdl=LBXGL_BrushWorld_LookupModelStateMdlNum(btcl_brushWorld, i);
	}else
	{
		if(ent->wmdl!=ent->lmdl)
			LBXGL_BrushWorld_DeleteModelStateEntNum(
				btcl_brushWorld, ent->entnum);

		mdl=LBXGL_BrushWorld_LookupModelStateEntNum(
			btcl_brushWorld, ent->entnum);

		if(!mdl)
		{
			ent->lmdl=ent->wmdl;
			mdl=LBXGL_Mdl_LoadModelInstance(ent->wmdl);
			LBXGL_BrushWorld_AddModelStateEntNum(
				btcl_brushWorld, mdl, ent->entnum);
		}
	}

	if(!mdl)return;
	BTCL_PredictEntWFrame(wrl, ent);
	V3F_COPY(ent->p_org, mdl->org);	
	QuatF_To3Matrix(ent->p_rot, mdl->rot);

//	mdl->org[2]+=48;

	if(ent->wanim)
			LBXGL_Mdl_SetAnimFrame(mdl, ent->wanim, ent->wfrm);
	else	LBXGL_Mdl_SetFrame(mdl, ent->wfrm);
}

void BTCL_SetupClientEntFrameEffects(BTCL_World *wrl, BTCL_Entity *ent)
{
	float ivel[4], svel[4];
	float tv0[4], tv1[4];
	LBXGL_ModelState *mdl;
	LBXGL_Light *dl;
	void *ptr;
	float f, g;
	int i, j, k;

	if(wrl->camnum<=0)
		return;

	mdl=NULL;

	if(ent->effects&BT_EFF_BEAM)
	{
		f=(ent->effects&BT_EFF_GLOW_HIGH)?0.5:0;
		ivel[0]=(ent->effects&BT_EFF_GLOW_RED)?1:f;
		ivel[1]=(ent->effects&BT_EFF_GLOW_GREEN)?1:f;
		ivel[2]=(ent->effects&BT_EFF_GLOW_BLUE)?1:f;
		V3F_NORMALIZE(ivel, ivel);

		i=0;
		if(ent->effects&BT_EFF_GLOW_BRIGHT0)i|=1;
		if(ent->effects&BT_EFF_GLOW_BRIGHT1)i|=2;
		if(ent->effects&BT_EFF_GLOW_BRIGHT2)i|=4;
		if(ent->effects&BT_EFF_GLOW_BRIGHT3)i|=8;
		f=0.25*pow(1.5, i);

		j=0;
		if(ent->effects&BT_EFF_GLOW_PULSE0)j|=1;
		if(ent->effects&BT_EFF_GLOW_PULSE1)j|=2;
		if(ent->effects&BT_EFF_GLOW_PULSE2)j|=4;
		if(ent->effects&BT_EFF_GLOW_PULSE3)j|=8;

		i=0;
		if(ent->effects&BT_EFF_ALPHA_BIT0)i|=1;
		if(ent->effects&BT_EFF_ALPHA_BIT1)i|=2;
		if(ent->effects&BT_EFF_ALPHA_BIT2)i|=4;
		if(ent->effects&BT_EFF_ALPHA_BIT3)i|=8;
//		g=i?(0.25*i):1.0;
		g=i?(i/16.0):1.0;
		ivel[3]=g;

		V3F_ADD(ent->org, ent->vel, svel);

//		printf("Laser: (%.2f %.2f %.2f) -> (%.2f %.2f %.2f) %.2f %.2f\n",
//			ent->org[0], ent->org[1], ent->org[2],
//			svel[0], svel[1], svel[2], f, g);

		if(!mdl)mdl=LBXGL_BrushWorld_LookupModelStateEntNum(
			btcl_brushWorld, ent->entnum);

		if(mdl)
		{
			gcfree(mdl->mdl);
			ptr=LBXGL_LaserEffect_Create(ent->org, svel, ivel, f);
			LBXGL_Mdl_SetModel(mdl, ptr);
		}else
		{
//			V3F_COPY(ent->clr, tv0);
			ptr=LBXGL_LaserEffect_Create(ent->org, svel, ivel, f);
			mdl=LBXGL_Mdl_WrapModelInstance(ptr);
			LBXGL_BrushWorld_AddModelStateEntNum(
				btcl_brushWorld, mdl, ent->entnum);
		}

		return;
	}

	if(ent->effects&BT_EFF_ALPHA_MASK)
	{
		i=0;
		if(ent->effects&BT_EFF_ALPHA_BIT0)i|=1;
		if(ent->effects&BT_EFF_ALPHA_BIT1)i|=2;
		if(ent->effects&BT_EFF_ALPHA_BIT2)i|=4;
		if(ent->effects&BT_EFF_ALPHA_BIT3)i|=8;

		if(!mdl)mdl=LBXGL_BrushWorld_LookupModelStateEntNum(
			btcl_brushWorld, ent->entnum);

//		printf("Eff %08X\n", ent->effects);

		if(mdl)
		{
			mdl->flags|=LBXGL_TXFL_VALPHA;
			mdl->flags&=~LBXGL_TXFL_ALPHA_MASK;
			mdl->flags|=i<<LBXGL_TXFL_ALPHA_LSHIFT;
		}
	}

	if(ent->effects2&BT_EF2_SCALE_MASK)
	{
		if(!mdl)mdl=LBXGL_BrushWorld_LookupModelStateEntNum(
			btcl_brushWorld, ent->entnum);
		if(mdl)
		{
			i=(ent->effects2&BT_EF2_SCALE_MASK) >>
				BT_EF2_SCALE_LSHIFT;
			mdl->flags&=~LBXGL_TXFL_SCALE_MASK;
			mdl->flags|=i<<LBXGL_TXFL_SCALE_LSHIFT;
		}
	}

	if(ent->effects&BT_EFF_ROTATE)
	{
		f=btclClientTime()*(360/10);
		f=((int)f)%360;
		ent->ang[0]=0;
		ent->ang[1]=0;
		ent->ang[2]=f;
	}

	if((ent->effects&BT_EFF_TRAIL_MASK)==BT_EFF_BLOODTRAIL)
	{
		V3F_SCALE(ent->vel, 0.05, ivel);
		V3F_SET(svel, 1, 1, 1);
		LBXGL_Particle_SpawnSpread(
			"generic_light_fall",
			1000*btclFrameTime(wrl),
			BT_RGB_RED_DARK,
			ent->org, ivel, svel);
	}

	if((ent->effects&BT_EFF_TRAIL_MASK)==BT_EFF_SMOKETRAIL)
	{
		V3F_SCALE(ent->vel, 0.05, ivel);
		V3F_SET(svel, 1, 1, 1);
		LBXGL_Particle_SpawnSpread(
			"smoke_light",
			1000*btclFrameTime(wrl),
			BT_RGB_WHITE,
			ent->org, ivel, svel);
	}

	if((ent->effects&BT_EFF_TRAIL_MASK)==BT_EFF_FIRETRAIL)
	{
		V3F_SCALE(ent->vel, 0.05, ivel);
		V3F_SET(svel, 1, 1, 1);
		LBXGL_Particle_SpawnSpread(
			"flame",
			1000*btclFrameTime(wrl),
			BT_RGB_WHITE,
			ent->org, ivel, svel);
	}

	if((ent->effects&BT_EFF_TRAIL_MASK)==BT_EFF_SPARKSTRAIL)
	{
		V3F_SCALE(ent->vel, 0.05, ivel);
		V3F_SET(svel, 1, 1, 1);
		LBXGL_Particle_SpawnSpread(
			"generic_light_fall",
			100*btclFrameTime(wrl),
			BT_RGB_YELLOW,
			ent->org, ivel, svel);
	}

	if(ent->effects&BT_EFF_GLOW_MASK)
	{
		f=(ent->effects&BT_EFF_GLOW_HIGH)?0.5:0;
		ivel[0]=(ent->effects&BT_EFF_GLOW_RED)?1:f;
		ivel[1]=(ent->effects&BT_EFF_GLOW_GREEN)?1:f;
		ivel[2]=(ent->effects&BT_EFF_GLOW_BLUE)?1:f;
		V3F_NORMALIZE(ivel, ivel);

		i=0;
		if(ent->effects&BT_EFF_GLOW_BRIGHT0)i|=1;
		if(ent->effects&BT_EFF_GLOW_BRIGHT1)i|=2;
		if(ent->effects&BT_EFF_GLOW_BRIGHT2)i|=4;
		if(ent->effects&BT_EFF_GLOW_BRIGHT3)i|=8;
		f=50*pow(i+1, 1.25);

		j=0;
		if(ent->effects&BT_EFF_GLOW_PULSE0)j|=1;
		if(ent->effects&BT_EFF_GLOW_PULSE1)j|=2;
		if(ent->effects&BT_EFF_GLOW_PULSE2)j|=4;
		if(ent->effects&BT_EFF_GLOW_PULSE3)j|=8;

		dl=lbxWorldGetLight(btcl_brushWorld, ent->entnum|0x10000);
		V3F_COPY(ent->org, dl->org);
		V3F_COPY(ivel, dl->clr);
		dl->val[0]=f; dl->val[1]=0;
		dl->style=j;
		dl->curval=f;

		dl->coeff[0]=0.50/0.58;
		dl->coeff[1]=0.50/0.58;
		dl->coeff[2]=1;

		dl->flags&=~LBXGL_LFL_DISABLE;	//make sure is turned on

//		V3F_COPY(ent->dir, dl->dir);
//		V2F_COPY(ent->val, dl->val);
//		dl->angle=ent->angle;
//		dl->flags=ent->flags;
//		dl->style=ent->style;
	}

	if(ent->effects || ent->effects2)
	{
		if(!mdl)mdl=LBXGL_BrushWorld_LookupModelStateEntNum(
			btcl_brushWorld, ent->entnum);
		if(mdl)
		{
			mdl->effects=ent->effects;
			mdl->effects2=ent->effects2;
		}
	}
//	LBXGL_Particle_SpawnSpread(ty, cnt, clr, org, vel, svel);
}

float btclAngleMod(float v)
{
	while(v>=360)v-=360;
	while(v<0)v+=360;
	return(v);
}

float BTCL_AngleDist1(float a, float b)
{
	float f;

	f=fabs(btclAngleMod(a)-btclAngleMod(b));
	if(f>180)f=360-f;

//	while(f>=360)f-=360;
//	while(f<0)f+=360;

	return(f);
}

float BTCL_AngleDist(float *v0, float *v1)
{
	float f;
	f=	BTCL_AngleDist1(v0[0], v1[0])+
		BTCL_AngleDist1(v0[1], v1[1])+
		BTCL_AngleDist1(v0[2], v1[2]);
	return(f);
}

void BTCL_SetupClientEntFrame(BTCL_World *wrl, BTCL_Entity *ent)
{
	float tv0[3], tv1[3], tv2[3];
	int i;

	if(wrl->camnum<=0)
		return;

	if(!(ent->flags&BTCL_EFL_DIRTY))
	{
		if(ent->mdl)
		{
			i=atoi(ent->mdl);
//			if(i>0)
//				return;
		}
	}
	ent->flags&=~BTCL_EFL_DIRTY;
	
	if(ent->entnum==wrl->camnum)
	{
		BTCL_PredictEntFrame(wrl, ent);
		
		V3F_ADD(ent->p_org, ent->vorg, tv0);
		UI_Camera_SetOrgFV(tv0);

		V3F_COPY(ent->p_ang, tv1);
		tv1[2]=btclAngleMod(tv1[2]-180);
		UI_Camera_GetAngFV(tv2);
//		printf("Angle Check %f %f %f\n",
//			tv2[0]-tv1[0],
//			tv2[1]-tv1[1],
//			tv2[2]-tv1[2]);

//		i=V3F_DIST(tv1, tv2);
		i=BTCL_AngleDist(tv1, tv2);

//		if((wrl->viewflags&BT_EF2_CAM_FORCEANGLE) ||
//			(V3F_DIST(tv1, tv2)>25))
//		if((wrl->viewflags&BT_EF2_CAM_FORCEANGLE) || (i>25))
		if(wrl->viewflags&BT_EF2_CAM_FORCEANGLE)
		{
			UI_Camera_SetAngFV(tv1);
		}else
		{
			//force update angle
			V3F_COPY(tv2, ent->ang);
			ent->ang[2]=btclAngleMod(ent->ang[2]+180);
//			BTCL_PredictEntFrame(wrl, ent);
		}

//		UI_Camera_SetOrgFV(ent->p_org);
//		UI_Camera_SetAngFV(ent->p_ang);

		ent->effects2|=BT_EF2_VIEWMODEL;	//viewmodel hint
		BTCL_SetupClientEntFrameEffects(wrl, ent);
		BTCL_SetupClientEntFrameWMdl(wrl, ent);
		return;
	}
	
	BTCL_SetupClientEntFrameEffects(wrl, ent);
	BTCL_SetupClientEntFrameMdl(wrl, ent);
}

void BTCL_SetupClientLightFrame(BTCL_World *wrl, BTCL_Light *ent)
{
	float tv0[4], tv1[4];
	LBXGL_ModelState *mdl;
	LBXGL_Light *dl;
	void *ptr;

	if(wrl->camnum<=0)
		return;

	if(ent->flags&LBXGL_LFL_LASER)
	{
		mdl=LBXGL_BrushWorld_LookupModelStateEntNum(
			btcl_brushWorld, ent->num);

		if(!mdl)
		{
			V3F_COPY(ent->clr, tv0);
			V3F_ADD(ent->org, ent->dir, tv1);
			tv0[3]=ent->val[0];
			ptr=LBXGL_LaserEffect_Create(ent->org, tv1, tv0, ent->val[1]);
			mdl=LBXGL_Mdl_WrapModelInstance(ptr);
			LBXGL_BrushWorld_AddModelStateEntNum(
				btcl_brushWorld, mdl, ent->num);
		}
		return;
	}

	if(ent->flags&LBXGL_LFL_CHANGED)
	{
		dl=lbxWorldGetLight(btcl_brushWorld, ent->num);
		V3F_COPY(ent->org, dl->org);
		V3F_COPY(ent->dir, dl->dir);
		V3F_COPY(ent->clr, dl->clr);
		V2F_COPY(ent->val, dl->val);
		V2F_COPY(ent->coeff, dl->coeff);

		V3F_COPY(ent->mins, dl->mins);
		V3F_COPY(ent->maxs, dl->maxs);
		V3F_COPY(ent->boxval, dl->boxval);

		dl->angle=ent->angle;
		dl->flags=ent->flags;
		dl->style=ent->style;
		
		dl->texname=ent->texname;
		
		ent->flags&=~LBXGL_LFL_CHANGED;
	}
}

LBXGL_API void BTCL_SetupClientFrame(BTCL_World *wrl)
{
	BTCL_Entity *cur;
	BTCL_Light *lcur;
	float f, g;

	if(wrl->camnum<=0)
		return;

	f=btclClientTime()+wrl->sv_dtadjust;
	wrl->sv_time=f;	//estimated time on server

	cur=wrl->ents;
	while(cur)
	{
		BTCL_SetupClientEntFrame(wrl, cur);
		cur=cur->next;
	}

	lcur=wrl->lights;
	while(lcur)
	{
		BTCL_SetupClientLightFrame(wrl, lcur);
		lcur=lcur->next;
	}
}

LBXGL_API void BTCL_SetupClientCamera(BTCL_World *wrl)
{
	PDGLUI_InputState *state;
	PDGLUI_Camera *cam;

	state=PDGLUI_GetState();
	cam=PDGLUI_GetCamera();

	if(wrl->viewflags&BT_EF2_CAM_2D)
	{
		cam->mode=PDGLUI_CAM_SOLID2D;
		V3F_COPY(wrl->view_mins, cam->mins2d);
		V3F_COPY(wrl->view_maxs, cam->maxs2d);
	}else
	{
		cam->mode=PDGLUI_CAM_PERSP;
		cam->fov=wrl->view_fov;
	}
}

LBXGL_API dyt BTCL_ComposeBrushDelta(BTCL_World *wrl, LBXGL_Brush *brush)
{
	dyt t, l, l1;
	int i;
	
	if(!brush->idnum)
	{
		btcl_brushWorld->map_stat_brushes++;
		i=btcl_brushWorld->map_stat_brushes;
		brush->idnum=i;
		brush->flags|=BTGE_BRFL_NEW;
	}
	
	if(brush->flags&BTGE_BRFL_DEL)
	{
		t=dylist2s("delbrush", dyint(brush->idnum));
		return(t);
	}

	l=NULL;
	for(i=0; i<brush->n_face; i++)
	{
		l1=NULL;
		t=dylist5s("norm",
			dyflonum(brush->norm[i*4+0]),
			dyflonum(brush->norm[i*4+1]),
			dyflonum(brush->norm[i*4+2]),
			dyflonum(brush->norm[i*4+3]));
		l1=dycons(t, l1);

		t=dylist5s("sdir",
			dyflonum(brush->sdir[i*4+0]),
			dyflonum(brush->sdir[i*4+1]),
			dyflonum(brush->sdir[i*4+2]),
			dyflonum(brush->sdir[i*4+3]));
		l1=dycons(t, l1);

		t=dylist5s("tdir",
			dyflonum(brush->tdir[i*4+0]),
			dyflonum(brush->tdir[i*4+1]),
			dyflonum(brush->tdir[i*4+2]),
			dyflonum(brush->tdir[i*4+3]));
		l1=dycons(t, l1);

		t=dylist2s("texname", dystring(brush->tex[i]));
		l1=dycons(t, l1);

		t=dycons2s("face", dyint(i), l1);
		l=dycons(t, l);
	}

	if(brush->flags&BTGE_BRFL_NEW)
	{
		t=dycons3s("newbrush",
			dyint(brush->se_owner->idnum),
			dyint(brush->idnum), l);
		brush->flags&=~BTGE_BRFL_NEW;
		return(t);
	}

	if(!l)return(NULL);
	t=dycons2s("bdelta", dyint(brush->idnum), l);
	return(t);
}

LBXGL_API dyt BTCL_ComposeWorldDelta(BTCL_World *wrl)
{
	LBXGL_Brush *cur;
	LBXGL_SEntity *scur;
	BTGE_SEntPair *kcur;
	dyt t, l, l1;
	int i;

	l=NULL;
 	
	cur=btcl_brushWorld->brush;
	while(cur)
	{
		if(!cur->idnum)
		{
			btcl_brushWorld->map_stat_brushes++;
			i=btcl_brushWorld->map_stat_brushes;
			cur->idnum=i;
			cur->flags|=BTGE_BRFL_NEW|BTGE_BRFL_DIRTY;
		}

		if(cur->flags&BTGE_BRFL_DIRTY)
		{
			t=BTCL_ComposeBrushDelta(wrl, cur);
			l=dycons(t, l);
//			t=dylist2s("bdelta", dyint(cur->idnum));

			cur->flags&=~BTGE_BRFL_DIRTY;
		}

		cur=cur->next;
	}

	scur=btcl_brushWorld->entity;
	while(scur)
	{
		if(!scur->idnum)
		{
			btcl_brushWorld->map_stat_ents++;
			i=btcl_brushWorld->map_stat_ents;
			scur->idnum=i;

			kcur=scur->keys; l1=NULL;
			while(kcur)
			{
				t=dylist3s("key",
					dystring(kcur->name),
					dystring(kcur->value));
				l1=dycons(t, l1);
				
				kcur->flags&=~BTGE_SEFL_DIRTY;
				kcur=kcur->next;
			}

			scur->flags&=~BTGE_SEFL_DIRTY;

			t=dycons2s("newsent", dyint(i), l1);
			l=dycons(t, l);

			scur=scur->next;
			continue;
		}

		if(scur->flags&BTGE_SEFL_DIRTY)
		{
//			btcl_brushWorld->map_stat_ents++;
//			i=btcl_brushWorld->map_stat_ents;
//			scur->idnum=i;

			kcur=scur->keys; l1=NULL;
			while(kcur)
			{
				t=dylist3s("key",
					dystring(kcur->name),
					dystring(kcur->value));
				l1=dycons(t, l1);
				
				kcur->flags&=~BTGE_SEFL_DIRTY;
				kcur=kcur->next;
			}

			scur->flags&=~BTGE_SEFL_DIRTY;

			t=dycons2s("sedelta", dyint(i), l1);
			l=dycons(t, l);

			scur=scur->next;
			continue;
		}

		scur=scur->next;
	}

	if(!l)return(NULL);
	
	t=dyconss("wdelta", l);
	printf("BTCL_ComposeWorldDelta\n");
	dysPrintln(t);

	return(t);
}

LBXGL_API dyt BTCL_ComposeUpdate(BTCL_World *wrl)
{
	float tv[3];
	PDGLUI_InputState *state;
	PDGLUI_Camera *cam;
	dyt t, lst;
	
	state=PDGLUI_GetState();
	cam=PDGLUI_GetCamera();
	
	lst=NULL;

	//frame delta time
	t=dylist2s("framedt", dyflonum(state->dt_f));
	lst=dycons(t, lst);

	//time client thinks it is on server
	t=dylist2s("svtime", dyflonum(btclTime(wrl)));
	lst=dycons(t, lst);

	//current time on client
	t=dylist2s("cltime", dyflonum(btclClientTime()));
	lst=dycons(t, lst);

	//mouse state
	t=dylist4s("mpos",
		dyint(state->mx),
		dyint(state->my),
		dyint(state->mb));
	lst=dycons(t, lst);

	//last mouse state
	t=dylist4s("lmpos",
		dyint(state->lmx),
		dyint(state->lmy),
		dyint(state->lmb));
	lst=dycons(t, lst);

	//impulse angle (mouse movement)
	t=dylist4s("iang",
		dyflonum(cam->iang[0]),
		dyflonum(cam->iang[1]),
		dyflonum(cam->iang[2]));
	lst=dycons(t, lst);

	//current view origin
	t=dylist4s("org",
		dyflonum(cam->org[0]),
		dyflonum(cam->org[1]),
		dyflonum(cam->org[2]));
	lst=dycons(t, lst);

	//current view angle
	t=dylist4s("ang",
		dyflonum(cam->ang[0]),
		dyflonum(cam->ang[1]),
		dyflonum(cam->ang[2]+90));
	lst=dycons(t, lst);

	//impulse velocity
	V3F_COPY(cam->ivel, tv);
	tv[2]=cam->izvel;	//require explicit up/down movement

	t=dylist4s("ivel",
		dyflonum(tv[0]/state->dt_f),
		dyflonum(tv[1]/state->dt_f),
		dyflonum(tv[2]/state->dt_f));
	lst=dycons(t, lst);


	t=dylist4s("morg",
		dyflonum(cam->morg[0]),
		dyflonum(cam->morg[1]),
		dyflonum(cam->morg[2]));
	lst=dycons(t, lst);

	t=dylist4s("mend",
		dyflonum(cam->mend[0]),
		dyflonum(cam->mend[1]),
		dyflonum(cam->mend[2]));
	lst=dycons(t, lst);

	t=dylist4s("mdir",
		dyflonum(cam->mdir[0]),
		dyflonum(cam->mdir[1]),
		dyflonum(cam->mdir[2]));
	lst=dycons(t, lst);

	t=dylist2s("impfl", dyint(wrl->impfl));
	lst=dycons(t, lst);

	t=BTCL_ComposeWorldDelta(wrl);
	if(t)lst=dycons(t, lst);

	t=dycons2s("impulse", dyint(wrl->camnum), lst);
	return(t);
}

LBXGL_API void BTCL_Draw2D(BTCL_World *wrl)
{
	char tb[256];
	float tv[3];
	BTCL_Entity *cur;
	char *s;
	float cr, cg, cb;
	float f, g;
	int w, h, wxs, wys, wxs2, wys2;
	int i;

	wxs=lbxgl_state->xs;
	wys=lbxgl_state->ys;
	wxs2=wxs/2; wys2=wys/2;

	if(wrl->viewflags&BT_EF2_ISDEAD)
	{
		glDisable(GL_TEXTURE_2D);
		glColor4f(0.25, 0.0, 0.0, 0.5);
		Draw_Square(-wxs2, -wys2, wxs, wys);
		glEnable(GL_TEXTURE_2D);
	}

	if(wrl->flash_time>0)
	{
		glDisable(GL_TEXTURE_2D);
		f=wrl->flash_time*wrl->flash_scale;
		cr=(wrl->flash_color&0x0000FF)/((float)0x0000FF);
		cg=(wrl->flash_color&0x00FF00)/((float)0x00FF00);
		cb=(wrl->flash_color&0xFF0000)/((float)0xFF0000);

		glColor4f(cr, cg, cb, f);
		Draw_Square(-wxs2, -wys2, wxs, wys);
		wrl->flash_time-=btclFrameTime(wrl);
		glEnable(GL_TEXTURE_2D);
	}

	if(wrl->cprint_msg)
	{
		w=0; h=0; i=0;
		s=wrl->cprint_msg;
		while(*s)
		{
			if(*s=='\n')
			{
				s++;
				if(i>w)w=i;
				i=0; h++;
				continue;
			}
			if(*s=='\t')
			{
				s++; i=(i+8)&(~7);
				continue;
			}
			i++; s++;
			if(i>w)w=i;
		}

		GfxFont_DrawString2(wrl->cprint_msg, -w*6, -6+(h*6),
			12, 12,  255, 255, 255, 255);
	
//		i=-strlen(wrl->cprint_msg)*6;
//		i=-128;
//		GfxFont_DrawString(wrl->cprint_msg, i, -6,
//			12, 12,  255, 255, 255, 255);

		if(wrl->cprint_time<btclClientTime())
			wrl->cprint_msg=NULL;
	}

	if(wrl->cprint_boxmsg)
	{
		w=0; h=0; i=0;
		s=wrl->cprint_boxmsg;
		while(*s)
		{
			if(*s=='\n')
			{
				s++;
				if(i>w)w=i;
				i=0; h++;
				continue;
			}
			if(*s=='\t')
			{
				s++; i=(i+8)&(~7);
				continue;
			}
			i++; s++;
			if(i>w)w=i;
		}

		Draw_FillSquareRGBA(-w*6-32, -6-(h*6)-32,
			w*12+64, h*12+64,  0, 0, 0, 255);
		GfxFont_DrawString2(wrl->cprint_boxmsg, -w*6, -6+(h*6),
			12, 12,  255, 255, 255, 255);
	
//		i=-strlen(wrl->cprint_msg)*6;
//		i=-128;
//		GfxFont_DrawString(wrl->cprint_msg, i, -6,
//			12, 12,  255, 255, 255, 255);

//		if(wrl->cprint_time<btclClientTime())
//			wrl->cprint_msg=NULL;
	}

	if(wrl->maxHP>0)
	{
		Draw_FillSquareRGBA(wxs2-10*16, wys2-2*16,
			10*16, 2*16,  0, 0, 0, 128);

		sprintf(tb, "HP=%d/%d\nMP=%d/%d",
			(int)(wrl->curHP), (int)(wrl->maxHP),
			(int)(wrl->curMP), (int)(wrl->maxMP));
		GfxFont_DrawString2(tb, wxs2-10*16, wys2-1*16,
			16, 16,  255, 255, 0, 255);
	}

	i=ConCmds_CvarGetNum("cl_showentids");
	if(i>0)
	{
		cur=wrl->ents;
		while(cur)
		{
//			GfxFont_DrawString(wrl->cprint_msg, i, -6,
//				12, 12,  255, 255, 255, 255);

			sprintf(tb, "%d", cur->entnum);
			UI_Camera_ProjectWorldToScreen(cur->org, tv);
			GfxFont_DrawString(tb, tv[0], tv[1],
				8, 8,  255, 255, 255, 255);

//			if(cur->entnum==entnum)
//				return(cur);
			cur=cur->next;
		}
	}
//	return(NULL);
}
