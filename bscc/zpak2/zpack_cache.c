#include <zpack.h>

ZPACK_CacheFile *ZPACK_GetCacheFragmented(ZPACK_Context *ctx, char *name);
void ZPACK_CommitCacheFragmented(ZPACK_Context *ctx, ZPACK_CacheFile *cur);

void ZPACK_ClearCacheMost(ZPACK_Context *ctx);

ZPACK_CacheFile *ZPACK_AllocCache(ZPACK_Context *ctx)
{
	ZPACK_CacheFile *tmp;
	int i;

	if(!ctx->cachebuf)
	{
		ctx->cachebuf=malloc(1024*sizeof(ZPACK_CacheFile));
		memset(ctx->cachebuf, 0, 1024*sizeof(ZPACK_CacheFile));
		ctx->cachefree=NULL;

		for(i=0; i<1024; i++)
		{
			ctx->cachebuf[i].next=ctx->cachefree;
			ctx->cachefree=&ctx->cachebuf[i];
		}
	}

	if(!ctx->cachefree)
	{
		//no free entries, force files to disk
		return(NULL);	//for now, fail
	}

	tmp=ctx->cachefree;
	ctx->cachefree=tmp->next;

	memset(tmp, 0, sizeof(ZPACK_CacheFile));
	return(tmp);
}

void ZPACK_FreeCache(ZPACK_Context *ctx, ZPACK_CacheFile *cur)
{
	memset(cur, 0, sizeof(ZPACK_CacheFile));	//sanitize
	cur->next=ctx->cachefree;
	ctx->cachefree=cur;
}


void ZPACK_MoveFrontMRU(ZPACK_Context *ctx, ZPACK_CacheFile *cur)
{
	if(cur->prev)	//relink in mru list
	{
		cur->prev->next=cur->next;
		if(cur->next)cur->next->prev=cur->prev;

		cur->next=ctx->mru;
		if(ctx->mru)ctx->mru->prev=cur;
		cur->prev=NULL;
		ctx->mru=cur;
	}
}

ZPACK_CacheFile *ZPACK_LookupCache(ZPACK_Context *ctx, char *name)
{
	ZPACK_CacheFile *cur;
	char *s;
	int i, j, hi;

	hi=ZPACK_HashName(name);
	cur=ctx->hash[hi];
	while(cur)
	{
		if(!zpack_stricmp(cur->path, name))break;
		cur=cur->hnext;
	}

	return(cur);
}

ZPACK_CacheFile *ZPACK_GetCache(ZPACK_Context *ctx, char *name)
{
	ZPACK_CacheFile *cur;
	byte *buf;
	char *s;
	int i, j, k, hi;

	cur=ZPACK_LookupCache(ctx, name);
	if(cur)
	{
		ZPACK_MoveFrontMRU(ctx, cur);
		return(cur);
	}

	//ok, file is not cached, so do so

	hi=ZPACK_HashName(name);
	i=ZPACK_LocateFile(ctx, name);
	if(i<0)	//file does not exist in image
	{
//		printf("No File: %s\n", name);
		return(NULL);
	}

	j=ctx->root[i].flags;


	if(j&2)
	{
//		printf("Cache: Is Directory: %s\n", name);
		return(NULL);	//file is a directory
	}

	cur=ZPACK_AllocCache(ctx);
	cur->path=strdup(name);

	cur->next=ctx->mru;
	if(ctx->mru)ctx->mru->prev=cur;
	cur->prev=NULL;
	ctx->mru=cur;

	cur->hnext=ctx->hash[hi];
	ctx->hash[hi]=cur;


	cur->offs=ctx->root[i].offset;
	cur->size=ctx->root[i].csize;
	cur->lsize=ctx->root[i].usize;
	cur->method=ctx->root[i].method;

	cur->bsize=(cur->lsize+6144)&(~4095);
	cur->buffer=malloc(cur->bsize);

	if(!cur->method)
	{
		fseek(ctx->fd, cur->offs, 0);
		fread(cur->buffer, 1, cur->lsize, ctx->fd);
	}

	if((cur->method==8) || (cur->method==9))
	{
		buf=malloc(cur->size);
		fseek(ctx->fd, cur->offs, 0);
		fread(buf, 1, cur->size, ctx->fd);
		PDUNZ_DecodeStream(buf, cur->buffer,
			cur->size, cur->lsize);
		free(buf);

		j=~ZPACK_CRC32(cur->buffer, cur->lsize, (u32)-1);
		k=ctx->root[i].crc32;
		if(j!=k)
		{
			printf("ZPACK_GetCache: Decode CRC Problem "
				"(%08X!=%08X) %08X\n", j, k, ~j);
			free(cur->buffer);
			cur->buffer=NULL;
		}
	}

	ZPACK_ClearCacheMost(ctx);

	return(cur);
}

