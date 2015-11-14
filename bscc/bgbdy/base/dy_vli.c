/*
Written 2007-2011 by Brendan G Bohannon
*/

#include <bgbdy.h>

BGBDY_API int BGBDY_PeekChar(char **str)
{
	unsigned char *s;
	int i;

	s=(unsigned char *)(*str);
	i=*s++;

	if(!(i&0x80))return(i);

	if(i>=0xFC)
	{
		i=(i&1)<<30;
		i+=((*s++)&63)<<24;
		i+=((*s++)&63)<<18;
		i+=((*s++)&63)<<12;
		i+=((*s++)&63)<<6;
		i+=(*s++)&63;
	}else if(i>=0xF8)
	{
		i=(i&3)<<24;
		i+=((*s++)&63)<<18;
		i+=((*s++)&63)<<12;
		i+=((*s++)&63)<<6;
		i+=(*s++)&63;
	}else if(i>=0xF0)
	{
		i=(i&7)<<18;
		i+=((*s++)&63)<<12;
		i+=((*s++)&63)<<6;
		i+=(*s++)&63;
	}else if(i>=0xE0)
	{
		i=(i&15)<<12;
		i+=((*s++)&63)<<6;
		i+=(*s++)&63;
	}else if(i>=0xC0)
	{
		i=(i&31)<<6;
		i+=(*s++)&63;
	}else if(i>=0x80)
	{
//		continue;
	}

	return(i);
}

BGBDY_API int BGBDY_ParseChar(char **str)
{
	unsigned char *s;
	int i;

	s=(unsigned char *)(*str);
	i=*s++;

	if(!(i&0x80))
	{
		*str=(char *)s;
		return(i);
	}

	if(i>=0xFC)
	{
		i=(i&1)<<30;
		i+=((*s++)&63)<<24;
		i+=((*s++)&63)<<18;
		i+=((*s++)&63)<<12;
		i+=((*s++)&63)<<6;
		i+=(*s++)&63;
	}else if(i>=0xF8)
	{
		i=(i&3)<<24;
		i+=((*s++)&63)<<18;
		i+=((*s++)&63)<<12;
		i+=((*s++)&63)<<6;
		i+=(*s++)&63;
	}else if(i>=0xF0)
	{
		i=(i&7)<<18;
		i+=((*s++)&63)<<12;
		i+=((*s++)&63)<<6;
		i+=(*s++)&63;
	}else if(i>=0xE0)
	{
		i=(i&15)<<12;
		i+=((*s++)&63)<<6;
		i+=(*s++)&63;
	}else if(i>=0xC0)
	{
		i=(i&31)<<6;
		i+=(*s++)&63;
	}else if(i>=0x80)
	{
//		continue;
	}

	*str=(char *)s;
	return(i);
}

BGBDY_API int BGBDY_EmitChar(char **str, int j)
{
	unsigned char *t;
	int i;

	t=(unsigned char *)(*str);

	if(j<128)
	{
		if(j<=0) //Modified-UTF8
			{*t++=0xC0; *t++=0x80;}
			else *t++=j;
		*str=(char *)t;
		return(0);
	}

	if(j>0x3FFFFFF)
	{
		*t++=0xFC+(j>>30);
		*t++=0x80+((j>>24)&0x3F);
		*t++=0x80+((j>>18)&0x3F);
		*t++=0x80+((j>>12)&0x3F);
		*t++=0x80+((j>>6)&0x3F);
		*t++=0x80+(j&0x3F);
	}else if(j>0x1FFFFF)
	{
		*t++=0xF8+(j>>24);
		*t++=0x80+((j>>18)&0x3F);
		*t++=0x80+((j>>12)&0x3F);
		*t++=0x80+((j>>6)&0x3F);
		*t++=0x80+(j&0x3F);
	}else if(j>0xFFFF)
	{
		*t++=0xF0+(j>>18);
		*t++=0x80+((j>>12)&0x3F);
		*t++=0x80+((j>>6)&0x3F);
		*t++=0x80+(j&0x3F);
	}else if(j>0x7FF)
	{
		*t++=0xE0+(j>>12);
		*t++=0x80+((j>>6)&0x3F);
		*t++=0x80+(j&0x3F);
	}else if((j>0x7F) || !j)
	{
		*t++=0xC0+(j>>6);
		*t++=0x80+(j&0x3F);
	}else
	{
		if(!j) //Modified-UTF8
			{ *t++=0xC0; *t++=0x80; }
			else *t++=j;
	}

//	}else *t++=j;

	*str=(char *)t;
	return(0);
}

