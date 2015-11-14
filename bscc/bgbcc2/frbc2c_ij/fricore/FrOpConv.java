package fricore;

public abstract class FrOpConv extends FrOpcode
{
	public static FrOpcode makeOpConv(
		int z, int y, int d, int s)
	{
		switch(z)
		{
		case FR2C_OPCONV_I2L:
		case FR2C_OPCONV_L2I:
			return FrUnOp.makeUnOp(FR2C_TY_I, y, FR2C_UNOP_MOV, d, s);
		case FR2C_OPCONV_F2D:
		case FR2C_OPCONV_D2F:
			return FrUnOp.makeUnOp(FR2C_TY_F, y, FR2C_UNOP_MOV, d, s);

		case FR2C_OPCONV_I2F:	case FR2C_OPCONV_L2F:
		case FR2C_OPCONV_I2D:	case FR2C_OPCONV_L2D:
			switch(y)
			{
			case FR2C_OPMODE_RR:
				return new Conv2_RR_I2F(d, s);
			case FR2C_OPMODE_RA:
				return new Conv2_RA_I2F(d, s);
			case FR2C_OPMODE_RL:
				return new Conv2_RL_I2F(d, s);
			case FR2C_OPMODE_AR:
				return new Conv2_AR_I2F(d, s);
			case FR2C_OPMODE_AA:
				return new Conv2_AA_I2F(d, s);
			case FR2C_OPMODE_AL:
				return new Conv2_AL_I2F(d, s);
			case FR2C_OPMODE_LR:
				return new Conv2_LR_I2F(d, s);
			case FR2C_OPMODE_LA:
				return new Conv2_LA_I2F(d, s);
			case FR2C_OPMODE_LL:
				return new Conv2_LL_I2F(d, s);
			}

		case FR2C_OPCONV_F2I:	case FR2C_OPCONV_F2L:
		case FR2C_OPCONV_D2I:	case FR2C_OPCONV_D2L:
			switch(y)
			{
			case FR2C_OPMODE_RR:
				return new Conv2_RR_F2I(d, s);
			case FR2C_OPMODE_RA:
				return new Conv2_RA_F2I(d, s);
			case FR2C_OPMODE_RL:
				return new Conv2_RL_F2I(d, s);
			case FR2C_OPMODE_AR:
				return new Conv2_AR_F2I(d, s);
			case FR2C_OPMODE_AA:
				return new Conv2_AA_F2I(d, s);
			case FR2C_OPMODE_AL:
				return new Conv2_AL_F2I(d, s);
			case FR2C_OPMODE_LR:
				return new Conv2_LR_F2I(d, s);
			case FR2C_OPMODE_LA:
				return new Conv2_LA_F2I(d, s);
			case FR2C_OPMODE_LL:
				return new Conv2_LL_F2I(d, s);
			}
		}
		return null;
	}


	//Int->Float .. Long->Double

	private static final class Conv2_RR_I2F extends FrOpcodeDST
	{
		public Conv2_RR_I2F(int d, int s)
			{ super(d, s, 0); }
		public void run(FrVMFrame frm)
			{ frm.regf[d]=(double)(frm.regi[s]); }
	}

	private static final class Conv2_RA_I2F extends FrOpcodeDST
	{
		public Conv2_RA_I2F(int d, int s)
			{ super(d, s, 0); }
		public void run(FrVMFrame frm)
			{ frm.regf[d]=(double)(frm.argi[s]); }
	}

	private static final class Conv2_RL_I2F extends FrOpcodeDST
	{
		public Conv2_RL_I2F(int d, int s)
			{ super(d, s, 0); }
		public void run(FrVMFrame frm)
			{ frm.regf[d]=(double)(frm.loci[s]); }
	}

	private static final class Conv2_AR_I2F extends FrOpcodeDST
	{
		public Conv2_AR_I2F(int d, int s)
			{ super(d, s, 0); }
		public void run(FrVMFrame frm)
			{ frm.argf[d]=(double)(frm.regi[s]); }
	}

