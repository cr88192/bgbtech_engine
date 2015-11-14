#ifndef BGBBTJ_INTFCN_H
#define BGBBTJ_INTFCN_H

#ifdef BGBBTJ_NEED_INTFCN

static inline u16 bgbbtj_bswap16(u16 src)
{
#if defined(_MSC_VER) && defined(_M_IX86)
	int i;
	__asm {
		mov ax, src
		xchg al, ah
		mov i, eax
	}
	return(i);
#else
	return(((src<<8)&0xFF00)|((src>>8)&0x00FF));
#endif
}

static inline u32 bgbbtj_bswap32(u32 src)
{
#if defined(_MSC_VER) && defined(_M_IX86)
	int i;
	__asm {
		mov eax, src
		bswap eax
		mov i, eax
	}
	return(i);
#else
	int i;
	i=((src<<24)&0xFF000000)|
	  ((src<< 8)&0x00FF0000)|
	  ((src>> 8)&0x0000FF00)|
	  ((src>>24)&0x000000FF);
	return(i);
#endif
}

static inline u64 bgbbtj_bswap64(u64 src)
{
	return(
		(((u64)bgbbtj_bswap32(src))<<32)|
		((u64)bgbbtj_bswap32(src>>32)));
}


static inline int bgbbtj_getu8(byte *src)
{
	return(*src);
}

static inline int bgbbtj_gets8(byte *src)
{
	return(*(signed char *)src);
}

static inline void bgbbtj_setu8(byte *src, int val)
	{ *src=val; }
static inline void bgbbtj_sets8(byte *src, int val)
	{ *src=val; }


static inline int bgbbtj_getu16le(byte *src)
{
#if defined(X86) || defined(X86_64)
	return(*(u16 *)src);
#else
	return((u16)(src[0]|(src[1]<<8)));
#endif
}

static inline int bgbbtj_gets16le(byte *src)
{
#if defined(X86) || defined(X86_64)
	return(*(s16 *)src);
#else
	return((s16)(src[0]|(src[1]<<8)));
#endif
}

static inline int bgbbtj_getu16be(byte *src)
{
#if defined(_MSC_VER) && defined(_M_IX86)
	int i;
	__asm {
		mov eax, src
		mov ax, dword ptr [eax]
		xchg al, ah
		movzx eax, ax
		mov i, eax
	}
	return(i);
#else
	return((u16)(src[1]|(src[0]<<8)));
#endif
}

static inline int bgbbtj_gets16be(byte *src)
{
#if defined(_MSC_VER) && defined(_M_IX86)
	int i;
	__asm {
		mov eax, src
		mov ax, dword ptr [eax]
		xchg al, ah
		movsx eax, ax
		mov i, eax
	}
	return(i);
#else
	return((s16)(src[1]|(src[0]<<8)));
#endif
}

static inline void bgbbtj_setu16le(byte *dst, int val)
{
#if defined(X86) || defined(X86_64)
	*(u16 *)dst=val;
#else
	dst[0]=(byte) val;
	dst[1]=(byte)(val>>8);
#endif
}

static inline void bgbbtj_setu16be(byte *dst, int val)
{
#if defined(_MSC_VER) && defined(_M_IX86)
	__asm {
		mov edx, dst
		mov eax, val
		xchg al, ah
		mov word ptr [edx], ax
	}
#else
	src[0]=(byte)(val>>8);
	src[1]=(byte) val;
#endif
}

static inline void bgbbtj_sets16le(byte *dst, int val)
	{ bgbbtj_setu16le(dst, val); }
static inline void bgbbtj_sets16be(byte *dst, int val)
	{ bgbbtj_setu16be(dst, val); }


static inline int bgbbtj_getu24le(byte *src)
{
#if defined(X86) || defined(X86_64)
	return((*(u32 *)src)&0x00FFFFFF);
#else
	return((u32)(src[0]|(src[1]<<8)|(src[2]<<16)));
#endif
}

static inline int bgbbtj_gets24le(byte *src)
{
#if defined(X86) || defined(X86_64)
	return(((*(s32 *)src)<<8)>>8);
#else
	return((((s32)(src[0]|(src[1]<<8)|(src[2]<<16)))<<8)>>8);
#endif
}

static inline int bgbbtj_getu24be(byte *src)
{
#if defined(_MSC_VER) && defined(_M_IX86)
	int i;
	__asm {
		mov eax, src
		mov eax, dword ptr [eax]
		bswap eax
		shr eax, 8
		mov i, eax
	}
	return(i);
#else
	return((u32)(src[2]|(src[1]<<8)|(src[0]<<16)));
#endif
}

