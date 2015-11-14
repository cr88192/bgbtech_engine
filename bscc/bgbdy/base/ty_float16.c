#include <bgbgc.h>
#include <bgbdy.h>

u32 bgbdy_float16_mantissatable[2048];
u32 bgbdy_float16_exponenttable[64];
short bgbdy_float16_offsettable[64];

u16 bgbdy_float16_basetable[512];
byte bgbdy_float16_shifttable[512];

#if 0
u32 bgbdy_float18_mantissatable[4096];
u32 bgbdy_float18_exponenttable[128];
short bgbdy_float18_offsettable[128];

u16 bgbdy_float18_basetable[512];
byte bgbdy_float18_shifttable[512];
#endif

u32 BGBDY_Float16_ConvertMantissa(u32 i)
{
	u32 m;
	u32 e;
	
	if(!i)return(0);
	
	e=0; m=i<<13;
	while(!(m&0x00800000U))
	{
		e-=0x00800000U;
		m<<=1;
	}
	m&=~0x00800000U;
	e+=0x38800000U;
	return(m|e);
}

BGBDY_API int BGBDY_Float16_Init()
{
	static int init=0;
	int i, e;

	if(init)return(0);
	init=1;

	for(i=0; i<1024; i++)
	{
		bgbdy_float16_mantissatable[i]=BGBDY_Float16_ConvertMantissa(i);
	}

	for(i=1024; i<2048; i++)
	{
		bgbdy_float16_mantissatable[i]=0x38000000U+((i-1024)<<13);
	}

	for(i=0; i<31; i++)
	{
		bgbdy_float16_exponenttable[i]=0x00000000U+(i<<23);
	}

	for(i=32; i<63; i++)
	{
		bgbdy_float16_exponenttable[i]=0x80000000U+((i-32)<<23);
	}

//	bgbdy_float16_exponenttable[31]=0x47800000U;
//	bgbdy_float16_exponenttable[63]=0xC7800000U;

	bgbdy_float16_exponenttable[31]=0x7F800000U;	//+ NaN/Inf
	bgbdy_float16_exponenttable[63]=0xFF800000U;	//- NaN/Inf



	for(i=1; i<32; i++)
	{
		bgbdy_float16_offsettable[i]=1024;
	}

	bgbdy_float16_offsettable[0]=0;
	bgbdy_float16_offsettable[32]=0;

	for(i=0; i<256; i++)
	{
		e=i-127;
		if(e<-24)	//zeroes
		{
			bgbdy_float16_basetable[i|0x000]=0x0000;	//-0
			bgbdy_float16_basetable[i|0x100]=0x8000;	//+0
			bgbdy_float16_shifttable[i|0x000]=24;
			bgbdy_float16_shifttable[i|0x100]=24;
		}
		else if(e<-14)	//denormals
		{
			bgbdy_float16_basetable[i|0x000]=(0x0400>>(-e-14))|0x0000;
			bgbdy_float16_basetable[i|0x100]=(0x0400>>(-e-14))|0x8000;
			bgbdy_float16_shifttable[i|0x000]=-e-1;
			bgbdy_float16_shifttable[i|0x100]=-e-1;
		}
		else if(e<=15) //normal range
		{
			bgbdy_float16_basetable[i|0x000]=((e+15)<<10)|0x0000;
			bgbdy_float16_basetable[i|0x100]=((e+15)<<10)|0x8000;
			bgbdy_float16_shifttable[i|0x000]=13;
			bgbdy_float16_shifttable[i|0x100]=13;
		}
		else if(e<128)	//overflow to Inf
		{
			bgbdy_float16_basetable[i|0x000]=0x7C00;
			bgbdy_float16_basetable[i|0x100]=0xFC00;
			bgbdy_float16_shifttable[i|0x000]=24;
			bgbdy_float16_shifttable[i|0x100]=24;
		}
		else	//Inf and NaN
		{
			bgbdy_float16_basetable[i|0x000]=0x7C00;
			bgbdy_float16_basetable[i|0x100]=0xFC00;
			bgbdy_float16_shifttable[i|0x000]=13;
			bgbdy_float16_shifttable[i|0x100]=13;
		}
	}
	
	return(1);
}

BGBDY_API double dtyDecodeFloat16(float16 val)
{
	u32 f, h;
	h=val.v;
	f=bgbdy_float16_mantissatable[
		bgbdy_float16_offsettable[h>>10]+(h&0x3ff)]+
		bgbdy_float16_exponenttable[h>>10];
	return(*(float *)(&f));
}

