package fricore;

public abstract class FrUnOp extends FrOpcode
{
	public static FrOpcode makeUnOp(
		int z, int y, int op, int d, int s)
	{
		switch(z)
		{
		case FR2C_TY_I:
		case FR2C_TY_L:
			switch(y)
			{
			case FR2C_OPMODE_RR:
				return new UnOp2_RRI(
					FrUnaryOperator.getOperator(op),
					d, s);
			case FR2C_OPMODE_RA:
				return new UnOp2_RAI(
					FrUnaryOperator.getOperator(op),
					d, s);
			case FR2C_OPMODE_RL:
				return new UnOp2_RLI(
					FrUnaryOperator.getOperator(op),
					d, s);
			case FR2C_OPMODE_AR:
				return new UnOp2_ARI(
					FrUnaryOperator.getOperator(op),
					d, s);
			case FR2C_OPMODE_AA:
				return new UnOp2_AAI(
					FrUnaryOperator.getOperator(op),
					d, s);
			case FR2C_OPMODE_AL:
				return new UnOp2_ALI(
					FrUnaryOperator.getOperator(op),
					d, s);
			case FR2C_OPMODE_LR:
				return new UnOp2_LRI(
					FrUnaryOperator.getOperator(op),
					d, s);
			case FR2C_OPMODE_LA:
				return new UnOp2_LAI(
					FrUnaryOperator.getOperator(op),
					d, s);
			case FR2C_OPMODE_LL:
				return new UnOp2_LLI(
					FrUnaryOperator.getOperator(op),
					d, s);
			}
		case FR2C_TY_F:
		case FR2C_TY_D:
			switch(y)
			{
			case FR2C_OPMODE_RR:
				return new UnOp2_RRF(
					FrUnaryOperator.getOperator(op),
					d, s);
			case FR2C_OPMODE_RA:
				return new UnOp2_RAF(
					FrUnaryOperator.getOperator(op),
					d, s);
			case FR2C_OPMODE_RL:
				return new UnOp2_RLF(
					FrUnaryOperator.getOperator(op),
					d, s);
			case FR2C_OPMODE_AR:
				return new UnOp2_ARF(
					FrUnaryOperator.getOperator(op),
					d, s);
			case FR2C_OPMODE_AA:
				return new UnOp2_AAF(
					FrUnaryOperator.getOperator(op),
					d, s);
			case FR2C_OPMODE_AL:
				return new UnOp2_ALF(
					FrUnaryOperator.getOperator(op),
					d, s);
			case FR2C_OPMODE_LR:
				return new UnOp2_LRF(
					FrUnaryOperator.getOperator(op),
					d, s);
			case FR2C_OPMODE_LA:
				return new UnOp2_LAF(
					FrUnaryOperator.getOperator(op),
					d, s);
			case FR2C_OPMODE_LL:
				return new UnOp2_LLF(
					FrUnaryOperator.getOperator(op),
					d, s);
			}
		}
		return null;
	}


	//Indirect Int

	private static final class UnOp2_RRI extends FrOpcodeDST
	{
		public UnOp2_RRI(FrUnaryOperator op, int d, int s)
			{ super(d, s, 0); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=op.op(frm.regi[s]); }
		FrUnaryOperator op;
	}

	private static final class UnOp2_RAI extends FrOpcodeDST
	{
		public UnOp2_RAI(FrUnaryOperator op, int d, int s)
			{ super(d, s, 0); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=op.op(frm.argi[s]); }
		FrUnaryOperator op;
	}

	private static final class UnOp2_RLI extends FrOpcodeDST
	{
		public UnOp2_RLI(FrUnaryOperator op, int d, int s)
			{ super(d, s, 0); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=op.op(frm.loci[s]); }
		FrUnaryOperator op;
	}

	private static final class UnOp2_ARI extends FrOpcodeDST
	{
		public UnOp2_ARI(FrUnaryOperator op, int d, int s)
			{ super(d, s, 0); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.argi[d]=op.op(frm.regi[s]); }
		FrUnaryOperator op;
	}

