#include <bgbgc.h>
#include <bgbdy.h>

BGBGC_ClassInfo *bgbdyc_fisrt;
BGBGC_ClassInfo *bgbdyc_classhash[4096];

BGBGC_Alias *bgbdyc_classalias[1024];
int bgbdyc_nclassalias=0;

BGBGC_ClassInfo *(*bgbdyc_classresolver[256])(char *name);
int bgbdyc_nclassresolver=0;

void *bgbdyc_mutex=NULL;

dyt bgbdyc_classobj_copyvalue(dyt p)
{
	BGBGC_ClassBase *obj, *tmp;

	obj=(BGBGC_ClassBase *)p;
	if(obj->info->is_struct)
	{
//		tmp=BGBDYC_CloneInstance(obj);
		tmp=dycCopyObject(obj);
		return((dyt)tmp);
	}
	return(p);
}

int bgbdyc_classobj_dropvalue(dyt p)
{
	BGBGC_ClassBase *obj;

	obj=(BGBGC_ClassBase *)p;
	if(obj->info->is_struct)
	{
		dycCallSig(obj, "<deinit>", "()v");
		BGBDYC_FreeInstance(obj);
		return(1);
	}
	return(0);
}

void BGBDYC_Init()
{
	static int init=0;
	int i;

	if(init)return;
	init=1;

	bgbdyc_mutex=thFastMutex();

	dyTypeCopyValue("_classobj_t", bgbdyc_classobj_copyvalue);
	dyTypeDropValue("_classobj_t", bgbdyc_classobj_dropvalue);

	BGBDYC_InitHash();
}

void BGBDYC_RegisterClassResolver(BGBGC_ClassInfo *(*fn)(char *name))
{
	int i;
	
	i=bgbdyc_nclassresolver++;
	bgbdyc_classresolver[i]=fn;
}

void BGBDYC_RegisterClassAlias(char *name, char *altname)
{
	char *s0, *s1;
	int i;
	
	s0=dystrsym(name);
	s1=dystrsym(altname);
	
	for(i=0; i<bgbdyc_nclassalias; i++)
	{
		if(bgbdyc_classalias[i]->name!=s0)
			continue;
		if(bgbdyc_classalias[i]->altname!=s1)
			continue;
		return;
	}
	
	i=bgbdyc_nclassalias++;
	bgbdyc_classalias[i]=gctalloc("_dyc_alias_t", sizeof(BGBGC_Alias));
	bgbdyc_classalias[i]->name=s0;
	bgbdyc_classalias[i]->altname=s1;
}

char *BGBDYC_LookupClassAlias(char *name)
{
	char *s0, *s1;
	int i;
	
	s0=dystrsym(name);	
	for(i=0; i<bgbdyc_nclassalias; i++)
	{
		if(bgbdyc_classalias[i]->name==s0)
			return(bgbdyc_classalias[i]->altname);
	}
	return(NULL);
}

BGBGC_ClassInfo *BGBDYC_LookupClassSName(
	BGBGC_ClassInfo *last, char *name)
{
	BGBGC_ClassInfo *cur;

	cur=last;
	while(cur)
	{
		if(!strcmp(cur->sname, name))
			return(cur);
		cur=cur->next;
	}

	return(NULL);
}

BGBGC_ClassInfo *BGBDYC_LookupClassQName(
	BGBGC_ClassInfo *last, char *name, int fl)
{
	static int rcnt=0;

	BGBGC_ClassInfo *cur;
	char *s;
	int i;

	cur=last;
	while(cur)
	{
		if(!strcmp(cur->qname, name))
			return(cur);
		cur=cur->next;
	}

	if(rcnt>64)return(NULL);
	rcnt++;

	s=BGBDYC_LookupClassAlias(name);
	if(s)
	{
		cur=BGBDYC_LookupClassQName(bgbdyc_fisrt, s, fl);
		if(cur) { rcnt--; return(cur); }
	}

	if(fl&1)
	{
		for(i=0; i<bgbdyc_nclassresolver; i++)
		{
			cur=bgbdyc_classresolver[i](name);
			if(cur && !strcmp(cur->qname, name))
				{ rcnt--; return(cur); }
		}
	}

	rcnt--;
	return(NULL);
}

BGBGC_ClassInfo *BGBDYC_FindClassQNameFl(char *name, int fl)
{
	BGBGC_ClassInfo *tmp;
	char *s;
	int i;

	s=name; i=0;
	while(*s)i=(i+(*s++))*4093;
	i=(i>>12)&4095;

	tmp=bgbdyc_classhash[i];
	if(tmp && !strcmp(tmp->qname, name))
		return(tmp);
	tmp=BGBDYC_LookupClassQName(bgbdyc_fisrt, name, fl);
	bgbdyc_classhash[i]=tmp;
	return(tmp);
}

BGBGC_ClassInfo *BGBDYC_FindClassQName(char *name)
{
	return(BGBDYC_FindClassQNameFl(name, 1));
}

#if 0
int BGBDYC_IndexClassSuper(BGBGC_ClassInfo *inf, BGBGC_ClassInfo *super)
{
	BGBGC_ClassVinf *vinf;
	int i;

	vinf=inf->vinf[inf->ver];

	if(!inf->mi_class)
	{
		i=16;
		inf->mi_class=gcalloc(i*sizeof(BGBGC_ClassInfo *));
		vinf->mi_vtab=gcalloc(i*sizeof(BGBGC_ClassMeth **));
		inf->m_mi=i;
	}

	for(i=0; i<inf->n_mi; i++)
		if(inf->mi_class[i]==super)
			return(i);

	if((inf->n_mi+1)>=inf->m_mi)
	{
		i=inf->m_mi+(inf->m_mi>>1);
		inf->mi_class=gcrealloc(inf->mi_class,
			i*sizeof(BGBGC_ClassInfo *));
		vinf->mi_vtab=gcrealloc(vinf->mi_vtab,
			i*sizeof(BGBGC_ClassMeth **));
		inf->m_mi=i;
	}

	i=inf->n_mi++;
	inf->mi_class[i]=super;
	return(i);
}

