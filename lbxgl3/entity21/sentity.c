#include <lbxgl2.h>


LBXGL_API char *LBXGL_SEnt_GetStr(LBXGL_SEntity *ent, char *name)
{
	LBXGL_SEntPair *cur, *tmp;

	cur=ent->keys;
	while(cur)
	{
		if(!strcmp(cur->name, name))
			return(cur->value);
		cur=cur->next;
	}
	return(NULL);
}

LBXGL_API void LBXGL_SEnt_SetStr(LBXGL_SEntity *ent, char *name, char *val)
{
	LBXGL_SEntPair *cur, *tmp;

	ent->flags|=BTGE_SEFL_DIRTY;

	cur=ent->keys;
	while(cur)
	{
		if(!strcmp(cur->name, name))
		{
			cur->flags|=BTGE_SEFL_DIRTY;
			cur->value=dystrdup(val);
			return;
		}
		cur=cur->next;
	}

	tmp=gctalloc("lbxgl_sentpair_s", sizeof(LBXGL_SEntPair));
	tmp->name=dystrdup(name);
	tmp->value=dystrdup(val);
	tmp->flags|=BTGE_SEFL_DIRTY;

	if(ent->keys)
	{
		cur=ent->keys;
		while(cur->next)cur=cur->next;
		cur->next=tmp;
	}else
	{
		ent->keys=tmp;
	}
}

/** Updates SEntity without marking fields dirty */
LBXGL_API void LBXGL_SEnt_QuietSetStr(LBXGL_SEntity *ent,
	char *name, char *val)
{
	LBXGL_SEntPair *cur, *tmp;

//	ent->flags|=BTGE_SEFL_DIRTY;

	cur=ent->keys;
	while(cur)
	{
		if(!strcmp(cur->name, name))
		{
//			cur->flags|=BTGE_SEFL_DIRTY;
			cur->value=dystrdup(val);
			return;
		}
		cur=cur->next;
	}

	tmp=gctalloc("lbxgl_sentpair_s", sizeof(LBXGL_SEntPair));
	tmp->name=dystrdup(name);
	tmp->value=dystrdup(val);
//	tmp->flags|=BTGE_SEFL_DIRTY;

	if(ent->keys)
	{
		cur=ent->keys;
		while(cur->next)cur=cur->next;
		cur->next=tmp;
	}else
	{
		ent->keys=tmp;
	}
}

/** Only updates SEntity field if the entity is not dirty */
LBXGL_API void LBXGL_SEnt_SetStrNoDirty(LBXGL_SEntity *ent,
	char *name, char *val)
{
	if(ent->flags&BTGE_SEFL_DIRTY)
		return;
	LBXGL_SEnt_QuietSetStr(ent, name, val);
}

LBXGL_API float LBXGL_SEnt_GetNum(LBXGL_SEntity *ent, char *name)
{
	char *s;
	s=LBXGL_SEnt_GetStr(ent, name);
	if(s)return(atof(s));
	return(0);
}

LBXGL_API void LBXGL_SEnt_SetNum(LBXGL_SEntity *ent, char *name, float val)
{
	char buf[64];
	sprintf(buf, "%g", val);
	LBXGL_SEnt_SetStr(ent, name, buf);
}

LBXGL_API int LBXGL_SEnt_GetVec2(LBXGL_SEntity *ent, char *name, float *val)
{
	char *s;

	s=LBXGL_SEnt_GetStr(ent, name);
	if(!s) { V2F_ZERO(val); return(-1); }

	sscanf(s, "%g %g", val+0, val+1);
	return(0);
}

LBXGL_API int LBXGL_SEnt_GetVec3(LBXGL_SEntity *ent, char *name, float *val)
{
	char *s;

	s=LBXGL_SEnt_GetStr(ent, name);
	if(!s) { V3F_ZERO(val); return(-1); }

	sscanf(s, "%g %g %g", val+0, val+1, val+2);
	return(0);
}

LBXGL_API int LBXGL_SEnt_GetVec4(LBXGL_SEntity *ent, char *name, float *val)
{
	char *s;

	s=LBXGL_SEnt_GetStr(ent, name);
	if(!s) { V4F_ZERO(val); return(-1); }

	sscanf(s, "%g %g %g %g", val+0, val+1, val+2, val+3);
	return(0);
}

