package fricore;

import java.lang.*;

public class FrVMImage
{
	public FrVMImage(byte[] image, int base, int szImage)
	{
		decodeImage(new FrBytePointer(
			image, base, base+szImage));
	}

	public int decodeImage(
			byte[] image, int base, int szImage)
	{
		return decodeImage(new FrBytePointer(
			image, base, base+szImage));
	}

	public int decodeImage(FrBytePointer cs)
	{
		FrBytePointer css, cse;
		int tag;
		int i, j, k;
			
		css=cs.add(0);
		i=cs.getui_npp();
		j=cs.getui_npp();
		if(i!=FR2C_FOURCC_FRBC)
			return(-1);
		if(i!=FR2C_FOURCC_2C00)
			return(-1);
		k=cs.getsi_npp();
		if(k<12)
			return(-1);
		cse=css.add(k);

		while(cs.lt(cse))
		{
			i=cs.getub_npp();
			switch(i)
			{
			case FrVMImage.FR2C_TAG_TWOCC_L24:
				k=cs.getuw24_npp()-6;
				tag=cs.getus_npp();
				decodeTopTwoCC(tag, cs.add(0), cs.add(k));
				cs.nadd(k);
				break;
			case FrVMImage.FR2C_TAG_FOURCC_L24:
				k=cs.getuw24_npp()-8;
				tag=cs.getui_npp();
				decodeTopFourCC(tag, cs.add(0), cs.add(k));
				cs.nadd(k);
				break;
			case FrVMImage.FR2C_TAG_TWOCC_L8:
				k=cs.getub_npp()-4;
				tag=cs.getus_npp();
				decodeTopTwoCC(tag, cs.add(0), cs.add(k));
				cs.nadd(k);
				break;
			default:
				return(-1);
			}
		}
		
		for(i=0; i<nLidx; i++)
		{
			getLiteralObj(i);
		}
		
		return(0);
	}

	public void decodeTopTwoCC(
		int tcc, FrBytePointer cs, FrBytePointer cse)
	{
	}

	public void decodeTopFourCC(
		int fcc, FrBytePointer cs, FrBytePointer cse)
	{
		int i, j, k, n;
		switch(fcc)
		{
		case FR2C_FOURCC_LVT4:
			n=cse.sub(cs)/4;
			lvt4=new int[n];
			for(i=0; i<n; i++)
				{ lvt4[i]=cs.getui_npp(); }
			nLvt4=n;
			break;
		case FR2C_FOURCC_LVT8:
			n=cse.sub(cs)/8;
			lvt8=new long[n];
			for(i=0; i<n; i++)
				{ lvt8[i]=cs.getul_npp(); }
			nLvt8=n;
			break;
		case FR2C_FOURCC_STRS:
			n=cse.sub(cs);
			strs=new byte[n];
			szStrs=n;
			for(i=0; i<n; i++)
				{ strs[i]=(byte)cs.getsb(i); }
			vaStrs=vmi_addr_rov;
			vmi_addr_rov+=(n+4095)&(~4095);
			vaWStrs=vmi_addr_rov;
			vmi_addr_rov+=(2*n+4095)&(~4095);
			break;

		case FR2C_FOURCC_LDEF:
			n=cse.sub(cs);
			ldef=new byte[n];
			szLdef=n;
			for(i=0; i<n; i++)
				{ ldef[i]=(byte)cs.getsb(i); }
			break;
		case FR2C_FOURCC_GDEF:
			n=cse.sub(cs);
			gdef=new byte[n];
			szGdef=n;
			for(i=0; i<n; i++)
				{ gdef[i]=(byte)cs.getsb(i); }
			break;

		case FR2C_FOURCC_LIDX:
			n=cse.sub(cs)/4;
			lidx=new int[n];
			for(i=0; i<n; i++)
				{ lidx[i]=cs.getsi_npp(); }
			nLidx=n;
			literals=new FrLiteral[n];
			break;
		case FR2C_FOURCC_GIDX:
			n=cse.sub(cs)/4;
			gidx=new int[n];
			for(i=0; i<n; i++)
				{ gidx[i]=cs.getsi_npp(); }
			nGidx=n;
			globals=new FrGlobal[n];
			break;
		}
	}