static inline int bgbbtj_gets24be(byte *src)
{
#if defined(_MSC_VER) && defined(_M_IX86)
	int i;
	__asm {
		mov eax, src
		mov eax, dword ptr [eax]
		bswap eax
		sar eax, 8
		mov i, eax
	}
	return(i);
#else
	return((u32)(src[2]|(src[1]<<8)|(src[0]<<16)));
#endif
}

static inline void bgbbtj_setu24le(byte *dst, int val)
{
#if defined(_MSC_VER) && defined(_M_IX86)
	__asm {
		mov edx, dst
		mov eax, val
		mov word ptr [edx], ax
		shr eax, 16
		mov byte ptr [edx+2], al
	}
#else
	dst[0]=(byte) val;
	dst[1]=(byte)(val>> 8);
	dst[2]=(byte)(val>>16);
#endif
}

static inline void bgbbtj_setu24be(byte *dst, int val)
{
#if defined(_MSC_VER) && defined(_M_IX86)
	__asm {
		mov edx, dst
		mov eax, val
		bswap eax
		shr eax, 8
		mov word ptr [edx], ax
		shr eax, 16
		mov byte ptr [edx+2], al
	}
#else
	src[0]=(byte)(val>>16);
	src[1]=(byte)(val>> 8);
	src[2]=(byte) val;
#endif
}

static inline void bgbbtj_sets24le(byte *dst, int val)
	{ bgbbtj_setu24le(dst, val); }
static inline void bgbbtj_sets24be(byte *dst, int val)
	{ bgbbtj_setu24be(dst, val); }


static inline u32 bgbbtj_getu32le(byte *src)
{
#if defined(X86) || defined(X86_64)
	return(*(u32 *)src);
#else
	return((u32)(src[0]|(src[1]<<8)|(src[2]<<16)|(src[3]<<24)));
#endif
}

static inline s32 bgbbtj_gets32le(byte *src)
{
#if defined(X86) || defined(X86_64)
	return(*(s32 *)src);
#else
	return((s32)(src[0]|(src[1]<<8)|(src[2]<<16)|(src[3]<<24)));
#endif
}

static inline u32 bgbbtj_getu32be(byte *src)
{
#if defined(_MSC_VER) && defined(_M_IX86)
	int i;
	__asm {
		mov eax, src
		mov eax, dword ptr [eax]
		bswap eax
		mov i, eax
	}
	return(i);
#else
	return((u32)(src[3]|(src[2]<<8)|(src[1]<<16)|(src[0]<<24)));
#endif
}

static inline s32 bgbbtj_gets32be(byte *src)
{
#if defined(_MSC_VER) && defined(_M_IX86)
	int i;
	__asm {
		mov eax, src
		mov eax, dword ptr [eax]
		bswap eax
		mov i, eax
	}
	return(i);
#else
	return((s32)(src[3]|(src[2]<<8)|(src[1]<<16)|(src[0]<<24)));
#endif
}

static inline void bgbbtj_setu32le(byte *dst, int val)
{
#if defined(X86) || defined(X86_64)
	*(u32 *)dst=val;
#else
	dst[0]=(byte) val     ;
	dst[1]=(byte)(val>> 8);
	dst[2]=(byte)(val>>16);
	dst[3]=(byte)(val>>24);
#endif
}

static inline void bgbbtj_setu32be(byte *dst, int val)
{
#if defined(_MSC_VER) && defined(_M_IX86)
	__asm {
		mov edx, dst
		mov eax, val
		bswap eax
		mov dword ptr [edx], eax
	}
#else
	dst[0]=(byte)(val>>24);
	dst[1]=(byte)(val>>16);
	src[2]=(byte)(val>> 8);
	src[3]=(byte) val     ;
#endif
}

static inline void bgbbtj_sets32le(byte *dst, int val)
	{ bgbbtj_setu32le(dst, val); }
static inline void bgbbtj_sets32be(byte *dst, int val)
	{ bgbbtj_setu32be(dst, val); }


static inline u64 bgbbtj_getu40le(byte *src)
{
	return(bgbbtj_getu32le(src)|(((u64)src[4])<<32));
}

static inline s64 bgbbtj_gets40le(byte *src)
{
	return(bgbbtj_getu32le(src)|(((s64)((signed char *)src)[4])<<32));
}

static inline u64 bgbbtj_getu40be(byte *src)
{
	return((((u64)bgbbtj_getu32be(src))<<8)|((u64)src[4]));
}

static inline s64 bgbbtj_gets40be(byte *src)
{
	return((((s64)bgbbtj_gets32be(src))<<8)|((s64)src[4]));
}

