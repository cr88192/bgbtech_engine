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

#define OP_SWNJ(OPN, YYN) \
	case FR2C_OPMODE_##YYN: tmp->run=FR2CI_Op_##OPN##_##YYN;

#define OPB_SWNJ(OPN) \
	switch(y) {	\
		OP_SWNJ(OPN, RR)	\
		OP_SWNJ(OPN, RA)	\
		OP_SWNJ(OPN, RL)	\
		OP_SWNJ(OPN, AR)	\
		OP_SWNJ(OPN, AA)	\
		OP_SWNJ(OPN, AL)	\
		OP_SWNJ(OPN, LR)	\
		OP_SWNJ(OPN, LA)	\
		OP_SWNJ(OPN, LL)	}

#define OP_NILDI(OPN, YYN, DN, SN, TN, TYN, TY) \
	void FR2CI_Op_##OPN##_##YYN(FR2CI_Frame *frm, FR2CI_Opcode *op) \
		{ frm->DN[op->d].TY=((TYN*)frm->SN[op->s].p)[frm->TN[op->t].i]; }

#define OPB_NILDI(OPN, OP, TY) \
	OP_NILDI(OPN, RRR, reg, reg, reg, OP, TY) \
	OP_NILDI(OPN, RRA, reg, reg, arg, OP, TY) \
	OP_NILDI(OPN, RRL, reg, reg, loc, OP, TY) \
	OP_NILDI(OPN, RAR, reg, arg, reg, OP, TY) \
	OP_NILDI(OPN, RAA, reg, arg, arg, OP, TY) \
	OP_NILDI(OPN, RAL, reg, arg, loc, OP, TY) \
	OP_NILDI(OPN, RLR, reg, loc, reg, OP, TY) \
	OP_NILDI(OPN, RLA, reg, loc, arg, OP, TY) \
	OP_NILDI(OPN, RLL, reg, loc, loc, OP, TY) \
	OP_NILDI(OPN, ARR, arg, reg, reg, OP, TY) \
	OP_NILDI(OPN, LRR, loc, reg, reg, OP, TY) \
	OP_NILDI(OPN, LRA, loc, reg, arg, OP, TY) \
	OP_NILDI(OPN, LAR, loc, arg, reg, OP, TY) \
	OP_NILDI(OPN, LRL, loc, reg, loc, OP, TY) \
	OP_NILDI(OPN, LLR, loc, loc, reg, OP, TY) \
	OP_NILDI(OPN, LLL, loc, loc, loc, OP, TY)

OPB_NILDI(LoadIndexI, s32, i)
OPB_NILDI(LoadIndexL, s64, l)
OPB_NILDI(LoadIndexF, f32, f)
OPB_NILDI(LoadIndexD, f64, d)
OPB_NILDI(LoadIndexSB, sbyte, i)
OPB_NILDI(LoadIndexUB, byte, i)
OPB_NILDI(LoadIndexSS, s16, i)
OPB_NILDI(LoadIndexUS, u16, i)
OPB_NILDI(LoadIndexP, void*, p)


#define OP_NISTI(OPN, YYN, DN, SN, TN, TYN, TY) \
	void FR2CI_Op_##OPN##_##YYN(FR2CI_Frame *frm, FR2CI_Opcode *op) \
		{ ((TYN*)frm->DN[op->d].p)[frm->SN[op->s].i]=frm->TN[op->t].TY; }

#define OPB_NISTI(OPN, OP, TY) \
	OP_NISTI(OPN, RRR, reg, reg, reg, OP, TY) \
	OP_NISTI(OPN, RRA, reg, reg, arg, OP, TY) \
	OP_NISTI(OPN, RRL, reg, reg, loc, OP, TY) \
	OP_NISTI(OPN, RAR, reg, arg, reg, OP, TY) \
	OP_NISTI(OPN, RAA, reg, arg, arg, OP, TY) \
	OP_NISTI(OPN, RAL, reg, arg, loc, OP, TY) \
	OP_NISTI(OPN, RLR, reg, loc, reg, OP, TY) \
	OP_NISTI(OPN, RLA, reg, loc, arg, OP, TY) \
	OP_NISTI(OPN, RLL, reg, loc, loc, OP, TY) \
	OP_NISTI(OPN, ARR, arg, reg, reg, OP, TY) \
	OP_NISTI(OPN, LRR, loc, reg, reg, OP, TY) \
	OP_NISTI(OPN, LRA, loc, reg, arg, OP, TY) \
	OP_NISTI(OPN, LAR, loc, arg, reg, OP, TY) \
	OP_NISTI(OPN, LRL, loc, reg, loc, OP, TY) \
	OP_NISTI(OPN, LLR, loc, loc, reg, OP, TY) \
	OP_NISTI(OPN, LLL, loc, loc, loc, OP, TY)

