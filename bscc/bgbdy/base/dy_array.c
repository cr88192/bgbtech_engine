#include <bgbgc.h>
#include <bgbdy.h>

void *bgbdy_array_lock=NULL;

BGBDY_API dyt dyarray(int cnt)
	{ return((dyt)dycNewArray("r", cnt)); }
BGBDY_API int dyarrayp(dyt p)
{
	return(dycArraySigP(p, "r"));
//	return(dycArrayP(p));
}

BGBDY_API dyt dyarrayidx(dyt p, int idx)
{
	dycArray arr;

	if(dycArrayP(p))
	{
		arr=(dycArray)p;
		if((*(arr->sig))=='r')
			return(dycGetArrayr(arr, idx));
		return(dycGetArrayVar(arr, idx));
	}
	return(UNDEFINED);

#if 0
	if(dycArraySigP(p, "r"))
		return(dycGetArrayr((dycArray)p, idx));
	if(!dycArrayP(p))return(UNDEFINED);
	return(dycGetArrayVar((dycArray)p, idx));
#endif
}

BGBDY_API void dyarraysetidx(dyt p, int idx, dyt q)
{
#if 1
	dycArray arr;

	if(dycArrayP(p))
	{
		arr=(dycArray)p;
		if((*(arr->sig))=='r')
		{
			dycSetArrayr(arr, idx, q);
			return;
		}
		dycSetArrayVar(arr, idx, q);
		return;
	}
#endif

#if 0
	if(dycArraySigP(p, "r"))
	{
		dycSetArrayr((dycArray)p, idx, q);
		return;
	}
	if(dycArrayP(p))
		dycSetArrayVar((dycArray)p, idx, q);
#endif
}

BGBDY_API dyt dyarrayidxref(dyt p, int idx)
{
	dycArray arr;
	void *q;
	dyt t;

	if(dycArrayP(p))
	{
		arr=(dycArray)p;
//		if((*(arr->sig))=='r')
//			return(dycGetArrayr(arr, idx));
//		return(dycGetArrayVar(arr, idx));
		q=dycArrayIdxPtr(arr, idx);
		if(!q)return(UNDEFINED);
		t=dyllTypeboxPtr(arr->sig, q);
		return(t);
	}
	return(UNDEFINED);
}

//BGBDY_API dyt *dyarrayv(dyt p) { return(((dycArray)p)->data); }
//BGBDY_API int dyarraysz(dyt p) { return(((dycArray)p)->cnt); }

BGBDY_API dyt *dyarrayv(dyt p) { return(dycArrayV((dycArray)p)); }
BGBDY_API int dyarraysz(dyt p) { return(dycArraySz((dycArray)p)); }

BGBDY_API dyt dyArrayIdx(dyt p, int idx)
	{ return(dyarrayidx(p, idx)); }
BGBDY_API void dyArraySetIdx(dyt p, int idx, dyt q)
	{ dyarraysetidx(p, idx, q); }

BGBDY_API dytf dytfArrayIdx(dytf p, int idx)
	{ return(dytfWrap(dyarrayidx(dytfUnwrap(p), idx))); }
BGBDY_API void dytfArraySetIdx(dytf p, int idx, dytf q)
	{ dyarraysetidx(dytfUnwrap(p), idx, dytfUnwrap(q)); }

BGBDY_API dyt dyWrapArray(dyt *args, int cnt)
{
	dycArray tmp;
	dyt *p;
	int i;

	tmp=dycNewArray("r", cnt);
	p=(dyt *)tmp->data;
	for(i=0; i<cnt; i++)p[i]=args[i];
	return((dyt)tmp);
}


BGBDY_API int dyByteArrayP(dyt p)
{
	return(dycArraySigP(p, "h"));
//	return(dycArrayP(p));
}

BGBDY_API int dyByteArraySize(dyt p)
{
	return(dycArraySz((dycArray)p));
}

BGBDY_API byte *dyByteArrayData(dyt p)
{
	return(dycArrayV((dycArray)p));
}

BGBDY_API dyt dyByteArrayFromBuffer(byte *buf, int sz)
{
	dycArray tmp;
	int i;

	tmp=dycNewArray("h", sz);
	memcpy(tmp->data, buf, sz);
	return((dyt)tmp);
}

char *bgbdy_vec2_toString(dyt obj)
{
	char tb[256];
	float *fa;

	fa=(float *)obj;
	sprintf(tb, "#V2(%f %f)", fa[0], fa[1]);
	return(gcrstrdup(tb));
}

char *bgbdy_vec3_toString(dyt obj)
{
	char tb[256];
	float *fa;

	fa=(float *)obj;
	sprintf(tb, "#V3(%f %f %f)", fa[0], fa[1], fa[2]);
	return(gcrstrdup(tb));
}

