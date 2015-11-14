#include <frbc2ci.h>

#define OP_SWNI(OPN, YYN) \
	case FR2C_OPMODE_##YYN: tmp->run=FR2CI_Op_##OPN##_##YYN;

#define OPB_SWNI(OPN) \
	switch(y) {	\
		OP_SWNI(OPN, RR)	\
		OP_SWNI(OPN, RA)	\
		OP_SWNI(OPN, RL)	\
		OP_SWNI(OPN, AR)	\
		OP_SWNI(OPN, AA)	\
		OP_SWNI(OPN, AL)	\
		OP_SWNI(OPN, LR)	\
		OP_SWNI(OPN, LA)	\
		OP_SWNI(OPN, LL)	}

#define OP_SWNW(OPN, YYN) \
	case FR2C_OPMODE2_##YYN: tmp->run=FR2CI_Op_##OPN##_##YYN;

#define OPB_SWNW(OPN) \
	switch(w) {	\
		OP_SWNW(OPN, IR)	\
		OP_SWNW(OPN, IA)	\
		OP_SWNW(OPN, IL)	\
		OP_SWNW(OPN, LR)	\
		OP_SWNW(OPN, LA)	\
		OP_SWNW(OPN, LL)	\
		OP_SWNW(OPN, FR)	\
		OP_SWNW(OPN, FA)	\
		OP_SWNW(OPN, FL)	\
		OP_SWNW(OPN, DR)	\
		OP_SWNW(OPN, DA)	\
		OP_SWNW(OPN, DL)	\
		OP_SWNW(OPN, PR)	\
		OP_SWNW(OPN, PA)	\
		OP_SWNW(OPN, PL)	}

#define OP_MVVAR(OPN, YYN, DN, SN, TY) \
	void FR2CI_Op_##OPN##_##YYN(FR2CI_Frame *frm, FR2CI_Opcode *op) \
		{ DN[op->d].TY=SN[op->s].TY; }

#define OPB_LDVAR(OPN, SN) \
	OP_MVVAR(OPN, IR, frm->reg, SN, i) \
	OP_MVVAR(OPN, IA, frm->arg, SN, i) \
	OP_MVVAR(OPN, IL, frm->loc, SN, i) \
	OP_MVVAR(OPN, LR, frm->reg, SN, l) \
	OP_MVVAR(OPN, LA, frm->arg, SN, l) \
	OP_MVVAR(OPN, LL, frm->loc, SN, l) \
	OP_MVVAR(OPN, FR, frm->reg, SN, f) \
	OP_MVVAR(OPN, FA, frm->arg, SN, f) \
	OP_MVVAR(OPN, FL, frm->loc, SN, f) \
	OP_MVVAR(OPN, DR, frm->reg, SN, d) \
	OP_MVVAR(OPN, DA, frm->arg, SN, d) \
	OP_MVVAR(OPN, DL, frm->loc, SN, d) \
	OP_MVVAR(OPN, PR, frm->reg, SN, p) \
	OP_MVVAR(OPN, PA, frm->arg, SN, p) \
	OP_MVVAR(OPN, PL, frm->loc, SN, p)

OPB_LDVAR(LDLVar, frm->loc)
OPB_LDVAR(LDGVar, ((FR2CI_Value *)op->imm.p))
OPB_LDVAR(LDAGVar, (nlint)&((FR2CI_Value *)op->imm.p))

#define OPB_STVAR(OPN, DN) \
	OP_MVVAR(OPN, IR, DN, frm->reg, i) \
	OP_MVVAR(OPN, IA, DN, frm->arg, i) \
	OP_MVVAR(OPN, IL, DN, frm->loc, i) \
	OP_MVVAR(OPN, LR, DN, frm->reg, l) \
	OP_MVVAR(OPN, LA, DN, frm->arg, l) \
	OP_MVVAR(OPN, LL, DN, frm->loc, l) \
	OP_MVVAR(OPN, FR, DN, frm->reg, f) \
	OP_MVVAR(OPN, FA, DN, frm->arg, f) \
	OP_MVVAR(OPN, FL, DN, frm->loc, f) \
	OP_MVVAR(OPN, DR, DN, frm->reg, d) \
	OP_MVVAR(OPN, DA, DN, frm->arg, d) \
	OP_MVVAR(OPN, DL, DN, frm->loc, d) \
	OP_MVVAR(OPN, PR, DN, frm->reg, p) \
	OP_MVVAR(OPN, PA, DN, frm->arg, p) \
	OP_MVVAR(OPN, PL, DN, frm->loc, p)

OPB_STVAR(STLVar, frm->loc)
OPB_STVAR(STGVar, ((FR2CI_Value *)op->imm.p))

