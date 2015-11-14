#include <btgesv.h>

BTGE_BrushWorld *btge_world;
dyt btge_msgqueue=NULL;				//messages queued for client
BTGE_ClientInfo *btge_client=NULL;	//client list
BTGE_MessageHandler *btge_msghandler;

BTGE_ClientInfo *BTGE_LookupClientInfo(int clnum)
{
	BTGE_ClientInfo *cur;
	
	cur=btge_client;
	while(cur)
	{
		if(cur->clnum==clnum)
			return(cur);
		cur=cur->next;
	}
	return(NULL);
}

BTGE_ClientInfo *BTGE_GetClientInfo(int clnum)
{
	BTGE_ClientInfo *tmp;

	tmp=BTGE_LookupClientInfo(clnum);
	if(tmp)return(tmp);
	
	tmp=gctalloc("btge_clientinfo_t", sizeof(BTGE_ClientInfo));
	tmp->clnum=clnum;

	tmp->next=btge_client;
	btge_client=tmp;
	
	tmp->deltamap_ent=gcalloc(4096);
	tmp->deltamap_brush=gcalloc(4096);
	tmp->deltamap_light=gcalloc(4096);
	
	return(tmp);
}

BTGE_MessageHandler *BTGE_LookupMessageHandler(dyt name)
{
	BTGE_MessageHandler *cur;

	cur=btge_msghandler;
	while(cur)
	{
		if(cur->name==name)
			return(cur);
		cur=cur->next;
	}
	return(NULL);
}

BTGE_MessageHandler *BTGE_GetMessageHandler(dyt name)
{
	BTGE_MessageHandler *tmp;

	tmp=BTGE_LookupMessageHandler(name);
	if(tmp)return(tmp);
	
	tmp=gctalloc("btge_messagehandler_t",
		sizeof(BTGE_MessageHandler));
	tmp->name=name;
	
	tmp->next=btge_msghandler;
	btge_msghandler=tmp;
	return(tmp);
}

BTSV_API BTGE_MessageHandler *BTGE_RegisterMessageHandler(char *name,
	void (*fcn)(btEntity ent, dyt msg))
{
	BTGE_MessageHandler *tmp;

	tmp=BTGE_GetMessageHandler(dysym(name));
	tmp->Handler=fcn;
	return(tmp);
}

BTSV_API BTGE_MessageHandler *BTGE_RegisterDyMessageHandler(
	char *name, dyt fcn)
{
	BTGE_MessageHandler *tmp;

	tmp=BTGE_GetMessageHandler(dysym(name));
	tmp->dyHandler=fcn;
	return(tmp);
}

dyt BTGE_DeltaNamedVec3(char *name, vec3 v)
{
	dyt t, t0, t1, t2;

	t0=dyflonum(v3x(v));
	t1=dyflonum(v3y(v));
	t2=dyflonum(v3z(v));
	t=dylist4s(name, t0, t1, t2);
	return(t);
}

