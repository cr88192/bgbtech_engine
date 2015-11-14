#include <bgbgc.h>
#include <bgbdy.h>

extern void *bgbdy_array_lock;

dycArray dyc_array_freelist=NULL;

void BGBDYC_Array_Lock(void)
	{ thLockFastMutex(bgbdy_array_lock); }
void BGBDYC_Array_Unlock(void)
	{ thUnlockFastMutex(bgbdy_array_lock); }

dycArray dycAllocArrayHeader(void)
{
	dycArray tmp;

#if 1
	BGBDYC_Array_Lock();
	if(dyc_array_freelist)
	{
		tmp=dyc_array_freelist;
		dyc_array_freelist=(dycArray)tmp->data;
		BGBDYC_Array_Unlock();
		return(tmp);
	}
	BGBDYC_Array_Unlock();
#endif

//	tmp=gctalloc("_array_t", sizeof(BGBDYC_Array)+8);
	tmp=gctalloc("_array_t", sizeof(BGBDYC_Array));
	return(tmp);
}

void dycFreeArrayHeader(dycArray arr)
{
	BGBDYC_Array_Lock();
	arr->data=dyc_array_freelist;
	dyc_array_freelist=arr;
	BGBDYC_Array_Unlock();
}

BGBDY_API dycArray dycNewArray(char *sig, int cnt)
{
	dycArray tmp;
	void *p, *p1;
	int i, asz, offs;

	asz=dyllSigAlignSize(sig);
	offs=sizeof(void *)/asz;
	if(!offs)offs=1;

	p=gctalloc("_array_data_t", (cnt+offs)*asz);
	p1=(void *)(((byte *)p)+(offs*asz));

//	tmp=gctalloc("_array_t", sizeof(dycArray)+8);
	tmp=dycAllocArrayHeader();
	tmp->data=p1;
	tmp->sig=dystrsym(sig);
	tmp->cnt=cnt;
	tmp->step=asz;
	tmp->offs=0;
	tmp->flags=0;

	*(void **)p=(void *)tmp;

	return(tmp);
}

BGBDY_API dycArray dycArrayCopyValue(dycArray arr)
{
	dycArray tmp;

	if(arr->flags&BGBDYC_ARRAY_INDEXER)
	{
//		tmp=gctalloc("_array_t", sizeof(dycArray));
		tmp=dycAllocArrayHeader();
		tmp->data=arr->data;
		tmp->sig=arr->sig;
		tmp->cnt=arr->cnt;
		tmp->step=arr->step;
		tmp->offs=arr->offs;
		tmp->flags=arr->flags;
		return(tmp);
	}

	if(arr->flags&BGBDYC_ARRAY_BYVALUE)
	{
		tmp=dycNewArray(arr->sig, arr->cnt);
		tmp->offs=arr->offs;
		tmp->flags=arr->flags;
		memcpy(tmp->data, arr->data, arr->cnt*arr->step);
		return(tmp);
	}

	return(arr);
//	return(arr->data);
}

BGBDY_API int dycArrayDropValue(dycArray arr)
{
	if(arr->flags&BGBDYC_ARRAY_INDEXER)
	{
		dycFreeArrayHeader(arr);
		return(1);
	}

	if(arr->flags&BGBDYC_ARRAY_BYVALUE)
	{
		gcfree(arr->data);
		dycFreeArrayHeader(arr);
		return(1);
	}
	
	return(0);
}

BGBDY_API int dycArrayP(dyt p)
{
	return(dytypep(p, "_array_t"));
}

BGBDY_API int dycArrayDataP(dyt p)
{
	return(dytypep(p, "_array_data_t"));
}

BGBDY_API int dycArray2P(dyt p)
{
	return(dytypep(p, "_array_t") ||
		dytypep(p, "_array_data_t"));
}

BGBDY_API dycArray dycArrayFromData(dyt p)
{
	void *p1;
	if(dytypep(p, "_array_t"))
		return((dycArray)p);
	if(dytypep(p, "_array_data_t"))
	{
		p1=gcgetbase((void *)p);
		return(*(void **)p1);
	}
	return(NULL);
}

BGBDY_API int dycArraySigP(dyt p, char *sig)
{
	if(!dytypep(p, "_array_t"))
		return(0);
	if(!strcmp(((dycArray)p)->sig, sig))
		return(1);
	return(0);
}

// BGBDY_API void *dycArrayV(dycArray arr)	{ return(arr->data); }
// BGBDY_API int dycArraySz(dycArray arr)	{ return(arr->cnt); }

BGBDY_API void *dycArrayV(dycArray arr)
	{ return(((byte *)arr->data) + arr->offs*arr->step); }
BGBDY_API int dycArraySz(dycArray arr)
	{ return(arr->cnt - arr->offs); }

BGBDY_API char *dycArraySig(dycArray arr)	{ return(arr->sig); }
BGBDY_API int dycArrayStep(dycArray arr)	{ return(arr->step); }

BGBDY_API dycArray dycArrayAddOffset(dycArray arr, int offs)
{
	dycArray tmp;

	tmp=dycAllocArrayHeader();
	tmp->data=arr->data;
	tmp->sig=arr->sig;
	tmp->cnt=arr->cnt;
	tmp->step=arr->step;
	tmp->offs=arr->offs+offs;
	tmp->flags=arr->flags|BGBDYC_ARRAY_INDEXER;
	return(tmp);
}

BGBDY_API int dycArraySubtract(dycArray arra, dycArray arrb)
{
	byte *pa, *pb;
	int i;
	
	if(arra->data==arrb->data)
		{ return(arra->offs-arrb->offs); }
		
	pa=dycArrayIdxPtr(arra, 0);
	pb=dycArrayIdxPtr(arrb, 0);
	i=(pa-pb)/arra->step;
	return(i);
}

BGBDY_API dyt dycArrayIndexRef(dycArray arr, int idx)
{
	idx+=arr->offs;
	if((idx<0) || (idx>=arr->cnt))
		return(NULL);
	return((dyt)dycArrayIdxPtr(arr, idx));
}

