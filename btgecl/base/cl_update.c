#include <lbxgl2.h>
#include <btgecl.h>

BTCL_World *btcl_world;
PDGLUI_InputState *btcl_state;
PDGLUI_Camera *btcl_cam;
LBXGL_BrushWorld *btcl_brushWorld;
BTCL_MessageHandler *btcl_handler;
float btcl_dt;

void BTCL_Init(void)
{
	static int init=0;
	if(init)return;
	init=1;

	if(!btcl_state)btcl_state=PDGLUI_GetState();
	if(!btcl_cam)btcl_cam=PDGLUI_GetCamera();
	
	BTCL_ScriptInit();
}

/** return predicted server time */
float btclTime(BTCL_World *wrl)
	{ return(wrl->sv_time); }
float btclMsgTime(BTCL_World *wrl)
	{ return(wrl->msg_time); }

float btclFrameTime(BTCL_World *wrl)
	{ return(btcl_dt); }

float btclClientTime(void)
{
	float f;
//	f=PDGL_TimeMS()/1000.0;
	f=btcl_state->time_f;
	return(f);
}

BTCL_MessageHandler *BTCL_LookupHandler(dyt name)
{
	BTCL_MessageHandler *cur;

	cur=btcl_handler;
	while(cur)
	{
		if(cur->name==name)
			return(cur);
		cur=cur->next;
	}
	return(NULL);
}

BTCL_MessageHandler *BTCL_GetHandler(dyt name)
{
	BTCL_MessageHandler *tmp;

	tmp=BTCL_LookupHandler(name);
	if(tmp)return(tmp);
	
	tmp=gctalloc("btcl_messagehandler_t",
		sizeof(BTCL_MessageHandler));
	tmp->name=name;
	
	tmp->next=btcl_handler;
	btcl_handler=tmp;
	return(tmp);
}

BTCL_API BTCL_MessageHandler *BTCL_RegisterHandler(char *name,
	void (*fcn)(BTCL_World *wrl, dyt msg))
{
	BTCL_MessageHandler *tmp;

	tmp=BTCL_GetHandler(dysym(name));
	tmp->Handler=fcn;
	return(tmp);
}

BTCL_API BTCL_MessageHandler *BTCL_RegisterDyHandler(char *name,
	dyt fcn)
{
	BTCL_MessageHandler *tmp;

	tmp=BTCL_GetHandler(dysym(name));
	tmp->dyHandler=fcn;
	return(tmp);
}

BTCL_API BTCL_World *BTCL_NewWorld(void)
{
	BTCL_World *tmp;
	
	BTCL_Init();
	tmp=gctalloc("btcl_world_t", sizeof(BTCL_World));
	return(tmp);
}

BTCL_API void BTCL_BindWorld(BTCL_World *wrl)
{
	btcl_world=wrl;
}

BTCL_API void BTCL_BindBrushWorld(LBXGL_BrushWorld *wrl)
{
	btcl_brushWorld=wrl;
}

BTCL_API void BTCL_StepWorld(BTCL_World *wrl, float dt)
{
	btcl_dt=dt;
//	wrl->
}

BTCL_API void BTCL_LoadMapFile(BTCL_World *wrl, char *map)
{
	LBXGL_Brush *bcur;
	LBXGL_SEntity *scur;
	BTGE_SEntPair *kcur;

	if(wrl->mapname && !strcmp(wrl->mapname, map))
		return;

	wrl->mapname=dystrdup(map);
//	LBXGL_BrushMap_LoadFile(btcl_brushWorld, map);
	LBXGL_BrushMap_ClearMap(btcl_brushWorld, map);

#if 1
	//just loaded map, avoid client pushing a bunch of deltas.

	bcur=btcl_brushWorld->brush;
	while(bcur)
	{
		bcur->flags&=~BTGE_BRFL_DIRTY;
		bcur=bcur->next;
	}

	scur=btcl_brushWorld->entity;
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
}

BTCL_API void BTCL_LoadMap(BTCL_World *wrl, char *map)
{
	char tb[256];

//	btCvarSet("mapname", map);

	sprintf(tb, "maps/%s.map", map);
	BTCL_LoadMapFile(wrl, tb);
}

