#include <bgbgc.h>

#ifdef WIN32
#include <windows.h>
#endif

#ifdef linux
#include <unistd.h>
#include <pthread.h>
#endif

#ifdef NATIVECLIENT
#include <unistd.h>
#include <pthread.h>
#endif

// extern void *bgbgc_alloc_mutex;

// void *bgbgc_thread_hdl[1024];
// void *bgbgc_thread_stktop[1024];
thContext *bgbgc_thread_inf[1024];

int bgbgc_thread_num=0;

int bgbgc_threadctx_tls=-1;
int bgbgc_threadtls_tls=-1;
int bgbgc_thread_tlsrov=1;

char *bgbgc_thread_tlsname[1024];


#ifdef WIN32
HANDLE bgbgc_thread_ptrevent;
#endif

// #ifdef linux
#if defined(linux) || defined(NATIVECLIENT)
pthread_key_t bgbgc_threadctx_tls2;

pthread_mutex_t bgbgc_threadptr_mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t bgbgc_threadptr_cond=PTHREAD_COND_INITIALIZER;
#endif

volatile long bgbgc_thread_ptrwait=0;
volatile long bgbgc_thread_ptrwait2=0;
volatile void *bgbgc_thread_sigptr=NULL;


#ifdef BGBGC_USEPRECISE

void BGBGC_PushRootFrame()
{
	thContext *inf;
	int i;

	inf=thGetContext();

	if(!inf->rootframe)
	{
		inf->rootframe=malloc(256*sizeof(int));
		inf->rootframepos=0;
		inf->szrootframe=256;
	}

	if(!inf->rootstack)
	{
		inf->rootstack=malloc(1024*sizeof(void *));
		inf->rootstackpos=0;
		inf->szrootstack=1024;
	}

	if((inf->rootframepos+1)>=inf->szrootframe)
	{
		i=inf->szrootframe+(inf->szrootframe>>1);
		inf->rootframe=realloc(inf->rootframe, i*sizeof(int));
		inf->szrootframe=i;
	}

	inf->rootframe[inf->rootframepos++]=
		inf->rootstackpos;
}

void BGBGC_PopRootFrame()
{
	thContext *inf;
	void *p;
	int i;

	inf=thGetContext();

	inf->rootframepos--;
	i=inf->rootframe[inf->rootframepos];

	while(inf->rootstackpos>i)
	{
		inf->rootstackpos--;
		p=inf->rootstack[inf->rootstackpos];
		BGBGC_CollectAssignPrecise((void **)p, NULL);
	}
}

void BGBGC_PushFrameRoot(void **ref)
{
	thContext *inf;
	int i;

	inf=thGetContext();

	if((inf->rootstackpos+1)>=inf->szrootstack)
	{
		i=inf->szrootstack+(inf->szrootstack>>1);
		inf->rootstack=realloc(inf->rootstack, i*sizeof(void *));
		inf->szrootstack=i;
	}

	if(!inf->rootframepos)
		BGBGC_PushRootFrame();

	*ref=NULL;
	inf->rootstack[inf->rootstackpos++]=(void *)ref;
}

#endif


void bgbgc_thread_markctx(thContext *inf)
{
	void *p, *q;
	int i, j;

	if(!inf)return;

	p=inf->stktop;
	if(!p)return;
	if(p==UNDEFINED)return;

//	j=(1<<16)-256;
	j=((1<<18)-6144)/sizeof(void *);

	q=(void *)(((void **)p)-j);
	BGBGC_ScanRange((void **)q, j);

	if(inf->tls)
		BGBGC_ScanRange(inf->tls, bgbgc_thread_tlsrov);

#ifdef BGBGC_USEPRECISE
	for(i=0; i<inf->rootstackpos; i++)
		BGBGC_MarkPrecise(*(void **)(inf->rootstack[i]));
#endif
}

