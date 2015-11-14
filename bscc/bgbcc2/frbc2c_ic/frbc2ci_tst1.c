#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>

#include <frbc2ci.h>
#include <frbcigl.h>
#include <frbcigl_main.h>

int main_prestart(int argc, char *argv[])
{
}

int main_startup(int argc, char *argv[])
{
}

int main_body()
{
}

int main_shutdown()
{
}

#if 0
int main()
{
	FR2CI_LoaderCtx *ldr;
	FR2CI_Context *ctx;
	s64 accOp;
	double dtf;
	int t0, t1, t2, dt;
	
	ldr=FR2CI_AllocLoader();
	ctx=FR2CI_AllocContext(ldr);
	FR2CI_SetupNopBench(ctx);
	
	accOp=0;
	t0=clock(); t1=t0;
	while(t1<(t0+30*CLOCKS_PER_SEC))
	{
		ctx->stat_n_op=0;
		ctx->stat_n_tr=0;
		FR2CI_RunContext(ctx);
		accOp+=ctx->stat_n_op;

		t1=clock();
		dt=t1-t0;
		dtf=dt/((double)CLOCKS_PER_SEC);
		
		printf("%fs %I64d %f Mips\r",
			dtf, accOp, accOp/(dtf*1000000));
	}
}
#endif
