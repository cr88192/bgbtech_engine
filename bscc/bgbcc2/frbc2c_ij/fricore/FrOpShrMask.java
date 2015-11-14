package fricore;

public abstract class FrOpShrMask extends FrOpcode
{
	public static FrOpcode makeOpShrMask(
		int z, int y, int d, int s, int sh, int mm)
	{
		switch(y)
		{
		case FR2C_OPMODE_RR:
			return new ShrMask2_RRI(d, s, sh, mm);
		case FR2C_OPMODE_RA:
			return new ShrMask2_RAI(d, s, sh, mm);
		case FR2C_OPMODE_RL:
			return new ShrMask2_RLI(d, s, sh, mm);
		case FR2C_OPMODE_AR:
			return new ShrMask2_ARI(d, s, sh, mm);
		case FR2C_OPMODE_AA:
			return new ShrMask2_AAI(d, s, sh, mm);
		case FR2C_OPMODE_AL:
			return new ShrMask2_ALI(d, s, sh, mm);
		case FR2C_OPMODE_LR:
			return new ShrMask2_LRI(d, s, sh, mm);
		case FR2C_OPMODE_LA:
			return new ShrMask2_LAI(d, s, sh, mm);
		case FR2C_OPMODE_LL:
			return new ShrMask2_LLI(d, s, sh, mm);
		}
		return null;
	}

	public static FrOpcode makeOpShlMask(
		int z, int y, int d, int s, int sh, int mm)
	{
		switch(y)
		{
		case FR2C_OPMODE_RR:
			return new ShlMask2_RRI(d, s, sh, mm);
		case FR2C_OPMODE_RA:
			return new ShlMask2_RAI(d, s, sh, mm);
		case FR2C_OPMODE_RL:
			return new ShlMask2_RLI(d, s, sh, mm);
		case FR2C_OPMODE_AR:
			return new ShlMask2_ARI(d, s, sh, mm);
		case FR2C_OPMODE_AA:
			return new ShlMask2_AAI(d, s, sh, mm);
		case FR2C_OPMODE_AL:
			return new ShlMask2_ALI(d, s, sh, mm);
		case FR2C_OPMODE_LR:
			return new ShlMask2_LRI(d, s, sh, mm);
		case FR2C_OPMODE_LA:
			return new ShlMask2_LAI(d, s, sh, mm);
		case FR2C_OPMODE_LL:
			return new ShlMask2_LLI(d, s, sh, mm);
		}
		return null;
	}


	//Shift Right And Mask

	private static final class ShrMask2_RRI extends FrOpcodeDST
	{
		public ShrMask2_RRI(int d, int s, int sh, int mm)
			{ super(d, s, 0); this.sh=sh; this.mask=(1<<mm)-1; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=(frm.regi[s]>>sh)&mask; }
		final int sh; final long mask;
	}

	private static final class ShrMask2_RAI extends FrOpcodeDST
	{
		public ShrMask2_RAI(int d, int s, int sh, int mm)
			{ super(d, s, 0); this.sh=sh; this.mask=(1<<mm)-1; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=(frm.argi[s]>>sh)&mask; }
		final int sh; final long mask;
	}

	private static final class ShrMask2_RLI extends FrOpcodeDST
	{
		public ShrMask2_RLI(int d, int s, int sh, int mm)
			{ super(d, s, 0); this.sh=sh; this.mask=(1<<mm)-1; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=(frm.loci[s]>>sh)&mask; }
		int sh; long mask;
	}

	private static final class ShrMask2_ARI extends FrOpcodeDST
	{
		public ShrMask2_ARI(int d, int s, int sh, int mm)
			{ super(d, s, 0); this.sh=sh; this.mask=(1<<mm)-1; }
		public void run(FrVMFrame frm)
			{ frm.argi[d]=(frm.regi[s]>>sh)&mask; }
		final int sh; final long mask;
	}

	private static final class ShrMask2_AAI extends FrOpcodeDST
	{
		public ShrMask2_AAI(int d, int s, int sh, int mm)
			{ super(d, s, 0); this.sh=sh; this.mask=(1<<mm)-1; }
		public void run(FrVMFrame frm)
			{ frm.argi[d]=(frm.argi[s]>>sh)&mask; }
		final int sh; final long mask;
	}

	private static final class ShrMask2_ALI extends FrOpcodeDST
	{
		public ShrMask2_ALI(int d, int s, int sh, int mm)
			{ super(d, s, 0); this.sh=sh; this.mask=(1<<mm)-1; }
		public void run(FrVMFrame frm)
			{ frm.argi[d]=(frm.loci[s]>>sh)&mask; }
		final int sh; final long mask;
	}

