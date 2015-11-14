#include <bgbgc.h>
#include <bgbdy.h>

// dyt dyoo_objstack[256];
// int dyoo_objstackpos;

BGBDY_ObjVTab *dyoo_vtab_first=NULL;

void dyoo_init()
{
}

BGBDY_ObjVTab *BGBDY_GetTypeVTab(char *str)
{
	BGBDY_ObjVTab *ty;
	ty=BGBGC_GetTypeVTab(str);
	if(!ty)
	{
		ty=gcalloc(sizeof(BGBDY_ObjVTab));
		memset(ty, 0, sizeof(BGBDY_ObjVTab));
		ty->next=dyoo_vtab_first;
		dyoo_vtab_first=ty;
		BGBGC_SetTypeVTab(str, ty);
	}
	return(ty);
}

BGBDY_ObjVTab *BGBDY_GetObjVTab(void *obj)
{
	BGBDY_ObjVTab *ty;
	char *s;

	ty=BGBGC_GetObjVTab(obj);
	if(ty)return(ty);

	s=dygettype(obj);
	ty=BGBDY_GetTypeVTab(s);
	return(ty);
}

BGBDY_ObjVTab *BGBDY_GetObjVTabFast(void *obj)
{
	BGBDY_ObjVTab *ty;
	ty=BGBGC_GetObjVTab(obj);
	return(ty);
}

int BGBDY_SetGetSlot(char *type, void *(*fn)(void *p, char *sym))
{
	BGBDY_ObjVTab *ty;
	ty=BGBDY_GetTypeVTab(type);
	ty->get_slot=fn;
	return(0);
}

int BGBDY_SetSetSlot(char *type, void *(*fn)(void *p, char *sym, void *val))
{
	BGBDY_ObjVTab *ty;
	ty=BGBDY_GetTypeVTab(type);
	ty->set_slot=fn;
	return(0);
}

int BGBDY_SetNextSlot(char *type, char *(*fn)(void *p, char *sym))
{
	BGBDY_ObjVTab *ty;
	ty=BGBDY_GetTypeVTab(type);
	ty->nxt_slot=fn;
	return(0);
}

int BGBDY_SetGetDel(char *type, void *(*fn)(void *p, char *sym))
{
	BGBDY_ObjVTab *ty;
	ty=BGBDY_GetTypeVTab(type);
	ty->get_del=fn;
	return(0);
}

int BGBDY_SetSetDel(char *type, void *(*fn)(void *p, char *sym, void *val))
{
	BGBDY_ObjVTab *ty;
	ty=BGBDY_GetTypeVTab(type);
	ty->set_del=fn;
	return(0);
}

int BGBDY_SetNextDel(char *type, char *(*fn)(void *p, char *sym))
{
	BGBDY_ObjVTab *ty;
	ty=BGBDY_GetTypeVTab(type);
	ty->nxt_del=fn;
	return(0);
}

int BGBDY_SetTrySetSlot(char *type, void *(*fn)(void *p, char *sym, void *val))
{
	BGBDY_ObjVTab *ty;
	ty=BGBDY_GetTypeVTab(type);
	ty->try_set_slot=fn;
	return(0);
}

int BGBDY_SetCallMethod(char *type, void *(*fn)(
	void *p, char *sym, void **args, int n))
{
	BGBDY_ObjVTab *ty;
	ty=BGBDY_GetTypeVTab(type);
	ty->call_method=fn;
	return(0);
}

int BGBDY_SetApply(char *type, void *(*fn)(void *p, void **args, int n))
{
	BGBDY_ObjVTab *ty;
	ty=BGBDY_GetTypeVTab(type);
	ty->apply=fn;
	return(0);
}

int BGBDY_SetApplyObj(char *type,
	void *(*fn)(void *p, void *o, void **args, int n))
{
	BGBDY_ObjVTab *ty;
	ty=BGBDY_GetTypeVTab(type);
	ty->applyobj=fn;
	return(0);
}

int BGBDY_SetToString(char *type, char *(*fn)(void *p))
{
	BGBDY_ObjVTab *ty;
	ty=BGBDY_GetTypeVTab(type);
	ty->tostring=fn;
	return(0);
}

int BGBDY_SetVtabMethod(char *type, char *name, void *func)
{
	BGBDY_ObjVTab *ty;
	int i;

	ty=BGBDY_GetTypeVTab(type);

	if(!ty->vtab_name)
	{
		ty->m_vtab=16; ty->n_vtab=0;
		ty->vtab_func=gcalloc(ty->m_vtab*sizeof(void *));
		ty->vtab_name=gcalloc(ty->m_vtab*sizeof(char *));
	}

	for(i=0; i<ty->n_vtab; i++)
		if(!strcmp(ty->vtab_name[i], name))
	{
		ty->vtab_func[i]=func;
		return(0);
	}

	if((ty->n_vtab+1)>=ty->m_vtab)
	{
		i=ty->m_vtab+(ty->m_vtab>>1);
		ty->vtab_name=gcrealloc(ty->vtab_name, i*sizeof(char *));
		ty->vtab_func=gcrealloc(ty->vtab_func, i*sizeof(void *));
		ty->m_vtab=i;
	}

	i=ty->n_vtab++;
	ty->vtab_name[i]=dystrdup(name);
	ty->vtab_func[i]=func;

	return(0);
}

void *BGBDY_GetTypeVtabMethod(char *type, char *name)
{
	BGBDY_ObjVTab *ty;
	int i;

	ty=BGBDY_GetTypeVTab(type);
	if(!ty)return(NULL);

	if(!(ty->vtab_name))return(NULL);
	for(i=0; i<ty->n_vtab; i++)
		if(!strcmp(ty->vtab_name[i], name))
			return(ty->vtab_func[i]);
	return(NULL);
}

void *BGBDY_GetVtabMethod(void *obj, char *name)
{
	BGBDY_ObjVTab *ty;
	int i;

	if(!obj)return(NULL);

	ty=BGBDY_GetObjVTab(obj);
	if(!(ty->vtab_name))return(NULL);

	for(i=0; i<ty->n_vtab; i++)
		if(!strcmp(ty->vtab_name[i], name))
			return(ty->vtab_func[i]);
	return(NULL);
}


void *BGBDY_GetSlot(void *p, char *s)
{
	BGBDY_ObjVTab *ty;

	if(!p)return(UNDEFINED);

	ty=BGBDY_GetObjVTab(p);
	if(!ty)return(UNDEFINED);
	if(ty->get_slot)
		return(ty->get_slot(p, s));
	return(UNDEFINED);
}

void *BGBDY_SetSlot(void *p, char *s, void *v)
{
	BGBDY_ObjVTab *ty;

	if(!p)return(UNDEFINED);

	ty=BGBDY_GetObjVTab(p);
	if(!ty)return(UNDEFINED);
	if(ty->set_slot)
		return(ty->set_slot(p, s, v));
	return(UNDEFINED);
}

char *BGBDY_NextSlot(void *p, char *s)
{
	BGBDY_ObjVTab *ty;

	if(!p)return(UNDEFINED);

	ty=BGBDY_GetObjVTab(p);
	if(!ty)return(UNDEFINED);
	if(ty->nxt_slot)
		return(ty->nxt_slot(p, s));
	return(UNDEFINED);
}

void *BGBDY_GetDel(void *p, char *s)
{
	BGBDY_ObjVTab *ty;

	if(!p)return(UNDEFINED);

	ty=BGBDY_GetObjVTab(p);
	if(!ty)return(UNDEFINED);
	if(ty->get_del)
		return(ty->get_del(p, s));
	return(UNDEFINED);
}

void *BGBDY_SetDel(void *p, char *s, void *v)
{
	BGBDY_ObjVTab *ty;

	if(!p)return(UNDEFINED);

	ty=BGBDY_GetObjVTab(p);
	if(!ty)return(UNDEFINED);
	if(ty->set_del)
		return(ty->set_del(p, s, v));
	return(UNDEFINED);
}

char *BGBDY_NextDel(void *p, char *s)
{
	BGBDY_ObjVTab *ty;

	if(!p)return(UNDEFINED);

	ty=BGBDY_GetObjVTab(p);
	if(!ty)return(UNDEFINED);
	if(ty->nxt_del)
		return(ty->nxt_del(p, s));
	return(UNDEFINED);
}

void *BGBDY_TrySetSlot(void *p, char *s, void *v)
{
	BGBDY_ObjVTab *ty;

	if(!p)return(UNDEFINED);

	ty=BGBDY_GetObjVTab(p);
	if(!ty)return(UNDEFINED);
	if(ty->try_set_slot)
		return(ty->try_set_slot(p, s, v));
	return(UNDEFINED);
}

void *BGBDY_CallMethod(void *p, char *s, void **a, int n)
{
	BGBDY_ObjVTab *ty;

	if(!p)return(UNDEFINED);

	ty=BGBDY_GetObjVTab(p);
	if(!ty)return(UNDEFINED);
	if(ty->call_method)
		return(ty->call_method(p, s, a, n));
	return(UNDEFINED);
}

void *BGBDY_Apply(void *p, void **a, int n)
{
	BGBDY_ObjVTab *ty;

	if(!p)return(UNDEFINED);

	ty=BGBDY_GetObjVTab(p);
	if(!ty)return(UNDEFINED);
	if(ty->apply)
		return(ty->apply(p, a, n));
	return(UNDEFINED);
}

int BGBDY_CanApplyP(void *p)
{
	BGBDY_ObjVTab *ty;

	if(!p)return(0);

	ty=BGBDY_GetObjVTab(p);
	if(!ty)return(0);
	if(ty->apply)return(1);
	return(0);
}

void *BGBDY_ApplyObj(void *p, void *o, void **a, int n)
{
	BGBDY_ObjVTab *ty;

	if(!p)return(UNDEFINED);

	ty=BGBDY_GetObjVTab(p);
	if(!ty)return(UNDEFINED);
	if(ty->applyobj)return(ty->applyobj(p, o, a, n));
	if(ty->apply)return(ty->apply(p, a, n));
	return(UNDEFINED);
}

