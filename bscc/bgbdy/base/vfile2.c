#include <bgbgc.h>
#include <bgbdy.h>

VFILE_Interface *vfile2_ifroot=NULL;
VFILE_Mount_Interface *vfile2_mifroot=NULL;
VFILE *vfile2_fdroot=NULL;
VFILE_Mount *vfile2_mtab=NULL;

void *vf_mutex=NULL;

void vf_lock()
{
	thLockFastMutex(vf_mutex);
}

void vf_unlock()
{
	thUnlockFastMutex(vf_mutex);
}


void vfdir_init();

void vf_init()
{
	static int init=0;
	char buf[64];
	int i;

	if(init)return;
	init=1;

	vf_mutex=thFastMutex();

	dyTypeCallMethod("_vfile_t", vf_fd_method);

	vfdir_init();
	vfzip_init();
	vfzpak_init();
	vfexwad_init();
//	tyvf_init();

	vf_mount("", "/", "dir", NULL);
	vf_mount("resource", "/", "dir", NULL);

	for(i=0; i<10; i++)
	{
		sprintf(buf, "resource/pak%d.zip", i);
		vf_mount(buf, "/", "zip_ro", NULL);
	}
}

dyt vf_fd_method(dyt obj, dyt sym, dyt *args, int nargs)
{
	int i, j;

	if(!strcmp(dysymv(sym), "read") && (nargs==3))
	{
		i=dyintv(args[1]); j=dyintv(args[2]);
		i=vfread(args[0], i, j, (VFILE *)obj);
		return(dyint(i));
	}

	if(!strcmp(dysymv(sym), "write") && (nargs==3))
	{
		i=dyintv(args[1]); j=dyintv(args[2]);
		i=vfwrite(args[0], i, j, (VFILE *)obj);
		return(dyint(i));
	}

	if(!strcmp(dysymv(sym), "getc") && (nargs==0))
		return(dyint(vfgetc((VFILE *)obj)));
	if(!strcmp(dysymv(sym), "putc") && (nargs==1))
		return(dyint(vfputc(dyintv(args[0]), (VFILE *)obj)));

	if(!strcmp(dysymv(sym), "flush") && (nargs==0))
		return(dyint(vfflush((VFILE *)obj)));

	if(!strcmp(dysymv(sym), "seek") && (nargs==2))
		return(dylong(vfseek((VFILE *)obj,
			dylongv(args[0]), dyintv(args[1]))));
	if(!strcmp(dysymv(sym), "tell") && (nargs==0))
		return(dylong(vftell((VFILE *)obj)));
	if(!strcmp(dysymv(sym), "eof") && (nargs==0))
		return(dyint(vfeof((VFILE *)obj)));
	if(!strcmp(dysymv(sym), "close") && (nargs==0))
		return(dyint(vfclose((VFILE *)obj)));

	if(!strcmp(dysymv(sym), "gets") && (nargs==2))
		return((dyt)(vfgets((char *)(args[0]),
			dyintv(args[1]), (VFILE *)obj)));

	if(!strcmp(dysymv(sym), "ioctl") && (nargs==2))
		return(dyint(vfioctl((VFILE *)obj, dyintv(args[0]),
			args[1])));
	if(!strcmp(dysymv(sym), "ioctl2") && (nargs==3))
		return(dyint(vfioctl2((VFILE *)obj, dyintv(args[0]),
			args[1], args[2])));
	if(!strcmp(dysymv(sym), "ioctl3") && (nargs==4))
		return(dyint(vfioctl3((VFILE *)obj, dyintv(args[0]),
			args[1], args[2], args[3])));

	return(UNDEFINED);
}

BGBDY_API int vfread(void *buf, int s1, int s2, VFILE *fd)
{
	unsigned char *s, *t;
	int i, j;

	if(fd->iface->vfread)
		return(fd->iface->vfread(buf, s1, s2, fd));

	if(fd->iface->vfgetc)
	{
		i=s1*s2;
		s=buf;

		while(i--)
		{
			j=fd->iface->vfgetc(fd);
			if(j<0)break;
			*s++=j;
		}

		return((s-((unsigned char *)buf))/s1);
	}

	return(0);
}

