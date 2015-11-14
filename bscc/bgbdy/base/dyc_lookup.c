#include <bgbgc.h>
#include <bgbdy.h>

BGBGC_ClassSlot *BGBDYC_LookupSlotBasic(
	BGBGC_ClassInfo *inf, char *name)
{
	BGBGC_ClassSlot *tmp;
	BGBGC_ClassInfo *tinf;
	char *s;
	int i;

	for(i=0; i<inf->n_slot; i++)
		if(!strcmp(inf->slot[i]->name, name))
	{
		tmp=inf->slot[i];
		return(tmp);
	}

	return(NULL);
}


#if 0
BGBGC_ClassSlot *BGBDYC_LookupSlotRA(
	BGBGC_ClassInfo *inf, char *name, int nhi)
{
	BGBGC_ClassSlot *tmp;
	char *s;
	int i, hi;

	hi=inf->qhash*4093+nhi;
	tmp=BGBDYC_CheckHashSlot(inf, name, hi);
	if(tmp)return(tmp);

	if(inf->super)
	{
		tmp=BGBDYC_LookupSlotRA(inf->super, name, nhi);
		if(tmp)BGBDYC_AddHashSlot(inf, tmp, hi);
		return(tmp);
	}else
	{
		for(i=0; i<inf->n_misup; i++)
		{
			tmp=BGBDYC_LookupSlotRA(
				inf->misup[i], name, nhi);
			if(tmp)
			{
				BGBDYC_AddHashSlot(inf, tmp, hi);
				return(tmp);
			}
		}
	}

	return(NULL);
}

BGBGC_ClassSlot *BGBDYC_LookupSlotRB(
	BGBGC_ClassInfo *inf, char *name, int nhi)
{
	BGBGC_ClassSlot *tmp;
	BGBGC_ClassInfo *tinf;
	char *s;
	int i, hi;

	hi=inf->qhash*4093+nhi;
	for(i=0; i<inf->n_slot; i++)
	{
		tmp=inf->slot[i];
		if(!strcmp(tmp->name, name))
		{
			BGBDYC_AddHashSlot(inf, tmp, hi);
			return(tmp);
		}
	}

	if(inf->super)
	{
		tmp=BGBDYC_LookupSlotRB(inf->super, name, nhi);
		if(tmp)BGBDYC_AddHashSlot(inf, tmp, hi);
		return(tmp);
	}else
	{
		for(i=0; i<inf->n_misup; i++)
		{
			tmp=BGBDYC_LookupSlotRB(
				inf->misup[i], name, nhi);
			if(tmp)
			{
				BGBDYC_AddHashSlot(inf, tmp, hi);
				return(tmp);
			}
		}
	}

	return(NULL);
}

BGBGC_ClassSlot *BGBDYC_LookupSlot(
	BGBGC_ClassInfo *inf, char *name)
{
	BGBGC_ClassSlot *tmp;
	int hi;

	hi=BGBDYC_CalcHashName(name);
	tmp=BGBDYC_LookupSlotRA(inf, name, hi);
	if(tmp)return(tmp);
	tmp=BGBDYC_LookupSlotRB(inf, name, hi);
	return(tmp);
}
#endif

BGBGC_ClassSlot *BGBDYC_LookupSlotR(
	BGBGC_ClassInfo *inf, char *name, int nhi)
{
	BGBGC_ClassSlot *tmp;
	char *s;
	int i, hi;

	hi=inf->qhash*4093+nhi;
	tmp=BGBDYC_CheckHashSlot(inf, name, hi);
	if(tmp)return(tmp);

	for(i=0; i<inf->n_slot; i++)
	{
		tmp=inf->slot[i];
		if(!strcmp(tmp->name, name))
		{
			BGBDYC_AddHashSlot(inf, tmp, hi);
			return(tmp);
		}
	}

	if(inf->super)
	{
		tmp=BGBDYC_LookupSlotR(inf->super, name, nhi);
		if(tmp)BGBDYC_AddHashSlot(inf, tmp, hi);
		return(tmp);
	}else
	{
		for(i=0; i<inf->n_misup; i++)
		{
			tmp=BGBDYC_LookupSlotR(
				inf->misup[i], name, nhi);
			if(tmp)
			{
				BGBDYC_AddHashSlot(inf, tmp, hi);
				return(tmp);
			}
		}
	}

	return(NULL);
}

