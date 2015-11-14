package fricore;

public abstract class FrCmpOpImm extends FrOpcode
{
	public static FrOpcode makeCmpOpImm(
		int z, int y, int op, int d, int s, long t)
	{
		switch(z)
		{
		case FR2C_TY_I:
		case FR2C_TY_L:
			switch(y)
			{
			case FR2C_OPMODE_RR:
				return new CmpOp2_RRI(
					FrCompareOperator.getOperator(op),
					d, s, t);
			case FR2C_OPMODE_RA:
				return new CmpOp2_RAI(
					FrCompareOperator.getOperator(op),
					d, s, t);
			case FR2C_OPMODE_RL:
				return new CmpOp2_RLI(
					FrCompareOperator.getOperator(op),
					d, s, t);

			case FR2C_OPMODE_AR:
				return new CmpOp2_ARI(
					FrCompareOperator.getOperator(op),
					d, s, t);
			case FR2C_OPMODE_AA:
				return new CmpOp2_AAI(
					FrCompareOperator.getOperator(op),
					d, s, t);
			case FR2C_OPMODE_AL:
				return new CmpOp2_ALI(
					FrCompareOperator.getOperator(op),
					d, s, t);

			case FR2C_OPMODE_LR:
				return new CmpOp2_ARI(
					FrCompareOperator.getOperator(op),
					d, s, t);
			case FR2C_OPMODE_LA:
				return new CmpOp2_AAI(
					FrCompareOperator.getOperator(op),
					d, s, t);
			case FR2C_OPMODE_LL:
				return new CmpOp2_ALI(
					FrCompareOperator.getOperator(op),
					d, s, t);
			}
		}
		return null;
	}

	public static FrOpcode makeCmpOpImm(
		int z, int y, int op, int d, int s, double t)

	{
		switch(z)
		{
		case FR2C_TY_F:
		case FR2C_TY_D:
			switch(y)
			{
			case FR2C_OPMODE_RR:
				return new CmpOp2_RRF(
					FrCompareOperator.getOperator(op),
					d, s, t);
			case FR2C_OPMODE_RA:
				return new CmpOp2_RAF(
					FrCompareOperator.getOperator(op),
					d, s, t);
			case FR2C_OPMODE_RL:
				return new CmpOp2_RLF(
					FrCompareOperator.getOperator(op),
					d, s, t);
			case FR2C_OPMODE_AR:
				return new CmpOp2_ARF(
					FrCompareOperator.getOperator(op),
					d, s, t);
			case FR2C_OPMODE_AA:
				return new CmpOp2_AAF(
					FrCompareOperator.getOperator(op),
					d, s, t);
			case FR2C_OPMODE_AL:
				return new CmpOp2_ALF(
					FrCompareOperator.getOperator(op),
					d, s, t);
			case FR2C_OPMODE_LR:
				return new CmpOp2_ARF(
					FrCompareOperator.getOperator(op),
					d, s, t);
			case FR2C_OPMODE_LA:
				return new CmpOp2_AAF(
					FrCompareOperator.getOperator(op),
					d, s, t);
			case FR2C_OPMODE_LL:
				return new CmpOp2_ALF(
					FrCompareOperator.getOperator(op),
					d, s, t);
			}
		}
		return null;
	}



	//Indirect Int

	private static final class CmpOp2_RRI extends FrOpcodeDSII
	{
		public CmpOp2_RRI(FrCompareOperator op, int d, int s, long t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=(long)op.op(frm.regi[s], imm); }
		FrCompareOperator op;
	}

	private static final class CmpOp2_RAI extends FrOpcodeDSII
	{
		public CmpOp2_RAI(FrCompareOperator op, int d, int s, long t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=(long)op.op(frm.argi[s], imm); }
		FrCompareOperator op;
	}

	private static final class CmpOp2_RLI extends FrOpcodeDSII
	{
		public CmpOp2_RLI(FrCompareOperator op, int d, int s, long t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=(long)op.op(frm.loci[s], imm); }
		FrCompareOperator op;
	}

