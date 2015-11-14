#include <bgbfrir.h>

BGBFRIR_API FRIR_Trace *FRIR_ThOpE_NOP(FRIR_Context *ctx,
	FRIR_Trace *tr, FRIR_Opcode *op)
{
	return(tr->next);
}


FRIR_Trace *FRIR_ThOpE_JMP_LT_I(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	if(ctx->r[op->a].i<ctx->r[op->b].i)return(tr->jmpnext);
	return(tr->next);
}

FRIR_Trace *FRIR_ThOpE_JMP_LT_L(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	if(ctx->r[op->a].l<ctx->r[op->b].l)
		return(tr->jmpnext);
	return(tr->next);
}

FRIR_Trace *FRIR_ThOpE_JMP_LT_F(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	if(ctx->r[op->a].f<ctx->r[op->b].f)
		return(tr->jmpnext);
	return(tr->next);
}

FRIR_Trace *FRIR_ThOpE_JMP_LT_D(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	if(ctx->r[op->a].d<ctx->r[op->b].d)
		return(tr->jmpnext);
	return(tr->next);
}

FRIR_Trace *FRIR_ThOpE_JMP_LT_A(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	if(ctx->r[op->a].a<ctx->r[op->b].a)
		return(tr->jmpnext);
	return(tr->next);
}

FRIR_Trace *FRIR_ThOpE_JMP_GT_I(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	if(ctx->r[op->a].i>ctx->r[op->b].i)return(tr->jmpnext);
	return(tr->next);
}

FRIR_Trace *FRIR_ThOpE_JMP_GT_L(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	if(ctx->r[op->a].l>ctx->r[op->b].l)
		return(tr->jmpnext);
	return(tr->next);
}

FRIR_Trace *FRIR_ThOpE_JMP_GT_F(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	if(ctx->r[op->a].f>ctx->r[op->b].f)
		return(tr->jmpnext);
	return(tr->next);
}

FRIR_Trace *FRIR_ThOpE_JMP_GT_D(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	if(ctx->r[op->a].d>ctx->r[op->b].d)
		return(tr->jmpnext);
	return(tr->next);
}

FRIR_Trace *FRIR_ThOpE_JMP_GT_A(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	if(ctx->r[op->a].a>ctx->r[op->b].a)
		return(tr->jmpnext);
	return(tr->next);
}

FRIR_Trace *FRIR_ThOpE_JMP_LE_I(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	if(ctx->r[op->a].i<=ctx->r[op->b].i)return(tr->jmpnext);
	return(tr->next);
}

FRIR_Trace *FRIR_ThOpE_JMP_LE_L(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	if(ctx->r[op->a].l<=ctx->r[op->b].l)
		return(tr->jmpnext);
	return(tr->next);
}

FRIR_Trace *FRIR_ThOpE_JMP_LE_F(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	if(ctx->r[op->a].f<=ctx->r[op->b].f)
		return(tr->jmpnext);
	return(tr->next);
}

FRIR_Trace *FRIR_ThOpE_JMP_LE_D(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	if(ctx->r[op->a].d<=ctx->r[op->b].d)
		return(tr->jmpnext);
	return(tr->next);
}

FRIR_Trace *FRIR_ThOpE_JMP_LE_A(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	if(ctx->r[op->a].a<=ctx->r[op->b].a)
		return(tr->jmpnext);
	return(tr->next);
}

FRIR_Trace *FRIR_ThOpE_JMP_GE_I(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	if(ctx->r[op->a].i>=ctx->r[op->b].i)return(tr->jmpnext);
	return(tr->next);
}

FRIR_Trace *FRIR_ThOpE_JMP_GE_L(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	if(ctx->r[op->a].l>=ctx->r[op->b].l)
		return(tr->jmpnext);
	return(tr->next);
}

FRIR_Trace *FRIR_ThOpE_JMP_GE_F(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	if(ctx->r[op->a].f>=ctx->r[op->b].f)
		return(tr->jmpnext);
	return(tr->next);
}

