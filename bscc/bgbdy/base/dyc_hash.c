#include <bgbgc.h>
#include <bgbdy.h>

// BGBGC_ClassSlot *bgbdyc_slothash[16384];
// BGBGC_ClassInfo *bgbdyc_slothash_class[16384];
// BGBGC_ClassInfo *bgbdyc_curclass;

// BGBGC_ClassMinf *bgbdyc_minfhash[16384];
// BGBGC_ClassInfo *bgbdyc_minfhash_class[16384];


BGBGC_ClassSlot **bgbdyc_slothash;
BGBGC_ClassMinf **bgbdyc_minfhash;
BGBGC_ClassInfo **bgbdyc_slothash_class;
BGBGC_ClassInfo **bgbdyc_minfhash_class;
int *bgbdyc_slothash_cnt;
int *bgbdyc_minfhash_cnt;
int bgbdyc_slothash_sz;
int bgbdyc_minfhash_sz;

//dyt bgbdyc_slothash_lock;
//dyt bgbdyc_minfhash_lock;

BGBDY_API int BGBDYC_SlotHashUsed()
{
	int i, j;
	j=0; for(i=0; i<bgbdyc_slothash_sz; i++)
		if(bgbdyc_slothash[i])j++;
	return(j);
}

BGBDY_API int BGBDYC_MinfHashUsed()
{
	int i, j;
	j=0; for(i=0; i<bgbdyc_minfhash_sz; i++)
		if(bgbdyc_minfhash[i])j++;
	return(j);
}

BGBDY_API int BGBDYC_SlotHashSize()
	{ return(bgbdyc_slothash_sz); }
BGBDY_API int BGBDYC_MinfHashSize()
	{ return(bgbdyc_minfhash_sz); }

BGBDY_API int BGBDYC_DumpSlotHash()
{
	int i, j;

	for(i=0; i<bgbdyc_slothash_sz; i++)
		if(bgbdyc_slothash[i])
	{
		printf("%d: %p %p %s.%s %s\n", i,
			bgbdyc_slothash_class[i], bgbdyc_slothash[i],
			bgbdyc_slothash_class[i]->qname,
			bgbdyc_slothash[i]->name, bgbdyc_slothash[i]->sig);
	}

	return(i);
}

BGBDY_API int BGBDYC_DumpMinfHash()
{
	int i, j;

	for(i=0; i<bgbdyc_minfhash_sz; i++)
		if(bgbdyc_minfhash[i])
	{
		printf("%d: %p %p %s.%s %s\n", i,
			bgbdyc_minfhash_class[i], bgbdyc_minfhash[i],
			bgbdyc_minfhash_class[i]->qname,
			bgbdyc_minfhash[i]->name, bgbdyc_minfhash[i]->sig);
	}

	return(i);
}

void BGBDYC_InitHash()
{
	int i;

//	bgbdyc_slothash_lock=thMutex();
//	bgbdyc_minfhash_lock=thMutex();

	i=16384;
	bgbdyc_slothash=gcalloc(i*sizeof(BGBGC_ClassSlot *));
	bgbdyc_minfhash=gcalloc(i*sizeof(BGBGC_ClassMinf *));
	bgbdyc_slothash_class=gcalloc(i*sizeof(BGBGC_ClassInfo *));
	bgbdyc_minfhash_class=gcalloc(i*sizeof(BGBGC_ClassInfo *));
	bgbdyc_slothash_cnt=gcalloc(i*sizeof(int));
	bgbdyc_minfhash_cnt=gcalloc(i*sizeof(int));
	bgbdyc_slothash_sz=i;
	bgbdyc_minfhash_sz=i;
}

int BGBDYC_CalcHashName(char *name)
{
	char *s;
	int hi;

	s=name; hi=0;
	if(*s)hi=(*s++)<<12;
	while(*s)hi=(hi+(*s++))*4093;
	return(hi);
}

int BGBDYC_CalcHashNameSig(char *name, char *sig)
{
	char *s;
	int hi;

	s=name; hi=0;
	if(*s)hi=(*s++)<<12;
	while(*s)hi=(hi+(*s++))*4093;
	s=sig; while(*s)hi=(hi+(*s++))*4093;
	return(hi);
}

void BGBDYC_SortHashItems_Swap(
	void **h_data, void **h_class, int *h_cnt,
	int i, int j)
{
	void *p;
	int k;

	if(i==j)return;

	p=h_data[i]; h_data[i]=h_data[j]; h_data[j]=p;
	p=h_class[i]; h_class[i]=h_class[j]; h_class[j]=p;
	k=h_cnt[i]; h_cnt[i]=h_cnt[j]; h_cnt[j]=k;
}

