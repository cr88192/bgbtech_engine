#include <lbxgl2.h>
#include <bgbscr1.h>

bs1_elem lbxgl_bs1if_observer[4096];

//bs1_elem BS1_MM_AllocObjNameAPtr(char *ty, int sz);


int LBXGL_BS1IF_IndexObserver(bs1_elem o)	//AH:ignore
{
	int i;

	for(i=0; i<4096; i++)
		if(lbxgl_bs1if_observer[i]==o)
			return(i);

	for(i=0; i<4096; i++)
		if(lbxgl_bs1if_observer[i]==BS1_MM_NULL)
	{
		lbxgl_bs1if_observer[i]=o;
		return(i);
	}

	return(-1);
}

bs1_elem LBXGL_BS1IF_Value2Elem(void *v)	//AH:ignore
{
	double *da, *db;
	float *fa,* fb;
	float f;
	bs1_elem t;
	void *p;
	char *ty, *s;
	int i, j, k, l;

	ty=ObjType_GetTypeName(v);
	t=BS1_MM_NULL;

	if(!strcmp(ty, "int_t"))	t=BS1_FIXNUM(NetVal_UnwrapInt(v));
	if(!strcmp(ty, "float_t"))
	{
		f=NetVal_UnwrapFloat(v);
		i=f;
		if(fabs(f-i)<0.0001)t=BS1_FIXNUM(i);
			else t=BS1_FLONUM(f);
	}

	if(!strcmp(ty, "bool_t"))	t=BS1_BOOL(NetVal_UnwrapBool(v));
	if(!strcmp(ty, "string_t"))	t=BS1_STRING((char *)v);
	if(!strcmp(ty, "symbol_t"))	t=BS1_SYM((char *)v);

	if(!strcmp(ty, "bggl2_numarray_t"))
	{
		da=v;
		l=ObjType_GetSize(da)/sizeof(double);
		t=BS1_TYVec_MakeFVec(NULL, l);
		fb=BS1_TY_GetObjData(t);
		for(i=0; i<l; i++)fb[i]=da[i];
	}

	if(!strcmp(ty, "lbxgl_entity_t"))
	{
		t=BS1_MM_AllocObjNameAPtr("lbxgl_entity", sizeof(void *));
		p=BS1_TY_GetObjDataAPtr(t);
		*(void **)p=v;
	}

	return(t);
}

int lbxgl_bs1if_ghandler(LBXGL_Entity *ent, void *data, void **msgdata)
{
	bs1_elem t, l, o;
	int i;

	i=LBXGL_Entity_GetPropertyFloat(ent, "_observer");
	if(i<=0)return(-1);
	o=lbxgl_bs1if_observer[i];
	if(!BS1_OBJECTP(o))
		return(-1);

	l=BS1_MM_NULL;
	for(i=0; msgdata[i]; i++)
	{
		t=LBXGL_BS1IF_Value2Elem(msgdata[i]);
		l=BS1_CONS(t, l);
	}
	l=BS1_TY_NReverse(l);

	t=LBXGL_BS1IF_Value2Elem(ent);
	l=BS1_CONS(t, l);

	BS1_SVM_CallFuncObj(o, BS1_SYM(data), l);
	return(0);
}


bs1_elem lbxgl_bs1if_loadindex(bs1_elem obj, bs1_elem slot)	//AH:ignore
{
	void *p, *q;
	char *s;
	bs1_elem t;
	int i;

	p=BS1_TY_GetObjDataAPtr(obj);
	p=*(void **)p;
	s=BS1_TOSYM(slot);

	if(!strcmp(s, "observer"))
	{
		i=LBXGL_Entity_GetPropertyFloat(p, "_observer");
		if(i<=0)return(BS1_MM_NULL);
		t=lbxgl_bs1if_observer[i];
		return(t);
	}

	q=LBXGL_Entity_GetProperty(p, s);
	t=LBXGL_BS1IF_Value2Elem(q);
	return(t);
}

bs1_elem lbxgl_bs1if_storeindex(bs1_elem obj, bs1_elem slot, bs1_elem val)	//AH:ignore
{
	void *p, *q;
	float *fa;
	char *s;
	bs1_elem t;
	int i;

	p=BS1_TY_GetObjDataAPtr(obj);
	p=*(void **)p;
	s=BS1_TOSYM(slot);

	if(!strcmp(s, "observer"))
	{
		i=LBXGL_BS1IF_IndexObserver(val);
		if(i<=0)return(BS1_MM_NULL);
		LBXGL_Entity_SetPropertyFloat(p, "_observer", i);
		return(val);
	}

	if(BS1_NUMBERP(val))
	{
		LBXGL_Entity_SetPropertyFloat(p, s, BS1_TOFLOAT(val));
		return(val);		
	}

	if(BS1_STRINGP(val) || BS1_SYMBOLP(val))
	{
		LBXGL_Entity_SetPropertyString(p, s, BS1_TOSTRING(val));
		return(val);		
	}

	if(BS1_FVECTORP(val))
	{
		i=BS1_MM_GetObjSize(val)/sizeof(float);
		fa=BS1_TY_GetObjData(val);
		LBXGL_Entity_SetPropertyFVector(p, s, fa, i);
		return(val);		
	}

	return(BS1_MM_NULL);
}

bs1_elem lbxgl_bs1if_methodcall(bs1_elem obj, bs1_elem slot, bs1_elem args)	//AH:ignore
{
	LBXGL_Entity *ent;
	void *p, *q;
	float *fa;
	char *s;
	bs1_elem t;
	int i;

	p=BS1_TY_GetObjDataAPtr(obj);
	ent=*(void **)p;
	s=BS1_TOSYM(slot);

	i=LBXGL_Entity_GetPropertyFloat(ent, "_observer");

	if((i>0) && BS1_OBJECTP(lbxgl_bs1if_observer[i]))
	{
		args=BS1_CONS(obj, args);
		obj=lbxgl_bs1if_observer[i];
		t=BS1_SVM_CallFuncObj(obj, slot, args);
		return(t);
	}


	//FIXME: recreate some kind of entity message-handling system

	return(BS1_MM_NULL);
}


int LBXGL_BS1IF_SpawnEnt(LBXGL_Entity *ent)
{
	char *s;
	bs1_elem t;

	s=LBXGL_Entity_GetPropertyString(ent, "classname");
	t=BS1_SVM_CallFuncObj(BS1_MM_NULL, BS1_SYM(s),
		BS1_CONS(LBXGL_BS1IF_Value2Elem(ent), BS1_MM_NULL));
	return(0);
}

int LBXGL_BS1IF_Init()
{
	int i;

	BS1_GC_AddRootArray(lbxgl_bs1if_observer, 4096);
	lbxgl_bs1if_observer[0]=BS1_MM_FALSE;

	i=BS1_TY_HashType("lbxgl_entity");
	BS1_TY_SetLoadIndexFunc(i, &lbxgl_bs1if_loadindex);
	BS1_TY_SetStoreIndexFunc(i, &lbxgl_bs1if_storeindex);
	BS1_TY_SetMethodCallFunc(i, &lbxgl_bs1if_methodcall);

	return(0);
}