OPB_NISTI(StoreIndexI, s32, i)
OPB_NISTI(StoreIndexL, s64, l)
OPB_NISTI(StoreIndexF, f32, f)
OPB_NISTI(StoreIndexD, f64, d)
OPB_NISTI(StoreIndexB, byte, i)
//OPB_NISTI(StoreIndexUB, u8, i)
OPB_NISTI(StoreIndexS, s16, i)
//OPB_NISTI(StoreIndexUS, u16, i)
OPB_NISTI(StoreIndexP, void*, p)


FR2CI_Opcode *FR2CI_MakeOpLoadIndex(
	FR2CI_Image *img,
	int z, int y,
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
		OPB_SWNI(LoadIndexI)
		break;
	case FR2C_TY_L:
		OPB_SWNI(LoadIndexL)
		break;
	case FR2C_TY_F:
		OPB_SWNI(LoadIndexF)
		break;
	case FR2C_TY_D:
		OPB_SWNI(LoadIndexD)
		break;
	case FR2C_TY_SB:
		OPB_SWNI(LoadIndexSB)
		break;
	case FR2C_TY_UB:
		OPB_SWNI(LoadIndexUB)
		break;
	case FR2C_TY_SS:
		OPB_SWNI(LoadIndexSS)
		break;
	case FR2C_TY_US:
		OPB_SWNI(LoadIndexUS)
		break;
	case FR2C_TY_P:
		OPB_SWNI(LoadIndexP)
		break;
	}
	
	if(!tmp->run)
	{
		FR2CI_FreeOpcode(img, tmp);
		return(NULL);
	}
	
	return(tmp);
}

FR2CI_Opcode *FR2CI_MakeOpStoreIndex(
	FR2CI_Image *img,
	int z, int y,
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
		OPB_SWNI(StoreIndexI)
		break;
	case FR2C_TY_L:
		OPB_SWNI(StoreIndexL)
		break;
	case FR2C_TY_F:
		OPB_SWNI(StoreIndexF)
		break;
	case FR2C_TY_D:
		OPB_SWNI(StoreIndexD)
		break;
	case FR2C_TY_SB:
	case FR2C_TY_UB:
		OPB_SWNI(StoreIndexB)
		break;
	case FR2C_TY_SS:
	case FR2C_TY_US:
		OPB_SWNI(StoreIndexS)
		break;
	case FR2C_TY_P:
		OPB_SWNI(StoreIndexP)
		break;
	}
	
	if(!tmp->run)
	{
		FR2CI_FreeOpcode(img, tmp);
		return(NULL);
	}
	
	return(tmp);
}



#define OP_NILDIC(OPN, YYN, DN, SN, TYN, TY) \
	void FR2CI_Op_##OPN##_##YYN(FR2CI_Frame *frm, FR2CI_Opcode *op) \
		{ frm->DN[op->d].TY=((TYN*)frm->SN[op->s].p)[op->t]; }

#define OPB_NILDIC(OPN, OP, TY) \
	OP_NILDIC(OPN, RR, reg, reg, OP, TY) \
	OP_NILDIC(OPN, RA, reg, arg, OP, TY) \
	OP_NILDIC(OPN, RL, reg, loc, OP, TY) \
	OP_NILDIC(OPN, AR, arg, reg, OP, TY) \
	OP_NILDIC(OPN, AA, arg, arg, OP, TY) \
	OP_NILDIC(OPN, AL, arg, loc, OP, TY) \
	OP_NILDIC(OPN, LR, loc, reg, OP, TY) \
	OP_NILDIC(OPN, LA, loc, arg, OP, TY) \
	OP_NILDIC(OPN, LL, loc, loc, OP, TY) \

OPB_NILDIC(LoadIndexIC, s32, i)
OPB_NILDIC(LoadIndexLC, s64, l)
OPB_NILDIC(LoadIndexFC, f32, f)
OPB_NILDIC(LoadIndexDC, f64, d)
OPB_NILDIC(LoadIndexSBC, sbyte, i)
OPB_NILDIC(LoadIndexUBC, byte, i)
OPB_NILDIC(LoadIndexSSC, s16, i)
OPB_NILDIC(LoadIndexUSC, u16, i)
OPB_NILDIC(LoadIndexPC, void*, p)

#define OP_NISTIC(OPN, YYN, DN, TN, TYN, TY) \
	void FR2CI_Op_##OPN##_##YYN(FR2CI_Frame *frm, FR2CI_Opcode *op) \
		{ ((TYN*)frm->DN[op->d].p)[op->s]=frm->TN[op->t].TY; }