void bgbgc_thread_mark()
{
	void *p, *q;
	int i, j;

	for(i=0; i<bgbgc_thread_num; i++)
	{
//		p=bgbgc_thread_stktop[i];
//		if(!p)continue;
//		if(p==UNDEFINED)continue;

//		j=(1<<16)-256;
//		q=(void *)(((void **)p)-j);
//		BGBGC_ScanRange((void **)q, j);

		bgbgc_thread_markctx(bgbgc_thread_inf[i]);
	}
}

void bgbgc_thread_init()
{
	static int init=0;

	if(init)return;
	init=1;

	BGBGC_AddMarkHandler(bgbgc_thread_mark);

//	bgbgc_alloc_mutex=thMutex();

#ifdef WIN32
	bgbgc_threadctx_tls=TlsAlloc();
	bgbgc_threadtls_tls=TlsAlloc();
	bgbgc_thread_ptrevent=CreateEvent(
		NULL, TRUE, FALSE, TEXT("PointerEvent")); 
#endif

//#ifdef linux
#if defined(linux) || defined(NATIVECLIENT)
	pthread_key_create(&bgbgc_threadctx_tls2, NULL);
	pthread_mutex_init(&bgbgc_threadptr_mutex, NULL);
	pthread_cond_init(&bgbgc_threadptr_cond, NULL);
#endif

	thGetContext();	//make sure TLS set up
}

BGBGC_API int thAllocTls()
{
	int i;

	i=bgbgc_thread_tlsrov++;
	bgbgc_thread_tlsname[i]="";
	return(i);
}

BGBGC_API int thAllocTlsName(char *name, int sz)
{
	int i, j, n;

	for(i=0; i<bgbgc_thread_tlsrov; i++)
		if(bgbgc_thread_tlsname[i] &&
			!strcmp(bgbgc_thread_tlsname[i], name))
				return(i);

	n=(sz+(sizeof(void *)-1))/sizeof(void *);
	i=bgbgc_thread_tlsrov;
	bgbgc_thread_tlsrov+=n;
	bgbgc_thread_tlsname[i]=strdup(name);
	for(j=i+1; j<bgbgc_thread_tlsrov; j++)
		bgbgc_thread_tlsname[j]="~";

	return(i);
}

BGBGC_API void thFreeTls(int idx)
{
}

BGBGC_API void *thGetTlsValue(int idx)
{
	thContext *inf;
	inf=thGetContext();
	if(!inf || !inf->tls)
		return(NULL);
	return(inf->tls[idx]);
}

BGBGC_API void thSetTlsValue(int idx, void *val)
{
	thContext *inf;
	inf=thGetContext();
	inf->tls[idx]=val;
}

BGBGC_API void *thGetTlsPtr(int idx)
{
	thContext *inf;
	inf=thGetContext();
	return(&(inf->tls[idx]));
}

#ifdef WIN32
DWORD WINAPI BGBGC_ThreadProc(LPVOID lpParam) 
{
	thContext *inf;
	int i;

	inf=lpParam;
	TlsSetValue(bgbgc_threadctx_tls, (void *)inf);

	if(!inf->tls)
	{
		inf->tls=malloc(1024*sizeof(void *));
		memset(inf->tls, 0, 1024*sizeof(void *));
		inf->sz_tls=1024;

//		TlsSetValue(bgbgc_threadctx_tls, (void *)inf);
		TlsSetValue(bgbgc_threadtls_tls, (void *)(inf->tls));
	}

//	bgbgc_thread_stktop[inf->id]=&lpParam;
//	bgbgc_thread_inf[inf->id]=inf;

	bgbgc_thread_inf[inf->id]=inf;
	inf->stktop=&lpParam;

	i=inf->fcn(inf->ptr);

//	bgbgc_thread_hdl[inf->id]=NULL;
//	bgbgc_thread_stktop[inf->id]=NULL;

	inf->hdl=NULL; inf->stktop=NULL;
	bgbgc_thread_inf[inf->id]=NULL;
	return(i);
}

