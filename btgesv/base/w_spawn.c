#include <btgesv.h>

BTGE_BrushWorld *btge_world;

float btge_time;		//world time
float btge_steptime;	//world step time
// btEntity btge_client;	//player
float btge_worldspawntime;		//local time world spawned
int btge_skill;

vec3d btge_spawn_org;
int btge_spawn_isPrefab=0;
int btge_spawn_lightnum;
int btge_spawn_prefabnum;
int btge_spawn_prefabmax;

char **btge_spawn_alias_src;
char **btge_spawn_alias_dst;
int btge_spawn_n_alias;
int btge_spawn_m_alias;

BTSV_API int btGetSkill()
	{ return(btge_skill); }

BTSV_API void btRegisterSpawnAlias(char *srcname, char *dstname)
{
	int i;

	if(!btge_spawn_alias_src)
	{
		btge_spawn_alias_src=gcalloc(64*sizeof(char *));
		btge_spawn_alias_dst=gcalloc(64*sizeof(char *));
		btge_spawn_m_alias=64;
	}
	
	for(i=0; i<btge_spawn_n_alias; i++)
		if(!strcmp(btge_spawn_alias_src[i], srcname))
	{
		btge_spawn_alias_dst[i]=dystrdup(dstname);
		return;
	}
	
	if((btge_spawn_n_alias+1)>=btge_spawn_m_alias)
	{
		i=btge_spawn_m_alias+(btge_spawn_m_alias>>1);
		btge_spawn_alias_src=gcrealloc(
			btge_spawn_alias_src, i*sizeof(char *));
		btge_spawn_alias_dst=gcrealloc(
			btge_spawn_alias_dst, i*sizeof(char *));
		btge_spawn_m_alias=i;
	}
	
	i=btge_spawn_n_alias++;
	btge_spawn_alias_src[i]=dystrdup(srcname);
	btge_spawn_alias_dst[i]=dystrdup(dstname);
}

BTSV_API char *btLookupSpawnAliasI(char *name)
{
	int i;
	
	for(i=0; i<btge_spawn_n_alias; i++)
		if(!strcmp(btge_spawn_alias_src[i], name))
			return(btge_spawn_alias_dst[i]);
	return(NULL);
}

BTSV_API char *btLookupSpawnAlias(char *name)
{
	char *s0, *s1;
	int i;
	
	s0=btLookupSpawnAliasI(name);
	if(!s0)return(NULL);
	for(i=0; i<16; i++)
	{
		s1=btLookupSpawnAliasI(s0);
		if(!s1)break;
		s0=s1;
	}
	return(s0);
}

BTSV_API void btInitEntity(btEntity ent, btSEntity args)
{
	char tb[1024];
	float v0[3], v1[3], v2[3];
	BTGE_SEntPair *cur;
	char *s, *t;
	int i;
	
	if(!args)return;
	
	cur=args->keys;
	while(cur)
	{
		s=cur->value;
		t=tb;
		
		if(btge_spawn_isPrefab)
		{
			if(!strcmp(cur->name, "target") ||
				!strcmp(cur->name, "targetname") ||
				!strcmp(cur->name, "killtarget") ||
				!strcmp(cur->name, "landmark"))
			{
				if(((*s>='a') && (*s<='z')) ||
					((*s>='A') && (*s<='Z')))
				{
					sprintf(t, "PFN%d_", btge_spawn_prefabnum);
					t+=strlen(t);
				}
			}
		}

		while(*s)
		{
			if(*s=='\\')
			{
				s++;
				i=*s++;
				switch(i)
				{
				case '\\': *t++=i; break;
				case 't': *t++='\t'; break;
				case 'n': *t++='\n'; break;
				default: *t++=i; break;
				}
				continue;
			}
			*t++=*s++;
		}
		*t++=0;
	
		btSetFieldAsString(ent, cur->name, tb);
//		btSetFieldAsString(ent, cur->name, cur->value);
		cur=cur->next;
	}

	//remove physical status flags
	ent->flags&=~(
		BT_FL_ONGROUND|BT_FL_PARTIALGROUND|
		BT_FL_WATERJUMP|BT_FL_JUMPRELEASED|
		BT_FL_SIMPLEMOVE|BT_FL_OFFGROUND);

	ent->spawnOrigin=ent->origin;
	ent->spawnAngles=ent->angles;

	ent->sent=args;
//	btGetBModel(ent)=args->bmdl;
	if(btGetBModel(ent))
	{
		BTGE_BrushModel_BoxModel(btGetBModel(ent), v0, v1);
		ent->mins=vec3vf(v0);
		ent->maxs=vec3vf(v1);
//		ent->origin=vec3vf(btGetBModel(ent)->mdl_org);
		if(v3dlen(ent->origin)<0.01)
			ent->origin=vec3dvf(btGetBModel(ent)->mdl_org);

		ent->effects2|=BT_EF2_NOLERP;

//		ent->origin=v3add(ent->origin, vec3vf(btGetBModel(ent)->mdl_org));
//		ent->origin=v3sub(vec3vf(btGetBModel(ent)->mdl_org), ent->origin);
//		ent->origin=v3sub(ent->origin, vec3vf(btGetBModel(ent)->mdl_org));

		if(args->wrl!=btge_world)
		{
			sprintf(tb, "%s:%d",
				args->wrl->map_name, btGetBModel(ent)->mdlnum);
			ent->model=dystrdup(tb);
		}else ent->model=dystrdup(gcrdtoa(btGetBModel(ent)->mdlnum));

		ent->oldorigin=v3dsub(ent->origin, v3f2d(ent->maxs));
	}

	//adjust for load origin
	ent->origin=v3dadd(ent->origin, btge_spawn_org);

	if(btge_spawn_isPrefab)
	{
		if(args->light)
		{
			ent->light=gctalloc("btge_light_t", sizeof(BTGE_Light));
			memcpy(ent->light, args->light, sizeof(BTGE_Light));

//			ent->light->num+=1024;
			ent->light->num=btge_spawn_lightnum++;
			ent->light->flags|=BTGE_LFL_CHANGED;

			vfvec3d(v2, btge_spawn_org);
			V3F_ADD(ent->light->org, v2, ent->light->org);

			ent->light->next=btge_world->light;
			btge_world->light=ent->light;
		}
	}else
	{
		ent->light=args->light;
	}
}