FR2CI_Opcode *FR2CI_MakeOpLDVar(
	FR2CI_Image *img, int op,
	int w, int d, int idx)
{
	FR2CI_Opcode *tmp;
	
	tmp=FR2CI_AllocOpcode(img);
	tmp->d=d;
	tmp->s=idx;
	tmp->imm.p=img->gbl;

	switch(op)
	{
	case FR2C_OP_LDLVAR:
		tmp->d=d;
		tmp->s=idx;
		OPB_SWNW(LDLVar)
		break;
	case FR2C_OP_STLVAR:
		tmp->d=idx;
		tmp->s=d;
		OPB_SWNW(STLVar)
		break;
	case FR2C_OP_LDGVAR:
		tmp->d=d;
		tmp->s=idx;
		OPB_SWNW(LDGVar)
		break;
	case FR2C_OP_STGVAR:
		tmp->d=idx;
		tmp->s=d;
		OPB_SWNW(STGVar)
		break;
	case FR2C_OP_LDAGVAR:
		tmp->d=d;
		tmp->s=idx;
		OPB_SWNW(LDAGVar)
		break;
	}

	if(!tmp->run)
	{
		FR2CI_FreeOpcode(img, tmp);
		return(NULL);
	}
	
	return(tmp);
}

#define OP_NKSHR(OPN, YYN, DN, SN, OP, TY) \
	void FR2CI_Op_##OPN##_##YYN(FR2CI_Frame *frm, FR2CI_Opcode *op) \
		{ frm->DN[op->d].TY=(frm->SN[op->s].TY OP op->t)&(op->imm.TY); }

#define OPB_NKSHR(OPN, OP, TY) \
	OP_NKSHR(OPN, RR, reg, reg, OP, TY) \
	OP_NKSHR(OPN, RA, reg, arg, OP, TY) \
	OP_NKSHR(OPN, RL, reg, loc, OP, TY) \
	OP_NKSHR(OPN, AR, arg, reg, OP, TY) \
	OP_NKSHR(OPN, AA, arg, arg, OP, TY) \
	OP_NKSHR(OPN, AL, arg, loc, OP, TY) \
	OP_NKSHR(OPN, LR, loc, reg, OP, TY) \
	OP_NKSHR(OPN, LA, loc, arg, OP, TY) \
	OP_NKSHR(OPN, LL, loc, loc, OP, TY) \

OPB_NKSHR(ShrMaskI, >>, i);
OPB_NKSHR(ShrMaskL, >>, l);
OPB_NKSHR(ShlMaskI, <<, i);
OPB_NKSHR(ShlMaskL, <<, l);

FR2CI_Opcode*FR2CI_MakeOpShrMask(
	FR2CI_Image *img, int op,
	int y, int d, int s, int shr, int msh)
{
	FR2CI_Opcode *tmp;
	
	tmp=FR2CI_AllocOpcode(img);
	tmp->d=d;
	tmp->s=s;
	tmp->t=shr;
	tmp->imm.l=(1LL<<msh)-1;

	switch(op)
	{
	case FR2C_OP_SHRMASK_I:
		tmp->imm.i=tmp->imm.l;
		OPB_SWNI(ShrMaskI)
		break;
	case FR2C_OP_SHRMASK_L:
		tmp->imm.l=tmp->imm.l;
		OPB_SWNI(ShrMaskL)
		break;
	case FR2C_OP_SHLMASK_I:
		tmp->imm.i=tmp->imm.l<<shr;
		OPB_SWNI(ShlMaskI)
		break;
	case FR2C_OP_SHLMASK_L:
		tmp->imm.l=tmp->imm.l<<shr;
		OPB_SWNI(ShlMaskL)
		break;
	}

	if(!tmp->run)
	{
		FR2CI_FreeOpcode(img, tmp);
		return(NULL);
	}
	
	return(tmp);
}

void FR2CI_Op_Nop(FR2CI_Frame *frm, FR2CI_Opcode *op)
	{ }

FR2CI_Opcode *FR2CI_MakeOpNop(FR2CI_Image *img)
{
	FR2CI_Opcode *tmp;
	
	tmp=FR2CI_AllocOpcode(img);
	tmp->run=FR2CI_Op_Nop;
	return(tmp);
}


void FR2CI_Op_InitArr_ClopG(FR2CI_Frame *frm, FR2CI_CleanupOpcode *op)
{
	FR2CI_ContextPoolFree(
		frm->ctx, frm->loc[op->d].p, op->t);
}

void FR2CI_Op_InitArrG(FR2CI_Frame *frm, FR2CI_Opcode *op)
{
	FR2CI_CleanupOpcode *clop;
	frm->loc[op->d].p=FR2CI_ContextPoolAlloc(
		frm->ctx, op->t);
	clop=FR2CI_FrameNewCleanupOp(frm->ctx, frm);
	clop->d=op->d; clop->t=op->t;
	clop->run=FR2CI_Op_InitArr_ClopG;
}

