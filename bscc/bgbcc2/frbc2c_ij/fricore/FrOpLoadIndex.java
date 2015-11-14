package fricore;

public abstract class FrOpLoadIndex extends FrOpcode
{
	public static FrOpcode makeOpLoadIndex(
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
				return new LoadIndexOp2_RRRI(
					getOprLoadIndexI(z), d, s, t);
			case FR2C_OPMODE_RRA:
				return new LoadIndexOp2_RRAI(
					getOprLoadIndexI(z), d, s, t);
			case FR2C_OPMODE_RRL:
				return new LoadIndexOp2_RRLI(
					getOprLoadIndexI(z), d, s, t);

			case FR2C_OPMODE_RAR:
				return new LoadIndexOp2_RARI(
					getOprLoadIndexI(z), d, s, t);
			case FR2C_OPMODE_RAA:
				return new LoadIndexOp2_RAAI(
					getOprLoadIndexI(z), d, s, t);
			case FR2C_OPMODE_RAL:
				return new LoadIndexOp2_RALI(
					getOprLoadIndexI(z), d, s, t);

			case FR2C_OPMODE_RLR:
				return new LoadIndexOp2_RLRI(
					getOprLoadIndexI(z), d, s, t);
			case FR2C_OPMODE_RLA:
				return new LoadIndexOp2_RLAI(
					getOprLoadIndexI(z), d, s, t);
			case FR2C_OPMODE_RLL:
				return new LoadIndexOp2_RLLI(
					getOprLoadIndexI(z), d, s, t);

			case FR2C_OPMODE_ARR:
				return new LoadIndexOp2_ARRI(
					getOprLoadIndexI(z), d, s, t);

			case FR2C_OPMODE_LRR:
				return new LoadIndexOp2_LRRI(
					getOprLoadIndexI(z), d, s, t);
			case FR2C_OPMODE_LRA:
				return new LoadIndexOp2_LRAI(
					getOprLoadIndexI(z), d, s, t);
			case FR2C_OPMODE_LAR:
				return new LoadIndexOp2_LARI(
					getOprLoadIndexI(z), d, s, t);

			case FR2C_OPMODE_LRL:
				return new LoadIndexOp2_LRLI(
					getOprLoadIndexI(z), d, s, t);
			case FR2C_OPMODE_LLR:
				return new LoadIndexOp2_LLRI(
					getOprLoadIndexI(z), d, s, t);
			case FR2C_OPMODE_LLL:
				return new LoadIndexOp2_LLLI(
					getOprLoadIndexI(z), d, s, t);
			}

