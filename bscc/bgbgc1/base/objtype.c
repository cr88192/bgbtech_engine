#include <bgbgc.h>

#ifdef MSVC
#include <windows.h>
#endif


char *(*bgbgc_typehook_f[1024])(void *p);
int bgbgc_ntypehook;

BGBGC_ObjType *bgbdy_objtypes=NULL;
BGBGC_ObjType *bgbdy_objtype_hash[16384];

BGBGC_SourceInfo *bgbdy_srcinfo_hash[65536];

#if 1
static void *bgbgc_objtype_lastptr[256];
static BGBGC_ObjType *bgbgc_objtype_lasttype[256];
static int bgbgc_objtype_laststat=0;

void BGBGC_ObjType_FlushTypeCache()
{
	int i;

	if(!bgbgc_objtype_laststat)
		return;

	for(i=0; i<256; i++)
	{
		bgbgc_objtype_lastptr[i]=NULL;
		bgbgc_objtype_lasttype[i]=NULL;
	}
	bgbgc_objtype_laststat=0;
}

BGBGC_ObjType *BGBGC_ObjType_CheckPtrTypeCache(void *p)
{
	int i;

	i=(nlint)p;
	i=((i*65521)>>16)&0xFF;
	if(p && (p==bgbgc_objtype_lastptr[i]))
		return(bgbgc_objtype_lasttype[i]);
	return(NULL);
}

void BGBGC_ObjType_SetPtrTypeCache(void *p, BGBGC_ObjType *ty)
{
	int i;

	i=(nlint)p;
	i=((i*65521)>>16)&0xFF;
	bgbgc_objtype_lastptr[i]=p;
	bgbgc_objtype_lasttype[i]=ty;
}
#endif

BGBGC_ObjType *BGBGC_NewType(char *name, char *form)
{
	BGBGC_ObjType *tmp, *tmp2;
	int n;

	n=1;
	tmp=BGBGC_FindType(name);
	if(tmp)
	{
		if(strcmp(form, tmp->form) && tmp->form[0])
		{
			printf("BGBGC_NewType: mismatched redefinition "
				"of type '%s'.\n", name);
			return(NULL);
		}
		n=0;
	}else
	{
		tmp=malloc(sizeof(BGBGC_ObjType));
		memset(tmp, 0, sizeof(BGBGC_ObjType));
	}

//	tmp->name=strdup(name);
//	tmp->form=strdup(form);
	tmp->name=bgbgc_strdup(name);
	tmp->form=bgbgc_strdup(form);
	tmp->cnt=0;
	tmp->cntsz=0;

	if(n)
	{
		tmp->next=bgbdy_objtypes;
		bgbdy_objtypes=tmp;
	}

	return(tmp);
}

BGBGC_ObjType *BGBGC_FindTypeHash(char *type)
{
	char *s;
	int i, j, k;

	s=type; i=0;
	while(*s)i=(i*4093)+(*s++);
	i*=4093;

	for(j=0; j<64; j++)
	{
		k=(i>>12)&16383;
		if(!bgbdy_objtype_hash[k])break;
		if(!strcmp(bgbdy_objtype_hash[k]->name, type))
			return(bgbdy_objtype_hash[k]);
		i*=4093;
	}

	return(NULL);
}

void BGBGC_AddTypeHash(BGBGC_ObjType *type)
{
	char *s;
	int i, j, k;

	s=type->name; i=0;
	while(*s)i=(i*4093)+(*s++);
	i*=4093;

	for(j=0; j<64; j++)
	{
		k=(i>>12)&16383;
		if(!bgbdy_objtype_hash[k])
			{ bgbdy_objtype_hash[k]=type; type->idx=k; break; }
		if(bgbdy_objtype_hash[k]==type) break;
		i*=4093;
	}
}

BGBGC_ObjType *BGBGC_FindType(char *type)
{
	BGBGC_ObjType *cur;
	char *s;
	int i, j, k;

	BGBGC_InitTY();

	if(!type)return(NULL);

	cur=BGBGC_FindTypeHash(type);
	if(cur)return(cur);

	cur=bgbdy_objtypes;
	while(cur)
	{
		if(cur->name)if(!strcmp(type, cur->name))
		{
			BGBGC_AddTypeHash(cur);
			return(cur);
		}
		cur=cur->next;
	}

	return(NULL);
}

