#include <bgbdy.h>

BGBDY_API char *dyllFlagSigNextFlag(char *sig)
{
	char *s;

	if(!sig || !(*sig))return(sig);
	
	s=sig;
	switch(*s++)
	{
	case 'A': case 'B':
	case 'C': case 'D':
		s=dyllFlagSigNextFlag(s);
		break;
	case 'u': case 'U':
		dyll_SigReadQName(&s);
		break;
	default:
		break;
	}
	return(s);
}

BGBDY_API s64 dyllFlagSigPeekFlag(char *sig)
{
	s64 fl;
	char *s;
	
	if(!sig || !(*sig))return(0);
	
	fl=0; s=sig;
	switch(*s++)
	{
	case 'A':
	case 'B':
		switch(*s++)
		{
		case 'u': case 'U':
			dyll_SigReadQName(&s); fl=0; break;
		default: fl=0; break;
		}
		break;
	case 'C':
		switch(*s++)
		{
		case 'a': fl=BGBDYC_FLAG_ASYNC; break;
		case 'b': fl=BGBDYC_FLAG_BYREF; break;
		case 'd': fl=BGBDYC_FLAG_TYPEDEF; break;
		case 'e': fl=BGBDYC_FLAG_EXTERN; break;

		case 'g': fl=BGBDYC_FLAG_SETGID; break;
		case 'h': fl=BGBDYC_FLAG_SETUID; break;

		case 'i': fl=BGBDYC_FLAG_INTERFACE; break;
		case 's': fl=BGBDYC_FLAG_SYNCHRONIZED; break;
		case 'v': fl=BGBDYC_FLAG_VOLATILE; break;
		case 'u': case 'U':
			dyll_SigReadQName(&s); fl=0; break;
		default: fl=0; break;
		}
		break;
	case 'D':
		switch(*s++)
		{
		case 'e': fl=BGBDYC_FLAG_DLLEXPORT; break;
		case 'i': fl=BGBDYC_FLAG_DLLIMPORT; break;

		case 't': fl=BGBDYC_FLAG_THREAD; break;
		case 'u': case 'U':
			dyll_SigReadQName(&s); fl=0; break;
		default: fl=0; break;
		}
		break;

	case 'a': fl=BGBDYC_FLAG_ABSTRACT; break;
	case 'b': fl=BGBDYC_FLAG_BIGENDIAN; break;
	case 'c': fl=BGBDYC_FLAG_CDECL; break;
	case 'd': fl=BGBDYC_FLAG_DELEGATE; break;
	case 'e': fl=BGBDYC_FLAG_EVENT; break;
	case 'f': fl=BGBDYC_FLAG_AMD64; break;
	case 'g': fl=BGBDYC_FLAG_GETTER; break;
	case 'h': fl=BGBDYC_FLAG_SETTER; break;
	case 'i': fl=BGBDYC_FLAG_INLINE; break;
	case 'j': fl=BGBDYC_FLAG_FINAL; break;
	case 'k': fl=BGBDYC_FLAG_CONST; break;
	case 'l': fl=BGBDYC_FLAG_LTLENDIAN; break;
	//m
	case 'n': fl=BGBDYC_FLAG_NATIVE; break;
	case 'o': fl=BGBDYC_FLAG_OVERRIDE; break;
	case 'p': fl=BGBDYC_FLAG_PUBLIC; break;
	case 'q': fl=BGBDYC_FLAG_PRIVATE; break;
	case 'r': fl=BGBDYC_FLAG_PROTECTED; break;
	case 's': fl=BGBDYC_FLAG_STATIC; break;
	case 't': fl=BGBDYC_FLAG_THISCALL; break;
	case 'u': case 'U':
		dyll_SigReadQName(&s); fl=0; break;

	case 'v': fl=BGBDYC_FLAG_VIRTUAL; break;
	case 'w': fl=BGBDYC_FLAG_STDCALL; break;
	case 'x': fl=BGBDYC_FLAG_XCALL; break;
	case 'y': fl=BGBDYC_FLAG_TRANSIENT; break;
	//z

	default:
		fl=0;
		break;
	}
	return(fl);
}

BGBDY_API s64 dyllFlagSigReadFlag(char **rsig)
{
	s64 fl;
	char *s;
	
	s=*rsig;
	fl=dyllFlagSigPeekFlag(s);
	s=dyllFlagSigNextFlag(s);
	*rsig=s;
	return(fl);
}

