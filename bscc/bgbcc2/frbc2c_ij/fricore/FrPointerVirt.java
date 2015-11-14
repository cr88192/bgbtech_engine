package fricore;

/** Represents a virtual pointer object.
  * In this case, the pointer is outside of an addressable memory region.
  * However, such pointers still need to be able to be represented.
  * Throw FrSegFaultException if any attempt is made to access memory 
  * through it.
  */
public class FrPointerVirt extends FrPointer
{
	public FrPointerVirt(FrMemSpace space, long a)
		{ this.space=space; this.addr=a; }

	private void accessEx()
	{
		throw new FrSegFaultException();
	}

	private FrPointer createPointer(long newaddr)
	{
		if((newaddr&(~4095))!=(addr&(~4095)))
			{ return(space.addrToPointer(newaddr)); }
		return new FrPointerVirt(space, newaddr);
	}

	public int geti(int idx)
		{ accessEx(); return 0; }
	public long getl(int idx)
		{ accessEx(); return 0; }
	public float getf(int idx)
		{ accessEx(); return 0.0F; }
	public double getd(int idx)
		{ accessEx(); return 0.0; }
	public int getsb(int idx)
		{ accessEx(); return 0; }
	public int getub(int idx)
		{ accessEx(); return 0; }
	public int getss(int idx)
		{ accessEx(); return 0; }
	public int getus(int idx)
		{ accessEx(); return 0; }
	public long getsi(int idx)
		{ accessEx(); return 0; }
	public long getui(int idx)
		{ accessEx(); return 0; }
	public long getsl(int idx)
		{ accessEx(); return 0; }
	public long getul(int idx)
		{ accessEx(); return 0; }

	public void seti(int idx, int val)
		{ accessEx(); }
	public void setl(int idx, long val)
		{ accessEx(); }
	public void setf(int idx, float val)
		{ accessEx(); }
	public void setd(int idx, double val)
		{ accessEx(); }
	public void setsb(int idx, int val)
		{ accessEx(); }
	public void setub(int idx, int val)
		{ accessEx(); }
	public void setss(int idx, int val)
		{ accessEx(); }
	public void setus(int idx, int val)
		{ accessEx(); }

	public FrPointer getp(int idx)
		{ accessEx(); return null; }
	public void setp(int idx, FrPointer val)
		{ accessEx(); }

	public FrPointer leai(int idx)
		{ return createPointer(addr+(idx<<2)); }
	public FrPointer leal(int idx)
		{ return createPointer(addr+(idx<<3)); }
	public FrPointer leaf(int idx)
		{ return createPointer(addr+(idx<<2)); }
	public FrPointer lead(int idx)
		{ return createPointer(addr+(idx<<3)); }
	public FrPointer leab(int idx)
		{ return createPointer(addr+idx); }
	public FrPointer leas(int idx)
		{ return createPointer(addr+(idx<<1)); }
	public FrPointer leap(int idx)
		{ return createPointer(addr+(idx<<3)); }

	public FrMemSpace getSpace()
		{ return space; }
	public long getAddress()
		{ return addr; }
	
	final FrMemSpace space;
	final long addr;
}