BGBGC_ClassSlot *BGBDYC_LookupSlot(
	BGBGC_ClassInfo *inf, char *name)
{
	BGBGC_ClassSlot *tmp;
	int hi;

	hi=BGBDYC_CalcHashName(name);
	tmp=BGBDYC_LookupSlotR(inf, name, hi);
	return(tmp);
}

BGBGC_ClassSlot *BGBDYC_LookupSlotIface(
	BGBGC_ClassInfo *inf, BGBGC_ClassSlot *idx)
{
	BGBGC_ClassSlot *tmp;
	char *s;
	int i, hi;

	hi=inf->qhash*4093+idx->hash;
	tmp=BGBDYC_CheckHashSlot2(inf, idx->name, hi);
	if(tmp)return(tmp);

	for(i=0; i<inf->n_slot; i++)
	{
		tmp=inf->slot[i];
		if(tmp->name==idx->name)
			{ BGBDYC_AddHashSlot(inf, tmp, hi); return(tmp); }
	}

	if(inf->super)
	{
		tmp=BGBDYC_LookupSlotIface(inf->super, idx);
		if(tmp)BGBDYC_AddHashSlot(inf, tmp, hi);
		return(tmp);
	}else
	{
		for(i=0; i<inf->n_misup; i++)
		{
			tmp=BGBDYC_LookupSlotIface(inf->misup[i], idx);
			if(tmp)
			{
				BGBDYC_AddHashSlot(inf, tmp, hi);
				return(tmp);
			}
		}
	}

	return(NULL);
}

BGBGC_ClassMinf *BGBDYC_LookupMethodSigRA(
	BGBGC_ClassInfo *inf, char *name, char *sig, int nhi)
{
	BGBGC_ClassMinf *minf;
	int hi;
	int i;

	hi=inf->qhash*4093+nhi;
	minf=BGBDYC_CheckHashMethodSig(inf, name, sig, hi);
	if(minf)return(minf);

	if(inf->super)
	{
		minf=BGBDYC_LookupMethodSigRA(
			inf->super, name, sig, nhi);
		if(minf)BGBDYC_AddHashMethod(inf, minf, hi);
		return(minf);
	}else
	{
		for(i=0; i<inf->n_misup; i++)
		{
			minf=BGBDYC_LookupMethodSigRA(
				inf->misup[i], name, sig, nhi);
			if(minf)
			{
				BGBDYC_AddHashMethod(inf, minf, hi);
				return(minf);
			}
		}
	}

	return(NULL);
}

BGBGC_ClassMinf *BGBDYC_LookupMethodSigRB(
	BGBGC_ClassInfo *inf, char *name, char *sig, int nhi)
{
	BGBGC_ClassMinf *minf;
	int i, hi;

	hi=inf->qhash*4093+nhi;
	for(i=0; i<inf->n_minf; i++)
	{
		minf=inf->minf[i];
		if(!strcmp(minf->name, name) &&
			((*sig=='?') || !strcmp(minf->sig, sig)))
		{
			BGBDYC_AddHashMethod(inf, minf, hi);
			return(minf);
		}
	}

	if(inf->super)
	{
		minf=BGBDYC_LookupMethodSigRB(
			inf->super, name, sig, nhi);
		if(minf)
		{
			BGBDYC_AddHashMethod(inf, minf, hi);
			return(minf);
		}
	}else
	{
		for(i=0; i<inf->n_misup; i++)
		{
			minf=BGBDYC_LookupMethodSigRB(
				inf->misup[i], name, sig, nhi);
			if(minf)
			{
				BGBDYC_AddHashMethod(inf, minf, hi);
				return(minf);
			}
		}
	}

	return(NULL);
}

