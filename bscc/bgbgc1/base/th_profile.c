#include <bgbgc.h>

BGBGC_ImportFuncs_t bgbgc_importfuncs;

volatile int bgbgc_prof_active=0;
volatile char bgbgc_prof_reqstop=0;
volatile char bgbgc_prof_reqalloc=0;
volatile int **bgbgc_prof_stats=NULL;
volatile int *bgbgc_prof_statsb=NULL;

volatile u32 *bgbgc_prof_stats_eip=NULL;
volatile int *bgbgc_prof_stats_cnt=NULL;
volatile u32 bgbgc_prof_totcnt=0;

volatile int *bgbgc_prof_statsagg_cnt=NULL;
volatile char **bgbgc_prof_statsagg_lib=NULL;
volatile char **bgbgc_prof_statsagg_sym=NULL;
volatile int bgbgc_prof_statsagg_num=0;
volatile int bgbgc_prof_statsagg_totcnt=0;

BGBGC_API BGBGC_ImportFuncs_t *BGBGC_GetImportFuncs()
{
	return(&bgbgc_importfuncs);
}

char *BGBGC_GetAddrName(void *ptr)
{
	if(bgbgc_importfuncs.GetAddrName)
		return(bgbgc_importfuncs.GetAddrName(ptr));
	return(NULL);
}

char *BGBGC_GetAddrLastName(void *ptr, void **rbp)
{
	if(bgbgc_importfuncs.GetAddrLastName)
		return(bgbgc_importfuncs.GetAddrLastName(ptr, rbp));
	return(NULL);
}

char *BGBGC_LookupSectionAddrName(void *ptr)
{
	char *s;
	if(bgbgc_importfuncs.LookupSectionAddrName)
	{
		s=bgbgc_importfuncs.LookupSectionAddrName(ptr);
		if(s)return(s);
	}
	
	if(BGBGC_GetChunk(ptr))
		return("<HEAP>");
	
	return(NULL);
}

void BGBGC_ProfileLoop_HandleThread(thContext *inf)
{
	BGBGC_RegisterState regs;
	int i, j, k;

	BGBGC_GetThreadRegisterState(inf, &regs);
#ifdef X86
	i=(regs.eip)>>16;
	j=(regs.eip&65535)>>4;
	
//	if(!bgbgc_prof_stats[i])
//	{
//		k=4096*sizeof(int);
//		bgbgc_prof_stats[i]=malloc(k);
//		memset(bgbgc_prof_stats[i], 0, k);
//	}

	bgbgc_prof_totcnt++;
	bgbgc_prof_statsb[i]++;

	if(!bgbgc_prof_stats[i])
	{
		bgbgc_prof_reqalloc=1;
		return;
	}
	
	bgbgc_prof_stats[i][j]++;

//	bgbgc_prof_stats[(regs.eip)>>4]++;
#endif
}

void BGBGC_ProfileDumpStats_SortSwap(int a, int b)
{
	u32 ipa, ipb;
	int cna, cnb;
	
	ipa=bgbgc_prof_stats_eip[a];
	ipb=bgbgc_prof_stats_eip[b];
	bgbgc_prof_stats_eip[a]=ipb;
	bgbgc_prof_stats_eip[b]=ipa;

	cna=bgbgc_prof_stats_cnt[a];
	cnb=bgbgc_prof_stats_cnt[b];
	bgbgc_prof_stats_cnt[a]=cnb;
	bgbgc_prof_stats_cnt[b]=cna;
}

void BGBGC_ProfileDumpStats_Sort(int base, int lim)
{
	static int depth=0;
	int mp;
	int low, mid, high;
	int i, j;

	if((lim-base)<2)return;

	if(depth>=32)	//bad case, use selection sort
	{
		for(i=base; i<lim; i++)for(j=i+1; j<lim; j++)
//			if(bgbgc_prof_stats_cnt[j]<bgbgc_prof_stats_cnt[i])
			if(bgbgc_prof_stats_cnt[j]>bgbgc_prof_stats_cnt[i])
				BGBGC_ProfileDumpStats_SortSwap(i, j);
		return;
	}

	depth++;

	low=base; high=lim;

	mid=(low+high)/2;
	mp=bgbgc_prof_stats_cnt[mid];

	i=low;
	while(i<high)
	{
		if(bgbgc_prof_stats_cnt[i]>mp)
//		if(bgbgc_prof_stats_cnt[i]<mp)
		{
			BGBGC_ProfileDumpStats_SortSwap(low++, i++);
			continue;
		}
		if(bgbgc_prof_stats_cnt[i]<mp)
//		if(bgbgc_prof_stats_cnt[i]>mp)
		{
			BGBGC_ProfileDumpStats_SortSwap(--high, i);
			continue;
		}
		i++;
	}

	BGBGC_ProfileDumpStats_Sort(base, low);
	BGBGC_ProfileDumpStats_Sort(high, lim);

	depth--;
}

