#if 1

#define DYLL_EH_CODE 0xE0424742

#define DYLL_EH_OK		 0
#define DYLL_EH_RETHROW	 1

typedef struct BGBGC_Chan_s BGBGC_Chan;
// typedef struct BGBGC_Queue_s BGBGC_Queue;

struct BGBGC_Chan_s {
volatile char buf[64];
volatile signed char get, set;
volatile signed char gh, sh;
};

// struct BGBGC_Queue_s {
// volatile dyt msg[64];
// volatile int irov, orov;
// volatile dyt ovf;
// };


#if 0
typedef struct DYLL_TryFrame_s DYLL_TryFrame;
typedef struct DYLL_TryState_s DYLL_TryState;

struct DYLL_TryFrame_s {
DYLL_TryFrame *next;

byte *start_ip;
byte *end_ip;
};

struct DYLL_TryState_s {
DYLL_TryFrame *frm;
char try_p;
};
#endif

typedef struct DYLL_TryFrame_s *dyllException;

typedef struct DYLL_TryFrame_s DYLL_TryFrame;

struct DYLL_TryFrame_s {
DYLL_TryFrame *next;
void *jmp;
void *ptr;
};

#endif

#if 0

#define dyllBeginTry()	\
	DYLL_TryState *_try_;	\
	_try_=DYLL_BeginTry(); \
	if(_try_->try_p) {

#define dyllEndTry()	} DYLL_EndTry(_try_);

#define dyllBeginCatch(name) 	if(DYLL_CatchP(_try_, name)) {
#define dyllEndCatch()		}

#define dyllBeginUnwind() 	if(DYLL_UnwindP(_try_, name)) {
#define dyllEndUnwind()		}

#endif


#if 1

typedef struct DYLL_MRBC_Context_s DYLL_MRBC_Context;
typedef struct DYLL_MRBC2_Context_s DYLL_MRBC2_Context;

struct DYLL_MRBC_Context_s {
void **global;
void **stack;
char **ipstack;
int *markstack;
int stackpos;
int markstackpos;
int ipstackpos;

char *ip;
};

struct DYLL_MRBC2_Context_s {
dytf *global;
dytf *stack;
char **ipstack;
int *markstack;
int stackpos;
int markstackpos;
int ipstackpos;

char *ip;
};

#endif

#if 1

#define DYSH_CLR_BLACK		0
#define DYSH_CLR_BLUE		1
#define DYSH_CLR_GREEN		2
#define DYSH_CLR_CYAN		3
#define DYSH_CLR_RED		4
#define DYSH_CLR_MAGENTA	5
#define DYSH_CLR_BROWN		6
#define DYSH_CLR_WHITE		7
#define DYSH_CLR_GRAY		8
#define DYSH_CLR_HIBLUE		9
#define DYSH_CLR_HIGREEN	10
#define DYSH_CLR_HICYAN		11
#define DYSH_CLR_HIRED		12
#define DYSH_CLR_HIMAGENTA	13
#define DYSH_CLR_YELLOW		14
#define DYSH_CLR_HIWHITE	15

#define DYSH_CLR_DEFAULT	8	//default background color

#define DYSH_EFF_DEFAULT	0	//no effects
#define DYSH_EFF_BLINK		1	//text is blinking
#define DYSH_EFF_UNDERLINE	2	//text is underlined

typedef struct DYSH_ConsoleIface_s DYSH_ConsoleIface;
typedef struct DYSH_ConsoleData_s DYSH_ConsoleData;
typedef struct DYSH_Console_s DYSH_Console;
typedef struct DYSH_ShellIface_s DYSH_ShellIface;
typedef struct DYSH_Shell_s DYSH_Shell;
typedef struct DYSH_ConCmd_s DYSH_ConCmd;

typedef struct DYSH_Console_s *dyshConsole;
typedef struct DYSH_Shell_s *dyshShell;

struct DYSH_ConsoleIface_s {
void *resv0;
void *resv1;
void *resv2;
void *resv3;
void (*puts)(dyshConsole self, char *str);
};

struct DYSH_ShellIface_s {
void *resv0;
void *resv1;
void *resv2;
void *resv3;
void (*Puts)(dyshShell self, char *str);
void (*AddHistory)(dyshShell self, char *str);
char *(*GetHistory)(dyshShell self, int idx);
int (*ExecuteCmd)(dyshShell self, char *str);
int (*ExecuteScript)(dyshShell self, char *str);
char *(*Complete)(dyshShell self, char *str);
};

struct DYSH_ConsoleData_s {
char **conbuf;		//console buffer text
byte **conclr;		//console buffer colors
byte **coneff;		//console buffer effects
int szconbuf;
int width;			//console width (auto line break), 0=no limit

byte clr;
byte eff;
short xpos;
short ypos;
};

struct DYSH_Console_s {
DYSH_ConsoleIface *vt;	//vtable
DYSH_ConsoleData *cd;	//console data
void *data;				//implementation-specific data
};

struct DYSH_Shell_s {
DYSH_ShellIface *vt;
char **history;
int szhistory;
dyshConsole con;
void *data;
char *jmp_tgt;
};

struct DYSH_ConCmd_s {
DYSH_ConCmd *next;
char *name;
char *desc;
int (*func)(dyshShell shell, char **args);
};

#endif
