/*
Directory Based Files/Mounts
 */

#include <bgbgc.h>
#include <bgbdy.h>

#include <sys/types.h>
#include <sys/stat.h>

#ifdef WIN32
#include <windows.h>
#endif

#ifndef MSVC
#include <dirent.h>
#include <unistd.h>
#endif


#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#ifndef PATHSEP
#define PATHSEP	'\\'
#endif
#endif

#if defined(WINDOWS) || defined(MSDOS)
#ifndef PATHSEP
#define PATHSEP	'\\'
#endif
#endif

#if defined(__CYGWIN__) || defined(__MINGW32__)
#ifndef PATHSEP
#define PATHSEP	'\\'
#endif
#endif

#ifndef PATHSEP
#define PATHSEP	'/'
#endif

int vfdir_vfread(void *buf, int s1, int s2, VFILE *fd)
{
	return(fread(buf, s1, s2, fd->data));
}

int vfdir_vfwrite(void *buf, int s1, int s2, VFILE *fd)
{
//	printf("vfwrite fd\n");
	return(fwrite(buf, s1, s2, fd->data));
}

int vfdir_vfgetc(VFILE *fd)
{
	return(fgetc(fd->data));
}

int vfdir_vfputc(int c, VFILE *fd)
{
//	printf("vfputc fd\n");
	return(fputc(c, fd->data));
}

int vfdir_vfflush(VFILE *fd)
{
	return(fflush(fd->data));
}

vfoffs_t vfdir_vfseek(VFILE *fd, vfoffs_t rel, int pos)
{
	return(fseek(fd->data, rel, pos));
}

vfoffs_t vfdir_vftell(VFILE *fd)
{
	return(ftell(fd->data));
}

int vfdir_vfeof(VFILE *fd)
{
	return(feof((FILE *)fd->data));
}

int vfdir_vfclose(VFILE *fd)
{
	fclose(fd->data);
	vf_destroy(fd);
	return(0);
}

VFILE_Interface vfdir_iface={
NULL,
"file",
vfdir_vfread,	//read
vfdir_vfwrite,	//write
vfdir_vfgetc,	//getc
vfdir_vfputc,	//putc
vfdir_vfflush,	//flush
vfdir_vfseek,	//seek
vfdir_vftell,	//tell
vfdir_vfeof,	//eof
vfdir_vfclose,	//close
NULL		//ioctl
};

BGBDY_API VFILE *vfdir_wrapfd(FILE *fd)
{
	VFILE *tmp;

	tmp=vf_new();
	tmp->iface=&vfdir_iface;
	tmp->data=fd;

	return(tmp);
}

VFILE *vfdir_vfopen(VFILE_Mount *mnt, char *name, char *mode)
{
	char buf[256];
	char *s;
	VFILE *tmp;
	FILE *fd;

	if(mnt->src && *mnt->src)
		sprintf(buf, "%s/%s", mnt->src, name);
		else strcpy(buf, name);

	s=buf;
	while(*s)
	{
		if(*s=='\\')*s=PATHSEP;
		if(*s=='/')*s=PATHSEP;
		s++;
	}
//	printf("open %s\n", buf);

	fd=fopen(buf, mode);
	if(!fd)return(NULL);

//	printf("open %s %p\n", buf, fd);

	tmp=vf_new();
	tmp->iface=&vfdir_iface;
	tmp->data=fd;
	return(tmp);
}

VFILE *vfdir_vfopen_ro(VFILE_Mount *mnt, char *name, char *mode)
{
	if(mode[0]=='w')return(NULL);
	if(mode[0]=='a')return(NULL);
	if(mode[1]=='+')return(NULL);
	if(mode[2]=='+')return(NULL);

	return(vfdir_vfopen(mnt, name, mode));
}

VFILE *vfdir_open_os(char *name, char *mode)
{
	char buf[256];
	char *s;
	VFILE *tmp;
	FILE *fd;

	strcpy(buf, name);
	s=buf;
	while(*s)
	{
		if(*s=='\\')*s=PATHSEP;
		if(*s=='/')*s=PATHSEP;
		s++;
	}
//	printf("open %s\n", buf);

	fd=fopen(buf, mode);
	if(!fd)return(NULL);

	tmp=vf_new();
	tmp->iface=&vfdir_iface;
	tmp->data=fd;
	return(tmp);
}

