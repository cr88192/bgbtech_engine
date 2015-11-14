package fricore;

public class FrBytePointer
{
	public FrBytePointer(byte[] data, int szData)
	{
		this.data=data;
		this.szData=szData;
		this.bData=0;
	}

	public FrBytePointer(byte[] data, int bData, int szData)
	{
		this.data=data;
		this.szData=szData;
		this.bData=bData;
	}

	public int getsb(int idx)
		{ return(data[bData+idx]); }
	public int getub(int idx)
		{ return(data[bData+idx]&255); }
	public int getss(int idx)
		{ return((getsb(idx+0)<<8)|getub(idx+1)); }
	public int getus(int idx)
		{ return((getub(idx+0)<<8)|getub(idx+1)); }
	public int getsi(int idx)
		{ return((getss(idx+0)<<16)|getus(idx+2)); }
	public int getui(int idx)
		{ return((getus(idx+0)<<16)|getus(idx+2)); }
	public int getsw24(int idx)
		{ return((getsb(idx+0)<<16)|getus(idx+1)); }
	public int getuw24(int idx)
		{ return((getub(idx+0)<<16)|getus(idx+1)); }

	public long getsl(int idx)
		{ return((((long)getsi(idx+0))<<32)|((long)getui(idx+4))); }
	public long getul(int idx)
		{ return((((long)getui(idx+0))<<32)|((long)getui(idx+4))); }

	public void setb(int idx, int val)
		{ data[bData+idx]=(byte)val; }
	public void sets(int idx, int val)
		{ setb(2*idx+0, (val>>8)&255); setb(2*idx+1, val&255); }
	public void seti(int idx, int val)
		{ sets(2*idx+0, (val>>16)&65535); sets(2*idx+1, val&65535); }
	public void setl(int idx, long val)
		{ seti(2*idx+0, (int)(val>>32)); seti(2*idx+1, (int)val); }

	public void setb_le(int idx, int val)
		{ data[bData+idx]=(byte)val; }
	public void sets_le(int idx, int val)
		{ setb(2*idx+0, val&255); setb(2*idx+1, (val>>8)&255); }
	public void seti_le(int idx, int val)
		{ sets(2*idx+0, val&65535); sets(2*idx+1, (val>>16)&65535); }
	public void setl_le(int idx, long val)
		{ seti(2*idx+0, (int)val); seti(2*idx+1, (int)(val>>32)); }

	public FrBytePointer add(int idx)
		{ return new FrBytePointer(data, bData+idx, szData); }
	public FrBytePointer inc()
		{ return add(1); }
	public FrBytePointer dec()
		{ return add(1); }

	public void nadd(int idx)
		{ bData+=idx; }
	public void ninc()
		{ nadd(1); }
	public void ndec()
		{ nadd(1); }

	public int getsb_npp()
		{ nadd(1); return(getsb(-1)); }
	public int getub_npp()
		{ nadd(1); return(getub(-1)); }
	public int getss_npp()
		{ nadd(2); return(getss(-2)); }
	public int getus_npp()
		{ nadd(2); return(getus(-2)); }
	public int getsi_npp()
		{ nadd(4); return(getsi(-4)); }
	public int getui_npp()
		{ nadd(4); return(getui(-4)); }
	public int getsw24_npp()
		{ nadd(3); return(getsw24(-3)); }
	public int getuw24_npp()
		{ nadd(3); return(getuw24(-3)); }

	public long getsl_npp()
		{ nadd(8); return(getsl(-8)); }
	public long getul_npp()
		{ nadd(8); return(getul(-8)); }

	public boolean eq(FrBytePointer other)
		{ return(bData==other.bData); }
	public boolean ne(FrBytePointer other)
		{ return(bData!=other.bData); }
	public boolean lt(FrBytePointer other)
		{ return(bData<other.bData); }
	public boolean gt(FrBytePointer other)
		{ return(bData>other.bData); }
	public boolean le(FrBytePointer other)
		{ return(bData<=other.bData); }
	public boolean ge(FrBytePointer other)
		{ return(bData>=other.bData); }

	public int sub(FrBytePointer other)
		{ return(bData-other.bData); }

	public char readUtf8CharN()
	{
		int i, j, k, l;
		
		//0xxx-xxxx	(0-127)
		//10xx-xxxx (Invalid First Char)
		//110x-xxxx 10xx-xxxx (0-2047)
		//1110-xxxx 10xx-xxxx 10xx-xxxx (0-65535)
		//1111-0xxx 10xx-xxxx 10xx-xxxx 10xx-xxxx (0-2M)
		
		i=getub_npp();
		if(i<0x80)
			return((char)i);
		if(i<0xC0)
			return((char)i);
		if(i<0xE0)
		{
			j=getub_npp();
			return((char)(((i&31)<<6)|(j&63)));
		}
		if(i<0xF0)
		{
			j=getub_npp(); k=getub_npp();
			return((char)(((i&15)<<12)|((j&63)<<6)|(k&63)));
		}
		if(i<0xF8)
		{
			j=getub_npp(); k=getub_npp(); l=getub_npp();
			return((char)(((i&7)<<18)|((j&63)<<12)|((k&63)<<6)|(l&63)));
		}
		return((char)0);
	}

	public long readUvlieN()
	{
		int i, j, k, l, m, n;
		
		//0xxx-xxxx	(0-127)
		//10xx-xxxx (Invalid First Char)
		//110x-xxxx 10xx-xxxx (0-2047)
		//1110-xxxx 10xx-xxxx 10xx-xxxx (0-65535)
		
		i=getub_npp();
		if(i<0x80)
			return((long)i);
		if(i<0xC0)
			return((long)(-i));
		if(i<0xE0)
		{
			j=getub_npp();
			return((long)(((i&31)<<8)|j));
		}
		if(i<0xF0)
		{
			j=getub_npp(); k=getub_npp();
			return((long)(((i&15)<<16)|(j<<8)|k));
		}
		if(i<0xF8)
		{
			j=getub_npp(); k=getub_npp(); l=getub_npp();
			return((long)(((i&7)<<24)|(j<<16)|(k<<8)|l));
		}
		if(i<0xFC)
		{
			j=getub_npp(); k=getub_npp(); l=getub_npp(); m=getub_npp();
			return((((long)(i&3))<<32)|(((long)j)<<24)|
				(((long)k)<<16)|(((long)l)<<8)|((long)m));
		}
		if(i<0xFE)
		{
			j=getub_npp(); k=getub_npp(); l=getub_npp();
			m=getub_npp(); n=getub_npp();
			return((((long)(i&1))<<40)|(((long)j)<<32)|
				(((long)k)<<24)|(((long)l)<<16)|
				(((long)m)<<8)|((long)n));
		}
		
		return(0L);
	}

	public long readSvlieN()
	{
		long t, v;
		t=readUvlieN();
		if(t<0)
			{ return 0; }
		v=(t>>1)^((t<<63)>>63);
		return v;
	}

	public String readStringUtf8N()
	{
		char[] buf;
		int i, j, k, n;
	
		buf=new char[256]; n=0;		
		while(getub(0)!=0)
			{ buf[n++]=readUtf8CharN(); }
		ninc();
		return(new String(buf, 0, n));
	}
	
	public int getAddr()
		{ return bData; }
	public void setAddr(int addr)
		{ bData=addr; }

	byte[] data;
	int szData;
	int bData;
}
