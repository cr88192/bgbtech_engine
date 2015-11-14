/*
Written 2007-2010 by Brendan G Bohannon
This code is re-assigned into the public domain.
*/

#include <bs2asm.h>

BGBSVM_API byte *BS2ASM_EncodeOpcode(byte *ip, int n)
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

BGBSVM_API byte *BS2ASM_EncVLI(byte *ip, long long i)
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

BGBSVM_API byte *BS2ASM_EncSVLI(byte *ip, long long i)
{
	i=(i>=0)?(i*2):(((-i)*2)-1);
	ip=BS2ASM_EncVLI(ip, i);
	return(ip);
}

BGBSVM_API s64 BS2ASM_DecVLI(byte **rip)
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

BGBSVM_API s64 BS2ASM_DecSVLI(byte **rip)
{
	s64 i;

	i=BS2ASM_DecVLI(rip);
	i=(i&1)?(-((i>>1)+1)):(i>>1);
	return(i);
}


BGBSVM_API byte *BS2ASM_EncS16(byte *ip, s16 i)
{
	*ip++=i&255;
	*ip++=(i>>8)&255;
	return(ip);
}

BGBSVM_API byte *BS2ASM_EncS32(byte *ip, s32 i)
{
	*ip++=i&255;
	*ip++=(i>>8)&255;
	*ip++=(i>>16)&255;
	*ip++=(i>>24)&255;
	return(ip);
}

BGBSVM_API byte *BS2ASM_EncS64(byte *ip, s64 i)
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

BGBSVM_API byte *BS2ASM_EncU16(byte *ip, u16 i)
{
	*ip++=i&255;
	*ip++=(i>>8)&255;
	return(ip);
}

BGBSVM_API byte *BS2ASM_EncU32(byte *ip, u32 i)
{
	*ip++=i&255;
	*ip++=(i>>8)&255;
	*ip++=(i>>16)&255;
	*ip++=(i>>24)&255;
	return(ip);
}

BGBSVM_API byte *BS2ASM_EncU64(byte *ip, u64 i)
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

BGBSVM_API byte *BS2ASM_EncF32(byte *ip, f32 i)
{
	ip=BS2ASM_EncU32(ip, *(u32 *)(&i));
	return(ip);
}

BGBSVM_API byte *BS2ASM_EncF64(byte *ip, f64 i)
{
	ip=BS2ASM_EncU64(ip, *(u64 *)(&i));
	return(ip);
}

BGBSVM_API u32 BS2ASM_DecU32(byte **rip)
{
	byte *ip;
	u32 l;
	ip=*rip;
	l=ip[0]|(ip[1]<<8)|(ip[2]<<16)|(ip[3]<<24);
	*rip=ip;
	return(l);
}

BGBSVM_API s32 BS2ASM_DecS32(byte **rip)
{
	byte *ip;
	s32 l;
	ip=*rip;
	l=ip[0]|(ip[1]<<8)|(ip[2]<<16)|(ip[3]<<24);
	*rip=ip;
	return(l);
}

BGBSVM_API u64 BS2ASM_DecU64(byte **rip)
{
	u64 l;
	u32 hl, hh;
	
	hl=BS2ASM_DecU32(rip);
	hh=BS2ASM_DecU32(rip);
	l=hl|(((u64)hh)<<32);
	return(l);
}

BGBSVM_API s64 BS2ASM_DecS64(byte **rip)
{
	s64 l;
	u32 hl, hh;
	
	hl=BS2ASM_DecU32(rip);
	hh=BS2ASM_DecU32(rip);
	l=hl|(((s64)hh)<<32);
	return(l);
}

BGBSVM_API s128 BS2ASM_DecS128(byte **rip)
{
	s128 l;
	u32 ll, lh, hl, hh;
	
	ll=BS2ASM_DecU32(rip);
	lh=BS2ASM_DecU32(rip);
	hl=BS2ASM_DecU32(rip);
	hh=BS2ASM_DecU32(rip);

	l=s128mk4(ll, lh, hl, hh);

	return(l);
}

BGBSVM_API f32 BS2ASM_DecFloat32(byte **rip)
{
	u32 l;
	f32 f;

	l=BS2ASM_DecU32(rip);
	f=*(f32 *)(&l);
	return(f);
}

BGBSVM_API f64 BS2ASM_DecFloat64(byte **rip)
{
	u64 l;
	f64 f;

	l=BS2ASM_DecU64(rip);
	f=*(f64 *)(&l);
	return(f);
}

BGBSVM_API f128 BS2ASM_DecFloat128(byte **rip)
{
	s128 l;
	f128 f;

	l=BS2ASM_DecS128(rip);
	f=*(f128 *)(&l);
	return(f);
}
