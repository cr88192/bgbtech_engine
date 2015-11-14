#include <bgbgc.h>
#include <bgbdy.h>

char *dyll_SigReadQName(char **sig)
{
	char buf[256];
	char *s, *t;

	s=*sig; t=buf;
	while(*s && (*s!=';'))
		*t++=*s++;
	if(*s==';')s++;
	*t++=0;

	*sig=s;

	return(gcrstrdup(buf));
}

BGBDY_API char *dyllSigGetStructName(char *sig)
{
	char *s;
	
	s=sig;
	while(*s=='P')s++;
	if(*s=='X')
		{ s++; return(dyll_SigReadQName(&s)); }
	if(*s=='L')
		{ s++; return(dyll_SigReadQName(&s)); }
	if(*s=='U')
		{ s++; return(dyll_SigReadQName(&s)); }
	return(NULL);
}

BGBDY_API int dyllSigQuickSize(char *sig)
{
	char *s, *s1;
	int i, j, sz;

	sz=sizeof(void *);

	s=sig;
	switch(*s++)
	{
	case 'a': case 'c': case 'h': sz=sizeof(char); break;
	case 's': case 't': case 'w': sz=sizeof(short); break;
	case 'b': case 'i': case 'j': sz=sizeof(int); break;
	case 'l': case 'm': sz=sizeof(long); break;
	case 'f': sz=sizeof(float); break;
	case 'd': sz=sizeof(double); break;
	case 'e': sz=sizeof(long double); break;
	case 'k': sz=sizeof(short); break;

	case 'g': sz=16; break;
	case 'n': case 'o': sz=16; break;

	case 'r': sz=sizeof(dyt); break;

	case 'x': case 'y': sz=sizeof(long long); break;

	case 'P': sz=sizeof(void *); break;
	case 'L': sz=sizeof(dyt); break;
	case 'Q': sz=sizeof(dyt); break;

	case 'A':
		if((*s>='0') && (*s<='9'))
		{
			s1=dyll_SigReadQName(&s);
			i=1;
			while(*s1)
			{
				j=atoi(s1);
				if(j>0)i*=j;
				while(*s1 && (*s1!=','))s1++;
				if(*s1==',')s1++;
			}
			j=dyllSigQuickSize(s);
			sz=i*j;
			break;
		}
		break;

	case 'X':
	case 'U':
		s1=dyll_SigReadQName(&s);
		s1=dyllTypeGetMetaPacksig(s1);
		if(s1)
		{
			sz=dyllSigSizeList(s1);
		}else sz=0;
		break;

	case 'C':
		switch(*s++)
		{
		case 'a': sz=sizeof(vec2); break;
		case 'b': sz=sizeof(vec3); break;
		case 'c': sz=sizeof(vec4); break;
		case 'q': sz=sizeof(quat); break;

		case 'f': sz=sizeof(fcomplex); break;
		case 'd': sz=sizeof(dcomplex); break;
		case 'r': sz=sizeof(dytf); break;

		case '2': case '3': case '4': case '5':
		case '6': case '7': case '8': case '9':
		case 'x': case 'y': case 'z':
			sz=sizeof(dyt); break;
		}
		break;

	case 'D':
		switch(*s++)
		{
		case 'a': sz=sizeof(vec2d); break;
		case 'b': sz=sizeof(vec3d); break;
		case 'c': sz=sizeof(vec4d); break;
		case 'q': sz=sizeof(quatd); break;

		case 'f': sz=sizeof(fcomplex); break;
		case 'd': sz=sizeof(dcomplex); break;
		case 'r': sz=sizeof(dytf); break;

		case '2': case '3': case '4': case '5':
		case '6': case '7': case '8': case '9':
		case 'x': case 'y': case 'z':
			sz=sizeof(dyt); break;
		}
		break;

	default: break;
	}

	return(sz);
}

BGBDY_API int dyllSigAlignSize(char *sig)
{
	int sz, al;
	dyllSigSizeAlign(sig, &sz, &al);
	if(sz%al)sz+=al-(sz%al);
	return(sz);
}

BGBDY_API int dyllSigAlign(char *sig)
{
	int sz, al;
	dyllSigSizeAlign(sig, &sz, &al);
	return(al);
}

