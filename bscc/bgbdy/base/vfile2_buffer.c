#include <bgbgc.h>
#include <bgbdy.h>

int vfbuf_vfread(void *buf, int s1, int s2, VFILE *fd)
{
	VFZIP_FileCtx *ctx;
	int i;

	ctx=fd->data;
	i=s1*s2;
	if((ctx->offs+i)>ctx->ent->usize)
		i=ctx->ent->usize-ctx->offs;
	i/=s1;

//	VFZIP_ReCacheFile(ctx->ctx, ctx->ent);
	ctx->ent->iocnt=256;

	memcpy(buf, ctx->ent->buffer+ctx->offs, i*s1);
	ctx->offs+=i*s1;
	return(i);
}

int vfbuf_vfwrite(void *buf, int s1, int s2, VFILE *fd)
{
	VFZIP_FileCtx *ctx;
	int i;

	ctx=fd->data;
	ctx->ctx->dirty=1;

	i=s1*s2;
	if((ctx->offs+i)>ctx->ent->bsize)
	{
		ctx->ent->bsize=ctx->offs+i+4096;
		ctx->ent->bsize&=~4095;
		ctx->ent->buffer=realloc(ctx->ent->buffer, ctx->ent->bsize);
	}
	if((ctx->offs+i)>ctx->ent->usize)
		ctx->ent->usize=ctx->offs+i;

	memcpy(ctx->ent->buffer+ctx->offs, buf, i);
	ctx->offs+=i;
	return(i/s1);
}

int vfbuf_vfgetc(VFILE *fd)
{
	VFZIP_FileCtx *ctx;
	int i;

	ctx=fd->data;

	if(ctx->offs>=ctx->ent->usize)
		return(-1);

	VFZIP_ReCacheFile(ctx->ctx, ctx->ent);
	ctx->ent->iocnt=256;

	i=ctx->ent->buffer[ctx->offs];
	ctx->offs++;
	return(i);
}

int vfbuf_vfputc(int c, VFILE *fd)
{
	int i;
	byte b;

	b=c;
	vfbuf_vfwrite(&b, 1, 1, fd);
	return(0);
}

int vfbuf_vfflush(VFILE *fd)
{
	VFZIP_FileCtx *ctx;
	ctx=fd->data;

	//flushing means: rewrite archive...
	if(!ctx->ctx->fd && ctx->ctx->dirty)
	{
		VFZIP_WriteZip(ctx->ctx, ctx->ctx->name);
		ctx->ctx->dirty=0;
	}

	return(0);
}

vfoffs_t vfbuf_vfseek(VFILE *fd, vfoffs_t rel, int pos)
{
	VFZIP_FileCtx *ctx;
	int i;

	ctx=fd->data;

	switch(pos)
	{
	case 0:
		ctx->offs=rel;
		break;
	case 1:
		ctx->offs+=rel;
		break;
	case 2:
		ctx->offs+=ctx->ent->usize+rel;
		break;
	default:
		break;
	}
	return(ctx->offs);
}

vfoffs_t vfbuf_vftell(VFILE *fd)
{
	VFZIP_FileCtx *ctx;
	ctx=fd->data;
	return(ctx->offs);
}

int vfbuf_vfeof(VFILE *fd)
{
	VFZIP_FileCtx *ctx;
	ctx=fd->data;
	if(ctx->offs>=ctx->ent->usize)
		return(1);
	return(0);
}

int vfbuf_vfclose(VFILE *fd)
{
	VFZIP_FileCtx *ctx;
	ctx=fd->data;

	ctx->ent->ucnt--;
	if(ctx->ent->ucnt<0)
		ctx->ent->ucnt=0;
	gcfree(ctx);

	//inefficient: a file was modified, rewrite archive
	if(!ctx->ctx->fd && ctx->ctx->dirty)
	{
//		VFZIP_WriteZip(ctx->ctx, ctx->ctx->name);
		ctx->ctx->dirty=0;
	}

	return(0);
}