int BGBDYC_InheritMulti(BGBGC_ClassInfo *inf, BGBGC_ClassInfo *super)
{
	BGBGC_ClassInfo *tinf;
	BGBGC_ClassVinf *vinf, *svinf;
	int id;
	int i, j, k;

	vinf=inf->vinf[inf->ver];
	svinf=super->vinf[super->ver];

	inf->snext=super->first;
	super->first=inf;

	inf->super=NULL;	//no direct superclass

	if(!inf->misup)inf->misup=gcalloc(16*sizeof(BGBGC_ClassInfo *));
	i=inf->n_misup++;
	inf->misup[i]=super;


	id=BGBDYC_IndexClassSuper(inf, super);

	i=svinf->sz_vtab*sizeof(BGBGC_ClassMeth *);
	vinf->mi_vtab[id]=gcalloc(i);
	memcpy(vinf->mi_vtab[id], svinf->vtab, i);

	for(i=0; i<super->n_mi; i++)
	{
		tinf=super->mi_class[i];
		j=BGBDYC_IndexClassSuper(inf, tinf);

		k=tinf->vinf[tinf->ver]->sz_vtab*sizeof(BGBGC_ClassMeth *);
		vinf->mi_vtab[j]=gcalloc(k);
		memcpy(vinf->mi_vtab[j],
			tinf->vinf[tinf->ver]->mi_vtab[i], k);
	}
}
#endif

int BGBDYC_EnumClassSlots(BGBGC_ClassInfo *inf,
	BGBGC_ClassSlot **arr, int num)
{
	BGBGC_ClassSlot *idx;
	int i, j;

	for(i=0; i<inf->n_slot; i++)
	{
		idx=inf->slot[i];
		for(j=0; j<num; j++)if(arr[j]==idx)break;
		if(j<num)continue;
		arr[num++]=idx;
	}

	if(inf->super)
	{
		num=BGBDYC_EnumClassSlots(inf->super, arr, num);
		return(num);
	}

	for(i=0; i<inf->n_misup; i++)
		num=BGBDYC_EnumClassSlots(inf->misup[i], arr, num);
	return(num);
}

int BGBDYC_EnumClassMinf(BGBGC_ClassInfo *inf,
	BGBGC_ClassMinf **arr, int num)
{
	BGBGC_ClassMinf *idx;
	int i, j;

	for(i=0; i<inf->n_minf; i++)
	{
		idx=inf->minf[i];
		for(j=0; j<num; j++)if(arr[j]==idx)break;
		if(j<num)continue;
		arr[num++]=idx;
	}

	if(inf->super)
	{
		num=BGBDYC_EnumClassMinf(inf->super, arr, num);
		return(num);
	}

	for(i=0; i<inf->n_misup; i++)
		num=BGBDYC_EnumClassMinf(inf->misup[i], arr, num);
	return(num);
}

BGBGC_ClassSlot *BGBDYC_InheritMultiSlot(BGBGC_ClassInfo *inf,
	BGBGC_ClassSlot *idx)
{
	BGBGC_ClassSlot *tmp;
	BGBGC_ClassVinf *vinf;
	int i, j, k;

	BGBDYC_CheckAddSlot(inf);

	tmp=gctalloc("_class_slot_t", sizeof(BGBGC_ClassSlot));
	i=inf->n_slot++;
	inf->slot[i]=tmp;
	tmp->num=i;

	tmp->info=inf;
	tmp->name=idx->qname;
	tmp->sig=idx->sig;
	tmp->qname=idx->qname;
	tmp->hash=BGBDYC_CalcHashName(tmp->name);
	tmp->qhash=BGBDYC_CalcHashName(tmp->qname);

	tmp->type=idx->type;
	vinf=inf->vinf[inf->ver];

	switch(idx->type)
	{
	case BGBDYC_SLOT_DIRECT:
	case BGBDYC_SLOT_INDIRECT:
		dyllSigSizeAlign(tmp->sig, &j, &k);
		BGBDYC_AlignClassLength(inf, k);
		tmp->type=BGBDYC_SLOT_DIRECT;
		tmp->offs=vinf->length;
		tmp->size=j;
		vinf->slot_offs[0][tmp->num]=tmp->offs;
		vinf->length+=j;
		break;
	case BGBDYC_SLOT_STATIC:
		tmp->val=idx->val;
		break;
	case BGBDYC_SLOT_INTERFACE: break; //should not happen
	}

	return(tmp);
}

BGBGC_ClassMinf *BGBDYC_InheritMultiMinf(BGBGC_ClassInfo *inf,
	BGBGC_ClassMinf *idx)
{
	char tb[256];
	BGBGC_ClassMinf *minf, *cur;
	BGBGC_ClassVinf *vinf, *svinf;
	BGBGC_ClassMeth *meth;
	BGBGC_ClassSlot *sinf;
	char *s, *t;
	int i, j, hi;

	BGBDYC_CheckAddMinf(inf);

	minf=gctalloc("_class_minf_t", sizeof(BGBGC_ClassMinf));
	minf->info=inf;
	minf->name=idx->qname;
	minf->sig=idx->sig;
	minf->qname=idx->qname;
	minf->hash=BGBDYC_CalcHashNameSig(minf->name, minf->sig);
	minf->qhash=BGBDYC_CalcHashNameSig(minf->qname, minf->sig);
	minf->type=BGBDYC_SLOT_DIRECT;

	i=inf->n_minf++;
	inf->minf[i]=minf;
	minf->num=i;

	minf->type=idx->type;
	vinf=inf->vinf[inf->ver];

	switch(idx->type)
	{
	case BGBDYC_SLOT_DIRECT:
	case BGBDYC_SLOT_INDIRECT:
		minf->idx=vinf->e_minf++;
		vinf->minf_offs[0][minf->num]=minf->idx;
		minf->type=BGBDYC_SLOT_DIRECT;

		svinf=idx->info->vinf[idx->info->ver];
		i=svinf->minf_offs[0][idx->num];

		j=vinf->minf_offs[0][minf->num];
		meth=svinf->vtab[i];
		vinf->vtab[j]=meth;

		break;
	case BGBDYC_SLOT_DYNAMIC:
		sprintf(tb, "%s:__%s%s_",
			idx->info->qname, idx->name, idx->sig);
		sinf=BGBDYC_LookupSlot(inf, tb);
		minf->type=BGBDYC_SLOT_DYNAMIC;
		minf->idx=sinf->num;
		break;
	}

	return(minf);
}

int BGBDYC_InheritMulti(BGBGC_ClassInfo *inf, BGBGC_ClassInfo *super)
{
	void *arr[256];
	BGBGC_ClassVinf *vinf;
	int i, j, k, n;

	inf->is_mi=1;

	if(!inf->misup)inf->misup=gcalloc(16*sizeof(BGBGC_ClassInfo *));
	i=inf->n_misup++;
	inf->misup[i]=super;

	if(inf->is_iface)
		return(0);

	vinf=inf->vinf[inf->ver];
	vinf->slot_offs=gcalloc(2*sizeof(int *));
	vinf->minf_offs=gcalloc(2*sizeof(int *));

	n=BGBDYC_EnumClassSlots(inf, (BGBGC_ClassSlot **)arr, 0);
	for(i=0; i<n; i++)
		BGBDYC_InheritMultiSlot(inf, (BGBGC_ClassSlot *)arr[i]);

	n=BGBDYC_EnumClassMinf(inf, (BGBGC_ClassMinf **)arr, 0);
	for(i=0; i<n; i++)
		BGBDYC_InheritMultiMinf(inf, (BGBGC_ClassMinf *)arr[i]);

	return(0);
}

