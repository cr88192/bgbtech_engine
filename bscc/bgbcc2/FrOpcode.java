package fricore;

public abstract class FrOpcode
{
	public abstract void run(FrVMFrame ctx);

	public static FrOpcode decodeOpcode(
		FrVMImage img, FrBytePointer cs, FrBytePointer cse)
	{
		int z, y, o;
		int d, s, t;
		int i, j, k, l, m;
	
		i=cs.getub(0);
		switch(i)
		{
		case FR2C_OP_BINOP_I:
			cs.ninc(); j=cs.getub_npp(); k=cs.getub_npp();
			return FrBinOp.makeBinOp(FR2C_TY_I, FR2C_OPMODE_RRR,
				(j>>4)&15, j&15, (k>>4)&15, k&15);
		case FR2C_OP_BINOP_L:
			cs.ninc(); j=cs.getub_npp(); k=cs.getub_npp();
			return FrBinOp.makeBinOp(FR2C_TY_L, FR2C_OPMODE_RRR,
				(j>>4)&15, j&15, (k>>4)&15, k&15);
		case FR2C_OP_BINOP_F:
			cs.ninc(); j=cs.getub_npp(); k=cs.getub_npp();
			return FrBinOp.makeBinOp(FR2C_TY_F, FR2C_OPMODE_RRR,
				(j>>4)&15, j&15, (k>>4)&15, k&15);
		case FR2C_OP_BINOP_D:
			cs.ninc(); j=cs.getub_npp(); k=cs.getub_npp();
			return FrBinOp.makeBinOp(FR2C_TY_D, FR2C_OPMODE_RRR,
				(j>>4)&15, j&15, (k>>4)&15, k&15);
		case FR2C_OP_BINOP2:
			cs.ninc(); j=cs.getub_npp(); k=cs.getub_npp(); l=cs.getub_npp();
			return FrBinOp.makeBinOp((j>>4)&15, j&15,
				(k>>4)&15, k&15, (l>>4)&15, l&15);
		case FR2C_OP_BINOPW_I:
			cs.ninc(); j=cs.getub_npp();	d=cs.getub_npp();
			s=cs.getub_npp();	t=cs.getub_npp();
			return FrBinOp.makeBinOp(FR2C_TY_I, j&15,
				(j>>4)&15, d, s, t);
		case FR2C_OP_BINOPW_L:
			cs.ninc(); j=cs.getub_npp();	d=cs.getub_npp();
			s=cs.getub_npp();	t=cs.getub_npp();
			return FrBinOp.makeBinOp(FR2C_TY_L, j&15,
				(j>>4)&15, d, s, t);
		case FR2C_OP_BINOPW_F:
			cs.ninc(); j=cs.getub_npp();	d=cs.getub_npp();
			s=cs.getub_npp();	t=cs.getub_npp();
			return FrBinOp.makeBinOp(FR2C_TY_F, j&15,
				(j>>4)&15, d, s, t);
		case FR2C_OP_BINOPW_D:
			cs.ninc(); j=cs.getub_npp();	d=cs.getub_npp();
			s=cs.getub_npp();	t=cs.getub_npp();
			return FrBinOp.makeBinOp(FR2C_TY_D, j&15,
				(j>>4)&15, d, s, t);

		case FR2C_OP_BINOP_IC:
			cs.ninc(); j=cs.getub_npp();
			k=cs.getub_npp(); l=cs.getub_npp();
			return FrBinOpImm.makeBinOpImm(FR2C_TY_I, FR2C_OPMODE_RR,
				(j>>4)&15, j&15, (k>>4)&15,
				img.expandConst12Int(((k&15)<<8)|l));
		case FR2C_OP_BINOP_LC:
			cs.ninc(); j=cs.getub_npp();
			k=cs.getub_npp(); l=cs.getub_npp();
			return FrBinOpImm.makeBinOpImm(FR2C_TY_L, FR2C_OPMODE_RR,
				(j>>4)&15, j&15, (k>>4)&15,
				img.expandConst12Int(((k&15)<<8)|l));

		case FR2C_OP_BINOP_FC:
			cs.ninc(); j=cs.getub_npp();
			k=cs.getub_npp(); l=cs.getub_npp();
			return FrBinOpImm.makeBinOpImm(FR2C_TY_F, FR2C_OPMODE_RR,
				(j>>4)&15, j&15, (k>>4)&15,
				(double)img.getLiteralFloat(((k&15)<<8)|l));
		case FR2C_OP_BINOP_DC:
			cs.ninc(); j=cs.getub_npp();
			k=cs.getub_npp(); l=cs.getub_npp();
			return FrBinOpImm.makeBinOpImm(FR2C_TY_D, FR2C_OPMODE_RR,
				(j>>4)&15, j&15, (k>>4)&15,
				img.getLiteralDouble(((k&15)<<8)|l));

		case FR2C_OP_BINOP2_C:
			cs.ninc(); j=cs.getub_npp(); k=cs.getub_npp();
			l=cs.getub_npp(); m=cs.getub_npp();
			
			z=(j>>4)&15;	y=j&15;			o=(k>>4)&15;
			d=k&15;			s=(l>>4)&15;	t=((l&15)<<8)|m;
			
			switch(z)
			{
			case FR2C_TY_SB:
			case FR2C_TY_UB:
			case FR2C_TY_SS:
			case FR2C_TY_US:
			case FR2C_TY_I:
				return FrBinOpImm.makeBinOpImm(z, y, o, d, s,
					(long)img.getLiteralInt(t));
			case FR2C_TY_L:
				return FrBinOpImm.makeBinOpImm(z, y, o, d, s,
					img.getLiteralLong(t));
			case FR2C_TY_F:
				return FrBinOpImm.makeBinOpImm(z, y, o, d, s,
					(double)img.getLiteralFloat(t));
			case FR2C_TY_D:
				return FrBinOpImm.makeBinOpImm(z, y, o, d, s,
					img.getLiteralDouble(t));
			default:
				break;
			}
			break;

		case FR2C_OP_BINOPW_IC:
			cs.ninc(); j=cs.getub_npp();	d=cs.getub_npp();
			s=cs.getub_npp();
//			t=cs.getub_npp();
			t=cs.getuw24_npp();
			return FrBinOpImm.makeBinOpImm(FR2C_TY_I, j&15,
				(j>>4)&15, d, s, (long)img.getLiteralInt(t));
		case FR2C_OP_BINOPW_LC:
			cs.ninc(); j=cs.getub_npp();	d=cs.getub_npp();
			s=cs.getub_npp();
//			t=cs.getub_npp();
			t=cs.getuw24_npp();
			return FrBinOpImm.makeBinOpImm(FR2C_TY_L, j&15,
				(j>>4)&15, d, s, (long)img.getLiteralLong(t));
		case FR2C_OP_BINOPW_FC:
			cs.ninc(); j=cs.getub_npp();	d=cs.getub_npp();
			s=cs.getub_npp();		t=cs.getuw24_npp();
			return FrBinOpImm.makeBinOpImm(FR2C_TY_F, j&15,
				(j>>4)&15, d, s, (double)img.getLiteralFloat(t));
		case FR2C_OP_BINOPW_DC:
			cs.ninc(); j=cs.getub_npp();	d=cs.getub_npp();
			s=cs.getub_npp();		t=cs.getuw24_npp();
			return FrBinOpImm.makeBinOpImm(FR2C_TY_D, j&15,
				(j>>4)&15, d, s, (double)img.getLiteralDouble(t));

		case FR2C_OP_UNOP_I:
			cs.ninc(); j=cs.getub_npp(); k=cs.getub_npp();
			return FrUnOp.makeUnOp(FR2C_TY_I, j&15,
				(j>>4)&15, (k>>4)&15, k&15);
		case FR2C_OP_UNOP_L:
			cs.ninc(); j=cs.getub_npp(); k=cs.getub_npp();
			return FrUnOp.makeUnOp(FR2C_TY_L, j&15,
				(j>>4)&15, (k>>4)&15, k&15);
		case FR2C_OP_UNOP_F:
			cs.ninc(); j=cs.getub_npp(); k=cs.getub_npp();
			return FrUnOp.makeUnOp(FR2C_TY_F, j&15,
				(j>>4)&15, (k>>4)&15, k&15);
		case FR2C_OP_UNOP_D:
			cs.ninc(); j=cs.getub_npp(); k=cs.getub_npp();
			return FrUnOp.makeUnOp(FR2C_TY_D, j&15,
				(j>>4)&15, (k>>4)&15, k&15);

		case FR2C_OP_UNOP_IC:
			cs.ninc(); j=cs.getub_npp();
			k=cs.getub_npp(); l=cs.getub_npp();
			if(((j>>4)&15)<8)
			{	return FrUnOpImm.makeUnOpImm(FR2C_TY_I, j&15,
					(j>>4)&15, (k>>4)&15,
					img.expandConst12Int(((k&15)<<8)|l)); }
			else
			{	return FrUnOpImm.makeUnOpImm(FR2C_TY_I, j&15,
					(j>>4)&15, (k>>4)&15,
					img.getLiteralInt(((k&15)<<8)|l));	}
		case FR2C_OP_UNOP_LC:
			cs.ninc(); j=cs.getub_npp();
			k=cs.getub_npp(); l=cs.getub_npp();
			if(((j>>4)&15)<8)
			{	return FrUnOpImm.makeUnOpImm(FR2C_TY_L, j&15,
					(j>>4)&15, (k>>4)&15,
					img.expandConst12Int(((k&15)<<8)|l)); }
			else
			{	return FrUnOpImm.makeUnOpImm(FR2C_TY_L, j&15,
					(j>>4)&15, (k>>4)&15,
					img.getLiteralLong(((k&15)<<8)|l));	}

		case FR2C_OP_UNOP_FC:
			cs.ninc(); j=cs.getub_npp();
			k=cs.getub_npp(); l=cs.getub_npp();
			if(((j>>4)&15)<8)
			{	return FrUnOpImm.makeUnOpImm(FR2C_TY_F, j&15,
					(j>>4)&15, (k>>4)&15,
					img.expandConst12Float(((k&15)<<8)|l)); }
			else
			{	return FrUnOpImm.makeUnOpImm(FR2C_TY_F, j&15,
					(j>>4)&15, (k>>4)&15,
					img.getLiteralFloat(((k&15)<<8)|l));	}
		case FR2C_OP_UNOP_DC:
			cs.ninc(); j=cs.getub_npp();
			k=cs.getub_npp(); l=cs.getub_npp();
			if(((j>>4)&15)<8)
			{	return FrUnOpImm.makeUnOpImm(FR2C_TY_D, j&15,
					(j>>4)&15, (k>>4)&15,
					img.expandConst12Float(((k&15)<<8)|l)); }
			else
			{	return FrUnOpImm.makeUnOpImm(FR2C_TY_D, j&15,
					(j>>4)&15, (k>>4)&15,
					img.getLiteralDouble(((k&15)<<8)|l));	}

		case FR2C_OP_CMP_I:
			cs.ninc(); j=cs.getub_npp(); k=cs.getub_npp();
			return FrCmpOp.makeCmpOp(FR2C_TY_I, FR2C_OPMODE_RRR,
				(j>>4)&15, j&15, (k>>4)&15, k&15);
		case FR2C_OP_CMP_L:
			cs.ninc(); j=cs.getub_npp(); k=cs.getub_npp();
			return FrCmpOp.makeCmpOp(FR2C_TY_L, FR2C_OPMODE_RRR,
				(j>>4)&15, j&15, (k>>4)&15, k&15);
		case FR2C_OP_CMP_F:
			cs.ninc(); j=cs.getub_npp(); k=cs.getub_npp();
			return FrCmpOp.makeCmpOp(FR2C_TY_F, FR2C_OPMODE_RRR,
				(j>>4)&15, j&15, (k>>4)&15, k&15);
		case FR2C_OP_CMP_D:
			cs.ninc(); j=cs.getub_npp(); k=cs.getub_npp();
			return FrCmpOp.makeCmpOp(FR2C_TY_D, FR2C_OPMODE_RRR,
				(j>>4)&15, j&15, (k>>4)&15, k&15);

		case FR2C_OP_CMP_IC:
			cs.ninc(); j=cs.getub_npp();
			k=cs.getub_npp(); l=cs.getub_npp();
			if(((j>>4)&15)<8)
			{	return FrCmpOpImm.makeCmpOpImm(FR2C_TY_I, FR2C_OPMODE_RR,
					(j>>4)&15, j&15, (k>>4)&15,
					img.expandConst12Int(((k&15)<<8)|l)); }
			else
			{	return FrCmpOpImm.makeCmpOpImm(FR2C_TY_I, FR2C_OPMODE_RR,
					(j>>4)&15, j&15, (k>>4)&15,
					img.getLiteralInt(((k&15)<<8)|l));	}
		case FR2C_OP_CMP_LC:
			cs.ninc(); j=cs.getub_npp();
			k=cs.getub_npp(); l=cs.getub_npp();
			if(((j>>4)&15)<8)
			{	return FrCmpOpImm.makeCmpOpImm(FR2C_TY_L, FR2C_OPMODE_RR,
					(j>>4)&15, j&15, (k>>4)&15,
					img.expandConst12Int(((k&15)<<8)|l)); }
			else
			{	return FrCmpOpImm.makeCmpOpImm(FR2C_TY_L, FR2C_OPMODE_RR,
					(j>>4)&15, j&15, (k>>4)&15,
					img.getLiteralLong(((k&15)<<8)|l));	}

		case FR2C_OP_CMP_FC:
			cs.ninc(); j=cs.getub_npp();
			k=cs.getub_npp(); l=cs.getub_npp();
			if(((j>>4)&15)<8)
			{	return FrCmpOpImm.makeCmpOpImm(FR2C_TY_F, FR2C_OPMODE_RR,
					(j>>4)&15, j&15, (k>>4)&15,
					img.expandConst12Float(((k&15)<<8)|l)); }
			else
			{	return FrCmpOpImm.makeCmpOpImm(FR2C_TY_F, FR2C_OPMODE_RR,
					(j>>4)&15, j&15, (k>>4)&15,
					img.getLiteralFloat(((k&15)<<8)|l));	}
		case FR2C_OP_CMP_DC:
			cs.ninc(); j=cs.getub_npp();
			k=cs.getub_npp(); l=cs.getub_npp();
			if(((j>>4)&15)<8)
			{	return FrCmpOpImm.makeCmpOpImm(FR2C_TY_D, j&15,
					(j>>4)&15, j&15, (k>>4)&15,
					img.expandConst12Float(((k&15)<<8)|l)); }
			else
			{	return FrCmpOpImm.makeCmpOpImm(FR2C_TY_D, j&15,
					(j>>4)&15, j&15, (k>>4)&15,
					img.getLiteralDouble(((k&15)<<8)|l));	}

		case FR2C_OP_LDCONST:
			cs.ninc(); j=cs.getub_npp(); k=cs.getus_npp();
			return FrOpLdConst.makeOpLdConst(img, (j>>4)&15, j&15, k);
		case FR2C_OP_LDCONSTW:
			cs.ninc(); j=cs.getub_npp();
			k=cs.getub_npp(); l=cs.getui_npp();
			return FrOpLdConst.makeOpLdConst(img, (j>>4)&15, k, l);

		case FR2C_OP_CONV:
			cs.ninc(); j=cs.getub_npp(); k=cs.getub_npp();
			return FrOpConv.makeOpConv((j>>4)&15, j&15, (k>>4)&15, k&15);

		case FR2C_OP_CONV_I:
		case FR2C_OP_CONV_L:
			cs.ninc(); j=cs.getub_npp(); k=cs.getub_npp();
			return FrOpConvI.makeOpConvI((j>>4)&15, j&15, (k>>4)&15, k&15);

		case FR2C_OP_SHRMASK_I:
			cs.ninc(); j=cs.getub_npp();
			k=cs.getub_npp(); l=cs.getub_npp();
			m=((k&15)<<8)|l;
			return FrOpShrMask.makeOpShrMask(FR2C_TY_I, (j>>4)&15,
				j&15, (k>>4)&15, (m>>6)&63, m&63);
		case FR2C_OP_SHRMASK_L:
			cs.ninc(); j=cs.getub_npp();
			k=cs.getub_npp(); l=cs.getub_npp();
			m=((k&15)<<8)|l;
			return FrOpShrMask.makeOpShrMask(FR2C_TY_L, (j>>4)&15,
				j&15, (k>>4)&15, (m>>6)&63, m&63);
		case FR2C_OP_SHLMASK_I:
			cs.ninc(); j=cs.getub_npp();
			k=cs.getub_npp(); l=cs.getub_npp();
			m=((k&15)<<8)|l;
			return FrOpShrMask.makeOpShlMask(FR2C_TY_I, (j>>4)&15,
				j&15, (k>>4)&15, (m>>6)&63, m&63);
		case FR2C_OP_SHLMASK_L:
			cs.ninc(); j=cs.getub_npp();
			k=cs.getub_npp(); l=cs.getub_npp();
			m=((k&15)<<8)|l;
			return FrOpShrMask.makeOpShlMask(FR2C_TY_L, (j>>4)&15,
				j&15, (k>>4)&15, (m>>6)&63, m&63);

		case FR2C_OP_LOADINDEX_I:
			cs.ninc(); j=cs.getub_npp(); k=cs.getub_npp();
			return FrOpLoadIndex.makeOpLoadIndex(FR2C_TY_I,
				(j>>4)&15, j&15, (k>>4)&15, k&15);
		case FR2C_OP_LOADINDEX_L:
			cs.ninc(); j=cs.getub_npp(); k=cs.getub_npp();
			return FrOpLoadIndex.makeOpLoadIndex(FR2C_TY_L,
				(j>>4)&15, j&15, (k>>4)&15, k&15);
		case FR2C_OP_LOADINDEX_F:
			cs.ninc(); j=cs.getub_npp(); k=cs.getub_npp();
			return FrOpLoadIndex.makeOpLoadIndex(FR2C_TY_F,
				(j>>4)&15, j&15, (k>>4)&15, k&15);
		case FR2C_OP_LOADINDEX_D:
			cs.ninc(); j=cs.getub_npp(); k=cs.getub_npp();
			return FrOpLoadIndex.makeOpLoadIndex(FR2C_TY_D,
				(j>>4)&15, j&15, (k>>4)&15, k&15);
		case FR2C_OP_LOADINDEX_SB:
			cs.ninc(); j=cs.getub_npp(); k=cs.getub_npp();
			return FrOpLoadIndex.makeOpLoadIndex(FR2C_TY_SB,
				(j>>4)&15, j&15, (k>>4)&15, k&15);
		case FR2C_OP_LOADINDEX_UB:
			cs.ninc(); j=cs.getub_npp(); k=cs.getub_npp();
			return FrOpLoadIndex.makeOpLoadIndex(FR2C_TY_UB,
				(j>>4)&15, j&15, (k>>4)&15, k&15);
		case FR2C_OP_LOADINDEX_SS:
			cs.ninc(); j=cs.getub_npp(); k=cs.getub_npp();
			return FrOpLoadIndex.makeOpLoadIndex(FR2C_TY_SS,
				(j>>4)&15, j&15, (k>>4)&15, k&15);
		case FR2C_OP_LOADINDEX_US:
			cs.ninc(); j=cs.getub_npp(); k=cs.getub_npp();
			return FrOpLoadIndex.makeOpLoadIndex(FR2C_TY_US,
				(j>>4)&15, j&15, (k>>4)&15, k&15);
		case FR2C_OP_LOADINDEX_P:
			cs.ninc(); j=cs.getub_npp(); k=cs.getub_npp();
			return FrOpLoadIndex.makeOpLoadIndex(FR2C_TY_P,
				(j>>4)&15, j&15, (k>>4)&15, k&15);

		default:
			return null;
		}
		return null;
	}

