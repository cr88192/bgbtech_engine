#include <bgbgc.h>

void **bgbgc_lobj;
int *bgbgc_lobj_size;
int *bgbgc_lobj_flag;
int bgbgc_lobj_num;
int bgbgc_lobj_max;

int BGBGC_InitLow()
{
	int i;

#ifdef BGBGC_SHAVEMEM
	i=1<<16;
#else
	i=1<<20;
#endif
	
	bgbgc_lobj=malloc(i*sizeof(void *));
	bgbgc_lobj_size=malloc(i*sizeof(int));
	bgbgc_lobj_flag=malloc(i*sizeof(int));
	bgbgc_lobj_max=i;

	bgbgc_lobj_num=0;
	memset(bgbgc_lobj, 0, i*sizeof(void *));

	bgbgc_size_lim_lobjs=BGBGC_LOBJSIZE;

#ifdef BGBGC_TINYLIST
	BGBGC_FlushTiny();
#endif

	return(0);
}

int BGBGC_BaseHeapLObj(size_t heap)
{
	while(heap>bgbgc_size_lim_lobjs)
		bgbgc_size_lim_lobjs+=
			bgbgc_size_lim_lobjs>>2;
	return(0);
}

static int log2up(int v)
{
	int c;
	c=0;
	while(v>1)
	{
		c++;
		if((v>1) && (v&1))v++;
		v>>=1;
	}
	return(c);
}

int BGBGC_LookupLObj(void *p)
{
	void *q, *r;
	int i;

	for(i=0; i<bgbgc_lobj_num; i++)
		if(bgbgc_lobj[i])
	{
		q=bgbgc_lobj[i];
		r=((byte *)q)+bgbgc_lobj_size[i];
		if((p>=q) && (p<r))return(i);
	}
	return(-1);
}

int BGBGC_InsertLObj(void *p, int sz, int fl)
{
	void *q, *r;
	int i, j, n;

	if((bgbgc_lobj_num+1)>=bgbgc_lobj_max)
	{
		n=bgbgc_lobj_max;
		i=n; n=n+(n>>1);

		bgbgc_lobj=realloc(bgbgc_lobj, n*sizeof(void *));
		bgbgc_lobj_size=realloc(bgbgc_lobj_size, n*sizeof(int));
		bgbgc_lobj_flag=realloc(bgbgc_lobj_flag, n*sizeof(int));
		memset(bgbgc_lobj+i, 0, (n-i)*sizeof(void *));
		bgbgc_lobj_max=n;
	}


	i=0; j=bgbgc_lobj_num>>1;
	while(j>1)
	{
		if(bgbgc_lobj[i+j]<=p)
			i+=j;
		j>>=1;
	}

	for(; i<bgbgc_lobj_num; i++)
	{
		if((i+1)<bgbgc_lobj_num)
		{
			q=bgbgc_lobj[i+1];
			if(q>p)break;
		}
	}
	
	for(j=bgbgc_lobj_num++; j>i; j--)
	{
		bgbgc_lobj[j]=bgbgc_lobj[j-1];
		bgbgc_lobj_size[j]=bgbgc_lobj_size[j-1];
		bgbgc_lobj_flag[j]=bgbgc_lobj_flag[j-1];
	}

	bgbgc_lobj[i]=p;
	bgbgc_lobj_size[i]=sz;
	bgbgc_lobj_flag[i]=fl;

	return(i);

}

void *BGBGC_AllocLObj(int size)
{
	int i, n;
	void *p, *p2, *q;

	p=malloc(size+64);
	if(!p)return(NULL);
	
//	i=(nlint)p;
//	if(i&15)p=((byte *)p)+(16-(i&15));

	i=(size+16)&(~15);
	q=((byte *)p)+i;
	*(int *)q=0x31415927;
	
	BGBGC_InsertLObj(p, size, 0);
	return(p);
}