int BGBDYC_InheritSingle(BGBGC_ClassInfo *inf, BGBGC_ClassInfo *super)
{
	BGBGC_ClassInfo *tinf, *tcur;
	BGBGC_ClassVinf *vinf, *svinf;
	int i, j, k;

	inf->super=super;
	inf->snext=super->first;
	super->first=inf;

	inf->is_mi=super->is_mi;

	vinf=inf->vinf[inf->ver];
	svinf=super->vinf[super->ver];

	tcur=inf; i=0; while(tcur) { i++; tcur=tcur->super; }
	vinf->vers=gcalloc((i+1)*sizeof(short));
	vinf->slot_offs=gcalloc((i+1)*sizeof(int *));
	vinf->minf_offs=gcalloc((i+1)*sizeof(int *));

	vinf->vers[0]=inf->ver;
	tcur=inf->super; i=1; while(tcur)
	{
		vinf->vers[i]=tcur->ver;
		vinf->slot_offs[i]=svinf->slot_offs[i-1];
		vinf->minf_offs[i]=svinf->minf_offs[i-1];
		i++; tcur=tcur->super;
	}

	vinf->start=svinf->length;
	vinf->length=vinf->start;

	vinf->s_minf=svinf->e_minf;
	vinf->e_minf=vinf->s_minf;

	if(svinf->vtab)
	{
		vinf->sz_vtab=svinf->sz_vtab;
		i=vinf->sz_vtab*sizeof(BGBGC_ClassMeth *);
		vinf->vtab=gcalloc(i);
		memcpy(vinf->vtab, svinf->vtab, i);
	}

	if(svinf->idata)
	{
		i=svinf->sz_idata;
		vinf->sz_idata=i;
		vinf->idata=gcalloc(i);
		memcpy(vinf->idata, svinf->idata, i);
	}

#if 0
	for(i=0; i<super->n_mi; i++)
	{
		tinf=super->mi_class[i];
		j=BGBDYC_IndexClassSuper(inf, tinf);
		k=tinf->vinf[tinf->ver]->sz_vtab*sizeof(BGBGC_ClassMeth *);
		vinf->mi_vtab[j]=gcalloc(k);
		memcpy(vinf->mi_vtab[j],
			tinf->vinf[tinf->ver]->mi_vtab[i], k);
	}
#endif

	return(0);
}

int BGBDYC_InheritNone(BGBGC_ClassInfo *inf)
{
	BGBGC_ClassInfo *tinf, *tcur;
	BGBGC_ClassVinf *vinf;
	int i, j, k;

	vinf=inf->vinf[inf->ver];

	vinf->vers=gcalloc(2*sizeof(short));
	vinf->slot_offs=gcalloc(2*sizeof(int *));
	vinf->minf_offs=gcalloc(2*sizeof(int *));

	vinf->vers[0]=inf->ver;

	vinf->start=0;
	vinf->length=0;

	vinf->s_minf=0;
	vinf->e_minf=0;

	return(0);
}

BGBGC_ClassInfo *BGBDYC_CreateClassI(char *qname, char *super, int type)
{
	BGBGC_ClassInfo *tmp, *tinf;
	char **a;
	char *s, *s0;
	int i, j, k;

	BGBDYC_Init();

	tmp=gctalloc("_classinfo_t", sizeof(BGBGC_ClassInfo));

	s=qname+strlen(qname);
	while((s>qname) && (*s!='.') && (*s!='/'))s--;
	if((*s=='.') || (*s=='/'))s++;

	tmp->sname=dystrsym(s);
	tmp->qname=dystrsym(qname);

	BGBDYC_LinkClassNamespace(tmp);

	s=qname; i=0;
	while(*s)i=(i+(*s++))*4093;
	tmp->qhash=i;

	tmp->ver=0;
	tmp->is_iface=(type==BGBDYC_CLASS_INTERFACE);
	tmp->is_struct=(type==BGBDYC_CLASS_STRUCT);

	if(tmp->is_iface)
		tmp->flags|=BGBDYC_FLAG_INTERFACE;
	if(tmp->is_struct)
		tmp->flags|=BGBDYC_FLAG_STRUCT;

	tmp->vinf=gcalloc(16*sizeof(BGBGC_ClassVinf *));
	tmp->vinf[0]=gcalloc(sizeof(BGBGC_ClassVinf));
	tmp->n_vinf=1; tmp->m_vinf=16;
	tmp->vinf[0]->info=tmp;

	if(super)
	{
		a=gcrsplit(super);
		if(a[1])
		{
			for(i=0; a[i]; i++)
			{
				tinf=BGBDYC_FindClassQName(a[i]);
				BGBDYC_InheritMulti(tmp, tinf);
			}
		}else
		{
			tinf=BGBDYC_FindClassQName(super);
			if(tinf)
			{
				BGBDYC_InheritSingle(tmp, tinf);
			}else
			{
				printf("BGBDYC_CreateClassI: No Superclass %s\n", super);
				BGBDYC_InheritNone(tmp);
			}
		}
	}else
	{
		BGBDYC_InheritNone(tmp);
	}

	tmp->next=bgbdyc_fisrt;
	bgbdyc_fisrt=tmp;

	return(tmp);
}

BGBGC_ClassInfo *BGBDYC_CreateClass(char *qname, char *super)
{
	BGBGC_ClassInfo *tmp;
	tmp=BGBDYC_CreateClassI(qname, super, BGBDYC_CLASS_BASIC);
	return(tmp);
}

BGBGC_ClassInfo *BGBDYC_CreateInterface(char *qname, char *super)
{
	BGBGC_ClassInfo *tmp;
	tmp=BGBDYC_CreateClassI(qname, super, BGBDYC_CLASS_INTERFACE);
	return(tmp);
}

BGBGC_ClassInfo *BGBDYC_CreateClassAnon(BGBGC_ClassInfo *super)
{
	BGBGC_ClassInfo *tmp, *tinf;
	char **a;
	char *s;
	int i, j, k;

	BGBDYC_Init();

	tmp=gctalloc("_classinfo_t", sizeof(BGBGC_ClassInfo));

	tmp->qhash=(*(int *)(&tmp))*4093;
	tmp->ver=0;

	tmp->vinf=gcalloc(16*sizeof(BGBGC_ClassVinf *));
	tmp->vinf[0]=gcalloc(sizeof(BGBGC_ClassVinf));
	tmp->vinf[0]->info=tmp;
	tmp->n_vinf=1; tmp->m_vinf=16;

	if(super)BGBDYC_InheritSingle(tmp, super);


	return(tmp);
}

