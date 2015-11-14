#include <btgesv.h>


BTGE_API char *BTGE_SEnt_GetStr(BTGE_SEntity *ent, char *name)
{
	BTGE_SEntPair *cur, *tmp;

	cur=ent->keys;
	while(cur)
	{
		if(!strcmp(cur->name, name))
			return(cur->value);
		cur=cur->next;
	}
	return(NULL);
}

BTGE_API void BTGE_SEnt_SetStr(BTGE_SEntity *ent, char *name, char *val)
{
	BTGE_SEntPair *cur, *tmp;

	cur=ent->keys;
	while(cur)
	{
		if(!strcmp(cur->name, name))
		{
			cur->value=dystrdup(val);
			return;
		}
		cur=cur->next;
	}

	tmp=gctalloc("lbxgl_sentpair_s", sizeof(BTGE_SEntPair));
	tmp->name=dystrdup(name);
	tmp->value=dystrdup(val);

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

BTGE_API float BTGE_SEnt_GetNum(BTGE_SEntity *ent, char *name)
{
	char *s;
	s=BTGE_SEnt_GetStr(ent, name);
	if(s)return(atof(s));
	return(0);
}

BTGE_API void BTGE_SEnt_SetNum(BTGE_SEntity *ent, char *name, float val)
{
	char buf[64];
	sprintf(buf, "%g", val);
	BTGE_SEnt_SetStr(ent, name, buf);
}

BTGE_API int BTGE_SEnt_GetVec2(BTGE_SEntity *ent, char *name, float *val)
{
	char *s;

	s=BTGE_SEnt_GetStr(ent, name);
	if(!s) { V2F_ZERO(val); return(-1); }

	sscanf(s, "%g %g", val+0, val+1);
	return(0);
}

BTGE_API int BTGE_SEnt_GetVec3(BTGE_SEntity *ent, char *name, float *val)
{
	char *s;

	s=BTGE_SEnt_GetStr(ent, name);
	if(!s) { V3F_ZERO(val); return(-1); }

	sscanf(s, "%g %g %g", val+0, val+1, val+2);
	return(0);
}

BTGE_API int BTGE_SEnt_GetVec4(BTGE_SEntity *ent, char *name, float *val)
{
	char *s;

	s=BTGE_SEnt_GetStr(ent, name);
	if(!s) { V4F_ZERO(val); return(-1); }

	sscanf(s, "%g %g %g %g", val+0, val+1, val+2, val+3);
	return(0);
}

BTGE_API void BTGE_SEnt_SetVec2(BTGE_SEntity *ent, char *name, float *val)
{
	char buf[64];
	sprintf(buf, "%.2f %.2f", val[0], val[1]);
	BTGE_SEnt_SetStr(ent, name, buf);
}

BTGE_API void BTGE_SEnt_SetVec3(BTGE_SEntity *ent, char *name, float *val)
{
	char buf[64];
	sprintf(buf, "%.2f %.2f %.2f", val[0], val[1], val[2]);
	BTGE_SEnt_SetStr(ent, name, buf);
}

BTGE_API void BTGE_SEnt_SetVec4(BTGE_SEntity *ent, char *name, float *val)
{
	char buf[64];
	sprintf(buf, "%.2f %.2f %.2f %.2f", val[0], val[1], val[2], val[3]);
	BTGE_SEnt_SetStr(ent, name, buf);
}

BTGE_API void BTGE_SEnt_AddEntity(BTGE_SEntity **wrl, BTGE_SEntity *ent)
{
	BTGE_SEntity *cur;

	if(*wrl)
	{
		cur=*wrl;
		while(cur->next)cur=cur->next;
		cur->next=ent;
	}else *wrl=ent;
}

BTGE_API BTGE_SEntity *BTGE_SEnt_NewEntity(BTGE_SEntity **wrl, char *type)
{
	BTGE_SEntity *tmp;

	tmp=gctalloc("lbxgl_sentity_s", sizeof(BTGE_SEntity));
	BTGE_SEnt_SetStr(tmp, "classname", type);

	BTGE_SEnt_AddEntity(wrl, tmp);
	return(tmp);
}

BTGE_API BTGE_SEntity *BTGE_SEnt_NewBasicEntity(BTGE_SEntity **wrl,
	char *type, float *org)
{
	BTGE_SEntity *tmp;

	tmp=gctalloc("lbxgl_sentity_s", sizeof(BTGE_SEntity));
	BTGE_SEnt_SetStr(tmp, "classname", type);
	BTGE_SEnt_SetVec3(tmp, "origin", org);

	BTGE_SEnt_AddEntity(wrl, tmp);
	return(tmp);
}

BTGE_API BTGE_SEntity *BTGE_SEnt_NewBasicLight(BTGE_SEntity **wrl,
	float *org, float val)
{
	BTGE_SEntity *tmp;

	tmp=gctalloc("lbxgl_sentity_s", sizeof(BTGE_SEntity));
	BTGE_SEnt_SetStr(tmp, "classname", "light");
	BTGE_SEnt_SetVec3(tmp, "origin", org);
	BTGE_SEnt_SetNum(tmp, "light", val);

	BTGE_SEnt_SetStr(tmp, "color", "0.58 0.58 0.58");
	BTGE_SEnt_SetNum(tmp, "linear", 0);

	BTGE_SEnt_AddEntity(wrl, tmp);
	return(tmp);
}

BTGE_API void BTGE_SEnt_DestroyEntity(BTGE_SEntity *ent)
{
	BTGE_SEntPair *cur, *nxt;

	cur=ent->keys;
	while(cur)
	{
		nxt=cur->next;
		gcfree(cur);
		cur=nxt;
	}
	gcfree(ent);
}
