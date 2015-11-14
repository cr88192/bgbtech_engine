#include <bgbfrir2.h>

void BGBFR2_Step_BinaryAddI(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=ctx->regs[op->s].i+ctx->regs[op->t].i; }
void BGBFR2_Step_BinaryAddL(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].l=ctx->regs[op->s].l+ctx->regs[op->t].l; }
void BGBFR2_Step_BinaryAddF(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].f=ctx->regs[op->s].f+ctx->regs[op->t].f; }
void BGBFR2_Step_BinaryAddD(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].d=ctx->regs[op->s].d+ctx->regs[op->t].d; }

void BGBFR2_Step_BinarySubI(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=ctx->regs[op->s].i-ctx->regs[op->t].i; }
void BGBFR2_Step_BinarySubL(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].l=ctx->regs[op->s].l-ctx->regs[op->t].l; }
void BGBFR2_Step_BinarySubF(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].f=ctx->regs[op->s].f-ctx->regs[op->t].f; }
void BGBFR2_Step_BinarySubD(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].d=ctx->regs[op->s].d-ctx->regs[op->t].d; }

void BGBFR2_Step_BinaryMulI(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=ctx->regs[op->s].i*ctx->regs[op->t].i; }
void BGBFR2_Step_BinaryMulL(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].l=ctx->regs[op->s].l*ctx->regs[op->t].l; }
void BGBFR2_Step_BinaryMulF(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].f=ctx->regs[op->s].f*ctx->regs[op->t].f; }
void BGBFR2_Step_BinaryMulD(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].d=ctx->regs[op->s].d*ctx->regs[op->t].d; }

void BGBFR2_Step_BinaryDivI(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=ctx->regs[op->s].i/ctx->regs[op->t].i; }
void BGBFR2_Step_BinaryDivL(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].l=ctx->regs[op->s].l/ctx->regs[op->t].l; }
void BGBFR2_Step_BinaryDivF(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].f=ctx->regs[op->s].f/ctx->regs[op->t].f; }
void BGBFR2_Step_BinaryDivD(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].d=ctx->regs[op->s].d/ctx->regs[op->t].d; }

void BGBFR2_Step_BinaryModI(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=ctx->regs[op->s].i%ctx->regs[op->t].i; }
void BGBFR2_Step_BinaryModL(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].l=ctx->regs[op->s].l%ctx->regs[op->t].l; }
void BGBFR2_Step_BinaryModF(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].f=ctx->regs[op->s].f%ctx->regs[op->t].f; }
void BGBFR2_Step_BinaryModD(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].d=ctx->regs[op->s].d%ctx->regs[op->t].d; }

void BGBFR2_Step_BinaryAndI(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=ctx->regs[op->s].i&ctx->regs[op->t].i; }
void BGBFR2_Step_BinaryAndL(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].l=ctx->regs[op->s].l&ctx->regs[op->t].l; }
void BGBFR2_Step_BinaryAndF(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].f=(s64)ctx->regs[op->s].f&(s64)ctx->regs[op->t].f; }
void BGBFR2_Step_BinaryAndD(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].d=(s64)ctx->regs[op->s].d&(s64)ctx->regs[op->t].d; }

void BGBFR2_Step_BinaryOrI(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=ctx->regs[op->s].i|ctx->regs[op->t].i; }
void BGBFR2_Step_BinaryOrL(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].l=ctx->regs[op->s].l|ctx->regs[op->t].l; }
void BGBFR2_Step_BinaryOrF(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].f=(s64)ctx->regs[op->s].f|(s64)ctx->regs[op->t].f; }
void BGBFR2_Step_BinaryOrD(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].d=(s64)ctx->regs[op->s].d|(s64)ctx->regs[op->t].d; }

void BGBFR2_Step_BinaryXorI(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=ctx->regs[op->s].i^ctx->regs[op->t].i; }
void BGBFR2_Step_BinaryXorL(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].l=ctx->regs[op->s].l^ctx->regs[op->t].l; }
void BGBFR2_Step_BinaryXorF(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].f=(s64)ctx->regs[op->s].f^(s64)ctx->regs[op->t].f; }
void BGBFR2_Step_BinaryXorD(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].d=(s64)ctx->regs[op->s].d^(s64)ctx->regs[op->t].d; }

