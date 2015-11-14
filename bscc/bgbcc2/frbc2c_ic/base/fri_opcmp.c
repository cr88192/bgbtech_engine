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
		{ frm->DN[op->d].i=frm->SN[op->s].TY OP frm->TN[op->t].TY; }

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


#define OP_ND(OPN, YYN, DN, SN, TN, OP, TY, K) \
	void FR2CI_Op_##OPN##_##YYN(FR2CI_Frame *frm, FR2CI_Opcode *op) \
		{ frm->DN[op->d].i=K; }

#define OPB_ND(OPN, OP, TY, K) \
	OP_ND(OPN, RRR, reg, reg, reg, OP, TY, K) \
	OP_ND(OPN, RRA, reg, reg, arg, OP, TY, K) \
	OP_ND(OPN, RRL, reg, reg, loc, OP, TY, K) \
	OP_ND(OPN, RAR, reg, arg, reg, OP, TY, K) \
	OP_ND(OPN, RAA, reg, arg, arg, OP, TY, K) \
	OP_ND(OPN, RAL, reg, arg, loc, OP, TY, K) \
	OP_ND(OPN, RLR, reg, loc, reg, OP, TY, K) \
	OP_ND(OPN, RLA, reg, loc, arg, OP, TY, K) \
	OP_ND(OPN, RLL, reg, loc, loc, OP, TY, K) \
	OP_ND(OPN, ARR, arg, reg, reg, OP, TY, K) \
	OP_ND(OPN, LRR, loc, reg, reg, OP, TY, K) \
	OP_ND(OPN, LRA, loc, reg, arg, OP, TY, K) \
	OP_ND(OPN, LAR, loc, arg, reg, OP, TY, K) \
	OP_ND(OPN, LRL, loc, reg, loc, OP, TY, K) \
	OP_ND(OPN, LLR, loc, loc, reg, OP, TY, K) \
	OP_ND(OPN, LLL, loc, loc, loc, OP, TY, K)


OPB_NI(CmpEqI, ==, i)
OPB_NI(CmpNeI, !=, i)
OPB_NI(CmpLtI, <, i)
OPB_NI(CmpGtI, >, i)
OPB_NI(CmpLeI, <=, i)
OPB_NI(CmpGeI, >=, i)
OPB_ND(CmpAlI, <=, i, 1)
OPB_ND(CmpNvI, >=, i, 0)

OPB_NI(CmpEqL, ==, l)
OPB_NI(CmpNeL, !=, l)
OPB_NI(CmpLtL, <, l)
OPB_NI(CmpGtL, >, l)
OPB_NI(CmpLeL, <=, l)
OPB_NI(CmpGeL, >=, l)
OPB_ND(CmpAlL, <=, l, 1)
OPB_ND(CmpNvL, >=, l, 0)

OPB_NI(CmpEqF, ==, f)
OPB_NI(CmpNeF, !=, f)
OPB_NI(CmpLtF, <, f)
OPB_NI(CmpGtF, >, f)
OPB_NI(CmpLeF, <=, f)
OPB_NI(CmpGeF, >=, f)
OPB_ND(CmpAlF, <=, f, 1)
OPB_ND(CmpNvF, >=, f, 0)

OPB_NI(CmpEqD, ==, d)
OPB_NI(CmpNeD, !=, d)
OPB_NI(CmpLtD, <, d)
OPB_NI(CmpGtD, >, d)
OPB_NI(CmpLeD, <=, d)
OPB_NI(CmpGeD, >=, d)
OPB_ND(CmpAlD, <=, d, 1)
OPB_ND(CmpNvD, >=, d, 0)


FR2CI_Opcode *FR2CI_MakeCmpOp(
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
		case FR2C_CMP_EQ:
			OPB_SWNI(CmpEqI)
			break;
		case FR2C_CMP_NE:
			OPB_SWNI(CmpNeI)
			break;
		case FR2C_CMP_LT:
			OPB_SWNI(CmpLtI)
			break;
		case FR2C_CMP_GT:
			OPB_SWNI(CmpGtI)
			break;
		case FR2C_CMP_LE:
			OPB_SWNI(CmpLeI)
			break;
		case FR2C_CMP_GE:
			OPB_SWNI(CmpGeI)
			break;
		case FR2C_CMP_AL:
			OPB_SWNI(CmpAlI)
			break;
		case FR2C_CMP_NV:
			OPB_SWNI(CmpNvI)
			break;
		}
		break;
		
	case FR2C_TY_L:
		switch(op)
		{
		case FR2C_CMP_EQ:
			OPB_SWNI(CmpEqL)
			break;
		case FR2C_CMP_NE:
			OPB_SWNI(CmpNeL)
			break;
		case FR2C_CMP_LT:
			OPB_SWNI(CmpLtL)
			break;
		case FR2C_CMP_GT:
			OPB_SWNI(CmpGtL)
			break;
		case FR2C_CMP_LE:
			OPB_SWNI(CmpLeL)
			break;
		case FR2C_CMP_GE:
			OPB_SWNI(CmpGeL)
			break;
		case FR2C_CMP_AL:
			OPB_SWNI(CmpAlL)
			break;
		case FR2C_CMP_NV:
			OPB_SWNI(CmpNvL)
			break;
		}
		break;

	case FR2C_TY_F:
		switch(op)
		{
		case FR2C_CMP_EQ:
			OPB_SWNI(CmpEqF)
			break;
		case FR2C_CMP_NE:
			OPB_SWNI(CmpNeF)
			break;
		case FR2C_CMP_LT:
			OPB_SWNI(CmpLtF)
			break;
		case FR2C_CMP_GT:
			OPB_SWNI(CmpGtF)
			break;
		case FR2C_CMP_LE:
			OPB_SWNI(CmpLeF)
			break;
		case FR2C_CMP_GE:
			OPB_SWNI(CmpGeF)
			break;
		case FR2C_CMP_AL:
			OPB_SWNI(CmpAlF)
			break;
		case FR2C_CMP_NV:
			OPB_SWNI(CmpNvF)
			break;
		}
		break;

	case FR2C_TY_D:
		switch(op)
		{
		case FR2C_CMP_EQ:
			OPB_SWNI(CmpEqD)
			break;
		case FR2C_CMP_NE:
			OPB_SWNI(CmpNeD)
			break;
		case FR2C_CMP_LT:
			OPB_SWNI(CmpLtD)
			break;
		case FR2C_CMP_GT:
			OPB_SWNI(CmpGtD)
			break;
		case FR2C_CMP_LE:
			OPB_SWNI(CmpLeD)
			break;
		case FR2C_CMP_GE:
			OPB_SWNI(CmpGeD)
			break;
		case FR2C_CMP_AL:
			OPB_SWNI(CmpAlD)
			break;
		case FR2C_CMP_NV:
			OPB_SWNI(CmpNvD)
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