BGBDY_API s64 dyllDecodeFlagSig(char *sig)
{
	s64 fl;
	char *s;
	
	if(!sig)return(0);
	
	s=sig; fl=0;
	while(*s)
	{
		fl|=dyllFlagSigPeekFlag(s);
		s=dyllFlagSigNextFlag(s);
	}
	return(fl);
}

int DYLL_GetArch()
{
#ifdef X86
	return(BGBDYC_ARCH_X86);
#endif
#ifdef X86_64
	return(BGBDYC_ARCH_X64);
#endif
	return(BGBDYC_ARCH_DEFAULT);
}

int DYLL_FlagSig_ModifierChar(s64 i)
{
	int c;

	if(!i)return(0);

	if(DYLL_GetArch()==BGBDYC_ARCH_X86)
	{
		switch(i)
		{
		case BGBDYC_FLAG_CDECL: c='c'; break;
		case BGBDYC_FLAG_STDCALL: c='w'; break;
		default: c=0; break;
		}
		if(c)return(c);
	}
	if(DYLL_GetArch()==BGBDYC_ARCH_X64)
	{
		switch(i)
		{
		case BGBDYC_FLAG_WIN64: c='w'; break;
		case BGBDYC_FLAG_AMD64: c='f'; break;
		default: c=0; break;
		}
		if(c)return(c);
	}

	switch(i)
	{
	case BGBDYC_FLAG_ABSTRACT: c='a'; break;
	case BGBDYC_FLAG_BIGENDIAN: c='b'; break;
	//c (cdecl)
	case BGBDYC_FLAG_DELEGATE: c='d'; break;
	case BGBDYC_FLAG_EVENT: c='e'; break;
	//f (fastcall/amd64)
	case BGBDYC_FLAG_GETTER: c='g'; break;
	case BGBDYC_FLAG_SETTER: c='h'; break;
	case BGBDYC_FLAG_INLINE: c='i'; break;
	case BGBDYC_FLAG_FINAL: c='j'; break;
	case BGBDYC_FLAG_CONST: c='k'; break;
	case BGBDYC_FLAG_LTLENDIAN: c='l'; break;
	//m
	case BGBDYC_FLAG_NATIVE: c='n'; break;
	case BGBDYC_FLAG_OVERRIDE: c='o'; break;
	case BGBDYC_FLAG_PUBLIC: c='p'; break;
	case BGBDYC_FLAG_PRIVATE: c='q'; break;
	case BGBDYC_FLAG_PROTECTED: c='r'; break;
	case BGBDYC_FLAG_STATIC: c='s'; break;
	//t (thiscall)
	//u (special)
	case BGBDYC_FLAG_VIRTUAL: c='v'; break;
	//w (stdcall/win64)
	case BGBDYC_FLAG_XCALL: c='x'; break;
	case BGBDYC_FLAG_TRANSIENT: c='y'; break;
	//z

//	case BGBDYC_FLAG_TRANSIENT: c=('C'<<8)|'t'; break;

	case BGBDYC_FLAG_THREAD: c=('D'<<8)|'t'; break;
	default: c=0; break;
	}
	return(c);
}

char *DYLL_FlagSig_FlattenModifiers2(char *t, int fl)
{
	int i, j, k;

	for(i=0; i<31; i++)
	{
		j=DYLL_FlagSig_ModifierChar(fl&(1<<i));
		if(j)
		{
			if((j>>24)&0xFF)*t++=(j>>24)&0xFF;
			if((j>>16)&0xFF)*t++=(j>>16)&0xFF;
			if((j>> 8)&0xFF)*t++=(j>> 8)&0xFF;
			if(j&0xFF)*t++=j&0xFF;
		}
	}
	return(t);
}

char *DYLL_VarTypeFlagsString(s64 fl)
{
	char buf[256];
	char *s, *t, *t1;

	if(!fl)return(NULL);

	t=buf;
	t=DYLL_FlagSig_FlattenModifiers2(t, fl);
	*t=0;

	return(dyllString(buf));
}

BGBDY_API char *dyllEncodeFlagSig(s64 fl)
{
	return(DYLL_VarTypeFlagsString(fl));
}