void BGBFR2_Step_BinaryShlI(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=ctx->regs[op->s].i<<ctx->regs[op->t].i; }
void BGBFR2_Step_BinaryShlL(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].l=ctx->regs[op->s].l<<ctx->regs[op->t].l; }
void BGBFR2_Step_BinaryShlF(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].f=(s64)ctx->regs[op->s].f<<(int)ctx->regs[op->t].f; }
void BGBFR2_Step_BinaryShlD(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].d=(s64)ctx->regs[op->s].d<<(int)ctx->regs[op->t].d; }

void BGBFR2_Step_BinaryShrI(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=ctx->regs[op->s].i>>ctx->regs[op->t].i; }
void BGBFR2_Step_BinaryShrL(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].l=ctx->regs[op->s].l>>ctx->regs[op->t].l; }
void BGBFR2_Step_BinaryShrF(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].f=(s64)ctx->regs[op->s].f>>(int)ctx->regs[op->t].f; }
void BGBFR2_Step_BinaryShrD(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].d=(s64)ctx->regs[op->s].d>>(int)ctx->regs[op->t].d; }

void BGBFR2_Step_BinaryShrrI(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=(u32)ctx->regs[op->s].i>>ctx->regs[op->t].i; }
void BGBFR2_Step_BinaryShrrL(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].l=(u64)ctx->regs[op->s].l>>ctx->regs[op->t].l; }
void BGBFR2_Step_BinaryShrrF(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].f=(u64)ctx->regs[op->s].f>>(int)ctx->regs[op->t].f; }
void BGBFR2_Step_BinaryShrrD(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].d=(u64)ctx->regs[op->s].d>>(int)ctx->regs[op->t].d; }

