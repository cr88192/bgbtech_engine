#include <bgbasm.h>
#include <bgbgc.h>
#include <bgbdy.h>

// DYLL_BoxSig
//	{ char *sig; };
// DYLL_BoxVal
//	{ char *sig; int sz; byte data[1]; };
// DYLL_BoxPtr
//	{ char *sig; void *ptr; };

DYLL_BoxPtr *dyll_boxptr_flst=NULL;

BGBDY_API DYLL_BoxPtr *DYLL_TypeBox_AllocBoxPtr()
{
	DYLL_BoxPtr *tmp;

	if(dyll_boxptr_flst)
	{
		tmp=dyll_boxptr_flst;
		dyll_boxptr_flst=tmp->ptr;
		return(tmp);
	}

	tmp=gctalloc("_type_boxptr_t", sizeof(DYLL_BoxPtr));
	return(tmp);
}

BGBDY_API void DYLL_TypeBox_FreeBoxPtr(DYLL_BoxPtr *tmp)
{
	tmp->ptr=dyll_boxptr_flst;
	dyll_boxptr_flst=tmp;
}

char *dyll_typebox_sig_toString(dyt obj)
{
	char tb[1024];
	DYLL_BoxSig *tmp;

	tmp=(DYLL_BoxSig *)obj;
	sprintf(tb, "#!(_type_sig_t:\"%s\")", tmp->sig);
	return(gcrstrdup(tb));
}

char *dyll_typebox_boxval_toString(dyt obj)
{
	char tb[1024], tb1[1024];
	DYLL_BoxVal *tmp;
	char *s, *t;
	int i, sz;

	tmp=(DYLL_BoxVal *)obj;
	sz=tmp->sz;
	if(sz>24)sz=24;

	t=tb1;
	for(i=0; i<sz; i++)
	{
		if(i && !(i%4)) *t++=' ';
		sprintf(t, "%02X", tmp->data[i]);
		t+=2;
	}
	*t++=0;

	sprintf(tb, "#!(_type_boxval_t:\"%s\"<%s>)", tmp->sig, tb1);
	return(gcrstrdup(tb));
}

char *dyll_typebox_boxptr_toString(dyt obj)
{
	char tb[1024];
	DYLL_BoxPtr *tmp;

	tmp=(DYLL_BoxPtr *)obj;
	sprintf(tb, "#!(_type_boxptr_t:\"%s\",%p)", tmp->sig, tmp->ptr);
	return(gcrstrdup(tb));
}

dyt dyll_ctop_GetSlot(dyt obj, char *key)
	{ return(dyllCTopGet(obj, key)); }
dyt dyll_ctop_SetSlot(dyt obj, char *key, dyt val)
	{ return(UNDEFINED); }

dyt dyll_ctop_CallMethod(dyt obj, dyt sym, dyt *args, int nargs)
{
	dyt fcn;
	fcn=dyllCTopGet(obj, dysymv(sym));
	if(!fcn || (fcn==UNDEFINED))
		return(UNDEFINED);
	return(dyApplyMethod(obj, fcn, args, nargs));
}

dyt dyll_typebox_boxval_GetSlot(dyt obj, char *key)
{
	DYLL_BoxVal *tmp;
	char *qn;
	tmp=(DYLL_BoxVal *)obj;
	qn=dyllSigGetStructName(tmp->sig);
	if(!qn)return(UNDEFINED);
	return(dyllGetStructSlotDynamic(tmp->data, qn, key));
}

dyt dyll_typebox_boxval_SetSlot(dyt obj, char *key, dyt val)
{
	DYLL_BoxVal *tmp;
	char *qn;
	tmp=(DYLL_BoxVal *)obj;
	qn=dyllSigGetStructName(tmp->sig);
	if(!qn)return(UNDEFINED);
	return(dyllSetStructSlotDynamic(tmp->data, qn, key, val));
}

dyt dyll_typebox_boxptr_GetSlot(dyt obj, char *key)
{
	DYLL_BoxPtr *tmp;
	char *qn;
	tmp=(DYLL_BoxPtr *)obj;
	qn=dyllSigGetStructName(tmp->sig);
	if(!qn)return(UNDEFINED);
	return(dyllGetStructSlotDynamic(tmp->ptr, qn, key));
}

