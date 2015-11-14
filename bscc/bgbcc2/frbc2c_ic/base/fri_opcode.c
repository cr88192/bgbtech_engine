#include <frbc2ci.h>

static int fr2ci_signext12(int i)
{
	return(i|(((i<<11)>>31)&(~4095)));
}

static float fr2ci_signext12f(int i)
{
	u32 s, e, m, t;
	
	s=(i>>11)<<31;
	e=(i>>6)&31;
	e=e+(127-15);
	e<<=23;
	m=(i&63)<<17;
	t=s|e|m;
	return(*(float *)(&t));
//	return(i|(((i<<11)>>31)&(~4095)));
}

s32 FR2CI_ConstOrLit12I(FR2CI_Image *img, int opr, int imm)
{
	if(opr<8)
		{ return(fr2ci_signext12(imm)); }
	return(FR2CI_GetImageLiteralInt32(img, imm));
}

s64 FR2CI_ConstOrLit12L(FR2CI_Image *img, int opr, int imm)
{
	if(opr<8)
		{ return(fr2ci_signext12(imm)); }
	return(FR2CI_GetImageLiteralInt64(img, imm));
}

double FR2CI_ConstOrLit12F(FR2CI_Image *img, int opr, int imm)
{
	if(opr<8)
		{ return(fr2ci_signext12f(imm)); }
	return(FR2CI_GetImageLiteralFloat32(img, imm));
}

double FR2CI_ConstOrLit12D(FR2CI_Image *img, int opr, int imm)
{
	if(opr<8)
		{ return(fr2ci_signext12f(imm)); }
	return(FR2CI_GetImageLiteralFloat64(img, imm));
}

