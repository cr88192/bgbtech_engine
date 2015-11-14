#include <zpack.h>

ZPACK_API ZPACK_FILE *ZPACK_OpenFile(ZPACK_Context *ctx, char *name, char *mode)
{
	ZPACK_CacheFile *cf;
	ZPACK_FILE *tmp;
	int i;

	while(*name && (*name=='/'))name++;
	if(!(*name))return(NULL);

	if((ctx->flags&1) && ((*mode=='w') || (*mode=='a')))
		return(NULL);

	if(*mode=='r')
	{
		cf=ZPACK_GetCache(ctx, name);
		if(!cf)return(NULL);

		tmp=malloc(sizeof(ZPACK_FILE));
		memset(tmp, 0, sizeof(ZPACK_FILE));

		tmp->ctx=ctx;
		tmp->cache=cf;
		tmp->offs=0;

		return(tmp);
	}

	if(*mode=='w')
	{
		ZPACK_DropCache(ctx, name);
		i=ZPACK_CreateFile(ctx, name);
		if(i<0)
		{
			printf("Can't create entry for %s\n", name);
			return(NULL);
		}

		cf=ZPACK_GetCache(ctx, name);
		if(!cf)
		{
			printf("Can't get cache for %s\n", name);
			return(NULL);
		}

		tmp=malloc(sizeof(ZPACK_FILE));
		memset(tmp, 0, sizeof(ZPACK_FILE));

		tmp->ctx=ctx;
		tmp->cache=cf;
		tmp->offs=0;

		return(tmp);
	}

	if(*mode=='a')
	{
		cf=ZPACK_GetCache(ctx, name);
		if(!cf)
		{
			i=ZPACK_CreateFile(ctx, name);
			if(i<0)
			{
				printf("Can't create entry for %s\n", name);
				return(NULL);
			}

			cf=ZPACK_GetCache(ctx, name);
			if(!cf)
			{
				printf("Can't get cache for %s\n", name);
				return(NULL);
			}
		}

		tmp=malloc(sizeof(ZPACK_FILE));
		memset(tmp, 0, sizeof(ZPACK_FILE));

		tmp->ctx=ctx;
		tmp->cache=cf;
		tmp->offs=cf->lsize;

		return(tmp);
	}

	return(NULL);
}

ZPACK_API int ZPACK_Close(ZPACK_FILE *fd)
{
	ZPACK_CommitCache(fd->ctx, fd->cache);
//	ZPACK_CommitRoot(fd->ctx);
//	fflush(fd->ctx->fd);

//	fclose(fd->ctx->fd);
//	fd->ctx->fd=fopen(fd->ctx->name, "r+b");

	free(fd);
}

ZPACK_API int ZPACK_Flush(ZPACK_FILE *fd)
{
	ZPACK_CommitCache(fd->ctx, fd->cache);
//	ZPACK_CommitRoot(fd->ctx);
//	fflush(fd->ctx->fd);
}

ZPACK_API int ZPACK_Read(ZPACK_FILE *fd, byte *buf, int sz)
{
	int i, j;

	ZPACK_MoveFrontMRU(fd->ctx, fd->cache);

	i=fd->cache->lsize-fd->offs;
	if(sz<i)i=sz;
	if(i<0)i=0;

	memcpy(buf, fd->cache->buffer+fd->offs, i);
	fd->offs+=i;
	return(i);
}

ZPACK_API int ZPACK_Write(ZPACK_FILE *fd, byte *buf, int sz)
{
	int i, j, k;

	ZPACK_MoveFrontMRU(fd->ctx, fd->cache);
	fd->cache->dirty=1;

	//need to expand buffer
	if((fd->offs+sz)>=fd->cache->bsize)
	{
		i=fd->cache->bsize;

		j=i; k=fd->offs+sz;
		while(j<=k)j=j+(j>>1);

//		j=(fd->offs+sz+6144)&(~4095);
		fd->cache->bsize=j;

		fd->cache->buffer=realloc(fd->cache->buffer, j);
		memset(fd->cache->buffer+i, 0, j-i);
	}

	if((fd->offs+sz)>fd->cache->lsize)
	{
		fd->cache->lsize=fd->offs+sz;
	}

	memcpy(fd->cache->buffer+fd->offs, buf, sz);
	fd->offs+=sz;
	return(sz);
}

ZPACK_API u32 ZPACK_Seek(ZPACK_FILE *fd, u32 offs, int rel)
{
	switch(rel)
	{
	case 0:
		fd->offs=offs;
		break;
	case 1:
		fd->offs+=offs;
		break;
	case 2:
		fd->offs=fd->cache->lsize+offs;
		break;
	default:
		break;
	}

	return(fd->offs);
}

ZPACK_API u32 ZPACK_Tell(ZPACK_FILE *fd)
{
	return(fd->offs);
}

ZPACK_API int ZPACK_Eof(ZPACK_FILE *fd)
{
	return(fd->offs>=fd->cache->lsize);
}