	public String decodeStringTag(FrBytePointer rcs)
	{
		//char[] buf;
		FrBytePointer cs2;
		int i, j, k;
	
		i=rcs.getub_npp(); rcs.ninc();
		switch(i)
		{
		case FR2C_TAG_STRING:
		case FR2C_TAG_KEYWORD:
			return rcs.readStringUtf8N();
		case FR2C_TAG_STRINGREF:
		case FR2C_TAG_KEYWORDREF:
			i=(int)(rcs.readUvlieN());
			cs2=new FrBytePointer(strs, i, szStrs);
			return cs2.readStringUtf8N();
		default:
			return null;
		}
	}

	public long decodeIntegerTag(FrBytePointer rcs)
	{
		FrBytePointer cs2;
		long l;
		int i, j, k;
	
		i=rcs.getub_npp(); rcs.ninc();
		switch(i)
		{
		case FR2C_TAG_INT:
			l=rcs.readSvlieN();
			return l;
		default:
			return 0;
		}
	}

	public int getLiteralInt(int idx)
		{ return(lvt4[idx]); }
	public long getLiteralLong(int idx)
		{ return(lvt8[idx]); }
	public float getLiteralFloat(int idx)
		{ return(Float.intBitsToFloat(lvt4[idx])); }
	public double getLiteralDouble(int idx)
		{ return(Double.longBitsToDouble(lvt8[idx])); }

	public FrPointer getLiteralCharPointer(int idx)
	{
		return null;
	}

	public long getLiteralCharPtrAddr(int idx)
	{
		return vaStrs+idx;
	}

	public long getLiteralAddr(int idx)
	{
		int tag;
		int k;

		tag=0; k=idx;
		while((k&1)!=0)
			{ tag++; k=k>>1; }
		k=k>>1;
		tag=(tag<<1)|(k&1);
		k=k>>1;

		switch(tag)
		{
		case 0:
			return getLiteralObj(k).addr;
		case 1:
			return getLiteralCharPtrAddr(k);
		default:
			return 0;
		}
	}

	public long expandConst12Int(int idx)
	{
		if((idx&2048)!=0)
			idx|=(-1)<<12;
		return((long)idx);
//		return(lvt4[idx]);
	}

	public double expandConst12Float(int idx)
	{
		if((idx&2048)!=0)
			idx|=(-1)<<12;
		return((double)idx);
//		return(lvt4[idx]);
	}

	public FrGlobal getGlobal(int idx)
	{
		if(globals[idx]==null)
		{
			globals[idx]=decodeGlobal(new FrBytePointer(
				gdef, gidx[idx], szGdef));
		}
		return globals[idx];
	}

	public FrGlobal decodeGlobal(FrBytePointer cs)
	{
		FrGlobal tmp;
		int tag;
		int i, j, k;

		i=cs.getub_npp();
		switch(i)
		{
		case FrVMImage.FR2C_TAG_TWOCC_L24:
			k=cs.getuw24_npp()-6;
			tag=cs.getus_npp();
			tmp=decodeGlobalTwoCC(tag, cs.add(0), cs.add(k));
			cs.nadd(k);
			return tmp;
		case FrVMImage.FR2C_TAG_FOURCC_L24:
			k=cs.getuw24_npp()-8;
			tag=cs.getui_npp();
			tmp=decodeGlobalFourCC(tag, cs.add(0), cs.add(k));
			cs.nadd(k);
			return tmp;
		case FrVMImage.FR2C_TAG_TWOCC_L8:
			k=cs.getub_npp()-4;
			tag=cs.getus_npp();
			tmp=decodeGlobalTwoCC(tag, cs.add(0), cs.add(k));
			cs.nadd(k);
			return tmp;
		default:
			return null;
		}
	}

	public FrGlobal decodeGlobalTwoCC(
		int tcc, FrBytePointer cs, FrBytePointer cse)
	{
		return null;
	}

