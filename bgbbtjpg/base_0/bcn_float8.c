#include <bgbbtj.h>

byte bgbbtj_float8_basetable[512];
byte bgbbtj_float8_shifttable[512];

float bgbbtj_float8_base[256];

BGBBTJ_API int BGBBTJ_Float8_Init()
{
	static int init=0;
	int i, j, k, e;

	if(init)return(0);
	init=1;

	BGBBTJ_Float16_Init();

//	for(i=0; i<1024; i++)
//		{ bgbbtj_float8_mantissatable[i]=BGBBTJ_Float16_ConvertMantissa(i); }
//	for(i=1024; i<2048; i++)
//		{ bgbbtj_float8_mantissatable[i]=0x38000000U+((i-1024)<<13); }
//	for(i=0; i<31; i++)
//		{ bgbbtj_float8_exponenttable[i]=0x00000000U+(i<<23); }
//	for(i=32; i<63; i++)
//		{ bgbbtj_float8_exponenttable[i]=0x80000000U+((i-32)<<23); }

//	bgbbtj_float16_exponenttable[31]=0x47800000U;
//	bgbbtj_float16_exponenttable[63]=0xC7800000U;
//	bgbbtj_float16_exponenttable[31]=0x7F800000U;	//+ NaN/Inf
//	bgbbtj_float16_exponenttable[63]=0xFF800000U;	//- NaN/Inf

//	for(i=1; i<32; i++)
//	{
//		bgbbtj_float16_offsettable[i]=1024;
//		bgbbtj_float16_offsettable[i+32]=1024;
//	}

//	bgbbtj_float16_offsettable[0]=0;
//	bgbbtj_float16_offsettable[32]=0;

	for(i=0; i<128; i++)
	{
//		j=((i-0x40)<<7)+0x3C00;
		j=((i-0x38)<<7)+0x3C00;
		k=j|0x8000;
		bgbbtj_float8_base[0x00|i]=BGBBTJ_DecodeFloat16(j);
		bgbbtj_float8_base[0x80|i]=BGBBTJ_DecodeFloat16(k);
	}

	for(i=0; i<256; i++)
	{
		e=i-127;
		if(e<-10)	//zeroes
		{
			bgbbtj_float8_basetable[i|0x000]=0x00;	//-0
			bgbbtj_float8_basetable[i|0x100]=0x80;	//+0
			bgbbtj_float8_shifttable[i|0x000]=24;
			bgbbtj_float8_shifttable[i|0x100]=24;
		}
		else if(e<-6)	//denormals
		{
			bgbbtj_float8_basetable[i|0x000]=(0x0400>>(-e-14))|0x00;
			bgbbtj_float8_basetable[i|0x100]=(0x0400>>(-e-14))|0x80;
			bgbbtj_float8_shifttable[i|0x000]=-e-1;
			bgbbtj_float8_shifttable[i|0x100]=-e-1;
		}
		else if(e<=7) //normal range
		{
			bgbbtj_float8_basetable[i|0x000]=((e+7)<<3)|0x00;
			bgbbtj_float8_basetable[i|0x100]=((e+7)<<3)|0x80;
			bgbbtj_float8_shifttable[i|0x000]=20;
			bgbbtj_float8_shifttable[i|0x100]=20;
		}
		else if(e<128)	//overflow to Inf
		{
			bgbbtj_float8_basetable[i|0x000]=0x78;
			bgbbtj_float8_basetable[i|0x100]=0xF8;
			bgbbtj_float8_shifttable[i|0x000]=24;
			bgbbtj_float8_shifttable[i|0x100]=24;
		}
		else	//Inf and NaN
		{
			bgbbtj_float8_basetable[i|0x000]=0x78;
			bgbbtj_float8_basetable[i|0x100]=0xF8;
			bgbbtj_float8_shifttable[i|0x000]=20;
			bgbbtj_float8_shifttable[i|0x100]=20;
		}
	}
	
	return(1);
}

BGBBTJ_API float BGBBTJ_DecodeFloat8(byte val)
{
	return(bgbbtj_float8_base[val]);
}

BGBBTJ_API byte BGBBTJ_EncodeFloat8(double val)
{
	u16 t;
	u32 f;
	int i;
	
	*(float *)(&f)=val;
	i=(f>>23)&0x1FF;

	t=bgbbtj_float8_basetable[i]+
		((f&0x007FFFFF)>>bgbbtj_float8_shifttable[i]);
	return(t);
}


BGBBTJ_API byte BGBBTJ_NegFloat8(byte x)
{
	return(x^0x80);
}

BGBBTJ_API byte BGBBTJ_RcpFloat8(byte x)
{
	int z;
//	z=0x3C00-(x&0x7FFF);
	z=0x70-(x&0x7F);
	if(z<0)return(0);
	z=(x&0x80)|z;
	return(z);
}

BGBBTJ_API byte BGBBTJ_SqrtFloat8(byte x)
{
	int z;
	z=((x-0x38)>>1)+0x38;
	return(z);
}

BGBBTJ_API byte BGBBTJ_AddFloat8(byte x, byte y)
{
	int ux, uy, z, k, d;
	if((x^y)&0x80)
	{
		ux=x&0x7F;
		uy=y&0x7F;
		if(ux>=uy)
		{
			d=(ux>>3)-(uy>>3);
			k=((y&0x07)|0x08)>>d;
			z=x-k;
			if((x^z)&0x80)
				{ z^=0x7F; }
		}else
		{
			d=(uy>>3)-(ux>>3);
			k=((x&0x07)|0x08)>>d;
			z=y-k;
			if((y^z)&0x80)
				{ z^=0x7F; }
		}
	}else
	{
		if(x>=y)
		{
			d=(x>>3)-(y>>3);
			k=((y&0x07)|0x08)>>d;
			z=x+k;
			if((x^z)&0x80)
				{ z=(x&0x80)|0x78; }
		}else
		{
			d=(y>>3)-(x>>3);
			k=((x&0x07)|0x08)>>d;
			z=y+k;
			if((y^z)&0x80)
				{ z=(x&0x80)|0x78; }
		}
	}
	return(z);
}

BGBBTJ_API byte BGBBTJ_SubFloat8(byte x, byte y)
{
	return(BGBBTJ_AddFloat8(x, y^0x80));
}

BGBBTJ_API byte BGBBTJ_MulFloat8(byte x, byte y)
{
	int ux, uy, z, k, d;

	ux=x&0x7F;
	uy=y&0x7F;
	z=ux+uy-0x38;
	if(z&0x80)
	{
		if(z<0)return(0);
		z=((x^y)&0x80)|0x78;
	}else
	{
		z=((x^y)&0x80)|z;
	}
	return(z);
}

BGBBTJ_API byte BGBBTJ_MulFloat8F(byte x, byte y)
{
	int ux, uy, z, k, d;

	ux=x&0x7F;
	uy=y&0x7F;
	z=ux+uy-0x38;
	z=z&(~((((s32)z)<<24)>>31));
	z=((x^y)&0x80)|z;
	return(z);
}

BGBBTJ_API byte BGBBTJ_DivFloat8(byte x, byte y)
{
	return(BGBBTJ_MulFloat8(x, BGBBTJ_RcpFloat8(y)));
}

