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

#define OP_NI(OPN, YYN, DN, SN, OP, TY) \
	void FR2CI_Op_##OPN##_##YYN(FR2CI_Frame *frm, FR2CI_Opcode *op) \
		{ frm->DN[op->d].TY=frm->SN[op->s].TY OP op->imm.TY; }

#define OPB_NI(OPN, OP, TY) \
	OP_NI(OPN, RR, reg, reg, OP, TY) \
	OP_NI(OPN, RA, reg, arg, OP, TY) \
	OP_NI(OPN, RL, reg, loc, OP, TY) \
	OP_NI(OPN, AR, arg, reg, OP, TY) \
	OP_NI(OPN, AA, arg, arg, OP, TY) \
	OP_NI(OPN, AL, arg, loc, OP, TY) \
	OP_NI(OPN, LR, loc, reg, OP, TY) \
	OP_NI(OPN, LA, loc, arg, OP, TY) \
	OP_NI(OPN, LL, loc, loc, OP, TY)


#define OP_NJ(OPN, YYN, DN, SN, OP, DTY, STY, TTY) \
	void FR2CI_Op_##OPN##_##YYN(FR2CI_Frame *frm, FR2CI_Opcode *op) \
		{ frm->DN[op->d].DTY=frm->SN[op->s].STY OP op->imm.TTY; }

#define OPB_NJ(OPN, OP, DTY, STY, TTY) \
	OP_NJ(OPN, RR, reg, reg, OP, DTY, STY, TTY) \
	OP_NJ(OPN, RA, reg, arg, OP, DTY, STY, TTY) \
	OP_NJ(OPN, RL, reg, loc, OP, DTY, STY, TTY) \
	OP_NJ(OPN, AR, arg, reg, OP, DTY, STY, TTY) \
	OP_NJ(OPN, AA, arg, arg, OP, DTY, STY, TTY) \
	OP_NJ(OPN, AL, arg, loc, OP, DTY, STY, TTY) \
	OP_NJ(OPN, LR, loc, reg, OP, DTY, STY, TTY) \
	OP_NJ(OPN, LA, loc, arg, OP, DTY, STY, TTY) \
	OP_NJ(OPN, LL, loc, loc, OP, DTY, STY, TTY)

OPB_NI(AddIC, +, i)
OPB_NI(SubIC, -, i)
OPB_NI(MulIC, *, i)
OPB_NI(DivIC, /, i)
OPB_NI(ModIC, %, i)
OPB_NI(AndIC, &, i)
OPB_NI(OrIC, |, i)
OPB_NI(XorIC, ^, i)
OPB_NJ(ShlIC, <<, i, i, i)
OPB_NJ(ShrIC, >>, i, i, i)
OPB_NJ(ShrrIC, >>, i, ui, i)

OPB_NI(AddLC, +, l)
OPB_NI(SubLC, -, l)
OPB_NI(MulLC, *, l)
OPB_NI(DivLC, /, l)
OPB_NI(ModLC, %, l)
OPB_NI(AndLC, &, l)
OPB_NI(OrLC, |, l)
OPB_NI(XorLC, ^, l)
OPB_NJ(ShlLC, <<, l, l, i)
OPB_NJ(ShrLC, >>, l, l, i)
OPB_NJ(ShrrLC, >>, l, ul, i)

OPB_NI(AddFC, +, f)
OPB_NI(SubFC, -, f)
OPB_NI(MulFC, *, f)
OPB_NI(DivFC, /, f)

OPB_NI(AddDC, +, d)
OPB_NI(SubDC, -, d)
OPB_NI(MulDC, *, d)
OPB_NI(DivDC, /, d)