BTCL_API void BTCL_FlushWorld(BTCL_World *wrl)
{
	BTCL_Entity *cur, *nxt;
	BTCL_Light *lcur, *lnxt;

	BTGE_Voxel_WorldTeardown(btcl_brushWorld);

	PDGL_Sound_StopPlaying(-1);	//stop all sounds
	wrl->cprint_msg=NULL;
	wrl->cprint_boxmsg=NULL;
	wrl->cprint_boxname=NULL;
	wrl->cprint_boximg=NULL;

	wrl->mapname=NULL;
	wrl->bgm_name=NULL;
	wrl->bgm_id=0;
	wrl->camnum=0;
	wrl->guinum=0;

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

void BTCL_DecodeMsgOrg(BTCL_World *wrl, double *v, int nv, dyt msg)
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
		v[i]=wrl->cam_reforg[i]+dyfloatv(t);
	}
}

void BTCL_DecodeMsgOrg3(BTCL_World *wrl, double *v, dyt msg)
	{ BTCL_DecodeMsgOrg(wrl, v, 3, msg); }

void BTCL_DecodeBoneDeltaAttr(BTCL_World *wrl, BTCL_Entity *ent,
	int bone, dyt msg)
{
	dyt v;
	char *s;
	
	v=dycar(msg);
	if(v==dysym("org"))
	{
		ent->bone_fl[bone]|=1;
		BTCL_DecodeMsgVec3(ent->bone_org+(bone*3), msg);
		return;
	}
//	if(v==dysym("vel"))
//		{ BTCL_DecodeMsgVec3(ent->vel, msg); return; }

	if(v==dysym("rot"))
	{
		ent->bone_fl[bone]|=2;
		BTCL_DecodeMsgVec4(ent->bone_rot+(bone*4), msg);
		return;
	}
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
		ent->bone_fl=gcalloc(j*sizeof(int));
		ent->n_bones=j;
//		ent->bone_mask=0;
	}
	
	if(i>=ent->n_bones)
	{
		j=ent->n_bones;
		while(j<=i)j=j+(j>>1);
		ent->bone_org=gcrealloc(ent->bone_org, j*3*sizeof(float));
		ent->bone_rot=gcrealloc(ent->bone_rot, j*4*sizeof(float));
		ent->bone_fl=gcrealloc(ent->bone_rot, j*sizeof(int));
		ent->n_bones=j;
	}
	
