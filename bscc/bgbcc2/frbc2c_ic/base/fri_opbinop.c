#include <frbc2ci.h>

#define OP_SWNI(OPN, YYN) \
	case FR2C_OPMODE_##YYN: tmp->run=FR2CI_Op_##OPN##_##YYN;

#define OPB_SWNI(OPN) \
	switch(y) {	\
		OP_SWNI(OPN, RRR)	\
		OP_SWNI(OPN, RRA)	\
		OP_SWNI(OPN, RRL)	\
		OP_SWNI(OPN, RAR)	\
		OP_SWNI(OPN, RAA)	\
		OP_SWNI(OPN, RAL)	\
		OP_SWNI(OPN, RLR)	\
		OP_SWNI(OPN, RLA)	\
		OP_SWNI(OPN, RLL)	\
		OP_SWNI(OPN, ARR)	\
		OP_SWNI(OPN, LRR)	\
		OP_SWNI(OPN, LRA)	\
		OP_SWNI(OPN, LAR)	\
		OP_SWNI(OPN, LRL)	\
		OP_SWNI(OPN, LLR)	\
		OP_SWNI(OPN, LLL)	}

#define OP_NI(OPN, YYN, DN, SN, TN, OP, TY) \
	void FR2CI_Op_##OPN##_##YYN(FR2CI_Frame *frm, FR2CI_Opcode *op) \
		{ frm->DN[op->d].TY=frm->SN[op->s].TY OP frm->TN[op->t].TY; }

#define OPB_NI(OPN, OP, TY) \
	OP_NI(OPN, RRR, reg, reg, reg, OP, TY) \
	OP_NI(OPN, RRA, reg, reg, arg, OP, TY) \
	OP_NI(OPN, RRL, reg, reg, loc, OP, TY) \
	OP_NI(OPN, RAR, reg, arg, reg, OP, TY) \
	OP_NI(OPN, RAA, reg, arg, arg, OP, TY) \
	OP_NI(OPN, RAL, reg, arg, loc, OP, TY) \
	OP_NI(OPN, RLR, reg, loc, reg, OP, TY) \
	OP_NI(OPN, RLA, reg, loc, arg, OP, TY) \
	OP_NI(OPN, RLL, reg, loc, loc, OP, TY) \
	OP_NI(OPN, ARR, arg, reg, reg, OP, TY) \
	OP_NI(OPN, LRR, loc, reg, reg, OP, TY) \
	OP_NI(OPN, LRA, loc, reg, arg, OP, TY) \
	OP_NI(OPN, LAR, loc, arg, reg, OP, TY) \
	OP_NI(OPN, LRL, loc, reg, loc, OP, TY) \
	OP_NI(OPN, LLR, loc, loc, reg, OP, TY) \
	OP_NI(OPN, LLL, loc, loc, loc, OP, TY)


#define OP_NJ(OPN, YYN, DN, SN, TN, OP, DTY, STY, TTY) \
	void FR2CI_Op_##OPN##_##YYN(FR2CI_Frame *frm, FR2CI_Opcode *op) \
		{ frm->DN[op->d].DTY=frm->SN[op->s].STY OP frm->TN[op->t].TTY; }

#define OPB_NJ(OPN, OP, DTY, STY, TTY) \
	OP_NJ(OPN, RRR, reg, reg, reg, OP, DTY, STY, TTY) \
	OP_NJ(OPN, RRA, reg, reg, arg, OP, DTY, STY, TTY) \
	OP_NJ(OPN, RRL, reg, reg, loc, OP, DTY, STY, TTY) \
	OP_NJ(OPN, RAR, reg, arg, reg, OP, DTY, STY, TTY) \
	OP_NJ(OPN, RAA, reg, arg, arg, OP, DTY, STY, TTY) \
	OP_NJ(OPN, RAL, reg, arg, loc, OP, DTY, STY, TTY) \
	OP_NJ(OPN, RLR, reg, loc, reg, OP, DTY, STY, TTY) \
	OP_NJ(OPN, RLA, reg, loc, arg, OP, DTY, STY, TTY) \
	OP_NJ(OPN, RLL, reg, loc, loc, OP, DTY, STY, TTY) \
	OP_NJ(OPN, ARR, arg, reg, reg, OP, DTY, STY, TTY) \
	OP_NJ(OPN, LRR, loc, reg, reg, OP, DTY, STY, TTY) \
	OP_NJ(OPN, LRA, loc, reg, arg, OP, DTY, STY, TTY) \
	OP_NJ(OPN, LAR, loc, arg, reg, OP, DTY, STY, TTY) \
	OP_NJ(OPN, LRL, loc, reg, loc, OP, DTY, STY, TTY) \
	OP_NJ(OPN, LLR, loc, loc, reg, OP, DTY, STY, TTY) \
	OP_NJ(OPN, LLL, loc, loc, loc, OP, DTY, STY, TTY)

