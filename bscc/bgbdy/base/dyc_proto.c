#include <bgbgc.h>
#include <bgbdy.h>

extern void *bgbdyc_mutex;

BGBGC_ClassSlot **bgbdyc_protoslot;	//prototype slots
int bgbdyc_protoslot_num;		//number of prototype slots
int bgbdyc_protoslot_max;		//max prototype slots

BGBGC_ClassMinf **bgbdyc_protominf;	//prototype methods
int bgbdyc_protominf_num;		//number of prototype methods
int bgbdyc_protominf_max;		//max prototype methods

void BGBDYC_InitProto()
{
	int i;

	i=256;
	bgbdyc_protoslot=gcalloc(i*sizeof(BGBGC_ClassSlot *));
	bgbdyc_protominf=gcalloc(i*sizeof(BGBGC_ClassMinf *));
	bgbdyc_protoslot_num=0;
	bgbdyc_protominf_num=0;
	bgbdyc_protoslot_max=i;
	bgbdyc_protominf_max=i;
}

int BGBDYC_CheckAddProtoSlot()
{
	int i;

	if(!bgbdyc_protoslot)
	{
		bgbdyc_protoslot=gcalloc(64*sizeof(BGBGC_ClassSlot *));
		bgbdyc_protoslot_max=64;
	}

	if((bgbdyc_protoslot_num+1)>=bgbdyc_protoslot_max)
	{
		i=bgbdyc_protoslot_max+(bgbdyc_protoslot_max>>1);
		bgbdyc_protoslot=gcrealloc(bgbdyc_protoslot,
			i*sizeof(BGBGC_ClassSlot *));
		bgbdyc_protoslot_max=i;
	}

	return(0);
}

int BGBDYC_CheckAddProtoMinf()
{
	int i;

	if(!bgbdyc_protominf)
	{
		bgbdyc_protominf=gcalloc(64*sizeof(BGBGC_ClassMinf *));
		bgbdyc_protominf_max=64;
	}

	if((bgbdyc_protominf_num+1)>=bgbdyc_protominf_max)
	{
		i=bgbdyc_protominf_max+(bgbdyc_protominf_max>>1);
		bgbdyc_protominf=gcrealloc(bgbdyc_protominf,
			i*sizeof(BGBGC_ClassMinf *));
		bgbdyc_protominf_max=i;
	}

	return(0);
}

BGBGC_ClassSlot *BGBDYC_LookupProtoSlotFl(char *name, char *sig,
	s64 fl, dyAccessMode acc)
{
	BGBGC_ClassSlot *tmp;
	int i;

	for(i=0; i<bgbdyc_protoslot_num; i++)
	{
		tmp=bgbdyc_protoslot[i];
		if(!strcmp(tmp->name, name) &&
			!strcmp(tmp->sig, sig) &&
			(tmp->flags==fl) &&
			(tmp->access==acc))
		{
			return(tmp);
		}
	}
	return(NULL);
}

BGBGC_ClassSlot *BGBDYC_LookupProtoSlot(char *name, char *sig)
{
	return(BGBDYC_LookupProtoSlotFl(name, sig, 0,
		dyGetAccessModeDefault()));
}

BGBGC_ClassMinf *BGBDYC_LookupProtoMinfFl(char *name, char *sig,
	s64 fl, dyAccessMode acc)
{
	BGBGC_ClassMinf *tmp;
	int i;

	for(i=0; i<bgbdyc_protominf_num; i++)
	{
		tmp=bgbdyc_protominf[i];
		if(!strcmp(tmp->name, name) &&
			!strcmp(tmp->sig, sig) &&
			(tmp->flags==fl) &&
			(tmp->access==acc))
		{
			return(tmp);
		}
	}
	return(NULL);
}

BGBGC_ClassMinf *BGBDYC_LookupProtoMinf(char *name, char *sig)
{
	return(BGBDYC_LookupProtoMinfFl(name, sig, 0,
		dyGetAccessModeDefault()));
}

