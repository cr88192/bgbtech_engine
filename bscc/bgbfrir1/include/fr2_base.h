#define FR2_RS_OK				( 0)
#define FR2_RS_ERROR			(-1)
#define FR2_RS_INVALIDOPCODE	(-2)	//bad opcode
#define FR2_RS_INVALIDOPARGS	(-3)	//bad opcode arguments
#define FR2_RS_INVALIDOPTYPE	(-4)	//bad opcode type prefix
#define FR2_RS_NOFRAME			(-5)

#define FR2_RS_RETURN			( 1)
#define FR2_RS_CALL				( 2)

union FR2_Value_s {
s8 sb;
u8 ub;
s16 ss;
u16 us;
s32 i;
s64 l;
f32 f;
f64 d;
FR2_PointerInfo *p;
void *a;
};

struct FR2_OpcodeInfo_s {
u16 d, s, t;
int c;
};

struct FR2_PointerInfo_s {
void *p;
byte step;
};

struct FR2_StaticVarInfo_s {
char *name;
char *sig;
char *flags;
void *ptr;
FR2_Value *val;
FR2_Value data;
};

struct FR2_ObjectModule_s {
FR2_Function *func;			//functions
FR2_EnvCtx *envctx;			//linked environment
FR2_StaticVarInfo **svar;
int n_svar, m_svar;

FR2_Function **sfunc;
int n_sfunc, m_sfunc;

char *strtab;
int sz_strtab, msz_strtab;

byte *blobtab;
int sz_blobtab, msz_blobtab;

byte *ctab_buf;
int *ctab_idx;
FR2_Value **ctab_val;
int sz_ctab, msz_ctab;
int n_ctab, m_ctab;
};

struct FR2_Function_s {
char *name;					//function name
char *sig;					//function signature
char *flags;
byte *ip;					//entry point IP (start of bytecode)
int sz_ip;					//size of bytecode
int n_regs;					//needed registers

FR2_Function *next;			//next function
FR2_ObjectModule *object;	//object module

FR2_EnvCtx *envctx;			//linked environment
FR2_Function *evnext;		//next function (environment)
};

struct FR2_RunFrame_s {
FR2_EnvCtx *env;			//owning environment
FR2_RunCtx *run;			//owning run-context
FR2_RunFrame *next;			//next (return) frame
FR2_Value *regs;			//current registers
FR2_Value *rtreg;			//return register
FR2_Function *func;			//current function
byte *ip;					//current IP
int n_regs;					//number of allocated registers
};

struct FR2_RunCtx_s {
FR2_IRunCtx *vt;			//vtable
FR2_EnvCtx *env;			//owning environment
FR2_RunFrame *frame;		//top of frame-stack
FR2_RunCtx *next;			//next run context
};

struct FR2_EnvCtx_s {
FR2_IEnvCtx *vt;			//vtable
FR2_RunCtx *run;			//running contexts
FR2_RunCtx *slab_run;		//slab of free run contexts
FR2_RunFrame *slab_frame;	//slab of free run frames

FR2_ObjectModule **objs;
int n_objs, m_objs;

FR2_StaticVarInfo **svar;
int n_svar, m_svar;

FR2_Function *func;			//toplevel functions
};

struct FR2_IRunCtx_s {
void *resv[4];
void (*Release)(FR2_RunCtx *self);
int (*Run)(FR2_RunCtx *self);
int (*RunWithRet)(FR2_RunCtx *self, FR2_Value **ret);
};

struct FR2_IEnvCtx_s {
void *resv[4];
FR2_RunCtx *(*NewRunCtx)(FR2_EnvCtx *self);
FR2_RunCtx *(*NewRunCtxCallArgs)(
	FR2_EnvCtx *self, FR2_Function *func, FR2_Value **args);
int (*LinkObject)(FR2_EnvCtx *self, FR2_ObjectModule *obj);
};