	public static final int FR2C_OP_BINOP_I			=0x00;
	public static final int FR2C_OP_BINOP_L			=0x01;
	public static final int FR2C_OP_BINOP_F			=0x02;
	public static final int FR2C_OP_BINOP_D			=0x03;
	public static final int FR2C_OP_BINOP_IC			=0x04;
	public static final int FR2C_OP_BINOP_LC			=0x05;
	public static final int FR2C_OP_BINOP_FC			=0x06;
	public static final int FR2C_OP_BINOP_DC			=0x07;
	public static final int FR2C_OP_UNOP_I			=0x08;
	public static final int FR2C_OP_UNOP_L			=0x09;
	public static final int FR2C_OP_UNOP_F			=0x0A;
	public static final int FR2C_OP_UNOP_D			=0x0B;
	public static final int FR2C_OP_UNOP_IC			=0x0C;
	public static final int FR2C_OP_UNOP_LC			=0x0D;
	public static final int FR2C_OP_UNOP_FC			=0x0E;
	public static final int FR2C_OP_UNOP_DC			=0x0F;
	public static final int FR2C_OP_CMP_I				=0x10;
	public static final int FR2C_OP_CMP_L				=0x11;
	public static final int FR2C_OP_CMP_F				=0x12;
	public static final int FR2C_OP_CMP_D				=0x13;
	public static final int FR2C_OP_CMP_IC			=0x14;
	public static final int FR2C_OP_CMP_LC			=0x15;
	public static final int FR2C_OP_CMP_FC			=0x16;
	public static final int FR2C_OP_CMP_DC			=0x17;
	public static final int FR2C_OP_JCMP_I			=0x18;
	public static final int FR2C_OP_JCMP_L			=0x19;
	public static final int FR2C_OP_JCMP_F			=0x1A;
	public static final int FR2C_OP_JCMP_D			=0x1B;
	public static final int FR2C_OP_JCMP_IC			=0x1C;
	public static final int FR2C_OP_JCMP_LC			=0x1D;
	public static final int FR2C_OP_JCMP_FC			=0x1E;
	public static final int FR2C_OP_JCMP_DC			=0x1F;
	public static final int FR2C_OP_JMP				=0x20;
	public static final int FR2C_OP_JMPW				=0x21;
	public static final int FR2C_OP_LDLVAR			=0x22;
	public static final int FR2C_OP_STLVAR			=0x23;
	public static final int FR2C_OP_LDGVAR			=0x24;
	public static final int FR2C_OP_STGVAR			=0x25;
	public static final int FR2C_OP_LDCONST			=0x26;
	public static final int FR2C_OP_LDCONSTW			=0x27;
	public static final int FR2C_OP_CONV				=0x28;
	public static final int FR2C_OP_LDAGVAR			=0x29;
	public static final int FR2C_OP_CONV_I			=0x2A;
	public static final int FR2C_OP_CONV_L			=0x2B;
	public static final int FR2C_OP_SHRMASK_I			=0x2C;
	public static final int FR2C_OP_SHRMASK_L			=0x2D;
	public static final int FR2C_OP_SHLMASK_I			=0x2E;
	public static final int FR2C_OP_SHLMASK_L			=0x2F;
	public static final int FR2C_OP_JCMP_RZI			=0x30;	//Jump-Compare Register Zero Int
	public static final int FR2C_OP_JCMP_RZL			=0x31;	//Jump-Compare Register Zero Long
	public static final int FR2C_OP_JCMP_RZF			=0x32;	//Jump-Compare Register Zero Float
	public static final int FR2C_OP_JCMP_RZD			=0x33;	//Jump-Compare Register Zero Double
	public static final int FR2C_OP_JCMP_LZI			=0x34;	//Jump-Compare Local Zero Int
	public static final int FR2C_OP_JCMP_LZL			=0x35;	//Jump-Compare Local Zero Long
	public static final int FR2C_OP_JCMP_LZF			=0x36;	//Jump-Compare Local Zero Float
	public static final int FR2C_OP_JCMP_LZD			=0x37;	//Jump-Compare Local Zero Double
	public static final int FR2C_OP_JCMP_AZI			=0x38;	//Jump-Compare Arg Zero Int
	public static final int FR2C_OP_JCMP_AZL			=0x39;	//Jump-Compare Arg Zero Long
	public static final int FR2C_OP_JCMP_AZF			=0x3A;	//Jump-Compare Arg Zero Float
	public static final int FR2C_OP_JCMP_AZD			=0x3B;	//Jump-Compare Arg Zero Double
	public static final int FR2C_OP_JCMP_RZP			=0x3C;	//Jump-Compare Register Zero Pointer
	public static final int FR2C_OP_JCMP_AZP			=0x3D;	//Jump-Compare Local Zero Pointer
	public static final int FR2C_OP_JCMP_LZP			=0x3E;	//Jump-Compare Arg Zero Pointer
	public static final int FR2C_OP_JCMP__ZP			=0x3F;	//Jump-Compare - Zero Pointer
	public static final int FR2C_OP_LOADINDEX_I		=0x40;
	public static final int FR2C_OP_LOADINDEX_L		=0x41;
	public static final int FR2C_OP_LOADINDEX_F		=0x42;
	public static final int FR2C_OP_LOADINDEX_D		=0x43;
	public static final int FR2C_OP_LOADINDEX_SB		=0x44;
	public static final int FR2C_OP_LOADINDEX_UB		=0x45;
	public static final int FR2C_OP_LOADINDEX_SS		=0x46;
	public static final int FR2C_OP_LOADINDEX_US		=0x47;
	public static final int FR2C_OP_LOADINDEX_P		=0x48;
	public static final int FR2C_OP_LOADINDEX_IC		=0x49;
	public static final int FR2C_OP_LOADINDEX_LC		=0x4A;
	public static final int FR2C_OP_LOADINDEX_FC		=0x4B;
	public static final int FR2C_OP_LOADINDEX_DC		=0x4C;
	public static final int FR2C_OP_LOADINDEX_SBC		=0x4D;
	public static final int FR2C_OP_LOADINDEX_SSC		=0x4E;
	public static final int FR2C_OP_LOADINDEX_PC		=0x4F;
	public static final int FR2C_OP_STOREINDEX_I		=0x50;
	public static final int FR2C_OP_STOREINDEX_L		=0x51;
	public static final int FR2C_OP_STOREINDEX_F		=0x52;
	public static final int FR2C_OP_STOREINDEX_D		=0x53;
	public static final int FR2C_OP_STOREINDEX_SB		=0x54;
	public static final int FR2C_OP_STOREINDEX_UB		=0x55;
	public static final int FR2C_OP_STOREINDEX_SS		=0x56;
	public static final int FR2C_OP_STOREINDEX_US		=0x57;
	public static final int FR2C_OP_STOREINDEX_P		=0x58;
	public static final int FR2C_OP_STOREINDEX_IC		=0x59;
	public static final int FR2C_OP_STOREINDEX_LC		=0x5A;
	public static final int FR2C_OP_STOREINDEX_FC		=0x5B;
	public static final int FR2C_OP_STOREINDEX_DC		=0x5C;
	public static final int FR2C_OP_STOREINDEX_BC		=0x5D;
	public static final int FR2C_OP_STOREINDEX_SC		=0x5E;
	public static final int FR2C_OP_STOREINDEX_PC		=0x5F;
	public static final int FR2C_OP_LEA_I				=0x60;
	public static final int FR2C_OP_LEA_L				=0x61;
	public static final int FR2C_OP_LEA_F				=0x62;
	public static final int FR2C_OP_LEA_D				=0x63;
	public static final int FR2C_OP_LEA_B				=0x64;
	public static final int FR2C_OP_LEA_S				=0x65;
	public static final int FR2C_OP_LEA_P				=0x66;
	public static final int FR2C_OP_LEALVAR_V			=0x67;
	public static final int FR2C_OP_LEA_IC			=0x68;
	public static final int FR2C_OP_LEA_LC			=0x69;
	public static final int FR2C_OP_LEA_FC			=0x6A;
	public static final int FR2C_OP_LEA_DC			=0x6B;
	public static final int FR2C_OP_LEA_BC			=0x6C;
	public static final int FR2C_OP_LEA_SC			=0x6D;
	public static final int FR2C_OP_LEA_PC			=0x6E;
	public static final int FR2C_OP_LEALVAR_VC		=0x6F;
	public static final int FR2C_OP_CALL				=0x70;
	public static final int FR2C_OP_CALLP				=0x71;
	public static final int FR2C_OP_RET_V				=0x72;
	public static final int FR2C_OP_RET				=0x73;
	public static final int FR2C_OP_RETW				=0x74;
	public static final int FR2C_OP_LOADSLOT			=0x75;
	public static final int FR2C_OP_STORESLOT			=0x76;

