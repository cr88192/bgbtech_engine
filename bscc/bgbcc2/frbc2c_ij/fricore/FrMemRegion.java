package fricore;

public class FrMemRegion
{
	public FrMemRegion(FrMemSpace space, long addr, int sz)
	{
		this.space=space;
		this.addr=addr;
		this.data=new int[sz>>2];
		this.szData=sz;
	}

	public FrMemRegion(FrMemSpace space, long addr,
		byte[] init, int sz)
	{
		this(space, addr, sz);
		for(int i=0; i<sz; i++)
			data[i]=init[i];
	}

	public FrPointer makePointer(long addr)
		{ return new FrPointerRgn(this, (int)(addr-this.addr)); }

	FrMemSpace space;
	public int[] data;
	public int szData;
	public long addr;
}
