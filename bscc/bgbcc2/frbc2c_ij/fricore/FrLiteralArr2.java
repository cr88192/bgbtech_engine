package fricore;

public class FrLiteralArr2 extends FrLiteral
{
	public FrLiteralArr2(FrVMImage img,
		FrBytePointer cs, FrBytePointer cse)
	{
		FrBytePointer ct;
		int a, n;
		int i, j, k;
			
		this.img=img;
		
		n=cse.sub(cs)/2;
		a=img.tryAllocData(n*2);
		this.addr=img.vaData+a;
			
		ct=new FrBytePointer(img.sec_data, a, img.mszData);

		for(i=0; i<n; i++)
		{
			j=cs.getss(i);
			ct.sets_le(i, j);
		}
	}
}