dyt dyll_typebox_boxptr_SetSlot(dyt obj, char *key, dyt val)
{
	DYLL_BoxPtr *tmp;
	char *qn;
	tmp=(DYLL_BoxPtr *)obj;
	qn=dyllSigGetStructName(tmp->sig);
	if(!qn)return(UNDEFINED);
	return(dyllSetStructSlotDynamic(tmp->ptr, qn, key, val));
}


dyt dyll_typebox_boxval_copyvalue(dyt obj)
{
	DYLL_BoxVal *tmp, *tmp2;
	char *s;
	int sz;

	tmp=(DYLL_BoxVal *)obj;
	tmp2=gctalloc("_type_boxval_t", sizeof(DYLL_BoxVal)+tmp->sz);
	tmp2->sig=tmp->sig;
	tmp2->sz=tmp->sz;
	memcpy(tmp2->data, tmp->data, tmp->sz);
	return((dyt)tmp2);
}

dyt dyll_typebox_boxptr_copyvalue(dyt obj)
{
	DYLL_BoxPtr *tmp, *tmp2;

	tmp=(DYLL_BoxPtr *)obj;
//	tmp2=gctalloc("_type_boxptr_t", sizeof(DYLL_BoxPtr));
	tmp2=DYLL_TypeBox_AllocBoxPtr();
	tmp2->sig=tmp->sig;
	tmp2->ptr=tmp->ptr;
	tmp2->step=tmp->step;
	return((dyt)tmp2);
}

int dyll_typebox_boxval_dropvalue(dyt p)
	{ gcfree(p); return(1); }

int dyll_typebox_boxptr_dropvalue(dyt p)
{
//	gcfree(p);
	DYLL_TypeBox_FreeBoxPtr((DYLL_BoxPtr *)p);
	return(1);
}


dyt dyll_typebox_int_copyvalue(dyt p)
{
	void *q;
	q=BGBDY_AllocInt32();
	*(s32 *)q=*(s32 *)p;
	return(q);
}

dyt dyll_typebox_long_copyvalue(dyt p)
{
	void *q;
	q=BGBDY_AllocInt64();
	*(s64 *)q=*(s64 *)p;
	return(q);
}

dyt dyll_typebox_float_copyvalue(dyt p)
{
	void *q;
	q=BGBDY_AllocFloat32();
	*(f32 *)q=*(f32 *)p;
	return(q);
}

dyt dyll_typebox_double_copyvalue(dyt p)
{
	void *q;
	q=BGBDY_AllocFloat64();
	*(f64 *)q=*(f64 *)p;
	return(q);
}

dyt dyll_typebox_int128_copyvalue(dyt p)
{
	void *q;
	q=BGBDY_AllocInt128();
	*(s128 *)q=*(s128 *)p;
	return(q);
}

dyt dyll_typebox_float128_copyvalue(dyt p)
{
	void *q;
	q=BGBDY_AllocFloat128();
	*(f128 *)q=*(f128 *)p;
	return(q);
}

int dyll_typebox_int_dropvalue(dyt p)
	{ BGBDY_FreeInt32((s32 *)p); return(1); }
int dyll_typebox_long_dropvalue(dyt p)
	{ BGBDY_FreeInt64((s64 *)p); return(1); }
int dyll_typebox_float_dropvalue(dyt p)
	{ BGBDY_FreeFloat32((f32 *)p); return(1); }
int dyll_typebox_double_dropvalue(dyt p)
	{ BGBDY_FreeFloat64((f64 *)p); return(1); }
int dyll_typebox_int128_dropvalue(dyt p)
	{ BGBDY_FreeInt128((s128 *)p); return(1); }
int dyll_typebox_float128_dropvalue(dyt p)
	{ BGBDY_FreeFloat128((f128 *)p); return(1); }

dyt dyll_typebox_vec2_copyvalue(dyt p)
	{ return(dyvec2fv((float *)p)); }
dyt dyll_typebox_vec3_copyvalue(dyt p)
	{ return(dyvec3fv((float *)p)); }
dyt dyll_typebox_vec4_copyvalue(dyt p)
	{ return(dyvec4fv((float *)p)); }
dyt dyll_typebox_quat_copyvalue(dyt p)
	{ return(dyquatfv((float *)p)); }

