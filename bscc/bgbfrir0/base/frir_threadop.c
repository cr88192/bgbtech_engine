#include <bgbfrir.h>

BGBFRIR_API void FRIR_ThOp_NOP(FRIR_Context *ctx, FRIR_Opcode *op)
{
}

BGBFRIR_API void FRIR_ThOp_ADD_I(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].i+ctx->r[op->b].i; }
BGBFRIR_API void FRIR_ThOp_ADD_L(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].l=ctx->r[op->a].l+ctx->r[op->b].l; }
BGBFRIR_API void FRIR_ThOp_ADD_F(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].f=ctx->r[op->a].f+ctx->r[op->b].f; }
BGBFRIR_API void FRIR_ThOp_ADD_D(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].d=ctx->r[op->a].d+ctx->r[op->b].d; }
BGBFRIR_API void FRIR_ThOp_ADD_A(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].a=ctx->r[op->a].a+ctx->r[op->b].a; }

BGBFRIR_API void FRIR_ThOp_SUB_I(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].i-ctx->r[op->b].i; }
BGBFRIR_API void FRIR_ThOp_SUB_L(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].l=ctx->r[op->a].l-ctx->r[op->b].l; }
BGBFRIR_API void FRIR_ThOp_SUB_F(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].f=ctx->r[op->a].f-ctx->r[op->b].f; }
BGBFRIR_API void FRIR_ThOp_SUB_D(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].d=ctx->r[op->a].d-ctx->r[op->b].d; }
BGBFRIR_API void FRIR_ThOp_SUB_A(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].a=ctx->r[op->a].a-ctx->r[op->b].a; }

BGBFRIR_API void FRIR_ThOp_MUL_I(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].i*ctx->r[op->b].i; }
BGBFRIR_API void FRIR_ThOp_MUL_L(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].l=ctx->r[op->a].l*ctx->r[op->b].l; }
BGBFRIR_API void FRIR_ThOp_MUL_F(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].f=ctx->r[op->a].f*ctx->r[op->b].f; }
BGBFRIR_API void FRIR_ThOp_MUL_D(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].d=ctx->r[op->a].d*ctx->r[op->b].d; }
BGBFRIR_API void FRIR_ThOp_MUL_A(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].a=ctx->r[op->a].a*ctx->r[op->b].a; }

BGBFRIR_API void FRIR_ThOp_DIV_I(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].i/ctx->r[op->b].i; }
BGBFRIR_API void FRIR_ThOp_DIV_L(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].l=ctx->r[op->a].l/ctx->r[op->b].l; }
BGBFRIR_API void FRIR_ThOp_DIV_F(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].f=ctx->r[op->a].f/ctx->r[op->b].f; }
BGBFRIR_API void FRIR_ThOp_DIV_D(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].d=ctx->r[op->a].d/ctx->r[op->b].d; }
BGBFRIR_API void FRIR_ThOp_DIV_A(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].a=ctx->r[op->a].a/ctx->r[op->b].a; }

BGBFRIR_API void FRIR_ThOp_MOD_I(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].i%ctx->r[op->b].i; }
BGBFRIR_API void FRIR_ThOp_MOD_L(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].l=ctx->r[op->a].l%ctx->r[op->b].l; }

BGBFRIR_API void FRIR_ThOp_AND_I(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].i&ctx->r[op->b].i; }
BGBFRIR_API void FRIR_ThOp_AND_L(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].l=ctx->r[op->a].l&ctx->r[op->b].l; }
BGBFRIR_API void FRIR_ThOp_OR_I(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].i|ctx->r[op->b].i; }
BGBFRIR_API void FRIR_ThOp_OR_L(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].l=ctx->r[op->a].l|ctx->r[op->b].l; }
BGBFRIR_API void FRIR_ThOp_XOR_I(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].i^ctx->r[op->b].i; }
BGBFRIR_API void FRIR_ThOp_XOR_L(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].l=ctx->r[op->a].l^ctx->r[op->b].l; }

