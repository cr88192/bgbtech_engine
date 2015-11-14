package fricore;
import static fricore.FrOpcode.*;

public abstract class FrTailOpcode
{
    public abstract FrTrace run(FrVMFrame ctx);

	public static FrTailOpcode decodeTailOpcode(
		FrVMImage img, FrBytePointer cs, FrBytePointer cse)
	{
		int i, j, k, l;
		int a, b, c;
		int d, s, t;
	
		i=cs.getub(0);
		switch(i)
		{
		case FR2C_OP_JCMP_I:
			cs.ninc(); j=cs.getub_npp(); k=cs.getub_npp();
			l=cs.getss_npp();
			a=cs.getAddr()+l;
			return FrOpJCmp.makeCmpOp(FR2C_TY_I, j&15,
				(j>>4)&15, (k>>4)&15, k&15, a);
		case FR2C_OP_JCMP_L:
			cs.ninc(); j=cs.getub_npp(); k=cs.getub_npp();
			l=cs.getss_npp();
			a=cs.getAddr()+l;
			return FrOpJCmp.makeCmpOp(FR2C_TY_L, j&15,
				(j>>4)&15, (k>>4)&15, k&15, a);
		case FR2C_OP_JCMP_F:
			cs.ninc(); j=cs.getub_npp(); k=cs.getub_npp();
			l=cs.getss_npp();
			a=cs.getAddr()+l;
			return FrOpJCmp.makeCmpOp(FR2C_TY_F, j&15,
				(j>>4)&15, (k>>4)&15, k&15, a);
		case FR2C_OP_JCMP_D:
			cs.ninc(); j=cs.getub_npp(); k=cs.getub_npp();
			l=cs.getss_npp(); a=cs.getAddr()+l;
			return FrOpJCmp.makeCmpOp(FR2C_TY_D, j&15,
				(j>>4)&15, (k>>4)&15, k&15, a);

		case FR2C_OP_JCMP_IC:
			cs.ninc(); j=cs.getub_npp();
			k=cs.getub_npp(); l=cs.getub_npp();
			l=cs.getss_npp(); a=cs.getAddr()+l;
			if(((j>>4)&15)<8)
			{	return FrOpJCmpC.makeCmpOp(FR2C_TY_I, j&15,
					(j>>4)&15, (k>>4)&15,
					img.expandConst12Int(((k&15)<<8)|l), a); }
			else
			{	return FrOpJCmpC.makeCmpOp(FR2C_TY_I, j&15,
					(j>>4)&15, (k>>4)&15,
					img.getLiteralInt(((k&15)<<8)|l), a);	}
		case FR2C_OP_JCMP_LC:
			cs.ninc(); j=cs.getub_npp();
			k=cs.getub_npp(); l=cs.getub_npp();
			l=cs.getss_npp(); a=cs.getAddr()+l;
			if(((j>>4)&15)<8)
			{	return FrOpJCmpC.makeCmpOp(FR2C_TY_L, j&15,
					(j>>4)&15, (k>>4)&15,
					img.expandConst12Int(((k&15)<<8)|l), a); }
			else
			{	return FrOpJCmpC.makeCmpOp(FR2C_TY_L, j&15,
					(j>>4)&15, (k>>4)&15,
					img.getLiteralLong(((k&15)<<8)|l), a);	}
		case FR2C_OP_JCMP_FC:
			cs.ninc(); j=cs.getub_npp();
			k=cs.getub_npp(); l=cs.getub_npp();
			l=cs.getss_npp(); a=cs.getAddr()+l;
			if(((j>>4)&15)<8)
			{	return FrOpJCmpC.makeCmpOp(FR2C_TY_F, j&15,
					(j>>4)&15, (k>>4)&15,
					img.expandConst12Float(((k&15)<<8)|l), a); }
			else
			{	return FrOpJCmpC.makeCmpOp(FR2C_TY_F, j&15,
					(j>>4)&15, (k>>4)&15,
					img.getLiteralFloat(((k&15)<<8)|l), a);	}
		case FR2C_OP_JCMP_DC:
			cs.ninc(); j=cs.getub_npp();
			k=cs.getub_npp(); l=cs.getub_npp();
			l=cs.getss_npp(); a=cs.getAddr()+l;
			if(((j>>4)&15)<8)
			{	return FrOpJCmpC.makeCmpOp(FR2C_TY_D, j&15,
					(j>>4)&15, (k>>4)&15,
					img.expandConst12Float(((k&15)<<8)|l), a); }
			else
			{	return FrOpJCmpC.makeCmpOp(FR2C_TY_D, j&15,
					(j>>4)&15, (k>>4)&15,
					img.getLiteralDouble(((k&15)<<8)|l), a);	}

		case FR2C_OP_JMP:
			cs.ninc(); i=cs.getss_npp();
			a=cs.getAddr()+i;
			return new OpTailJmp(a);
		case FR2C_OP_JMPW:
			cs.ninc(); i=cs.getsi_npp();
			a=cs.getAddr()+i;
			return new OpTailJmp(a);

		case FR2C_OP_JCMP_RZI:
			cs.ninc(); j=cs.getub_npp();
			l=cs.getss_npp(); a=cs.getAddr()+l;
			return FrOpJCmpC.makeCmpOp(FR2C_TY_I, FR2C_OPMODE_RR,
				(j>>4)&15, j&15, 0, a);
		case FR2C_OP_JCMP_RZL:
			cs.ninc(); j=cs.getub_npp();
			l=cs.getss_npp(); a=cs.getAddr()+l;
			return FrOpJCmpC.makeCmpOp(FR2C_TY_L, FR2C_OPMODE_RR,
				(j>>4)&15, j&15, 0, a);
		case FR2C_OP_JCMP_RZF:
			cs.ninc(); j=cs.getub_npp();
			l=cs.getss_npp(); a=cs.getAddr()+l;
			return FrOpJCmpC.makeCmpOp(FR2C_TY_F, FR2C_OPMODE_RR,
				(j>>4)&15, j&15, 0, a);
		case FR2C_OP_JCMP_RZD:
			cs.ninc(); j=cs.getub_npp();
			l=cs.getss_npp(); a=cs.getAddr()+l;
			return FrOpJCmpC.makeCmpOp(FR2C_TY_D, FR2C_OPMODE_RR,
				(j>>4)&15, j&15, 0, a);

		case FR2C_OP_JCMP_LZI:
			cs.ninc(); j=cs.getub_npp();
			l=cs.getss_npp(); a=cs.getAddr()+l;
			return FrOpJCmpC.makeCmpOp(FR2C_TY_I, FR2C_OPMODE_RL,
				(j>>4)&15, j&15, 0, a);
		case FR2C_OP_JCMP_LZL:
			cs.ninc(); j=cs.getub_npp();
			l=cs.getss_npp(); a=cs.getAddr()+l;
			return FrOpJCmpC.makeCmpOp(FR2C_TY_L, FR2C_OPMODE_RL,
				(j>>4)&15, j&15, 0, a);
		case FR2C_OP_JCMP_LZF:
			cs.ninc(); j=cs.getub_npp();
			l=cs.getss_npp(); a=cs.getAddr()+l;
			return FrOpJCmpC.makeCmpOp(FR2C_TY_F, FR2C_OPMODE_RL,
				(j>>4)&15, j&15, 0, a);
		case FR2C_OP_JCMP_LZD:
			cs.ninc(); j=cs.getub_npp();
			l=cs.getss_npp(); a=cs.getAddr()+l;
			return FrOpJCmpC.makeCmpOp(FR2C_TY_D, FR2C_OPMODE_RL,
				(j>>4)&15, j&15, 0, a);

		case FR2C_OP_JCMP_AZI:
			cs.ninc(); j=cs.getub_npp();
			l=cs.getss_npp(); a=cs.getAddr()+l;
			return FrOpJCmpC.makeCmpOp(FR2C_TY_I, FR2C_OPMODE_RA,
				(j>>4)&15, j&15, 0, a);
		case FR2C_OP_JCMP_AZL:
			cs.ninc(); j=cs.getub_npp();
			l=cs.getss_npp(); a=cs.getAddr()+l;
			return FrOpJCmpC.makeCmpOp(FR2C_TY_L, FR2C_OPMODE_RA,
				(j>>4)&15, j&15, 0, a);
		case FR2C_OP_JCMP_AZF:
			cs.ninc(); j=cs.getub_npp();
			l=cs.getss_npp(); a=cs.getAddr()+l;
			return FrOpJCmpC.makeCmpOp(FR2C_TY_F, FR2C_OPMODE_RA,
				(j>>4)&15, j&15, 0, a);
		case FR2C_OP_JCMP_AZD:
			cs.ninc(); j=cs.getub_npp();
			l=cs.getss_npp(); a=cs.getAddr()+l;
			return FrOpJCmpC.makeCmpOp(FR2C_TY_D, FR2C_OPMODE_RA,
				(j>>4)&15, j&15, 0, a);

		case FR2C_OP_RET_V:
			cs.ninc();
			return new OpTailRetV();
		case FR2C_OP_RET:
			cs.ninc(); j=cs.getub_npp();
			return new OpTailRet((j>>4)&15, j&15);

		case FR2C_OP_LABEL:
			cs.ninc();
			return new OpTailNop();
		case FR2C_OP_LABEL2:
			cs.ninc(); i=cs.getuw24_npp();
			return new OpTailNop();

		case FR2C_OP_CALL:
			return FrOpCall.decodeCall(img, cs, cse);

		default:
			return null;
		}
	}