BGBGC_ClassInfo *BGBDYC_CreateClassAnon2(char *super)
{
	BGBGC_ClassInfo *tmp, *tinf;
	if(super)tinf=BGBDYC_FindClassQName(super);
		else tinf=NULL;
	tmp=BGBDYC_CreateClassAnon(tinf);
	return(tmp);
}

BGBGC_ClassInfo *BGBDYC_CreateStruct(char *qname)
{
	BGBGC_ClassInfo *tmp;
	tmp=BGBDYC_CreateClassI(qname, NULL, BGBDYC_CLASS_STRUCT);
	return(tmp);
}

int BGBDYC_AddClassInterface(
	BGBGC_ClassInfo *info, BGBGC_ClassInfo *iface)
{
	int i;

	if(!info->iface)
	{
		info->iface=gcalloc(16*sizeof(BGBGC_ClassInfo *));
		info->n_iface=0; info->m_iface=16;
	}
	
	//don't re-add if exists
	for(i=0; i<info->n_iface; i++)
		if(info->iface[i]==iface)
			return(0);
	
	if((info->n_iface+1)>=info->m_iface)
	{
		i=info->m_iface; i=i+(i>>1);
		info->iface=gcrealloc(info->iface,
			i*sizeof(BGBGC_ClassInfo *));
		info->m_iface=i;
	}
	
	i=info->n_iface++;
	info->iface[i]=iface;
	return(0);
}

int BGBDYC_AlignClassLength(
	BGBGC_ClassInfo *info, int align)
{
	int i;

	i=info->vinf[info->ver]->length;
	if(i%align)
		info->vinf[info->ver]->length=i+(align-(i%align));
	return(0);
}

int BGBDYC_CheckAddSlot(BGBGC_ClassInfo *inf)
{
	BGBGC_ClassVinf *vinf;
	int i;

	vinf=inf->vinf[inf->ver];

	if(!inf->slot)
	{
		inf->slot=gcalloc(64*sizeof(BGBGC_ClassSlot *));
		vinf->slot_offs[0]=gcalloc(64*sizeof(int));
		inf->m_slot=64;
	}

	if((inf->n_slot+1)>=inf->m_slot)
	{
		i=inf->m_slot+(inf->m_slot>>1);
		inf->slot=gcrealloc(inf->slot, i*sizeof(BGBGC_ClassSlot *));
		vinf->slot_offs[0]=gcrealloc(vinf->slot_offs[0], i*sizeof(int));
		inf->m_slot=i;
	}

	return(0);
}

int BGBDYC_CheckAddMinf(BGBGC_ClassInfo *inf)
{
	BGBGC_ClassVinf *vinf;
	int i;

	vinf=inf->vinf[inf->ver];

	if(!inf->minf)
	{
		inf->minf=gcalloc(64*sizeof(BGBGC_ClassSlot *));
		vinf->minf_offs[0]=gcalloc(64*sizeof(int));
		inf->m_minf=64;
	}

	if((inf->n_minf+1)>=inf->m_minf)
	{
		i=inf->m_minf+(inf->m_minf>>1);
		inf->minf=gcrealloc(inf->minf, i*sizeof(BGBGC_ClassMinf *));
		vinf->minf_offs[0]=gcrealloc(vinf->minf_offs[0], i*sizeof(int));
		inf->m_minf=i;
	}


	if(!vinf->vtab)
	{
		vinf->vtab=gcalloc(64*sizeof(BGBGC_ClassMeth *));
		vinf->sz_vtab=64;
	}

	if((vinf->e_minf+1)>=vinf->sz_vtab)
	{
		i=vinf->sz_vtab+(vinf->sz_vtab>>1);
		vinf->vtab=gcrealloc(vinf->vtab, i*sizeof(BGBGC_ClassMeth *));
		vinf->sz_vtab=i;
	}

	return(0);
}


BGBGC_ClassSlot *BGBDYC_AddSlotAbstract(
	BGBGC_ClassInfo *inf, char *name, char *sig)
{
	char tb[256];
	BGBGC_ClassSlot *tmp;
	int i, j, k;

	BGBDYC_CheckAddSlot(inf);

	tmp=gctalloc("_class_slot_t", sizeof(BGBGC_ClassSlot));
	i=inf->n_slot++;
	inf->slot[i]=tmp;
	tmp->num=i;

	tmp->info=inf;
	tmp->name=dystrsym(name);
	tmp->sig=dystrsym(sig);
	tmp->hash=BGBDYC_CalcHashName(name);

	sprintf(tb, "%s:%s", inf->qname, name);
	tmp->qname=dystrsym(tb);
	tmp->qhash=BGBDYC_CalcHashName(tb);

	return(tmp);
}

BGBGC_ClassSlot *BGBDYC_AddSlot(
	BGBGC_ClassInfo *inf, char *name, char *sig)
{
	BGBGC_ClassSlot *tmp;
	BGBGC_ClassVinf *vinf;
	int i, j, k;

	if(inf->is_iface)
	{
		tmp=BGBDYC_AddSlotIface(inf, name, sig);
		return(tmp);
	}

	vinf=inf->vinf[inf->ver];

	tmp=BGBDYC_AddSlotAbstract(inf, name, sig);
	dyllSigSizeAlign(sig, &j, &k);
	BGBDYC_AlignClassLength(inf, k);
	if(inf->super && !tmp->num)
		vinf->start=vinf->length;
	tmp->type=BGBDYC_SLOT_DIRECT;
	tmp->offs=vinf->length;
	tmp->size=j;

	vinf->slot_offs[0][tmp->num]=tmp->offs;
	vinf->length+=j;

	return(tmp);
}

BGBGC_ClassSlot *BGBDYC_AddSlotInit(
	BGBGC_ClassInfo *inf, char *name, char *sig)
{
	BGBGC_ClassSlot *tmp;

	tmp=BGBDYC_AddSlot(inf, name, sig);
	BGBDYC_CheckInitData(inf);

	return(tmp);
}

BGBGC_ClassSlot *BGBDYC_AddSlotStatic(
	BGBGC_ClassInfo *inf, char *name, char *sig)
{
	BGBGC_ClassSlot *tmp;
	int i, j, k;

	tmp=BGBDYC_AddSlotAbstract(inf, name, sig);
	dyllSigSizeAlign(sig, &j, &k);
	tmp->type=BGBDYC_SLOT_STATIC;

	if(j<=(4*sizeof(void *)))tmp->val=&(tmp->val_);
		else tmp->val=gcalloc(j);

	return(tmp);
}