int dyll_typebox_vec2_dropvalue(dyt p)
	{ BGBDY_FreeVec2((float *)p); return(1); }
int dyll_typebox_vec3_dropvalue(dyt p)
	{ BGBDY_FreeVec3((float *)p); return(1); }
int dyll_typebox_vec4_dropvalue(dyt p)
	{ BGBDY_FreeVec4((float *)p); return(1); }
int dyll_typebox_quat_dropvalue(dyt p)
	{ BGBDY_FreeQuat((float *)p); return(1); }


dyt dyll_typebox_vec2d_copyvalue(dyt p)
	{ return(dyvec2dv((double *)p)); }
dyt dyll_typebox_vec3d_copyvalue(dyt p)
	{ return(dyvec3dv((double *)p)); }
dyt dyll_typebox_vec4d_copyvalue(dyt p)
	{ return(dyvec4dv((double *)p)); }
dyt dyll_typebox_quatd_copyvalue(dyt p)
	{ return(dyquatdv((double *)p)); }

int dyll_typebox_vec2d_dropvalue(dyt p)
	{ BGBDY_FreeVec2D((double *)p); return(1); }
int dyll_typebox_vec3d_dropvalue(dyt p)
	{ BGBDY_FreeVec3D((double *)p); return(1); }
int dyll_typebox_vec4d_dropvalue(dyt p)
	{ BGBDY_FreeVec4D((double *)p); return(1); }
int dyll_typebox_quatd_dropvalue(dyt p)
	{ BGBDY_FreeQuatD((double *)p); return(1); }

void BGBGC_InitDyLL_Typebox()
{
	dyTypeToString("_type_sig_t", dyll_typebox_sig_toString);
	dyTypeToString("_type_boxval_t", dyll_typebox_boxval_toString);
	dyTypeToString("_type_boxptr_t", dyll_typebox_boxptr_toString);

	dyTypeGetSlot("_type_boxval_t", dyll_typebox_boxval_GetSlot);
	dyTypeSetSlot("_type_boxval_t", dyll_typebox_boxval_SetSlot);
	dyTypeGetSlot("_type_boxptr_t", dyll_typebox_boxptr_GetSlot);
	dyTypeSetSlot("_type_boxptr_t", dyll_typebox_boxptr_SetSlot);

	dyTypeCopyValue("_type_boxval_t", dyll_typebox_boxval_copyvalue);
	dyTypeDropValue("_type_boxval_t", dyll_typebox_boxval_dropvalue);
	dyTypeCopyValue("_type_boxptr_t", dyll_typebox_boxptr_copyvalue);
	dyTypeDropValue("_type_boxptr_t", dyll_typebox_boxptr_dropvalue);
	
	dyTypeGetSlot("_dyll_ctop_t", dyll_ctop_GetSlot);
	dyTypeSetSlot("_dyll_ctop_t", dyll_ctop_SetSlot);
	dyTypeCallMethod("_dyll_ctop_t", dyll_ctop_CallMethod);

	dyTypeCopyValue("_int_t", dyll_typebox_int_copyvalue);
	dyTypeDropValue("_int_t", dyll_typebox_int_dropvalue);
	dyTypeCopyValue("_long_t", dyll_typebox_long_copyvalue);
	dyTypeDropValue("_long_t", dyll_typebox_long_dropvalue);
	dyTypeCopyValue("_float_t", dyll_typebox_float_copyvalue);
	dyTypeDropValue("_float_t", dyll_typebox_float_dropvalue);
	dyTypeCopyValue("_double_t", dyll_typebox_double_copyvalue);
	dyTypeDropValue("_double_t", dyll_typebox_double_dropvalue);
	dyTypeCopyValue("_int128_t", dyll_typebox_int128_copyvalue);
	dyTypeDropValue("_int128_t", dyll_typebox_int128_dropvalue);
	dyTypeCopyValue("_float128_t", dyll_typebox_float128_copyvalue);
	dyTypeDropValue("_float128_t", dyll_typebox_float128_dropvalue);

	dyTypeCopyValue("_vec2_t", dyll_typebox_vec2_copyvalue);
	dyTypeDropValue("_vec2_t", dyll_typebox_vec2_dropvalue);
	dyTypeCopyValue("_vec3_t", dyll_typebox_vec3_copyvalue);
	dyTypeDropValue("_vec3_t", dyll_typebox_vec3_dropvalue);
	dyTypeCopyValue("_vec4_t", dyll_typebox_vec4_copyvalue);
	dyTypeDropValue("_vec4_t", dyll_typebox_vec4_dropvalue);
	dyTypeCopyValue("_quat_t", dyll_typebox_quat_copyvalue);
	dyTypeDropValue("_quat_t", dyll_typebox_quat_dropvalue);

	dyTypeCopyValue("_vec2d_t", dyll_typebox_vec2d_copyvalue);
	dyTypeDropValue("_vec2d_t", dyll_typebox_vec2d_dropvalue);
	dyTypeCopyValue("_vec3d_t", dyll_typebox_vec3d_copyvalue);
	dyTypeDropValue("_vec3d_t", dyll_typebox_vec3d_dropvalue);
	dyTypeCopyValue("_vec4d_t", dyll_typebox_vec4d_copyvalue);
	dyTypeDropValue("_vec4d_t", dyll_typebox_vec4d_dropvalue);
	dyTypeCopyValue("_quatd_t", dyll_typebox_quatd_copyvalue);
	dyTypeDropValue("_quatd_t", dyll_typebox_quatd_dropvalue);
}

