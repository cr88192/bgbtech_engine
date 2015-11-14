#define BGBGC_GCEH_EXCEPTION	0xE015DEAD

#define GCEH_TRY_DEFAULT	0
#define GCEH_TRY_SYSTEM		1

// typedef struct BGBGC_Chan_s BGBGC_Chan;
typedef struct BGBGC_Queue_s BGBGC_Queue;

typedef struct BGBGC_MailMsg_s BGBGC_MailMsg;
typedef struct BGBGC_MailBox_s BGBGC_MailBox;

typedef struct th_context_s thContext;

typedef struct BGBGC_ExceptionType_s BGBGC_ExceptionType;
typedef struct BGBGC_Exception_s BGBGC_Exception;

typedef struct BGBGC_Job_s BGBGC_Job;
typedef struct BGBGC_WorkerInfo_s BGBGC_WorkerInfo;

typedef BGBGC_Exception *gcehException;

typedef struct BGBGC_RegisterState_s BGBGC_RegisterState;

struct BGBGC_Queue_s {
volatile void *msg[64];
volatile int irov, orov;
// volatile dyt ovf;
};


struct th_context_s {
int (*fcn)(void *ptr);
void *ptr;
int id;

void *hdl;
void *stktop;

//void *tls[1024];
void **tls;
int sz_tls;

void **rootstack;
int rootstackpos, szrootstack;
int *rootframe;
int rootframepos, szrootframe;
};


struct BGBGC_MailMsg_s {
BGBGC_MailMsg *next;
BGBGC_MailBox *to, *frm;
void *key;
void *val;
};

struct BGBGC_MailBox_s {
BGBGC_MailMsg *inbox;
BGBGC_MailMsg *outbox;
BGBGC_MailMsg *msg;
void *mtx;
};

struct BGBGC_ExceptionType_s {
BGBGC_ExceptionType *next;
BGBGC_ExceptionType *parent;
char *name;
};

struct BGBGC_Exception_s {
BGBGC_ExceptionType *type;	//type of exception
BGBGC_Exception *chain;		//chained exception (rethrows)
char *message;				//user message string
void *data;					//custom data
u32 id;						//magic ID number (OS-specific)
};

struct BGBGC_Job_s {
BGBGC_Job *next;
int time;						//time-stamp for job
byte done;						//job is has been executed
void *(*func)(void *data);		//job handling function
int (*blocked)(void *data);	//waiting condition
void *data;						//user data
void *val;						//final value
int workerId;					//worker ID (for special workers)
};

struct BGBGC_WorkerInfo_s {
int workerId;					//worker ID (for special workers)
void (*begin)(BGBGC_WorkerInfo *info);
void (*end)(BGBGC_WorkerInfo *info);
};

struct BGBGC_RegisterState_s {
int dummy;
#ifdef X86
u32 eax, ecx, edx, ebx;
u32 esp, ebp, esi, edi;
u32 eip, eflags;
#endif
#ifdef X86_64
u64 rax, rcx, rdx, rbx;
u64 rsp, rbp, rsi, rdi;
u64 r8, r9, r10, r11;
u64 r12, r13, r14, r15;
u64 rip, rflags;
#endif
};

typedef struct BGBGC_ImportFuncs_s {
void *(*GetAddr)(char *sym);
char *(*GetAddrName)(void *ptr);
char *(*GetAddrLastName)(void *ptr, void **rbp);
char *(*LookupSectionAddrName)(void *ptr);
}BGBGC_ImportFuncs_t;