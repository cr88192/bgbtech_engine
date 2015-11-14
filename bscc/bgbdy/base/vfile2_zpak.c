#include <bgbgc.h>
#include <bgbdy.h>
#include <zpack.h>

int vfzpak_vfread(void *buf, int s1, int s2, VFILE *fd)
{
	ZPACK_FILE *zfd;
	int i;

	zfd=(ZPACK_FILE *)fd->data;
	i=ZPACK_Read(zfd, buf, s1*s2);

	return(i/s1);
}

int vfzpak_vfwrite(void *buf, int s1, int s2, VFILE *fd)
{
	ZPACK_FILE *zfd;
	int i;

	zfd=(ZPACK_FILE *)fd->data;
	i=ZPACK_Write(zfd, buf, s1*s2);

	return(i/s1);
}

int vfzpak_vfgetc(VFILE *fd)
{
	byte c[4];
	int i;
	i=vfzpak_vfread(c, 1, 1, fd);
	if(i<1)return(-1);
	return(c[0]);
}

int vfzpak_vfputc(int c, VFILE *fd)
{
	byte v[4];
	v[0]=c;
	vfzpak_vfwrite(v, 1, 1, fd);
	return(0);
}

int vfzpak_vfflush(VFILE *fd)
{
	ZPACK_FILE *zfd;
	int i;
	zfd=(ZPACK_FILE *)fd->data;
	i=ZPACK_Flush(zfd);
	return(i);
}

vfoffs_t vfzpak_vfseek(VFILE *fd, vfoffs_t pos, int rel)
{
	ZPACK_FILE *zfd;
	int i;
	zfd=(ZPACK_FILE *)fd->data;
	i=ZPACK_Seek(zfd, pos, rel);
	return(i);
}

vfoffs_t vfzpak_vftell(VFILE *fd)
{
	ZPACK_FILE *zfd;
	int i;
	zfd=(ZPACK_FILE *)fd->data;
	i=ZPACK_Tell(zfd);
	return(i);
}

int vfzpak_vfeof(VFILE *fd)
{
	ZPACK_FILE *zfd;
	int i;
	zfd=(ZPACK_FILE *)fd->data;
	i=ZPACK_Eof(zfd);
	return(i);
}

int vfzpak_vfclose(VFILE *fd)
{
	ZPACK_FILE *zfd;
	int i;
	zfd=(ZPACK_FILE *)fd->data;
	i=ZPACK_Close(zfd);
	return(i);
}

VFILE_Interface vfzpak_iface={
NULL,
"zpak_file",
vfzpak_vfread,	//read
vfzpak_vfwrite,	//write
vfzpak_vfgetc,	//getc
vfzpak_vfputc,	//putc
vfzpak_vfflush,	//flush
vfzpak_vfseek,	//seek
vfzpak_vftell,	//tell
vfzpak_vfeof,	//eof
vfzpak_vfclose,	//close
NULL		//ioctl
};

VFILE *vfzpak_vfopen(VFILE_Mount *mnt, char *name, char *mode)
{
	char buf[256];
	char *s, *t;
	VFILE *tmp;
	ZPACK_FILE *zfd;
	ZPACK_Context *ctx;
	int i;

	strcpy(buf, name);
	s=buf; t=buf;
	while(*s=='/')s++;
	while(*s)
	{
		while((s[0]=='/') && (s[1]=='/'))s++;
		if((*s>='A') && (*s<='Z'))*s=*s-'A'+'a';
		if(*s=='\\')*s='/';
		*t++=*s++;
	}

	ctx=(ZPACK_Context *)mnt->data;
	zfd=ZPACK_OpenFile(ctx, buf, mode);
	if(!zfd)return(NULL);

	tmp=vf_new();
	tmp->iface=&vfzpak_iface;
	tmp->data=zfd;
	return(tmp);
}

#if 0
VFILE_Mount *vfzpak_vfmount(VFILE_Mount_Interface *iface,
	char *src, char *dst, char **ops)
{
	VFILE_Mount *tmp;
	ZPACK_Context *ctx;
	int i, j, rw;

	rw=0;
	if(ops)
	{
		for(i=0; ops[i]; i++)
			if(!strcmp(ops[i], "rw"))rw=1;
	}

	ctx=ZPACK_OpenImage(src, rw?0:1);
	if(!ctx)return(NULL);

	tmp=vf_new_mount();

	tmp->iface=iface;
	tmp->src=strdup(src);
	tmp->dst=strdup(dst);

	tmp->data=ctx;

	return(tmp);
}
#endif