BGBGC_ObjType *BGBGC_FetchType(char *type)
{
	BGBGC_ObjType *ty;

	if(!type)return(NULL);

	ty=BGBGC_FindType(type);
	if(ty)return(ty);

	ty=BGBGC_NewType(type, "");
	BGBGC_AddTypeHash(ty);
	return(ty);
}

int BGBGC_IndexType(char *name)
{
	BGBGC_ObjType *ty;
	if(!name)return(-1);
	ty=BGBGC_FetchType(name);
	return(ty->idx);
}

BGBGC_ObjType *BGBGC_FetchIndexType(int idx)
{
	if(idx<0)return(NULL);
	if(idx>=16384)return(NULL);
	return(bgbdy_objtype_hash[idx]);
}

BGBGC_SourceInfo *BGBGC_FetchIndexSourceInfo(int idx)
{
	if(idx<0)return(NULL);
	if(idx>=65536)return(NULL);
	return(bgbdy_srcinfo_hash[idx]);
}

char *BGBGC_InternType(char *name)
{
	BGBGC_ObjType *ty;
	if(!name)return(NULL);
	ty=BGBGC_FetchType(name);
	return(ty->name);
}

void *BGBGC_New(char *type, int size)
{
	BGBGC_ObjType *ty;
	BGBGC_ObjInfo *inf;
	void *tmp;
	int sz;

	BGBGC_InitTY();

	if(!type)return(NULL);
	if(size<0)return(NULL);

	if(BGBGC_CheckLogAlloc())
	{
		printf("BGBGC_New: %s %d\n", type, size);
	}

//#ifdef BGBGC_USECONS
#if 0
	if(!strcmp(type, "_cons_t") && (size==(2*sizeof(void *))))
		return(BGBGC_AllocCons());
#endif

	ty=BGBGC_FetchType(type);

	sz=size+sizeof(BGBGC_ObjInfo);
	inf=BGBGC_Alloc(sz);
	if(!inf)return(NULL);

	tmp=(void *)(inf+1);
	memset(tmp, 0, size);

//	inf->type=ty;
	inf->size=size;
	inf->ty_idx=ty->idx;
	inf->ty_chk=0xF000;
	BGBGC_MarkInfoSane(inf);

	ty->cnt++;
	ty->cntsz+=sz;

	BGBGC_TryMarkSafe(tmp);
	return(tmp);
}

void *BGBGC_NewExec(char *type, int size)
{
	BGBGC_ObjType *ty;
	BGBGC_ObjInfo *inf;
	void *tmp;
	int sz;

	BGBGC_InitTY();

	if(!type)return(NULL);

	if(BGBGC_CheckLogAlloc())
	{
		printf("BGBGC_NewExec: %s %d\n", type, size);
	}

	ty=BGBGC_FetchType(type);
	sz=size+sizeof(BGBGC_ObjInfo);
	inf=BGBGC_AllocExec(sz);
	if(!inf)return(NULL);

	tmp=(void *)(inf+1); memset(tmp, 0, size);

//	inf->type=ty;
	inf->size=size;
	inf->ty_idx=ty->idx;
	inf->ty_chk=0xF000;
	BGBGC_MarkInfoSane(inf);

	ty->cnt++; ty->cntsz+=sz;

	BGBGC_TryMarkSafe(tmp);
	return(tmp);
}

#ifdef BGBGC_USEPRECISE
void *BGBGC_NewPrecise(char *type, int size)
{
	BGBGC_ObjType *ty;
	BGBGC_ObjInfo *inf;
	void *tmp;
	int sz;

	BGBGC_InitTY();

	if(!type)return(NULL);

	ty=BGBGC_FetchType(type);
	sz=size+sizeof(BGBGC_ObjInfo);
	inf=BGBGC_AllocPrecise(sz);
	if(!inf)return(NULL);

	tmp=(void *)(inf+1); memset(tmp, 0, size);

//	inf->type=ty;
	inf->size=size;
	inf->ty_idx=ty->idx;
	inf->ty_chk=0xF000;
	BGBGC_MarkInfoSane(inf);

	ty->cnt++; ty->cntsz+=sz;

	BGBGC_TryMarkSafe(tmp);
	return(tmp);
}
#endif