BGBDY_API dycArray dycArrayRefArray(dyt ref)
{
	return(dycArrayFromData(ref));
}

BGBDY_API int dycArrayRefP(dyt ref)
{
	return(dycArrayDataP(ref));
}

BGBDY_API int dycArrayRefIndex(dyt ref)
{
	dycArray arr;
	int i;

	arr=dycArrayFromData(ref);
	if(!arr)return(0);
	
	i=((byte *)ref)-((byte *)arr->data);
	i=i/arr->step;
	return(i);
}

BGBDY_API dyt dycArrayRefAddOffset(dyt ref, int offs)
{
	dycArray arr;
	byte *p;
	int i, idx;

	arr=dycArrayFromData(ref);
	if(!arr)return(NULL);
	
	i=((byte *)ref)-((byte *)arr->data);
	i=i/arr->step;

	idx=i+offs;
	if((idx<0) || (idx>=arr->cnt))
		return(NULL);

//	p=((byte *)ref)+offs*arr->step;
	p=((byte *)arr->data)+(idx*arr->step);
	return((dyt)p);
}

BGBDY_API void *dycArrayIdxPtr(dycArray arr, int idx)
{
	idx+=arr->offs;
	if((idx<0) || (idx>=arr->cnt))return(NULL);
	return((void *)(((byte *)arr->data)+(idx*arr->step)));
}

BGBDY_API dyt dycGetArrayVar(dycArray arr, int idx)
{
	void *p;
	dyt t;

	p=dycArrayIdxPtr(arr, idx);
	if(!p)return(UNDEFINED);
	t=dyllGetVarSig(p, arr->sig);
//	t=dyllExtractVariantBufferSig(arr->sig, p);
	return(t);
}

BGBDY_API void dycSetArrayVar(dycArray arr, int idx, dyt val)
{
	void *p;

	p=dycArrayIdxPtr(arr, idx);
	if(!p)return;
	dyllSetVarSig(p, arr->sig, val);
}

BGBDY_API int dycGetArraysb(dycArray arr, int idx)
	{ return(((char *)arr->data)[idx+arr->offs]); }
BGBDY_API int dycGetArrayub(dycArray arr, int idx)
	{ return(((byte *)arr->data)[idx+arr->offs]); }
BGBDY_API int dycGetArrayss(dycArray arr, int idx)
	{ return(((s16 *)arr->data)[idx+arr->offs]); }
BGBDY_API int dycGetArrayus(dycArray arr, int idx)
	{ return(((u16 *)arr->data)[idx+arr->offs]); }
BGBDY_API int dycGetArrayi(dycArray arr, int idx)
	{ return(((int *)arr->data)[idx+arr->offs]); }
BGBDY_API s64 dycGetArrayl(dycArray arr, int idx)
	{ return(((s64 *)arr->data)[idx+arr->offs]); }
BGBDY_API float dycGetArrayf(dycArray arr, int idx)
	{ return(((float *)arr->data)[idx+arr->offs]); }
BGBDY_API double dycGetArrayd(dycArray arr, int idx)
	{ return(((double *)arr->data)[idx+arr->offs]); }
BGBDY_API dyt dycGetArrayr(dycArray arr, int idx)
	{ return(((dyt *)arr->data)[idx+arr->offs]); }
BGBDY_API void *dycGetArrayp(dycArray arr, int idx)
	{ return(((void **)arr->data)[idx+arr->offs]); }

BGBDY_API u32 dycGetArrayui(dycArray arr, int idx)
	{ return(((u32 *)arr->data)[idx+arr->offs]); }

BGBDY_API void dycSetArraysb(dycArray arr, int idx, int val)
	{ ((char *)arr->data)[idx+arr->offs]=val; }
BGBDY_API void dycSetArrayub(dycArray arr, int idx, int val)
	{ ((byte *)arr->data)[idx+arr->offs]=val; }
BGBDY_API void dycSetArrayss(dycArray arr, int idx, int val)
	{ ((s16 *)arr->data)[idx+arr->offs]=val; }
BGBDY_API void dycSetArrayus(dycArray arr, int idx, int val)
	{ ((u16 *)arr->data)[idx+arr->offs]=val; }
BGBDY_API void dycSetArrayi(dycArray arr, int idx, int val)
	{ ((int *)arr->data)[idx+arr->offs]=val; }
BGBDY_API void dycSetArrayl(dycArray arr, int idx, s64 val)
	{ ((s64 *)arr->data)[idx+arr->offs]=val; }
BGBDY_API void dycSetArrayf(dycArray arr, int idx, float val)
	{ ((float *)arr->data)[idx+arr->offs]=val; }
BGBDY_API void dycSetArrayd(dycArray arr, int idx, double val)
	{ ((double *)arr->data)[idx+arr->offs]=val; }
BGBDY_API void dycSetArrayr(dycArray arr, int idx, dyt val)
	{ ((dyt *)arr->data)[idx+arr->offs]=val; }
BGBDY_API void dycSetArrayp(dycArray arr, int idx, void *val)
	{ ((void **)arr->data)[idx+arr->offs]=val; }


BGBDY_API bool dycArrayIdxCheck(dycArray arr, int idx)
{
	if(!arr)
		{ return(false); }
	idx+=arr->offs;
	if((idx<0) || (idx>=arr->cnt))
		{ return(false); }
	return(true);
}

BGBDY_API void *dycArrayIdxPtrBC(dycArray arr, int idx)
{
	idx+=arr->offs;
	if((idx<0) || (idx>=arr->cnt))
	{
		dyllThrow("BoundsCheck");
		return(NULL);
	}
	return((void *)(((byte *)arr->data)+(idx*arr->step)));
}

BGBDY_API int dycGetArraySsb(dycArray arr, int idx)
	{ return(*((char *)dycArrayIdxPtrBC(arr, idx))); }