dyt BTGE_EntityDelta(btEntity ent, btEntity clent)
{
//	BTGE_Inventory *ivcur;
	s64 reforg[3];
	float tv0[4];
	dyt lst, t, t0, t1, t2, t3;
	int i, fdm;
	
	//force delta message
	if(clent && clent->client)
	{
		i=ent->entnum;
		fdm=!(clent->client->deltamap_ent[i>>3]&(1<<(i&7)));
		clent->client->deltamap_ent[i>>3]|=(1<<(i&7));
		
		reforg[0]=clent->client->reforg[0];
		reforg[1]=clent->client->reforg[1];
		reforg[2]=clent->client->reforg[2];
	}else
	{
		fdm=0;
		reforg[0]=0;
		reforg[1]=0;
		reforg[2]=0;
	}
	
	lst=NULL;
	if((v3ddist(ent->origin, ent->oldorigin)>1) ||
		(ent->flags&BT_FL_CLIENT) || fdm)
	{
		tv0[0]=v3dx(ent->origin)-reforg[0];
		tv0[1]=v3dy(ent->origin)-reforg[1];
		tv0[2]=v3dz(ent->origin)-reforg[2];

#if 0
		if((ent==clent) && ((reforg[0]+reforg[1]+reforg[2])!=0))
		{
			printf("Client RelOrg=(%g %g %g)\n",
				tv0[0], tv0[1], tv0[2]);
		}
#endif

//		ent->oldorigin=ent->origin;

		t0=dyflonum(tv0[0]);
		t1=dyflonum(tv0[1]);
		t2=dyflonum(tv0[2]);

//		t0=dyfloat(v3x(ent->origin));
//		t1=dyfloat(v3y(ent->origin));
//		t2=dyfloat(v3z(ent->origin));

		t=dylist4s("org", t0, t1, t2);
		lst=dycons(t, lst);
	}

	if((v3ddist(ent->origin2, ent->oldorigin2)>1) ||
		(ent->flags&BT_FL_CLIENT) || fdm)
	{
//		ent->oldorigin=ent->origin;
		t0=dyflonum(v3dx(ent->origin2)-reforg[0]);
		t1=dyflonum(v3dy(ent->origin2)-reforg[1]);
		t2=dyflonum(v3dz(ent->origin2)-reforg[2]);

		t=dylist4s("org2", t0, t1, t2);
		lst=dycons(t, lst);
	}

	if((v3dist(ent->velocity, ent->oldvelocity)>1) ||
		(ent->flags&BT_FL_CLIENT) || fdm)
	{
//		ent->oldvelocity=ent->velocity;
		t0=dyflonum(v3x(ent->velocity));
		t1=dyflonum(v3y(ent->velocity));
		t2=dyflonum(v3z(ent->velocity));
		t=dylist4s("vel", t0, t1, t2);
		lst=dycons(t, lst);
	}

	if(!btGetBModel(ent) && ((v3dist(ent->angles, ent->oldangles)>1) ||
		(ent->flags&BT_FL_CLIENT) || fdm))
	{
//		ent->oldangles=ent->angles;
		t0=dyflonum(v3x(ent->angles));
		t1=dyflonum(v3y(ent->angles));
		t2=dyflonum(v3z(ent->angles)+90);
		t=dylist4s("ang", t0, t1, t2);
		lst=dycons(t, lst);
	}

	if(btGetBModel(ent) &&
		(v3dist(ent->angles2, ent->oldangles2)>1 || fdm))
	{
//		ent->oldangles2=ent->angles2;
		t0=dyflonum(v3x(ent->angles2));
		t1=dyflonum(v3y(ent->angles2));
//		t2=dyflonum(v3z(ent->angles2)+90);
		t2=dyflonum(v3z(ent->angles2));
		t=dylist4s("ang", t0, t1, t2);
		lst=dycons(t, lst);
	}

	if((v4dist(ent->rot, ent->oldrot)>1) || fdm)
	{
//		ent->oldmins=ent->mins;
		t0=dyflonum(qx(ent->rot));
		t1=dyflonum(qy(ent->rot));
		t2=dyflonum(qz(ent->rot));
		t3=dyflonum(qw(ent->rot));
		t=dylist5s("rot", t0, t1, t2, t3);
		lst=dycons(t, lst);
	}

	if((v4dist(ent->rot2, ent->oldrot2)>1) || fdm)
	{
//		ent->oldmins=ent->mins;
		t0=dyflonum(qx(ent->rot2));
		t1=dyflonum(qy(ent->rot2));
		t2=dyflonum(qz(ent->rot2));
		t3=dyflonum(qw(ent->rot2));
		t=dylist5s("rot2", t0, t1, t2, t3);
		lst=dycons(t, lst);
	}

	if((v4dist(ent->rotvel, ent->oldrotvel)>1) || fdm)
	{
//		ent->oldmins=ent->mins;
		t0=dyflonum(qx(ent->rotvel));
		t1=dyflonum(qy(ent->rotvel));
		t2=dyflonum(qz(ent->rotvel));
		t3=dyflonum(qw(ent->rotvel));
		t=dylist5s("rotvel", t0, t1, t2, t3);
		lst=dycons(t, lst);
	}

	if((v4dist(ent->rotvel2, ent->oldrotvel2)>1) || fdm)
	{
//		ent->oldmins=ent->mins;
		t0=dyflonum(qx(ent->rotvel2));
		t1=dyflonum(qy(ent->rotvel2));
		t2=dyflonum(qz(ent->rotvel2));
		t3=dyflonum(qw(ent->rotvel2));
		t=dylist5s("rotvel2", t0, t1, t2, t3);
		lst=dycons(t, lst);
	}

	if((v3dist(ent->mins, ent->oldmins)>1) || fdm)
	{
//		ent->oldmins=ent->mins;
		t0=dyflonum(v3x(ent->mins));
		t1=dyflonum(v3y(ent->mins));
		t2=dyflonum(v3z(ent->mins));
		t=dylist4s("mins", t0, t1, t2);
		lst=dycons(t, lst);
	}

	if((v3dist(ent->maxs, ent->oldmaxs)>1) || fdm)
	{
//		ent->oldmaxs=ent->maxs;
		t0=dyflonum(v3x(ent->maxs));
		t1=dyflonum(v3y(ent->maxs));
		t2=dyflonum(v3z(ent->maxs));
		t=dylist4s("maxs", t0, t1, t2);
		lst=dycons(t, lst);
	}

	if((v3dist(ent->vieworg, ent->oldvieworg)>1) || fdm)
	{
//		ent->oldvieworg=ent->vieworg;
		t0=dyflonum(v3x(ent->vieworg));
		t1=dyflonum(v3y(ent->vieworg));
		t2=dyflonum(v3z(ent->vieworg));
		t=dylist4s("vorg", t0, t1, t2);
		lst=dycons(t, lst);
	}

	if((ent->model!=ent->oldmodel) || fdm)
	{
//		ent->oldmodel=ent->model;
		t=dylist2s("mdl", dystring(ent->model));
		lst=dycons(t, lst);
	}

	if((ent->weaponmodel!=ent->oldweaponmodel) || fdm)
	{
//		ent->oldweaponmodel=ent->weaponmodel;
		t=dylist2s("wmdl", dystring(ent->weaponmodel));
		lst=dycons(t, lst);
	}

	if((ent->anim!=ent->oldanim) || fdm)
	{
//		ent->oldanim=ent->anim;
		t=dylist2s("anim", dystring(ent->anim));
		lst=dycons(t, lst);
	}

	if((ent->weaponanim!=ent->oldweaponanim) || fdm)
	{
//		ent->oldweaponanim=ent->weaponanim;
		t=dylist2s("wanim", dystring(ent->weaponanim));
		lst=dycons(t, lst);
	}

	if((ent->frame!=ent->oldframe) || fdm)
	{
//		ent->oldframe=ent->frame;
		t=dylist2s("frm", dyint(ent->frame));
		lst=dycons(t, lst);
	}

	if((ent->weaponframe!=ent->oldweaponframe) || fdm)
	{
//		ent->oldweaponframe=ent->weaponframe;
		t=dylist2s("wfrm", dyint(ent->weaponframe));
		lst=dycons(t, lst);
	}

	if((ent->effects!=ent->oldeffects) || fdm)
	{
//		ent->oldeffects=ent->effects;
		t=dylist2s("eff", dyint(ent->effects));
		lst=dycons(t, lst);
	}

	if((ent->effects2!=ent->oldeffects2) || fdm)
	{
//		i=ent->oldeffects2;
//		ent->oldeffects2=ent->effects2;
		t=dylist2s("eff2", dyint(ent->effects2));
		lst=dycons(t, lst);
	}

	if(!(ent->deltaFlags&BT_DTFL_SENTID) || fdm)
	{
		if(ent->sent)
		{
			t=dylist2s("sentid", dyint(ent->sent->idnum));
			lst=dycons(t, lst);
		}
	}

#if 0
	if(ent->flags&BT_FL_CLIENT)
	{
		ivcur=ent->items;
		while(ivcur)
		{
			t=dylist4s("inventory",
				dystring(ivcur->classname),
				dyint(ivcur->count),
				dyint(ivcur->slot));
			lst=dycons(t, lst);

			ivcur=ivcur->next;
		}

		t=dylist3s("hp", dyint(ent->health), dyint(ent->maxHealth));
		lst=dycons(t, lst);

		t=dylist3s("mp", dyint(ent->curMP), dyint(ent->maxMP));
		lst=dycons(t, lst);

		t=dylist3s("ap", dyint(ent->curArmor), dyint(ent->maxArmor));
		lst=dycons(t, lst);
	}
#endif

	for(i=0; i<4; i++)
	{
		if((ent->teamColor[i]!=ent->oldTeamColor[i]) || fdm)
		{
			t=dylist3s("teamcolor", dyint(i), dyint(ent->teamColor[i]));
			lst=dycons(t, lst);
		}

		if((ent->teamSkin[i]!=ent->oldTeamSkin[i]) || fdm)
		{
			t=dylist3s("teamskin", dyint(i), dyint(ent->teamSkin[i]));
			lst=dycons(t, lst);
		}
	}

	if(!lst)return(NULL);

	t=dycons2s("delta", dyint(ent->entnum), lst);
	return(t);
}