#if 1
VFILE_Mount *vfzpak_vfmount(VFILE_Mount_Interface *iface,
	char *src, char *dst, char **ops)
{
	VFILE_Mount *tmp;
	ZPACK_Context *ctx;
	int i, j, rw;

	rw=1;
	ctx=ZPACK_OpenImage(src, rw?0:1);
	if(!ctx)return(NULL);

	tmp=vf_new_mount();

	tmp->iface=iface;
	tmp->src=(char *)dystring(src);
	tmp->dst=(char *)dystring(dst);

	tmp->data=ctx;

	return(tmp);
}

VFILE_Mount *vfzpak_vfmount2(VFILE_Mount_Interface *iface,
	char *src, char *dst, char **ops)
{
	VFILE_Mount *tmp;
	ZPACK_Context *ctx;
	int i, j, rw;

	rw=0;
	ctx=ZPACK_OpenImage(src, rw?0:1);
	if(!ctx)return(NULL);

	tmp=vf_new_mount();

	tmp->iface=iface;
	tmp->src=(char *)dystring(src);
	tmp->dst=(char *)dystring(dst);

	tmp->data=ctx;

	return(tmp);
}
#endif

int vfzpak_vfumount(VFILE_Mount *mnt)
{
	ZPACK_Context *ctx;
	ctx=(ZPACK_Context *)mnt->data;
	ZPACK_CloseImage(ctx);
	return(0);
}


void *vfzpak_opendir(VFILE_Mount *mnt, char *name)
{
	char buf[256];
	char *s, *t;
	ZPACK_Context *ctx;
	ZPACK_DIR *tmp;

	strcpy(buf, name);
	s=buf; t=buf;
	while(*s=='/')s++;
	while(*s)
	{
		while((s[0]=='/') && (s[1]=='/'))s++;
		if((*s>='A') && (*s<='Z'))*s=*s-'A'+'a';
		if(*s=='\\')*s='/';
		*t++=*s++;
	}

	ctx=(ZPACK_Context *)mnt->data;
	tmp=ZPACK_OpenDir(ctx, buf);
	return(tmp);
}

void vfzpak_closedir(VFILE_Mount *mnt, void *dir)
{
	ZPACK_CloseDir(dir);
}

char *vfzpak_readdir(VFILE_Mount *mnt, void *dir)
{
	char *s;

	s=ZPACK_ReadDir(dir);
	return(s);
}

int vfzpak_statflags(VFILE_Mount *mnt, char *name)
{
	ZPACK_STAT st;
	int i;

	i=ZPACK_StatFile((ZPACK_Context *)mnt->data, name, &st);
	if(i<0)return(-1);

	i=0;
	if(st.flags&ZPACK_IFDIR)
		i|=VF_IFDIR;
	if(!i)i|=VF_IFREG;

	return(i);
}

VFILE_Mount_Interface vfzpak_mnt_iface={
NULL,			//chain
"zip_rw",		//name
vfzpak_vfopen,		//open
vfzpak_vfmount,		//mount
vfzpak_vfumount,	//umount

vfzpak_opendir,		//opendir
vfzpak_closedir,	//closedir
vfzpak_readdir,		//readdir
vfzpak_statflags,	//statflags
NULL				//mkdir
};

VFILE_Mount_Interface vfzpak_mnt_iface2={
NULL,			//chain
"zip_ro",		//name
vfzpak_vfopen,		//open
vfzpak_vfmount2,	//mount
vfzpak_vfumount,	//umount

vfzpak_opendir,		//opendir
vfzpak_closedir,	//closedir
vfzpak_readdir,		//readdir
vfzpak_statflags,	//statflags
NULL				//mkdir
};

void vfzpak_init()
{
	vf_register_fs(&vfzpak_mnt_iface);
	vf_register_fs(&vfzpak_mnt_iface2);
}