BGBDY_API dyt dyllTypeboxSig(char *str)
{
	DYLL_BoxSig *tmp;
	char *s;

	BGBGC_InitDyLL();

	s=basm_strdup(str);
	tmp=gctalloc("_type_sig_t", sizeof(DYLL_BoxSig));
	tmp->sig=s;

	return((dyt)tmp);
}

BGBDY_API int dyllTypeboxSigP(dyt obj)
	{ return(dytypep(obj, "_type_sig_t")); }

BGBDY_API char *dyllTypeboxSigV(dyt obj)
{
	DYLL_BoxSig *tmp;

	if(!dytypep(obj, "_type_sig_t"))
		return(NULL);
	tmp=(DYLL_BoxSig *)obj;
	return(tmp->sig);
}

BGBDY_API dyt dyllTypeboxSigFromRef(dyt ref)
{
	char *s;

	s=dyllSigFromPtr(ref);
	return(dyllTypeboxSig(s));
}

BGBDY_API int dyllTypeboxSigIsStrP(dyt obj, char *str)
{
	char tb[256];
	char *ty, *s, *t;
	int ret;

	ty=dyllTypeboxSigV(obj);
	return(dyllSigCheckName(ty, str));
}

BGBDY_API int dyllTypeboxSigEqP(dyt obja, dyt objb)
{
	char *tya, *tyb;

	tya=dyllTypeboxSigV(obja);
	tyb=dyllTypeboxSigV(objb);
	return(dyllSigCheckSigEqP(tya, tyb));
}

BGBDY_API dyt dyllTypeboxVal(char *str, void *ptr)
{
	DYLL_BoxVal *tmp;
	char *s;
	int sz;

	BGBGC_InitDyLL();

	s=basm_strdup(str);
	sz=dyllSigSizeList(s);
	tmp=gctalloc("_type_boxval_t", sizeof(DYLL_BoxVal)+sz);
	tmp->sig=s;
	tmp->sz=sz;
	if(ptr)memcpy(tmp->data, ptr, sz);

	return((dyt)tmp);
}

BGBDY_API int dyllTypeboxValP(dyt obj)
	{ return(dytypep(obj, "_type_boxval_t")); }

BGBDY_API char *dyllTypeboxValSig(dyt obj)
{
	DYLL_BoxVal *tmp;

	if(!dytypep(obj, "_type_boxval_t"))
		return(NULL);
	tmp=(DYLL_BoxVal *)obj;
	return(tmp->sig);
}

BGBDY_API void *dyllTypeboxValData(dyt obj)
{
	DYLL_BoxVal *tmp;

	if(!dytypep(obj, "_type_boxval_t"))
		return(NULL);
	tmp=(DYLL_BoxVal *)obj;
	return(tmp->data);
}

BGBDY_API int dyllTypeboxValSize(dyt obj)
{
	DYLL_BoxVal *tmp;

	if(!dytypep(obj, "_type_boxval_t"))
		return(-1);
	tmp=(DYLL_BoxVal *)obj;
	return(tmp->sz);
}


