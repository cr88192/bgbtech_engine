package fricore;

public abstract class FrTailOpcodeDSII extends FrTailOpcode
{
	public FrTailOpcodeDSII(int d, int s, long imm)
		{ this.d=d; this.s=s; this.imm=imm; }
	public FrTailOpcodeDSII(int d, int s, long imm, int a)
		{ this.d=d; this.s=s; this.imm=imm; this.jmpaddr=a; }
	public final int d, s;
	public final long imm;
}
