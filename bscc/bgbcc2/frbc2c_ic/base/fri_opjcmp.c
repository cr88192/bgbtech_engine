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

#define OP_NI(OPN, YYN, SN, TN, OP, TY) \
	FR2CI_Trace *FR2CI_Op_##OPN##_##YYN(FR2CI_Frame *frm, \
		FR2CI_TailOpcode *op) \
	{ return((frm->SN[op->s].TY OP frm->TN[op->t].TY)? \
		(op->jmptrace):(op->nexttrace)); }

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


#define OP_ND(OPN, YYN, SN, TN, OP, TY, K) \
	FR2CI_Trace *FR2CI_Op_##OPN##_##YYN(FR2CI_Frame *frm, \
		FR2CI_TailOpcode *op) \
	{ return((K)?(op->jmptrace):(op->nexttrace)); }

#define OPB_ND(OPN, OP, TY, K) \
	OP_ND(OPN, RR, reg, reg, OP, TY, K) \
	OP_ND(OPN, RA, reg, arg, OP, TY, K) \
	OP_ND(OPN, RL, reg, loc, OP, TY, K) \
	OP_ND(OPN, AR, arg, reg, OP, TY, K) \
	OP_ND(OPN, AA, arg, arg, OP, TY, K) \
	OP_ND(OPN, AL, arg, loc, OP, TY, K) \
	OP_ND(OPN, LR, loc, reg, OP, TY, K) \
	OP_ND(OPN, LA, loc, arg, OP, TY, K) \
	OP_ND(OPN, LL, loc, loc, OP, TY, K) \


OPB_NI(JCmpEqI, ==, i)
OPB_NI(JCmpNeI, !=, i)
OPB_NI(JCmpLtI, <, i)
OPB_NI(JCmpGtI, >, i)
OPB_NI(JCmpLeI, <=, i)
OPB_NI(JCmpGeI, >=, i)
//OPB_ND(JCmpAlI, <=, i, 1)
//OPB_ND(JCmpNvI, >=, i, 0)

OPB_NI(JCmpEqL, ==, l)
OPB_NI(JCmpNeL, !=, l)
OPB_NI(JCmpLtL, <, l)
OPB_NI(JCmpGtL, >, l)
OPB_NI(JCmpLeL, <=, l)
OPB_NI(JCmpGeL, >=, l)
//OPB_ND(JCmpAlL, <=, l, 1)
//OPB_ND(JCmpNvL, >=, l, 0)

OPB_NI(JCmpEqF, ==, f)
OPB_NI(JCmpNeF, !=, f)
OPB_NI(JCmpLtF, <, f)
OPB_NI(JCmpGtF, >, f)
OPB_NI(JCmpLeF, <=, f)
OPB_NI(JCmpGeF, >=, f)
//OPB_ND(JCmpAlF, <=, f, 1)
//OPB_ND(JCmpNvF, >=, f, 0)

OPB_NI(JCmpEqD, ==, d)
OPB_NI(JCmpNeD, !=, d)
OPB_NI(JCmpLtD, <, d)
OPB_NI(JCmpGtD, >, d)
OPB_NI(JCmpLeD, <=, d)
OPB_NI(JCmpGeD, >=, d)
//OPB_ND(JCmpAlD, <=, d, 1)
//OPB_ND(JCmpNvD, >=, d, 0)


