package fricore;

public abstract class FrCmpOp extends FrOpcode
{
	public static FrOpcode makeCmpOp(
		int z, int y, int op, int d, int s, int t)
	{
		switch(z)
		{
		case FR2C_TY_I:
		case FR2C_TY_L:
			switch(y)
			{
			case FR2C_OPMODE_RRR:
				return new CmpOp2_RRRI(
					FrCompareOperator.getOperator(op),
					d, s, t);
			case FR2C_OPMODE_RRA:
				return new CmpOp2_RRAI(
					FrCompareOperator.getOperator(op),
					d, s, t);
			case FR2C_OPMODE_RRL:
				return new CmpOp2_RRLI(
					FrCompareOperator.getOperator(op),
					d, s, t);

			case FR2C_OPMODE_RAR:
				return new CmpOp2_RARI(
					FrCompareOperator.getOperator(op),
					d, s, t);
			case FR2C_OPMODE_RAA:
				return new CmpOp2_RAAI(
					FrCompareOperator.getOperator(op),
					d, s, t);
			case FR2C_OPMODE_RAL:
				return new CmpOp2_RALI(
					FrCompareOperator.getOperator(op),
					d, s, t);

			case FR2C_OPMODE_RLR:
				return new CmpOp2_RLRI(
					FrCompareOperator.getOperator(op),
					d, s, t);
			case FR2C_OPMODE_RLA:
				return new CmpOp2_RLAI(
					FrCompareOperator.getOperator(op),
					d, s, t);
			case FR2C_OPMODE_RLL:
				return new CmpOp2_RLLI(
					FrCompareOperator.getOperator(op),
					d, s, t);
			}

		case FR2C_TY_F:
		case FR2C_TY_D:
			switch(y)
			{
			case FR2C_OPMODE_RRR:
				return new CmpOp2_RRRF(
					FrCompareOperator.getOperator(op),
					d, s, t);
			case FR2C_OPMODE_RRA:
				return new CmpOp2_RRAF(
					FrCompareOperator.getOperator(op),
					d, s, t);
			case FR2C_OPMODE_RRL:
				return new CmpOp2_RRLF(
					FrCompareOperator.getOperator(op),
					d, s, t);

			case FR2C_OPMODE_RAR:
				return new CmpOp2_RARF(
					FrCompareOperator.getOperator(op),
					d, s, t);
			case FR2C_OPMODE_RAA:
				return new CmpOp2_RAAF(
					FrCompareOperator.getOperator(op),
					d, s, t);
			case FR2C_OPMODE_RAL:
				return new CmpOp2_RALF(
					FrCompareOperator.getOperator(op),
					d, s, t);

			case FR2C_OPMODE_RLR:
				return new CmpOp2_RLRF(
					FrCompareOperator.getOperator(op),
					d, s, t);
			case FR2C_OPMODE_RLA:
				return new CmpOp2_RLAF(
					FrCompareOperator.getOperator(op),
					d, s, t);
			case FR2C_OPMODE_RLL:
				return new CmpOp2_RLLF(
					FrCompareOperator.getOperator(op),
					d, s, t);
			}
		}
		return null;
	}


	//Indirect Int

	private static final class CmpOp2_RRRI extends FrOpcodeDST
	{
		public CmpOp2_RRRI(FrCompareOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=(long)op.op(frm.regi[s], frm.regi[t]); }
		FrCompareOperator op;
	}

	private static final class CmpOp2_RRAI extends FrOpcodeDST
	{
		public CmpOp2_RRAI(FrCompareOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=(long)op.op(frm.regi[s], frm.argi[t]); }
		FrCompareOperator op;
	}

	private static final class CmpOp2_RRLI extends FrOpcodeDST
	{
		public CmpOp2_RRLI(FrCompareOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=(long)op.op(frm.regi[s], frm.loci[t]); }
		FrCompareOperator op;
	}

	private static final class CmpOp2_RARI extends FrOpcodeDST
	{
		public CmpOp2_RARI(FrCompareOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=(long)op.op(frm.argi[s], frm.regi[t]); }
		FrCompareOperator op;
	}

	private static final class CmpOp2_RAAI extends FrOpcodeDST
	{
		public CmpOp2_RAAI(FrCompareOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=(long)op.op(frm.argi[s], frm.argi[t]); }
		FrCompareOperator op;
	}

	private static final class CmpOp2_RALI extends FrOpcodeDST
	{
		public CmpOp2_RALI(FrCompareOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=(long)op.op(frm.argi[s], frm.loci[t]); }
		FrCompareOperator op;
	}

	private static final class CmpOp2_RLRI extends FrOpcodeDST
	{
		public CmpOp2_RLRI(FrCompareOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=(long)op.op(frm.loci[s], frm.regi[t]); }
		FrCompareOperator op;
	}

	private static final class CmpOp2_RLAI extends FrOpcodeDST
	{
		public CmpOp2_RLAI(FrCompareOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=(long)op.op(frm.loci[s], frm.argi[t]); }
		FrCompareOperator op;
	}

	private static final class CmpOp2_RLLI extends FrOpcodeDST
	{
		public CmpOp2_RLLI(FrCompareOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=(long)op.op(frm.loci[s], frm.loci[t]); }
		FrCompareOperator op;
	}

	private static final class CmpOp2_ARRI extends FrOpcodeDST
	{
		public CmpOp2_ARRI(FrCompareOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.argi[d]=(long)op.op(frm.regi[s], frm.regi[t]); }
		FrCompareOperator op;
	}