void BTGE_EntityPostDelta(btEntity ent)
{
	int i;

	if(v3ddist(ent->origin, ent->oldorigin)>1)
		ent->oldorigin=ent->origin;
	if(v3ddist(ent->origin2, ent->oldorigin2)>1)
		ent->oldorigin2=ent->origin2;
	if(v3dist(ent->velocity, ent->oldvelocity)>1)
		ent->oldvelocity=ent->velocity;
	if(v3dist(ent->angles, ent->oldangles)>1)
		ent->oldangles=ent->angles;
	if(v3dist(ent->angles2, ent->oldangles2)>1)
		ent->oldangles2=ent->angles2;
	if(v4dist(ent->rot, ent->oldrot)>1)
		ent->oldrot=ent->rot;
	if(v4dist(ent->rot2, ent->oldrot2)>1)
		ent->oldrot2=ent->rot2;
	if(v4dist(ent->rotvel, ent->oldrotvel)>1)
		ent->oldrotvel=ent->rotvel;
	if(v4dist(ent->rotvel2, ent->oldrotvel2)>1)
		ent->oldrotvel2=ent->rotvel2;
	if(v3dist(ent->mins, ent->oldmins)>1)
		ent->oldmins=ent->mins;
	if(v3dist(ent->maxs, ent->oldmaxs)>1)
		ent->oldmaxs=ent->maxs;
	if(v3dist(ent->vieworg, ent->oldvieworg)>1)
		ent->oldvieworg=ent->vieworg;

	ent->oldmodel=ent->model;
	ent->oldweaponmodel=ent->weaponmodel;
	ent->oldanim=ent->anim;
	ent->oldweaponanim=ent->weaponanim;
	ent->oldframe=ent->frame;
	ent->oldweaponframe=ent->weaponframe;
	ent->oldeffects=ent->effects;
	ent->oldeffects2=ent->effects2;

	ent->deltaFlags|=BT_DTFL_STATICMASK;

	for(i=0; i<4; i++)
	{
		ent->oldTeamColor[i]=ent->teamColor[i];
		ent->oldTeamSkin[i]=ent->teamSkin[i];
	}
}

void BTGE_EntityClearDelta(btEntity ent)
{
	int i;

	ent->oldorigin=vec3d(0,0,0);
	ent->oldorigin2=vec3d(0,0,0);
	ent->oldvelocity=vec3(0,0,0);
	ent->oldangles=vec3(0,0,0);
	ent->oldangles2=vec3(0,0,0);
	ent->oldmins=vec3(0,0,0);
	ent->oldmaxs=vec3(0,0,0);
	ent->oldvieworg=vec3(0,0,0);
	ent->oldmodel=NULL;
	ent->oldweaponmodel=NULL;
	ent->oldanim=NULL;
	ent->oldweaponanim=NULL;
	ent->oldframe=0;
	ent->oldweaponframe=0;
	ent->oldeffects=0;
	ent->oldeffects2=0;
	ent->deltaFlags=0;

	for(i=0; i<4; i++)
	{
		ent->oldTeamColor[i]=0;
		ent->oldTeamSkin[i]=0;
	}
}

dyt BTGE_LightDelta(btLight light, btEntity clent)
{
	s64 reforg[3];
	dyt lst, t, t0, t1, t2;
	int i, fdm;
	
	//force delta message
	fdm=0;
	if(clent && clent->client)
	{
		i=light->num;
		fdm=!(clent->client->deltamap_light[i>>3]&(1<<(i&7)));
		clent->client->deltamap_light[i>>3]|=(1<<(i&7));

		reforg[0]=clent->client->reforg[0];
		reforg[1]=clent->client->reforg[1];
		reforg[2]=clent->client->reforg[2];
	}else
	{
		reforg[0]=0;
		reforg[1]=0;
		reforg[2]=0;
	}
	

	lst=NULL;

	if(!(light->flags&BTGE_LFL_CHANGED) && !fdm)
		return(NULL);

	if(!light->oldval)
	{
		light->oldval=gctalloc("btge_light_t", sizeof(BTGE_Light));
	}

//	if(1)
	if((V3F_DIST(light->org, light->oldval->org)>0.1) || fdm)
	{
		t0=dyflonum(light->org[0]-reforg[0]);
		t1=dyflonum(light->org[1]-reforg[1]);
		t2=dyflonum(light->org[2]-reforg[2]);
		t=dylist4s("org", t0, t1, t2);
		lst=dycons(t, lst);
	}

//	if(1)
	if((V3F_DIST(light->dir, light->oldval->dir)>0.1) || fdm)
	{
		t0=dyflonum(light->dir[0]);
		t1=dyflonum(light->dir[1]);
		t2=dyflonum(light->dir[2]);
		t=dylist4s("dir", t0, t1, t2);
		lst=dycons(t, lst);
	}

//	if(1)
	if((V3F_DIST(light->clr, light->oldval->clr)>0.1) || fdm)
	{
		t0=dyflonum(light->clr[0]);
		t1=dyflonum(light->clr[1]);
		t2=dyflonum(light->clr[2]);
		t=dylist4s("clr", t0, t1, t2);
		lst=dycons(t, lst);
	}

//	if(1)
	if((V2F_DIST(light->val, light->oldval->val)>0.1) || fdm)
	{
		t0=dyflonum(light->val[0]);
		t1=dyflonum(light->val[1]);
		t=dylist3s("val", t0, t1);
		lst=dycons(t, lst);
	}

//	if(1)
	if((V3F_DIST(light->coeff, light->oldval->coeff)>0.1) || fdm)
	{
		t0=dyflonum(light->coeff[0]);
		t1=dyflonum(light->coeff[1]);
		t2=dyflonum(light->coeff[2]);
		t=dylist4s("coeff", t0, t1, t2);
		lst=dycons(t, lst);
	}

//	if(1)
	if((V3F_DIST(light->mins, light->oldval->mins)>0.1) || fdm)
	{
		t0=dyflonum(light->mins[0]);
		t1=dyflonum(light->mins[1]);
		t2=dyflonum(light->mins[2]);
		t=dylist4s("mins", t0, t1, t2);
		lst=dycons(t, lst);
	}

//	if(1)
	if((V3F_DIST(light->maxs, light->oldval->maxs)>0.1) || fdm)
	{
		t0=dyflonum(light->maxs[0]);
		t1=dyflonum(light->maxs[1]);
		t2=dyflonum(light->maxs[2]);
		t=dylist4s("maxs", t0, t1, t2);
		lst=dycons(t, lst);
	}
//	if(1)
	if((V3F_DIST(light->boxval, light->oldval->boxval)>0.1) || fdm)
	{
		t0=dyflonum(light->boxval[0]);
		t1=dyflonum(light->boxval[1]);
		t2=dyflonum(light->boxval[2]);
		t=dylist4s("boxval", t0, t1, t2);
		lst=dycons(t, lst);
	}

//	if(light->texname)
	if((light->texname!=light->oldval->texname) || fdm)
	{
		t=dylist2s("texname", dystring(light->texname));
		lst=dycons(t, lst);
	}

//	if(1)
	if((light->flags!=light->oldval->flags) || fdm)
	{
		t=dylist2s("fl", dyint(light->flags));
		lst=dycons(t, lst);
	}

//	if(1)
	if((light->style!=light->oldval->style) || fdm)
	{
		t=dylist2s("st", dyint(light->style));
		lst=dycons(t, lst);
	}

	if(!lst)return(NULL);

	t=dycons2s("ldelta", dyint(light->num), lst);
	return(t);
}

