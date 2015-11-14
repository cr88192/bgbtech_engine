package fricore;
import static fricore.FrOpcode.*;

public abstract class FrOpJCmp extends FrTailOpcode
{
	public static FrTailOpcode makeCmpOp(
		int z, int y, int op, int s, int t, int a)
	{
		switch(z)
		{
		case FR2C_TY_I:
		case FR2C_TY_L:
			switch(y)
			{
			case FR2C_OPMODE_RR:
				return new JCmpOp2_RRI(
					FrCompareOperator.getOperator(op),
					s, t, a);
			case FR2C_OPMODE_RA:
				return new JCmpOp2_RAI(
					FrCompareOperator.getOperator(op),
					s, t, a);
			case FR2C_OPMODE_RL:
				return new JCmpOp2_RLI(
					FrCompareOperator.getOperator(op),
					s, t, a);

			case FR2C_OPMODE_AR:
				return new JCmpOp2_ARI(
					FrCompareOperator.getOperator(op),
					s, t, a);
			case FR2C_OPMODE_AA:
				return new JCmpOp2_AAI(
					FrCompareOperator.getOperator(op),
					s, t, a);
			case FR2C_OPMODE_AL:
				return new JCmpOp2_ALI(
					FrCompareOperator.getOperator(op),
					s, t, a);

			case FR2C_OPMODE_LR:
				return new JCmpOp2_LRI(
					FrCompareOperator.getOperator(op),
					s, t, a);
			case FR2C_OPMODE_LA:
				return new JCmpOp2_LAI(
					FrCompareOperator.getOperator(op),
					s, t, a);
			case FR2C_OPMODE_LL:
				return new JCmpOp2_LLI(
					FrCompareOperator.getOperator(op),
					s, t, a);
			}

		case FR2C_TY_F:
		case FR2C_TY_D:
			switch(y)
			{
			case FR2C_OPMODE_RR:
				return new JCmpOp2_RRF(
					FrCompareOperator.getOperator(op),
					s, t, a);
			case FR2C_OPMODE_RA:
				return new JCmpOp2_RAF(
					FrCompareOperator.getOperator(op),
					s, t, a);
			case FR2C_OPMODE_RL:
				return new JCmpOp2_RLF(
					FrCompareOperator.getOperator(op),
					s, t, a);

			case FR2C_OPMODE_AR:
				return new JCmpOp2_ARF(
					FrCompareOperator.getOperator(op),
					s, t, a);
			case FR2C_OPMODE_AA:
				return new JCmpOp2_AAF(
					FrCompareOperator.getOperator(op),
					s, t, a);
			case FR2C_OPMODE_AL:
				return new JCmpOp2_ALF(
					FrCompareOperator.getOperator(op),
					s, t, a);

			case FR2C_OPMODE_LR:
				return new JCmpOp2_LRF(
					FrCompareOperator.getOperator(op),
					s, t, a);
			case FR2C_OPMODE_LA:
				return new JCmpOp2_LAF(
					FrCompareOperator.getOperator(op),
					s, t, a);
			case FR2C_OPMODE_LL:
				return new JCmpOp2_LLF(
					FrCompareOperator.getOperator(op),
					s, t, a);
			}
		}
		return null;
	}


	//Indirect Int

	private static final class JCmpOp2_RRI extends FrTailOpcodeDST
	{
		public JCmpOp2_RRI(FrCompareOperator op, int s, int t, int a)
			{ super(0, s, t, a); this.op=op; }
		public FrTrace run(FrVMFrame frm)
			{ if(op.opb(frm.regi[s], frm.regi[t]))
				{ return trace.jmpnext; }
			return trace.next; }
		FrCompareOperator op;
	}

	private static final class JCmpOp2_RAI extends FrTailOpcodeDST
	{
		public JCmpOp2_RAI(FrCompareOperator op, int s, int t, int a)
			{ super(0, s, t, a); this.op=op; }
		public FrTrace run(FrVMFrame frm)
			{ if(op.opb(frm.regi[s], frm.argi[t]))
				{ return trace.jmpnext; }
			return trace.next; }
		FrCompareOperator op;
	}

	private static final class JCmpOp2_RLI extends FrTailOpcodeDST
	{
		public JCmpOp2_RLI(FrCompareOperator op, int s, int t, int a)
			{ super(0, s, t, a); this.op=op; }
		public FrTrace run(FrVMFrame frm)
			{ if(op.opb(frm.regi[s], frm.loci[t]))
				{ return trace.jmpnext; }
			return trace.next; }
		FrCompareOperator op;
	}

	private static final class JCmpOp2_ARI extends FrTailOpcodeDST
	{
		public JCmpOp2_ARI(FrCompareOperator op, int s, int t, int a)
			{ super(0, s, t, a); this.op=op; }
		public FrTrace run(FrVMFrame frm)
			{ if(op.opb(frm.argi[s], frm.regi[t]))
				{ return trace.jmpnext; }
			return trace.next; }
		FrCompareOperator op;
	}

	private static final class JCmpOp2_AAI extends FrTailOpcodeDST
	{
		public JCmpOp2_AAI(FrCompareOperator op, int s, int t, int a)
			{ super(0, s, t, a); this.op=op; }
		public FrTrace run(FrVMFrame frm)
			{ if(op.opb(frm.argi[s], frm.argi[t]))
				{ return trace.jmpnext; }
			return trace.next; }
		FrCompareOperator op;
	}