void *BGBGC_NewRC(char *type, int size, int fl)
{
	BGBGC_ObjType *ty;
	BGBGC_ObjInfo *inf;
	void *tmp;
	int sz;

	BGBGC_InitTY();

	if(!type)return(NULL);

	if(BGBGC_CheckLogAlloc())
	{
		printf("BGBGC_NewRC: %s %d\n", type, size);
	}

	ty=BGBGC_FetchType(type);
	sz=size+sizeof(BGBGC_ObjInfo);

	if(fl&1)inf=BGBGC_AllocExec(sz);
	else	inf=BGBGC_Alloc(sz);
	if(!inf)return(NULL);

	tmp=(void *)(inf+1);
	memset(tmp, 0, size);

	inf->size=size;
	inf->ty_idx=ty->idx;
	inf->ty_chk=0x0000;
	BGBGC_MarkInfoSane(inf);

	ty->cnt++;
	ty->cntsz+=sz;

	BGBGC_TryMarkSafe(tmp);
	return(tmp);
}

int BGBGC_MarkInfoSane(BGBGC_ObjInfo *inf)
{
	int i;

	i=((inf->ty_idx*inf->size)&0x0FFF)^0x0AA5;
	inf->ty_chk=(inf->ty_chk&0xF000) | i;
	return(0);
}

int BGBGC_InfoSaneP(BGBGC_ObjInfo *inf)
{
	if(((volatile void **)inf)[1]==(void *)(0xF5EEDEAD))
		return(0);

	if(inf->ty_idx!=(inf->ty_idx&16383))return(0);
	if(!bgbdy_objtype_hash[inf->ty_idx])return(0);
	if((inf->ty_chk&0x0FFF)!=(((inf->ty_idx*inf->size)&0x0FFF)^0x0AA5))
		return(0);

	return(1);
}

int BGBGC_HandleMark(void *p)
{
	BGBGC_ObjInfo *inf;
	BGBGC_ObjType *ty;
	void *q;
	int i;

	if(!p)return(-1);

	inf=p;
	q=(void *)(inf+1);

	if(!BGBGC_InfoSaneP(inf))
	{
		if(((volatile void **)p)[1]==(void *)(0xF5EEDEAD))
			return(-1);
		printf("BGBGC_HandleMark: Heap Damage @ %p\n", p);
		return(-2);
	}

	ty=bgbdy_objtype_hash[inf->ty_idx];
	if(!ty)return(-1);

	if(ty->mark)
	{
		i=ty->mark(q);
		return(i);
	}
	return(-1);
}

int BGBGC_HandleDestroy(void *p)
{
	BGBGC_ObjInfo *inf;
	BGBGC_ObjType *ty;
	void *q;
	int i;

	if(!p)return(-1);

	inf=p;
	q=(void *)(inf+1);

	if(!BGBGC_InfoSaneP(inf))
	{
		if(((volatile void **)p)[1]==(void *)(0xF5EEDEAD))
			return(-1);
		printf("BGBGC_HandleDestroy: Heap Damage @ %p\n", p);
		return(-2);
	}

	ty=bgbdy_objtype_hash[inf->ty_idx];
	if(!ty)return(-1);

	if(ty->destroy)i=ty->destroy(q);
		else i=0;
	return(i);
}

int BGBGC_HandlePreserve(void *p)
{
	BGBGC_ObjInfo *inf;
	BGBGC_ObjType *ty;
	void *q;
	int i;

	if(!p)return(-1);

	inf=p;
	q=(void *)(inf+1);

	if(!BGBGC_InfoSaneP(inf))
	{
		if(((volatile void **)p)[1]==(void *)(0xF5EEDEAD))
			return(-1);
		printf("BGBGC_HandlePreserve: Heap Damage @ %p\n", p);
		return(-2);
	}

	ty=bgbdy_objtype_hash[inf->ty_idx];
	if(!ty)return(-1);

	if(ty->preserve)
	{
		i=ty->preserve(q);
		return(i);
	}
	return(-1);
}

char *BGBGC_HandleGetTypeName(void *p)
{
	BGBGC_ObjInfo *inf;
	BGBGC_ObjType *ty;
	void *q;
	int i;

	inf=p;
	q=(void *)(inf+1);

	if(!BGBGC_InfoSaneP(inf))
	{
		if(((volatile void **)p)[1]==(void *)(0xF5EEDEAD))
			return(NULL);
		return(UNDEFINED);
	}

	ty=bgbdy_objtype_hash[inf->ty_idx];
	if(!ty)return(UNDEFINED);
	return(ty->name);
}