FR2CI_Opcode *FR2CI_DecodeOpcode2B(FR2CI_Image *img,
	byte **rcs, byte *cse, int pfx)
{
	FR2CI_Opcode *op;
	byte *cs;
	double f, g;
	s64 li;
	int i, j, k;

	op=NULL;
	cs=*rcs;
	switch(pfx|(cs[1]))
	{
	case FR2C_OP_BINOPW_I:
		op=FR2CI_MakeBinOp(img, FR2C_TY_I, cs[2]&15,
			(cs[2]>>4)&15, cs[3], cs[4], cs[5]);
		cs+=6; break;
	case FR2C_OP_BINOPW_L:
		op=FR2CI_MakeBinOp(img, FR2C_TY_L, cs[2]&15,
			(cs[2]>>4)&15, cs[3], cs[4], cs[5]);
		cs+=6; break;
	case FR2C_OP_BINOPW_F:
		op=FR2CI_MakeBinOp(img, FR2C_TY_F, cs[2]&15,
			(cs[2]>>4)&15, cs[3], cs[4], cs[5]);
		cs+=6; break;
	case FR2C_OP_BINOPW_D:
		op=FR2CI_MakeBinOp(img, FR2C_TY_D, cs[2]&15,
			(cs[2]>>4)&15, cs[3], cs[4], cs[5]);
		cs+=6; break;

	case FR2C_OP_BINOPW_IC:
		li=FR2CI_GetImageLiteralInt32(img, (cs[5]<<8)|(cs[6]<<8)|cs[7]);
		op=FR2CI_MakeBinOpCL(img, FR2C_TY_I, cs[2]&15,
			(cs[2]>>4)&15, cs[3], cs[4], li);
		cs+=8; break;
	case FR2C_OP_BINOPW_LC:
		li=FR2CI_GetImageLiteralInt64(img, (cs[5]<<8)|(cs[6]<<8)|cs[7]);
		op=FR2CI_MakeBinOpCL(img, FR2C_TY_L, cs[2]&15,
			(cs[2]>>4)&15, cs[3], cs[4], li);
		cs+=8; break;
	case FR2C_OP_BINOPW_FC:
		f=FR2CI_GetImageLiteralFloat32(img, (cs[5]<<8)|(cs[6]<<8)|cs[7]);
		op=FR2CI_MakeBinOpCD(img, FR2C_TY_F, cs[2]&15,
			(cs[2]>>4)&15, cs[3], cs[4], f);
		cs+=8; break;
	case FR2C_OP_BINOPW_DC:
		f=FR2CI_GetImageLiteralFloat64(img, (cs[5]<<8)|(cs[6]<<8)|cs[7]);
		op=FR2CI_MakeBinOpCD(img, FR2C_TY_D, cs[2]&15,
			(cs[2]>>4)&15, cs[3], cs[4], f);
		cs+=8; break;

	case FR2C_OP_UNOPW_I:
		op=FR2CI_MakeUnOp(img, FR2C_TY_I, cs[2]&15,
			(cs[2]>>4)&15, cs[3], cs[4]);
		cs+=5; break;
	case FR2C_OP_UNOPW_L:
		op=FR2CI_MakeUnOp(img, FR2C_TY_L, cs[2]&15,
			(cs[2]>>4)&15, cs[3], cs[4]);
		cs+=5; break;
	case FR2C_OP_UNOPW_F:
		op=FR2CI_MakeUnOp(img, FR2C_TY_F, cs[2]&15,
			(cs[2]>>4)&15, cs[3], cs[4]);
		cs+=5; break;
	case FR2C_OP_UNOPW_D:
		op=FR2CI_MakeUnOp(img, FR2C_TY_D, cs[2]&15,
			(cs[2]>>4)&15, cs[3], cs[4]);
		cs+=5; break;

	case FR2C_OP_UNOPW_IC:
		li=FR2CI_GetImageLiteralInt32(img, (cs[4]<<8)|(cs[5]<<8)|cs[6]);
		op=FR2CI_MakeUnOpCL(img, FR2C_TY_I, cs[2]&15,
			(cs[2]>>4)&15, cs[3], li);
		cs+=7; break;
	case FR2C_OP_UNOPW_LC:
		li=FR2CI_GetImageLiteralInt32(img, (cs[4]<<8)|(cs[5]<<8)|cs[6]);
		op=FR2CI_MakeUnOpCL(img, FR2C_TY_L, cs[2]&15,
			(cs[2]>>4)&15, cs[3], li);
		cs+=7; break;
	case FR2C_OP_UNOPW_FC:
		li=FR2CI_GetImageLiteralInt32(img, (cs[4]<<8)|(cs[5]<<8)|cs[6]);
		op=FR2CI_MakeUnOpCL(img, FR2C_TY_F, cs[2]&15,
			(cs[2]>>4)&15, cs[3], li);
		cs+=7; break;
	case FR2C_OP_UNOPW_DC:
		li=FR2CI_GetImageLiteralInt32(img, (cs[4]<<8)|(cs[5]<<8)|cs[6]);
		op=FR2CI_MakeUnOpCL(img, FR2C_TY_D, cs[2]&15,
			(cs[2]>>4)&15, cs[3], li);
		cs+=7; break;


	case FR2C_OP_CMPW_I:
		op=FR2CI_MakeCmpOp(img, FR2C_TY_I, cs[2]&15,
			(cs[2]>>4)&15, cs[3], cs[4], cs[5]);
		cs+=6; break;
	case FR2C_OP_CMPW_L:
		op=FR2CI_MakeCmpOp(img, FR2C_TY_L, cs[2]&15,
			(cs[2]>>4)&15, cs[3], cs[4], cs[5]);
		cs+=6; break;
	case FR2C_OP_CMPW_F:
		op=FR2CI_MakeCmpOp(img, FR2C_TY_F, cs[2]&15,
			(cs[2]>>4)&15, cs[3], cs[4], cs[5]);
		cs+=6; break;
	case FR2C_OP_CMPW_D:
		op=FR2CI_MakeCmpOp(img, FR2C_TY_D, cs[2]&15,
			(cs[2]>>4)&15, cs[3], cs[4], cs[5]);
		cs+=6; break;

	case FR2C_OP_CMPW_IC:
		li=FR2CI_GetImageLiteralInt32(img, (cs[5]<<8)|(cs[6]<<8)|cs[7]);
		op=FR2CI_MakeCmpOpCL(img, FR2C_TY_I, cs[2]&15,
			(cs[2]>>4)&15, cs[3], cs[4], li);
		cs+=8; break;
	case FR2C_OP_CMPW_LC:
		li=FR2CI_GetImageLiteralInt64(img, (cs[5]<<8)|(cs[6]<<8)|cs[7]);
		op=FR2CI_MakeCmpOpCL(img, FR2C_TY_L, cs[2]&15,
			(cs[2]>>4)&15, cs[3], cs[4], li);
		cs+=8; break;
	case FR2C_OP_CMPW_FC:
		f=FR2CI_GetImageLiteralFloat32(img, (cs[5]<<8)|(cs[6]<<8)|cs[7]);
		op=FR2CI_MakeCmpOpCD(img, FR2C_TY_F, cs[2]&15,
			(cs[2]>>4)&15, cs[3], cs[4], f);
		cs+=8; break;
	case FR2C_OP_CMPW_DC:
		f=FR2CI_GetImageLiteralFloat64(img, (cs[5]<<8)|(cs[6]<<8)|cs[7]);
		op=FR2CI_MakeCmpOpCD(img, FR2C_TY_D, cs[2]&15,
			(cs[2]>>4)&15, cs[3], cs[4], f);
		cs+=8; break;

	case FR2C_OP_SIZEOFW_I:
		i=(cs[5]<<8)|(cs[6]<<8)|(cs[7]);
		op=FR2CI_MakeOpSizeOf(img, FR2C_OPMODE_RR, cs[2], i);
		cs+=6; break;
	case FR2C_OP_OFFSETOFW_I:
		i=(cs[3]<<8)|(cs[4]<<8)|(cs[5]); j=((cs[6])<<8)|(cs[7]);
		op=FR2CI_MakeOpOffsetOf(img, FR2C_OPMODE_RR, cs[2], i, j);
		cs+=8; break;

	case FR2C_OP_LOADSLOTW:
		i=(cs[5]<<8)|(cs[6]<<8)|(cs[7]); j=((cs[8])<<8)|(cs[9]);
		op=FR2CI_MakeOpLoadSlot(img,
			(cs[2]>>4)&15, cs[2]&15,
			cs[3], cs[4], i, j);
		cs+=10; break;
	case FR2C_OP_STORESLOTW:
		i=(cs[5]<<8)|(cs[6]<<8)|(cs[7]); j=((cs[8])<<8)|(cs[9]);
		op=FR2CI_MakeOpStoreSlot(img,
			(cs[2]>>4)&15, cs[2]&15,
			cs[3], cs[4], i, j);
		cs+=10; break;
	case FR2C_OP_LOADSLOTAW:
		i=(cs[5]<<8)|(cs[6]<<8)|(cs[7]); j=((cs[8])<<8)|(cs[9]);
		op=FR2CI_MakeOpLoadSlotAddr(img,
			(cs[2]>>4)&15, cs[2]&15,
			cs[3], cs[4], i, j);
		cs+=10; break;
	default:
		break;
	}
	
	*rcs=cs;
	return(op);
}

