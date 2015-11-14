#include <bgbgc.h>
#include <bgbdy.h>

extern void *bgbdyc_mutex;

int *BGBDYC_GetStructSlotVOffsTab(
	BGBGC_ClassVinf *vinf, BGBGC_ClassSlot *idx)
{
	BGBGC_ClassInfo *inf, *cur;
	int i;

	inf=idx->info; cur=vinf->info; i=0;
	while(cur)
	{
		if(cur==inf)return(vinf->slot_offs[i]);
		cur=cur->super; i++;
	}

	return(NULL);
}

int *BGBDYC_GetStructMinfOffsTab(BGBGC_ClassVinf *vinf, BGBGC_ClassMinf *idx)
{
	BGBGC_ClassInfo *inf, *cur;
	int i;

	inf=idx->info; cur=vinf->info; i=0;
	while(cur)
	{
		if(cur==inf)return(vinf->minf_offs[i]);
		cur=cur->super; i++;
	}

	return(NULL);
}
void *BGBDYC_GetStructSlotIdxPtr(
	BGBGC_ClassVinf *vinf, void *obj, BGBGC_ClassSlot *idx)
{
	BGBGC_ClassInfo *inf;
	int *tab;

	switch(idx->type)
	{
	case BGBDYC_SLOT_DIRECT:
		return((void *)(((byte *)obj)+idx->offs));
	case BGBDYC_SLOT_INDIRECT:
		tab=BGBDYC_GetStructSlotVOffsTab(vinf, idx);
		return((void *)(((byte *)obj)+tab[idx->num]));
	case BGBDYC_SLOT_STATIC:
		return(idx->val);
	case BGBDYC_SLOT_INTERFACE:
		idx=BGBDYC_LookupSlotIface(vinf->info, idx);
		if(!idx)return(NULL);
		return(BGBDYC_GetStructSlotIdxPtr(vinf, obj, idx));
	default: break;
	}

	return(NULL);
}

void *BGBDYC_GetStructSlotIdxPtrTH(
	BGBGC_ClassVinf *vinf, void *obj, BGBGC_ClassSlot *idx)
{
	BGBGC_ClassInfo *inf;
	int *tab;
	void *p;

	switch(idx->type)
	{
	case BGBDYC_SLOT_DIRECT:
		return((void *)(((byte *)obj)+idx->offs));
	case BGBDYC_SLOT_INDIRECT:
		thLockFastMutex(bgbdyc_mutex);
		tab=BGBDYC_GetStructSlotVOffsTab(vinf, idx);
		p=(void *)(((byte *)obj)+tab[idx->num]);
		thUnlockFastMutex(bgbdyc_mutex);
		return(p);
	case BGBDYC_SLOT_STATIC:
		return(idx->val);
	case BGBDYC_SLOT_INTERFACE:
		thLockFastMutex(bgbdyc_mutex);
		idx=BGBDYC_LookupSlotIface(vinf->info, idx);
		if(!idx)return(NULL);
		p=BGBDYC_GetStructSlotIdxPtr(vinf, obj, idx);
		thUnlockFastMutex(bgbdyc_mutex);
		return(p);
	default: break;
	}
	return(NULL);
}

dyt BGBDYC_GetStructMinfFunc(BGBGC_ClassVinf *vinf, BGBGC_ClassMinf *minf)
{
	BGBGC_ClassMinf *minf2;
	int *tab;
	void *p;
	int i;

	switch(minf->type)
	{
	case BGBDYC_SLOT_DIRECT:
		return(vinf->vtab[minf->idx]->fcn);
	case BGBDYC_SLOT_INDIRECT:
		tab=BGBDYC_GetStructMinfOffsTab(vinf, minf);
		return(vinf->vtab[tab[minf->num]]->fcn);
	case BGBDYC_SLOT_INTERFACE:
		minf2=BGBDYC_LookupMethodIface(vinf->info, minf);
		return(BGBDYC_GetStructMinfFunc(vinf, minf2));
	case BGBDYC_SLOT_DYNAMIC:
		p=BGBDYC_GetInitIdxPtr(vinf->info,
			minf->info->slot[minf->idx]);
		return(*(dyt *)p);
	default: break;
	}

	return(NULL);
}

