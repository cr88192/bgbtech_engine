#include <bgbfrir2.h>

FR2_RunFrame *BGBFR2_RunCtx_AllocFrame(FR2_RunCtx *ctx)
{
	FR2_RunFrame *tmp;

	tmp=BGBFR2_EnvCtx_AllocFrame(ctx->env);
	tmp->run=ctx;
	return(tmp);
}

void BGBFR2_RunCtx_FreeFrame(FR2_RunCtx *ctx, FR2_RunFrame *frm)
{
	BGBFR2_EnvCtx_FreeFrame(ctx->env, frm);
}

void BGBFR2_RunCtx_SetupFrame(FR2_RunFrame *frm, FR2_Function *func)
{
	frm->func=func;
	frm->ip=func->ip;
	
	if(!frm->regs || (frm->n_regs<func->n_regs))
	{
		if(frm->regs)gcfree(frm->regs);
		frm->regs=gcalloc(func->n_regs*sizeof(FR2_Value));
		frm->n_regs=func->n_regs;
	}
}

FR2_RunFrame *BGBFR2_RunCtx_SetupCall(
	FR2_RunFrame *frm, FR2_Function *func,
	int rtreg, byte *areg, int nareg)
{
	FR2_RunFrame *tmp;
	int i;

	tmp=BGBFR2_RunCtx_AllocFrame(frm->run);
	BGBFR2_RunCtx_SetupFrame(tmp, func);
	tmp->rtreg=frm->regs+rtreg;
	for(i=0; i<nareg; i++)
		{ tmp->regs[i]=frm->regs[areg[i]]; }
		
	tmp->next=frm->run->frame;
	frm->run->frame=tmp;
	return(tmp);
}

