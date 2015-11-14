package fricore;

public abstract class FrBinOp extends FrOpcode
{
	public static FrOpcode makeBinOp(
		int z, int y, int op, int d, int s, int t)
	{
		switch(z)
		{
		case FR2C_TY_I:
		case FR2C_TY_L:
			switch(y)
			{
			case FR2C_OPMODE_RRR:
				switch(op)
				{
				case FR2C_BINOP_ADD:
					return new BinOp_AddI_RRR(d, s, t);
				case FR2C_BINOP_SUB:
					return new BinOp_SubI_RRR(d, s, t);
				case FR2C_BINOP_MUL:
					return new BinOp_MulI_RRR(d, s, t);
				case FR2C_BINOP_DIV:
					return new BinOp_DivI_RRR(d, s, t);
				case FR2C_BINOP_MOD:
					return new BinOp_ModI_RRR(d, s, t);
				case FR2C_BINOP_AND:
					return new BinOp_AndI_RRR(d, s, t);
				case FR2C_BINOP_OR:
					return new BinOp_OrI_RRR(d, s, t);
				case FR2C_BINOP_XOR:
					return new BinOp_XorI_RRR(d, s, t);

				case FR2C_BINOP_SHL:
					return new BinOp_ShlI_RRR(d, s, t);
				case FR2C_BINOP_SHR:
					return new BinOp_ShrI_RRR(d, s, t);
				case FR2C_BINOP_SHRR:
					return new BinOp_ShrrI_RRR(d, s, t);
//				case FR2C_BINOP_DIV:
//					return new BinOp_DivI_RRR(d, s, t);
				default:
					break;
				}
				break;
			case FR2C_OPMODE_RRA:
				return new BinOp2_RRAI(
					FrBinaryOperator.getOperator(op),
					d, s, t);
			case FR2C_OPMODE_RRL:
				return new BinOp2_RRLI(
					FrBinaryOperator.getOperator(op),
					d, s, t);

			case FR2C_OPMODE_RAR:
				return new BinOp2_RARI(
					FrBinaryOperator.getOperator(op),
					d, s, t);
			case FR2C_OPMODE_RAA:
				return new BinOp2_RAAI(
					FrBinaryOperator.getOperator(op),
					d, s, t);
			case FR2C_OPMODE_RAL:
				return new BinOp2_RALI(
					FrBinaryOperator.getOperator(op),
					d, s, t);

			case FR2C_OPMODE_RLR:
				return new BinOp2_RLRI(
					FrBinaryOperator.getOperator(op),
					d, s, t);
			case FR2C_OPMODE_RLA:
				return new BinOp2_RLAI(
					FrBinaryOperator.getOperator(op),
					d, s, t);
			case FR2C_OPMODE_RLL:
				return new BinOp2_RLLI(
					FrBinaryOperator.getOperator(op),
					d, s, t);
			}

		case FR2C_TY_F:
		case FR2C_TY_D:
			switch(y)
			{
			case FR2C_OPMODE_RRR:
				return new BinOp2_RRRF(
					FrBinaryOperator.getOperator(op),
					d, s, t);
			case FR2C_OPMODE_RRA:
				return new BinOp2_RRAF(
					FrBinaryOperator.getOperator(op),
					d, s, t);
			case FR2C_OPMODE_RRL:
				return new BinOp2_RRLF(
					FrBinaryOperator.getOperator(op),
					d, s, t);

			case FR2C_OPMODE_RAR:
				return new BinOp2_RARF(
					FrBinaryOperator.getOperator(op),
					d, s, t);
			case FR2C_OPMODE_RAA:
				return new BinOp2_RAAF(
					FrBinaryOperator.getOperator(op),
					d, s, t);
			case FR2C_OPMODE_RAL:
				return new BinOp2_RALF(
					FrBinaryOperator.getOperator(op),
					d, s, t);

			case FR2C_OPMODE_RLR:
				return new BinOp2_RLRF(
					FrBinaryOperator.getOperator(op),
					d, s, t);
			case FR2C_OPMODE_RLA:
				return new BinOp2_RLAF(
					FrBinaryOperator.getOperator(op),
					d, s, t);
			case FR2C_OPMODE_RLL:
				return new BinOp2_RLLF(
					FrBinaryOperator.getOperator(op),
					d, s, t);
			}
		}
		return null;
	}


	//Indirect Int

	private static final class BinOp2_RRRI extends FrOpcodeDST
	{
		public BinOp2_RRRI(FrBinaryOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=op.op(frm.regi[s], frm.regi[t]); }
		FrBinaryOperator op;
	}

	private static final class BinOp2_RRAI extends FrOpcodeDST
	{
		public BinOp2_RRAI(FrBinaryOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=op.op(frm.regi[s], frm.argi[t]); }
		FrBinaryOperator op;
	}

