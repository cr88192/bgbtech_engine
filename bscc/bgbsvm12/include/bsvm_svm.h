/*
2^7=128, 3^5=243, 5^3=125, 7^2=49
190 512 000
2^5=32, 3^3=27, 5^2=25, 21600
*/

#define BSVM_PF_HINT_XI	1
#define BSVM_PF_HINT_XL	2
#define BSVM_PF_HINT_XF	4
#define BSVM_PF_HINT_XD	8
#define BSVM_PF_WIDE	16

#define BSVM_PF_ARR_MASK	0x0F00
#define BSVM_PF_ARR_XUBI	0x0100
#define BSVM_PF_ARR_XSBI	0x0200
#define BSVM_PF_ARR_XUSI	0x0300
#define BSVM_PF_ARR_XSSI	0x0400
#define BSVM_PF_ARR_XUDI	0x0500
#define BSVM_PF_ARR_XSDI	0x0600
#define BSVM_PF_ARR_XUDL	0x0700
#define BSVM_PF_ARR_XSDL	0x0800
#define BSVM_PF_ARR_XPP		0x0900	//pointer
#define BSVM_PF_ARR_XII		0x0A00
#define BSVM_PF_ARR_XLL		0x0B00
#define BSVM_PF_ARR_XFF		0x0C00
#define BSVM_PF_ARR_XDD		0x0D00

#define BSVM_PF_ARR_SIG		0x0F00	//signature used

#define BSVM_PF_ARR_IX		0x1000
#define BSVM_PF_HINT_PTR	0x2000	//pointer, will reuse array types

#define BSVM_PFHINT_JMPADDR		0x01000000	//jump target
#define BSVM_PFHINT_TRACEBREAK	0x02000000	//trace break
#define BSVM_PFHINT_TRACESTART	0x04000000	//trace start
#define BSVM_PFHINT_MAYTHROW	0x08000000	//may throw exception

#define BSVM_VMFL_VARSTACK	1

#define BSVM_VMGFL_JOINED		1
#define BSVM_VMGFL_DEBUGSTEP	2
#define BSVM_VMGFL_UNTRUSTED	4
#define BSVM_VMGFL_FROMSERVER	8
#define BSVM_VMGFL_FROMCLIENT	16

#define BSVM_RETS_NONE			0	//normal execution
#define BSVM_RETS_THROW			1	//thrown exception
#define BSVM_RETS_RETURN		2	//return
#define BSVM_RETS_BLOCK			3	//block temporarily
#define BSVM_RETS_SLEEP			4	//sleep operation
#define BSVM_RETS_ASSERT		5	//assert failed
#define BSVM_RETS_BREAKPT		6	//breakpoint trigger

#define BSVM_RETS_BOUNDSCHK		7	//bounds-check failed
#define BSVM_RETS_RIGHTSCHK		8	//rights-check failed
#define BSVM_RETS_NULLCHK		9	//null-check failed
#define BSVM_RETS_TYPECHK		10	//type-check failed


#define bsSet(rf, vl)		rf=vl


typedef struct BSVM_SVMState_s BSVM_SVMState;
typedef struct BSVM_ThreadOp_s BSVM_ThreadOp;
typedef struct BSVM_ThreadTrace_s BSVM_ThreadTrace;
typedef struct BSVM_ThreadCallInfo_s BSVM_ThreadCallInfo;
typedef struct BSVM_SVMContext_s BSVM_SVMContext;

// typedef struct {
// dyt lkey;
// dyt lval;
// dyt dkey;
// dyt dval;
// int nkey;
// }BSVM_Object;

struct BSVM_ThreadOp_s {
BSVM_ThreadOp *next;		//next opcode
BSVM_ThreadTrace *trace;	//owning trace
byte *ip;					//bytecode IP
BSVM_ThreadOp *(*fcn)(BSVM_SVMState *ctx, BSVM_ThreadOp *cur);
void *data;					//op specific data
dytf t, u, v;				//opcode values
int i, j, k;				//opcode indices
int pfhint;					//prefix hint flags
int opnum;
};