BGBGC_ClassSlot *BGBDYC_AddSlotDelegate(
	BGBGC_ClassInfo *inf, char *name, char *cname)
{
	char tb[64];
	BGBGC_ClassSlot *tmp;

	sprintf(tb, "__DI:%s", name);
	tmp=BGBDYC_AddSlot(inf, tb, "r");
	tmp->flags|=BGBDYC_FLAG_DELEGATE;

	return(tmp);
}

BGBGC_ClassSlot *BGBDYC_AddSlotStaticDelegate(
	BGBGC_ClassInfo *inf, char *name, char *cname)
{
	BGBGC_ClassSlot *tmp;

	tmp=BGBDYC_AddSlotStatic(inf, name, "r");
	tmp->flags|=BGBDYC_FLAG_DELEGATE;

	return(tmp);
}

BGBGC_ClassSlot *BGBDYC_AddSlotIface(
	BGBGC_ClassInfo *inf, char *name, char *sig)
{
	BGBGC_ClassSlot *tmp;

	tmp=BGBDYC_AddSlotAbstract(inf, name, sig);
	tmp->type=BGBDYC_SLOT_INTERFACE;
	tmp->offs=-1;

	return(tmp);
}

BGBGC_ClassMinf *BGBDYC_AddMethodSig(
	BGBGC_ClassInfo *inf, char *name, char *sig)
{
	char tb[256];
	BGBGC_ClassMinf *minf, *cur;
	BGBGC_ClassMeth *meth;
	char *s, *t;
	int i, hi;

//	if(inf->is_iface)
//	{
//		minf=BGBDYC_GetIfaceMethodSig(inf, name, sig);
//		return(minf);
//	}

	BGBDYC_CheckAddMinf(inf);

//	if(!strcmp(name, "<init>"))
//		name=inf->sname;

	minf=gctalloc("_class_minf_t", sizeof(BGBGC_ClassMinf));
	minf->info=inf;
	minf->name=dystrsym(name);
	minf->sig=dystrsym(sig);
	minf->hash=BGBDYC_CalcHashNameSig(name, sig);
	minf->type=BGBDYC_SLOT_DIRECT;

	sprintf(tb, "%s:%s", inf->qname, name);
	minf->qname=dystrsym(tb);
	minf->qhash=BGBDYC_CalcHashNameSig(tb, sig);

	i=inf->n_minf++;
	inf->minf[i]=minf;
	minf->num=i;

	return(minf);
}

BGBGC_ClassMinf *BGBDYC_AddSlotMethodSig(
	BGBGC_ClassInfo *inf, char *name, char *sig)
{
	char tb[256];
	BGBGC_ClassMinf *minf, *cur;
	BGBGC_ClassSlot *sinf;
	char *s, *t;
	int i, hi;

//	minf=BGBDYC_LookupMethodSig(inf, name, sig);
//	if(minf)return(minf);

	sprintf(tb, "__%s%s_", name, sig);
	sinf=BGBDYC_AddSlot(inf, tb, "r");

	minf=BGBDYC_AddMethodSig(inf, name, sig);
	minf->type=BGBDYC_SLOT_DYNAMIC;
	minf->idx=sinf->num;

	return(minf);
}

BGBGC_ClassMinf *BGBDYC_AddStaticSlotMethodSig(
	BGBGC_ClassInfo *inf, char *name, char *sig)
{
	char tb[256];
	BGBGC_ClassMinf *minf, *cur;
	BGBGC_ClassSlot *sinf;
	char *s, *t;
	int i, hi;

//	minf=BGBDYC_LookupMethodSig(inf, name, sig);
//	if(minf)return(minf);

	sprintf(tb, "__%s%s_", name, sig);
	sinf=BGBDYC_AddSlotStatic(inf, tb, "r");

	minf=BGBDYC_AddMethodSig(inf, name, sig);
	minf->type=BGBDYC_SLOT_DYNAMIC;
	minf->idx=sinf->num;

	return(minf);
}

int BGBDYC_CheckInitData(BGBGC_ClassInfo *inf)
{
	BGBGC_ClassVinf *vinf;
	int i;

	vinf=inf->vinf[inf->ver];

	if(!vinf->idata)
	{
		i=256;
		while(vinf->length>=i)i+=i>>1;
		vinf->idata=gcalloc(i);
		vinf->sz_idata=i;
	}

	if(vinf->length>=vinf->sz_idata)
	{
		i=vinf->sz_idata;
		while(vinf->length>i)i+=i>>1;
		vinf->idata=gcrealloc(vinf->idata, i);
		vinf->sz_idata=i;
	}
	return(0);
}

int BGBDYC_TouchMinfMethod(BGBGC_ClassInfo *inf, BGBGC_ClassMinf *minf)
{
	BGBGC_ClassMeth *meth;
	BGBGC_ClassVinf *vinf;
	int *tab;
	int i, id;

	if(minf->type==BGBDYC_SLOT_INTERFACE)return(0);
	if(minf->type==BGBDYC_SLOT_DYNAMIC)return(0);

//	BGBDYC_CheckAddMinf(inf);

	vinf=inf->vinf[inf->ver];
//	id=vinf->minf_offs[0][minf->num];
	tab=BGBDYC_GetClassMinfOffsTab(inf, minf);
	id=tab[minf->num];

	if(!vinf->vtab[id])
	{
		meth=gctalloc("_class_meth_t", sizeof(BGBGC_ClassMeth));
		meth->info=inf;
		vinf->vtab[id]=meth;
		return(1);
	}

	if(vinf->vtab[id]->info!=inf)
	{
		meth=gctalloc("_class_meth_t", sizeof(BGBGC_ClassMeth));
		meth->info=inf;
		meth->fcn=vinf->vtab[id]->fcn;
		vinf->vtab[id]=meth;
		return(1);
	}

	return(0);
}

BGBGC_ClassBase *BGBDYC_CreateInstance(BGBGC_ClassInfo *inf)
{
	BGBGC_ClassBase *tmp;
	BGBGC_ClassVinf *vinf;
	int i;

	vinf=inf->vinf[inf->ver];

	tmp=gctalloc("_classobj_t", sizeof(BGBGC_ClassBase));
	tmp->info=inf; tmp->vinf=vinf;
	tmp->data=gcalloc(vinf->length);
//	tmp->vers=vinf->vers;

	if(vinf->idata)
	{
		i=(vinf->sz_idata<vinf->length)?
			vinf->sz_idata:vinf->length;
		memcpy(tmp->data, vinf->idata, i);
	}

	return(tmp);
}

