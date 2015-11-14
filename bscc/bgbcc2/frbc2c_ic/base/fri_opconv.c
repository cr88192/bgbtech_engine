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

OPB_NJ(ConvI2L, , l, i)
OPB_NJ(ConvI2F, , f, i)
OPB_NJ(ConvI2D, , d, i)
OPB_NJ(ConvI2P, (void *)(nlint), p, i)
OPB_NJ(ConvL2I, , i, l)
OPB_NJ(ConvL2F, , f, l)
OPB_NJ(ConvL2D, , d, l)
OPB_NJ(ConvL2P, (void *)(nlint), p, l)
OPB_NJ(ConvF2I, , i, f)
OPB_NJ(ConvF2L, , l, f)
OPB_NJ(ConvF2D, , d, f)
OPB_NJ(ConvD2I, , i, d)
OPB_NJ(ConvD2L, , l, d)
OPB_NJ(ConvD2F, , f, d)
OPB_NJ(ConvP2I, (nlint), i, p)
OPB_NJ(ConvP2L, (nlint), l, p)


#define OP_NKZX(OPN, YYN, DN, SN, DTY, STY, SHR, MASK) \
	void FR2CI_Op_##OPN##_##YYN(FR2CI_Frame *frm, FR2CI_Opcode *op) \
		{ frm->DN[op->d].DTY=(frm->SN[op->s].STY>>SHR)&MASK; }

#define OP_NKSX(OPN, YYN, DN, SN, DTY, STY, SHR, MASK, SHL) \
	void FR2CI_Op_##OPN##_##YYN(FR2CI_Frame *frm, FR2CI_Opcode *op) \
	{	frm->DN[op->d].DTY=((frm->SN[op->s].STY>>SHR)&MASK)|	\
			(((frm->SN[op->s].STY<<SHL)>>31)&(~MASK));	}

#define OPB_NKZX(OPN, DTY, STY, SHR, MASK) \
	OP_NKZX(OPN, RR, reg, reg, DTY, STY, SHR, MASK) \
	OP_NKZX(OPN, RA, reg, arg, DTY, STY, SHR, MASK) \
	OP_NKZX(OPN, RL, reg, loc, DTY, STY, SHR, MASK) \
	OP_NKZX(OPN, AR, arg, reg, DTY, STY, SHR, MASK) \
	OP_NKZX(OPN, AA, arg, arg, DTY, STY, SHR, MASK) \
	OP_NKZX(OPN, AL, arg, loc, DTY, STY, SHR, MASK) \
	OP_NKZX(OPN, LR, loc, reg, DTY, STY, SHR, MASK) \
	OP_NKZX(OPN, LA, loc, arg, DTY, STY, SHR, MASK) \
	OP_NKZX(OPN, LL, loc, loc, DTY, STY, SHR, MASK) \

#define OPB_NKSX(OPN, DTY, STY, SHR, MASK, SHL) \
	OP_NKSX(OPN, RR, reg, reg, DTY, STY, SHR, MASK, SHL) \
	OP_NKSX(OPN, RA, reg, arg, DTY, STY, SHR, MASK, SHL) \
	OP_NKSX(OPN, RL, reg, loc, DTY, STY, SHR, MASK, SHL) \
	OP_NKSX(OPN, AR, arg, reg, DTY, STY, SHR, MASK, SHL) \
	OP_NKSX(OPN, AA, arg, arg, DTY, STY, SHR, MASK, SHL) \
	OP_NKSX(OPN, AL, arg, loc, DTY, STY, SHR, MASK, SHL) \
	OP_NKSX(OPN, LR, loc, reg, DTY, STY, SHR, MASK, SHL) \
	OP_NKSX(OPN, LA, loc, arg, DTY, STY, SHR, MASK, SHL) \
	OP_NKSX(OPN, LL, loc, loc, DTY, STY, SHR, MASK, SHL) \

