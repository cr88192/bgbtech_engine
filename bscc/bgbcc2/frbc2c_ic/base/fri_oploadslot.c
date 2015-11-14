#include <frbc2ci.h>

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

#define OP_NILDIC(OPN, YYN, DN, SN, TYN, TY) \
	void FR2CI_Op_##OPN##_##YYN(FR2CI_Frame *frm, FR2CI_Opcode *op) \
		{ frm->DN[op->d].TY=*(TYN*)(((byte*)frm->SN[op->s].p)+(op->t)); }

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

OPB_NILDIC(LoadSlotIC, s32, i)
OPB_NILDIC(LoadSlotLC, s64, l)
OPB_NILDIC(LoadSlotFC, f32, f)
OPB_NILDIC(LoadSlotDC, f64, d)
OPB_NILDIC(LoadSlotSBC, sbyte, i)
OPB_NILDIC(LoadSlotUBC, byte, i)
OPB_NILDIC(LoadSlotSSC, s16, i)
OPB_NILDIC(LoadSlotUSC, u16, i)
OPB_NILDIC(LoadSlotPC, void*, p)

#define OP_NISTIC(OPN, YYN, DN, TN, TYN, TY) \
	void FR2CI_Op_##OPN##_##YYN(FR2CI_Frame *frm, FR2CI_Opcode *op) \
		{ (*(TYN*)(((byte*)frm->DN[op->d].p)+(op->s)))=frm->TN[op->t].TY; }

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

OPB_NISTIC(StoreSlotIC, s32, i)
OPB_NISTIC(StoreSlotLC, s64, l)
OPB_NISTIC(StoreSlotFC, f32, f)
OPB_NISTIC(StoreSlotDC, f64, d)
OPB_NISTIC(StoreSlotBC, byte, i)
//OPB_NISTIC(StoreSlotUBC, u8, i)
OPB_NISTIC(StoreSlotSC, s16, i)
//OPB_NISTIC(StoreSlotUSC, u16, i)
OPB_NISTIC(StoreSlotPC, void*, p)

FR2CI_Opcode *FR2CI_MakeOpLoadSlot(
	FR2CI_Image *img,
	int z, int y,
	int d, int s, int objid, int fidx)
{
	FR2CI_LiteralInfo *str;
	FR2CI_Opcode *tmp;
	int offs;

	str=FR2CI_DecodeLiteralTableEntry(img, objid);
	if(!str)
		return(NULL);
	if(!str->field)
		return(NULL);
	if((fidx<0) || (fidx>=str->n_field))
		return(NULL);
	offs=str->field[fidx]->vi_offset;

	tmp=FR2CI_AllocOpcode(img);
	tmp->d=d;
	tmp->s=s;
	tmp->t=offs;
	
	switch(z)
	{
	case FR2C_TY_I:
		OPB_SWNJ(LoadSlotIC)
		break;
	case FR2C_TY_L:
		OPB_SWNJ(LoadSlotLC)
		break;
	case FR2C_TY_F:
		OPB_SWNJ(LoadSlotFC)
		break;
	case FR2C_TY_D:
		OPB_SWNJ(LoadSlotDC)
		break;
	case FR2C_TY_SB:
		OPB_SWNJ(LoadSlotSBC)
		break;
	case FR2C_TY_UB:
		OPB_SWNJ(LoadSlotUBC)
		break;
	case FR2C_TY_SS:
		OPB_SWNJ(LoadSlotSSC)
		break;
	case FR2C_TY_US:
		OPB_SWNJ(LoadSlotUSC)
		break;
	case FR2C_TY_P:
		OPB_SWNJ(LoadSlotPC)
		break;
	}
	
	if(!tmp->run)
	{
		FR2CI_FreeOpcode(img, tmp);
		return(NULL);
	}
	
	return(tmp);
}

FR2CI_Opcode *FR2CI_MakeOpStoreSlot(
	FR2CI_Image *img,
	int z, int y,
	int d, int s, int objid, int fidx)
{
	FR2CI_LiteralInfo *str;
	FR2CI_Opcode *tmp;
	int offs;

	str=FR2CI_DecodeLiteralTableEntry(img, objid);
	if(!str)
		return(NULL);
	if(!str->field)
		return(NULL);
	if((fidx<0) || (fidx>=str->n_field))
		return(NULL);
	offs=str->field[fidx]->vi_offset;

	tmp=FR2CI_AllocOpcode(img);
	tmp->d=d;
	tmp->s=offs;
	tmp->t=s;
	
	switch(z)
	{
	case FR2C_TY_I:
		OPB_SWNJ(StoreSlotIC)
		break;
	case FR2C_TY_L:
		OPB_SWNJ(StoreSlotLC)
		break;
	case FR2C_TY_F:
		OPB_SWNJ(StoreSlotFC)
		break;
	case FR2C_TY_D:
		OPB_SWNJ(StoreSlotDC)
		break;
	case FR2C_TY_SB:
	case FR2C_TY_UB:
		OPB_SWNJ(StoreSlotBC)
		break;
	case FR2C_TY_SS:
	case FR2C_TY_US:
		OPB_SWNJ(StoreSlotSC)
		break;
	case FR2C_TY_P:
		OPB_SWNJ(StoreSlotPC)
		break;
	}
	
	if(!tmp->run)
	{
		FR2CI_FreeOpcode(img, tmp);
		return(NULL);
	}
	
	return(tmp);
}