BGBDY_API int dycGetArraySub(dycArray arr, int idx)
	{ return(*((byte *)dycArrayIdxPtrBC(arr, idx))); }
BGBDY_API int dycGetArraySss(dycArray arr, int idx)
	{ return(*((s16 *)dycArrayIdxPtrBC(arr, idx))); }
BGBDY_API int dycGetArraySus(dycArray arr, int idx)
	{ return(*((u16 *)dycArrayIdxPtrBC(arr, idx))); }
BGBDY_API int dycGetArraySi(dycArray arr, int idx)
	{ return(*((int *)dycArrayIdxPtrBC(arr, idx))); }
BGBDY_API s64 dycGetArraySl(dycArray arr, int idx)
	{ return(*((s64 *)dycArrayIdxPtrBC(arr, idx))); }
BGBDY_API float dycGetArraySf(dycArray arr, int idx)
	{ return(*((float *)dycArrayIdxPtrBC(arr, idx))); }
BGBDY_API double dycGetArraySd(dycArray arr, int idx)
	{ return(*((double *)dycArrayIdxPtrBC(arr, idx))); }
BGBDY_API dyt dycGetArraySr(dycArray arr, int idx)
	{ return(*((dyt *)dycArrayIdxPtrBC(arr, idx))); }
BGBDY_API void *dycGetArraySp(dycArray arr, int idx)
	{ return(*((void **)dycArrayIdxPtrBC(arr, idx))); }

BGBDY_API void dycSetArraySsb(dycArray arr, int idx, int val)
	{ *(char *)dycArrayIdxPtrBC(arr, idx)=val; }
BGBDY_API void dycSetArraySub(dycArray arr, int idx, int val)
	{ *(byte *)dycArrayIdxPtrBC(arr, idx)=val; }
BGBDY_API void dycSetArraySss(dycArray arr, int idx, int val)
	{ *(s16 *)dycArrayIdxPtrBC(arr, idx)=val; }
BGBDY_API void dycSetArraySus(dycArray arr, int idx, int val)
	{ *(u16 *)dycArrayIdxPtrBC(arr, idx)=val; }
BGBDY_API void dycSetArraySi(dycArray arr, int idx, int val)
	{ *(int *)dycArrayIdxPtrBC(arr, idx)=val; }
BGBDY_API void dycSetArraySl(dycArray arr, int idx, s64 val)
	{ *(s64 *)dycArrayIdxPtrBC(arr, idx)=val; }
BGBDY_API void dycSetArraySf(dycArray arr, int idx, float val)
	{ *(float *)dycArrayIdxPtrBC(arr, idx)=val; }
BGBDY_API void dycSetArraySd(dycArray arr, int idx, double val)
	{ *(double *)dycArrayIdxPtrBC(arr, idx)=val; }
BGBDY_API void dycSetArraySr(dycArray arr, int idx, dyt val)
	{ *(dyt *)dycArrayIdxPtrBC(arr, idx)=val; }
BGBDY_API void dycSetArraySp(dycArray arr, int idx, void *val)
	{ *(void **)dycArrayIdxPtrBC(arr, idx)=val; }


BGBDY_API dycArray DYC_NewArrayM_R(char *sig, int ord, int *sz)
{
	dycArray tmp;
	dycArray *pa;
	int i;

	tmp=dycNewArray(sig, sz[0]);

	if(ord>1)
	{
		pa=(dycArray *)tmp->data;
		for(i=0; i<sz[0]; i++)
			pa[i]=DYC_NewArrayM_R(sig+1, ord-1, sz+1);
	}

	return(tmp);
}

BGBDY_API dycArray dycNewArrayM(char *sig, int ord, int *sz)
{
	char tb[256];
	char *s, *t;
	int i;

	t=tb; i=ord;
	while(i--)*t++='Q';
	strcpy(t, sig);

	return(DYC_NewArrayM_R(tb, ord, sz));
}

BGBDY_API dycArray dycNewArrayM2(char *sig, int sz0, int sz1)
{
	int sz[4]; sz[0]=sz0; sz[1]=sz1;
	return(dycNewArrayM(sig, 2, sz));
}

BGBDY_API dycArray dycNewArrayM3(char *sig, int sz0, int sz1, int sz2)
{
	int sz[4]; sz[0]=sz0; sz[1]=sz1; sz[2]=sz2;
	return(dycNewArrayM(sig, 3, sz));
}

BGBDY_API dycArray dycNewArrayM4(char *sig, int sz0, int sz1, int sz2, int sz3)
{
	int sz[4]; sz[0]=sz0; sz[1]=sz1; sz[2]=sz2; sz[3]=sz3;
	return(dycNewArrayM(sig, 4, sz));
}

BGBDY_API dycArray dycNewArrayM5(char *sig,
	int sz0, int sz1, int sz2, int sz3, int sz4)
{
	int sz[8]; sz[0]=sz0; sz[1]=sz1; sz[2]=sz2; sz[3]=sz3; sz[4]=sz4;
	return(dycNewArrayM(sig, 5, sz));
}

BGBDY_API dycArray dycNewArrayM6(char *sig,
	int sz0, int sz1, int sz2, int sz3, int sz4, int sz5)
{
	int sz[8];
	sz[0]=sz0; sz[1]=sz1; sz[2]=sz2; sz[3]=sz3;
	sz[4]=sz4; sz[5]=sz5;
	return(dycNewArrayM(sig, 6, sz));
}

BGBDY_API dycArray dycNewArrayM7(char *sig,
	int sz0, int sz1, int sz2, int sz3,
	int sz4, int sz5, int sz6)
{
	int sz[8];
	sz[0]=sz0; sz[1]=sz1; sz[2]=sz2; sz[3]=sz3;
	sz[4]=sz4; sz[5]=sz5; sz[6]=sz6;
	return(dycNewArrayM(sig, 7, sz));
}

