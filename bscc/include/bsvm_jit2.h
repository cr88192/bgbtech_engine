#define BSVM_JIT2_REGTY_BYTE		1
#define BSVM_JIT2_REGTY_WORD		2
#define BSVM_JIT2_REGTY_DWORD		3
#define BSVM_JIT2_REGTY_QWORD		4
#define BSVM_JIT2_REGTY_XMM			5
#define BSVM_JIT2_REGTY_XMM_PS		6
#define BSVM_JIT2_REGTY_XMM_PD		7
#define BSVM_JIT2_REGTY_XMM_SS		8
#define BSVM_JIT2_REGTY_XMM_SD		9

#define BSVM_JIT2_BINOP_ADD			0
#define BSVM_JIT2_BINOP_SUB			1
#define BSVM_JIT2_BINOP_MUL			2
#define BSVM_JIT2_BINOP_DIV			3
#define BSVM_JIT2_BINOP_MOD			4
#define BSVM_JIT2_BINOP_AND			5
#define BSVM_JIT2_BINOP_OR			6
#define BSVM_JIT2_BINOP_XOR			7
#define BSVM_JIT2_BINOP_SHL			8
#define BSVM_JIT2_BINOP_SHR			9
#define BSVM_JIT2_BINOP_EQ			10
#define BSVM_JIT2_BINOP_EQV			11
#define BSVM_JIT2_BINOP_NEQ			12
#define BSVM_JIT2_BINOP_NEQV		13
#define BSVM_JIT2_BINOP_L			14
#define BSVM_JIT2_BINOP_G			15
#define BSVM_JIT2_BINOP_LE			16
#define BSVM_JIT2_BINOP_GE			17
#define BSVM_JIT2_BINOP_IDIV		18
#define BSVM_JIT2_BINOP_DADD		19
#define BSVM_JIT2_BINOP_DSUB		20
#define BSVM_JIT2_BINOP_DMUL		21
#define BSVM_JIT2_BINOP_DDIV		22
#define BSVM_JIT2_BINOP_DMOD		23
#define BSVM_JIT2_BINOP_DAND		24
#define BSVM_JIT2_BINOP_DOR			25
#define BSVM_JIT2_BINOP_DXOR		26
#define BSVM_JIT2_BINOP_DIDIV		27
#define BSVM_JIT2_BINOP_EXP			28
#define BSVM_JIT2_BINOP_SHRR		29
#define BSVM_JIT2_BINOP_CONS		30
#define BSVM_JIT2_BINOP_UNORD		31

#define BSVM_JIT2_UNOP_NEG			(256|0)
#define BSVM_JIT2_UNOP_NOT			(256|1)
#define BSVM_JIT2_UNOP_LNOT			(256|2)
#define BSVM_JIT2_UNOP_POS			(256|3)
#define BSVM_JIT2_UNOP_UMUL			(256|4)
#define BSVM_JIT2_UNOP_UDIV			(256|5)
#define BSVM_JIT2_UNOP_UIDIV		(256|6)
#define BSVM_JIT2_UNOP_UMOD			(256|7)
#define BSVM_JIT2_UNOP_UAND			(256|8)
#define BSVM_JIT2_UNOP_UOR			(256|9)
#define BSVM_JIT2_UNOP_UXOR			(256|10)
#define BSVM_JIT2_UNOP_ULAND		(256|11)
#define BSVM_JIT2_UNOP_ULOR			(256|12)
#define BSVM_JIT2_UNOP_ULXOR		(256|13)
#define BSVM_JIT2_UNOP_INC			(256|14)
#define BSVM_JIT2_UNOP_DEC			(256|15)
#define BSVM_JIT2_UNOP_DEFER		(256|16)
#define BSVM_JIT2_UNOP_CAR			(256|17)
#define BSVM_JIT2_UNOP_CDR			(256|18)
#define BSVM_JIT2_UNOP_COPYVALUE	(256|19)
#define BSVM_JIT2_UNOP_DROPVALUE	(256|20)
#define BSVM_JIT2_UNOP_DELETE		(256|21)
#define BSVM_JIT2_UNOP_CLONE		(256|22)