void ZPACK_ReleaseCache(ZPACK_Context *ctx, ZPACK_CacheFile *cache)
{
	ZPACK_CacheFile *cur, *lst;
	char *s;
	int i, j, hi;

//	printf("Release Cache: %s %p\n", cache->path, cache);

	hi=ZPACK_HashName(cache->path);
	cur=ctx->hash[hi]; lst=NULL;
	while(cur)
	{
		if(cur==cache)
		{
			if(cur->prev)	//unlink from mru list
			{
				cur->prev->next=cur->next;
				if(cur->next)cur->next->prev=cur->prev;
			}else
			{
				if(cur->next)cur->next->prev=NULL;
				ctx->mru=cur->next;
			}

			if(lst)lst->hnext=cur->hnext;
				else ctx->hash[hi]=cur->hnext;

			if(cur->buffer)free(cur->buffer);
			free(cur->path);
			ZPACK_FreeCache(ctx, cur);
			break;
		}
		lst=cur; cur=cur->hnext;
	}

	if(!cur)
	{
		printf("ZPACK_ReleaseCache: Cache not present in hash list\n");
	}
}

void ZPACK_DropCache(ZPACK_Context *ctx, char *name)
{
	ZPACK_CacheFile *cur, *lst, *nxt;
	char *s;
	int i, j, hi;

	hi=ZPACK_HashName(name);
	cur=ctx->hash[hi]; lst=NULL;
	while(cur)
	{
		nxt=cur->hnext;

		if(!zpack_stricmp(cur->path, name))
		{
			if(cur->prev)	//unlink from mru list
			{
				cur->prev->next=cur->next;
				if(cur->next)cur->next->prev=cur->prev;
			}else
			{
				if(cur->next)cur->next->prev=NULL;
				ctx->mru=cur->next;
			}

			if(lst)lst->hnext=cur->hnext;
				else ctx->hash[hi]=cur->hnext;

			if(cur->buffer)free(cur->buffer);
			free(cur->path);
			ZPACK_FreeCache(ctx, cur);
		}

		lst=cur; cur=nxt;
	}

	return;
}


