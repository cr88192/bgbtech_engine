#include <bgbgc.h>

#ifdef MSVC
#include <windows.h>
#endif

BGBGC_ExceptionType *bgbgc_eh_root;

BGBGC_Exception *bgbgc_eh_exfree;

int BGBGC_EH_Init()
{
	static int init=0;
	BGBGC_Exception *tmp;
	int i;

	if(init)return(1);
	init=1;

	gcehRegisterException("SystemException", "Exception");

	gcehRegisterException("AccessViolationException", "SystemException");
	gcehRegisterException("ArrayBoundsException", "SystemException");
	gcehRegisterException("BreakpointException", "SystemException");
	gcehRegisterException("DataMisalignmentException", "SystemException");

	gcehRegisterException("InvalidOpcodeException", "SystemException");
	gcehRegisterException("StackOverflowException", "SystemException");

	gcehRegisterException("DivideByZeroException", "SystemException");

	for(i=0; i<256; i++)
	{
		tmp=gctalloc("_bgbgc_exception_t",
			sizeof(BGBGC_Exception));
		tmp->chain=bgbgc_eh_exfree;
		bgbgc_eh_exfree=tmp;
	}

	return(0);
}

BGBGC_ExceptionType *BGBGC_EH_LookupType(char *name)
{
	BGBGC_ExceptionType *cur;

	cur=bgbgc_eh_root;
	while(cur)
	{
		if(!strcmp(cur->name, name))
			return(cur);
		cur=cur->next;
	}
	return(NULL);
}

BGBGC_ExceptionType *BGBGC_EH_GetType(char *name)
{
	BGBGC_ExceptionType *tmp;

	if(!name)return(NULL);
	if(!*name)return(NULL);

	tmp=BGBGC_EH_LookupType(name);
	if(tmp)return(tmp);

	tmp=malloc(sizeof(BGBGC_ExceptionType));
	memset(tmp, 0, sizeof(BGBGC_ExceptionType));
	tmp->name=strdup(name);

	tmp->next=bgbgc_eh_root;
	bgbgc_eh_root=tmp;

	return(tmp);
}

int BGBGC_EH_TypeIsP(BGBGC_ExceptionType *src, BGBGC_ExceptionType *dst)
{
	BGBGC_ExceptionType *cur;

	cur=src;
	while(cur)
	{
		if(cur==dst)return(1);
		cur=cur->parent;
	}
	return(0);
}

BGBGC_API int gcehRegisterException(char *name, char *parent)
{
	BGBGC_ExceptionType *tmp, *up;

	BGBGC_EH_Init();

	tmp=BGBGC_EH_GetType(name);
	up=BGBGC_EH_GetType(parent);
	tmp->parent=up;
	return(0);
}

#ifdef MSVC
//stupid SEH crap...

LPEXCEPTION_POINTERS bgbgc_seh_dupptrs(LPEXCEPTION_POINTERS ep) //AH:ignore
{
	LPEXCEPTION_POINTERS ep2;
	LPEXCEPTION_RECORD er2;
	LPCONTEXT ctx2;

	ep2=malloc(sizeof(EXCEPTION_POINTERS));
	er2=malloc(sizeof(EXCEPTION_RECORD));
	ctx2=malloc(sizeof(CONTEXT));

	memcpy(ep2, ep, sizeof(EXCEPTION_POINTERS));
	memcpy(er2, ep->ExceptionRecord, sizeof(EXCEPTION_RECORD));
	memcpy(ctx2, ep->ContextRecord, sizeof(CONTEXT));
	ep2->ExceptionRecord=er2;
	ep2->ContextRecord=ctx2;

	return(ep2);
}

void bgbgc_seh_freeptrs(LPEXCEPTION_POINTERS ep) //AH:ignore
{
	free(ep->ExceptionRecord);
	free(ep->ContextRecord);
	free(ep);
}

char *bgbgc_seh_idname(u32 id) //AH:ignore
{
	char *s;

	s="SystemException";
	switch(id)
	{
	case EXCEPTION_ACCESS_VIOLATION:
		s="AccessViolationException"; break;
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
		s="ArrayBoundsException"; break;
	case EXCEPTION_BREAKPOINT:
		s="BreakpointException"; break;
	case EXCEPTION_DATATYPE_MISALIGNMENT:
		s="DataMisalignmentException"; break;

	case EXCEPTION_ILLEGAL_INSTRUCTION:
		s="InvalidOpcodeException"; break;
	case EXCEPTION_STACK_OVERFLOW:
		s="StackOverflowException"; break;

	case EXCEPTION_FLT_DIVIDE_BY_ZERO:
		s="DivideByZeroException"; break;
	case EXCEPTION_INT_DIVIDE_BY_ZERO:
		s="DivideByZeroException"; break;
	default: s="SystemException"; break;
	}
	return(s);
}