FR2CI_Opcode *FR2CI_DecodeOpcode(FR2CI_Image *img,
	byte **rcs, byte *cse)
{
	FR2CI_Opcode *op;
	byte *cs;
	double f, g;
	s64 li;
	int i, j, k;

	op=NULL;
	cs=*rcs;
	switch(*cs)
	{
	case FR2C_OP_BINOP_I:
		op=FR2CI_MakeBinOp(img, FR2C_TY_I, FR2C_OPMODE_RRR,
			(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, cs[2]&15);
		cs+=3; break;
	case FR2C_OP_BINOP_L:
		op=FR2CI_MakeBinOp(img, FR2C_TY_L, FR2C_OPMODE_RRR,
			(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, cs[2]&15);
		cs+=3; break;
	case FR2C_OP_BINOP_F:
		op=FR2CI_MakeBinOp(img, FR2C_TY_F, FR2C_OPMODE_RRR,
			(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, cs[2]&15);
		cs+=3; break;
	case FR2C_OP_BINOP_D:
		op=FR2CI_MakeBinOp(img, FR2C_TY_D, FR2C_OPMODE_RRR,
			(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, cs[2]&15);
		cs+=3; break;
	
	case FR2C_OP_BINOP_IC:
		i=fr2ci_signext12(((cs[2]&15)<<8)|cs[3]);
		op=FR2CI_MakeBinOpCL(img, FR2C_TY_I, FR2C_OPMODE_RR,
			(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, i);
		cs+=4; break;
	case FR2C_OP_BINOP_LC:
		i=fr2ci_signext12(((cs[2]&15)<<8)|cs[3]);
		op=FR2CI_MakeBinOpCL(img, FR2C_TY_L, FR2C_OPMODE_RR,
			(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, i);
		cs+=4; break;
	case FR2C_OP_BINOP_FC:
		f=FR2CI_GetImageLiteralFloat32(img, ((cs[2]&15)<<8)|cs[3]);
		op=FR2CI_MakeBinOpCD(img, FR2C_TY_F, FR2C_OPMODE_RR,
			(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, f);
		cs+=4; break;
	case FR2C_OP_BINOP_DC:
		f=FR2CI_GetImageLiteralFloat64(img, ((cs[2]&15)<<8)|cs[3]);
		op=FR2CI_MakeBinOpCD(img, FR2C_TY_D, FR2C_OPMODE_RR,
			(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, f);
		cs+=4; break;

	case FR2C_OP_BINOP2:
		op=FR2CI_MakeBinOp(img, (cs[1]>>4)&15, cs[1]&15,
			(cs[2]>>4)&15, cs[2]&15, (cs[3]>>4)&15, cs[3]&15);
		cs+=4; break;
	case FR2C_OP_BINOP2_C:
//		i=fr2ci_signext12(((cs[3]&15)<<8)|cs[4]);
		switch((cs[1]>>4)&15)
		{
		case FR2C_TY_I:
			li=FR2CI_GetImageLiteralInt32(img, ((cs[3]&15)<<8)|cs[4]);
			op=FR2CI_MakeBinOpCL(img, (cs[1]>>4)&15, cs[1]&15,
				(cs[2]>>4)&15, cs[2]&15, (cs[3]>>4)&15, li);
			break;
		case FR2C_TY_L:
			li=FR2CI_GetImageLiteralInt64(img, ((cs[3]&15)<<8)|cs[4]);
			op=FR2CI_MakeBinOpCL(img, (cs[1]>>4)&15, cs[1]&15,
				(cs[2]>>4)&15, cs[2]&15, (cs[3]>>4)&15, li);
			break;
		case FR2C_TY_F:
			f=FR2CI_GetImageLiteralFloat32(img, ((cs[3]&15)<<8)|cs[4]);
			op=FR2CI_MakeBinOpCD(img, (cs[1]>>4)&15, cs[1]&15,
				(cs[2]>>4)&15, cs[2]&15, (cs[3]>>4)&15, f);
			break;
		case FR2C_TY_D:
			f=FR2CI_GetImageLiteralFloat64(img, ((cs[3]&15)<<8)|cs[4]);
			op=FR2CI_MakeBinOpCD(img, (cs[1]>>4)&15, cs[1]&15,
				(cs[2]>>4)&15, cs[2]&15, (cs[3]>>4)&15, f);
			break;
		default:
			li=FR2CI_GetImageLiteralInt32(img, ((cs[3]&15)<<8)|cs[4]);
			op=FR2CI_MakeBinOpCL(img, (cs[1]>>4)&15, cs[1]&15,
				(cs[2]>>4)&15, cs[2]&15, (cs[3]>>4)&15, li);
			break;
		}
		cs+=5; break;

	case FR2C_OP_UNOP_I:
		op=FR2CI_MakeUnOp(img, FR2C_TY_I, cs[1]&15,
			(cs[1]>>4)&15, (cs[2]>>4)&15, cs[2]&15);
		cs+=3; break;
	case FR2C_OP_UNOP_L:
		op=FR2CI_MakeUnOp(img, FR2C_TY_L, cs[1]&15,
			(cs[1]>>4)&15, (cs[2]>>4)&15, cs[2]&15);
		cs+=3; break;
	case FR2C_OP_UNOP_F:
		op=FR2CI_MakeUnOp(img, FR2C_TY_F, cs[1]&15,
			(cs[1]>>4)&15, (cs[2]>>4)&15, cs[2]&15);
		cs+=3; break;
	case FR2C_OP_UNOP_D:
		op=FR2CI_MakeUnOp(img, FR2C_TY_D, cs[1]&15,
			(cs[1]>>4)&15, (cs[2]>>4)&15, cs[2]&15);
		cs+=3; break;

	case FR2C_OP_UNOP_IC:
		i=FR2CI_ConstOrLit12I(img, (cs[1]>>4)&15, ((cs[2]&15)<<8)|cs[3]);
		op=FR2CI_MakeUnOpCL(img, FR2C_TY_I, cs[1]&15,
			(cs[1]>>4)&7, (cs[2]>>4)&15, i);
		cs+=4; break;
	case FR2C_OP_UNOP_LC:
		li=FR2CI_ConstOrLit12L(img, (cs[1]>>4)&15, ((cs[2]&15)<<8)|cs[3]);
		op=FR2CI_MakeUnOpCL(img, FR2C_TY_L, cs[1]&15,
			(cs[1]>>4)&7, (cs[2]>>4)&15, li);
		cs+=4; break;
	case FR2C_OP_UNOP_FC:
		f=FR2CI_ConstOrLit12F(img, (cs[1]>>4)&15, ((cs[2]&15)<<8)|cs[3]);
		op=FR2CI_MakeUnOpCL(img, FR2C_TY_F, cs[1]&15,
			(cs[1]>>4)&7, (cs[2]>>4)&15, f);
		cs+=4; break;
	case FR2C_OP_UNOP_DC:
		f=FR2CI_ConstOrLit12D(img, (cs[1]>>4)&15, ((cs[2]&15)<<8)|cs[3]);
		op=FR2CI_MakeUnOpCL(img, FR2C_TY_D, cs[1]&15,
			(cs[1]>>4)&7, (cs[2]>>4)&15, f);
		cs+=4; break;

	case FR2C_OP_UNOP2:
		op=FR2CI_MakeUnOp(img, (cs[1]>>4)&15, cs[1]&15,
			(cs[2]>>4)&15, cs[2]&15, (cs[3]>>4)&15);
		cs+=4; break;
	case FR2C_OP_UNOP2_C:
		switch((cs[1]>>4)&15)
		{
		case FR2C_TY_I:
			li=FR2CI_GetImageLiteralInt32(img, ((cs[3]&15)<<8)|cs[4]);
			op=FR2CI_MakeUnOpCL(img, (cs[1]>>4)&15, cs[1]&15,
				(cs[2]>>4)&15, cs[2]&15, li);
			break;
		case FR2C_TY_L:
			li=FR2CI_GetImageLiteralInt64(img, ((cs[3]&15)<<8)|cs[4]);
			op=FR2CI_MakeUnOpCL(img, (cs[1]>>4)&15, cs[1]&15,
				(cs[2]>>4)&15, cs[2]&15, li);
			break;
		case FR2C_TY_F:
			f=FR2CI_GetImageLiteralFloat32(img, ((cs[3]&15)<<8)|cs[4]);
			op=FR2CI_MakeUnOpCL(img, (cs[1]>>4)&15, cs[1]&15,
				(cs[2]>>4)&15, cs[2]&15, f);
			break;
		case FR2C_TY_D:
			f=FR2CI_GetImageLiteralFloat64(img, ((cs[3]&15)<<8)|cs[4]);
			op=FR2CI_MakeUnOpCL(img, (cs[1]>>4)&15, cs[1]&15,
				(cs[2]>>4)&15, cs[2]&15, f);
			break;
		default:
			li=FR2CI_GetImageLiteralInt32(img, ((cs[3]&15)<<8)|cs[4]);
			op=FR2CI_MakeUnOpCL(img, (cs[1]>>4)&15, cs[1]&15,
				(cs[2]>>4)&15, cs[2]&15, li);
			break;
		}
//		i=FR2CI_ConstOrLit16I(img, (cs[2]>>4)&15, (cs[3]<<8)|cs[4]);
//		op=FR2CI_MakeUnOpCL(img, (cs[1]>>4)&15, cs[1]&15,
//			(cs[2]>>4)&15, cs[2]&15, i);
		cs+=5; break;

	case FR2C_OP_CMP_I:
		op=FR2CI_MakeCmpOp(img, FR2C_TY_I, FR2C_OPMODE_RRR,
			(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, cs[2]&15);
		cs+=3; break;
	case FR2C_OP_CMP_L:
		op=FR2CI_MakeCmpOp(img, FR2C_TY_L, FR2C_OPMODE_RRR,
			(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, cs[2]&15);
		cs+=3; break;
	case FR2C_OP_CMP_F:
		op=FR2CI_MakeCmpOp(img, FR2C_TY_F, FR2C_OPMODE_RRR,
			(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, cs[2]&15);
		cs+=3; break;
	case FR2C_OP_CMP_D:
		op=FR2CI_MakeCmpOp(img, FR2C_TY_D, FR2C_OPMODE_RRR,
			(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, cs[2]&15);
		cs+=3; break;
	
	case FR2C_OP_CMP_IC:
		i=FR2CI_ConstOrLit12I(img, (cs[1]>>4)&15, ((cs[2]&15)<<8)|cs[3]);
		op=FR2CI_MakeBinOpCL(img, FR2C_TY_I, FR2C_OPMODE_RR,
			(cs[1]>>4)&7, cs[1]&15, (cs[2]>>4)&15, i);
		cs+=4; break;
	case FR2C_OP_CMP_LC:
		li=FR2CI_ConstOrLit12L(img, (cs[1]>>4)&15, ((cs[2]&15)<<8)|cs[3]);
		op=FR2CI_MakeBinOpCL(img, FR2C_TY_L, FR2C_OPMODE_RR,
			(cs[1]>>4)&7, cs[1]&15, (cs[2]>>4)&15, li);
		cs+=4; break;
	case FR2C_OP_CMP_FC:
		f=FR2CI_ConstOrLit12F(img, (cs[1]>>4)&15, ((cs[2]&15)<<8)|cs[3]);
		op=FR2CI_MakeBinOpCD(img, FR2C_TY_F, FR2C_OPMODE_RR,
			(cs[1]>>4)&7, cs[1]&15, (cs[2]>>4)&15, f);
		cs+=4; break;
	case FR2C_OP_CMP_DC:
		f=FR2CI_ConstOrLit12D(img, (cs[1]>>4)&15, ((cs[2]&15)<<8)|cs[3]);
		op=FR2CI_MakeBinOpCD(img, FR2C_TY_D, FR2C_OPMODE_RR,
			(cs[1]>>4)&7, cs[1]&15, (cs[2]>>4)&15, f);
		cs+=4; break;
	
	case FR2C_OP_LDLVAR:
	case FR2C_OP_STLVAR:
	case FR2C_OP_LDGVAR:
	case FR2C_OP_STGVAR:
	case FR2C_OP_LDAGVAR:
	case FR2C_OP_LDCONST:
		op=FR2CI_MakeOpLDVar(img, cs[0],
			(cs[1]>>4)&15, cs[1]&15, (cs[2]<<8)|cs[3]);
		cs+=4; break;
	
	case FR2C_OP_CONV:
		op=FR2CI_MakeOpConv(img,
			(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, cs[2]&15);
		cs+=3; break;
	case FR2C_OP_CONV_I:
		op=FR2CI_MakeOpConvI(img,
			(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, cs[2]&15);
		cs+=3; break;
	case FR2C_OP_CONV_L:
		op=FR2CI_MakeOpConvL(img,
			(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, cs[2]&15);
		cs+=3; break;

	case FR2C_OP_SHRMASK_I:
	case FR2C_OP_SHRMASK_L:
	case FR2C_OP_SHLMASK_I:
	case FR2C_OP_SHLMASK_L:
		i=(((cs[2]&15)<<8)|cs[3]);
		op=FR2CI_MakeOpShrMask(img, cs[0],
			(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, (i>>6)&63, i&63);
		cs+=4; break;

	case FR2C_OP_LOADINDEX_I:
		op=FR2CI_MakeOpLoadIndex(img, FR2C_TY_I,
			(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, cs[2]&15);
		cs+=3; break;
	case FR2C_OP_LOADINDEX_L:
		op=FR2CI_MakeOpLoadIndex(img, FR2C_TY_L,
			(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, cs[2]&15);
		cs+=3; break;
	case FR2C_OP_LOADINDEX_F:
		op=FR2CI_MakeOpLoadIndex(img, FR2C_TY_F,
			(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, cs[2]&15);
		cs+=3; break;
	case FR2C_OP_LOADINDEX_D:
		op=FR2CI_MakeOpLoadIndex(img, FR2C_TY_D,
			(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, cs[2]&15);
		cs+=3; break;
	case FR2C_OP_LOADINDEX_SB:
		op=FR2CI_MakeOpLoadIndex(img, FR2C_TY_SB,
			(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, cs[2]&15);
		cs+=3; break;
	case FR2C_OP_LOADINDEX_UB:
		op=FR2CI_MakeOpLoadIndex(img, FR2C_TY_UB,
			(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, cs[2]&15);
		cs+=3; break;
	case FR2C_OP_LOADINDEX_SS:
		op=FR2CI_MakeOpLoadIndex(img, FR2C_TY_SS,
			(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, cs[2]&15);
		cs+=3; break;
	case FR2C_OP_LOADINDEX_US:
		op=FR2CI_MakeOpLoadIndex(img, FR2C_TY_US,
			(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, cs[2]&15);
		cs+=3; break;
	case FR2C_OP_LOADINDEX_P:
		op=FR2CI_MakeOpLoadIndex(img, FR2C_TY_P,
			(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, cs[2]&15);
		cs+=3; break;
	case FR2C_OP_LOADINDEX_IC:
		i=fr2ci_signext12(((cs[2]&15)<<8)|cs[3]);
		op=FR2CI_MakeOpLoadIndexC(img, FR2C_TY_I,
		(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, i);
		cs+=4; break;
	case FR2C_OP_LOADINDEX_LC:
		i=fr2ci_signext12(((cs[2]&15)<<8)|cs[3]);
		op=FR2CI_MakeOpLoadIndexC(img, FR2C_TY_L,
		(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, i);
		cs+=4; break;
	case FR2C_OP_LOADINDEX_FC:
		i=fr2ci_signext12(((cs[2]&15)<<8)|cs[3]);
		op=FR2CI_MakeOpLoadIndexC(img, FR2C_TY_F,
		(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, i);
		cs+=4; break;
	case FR2C_OP_LOADINDEX_DC:
		i=fr2ci_signext12(((cs[2]&15)<<8)|cs[3]);
		op=FR2CI_MakeOpLoadIndexC(img, FR2C_TY_D,
		(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, i);
		cs+=4; break;
	case FR2C_OP_LOADINDEX_SBC:
		i=fr2ci_signext12(((cs[2]&15)<<8)|cs[3]);
		op=FR2CI_MakeOpLoadIndexC(img, FR2C_TY_SB,
		(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, i);
		cs+=4; break;
	case FR2C_OP_LOADINDEX_UBC:
		i=fr2ci_signext12(((cs[2]&15)<<8)|cs[3]);
		op=FR2CI_MakeOpLoadIndexC(img, FR2C_TY_UB,
		(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, i);
		cs+=4; break;
	case FR2C_OP_LOADINDEX_SSC:
		i=fr2ci_signext12(((cs[2]&15)<<8)|cs[3]);
		op=FR2CI_MakeOpLoadIndexC(img, FR2C_TY_SS,
		(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, i);
		cs+=4; break;
	case FR2C_OP_LOADINDEX_USC:
		i=fr2ci_signext12(((cs[2]&15)<<8)|cs[3]);
		op=FR2CI_MakeOpLoadIndexC(img, FR2C_TY_US,
		(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, i);
		cs+=4; break;
	case FR2C_OP_LOADINDEX_PC:
		i=fr2ci_signext12(((cs[2]&15)<<8)|cs[3]);
		op=FR2CI_MakeOpLoadIndexC(img, FR2C_TY_P,
		(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, i);
		cs+=4; break;

	case FR2C_OP_STOREINDEX_I:
		op=FR2CI_MakeOpStoreIndex(img, FR2C_TY_I,
			(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, cs[2]&15);
		cs+=3; break;
	case FR2C_OP_STOREINDEX_L:
		op=FR2CI_MakeOpStoreIndex(img, FR2C_TY_L,
			(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, cs[2]&15);
		cs+=3; break;
	case FR2C_OP_STOREINDEX_F:
		op=FR2CI_MakeOpStoreIndex(img, FR2C_TY_F,
			(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, cs[2]&15);
		cs+=3; break;
	case FR2C_OP_STOREINDEX_D:
		op=FR2CI_MakeOpStoreIndex(img, FR2C_TY_D,
			(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, cs[2]&15);
		cs+=3; break;
	case FR2C_OP_STOREINDEX_B:
		op=FR2CI_MakeOpStoreIndex(img, FR2C_TY_SB,
			(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, cs[2]&15);
		cs+=3; break;
	case FR2C_OP_STOREINDEX_S:
		op=FR2CI_MakeOpStoreIndexC(img, FR2C_TY_SS,
			(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, cs[2]&15);
		cs+=3; break;
	case FR2C_OP_STOREINDEX_P:
		op=FR2CI_MakeOpStoreIndexC(img, FR2C_TY_P,
			(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, cs[2]&15);
		cs+=3; break;
	case FR2C_OP_STOREINDEX_IC:
		i=fr2ci_signext12(((cs[2]&15)<<8)|cs[3]);
		op=FR2CI_MakeOpStoreIndexC(img, FR2C_TY_I,
		(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, i);
		cs+=4; break;
	case FR2C_OP_STOREINDEX_LC:
		i=fr2ci_signext12(((cs[2]&15)<<8)|cs[3]);
		op=FR2CI_MakeOpStoreIndexC(img, FR2C_TY_L,
		(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, i);
		cs+=4; break;
	case FR2C_OP_STOREINDEX_FC:
		i=fr2ci_signext12(((cs[2]&15)<<8)|cs[3]);
		op=FR2CI_MakeOpStoreIndexC(img, FR2C_TY_F,
		(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, i);
		cs+=4; break;
	case FR2C_OP_STOREINDEX_DC:
		i=fr2ci_signext12(((cs[2]&15)<<8)|cs[3]);
		op=FR2CI_MakeOpStoreIndexC(img, FR2C_TY_D,
		(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, i);
		cs+=4; break;
	case FR2C_OP_STOREINDEX_BC:
		i=fr2ci_signext12(((cs[2]&15)<<8)|cs[3]);
		op=FR2CI_MakeOpStoreIndexC(img, FR2C_TY_SB,
		(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, i);
		cs+=4; break;
	case FR2C_OP_STOREINDEX_SC:
		i=fr2ci_signext12(((cs[2]&15)<<8)|cs[3]);
		op=FR2CI_MakeOpStoreIndexC(img, FR2C_TY_SS,
		(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, i);
		cs+=4; break;
	case FR2C_OP_STOREINDEX_PC:
		i=fr2ci_signext12(((cs[2]&15)<<8)|cs[3]);
		op=FR2CI_MakeOpStoreIndexC(img, FR2C_TY_P,
		(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, i);
		cs+=4; break;

	case FR2C_OP_LEA_I:
		op=FR2CI_MakeOpLea(img, FR2C_TY_I,
			(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, cs[2]&15);
		cs+=3; break;
	case FR2C_OP_LEA_L:
		op=FR2CI_MakeOpLea(img, FR2C_TY_L,
			(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, cs[2]&15);
		cs+=3; break;
	case FR2C_OP_LEA_F:
		op=FR2CI_MakeOpLea(img, FR2C_TY_F,
			(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, cs[2]&15);
		cs+=3; break;
	case FR2C_OP_LEA_D:
		op=FR2CI_MakeOpLea(img, FR2C_TY_D,
			(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, cs[2]&15);
		cs+=3; break;
	case FR2C_OP_LEA_B:
		op=FR2CI_MakeOpLea(img, FR2C_TY_SB,
			(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, cs[2]&15);
		cs+=3; break;
	case FR2C_OP_LEA_S:
		op=FR2CI_MakeOpLea(img, FR2C_TY_SS,
			(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, cs[2]&15);
		cs+=3; break;
	case FR2C_OP_LEA_P:
		op=FR2CI_MakeOpLea(img, FR2C_TY_P,
			(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, cs[2]&15);
		cs+=3; break;
	case FR2C_OP_LEA_IC:
		i=fr2ci_signext12(((cs[2]&15)<<8)|cs[3]);
		op=FR2CI_MakeOpLeaC(img, FR2C_TY_I,
		(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, i);
		cs+=4; break;
	case FR2C_OP_LEA_LC:
		i=fr2ci_signext12(((cs[2]&15)<<8)|cs[3]);
		op=FR2CI_MakeOpLeaC(img, FR2C_TY_L,
		(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, i);
		cs+=4; break;
	case FR2C_OP_LEA_FC:
		i=fr2ci_signext12(((cs[2]&15)<<8)|cs[3]);
		op=FR2CI_MakeOpLeaC(img, FR2C_TY_F,
		(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, i);
		cs+=4; break;
	case FR2C_OP_LEA_DC:
		i=fr2ci_signext12(((cs[2]&15)<<8)|cs[3]);
		op=FR2CI_MakeOpLeaC(img, FR2C_TY_D,
		(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, i);
		cs+=4; break;
	case FR2C_OP_LEA_BC:
		i=fr2ci_signext12(((cs[2]&15)<<8)|cs[3]);
		op=FR2CI_MakeOpLeaC(img, FR2C_TY_SB,
		(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, i);
		cs+=4; break;
	case FR2C_OP_LEA_SC:
		i=fr2ci_signext12(((cs[2]&15)<<8)|cs[3]);
		op=FR2CI_MakeOpLeaC(img, FR2C_TY_SS,
		(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, i);
		cs+=4; break;
	case FR2C_OP_LEA_PC:
		i=fr2ci_signext12(((cs[2]&15)<<8)|cs[3]);
		op=FR2CI_MakeOpLeaC(img, FR2C_TY_P,
		(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, i);
		cs+=4; break;

	case FR2C_OP_LOADSLOT:
		i=(cs[3]<<8)|(cs[4]); j=cs[5];
		op=FR2CI_MakeOpLoadSlot(img,
			(cs[1]>>4)&15, cs[1]&15,
			(cs[2]>>4)&15, cs[2]&15, i, j);
		cs+=6; break;
	case FR2C_OP_STORESLOT:
		i=(cs[3]<<8)|(cs[4]); j=cs[5];
		op=FR2CI_MakeOpStoreSlot(img,
			(cs[1]>>4)&15, cs[1]&15,
			(cs[2]>>4)&15, cs[2]&15, i, j);
		cs+=6; break;
	case FR2C_OP_INITOBJ:
		i=(cs[2]<<8)|(cs[3]);
		op=FR2CI_MakeOpInitObj(img, (cs[1]>>4)&15, cs[1]&15, i);
		cs+=4; break;

	case FR2C_OP_INITARR:
		i=(cs[2]<<8)|(cs[3]);
		op=FR2CI_MakeOpInitArr(img, (cs[1]>>4)&15, cs[1]&15, i);
		cs+=4; break;

	case FR2C_OP_COPYOBJ:
		i=(((cs[2]&15)<<8)|cs[3]);
		op=FR2CI_MakeOpCopyObj(img,
			(cs[1]>>4)&15, cs[1]&15, (cs[2]>>4)&15, i);
		cs+=4; break;
	case FR2C_OP_DROPOBJ:
		i=(((cs[1]&15)<<8)|cs[2]);
		op=FR2CI_MakeOpDropObj(img, FR2C_OPMODE_RR,
			(cs[1]>>4)&15, i);
		cs+=3; break;

	case FR2C_OP_SIZEOF_I:
		i=(cs[2]<<8)|(cs[3]);
		op=FR2CI_MakeOpSizeOf(img, (cs[1]>>4)&15, cs[1]&15, i);
		cs+=4; break;
	case FR2C_OP_OFFSETOF_I:
		i=(cs[2]<<8)|(cs[3]); j=(cs[4]);
		op=FR2CI_MakeOpOffsetOf(img, (cs[1]>>4)&15, cs[1]&15, i, j);
		cs+=5; break;

	case FR2C_OP_NOP:
		op=FR2CI_MakeOpNop(img);
		cs++; break;
	case FR2C_OP_NOP2:
		op=FR2CI_MakeOpNop(img);
		cs+=cs[1];
		break;

	case 0xE0:	case 0xE1:	case 0xE2:	case 0xE3:
	case 0xE4:	case 0xE5:	case 0xE6:	case 0xE7:
	case 0xE8:	case 0xE9:	case 0xEA:	case 0xEB:
	case 0xEC:	case 0xED:	case 0xEE:	case 0xEF:
		op=FR2CI_DecodeOpcode2B(img, &cs, cse, (cs[0]-0xE0)<<8);
		break;
		
	default:
		break;
	}
	
	*rcs=cs;
	return(op);
}

FR2CI_TailOpcode *FR2CI_DecodeTailOpcode(FR2CI_Image *img,
	byte **rcs, byte *cse)
{
	FR2CI_TailOpcode *op;
	byte *cs;
	double f, g;
	s64 li;
	int i, j, k, l;

	op=NULL;
	cs=*rcs;
	switch(*cs)
	{
	case FR2C_OP_JCMP_I:
		i=(short)((cs[3]<<8)|cs[4]);
		op=FR2CI_MakeJCmpOp(img, FR2C_TY_I, cs[1]&15,
			(cs[1]>>4)&15, (cs[2]>>4)&15, cs[2]&15, cs+5+i);
		cs+=5; break;
		break;
	case FR2C_OP_JCMP_L:
		i=(short)((cs[3]<<8)|cs[4]);
		op=FR2CI_MakeJCmpOp(img, FR2C_TY_L, cs[1]&15,
			(cs[1]>>4)&15, (cs[2]>>4)&15, cs[2]&15, cs+5+i);
		cs+=5; break;
		break;
	case FR2C_OP_JCMP_F:
		i=(short)((cs[3]<<8)|cs[4]);
		op=FR2CI_MakeJCmpOp(img, FR2C_TY_F, cs[1]&15,
			(cs[1]>>4)&15, (cs[2]>>4)&15, cs[2]&15, cs+5+i);
		cs+=5; break;
		break;
	case FR2C_OP_JCMP_D:
		i=(short)((cs[3]<<8)|cs[4]);
		op=FR2CI_MakeJCmpOp(img, FR2C_TY_D, cs[1]&15,
			(cs[1]>>4)&15, (cs[2]>>4)&15, cs[2]&15, cs+5+i);
		cs+=5; break;
		break;
	case FR2C_OP_JCMP_P:
		i=(short)((cs[3]<<8)|cs[4]);
		op=FR2CI_MakeJCmpOp(img, FR2C_TY_P, cs[1]&15,
			(cs[1]>>4)&15, (cs[2]>>4)&15, cs[2]&15, cs+5+i);
		cs+=5; break;
		break;

	case FR2C_OP_JCMP_IC:
		i=(short)((cs[4]<<8)|cs[5]);
		li=FR2CI_ConstOrLit12I(img, (cs[1]>>4)&15, ((cs[2]&15)<<8)|cs[3]);
		op=FR2CI_MakeJCmpOpLC(img, FR2C_TY_I, cs[1]&15,
			(cs[1]>>4)&7, (cs[2]>>4)&15, li, cs+6+i);
		cs+=6; break;
	case FR2C_OP_JCMP_LC:
		i=(short)((cs[4]<<8)|cs[5]);
		li=FR2CI_ConstOrLit12L(img, (cs[1]>>4)&15, ((cs[2]&15)<<8)|cs[3]);
		op=FR2CI_MakeJCmpOpLC(img, FR2C_TY_L, cs[1]&15,
			(cs[1]>>4)&7, (cs[2]>>4)&15, li, cs+6+i);
		cs+=6; break;
	case FR2C_OP_JCMP_FC:
		i=(short)((cs[4]<<8)|cs[5]);
		f=FR2CI_ConstOrLit12F(img, (cs[1]>>4)&15, ((cs[2]&15)<<8)|cs[3]);
		op=FR2CI_MakeJCmpOpLC(img, FR2C_TY_F, cs[1]&15,
			(cs[1]>>4)&7, (cs[2]>>4)&15, f, cs+6+i);
		cs+=6; break;
	case FR2C_OP_JCMP_DC:
		i=(short)((cs[4]<<8)|cs[5]);
		f=FR2CI_ConstOrLit12D(img, (cs[1]>>4)&15, ((cs[2]&15)<<8)|cs[3]);
		op=FR2CI_MakeJCmpOpLC(img, FR2C_TY_D, cs[1]&15,
			(cs[1]>>4)&7, (cs[2]>>4)&15, f, cs+6+i);
		cs+=6; break;

	case FR2C_OP_JMP:
		i=(short)((cs[1]<<8)|cs[2]);
		op=FR2CI_MakeOpJmp(img, cs+3+i);
		cs+=3; break;
		break;
	case FR2C_OP_JMPW:
		i=(cs[1]<<24)|(cs[2]<<16)|(cs[3]<<8)|cs[4];
		op=FR2CI_MakeOpJmp(img, cs+5+i);
		cs+=5; break;
		break;

	case FR2C_OP_JCMP_RZI:
	case FR2C_OP_JCMP_RZL:
	case FR2C_OP_JCMP_RZF:
	case FR2C_OP_JCMP_RZD:
	case FR2C_OP_JCMP_AZI:
	case FR2C_OP_JCMP_AZL:
	case FR2C_OP_JCMP_AZF:
	case FR2C_OP_JCMP_AZD:
	case FR2C_OP_JCMP_LZI:
	case FR2C_OP_JCMP_LZL:
	case FR2C_OP_JCMP_LZF:
	case FR2C_OP_JCMP_LZD:
	case FR2C_OP_JCMP_RZP:
	case FR2C_OP_JCMP_AZP:
	case FR2C_OP_JCMP_LZP:
		i=(short)((cs[2]<<8)|cs[3]);
		op=FR2CI_MakeOpJmpZ(img, cs[0],
			(cs[1]>>4)&15, cs[1]&15, cs+4+i);
		cs+=4; break;

	case FR2C_OP_LABEL:
		op=FR2CI_MakeOpJmp(img, cs+1);
		cs+=1; break;
		break;
	case FR2C_OP_LABEL2:
		op=FR2CI_MakeOpJmp(img, cs+4);
		cs+=4; break;
		break;

	case FR2C_OP_CALL:
		i=(cs[1]<<16)|(cs[2]<<8)|(cs[3]);
		j=cs[4]; cs+=5;
		op=FR2CI_DecodeOpCall(img, i, j, &cs);
		break;
	case FR2C_OP_CALLP:
		i=cs[1]; j=cs[2]; cs+=3;
		op=FR2CI_DecodeOpCallP(img, (i>>4)&15, i&15, j, &cs);
		break;

	case FR2C_OP_RET_V:
		op=FR2CI_MakeOpRetV(img);
		cs+=1; break;
		break;
	case FR2C_OP_RET:
		op=FR2CI_MakeOpRet(img, (cs[1]>>4)&15, cs[1]&15);
		cs+=2; break;
		break;

	default:
		break;
	}
	
	*rcs=cs;
	return(op);
}
