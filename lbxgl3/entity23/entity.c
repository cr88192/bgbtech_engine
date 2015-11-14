#include <lbxgl2.h>

#include <bgbccc.h>
#include <bgbgc.h>
#include <bgbdy.h>

static int lbxgl_entity_seq=1;

lbxEntCallback *lbxgl_entity_ghandlers=NULL;

LBXGL_API lbxEntity *lbxEntNew()
{
	lbxEntity *tmp;

	lbxEntInit();
	tmp=gctalloc("lbxgl_entity_t", sizeof(lbxEntity));
	return(tmp);
}

LBXGL_API lbxEntity *lbxEntNewClass(char *cn)
{
	lbxEntity *tmp;

	lbxEntInit();
	tmp=gctalloc("lbxgl_entity_t", sizeof(lbxEntity));
	return(tmp);
}

LBXGL_API lbxEntity *lbxEntNewWorld(lbxWorld *wrl)
{
	lbxEntity *cur, *tmp;

	tmp=lbxEntNew();
	if(!wrl->ents)
	{
		wrl->ents=tmp;
		return(tmp);
	}

	cur=wrl->ents;
	while(cur->next)cur=cur->next;
	cur->next=tmp;

	tmp->world=wrl;

	return(tmp);
}

LBXGL_API int lbxEntAllocId(lbxWorld *wrl)
{
	return(lbxgl_entity_seq++);
}

LBXGL_API LBXGL_Light *lbxEntNewLightWorld(lbxWorld *wrl)
	{ return(lbxWorldNewLight(wrl)); }
LBXGL_API LBXGL_Light *lbxEntNewDLightWorld(lbxWorld *wrl)
	{ return(lbxWorldNewLight(wrl)); }

LBXGL_API lbxEntity *lbxEntSpawnWorldSEnt(lbxWorld *wrl, LBXGL_SEntity *ent)
{
	char tb[256];
	lbxEntity *cur, *tmp;
	LBXGL_SEntPair *key;
	void *p;
	char *cn;

	if(!ent)return(NULL);
	cn=LBXGL_SEnt_GetStr(ent, "classname");
	if(!cn)return(NULL);

	sprintf(tb, "%s_init", cn);
	p=dyllGetAddr(tb);
	if(p) ((void(*)())p)();

	p=dyllGetAddr(cn);
	if(!p)
	{
		printf("lbxEntSpawnWorldSEnt: Failed Spawn '%s'\n", cn);
		return(NULL);
	}


	tmp=lbxEntNew();

	if(!wrl->ents)
	{
		wrl->ents=tmp;
	}else
	{
		cur=wrl->ents;
		while(cur->next)cur=cur->next;
		cur->next=tmp;

		tmp->world=wrl;
	}

	lbxEntSetOrg3F(tmp, 0,0,0);
	lbxEntSetVel3F(tmp, 0,0,0);
	lbxEntSetRot9F(tmp, 1,0,0, 0,1,0, 0,0,1);

	key=ent->keys;
	while(key)
	{
		lbxEntSetPropertyAsString(tmp, key->name, key->value);
		key=key->next;
	}

//	if(ent->light)
//		lbxEntSetProperty(tmp, "_light", ent->light);

	p=dyllGetAddr(cn);
	if(p)
	{
		((int(*)(lbxEntity *))p)(tmp);
	}else
	{
		printf("lbxEntSpawnWorldSEnt: Failed Spawn '%s'\n", cn);
	}

	return(tmp);
}


LBXGL_API void lbxEntUnlink(lbxEntity *ent)
{
	lbxEntity *cur, *lst;
	
	cur=ent->world->ents; lst=NULL;
	while(cur && cur!=ent) { lst=cur; cur=cur->next; }

	if(cur)
	{
		if(lst)
		{
			lst->next=cur->next;
		}else
		{
			ent->world->ents=cur->next;
		}
	}
}

LBXGL_API lbxEntProperty *lbxEntLookupProperty(lbxEntity *ent, char *name)
{
	lbxEntProperty *cur, *last;

	last=NULL;
	cur=ent->attr;
	while(cur)
	{
		if(!strcmp(cur->name, name))
		{
			if(last)
			{
				//mru move
				last->next=cur->next;
				cur->next=ent->attr;
				ent->attr=cur;
			}
			return(cur);
		}
		last=cur;
		cur=cur->next;
	}
	return(NULL);
}

LBXGL_API void *lbxEntGetProperty(lbxEntity *ent, char *name)
{
	lbxEntProperty *tmp;
	double *fa;
	float *fb;
	void *p;
	int i;

	tmp=lbxEntLookupProperty(ent, name);

	if(tmp)return(tmp->value);
	return(NULL);
}