BGBFRIR_API void FRIR_ThOp_SHL_I(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].i<<ctx->r[op->b].i; }
BGBFRIR_API void FRIR_ThOp_SHL_L(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].l=ctx->r[op->a].l<<ctx->r[op->b].i; }
BGBFRIR_API void FRIR_ThOp_SHR_I(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].i>>ctx->r[op->b].i; }
BGBFRIR_API void FRIR_ThOp_SHR_L(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].l=ctx->r[op->a].l>>ctx->r[op->b].i; }
BGBFRIR_API void FRIR_ThOp_SHRR_I(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=(u32)(ctx->r[op->a].i)>>ctx->r[op->b].i; }
BGBFRIR_API void FRIR_ThOp_SHRR_L(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].l=(u64)(ctx->r[op->a].l)>>ctx->r[op->b].i; }

BGBFRIR_API void FRIR_ThOp_NEG_I(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=-ctx->r[op->a].i; }
BGBFRIR_API void FRIR_ThOp_NEG_L(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].l=-ctx->r[op->a].l; }
BGBFRIR_API void FRIR_ThOp_NEG_F(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].f=-ctx->r[op->a].f; }
BGBFRIR_API void FRIR_ThOp_NEG_D(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].d=-ctx->r[op->a].d; }

BGBFRIR_API void FRIR_ThOp_NOT_I(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=~ctx->r[op->a].i; }
BGBFRIR_API void FRIR_ThOp_NOT_L(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].l=~ctx->r[op->a].l; }

BGBFRIR_API void FRIR_ThOp_MOV_IC(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=op->i.i; }
BGBFRIR_API void FRIR_ThOp_MOV_LC(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].l=op->i.l; }
BGBFRIR_API void FRIR_ThOp_MOV_FC(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].f=op->i.f; }
BGBFRIR_API void FRIR_ThOp_MOV_DC(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].d=op->i.d; }

BGBFRIR_API void FRIR_ThOp_MOV_I(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].i; }
BGBFRIR_API void FRIR_ThOp_MOV_L(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].l=ctx->r[op->a].l; }
BGBFRIR_API void FRIR_ThOp_MOV_F(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].f=ctx->r[op->a].f; }
BGBFRIR_API void FRIR_ThOp_MOV_D(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].d=ctx->r[op->a].d; }
BGBFRIR_API void FRIR_ThOp_MOV_A(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].a=ctx->r[op->a].a; }

BGBFRIR_API void FRIR_ThOp_CONV_I2L(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].l=ctx->r[op->a].i; }
BGBFRIR_API void FRIR_ThOp_CONV_I2F(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].f=ctx->r[op->a].i; }
BGBFRIR_API void FRIR_ThOp_CONV_I2D(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].d=ctx->r[op->a].i; }
BGBFRIR_API void FRIR_ThOp_CONV_I2A(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].a=ctx->r[op->a].i; }
BGBFRIR_API void FRIR_ThOp_CONV_L2I(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].l; }
BGBFRIR_API void FRIR_ThOp_CONV_L2F(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].f=ctx->r[op->a].l; }
BGBFRIR_API void FRIR_ThOp_CONV_L2D(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].d=ctx->r[op->a].l; }
BGBFRIR_API void FRIR_ThOp_CONV_L2A(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].a=ctx->r[op->a].l; }
BGBFRIR_API void FRIR_ThOp_CONV_F2I(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].f; }
BGBFRIR_API void FRIR_ThOp_CONV_F2L(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].l=ctx->r[op->a].f; }
BGBFRIR_API void FRIR_ThOp_CONV_F2D(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].d=ctx->r[op->a].f; }
BGBFRIR_API void FRIR_ThOp_CONV_D2I(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].d; }
BGBFRIR_API void FRIR_ThOp_CONV_D2L(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].l=ctx->r[op->a].d; }
BGBFRIR_API void FRIR_ThOp_CONV_D2F(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].f=ctx->r[op->a].d; }
BGBFRIR_API void FRIR_ThOp_CONV_A2I(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].a; }
BGBFRIR_API void FRIR_ThOp_CONV_A2L(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].l=ctx->r[op->a].a; }

