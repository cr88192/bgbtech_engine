package fricore;
import java.lang.*;

/** Represents a virtual pointer object.
  * Points to an region of bytes controlled by a particular VM image.
  * By implication, this memory is read-only.
  */
public class FrPointerImgBytes extends FrPointer
{
	public FrPointerImgBytes(FrMemSpace space, FrVMImage img, long a,
			byte[] vaData, long vaBase, long vaEnd)
	{
		this.space=space; this.img=img; this.addr=a;
		this.vaData=vaData; this.vaBase=vaBase; this.vaEnd=vaEnd;
	}

	private void accessEx()
	{
		throw new FrSegFaultException();
	}

	private FrPointer createPointer(long newaddr)
	{
		if((newaddr<vaBase) || (newaddr>=vaEnd))
			{ return(space.addrToPointer(newaddr)); }
		return new FrPointerImgBytes(space, img, newaddr,
			vaData, vaBase, vaEnd);
	}

	public int getsb(int idx)
		{ return vaData[(int)(addr-vaBase)+idx]; }
	public int getub(int idx)
		{ return vaData[(int)(addr-vaBase)+idx]&255; }

	public int getss(int idx)
		{ return getub(2*idx+0)|(getsb(2*idx+1)<<8); }
	public int getus(int idx)
		{ return getub(2*idx+0)|(getub(2*idx+1)<<8); }

	public int geti(int idx)
		{ return getus(2*idx+0)|(getss(2*idx+1)<<16); }
	public long getsi(int idx)
		{ return ((long)getus(2*idx+0))|(((long)getss(2*idx+1))<<16); }
	public long getui(int idx)
		{ return ((long)getus(2*idx+0))|(((long)getus(2*idx+1))<<16); }
	public long getl(int idx)
		{ return getui(2*idx+0)|(getsi(2*idx+1)<<32); }

	public float getf(int idx)
		{ return Float.intBitsToFloat(geti(idx)); }
	public double getd(int idx)
		{ return Double.longBitsToDouble(getl(idx)); }

	public long getsl(int idx)
		{ return getl(idx); }
	public long getul(int idx)
		{ return getl(idx); }

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
	final FrVMImage img;
	final long addr;
	
	final byte[] vaData;
	final long vaBase;
	final long vaEnd;
}
