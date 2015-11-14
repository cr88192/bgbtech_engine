package fricore;

import java.lang.*;

public class FrPointerRgn extends FrPointer
{
	public FrPointerRgn(FrMemRegion rgn, int base)
		{ this.rgn=rgn; this.base=base; }

	public int geti(int idx)
	{
		int i, j;
		
		if((base&3)!=0)
		{
			i=idx<<2;
			j=	getub(i+0)|(getub(i+1)<<8)|
				(getub(i+2)<<16)|(getub(i+3)<<24);
			return(j);
		}
		
		i=(base>>2)+idx;
		return(rgn.data[i]);
	}

	public long getl(int idx)
	{
		int i, j, k;
		
		if((base&3)!=0)
		{
			i=idx<<1;
			j=geti(i); k=geti(i+1);
			return(((long)j)|(((long)k)<<32));
		}
		
		i=base+(idx<<3);
		return(((long)rgn.data[i>>2])+
			(((long)rgn.data[(i>>2)+1])<<32));
	}
	
	public int getsb(int idx)
	{
		return((int)((byte)getub(idx)));
	}

	public int getub(int idx)
	{
		int i, j;
		i=base+idx;
		j=(rgn.data[i>>2]>>((i&3)<<3))&255;
		return(j);
	}
	
	public int getss(int idx)
	{
		return((int)((short)getus(idx)));
	}

	public int getus(int idx)
	{
		return(getub(idx*2)|(getub(idx*2+1)<<8));
	}

	public long getsi(int idx)
		{ return((long)geti(idx)); }
	public long getui(int idx)
		{ return(((long)geti(idx))&4294967295L); }
	public long getsl(int idx)
		{ return(getl(idx)); }
	public long getul(int idx)
		{ return(getl(idx)); }

	public void seti(int idx, int val)
	{
		int i, j;

		if((base&3)!=0)
		{
			i=idx<<2;
			setub(i+0, val);		setub(i+1, val>>8);
			setub(i+2, val>>16);	setub(i+3, val>>24);
			return;
		}

		i=(base>>2)+idx;
		rgn.data[i]=val;
	}

	public void setl(int idx, long val)
	{
		int i, j;

		if((base&3)!=0)
		{
			i=idx<<3;
			setub(i+0, (int)(val));		setub(i+1, (int)(val>>8));
			setub(i+2, (int)(val>>16));	setub(i+3, (int)(val>>24));
			setub(i+4, (int)(val>>32));	setub(i+5, (int)(val>>40));
			setub(i+6, (int)(val>>48));	setub(i+7, (int)(val>>56));
			return;
		}

		i=(base>>2)+(idx<<1);
		rgn.data[i+0]=(int)val;
		rgn.data[i+1]=(int)(val>>32);
	}

	public void setub(int idx, int val)
	{
		int i, j, k, l;
		l=val&255;
		i=base+idx; j=((i&3)<<3);
		k=rgn.data[i];
		k=(k&(~(255<<j)))|(l<<j);
		rgn.data[i]=k;
	}
	
	public void setus(int idx, int val)
	{
		setub((idx<<1)+0, val);
		setub((idx<<1)+1, val>>8);
	}

	public void setsb(int idx, int val)
		{ setub(idx, val); }
	public void setss(int idx, int val)
		{ setus(idx, val); }

	
	public float getf(int idx)
	{
		int v;
		v=geti(idx);
		return Float.intBitsToFloat(v);
	}
	
	public double getd(int idx)
	{
		long v;
		v=getl(idx);
		return Double.longBitsToDouble(v);
	}

	public void setf(int idx, float val)
	{
		int v;
		v=Float.floatToIntBits(val);
		seti(idx, v);
	}
	
	public void setd(int idx, double val)
	{
		long v;
		v=Double.doubleToLongBits(val);
		setl(idx, v);
	}

	public FrPointer getp(int idx)
		{ return(getSpace().addrToPointer(getl(idx))); }
	public void setp(int idx, FrPointer val)
		{ setl(idx, val.getAddress()); }

	public FrPointer leai(int idx)
		{ return new FrPointerRgn(rgn, base+(idx<<2)); }
	public FrPointer leal(int idx)
		{ return new FrPointerRgn(rgn, base+(idx<<3)); }
	public FrPointer leaf(int idx)
		{ return new FrPointerRgn(rgn, base+(idx<<2)); }
	public FrPointer lead(int idx)
		{ return new FrPointerRgn(rgn, base+(idx<<3)); }
	public FrPointer leab(int idx)
		{ return new FrPointerRgn(rgn, base+idx); }
	public FrPointer leas(int idx)
		{ return new FrPointerRgn(rgn, base+(idx<<1)); }
	public FrPointer leap(int idx)
		{ return new FrPointerRgn(rgn, base+(idx<<3)); }

	public FrMemSpace getSpace()
		{ return(rgn.space); }
	public long getAddress()
		{ return(rgn.addr+((long)base)); }
	
	public FrMemRegion rgn;
	public int base;
}