BGBDY_API int vfwrite(void *buf, int s1, int s2, VFILE *fd)
{
	unsigned char *s, *t;
	int i, j;

	if(fd->iface->vfwrite)
		return(fd->iface->vfwrite(buf, s1, s2, fd));

	if(fd->iface->vfputc)
	{
		i=s1*s2;
		s=buf;

		while(i--)j=fd->iface->vfputc(*s++, fd);
		return(s2);
	}

	return(0);
}

BGBDY_API int vfgetc(VFILE *fd)
{
	unsigned char buf[4];
	int i;

//	dyPrintf("vfgetc 0, %s\n", fd->iface->name);

	if(fd->iface->vfgetc)
		return(fd->iface->vfgetc(fd));

	dyPrintf("vfgetc 1\n");

	if(fd->iface->vfread)
	{
		i=fd->iface->vfread(buf, 1, 1, fd);
		if(i==1)return(buf[0]);
		return(-1);
	}

	return(-1);
}

BGBDY_API int vfputc(int c, VFILE *fd)
{
	unsigned char buf[4];
	int i;

	if(fd->iface->vfputc)
		return(fd->iface->vfputc(c, fd));

	if(fd->iface->vfwrite)
	{
		buf[0]=c;
		i=fd->iface->vfwrite(buf, 1, 1, fd);
		if(i==1)return(c);
		return(-1);
	}

	return(-1);
}

BGBDY_API int vfflush(VFILE *fd)
{
	if(fd->iface->vfflush)
		return(fd->iface->vfflush(fd));
	return(-1);
}


BGBDY_API vfoffs_t vfseek(VFILE *fd, vfoffs_t pos, int rel)
{
	if(fd->iface->vfseek)
		return(fd->iface->vfseek(fd, pos, rel));
	return(-1);
}

BGBDY_API vfoffs_t vftell(VFILE *fd)
{
	if(fd->iface->vftell)
		return(fd->iface->vftell(fd));
	if(fd->iface->vfseek)
		return(fd->iface->vfseek(fd, 0, 1));
	return(-1);
}

BGBDY_API int vfeof(VFILE *fd)
{
	if(fd->iface->vfeof)
		return(fd->iface->vfeof(fd));
	return(0);
}

BGBDY_API int vfclose(VFILE *fd)
{
	if(fd->iface->vfclose)
		return(fd->iface->vfclose(fd));
	return(-1);
}

BGBDY_API int vfioctl(VFILE *fd, int req, void *arg)
{
	if(fd->iface->vfioctl)
		return(fd->iface->vfioctl(fd, req, arg));
	return(-1);
}

BGBDY_API int vfioctl2(VFILE *fd, int req, void *arg0, void *arg1)
{
	void *parr[4];

	parr[0]=arg0;
	parr[1]=arg1;
	return(vfioctl(fd, req, parr));
}

BGBDY_API int vfioctl3(VFILE *fd, int req, void *arg0, void *arg1, void *arg2)
{
	void *parr[4];

	parr[0]=arg0;
	parr[1]=arg1;
	parr[2]=arg2;
	return(vfioctl(fd, req, parr));
}

BGBDY_API char *vfgets(char *s, int n, VFILE *fd)
{
	char *t;
	int c;

	t=s;
	while(!vfeof(fd) && ((t-s)<(n-1)))
	{
		c=vfgetc(fd);
		if(c<0)break;
		if(c=='\r')continue;
		if(c=='\n')break;
		if(c=='\x08'){s--; continue;}

		*t++=c;
	}
	*t++=0;

	if(*s)return(s);
	if(c<0)return(NULL);
	return(s);
}

BGBDY_API int vfputs(char *str, VFILE *fd)
{
	vfwrite(str, 1, strlen(str), fd);
	return(0);
}

BGBDY_API void vf_register(VFILE_Interface *iface)
{
	VFILE_Interface *cur;

	if(vfile2_ifroot)
	{
		cur=vfile2_ifroot;
		while(cur->next && (cur!=iface))cur=cur->next;
		if(cur==iface)return;
		cur->next=iface;
	}else vfile2_ifroot=iface;
}

BGBDY_API void vf_register_fs(VFILE_Mount_Interface *iface)
{
	VFILE_Mount_Interface *cur;

	if(vfile2_mifroot)
	{
		cur=vfile2_mifroot;
		while(cur->next && (cur!=iface))cur=cur->next;
		if(cur==iface)return;
		cur->next=iface;
	}else vfile2_mifroot=iface;
}