#if 0
void *BGBGC_AllocLObj(int size)
{
	int i, n;

#if 0
	for(i=0; i<bgbgc_lobj_num; i++)
		if(!bgbgc_lobj[i])
	{
		bgbgc_lobj[i]=malloc(size+64);
		bgbgc_lobj_size[i]=size;
		bgbgc_lobj_flag[i]=0;
		return(bgbgc_lobj[i]);
	}
#endif

	if((bgbgc_lobj_num+1)<bgbgc_lobj_max)
	{
		i=bgbgc_lobj_num++;
		bgbgc_lobj[i]=malloc(size+64);
		bgbgc_lobj_size[i]=size;
		bgbgc_lobj_flag[i]=0;

		//allocation failed
		if(!bgbgc_lobj[i])
			bgbgc_lobj_num--;

		return(bgbgc_lobj[i]);
	}

	n=bgbgc_lobj_max;
	i=n; n=n+(n>>1);

	bgbgc_lobj=realloc(bgbgc_lobj, n*sizeof(void *));
	bgbgc_lobj_size=realloc(bgbgc_lobj_size, n*sizeof(int));
	bgbgc_lobj_flag=realloc(bgbgc_lobj_flag, n*sizeof(int));
	memset(bgbgc_lobj+i, 0, (n-i)*sizeof(void *));
	bgbgc_lobj_max=n;

	i=bgbgc_lobj_num++;
	bgbgc_lobj[i]=malloc(size+64);
	bgbgc_lobj_size[i]=size;
	bgbgc_lobj_flag[i]=0;
	bgbgc_lobj_num=i+1;

	//allocation failed
	if(!bgbgc_lobj[i])
		bgbgc_lobj_num--;

	return(bgbgc_lobj[i]);
}
#endif

int BGBGC_FreeLObj(void *p)
{
	BGBGC_SourceInfo tsrcinf;
	void *q, *q2, *q3;
	char *s, *s1;
	int i, j, k, sz;

//	i=BGBGC_LookupLObj(p);
	i=BGBGC_LookupLObjS(p);

	if(i<0)
	{
		if(!p)return(-1);
#ifdef X86
		if(p<((void *)4096))return(-1);
		if(p>=((void *)0xC0000000UL))return(-1);
#endif
		i=BGBGC_LookupLObj(p);
	}
	
	if(i>=0)
	{
		sz=bgbgc_lobj_size[i];
		q=bgbgc_lobj[i];
		j=(sz+16)&(~15);
		q2=((byte *)q)+j;

#if 1
		if(!BGBGC_InfoSaneP(q))
		{
			printf("BGBGC_FreeLObj: Corrupt Info Obj @%p\n", q);

			if(i>0)
			{
				q3=bgbgc_lobj[i-1];

				s=BGBGC_HandleGetTypeName(q3);

				BGBGC_HandleGetObjSourceInfo(q3, &tsrcinf);
				s1=tsrcinf.lfn;
				if(!s1)s1="?";
				j=tsrcinf.lln;
				k=tsrcinf.access;
	
				printf("Prior-Obj @%p Ty=%s, LLn=%s:%d, Acc=%08X\n",
					q3, s, s1, j, k);
			}

#if 1
			for(; i<bgbgc_lobj_num; i++)
			{
				bgbgc_lobj[i]=bgbgc_lobj[i+1];
				bgbgc_lobj_size[i]=bgbgc_lobj_size[i+1];
				bgbgc_lobj_flag[i]=bgbgc_lobj_flag[i+1];
			}
			bgbgc_lobj_num--;
			return(-1);
#endif
		}
#endif

#if 1
		if((*(int *)q2)!=0x31415927)
		{
			printf("BGBGC_FreeLObj: Overrun Detect Obj @%p\n", q);

			s=BGBGC_HandleGetTypeName(q);

			BGBGC_HandleGetObjSourceInfo(q, &tsrcinf);
			s1=tsrcinf.lfn;
			if(!s1)s1="?";
			j=tsrcinf.lln;
			k=tsrcinf.access;
	
			printf("Obj @%p Ty=%s, LLn=%s:%d, Acc=%08X\n", p, s, s1, j, k);

#if 1
			for(; i<bgbgc_lobj_num; i++)
			{
				bgbgc_lobj[i]=bgbgc_lobj[i+1];
				bgbgc_lobj_size[i]=bgbgc_lobj_size[i+1];
				bgbgc_lobj_flag[i]=bgbgc_lobj_flag[i+1];
			}
			bgbgc_lobj_num--;
			return(-1);
#endif
		}
#endif

		free(q);
		bgbgc_lobj[i]=NULL;

#if 1
		for(; i<bgbgc_lobj_num; i++)
		{
			bgbgc_lobj[i]=bgbgc_lobj[i+1];
			bgbgc_lobj_size[i]=bgbgc_lobj_size[i+1];
			bgbgc_lobj_flag[i]=bgbgc_lobj_flag[i+1];
		}
		bgbgc_lobj_num--;
#endif

		return(sz);
//		return(0);
	}
	return(-1);
}

int BGBGC_FreeLObjIdx(int i)
{
	free(bgbgc_lobj[i]);
	bgbgc_lobj[i]=NULL;

#if 1
	for(; i<bgbgc_lobj_num; i++)
	{
		bgbgc_lobj[i]=bgbgc_lobj[i+1];
		bgbgc_lobj_size[i]=bgbgc_lobj_size[i+1];
		bgbgc_lobj_flag[i]=bgbgc_lobj_flag[i+1];
	}
	bgbgc_lobj_num--;
#endif

	return(0);
}