BTSV_API btEntity btNewEntityInit(btSEntity args)
{
	btEntity ent;
	
	ent=btNewEntity();
	btInitEntity(ent, args);
	return(ent);
}

BTSV_API btEntity btNewEntityTriggerInit(btSEntity args,
	BTGE_Entity_Iface *vt)
{
	btEntity ent;
	
	ent=btNewEntity();
//	ent->eTrig=gcalloc(sizeof(BTGE_EntityTrigger));
	btInitEntity(ent, args);
	ent->vt=gcalloc(sizeof(BTGE_Entity_Iface));
	if(vt)
	{
		//copy the vtable to allow assigning entries
		memcpy(ent->vt, vt, sizeof(BTGE_Entity_Iface));
		if(ent->vt->init)
			ent->vt->init(ent);
	}
	return(ent);
}

BTSV_API btEntity btNewEntityActorInit(btSEntity args,
	BTGE_Entity_Iface *vt, BTGE_Actor_Iface *avt)
{
	btEntity ent;
	
	ent=btNewEntity();
//	ent->eTrig=gcalloc(sizeof(BTGE_EntityTrigger));
//	ent->eAct=gcalloc(sizeof(BTGE_EntityActor));
	btInitEntity(ent, args);
//	ent->vt=vt;
//	ent->avt=avt;
	if(vt)
	{
		ent->vt=gcalloc(sizeof(BTGE_Entity_Iface));
		memcpy(ent->vt, vt, sizeof(BTGE_Entity_Iface));
	}
	if(avt)
	{
		ent->avt=gcalloc(sizeof(BTGE_Actor_Iface));
		memcpy(ent->avt, avt, sizeof(BTGE_Actor_Iface));
	}

	if(ent->vt && ent->vt->init)
		ent->vt->init(ent);
	return(ent);
}

BTSV_API char *btVec2ToStr(vec2 v)
{
	char tb[256];
	sprintf(tb, "%g %g", v2x(v), v2y(v));
	return(gcrstrdup(tb));
}

BTSV_API char *btVec3ToStr(vec3 v)
{
	char tb[256];
	sprintf(tb, "%g %g %g", v3x(v), v3y(v), v3z(v));
	return(gcrstrdup(tb));
}

BTSV_API char *btVec4ToStr(vec4 v)
{
	char tb[256];
	sprintf(tb, "%g %g %g %g", v4x(v), v4y(v), v4z(v), v4w(v));
	return(gcrstrdup(tb));
}

BTSV_API char *btQuatToStr(quat v)
{
	char tb[256];
	sprintf(tb, "%g %g %g %g", qx(v), qy(v), qz(v), qw(v));
	return(gcrstrdup(tb));
}

double btge_atof(char *str)
{
	if(!str)return(0);
	return(atof(str));
}

BTSV_API vec2 btStrToVec2(char *str)
{
	char **a;
	a=gcrsplit(str);
	if(!a[0] || !a[1])
		return(vec2(0,0));
	return(vec2(btge_atof(a[0]), btge_atof(a[1])));
}

BTSV_API vec3 btStrToVec3(char *str)
{
	char **a;
	a=gcrsplit(str);
	if(!a[0] || !a[1] || !a[2])
		return(vec3(0,0,0));
	return(vec3(btge_atof(a[0]), btge_atof(a[1]), btge_atof(a[2])));
}

BTSV_API vec4 btStrToVec4(char *str)
{
	char **a;
	a=gcrsplit(str);
	if(!a[0] || !a[1] || !a[2] || !a[3])
		return(vec4(0,0,0,0));
	return(vec4(btge_atof(a[0]), btge_atof(a[1]),
		btge_atof(a[2]), btge_atof(a[3])));
}

BTSV_API vec4 btStrToQuat(char *str)
{
	char **a;
	a=gcrsplit(str);
	if(!a[0] || !a[1] || !a[2] || !a[3])
		return(quat(0,0,0,0));
	return(quat(btge_atof(a[0]), btge_atof(a[1]),
		btge_atof(a[2]), btge_atof(a[3])));
}