BGBDY_API VFILE *vf_new()
{
	VFILE *tmp;

	tmp=gctalloc("_vfile_t", sizeof(VFILE));

	tmp->next=vfile2_fdroot;
	vfile2_fdroot=tmp;

	return(tmp);
}

BGBDY_API void vf_destroy(VFILE *fd)
{
	VFILE *cur;

	if(fd==vfile2_fdroot)
	{
		vfile2_fdroot=vfile2_fdroot->next;
		gcfree(fd);
	}else
	{
		cur=vfile2_fdroot;
		while(cur && (cur->next!=fd))
			cur=cur->next;
		if(cur)cur->next=fd->next;
		gcfree(fd);
	}
}


BGBDY_API VFILE_Mount *vf_new_mount()
{
	VFILE_Mount *tmp, *cur;

	tmp=gctalloc("_vfile_mount_t", sizeof(VFILE_Mount));

	cur=vfile2_mtab;
	if(cur)
	{
		while(cur->next)cur=cur->next;
		cur->next=tmp;
	}else vfile2_mtab=tmp;

//	tmp->next=vfile2_mtab;
//	vfile2_mtab=tmp;

	return(tmp);
}

BGBDY_API VFILE *vffopen(char *name, char *mode)
{
	VFILE_Mount *cur, *blst, *bcur, *blcur;
	VFILE *fd;
	char *s;

	vf_init();

	if((((*name>='a') && (*name<='z')) ||
		((*name>='A') && (*name<='Z'))) && (name[1]==':'))
	{
		fd=vfdir_open_os(name, mode);
		return(fd);
	}

	while((*name=='/') || (*name=='\\'))name++;

	if(!strcmp(name, "dev/stdin"))return(vfdir_wrapfd(stdin));
	if(!strcmp(name, "dev/stdout"))return(vfdir_wrapfd(stdout));
	if(!strcmp(name, "dev/stderr"))return(vfdir_wrapfd(stderr));

//	dyPrintf("attempt open '%s' '%s'\n", name, mode);

	blst=NULL;
	cur=vfile2_mtab;
	while(cur)
	{
//		dyPrintf("check mount '%s' '%s'\n", cur->src, cur->dst);

		if(!strncmp(cur->dst, name, strlen(cur->dst)))
		{
			bcur=blst; blcur=NULL;
			while(bcur && (strlen(bcur->dst)>strlen(cur->dst)))
			{
				blcur=bcur;
				bcur=bcur->chain;
			}

			if(blcur)
			{
				cur->chain=bcur;
				blcur->chain=cur;
			}else
			{
				cur->chain=blst;
				blst=cur;
			}
		}
		cur=cur->next;
	}

	bcur=blst;
	while(bcur)
	{
//		dyPrintf("try mount '%s' '%s'\n", bcur->src, bcur->dst);

		s=name+strlen(bcur->dst);
		while((*s=='/') || (*s=='\\'))s++;

		fd=bcur->iface->vfopen(bcur, s, mode);
		if(fd)return(fd);
		bcur=bcur->chain;
	}

//	dyPrintf("fail open '%s' '%s'\n", name, mode);

	return(NULL);
}

// BGBDY_API FILE *vf_try_fopen(char *name, char *mode)
// {
// }

BGBDY_API int vf_mount(char *src, char *dst, char *fs, char **ops)
{
	VFILE_Mount_Interface *cur;
	VFILE_Mount *mnt;

	vf_init();

//	dyPrintf("attempt mount '%s' '%s' '%s'\n", src, dst, fs);

	cur=vfile2_mifroot;
	while(cur)
	{
		if(cur->name && !strcmp(cur->name, fs))
			break;
		cur=cur->next;
	}
	if(!cur)
	{
		dyPrintf("no fs '%s'\n", fs);
		return(-1);
	}

	src=(char *)dystring(src);
	dst=(char *)dystring(dst);

	while(*src && (src[strlen(src)-1]=='/'))
		src[strlen(dst)-1]=0;

	while((*dst=='/') || (*dst=='\\'))dst++;
	while(*dst && (dst[strlen(dst)-1]=='/'))
		dst[strlen(dst)-1]=0;

//	dyPrintf("mount '%s' '%s' '%s'\n", src, dst, fs);

	mnt=cur->vfmount(cur, src, dst, ops);
	if(!mnt)return(-1);
	return(0);
}