int BGBGC_ShiftLObj2()
{
	int i, j;

	j=0;
	for(i=0; i<bgbgc_lobj_num; i++)
		if(bgbgc_lobj[i])
	{
		if(j!=i)
		{
			bgbgc_lobj[j]=bgbgc_lobj[i];
			bgbgc_lobj_size[j]=bgbgc_lobj_size[i];
			bgbgc_lobj_flag[j]=bgbgc_lobj_flag[i];
		}
		bgbgc_lobj_flag[j]&=~15;
		j++;
	}
	bgbgc_lobj_num=j;
	return(0);
}

void BGBGC_ShiftLObj_Swap(int i, int j)
{
	void *p;
	int k;

	if(i==j)return;

	p=bgbgc_lobj[i];
	bgbgc_lobj[i]=bgbgc_lobj[j];
	bgbgc_lobj[j]=p;

	k=bgbgc_lobj_size[i];
	bgbgc_lobj_size[i]=bgbgc_lobj_size[j];
	bgbgc_lobj_size[j]=k;

	k=bgbgc_lobj_flag[i];
	bgbgc_lobj_flag[i]=bgbgc_lobj_flag[j];
	bgbgc_lobj_flag[j]=k;
}

void BGBGC_ShiftLObj_Sort(int base, int lim)
{
	static int depth=0;
	void *mp;
	int low, mid, high;
	int i, j;

	if((lim-base)<2)return;

	if(depth>=32)	//bad case, use selection sort
	{
		for(i=base; i<lim; i++)for(j=i+1; j<lim; j++)
			if(bgbgc_lobj[j]<bgbgc_lobj[i])
				BGBGC_ShiftLObj_Swap(i, j);
		return;
	}

	depth++;

	low=base; high=lim;

	mid=(low+high)/2;
	mp=bgbgc_lobj[mid];

	i=low;
	while(i<high)
	{
		if(bgbgc_lobj[i]<mp)
		{
			BGBGC_ShiftLObj_Swap(low++, i++);
			continue;
		}
		if(bgbgc_lobj[i]>mp)
		{
			BGBGC_ShiftLObj_Swap(--high, i);
			continue;
		}
		i++;
	}

	BGBGC_ShiftLObj_Sort(base, low);
	BGBGC_ShiftLObj_Sort(high, lim);

	depth--;
}

int BGBGC_ShiftLObj()
{
	void *p;
	int i, j, k;

	BGBGC_ShiftLObj2();

	BGBGC_ShiftLObj_Sort(0, bgbgc_lobj_num);
#if 0
	for(i=0; i<bgbgc_lobj_num; i++)
		for(j=i+1; j<bgbgc_lobj_num; j++)
			if(bgbgc_lobj[j]<bgbgc_lobj[i])
	{
		p=bgbgc_lobj[i];
		bgbgc_lobj[i]=bgbgc_lobj[j];
		bgbgc_lobj[j]=p;

		k=bgbgc_lobj_size[i];
		bgbgc_lobj_size[i]=bgbgc_lobj_size[j];
		bgbgc_lobj_size[j]=k;

		k=bgbgc_lobj_flag[i];
		bgbgc_lobj_flag[i]=bgbgc_lobj_flag[j];
		bgbgc_lobj_flag[j]=k;
	}
#endif

	return(0);
}

int BGBGC_LookupLObjS(void *p)
{
	void *q, *r;
	int i, j;

	//dunno why this is... I forget...
//	return(BGBGC_LookupLObj(p));	//?...

	if(BGBGC_CheckSpaceP(p))
		return(-1);

	i=0; j=bgbgc_lobj_num>>1;
	while(j>1)
	{
		if(bgbgc_lobj[i+j]<=p)
			i+=j;
		j>>=1;
	}

	for(; i<bgbgc_lobj_num; i++)
	{
		q=bgbgc_lobj[i];
		if(q>p)break;
		r=((byte *)q)+bgbgc_lobj_size[i];
		if((p>=q) && (p<r))return(i);
	}

//	return(BGBGC_LookupLObj(p));
	return(-1);
}

int BGBGC_LookupLObjS2(void *p)
{
	int i;

	if(BGBGC_CheckSpaceP(p))
		return(-1);

	i=BGBGC_LookupLObjS(p);
	if(i>=0)return(i);
	return(BGBGC_LookupLObj(p));
}

s64 BGBGC_SizeLObjs()
{
	s64 sz;
	int i, j, k;

	sz=0;
	for(i=0; i<bgbgc_lobj_num; i++)
		if(bgbgc_lobj[i])
	{
		sz+=bgbgc_lobj_size[i];
	}
	return(sz);
}