FR2CI_TailOpcode *FR2CI_MakeJCmpOp(
	FR2CI_Image *img,
	int z, int y, int op,
	int s, int t, byte *ip)
{
	FR2CI_TailOpcode *tmp;
	
	tmp=FR2CI_AllocTailOpcode(img);
//	tmp->d=d;
	tmp->s=s;
	tmp->t=t;
	tmp->jmpip=ip;
	
	switch(z)
	{
	case FR2C_TY_I:
		switch(op)
		{
		case FR2C_CMP_EQ:
			OPB_SWNI(JCmpEqI)
			break;
		case FR2C_CMP_NE:
			OPB_SWNI(JCmpNeI)
			break;
		case FR2C_CMP_LT:
			OPB_SWNI(JCmpLtI)
			break;
		case FR2C_CMP_GT:
			OPB_SWNI(JCmpGtI)
			break;
		case FR2C_CMP_LE:
			OPB_SWNI(JCmpLeI)
			break;
		case FR2C_CMP_GE:
			OPB_SWNI(JCmpGeI)
			break;
		case FR2C_CMP_AL:
			tmp->run=FR2CI_Op_JmpAl;
//			OPB_SWNI(JCmpAlI)
			break;
		case FR2C_CMP_NV:
			tmp->run=FR2CI_Op_JmpNv;
//			OPB_SWNI(JCmpNvI)
			break;
		}
		break;
		
	case FR2C_TY_L:
		switch(op)
		{
		case FR2C_CMP_EQ:
			OPB_SWNI(JCmpEqL)
			break;
		case FR2C_CMP_NE:
			OPB_SWNI(JCmpNeL)
			break;
		case FR2C_CMP_LT:
			OPB_SWNI(JCmpLtL)
			break;
		case FR2C_CMP_GT:
			OPB_SWNI(JCmpGtL)
			break;
		case FR2C_CMP_LE:
			OPB_SWNI(JCmpLeL)
			break;
		case FR2C_CMP_GE:
			OPB_SWNI(JCmpGeL)
			break;
		case FR2C_CMP_AL:
			tmp->run=FR2CI_Op_JmpAl;
//			OPB_SWNI(JCmpAlL)
			break;
		case FR2C_CMP_NV:
			tmp->run=FR2CI_Op_JmpNv;
//			OPB_SWNI(JCmpNvL)
			break;
		}
		break;

	case FR2C_TY_F:
		switch(op)
		{
		case FR2C_CMP_EQ:
			OPB_SWNI(JCmpEqF)
			break;
		case FR2C_CMP_NE:
			OPB_SWNI(JCmpNeF)
			break;
		case FR2C_CMP_LT:
			OPB_SWNI(JCmpLtF)
			break;
		case FR2C_CMP_GT:
			OPB_SWNI(JCmpGtF)
			break;
		case FR2C_CMP_LE:
			OPB_SWNI(JCmpLeF)
			break;
		case FR2C_CMP_GE:
			OPB_SWNI(JCmpGeF)
			break;
		case FR2C_CMP_AL:
			tmp->run=FR2CI_Op_JmpAl;
//			OPB_SWNI(JCmpAlF)
			break;
		case FR2C_CMP_NV:
			tmp->run=FR2CI_Op_JmpNv;
//			OPB_SWNI(JCmpNvF)
			break;
		}
		break;

	case FR2C_TY_D:
		switch(op)
		{
		case FR2C_CMP_EQ:
			OPB_SWNI(JCmpEqD)
			break;
		case FR2C_CMP_NE:
			OPB_SWNI(JCmpNeD)
			break;
		case FR2C_CMP_LT:
			OPB_SWNI(JCmpLtD)
			break;
		case FR2C_CMP_GT:
			OPB_SWNI(JCmpGtD)
			break;
		case FR2C_CMP_LE:
			OPB_SWNI(JCmpLeD)
			break;
		case FR2C_CMP_GE:
			OPB_SWNI(JCmpGeD)
			break;
		case FR2C_CMP_AL:
			tmp->run=FR2CI_Op_JmpAl;
//			OPB_SWNI(JCmpAlD)
			break;
		case FR2C_CMP_NV:
			tmp->run=FR2CI_Op_JmpNv;
//			OPB_SWNI(JCmpNvD)
			break;
		}
		break;
	}
	
	if(!tmp->run)
	{
		FR2CI_FreeTailOpcode(img, tmp);
		return(NULL);
	}
	
	return(tmp);
}


#define OP_SWNIC(OPN, YYN) \
	case FR2C_OPMODE_##YYN: tmp->run=FR2CI_Op_##OPN##_##YYN;