FRIR_Trace *FRIR_ThOpE_JMP_GE_D(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	if(ctx->r[op->a].d>=ctx->r[op->b].d)
		return(tr->jmpnext);
	return(tr->next);
}

FRIR_Trace *FRIR_ThOpE_JMP_GE_A(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	if(ctx->r[op->a].a>=ctx->r[op->b].a)
		return(tr->jmpnext);
	return(tr->next);
}

FRIR_Trace *FRIR_ThOpE_JMP_EQ_I(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	if(ctx->r[op->a].i==ctx->r[op->b].i)return(tr->jmpnext);
	return(tr->next);
}

FRIR_Trace *FRIR_ThOpE_JMP_EQ_L(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	if(ctx->r[op->a].l==ctx->r[op->b].l)
		return(tr->jmpnext);
	return(tr->next);
}

FRIR_Trace *FRIR_ThOpE_JMP_EQ_F(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	if(ctx->r[op->a].f==ctx->r[op->b].f)
		return(tr->jmpnext);
	return(tr->next);
}

FRIR_Trace *FRIR_ThOpE_JMP_EQ_D(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	if(ctx->r[op->a].d==ctx->r[op->b].d)
		return(tr->jmpnext);
	return(tr->next);
}

FRIR_Trace *FRIR_ThOpE_JMP_EQ_A(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	if(ctx->r[op->a].a==ctx->r[op->b].a)
		return(tr->jmpnext);
	return(tr->next);
}

FRIR_Trace *FRIR_ThOpE_JMP_NE_I(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	if(ctx->r[op->a].i!=ctx->r[op->b].i)return(tr->jmpnext);
	return(tr->next);
}

FRIR_Trace *FRIR_ThOpE_JMP_NE_L(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	if(ctx->r[op->a].l!=ctx->r[op->b].l)
		return(tr->jmpnext);
	return(tr->next);
}

FRIR_Trace *FRIR_ThOpE_JMP_NE_F(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	if(ctx->r[op->a].f!=ctx->r[op->b].f)
		return(tr->jmpnext);
	return(tr->next);
}

FRIR_Trace *FRIR_ThOpE_JMP_NE_D(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	if(ctx->r[op->a].d!=ctx->r[op->b].d)
		return(tr->jmpnext);
	return(tr->next);
}

FRIR_Trace *FRIR_ThOpE_JMP_NE_A(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	if(ctx->r[op->a].a!=ctx->r[op->b].a)
		return(tr->jmpnext);
	return(tr->next);
}


FRIR_Trace *FRIR_ThOpE_JMP_LT_IC(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	if(ctx->r[op->a].i<op->i.i)return(tr->jmpnext);
	return(tr->next);
}

FRIR_Trace *FRIR_ThOpE_JMP_LT_LC(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	if(ctx->r[op->a].l<op->i.l)return(tr->jmpnext);
	return(tr->next);
}

FRIR_Trace *FRIR_ThOpE_JMP_LT_FC(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	if(ctx->r[op->a].f<op->i.f)return(tr->jmpnext);
	return(tr->next);
}

FRIR_Trace *FRIR_ThOpE_JMP_LT_DC(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	if(ctx->r[op->a].d<op->i.d)return(tr->jmpnext);
	return(tr->next);
}

FRIR_Trace *FRIR_ThOpE_JMP_GT_IC(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	if(ctx->r[op->a].i>op->i.i)return(tr->jmpnext);
	return(tr->next);
}

FRIR_Trace *FRIR_ThOpE_JMP_GT_LC(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	if(ctx->r[op->a].l>op->i.l)return(tr->jmpnext);
	return(tr->next);
}

FRIR_Trace *FRIR_ThOpE_JMP_GT_FC(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	if(ctx->r[op->a].f>op->i.f)return(tr->jmpnext);
	return(tr->next);
}

FRIR_Trace *FRIR_ThOpE_JMP_GT_DC(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	if(ctx->r[op->a].d>op->i.d)return(tr->jmpnext);
	return(tr->next);
}

FRIR_Trace *FRIR_ThOpE_JMP_LE_IC(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	if(ctx->r[op->a].i<=op->i.i)return(tr->jmpnext);
	return(tr->next);
}

