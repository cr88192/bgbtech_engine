#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifndef ZPACK_H
#define ZPACK_H

#ifdef _WIN32
#ifndef WIN32
#define WIN32
#endif
#endif

#ifdef WIN32
#ifdef ZPACK_DLL
#define ZPACK_API __declspec(dllexport)
#else
#define ZPACK_API __declspec(dllimport)
#endif
#else
#define ZPACK_API
#endif

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

#ifdef __cplusplus
extern "C" {
#endif

#define ZPACK_IFDIR	2

#ifdef LIBZPACK_C	//internal to the lib

#ifndef FOURCC
#define FOURCC(a, b, c, d) ((a)|((b)<<8)|((c)<<16)|((d)<<24))
#endif

#define FCC_ZPAK	FOURCC('Z', 'P', 'A', 'K')

#define FCC_PK12	FOURCC('P', 'K', 1, 2)
#define FCC_PK34	FOURCC('P', 'K', 3, 4)
#define FCC_PK56	FOURCC('P', 'K', 5, 6)
#define FCC_ZZ34	FOURCC('Z', 'Z', 3, 4)

// #define ZPACK_IFDIR	2

typedef struct ZPACK_DirEntry_s ZPACK_DirEntry;
typedef struct ZPACK_CacheFile_s ZPACK_CacheFile;
typedef struct ZPACK_SpansNode_s ZPACK_SpansNode;

struct ZPACK_DirEntry_s
{
// char name[32];		//file name, 0 padded
char name[64];		//file name, 0 padded
char *name2;		//file name (for overflow)
int chain;		//next dir entry
u16 date;		//date modified
u16 time;		//time modified
u16 flags;		//file flags (1&=fragmented, 2&=dir)
u16 method;		//method number, 0=store, 8=deflate, 9=deflate(ext)
u32 crc32;		//CRC32 of data (or first entry if dir)

u32 usize;		//uncompressed size of data
u32 csize;		//compressed size of data
u32 offset;		//offset to start of data

u32 hoffs, hfsize;	//file header offset and full size
};


struct ZPACK_CacheFile_s
{
ZPACK_CacheFile *next, *prev;	//mru cache chains
ZPACK_CacheFile *hnext;		//next in current hash
char *path;			//pathname for file
u32 lsize, bsize;		//logical and buffer size
u32 hoffs, offs, size;		//offset and size (in image)
byte dirty, method;

byte *buffer;			//cached buffer
};

struct ZPACK_SpansNode_s
{
int lnode;	//left sub-node
int rnode;	//right sub-node
u32 offs;	//offset/mid-point
u32 size;	//size/height
char *name;	//optional name/description
};

typedef struct {
ZPACK_DirEntry *root;		//root directory

ZPACK_CacheFile *mru;		//start of cache mru
ZPACK_CacheFile *hash[4096];	//cache hash chains
ZPACK_CacheFile *cachefree;	//free list for cache
ZPACK_CacheFile *cachebuf;	//buffer for cached files

ZPACK_SpansNode *span;		//spans buffer
int spans_root;			//spans root
int spans_free;			//free spans
int n_spans;			//number of spans

int ents, ients;		//root ents

FILE *fd;
char *name;
int flags;

int doffs, dsize, dents;	//central dir info
int doffs2, dsize2;		//secondary (padded) dir space
int hoffs, hsz;			//header offset and size
u32 eof;			//current known end of file
}ZPACK_Context;


typedef struct {
ZPACK_Context *ctx;
ZPACK_CacheFile *cache;
int offs;
}ZPACK_FILE;

typedef struct {
ZPACK_Context *ctx;
int base;	//first entry
int pos;	//current entry
//char nbuf[33];	//name buffer
char nbuf[65];	//name buffer
}ZPACK_DIR;

typedef struct {
u32 date_time;
u16 flags;
u16 method;
u32 crc32;
u32 usize;
u32 csize;
u32 offset;
}ZPACK_STAT;


//internal prototypes

u32 ZPACK_GetDateTime();
u32 ZPACK_GetTimeMS();

u16 ZPACK_GetDate();
u16 ZPACK_GetTime();

ZPACK_CacheFile *ZPACK_LookupCache(ZPACK_Context *ctx, char *name);
ZPACK_CacheFile *ZPACK_GetCache(ZPACK_Context *ctx, char *name);
void ZPACK_DropCache(ZPACK_Context *ctx, char *name);
void ZPACK_CommitCacheAll(ZPACK_Context *ctx);

void ZPACK_InsertSpan(ZPACK_Context *ctx, u64 offs, u32 size, char *name);
int ZPACK_DeleteSpan(ZPACK_Context *ctx, u64 offs);
int ZPACK_NextSpan(ZPACK_Context *ctx, u64 offs);
int ZPACK_CheckSpan(ZPACK_Context *ctx, u64 offs, u32 size);
u64 ZPACK_FindMinSpan(ZPACK_Context *ctx, u32 size);

#else

typedef void ZPACK_Context;
typedef void ZPACK_FILE;
typedef void ZPACK_DIR;

typedef struct {
u32 date_time;
u16 flags;
u16 method;
u32 crc32;
u32 usize;
u32 csize;
u32 offset;
}ZPACK_STAT;

#endif

//generic prototypes

ZPACK_API void ZPACK_CreateImage(char *name);
ZPACK_API ZPACK_Context *ZPACK_OpenImage(char *name, int ro);
ZPACK_API int ZPACK_CommitImage(ZPACK_Context *ctx);
ZPACK_API int ZPACK_CloseImage(ZPACK_Context *ctx);
ZPACK_API int ZPACK_DeleteFile(ZPACK_Context *ctx, char *name);
ZPACK_API int ZPACK_StatFile(ZPACK_Context *ctx, char *name, ZPACK_STAT *stat);
ZPACK_API int ZPACK_SetFileTime(ZPACK_Context *ctx, char *name, u32 t);

ZPACK_API ZPACK_FILE *ZPACK_OpenFile(ZPACK_Context *ctx, char *name, char *mode);
ZPACK_API int ZPACK_Close(ZPACK_FILE *fd);
ZPACK_API int ZPACK_Flush(ZPACK_FILE *fd);
ZPACK_API int ZPACK_Read(ZPACK_FILE *fd, byte *buf, int sz);
ZPACK_API int ZPACK_Write(ZPACK_FILE *fd, byte *buf, int sz);
ZPACK_API u32 ZPACK_Seek(ZPACK_FILE *fd, u32 offs, int rel);
ZPACK_API u32 ZPACK_Tell(ZPACK_FILE *fd);
ZPACK_API int ZPACK_Eof(ZPACK_FILE *fd);

ZPACK_API ZPACK_DIR *ZPACK_OpenDir(ZPACK_Context *ctx, char *name);
ZPACK_API void ZPACK_CloseDir(ZPACK_DIR *dir);
ZPACK_API void ZPACK_RewindDir(ZPACK_DIR *dir);
ZPACK_API char *ZPACK_ReadDir(ZPACK_DIR *dir);
ZPACK_API int ZPACK_MkDir(ZPACK_Context *ctx, char *name);

#ifdef __cplusplus
}
#endif

#endif