BGBDY_API char *dyllSigSizeAlign(char *sig, int *rsz, int *ral)
{
	char *s, *s1;
	int sz, al;
	int i, j;

	sz=sizeof(void *); al=sz;

	s=sig;
	switch(*s++)
	{
	case 'a': case 'c': case 'h': sz=sizeof(char); al=sz; break;
	case 's': case 't': case 'w': sz=sizeof(short); al=sz; break;
	case 'b': case 'i': case 'j': sz=sizeof(int); al=sz; break;
	case 'l': case 'm': sz=sizeof(long); al=sz; break;
	case 'f': sz=sizeof(float); al=sz; break;
	case 'd': sz=sizeof(double); al=sz; break;
	case 'e': sz=sizeof(long double); al=16; break;
	case 'k': sz=sizeof(short); al=sz; break;

	case 'g': sz=16; al=sz; break;
	case 'n': case 'o': sz=16; al=sz; break;

	case 'r': sz=sizeof(dyt); al=sz; break;

	case 'x': case 'y': sz=sizeof(long long); al=sz; break;

	case 'A':
		if((*s>='0') && (*s<='9'))
		{
			s1=dyll_SigReadQName(&s);
			i=1;
			while(*s1)
			{
				j=atoi(s1);
				if(j>0)i*=j;
				while(*s1 && (*s1!=','))s1++;
				if(*s1==',')s1++;
			}
			s=dyllSigSizeAlign(s, &sz, &al);
			sz=i*sz;
			break;
		}

		s=dyllSigSizeAlign(s, &sz, &al);
		break;

	case 'L':
		sz=sizeof(dyt); al=sz;
		while(*s && *s!=';')s++;
		if(*s==';')s++;
		break;

	case 'C':
		switch(*s++)
		{
//#if 1
#ifdef X86_64
		case 'a': sz=sizeof(vec2); al=sz; break;
		case 'b': sz=sizeof(vec3); al=sz; break;
		case 'c': sz=sizeof(vec4); al=sz; break;
		case 'q': sz=sizeof(quat); al=sz; break;

		case 'f': sz=sizeof(fcomplex); al=sz; break;
		case 'd': sz=sizeof(dcomplex); al=sz; break;
#endif

//#if 0
#ifdef X86
		case 'a': sz=sizeof(vec2); al=4; break;
		case 'b': sz=sizeof(vec3); al=4; break;
		case 'c': sz=sizeof(vec4); al=4; break;
		case 'q': sz=sizeof(quat); al=4; break;

		case 'f': sz=sizeof(fcomplex); al=4; break;
		case 'd': sz=sizeof(dcomplex); al=8; break;
#endif

		case 'r': sz=sizeof(dytf); al=sz; break;

		case 'x': case 'y': case 'z':
			sz=sizeof(dyt); al=sz; break;
		case '2': case '3': case '4': case '5':
		case '6': case '7': case '8': case '9':
			sz=sizeof(dyt); al=sz;
			s=dyllSigNext(s); break;
		}
		break;

	case 'D':
		switch(*s++)
		{
#ifdef X86_64
		case 'a': sz=sizeof(vec2d); al=sz; break;
		case 'b': sz=sizeof(vec3d); al=sz; break;
		case 'c': sz=sizeof(vec4d); al=sz; break;
		case 'q': sz=sizeof(quatd); al=sz; break;

		case 'f': sz=sizeof(fcomplex); al=sz; break;
		case 'd': sz=sizeof(dcomplex); al=sz; break;
#endif

#ifdef X86
		case 'a': sz=sizeof(vec2d); al=8; break;
		case 'b': sz=sizeof(vec3d); al=8; break;
		case 'c': sz=sizeof(vec4d); al=8; break;
		case 'q': sz=sizeof(quatd); al=8; break;

		case 'f': sz=sizeof(fcomplex); al=4; break;
		case 'd': sz=sizeof(dcomplex); al=8; break;
#endif

		case 'r': sz=sizeof(dytf); al=sz; break;

		case 'x': case 'y': case 'z':
			sz=sizeof(dyt); al=sz; break;
		case '2': case '3': case '4': case '5':
		case '6': case '7': case '8': case '9':
			sz=sizeof(dyt); al=sz;
			s=dyllSigNext(s); break;
		}
		break;

	case 'P':
		sz=sizeof(void *); al=sz;
		while(*s=='P')s++;
		s=dyllSigNext(s);
		break;

	case 'Q':
		sz=sizeof(void *); al=sz;
		while(*s=='Q')s++;
		s=dyllSigNext(s);
		break;

	case 'X':
	case 'U':
		s1=dyll_SigReadQName(&s);
		s1=dyllTypeGetMetaPacksig(s1);
		if(s1)
		{
			sz=dyllSigSizeListEsig(s1, NULL, &al);
		}
		break;

	default: s=NULL; break;
	}

	if(rsz)*rsz=sz;
	if(ral)*ral=al;
	return(s);
}

BGBDY_API char *dyllSigNext(char *sig)
{
	char *s;
	int i, sz, al;

	sz=sizeof(void *); al=sz;

	s=sig;
	switch(*s++)
	{
	case 'a': case 'c': case 'h': break;
	case 's': case 't': case 'w': break;
	case 'b': case 'i': case 'j': break;
	case 'l': case 'm': break;
	case 'f': break;
	case 'd': break;
	case 'e': break;
	case 'k': break;

	case 'g': break;
	case 'n': case 'o': break;

	case 'r': break;

	case 'x': case 'y': break;
	case 'z': break;

	case 'u':
		while(*s && *s!=';')s++;
		if(*s==';')s++;
		break;

	case 'C':
		switch(*s++)
		{
		case 'f': break;
		case 'd': break;
		case 'e': break;
		case 'g': break;
		case 'k': break;
		case 'r': break;

		case 'x': break;
		case 'y': break;
		case 'z': break;

		case 'u':
			while(*s && *s!=';')s++;
			if(*s==';')s++;
			break;

		case '2': case '3': case '4': case '5':
		case '6': case '7': case '8': case '9':
			s=dyllSigNext(s); break;
		default: break;
		}
		break;

	case 'D':
	case 'G':
		if(*s=='u')
		{
			while(*s && *s!=';')s++;
			if(*s==';')s++;
			break;
		}
		if((*s>='a') && (*s<='z'))s++;
		break;

	case 'P':
		while(*s=='P')s++;
		s=dyllSigNext(s);
		break;

	case 'U':
	case 'X':
	case 'L':
	case 'M':
	case 'N':
		while(*s && *s!=';')s++;
		if(*s==';')s++;
		break;

	case 'Q':
		while(*s=='Q')s++;
		s=dyllSigNext(s);
		break;

	case 'A':
		if((*s>='0') && (*s<='9'))
		{
			while(*s && *s!=';')s++;
			if(*s==';')s++;
			s=dyllSigNext(s);
			break;
		}
		if((*s>='A') && (*s<='D'))s++;
		if(*s=='u')
		{
			while(*s && *s!=';')s++;
			if(*s==';')s++;
			break;
		}
		if((*s>='a') && (*s<='z'))s++;
		break;

	case 'F':
		switch(*s++)
		{
		case 'u':
			while(*s && *s!=';')s++;
			if(*s==';')s++;
			break;
		default: break;
		}
		s=dyllSigNext(s);
		break;

	case '(':
		i=1;
		while(i && *s)
		{
			switch(*s++)
			{
			case ')': i--; break;
			case '(': i++; break;
			default: break;
			}
		}
		s=dyllSigNext(s);
		break;

	default: s=NULL; break;
	}

	return(s);
}

BGBDY_API char *dyllSigEsig(char *sig)
{
	char *s;
//	dyllSigSizeListEsig(sig, &s, NULL);
//	return(s);
	return(dyllSigRet(sig));
}