OPB_NKSX(ConvISXB, i, i, 0, 255, 24)
OPB_NKZX(ConvIZXB, i, i, 0, 255)
OPB_NKSX(ConvISXS, i, i, 0, 65535, 16)
OPB_NKZX(ConvIZXS, i, i, 0, 65535)
OPB_NKSX(ConvISXI, i, i, 0, 0xFFFFFFFFLL, 0)
OPB_NKZX(ConvIZXI, i, i, 0, 0xFFFFFFFFLL)
OPB_NKSX(ConvISXB2, i, i, 8, 255, 16)
OPB_NKZX(ConvIZXB2, i, i, 8, 255)
OPB_NKSX(ConvISXS2, i, i, 16, 65535, 0)
OPB_NKZX(ConvIZXS2, i, i, 16, 65535)
OPB_NKSX(ConvISXB3, i, i, 16, 255, 8)
OPB_NKZX(ConvIZXB3, i, i, 16, 255)
OPB_NKSX(ConvISXB4, i, i, 24, 255, 0)
OPB_NKZX(ConvIZXB4, i, i, 24, 255)
OPB_NKSX(ConvISXI2, i, i, 0, 0, 0)
OPB_NKZX(ConvIZXI2, i, i, 0, 0)

OPB_NKSX(ConvLSXB, l, l, 0, 255, 56)
OPB_NKZX(ConvLZXB, l, l, 0, 255)
OPB_NKSX(ConvLSXS, l, l, 0, 65535, 48)
OPB_NKZX(ConvLZXS, l, l, 0, 65535)
OPB_NKSX(ConvLSXI, l, l, 0, 0xFFFFFFFFLL, 32)
OPB_NKZX(ConvLZXI, l, l, 0, 0xFFFFFFFFLL)
OPB_NKSX(ConvLSXB2, l, l, 8, 255, 48)
OPB_NKZX(ConvLZXB2, l, l, 8, 255)
OPB_NKSX(ConvLSXS2, l, l, 16, 65535, 32)
OPB_NKZX(ConvLZXS2, l, l, 16, 65535)
OPB_NKSX(ConvLSXB3, l, l, 16, 255, 40)
OPB_NKZX(ConvLZXB3, l, l, 16, 255)
OPB_NKSX(ConvLSXB4, l, l, 24, 255, 32)
OPB_NKZX(ConvLZXB4, l, l, 24, 255)
OPB_NKSX(ConvLSXI2, l, l, 32, 0xFFFFFFFFLL, 0)
OPB_NKZX(ConvLZXI2, l, l, 32, 0xFFFFFFFFLL)

FR2CI_Opcode *FR2CI_MakeOpConv(
	FR2CI_Image *img,
	int z, int y, int d, int s)
{
	FR2CI_Opcode *tmp;
	
	tmp=FR2CI_AllocOpcode(img);
	tmp->d=d;
	tmp->s=s;
//	tmp->t=t;
	
	switch(z)
	{
	case FR2C_OPCONV_I2L:
		OPB_SWNI(ConvI2L)
		break;
	case FR2C_OPCONV_I2F:
		OPB_SWNI(ConvI2F)
		break;
	case FR2C_OPCONV_I2D:
		OPB_SWNI(ConvI2D)
		break;
	case FR2C_OPCONV_I2P:
		OPB_SWNI(ConvI2P)
		break;
	case FR2C_OPCONV_L2I:
		OPB_SWNI(ConvL2I)
		break;
	case FR2C_OPCONV_L2F:
		OPB_SWNI(ConvL2F)
		break;
	case FR2C_OPCONV_L2D:
		OPB_SWNI(ConvL2D)
		break;
	case FR2C_OPCONV_L2P:
		OPB_SWNI(ConvL2P)
		break;
	case FR2C_OPCONV_F2I:
		OPB_SWNI(ConvF2I)
		break;
	case FR2C_OPCONV_F2L:
		OPB_SWNI(ConvF2L)
		break;
	case FR2C_OPCONV_F2D:
		OPB_SWNI(ConvF2D)
		break;
	case FR2C_OPCONV_D2I:
		OPB_SWNI(ConvD2I)
		break;
	case FR2C_OPCONV_D2L:
		OPB_SWNI(ConvD2L)
		break;
	case FR2C_OPCONV_D2F:
		OPB_SWNI(ConvD2F)
		break;
	case FR2C_OPCONV_P2I:
		OPB_SWNI(ConvP2I)
		break;
	case FR2C_OPCONV_P2L:
		OPB_SWNI(ConvP2L)
		break;
	}
	
	if(!tmp->run)
	{
		FR2CI_FreeOpcode(img, tmp);
		return(NULL);
	}
	
	return(tmp);
}