	public static final int FR2C_OP_BINOP2			=0x80;
	public static final int FR2C_OP_BINOP2_C			=0x81;
	public static final int FR2C_OP_UNOP2				=0x82;
	public static final int FR2C_OP_UNOP2_C			=0x83;
	public static final int FR2C_OP_LOADINDEX_UBC		=0x84;
	public static final int FR2C_OP_LOADINDEX_USC		=0x85;

	public static final int FR2C_OP_BINOPW_I			=0x0100;
	public static final int FR2C_OP_BINOPW_L			=0x0101;
	public static final int FR2C_OP_BINOPW_F			=0x0102;
	public static final int FR2C_OP_BINOPW_D			=0x0103;
	public static final int FR2C_OP_BINOPW_IC			=0x0104;
	public static final int FR2C_OP_BINOPW_LC			=0x0105;
	public static final int FR2C_OP_BINOPW_FC			=0x0106;
	public static final int FR2C_OP_BINOPW_DC			=0x0107;
	public static final int FR2C_OP_UNOPW_I			=0x0108;
	public static final int FR2C_OP_UNOPW_L			=0x0109;
	public static final int FR2C_OP_UNOPW_F			=0x010A;
	public static final int FR2C_OP_UNOPW_D			=0x010B;
	public static final int FR2C_OP_UNOPW_IC			=0x010C;
	public static final int FR2C_OP_UNOPW_LC			=0x010D;
	public static final int FR2C_OP_UNOPW_FC			=0x010E;
	public static final int FR2C_OP_UNOPW_DC			=0x010F;
	public static final int FR2C_OP_CMPW_I			=0x0110;
	public static final int FR2C_OP_CMPW_L			=0x0111;
	public static final int FR2C_OP_CMPW_F			=0x0112;
	public static final int FR2C_OP_CMPW_D			=0x0113;
	public static final int FR2C_OP_CMPW_IC			=0x0114;
	public static final int FR2C_OP_CMPW_LC			=0x0115;
	public static final int FR2C_OP_CMPW_FC			=0x0116;
	public static final int FR2C_OP_CMPW_DC			=0x0117;
	public static final int FR2C_OP_JCMPW_I			=0x0118;
	public static final int FR2C_OP_JCMPW_L			=0x0119;
	public static final int FR2C_OP_JCMPW_F			=0x011A;
	public static final int FR2C_OP_JCMPW_D			=0x011B;
	public static final int FR2C_OP_JCMPW_IC			=0x011C;
	public static final int FR2C_OP_JCMPW_LC			=0x011D;
	public static final int FR2C_OP_JCMPW_FC			=0x011E;
	public static final int FR2C_OP_JCMPW_DC			=0x011F;


