#include <zpack.h>

ZPACK_API ZPACK_DIR *ZPACK_OpenDir(ZPACK_Context *ctx, char *name)
{
	ZPACK_DIR *tmp;
	int i, j;

	while(*name=='/')name++;
	if(!*name)
	{
		tmp=malloc(sizeof(ZPACK_DIR));
		tmp->ctx=ctx;
		tmp->base=0;
		tmp->pos=0;
		return(tmp);
	}

	i=ZPACK_LocateFile(ctx, name);
	if(i<0)return(NULL);

	j=ctx->root[i].flags;
	if(!(j&2))return(NULL);

	j=ctx->root[i].crc32;

	tmp=malloc(sizeof(ZPACK_DIR));
	tmp->ctx=ctx;
	tmp->base=j;
	tmp->pos=j;

	if(!j)
	{
		//dir has no contents
		tmp->base=-1;
		tmp->pos=-1;
	}

	return(tmp);
}

ZPACK_API void ZPACK_CloseDir(ZPACK_DIR *dir)
{
	free(dir);
}

ZPACK_API void ZPACK_RewindDir(ZPACK_DIR *dir)
{
	dir->pos=dir->base;
}

ZPACK_API char *ZPACK_ReadDir(ZPACK_DIR *dir)
{
	if(dir->pos<0)return(NULL);

	memset(dir->nbuf, 0, 33);
	memcpy(dir->nbuf, dir->ctx->root[dir->pos].name, 32);

	if(!dir->nbuf[0])
	{
		if(!dir->pos)
		{
			dir->pos=dir->ctx->root[dir->pos].chain;
			if(!dir->pos)dir->pos=-1;
			return(ZPACK_ReadDir(dir));
		}

		dir->pos=-1;
		return(NULL);
	}

	if(dir->nbuf[0]=='$')
	{
		dir->pos=dir->ctx->root[dir->pos].chain;
		if(!dir->pos)dir->pos=-1;
		return(ZPACK_ReadDir(dir));
	}

	dir->pos=dir->ctx->root[dir->pos].chain;
	if(!dir->pos)dir->pos=-1;

	return(dir->nbuf);
}

ZPACK_API int ZPACK_MkDir(ZPACK_Context *ctx, char *name)
{
	char buf[256];
	int i;

	sprintf(buf, "%s/", name);
	i=ZPACK_LocateFile(ctx, buf);
	if(!(i<0))return(-1);

	printf("Try Create %s\n", buf);

	i=ZPACK_CreateFile(ctx, buf);
	if(i<0)return(-1);
	return(0);
}