	private static final class BinOp2_RRLI extends FrOpcodeDST
	{
		public BinOp2_RRLI(FrBinaryOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=op.op(frm.regi[s], frm.loci[t]); }
		FrBinaryOperator op;
	}

	private static final class BinOp2_RARI extends FrOpcodeDST
	{
		public BinOp2_RARI(FrBinaryOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=op.op(frm.argi[s], frm.regi[t]); }
		FrBinaryOperator op;
	}

	private static final class BinOp2_RAAI extends FrOpcodeDST
	{
		public BinOp2_RAAI(FrBinaryOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=op.op(frm.argi[s], frm.argi[t]); }
		FrBinaryOperator op;
	}

	private static final class BinOp2_RALI extends FrOpcodeDST
	{
		public BinOp2_RALI(FrBinaryOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=op.op(frm.argi[s], frm.loci[t]); }
		FrBinaryOperator op;
	}

	private static final class BinOp2_RLRI extends FrOpcodeDST
	{
		public BinOp2_RLRI(FrBinaryOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=op.op(frm.loci[s], frm.regi[t]); }
		FrBinaryOperator op;
	}

	private static final class BinOp2_RLAI extends FrOpcodeDST
	{
		public BinOp2_RLAI(FrBinaryOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=op.op(frm.loci[s], frm.argi[t]); }
		FrBinaryOperator op;
	}

	private static final class BinOp2_RLLI extends FrOpcodeDST
	{
		public BinOp2_RLLI(FrBinaryOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=op.op(frm.loci[s], frm.loci[t]); }
		FrBinaryOperator op;
	}

	private static final class BinOp2_ARRI extends FrOpcodeDST
	{
		public BinOp2_ARRI(FrBinaryOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.argi[d]=op.op(frm.regi[s], frm.regi[t]); }
		FrBinaryOperator op;
	}

	private static final class BinOp2_LRRI extends FrOpcodeDST
	{
		public BinOp2_LRRI(FrBinaryOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.loci[d]=op.op(frm.regi[s], frm.regi[t]); }
		FrBinaryOperator op;
	}

	private static final class BinOp2_LRAI extends FrOpcodeDST
	{
		public BinOp2_LRAI(FrBinaryOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.loci[d]=op.op(frm.regi[s], frm.argi[t]); }
		FrBinaryOperator op;
	}

	private static final class BinOp2_LARI extends FrOpcodeDST
	{
		public BinOp2_LARI(FrBinaryOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.loci[d]=op.op(frm.argi[s], frm.regi[t]); }
		FrBinaryOperator op;
	}

	private static final class BinOp2_LRLI extends FrOpcodeDST
	{
		public BinOp2_LRLI(FrBinaryOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.loci[d]=op.op(frm.regi[s], frm.loci[t]); }
		FrBinaryOperator op;
	}

	private static final class BinOp2_LLRI extends FrOpcodeDST
	{
		public BinOp2_LLRI(FrBinaryOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.loci[d]=op.op(frm.loci[s], frm.regi[t]); }
		FrBinaryOperator op;
	}

	private static final class BinOp2_LLLI extends FrOpcodeDST
	{
		public BinOp2_LLLI(FrBinaryOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.loci[d]=op.op(frm.loci[s], frm.loci[t]); }
		FrBinaryOperator op;
	}


	//Indirect Float

	private static final class BinOp2_RRRF extends FrOpcodeDST
	{
		public BinOp2_RRRF(FrBinaryOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regf[d]=op.op(frm.regf[s], frm.regf[t]); }
		FrBinaryOperator op;
	}

	private static final class BinOp2_RRAF extends FrOpcodeDST
	{
		public BinOp2_RRAF(FrBinaryOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regf[d]=op.op(frm.regf[s], frm.argf[t]); }
		FrBinaryOperator op;
	}

	private static final class BinOp2_RRLF extends FrOpcodeDST
	{
		public BinOp2_RRLF(FrBinaryOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regf[d]=op.op(frm.regf[s], frm.locf[t]); }
		FrBinaryOperator op;
	}

	private static final class BinOp2_RARF extends FrOpcodeDST
	{
		public BinOp2_RARF(FrBinaryOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regf[d]=op.op(frm.argf[s], frm.regf[t]); }
		FrBinaryOperator op;
	}

	private static final class BinOp2_RAAF extends FrOpcodeDST
	{
		public BinOp2_RAAF(FrBinaryOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regf[d]=op.op(frm.argf[s], frm.argf[t]); }
		FrBinaryOperator op;
	}

	private static final class BinOp2_RALF extends FrOpcodeDST
	{
		public BinOp2_RALF(FrBinaryOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regf[d]=op.op(frm.argf[s], frm.locf[t]); }
		FrBinaryOperator op;
	}

	private static final class BinOp2_RLRF extends FrOpcodeDST
	{
		public BinOp2_RLRF(FrBinaryOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regf[d]=op.op(frm.locf[s], frm.regf[t]); }
		FrBinaryOperator op;
	}