	public static final int FR2C_BINOP_ADD			=0x00;
	public static final int FR2C_BINOP_SUB			=0x01;
	public static final int FR2C_BINOP_MUL			=0x02;
	public static final int FR2C_BINOP_DIV			=0x03;
	public static final int FR2C_BINOP_MOD			=0x04;
	public static final int FR2C_BINOP_AND			=0x05;
	public static final int FR2C_BINOP_OR				=0x06;
	public static final int FR2C_BINOP_XOR			=0x07;
	public static final int FR2C_BINOP_SHL			=0x08;
	public static final int FR2C_BINOP_SHR			=0x09;
	public static final int FR2C_BINOP_SHRR			=0x0A;
	public static final int FR2C_BINOP_MULH			=0x0B;
	public static final int FR2C_BINOP_UMULH			=0x0C;
	public static final int FR2C_BINOP_UDIV			=0x0D;

	public static final int FR2C_UNOP_MOV				=0x00;
	public static final int FR2C_UNOP_NEG				=0x01;
	public static final int FR2C_UNOP_NOT				=0x02;
	public static final int FR2C_UNOP_LNOT			=0x03;
	public static final int FR2C_UNOP_INC				=0x04;
	public static final int FR2C_UNOP_DEC				=0x05;

	public static final int FR2C_CMP_EQ				=0x00;
	public static final int FR2C_CMP_NE				=0x01;
	public static final int FR2C_CMP_LT				=0x02;
	public static final int FR2C_CMP_GT				=0x03;
	public static final int FR2C_CMP_LE				=0x04;
	public static final int FR2C_CMP_GE				=0x05;
	public static final int FR2C_CMP_AL				=0x06;
	public static final int FR2C_CMP_NV				=0x07;

