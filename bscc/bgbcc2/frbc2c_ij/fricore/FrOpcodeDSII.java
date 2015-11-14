package fricore;

public abstract class FrOpcodeDSII extends FrOpcode
{
//	public abstract void run(State ctx);
	public FrOpcodeDSII(int d, int s, long imm)
		{ this.d=d; this.s=s; this.imm=imm; }
	public final int d, s;
	public final long imm;
}