LBXGL_API int lbxEntSetProperty(lbxEntity *ent, char *name, void *value)
{
	lbxEntProperty *tmp;
	double *fa;
	float *fb;
	int i;

	tmp=lbxEntLookupProperty(ent, name);
	if(tmp)
	{
		tmp->value=value;
		tmp->flag|=1;
		return(0);
	}

	tmp=gctalloc("lbxgl_entproperty_t", sizeof(lbxEntProperty));
	tmp->name=dystrsym(name);
	tmp->value=value;
	tmp->flag=1;

	tmp->next=ent->attr;
	ent->attr=tmp;
	return(0);
}

LBXGL_API int lbxEntGetPropertyFlag(lbxEntity *ent, char *name)
{
	lbxEntProperty *tmp;
	tmp=lbxEntLookupProperty(ent, name);
	if(tmp)return(tmp->flag);
	return(0);
}

LBXGL_API void lbxEntSetPropertyFlag(lbxEntity *ent, char *name, int flag)
{
	lbxEntProperty *tmp;
	tmp=lbxEntLookupProperty(ent, name);
	if(tmp)tmp->flag=flag;
}

LBXGL_API int lbxEntSetPropertyAsString(lbxEntity *ent,
	char *name, char *value)
{
	double *fa;
	float *fb;
	char **as2;
	void *p;
	char *s, *t;
	int i, n;

//	s=lbxEntGetDeclSig(tmp, name);
//	if(s)
//	{
		//ERRM: need string+sig serialization/deserialization
//	}

	t=value;
	while(*t && (*t<=' '))t++;
	if(((*t>='0') && (*t<='9')) || (*t=='-'))
	{
		as2=gcrsplit(value);
		for(n=0; as2[n]; n++);

		if(n>1)
		{
			fa=gctalloc("bggl2_numarray_t", n*sizeof(double));
			for(i=0; i<n; i++)fa[i]=atof(as2[i]);
			p=fa;
		}else p=dyfloat(atof(as2[0]));
	}else p=dystrdup(value);

	i=lbxEntSetProperty(ent, name, p);
	return(i);
}

LBXGL_API char *lbxEntGetPropertyString(lbxEntity *ent, char *name)
{
	void *p;
	p=lbxEntGetProperty(ent, name);
	return(p);
}

LBXGL_API double lbxEntGetPropertyFloat(lbxEntity *ent, char *name)
{
	void *p;
	p=lbxEntGetProperty(ent, name);
	if(!p)return(0);
	return(dyfloatv(p));
}

LBXGL_API int lbxEntGetPropertyBool(lbxEntity *ent, char *name)
{
	void *p;
	p=lbxEntGetProperty(ent, name);
	if(!p)return(0);
	return(dytruep(p));
}

LBXGL_API int lbxEntGetPropertyFVector(lbxEntity *ent, char *name,
	float *vec, int num)
{
	double *fa;
	int i;

	fa=lbxEntGetProperty(ent, name);
	if(!fa)
	{
		VecNF_Zero(vec, num);
		return(-1);
	}

	for(i=0; i<num; i++)vec[i]=fa[i];
	return(0);
}

LBXGL_API int lbxEntGetPropertyDVector(lbxEntity *ent, char *name,
	double *vec, int num)
{
	double *fa;
	int i;

	fa=lbxEntGetProperty(ent, name);
	if(!fa)
	{
		VecND_Zero(vec, num);
		return(-1);
	}

	for(i=0; i<num; i++)vec[i]=fa[i];
	return(0);
}

LBXGL_API int lbxEntSetPropertyString(lbxEntity *ent, char *name, char *value)
{
	return(lbxEntSetProperty(ent, name, dystrdup(value)));
}

LBXGL_API int lbxEntSetPropertyFloat(lbxEntity *ent, char *name, double value)
{
	lbxEntProperty *tmp;
	double *fa;
	int i;

	i=lbxEntSetProperty(ent, name, dyfloat(value));
	return(i);
}

LBXGL_API int lbxEntSetPropertyBool(lbxEntity *ent, char *name, int value)
{
	return(lbxEntSetProperty(ent, name, dyint(value)));
}