char *bgbdy_vec4_toString(dyt obj)
{
	char tb[256];
	float *fa;

	fa=(float *)obj;
	sprintf(tb, "#V4(%f %f %f %f)", fa[0], fa[1], fa[2], fa[3]);
	return(gcrstrdup(tb));
}

char *bgbdy_quat_toString(dyt obj)
{
	char tb[256];
	float *fa;

	fa=(float *)obj;
//	sprintf(tb, "#VQ(%f %f %f %f)", fa[0], fa[1], fa[2], fa[3]);
	sprintf(tb, "#VQ(%f %f %f %f)", fa[3], fa[0], fa[1], fa[2]);
	return(gcrstrdup(tb));
}

char *bgbdy_complex_toString(dyt obj)
{
	char tb[256];
	double *fa;

	fa=(double *)obj;
	sprintf(tb, "#VC(%f %f)", fa[0], fa[1]);
	return(gcrstrdup(tb));
}

dyt bgbdy_vec_GetSlot(dyt obj, char *key)
{
	float *fa;
	char *s;

	fa=(float *)obj; s=key;
	if(!s)return(UNDEFINED);
	if(!strcmp(s, "x"))return(dyflonum(fa[0]));
	if(!strcmp(s, "y"))return(dyflonum(fa[1]));
	if(!strcmp(s, "z"))return(dyflonum(fa[2]));
	if(!strcmp(s, "w"))return(dyflonum(fa[3]));
	return(UNDEFINED);
}

dyt bgbdy_vec_SetSlot(dyt obj, char *key, dyt val)
{
	float *fa;
	char *s;

	fa=(float *)obj; s=key;
	if(!s)return(UNDEFINED);
	if(!strcmp(s, "x")) { fa[0]=dyfloatv(val); return(val); }
	if(!strcmp(s, "y")) { fa[1]=dyfloatv(val); return(val); }
	if(!strcmp(s, "z")) { fa[2]=dyfloatv(val); return(val); }
	if(!strcmp(s, "w")) { fa[3]=dyfloatv(val); return(val); }
	return(UNDEFINED);
}

dyt dyll_array_copyvalue(dyt p)
{
	return((dyt)dycArrayCopyValue((dycArray)p));
}

int dyll_array_dropvalue(dyt p)
{
	return(dycArrayDropValue((dycArray)p));
//	BGBDY_FreeVec2((float *)p); return(1);
}

int BGBDY_InitArrayTypes()
{
	bgbdy_array_lock=thFastMutex();

	dyTypeCopyValue("_array_t", dyll_array_copyvalue);
	dyTypeDropValue("_array_t", dyll_array_dropvalue);

	dyTypeToString("_vec2_t", bgbdy_vec2_toString);
	dyTypeToString("_vec3_t", bgbdy_vec3_toString);
	dyTypeToString("_vec4_t", bgbdy_vec4_toString);
	dyTypeToString("_quat_t", bgbdy_quat_toString);

	dyTypeToString("_complex_t", bgbdy_complex_toString);

	dyTypeGetSlot("_vec2_t", bgbdy_vec_GetSlot);
	dyTypeSetSlot("_vec2_t", bgbdy_vec_SetSlot);
	dyTypeGetSlot("_vec3_t", bgbdy_vec_GetSlot);
	dyTypeSetSlot("_vec3_t", bgbdy_vec_SetSlot);
	dyTypeGetSlot("_vec4_t", bgbdy_vec_GetSlot);
	dyTypeSetSlot("_vec4_t", bgbdy_vec_SetSlot);
	dyTypeGetSlot("_quat_t", bgbdy_vec_GetSlot);
	dyTypeSetSlot("_quat_t", bgbdy_vec_SetSlot);

	return(0);
}

dyt bgbdy_vec2_add(dyt a, dyt b)
{
	float *fa, *fb; fa=(float *)a; fb=(float *)b;
	return(dyvec2(fa[0]+fb[0], fa[1]+fb[1]));
}

dyt bgbdy_vec3_add(dyt a, dyt b)
{
	float *fa, *fb; fa=(float *)a; fb=(float *)b;
	return(dyvec3(fa[0]+fb[0], fa[1]+fb[1], fa[2]+fb[2]));
}

dyt bgbdy_vec4_add(dyt a, dyt b)
{
	float *fa, *fb; fa=(float *)a; fb=(float *)b;
	return(dyvec4(fa[0]+fb[0], fa[1]+fb[1], fa[2]+fb[2], fa[3]+fb[3]));
}

dyt bgbdy_quat_add(dyt a, dyt b)
{
	float *fa, *fb; fa=(float *)a; fb=(float *)b;
	return(dyquat(fa[0]+fb[0], fa[1]+fb[1], fa[2]+fb[2], fa[3]+fb[3]));
}