VFILE_Mount *vfdir_vfmount(VFILE_Mount_Interface *iface,
	char *src, char *dst, char **ops)
{
	char buf[256];
	VFILE_Mount *tmp;
	char *s;

	while((src[0]=='.') && ((src[1]=='/') || (src[1]=='\\')))src+=2;

	if((*src!='/') && (*src!='\\') && (*src!=':') && (src[1]!=':'))
	{
		getcwd(buf, 256);
		if(*src)
		{
			s=(char *)dystring(buf);
			sprintf(buf, "%s%c%s", s, PATHSEP, src);
		}
		src=buf;
	}

	tmp=vf_new_mount();

	tmp->iface=iface;
	tmp->src=strdup(src);
	tmp->dst=strdup(dst);

	printf("mounting %s as %s\n", src, *dst?dst:"/");

	return(tmp);
}

int vfdir_mkdir(VFILE_Mount *mnt, char *name, int flags)
{
	char buf[256];
	char *s;
	int i;

	if(mnt->src && *mnt->src)
		sprintf(buf, "%s/%s", mnt->src, name);
		else strcpy(buf, name);

	s=buf;
	while(*s)
	{
		if(*s=='\\')*s=PATHSEP;
		if(*s=='/')*s=PATHSEP;
		s++;
	}
//	printf("open %s\n", buf);

	i=vfdir_mkdir_os(buf, flags);
	return(i);
}

#ifndef MSVC
void *vfdir_opendir(VFILE_Mount *mnt, char *name)
{
	char buf[256];
	char *s, *t;
	DIR *tmp;

	if(mnt->src && *mnt->src)
		sprintf(buf, "%s/%s", mnt->src, name);
		else strcpy(buf, name);

	s=buf; t=buf;
	while(*s)
	{
		while((s[0]=='/') && (s[1]=='/'))s++;
		if(*s=='\\')*s=PATHSEP;
		if(*s=='/')*s=PATHSEP;
		*t++=*s++;
	}

	tmp=opendir(buf);
	return(tmp);

	return(NULL);
}

void vfdir_closedir(VFILE_Mount *mnt, void *dir)
{
	closedir(dir);
}

char *vfdir_readdir(VFILE_Mount *mnt, void *dir)
{
	char buf[256];
	struct dirent *de;

	de=readdir(dir);
	if(!de)return(NULL);

	strcpy(buf, de->d_name);
	return(dystrdup(buf));
}

int vfdir_statflags(VFILE_Mount *mnt, char *name)
{
	struct stat st;
	int i;

	i=stat(name, &st);
	if(i<0)return(-1);
	return(st.st_mode);
}

void *vfdir_opendir_os(char *name)
{
//	return(closedir(name));
	return(opendir(name));
}

void vfdir_closedir_os(void *dir)
{
	closedir(dir);
}

char *vfdir_readdir_os(void *dir)
{
	char buf[256];
	struct dirent *de;

	de=readdir(dir);
	if(!de)return(NULL);

	strcpy(buf, de->d_name);
	return(dystrdup(buf));
}

int vfdir_statflags_os(char *name)
{
	struct stat st;
	int i;

	i=stat(name, &st);
	if(i<0)return(-1);
	return(st.st_mode);
}

int vfdir_mkdir_os(char *name, int mode)
{
	mkdir(name, 0);
	return(0);
}

#endif

#ifdef MSVC

struct vfdir_buf_s {
char **name;
int n_name;
int pos;
};

void *vfdir_opendir(VFILE_Mount *mnt, char *name)
{
	char buf[1024];
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	struct vfdir_buf_s *tmp;
	char *s, *t;
	int i;

	if(mnt->src && *mnt->src)
		sprintf(buf, "%s/%s/*", mnt->src, name);
		else sprintf(buf, "%s/*", name);

	s=buf; t=buf;
	while(*s)
	{
		while((s[0]=='/') && (s[1]=='/'))s++;
		if(*s=='/')*s='\\';
		*t++=*s++;
	}

	hFind=FindFirstFile(buf, &FindFileData);
	tmp=NULL;

	if(hFind!=INVALID_HANDLE_VALUE) 
	{
		tmp=malloc(sizeof(struct vfdir_buf_s));
		tmp->name=malloc(4096*sizeof(char *));
		tmp->n_name=0;
		tmp->pos=0;

		s=strdup(FindFileData.cFileName);
		tmp->name[tmp->n_name++]=s;
   
		while (FindNextFile(hFind, &FindFileData) != 0) 
		{
			s=strdup(FindFileData.cFileName);
			tmp->name[tmp->n_name++]=s;
		}
    
		FindClose(hFind);
	}

	return(tmp);
}