BGBDY_API int dyllSigSizeListEsig(char *sig, char **esig, int *algn)
{
	char *s;
	int tsz, tal, sz, al;

	s=sig; tsz=0; tal=1;
	if(*s=='(')s++;
	while(*s && (*s!=')'))
	{
		dyllSigSizeAlign(s, &sz, &al);
		if(tsz%al)tsz+=al-(tsz%al);
		tsz+=sz;
		if(al>tal)tal=al;

		s=dyllSigNext(s);
	}
	if(tsz%tal)tsz+=tal-(tsz%tal);
	if(esig)*esig=s;
	if(algn)*algn=tal;
	return(tsz);
}

BGBDY_API int dyllSigSizeList(char *sig)
	{ return(dyllSigSizeListEsig(sig, NULL, NULL)); }


BGBDY_API dytf dyllExtractVariantBufferSigB(char *esig, byte *buf)
{
	void *pv;
	dytf p;

	switch(*esig)
	{
	case 0: case 'v': p=DYTF_NULL; break;

	case 'a': case 'c':
		p=dytfInt(*(s8 *)buf); break;
	case 'b':
		p=dytfBool(*(s8 *)buf); break;
	case 'h':
		p=dytfInt(*(u8 *)buf); break;
	case 's':
		p=dytfInt(*(s16 *)buf); break;
	case 't':
	case 'w':
		p=dytfInt(*(u16 *)buf); break;
	case 'i':
		p=dytfInt(*(s32 *)buf); break;
	case 'j':
		p=dytfInt(*(u32 *)buf); break;
	case 'l':
		p=dytfLong(*(long *)buf); break;
	case 'm':
		p=dytfLong(*(unsigned long *)buf); break;
	case 'x':
		p=dytfLong(*(s64 *)buf); break;
	case 'y':
		p=dytfLong(*(u64 *)buf); break;

	case 'd': p=dytfDouble(*(double *)buf); break;
	case 'e': p=dytfDouble(*(long double *)buf); break;
	case 'f': p=dytfFloat(*(float *)buf); break;
	case 'k': p=dytfFloat(*(float *)buf); break;

	case 'P':
//		if(esig[1]=='c')
//			{ p=dystring(*(char **)buf); break; }
//		p=*(dyt *)buf;

		if(esig[1]=='c')
		{
			pv=*(void **)buf;
			p=dytfPtrF(pv);
			if(!dygettype(pv))
				p=dytfString((char *)pv);
			break;
		}

		pv=*(void **)buf;
		p=dytfPtrF(pv);
		if(!dygettype((dyt)pv))
		{
//			p=dyllTypeboxPtr(esig, p);
			p=dytfPtrF(dyllTypeboxPtrChkFcn(esig, pv));
		}
		break;

	case 'r':
		p=dytfWrap(*(dyt *)buf);
		break;
	case 'L':
		p=dytfPtrF(*(dyt *)buf);
		break;

	case 'A':
		switch(esig[1])
		{
		case '0': case '1':
		case '2': case '3': case '4': case '5':
		case '6': case '7': case '8': case '9':
			p=dytfPtrF(*(dyt *)buf); break;
		default: p=DYTF_UNDEFINED; break;
		}
		break;

	case 'C':
		switch(esig[1])
		{
		case 'a': p=dytfPtrF(dywvec2(*(vec2 *)buf)); break;
		case 'b': p=dytfPtrF(dywvec3(*(vec3 *)buf)); break;
		case 'c': p=dytfPtrF(dywvec4(*(vec4 *)buf)); break;
		case 'q': p=dytfPtrF(dywquat(*(quat *)buf)); break;

		case 'f': p=dytfPtrF(dyfcomplex(*(fcomplex *)buf)); break;
		case 'd': p=dytfPtrF(dydcomplex(*(dcomplex *)buf)); break;
		case 'r': p=*(dytf *)buf; break;

		case 'x': case 'y': case 'z':
			p=dytfWrap(*(dyt *)buf); break;

		case '2': case '3': case '4': case '5':
		case '6': case '7': case '8': case '9':
			p=dytfPtrF(*(dyt *)buf); break;
		default: p=DYTF_UNDEFINED; break;
		}
		break;

	case 'D':
		switch(esig[1])
		{
		case 'a': p=dytfPtrF(dywvec2d(*(vec2d *)buf)); break;
		case 'b': p=dytfPtrF(dywvec3d(*(vec3d *)buf)); break;
		case 'c': p=dytfPtrF(dywvec4d(*(vec4d *)buf)); break;
		case 'q': p=dytfPtrF(dywquatd(*(quatd *)buf)); break;

		default: p=DYTF_UNDEFINED; break;
		}
		break;


//	default: p=NULL; break;

	default:
//		p=dyllGetVarSig(buf, esig);
		p=dytfWrap(dyllGetVarSigFlags(buf, esig, 0));
		break;

	}
	return(p);
}