struct BSVM_ThreadTrace_s {
BSVM_ThreadTrace *next;		//next trace (linear, per-method)
BSVM_ThreadTrace *jmpnext;	//jump-next trace (direct jump)
BSVM_ThreadOp *thop;		//first opcode in trace
int n_thop;					//number of opcodes
BSVM_ThreadTrace *(*run)(BSVM_SVMState *ctx,
	BSVM_ThreadTrace *tr);
BSVM_ThreadTrace *(*end)(BSVM_SVMState *ctx,
	BSVM_ThreadTrace *tr, BSVM_ThreadOp *op);
byte cnt_jit;
};

struct BSVM_ThreadCallInfo_s {
BSVM_ThreadCallInfo *next;	//next callinfo
BSVM_ThreadOp *thop;		//owning opcode
void (*CallObj)(BSVM_SVMState *ctx,
	BSVM_ThreadCallInfo *cinf, dytf obj);
void (*TailCallObj)(BSVM_SVMState *ctx,
	BSVM_ThreadCallInfo *cinf, dytf obj);
dytf hdl;
dytf key;
};

typedef struct {
dytf self;
dytf top;
dytf lit;
dytf func;
dytf klenv;
dytf vlenv;
dytf kdenv;
dytf vdenv;
dytf oklenv;
dytf ovlenv;
dytf okdenv;
dytf ovdenv;
byte *ip;
byte *ipe;
int iv[4];

int stackpos;		//value stack position
int markstackpos;	//mark stack position
//int lestackpos;		//local env stack position (non-capture)
int vmflags;		//vm state flags

char *dbg_fn;	//debug filename
int dbg_ln;		//debug line number

dyAccessMode access;

#ifdef BSVM_JIT
int (*thunk)(BSVM_SVMState *ctx);
#endif

#ifdef BSVM_THREADCODE
BSVM_ThreadOp *thop;
BSVM_ThreadTrace *thtr;
#endif

#ifdef BSVM_VARSTACK
dytf *lekstack;		//local env stack (non-capture)
dytf *levstack;		//local env stack (non-capture)
int lestackpos;		//local env stack position (non-capture)
int olestackpos;	//old local env stack position
#endif

dycObject this_obj;
dycClass this_inf;

}BSVM_SVMFrame;


struct BSVM_SVMState_s {
dytf *stack;			//value stack
int stackpos;		//value stack position
int stacklim;
int ostackpos;		//old value stack position

int *markstack;		//mark stack
int markstackpos;	//mark stack position
int markstacklim;

#ifdef BSVM_VARSTACK
dytf *lekstack;		//local env stack (non-capture)
dytf *levstack;		//local env stack (non-capture)
int lestackpos;		//local env stack position (non-capture)
int lestacklim;		//local env stack limit (non-capture)
int olestackpos;	//old local env stack position
#endif

BSVM_SVMFrame **flowstack;
int flowstackpos;
int flowstacklim;

byte *ip;		//instruction pointer
byte *ipe;		//ip end

dytf self;		//self object
dytf top;		//toplevel object
dytf lit;		//literal table
dytf func;		//current function
dytf pkgname;	//current package name

dytf klenv;		//lexical env (keys)
dytf vlenv;		//lexical env (values)
dytf kdenv;		//dynamic env (keys)
dytf vdenv;		//dynamic env (values)

dytf oklenv;	//old lexical env (keys)
dytf ovlenv;	//old lexical env (values)
dytf okdenv;	//old dynamic env (keys)
dytf ovdenv;	//old dynamic env (values)

int args;	//base stack pos of args list
int eargs;	//base stack pos of end of args list

// dyt reg[16];
short sync_lock;

dyAccessMode access;
dytf thrown_obj;		//thrown obj (dispatching exception)
dytf thrown_obj2;	//thrown obj (during finally)

dytf thread_join;	//we are joining on another thread

#ifdef BSVM_JIT
int (*thunk)(BSVM_SVMState *ctx);
#endif

#ifdef BSVM_THREADCODE
BSVM_ThreadOp *thop;
BSVM_ThreadTrace *thtr;
int rs;
#endif

int pfhint;		//prefix hint
char *pfsig;	//prefix hint signature

int vmflags;	//vm state flags
int vmgflags;	//vm state global flags

char *dbg_fn;	//debug filename
int dbg_ln;		//debug line number

s32 *fbvt_int;
s64 *fbvt_long;
f32 *fbvt_float;
f64 *fbvt_double;

dycObject this_obj;
dycClass this_inf;

BSVM_SVMState *next;
};