BGBDY_API dycArray dycNewArrayM8(char *sig,
	int sz0, int sz1, int sz2, int sz3,
	int sz4, int sz5, int sz6, int sz7)
{
	int sz[8];
	sz[0]=sz0; sz[1]=sz1; sz[2]=sz2; sz[3]=sz3;
	sz[4]=sz4; sz[5]=sz5; sz[6]=sz6; sz[7]=sz7;
	return(dycNewArrayM(sig, 8, sz));
}

BGBDY_API dycArray dycNewArrayM9(char *sig,
	int sz0, int sz1, int sz2, int sz3,
	int sz4, int sz5, int sz6, int sz7,
	int sz8)
{
	int sz[16];
	sz[0]=sz0; sz[1]=sz1; sz[2]=sz2; sz[3]=sz3;
	sz[4]=sz4; sz[5]=sz5; sz[6]=sz6; sz[7]=sz7;
	sz[8]=sz4; sz[9]=sz5; sz[10]=sz6; sz[11]=sz7;
	return(dycNewArrayM(sig, 9, sz));
}

BGBDY_API dycArray dycNewArrayM10(char *sig,
	int sz0, int sz1, int sz2, int sz3,
	int sz4, int sz5, int sz6, int sz7,
	int sz8, int sz9)
{
	int sz[16];
	sz[0]=sz0; sz[1]=sz1; sz[2]=sz2; sz[3]=sz3;
	sz[4]=sz4; sz[5]=sz5; sz[6]=sz6; sz[7]=sz7;
	sz[8]=sz8; sz[9]=sz9;
	return(dycNewArrayM(sig, 10, sz));
}

BGBDY_API dycArray dycNewArrayM11(char *sig,
	int sz0, int sz1, int sz2 , int sz3,
	int sz4, int sz5, int sz6 , int sz7,
	int sz8, int sz9, int sz10)
{
	int sz[16];
	sz[0]=sz0; sz[1]=sz1; sz[2]=sz2; sz[3]=sz3;
	sz[4]=sz4; sz[5]=sz5; sz[6]=sz6; sz[7]=sz7;
	sz[8]=sz8; sz[9]=sz9; sz[10]=sz10;
	return(dycNewArrayM(sig, 11, sz));
}

BGBDY_API dycArray dycNewArrayM12(char *sig,
	int sz0, int sz1, int sz2 , int sz3,
	int sz4, int sz5, int sz6 , int sz7,
	int sz8, int sz9, int sz10, int sz11)
{
	int sz[16];
	sz[0]=sz0; sz[1]=sz1; sz[ 2]=sz2 ; sz[ 3]=sz3 ;
	sz[4]=sz4; sz[5]=sz5; sz[ 6]=sz6 ; sz[ 7]=sz7 ;
	sz[8]=sz8; sz[9]=sz9; sz[10]=sz10; sz[11]=sz11;
	return(dycNewArrayM(sig, 12, sz));
}


BGBDY_API dycArrayN dycNewArrayN(char *sig, int ord, int *sz)
{
	dycArrayN tmp;
	void *p, *q;
	int i, cnt, asz, osz;

	cnt=1;
	for(i=0; i<ord; i++)cnt*=sz[i];


	asz=dyllSigAlignSize(sig);

	osz=ord*sizeof(int);
	if(osz%asz)osz+=asz-(osz%asz);

	p=gcalloc(osz+cnt*asz);
	q=(void *)(((byte *)p)+osz);

	tmp=gctalloc("_sqrarray_t", sizeof(dycArray));
	tmp->data=q;
	tmp->sig=dystrsym(sig);
	tmp->cnt=cnt;
	tmp->step=asz;

	tmp->ord=ord;
	tmp->psz=p;
	for(i=0; i<ord; i++)tmp->psz[i]=sz[i];

	return(tmp);
}

BGBDY_API int dycArrayNP(dyt p)
{
	return(dytypep(p, "_sqrarray_t"));
}

BGBDY_API int dycArrayNSigP(dyt p, char *sig)
{
	if(!dytypep(p, "_sqrarray_t"))
		return(0);
	if(!strcmp(((dycArrayN)p)->sig, sig))
		return(1);
	return(0);
}

BGBDY_API int dycArrayNSigOrdP(dyt p, char *sig, int ord)
{
	if(!dytypep(p, "_sqrarray_t"))
		return(0);
	if(((dycArrayN)p)->ord!=ord)
		return(0);
	if(!strcmp(((dycArrayN)p)->sig, sig))
		return(1);
	return(0);
}

BGBDY_API void *dycArrayNV(dycArrayN arr)		{ return(arr->data); }
BGBDY_API int *dycArrayNSz(dycArrayN arr)		{ return(arr->psz); }
BGBDY_API int dycArrayNCnt(dycArrayN arr)		{ return(arr->cnt); }
BGBDY_API int dycArrayNOrd(dycArrayN arr)		{ return(arr->ord); }
BGBDY_API char *dycArrayNSig(dycArrayN arr)	{ return(arr->sig); }
BGBDY_API int dycArrayNStep(dycArrayN arr)	{ return(arr->step); }

BGBDY_API void *dycArrayNIdxPtr(dycArrayN arr, int *idx)
{
	int i, j, b, st;

	st=1; b=0;
	for(i=(arr->ord-1); i>=0; i--)
	{
		j=idx[i];
		if(arr->offs)j+=arr->offs[i];
		if((j<0) || (j>=arr->psz[i]))
			return(NULL);
		b+=j*st; st*=arr->psz[i];
	}

	return((void *)(((byte *)arr->data)+(b*arr->step)));
}

BGBDY_API void *dycArrayNIdxPtrBC(dycArrayN arr, int *idx)
{
	int i, j, b, st;

	st=1; b=0;
	for(i=(arr->ord-1); i>=0; i--)
	{
		j=idx[i];
		if(arr->offs)j+=arr->offs[i];
		if((j<0) || (j>=arr->psz[i]))
		{
			dyllThrow("BoundsCheck");
			return(NULL);
		}
		b+=j*st; st*=arr->psz[i];
	}

	return((void *)(((byte *)arr->data)+(b*arr->step)));
}

