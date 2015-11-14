package fricore;

public abstract class FrCompareOperator
{
	public abstract int op(int x, int y);
	public abstract int op(long x, long y);
	public abstract int op(float x, float y);
	public abstract int op(double x, double y);

	public abstract boolean opb(int x, int y);
	public abstract boolean opb(long x, long y);
	public abstract boolean opb(float x, float y);
	public abstract boolean opb(double x, double y);

	public static FrCompareOperator[] ops;
	
	public static FrCompareOperator getOperator(int op)
	{
		if(ops==null)
			{ init(); }
		return(ops[op]);
	}

	public static void init()
	{
		ops=new FrCompareOperator[8];
		ops[FrOpcode.FR2C_CMP_EQ]=new CmpEq();
		ops[FrOpcode.FR2C_CMP_NE]=new CmpNe();
		ops[FrOpcode.FR2C_CMP_LT]=new CmpLt();
		ops[FrOpcode.FR2C_CMP_GT]=new CmpGt();
		ops[FrOpcode.FR2C_CMP_LE]=new CmpLe();
		ops[FrOpcode.FR2C_CMP_GE]=new CmpGe();
		ops[FrOpcode.FR2C_CMP_AL]=new CmpAl();
		ops[FrOpcode.FR2C_CMP_NV]=new CmpNv();
	}
	
	public static final class CmpEq extends FrCompareOperator
	{
		public int op(int x, int y) { return (x==y)?1:0; }
		public int op(long x, long y) { return (x==y)?1:0; }
		public int op(float x, float y) { return (x==y)?1:0; }
		public int op(double x, double y) { return (x==y)?1:0; }

		public boolean opb(int x, int y) { return(x==y); }
		public boolean opb(long x, long y) { return(x==y); }
		public boolean opb(float x, float y) { return(x==y); }
		public boolean opb(double x, double y) { return(x==y); }
	}

	public static final class CmpNe extends FrCompareOperator
	{
		public int op(int x, int y) { return (x!=y)?1:0; }
		public int op(long x, long y) { return (x!=y)?1:0; }
		public int op(float x, float y) { return (x!=y)?1:0; }
		public int op(double x, double y) { return (x!=y)?1:0; }

		public boolean opb(int x, int y) { return(x!=y); }
		public boolean opb(long x, long y) { return(x!=y); }
		public boolean opb(float x, float y) { return(x!=y); }
		public boolean opb(double x, double y) { return(x!=y); }
	}

	public static final class CmpLt extends FrCompareOperator
	{
		public int op(int x, int y) { return (x<y)?1:0; }
		public int op(long x, long y) { return (x<y)?1:0; }
		public int op(float x, float y) { return (x<y)?1:0; }
		public int op(double x, double y) { return (x<y)?1:0; }

		public boolean opb(int x, int y) { return(x<y); }
		public boolean opb(long x, long y) { return(x<y); }
		public boolean opb(float x, float y) { return(x<y); }
		public boolean opb(double x, double y) { return(x<y); }
	}

	public static final class CmpGt extends FrCompareOperator
	{
		public int op(int x, int y) { return (x>y)?1:0; }
		public int op(long x, long y) { return (x>y)?1:0; }
		public int op(float x, float y) { return (x>y)?1:0; }
		public int op(double x, double y) { return (x>y)?1:0; }

		public boolean opb(int x, int y) { return(x>y); }
		public boolean opb(long x, long y) { return(x>y); }
		public boolean opb(float x, float y) { return(x>y); }
		public boolean opb(double x, double y) { return(x>y); }
	}

	public static final class CmpLe extends FrCompareOperator
	{
		public int op(int x, int y) { return (x<=y)?1:0; }
		public int op(long x, long y) { return (x<=y)?1:0; }
		public int op(float x, float y) { return (x<=y)?1:0; }
		public int op(double x, double y) { return (x<=y)?1:0; }

		public boolean opb(int x, int y) { return(x<=y); }
		public boolean opb(long x, long y) { return(x<=y); }
		public boolean opb(float x, float y) { return(x<=y); }
		public boolean opb(double x, double y) { return(x<=y); }
	}

	public static final class CmpGe extends FrCompareOperator
	{
		public int op(int x, int y) { return (x>=y)?1:0; }
		public int op(long x, long y) { return (x>=y)?1:0; }
		public int op(float x, float y) { return (x>=y)?1:0; }
		public int op(double x, double y) { return (x>=y)?1:0; }

		public boolean opb(int x, int y) { return(x>=y); }
		public boolean opb(long x, long y) { return(x>=y); }
		public boolean opb(float x, float y) { return(x>=y); }
		public boolean opb(double x, double y) { return(x>=y); }
	}

	public static final class CmpAl extends FrCompareOperator
	{
		public int op(int x, int y) { return 1; }
		public int op(long x, long y) { return 1; }
		public int op(float x, float y) { return 1; }
		public int op(double x, double y) { return 1; }

		public boolean opb(int x, int y) { return true; }
		public boolean opb(long x, long y) { return true; }
		public boolean opb(float x, float y) { return true; }
		public boolean opb(double x, double y) { return true; }
	}

	public static final class CmpNv extends FrCompareOperator
	{
		public int op(int x, int y) { return 0; }
		public int op(long x, long y) { return 0; }
		public int op(float x, float y) { return 0; }
		public int op(double x, double y) { return 0; }

		public boolean opb(int x, int y) { return false; }
		public boolean opb(long x, long y) { return false; }
		public boolean opb(float x, float y) { return false; }
		public boolean opb(double x, double y) { return false; }
	}
}