BGBGC_ObjProto *BGBDYC_CreateInstanceProto(BGBGC_ClassInfo *inf)
{
	BGBGC_ObjProto *tmp1;
	BGBGC_ClassBase *tmp;
	BGBGC_ClassVinf *vinf;
	int i;

	tmp1=BGBDYC_AllocProtoObj();
	tmp=gctalloc("_classobj_t", sizeof(BGBGC_ClassBase));

	vinf=inf->vinf[inf->ver];

	tmp->info=inf; tmp->vinf=vinf;
	tmp->data=gcalloc(vinf->length);
//	tmp->vers=vinf->vers;
	tmp->proto=tmp1;

	if(vinf->idata)
	{
		i=(vinf->sz_idata<vinf->length)?
			vinf->sz_idata:vinf->length;
		memcpy(tmp->data, vinf->idata, i);
	}

	return(tmp1);
}

BGBGC_ClassBase *BGBDYC_CloneInstance(BGBGC_ClassBase *obj)
{
	BGBGC_ClassBase *tmp;
	BGBGC_ClassVinf *vinf;
//	dycMethod minf;
	int i;

//	vinf=obj->info->vinf[obj->vers[0]];
	vinf=obj->vinf;

	tmp=gctalloc("_classobj_t", sizeof(BGBGC_ClassBase));
	tmp->info=obj->info; tmp->vinf=obj->vinf;
	tmp->data=gcalloc(vinf->length);
//	tmp->vers=obj->vers;

	memcpy(tmp->data, obj->data, vinf->length);
	if(obj->proto)tmp->proto=BGBDYC_CloneProtoObj(obj->proto);

	return(tmp);
}

void BGBDYC_FreeInstance(BGBGC_ClassBase *obj)
{
	if(!obj)return;

	if(obj->proto)BGBDYC_FreeProtoObj(obj->proto);
	if(obj->data)gcfree(obj->data);
	gcfree(obj);
}

void BGBDYC_SetInstance(BGBGC_ClassBase *dst, BGBGC_ClassBase *src)
{
	BGBGC_ClassBase *tmp;
	BGBGC_ClassVinf *vinf;
	int i;

	if(dst->vinf != src->vinf)
	{
		//BAD: this is only valid between objects with the same layout
		return;
	}

	vinf=dst->vinf;
	memcpy(dst->data, src->data, vinf->length);

//	if(obj->proto)tmp->proto=BGBDYC_CloneProtoObj(obj->proto);

	return;
}

int BGBDYC_SubclassP(BGBGC_ClassInfo *info, BGBGC_ClassInfo *super)
{
	static dycClass hasha[256], hashb[256];
	static char hashc[256];
	BGBGC_ClassInfo *cur;
	int i;

	if(info==super)return(1);

	i=((super->qhash*4093+info->qhash)>>12)&255;
	if((hasha[i]==info) && (hashb[i]==super))
		return(hashc[i]);

	cur=info;
	while(cur)
	{
		if(cur==super)
		{
			hasha[i]=info; hashb[i]=super; hashc[i]=1;
			return(1);
		}
		cur=cur->super;
	}
	hasha[i]=info; hashb[i]=super; hashc[i]=0;
	return(0);
}

int BGBDYC_SubclassOrImplP(BGBGC_ClassInfo *info, BGBGC_ClassInfo *super)
{
	static dycClass hasha[256], hashb[256];
	static char hashc[256];
	BGBGC_ClassInfo *cur;
	int i, j;

	if(info==super)return(1);

	i=((super->qhash*4093+info->qhash)>>12)&255;
	if((hasha[i]==info) && (hashb[i]==super))
		return(hashc[i]);

	cur=info;
	while(cur)
	{
		if(cur==super)
		{
			hasha[i]=info; hashb[i]=super; hashc[i]=1;
			return(1);
		}

		for(j=0; j<cur->n_iface; j++)
		{
			if(cur->iface[j]==super)
			{
				hasha[i]=info; hashb[i]=super; hashc[i]=1;
				return(1);
			}
		}

		cur=cur->super;
	}
	hasha[i]=info; hashb[i]=super; hashc[i]=0;
	return(0);
}

int BGBDYC_InstanceP(BGBGC_ClassInfo *info, BGBGC_ClassInfo *super)
{
	if(!info || !super)return(0);
	
	if(super->is_iface)
	{
		if(info->is_iface)
			return(BGBDYC_SubclassP(info, super));
		return(BGBDYC_SubclassOrImplP(info, super));
	}
	
	return(BGBDYC_SubclassP(info, super));
}

#if 0
void *BGBDYC_GetObjectSlotIdxPtr_MI(
	BGBGC_ClassBase *obj, BGBGC_ClassSlot *idx)
{
	dycClass infa, infb;
	int i;

	if(BGBDYC_SubclassP(obj->info, idx->info))
		return((void *)(obj->data+idx->offs));

	infa=obj->info; infb=idx->info;
	for(i=0; infa->n_mi; i++)
	{
		if(BGBDYC_SubclassP(infa->mi_class[i], infb))
			return((void *)(obj->mi_data[i]+idx->offs));
	}
	return(NULL);
}
#endif

BGBGC_ClassVinf *BGBDYC_GetClassSlotVInfo(
	BGBGC_ClassInfo *info, BGBGC_ClassSlot *idx)
{
	BGBGC_ClassInfo *inf, *cur;
	BGBGC_ClassVinf *vinf;
	int i;

	vinf=info->vinf[info->ver];

	inf=idx->info; cur=info; i=0;
	while(cur)
	{
		if(cur==inf)return(cur->vinf[vinf->vers[i]]);
		cur=cur->super; i++;
	}

	return(NULL);
}

BGBGC_ClassVinf *BGBDYC_GetObjectSlotVInfo(
	BGBGC_ClassBase *obj, BGBGC_ClassSlot *idx)
{
	BGBGC_ClassInfo *inf, *cur;
	BGBGC_ClassVinf *vinf;
	short *vers;
	int i;

	vers=obj->vinf->vers;
	inf=idx->info; cur=obj->info; i=0;
	while(cur)
	{
		if(cur==inf)return(cur->vinf[vers[i]]);
		cur=cur->super; i++;
	}

	return(NULL);
}

int BGBDYC_GetClassSlotVIdx(
	BGBGC_ClassInfo *info, BGBGC_ClassSlot *idx)
{
	BGBGC_ClassInfo *inf, *cur;
	BGBGC_ClassVinf *vinf;
	int i;

	vinf=info->vinf[info->ver];

	inf=idx->info; cur=info; i=0;
	while(cur)
	{
		if(cur==inf)return(i);
		cur=cur->super; i++;
	}

	return(-1);
}

int BGBDYC_GetObjectSlotVIdx(
	BGBGC_ClassBase *obj, BGBGC_ClassSlot *idx)
{
	BGBGC_ClassInfo *inf, *cur;
	BGBGC_ClassVinf *vinf;
	int i;

	inf=idx->info; cur=obj->info; i=0;
	while(cur)
	{
		if(cur==inf)return(i);
		cur=cur->super; i++;
	}

	return(-1);
}