int bgbgc_eh_filter(u32 code,
	LPEXCEPTION_POINTERS ep, int fl,
	BGBGC_Exception **rtmp) //AH:ignore
{
	LPEXCEPTION_POINTERS ep2;
	BGBGC_Exception *tmp;
	char *s;

	if (code == BGBGC_GCEH_EXCEPTION)
	{
		tmp=(BGBGC_Exception *)
			ep->ExceptionRecord->ExceptionInformation[0];
		*rtmp=tmp;
		return(EXCEPTION_EXECUTE_HANDLER);
	}

	if(fl&GCEH_TRY_SYSTEM)
	{
		ep2=bgbgc_seh_dupptrs(ep);
		s=bgbgc_seh_idname(code);

		tmp=gcehNew(s, NULL, ep2);
		tmp->id=code;

		*rtmp=tmp;
		return(EXCEPTION_EXECUTE_HANDLER);
	}

	return(EXCEPTION_CONTINUE_SEARCH);
}
#endif

BGBGC_API gcehException gcehTryFl(
	void (*func)(void *data), void *data, int fl)
{
	BGBGC_Exception *tmp;

	BGBGC_EH_Init();

#ifdef MSVC
	__try {
		func(data);
		return(NULL);
	}
	__except(bgbgc_eh_filter(
		GetExceptionCode(),
		GetExceptionInformation(), fl, &tmp))
			{ return(tmp); }
#else
	func(data);
	return(NULL);
#endif
}

BGBGC_API gcehException gcehTry(void (*func)(void *data), void *data)
	{ return(gcehTryFl(func, data, GCEH_TRY_DEFAULT)); }
BGBGC_API gcehException gcehTrySys(void (*func)(void *data), void *data)
	{ return(gcehTryFl(func, data, GCEH_TRY_SYSTEM)); }

BGBGC_API void gcehThrow(gcehException ex)
{
#ifdef MSVC
	LPEXCEPTION_POINTERS ep;
#endif

	BGBGC_EH_Init();

#ifdef MSVC
	if(gcehCheckIsP(ex, "SystemException"))
	{
		ep=ex->data;
		RaiseException(
			ep->ExceptionRecord->ExceptionCode,
			ep->ExceptionRecord->ExceptionFlags,
			ep->ExceptionRecord->NumberParameters,
			ep->ExceptionRecord->ExceptionInformation);
		return;
	}

	RaiseException(BGBGC_GCEH_EXCEPTION, 0, 1, (ULONG_PTR *)(&ex));
#else
	*(int *)-1=-1;
#endif
}

BGBGC_API gcehException gcehNew(char *name, char *msg, void *data)
{
	BGBGC_ExceptionType *ty;
	BGBGC_Exception *tmp;

	BGBGC_EH_Init();


	ty=BGBGC_EH_GetType(name);

	if(bgbgc_eh_exfree)
	{
		tmp=bgbgc_eh_exfree;
		bgbgc_eh_exfree=tmp->chain;
	}else
	{
		tmp=gctalloc("_bgbgc_exception_t",
			sizeof(BGBGC_Exception));
//		memset(tmp, 0, sizeof(BGBGC_Exception));
	}

	tmp->type=ty;
	tmp->data=data;
	if(msg) tmp->message=strdup(msg);

	return(tmp);
}

BGBGC_API void gcehThrowNewMsgData(char *name, char *msg, void *data)
{
	BGBGC_Exception *tmp;
	tmp=gcehNew(name, msg, data);
	gcehThrow(tmp);
}

BGBGC_API void gcehThrowNew(char *name)
	{ gcehThrowNewMsgData(name, NULL, NULL); }
BGBGC_API void gcehThrowNewMsg(char *name, char *msg)
	{ gcehThrowNewMsgData(name, msg, NULL); }
BGBGC_API void gcehThrowNewData(char *name, void *data)
	{ gcehThrowNewMsgData(name, NULL, data); }

BGBGC_API int gcehCheckIsP(gcehException ex, char *name)
{
	BGBGC_ExceptionType *ty;
	ty=BGBGC_EH_GetType(name);
	return(BGBGC_EH_TypeIsP(ex->type, ty));
}

BGBGC_API void gcehFree(gcehException ex)
{
#ifdef MSVC
	if(gcehCheckIsP(ex, "SystemException"))
		bgbgc_seh_freeptrs(ex->data);
#endif

	ex->chain=bgbgc_eh_exfree;
	bgbgc_eh_exfree=ex;
//	gcfree(ex);
}

BGBGC_API char *gcehGetName(gcehException ex)
	{ return(ex->type->name); }
BGBGC_API char *gcehGetMessage(gcehException ex)
	{ return(ex->message); }
BGBGC_API void *gcehGetData(gcehException ex)
	{ return(ex->data); }