BGBDY_API dyt dyllExtractVariantBufferSig(char *esig, byte *buf)
{
	dyt p;

	switch(*esig)
	{
	case 0: case 'v': p=NULL; break;

	case 'a': case 'c':
		p=dyint(*(s8 *)buf); break;
	case 'b':
		p=dybool(*(s8 *)buf); break;
	case 'h':
		p=dyint(*(u8 *)buf); break;
	case 's':
		p=dyint(*(s16 *)buf); break;
	case 't':
	case 'w':
		p=dyint(*(u16 *)buf); break;
	case 'i':
		p=dyint(*(s32 *)buf); break;
	case 'j':
		p=dyint(*(u32 *)buf); break;
	case 'l':
		p=dylong(*(long *)buf); break;
	case 'm':
		p=dylong(*(unsigned long *)buf); break;
	case 'x':
		p=dylong(*(s64 *)buf); break;
	case 'y':
		p=dylong(*(u64 *)buf); break;

	case 'd': p=dydouble(*(double *)buf); break;
	case 'e': p=dydouble(*(long double *)buf); break;
	case 'f': p=dyfloat(*(float *)buf); break;
	case 'k': p=dyflonum(*(float *)buf); break;

	case 'P':
//		if(esig[1]=='c')
//			{ p=dystring(*(char **)buf); break; }
//		p=*(dyt *)buf;

		if(esig[1]=='c')
		{
			p=(dyt)(*(char **)buf);
			if(!dygettype(p))
				p=(dyt)dystring((char *)p);
			break;
		}

		p=*(void **)buf;
		if(!dygettype((dyt)p))
		{
//			p=dyllTypeboxPtr(esig, p);
			p=dyllTypeboxPtrChkFcn(esig, p);
		}
		break;

	case 'r': case 'L':
		p=*(dyt *)buf; break;

	case 'C':
		switch(esig[1])
		{
		case 'a': p=dywvec2(*(vec2 *)buf); break;
		case 'b': p=dywvec3(*(vec3 *)buf); break;
		case 'c': p=dywvec4(*(vec4 *)buf); break;
		case 'q': p=dywquat(*(quat *)buf); break;

		case 'f': p=dyfcomplex(*(fcomplex *)buf); break;
		case 'd': p=dydcomplex(*(dcomplex *)buf); break;
		case 'r': p=dytfUnwrap(*(dytf *)buf); break;

		case 'x': case 'y': case 'z':
			p=*(dyt *)buf; break;

		case '2': case '3': case '4': case '5':
		case '6': case '7': case '8': case '9':
			p=*(dyt *)buf; break;
		default: p=NULL; break;
		}
		break;

	case 'D':
		switch(esig[1])
		{
		case 'a': p=dywvec2d(*(vec2d *)buf); break;
		case 'b': p=dywvec3d(*(vec3d *)buf); break;
		case 'c': p=dywvec4d(*(vec4d *)buf); break;
		case 'q': p=dywquatd(*(quatd *)buf); break;

		default: p=NULL; break;
		}
		break;


//	default: p=NULL; break;

	default:
//		p=dyllGetVarSig(buf, esig);
		p=dyllGetVarSigFlags(buf, esig, 0);
		break;

	}
	return(p);
}

BGBDY_API dyt dyllGetVarSig(void *p, char *sig)
{
	return(dyllExtractVariantBufferSig(sig, p));
//	return(dyllGetVarSigFlags(p, sig, 0));
}

BGBDY_API dyt dyllGetVarSigFlags(void *p, char *sig, s64 fl)
{
	void *q;

	if(!strcmp(sig, "a"))return(dyint(*(signed char *)p));
	if(!strcmp(sig, "b"))return(dybool(*(char *)p));
	if(!strcmp(sig, "c"))return(dyint(*(char *)p));
	if(!strcmp(sig, "d"))return(dydouble(*(double *)p));
	if(!strcmp(sig, "e"))return(dydouble(*(long double *)p));
	if(!strcmp(sig, "f"))return(dyfloat(*(float *)p));
	//g=float128
	if(!strcmp(sig, "h"))return(dyint(*(unsigned char *)p));
	if(!strcmp(sig, "i"))return(dyint(*(int *)p));
	if(!strcmp(sig, "j"))return(dyint(*(unsigned int *)p));
	//k=float16
	if(!strcmp(sig, "l"))return(dylong(*(long *)p));
	if(!strcmp(sig, "m"))return(dylong(*(unsigned long *)p));
	//n=int128, o=uint128
	//p,q=reserved
	if(!strcmp(sig, "r"))return(*(dyt *)p);
	if(!strcmp(sig, "s"))return(dyint(*(short *)p));
	if(!strcmp(sig, "t"))return(dyint(*(unsigned short *)p));
	//u=bad
	if(!strcmp(sig, "v"))return(NULL);
	if(!strcmp(sig, "w"))return(dyint(*(short *)p));
	if(!strcmp(sig, "x"))return(dylong(*(long long *)p));
	if(!strcmp(sig, "y"))return(dylong(*(unsigned long long *)p));

	if(!strcmp(sig, "Cd"))return(dydcomplex(*(dcomplex *)p));
	if(!strcmp(sig, "Cf"))return(dyfcomplex(*(fcomplex *)p));
	if(!strcmp(sig, "Cr"))return(dytfUnwrap(*(dytf *)p));

	if(!strcmp(sig, "Cx"))return(*(dyt *)p);
	if(!strcmp(sig, "Cy"))return(*(dyt *)p);
	if(!strcmp(sig, "Cz"))return(*(dyt *)p);

	if(!strcmp(sig, "Ca"))return(dywvec2(*(vec2 *)p));
	if(!strcmp(sig, "Cb"))return(dywvec3(*(vec3 *)p));
	if(!strcmp(sig, "Cc"))return(dywvec4(*(vec4 *)p));
	if(!strcmp(sig, "Cq"))return(dywquat(*(quat *)p));

	if(!strcmp(sig, "Da"))return(dywvec2d(*(vec2d *)p));
	if(!strcmp(sig, "Db"))return(dywvec3d(*(vec3d *)p));
	if(!strcmp(sig, "Dc"))return(dywvec4d(*(vec4d *)p));
	if(!strcmp(sig, "Dq"))return(dywquatd(*(quatd *)p));

	if(*sig=='P')
	{
		if(sig[1]=='c')
		{
			q=*(char **)p;
			if(!dygettype(q))
				q=(dyt)dystring(q);
			if(dycArrayDataP((dyt)q))
				q=(void *)dycArrayFromData((dyt)q);

			return(q);
		}

		q=*(void **)p;
		if(!q)return(q);
		if(q==UNDEFINED)return(q);
		
		if(!dygettype((dyt)q))
		{
//			return(dyllTypeboxPtr(sig, q));
			return(dyllTypeboxPtrChkFcn(sig, q));
		}

		if(dycArrayDataP((dyt)q))
			q=(void *)dycArrayFromData((dyt)q);

		if(fl&BGBDYC_FLAG_DYNAMIC)
			return((dyt)q);

//		return(dyllTypeboxPtr(sig, q));
		return(dyllTypeboxPtrChkFcn(sig, q));
//		return((dyt)q);
	}

	if(*sig=='U')
	{
		if(!strcmp(sig, "Uvec2;"))
			return(dyvec2fv(p));
		if(!strcmp(sig, "Uvec3;"))
			return(dyvec3fv(p));
		if(!strcmp(sig, "Uvec4;"))
			return(dyvec4fv(p));
		if(!strcmp(sig, "Uquat;"))
			return(dyquatfv(p));
	}

	if(*sig=='X')
	{
		return(dyllTypeboxVal(sig, p));
	}

	if(*sig=='(')
	{
		q=dyRawFunc(sig, p);
		return(q);
	}

	if(*sig=='L')return(*(dyt *)p);
	if(*sig=='Q')return(*(dyt *)p);
	if(*sig=='C')return(*(dyt *)p);

	return(NULL);
}