BGBDY_API dyt dyllTypeboxPtrChkFcn(char *str, void *ptr)
{
	dyt p;
	char *s;

	BGBGC_InitDyLL();

	if(!ptr || (ptr==UNDEFINED))
		return(ptr);

	s=dyllGetAddrName(ptr);
	if(s)
	{
		p=dyllGetDynamic(s);
		if(p && (p!=UNDEFINED))
			return(p);
	}

	return(dyllTypeboxPtr(str, ptr));
}

BGBDY_API dyt dyllTypeboxPtr(char *str, void *ptr)
{
	DYLL_BoxPtr *tmp;
	char *s;

	BGBGC_InitDyLL();

	if(!ptr || (ptr==UNDEFINED))
		return(ptr);

	s=basm_strdup(str);
//	tmp=gctalloc("_type_boxptr_t", sizeof(DYLL_BoxPtr));
	tmp=DYLL_TypeBox_AllocBoxPtr();
	tmp->sig=s;
	tmp->ptr=ptr;
	tmp->step=dyllSigQuickSize(tmp->sig);

	return((dyt)tmp);
}

BGBDY_API int dyllTypeboxPtrP(dyt obj)
	{ return(dytypep(obj, "_type_boxptr_t")); }

BGBDY_API char *dyllTypeboxPtrSig(dyt obj)
{
	DYLL_BoxPtr *tmp;

	if(!dytypep(obj, "_type_boxptr_t"))
		return(NULL);
	tmp=(DYLL_BoxPtr *)obj;
	return(tmp->sig);
}

BGBDY_API void *dyllTypeboxPtrData(dyt obj)
{
	DYLL_BoxPtr *tmp;

	if(!dytypep(obj, "_type_boxptr_t"))
		return(NULL);
	tmp=(DYLL_BoxPtr *)obj;
	return(tmp->ptr);
}

dyt dyllTypeboxPtrGetIndex(dyt obj, int idx)
{
	DYLL_BoxPtr *tmp;
	dyt t;
	int sz;

	if(!dytypep(obj, "_type_boxptr_t"))
		return(UNDEFINED);
	tmp=(DYLL_BoxPtr *)obj;
//	sz=idx*dyllSigQuickSize(tmp->sig);
	sz=tmp->step;
	t=dyllGetVarSig(((char *)tmp->ptr)+idx*sz, tmp->sig);
	return(t);
}

dyt dyllTypeboxPtrSetIndex(dyt obj, int idx, dyt val)
{
	DYLL_BoxPtr *tmp;
	dyt t;
	int sz;

	if(!dytypep(obj, "_type_boxptr_t"))
		return(UNDEFINED);
	tmp=(DYLL_BoxPtr *)obj;
//	sz=idx*dyllSigQuickSize(tmp->sig);
	sz=tmp->step;
	t=dyllGetVarSig(((char *)tmp->ptr)+idx*sz, tmp->sig);
	dyllSetVarSig(((char *)tmp->ptr)+idx*sz, tmp->sig, val);
	return(t);
}

dyt dyllTypeboxPtrAddIndex(dyt obj, int idx)
{
	DYLL_BoxPtr *tmp;
	dyt t;
	int sz;

	if(!dytypep(obj, "_type_boxptr_t"))
		return(UNDEFINED);
	tmp=(DYLL_BoxPtr *)obj;
//	sz=idx*dyllSigQuickSize(tmp->sig);
	sz=tmp->step;
//	t=dyllGetVarSig(((char *)tmp->ptr)+idx*sz, tmp->sig);
	t=dyllTypeboxPtr(tmp->sig, ((char *)tmp->ptr)+idx*sz);
	return(t);
}