BTSV_API char *btGetFieldAsString(btEntity ent, char *var)
{
	char *sig;
	void *p;
	int offs;
	int i;

	if(!var)
		return(NULL);

	if(!strcmp(var, "light"))
		{ return(dystrdup(gcrdtoa(ent->lightlevel))); }

	i=dyllGetStructSlotSigOffs("BTGE_Entity_s", var, &sig, &offs);
	if(i<0)return(NULL);
	if(offs<0)return(NULL);
	if(offs>=sizeof(BTGE_Entity))return(NULL);
	
	p=(void *)(((byte *)ent)+offs);
	if(!strcmp(sig, "i"))
		{ return(dystrdup(gcrlltoa(*(int *)p))); }
	if(!strcmp(sig, "x"))
		{ return(dystrdup(gcrlltoa(*(s64 *)p))); }
	if(!strcmp(sig, "f"))
		{ return(dystrdup(gcrdtoa(*(float *)p))); }
	if(!strcmp(sig, "d"))
		{ return(dystrdup(gcrdtoa(*(double *)p))); }
	if(!strcmp(sig, "Pc"))
		{ return(dystrdup(*(char **)p)); }

	if(!strcmp(sig, "Uvec2;"))
		{ return(dystrdup(btVec2ToStr(*(vec2 *)p))); }
	if(!strcmp(sig, "Uvec3;"))
		{ return(dystrdup(btVec3ToStr(*(vec3 *)p))); }
	if(!strcmp(sig, "Uvec4;"))
		{ return(dystrdup(btVec4ToStr(*(vec4 *)p))); }
	if(!strcmp(sig, "Uquat;"))
		{ return(dystrdup(btQuatToStr(*(quat *)p))); }

	if(!strcmp(sig, "Ca"))
		{ return(dystrdup(btVec2ToStr(*(vec2 *)p))); }
	if(!strcmp(sig, "Cb"))
		{ return(dystrdup(btVec3ToStr(*(vec3 *)p))); }
	if(!strcmp(sig, "Cc"))
		{ return(dystrdup(btVec4ToStr(*(vec4 *)p))); }
	if(!strcmp(sig, "Cq"))
		{ return(dystrdup(btQuatToStr(*(quat *)p))); }

	return(NULL);	
}

BTSV_API void btSetFieldAsString(btEntity ent, char *var, char *val)
{
	char *sig;
	void *p;
	int offs;
	int i;

	if(!var || !val)
		return;

	if(!strcmp(var, "origin"))
		{ ent->origin=v3f2d(btStrToVec3(val)); return; }
	if(!strcmp(var, "angles"))
		{ ent->angles=btStrToVec3(val); return; }
	if(!strcmp(var, "velocity"))
		{ ent->velocity=btStrToVec3(val); return; }
	if(!strcmp(var, "angle"))
		{ ent->angles=vec3(0,0,atof(val)); return; }

	if(!strcmp(var, "movedir"))
		{ ent->angles=vec3(0,0,atof(val)); return; }

	if(!strcmp(var, "view_mins"))
		{ ent->viewMins=btStrToVec3(val); return; }
	if(!strcmp(var, "view_maxs"))
		{ ent->viewMaxs=btStrToVec3(val); return; }
	if(!strcmp(var, "view_fov"))
		{ ent->viewFOV=atof(val); return; }

	if(!strcmp(var, "classname"))
		{ ent->classname=dystrdup(val); return; }
	if(!strcmp(var, "model"))
		{ ent->model=dystrdup(val); return; }
	if(!strcmp(var, "targetname"))
		{ ent->targetname=dystrdup(val); return; }
	if(!strcmp(var, "target"))
		{ ent->target=dystrdup(val); return; }
	if(!strcmp(var, "killtarget"))
		{ ent->killtarget=dystrdup(val); return; }
	if(!strcmp(var, "message"))
		{ ent->message=dystrdup(val); return; }
	if(!strcmp(var, "map"))
		{ ent->map=dystrdup(val); return; }

	if(!strcmp(var, "sound"))
		{ ent->sound=dystrdup(val); return; }
	if(!strcmp(var, "noise"))
		{ ent->sound=dystrdup(val); return; }

	if(!strcmp(var, "landmark"))
		{ ent->landmark=dystrdup(val); return; }
	if(!strcmp(var, "anim"))
		{ ent->anim=dystrdup(val); return; }
	if(!strcmp(var, "oldanim"))
		{ ent->oldanim=dystrdup(val); return; }
	if(!strcmp(var, "use_eval"))
		{ ent->use_eval=dystrdup(val); return; }

	if(!strcmp(var, "spawnflags"))
		{ ent->spawnflags=atoi(val); return; }
	if(!strcmp(var, "health"))
		{ ent->health=atof(val); return; }
	if(!strcmp(var, "speed"))
		{ ent->speed=atof(val); return; }

	if(!strcmp(var, "wait"))
		{ ent->wait=atof(val); return; }
	if(!strcmp(var, "delay"))
		{ ent->delay=atof(val); return; }
	if(!strcmp(var, "damage"))
		{ ent->damage=atof(val); return; }
	if(!strcmp(var, "lip"))
		{ ent->lip=atof(val); return; }
	if(!strcmp(var, "height"))
		{ ent->height=atof(val); return; }

	if(!strcmp(var, "light"))
		{ ent->lightlevel=atof(val); return; }

	if(!strcmp(var, "count"))
		{ ent->count=atoi(val); return; }

	i=dyllGetStructSlotSigOffs("BTGE_Entity_s", var, &sig, &offs);
	if(i<0)return;
	if(offs<0)return;
	if(offs>=sizeof(BTGE_Entity))return;
	
	p=(void *)(((byte *)ent)+offs);
	if(!strcmp(sig, "i"))
		{ *(int *)p=atoi(val); return; }
	if(!strcmp(sig, "x"))
		{ *(s64 *)p=atoi(val); return; }
	if(!strcmp(sig, "f"))
		{ *(float *)p=atof(val); return; }
	if(!strcmp(sig, "d"))
		{ *(double *)p=atof(val); return; }
	if(!strcmp(sig, "Pc"))
		{ *(char **)p=dystrdup(val); return; }

	if(!strcmp(sig, "Uvec2;"))
		{ *(vec2 *)p=btStrToVec2(val); return; }
	if(!strcmp(sig, "Uvec3;"))
		{ *(vec3 *)p=btStrToVec3(val); return; }
	if(!strcmp(sig, "Uvec4;"))
		{ *(vec4 *)p=btStrToVec4(val); return; }
	if(!strcmp(sig, "Uquat;"))
		{ *(quat *)p=btStrToQuat(val); return; }

	if(!strcmp(sig, "Ca"))
		{ *(vec2 *)p=btStrToVec2(val); return; }
	if(!strcmp(sig, "Cb"))
		{ *(vec3 *)p=btStrToVec3(val); return; }
	if(!strcmp(sig, "Cc"))
		{ *(vec4 *)p=btStrToVec4(val); return; }
	if(!strcmp(sig, "Cq"))
		{ *(quat *)p=btStrToQuat(val); return; }
		
	printf("btSetFieldAsString: unhandled type '%s', '%s'='%s'\n",
		sig, var, val);
}


