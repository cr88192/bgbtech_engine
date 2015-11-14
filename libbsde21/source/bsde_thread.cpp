#include "bsde_internal.h"

BSDE_ThreadState *bsde_thread_state[64];

BSDE_ThreadState *BSDE_GetThreadState()
{
	BSDE_ThreadState *tmp;

	tmp=bsde_thread_state[0];
	if(!tmp)
	{
		tmp=(BSDE_ThreadState *)malloc(sizeof(BSDE_ThreadState));
		memset(tmp, 0, sizeof(BSDE_ThreadState));
		bsde_thread_state[0]=tmp;
	}

	return(tmp);
}