LBXGL_API int lbxEntSetPropertyFVector(lbxEntity *ent, char *name,
	float *value, int num)
{
	lbxEntProperty *tmp;
	double *fa;
	int i;

	tmp=lbxEntLookupProperty(ent, name);
	if(tmp)
	{
		fa=tmp->value;
		for(i=0; i<num; i++)fa[i]=value[i];
		tmp->flag|=1;
		return(0);
	}

	fa=gctalloc("bggl2_numarray_t", num*sizeof(double));
	for(i=0; i<num; i++)fa[i]=value[i];

	return(lbxEntSetProperty(ent, name, fa));
}

LBXGL_API int lbxEntSetPropertyDVector(lbxEntity *ent, char *name,
	double *value, int num)
{
	lbxEntProperty *tmp;
	double *fa;
	int i;

	tmp=lbxEntLookupProperty(ent, name);
	if(tmp)
	{
		fa=tmp->value;
		for(i=0; i<num; i++)fa[i]=value[i];
		tmp->flag|=1;
		return(0);
	}

	fa=gctalloc("bggl2_numarray_t", num*sizeof(double));
	for(i=0; i<num; i++)fa[i]=value[i];

	return(lbxEntSetProperty(ent, name, fa));
}

LBXGL_API int lbxEntSetPropertyFVector2(lbxEntity *ent, char *name,
	float *value, int num)
{
	lbxEntProperty *tmp;
	double *fa;
	int i;

	tmp=lbxEntLookupProperty(ent, name);
	if(tmp)
	{
		fa=tmp->value;
		for(i=0; i<num; i++)fa[i]=value[i];
		tmp->flag&=~1;
		return(0);
	}

	fa=gctalloc("bggl2_numarray_t", num*sizeof(double));
	for(i=0; i<num; i++)fa[i]=value[i];

	return(lbxEntSetProperty(ent, name, fa));
}

LBXGL_API int lbxEntSetPropertyDVector2(lbxEntity *ent, char *name,
	double *value, int num)
{
	lbxEntProperty *tmp;
	double *fa;
	int i;

	tmp=lbxEntLookupProperty(ent, name);
	if(tmp)
	{
		fa=tmp->value;
		for(i=0; i<num; i++)fa[i]=value[i];
		tmp->flag&=~1;
		return(0);
	}

	fa=gctalloc("bggl2_numarray_t", num*sizeof(double));
	for(i=0; i<num; i++)fa[i]=value[i];

	return(lbxEntSetProperty(ent, name, fa));
}

#if 0
// lbxEntCallback *lbxEntGetCallback(lbxEntity *ent, char *name)
{
	lbxEntCallback *cur, *tmp;

	if(name)
	{
		cur=ent->cb;
		while(cur)
		{
			if(cur->name)if(!strcmp(cur->name, name))
				return(cur);
			cur=cur->next;
		}
	}else
	{
		cur=ent->cb;
		while(cur)
		{
			if(!cur->name)return(cur);
			cur=cur->next;
		}
	}

	tmp=gctalloc("lbxgl_entcallback_t", sizeof(lbxEntCallback));
	if(name)tmp->name=dystrsym(name);
	tmp->next=ent->cb;
	ent->cb=tmp;
	return(tmp);
}

// void lbxEntSetCallbackN(lbxEntity *ent, char *name, int n,
	void *(*func)())
{
	lbxEntCallback *cb;
	cb=lbxEntGetCallback(ent, name);
	if(n==0)cb->f0=func;
	if(n==1)cb->f1=func;
	if(n==2)cb->f2=func;
	if(n==3)cb->f3=func;
	if(n==4)cb->f4=func;
	if(n==5)cb->f5=func;
	if(n==6)cb->f6=func;
	if(n==7)cb->f7=func;
	if(n==8)cb->f8=func;
}


// int lbxEntSetCallback(lbxEntity *ent, char *name, void *data,
	void *(*func)(lbxEntity *ent, void *data, void **args, int nargs))
{
	lbxEntCallback *cur, *tmp;

	if(name)
	{
		cur=ent->cb;
		while(cur)
		{
			if(cur->name)if(!strcmp(cur->name, name))
			{
				cur->func=func;
				cur->data=data;
				return(0);
			}
			cur=cur->next;
		}
	}

	tmp=gctalloc("lbxgl_entcallback_t", sizeof(lbxEntCallback));
	if(name)tmp->name=dysymbol(name);
	tmp->func=func;
	tmp->data=data;

	tmp->next=ent->cb;
	ent->cb=tmp;
	return(0);
}