	public static final int FR2C_OPMODE_RR			=0x00;
	public static final int FR2C_OPMODE_RA			=0x01;
	public static final int FR2C_OPMODE_RL			=0x02;
	public static final int FR2C_OPMODE_AR			=0x03;
	public static final int FR2C_OPMODE_AA			=0x04;
	public static final int FR2C_OPMODE_AL			=0x05;
	public static final int FR2C_OPMODE_LR			=0x06;
	public static final int FR2C_OPMODE_LA			=0x07;
	public static final int FR2C_OPMODE_LL			=0x08;
	public static final int FR2C_OPMODE_RRR			=0x00;
	public static final int FR2C_OPMODE_RRA			=0x01;
	public static final int FR2C_OPMODE_RRL			=0x02;
	public static final int FR2C_OPMODE_RAR			=0x03;
	public static final int FR2C_OPMODE_RAA			=0x04;
	public static final int FR2C_OPMODE_RAL			=0x05;
	public static final int FR2C_OPMODE_RLR			=0x06;
	public static final int FR2C_OPMODE_RLA			=0x07;
	public static final int FR2C_OPMODE_RLL			=0x08;
	public static final int FR2C_OPMODE_ARR			=0x09;
	public static final int FR2C_OPMODE_LRR			=0x0A;
	public static final int FR2C_OPMODE_LRA			=0x0B;
	public static final int FR2C_OPMODE_LAR			=0x0C;
	public static final int FR2C_OPMODE_LRL			=0x0D;
	public static final int FR2C_OPMODE_LLR			=0x0E;
	public static final int FR2C_OPMODE_LLL			=0x0F;