BGBGC_ClassSlot *BGBDYC_GetProtoSlotFl(char *name, char *sig,
	s64 fl, dyAccessMode acc)
{
	BGBGC_ClassSlot *tmp;
	int i;

	tmp=BGBDYC_LookupProtoSlotFl(name, sig, fl, acc);
	if(tmp)return(tmp);

	BGBDYC_CheckAddProtoSlot();

	tmp=gctalloc("_class_slot_t", sizeof(BGBGC_ClassSlot));
	i=bgbdyc_protoslot_num++;
	bgbdyc_protoslot[i]=tmp;
	tmp->num=i;

	tmp->type=BGBDYC_SLOT_PROTOTYPE;
	tmp->name=dystrsym(name);
	tmp->sig=dystrsym(sig);
	tmp->flags=fl;
	tmp->access=acc;
	tmp->hash=BGBDYC_CalcHashName(name);

	tmp->size=dyllSigQuickSize(sig);

	return(tmp);
}

BGBGC_ClassSlot *BGBDYC_GetProtoSlot(char *name, char *sig)
{
	return(BGBDYC_GetProtoSlotFl(name, sig, 0,
		dyGetAccessModeDefault()));
}

BGBGC_ClassMinf *BGBDYC_GetProtoMinfFl(char *name, char *sig,
	s64 fl, dyAccessMode acc)
{
	BGBGC_ClassMinf *tmp;
	int i;

	tmp=BGBDYC_LookupProtoMinfFl(name, sig, fl, acc);
	if(tmp)return(tmp);

	BGBDYC_CheckAddProtoSlot();

	tmp=gctalloc("_class_minf_t", sizeof(BGBGC_ClassMinf));
	i=bgbdyc_protominf_num++;
	bgbdyc_protominf[i]=tmp;
	tmp->num=i;

	tmp->type=BGBDYC_SLOT_PROTOTYPE;
	tmp->name=dystrsym(name);
	tmp->sig=dystrsym(sig);
	tmp->flags=fl;
	tmp->access=acc;
	tmp->hash=BGBDYC_CalcHashNameSig(name, sig);

	return(tmp);
}

BGBGC_ClassMinf *BGBDYC_GetProtoMinf(char *name, char *sig)
{
	return(BGBDYC_GetProtoMinfFl(name, sig, 0,
		dyGetAccessModeDefault()));
}


BGBDY_API dycSlot dycGetProtoSlotInfo(char *name, char *sig)
{
	dycSlot tmp;
	BGBDYC_Init();
	thLockFastMutex(bgbdyc_mutex);
	tmp=BGBDYC_GetProtoSlotFl(name, sig, 0,
		dyGetAccessModeDefault());
	thUnlockFastMutex(bgbdyc_mutex);
	return(tmp);
}

BGBDY_API dycSlot dycGetProtoSlotInfoFl(char *name, char *sig, s64 fl)
{
	dycSlot tmp;
	BGBDYC_Init();
	thLockFastMutex(bgbdyc_mutex);
	tmp=BGBDYC_GetProtoSlotFl(name, sig, fl,
		dyGetAccessModeDefault());
	thUnlockFastMutex(bgbdyc_mutex);
	return(tmp);
}

BGBDY_API dycSlot dycGetProtoSlotInfoFlAcc(
	char *name, char *sig, s64 fl, dyAccessMode acc)
{
	dycSlot tmp;
	BGBDYC_Init();
	thLockFastMutex(bgbdyc_mutex);
	tmp=BGBDYC_GetProtoSlotFl(name, sig, fl, acc);
	thUnlockFastMutex(bgbdyc_mutex);
	return(tmp);
}

BGBDY_API dycMethod dycGetProtoMethodInfo(char *name, char *sig)
{
	dycMethod tmp;
	BGBDYC_Init();
	thLockFastMutex(bgbdyc_mutex);
	tmp=BGBDYC_GetProtoMinfFl(name, sig, 0,
		dyGetAccessModeDefault());
	thUnlockFastMutex(bgbdyc_mutex);
	return(tmp);
}

BGBDY_API dycMethod dycGetProtoMethodInfoFl(char *name, char *sig, s64 fl)
{
	dycMethod tmp;
	BGBDYC_Init();
	thLockFastMutex(bgbdyc_mutex);
	tmp=BGBDYC_GetProtoMinfFl(name, sig, fl,
		dyGetAccessModeDefault());
	thUnlockFastMutex(bgbdyc_mutex);
	return(tmp);
}