BGBGC_API void *thThread(int (*fcn)(void *ptr), void *ptr)
{
	thContext *inf;
	HANDLE hth;
	DWORD thid;
	int i;

	bgbgc_thread_init();

//	for(i=0; i<bgbgc_thread_num; i++)
//		if(!bgbgc_thread_hdl[i])
//			break;

	for(i=0; i<bgbgc_thread_num; i++)
		if(!bgbgc_thread_inf[i])
			break;

	if(i>=bgbgc_thread_num)i=bgbgc_thread_num++;

	inf=malloc(sizeof(thContext));
	memset(inf, 0, sizeof(thContext));

	inf->id=i;

//	bgbgc_thread_hdl[i]=UNDEFINED;
//	bgbgc_thread_stktop[i]=UNDEFINED;

	inf->hdl=UNDEFINED;
	inf->stktop=UNDEFINED;

	inf->fcn=fcn;
	inf->ptr=ptr;

	hth=CreateThread(NULL, 1<<18,
		BGBGC_ThreadProc, (LPVOID)(inf),
		0, &thid);

//	bgbgc_thread_hdl[i]=hth;
	inf->hdl=hth;

	printf("Th %p %d %p\n", hth, thid, fcn);

	return(hth);
}


void BGBGC_SuspendThreads()
{
	HANDLE hth;
	thContext *inf;
	int i;

	inf=thGetContext();

	for(i=0; i<bgbgc_thread_num; i++)
	{
		if(!bgbgc_thread_inf[i])
			continue;

		hth=bgbgc_thread_inf[i]->hdl;
		if(hth==inf->hdl)continue;
		SuspendThread(hth);
	}
}

void BGBGC_ResumeThreads()
{
	HANDLE hth;
	thContext *inf;
	int i;

	inf=thGetContext();

	for(i=0; i<bgbgc_thread_num; i++)
	{
		if(!bgbgc_thread_inf[i])
			continue;

		hth=bgbgc_thread_inf[i]->hdl;
		if(hth==inf->hdl)continue;
		ResumeThread(hth);
	}
}

void BGBGC_HandleThreadStates(
	void (*fcn)(thContext *inf))
{
	HANDLE hth;
	thContext *inf;
	int i;

	inf=thGetContext();

	for(i=0; i<bgbgc_thread_num; i++)
	{
		if(!bgbgc_thread_inf[i])
			continue;

		hth=bgbgc_thread_inf[i]->hdl;
		if(hth==inf->hdl)continue;
		SuspendThread(hth);
		fcn(bgbgc_thread_inf[i]);
		ResumeThread(hth);
	}
}

void BGBGC_GetThreadRegisterState(
	thContext *inf, BGBGC_RegisterState *regs)
{
	CONTEXT rctx;
	
	rctx.ContextFlags=CONTEXT_INTEGER|CONTEXT_CONTROL;
	GetThreadContext(inf->hdl, &rctx);
	
#ifdef X86
	regs->eax=rctx.Eax;		regs->ecx=rctx.Ecx;
	regs->edx=rctx.Edx;		regs->ebx=rctx.Ebx;
	regs->esp=rctx.Esp;		regs->ebp=rctx.Ebp;
	regs->esi=rctx.Esi;		regs->edi=rctx.Edi;

	regs->eip=rctx.Eip;
	regs->eflags=rctx.EFlags;
#endif

#ifdef X86_64
	regs->rax=rctx.Rax;		regs->rcx=rctx.Rcx;
	regs->rdx=rctx.Rdx;		regs->rbx=rctx.Rbx;
	regs->rsp=rctx.Rsp;		regs->rbp=rctx.Rbp;
	regs->rsi=rctx.Rsi;		regs->rdi=rctx.Rdi;
	regs->r8=rctx.R8;		regs->r9=rctx.R9;
	regs->r10=rctx.R10;		regs->r11=rctx.R11;
	regs->r12=rctx.R12;		regs->r13=rctx.R13;
	regs->r14=rctx.R14;		regs->r15=rctx.R15;

	regs->rip=rctx.Rip;
	regs->rflags=rctx.RFlags;
#endif
}