static inline void bgbbtj_setu40le(byte *dst, u64 val)
{
#if defined(X86) || defined(X86_64)
	*(u32 *)dst=val;
	dst[4]=(byte)(val>>32);
#else
	int val0, val1;
	val0=(int)val;
	val1=(int)(val>>32);
	dst[0]=(byte) val0     ;
	dst[1]=(byte)(val0>> 8);
	dst[2]=(byte)(val0>>16);
	dst[3]=(byte)(val0>>24);
	dst[4]=(byte) val1     ;
#endif
}

static inline void bgbbtj_setu40be(byte *dst, u64 val)
{
#if defined(_MSC_VER) && defined(_M_IX86)
	int val0, val1;
	val0=(int)val;
	val1=(int)(val>>8);
	__asm {
		mov edx, dst
		mov eax, val1
		bswap eax
		mov dword ptr [edx], eax
		mov eax, val0
		mov byte ptr [edx+4], al
	}
#else
	int val0, val1;
	val0=(int)val;
	val1=(int)(val>>32);
	dst[0]=(byte) val1     ;
	dst[1]=(byte)(val0>>24);
	dst[2]=(byte)(val0>>16);
	src[3]=(byte)(val0>> 8);
	src[4]=(byte) val0     ;
#endif
}

static inline void bgbbtj_sets40le(byte *dst, s64 val)
	{ bgbbtj_setu40le(dst, (u64)val); }
static inline void bgbbtj_sets40be(byte *dst, s64 val)
	{ bgbbtj_setu40be(dst, (u64)val); }


static inline u64 bgbbtj_getu48le(byte *src)
{
	return(((u64)bgbbtj_getu32le(src))|
		(((u64)bgbbtj_getu16le(src+4))<<32));
}

static inline s64 bgbbtj_gets48le(byte *src)
{
	return(((s64)bgbbtj_getu32le(src))|
		(((s64)bgbbtj_getu16le(src+4))<<32));
}

static inline u64 bgbbtj_getu48be(byte *src)
{
	return((((u64)bgbbtj_getu32be(src))<<16)|
		((u64)bgbbtj_getu16be(src+4)));
}

static inline s64 bgbbtj_gets48be(byte *src)
{
	return((((s64)bgbbtj_gets32be(src))<<16)|
		((s64)bgbbtj_getu16be(src+4)));
}

static inline void bgbbtj_setu48le(byte *dst, u64 val)
{
#if defined(X86) || defined(X86_64)
	*(u32 *)dst=val;
	*(u16 *)(dst+4)=(u16)(val>>32);
#else
	int val0, val1;
	val0=(int)val;
	val1=(int)(val>>32);
	dst[0]=(byte) val0     ;
	dst[1]=(byte)(val0>> 8);
	dst[2]=(byte)(val0>>16);
	dst[3]=(byte)(val0>>24);
	dst[4]=(byte) val1     ;
	dst[5]=(byte)(val1>> 8);
#endif
}

static inline void bgbbtj_setu48be(byte *dst, u64 val)
{
#if defined(_MSC_VER) && defined(_M_IX86)
	int val0, val1;
	val0=(int)val;
	val1=(int)(val>>16);
	__asm {
		mov edx, dst
		mov eax, val1
		bswap eax
		mov dword ptr [edx], eax
		mov eax, val0
		xchg al, ah
		mov word ptr [edx+4], ax
	}
#else
	int val0, val1;
	val0=(int)val;
	val1=(int)(val>>32);
	dst[0]=(byte)(val1>> 8);
	dst[1]=(byte) val1     ;
	dst[2]=(byte)(val0>>24);
	dst[3]=(byte)(val0>>16);
	src[4]=(byte)(val0>> 8);
	src[5]=(byte) val0     ;
#endif
}

static inline void bgbbtj_sets48le(byte *dst, s64 val)
	{ bgbbtj_setu48le(dst, (u64)val); }
static inline void bgbbtj_sets48be(byte *dst, s64 val)
	{ bgbbtj_setu48be(dst, (u64)val); }


static inline u64 bgbbtj_getu56le(byte *src)
{
	return(((u64)bgbbtj_getu32le(src))|
		(((u64)bgbbtj_getu24le(src+4))<<32));
}

static inline s64 bgbbtj_gets56le(byte *src)
{
	return(((s64)bgbbtj_getu32le(src))|
		(((s64)bgbbtj_getu24le(src+4))<<32));
}

static inline u64 bgbbtj_getu56be(byte *src)
{
	return((((u64)bgbbtj_getu32be(src))<<24)|
		((u64)bgbbtj_getu24be(src+4)));
}

static inline s64 bgbbtj_gets56be(byte *src)
{
	return((((s64)bgbbtj_gets32be(src))<<24)|
		((s64)bgbbtj_getu24be(src+4)));
}

