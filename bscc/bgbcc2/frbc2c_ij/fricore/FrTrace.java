package fricore;

public abstract class FrTrace
{
    public abstract FrTrace run(FrVMFrame ctx);

    public static FrTrace create(
		FrOpcode[] ops, int nOps,
		FrTailOpcode top)
	{
		switch(nOps)
		{
		case 0:
			return new Trace2A0(top);
		case 1:
			return new Trace2A1(ops[0], top);
		case 2:
			return new Trace2A2(ops[0], ops[1], top);
		case 3:
			return new Trace2A3(ops[0], ops[1], ops[2], top);
		case 4:
			return new Trace2A4(ops[0], ops[1], ops[2], ops[3], top);
		case 5:
			return new Trace2A5(ops[0], ops[1], ops[2], ops[3],
				ops[4], top);
		case 6:
			return new Trace2A6(ops[0], ops[1], ops[2], ops[3],
				ops[4], ops[5], top);
		case 7:
			return new Trace2A7(ops[0], ops[1], ops[2], ops[3],
				ops[4], ops[5], ops[6], top);
		case 8:
			return new Trace2A8(ops[0], ops[1], ops[2], ops[3],
				ops[4], ops[5], ops[6], ops[7], top);
		default:
			return new Trace2An(ops, nOps, top);
		}
	}
    
    public FrFunction func;
    public FrTrace next;
    public int addr;

    public FrTrace jmpnext;
    public int jmpaddr;
    
    private static final class Trace2A0 extends FrTrace
    {
		public Trace2A0(FrTailOpcode top)
			{ tailop=top; }
		public FrTrace run(FrVMFrame ctx)
		{
			return tailop.run(ctx);
		}
		FrTailOpcode tailop;
    }

    private static final class Trace2A1 extends FrTrace
    {
		public Trace2A1(FrOpcode opa0, FrTailOpcode top)
			{ op0=opa0; tailop=top; }
		public FrTrace run(FrVMFrame ctx)
		{
			op0.run(ctx);
			return tailop.run(ctx);
		}
		FrOpcode op0;
		FrTailOpcode tailop;
    }

    private static final class Trace2A2 extends FrTrace
    {
		public Trace2A2(FrOpcode opa0, FrOpcode opa1, FrTailOpcode top)
			{ op0=opa0; op1=opa1; tailop=top; }
		public FrTrace run(FrVMFrame ctx)
		{
			op0.run(ctx);
			op1.run(ctx);
			return tailop.run(ctx);
		}
		FrOpcode op0, op1;
		FrTailOpcode tailop;
    }

    private static final class Trace2A3 extends FrTrace
    {
		public Trace2A3(
				FrOpcode opa0, FrOpcode opa1,
				FrOpcode opa2, FrTailOpcode top)
			{ op0=opa0; op1=opa1; op2=opa2; tailop=top; }
		public FrTrace run(FrVMFrame ctx)
		{
			op0.run(ctx);	op1.run(ctx);
			op2.run(ctx);
			return tailop.run(ctx);
		}
		FrOpcode op0, op1, op2;
		FrTailOpcode tailop;
    }

    private static final class Trace2A4 extends FrTrace
    {
		public Trace2A4(
				FrOpcode opa0, FrOpcode opa1,
				FrOpcode opa2, FrOpcode opa3,
				FrTailOpcode top)
			{ op0=opa0; op1=opa1; op2=opa2; op3=opa3; tailop=top; }
		public FrTrace run(FrVMFrame ctx)
		{
			op0.run(ctx);	op1.run(ctx);
			op2.run(ctx);	op3.run(ctx);
			return tailop.run(ctx);
		}
		FrOpcode op0, op1, op2, op3;
		FrTailOpcode tailop;
    }