FR2CI_Opcode *FR2CI_MakeOpInitArr(FR2CI_Image *img,
	int z, int d, int c)
{
	FR2CI_Opcode *tmp;
	int csz;
	
	switch(z&15)
	{
	case FR2C_TY_I: csz=sizeof(s32); break;
	case FR2C_TY_L: csz=sizeof(s64); break;
	case FR2C_TY_F: csz=sizeof(f32); break;
	case FR2C_TY_D: csz=sizeof(f64); break;
	case FR2C_TY_P: csz=sizeof(void *); break;
	case FR2C_TY_NL: csz=sizeof(long); break;

	case FR2C_TY_SB: csz=sizeof(byte); break;
	case FR2C_TY_UB: csz=sizeof(byte); break;
	case FR2C_TY_SS: csz=sizeof(s16); break;
	case FR2C_TY_US: csz=sizeof(u16); break;
	case FR2C_TY_UI: csz=sizeof(u32); break;
	case FR2C_TY_UL: csz=sizeof(u64); break;
	case FR2C_TY_UNL: csz=sizeof(long); break;
	default:
		csz=8; break;
	}
	
	tmp=FR2CI_AllocOpcode(img);
	tmp->d=d; tmp->t=c*csz;
	tmp->run=FR2CI_Op_InitArrG;
	return(tmp);
}


void FR2CI_Op_InitObj_ClopGR(FR2CI_Frame *frm, FR2CI_CleanupOpcode *op)
{
	FR2CI_ContextPoolFree(
		frm->ctx, frm->reg[op->d].p, op->t);
}

void FR2CI_Op_InitObj_ClopGA(FR2CI_Frame *frm, FR2CI_CleanupOpcode *op)
{
	FR2CI_ContextPoolFree(
		frm->ctx, frm->arg[op->d].p, op->t);
}

void FR2CI_Op_InitObj_ClopGL(FR2CI_Frame *frm, FR2CI_CleanupOpcode *op)
{
	FR2CI_ContextPoolFree(
		frm->ctx, frm->loc[op->d].p, op->t);
}

void FR2CI_Op_InitObjGR(FR2CI_Frame *frm, FR2CI_Opcode *op)
{
//	FR2CI_CleanupOpcode *clop;
	frm->reg[op->d].p=FR2CI_ContextPoolAlloc(
		frm->ctx, op->t);
//	clop=FR2CI_FrameNewCleanupOp(frm->ctx, frm);
//	clop->d=op->d; clop->t=op->t;
//	clop->run=FR2CI_Op_InitArr_ClopG;
}

void FR2CI_Op_InitObjGA(FR2CI_Frame *frm, FR2CI_Opcode *op)
{
//	FR2CI_CleanupOpcode *clop;
//	frm->loc[op->d].p=FR2CI_ContextPoolAlloc(
//		frm->ctx, op->t);
//	clop=FR2CI_FrameNewCleanupOp(frm->ctx, frm);
//	clop->d=op->d; clop->t=op->t;
//	clop->run=FR2CI_Op_InitArr_ClopG;
}

void FR2CI_Op_InitObjGL(FR2CI_Frame *frm, FR2CI_Opcode *op)
{
	FR2CI_CleanupOpcode *clop;
	frm->loc[op->d].p=FR2CI_ContextPoolAlloc(
		frm->ctx, op->t);
	clop=FR2CI_FrameNewCleanupOp(frm->ctx, frm);
	clop->d=op->d; clop->t=op->t;
	clop->run=FR2CI_Op_InitObj_ClopGL;
}

FR2CI_Opcode *FR2CI_MakeOpInitObj(FR2CI_Image *img,
	int w, int d, int c)
{
	FR2CI_LiteralInfo *str;
	FR2CI_Opcode *tmp;
	int csz;

	str=FR2CI_DecodeLiteralTableEntry(img, c);
	if(!str)
		return(NULL);
	if(!str->field)
		return(NULL);

	tmp=FR2CI_AllocOpcode(img);
	tmp->d=d;
	tmp->t=str->st_size;

	switch(w&15)
	{
	case FR2C_OPMODE2_IR:
	case FR2C_OPMODE2_PR:
		tmp->run=FR2CI_Op_InitObjGR;
		break;
	case FR2C_OPMODE2_IA:
	case FR2C_OPMODE2_PA:
		tmp->run=FR2CI_Op_InitObjGA;
		break;
	case FR2C_OPMODE2_IL:
	case FR2C_OPMODE2_PL:
		tmp->run=FR2CI_Op_InitObjGL;
		break;
	default:
		break;
	}

	if(!tmp->run)
	{
		FR2CI_FreeOpcode(img, tmp);
		return(NULL);
	}

	return(tmp);
}