BGBDY_API char *BGBDY_NextChar(char *str)
{
	char *s;
	s=str;
	BGBDY_ParseChar(&s);
	return(s);
}

BGBDY_API char *BGBDY_PriorChar(char *str)
{
	char *s;
	s=str-1;
	while((*s&0xC0)==0x80)s--;
	return(s);
}

BGBDY_API byte *BGBDY_EncodeOpcode(byte *ip, int n)
{
	if(n>=192)
	{
		*ip++=0xC0+(n>>8);
		*ip++=n&0xFF;
		return(ip);
	}

	*ip++=n;
	return(ip);
}

BGBDY_API byte *BGBDY_EncVLI(byte *ip, s64 i)
{
	if(i>=0x100000000000000LL)
	{
		*ip++=0xFF;
		*ip++=(i>>56)&0xFF;
		*ip++=(i>>48)&0xFF;
		*ip++=(i>>40)&0xFF;
		*ip++=(i>>32)&0xFF;
		*ip++=(i>>24)&0xFF;
		*ip++=(i>>16)&0xFF;
		*ip++=(i>>8)&0xFF;
		*ip++=i&255;
		return(ip);
	}

	if(i>=0x2000000000000LL)
	{
		*ip++=(i>>56)|0xFE;
		*ip++=(i>>48)&0xFF;
		*ip++=(i>>40)&0xFF;
		*ip++=(i>>32)&0xFF;
		*ip++=(i>>24)&0xFF;
		*ip++=(i>>16)&0xFF;
		*ip++=(i>>8)&0xFF;
		*ip++=i&255;
		return(ip);
	}

	if(i>=0x40000000000LL)
	{
		*ip++=(i>>48)|0xFC;
		*ip++=(i>>40)&0xFF;
		*ip++=(i>>32)&0xFF;
		*ip++=(i>>24)&0xFF;
		*ip++=(i>>16)&0xFF;
		*ip++=(i>>8)&0xFF;
		*ip++=i&255;
		return(ip);
	}

	if(i>=0x800000000LL)
	{
		*ip++=(i>>40)|0xF8;
		*ip++=(i>>32)&0xFF;
		*ip++=(i>>24)&0xFF;
		*ip++=(i>>16)&0xFF;
		*ip++=(i>>8)&0xFF;
		*ip++=i&255;
		return(ip);
	}

	if(i>=0x10000000)
	{
		*ip++=(i>>32)|0xF0;
		*ip++=(i>>24)&0xFF;
		*ip++=(i>>16)&0xFF;
		*ip++=(i>>8)&0xFF;
		*ip++=i&255;
		return(ip);
	}

	if(i>=0x200000)
	{
		*ip++=(i>>24)|0xE0;
		*ip++=(i>>16)&0xFF;
		*ip++=(i>>8)&0xFF;
		*ip++=i&255;
		return(ip);
	}

	if(i>=0x4000)
	{
		*ip++=(i>>16)|0xC0;
		*ip++=(i>>8)&0xFF;
		*ip++=i&255;
		return(ip);
	}

	if(i>=0x80)
	{
		*ip++=(i>>8)|0x80;
		*ip++=i&255;
		return(ip);
	}

	*ip++=i;
	return(ip);
}

BGBDY_API byte *BGBDY_EncSVLI(byte *ip, s64 i)
{
	i=(i>=0)?(i*2):(((-i)*2)-1);
	ip=BGBDY_EncVLI(ip, i);
	return(ip);
}