void BGBDYC_SortHashItems(
	void **h_data, void **h_class, int *h_cnt,
	int base, int lim)
{
	static int depth=0;
	int mp;
	int low, mid, high;
	int i, j, k;

	if((lim-base)<2)return;

	if(depth>=32)	//bad case, use selection sort
	{
		j=h_cnt[base];
		for(i=base+1; i<lim; i++)
			if(h_cnt[i]!=j)break;
		if(i>=lim)return;

		k=1; low=base; high=lim;
		while(k)
		{
			k=0;
			high--;
			for(i=low; i<high; i++)
				if(h_cnt[i+1]>h_cnt[i])
			{
				BGBDYC_SortHashItems_Swap(
					h_data, h_class, h_cnt, i, i+1);
				k++;
			}

			low++;
			for(i=high; i>low; i--)
				if(h_cnt[i-1]<h_cnt[i])
			{
				BGBDYC_SortHashItems_Swap(
					h_data, h_class, h_cnt, i, i-1);
				k++;
			}
		}

//		for(i=base; i<lim; i++)for(j=i+1; j<lim; j++)
//			if(h_cnt[j]<h_cnt[i])
//				BGBDYC_SortHashItems_Swap(
//					h_data, h_class, h_cnt, i, j);
		return;
	}

	depth++;

	low=base; high=lim; mid=(low+high)/2;
	i=h_cnt[low]; j=h_cnt[mid]; k=h_cnt[high-1];
	if((i==j) && (j==k))
	{
		for(i=low; i<high; i++)
			if(h_cnt[i]!=j)break;
		if(i>=high)return;
	}
	mp=(i>=j)?((i<k)?i:k):((j<k)?j:k);

	i=low;
	while(i<high)
	{
		if(h_cnt[i]>mp)
		{
			BGBDYC_SortHashItems_Swap(
				h_data, h_class, h_cnt, low++, i++);
			continue;
		}
		if(h_cnt[i]<mp)
		{
			BGBDYC_SortHashItems_Swap(
				h_data, h_class, h_cnt, --high, i);
			continue;
		}
		i++;
	}

	BGBDYC_SortHashItems(h_data, h_class, h_cnt, base, low);
	BGBDYC_SortHashItems(h_data, h_class, h_cnt, high, lim);
	depth--;
}

void BGBDYC_ResizeSlotHash()
{
	BGBGC_ClassSlot **adata, **bdata;
	BGBGC_ClassInfo **aclass, **bclass;
	int *acnt, *bcnt;
	int asz, bsz, sz, hi;
	int i, j, k;

	adata=bgbdyc_slothash;
	aclass=bgbdyc_slothash_class;
	acnt=bgbdyc_slothash_cnt;
	asz=bgbdyc_slothash_sz;

	BGBDYC_SortHashItems((void **)adata, (void **)aclass,
		acnt, 0, asz);

	bsz=asz+(asz>>1);
	if(bsz>(1<<20))bsz=1<<20;
	if(acnt[asz-(asz>>1)]<10)bsz=asz;

	i=bsz;
	bdata=gcalloc(i*sizeof(BGBGC_ClassSlot *));
	bclass=gcalloc(i*sizeof(BGBGC_ClassInfo *));
	bcnt=gcalloc(i*sizeof(int));

	sz=asz; if(sz==bsz)sz-=sz>>1;
	for(i=0; i<sz; i++)
	{
		if(!adata[i])continue;
		if(!acnt[i])break;

		hi=BGBDYC_CalcHashName(adata[i]->name);
		hi>>=12;
		for(j=0; j<16; j++)
		{
//			k=(hi>>12)%bsz;
			k=((unsigned int)hi)%bsz;
			if(!bdata[k])
			{
				bdata[k]=adata[i];
				bclass[k]=aclass[i];
				bcnt[k]=acnt[i]>>4;
				break;
			}
//			hi*=4093;
			hi*=127;
		}
	}


	bgbdyc_slothash=bdata;
	bgbdyc_slothash_class=bclass;
	bgbdyc_slothash_cnt=bcnt;
	bgbdyc_slothash_sz=bsz;

	gcfree(adata);
	gcfree(aclass);
	gcfree(acnt);
}