void BTGE_LightPostDelta(btLight light)
{
	light->flags&=~BTGE_LFL_CHANGED;

	if(!light->oldval)
	{
		light->oldval=gctalloc("btge_light_t", sizeof(BTGE_Light));
	}

	V3F_COPY(light->org, light->oldval->org);
	V3F_COPY(light->dir, light->oldval->dir);
	V3F_COPY(light->clr, light->oldval->clr);
	V2F_COPY(light->val, light->oldval->val);
	V3F_COPY(light->coeff, light->oldval->coeff);

	V3F_COPY(light->mins, light->oldval->mins);
	V3F_COPY(light->maxs, light->oldval->maxs);
	V3F_COPY(light->amins, light->oldval->amins);
	V3F_COPY(light->amaxs, light->oldval->amaxs);
	V3F_COPY(light->boxval, light->oldval->boxval);
	V3F_COPY(light->rel_org, light->oldval->rel_org);
	V3F_COPY(light->rel_dir, light->oldval->rel_dir);

	light->oldval->angle=light->angle;
	light->oldval->flags=light->flags;
	light->oldval->num=light->num;
	light->oldval->leaf=light->leaf;
	light->oldval->style=light->style;
	light->oldval->envtex=light->envtex;

	light->oldval->texname=light->texname;
}

void BTGE_LightClearDelta(btLight light)
{
	light->flags|=BTGE_LFL_CHANGED;

	if(light->oldval)
	{
		gcfree(light->oldval);
		light->oldval=NULL;
	}
}

BTSV_API dyt BTGE_ComposeBrushPatchDelta(
	BTGE_BrushWorld *wrl, BTGE_Brush *brush, btEntity clent)
{
	BTGE_BrushPatchPoint *pt;
	dyt t, l, l1, l2, l3;
	int i, j;

	l=NULL;

	l3=NULL;
	for(i=0; i<brush->patch->ny_pts; i++)
	{
		l1=NULL;
		for(j=0; j<brush->patch->nx_pts; j++)
		{
			pt=brush->patch->pts[i][j];
			
			l2=NULL;
			
			t=dylist4s("xyz",
				dyflonum(pt->xyz[0]),
				dyflonum(pt->xyz[1]),
				dyflonum(pt->xyz[2]));
			l2=dycons(t, l2);

			t=dylist3s("uv",
				dyflonum(pt->uv[0]),
				dyflonum(pt->uv[1]));
			l2=dycons(t, l2);

			t=dylist5s("norm",
				dyflonum(pt->norm[0]),
				dyflonum(pt->norm[1]),
				dyflonum(pt->norm[2]),
				dyflonum(pt->norm[3]));
			l2=dycons(t, l2);

#if 0
			t=dylist5s("rgba",
				dyflonum(pt->rgba[0]),
				dyflonum(pt->rgba[1]),
				dyflonum(pt->rgba[2]),
				dyflonum(pt->rgba[3]));
			l2=dycons(t, l2);
#endif

			t=dyconss("point", l2);
			l1=dycons(t, l1);
		}
		l1=dynreverse(l1);
		t=dyconss("pts", l1);
		l3=dycons(t, l3);
	}
	l3=dynreverse(l3);
	t=dyconss("pts", l3);
	l=dycons(t, l);

#if 0
	t=dylist5s("sdir",
		dyflonum(brush->patch->sdir[0]),
		dyflonum(brush->patch->sdir[1]),
		dyflonum(brush->patch->sdir[2]),
		dyflonum(brush->patch->sdir[3]));
	l=dycons(t, l);

	t=dylist5s("tdir",
		dyflonum(brush->patch->tdir[0]),
		dyflonum(brush->patch->tdir[1]),
		dyflonum(brush->patch->tdir[2]),
		dyflonum(brush->patch->tdir[3]));
	l=dycons(t, l);
#endif

	t=dylist2s("texname", dystring(brush->patch->tex));
	l=dycons(t, l);

	t=dylist2s("nx_pts", dyint(brush->patch->nx_pts));
	l=dycons(t, l);

	t=dylist2s("ny_pts", dyint(brush->patch->ny_pts));
	l=dycons(t, l);

#if 0
	l=dycons(dylist2s("xoffs", dyflonum(brush->patch->xoffs)), l);
	l=dycons(dylist2s("yoffs", dyflonum(brush->patch->yoffs)), l);
	l=dycons(dylist2s("rot", dyflonum(brush->patch->rot)), l);
	l=dycons(dylist2s("xscale", dyflonum(brush->patch->xscale)), l);
	l=dycons(dylist2s("yscale", dyflonum(brush->patch->yscale)), l);
#endif

	if(!l)return(NULL);
//	t=dycons2s("patchdef", dyint(brush->idnum), l);
	t=dyconss("patchdef", l);
	return(t);
}