BGBFRIR_API void FRIR_ThOp_LEA_B(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].a=ctx->r[op->a].a+ctx->r[op->b].i*sizeof(byte); }
BGBFRIR_API void FRIR_ThOp_LEA_S(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].a=ctx->r[op->a].a+ctx->r[op->b].i*sizeof(s16); }
BGBFRIR_API void FRIR_ThOp_LEA_I(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].a=ctx->r[op->a].a+ctx->r[op->b].i*sizeof(s32); }
BGBFRIR_API void FRIR_ThOp_LEA_L(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].a=ctx->r[op->a].a+ctx->r[op->b].i*sizeof(s64); }
BGBFRIR_API void FRIR_ThOp_LEA_F(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].a=ctx->r[op->a].a+ctx->r[op->b].i*sizeof(f32); }
BGBFRIR_API void FRIR_ThOp_LEA_D(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].a=ctx->r[op->a].a+ctx->r[op->b].i*sizeof(f64); }
BGBFRIR_API void FRIR_ThOp_LEA_A(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].a=ctx->r[op->a].a+ctx->r[op->b].i*sizeof(siptr); }

BGBFRIR_API void FRIR_ThOp_LEA2_B(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].a=ctx->r[op->a].a+
		(ctx->r[op->b].i+op->i.i)*sizeof(byte); }
BGBFRIR_API void FRIR_ThOp_LEA2_S(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].a=ctx->r[op->a].a+
		(ctx->r[op->b].i+op->i.i)*sizeof(s16); }
BGBFRIR_API void FRIR_ThOp_LEA2_I(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].a=ctx->r[op->a].a+
		(ctx->r[op->b].i+op->i.i)*sizeof(s32); }
BGBFRIR_API void FRIR_ThOp_LEA2_L(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].a=ctx->r[op->a].a+
		(ctx->r[op->b].i+op->i.i)*sizeof(s64); }
BGBFRIR_API void FRIR_ThOp_LEA2_F(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].a=ctx->r[op->a].a+
		(ctx->r[op->b].i+op->i.i)*sizeof(f32); }
BGBFRIR_API void FRIR_ThOp_LEA2_D(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].a=ctx->r[op->a].a+
		(ctx->r[op->b].i+op->i.i)*sizeof(f64); }
BGBFRIR_API void FRIR_ThOp_LEA2_A(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].a=ctx->r[op->a].a+
		(ctx->r[op->b].i+op->i.i)*sizeof(siptr); }

BGBFRIR_API void FRIR_ThOp_LLLOAD_I(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->lvar[op->i.i].i; }
BGBFRIR_API void FRIR_ThOp_LLLOAD_L(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].l=ctx->lvar[op->i.i].l; }
BGBFRIR_API void FRIR_ThOp_LLLOAD_F(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].f=ctx->lvar[op->i.i].f; }
BGBFRIR_API void FRIR_ThOp_LLLOAD_D(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].d=ctx->lvar[op->i.i].d; }
BGBFRIR_API void FRIR_ThOp_LLLOAD_A(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].a=ctx->lvar[op->i.i].a; }

BGBFRIR_API void FRIR_ThOp_LLSTORE_I(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->lvar[op->i.i].i=ctx->r[op->a].i; }
BGBFRIR_API void FRIR_ThOp_LLSTORE_L(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->lvar[op->i.i].l=ctx->r[op->a].l; }
BGBFRIR_API void FRIR_ThOp_LLSTORE_F(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->lvar[op->i.i].f=ctx->r[op->a].f; }
BGBFRIR_API void FRIR_ThOp_LLSTORE_D(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->lvar[op->i.i].d=ctx->r[op->a].d; }
BGBFRIR_API void FRIR_ThOp_LLSTORE_A(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->lvar[op->i.i].a=ctx->r[op->a].a; }