#if 0
BGBGC_API void *thMutex()
{
	HANDLE hdl; 
	hdl=CreateMutex(NULL, FALSE, NULL);
	return(hdl);
}

BGBGC_API void thLockMutex(void *p)
{
	if(!p)return;
	WaitForSingleObject((HANDLE)p, INFINITE);
}

BGBGC_API int thTryLockMutex(void *p)
{
	int i;

	if(!p)return(-1);
	i=WaitForSingleObject((HANDLE)p, 0);
	if(i==WAIT_TIMEOUT)return(-1);
	return(0);
}

BGBGC_API void thUnlockMutex(void *p)
{
	if(!p)return;
	ReleaseMutex((HANDLE)p);
}

BGBGC_API void thFreeMutex(void *p)
{
	if(!p)return;
//	gcfree(p);
}
#endif

#if 1
BGBGC_API void *thMutex()
{
	CRITICAL_SECTION *pCS;
	pCS=gcatomic(sizeof(CRITICAL_SECTION));
	InitializeCriticalSectionAndSpinCount(pCS, 0x00000400);
	return(pCS);
}

BGBGC_API void thLockMutex(void *p)
{
	if(!p)return;
	EnterCriticalSection((CRITICAL_SECTION *)p);
}

BGBGC_API int thTryLockMutex(void *p)
{
	int i;

	if(!p)return(-1);
	i=TryEnterCriticalSection((CRITICAL_SECTION *)p);
	if(!i)return(-1);
	return(0);
}

BGBGC_API void thUnlockMutex(void *p)
{
	if(!p)return;
	LeaveCriticalSection((CRITICAL_SECTION *)p);
}

BGBGC_API void thFreeMutex(void *p)
{
	if(!p)return;
	DeleteCriticalSection((CRITICAL_SECTION *)p);
	gcfree(p);
}
#endif

#if 1
BGBGC_API void *thFastMutex()
{
	int *pi;
	pi=gcatomic(sizeof(int));
	*pi=0;
	return((void *)pi);
}

BGBGC_API void thLockFastMutex(void *p)
{
	int i;

	if(!p)return;
	while(InterlockedExchange((PLONG)p, 2))
		Sleep(0);
	*(volatile int *)p=1;
}

BGBGC_API int thTryLockFastMutex(void *p)
{
	int i;

	if(!p)return(-1);
	while(1)
	{
		if(*(volatile int *)p)return(-1);
		if(!InterlockedExchange((PLONG)p, 1))break;
	}
	return(0);
}

BGBGC_API void thUnlockFastMutex(void *p)
{
	if(!p)return;
	if(InterlockedExchange((PLONG)p, 0)>1)
		Sleep(0);
}

BGBGC_API void thFreeFastMutex(void *p)
{
	if(!p)return;
	gcfree(p);
}
#endif

BGBGC_API void thSleep(int ms)
{
	Sleep(ms);
}


BGBGC_API thContext *thGetContext()
{
	thContext *inf;
	int i;

	inf=(thContext *)TlsGetValue(bgbgc_threadctx_tls);
	if(!inf)
	{
		for(i=0; i<bgbgc_thread_num; i++)
			if(!bgbgc_thread_inf[i])
				break;

		if(i>=bgbgc_thread_num)i=bgbgc_thread_num++;

		inf=malloc(sizeof(thContext));
		memset(inf, 0, sizeof(thContext));
		inf->id=i;

		inf->tls=malloc(1024*sizeof(void *));
		memset(inf->tls, 0, 1024*sizeof(void *));
		inf->sz_tls=1024;

		TlsSetValue(bgbgc_threadctx_tls, (void *)inf);
		TlsSetValue(bgbgc_threadtls_tls, (void *)(inf->tls));

		bgbgc_thread_inf[inf->id]=inf;
	}
	return(inf);
}

BGBGC_API int thGetTlsTlsW32()
{
	bgbgc_thread_init();
	return(bgbgc_threadtls_tls);
}

