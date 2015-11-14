package fricore;

/** Bytecoded Function */
public class FrFunction extends FrGlobal
{
	public FrFunction(FrVMImage img,
		FrBytePointer func, int szFunc)
	{
		FrBytePointer ip, ipe;
		int tag;
		int i, j, k;
		
		this.img=img;
		
		ip=func; ipe=func.add(szFunc);
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
			case FrVMImage.FR2C_TAG_NLOCALS:
				nLocals=(int)img.decodeIntegerTag(ip);
				break;
			case FrVMImage.FR2C_TAG_NREGS:
				nRegs=(int)img.decodeIntegerTag(ip);
				break;
			case FrVMImage.FR2C_TAG_NARGS:
				nArgs=(int)img.decodeIntegerTag(ip);
				break;
			case FrVMImage.FR2C_TAG_NCARGS:
				nCArgs=(int)img.decodeIntegerTag(ip);
				break;

			case FrVMImage.FR2C_TAG_IMGDATA_L24:
				k=ip.getuw24_npp()-4;
				code=new byte[k];
				for(j=0; j<k; j++)
					{ code[j]=(byte)ip.getub(j); }
				ip.nadd(k);
				break;
			case FrVMImage.FR2C_TAG_IMGDATA_L8:
				k=ip.getub_npp()-2;
				code=new byte[k];
				for(j=0; j<k; j++)
					{ code[j]=(byte)ip.getub(j); }
				ip.nadd(k);
				break;

			case FrVMImage.FR2C_TAG_TWOCC_L24:
				k=ip.getuw24_npp()-6;
				tag=ip.getus_npp();
				switch(tag)
				{
				case FrVMImage.FR2C_TWOCC_BC:
					code=new byte[k];
					for(j=0; j<k; j++)
						{ code[j]=(byte)ip.getub(j); }
					ip.nadd(k);
				default:
					ip.nadd(k);
					break;
				}
				break;
			case FrVMImage.FR2C_TAG_FOURCC_L24:
				k=ip.getuw24_npp()-8;
				tag=ip.getui_npp();
				switch(tag)
				{
				default:
					ip.nadd(k);
					break;
				}
				break;
			case FrVMImage.FR2C_TAG_TWOCC_L8:
				k=ip.getub_npp()-4;
				tag=ip.getus_npp();
				switch(tag)
				{
				case FrVMImage.FR2C_TWOCC_BC:
					code=new byte[k];
					for(j=0; j<k; j++)
						{ code[j]=(byte)ip.getub(j); }
					ip.nadd(k);
				default:
					ip.nadd(k);
					break;
				}
				break;
			}
		}
	}

	public FrTrace decodeTrace(FrBytePointer cs, FrBytePointer cse)
	{
		FrOpcode[] ops;
		FrOpcode op;
		FrTailOpcode top;
		FrTrace tr;
		int a;
		int n;

		if(cs.ge(cse))
			return null;

		a=cs.bData;

		ops=new FrOpcode[256]; n=0;
		while(cs.lt(cse))
		{
			op=FrOpcode.decodeOpcode(img, cs, cse);
			if(op==null)
				{ break; }
			ops[n++]=op;
		}
		top=FrTailOpcode.decodeTailOpcode(img, cs, cse);
		
		if(top==null)
			{ throw new FrDecodeFailException(); }
		
		tr=FrTrace.create(ops, n, top);

		tr.func=this;
		tr.addr=a;
		top.trace=tr;
		tr.jmpaddr=top.jmpaddr;

		return tr;
	}

	public FrTrace decode()
	{
		FrBytePointer cs, cse;
		FrTrace tr;
		int i, j, k;

		if(traces!=null)
			return traces[0];

		traces=new FrTrace[256];
		nTraces=0;
		
		cs=new FrBytePointer(code, 0, szCode);
		cse=cs.add(szCode);

		while(cs.lt(cse))
		{
			tr=decodeTrace(cs, cse);
			if(tr==null)
				break;
			traces[nTraces++]=tr;
		}
		
		for(i=0; i<(nTraces-1); i++)
		{
			tr=traces[i];
			tr.next=traces[i+1];
			if(tr.jmpaddr>=0)
				{ tr.jmpnext=findTrace(tr.jmpaddr); }
		}
		
		return traces[0];
	}

	public FrTrace findTrace(int addr)
	{
		int i;
		
		for(i=0; i<nTraces; i++)
			if(traces[i].addr==addr)
				return(traces[i]);
		return null;
	}

	public FrVMFrame createFrame(FrVMContext ctx)
	{
		return new FrVMFrame(ctx, nRegs, nArgs, nLocals);
	}
	
//	FrVMImage img;
	
	byte[] code;
	int szCode;
	
	FrTrace[] traces;
	int nTraces;
	
//	public String name;
//	public String sig;
//	public String flags;

	public int nLocals;
	public int nRegs;
	public int nArgs;
	public int nCArgs;
}