BGBDY_API int dycGetArrayNsb(dycArrayN arr, int *idx)
	{ return(*((char *)dycArrayNIdxPtr(arr, idx))); }
BGBDY_API int dycGetArrayNub(dycArrayN arr, int *idx)
	{ return(*((byte *)dycArrayNIdxPtr(arr, idx))); }
BGBDY_API int dycGetArrayNss(dycArrayN arr, int *idx)
	{ return(*((s16 *)dycArrayNIdxPtr(arr, idx))); }
BGBDY_API int dycGetArrayNus(dycArrayN arr, int *idx)
	{ return(*((u16 *)dycArrayNIdxPtr(arr, idx))); }
BGBDY_API int dycGetArrayNi(dycArrayN arr, int *idx)
	{ return(*((int *)dycArrayNIdxPtr(arr, idx))); }
BGBDY_API s64 dycGetArrayNl(dycArrayN arr, int *idx)
	{ return(*((s64 *)dycArrayNIdxPtr(arr, idx))); }
BGBDY_API float dycGetArrayNf(dycArrayN arr, int *idx)
	{ return(*((float *)dycArrayNIdxPtr(arr, idx))); }
BGBDY_API double dycGetArrayNd(dycArrayN arr, int *idx)
	{ return(*((double *)dycArrayNIdxPtr(arr, idx))); }
BGBDY_API dyt dycGetArrayNr(dycArrayN arr, int *idx)
	{ return(*((dyt *)dycArrayNIdxPtr(arr, idx))); }
BGBDY_API void *dycGetArrayNp(dycArrayN arr, int *idx)
	{ return(*((void **)dycArrayNIdxPtr(arr, idx))); }

BGBDY_API void dycSetArrayNsb(dycArrayN arr, int *idx, int val)
	{ *(char *)dycArrayNIdxPtr(arr, idx)=val; }
BGBDY_API void dycSetArrayNub(dycArrayN arr, int *idx, int val)
	{ *(byte *)dycArrayNIdxPtr(arr, idx)=val; }
BGBDY_API void dycSetArrayNss(dycArrayN arr, int *idx, int val)
	{ *(s16 *)dycArrayNIdxPtr(arr, idx)=val; }
BGBDY_API void dycSetArrayNus(dycArrayN arr, int *idx, int val)
	{ *(u16 *)dycArrayNIdxPtr(arr, idx)=val; }
BGBDY_API void dycSetArrayNi(dycArrayN arr, int *idx, int val)
	{ *(int *)dycArrayNIdxPtr(arr, idx)=val; }
BGBDY_API void dycSetArrayNl(dycArrayN arr, int *idx, s64 val)
	{ *(s64 *)dycArrayNIdxPtr(arr, idx)=val; }
BGBDY_API void dycSetArrayNf(dycArrayN arr, int *idx, float val)
	{ *(float *)dycArrayNIdxPtr(arr, idx)=val; }
BGBDY_API void dycSetArrayNd(dycArrayN arr, int *idx, double val)
	{ *(double *)dycArrayNIdxPtr(arr, idx)=val; }
BGBDY_API void dycSetArrayNr(dycArrayN arr, int *idx, dyt val)
	{ *(dyt *)dycArrayNIdxPtr(arr, idx)=val; }
BGBDY_API void dycSetArrayNp(dycArrayN arr, int *idx, void *val)
	{ *(void **)dycArrayNIdxPtr(arr, idx)=val; }

BGBDY_API int dycGetArrayNSsb(dycArrayN arr, int *idx)
	{ return(*((char *)dycArrayNIdxPtrBC(arr, idx))); }
BGBDY_API int dycGetArrayNSub(dycArrayN arr, int *idx)
	{ return(*((byte *)dycArrayNIdxPtrBC(arr, idx))); }
BGBDY_API int dycGetArrayNSss(dycArrayN arr, int *idx)
	{ return(*((s16 *)dycArrayNIdxPtrBC(arr, idx))); }
BGBDY_API int dycGetArrayNSus(dycArrayN arr, int *idx)
	{ return(*((u16 *)dycArrayNIdxPtrBC(arr, idx))); }
BGBDY_API int dycGetArrayNSi(dycArrayN arr, int *idx)
	{ return(*((int *)dycArrayNIdxPtrBC(arr, idx))); }
BGBDY_API s64 dycGetArrayNSl(dycArrayN arr, int *idx)
	{ return(*((s64 *)dycArrayNIdxPtrBC(arr, idx))); }
BGBDY_API float dycGetArrayNSf(dycArrayN arr, int *idx)
	{ return(*((float *)dycArrayNIdxPtrBC(arr, idx))); }
BGBDY_API double dycGetArrayNSd(dycArrayN arr, int *idx)
	{ return(*((double *)dycArrayNIdxPtrBC(arr, idx))); }
BGBDY_API dyt dycGetArrayNSr(dycArrayN arr, int *idx)
	{ return(*((dyt *)dycArrayNIdxPtrBC(arr, idx))); }
BGBDY_API void *dycGetArrayNSp(dycArrayN arr, int *idx)
	{ return(*((void **)dycArrayNIdxPtrBC(arr, idx))); }

BGBDY_API void dycSetArrayNSsb(dycArrayN arr, int *idx, int val)
	{ *(char *)dycArrayNIdxPtrBC(arr, idx)=val; }
BGBDY_API void dycSetArrayNSub(dycArrayN arr, int *idx, int val)
	{ *(byte *)dycArrayNIdxPtrBC(arr, idx)=val; }
BGBDY_API void dycSetArrayNSss(dycArrayN arr, int *idx, int val)
	{ *(s16 *)dycArrayNIdxPtrBC(arr, idx)=val; }
