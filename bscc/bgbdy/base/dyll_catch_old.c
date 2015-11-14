/*

Idea:
dyllBeginTry();
stuff...
state=dyllEndTry();

If an exception is thrown between BeginTry and EndTry, EndTry magically
returns with the exception state, otherwise EndTry returns with NULL.

These calls will serve both for structuring, and as an implicit means of
registering the handler machinery.

 */

#define DYLL_HASASM

#include <bgbgc.h>
#include <bgbdy.h>

#ifdef DYLL_HASASM
#include <bgbasm.h>
#endif


dyll_tryframe *dyll_catch_frame=NULL;

dyll_tryframe *dyll_catch_ihash[4096];

int DYLL_HashIP(void *ip)
{
	int i;
	i=(((u32)ip)*4093)>>12;
}

void *DYLL_CatchGetStartIP(void *ip)
{
	dyll_tryframe *cur;
	int i, hi;

	hi=(((u32)ip)*4093)>>12;
	hi&=4095;
	cur=dyll_catch_ihash[hi];
	if(cur && (cur->start_ip==ip))
		return(cur);

	cur=dyll_catch_frame;
	while(cur)
	{
		if(cur->start_ip==ip)
		{
			dyll_catch_ihash[hi]=cur;
			return(cur);
		}
		cur=cur->next;
	}

	cur=malloc(sizeof(dyll_tryframe));
	memset(cur, 0, sizeof(dyll_tryframe));

	cur->start_ip=ip;

	cur->next=dyll_catch_frame;
	dyll_catch_frame=cur;

	dyll_catch_ihash[hi]=cur;
	return(cur);
}

DYLL_TryState *DYLL_AllocTryState()
{
	DYLL_TryState *state;
	state=malloc(sizeof(DYLL_TryState));
	memset(state, 0, sizeof(DYLL_TryState));
	return(state);
}

void *DYLL_BeginTry()
{
	DYLL_TryState *state;
	void *regs[8];
	void *p;
	int i;

	state=DYLL_AllocTryState();
	i=BASM_GetReturnState(regs);
	p=DYLL_CatchGetStartIP(regs[0]);
	return(p);
}

void DYLL_EndTry(void *p)
{
	dyll_tryframe *cur;
	void *regs[8];
	int i;

	cur=p;

	i=BASM_GetReturnState(regs);
	cur->end_ip=regs[0];
}