FR2CI_Opcode *FR2CI_MakeOpConvI(
	FR2CI_Image *img,
	int z, int y, int d, int s)
{
	FR2CI_Opcode *tmp;
	
	tmp=FR2CI_AllocOpcode(img);
	tmp->d=d;
	tmp->s=s;
//	tmp->t=t;
	
	switch(z)
	{
	case FR2C_OPCONV2_SXB:
		OPB_SWNI(ConvISXB)
		break;
	case FR2C_OPCONV2_ZXB:
		OPB_SWNI(ConvIZXB)
		break;
	case FR2C_OPCONV2_SXS:
		OPB_SWNI(ConvISXS)
		break;
	case FR2C_OPCONV2_ZXS:
		OPB_SWNI(ConvIZXS)
		break;
	case FR2C_OPCONV2_SXI:
		OPB_SWNI(ConvISXI)
		break;
	case FR2C_OPCONV2_ZXI:
		OPB_SWNI(ConvIZXI)
		break;
	case FR2C_OPCONV2_SXB2:
		OPB_SWNI(ConvISXB2)
		break;
	case FR2C_OPCONV2_ZXB2:
		OPB_SWNI(ConvIZXB2)
		break;
	case FR2C_OPCONV2_SXS2:
		OPB_SWNI(ConvISXS2)
		break;
	case FR2C_OPCONV2_ZXS2:
		OPB_SWNI(ConvIZXS2)
		break;
	case FR2C_OPCONV2_SXB3:
		OPB_SWNI(ConvISXB3)
		break;
	case FR2C_OPCONV2_ZXB3:
		OPB_SWNI(ConvIZXB3)
		break;
	case FR2C_OPCONV2_SXB4:
		OPB_SWNI(ConvISXB4)
		break;
	case FR2C_OPCONV2_ZXB4:
		OPB_SWNI(ConvIZXB4)
		break;
	case FR2C_OPCONV2_SXI2:
		OPB_SWNI(ConvISXI2)
		break;
	case FR2C_OPCONV2_ZXI2:
		OPB_SWNI(ConvIZXI2)
		break;
	}
	
	if(!tmp->run)
	{
		FR2CI_FreeOpcode(img, tmp);
		return(NULL);
	}
	
	return(tmp);
}

FR2CI_Opcode *FR2CI_MakeOpConvL(
	FR2CI_Image *img,
	int z, int y, int d, int s)
{
	FR2CI_Opcode *tmp;
	
	tmp=FR2CI_AllocOpcode(img);
	tmp->d=d;
	tmp->s=s;
//	tmp->t=t;
	
	switch(z)
	{
	case FR2C_OPCONV2_SXB:
		OPB_SWNI(ConvLSXB)
		break;
	case FR2C_OPCONV2_ZXB:
		OPB_SWNI(ConvLZXB)
		break;
	case FR2C_OPCONV2_SXS:
		OPB_SWNI(ConvLSXS)
		break;
	case FR2C_OPCONV2_ZXS:
		OPB_SWNI(ConvLZXS)
		break;
	case FR2C_OPCONV2_SXI:
		OPB_SWNI(ConvLSXI)
		break;
	case FR2C_OPCONV2_ZXI:
		OPB_SWNI(ConvLZXI)
		break;
	case FR2C_OPCONV2_SXB2:
		OPB_SWNI(ConvLSXB2)
		break;
	case FR2C_OPCONV2_ZXB2:
		OPB_SWNI(ConvLZXB2)
		break;
	case FR2C_OPCONV2_SXS2:
		OPB_SWNI(ConvLSXS2)
		break;
	case FR2C_OPCONV2_ZXS2:
		OPB_SWNI(ConvLZXS2)
		break;
	case FR2C_OPCONV2_SXB3:
		OPB_SWNI(ConvLSXB3)
		break;
	case FR2C_OPCONV2_ZXB3:
		OPB_SWNI(ConvLZXB3)
		break;
	case FR2C_OPCONV2_SXB4:
		OPB_SWNI(ConvLSXB4)
		break;
	case FR2C_OPCONV2_ZXB4:
		OPB_SWNI(ConvLZXB4)
		break;
	case FR2C_OPCONV2_SXI2:
		OPB_SWNI(ConvLSXI2)
		break;
	case FR2C_OPCONV2_ZXI2:
		OPB_SWNI(ConvLZXI2)
		break;
	}
	
	if(!tmp->run)
	{
		FR2CI_FreeOpcode(img, tmp);
		return(NULL);
	}
	
	return(tmp);
}