		case FR2C_TY_F:
		case FR2C_TY_D:
			switch(y)
			{
			case FR2C_OPMODE_RRR:
				return new LoadIndexOp2_RRRF(
					getOprLoadIndexF(z), d, s, t);
			case FR2C_OPMODE_RRA:
				return new LoadIndexOp2_RRAF(
					getOprLoadIndexF(z), d, s, t);
			case FR2C_OPMODE_RRL:
				return new LoadIndexOp2_RRLF(
					getOprLoadIndexF(z), d, s, t);

			case FR2C_OPMODE_RAR:
				return new LoadIndexOp2_RARF(
					getOprLoadIndexF(z), d, s, t);
			case FR2C_OPMODE_RAA:
				return new LoadIndexOp2_RAAF(
					getOprLoadIndexF(z), d, s, t);
			case FR2C_OPMODE_RAL:
				return new LoadIndexOp2_RALF(
					getOprLoadIndexF(z), d, s, t);

			case FR2C_OPMODE_RLR:
				return new LoadIndexOp2_RLRF(
					getOprLoadIndexF(z), d, s, t);
			case FR2C_OPMODE_RLA:
				return new LoadIndexOp2_RLAF(
					getOprLoadIndexF(z), d, s, t);
			case FR2C_OPMODE_RLL:
				return new LoadIndexOp2_RLLF(
					getOprLoadIndexF(z), d, s, t);

			case FR2C_OPMODE_ARR:
				return new LoadIndexOp2_ARRF(
					getOprLoadIndexF(z), d, s, t);

			case FR2C_OPMODE_LRR:
				return new LoadIndexOp2_LRRF(
					getOprLoadIndexF(z), d, s, t);
			case FR2C_OPMODE_LRA:
				return new LoadIndexOp2_LRAF(
					getOprLoadIndexF(z), d, s, t);
			case FR2C_OPMODE_LAR:
				return new LoadIndexOp2_LARF(
					getOprLoadIndexF(z), d, s, t);

			case FR2C_OPMODE_LRL:
				return new LoadIndexOp2_LRLF(
					getOprLoadIndexF(z), d, s, t);
			case FR2C_OPMODE_LLR:
				return new LoadIndexOp2_LLRF(
					getOprLoadIndexF(z), d, s, t);
			case FR2C_OPMODE_LLL:
				return new LoadIndexOp2_LLLF(
					getOprLoadIndexF(z), d, s, t);
			}
		}
		return null;
	}

	private static LoadIndexI_Opr getOprLoadIndexI(int z)
	{
		switch(z)
		{
		case FR2C_TY_I:
			return new LoadIndexI2_SI();
		case FR2C_TY_UI:
			return new LoadIndexI2_UI();
		case FR2C_TY_L:
		case FR2C_TY_NL:
			return new LoadIndexI2_SL();
		case FR2C_TY_UL:
		case FR2C_TY_UNL:
			return new LoadIndexI2_UL();

		case FR2C_TY_SB:
			return new LoadIndexI2_SB();
		case FR2C_TY_UB:
			return new LoadIndexI2_UB();
		case FR2C_TY_SS:
			return new LoadIndexI2_SS();
		case FR2C_TY_US:
			return new LoadIndexI2_US();
		}
		return null;
	}

	private static LoadIndexF_Opr getOprLoadIndexF(int z)
	{
		switch(z)
		{
		case FR2C_TY_F:
			return new LoadIndexF2_F();
		case FR2C_TY_D:
			return new LoadIndexF2_D();
		}
		return null;
	}

	private static abstract class LoadIndexI_Opr
	{
		public abstract long op(FrPointer ptr, int idx);
	}

	private static abstract class LoadIndexF_Opr
	{
		public abstract double op(FrPointer ptr, int idx);
	}

	private static final class LoadIndexI2_SI extends LoadIndexI_Opr
		{ public long op(FrPointer ptr, int idx)
			{ return(ptr.getsi(idx)); } }
	private static final class LoadIndexI2_UI extends LoadIndexI_Opr
		{ public long op(FrPointer ptr, int idx)
			{ return(ptr.getui(idx)); } }
	private static final class LoadIndexI2_SL extends LoadIndexI_Opr
		{ public long op(FrPointer ptr, int idx)
			{ return(ptr.getsl(idx)); } }
	private static final class LoadIndexI2_UL extends LoadIndexI_Opr
		{ public long op(FrPointer ptr, int idx)
			{ return(ptr.getul(idx)); } }

	private static final class LoadIndexI2_SB extends LoadIndexI_Opr
		{ public long op(FrPointer ptr, int idx)
			{ return((long)ptr.getsb(idx)); } }
	private static final class LoadIndexI2_UB extends LoadIndexI_Opr
		{ public long op(FrPointer ptr, int idx)
			{ return((long)ptr.getub(idx)); } }
	private static final class LoadIndexI2_SS extends LoadIndexI_Opr
		{ public long op(FrPointer ptr, int idx)
			{ return((long)ptr.getss(idx)); } }
	private static final class LoadIndexI2_US extends LoadIndexI_Opr
		{ public long op(FrPointer ptr, int idx)
			{ return((long)ptr.getus(idx)); } }

	private static final class LoadIndexF2_F extends LoadIndexF_Opr
		{ public double op(FrPointer ptr, int idx)
			{ return(ptr.getf(idx)); } }
	private static final class LoadIndexF2_D extends LoadIndexF_Opr
		{ public double op(FrPointer ptr, int idx)
			{ return(ptr.getd(idx)); } }

	//Indirect Int

	private static final class LoadIndexOp2_RRRI extends FrOpcodeDST
	{
		public LoadIndexOp2_RRRI(LoadIndexI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=op.op(frm.regp[s], (int)frm.regi[t]); }
		LoadIndexI_Opr op;
	}

	private static final class LoadIndexOp2_RRAI extends FrOpcodeDST
	{
		public LoadIndexOp2_RRAI(LoadIndexI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=op.op(frm.regp[s], (int)frm.argi[t]); }
		LoadIndexI_Opr op;
	}

	private static final class LoadIndexOp2_RRLI extends FrOpcodeDST
	{
		public LoadIndexOp2_RRLI(LoadIndexI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=op.op(frm.regp[s], (int)frm.loci[t]); }
		LoadIndexI_Opr op;
	}

	private static final class LoadIndexOp2_RARI extends FrOpcodeDST
	{
		public LoadIndexOp2_RARI(LoadIndexI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=op.op(frm.argp[s], (int)frm.regi[t]); }
		LoadIndexI_Opr op;
	}

	private static final class LoadIndexOp2_RAAI extends FrOpcodeDST
	{
		public LoadIndexOp2_RAAI(LoadIndexI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=op.op(frm.argp[s], (int)frm.argi[t]); }
		LoadIndexI_Opr op;
	}

	private static final class LoadIndexOp2_RALI extends FrOpcodeDST
	{
		public LoadIndexOp2_RALI(LoadIndexI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=op.op(frm.argp[s], (int)frm.loci[t]); }
		LoadIndexI_Opr op;
	}

	private static final class LoadIndexOp2_RLRI extends FrOpcodeDST
	{
		public LoadIndexOp2_RLRI(LoadIndexI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=op.op(frm.locp[s], (int)frm.regi[t]); }
		LoadIndexI_Opr op;
	}

	private static final class LoadIndexOp2_RLAI extends FrOpcodeDST
	{
		public LoadIndexOp2_RLAI(LoadIndexI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=op.op(frm.locp[s], (int)frm.argi[t]); }
		LoadIndexI_Opr op;
	}

	private static final class LoadIndexOp2_RLLI extends FrOpcodeDST
	{
		public LoadIndexOp2_RLLI(LoadIndexI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=op.op(frm.locp[s], (int)frm.loci[t]); }
		LoadIndexI_Opr op;
	}

	private static final class LoadIndexOp2_ARRI extends FrOpcodeDST
	{
		public LoadIndexOp2_ARRI(LoadIndexI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.argi[d]=op.op(frm.regp[s], (int)frm.regi[t]); }
		LoadIndexI_Opr op;
	}

	private static final class LoadIndexOp2_LRRI extends FrOpcodeDST
	{
		public LoadIndexOp2_LRRI(LoadIndexI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.loci[d]=op.op(frm.regp[s], (int)frm.regi[t]); }
		LoadIndexI_Opr op;
	}

	private static final class LoadIndexOp2_LRAI extends FrOpcodeDST
	{
		public LoadIndexOp2_LRAI(LoadIndexI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.loci[d]=op.op(frm.regp[s], (int)frm.argi[t]); }
		LoadIndexI_Opr op;
	}

	private static final class LoadIndexOp2_LARI extends FrOpcodeDST
	{
		public LoadIndexOp2_LARI(LoadIndexI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.loci[d]=op.op(frm.argp[s], (int)frm.regi[t]); }
		LoadIndexI_Opr op;
	}

	private static final class LoadIndexOp2_LRLI extends FrOpcodeDST
	{
		public LoadIndexOp2_LRLI(LoadIndexI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.loci[d]=op.op(frm.regp[s], (int)frm.loci[t]); }
		LoadIndexI_Opr op;
	}

	private static final class LoadIndexOp2_LLRI extends FrOpcodeDST
	{
		public LoadIndexOp2_LLRI(LoadIndexI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.loci[d]=op.op(frm.locp[s], (int)frm.regi[t]); }
		LoadIndexI_Opr op;
	}

	private static final class LoadIndexOp2_LLLI extends FrOpcodeDST
	{
		public LoadIndexOp2_LLLI(LoadIndexI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.loci[d]=op.op(frm.locp[s], (int)frm.loci[t]); }
		LoadIndexI_Opr op;
	}


	//Indirect Float

	private static final class LoadIndexOp2_RRRF extends FrOpcodeDST
	{
		public LoadIndexOp2_RRRF(LoadIndexF_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regf[d]=op.op(frm.regp[s], (int)frm.regi[t]); }
		LoadIndexF_Opr op;
	}

	private static final class LoadIndexOp2_RRAF extends FrOpcodeDST
	{
		public LoadIndexOp2_RRAF(LoadIndexF_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regf[d]=op.op(frm.regp[s], (int)frm.argi[t]); }
		LoadIndexF_Opr op;
	}

	private static final class LoadIndexOp2_RRLF extends FrOpcodeDST
	{
		public LoadIndexOp2_RRLF(LoadIndexF_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regf[d]=op.op(frm.regp[s], (int)frm.loci[t]); }
		LoadIndexF_Opr op;
	}

	private static final class LoadIndexOp2_RARF extends FrOpcodeDST
	{
		public LoadIndexOp2_RARF(LoadIndexF_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regf[d]=op.op(frm.argp[s], (int)frm.regi[t]); }
		LoadIndexF_Opr op;
	}

	private static final class LoadIndexOp2_RAAF extends FrOpcodeDST
	{
		public LoadIndexOp2_RAAF(LoadIndexF_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regf[d]=op.op(frm.argp[s], (int)frm.argi[t]); }
		LoadIndexF_Opr op;
	}

	private static final class LoadIndexOp2_RALF extends FrOpcodeDST
	{
		public LoadIndexOp2_RALF(LoadIndexF_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regf[d]=op.op(frm.argp[s], (int)frm.loci[t]); }
		LoadIndexF_Opr op;
	}

	private static final class LoadIndexOp2_RLRF extends FrOpcodeDST
	{
		public LoadIndexOp2_RLRF(LoadIndexF_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regf[d]=op.op(frm.locp[s], (int)frm.regi[t]); }
		LoadIndexF_Opr op;
	}

	private static final class LoadIndexOp2_RLAF extends FrOpcodeDST
	{
		public LoadIndexOp2_RLAF(LoadIndexF_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regf[d]=op.op(frm.locp[s], (int)frm.argi[t]); }
		LoadIndexF_Opr op;
	}

	private static final class LoadIndexOp2_RLLF extends FrOpcodeDST
	{
		public LoadIndexOp2_RLLF(LoadIndexF_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regf[d]=op.op(frm.locp[s], (int)frm.loci[t]); }
		LoadIndexF_Opr op;
	}

	private static final class LoadIndexOp2_ARRF extends FrOpcodeDST
	{
		public LoadIndexOp2_ARRF(LoadIndexF_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.argf[d]=op.op(frm.regp[s], (int)frm.regi[t]); }
		LoadIndexF_Opr op;
	}

	private static final class LoadIndexOp2_LRRF extends FrOpcodeDST
	{
		public LoadIndexOp2_LRRF(LoadIndexF_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.locf[d]=op.op(frm.regp[s], (int)frm.regi[t]); }
		LoadIndexF_Opr op;
	}

	private static final class LoadIndexOp2_LRAF extends FrOpcodeDST
	{
		public LoadIndexOp2_LRAF(LoadIndexF_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.locf[d]=op.op(frm.regp[s], (int)frm.argi[t]); }
		LoadIndexF_Opr op;
	}

	private static final class LoadIndexOp2_LARF extends FrOpcodeDST
	{
		public LoadIndexOp2_LARF(LoadIndexF_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.locf[d]=op.op(frm.argp[s], (int)frm.regi[t]); }
		LoadIndexF_Opr op;
	}

	private static final class LoadIndexOp2_LRLF extends FrOpcodeDST
	{
		public LoadIndexOp2_LRLF(LoadIndexF_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.locf[d]=op.op(frm.regp[s], (int)frm.loci[t]); }
		LoadIndexF_Opr op;
	}

	private static final class LoadIndexOp2_LLRF extends FrOpcodeDST
	{
		public LoadIndexOp2_LLRF(LoadIndexF_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.locf[d]=op.op(frm.locp[s], (int)frm.regi[t]); }
		LoadIndexF_Opr op;
	}

	private static final class LoadIndexOp2_LLLF extends FrOpcodeDST
	{
		public LoadIndexOp2_LLLF(LoadIndexF_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.locf[d]=op.op(frm.locp[s], (int)frm.loci[t]); }
		LoadIndexF_Opr op;
	}

}