VFILE_Interface vfbuf_iface={
NULL,
"buffer_file",
vfbuf_vfread,	//read
vfbuf_vfwrite,	//write
vfbuf_vfgetc,	//getc
vfbuf_vfputc,	//putc
vfbuf_vfflush,	//flush
vfbuf_vfseek,	//seek
vfbuf_vftell,	//tell
vfbuf_vfeof,	//eof
vfbuf_vfclose,	//close
NULL		//ioctl
};

BGBDY_API VFILE *VFBUF_VfWrapEntry(VFBUF_Entry *ent)
{
	char buf[256];
	char *s;
	VFILE *tmp;
//	VFBUF_Entry *ent;
	VFBUF_FileCtx *ctx;
//	VFZIP_Context *cctx;
	int i;

	ctx=gcalloc(sizeof(VFBUF_FileCtx));
//	ctx->ctx=mnt->data;
	ctx->ent=ent;
	ctx->offs=0;

//	cctx=mnt->data;
//	cctx->dirty=1;

	ent->ucnt++;

	tmp=vf_new();
	tmp->iface=&vfbuf_iface;
	tmp->data=ctx;
	return(tmp);
}

BGBDY_API VFBUF_Entry *VFBUF_EntryWrapBuffer(
	char *name, void *data, int sz)
{
	VFBUF_Entry *ent;
	
	ent=gcalloc(sizeof(VFBUF_Entry));
	ent->name=dystrdup(name);

	ent->buffer=gcatomic(sz);
	ent->bsize=sz;
	memcpy(ent->buffer, data, sz);

	return(ent);
}

#if 0
VFILE *vfbuf_vfopen_w(VFILE_Mount *mnt, char *name, char *mode)
{
	char buf[256];
	char *s;
	VFILE *tmp;
	VFZIP_Entry *ent;
	VFZIP_FileCtx *ctx;
	VFZIP_Context *cctx;
	int i;

	ent=NULL;	//fix warning

	ctx=gcalloc(sizeof(VFZIP_FileCtx));
	ctx->ctx=mnt->data;
	ctx->ent=ent;
	ctx->offs=0;

	cctx=mnt->data;
	cctx->dirty=1;
	ent=VFZIP_CreateFile(cctx, name);
	ent->ucnt++;

	ctx->ent=ent;	//? (fixing warnings, forgot what code does)

	tmp=vf_new();
	tmp->iface=&vfbuf_iface;
	tmp->data=ctx;
	return(tmp);
}

VFILE *vfbuf_vfopen(VFILE_Mount *mnt, char *name, char *mode)
{
	char buf[256];
	char *s;
	VFILE *tmp;
	VFZIP_Entry *ent;
	VFZIP_FileCtx *ctx;
	VFZIP_Context *cctx;
	int i;

	strcpy(buf, name);
	s=buf;
	while(*s)
	{
		if(*s=='\\')*s='/';
		if((*s>='A') && (*s<='Z'))*s=*s-'A'+'a';
		s++;
	}

	if(mode[0]=='w')
	{
		cctx=mnt->data;
		if(cctx->fd)return(NULL);	//not rrw
		tmp=vfbuf_vfopen_w(mnt, buf, mode);
		return(tmp);
	}

	if(mode[1]=='+')
	{
		cctx=mnt->data;
		if(cctx->fd)return(NULL);	//not rrw
	}


	ent=VFZIP_LocateFile(mnt->data, name);
	VFZIP_CacheFile(mnt->data, name);
	if(!ent || !ent->buffer)return(NULL);

	ctx=gcalloc(sizeof(VFZIP_FileCtx));
	ctx->ctx=mnt->data;
	ctx->ent=ent;
	ctx->offs=0;

	ent->ucnt++;

	tmp=vf_new();
	tmp->iface=&vfbuf_iface;
	tmp->data=ctx;
	return(tmp);
}
#endif