BGBDY_API void dyllSetVarSig(void *var, char *sig, dyt val)
{
	switch(*sig)
	{
	case 'a':
		*(signed char *)var=dyintv(val);
		break;
	case 'b':
		*(signed char *)var=dytruep(val);
		break;
	case 'c':
		*(signed char *)var=dyintv(val);
		break;
	case 'd':
		*(double *)var=dydoublev(val);
		break;
	case 'e':
		*(long double *)var=dydoublev(val);
		break;
	case 'f':
		*(float *)var=dyfloatv(val);
		break;

	//g=float128

	case 'h':
		*(unsigned char *)var=dyintv(val);
		break;
	case 'i':
		*(signed int *)var=dyintv(val);
		break;
	case 'j':
		*(unsigned int *)var=dyintv(val);
		break;

	//k=float16

	case 'l':
		*(signed long *)var=dylongv(val);
		break;
	case 'm':
		*(unsigned long *)var=dylongv(val);
		break;

	case 'r':
		*(dyt *)var=val;	
		break;
	case 's':
		*(short *)var=dyintv(val);
		break;
	case 't':
		*(unsigned short *)var=dyintv(val);
		break;
	case 'w':
		*(short *)var=dyintv(val);
		break;
	case 'x':
		*(long long *)var=dylongv(val);
		break;
	case 'y':
		*(unsigned long long *)var=dylongv(val);
		break;

	default:
		dyllSetVarSig2(var, sig, val);
		break;
	}
}

BGBDY_API void dyllSetVarSig2(void *var, char *sig, dyt val)
{
	void *p, *p1;

	if(!strcmp(sig, "a"))*(signed char *)var=dyintv(val);
	if(!strcmp(sig, "b"))*(signed char *)var=dytruep(val);
	if(!strcmp(sig, "c"))*(signed char *)var=dyintv(val);
	if(!strcmp(sig, "d"))*(double *)var=dydoublev(val);
	if(!strcmp(sig, "e"))*(long double *)var=dydoublev(val);
	if(!strcmp(sig, "f"))*(float *)var=dyfloatv(val);
	//g=float128
	if(!strcmp(sig, "h"))*(unsigned char *)var=dyintv(val);
	if(!strcmp(sig, "i"))*(signed int *)var=dyintv(val);
	if(!strcmp(sig, "j"))*(unsigned int *)var=dyintv(val);
	//k=float16
	if(!strcmp(sig, "l"))*(signed long *)var=dylongv(val);
	if(!strcmp(sig, "m"))*(unsigned long *)var=dylongv(val);
	//n=int128, o=uint128
	//p,q=reserved
	if(!strcmp(sig, "r"))*(dyt *)var=val;	
	if(!strcmp(sig, "s"))*(short *)var=dyintv(val);
	if(!strcmp(sig, "t"))*(unsigned short *)var=dyintv(val);
	//u=bad
	if(!strcmp(sig, "w"))*(short *)var=dyintv(val);
	if(!strcmp(sig, "x"))*(long long *)var=dylongv(val);
	if(!strcmp(sig, "y"))*(unsigned long long *)var=dylongv(val);

	if(!strcmp(sig, "Cd"))*(dcomplex *)var=dydcomplexv(val);
	if(!strcmp(sig, "Cf"))*(fcomplex *)var=dyfcomplexv(val);
	if(!strcmp(sig, "Cr"))*(dytf *)var=dytfWrap(val);

	if(!strcmp(sig, "Cx"))*(dyt *)var=val;	
	if(!strcmp(sig, "Cy"))*(dyt *)var=val;	
	if(!strcmp(sig, "Cz"))*(dyt *)var=val;	

	if(!strcmp(sig, "Ca"))*(vec2 *)var=dyvec2v(val);
	if(!strcmp(sig, "Cb"))*(vec3 *)var=dyvec3v(val);
	if(!strcmp(sig, "Cc"))*(vec4 *)var=dyvec4v(val);
	if(!strcmp(sig, "Cq"))*(quat *)var=dyquatv(val);

	if(!strcmp(sig, "Da"))*(vec2d *)var=dyvec2vd(val);
	if(!strcmp(sig, "Db"))*(vec3d *)var=dyvec3vd(val);
	if(!strcmp(sig, "Dc"))*(vec4d *)var=dyvec4vd(val);
	if(!strcmp(sig, "Dq"))*(quatd *)var=dyquatvd(val);

	if(*sig=='P')
	{
		if(dyllTypeboxPtrP(val))
		{
			p=dyllTypeboxPtrData(val);
			*(void **)var=p;
			return;
		}
		if(dyllTypeboxValP(val))
		{
			p=dyllTypeboxValData(val);
			*(void **)var=*(void **)p;
			return;
		}
		if(dycArrayP(val))
		{
#if 0
			if(sig[1]=='v')
			{
				*(void **)var=(void *)val;
				return;
			}
#endif

			//HACK...
			if(!strcmp(sig, "PXBGBDYC_Array_s;"))
			{
				*(void **)var=(void *)val;
				return;
			}

			p=dycArrayV((dycArray)val);
			*(void **)var=p;
			return;
		}
		*(void **)var=(void *)val;
		return;
	}
	if(*sig=='L')*(dyt *)var=val;	
	if(*sig=='Q')*(dyt *)var=val;	

	if(*sig=='U')
	{
		if(!strcmp(sig, "Uvec2;"))
			{ dyvecv2fv(val, var); return; }
		if(!strcmp(sig, "Uvec3;"))
			{ dyvecv3fv(val, var); return; }
		if(!strcmp(sig, "Uvec4;"))
			{ dyvecv4fv(val, var); return; }
		if(!strcmp(sig, "Uquat;"))
			{ dyquatvfv(val, var); return; }
	}

	return;
}