#define OPB_SWNIC(OPN) \
	switch(y) {	\
		OP_SWNIC(OPN, RR)	\
		OP_SWNIC(OPN, RA)	\
		OP_SWNIC(OPN, RL)	}

#define OP_NIC(OPN, YYN, SN, OP, TY) \
	FR2CI_Trace *FR2CI_Op_##OPN##_##YYN(FR2CI_Frame *frm, \
		FR2CI_TailOpcode *op) \
	{ return((frm->SN[op->s].TY OP op->imm.TY)? \
		(op->jmptrace):(op->nexttrace)); }

#define OPB_NIC(OPN, OP, TY) \
	OP_NIC(OPN, RR, reg, OP, TY) \
	OP_NIC(OPN, RA, arg, OP, TY) \
	OP_NIC(OPN, RL, loc, OP, TY) \

OPB_NIC(JCmpEqIC, ==, i)
OPB_NIC(JCmpNeIC, !=, i)
OPB_NIC(JCmpLtIC, <, i)
OPB_NIC(JCmpGtIC, >, i)
OPB_NIC(JCmpLeIC, <=, i)
OPB_NIC(JCmpGeIC, >=, i)

OPB_NIC(JCmpEqLC, ==, l)
OPB_NIC(JCmpNeLC, !=, l)
OPB_NIC(JCmpLtLC, <, l)
OPB_NIC(JCmpGtLC, >, l)
OPB_NIC(JCmpLeLC, <=, l)
OPB_NIC(JCmpGeLC, >=, l)

OPB_NIC(JCmpEqFC, ==, f)
OPB_NIC(JCmpNeFC, !=, f)
OPB_NIC(JCmpLtFC, <, f)
OPB_NIC(JCmpGtFC, >, f)
OPB_NIC(JCmpLeFC, <=, f)
OPB_NIC(JCmpGeFC, >=, f)

OPB_NIC(JCmpEqDC, ==, d)
OPB_NIC(JCmpNeDC, !=, d)
OPB_NIC(JCmpLtDC, <, d)
OPB_NIC(JCmpGtDC, >, d)
OPB_NIC(JCmpLeDC, <=, d)
OPB_NIC(JCmpGeDC, >=, d)


FR2CI_TailOpcode *FR2CI_MakeJCmpOpLC(
	FR2CI_Image *img,
	int z, int y, int op,
	int s, s64 v, byte *ip)
{
	FR2CI_TailOpcode *tmp;
	
	tmp=FR2CI_AllocTailOpcode(img);
//	tmp->d=d;
	tmp->s=s;
//	tmp->t=t;
	tmp->jmpip=ip;
	
	switch(z)
	{
	case FR2C_TY_I:
		tmp->imm.i=v;
		switch(op)
		{
		case FR2C_CMP_EQ:
			OPB_SWNIC(JCmpEqIC)
			break;
		case FR2C_CMP_NE:
			OPB_SWNIC(JCmpNeIC)
			break;
		case FR2C_CMP_LT:
			OPB_SWNIC(JCmpLtIC)
			break;
		case FR2C_CMP_GT:
			OPB_SWNIC(JCmpGtIC)
			break;
		case FR2C_CMP_LE:
			OPB_SWNIC(JCmpLeIC)
			break;
		case FR2C_CMP_GE:
			OPB_SWNIC(JCmpGeIC)
			break;
		case FR2C_CMP_AL:
			tmp->run=FR2CI_Op_JmpAl;
//			OPB_SWNI(JCmpAlI)
			break;
		case FR2C_CMP_NV:
			tmp->run=FR2CI_Op_JmpNv;
//			OPB_SWNI(JCmpNvI)
			break;
		}
		break;
		
	case FR2C_TY_L:
		tmp->imm.l=v;
		switch(op)
		{
		case FR2C_CMP_EQ:
			OPB_SWNIC(JCmpEqLC)
			break;
		case FR2C_CMP_NE:
			OPB_SWNIC(JCmpNeLC)
			break;
		case FR2C_CMP_LT:
			OPB_SWNIC(JCmpLtLC)
			break;
		case FR2C_CMP_GT:
			OPB_SWNIC(JCmpGtLC)
			break;
		case FR2C_CMP_LE:
			OPB_SWNIC(JCmpLeLC)
			break;
		case FR2C_CMP_GE:
			OPB_SWNIC(JCmpGeLC)
			break;
		case FR2C_CMP_AL:
			tmp->run=FR2CI_Op_JmpAl;
//			OPB_SWNI(JCmpAlL)
			break;
		case FR2C_CMP_NV:
			tmp->run=FR2CI_Op_JmpNv;
//			OPB_SWNI(JCmpNvL)
			break;
		}
		break;

	case FR2C_TY_F:
	case FR2C_TY_D:
		break;
	}
	
	if(!tmp->run)
	{
		FR2CI_FreeTailOpcode(img, tmp);
		return(NULL);
	}
	
	return(tmp);
}