	private static final class UnOp2_AAI extends FrOpcodeDST
	{
		public UnOp2_AAI(FrUnaryOperator op, int d, int s)
			{ super(d, s, 0); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.argi[d]=op.op(frm.argi[s]); }
		FrUnaryOperator op;
	}

	private static final class UnOp2_ALI extends FrOpcodeDST
	{
		public UnOp2_ALI(FrUnaryOperator op, int d, int s)
			{ super(d, s, 0); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.argi[d]=op.op(frm.loci[s]); }
		FrUnaryOperator op;
	}

	private static final class UnOp2_LRI extends FrOpcodeDST
	{
		public UnOp2_LRI(FrUnaryOperator op, int d, int s)
			{ super(d, s, 0); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.loci[d]=op.op(frm.regi[s]); }
		FrUnaryOperator op;
	}

	private static final class UnOp2_LAI extends FrOpcodeDST
	{
		public UnOp2_LAI(FrUnaryOperator op, int d, int s)
			{ super(d, s, 0); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.loci[d]=op.op(frm.argi[s]); }
		FrUnaryOperator op;
	}

	private static final class UnOp2_LLI extends FrOpcodeDST
	{
		public UnOp2_LLI(FrUnaryOperator op, int d, int s)
			{ super(d, s, 0); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.loci[d]=op.op(frm.loci[s]); }
		FrUnaryOperator op;
	}


	//Indirect Float

	private static final class UnOp2_RRF extends FrOpcodeDST
	{
		public UnOp2_RRF(FrUnaryOperator op, int d, int s)
			{ super(d, s, 0); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regf[d]=op.op(frm.regf[s]); }
		FrUnaryOperator op;
	}

	private static final class UnOp2_RAF extends FrOpcodeDST
	{
		public UnOp2_RAF(FrUnaryOperator op, int d, int s)
			{ super(d, s, 0); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regf[d]=op.op(frm.argf[s]); }
		FrUnaryOperator op;
	}

	private static final class UnOp2_RLF extends FrOpcodeDST
	{
		public UnOp2_RLF(FrUnaryOperator op, int d, int s)
			{ super(d, s, 0); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regf[d]=op.op(frm.locf[s]); }
		FrUnaryOperator op;
	}

	private static final class UnOp2_ARF extends FrOpcodeDST
	{
		public UnOp2_ARF(FrUnaryOperator op, int d, int s)
			{ super(d, s, 0); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.argf[d]=op.op(frm.regf[s]); }
		FrUnaryOperator op;
	}

	private static final class UnOp2_AAF extends FrOpcodeDST
	{
		public UnOp2_AAF(FrUnaryOperator op, int d, int s)
			{ super(d, s, 0); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.argf[d]=op.op(frm.argf[s]); }
		FrUnaryOperator op;
	}

	private static final class UnOp2_ALF extends FrOpcodeDST
	{
		public UnOp2_ALF(FrUnaryOperator op, int d, int s)
			{ super(d, s, 0); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.argf[d]=op.op(frm.locf[s]); }
		FrUnaryOperator op;
	}

	private static final class UnOp2_LRF extends FrOpcodeDST
	{
		public UnOp2_LRF(FrUnaryOperator op, int d, int s)
			{ super(d, s, 0); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.locf[d]=op.op(frm.regf[s]); }
		FrUnaryOperator op;
	}

	private static final class UnOp2_LAF extends FrOpcodeDST
	{
		public UnOp2_LAF(FrUnaryOperator op, int d, int s)
			{ super(d, s, 0); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.locf[d]=op.op(frm.argf[s]); }
		FrUnaryOperator op;
	}

	private static final class UnOp2_LLF extends FrOpcodeDST
	{
		public UnOp2_LLF(FrUnaryOperator op, int d, int s)
			{ super(d, s, 0); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.locf[d]=op.op(frm.locf[s]); }
		FrUnaryOperator op;
	}
}