LBXGL_API void LBXGL_SEnt_SetVec2(LBXGL_SEntity *ent, char *name, float *val)
{
	char buf[64];
	sprintf(buf, "%.2f %.2f", val[0], val[1]);
	LBXGL_SEnt_SetStr(ent, name, buf);
}

LBXGL_API void LBXGL_SEnt_SetVec3(LBXGL_SEntity *ent, char *name, float *val)
{
	char buf[64];
	sprintf(buf, "%.2f %.2f %.2f", val[0], val[1], val[2]);
	LBXGL_SEnt_SetStr(ent, name, buf);
}

LBXGL_API void LBXGL_SEnt_SetVec4(LBXGL_SEntity *ent, char *name, float *val)
{
	char buf[64];
	sprintf(buf, "%.2f %.2f %.2f %.2f", val[0], val[1], val[2], val[3]);
	LBXGL_SEnt_SetStr(ent, name, buf);
}

LBXGL_API void LBXGL_SEnt_AddEntity(LBXGL_SEntity **wrl, LBXGL_SEntity *ent)
{
	LBXGL_SEntity *cur;

	if(*wrl)
	{
		cur=*wrl;
		while(cur->next)cur=cur->next;
		cur->next=ent;
	}else *wrl=ent;
}

LBXGL_API LBXGL_SEntity *LBXGL_SEnt_NewEntity(LBXGL_SEntity **wrl, char *type)
{
	LBXGL_SEntity *tmp;

	tmp=gctalloc("lbxgl_sentity_s", sizeof(LBXGL_SEntity));
	LBXGL_SEnt_SetStr(tmp, "classname", type);

	LBXGL_SEnt_AddEntity(wrl, tmp);
	return(tmp);
}

LBXGL_API LBXGL_SEntity *LBXGL_SEnt_NewBasicEntity(LBXGL_SEntity **wrl,
	char *type, float *org)
{
	LBXGL_SEntity *tmp;

	tmp=gctalloc("lbxgl_sentity_s", sizeof(LBXGL_SEntity));
	LBXGL_SEnt_SetStr(tmp, "classname", type);
	LBXGL_SEnt_SetVec3(tmp, "origin", org);

	LBXGL_SEnt_AddEntity(wrl, tmp);
	return(tmp);
}

LBXGL_API LBXGL_SEntity *LBXGL_SEnt_NewBasicLight(LBXGL_SEntity **wrl,
	float *org, float val)
{
	LBXGL_SEntity *tmp;

	tmp=gctalloc("lbxgl_sentity_s", sizeof(LBXGL_SEntity));
	LBXGL_SEnt_SetStr(tmp, "classname", "light");
	LBXGL_SEnt_SetVec3(tmp, "origin", org);
	LBXGL_SEnt_SetNum(tmp, "light", val);

	LBXGL_SEnt_SetStr(tmp, "color", "0.58 0.58 0.58");
	LBXGL_SEnt_SetNum(tmp, "linear", 0);

	LBXGL_SEnt_AddEntity(wrl, tmp);
	return(tmp);
}

LBXGL_API void LBXGL_SEnt_DestroyEntity(LBXGL_SEntity *ent)
{
	LBXGL_SEntPair *cur, *nxt;

	cur=ent->keys;
	while(cur)
	{
		nxt=cur->next;
		gcfree(cur);
		cur=nxt;
	}
	gcfree(ent);
}

LBXGL_API LBXGL_SEntity *LBXGL_SEnt_NewCloneEntity(LBXGL_SEntity **wrl,
	char *type, LBXGL_SEntity *sent)
{
	LBXGL_SEntity *tmp;
	LBXGL_SEntPair *cur;
	LBXGL_Brush *bcur;

	tmp=gctalloc("lbxgl_sentity_s", sizeof(LBXGL_SEntity));

	cur=sent->keys;
	while(cur)
	{
		LBXGL_SEnt_SetStr(tmp, cur->name, cur->value);
		cur=cur->next;
	}

	if(type)LBXGL_SEnt_SetStr(tmp, "classname", type);

	if(sent->brush)
	{
		tmp->brush=LBXGL_Brush_CloneBrushEntList(sent->brush);
		bcur=tmp->brush;
		while(bcur)
		{
			bcur->se_owner=tmp;
			bcur=bcur->enext;
		}
	}

	if(wrl)LBXGL_SEnt_AddEntity(wrl, tmp);
	return(tmp);
}