BGBGC_API void thWaitPtr(void *ptr)
{
	if(!ptr)return;

	while(1)
	{
		WaitForSingleObject(bgbgc_thread_ptrevent, 10);
		InterlockedDecrement((long *)(&bgbgc_thread_ptrwait));
		if(bgbgc_thread_sigptr==ptr)break;
		InterlockedIncrement((long *)(&bgbgc_thread_ptrwait2));
		while(bgbgc_thread_sigptr!=UNDEFINED)SwitchToThread();
		InterlockedIncrement((long *)(&bgbgc_thread_ptrwait));
		InterlockedDecrement((long *)(&bgbgc_thread_ptrwait2));
	}
}

BGBGC_API void thSignalPtr(void *ptr)
{
	if(!ptr)return;
	if(!bgbgc_thread_ptrwait)return;

	while(bgbgc_thread_sigptr)SwitchToThread();
	bgbgc_thread_sigptr=ptr;
	SetEvent(bgbgc_thread_ptrevent);
	while(bgbgc_thread_ptrwait)SwitchToThread();
	ResetEvent(bgbgc_thread_ptrevent);

	bgbgc_thread_sigptr=UNDEFINED;
	while(bgbgc_thread_ptrwait2)SwitchToThread();
	bgbgc_thread_sigptr=NULL;
}

#endif

//#ifdef linux
#if defined(linux) || defined(NATIVECLIENT)

void BGBGC_SuspendThreads()
{
}

void BGBGC_ResumeThreads()
{
}

void *BGBGC_ThreadProcLnx(void *parm) 
{
	thContext *inf;
	int i;

	inf=parm;
//	TlsSetValue(bgbgc_threadctx_tls, (void *)inf);
	pthread_setspecific(bgbgc_threadctx_tls2, (void *)inf);

//	bgbgc_thread_stktop[inf->id]=&parm;
	bgbgc_thread_inf[inf->id]=inf;
	inf->stktop=&parm;

	i=inf->fcn(inf->ptr);

//	bgbgc_thread_hdl[inf->id]=NULL;
//	bgbgc_thread_stktop[inf->id]=NULL;
	bgbgc_thread_inf[inf->id]=NULL;
	return(NULL);
}

BGBGC_API void *thThread(int (*fcn)(void *ptr), void *ptr)
{
	thContext *inf;
	pthread_t *th;
	int i;

	bgbgc_thread_init();

//	for(i=0; i<bgbgc_thread_num; i++)
//		if(!bgbgc_thread_hdl[i])
//			break;

	for(i=0; i<bgbgc_thread_num; i++)
		if(!bgbgc_thread_inf[i])
			break;

	if(i>=bgbgc_thread_num)i=bgbgc_thread_num++;

	inf=malloc(sizeof(thContext));
	memset(inf, 0, sizeof(thContext));

	inf->id=i;
//	bgbgc_thread_hdl[i]=UNDEFINED;
//	bgbgc_thread_stktop[i]=UNDEFINED;

	inf->tls=malloc(1024*sizeof(void *));
	memset(inf->tls, 0, 1024*sizeof(void *));

	inf->hdl=UNDEFINED;
	inf->stktop=UNDEFINED;

	inf->fcn=fcn;
	inf->ptr=ptr;

	th=malloc(sizeof(pthread_t));
	i = pthread_create(th, NULL, BGBGC_ThreadProcLnx, (void *)inf);

//	bgbgc_thread_hdl[i]=th;
	inf->hdl=th;

	return(th);
}

void *thMutexLL()
{
	pthread_mutex_t *mutex;
	mutex=malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(mutex, NULL);
	return((void *)mutex);
}

BGBGC_API void *thMutex()
{
	pthread_mutex_t *mutex;
	mutex=gcatomic(sizeof(pthread_mutex_t));
	pthread_mutex_init(mutex, NULL);
	return((void *)mutex);
}

