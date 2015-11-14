#ifndef OBJTYPE_H
#define OBJTYPE_H

#define BGBGC_FIFL_ATOMIC			1
#define BGBGC_FIFL_EXEC				2
#define BGBGC_FIFL_SINGLEWRAP		4

typedef struct BGBGC_ObjType_s BGBGC_ObjType;
typedef struct BGBGC_HandleType_s BGBGC_HandleType;
typedef struct BGBGC_ObjInfo_s BGBGC_ObjInfo;
typedef struct BGBGC_SourceInfo_s BGBGC_SourceInfo;
typedef struct BGBGC_FastIndexInfo_s BGBGC_FastIndexInfo;

struct BGBGC_ObjType_s {
BGBGC_ObjType *next;
char *name;
char *form;
int size, idx;
int cnt, cntsz;

void *vtab;

// char **vtab_name;
// void **vtab_func;
// int n_vtab, m_vtab;

int (*mark)(void *p);		//custom mark handler
int (*destroy)(void *p);	//handle destroyed objects
int (*preserve)(void *p);	//object was preserved

// void *(*get_slot)	(void *p, char *s);
// void *(*set_slot)	(void *p, char *s, void *v);
// char *(*nxt_slot)	(void *p, char *s);
// void *(*call_method)	(void *p, char *s, void **a, int n);
// void *(*apply)		(void *p, void **a, int n);

// void *(*try_set_slot)	(void *p, char *s, void *v);
// void *(*get_del)	(void *p, char *s);
// void *(*set_del)	(void *p, char *s, void *v);
// char *(*nxt_del)	(void *p, char *s);
};

struct BGBGC_HandleType_s {
BGBGC_ObjType *next;
char *name;
int min, max;
void *buf;
};

struct BGBGC_ObjInfo_s {
u32 size;		//object size
u16 ty_idx;		//type index
u16 ty_chk;		//flags+check value
//8-16
u16 srcinfo;	//source-info
u16 flags;		//flags bits
u16 size2;		//more size bits
byte ty_idx2;	//more type index bits
byte srcinfo2;	//more source info
};

struct BGBGC_SourceInfo_s {
char *lfn;
int lln;
u32 access;		//access mode
};

struct BGBGC_FastIndexInfo_s {
// BGBGC_ObjType *type;
char *name;				//type name
int size;				//object size
int flags;				//allocation flags
int idx;				//fast-index index
void *freeobjs;			//free object list
void *lock;
};

#endif