BGBDY_API float16 dtyEncodeFloat16(double val)
{
	float16 t;
	u32 f;
	int i;
	
	*(float *)(&f)=val;
	i=(f>>23)&0x1ff;

	t.v=bgbdy_float16_basetable[i]+
		((f&0x007fffff)>>bgbdy_float16_shifttable[i]);
	return(t);
}


#if 0
u32 BGBDY_Float18_ConvertMantissa(u32 i)
{
	u32 m;
	u32 e;
	
	if(!i)return(0);
	
	e=0; m=i<<12;
	while(!(m&0x00800000U))
	{
		e-=0x00800000U;
		m<<=1;
	}
	m&=~0x00800000U;
	e+=0x38800000U;
	return(m|e);
}

BGBDY_API int BGBDY_Float18_Init()
{
	static int init=0;
	int i, e;

	if(init)return(0);
	init=1;

	for(i=0; i<2048; i++)
	{
		bgbdy_float18_mantissatable[i]=BGBDY_Float18_ConvertMantissa(i);
	}

	for(i=2048; i<4096; i++)
	{
		bgbdy_float18_mantissatable[i]=0x38000000U+((i-2048)<<12);
	}

	for(i=0; i<63; i++)
	{
		bgbdy_float18_exponenttable[i]=0x00000000U+(i<<23);
	}

	for(i=64; i<127; i++)
	{
		bgbdy_float18_exponenttable[i]=0x80000000U+((i-64)<<23);
	}

//	bgbdy_float18_exponenttable[31]=0x47800000U;
//	bgbdy_float18_exponenttable[63]=0xC7800000U;

	bgbdy_float18_exponenttable[63]=0x7F800000U;	//+ NaN/Inf
	bgbdy_float18_exponenttable[127]=0xFF800000U;	//- NaN/Inf



	for(i=1; i<64; i++)
	{
		bgbdy_float18_offsettable[i]=2048;
	}

	bgbdy_float18_offsettable[0]=0;
	bgbdy_float18_offsettable[64]=0;

	for(i=0; i<256; i++)
	{
		e=i-127;
		if(e<-48)	//zeroes
		{
			bgbdy_float18_basetable[i|0x000]=0x00000;	//-0
			bgbdy_float18_basetable[i|0x100]=0x20000;	//+0
			bgbdy_float18_shifttable[i|0x000]=24;
			bgbdy_float18_shifttable[i|0x100]=24;
		}
		else if(e<-29)	//denormals
		{
			bgbdy_float18_basetable[i|0x000]=(0x0800>>(-e-29))|0x00000;
			bgbdy_float18_basetable[i|0x100]=(0x0800>>(-e-29))|0x20000;
			bgbdy_float18_shifttable[i|0x000]=-e-1;
			bgbdy_float18_shifttable[i|0x100]=-e-1;
		}
		else if(e<=31) //normal range
		{
			bgbdy_float18_basetable[i|0x000]=((e+31)<<11)|0x00000;
			bgbdy_float18_basetable[i|0x100]=((e+31)<<11)|0x20000;
			bgbdy_float18_shifttable[i|0x000]=12;
			bgbdy_float18_shifttable[i|0x100]=12;
		}
		else if(e<256)	//overflow to Inf
		{
			bgbdy_float18_basetable[i|0x000]=0x1F800;
			bgbdy_float18_basetable[i|0x100]=0x3F800;
			bgbdy_float18_shifttable[i|0x000]=24;
			bgbdy_float18_shifttable[i|0x100]=24;
		}
		else	//Inf and NaN
		{
			bgbdy_float18_basetable[i|0x000]=0x1F800;
			bgbdy_float18_basetable[i|0x100]=0x3F800;
			bgbdy_float18_shifttable[i|0x000]=12;
			bgbdy_float18_shifttable[i|0x100]=12;
		}
	}
	
	return(1);
}


BGBDY_API double dtyDecodeFloat18(float18 val)
{
	u32 f, h;
	h=val.v;
	f=bgbdy_float18_mantissatable[
		bgbdy_float18_offsettable[h>>11]+(h&0x7ff)]+
		bgbdy_float18_exponenttable[h>>11];
	return(*(float *)(&f));
}

BGBDY_API float18 dtyEncodeFloat18(double val)
{
	float18 t;
	u32 f;
	int i;
	
	*(float *)(&f)=val;
	i=(f>>23)&0x1ff;

	t.v=bgbdy_float18_basetable[i]+
		((f&0x007fffff)>>bgbdy_float18_shifttable[i]);
	return(t);
}
#endif
