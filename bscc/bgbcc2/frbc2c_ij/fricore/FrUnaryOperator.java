package fricore;

public abstract class FrUnaryOperator
{
	public abstract int op(int x);
	public abstract long op(long x);
	public abstract float op(float x);
	public abstract double op(double x);
	public static FrUnaryOperator[] ops;
	
	public static FrUnaryOperator getOperator(int op)
	{
		if(ops==null)
			{ init(); }
		return(ops[op]);
	}

	public static void init()
	{
		ops=new FrUnaryOperator[16];
		ops[FrOpcode.FR2C_UNOP_MOV]=new UnMov();
		ops[FrOpcode.FR2C_UNOP_NEG]=new UnNeg();
		ops[FrOpcode.FR2C_UNOP_NOT]=new UnNot();
		ops[FrOpcode.FR2C_UNOP_LNOT]=new UnLNot();
		ops[FrOpcode.FR2C_UNOP_INC]=new UnInc();
		ops[FrOpcode.FR2C_UNOP_DEC]=new UnDec();
	}
	
	public static final class UnMov extends FrUnaryOperator
	{
		public int op(int x) { return x; }
		public long op(long x) { return x; }
		public float op(float x) { return x; }
		public double op(double x) { return x; }
	}

	public static final class UnNeg extends FrUnaryOperator
	{
		public int op(int x) { return -x; }
		public long op(long x) { return -x; }
		public float op(float x) { return -x; }
		public double op(double x) { return -x; }
	}

	public static final class UnNot extends FrUnaryOperator
	{
		public int op(int x) { return ~x; }
		public long op(long x) { return ~x; }
		public float op(float x) { return (float)(~((long)x)); }
		public double op(double x) { return (double)(~((long)x)); }
	}

	public static final class UnLNot extends FrUnaryOperator
	{
		public int op(int x) { return (x==0)?1:0; }
		public long op(long x) { return (x==0L)?1:0; }
		public float op(float x) { return (x==0.0F)?1:0; }
		public double op(double x) { return (x==0.0)?1:0; }
	}

	public static final class UnInc extends FrUnaryOperator
	{
		public int op(int x) { return x+1; }
		public long op(long x) { return x+1; }
		public float op(float x) { return x+1; }
		public double op(double x) { return x+1; }
	}

	public static final class UnDec extends FrUnaryOperator
	{
		public int op(int x) { return x-1; }
		public long op(long x) { return x-1; }
		public float op(float x) { return x-1; }
		public double op(double x) { return x-1; }
	}

}