typedef struct {
dyt mlenv;		//module env names
dyt mtenv;		//module env types
dyt mvenv;		//module env values
dyt mname;		//module name
dyt mimps;		//module imports
dyt mblks;		//module blocks
dyt mfcns;		//module funcs

dyt lenv, llenv;	//lex env names
dyt tenv, ltenv;	//lex env types
dyt venv, lvenv;	//lex env values

dyt msstruct;		//module serialized structs

dyt *lit;
byte *ip;

int lvl;		//block level, 0=toplevel, 1..=in function
int olvl;		//object level, 0=module, 1..=in object
int plvl;		//package level, 0=global, 1=in-package
int cap;		//set to indicate var capture
int pcap;		//package capture
int tail;		//this expression is in tail position
int i_cx_p;		//infer: expression is clean (cleared to indicate dirty)
int i_cap;		//infer: guess as to whether or not there is var capture

dyt cf_n;	//current function name
dyt cf_ty;	//current function type
dyt cf_fl;	//current function flags

dyt cs_fl;	//current statement flags
dyt cb_fl;	//current block flags

dyAccessMode access;
int vmgflags;

char **goto_name;
byte **goto_dest;
char **lbl_name;
byte **lbl_dest;
int n_goto, n_lbl;

char **contstack;
char **breakstack;
int contstackpos;
int breakstackpos;
}BSVM_SVMCState;

typedef struct BSVM_SVMBlock_s BSVM_SVMBlock;

struct BSVM_SVMBlock_s {
dyt type;	//func return type
dyt name;	//func name
dyt args;	//args list
dyt fargs;	//filtered args
dyt exps;	//source expressions
dyt lit;	//literals table
dyt attr;	//attributes
dyt lenv;	//lexical environment
byte *body;
int szbody;
#ifdef BSVM_JIT
int (*thunk)(BSVM_SVMState *ctx);
dyt th_name;	//thunk name (unique)
#endif
s64 flags;
dyAccessMode access;
int vmflags;

#ifdef BSVM_THREADCODE
BSVM_ThreadOp *thops;
BSVM_ThreadTrace *thtrace;
#endif

BSVM_SVMBlock *next;
BSVM_SVMBlock *chain;
};

typedef struct {
dytf lkey;
dytf lval;
dytf top;
dytf pkgname;
BSVM_SVMBlock *blk;
}BSVM_SVMLambda;

typedef struct {
char *name;
char *desc;
dytf (*fcn)(BSVM_SVMState *ctx, dytf args);
int nargs;
}BSVM_SVMBuiltin;

typedef struct {
dytf (*fcn)();
int nargs;
}BSVM_SVMFixed;

typedef struct BSVM_SVMModule_s {
dyt name;	//module name
dyt import;	//list of imported modules
dyt block;	//block defs
dyt funcs;	//toplevel functions block list

dyt lenv;	//top var names
dyt tenv;	//top var types
dyt venv;	//top var values (constants, ...)
}BSVM_SVMModule;


typedef struct BSVM_SVMVarInfo_s BSVM_SVMVarInfo;

struct BSVM_SVMVarInfo_s {
char *name;		//name of variable
char *sig;		//signature string
char *flsig;	//flag signature

//char *qname;	//qualified name ('<class>:<name>')

dytf value;
dytf attr;

//void *val;	//value (static slots)
//void *val_[4];	//if value is inline

//int hash;	//hash value
//int qhash;	//qualified name hash
//int offs;	//offset in object
//int size;	//size in bytes
//short type;	//slot lookup type
//short num;	//slot number

s64 flags;	//slot flags
dyAccessMode access;	//access control
};

typedef struct BSVM_SVMOverlayInfo_s BSVM_SVMOverlayInfo;

struct BSVM_SVMOverlayInfo_s {
char *name;
dytf attr;
int base;
int count;
int flags;
};

struct BSVM_SVMContext_s {
BSVM_SVMBlock *block;
BSVM_SVMBlock *ch_blk;
char *lfn;
int lln;
};