#define OP_NILEAC(OPN, YYN, DN, SN, TYN, TY) \
	void FR2CI_Op_##OPN##_##YYN(FR2CI_Frame *frm, FR2CI_Opcode *op) \
		{ frm->DN[op->d].p=(((byte*)frm->SN[op->s].p)+(op->t)); }

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

OPB_NILEAC(LoadSlotAddrIC, s32, i)
OPB_NILEAC(LoadSlotAddrLC, s64, l)
OPB_NILEAC(LoadSlotAddrFC, f32, f)
OPB_NILEAC(LoadSlotAddrDC, f64, d)
OPB_NILEAC(LoadSlotAddrBC, byte, i)
OPB_NILEAC(LoadSlotAddrSC, s16, i)
OPB_NILEAC(LoadSlotAddrPC, void*, p)


FR2CI_Opcode *FR2CI_MakeOpLoadSlotAddr(
	FR2CI_Image *img,
	int z, int y,
	int d, int s, int objid, int fidx)
{
	FR2CI_LiteralInfo *str;
	FR2CI_Opcode *tmp;
	int offs;

	str=FR2CI_DecodeLiteralTableEntry(img, objid);
	if(!str)
		return(NULL);
	if(!str->field)
		return(NULL);
	if((fidx<0) || (fidx>=str->n_field))
		return(NULL);
	offs=str->field[fidx]->vi_offset;

	tmp=FR2CI_AllocOpcode(img);
	tmp->d=d;
	tmp->s=s;
	tmp->t=offs;
	
	switch(z)
	{
	case FR2C_TY_I:
		OPB_SWNJ(LoadSlotAddrIC)
		break;
	case FR2C_TY_L:
		OPB_SWNJ(LoadSlotAddrLC)
		break;
	case FR2C_TY_F:
		OPB_SWNJ(LoadSlotAddrFC)
		break;
	case FR2C_TY_D:
		OPB_SWNJ(LoadSlotAddrDC)
		break;
	case FR2C_TY_SB:
	case FR2C_TY_UB:
		OPB_SWNJ(LoadSlotAddrBC)
		break;
	case FR2C_TY_SS:
	case FR2C_TY_US:
		OPB_SWNJ(LoadSlotAddrSC)
		break;
	case FR2C_TY_P:
		OPB_SWNJ(LoadSlotAddrPC)
		break;
	}
	
	if(!tmp->run)
	{
		FR2CI_FreeOpcode(img, tmp);
		return(NULL);
	}
	
	return(tmp);
}


#define OP_NICPYO(OPN, YYN, DN, SN) \
	void FR2CI_Op_##OPN##_##YYN(FR2CI_Frame *frm, FR2CI_Opcode *op) \
		{ memcpy(frm->DN[op->d].p, frm->SN[op->s].p, op->t); }

#define OPB_NICPYO(OPN) \
	OP_NICPYO(OPN, RR, reg, reg) \
	OP_NICPYO(OPN, RA, reg, arg) \
	OP_NICPYO(OPN, RL, reg, loc) \
	OP_NICPYO(OPN, AR, arg, reg) \
	OP_NICPYO(OPN, AA, arg, arg) \
	OP_NICPYO(OPN, AL, arg, loc) \
	OP_NICPYO(OPN, LR, loc, reg) \
	OP_NICPYO(OPN, LA, loc, arg) \
	OP_NICPYO(OPN, LL, loc, loc) \

OPB_NICPYO(CopyObj)

FR2CI_Opcode *FR2CI_MakeOpCopyObj(FR2CI_Image *img,
	int y, int d, int s, int c)
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
	tmp->s=s;
	tmp->t=str->st_size;

	OPB_SWNJ(CopyObj)

	if(!tmp->run)
	{
		FR2CI_FreeOpcode(img, tmp);
		return(NULL);
	}

	return(tmp);
}