#define BSVM_JIT2_ARCH_DEFAULT	0	//any arch
#define BSVM_JIT2_ARCH_X86		1	//32-bit x86
#define BSVM_JIT2_ARCH_X64		2	//64-bit x86 / x86-64
#define BSVM_JIT2_ARCH_ARM		3	//ARM (32-bit fixed-width)
#define BSVM_JIT2_ARCH_PPC		4	//PPC
#define BSVM_JIT2_ARCH_PPC64	5	//PPC64
#define BSVM_JIT2_ARCH_THUMB	6	//ARM (Thumb mode)
#define BSVM_JIT2_ARCH_ARM64	7	//ARM (64-bit fixed-width)

#define BSVM_JIT2_VARTY_NONE	0	//no storage
#define BSVM_JIT2_VARTY_GLOBAL	1	//global variable (C toplevel)
#define BSVM_JIT2_VARTY_LOCAL	2	//local variable
#define BSVM_JIT2_VARTY_ARG		3	//function argument
#define BSVM_JIT2_VARTY_TVAR	4	//temporary variable
#define BSVM_JIT2_VARTY_STACK	5	//stack location
#define BSVM_JIT2_VARTY_TARG	6	//call argument

#define BSVM_JIT2_ABITY_NONE	0	//none / default
#define BSVM_JIT2_ABITY_CDECL	1	//cdecl (x86)
#define BSVM_JIT2_ABITY_STDCALL	2	//stdcall (x86)
#define BSVM_JIT2_ABITY_WIN64	3	//Win64 (x86-64)
#define BSVM_JIT2_ABITY_AMD64	4	//SysV/AMD64 (x86-64)
#define BSVM_JIT2_ABITY_BSVMI	5	//BGBScript VM Internal

#define BSVM_Jit2_StubError(ctx) \
	BSVM_Jit2_StubErrorLLn(ctx, __FILE__, __LINE__)
#define BSVM_Jit2_CaseError(ctx) \
	BSVM_Jit2_CaseErrorLLn(ctx, __FILE__, __LINE__)
#define BSVM_Jit2_TypeCheckError(ctx) \
	BSVM_Jit2_TypeCheckErrorLLn(ctx, __FILE__, __LINE__)
#define BSVM_Jit2_SanityCheckError(ctx, str) \
	BSVM_Jit2_SanityCheckErrorLLn(ctx, __FILE__, __LINE__, str)

#define BSVM_Jit2_Warn(ctx, str, ...) \
	BSVM_Jit2_WarnLLn(ctx, __FILE__, __LINE__, str, __VA_ARGS__)
#define BSVM_Jit2_Error(ctx, str, ...) \
	BSVM_Jit2_ErrorLLn(ctx, __FILE__, __LINE__, str, __VA_ARGS__)

#define bsj2_print(ctx, ...) \
	bsj2_print_lln(ctx, __FILE__, __LINE__, __VA_ARGS__)
#define bsj2_printlog(ctx, ...) \
	bsj2_printlog_lln(ctx, __FILE__, __LINE__, __VA_ARGS__)
#define bsj2_printdata(ctx, ...) \
	bsj2_printdata_lln(ctx, __FILE__, __LINE__, __VA_ARGS__)
#define bsj2_printbss(ctx, ...) \
	bsj2_printbss_lln(ctx, __FILE__, __LINE__, __VA_ARGS__)
#define bsj2_printasm(ctx, ...) \
	bsj2_printasm_lln(ctx, __FILE__, __LINE__, __VA_ARGS__)


typedef struct BSVM_Jit2_TypeInfo_s BSVM_Jit2_TypeInfo;
typedef struct BSVM_Jit2_VarInfo_s BSVM_Jit2_VarInfo;
typedef struct BSVM_Jit2_MemInfo_s BSVM_Jit2_MemInfo;
typedef struct BSVM_Jit2_Context_s BSVM_Jit2_Context;
typedef struct BSVM_Jit2_TopContext_s BSVM_Jit2_TopContext;