BTSV_API dyt BTGE_ComposeBrushDelta(
	BTGE_BrushWorld *wrl, BTGE_Brush *brush, btEntity clent)
{
	dyt t, l, l1;
	int i;
	
	if(brush->flags&BTGE_BRFL_DEL)
	{
		t=dylist2s("delbrush", dyint(brush->idnum));
		return(t);
	}

	if(brush->patch)
	{
		t=BTGE_ComposeBrushPatchDelta(wrl, brush, clent);
		l=dylist1(t);

		if(brush->flags&BTGE_BRFL_NEW)
		{
			t=dycons3s("newbrush",
				dyint(brush->se_owner->idnum),
				dyint(brush->idnum), l);
//			brush->flags&=~BTGE_BRFL_NEW;
			return(t);
		}

		if(!l)return(NULL);
		t=dycons2s("bdelta", dyint(brush->idnum), l);
		
//		printf("BTGE_ComposeBrushDelta: PatchDef\n");
//		dysPrintln(t);
		
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
//		brush->flags&=~BTGE_BRFL_NEW;
		return(t);
	}

	if(!l)return(NULL);
	t=dycons2s("bdelta", dyint(brush->idnum), l);
	return(t);
}

BTSV_API dyt BTGE_BrushDelta(BTGE_Brush *brush, btEntity clent)
{
	dyt t;
	int i, fdm;

	if(brush->flags&BTGE_BRFL_TRANSIENT)
		return(NULL);
	
	if(!brush->idnum)
	{
		btge_world->map_stat_brushes++;
		i=btge_world->map_stat_brushes;
		brush->idnum=i;
		brush->flags|=BTGE_BRFL_NEW|BTGE_BRFL_DIRTY;
	}

	if(clent)
	{
		i=brush->idnum;
		fdm=!(clent->client->deltamap_brush[i>>3]&(1<<(i&7)));
		clent->client->deltamap_brush[i>>3]|=(1<<(i&7));
	}

	if(brush->flags&(BTGE_BRFL_NEW|BTGE_BRFL_DIRTY) || fdm)
	{
		t=BTGE_ComposeBrushDelta(btge_world, brush, clent);
//		brush->flags&=~BTGE_BRFL_DIRTY;
//		brush->flags&=~BTGE_BRFL_NEW;
		return(t);
	}
	
	return(NULL);
}

BTSV_API void BTGE_BrushPreDelta(BTGE_Brush *brush)
{
	dyt t;
	int i;

	if(brush->flags&BTGE_BRFL_TRANSIENT)
		return;
	
	if(!brush->idnum)
	{
		btge_world->map_stat_brushes++;
		i=btge_world->map_stat_brushes;
		brush->idnum=i;
		brush->flags|=BTGE_BRFL_NEW|BTGE_BRFL_DIRTY;
	}
}

BTSV_API void BTGE_BrushPostDelta(BTGE_Brush *brush)
{
	dyt t;
	int i;

	if(brush->flags&BTGE_BRFL_TRANSIENT)
		return;
	
	brush->flags&=~(BTGE_BRFL_NEW|BTGE_BRFL_DIRTY);
}

BTSV_API dyt BTGE_SEntityDelta(BTGE_SEntity *scur, btEntity clent)
{
	BTGE_SEntPair *kcur;
	dyt t, l1;
	int i;

	if(!scur->idnum)
	{
		btge_world->map_stat_ents++;
		i=btge_world->map_stat_ents;
		scur->idnum=i;
		scur->flags|=BTGE_SEFL_NEW;
	}
	
	if(scur->flags&BTGE_SEFL_NEW)
	{
		kcur=scur->keys; l1=NULL;
		while(kcur)
		{
			t=dylist3s("key",
				dystring(kcur->name),
				dystring(kcur->value));
			l1=dycons(t, l1);
				
//			kcur->flags&=~BTGE_SEFL_DIRTY;
			kcur=kcur->next;
		}

//		scur->flags&=~(BTGE_SEFL_DIRTY|BTGE_SEFL_NEW);

		i=scur->idnum;
		t=dycons2s("newsent", dyint(i), l1);
		return(t);
	}

	if(scur->flags&BTGE_SEFL_DIRTY)
	{
//		btcl_brushWorld->map_stat_ents++;
//		i=btcl_brushWorld->map_stat_ents;
//		scur->idnum=i;

		kcur=scur->keys; l1=NULL;
		while(kcur)
		{
			t=dylist3s("key",
				dystring(kcur->name),
				dystring(kcur->value));
			l1=dycons(t, l1);
				
//			kcur->flags&=~BTGE_SEFL_DIRTY;
			kcur=kcur->next;
		}

//		scur->flags&=~BTGE_SEFL_DIRTY;

		if(!l1)return(NULL);

		i=scur->idnum;
		t=dycons2s("sedelta", dyint(i), l1);
		return(t);
	}

	return(NULL);
}

BTSV_API void BTGE_SEntityPreDelta(BTGE_SEntity *scur)
{
	int i;

	if(!scur->idnum)
	{
		btge_world->map_stat_ents++;
		i=btge_world->map_stat_ents;
		scur->idnum=i;
		scur->flags|=BTGE_SEFL_NEW;
	}
}

BTSV_API void BTGE_SEntityPostDelta(BTGE_SEntity *scur)
{
	BTGE_SEntPair *kcur;
	dyt t, l1;
	int i;

	if(scur->flags&(BTGE_SEFL_NEW|BTGE_SEFL_DIRTY))
	{
		kcur=scur->keys;
		while(kcur)
		{
			kcur->flags&=~BTGE_SEFL_DIRTY;
			kcur=kcur->next;
		}

		scur->flags&=~(BTGE_SEFL_DIRTY|BTGE_SEFL_NEW);
	}
}

dyt BTGE_DeltaWorld(btEntity clent)
{
	btEntity cur;
	btLight lcur;
	BTGE_Brush *bcur;
	BTGE_SEntity *scur;
	BTGE_SEntPair *kcur;
	dyt t, lst;
	int i;

#if 1
	bcur=btge_world->brush;
	while(bcur)
	{
		BTGE_BrushPreDelta(bcur);
		bcur=bcur->next;
	}

	scur=btge_world->entity;
	while(scur)
	{
		BTGE_SEntityPreDelta(scur);
		scur=scur->next;
	}
#endif
	
	lst=NULL;
	for(i=1; i<btge_world->n_ents; i++)
	{
		cur=btge_world->ents[i];
		if(!cur)continue;
		t=BTGE_EntityDelta(cur, clent);
		if(t)lst=dycons(t, lst);
	}
	
	lcur=btge_world->light;
	while(lcur)
	{
		t=BTGE_LightDelta(lcur, clent);
		if(t)lst=dycons(t, lst);
		lcur=lcur->next;
	}

#if 1
	bcur=btge_world->brush;
	while(bcur)
	{
		t=BTGE_BrushDelta(bcur, clent);
		if(t)lst=dycons(t, lst);
		bcur=bcur->next;
	}

	scur=btge_world->entity;
	while(scur)
	{
		t=BTGE_SEntityDelta(scur, clent);
		if(t)lst=dycons(t, lst);
		scur=scur->next;
	}
#endif

	t=BTGE_Voxel_ComposeWorldDelta(btge_world);
	if(t)lst=dycons(t, lst);

	lst=BTCMUI_WorldDelta(btge_world, lst);

	if(!lst)return(NULL);
	
	t=dyconss("wdelta", lst);
	return(t);
}