int *BGBDYC_GetClassSlotVOffsTab(
	BGBGC_ClassInfo *info, BGBGC_ClassSlot *idx)
{
	BGBGC_ClassInfo *inf, *cur;
	BGBGC_ClassVinf *vinf;
	int i;

	vinf=info->vinf[info->ver];

	inf=idx->info; cur=info; i=0;
	while(cur)
	{
		if(cur==inf)return(vinf->slot_offs[i]);
		cur=cur->super; i++;
	}

	return(NULL);
}

int *BGBDYC_GetObjectSlotVOffsTab(
	BGBGC_ClassBase *obj, BGBGC_ClassSlot *idx)
{
	BGBGC_ClassInfo *inf, *cur;
//	BGBGC_ClassVinf *vinf;
	int i;

	inf=idx->info; cur=obj->info; i=0;
	while(cur)
	{
		if(cur==inf)return(obj->vinf->slot_offs[i]);
		cur=cur->super; i++;
	}

	return(NULL);
}

void *BGBDYC_GetObjectSlotIdxPtr_MI(
	BGBGC_ClassBase *obj, BGBGC_ClassSlot *idx)
{
//	BGBGC_ClassVinf *vinf;
	int *tab;
	int i;

	switch(idx->type)
	{
	case BGBDYC_SLOT_STATIC:
		return(idx->val);
	case BGBDYC_SLOT_INTERFACE:
//		idx=BGBDYC_LookupObjectSlotIface(obj, idx);
//		if(!idx)return(NULL);
//		return(BGBDYC_GetObjectSlotIdxPtr(obj, idx));
		return(BGBDYC_GetObjectSlotIdxPtr_DI(obj, idx));
	case BGBDYC_SLOT_PROTOTYPE:
		return(BGBDYC_GetProtoSlotIdxPtr(obj->proto, idx));
	default: break;
	}

	if(BGBDYC_SubclassP(obj->info, idx->info))
	{
		switch(idx->type)
		{
		case BGBDYC_SLOT_DIRECT:
			return((void *)(obj->data+idx->offs));
		case BGBDYC_SLOT_INDIRECT:
//			vinf=BGBDYC_GetObjectSlotVInfo(obj, idx);
//			return((void *)(obj->data+vinf->slot_offs[0][idx->num]));

			tab=BGBDYC_GetObjectSlotVOffsTab(obj, idx);
			return((void *)(obj->data+tab[idx->num]));
		default: break;
		}
		return(NULL);
	}

	idx=BGBDYC_LookupSlotR(obj->info, idx->qname, idx->qhash);
	if(!idx)return(NULL);

	switch(idx->type)
	{
	case BGBDYC_SLOT_DIRECT:
		return((void *)(obj->data+idx->offs));
	case BGBDYC_SLOT_INDIRECT:
//		vinf=BGBDYC_GetObjectSlotVInfo(obj, idx);
//		return((void *)(obj->data+vinf->slot_offs[0][idx->num]));

		tab=BGBDYC_GetObjectSlotVOffsTab(obj, idx);
		return((void *)(obj->data+tab[idx->num]));
	default: break;
	}

	return(NULL);
}

void *BGBDYC_GetInitIdxPtr_MI(
	BGBGC_ClassInfo *inf, BGBGC_ClassSlot *idx)
{
	BGBGC_ClassVinf *vinf;
	int *tab;
	int i;

	switch(idx->type)
	{
	case BGBDYC_SLOT_STATIC:
		return(idx->val);
	case BGBDYC_SLOT_INTERFACE:
		idx=BGBDYC_LookupSlotIface(inf, idx);
		if(!idx)return(NULL);
//		if(!idx)return(BGBDYC_GetObjectSlotIdxPtr_DI(obj, idx));
		return(BGBDYC_GetInitIdxPtr(inf, idx));
	default: break;
	}

	if(BGBDYC_SubclassP(inf, idx->info))
	{
		switch(idx->type)
		{
		case BGBDYC_SLOT_DIRECT:
//			BGBDYC_CheckInitData(inf);
//			vinf=BGBDYC_GetClassSlotVInfo(inf, idx);
//			return((void *)(vinf->idata+idx->offs));
		case BGBDYC_SLOT_INDIRECT:
//			BGBDYC_CheckInitData(inf);
//			vinf=BGBDYC_GetClassSlotVInfo(inf, idx);
//			return((void *)(vinf->idata+vinf->slot_offs[0][idx->num]));

			BGBDYC_CheckInitData(inf);
			vinf=inf->vinf[inf->ver];
			tab=BGBDYC_GetClassSlotVOffsTab(inf, idx);
			return((void *)(((byte *)vinf->idata)+tab[idx->num]));
		default: break;
		}
		return(NULL);
	}

	idx=BGBDYC_LookupSlotR(inf, idx->qname, idx->qhash);
	if(!idx)return(NULL);

	switch(idx->type)
	{
	case BGBDYC_SLOT_DIRECT:
//		BGBDYC_CheckInitData(inf);
//		vinf=BGBDYC_GetClassSlotVInfo(inf, idx);
//		return((void *)(vinf->idata+idx->offs));
	case BGBDYC_SLOT_INDIRECT:
//		BGBDYC_CheckInitData(inf);
//		vinf=BGBDYC_GetClassSlotVInfo(inf, idx);
//		return((void *)(vinf->idata+vinf->slot_offs[0][idx->num]));

		BGBDYC_CheckInitData(inf);
		vinf=inf->vinf[inf->ver];
		tab=BGBDYC_GetClassSlotVOffsTab(inf, idx);
		return((void *)(((byte *)vinf->idata)+tab[idx->num]));
	default: break;
	}

	return(NULL);
}

