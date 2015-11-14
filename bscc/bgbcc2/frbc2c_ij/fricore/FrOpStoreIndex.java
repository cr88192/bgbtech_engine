package fricore;

public abstract class FrOpStoreIndex extends FrOpcode
{
	public static FrOpcode makeOpStoreIndex(
		int z, int y, int d, int s, int t)
	{
		switch(z)
		{
		case FR2C_TY_I:		case FR2C_TY_L:
		case FR2C_TY_UI:	case FR2C_TY_UL:
		case FR2C_TY_NL:	case FR2C_TY_UNL:
		case FR2C_TY_SB:	case FR2C_TY_UB:
		case FR2C_TY_SS:	case FR2C_TY_US:
			switch(y)
			{
			case FR2C_OPMODE_RRR:
				return new StoreIndexOp2_RRRI(
					getOprStoreIndexI(z), d, s, t);
			case FR2C_OPMODE_RRA:
				return new StoreIndexOp2_RRAI(
					getOprStoreIndexI(z), d, s, t);
			case FR2C_OPMODE_RRL:
				return new StoreIndexOp2_RRLI(
					getOprStoreIndexI(z), d, s, t);

			case FR2C_OPMODE_RAR:
				return new StoreIndexOp2_RARI(
					getOprStoreIndexI(z), d, s, t);
			case FR2C_OPMODE_RAA:
				return new StoreIndexOp2_RAAI(
					getOprStoreIndexI(z), d, s, t);
			case FR2C_OPMODE_RAL:
				return new StoreIndexOp2_RALI(
					getOprStoreIndexI(z), d, s, t);

			case FR2C_OPMODE_RLR:
				return new StoreIndexOp2_RLRI(
					getOprStoreIndexI(z), d, s, t);
			case FR2C_OPMODE_RLA:
				return new StoreIndexOp2_RLAI(
					getOprStoreIndexI(z), d, s, t);
			case FR2C_OPMODE_RLL:
				return new StoreIndexOp2_RLLI(
					getOprStoreIndexI(z), d, s, t);

			case FR2C_OPMODE_ARR:
				return new StoreIndexOp2_ARRI(
					getOprStoreIndexI(z), d, s, t);

			case FR2C_OPMODE_LRR:
				return new StoreIndexOp2_LRRI(
					getOprStoreIndexI(z), d, s, t);
			case FR2C_OPMODE_LRA:
				return new StoreIndexOp2_LRAI(
					getOprStoreIndexI(z), d, s, t);
			case FR2C_OPMODE_LAR:
				return new StoreIndexOp2_LARI(
					getOprStoreIndexI(z), d, s, t);

			case FR2C_OPMODE_LRL:
				return new StoreIndexOp2_LRLI(
					getOprStoreIndexI(z), d, s, t);
			case FR2C_OPMODE_LLR:
				return new StoreIndexOp2_LLRI(
					getOprStoreIndexI(z), d, s, t);
			case FR2C_OPMODE_LLL:
				return new StoreIndexOp2_LLLI(
					getOprStoreIndexI(z), d, s, t);
			}

		case FR2C_TY_F:
		case FR2C_TY_D:
			switch(y)
			{
			case FR2C_OPMODE_RRR:
				return new StoreIndexOp2_RRRF(
					getOprStoreIndexF(z), d, s, t);
			case FR2C_OPMODE_RRA:
				return new StoreIndexOp2_RRAF(
					getOprStoreIndexF(z), d, s, t);
			case FR2C_OPMODE_RRL:
				return new StoreIndexOp2_RRLF(
					getOprStoreIndexF(z), d, s, t);

			case FR2C_OPMODE_RAR:
				return new StoreIndexOp2_RARF(
					getOprStoreIndexF(z), d, s, t);
			case FR2C_OPMODE_RAA:
				return new StoreIndexOp2_RAAF(
					getOprStoreIndexF(z), d, s, t);
			case FR2C_OPMODE_RAL:
				return new StoreIndexOp2_RALF(
					getOprStoreIndexF(z), d, s, t);

			case FR2C_OPMODE_RLR:
				return new StoreIndexOp2_RLRF(
					getOprStoreIndexF(z), d, s, t);
			case FR2C_OPMODE_RLA:
				return new StoreIndexOp2_RLAF(
					getOprStoreIndexF(z), d, s, t);
			case FR2C_OPMODE_RLL:
				return new StoreIndexOp2_RLLF(
					getOprStoreIndexF(z), d, s, t);

			case FR2C_OPMODE_ARR:
				return new StoreIndexOp2_ARRF(
					getOprStoreIndexF(z), d, s, t);

			case FR2C_OPMODE_LRR:
				return new StoreIndexOp2_LRRF(
					getOprStoreIndexF(z), d, s, t);
			case FR2C_OPMODE_LRA:
				return new StoreIndexOp2_LRAF(
					getOprStoreIndexF(z), d, s, t);
			case FR2C_OPMODE_LAR:
				return new StoreIndexOp2_LARF(
					getOprStoreIndexF(z), d, s, t);

			case FR2C_OPMODE_LRL:
				return new StoreIndexOp2_LRLF(
					getOprStoreIndexF(z), d, s, t);
			case FR2C_OPMODE_LLR:
				return new StoreIndexOp2_LLRF(
					getOprStoreIndexF(z), d, s, t);
			case FR2C_OPMODE_LLL:
				return new StoreIndexOp2_LLLF(
					getOprStoreIndexF(z), d, s, t);
			}
		}
		return null;
	}