static double BGBGC_DecodeHalf(unsigned short val)
	{ return(0); }
static unsigned short BGBGC_EncodeHalf(double val)
	{ return(0); }

BGBDY_API s64 dyllSigGetPtrS64(char *sig, void *ptr)
{
	s64 val;
	char *s;

	s=sig;
	switch(*s++)
	{
	case 'a': case 'c': val=*(char *)ptr; break;
	case 'h': val=*(unsigned char *)ptr; break;

	case 's': val=*(short *)ptr; break;
	case 't': case 'w': val=*(unsigned short *)ptr; break;

	case 'b': case 'i': val=*(int *)ptr; break;
	case 'j': val=*(unsigned int *)ptr; break;

	case 'l': val=*(long *)ptr; break;
	case 'm': val=*(unsigned long *)ptr; break;

	case 'f': val=*(float *)ptr; break;
	case 'd': val=*(double *)ptr; break;
	case 'e': val=*(long double *)ptr; break;
	case 'k': val=BGBGC_DecodeHalf(*(unsigned short *)ptr); break;

	case 'g': val=0; break; //float128
	case 'n': case 'o':
		val=*(s64 *)ptr; break; //cheap trick: low half of int128

	case 'r': val=dylongv(*(dyt *)ptr); break;

	case 'x': case 'y': val=*(s64 *)ptr; break;

	case 'C':
		switch(*s++)
		{
		case 'f': val=((fcomplex *)ptr)->r; break;
		case 'd': val=((dcomplex *)ptr)->r; break;
		case 'r': val=dytfRealv(*(dytf *)ptr); break;
		}
		break;
	case 'P':
		val=0;

		while(*s=='P')s++;
		if((*s>='a') && (*s<='z'))
			{ s++; break; }
		if(*s=='X')
		{
			while(*s && *s!=';')s++;
			if(*s==';')s++;
		}
		break;

	default: s=NULL; val=0; break;
	}

	return(val);
}

BGBDY_API double dyllSigGetPtrDouble(char *sig, void *ptr)
{
	double val;
	char *s;

	s=sig;
	switch(*s++)
	{
	case 'a': case 'c': val=*(char *)ptr; break;
	case 'h': val=*(unsigned char *)ptr; break;

	case 's': val=*(short *)ptr; break;
	case 't': case 'w': val=*(unsigned short *)ptr; break;

	case 'b': case 'i': val=*(int *)ptr; break;
	case 'j': val=*(unsigned int *)ptr; break;

	case 'l': val=*(long *)ptr; break;
	case 'm': val=*(unsigned long *)ptr; break;

	case 'f': val=*(float *)ptr; break;
	case 'd': val=*(double *)ptr; break;
	case 'e': val=*(long double *)ptr; break;
	case 'k': val=BGBGC_DecodeHalf(*(unsigned short *)ptr); break;

	case 'g': val=0; break; //float128
	case 'n': case 'o':
		val=*(s64 *)ptr; break; //cheap trick: low half of int128

	case 'r': val=dydoublev(*(dyt *)ptr); break;

	case 'x': val=*(long long *)ptr; break;
	case 'y': val=*(unsigned long long *)ptr; break;

	case 'C':
		switch(*s++)
		{
		case 'f': val=((fcomplex *)ptr)->r; break;
		case 'd': val=((dcomplex *)ptr)->r; break;
		case 'r': val=dytfRealv(*(dytf *)ptr); break;
		}
		break;
	case 'P':
		val=0;

		while(*s=='P')s++;
		if((*s>='a') && (*s<='z'))
			{ s++; break; }
		if(*s=='X')
		{
			while(*s && *s!=';')s++;
			if(*s==';')s++;
		}
		break;

	default: s=NULL; val=0; break;
	}

	return(val);
}

BGBDY_API void dyllSigSetPtrS64(char *sig, void *ptr, s64 val)
{
	char *s;

	s=sig;
	switch(*s++)
	{
	case 'a': case 'c': *(char *)ptr=val; break;
	case 'h': *(unsigned char *)ptr=val; break;

	case 's': *(short *)ptr=val; break;
	case 't': case 'w': *(unsigned short *)ptr=val; break;

	case 'b': case 'i': *(int *)ptr=val; break;
	case 'j': *(unsigned int *)ptr=val; break;

	case 'l': *(long *)ptr=val; break;
	case 'm': *(unsigned long *)ptr=val; break;

	case 'f': *(float *)ptr=val; break;
	case 'd': *(double *)ptr=val; break;
	case 'e': *(long double *)ptr=val; break;
//	case 'k': val=BGBGC_DecodeHalf(*(unsigned short *)ptr); break;

	case 'g': break; //float128
	case 'n': case 'o':
		*(s64 *)ptr=val; break; //cheap trick: low half of int128

	case 'r': *(dyt *)ptr=dylong(val); break;

	case 'x': case 'y': *(s64 *)ptr=val; break;

	case 'C':
		switch(*s++)
		{
		case 'f': ((fcomplex *)ptr)->r=val; ((fcomplex *)ptr)->i=0; break;
		case 'd': ((dcomplex *)ptr)->r=val; ((dcomplex *)ptr)->i=0; break;
		case 'r': *(dytf *)ptr=dytfReal(val); break;
		}
		break;
	case 'P':
		val=0;
		break;

	default: break;
	}
}