	private static final class BinOp2_RLAF extends FrOpcodeDST
	{
		public BinOp2_RLAF(FrBinaryOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regf[d]=op.op(frm.locf[s], frm.argf[t]); }
		FrBinaryOperator op;
	}

	private static final class BinOp2_RLLF extends FrOpcodeDST
	{
		public BinOp2_RLLF(FrBinaryOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regf[d]=op.op(frm.locf[s], frm.locf[t]); }
		FrBinaryOperator op;
	}

	private static final class BinOp2_ARRF extends FrOpcodeDST
	{
		public BinOp2_ARRF(FrBinaryOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.argf[d]=op.op(frm.regf[s], frm.regf[t]); }
		FrBinaryOperator op;
	}

	private static final class BinOp2_LRRF extends FrOpcodeDST
	{
		public BinOp2_LRRF(FrBinaryOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.locf[d]=op.op(frm.regf[s], frm.regf[t]); }
		FrBinaryOperator op;
	}

	private static final class BinOp2_LRAF extends FrOpcodeDST
	{
		public BinOp2_LRAF(FrBinaryOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.locf[d]=op.op(frm.regf[s], frm.argf[t]); }
		FrBinaryOperator op;
	}

	private static final class BinOp2_LARF extends FrOpcodeDST
	{
		public BinOp2_LARF(FrBinaryOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.locf[d]=op.op(frm.argf[s], frm.regf[t]); }
		FrBinaryOperator op;
	}

	private static final class BinOp2_LRLF extends FrOpcodeDST
	{
		public BinOp2_LRLF(FrBinaryOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.locf[d]=op.op(frm.regf[s], frm.locf[t]); }
		FrBinaryOperator op;
	}

	private static final class BinOp2_LLRF extends FrOpcodeDST
	{
		public BinOp2_LLRF(FrBinaryOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.locf[d]=op.op(frm.locf[s], frm.regf[t]); }
		FrBinaryOperator op;
	}

	private static final class BinOp2_LLLF extends FrOpcodeDST
	{
		public BinOp2_LLLF(FrBinaryOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.locf[d]=op.op(frm.locf[s], frm.locf[t]); }
		FrBinaryOperator op;
	}


	//Direct
	private static class BinOp_AddI_RRR extends FrOpcodeDST
	{
		public BinOp_AddI_RRR(int d, int s, int t)
			{ super(d, s, t); }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=frm.regi[s]+frm.regi[t]; }
	}

	private static class BinOp_SubI_RRR extends FrOpcodeDST
	{
		public BinOp_SubI_RRR(int d, int s, int t)
			{ super(d, s, t); }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=frm.regi[s]-frm.regi[t]; }
	}

	private static class BinOp_MulI_RRR extends FrOpcodeDST
	{
		public BinOp_MulI_RRR(int d, int s, int t)
			{ super(d, s, t); }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=frm.regi[s]*frm.regi[t]; }
	}

	private static class BinOp_DivI_RRR extends FrOpcodeDST
	{
		public BinOp_DivI_RRR(int d, int s, int t)
			{ super(d, s, t); }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=frm.regi[s]/frm.regi[t]; }
	}

	private static class BinOp_ModI_RRR extends FrOpcodeDST
	{
		public BinOp_ModI_RRR(int d, int s, int t)
			{ super(d, s, t); }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=frm.regi[s]%frm.regi[t]; }
	}

	private static class BinOp_AndI_RRR extends FrOpcodeDST
	{
		public BinOp_AndI_RRR(int d, int s, int t)
			{ super(d, s, t); }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=frm.regi[s]&frm.regi[t]; }
	}

	private static class BinOp_OrI_RRR extends FrOpcodeDST
	{
		public BinOp_OrI_RRR(int d, int s, int t)
			{ super(d, s, t); }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=frm.regi[s]|frm.regi[t]; }
	}

	private static class BinOp_XorI_RRR extends FrOpcodeDST
	{
		public BinOp_XorI_RRR(int d, int s, int t)
			{ super(d, s, t); }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=frm.regi[s]^frm.regi[t]; }
	}

	private static class BinOp_ShlI_RRR extends FrOpcodeDST
	{
		public BinOp_ShlI_RRR(int d, int s, int t)
			{ super(d, s, t); }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=frm.regi[s]<<frm.regi[t]; }
	}

	private static class BinOp_ShrI_RRR extends FrOpcodeDST
	{
		public BinOp_ShrI_RRR(int d, int s, int t)
			{ super(d, s, t); }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=frm.regi[s]>>frm.regi[t]; }
	}

	private static class BinOp_ShrrI_RRR extends FrOpcodeDST
	{
		public BinOp_ShrrI_RRR(int d, int s, int t)
			{ super(d, s, t); }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=frm.regi[s]>>>frm.regi[t]; }
	}
}