BTSV_API btEntity BT_SpawnSEntity(BTGE_SEntity *ent)
{
	char tb[256];
	dycClass inf;
	dycObject obj;
	btEntity tmp;
	void *ptr;
	dyt v;
	char **a;
	char *s, *s1;
	int i;

	//classpkg: indicates to import a packages prior to spawning entity
	s1=BTGE_SEnt_GetStr(ent, "classpkg");
	if(s1) { bsvmRootImportPackageList(s1); }

	s=BTGE_SEnt_GetStr(ent, "classname");
	if(!s)return(NULL);
	
	if(btge_spawn_isPrefab)
	{
		//HACK: rename worldspawn entities for prefabs
		if(!strcmp(s, "worldspawn"))
		{
			s="prefab_worldspawn";
			BTGE_SEnt_SetStr(ent, "classname", s);
		}
	}
	
	//check if the spawn has been redirected
	s1=btLookupSpawnAlias(s);
	if(s1)s=s1;
	
	sprintf(tb, "BtSp_%s", s);
	ptr=dyllGetAddr(tb);

	if(ptr)
	{
		tmp=((btEntity (*)(btSEntity))ptr)(ent);
		return(tmp);
	}

#if 0
	sprintf(tb, "BtSg_%s", s);
	ptr=dyllGetAddr(tb);
	if(ptr)
	{
		((void (*)(btWorld, btSEntity))ptr)(btge_world, ent);
		return(NULL);
	}
#endif

	if(!ptr)
	{
		tmp=btSpawnItem(ent);
		if(tmp)return(tmp);

		//try passing to BGBScript
		v=bsvmCallName1(s, (dyt)ent);
		if(v!=UNDEFINED)
			return((btEntity)v);

		dyllGetStructClassInfo(s);
		inf=dycGetClass(s);
		if(inf)
		{
			tmp=btNewEntityTriggerInit(ent, NULL);
			obj=dycNewInitN(inf, dylist2((dyt)tmp, (dyt)ent));
			tmp->dy_this=(dyt)obj;
			printf("BT_SpawnSEntity: Spawn From Class "
				"inf=%p obj=%p ent=%p\n", inf, obj, tmp);
			return(tmp);
		}

//		v=dyNewN(s, dylist1((dyt)ent));
//		if(v && (v!=UNDEFINED))
//			return;

		btDPrint("BT_SpawnSEntity: No spawnfunc for '%s' %s\n",
			s, ent->bmdl?"(bmdl)":"");

		if(ent->bmdl)
			{ tmp=BtSp_func_nowall(ent); }
		else
			{ tmp=BtSp_misc_nospawn(ent); }
		return(tmp);
	}
	
	return(NULL);
}

BTSV_API btEntity btSpawnSEntity(btSEntity ent)
	{ return(BT_SpawnSEntity(ent)); }

BTSV_API btEntity btSpawnEntityAttr(dytva rest)
{
	BTGE_SEntity *sent;
	btEntity tmp;
	dyt c, tn, tv;
	
	sent=BTGE_SEnt_AllocEntity();
	
	c=rest;
	while(dyconsp(c))
	{
		tn=dycar(c);
		if(!dykeywordp(tn))
			break;
		tv=dycadr(c);
		BTGE_SEnt_SetDynamic(sent, dysymbolv(tn), tv);
		c=dycddr(c);
	}
	
	if(c)
	{
		printf("Bad Argument List %s\n", dyToString(c));
	}
	
	tmp=BT_SpawnSEntity(sent);
	return(tmp);
}

BTSV_API btEntity btSpawnEntityBasic(
	char *classname, vec3 org, float angle)
{
	BTGE_SEntity *sent;
	btEntity tmp;
	float tv[3];
	
	sent=BTGE_SEnt_AllocEntity();

	vfvec3(tv, org);
	
	BTGE_SEnt_SetStr(sent, "classname", classname);
	BTGE_SEnt_SetVec3(sent, "origin", tv);
	BTGE_SEnt_SetNum(sent, "angle", angle);

	tmp=BT_SpawnSEntity(sent);
	return(tmp);
}