void BGBFR2_Step_BinaryCmplI(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
{	FR2_Value *rs, *rt;
	rs=ctx->regs[op->s]; rt=ctx->regs[op->t];
	ctx->regs[op->d].i=(rs.i==rt.i)?0:((rs.i>rt.i)?1:(-1)); }
void BGBFR2_Step_BinaryCmplL(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
{	FR2_Value *rs, *rt;
	rs=ctx->regs[op->s]; rt=ctx->regs[op->t];
	ctx->regs[op->d].i=(rs.l==rt.l)?0:((rs.l>rt.l)?1:(-1)); }
void BGBFR2_Step_BinaryCmplF(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
{	FR2_Value *rs, *rt;
	rs=ctx->regs[op->s]; rt=ctx->regs[op->t];
	ctx->regs[op->d].i=(rs.f==rt.f)?0:((rs.f>rt.f)?1:(-1)); }
void BGBFR2_Step_BinaryCmplD(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
{	FR2_Value *rs, *rt;
	rs=ctx->regs[op->s]; rt=ctx->regs[op->t];
	ctx->regs[op->d].i=(rs.d==rt.d)?0:((rs.d>rt.d)?1:(-1)); }

void BGBFR2_Step_BinaryCmpgI(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
{	FR2_Value *rs, *rt;
	rs=ctx->regs[op->s]; rt=ctx->regs[op->t];
	ctx->regs[op->d].i=(rs.i==rt.i)?0:((rs.i<rt.i)?(-1):1); }
void BGBFR2_Step_BinaryCmpgL(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
{	FR2_Value *rs, *rt;
	rs=ctx->regs[op->s]; rt=ctx->regs[op->t];
	ctx->regs[op->d].i=(rs.l==rt.l)?0:((rs.l<rt.l)?(-1):1); }
void BGBFR2_Step_BinaryCmpgF(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
{	FR2_Value *rs, *rt;
	rs=ctx->regs[op->s]; rt=ctx->regs[op->t];
	ctx->regs[op->d].i=(rs.f==rt.f)?0:((rs.f<rt.f)?(-1):1); }
void BGBFR2_Step_BinaryCmpgD(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
{	FR2_Value *rs, *rt;
	rs=ctx->regs[op->s]; rt=ctx->regs[op->t];
	ctx->regs[op->d].i=(rs.d==rt.d)?0:((rs.d<rt.d)?(-1):1); }

void BGBFR2_Step_BinaryLAndI(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=(s32)ctx->regs[op->s].i&&ctx->regs[op->t].i; }
void BGBFR2_Step_BinaryLAndL(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].l=(s64)ctx->regs[op->s].l&&ctx->regs[op->t].l; }
void BGBFR2_Step_BinaryLAndF(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].f=(s64)ctx->regs[op->s].f&&(s64)ctx->regs[op->t].f; }
void BGBFR2_Step_BinaryLAndD(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].d=(s64)ctx->regs[op->s].d&&(s64)ctx->regs[op->t].d; }

void BGBFR2_Step_BinaryLOrI(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=(s32)ctx->regs[op->s].i||ctx->regs[op->t].i; }
void BGBFR2_Step_BinaryLOrL(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].l=(s64)ctx->regs[op->s].l||ctx->regs[op->t].l; }
void BGBFR2_Step_BinaryLOrF(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].f=(s64)ctx->regs[op->s].f||(s64)ctx->regs[op->t].f; }
void BGBFR2_Step_BinaryLOrD(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].d=(s64)ctx->regs[op->s].d||(s64)ctx->regs[op->t].d; }

void BGBFR2_Step_BinaryCmpEqI(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=(ctx->regs[op->s].i==ctx->regs[op->t].i); }
void BGBFR2_Step_BinaryCmpEqL(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=(ctx->regs[op->s].l==ctx->regs[op->t].l); }
void BGBFR2_Step_BinaryCmpEqF(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=(ctx->regs[op->s].f==ctx->regs[op->t].f); }
void BGBFR2_Step_BinaryCmpEqD(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=(ctx->regs[op->s].d==ctx->regs[op->t].d); }

void BGBFR2_Step_BinaryCmpNeI(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=(ctx->regs[op->s].i!=ctx->regs[op->t].i); }
void BGBFR2_Step_BinaryCmpNeL(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=(ctx->regs[op->s].l!=ctx->regs[op->t].l); }
void BGBFR2_Step_BinaryCmpNeF(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=(ctx->regs[op->s].f!=ctx->regs[op->t].f); }
void BGBFR2_Step_BinaryCmpNeD(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=(ctx->regs[op->s].d!=ctx->regs[op->t].d); }

void BGBFR2_Step_BinaryCmpLtI(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=(ctx->regs[op->s].i<ctx->regs[op->t].i); }
void BGBFR2_Step_BinaryCmpLtL(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=(ctx->regs[op->s].l<ctx->regs[op->t].l); }
void BGBFR2_Step_BinaryCmpLtF(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=(ctx->regs[op->s].f<ctx->regs[op->t].f); }
void BGBFR2_Step_BinaryCmpLtD(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=(ctx->regs[op->s].d<ctx->regs[op->t].d); }

void BGBFR2_Step_BinaryCmpGtI(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=(ctx->regs[op->s].i>ctx->regs[op->t].i); }
void BGBFR2_Step_BinaryCmpGtL(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=(ctx->regs[op->s].l>ctx->regs[op->t].l); }
void BGBFR2_Step_BinaryCmpGtF(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=(ctx->regs[op->s].f>ctx->regs[op->t].f); }
void BGBFR2_Step_BinaryCmpGtD(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=(ctx->regs[op->s].d>ctx->regs[op->t].d); }

void BGBFR2_Step_BinaryCmpLeI(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=(ctx->regs[op->s].i<=ctx->regs[op->t].i); }
void BGBFR2_Step_BinaryCmpLeL(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=(ctx->regs[op->s].l<=ctx->regs[op->t].l); }
void BGBFR2_Step_BinaryCmpLeF(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=(ctx->regs[op->s].f<=ctx->regs[op->t].f); }
void BGBFR2_Step_BinaryCmpLeD(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=(ctx->regs[op->s].d<=ctx->regs[op->t].d); }

void BGBFR2_Step_BinaryCmpGeI(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=(ctx->regs[op->s].i>=ctx->regs[op->t].i); }
void BGBFR2_Step_BinaryCmpGeL(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=(ctx->regs[op->s].l>=ctx->regs[op->t].l); }
void BGBFR2_Step_BinaryCmpGeF(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=(ctx->regs[op->s].f>=ctx->regs[op->t].f); }
void BGBFR2_Step_BinaryCmpGeD(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=(ctx->regs[op->s].d>=ctx->regs[op->t].d); }

void BGBFR2_Step_BinaryCmpUnI(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=0; }
void BGBFR2_Step_BinaryCmpUnL(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=0; }
void BGBFR2_Step_BinaryCmpUnF(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
{	ctx->regs[op->d].i=!(ctx->regs[op->s].f>=ctx->regs[op->t].f) &&
		!(ctx->regs[op->s].f<=ctx->regs[op->t].f); }
void BGBFR2_Step_BinaryCmpUnD(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
{	ctx->regs[op->d].i=!(ctx->regs[op->s].d>=ctx->regs[op->t].d) &&
		!(ctx->regs[op->s].d<=ctx->regs[op->t].d);}

void BGBFR2_Step_BinaryCmpTrI(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=0; }
void BGBFR2_Step_BinaryCmpTrL(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=0; }
void BGBFR2_Step_BinaryCmpTrF(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=0; }
void BGBFR2_Step_BinaryCmpTrD(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=0; }

void BGBFR2_Step_BinaryLoadIndexI(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=*(s32 *)BGBFR2_PointerAddr(
		ctx->regs[op->s].p, ctx->regs[op->t].i); }
void BGBFR2_Step_BinaryLoadIndexL(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].l=*(s64 *)BGBFR2_PointerAddr(
		ctx->regs[op->s].p, ctx->regs[op->t].i); }
void BGBFR2_Step_BinaryLoadIndexF(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].f=*(f32 *)BGBFR2_PointerAddr(
		ctx->regs[op->s].p, ctx->regs[op->t].i); }
void BGBFR2_Step_BinaryLoadIndexD(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].d=*(f64 *)BGBFR2_PointerAddr(
		ctx->regs[op->s].p, ctx->regs[op->t].i); }

void BGBFR2_Step_BinaryStoreIndexI(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ *(s32 *)BGBFR2_PointerAddr(ctx->regs[op->d].p,
		ctx->regs[op->s].i)=ctx->regs[op->t].i; }
void BGBFR2_Step_BinaryStoreIndexL(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ *(s64 *)BGBFR2_PointerAddr(ctx->regs[op->d].p,
		ctx->regs[op->s].i)=ctx->regs[op->t].l; }
void BGBFR2_Step_BinaryStoreIndexF(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ *(f32 *)BGBFR2_PointerAddr(ctx->regs[op->d].p,
		ctx->regs[op->s].i)=ctx->regs[op->t].f; }
void BGBFR2_Step_BinaryStoreIndexD(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ *(f64 *)BGBFR2_PointerAddr(ctx->regs[op->d].p,
		ctx->regs[op->s].i)=ctx->regs[op->t].d; }




void BGBFR2_Step_BinaryAddIC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=ctx->regs[op->s].i+op->c; }
void BGBFR2_Step_BinaryAddLC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].l=ctx->regs[op->s].l+op->c; }