char *BGBDYC_RemapNameSpecial(
	BGBGC_ClassInfo *inf, char *name, char *sig)
{
	char tb[256];

	if((*name)=='<')
	{
		if(!strcmp(name, "<init>"))
			return(inf->sname);

		if(!strcmp(name, "<clinit>"))
		{
			sprintf(tb, "_%s_clinit_", inf->sname);
			return(dystrsym(tb));
		}

		if(!strcmp(name, "<deinit>"))
		{
			sprintf(tb, "_%s_deinit_", inf->sname);
			return(dystrsym(tb));
		}
	}

	return(name);
}

BGBGC_ClassMinf *BGBDYC_LookupMethodSigRS(
	BGBGC_ClassInfo *inf, char *name, char *sig)
{
	BGBGC_ClassMinf *minf;
	char *name2;
	int i, hi;

//	name2=name;
//	if(!strcmp(name, "<init>"))
//		name2=inf->sname;
	name2=BGBDYC_RemapNameSpecial(inf, name, sig);

	hi=BGBDYC_CalcHashNameSig(name2, sig);
	hi=inf->qhash*4093+hi;

	minf=BGBDYC_CheckHashMethodSig(inf, name2, sig, hi);
	if(minf)return(minf);

	for(i=0; i<inf->n_minf; i++)
	{
		minf=inf->minf[i];
		if(!strcmp(minf->name, name2) && !strcmp(minf->sig, sig))
		{
			BGBDYC_AddHashMethod(inf, minf, hi);
			return(minf);
		}
	}

	if(inf->super)
	{
		minf=BGBDYC_LookupMethodSigRS(inf->super, name, sig);
		if(minf)
		{
			BGBDYC_AddHashMethod(inf, minf, hi);
			return(minf);
		}
	}else
	{
		for(i=0; i<inf->n_misup; i++)
		{
			minf=BGBDYC_LookupMethodSigRS(inf->misup[i], name, sig);
			if(minf)
			{
				BGBDYC_AddHashMethod(inf, minf, hi);
				return(minf);
			}
		}
	}

	return(NULL);
}

BGBGC_ClassMinf *BGBDYC_LookupMethodSig(
	BGBGC_ClassInfo *inf, char *name, char *sig)
{
	BGBGC_ClassMinf *tmp;
	int hi;

	if(*name=='<')
	{
		tmp=BGBDYC_LookupMethodSigRS(inf, name, sig);
		return(tmp);
//		if(tmp)return(tmp);
	}

	hi=BGBDYC_CalcHashNameSig(name, sig);
	tmp=BGBDYC_LookupMethodSigRA(inf, name, sig, hi);
	if(tmp)return(tmp);
	tmp=BGBDYC_LookupMethodSigRB(inf, name, sig, hi);
	return(tmp);
}

BGBGC_ClassMinf *BGBDYC_GetMethodSig(
	BGBGC_ClassInfo *inf, char *name, char *sig)
{
	BGBGC_ClassMinf *minf, *cur;
	BGBGC_ClassMeth *meth;
	char *s, *t;
	int i, hi;

	if(inf->is_iface)
	{
		minf=BGBDYC_GetIfaceMethodSig(inf, name, sig);
		return(minf);
	}

//	if(!strcmp(name, "<init>"))
//		name=inf->sname;
	name=BGBDYC_RemapNameSpecial(inf, name, sig);

	minf=BGBDYC_LookupMethodSig(inf, name, sig);
	if(minf)return(minf);

	minf=BGBDYC_AddMethodSig(inf, name, sig);
	minf->idx=inf->vinf[inf->ver]->e_minf++;
	inf->vinf[inf->ver]->minf_offs[0][minf->num]=minf->idx;
	minf->type=BGBDYC_SLOT_DIRECT;

	BGBDYC_TouchMinfMethod(inf, minf);
	return(minf);
}