	private static final class OpTailNop extends FrTailOpcode
		{ public FrTrace run(FrVMFrame frm) { return trace.next; } }

	private static final class OpTailJmp extends FrTailOpcode
	{
		public OpTailJmp(int addr)
			{ jmpaddr=addr; }
		public FrTrace run(FrVMFrame frm)
			{ return trace.jmpnext; }
	}

	private static final class OpTailRetV extends FrTailOpcode
	{
		public OpTailRetV()
			{  }
		public FrTrace run(FrVMFrame frm)
		{
			FrVMFrame frame;
			if(frm.ctx.callstackpos>0)
			{
				frame=frm.ctx.callstack[--frm.ctx.callstackpos];
				frm.ctx.frame=frame;
				return frame.rettrace;
			}
			return null;
		}
	}

	private static final class OpTailRet extends FrTailOpcode
	{
		public OpTailRet(int w, int s)
			{ this.w=w; this.s=s; }
		public FrTrace run(FrVMFrame frm)
		{
			FrVMFrame frame;
			switch(w)
			{
			case FR2C_OPMODE2_IR:	case FR2C_OPMODE2_LR:
				frm.ctx.reti=frm.regi[s]; break;
			case FR2C_OPMODE2_IA:	case FR2C_OPMODE2_LA:
				frm.ctx.reti=frm.argi[s]; break;
			case FR2C_OPMODE2_IL:	case FR2C_OPMODE2_LL:
				frm.ctx.reti=frm.loci[s]; break;
			case FR2C_OPMODE2_FR:	case FR2C_OPMODE2_DR:
				frm.ctx.retf=frm.regf[s]; break;
			case FR2C_OPMODE2_FA:	case FR2C_OPMODE2_DA:
				frm.ctx.retf=frm.argf[s]; break;
			case FR2C_OPMODE2_FL:	case FR2C_OPMODE2_DL:
				frm.ctx.retf=frm.locf[s]; break;
			case FR2C_OPMODE2_PR:
				frm.ctx.retp=frm.regp[s]; break;
			case FR2C_OPMODE2_PA:
				frm.ctx.retp=frm.argp[s]; break;
			case FR2C_OPMODE2_PL:
				frm.ctx.retp=frm.locp[s]; break;
			}

			if(frm.ctx.callstackpos>0)
			{
				frame=frm.ctx.callstack[--frm.ctx.callstackpos];
				frm.ctx.frame=frame;				
				return frame.rettrace;
			}
		
			return null;
		}
		final int w, s;
	}

	public FrTrace trace;
    public int jmpaddr=-1;
}