BGBDY_API void dycSetArrayNSus(dycArrayN arr, int *idx, int val)
	{ *(u16 *)dycArrayNIdxPtrBC(arr, idx)=val; }
BGBDY_API void dycSetArrayNSi(dycArrayN arr, int *idx, int val)
	{ *(int *)dycArrayNIdxPtrBC(arr, idx)=val; }
BGBDY_API void dycSetArrayNSl(dycArrayN arr, int *idx, s64 val)
	{ *(s64 *)dycArrayNIdxPtrBC(arr, idx)=val; }
BGBDY_API void dycSetArrayNSf(dycArrayN arr, int *idx, float val)
	{ *(float *)dycArrayNIdxPtrBC(arr, idx)=val; }
BGBDY_API void dycSetArrayNSd(dycArrayN arr, int *idx, double val)
	{ *(double *)dycArrayNIdxPtrBC(arr, idx)=val; }
BGBDY_API void dycSetArrayNSr(dycArrayN arr, int *idx, dyt val)
	{ *(dyt *)dycArrayNIdxPtrBC(arr, idx)=val; }
BGBDY_API void dycSetArrayNSp(dycArrayN arr, int *idx, void *val)
	{ *(void **)dycArrayNIdxPtrBC(arr, idx)=val; }


BGBDY_API void *dycArray2IdxPtr(dycArrayN arr, int y, int x)
{
	int i;

	if(arr->offs)
	{
		x+=arr->offs[1];
		y+=arr->offs[0];
	}

	i=y*arr->psz[1]+x;
	return((void *)(((byte *)arr->data)+(i*arr->step)));
}

BGBDY_API void *dycArray2IdxPtrBC(dycArrayN arr, int y, int x)
{
	int i;

	if(arr->offs)
	{
		x+=arr->offs[1];
		y+=arr->offs[0];
	}

	if((y<0) || (y>=arr->psz[0]))dyllThrow("BoundsCheck");
	if((x<0) || (x>=arr->psz[1]))dyllThrow("BoundsCheck");

	i=y*arr->psz[1]+x;
	return((void *)(((byte *)arr->data)+(i*arr->step)));
}

BGBDY_API int dycGetArray2sb(dycArrayN arr, int y, int x)
	{ return(*((char *)dycArray2IdxPtr(arr, y, x))); }
BGBDY_API int dycGetArray2ub(dycArrayN arr, int y, int x)
	{ return(*((byte *)dycArray2IdxPtr(arr, y, x))); }
BGBDY_API int dycGetArray2ss(dycArrayN arr, int y, int x)
	{ return(*((s16 *)dycArray2IdxPtr(arr, y, x))); }
BGBDY_API int dycGetArray2us(dycArrayN arr, int y, int x)
	{ return(*((u16 *)dycArray2IdxPtr(arr, y, x))); }
BGBDY_API int dycGetArray2i(dycArrayN arr, int y, int x)
	{ return(*((int *)dycArray2IdxPtr(arr, y, x))); }
BGBDY_API s64 dycGetArray2l(dycArrayN arr, int y, int x)
	{ return(*((s64 *)dycArray2IdxPtr(arr, y, x))); }
BGBDY_API float dycGetArray2f(dycArrayN arr, int y, int x)
	{ return(*((float *)dycArray2IdxPtr(arr, y, x))); }
BGBDY_API double dycGetArray2d(dycArrayN arr, int y, int x)
	{ return(*((double *)dycArray2IdxPtr(arr, y, x))); }
BGBDY_API dyt dycGetArray2r(dycArrayN arr, int y, int x)
	{ return(*((dyt *)dycArray2IdxPtr(arr, y, x))); }
BGBDY_API void *dycGetArray2p(dycArrayN arr, int y, int x)
	{ return(*((void **)dycArray2IdxPtr(arr, y, x))); }

BGBDY_API void dycSetArray2sb(dycArrayN arr, int y, int x, int val)
	{ *(char *)dycArray2IdxPtr(arr, y, x)=val; }
BGBDY_API void dycSetArray2ub(dycArrayN arr, int y, int x, int val)
	{ *(byte *)dycArray2IdxPtr(arr, y, x)=val; }
BGBDY_API void dycSetArray2ss(dycArrayN arr, int y, int x, int val)
	{ *(s16 *)dycArray2IdxPtr(arr, y, x)=val; }
BGBDY_API void dycSetArray2us(dycArrayN arr, int y, int x, int val)
	{ *(u16 *)dycArray2IdxPtr(arr, y, x)=val; }
BGBDY_API void dycSetArray2i(dycArrayN arr, int y, int x, int val)
	{ *(int *)dycArray2IdxPtr(arr, y, x)=val; }
BGBDY_API void dycSetArray2l(dycArrayN arr, int y, int x, s64 val)
	{ *(s64 *)dycArray2IdxPtr(arr, y, x)=val; }
BGBDY_API void dycSetArray2f(dycArrayN arr, int y, int x, float val)
	{ *(float *)dycArray2IdxPtr(arr, y, x)=val; }
BGBDY_API void dycSetArray2d(dycArrayN arr, int y, int x, double val)
	{ *(double *)dycArray2IdxPtr(arr, y, x)=val; }
BGBDY_API void dycSetArray2r(dycArrayN arr, int y, int x, dyt val)
	{ *(dyt *)dycArray2IdxPtr(arr, y, x)=val; }
BGBDY_API void dycSetArray2p(dycArrayN arr, int y, int x, void *val)
	{ *(void **)dycArray2IdxPtr(arr, y, x)=val; }

BGBDY_API int dycGetArray2Ssb(dycArrayN arr, int y, int x)
	{ return(*((char *)dycArray2IdxPtrBC(arr, y, x))); }
BGBDY_API int dycGetArray2Sub(dycArrayN arr, int y, int x)
	{ return(*((byte *)dycArray2IdxPtrBC(arr, y, x))); }
