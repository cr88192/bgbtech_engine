package fricore;

public abstract class FrOpLoadIndexC extends FrOpcode
{
	public static FrOpcode makeOpLoadIndexC(
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
				return new LoadIndexOp2_RRI(
					getOprLoadIndexI(z), d, s, t);
			case FR2C_OPMODE_RRA:
				return new LoadIndexOp2_RAI(
					getOprLoadIndexI(z), d, s, t);
			case FR2C_OPMODE_RRL:
				return new LoadIndexOp2_RLI(
					getOprLoadIndexI(z), d, s, t);

			case FR2C_OPMODE_RAR:
				return new LoadIndexOp2_ARI(
					getOprLoadIndexI(z), d, s, t);
			case FR2C_OPMODE_RAA:
				return new LoadIndexOp2_AAI(
					getOprLoadIndexI(z), d, s, t);
			case FR2C_OPMODE_RAL:
				return new LoadIndexOp2_ALI(
					getOprLoadIndexI(z), d, s, t);

			case FR2C_OPMODE_RLR:
				return new LoadIndexOp2_LRI(
					getOprLoadIndexI(z), d, s, t);
			case FR2C_OPMODE_RLA:
				return new LoadIndexOp2_LAI(
					getOprLoadIndexI(z), d, s, t);
			case FR2C_OPMODE_RLL:
				return new LoadIndexOp2_LLI(
					getOprLoadIndexI(z), d, s, t);
			}

		case FR2C_TY_F:
		case FR2C_TY_D:
			switch(y)
			{
			case FR2C_OPMODE_RRR:
				return new LoadIndexOp2_RRF(
					getOprLoadIndexF(z), d, s, t);
			case FR2C_OPMODE_RRA:
				return new LoadIndexOp2_RAF(
					getOprLoadIndexF(z), d, s, t);
			case FR2C_OPMODE_RRL:
				return new LoadIndexOp2_RLF(
					getOprLoadIndexF(z), d, s, t);

			case FR2C_OPMODE_RAR:
				return new LoadIndexOp2_ARF(
					getOprLoadIndexF(z), d, s, t);
			case FR2C_OPMODE_RAA:
				return new LoadIndexOp2_AAF(
					getOprLoadIndexF(z), d, s, t);
			case FR2C_OPMODE_RAL:
				return new LoadIndexOp2_ALF(
					getOprLoadIndexF(z), d, s, t);

			case FR2C_OPMODE_RLR:
				return new LoadIndexOp2_LRF(
					getOprLoadIndexF(z), d, s, t);
			case FR2C_OPMODE_RLA:
				return new LoadIndexOp2_LAF(
					getOprLoadIndexF(z), d, s, t);
			case FR2C_OPMODE_RLL:
				return new LoadIndexOp2_LLF(
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

	private static final class LoadIndexOp2_RRI extends FrOpcodeDST
	{
		public LoadIndexOp2_RRI(LoadIndexI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=op.op(frm.regp[s], t); }
		LoadIndexI_Opr op;
	}

	private static final class LoadIndexOp2_RAI extends FrOpcodeDST
	{
		public LoadIndexOp2_RAI(LoadIndexI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=op.op(frm.regp[s], t); }
		LoadIndexI_Opr op;
	}

	private static final class LoadIndexOp2_RLI extends FrOpcodeDST
	{
		public LoadIndexOp2_RLI(LoadIndexI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=op.op(frm.regp[s], t); }
		LoadIndexI_Opr op;
	}

	private static final class LoadIndexOp2_ARI extends FrOpcodeDST
	{
		public LoadIndexOp2_ARI(LoadIndexI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=op.op(frm.argp[s], t); }
		LoadIndexI_Opr op;
	}

	private static final class LoadIndexOp2_AAI extends FrOpcodeDST
	{
		public LoadIndexOp2_AAI(LoadIndexI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=op.op(frm.argp[s], t); }
		LoadIndexI_Opr op;
	}

	private static final class LoadIndexOp2_ALI extends FrOpcodeDST
	{
		public LoadIndexOp2_ALI(LoadIndexI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=op.op(frm.argp[s], t); }
		LoadIndexI_Opr op;
	}

	private static final class LoadIndexOp2_LRI extends FrOpcodeDST
	{
		public LoadIndexOp2_LRI(LoadIndexI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=op.op(frm.locp[s], t); }
		LoadIndexI_Opr op;
	}

	private static final class LoadIndexOp2_LAI extends FrOpcodeDST
	{
		public LoadIndexOp2_LAI(LoadIndexI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=op.op(frm.locp[s], t); }
		LoadIndexI_Opr op;
	}

	private static final class LoadIndexOp2_LLI extends FrOpcodeDST
	{
		public LoadIndexOp2_LLI(LoadIndexI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=op.op(frm.locp[s], t); }
		LoadIndexI_Opr op;
	}


	//Indirect Float

	private static final class LoadIndexOp2_RRF extends FrOpcodeDST
	{
		public LoadIndexOp2_RRF(LoadIndexF_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regf[d]=op.op(frm.regp[s], t); }
		LoadIndexF_Opr op;
	}

	private static final class LoadIndexOp2_RAF extends FrOpcodeDST
	{
		public LoadIndexOp2_RAF(LoadIndexF_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regf[d]=op.op(frm.regp[s], t); }
		LoadIndexF_Opr op;
	}

	private static final class LoadIndexOp2_RLF extends FrOpcodeDST
	{
		public LoadIndexOp2_RLF(LoadIndexF_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regf[d]=op.op(frm.regp[s], t); }
		LoadIndexF_Opr op;
	}

	private static final class LoadIndexOp2_ARF extends FrOpcodeDST
	{
		public LoadIndexOp2_ARF(LoadIndexF_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regf[d]=op.op(frm.argp[s], t); }
		LoadIndexF_Opr op;
	}

	private static final class LoadIndexOp2_AAF extends FrOpcodeDST
	{
		public LoadIndexOp2_AAF(LoadIndexF_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regf[d]=op.op(frm.argp[s], t); }
		LoadIndexF_Opr op;
	}

	private static final class LoadIndexOp2_ALF extends FrOpcodeDST
	{
		public LoadIndexOp2_ALF(LoadIndexF_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regf[d]=op.op(frm.argp[s], t); }
		LoadIndexF_Opr op;
	}

	private static final class LoadIndexOp2_LRF extends FrOpcodeDST
	{
		public LoadIndexOp2_LRF(LoadIndexF_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regf[d]=op.op(frm.locp[s], t); }
		LoadIndexF_Opr op;
	}

	private static final class LoadIndexOp2_LAF extends FrOpcodeDST
	{
		public LoadIndexOp2_LAF(LoadIndexF_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regf[d]=op.op(frm.locp[s], t); }
		LoadIndexF_Opr op;
	}

	private static final class LoadIndexOp2_LLF extends FrOpcodeDST
	{
		public LoadIndexOp2_LLF(LoadIndexF_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regf[d]=op.op(frm.locp[s], t); }
		LoadIndexF_Opr op;
	}
}