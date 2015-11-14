#define VFZIP_CACHE_MAX		(1<<24)
#define VFZIP_CACHE_MAXCACHE	(1<<20)

#define FCC_PK_FILE	0x04034B50
#define FCC_PK_DIRFILE	0x02014B50
#define FCC_PK_DIREND	0x06054B50

typedef struct VFZIP_Entry_s VFZIP_Entry;
typedef struct VFZIP_Context_s VFZIP_Context;
typedef struct VFZIP_FileCtx_s VFZIP_FileCtx;

typedef struct VFZIP_Entry_s VFBUF_Entry;
typedef struct VFZIP_FileCtx_s VFBUF_FileCtx;

struct VFZIP_Entry_s {
VFZIP_Entry *next;	//next in file
VFZIP_Entry *hnext;	//next in hash
VFZIP_Entry *dnext;	//next in dir
VFZIP_Entry *dfirst;	//first in sub dir
VFZIP_Entry *cnext;	//next in cache
char *name;		//full name
char *rname;		//rel name

int offs, doffs, csize, usize, method, crc;
int ucnt;	//use count for file
int iocnt;	//io count for file (detects idle files)

byte *buffer;		//cached version of file
int bsize;		//buffer size
};

struct VFZIP_Context_s {
FILE *fd;			//zipfile
char *name;

VFZIP_Entry *first;		//first file
VFZIP_Entry *root;		//root entry
VFZIP_Entry *hash[4096];	//hash chains
VFZIP_Entry *croot;		//cache root
int cache;			//amount of cached data
int dirty;			//RRW: file is dirty
};

struct VFZIP_FileCtx_s {
VFZIP_Context *ctx;
VFZIP_Entry *ent;
int offs;
};