BGBFRIR_API void FRIR_ThOp_LALOAD_I(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->larg[op->i.i].i; }
BGBFRIR_API void FRIR_ThOp_LALOAD_L(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].l=ctx->larg[op->i.i].l; }
BGBFRIR_API void FRIR_ThOp_LALOAD_F(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].f=ctx->larg[op->i.i].f; }
BGBFRIR_API void FRIR_ThOp_LALOAD_D(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].d=ctx->larg[op->i.i].d; }
BGBFRIR_API void FRIR_ThOp_LALOAD_A(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].a=ctx->larg[op->i.i].a; }

BGBFRIR_API void FRIR_ThOp_LASTORE_I(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->larg[op->i.i].i=ctx->r[op->a].i; }
BGBFRIR_API void FRIR_ThOp_LASTORE_L(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->larg[op->i.i].l=ctx->r[op->a].l; }
BGBFRIR_API void FRIR_ThOp_LASTORE_F(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->larg[op->i.i].f=ctx->r[op->a].f; }
BGBFRIR_API void FRIR_ThOp_LASTORE_D(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->larg[op->i.i].d=ctx->r[op->a].d; }
BGBFRIR_API void FRIR_ThOp_LASTORE_A(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->larg[op->i.i].a=ctx->r[op->a].a; }

BGBFRIR_API void FRIR_ThOp_LXLOAD_I(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->lenv[op->i.k.y]->vars[op->i.k.x].i; }
BGBFRIR_API void FRIR_ThOp_LXLOAD_L(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].l=ctx->lenv[op->i.k.y]->vars[op->i.k.x].l; }
BGBFRIR_API void FRIR_ThOp_LXLOAD_F(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].f=ctx->lenv[op->i.k.y]->vars[op->i.k.x].f; }
BGBFRIR_API void FRIR_ThOp_LXLOAD_D(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].d=ctx->lenv[op->i.k.y]->vars[op->i.k.x].d; }
BGBFRIR_API void FRIR_ThOp_LXLOAD_A(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].a=ctx->lenv[op->i.k.y]->vars[op->i.k.x].a; }

BGBFRIR_API void FRIR_ThOp_LXSTORE_I(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->lenv[op->i.k.y]->vars[op->i.k.x].i=ctx->r[op->a].i; }
BGBFRIR_API void FRIR_ThOp_LXSTORE_L(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->lenv[op->i.k.y]->vars[op->i.k.x].l=ctx->r[op->a].l; }
BGBFRIR_API void FRIR_ThOp_LXSTORE_F(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->lenv[op->i.k.y]->vars[op->i.k.x].f=ctx->r[op->a].f; }
BGBFRIR_API void FRIR_ThOp_LXSTORE_D(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->lenv[op->i.k.y]->vars[op->i.k.x].d=ctx->r[op->a].d; }
BGBFRIR_API void FRIR_ThOp_LXSTORE_A(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->lenv[op->i.k.y]->vars[op->i.k.x].a=ctx->r[op->a].a; }