void BTGE_PostDeltaWorld()
{
	BTGE_Brush *bcur;
	BTGE_SEntity *scur;
	BTGE_SEntPair *kcur;
	btEntity cur;
	btLight lcur;
	int i;
	
	for(i=1; i<btge_world->n_ents; i++)
	{
		cur=btge_world->ents[i];
		if(!cur)continue;
		BTGE_EntityPostDelta(cur);
	}
	
	lcur=btge_world->light;
	while(lcur)
	{
		BTGE_LightPostDelta(lcur);
		lcur=lcur->next;
	}

#if 1
	bcur=btge_world->brush;
	while(bcur)
	{
		BTGE_BrushPostDelta(bcur);
		bcur=bcur->next;
	}

	scur=btge_world->entity;
	while(scur)
	{
		BTGE_SEntityPostDelta(scur);
		scur=scur->next;
	}
#endif

	BTGE_Voxel_WorldPostDelta(btge_world);
	BTCMUI_WorldPostDelta(btge_world);
}

void BTGE_ClearDeltaWorld()
{
	btEntity cur;
	btLight lcur;
	int i;
	
	for(i=1; i<btge_world->n_ents; i++)
	{
		cur=btge_world->ents[i];
		if(!cur)continue;
		BTGE_EntityClearDelta(cur);
	}
	
	lcur=btge_world->light;
	while(lcur)
	{
		BTGE_LightClearDelta(lcur);
		lcur=lcur->next;
	}
}

dyt BTGE_ComposeWorldUpdate(int clnum)
{
	BTGE_Inventory *ivcur;
	btEntity clent;
	dyt t, lst;

	BTGE_BrushWorld_DrawBuildFast(btge_world);

	clent=btLookupClientEntity(clnum);

//	lst=btge_msgqueue;
//	lst=dylistcopy(btge_msgqueue);
	lst=dyListCopyR(btge_msgqueue);
	t=BTGE_DeltaWorld(clent);
	if(t)lst=dycons(t, lst);
	
//	clent=btLookupClientEntity(clnum);
	if(clent)
	{
		lst=dynappend(lst, clent->msg_queue);
		clent->msg_queue=NULL;

//		t=dylist2s("cltime", dyflonum(clent->client_time));
//		lst=dycons(t, lst);

		t=dylist3s("time",
			dyflonum(btTime()),
			dyflonum(clent->client_time));
		lst=dycons(t, lst);

		ivcur=clent->items;
		while(ivcur)
		{
			t=dylist4s("inven",
				dystring(ivcur->classname),
				dyint(ivcur->count),
				dyint(ivcur->slot));
			lst=dycons(t, lst);

			ivcur=ivcur->next;
		}

		t=dylist3s("hp", dyint(clent->health), dyint(clent->maxHealth));
		lst=dycons(t, lst);

		t=dylist3s("mp", dyint(clent->curMP), dyint(clent->maxMP));
		lst=dycons(t, lst);

		t=dylist3s("ap", dyint(clent->curArmor), dyint(clent->maxArmor));
		lst=dycons(t, lst);
		
		if((clent->client->reforg[0]!=clent->client->oldreforg[0]) ||
			(clent->client->reforg[1]!=clent->client->oldreforg[1]) ||
			(clent->client->reforg[2]!=clent->client->oldreforg[2]))
		{
			t=dylist4s("reforg",
				dyint(clent->client->reforg[0]),
				dyint(clent->client->reforg[1]),
				dyint(clent->client->reforg[2]));
			lst=dycons(t, lst);
			
			clent->client->oldreforg[0]=clent->client->reforg[0];
			clent->client->oldreforg[1]=clent->client->reforg[1];
			clent->client->oldreforg[2]=clent->client->reforg[2];
		}
	}


	//current server time
//	t=dylist2s("svtime", dyflonum(btTime()));
//	lst=dycons(t, lst);

//	btge_msgqueue=NULL;
	return(lst);
}

void BTGE_FinishWorldUpdate()
{
	dyt t, lst;

	BTGE_PostDeltaWorld();

	dyFreeListR(btge_msgqueue);
	btge_msgqueue=NULL;
}

void BTGE_FlushWorldUpdate()
{
	dyt t, lst;

	BTGE_ClearDeltaWorld();

	dyFreeListR(btge_msgqueue);
	btge_msgqueue=NULL;
}

BTSV_API dyt btsvComposeUpdate(int clnum)
{
	return(BTGE_ComposeWorldUpdate(clnum));
}

BTSV_API void btsvFinishUpdate(void)
{
	BTGE_FinishWorldUpdate();
}

BTSV_API void btsvFlushUpdate(void)
{
	BTGE_FlushWorldUpdate();
}

BTSV_API void btsvFreeUpdate(dyt msg)
{
	if(!msg)return;
	dyFreeListR(msg);
}

BTSV_API void BTGE_AddUpdateQueue(dyt msg)
{
	if(!msg)return;
	btge_msgqueue=dycons(msg, btge_msgqueue);
}

BTSV_API void BTGE_AddUpdateEntityQueue(btEntity ent, dyt msg)
{
	if(!msg)return;
	ent->msg_queue=dycons(msg, ent->msg_queue);
}



void BTGE_DecodeMsgVec(float *v, int nv, dyt msg)
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

void BTGE_DecodeMsgVec2(float *v, dyt msg)
	{ BTGE_DecodeMsgVec(v, 2, msg); }
void BTGE_DecodeMsgVec3(float *v, dyt msg)
	{ BTGE_DecodeMsgVec(v, 3, msg); }
void BTGE_DecodeMsgVec4(float *v, dyt msg)
	{ BTGE_DecodeMsgVec(v, 4, msg); }

void BTGE_ClientImpulseMove(btEntity ent, vec3 ivel, float dt)
{
	vec3d io;

	//don't try to move if camera position is locked
	if(ent->effects2&BT_EF2_CAM_FIXED_ORG)
		return;

//	printf("BTGE_ClientImpulseMove %p %f\n", ent, dt);

	io=btBoxStepMove(ent->origin,
			ent->mins, ent->maxs, ivel, dt, ent);
	ent->origin=io;

	BT_CheckFireBoxTrigger(ent);
}