int BGBFR2_Interp_StepGeneric(FR2_RunFrame *ctx, int pfx, int opn)
{
	FR2_OpcodeInfo op;
	int opi, opj, opk, rs;
	int i, j, k, l;

	rs=0;
	switch(opn)
	{
	case FR2_OPG_UNARY:
		opi=*ctx->ip++; 
		op.d=*ctx->ip++;
		op.s=*ctx->ip++;
		switch(opi)
		{
		case FR2_UOP_MOV:
			switch(pfx)
			{
			case FR2_PFX_Z: rs=FR2_RS_INVALIDOPTYPE; break;
			case FR2_PFX_I: BGBFR2_Step_UnaryMovI(ctx, &op); break;
			case FR2_PFX_L: BGBFR2_Step_UnaryMovL(ctx, &op); break;
			case FR2_PFX_F: BGBFR2_Step_UnaryMovF(ctx, &op); break;
			case FR2_PFX_D: BGBFR2_Step_UnaryMovD(ctx, &op); break;
			case FR2_PFX_P: BGBFR2_Step_UnaryMovP(ctx, &op); break;
			case FR2_PFX_A: BGBFR2_Step_UnaryMovA(ctx, &op); break;
			default: rs=FR2_RS_INVALIDOPTYPE; break;
			}
			break;
		case FR2_UOP_NEG:
			switch(pfx)
			{
			case FR2_PFX_Z: rs=FR2_RS_INVALIDOPTYPE; break;
			case FR2_PFX_I: BGBFR2_Step_UnaryNegI(ctx, &op); break;
			case FR2_PFX_L: BGBFR2_Step_UnaryNegL(ctx, &op); break;
			case FR2_PFX_F: BGBFR2_Step_UnaryNegF(ctx, &op); break;
			case FR2_PFX_D: BGBFR2_Step_UnaryNegD(ctx, &op); break;
			default: rs=FR2_RS_INVALIDOPTYPE; break;
			}
			break;
		case FR2_UOP_NOT:
			switch(pfx)
			{
			case FR2_PFX_Z: rs=FR2_RS_INVALIDOPTYPE; break;
			case FR2_PFX_I: BGBFR2_Step_UnaryNotI(ctx, &op); break;
			case FR2_PFX_L: BGBFR2_Step_UnaryNotL(ctx, &op); break;
			case FR2_PFX_F: BGBFR2_Step_UnaryNotF(ctx, &op); break;
			case FR2_PFX_D: BGBFR2_Step_UnaryNotD(ctx, &op); break;
			default: rs=FR2_RS_INVALIDOPTYPE; break;
			}
			break;
		case FR2_UOP_LNOT:
			switch(pfx)
			{
			case FR2_PFX_Z: rs=FR2_RS_INVALIDOPTYPE; break;
			case FR2_PFX_I: BGBFR2_Step_UnaryLNotI(ctx, &op); break;
			case FR2_PFX_L: BGBFR2_Step_UnaryLNotL(ctx, &op); break;
			case FR2_PFX_F: BGBFR2_Step_UnaryLNotF(ctx, &op); break;
			case FR2_PFX_D: BGBFR2_Step_UnaryLNotD(ctx, &op); break;
			default: rs=FR2_RS_INVALIDOPTYPE; break;
			}
			break;
		case FR2_UOP_INC:
			switch(pfx)
			{
			case FR2_PFX_Z: rs=FR2_RS_INVALIDOPTYPE; break;
			case FR2_PFX_I: BGBFR2_Step_UnaryIncI(ctx, &op); break;
			case FR2_PFX_L: BGBFR2_Step_UnaryIncL(ctx, &op); break;
			case FR2_PFX_F: BGBFR2_Step_UnaryIncF(ctx, &op); break;
			case FR2_PFX_D: BGBFR2_Step_UnaryIncD(ctx, &op); break;
			default: rs=FR2_RS_INVALIDOPTYPE; break;
			}
			break;
		case FR2_UOP_DEC:
			switch(pfx)
			{
			case FR2_PFX_Z: rs=FR2_RS_INVALIDOPTYPE; break;
			case FR2_PFX_I: BGBFR2_Step_UnaryDecI(ctx, &op); break;
			case FR2_PFX_L: BGBFR2_Step_UnaryDecL(ctx, &op); break;
			case FR2_PFX_F: BGBFR2_Step_UnaryDecF(ctx, &op); break;
			case FR2_PFX_D: BGBFR2_Step_UnaryDecD(ctx, &op); break;
			default: rs=FR2_RS_INVALIDOPTYPE; break;
			}
			break;
		case FR2_UOP_DEFER:
			switch(pfx)
			{
			case FR2_PFX_Z: rs=FR2_RS_INVALIDOPTYPE; break;
			case FR2_PFX_I: BGBFR2_Step_UnaryDeferI(ctx, &op); break;
			case FR2_PFX_L: BGBFR2_Step_UnaryDeferL(ctx, &op); break;
			case FR2_PFX_F: BGBFR2_Step_UnaryDeferF(ctx, &op); break;
			case FR2_PFX_D: BGBFR2_Step_UnaryDeferD(ctx, &op); break;
			default: rs=FR2_RS_INVALIDOPTYPE; break;
			}
			break;
		case FR2_UOP_SETDEFER:
			switch(pfx)
			{
			case FR2_PFX_Z: rs=FR2_RS_INVALIDOPTYPE; break;
			case FR2_PFX_I: BGBFR2_Step_UnarySetDeferI(ctx, &op); break;
			case FR2_PFX_L: BGBFR2_Step_UnarySetDeferL(ctx, &op); break;
			case FR2_PFX_F: BGBFR2_Step_UnarySetDeferF(ctx, &op); break;
			case FR2_PFX_D: BGBFR2_Step_UnarySetDeferD(ctx, &op); break;
			default: rs=FR2_RS_INVALIDOPTYPE; break;
			}
			break;
		default:
			rs=FR2_RS_INVALIDOPCODE;
			break;
		}
	case FR2_OPG_BINARY:
		opi=*ctx->ip++; 
		op.d=*ctx->ip++;
		op.s=*ctx->ip++;
		op.t=*ctx->ip++;
		switch(opi)
		{
		case FR2_BOP_ADD:
			switch(pfx)
			{
			case FR2_PFX_Z: rs=FR2_RS_INVALIDOPTYPE; break;
			case FR2_PFX_I: BGBFR2_Step_BinaryAddI(ctx, &op); break;
			case FR2_PFX_L: BGBFR2_Step_BinaryAddL(ctx, &op); break;
			case FR2_PFX_F: BGBFR2_Step_BinaryAddF(ctx, &op); break;
			case FR2_PFX_D: BGBFR2_Step_BinaryAddD(ctx, &op); break;
			default: rs=FR2_RS_INVALIDOPTYPE; break;
			}
			break;
		case FR2_BOP_SUB:
			switch(pfx)
			{
			case FR2_PFX_Z: rs=FR2_RS_INVALIDOPTYPE; break;
			case FR2_PFX_I: BGBFR2_Step_BinarySubI(ctx, &op); break;
			case FR2_PFX_L: BGBFR2_Step_BinarySubL(ctx, &op); break;
			case FR2_PFX_F: BGBFR2_Step_BinarySubF(ctx, &op); break;
			case FR2_PFX_D: BGBFR2_Step_BinarySubD(ctx, &op); break;
			default: rs=FR2_RS_INVALIDOPTYPE; break;
			}
			break;
		case FR2_BOP_MUL:
			switch(pfx)
			{
			case FR2_PFX_Z: rs=FR2_RS_INVALIDOPTYPE; break;
			case FR2_PFX_I: BGBFR2_Step_BinaryMulI(ctx, &op); break;
			case FR2_PFX_L: BGBFR2_Step_BinaryMulL(ctx, &op); break;
			case FR2_PFX_F: BGBFR2_Step_BinaryMulF(ctx, &op); break;
			case FR2_PFX_D: BGBFR2_Step_BinaryMulD(ctx, &op); break;
			default: rs=FR2_RS_INVALIDOPTYPE; break;
			}
			break;
		case FR2_BOP_DIV:
			switch(pfx)
			{
			case FR2_PFX_Z: rs=FR2_RS_INVALIDOPTYPE; break;
			case FR2_PFX_I: BGBFR2_Step_BinaryDivI(ctx, &op); break;
			case FR2_PFX_L: BGBFR2_Step_BinaryDivL(ctx, &op); break;
			case FR2_PFX_F: BGBFR2_Step_BinaryDivF(ctx, &op); break;
			case FR2_PFX_D: BGBFR2_Step_BinaryDivD(ctx, &op); break;
			default: rs=FR2_RS_INVALIDOPTYPE; break;
			}
			break;
		case FR2_BOP_MOD:
			switch(pfx)
			{
			case FR2_PFX_Z: rs=FR2_RS_INVALIDOPTYPE; break;
			case FR2_PFX_I: BGBFR2_Step_BinaryModI(ctx, &op); break;
			case FR2_PFX_L: BGBFR2_Step_BinaryModL(ctx, &op); break;
			case FR2_PFX_F: BGBFR2_Step_BinaryModF(ctx, &op); break;
			case FR2_PFX_D: BGBFR2_Step_BinaryModD(ctx, &op); break;
			default: rs=FR2_RS_INVALIDOPTYPE; break;
			}
			break;
		case FR2_BOP_AND:
			switch(pfx)
			{
			case FR2_PFX_Z: rs=FR2_RS_INVALIDOPTYPE; break;
			case FR2_PFX_I: BGBFR2_Step_BinaryAndI(ctx, &op); break;
			case FR2_PFX_L: BGBFR2_Step_BinaryAndL(ctx, &op); break;
			case FR2_PFX_F: BGBFR2_Step_BinaryAndF(ctx, &op); break;
			case FR2_PFX_D: BGBFR2_Step_BinaryAndD(ctx, &op); break;
			default: rs=FR2_RS_INVALIDOPTYPE; break;
			}
			break;
		case FR2_BOP_OR:
			switch(pfx)
			{
			case FR2_PFX_Z: rs=FR2_RS_INVALIDOPTYPE; break;
			case FR2_PFX_I: BGBFR2_Step_BinaryOrI(ctx, &op); break;
			case FR2_PFX_L: BGBFR2_Step_BinaryOrL(ctx, &op); break;
			case FR2_PFX_F: BGBFR2_Step_BinaryOrF(ctx, &op); break;
			case FR2_PFX_D: BGBFR2_Step_BinaryOrD(ctx, &op); break;
			default: rs=FR2_RS_INVALIDOPTYPE; break;
			}
			break;
		case FR2_BOP_XOR:
			switch(pfx)
			{
			case FR2_PFX_Z: rs=FR2_RS_INVALIDOPTYPE; break;
			case FR2_PFX_I: BGBFR2_Step_BinaryXorI(ctx, &op); break;
			case FR2_PFX_L: BGBFR2_Step_BinaryXorL(ctx, &op); break;
			case FR2_PFX_F: BGBFR2_Step_BinaryXorF(ctx, &op); break;
			case FR2_PFX_D: BGBFR2_Step_BinaryXorD(ctx, &op); break;
			default: rs=FR2_RS_INVALIDOPTYPE; break;
			}
			break;
		case FR2_BOP_SHL:
			switch(pfx)
			{
			case FR2_PFX_Z: rs=FR2_RS_INVALIDOPTYPE; break;
			case FR2_PFX_I: BGBFR2_Step_BinaryShlI(ctx, &op); break;
			case FR2_PFX_L: BGBFR2_Step_BinaryShlL(ctx, &op); break;
			case FR2_PFX_F: BGBFR2_Step_BinaryShlF(ctx, &op); break;
			case FR2_PFX_D: BGBFR2_Step_BinaryShlD(ctx, &op); break;
			default: rs=FR2_RS_INVALIDOPTYPE; break;
			}
			break;
		case FR2_BOP_SHR:
			switch(pfx)
			{
			case FR2_PFX_Z: rs=FR2_RS_INVALIDOPTYPE; break;
			case FR2_PFX_I: BGBFR2_Step_BinaryShrI(ctx, &op); break;
			case FR2_PFX_L: BGBFR2_Step_BinaryShrL(ctx, &op); break;
			case FR2_PFX_F: BGBFR2_Step_BinaryShrF(ctx, &op); break;
			case FR2_PFX_D: BGBFR2_Step_BinaryShrD(ctx, &op); break;
			default: rs=FR2_RS_INVALIDOPTYPE; break;
			}
			break;
		case FR2_BOP_SHRR:
			switch(pfx)
			{
			case FR2_PFX_Z: rs=FR2_RS_INVALIDOPTYPE; break;
			case FR2_PFX_I: BGBFR2_Step_BinaryShrrI(ctx, &op); break;
			case FR2_PFX_L: BGBFR2_Step_BinaryShrrL(ctx, &op); break;
			case FR2_PFX_F: BGBFR2_Step_BinaryShrrF(ctx, &op); break;
			case FR2_PFX_D: BGBFR2_Step_BinaryShrrD(ctx, &op); break;
			default: rs=FR2_RS_INVALIDOPTYPE; break;
			}
			break;
		case FR2_BOP_CMPL:
			switch(pfx)
			{
			case FR2_PFX_Z: rs=FR2_RS_INVALIDOPTYPE; break;
			case FR2_PFX_I: BGBFR2_Step_BinaryCmplI(ctx, &op); break;
			case FR2_PFX_L: BGBFR2_Step_BinaryCmplL(ctx, &op); break;
			case FR2_PFX_F: BGBFR2_Step_BinaryCmplF(ctx, &op); break;
			case FR2_PFX_D: BGBFR2_Step_BinaryCmplD(ctx, &op); break;
			default: rs=FR2_RS_INVALIDOPTYPE; break;
			}
			break;
		case FR2_BOP_CMPG:
			switch(pfx)
			{
			case FR2_PFX_Z: rs=FR2_RS_INVALIDOPTYPE; break;
			case FR2_PFX_I: BGBFR2_Step_BinaryCmpgI(ctx, &op); break;
			case FR2_PFX_L: BGBFR2_Step_BinaryCmpgL(ctx, &op); break;
			case FR2_PFX_F: BGBFR2_Step_BinaryCmpgF(ctx, &op); break;
			case FR2_PFX_D: BGBFR2_Step_BinaryCmpgD(ctx, &op); break;
			default: rs=FR2_RS_INVALIDOPTYPE; break;
			}
			break;
		case FR2_BOP_LAND:
			switch(pfx)
			{
			case FR2_PFX_Z: rs=FR2_RS_INVALIDOPTYPE; break;
			case FR2_PFX_I: BGBFR2_Step_BinaryLAndI(ctx, &op); break;
			case FR2_PFX_L: BGBFR2_Step_BinaryLAndL(ctx, &op); break;
			case FR2_PFX_F: BGBFR2_Step_BinaryLAndF(ctx, &op); break;
			case FR2_PFX_D: BGBFR2_Step_BinaryLAndD(ctx, &op); break;
			default: rs=FR2_RS_INVALIDOPTYPE; break;
			}
			break;
		case FR2_BOP_LOR:
			switch(pfx)
			{
			case FR2_PFX_Z: rs=FR2_RS_INVALIDOPTYPE; break;
			case FR2_PFX_I: BGBFR2_Step_BinaryLOrI(ctx, &op); break;
			case FR2_PFX_L: BGBFR2_Step_BinaryLOrL(ctx, &op); break;
			case FR2_PFX_F: BGBFR2_Step_BinaryLOrF(ctx, &op); break;
			case FR2_PFX_D: BGBFR2_Step_BinaryLOrD(ctx, &op); break;
			default: rs=FR2_RS_INVALIDOPTYPE; break;
			}
			break;
		case FR2_BOP_CMP_EQ:
			switch(pfx)
			{
			case FR2_PFX_Z: rs=FR2_RS_INVALIDOPTYPE; break;
			case FR2_PFX_I: BGBFR2_Step_BinaryCmpEqI(ctx, &op); break;
			case FR2_PFX_L: BGBFR2_Step_BinaryCmpEqL(ctx, &op); break;
			case FR2_PFX_F: BGBFR2_Step_BinaryCmpEqF(ctx, &op); break;
			case FR2_PFX_D: BGBFR2_Step_BinaryCmpEqD(ctx, &op); break;
			default: rs=FR2_RS_INVALIDOPTYPE; break;
			}
			break;
		case FR2_BOP_CMP_NE:
			switch(pfx)
			{
			case FR2_PFX_Z: rs=FR2_RS_INVALIDOPTYPE; break;
			case FR2_PFX_I: BGBFR2_Step_BinaryCmpNeI(ctx, &op); break;
			case FR2_PFX_L: BGBFR2_Step_BinaryCmpNeL(ctx, &op); break;
			case FR2_PFX_F: BGBFR2_Step_BinaryCmpNeF(ctx, &op); break;
			case FR2_PFX_D: BGBFR2_Step_BinaryCmpNeD(ctx, &op); break;
			default: rs=FR2_RS_INVALIDOPTYPE; break;
			}
			break;
		case FR2_BOP_CMP_LT:
			switch(pfx)
			{
			case FR2_PFX_Z: rs=FR2_RS_INVALIDOPTYPE; break;
			case FR2_PFX_I: BGBFR2_Step_BinaryCmpLtI(ctx, &op); break;
			case FR2_PFX_L: BGBFR2_Step_BinaryCmpLtL(ctx, &op); break;
			case FR2_PFX_F: BGBFR2_Step_BinaryCmpLtF(ctx, &op); break;
			case FR2_PFX_D: BGBFR2_Step_BinaryCmpLtD(ctx, &op); break;
			default: rs=FR2_RS_INVALIDOPTYPE; break;
			}
			break;
		case FR2_BOP_CMP_GT:
			switch(pfx)
			{
			case FR2_PFX_Z: rs=FR2_RS_INVALIDOPTYPE; break;
			case FR2_PFX_I: BGBFR2_Step_BinaryCmpGtI(ctx, &op); break;
			case FR2_PFX_L: BGBFR2_Step_BinaryCmpGtL(ctx, &op); break;
			case FR2_PFX_F: BGBFR2_Step_BinaryCmpGtF(ctx, &op); break;
			case FR2_PFX_D: BGBFR2_Step_BinaryCmpGtD(ctx, &op); break;
			default: rs=FR2_RS_INVALIDOPTYPE; break;
			}
			break;
		case FR2_BOP_CMP_LE:
			switch(pfx)
			{
			case FR2_PFX_Z: rs=FR2_RS_INVALIDOPTYPE; break;
			case FR2_PFX_I: BGBFR2_Step_BinaryCmpLeI(ctx, &op); break;
			case FR2_PFX_L: BGBFR2_Step_BinaryCmpLeL(ctx, &op); break;
			case FR2_PFX_F: BGBFR2_Step_BinaryCmpLeF(ctx, &op); break;
			case FR2_PFX_D: BGBFR2_Step_BinaryCmpLeD(ctx, &op); break;
			default: rs=FR2_RS_INVALIDOPTYPE; break;
			}
			break;
		case FR2_BOP_CMP_GE:
			switch(pfx)
			{
			case FR2_PFX_Z: rs=FR2_RS_INVALIDOPTYPE; break;
			case FR2_PFX_I: BGBFR2_Step_BinaryCmpGeI(ctx, &op); break;
			case FR2_PFX_L: BGBFR2_Step_BinaryCmpGeL(ctx, &op); break;
			case FR2_PFX_F: BGBFR2_Step_BinaryCmpGeF(ctx, &op); break;
			case FR2_PFX_D: BGBFR2_Step_BinaryCmpGeD(ctx, &op); break;
			default: rs=FR2_RS_INVALIDOPTYPE; break;
			}
			break;
		case FR2_BOP_CMP_UN:
			switch(pfx)
			{
			case FR2_PFX_Z: rs=FR2_RS_INVALIDOPTYPE; break;
			case FR2_PFX_I: BGBFR2_Step_BinaryCmpUnI(ctx, &op); break;
			case FR2_PFX_L: BGBFR2_Step_BinaryCmpUnL(ctx, &op); break;
			case FR2_PFX_F: BGBFR2_Step_BinaryCmpUnF(ctx, &op); break;
			case FR2_PFX_D: BGBFR2_Step_BinaryCmpUnD(ctx, &op); break;
			default: rs=FR2_RS_INVALIDOPTYPE; break;
			}
			break;
		case FR2_BOP_CMP_TR:
			switch(pfx)
			{
			case FR2_PFX_Z: rs=FR2_RS_INVALIDOPTYPE; break;
			case FR2_PFX_I: BGBFR2_Step_BinaryCmpTrI(ctx, &op); break;
			case FR2_PFX_L: BGBFR2_Step_BinaryCmpTrL(ctx, &op); break;
			case FR2_PFX_F: BGBFR2_Step_BinaryCmpTrF(ctx, &op); break;
			case FR2_PFX_D: BGBFR2_Step_BinaryCmpTrD(ctx, &op); break;
			default: rs=FR2_RS_INVALIDOPTYPE; break;
			}
			break;
		case FR2_BOP_LOADINDEX:
			switch(pfx)
			{
			case FR2_PFX_Z: rs=FR2_RS_INVALIDOPTYPE; break;
			case FR2_PFX_I: BGBFR2_Step_BinaryLoadIndexI(ctx, &op); break;
			case FR2_PFX_L: BGBFR2_Step_BinaryLoadIndexL(ctx, &op); break;
			case FR2_PFX_F: BGBFR2_Step_BinaryLoadIndexF(ctx, &op); break;
			case FR2_PFX_D: BGBFR2_Step_BinaryLoadIndexD(ctx, &op); break;
			default: rs=FR2_RS_INVALIDOPTYPE; break;
			}
			break;
		case FR2_BOP_STOREINDEX:
			switch(pfx)
			{
			case FR2_PFX_Z: rs=FR2_RS_INVALIDOPTYPE; break;
			case FR2_PFX_I: BGBFR2_Step_BinaryStoreIndexI(ctx, &op); break;
			case FR2_PFX_L: BGBFR2_Step_BinaryStoreIndexL(ctx, &op); break;
			case FR2_PFX_F: BGBFR2_Step_BinaryStoreIndexF(ctx, &op); break;
			case FR2_PFX_D: BGBFR2_Step_BinaryStoreIndexD(ctx, &op); break;
			default: rs=FR2_RS_INVALIDOPTYPE; break;
			}
			break;
		default:
			rs=FR2_RS_INVALIDOPCODE;
			break;
		}
		break;
	case FR2_OPG_CALL:
		opi=*ctx->ip++;
		opj=*ctx->ip++;
		opk=*ctx->ip++;
		BGBFR2_RunCtx_SetupCall(ctx, ctx->regs[opj].a, opi, ctx->ip, opk);
		ctx->ip+=opk;
		rs=FR2_RS_CALL;
		break;
	case FR2_OPG_RET:
		if(pfx==FR2_PFX_Z)
			{ rs=FR2_RS_RETURN; break; }
		opi=*ctx->ip++;
		if(ctx->rtreg)
			{ *(ctx->rtreg)=ctx->regs[opi]; }
		rs=FR2_RS_RETURN;
		break;
	case FR2_OPG_SLOAD:
		op.d=*ctx->ip++;
		op.c=(ctx->ip[0]<<8)|(ctx->ip[1]); ctx->ip+=2;
		switch(pfx)
		{
		case FR2_PFX_Z: rs=FR2_RS_INVALIDOPTYPE; break;
		case FR2_PFX_I: BGBFR2_Step_SLoadIC(ctx, &op); break;
		case FR2_PFX_L: BGBFR2_Step_SLoadLC(ctx, &op); break;
		case FR2_PFX_F: BGBFR2_Step_SLoadFC(ctx, &op); break;
		case FR2_PFX_D: BGBFR2_Step_SLoadDC(ctx, &op); break;
		default: rs=FR2_RS_INVALIDOPTYPE; break;
		}
		break;
	case FR2_OPG_SSTORE:
		op.s=*ctx->ip++;
		op.c=(ctx->ip[0]<<8)|(ctx->ip[1]); ctx->ip+=2;
		switch(pfx)
		{
		case FR2_PFX_Z: rs=FR2_RS_INVALIDOPTYPE; break;
		case FR2_PFX_I: BGBFR2_Step_SStoreIC(ctx, &op); break;
		case FR2_PFX_L: BGBFR2_Step_SStoreLC(ctx, &op); break;
		case FR2_PFX_F: BGBFR2_Step_SStoreFC(ctx, &op); break;
		case FR2_PFX_D: BGBFR2_Step_SStoreDC(ctx, &op); break;
		default: rs=FR2_RS_INVALIDOPTYPE; break;
		}
		break;
	case FR2_OPG_SCALL:
		opj=(ctx->ip[0]<<8)|(ctx->ip[1]); ctx->ip+=2;
		opi=*ctx->ip++;
		opk=*ctx->ip++;
		BGBFR2_RunCtx_SetupCall(ctx,
			ctx->object->svar[opj]->val->a,
			opi, ctx->ip, opk);
		ctx->ip+=opk;
		rs=FR2_RS_CALL;
		break;
	case FR2_OPG_LDCONST:
		op.d=*ctx->ip++;
		op.c=(ctx->ip[0]<<8)|(ctx->ip[1]); ctx->ip+=2;
		switch(pfx)
		{
		case FR2_PFX_Z: rs=FR2_RS_INVALIDOPTYPE; break;
		case FR2_PFX_I: BGBFR2_Step_LoadConstIC(ctx, &op); break;
		case FR2_PFX_L: BGBFR2_Step_LoadConstLC(ctx, &op); break;
		case FR2_PFX_F: BGBFR2_Step_LoadConstFC(ctx, &op); break;
		case FR2_PFX_D: BGBFR2_Step_LoadConstDC(ctx, &op); break;
		default: rs=FR2_RS_INVALIDOPTYPE; break;
		}
		break;
	default:
		rs=FR2_RS_INVALIDOPCODE;
		break;
	}
	return(rs);
}