int BGBDY_CanApplyObjP(void *p)
{
	BGBDY_ObjVTab *ty;

	if(!p)return(0);

	ty=BGBDY_GetObjVTab(p);
	if(!ty)return(0);
	if(ty->applyobj)return(1);
	return(0);
}

char *BGBDY_ToString(void *p)
{
	BGBDY_ObjVTab *ty;

	if(!p)return(NULL);

	ty=BGBDY_GetObjVTab(p);
	if(!ty)return(NULL);
	if(ty->tostring)
		return(ty->tostring(p));
	return(NULL);
}

BGBDY_API char *dyToString(dyt obj)
{
	char tb[4096];
	char *s, *ty;

	s=BGBDY_ToString((void *)obj);
	if(s)
	{
		if(dystringp((dyt)s))
			return(s);
		return(dystrdup(s));
	}

	dysDumpStr(tb, obj);
	return(dystrdup(tb));
}

int BGBDY_SetCopyValue(char *type, dyt (*fn)(dyt p))
{
	BGBDY_ObjVTab *ty;
	ty=BGBDY_GetTypeVTab(type);
	ty->copyValue=fn;
	return(0);
}

int BGBDY_SetDropValue(char *type, int (*fn)(dyt p))
{
	BGBDY_ObjVTab *ty;
	ty=BGBDY_GetTypeVTab(type);
	ty->dropValue=fn;
	return(0);
}

BGBDY_API int dyCheckValueNoCopyP(dyt p)
{
	if(BGBDY_IsFixRealFP((byte *)p))
		return(1);
	return(0);
}

/** If passed a value type, return a copy */
BGBDY_API dyt dyCopyValue(dyt p)
{
	BGBDY_ObjVTab *ty;
	if(dyCheckValueNoCopyP(p))
		return(p);
	ty=BGBDY_GetObjVTabFast(p);
	if(ty && ty->copyValue)
		return(ty->copyValue(p));
	return(p);
}

/** If passed a value type, free it.
  * Should return non-zero if a value-type was recieved. */
BGBDY_API int dyDropValue(dyt p)
{
	BGBDY_ObjVTab *ty;
	if(dyCheckValueNoCopyP(p))
		return(0);
	ty=BGBDY_GetObjVTabFast(p);
	if(ty && ty->dropValue)
		return(ty->dropValue(p));
	return(0);
}

BGBDY_API void dyTypeSetClassAlias(char *type, char *qname)
{
	BGBDY_ObjVTab *ty;
	ty=BGBDY_GetTypeVTab(type);
	ty->classAlias=dystrdup(qname);
}

BGBDY_API void dyTypeEncodeBinary(char *type,
	void (*fcn)(dysObjectStream ctx, dyt val))
{
	BGBDY_ObjVTab *ty;
	ty=BGBDY_GetTypeVTab(type);
	ty->encodeBinary=fcn;
}

BGBDY_API void dyTypeDecodeBinary(char *type,
	dyt (*fcn)(dysObjectStream ctx))
{
	BGBDY_ObjVTab *ty;
	ty=BGBDY_GetTypeVTab(type);
	ty->decodeBinary=fcn;
}

BGBDY_API void dyTypeGetSlot(char *ty, dyt (*fcn)(dyt, char*))
	{ BGBDY_SetGetSlot(ty, (void *(*)(void*,char*))fcn); }
BGBDY_API void dyTypeSetSlot(char *ty, dyt (*fcn)(dyt, char*, dyt))
	{ BGBDY_SetSetSlot(ty, (void *(*)(void*,char*,void*))fcn); }
BGBDY_API void dyTypeNextSlot(char *ty, dyt (*fcn)(dyt, char*))
	{ BGBDY_SetNextSlot(ty, (char *(*)(void*, char*))fcn); }
BGBDY_API void dyTypeTrySetSlot(char *ty, dyt (*fcn)(dyt, char*, dyt))
	{ BGBDY_SetTrySetSlot(ty, (void *(*)(void*,char*,void*))fcn); }

BGBDY_API void dyTypeGetDelegate(char *ty, dyt (*fcn)(dyt, char*))
	{ BGBDY_SetGetDel(ty, (void *(*)(void*,char*))fcn); }
BGBDY_API void dyTypeSetDelegate(char *ty, dyt (*fcn)(dyt, char*, dyt))
	{ BGBDY_SetSetDel(ty, (void *(*)(void*,char*,void*))fcn); }
BGBDY_API void dyTypeNextDelegate(char *ty, dyt (*fcn)(dyt, char*))
	{ BGBDY_SetNextDel(ty, (char *(*)(void*, char*))fcn); }

BGBDY_API void dyTypeCallMethod(char *ty, dyt (*fcn)(dyt, dyt, dyt *, int))
	{ BGBDY_SetCallMethod(ty, (void *(*)(void*,char*,void**,int))fcn); }
BGBDY_API void dyTypeApply(char *ty, dyt (*fcn)(dyt, dyt *, int))
	{ BGBDY_SetApply(ty, (void *(*)(void*,void**,int))fcn); }
BGBDY_API void dyTypeApplyObj(char *ty, dyt (*fcn)(dyt, dyt, dyt *, int))
	{ BGBDY_SetApplyObj(ty, (void *(*)(void*,void*,void**,int))fcn); }

BGBDY_API void dyTypeToString(char *ty, char *(*fcn)(dyt))
	{ BGBDY_SetToString(ty, (char *(*)(void*))fcn); }

BGBDY_API void dyTypeCopyValue(char *ty, dyt (*fcn)(dyt))
	{ BGBDY_SetCopyValue(ty, fcn); }
BGBDY_API void dyTypeDropValue(char *ty, int (*fcn)(dyt))
	{ BGBDY_SetDropValue(ty, fcn); }

BGBDY_API void dyTypeVtabMethod(char *ty, char *name, dyt mth)
	{ BGBDY_SetVtabMethod(ty, name, mth); }

BGBDY_API void dyTypeMethod0(char *ty, char *sym, dyt (*fcn)(dyt self))
	{ dyTypeVtabMethod(ty, sym, dyMethod0(fcn)); }
BGBDY_API void dyTypeMethod1(char *ty, char *sym, dyt (*fcn)(dyt self,dyt))
	{ dyTypeVtabMethod(ty, sym, dyMethod1(fcn)); }
BGBDY_API void dyTypeMethod2(char *ty, char *sym, dyt (*fcn)(dyt self,dyt,dyt))
	{ dyTypeVtabMethod(ty, sym, dyMethod2(fcn)); }
BGBDY_API void dyTypeMethod3(char *ty, char *sym, dyt (*fcn)(dyt self,dyt,dyt,dyt))
	{ dyTypeVtabMethod(ty, sym, dyMethod3(fcn)); }

BGBDY_API void dyTypeMethodN(char *ty, char *sym, dyt (*fcn)(dyt self,dyt rest))
	{ dyTypeVtabMethod(ty, sym, dyMethodN(fcn)); }
BGBDY_API void dyTypeMethod1N(char *ty, char *sym, dyt (*fcn)(dyt self,dyt,dyt rest))
	{ dyTypeVtabMethod(ty, sym, dyMethod1N(fcn)); }
BGBDY_API void dyTypeMethod2N(char *ty, char *sym, dyt (*fcn)(dyt self,dyt,dyt,dyt rest))
	{ dyTypeVtabMethod(ty, sym, dyMethod2N(fcn)); }
BGBDY_API void dyTypeMethod3N(char *ty, char *sym, dyt (*fcn)(dyt self,dyt,dyt,dyt,dyt rest))
	{ dyTypeVtabMethod(ty, sym, dyMethod3N(fcn)); }


BGBDY_API dyt dyObject()
{
	BGBGC_Object *tmp;
	int cnt;

	tmp=gctalloc("_object_t", sizeof(BGBGC_Object));

	cnt=64;
	tmp->cnt=cnt;
	tmp->var=gcalloc(cnt*sizeof(char *));
	tmp->val=gcalloc(cnt*sizeof(dytf));
	tmp->vari=gcalloc(cnt*sizeof(dycSlot));

	tmp->dcnt=0;
	tmp->dvar=gcalloc(16*sizeof(char *));
	tmp->dval=gcalloc(16*sizeof(dyt));
	return((dyt)tmp);
}

BGBDY_API dyt dyObjectParent(dyt parent)
{
	dyt t;
	t=dyObject();
	dySetDelegate(t, "parent", parent);
	return(t);
}

BGBDY_API int dyObjectp(dyt obj)
	{ return(dytypep(obj, "_object_t")); }

BGBDY_API int dyObjectP(dyt obj)
	{ return(dytypep(obj, "_object_t")); }

BGBDY_API dyt dyObject1(char *var0, dyt val0)
{
	dyt tmp;
	tmp=dyObject();
	dyBind(tmp, var0, val0);
	return(tmp);
}

BGBDY_API dyt dyObject2(char *var0, dyt val0, char *var1, dyt val1)
{
	dyt tmp;
	tmp=dyObject();
	dyBind(tmp, var0, val0);
	dyBind(tmp, var1, val1);
	return(tmp);
}

BGBDY_API dyt dyObject3(char *var0, dyt val0,
	char *var1, dyt val1, char *var2, dyt val2)
{
	dyt tmp;
	tmp=dyObject();
	dyBind(tmp, var0, val0);
	dyBind(tmp, var1, val1);
	dyBind(tmp, var2, val2);
	return(tmp);
}

BGBDY_API dyt dyObject4(
	char *var0, dyt val0,	char *var1, dyt val1,
	char *var2, dyt val2,	char *var3, dyt val3)
{
	dyt tmp;
	tmp=dyObject();
	dyBind(tmp, var0, val0);
	dyBind(tmp, var1, val1);
	dyBind(tmp, var2, val2);
	dyBind(tmp, var3, val3);
	return(tmp);
}