BGBDY_API void vf_umount(char *name)
{
	VFILE_Mount *cur;

	if(!name)
	{
		cur=vfile2_mtab;
		while(cur)
		{
			if(cur->iface->vfumount)
				cur->iface->vfumount(cur);
			cur=cur->next;
		}
		return;
	}

	cur=vfile2_mtab;
	while(cur)
	{
		if(cur->src && !strcmp(cur->src, name))
		{
			if(cur->iface->vfumount)
				cur->iface->vfumount(cur);
			return;
		}
		if(cur->src && !strcmp(cur->dst, name))
		{
			if(cur->iface->vfumount)
				cur->iface->vfumount(cur);
			return;
		}
		cur=cur->next;
	}
}

BGBDY_API void vfDumpMounts(void)
{
	VFILE_Mount *cur;

	cur=vfile2_mtab;
	while(cur)
	{
		dyPrintf("%s %s %s\n", cur->src, cur->dst, cur->iface->name);
		cur=cur->next;
	}
}


BGBDY_API VFDIR *vfopendir(char *name)
{
	VFILE_Mount *cur;
	VFDIR *tmp;
	void *p;
	char *s, *t;
	int i, j;

	vf_init();

	tmp=gctalloc("_vfdir_t", sizeof(VFDIR));
	tmp->str_buf=gcatomic(65536);
	memset(tmp->str_buf, 0, 65536);

	tmp->str_idx=gcalloc(4096*sizeof(char *));
	memset(tmp->str_idx, 0, 4096*sizeof(char *));
	tmp->str_pos=0;


	if((((*name>='a') && (*name<='z')) ||
		((*name>='A') && (*name<='Z'))) && (name[1]==':'))
	{
		p=vfdir_opendir_os(name);
		if(!p) { return(NULL); }

		while(1)
		{
			s=vfdir_readdir_os(p);
			if(!s)break;

			t=tmp->str_buf;
			while(*t)
			{
				if(!strcmp(t, s))
					break;
				t+=strlen(t)+1;
			}

			if(!*t)
			{
				strcpy(t, s);
				tmp->str_idx[tmp->str_pos++]=t;
			}
		}

		vfdir_closedir_os(p);
	}

	while((*name=='/') || (*name=='\\'))name++;

	//first pass: enumerate filenames

	cur=vfile2_mtab;
	while(cur)
	{
		if(!strncmp(cur->dst, name, strlen(cur->dst)))
		{
			if(!cur->iface->vf_opendir)
				{ cur=cur->next; continue; }

			s=name+strlen(cur->dst);
			while((*s=='/') || (*s=='\\'))s++;
			p=cur->iface->vf_opendir(cur, s);
			if(!p) { cur=cur->next; continue; }

			while(1)
			{
				s=cur->iface->vf_readdir(cur, p);
				if(!s)break;

				t=tmp->str_buf;
				while(*t)
				{
					if(!strcmp(t, s))
						break;
					t+=strlen(t)+1;
				}

				if(!*t)
				{
					strcpy(t, s);
					tmp->str_idx[tmp->str_pos++]=t;
				}
			}

			cur->iface->vf_closedir(cur, p);
		}
		cur=cur->next;
	}

	//second pass: sort into lexiographic order

	//stupid slow sort...
	for(i=0; i<tmp->str_pos; i++)
		for(j=i+1; j<tmp->str_pos; j++)
	{
		if(strcmp(tmp->str_idx[j], tmp->str_idx[i])<0)
		{
			s=tmp->str_idx[i];
			tmp->str_idx[i]=tmp->str_idx[j];
			tmp->str_idx[j]=s;
		}
	}

	if(!tmp->str_idx[0])
	{
		vfclosedir(tmp);
		return(NULL);
	}

	tmp->str_pos=0;
	return(tmp);
}

BGBDY_API void vfclosedir(VFDIR *dir)
{
	gcfree(dir->str_buf);
	gcfree(dir->str_idx);
	gcfree(dir);
}

BGBDY_API char *vfreaddir(VFDIR *dir)
{
	char *s;

	s=dir->str_idx[dir->str_pos];
	if(!s)return(NULL);
	dir->str_pos++;
	return(s);
}

