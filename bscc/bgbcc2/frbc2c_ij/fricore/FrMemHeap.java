package fricore;

public class FrMemHeap extends FrMemRegion
{
	public FrMemHeap(FrMemSpace space, long addr, int sz)
	{
		super(space, addr, sz);
		map=new byte[sz>>4];
		szMap=sz>>4;
	}
	
	public FrPointer malloc(int sz)
	{
		int nc;
		int i, j, k;

		nc=(sz+15)>>4; i=0;
		while(i<szMap)
		{
			for(j=0; j<nc; j++)
				if(map[i+j]!=0)
					break;
			if(j<nc)
			{
				i+=j;
				while(i<szMap)
				{
					if(map[i]==0)
						break;
					i++;
				}
				continue;
			}

			map[i]=1;
			for(j=0; j<nc; j++)
				map[i+j]=2;
			return new FrPointerRgn(this, i<<4);
		}
		return null;
	}

	public boolean free(FrPointer ptr)
	{
		int a;
		int i, j, k;
		
		a=((FrPointerRgn)ptr).base;
		i=a>>4;
		if(map[i]!=1)
			return false;
		
		map[i]=0;
		for(j=1; map[i+j]==2; j++)
			map[i+j]=0;
		return true;
	}
	
	byte[] map;
	int szMap;
}