package fricore;

public abstract class FrOpInitArr extends FrOpcode
{
	public static FrOpcode makeOpInitArr(
		int z, int d, int c)
	{
		switch(z)
		{
		case FR2C_TY_I:
		case FR2C_TY_F:
			return new InitArrI(d, c*4);
		case FR2C_TY_L:
		case FR2C_TY_D:
		case FR2C_TY_P:
			return new InitArrI(d, c*8);

		case FR2C_TY_SB:
		case FR2C_TY_UB:
			return new InitArrI(d, c);
		case FR2C_TY_SS:
		case FR2C_TY_US:
			return new InitArrI(d, c*2);

		default:
			return new InitArrI(d, c*8);
		}
	}

	private static final class InitArrI extends FrOpcode
	{
		public InitArrI(int d, int c)
			{ this.d=d; this.c=c; }
		public void run(FrVMFrame frm)
			{ frm.locp[d]=frm.allocLocalData(c); }
		private final int d, c;
	}
}