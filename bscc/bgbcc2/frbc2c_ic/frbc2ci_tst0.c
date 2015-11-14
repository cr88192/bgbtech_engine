#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>

#include <frbc2ci.h>

byte *frbc_LoadImageFile_cb(char *name, int *rsz, int *rfl)
{
	void *buf;
	FILE *fd;
	int sz;

//	blnk_initvfs();
	fd=fopen(name, "rb");
	if(!fd)return(NULL);

	fseek(fd, 0, 2);
	sz=ftell(fd);
	fseek(fd, 0, 0);

	buf=malloc(sz+16);
	memset(buf, 0, sz+16);
	fread(buf, 1, sz, fd);

	fclose(fd);

	if(rsz)*rsz=sz;
	if(rfl)*rfl=0;
	return(buf);
}

byte *frbc_ReleaseImageFileBuffer_cb(byte *buf, int fl)
{
	free(buf);
}


int main(int argc, char *argv[])
{
	FR2CI_Value arga[4];
	char *arga2[4];
	FR2CI_LoaderCtx *ldr;
	FR2CI_Context *ctx;
	FR2CI_Function *func;

	ldr=FR2CI_AllocLoader();
	ldr->LoadImageFile=frbc_LoadImageFile_cb;
	ldr->ReleaseImageFileBuffer=frbc_ReleaseImageFileBuffer_cb;
	
	FR2CI_LoadProgramName(ldr, argv[1]);

	arga2[0]=argv[1];
	arga[0].i=1;
	arga[1].p=arga2;

	ctx=FR2CI_AllocContext(ldr);
	func=FR2CI_LookupFunctionNameGeneric(ldr, "main", "(iPPc)i");
	FR2CI_SetupRawCall(ctx, func, arga, 2);
	FR2CI_RunContext(ctx);
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
