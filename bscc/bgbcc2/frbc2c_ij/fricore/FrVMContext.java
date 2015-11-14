package fricore;

public class FrVMContext
{
	public void run(FrTrace frm)
	{
		FrTrace cur;
//		try
//		{
			cur=frm;
			while(cur!=null)
				{ cur=cur.run(frame); }
//		}
//		catch(Exception ex)
//		{
//		}
	}

	public FrVMFrame frame;
	public FrVMFrame[] callstack;
	public int callstackpos;

	public long reti;
	public double retf;
	public FrPointer retp;
	
	FrMemSpace space;
}
