#include <bgbgc.h>

int bgbgc_stat_type[65536];
int bgbgc_stat_type_idx[65536];
int bgbgc_stat_type_bytes[65536];

int bgbgc_stat_srcinf[65536];
int bgbgc_stat_srcinf_idx[65536];
int bgbgc_stat_srcinf_bytes[65536];

int bgbgc_stat_sizes[512];
int bgbgc_stat_sizetab[512];
int bgbgc_stat_sizes_log2[64];

double bgbgc_log2(double f)
{
	return(log(f)/log(2));
}

int BGBGC_Stat_ScanStatObj(void *p) //AH:ignore
{
	BGBGC_ObjInfo *inf;
	BGBGC_SourceInfo tsrcinf;
	char *ty, *lfn;
	int lln, acc;
	int i, j;
	
//	if(k>0)BGBGC_Mark(p);
//	if(k==-2)BGBGC_PrintBacktrack(chk, i);

	ty=BGBGC_HandleGetTypeName(p);
	if(!ty)return(-1);
	if(ty==UNDEFINED)
	{
		return(-2);
	}

	inf=p;
	bgbgc_stat_type[inf->ty_idx]++;
	bgbgc_stat_srcinf[inf->srcinfo]++;

//	bgbgc_stat_type_bytes[inf->ty_idx]+=inf->size;
//	bgbgc_stat_srcinf_bytes[inf->srcinfo]+=inf->size;

	i=inf->size;
	if(i<0)
		{ i=0; }
	if(i>(1<<28))
		{ j=0; }

//	i=(i+15)/16;
//	j=(int)(log(i)*13.13883);
	j=(int)(bgbgc_log2(i)*8+0.5);
	if(j<0)j=0;
	if(j>255)j=255;
	bgbgc_stat_sizes[j]++;

//	j=(int)(ceil(bgbgc_log2(i)));
	j=(int)(bgbgc_log2(i)+0.5);
	bgbgc_stat_sizes_log2[j]++;

	i=(i+15)/16;

	bgbgc_stat_type_bytes[inf->ty_idx]+=i*16;
	bgbgc_stat_srcinf_bytes[inf->srcinfo]+=i*16;

#if 0
	BGBGC_HandleGetObjSourceInfo(p, &tsrcinf);
	lfn=tsrcinf.lfn;
	if(!lfn)lfn="?";
	lln=tsrcinf.lln;
	acc=tsrcinf.access;
#endif
}

int BGBGC_Stat_ScanChk(BGBGC_Chunk *chk) //AH:ignore
{
	int i, j, k, cells;
	int csize, mapsize;
	void *p;

	cells=chk->cells;
	for(i=0; i<cells; i++)
	{
		j=chk->map[i];
//		k=j&15;
		k=j&3;

		if(k==BGBGC_MAP_TYPE_HEAD)	//unmarked object
		{
			p=(chk->data)+(i<<BGBGC_CELLSHIFT);
			k=BGBGC_Stat_ScanStatObj(p);
			if(k==-2)BGBGC_PrintBacktrack(chk, i);
			continue;
		}

		if(k==BGBGC_MAP_LOCK_HEAD)	//locked object
		{
			p=(chk->data)+(i<<BGBGC_CELLSHIFT);
			k=BGBGC_Stat_ScanStatObj(p);
			if(k==-2)BGBGC_PrintBacktrack(chk, i);
			continue;
		}
	}
	return(0);
}

int BGBGC_Stat_Scan()
{
	BGBGC_Chunk *chk;
	int i, j;

	chk=bgbgc_initchain;
	while(chk)
	{
		BGBGC_Stat_ScanChk(chk);
		chk=chk->next;
	}

	for(i=0; i<bgbgc_lobj_num; i++)
	{
		BGBGC_Stat_ScanStatObj(bgbgc_lobj[i]);
	}

	return(0);
}

int BGBGC_Stat_SortStats(int *stat, int *idx, int *bytes, int n)
{
	int i, j, k;

#if 1
	//collapse
	for(i=0, j=0; i<n; i++)
	{
		if(stat[i])
		{
			stat[j]=stat[i];
			idx[j]=idx[i];
			bytes[j]=bytes[i];
			j++;

//			stat[i]=0;
//			idx[i]=0;
		}
	}
	
	for(i=j; i<n; i++)
	{
		stat[i]=0;
		idx[i]=0;
	}
	
	n=j;
#endif

	for(i=0; i<n; i++)
	{
		for(j=i+1; j<n; j++)
		{
			if(stat[j]>stat[i])
			{
				k=stat[i];
				stat[i]=stat[j];
				stat[j]=k;

				k=idx[i];
				idx[i]=idx[j];
				idx[j]=k;

				k=bytes[i];
				bytes[i]=bytes[j];
				bytes[j]=k;
			}
		}
	}
	
	return(n);
}

char *BGBGC_Stat_GetIDName(int id)
{
	char *str;
	switch(id)
	{
	case 0: str="root"; break;
	case 1: str="user"; break;
	case 2: str="guest"; break;
	default: str="unknown"; break;
	}
	return(str);
}

