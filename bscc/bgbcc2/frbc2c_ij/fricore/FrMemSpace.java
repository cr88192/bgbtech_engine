package fricore;

/** Logical Address Space */
public class FrMemSpace
{
	public FrMemSpace()
		{ this(64); }

	public FrMemSpace(int nrgn)
	{
		rgns=new FrMemRegion[nrgn];
		nRgns=0; mRgns=nrgn;

		images=new FrVMImage[nrgn];
		nImages=0; mImages=nrgn;
		
		rov_dynamic=VABASE_DYNAMIC;
	}

	public FrMemRegion addrToRegion(long addr)
	{
		long rs, re;
		int i;
		
		for(i=0; i<nRgns; i++)
		{
			rs=rgns[i].addr;
			re=rs+((long)rgns[i].szData);
			if((addr>=rs) && (addr<re))
				return(rgns[i]);
		}
		return(null);
	}

	public FrVMImage addrToImage(long addr)
	{
		long rs, re;
		int i;
		
		for(i=0; i<nImages; i++)
		{
			rs=images[i].vaStart;
			re=images[i].vaEnd;
			if((addr>=rs) && (addr<re))
				return(images[i]);
		}
		return(null);
	}

	public FrPointer addrToPointer(long addr)
	{
		FrMemRegion rgn;
		FrVMImage img;
		FrPointer ptr;
		
		rgn=addrToRegion(addr);
		if(rgn!=null)
			{ return rgn.makePointer(addr); }

		img=addrToImage(addr);
		if(img!=null)
		{
			ptr=img.addrToPointer(this, addr);
			if(ptr!=null)
				return ptr;
		}

		return new FrPointerVirt(this, addr);
//		return null;
	}

	public FrMemRegion mmap(long addr, int len, int prot, int flags,
		int fdio, int fdoffs)
	{
		int i;
		
		if(len<=0)
			return(null);
		
		if(addr<=0)
		{
			addr=rov_dynamic;
			rov_dynamic=(rov_dynamic+len+4095)&(~4095);
//			addr=((long)nRgns)<<32;
		}

		if(fdio<=0)
		{
			i=nRgns++;
			rgns[i]=new FrMemRegion(this, addr, len);
			return(rgns[i]);
		}
		
		return(null);
	}

	public FrPointer malloc(int sz)
	{
		FrPointer tptr;
		long addr;
		int len;
		int i, j;
		
		for(i=0; i<nRgns; i++)
		{
			if(rgns[i] instanceof FrMemHeap)
			{
				tptr=((FrMemHeap)rgns[i]).malloc(sz);
				if(tptr!=null)
					return tptr;
			}
		}

		len=1<<24;

		addr=rov_dynamic;
		rov_dynamic=(rov_dynamic+len+4095)&(~4095);

		i=nRgns++;
		rgns[i]=new FrMemRegion(this, addr, len);

		tptr=((FrMemHeap)rgns[i]).malloc(sz);
		if(tptr!=null)
			return tptr;
		return null;
	}

	public boolean free(FrPointer ptr)
	{
		FrMemRegion rgn;
		if(!(ptr instanceof FrPointerRgn))
			return false;
		rgn=((FrPointerRgn)ptr).rgn;
		if(!(rgn instanceof FrMemHeap))
			return false;
		return ((FrMemHeap)rgn).free(ptr);
	}

	public static long rov_dynamic;

	public FrMemRegion[] rgns;
	public int nRgns, mRgns;
	
	public FrVMImage[] images;
	public int nImages, mImages;
	
	/** Base address for dynamically allocated memory */
	public static final long VABASE_DYNAMIC		=0x000000000000L;

	/** Base address for context frame variables */
	public static final long VABASE_CTX_LOCAL		=0x100000000000L;
	public static final long VABASE_CTX_ARGS		=0x110000000000L;
	
	/** Base address for image-controlled memory */
	public static final long VABASE_IMAGE			=0x400000000000L;
}