	public FrGlobal decodeGlobalFourCC(
		int fcc, FrBytePointer cs, FrBytePointer cse)
	{
		int i, j, k, n;
		
		switch(fcc)
		{
		case FR2C_FOURCC_FUNC:
			return new FrFunction(this, cs, cse.sub(cs));
		case FR2C_FOURCC_DVAR:
			return new FrFunction(this, cs, cse.sub(cs));
		default:
			return null;
		}
	}

	public FrLiteral getLiteralObj(int idx)
	{
		if(literals[idx]==null)
		{
			literals[idx]=decodeLiteralObj(new FrBytePointer(
				ldef, lidx[idx], szLdef));
		}
		return literals[idx];
	}

	public FrLiteral decodeLiteralObj(FrBytePointer cs)
	{
		FrLiteral tmp;
		int tag;
		int i, j, k;

		i=cs.getub_npp();
		switch(i)
		{
		case FrVMImage.FR2C_TAG_TWOCC_L24:
			k=cs.getuw24_npp()-6;
			tag=cs.getus_npp();
			tmp=decodeLiteralObjTwoCC(tag, cs.add(0), cs.add(k));
			cs.nadd(k);
			return tmp;
		case FrVMImage.FR2C_TAG_FOURCC_L24:
			k=cs.getuw24_npp()-8;
			tag=cs.getui_npp();
			tmp=decodeLiteralObjFourCC(tag, cs.add(0), cs.add(k));
			cs.nadd(k);
			return tmp;
		case FrVMImage.FR2C_TAG_TWOCC_L8:
			k=cs.getub_npp()-4;
			tag=cs.getus_npp();
			tmp=decodeLiteralObjTwoCC(tag, cs.add(0), cs.add(k));
			cs.nadd(k);
			return tmp;
		default:
			return null;
		}
	}

	public FrLiteral decodeLiteralObjTwoCC(
		int tcc, FrBytePointer cs, FrBytePointer cse)
	{
		return null;
	}

	public FrLiteral decodeLiteralObjFourCC(
		int fcc, FrBytePointer cs, FrBytePointer cse)
	{
		int i, j, k, n;
		
		switch(fcc)
		{
		case FR2C_FOURCC_ARR1:
			return new FrLiteralArr1(this, cs, cse);
		case FR2C_FOURCC_ARR2:
			return new FrLiteralArr2(this, cs, cse);
		case FR2C_FOURCC_ARR4:
			return new FrLiteralArr4(this, cs, cse);
		case FR2C_FOURCC_ARR8:
			return new FrLiteralArr8(this, cs, cse);
//		case FR2C_FOURCC_FUNC:
//			return new FrFunction(this, cs, cse.sub(cs));
//		case FR2C_FOURCC_DVAR:
//			return new FrFunction(this, cs, cse.sub(cs));
		default:
			return null;
		}
	}

	public FrPointer addrToPointer(FrMemSpace space, long addr)
	{
		int i, j, k;
		if((addr>=vaStrs) && (addr<(vaStrs+szStrs)))
		{
			return new FrPointerImgBytes(
				space, this, addr, strs, vaStrs, vaStrs+szStrs);
		}
		if((addr>=vaWStrs) && (addr<(vaWStrs+2*szStrs)))
		{
			if(wstrs==null)
			{
				//FIXME: make not stupid.
				wstrs=new byte[2*szStrs];
				for(i=0; i<szStrs; i++)
				{
					wstrs[i*2+0]=strs[i];
					wstrs[i*2+1]=(byte)0;
				}
			}
		
			return new FrPointerImgBytes(
				space, this, addr, wstrs, vaWStrs, vaWStrs+2*szStrs);
		}
		return null;
	}

	public int tryAllocData(int sz)
	{
		byte[] tdata;
		int a;
		int i, j, k;
		
		if(sec_data==null)
		{
			vaData=vmi_addr_rov;
			vmi_addr_rov+=1<<26;
		
			i=262144;
			while(sz>=i)
				i=i+(i>>1);
			sec_data=new byte[i];
			mszData=i;
		}
		
		if((szData+sz)>mszData)
		{
			i=mszData;
			while((szData+sz)>=i)
				i=i+(i>>1);
			tdata=new byte[i];
			for(i=0; i<szData; i++)
				tdata[i]=sec_data[i];
			sec_data=tdata;
			mszData=i;
		}
		
		a=szData;
		szData+=(sz+15)&(~15);
		return a;
	}