void ZPACK_CommitCache(ZPACK_Context *ctx, ZPACK_CacheFile *cur)
{
	byte *buf;
	int i, j, k, l, sz, sz2;
	int md, mt;

	if(!cur->dirty)return;
	if(ctx->flags&1)
	{
		printf("ZPACK: Fail Commit, R/O\n");
		return;
	}

//	if(cur->lsize>=64)
	if(cur->lsize>=256)
	{
		cur->method=0;
		buf=malloc(2*cur->lsize);
//		sz=PDZ2_EncodeStream(cur->buffer, buf,
//			cur->lsize, 2*cur->lsize);
//		sz=PDZ2_EncodeStream32Lvl(cur->buffer, buf,
//			cur->lsize, 2*cur->lsize, 8);
		sz=PDZ2_EncodeStream32Lvl(cur->buffer, buf,
			cur->lsize, 2*cur->lsize, 5);
//		if((sz>=0) && (sz<(cur->lsize*0.9)))
		if((sz>=0) && (sz<(cur->lsize*0.8)))
			cur->method=8;

		if(sz>=(2*cur->lsize))
		{
			printf("Compress Overflow\n");
			*(int *)-1=-1;
		}

		if(!cur->method)
		{
			free(buf);
			buf=cur->buffer;
			sz=cur->lsize;
		}
	}else
	{
		cur->method=0;
		buf=cur->buffer;
		sz=cur->lsize;
	}

	sz2=sz+30+strlen(cur->path);

#if 0
	if(!ZPACK_CheckSpan(ctx, cur->hoffs, sz2))
	{
		ZPACK_DeleteSpan(ctx, cur->hoffs);
		cur->hoffs=ZPACK_PrevSpanEnd(ctx, cur->hoffs);
	}
#endif

	if(!ZPACK_CheckSpan(ctx, cur->hoffs, sz2))
	{
		fseek(ctx->fd, cur->hoffs, 0);
		ZPACK_WriteU32(ctx->fd, FCC_ZZ34);

		ZPACK_DeleteSpan(ctx, cur->hoffs);
		cur->hoffs=ZPACK_FindMinSpan(ctx, sz2);
		if((cur->hoffs+sz2)>ctx->eof)ctx->eof=cur->hoffs+sz2;
		cur->offs=cur->hoffs+30+strlen(cur->path);
	}

	cur->size=sz;

//	printf("Commit Cache @ %08X..%08X sz %d\n",
//		cur->hoffs, cur->hoffs+sz2-1, sz2);

	//make sure span is up to date
	ZPACK_InsertSpan(ctx, cur->hoffs, sz2, cur->path);

	fseek(ctx->fd, 0, 2);
	i=ftell(ctx->fd);
	if(i<cur->hoffs)ZPACK_WritePad(ctx->fd, i-cur->offs);
	fseek(ctx->fd, cur->hoffs, 0);

	md=ZPACK_GetDate();
	mt=ZPACK_GetTime();

	j=~ZPACK_CRC32(cur->buffer, cur->lsize, (u32)-1);
	k=strlen(cur->path);

	l=10;
	if(cur->method==8)l=20;
	if(cur->method==9)l=21;

	//write local file header
	ZPACK_WriteU32(ctx->fd, FCC_PK34);
	ZPACK_WriteU16(ctx->fd, l);		//ver needed to extract
	ZPACK_WriteU16(ctx->fd, 0);		//flags
	ZPACK_WriteU16(ctx->fd, cur->method);	//method
	ZPACK_WriteU16(ctx->fd, mt);		//time
	ZPACK_WriteU16(ctx->fd, md);		//date
	ZPACK_WriteU32(ctx->fd, j);		//crc
	ZPACK_WriteU32(ctx->fd, cur->size);	//csize
	ZPACK_WriteU32(ctx->fd, cur->lsize);	//usize
	ZPACK_WriteU16(ctx->fd, k);		//name len
	ZPACK_WriteU16(ctx->fd, 0);		//extra len

	fwrite(cur->path, 1, k, ctx->fd);


	fwrite(buf, 1, cur->size, ctx->fd);

	cur->dirty=0;

	if(cur->method)free(buf);


	i=ZPACK_LocateFile(ctx, cur->path);
	if(i<0)
	{
		printf("Commit Cache: can't locate %s\n", cur->path);
		*(int *)-1=-1;
		return;
	}

	j=ctx->root[i].flags;
	ctx->root[i].flags=j&(~4);

	j=~ZPACK_CRC32(cur->buffer, cur->lsize, (u32)-1);

	ctx->root[i].hoffs=cur->hoffs;
	ctx->root[i].offset=cur->offs;
	ctx->root[i].csize=cur->size;
	ctx->root[i].usize=cur->lsize;
	ctx->root[i].method=cur->method;

	ctx->root[i].date=md;
	ctx->root[i].time=mt;

	ctx->root[i].crc32=j;

	fflush(ctx->fd);
}

void ZPACK_CommitCacheAll(ZPACK_Context *ctx)
{
	ZPACK_CacheFile *cur;

	cur=ctx->mru;
	while(cur)
	{
		ZPACK_CommitCache(ctx, cur);
		cur=cur->next;
	}
}

void ZPACK_ClearCacheAll(ZPACK_Context *ctx)
{
	ZPACK_CacheFile *cur, *nxt;

	cur=ctx->mru;
	while(cur)
	{
		nxt=cur->next;
		ZPACK_CommitCache(ctx, cur);
		ZPACK_ReleaseCache(ctx, cur);
		cur=nxt;
	}
}

void ZPACK_ClearCacheMost(ZPACK_Context *ctx)
{
	ZPACK_CacheFile *cur, *nxt;
	int i, j;

	i=0; j=0;
	cur=ctx->mru;
	while(cur)
	{
		i++; j+=cur->lsize;

		if((i<8) || ((i<512) && (j<(1<<24))))
		{
//			printf("ClearCacheMose: skip %s\n", cur->path);
			cur=cur->next;
			continue;
		}

		nxt=cur->next;
		ZPACK_CommitCache(ctx, cur);
		ZPACK_ReleaseCache(ctx, cur);
		cur=nxt;
	}
}