int BGBFR2_Interp_Step(FR2_RunFrame *ctx)
{
	FR2_OpcodeInfo op;
	int opi, opj, opk, opn, pfx, rs;
	int i, j, k, l;
	
	rs=0;
	opi=*ctx->ip++;
	switch(opi)
	{
	case (FR2_OPFX_UNARY4I|FR2_UOP_MOV):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_UnaryMovI(ctx, &op); break;
	case (FR2_OPFX_UNARY4I|FR2_UOP_NEG):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_UnaryNegI(ctx, &op); break;
	case (FR2_OPFX_UNARY4I|FR2_UOP_NOT):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_UnaryNotI(ctx, &op); break;
	case (FR2_OPFX_UNARY4I|FR2_UOP_LNOT):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_UnaryLNotI(ctx, &op); break;
	case (FR2_OPFX_UNARY4I|FR2_UOP_INC):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_UnaryIncI(ctx, &op); break;
	case (FR2_OPFX_UNARY4I|FR2_UOP_DEC):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_UnaryDecI(ctx, &op); break;
	case (FR2_OPFX_UNARY4I|FR2_UOP_DEFER):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_UnaryDeferI(ctx, &op); break;
	case (FR2_OPFX_UNARY4I|FR2_UOP_SETDEFER):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_UnarySetDeferI(ctx, &op); break;

	case (FR2_OPFX_UNARY4L|FR2_UOP_MOV):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_UnaryMovL(ctx, &op); break;
	case (FR2_OPFX_UNARY4L|FR2_UOP_NEG):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_UnaryNegL(ctx, &op); break;
	case (FR2_OPFX_UNARY4L|FR2_UOP_NOT):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_UnaryNotL(ctx, &op); break;
	case (FR2_OPFX_UNARY4L|FR2_UOP_LNOT):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_UnaryLNotL(ctx, &op); break;
	case (FR2_OPFX_UNARY4L|FR2_UOP_INC):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_UnaryIncL(ctx, &op); break;
	case (FR2_OPFX_UNARY4L|FR2_UOP_DEC):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_UnaryDecL(ctx, &op); break;
	case (FR2_OPFX_UNARY4L|FR2_UOP_DEFER):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_UnaryDeferL(ctx, &op); break;
	case (FR2_OPFX_UNARY4L|FR2_UOP_SETDEFER):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_UnarySetDeferL(ctx, &op); break;

	case (FR2_OPFX_UNARY4F|FR2_UOP_MOV):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_UnaryMovF(ctx, &op); break;
	case (FR2_OPFX_UNARY4F|FR2_UOP_NEG):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_UnaryNegF(ctx, &op); break;
	case (FR2_OPFX_UNARY4F|FR2_UOP_NOT):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_UnaryNotF(ctx, &op); break;
	case (FR2_OPFX_UNARY4F|FR2_UOP_LNOT):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_UnaryLNotF(ctx, &op); break;
	case (FR2_OPFX_UNARY4F|FR2_UOP_INC):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_UnaryIncF(ctx, &op); break;
	case (FR2_OPFX_UNARY4F|FR2_UOP_DEC):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_UnaryDecF(ctx, &op); break;
	case (FR2_OPFX_UNARY4F|FR2_UOP_DEFER):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_UnaryDeferF(ctx, &op); break;
	case (FR2_OPFX_UNARY4F|FR2_UOP_SETDEFER):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_UnarySetDeferF(ctx, &op); break;

	case (FR2_OPFX_UNARY4D|FR2_UOP_MOV):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_UnaryMovD(ctx, &op); break;
	case (FR2_OPFX_UNARY4D|FR2_UOP_NEG):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_UnaryNegD(ctx, &op); break;
	case (FR2_OPFX_UNARY4D|FR2_UOP_NOT):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_UnaryNotD(ctx, &op); break;
	case (FR2_OPFX_UNARY4D|FR2_UOP_LNOT):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_UnaryLNotD(ctx, &op); break;
	case (FR2_OPFX_UNARY4D|FR2_UOP_INC):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_UnaryIncD(ctx, &op); break;
	case (FR2_OPFX_UNARY4D|FR2_UOP_DEC):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_UnaryDecD(ctx, &op); break;
	case (FR2_OPFX_UNARY4D|FR2_UOP_DEFER):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_UnaryDeferD(ctx, &op); break;
	case (FR2_OPFX_UNARY4D|FR2_UOP_SETDEFER):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_UnarySetDeferD(ctx, &op); break;

	case (FR2_OPFX_BINARY4I| 0):	case (FR2_OPFX_BINARY4I| 1):
	case (FR2_OPFX_BINARY4I| 2):	case (FR2_OPFX_BINARY4I| 3):
	case (FR2_OPFX_BINARY4I| 4):	case (FR2_OPFX_BINARY4I| 5):
	case (FR2_OPFX_BINARY4I| 6):	case (FR2_OPFX_BINARY4I| 7):
	case (FR2_OPFX_BINARY4I| 8):	case (FR2_OPFX_BINARY4I| 9):
	case (FR2_OPFX_BINARY4I|10):	case (FR2_OPFX_BINARY4I|11):
	case (FR2_OPFX_BINARY4I|12):	case (FR2_OPFX_BINARY4I|13):
	case (FR2_OPFX_BINARY4I|14):	case (FR2_OPFX_BINARY4I|15):
		opj=*ctx->ip++; opk=*ctx->ip++;
		opn=((opi&15)<<4)|((opj>>4)&15);
		op.d=opj&15; op.s=(opjk>4)&15; op.t=opk&15;
		switch(opn)
		{
		case FR2_BOP_ADD:    BGBFR2_Step_BinaryAddI(ctx, &op); break;
		case FR2_BOP_SUB:    BGBFR2_Step_BinarySubI(ctx, &op); break;
		case FR2_BOP_MUL:    BGBFR2_Step_BinaryMulI(ctx, &op); break;
		case FR2_BOP_DIV:    BGBFR2_Step_BinaryDivI(ctx, &op); break;
		case FR2_BOP_MOD:    BGBFR2_Step_BinaryModI(ctx, &op); break;
		case FR2_BOP_AND:    BGBFR2_Step_BinaryAndI(ctx, &op); break;
		case FR2_BOP_OR:     BGBFR2_Step_BinaryOrI(ctx, &op); break;
		case FR2_BOP_XOR:    BGBFR2_Step_BinaryXorI(ctx, &op); break;
		case FR2_BOP_SHL:    BGBFR2_Step_BinaryShlI(ctx, &op); break;
		case FR2_BOP_SHR:    BGBFR2_Step_BinaryShrI(ctx, &op); break;
		case FR2_BOP_SHRR:   BGBFR2_Step_BinaryShrrI(ctx, &op); break;
		case FR2_BOP_CMPL:   BGBFR2_Step_BinaryCmplI(ctx, &op); break;
		case FR2_BOP_CMPG:   BGBFR2_Step_BinaryCmpgI(ctx, &op); break;
		case FR2_BOP_LAND:   BGBFR2_Step_BinaryLAndI(ctx, &op); break;
		case FR2_BOP_LOR:    BGBFR2_Step_BinaryLOrI(ctx, &op); break;
		case FR2_BOP_LXOR:   BGBFR2_Step_BinaryLXorI(ctx, &op); break;
		case FR2_BOP_CMP_EQ: BGBFR2_Step_BinaryCmpEqI(ctx, &op); break;
		case FR2_BOP_CMP_NE: BGBFR2_Step_BinaryCmpNeI(ctx, &op); break;
		case FR2_BOP_CMP_LT: BGBFR2_Step_BinaryCmpLtI(ctx, &op); break;
		case FR2_BOP_CMP_GT: BGBFR2_Step_BinaryCmpGtI(ctx, &op); break;
		case FR2_BOP_CMP_LE: BGBFR2_Step_BinaryCmpLeI(ctx, &op); break;
		case FR2_BOP_CMP_GE: BGBFR2_Step_BinaryCmpGeI(ctx, &op); break;
		case FR2_BOP_CMP_UN: BGBFR2_Step_BinaryCmpUnI(ctx, &op); break;
		case FR2_BOP_CMP_TR: BGBFR2_Step_BinaryCmpTrI(ctx, &op); break;
		case FR2_BOP_LOADINDEX:
			BGBFR2_Step_BinaryLoadIndexI(ctx, &op); break;
		case FR2_BOP_STOREINDEX:
			BGBFR2_Step_BinaryStoreIndexI(ctx, &op); break;
		default: rs=FR2_RS_INVALIDOPCODE; break;
		}
		break;
	case (FR2_OPFX_BINARY4L| 0):	case (FR2_OPFX_BINARY4L| 1):
	case (FR2_OPFX_BINARY4L| 2):	case (FR2_OPFX_BINARY4L| 3):
	case (FR2_OPFX_BINARY4L| 4):	case (FR2_OPFX_BINARY4L| 5):
	case (FR2_OPFX_BINARY4L| 6):	case (FR2_OPFX_BINARY4L| 7):
	case (FR2_OPFX_BINARY4L| 8):	case (FR2_OPFX_BINARY4L| 9):
	case (FR2_OPFX_BINARY4L|10):	case (FR2_OPFX_BINARY4L|11):
	case (FR2_OPFX_BINARY4L|12):	case (FR2_OPFX_BINARY4L|13):
	case (FR2_OPFX_BINARY4L|14):	case (FR2_OPFX_BINARY4L|15):
		opj=*ctx->ip++; opk=*ctx->ip++;
		opn=((opi&15)<<4)|((opj>>4)&15);
		op.d=opj&15; op.s=(opjk>4)&15; op.t=opk&15;
		switch(opn)
		{
		case FR2_BOP_ADD:    BGBFR2_Step_BinaryAddL(ctx, &op); break;
		case FR2_BOP_SUB:    BGBFR2_Step_BinarySubL(ctx, &op); break;
		case FR2_BOP_MUL:    BGBFR2_Step_BinaryMulL(ctx, &op); break;
		case FR2_BOP_DIV:    BGBFR2_Step_BinaryDivL(ctx, &op); break;
		case FR2_BOP_MOD:    BGBFR2_Step_BinaryModL(ctx, &op); break;
		case FR2_BOP_AND:    BGBFR2_Step_BinaryAndL(ctx, &op); break;
		case FR2_BOP_OR:     BGBFR2_Step_BinaryOrL(ctx, &op); break;
		case FR2_BOP_XOR:    BGBFR2_Step_BinaryXorL(ctx, &op); break;
		case FR2_BOP_SHL:    BGBFR2_Step_BinaryShlL(ctx, &op); break;
		case FR2_BOP_SHR:    BGBFR2_Step_BinaryShrL(ctx, &op); break;
		case FR2_BOP_SHRR:   BGBFR2_Step_BinaryShrrL(ctx, &op); break;
		case FR2_BOP_CMPL:   BGBFR2_Step_BinaryCmplL(ctx, &op); break;
		case FR2_BOP_CMPG:   BGBFR2_Step_BinaryCmpgL(ctx, &op); break;
		case FR2_BOP_LAND:   BGBFR2_Step_BinaryLAndL(ctx, &op); break;
		case FR2_BOP_LOR:    BGBFR2_Step_BinaryLOrL(ctx, &op); break;
		case FR2_BOP_LXOR:   BGBFR2_Step_BinaryLXorL(ctx, &op); break;
		case FR2_BOP_CMP_EQ: BGBFR2_Step_BinaryCmpEqL(ctx, &op); break;
		case FR2_BOP_CMP_NE: BGBFR2_Step_BinaryCmpNeL(ctx, &op); break;
		case FR2_BOP_CMP_LT: BGBFR2_Step_BinaryCmpLtL(ctx, &op); break;
		case FR2_BOP_CMP_GT: BGBFR2_Step_BinaryCmpGtL(ctx, &op); break;
		case FR2_BOP_CMP_LE: BGBFR2_Step_BinaryCmpLeL(ctx, &op); break;
		case FR2_BOP_CMP_GE: BGBFR2_Step_BinaryCmpGeL(ctx, &op); break;
		case FR2_BOP_CMP_UN: BGBFR2_Step_BinaryCmpUnL(ctx, &op); break;
		case FR2_BOP_CMP_TR: BGBFR2_Step_BinaryCmpTrL(ctx, &op); break;
		case FR2_BOP_LOADINDEX:
			BGBFR2_Step_BinaryLoadIndexL(ctx, &op); break;
		case FR2_BOP_STOREINDEX:
			BGBFR2_Step_BinaryStoreIndexL(ctx, &op); break;
		default: rs=FR2_RS_INVALIDOPCODE; break;
		}
		break;
	case (FR2_OPFX_BINARY4F| 0):	case (FR2_OPFX_BINARY4F| 1):
	case (FR2_OPFX_BINARY4F| 2):	case (FR2_OPFX_BINARY4F| 3):
	case (FR2_OPFX_BINARY4F| 4):	case (FR2_OPFX_BINARY4F| 5):
	case (FR2_OPFX_BINARY4F| 6):	case (FR2_OPFX_BINARY4F| 7):
	case (FR2_OPFX_BINARY4F| 8):	case (FR2_OPFX_BINARY4F| 9):
	case (FR2_OPFX_BINARY4F|10):	case (FR2_OPFX_BINARY4F|11):
	case (FR2_OPFX_BINARY4F|12):	case (FR2_OPFX_BINARY4F|13):
	case (FR2_OPFX_BINARY4F|14):	case (FR2_OPFX_BINARY4F|15):
		opj=*ctx->ip++; opk=*ctx->ip++;
		opn=((opi&15)<<4)|((opj>>4)&15);
		op.d=opj&15; op.s=(opjk>4)&15; op.t=opk&15;
		switch(opn)
		{
		case FR2_BOP_ADD:    BGBFR2_Step_BinaryAddF(ctx, &op); break;
		case FR2_BOP_SUB:    BGBFR2_Step_BinarySubF(ctx, &op); break;
		case FR2_BOP_MUL:    BGBFR2_Step_BinaryMulF(ctx, &op); break;
		case FR2_BOP_DIV:    BGBFR2_Step_BinaryDivF(ctx, &op); break;
		case FR2_BOP_MOD:    BGBFR2_Step_BinaryModF(ctx, &op); break;
		case FR2_BOP_AND:    BGBFR2_Step_BinaryAndF(ctx, &op); break;
		case FR2_BOP_OR:     BGBFR2_Step_BinaryOrF(ctx, &op); break;
		case FR2_BOP_XOR:    BGBFR2_Step_BinaryXorF(ctx, &op); break;
		case FR2_BOP_SHL:    BGBFR2_Step_BinaryShlF(ctx, &op); break;
		case FR2_BOP_SHR:    BGBFR2_Step_BinaryShrF(ctx, &op); break;
		case FR2_BOP_SHRR:   BGBFR2_Step_BinaryShrrF(ctx, &op); break;
		case FR2_BOP_CMPL:   BGBFR2_Step_BinaryCmplF(ctx, &op); break;
		case FR2_BOP_CMPG:   BGBFR2_Step_BinaryCmpgF(ctx, &op); break;
		case FR2_BOP_LAND:   BGBFR2_Step_BinaryLAndF(ctx, &op); break;
		case FR2_BOP_LOR:    BGBFR2_Step_BinaryLOrF(ctx, &op); break;
		case FR2_BOP_LXOR:   BGBFR2_Step_BinaryLXorF(ctx, &op); break;
		case FR2_BOP_CMP_EQ: BGBFR2_Step_BinaryCmpEqF(ctx, &op); break;
		case FR2_BOP_CMP_NE: BGBFR2_Step_BinaryCmpNeF(ctx, &op); break;
		case FR2_BOP_CMP_LT: BGBFR2_Step_BinaryCmpLtF(ctx, &op); break;
		case FR2_BOP_CMP_GT: BGBFR2_Step_BinaryCmpGtF(ctx, &op); break;
		case FR2_BOP_CMP_LE: BGBFR2_Step_BinaryCmpLeF(ctx, &op); break;
		case FR2_BOP_CMP_GE: BGBFR2_Step_BinaryCmpGeF(ctx, &op); break;
		case FR2_BOP_CMP_UN: BGBFR2_Step_BinaryCmpUnF(ctx, &op); break;
		case FR2_BOP_CMP_TR: BGBFR2_Step_BinaryCmpTrF(ctx, &op); break;
		case FR2_BOP_LOADINDEX:
			BGBFR2_Step_BinaryLoadIndexF(ctx, &op); break;
		case FR2_BOP_STOREINDEX:
			BGBFR2_Step_BinaryStoreIndexF(ctx, &op); break;
		default: rs=FR2_RS_INVALIDOPCODE; break;
		}
		break;
	case (FR2_OPFX_BINARY4D| 0):	case (FR2_OPFX_BINARY4D| 1):
	case (FR2_OPFX_BINARY4D| 2):	case (FR2_OPFX_BINARY4D| 3):
	case (FR2_OPFX_BINARY4D| 4):	case (FR2_OPFX_BINARY4D| 5):
	case (FR2_OPFX_BINARY4D| 6):	case (FR2_OPFX_BINARY4D| 7):
	case (FR2_OPFX_BINARY4D| 8):	case (FR2_OPFX_BINARY4D| 9):
	case (FR2_OPFX_BINARY4D|10):	case (FR2_OPFX_BINARY4D|11):
	case (FR2_OPFX_BINARY4D|12):	case (FR2_OPFX_BINARY4D|13):
	case (FR2_OPFX_BINARY4D|14):	case (FR2_OPFX_BINARY4D|15):
		opj=*ctx->ip++; opk=*ctx->ip++;
		opn=((opi&15)<<4)|((opj>>4)&15);
		op.d=opj&15; op.s=(opjk>4)&15; op.t=opk&15;
		switch(opn)
		{
		case FR2_BOP_ADD:    BGBFR2_Step_BinaryAddD(ctx, &op); break;
		case FR2_BOP_SUB:    BGBFR2_Step_BinarySubD(ctx, &op); break;
		case FR2_BOP_MUL:    BGBFR2_Step_BinaryMulD(ctx, &op); break;
		case FR2_BOP_DIV:    BGBFR2_Step_BinaryDivD(ctx, &op); break;
		case FR2_BOP_MOD:    BGBFR2_Step_BinaryModD(ctx, &op); break;
		case FR2_BOP_AND:    BGBFR2_Step_BinaryAndD(ctx, &op); break;
		case FR2_BOP_OR:     BGBFR2_Step_BinaryOrD(ctx, &op); break;
		case FR2_BOP_XOR:    BGBFR2_Step_BinaryXorD(ctx, &op); break;
		case FR2_BOP_SHL:    BGBFR2_Step_BinaryShlD(ctx, &op); break;
		case FR2_BOP_SHR:    BGBFR2_Step_BinaryShrD(ctx, &op); break;
		case FR2_BOP_SHRR:   BGBFR2_Step_BinaryShrrD(ctx, &op); break;
		case FR2_BOP_CMPL:   BGBFR2_Step_BinaryCmplD(ctx, &op); break;
		case FR2_BOP_CMPG:   BGBFR2_Step_BinaryCmpgD(ctx, &op); break;
		case FR2_BOP_LAND:   BGBFR2_Step_BinaryLAndD(ctx, &op); break;
		case FR2_BOP_LOR:    BGBFR2_Step_BinaryLOrD(ctx, &op); break;
		case FR2_BOP_LXOR:   BGBFR2_Step_BinaryLXorD(ctx, &op); break;
		case FR2_BOP_CMP_EQ: BGBFR2_Step_BinaryCmpEqD(ctx, &op); break;
		case FR2_BOP_CMP_NE: BGBFR2_Step_BinaryCmpNeD(ctx, &op); break;
		case FR2_BOP_CMP_LT: BGBFR2_Step_BinaryCmpLtD(ctx, &op); break;
		case FR2_BOP_CMP_GT: BGBFR2_Step_BinaryCmpGtD(ctx, &op); break;
		case FR2_BOP_CMP_LE: BGBFR2_Step_BinaryCmpLeD(ctx, &op); break;
		case FR2_BOP_CMP_GE: BGBFR2_Step_BinaryCmpGeD(ctx, &op); break;
		case FR2_BOP_CMP_UN: BGBFR2_Step_BinaryCmpUnD(ctx, &op); break;
		case FR2_BOP_CMP_TR: BGBFR2_Step_BinaryCmpTrD(ctx, &op); break;
		case FR2_BOP_LOADINDEX:
			BGBFR2_Step_BinaryLoadIndexD(ctx, &op); break;
		case FR2_BOP_STOREINDEX:
			BGBFR2_Step_BinaryStoreIndexD(ctx, &op); break;
		default: rs=FR2_RS_INVALIDOPCODE; break;
		}
		break;

#if 0
	case (FR2_OPFX_BINARY4I|FR2_BOP_ADD):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryAddI(ctx, &op); break;
	case (FR2_OPFX_BINARY4I|FR2_BOP_SUB):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinarySubI(ctx, &op); break;
	case (FR2_OPFX_BINARY4I|FR2_BOP_MUL):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryMulI(ctx, &op); break;
	case (FR2_OPFX_BINARY4I|FR2_BOP_DIV):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryDivI(ctx, &op); break;
	case (FR2_OPFX_BINARY4I|FR2_BOP_MOD):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryModI(ctx, &op); break;
	case (FR2_OPFX_BINARY4I|FR2_BOP_AND):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryAndI(ctx, &op); break;
	case (FR2_OPFX_BINARY4I|FR2_BOP_OR):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryOrI(ctx, &op); break;
	case (FR2_OPFX_BINARY4I|FR2_BOP_XOR):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryXorI(ctx, &op); break;
	case (FR2_OPFX_BINARY4I|FR2_BOP_SHL):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryShlI(ctx, &op); break;
	case (FR2_OPFX_BINARY4I|FR2_BOP_SHR):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryShrI(ctx, &op); break;
	case (FR2_OPFX_BINARY4I|FR2_BOP_SHRR):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryShrrI(ctx, &op); break;
	case (FR2_OPFX_BINARY4I|FR2_BOP_CMPL):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryCmplI(ctx, &op); break;
	case (FR2_OPFX_BINARY4I|FR2_BOP_CMPG):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryCmpgI(ctx, &op); break;
	case (FR2_OPFX_BINARY4I|FR2_BOP_LAND):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryLAndI(ctx, &op); break;
	case (FR2_OPFX_BINARY4I|FR2_BOP_LOR):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryLOrI(ctx, &op); break;
	case (FR2_OPFX_BINARY4I|FR2_BOP_CMP_EQ):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryCmpEqI(ctx, &op); break;
	case (FR2_OPFX_BINARY4I|FR2_BOP_CMP_NE):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryCmpNeI(ctx, &op); break;
	case (FR2_OPFX_BINARY4I|FR2_BOP_CMP_LT):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryCmpLtI(ctx, &op); break;
	case (FR2_OPFX_BINARY4I|FR2_BOP_CMP_GT):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryCmpGtI(ctx, &op); break;
	case (FR2_OPFX_BINARY4I|FR2_BOP_CMP_LE):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryCmpLeI(ctx, &op); break;
	case (FR2_OPFX_BINARY4I|FR2_BOP_CMP_GE):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryCmpGeI(ctx, &op); break;
	case (FR2_OPFX_BINARY4I|FR2_BOP_CMP_UN):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryCmpUnI(ctx, &op); break;
	case (FR2_OPFX_BINARY4I|FR2_BOP_CMP_TR):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryCmpTrI(ctx, &op); break;
	case (FR2_OPFX_BINARY4I|FR2_BOP_LOADINDEX):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryLoadIndexI(ctx, &op); break;
	case (FR2_OPFX_BINARY4I|FR2_BOP_STOREINDEX):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryStoreIndexI(ctx, &op); break;

	case (FR2_OPFX_BINARY4L|FR2_BOP_ADD):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryAddL(ctx, &op); break;
	case (FR2_OPFX_BINARY4L|FR2_BOP_SUB):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinarySubL(ctx, &op); break;
	case (FR2_OPFX_BINARY4L|FR2_BOP_MUL):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryMulL(ctx, &op); break;
	case (FR2_OPFX_BINARY4L|FR2_BOP_DIV):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryDivL(ctx, &op); break;
	case (FR2_OPFX_BINARY4L|FR2_BOP_MOD):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryModL(ctx, &op); break;
	case (FR2_OPFX_BINARY4L|FR2_BOP_AND):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryAndL(ctx, &op); break;
	case (FR2_OPFX_BINARY4L|FR2_BOP_OR):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryOrL(ctx, &op); break;
	case (FR2_OPFX_BINARY4L|FR2_BOP_XOR):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryXorL(ctx, &op); break;
	case (FR2_OPFX_BINARY4L|FR2_BOP_SHL):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryShlL(ctx, &op); break;
	case (FR2_OPFX_BINARY4L|FR2_BOP_SHR):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryShrL(ctx, &op); break;
	case (FR2_OPFX_BINARY4L|FR2_BOP_SHRR):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryShrrL(ctx, &op); break;
	case (FR2_OPFX_BINARY4L|FR2_BOP_CMPL):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryCmplL(ctx, &op); break;
	case (FR2_OPFX_BINARY4L|FR2_BOP_CMPG):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryCmpgL(ctx, &op); break;
	case (FR2_OPFX_BINARY4L|FR2_BOP_LAND):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryLAndL(ctx, &op); break;
	case (FR2_OPFX_BINARY4L|FR2_BOP_LOR):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryLOrL(ctx, &op); break;
	case (FR2_OPFX_BINARY4L|FR2_BOP_CMP_EQ):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryCmpEqL(ctx, &op); break;
	case (FR2_OPFX_BINARY4L|FR2_BOP_CMP_NE):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryCmpNeL(ctx, &op); break;
	case (FR2_OPFX_BINARY4L|FR2_BOP_CMP_LT):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryCmpLtL(ctx, &op); break;
	case (FR2_OPFX_BINARY4L|FR2_BOP_CMP_GT):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryCmpGtL(ctx, &op); break;
	case (FR2_OPFX_BINARY4L|FR2_BOP_CMP_LE):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryCmpLeL(ctx, &op); break;
	case (FR2_OPFX_BINARY4L|FR2_BOP_CMP_GE):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryCmpGeL(ctx, &op); break;
	case (FR2_OPFX_BINARY4L|FR2_BOP_CMP_UN):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryCmpUnL(ctx, &op); break;
	case (FR2_OPFX_BINARY4L|FR2_BOP_CMP_TR):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryCmpTrL(ctx, &op); break;
	case (FR2_OPFX_BINARY4L|FR2_BOP_LOADINDEX):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryLoadIndexL(ctx, &op); break;
	case (FR2_OPFX_BINARY4L|FR2_BOP_STOREINDEX):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryStoreIndexL(ctx, &op); break;

	case (FR2_OPFX_BINARY4F|FR2_BOP_ADD):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryAddF(ctx, &op); break;
	case (FR2_OPFX_BINARY4F|FR2_BOP_SUB):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinarySubF(ctx, &op); break;
	case (FR2_OPFX_BINARY4F|FR2_BOP_MUL):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryMulF(ctx, &op); break;
	case (FR2_OPFX_BINARY4F|FR2_BOP_DIV):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryDivF(ctx, &op); break;
	case (FR2_OPFX_BINARY4F|FR2_BOP_MOD):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryModF(ctx, &op); break;
	case (FR2_OPFX_BINARY4F|FR2_BOP_AND):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryAndF(ctx, &op); break;
	case (FR2_OPFX_BINARY4F|FR2_BOP_OR):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryOrF(ctx, &op); break;
	case (FR2_OPFX_BINARY4F|FR2_BOP_XOR):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryXorF(ctx, &op); break;
	case (FR2_OPFX_BINARY4F|FR2_BOP_SHL):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryShlF(ctx, &op); break;
	case (FR2_OPFX_BINARY4F|FR2_BOP_SHR):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryShrF(ctx, &op); break;
	case (FR2_OPFX_BINARY4F|FR2_BOP_SHRR):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryShrrF(ctx, &op); break;
	case (FR2_OPFX_BINARY4F|FR2_BOP_CMPL):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryCmplF(ctx, &op); break;
	case (FR2_OPFX_BINARY4F|FR2_BOP_CMPG):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryCmpgF(ctx, &op); break;
	case (FR2_OPFX_BINARY4F|FR2_BOP_LAND):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryLAndF(ctx, &op); break;
	case (FR2_OPFX_BINARY4F|FR2_BOP_LOR):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryLOrF(ctx, &op); break;
	case (FR2_OPFX_BINARY4F|FR2_BOP_CMP_EQ):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryCmpEqF(ctx, &op); break;
	case (FR2_OPFX_BINARY4F|FR2_BOP_CMP_NE):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryCmpNeF(ctx, &op); break;
	case (FR2_OPFX_BINARY4F|FR2_BOP_CMP_LT):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryCmpLtF(ctx, &op); break;
	case (FR2_OPFX_BINARY4F|FR2_BOP_CMP_GT):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryCmpGtF(ctx, &op); break;
	case (FR2_OPFX_BINARY4F|FR2_BOP_CMP_LE):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryCmpLeF(ctx, &op); break;
	case (FR2_OPFX_BINARY4F|FR2_BOP_CMP_GE):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryCmpGeF(ctx, &op); break;
	case (FR2_OPFX_BINARY4F|FR2_BOP_CMP_UN):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryCmpUnF(ctx, &op); break;
	case (FR2_OPFX_BINARY4F|FR2_BOP_CMP_TR):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryCmpTrF(ctx, &op); break;
	case (FR2_OPFX_BINARY4F|FR2_BOP_LOADINDEX):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryLoadIndexF(ctx, &op); break;
	case (FR2_OPFX_BINARY4F|FR2_BOP_STOREINDEX):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryStoreIndexF(ctx, &op); break;

	case (FR2_OPFX_BINARY4D|FR2_BOP_ADD):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryAddD(ctx, &op); break;
	case (FR2_OPFX_BINARY4D|FR2_BOP_SUB):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinarySubD(ctx, &op); break;
	case (FR2_OPFX_BINARY4D|FR2_BOP_MUL):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryMulD(ctx, &op); break;
	case (FR2_OPFX_BINARY4D|FR2_BOP_DIV):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryDivD(ctx, &op); break;
	case (FR2_OPFX_BINARY4D|FR2_BOP_MOD):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryModD(ctx, &op); break;
	case (FR2_OPFX_BINARY4D|FR2_BOP_AND):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryAndD(ctx, &op); break;
	case (FR2_OPFX_BINARY4D|FR2_BOP_OR):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryOrD(ctx, &op); break;
	case (FR2_OPFX_BINARY4D|FR2_BOP_XOR):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryXorD(ctx, &op); break;
	case (FR2_OPFX_BINARY4D|FR2_BOP_SHL):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryShlD(ctx, &op); break;
	case (FR2_OPFX_BINARY4D|FR2_BOP_SHR):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryShrD(ctx, &op); break;
	case (FR2_OPFX_BINARY4D|FR2_BOP_SHRR):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryShrrD(ctx, &op); break;
	case (FR2_OPFX_BINARY4D|FR2_BOP_CMPL):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryCmplD(ctx, &op); break;
	case (FR2_OPFX_BINARY4D|FR2_BOP_CMPG):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryCmpgD(ctx, &op); break;
	case (FR2_OPFX_BINARY4D|FR2_BOP_LAND):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryLAndD(ctx, &op); break;
	case (FR2_OPFX_BINARY4D|FR2_BOP_LOR):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryLOrD(ctx, &op); break;
	case (FR2_OPFX_BINARY4D|FR2_BOP_CMP_EQ):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryCmpEqD(ctx, &op); break;
	case (FR2_OPFX_BINARY4D|FR2_BOP_CMP_NE):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryCmpNeD(ctx, &op); break;
	case (FR2_OPFX_BINARY4D|FR2_BOP_CMP_LT):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryCmpLtD(ctx, &op); break;
	case (FR2_OPFX_BINARY4D|FR2_BOP_CMP_GT):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryCmpGtD(ctx, &op); break;
	case (FR2_OPFX_BINARY4D|FR2_BOP_CMP_LE):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryCmpLeD(ctx, &op); break;
	case (FR2_OPFX_BINARY4D|FR2_BOP_CMP_GE):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryCmpGeD(ctx, &op); break;
	case (FR2_OPFX_BINARY4D|FR2_BOP_CMP_UN):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryCmpUnD(ctx, &op); break;
	case (FR2_OPFX_BINARY4D|FR2_BOP_CMP_TR):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryCmpTrD(ctx, &op); break;
	case (FR2_OPFX_BINARY4D|FR2_BOP_LOADINDEX):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryLoadIndexD(ctx, &op); break;
	case (FR2_OPFX_BINARY4D|FR2_BOP_STOREINDEX):
		i=*ctx->ip++; op.d=(i>>4)&15; op.s=i&15;
		BGBFR2_Step_BinaryStoreIndexD(ctx, &op); break;
#endif

	case (FR2_OPFX_UNARY4IC|FR2_UOP_MOV):
		i=*ctx->ip++; j=*ctx->ip++; op.d=(i>>4)&15;
		k=((i&15)<<8)|j; op.c=(k&0x800)?(k|((-1)<<12)):k;
		BGBFR2_Step_UnaryMovI(ctx, &op); break;
	case (FR2_OPFX_UNARY4IC|FR2_UOP_NEG):
		i=*ctx->ip++; j=*ctx->ip++; op.d=(i>>4)&15;
		k=((i&15)<<8)|j; op.c=(k&0x800)?(k|((-1)<<12)):k;
		BGBFR2_Step_UnaryNegI(ctx, &op); break;
	case (FR2_OPFX_UNARY4IC|FR2_UOP_NOT):
		i=*ctx->ip++; j=*ctx->ip++; op.d=(i>>4)&15;
		k=((i&15)<<8)|j; op.c=(k&0x800)?(k|((-1)<<12)):k;
		BGBFR2_Step_UnaryNotI(ctx, &op); break;
	case (FR2_OPFX_UNARY4IC|FR2_UOP_LNOT):
		i=*ctx->ip++; j=*ctx->ip++; op.d=(i>>4)&15;
		k=((i&15)<<8)|j; op.c=(k&0x800)?(k|((-1)<<12)):k;
		BGBFR2_Step_UnaryLNotI(ctx, &op); break;
	case (FR2_OPFX_UNARY4IC|FR2_UOP_INC):
		i=*ctx->ip++; j=*ctx->ip++; op.d=(i>>4)&15;
		k=((i&15)<<8)|j; op.c=(k&0x800)?(k|((-1)<<12)):k;
		BGBFR2_Step_UnaryIncI(ctx, &op); break;
	case (FR2_OPFX_UNARY4IC|FR2_UOP_DEC):
		i=*ctx->ip++; j=*ctx->ip++; op.d=(i>>4)&15;
		k=((i&15)<<8)|j; op.c=(k&0x800)?(k|((-1)<<12)):k;
		BGBFR2_Step_UnaryDecI(ctx, &op); break;
//	case (FR2_OPFX_UNARY4IC|FR2_UOP_DEFER):
//		i=*ctx->ip++; j=*ctx->ip++; op.d=(i>>4)&15;
//		k=((i&15)<<8)|j; op.c=(k&0x800)?(k|((-1)<<12)):k;
//		BGBFR2_Step_UnaryDeferI(ctx, &op); break;
//	case (FR2_OPFX_UNARY4IC|FR2_UOP_SETDEFER):
//		i=*ctx->ip++; j=*ctx->ip++; op.d=(i>>4)&15;
//		k=((i&15)<<8)|j; op.c=(k&0x800)?(k|((-1)<<12)):k;
//		BGBFR2_Step_UnarySetDeferI(ctx, &op); break;

	case (FR2_OPFX_BINARY4IC| 0):	case (FR2_OPFX_BINARY4IC| 1):
	case (FR2_OPFX_BINARY4IC| 2):	case (FR2_OPFX_BINARY4IC| 3):
	case (FR2_OPFX_BINARY4IC| 4):	case (FR2_OPFX_BINARY4IC| 5):
	case (FR2_OPFX_BINARY4IC| 6):	case (FR2_OPFX_BINARY4IC| 7):
	case (FR2_OPFX_BINARY4IC| 8):	case (FR2_OPFX_BINARY4IC| 9):
	case (FR2_OPFX_BINARY4IC|10):	case (FR2_OPFX_BINARY4IC|11):
	case (FR2_OPFX_BINARY4IC|12):	case (FR2_OPFX_BINARY4IC|13):
	case (FR2_OPFX_BINARY4IC|14):	case (FR2_OPFX_BINARY4IC|15):
		opj=*ctx->ip++; opk=*ctx->ip++; j=*ctx->ip++;
		opn=((opi&15)<<4)|((opj>>4)&15);
		op.d=opj&15; op.s=(opjk>4)&15;
		k=((opk&15)<<8)|j; op.c=(k&0x800)?(k|((-1)<<12)):k;
		switch(opn)
		{
		case FR2_BOP_ADD:    BGBFR2_Step_BinaryAddIC(ctx, &op); break;
		case FR2_BOP_SUB:    BGBFR2_Step_BinarySubIC(ctx, &op); break;
		case FR2_BOP_MUL:    BGBFR2_Step_BinaryMulIC(ctx, &op); break;
		case FR2_BOP_DIV:    BGBFR2_Step_BinaryDivIC(ctx, &op); break;
		case FR2_BOP_MOD:    BGBFR2_Step_BinaryModIC(ctx, &op); break;
		case FR2_BOP_AND:    BGBFR2_Step_BinaryAndIC(ctx, &op); break;
		case FR2_BOP_OR:     BGBFR2_Step_BinaryOrIC(ctx, &op); break;
		case FR2_BOP_XOR:    BGBFR2_Step_BinaryXorIC(ctx, &op); break;
		case FR2_BOP_SHL:    BGBFR2_Step_BinaryShlIC(ctx, &op); break;
		case FR2_BOP_SHR:    BGBFR2_Step_BinaryShrIC(ctx, &op); break;
		case FR2_BOP_SHRR:   BGBFR2_Step_BinaryShrrIC(ctx, &op); break;
		case FR2_BOP_CMPL:   BGBFR2_Step_BinaryCmplIC(ctx, &op); break;
		case FR2_BOP_CMPG:   BGBFR2_Step_BinaryCmplIC(ctx, &op); break;
		case FR2_BOP_LAND:   BGBFR2_Step_BinaryLAndIC(ctx, &op); break;
		case FR2_BOP_LOR:    BGBFR2_Step_BinaryLOrIC(ctx, &op); break;
		case FR2_BOP_LXOR:   BGBFR2_Step_BinaryLXorIC(ctx, &op); break;
		case FR2_BOP_CMP_EQ: BGBFR2_Step_BinaryCmpEqIC(ctx, &op); break;
		case FR2_BOP_CMP_NE: BGBFR2_Step_BinaryCmpNeIC(ctx, &op); break;
		case FR2_BOP_CMP_LT: BGBFR2_Step_BinaryCmpLtIC(ctx, &op); break;
		case FR2_BOP_CMP_GT: BGBFR2_Step_BinaryCmpGtIC(ctx, &op); break;
		case FR2_BOP_CMP_LE: BGBFR2_Step_BinaryCmpLeIC(ctx, &op); break;
		case FR2_BOP_CMP_GE: BGBFR2_Step_BinaryCmpGeIC(ctx, &op); break;
//		case FR2_BOP_CMP_UN: BGBFR2_Step_BinaryCmpUnIC(ctx, &op); break;
//		case FR2_BOP_CMP_TR: BGBFR2_Step_BinaryCmpTrIC(ctx, &op); break;
		case FR2_BOP_LOADINDEX:
			BGBFR2_Step_BinaryLoadIndexIC(ctx, &op); break;
		case FR2_BOP_STOREINDEX:
			BGBFR2_Step_BinaryStoreIndexIC(ctx, &op); break;
		default: rs=FR2_RS_INVALIDOPCODE; break;
		}
		break;

	case (FR2_OPFX_UJMP4|FR2_PFX_Z):
		k=(ctx->ip[0]<<16)|(ctx->ip[1]<<8)|(ctx->ip[2]);
		k=(k&0x800000)?(k|((-1)<<24)):k;
		ctx->ip+=k+3;
		break;
	case (FR2_OPFX_UJMP4|FR2_PFX_I):
		opj=*ctx->ip++;	opk=*ctx->ip++; opl=*ctx->ip++;
		opn=(opj>>4)&15; op.s=opj&15;
		switch(opn)
		{
		case FR2_JOP_JMP1_EQ: j=(ctx->regs[op.s].i==0); break;
		case FR2_JOP_JMP1_NE: j=(ctx->regs[op.s].i!=0); break;
		case FR2_JOP_JMP1_LT: j=(ctx->regs[op.s].i<0); break;
		case FR2_JOP_JMP1_GT: j=(ctx->regs[op.s].i>0); break;
		case FR2_JOP_JMP1_LE: j=(ctx->regs[op.s].i<=0); break;
		case FR2_JOP_JMP1_GE: j=(ctx->regs[op.s].i>=0); break;
		default: rs=FR2_RS_INVALIDOPCODE; j=0; break;
		}
		if(j)
		{
			k=(opk<<8)|(opl);
			k=(k&0x800000)?(k|((-1)<<24)):k;
			ctx->ip+=k;
		}
		break;
	case (FR2_OPFX_UJMP4|FR2_PFX_L):
		opj=*ctx->ip++;	opk=*ctx->ip++; opl=*ctx->ip++;
		opn=(opj>>4)&15; op.s=opj&15;
		switch(opn)
		{
		case FR2_JOP_JMP1_EQ: j=(ctx->regs[op.s].l==0); break;
		case FR2_JOP_JMP1_NE: j=(ctx->regs[op.s].l!=0); break;
		case FR2_JOP_JMP1_LT: j=(ctx->regs[op.s].l<0); break;
		case FR2_JOP_JMP1_GT: j=(ctx->regs[op.s].l>0); break;
		case FR2_JOP_JMP1_LE: j=(ctx->regs[op.s].l<=0); break;
		case FR2_JOP_JMP1_GE: j=(ctx->regs[op.s].l>=0); break;
		default: rs=FR2_RS_INVALIDOPCODE; j=0; break;
		}
		if(j)
		{
			k=(opk<<8)|(opl);
			k=(k&0x800000)?(k|((-1)<<24)):k;
			ctx->ip+=k;
		}
		break;
	case (FR2_OPFX_UJMP4|FR2_PFX_F):
		opj=*ctx->ip++;	opk=*ctx->ip++; opl=*ctx->ip++;
		opn=(opj>>4)&15; op.s=opj&15;
		switch(opn)
		{
		case FR2_JOP_JMP1_EQ: j=(ctx->regs[op.s].f==0); break;
		case FR2_JOP_JMP1_NE: j=(ctx->regs[op.s].f!=0); break;
		case FR2_JOP_JMP1_LT: j=(ctx->regs[op.s].f<0); break;
		case FR2_JOP_JMP1_GT: j=(ctx->regs[op.s].f>0); break;
		case FR2_JOP_JMP1_LE: j=(ctx->regs[op.s].f<=0); break;
		case FR2_JOP_JMP1_GE: j=(ctx->regs[op.s].f>=0); break;
		default: rs=FR2_RS_INVALIDOPCODE; j=0; break;
		}
		if(j)
		{
			k=(opk<<8)|(opl);
			k=(k&0x800000)?(k|((-1)<<24)):k;
			ctx->ip+=k;
		}
		break;
	case (FR2_OPFX_UJMP4|FR2_PFX_D):
		opj=*ctx->ip++;	opk=*ctx->ip++; opl=*ctx->ip++;
		opn=(opj>>4)&15; op.s=opj&15;
		switch(opn)
		{
		case FR2_JOP_JMP1_EQ: j=(ctx->regs[op.s].d==0); break;
		case FR2_JOP_JMP1_NE: j=(ctx->regs[op.s].d!=0); break;
		case FR2_JOP_JMP1_LT: j=(ctx->regs[op.s].d<0); break;
		case FR2_JOP_JMP1_GT: j=(ctx->regs[op.s].d>0); break;
		case FR2_JOP_JMP1_LE: j=(ctx->regs[op.s].d<=0); break;
		case FR2_JOP_JMP1_GE: j=(ctx->regs[op.s].d>=0); break;
		default: rs=FR2_RS_INVALIDOPCODE; j=0; break;
		}
		if(j)
		{
			k=(opk<<8)|(opl);
			k=(k&0x800000)?(k|((-1)<<24)):k;
			ctx->ip+=k;
		}
		break;
	case (FR2_OPFX_UJMP4|FR2_PFX_P):
		opj=*ctx->ip++;	opk=*ctx->ip++; opl=*ctx->ip++;
		opn=(opj>>4)&15; op.s=opj&15;
		switch(opn)
		{
		case FR2_JOP_JMP1_EQ: j=(ctx->regs[op.s].p==NULL); break;
		case FR2_JOP_JMP1_NE: j=(ctx->regs[op.s].p!=NULL); break;
		default: rs=FR2_RS_INVALIDOPCODE; j=0; break;
		}
		if(j)
		{
			k=(opk<<8)|(opl);
			k=(k&0x800000)?(k|((-1)<<24)):k;
			ctx->ip+=k;
		}
		break;
	case (FR2_OPFX_UJMP4|FR2_PFX_A):
		opj=*ctx->ip++;	opk=*ctx->ip++; opl=*ctx->ip++;
		opn=(opj>>4)&15; op.s=opj&15;
		switch(opn)
		{
		case FR2_JOP_JMP1_EQ: j=(ctx->regs[op.s].p==NULL); break;
		case FR2_JOP_JMP1_NE: j=(ctx->regs[op.s].p!=NULL); break;
		default: rs=FR2_RS_INVALIDOPCODE; j=0; break;
		}
		if(j)
		{
			k=(opk<<8)|(opl);
			k=(k&0x800000)?(k|((-1)<<24)):k;
			ctx->ip+=k;
		}
		break;

	case (FR2_OPFX_ESCG1| 0):	case (FR2_OPFX_ESCG1| 1):
	case (FR2_OPFX_ESCG1| 2):	case (FR2_OPFX_ESCG1| 3):
	case (FR2_OPFX_ESCG1| 4):	case (FR2_OPFX_ESCG1| 5):
	case (FR2_OPFX_ESCG1| 6):	case (FR2_OPFX_ESCG1| 7):
	case (FR2_OPFX_ESCG1| 8):	case (FR2_OPFX_ESCG1| 9):
	case (FR2_OPFX_ESCG1|10):	case (FR2_OPFX_ESCG1|11):
	case (FR2_OPFX_ESCG1|12):	case (FR2_OPFX_ESCG1|13):
	case (FR2_OPFX_ESCG1|14):	case (FR2_OPFX_ESCG1|15):
		pfx=opi&15;
		opj=*ctx->ip++;
		rs=BGBFR2_Interp_StepGeneric(ctx, pfx, opj);
		break;
	case (FR2_OPFX_ESCG2| 0):	case (FR2_OPFX_ESCG2| 1):
	case (FR2_OPFX_ESCG2| 2):	case (FR2_OPFX_ESCG2| 3):
	case (FR2_OPFX_ESCG2| 4):	case (FR2_OPFX_ESCG2| 5):
	case (FR2_OPFX_ESCG2| 6):	case (FR2_OPFX_ESCG2| 7):
	case (FR2_OPFX_ESCG2| 8):	case (FR2_OPFX_ESCG2| 9):
	case (FR2_OPFX_ESCG2|10):	case (FR2_OPFX_ESCG2|11):
	case (FR2_OPFX_ESCG2|12):	case (FR2_OPFX_ESCG2|13):
	case (FR2_OPFX_ESCG2|14):	case (FR2_OPFX_ESCG2|15):
		opj=*ctx->ip++;	opk=*ctx->ip++;
		pfx=((opi&15)<<4)|((opj>>4)&15);
		i=((opj&15)<<8)|opk;
		rs=BGBFR2_Interp_StepGeneric(ctx, pfx, i);
		break;
	default:
		rs=FR2_RS_INVALIDOPCODE;
		break;
	}
	return(rs);
}

int BGBFR2_Interp_RunFrame(FR2_RunFrame *ctx)
{
	int rs;
	rs=0;
	while(!rs)
		{ rs=BGBFR2_Interp_Step(ctx); }
	return(rs);
}

int BGBFR2_Interp_RunContext(FR2_RunCtx *ctx, FR2_Value *rtreg)
{
	FR2_RunFrame *ftmp;
	int rs;

	if(!ctx->frame)
		{ return(FR2_RS_NOFRAME); }

	ctx->frame->rtreg=rtreg;

	while(1)
	{
		rs=BGBFR2_Interp_RunFrame(ctx->frame);
		if(!rs)continue;
		if(rs==FR2_RS_CALL)
		{
			rs=0;
			continue;
		}
		if(rs==FR2_RS_RETURN)
		{
			if(ctx->frame->next)
			{
				ftmp=ctx->frame;
				ctx->frame=ftmp->next;
				BGBFR2_RunCtx_FreeFrame(ctx, ftmp);
				rs=0;
				continue;
			}
			break;
		}
		if(rs<0)break;
	}
	return(rs);
}