BTGE_Light *BT_SpawnSEntityLight(BTGE_SEntity *ent)
{
	float tv[3], tv1[3], tv2[3];
	BTGE_Light *lcur;
	char *s;
	float f, g;
	int i, j;

	lcur=ent->light;
	if(!lcur)
	{
		lcur=gctalloc("btge_light_t", sizeof(BTGE_Light));
		ent->light=lcur;
	}
	
	lcur->coeff[0]=0.50/0.58;
	lcur->coeff[1]=0.50/0.58;
	lcur->coeff[2]=1;
	
	i=BTGE_SEnt_GetVec3(ent, "color", lcur->clr);
	if(i<0)i=BTGE_SEnt_GetVec3(ent, "_color", lcur->clr);
	if(i<0)
	{
//		g=0.57735;
		g=1;
		V3F_SET(lcur->clr, g, g, g);
	}

	BTGE_SEnt_GetVec3(ent, "origin", lcur->org);
	lcur->val[0]=BTGE_SEnt_GetNum(ent, "light");
//	lcur->val[1]=0;
	lcur->val[1]=BTGE_SEnt_GetNum(ent, "hilight");

	if(lcur->val[0]==0)
	{
		i=BTGE_SEnt_GetVec3(ent, "light_radius", tv);
		if(i>=0)
		{
			V3F_COPY(tv, lcur->boxval);

			V3F_SCALE(tv, -1.001, lcur->mins);
			V3F_SCALE(tv,  1.001, lcur->maxs);
			lcur->flags|=BTGE_LFL_BOXCULL;

			f=V3F_LEN(tv);
			g=(tv[0]+tv[1]+tv[2])/3;
			lcur->val[0]=(f+g)/2;

//			lcur->val[0]=V3F_LEN(tv);
		}

		i=BTGE_SEnt_GetVec3(ent, "light_start", tv1);
		j=BTGE_SEnt_GetVec3(ent, "light_end", tv2);
		if((i>=0) && (j>=0))
		{
			f=V3F_SUB(tv2, tv1, lcur->dir);
			lcur->val[0]=f;
		}

		//adjust light center within bbox
		i=BTGE_SEnt_GetVec3(ent, "light_center", tv);
		if(i>=0)
		{
			V3F_ADD(lcur->org, tv, lcur->org);
			V3F_SUB(lcur->mins, tv, lcur->mins);
			V3F_SUB(lcur->maxs, tv, lcur->maxs);
		}
	}

	if(lcur->val[0]==0)
		lcur->val[0]=250;

	if(lcur->val[0]==-1)
		{ lcur->flags|=BTGE_LFL_DISABLE; }

	lcur->curval=lcur->val[0];

	//TEST: adding glare
//	if(lcur->val[1]==0)
//		lcur->val[1]=lcur->val[0];

	lcur->flags=BTGE_SEnt_GetNum(ent, "_lightflags");

	i=BTGE_SEnt_GetNum(ent, "linear");
	if(i)lcur->flags|=BTGE_LFL_LINEAR;

	i=BTGE_SEnt_GetNum(ent, "noshadows");
	if(i)lcur->flags|=BTGE_LFL_NOSHADOW;

	i=BTGE_SEnt_GetNum(ent, "lightvol");
	if(i)lcur->flags|=BTGE_LFL_LIGHTVOL;

	s=BTGE_SEnt_GetStr(ent, "texture");
	if(s)
	{
		lcur->texname=dystrdup(s);
//		lcur->tex_falloff=BTGE_Texture_LoadImage(s);
	}

	s=BTGE_SEnt_GetStr(ent, "style");
	if(s)
	{
		i=atoi(s);
		if(!strcmp(s, "Slow Pulse"))i=1;
		if(!strcmp(s, "Slow Strobe"))i=2;
		lcur->style=i;
	}

	i=BTGE_SEnt_GetVec3(ent, "direction", lcur->dir);
	if(i>=0)lcur->flags|=BTGE_LFL_SPOTLIGHT;
	lcur->angle=BTGE_SEnt_GetNum(ent, "angle");

	return(lcur);
}

void BT_SpawnInit(BTGE_BrushWorld *world)
{
	static int init=0;

	if(init)return;
	init=1;

	BTGE_Voxel_SetSpawnerTick(BT_VoxelSpawnerTick);
	
	world->HandleLoadRegion=BT_Spawn_HandleLoadRegion;
	world->HandleSaveRegion=BT_Spawn_HandleSaveRegion;
}

int BT_Spawn_HandleLoadRegion(BTGE_BrushWorld *world, BTGE_VoxelRegion *rgn)
{
	BTGE_SEntity *ecur;
	BTGE_Light *lcur;
	char *s;
	int i, j, n, sk, nosp;

	sk=btge_skill;

	if(btGameIsDeathmatch())
		sk|=BT_SKILL_DEATHMATCH;
	if(btGameIsCoop())
		sk|=BT_SKILL_COOP;
	
	btge_spawn_org=vec3d(rgn->ox*32.0, rgn->oy*32.0, rgn->oz*32.0);
	
	ecur=rgn->sentity; n=1;
	while(ecur)
	{
		nosp=0;
	
		i=BTGE_SEnt_GetNum(ecur, "spawnflags");

#if 1
		//spawn_chance is a number from 1 to 100 indicating
		//the percentage of how likely an entity is to spawn.
		j=BTGE_SEnt_GetNum(ecur, "spawn_chance");
		if(j && ((btRandom()*100)>=j))
			nosp=1;
#endif

		if(	((i&BT_SPAWNFLAG_NOTEASY) && (sk==BT_SKILL_EASY)) ||
			((i&BT_SPAWNFLAG_NOTMEDIUM) && (sk==BT_SKILL_MEDIUM)) ||
			((i&BT_SPAWNFLAG_NOTHARD) && (sk==BT_SKILL_HARD)) ||
			((i&BT_SPAWNFLAG_NOTHARD) && (sk==BT_SKILL_NIGHTMARE)) ||
			((i&BT_SPAWNFLAG_NOTDEATHMATCH) && (sk&BT_SKILL_DEATHMATCH)) ||
			((i&BT_SPAWNFLAG_NOTCOOP) && (sk&BT_SKILL_COOP)) ||
			nosp)
		{
			if(ecur->bmdl)
			{
				BtSp_func_nowall(ecur);
			}else
			{
				s=BTGE_SEnt_GetStr(ecur, "target");
				if(s)
				{
					BtSp_misc_nospawn(ecur);
				}
			}
		
			ecur=ecur->next; continue;
		}

		s=BTGE_SEnt_GetStr(ecur, "classname");
		if(s && !strncmp(s, "light", 5))
		{
			lcur=BT_SpawnSEntityLight(ecur);
			lcur->num=n++;
			
			lcur->next=world->light;
			world->light=lcur;
		}

		BT_SpawnSEntity(ecur);
		ecur=ecur->next;
	}

	btge_spawn_org=vec3d(0, 0, 0);

	return(0);
}