BGBDY_API void dyllSigSetPtrDouble(char *sig, void *ptr, double val)
{
	char *s;

	s=sig;
	switch(*s++)
	{
	case 'a': case 'c': *(char *)ptr=val; break;
	case 'h': *(unsigned char *)ptr=val; break;

	case 's': *(short *)ptr=val; break;
	case 't': case 'w': *(unsigned short *)ptr=val; break;

	case 'b': case 'i': *(int *)ptr=val; break;
	case 'j': *(unsigned int *)ptr=val; break;

	case 'l': *(long *)ptr=val; break;
	case 'm': *(unsigned long *)ptr=val; break;

	case 'f': *(float *)ptr=val; break;
	case 'd': *(double *)ptr=val; break;
	case 'e': *(long double *)ptr=val; break;
//	case 'k': val=BGBGC_DecodeHalf(*(unsigned short *)ptr); break;

	case 'g': break; //float128
	case 'n': case 'o':
		*(s64 *)ptr=val; break; //cheap trick: low half of int128

	case 'r': *(dyt *)ptr=dydouble(val); break;

	case 'x': case 'y': *(s64 *)ptr=val; break;

	case 'C':
		switch(*s++)
		{
		case 'f': ((fcomplex *)ptr)->r=val; ((fcomplex *)ptr)->i=0; break;
		case 'd': ((dcomplex *)ptr)->r=val; ((dcomplex *)ptr)->i=0; break;
		case 'r': *(dytf *)ptr=dytfReal(val); break;
		}
		break;
	case 'P':
		val=0;
		break;

	default: break;
	}
}

BGBDY_API s64 dyllExtractLongBufferSig(char *esig, byte *buf)
{
	s64 i;

	switch(*esig)
	{
	case 0: case 'v': i=0; break;

	case 'a': case 'b': case 'c': case 'h':
	case 's': case 't': case 'w':
	case 'i': case 'j':
		i=*(int *)buf; break;
	case 'l': case 'm': case 'x': case 'y':
		i=*(s64 *)buf; break;
	case 'r': case 'P': case 'L':
		i=dylongv(*(dyt *)buf); break;

	case 'd': i=*(double *)buf; break;
	case 'e': i=*(long double *)buf; break;
	case 'f': i=*(float *)buf; break;
	case 'k': i=*(float *)buf; break;

	case 'C':
		switch(*esig++)
		{
		case 'f': i=dtyFCreal(*(fcomplex *)buf); break;
		case 'd': i=dtyDCreal(*(dcomplex *)buf); break;
		case 'r': i=dytfRealv(*(dytf *)buf); break;
		}
		break;

	default: i=0; break;
	}
	return(i);
}

BGBDY_API double dyllExtractRealBufferSig(char *esig, byte *buf)
{
	double f;

	switch(*esig)
	{
	case 0: case 'v': f=0; break;

	case 'a': case 'b': case 'c': case 'h':
	case 's': case 't': case 'w':
	case 'i': case 'j':
		f=*(int *)buf; break;
	case 'l': case 'm': case 'x': case 'y':
		f=*(s64 *)buf; break;
	case 'r': case 'P': case 'L':
		f=dydoublev(*(dyt *)buf); break;

	case 'd': f=*(double *)buf; break;
	case 'e': f=*(long double *)buf; break;
	case 'f': f=*(float *)buf; break;
	case 'k': f=*(float *)buf; break;

	case 'C':
		switch(*esig++)
		{
		case 'f': f=dtyFCreal(*(fcomplex *)buf); break;
		case 'd': f=dtyDCreal(*(dcomplex *)buf); break;
		case 'r': f=dytfRealv(*(dytf *)buf); break;
		}
		break;

	default: f=0; break;
	}
	return(f);
}

BGBDY_API char *dyllSigFromDytname(char *name)
{
	char tb[256];
	char *s;
	
	sprintf(tb, "bgbdy/%s:sig", name);
	s=dyllMetaLookupKey(tb);
	return(s);
}

BGBDY_API char *dyllSigFromPtr(dyt ref)
{
	char *ty, *ty1;
	
	if(!ref)return("Cv");
	if(ref==UNDEFINED)return("Dv");

	if(dycObjectP(ref))
	{
		ty=dycGetObjClassName((dycObject)ref);

		if(!strcmp(ty, "System/String"))return("Cs");
		if(!strcmp(ty, "java/lang/String"))return("Ct");

		return(gcrsprint("L%s;", ty));
	}

	ty=dygettype(ref);
	if(!ty)return(NULL);
	
	ty1=dyllSigFromDytname(ty);
	if(ty1)return(ty1);

//	if(!strcmp(ty, "_classobj_t"))
//		return("Co");
	return(gcrsprint("Cu%s;", ty));
}

BGBDY_API int dyllSigCheckName(char *sig, char *str)
{
	char tb[256];
	char *ty, *s, *t;
	int ret;

	if(!sig || !str)return(0);

	if(!strcmp(sig, str))return(1);
	
	ret=0;
	switch(sig[0])
	{
	case 'C':
		switch(sig[1])
		{
		case 'o':
			if(!strcmp(str, "object")) { ret=1; break; }
			if(!strcmp(str, "Object")) { ret=1; break; }
			break;
		case 'u':
			s=sig+2; t=tb;
			while(*s && (*s!=';'))*t++=*s++;
			*t++=0;
			if(!strcmp(tb, str)) { ret=1; break; }
			if(!strcmp(tb, "_fixint_t"))
			{
				if(!strcmp(str, "int")) { ret=1; break; }
				if(!strcmp(str, "integer")) { ret=1; break; }
				if(!strcmp(str, "number")) { ret=1; break; }
			}
			if(!strcmp(tb, "_flonum_t"))
			{
				if(!strcmp(str, "float")) { ret=1; break; }
				if(!strcmp(str, "number")) { ret=1; break; }
			}
			break;
			if(!strcmp(tb, "_object_t"))
			{
				if(!strcmp(str, "object")) { ret=1; break; }
				if(!strcmp(str, "Object")) { ret=1; break; }
			}
		case 'v':
			if(!strcmp(str, "null")) { ret=1; break; }
			if(!strcmp(str, "NULL")) { ret=1; break; }
			if(!strcmp(str, "_null_t")) { ret=1; break; }
			break;
		}
		break;
	case 'D':
		switch(sig[1])
		{
		case 'v':
			if(!strcmp(str, "undefined"))ret=1;
			if(!strcmp(str, "UNDEFINED"))ret=1;
			if(!strcmp(str, "_undefined_t"))ret=1;
			break;
		}
		break;
	default: break;
	}
	
	return(ret);
}