	private static final class ShrMask2_LRI extends FrOpcodeDST
	{
		public ShrMask2_LRI(int d, int s, int sh, int mm)
			{ super(d, s, 0); this.sh=sh; this.mask=(1<<mm)-1; }
		public void run(FrVMFrame frm)
			{ frm.loci[d]=(frm.regi[s]>>sh)&mask; }
		final int sh; final long mask;
	}

	private static final class ShrMask2_LAI extends FrOpcodeDST
	{
		public ShrMask2_LAI(int d, int s, int sh, int mm)
			{ super(d, s, 0); this.sh=sh; this.mask=(1<<mm)-1; }
		public void run(FrVMFrame frm)
			{ frm.loci[d]=(frm.argi[s]>>sh)&mask; }
		final int sh; final long mask;
	}

	private static final class ShrMask2_LLI extends FrOpcodeDST
	{
		public ShrMask2_LLI(int d, int s, int sh, int mm)
			{ super(d, s, 0); this.sh=sh; this.mask=(1<<mm)-1; }
		public void run(FrVMFrame frm)
			{ frm.loci[d]=(frm.loci[s]>>sh)&mask; }
		final int sh; final long mask;
	}


	//Mask And Shift Left

	private static final class ShlMask2_RRI extends FrOpcodeDST
	{
		public ShlMask2_RRI(int d, int s, int sh, int mm)
			{ super(d, s, 0); this.sh=sh; this.mask=(1<<mm)-1; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=(frm.regi[s]&mask)<<sh; }
		final int sh; final long mask;
	}

	private static final class ShlMask2_RAI extends FrOpcodeDST
	{
		public ShlMask2_RAI(int d, int s, int sh, int mm)
			{ super(d, s, 0); this.sh=sh; this.mask=(1<<mm)-1; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=(frm.argi[s]&mask)<<sh; }
		final int sh; final long mask;
	}

	private static final class ShlMask2_RLI extends FrOpcodeDST
	{
		public ShlMask2_RLI(int d, int s, int sh, int mm)
			{ super(d, s, 0); this.sh=sh; this.mask=(1<<mm)-1; }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=(frm.loci[s]&mask)<<sh; }
		final int sh; final long mask;
	}

	private static final class ShlMask2_ARI extends FrOpcodeDST
	{
		public ShlMask2_ARI(int d, int s, int sh, int mm)
			{ super(d, s, 0); this.sh=sh; this.mask=(1<<mm)-1; }
		public void run(FrVMFrame frm)
			{ frm.argi[d]=(frm.regi[s]&mask)<<sh; }
		final int sh; final long mask;
	}

	private static final class ShlMask2_AAI extends FrOpcodeDST
	{
		public ShlMask2_AAI(int d, int s, int sh, int mm)
			{ super(d, s, 0); this.sh=sh; this.mask=(1<<mm)-1; }
		public void run(FrVMFrame frm)
			{ frm.argi[d]=(frm.argi[s]&mask)<<sh; }
		final int sh; final long mask;
	}

	private static final class ShlMask2_ALI extends FrOpcodeDST
	{
		public ShlMask2_ALI(int d, int s, int sh, int mm)
			{ super(d, s, 0); this.sh=sh; this.mask=(1<<mm)-1; }
		public void run(FrVMFrame frm)
			{ frm.argi[d]=(frm.loci[s]&mask)<<sh; }
		final int sh; final long mask;
	}

	private static final class ShlMask2_LRI extends FrOpcodeDST
	{
		public ShlMask2_LRI(int d, int s, int sh, int mm)
			{ super(d, s, 0); this.sh=sh; this.mask=(1<<mm)-1; }
		public void run(FrVMFrame frm)
			{ frm.loci[d]=(frm.regi[s]&mask)<<sh; }
		final int sh; final long mask;
	}

	private static final class ShlMask2_LAI extends FrOpcodeDST
	{
		public ShlMask2_LAI(int d, int s, int sh, int mm)
			{ super(d, s, 0); this.sh=sh; this.mask=(1<<mm)-1; }
		public void run(FrVMFrame frm)
			{ frm.loci[d]=(frm.argi[s]&mask)<<sh; }
		final int sh; final long mask;
	}

	private static final class ShlMask2_LLI extends FrOpcodeDST
	{
		public ShlMask2_LLI(int d, int s, int sh, int mm)
			{ super(d, s, 0); this.sh=sh; this.mask=(1<<mm)-1; }
		public void run(FrVMFrame frm)
			{ frm.loci[d]=(frm.loci[s]&mask)<<sh; }
		final int sh; final long mask;
	}
}