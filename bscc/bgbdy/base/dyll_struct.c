#include <bgbdy.h>

BGBDY_API dycClass dyllGetStructClassInfo(char *qname)
{
	char tb[256];
	dycClass inf;
	char *s, *s0, *s1, *s2;
	s64 fl;
	int i;

	sprintf(tb, "bgbdy/%s:sig", qname);
	s0=dyllMetaLookupKey(tb);
	if(s0)
	{
		s=s0;
		while(*s=='P')s++;
		if((*s!='X') && (*s!='U'))
			return(NULL);
		s++;
		s1=dyll_SigReadQName(&s);
		s1=dyllString(s1);

		printf("dyllGetStructClassInfo: C '%s' -> '%s'\n", qname, s1);

		inf=dyllGetStructClassInfo(s1);
		return(inf);
	}
	

	s=dyllMetaLookupKey(qname);
	if(!s)return(NULL);

//	printf("dyllGetStructClassInfo: A '%s' -> '%s'\n", qname, s);
	
	if(!strcmp(s, "typedef") ||
		!strcmp(s, "type"))
	{
		sprintf(tb, "%s:sig", qname);
		s0=dyllMetaLookupKey(tb);
		
//		printf("dyllGetStructClassInfo: '%s' -> '%s'\n", qname, s0);
		
		s=s0;
		while(*s=='P')s++;
		if((*s!='X') && (*s!='U'))
			return(NULL);
		s++;
		s1=dyll_SigReadQName(&s);
		s1=dyllString(s1);

//		printf("dyllGetStructClassInfo: B '%s' -> '%s'\n", qname, s1);

		inf=dyllGetStructClassInfo(s1);
		return(inf);
	}
	
	if(strcmp(s, "struct"))
		return(NULL);
	
	inf=dycLookupClass(qname);
	if(inf)return(inf);
	
	dycBeginStruct(qname);
	
	for(i=0; i<1024; i++)
	{
		sprintf(tb, "%s:field.%d", qname, i);
		s0=dyllMetaLookupKey(tb);
		if(!s0)break;

//		sprintf(tb, "%s/%s:name", qname, s);
//		s0=dyllMetaLookupKey(tb);
//		if(!s0)break;

		sprintf(tb, "%s/%s:sig", qname, s0);
		s1=dyllMetaLookupKey(tb);
		if(!s1)break;
		dycSlotBasic(s0, s1);

		sprintf(tb, "%s/%s:flagstr", qname, s0);
		s2=dyllMetaLookupKey(tb);
		if(s2 && (*s2))
		{
			fl=dyllDecodeFlagSig(s2);
			dycAddSlotFlags(s0, fl);
		}
	}
	
	inf=dycEndStruct();
	return(inf);
}

/** Fetch the sig, offset, and flags for a given struct field */
BGBDY_API int dyllGetStructSlotSigOffsFlags(char *qname, char *fname,
	char **rsig, int *roffs, s64 *rflags)
{
	static int init=0;
	static char *hash_qn[256];
	static char *hash_fn[256];
	static char *hash_s[256];
	static int hash_o[256];
	static s64 hash_fl[256];

	dycClass inf;
	dycClassV vinf;
	dycSlot finf;
	s64 fl;
	char *s;
	int i, h;

	if(!init)
	{
		for(i=0; i<256; i++)
			{ hash_qn[i]=NULL; hash_fn[i]=NULL; }
		init=1;
	}

	h=0;
	s=qname; while(*s)h=(h*251)+(*s++);
	s=fname; while(*s)h=(h*251)+(*s++);
	h=(h>>8)&255;
	
	if(hash_qn[h] && hash_fn[h] &&
		!strcmp(hash_qn[h], qname) &&
		!strcmp(hash_fn[h], fname))
	{
		if(rsig)*rsig=hash_s[h];
		if(roffs)*roffs=hash_o[h];
		if(rflags)*rflags=hash_fl[h];
		return(0);
	}
	
	inf=dyllGetStructClassInfo(qname);
	if(!inf)return(-1);
	finf=dycIdxSlot(inf, fname);
	if(!finf)return(-1);
	
	vinf=dycGetClassVersion(inf);
	
	s=dycGetSlotSig(finf);
	i=dycGetStructSlotOffset(vinf, finf);
	fl=dycGetSlotFlags(finf);

	if(!s || (i<0))return(-1);

	hash_qn[h]=dyllString(qname);
	hash_fn[h]=dyllString(fname);
	hash_s[h]=dyllString(s);
	hash_o[h]=i;
	hash_fl[h]=fl;

	if(rsig)*rsig=s;
	if(roffs)*roffs=i;
	if(rflags)*rflags=fl;
	return(0);
}

/** Fetch the sig and offset for a given struct field */
BGBDY_API int dyllGetStructSlotSigOffs(char *qname, char *fname,
	char **rsig, int *roffs)
{
	return(dyllGetStructSlotSigOffsFlags(
		qname, fname, rsig, roffs, NULL));
}

BGBDY_API char *dyllGetStructSlotSig(char *qname, char *fname)
{
	char *sig;
	int i, j;
	i=dyllGetStructSlotSigOffs(qname, fname, &sig, &j);
	if(i<0)return(NULL);
	return(sig);
}

BGBDY_API int dyllGetStructSlotOffs(char *qname, char *fname)
{
	char *sig;
	int i, j;
	i=dyllGetStructSlotSigOffs(qname, fname, &sig, &j);
	if(i<0)return(-1);
	return(j);
}

BGBDY_API dyt dyllGetStructSlotDynamic(void *ptr, char *qname, char *fname)
{
	s64 fl;
	char *sig;
	int i, j;
	i=dyllGetStructSlotSigOffsFlags(qname, fname, &sig, &j, &fl);
	if(i<0)return(UNDEFINED);
	return(dyllGetVarSigFlags(((byte *)ptr)+j, sig, fl));
}

BGBDY_API dyt dyllSetStructSlotDynamic(void *ptr,
	char *qname, char *fname, dyt val)
{
	char *sig;
	int i, j;
	i=dyllGetStructSlotSigOffs(qname, fname, &sig, &j);
	if(i<0)return(UNDEFINED);
	dyllSetVarSig(((byte *)ptr)+j, sig, val);
	return(val);
}

BGBDY_API dyt dyllGetDynamicStructSlotDynamic(
	void *obj, char *fname)
{
	s64 fl;
	char *ty;
	char *sig;
	int i, j;
	ty=dygettype(obj);
	if(!ty)return(UNDEFINED);
	i=dyllGetStructSlotSigOffsFlags(ty, fname, &sig, &j, &fl);
	if(i<0)return(UNDEFINED);
	return(dyllGetVarSigFlags(((byte *)obj)+j, sig, fl));
}

BGBDY_API dyt dyllSetDynamicStructSlotDynamic(void *obj, char *fname, dyt val)
{
	char *ty;
	char *sig;
	int i, j;
	ty=dygettype(obj);
	if(!ty)return(UNDEFINED);
	i=dyllGetStructSlotSigOffs(ty, fname, &sig, &j);
	if(i<0)return(UNDEFINED);
	dyllSetVarSig(((byte *)obj)+j, sig, val);
	return(val);
}