BGBGC_API void thLockMutex(void *p)
{
	if(!p)return;
	pthread_mutex_lock((pthread_mutex_t *)p);
}

BGBGC_API int thTryLockMutex(void *p)
{
	if(!p)return;
	pthread_mutex_trylock((pthread_mutex_t *)p);
}

BGBGC_API void thUnlockMutex(void *p)
{
	if(!p)return;
	pthread_mutex_unlock((pthread_mutex_t *)p);
}

BGBGC_API void thFreeMutex(void *p)
{
	if(!p)return;
	gcfree(p);
}

//need faster implementation...
BGBGC_API void *thFastMutex()
	{ return(thMutex()); }
BGBGC_API void thLockFastMutex(void *p)
	{ thLockMutex(p); }
BGBGC_API int thTryLockFastMutex(void *p)
	{ thTryLockMutex(p); }
BGBGC_API void thUnlockFastMutex(void *p)
	{ thUnlockMutex(p); }
BGBGC_API void thFreeFastMutex(void *p)
	{ thFreeMutex(p); }


BGBGC_API void thSleep(int ms)
{
	usleep(ms*1000);
}

BGBGC_API thContext *thGetContext()
{
	thContext *inf;
	inf=pthread_getspecific(bgbgc_threadctx_tls2);
	if(!inf)
	{
		inf=malloc(sizeof(thContext));
		memset(inf, 0, sizeof(thContext));

		inf->tls=malloc(1024*sizeof(void *));
		memset(inf->tls, 0, 1024*sizeof(void *));

		pthread_setspecific(bgbgc_threadctx_tls2, (void *)inf);
	}
	return(inf);
}

BGBGC_API void thWaitPtr(void *ptr)
{
	if(!ptr)return;

	while(1)
	{
		pthread_mutex_lock(&bgbgc_threadptr_mutex);
		pthread_cond_wait(&bgbgc_threadptr_cond,
			&bgbgc_threadptr_mutex);
		bgbgc_thread_ptrwait--;
		if(bgbgc_thread_sigptr==ptr)break;

		bgbgc_thread_ptrwait2++;
		pthread_mutex_unlock(&bgbgc_threadptr_mutex);

		while(bgbgc_thread_sigptr!=UNDEFINED)usleep(0);

		pthread_mutex_lock(&bgbgc_threadptr_mutex);
		bgbgc_thread_ptrwait++;
		bgbgc_thread_ptrwait2--;
		pthread_mutex_unlock(&bgbgc_threadptr_mutex);
	}

	pthread_mutex_unlock(&bgbgc_threadptr_mutex);
}

BGBGC_API void thSignalPtr(void *ptr)
{
	if(!ptr)return;
	if(!bgbgc_thread_ptrwait)return;

	while(bgbgc_thread_sigptr)usleep(0);
	bgbgc_thread_sigptr=ptr;
	pthread_cond_broadcast(&bgbgc_threadptr_cond);
	while(bgbgc_thread_ptrwait)usleep(0);
//	ResetEvent(bgbgc_threadptr_cond);

	bgbgc_thread_sigptr=UNDEFINED;
	while(bgbgc_thread_ptrwait2)usleep(0);
	bgbgc_thread_sigptr=NULL;
}

#endif


#if !defined(WIN32) && !defined(linux) && !defined(NATIVECLIENT)

BGBGC_API void *thThread(int (*fcn)(void *ptr), void *ptr)
{
	return(NULL);
}

BGBGC_API void *thMutex()
{
	return(NULL);
}

BGBGC_API void thLockMutex(void *p)
{
}

BGBGC_API int thTryLockMutex(void *p)
{
}

BGBGC_API void thUnlockMutex(void *p)
{
}

// void thLockQInt(int *ri) { }
// void thInitQInt(int *ri) { }
// void thUnlockQInt(int *ri) { }

BGBGC_API void thSleep(int ms)
{
}

BGBGC_API void thWaitPtr(void *ptr)
{
}

BGBGC_API void thSignalPtr(void *ptr)
{
}

#endif
