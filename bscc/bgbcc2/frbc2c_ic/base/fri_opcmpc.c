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
		{ frm->DN[op->d].i=frm->SN[op->s].TY OP op->imm.TY; }

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

#define OP_NK(OPN, YYN, DN, SN, OP, TY, K) \
	void FR2CI_Op_##OPN##_##YYN(FR2CI_Frame *frm, FR2CI_Opcode *op) \
		{ frm->DN[op->d].i=K; }

#define OPB_NK(OPN, OP, TY, K) \
	OP_NK(OPN, RR, reg, reg, OP, TY, K) \
	OP_NK(OPN, RA, reg, arg, OP, TY, K) \
	OP_NK(OPN, RL, reg, loc, OP, TY, K) \
	OP_NK(OPN, AR, arg, reg, OP, TY, K) \
	OP_NK(OPN, AA, arg, arg, OP, TY, K) \
	OP_NK(OPN, AL, arg, loc, OP, TY, K) \
	OP_NK(OPN, LR, loc, reg, OP, TY, K) \
	OP_NK(OPN, LA, loc, arg, OP, TY, K) \
	OP_NK(OPN, LL, loc, loc, OP, TY, K)

OPB_NI(CmpEqIC, ==, i)
OPB_NI(CmpNeIC, !=, i)
OPB_NI(CmpLtIC, <, i)
OPB_NI(CmpGtIC, >, i)
OPB_NI(CmpLeIC, <=, i)
OPB_NI(CmpGeIC, >=, i)
OPB_NK(CmpAlIC, ==, i, 1)
OPB_NK(CmpNvIC, ==, i, 0)

OPB_NI(CmpEqLC, ==, l)
OPB_NI(CmpNeLC, !=, l)
OPB_NI(CmpLtLC, <, l)
OPB_NI(CmpGtLC, >, l)
OPB_NI(CmpLeLC, <=, l)
OPB_NI(CmpGeLC, >=, l)

OPB_NI(CmpEqFC, ==, f)
OPB_NI(CmpNeFC, !=, f)
OPB_NI(CmpLtFC, <, f)
OPB_NI(CmpGtFC, >, f)
OPB_NI(CmpLeFC, <=, f)
OPB_NI(CmpGeFC, >=, f)

OPB_NI(CmpEqDC, ==, d)
OPB_NI(CmpNeDC, !=, d)
OPB_NI(CmpLtDC, <, d)
OPB_NI(CmpGtDC, >, d)
OPB_NI(CmpLeDC, <=, d)
OPB_NI(CmpGeDC, >=, d)