typedef BSVM_Jit2_TypeInfo *bsjType;
typedef BSVM_Jit2_VarInfo *bsjVar;
typedef BSVM_Jit2_VarInfo *bsjReg;
typedef BSVM_Jit2_MemInfo *bsjMem;
typedef BSVM_Jit2_Context *bsjContext;
typedef BSVM_Jit2_TopContext *bsjTopContext;

struct BSVM_Jit2_TypeInfo_s
{
BSVM_Jit2_TopContext *top;
BSVM_Jit2_TypeInfo *next;
char *sig;
};

struct BSVM_Jit2_VarInfo_s
{
BSVM_Jit2_VarInfo *next;	//next in relevant chain
BSVM_Jit2_VarInfo *lnext;	//next in lexical chain
BSVM_Jit2_Context *ctx;
char *name;		//variable name
bsjType type;	//var/reg type
bsjReg reg;		//bound register (var only)
int offs;		//stack-frame offset (var only)
int regidx;		//register index (CPU specific, reg only)
bsjVar mapvar;	//mapped var (reg only)
byte varty;		//variable storage type
dyt value;		//constant value (stack)
};

struct BSVM_Jit2_MemInfo_s {
BSVM_Jit2_Context *ctx;
char *name;
bsjReg breg;
bsjReg ireg;
int scale;
int disp;
int esp;	//stack position
};

struct BSVM_Jit2_Context_s
{
BSVM_Jit2_TopContext *top;
BSVM_Jit2_VarInfo *locals;	//plain local variables
BSVM_Jit2_VarInfo *args;	//plain arguments
BSVM_Jit2_VarInfo *lexenv;	//lexical environment

bsjReg reg[16];
bsjReg xreg[16];

bsjVar stack[256];
int stackpos;

int markstack[64];
int markstackpos;

byte var_mask[64];
int n_var;

int mask_usedregs;		//used registers
int mask_usedxregs;		//used XMM registers
int n_passes;			//n_passes
int abitype;			//ABI type in use

//CPU stack
int vstack_offs;	//current stack position

int offs_local;		//offset for locals
int offs_tvar;		//offset for tvars
int offs_vstack;	//offset for value stack
int offs_targ;		//offset for temp arguments
int offs_savereg;	//offset for saved registers

int sz_local;		//size of locals (bytes)
int sz_tvar;		//size of tvars (bytes)
int sz_vstack;		//size of value stack (bytes)
int sz_targ;		//size of temp arguments (bytes)
int sz_args;		//size of function arguments (bytes)

int sz_savereg;		//size of saved registers
int sz_frame;		//combined size of stack frame

char *lbl_func;		//label for function
char *lbl_return;	//return label

//output
char *asm_cts;		//ASM output buffer start
char *asm_cte;		//ASM output buffer end
char *asm_ct;		//ASM output buffer position

char *log_cts;		//debug log output buffer start
char *log_cte;		//debug log output buffer end
char *log_ct;		//debug log output buffer position

char *text_cts;		//text section output buffer start
char *text_cte;		//text section output buffer end
char *text_ct;		//text section output buffer position

char *data_cts;		//data section output buffer start
char *data_cte;		//data section output buffer end
char *data_ct;		//data section output buffer position

char *bss_cts;		//bss section output buffer start
char *bss_cte;		//bss section output buffer end
char *bss_ct;		//bss section output buffer position

//bytecode state
byte *ip;			//current bytecode position
byte *ips;			//current bytecode start
byte *ipe;			//current bytecode end
dyt lit;			//current literal table

// char *cf_n;				//current function name
dyt func;				//function being compiled
BSVM_SVMBlock *blk;		//block definition

int n_lbl;
byte *lbl[256];
char *lbln[256];
};

struct BSVM_Jit2_TopContext_s
{
BSVM_Jit2_TypeInfo *type;
byte arch;		//CPU arch number
byte nojit;		//is not JIT
};