FR2CI_Opcode *FR2CI_MakeBinOpCL(
	FR2CI_Image *img,
	int z, int y, int op,
	int d, int s, s64 t)
{
	FR2CI_Opcode *tmp;
	
	tmp=FR2CI_AllocOpcode(img);
	tmp->d=d;
	tmp->s=s;
//	tmp->imm.l=t;
	
	switch(z)
	{
	case FR2C_TY_I:
		tmp->imm.i=t;
		switch(op)
		{
		case FR2C_BINOP_ADD:
			OPB_SWNI(AddIC)
			break;
		case FR2C_BINOP_SUB:
			OPB_SWNI(SubIC)
			break;
		case FR2C_BINOP_MUL:
			OPB_SWNI(MulIC)
			break;
		case FR2C_BINOP_DIV:
			OPB_SWNI(DivIC)
			break;
		case FR2C_BINOP_MOD:
			OPB_SWNI(ModIC)
			break;
		case FR2C_BINOP_AND:
			OPB_SWNI(AndIC)
			break;
		case FR2C_BINOP_OR:
			OPB_SWNI(OrIC)
			break;
		case FR2C_BINOP_XOR:
			OPB_SWNI(XorIC)
			break;
		case FR2C_BINOP_SHL:
			OPB_SWNI(ShlIC)
			break;
		case FR2C_BINOP_SHR:
			OPB_SWNI(ShrIC)
			break;
		case FR2C_BINOP_SHRR:
			OPB_SWNI(ShrrIC)
			break;
		}
		break;
		
	case FR2C_TY_L:
		tmp->imm.l=t;
		switch(op)
		{
		case FR2C_BINOP_ADD:
			OPB_SWNI(AddLC)
			break;
		case FR2C_BINOP_SUB:
			OPB_SWNI(SubLC)
			break;
		case FR2C_BINOP_MUL:
			OPB_SWNI(MulLC)
			break;
		case FR2C_BINOP_DIV:
			OPB_SWNI(DivLC)
			break;
		case FR2C_BINOP_MOD:
			OPB_SWNI(ModLC)
			break;
		case FR2C_BINOP_AND:
			OPB_SWNI(AndLC)
			break;
		case FR2C_BINOP_OR:
			OPB_SWNI(OrLC)
			break;
		case FR2C_BINOP_XOR:
			OPB_SWNI(XorLC)
			break;
		case FR2C_BINOP_SHL:
			OPB_SWNI(ShlLC)
			break;
		case FR2C_BINOP_SHR:
			OPB_SWNI(ShrLC)
			break;
		case FR2C_BINOP_SHRR:
			OPB_SWNI(ShrrLC)
			break;
		}
		break;

	case FR2C_TY_F:
		tmp->imm.f=t;
		switch(op)
		{
		case FR2C_BINOP_ADD:
			OPB_SWNI(AddFC)
			break;
		case FR2C_BINOP_SUB:
			OPB_SWNI(SubFC)
			break;
		case FR2C_BINOP_MUL:
			OPB_SWNI(MulFC)
			break;
		case FR2C_BINOP_DIV:
			OPB_SWNI(DivFC)
			break;
		}
		break;

	case FR2C_TY_D:
		tmp->imm.d=t;
		switch(op)
		{
		case FR2C_BINOP_ADD:
			OPB_SWNI(AddDC)
			break;
		case FR2C_BINOP_SUB:
			OPB_SWNI(SubDC)
			break;
		case FR2C_BINOP_MUL:
			OPB_SWNI(MulDC)
			break;
		case FR2C_BINOP_DIV:
			OPB_SWNI(DivDC)
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

FR2CI_Opcode *FR2CI_MakeBinOpCD(
	FR2CI_Image *img,
	int z, int y, int op,
	int d, int s, double t)
{
	FR2CI_Opcode *tmp;
	
	tmp=FR2CI_AllocOpcode(img);
	tmp->d=d;
	tmp->s=s;
//	tmp->imm.l=t;
	
	switch(z)
	{
	case FR2C_TY_F:
		tmp->imm.f=t;
		switch(op)
		{
		case FR2C_BINOP_ADD:
			OPB_SWNI(AddFC)
			break;
		case FR2C_BINOP_SUB:
			OPB_SWNI(SubFC)
			break;
		case FR2C_BINOP_MUL:
			OPB_SWNI(MulFC)
			break;
		case FR2C_BINOP_DIV:
			OPB_SWNI(DivFC)
			break;
		}
		break;

	case FR2C_TY_D:
		tmp->imm.d=t;
		switch(op)
		{
		case FR2C_BINOP_ADD:
			OPB_SWNI(AddDC)
			break;
		case FR2C_BINOP_SUB:
			OPB_SWNI(SubDC)
			break;
		case FR2C_BINOP_MUL:
			OPB_SWNI(MulDC)
			break;
		case FR2C_BINOP_DIV:
			OPB_SWNI(DivDC)
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
