
#define FRIR_RET_NONE		0
#define FRIR_RET_NORMAL		1
#define FRIR_RET_BADJUMP	2
#define FRIR_RET_BADOPCODE	3
#define FRIR_RET_BADMETHOD	4

typedef union FRIR_Value_u FRIR_Value;
typedef struct FRIR_LexEnvFrame_s FRIR_LexEnvFrame;
typedef struct FRIR_Frame_s FRIR_Frame;
typedef struct FRIR_Context_s FRIR_Context;
typedef struct FRIR_Method_s FRIR_Method;
// typedef struct FRIR_Label_s FRIR_Label;
typedef struct FRIR_Trace_s FRIR_Trace;
typedef struct FRIR_Opcode_s FRIR_Opcode;

typedef struct FRIR_StaticMethodInfo_s FRIR_StaticMethodInfo;
typedef struct FRIR_MethodCallInfo_s FRIR_MethodCallInfo;

typedef void (*FRIR_OpHandlerFcn)(FRIR_Context *ctx, FRIR_Opcode *op);
typedef FRIR_Trace *(*FRIR_TraceEndFcn)(FRIR_Context *ctx,
	FRIR_Trace *tr, FRIR_Opcode *op);

union FRIR_Value_u {
s32 i;						//int
s64 l;						//long
float f;					//float
double d;					//double
siptr a;					//address
void *pv;					//raw pointer
dyt r;						//variant
dytf rf;					//variant 2
struct { int x, y; }k;		//lexical env index
};

struct FRIR_LexEnvFrame_s {
FRIR_Value *vars;
int n_vars;
};

struct FRIR_Frame_s {
FRIR_Trace *trace;				//return trace
// int *ri;
// s64 *rl;
// float *rf;
// double *rd;
// siptr *ra;
FRIR_Value *r;
FRIR_Value *vspos;				//var stack pos
FRIR_Value *aspos;				//arg stack pos
FRIR_Value *rspos;				//reg stack pos

int reg_ret;					//return register
FRIR_Value *self;				//self value
FRIR_Value *top;				//toplevel

};

struct FRIR_Context_s {
FRIR_Trace *trace;				//current trace
FRIR_Frame **framestack;		//frame stack base
FRIR_Value *varstack;			//var stack base
FRIR_Value *argstack;			//arg stack base
FRIR_Value *regstack;			//register stack base

FRIR_Frame *frame;				//active frame
FRIR_Frame **fspos;				//frame stack position
FRIR_Value *vspos;				//var stack pos
FRIR_Value *aspos;				//arg stack pos
FRIR_Value *rspos;				//register stack pos

int ret;						//ctx return state (0=normal execution)

//register state (shared with frame)
FRIR_Value *r;
FRIR_Value *lvar;				//local var pos
FRIR_Value *larg;				//local arg pos
FRIR_LexEnvFrame **lenv;		//lexical environment

//int *ri;
//s64 *rl;
//float *rf;
//double *rd;
//siptr *ra;
};

struct FRIR_Method_s {
FRIR_Opcode **ops;				//first opcode in method
FRIR_Trace **trace;				//first trace in method
// FRIR_Label **label;
int n_ops;						//number of opcodes
int n_trace;					//number of traces
// int n_label;					//number of labels
int m_ops;						//max number of opcodes
int m_trace;					//max number of traces
// int m_label;

//temporary
int tr_start;
int tr_label;

//
int req_vars;					//required number of vars
int req_regs;					//required number of regs
};

// struct FRIR_Label_s {
// int lblnum;
// int opnum;
// };

struct FRIR_Trace_s {
FRIR_Trace *next;				//next trace (linear, per-method)
FRIR_Trace *jmpnext;			//jump-next trace (direct jump)
FRIR_Method *method;			//owning method
FRIR_Opcode *ops;				//opcodes in trace
int n_ops;						//number of opcodes
int label;						//label id
int jmp_label;					//jump target label
FRIR_Trace *(*run)(FRIR_Context *ctx, FRIR_Trace *tr);
FRIR_Trace *(*end)(FRIR_Context *ctx,
	FRIR_Trace *tr, FRIR_Opcode *op);
};

struct FRIR_Opcode_s {
void (*fcn)(FRIR_Context *ctx, FRIR_Opcode *op);
int op;
int a, b, c;
FRIR_Value i;
};

struct FRIR_StaticMethodInfo_s {
char *path;			//qualified path name
char *name;			//function/method name
char *sig;			//signature for call
FRIR_Method *mth;	//target method
};

struct FRIR_MethodCallInfo_s {
char *name;			//function/method name
char *sig;			//signature for call
int *args;			//register arg indices
int nargs;			//number of args
FRIR_Method *mth;	//target method
// int rret;		//register return
};