BGBDY_API dycMethod dycGetProtoMethodInfoFlAcc(char *name, char *sig,
	s64 fl, dyAccessMode acc)
{
	dycMethod tmp;
	BGBDYC_Init();
	thLockFastMutex(bgbdyc_mutex);
	tmp=BGBDYC_GetProtoMinfFl(name, sig, fl, acc);
	thUnlockFastMutex(bgbdyc_mutex);
	return(tmp);
}


BGBGC_ObjProto *BGBDYC_AllocProtoObj()
{
	BGBGC_ObjProto *tmp;
	tmp=gctalloc("_classobj_proto_t", sizeof(BGBGC_ObjProto));
	return(tmp);
}

BGBGC_ObjProto *BGBDYC_CloneProtoObj(BGBGC_ObjProto *obj)
{
	BGBGC_ObjProto *tmp;

	tmp=gctalloc("_classobj_proto_t", sizeof(BGBGC_ObjProto));

	if(obj->slot)
	{
		tmp->slot=gcalloc(obj->m_slot*sizeof(BGBGC_ClassSlot *));
		tmp->sval=gcalloc(obj->m_slot*sizeof(void *));
		tmp->n_slot=obj->n_slot; tmp->m_slot=obj->m_slot;
		memcpy(tmp->slot, obj->slot,
			obj->m_slot*sizeof(BGBGC_ClassSlot *));
		memcpy(tmp->sval, obj->sval,
			obj->m_slot*sizeof(void *));
	}

	if(obj->minf)
	{
		tmp->minf=gcalloc(obj->m_minf*sizeof(BGBGC_ClassMinf *));
		tmp->mval=gcalloc(obj->m_minf*sizeof(dyt));
		tmp->n_minf=obj->n_minf; tmp->m_minf=obj->m_minf;
		memcpy(tmp->minf, obj->minf,
			obj->m_minf*sizeof(BGBGC_ClassMinf *));
		memcpy(tmp->mval, obj->mval,
			obj->m_minf*sizeof(void *));
	}

	return(tmp);
}

void BGBDYC_FreeProtoObj(BGBGC_ObjProto *obj)
{
	if(obj->slot)gcfree(obj->slot);
	if(obj->sval)gcfree(obj->sval);
	if(obj->minf)gcfree(obj->minf);
	if(obj->mval)gcfree(obj->mval);
	gcfree(obj);
}

void *BGBDYC_LookupProtoSlotIdxPtr(
	BGBGC_ObjProto *obj, BGBGC_ClassSlot *idx)
{
	int i, j;

	for(i=0; i<obj->n_slot; i++)
		if(obj->slot[i] && (obj->slot[i]==idx))
			return((void *)(obj->sval+i));
	return(NULL);
}

dyt *BGBDYC_LookupProtoMinfIdxPtr(
	BGBGC_ObjProto *obj, BGBGC_ClassMinf *idx)
{
	int i, j;

	for(i=0; i<obj->n_minf; i++)
		if(obj->minf[i] && (obj->minf[i]==idx))
			return(obj->mval+i);
	return(NULL);
}

void *BGBDYC_GetProtoSlotIdxPtr(
	BGBGC_ObjProto *obj, BGBGC_ClassSlot *idx)
{
	int i, j;

	for(i=0; i<obj->n_slot; i++)
		if(obj->slot[i] && (obj->slot[i]==idx))
			return((void *)(obj->sval+i));

	if(!obj->slot)
	{
		i=16;
		obj->slot=gcalloc(i*sizeof(BGBGC_ClassSlot *));
		obj->sval=gcalloc(i*sizeof(void *));
		obj->n_slot=0; obj->m_slot=i;
	}

	j=(idx->size+(sizeof(void *)-1))/sizeof(void *);
	if((obj->n_slot+j)>=obj->m_slot)
	{
		i=obj->m_slot+(obj->m_slot>>1);
		obj->slot=gcrealloc(obj->slot, i*sizeof(BGBGC_ClassSlot *));
		obj->sval=gcrealloc(obj->sval, i*sizeof(void *));
		obj->m_slot=i;
	}

	i=obj->n_slot;
	obj->n_slot+=j;
	obj->slot[i]=idx;

	return((void *)(obj->sval+i));
}