// int lbxEntSetGCallback(char *name, void *data,
	void *(*func)(lbxEntity *ent, void *data, void **args, int nargs))
{
	lbxEntCallback *cur, *tmp;

	if(name)
	{
		cur=lbxgl_entity_ghandlers;
		while(cur)
		{
			if(cur->name)if(!strcmp(cur->name, name))
			{
				cur->func=func;
				cur->data=data;
				return(0);
			}
			cur=cur->next;
		}
	}

	tmp=gctalloc("lbxgl_entcallback_t", sizeof(lbxEntCallback));
	if(name)tmp->name=dysymbol(name);
	tmp->func=func;
	tmp->data=data;

	tmp->next=lbxgl_entity_ghandlers;
	lbxgl_entity_ghandlers=tmp;
	return(0);
}

// void *lbxEntInvokeCallback(lbxEntity *ent,
	lbxEntCallback *cb, void **args, int nargs)
{
	if((nargs==0) && cb->f0)return(cb->f0(ent));
	if((nargs==1) && cb->f1)return(cb->f1(ent, args[0]));
	if((nargs==2) && cb->f2)return(cb->f2(ent, args[0], args[1]));
	if((nargs==3) && cb->f3)return(cb->f3(ent, args[0], args[1], args[2]));

	if((nargs==4) && cb->f4)return(cb->f4(ent, args[0], args[1], args[2],
		args[3]));
	if((nargs==5) && cb->f5)return(cb->f5(ent, args[0], args[1], args[2],
		args[3], args[4]));
	if((nargs==6) && cb->f6)return(cb->f6(ent, args[0], args[1], args[2],
		args[3], args[4], args[5]));
	if((nargs==7) && cb->f7)return(cb->f7(ent, args[0], args[1], args[2],
		args[3], args[4], args[5], args[6]));
	if((nargs==8) && cb->f8)return(cb->f8(ent, args[0], args[1], args[2],
		args[3], args[4], args[5], args[6], args[7]));

	if(cb->func)return(cb->func(ent, cb->data, args, nargs));
	return(UNDEFINED);
}

// void *lbxEntSendMessage(lbxEntity *ent, char *name, void **args, int nargs)
{
	lbxEntCallback *cur;
	void *p;
	int i;

	cur=ent->cb;
	while(cur)
	{
		if(cur->name)if(!strcmp(cur->name, name))
		{
			p=lbxEntInvokeCallback(ent, cur, args, nargs);
			return(p);
		}
		cur=cur->next;
	}

	cur=lbxgl_entity_ghandlers;
	while(cur)
	{
		if(cur->name)if(!strcmp(cur->name, name))
		{
			p=lbxEntInvokeCallback(ent, cur, args, nargs);
			return(p);
		}
		cur=cur->next;
	}

	cur=ent->cb;
	while(cur)
	{
		if(!cur->name)
		{
			p=lbxEntInvokeCallback(ent, cur, args, nargs);
			if(p!=UNDEFINED)return(p);
		}
		cur=cur->next;
	}

	cur=lbxgl_entity_ghandlers;
	while(cur)
	{
		if(!cur->name)
		{
			p=lbxEntInvokeCallback(ent, cur, args, nargs);
			if(p!=UNDEFINED)return(p);
		}
		cur=cur->next;
	}

	return(UNDEFINED);
}

// int lbxEntSendMessageAll(lbxEntity *ents, char *name,
	void **args, int nargs)
{
	lbxEntity *cur;

	cur=ents;
	while(cur)
	{
		lbxEntSendMessage(cur, name, args, nargs);
		cur=cur->next;
	}
	return(0);
}
#endif

void LBXGL_Entity_ExpandSig(char *s, char *t)
{
	int i;

	while(*s)
	{
		if((*s=='L')||(*s=='X')||(*s=='U')||
			(*s=='u')||(*s=='N')||(*s=='M'))
		{
			while(*s && *s!=';')
				*t++=*s++;
			if(*s==';')*t++=*s++;
			continue;
		}
		if(*s=='A')
		{
			s++;
			i=*s++;
			switch(i)
			{
			case 'e':
				strcpy(t, "PXlbxEntity_s;");
				t+=strlen(t);
				break;
			case 'w':
				strcpy(t, "PXLBXGL_BrushWorld_s;");
				t+=strlen(t);
				break;
			default:
				*t++='A';
				*t++=i;
				break;
			}
			continue;
		}

		*t++=*s++;
	}
	*t++=0;
}

LBXGL_API void lbxEntDecl(lbxEntity *ent, char *name, char *sig)
{
	char tb1[256], tb2[256];

	LBXGL_Entity_ExpandSig(sig, tb2);

	sprintf(tb1, "lbxgl_entity/%s", name);
	dyllMetaBindKey(tb1, "entity_decl");

	sprintf(tb1, "lbxgl_entity/%s:sig", name);
	dyllMetaBindKey(tb1, tb2);
}

