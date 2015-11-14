#include <bgbgc.h>

BGBGC_LocalHeapInfo *bgbgc_localheap_root;

BGBGC_LocalHeapInfo *BGBGC_CreateLocalHeap()
{
	BGBGC_LocalHeapInfo *tmp;
	tmp=malloc(sizeof(BGBGC_LocalHeapInfo));
	memset(tmp, 0, sizeof(BGBGC_LocalHeapInfo));
	return(tmp);
}

void BGBGC_DestroyLocalHeap(BGBGC_LocalHeapInfo *ctx)
{
	void *p, *q;
	int i;

	for(i=0; i<ctx->nblock; i++)
		free(ctx->block[i]);
	
	p=ctx->list;
	while(p)
	{
		q=*(void **)p;
		free(p);
		p=q;
	}
	
	free(ctx);
}

int BGBGC_CheckPointerLocalHeap(BGBGC_LocalHeapInfo *ctx, void *ptr)
{
	byte *base, *end, *p;
	int i;
	
	p=ptr;
	for(i=0; i<ctx->nblock; i++)
	{
		base=ctx->block[i];
		end=base+BGBGC_CHKSIZE;
		if((p>=base) && (p<end))
			return(1);
	}
	return(0);
}

void BGBGC_ClearLocalHeap(BGBGC_LocalHeapInfo *ctx)
{
	int i;

	if(ctx->rov)
	{
		printf("BGBGC_CleanupAll: Used %d block(s)\n",
			ctx->nblock);

		for(i=1; i<ctx->nblock; i++)
			free(ctx->block[i]);
		ctx->nblock=1;

		ctx->rov=ctx->block[0];
		ctx->srov=ctx->block[0];
		ctx->erov=ctx->srov+BGBGC_CHKSIZE;
	}
}

void *BGBGC_LocalAllocRaw(BGBGC_LocalHeapInfo *ctx, int sz)
{
	void *p;
	int i, j, k, n;

	if(sz>=16384)
	{
		sz+=2*sizeof(void *);
		p=malloc(sz);
		memset(p, 0, sz);
		*(void **)p=ctx->list;
		ctx->list=p;
		p=(void *)(((void **)p)+2);
		return(p);
	}

	sz=(sz+BGBGC_CELLPAD)&(~BGBGC_CELLPAD);

	if(!ctx->rov)
	{
		for(i=0; i<256; i++)
			ctx->block[i]=NULL;

		i=ctx->nblock++;
		ctx->srov=malloc(BGBGC_CHKSIZE);
		ctx->erov=ctx->srov+BGBGC_CHKSIZE;
		ctx->rov=ctx->srov;
		ctx->block[i]=ctx->rov;
		ctx->block_map[i]=malloc(BGBGC_CHKSIZE>>BGBGC_MAPSHIFT);
	}

	if((ctx->rov+sz)>=(ctx->erov-4096))
	{
		n=ctx->nblock++;
		if(!ctx->block[n])
		{
			printf("BGBCC: Expand\n");
			ctx->block[n]=malloc(BGBGC_CHKSIZE);
		}
		
		ctx->srov=ctx->block[n];
		ctx->erov=ctx->srov+BGBGC_CHKSIZE;
		ctx->rov=ctx->srov;
	}

	n=ctx->nblock-1;
	i=(ctx->rov-ctx->srov)>>BGBGC_MAPSHIFT;
	j=sz>>BGBGC_CELLSHIFT;
	k=i+j;
	ctx->block_map[n][i]=1;
	for(i=i+1; i<k; i++)
		ctx->block_map[n][i]=2;

	p=ctx->rov;
	ctx->rov+=(sz+15)&(~15);
	memset(p, 0, sz);

	return(p);
}

void *BGBGC_LocalTyAlloc(BGBGC_LocalHeapInfo *ctx, char *type, int sz)
{
	BGBGC_ObjInfo *head;
	BGBGC_ObjType *ty;
	void *p;
	int i, n, sz1, sz2, szh;

	szh=(sizeof(BGBGC_ObjInfo)+15)&(~15);
	sz1=szh+sz;
	ty=BGBGC_FetchType(type);

	p=BGBGC_LocalAllocRaw(ctx, sz1);

	head=(BGBGC_ObjInfo *)p;
	head->type=bcg2_strdup(ty);
	head->size=sz;
	p=(byte *)(((byte *)p)+hsz);

	return(p);
}

void *BGBGC_LocalMalloc(BGBGC_LocalHeapInfo *ctx, int sz)
{
	BGBGC_LocalTyAlloc(ctx, ty, sz);
}

void *BGBGC_LocalRealloc(BGBGC_LocalHeapInfo *ctx, void *ptr, int sz)
{
	BGBGC_LocalTyAlloc(ctx, ty, sz);
}