void BGBDYC_ResizeMinfHash()
{
	BGBGC_ClassMinf **adata, **bdata;
	BGBGC_ClassInfo **aclass, **bclass;
	int *acnt, *bcnt;
	int asz, bsz, sz, hi;
	int i, j, k;

	adata=bgbdyc_minfhash;
	aclass=bgbdyc_minfhash_class;
	acnt=bgbdyc_minfhash_cnt;
	asz=bgbdyc_minfhash_sz;

	BGBDYC_SortHashItems((void **)adata, (void **)aclass,
		acnt, 0, asz);

	bsz=asz+(asz>>1);
	if(bsz>(1<<20))bsz=1<<20;
	if(acnt[asz-(asz>>1)]<10)bsz=asz;

	i=bsz;
	bdata=gcalloc(i*sizeof(BGBGC_ClassMinf *));
	bclass=gcalloc(i*sizeof(BGBGC_ClassInfo *));
	bcnt=gcalloc(i*sizeof(int));

	sz=asz; if(sz==bsz)sz-=sz>>1;
	for(i=0; i<sz; i++)
	{
		if(!adata[i])continue;
		if(!acnt[i])break;

		hi=BGBDYC_CalcHashNameSig(adata[i]->name, adata[i]->sig);
		hi>>=12;
		for(j=0; j<16; j++)
		{
//			k=(hi>>12)%bsz;
			k=((unsigned int)hi)%bsz;
			if(!bdata[k])
			{
				bdata[k]=adata[i];
				bclass[k]=aclass[i];
				bcnt[k]=acnt[i]>>4;
				break;
			}
//			hi*=4093;
			hi*=127;
		}
	}


	bgbdyc_minfhash=bdata;
	bgbdyc_minfhash_class=bclass;
	bgbdyc_minfhash_cnt=bcnt;
	bgbdyc_minfhash_sz=bsz;
}

static bgbgc_safe_inc(int *ri)
{
	*ri=(*ri)+1;
	if((*ri)>(1<<30))*ri=1<<30;
}

BGBGC_ClassSlot *BGBDYC_CheckHashSlot(
	BGBGC_ClassInfo *inf, char *name, int hi)
{
	BGBGC_ClassSlot *sinf;
	BGBGC_ClassInfo *tinf;
	char *s, *t;
	int i, j, k;

#if 1
	//special case: faster lookup for 2^n hash sizes
	if(!(bgbdyc_slothash_sz&(bgbdyc_slothash_sz>>1)))
	{
		j=hi>>12;
		for(i=0; i<16; i++)
		{
//			k=((unsigned int)j)%bgbdyc_slothash_sz;
			k=j&(bgbdyc_slothash_sz-1);

			sinf=bgbdyc_slothash[k];
			if(!sinf)break;

			tinf=bgbdyc_slothash_class[k];
			if(tinf!=inf) { j*=127; continue; }

			if(!strcmp(sinf->name, name))
			{
				if(bgbdyc_slothash_cnt[k]<(1<<30))
					bgbdyc_slothash_cnt[k]++;
				return(sinf);
			}
			j*=127;
		}
		return(NULL);
	}
#endif

//	thLockMutex(bgbdyc_slothash_lock);
	j=hi>>12;
	for(i=0; i<16; i++)
	{
		k=((unsigned int)j)%bgbdyc_slothash_sz;
//		k=j&(bgbdyc_slothash_sz-1);

		sinf=bgbdyc_slothash[k];
		if(!sinf)break;

		tinf=bgbdyc_slothash_class[k];
		if(tinf!=inf) { j*=127; continue; }

		if(!strcmp(sinf->name, name))
		{
//			bgbgc_safe_inc(bgbdyc_slothash_cnt+k);
//			thUnlockMutex(bgbdyc_slothash_lock);
			if(bgbdyc_slothash_cnt[k]<(1<<30))
				bgbdyc_slothash_cnt[k]++;
			return(sinf);
		}
		j*=127;
	}
//	thUnlockMutex(bgbdyc_slothash_lock);
	return(NULL);
}

