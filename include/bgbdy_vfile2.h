#ifndef VFILE2_H
#define VFILE2_H

#ifndef VF_IFMT
#define VF_IFMT		0xF000	//type of file
#define VF_IFDIR	0x4000	//directory
#define VF_IFCHR	0x2000	//character special
#define VF_IFBLK	0x6000	//block special
#define VF_IFREG	0x8000	//regular
#define VF_IFLNK	0xA000	//symbolic link
#define VF_IFSOCK	0xC000	//socket
#define VF_IFIFO	0x1000	//fifo
#endif

#ifndef VFSOFF_T
#define VFSOFF_T
typedef long long vfoffs_t;
#endif

typedef struct VFILE_Interface_s VFILE_Interface;
typedef struct VFILE_Mount_Interface_s VFILE_Mount_Interface;
typedef struct VFILE_Mount_s VFILE_Mount;

#ifndef VFILE_T
#define VFILE_T
typedef as_variant dytname("_vfile_t") struct VFILE_s VFILE;
typedef as_variant dytname("_vfdir_t") struct VFDIR_s VFDIR;
#endif

struct VFILE_Interface_s {
VFILE_Interface *next;
char *name;

int (*vfread)(void *buf, int s1, int s2, VFILE *fd);
int (*vfwrite)(void *buf, int s1, int s2, VFILE *fd);
int (*vfgetc)(VFILE *fd);
int (*vfputc)(int c, VFILE *fd);
int (*vfflush)(VFILE *fd);

vfoffs_t (*vfseek)(VFILE *fd, vfoffs_t pos, int rel);
vfoffs_t (*vftell)(VFILE *fd);
int (*vfeof)(VFILE *fd);
int (*vfclose)(VFILE *fd);
int (*vfioctl)(VFILE *fd, int req, void *arg);
};

struct VFILE_s {
VFILE *next;
char *name;

VFILE_Interface *iface;

//interface-specific scratchpad
void *data;
byte tdata[64];
int idata[4];
};

struct VFDIR_s {
char *str_buf;
char **str_idx;
int str_pos;
};

struct VFILE_Mount_Interface_s {
VFILE_Mount_Interface *next;
char *name;

VFILE *(*vfopen)(VFILE_Mount *mnt, char *name, char *mode);
VFILE_Mount *(*vfmount)(VFILE_Mount_Interface *iface,
	char *src, char *dst, char **ops);
int (*vfumount)(VFILE_Mount *mnt);

void *(*vf_opendir)(VFILE_Mount *mnt, char *name);
void (*vf_closedir)(VFILE_Mount *mnt, void *dir);
char *(*vf_readdir)(VFILE_Mount *mnt, void *dir);
int (*vf_statflags)(VFILE_Mount *mnt, char *name);
int (*vf_mkdir)(VFILE_Mount *mnt, char *name, int flags);
};

struct VFILE_Mount_s {
VFILE_Mount *next, *chain;
VFILE_Mount_Interface *iface;

char *src;
char *dst;
void *data;
};


#endif