	private static final class JCmpOp2_ALI extends FrTailOpcodeDST
	{
		public JCmpOp2_ALI(FrCompareOperator op, int s, int t, int a)
			{ super(0, s, t, a); this.op=op; }
		public FrTrace run(FrVMFrame frm)
			{ if(op.opb(frm.argi[s], frm.loci[t]))
				{ return trace.jmpnext; }
			return trace.next; }
		FrCompareOperator op;
	}

	private static final class JCmpOp2_LRI extends FrTailOpcodeDST
	{
		public JCmpOp2_LRI(FrCompareOperator op, int s, int t, int a)
			{ super(0, s, t, a); this.op=op; }
		public FrTrace run(FrVMFrame frm)
			{ if(op.opb(frm.loci[s], frm.regi[t]))
				{ return trace.jmpnext; }
			return trace.next; }
		FrCompareOperator op;
	}

	private static final class JCmpOp2_LAI extends FrTailOpcodeDST
	{
		public JCmpOp2_LAI(FrCompareOperator op, int s, int t, int a)
			{ super(0, s, t, a); this.op=op; }
		public FrTrace run(FrVMFrame frm)
			{ if(op.opb(frm.loci[s], frm.argi[t]))
				{ return trace.jmpnext; }
			return trace.next; }
		FrCompareOperator op;
	}

	private static final class JCmpOp2_LLI extends FrTailOpcodeDST
	{
		public JCmpOp2_LLI(FrCompareOperator op, int s, int t, int a)
			{ super(0, s, t, a); this.op=op; }
		public FrTrace run(FrVMFrame frm)
			{ if(op.opb(frm.loci[s], frm.loci[t]))
				{ return trace.jmpnext; }
			return trace.next; }
		FrCompareOperator op;
	}


	//Indirect Float

	private static final class JCmpOp2_RRF extends FrTailOpcodeDST
	{
		public JCmpOp2_RRF(FrCompareOperator op, int s, int t, int a)
			{ super(0, s, t, a); this.op=op; }
		public FrTrace run(FrVMFrame frm)
			{ if(op.opb(frm.regf[s], frm.regf[t]))
				{ return trace.jmpnext; }
			return trace.next; }
		FrCompareOperator op;
	}

	private static final class JCmpOp2_RAF extends FrTailOpcodeDST
	{
		public JCmpOp2_RAF(FrCompareOperator op, int s, int t, int a)
			{ super(0, s, t, a); this.op=op; }
		public FrTrace run(FrVMFrame frm)
			{ if(op.opb(frm.regf[s], frm.argf[t]))
				{ return trace.jmpnext; }
			return trace.next; }
		FrCompareOperator op;
	}

	private static final class JCmpOp2_RLF extends FrTailOpcodeDST
	{
		public JCmpOp2_RLF(FrCompareOperator op, int s, int t, int a)
			{ super(0, s, t, a); this.op=op; }
		public FrTrace run(FrVMFrame frm)
			{ if(op.opb(frm.regf[s], frm.locf[t]))
				{ return trace.jmpnext; }
			return trace.next; }
		FrCompareOperator op;
	}

	private static final class JCmpOp2_ARF extends FrTailOpcodeDST
	{
		public JCmpOp2_ARF(FrCompareOperator op, int s, int t, int a)
			{ super(0, s, t, a); this.op=op; }
		public FrTrace run(FrVMFrame frm)
			{ if(op.opb(frm.argf[s], frm.regf[t]))
				{ return trace.jmpnext; }
			return trace.next; }
		FrCompareOperator op;
	}

	private static final class JCmpOp2_AAF extends FrTailOpcodeDST
	{
		public JCmpOp2_AAF(FrCompareOperator op, int s, int t, int a)
			{ super(0, s, t, a); this.op=op; }
		public FrTrace run(FrVMFrame frm)
			{ if(op.opb(frm.argf[s], frm.argf[t]))
				{ return trace.jmpnext; }
			return trace.next; }
		FrCompareOperator op;
	}

	private static final class JCmpOp2_ALF extends FrTailOpcodeDST
	{
		public JCmpOp2_ALF(FrCompareOperator op, int s, int t, int a)
			{ super(0, s, t, a); this.op=op; }
		public FrTrace run(FrVMFrame frm)
			{ if(op.opb(frm.argf[s], frm.locf[t]))
				{ return trace.jmpnext; }
			return trace.next; }
		FrCompareOperator op;
	}

	private static final class JCmpOp2_LRF extends FrTailOpcodeDST
	{
		public JCmpOp2_LRF(FrCompareOperator op, int s, int t, int a)
			{ super(0, s, t, a); this.op=op; }
		public FrTrace run(FrVMFrame frm)
			{ if(op.opb(frm.locf[s], frm.regf[t]))
				{ return trace.jmpnext; }
			return trace.next; }
		FrCompareOperator op;
	}

	private static final class JCmpOp2_LAF extends FrTailOpcodeDST
	{
		public JCmpOp2_LAF(FrCompareOperator op, int s, int t, int a)
			{ super(0, s, t, a); this.op=op; }
		public FrTrace run(FrVMFrame frm)
			{ if(op.opb(frm.locf[s], frm.argf[t]))
				{ return trace.jmpnext; }
			return trace.next; }
		FrCompareOperator op;
	}

	private static final class JCmpOp2_LLF extends FrTailOpcodeDST
	{
		public JCmpOp2_LLF(FrCompareOperator op, int s, int t, int a)
			{ super(0, s, t, a); this.op=op; }
		public FrTrace run(FrVMFrame frm)
			{ if(op.opb(frm.locf[s], frm.locf[t]))
				{ return trace.jmpnext; }
			return trace.next; }
		FrCompareOperator op;
	}
}
