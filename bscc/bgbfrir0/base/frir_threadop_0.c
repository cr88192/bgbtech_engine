#include <bgbfrir.h>

BGBFRIR_API void FRIR_ThOp_NOP(FRIR_Context *ctx, FRIR_Opcode *op)
{
}

BGBFRIR_API void FRIR_ThOp_ADD_I(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->ri[op->c]=ctx->ri[op->a]+ctx->ri[op->b]; }
BGBFRIR_API void FRIR_ThOp_ADD_L(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->rl[op->c]=ctx->rl[op->a]+ctx->rl[op->b]; }
BGBFRIR_API void FRIR_ThOp_ADD_F(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->rf[op->c]=ctx->rf[op->a]+ctx->rf[op->b]; }
BGBFRIR_API void FRIR_ThOp_ADD_D(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->rd[op->c]=ctx->rd[op->a]+ctx->rd[op->b]; }
BGBFRIR_API void FRIR_ThOp_ADD_A(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->ra[op->c]=ctx->ra[op->a]+ctx->ra[op->b]; }

BGBFRIR_API void FRIR_ThOp_SUB_I(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->ri[op->c]=ctx->ri[op->a]-ctx->ri[op->b]; }
BGBFRIR_API void FRIR_ThOp_SUB_L(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->rl[op->c]=ctx->rl[op->a]-ctx->rl[op->b]; }
BGBFRIR_API void FRIR_ThOp_SUB_F(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->rf[op->c]=ctx->rf[op->a]-ctx->rf[op->b]; }
BGBFRIR_API void FRIR_ThOp_SUB_D(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->rd[op->c]=ctx->rd[op->a]-ctx->rd[op->b]; }
BGBFRIR_API void FRIR_ThOp_SUB_A(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->ra[op->c]=ctx->ra[op->a]-ctx->ra[op->b]; }

BGBFRIR_API void FRIR_ThOp_MUL_I(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->ri[op->c]=ctx->ri[op->a]*ctx->ri[op->b]; }
BGBFRIR_API void FRIR_ThOp_MUL_L(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->rl[op->c]=ctx->rl[op->a]*ctx->rl[op->b]; }
BGBFRIR_API void FRIR_ThOp_MUL_F(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->rf[op->c]=ctx->rf[op->a]*ctx->rf[op->b]; }
BGBFRIR_API void FRIR_ThOp_MUL_D(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->rd[op->c]=ctx->rd[op->a]*ctx->rd[op->b]; }
BGBFRIR_API void FRIR_ThOp_MUL_A(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->ra[op->c]=ctx->ra[op->a]*ctx->ra[op->b]; }

BGBFRIR_API void FRIR_ThOp_DIV_I(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->ri[op->c]=ctx->ri[op->a]/ctx->ri[op->b]; }
BGBFRIR_API void FRIR_ThOp_DIV_L(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->rl[op->c]=ctx->rl[op->a]/ctx->rl[op->b]; }
BGBFRIR_API void FRIR_ThOp_DIV_F(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->rf[op->c]=ctx->rf[op->a]/ctx->rf[op->b]; }
BGBFRIR_API void FRIR_ThOp_DIV_D(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->rd[op->c]=ctx->rd[op->a]/ctx->rd[op->b]; }
BGBFRIR_API void FRIR_ThOp_DIV_A(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->ra[op->c]=ctx->ra[op->a]/ctx->ra[op->b]; }

BGBFRIR_API void FRIR_ThOp_MOD_I(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->ri[op->c]=ctx->ri[op->a]%ctx->ri[op->b]; }
BGBFRIR_API void FRIR_ThOp_MOD_L(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->rl[op->c]=ctx->rl[op->a]%ctx->rl[op->b]; }

BGBFRIR_API void FRIR_ThOp_AND_I(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->ri[op->c]=ctx->ri[op->a]&ctx->ri[op->b]; }
BGBFRIR_API void FRIR_ThOp_AND_L(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->rl[op->c]=ctx->rl[op->a]&ctx->rl[op->b]; }
BGBFRIR_API void FRIR_ThOp_OR_I(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->ri[op->c]=ctx->ri[op->a]|ctx->ri[op->b]; }
BGBFRIR_API void FRIR_ThOp_OR_L(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->rl[op->c]=ctx->rl[op->a]|ctx->rl[op->b]; }
BGBFRIR_API void FRIR_ThOp_XOR_I(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->ri[op->c]=ctx->ri[op->a]^ctx->ri[op->b]; }
BGBFRIR_API void FRIR_ThOp_XOR_L(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->rl[op->c]=ctx->rl[op->a]^ctx->rl[op->b]; }

BGBFRIR_API void FRIR_ThOp_SHL_I(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->ri[op->c]=ctx->ri[op->a]<<ctx->ri[op->b]; }
BGBFRIR_API void FRIR_ThOp_SHL_L(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->rl[op->c]=ctx->rl[op->a]<<ctx->ri[op->b]; }
BGBFRIR_API void FRIR_ThOp_SHR_I(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->ri[op->c]=ctx->ri[op->a]>>ctx->ri[op->b]; }
BGBFRIR_API void FRIR_ThOp_SHR_L(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->rl[op->c]=ctx->rl[op->a]>>ctx->ri[op->b]; }
BGBFRIR_API void FRIR_ThOp_SHRR_I(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->ri[op->c]=(u32)(ctx->ri[op->a])>>ctx->ri[op->b]; }
BGBFRIR_API void FRIR_ThOp_SHRR_L(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->rl[op->c]=(u64)(ctx->rl[op->a])>>ctx->ri[op->b]; }