	private static StoreIndexI_Opr getOprStoreIndexI(int z)
	{
		switch(z)
		{
		case FR2C_TY_I:
			return new StoreIndexI2_SI();
		case FR2C_TY_UI:
			return new StoreIndexI2_UI();
		case FR2C_TY_L:
		case FR2C_TY_NL:
			return new StoreIndexI2_SL();
		case FR2C_TY_UL:
		case FR2C_TY_UNL:
			return new StoreIndexI2_UL();

		case FR2C_TY_SB:
			return new StoreIndexI2_SB();
		case FR2C_TY_UB:
			return new StoreIndexI2_UB();
		case FR2C_TY_SS:
			return new StoreIndexI2_SS();
		case FR2C_TY_US:
			return new StoreIndexI2_US();
		}
		return null;
	}

	private static StoreIndexF_Opr getOprStoreIndexF(int z)
	{
		switch(z)
		{
		case FR2C_TY_F:
			return new StoreIndexF2_F();
		case FR2C_TY_D:
			return new StoreIndexF2_D();
		}
		return null;
	}

	private static abstract class StoreIndexI_Opr
	{
		public abstract void op(FrPointer ptr, int idx, long val);
	}

	private static abstract class StoreIndexF_Opr
	{
		public abstract void op(FrPointer ptr, int idx, double val);
	}

	private static final class StoreIndexI2_SI extends StoreIndexI_Opr
		{ public void op(FrPointer ptr, int idx, long val)
			{ ptr.seti(idx, (int)val); } }
	private static final class StoreIndexI2_UI extends StoreIndexI_Opr
		{ public void op(FrPointer ptr, int idx, long val)
			{ ptr.seti(idx, (int)val); } }
	private static final class StoreIndexI2_SL extends StoreIndexI_Opr
		{ public void op(FrPointer ptr, int idx, long val)
			{ ptr.setl(idx, val); } }
	private static final class StoreIndexI2_UL extends StoreIndexI_Opr
		{ public void op(FrPointer ptr, int idx, long val)
			{ ptr.setl(idx, val); } }

	private static final class StoreIndexI2_SB extends StoreIndexI_Opr
		{ public void op(FrPointer ptr, int idx, long val)
			{ ptr.setsb(idx, (int)val); } }
	private static final class StoreIndexI2_UB extends StoreIndexI_Opr
		{ public void op(FrPointer ptr, int idx, long val)
			{ ptr.setub(idx, (int)val); } }
	private static final class StoreIndexI2_SS extends StoreIndexI_Opr
		{ public void op(FrPointer ptr, int idx, long val)
			{ ptr.setss(idx, (int)val); } }
	private static final class StoreIndexI2_US extends StoreIndexI_Opr
		{ public void op(FrPointer ptr, int idx, long val)
			{ ptr.setus(idx, (int)val); } }

	private static final class StoreIndexF2_F extends StoreIndexF_Opr
		{ public void op(FrPointer ptr, int idx, double val)
			{ ptr.setf(idx, (float)val); } }
	private static final class StoreIndexF2_D extends StoreIndexF_Opr
		{ public void op(FrPointer ptr, int idx, double val)
			{ ptr.setd(idx, val); } }


	//Indirect Int

	private static final class StoreIndexOp2_RRRI extends FrOpcodeDST
	{
		public StoreIndexOp2_RRRI(StoreIndexI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ op.op(frm.regp[d], (int)frm.regi[s], frm.regi[t]); }
		StoreIndexI_Opr op;
	}