int BGBGC_SetMark(char *type, int (*fn)(void *p))
{
	BGBGC_ObjType *ty;
	ty=BGBGC_FetchType(type);
	ty->mark=fn;
	return(0);
}

int BGBGC_SetDestroy(char *type, int (*fn)(void *p))
{
	BGBGC_ObjType *ty;
	ty=BGBGC_FetchType(type);
	ty->destroy=fn;
	return(0);
}

int BGBGC_SetPreserve(char *type, int (*fn)(void *p))
{
	BGBGC_ObjType *ty;
	ty=BGBGC_FetchType(type);
	ty->preserve=fn;
	return(0);
}

int BGBGC_BadInfoP(BGBGC_ObjInfo *inf)
{
	if(!BGBGC_InfoSaneP(inf))
	{
		if(((volatile void **)inf)[1]==(void *)(0xF5EEDEAD))
			return(-1);
		printf("BGBGC_BadInfoP: Heap Damage @ %p\n", inf);
		return(-2);
	}
	return(0);
}

BGBGC_ObjType *BGBGC_GetType(void *p)
{
	BGBGC_ObjInfo *inf;
	BGBGC_ObjType *ty;
	char *s;

	ty=BGBGC_ObjType_CheckPtrTypeCache(p);
	if(ty)return(ty);

	inf=BGBGC_TryGetBase(p);
//	inf=p-sizeof(BGBGC_ObjInfo);

	if(!inf)
	{
		if(BGBGC_BadInfoP(inf))
			return(NULL);
		s=BGBGC_GetTypeName(p);
		if(!s)return(NULL);
		ty=BGBGC_FetchType(s);
		BGBGC_ObjType_SetPtrTypeCache(p, ty);
		return(ty);
	}

	ty=bgbdy_objtype_hash[inf->ty_idx];
	BGBGC_ObjType_SetPtrTypeCache(p, ty);
	return(ty);
}

BGBGC_ObjType *BGBGC_GetTypeFast(void *p)
{
	BGBGC_ObjInfo *inf;
	BGBGC_ObjType *ty;

//	inf=BGBGC_GetBase(p);
//	inf=(BGBGC_ObjInfo *)(((byte *)p)-sizeof(BGBGC_ObjInfo));
	inf=((BGBGC_ObjInfo *)p)-1;

	ty=bgbdy_objtype_hash[inf->ty_idx];
	return(ty);
}

char *BGBGC_GetTypeNameHook(void *p)
{
	char *s;
	int i;

	for(i=0; i<bgbgc_ntypehook; i++)
	{
		s=bgbgc_typehook_f[i](p);
		if(s) return(s);
	}

	return(NULL);
}

char *BGBGC_GetTypeName(void *p)
{
	BGBGC_ObjInfo *inf;
	BGBGC_ObjType *ty;
	char *s;

	s=gcllSpaceType(p);
	if(s)return(s);

//	if(!p)return(NULL);
	if((p<=BGBGC_NULLEND) && (p>=BGBGC_NULLBASE))
		return(NULL);

	if(BGBGC_CheckCons(p)>=0)
		return("_cons_t");

	inf=BGBGC_TryGetBase(p);
	if(inf)
	{
		if(BGBGC_BadInfoP(inf))
			return(NULL);
		ty=bgbdy_objtype_hash[inf->ty_idx];
		return(ty->name);
	}
	
	s=BGBGC_GetTypeNameHook(p);
	if(s)return(s);
	
	return(NULL);
}

char *BGBGC_GetTypeNameFast(void *p)
{
	BGBGC_ObjInfo *inf;
	BGBGC_ObjType *ty;

	if(!p)return(NULL);

//	if(BGBGC_ConsP(p))
//		return("_cons_t");

//	inf=BGBGC_TryGetBase(p);
//	if(!inf)return(NULL);

//	inf=(BGBGC_ObjInfo *)(((byte *)p)-sizeof(BGBGC_ObjInfo));
	inf=((BGBGC_ObjInfo *)p)-1;

	ty=bgbdy_objtype_hash[inf->ty_idx];
	return(ty->name);
}

