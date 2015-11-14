typedef struct DYLL_BoxSig_s DYLL_BoxSig;
typedef struct DYLL_BoxVal_s DYLL_BoxVal;
typedef struct DYLL_BoxPtr_s DYLL_BoxPtr;

typedef struct DYLL_ValueTransfer_s DYLL_ValueTransfer;
typedef struct DYLL_ArgList_s DYLL_ArgList;

struct DYLL_BoxSig_s
{
char *sig;
};

struct DYLL_BoxVal_s
{
char *sig;
int sz;
byte data[1];
};

struct DYLL_BoxPtr_s
{
void *ptr;
void *base;
int cnt;
int step;
char *sig;
};

struct DYLL_ValueTransfer_s
{
void (*CopyValueBuf)(dycValue *v, void *p);
void (*CopyBufValue)(void *p, dycValue *v);
//...
int offs;
};

struct DYLL_ArgList_s
{
DYLL_ValueTransfer *args;	//argument handlers
int nargs;					//args count
int argsz;					//arg buffer size
DYLL_ValueTransfer *ret;	//return handler
char *sig;					//input signature
char *esig;					//return signature
void (*CopyValuesBuf)(DYLL_ArgList *ctx, dycValue *args, byte *buf);
void (*CopyBufValues)(DYLL_ArgList *ctx, byte *buf, dycValue *args);
//...
};

typedef struct PDUNZ2_Context_s PDUNZ2_Context;
struct PDUNZ2_Context_s
{
PDUNZ2_Context *next;	//next link

byte *ibuf;				//initial input buffer
byte *obuf;				//initial output buffer

byte *cs;				//current input pos
byte *cse;				//end of input buffer (trauncation)
byte *ct;				//current output pos
byte *ce;				//end of output buffer

u32 win;				//bit window
int pos;				//bit window position

u16 ltab_idx[256];		//literal index table
u16 dtab_idx[256];		//distance index table

u16 ltab_code[288];		//literal codes
u16 ltab_mask[288];		//literal code masks
u16 ltab_next[288];		//literal table code chains
byte ltab_len[288];		//literal code lengths

u16 dtab_code[32];		//distance codes
u16 dtab_mask[32];		//distance code masks
u16 dtab_next[32];		//distance table code chains
byte dtab_len[32];		//distance code lengths
};