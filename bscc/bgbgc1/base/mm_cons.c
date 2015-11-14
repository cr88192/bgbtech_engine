#include <bgbgc.h>

bgbgc_consref **bgbgc_cblk_data;
byte **bgbgc_cblk_map;
int bgbgc_cblk_num;
int bgbgc_cblk_max;

int bgbgc_cblk_rov_blk=0;
int bgbgc_cblk_rov_cell=0;

extern volatile int bgbgc_lastgc;
extern volatile int bgbgc_used_cons;
extern volatile int bgbgc_freed_cons;
extern volatile int bgbgc_total_cons;
extern volatile void *bgbgc_alloc_mutex;

static void *bgbgc_conspool=NULL;

size_t bgbgc_size_lim_cons;

#ifdef BGBGC_USEPRECISE
byte *bgbgc_precise_cbase=NULL;
byte *bgbgc_precise_cend=NULL;
#endif

int BGBGC_InitCons()
{
	int i;

	i=1<<12;
	bgbgc_cblk_data=malloc(i*sizeof(bgbgc_consref *));
	bgbgc_cblk_map=malloc(i*sizeof(byte *));
	bgbgc_cblk_max=i;

	bgbgc_cblk_num=0;
	memset(bgbgc_cblk_data, 0, i*sizeof(bgbgc_consref *));
	memset(bgbgc_cblk_map, 0, i*sizeof(byte *));

	bgbgc_used_cons=0;
	bgbgc_total_cons=0;

	bgbgc_size_lim_cons=BGBGC_CONSLIM;

	for(i=0; i<BGBGC_INITCONS; i++)
		BGBGC_AllocCBlk();

	return(0);
}

