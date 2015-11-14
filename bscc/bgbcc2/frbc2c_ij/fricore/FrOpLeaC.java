package fricore;

public abstract class FrOpLeaC extends FrOpcode
{
	public static FrOpcode makeOpLeaC(
		int z, int y, int d, int s, int t)
	{
		switch(z)
		{
		case FR2C_TY_I:		case FR2C_TY_L:
		case FR2C_TY_UI:	case FR2C_TY_UL:
		case FR2C_TY_NL:	case FR2C_TY_UNL:
		case FR2C_TY_SB:	case FR2C_TY_UB:
		case FR2C_TY_SS:	case FR2C_TY_US:
		case FR2C_TY_F:		case FR2C_TY_D:
			switch(y)
			{
			case FR2C_OPMODE_RRR:
				return new LeaOp2_RRI(
					getOprLeaI(z), d, s, t);
			case FR2C_OPMODE_RRA:
				return new LeaOp2_RAI(
					getOprLeaI(z), d, s, t);
			case FR2C_OPMODE_RRL:
				return new LeaOp2_RLI(
					getOprLeaI(z), d, s, t);

			case FR2C_OPMODE_RAR:
				return new LeaOp2_ARI(
					getOprLeaI(z), d, s, t);
			case FR2C_OPMODE_RAA:
				return new LeaOp2_AAI(
					getOprLeaI(z), d, s, t);
			case FR2C_OPMODE_RAL:
				return new LeaOp2_ALI(
					getOprLeaI(z), d, s, t);

			case FR2C_OPMODE_RLR:
				return new LeaOp2_LRI(
					getOprLeaI(z), d, s, t);
			case FR2C_OPMODE_RLA:
				return new LeaOp2_LAI(
					getOprLeaI(z), d, s, t);
			case FR2C_OPMODE_RLL:
				return new LeaOp2_LLI(
					getOprLeaI(z), d, s, t);
			}
		}
		return null;
	}

	private static LeaI_Opr getOprLeaI(int z)
	{
		switch(z)
		{
		case FR2C_TY_I:
			return new LeaI2_SI();
		case FR2C_TY_UI:
			return new LeaI2_UI();
		case FR2C_TY_L:
		case FR2C_TY_NL:
			return new LeaI2_SL();
		case FR2C_TY_UL:
		case FR2C_TY_UNL:
			return new LeaI2_UL();

		case FR2C_TY_SB:
			return new LeaI2_SB();
		case FR2C_TY_UB:
			return new LeaI2_UB();
		case FR2C_TY_SS:
			return new LeaI2_SS();
		case FR2C_TY_US:
			return new LeaI2_US();
		case FR2C_TY_F:
			return new LeaI2_F();
		case FR2C_TY_D:
			return new LeaI2_D();
		}
		return null;
	}

	private static abstract class LeaI_Opr
	{
		public abstract FrPointer op(FrPointer ptr, int idx);
	}

	private static final class LeaI2_SI extends LeaI_Opr
		{ public FrPointer op(FrPointer ptr, int idx)
			{ return(ptr.leai(idx)); } }
	private static final class LeaI2_UI extends LeaI_Opr
		{ public FrPointer op(FrPointer ptr, int idx)
			{ return(ptr.leai(idx)); } }
	private static final class LeaI2_SL extends LeaI_Opr
		{ public FrPointer op(FrPointer ptr, int idx)
			{ return(ptr.leal(idx)); } }
	private static final class LeaI2_UL extends LeaI_Opr
		{ public FrPointer op(FrPointer ptr, int idx)
			{ return(ptr.leal(idx)); } }

	private static final class LeaI2_SB extends LeaI_Opr
		{ public FrPointer op(FrPointer ptr, int idx)
			{ return(ptr.leab(idx)); } }
	private static final class LeaI2_UB extends LeaI_Opr
		{ public FrPointer op(FrPointer ptr, int idx)
			{ return(ptr.leab(idx)); } }
	private static final class LeaI2_SS extends LeaI_Opr
		{ public FrPointer op(FrPointer ptr, int idx)
			{ return(ptr.leas(idx)); } }
	private static final class LeaI2_US extends LeaI_Opr
		{ public FrPointer op(FrPointer ptr, int idx)
			{ return(ptr.leas(idx)); } }

	private static final class LeaI2_F extends LeaI_Opr
		{ public FrPointer op(FrPointer ptr, int idx)
			{ return(ptr.leaf(idx)); } }
	private static final class LeaI2_D extends LeaI_Opr
		{ public FrPointer op(FrPointer ptr, int idx)
			{ return(ptr.lead(idx)); } }


	//Indirect Int

	private static final class LeaOp2_RRI extends FrOpcodeDST
	{
		public LeaOp2_RRI(LeaI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regp[d]=op.op(frm.regp[s], t); }
		LeaI_Opr op;
	}

	private static final class LeaOp2_RAI extends FrOpcodeDST
	{
		public LeaOp2_RAI(LeaI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regp[d]=op.op(frm.regp[s], t); }
		LeaI_Opr op;
	}

	private static final class LeaOp2_RLI extends FrOpcodeDST
	{
		public LeaOp2_RLI(LeaI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regp[d]=op.op(frm.regp[s], t); }
		LeaI_Opr op;
	}

	private static final class LeaOp2_ARI extends FrOpcodeDST
	{
		public LeaOp2_ARI(LeaI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regp[d]=op.op(frm.argp[s], t); }
		LeaI_Opr op;
	}

	private static final class LeaOp2_AAI extends FrOpcodeDST
	{
		public LeaOp2_AAI(LeaI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regp[d]=op.op(frm.argp[s], t); }
		LeaI_Opr op;
	}

	private static final class LeaOp2_ALI extends FrOpcodeDST
	{
		public LeaOp2_ALI(LeaI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regp[d]=op.op(frm.argp[s], t); }
		LeaI_Opr op;
	}

	private static final class LeaOp2_LRI extends FrOpcodeDST
	{
		public LeaOp2_LRI(LeaI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regp[d]=op.op(frm.locp[s], t); }
		LeaI_Opr op;
	}

	private static final class LeaOp2_LAI extends FrOpcodeDST
	{
		public LeaOp2_LAI(LeaI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regp[d]=op.op(frm.locp[s], t); }
		LeaI_Opr op;
	}

	private static final class LeaOp2_LLI extends FrOpcodeDST
	{
		public LeaOp2_LLI(LeaI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regp[d]=op.op(frm.locp[s], t); }
		LeaI_Opr op;
	}
}