package fricore;

public abstract class FrOpcodeDSIF extends FrOpcode
{
//	public abstract void run(State ctx);
	public FrOpcodeDSIF(int d, int s, double imm)
		{ this.d=d; this.s=s; this.imm=imm; }
	public final int d, s;
	public final double imm;
}