dyt dyGetInner(dyt obj, char *str)
{
	return(dytfUnwrap(dyGet_I(obj, str)));
}

dytf dyGet_I(dyt obj, char *str)
{
	BGBGC_Object *tmp;
	int hi;
	char *s;
	int i, j, k;

	if(!obj || !str)
	{
		if(!obj)dySetStatus("badObj");
		if(!str)dySetStatus("badSlot");
		return(DYTF_UNDEFINED);
	}

	tmp=(BGBGC_Object *)obj;

	if(!dyCheckObjectCurrentAccess(
		tmp->access, BGBDY_ACCESS_READ_ANY))
	{
		dySetStatus("accessDenied");
		return(DYTF_UNDEFINED);
	}

	hi=0; s=str;
	while(*s)hi=(hi*251+1)+(*s++);

	i=hi;
	for(j=0; j<16; j++)
	{
		i=(i*251+1)&0x3FFFFFFF; k=(i>>8)%tmp->cnt;
		if(!tmp->var[k])
		{
			dySetStatus("noSlot");
			return(DYTF_UNDEFINED);
		}
		if(!strcmp(tmp->var[k], str))
			return(tmp->val[k]);
	}

	dySetStatus("noSlot");
	return(DYTF_UNDEFINED);
}

void dyBindRehash_I(dyt obj)
{
	BGBGC_Object *tmp;
	dycSlot *varit;
	char **vart;
	dytf *valt;
	int cnt;
	int i;

	tmp=(BGBGC_Object *)obj;

	//hash is full, expand
	gcSet(varit, tmp->vari);
	gcSet(vart, tmp->var);
	gcSet(valt, tmp->val);
	cnt=tmp->cnt;

	tmp->cnt=cnt+(cnt>>1);
	gcSet(tmp->vari, gcalloc(tmp->cnt*sizeof(dycSlot)));
	gcSet(tmp->var, gcalloc(tmp->cnt*sizeof(char *)));
	gcSet(tmp->val, gcalloc(tmp->cnt*sizeof(dytf)));

	for(i=0; i<cnt; i++)
	{
		if(varit[i])
			dyBindInfo_I(obj, varit[i], valt[i]);
		else dyBind_I(obj, vart[i], valt[i]);
	}
}

dytf dyBind_I(dyt obj, char *var, dytf val)
{
	BGBGC_Object *tmp;
	dytf p;
	int hi, cnt;
	char *s;
	int i, j, k;

	if(!obj || !var)
	{
		if(!obj)dySetStatus("badObj");
		if(!var)dySetStatus("badSlot");
		return(DYTF_UNDEFINED);
	}

	tmp=(BGBGC_Object *)obj;

	if(!dyCheckObjectCurrentAccess(
		tmp->access, BGBDY_ACCESS_WRITE_ANY))
	{
		dySetStatus("accessDenied");
		return(DYTF_UNDEFINED);
	}

	if(!tmp->var)
	{
		dySetStatus("noSlot");
		return(DYTF_UNDEFINED);
	}

	hi=0; s=var;
	while(*s)hi=(hi*251+1)+(*s++);

	i=hi;
	for(j=0; j<16; j++)
	{
		i=(i*251+1)&0x3FFFFFFF; k=(i>>8)%tmp->cnt;
		if(!tmp->var[k])
		{
			gcSet(tmp->var[k], dysymbol(var));
			gcSet64(tmp->val[k], val);
			return(DYTF_NULL);
		}
		if(!strcmp(tmp->var[k], var))
		{
			gcSet64(p, tmp->val[k]);
			gcSet64(tmp->val[k], val);
			return(p);
		}
	}

	dyBindRehash_I(obj);

	p=dyBind_I(obj, var, val);
	return(p);
}

dytf dyBindInfo_I(dyt obj, dycSlot var, dytf val)
{
	BGBGC_Object *tmp;
	dycSlot *varit;
	char **vart;
	dytf *valt;
	dytf p;
	int hi, cnt;
	char *s;
	int i, j, k;

	if(!obj || !var)
	{
		if(!obj)dySetStatus("badObj");
		if(!var)dySetStatus("badSlot");
		return(DYTF_UNDEFINED);
	}

	tmp=(BGBGC_Object *)obj;

	if(!dyCheckObjectCurrentAccess(
		tmp->access, BGBDY_ACCESS_WRITE_ANY))
	{
		dySetStatus("accessDenied");
		return(DYTF_UNDEFINED);
	}

	if(!tmp->var)
	{
		dySetStatus("noSlot");
		return(DYTF_UNDEFINED);
	}

	hi=0; s=var->name;
	while(*s)hi=(hi*251+1)+(*s++);

	i=hi;
	for(j=0; j<16; j++)
	{
		i=(i*251+1)&0x3FFFFFFF; k=(i>>8)%tmp->cnt;
		if(!tmp->var[k])
		{
			gcSet(tmp->vari[k], var);
			gcSet(tmp->var[k], dysymbol(var->name));
			gcSet64(tmp->val[k], val);
			return(DYTF_NULL);
		}
		if(!strcmp(tmp->var[k], var->name))
		{
			gcSet64(p, tmp->val[k]);
			gcSet64(tmp->val[k], val);
			return(p);
		}
	}

	dyBindRehash_I(obj);

	p=dyBindInfo_I(obj, var, val);
	return(p);
}

dytf dyTrySet_I(dyt obj, char *var, dytf val)
{
	BGBGC_Object *tmp;
	dytf p;
	int hi, cnt;
	char *s;
	int i, j, k;

	if(!obj || !var)
	{
		if(!obj)dySetStatus("badObj");
		if(!var)dySetStatus("badSlot");
		return(DYTF_UNDEFINED);
	}

	tmp=(BGBGC_Object *)obj;

	if(!dyCheckObjectCurrentAccess(
		tmp->access, BGBDY_ACCESS_WRITE_ANY))
	{
		dySetStatus("accessDenied");
		return(DYTF_UNDEFINED);
	}

	if(!tmp->var)
	{
		dySetStatus("noSlot");
		return(DYTF_UNDEFINED);
	}

	hi=0; s=var;
	while(*s)hi=(hi*251+1)+(*s++);

	i=hi;
	for(j=0; j<16; j++)
	{
		i=(i*251+1)&0x3FFFFFFF; k=(i>>8)%tmp->cnt;
		if(!tmp->var[k])
		{
			dySetStatus("noSlot");
			return(DYTF_UNDEFINED);
		}
		if(!strcmp(tmp->var[k], var))
		{
			p=tmp->val[k];
			gcSet64(tmp->val[k], val);
			return(p);
		}
	}

	dySetStatus("noSlot");
	return(DYTF_UNDEFINED);
}

dyt dyTryGetRef_I(dyt obj, char *var)
{
	BGBGC_Object *tmp;
	dyt p;
	int hi, cnt;
	char *s;
	int i, j, k;

	if(!obj || !var)
	{
		if(!obj)dySetStatus("badObj");
		if(!var)dySetStatus("badSlot");
		return(UNDEFINED);
	}

	tmp=(BGBGC_Object *)obj;

	if(!dyCheckObjectCurrentAccess(
		tmp->access, BGBDY_ACCESS_READWRITE_ANY))
	{
		dySetStatus("accessDenied");
		return(UNDEFINED);
	}

	if(!tmp->var)
	{
		dySetStatus("noSlot");
		return(UNDEFINED);
	}

	hi=0; s=var;
	while(*s)hi=(hi*251+1)+(*s++);

	i=hi;
	for(j=0; j<16; j++)
	{
		i=(i*251+1)&0x3FFFFFFF; k=(i>>8)%tmp->cnt;
		if(!tmp->var[k])
		{
			dySetStatus("noSlot");
			return(UNDEFINED);
		}
		if(!strcmp(tmp->var[k], var))
		{
//			p=tmp->val[k];
//			gcSet(tmp->val[k], val);
			p=dyllTypeboxPtr("Cr", (void *)(&tmp->val[k]));
			return(p);
		}
	}

	dySetStatus("noSlot");
	return(UNDEFINED);
}

dycSlot dyTryGetInfo_I(dyt obj, char *var)
{
	BGBGC_Object *tmp;
	dyt p;
	int hi, cnt;
	char *s;
	int i, j, k;

	if(!obj || !var)
	{
		if(!obj)dySetStatus("badObj");
		if(!var)dySetStatus("badSlot");
		return(UNDEFINED);
	}

	tmp=(BGBGC_Object *)obj;

	if(!dyCheckObjectCurrentAccess(
		tmp->access, BGBDY_ACCESS_READWRITE_ANY))
	{
		dySetStatus("accessDenied");
		return(UNDEFINED);
	}

	if(!tmp->var)
	{
		dySetStatus("noSlot");
		return(UNDEFINED);
	}

	hi=0; s=var;
	while(*s)hi=(hi*251+1)+(*s++);

	i=hi;
	for(j=0; j<16; j++)
	{
		i=(i*251+1)&0x3FFFFFFF; k=(i>>8)%tmp->cnt;
		if(!tmp->var[k])
		{
			dySetStatus("noSlot");
			return(UNDEFINED);
		}
		if(!strcmp(tmp->var[k], var))
		{
			return(tmp->vari[k]);
		}
	}

	dySetStatus("noSlot");
	return(UNDEFINED);
}

BGBDY_API dytf dyBindB(dyt obj, char *var, dytf val)
{
	if(dytypep(obj, "_object_t"))
		{ return(dyBind_I(obj, var, val)); }
	return(dytfWrap(dyBind(obj, var, dytfUnwrap(val))));
}