void BTGE_DecodeClientBDeltaFaceMsg(dyt msg, BTGE_Brush *brush, int fn)
{
	float tv[4];
	dyt v, c;
	int i, j;

	BTGE_Brush_CheckExpandFaces(brush, fn+1);
	if(fn>=brush->n_face)brush->n_face=fn+1;

	v=dycar(msg);

	if(v==dysym("norm"))
	{
		BTGE_DecodeMsgVec4(tv, msg);
		V4F_COPY(tv, brush->norm+(fn*4));
		return;
	}

	if(v==dysym("sdir"))
	{
		BTGE_DecodeMsgVec4(tv, msg);
		V4F_COPY(tv, brush->sdir+(fn*4));
		return;
	}

	if(v==dysym("tdir"))
	{
		BTGE_DecodeMsgVec4(tv, msg);
		V4F_COPY(tv, brush->tdir+(fn*4));
		return;
	}

	if(v==dysym("texname"))
	{
		brush->tex[fn]=dystrdup(dystringv(dycadr(msg)));
		return;
	}
}

void BTGE_DecodeClientBDeltaMsg(dyt msg, BTGE_Brush *brush)
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
			BTGE_DecodeClientBDeltaFaceMsg(dycar(c), brush, i);
			c=dycdr(c);
		}
		return;
	}
}

void BTGE_DecodeClientSEDeltaMsg(dyt msg, BTGE_SEntity *sent)
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
		BTGE_SEnt_SetStr(sent, s0, s1);
		return;
	}
}

void BTGE_DecodeClientDeltaMsg(dyt msg)
{
	float tv[4];
	BTGE_SEntity *sent;
	BTGE_Brush *brush;
	dyt v, c;
	int i, j;

	v=dycar(msg);

	if(v==dysym("bdelta"))
	{
		i=dyintv(dycadr(msg));
		brush=BTGE_BrushWorld_LookupBrush(btge_world, i);
		if(!brush)
		{
			printf("BTGE_DecodeClientDeltaMsg: BDelta, bad brush %d\n", i);
			//make new brush?...
			return;
		}

//		BTGE_BrushWorld_DrawFlush(btge_world);
//		BTGE_Brush_DestroyFaces(brush);
		BTGE_BrushWorld_DrawFlushBrush(btge_world, brush);

		c=dycddr(msg);
		while(c)
		{
			BTGE_DecodeClientBDeltaMsg(dycar(c), brush);
			c=dycdr(c);
		}

		BTGE_Brush_BuildFaces(brush);
		BTGE_BrushWorld_DrawFlushBrush(btge_world, brush);
		return;
	}

	if(v==dysym("delbrush"))
	{
		BTGE_BrushWorld_DrawFlush(btge_world);

		i=dyintv(dycadr(msg));
		brush=BTGE_BrushWorld_LookupBrush(btge_world, i);
		if(!brush)return;
		brush->flags|=BTGE_BRFL_DEL;
		return;
	}

	if(v==dysym("newbrush"))
	{
		i=dyintv(dycadr(msg));
		j=dyintv(dycaddr(msg));
//		brush=BTGE_BrushWorld_LookupBrush(btge_world, i);
		brush=BTGE_BrushWorld_CreateBrushId(btge_world, i, j);
		if(!brush)
		{
			printf("BTGE_DecodeClientDeltaMsg: "
				"Newbrush failed %d %d\n", i, j);
			return;
		}

		BTGE_BrushWorld_DrawFlush(btge_world);

//		BTGE_Brush_DestroyFaces(brush);

		c=dycdddr(msg);
		while(c)
		{
			BTGE_DecodeClientBDeltaMsg(dycar(c), brush);
			c=dycdr(c);
		}
		return;
	}

	if(v==dysym("newsent"))
	{
		i=dyintv(dycadr(msg));

		sent=BTGE_BrushWorld_LookupSEntityId(btge_world, i);
		if(sent)
		{
			printf("BTGE_DecodeClientDeltaMsg: "
				"NewSEnt: Already Exists %d\n", i);
			return;
		}
		
		sent=BTGE_BrushWorld_GetSEntityId(btge_world, i);

		BTGE_BrushWorld_DrawFlush(btge_world);

		c=dycddr(msg);
		while(c)
		{
			BTGE_DecodeClientSEDeltaMsg(dycar(c), sent);
			c=dycdr(c);
		}
		return;
	}

	if(v==dysym("sedelta"))
	{
		i=dyintv(dycadr(msg));
		sent=BTGE_BrushWorld_LookupSEntityId(btge_world, i);
		if(!sent)
		{
			printf("BTGE_DecodeClientDeltaMsg: "
				"SEDelta: Doesn't Exist %d\n", i);
			return;
		}

		BTGE_BrushWorld_DrawFlush(btge_world);

		c=dycddr(msg);
		while(c)
		{
			BTGE_DecodeClientSEDeltaMsg(dycar(c), sent);
			c=dycdr(c);
		}
		return;
	}
}

void BTGE_DecodeClientWDelta(dyt msg)
{
	dyt c;

	printf("BTGE_DecodeClientWDelta\n");
	dysPrintln(msg);

	c=dycdr(msg);
	while(c)
	{
		BTGE_DecodeClientDeltaMsg(dycar(c));
		c=dycdr(c);
	}
}