LBXGL_API char *lbxEntGetDeclSig(lbxEntity *ent, char *name)
{
	char tb[256];
	char *s;

	sprintf(tb, "lbxgl_entity/%s:sig", name);
	s=dyllMetaLookupKey(tb);
	return(s);
}

lbxEntCallback *lbxEntGetCallback(lbxEntity *ent, char *name)
{
	lbxEntCallback *cur, *tmp;

	cur=ent->cb;
	while(cur)
	{
		if(cur->name)if(!strcmp(cur->name, name))
			return(cur);
		cur=cur->next;
	}

	tmp=gctalloc("lbxgl_entcallback_t", sizeof(lbxEntCallback));
	tmp->name=dystrsym(name);
	tmp->next=ent->cb;
	ent->cb=tmp;
	return(tmp);
}

LBXGL_API int lbxEntSetMethod(lbxEntity *ent, char *name, void *func)
{
	lbxEntCallback *tmp;
	tmp=lbxEntGetCallback(ent, name);
	tmp->func=func;
	return(0);
}

LBXGL_API void *lbxEntGetMethod(lbxEntity *ent, char *name)
{
	lbxEntCallback *tmp;
	tmp=lbxEntGetCallback(ent, name);
	return(tmp->func);
}


LBXGL_API int lbxEntCalli(lbxEntity *ent, char *name, ...)
{
	va_list lst;
	void *p;
	char *s;
	int i;

	p=lbxEntGetMethod(ent, name);
	if(!p)return(0);

	s=lbxEntGetDeclSig(ent, name);
	if(!s)s=dyllGetAddrSig(p);

	va_start(lst, name);
	p=dyllApplyPtrSigObjV(p, s, ent, lst);
//	p=dyllApplyPtrObjV(p, ent, lst);
	va_end(lst);
	if(!p)return(0);
	return(*(int *)p);
}

LBXGL_API s64 lbxEntCalll(lbxEntity *ent, char *name, ...)
{
	va_list lst;
	void *p;
	char *s;
	int i;

	p=lbxEntGetMethod(ent, name);
	if(!p)return(0);

	s=lbxEntGetDeclSig(ent, name);
	if(!s)s=dyllGetAddrSig(p);

	va_start(lst, name);
	p=dyllApplyPtrSigObjV(p, s, ent, lst);
//	p=dyllApplyPtrObjV(p, ent, lst);
	va_end(lst);
	if(!p)return(0);
	return(*(s64 *)p);
}

LBXGL_API float lbxEntCallf(lbxEntity *ent, char *name, ...)
{
	va_list lst;
	void *p;
	char *s;
	int i;

	p=lbxEntGetMethod(ent, name);
	if(!p)return(0);

	s=lbxEntGetDeclSig(ent, name);
	if(!s)s=dyllGetAddrSig(p);

	va_start(lst, name);
	p=dyllApplyPtrSigObjV(p, s, ent, lst);
//	p=dyllApplyPtrObjV(p, ent, lst);
	va_end(lst);
	if(!p)return(0);
	return(*(float *)p);
}

LBXGL_API double lbxEntCalld(lbxEntity *ent, char *name, ...)
{
	va_list lst;
	void *p;
	char *s;
	int i;

	p=lbxEntGetMethod(ent, name);
	if(!p)return(0);

	s=lbxEntGetDeclSig(ent, name);
	if(!s)s=dyllGetAddrSig(p);

	va_start(lst, name);
	p=dyllApplyPtrSigObjV(p, s, ent, lst);
//	p=dyllApplyPtrObjV(p, ent, lst);
	va_end(lst);
	if(!p)return(0);
	return(*(double *)p);
}

LBXGL_API void *lbxEntCallp(lbxEntity *ent, char *name, ...)
{
	va_list lst;
	void *p;
	char *s;
	int i;

	p=lbxEntGetMethod(ent, name);
	if(!p)return(NULL);

	s=lbxEntGetDeclSig(ent, name);
	if(!s)s=dyllGetAddrSig(p);

	va_start(lst, name);
	p=dyllApplyPtrSigObjV(p, s, ent, lst);
//	p=dyllApplyPtrObjV(p, ent, lst);
	va_end(lst);
	if(!p)return(NULL);
	return(*(void **)p);
}

LBXGL_API int lbxEntInit()
{
	static int init=0;

	if(init)return(0);
	init=1;

	ccLoadModule("lbxgl2.h");

	return(0);
}