void BGBGC_ProfileDumpStats_AggAddName(char *lib, char *sym, int cnt)
{
	int i, j, k;

	if(!bgbgc_prof_statsagg_cnt)
	{
		bgbgc_prof_statsagg_cnt=malloc((1<<16)*sizeof(int));
		bgbgc_prof_statsagg_lib=malloc((1<<16)*sizeof(char *));
		bgbgc_prof_statsagg_sym=malloc((1<<16)*sizeof(char *));
	}
	
	bgbgc_prof_statsagg_totcnt+=cnt;
	
	for(i=0; i<bgbgc_prof_statsagg_num; i++)
	{
		if(!strcmp(bgbgc_prof_statsagg_lib[i], lib) &&
			!strcmp(bgbgc_prof_statsagg_sym[i], sym))
		{
			bgbgc_prof_statsagg_cnt[i]+=cnt;
			return;
		}
	}
	
	i=bgbgc_prof_statsagg_num++;
	bgbgc_prof_statsagg_cnt[i]=cnt;
	bgbgc_prof_statsagg_lib[i]=lib;
	bgbgc_prof_statsagg_sym[i]=sym;
}

void BGBGC_ProfileDumpStats_SortSwapAgg(int a, int b)
{
	char *sla, *slb;
	char *ssa, *ssb;
	u32 ipa, ipb;
	int cna, cnb;
	
//	ipa=bgbgc_prof_stats_eip[a];
//	ipb=bgbgc_prof_stats_eip[b];
//	bgbgc_prof_stats_eip[a]=ipb;
//	bgbgc_prof_stats_eip[b]=ipa;

	cna=bgbgc_prof_statsagg_cnt[a];
	cnb=bgbgc_prof_statsagg_cnt[b];
	bgbgc_prof_statsagg_cnt[a]=cnb;
	bgbgc_prof_statsagg_cnt[b]=cna;

	sla=bgbgc_prof_statsagg_lib[a];
	slb=bgbgc_prof_statsagg_lib[b];
	bgbgc_prof_statsagg_lib[a]=slb;
	bgbgc_prof_statsagg_lib[b]=sla;

	ssa=bgbgc_prof_statsagg_sym[a];
	ssb=bgbgc_prof_statsagg_sym[b];
	bgbgc_prof_statsagg_sym[a]=ssb;
	bgbgc_prof_statsagg_sym[b]=ssa;
}

void BGBGC_ProfileDumpStats_SortAgg(int base, int lim)
{
	static int depth=0;
	int mp;
	int low, mid, high;
	int i, j;

	if((lim-base)<2)return;

	if(depth>=32)	//bad case, use selection sort
	{
		for(i=base; i<lim; i++)for(j=i+1; j<lim; j++)
			if(bgbgc_prof_statsagg_cnt[j]>bgbgc_prof_statsagg_cnt[i])
				BGBGC_ProfileDumpStats_SortSwapAgg(i, j);
		return;
	}

	depth++;

	low=base; high=lim;

	mid=(low+high)/2;
	mp=bgbgc_prof_statsagg_cnt[mid];

	i=low;
	while(i<high)
	{
		if(bgbgc_prof_statsagg_cnt[i]>mp)
		{
			BGBGC_ProfileDumpStats_SortSwapAgg(low++, i++);
			continue;
		}
		if(bgbgc_prof_statsagg_cnt[i]<mp)
		{
			BGBGC_ProfileDumpStats_SortSwapAgg(--high, i);
			continue;
		}
		i++;
	}

	BGBGC_ProfileDumpStats_SortAgg(base, low);
	BGBGC_ProfileDumpStats_SortAgg(high, lim);

	depth--;
}