void BTGE_DecodeClientImpulseEvent(dyt msg, btEntity ent, float dt)
{
	float tv[4];
	dyt v;
	int i, j;
	BTGE_MessageHandler *hdlr;

	v=dycar(msg);

	hdlr=BTGE_LookupMessageHandler(v);
	if(hdlr)
	{
		if(hdlr->Handler)
		{
			hdlr->Handler(ent, msg);
			return;
		}
		if(hdlr->dyHandler)
		{
			BSVM_CallFuncObj(NULL, hdlr->dyHandler,
				dylist2((dyt)ent, msg));
//			hdlr->Handler(wrl, msg);
			return;
		}
	}

//	printf("BTGE_DecodeClientImpulseEvent %s\n", dysymv(v));

	if(v==dysym("framedt"))
	{
		dt=dyfloatv(dycadr(msg));
		return;
	}

	if(v==dysym("org"))
	{
		//origin on client
		return;
	}

	if(v==dysym("ang"))
	{
		//angle is locked, so ingore client rotations
		if(ent->effects2&(BT_EF2_CAM_FORCEANGLE|BT_EF2_CAM_FIXED_ROT))
			return;

		BTGE_DecodeMsgVec3(tv, msg);
//		ent->morg=vec3vf(tv);
		ent->angles=vec3vf(tv);
		return;
	}

	if(v==dysym("ivel"))
	{
		BTGE_DecodeMsgVec3(tv, msg);
		ent->impulseVelocity=vec3vf(tv);
//		BTGE_ClientImpulseMove(ent, vec3vf(tv), dt);
		return;
	}

	if(v==dysym("impfl"))
	{
		i=dyintv(dycadr(msg));
		ent->impulseFlags|=i;
//		BT_PlayerImpulse(ent, i);
	}

	if(v==dysym("morg"))
	{
		BTGE_DecodeMsgVec3(tv, msg);
		
//		tv[0]+=ent->client->reforg[0];
//		tv[1]+=ent->client->reforg[1];
//		tv[2]+=ent->client->reforg[2];
//		ent->morg=vec3dvf(tv);

		ent->morg=vec3d(
			tv[0]+ent->client->reforg[0],
			tv[1]+ent->client->reforg[1],
			tv[2]+ent->client->reforg[2]);

		return;
	}
	if(v==dysym("mend"))
	{
		BTGE_DecodeMsgVec3(tv, msg);

//		tv[0]+=ent->client->reforg[0];
//		tv[1]+=ent->client->reforg[1];
//		tv[2]+=ent->client->reforg[2];
//		ent->mend=vec3dvf(tv);

		ent->mend=vec3d(
			tv[0]+ent->client->reforg[0],
			tv[1]+ent->client->reforg[1],
			tv[2]+ent->client->reforg[2]);

		return;
	}
	if(v==dysym("mdir"))
	{
		BTGE_DecodeMsgVec3(tv, msg);
		ent->mdir=vec3vf(tv);
		return;
	}
	if(v==dysym("wdelta"))
		{ BTGE_DecodeClientWDelta(msg); return; }

	if(v==dysym("inv_click"))
	{
		BT_ClientInventoryClick(ent, dystringv(dycadr(msg)));
//		BTGE_DecodeMsgVec3(tv, msg);
//		ent->mdir=vec3vf(tv);
		return;
	}
}

void BTGE_DecodeClientImpulse(dyt msg, float dt)
{
	btEntity ent;
	int cam;
	dyt c;
	int i;

	i=dyintv(dycadr(msg));
	if(i<=0)
	{
		printf("BTGE_DecodeClientImpulse: Bad Ent Id %d\n", i);
		return;
	}
	ent=btGetEntity(i);
	if(!ent)
	{
		printf("BTGE_DecodeClientImpulse: Unknown Ent Id %d\n", i);
		return;
	}

//	printf("BTGE_DecodeClientImpulse %p %f\n", ent, dt);

	c=dycddr(msg);
	while(c)
	{
		BTGE_DecodeClientImpulseEvent(dycar(c), ent, dt);
		c=dycdr(c);
	}
}

void BTGE_DecodeClientStuffCmd(dyt msg, int clnum)
{
	btEntity ent;
	int cam;
	dyt c;
	char *s, **a;
	int i;

	i=dyintv(dycadr(msg));
	if(i<=0)
	{
		printf("BTGE_DecodeClientStuffCmd: Bad Ent Id %d\n", i);
		return;
	}
	ent=btGetEntity(i);
	if(!ent)
	{
		printf("BTGE_DecodeClientStuffCmd: Unknown Ent Id %d\n", i);
		return;
	}

	s=dystringv(dycaddr(msg));
	a=gcrsplit(s);
	BT_ClientConCmd(ent, a);
}

void BTGE_DecodeClientConnect(dyt msg, int clnum)
{
	BTGE_ClientInfo *clinf;
	btEntity clent;
	btEntity went;
	dyt t, lst;
	char *s;

	printf("BTGE_DecodeClientConnect\n");

	clinf=BTGE_GetClientInfo(clnum);

	lst=NULL;

	s=btCvarGet("mapname");
	t=dylist2s("mapname", dystring(s));
	lst=dycons(t, lst);

	clent=btSpawnPlayer();
	t=dylist2s("camera", dyint(clent->entnum));
	lst=dycons(t, lst);
	clent->clientnum=clnum;

	clent->client=clinf;
	clinf->entity=clent;

	//send initial player state now
	t=BTGE_EntityDelta(clent, clent);
	t=dylist2s("wdelta", t);
	lst=dynappend(lst, dylist1(t));
//	if(t)lst=dycons(t, lst);

//	t=dyconss("wdelta", lst);
	BTGE_Net_ServerSendUpdate(clnum, lst);

	BTGE_FlushWorldUpdate();	//force resend all deltas

	went=btFindClassname("worldspawn");

	if(went)
	{
		s=BTGE_SEnt_GetStr(went->sent, "sky");
		if(s)
		{
			btStuffCmd(clent, "sky3 bkgr\n");
			btStuffCmd(clent, "sky2 stars\n");

			btStuffCmd(clent, "sky %s\n", s);
		}
	}

	//newly connected client
	BT_ClientConnected(clent);

//	BTGE_Net_ServerSendUpdate(clnum, dylist1(t));
//	return(t);
}

void BTGE_DecodeClientUIDelta(dyt msg, int clnum)
{
	BTCMUI_WorldDecodeDelta(btge_world, msg);
}

void BTGE_DecodeClientUpdateMsg(dyt msg, float dt, int clnum)
{
	dyt v;
	float f, g, h;
//	float dt, clt;
	
	v=dycar(msg);
	if(v==dysym("impulse"))
		{ BTGE_DecodeClientImpulse(msg, dt); return; }
	if(v==dysym("connect"))
		{ BTGE_DecodeClientConnect(msg, clnum); return; }
	if(v==dysym("stuffcmd"))
		{ BTGE_DecodeClientStuffCmd(msg, clnum); return; }
	if(v==dysym("wdelta"))
		{ BTGE_DecodeClientWDelta(msg); return; }
	if(v==dysym("uidelta"))
		{ BTGE_DecodeClientUIDelta(msg, clnum); return; }
}

BTSV_API void BTGE_DecodeClientUpdate(dyt msg, float dt, int clnum)
{
	dyt c;
	int i;

//	c=dycdr(msg);
	c=msg;
	while(c)
	{
		BTGE_DecodeClientUpdateMsg(dycar(c), dt, clnum);
		c=dycdr(c);
	}
}