	private static final class CmpOp2_LRRI extends FrOpcodeDST
	{
		public CmpOp2_LRRI(FrCompareOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.loci[d]=(long)op.op(frm.regi[s], frm.regi[t]); }
		FrCompareOperator op;
	}

	private static final class CmpOp2_LRAI extends FrOpcodeDST
	{
		public CmpOp2_LRAI(FrCompareOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.loci[d]=(long)op.op(frm.regi[s], frm.argi[t]); }
		FrCompareOperator op;
	}

	private static final class CmpOp2_LARI extends FrOpcodeDST
	{
		public CmpOp2_LARI(FrCompareOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.loci[d]=(long)op.op(frm.argi[s], frm.regi[t]); }
		FrCompareOperator op;
	}

	private static final class CmpOp2_LRLI extends FrOpcodeDST
	{
		public CmpOp2_LRLI(FrCompareOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.loci[d]=(long)op.op(frm.regi[s], frm.loci[t]); }
		FrCompareOperator op;
	}

	private static final class CmpOp2_LLRI extends FrOpcodeDST
	{
		public CmpOp2_LLRI(FrCompareOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.loci[d]=(long)op.op(frm.loci[s], frm.regi[t]); }
		FrCompareOperator op;
	}

	private static final class CmpOp2_LLLI extends FrOpcodeDST
	{
		public CmpOp2_LLLI(FrCompareOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.loci[d]=(long)op.op(frm.loci[s], frm.loci[t]); }
		FrCompareOperator op;
	}


	//Indirect Float

	private static final class CmpOp2_RRRF extends FrOpcodeDST
	{
		public CmpOp2_RRRF(FrCompareOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=(long)op.op(frm.regf[s], frm.regf[t]); }
		FrCompareOperator op;
	}

	private static final class CmpOp2_RRAF extends FrOpcodeDST
	{
		public CmpOp2_RRAF(FrCompareOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=(long)op.op(frm.regf[s], frm.argf[t]); }
		FrCompareOperator op;
	}

	private static final class CmpOp2_RRLF extends FrOpcodeDST
	{
		public CmpOp2_RRLF(FrCompareOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=(long)op.op(frm.regf[s], frm.locf[t]); }
		FrCompareOperator op;
	}

	private static final class CmpOp2_RARF extends FrOpcodeDST
	{
		public CmpOp2_RARF(FrCompareOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=(long)op.op(frm.argf[s], frm.regf[t]); }
		FrCompareOperator op;
	}

	private static final class CmpOp2_RAAF extends FrOpcodeDST
	{
		public CmpOp2_RAAF(FrCompareOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=(long)op.op(frm.argf[s], frm.argf[t]); }
		FrCompareOperator op;
	}

	private static final class CmpOp2_RALF extends FrOpcodeDST
	{
		public CmpOp2_RALF(FrCompareOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=(long)op.op(frm.argf[s], frm.locf[t]); }
		FrCompareOperator op;
	}

	private static final class CmpOp2_RLRF extends FrOpcodeDST
	{
		public CmpOp2_RLRF(FrCompareOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=(long)op.op(frm.locf[s], frm.regf[t]); }
		FrCompareOperator op;
	}

	private static final class CmpOp2_RLAF extends FrOpcodeDST
	{
		public CmpOp2_RLAF(FrCompareOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=(long)op.op(frm.locf[s], frm.argf[t]); }
		FrCompareOperator op;
	}

	private static final class CmpOp2_RLLF extends FrOpcodeDST
	{
		public CmpOp2_RLLF(FrCompareOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=(long)op.op(frm.locf[s], frm.locf[t]); }
		FrCompareOperator op;
	}

	private static final class CmpOp2_ARRF extends FrOpcodeDST
	{
		public CmpOp2_ARRF(FrCompareOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.argi[d]=(long)op.op(frm.regf[s], frm.regf[t]); }
		FrCompareOperator op;
	}

	private static final class CmpOp2_LRRF extends FrOpcodeDST
	{
		public CmpOp2_LRRF(FrCompareOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.loci[d]=(long)op.op(frm.regf[s], frm.regf[t]); }
		FrCompareOperator op;
	}

	private static final class CmpOp2_LRAF extends FrOpcodeDST
	{
		public CmpOp2_LRAF(FrCompareOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.loci[d]=(long)op.op(frm.regf[s], frm.argf[t]); }
		FrCompareOperator op;
	}

	private static final class CmpOp2_LARF extends FrOpcodeDST
	{
		public CmpOp2_LARF(FrCompareOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.loci[d]=(long)op.op(frm.argf[s], frm.regf[t]); }
		FrCompareOperator op;
	}

	private static final class CmpOp2_LRLF extends FrOpcodeDST
	{
		public CmpOp2_LRLF(FrCompareOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.loci[d]=(long)op.op(frm.regf[s], frm.locf[t]); }
		FrCompareOperator op;
	}

	private static final class CmpOp2_LLRF extends FrOpcodeDST
	{
		public CmpOp2_LLRF(FrCompareOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.loci[d]=(long)op.op(frm.locf[s], frm.regf[t]); }
		FrCompareOperator op;
	}

	private static final class CmpOp2_LLLF extends FrOpcodeDST
	{
		public CmpOp2_LLLF(FrCompareOperator op, int d, int s, int t)
			{ super(d, s, t); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.loci[d]=(long)op.op(frm.locf[s], frm.locf[t]); }
		FrCompareOperator op;
	}
}