int BT_Spawn_HandleSaveRegion(BTGE_BrushWorld *world, BTGE_VoxelRegion *rgn)
{
	return(0);
}

void BT_SpawnWorld(BTGE_BrushWorld *world)
{
	BTGE_SEntity *ecur;
	BTGE_Light *lcur;
	char *s;
	int i, j, n, sk, nosp;
	
	printf("BT_SpawnWorld %p\n", world);
//	*(int *)-1=-1;
	
	btge_time=0.1;	//clock reset
	btge_worldspawntime=btge_time;
	world->time_setactive=0;
	world->time_autovis=0;
	world->time_rebuildvis=0;
	world->time_turtle=0;

	BTGE_BrushWorld_DrawBuildFast(world);

	BT_SpawnInit(world);

	sk=btCvarGetf("skill");
	btge_skill=sk;

	if(btGameIsDeathmatch())
		sk|=BT_SKILL_DEATHMATCH;
	if(btGameIsCoop())
		sk|=BT_SKILL_COOP;

	world->light=NULL;
	ecur=world->entity; n=1;
	while(ecur)
	{
		nosp=0;
	
		i=BTGE_SEnt_GetNum(ecur, "spawnflags");

#if 1
		//handle Doom3-style flags
		j=i;
		if(BTGE_SEnt_GetNum(ecur, "not_easy"))
			i|=BT_SPAWNFLAG_NOTEASY;
		if(BTGE_SEnt_GetNum(ecur, "not_medium"))
			i|=BT_SPAWNFLAG_NOTMEDIUM;
		if(BTGE_SEnt_GetNum(ecur, "not_hard"))
			i|=BT_SPAWNFLAG_NOTHARD;
		if(BTGE_SEnt_GetNum(ecur, "not_deathmatch"))
			i|=BT_SPAWNFLAG_NOTDEATHMATCH;
		if(BTGE_SEnt_GetNum(ecur, "not_coop"))
			i|=BT_SPAWNFLAG_NOTCOOP;
		if(j!=i)BTGE_SEnt_SetNum(ecur, "spawnflags", i);
#endif

#if 1
		//spawn_chance is a number from 1 to 100 indicating
		//the percentage of how likely an entity is to spawn.
		j=BTGE_SEnt_GetNum(ecur, "spawn_chance");
		if(j && ((btRandom()*100)>=j))
			nosp=1;
#endif

		if(	((i&BT_SPAWNFLAG_NOTEASY) && (sk==BT_SKILL_EASY)) ||
			((i&BT_SPAWNFLAG_NOTMEDIUM) && (sk==BT_SKILL_MEDIUM)) ||
			((i&BT_SPAWNFLAG_NOTHARD) && (sk==BT_SKILL_HARD)) ||
			((i&BT_SPAWNFLAG_NOTHARD) && (sk==BT_SKILL_NIGHTMARE)) ||
			((i&BT_SPAWNFLAG_NOTDEATHMATCH) && (sk&BT_SKILL_DEATHMATCH)) ||
			((i&BT_SPAWNFLAG_NOTCOOP) && (sk&BT_SKILL_COOP)) ||
			nosp)
		{
			if(ecur->bmdl)
			{
				BtSp_func_nowall(ecur);
			}else
			{
				s=BTGE_SEnt_GetStr(ecur, "target");
				if(s)
				{
					BtSp_misc_nospawn(ecur);
				}
			}
		
			ecur=ecur->next; continue;
		}

		s=BTGE_SEnt_GetStr(ecur, "classname");
		if(s && !strncmp(s, "light", 5))
		{
			lcur=BT_SpawnSEntityLight(ecur);
			lcur->num=n++;
			
			lcur->next=world->light;
			world->light=lcur;
		}

		BT_SpawnSEntity(ecur);
		ecur=ecur->next;
	}
}

BTSV_API void btSpawnPrefab(char *name, vec3d origin)
{
	char tb[256];
	BTGE_BrushWorld *wrl;
	vec3d oldorg;
	int oldpf, pfnum, oldpfnum;
	
	wrl=BTGE_BrushPrefab_GetName(name);
	if(!wrl)
	{
		printf("btSpawnPrefab: fail get '%s'\n", name);
		return;
	}

	pfnum=btge_spawn_prefabmax++;
	oldpfnum=btge_spawn_prefabnum;
	btge_spawn_prefabnum=pfnum;

	oldpf=btge_spawn_isPrefab;
	btge_spawn_isPrefab=1;
	oldorg=btge_spawn_org;
	btge_spawn_org=v3dadd(btge_spawn_org, origin);
	BT_SpawnWorld(wrl);
	btge_spawn_org=oldorg;
	btge_spawn_isPrefab=oldpf;
	btge_spawn_prefabnum=oldpfnum;
}

// BTSV_API void btSpawnWorld()
BTSV_API void btsvSpawnWorld(void)
{
	btge_spawn_isPrefab=0;
	btge_spawn_lightnum=1024;
	btge_spawn_prefabnum=0;
	btge_spawn_prefabmax=0;
	BT_SpawnWorld(btge_world);
}