BGBDY_API dyt dyBind(dyt obj, char *var, dyt val)
{
	char tb[256];
	dycSlot sinf;
	dyt p, o;
	int i;

	if(dytypep(obj, "_object_t"))
		{ return(dytfUnwrap(dyBind_I(obj, var, dytfWrap(val)))); }

	if(dytypep(obj, "_classobj_t"))
	{
		i=dycSetSlot((dycObject)obj, var, val);
		if(i>=0)return(val);
		
		o=val;
		sprintf(tb, "set_%s", var);
		p=dycCallNameArray((dycObject)obj, tb, &o, 1);
		if(p!=UNDEFINED)return(val);

		sinf=dycAddObjSlot((dycObject)obj, var, "r");
		if(sinf)
		{
			i=dycSet((dycObject)obj, sinf, val);
			if(i>=0)return(val);
		}

		return(UNDEFINED);
	}

	if(dytypep(obj, "_classinfo_t"))
	{
		i=dycSetStaticSlot((dycClass)obj, var, val);
		if(i>=0)return(val);

		return(UNDEFINED);
	}

	p=BGBDY_SetSlot(obj, var, val);
	return(p);
}

BGBDY_API dyt dyBindInfo(dyt obj, dycSlot var, dyt val)
{
	char tb[256];
	dycSlot sinf;
	dyt p, o;
	int i;

	if(dytypep(obj, "_object_t"))
		return(dytfUnwrap(dyBindInfo_I(obj, var, dytfWrap(val))));

	if(dytypep(obj, "_classobj_t"))
	{
		i=dycSetSlot((dycObject)obj, var->name, val);
		if(i>=0)return(val);
		
		o=val;
		sprintf(tb, "set_%s", var->name);
		p=dycCallNameArray((dycObject)obj, tb, &o, 1);
		if(p!=UNDEFINED)return(val);

		if(dycObjectDynamicP((dycObject)obj))
		{
			sinf=dycAddObjSlot((dycObject)obj, var->name, var->sig);
			if(sinf)
			{
				i=dycSet((dycObject)obj, sinf, val);
				if(i>=0)return(val);
			}
		}

		return(UNDEFINED);
	}

	if(dytypep(obj, "_classinfo_t"))
	{
		i=dycSetStaticSlot((dycClass)obj, var->name, val);
		if(i>=0)return(val);

		return(UNDEFINED);
	}

	p=BGBDY_SetSlot(obj, var->name, val);
	return(p);
}

BGBDY_API dyt dyTrySet(dyt obj, char *var, dyt val)
{
	char tb[256];
	dyt p, o;
	int i;

	if(dytypep(obj, "_object_t"))
		return(dytfUnwrap(dyTrySet_I(obj, var, dytfWrap(val))));

	if(dytypep(obj, "_classobj_t"))
	{
		i=dycSetSlot((dycObject)obj, var, val);
		if(i>=0)return(NULL);
		
		o=val;
		sprintf(tb, "set_%s", var);
		p=dycCallNameArray((dycObject)obj, tb, &o, 1);
		if(p!=UNDEFINED)return(NULL);
		return(UNDEFINED);
	}

	if(dytypep(obj, "_classinfo_t"))
	{
		i=dycSetStaticSlot((dycClass)obj, var, val);
		if(i>=0)return(val);

		return(UNDEFINED);
	}

	p=BGBDY_TrySetSlot(obj, var, val);
	return(p);
}

dyt dyGetInner2(dyt obj, char *str)
{
	return(dytfUnwrap(dyGet_I2(obj, str)));
}

dytf dyGet_I2(dyt obj, char *sym)
{
	char tb[256];
	dyt arr[64];
	BGBGC_Object *tmp;
	char *s;
	dytf p, q, o;
	int i, n;

#if 1
	tmp=(BGBGC_Object *)obj;
	if(!dyCheckObjectCurrentAccess(
		tmp->access, BGBDY_ACCESS_READ_ANY))
	{
		dySetStatus("accessDenied");
		return(DYTF_UNDEFINED);
	}
#endif

	p=dyGet_I(obj, sym);
//	if(p!=UNDEFINED)
	if(!dytfUndefinedP(p))
		return(p);

#if 1
	sprintf(tb, "get_%s", sym);
	p=dyGet_I(obj, tb);
//	if(p!=UNDEFINED)
	if(!dytfUndefinedP(p))
	{
		q=dytfApplyMethod(dytfPtrF(obj), p, &o, 0);
		return(q);
	}

	p=dyGet_I(obj, "get_any_");
//	if(p!=UNDEFINED)
	if(!dytfUndefinedP(p))
	{
		o=dytfSymbol(sym);
		q=dytfApplyMethod(dytfPtrF(obj), p, &o, 1);
		return(q);
	}
#endif

//	for(i=0; i<dyoo_objstackpos; i++)
//		if(dyoo_objstack[i]==obj)
//			return(DYTF_UNDEFINED);
//	gcSet(dyoo_objstack[dyoo_objstackpos++], obj);

	if(DYO_RecursiveCheck(obj))
		return(DYTF_UNDEFINED);
	DYO_RecursivePush(obj);

//	tmp=(BGBGC_Object *)obj;
	for(i=0; i<tmp->dcnt; i++)
	{
		p=dytfGet(dytfPtrF(tmp->dval[i]), sym);
//		if(p!=UNDEFINED)
		if(!dytfUndefinedP(p))
		{
//			dyoo_objstackpos--;
			DYO_RecursivePop();
			return(p);
		}
	}
//	dyoo_objstackpos--;
	DYO_RecursivePop();

	q=dyGet_I(obj, "_pkgname_");
//	if(q && q!=UNDEFINED)
	if(!dytfNullP(q) && !dytfUndefinedP(q))
	{
		sprintf(tb, "%s/%s", dytfStringvF(q), sym);
		p=dytfWrap(dyllGetDynamic(tb));
//		if(p && p!=UNDEFINED)
		if(!dytfNullP(p) && !dytfUndefinedP(p))
			return(p);
	}

	dySetStatus("noSlot");
	return(DYTF_UNDEFINED);
}

BGBDY_API dyt dyGet(dyt obj, char *sym)
{
	char tb[256];
	dyt arr[64];
	BGBGC_Object *tmp;
	char *s;
	dyt p, q, o;
	int i, n;

	if(!obj)return(UNDEFINED);

	if(dytypep(obj, "_object_t"))
		{ return(dyGetInner2(obj, sym)); }

	if(dytypep(obj, "_classobj_t"))
	{
		p=dycGetSlot((dycObject)obj, sym);
		if(p!=UNDEFINED)return(p);

		p=dycLookupMethodSig((dycObject)obj, sym, "?");
		if(p && (p!=UNDEFINED))return(p);

		sprintf(tb, "get_%s", sym);
		p=dycCallNameN((dycObject)obj, tb, NULL);
		if(p!=UNDEFINED)return(p);

//		p=dycCallNameN((dycObject)obj, "get_", NULL);
//		if(p!=UNDEFINED)return(p);

		o=dysymbol(sym);
		p=dycCallNameArray((dycObject)obj, "get_any_", &o, 1);
		if(p!=UNDEFINED)return(NULL);

#if 1
//		for(i=0; i<dyoo_objstackpos; i++)
//			if(dyoo_objstack[i]==obj)
//				return(UNDEFINED);
//		gcSet(dyoo_objstack[dyoo_objstackpos++], obj);

		if(DYO_RecursiveCheck(obj))
			return(UNDEFINED);
		DYO_RecursivePush(obj);

		n=dycGetObjectDelegateArray((dycObject)obj, arr, 64);

//		tmp=(BGBGC_Object *)obj;
		for(i=0; i<n; i++)
		{
			p=dyGet(arr[i], sym);
			if(p!=UNDEFINED)
			{
//				dyoo_objstackpos--;
				DYO_RecursivePop();
				return(p);
			}
		}
//		dyoo_objstackpos--;
		DYO_RecursivePop();
#endif

		return(UNDEFINED);
	}

	if(dytypep(obj, "_classinfo_t"))
	{
		p=dycGetStaticSlot((dycClass)obj, sym);
		if(p!=UNDEFINED)return(p);

		p=dycLookupStaticMethodSig((dycClass)obj, sym, "?");
		if(p && p!=UNDEFINED)return(p);

		return(UNDEFINED);
	}

	p=BGBDY_GetSlot(obj, sym);
	if(p!=UNDEFINED)return(p);

	p=dyllGetDynamicStructSlotDynamic(obj, sym);
	if(p!=UNDEFINED)return(p);

	s=BGBDY_NextDel(obj, NULL);
	if(s && (s!=UNDEFINED))
	{
//		for(i=0; i<dyoo_objstackpos; i++)
//			if(dyoo_objstack[i]==obj)
//				return(UNDEFINED);
//		gcSet(dyoo_objstack[dyoo_objstackpos++], obj);
		if(DYO_RecursiveCheck(obj))
			return(UNDEFINED);
		DYO_RecursivePush(obj);

		while(s)
		{
			o=BGBDY_GetDel(obj, s);
			if(o && (o!=UNDEFINED))
			{
				p=dyGet(o, sym);
				if(p!=UNDEFINED)
				{
//					dyoo_objstackpos--;
					DYO_RecursivePop();
					return(p);
				}
			}
			s=BGBDY_NextDel(obj, s);
		}
//		dyoo_objstackpos--;
		DYO_RecursivePop();

		dySetStatus("noSlot");
		return(UNDEFINED);
	}

	dySetStatus("noSlot");
	return(UNDEFINED);
}

BGBDY_API int dyGeti(dyt obj, char *sym) { return(dyintv(dyGet(obj, sym))); }
BGBDY_API s64 dyGetl(dyt obj, char *sym) { return(dylongv(dyGet(obj, sym))); }
BGBDY_API double dyGetf(dyt obj, char *sym) { return(dydoublev(dyGet(obj, sym))); }
BGBDY_API double dyGetd(dyt obj, char *sym) { return(dydoublev(dyGet(obj, sym))); }