#define OPB_NISTIC(OPN, OP, TY) \
	OP_NISTIC(OPN, RR, reg, reg, OP, TY) \
	OP_NISTIC(OPN, RA, reg, arg, OP, TY) \
	OP_NISTIC(OPN, RL, reg, loc, OP, TY) \
	OP_NISTIC(OPN, AR, arg, reg, OP, TY) \
	OP_NISTIC(OPN, AA, arg, arg, OP, TY) \
	OP_NISTIC(OPN, AL, arg, loc, OP, TY) \
	OP_NISTIC(OPN, LR, loc, reg, OP, TY) \
	OP_NISTIC(OPN, LA, loc, arg, OP, TY) \
	OP_NISTIC(OPN, LL, loc, loc, OP, TY) \

OPB_NISTIC(StoreIndexIC, s32, i)
OPB_NISTIC(StoreIndexLC, s64, l)
OPB_NISTIC(StoreIndexFC, f32, f)
OPB_NISTIC(StoreIndexDC, f64, d)
OPB_NISTIC(StoreIndexBC, byte, i)
//OPB_NISTIC(StoreIndexUBC, u8, i)
OPB_NISTIC(StoreIndexSC, s16, i)
//OPB_NISTIC(StoreIndexUSC, u16, i)
OPB_NISTIC(StoreIndexPC, void*, p)

FR2CI_Opcode *FR2CI_MakeOpLoadIndexC(
	FR2CI_Image *img,
	int z, int y,
	int d, int s, int imm)
{
	FR2CI_Opcode *tmp;
	
	tmp=FR2CI_AllocOpcode(img);
	tmp->d=d;
	tmp->s=s;
	tmp->t=imm;
	
	switch(z)
	{
	case FR2C_TY_I:
		OPB_SWNJ(LoadIndexIC)
		break;
	case FR2C_TY_L:
		OPB_SWNJ(LoadIndexLC)
		break;
	case FR2C_TY_F:
		OPB_SWNJ(LoadIndexFC)
		break;
	case FR2C_TY_D:
		OPB_SWNJ(LoadIndexDC)
		break;
	case FR2C_TY_SB:
		OPB_SWNJ(LoadIndexSBC)
		break;
	case FR2C_TY_UB:
		OPB_SWNJ(LoadIndexUBC)
		break;
	case FR2C_TY_SS:
		OPB_SWNJ(LoadIndexSSC)
		break;
	case FR2C_TY_US:
		OPB_SWNJ(LoadIndexUSC)
		break;
	case FR2C_TY_P:
		OPB_SWNJ(LoadIndexPC)
		break;
	}
	
	if(!tmp->run)
	{
		FR2CI_FreeOpcode(img, tmp);
		return(NULL);
	}
	
	return(tmp);
}

FR2CI_Opcode *FR2CI_MakeOpStoreIndexC(
	FR2CI_Image *img,
	int z, int y,
	int d, int s, int imm)
{
	FR2CI_Opcode *tmp;
	
	tmp=FR2CI_AllocOpcode(img);
	tmp->d=d;
	tmp->s=imm;
	tmp->t=s;
	
	switch(z)
	{
	case FR2C_TY_I:
		OPB_SWNJ(StoreIndexIC)
		break;
	case FR2C_TY_L:
		OPB_SWNJ(StoreIndexLC)
		break;
	case FR2C_TY_F:
		OPB_SWNJ(StoreIndexFC)
		break;
	case FR2C_TY_D:
		OPB_SWNJ(StoreIndexDC)
		break;
	case FR2C_TY_SB:
	case FR2C_TY_UB:
		OPB_SWNJ(StoreIndexBC)
		break;
	case FR2C_TY_SS:
	case FR2C_TY_US:
		OPB_SWNJ(StoreIndexSC)
		break;
	case FR2C_TY_P:
		OPB_SWNJ(StoreIndexPC)
		break;
	}
	
	if(!tmp->run)
	{
		FR2CI_FreeOpcode(img, tmp);
		return(NULL);
	}
	
	return(tmp);
}


#define OP_NILEA(OPN, YYN, DN, SN, TN, TYN, TY) \
	void FR2CI_Op_##OPN##_##YYN(FR2CI_Frame *frm, FR2CI_Opcode *op) \
		{ frm->DN[op->d].p=&(((TYN*)frm->SN[op->s].p)[frm->TN[op->t].i]); }