void BGBDYC_AddHashSlot(
	BGBGC_ClassInfo *inf, BGBGC_ClassSlot *sinf, int hi)
{
	int i, j, k;

	printf("Add Slot %p %s %s\n", sinf, sinf->name, sinf->sig);

//	thLockMutex(bgbdyc_slothash_lock);
	j=hi>>12;
	for(i=0; i<16; i++)
	{
		k=((unsigned int)j)%bgbdyc_slothash_sz;
		if(!bgbdyc_slothash[k])
		{
			bgbdyc_slothash[k]=sinf;
			bgbdyc_slothash_class[k]=inf;
			bgbdyc_slothash_cnt[k]=0;
//			thUnlockMutex(bgbdyc_slothash_lock);
			return;
		}
		j*=127;
	}

	BGBDYC_ResizeSlotHash();
	BGBDYC_AddHashSlot(inf, sinf, hi);
//	thUnlockMutex(bgbdyc_slothash_lock);
}

BGBGC_ClassMinf *BGBDYC_CheckHashMethodSig(
	BGBGC_ClassInfo *inf, char *name, char *sig, int hi)
{
	BGBGC_ClassMinf *minf;
	BGBGC_ClassInfo *tinf;
	char *s, *t;
	int i, j, k;

//	thLockMutex(bgbdyc_minfhash_lock);
	j=hi>>12;
	for(i=0; i<16; i++)
	{
		k=((unsigned int)j)%bgbdyc_minfhash_sz;
		minf=bgbdyc_minfhash[k];
		tinf=bgbdyc_minfhash_class[k];

		if(!minf)break;
		if(tinf!=inf) { j*=127; continue; }

		if(!strcmp(minf->name, name) && !strcmp(minf->sig, sig))
		{
//			bgbgc_safe_inc(bgbdyc_minfhash_cnt+k);
//			thUnlockMutex(bgbdyc_minfhash_lock);
			if(bgbdyc_minfhash_cnt[k]<(1<<30))
				bgbdyc_minfhash_cnt[k]++;
			return(minf);
		}
		j*=127;
	}

//	thUnlockMutex(bgbdyc_minfhash_lock);
	return(NULL);
}

void BGBDYC_AddHashMethod(
	BGBGC_ClassInfo *inf, BGBGC_ClassMinf *minf, int hi)
{
	int i, j, k;

//	printf("Add Method %p\n", minf);

//	thLockMutex(bgbdyc_minfhash_lock);
	j=hi>>12;
	for(i=0; i<16; i++)
	{
		k=((unsigned int)j)%bgbdyc_minfhash_sz;
		if(!bgbdyc_minfhash[k])
		{
//			printf("Add Method 2 %p %d\n", minf, k);

			bgbdyc_minfhash[k]=minf;
			bgbdyc_minfhash_class[k]=inf;
			bgbdyc_minfhash_cnt[k]=0;
//			thUnlockMutex(bgbdyc_minfhash_lock);
			return;
		}
		j*=127;
	}

	BGBDYC_ResizeMinfHash();
	BGBDYC_AddHashMethod(inf, minf, hi);
//	thUnlockMutex(bgbdyc_minfhash_lock);
}

BGBGC_ClassSlot *BGBDYC_CheckHashSlot2(
	BGBGC_ClassInfo *inf, char *name, int hi)
{
	BGBGC_ClassSlot *sinf;
	BGBGC_ClassInfo *tinf;
	char *s, *t;
	int i, j, k;

	j=hi>>12;
	for(i=0; i<16; i++)
	{
		k=((unsigned int)j)%bgbdyc_slothash_sz;

		sinf=bgbdyc_slothash[k];
		if(!sinf)break;

		tinf=bgbdyc_slothash_class[k];
		if(tinf!=inf) { j*=127; continue; }

		if(sinf->name==name)
		{
			if(bgbdyc_slothash_cnt[k]<(1<<30))
				bgbdyc_slothash_cnt[k]++;
			return(sinf);
		}
		j*=127;
	}
	return(NULL);
}

BGBGC_ClassMinf *BGBDYC_CheckHashMethodSig2(
	BGBGC_ClassInfo *inf, char *name, char *sig, int hi)
{
	BGBGC_ClassMinf *minf;
	BGBGC_ClassInfo *tinf;
	char *s, *t;
	int i, j, k;

	j=hi>>12;
	for(i=0; i<16; i++)
	{
		k=((unsigned int)j)%bgbdyc_minfhash_sz;
		minf=bgbdyc_minfhash[k];
		tinf=bgbdyc_minfhash_class[k];

		if(!minf)break;
		if(tinf!=inf) { j*=127; continue; }

		if((minf->name==name) && (minf->sig==sig))
		{
			if(bgbdyc_minfhash_cnt[k]<(1<<30))
				bgbdyc_minfhash_cnt[k]++;
			return(minf);
		}
		j*=127;
	}
	return(NULL);
}

