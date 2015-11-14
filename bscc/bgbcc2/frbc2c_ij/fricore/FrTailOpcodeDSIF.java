package fricore;

public abstract class FrTailOpcodeDSIF extends FrTailOpcode
{
	public FrTailOpcodeDSIF(int d, int s, double imm)
		{ this.d=d; this.s=s; this.imm=imm; }
	public FrTailOpcodeDSIF(int d, int s, double imm, int a)
		{ this.d=d; this.s=s; this.imm=imm; this.jmpaddr=a; }
	public final int d, s;
	public final double imm;
}