int BGBGC_ProfileDumpStats()
{
	char *s, *s1, *s2;
	void *ptr, *bptr;
	u32 eip;
	int i, j, k, n, n2, dst;

	if(!bgbgc_prof_stats_eip)
	{
		i=(1<<20)*sizeof(u32);
		bgbgc_prof_stats_eip=malloc(i);
		i=(1<<20)*sizeof(int);
		bgbgc_prof_stats_cnt=malloc(i);
	}
	
	n=0;
	for(i=0; i<(1<<16); i++)
	{
		if(!bgbgc_prof_stats[i])
			continue;
		for(j=0; j<4096; j++)
		{
			k=bgbgc_prof_stats[i][j];
			if(!k)continue;
			
			if(n>=(1<<20))
				break;
			
			bgbgc_prof_stats_eip[n]=(i<<16)|(j<<4);
			bgbgc_prof_stats_cnt[n]=k;
			n++;
		}
	}
	
	BGBGC_ProfileDumpStats_Sort(0, n);
	
	n2=n;
	if(n2>64)n2=64;
	
	gc_printf("Profile Stats:\n");
	for(i=0; i<n2; i++)
	{
		eip=bgbgc_prof_stats_eip[i];
//		s=BGBGC_GetAddrName((void *)eip);

		ptr=(void *)eip;
		s=BGBGC_GetAddrLastName(ptr, &bptr);
		if(!s)s="<NULL>";
		dst=((byte *)ptr)-((byte *)bptr);
	
		s1=BGBGC_LookupSectionAddrName(ptr);
		s2=BGBGC_LookupSectionAddrName(bptr);
		if(!s1 || !s2 || (s1!=s2))
		{
			s1="<NULL>";
			if(dst>16384)
				s="?";
		}
	
		gc_printf("%08X: %7d (%16s!%-40s+%7d) %3.2f%%\n",
			bgbgc_prof_stats_eip[i],
			bgbgc_prof_stats_cnt[i],
			s1, s, dst,
			(100.0*bgbgc_prof_stats_cnt[i])/(bgbgc_prof_totcnt+1));
	}
	
	bgbgc_prof_statsagg_num=0;
	bgbgc_prof_statsagg_totcnt=0;

	n2=n;
	if(n2>1024)n2=1024;
	
//	gc_printf("Profile Stats:\n");
	for(i=0; i<n2; i++)
	{
		eip=bgbgc_prof_stats_eip[i];
//		s=BGBGC_GetAddrName((void *)eip);

		ptr=(void *)eip;
		s=BGBGC_GetAddrLastName(ptr, &bptr);
		if(!s)s="<NULL>";
		dst=((byte *)ptr)-((byte *)bptr);
	
		s1=BGBGC_LookupSectionAddrName(ptr);
		if(!s1)
		{
			s1="<NULL>";
			if(dst>16384)
				s="?";
		}
	
		if(!strcmp(s, "NtDelayExecution"))
			continue;
	
		BGBGC_ProfileDumpStats_AggAddName(s1, s,
			bgbgc_prof_stats_cnt[i]);
	}

	BGBGC_ProfileDumpStats_SortAgg(0,
		bgbgc_prof_statsagg_num);

	n2=bgbgc_prof_statsagg_num;
	if(n2>64)n2=64;
	
	gc_printf("Profile Stats Aggregate:\n");
	for(i=0; i<n2; i++)
	{
		s1=bgbgc_prof_statsagg_lib[i];
		s=bgbgc_prof_statsagg_sym[i];
	
		gc_printf("%20s!%-40s %9d %3.2f%%\n",
			s1, s, bgbgc_prof_statsagg_cnt[i],
//			(100.0*bgbgc_prof_statsagg_cnt[i])/(bgbgc_prof_totcnt+1));
			(100.0*bgbgc_prof_statsagg_cnt[i])/
				(bgbgc_prof_statsagg_totcnt+1));
	}
}

int BGBGC_ProfileLoop(void *p)
{
	int ncnt;
	int i, j, k;

	if(bgbgc_prof_active)
		return(0);

	if(!bgbgc_prof_stats)
	{
		i=(1<<16)*sizeof(int *);
		bgbgc_prof_stats=malloc(i);
		memset(bgbgc_prof_stats, 0, i);

		i=(1<<16)*sizeof(int);
		bgbgc_prof_statsb=malloc(i);
		memset(bgbgc_prof_statsb, 0, i);
	}

	gc_printf("Profiler Start:\n");

	ncnt=30*1000;
	bgbgc_prof_active++;
	while(1)
	{
		if(bgbgc_prof_reqstop)
			break;
	
		BGBGC_HandleThreadStates(BGBGC_ProfileLoop_HandleThread);
		
		if(bgbgc_prof_reqalloc)
		{
			for(j=0; j<(1<<16); j++)
			{
				if(bgbgc_prof_statsb[j] && !bgbgc_prof_stats[j])
				{
					k=4096*sizeof(int);
					bgbgc_prof_stats[j]=malloc(k);
					memset(bgbgc_prof_stats[j], 0, k);
				}
			}
			bgbgc_prof_reqalloc=0;
		}
		
//		thSleep(10);
		thSleep(1);
		
		ncnt--;
		if(ncnt<=0)
		{
			BGBGC_ProfileDumpStats();
			ncnt=30*1000;
			continue;
		}
	}

	gc_printf("Profiler Stop:\n");

	bgbgc_prof_active--;
	
	if(bgbgc_prof_active<=0)
	{
		bgbgc_prof_reqstop=0;
		bgbgc_prof_active=0;
	}
	return(0);
}

BGBGC_API int BGBGC_ProfilerSetActive(int fl)
{
	if(fl)
	{
		if(!bgbgc_prof_active)
		{
			thThread(BGBGC_ProfileLoop, NULL);
		}
	}else
	{
		if(bgbgc_prof_active)
		{
			bgbgc_prof_reqstop=1;
		}
	}
}