void FR2CI_Op_DropObjGR(FR2CI_Frame *frm, FR2CI_Opcode *op)
{
	FR2CI_ContextPoolFree(
		frm->ctx, frm->reg[op->d].p, op->t);
}

void FR2CI_Op_DropObjGA(FR2CI_Frame *frm, FR2CI_Opcode *op)
{
	FR2CI_ContextPoolFree(
		frm->ctx, frm->arg[op->d].p, op->t);
}

void FR2CI_Op_DropObjGL(FR2CI_Frame *frm, FR2CI_Opcode *op)
{
	FR2CI_ContextPoolFree(
		frm->ctx, frm->loc[op->d].p, op->t);
}

FR2CI_Opcode *FR2CI_MakeOpDropObj(FR2CI_Image *img,
	int w, int d, int c)
{
	FR2CI_LiteralInfo *str;
	FR2CI_Opcode *tmp;
	int csz;

	str=FR2CI_DecodeLiteralTableEntry(img, c);
	if(!str)
		return(NULL);
	if(!str->field)
		return(NULL);

	tmp=FR2CI_AllocOpcode(img);
	tmp->d=d;
	tmp->t=str->st_size;

	switch(w&15)
	{
	case FR2C_OPMODE2_IR:
	case FR2C_OPMODE2_PR:
		tmp->run=FR2CI_Op_DropObjGR;
		break;
	case FR2C_OPMODE2_IA:
	case FR2C_OPMODE2_PA:
		tmp->run=FR2CI_Op_DropObjGA;
		break;
	case FR2C_OPMODE2_IL:
	case FR2C_OPMODE2_PL:
		tmp->run=FR2CI_Op_DropObjGL;
		break;
	default:
		break;
	}

	if(!tmp->run)
	{
		FR2CI_FreeOpcode(img, tmp);
		return(NULL);
	}

	return(tmp);
}


void FR2CI_Op_SizeofGR(FR2CI_Frame *frm, FR2CI_Opcode *op)
	{ frm->reg[op->d].i=op->t; }
void FR2CI_Op_SizeofGA(FR2CI_Frame *frm, FR2CI_Opcode *op)
	{ frm->arg[op->d].i=op->t; }
void FR2CI_Op_SizeofGL(FR2CI_Frame *frm, FR2CI_Opcode *op)
	{ frm->loc[op->d].i=op->t; }

FR2CI_Opcode *FR2CI_MakeOpSizeOf(FR2CI_Image *img,
	int y, int d, int c)
{
	FR2CI_LiteralInfo *str;
	FR2CI_Opcode *tmp;
	int csz;

	str=FR2CI_DecodeLiteralTableEntry(img, c);
	if(!str)
		return(NULL);
//	if(!str->field)
//		return(NULL);

	tmp=FR2CI_AllocOpcode(img);
	tmp->d=d;
	tmp->t=str->st_size;

	switch(y&15)
	{
	case FR2C_OPMODE2_IR:
	case FR2C_OPMODE2_PR:
		tmp->run=FR2CI_Op_SizeofGR;
		break;
	case FR2C_OPMODE2_IA:
	case FR2C_OPMODE2_PA:
		tmp->run=FR2CI_Op_SizeofGA;
		break;
	case FR2C_OPMODE2_IL:
	case FR2C_OPMODE2_PL:
		tmp->run=FR2CI_Op_SizeofGL;
		break;
	default:
		break;
	}

	if(!tmp->run)
	{
		FR2CI_FreeOpcode(img, tmp);
		return(NULL);
	}

	return(tmp);
}

FR2CI_Opcode *FR2CI_MakeOpOffsetOf(FR2CI_Image *img,
	int y, int d, int c, int idx)
{
	FR2CI_LiteralInfo *str;
	FR2CI_Opcode *tmp;
	int offs;

	str=FR2CI_DecodeLiteralTableEntry(img, c);
	if(!str)
		return(NULL);
	if(!str->field)
		return(NULL);
	if((idx<0) || (idx>=str->n_field))
		return(NULL);
	offs=str->field[idx]->vi_offset;
	
	tmp=FR2CI_AllocOpcode(img);
	tmp->d=d;
	tmp->t=offs;

	switch(y&15)
	{
	case FR2C_OPMODE2_IR:
	case FR2C_OPMODE2_PR:
		tmp->run=FR2CI_Op_SizeofGR;
		break;
	case FR2C_OPMODE2_IA:
	case FR2C_OPMODE2_PA:
		tmp->run=FR2CI_Op_SizeofGA;
		break;
	case FR2C_OPMODE2_IL:
	case FR2C_OPMODE2_PL:
		tmp->run=FR2CI_Op_SizeofGL;
		break;
	default:
		break;
	}

	if(!tmp->run)
	{
		FR2CI_FreeOpcode(img, tmp);
		return(NULL);
	}

	return(tmp);
}