OPB_NI(AddI, +, i)
OPB_NI(SubI, -, i)
OPB_NI(MulI, *, i)
OPB_NI(DivI, /, i)
OPB_NI(ModI, %, i)
OPB_NI(AndI, &, i)
OPB_NI(OrI, |, i)
OPB_NI(XorI, ^, i)
OPB_NJ(ShlI, <<, i, i, i)
OPB_NJ(ShrI, >>, i, i, i)
OPB_NJ(ShrrI, >>, i, ui, i)

OPB_NI(AddL, +, l)
OPB_NI(SubL, -, l)
OPB_NI(MulL, *, l)
OPB_NI(DivL, /, l)
OPB_NI(ModL, %, l)
OPB_NI(AndL, &, l)
OPB_NI(OrL, |, l)
OPB_NI(XorL, ^, l)
OPB_NJ(ShlL, <<, l, l, i)
OPB_NJ(ShrL, >>, l, l, i)
OPB_NJ(ShrrL, >>, l, ul, i)

OPB_NI(AddF, +, f)
OPB_NI(SubF, -, f)
OPB_NI(MulF, *, f)
OPB_NI(DivF, /, f)

OPB_NI(AddD, +, d)
OPB_NI(SubD, -, d)
OPB_NI(MulD, *, d)
OPB_NI(DivD, /, d)



FR2CI_Opcode *FR2CI_MakeBinOp(
	FR2CI_Image *img,
	int z, int y, int op,
	int d, int s, int t)
{
	FR2CI_Opcode *tmp;
	
	tmp=FR2CI_AllocOpcode(img);
	tmp->d=d;
	tmp->s=s;
	tmp->t=t;
	
	switch(z)
	{
	case FR2C_TY_I:
		switch(op)
		{
		case FR2C_BINOP_ADD:
			OPB_SWNI(AddI)
			break;
		case FR2C_BINOP_SUB:
			OPB_SWNI(SubI)
			break;
		case FR2C_BINOP_MUL:
			OPB_SWNI(MulI)
			break;
		case FR2C_BINOP_DIV:
			OPB_SWNI(DivI)
			break;
		case FR2C_BINOP_MOD:
			OPB_SWNI(ModI)
			break;
		case FR2C_BINOP_AND:
			OPB_SWNI(AndI)
			break;
		case FR2C_BINOP_OR:
			OPB_SWNI(OrI)
			break;
		case FR2C_BINOP_XOR:
			OPB_SWNI(XorI)
			break;
		case FR2C_BINOP_SHL:
			OPB_SWNI(ShlI)
			break;
		case FR2C_BINOP_SHR:
			OPB_SWNI(ShrI)
			break;
		case FR2C_BINOP_SHRR:
			OPB_SWNI(ShrrI)
			break;
		}
		break;
		
	case FR2C_TY_L:
		switch(op)
		{
		case FR2C_BINOP_ADD:
			OPB_SWNI(AddL)
			break;
		case FR2C_BINOP_SUB:
			OPB_SWNI(SubL)
			break;
		case FR2C_BINOP_MUL:
			OPB_SWNI(MulL)
			break;
		case FR2C_BINOP_DIV:
			OPB_SWNI(DivL)
			break;
		case FR2C_BINOP_MOD:
			OPB_SWNI(ModL)
			break;
		case FR2C_BINOP_AND:
			OPB_SWNI(AndL)
			break;
		case FR2C_BINOP_OR:
			OPB_SWNI(OrL)
			break;
		case FR2C_BINOP_XOR:
			OPB_SWNI(XorL)
			break;
		case FR2C_BINOP_SHL:
			OPB_SWNI(ShlL)
			break;
		case FR2C_BINOP_SHR:
			OPB_SWNI(ShrL)
			break;
		case FR2C_BINOP_SHRR:
			OPB_SWNI(ShrrL)
			break;
		}
		break;

	case FR2C_TY_F:
		switch(op)
		{
		case FR2C_BINOP_ADD:
			OPB_SWNI(AddF)
			break;
		case FR2C_BINOP_SUB:
			OPB_SWNI(SubF)
			break;
		case FR2C_BINOP_MUL:
			OPB_SWNI(MulF)
			break;
		case FR2C_BINOP_DIV:
			OPB_SWNI(DivF)
			break;
		}
		break;

	case FR2C_TY_D:
		switch(op)
		{
		case FR2C_BINOP_ADD:
			OPB_SWNI(AddD)
			break;
		case FR2C_BINOP_SUB:
			OPB_SWNI(SubD)
			break;
		case FR2C_BINOP_MUL:
			OPB_SWNI(MulD)
			break;
		case FR2C_BINOP_DIV:
			OPB_SWNI(DivD)
			break;
		}
		break;
	}
	
	if(!tmp->run)
	{
		FR2CI_FreeOpcode(img, tmp);
		return(NULL);
	}
	
	return(tmp);
}