	private static final class StoreIndexOp2_RRAI extends FrOpcodeDST
	{
		public StoreIndexOp2_RRAI(StoreIndexI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ op.op(frm.regp[d], (int)frm.regi[s], frm.argi[t]); }
		StoreIndexI_Opr op;
	}

	private static final class StoreIndexOp2_RRLI extends FrOpcodeDST
	{
		public StoreIndexOp2_RRLI(StoreIndexI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ op.op(frm.regp[d], (int)frm.regi[s], frm.loci[t]); }
		StoreIndexI_Opr op;
	}

	private static final class StoreIndexOp2_RARI extends FrOpcodeDST
	{
		public StoreIndexOp2_RARI(StoreIndexI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ op.op(frm.regp[d], (int)frm.argi[s], frm.regi[t]); }
		StoreIndexI_Opr op;
	}

	private static final class StoreIndexOp2_RAAI extends FrOpcodeDST
	{
		public StoreIndexOp2_RAAI(StoreIndexI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ op.op(frm.regp[d], (int)frm.argi[s], frm.argi[t]); }
		StoreIndexI_Opr op;
	}

	private static final class StoreIndexOp2_RALI extends FrOpcodeDST
	{
		public StoreIndexOp2_RALI(StoreIndexI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ op.op(frm.regp[d], (int)frm.argi[s], frm.loci[t]); }
		StoreIndexI_Opr op;
	}

	private static final class StoreIndexOp2_RLRI extends FrOpcodeDST
	{
		public StoreIndexOp2_RLRI(StoreIndexI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ op.op(frm.regp[d], (int)frm.loci[s], frm.regi[t]); }
		StoreIndexI_Opr op;
	}

	private static final class StoreIndexOp2_RLAI extends FrOpcodeDST
	{
		public StoreIndexOp2_RLAI(StoreIndexI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ op.op(frm.regp[d], (int)frm.loci[s], frm.argi[t]); }
		StoreIndexI_Opr op;
	}

	private static final class StoreIndexOp2_RLLI extends FrOpcodeDST
	{
		public StoreIndexOp2_RLLI(StoreIndexI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ op.op(frm.regp[d], (int)frm.loci[s], frm.loci[t]); }
		StoreIndexI_Opr op;
	}

	private static final class StoreIndexOp2_ARRI extends FrOpcodeDST
	{
		public StoreIndexOp2_ARRI(StoreIndexI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ op.op(frm.argp[d], (int)frm.regi[s], frm.regi[t]); }
		StoreIndexI_Opr op;
	}

	private static final class StoreIndexOp2_LRRI extends FrOpcodeDST
	{
		public StoreIndexOp2_LRRI(StoreIndexI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ op.op(frm.locp[d], (int)frm.regi[s], frm.regi[t]); }
		StoreIndexI_Opr op;
	}

	private static final class StoreIndexOp2_LRAI extends FrOpcodeDST
	{
		public StoreIndexOp2_LRAI(StoreIndexI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ op.op(frm.locp[d], (int)frm.regi[s], frm.argi[t]); }
		StoreIndexI_Opr op;
	}

	private static final class StoreIndexOp2_LARI extends FrOpcodeDST
	{
		public StoreIndexOp2_LARI(StoreIndexI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ op.op(frm.locp[d], (int)frm.argi[s], frm.regi[t]); }
		StoreIndexI_Opr op;
	}

	private static final class StoreIndexOp2_LRLI extends FrOpcodeDST
	{
		public StoreIndexOp2_LRLI(StoreIndexI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ op.op(frm.locp[d], (int)frm.regi[s], frm.loci[t]); }
		StoreIndexI_Opr op;
	}

	private static final class StoreIndexOp2_LLRI extends FrOpcodeDST
	{
		public StoreIndexOp2_LLRI(StoreIndexI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ op.op(frm.locp[d], (int)frm.loci[s], frm.regi[t]); }
		StoreIndexI_Opr op;
	}

	private static final class StoreIndexOp2_LLLI extends FrOpcodeDST
	{
		public StoreIndexOp2_LLLI(StoreIndexI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ op.op(frm.locp[d], (int)frm.loci[s], frm.loci[t]); }
		StoreIndexI_Opr op;
	}


	//Indirect Float