BGBFRIR_API void FRIR_ThOp_CMP_LT_I(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].i<ctx->r[op->b].i; }
BGBFRIR_API void FRIR_ThOp_CMP_LT_L(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].l<ctx->r[op->b].l; }
BGBFRIR_API void FRIR_ThOp_CMP_LT_F(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].f<ctx->r[op->b].f; }
BGBFRIR_API void FRIR_ThOp_CMP_LT_D(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].d<ctx->r[op->b].d; }
BGBFRIR_API void FRIR_ThOp_CMP_LT_A(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].a<ctx->r[op->b].a; }
BGBFRIR_API void FRIR_ThOp_CMP_GT_I(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].i>ctx->r[op->b].i; }
BGBFRIR_API void FRIR_ThOp_CMP_GT_L(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].l>ctx->r[op->b].l; }
BGBFRIR_API void FRIR_ThOp_CMP_GT_F(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].f>ctx->r[op->b].f; }
BGBFRIR_API void FRIR_ThOp_CMP_GT_D(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].d>ctx->r[op->b].d; }
BGBFRIR_API void FRIR_ThOp_CMP_GT_A(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].a>ctx->r[op->b].a; }
BGBFRIR_API void FRIR_ThOp_CMP_LE_I(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].i<=ctx->r[op->b].i; }
BGBFRIR_API void FRIR_ThOp_CMP_LE_L(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].l<=ctx->r[op->b].l; }
BGBFRIR_API void FRIR_ThOp_CMP_LE_F(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].f<=ctx->r[op->b].f; }
BGBFRIR_API void FRIR_ThOp_CMP_LE_D(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].d<=ctx->r[op->b].d; }
BGBFRIR_API void FRIR_ThOp_CMP_LE_A(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].a<=ctx->r[op->b].a; }
BGBFRIR_API void FRIR_ThOp_CMP_GE_I(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].i>=ctx->r[op->b].i; }
BGBFRIR_API void FRIR_ThOp_CMP_GE_L(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].l>=ctx->r[op->b].l; }
BGBFRIR_API void FRIR_ThOp_CMP_GE_F(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].f>=ctx->r[op->b].f; }
BGBFRIR_API void FRIR_ThOp_CMP_GE_D(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].d>=ctx->r[op->b].d; }
BGBFRIR_API void FRIR_ThOp_CMP_GE_A(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].a>=ctx->r[op->b].a; }
BGBFRIR_API void FRIR_ThOp_CMP_EQ_I(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].i==ctx->r[op->b].i; }
BGBFRIR_API void FRIR_ThOp_CMP_EQ_L(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].l==ctx->r[op->b].l; }
BGBFRIR_API void FRIR_ThOp_CMP_EQ_F(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].f==ctx->r[op->b].f; }
BGBFRIR_API void FRIR_ThOp_CMP_EQ_D(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].d==ctx->r[op->b].d; }
BGBFRIR_API void FRIR_ThOp_CMP_EQ_A(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].a==ctx->r[op->b].a; }
BGBFRIR_API void FRIR_ThOp_CMP_NE_I(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].i!=ctx->r[op->b].i; }
BGBFRIR_API void FRIR_ThOp_CMP_NE_L(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].l!=ctx->r[op->b].l; }
BGBFRIR_API void FRIR_ThOp_CMP_NE_F(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].f!=ctx->r[op->b].f; }
BGBFRIR_API void FRIR_ThOp_CMP_NE_D(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].d!=ctx->r[op->b].d; }
BGBFRIR_API void FRIR_ThOp_CMP_NE_A(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].a!=ctx->r[op->b].a; }