int BGBGC_GetTypeIdx(void *p)
{
	static int cons_idx=-1;
	BGBGC_ObjInfo *inf;
	BGBGC_ObjType *cur;
	char *s;

	if(!p)return(-1);

	if(BGBGC_ConsP(p))
	{
		if(cons_idx>=0)return(cons_idx);
		cur=BGBGC_FetchType("_cons_t");
		cons_idx=cur->idx;
		return(cons_idx);
	}

	s=gcllSpaceType(p);
	if(s)
	{
		cur=BGBGC_FetchType(s);
		return(cur->idx);
	}

	inf=BGBGC_TryGetBase(p);
	if(inf)
	{
		if(BGBGC_BadInfoP(inf))
			return(-1);
		return(inf->ty_idx);
	}

	s=BGBGC_GetTypeNameHook(p);
	if(s)
	{
		cur=BGBGC_FetchType(s);
		return(cur->idx);
	}

	return(-1);
}

int BGBGC_GetTypeIdxFast(void *p)
{
	BGBGC_ObjInfo *inf;

	if(!p)return(-1);
	inf=((BGBGC_ObjInfo *)p)-1;
	return(inf->ty_idx);
}

int BGBGC_GetSize(void *p)
{
	BGBGC_ObjInfo *inf;
	int i;

	if(!p)return(0);

	inf=BGBGC_TryGetBase(p);
	if(!inf)
	{
		if(BGBGC_ConsP(p))
			return(2*sizeof(void *));
		return(0);
	}

	if(BGBGC_BadInfoP(inf))
		return(0);

	return(inf->size);
}

int BGBGC_GetSizeFast(void *p)
{
	BGBGC_ObjInfo *inf;

	if(!p)return(0);
//	inf=BGBGC_GetBase(p);
//	if(!inf)return(0);

	inf=((BGBGC_ObjInfo *)p)-1;
	return(inf->size);
}

void *BGBGC_GetObjBase(void *p)
{
	void *q;

	p=BGBGC_GetBase(p);
	if(!p)return(p);

	p=(void *)(((BGBGC_ObjInfo *)p)+1);
	return(p);
}

size_t BGBGC_GetOffset(void *p)
{
	void *q;
	q=BGBGC_GetObjBase(p);
	return(((byte *)p)-((byte *)q));
}

size_t BGBGC_GetRelSize(void *p)
{
	return(BGBGC_GetSize(p)-BGBGC_GetOffset(p));
}

int BGBGC_TypeP(void *p, char *type)
{
	BGBGC_ObjInfo *inf;
	char *s;

	s=BGBGC_GetTypeName(p);
	if(!strcmp(s, type))return(1);
	return(0);
}

void *BGBGC_Copy(void *p)
{
	BGBGC_ObjInfo *inf;
	BGBGC_ObjType *ty;
	void *q;

	if(!p)return(NULL);

	inf=BGBGC_TryGetBase(p);
	if(!inf)return(NULL);
//	inf=p-sizeof(BGBGC_ObjInfo);

	ty=bgbdy_objtype_hash[inf->ty_idx];

	q=BGBGC_New(ty->name, inf->size);
	memcpy(q, p, inf->size);

	return(q);
}

int BGBGC_IncPtr(void *p)
{
	BGBGC_ObjInfo *inf;
	int i;

	if(!p)return(0);

	inf=BGBGC_TryGetBase(p);
	if(!inf) { return(0); }

	i=(inf->ty_chk>>12)&15;
	if(i==15)return(15);
	i++;
	inf->ty_chk=(inf->ty_chk&0xFFF)|(i<<15);

	return(i);
}

int BGBGC_DecPtr(void *p)
{
	BGBGC_ObjInfo *inf;
	int i;

	if(!p)return(0);

	inf=BGBGC_TryGetBase(p);
	if(!inf) { return(0); }

	i=(inf->ty_chk>>12)&15;
	if(i==15)return(15);
	if(i<2)
	{
		BGBGC_Free(p);
		return(0);
	}

	i--;
	inf->ty_chk=(inf->ty_chk&0xFFF)|(i<<15);

	return(i);
}