FR2CI_Opcode *FR2CI_MakeCmpOpCL(
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
		case FR2C_CMP_EQ:
			OPB_SWNI(CmpEqIC)
			break;
		case FR2C_CMP_NE:
			OPB_SWNI(CmpNeIC)
			break;
		case FR2C_CMP_LT:
			OPB_SWNI(CmpLtIC)
			break;
		case FR2C_CMP_GT:
			OPB_SWNI(CmpGtIC)
			break;
		case FR2C_CMP_LE:
			OPB_SWNI(CmpLeIC)
			break;
		case FR2C_CMP_GE:
			OPB_SWNI(CmpGeIC)
			break;
		case FR2C_CMP_AL:
			OPB_SWNI(CmpAlIC)
			break;
		case FR2C_CMP_NV:
			OPB_SWNI(CmpNvIC)
			break;
		}
		break;
		
	case FR2C_TY_L:
		tmp->imm.l=t;
		switch(op)
		{
		case FR2C_CMP_EQ:
			OPB_SWNI(CmpEqLC)
			break;
		case FR2C_CMP_NE:
			OPB_SWNI(CmpNeLC)
			break;
		case FR2C_CMP_LT:
			OPB_SWNI(CmpLtLC)
			break;
		case FR2C_CMP_GT:
			OPB_SWNI(CmpGtLC)
			break;
		case FR2C_CMP_LE:
			OPB_SWNI(CmpLeLC)
			break;
		case FR2C_CMP_GE:
			OPB_SWNI(CmpGeLC)
			break;
		case FR2C_CMP_AL:
			OPB_SWNI(CmpAlIC)
			break;
		case FR2C_CMP_NV:
			OPB_SWNI(CmpNvIC)
			break;
		}
		break;

	case FR2C_TY_F:
		tmp->imm.f=t;
		switch(op)
		{
		case FR2C_CMP_EQ:
			OPB_SWNI(CmpEqFC)
			break;
		case FR2C_CMP_NE:
			OPB_SWNI(CmpNeFC)
			break;
		case FR2C_CMP_LT:
			OPB_SWNI(CmpLtFC)
			break;
		case FR2C_CMP_GT:
			OPB_SWNI(CmpGtFC)
			break;
		case FR2C_CMP_LE:
			OPB_SWNI(CmpLeFC)
			break;
		case FR2C_CMP_GE:
			OPB_SWNI(CmpGeFC)
			break;
		case FR2C_CMP_AL:
			OPB_SWNI(CmpAlIC)
			break;
		case FR2C_CMP_NV:
			OPB_SWNI(CmpNvIC)
			break;
		}
		break;

	case FR2C_TY_D:
		tmp->imm.d=t;
		switch(op)
		{
		case FR2C_CMP_EQ:
			OPB_SWNI(CmpEqDC)
			break;
		case FR2C_CMP_NE:
			OPB_SWNI(CmpNeDC)
			break;
		case FR2C_CMP_LT:
			OPB_SWNI(CmpLtDC)
			break;
		case FR2C_CMP_GT:
			OPB_SWNI(CmpGtDC)
			break;
		case FR2C_CMP_LE:
			OPB_SWNI(CmpLeDC)
			break;
		case FR2C_CMP_GE:
			OPB_SWNI(CmpGeDC)
			break;
		case FR2C_CMP_AL:
			OPB_SWNI(CmpAlIC)
			break;
		case FR2C_CMP_NV:
			OPB_SWNI(CmpNvIC)
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

FR2CI_Opcode *FR2CI_MakeCmpOpCD(
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
		case FR2C_CMP_EQ:
			OPB_SWNI(CmpEqFC)
			break;
		case FR2C_CMP_NE:
			OPB_SWNI(CmpNeFC)
			break;
		case FR2C_CMP_LT:
			OPB_SWNI(CmpLtFC)
			break;
		case FR2C_CMP_GT:
			OPB_SWNI(CmpGtFC)
			break;
		case FR2C_CMP_LE:
			OPB_SWNI(CmpLeFC)
			break;
		case FR2C_CMP_GE:
			OPB_SWNI(CmpGeFC)
			break;
		case FR2C_CMP_AL:
			OPB_SWNI(CmpAlIC)
			break;
		case FR2C_CMP_NV:
			OPB_SWNI(CmpNvIC)
			break;
		}
		break;

	case FR2C_TY_D:
		tmp->imm.d=t;
		switch(op)
		{
		case FR2C_CMP_EQ:
			OPB_SWNI(CmpEqDC)
			break;
		case FR2C_CMP_NE:
			OPB_SWNI(CmpNeDC)
			break;
		case FR2C_CMP_LT:
			OPB_SWNI(CmpLtDC)
			break;
		case FR2C_CMP_GT:
			OPB_SWNI(CmpGtDC)
			break;
		case FR2C_CMP_LE:
			OPB_SWNI(CmpLeDC)
			break;
		case FR2C_CMP_GE:
			OPB_SWNI(CmpGeDC)
			break;
		case FR2C_CMP_AL:
			OPB_SWNI(CmpAlIC)
			break;
		case FR2C_CMP_NV:
			OPB_SWNI(CmpNvIC)
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