void BGBDYC_SetStructMinfFunc(BGBGC_ClassVinf *vinf,
	BGBGC_ClassMinf *minf, dyt fcn)
{
	BGBGC_ClassMinf *minf2;
	BGBGC_ClassMeth *meth;
	int *tab;
	void *p;
	int i, id;

	switch(minf->type)
	{
	case BGBDYC_SLOT_DIRECT:
	case BGBDYC_SLOT_INDIRECT:
		tab=BGBDYC_GetStructMinfOffsTab(vinf, minf);
		i=tab[minf->num];
		vinf->vtab[i]->fcn=fcn;
		break;
	case BGBDYC_SLOT_INTERFACE:
		minf2=BGBDYC_LookupMethodIface(vinf->info, minf);
		BGBDYC_SetStructMinfFunc(vinf, minf2, fcn);
		break;
	case BGBDYC_SLOT_DYNAMIC:
		p=BGBDYC_GetInitIdxPtr(vinf->info,
			minf->info->slot[minf->idx]);
		*(dyt *)p=fcn;
		break;
	default: break;
	}
}

void BGBDYC_InitStruct(BGBGC_ClassVinf *vinf, void *obj)
{
	BGBGC_ClassInfo *inf;
	int i;

	inf=vinf->info;

	if(vinf->idata)
	{
		if(vinf->sz_idata<vinf->length)
		{
			memset(obj, 0, vinf->length);
			memcpy(obj, vinf->idata, vinf->sz_idata);
		}else
		{
			memcpy(obj, vinf->idata, vinf->length);
		}
	}else
	{
		memset(obj, 0, vinf->length);
	}
}


void *BGBDYC_GetDynSlotIdxPtrTH(
	dyt obj, BGBGC_ClassSlot *idx)
{
	BGBGC_ClassInfo *inf;
	BGBGC_ClassVinf *vinf;

	inf=idx->info;
	if(inf)
	{
		if(inf->is_struct)
		{
			vinf=inf->vinf[inf->ver];
			return(BGBDYC_GetStructSlotIdxPtrTH(vinf, obj, idx));
		}
		if(inf->is_iface)
		{
		}

		return(BGBDYC_GetObjectSlotIdxPtr((dycObject)obj, idx));
	}

//	if(dycObjectP(obj))
	return(BGBDYC_GetObjectSlotIdxPtr((dycObject)obj, idx));
}


int BGBDYC_GetStructSlotOffset(
	BGBGC_ClassVinf *vinf, BGBGC_ClassSlot *idx)
{
	BGBGC_ClassInfo *inf;
	int *tab;
	void *p;
	int i;

	switch(idx->type)
	{
	case BGBDYC_SLOT_DIRECT:
		return(idx->offs);
	case BGBDYC_SLOT_INDIRECT:
		tab=BGBDYC_GetStructSlotVOffsTab(vinf, idx);
		i=tab[idx->num];
		return(i);
	case BGBDYC_SLOT_STATIC:
		return(-1);
	case BGBDYC_SLOT_INTERFACE:
		idx=BGBDYC_LookupSlotIface(vinf->info, idx);
		if(!idx)return(-1);
		i=BGBDYC_GetStructSlotOffset(vinf, idx);
		return(i);
	default: break;
	}
	return(-1);
}

int BGBDYC_GetSlotOffset(
	BGBGC_ClassInfo *inf, BGBGC_ClassSlot *idx)
{
	BGBGC_ClassVinf *vinf;
	int i;

	vinf=BGBDYC_GetClassSlotVInfo(inf, idx);
	i=BGBDYC_GetStructSlotOffset(vinf, idx);
	return(i);
}