BTSV_API btEntity btSpawnPlayer(void)
{
	btEntity ent, spawn;
	
	ent=btNewEntity();
	spawn=btFindSpawnpoint();

	ent->classname="player";
	ent->solidtype=BT_SOLID_SLIDEBOX;
	ent->movetype=BT_MOVE_WALK;
//	ent->mins=vec3(-16, -16,  0);
//	ent->maxs=vec3( 16,  16, 56);
	ent->mins=vec3(-12, -12,  0);
	ent->maxs=vec3( 12,  12, 56);

	ent->vieworg=vec3(0, 0, 48);
//	player->origin=vec3vf(cam->org);
//	player->angles=vec3vf(cam->ang);

	btSetModel(ent, "models/monsters/soldier/soldier.model");

	ent->flags|=BT_FL_CLIENT;
	ent->takedamage=BT_DAMAGE_AIM;

	ent->itemflags|=BT_ITFL_HAMMERDRILL|BT_ITFL_BLASTER;

	ent->health=100;
	ent->maxHealth=100;
	ent->mass=200;

	ent->curMP=100;
	ent->maxMP=100;

	ent->curArmor=0;
	ent->maxArmor=100;

	if(spawn)
	{
		ent->origin=spawn->origin;
		ent->angles=spawn->angles;
		ent->effects2|=BT_EF2_CAM_FORCEANGLE;
		ent->pauseTime=btDelayTime(0.5);

		//certain spawn points have added special effects
		
		if(!strcmp(spawn->classname, "info_player_fixed"))
		{
			ent->effects2|=BT_EF2_CAM_FIXED_ORG;
			ent->effects2|=BT_EF2_CAM_FIXED_ROT;
		}
		if(!strcmp(spawn->classname, "info_player_fixed2d"))
		{
			ent->effects2|=BT_EF2_CAM_FIXED_ORG;
			ent->effects2|=BT_EF2_CAM_FIXED_ROT;
			ent->effects2|=BT_EF2_CAM_2D;
			ent->viewMins=spawn->viewMins;
			ent->viewMaxs=spawn->viewMaxs;
		}

		if(!strcmp(spawn->classname, "info_player_start2d"))
		{
			ent->effects2|=BT_EF2_CAM_FIXED_ROT;
			ent->effects2|=BT_EF2_CAM_2D;
			ent->viewMins=spawn->viewMins;
			ent->viewMaxs=spawn->viewMaxs;
		}
	}

	ent->origin=BTSV_Voxel_FindEntitySpawnPosition(ent, ent->origin);

	BT_PlayerWeaponChange(ent, 2);

	BT_RestoreClientState(ent);

//	btge_client=ent;
	btSetClientCamera(ent, ent);

	if(btGameIsDeathmatch())
	{
		btParticleTeleportExplosion(btCalcCenter(ent));
		btSound(ent, BT_CHAN_VOICE, "sound/misc/tele1",
			1.0, BT_ATTN_NORM);
	}

	return(ent);
}

BTSV_API btEntity btRespawnPlayer(btEntity ent)
{
	btEntity spawn;
	
//	ent=btNewEntity();
	spawn=btFindSpawnpoint();

	ent->classname="player";
	ent->solidtype=BT_SOLID_SLIDEBOX;
	ent->movetype=BT_MOVE_STEP;
	ent->mins=vec3(-16, -16,  0);
	ent->maxs=vec3( 16,  16, 56);
	ent->vieworg=vec3(0, 0, 48);
//	player->origin=vec3vf(cam->org);
//	player->angles=vec3vf(cam->ang);

	btSetModel(ent, "models/monsters/soldier/soldier.model");

	ent->flags|=BT_FL_CLIENT;
	ent->takedamage=BT_DAMAGE_AIM;

	ent->itemflags&=~BT_ITFL_GIVEALL;	//take away stuff
	ent->itemflags|=BT_ITFL_HAMMERDRILL|BT_ITFL_BLASTER;
	ent->effects2&=~BT_EF2_ISDEAD;

	ent->health=100;
	ent->maxHealth=100;
	ent->mass=200;

	ent->curMP=100;
	ent->maxMP=100;

	if(spawn)
	{
		ent->origin=spawn->origin;
		ent->angles=spawn->angles;
		ent->effects2|=BT_EF2_CAM_FORCEANGLE;
		ent->pauseTime=btDelayTime(0.5);

		//certain spawn points have added special effects
		
		if(!strcmp(spawn->classname, "info_player_fixed"))
		{
			ent->effects2|=BT_EF2_CAM_FIXED_ORG;
			ent->effects2|=BT_EF2_CAM_FIXED_ROT;
		}
		if(!strcmp(spawn->classname, "info_player_fixed2d"))
		{
			ent->effects2|=BT_EF2_CAM_FIXED_ORG;
			ent->effects2|=BT_EF2_CAM_FIXED_ROT;
			ent->effects2|=BT_EF2_CAM_2D;
			ent->viewMins=spawn->viewMins;
			ent->viewMaxs=spawn->viewMaxs;
		}

		if(!strcmp(spawn->classname, "info_player_start2d"))
		{
			ent->effects2|=BT_EF2_CAM_FIXED_ROT;
			ent->effects2|=BT_EF2_CAM_2D;
			ent->viewMins=spawn->viewMins;
			ent->viewMaxs=spawn->viewMaxs;
		}
	}

	BT_PlayerWeaponChange(ent, 2);

	BT_RestoreClientState(ent);

//	btge_client=ent;
	btSetClientCamera(ent, ent);

	if(btGameIsDeathmatch())
	{
		btParticleTeleportExplosion(btCalcCenter(ent));
		btSound(ent, BT_CHAN_VOICE, "sound/misc/tele1",
			1.0, BT_ATTN_NORM);
	}

	return(ent);
}