BGBDY_API void dyGet2fv(dyt obj, char *sym, float *v) { dyvecv2fv(dyGet(obj, sym), v); }
BGBDY_API void dyGet3fv(dyt obj, char *sym, float *v) { dyvecv3fv(dyGet(obj, sym), v); }
BGBDY_API void dyGet4fv(dyt obj, char *sym, float *v) { dyvecv4fv(dyGet(obj, sym), v); }
BGBDY_API void dyGet2dv(dyt obj, char *sym, double *v) { dyvecv2dv(dyGet(obj, sym), v); }
BGBDY_API void dyGet3dv(dyt obj, char *sym, double *v) { dyvecv3dv(dyGet(obj, sym), v); }
BGBDY_API void dyGet4dv(dyt obj, char *sym, double *v) { dyvecv4dv(dyGet(obj, sym), v); }

BGBDY_API char *dyGets(dyt obj, char *sym) { return(dystringv(dyGet(obj, sym))); }

dytf dySetR_I2(dyt obj, char *sym, dytf val)
{
	char tb[256];
	dytf arr[64];
	BGBGC_Object *tmp;
	dytf p, q, o;
	int i, n;

#if 1
	tmp=(BGBGC_Object *)obj;
	if(!dyCheckObjectCurrentAccess(
		tmp->access, BGBDY_ACCESS_WRITE_ANY))
	{
		dySetStatus("accessDenied");
		return(DYTF_UNDEFINED);
	}
#endif

	p=dyTrySet_I(obj, sym, val);
	if(!dytfUndefinedP(p))
	{
		if(dytfDropValue(p))
			return(DYTF_NULL);
		return(p);
	}

#if 1
	sprintf(tb, "set_%s", sym);
	p=dyGet_I(obj, tb);
	if(!dytfUndefinedP(p))
	{
		o=val;
		q=dyApplyMethodB(obj, dytfPtrvF(p), &o, 1);
		return(q);
	}

	p=dyGet_I(obj, "set_any_");
	if(!dytfUndefinedP(p))
	{
		arr[0]=dytfSymbol(sym);
		arr[1]=val;
		q=dyApplyMethodB(obj, dytfPtrvF(p), arr, 2);
		return(q);
	}
#endif

//	for(i=0; i<dyoo_objstackpos; i++)
//		if(dyoo_objstack[i]==obj)
//			return(DYTF_UNDEFINED);
//	gcSet(dyoo_objstack[dyoo_objstackpos++], obj);

	if(DYO_RecursiveCheck(obj))
		return(DYTF_UNDEFINED);
	DYO_RecursivePush(obj);

	tmp=(BGBGC_Object *)obj;
	for(i=0; i<tmp->dcnt; i++)
	{
		p=dySetRB(tmp->dval[i], sym, val);
		if(!dytfUndefinedP(p))
		{
			DYO_RecursivePop();
//			dyoo_objstackpos--;
			return(p);
		}
	}
//	dyoo_objstackpos--;
	DYO_RecursivePop();

	dySetStatus("noSlot");
	return(DYTF_UNDEFINED);
}

dytf dySetRB(dyt obj, char *sym, dytf val)
{
	dytf p;

	if(dytypep(obj, "_object_t"))
	{
		p=dySetR_I2(obj, sym, val);
		return(p);
	}

	p=dytfWrap(dySetR(obj, sym, dytfUnwrap(val)));
	return(p);
}

dyt dySetR(dyt obj, char *sym, dyt val)
{
	char tb[256];
	dyt arr[64];
	BGBGC_Object *tmp;
	char *s;
	dyt p, q, o;
	int i, n;

	if(dytypep(obj, "_object_t"))
	{
		p=dytfUnwrap(dySetR_I2(obj, sym, dytfWrap(val)));
		return(p);
	}

	if(dytypep(obj, "_classobj_t"))
	{
		i=dycSetSlot((dycObject)obj, sym, val);
		if(i>=0)return(NULL);
		
		o=val;
		sprintf(tb, "set_%s", sym);
		p=dycCallNameArray((dycObject)obj, tb, &o, 1);
		if(p!=UNDEFINED)return(NULL);

		arr[0]=dysymbol(sym);
		arr[1]=val;
		p=dycCallNameArray((dycObject)obj, "set_any_", arr, 2);
		if(p!=UNDEFINED)return(NULL);

#if 1
//		for(i=0; i<dyoo_objstackpos; i++)
//			if(dyoo_objstack[i]==obj)
//				return(UNDEFINED);
//		gcSet(dyoo_objstack[dyoo_objstackpos++], obj);

		if(DYO_RecursiveCheck(obj))
			return(UNDEFINED);
		DYO_RecursivePush(obj);

		n=dycGetObjectDelegateArray((dycObject)obj, arr, 64);

//		tmp=(BGBGC_Object *)obj;
		for(i=0; i<n; i++)
		{
			p=dySetR(arr[i], sym, val);
			if(p!=UNDEFINED)
			{
//				dyoo_objstackpos--;
				DYO_RecursivePop();
				return(p);
			}
		}
//		dyoo_objstackpos--;
		DYO_RecursivePop();
#endif

		return(UNDEFINED);
	}

	if(dytypep(obj, "_classinfo_t"))
	{
		i=dycSetStaticSlot((dycClass)obj, sym, val);
		if(i>=0)return(val);

		return(UNDEFINED);
	}

	p=BGBDY_TrySetSlot(obj, sym, val);
	if(p!=UNDEFINED)return(p);

	p=dyllSetDynamicStructSlotDynamic(obj, sym, val);
	if(p!=UNDEFINED)return(p);

	s=BGBDY_NextDel(obj, NULL);
	if(s && (s!=UNDEFINED))
	{
//		for(i=0; i<dyoo_objstackpos; i++)
//			if(dyoo_objstack[i]==obj)
//				return(UNDEFINED);
//		gcSet(dyoo_objstack[dyoo_objstackpos++], obj);

		if(DYO_RecursiveCheck(obj))
			return(UNDEFINED);
		DYO_RecursivePush(obj);

		while(s)
		{
			o=BGBDY_GetDel(obj, s);
			if(o && (o!=UNDEFINED))
			{
				p=dySetR(o, sym, val);
				if(p!=UNDEFINED)
				{
//					dyoo_objstackpos--;
					DYO_RecursivePop();
					return(p);
				}
			}
			s=BGBDY_NextDel(obj, s);
		}
//		dyoo_objstackpos--;
		DYO_RecursivePop();

		dySetStatus("noSlot");
		return(UNDEFINED);
	}

	dySetStatus("noSlot");
	return(UNDEFINED);
}

BGBDY_API dyt dySet(dyt obj, char *sym, dyt val)
{
	dyt p;

	if(!obj)return(UNDEFINED);

	p=dySetR(obj, sym, val);
	if(p!=UNDEFINED)return(p);

	p=dyBind(obj, sym, val);
	return(p);
}

BGBDY_API dytf dySetB(dyt obj, char *sym, dytf val)
{
	dytf p;

	if(!obj)return(DYTF_UNDEFINED);

	p=dySetRB(obj, sym, val);
	if(!dytfUndefinedP(p))
		return(p);

	p=dyBindB(obj, sym, val);
	return(p);
}

BGBDY_API void dySeti(dyt obj, char *sym, int val) { dySet(obj, sym, dyint(val)); }
BGBDY_API void dySetl(dyt obj, char *sym, s64 val) { dySet(obj, sym, dylong(val)); }
BGBDY_API void dySetf(dyt obj, char *sym, double val) { dySet(obj, sym, dyfloat(val)); }
BGBDY_API void dySetd(dyt obj, char *sym, double val) { dySet(obj, sym, dydouble(val)); }

BGBDY_API void dySet2f(dyt obj, char *sym, double x, double y)
	{ dySet(obj, sym, dyvec2(x, y)); }
BGBDY_API void dySet3f(dyt obj, char *sym, double x, double y, double z)
	{ dySet(obj, sym, dyvec3(x, y, z)); }
BGBDY_API void dySet4f(dyt obj, char *sym, double x, double y, double z, double w)
	{ dySet(obj, sym, dyvec4(x, y, z, w)); }

BGBDY_API void dySet2fv(dyt obj, char *sym, float *v) { dySet(obj, sym, dyvec2fv(v)); }
BGBDY_API void dySet3fv(dyt obj, char *sym, float *v) { dySet(obj, sym, dyvec3fv(v)); }
BGBDY_API void dySet4fv(dyt obj, char *sym, float *v) { dySet(obj, sym, dyvec4fv(v)); }
BGBDY_API void dySet2dv(dyt obj, char *sym, double *v) { dySet(obj, sym, dyvec2dv(v)); }
BGBDY_API void dySet3dv(dyt obj, char *sym, double *v) { dySet(obj, sym, dyvec3dv(v)); }
BGBDY_API void dySet4dv(dyt obj, char *sym, double *v) { dySet(obj, sym, dyvec4dv(v)); }

BGBDY_API void dySets(dyt obj, char *sym, char *val) { dySet(obj, sym, dystring(val)); }


