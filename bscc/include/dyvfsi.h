#ifndef DYVFSI_H
#define DYVFSI_H

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

#define VF_PROTO_IPV4	(1<<8)
#define	VF_PROTO_IPV6	(2<<8)
#define VF_PROTO_TCP	1
#define	VF_PROTO_UDP	2

#define VF_PROTO_UNDEFINED	0
#define VF_PROTO_IPV4UDP	(PROTO_IPV4|PROTO_UDP)
#define VF_PROTO_IPV4TCP	(PROTO_IPV4|PROTO_TCP)
#define VF_PROTO_IPV6UDP	(PROTO_IPV6|PROTO_UDP)
#define VF_PROTO_IPV6TCP	(PROTO_IPV6|PROTO_TCP)

/* these may be loaded in the lower 8 flag bits for send and similar */
#define VF_PROTO_PF_UNDEFINED	0

#define VF_PROTO_PF_IPV4	4
#define VF_PROTO_PF_TCP		6
#define VF_PROTO_PF_UDP		17

#define VF_PROTO_PF_IPV6	41
#define VF_PROTO_PF_IPV6ROUTE	43
#define VF_PROTO_PF_IPV6FRAG	44
#define VF_PROTO_PF_IPV6ICMP	58
#define VF_PROTO_PF_IPV6NONXT	59
#define VF_PROTO_PF_IPV6OPTS	60

#ifndef BYTE_T
#define BYTE_T
typedef unsigned char byte;
#endif

#ifndef PDLIB_INT_BITS_T
#define PDLIB_INT_BITS_T
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef unsigned long long u64;
typedef signed long long s64;
#endif

#ifndef VFSOFF_T
#define VFSOFF_T
typedef long long vfoffs_t;
#endif

#ifndef VFILE_T
#define VFILE_T
typedef struct VFILE_s VFILE;
typedef struct VFDIR_s VFDIR;
#endif

typedef struct VFILE_Addr_s VFILE_Addr;
typedef struct VFILE_Stat_s VFILE_Stat;
typedef struct VFILE_Env_ps *VFILE_Env;


struct VFILE_Addr_s {
int proto;
int flags;
unsigned short port;
unsigned long addr4;
byte addr6[16];
};

struct VFILE_Stat_s {
u32 mode;
s64 size;
u32 atime;
u32 mtime;
u32 ctime;
};

struct VFILE_Env_ps {
void *resv0;	//0
void *resv1;	//1
void *resv2;	//2
void *resv3;	//3

//basic IO
int (*vfread)(VFILE_Env *env, void *buf, int s1, int s2, VFILE *fd);	//4
int (*vfwrite)(VFILE_Env *env, void *buf, int s1, int s2, VFILE *fd);	//5
int (*vfgetc)(VFILE_Env *env, VFILE *fd);				//6
int (*vfputc)(VFILE_Env *env, int c, VFILE *fd);			//7
int (*vfflush)(VFILE_Env *env, VFILE *fd);				//8

vfoffs_t (*vfseek)(VFILE_Env *env, VFILE *fd, vfoffs_t pos, int rel);	//9
vfoffs_t (*vftell)(VFILE_Env *env, VFILE *fd);				//10
int (*vfeof)(VFILE_Env *env, VFILE *fd);				//11
int (*vfclose)(VFILE_Env *env, VFILE *fd);				//12
int (*vfioctl)(VFILE_Env *env, VFILE *fd, int req, void *arg);		//13

//FS management
VFILE *(*vfopen)(VFILE_Env *env, char *name, char *mode);		//14
int (*vfmount)(VFILE_Env *env,
	char *src, char *dst, char *fstype, char **ops);		//15
int (*vfumount)(VFILE_Env *env, char *mnt);				//16

void *(*vfopendir)(VFILE_Env *env, char *name);				//17
void (*vfclosedir)(VFILE_Env *env, void *dir);				//18
char *(*vfreaddir)(VFILE_Env *env, void *dir);				//19
void (*vfrewinddir)(VFILE_Env *env, void *dir);				//20

int (*vfstatflags)(VFILE_Env *env, char *name);				//21
int (*vfstat)(VFILE_Env *env, char *name, VFILE_Stat *st);		//22
int (*vfsetstat)(VFILE_Env *env, char *name, VFILE_Stat *st, int fl);	//23

int (*vfmkdir)(VFILE_Env *env, char *name);				//24
int (*vfunlink)(VFILE_Env *env, char *name);				//25
int (*vflink)(VFILE_Env *env, char *oldname, char *newname);		//26
int (*vfscmd)(VFILE_Env *env, int cmd, void *arg, int szarg);		//27

//Socket IO
};

#endif
