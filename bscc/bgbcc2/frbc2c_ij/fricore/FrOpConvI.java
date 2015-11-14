package fricore;

public abstract class FrOpConvI extends FrOpcode
{
	public static FrOpcode makeOpConvI(
		int z, int y, int d, int s)
	{
		switch(y)
		{
		case FR2C_OPMODE_RR:
			return new ConvI2_RRI(makeConvOpr(z), d, s);
		case FR2C_OPMODE_RA:
			return new ConvI2_RAI(makeConvOpr(z), d, s);
		case FR2C_OPMODE_RL:
			return new ConvI2_RLI(makeConvOpr(z), d, s);
		case FR2C_OPMODE_AR:
			return new ConvI2_ARI(makeConvOpr(z), d, s);
		case FR2C_OPMODE_AA:
			return new ConvI2_AAI(makeConvOpr(z), d, s);
		case FR2C_OPMODE_AL:
			return new ConvI2_ALI(makeConvOpr(z), d, s);
		case FR2C_OPMODE_LR:
			return new ConvI2_LRI(makeConvOpr(z), d, s);
		case FR2C_OPMODE_LA:
			return new ConvI2_LAI(makeConvOpr(z), d, s);
		case FR2C_OPMODE_LL:
			return new ConvI2_LLI(makeConvOpr(z), d, s);
		}
		return null;
	}

	private static ConvI_Opr makeConvOpr(int z)
	{
		switch(z)
		{
		case FR2C_CONVI_SXB:
			return new ConvI2_SXB();
		case FR2C_CONVI_ZXB:
			return new ConvI2_ZXB();
		case FR2C_CONVI_SXS:
			return new ConvI2_SXS();
		case FR2C_CONVI_ZXS:
			return new ConvI2_ZXS();
		case FR2C_CONVI_SXI:
			return new ConvI2_SXI();
		case FR2C_CONVI_ZXI:
			return new ConvI2_ZXI();
		case FR2C_CONVI_SXB2:
			return new ConvI2_SXB2();
		case FR2C_CONVI_ZXB2:
			return new ConvI2_ZXB2();
		case FR2C_CONVI_SXS2:
			return new ConvI2_SXS2();
		case FR2C_CONVI_ZXS2:
			return new ConvI2_ZXS2();
		case FR2C_CONVI_SXB3:
			return new ConvI2_SXB3();
		case FR2C_CONVI_ZXB3:
			return new ConvI2_ZXB3();
		case FR2C_CONVI_SXB4:
			return new ConvI2_SXB4();
		case FR2C_CONVI_ZXB4:
			return new ConvI2_ZXB4();
		case FR2C_CONVI_SXI2:
			return new ConvI2_SXI2();
		case FR2C_CONVI_ZXI2:
			return new ConvI2_ZXI2();
		}
		return null;
	}

	private static abstract class ConvI_Opr
	{
		public abstract long op(long x);
	}

	private static final class ConvI2_SXB extends ConvI_Opr
		{ public long op(long x) { return((x<<56)>>56); } }
	private static final class ConvI2_ZXB extends ConvI_Opr
		{ public long op(long x) { return(x&255); } }
	private static final class ConvI2_SXS extends ConvI_Opr
		{ public long op(long x) { return((x<<48)>>48); } }
	private static final class ConvI2_ZXS extends ConvI_Opr
		{ public long op(long x) { return(x&65535); } }
	private static final class ConvI2_SXI extends ConvI_Opr
		{ public long op(long x) { return((x<<32)>>32); } }
	private static final class ConvI2_ZXI extends ConvI_Opr
		{ public long op(long x) { return(x&4294967295L); } }
	private static final class ConvI2_SXB2 extends ConvI_Opr
		{ public long op(long x) { return((x<<48)>>56); } }
	private static final class ConvI2_ZXB2 extends ConvI_Opr
		{ public long op(long x) { return((x>>8)&255); } }
	private static final class ConvI2_SXS2 extends ConvI_Opr
		{ public long op(long x) { return((x<<32)>>48); } }
	private static final class ConvI2_ZXS2 extends ConvI_Opr
		{ public long op(long x) { return((x>>16)&65535); } }
	private static final class ConvI2_SXB3 extends ConvI_Opr
		{ public long op(long x) { return((x<<40)>>56); } }
	private static final class ConvI2_ZXB3 extends ConvI_Opr
		{ public long op(long x) { return((x>>16)&255); } }
	private static final class ConvI2_SXB4 extends ConvI_Opr
		{ public long op(long x) { return((x<<32)>>56); } }
	private static final class ConvI2_ZXB4 extends ConvI_Opr
		{ public long op(long x) { return((x>>24)&255); } }
	private static final class ConvI2_SXI2 extends ConvI_Opr
		{ public long op(long x) { return(x>>32); } }
	private static final class ConvI2_ZXI2 extends ConvI_Opr
		{ public long op(long x) { return(x>>>32); } }


	private static final class ConvI2_RRI extends FrOpcodeDST
	{
		public ConvI2_RRI(ConvI_Opr op, int d, int s)
			{ super(d, s, 0); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=op.op(frm.regi[s]); }
		ConvI_Opr op;
	}

	private static final class ConvI2_RAI extends FrOpcodeDST
	{
		public ConvI2_RAI(ConvI_Opr op, int d, int s)
			{ super(d, s, 0); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=op.op(frm.argi[s]); }
		ConvI_Opr op;
	}

	private static final class ConvI2_RLI extends FrOpcodeDST
	{
		public ConvI2_RLI(ConvI_Opr op, int d, int s)
			{ super(d, s, 0); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=op.op(frm.loci[s]); }
		ConvI_Opr op;
	}

	private static final class ConvI2_ARI extends FrOpcodeDST
	{
		public ConvI2_ARI(ConvI_Opr op, int d, int s)
			{ super(d, s, 0); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.argi[d]=op.op(frm.regi[s]); }
		ConvI_Opr op;
	}

	private static final class ConvI2_AAI extends FrOpcodeDST
	{
		public ConvI2_AAI(ConvI_Opr op, int d, int s)
			{ super(d, s, 0); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.argi[d]=op.op(frm.argi[s]); }
		ConvI_Opr op;
	}

	private static final class ConvI2_ALI extends FrOpcodeDST
	{
		public ConvI2_ALI(ConvI_Opr op, int d, int s)
			{ super(d, s, 0); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.argi[d]=op.op(frm.loci[s]); }
		ConvI_Opr op;
	}

	private static final class ConvI2_LRI extends FrOpcodeDST
	{
		public ConvI2_LRI(ConvI_Opr op, int d, int s)
			{ super(d, s, 0); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.loci[d]=op.op(frm.regi[s]); }
		ConvI_Opr op;
	}

	private static final class ConvI2_LAI extends FrOpcodeDST
	{
		public ConvI2_LAI(ConvI_Opr op, int d, int s)
			{ super(d, s, 0); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.loci[d]=op.op(frm.argi[s]); }
		ConvI_Opr op;
	}

	private static final class ConvI2_LLI extends FrOpcodeDST
	{
		public ConvI2_LLI(ConvI_Opr op, int d, int s)
			{ super(d, s, 0); this.op=op; }
		public void run(FrVMFrame frm)
			{ frm.loci[d]=op.op(frm.loci[s]); }
		ConvI_Opr op;
	}
}