BGBDY_API dyt dyTryGetRef(dyt obj, char *sym)
{
	char tb[256];
	BGBGC_Object *tmp;
	char *s;
	dyt p, q, o;
	int i;

	if(!obj)return(UNDEFINED);

	if(dytypep(obj, "_object_t"))
	{
#if 1
		tmp=(BGBGC_Object *)obj;
		if(!dyCheckObjectCurrentAccess(
			tmp->access, BGBDY_ACCESS_READWRITE_ANY))
		{
			dySetStatus("accessDenied");
			return(UNDEFINED);
		}
#endif

		p=dyTryGetRef_I(obj, sym);
		if(p!=UNDEFINED)return(p);

//		for(i=0; i<dyoo_objstackpos; i++)
//			if(dyoo_objstack[i]==obj)
//				return(UNDEFINED);
//		gcSet(dyoo_objstack[dyoo_objstackpos++], obj);

		if(DYO_RecursiveCheck(obj))
			return(UNDEFINED);
		DYO_RecursivePush(obj);

		tmp=(BGBGC_Object *)obj;
		for(i=0; i<tmp->dcnt; i++)
		{
//			p=dyTryGetRef_I(tmp->dval[i], sym);
			p=dyTryGetRef(tmp->dval[i], sym);
			if(p!=UNDEFINED)
			{
//				dyoo_objstackpos--;
				DYO_RecursivePop();
				return(p);
			}
		}
//		dyoo_objstackpos--;
		DYO_RecursivePop();

#if 0
		q=dyGetInner(obj, "_pkgname_");
		if(q && q!=UNDEFINED)
		{
			sprintf(tb, "%s/%s", dystringv(q), sym);
			p=dyllGetDynamic(tb);
			if(p && p!=UNDEFINED)
				return(p);
		}
#endif

		dySetStatus("noSlot");
		return(UNDEFINED);
	}

#if 0
	if(dytypep(obj, "_classobj_t"))
	{
		p=dycGetSlot((dycObject)obj, sym);
		if(p!=UNDEFINED)return(p);
		sprintf(tb, "get_%s", sym);
		p=dycCallNameN((dycObject)obj, tb, NULL);
		if(p!=UNDEFINED)return(p);
		return(UNDEFINED);
	}

	p=BGBDY_GetSlot(obj, sym);
	if(p!=UNDEFINED)return(p);

	s=BGBDY_NextDel(obj, NULL);
	if(s && (s!=UNDEFINED))
	{
//		for(i=0; i<dyoo_objstackpos; i++)
//			if(dyoo_objstack[i]==obj)
//				return(UNDEFINED);
//		gcSet(dyoo_objstack[dyoo_objstackpos++], obj);

		if(DYO_RecursiveCheck(obj))
			return(UNDEFINED);
		DYO_RecursivePush(obj);

		while(s)
		{
			o=BGBDY_GetDel(obj, s);
			if(o && (o!=UNDEFINED))
			{
				p=dyGet(o, sym);
				if(p!=UNDEFINED)
				{
//					dyoo_objstackpos--;
					DYO_RecursivePop();
					return(p);
				}
			}
			s=BGBDY_NextDel(obj, s);
		}
//		dyoo_objstackpos--;
		DYO_RecursivePop();

		dySetStatus("noSlot");
		return(UNDEFINED);
	}
#endif

	dySetStatus("noSlot");
	return(UNDEFINED);
}

BGBDY_API dycSlot dyTryGetInfo(dyt obj, char *sym)
{
	char tb[256];
	BGBGC_Object *tmp;
	dycSlot inf;
	char *s;
	dyt p, q, o;
	int i;

	if(!obj)return(UNDEFINED);

	if(dytypep(obj, "_object_t"))
	{
#if 1
		tmp=(BGBGC_Object *)obj;
		if(!dyCheckObjectCurrentAccess(
			tmp->access, BGBDY_ACCESS_READWRITE_ANY))
		{
			dySetStatus("accessDenied");
			return(UNDEFINED);
		}
#endif

		inf=dyTryGetInfo_I(obj, sym);
		if(inf!=UNDEFINED)return(inf);

//		for(i=0; i<dyoo_objstackpos; i++)
//			if(dyoo_objstack[i]==obj)
//				return(UNDEFINED);
//		gcSet(dyoo_objstack[dyoo_objstackpos++], obj);

		if(DYO_RecursiveCheck(obj))
			return(UNDEFINED);
		DYO_RecursivePush(obj);

		tmp=(BGBGC_Object *)obj;
		for(i=0; i<tmp->dcnt; i++)
		{
//			p=dyTryGetRef_I(tmp->dval[i], sym);
			inf=dyTryGetInfo(tmp->dval[i], sym);
			if(inf!=UNDEFINED)
			{
//				dyoo_objstackpos--;
				DYO_RecursivePop();
				return(inf);
			}
		}
//		dyoo_objstackpos--;
		DYO_RecursivePop();

		dySetStatus("noSlot");
		return(UNDEFINED);
	}

#if 0
	if(dytypep(obj, "_classobj_t"))
	{
		p=dycGetSlot((dycObject)obj, sym);
		if(p!=UNDEFINED)return(p);
		sprintf(tb, "get_%s", sym);
		p=dycCallNameN((dycObject)obj, tb, NULL);
		if(p!=UNDEFINED)return(p);
		return(UNDEFINED);
	}

	p=BGBDY_GetSlot(obj, sym);
	if(p!=UNDEFINED)return(p);

	s=BGBDY_NextDel(obj, NULL);
	if(s && (s!=UNDEFINED))
	{
//		for(i=0; i<dyoo_objstackpos; i++)
//			if(dyoo_objstack[i]==obj)
//				return(UNDEFINED);
//		gcSet(dyoo_objstack[dyoo_objstackpos++], obj);
		if(DYO_RecursiveCheck(obj))
			return(UNDEFINED);
		DYO_RecursivePush(obj);

		while(s)
		{
			o=BGBDY_GetDel(obj, s);
			if(o && (o!=UNDEFINED))
			{
				p=dyGet(o, sym);
				if(p!=UNDEFINED)
				{
//					dyoo_objstackpos--;
					DYO_RecursivePop();
					return(p);
				}
			}
			s=BGBDY_NextDel(obj, s);
		}
//		dyoo_objstackpos--;
		DYO_RecursivePop();

		dySetStatus("noSlot");
		return(UNDEFINED);
	}
#endif

	dySetStatus("noSlot");
	return(UNDEFINED);
}


BGBDY_API dyt dyGetIdx(dyt obj, int idx)
{
	char *s;
	dyt t, c;
	int i;

	if(dyarrayp(obj))
		return(dyarrayidx(obj, idx));

	if(dytypep(obj, "_sparse_t"))
		return(BGBGC_SparseGetIdx(obj, idx));

	if(dycArrayP(obj))
		return(dyarrayidx(obj, idx));

	if(dystringp(obj))
	{
		s=(char *)obj; i=idx;
//		t=dychar(((char *)obj)[i]);

		if(i<0)return(NULL);
		while(*s && i)
			{ s=BGBDY_NextChar(s); i--; }
		if(!(*s) && (i>0))
			return(NULL);
		t=dychar(BGBDY_PeekChar(&s));
		return(t);
	}

	if(dycArrayRefP(obj))
	{
		c=(dyt)dycArrayRefArray(obj);
		i=dycArrayRefIndex(obj)+idx;
		t=dycGetArrayVar((dycArray)c, i);
		return(t);
	}

	if(dyconsp(obj))
	{
		c=obj; i=idx;
		while(i && dyconsp(dyCdrF(c))) { i--; c=dyCdrF(c); }
		while(i) { i--; dySetCdrF(c, dylist1(NULL)); c=dyCdrF(c); }

		if(dyconsp(c))t=dycar(c);
			else t=UNDEFINED;
		return(t);
	}

	if(dyllTypeboxPtrP(obj))
		{ return(dyllTypeboxPtrGetIndex(obj, idx)); }

	dySetStatus("badObjType");
	return(UNDEFINED);
}

BGBDY_API dyt dySetIdx(dyt obj, int idx, dyt val)
{
	dyt c;
	int i;

	if(dyarrayp(obj))
	{
		dyarraysetidx(obj, idx, val);
		return(NULL);
	}

	if(dytypep(obj, "_sparse_t"))
	{
		BGBGC_SparseSetIdx(obj, idx, val);
		return(NULL);
	}

	if(dycArrayP(obj))
	{
		dyarraysetidx(obj, idx, val);
		return(NULL);
	}

	if(dycArrayRefP(obj))
	{
		c=(dyt)dycArrayRefArray(obj);
		i=dycArrayRefIndex(obj)+idx;
		dycSetArrayVar((dycArray)c, i, val);
		return(NULL);
	}

	if(dyconsp(obj))
	{
		c=obj; i=idx;
		while(i && dyconsp(dyCdrF(c))) { i--; c=dyCdrF(c); }
		while(i) { i--; dySetCdrF(c, dylist1(NULL)); c=dyCdrF(c); }

		if(dyconsp(c))dysetcar(c, val);
		return(NULL);
	}

	if(dyllTypeboxPtrP(obj))
		{ return(dyllTypeboxPtrSetIndex(obj, idx, val)); }

	dySetStatus("badObjType");
	return(UNDEFINED);
}

BGBDY_API dyt dyGetIdxRef(dyt obj, int idx)
{
	char *s;
	dyt t;
	int i;

	if(dyarrayp(obj))
		return(dyarrayidxref(obj, idx));

	if(dycArrayP(obj))
		return(dyarrayidxref(obj, idx));

	if(dytypep(obj, "_sparse_t"))
		return(BGBGC_SparseGetIdx(obj, idx));

	if(dystringp(obj))
	{
		s=(char *)obj; i=idx;
//		t=dychar(((char *)obj)[i]);

		if(i<0)return(NULL);
		while(*s && i)
			{ s=BGBDY_NextChar(s); i--; }
		if(!(*s) && (i>0))
			return(NULL);
		return((dyt)s);
//		t=dychar(BGBDY_PeekChar(&s));
//		return(t);
	}

	if(dyllTypeboxPtrP(obj))
		{ return(dyllTypeboxPtrGetIndex(obj, idx)); }

	dySetStatus("badObjType");
	return(UNDEFINED);
}

BGBDY_API dyt dyGetKey(dyt obj, dyt key)
{
	char *s;

	s=dygettype(key);
	if(!s || !strcmp(s, "_symbol_t") ||
		!strcmp(s, "_string_t") || !strcmp(s, "_keyword_t"))
	{
		return(dyGet(obj, (char *)key));
	}

	if(dyintp(key))
	{
		return(dyGetIdx(obj, dyintv(key)));
	}

	dySetStatus("badKeyType");
	return(UNDEFINED);
}