    private static final class Trace2A5 extends FrTrace
    {
		public Trace2A5(
			FrOpcode opa0, FrOpcode opa1,
			FrOpcode opa2, FrOpcode opa3,
			FrOpcode opa4,
			FrTailOpcode top)
		{
			op0=opa0; op1=opa1; op2=opa2; op3=opa3;
			op4=opa4;
			tailop=top;
		}
		public FrTrace run(FrVMFrame ctx)
		{
			op0.run(ctx);	op1.run(ctx);
			op2.run(ctx);	op3.run(ctx);
			op4.run(ctx);
			return tailop.run(ctx);
		}
		FrOpcode op0, op1, op2, op3;
		FrOpcode op4;
		FrTailOpcode tailop;
    }

    private static final class Trace2A6 extends FrTrace
    {
		public Trace2A6(
			FrOpcode opa0, FrOpcode opa1,
			FrOpcode opa2, FrOpcode opa3,
			FrOpcode opa4, FrOpcode opa5,
			FrTailOpcode top)
		{
			op0=opa0; op1=opa1; op2=opa2; op3=opa3;
			op4=opa4; op5=opa5;
			tailop=top;
		}
		public FrTrace run(FrVMFrame ctx)
		{
			op0.run(ctx);	op1.run(ctx);
			op2.run(ctx);	op3.run(ctx);
			op4.run(ctx);	op5.run(ctx);
			return tailop.run(ctx);
		}
		FrOpcode op0, op1, op2, op3;
		FrOpcode op4, op5;
		FrTailOpcode tailop;
    }

    private static final class Trace2A7 extends FrTrace
    {
		public Trace2A7(
			FrOpcode opa0, FrOpcode opa1,
			FrOpcode opa2, FrOpcode opa3,
			FrOpcode opa4, FrOpcode opa5,
			FrOpcode opa6,
			FrTailOpcode top)
		{
			op0=opa0; op1=opa1; op2=opa2; op3=opa3;
			op4=opa4; op5=opa5; op6=opa6;
			tailop=top;
		}
		public FrTrace run(FrVMFrame ctx)
		{
			op0.run(ctx);	op1.run(ctx);
			op2.run(ctx);	op3.run(ctx);
			op4.run(ctx);	op5.run(ctx);
			op6.run(ctx);
			return tailop.run(ctx);
		}
		FrOpcode op0, op1, op2, op3;
		FrOpcode op4, op5, op6;
		FrTailOpcode tailop;
    }

    private static final class Trace2A8 extends FrTrace
    {
		public Trace2A8(
			FrOpcode opa0, FrOpcode opa1,
			FrOpcode opa2, FrOpcode opa3,
			FrOpcode opa4, FrOpcode opa5,
			FrOpcode opa6, FrOpcode opa7,
			FrTailOpcode top)
		{
			op0=opa0; op1=opa1; op2=opa2; op3=opa3;
			op4=opa4; op5=opa5; op6=opa6; op7=opa7;
			tailop=top;
		}
		public FrTrace run(FrVMFrame ctx)
		{
			op0.run(ctx);	op1.run(ctx);
			op2.run(ctx);	op3.run(ctx);
			op4.run(ctx);	op5.run(ctx);
			op6.run(ctx);	op7.run(ctx);
			return tailop.run(ctx);
		}
		FrOpcode op0, op1, op2, op3;
		FrOpcode op4, op5, op6, op7;
		FrTailOpcode tailop;
    }

    private static final class Trace2An extends FrTrace
    {
		public Trace2An(
				FrOpcode[] ops, int nOps,
				FrTailOpcode top)
		{
			int i;
			this.ops=new FrOpcode[nOps];
			this.nOps=nOps;
			for(i=0; i<nOps; i++)
				this.ops[i]=ops[i];
			tailop=top;
		}
		public FrTrace run(FrVMFrame ctx)
		{
			int i;
			for(i=0; i<nOps; i++)
				{ ops[i].run(ctx); }
			return tailop.run(ctx);
		}
		FrOpcode[] ops;
		int nOps;
		FrTailOpcode tailop;
    }
}