void BGBFR2_Step_BinarySubIC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=ctx->regs[op->s].i-op->c; }
void BGBFR2_Step_BinarySubLC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].l=ctx->regs[op->s].l-op->c; }

void BGBFR2_Step_BinaryMulIC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=ctx->regs[op->s].i*op->c; }
void BGBFR2_Step_BinaryMulLC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].l=ctx->regs[op->s].l*op->c; }

void BGBFR2_Step_BinaryDivIC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=ctx->regs[op->s].i/op->c; }
void BGBFR2_Step_BinaryDivLC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].l=ctx->regs[op->s].l/op->c; }

void BGBFR2_Step_BinaryModIC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=ctx->regs[op->s].i%op->c; }
void BGBFR2_Step_BinaryModLC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].l=ctx->regs[op->s].l%op->c; }

void BGBFR2_Step_BinaryAndIC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=ctx->regs[op->s].i&op->c; }
void BGBFR2_Step_BinaryAndLC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].l=ctx->regs[op->s].l&op->c; }

void BGBFR2_Step_BinaryOrIC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=ctx->regs[op->s].i|op->c; }
void BGBFR2_Step_BinaryOrLC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].l=ctx->regs[op->s].l|op->c; }

void BGBFR2_Step_BinaryXorIC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=ctx->regs[op->s].i^op->c; }
void BGBFR2_Step_BinaryXorLC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].l=ctx->regs[op->s].l^op->c; }