BGBDY_API int dyllSigCheckSigEqP(char *siga, char *sigb)
{
	if(!siga || !sigb)
		return(0);
	if(!strcmp(siga, sigb))
		return(1);
		
	return(0);
}

BGBDY_API dyt dyllSigCastVariantSig(char *sig, dyt val)
{
	dyt t;

	switch(*sig)
	{
	case 'a':
		if(!dyrealx2p(val))
			{ t=UNDEFINED; break; }
		t=dyint((s8)dyintv(val)); break;
	case 'b':
		if(!dyrealx2p(val))
			{ t=UNDEFINED; break; }
		t=dybool(dytruep(val)); break;
	case 'c':
		if(!dyrealx2p(val))
			{ t=UNDEFINED; break; }
		t=dyint((s8)dyintv(val)); break;
	case 'd':
		if(!dyrealx2p(val))
			{ t=UNDEFINED; break; }
		t=dydouble(dyrealv(val)); break;
	case 'e':
		if(!dyrealx2p(val))
			{ t=UNDEFINED; break; }
		t=dydouble(dyrealv(val)); break;
	case 'f':
		if(!dyrealx2p(val))
			{ t=UNDEFINED; break; }
		t=dyfloat(dyrealv(val)); break;
	case 'g':
		if(!dyrealx2p(val))
			{ t=UNDEFINED; break; }
		t=dyfloat128(dyfloat128v(val)); break;
	case 'h':
		if(!dyrealx2p(val))
			{ t=UNDEFINED; break; }
		t=dyint((u8)dyintv(val)); break;
	case 'i':
		if(!dyrealx2p(val))
			{ t=UNDEFINED; break; }
		t=dyint((s32)dyintv(val)); break;
	case 'j':
		if(!dyrealx2p(val))
			{ t=UNDEFINED; break; }
		t=dylong((u32)dyintv(val)); break;
	case 'k':
		if(!dyrealx2p(val))
			{ t=UNDEFINED; break; }
		t=dyflonum(dyrealv(val)); break;
	case 'l':
		if(!dyrealx2p(val))
			{ t=UNDEFINED; break; }
		t=dylong(dylongv(val)); break;
	case 'm':
		if(!dyrealx2p(val))
			{ t=UNDEFINED; break; }
		t=dylong(dylongv(val)); break;
	case 'n':
		if(!dyrealx2p(val))
			{ t=UNDEFINED; break; }
		t=dyint128(dyint128v(val)); break;
	case 'o':
		if(!dyrealx2p(val))
			{ t=UNDEFINED; break; }
		t=dyint128(dyint128v(val)); break;

	case 'p': t=UNDEFINED; break;
	case 'q': t=UNDEFINED; break;

	case 'r':
		if(dyllTypeboxPtrP(val))
		{
			t=dyllTypeboxPtrData(val);
			break;
		}

		t=val;
		break;

	case 's': t=dyint((s16)dyintv(val)); break;
	case 't': t=dyint((u16)dyintv(val)); break;
	case 'u': t=UNDEFINED; break;
	case 'v': t=NULL; break;
	case 'w': t=dyint((u16)dyintv(val)); break;
	case 'x': t=dylong(dylongv(val)); break;
	case 'y': t=dylong(dylongv(val)); break;
	case 'z': t=NULL; break;
	
	case 'C':
		switch(sig[1])
		{
		case 'a': t=dywvec2(dyvec2v(val)); break;
		case 'b': t=dywvec3(dyvec3v(val)); break;
		case 'c': t=dywvec4(dyvec4v(val)); break;
		case 'q': t=dywquat(dyquatv(val)); break;
		
		case 'o':
			if(!val) { t=val; break; }
			if(dycObjectP(val))
			{
				t=val;
				break;
			}
			t=UNDEFINED;
			break;
		case 's':
			if(!val) { t=val; break; }
			if(dystringp(val))
			{
				t=val;
				break;
			}
			if(dywstringp(val))
			{
				t=dystring(dystringv(val));
				break;
			}
			t=UNDEFINED;
			break;
		
		default: t=val; break;
		}
		break;
	case 'D':
		switch(sig[1])
		{
		case 'a': t=dywvec2d(dyvec2vd(val)); break;
		case 'b': t=dywvec3d(dyvec3vd(val)); break;
		case 'c': t=dywvec4d(dyvec4vd(val)); break;
		case 'q': t=dywquatd(dyquatvd(val)); break;
		default: t=val; break;
		}
		break;
	
	case 'L':
		if(!val) { t=val; break; }
		if(dycObjectP(val))
		{
			t=val;
			break;
		}
		t=UNDEFINED;
		break;
	case 'Q':
		if(!val) { t=val; break; }
		if(dycArrayP(val))
		{
			t=val;
			break;
		}
		t=UNDEFINED;
		break;

	case 'A':
		if((sig[1]>='0') && (sig[1]<='9'))
		{
			if(dycArrayP(val))
			{
				t=val;
				break;
			}
		}
		t=UNDEFINED;
		break;

	case 'P':
		if(dyllTypeboxPtrP(val))
		{
			t=dyllTypeboxPtr(sig+1,
				dyllTypeboxPtrData(val));
			break;
		}

		if(dylongp(val))
		{
			t=dyllTypeboxPtr(sig+1, (void *)((nlint)dylongv(val)));
			break;
		}

		t=dyllTypeboxPtr(sig+1, (void *)val);

//		t=val;
		break;
	
	default:
//		t=val;
		t=UNDEFINED;
		break;
	}
	
	return(t);
}
