package fricore;

/** Bytecoded Function */
public class FrGlobalVar extends FrGlobal
{
	public FrGlobalVar(FrVMImage img,
		FrBytePointer data, int szData)
	{
		FrBytePointer ip, ipe;
		int tag, vtag;
		int i, j, k;
		
		this.img=img;
		valueIdx=-1;
		
		ip=data; ipe=data.add(szData);
		while(ip.lt(ipe))
		{
			i=ip.getub_npp();
			switch(i)
			{
			case FrVMImage.FR2C_TAG_NAME:
				name=img.decodeStringTag(ip);
				break;
			case FrVMImage.FR2C_TAG_SIG:
				sig=img.decodeStringTag(ip);
				break;
			case FrVMImage.FR2C_TAG_FLAGS:
				flags=img.decodeStringTag(ip);
				break;
			case FrVMImage.FR2C_TAG_VALUE:
				valueIdx=(int)img.decodeIntegerTag(ip);
				break;

			case FrVMImage.FR2C_TAG_IMGDATA_L24:
				k=ip.getuw24_npp()-4;
				ip.nadd(k);
				break;
			case FrVMImage.FR2C_TAG_IMGDATA_L8:
				k=ip.getub_npp()-2;
				ip.nadd(k);
				break;

			case FrVMImage.FR2C_TAG_TWOCC_L24:
				k=ip.getuw24_npp()-6;
				tag=ip.getus_npp();
				ip.nadd(k);
				break;
			case FrVMImage.FR2C_TAG_FOURCC_L24:
				k=ip.getuw24_npp()-8;
				tag=ip.getui_npp();
				ip.nadd(k);
				break;
			case FrVMImage.FR2C_TAG_TWOCC_L8:
				k=ip.getub_npp()-4;
				tag=ip.getus_npp();
				ip.nadd(k);
				break;
			}
		}
		
		if(valueIdx>=0)
		{
			switch(sig.charAt(0))
			{
			case 'i':	case 'j':
			case 'c':	case 'h':
			case 's':	case 't':
			case 'w':
				valuei=(long)img.getLiteralInt(valueIdx);
				break;
			case 'l':	case 'm':
			case 'x':	case 'y':
				valuei=img.getLiteralLong(valueIdx);
				break;
			case 'f':
				valuef=img.getLiteralFloat(valueIdx);
				break;
			case 'd':
				valuef=img.getLiteralDouble(valueIdx);
				break;
			case 'P':
				valuei=img.getLiteralAddr(valueIdx);
				break;
			}
		}
	}
	
	public int getValuei(FrVMContext ctx)
		{ return (int)valuei; }
	public long getValuel(FrVMContext ctx)
		{ return valuei; }
	public float getValuef(FrVMContext ctx)
		{ return (float)valuef; }
	public double getValued(FrVMContext ctx)
		{ return valuef; }
	public FrPointer getValuep(FrVMContext ctx)
	{
		return ctx.space.addrToPointer(valuei);
//		return valuep;
	}
	
	public int valueIdx;
	
	public long valuei;
	public double valuef;
//	public FrPointer valuep;
}