BGBDY_API dyt dySetKey(dyt obj, dyt key, dyt val)
{
	char *s;

	s=dygettype(key);
	if(!s || !strcmp(s, "_symbol_t") ||
		!strcmp(s, "_string_t") || !strcmp(s, "_keyword_t"))
	{
		return(dySet(obj, (char *)key, val));
	}

	if(dyintp(key))
	{
		return(dySetIdx(obj, dyintv(key), val));
	}

	dySetStatus("badKeyType");
	return(UNDEFINED);
}

BGBDY_API dyt dyGetKeyRef(dyt obj, dyt key)
{
	char *s;

	s=dygettype(key);
	if(!s || !strcmp(s, "_symbol_t") ||
		!strcmp(s, "_string_t") || !strcmp(s, "_keyword_t"))
	{
		return(dyTryGetRef(obj, (char *)key));
	}

	if(dyintp(key))
	{
		return(dyGetIdxRef(obj, dyintv(key)));
	}

	dySetStatus("badKeyType");
	return(UNDEFINED);
}


BGBDY_API dyt dyGetDelegate(dyt obj, char *sym)
{
	BGBGC_Object *tmp;
	dyt p;
	int i;

	if(!obj || !sym)
	{
		if(!obj)dySetStatus("badObj");
		if(!sym)dySetStatus("badSlot");
		return(UNDEFINED);
	}

	if(dytypep(obj, "_object_t"))
	{
		tmp=(BGBGC_Object *)obj;

		if(!dyCheckObjectCurrentAccess(
			tmp->access, BGBDY_ACCESS_READ_ANY))
		{
			dySetStatus("accessDenied");
			return(UNDEFINED);
		}

		for(i=0; i<tmp->dcnt; i++)
			if(!strcmp(tmp->dvar[i], sym))
				return(tmp->dval[i]);
		dySetStatus("noSlot");
		return(UNDEFINED);
	}

	p=BGBDY_GetDel(obj, sym);
	if(p!=UNDEFINED)return(p);

	return(UNDEFINED);
}

BGBDY_API void dySetDelegate(dyt obj, char *sym, dyt val)
{
	BGBGC_Object *tmp;
	dyt p;
	int i;

	if(!obj || !sym)
	{
		if(!obj)dySetStatus("badObj");
		if(!sym)dySetStatus("badSlot");
		return;
	}

	if(dytypep(obj, "_object_t"))
	{
		tmp=(BGBGC_Object *)obj;

		if(!dyCheckObjectCurrentAccess(
			tmp->access, BGBDY_ACCESS_WRITE_ANY))
		{
			dySetStatus("accessDenied");
			return;
		}

		for(i=0; i<tmp->dcnt; i++)
			if(!strcmp(tmp->dvar[i], sym))
				{ gcSet(tmp->dval[i], val); return; }
		i=tmp->dcnt++;
		tmp->dvar[i]=dystrsym(sym);
		tmp->dval[i]=val;
		return;
	}

	p=BGBDY_SetDel(obj, sym, val);
	if(p!=UNDEFINED)return;

	return;
}

BGBDY_API dyt dyGetParent(dyt obj)
	{ return(dyGetDelegate(obj, "parent")); }
BGBDY_API void dySetParent(dyt obj, dyt val)
	{ dySetDelegate(obj, "parent", val); }


BGBDY_API void dyBindMethod(dyt obj, char *sym, dyt mth)
{
	dyBind(obj, sym, mth);
}

BGBDY_API void dyDefMethod(dyt obj, char *sym,
	dyt (*fcn)(dyt self, dyt *args, int nargs))
		{ dyBindMethod(obj, sym, dyMethod(fcn)); }

BGBDY_API void dyDefMethod0(dyt obj, char *sym, dyt (*fcn)(dyt self))
	{ dyBindMethod(obj, sym, dyMethod0(fcn)); }
BGBDY_API void dyDefMethod1(dyt obj, char *sym, dyt (*fcn)(dyt self,dyt))
	{ dyBindMethod(obj, sym, dyMethod1(fcn)); }
BGBDY_API void dyDefMethod2(dyt obj, char *sym, dyt (*fcn)(dyt self,dyt,dyt))
	{ dyBindMethod(obj, sym, dyMethod2(fcn)); }
BGBDY_API void dyDefMethod3(dyt obj, char *sym, dyt (*fcn)(dyt self,dyt,dyt,dyt))
	{ dyBindMethod(obj, sym, dyMethod3(fcn)); }
BGBDY_API void dyDefMethod4(dyt obj, char *sym, dyt (*fcn)(dyt self,dyt,dyt,dyt,dyt))
	{ dyBindMethod(obj, sym, dyMethod4(fcn)); }
BGBDY_API void dyDefMethod5(dyt obj, char *sym, dyt (*fcn)(dyt self,dyt,dyt,dyt,dyt,dyt))
	{ dyBindMethod(obj, sym, dyMethod5(fcn)); }
BGBDY_API void dyDefMethod6(dyt obj, char *sym, dyt (*fcn)(dyt self,dyt,dyt,dyt,dyt,dyt,dyt))
	{ dyBindMethod(obj, sym, dyMethod6(fcn)); }
BGBDY_API void dyDefMethod7(dyt obj, char *sym, dyt (*fcn)(dyt self,dyt,dyt,dyt,dyt,dyt,dyt,dyt))
	{ dyBindMethod(obj, sym, dyMethod7(fcn)); }
BGBDY_API void dyDefMethod8(dyt obj, char *sym, dyt (*fcn)(dyt self,dyt,dyt,dyt,dyt,dyt,dyt,dyt,dyt))
	{ dyBindMethod(obj, sym, dyMethod8(fcn)); }

BGBDY_API void dyDefMethodN(dyt obj, char *sym, dyt (*fcn)(dyt self,dyt rest))
	{ dyBindMethod(obj, sym, dyMethodN(fcn)); }
BGBDY_API void dyDefMethod1N(dyt obj, char *sym, dyt (*fcn)(dyt self,dyt,dyt rest))
	{ dyBindMethod(obj, sym, dyMethod1N(fcn)); }
BGBDY_API void dyDefMethod2N(dyt obj, char *sym, dyt (*fcn)(dyt self,dyt,dyt,dyt rest))
	{ dyBindMethod(obj, sym, dyMethod2N(fcn)); }
BGBDY_API void dyDefMethod3N(dyt obj, char *sym, dyt (*fcn)(dyt self,dyt,dyt,dyt,dyt rest))
	{ dyBindMethod(obj, sym, dyMethod3N(fcn)); }
BGBDY_API void dyDefMethod4N(dyt obj, char *sym, dyt (*fcn)(dyt self,dyt,dyt,dyt,dyt,dyt rest))
	{ dyBindMethod(obj, sym, dyMethod4N(fcn)); }
BGBDY_API void dyDefMethod5N(dyt obj, char *sym, dyt (*fcn)(dyt self,dyt,dyt,dyt,dyt,dyt,dyt rest))
	{ dyBindMethod(obj, sym, dyMethod5N(fcn)); }
BGBDY_API void dyDefMethod6N(dyt obj, char *sym, dyt (*fcn)(dyt self,dyt,dyt,dyt,dyt,dyt,dyt,dyt rest))
	{ dyBindMethod(obj, sym, dyMethod6N(fcn)); }
BGBDY_API void dyDefMethod7N(dyt obj, char *sym, dyt (*fcn)(dyt self,dyt,dyt,dyt,dyt,dyt,dyt,dyt,dyt rest))
	{ dyBindMethod(obj, sym, dyMethod7N(fcn)); }
BGBDY_API void dyDefMethod8N(dyt obj, char *sym, dyt (*fcn)(dyt self,dyt,dyt,dyt,dyt,dyt,dyt,dyt,dyt,dyt rest))
	{ dyBindMethod(obj, sym, dyMethod8N(fcn)); }


dyt dyCall_I(dyt obj, char *sym, dyt *args, int nargs)
{
	dyt p, mth;
	int i;

	if(!obj)return(UNDEFINED);

	if(dytypep(obj, "_object_t"))
	{
		mth=dyGet(obj, sym);
		if(mth && (mth!=UNDEFINED))
			return(dyApplyMethod(obj, mth, args, nargs));

		mth=dyGet(obj, "doesNotUnderstand");
		if(mth && (mth!=UNDEFINED))
		{
			for(i=nargs; i>0; i++)
				args[i]=args[i-1];
			args[0]=dysymbol(sym);

			return(dyApplyMethod(obj, mth, args, nargs+1));
		}
		return(UNDEFINED);
	}

	if(dytypep(obj, "_classobj_t"))
	{
		p=dycCallNameArray((dycObject)obj, sym, args, nargs);
		return(p);
	}

	if(dytypep(obj, "_classinfo_t"))
	{
		p=dycCallStaticNameArray((dycClass)obj, sym, args, nargs);
		return(p);
	}

	p=BGBDY_CallMethod(obj, sym, args, nargs);
	if(p!=UNDEFINED)return(p);

	mth=BGBDY_GetVtabMethod(obj, sym);
	if(mth && (mth!=UNDEFINED))
		return(dyApplyMethod(obj, mth, args, nargs));

	p=dycCallAliasNameArray(obj, sym, args, nargs);
	if(p!=UNDEFINED)return(p);

	mth=dyGet(obj, sym);
	if(mth && (mth!=UNDEFINED))
		return(dyApplyMethod(obj, mth, args, nargs));

	return(UNDEFINED);
}

BGBDY_API dyt dyCall(dyt obj, char *sym, dyt *args, int nargs)
{
	dyt ar[16];
	int i;

	for(i=0; i<nargs; i++)
		ar[i]=args[i];
	return(dyCall_I(obj, sym, ar, nargs));
}

BGBDY_API dyt dyCall0(dyt obj, char *sym)
{
	dyt ar[8];
	return(dyCall_I(obj, sym, ar, 0));
}

BGBDY_API dyt dyCall1(dyt obj, char *sym, dyt a)
{
	dyt ar[8]; ar[0]=a;
	return(dyCall_I(obj, sym, ar, 1));
}