	private static final class StoreIndexOp2_RRRF extends FrOpcodeDST
	{
		public StoreIndexOp2_RRRF(StoreIndexF_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ op.op(frm.regp[d], (int)frm.regi[s], frm.regf[t]); }
		StoreIndexF_Opr op;
	}

	private static final class StoreIndexOp2_RRAF extends FrOpcodeDST
	{
		public StoreIndexOp2_RRAF(StoreIndexF_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ op.op(frm.regp[d], (int)frm.regi[s], frm.argf[t]); }
		StoreIndexF_Opr op;
	}

	private static final class StoreIndexOp2_RRLF extends FrOpcodeDST
	{
		public StoreIndexOp2_RRLF(StoreIndexF_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ op.op(frm.regp[d], (int)frm.regi[s], frm.locf[t]); }
		StoreIndexF_Opr op;
	}

	private static final class StoreIndexOp2_RARF extends FrOpcodeDST
	{
		public StoreIndexOp2_RARF(StoreIndexF_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ op.op(frm.regp[d], (int)frm.argi[s], frm.regf[t]); }
		StoreIndexF_Opr op;
	}

	private static final class StoreIndexOp2_RAAF extends FrOpcodeDST
	{
		public StoreIndexOp2_RAAF(StoreIndexF_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ op.op(frm.regp[d], (int)frm.argi[s], frm.argf[t]); }
		StoreIndexF_Opr op;
	}

	private static final class StoreIndexOp2_RALF extends FrOpcodeDST
	{
		public StoreIndexOp2_RALF(StoreIndexF_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ op.op(frm.regp[d], (int)frm.argi[s], frm.locf[t]); }
		StoreIndexF_Opr op;
	}

	private static final class StoreIndexOp2_RLRF extends FrOpcodeDST
	{
		public StoreIndexOp2_RLRF(StoreIndexF_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ op.op(frm.regp[d], (int)frm.loci[s], frm.regf[t]); }
		StoreIndexF_Opr op;
	}

	private static final class StoreIndexOp2_RLAF extends FrOpcodeDST
	{
		public StoreIndexOp2_RLAF(StoreIndexF_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ op.op(frm.regp[d], (int)frm.loci[s], frm.argf[t]); }
		StoreIndexF_Opr op;
	}

	private static final class StoreIndexOp2_RLLF extends FrOpcodeDST
	{
		public StoreIndexOp2_RLLF(StoreIndexF_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ op.op(frm.regp[d], (int)frm.loci[s], frm.locf[t]); }
		StoreIndexF_Opr op;
	}

	private static final class StoreIndexOp2_ARRF extends FrOpcodeDST
	{
		public StoreIndexOp2_ARRF(StoreIndexF_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ op.op(frm.argp[d], (int)frm.regi[s], frm.regf[t]); }
		StoreIndexF_Opr op;
	}

	private static final class StoreIndexOp2_LRRF extends FrOpcodeDST
	{
		public StoreIndexOp2_LRRF(StoreIndexF_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ op.op(frm.locp[d], (int)frm.regi[s], frm.regf[t]); }
		StoreIndexF_Opr op;
	}

	private static final class StoreIndexOp2_LRAF extends FrOpcodeDST
	{
		public StoreIndexOp2_LRAF(StoreIndexF_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ op.op(frm.locp[d], (int)frm.regi[s], frm.argf[t]); }
		StoreIndexF_Opr op;
	}

	private static final class StoreIndexOp2_LARF extends FrOpcodeDST
	{
		public StoreIndexOp2_LARF(StoreIndexF_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ op.op(frm.locp[d], (int)frm.argi[s], frm.regf[t]); }
		StoreIndexF_Opr op;
	}

	private static final class StoreIndexOp2_LRLF extends FrOpcodeDST
	{
		public StoreIndexOp2_LRLF(StoreIndexF_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ op.op(frm.locp[d], (int)frm.regi[s], frm.locf[t]); }
		StoreIndexF_Opr op;
	}

	private static final class StoreIndexOp2_LLRF extends FrOpcodeDST
	{
		public StoreIndexOp2_LLRF(StoreIndexF_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ op.op(frm.locp[d], (int)frm.loci[s], frm.regf[t]); }
		StoreIndexF_Opr op;
	}

	private static final class StoreIndexOp2_LLLF extends FrOpcodeDST
	{
		public StoreIndexOp2_LLLF(StoreIndexF_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ op.op(frm.locp[d], (int)frm.loci[s], frm.locf[t]); }
		StoreIndexF_Opr op;
	}

}