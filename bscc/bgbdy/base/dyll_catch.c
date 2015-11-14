/*

Idea:
  ctx=dyllBeginUnwind();
stuff...
  void dyllEndUnwind(ctx);

Creates an unwind state, where unwinding will transfer control back to
BeginUnwind. EndUnwind will remove the handler.

  void dyllUnwindNext(ctx);

Used during an exception to pass the current exception onto the next handler.

  dyt dyllUnwindObj(dyt ctx);

Get the object associated with the current unwind state.

  void dyllDoUnwind(dyt obj);
Initiate an unwind with the given object.


  void dyllThrowObj(dyt obj);
  void dyllThrowName(char *str);
  void dyllThrowClass(char *str);
  void dyllThrow(char *str);

Throw an exception.
ThrowObj takes an instance of an exception class.
ThrowName takes a raw name and throws it (symbolic only).
ThrowClass assumes str is a classname and tries to create an instance of said
class and throws it.
Throw is more generic, and will try to figure out what to do with the string.

  int dyllCatchP(dyt ctx, char *str)

Returns non-zero if a given exception was thrown.
If str is NULL, this simply tells if an exception was thrown.
If a name was thrown, str is matched exactly.
If an object was thrown and str refers to a superclass of the thrown object,
it matches.

 */

#define DYLL_HASASM

#include <bgbgc.h>
#include <bgbdy.h>

#ifdef DYLL_HASASM
#include <bgbasm.h>
#endif

#ifdef MSVC
#include <windows.h>
#endif


BGBDY_API dyllException dyllBeginUnwind()
{
	DYLL_TryFrame *frame;
	void *p;

	frame=gcalloc(sizeof(DYLL_TryFrame));
	frame->jmp=gcalloc(sizeof(BGBASM_JmpBuf));

	p=BASM_BeginUnwind(frame->jmp);
	frame->ptr=p;

	return(frame);
}

BGBDY_API void dyllEndUnwind(dyllException ctx)
{
	DYLL_TryFrame *frame;

	frame=ctx;
	BASM_EndUnwind(frame->jmp);
	gcfree(frame);
}

BGBDY_API void dyllUnwindNext(dyllException ctx)
{
	DYLL_TryFrame *frame;
	void *p;

	frame=ctx;
	p=frame->ptr;
	BASM_EndUnwind(frame->jmp);
	gcfree(frame->jmp);
	gcfree(frame);

	BASM_DoUnwind(p);
}

BGBDY_API dyt dyllUnwindObj(dyllException ctx)
{
	DYLL_TryFrame *frame;

	frame=ctx;
	return((dyt)frame->ptr);
}

BGBDY_API void dyllDoUnwind(dyt obj)
{
	BASM_DoUnwind((void *)obj);
}


BGBDY_API void dyllThrowObj(dyt obj)
	{ dyllDoUnwind(obj); }
BGBDY_API void dyllThrowName(char *str)
	{ dyllDoUnwind(dysymbol(str)); }

BGBDY_API void dyllThrowClass(char *str)
{
	dycClass cls;
	dycObject obj;

	cls=dycGetClass(str);
	if(!cls)cls=dycGetClass(gcrsprint("java/lang/%s", str));
	if(!cls)cls=dycGetClass(gcrsprint("java/io/%s", str));
	if(cls)
	{
		obj=dycAlloc(cls);
		dyllDoUnwind((dyt)obj);
	}

	dyllDoUnwind(dysymbol(str));
}

BGBDY_API void dyllThrow(char *str)
{
	dyllThrowClass(str);
}

BGBDY_API int dyllCatchP(dyllException ctx, char *str)
{
	dycClass cls;
	dyt p;

	p=dyllUnwindObj(ctx);
	if(!p)return(0);	//no exception thrown
	if(!str)return(1);	//match any exception

	if(dysymbolp(p))
	{
		if(p==dysym(str))
			return(1);
		return(0);
	}

	if(dycObjectP(p))
	{
		cls=dycGetClass(str);
		if(!cls)return(0);
		if(dycInstanceP((dycObject)p, cls))
			return(1);
		return(0);
	}

	return(0);
}


//alternate mechanism/interface

#ifdef MSVC
char *dyll_seh_idname(u32 id) //AH:ignore
{
	char *s;

//	s="SystemException";

	switch(id)
	{
	case EXCEPTION_ACCESS_VIOLATION:
		s="bgb/vm/AccessViolationException"; break;
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
		s="bgb/vm/ArrayBoundsException"; break;
	case EXCEPTION_BREAKPOINT:
		s="bgb/vm/BreakpointException"; break;
	case EXCEPTION_DATATYPE_MISALIGNMENT:
		s="bgb/vm/DataMisalignmentException"; break;

	case EXCEPTION_ILLEGAL_INSTRUCTION:
		s="bgb/vm/InvalidOpcodeException"; break;
	case EXCEPTION_STACK_OVERFLOW:
		s="bgb/vm/StackOverflowException"; break;

	case EXCEPTION_FLT_DIVIDE_BY_ZERO:
		s="bgb/vm/DivideByZeroException"; break;
	case EXCEPTION_INT_DIVIDE_BY_ZERO:
		s="bgb/vm/DivideByZeroException"; break;
	default: s=NULL; break;
	}
	return(s);
}

int dyll_eh_filter(u32 code,
	LPEXCEPTION_POINTERS ep, int fl,
	dyllException tmp) //AH:ignore
{
	LPEXCEPTION_POINTERS ep2;
//	dyllException tmp;
	void *obj;
	char *s;

	if (code == DYLL_EH_CODE)
	{
		obj=(BGBGC_Exception *)
			ep->ExceptionRecord->ExceptionInformation[0];
		tmp->ptr=obj;
		return(EXCEPTION_EXECUTE_HANDLER);
	}

#if 1
	if(fl&GCEH_TRY_SYSTEM)
	{
//		ep2=bgbgc_seh_dupptrs(ep);
		s=dyll_seh_idname(code);
		if(s)
		{
			obj=dycNewClass(s);
			tmp->ptr=obj;
			return(EXCEPTION_EXECUTE_HANDLER);
		}
	}
#endif

	return(EXCEPTION_CONTINUE_SEARCH);
}
#endif

BGBDY_API void dyllTryCatchFl(
	void (*fcnTry)(void *data),
	int (*fcnCatch)(dyllException ex, void *data),
	void *data, int fl)
{
	DYLL_TryFrame otmp;
	dyllException tmp;
	int i;
	
	tmp=&otmp;
	memset(tmp, 0, sizeof(DYLL_TryFrame));

#ifdef MSVC
	__try {
		fcnTry(data);
		return;
	}
	__except(dyll_eh_filter(
		GetExceptionCode(),
		GetExceptionInformation(), fl, tmp))
	{
		if(!fcnCatch)
			return;
		i=fcnCatch(tmp, data);
		if(i==DYLL_EH_RETHROW)
			dyllThrowObj(tmp->ptr);
	}
#endif
}

BGBDY_API void dyllTryCatch(
	void (*fcnTry)(void *data),
	int (*fcnCatch)(dyllException ex, void *data),
	void *data)
{
	dyllTryCatchFl(fcnTry, fcnCatch, data, 0);
}

BGBDY_API void dyllTry(
	void (*fcnTry)(void *data),
	void *data)
{
	dyllTryCatchFl(fcnTry, NULL, data, GCEH_TRY_SYSTEM);
}