BGBDY_API dyt dyCall2(dyt obj, char *sym, dyt a, dyt b)
{
	dyt ar[8]; ar[0]=a; ar[1]=b;
	return(dyCall_I(obj, sym, ar, 2));
}

BGBDY_API dyt dyCall3(dyt obj, char *sym, dyt a, dyt b, dyt c)
{
	dyt ar[8]; ar[0]=a; ar[1]=b; ar[2]=c;
	return(dyCall_I(obj, sym, ar, 3));
}

BGBDY_API dyt dyCall4(dyt obj, char *sym, dyt a, dyt b, dyt c, dyt d)
{
	dyt ar[8]; ar[0]=a; ar[1]=b; ar[2]=c; ar[3]=d;
	return(dyCall_I(obj, sym, ar, 4));
}

BGBDY_API dyt dyCall5(dyt obj, char *sym, dyt a, dyt b, dyt c, dyt d, dyt e)
{
	dyt ar[8]; ar[0]=a; ar[1]=b; ar[2]=c; ar[3]=d; ar[4]=e;
	return(dyCall_I(obj, sym, ar, 5));
}

BGBDY_API dyt dyCall6(dyt obj, char *sym, dyt a, dyt b, dyt c, dyt d, dyt e, dyt f)
{
	dyt ar[8]; ar[0]=a; ar[1]=b; ar[2]=c; ar[3]=d; ar[4]=e; ar[5]=f;
	return(dyCall_I(obj, sym, ar, 6));
}

BGBDY_API dyt dyCall7(dyt obj, char *sym,
	dyt a, dyt b, dyt c, dyt d, dyt e, dyt f, dyt g)
{
	dyt ar[10];
	ar[0]=a; ar[1]=b; ar[2]=c; ar[3]=d; ar[4]=e; ar[5]=f; ar[6]=g;
	return(dyCall_I(obj, sym, ar, 7));
}

BGBDY_API dyt dyCall8(dyt obj, char *sym,
	dyt a, dyt b, dyt c, dyt d, dyt e, dyt f, dyt g, dyt h)
{
	dyt ar[10];
	ar[0]=a; ar[1]=b; ar[2]=c; ar[3]=d; ar[4]=e; ar[5]=f; ar[6]=g; ar[7]=h;
	return(dyCall_I(obj, sym, ar, 7));
}

BGBDY_API dyt dyCallN(dyt obj, char *sym, dyt args)
{
	dyt ar[16];
	dyt *par;
	dyt c;
	int i, j, k;

//	if(dytypep(obj, "_classobj_t"))
//	{
//		c=dycCallNameN((dycObject)obj, sym, args);
//		return(c);
//	}

	if(dyarrayp(args))
	{
		par=dyarrayv(args);
		i=dyarraysz(args);
		return(dyCall(obj, sym, par, i));
	}

	if(dyconsp(args) || (args==NULL))
	{
		i=0; c=args;
		while(dyconsp(c))
		{
			ar[i++]=dycar(c);
			c=dycdr(c);
		}

		if(c && dyarrayp(c))
		{
			par=dyarrayv(c);
			k=dyarraysz(c);
			for(j=0; j<k; j++)
				ar[i++]=par[j];
		}

		return(dyCall_I(obj, sym, ar, i));
	}

	return(UNDEFINED);
}

BGBDY_API dyt dyCall1N(dyt obj, char *sym, dyt a, dyt args)
	{ return(dyCallN(obj, sym, dycons(a, args))); }
BGBDY_API dyt dyCall2N(dyt obj, char *sym, dyt a, dyt b, dyt args)
	{ return(dyCallN(obj, sym, dycons(a, dycons(b, args)))); }
BGBDY_API dyt dyCall3N(dyt obj, char *sym, dyt a, dyt b, dyt c, dyt args)
	{ return(dyCallN(obj, sym, dycons(a, dycons(b, dycons(c, args))))); }
BGBDY_API dyt dyCall4N(dyt obj, char *sym, dyt a, dyt b, dyt c, dyt d, dyt args)
	{ return(dyCallN(obj, sym, dycons(a, dycons(b, dycons(c, dycons(d, args)))))); }
BGBDY_API dyt dyCall5N(dyt obj, char *sym, dyt a, dyt b, dyt c, dyt d, dyt e, dyt args)
	{ return(dyCallN(obj, sym, dycons(a, dycons(b, dycons(c, dycons(d,
		dycons(e, args))))))); }
BGBDY_API dyt dyCall6N(dyt obj, char *sym, dyt a, dyt b, dyt c, dyt d, dyt e, dyt f,
		dyt args)
	{ return(dyCallN(obj, sym, dycons(a, dycons(b, dycons(c, dycons(d,
		dycons(e, dycons(f, args)))))))); }
BGBDY_API dyt dyCall7N(dyt obj, char *sym, dyt a, dyt b, dyt c, dyt d, dyt e, dyt f,
		dyt g, dyt args)
	{ return(dyCallN(obj, sym, dycons(a, dycons(b, dycons(c, dycons(d,
		dycons(e, dycons(f, dycons(g, args))))))))); }
BGBDY_API dyt dyCall8N(dyt obj, char *sym, dyt a, dyt b, dyt c, dyt d, dyt e, dyt f,
		dyt g, dyt h, dyt args)
	{ return(dyCallN(obj, sym, dycons(a, dycons(b, dycons(c, dycons(d,
		dycons(e, dycons(f, dycons(g, dycons(h, args)))))))))); }

BGBDY_API dyt dyCallSig(dyt obj, char *sym, char *sig, ...)
{
	dyt arr[32];
	BGBGC_RawFunc *rinf;
	void *pv;
	dyt p;
	va_list lst;
	int i;

	if(dytypep(obj, "_classobj_t"))
	{
		va_start(lst, sig);
		p=dycCallSigV((dycObject)obj, sym, sig, lst);
		va_end(lst);
		return(p);
	}

	if(dytypep(obj, "_classinfo_t"))
	{
		va_start(lst, sig);
		p=dycCallStaticSigV((dycClass)obj, sym, sig, lst);
		va_end(lst);
		return(p);
	}

	if(dytypep(obj, "_object_t"))
	{
		p=dyGet(obj, sym);

//		if(dytypep(p, "_raw_func_t") ||
//			dytypep(p, "_raw_close_t") ||
//			dytypep(p, "_raw_method_t"))
		if(dytypep(p, "_raw_func_t"))
		{
			va_start(lst, sig);
//			p=dyApplyRawFuncSig(obj, p, sig, lst);

			rinf=(BGBGC_RawFunc *)p;
			pv=dyllApplyRawFuncV(rinf, obj, sig, lst);
			p=dyllExtractVariantBufferSig(rinf->esig, pv);
			va_end(lst);
			return(p);
		}
	}


	va_start(lst, sig);
	i=dyllUnpackArgsSigArray(sig, lst, arr);
	va_end(lst);

	return(dyCall_I(obj, sym, arr, i));
}

BGBDY_API dyt dyLookupMethodSig(dyt obj, char *name, char *sig)
{
	dyt p;

	if(dytypep(obj, "_classobj_t"))
		return(dycLookupMethodSig((dycObject)obj, name, sig));

	p=dyGet(obj, name);
	return(p);
}

BGBDY_API int dyDelete(dyt obj)
{
	BGBGC_Object *tmp;

	if(dytypep(obj, "_classobj_t"))
	{
		dycDeleteObject((dycObject)obj);
		return(0);
	}

	if(dytypep(obj, "_object_t"))
	{
		tmp=(BGBGC_Object *)obj;
		if(tmp->var)gcfree(tmp->var);
		if(tmp->val)gcfree(tmp->val);
		if(tmp->dvar)gcfree(tmp->dvar);
		if(tmp->dval)gcfree(tmp->dval);
		gcfree(tmp);
		return(0);
	}

	return(-1);
}

BGBDY_API dyt dyNewN(char *name, dyt args)
{
	dycObject obj;
	
	dyllGetStructClassInfo(name);
	obj=dycNewInitClassN(name, args);
	if(obj)return((dyt)obj);
	
	return(NULL);
}

BGBDY_API dyt dyNewA(char *name, dyt *args, int nargs)
{
	dycObject obj;
	
	dyllGetStructClassInfo(name);
	obj=dycNewInitClassArray(name, args, nargs);
	if(obj)return((dyt)obj);
	
	return(NULL);
}

BGBDY_API dyt dyClone(dyt obj)
{
	BGBGC_Object *lobj;
	BGBGC_Object *tmp;
	int i, cnt;

	if(dyObjectP(obj))
	{
#if 1
		tmp=(BGBGC_Object *)obj;
		if(!dyCheckObjectCurrentAccess(
			tmp->access, BGBDY_ACCESS_READ_ANY))
		{
			dySetStatus("accessDenied");
			return(UNDEFINED);
		}
#endif

		lobj=(BGBGC_Object *)obj;
		tmp=gctalloc("_object_t", sizeof(BGBGC_Object));

		tmp->cnt=lobj->cnt;
		tmp->var=gcalloc(tmp->cnt*sizeof(char *));
		tmp->val=gcalloc(tmp->cnt*sizeof(dyt));

		tmp->dcnt=lobj->dcnt;
		tmp->dvar=gcalloc(tmp->dcnt*sizeof(char *));
		tmp->dval=gcalloc(tmp->dcnt*sizeof(dyt));

		for(i=0; i<tmp->cnt; i++)
		{
			tmp->var[i]=lobj->var[i];
			tmp->val[i]=lobj->val[i];
		}

		for(i=0; i<tmp->dcnt; i++)
		{
			tmp->dvar[i]=lobj->dvar[i];
			tmp->dval[i]=lobj->dval[i];
		}

		return((dyt)tmp);
	}

	if(dycObjectP(obj))
	{
		return((dyt)dycCloneObject((dycObject)obj));
	}
	
	return(NULL);
}
