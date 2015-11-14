package fricore;

public abstract class FrUnOpImm extends FrOpcode
{
	public static FrOpcode makeUnOpImm(
		int z, int y, int op, int d, long t)
	{
		switch(z)
		{
		case FR2C_TY_I:
		case FR2C_TY_L:
			switch(y)
			{
			case FR2C_OPMODE_RR:
				return new UnOp2_AI(
					FrUnaryOperator.getOperator(op),
					d, t);
			case FR2C_OPMODE_RA:
				return new UnOp2_AI(
					FrUnaryOperator.getOperator(op),
					d, t);
			case FR2C_OPMODE_RL:
				return new UnOp2_LI(
					FrUnaryOperator.getOperator(op),
					d, t);
			}
		}
		return null;
	}

	public static FrOpcode makeUnOpImm(
		int z, int y, int op, int d, double t)

	{
		switch(z)
		{
		case FR2C_TY_F:
		case FR2C_TY_D:
			switch(y)
			{
			case FR2C_OPMODE_RR:
				return new UnOp2_RF(
					FrUnaryOperator.getOperator(op),
					d, t);
			case FR2C_OPMODE_RA:
				return new UnOp2_AF(
					FrUnaryOperator.getOperator(op),
					d, t);
			case FR2C_OPMODE_RL:
				return new UnOp2_LF(
					FrUnaryOperator.getOperator(op),
					d, t);
			}
		}
		return null;
	}



	//Indirect Int

	private static final class UnOp2_RI extends FrOpcodeDSII
	{
		public UnOp2_RI(FrUnaryOperator op, int d, long t)
			{ super(d, 0, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=op.op(imm); }
		FrUnaryOperator op;
	}

	private static final class UnOp2_AI extends FrOpcodeDSII
	{
		public UnOp2_AI(FrUnaryOperator op, int d, long t)
			{ super(d, 0, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.argi[d]=op.op(imm); }
		FrUnaryOperator op;
	}

	private static final class UnOp2_LI extends FrOpcodeDSII
	{
		public UnOp2_LI(FrUnaryOperator op, int d, long t)
			{ super(d, 0, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.loci[d]=op.op(imm); }
		FrUnaryOperator op;
	}


	//Indirect Float

	private static final class UnOp2_RF extends FrOpcodeDSIF
	{
		public UnOp2_RF(FrUnaryOperator op, int d, double t)
			{ super(d, 0, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regf[d]=op.op(imm); }
		FrUnaryOperator op;
	}

	private static final class UnOp2_AF extends FrOpcodeDSIF
	{
		public UnOp2_AF(FrUnaryOperator op, int d, double t)
			{ super(d, 0, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.argf[d]=op.op(imm); }
		FrUnaryOperator op;
	}

	private static final class UnOp2_LF extends FrOpcodeDSIF
	{
		public UnOp2_LF(FrUnaryOperator op, int d, double t)
			{ super(d, 0, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.locf[d]=op.op(imm); }
		FrUnaryOperator op;
	}
}