	public long vaStart;	//starting VA
	public long vaEnd;		//ending VA

	public int[] lvt4;
	public int nLvt4;

	public long[] lvt8;
	public int nLvt8;

	public byte[] strs;
	public int szStrs;
	public long vaStrs;

	public byte[] wstrs;
	public long vaWStrs;

	public int[] lidx;
	public int nLidx;
	FrLiteral[] literals;

	public int[] gidx;
	public int nGidx;
	FrGlobal[] globals;
//	int nGlobals;

	public byte[] ldef;
	public int szLdef;

	public byte[] gdef;
	public int szGdef;


	public byte[] sec_data;
	public int szData, mszData;
	public long vaData;

	static long vmi_addr_rov=FrMemSpace.VABASE_IMAGE;

//	FrPointerRgn rgn_strs;

	public static final int FR2C_TAG_EOL			=0x00;
	public static final int FR2C_TAG_BOOL			=0x01;
	public static final int FR2C_TAG_INT			=0x02;
	public static final int FR2C_TAG_BITARRAY		=0x03;
	public static final int FR2C_TAG_BYTEARRAY	=0x04;
	public static final int FR2C_TAG_NULL			=0x05;
	public static final int FR2C_TAG_UUID			=0x06;

	public static final int FR2C_TAG_FLOAT		=0x09;
	public static final int FR2C_TAG_STRING		=0x12;
	public static final int FR2C_TAG_KEYWORD		=0x13;
	public static final int FR2C_TAG_BEGIN_NODE	=0x14;

	public static final int FR2C_TAG_STRINGREF	=0x21;
	public static final int FR2C_TAG_KEYWORDREF	=0x22;

	public static final int FR2C_TAG_NAME			=0x81;
	public static final int FR2C_TAG_SIG			=0x82;
	public static final int FR2C_TAG_FLAGS		=0x83;
	public static final int FR2C_TAG_NLOCALS		=0x84;
	public static final int FR2C_TAG_NREGS		=0x85;
	public static final int FR2C_TAG_NARGS		=0x86;
	public static final int FR2C_TAG_NCARGS		=0x87;
	public static final int FR2C_TAG_VALUE		=0x88;
	public static final int FR2C_TAG_LIBNAME		=0x89;

	public static final int FR2C_TAG_EOD			=0xE0;
	public static final int FR2C_TAG_IMGDATA_L24	=0xE1;
	public static final int FR2C_TAG_IMGDATA_L8	=0xE2;
	public static final int FR2C_TAG_TWOCC_L24	=0xE3;
	public static final int FR2C_TAG_FOURCC_L24	=0xE4;
	public static final int FR2C_TAG_TWOCC_L8		=0xE5;
	public static final int FR2C_TAG_FOURCC_L56	=0xE6;
	public static final int FR2C_TAG_EIGHTCC_L24	=0xE7;

	public static final int FR2C_TWOCC_BC			=0x4243;

	public static final int FR2C_FOURCC_FRBC		=0x46524243;
	public static final int FR2C_FOURCC_2C00		=0x32433030;
	public static final int FR2C_FOURCC_LVT4		=0x4C565434;
	public static final int FR2C_FOURCC_LVT8		=0x4C565438;
	public static final int FR2C_FOURCC_STRS		=0x53545253;
	public static final int FR2C_FOURCC_LIDX		=0x4C494458;
	public static final int FR2C_FOURCC_GIDX		=0x47494458;
	public static final int FR2C_FOURCC_LDEF		=0x4C444546;
	public static final int FR2C_FOURCC_GDEF		=0x47444546;
	public static final int FR2C_FOURCC_FUNC		=0x46554E43;
	public static final int FR2C_FOURCC_DVAR		=0x44564152;
	public static final int FR2C_FOURCC_ARR1		=0x41525231;
	public static final int FR2C_FOURCC_ARR2		=0x41525232;
	public static final int FR2C_FOURCC_ARR4		=0x41525234;
	public static final int FR2C_FOURCC_ARR8		=0x41525238;
}
