package fricore;

/** Literal Table Entry */
public class FrLiteral
{
	public static FrLiteral createArr4(FrVMImage img,
		FrBytePointer cs, FrBytePointer cse)
	{
		return new FrLiteralArr4(img, cs, cse);
	}

	FrVMImage img;
	public long addr;
}