	public static final int FR2C_OPCONV_I2L			=0x00;	//Int->Long
	public static final int FR2C_OPCONV_I2F			=0x01;	//Int->Float
	public static final int FR2C_OPCONV_I2D			=0x02;	//Int->Double
	public static final int FR2C_OPCONV_I2P			=0x03;	//Int->Pointer
	public static final int FR2C_OPCONV_L2I			=0x04;	//Long->Int
	public static final int FR2C_OPCONV_L2F			=0x05;	//Long->Float
	public static final int FR2C_OPCONV_L2D			=0x06;	//Long->Double
	public static final int FR2C_OPCONV_L2P			=0x07;	//Long->Pointer
	public static final int FR2C_OPCONV_F2I			=0x08;	//Float->Int
	public static final int FR2C_OPCONV_F2L			=0x09;	//Float->Long
	public static final int FR2C_OPCONV_F2D			=0x0A;	//Float->Double
	public static final int FR2C_OPCONV_D2I			=0x0B;	//Double->Int
	public static final int FR2C_OPCONV_D2L			=0x0C;	//Double->Long
	public static final int FR2C_OPCONV_D2F			=0x0D;	//Double->Float
	public static final int FR2C_OPCONV_P2I			=0x0E;	//Pointer->Int
	public static final int FR2C_OPCONV_P2L			=0x0F;	//Pointer->Long

