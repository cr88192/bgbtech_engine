package fricore;

public abstract class FrBinaryOperator
{
	public abstract int op(int x, int y);
	public abstract long op(long x, long y);
	public abstract float op(float x, float y);
	public abstract double op(double x, double y);
	public static FrBinaryOperator[] ops;
	
	public static FrBinaryOperator getOperator(int op)
	{
		if(ops==null)
			{ init(); }
		return(ops[op]);
	}

	public static void init()
	{
		ops=new FrBinaryOperator[16];
		ops[FrOpcode.FR2C_BINOP_ADD]=new BinAdd();
		ops[FrOpcode.FR2C_BINOP_SUB]=new BinSub();
		ops[FrOpcode.FR2C_BINOP_MUL]=new BinMul();
		ops[FrOpcode.FR2C_BINOP_DIV]=new BinDiv();
		ops[FrOpcode.FR2C_BINOP_MOD]=new BinMod();
		ops[FrOpcode.FR2C_BINOP_AND]=new BinAnd();
		ops[FrOpcode.FR2C_BINOP_OR] =new BinOr();
		ops[FrOpcode.FR2C_BINOP_XOR]=new BinXor();
		ops[FrOpcode.FR2C_BINOP_SHL]=new BinShl();
		ops[FrOpcode.FR2C_BINOP_SHR]=new BinShr();
		ops[FrOpcode.FR2C_BINOP_SHRR]=new BinShrr();
//		ops[FrOpcode.FR2C_BINOP_DIV]=new BinDiv();
	}
	
	public static final class BinAdd extends FrBinaryOperator
	{
		public int op(int x, int y) { return x+y; }
		public long op(long x, long y) { return x+y; }
		public float op(float x, float y) { return x+y; }
		public double op(double x, double y) { return x+y; }
	}

	public static final class BinSub extends FrBinaryOperator
	{
		public int op(int x, int y) { return x-y; }
		public long op(long x, long y) { return x-y; }
		public float op(float x, float y) { return x-y; }
		public double op(double x, double y) { return x-y; }
	}

	public static final class BinMul extends FrBinaryOperator
	{
		public int op(int x, int y) { return x*y; }
		public long op(long x, long y) { return x*y; }
		public float op(float x, float y) { return x*y; }
		public double op(double x, double y) { return x*y; }
	}

	public static final class BinDiv extends FrBinaryOperator
	{
		public int op(int x, int y) { return x/y; }
		public long op(long x, long y) { return x/y; }
		public float op(float x, float y) { return x/y; }
		public double op(double x, double y) { return x/y; }
	}

	public static final class BinMod extends FrBinaryOperator
	{
		public int op(int x, int y) { return x%y; }
		public long op(long x, long y) { return x%y; }
		public float op(float x, float y) { return x; }
		public double op(double x, double y) { return x; }
	}

	public static final class BinAnd extends FrBinaryOperator
	{
		public int op(int x, int y) { return x&y; }
		public long op(long x, long y) { return x&y; }
		public float op(float x, float y) { return x; }
		public double op(double x, double y) { return x; }
	}

	public static final class BinOr extends FrBinaryOperator
	{
		public int op(int x, int y) { return x|y; }
		public long op(long x, long y) { return x|y; }
		public float op(float x, float y) { return x; }
		public double op(double x, double y) { return x; }
	}

	public static final class BinXor extends FrBinaryOperator
	{
		public int op(int x, int y) { return x^y; }
		public long op(long x, long y) { return x^y; }
		public float op(float x, float y) { return x; }
		public double op(double x, double y) { return x; }
	}

	public static final class BinShl extends FrBinaryOperator
	{
		public int op(int x, int y) { return x<<y; }
		public long op(long x, long y) { return x<<((int)y); }
		public float op(float x, float y) { return x; }
		public double op(double x, double y) { return x; }
	}

	public static final class BinShr extends FrBinaryOperator
	{
		public int op(int x, int y) { return x>>y; }
		public long op(long x, long y) { return x>>((int)y); }
		public float op(float x, float y) { return x; }
		public double op(double x, double y) { return x; }
	}

	public static final class BinShrr extends FrBinaryOperator
	{
		public int op(int x, int y) { return x>>>y; }
		public long op(long x, long y) { return x>>>((int)y); }
		public float op(float x, float y) { return x; }
		public double op(double x, double y) { return x; }
	}
}
