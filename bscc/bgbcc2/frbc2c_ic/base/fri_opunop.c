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
		{ frm->DN[op->d].TY=OP frm->SN[op->s].TY; }

#define OPB_NI(OPN, OP, TY) \
	OP_NI(OPN, RR, reg, reg, OP, TY) \
	OP_NI(OPN, RA, reg, arg, OP, TY) \
	OP_NI(OPN, RL, reg, loc, OP, TY) \
	OP_NI(OPN, AR, arg, reg, OP, TY) \
	OP_NI(OPN, AA, arg, arg, OP, TY) \
	OP_NI(OPN, AL, arg, loc, OP, TY) \
	OP_NI(OPN, LR, loc, reg, OP, TY) \
	OP_NI(OPN, LA, loc, arg, OP, TY) \
	OP_NI(OPN, LL, loc, loc, OP, TY) \


#define OP_NJ(OPN, YYN, DN, SN, OP, DTY, STY) \
	void FR2CI_Op_##OPN##_##YYN(FR2CI_Frame *frm, FR2CI_Opcode *op) \
		{ frm->DN[op->d].DTY=OP frm->SN[op->s].STY; }

#define OPB_NJ(OPN, OP, DTY, STY) \
	OP_NJ(OPN, RR, reg, reg, OP, DTY, STY) \
	OP_NJ(OPN, RA, reg, arg, OP, DTY, STY) \
	OP_NJ(OPN, RL, reg, loc, OP, DTY, STY) \
	OP_NJ(OPN, AR, arg, reg, OP, DTY, STY) \
	OP_NJ(OPN, AA, arg, arg, OP, DTY, STY) \
	OP_NJ(OPN, AL, arg, loc, OP, DTY, STY) \
	OP_NJ(OPN, LR, loc, reg, OP, DTY, STY) \
	OP_NJ(OPN, LA, loc, arg, OP, DTY, STY) \
	OP_NJ(OPN, LL, loc, loc, OP, DTY, STY) \

#define OP_NK(OPN, YYN, DN, SN, OP, TY) \
	void FR2CI_Op_##OPN##_##YYN(FR2CI_Frame *frm, FR2CI_Opcode *op) \
		{ frm->DN[op->d].TY=frm->SN[op->s].TY OP; }

#define OPB_NK(OPN, OP, TY) \
	OP_NK(OPN, RR, reg, reg, OP, TY) \
	OP_NK(OPN, RA, reg, arg, OP, TY) \
	OP_NK(OPN, RL, reg, loc, OP, TY) \
	OP_NK(OPN, AR, arg, reg, OP, TY) \
	OP_NK(OPN, AA, arg, arg, OP, TY) \
	OP_NK(OPN, AL, arg, loc, OP, TY) \
	OP_NK(OPN, LR, loc, reg, OP, TY) \
	OP_NK(OPN, LA, loc, arg, OP, TY) \
	OP_NK(OPN, LL, loc, loc, OP, TY) \

OPB_NI(MovI, , i)
OPB_NI(NegI, -, i)
OPB_NI(NotI, ~, i)
OPB_NI(LNotI, !, i)
OPB_NK(IncI, +1, i)
OPB_NK(DecI, -1, i)

OPB_NI(MovL, , l)
OPB_NI(NegL, -, l)
OPB_NI(NotL, ~, l)
OPB_NI(LNotL, !, l)
OPB_NK(IncL, +1, l)
OPB_NK(DecL, -1, l)

OPB_NI(MovF, , f)
OPB_NI(NegF, -, f)
OPB_NK(IncF, +1, f)
OPB_NK(DecF, -1, f)

OPB_NI(MovD, , d)
OPB_NI(NegD, -, d)
OPB_NK(IncD, +1, d)
OPB_NK(DecD, -1, d)


FR2CI_Opcode *FR2CI_MakeUnOp(
	FR2CI_Image *img,
	int z, int y, int op,
	int d, int s)
{
	FR2CI_Opcode *tmp;
	
	tmp=FR2CI_AllocOpcode(img);
	tmp->d=d;
	tmp->s=s;
//	tmp->t=t;
	
	switch(z)
	{
	case FR2C_TY_I:
		switch(op)
		{
		case FR2C_UNOP_MOV:
			OPB_SWNI(MovI)
			break;
		case FR2C_UNOP_NEG:
			OPB_SWNI(NegI)
			break;
		case FR2C_UNOP_NOT:
			OPB_SWNI(NotI)
			break;
		case FR2C_UNOP_LNOT:
			OPB_SWNI(LNotI)
			break;
		case FR2C_UNOP_INC:
			OPB_SWNI(IncI)
			break;
		case FR2C_UNOP_DEC:
			OPB_SWNI(DecI)
			break;
		}
		break;
		
	case FR2C_TY_L:
		switch(op)
		{
		case FR2C_UNOP_MOV:
			OPB_SWNI(MovL)
			break;
		case FR2C_UNOP_NEG:
			OPB_SWNI(NegL)
			break;
		case FR2C_UNOP_NOT:
			OPB_SWNI(NotL)
			break;
		case FR2C_UNOP_LNOT:
			OPB_SWNI(LNotL)
			break;
		case FR2C_UNOP_INC:
			OPB_SWNI(IncL)
			break;
		case FR2C_UNOP_DEC:
			OPB_SWNI(DecL)
			break;
		}
		break;

	case FR2C_TY_F:
		switch(op)
		{
		case FR2C_UNOP_MOV:
			OPB_SWNI(MovF)
			break;
		case FR2C_UNOP_NEG:
			OPB_SWNI(NegF)
			break;
		case FR2C_UNOP_INC:
			OPB_SWNI(IncF)
			break;
		case FR2C_UNOP_DEC:
			OPB_SWNI(DecF)
			break;
		}
		break;

	case FR2C_TY_D:
		switch(op)
		{
		case FR2C_UNOP_MOV:
			OPB_SWNI(MovD)
			break;
		case FR2C_UNOP_NEG:
			OPB_SWNI(NegD)
			break;
		case FR2C_UNOP_INC:
			OPB_SWNI(IncD)
			break;
		case FR2C_UNOP_DEC:
			OPB_SWNI(DecD)
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