void vfdir_closedir(VFILE_Mount *mnt, void *dir)
{
	struct vfdir_buf_s *tmp;
	int i;

	tmp=dir;

	for(i=0; i<tmp->n_name; i++)
		free(tmp->name[i]);
	free(tmp->name);
	free(tmp);

//	closedir(dir);
}

char *vfdir_readdir(VFILE_Mount *mnt, void *dir)
{
	struct vfdir_buf_s *tmp;
	int i;

	tmp=dir;

	i=tmp->pos++;
	if(i>=tmp->n_name)
		return(NULL);

	return(tmp->name[i]);
}

int vfdir_statflags(VFILE_Mount *mnt, char *name)
{
	char buf[1024];
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	char *s, *t;
	int i;

	if(mnt->src && *mnt->src)
		sprintf(buf, "%s/%s", mnt->src, name);
		else sprintf(buf, "%s", name);

	s=buf; t=buf;
	while(*s)
	{
		while((s[0]=='/') && (s[1]=='/'))s++;
		if(*s=='/')*s='\\';
		*t++=*s++;
	}

	hFind=FindFirstFile(buf, &FindFileData);
	if(hFind==INVALID_HANDLE_VALUE) 
		return(-1);
	FindClose(hFind);

	i=0;
	if(FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
		i|=VF_IFDIR;
	if(FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DEVICE)
		i|=VF_IFCHR;

	if(!i)i|=VF_IFREG;

	return(i);
}

void *vfdir_opendir_os(char *name)
{
	char buf[1024];
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	struct vfdir_buf_s *tmp;
	char *s, *t;
	int i;

	sprintf(buf, "%s/*", name);

	s=buf; t=buf;
	while(*s)
	{
		while((s[0]=='/') && (s[1]=='/'))s++;
		if(*s=='/')*s='\\';
		*t++=*s++;
	}

	hFind=FindFirstFile(buf, &FindFileData);
	tmp=NULL;

	if(hFind!=INVALID_HANDLE_VALUE) 
	{
		tmp=malloc(sizeof(struct vfdir_buf_s));
		tmp->name=malloc(4096*sizeof(char *));
		tmp->n_name=0;
		tmp->pos=0;

		s=strdup(FindFileData.cFileName);
		tmp->name[tmp->n_name++]=s;
   
		while (FindNextFile(hFind, &FindFileData) != 0) 
		{
			s=strdup(FindFileData.cFileName);
			tmp->name[tmp->n_name++]=s;
		}
    
		FindClose(hFind);
	}

	return(tmp);
}

void vfdir_closedir_os(void *dir)
{
	struct vfdir_buf_s *tmp;
	int i;

	tmp=dir;

	for(i=0; i<tmp->n_name; i++)
		free(tmp->name[i]);
	free(tmp->name);
	free(tmp);
}

char *vfdir_readdir_os(void *dir)
{
	struct vfdir_buf_s *tmp;
	int i;

	tmp=dir;

	i=tmp->pos++;
	if(i>=tmp->n_name)
		return(NULL);

	return(tmp->name[i]);
}

int vfdir_statflags_os(char *name)
{
	char buf[1024];
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	char *s, *t;
	int i;

	sprintf(buf, "%s", name);

	s=buf; t=buf;
	while(*s)
	{
		while((s[0]=='/') && (s[1]=='/'))s++;
		if(*s=='/')*s='\\';
		*t++=*s++;
	}

	hFind=FindFirstFile(buf, &FindFileData);
	if(hFind==INVALID_HANDLE_VALUE) 
		return(-1);
	FindClose(hFind);

	i=0;
	if(FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
		i|=VF_IFDIR;
	if(FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DEVICE)
		i|=VF_IFCHR;

	if(!i)i|=VF_IFREG;

	return(i);
}

int vfdir_mkdir_os(char *name, int mode)
{
	return(-1);
}

#endif



VFILE_Mount_Interface vdir_mnt_iface={
NULL,	//chain, NULL
"dir",
vfdir_vfopen,
vfdir_vfmount,
NULL,

vfdir_opendir,
vfdir_closedir,
vfdir_readdir,
vfdir_statflags,
vfdir_mkdir
};

VFILE_Mount_Interface vdir_ro_mnt_iface={
NULL,	//chain, NULL
"dir_ro",
vfdir_vfopen_ro,
vfdir_vfmount,
NULL,

vfdir_opendir,
vfdir_closedir,
vfdir_readdir,
vfdir_statflags,
NULL
};

void vfdir_init()
{
	vf_register_fs(&vdir_mnt_iface);
	vf_register_fs(&vdir_ro_mnt_iface);
}