	public static final int FR2C_CONVI_SXB			=0x00;
	public static final int FR2C_CONVI_ZXB			=0x01;
	public static final int FR2C_CONVI_SXS			=0x02;
	public static final int FR2C_CONVI_ZXS			=0x03;
	public static final int FR2C_CONVI_SXI			=0x04;
	public static final int FR2C_CONVI_ZXI			=0x05;
	public static final int FR2C_CONVI_SXB2			=0x06;
	public static final int FR2C_CONVI_ZXB2			=0x07;
	public static final int FR2C_CONVI_SXS2			=0x08;
	public static final int FR2C_CONVI_ZXS2			=0x09;
	public static final int FR2C_CONVI_SXB3			=0x0A;
	public static final int FR2C_CONVI_ZXB3			=0x0B;
	public static final int FR2C_CONVI_SXB4			=0x0C;
	public static final int FR2C_CONVI_ZXB4			=0x0D;
	public static final int FR2C_CONVI_SXI2			=0x0E;
	public static final int FR2C_CONVI_ZXI2			=0x0F;

	public static final int FR2C_JMPMODE_A16			=0x01;
	public static final int FR2C_JMPMODE_A32			=0x02;

	public static final int FR2C_JMPMODE_LI12			=0x08;
	public static final int FR2C_JMPMODE_LJ12			=0x09;
	public static final int FR2C_JMPMODE_LF12			=0x0A;
	public static final int FR2C_JMPMODE_LD12			=0x0B;