FR2CI_TailOpcode *FR2CI_MakeJCmpOpDC(
	FR2CI_Image *img,
	int z, int y, int op,
	int s, double v, byte *ip)
{
	FR2CI_TailOpcode *tmp;
	
	tmp=FR2CI_AllocTailOpcode(img);
//	tmp->d=d;
	tmp->s=s;
//	tmp->t=t;
	tmp->jmpip=ip;
	
	switch(z)
	{
	case FR2C_TY_F:
		tmp->imm.f=v;
		switch(op)
		{
		case FR2C_CMP_EQ:
			OPB_SWNIC(JCmpEqFC)
			break;
		case FR2C_CMP_NE:
			OPB_SWNIC(JCmpNeFC)
			break;
		case FR2C_CMP_LT:
			OPB_SWNIC(JCmpLtFC)
			break;
		case FR2C_CMP_GT:
			OPB_SWNIC(JCmpGtFC)
			break;
		case FR2C_CMP_LE:
			OPB_SWNIC(JCmpLeFC)
			break;
		case FR2C_CMP_GE:
			OPB_SWNIC(JCmpGeFC)
			break;
		case FR2C_CMP_AL:
			tmp->run=FR2CI_Op_JmpAl;
//			OPB_SWNI(JCmpAlF)
			break;
		case FR2C_CMP_NV:
			tmp->run=FR2CI_Op_JmpNv;
//			OPB_SWNI(JCmpNvF)
			break;
		}
		break;

	case FR2C_TY_D:
		tmp->imm.d=v;
		switch(op)
		{
		case FR2C_CMP_EQ:
			OPB_SWNIC(JCmpEqDC)
			break;
		case FR2C_CMP_NE:
			OPB_SWNIC(JCmpNeDC)
			break;
		case FR2C_CMP_LT:
			OPB_SWNIC(JCmpLtDC)
			break;
		case FR2C_CMP_GT:
			OPB_SWNIC(JCmpGtDC)
			break;
		case FR2C_CMP_LE:
			OPB_SWNIC(JCmpLeDC)
			break;
		case FR2C_CMP_GE:
			OPB_SWNIC(JCmpGeDC)
			break;
		case FR2C_CMP_AL:
			tmp->run=FR2CI_Op_JmpAl;
//			OPB_SWNI(JCmpAlD)
			break;
		case FR2C_CMP_NV:
			tmp->run=FR2CI_Op_JmpNv;
//			OPB_SWNI(JCmpNvD)
			break;
		}
		break;
	}
	
	if(!tmp->run)
	{
		FR2CI_FreeTailOpcode(img, tmp);
		return(NULL);
	}
	
	return(tmp);
}

FR2CI_Trace *FR2CI_Op_JmpAl(FR2CI_Frame *frm, FR2CI_TailOpcode *op)
	{ return(op->jmptrace); }

FR2CI_Trace *FR2CI_Op_JmpNv(FR2CI_Frame *frm, FR2CI_TailOpcode *op)
	{ return(op->nexttrace); }