void BT_EntityRespawnEffect(btEntity ent)
{
	btParticleTeleportExplosion(btCalcCenter(ent));
	btSound(ent, BT_CHAN_VOICE, "sound/misc/tele1",
		1.0, BT_ATTN_NORM);
}

char *BTGE_SpawnPickRandomEnemy()
{
	btItem it;

	it=btGetRandomMonsterItem();

	return(it->classname);

#if 0
	cn="monster_army";
	
	f=btRandom();
	if(f<0.35)cn="monster_dog";
//	if(f<0.25)cn="monster_ogre";
//	if(f<0.20)cn="monster_knight";
//	if(f<0.15)cn="monster_shalrath";
//	if(f<0.10)cn="monster_shambler";

	if(f<0.10)cn="monster_ogre";
	if(f<0.08)cn="monster_knight";
	if(f<0.05)cn="monster_shalrath";
	if(f<0.01)cn="monster_shambler";
	if(f<0.0025)cn="monster_enemyhead";
#endif
}

BTSV_API void BT_VoxelSpawnerTick(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	BTGE_VoxelData *vox, int x, int y, int z)
{
	float org[3], org1[3], mins[3], maxs[3];
	btEntity ent, elst, ecur;
	float vsz, vsz2;
	char *cn;
	float f, g;
	int i, j, n;
	
	vsz=chk->rgn->voxsz;
	org[0]=(x+0.5)*vsz; org[1]=(y+0.5)*vsz; org[2]=(z+2.5)*vsz;

//	btParticleExplosion(vec3vf(org));
//	vox->delay=100+btRandom()*25;
	vox->delay=200+btRandom()*50;
	chk->tickdirty=1;

	if((btge_time-btge_worldspawntime)<20)
		return;

#if 1
	//spawm limiter
	elst=BT_CheckBoxEnts(
		v3dsub(vec3dvf(org), vec3d(512, 512, 256)),
		v3dadd(vec3dvf(org), vec3d(512, 512, 256)),
		BT_TRACE_MONSTERS, NULL);
	ecur=elst; n=0;
	while(ecur)
	{
		if(ecur->flags&BT_FL_MONSTER)
			{ n++; }
		ecur=ecur->chain;
	}
	
//	if(n>=8)
	if(n>=2)
	{
		btParticleExplosion(vec3dvf(org));
		return;
	}
#endif

#if 0
	cn="monster_army";
	
	f=btRandom();
	if(f<0.35)cn="monster_dog";
//	if(f<0.25)cn="monster_ogre";
//	if(f<0.20)cn="monster_knight";
//	if(f<0.15)cn="monster_shalrath";
//	if(f<0.10)cn="monster_shambler";

	if(f<0.10)cn="monster_ogre";
	if(f<0.08)cn="monster_knight";
	if(f<0.05)cn="monster_shalrath";
	if(f<0.01)cn="monster_shambler";
	if(f<0.0025)cn="monster_enemyhead";
#endif

	cn=BTGE_SpawnPickRandomEnemy();

	g=cos(2*M_PI*btTime()*(1.0/(24*60)));
	f=btCvarGetf("r_forcetime");
	if(f!=0.0)g=f;

	g=2*g+1;

	//spawn passive mob if spawner is exposed to sun.
	f=btRandom();
//	if((f<0.25) || btGameIsNoMonsters())
	if(((vox->slight&0xF0)>=0x40) &&
		((f<(0.25*g)) || btGameIsNoMonsters()))
	{
		f=btRandom();
		cn="passive_sheep";
		if(f<0.50)cn="passive_pig";
	}

	if(btGameIsDeathmatch() || btGameIsNoMonsters())
	{
		if(!strncmp(cn, "monster_", 8))
			return;
	}

	if(btGetSkill()==BT_SKILL_EASY)
	{
		if(!strncmp(cn, "monster_", 8))
		{
			f=btRandom();
			if(f<0.75)
				{ cn=NULL; }
		}
	}

	if(!cn)
	{
		btParticleExplosion(vec3dvf(org));
		return;
	}

	i=btPointBoxContents(vec3dvf(org),
		vec3(-24, -24, 0), vec3(24, 24, 48), NULL);
	if(!i)
	{
//		btParticleTeleportExplosion(vec3vf(org));
		ent=btSpawnEntityBasic(cn, vec3vf(org), 0);
		if(ent)
		{
			ent->flags|=BT_FL_AI_FREEROAM;
			ent->flags|=BT_FL_AI_DESPAWN;
//			ent->despawnTime=btTime()+10*60;
			ent->despawnTime=btTime()+5*60;
			BT_EntityRespawnEffect(ent);
		}
		//btSound(ent, BT_CHAN_VOICE, "sound/misc/tele1",
			//1.0, BT_ATTN_NORM);
		return;
	}
	
	//avoid spawning extra monsters...
	if(i&BT_CONTENTS_MONSTER)
		return;

	for(i=0; i<8; i++)
	{
		org1[0]=org[0]+((i&1)?32:-32);
		org1[1]=org[1]+((i&2)?32:-32);
		org1[2]=org[2]+((i&4)?32:0);

		j=btPointBoxContents(vec3dvf(org1),
			vec3(-24, -24, -24), vec3(24, 24, 48), NULL);
		if(!j)
		{
//			btParticleTeleportExplosion(vec3vf(org1));
			ent=btSpawnEntityBasic(cn, vec3vf(org1), 0);
			if(ent)
			{
				ent->flags|=BT_FL_AI_FREEROAM;
				ent->flags|=BT_FL_AI_DESPAWN;
				BT_EntityRespawnEffect(ent);
			}
			return;
		}
		
		//avoid spawning extra monsters
		if(j&BT_CONTENTS_MONSTER)
			return;
	}

}
