#include <bgbfrir2.h>

void BGBFR2_Step_UnaryMovI(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=ctx->regs[op->s].i; }
void BGBFR2_Step_UnaryMovL(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].l=ctx->regs[op->s].l; }
void BGBFR2_Step_UnaryMovF(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].f=ctx->regs[op->s].f; }
void BGBFR2_Step_UnaryMovD(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].d=ctx->regs[op->s].d; }
void BGBFR2_Step_UnaryMovP(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].p=ctx->regs[op->s].p; }
void BGBFR2_Step_UnaryMovA(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].a=ctx->regs[op->s].a; }
void BGBFR2_Step_UnaryNegI(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=-ctx->regs[op->s].i; }
void BGBFR2_Step_UnaryNegL(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].l=-ctx->regs[op->s].l; }
void BGBFR2_Step_UnaryNegF(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].f=-ctx->regs[op->s].f; }
void BGBFR2_Step_UnaryNegD(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].d=-ctx->regs[op->s].d; }
void BGBFR2_Step_UnaryNotI(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=~ctx->regs[op->s].i; }
void BGBFR2_Step_UnaryNotL(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].l=~ctx->regs[op->s].l; }
void BGBFR2_Step_UnaryNotF(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].f=~(s64)ctx->regs[op->s].f; }
void BGBFR2_Step_UnaryNotD(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].d=~(s64)ctx->regs[op->s].d; }
void BGBFR2_Step_UnaryLNotI(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=!ctx->regs[op->s].i; }
void BGBFR2_Step_UnaryLNotL(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].l=!ctx->regs[op->s].l; }
void BGBFR2_Step_UnaryLNotF(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].f=!(int)ctx->regs[op->s].f; }
void BGBFR2_Step_UnaryLNotD(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].d=!(int)ctx->regs[op->s].d; }
void BGBFR2_Step_UnaryIncI(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=ctx->regs[op->s].i+1; }
void BGBFR2_Step_UnaryIncL(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].l=ctx->regs[op->s].l+1; }
void BGBFR2_Step_UnaryIncF(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].f=ctx->regs[op->s].f+1; }
void BGBFR2_Step_UnaryIncD(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].d=ctx->regs[op->s].d+1; }
void BGBFR2_Step_UnaryDecI(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=ctx->regs[op->s].i-1; }
void BGBFR2_Step_UnaryDecL(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].l=ctx->regs[op->s].l-1; }
void BGBFR2_Step_UnaryDecF(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].f=ctx->regs[op->s].f-1; }
void BGBFR2_Step_UnaryDecD(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].d=ctx->regs[op->s].d-1; }

void *BGBFR2_PointerAddr(FR2_PointerInfo *ptr)
	{ return(ptr->p); }
void *BGBFR2_PointerAddrIndex(FR2_PointerInfo *ptr, int idx)
{
	return(((byte *)ptr->p)+(idx*step));
}

void BGBFR2_Step_UnaryDeferI(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=*(s32 *)BGBFR2_PointerAddr(ctx->regs[op->s].p); }
void BGBFR2_Step_UnaryDeferL(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].l=*(s64 *)BGBFR2_PointerAddr(ctx->regs[op->s].p); }
void BGBFR2_Step_UnaryDeferF(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].f=*(f32 *)BGBFR2_PointerAddr(ctx->regs[op->s].p); }
void BGBFR2_Step_UnaryDeferD(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].d=*(f64 *)BGBFR2_PointerAddr(ctx->regs[op->s].p); }

void BGBFR2_Step_UnarySetDeferI(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ *(s32 *)BGBFR2_PointerAddr(ctx->regs[op->d].p)=ctx->regs[op->s].i; }
void BGBFR2_Step_UnarySetDeferL(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ *(s64 *)BGBFR2_PointerAddr(ctx->regs[op->d].p)=ctx->regs[op->s].l; }
void BGBFR2_Step_UnarySetDeferF(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ *(f32 *)BGBFR2_PointerAddr(ctx->regs[op->d].p)=ctx->regs[op->s].f; }
void BGBFR2_Step_UnarySetDeferD(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ *(f64 *)BGBFR2_PointerAddr(ctx->regs[op->d].p)=ctx->regs[op->s].d; }

void BGBFR2_Step_UnaryMovIC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=op->c; }
void BGBFR2_Step_UnaryMovLC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].l=op->c; }
void BGBFR2_Step_UnaryNegIC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=-op->c; }
void BGBFR2_Step_UnaryNegLC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].l=-op->c; }
void BGBFR2_Step_UnaryNotIC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=~op->c; }
void BGBFR2_Step_UnaryNotLC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].l=~op->c; }
void BGBFR2_Step_UnaryLNotIC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=!op->c; }
void BGBFR2_Step_UnaryLNotLC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].l=!op->c; }
void BGBFR2_Step_UnaryIncIC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=op->c+1; }
void BGBFR2_Step_UnaryIncLC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].l=op->c+1; }
void BGBFR2_Step_UnaryDecIC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=op->c-1; }
void BGBFR2_Step_UnaryDecLC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].l=op->c-1; }

void BGBFR2_Step_SLoadIC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=ctx->object->svar[op->c]->val->i; }
void BGBFR2_Step_SLoadLC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].l=ctx->object->svar[op->c]->val->l; }
void BGBFR2_Step_SLoadFC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].f=ctx->object->svar[op->c]->val->f; }
void BGBFR2_Step_SLoadDC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].d=ctx->object->svar[op->c]->val->d; }

void BGBFR2_Step_SStoreIC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->object->svar[op->c]->val->i=ctx->regs[op->s].i; }
void BGBFR2_Step_SStoreLC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->object->svar[op->c]->val->l=ctx->regs[op->s].l; }
void BGBFR2_Step_SStoreFC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->object->svar[op->c]->val->f=ctx->regs[op->s].f; }
void BGBFR2_Step_SStoreDC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->object->svar[op->c]->val->d=ctx->regs[op->s].d; }

void BGBFR2_Step_LoadConstIC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=ctx->object->ctab_val[op->c]->i; }
void BGBFR2_Step_LoadConstLC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].l=ctx->object->ctab_val[op->c]->l; }
void BGBFR2_Step_LoadConstFC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].f=ctx->object->ctab_val[op->c]->f; }
void BGBFR2_Step_LoadConstDC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].d=ctx->object->ctab_val[op->c]->d; }