BGBDY_API char *dyllTypeGetMetaPacksig(char *name)
{
	byte *buf, *buf1;
	char *s, *s0, *s1, *t;
	int i;

	BGBGC_InitDyLL();

	s=name;
	if(!s)return(NULL);

	if(!strncmp(s, "_XC_", 4))
	{
		buf=gcralloc(256);
		BASM_UnmangleXCall(s+4, buf);

		s1=buf;
		while(*s1 && (*s1!='('))s1++;
		if(*s1!='(')return(NULL);
		return(s1);
	}

	printf("dyllTypeGetMetaPacksig: check '%s'\n", s);

//	s1=DYLL_MetaPath_LookupKey2(s);
	s1=dyllMetaLookupKey(s);
	if(!s1)return(NULL);

	printf("dyllTypeGetMetaPacksig: Got '%s'\n", s1);

	if(!strcmp(s1, "func"))
	{
		buf=gcralloc(256);
		sprintf(buf, "%s:sig", s);
//		s1=DYLL_MetaPath_LookupKey2(buf);
		s1=dyllMetaLookupKey(buf);
		if(!s1)return(NULL);
		return(s1);
	}

	if(!strcmp(s1, "typedef") ||
		!strcmp(s1, "type"))
	{
		buf=gcralloc(256);

		sprintf(buf, "%s:sig", s);
		s0=dyllMetaLookupKey(buf);

		if(!s0)return(NULL);

//		while(*s0=='P')s0++;
		if((*s0=='X') || (*s0=='U'))
		{
			s0++;
			s1=dyll_SigReadQName(&s0);
			s1=dyllTypeGetMetaPacksig(s1);
			return(s1);
		}
		
		return(s0);
	}

	if(!strcmp(s1, "struct"))
	{
		buf=gcralloc(256);

		buf1=gcralloc(1024);
		t=buf1;

		for(i=0; i<256; i++)
		{
			sprintf(buf, "%s:field.%d", s, i);
//			s1=DYLL_MetaPath_LookupKey2(buf);
			s1=dyllMetaLookupKey(buf);
			if(!s1)break;

			sprintf(buf, "%s/%s:sig", s, s1, i);
//			s1=DYLL_MetaPath_LookupKey2(buf);
			s1=dyllMetaLookupKey(buf);
			if(!s1)break;

			strcpy(t, s1); t+=strlen(t);
		}

		s1=basm_strdup(buf1);
		return(s1);
	}

	return(NULL);
}


struct dyll_ctop_s
{
	char *name;
	char *prefix;
	void *ptr;
	dyAccessMode access;
};

char *dyll_ctophash_name[4096];
dyt dyll_ctophash_val[4096];

dyt DYLL_CTopGetHash(char *name)
{
	char *s;
	int i, j, hi;

	s=name; hi=0;
	while(*s)hi=hi*4093+(*s++);
	
	for(i=0; i<16; i++)
	{
		j=(hi>>12)&4095;
		s=dyll_ctophash_name[j];
		if(!s)break;
		if(!strcmp(s, name))
			return(dyll_ctophash_val[j]);
		hi*=127;
	}
	return(NULL);
}

void DYLL_CTopSetHash(char *name, dyt val)
{
	char *s;
	int i, j, hi;

	s=name; hi=0;
	while(*s)hi=hi*4093+(*s++);
	
	for(i=0; i<15; i++)
	{
		j=(hi>>12)&4095;
		s=dyll_ctophash_name[j];
		if(!s)
		{
			dyll_ctophash_name[j]=basm_strdup(name);
			dyll_ctophash_val[j]=val;
			return;
		}
		if(!strcmp(s, name))
		{
			dyll_ctophash_val[j]=val;
			return;
		}
		hi*=127;
	}

	j=(hi>>12)&4095;
	dyll_ctophash_name[j]=basm_strdup(name);
	dyll_ctophash_val[j]=val;
}

BGBDY_API dyt dyllNewCTop(char *name, char *prefix)
{
	struct dyll_ctop_s *tmp;

	BGBGC_InitDyLL();
	
	tmp=gctalloc("_dyll_ctop_t", sizeof(struct dyll_ctop_s));
	tmp->name=basm_strdup(name);
	tmp->prefix=basm_strdup(prefix);

	tmp->access=prefix?BGBDY_ACCESS_DEFAULT:BGBDY_ACCESS_ROOTONLY;

	return((dyt)tmp);
}

BGBDY_API dyt dyllCTopGet(dyt ctop, char *name)
{
	char tb[256];
	struct dyll_ctop_s *tmp;
	dyt t;

	tmp=(struct dyll_ctop_s *)ctop;

	if(!dyCheckObjectCurrentAccess(tmp->access, BGBDY_ACCESS_READ_ANY))
		return(UNDEFINED);

	if(tmp->prefix)sprintf(tb, "%s_%s", tmp->prefix, name);
		else strcpy(tb, name);

	t=dyllGetDynamic(tb);
	return(t);
}