void BGBFR2_Step_BinaryShlIC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=ctx->regs[op->s].i<<op->c; }
void BGBFR2_Step_BinaryShlLC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].l=ctx->regs[op->s].l<<op->c; }

void BGBFR2_Step_BinaryShrIC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=ctx->regs[op->s].i>>op->c; }
void BGBFR2_Step_BinaryShrLC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].l=ctx->regs[op->s].l>>op->c; }

void BGBFR2_Step_BinaryShrrIC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=(u32)ctx->regs[op->s].i>>op->c; }
void BGBFR2_Step_BinaryShrrLC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].l=(u64)ctx->regs[op->s].l>>op->c; }

void BGBFR2_Step_BinaryCmplIC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
{	FR2_Value *rs, *rt;
	rs=ctx->regs[op->s]; rt=ctx->regs[op->t];
	ctx->regs[op->d].i=(rs.i==op->c)?0:((rs.i>op->c)?1:(-1)); }
void BGBFR2_Step_BinaryCmplLC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
{	FR2_Value *rs, *rt;
	rs=ctx->regs[op->s]; rt=ctx->regs[op->t];
	ctx->regs[op->d].i=(rs.l==op->c)?0:((rs.l>op->c)?1:(-1)); }

void BGBFR2_Step_BinaryLAndIC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=(s32)ctx->regs[op->s].i&&op->c; }
void BGBFR2_Step_BinaryLAndLC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].l=(s64)ctx->regs[op->s].l&&op->c; }

void BGBFR2_Step_BinaryLOrIC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=(s32)ctx->regs[op->s].i||op->c; }
void BGBFR2_Step_BinaryLOrLC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].l=(s64)ctx->regs[op->s].l||op->c; }

void BGBFR2_Step_BinaryCmpEqIC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=(ctx->regs[op->s].i==op->c); }
void BGBFR2_Step_BinaryCmpEqLC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=(ctx->regs[op->s].l==op->c); }

void BGBFR2_Step_BinaryCmpNeIC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=(ctx->regs[op->s].i!=op->c); }
void BGBFR2_Step_BinaryCmpNeLC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=(ctx->regs[op->s].l!=op->c); }

void BGBFR2_Step_BinaryCmpLtIC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=(ctx->regs[op->s].i<op->c); }
void BGBFR2_Step_BinaryCmpLtLC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=(ctx->regs[op->s].l<op->c); }

void BGBFR2_Step_BinaryCmpGtIC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=(ctx->regs[op->s].i>op->c); }
void BGBFR2_Step_BinaryCmpGtLC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=(ctx->regs[op->s].l>op->c); }

void BGBFR2_Step_BinaryCmpLeIC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=(ctx->regs[op->s].i<=op->c); }
void BGBFR2_Step_BinaryCmpLeLC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=(ctx->regs[op->s].l<=op->c); }

void BGBFR2_Step_BinaryCmpGeIC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=(ctx->regs[op->s].i>=op->c); }
void BGBFR2_Step_BinaryCmpGeLC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=(ctx->regs[op->s].l>=op->c); }

void BGBFR2_Step_BinaryLoadIndexIC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].i=*(s32 *)BGBFR2_PointerAddr(
		ctx->regs[op->s].p, op->c); }
void BGBFR2_Step_BinaryLoadIndexLC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ ctx->regs[op->d].l=*(s64 *)BGBFR2_PointerAddr(
		ctx->regs[op->s].p, op->c); }

void BGBFR2_Step_BinaryStoreIndexIC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ *(s32 *)BGBFR2_PointerAddr(ctx->regs[op->d].p,
		op->c)=ctx->regs[op->s].i; }
void BGBFR2_Step_BinaryStoreIndexLC(FR2_RunFrame *ctx, FR2_OpcodeInfo *op)
	{ *(s64 *)BGBFR2_PointerAddr(ctx->regs[op->d].p,
		op->c)=ctx->regs[op->s].l; }
