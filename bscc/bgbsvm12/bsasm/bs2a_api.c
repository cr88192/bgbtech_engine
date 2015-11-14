#include <bs2asm.h>

void BS2ASM_Error(BS2ASM_Context *ctx, char *str, ...)
{
}

BS2ASM_Context *BS2ASM_AllocContext()
{
	BS2ASM_Context *ctx;
	ctx=bs2asm_tmalloc("_bs2asm_context_t", sizeof(BS2ASM_Context));
	return(ctx);
}

BGBSVM_API int BS2ASM_AssembleBuffer(char *str, byte *buf, int lim)
{
	BS2ASM_Context *ctx;
	int sz;

	ctx=BS2ASM_AllocContext();
	BS2ASM_EntParse_Buffer(ctx, str);
//	BS2ASM_DumpEntityTree(ctx->top);
//	sz=BS2ASM_FlattenBuffer(ctx, buf, lim);
//	BS2ASM_CleanupAll();
	return(sz);
}
