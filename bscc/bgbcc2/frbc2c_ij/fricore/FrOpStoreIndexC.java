package fricore;

public abstract class FrOpStoreIndexC extends FrOpcode
{
	public static FrOpcode makeOpStoreIndexC(
		int z, int y, int d, int t, int s)
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
			case FR2C_OPMODE_RR:
				return new StoreIndexOp2_RRI(
					getOprStoreIndexI(z), d, s, t);
			case FR2C_OPMODE_RA:
				return new StoreIndexOp2_RAI(
					getOprStoreIndexI(z), d, s, t);
			case FR2C_OPMODE_RL:
				return new StoreIndexOp2_RLI(
					getOprStoreIndexI(z), d, s, t);
			case FR2C_OPMODE_AR:
				return new StoreIndexOp2_ARI(
					getOprStoreIndexI(z), d, s, t);
			case FR2C_OPMODE_AA:
				return new StoreIndexOp2_AAI(
					getOprStoreIndexI(z), d, s, t);
			case FR2C_OPMODE_AL:
				return new StoreIndexOp2_ALI(
					getOprStoreIndexI(z), d, s, t);
			case FR2C_OPMODE_LR:
				return new StoreIndexOp2_LRI(
					getOprStoreIndexI(z), d, s, t);
			case FR2C_OPMODE_LA:
				return new StoreIndexOp2_LAI(
					getOprStoreIndexI(z), d, s, t);
			case FR2C_OPMODE_LL:
				return new StoreIndexOp2_LLI(
					getOprStoreIndexI(z), d, s, t);
			}

		case FR2C_TY_F:
		case FR2C_TY_D:
			switch(y)
			{
			case FR2C_OPMODE_RR:
				return new StoreIndexOp2_RRF(
					getOprStoreIndexF(z), d, s, t);
			case FR2C_OPMODE_RA:
				return new StoreIndexOp2_RAF(
					getOprStoreIndexF(z), d, s, t);
			case FR2C_OPMODE_RL:
				return new StoreIndexOp2_RLF(
					getOprStoreIndexF(z), d, s, t);
			case FR2C_OPMODE_AR:
				return new StoreIndexOp2_ARF(
					getOprStoreIndexF(z), d, s, t);
			case FR2C_OPMODE_AA:
				return new StoreIndexOp2_AAF(
					getOprStoreIndexF(z), d, s, t);
			case FR2C_OPMODE_AL:
				return new StoreIndexOp2_ALF(
					getOprStoreIndexF(z), d, s, t);
			case FR2C_OPMODE_LR:
				return new StoreIndexOp2_LRF(
					getOprStoreIndexF(z), d, s, t);
			case FR2C_OPMODE_LA:
				return new StoreIndexOp2_LAF(
					getOprStoreIndexF(z), d, s, t);
			case FR2C_OPMODE_LL:
				return new StoreIndexOp2_LLF(
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

	private static final class StoreIndexOp2_RRI extends FrOpcodeDST
	{
		public StoreIndexOp2_RRI(StoreIndexI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ op.op(frm.regp[d], s, frm.regi[t]); }
		StoreIndexI_Opr op;
	}

	private static final class StoreIndexOp2_RAI extends FrOpcodeDST
	{
		public StoreIndexOp2_RAI(StoreIndexI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ op.op(frm.regp[d], s, frm.argi[t]); }
		StoreIndexI_Opr op;
	}

	private static final class StoreIndexOp2_RLI extends FrOpcodeDST
	{
		public StoreIndexOp2_RLI(StoreIndexI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ op.op(frm.regp[d], s, frm.loci[t]); }
		StoreIndexI_Opr op;
	}

	private static final class StoreIndexOp2_ARI extends FrOpcodeDST
	{
		public StoreIndexOp2_ARI(StoreIndexI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ op.op(frm.argp[d], s, frm.regi[t]); }
		StoreIndexI_Opr op;
	}

	private static final class StoreIndexOp2_AAI extends FrOpcodeDST
	{
		public StoreIndexOp2_AAI(StoreIndexI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ op.op(frm.argp[d], s, frm.argi[t]); }
		StoreIndexI_Opr op;
	}

	private static final class StoreIndexOp2_ALI extends FrOpcodeDST
	{
		public StoreIndexOp2_ALI(StoreIndexI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ op.op(frm.argp[d], s, frm.loci[t]); }
		StoreIndexI_Opr op;
	}

	private static final class StoreIndexOp2_LRI extends FrOpcodeDST
	{
		public StoreIndexOp2_LRI(StoreIndexI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ op.op(frm.locp[d], s, frm.regi[t]); }
		StoreIndexI_Opr op;
	}

	private static final class StoreIndexOp2_LAI extends FrOpcodeDST
	{
		public StoreIndexOp2_LAI(StoreIndexI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ op.op(frm.locp[d], s, frm.argi[t]); }
		StoreIndexI_Opr op;
	}

	private static final class StoreIndexOp2_LLI extends FrOpcodeDST
	{
		public StoreIndexOp2_LLI(StoreIndexI_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ op.op(frm.locp[d], s, frm.loci[t]); }
		StoreIndexI_Opr op;
	}


	//Indirect Float

	private static final class StoreIndexOp2_RRF extends FrOpcodeDST
	{
		public StoreIndexOp2_RRF(StoreIndexF_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ op.op(frm.regp[d], s, frm.regf[t]); }
		StoreIndexF_Opr op;
	}

	private static final class StoreIndexOp2_RAF extends FrOpcodeDST
	{
		public StoreIndexOp2_RAF(StoreIndexF_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ op.op(frm.regp[d], s, frm.argf[t]); }
		StoreIndexF_Opr op;
	}

	private static final class StoreIndexOp2_RLF extends FrOpcodeDST
	{
		public StoreIndexOp2_RLF(StoreIndexF_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ op.op(frm.regp[d], s, frm.locf[t]); }
		StoreIndexF_Opr op;
	}

	private static final class StoreIndexOp2_ARF extends FrOpcodeDST
	{
		public StoreIndexOp2_ARF(StoreIndexF_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ op.op(frm.argp[d], s, frm.regf[t]); }
		StoreIndexF_Opr op;
	}

	private static final class StoreIndexOp2_AAF extends FrOpcodeDST
	{
		public StoreIndexOp2_AAF(StoreIndexF_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ op.op(frm.argp[d], s, frm.argf[t]); }
		StoreIndexF_Opr op;
	}

	private static final class StoreIndexOp2_ALF extends FrOpcodeDST
	{
		public StoreIndexOp2_ALF(StoreIndexF_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ op.op(frm.argp[d], s, frm.locf[t]); }
		StoreIndexF_Opr op;
	}

	private static final class StoreIndexOp2_LRF extends FrOpcodeDST
	{
		public StoreIndexOp2_LRF(StoreIndexF_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ op.op(frm.locp[d], s, frm.regf[t]); }
		StoreIndexF_Opr op;
	}

	private static final class StoreIndexOp2_LAF extends FrOpcodeDST
	{
		public StoreIndexOp2_LAF(StoreIndexF_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ op.op(frm.locp[d], s, frm.argf[t]); }
		StoreIndexF_Opr op;
	}

	private static final class StoreIndexOp2_LLF extends FrOpcodeDST
	{
		public StoreIndexOp2_LLF(StoreIndexF_Opr op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ op.op(frm.locp[d], s, frm.locf[t]); }
		StoreIndexF_Opr op;
	}
}