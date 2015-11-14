package fricore;

public class FrLiteralArr8 extends FrLiteral
{
	public FrLiteralArr8(FrVMImage img,
		FrBytePointer cs, FrBytePointer cse)
	{
		FrBytePointer ct;
		long li;
		int a, n;
		int i, j, k;
			
		this.img=img;
		
		n=cse.sub(cs)/8;
		a=img.tryAllocData(n*8);
		this.addr=img.vaData+a;
			
		ct=new FrBytePointer(img.sec_data, a, img.mszData);

		for(i=0; i<n; i++)
		{
			li=cs.getsl(i);
			ct.setl_le(i, li);
		}
	}
}