FR2CI_TailOpcode *FR2CI_MakeOpJmp(
	FR2CI_Image *img, byte *ip)
{
	FR2CI_TailOpcode *tmp;
	
	tmp=FR2CI_AllocTailOpcode(img);
	tmp->jmpip=ip;
	tmp->run=FR2CI_Op_JmpAl;
	
	if(!tmp->run)
	{
		FR2CI_FreeTailOpcode(img, tmp);
		return(NULL);
	}
	
	return(tmp);
}

FR2CI_TailOpcode *FR2CI_MakeOpJmpZ(
	FR2CI_Image *img, int opc, int op, int s, byte *ip)
{
	FR2CI_TailOpcode *tmp;
	switch(opc)
	{
	case FR2C_OP_JCMP_RZI:
		tmp=FR2CI_MakeJCmpOpLC(img, FR2C_TY_I,
			FR2C_OPMODE_RR, op, s, 0, ip);
			break;
	case FR2C_OP_JCMP_RZL:
		tmp=FR2CI_MakeJCmpOpLC(img, FR2C_TY_L,
			FR2C_OPMODE_RR, op, s, 0, ip);
			break;
	case FR2C_OP_JCMP_RZF:
		tmp=FR2CI_MakeJCmpOpDC(img, FR2C_TY_F,
			FR2C_OPMODE_RR, op, s, 0, ip);
			break;
	case FR2C_OP_JCMP_RZD:
		tmp=FR2CI_MakeJCmpOpDC(img, FR2C_TY_D,
			FR2C_OPMODE_RR, op, s, 0, ip);
			break;
	case FR2C_OP_JCMP_RZP:
		tmp=FR2CI_MakeJCmpOpDC(img, FR2C_TY_P,
			FR2C_OPMODE_RR, op, s, 0, ip);
			break;

	case FR2C_OP_JCMP_AZI:
		tmp=FR2CI_MakeJCmpOpLC(img, FR2C_TY_I,
			FR2C_OPMODE_RA, op, s, 0, ip);
			break;
	case FR2C_OP_JCMP_AZL:
		tmp=FR2CI_MakeJCmpOpLC(img, FR2C_TY_L,
			FR2C_OPMODE_RA, op, s, 0, ip);
			break;
	case FR2C_OP_JCMP_AZF:
		tmp=FR2CI_MakeJCmpOpDC(img, FR2C_TY_F,
			FR2C_OPMODE_RA, op, s, 0, ip);
			break;
	case FR2C_OP_JCMP_AZD:
		tmp=FR2CI_MakeJCmpOpDC(img, FR2C_TY_D,
			FR2C_OPMODE_RA, op, s, 0, ip);
			break;
	case FR2C_OP_JCMP_AZP:
		tmp=FR2CI_MakeJCmpOpDC(img, FR2C_TY_P,
			FR2C_OPMODE_RA, op, s, 0, ip);
			break;

	case FR2C_OP_JCMP_LZI:
		tmp=FR2CI_MakeJCmpOpLC(img, FR2C_TY_I,
			FR2C_OPMODE_RL, op, s, 0, ip);
			break;
	case FR2C_OP_JCMP_LZL:
		tmp=FR2CI_MakeJCmpOpLC(img, FR2C_TY_L,
			FR2C_OPMODE_RL, op, s, 0, ip);
			break;
	case FR2C_OP_JCMP_LZF:
		tmp=FR2CI_MakeJCmpOpDC(img, FR2C_TY_F,
			FR2C_OPMODE_RL, op, s, 0, ip);
			break;
	case FR2C_OP_JCMP_LZD:
		tmp=FR2CI_MakeJCmpOpDC(img, FR2C_TY_D,
			FR2C_OPMODE_RL, op, s, 0, ip);
			break;
	case FR2C_OP_JCMP_LZP:
		tmp=FR2CI_MakeJCmpOpDC(img, FR2C_TY_P,
			FR2C_OPMODE_RL, op, s, 0, ip);
			break;
	}

	return(tmp);
}