BGBFRIR_API void FRIR_ThOp_CMP_LT_IC(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].i<op->i.i; }
BGBFRIR_API void FRIR_ThOp_CMP_LT_LC(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].l<op->i.l; }
BGBFRIR_API void FRIR_ThOp_CMP_LT_FC(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].f<op->i.f; }
BGBFRIR_API void FRIR_ThOp_CMP_LT_DC(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].d<op->i.d; }
BGBFRIR_API void FRIR_ThOp_CMP_GT_IC(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].i>op->i.i; }
BGBFRIR_API void FRIR_ThOp_CMP_GT_LC(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].l>op->i.l; }
BGBFRIR_API void FRIR_ThOp_CMP_GT_FC(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].f>op->i.f; }
BGBFRIR_API void FRIR_ThOp_CMP_GT_DC(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].d>op->i.d; }
BGBFRIR_API void FRIR_ThOp_CMP_LE_IC(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].i<=op->i.i; }
BGBFRIR_API void FRIR_ThOp_CMP_LE_LC(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].l<=op->i.l; }
BGBFRIR_API void FRIR_ThOp_CMP_LE_FC(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].f<=op->i.f; }
BGBFRIR_API void FRIR_ThOp_CMP_LE_DC(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].d<=op->i.d; }
BGBFRIR_API void FRIR_ThOp_CMP_GE_IC(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].i>=op->i.i; }
BGBFRIR_API void FRIR_ThOp_CMP_GE_LC(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].l>=op->i.l; }
BGBFRIR_API void FRIR_ThOp_CMP_GE_FC(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].f>=op->i.f; }
BGBFRIR_API void FRIR_ThOp_CMP_GE_DC(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].d>=op->i.d; }
BGBFRIR_API void FRIR_ThOp_CMP_EQ_IC(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].i==op->i.i; }
BGBFRIR_API void FRIR_ThOp_CMP_EQ_LC(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].l==op->i.l; }
BGBFRIR_API void FRIR_ThOp_CMP_EQ_FC(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].f==op->i.f; }
BGBFRIR_API void FRIR_ThOp_CMP_EQ_DC(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].d==op->i.d; }
BGBFRIR_API void FRIR_ThOp_CMP_NE_IC(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].i!=op->i.i; }
BGBFRIR_API void FRIR_ThOp_CMP_NE_LC(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].l!=op->i.l; }
BGBFRIR_API void FRIR_ThOp_CMP_NE_FC(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].f!=op->i.f; }
BGBFRIR_API void FRIR_ThOp_CMP_NE_DC(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].d!=op->i.d; }

BGBFRIR_API void FRIR_ThOp_ADD_IC(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].i+op->i.i; }
BGBFRIR_API void FRIR_ThOp_ADD_LC(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].l=ctx->r[op->a].l+op->i.l; }
BGBFRIR_API void FRIR_ThOp_SUB_IC(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].i-op->i.i; }
BGBFRIR_API void FRIR_ThOp_SUB_LC(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].l=ctx->r[op->a].l-op->i.l; }
BGBFRIR_API void FRIR_ThOp_MUL_IC(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].i*op->i.i; }
BGBFRIR_API void FRIR_ThOp_MUL_LC(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].l=ctx->r[op->a].l*op->i.l; }
BGBFRIR_API void FRIR_ThOp_DIV_IC(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].i/op->i.i; }
BGBFRIR_API void FRIR_ThOp_DIV_LC(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].l=ctx->r[op->a].l/op->i.l; }
BGBFRIR_API void FRIR_ThOp_MOD_IC(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].i%op->i.i; }
BGBFRIR_API void FRIR_ThOp_MOD_LC(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].l=ctx->r[op->a].l%op->i.l; }
BGBFRIR_API void FRIR_ThOp_AND_IC(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].i&op->i.i; }
BGBFRIR_API void FRIR_ThOp_AND_LC(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].l=ctx->r[op->a].l&op->i.l; }
BGBFRIR_API void FRIR_ThOp_OR_IC(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].i|op->i.i; }
BGBFRIR_API void FRIR_ThOp_OR_LC(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].l=ctx->r[op->a].l|op->i.l; }
BGBFRIR_API void FRIR_ThOp_XOR_IC(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].i^op->i.i; }
BGBFRIR_API void FRIR_ThOp_XOR_LC(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].l=ctx->r[op->a].l^op->i.l; }
BGBFRIR_API void FRIR_ThOp_SHL_IC(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].i<<op->i.i; }
BGBFRIR_API void FRIR_ThOp_SHL_LC(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].l=ctx->r[op->a].l<<op->i.l; }
BGBFRIR_API void FRIR_ThOp_SHR_IC(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=ctx->r[op->a].i>>op->i.i; }
BGBFRIR_API void FRIR_ThOp_SHR_LC(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].l=ctx->r[op->a].l>>op->i.l; }
BGBFRIR_API void FRIR_ThOp_SHRR_IC(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].i=((u32)(ctx->r[op->a].i))>>op->i.i; }
BGBFRIR_API void FRIR_ThOp_SHRR_LC(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->r[op->c].l=((u64)(ctx->r[op->a].l))>>op->i.l; }