	private static final class CmpOp2_ARI extends FrOpcodeDSII
	{
		public CmpOp2_ARI(FrCompareOperator op, int d, int s, long t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.argi[d]=(long)op.op(frm.regi[s], imm); }
		FrCompareOperator op;
	}

	private static final class CmpOp2_AAI extends FrOpcodeDSII
	{
		public CmpOp2_AAI(FrCompareOperator op, int d, int s, long t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.argi[d]=(long)op.op(frm.argi[s], imm); }
		FrCompareOperator op;
	}

	private static final class CmpOp2_ALI extends FrOpcodeDSII
	{
		public CmpOp2_ALI(FrCompareOperator op, int d, int s, long t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.argi[d]=(long)op.op(frm.loci[s], imm); }
		FrCompareOperator op;
	}

	private static final class CmpOp2_LRI extends FrOpcodeDSII
	{
		public CmpOp2_LRI(FrCompareOperator op, int d, int s, long t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.loci[d]=(long)op.op(frm.regi[s], imm); }
		FrCompareOperator op;
	}

	private static final class CmpOp2_LAI extends FrOpcodeDSII
	{
		public CmpOp2_LAI(FrCompareOperator op, int d, int s, long t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.loci[d]=(long)op.op(frm.argi[s], imm); }
		FrCompareOperator op;
	}

	private static final class CmpOp2_LLI extends FrOpcodeDSII
	{
		public CmpOp2_LLI(FrCompareOperator op, int d, int s, long t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.loci[d]=(long)op.op(frm.loci[s], imm); }
		FrCompareOperator op;
	}


	//Indirect Float

	private static final class CmpOp2_RRF extends FrOpcodeDSIF
	{
		public CmpOp2_RRF(FrCompareOperator op, int d, int s, double t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=(long)op.op(frm.regf[s], imm); }
		FrCompareOperator op;
	}

	private static final class CmpOp2_RAF extends FrOpcodeDSIF
	{
		public CmpOp2_RAF(FrCompareOperator op, int d, int s, double t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=(long)op.op(frm.argf[s], imm); }
		FrCompareOperator op;
	}

	private static final class CmpOp2_RLF extends FrOpcodeDSIF
	{
		public CmpOp2_RLF(FrCompareOperator op, int d, int s, double t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=(long)op.op(frm.locf[s], imm); }
		FrCompareOperator op;
	}

	private static final class CmpOp2_ARF extends FrOpcodeDSIF
	{
		public CmpOp2_ARF(FrCompareOperator op, int d, int s, double t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.argi[d]=(long)op.op(frm.regf[s], imm); }
		FrCompareOperator op;
	}

	private static final class CmpOp2_AAF extends FrOpcodeDSIF
	{
		public CmpOp2_AAF(FrCompareOperator op, int d, int s, double t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.argi[d]=(long)op.op(frm.argf[s], imm); }
		FrCompareOperator op;
	}

	private static final class CmpOp2_ALF extends FrOpcodeDSIF
	{
		public CmpOp2_ALF(FrCompareOperator op, int d, int s, double t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.argi[d]=(long)op.op(frm.locf[s], imm); }
		FrCompareOperator op;
	}

	private static final class CmpOp2_LRF extends FrOpcodeDSIF
	{
		public CmpOp2_LRF(FrCompareOperator op, int d, int s, double t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.loci[d]=(long)op.op(frm.regf[s], imm); }
		FrCompareOperator op;
	}

	private static final class CmpOp2_LAF extends FrOpcodeDSIF
	{
		public CmpOp2_LAF(FrCompareOperator op, int d, int s, double t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.loci[d]=(long)op.op(frm.argf[s], imm); }
		FrCompareOperator op;
	}

	private static final class CmpOp2_LLF extends FrOpcodeDSIF
	{
		public CmpOp2_LLF(FrCompareOperator op, int d, int s, double t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.loci[d]=(long)op.op(frm.locf[s], imm); }
		FrCompareOperator op;
	}
}
