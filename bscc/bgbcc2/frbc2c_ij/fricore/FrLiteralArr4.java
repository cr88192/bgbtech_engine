package fricore;

public class FrLiteralArr4 extends FrLiteral
{
	public FrLiteralArr4(FrVMImage img,
		FrBytePointer cs, FrBytePointer cse)
	{
		FrBytePointer ct;
		int a, n;
		int i, j, k;
			
		this.img=img;
		
		n=cse.sub(cs)/4;
		a=img.tryAllocData(n*4);
		this.addr=img.vaData+a;
			
		ct=new FrBytePointer(img.sec_data, a, img.mszData);

		for(i=0; i<n; i++)
		{
			j=cs.getsi(i);
			ct.seti_le(i, j);
		}
	}
}
