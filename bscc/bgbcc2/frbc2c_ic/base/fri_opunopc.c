#include <frbc2ci.h>

#define OP_SWNI(OPN, YYN) \
	case FR2C_OPMODE_##YYN: tmp->run=FR2CI_Op_##OPN##_##YYN;

#define OPB_SWNI(OPN) \
	switch(y) {	\
		OP_SWNI(OPN, RR)	\
		OP_SWNI(OPN, RA)	\
		OP_SWNI(OPN, RL)	}

#define OP_NI(OPN, YYN, DN, OP, TY) \
	void FR2CI_Op_##OPN##_##YYN(FR2CI_Frame *frm, FR2CI_Opcode *op) \
		{ frm->DN[op->d].TY=OP op->imm.TY; }

#define OPB_NI(OPN, OP, TY) \
	OP_NI(OPN, RR, reg, OP, TY) \
	OP_NI(OPN, RA, arg, OP, TY) \
	OP_NI(OPN, RL, loc, OP, TY)


#define OP_NJ(OPN, YYN, DN, OP, DTY, STY) \
	void FR2CI_Op_##OPN##_##YYN(FR2CI_Frame *frm, FR2CI_Opcode *op) \
		{ frm->DN[op->d].DTY=OP op->imm.STY; }

#define OPB_NJ(OPN, OP, DTY, STY) \
	OP_NJ(OPN, RR, reg, OP, DTY, STY) \
	OP_NJ(OPN, RA, arg, OP, DTY, STY) \
	OP_NJ(OPN, RL, loc, OP, DTY, STY)

#define OP_NK(OPN, YYN, DN, OP, TY) \
	void FR2CI_Op_##OPN##_##YYN(FR2CI_Frame *frm, FR2CI_Opcode *op) \
		{ frm->DN[op->d].TY=op->imm.TY OP; }

#define OPB_NK(OPN, OP, TY) \
	OP_NK(OPN, RR, reg, OP, TY) \
	OP_NK(OPN, RA, arg, OP, TY) \
	OP_NK(OPN, RL, loc, OP, TY)

OPB_NI(MovIC, , i)
OPB_NI(NegIC, -, i)
OPB_NI(NotIC, ~, i)
OPB_NI(LNotIC, !, i)
OPB_NK(IncIC, +1, i)
OPB_NK(DecIC, -1, i)

OPB_NI(MovLC, , l)
OPB_NI(NegLC, -, l)
OPB_NI(NotLC, ~, l)
OPB_NI(LNotLC, !, l)
OPB_NK(IncLC, +1, l)
OPB_NK(DecLC, -1, l)

OPB_NI(MovFC, , f)
OPB_NI(NegFC, -, f)
OPB_NK(IncFC, +1, f)
OPB_NK(DecFC, -1, f)

OPB_NI(MovDC, , d)
OPB_NI(NegDC, -, d)
OPB_NK(IncDC, +1, d)
OPB_NK(DecDC, -1, d)


FR2CI_Opcode *FR2CI_MakeUnOpCL(
	FR2CI_Image *img,
	int z, int y, int op,
	int d, s64 s)
{
	FR2CI_Opcode *tmp;
	
	tmp=FR2CI_AllocOpcode(img);
	tmp->d=d;
//	tmp->s=s;
//	tmp->t=t;
	
	switch(z)
	{
	case FR2C_TY_I:
		tmp->imm.i=s;
		switch(op)
		{
		case FR2C_UNOP_MOV:
			OPB_SWNI(MovIC)
			break;
		case FR2C_UNOP_NEG:
			OPB_SWNI(NegIC)
			break;
		case FR2C_UNOP_NOT:
			OPB_SWNI(NotIC)
			break;
		case FR2C_UNOP_LNOT:
			OPB_SWNI(LNotIC)
			break;
		case FR2C_UNOP_INC:
			OPB_SWNI(IncIC)
			break;
		case FR2C_UNOP_DEC:
			OPB_SWNI(DecIC)
			break;
		}
		break;
		
	case FR2C_TY_L:
		tmp->imm.l=s;
		switch(op)
		{
		case FR2C_UNOP_MOV:
			OPB_SWNI(MovLC)
			break;
		case FR2C_UNOP_NEG:
			OPB_SWNI(NegLC)
			break;
		case FR2C_UNOP_NOT:
			OPB_SWNI(NotLC)
			break;
		case FR2C_UNOP_LNOT:
			OPB_SWNI(LNotLC)
			break;
		case FR2C_UNOP_INC:
			OPB_SWNI(IncLC)
			break;
		case FR2C_UNOP_DEC:
			OPB_SWNI(DecLC)
			break;
		}
		break;

	case FR2C_TY_F:
		tmp->imm.f=s;
		switch(op)
		{
		case FR2C_UNOP_MOV:
			OPB_SWNI(MovFC)
			break;
		case FR2C_UNOP_NEG:
			OPB_SWNI(NegFC)
			break;
		case FR2C_UNOP_INC:
			OPB_SWNI(IncFC)
			break;
		case FR2C_UNOP_DEC:
			OPB_SWNI(DecFC)
			break;
		}
		break;

	case FR2C_TY_D:
		tmp->imm.d=s;
		switch(op)
		{
		case FR2C_UNOP_MOV:
			OPB_SWNI(MovDC)
			break;
		case FR2C_UNOP_NEG:
			OPB_SWNI(NegDC)
			break;
		case FR2C_UNOP_INC:
			OPB_SWNI(IncDC)
			break;
		case FR2C_UNOP_DEC:
			OPB_SWNI(DecDC)
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

FR2CI_Opcode *FR2CI_MakeUnOpCD(
	FR2CI_Image *img,
	int z, int y, int op,
	int d, f64 s)
{
	FR2CI_Opcode *tmp;
	
	tmp=FR2CI_AllocOpcode(img);
	tmp->d=d;
//	tmp->s=s;
//	tmp->t=t;
	
	switch(z)
	{
	case FR2C_TY_I:
	case FR2C_TY_L:
		FR2CI_FreeOpcode(img, tmp);
		tmp=FR2CI_MakeUnOpCL(img, z, y, op, d, s);
		break;

	case FR2C_TY_F:
		tmp->imm.f=s;
		switch(op)
		{
		case FR2C_UNOP_MOV:
			OPB_SWNI(MovFC)
			break;
		case FR2C_UNOP_NEG:
			OPB_SWNI(NegFC)
			break;
		case FR2C_UNOP_INC:
			OPB_SWNI(IncFC)
			break;
		case FR2C_UNOP_DEC:
			OPB_SWNI(DecFC)
			break;
		}
		break;

	case FR2C_TY_D:
		tmp->imm.d=s;
		switch(op)
		{
		case FR2C_UNOP_MOV:
			OPB_SWNI(MovDC)
			break;
		case FR2C_UNOP_NEG:
			OPB_SWNI(NegDC)
			break;
		case FR2C_UNOP_INC:
			OPB_SWNI(IncDC)
			break;
		case FR2C_UNOP_DEC:
			OPB_SWNI(DecDC)
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