BGBDY_API void vfrewinddir(VFDIR *dir)
{
	dir->str_pos=0;
}

BGBDY_API int vfstatflags(char *name)
{
	VFILE_Mount *cur;
	void *p;
	char *s, *t;
	int i, j;

	if((((*name>='a') && (*name<='z')) ||
		((*name>='A') && (*name<='Z'))) && (name[1]==':'))
	{
		i=vfdir_statflags_os(name);
		return(i);
	}

	while((*name=='/') || (*name=='\\'))name++;

	cur=vfile2_mtab;
	while(cur)
	{
		if(!strncmp(cur->dst, name, strlen(cur->dst)))
		{
			if(!cur->iface->vf_statflags)
				{ cur=cur->next; continue; }

			s=name+strlen(cur->dst);
			while((*s=='/') || (*s=='\\'))s++;
			i=cur->iface->vf_statflags(cur, s);
			if(i<0) { cur=cur->next; continue; }

			return(i);
		}
		cur=cur->next;
	}

	return(-1);
}

BGBDY_API int vfmkdir(char *name, int flags)
{
	VFILE_Mount *cur, *blst, *bcur, *blcur;
	char *s;
	int i;

	vf_init();

	if((((*name>='a') && (*name<='z')) ||
		((*name>='A') && (*name<='Z'))) && (name[1]==':'))
	{
		i=vfdir_mkdir_os(name, flags);
		return(i);
	}

	while((*name=='/') || (*name=='\\'))name++;

//	if(!strcmp(name, "dev/stdin"))return(vfdir_wrapfd(stdin));
//	if(!strcmp(name, "dev/stdout"))return(vfdir_wrapfd(stdout));
//	if(!strcmp(name, "dev/stderr"))return(vfdir_wrapfd(stderr));

//	dyPrintf("attempt open '%s' '%s'\n", name, mode);

	blst=NULL;
	cur=vfile2_mtab;
	while(cur)
	{
//		dyPrintf("check mount '%s' '%s'\n", cur->src, cur->dst);

		if(!strncmp(cur->dst, name, strlen(cur->dst)))
		{
			bcur=blst; blcur=NULL;
			while(bcur && (strlen(bcur->dst)>strlen(cur->dst)))
			{
				blcur=bcur;
				bcur=bcur->chain;
			}

			if(blcur)
			{
				cur->chain=bcur;
				blcur->chain=cur;
			}else
			{
				cur->chain=blst;
				blst=cur;
			}
		}
		cur=cur->next;
	}

	bcur=blst;
	while(bcur)
	{
//		dyPrintf("try mount '%s' '%s'\n", bcur->src, bcur->dst);

		s=name+strlen(bcur->dst);
		while((*s=='/') || (*s=='\\'))s++;

		i=bcur->iface->vf_mkdir(bcur, s, flags);
		if(i>=0)return(i);
		bcur=bcur->chain;
	}

//	dyPrintf("fail open '%s' '%s'\n", name, mode);

	return(-1);
}


BGBDY_API char *vfgenabs(char *base, char *name)
{
	char buf[256];
	char *s;

	s=name;
	if(*s=='/')return(name);
	if(*s=='\\')return(name);

	while(*s && (*s!='/') && (*s!='\\'))s++;
	if(*s==':')return(name);

	sprintf(buf, "%s/%s", base, name);
	name=(char *)dystring(buf);
	return(name);
}

BGBDY_API char *vfgetext(char *name)
{
	char *s;

	s=name+strlen(name);
	while((s>name) && (*s!='.'))s--;
	if(s>name)return(s+1);
	return(NULL);
}

BGBDY_API int vfcmpext(char *str, char *ext)
{
	char *s;

	if(!ext)return(1);
	while(*ext)
	{
		s=str;
		while(*s && *ext && (*s==*ext))
			{ s++; ext++; }
		if(!*s && (!(*ext) || (*ext==';')))
			return(1);

		while(*ext && (*ext!=';'))ext++;
		if(*ext==';')ext++;
	}
	return(0);
}

