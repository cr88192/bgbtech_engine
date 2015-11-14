#ifndef BSVM_BASE_H
#define BSVM_BASE_H

#define BSVM_UOP_NUM		64
#define BSVM_BOP_NUM		64

#define BSVM_TYPEFL_UNSIGNED	1
#define BSVM_TYPEFL_EXTERN	2
#define BSVM_TYPEFL_STATIC	4
#define BSVM_TYPEFL_CONST	8
#define BSVM_TYPEFL_VOLATILE	16

#define BSVM_TYPEFL_NATIVE	32
#define BSVM_TYPEFL_PUBLIC	64
#define BSVM_TYPEFL_PRIVATE	128
#define BSVM_TYPEFL_PROTECTED	256
#define BSVM_TYPEFL_VIRTUAL	512
#define BSVM_TYPEFL_PERSISTENT	1024
#define BSVM_TYPEFL_INLINE	2048
#define BSVM_TYPEFL_SIGNED	4096

#define BSVM_TYPEFL_FUNCTION	65536	//is a function
#define BSVM_TYPEFL_CLASS	131072	//is a class

#define BSVM_TYPEFL_STDCALL	1048576	//'__stdcall'
#define BSVM_TYPEFL_CDECL	2097152	//'__cdecl'
#define BSVM_TYPEFL_PROXY	4194304	//'__proxy'


#define BSVM_TOKEN_NAME			1
#define BSVM_TOKEN_NUMBER		2
#define BSVM_TOKEN_STRING		3
#define BSVM_TOKEN_CHARSTRING	4
#define BSVM_TOKEN_SEPERATOR	5
#define BSVM_TOKEN_OPERATOR		6
#define BSVM_TOKEN_BRACE		7
#define BSVM_TOKEN_MISC			8
#define BSVM_TOKEN_NULL			9
#define BSVM_TOKEN_INT			10
#define BSVM_TOKEN_FLOAT		11
#define BSVM_TOKEN_STRING_OVF	12	//overlong string

#define BSVM_COMPFL_INARGS	1	//compiling function args

#define BSVM_COMPFL_WARNING	256	//warnings are present
#define BSVM_COMPFL_ERROR	512	//errors are present
#define BSVM_COMPFL_FATAL	1024	//fatal errors are present

typedef struct BSVM_ParseContext_s BSVM_ParseContext;
typedef struct BSVM_ParseItem_s BSVM_ParseItem;
typedef struct BSVM_CompileItem_s BSVM_CompileItem;
typedef struct BSVM_CompileContext_s BSVM_CompileContext;

typedef struct BSVM_Block_s BSVM_Block;
typedef struct BSVM_FunArg_s BSVM_FunArg;
typedef struct BSVM_VarAttr_s BSVM_VarAttr;

typedef struct BSVM_TextFrag_s BSVM_TextFrag;
typedef struct BSVM_BlockFrag_s BSVM_BlockFrag;
typedef struct BSVM_CgenLabel_s BSVM_CgenLabel;
typedef struct BSVM_CgenCtx_s BSVM_CgenCtx;

//serves as a context for both the parser and the reducer
struct BSVM_ParseCtx_s {
dyt body;	//all previously parsed code
dyt ctdef;	//"complex" types (struct/unions/...)
};


struct BSVM_ParseItem_s {
BSVM_ParseItem *next;
char *name;

//dyt (*func)(char **s);
dytf (*func)(BSVM_ParseContext *ctx, char **s);
};


struct BSVM_VarAttr_s {
BSVM_VarAttr *next;
char *name;
char *value;
};

struct BSVM_FunArg_s {
BSVM_FunArg *next;
BSVM_FunArg *first;	//sub-pattern
BSVM_VarAttr *attr;

int ty; //1=var, 2=rest, 3=literal
char *name;	//var name
char *type;	//type base name
char *rtype;	//type rel name
int mode;	//type mode flags
int value;	//value index
int order;	//array order
short arrsz[8];	//array sizes
};

struct BSVM_Block_s {
BSVM_Block *chain;		//chained functions
unsigned short *optable;
void **valuetable;
int oplen;
int nvalues;

BSVM_FunArg *args;

char *name;
dyt src_args, src_body;
int flags;	//block/compile-state flags
};

struct BSVM_CompileItem_s {
BSVM_CompileItem *next;
char *name;

u16 *(*func)(u16 *ip, BSVM_CompileContext *ctx, dyt expr);
};

struct BSVM_CompileContext_s {
BSVM_CompileContext *next;
char *name;
u32 crc;	//source file checksum
int flag;	//generic flags, 1&=just compiled

dyt src;	//previously parsed code

void **values;
BSVM_FunArg *env;	//current environment scope
BSVM_VarAttr *attr;	//general commands/attributes
};



struct BSVM_TextFrag_s {
BSVM_TextFrag *next;
char *text;
};

struct BSVM_BlockFrag_s {
BSVM_BlockFrag *next;
char *name;
BSVM_Block *blk;
};

struct BSVM_CgenLabel_s {
BSVM_CgenLabel *next;
char *name;
unsigned short *ip;
};

struct BSVM_CgenCtx_s {
char *name, *modname, *initname;

BSVM_TextFrag *headtop;	//header top
BSVM_TextFrag *headbody;	//header body

BSVM_TextFrag *top;		//top of the file
BSVM_TextFrag *funcs;	//functions and similar

BSVM_TextFrag *inittop;	//top of init stub
BSVM_TextFrag *initbody;	//body of init stub
BSVM_TextFrag *tmptop;	//top of cur func
BSVM_TextFrag *tmpbody;	//body of cur func/struct/...

BSVM_BlockFrag *blocks;	//converted blocks
};

#endif