int BGBGC_SafeDecPtr(void *p)
{
	BGBGC_ObjInfo *inf;
	int i;

	if(!p)return(0);

	inf=BGBGC_TryGetBase(p);
	if(!inf) { return(0); }

	i=(inf->ty_chk>>12)&15;
	if(i==15)return(15);
	if(!i)return(0);
	i--;
	inf->ty_chk=(inf->ty_chk&0xFFF)|(i<<15);

	return(i);
}

BGBGC_API void *BGBGC_GetTypeVTab(char *str)
{
	BGBGC_ObjInfo *inf;
	BGBGC_ObjType *ty;
	char *s;

	if(!str)return(NULL);

	ty=BGBGC_FetchType(str);
	return(ty->vtab);
}

BGBGC_API void BGBGC_SetTypeVTab(char *str, void *p)
{
	BGBGC_ObjInfo *inf;
	BGBGC_ObjType *ty;
	char *s;

	if(!str)return;

	ty=BGBGC_FetchType(str);
	ty->vtab=p;
}

BGBGC_API void *BGBGC_GetObjVTab(void *p)
{
	BGBGC_ObjInfo *inf;
	BGBGC_ObjType *ty;
	char *s;

	if(!p)return(NULL);

	ty=BGBGC_ObjType_CheckPtrTypeCache(p);
	if(ty)return(ty->vtab);

	if(BGBGC_ConsP(p))return(BGBGC_GetTypeVTab("_cons_t"));

	s=gcllSpaceType(p);
//	if(s)return(BGBGC_GetTypeVTab(s));
	if(s)
	{
		ty=BGBGC_FetchType(s);
		if(!ty)return(NULL);
		BGBGC_ObjType_SetPtrTypeCache(p, ty);
		return(ty->vtab);
	}

	inf=BGBGC_TryGetBase(p);
	if(inf)
	{
		ty=bgbdy_objtype_hash[inf->ty_idx];
		if(!ty)return(NULL);
		BGBGC_ObjType_SetPtrTypeCache(p, ty);
		return(ty->vtab);
	}

	s=BGBGC_GetTypeNameHook(p);
	if(s)
	{
		ty=BGBGC_FetchType(s);
		if(!ty)return(NULL);
		BGBGC_ObjType_SetPtrTypeCache(p, ty);
		return(ty->vtab);
	}

	return(NULL);
}



static int bgdy_weak_mark(void *p)
{
	return(0);
}

static int bgdy_weak_preserve(void *p)
{
	void **a;
	int i, j, k;

	a=(void **)p;
	j=BGBGC_GetSize(p)/sizeof(void *);
	for(i=0; i<j; i++)
	{
		k=BGBGC_CheckObjPtr(a[i]);
		if(k<0)a[i]=NULL;
	}

	return(0);
}


int BGBGC_HashSourceInfo(BGBGC_SourceInfo *srcinf)
{
	int hi;
	byte *cs, *cse;
	
	cs=(byte *)srcinf;
	cse=cs+sizeof(BGBGC_SourceInfo);
	hi=0;
	while(cs<cse)
		{ hi=(hi*4093+1)+(*cs++); }
	return(hi);
}

int BGBGC_IndexSourceInfo(BGBGC_SourceInfo *srcinf)
{
	BGBGC_SourceInfo *srcinf2;
	int hi;
	int i, j, k;

	
	hi=BGBGC_HashSourceInfo(srcinf);
	j=hi;
	for(i=0; i<256; i++)
	{
		k=(j>>12)&0xFFFF;
		srcinf2=bgbdy_srcinfo_hash[k];
		if(!srcinf2)
		{
			srcinf2=malloc(sizeof(BGBGC_SourceInfo));
			*srcinf2=*srcinf;
			bgbdy_srcinfo_hash[k]=srcinf2;
			return(k);
		}
		if(!memcmp(srcinf, srcinf2, sizeof(BGBGC_SourceInfo)))
			{ return(k); }
		k=k*4093+1;
	}
	return(0);
}

int BGBGC_GetObjSourceInfo(void *p, BGBGC_SourceInfo *srcinf)
{
	BGBGC_ObjInfo *inf;
	if(!p)
	{
		memset(srcinf, 0, sizeof(BGBGC_SourceInfo));
		return(-1);
	}

	inf=BGBGC_TryGetBase(p);
	if(!inf)
	{
		memset(srcinf, 0, sizeof(BGBGC_SourceInfo));
		return(-1);
	}

	return(BGBGC_HandleGetObjSourceInfo(inf, srcinf));
}

