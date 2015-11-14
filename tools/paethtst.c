#include <stdio.h>

#if 1
int PDPNG_Paeth0(int a, int b, int c)
{
	int p, pa, pb, pc;

	p=a+b-c;
//	pa=(p>a)?(p-a):(a-p);
//	pb=(p>b)?(p-b):(b-p);
//	pc=(p>c)?(p-c):(c-p);

	pa=p-a; pa=pa*pa;
	pb=p-b; pb=pb*pb;
	pc=p-c; pc=pc*pc;

	p=(pa<=pb)?((pa<=pc)?a:c):((pb<=pc)?b:c);
	return(p);
}
#endif

#if 0
int PDPNG_Paeth1(int a, int b, int c)
{
	int p, pa, pb, pc;
	int ma, mb, mc;

	p=a+b-c;
//	pa=(p>a)?(p-a):(a-p);
//	pb=(p>b)?(p-b):(b-p);
//	pc=(p>c)?(p-c):(c-p);

	pa=p-a; pa=pa*pa;
	pb=p-b; pb=pb*pb;
	pc=p-c; pc=pc*pc;

	ma=-((pb-pa)>>24);
	mb=-((pc-pb)>>24);
	mc=-((pc-pa)>>24);

//	p=(1-ma)*((1-mb)*c+mb*b) + ma*((1-mc)*c + mc*a);
	p=ma*(mb*c+(1-mb)*b) + (1-ma)*(mc*c + (1-mc)*a);

//	p=(pb>pa)?((pc>pb)?c:b):((pc>pa)?c:a);

//	p=(pa<=pb)?((pa<=pc)?a:c):((pb<=pc)?b:c);
	return(p);
}
#endif

#if 1
int PDPNG_Paeth1(int a, int b, int c)
{
	int p, pa, pb, pc;
	int ma, mb, mc;

	p=a+b-c;
//	pa=(p>a)?(p-a):(a-p);
//	pb=(p>b)?(p-b):(b-p);
//	pc=(p>c)?(p-c):(c-p);

	pa=p-a; pa=pa*pa;
	pb=p-b; pb=pb*pb;
	pc=p-c; pc=pc*pc;

	ma=((pb-pa)>>24);
	mb=((pc-pb)>>24);
	mc=((pc-pa)>>24);

//	p=(1-ma)*((1-mb)*c+mb*b) + ma*((1-mc)*c + mc*a);
	p=(ma&((mb&c)|((~mb)&b))) | ((~ma)&((mc&c)|((~mc)&a)));

//	p=(pb>pa)?((pc>pb)?c:b):((pc>pa)?c:a);

//	p=(pa<=pb)?((pa<=pc)?a:c):((pb<=pc)?b:c);
	return(p);
}
#endif


int main()
{
	int a, b, c;
	int a1, b1;
	int p0, p1, p2, p3;
	
	for(a=0; a<255; a++)
		for(b=0; b<255; b++)
			for(c=0; c<255; c++)
	{
		p0=PDPNG_Paeth0(a, b, c);
		p1=PDPNG_Paeth1(a, b, c);
		if(p0!=p1)
		{
			printf("%d %d %d: %d %d\n", a, b, c, p0, p1);
		}
	}

	for(a=0; a<255; a++)
		for(b=0; b<255; b++)
	{
//		p0=(a+b)/2;
		p0=(a+b+1)/2;
		p1=a-b;

//		p0&=255; p1&=255;

//		p1=(p1+128)&255;
//		p1-=128;

		b1=(2*p0-p1+1)/2;
		a1=p1+b1;

		p2=p0;
		p3=p1;
//		p3=-((-p1)&255);

//		b1=(2*p2-p3+1)/2;
		b1=(2*p2-p3)/2;
		a1=p3+b1;
		
		a1&=255; b1&=255;
		
		if((a!=a1) || (b!=b1))
		{
			printf("%d %d -> %d %d -> %d %d -> %d %d\n", a, b,
				p0, p1, p2, p3, a1, b1);
		}
	}
}