#define OPB_NILEA(OPN, OP, TY) \
	OP_NILEA(OPN, RRR, reg, reg, reg, OP, TY) \
	OP_NILEA(OPN, RRA, reg, reg, arg, OP, TY) \
	OP_NILEA(OPN, RRL, reg, reg, loc, OP, TY) \
	OP_NILEA(OPN, RAR, reg, arg, reg, OP, TY) \
	OP_NILEA(OPN, RAA, reg, arg, arg, OP, TY) \
	OP_NILEA(OPN, RAL, reg, arg, loc, OP, TY) \
	OP_NILEA(OPN, RLR, reg, loc, reg, OP, TY) \
	OP_NILEA(OPN, RLA, reg, loc, arg, OP, TY) \
	OP_NILEA(OPN, RLL, reg, loc, loc, OP, TY) \
	OP_NILEA(OPN, ARR, arg, reg, reg, OP, TY) \
	OP_NILEA(OPN, LRR, loc, reg, reg, OP, TY) \
	OP_NILEA(OPN, LRA, loc, reg, arg, OP, TY) \
	OP_NILEA(OPN, LAR, loc, arg, reg, OP, TY) \
	OP_NILEA(OPN, LRL, loc, reg, loc, OP, TY) \
	OP_NILEA(OPN, LLR, loc, loc, reg, OP, TY) \
	OP_NILEA(OPN, LLL, loc, loc, loc, OP, TY)

OPB_NILEA(LeaI, s32, i)
OPB_NILEA(LeaL, s64, l)
OPB_NILEA(LeaF, f32, f)
OPB_NILEA(LeaD, f64, d)
OPB_NILEA(LeaB, byte, i)
OPB_NILEA(LeaS, s16, i)
OPB_NILEA(LeaP, void*, p)


#define OP_NILEAC(OPN, YYN, DN, SN, TYN, TY) \
	void FR2CI_Op_##OPN##_##YYN(FR2CI_Frame *frm, FR2CI_Opcode *op) \
		{ frm->DN[op->d].p=&(((TYN*)frm->SN[op->s].p)[op->t]); }

#define OPB_NILEAC(OPN, OP, TY) \
	OP_NILEAC(OPN, RR, reg, reg, OP, TY) \
	OP_NILEAC(OPN, RA, reg, arg, OP, TY) \
	OP_NILEAC(OPN, RL, reg, loc, OP, TY) \
	OP_NILEAC(OPN, AR, arg, reg, OP, TY) \
	OP_NILEAC(OPN, AA, arg, arg, OP, TY) \
	OP_NILEAC(OPN, AL, arg, loc, OP, TY) \
	OP_NILEAC(OPN, LR, loc, reg, OP, TY) \
	OP_NILEAC(OPN, LA, loc, arg, OP, TY) \
	OP_NILEAC(OPN, LL, loc, loc, OP, TY) \

OPB_NILEAC(LeaIC, s32, i)
OPB_NILEAC(LeaLC, s64, l)
OPB_NILEAC(LeaFC, f32, f)
OPB_NILEAC(LeaDC, f64, d)
OPB_NILEAC(LeaBC, byte, i)
OPB_NILEAC(LeaSC, s16, i)
OPB_NILEAC(LeaPC, void*, p)


FR2CI_Opcode *FR2CI_MakeOpLea(
	FR2CI_Image *img,
	int z, int y,
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
		OPB_SWNI(LeaI)
		break;
	case FR2C_TY_L:
		OPB_SWNI(LeaL)
		break;
	case FR2C_TY_F:
		OPB_SWNI(LeaF)
		break;
	case FR2C_TY_D:
		OPB_SWNI(LeaD)
		break;
	case FR2C_TY_SB:
	case FR2C_TY_UB:
		OPB_SWNI(LeaB)
		break;
	case FR2C_TY_SS:
	case FR2C_TY_US:
		OPB_SWNI(LeaS)
		break;
	case FR2C_TY_P:
		OPB_SWNI(LeaP)
		break;
	}
	
	if(!tmp->run)
	{
		FR2CI_FreeOpcode(img, tmp);
		return(NULL);
	}
	
	return(tmp);
}

FR2CI_Opcode *FR2CI_MakeOpLeaC(
	FR2CI_Image *img,
	int z, int y,
	int d, int s, int imm)
{
	FR2CI_Opcode *tmp;
	
	tmp=FR2CI_AllocOpcode(img);
	tmp->d=d;
	tmp->s=s;
	tmp->t=imm;
	
	switch(z)
	{
	case FR2C_TY_I:
		OPB_SWNJ(LeaIC)
		break;
	case FR2C_TY_L:
		OPB_SWNJ(LeaLC)
		break;
	case FR2C_TY_F:
		OPB_SWNJ(LeaFC)
		break;
	case FR2C_TY_D:
		OPB_SWNJ(LeaDC)
		break;
	case FR2C_TY_SB:
	case FR2C_TY_UB:
		OPB_SWNJ(LeaBC)
		break;
	case FR2C_TY_SS:
	case FR2C_TY_US:
		OPB_SWNJ(LeaSC)
		break;
	case FR2C_TY_P:
		OPB_SWNJ(LeaPC)
		break;
	}
	
	if(!tmp->run)
	{
		FR2CI_FreeOpcode(img, tmp);
		return(NULL);
	}
	
	return(tmp);
}