BGBGC_ClassMinf *BGBDYC_GetIfaceMethodSig(
	BGBGC_ClassInfo *inf, char *name, char *sig)
{
	BGBGC_ClassMinf *minf, *cur;
	BGBGC_ClassMeth *meth;
	char *s, *t;
	int i, hi;

//	if(!strcmp(name, "<init>"))
//		name=inf->sname;
	name=BGBDYC_RemapNameSpecial(inf, name, sig);

	minf=BGBDYC_LookupMethodSig(inf, name, sig);
	if(minf)return(minf);

	minf=BGBDYC_AddMethodSig(inf, name, sig);
	minf->type=BGBDYC_SLOT_INTERFACE;
	minf->idx=-1;

	return(minf);
}

BGBGC_ClassMinf *BGBDYC_LookupMethodIface(
	BGBGC_ClassInfo *inf, BGBGC_ClassMinf *idx)
{
	BGBGC_ClassMinf *minf;
	int i, hi;

	hi=(inf->qhash*4093)+idx->hash;
	minf=BGBDYC_CheckHashMethodSig2(inf, idx->name, idx->sig, hi);
	if(minf)return(minf);

	for(i=0; i<inf->n_minf; i++)
	{
		minf=inf->minf[i];
		if((minf->name==idx->name) && (minf->sig==idx->sig))
			{ BGBDYC_AddHashMethod(inf, minf, hi); return(minf); }
	}

	if(inf->super)
	{
		minf=BGBDYC_LookupMethodIface(inf->super, idx);
		if(minf) { BGBDYC_AddHashMethod(inf, minf, hi); return(minf); }
	}else
	{
		for(i=0; i<inf->n_misup; i++)
		{
			minf=BGBDYC_LookupMethodIface(inf->misup[i], idx);
			if(minf)
			{
				BGBDYC_AddHashMethod(inf, minf, hi);
				return(minf);
			}
		}
	}

	return(NULL);
}

BGBGC_ClassSlot *BGBDYC_LookupObjectSlot(
	BGBGC_ClassBase *obj, char *name)
{
	BGBGC_ClassSlot *tmp;

	tmp=BGBDYC_LookupSlot(obj->info, name);
	if(tmp)return(tmp);

	if(obj->proto)
	{
		tmp=BGBDYC_LookupObjProtoSlot(obj->proto, name);
		return(tmp);
	}
	return(NULL);
}

void *BGBDYC_LookupObjectSlotPtr(
	BGBGC_ClassBase *obj, char *name)
{
	BGBGC_ClassSlot *slot;
	slot=BGBDYC_LookupObjectSlot(obj, name);
	return(BGBDYC_GetObjectSlotIdxPtr(obj, slot));
}

char *BGBDYC_LookupObjectSlotSig(
	BGBGC_ClassBase *obj, char *name)
{
	BGBGC_ClassSlot *slot;
	slot=BGBDYC_LookupObjectSlot(obj, name);
	if(!slot)return(NULL);
	return(slot->sig);
}

BGBGC_ClassMinf *BGBDYC_LookupObjectMethodSig(
	BGBGC_ClassBase *obj, char *name, char *sig)
{
	BGBGC_ClassMinf *minf;
	minf=BGBDYC_LookupMethodSig(obj->info, name, sig);
	if(minf)return(minf);

	if(obj->proto)
	{
		minf=BGBDYC_LookupObjProtoMethodSig(obj->proto, name, sig);
		return(minf);
	}

	return(NULL);
}

BGBGC_ClassMinf *BGBDYC_LookupClassMethodSig(
	char *cname, char *sname, char *sig)
{
	BGBGC_ClassInfo *inf;
	BGBGC_ClassMinf *minf;

	inf=BGBDYC_FindClassQName(cname);
	if(!inf)return(NULL);

	minf=BGBDYC_LookupMethodSig(inf, sname, sig);
	return(minf);
}

BGBGC_ClassSlot *BGBDYC_LookupObjectSlotIface(
	BGBGC_ClassBase *obj, BGBGC_ClassSlot *idx)
{
	BGBGC_ClassSlot *tmp;

	tmp=BGBDYC_LookupSlotIface(obj->info, idx);
	if(tmp)return(tmp);

	if(obj->proto)
	{
		tmp=BGBDYC_LookupObjProtoSlotIface(obj->proto, idx);
		return(tmp);
	}
	return(NULL);
}