	public static final int FR2C_TY_I				=0x00;	//Int
	public static final int FR2C_TY_L				=0x01;	//Long (64-bit)
	public static final int FR2C_TY_F				=0x02;	//Float
	public static final int FR2C_TY_D				=0x03;	//Double
	public static final int FR2C_TY_P				=0x04;	//Pointer (abstract)
	public static final int FR2C_TY_V				=0x05;	//Void
	public static final int FR2C_TY_S				=0x06;	//Struct / Union
	public static final int FR2C_TY_NL			=0x07;	//Native Long (32/64 bit)
	public static final int FR2C_TY_SB			=0x08;	//Signed Byte
	public static final int FR2C_TY_UB			=0x09;	//Unsigned Byte
	public static final int FR2C_TY_SS			=0x0A;	//Signed Short
	public static final int FR2C_TY_US			=0x0B;	//Unsigned Short
	public static final int FR2C_TY_UI			=0x0C;	//Unsigned Int
	public static final int FR2C_TY_UL			=0x0D;	//Unsigned Long (64-bit)
	public static final int FR2C_TY_UNL			=0x0E;
	public static final int FR2C_TY_UNDEF_I		=0x0F;	//Undefined (Implicit Int)

	public static final int FR2C_OPMODE2_IR			=0x00;
	public static final int FR2C_OPMODE2_IA			=0x01;
	public static final int FR2C_OPMODE2_IL			=0x02;
	public static final int FR2C_OPMODE2_LR			=0x03;
	public static final int FR2C_OPMODE2_LA			=0x04;
	public static final int FR2C_OPMODE2_LL			=0x05;
	public static final int FR2C_OPMODE2_FR			=0x06;
	public static final int FR2C_OPMODE2_FA			=0x07;
	public static final int FR2C_OPMODE2_FL			=0x08;
	public static final int FR2C_OPMODE2_DR			=0x09;
	public static final int FR2C_OPMODE2_DA			=0x0A;
	public static final int FR2C_OPMODE2_DL			=0x0B;
	public static final int FR2C_OPMODE2_PR			=0x0C;
	public static final int FR2C_OPMODE2_PA			=0x0D;
	public static final int FR2C_OPMODE2_PL			=0x0E;

}
