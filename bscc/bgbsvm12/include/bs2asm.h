#include <bgbgc.h>
#include <bgbdy.h>
#include <bgbsvm.h>

#ifndef BS2ASM_H
#define BS2ASM_H

#if 0
#ifdef WIN32
#ifdef BGBSVM_DLL
#define BS2ASM_API __declspec(dllexport)
#else
#define BS2ASM_API __declspec(dllimport)
#endif
#else
#define BS2ASM_API
#endif
#endif

#define BS2ASM_SZIDXHASH	4096

#ifndef BTK_NAME
#define BTK_NAME	1
#define BTK_NUMBER	2
#define BTK_STRING	3
#define BTK_CHARSTRING	4
#define BTK_SEPERATOR	5
#define BTK_OPERATOR	6
#define BTK_BRACE	7
#define BTK_MISC	8
#define BTK_NULL	9
#define BTK_RESV_	10	//special, don't use ('\n')
#define BTK_BREAK	11	//linebreak, ...
#define BTK_COMMENT	12	//comment, ...
// #define BTK_INT		13
// #define BTK_FLOAT	14
#endif


typedef struct BS2ASM_ImageInfo_s		BS2ASM_ImageInfo;
typedef struct BS2ASM_ImageLump_s		BS2ASM_ImageLump;

typedef struct BS2ASM_EntityFieldInfo_s BS2ASM_EntityFieldInfo;
typedef struct BS2ASM_TagEntityAttr_s BS2ASM_TagEntityAttr;
// typedef struct BS2ASM_TagEntityLabel_s BS2ASM_TagEntityLabel;
typedef struct BS2ASM_TagEntity_s BS2ASM_TagEntity;
typedef struct BS2ASM_Context_s BS2ASM_Context;

typedef struct BS2ASM_TagEntity_s BS2ASM_Block;
typedef struct BS2ASM_TagEntity_s BS2ASM_List;


struct BS2ASM_ImageInfo_s {
BS2ASM_ImageLump **lump;		//lumps array
int n_lump;					//num lumps
int m_lump;					//max lumps
};

struct BS2ASM_ImageLump_s {
//raw fields
int ty_idx;					//lump index of lump type
int sz_data;				//size of lump data
byte *data;					//raw lump data

//ASM fields
BS2ASM_Context *ctx;		//owning context
BS2ASM_TagEntity *ent;		//associated entity
byte *dp;					//data output pointer
int msz_data;				//current data size

//in-memory field
char *ty_str;				//type-name string
void *value;				//type-specific value pointer
};

struct BS2ASM_EntityFieldInfo_s {
char *name;
char *type;
int offs;
int flag;
};

struct BS2ASM_TagEntityAttr_s {
BS2ASM_TagEntityAttr *next;
char *key;
char *value;
BS2ASM_TagEntity *entval;
};

// struct BS2ASM_TagEntityLabel_s {
// char *name;
// int offs;
// };

struct BS2ASM_TagEntity_s {
BS2ASM_TagEntity *next;
BS2ASM_TagEntity *first;
BS2ASM_TagEntityAttr *attr;
BS2ASM_ImageLump *lump;
BS2ASM_Context *ctx;
char *tagtype;

byte *ip;
byte *body;
int maxbody;
int szbody;
int litidx;			//literal index (current entity)
int validx;			//value index

char *name;			//name field for entity
char *type;			//type field for entity
int offs;			//offset (labels)
char *extname;		//name field for entity
char *flagstr;		//flags string field
char *value;		//flags string field

union {
s64 intval;
double realval;
}val;

//labels (blocks), also list items (lists)
BS2ASM_TagEntity **lbl;
int nlbl, mlbl;
};

struct BS2ASM_Context_s {
BS2ASM_TagEntity **lit;
int nlit, mlit;

BS2ASM_TagEntity *curscope;
BS2ASM_TagEntity *top;

int idxhash[BS2ASM_SZIDXHASH];
};

#include <bsvm_ops.h>
#include <bsvm_auto.h>

#endif