BGBGC_ClassMinf *BGBDYC_LookupObjectMethodIface(
	BGBGC_ClassBase *obj, BGBGC_ClassMinf *idx)
{
	BGBGC_ClassMinf *tmp;

	tmp=BGBDYC_LookupMethodIface(obj->info, idx);
	if(tmp)return(tmp);

	if(obj->proto)
	{
		tmp=BGBDYC_LookupObjProtoMethodIface(obj->proto, idx);
		return(tmp);
	}
	return(NULL);
}

#if 0
// BGBGC_ClassMinf *BGBDYC_LookupMethodIface(
	BGBGC_ClassInfo *inf, BGBGC_ClassMinf *minf)
{
	BGBGC_ClassMinf *tmp;
	tmp=BGBDYC_LookupMethodSigRA(inf,
		minf->name, minf->sig, minf->hash);
	if(tmp)return(tmp);
	tmp=BGBDYC_LookupMethodSigRB(inf,
		minf->name, minf->sig, minf->hash);
	return(tmp);
}
#endif


int *BGBDYC_GetClassMinfOffsTab(BGBGC_ClassInfo *info, BGBGC_ClassMinf *idx)
{
	BGBGC_ClassInfo *inf, *cur;
	BGBGC_ClassVinf *vinf;
	int i;

	vinf=info->vinf[info->ver];

	inf=idx->info; cur=info; i=0;
	while(cur)
	{
		if(cur==inf)return(vinf->minf_offs[i]);
		cur=cur->super; i++;
	}

	return(NULL);
}

int *BGBDYC_GetObjectMinfOffsTab(BGBGC_ClassBase *obj, BGBGC_ClassMinf *idx)
{
	BGBGC_ClassInfo *inf, *cur;
	int i;

	inf=idx->info; cur=obj->info; i=0;
	while(cur)
	{
		if(cur==inf)return(obj->vinf->minf_offs[i]);
		cur=cur->super; i++;
	}

	return(NULL);
}

dyt BGBDYC_GetStaticMinfFunc(BGBGC_ClassInfo *inf, BGBGC_ClassMinf *minf)
{
	BGBGC_ClassMinf *minf2;
	int *tab;
	void *p;
	int i;

	switch(minf->type)
	{
	case BGBDYC_SLOT_DIRECT:
		return(inf->vinf[inf->ver]->vtab[minf->idx]->fcn);
	case BGBDYC_SLOT_INDIRECT:
//		i=inf->vinf[inf->ver]->minf_offs[0][minf->num];
//		return(inf->vinf[inf->ver]->vtab[i]->fcn);

		tab=BGBDYC_GetClassMinfOffsTab(inf, minf);
		return(inf->vinf[inf->ver]->vtab[tab[minf->num]]->fcn);
	case BGBDYC_SLOT_INTERFACE:
		minf2=BGBDYC_LookupMethodIface(inf, minf);
		if(minf==minf2)break;
		return(BGBDYC_GetStaticMinfFunc(inf, minf2));
	case BGBDYC_SLOT_DYNAMIC:
		p=BGBDYC_GetInitIdxPtr(inf, minf->info->slot[minf->idx]);
		return(*(dyt *)p);
	default: break;
	}

	return(NULL);
}