BGBDY_API s64 BGBDY_DecVLI(byte **rip)
{
	byte *ip;
	s64 i;

	ip=*rip;
	if(*ip<0x80)
	{
		i=*ip++;
		*rip=ip; return(i);
	}

	if(*ip<0xC0)
	{
		i=(*ip++)&0x3F;
		i=(i<<8)|(*ip++);
		*rip=ip; return(i);
	}

	if(*ip<0xE0)
	{
		i=(*ip++)&0x1F;
		i=(i<<8)|(*ip++);
		i=(i<<8)|(*ip++);
		*rip=ip; return(i);
	}

	if(*ip<0xF0)
	{
		i=(*ip++)&0x0F;
		i=(i<<8)|(*ip++);
		i=(i<<8)|(*ip++);
		i=(i<<8)|(*ip++);
		*rip=ip; return(i);
	}

	if(*ip<0xF8)
	{
		i=(*ip++)&0x07;
		i=(i<<8)|(*ip++);
		i=(i<<8)|(*ip++);
		i=(i<<8)|(*ip++);
		i=(i<<8)|(*ip++);
		*rip=ip; return(i);
	}

	if(*ip<0xFC)
	{
		i=(*ip++)&0x03;
		i=(i<<8)|(*ip++);
		i=(i<<8)|(*ip++);
		i=(i<<8)|(*ip++);
		i=(i<<8)|(*ip++);
		i=(i<<8)|(*ip++);
		*rip=ip; return(i);
	}

	if(*ip<0xFE)
	{
		i=(*ip++)&0x01;
		i=(i<<8)|(*ip++);
		i=(i<<8)|(*ip++);
		i=(i<<8)|(*ip++);
		i=(i<<8)|(*ip++);
		i=(i<<8)|(*ip++);
		i=(i<<8)|(*ip++);
		*rip=ip; return(i);
	}

	if(*ip<0xFF)
	{
		i=0; ip++;
		i=(i<<8)|(*ip++);
		i=(i<<8)|(*ip++);
		i=(i<<8)|(*ip++);
		i=(i<<8)|(*ip++);
		i=(i<<8)|(*ip++);
		i=(i<<8)|(*ip++);
		i=(i<<8)|(*ip++);
		*rip=ip; return(i);
	}

	i=0; ip++;
	i=(i<<8)|(*ip++);
	i=(i<<8)|(*ip++);
	i=(i<<8)|(*ip++);
	i=(i<<8)|(*ip++);
	i=(i<<8)|(*ip++);
	i=(i<<8)|(*ip++);
	i=(i<<8)|(*ip++);
	i=(i<<8)|(*ip++);
	*rip=ip;
	return(i);
}

BGBDY_API s64 BGBDY_DecSVLI(byte **rip)
{
	s64 i;

	i=BGBDY_DecVLI(rip);
	i=(i&1)?(-((i>>1)+1)):(i>>1);
	return(i);
}


BGBDY_API byte *BGBDY_EncS16(byte *ip, s16 i)
{
	*ip++=i&255;
	*ip++=(i>>8)&255;
	return(ip);
}

BGBDY_API byte *BGBDY_EncS32(byte *ip, s32 i)
{
	*ip++=i&255;
	*ip++=(i>>8)&255;
	*ip++=(i>>16)&255;
	*ip++=(i>>24)&255;
	return(ip);
}

BGBDY_API byte *BGBDY_EncS64(byte *ip, s64 i)
{
	*ip++=i&255;
	*ip++=(i>>8)&255;
	*ip++=(i>>16)&255;
	*ip++=(i>>24)&255;
	*ip++=(i>>32)&255;
	*ip++=(i>>40)&255;
	*ip++=(i>>48)&255;
	*ip++=(i>>56)&255;
	return(ip);
}

BGBDY_API byte *BGBDY_EncU16(byte *ip, u16 i)
{
	*ip++=i&255;
	*ip++=(i>>8)&255;
	return(ip);
}

BGBDY_API byte *BGBDY_EncU32(byte *ip, u32 i)
{
	*ip++=i&255;
	*ip++=(i>>8)&255;
	*ip++=(i>>16)&255;
	*ip++=(i>>24)&255;
	return(ip);
}

BGBDY_API byte *BGBDY_EncU64(byte *ip, u64 i)
{
	*ip++=i&255;
	*ip++=(i>>8)&255;
	*ip++=(i>>16)&255;
	*ip++=(i>>24)&255;
	*ip++=(i>>32)&255;
	*ip++=(i>>40)&255;
	*ip++=(i>>48)&255;
	*ip++=(i>>56)&255;
	return(ip);
}

BGBDY_API byte *BGBDY_EncF32(byte *ip, f32 i)
{
	ip=BGBDY_EncU32(ip, *(u32 *)(&i));
	return(ip);
}

BGBDY_API byte *BGBDY_EncF64(byte *ip, f64 i)
{
	ip=BGBDY_EncU64(ip, *(u64 *)(&i));
	return(ip);
}