BGBFRIR_API void FRIR_ThOp_NEG_I(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->ri[op->c]=-ctx->ri[op->a]; }
BGBFRIR_API void FRIR_ThOp_NEG_L(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->rl[op->c]=-ctx->rl[op->a]; }
BGBFRIR_API void FRIR_ThOp_NEG_F(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->rf[op->c]=-ctx->rf[op->a]; }
BGBFRIR_API void FRIR_ThOp_NEG_D(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->rd[op->c]=-ctx->rd[op->a]; }

BGBFRIR_API void FRIR_ThOp_NOT_I(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->ri[op->c]=~ctx->ri[op->a]; }
BGBFRIR_API void FRIR_ThOp_NOT_L(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->rl[op->c]=~ctx->rl[op->a]; }

BGBFRIR_API void FRIR_ThOp_MOV_IC(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->ri[op->c]=op->i.i; }
BGBFRIR_API void FRIR_ThOp_MOV_LC(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->rl[op->c]=op->i.l; }
BGBFRIR_API void FRIR_ThOp_MOV_FC(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->rf[op->c]=op->i.f; }
BGBFRIR_API void FRIR_ThOp_MOV_DC(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->rd[op->c]=op->i.d; }

BGBFRIR_API void FRIR_ThOp_MOV_I(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->ri[op->c]=ctx->ri[op->a]; }
BGBFRIR_API void FRIR_ThOp_MOV_L(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->rl[op->c]=ctx->rl[op->a]; }
BGBFRIR_API void FRIR_ThOp_MOV_F(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->rf[op->c]=ctx->rf[op->a]; }
BGBFRIR_API void FRIR_ThOp_MOV_D(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->rd[op->c]=ctx->rd[op->a]; }
BGBFRIR_API void FRIR_ThOp_MOV_A(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->ra[op->c]=ctx->ra[op->a]; }

BGBFRIR_API void FRIR_ThOp_CONV_I2L(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->rl[op->c]=ctx->ri[op->a]; }
BGBFRIR_API void FRIR_ThOp_CONV_I2F(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->rf[op->c]=ctx->ri[op->a]; }
BGBFRIR_API void FRIR_ThOp_CONV_I2D(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->rd[op->c]=ctx->ri[op->a]; }
BGBFRIR_API void FRIR_ThOp_CONV_I2A(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->ra[op->c]=ctx->ri[op->a]; }

BGBFRIR_API void FRIR_ThOp_LEA_B(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->ra[op->c]=ctx->ra[op->a]+ctx->ri[op->b]*sizeof(byte); }
BGBFRIR_API void FRIR_ThOp_LEA_S(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->ra[op->c]=ctx->ra[op->a]+ctx->ri[op->b]*sizeof(s16); }
BGBFRIR_API void FRIR_ThOp_LEA_I(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->ra[op->c]=ctx->ra[op->a]+ctx->ri[op->b]*sizeof(s32); }
BGBFRIR_API void FRIR_ThOp_LEA_L(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->ra[op->c]=ctx->ra[op->a]+ctx->ri[op->b]*sizeof(s64); }
BGBFRIR_API void FRIR_ThOp_LEA_F(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->ra[op->c]=ctx->ra[op->a]+ctx->ri[op->b]*sizeof(f32); }
BGBFRIR_API void FRIR_ThOp_LEA_D(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->ra[op->c]=ctx->ra[op->a]+ctx->ri[op->b]*sizeof(f64); }
BGBFRIR_API void FRIR_ThOp_LEA_A(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->ra[op->c]=ctx->ra[op->a]+ctx->ri[op->b]*sizeof(siptr); }

BGBFRIR_API void FRIR_ThOp_LEA2_B(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->ra[op->c]=ctx->ra[op->a]+(ctx->ri[op->b]+op->i.i)*sizeof(byte); }
BGBFRIR_API void FRIR_ThOp_LEA2_S(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->ra[op->c]=ctx->ra[op->a]+(ctx->ri[op->b]+op->i.i)*sizeof(s16); }
BGBFRIR_API void FRIR_ThOp_LEA2_I(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->ra[op->c]=ctx->ra[op->a]+(ctx->ri[op->b]+op->i.i)*sizeof(s32); }
BGBFRIR_API void FRIR_ThOp_LEA2_L(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->ra[op->c]=ctx->ra[op->a]+(ctx->ri[op->b]+op->i.i)*sizeof(s64); }
BGBFRIR_API void FRIR_ThOp_LEA2_F(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->ra[op->c]=ctx->ra[op->a]+(ctx->ri[op->b]+op->i.i)*sizeof(f32); }
BGBFRIR_API void FRIR_ThOp_LEA2_D(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->ra[op->c]=ctx->ra[op->a]+(ctx->ri[op->b]+op->i.i)*sizeof(f64); }
BGBFRIR_API void FRIR_ThOp_LEA2_A(FRIR_Context *ctx, FRIR_Opcode *op)
	{ ctx->ra[op->c]=ctx->ra[op->a]+(ctx->ri[op->b]+op->i.i)*sizeof(siptr); }