void BGBDYC_SetStaticMinfFunc(BGBGC_ClassInfo *inf,
	BGBGC_ClassMinf *minf, dyt fcn)
{
	BGBGC_ClassMinf *minf2;
	BGBGC_ClassMeth *meth;
	BGBGC_ClassVinf *vinf;
	int *tab;
	void *p;
	int i, id;

	switch(minf->type)
	{
	case BGBDYC_SLOT_DIRECT:
	case BGBDYC_SLOT_INDIRECT:
//		vinf=inf->vinf[inf->ver];
//		id=vinf->minf_offs[0][minf->num];
//		vinf->vtab[id]->fcn=fcn;

		vinf=inf->vinf[inf->ver];
		tab=BGBDYC_GetClassMinfOffsTab(inf, minf);
		i=tab[minf->num];
		vinf->vtab[i]->fcn=fcn;
		vinf->vtab[i]->ApplyU=dycGetMethodApplyU(fcn);
		vinf->vtab[i]->ApplyV=dycGetMethodApplyV(fcn);
		break;
	case BGBDYC_SLOT_INTERFACE:
		minf2=BGBDYC_LookupMethodIface(inf, minf);
		if(minf==minf2)break;
		BGBDYC_SetStaticMinfFunc(inf, minf2, fcn);
		break;
	case BGBDYC_SLOT_DYNAMIC:
		p=BGBDYC_GetInitIdxPtr(inf, minf->info->slot[minf->idx]);
		*(dyt *)p=fcn;
		break;
	default: break;
	}
}

dyt BGBDYC_GetMinfFunc(BGBGC_ClassBase *obj, BGBGC_ClassMinf *minf)
{
//	BGBGC_ClassInfo *inf;
	BGBGC_ClassMinf *minf2;
	int *tab;
	void *p;
	int i;

//	inf=obj->info;
	switch(minf->type)
	{
	case BGBDYC_SLOT_DIRECT:
//		return(inf->vinf[inf->ver]->vtab[minf->idx]->fcn);
		return(obj->vinf->vtab[minf->idx]->fcn);
	case BGBDYC_SLOT_INDIRECT:
//		i=inf->vinf[inf->ver]->minf_offs[0][minf->num];
//		return(inf->vinf[inf->ver]->vtab[i]->fcn);
		tab=BGBDYC_GetObjectMinfOffsTab(obj, minf);
		i=tab[minf->num];
		return(obj->vinf->vtab[i]->fcn);

	case BGBDYC_SLOT_INTERFACE:
		minf2=BGBDYC_LookupObjectMethodIface(obj, minf);
		if(minf==minf2)break;
		return(BGBDYC_GetMinfFunc(obj, minf2));
	case BGBDYC_SLOT_DYNAMIC:
		p=BGBDYC_GetObjectSlotIdxPtr(obj,
			minf->info->slot[minf->idx]);
		return(*(dyt *)p);
	case BGBDYC_SLOT_PROTOTYPE:
		p=BGBDYC_GetProtoMinfIdxPtr(obj->proto, minf);
		return(*(dyt *)p);
		break;
	default: break;
	}

	return(NULL);
}

void BGBDYC_SetMinfFunc(BGBGC_ClassBase *obj, BGBGC_ClassMinf *minf, dyt fcn)
{
	BGBGC_ClassInfo *inf;
	BGBGC_ClassMinf *minf2;
	BGBGC_ClassVinf *vinf;
	int *tab;
	void *p;
	int i, id;

	inf=obj->info;
	switch(minf->type)
	{
	case BGBDYC_SLOT_DIRECT:
	case BGBDYC_SLOT_INDIRECT:
//		vinf=inf->vinf[inf->ver];
//		id=vinf->minf_offs[0][minf->num];
//		vinf->vtab[id]->fcn=fcn;
		tab=BGBDYC_GetObjectMinfOffsTab(obj, minf);
		i=tab[minf->num];
		obj->vinf->vtab[i]->fcn=fcn;
		obj->vinf->vtab[i]->ApplyU=dycGetMethodApplyU(fcn);
		obj->vinf->vtab[i]->ApplyV=dycGetMethodApplyV(fcn);
		break;
	case BGBDYC_SLOT_INTERFACE:
		minf2=BGBDYC_LookupObjectMethodIface(obj, minf);
		if(minf==minf2)break;
		BGBDYC_SetMinfFunc(obj, minf2, fcn);
		break;
	case BGBDYC_SLOT_DYNAMIC:
		p=BGBDYC_GetObjectSlotIdxPtr(obj,
			minf->info->slot[minf->idx]);
		*(dyt *)p=fcn;
		break;
	case BGBDYC_SLOT_PROTOTYPE:
		p=BGBDYC_GetProtoMinfIdxPtr(obj->proto, minf);
		*(dyt *)p=fcn;
		break;
	default: break;
	}
}

