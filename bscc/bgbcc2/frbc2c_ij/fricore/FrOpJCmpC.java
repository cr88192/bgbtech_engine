package fricore;
import static fricore.FrOpcode.*;

public abstract class FrOpJCmpC extends FrTailOpcode
{
	public static FrTailOpcode makeCmpOp(
		int z, int y, int op, int s, long t, int a)
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
			}
		}
		return null;
	}

	public static FrTailOpcode makeCmpOp(
		int z, int y, int op, int s, double t, int a)
	{
		switch(z)
		{
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
			}
		}
		return null;
	}


	//Indirect Int

	private static final class JCmpOp2_RRI extends FrTailOpcodeDSII
	{
		public JCmpOp2_RRI(FrCompareOperator op, int s, long t, int a)
			{ super(0, s, t, a); this.op=op; }
		public FrTrace run(FrVMFrame frm)
			{ if(op.opb(frm.regi[s], imm))
				{ return trace.jmpnext; }
			return trace.next; }
		FrCompareOperator op;
	}

	private static final class JCmpOp2_RAI extends FrTailOpcodeDSII
	{
		public JCmpOp2_RAI(FrCompareOperator op, int s, long t, int a)
			{ super(0, s, t, a); this.op=op; }
		public FrTrace run(FrVMFrame frm)
			{ if(op.opb(frm.regi[s], imm))
				{ return trace.jmpnext; }
			return trace.next; }
		FrCompareOperator op;
	}

	private static final class JCmpOp2_RLI extends FrTailOpcodeDSII
	{
		public JCmpOp2_RLI(FrCompareOperator op, int s, long t, int a)
			{ super(0, s, t, a); this.op=op; }
		public FrTrace run(FrVMFrame frm)
			{ if(op.opb(frm.regi[s], imm))
				{ return trace.jmpnext; }
			return trace.next; }
		FrCompareOperator op;
	}


	//Indirect Float

	private static final class JCmpOp2_RRF extends FrTailOpcodeDSIF
	{
		public JCmpOp2_RRF(FrCompareOperator op, int s, double t, int a)
			{ super(0, s, t, a); this.op=op; }
		public FrTrace run(FrVMFrame frm)
			{ if(op.opb(frm.regf[s], imm))
				{ return trace.jmpnext; }
			return trace.next; }
		FrCompareOperator op;
	}

	private static final class JCmpOp2_RAF extends FrTailOpcodeDSIF
	{
		public JCmpOp2_RAF(FrCompareOperator op, int s, double t, int a)
			{ super(0, s, t, a); this.op=op; }
		public FrTrace run(FrVMFrame frm)
			{ if(op.opb(frm.regf[s], imm))
				{ return trace.jmpnext; }
			return trace.next; }
		FrCompareOperator op;
	}

	private static final class JCmpOp2_RLF extends FrTailOpcodeDSIF
	{
		public JCmpOp2_RLF(FrCompareOperator op, int s, double t, int a)
			{ super(0, s, t, a); this.op=op; }
		public FrTrace run(FrVMFrame frm)
			{ if(op.opb(frm.regf[s], imm))
				{ return trace.jmpnext; }
			return trace.next; }
		FrCompareOperator op;
	}
}
