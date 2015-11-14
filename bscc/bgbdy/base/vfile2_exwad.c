#define DYLL_HASASM

#include <bgbgc.h>
#include <bgbdy.h>

#ifdef DYLL_HASASM
#include <bgbasm.h>
#include <bgblink.h>
#endif

ExWAD_Context *vfexwad_mounts=NULL;


int vfexwad_vfread(void *buf, int s1, int s2, VFILE *fd)
{
	ExWAD_File *ctx;
	int i;

	ctx=fd->data;
	i=s1*s2;
	if((ctx->offs+i)>ctx->ent->sz_buffer)
		i=ctx->ent->sz_buffer-ctx->offs;
	i/=s1;

	ExWAD_ReCacheFile(ctx->ctx, ctx->ent);
//	ctx->ent->iocnt=256;

	memcpy(buf, ctx->ent->buffer+ctx->offs, i*s1);
	ctx->offs+=i*s1;
	return(i);
}

int vfexwad_vfwrite(void *buf, int s1, int s2, VFILE *fd)
{
	ExWAD_File *ctx;
	int i;

	return(-1);

#if 0
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
#endif
}

int vfexwad_vfgetc(VFILE *fd)
{
	ExWAD_File *ctx;
	int i;

	ctx=fd->data;

	if(ctx->offs>=ctx->ent->sz_buffer)
		return(-1);

	ExWAD_ReCacheFile(ctx->ctx, ctx->ent);
//	ctx->ent->iocnt=256;

	i=ctx->ent->buffer[ctx->offs];
	ctx->offs++;
	return(i);
}

int vfexwad_vfputc(int c, VFILE *fd)
{
	int i;
	byte b;

	b=c;
	vfexwad_vfwrite(&b, 1, 1, fd);
	return(0);
}

int vfexwad_vfflush(VFILE *fd)
{
	ExWAD_File *ctx;
	ctx=fd->data;

	return(0);
}

vfoffs_t vfexwad_vfseek(VFILE *fd, vfoffs_t rel, int pos)
{
	ExWAD_File *ctx;
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
		ctx->offs+=ctx->ent->sz_buffer+rel;
		break;
	default:
		break;
	}
	return(ctx->offs);
}

vfoffs_t vfexwad_vftell(VFILE *fd)
{
	ExWAD_File *ctx;
	ctx=fd->data;
	return(ctx->offs);
}

int vfexwad_vfeof(VFILE *fd)
{
	ExWAD_File *ctx;
	ctx=fd->data;
	if(ctx->offs>=ctx->ent->sz_buffer)
		return(1);
	return(0);
}

int vfexwad_vfclose(VFILE *fd)
{
	ExWAD_File *ctx;
	ctx=fd->data;

	ctx->ent->ucnt--;
	if(ctx->ent->ucnt<0)
		ctx->ent->ucnt=0;
	gcfree(ctx);

#if 0
	//inefficient: a file was modified, rewrite archive
	if(!ctx->ctx->fd && ctx->ctx->dirty)
	{
		ExWAD_WriteZip(ctx->ctx, ctx->ctx->name);
		ctx->ctx->dirty=0;
	}
#endif

	return(0);
}

VFILE_Interface vfexwad_iface={
NULL,
"exwad_file",
vfexwad_vfread,		//read
vfexwad_vfwrite,	//write
vfexwad_vfgetc,		//getc
vfexwad_vfputc,		//putc
vfexwad_vfflush,	//flush
vfexwad_vfseek,		//seek
vfexwad_vftell,		//tell
vfexwad_vfeof,		//eof
vfexwad_vfclose,	//close
NULL				//ioctl
};

VFILE *vfexwad_vfopen(VFILE_Mount *mnt, char *name, char *mode)
{
	char buf[256];
	char *s;
	VFILE *tmp;
	ExWAD_CachedFile *ent;
	ExWAD_File *ctx;
	ExWAD_Context *cctx;
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
		return(NULL);
	if(mode[1]=='+')
		return(NULL);

	cctx=mnt->data;
	ctx=ExWAD_OpenFile(cctx, name);
	if(!ctx)return(NULL);
	ctx->ent->ucnt++;

	tmp=vf_new();
	tmp->iface=&vfexwad_iface;
	tmp->data=ctx;
	return(tmp);
}

VFILE_Mount *vfexwad_vfmount(VFILE_Mount_Interface *iface,
	char *src, char *dst, char **ops)
{
	VFILE_Mount *tmp;
	ExWAD_Context *ctx;
	byte *buf;
	int i, j, sz;

//	buf=BLNK_LoadModuleFileDataOS(ta[i], &sz);
	buf=BLNK_LoadModuleWadDataOS(src, &sz);
	if(!buf)
	{
		dyPrintf("vfexwad_vfmount: No Module Data %s\n", src);
		return(NULL);
	}

	ctx=ExWAD_LoadImageBufferRead(src, buf, sz);
	if(!ctx)
	{
		dyPrintf("vfexwad_vfmount: Bad Image %s %p %d\n", src, buf, sz);
		return(NULL);
	}

	if(buf)
	{
		dyPrintf("Got Module Data %p %d\n", buf, sz);
//		BLNK_FreeModuleDataOS(modbuf);
	}

//	ctx=ExWAD_OpenZip(src, rrw);
//	if(!ctx)return(NULL);

	tmp=vf_new_mount();

	tmp->iface=iface;
	tmp->src=strdup(src);
	tmp->dst=strdup(dst);

	tmp->data=ctx;

	ctx->mount=tmp;
	ctx->next=vfexwad_mounts;
	vfexwad_mounts=ctx;

	return(tmp);
}

VFILE_Mount_Interface vfexwad_mnt_iface={
NULL,		//chain
"exwad",		//name
vfexwad_vfopen,	//open
vfexwad_vfmount,	//mount
NULL,		//umount

NULL,		//opendir
NULL,		//closedir
NULL,		//readdir
NULL,		//statflags
NULL		//mkdir
};

void vfexwad_init()
{
	vf_register_fs(&vfexwad_mnt_iface);
}

/** HACK API: Try to open a file in any ExWAD mounts */
BGBDY_API VFILE *VFILE_ExWAD_OpenAny(char *name, char *mode)
{
	ExWAD_Context *cur;
	VFILE *tmp;

	cur=vfexwad_mounts;
	while(cur)
	{
		tmp=vfexwad_vfopen(cur->mount, name, mode);
		if(tmp)return(tmp);
		cur=cur->next;
	}
	return(NULL);
}