BGBDY_API int dycGetArray2Sss(dycArrayN arr, int y, int x)
	{ return(*((s16 *)dycArray2IdxPtrBC(arr, y, x))); }
BGBDY_API int dycGetArray2Sus(dycArrayN arr, int y, int x)
	{ return(*((u16 *)dycArray2IdxPtrBC(arr, y, x))); }
BGBDY_API int dycGetArray2Si(dycArrayN arr, int y, int x)
	{ return(*((int *)dycArray2IdxPtrBC(arr, y, x))); }
BGBDY_API s64 dycGetArray2Sl(dycArrayN arr, int y, int x)
	{ return(*((s64 *)dycArray2IdxPtrBC(arr, y, x))); }
BGBDY_API float dycGetArray2Sf(dycArrayN arr, int y, int x)
	{ return(*((float *)dycArray2IdxPtrBC(arr, y, x))); }
BGBDY_API double dycGetArray2Sd(dycArrayN arr, int y, int x)
	{ return(*((double *)dycArray2IdxPtrBC(arr, y, x))); }
BGBDY_API dyt dycGetArray2Sr(dycArrayN arr, int y, int x)
	{ return(*((dyt *)dycArray2IdxPtrBC(arr, y, x))); }
BGBDY_API void *dycGetArray2Sp(dycArrayN arr, int y, int x)
	{ return(*((void **)dycArray2IdxPtrBC(arr, y, x))); }

BGBDY_API void dycSetArray2Ssb(dycArrayN arr, int y, int x, int val)
	{ *(char *)dycArray2IdxPtrBC(arr, y, x)=val; }
BGBDY_API void dycSetArray2Sub(dycArrayN arr, int y, int x, int val)
	{ *(byte *)dycArray2IdxPtrBC(arr, y, x)=val; }
BGBDY_API void dycSetArray2Sss(dycArrayN arr, int y, int x, int val)
	{ *(s16 *)dycArray2IdxPtrBC(arr, y, x)=val; }
BGBDY_API void dycSetArray2Sus(dycArrayN arr, int y, int x, int val)
	{ *(u16 *)dycArray2IdxPtrBC(arr, y, x)=val; }
BGBDY_API void dycSetArray2Si(dycArrayN arr, int y, int x, int val)
	{ *(int *)dycArray2IdxPtrBC(arr, y, x)=val; }
BGBDY_API void dycSetArray2Sl(dycArrayN arr, int y, int x, s64 val)
	{ *(s64 *)dycArray2IdxPtrBC(arr, y, x)=val; }
BGBDY_API void dycSetArray2Sf(dycArrayN arr, int y, int x, float val)
	{ *(float *)dycArray2IdxPtrBC(arr, y, x)=val; }
BGBDY_API void dycSetArray2Sd(dycArrayN arr, int y, int x, double val)
	{ *(double *)dycArray2IdxPtrBC(arr, y, x)=val; }
BGBDY_API void dycSetArray2Sr(dycArrayN arr, int y, int x, dyt val)
	{ *(dyt *)dycArray2IdxPtrBC(arr, y, x)=val; }
BGBDY_API void dycSetArray2Sp(dycArrayN arr, int y, int x, void *val)
	{ *(void **)dycArray2IdxPtrBC(arr, y, x)=val; }



BGBDY_API void *dycArray3IdxPtr(dycArrayN arr, int z, int y, int x)
{
	int i;

	if(arr->offs)
	{
		x+=arr->offs[2];
		y+=arr->offs[1];
		z+=arr->offs[0];
	}

	i=z*(arr->psz[1]*arr->psz[2]) * y*arr->psz[2] + x;
	return((void *)(((byte *)arr->data)+(i*arr->step)));
}

BGBDY_API void *dycArray3IdxPtrBC(dycArrayN arr, int z, int y, int x)
{
	int i;

	if(arr->offs)
	{
		x+=arr->offs[2];
		y+=arr->offs[1];
		z+=arr->offs[0];
	}

	if((z<0) || (z>=arr->psz[0]))dyllThrow("BoundsCheck");
	if((y<0) || (y>=arr->psz[1]))dyllThrow("BoundsCheck");
	if((x<0) || (x>=arr->psz[2]))dyllThrow("BoundsCheck");

	i=z*(arr->psz[1]*arr->psz[2]) * y*arr->psz[2] + x;
	return((void *)(((byte *)arr->data)+(i*arr->step)));
}

BGBDY_API int dycGetArray3sb(dycArrayN arr, int z, int y, int x)
	{ return(*((char *)dycArray3IdxPtr(arr, z, y, x))); }
BGBDY_API int dycGetArray3ub(dycArrayN arr, int z, int y, int x)
	{ return(*((byte *)dycArray3IdxPtr(arr, z, y, x))); }
BGBDY_API int dycGetArray3ss(dycArrayN arr, int z, int y, int x)
	{ return(*((s16 *)dycArray3IdxPtr(arr, z, y, x))); }
BGBDY_API int dycGetArray3us(dycArrayN arr, int z, int y, int x)
	{ return(*((u16 *)dycArray3IdxPtr(arr, z, y, x))); }
BGBDY_API int dycGetArray3i(dycArrayN arr, int z, int y, int x)
	{ return(*((int *)dycArray3IdxPtr(arr, z, y, x))); }
BGBDY_API s64 dycGetArray3l(dycArrayN arr, int z, int y, int x)
	{ return(*((s64 *)dycArray3IdxPtr(arr, z, y, x))); }
BGBDY_API float dycGetArray3f(dycArrayN arr, int z, int y, int x)
	{ return(*((float *)dycArray3IdxPtr(arr, z, y, x))); }
BGBDY_API double dycGetArray3d(dycArrayN arr, int z, int y, int x)
	{ return(*((double *)dycArray3IdxPtr(arr, z, y, x))); }
BGBDY_API dyt dycGetArray3r(dycArrayN arr, int z, int y, int x)
	{ return(*((dyt *)dycArray3IdxPtr(arr, z, y, x))); }
