package fricore;

public abstract class FrCompareZeroOpr
{
	public abstract int op(int x);
	public abstract int op(long x);
	public abstract int op(float x);
	public abstract int op(double x);
	public static FrCompareZeroOpr[] ops;
	
	public static FrCompareZeroOpr getOperator(int op)
	{
		if(ops==null)
			{ init(); }
		return(ops[op]);
	}

	public static void init()
	{
		ops=new FrCompareZeroOpr[8];
		ops[FrOpcode.FR2C_CMP_EQ]=new CmpEq();
		ops[FrOpcode.FR2C_CMP_NE]=new CmpNe();
		ops[FrOpcode.FR2C_CMP_LT]=new CmpLt();
		ops[FrOpcode.FR2C_CMP_GT]=new CmpGt();
		ops[FrOpcode.FR2C_CMP_LE]=new CmpLe();
		ops[FrOpcode.FR2C_CMP_GE]=new CmpGe();
		ops[FrOpcode.FR2C_CMP_AL]=new CmpAl();
		ops[FrOpcode.FR2C_CMP_NV]=new CmpNv();
	}
	
	public static final class CmpEq extends FrCompareZeroOpr
	{
		public int op(int x) { return (x==0)?1:0; }
		public int op(long x) { return (x==0)?1:0; }
		public int op(float x) { return (x==0)?1:0; }
		public int op(double x) { return (x==0)?1:0; }
	}

	public static final class CmpNe extends FrCompareZeroOpr
	{
		public int op(int x) { return (x!=0)?1:0; }
		public int op(long x) { return (x!=0)?1:0; }
		public int op(float x) { return (x!=0)?1:0; }
		public int op(double x) { return (x!=0)?1:0; }
	}

	public static final class CmpLt extends FrCompareZeroOpr
	{
		public int op(int x) { return (x<0)?1:0; }
		public int op(long x) { return (x<0)?1:0; }
		public int op(float x) { return (x<0)?1:0; }
		public int op(double x) { return (x<0)?1:0; }
	}

	public static final class CmpGt extends FrCompareZeroOpr
	{
		public int op(int x) { return (x>0)?1:0; }
		public int op(long x) { return (x>0)?1:0; }
		public int op(float x) { return (x>0)?1:0; }
		public int op(double x) { return (x>0)?1:0; }
	}

	public static final class CmpLe extends FrCompareZeroOpr
	{
		public int op(int x) { return (x<=0)?1:0; }
		public int op(long x) { return (x<=0)?1:0; }
		public int op(float x) { return (x<=0)?1:0; }
		public int op(double x) { return (x<=0)?1:0; }
	}

	public static final class CmpGe extends FrCompareZeroOpr
	{
		public int op(int x) { return (x>=0)?1:0; }
		public int op(long x) { return (x>=0)?1:0; }
		public int op(float x) { return (x>=0)?1:0; }
		public int op(double x) { return (x>=0)?1:0; }
	}

	public static final class CmpAl extends FrCompareZeroOpr
	{
		public int op(int x) { return 1; }
		public int op(long x) { return 1; }
		public int op(float x) { return 1; }
		public int op(double x) { return 1; }
	}

	public static final class CmpNv extends FrCompareZeroOpr
	{
		public int op(int x) { return 0; }
		public int op(long x) { return 0; }
		public int op(float x) { return 0; }
		public int op(double x) { return 0; }
	}
}
