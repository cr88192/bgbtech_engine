package fricore;

public abstract class FrOpLea extends FrOpcode
{
	public static FrOpcode makeOpLea(
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
				return new LeaOp2_RRRI(
					getOprLeaI(z), d, s, t);
			case FR2C_OPMODE_RRA:
				return new LeaOp2_RRAI(
					getOprLeaI(z), d, s, t);
			case FR2C_OPMODE_RRL:
				return new LeaOp2_RRLI(
					getOprLeaI(z), d, s, t);

			case FR2C_OPMODE_RAR:
				return new LeaOp2_RARI(
					getOprLeaI(z), d, s, t);
			case FR2C_OPMODE_RAA:
				return new LeaOp2_RAAI(
					getOprLeaI(z), d, s, t);
			case FR2C_OPMODE_RAL:
				return new LeaOp2_RALI(
					getOprLeaI(z), d, s, t);

			case FR2C_OPMODE_RLR:
				return new LeaOp2_RLRI(
					getOprLeaI(z), d, s, t);
			case FR2C_OPMODE_RLA:
				return new LeaOp2_RLAI(
					getOprLeaI(z), d, s, t);
			case FR2C_OPMODE_RLL:
				return new LeaOp2_RLLI(
					getOprLeaI(z), d, s, t);

			case FR2C_OPMODE_ARR:
				return new LeaOp2_ARRI(
					getOprLeaI(z), d, s, t);

			case FR2C_OPMODE_LRR:
				return new LeaOp2_LRRI(
					getOprLeaI(z), d, s, t);
			case FR2C_OPMODE_LRA:
				return new LeaOp2_LRAI(
					getOprLeaI(z), d, s, t);
			case FR2C_OPMODE_LAR:
				return new LeaOp2_LARI(
					getOprLeaI(z), d, s, t);

			case FR2C_OPMODE_LRL:
				return new LeaOp2_LRLI(
					getOprLeaI(z), d, s, t);
			case FR2C_OPMODE_LLR:
				return new LeaOp2_LLRI(
					getOprLeaI(z), d, s, t);
			case FR2C_OPMODE_LLL:
				return new LeaOp2_LLLI(
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

	private static final class LeaOp2_RRRI extends FrOpcodeDST
	{
		public LeaOp2_RRRI(LeaI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regp[d]=op.op(frm.regp[s], (int)frm.regi[t]); }
		LeaI_Opr op;
	}

	private static final class LeaOp2_RRAI extends FrOpcodeDST
	{
		public LeaOp2_RRAI(LeaI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regp[d]=op.op(frm.regp[s], (int)frm.argi[t]); }
		LeaI_Opr op;
	}

	private static final class LeaOp2_RRLI extends FrOpcodeDST
	{
		public LeaOp2_RRLI(LeaI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regp[d]=op.op(frm.regp[s], (int)frm.loci[t]); }
		LeaI_Opr op;
	}

	private static final class LeaOp2_RARI extends FrOpcodeDST
	{
		public LeaOp2_RARI(LeaI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regp[d]=op.op(frm.argp[s], (int)frm.regi[t]); }
		LeaI_Opr op;
	}

	private static final class LeaOp2_RAAI extends FrOpcodeDST
	{
		public LeaOp2_RAAI(LeaI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regp[d]=op.op(frm.argp[s], (int)frm.argi[t]); }
		LeaI_Opr op;
	}

	private static final class LeaOp2_RALI extends FrOpcodeDST
	{
		public LeaOp2_RALI(LeaI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regp[d]=op.op(frm.argp[s], (int)frm.loci[t]); }
		LeaI_Opr op;
	}

	private static final class LeaOp2_RLRI extends FrOpcodeDST
	{
		public LeaOp2_RLRI(LeaI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regp[d]=op.op(frm.locp[s], (int)frm.regi[t]); }
		LeaI_Opr op;
	}

	private static final class LeaOp2_RLAI extends FrOpcodeDST
	{
		public LeaOp2_RLAI(LeaI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regp[d]=op.op(frm.locp[s], (int)frm.argi[t]); }
		LeaI_Opr op;
	}

	private static final class LeaOp2_RLLI extends FrOpcodeDST
	{
		public LeaOp2_RLLI(LeaI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regp[d]=op.op(frm.locp[s], (int)frm.loci[t]); }
		LeaI_Opr op;
	}

	private static final class LeaOp2_ARRI extends FrOpcodeDST
	{
		public LeaOp2_ARRI(LeaI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.argp[d]=op.op(frm.regp[s], (int)frm.regi[t]); }
		LeaI_Opr op;
	}

	private static final class LeaOp2_LRRI extends FrOpcodeDST
	{
		public LeaOp2_LRRI(LeaI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.locp[d]=op.op(frm.regp[s], (int)frm.regi[t]); }
		LeaI_Opr op;
	}

	private static final class LeaOp2_LRAI extends FrOpcodeDST
	{
		public LeaOp2_LRAI(LeaI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.locp[d]=op.op(frm.regp[s], (int)frm.argi[t]); }
		LeaI_Opr op;
	}

	private static final class LeaOp2_LARI extends FrOpcodeDST
	{
		public LeaOp2_LARI(LeaI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.locp[d]=op.op(frm.argp[s], (int)frm.regi[t]); }
		LeaI_Opr op;
	}

	private static final class LeaOp2_LRLI extends FrOpcodeDST
	{
		public LeaOp2_LRLI(LeaI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.locp[d]=op.op(frm.regp[s], (int)frm.loci[t]); }
		LeaI_Opr op;
	}

	private static final class LeaOp2_LLRI extends FrOpcodeDST
	{
		public LeaOp2_LLRI(LeaI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.locp[d]=op.op(frm.locp[s], (int)frm.regi[t]); }
		LeaI_Opr op;
	}

	private static final class LeaOp2_LLLI extends FrOpcodeDST
	{
		public LeaOp2_LLLI(LeaI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.locp[d]=op.op(frm.locp[s], (int)frm.loci[t]); }
		LeaI_Opr op;
	}
}