BGBDY_API byte BGBDY_Transpose4(byte val)
{
	static byte trans[16]=
		{ 0, 8, 4, 12, 2, 10, 6, 14, 1, 9, 5, 13, 3, 11, 7, 15 };
	return(trans[val&15]);
}

BGBDY_API byte BGBDY_Transpose8(byte val)
{
	byte t;
	
	t=(((byte)BGBDY_Transpose4(val))<<4)|
		BGBDY_Transpose4(val>>4);
	return(t);
}

BGBDY_API u16 BGBDY_Transpose16(u16 val)
{
	u16 t;
	
	t=(((u16)BGBDY_Transpose8(val))<<8)|
		BGBDY_Transpose8(val>>8);
	return(t);
}

BGBDY_API u32 BGBDY_Transpose32(u32 val)
{
	u32 t;
	
	t=(((u32)BGBDY_Transpose16(val))<<16)|
		BGBDY_Transpose16(val>>16);
	return(t);
}

BGBDY_API u64 BGBDY_Transpose64(u64 val)
{
	u64 t;
	
	t=(((u64)BGBDY_Transpose32(val))<<32)|
		BGBDY_Transpose32(val>>32);
	return(t);
}

BGBDY_API byte *BGBDY_EncVLF(byte *ip, f64 val)
{
	u64 v, w;
	s64 f;
	int e, sg;
	
	if(val==0)
	{
		ip=BGBDY_EncSVLI(ip, 0);
		ip=BGBDY_EncSVLI(ip, 0);
		return(ip);
	}

	v=*(u64 *)(&val);
	e=((v>>52)&0x7FF)-1023;
	sg=(v>>63)&1;
	
	if(e==1024)
	{
		ip=BGBDY_EncSVLI(ip, 0);
		if(v&0x000FFFFFFFFFFFFF)
			{ ip=BGBDY_EncSVLI(ip, sg?-2:2); }
		else
			{ ip=BGBDY_EncSVLI(ip, sg?-4:4); }
	}
	
	w=BGBDY_Transpose64(v<<12);
	f=w; if(sg)f=-f;

	ip=BGBDY_EncSVLI(ip, e);
	ip=BGBDY_EncSVLI(ip, f);
	return(ip);
}

BGBDY_API u32 BGBDY_DecU32(byte **rip)
{
	byte *ip;
	u32 l;
	ip=*rip;
	l=ip[0]|(ip[1]<<8)|(ip[2]<<16)|(ip[3]<<24);
	*rip=ip;
	return(l);
}

BGBDY_API s32 BGBDY_DecS32(byte **rip)
{
	byte *ip;
	s32 l;
	ip=*rip;
	l=ip[0]|(ip[1]<<8)|(ip[2]<<16)|(ip[3]<<24);
	*rip=ip;
	return(l);
}

BGBDY_API u64 BGBDY_DecU64(byte **rip)
{
	u64 l;
	u32 hl, hh;
	
	hl=BGBDY_DecU32(rip);
	hh=BGBDY_DecU32(rip);
	l=hl|(((u64)hh)<<32);
	return(l);
}

BGBDY_API s64 BGBDY_DecS64(byte **rip)
{
	s64 l;
	u32 hl, hh;
	
	hl=BGBDY_DecU32(rip);
	hh=BGBDY_DecU32(rip);
	l=hl|(((s64)hh)<<32);
	return(l);
}

BGBDY_API s128 BGBDY_DecS128(byte **rip)
{
	s128 l;
	u32 ll, lh, hl, hh;
	
	ll=BGBDY_DecU32(rip);
	lh=BGBDY_DecU32(rip);
	hl=BGBDY_DecU32(rip);
	hh=BGBDY_DecU32(rip);

	l=s128mk4(ll, lh, hl, hh);

	return(l);
}

BGBDY_API f32 BGBDY_DecFloat32(byte **rip)
{
	u32 l;
	f32 f;

	l=BGBDY_DecU32(rip);
	f=*(f32 *)(&l);
	return(f);
}

BGBDY_API f64 BGBDY_DecFloat64(byte **rip)
{
	u64 l;
	f64 f;

	l=BGBDY_DecU64(rip);
	f=*(f64 *)(&l);
	return(f);
}

BGBDY_API f128 BGBDY_DecFloat128(byte **rip)
{
	s128 l;
	f128 f;

	l=BGBDY_DecS128(rip);
	f=*(f128 *)(&l);
	return(f);
}