static inline void bgbbtj_setu56le(byte *dst, u64 val)
{
#if defined(X86) || defined(X86_64)
	*(u32 *)dst=val;
	bgbbtj_setu24le(dst+4, val>>32);
#else
	int val0, val1;
	val0=(int)val;
	val1=(int)(val>>32);
	dst[0]=(byte) val0     ;
	dst[1]=(byte)(val0>> 8);
	dst[2]=(byte)(val0>>16);
	dst[3]=(byte)(val0>>24);
	dst[4]=(byte) val1     ;
	dst[5]=(byte)(val1>> 8);
	dst[6]=(byte)(val1>>16);
#endif
}

static inline void bgbbtj_setu56be(byte *dst, u64 val)
{
#if defined(_MSC_VER) && defined(_M_IX86)
	int val0, val1;
	val0=(int)val;
	val1=(int)(val>>24);
	__asm {
		mov edx, dst
		mov eax, val1
		bswap eax
		mov dword ptr [edx], eax
		mov eax, val0
		bswap eax
		mov word ptr [edx+3], ax
	}
#else
	int val0, val1;
	val0=(int)val;
	val1=(int)(val>>32);
	dst[0]=(byte)(val1>>16);
	dst[1]=(byte)(val1>> 8);
	dst[2]=(byte) val1     ;
	dst[3]=(byte)(val0>>24);
	dst[4]=(byte)(val0>>16);
	src[5]=(byte)(val0>> 8);
	src[6]=(byte) val0     ;
#endif
}

static inline void bgbbtj_sets56le(byte *dst, s64 val)
	{ bgbbtj_setu56le(dst, (u64)val); }
static inline void bgbbtj_sets56be(byte *dst, s64 val)
	{ bgbbtj_setu56be(dst, (u64)val); }


static inline u64 bgbbtj_getu64le(byte *src)
{
#if defined(X86) || defined(X86_64)
	return(*(u64 *)src);
#else
	return(((u64)bgbbtj_getu32le(src))|
		(((u64)bgbbtj_getu32le(src+4))<<32));
#endif
}

static inline s64 bgbbtj_gets64le(byte *src)
{
#if defined(X86) || defined(X86_64)
	return(*(s64 *)src);
#else
	return(((s64)bgbbtj_getu32le(src))|
		(((s64)bgbbtj_getu32le(src+4))<<32));
#endif
}

static inline u64 bgbbtj_getu64be(byte *src)
{
	return((((u64)bgbbtj_getu32be(src))<<32)|
		((u64)bgbbtj_getu32be(src+4)));
}

static inline s64 bgbbtj_gets64be(byte *src)
{
	return((((s64)bgbbtj_gets32be(src))<<32)|
		((s64)bgbbtj_getu32be(src+4)));
}

static inline void bgbbtj_setu64le(byte *dst, u64 val)
{
#if defined(X86) || defined(X86_64)
	*(u64 *)dst=val;
#else
	int val0, val1;
	val0=(int)val;
	val1=(int)(val>>32);
	dst[0]=(byte) val0     ;
	dst[1]=(byte)(val0>> 8);
	dst[2]=(byte)(val0>>16);
	dst[3]=(byte)(val0>>24);
	dst[4]=(byte) val1     ;
	dst[5]=(byte)(val1>> 8);
	dst[6]=(byte)(val1>>16);
	dst[7]=(byte)(val1>>24);
#endif
}

static inline void bgbbtj_setu64be(byte *dst, u64 val)
{
#if defined(_MSC_VER) && defined(_M_IX86)
	int val0, val1;
	val0=(int)val;
	val1=(int)(val>>32);
	__asm {
		mov edx, dst
		mov eax, val1
		bswap eax
		mov dword ptr [edx], eax
		mov eax, val0
		bswap eax
		mov word ptr [edx+4], ax
	}
#else
	int val0, val1;
	val0=(int)val;
	val1=(int)(val>>32);
	dst[0]=(byte)(val1>>24);
	dst[1]=(byte)(val1>>16);
	dst[2]=(byte)(val1>> 8);
	dst[3]=(byte) val1     ;
	dst[4]=(byte)(val0>>24);
	dst[5]=(byte)(val0>>16);
	src[6]=(byte)(val0>> 8);
	src[7]=(byte) val0     ;
#endif
}

static inline void bgbbtj_sets64le(byte *dst, s64 val)
	{ bgbbtj_setu64le(dst, (u64)val); }
static inline void bgbbtj_sets64be(byte *dst, s64 val)
	{ bgbbtj_setu64be(dst, (u64)val); }


#endif //BGBBTJ_NEED_INTFCN

#endif //BGBBTJ_INTFCN_H