BGBDY_API char *vfTryFindOSPath(char *name, char *mode)
{
	char tb[1024];
	VFILE_Mount *cur;
	void *p;
	char *s, *t;
	int i, j;

	if((((*name>='a') && (*name<='z')) ||
		((*name>='A') && (*name<='Z'))) && (name[1]==':'))
	{
		return(name);
	}

	while((*name=='/') || (*name=='\\'))name++;

	cur=vfile2_mtab;
	while(cur)
	{
		if(!cur->iface->name || strcmp(cur->iface->name, "dir"))
		{
			cur=cur->next;
			continue;
		}

		if(!strncmp(cur->dst, name, strlen(cur->dst)))
		{
			if(!cur->iface->vf_statflags)
				{ cur=cur->next; continue; }

			s=name+strlen(cur->dst);
			while((*s=='/') || (*s=='\\'))s++;
			i=cur->iface->vf_statflags(cur, s);
			if(i<0) { cur=cur->next; continue; }

			if(cur->src && *cur->src)
				sprintf(tb, "%s/%s", cur->src, name);
			else sprintf(tb, "%s", name);

			return(gcrstrdup(tb));
		}
		cur=cur->next;
	}

	return(NULL);
}

BGBDY_API byte *vf_bufferin(VFILE *fd)
{
	byte *tmp;
	int size;

	if(!fd)return(NULL);

	vfseek(fd, 0, 2);
	size=vftell(fd);
	vfseek(fd, 0, 0);
	tmp=gcatomic(size+1);
	vfread(tmp, 1, size, fd);

	return(tmp);
}

BGBDY_API byte *vf_bufferin_sz(VFILE *fd, int *rsz)
{
	byte *tmp;
	int size;

	if(!fd)return(NULL);

	vfseek(fd, 0, 2);
	size=vftell(fd);
	vfseek(fd, 0, 0);
	tmp=gcatomic(size+1);
	vfread(tmp, 1, size, fd);

	if(rsz)*rsz=size;

	return(tmp);
}

BGBDY_API byte *vf_loadfile(char *name, int *rsz)
{
	VFILE *fd;
	byte *buf;

	fd=vffopen(name, "rb");
	if(!fd)return(NULL);
	buf=vf_bufferin_sz(fd, rsz);
	vfclose(fd);
	return(buf);
}

BGBDY_API int vf_storefile(char *name, void *buf, int sz)
{
	VFILE *fd;

	fd=vffopen(name, "wb");
	if(!fd)return(-1);
	
	if(fd)
	{
		vfwrite(buf, 1, sz, fd);
		vfclose(fd);
	}

	return(0);
}

BGBDY_API void vfprint(VFILE *fd, char *str, ...)
{
	char buf[4096];
	va_list lst;

	va_start(lst, str);
	vsprintf(buf, str, lst);
	va_end(lst);

	vfwrite(buf, 1, strlen(buf), fd);
}

BGBDY_API int vfDeflateBuffer(byte *obuf, byte *ibuf, int osz, int isz)
{
	int i;
	vf_lock();
	i=PDZ2_EncodeStream(ibuf, obuf, isz, osz);
	vf_unlock();
	return(i);
}

BGBDY_API int vfDeflateBuffer64(byte *obuf, byte *ibuf, int osz, int isz)
{
	int i;
	vf_lock();
	i=PDZ2_EncodeStream64(ibuf, obuf, isz, osz);
	vf_unlock();
	return(i);
}

BGBDY_API int vfDeflateBufferLvl(byte *obuf, byte *ibuf, int osz, int isz, int lvl)
{
	int i;
	vf_lock();
	i=PDZ2_EncodeStreamLvl(ibuf, obuf, isz, osz, lvl);
	vf_unlock();
	return(i);
}

BGBDY_API int vfInflateBuffer(byte *obuf, byte *ibuf, int osz, int isz)
{
	int i;
//	vf_lock();
//	i=PDUNZ_DecodeStream(ibuf, obuf, isz, osz);
//	vf_unlock();
	i=PDUNZ2_DecodeStream(ibuf, obuf, isz, osz);
	return(i);
}



//VFILE Env

static int vf_env_vfread(VFILE_Env *env, void *buf, int s1, int s2, VFILE *fd)
	{ return(vfread(buf, s1, s2, fd)); }
static int vf_env_vfwrite(VFILE_Env *env, void *buf, int s1, int s2, VFILE *fd)
	{ return(vfwrite(buf, s1, s2, fd)); }