int BGBGC_BaseHeapCons(size_t heap)
{
	if(!bgbgc_size_lim_cons)
		bgbgc_size_lim_cons=BGBGC_CONSLIM;

	while(heap>bgbgc_size_lim_cons)
		bgbgc_size_lim_cons+=
			bgbgc_size_lim_cons>>2;
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

void BGBGC_AllocCBlk()
{
	int i, n;

	if(bgbgc_cblk_num<bgbgc_cblk_max)
	{
		i=bgbgc_cblk_num++;
		bgbgc_cblk_data[i]=malloc(BGBGC_CBLK_CELLS*2*sizeof(bgbgc_consref));
		bgbgc_cblk_map[i]=malloc(BGBGC_CBLK_CELLS);

		memset(bgbgc_cblk_data[i], 0,
			BGBGC_CBLK_CELLS*2*sizeof(bgbgc_consref));
		memset(bgbgc_cblk_map[i], 0, BGBGC_CBLK_CELLS);

		bgbgc_total_cons+=BGBGC_CBLK_CELLS;
		bgbgc_cblk_rov_blk=i;
		bgbgc_cblk_rov_cell=0;
		return;
	}

	n=bgbgc_cblk_max;
	i=n; n=n+(n>>1);

	bgbgc_cblk_data=realloc(bgbgc_cblk_data, n*sizeof(bgbgc_consref *));
	bgbgc_cblk_map=realloc(bgbgc_cblk_map, n*sizeof(void *));
	memset(bgbgc_cblk_data+i, 0, (n-i)*sizeof(bgbgc_consref *));
	memset(bgbgc_cblk_map+i, 0, (n-i)*sizeof(void *));

	bgbgc_cblk_max=n;
	bgbgc_cblk_num=i+1;

	bgbgc_cblk_data[i]=malloc(BGBGC_CBLK_CELLS*2*sizeof(bgbgc_consref));
	bgbgc_cblk_map[i]=malloc(BGBGC_CBLK_CELLS);
	memset(bgbgc_cblk_data[i], 0, BGBGC_CBLK_CELLS*2*sizeof(bgbgc_consref));
	memset(bgbgc_cblk_map[i], 0, BGBGC_CBLK_CELLS);

	bgbgc_total_cons+=BGBGC_CBLK_CELLS;
	bgbgc_cblk_rov_blk=i;
	bgbgc_cblk_rov_cell=0;
}

void *BGBGC_AllocConsInner()
{
	int i, j;

	i=bgbgc_cblk_rov_blk;
	j=bgbgc_cblk_rov_cell;
	for(; j<BGBGC_CBLK_CELLS; j++)
		if(!bgbgc_cblk_map[i][j])
	{
		bgbgc_cblk_map[i][j]=1;
		bgbgc_used_cons++;

		bgbgc_cblk_rov_blk=i;
		bgbgc_cblk_rov_cell=j+1;
		return(((bgbgc_consref *)bgbgc_cblk_data[i])+j*2);
	}
	bgbgc_cblk_rov_cell=0;

	i=bgbgc_cblk_rov_blk;
	while(1)
	{
		for(j=0; j<BGBGC_CBLK_CELLS; j++)
			if(!bgbgc_cblk_map[i][j])
		{
			bgbgc_cblk_map[i][j]=1;
			bgbgc_used_cons++;

			bgbgc_cblk_rov_blk=i;
			bgbgc_cblk_rov_cell=j+1;

			return(((bgbgc_consref *)bgbgc_cblk_data[i])+j*2);
		}
		i=(i+1)%bgbgc_cblk_num;
		if(i==bgbgc_cblk_rov_blk)
			break;
	}

	return(NULL);
}


BGBGC_API void *BGBGC_AllocCons()
{
	void *p;
	int t, r;

	thLockFastMutex((void *)bgbgc_alloc_mutex);

	//temp: deal with bad conses.
//	if(bgbgc_conspool && !BGBGC_ConsP_I(bgbgc_conspool))
//		{ bgbgc_conspool=NULL; }

	if(bgbgc_conspool)
	{
		p=bgbgc_conspool;
//		bgbgc_conspool=((void **)p)[1];
		bgbgc_conspool=*(void **)p;
		thUnlockFastMutex((void *)bgbgc_alloc_mutex);
		return(p);
	}


	p=BGBGC_AllocConsInner();
	if(p)
	{
		thUnlockFastMutex((void *)bgbgc_alloc_mutex);
		return(p);
	}

	//if below the limit, expand heap
	if(bgbgc_total_cons<bgbgc_size_lim_cons)
	{
		BGBGC_AllocCBlk();
		p=BGBGC_AllocConsInner();
		thUnlockFastMutex((void *)bgbgc_alloc_mutex);
		return(p);
	}

	t=clock();
	//if last gc too recent, don't bother
//	if((t-bgbgc_lastgc)>(30*CLOCKS_PER_SEC))
	if(!BGBGC_CheckNoGC())
	{
		BGBGC_Collect();
		bgbgc_lastgc=clock();

		r=(100*bgbgc_used_cons)/bgbgc_total_cons;

		//heap mostly full, expand
		if(r>75)
		{
			BGBGC_AllocCBlk();
			p=BGBGC_AllocConsInner();
			thUnlockFastMutex((void *)bgbgc_alloc_mutex);
			return(p);
		}

		p=BGBGC_AllocConsInner();
		if(p)
		{
			thUnlockFastMutex((void *)bgbgc_alloc_mutex);
			return(p);
		}
	}

	BGBGC_AllocCBlk();
	p=BGBGC_AllocConsInner();
	thUnlockFastMutex((void *)bgbgc_alloc_mutex);
	return(p);
}

BGBGC_API void BGBGC_FreeCons(void *p)
{
	BGBGC_TryMarkSafe(p);

//	if(!BGBGC_ConsP_I(p))
//		{ *(int *)-1=-1; }

	((bgbgc_consref *)p)[0]=0xF5EEC095BAADC095ULL;
	((bgbgc_consref *)p)[1]=0xF5EEC095BAADC095ULL;

	thLockFastMutex((void *)bgbgc_alloc_mutex);
//	((void **)p)[1]=bgbgc_conspool;
	*(void **)p=bgbgc_conspool;
	bgbgc_conspool=p;
	thUnlockFastMutex((void *)bgbgc_alloc_mutex);
}

int BGBGC_CheckCons(void *ptr)
{
//	void *q, *r;
	bgbgc_consref *p, *q, *r;
	int i;

	p=(bgbgc_consref *)ptr;
	for(i=0; i<bgbgc_cblk_num; i++)
	{
//		q=bgbgc_cblk_data[i];
//		r=((void **)q)+(2*BGBGC_CBLK_CELLS);
//		if((p>=q) && (p<r))return(i);

		q=(bgbgc_consref *)(bgbgc_cblk_data[i]);
		if(p<q)continue;
		r=q+(2*BGBGC_CBLK_CELLS);
		if(p>=r)continue;
		return(i);
	}
	return(-1);
}

int BGBGC_ConsP_I(void *p)
{
	return(BGBGC_CheckCons(p)>=0);
}

BGBGC_API int BGBGC_ConsP(void *p)
{
	if(BGBGC_CheckCons(p)>=0)
		return(1);
	if(p==((void *)(nlint)0xF5EEC095BAADC095ULL))
		{ *(int *)-1=-1; }
	return(0);
}


int BGBGC_MarkCons(void *p, int i)
{
	bgbgc_consref *pp;
	int j, k;

	j=(((byte *)p)-((byte *)bgbgc_cblk_data[i]))/(2*sizeof(bgbgc_consref));

	k=bgbgc_cblk_map[i][j];
	if(k&2)return(0);
	if(!(k&1))return(-1);

	bgbgc_cblk_map[i][j]=k|2;

	pp=((bgbgc_consref *)bgbgc_cblk_data[i])+(2*j);

#ifdef BGBGC_USEPRECISE
	BGBGC_MarkPrecise((void *)((nlint)(pp[0])));
	BGBGC_MarkPrecise((void *)((nlint)(pp[1])));
#endif

#ifndef BGBGC_USEPRECISE
	BGBGC_Mark((void *)((nlint)(pp[0])));
	BGBGC_Mark((void *)((nlint)(pp[1])));
#endif

	return(0);
}

#ifdef BGBGC_USEPRECISE
int BGBGC_MarkConsPrecise(int idx)
{
	bgbgc_consref *pp;
	int i, j, k;

	i=idx>>BGBGC_CBLK_BITS;
	j=idx&((1<<BGBGC_CBLK_BITS)-1);

	k=bgbgc_cblk_map[i][j];
	if(k&2)return(0);
	if(!(k&1))return(-1);

	bgbgc_cblk_map[i][j]=k|2;

	pp=((bgbgc_consref *)bgbgc_cblk_data[i])+(2*j);
	BGBGC_MarkPrecise(pp[0]);
	BGBGC_MarkPrecise(pp[1]);

	return(0);
}
#endif

#ifdef BGBGC_USEPRECISE
int BGBGC_IncCons(void *p)
{
	int i, j, k;

	k=((byte *)p)-((byte *)bgbgc_precise_cbase);
	i=k>>BGBGC_CBLK_BITS;
	j=k&((1<<BGBGC_CBLK_BITS)-1);

	k=bgbgc_cblk_map[i][j];
	switch((k>>4)&7)
	{
	case 0: break;	//conservative
	case 1: break;	//atomic (conservative)
	case 2: break;	//defiled
	case 3: case 4: case 5: case 6:
		bgbgc_cblk_map[i][j]=k+16; break;
	case 7: break;	//many
	}
	return(0);
}

int BGBGC_DecCons(void *p)
{
	int i, j, k;

	k=((byte *)p)-((byte *)bgbgc_precise_cbase);
	i=k>>BGBGC_CBLK_BITS;
	j=k&((1<<BGBGC_CBLK_BITS)-1);

	k=bgbgc_cblk_map[i][j];
	switch((k>>4)&7)
	{
	case 0: break;	//conservative
	case 1: break;	//atomic (conservative)
	case 2: break;	//defiled
	case 3: case 4:
//		BGBGC_Free(p);
		BGBGC_FreeCons(((void **)bgbgc_cblk_data[i])+j*2);
		break;
	case 5: case 6:
		bgbgc_cblk_map[i][j]=k-16;
		break;
	case 7: break;	//many
	}
	return(0);
}

int BGBGC_SafeDecCons(void *p)
{
	int i, j, k;

	k=((byte *)p)-((byte *)bgbgc_precise_cbase);
	i=k>>BGBGC_CBLK_BITS;
	j=k&((1<<BGBGC_CBLK_BITS)-1);

	k=bgbgc_cblk_map[i][j];
	switch((k>>4)&7)
	{
	case 0: break;	//conservative
	case 1: break;	//atomic (conservative)
	case 2: break;	//defiled
	case 3: break;
	case 4: case 5: case 6:
		bgbgc_cblk_map[i][j]=k-16;
		break;
	case 7: break;	//many
	}
	return(0);
}

void *BGBGC_DefileCons(void *p)
{
	int i, j, k;

	k=((byte *)p)-((byte *)bgbgc_precise_cbase);
	i=k>>BGBGC_CBLK_BITS;
	j=k&((1<<BGBGC_CBLK_BITS)-1);

	k=bgbgc_cblk_map[i][j];
	switch((k>>4)&7)
	{
	case 0: break;	//conservative
	case 1: break;	//atomic (conservative)
	case 2: break;	//defiled
	case 3: case 4: case 5: case 6: case 7:
		bgbgc_cblk_map[i][j]=(k&0x8F)|0x20;
		break;
	}

	return(((void **)bgbgc_cblk_data[i])+j*2);
}

void *BGBGC_UnDefileCons(void *p)
{
	void *q;
	int i, j;

	i=BGBGC_CheckCons(p);
	if(i<0)return(NULL);

	j=(((byte *)p)-((byte *)bgbgc_cblk_data[i]))/(2*sizeof(void *));
	q=(void *)(bgbgc_precise_cbase+(i<<BGBGC_CBLK_BITS)+j);
	return(q);
}

void *BGBGC_PreciseGetConsPtr(void *p)
{
	int i, j, k;

	k=((byte *)p)-((byte *)bgbgc_precise_cbase);
	i=k>>BGBGC_CBLK_BITS;
	j=k&((1<<BGBGC_CBLK_BITS)-1);
	return(((void **)bgbgc_cblk_data[i])+(j*2));
}

void *BGBGC_AllocConsPrecise()
{
	void *p, *q;
	int i, j;

	p=BGBGC_AllocCons();
//	((void **)p)[0]=NULL;
//	((void **)p)[1]=NULL;

	((bgbgc_consref *)p)[0]=0;
	((bgbgc_consref *)p)[1]=0;

	i=BGBGC_CheckCons(p);
	if(i<0)return(NULL);

	j=(((byte *)p)-((byte *)bgbgc_cblk_data[i]))/(2*sizeof(bgbgc_consref));
	q=(void *)(bgbgc_precise_cbase+(i<<BGBGC_CBLK_BITS)+j);
	return(q);
}
#endif

int BGBGC_SweepCons()
{
	int i, j, k;

	bgbgc_used_cons=0;
	bgbgc_freed_cons=0;

	for(i=0; i<bgbgc_cblk_num; i++)
	{
		for(j=0; j<BGBGC_CBLK_CELLS; j++)
		{
			k=bgbgc_cblk_map[i][j];
			if(!k)continue;
			if(k&2)
			{
				bgbgc_used_cons++;
				k&=(~2);
			}else
			{
				bgbgc_freed_cons++;
				k=0;
			}
			bgbgc_cblk_map[i][j]=k;
		}
	}

	return(0);
}

