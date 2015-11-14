package fricore;

public abstract class FrBinOpImm extends FrOpcode
{
	public static FrOpcode makeBinOpImm(
		int z, int y, int op, int d, int s, long t)
	{
		switch(z)
		{
		case FR2C_TY_I:
		case FR2C_TY_L:
			switch(y)
			{
			case FR2C_OPMODE_RR:
				switch(op)
				{
				case FR2C_BINOP_ADD:
					return new BinOp_AddI_RR(d, s, t);
				case FR2C_BINOP_SUB:
					return new BinOp_SubI_RR(d, s, t);
				case FR2C_BINOP_MUL:
					return new BinOp_MulI_RR(d, s, t);
				case FR2C_BINOP_DIV:
					return new BinOp_DivI_RR(d, s, t);
				case FR2C_BINOP_MOD:
					return new BinOp_ModI_RR(d, s, t);
				case FR2C_BINOP_AND:
					return new BinOp_AndI_RR(d, s, t);
				case FR2C_BINOP_OR:
					return new BinOp_OrI_RR(d, s, t);
				case FR2C_BINOP_XOR:
					return new BinOp_XorI_RR(d, s, t);

				case FR2C_BINOP_SHL:
					return new BinOp_ShlI_RR(d, s, t);
				case FR2C_BINOP_SHR:
					return new BinOp_ShrI_RR(d, s, t);
				case FR2C_BINOP_SHRR:
					return new BinOp_ShrrI_RR(d, s, t);
//				case FR2C_BINOP_DIV:
//					return new BinOp_DivI_RRR(d, s, t);
				default:
					break;
				}
				break;
			case FR2C_OPMODE_RA:
				return new BinOp2_RAI(
					FrBinaryOperator.getOperator(op),
					d, s, t);
			case FR2C_OPMODE_RL:
				return new BinOp2_RLI(
					FrBinaryOperator.getOperator(op),
					d, s, t);

			case FR2C_OPMODE_AR:
				return new BinOp2_ARI(
					FrBinaryOperator.getOperator(op),
					d, s, t);
			case FR2C_OPMODE_AA:
				return new BinOp2_AAI(
					FrBinaryOperator.getOperator(op),
					d, s, t);
			case FR2C_OPMODE_AL:
				return new BinOp2_ALI(
					FrBinaryOperator.getOperator(op),
					d, s, t);

			case FR2C_OPMODE_LR:
				return new BinOp2_ARI(
					FrBinaryOperator.getOperator(op),
					d, s, t);
			case FR2C_OPMODE_LA:
				return new BinOp2_AAI(
					FrBinaryOperator.getOperator(op),
					d, s, t);
			case FR2C_OPMODE_LL:
				return new BinOp2_ALI(
					FrBinaryOperator.getOperator(op),
					d, s, t);
			}
		}
		return null;
	}

	public static FrOpcode makeBinOpImm(
		int z, int y, int op, int d, int s, double t)

	{
		switch(z)
		{
		case FR2C_TY_F:
		case FR2C_TY_D:
			switch(y)
			{
			case FR2C_OPMODE_RR:
				return new BinOp2_RRF(
					FrBinaryOperator.getOperator(op),
					d, s, t);
			case FR2C_OPMODE_RA:
				return new BinOp2_RAF(
					FrBinaryOperator.getOperator(op),
					d, s, t);
			case FR2C_OPMODE_RL:
				return new BinOp2_RLF(
					FrBinaryOperator.getOperator(op),
					d, s, t);
			case FR2C_OPMODE_AR:
				return new BinOp2_ARF(
					FrBinaryOperator.getOperator(op),
					d, s, t);
			case FR2C_OPMODE_AA:
				return new BinOp2_AAF(
					FrBinaryOperator.getOperator(op),
					d, s, t);
			case FR2C_OPMODE_AL:
				return new BinOp2_ALF(
					FrBinaryOperator.getOperator(op),
					d, s, t);
			case FR2C_OPMODE_LR:
				return new BinOp2_ARF(
					FrBinaryOperator.getOperator(op),
					d, s, t);
			case FR2C_OPMODE_LA:
				return new BinOp2_AAF(
					FrBinaryOperator.getOperator(op),
					d, s, t);
			case FR2C_OPMODE_LL:
				return new BinOp2_ALF(
					FrBinaryOperator.getOperator(op),
					d, s, t);
			}
		}
		return null;
	}



	//Indirect Int

	private static final class BinOp2_RRI extends FrOpcodeDSII
	{
		public BinOp2_RRI(FrBinaryOperator op, int d, int s, long t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=op.op(frm.regi[s], imm); }
		FrBinaryOperator op;
	}

	private static final class BinOp2_RAI extends FrOpcodeDSII
	{
		public BinOp2_RAI(FrBinaryOperator op, int d, int s, long t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=op.op(frm.argi[s], imm); }
		FrBinaryOperator op;
	}

	private static final class BinOp2_RLI extends FrOpcodeDSII
	{
		public BinOp2_RLI(FrBinaryOperator op, int d, int s, long t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=op.op(frm.loci[s], imm); }
		FrBinaryOperator op;
	}

	private static final class BinOp2_ARI extends FrOpcodeDSII
	{
		public BinOp2_ARI(FrBinaryOperator op, int d, int s, long t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.argi[d]=op.op(frm.regi[s], imm); }
		FrBinaryOperator op;
	}

	private static final class BinOp2_AAI extends FrOpcodeDSII
	{
		public BinOp2_AAI(FrBinaryOperator op, int d, int s, long t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.argi[d]=op.op(frm.argi[s], imm); }
		FrBinaryOperator op;
	}