dyt *BGBDYC_GetProtoMinfIdxPtr(
	BGBGC_ObjProto *obj, BGBGC_ClassMinf *idx)
{
	int i, j;

	for(i=0; i<obj->n_minf; i++)
		if(obj->minf[i] && (obj->minf[i]==idx))
			return(obj->mval+i);

	if(!obj->minf)
	{
		i=16;
		obj->minf=gcalloc(i*sizeof(BGBGC_ClassMinf *));
		obj->mval=gcalloc(i*sizeof(dyt));
		obj->n_minf=0; obj->m_minf=i;
	}

	if((obj->n_minf+1)>=obj->m_minf)
	{
		i=obj->m_minf+(obj->m_minf>>1);
		obj->minf=gcrealloc(obj->minf, i*sizeof(BGBGC_ClassMinf *));
		obj->mval=gcrealloc(obj->mval, i*sizeof(dyt));
		obj->m_minf=i;
	}

	i=obj->n_minf++;
	obj->minf[i]=idx;
	return(obj->mval+i);
}

BGBGC_ClassSlot *BGBDYC_LookupObjProtoSlot(
	BGBGC_ObjProto *obj, char *name)
{
	int i, j;

	for(i=0; i<obj->n_slot; i++)
		if(obj->slot[i] && !strcmp(obj->slot[i]->name, name))
			return(obj->slot[i]);
	return(NULL);
}

BGBGC_ClassSlot *BGBDYC_LookupObjProtoSlotIface(
	BGBGC_ObjProto *obj, BGBGC_ClassSlot *idx)
{
	int i, j;

	for(i=0; i<obj->n_slot; i++)
		if(obj->slot[i] && (obj->slot[i]->name==idx->name))
			return(obj->slot[i]);
	return(NULL);
}

BGBGC_ClassMinf *BGBDYC_LookupObjProtoMethodSig(
	BGBGC_ObjProto *obj, char *name, char *sig)
{
	int i, j;

	for(i=0; i<obj->n_minf; i++)
		if(!strcmp(obj->minf[i]->name, name) &&
			!strcmp(obj->minf[i]->sig, sig))
				return(obj->minf[i]);
	return(NULL);
}

BGBGC_ClassMinf *BGBDYC_LookupObjProtoMethodIface(
	BGBGC_ObjProto *obj, BGBGC_ClassMinf *idx)
{
	int i, j;

	for(i=0; i<obj->n_minf; i++)
		if((obj->minf[i]->name==idx->name) &&
			(obj->minf[i]->sig==idx->sig))
				return(obj->minf[i]);
	return(NULL);
}


BGBGC_ClassSlot *BGBDYC_AddObjProtoSlot(
	BGBGC_ObjProto *obj, char *name, char *sig)
{
	BGBGC_ClassSlot *tmp;

	tmp=BGBDYC_LookupObjProtoSlot(obj, name);
	if(tmp)return(tmp);
	tmp=BGBDYC_GetProtoSlot(name, sig);
	BGBDYC_GetProtoSlotIdxPtr(obj, tmp);
	return(tmp);
}

BGBGC_ClassMinf *BGBDYC_AddObjProtoMethodSig(
	BGBGC_ObjProto *obj, char *name, char *sig)
{
	BGBGC_ClassMinf *tmp;

	tmp=BGBDYC_LookupObjProtoMethodSig(obj, name, sig);
	if(tmp)return(tmp);
	tmp=BGBDYC_GetProtoMinf(name, sig);
	BGBDYC_GetProtoMinfIdxPtr(obj, tmp);
	return(tmp);
}


int BGBDYC_EnumClassDelegateSlots(BGBGC_ClassInfo *inf,
	BGBGC_ClassSlot **arr, int num, int lim)
{
	BGBGC_ClassSlot *idx;
	int i, j;

	if(inf->super)
	{
		if(num>=lim)return(num);
		num=BGBDYC_EnumClassDelegateSlots(
			inf->super, arr, num, lim);
//		return(num);
	}

	if(num>=lim)return(num);
	for(i=0; i<inf->n_slot; i++)
	{
		idx=inf->slot[i];
//		if(!(idx->flags&BGBDYC_FLAG_DELEGATE))
//			continue;

		if(strncmp(idx->name, "__DI:", 5))
			continue;

		for(j=0; j<num; j++)if(arr[j]==idx)break;
		if(j<num)continue;
		if(num>=lim)break;
		arr[num++]=idx;
	}
	if(num>=lim)return(num);

	for(i=0; i<inf->n_misup; i++)
		num=BGBDYC_EnumClassDelegateSlots(
			inf->misup[i], arr, num, lim);
	return(num);
}

