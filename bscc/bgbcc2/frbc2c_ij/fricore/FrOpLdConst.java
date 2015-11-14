package fricore;

public abstract class FrOpLdConst extends FrOpcode
{
	public static FrOpcode makeOpLdConst(
		FrVMImage img, int w, int d, int t)
	{
		switch(w)
		{
		case FR2C_OPMODE2_IR:
			return new LdcOp2_IR(d, img.getLiteralInt(t));
		case FR2C_OPMODE2_IA:
			return new LdcOp2_IA(d, img.getLiteralInt(t));
		case FR2C_OPMODE2_IL:
			return new LdcOp2_IL(d, img.getLiteralInt(t));
		case FR2C_OPMODE2_LR:
			return new LdcOp2_IR(d, img.getLiteralLong(t));
		case FR2C_OPMODE2_LA:
			return new LdcOp2_IA(d, img.getLiteralLong(t));
		case FR2C_OPMODE2_LL:
			return new LdcOp2_IL(d, img.getLiteralLong(t));
		case FR2C_OPMODE2_FR:
			return new LdcOp2_FR(d, img.getLiteralFloat(t));
		case FR2C_OPMODE2_FA:
			return new LdcOp2_FA(d, img.getLiteralFloat(t));
		case FR2C_OPMODE2_FL:
			return new LdcOp2_FL(d, img.getLiteralFloat(t));
		case FR2C_OPMODE2_DR:
			return new LdcOp2_FR(d, img.getLiteralDouble(t));
		case FR2C_OPMODE2_DA:
			return new LdcOp2_FA(d, img.getLiteralDouble(t));
		case FR2C_OPMODE2_DL:
			return new LdcOp2_FL(d, img.getLiteralDouble(t));

		default:
			break;
		}
		return null;
	}

	private static final class LdcOp2_IR extends FrOpcode
	{
		public LdcOp2_IR(int d, long t)
			{ this.d=d; this.t=t; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=t; }
		int d; long t;
	}
	private static final class LdcOp2_IA extends FrOpcode
	{
		public LdcOp2_IA(int d, long t)
			{ this.d=d; this.t=t; }
		public void run(FrVMFrame frm)
			{ frm.argi[d]=t; }
		int d; long t;
	}
	private static final class LdcOp2_IL extends FrOpcode
	{
		public LdcOp2_IL(int d, long t)
			{ this.d=d; this.t=t; }
		public void run(FrVMFrame frm)
			{ frm.loci[d]=t; }
		int d; long t;
	}

	private static final class LdcOp2_FR extends FrOpcode
	{
		public LdcOp2_FR(int d, double t)
			{ this.d=d; this.t=t; }
		public void run(FrVMFrame frm)
			{ frm.regf[d]=t; }
		int d; double t;
	}
	private static final class LdcOp2_FA extends FrOpcode
	{
		public LdcOp2_FA(int d, double t)
			{ this.d=d; this.t=t; }
		public void run(FrVMFrame frm)
			{ frm.argf[d]=t; }
		int d; double t;
	}
	private static final class LdcOp2_FL extends FrOpcode
	{
		public LdcOp2_FL(int d, double t)
			{ this.d=d; this.t=t; }
		public void run(FrVMFrame frm)
			{ frm.locf[d]=t; }
		int d; double t;
	}
}