	private static final class BinOp2_ALI extends FrOpcodeDSII
	{
		public BinOp2_ALI(FrBinaryOperator op, int d, int s, long t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.argi[d]=op.op(frm.loci[s], imm); }
		FrBinaryOperator op;
	}

	private static final class BinOp2_LRI extends FrOpcodeDSII
	{
		public BinOp2_LRI(FrBinaryOperator op, int d, int s, long t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.loci[d]=op.op(frm.regi[s], imm); }
		FrBinaryOperator op;
	}

	private static final class BinOp2_LAI extends FrOpcodeDSII
	{
		public BinOp2_LAI(FrBinaryOperator op, int d, int s, long t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.loci[d]=op.op(frm.argi[s], imm); }
		FrBinaryOperator op;
	}

	private static final class BinOp2_LLI extends FrOpcodeDSII
	{
		public BinOp2_LLI(FrBinaryOperator op, int d, int s, long t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.loci[d]=op.op(frm.loci[s], imm); }
		FrBinaryOperator op;
	}


	//Indirect Float

	private static final class BinOp2_RRF extends FrOpcodeDSIF
	{
		public BinOp2_RRF(FrBinaryOperator op, int d, int s, double t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regf[d]=op.op(frm.regf[s], imm); }
		FrBinaryOperator op;
	}

	private static final class BinOp2_RAF extends FrOpcodeDSIF
	{
		public BinOp2_RAF(FrBinaryOperator op, int d, int s, double t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regf[d]=op.op(frm.argf[s], imm); }
		FrBinaryOperator op;
	}

	private static final class BinOp2_RLF extends FrOpcodeDSIF
	{
		public BinOp2_RLF(FrBinaryOperator op, int d, int s, double t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regf[d]=op.op(frm.locf[s], imm); }
		FrBinaryOperator op;
	}

	private static final class BinOp2_ARF extends FrOpcodeDSIF
	{
		public BinOp2_ARF(FrBinaryOperator op, int d, int s, double t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.argf[d]=op.op(frm.regf[s], imm); }
		FrBinaryOperator op;
	}

	private static final class BinOp2_AAF extends FrOpcodeDSIF
	{
		public BinOp2_AAF(FrBinaryOperator op, int d, int s, double t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.argf[d]=op.op(frm.argf[s], imm); }
		FrBinaryOperator op;
	}

	private static final class BinOp2_ALF extends FrOpcodeDSIF
	{
		public BinOp2_ALF(FrBinaryOperator op, int d, int s, double t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.argf[d]=op.op(frm.locf[s], imm); }
		FrBinaryOperator op;
	}

	private static final class BinOp2_LRF extends FrOpcodeDSIF
	{
		public BinOp2_LRF(FrBinaryOperator op, int d, int s, double t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.locf[d]=op.op(frm.regf[s], imm); }
		FrBinaryOperator op;
	}

	private static final class BinOp2_LAF extends FrOpcodeDSIF
	{
		public BinOp2_LAF(FrBinaryOperator op, int d, int s, double t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.locf[d]=op.op(frm.argf[s], imm); }
		FrBinaryOperator op;
	}

	private static final class BinOp2_LLF extends FrOpcodeDSIF
	{
		public BinOp2_LLF(FrBinaryOperator op, int d, int s, double t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.locf[d]=op.op(frm.locf[s], imm); }
		FrBinaryOperator op;
	}


	//Direct
	private static class BinOp_AddI_RR extends FrOpcodeDSII
	{
		public BinOp_AddI_RR(int d, int s, long t)
			{ super(d, s, t); }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=frm.regi[s]+imm; }
	}

	private static class BinOp_SubI_RR extends FrOpcodeDSII
	{
		public BinOp_SubI_RR(int d, int s, long t)
			{ super(d, s, t); }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=frm.regi[s]-imm; }
	}

	private static class BinOp_MulI_RR extends FrOpcodeDSII
	{
		public BinOp_MulI_RR(int d, int s, long t)
			{ super(d, s, t); }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=frm.regi[s]*imm; }
	}

	private static class BinOp_DivI_RR extends FrOpcodeDSII
	{
		public BinOp_DivI_RR(int d, int s, long t)
			{ super(d, s, t); }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=frm.regi[s]/imm; }
	}

	private static class BinOp_ModI_RR extends FrOpcodeDSII
	{
		public BinOp_ModI_RR(int d, int s, long t)
			{ super(d, s, t); }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=frm.regi[s]%imm; }
	}

	private static class BinOp_AndI_RR extends FrOpcodeDSII
	{
		public BinOp_AndI_RR(int d, int s, long t)
			{ super(d, s, t); }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=frm.regi[s]&imm; }
	}

	private static class BinOp_OrI_RR extends FrOpcodeDSII
	{
		public BinOp_OrI_RR(int d, int s, long t)
			{ super(d, s, t); }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=frm.regi[s]|imm; }
	}

	private static class BinOp_XorI_RR extends FrOpcodeDSII
	{
		public BinOp_XorI_RR(int d, int s, long t)
			{ super(d, s, t); }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=frm.regi[s]^imm; }
	}

	private static class BinOp_ShlI_RR extends FrOpcodeDSII
	{
		public BinOp_ShlI_RR(int d, int s, long t)
			{ super(d, s, t); }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=frm.regi[s]<<imm; }
	}

	private static class BinOp_ShrI_RR extends FrOpcodeDSII
	{
		public BinOp_ShrI_RR(int d, int s, long t)
			{ super(d, s, t); }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=frm.regi[s]>>imm; }
	}

	private static class BinOp_ShrrI_RR extends FrOpcodeDSII
	{
		public BinOp_ShrrI_RR(int d, int s, long t)
			{ super(d, s, t); }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=frm.regi[s]>>>imm; }
	}
}