void *BGBDYC_GetObjectSlotIdxPtr(
	BGBGC_ClassBase *obj, BGBGC_ClassSlot *idx)
{
	BGBGC_ClassInfo *inf;
//	BGBGC_ClassVinf *vinf;
	int *tab;

	if(obj->info->is_mi)
		return(BGBDYC_GetObjectSlotIdxPtr_MI(obj, idx));

	switch(idx->type)
	{
	case BGBDYC_SLOT_DIRECT:
//		if(obj->info->n_mi)
//		if(obj->mi_data)
//			return(BGBDYC_GetObjectSlotIdxPtr_MI(obj, idx));
		return((void *)(obj->data+idx->offs));
	case BGBDYC_SLOT_INDIRECT:
//		vinf=BGBDYC_GetObjectSlotVInfo(obj, idx);
//		return((void *)(obj->data+vinf->slot_offs[0][idx->num]));
		tab=BGBDYC_GetObjectSlotVOffsTab(obj, idx);
		return((void *)(obj->data+tab[idx->num]));
	case BGBDYC_SLOT_STATIC:
		return(idx->val);
	case BGBDYC_SLOT_INTERFACE:
//		idx=BGBDYC_LookupObjectSlotIface(obj, idx);
//		if(!idx)return(NULL);
//		if(!idx)return(BGBDYC_GetObjectSlotIdxPtr_DI(obj, idx));
//		return(BGBDYC_GetObjectSlotIdxPtr(obj, idx));
		return(BGBDYC_GetObjectSlotIdxPtr_DI(obj, idx));
	case BGBDYC_SLOT_PROTOTYPE:
		return(BGBDYC_GetProtoSlotIdxPtr(obj->proto, idx));
	default: break;
	}

	return(NULL);
}

void *BGBDYC_GetInitIdxPtr(
	BGBGC_ClassInfo *inf, BGBGC_ClassSlot *idx)
{
	BGBGC_ClassVinf *vinf;
	int *tab;

	if(inf->is_mi)
		return(BGBDYC_GetInitIdxPtr_MI(inf, idx));

	switch(idx->type)
	{
	case BGBDYC_SLOT_DIRECT:
//		BGBDYC_CheckInitData(inf);
//		vinf=inf->vinf[inf->ver];
//		return((void *)(vinf->idata+idx->offs));
	case BGBDYC_SLOT_INDIRECT:
		BGBDYC_CheckInitData(inf);
//		vinf=BGBDYC_GetClassSlotVInfo(inf, idx);
//		return((void *)(vinf->idata+vinf->slot_offs[0][idx->num]));

		vinf=inf->vinf[inf->ver];
		tab=BGBDYC_GetClassSlotVOffsTab(inf, idx);
		return((void *)(((byte *)vinf->idata)+tab[idx->num]));
	case BGBDYC_SLOT_STATIC:
		return(idx->val);
	case BGBDYC_SLOT_INTERFACE:
		idx=BGBDYC_LookupSlotIface(inf, idx);
		if(!idx)return(NULL);
		return(BGBDYC_GetInitIdxPtr(inf, idx));
	default: break;
	}

	return(NULL);
}


void *BGBDYC_GetObjectSlotIdxPtrTH(
	BGBGC_ClassBase *obj, BGBGC_ClassSlot *idx)
{
	BGBGC_ClassInfo *inf;
//	BGBGC_ClassVinf *vinf;
	int *tab;
	void *p;

	if(obj->info->is_mi)
	{
		thLockFastMutex(bgbdyc_mutex);
		p=BGBDYC_GetObjectSlotIdxPtr_MI(obj, idx);
		thUnlockFastMutex(bgbdyc_mutex);
		return(p);
	}

	switch(idx->type)
	{
	case BGBDYC_SLOT_DIRECT:
//		if(obj->info->n_mi)
//		if(obj->mi_data)
//			return(BGBDYC_GetObjectSlotIdxPtr_MI(obj, idx));
		return((void *)(obj->data+idx->offs));
	case BGBDYC_SLOT_INDIRECT:
		thLockFastMutex(bgbdyc_mutex);
//		vinf=BGBDYC_GetObjectSlotVInfo(obj, idx);
//		p=(void *)(obj->data+vinf->slot_offs[0][idx->num]);
		tab=BGBDYC_GetObjectSlotVOffsTab(obj, idx);
		p=(void *)(obj->data+tab[idx->num]);
		thUnlockFastMutex(bgbdyc_mutex);
		return(p);
	case BGBDYC_SLOT_STATIC:
		return(idx->val);
	case BGBDYC_SLOT_INTERFACE:
		thLockFastMutex(bgbdyc_mutex);
#if 0
		idx=BGBDYC_LookupObjectSlotIface(obj, idx);
//		if(!idx) { thUnlockFastMutex(bgbdyc_mutex); return(NULL); }
		if(!idx)
		{
			p=BGBDYC_GetObjectSlotIdxPtr_DI(obj, idx);
			thUnlockFastMutex(bgbdyc_mutex);
			return(p);
		}
		p=BGBDYC_GetObjectSlotIdxPtr(obj, idx);
#endif

		p=BGBDYC_GetObjectSlotIdxPtr_DI(obj, idx);
		thUnlockFastMutex(bgbdyc_mutex);
		return(p);
	case BGBDYC_SLOT_PROTOTYPE:
		thLockFastMutex(bgbdyc_mutex);
		p=BGBDYC_GetProtoSlotIdxPtr(obj->proto, idx);
		thUnlockFastMutex(bgbdyc_mutex);
		return(p);
	default: break;
	}

	return(NULL);
}

void *BGBDYC_GetInitIdxPtrTH(
	BGBGC_ClassInfo *inf, BGBGC_ClassSlot *idx)
{
	BGBGC_ClassVinf *vinf;
	int *tab;
	void *p;

	if(inf->is_mi)
	{
		thLockFastMutex(bgbdyc_mutex);
		p=BGBDYC_GetInitIdxPtr_MI(inf, idx);
		thUnlockFastMutex(bgbdyc_mutex);
		return(p);
	}

	switch(idx->type)
	{
	case BGBDYC_SLOT_DIRECT:
//		thLockFastMutex(bgbdyc_mutex);
//		BGBDYC_CheckInitData(inf);
//		vinf=inf->vinf[inf->ver];
//		p=(void *)(vinf->idata+idx->offs);
//		thUnlockFastMutex(bgbdyc_mutex);
//		return(p);
	case BGBDYC_SLOT_INDIRECT:
		thLockFastMutex(bgbdyc_mutex);
		BGBDYC_CheckInitData(inf);
//		vinf=BGBDYC_GetClassSlotVInfo(inf, idx);
//		p=(void *)(vinf->idata+vinf->slot_offs[0][idx->num]);

		vinf=inf->vinf[inf->ver];
		tab=BGBDYC_GetClassSlotVOffsTab(inf, idx);
		p=(void *)(((byte *)vinf->idata)+tab[idx->num]);

		thUnlockFastMutex(bgbdyc_mutex);
		return(p);
	case BGBDYC_SLOT_STATIC:
		return(idx->val);
	case BGBDYC_SLOT_INTERFACE:
		thLockFastMutex(bgbdyc_mutex);
		idx=BGBDYC_LookupSlotIface(inf, idx);
		if(!idx) { thUnlockFastMutex(bgbdyc_mutex); return(NULL); }
		p=BGBDYC_GetInitIdxPtr(inf, idx);
		thUnlockFastMutex(bgbdyc_mutex);
		return(p);
	default: break;
	}

	return(NULL);
}

