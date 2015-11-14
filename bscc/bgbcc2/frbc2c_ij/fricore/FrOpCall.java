package fricore;
import static fricore.FrOpcode.*;

public abstract class FrOpCall extends FrTailOpcode
{
	public static FrTailOpcode decodeCall(
		FrVMImage img, FrBytePointer cs, FrBytePointer cse)
	{
		CopyArg[] ca;
		FrGlobal fcn;
		int nca;
		int i, j, k;
		
		cs.ninc(); j=cs.getuw24_npp();
		nca=cs.getub_npp();
		
		ca=new CopyArg[nca];
		for(i=0; i<nca; i++)
			{ ca[i]=decodeCopyArg(img, cs, cse, i); }
		
		fcn=img.getGlobal(j);
		
		if(fcn instanceof FrFunction)
			return new CallFunc((FrFunction)fcn, ca, nca);
		
		return null;
	}

	private static CopyArg decodeCopyArg(
		FrVMImage img, FrBytePointer cs, FrBytePointer cse,
		int an)
	{
		int i, j, k;

		i=cs.getub_npp();
		
		switch((i>>4)&15)
		{
		case 0x0: return new CopyArg_IR(an, i&15);
		case 0x1: return new CopyArg_IR(an, i&15);
		case 0x2: return new CopyArg_FR(an, i&15);
		case 0x3: return new CopyArg_FR(an, i&15);
		case 0x4: return new CopyArg_PR(an, i&15);
		case 0x5: return new CopyArg_IL(an, i&15);
		case 0x6: return new CopyArg_IL(an, i&15);
		case 0x7: return new CopyArg_FL(an, i&15);
		case 0x8: return new CopyArg_FL(an, i&15);
		case 0x9: return new CopyArg_PL(an, i&15);
		}
		
		switch(i)
		{
		case 0xA0:	case 0xA1:
			j=cs.getub_npp();
			return new CopyArg_IR(an, j);
		case 0xA2:	case 0xA3:
			j=cs.getub_npp();
			return new CopyArg_FR(an, j);
		case 0xA4:
			j=cs.getub_npp();
			return new CopyArg_PR(an, j);
		case 0xA5:	case 0xA6:
			j=cs.getub_npp();
			return new CopyArg_IA(an, j);
		case 0xA7:	case 0xA8:
			j=cs.getub_npp();
			return new CopyArg_FA(an, j);
		case 0xA9:
			j=cs.getub_npp();
			return new CopyArg_PA(an, j);
		case 0xAA:	case 0xAB:
			j=cs.getub_npp();
			return new CopyArg_IL(an, j);
		case 0xAC:	case 0xAD:
			j=cs.getub_npp();
			return new CopyArg_FL(an, j);
		case 0xAE:
			j=cs.getub_npp();
			return new CopyArg_PL(an, j);

		case 0xB0:	case 0xB1:
			j=cs.getsb_npp();
			return new CopyArg_IC(an, (long)j);
		case 0xB2:	case 0xB3:
			j=cs.getss_npp();
			return new CopyArg_IC(an, (long)j);

		case 0xB4:
			j=cs.getuw24_npp();
			return new CopyArg_IC(an, img.getLiteralInt(j));
		case 0xB5:
			j=cs.getuw24_npp();
			return new CopyArg_IC(an, img.getLiteralLong(j));
		case 0xB6:
			j=cs.getuw24_npp();
			return new CopyArg_FC(an, img.getLiteralFloat(j));
		case 0xB7:
			j=cs.getuw24_npp();
			return new CopyArg_FC(an, img.getLiteralDouble(j));
		}
		return null;
	}

	private static abstract class CopyArg
	{
		public CopyArg(int d, int s)
			{ this.d=d; this.s=s; }

		public abstract void run(FrVMFrame dst, FrVMFrame src);
		public final int d, s;
	}

	private static final class CopyArg_IR extends CopyArg
	{
		public CopyArg_IR(int d, int s)
			{ super(d, s); }
		public void run(FrVMFrame dst, FrVMFrame src)
			{ dst.argi[d]=src.regi[s]; }
	}

	private static final class CopyArg_IA extends CopyArg
	{
		public CopyArg_IA(int d, int s)
			{ super(d, s); }
		public void run(FrVMFrame dst, FrVMFrame src)
			{ dst.argi[d]=src.argi[s]; }
	}

	private static final class CopyArg_IL extends CopyArg
	{
		public CopyArg_IL(int d, int s)
			{ super(d, s); }
		public void run(FrVMFrame dst, FrVMFrame src)
			{ dst.argi[d]=src.loci[s]; }
	}

	private static final class CopyArg_FR extends CopyArg
	{
		public CopyArg_FR(int d, int s)
			{ super(d, s); }
		public void run(FrVMFrame dst, FrVMFrame src)
			{ dst.argf[d]=src.regf[s]; }
	}

	private static final class CopyArg_FA extends CopyArg
	{
		public CopyArg_FA(int d, int s)
			{ super(d, s); }
		public void run(FrVMFrame dst, FrVMFrame src)
			{ dst.argf[d]=src.argf[s]; }
	}

	private static final class CopyArg_FL extends CopyArg
	{
		public CopyArg_FL(int d, int s)
			{ super(d, s); }
		public void run(FrVMFrame dst, FrVMFrame src)
			{ dst.argf[d]=src.locf[s]; }
	}

	private static final class CopyArg_PR extends CopyArg
	{
		public CopyArg_PR(int d, int s)
			{ super(d, s); }
		public void run(FrVMFrame dst, FrVMFrame src)
			{ dst.argp[d]=src.regp[s]; }
	}

	private static final class CopyArg_PA extends CopyArg
	{
		public CopyArg_PA(int d, int s)
			{ super(d, s); }
		public void run(FrVMFrame dst, FrVMFrame src)
			{ dst.argp[d]=src.argp[s]; }
	}

	private static final class CopyArg_PL extends CopyArg
	{
		public CopyArg_PL(int d, int s)
			{ super(d, s); }
		public void run(FrVMFrame dst, FrVMFrame src)
			{ dst.argp[d]=src.locp[s]; }
	}

	private static final class CopyArg_IC extends CopyArg
	{
		public CopyArg_IC(int d, long s)
			{ super(d, 0); this.imm=s; }
		public void run(FrVMFrame dst, FrVMFrame src)
			{ dst.argi[d]=imm; }
		public final long imm;
	}

	private static final class CopyArg_FC extends CopyArg
	{
		public CopyArg_FC(int d, double s)
			{ super(d, 0); this.imm=s; }
		public void run(FrVMFrame dst, FrVMFrame src)
			{ dst.argf[d]=imm; }
		public final double imm;
	}
	
	private static final class CallFunc extends FrOpCall
	{
		public CallFunc(FrFunction func, CopyArg[] ca, int nca)
		{
			this.func=func;
			this.ca=ca;
			this.nca=nca;
		}
		
		public FrTrace run(FrVMFrame frm)
		{
			FrVMFrame newframe;
			int i;

			frm.rettrace=trace.next;
			frm.ctx.callstack[frm.ctx.callstackpos++]=frm;

			newframe=func.createFrame(frm.ctx);
			frm.ctx.frame=newframe;
			
			for(i=0; i<nca; i++)
				ca[i].run(newframe, frm);
			
			return func.decode();
		}

		private final FrFunction func;
		private final CopyArg[] ca;
		private final int nca;
	}
}