BGBDY_API void *dycGetArray3p(dycArrayN arr, int z, int y, int x)
	{ return(*((void **)dycArray3IdxPtr(arr, z, y, x))); }

BGBDY_API void dycSetArray3sb(dycArrayN arr, int z, int y, int x, int val)
	{ *(char *)dycArray3IdxPtr(arr, z, y, x)=val; }
BGBDY_API void dycSetArray3ub(dycArrayN arr, int z, int y, int x, int val)
	{ *(byte *)dycArray3IdxPtr(arr, z, y, x)=val; }
BGBDY_API void dycSetArray3ss(dycArrayN arr, int z, int y, int x, int val)
	{ *(s16 *)dycArray3IdxPtr(arr, z, y, x)=val; }
BGBDY_API void dycSetArray3us(dycArrayN arr, int z, int y, int x, int val)
	{ *(u16 *)dycArray3IdxPtr(arr, z, y, x)=val; }
BGBDY_API void dycSetArray3i(dycArrayN arr, int z, int y, int x, int val)
	{ *(int *)dycArray3IdxPtr(arr, z, y, x)=val; }
BGBDY_API void dycSetArray3l(dycArrayN arr, int z, int y, int x, s64 val)
	{ *(s64 *)dycArray3IdxPtr(arr, z, y, x)=val; }
BGBDY_API void dycSetArray3f(dycArrayN arr, int z, int y, int x, float val)
	{ *(float *)dycArray3IdxPtr(arr, z, y, x)=val; }
BGBDY_API void dycSetArray3d(dycArrayN arr, int z, int y, int x, double val)
	{ *(double *)dycArray3IdxPtr(arr, z, y, x)=val; }
BGBDY_API void dycSetArray3r(dycArrayN arr, int z, int y, int x, dyt val)
	{ *(dyt *)dycArray3IdxPtr(arr, z, y, x)=val; }
BGBDY_API void dycSetArray3p(dycArrayN arr, int z, int y, int x, void *val)
	{ *(void **)dycArray3IdxPtr(arr, z, y, x)=val; }

BGBDY_API int dycGetArray3Ssb(dycArrayN arr, int z, int y, int x)
	{ return(*((char *)dycArray3IdxPtrBC(arr, z, y, x))); }
BGBDY_API int dycGetArray3Sub(dycArrayN arr, int z, int y, int x)
	{ return(*((byte *)dycArray3IdxPtrBC(arr, z, y, x))); }
BGBDY_API int dycGetArray3Sss(dycArrayN arr, int z, int y, int x)
	{ return(*((s16 *)dycArray3IdxPtrBC(arr, z, y, x))); }
BGBDY_API int dycGetArray3Sus(dycArrayN arr, int z, int y, int x)
	{ return(*((u16 *)dycArray3IdxPtrBC(arr, z, y, x))); }
BGBDY_API int dycGetArray3Si(dycArrayN arr, int z, int y, int x)
	{ return(*((int *)dycArray3IdxPtrBC(arr, z, y, x))); }
BGBDY_API s64 dycGetArray3Sl(dycArrayN arr, int z, int y, int x)
	{ return(*((s64 *)dycArray3IdxPtrBC(arr, z, y, x))); }
BGBDY_API float dycGetArray3Sf(dycArrayN arr, int z, int y, int x)
	{ return(*((float *)dycArray3IdxPtrBC(arr, z, y, x))); }
BGBDY_API double dycGetArray3Sd(dycArrayN arr, int z, int y, int x)
	{ return(*((double *)dycArray3IdxPtrBC(arr, z, y, x))); }
BGBDY_API dyt dycGetArray3Sr(dycArrayN arr, int z, int y, int x)
	{ return(*((dyt *)dycArray3IdxPtrBC(arr, z, y, x))); }
BGBDY_API void *dycGetArray3Sp(dycArrayN arr, int z, int y, int x)
	{ return(*((void **)dycArray3IdxPtrBC(arr, z, y, x))); }

BGBDY_API void dycSetArray3Ssb(dycArrayN arr, int z, int y, int x, int val)
	{ *(char *)dycArray3IdxPtrBC(arr, z, y, x)=val; }
BGBDY_API void dycSetArray3Sub(dycArrayN arr, int z, int y, int x, int val)
	{ *(byte *)dycArray3IdxPtrBC(arr, z, y, x)=val; }
BGBDY_API void dycSetArray3Sss(dycArrayN arr, int z, int y, int x, int val)
	{ *(s16 *)dycArray3IdxPtrBC(arr, z, y, x)=val; }
BGBDY_API void dycSetArray3Sus(dycArrayN arr, int z, int y, int x, int val)
	{ *(u16 *)dycArray3IdxPtrBC(arr, z, y, x)=val; }
BGBDY_API void dycSetArray3Si(dycArrayN arr, int z, int y, int x, int val)
	{ *(int *)dycArray3IdxPtrBC(arr, z, y, x)=val; }
BGBDY_API void dycSetArray3Sl(dycArrayN arr, int z, int y, int x, s64 val)
	{ *(s64 *)dycArray3IdxPtrBC(arr, z, y, x)=val; }
BGBDY_API void dycSetArray3Sf(dycArrayN arr, int z, int y, int x, float val)
	{ *(float *)dycArray3IdxPtrBC(arr, z, y, x)=val; }
BGBDY_API void dycSetArray3Sd(dycArrayN arr, int z, int y, int x, double val)
	{ *(double *)dycArray3IdxPtrBC(arr, z, y, x)=val; }
BGBDY_API void dycSetArray3Sr(dycArrayN arr, int z, int y, int x, dyt val)
	{ *(dyt *)dycArray3IdxPtrBC(arr, z, y, x)=val; }
BGBDY_API void dycSetArray3Sp(dycArrayN arr, int z, int y, int x, void *val)
	{ *(void **)dycArray3IdxPtrBC(arr, z, y, x)=val; }