FRIR_Trace *FRIR_ThOpE_JMP_LE_LC(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	if(ctx->r[op->a].l<=op->i.l)return(tr->jmpnext);
	return(tr->next);
}

FRIR_Trace *FRIR_ThOpE_JMP_LE_FC(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	if(ctx->r[op->a].f<=op->i.f)return(tr->jmpnext);
	return(tr->next);
}

FRIR_Trace *FRIR_ThOpE_JMP_LE_DC(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	if(ctx->r[op->a].d<=op->i.d)return(tr->jmpnext);
	return(tr->next);
}

FRIR_Trace *FRIR_ThOpE_JMP_GE_IC(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	if(ctx->r[op->a].i>=op->i.i)return(tr->jmpnext);
	return(tr->next);
}

FRIR_Trace *FRIR_ThOpE_JMP_GE_LC(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	if(ctx->r[op->a].l>=op->i.l)return(tr->jmpnext);
	return(tr->next);
}

FRIR_Trace *FRIR_ThOpE_JMP_GE_FC(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	if(ctx->r[op->a].f>=op->i.f)return(tr->jmpnext);
	return(tr->next);
}

FRIR_Trace *FRIR_ThOpE_JMP_GE_DC(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	if(ctx->r[op->a].d>=op->i.d)return(tr->jmpnext);
	return(tr->next);
}

FRIR_Trace *FRIR_ThOpE_JMP_EQ_IC(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	if(ctx->r[op->a].i==op->i.i)return(tr->jmpnext);
	return(tr->next);
}

FRIR_Trace *FRIR_ThOpE_JMP_EQ_LC(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	if(ctx->r[op->a].l==op->i.l)return(tr->jmpnext);
	return(tr->next);
}

FRIR_Trace *FRIR_ThOpE_JMP_EQ_FC(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	if(ctx->r[op->a].f==op->i.f)return(tr->jmpnext);
	return(tr->next);
}

FRIR_Trace *FRIR_ThOpE_JMP_EQ_DC(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	if(ctx->r[op->a].d==op->i.d)return(tr->jmpnext);
	return(tr->next);
}

FRIR_Trace *FRIR_ThOpE_JMP_NE_IC(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	if(ctx->r[op->a].i!=op->i.i)return(tr->jmpnext);
	return(tr->next);
}

FRIR_Trace *FRIR_ThOpE_JMP_NE_LC(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	if(ctx->r[op->a].l!=op->i.l)return(tr->jmpnext);
	return(tr->next);
}

FRIR_Trace *FRIR_ThOpE_JMP_NE_FC(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	if(ctx->r[op->a].f!=op->i.f)return(tr->jmpnext);
	return(tr->next);
}

FRIR_Trace *FRIR_ThOpE_JMP_NE_DC(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	if(ctx->r[op->a].d!=op->i.d)return(tr->jmpnext);
	return(tr->next);
}


FRIR_Trace *FRIR_ThOpE_JMP_LT_IZ(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	if(ctx->r[op->a].i<0)return(tr->jmpnext);
	return(tr->next);
}

FRIR_Trace *FRIR_ThOpE_JMP_GT_IZ(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	if(ctx->r[op->a].i>0)return(tr->jmpnext);
	return(tr->next);
}

FRIR_Trace *FRIR_ThOpE_JMP_LE_IZ(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	if(ctx->r[op->a].i<=0)return(tr->jmpnext);
	return(tr->next);
}

FRIR_Trace *FRIR_ThOpE_JMP_GE_IZ(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	if(ctx->r[op->a].i>=0)return(tr->jmpnext);
	return(tr->next);
}

FRIR_Trace *FRIR_ThOpE_JMP_EQ_IZ(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	if(ctx->r[op->a].i==0)return(tr->jmpnext);
	return(tr->next);
}

FRIR_Trace *FRIR_ThOpE_JMP_NE_IZ(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	if(ctx->r[op->a].i!=0)return(tr->jmpnext);
	return(tr->next);
}