int BGBGC_HandleGetObjSourceInfo(void *p, BGBGC_SourceInfo *srcinf)
{
	BGBGC_ObjInfo *inf;
	BGBGC_SourceInfo *srcinf2;
	int i;

	inf=p;
	if(BGBGC_BadInfoP(inf))
	{
		memset(srcinf, 0, sizeof(BGBGC_SourceInfo));
		return(-1);
	}

	i=inf->srcinfo;
	srcinf2=bgbdy_srcinfo_hash[i];
	if(srcinf2)
		{ memcpy(srcinf, srcinf2, sizeof(BGBGC_SourceInfo)); }
	else
		{ memset(srcinf, 0, sizeof(BGBGC_SourceInfo)); }

	return(i);
}

int BGBGC_SetObjSourceInfo(void *p, BGBGC_SourceInfo *srcinf)
{
	BGBGC_ObjInfo *inf;
	BGBGC_SourceInfo *srcinf2;
	int i;

	if(!p)
		{ return(-1); }

	inf=BGBGC_TryGetBase(p);
	if(!inf || BGBGC_BadInfoP(inf))
		{ return(-1); }

	i=BGBGC_IndexSourceInfo(srcinf);
	inf->srcinfo=i;

	return(i);
}

int BGBGC_SetObjLLn(void *p, char *fn, int ln)
{
	BGBGC_SourceInfo tsrcinf;
	BGBGC_ObjInfo *inf;
	BGBGC_SourceInfo *srcinf2;
	int i;

	if(!p)
		{ return(-1); }

	inf=BGBGC_TryGetBase(p);
	if(!inf || BGBGC_BadInfoP(inf))
		{ return(-1); }

	i=inf->srcinfo;
	srcinf2=bgbdy_srcinfo_hash[i];
	if(srcinf2)
		{ memcpy(&tsrcinf, srcinf2, sizeof(BGBGC_SourceInfo)); }
	else
		{ memset(&tsrcinf, 0, sizeof(BGBGC_SourceInfo)); }

	tsrcinf.lfn=bgbgc_strdup(fn);
	tsrcinf.lln=ln;

	i=BGBGC_IndexSourceInfo(&tsrcinf);
	inf->srcinfo=i;

	return(i);
}

int BGBGC_SetObjLLnAccess(void *p, char *fn, int ln, u32 acc)
{
	BGBGC_SourceInfo tsrcinf;
	BGBGC_ObjInfo *inf;
	BGBGC_SourceInfo *srcinf2;
	int i;

	if(!p)
		{ return(-1); }

	inf=BGBGC_TryGetBase(p);
	if(!inf || BGBGC_BadInfoP(inf))
		{ return(-1); }

	i=inf->srcinfo;
	srcinf2=bgbdy_srcinfo_hash[i];
	if(srcinf2)
		{ memcpy(&tsrcinf, srcinf2, sizeof(BGBGC_SourceInfo)); }
	else
		{ memset(&tsrcinf, 0, sizeof(BGBGC_SourceInfo)); }

	tsrcinf.lfn=bgbgc_strdup(fn);
	tsrcinf.lln=ln;
	tsrcinf.access=acc;

	i=BGBGC_IndexSourceInfo(&tsrcinf);
	inf->srcinfo=i;

	return(i);
}

int BGBGC_InitTY()
{
	static int init=0;
	BGBGC_SourceInfo *srcinf;
	int i;

	if(init)return(0);
	init=1;

	for(i=0; i<16384; i++)
		bgbdy_objtype_hash[i]=NULL;

	bgbdy_objtypes=NULL;
	BGBGC_Init();
//	BGBGC_InitTypes();

	BGBGC_SetMark("_weak_t", bgdy_weak_mark);
	BGBGC_SetPreserve("_weak_t", bgdy_weak_preserve);

	srcinf=malloc(sizeof(BGBGC_SourceInfo));
	memset(srcinf, 0, sizeof(BGBGC_SourceInfo));
	i=BGBGC_HashSourceInfo(srcinf);
	i=(i>>12)&0xFFFF;
	bgbdy_srcinfo_hash[0]=srcinf;
	bgbdy_srcinfo_hash[i]=srcinf;

	return(0);
}
