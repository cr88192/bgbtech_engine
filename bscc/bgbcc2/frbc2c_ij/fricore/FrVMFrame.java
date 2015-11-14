package fricore;

public class FrVMFrame
{
	public FrVMFrame(FrVMContext ctx)
	{
		this(ctx, 64, 32, 64);
	}
	
	public FrVMFrame(FrVMContext ctx, int nreg, int narg, int nloc)
	{
		this.ctx=ctx;

		regi=new long[nreg];
		regf=new double[nreg];
		regp=new FrPointer[nreg];

		argi=new long[narg];
		argf=new double[narg];
		argp=new FrPointer[narg];

		loci=new long[nloc];
		locf=new double[nloc];
		locp=new FrPointer[nloc];
	}

	public FrPointer allocLocalData(int sz)
	{
		FrPointer arr;
		int i;
		
		arr=ctx.space.malloc(sz);
		
		if(onRet==null)
		{
			onRet=new FrOpcode[64];
			nOnRet=0;
			mOnRet=64;
		}
		
		i=nOnRet++;
		onRet[i]=new FreeLocalI(arr);
		return arr;
	}

	public FrVMContext ctx;	//owning context
	public FrTrace rettrace;	//return trace
//	public int ret_z, ret_y, ret_d;
	
	public long[] regi;
	public double[] regf;
	public FrPointer[] regp;

	public long[] argi;
	public double[] argf;
	public FrPointer[] argp;

	public long[] loci;
	public double[] locf;
	public FrPointer[] locp;

	public FrOpcode[] onRet;
	public int nOnRet, mOnRet;
	
//	public abstract void run(State ctx);

	private static final class FreeLocalI extends FrOpcode
	{
		public FreeLocalI(FrPointer ptr)
			{ this.ptr=ptr; }
		public void run(FrVMFrame frm)
			{ frm.ctx.space.free(ptr); }
		private final FrPointer ptr;
	}

}
