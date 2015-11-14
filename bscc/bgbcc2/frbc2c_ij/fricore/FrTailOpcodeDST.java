package fricore;

public abstract class FrTailOpcodeDST extends FrTailOpcode
{
//	public abstract void run(State ctx);
	public FrTailOpcodeDST(int d, int s, int t)
		{ this.d=d; this.s=s; this.t=t; }
	public FrTailOpcodeDST(int d, int s, int t, int a)
		{ this.d=d; this.s=s; this.t=t; this.jmpaddr=a; }
	public final int d, s, t;
}