//	ent->bone_mask|=(1<<i);
//	ent->bone_fl[i]|=1;
	
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
//	if(v==dysym("org"))
//		{ BTCL_DecodeMsgVec3(ent->org, msg); return; }
	if(v==dysym("org"))
		{ BTCL_DecodeMsgOrg3(wrl, ent->org, msg); return; }

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
		{ ent->wfrm=dyintv(dycadr(msg)); return; }
	if(v==dysym("fcfrm"))
		{ ent->fcfrm=dyintv(dycadr(msg)); return; }

	if(v==dysym("eff"))
		{ ent->effects=dyintv(dycadr(msg)); return; }
	if(v==dysym("eff2"))
		{ ent->effects2=dyintv(dycadr(msg)); return; }
	if(v==dysym("sentid"))
		{ ent->sentid=dyintv(dycadr(msg)); return; }

	if(v==dysym("bone"))
		{ BTCL_DecodeBoneDelta(wrl, ent, msg); return; }

	if(v==dysym("delbone"))
	{
		i=dyintv(dycadr(msg));
		if(i<0)
		{
			for(i=0; i<ent->n_bones; i++)
			{
				ent->bone_fl[i]=0;
			}
//			ent->bone_fl=NULL;
			return;
		}
		
		ent->bone_fl[i]=0;
//		ent->bone_mask&=~(1<<i);
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
//	if(v==dysym("org"))
//		{ BTCL_DecodeMsgVec3(ent->org, msg); return; }
	if(v==dysym("org"))
		{ BTCL_DecodeMsgOrg3(wrl, ent->org, msg); return; }

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

void BTCL_DecodeBDeltaFaceMsg(BTCL_World *wrl, dyt msg,
	LBXGL_Brush *brush, int fn)
{
	float tv[4];
	dyt v, c;
	int i, j;

	LBXGL_Brush_CheckExpandFaces(brush, fn+1);
	if(fn>=brush->n_face)brush->n_face=fn+1;

	v=dycar(msg);

	if(v==dysym("norm"))
	{
		BTCL_DecodeMsgVec4(tv, msg);
		V4F_COPY(tv, brush->norm+(fn*4));
		return;
	}

	if(v==dysym("sdir"))
	{
		BTCL_DecodeMsgVec4(tv, msg);
		V4F_COPY(tv, brush->sdir+(fn*4));
		return;
	}

	if(v==dysym("tdir"))
	{
		BTCL_DecodeMsgVec4(tv, msg);
		V4F_COPY(tv, brush->tdir+(fn*4));
		return;
	}

	if(v==dysym("texname"))
	{
		brush->tex[fn]=dystrdup(dystringv(dycadr(msg)));
		return;
	}
}

void BTCL_DecodeBDeltaPatchPts(BTCL_World *wrl, dyt msg,
	LBXGL_Brush *brush)
{
	BTGE_BrushPatchPoint *pta[256];
	float org[3];
	BTGE_BrushPatchPoint **ptb;
	BTGE_BrushPatchPoint *pt;
	dyt v, c, c2, v2, c3, v3;
	float f, g, h;
	char *tex, *s;
	int i, j, ny, nx;

	V3F_ZERO(org);
//	if(world->entity_psel)
//		BTGE_SEnt_GetVec3(world->entity_psel, "origin", org);

	c=dycdr(msg);

	ny=0; nx=0;
	while(c)
	{
		v=dycaar(c);

		if(v==dysym("pts"))
		{
			for(i=0; i<256; i++)
				pta[i]=NULL;

			c2=dycdar(c); i=0;
//			for(i=0; i<256; i++)
			while(c2 && (i<256))
			{
				v2=dycaar(c2);
				
				if(v2==dysym("point"))
				{
					pt=gctalloc("lbxgl_brushpatchpoint_t",
						sizeof(BTGE_BrushPatchPoint));
					
					c3=dycdar(c2);
					while(c3)
					{
						v3=dycaar(c3);

						if(v3==dysym("xyz"))
							{ BTCL_DecodeMsgVec3(pt->xyz, dycar(c3)); }
						if(v3==dysym("uv"))
							{ BTCL_DecodeMsgVec2(pt->uv, dycar(c3)); }
						if(v3==dysym("norm"))
							{ BTCL_DecodeMsgVec4(pt->norm, dycar(c3)); }
						if(v3==dysym("rgba"))
							{ BTCL_DecodeMsgVec4(pt->rgba, dycar(c3)); }
						
						c3=dycdr(c3);
					}
			
					pta[i]=pt;
					i++;
				}
				
				c2=dycdr(c2);
			}
		
			if(i>nx)nx=i;
		
			ptb=gcalloc((nx+1)*sizeof(BTGE_BrushPatchPoint **));		
			for(i=0; i<nx; i++)
				ptb[i]=pta[i];

			if((ny+1)>=brush->patch->m_pts)
			{
				i=brush->patch->m_pts;
				if((ny+1)>=i)i=i+(i>>1);
				brush->patch->pts=gcrealloc(brush->patch->pts,
					i*sizeof(BTGE_BrushPatchPoint **));
				brush->patch->m_pts=i;
			}
		
			i=ny++;
			brush->patch->pts[i]=ptb;
		}

		c=dycdr(c);
	}

	brush->patch->nx_pts=nx;
	brush->patch->ny_pts=ny;
	
	printf("BTCL_DecodeBDeltaPatchPts: %d %d\n", nx, ny);
}

void BTCL_DecodeBDeltaPatchMsg2(BTCL_World *wrl, dyt msg,
	LBXGL_Brush *brush)
{
	float tv[4];
	dyt v, c;
	int i, j;

	if(!brush)return;

	v=dycar(msg);

	if(v==dysym("sdir"))
	{
		BTCL_DecodeMsgVec4(tv, msg);
		V4F_COPY(tv, brush->patch->sdir);
		return;
	}

	if(v==dysym("tdir"))
	{
		BTCL_DecodeMsgVec4(tv, msg);
		V4F_COPY(tv, brush->patch->tdir);
		return;
	}

	if(v==dysym("texname"))
	{
		brush->patch->tex=dystrdup(dystringv(dycadr(msg)));
		return;
	}

	if(v==dysym("xoffs"))
		{ brush->patch->xoffs=dyfloatv(dycadr(msg)); return; }
	if(v==dysym("yoffs"))
		{ brush->patch->yoffs=dyfloatv(dycadr(msg)); return; }
	if(v==dysym("rot"))
		{ brush->patch->rot=dyfloatv(dycadr(msg)); return; }
	if(v==dysym("xscale"))
		{ brush->patch->xscale=dyfloatv(dycadr(msg)); return; }
	if(v==dysym("yscale"))
		{ brush->patch->yscale=dyfloatv(dycadr(msg)); return; }

	if(v==dysym("pts"))
	{
		BTCL_DecodeBDeltaPatchPts(wrl, msg, brush);
		return;
	}
}

void BTCL_DecodeBDeltaPatchMsg(BTCL_World *wrl, dyt msg,
	LBXGL_Brush *brush)
{
	BTGE_BrushPatch *ptmp;
	dyt v, c;
	int i, j;

	printf("BTCL_DecodeBDeltaPatchMsg: ");
	dysPrintln(msg);

	ptmp=gctalloc("lbxgl_brushpatch_t", sizeof(BTGE_BrushPatch));
	brush->patch=ptmp;

	i=16;
	ptmp->pts=gcalloc(i*sizeof(BTGE_BrushPatchPoint **));
	ptmp->nx_pts=0;
	ptmp->ny_pts=0;
	ptmp->m_pts=i;

	c=dycdr(msg);
	while(c)
	{
		BTCL_DecodeBDeltaPatchMsg2(wrl, dycar(c), brush);
		c=dycdr(c);
	}
}

void BTCL_DecodeBDeltaMeshMsg(BTCL_World *wrl, dyt msg,
	LBXGL_Brush *brush)
{
}

void BTCL_DecodeBDeltaMsg(BTCL_World *wrl, dyt msg,
	LBXGL_Brush *brush)
{
	float tv[4];
	dyt v, c;
	int i, j;

	if(!brush)return;

	v=dycar(msg);

	if(v==dysym("face"))
	{
		i=dyintv(dycadr(msg));
		c=dycddr(msg);
		while(c)
		{
			BTCL_DecodeBDeltaFaceMsg(wrl, dycar(c), brush, i);
			c=dycdr(c);
		}
		return;
	}

	if(v==dysym("patchdef"))
		{ BTCL_DecodeBDeltaPatchMsg(wrl, msg, brush); return; }
	if(v==dysym("meshdef"))
		{ BTCL_DecodeBDeltaMeshMsg(wrl, msg, brush); return; }
}

void BTCL_DecodeSEDeltaMsg(BTCL_World *wrl, dyt msg, LBXGL_SEntity *sent)
{
	float tv[4];
	dyt v, c;
	char *s0, *s1;
	int i, j;

	if(!sent)return;

	v=dycar(msg);

	if(v==dysym("key"))
	{
		s0=dystringv(dycadr(msg));
		s1=dystringv(dycaddr(msg));
//		LBXGL_SEnt_SetStr(sent, s0, s1);
//		LBXGL_SEnt_QuietSetStr(sent, s0, s1);
		LBXGL_SEnt_SetStrNoDirty(sent, s0, s1);
		return;
	}
}

void BTCL_DecodeRegionSEDeltaMsg(BTCL_World *wrl,
	BTGE_VoxelRegion *rgn, dyt msg)
{
	LBXGL_SEntity *sent;
	LBXGL_Brush *brush;
	char *s;
	dyt v, c;
	float f, g, h;
	float dt, clt;
	int i, j;

	i=dyintv(dycadr(msg));

//	sent=LBXGL_BrushWorld_GetSEntityId(btcl_brushWorld, i);
	sent=BTGE_VoxelRegion_GetSEntityId(btcl_brushWorld, rgn, i);

	if(sent->flags&BTGE_SEFL_DIRTY)
		return;

//	LBXGL_BrushWorld_DrawFlush(btcl_brushWorld);

	c=dycddr(msg);
	while(c)
	{
		BTCL_DecodeSEDeltaMsg(wrl, dycar(c), sent);
		c=dycdr(c);
	}
	return;
}

void BTCL_DecodeRegionBDelta(BTCL_World *wrl,
	BTGE_VoxelRegion *rgn, dyt msg)
{
	LBXGL_SEntity *sent;
	LBXGL_Brush *brush;
	char *s;
	dyt v, c;
	float f, g, h;
	float dt, clt;
	int i, j;

	i=dyintv(dycadr(msg));
	brush=BTGE_VoxelRegion_LookupBrush(btcl_brushWorld, rgn, i);
	if(!brush)
	{
		printf("BTCL_DecodeRegionBDelta: BDelta, bad brush %d\n", i);
		//make new brush?...
		return;
	}

//	LBXGL_BrushWorld_DrawFlush(btcl_brushWorld);
//	LBXGL_Brush_DestroyFaces(brush);
	LBXGL_BrushWorld_DrawFlushBrush(btcl_brushWorld, brush);
	brush->flags&=~(BTGE_BRFL_DIRTY|BTGE_BRFL_NEW);

	c=dycddr(msg);
	while(c)
	{
		BTCL_DecodeBDeltaMsg(wrl, dycar(c), brush);
		c=dycdr(c);
	}
	return;
}

void BTCL_DecodeRegionDeltaAttr(BTCL_World *wrl,
	BTGE_VoxelRegion *rgn, dyt msg)
{
	dyt v, c;
	int i, j;

	v=dycar(msg);

	if(v==dysym("sedelta"))
		{ BTCL_DecodeRegionSEDeltaMsg(wrl, rgn, msg); return; }
	if(v==dysym("bdelta"))
		{ BTCL_DecodeRegionBDelta(wrl, rgn, msg); return; }
}

void BTCL_DecodeRegionDeltaMsg(BTCL_World *wrl, dyt msg)
{
	BTGE_VoxelRegion *rgn;
	dyt c;
	int x, y, z, xs, ys, zs;
	int i;


	xs=BTGE_Voxel_GetWorldRegionXSizeVoxels(btcl_brushWorld);
	ys=BTGE_Voxel_GetWorldRegionYSizeVoxels(btcl_brushWorld);
	zs=BTGE_Voxel_GetWorldRegionZSizeVoxels(btcl_brushWorld);
	
	x=dyintv(dycadr(msg))*xs;
	y=dyintv(dycaddr(msg))*ys;
	z=dyintv(dycadddr(msg))*zs;
	
	rgn=BTGE_Voxel_GetWorldRegionFlags(btcl_brushWorld, x, y, z, 1);

//	printf("BTCL_DecodeLDelta: ");
//	dysPrintln(msg);
	
//	ent->time=btclMsgTime(wrl);
	
	c=dycddddr(msg);
	while(c)
	{
		BTCL_DecodeRegionDeltaAttr(wrl, rgn, dycar(c));
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

//	io[0]=ent->org[0]+(ent->mins[0]+ent->maxs[0])*0.5;
//	io[1]=ent->org[1]+(ent->mins[1]+ent->maxs[1])*0.5;
//	io[2]=ent->org[2]+(ent->mins[2]+ent->maxs[2])*0.5;

	io[0]=(ent->org[0]-wrl->cam_reforg[0])+(ent->mins[0]+ent->maxs[0])*0.5;
	io[1]=(ent->org[1]-wrl->cam_reforg[1])+(ent->mins[1]+ent->maxs[1])*0.5;
	io[2]=(ent->org[2]-wrl->cam_reforg[2])+(ent->mins[2]+ent->maxs[2])*0.5;

	if(chan&7)
	{
		if(ent->s_chan[chan&7]>0)
			PDGL_Sound_StopPlaying(ent->s_chan[chan&7]);
		ent->s_chan[chan&7]=0;
	}

//	V3F_SCALE(io, 1.0/40, io);
	V3F_SCALE(io, 1.0/32, io);
//	V3F_SCALE(ent->org, 1.0/40, io);
//	V3F_SCALE(ent->vel, 1.0/40, iv);
	V3F_SCALE(ent->vel, 1.0/32, iv);
	if(!(*name) || (*name=='-'))
		{ i=0; }
	else
		{ i=PDGL_Sound_PlayPointSample(name, fl, vol, 1.0, io, iv); }
	if(chan&7)
	{
		//stop prior sound on a given channel
//		if(ent->s_chan[chan&7]>0)
//			PDGL_Sound_StopPlaying(ent->s_chan[chan&7]);
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

//	io[0]=ent->org[0]+(ent->mins[0]+ent->maxs[0])*0.5;
//	io[1]=ent->org[1]+(ent->mins[1]+ent->maxs[1])*0.5;
//	io[2]=ent->org[2]+(ent->mins[2]+ent->maxs[2])*0.5;

	io[0]=(ent->org[0]-wrl->cam_reforg[0])+(ent->mins[0]+ent->maxs[0])*0.5;
	io[1]=(ent->org[1]-wrl->cam_reforg[1])+(ent->mins[1]+ent->maxs[1])*0.5;
	io[2]=(ent->org[2]-wrl->cam_reforg[2])+(ent->mins[2]+ent->maxs[2])*0.5;

	str=text;
	if(ctrl)
	{
		sprintf(tb, "%s %s", ctrl, text);
		str=tb;
	}

	if(chan&7)
	{
		if(ent->s_chan[chan&7]>0)
			PDGL_Sound_StopPlaying(ent->s_chan[chan&7]);
		ent->s_chan[chan&7]=0;
	}

//	V3F_SCALE(io, 1.0/40, io);
	V3F_SCALE(io, 1.0/32, io);
//	V3F_SCALE(ent->org, 1.0/40, io);
//	V3F_SCALE(ent->vel, 1.0/40, iv);
	V3F_SCALE(ent->vel, 1.0/32, iv);
//	i=PDGL_Sound_PlayPointSample(name, fl, vol, 1.0, io, iv);
//	i=PDGL_Sound_PlayPointSample(name, fl, vol, 1.0, io, iv);

//	i=PDGL_Voice_PointSpeak(voice, str, fl, vol, 1.0, io, iv);
	i=PDGL_Sound_PointSpeak(voice, str, fl, vol, 1.0, io, iv);

	if(chan&7)
	{
		//stop prior sound on a given channel
//		if(ent->s_chan[chan&7]>0)
//			PDGL_Sound_StopPlaying(ent->s_chan[chan&7]);
		ent->s_chan[chan&7]=i;
	}
}


void BTCL_DecodeParticleEffect(BTCL_World *wrl, dyt msg)
{
	double dorg[3];
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
//		else if(v==dysym("org"))
//			{ BTCL_DecodeMsgOrg3(wrl, dorg, l); }
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

void BTCL_DecodeBDelta(BTCL_World *wrl, dyt msg)
{
	LBXGL_SEntity *sent;
	LBXGL_Brush *brush;
	char *s;
	dyt v, c;
	float f, g, h;
	float dt, clt;
	int i, j;

	i=dyintv(dycadr(msg));
	brush=LBXGL_BrushWorld_LookupBrush(btcl_brushWorld, i);
	if(!brush)
	{
		printf("BTGE_DecodeClientDeltaMsg: BDelta, bad brush %d\n", i);
		//make new brush?...
		return;
	}

//	LBXGL_BrushWorld_DrawFlush(btcl_brushWorld);
//	LBXGL_Brush_DestroyFaces(brush);
	LBXGL_BrushWorld_DrawFlushBrush(btcl_brushWorld, brush);
	brush->flags&=~(BTGE_BRFL_DIRTY|BTGE_BRFL_NEW);

	c=dycddr(msg);
	while(c)
	{
		BTCL_DecodeBDeltaMsg(wrl, dycar(c), brush);
		c=dycdr(c);
	}
	return;
}

void BTCL_DecodeDelBrush(BTCL_World *wrl, dyt msg)
{
	LBXGL_SEntity *sent;
	LBXGL_Brush *brush;
	char *s;
	dyt v, c;
	float f, g, h;
	float dt, clt;
	int i, j;

//	LBXGL_BrushWorld_DrawFlush(btcl_brushWorld);
//	LBXGL_BrushWorld_DrawFlushBrush(btcl_brushWorld, brush);

	i=dyintv(dycadr(msg));
	brush=LBXGL_BrushWorld_LookupBrush(btcl_brushWorld, i);
	if(!brush)return;
	LBXGL_BrushWorld_DrawFlushBrush(btcl_brushWorld, brush);
	brush->flags|=BTGE_BRFL_DEL;
	return;
}

void BTCL_DecodeNewBrush(BTCL_World *wrl, dyt msg)
{
	LBXGL_SEntity *sent;
	LBXGL_Brush *brush;
	char *s;
	dyt v, c;
	float f, g, h;
	float dt, clt;
	int i, j;

	i=dyintv(dycadr(msg));		//sentid
	j=dyintv(dycaddr(msg));		//brushid
	brush=LBXGL_BrushWorld_LookupBrush(btcl_brushWorld, j);
	if(!brush)
	{
		brush=LBXGL_BrushWorld_CreateBrushId(btcl_brushWorld, i, j);
	}
	if(!brush)
	{
		printf("BTGE_DecodeClientDeltaMsg: "
			"Newbrush failed sent=%d brush=%d\n", i, j);
		return;
	}

	LBXGL_BrushWorld_DrawFlush(btcl_brushWorld);

//	LBXGL_Brush_DestroyFaces(brush);

	c=dycdddr(msg);
	while(c)
	{
		BTCL_DecodeBDeltaMsg(wrl, dycar(c), brush);
		c=dycdr(c);
	}

//	LBXGL_BrushWorld_DrawFlushBrush(btcl_brushWorld, brush);
//	brush->flags&=~(BTGE_BRFL_DIRTY|BTGE_BRFL_NEW);

	return;
}

void BTCL_DecodeNewSEnt(BTCL_World *wrl, dyt msg)
{
	LBXGL_SEntity *sent;
	LBXGL_Brush *brush;
	char *s;
	dyt v, c;
	float f, g, h;
	float dt, clt;
	int i, j;

	i=dyintv(dycadr(msg));

#if 0
	sent=LBXGL_BrushWorld_LookupSEntityId(btcl_brushWorld, i);
	if(sent)
	{
		printf("BTGE_DecodeClientDeltaMsg: "
			"NewSEnt: Already Exists %d\n", i);
		return;
	}
#endif

	sent=LBXGL_BrushWorld_GetSEntityId(btcl_brushWorld, i);

	LBXGL_BrushWorld_DrawFlush(btcl_brushWorld);

	c=dycddr(msg);
	while(c)
	{
		BTCL_DecodeSEDeltaMsg(wrl, dycar(c), sent);
		c=dycdr(c);
	}
	return;
}

void BTCL_DecodeSEDelta(BTCL_World *wrl, dyt msg)
{
	LBXGL_SEntity *sent;
	LBXGL_Brush *brush;
	char *s;
	dyt v, c;
	float f, g, h;
	float dt, clt;
	int i, j;

	i=dyintv(dycadr(msg));

#if 0
	sent=LBXGL_BrushWorld_LookupSEntityId(btcl_brushWorld, i);
	if(!sent)
	{
		printf("BTGE_DecodeSEDelta: "
			"SEDelta: Doesn't Exist %d\n", i);
		return;
	}
#endif

	sent=LBXGL_BrushWorld_GetSEntityId(btcl_brushWorld, i);

	if(sent->flags&BTGE_SEFL_DIRTY)
		return;

//	LBXGL_BrushWorld_DrawFlush(btcl_brushWorld);

	c=dycddr(msg);
	while(c)
	{
		BTCL_DecodeSEDeltaMsg(wrl, dycar(c), sent);
		c=dycdr(c);
	}
	return;
}

void BTCL_DecodePushSound(BTCL_World *wrl, dyt msg)
{
	dyt n, ty, d;
	
	n=dycadr(msg);
	ty=dycaddr(msg);
	d=dycadddr(msg);
	
	if(dyByteArrayP(d))
	{
		PDGL_Sound_LoadDecodeSampleBuffer(dystringv(n), dystringv(ty),
			dyByteArrayData(d), dyByteArraySize(d));
	}
}

void BTCL_DecodePushImage(BTCL_World *wrl, dyt msg)
{
	dyt n, ty, d;
	
	n=dycadr(msg);
	ty=dycaddr(msg);
	d=dycadddr(msg);
	
	if(dyByteArrayP(d))
	{
		LBXGL_Texture_LoadImageBufferSize(dystringv(n), dystringv(ty),
			dyByteArrayData(d), dyByteArraySize(d), NULL, NULL);
	}
}

void BTCL_DecodeVoxelDeltaMsg(BTCL_World *wrl, dyt msg)
{
	BTGE_Voxel_WorldDecodeDelta(btcl_brushWorld, msg);
}

void BTCL_DecodePushFile(BTCL_World *wrl, dyt msg)
{
	BTGE_FileCache_ClientDecodePushFile(btcl_brushWorld, msg);
}

void BTCL_DecodeUpdateMsg(BTCL_World *wrl, dyt msg)
{
	float tv[4];
	LBXGL_SEntity *sent;
	LBXGL_Brush *brush;
	BTCL_MessageHandler *hdlr;
	char *s;
	dyt v;
	float f, g, h;
	float dt, clt;
	int i, j;

	if(!dyconsp(msg))
	{
		printf("Bad Message: ");
		dysPrintln(msg);
		return;
	}
	
	v=dycar(msg);

	hdlr=BTCL_LookupHandler(v);
	if(hdlr)
	{
		if(hdlr->Handler)
		{
			hdlr->Handler(wrl, msg);
			return;
		}
		if(hdlr->dyHandler)
		{
			BSVM_CallFuncObj(NULL, hdlr->dyHandler,
				dylist2((dyt)wrl, msg));
//			hdlr->Handler(wrl, msg);
			return;
		}
	}

	if(v==dysym("delta"))
		{ BTCL_DecodeDelta(wrl, msg); return; }
	if(v==dysym("delent"))
		{ BTCL_DeleteEntity(wrl, dyintv(dycadr(msg))); return; }
	if(v==dysym("ldelta"))
		{ BTCL_DecodeLDelta(wrl, msg); return; }
	if(v==dysym("wdelta"))
		{ BTCL_DecodeWDelta(wrl, msg); return; }
	if(v==dysym("uidelta"))
		{ BTCL_DecodeUIDelta(wrl, msg); return; }
	if(v==dysym("delui"))
		{ BTCL_DecodeDeleteUI(wrl, msg); return; }
	if(v==dysym("voxdelta"))
		{ BTCL_DecodeVoxelDeltaMsg(wrl, msg); return; }

#if 1
	if(v==dysym("bdelta"))
		{ BTCL_DecodeBDelta(wrl, msg); return; }
	if(v==dysym("delbrush"))
		{ BTCL_DecodeDelBrush(wrl, msg); return; }
	if(v==dysym("newbrush"))
		{ BTCL_DecodeNewBrush(wrl, msg); return; }
	if(v==dysym("newsent"))
		{ BTCL_DecodeNewSEnt(wrl, msg); return; }
	if(v==dysym("sedelta"))
		{ BTCL_DecodeSEDelta(wrl, msg); return; }
#endif

	if(v==dysym("sound"))
		{ BTCL_DecodeSound(wrl, msg); return; }

	if(v==dysym("speak"))
		{ BTCL_DecodeSpeak(wrl, msg); return; }

	if(v==dysym("particles"))
		{ BTCL_DecodeParticleEffect(wrl, msg); return; }

	if(v==dysym("push_sound"))
		{ BTCL_DecodePushSound(wrl, msg); return; }
	if(v==dysym("push_image"))
		{ BTCL_DecodePushImage(wrl, msg); return; }
	if(v==dysym("push_file"))
		{ BTCL_DecodePushFile(wrl, msg); return; }

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

	if(v==dysym("guinum"))
	{
		wrl->guinum=dyintv(dycadr(msg));
		printf("GUI: %d\n", wrl->guinum);
//		PDGLUI_HideMenu(NULL);	//avoid lingering menus
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
		{
			wrl->cprint_boxmsg=NULL;
			wrl->cprint_boxname=NULL;
			wrl->cprint_boximg=NULL;
		}
//		wrl->cprint_time=btclClientTime()+5;
//		printf("centerprint: %s\n", wrl->cprint_msg);
		return;
	}

	if(v==dysym("centerprint_boxname"))
	{
		wrl->cprint_boxname=dystringv(dycadr(msg));
	}
	if(v==dysym("centerprint_boximg"))
	{
		wrl->cprint_boximg=dystringv(dycadr(msg));
	}

	if(v==dysym("centerprint_eval"))
	{
		wrl->cprint_eval=dystringv(dycadr(msg));
		if(wrl->cprint_eval && *wrl->cprint_eval)
		{
			wrl->cprint_eval_val=
				btScriptEvalFromServer(wrl->cprint_eval);
		}else
		{
			wrl->cprint_eval_val=NULL;
		}
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

	if(v==dysym("stuffcode"))
	{
		btScriptEvalFromServer(dystringv(dycadr(msg)));
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

	if(v==dysym("ap"))
	{
		wrl->curAP=dyintv(dycadr(msg));
		wrl->maxAP=dyintv(dycaddr(msg));
		return;
	}

	if(v==dysym("inven"))
	{
//		wrl->curMP=dyintv(dycadr(msg));
//		wrl->maxMP=dyintv(dycaddr(msg));
		return;
	}

	if(v==dysym("reforg"))
	{
		wrl->cam_reforg[0]=dyintv(dycadr(msg));
		wrl->cam_reforg[1]=dyintv(dycaddr(msg));
		wrl->cam_reforg[2]=dyintv(dycadddr(msg));
		wrl->cam_rebase=1;
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

BTCL_API void BTCL_DecodeUpdate(BTCL_World *wrl, dyt msg)
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
