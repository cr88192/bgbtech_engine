package fricore;

public abstract class FrOpcodeDST extends FrOpcode
{
//	public abstract void run(State ctx);
	public FrOpcodeDST(int d, int s, int t)
		{ this.d=d; this.s=s; this.t=t; }
	public final int d, s, t;
}