char *BGBGC_Stat_GetModeName(int id)
{
	static char buf[64];
	
	buf[0]=(id&0x800)?'S':'-';
	buf[1]=(id&0x400)?'R':'-';
	buf[2]=(id&0x200)?'W':'-';
	buf[3]=(id&0x100)?'X':'-';
	buf[4]='_';
	buf[5]=(id&0x080)?'S':'-';
	buf[6]=(id&0x040)?'R':'-';
	buf[7]=(id&0x020)?'W':'-';
	buf[8]=(id&0x010)?'X':'-';
	buf[9]='_';
	buf[10]=(id&0x008)?'S':'-';
	buf[11]=(id&0x004)?'R':'-';
	buf[12]=(id&0x002)?'W':'-';
	buf[13]=(id&0x001)?'X':'-';
	buf[14]=0;
	
	return(buf);
}

int BGBGC_Stat_StatHeap(int fl)
{
	BGBGC_ObjType *tyinf;
	BGBGC_SourceInfo *srcinf;
	char *lfn, *uidn, *gidn, *modn;
	int lln, acc;
	int i, j, k, i1;
	
	for(i=0; i<65536; i++)
	{
		bgbgc_stat_type[i]=0;
		bgbgc_stat_srcinf[i]=0;

		bgbgc_stat_type_bytes[i]=0;
		bgbgc_stat_srcinf_bytes[i]=0;

		bgbgc_stat_type_idx[i]=i;
		bgbgc_stat_srcinf_idx[i]=i;
	}

	for(i=0; i<256; i++)
	{
		bgbgc_stat_sizes[i]=0;
		
//		j=(int)pow(2.71828, i/13.13883);
		j=(int)(pow(2, i/8.0)+0.5);
		bgbgc_stat_sizetab[i]=j;
	}
	
	BGBGC_Stat_Scan();

	BGBGC_Stat_SortStats(bgbgc_stat_type,
		bgbgc_stat_type_idx, bgbgc_stat_type_bytes, 65536);
	BGBGC_Stat_SortStats(bgbgc_stat_srcinf,
		bgbgc_stat_srcinf_idx, bgbgc_stat_srcinf_bytes, 65536);

	gc_printf("Sizes (TSz=Bytes):\n");
	for(i=0; i<512; i++)
	{
		j=bgbgc_stat_sizes[i];
		if(!j)continue;
		k=bgbgc_stat_sizetab[i];
		gc_printf("%4d(%6d): Sz=%d, TSz(B)=%d\n", i, j, k, j*k);
	}

	gc_printf("Sizes Log2 (TSz=Bytes):\n");
	for(i=0; i<64; i++)
	{
		j=bgbgc_stat_sizes_log2[i];
		if(!j)continue;
		k=pow(2, i);
		gc_printf("%4d(%6d): Sz=%d, TSz(B)=%d\n", i, j, k, j*k);
	}

	gc_printf("Types (TSz=KiB):\n");
	for(i=0; i<65536; i++)
	{
		i1=bgbgc_stat_type_idx[i];
		j=bgbgc_stat_type[i];
		k=bgbgc_stat_type_bytes[i];
		if(!j)continue;
		tyinf=BGBGC_FetchIndexType(i1);
		if(!tyinf)
		{
			gc_printf("%6d(%6d): BAD Type\n", i1, j);
			continue;
		}

		k=(k+1023)/1024;

		gc_printf("%5d(%6d): Ty=%-32s, TSz=%d\n", i1, j, tyinf->name, k);
	}

	gc_printf("SourceInfo (TSz=KiB):\n");
	for(i=0; i<65536; i++)
	{
		i1=bgbgc_stat_srcinf_idx[i];
		j=bgbgc_stat_srcinf[i];
		k=bgbgc_stat_srcinf_bytes[i];
		if(!j)continue;
		srcinf=BGBGC_FetchIndexSourceInfo(i1);
		if(!srcinf)
		{
			gc_printf("%6d(%6d): BAD SrcInfo\n", i1, j);
			continue;
		}

		lfn=srcinf->lfn;
		lln=srcinf->lln;
		if(!lfn)lfn="?";
		acc=srcinf->access;
		
		k=(k+1023)/1024;
		
//		gc_printf("%6d(%6d): LLn=%24s:%4d, Acc=%08X, Sz=%d\n",
//			i1, j, lfn, lln, acc, k);

//		gc_printf("%6d(%6d): LLn=%24s:%4d, Sz=%d\n",
//			i1, j, lfn, lln, k);

		gc_printf("%5d(%6d): LLn=%24s:%4d, TSz=%6d, Acc=%X\n",
			i1, j, lfn, lln, k, acc);

#if 0
		gidn=BGBGC_Stat_GetIDName((acc>>22)&1023);
		uidn=BGBGC_Stat_GetIDName((acc>>12)&1023);
		modn=BGBGC_Stat_GetModeName(acc&4095);

		gc_printf("    Acc=%08X, UGID=%s/%s Mode=%s\n",
			acc, uidn, gidn, modn);
#endif
	}
}
