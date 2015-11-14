package fricore;

public class FrLiteralArr1 extends FrLiteral
{
	public FrLiteralArr1(FrVMImage img,
		FrBytePointer cs, FrBytePointer cse)
	{
		FrBytePointer ct;
		int a, n;
		int i, j, k;
			
		this.img=img;
		
		n=cse.sub(cs);
		a=img.tryAllocData(n);
		this.addr=img.vaData+a;
			
		ct=new FrBytePointer(img.sec_data, a, img.mszData);

		for(i=0; i<n; i++)
		{
			j=cs.getsb(i);
			ct.setb(i, j);
		}
	}
}