void *BGBDYC_GetObjectSlotIdxPtr_DI(
	BGBGC_ClassBase *obj, BGBGC_ClassSlot *idx)
{
	static BGBGC_ClassBase *stk[64];
	static int stkpos=0;

	BGBGC_ClassSlot *arr[64];
	BGBGC_ClassSlot *idx1;
	BGBGC_ClassBase *obj1;
	void *p;
	int i, n;

	for(i=0; i<stkpos; i++)
		if(stk[i]==obj)return(NULL);
	stk[stkpos++]=obj;

	idx1=BGBDYC_LookupObjectSlotIface(obj, idx);
	if(idx1)
	{
		p=BGBDYC_GetObjectSlotIdxPtr(obj, idx);
		stkpos--;
		return(p);
	}

	n=BGBDYC_EnumClassDelegateSlots(obj->info, arr, 0, 64);

	for(i=0; i<n; i++)
	{
		p=BGBDYC_GetObjectSlotIdxPtr(obj, arr[i]);
		if(!p)continue;
		obj1=*(BGBGC_ClassBase **)p;
		if(!obj1)continue;

		p=BGBDYC_GetObjectSlotIdxPtr_DI(obj1, idx);
		if(p)
		{
			stkpos--;
			return(p);
		}
	}

	stkpos--;
	return(NULL);
}

dyt BGBDYC_GetMinfFunc_DI(BGBGC_ClassBase *obj,
	BGBGC_ClassMinf *idx, BGBGC_ClassBase **robj)
{
	static BGBGC_ClassBase *stk[64];
	static int stkpos=0;

	BGBGC_ClassSlot *arr[64];
	BGBGC_ClassMinf *idx1;
	BGBGC_ClassBase *obj1;
	void *p;
	int i, n;

	for(i=0; i<stkpos; i++)
		if(stk[i]==obj)return(NULL);
	stk[stkpos++]=obj;

	idx1=BGBDYC_LookupObjectMethodIface(obj, idx);
	if(idx1)
	{
		*robj=obj;
		p=BGBDYC_GetMinfFunc(obj, idx);
		stkpos--;
		return(p);
	}

	n=BGBDYC_EnumClassDelegateSlots(obj->info, arr, 0, 64);

	for(i=0; i<n; i++)
	{
		p=BGBDYC_GetObjectSlotIdxPtr(obj, arr[i]);
		if(!p)continue;
		obj1=*(BGBGC_ClassBase **)p;
		if(!obj1)continue;

		p=BGBDYC_GetMinfFunc_DI(obj1, idx, robj);
		if(p)
		{
			stkpos--;
			return(p);
		}
	}

	stkpos--;
	return(NULL);
}

BGBGC_ClassBase *BGBDYC_GetObjectDelegate(BGBGC_ClassBase *obj,
	char *name)
{
	char tb[64];
	BGBGC_ClassSlot *idx;
	void *p;

	sprintf(tb, "__DI:%s", name);
	idx=BGBDYC_LookupObjectSlot(obj, tb);
	if(!idx)return(NULL);
	p=BGBDYC_GetObjectSlotIdxPtr(obj, idx);
	if(!p)return(NULL);
	return(*(BGBGC_ClassBase **)p);
}

void BGBDYC_SetObjectDelegate(BGBGC_ClassBase *obj,
	char *name, BGBGC_ClassBase *obj1)
{
	char tb[64];
	BGBGC_ClassSlot *idx;
	void *p;

	sprintf(tb, "__DI:%s", name);
	idx=BGBDYC_LookupObjectSlot(obj, tb);
	if(!idx)return;
	p=BGBDYC_GetObjectSlotIdxPtr(obj, idx);
	if(!p)return;
	*(BGBGC_ClassBase **)p=obj1;
}
