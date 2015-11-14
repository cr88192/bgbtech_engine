#include <zpack.h>

u16 (*ZPACK_US_f)(u16 i);
u32 (*ZPACK_UI_f)(u32 i);
u64 (*ZPACK_UL_f)(u64 i);

static u32 (*ZPACK_GetDateTime_f)();
static u32 (*ZPACK_GetTimeMS_f)();

static u16 zpack_us(u16 i) { return(i); }
static u32 zpack_ui(u32 i) { return(i); }
static u64 zpack_ul(u64 i) { return(i); }

static u16 zpack_swus(u16 i) { return((i>>8)|(i<<8)); }
static u32 zpack_swui(u32 i)
	{ return((i>>24)|(i<<24)|((i>>8)&0xFF00)|((i<<8)&0xFF0000)); }
static u64 zpack_swul(u64 i)
{
	return(zpack_swui((u32)(i>>32))|((u64)zpack_swui((u32)i)<<32));
//	return(	((i>>56)&0x00000000000000FF)|((i<<56)&0xFF00000000000000)|
//		((i>>48)&0x000000000000FF00)|((i<<48)&0x00FF000000000000)|
//		((i>>24)&0x0000000000FF0000)|((i<<24)&0x0000FF0000000000)|
//		((i>>8) &0x00000000FF000000)|((i<<8) &0x000000FF00000000));
}

static u32 zpack_def_time() { return(0); }
static u32 zpack_def_timems() { return(0); }

static int zpack_init=0;
int zpack_endianess;	//0=LE, 1=BE

ZPACK_API void ZPACK_SetTimeFunc(u32 (*fn)())
	{ ZPACK_GetDateTime_f=fn; }
ZPACK_API void ZPACK_SetTimeFuncMS(u32 (*fn)())
	{ ZPACK_GetTimeMS_f=fn; }

u32 ZPACK_GetDateTime()
{
	if(!ZPACK_GetDateTime_f)
		return(0);
	return(ZPACK_GetDateTime_f());
}

u32 ZPACK_GetTimeMS()
{
	if(!ZPACK_GetTimeMS_f)
		return(0);
	return(ZPACK_GetTimeMS_f());
}

u16 ZPACK_US(u16 i) {return(ZPACK_US_f(i));}
u32 ZPACK_UI(u32 i) {return(ZPACK_UI_f(i));}
u64 ZPACK_UL(u64 i) {return(ZPACK_UL_f(i));}

u16 ZPACK_GetDate()
{
	u32 t;
	t=ZPACK_GetDateTime();
	return(t>>16);
}

u16 ZPACK_GetTime()
{
	u32 t;
	t=ZPACK_GetDateTime();
	return(t&0xFFFF);
}

ZPACK_API int ZPACK_Init()
{
	byte ut[4]={0x12, 0x34, 0x56, 0x78};

	if(zpack_init)return(0);
	zpack_init=1;

	if(sizeof(byte)!=1)
	{
		printf("sizeof(byte)!=1\n");
		return(-1);
	}
	if(sizeof(u16)!=2)
	{
		printf("sizeof(u16)!=2\n");
		return(-1);
	}
	if(sizeof(u32)!=4)
	{
		printf("sizeof(u32)!=4\n");
		return(-1);
	}
	if(sizeof(u64)!=8)
	{
		printf("sizeof(u64)!=8\n");
		return(-1);
	}

	ZPACK_GetDateTime_f=&zpack_def_time;
	ZPACK_GetTimeMS_f=&zpack_def_timems;

	if(*((u32*)(&ut))==0x78563412)
	{
		zpack_endianess=0;
		ZPACK_US_f=&zpack_us;
		ZPACK_UI_f=&zpack_ui;
		ZPACK_UL_f=&zpack_ul;
	}
	if(*((u32*)(&ut))==0x12345678)
	{
		zpack_endianess=1;
		ZPACK_US_f=&zpack_swus;
		ZPACK_UI_f=&zpack_swui;
		ZPACK_UL_f=&zpack_swul;
	}

	return(0);
}