	private static final class Conv2_AA_I2F extends FrOpcodeDST
	{
		public Conv2_AA_I2F(int d, int s)
			{ super(d, s, 0); }
		public void run(FrVMFrame frm)
			{ frm.argf[d]=(double)(frm.argi[s]); }
	}

	private static final class Conv2_AL_I2F extends FrOpcodeDST
	{
		public Conv2_AL_I2F(int d, int s)
			{ super(d, s, 0); }
		public void run(FrVMFrame frm)
			{ frm.argf[d]=(double)(frm.loci[s]); }
	}

	private static final class Conv2_LR_I2F extends FrOpcodeDST
	{
		public Conv2_LR_I2F(int d, int s)
			{ super(d, s, 0); }
		public void run(FrVMFrame frm)
			{ frm.locf[d]=(double)(frm.regi[s]); }
	}

	private static final class Conv2_LA_I2F extends FrOpcodeDST
	{
		public Conv2_LA_I2F(int d, int s)
			{ super(d, s, 0); }
		public void run(FrVMFrame frm)
			{ frm.locf[d]=(double)(frm.argi[s]); }
	}

	private static final class Conv2_LL_I2F extends FrOpcodeDST
	{
		public Conv2_LL_I2F(int d, int s)
			{ super(d, s, 0); }
		public void run(FrVMFrame frm)
			{ frm.locf[d]=(double)(frm.loci[s]); }
	}



	//Float->Int .. Double->Long

	private static final class Conv2_RR_F2I extends FrOpcodeDST
	{
		public Conv2_RR_F2I(int d, int s)
			{ super(d, s, 0); }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=(long)(frm.regf[s]); }
	}

	private static final class Conv2_RA_F2I extends FrOpcodeDST
	{
		public Conv2_RA_F2I(int d, int s)
			{ super(d, s, 0); }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=(long)(frm.argf[s]); }
	}

	private static final class Conv2_RL_F2I extends FrOpcodeDST
	{
		public Conv2_RL_F2I(int d, int s)
			{ super(d, s, 0); }
		public void run(FrVMFrame frm)
			{ frm.regi[d]=(long)(frm.locf[s]); }
	}

	private static final class Conv2_AR_F2I extends FrOpcodeDST
	{
		public Conv2_AR_F2I(int d, int s)
			{ super(d, s, 0); }
		public void run(FrVMFrame frm)
			{ frm.argi[d]=(long)(frm.regf[s]); }
	}

	private static final class Conv2_AA_F2I extends FrOpcodeDST
	{
		public Conv2_AA_F2I(int d, int s)
			{ super(d, s, 0); }
		public void run(FrVMFrame frm)
			{ frm.argi[d]=(long)(frm.argf[s]); }
	}

	private static final class Conv2_AL_F2I extends FrOpcodeDST
	{
		public Conv2_AL_F2I(int d, int s)
			{ super(d, s, 0); }
		public void run(FrVMFrame frm)
			{ frm.argi[d]=(long)(frm.locf[s]); }
	}

	private static final class Conv2_LR_F2I extends FrOpcodeDST
	{
		public Conv2_LR_F2I(int d, int s)
			{ super(d, s, 0); }
		public void run(FrVMFrame frm)
			{ frm.loci[d]=(long)(frm.regf[s]); }
	}

	private static final class Conv2_LA_F2I extends FrOpcodeDST
	{
		public Conv2_LA_F2I(int d, int s)
			{ super(d, s, 0); }
		public void run(FrVMFrame frm)
			{ frm.loci[d]=(long)(frm.argf[s]); }
	}

	private static final class Conv2_LL_F2I extends FrOpcodeDST
	{
		public Conv2_LL_F2I(int d, int s)
			{ super(d, s, 0); }
		public void run(FrVMFrame frm)
			{ frm.loci[d]=(long)(frm.locf[s]); }
	}
}