static int vf_env_vfgetc(VFILE_Env *env, VFILE *fd)
	{ return(vfgetc(fd)); }
static int vf_env_vfputc(VFILE_Env *env, int c, VFILE *fd)
	{ return(vfputc(c, fd)); }
static int vf_env_vfflush(VFILE_Env *env, VFILE *fd)
	{ return(vfflush(fd)); }

static vfoffs_t vf_env_vfseek(VFILE_Env *env, VFILE *fd, vfoffs_t pos, int rel)
	{ return(vfseek(fd, pos, rel)); }
static vfoffs_t vf_env_vftell(VFILE_Env *env, VFILE *fd)
	{ return(vftell(fd)); }
static int vf_env_vfeof(VFILE_Env *env, VFILE *fd)
	{ return(vfeof(fd)); }
static int vf_env_vfclose(VFILE_Env *env, VFILE *fd)
	{ return(vfclose(fd)); }
static int vf_env_vfioctl(VFILE_Env *env, VFILE *fd, int req, void *arg)
	{ return(vfioctl(fd, req, arg)); }

static VFILE *vf_env_vfopen(VFILE_Env *env, char *name, char *mode)
	{ return(vffopen(name, mode)); }
static int vf_env_vfmount(VFILE_Env *env,
		char *src, char *dst, char *fstype, char **ops)
	{ vf_mount(src, dst, fstype, ops); return(0); }
static int vf_env_vfumount(VFILE_Env *env, char *mnt)
	{ vf_umount(mnt); return(0); }

static void *vf_env_vfopendir(VFILE_Env *env, char *name)
	{ return(vfopendir(name)); }
static void vf_env_vfclosedir(VFILE_Env *env, void *dir)
	{ vfclosedir(dir); }
static char *vf_env_vfreaddir(VFILE_Env *env, void *dir)
	{ return(vfreaddir(dir)); }
static void vf_env_vfrewinddir(VFILE_Env *env, void *dir)
	{ vfrewinddir(dir); }

static int vf_env_vfstatflags(VFILE_Env *env, char *name)
	{ return(vfstatflags(name)); }

static int vf_env_vfstat(VFILE_Env *env, char *name, VFILE_Stat *st)
	{ return(-1); }
static int vf_env_vfsetstat(VFILE_Env *env, char *name, VFILE_Stat *st, int fl)
	{ return(-1); }
static int vf_env_vfmkdir(VFILE_Env *env, char *name)
	{ return(-1); }
static int vf_env_vfunlink(VFILE_Env *env, char *name)
	{ return(-1); }
static int vf_env_vflink(VFILE_Env *env, char *sname, char *dname)
	{ return(-1); }
static int vf_env_vfscmd(VFILE_Env *env, int cmd, void *arg, int szarg)
	{ return(-1); }

struct VFILE_Env_ps vf_env_iface={
NULL,			// 0
NULL,			// 1
NULL,			// 2
NULL,			// 3
vf_env_vfread,		// 4, vfread
vf_env_vfwrite,		// 5, vfwrite
vf_env_vfgetc,		// 6, vfgetc
vf_env_vfputc,		// 7, vfputc
vf_env_vfflush,		// 8, vfflush
vf_env_vfseek,		// 9, vfseek
vf_env_vftell,		//10, vftell
vf_env_vfeof,		//11, vfeof
vf_env_vfclose,		//12, vfclose
vf_env_vfioctl,		//13, vfioctl
vf_env_vfopen,		//14, vfopen
vf_env_vfmount,		//15, vfmount
vf_env_vfumount,	//16, vfumount
vf_env_vfopendir,	//17, vfopendir
vf_env_vfclosedir,	//18, vfclosedir
vf_env_vfreaddir,	//19, vfreaddir
vf_env_vfrewinddir,	//20, vfrewinddir
vf_env_vfstatflags,	//21, vfstatflags
vf_env_vfstat,		//22, vfstat
vf_env_vfsetstat,	//23, vfsetstat
vf_env_vfmkdir,		//24, vfmkdir
vf_env_vfunlink,	//25, vfunlink
vf_env_vflink,		//26, vflink
vf_env_vfscmd		//27, vfscmd
};

BGBDY_API void *vfGetEnv()
{
	static VFILE_Env env;
	env=&vf_env_iface;
	return((void *)(&env));
}