dyt BGBDYC_GetMinfFunc2(BGBGC_ClassBase *obj, BGBGC_ClassMinf *minf,
	BGBGC_ClassBase **robj)
{
//	BGBGC_ClassInfo *inf;
	BGBGC_ClassMinf *minf2;
	int *tab;
	void *p;
	int i;

//	inf=obj->info;
	switch(minf->type)
	{
	case BGBDYC_SLOT_DIRECT:
		*robj=obj;
//		return(inf->vinf[inf->ver]->vtab[minf->idx]->fcn);
		return(obj->vinf->vtab[minf->idx]->fcn);
	case BGBDYC_SLOT_INDIRECT:
		*robj=obj;
//		i=inf->vinf[inf->ver]->minf_offs[0][minf->num];
//		return(inf->vinf[inf->ver]->vtab[i]->fcn);

		tab=BGBDYC_GetObjectMinfOffsTab(obj, minf);
		i=tab[minf->num];
		return(obj->vinf->vtab[i]->fcn);

	case BGBDYC_SLOT_INTERFACE:
//		minf2=BGBDYC_LookupObjectMethodIface(obj, minf);
//		return(BGBDYC_GetMinfFunc(obj, minf2));
		return(BGBDYC_GetMinfFunc_DI(obj, minf, robj));
	case BGBDYC_SLOT_DYNAMIC:
		*robj=obj;
		p=BGBDYC_GetObjectSlotIdxPtr(obj,
			minf->info->slot[minf->idx]);
		return(*(dyt *)p);
	case BGBDYC_SLOT_PROTOTYPE:
		*robj=obj;
		p=BGBDYC_GetProtoMinfIdxPtr(obj->proto, minf);
		return(*(dyt *)p);
		break;
	default: break;
	}

	return(NULL);
}

dyt BGBDYC_GetMinfFunc3U(BGBGC_ClassBase *obj, BGBGC_ClassMinf *minf,
	BGBDYC_ApplyU_t *apply)
{
//	BGBGC_ClassInfo *inf;
	BGBGC_ClassMinf *minf2;
	int *tab;
	void *p;
	int i;

//	inf=obj->info;
	switch(minf->type)
	{
	case BGBDYC_SLOT_DIRECT:
		*apply=obj->vinf->vtab[minf->idx]->ApplyU;
		return(obj->vinf->vtab[minf->idx]->fcn);
	case BGBDYC_SLOT_INDIRECT:
		tab=BGBDYC_GetObjectMinfOffsTab(obj, minf);
		i=tab[minf->num];
		*apply=obj->vinf->vtab[i]->ApplyU;
		return(obj->vinf->vtab[i]->fcn);

	case BGBDYC_SLOT_INTERFACE:
		minf2=BGBDYC_LookupObjectMethodIface(obj, minf);
		if(minf==minf2)break;
		return(BGBDYC_GetMinfFunc3U(obj, minf2, apply));
	case BGBDYC_SLOT_DYNAMIC:
		p=BGBDYC_GetObjectSlotIdxPtr(obj,
			minf->info->slot[minf->idx]);
		*apply=NULL;
		return(*(dyt *)p);
	case BGBDYC_SLOT_PROTOTYPE:
		p=BGBDYC_GetProtoMinfIdxPtr(obj->proto, minf);
		*apply=NULL;
		return(*(dyt *)p);
	default: break;
	}

	*apply=NULL;
	return(NULL);
}

dyt BGBDYC_GetMinfFunc3V(BGBGC_ClassBase *obj, BGBGC_ClassMinf *minf,
	BGBDYC_ApplyV_t *apply)
{
//	BGBGC_ClassInfo *inf;
	BGBGC_ClassMinf *minf2;
	int *tab;
	void *p;
	int i;

//	inf=obj->info;
	switch(minf->type)
	{
	case BGBDYC_SLOT_DIRECT:
		*apply=obj->vinf->vtab[minf->idx]->ApplyV;
		return(obj->vinf->vtab[minf->idx]->fcn);
	case BGBDYC_SLOT_INDIRECT:
		tab=BGBDYC_GetObjectMinfOffsTab(obj, minf);
		i=tab[minf->num];
		*apply=obj->vinf->vtab[i]->ApplyV;
		return(obj->vinf->vtab[i]->fcn);

	case BGBDYC_SLOT_INTERFACE:
		minf2=BGBDYC_LookupObjectMethodIface(obj, minf);
		if(minf==minf2)break;
		return(BGBDYC_GetMinfFunc3V(obj, minf2, apply));
	case BGBDYC_SLOT_DYNAMIC:
		p=BGBDYC_GetObjectSlotIdxPtr(obj,
			minf->info->slot[minf->idx]);
		*apply=NULL;
		return(*(dyt *)p);
	case BGBDYC_SLOT_PROTOTYPE:
		p=BGBDYC_GetProtoMinfIdxPtr(obj->proto, minf);
		*apply=NULL;
		return(*(dyt *)p);
	default: break;
	}

	*apply=NULL;
	return(NULL);
}

int BGBDYC_GetClassSlotArrayBuf(BGBGC_ClassInfo *inf, BGBGC_ClassSlot **arr)
{
	int i, n, n1;

	if(inf->super)
	{
		n=BGBDYC_GetClassSlotArrayBuf(inf->super, arr);
	}else n=0;

	n1=n+inf->n_slot;
	for(i=0; i<inf->n_slot; i++)
		arr[n+i]=inf->slot[i];
	return(n1);
}

int BGBDYC_GetClassMinfArrayBuf(BGBGC_ClassInfo *inf, BGBGC_ClassMinf **arr)
{
	int i, n, n1;

	if(inf->super)
	{
		n=BGBDYC_GetClassMinfArrayBuf(inf->super, arr);
	}else n=0;

	n1=n+inf->n_minf;
	for(i=0; i<inf->n_minf; i++)
		arr[n+i]=inf->minf[i];
	return(n1);
}

BGBGC_ClassSlot **BGBDYC_GetClassSlotArray(BGBGC_ClassInfo *inf)
{
	BGBGC_ClassSlot *buf[1024];
	BGBGC_ClassSlot **arr;
	int i, n;

	n=BGBDYC_GetClassSlotArrayBuf(inf, buf);

	arr=(BGBGC_ClassSlot **)
		gcalloc((n+1)*sizeof(BGBGC_ClassSlot *));
	for(i=0; i<n; i++)arr[i]=buf[i];
	arr[n]=NULL;

	return(arr);
}

BGBGC_ClassMinf **BGBDYC_GetClassMinfArray(BGBGC_ClassInfo *inf)
{
	BGBGC_ClassMinf *buf[1024];
	BGBGC_ClassMinf **arr;
	int i, n;

	n=BGBDYC_GetClassMinfArrayBuf(inf, buf);

	arr=(BGBGC_ClassMinf **)
		gcalloc((n+1)*sizeof(BGBGC_ClassSlot *));
	for(i=0; i<n; i++)arr[i]=buf[i];
	arr[n]=NULL;

	return(arr);
}

BGBGC_ClassSlot **BGBDYC_GetClassDelegateSlotArray(BGBGC_ClassInfo *inf)
{
	BGBGC_ClassSlot *buf[1024];
	BGBGC_ClassSlot **arr;
	int i, n;

//	n=BGBDYC_GetClassSlotArrayBuf(inf, buf);
	n=BGBDYC_EnumClassDelegateSlots(inf, buf, 0, 1024);

	arr=(BGBGC_ClassSlot **)
		gcalloc((n+1)*sizeof(BGBGC_ClassSlot *));
	for(i=0; i<n; i++)arr[i]=buf[i];
	arr[n]=NULL;

	return(arr);
}
