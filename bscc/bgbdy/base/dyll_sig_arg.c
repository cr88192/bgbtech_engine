#include <bgbgc.h>
#include <bgbdy.h>

BGBDY_API char *dyllSigRet(char *sig)
{
	char *s;

	s=sig;
	if(*s=='(')
	{
		while(*s && (*s!=')'))s++;
		if(*s==')')return(s+1);
		return(NULL);
	}

	while(*s && (*s!=':'))s++;
	if(*s==':')return(s+1);
	return(NULL);
}

BGBDY_API int dyllCheckCallArg(char *fsig, char *sig)
{
	int i;

	switch(*fsig)
	{
	case 'a': case 'c': case 'h':
	case 's': case 't': case 'w':
	case 'b': case 'i': case 'j':
	case 'l': case 'm':
	case 'f': case 'd': case 'e': case 'k':
	case 'g':
	case 'n': case 'o':
	case 'x': case 'y':
		switch(*sig)
		{
		case 'a': case 'c': case 'h':
		case 's': case 't': case 'w':
		case 'b': case 'i': case 'j':
		case 'l': case 'm':
		case 'f': case 'd': case 'e': case 'k':
		case 'g':
		case 'n': case 'o':
		case 'x': case 'y':
			i=1; break;
		default: i=0; break;
		}
		break;

	case 'r':
		switch(*sig)
		{
		case 'a': case 'c': case 'h':
		case 's': case 't': case 'w':
		case 'b': case 'i': case 'j':
		case 'l': case 'm':
		case 'f': case 'd': case 'e': case 'k':
		case 'g':
		case 'n': case 'o':
		case 'x': case 'y':
		case 'r': case 'L':
			i=1; break;
		default: i=0; break;
		}
		break;

	case 'L':
		switch(*sig)
		{
		case 'r': case 'L':
			i=1; break;
		default: i=0; break;
		}
		break;

	case 'X':
		switch(*sig)
		{
		case 'X':
			while((*sig!=';') && (*fsig!=';') && (*sig==*fsig))
				{ sig++; fsig++; }
			if((*sig==';') && (*fsig==';'))
				i=1; break;
			i=0; break;
		default: i=0; break;
		}
		break;

	case 'P':
		while((*sig=='P') && (*fsig=='P'))
			{ sig++; fsig++; }
		if(*fsig=='P') { i=0; break; }
		i=dyllCheckCallArg(fsig, sig);
		break;

	default:
		i=0;
		break;
	}

	return(i);
}

BGBDY_API int dyllCheckCallSig(char *fsig, char *sig)
{
	char *s, *t;
	int i;

	s=fsig; t=sig;
	if(*s=='(')s++;
	while(*s && (*s!=')') && ((*s)==(*t)))
		{ s++; t++; }
	if(!(*s) || (*s==')'))
		return(2);

	s=fsig; t=sig;
	if(*s=='(')s++;
	while(1)
	{
		if(!(*s) && !(*t))
			return(1);

		if(*s=='z')return(1);

		i=dyllCheckCallArg(s, t);
		if(!i)return(0);

		s=dyllSigNext(s);
		t=dyllSigNext(t);
	}

	return(0);
}


BGBDY_API int dyllCheckCallSigValue(char *sig, dyt v)
{
	char *s0, *s1;
	int ret;

	ret=-1;		//default=no match
	switch(*sig)
	{
	case 0: case 'v': ret=-1; break;

	case 'a': case 'b': case 'c': case 'h':
	case 's': case 't': case 'w':
	case 'i': case 'j':
		if(dyintp(v))ret=0;
		else if(dyrealp(v))ret=2;
		break;
	case 'l': case 'm':
		if(dyintp(v))ret=0;
		else if(dylongp(v))ret=1;
		else if(dyrealp(v))ret=2;
		break;
	case 'x': case 'y':
		if(dylongp(v))ret=0;
		else if(dyintp(v))ret=1;
		else if(dyrealp(v))ret=2;
		break;

	case 'd':
		if(dydoublep(v))ret=0;
		else if(dyfloatp(v))ret=1;
		else if(dyrealp(v))ret=2;
		break;
	case 'e':
		if(dydoublep(v))ret=0;
		else if(dyfloatp(v))ret=1;
		else if(dyrealp(v))ret=2;
		break;
	case 'f':
		if(dyfloatp(v))ret=0;
		else if(dydoublep(v))ret=1;
		else if(dyrealp(v))ret=2;
		break;
	case 'k':
		if(dyfloatp(v))ret=0;
		else if(dydoublep(v))ret=1;
		else if(dyrealp(v))ret=2;
		break;

	case 'P':
		if(sig[1]=='c')
		{
			if(dystringp(v) || dysymbolp(v) || dykeywordp(v))
				ret=0;
			break;
		}

		ret=1;
		break;

	case 'r':
		ret=0; break;
	case 'L':
		if(!dycObjectP(v))
			break;
		s0=dyllSigGetStructName(sig);
		s1=dycGetObjClassName((dycObject)v);
		if(!strcmp(s0, s1))
			{ ret=0; break; }
		break;

	case 'U':
		if(dycObjectP(v))
		{
			s0=dyllSigGetStructName(sig);
			s1=dycGetObjClassShortName((dycObject)v);
			if(!strcmp(s0, s1))
				{ ret=1; break; }
			s1=dycGetObjClassName((dycObject)v);
			if(!strcmp(s0, s1))
				{ ret=0; break; }
			break;
		}
		break;

	case 'Q':
		if(dycArrayP(v))
		{
			ret=0;
			break;
		}
		break;

	case 'C':
		switch(sig[1])
		{
		case 'a': if(dyvec2p(v))ret=0; break;
		case 'b': if(dyvec3p(v))ret=0; break;
		case 'c': if(dyvec4p(v))ret=0; break;
		case 'q': if(dyquatp(v))ret=0; break;

		case 'f': if(dyfcomplexp(v))ret=0; break;
		case 'd': if(dydcomplexp(v))ret=0; break;
		case 'r': ret=0; break;

		case 'x': case 'y': case 'z':
			ret=0; break;

		case '2': case '3': case '4': case '5':
		case '6': case '7': case '8': case '9':
			break;
		default: break;
		}
		break;

	case 'D':
		switch(sig[1])
		{
		case 'a': if(dyvec2p(v))ret=0; break;
		case 'b': if(dyvec3p(v))ret=0; break;
		case 'c': if(dyvec4p(v))ret=0; break;
		case 'q': if(dyquatp(v))ret=0; break;
		default: break;
		}
		break;


//	default: p=NULL; break;

	default: ret=-1; break;

	}
	
	return(ret);
}

BGBDY_API int dyllCheckCallSigList(char *sig, dyt lst)
{
	dyt c, v;
	char *s;
	int i, j;
	
	s=sig; c=lst; j=0;
	if(*s=='(')s++;
	while(*s && (*s!=')') && c)
	{
		v=dycar(c); c=dycdr(c);
		i=dyllCheckCallSigValue(s, v);
		if(i<0)return(-1);
		j+=i;
		s=dyllSigNext(s);
	}
	
	if((*s==')') && (c==NULL))
		return(j);
//	if((*s!=')') && (c!=NULL))
//		return(j);
	
	return(-1);
}

BGBDY_API int dyllCheckCallSigArray(char *sig, dyt *args, int nargs)
{
	dyt c, v;
	char *s;
	int i, j, k;
	
	s=sig; j=0; k=0;
	if(*s=='(')s++;
	while(*s && (*s!=')') && (k<nargs))
	{
		v=args[k++];
		i=dyllCheckCallSigValue(s, v);
		if(i<0)return(-1);
		j+=i;
		s=dyllSigNext(s);
	}
	
	if((*s==')') && (k>=nargs))
		return(j);
//	if((*s!=')') && (c!=NULL))
//		return(j);
	
	return(-1);
}

BGBDY_API int dyllUnpackListSigUArgs(char *sig, dyt lst, void **arr)
{
	void **a;
	dyt c, v;
	char *s;
	int i;

	s=sig; a=arr; c=lst;
	if(*s=='(')s++;
	while(*s && (*s!=':') && (*s!=')'))
	{
		v=dycar(c);
		c=dycdr(c);
		switch(*s++)
		{
		case 'a': case 'b': case 'c': case 'h':
		case 's': case 't': case 'w':
		case 'i': case 'j': *(int *)(a++)=dyintv(v); break;
		case 'l': case 'm': *(long *)(a++)=dylongv(v); break;
		case 'f': *(float *)(a++)=dyfloatv(v); break;
		case 'd': *(double *)a=dydoublev(v); a+=2; break;
		case 'e': *(double *)a=dydoublev(v); a+=2; break;
		case 'k': *(float *)(a++)=dyfloatv(v); break;

		case 'n': case 'o':
			*(s64 *)a=dylongv(v); a+=2; break;

		case 'r':
			*arr++=v;
			break;

		case 'x': case 'y':
			*(s64 *)a=dylongv(v); a+=2; break;
			break;

		case 'P':
			*arr++=v;

			while(*s=='P')s++;
			s=dyllSigNext(s);
			break;

		case 'L':
			*arr++=v;
			while(*s && *s!=';')s++;
			if(*s==';')s++;
			break;

		case 'Q':
			*arr++=v;
			while(*s=='Q')s++;
			s=dyllSigNext(s);
			break;

		case 'C':
			switch(*s++)
			{
			case 'a': *(vec2 *)a=dyvec2v(v); a+=2; break;
			case 'b': *(vec3 *)a=dyvec3v(v); a+=4; break;
			case 'c': *(vec4 *)a=dyvec4v(v); a+=4; break;
			case 'q': *(quat *)a=dyquatv(v); a+=4; break;

			case 'x': case 'y': case 'z':
				*arr++=v;
				break;

			case '2': case '3': case '4': case '5':
			case '6': case '7': case '8': case '9':
				*arr++=v;
				s=dyllSigNext(s); break;
			}
			break;

		case 'D':
			switch(*s++)
			{
			case 'a': *(vec2d *)a=dyvec2vd(v); a+=4; break;
			case 'b': *(vec3d *)a=dyvec3vd(v); a+=8; break;
			case 'c': *(vec4d *)a=dyvec4vd(v); a+=8; break;
			case 'q': *(quatd *)a=dyquatvd(v); a+=8; break;
			}
			break;

		default: break;
		}
	}

	return(a-arr);
}

BGBDY_API int dyllUnpackArraySigUArgs(char *sig, dyt *args, void **arr)
{
	void **a;
	dyt c, v;
	char *s;
	int i;

	s=sig; a=arr; i=0;
	if(*s=='(')s++;
	while(*s && (*s!=')'))
	{
		v=args[i++];
		switch(*s++)
		{
		case 'a': case 'b': case 'c': case 'h':
		case 's': case 't': case 'w':
		case 'i': case 'j': *(int *)(a++)=dyintv(v); break;
		case 'l': case 'm': *(long *)(a++)=dylongv(v); break;
		case 'f': *(float *)(a++)=dyfloatv(v); break;
		case 'd': *(double *)a=dydoublev(v); a+=2; break;
		case 'e': *(double *)a=dydoublev(v); a+=2; break;
		case 'k': *(float *)(a++)=dyfloatv(v); break;

		case 'n': case 'o':
			*(s64 *)a=dylongv(v); a+=2; break;

		case 'r':
			*arr++=v;
			break;

		case 'x': case 'y':
			*(s64 *)a=dylongv(v); a+=2; break;
			break;

		case 'P':
			*arr++=v;

			while(*s=='P')s++;
			s=dyllSigNext(s);
			break;

		case 'L':
			*arr++=v;
			while(*s && *s!=';')s++;
			if(*s==';')s++;
			break;

		case 'Q':
			*arr++=v;
			while(*s=='Q')s++;
			s=dyllSigNext(s);
			break;

		case 'C':
			switch(*s++)
			{
			case 'a': *(vec2 *)a=dyvec2v(v); a+=2; break;
			case 'b': *(vec3 *)a=dyvec3v(v); a+=4; break;
			case 'c': *(vec4 *)a=dyvec4v(v); a+=4; break;
			case 'q': *(quat *)a=dyquatv(v); a+=4; break;

			case 'x': case 'y': case 'z':
				*arr++=v;
				break;

			case '2': case '3': case '4': case '5':
			case '6': case '7': case '8': case '9':
				*arr++=v;
				s=dyllSigNext(s); break;
			}
			break;

		case 'D':
			switch(*s++)
			{
			case 'a': *(vec2d *)a=dyvec2vd(v); a+=4; break;
			case 'b': *(vec3d *)a=dyvec3vd(v); a+=8; break;
			case 'c': *(vec4d *)a=dyvec4vd(v); a+=8; break;
			case 'q': *(quatd *)a=dyquatvd(v); a+=8; break;
			}
			break;


		default: break;
		}
	}

	return(a-arr);
}

BGBDY_API int dyllUnpackArgsSigArray(char *sig, va_list lst, dyt *arr)
{
	char *s;
	byte *tb;
	int i;

	s=sig; i=0;
	if(*s=='(')s++;
	while(*s && (*s!=':') && (*s!=')'))
	{
		switch(*s++)
		{
		case 'a': case 'b': case 'c': case 'h':
		case 's': case 't': case 'w':
		case 'i': case 'j':
			arr[i++]=dyint(va_arg(lst, int));
			break;
		case 'l': case 'm':
			arr[i++]=dylong(va_arg(lst, long));
			break;
		case 'f': case 'd':
			arr[i++]=dydouble(va_arg(lst, double));
			break;
		case 'e':
			arr[i++]=dydouble(va_arg(lst, long double));
			break;
//		case 'g':
//			arr[i++]=dydouble(va_arg(lst, long double));
//			break;
		case 'k':
			arr[i++]=dyflonum(va_arg(lst, double));
			break;

		case 'n': case 'o':
			arr[i++]=dylong(va_arg(lst, s64));
			break;

		case 'r':
			arr[i++]=va_arg(lst, void *);
			break;

		case 'x': case 'y':
			arr[i++]=dylong(va_arg(lst, s64));
			break;

		case 'C':
			switch(*s++)
			{
			case 'a': arr[i++]=dywvec2(va_arg(lst, vec2)); break;
			case 'b': arr[i++]=dywvec3(va_arg(lst, vec3)); break;
			case 'c': arr[i++]=dywvec4(va_arg(lst, vec4)); break;
			case 'q': arr[i++]=dywquat(va_arg(lst, quat)); break;

			case 'f':
				arr[i++]=dydcomplex(va_arg(lst, dcomplex));
				break;
			case 'd':
				arr[i++]=dydcomplex(va_arg(lst, dcomplex));
				break;
//			case 'e': i+=32; break;
//			case 'g': i+=32; break;
//			case 'k': break;

			case 'r':
				arr[i++]=dytfUnwrap(va_arg(lst, dytf));
				break;

			case 'x': case 'y': case 'z':
				arr[i++]=va_arg(lst, void *);
				break;

			case '2': case '3': case '4': case '5':
			case '6': case '7': case '8': case '9':
				arr[i++]=va_arg(lst, void *);
				s=dyllSigNext(s); break;
			}
			break;

		case 'D':
			switch(*s++)
			{
			case 'a': arr[i++]=dywvec2d(va_arg(lst, vec2d)); break;
			case 'b': arr[i++]=dywvec3d(va_arg(lst, vec3d)); break;
			case 'c': arr[i++]=dywvec4d(va_arg(lst, vec4d)); break;
			case 'q': arr[i++]=dywquatd(va_arg(lst, quatd)); break;
			}
			break;

		case 'P':
			arr[i++]=va_arg(lst, void *);

			while(*s=='P')s++;
			if((*s>='a') && (*s<='z'))
				{ s++; break; }
			if(*s=='X')
			{
				while(*s && *s!=';')s++;
				if(*s==';')s++;
			}
			break;

		case 'L':
			arr[i++]=va_arg(lst, void *);
			while(*s && *s!=';')s++;
			if(*s==';')s++;
			break;

		default: break;
		}
	}

	return(i);
}

BGBDY_API dyt dyllUnpackArgsSigList(char *sig, va_list lst)
{
	dyt arr[32];
	dyt l;
	int i, n;
	
	n=dyllUnpackArgsSigArray(sig, lst, arr);
	
	l=NULL;
	for(i=n-1; i>=0; i--)
		l=dycons(arr[i], l);
	return(l);
}

BGBDY_API int dyllUnpackUArgsSigArray(char *sig, void **args, dyt *arr)
{
	char *s;
	int i, j;

	s=sig; i=0; j=0;
	if(*s=='(')s++;
	while(*s && (*s!=':') && (*s!=')'))
	{
		switch(*s++)
		{
		case 'a': case 'b': case 'c': case 'h':
		case 's': case 't': case 'w':
		case 'i': case 'j':
			arr[i++]=dyint(*(int *)(args+(j++)));
			break;
		case 'l': case 'm':
			arr[i++]=dylong(*(long *)(args+(j++)));
			break;
		case 'f':
			arr[i++]=dyfloat(*(float *)(args+(j++)));
			break;
		case 'd':
			arr[i++]=dydouble(*(double *)(args+j)); j+=2;
			break;
//		case 'e':
//		case 'g':
//		case 'k':

//		case 'n': case 'o':
//			arr[i++]=dylong(va_arg(lst, s64));
//			break;

		case 'r':
			arr[i++]=args[j++];
			break;

		case 'x': case 'y':
			arr[i++]=dylong(*(double *)(args+j)); j+=2;
			break;

		case 'P':
			arr[i++]=args[j++];

			while(*s=='P')s++;
			s=dyllSigNext(s);
			break;

		case 'L':
			arr[i++]=args[j++];
			while(*s && *s!=';')s++;
			if(*s==';')s++;
			break;

		case 'C':
			switch(*s++)
			{
			case 'a': arr[i++]=dywvec2(*(vec2 *)(args+j)); j+=2; break;
			case 'b': arr[i++]=dywvec3(*(vec3 *)(args+j)); j+=4; break;
			case 'c': arr[i++]=dywvec4(*(vec4 *)(args+j)); j+=4; break;
			case 'q': arr[i++]=dywquat(*(quat *)(args+j)); j+=4; break;

			case 'r':
				arr[i++]=dytfUnwrap(*(dytf *)(args+j)); j+=2;
				break;

			case 'x': case 'y': case 'z':
				arr[i++]=args[j++];
				break;

			case '2': case '3': case '4': case '5':
			case '6': case '7': case '8': case '9':
				arr[i++]=args[j++];
				s=dyllSigNext(s); break;
			}
			break;

		case 'D':
			switch(*s++)
			{
			case 'a': arr[i++]=dywvec2d(*(vec2d *)(args+j)); j+=4; break;
			case 'b': arr[i++]=dywvec3d(*(vec3d *)(args+j)); j+=8; break;
			case 'c': arr[i++]=dywvec4d(*(vec4d *)(args+j)); j+=8; break;
			case 'q': arr[i++]=dywquatd(*(quatd *)(args+j)); j+=8; break;
			}
			break;

		default: break;
		}
	}

	return(i);
}

BGBDY_API int dyllUnpackArgsSigUArgs(char *sig, va_list lst, void **arr)
{
	void **a;
	char *s;
	int i;

	s=sig; a=arr;
	if(*s=='(')s++;
	while(*s && (*s!=':') && (*s!=')'))
	{
		switch(*s++)
		{
		case 'a': case 'b': case 'c': case 'h':
		case 's': case 't': case 'w':
		case 'i': case 'j': *(int *)(a++)=va_arg(lst, int); break;
		case 'l': case 'm': *(long *)(a++)=va_arg(lst, long); break;
		case 'f': *(float *)(a++)=va_arg(lst, double); break;
		case 'd': *(double *)a=va_arg(lst, double); a+=2; break;
		case 'e': *(double *)a=va_arg(lst, long double); a+=2; break;
		case 'k': *(float *)(a++)=va_arg(lst, double); break;

		case 'n': case 'o':
			*(s64 *)a=va_arg(lst, s64); a+=2; break;

		case 'r':
			*arr++=va_arg(lst, void *);
			break;

		case 'x': case 'y':
			*(s64 *)a=va_arg(lst, s64); a+=2; break;
			break;

		case 'P':
			*arr++=va_arg(lst, void *);

			while(*s=='P')s++;
			s=dyllSigNext(s);
			break;

		case 'L':
			*arr++=va_arg(lst, void *);
			while(*s && *s!=';')s++;
			if(*s==';')s++;
			break;

		case 'Q':
			*arr++=va_arg(lst, void *);
			while(*s=='Q')s++;
			s=dyllSigNext(s);
			break;

		case 'C':
			switch(*s++)
			{
			case 'a': *(vec2 *)a=va_arg(lst, vec2); a+=2; break;
			case 'b': *(vec3 *)a=va_arg(lst, vec3); a+=4; break;
			case 'c': *(vec4 *)a=va_arg(lst, vec4); a+=4; break;
			case 'q': *(quat *)a=va_arg(lst, quat); a+=4; break;

			case 'x': case 'y': case 'z':
				*arr++=va_arg(lst, void *);
				break;

			case '2': case '3': case '4': case '5':
			case '6': case '7': case '8': case '9':
				*arr++=va_arg(lst, void *);
				s=dyllSigNext(s); break;
			}
			break;

		case 'D':
			switch(*s++)
			{
			case 'a': *(vec2d *)a=va_arg(lst, vec2d); a+=4; break;
			case 'b': *(vec3d *)a=va_arg(lst, vec3d); a+=8; break;
			case 'c': *(vec4d *)a=va_arg(lst, vec4d); a+=8; break;
			case 'q': *(quatd *)a=va_arg(lst, quatd); a+=8; break;
			}
			break;

		default: break;
		}
	}

	return(a-arr);
}

BGBDY_API int dyllGetSigUArgsSize(char *sig)
{
	char *s;
	int i;

	s=sig; i=0;
	if(*s!='(')return(-1);
	s++;

	while(*s && (*s!=')'))
	{
		switch(*s++)
		{
		case 'a': case 'b': case 'c': case 'h':
		case 's': case 't': case 'w':
		case 'i': case 'j': i++; break;

		case 'l': case 'm': i++; break;
		case 'f': i++; break;
		case 'd': i+=2; break;

//		case 'n': case 'o':

		case 'r': i++; break;
		case 'x': case 'y': i+=2; break;

		case 'P':
			i++;

			while(*s=='P')s++;
			s=dyllSigNext(s);
			break;

		case 'L':
			i++;

			while(*s && *s!=';')s++;
			if(*s==';')s++;
			break;

		case 'Q':
			i++;
			while(*s=='Q')s++;
			s=dyllSigNext(s);
			break;

		case 'C':
			switch(*s++)
			{
			case 'a': i+=2; break;
			case 'b': i+=4; break;
			case 'v': i+=4; break;
			case 'q': i+=4; break;

			case 'r': i+=2; break;

			case 'x': case 'y': case 'z':
				i++; break;

			case '2': case '3': case '4': case '5':
			case '6': case '7': case '8': case '9':
				i++; s=dyllSigNext(s); break;
			}
			break;

		case 'D':
			switch(*s++)
			{
			case 'a': i+=4; break;
			case 'b': i+=8; break;
			case 'v': i+=8; break;
			case 'q': i+=8; break;
			}
			break;

		default: break;
		}
	}

	return(i);
}

BGBDY_API int dyllRepackBufSigArray(char *sig, char **esig,
	byte *buf, dyt *arr, int nargs)
{
	char *s;
	byte *tb;
	dyt c;
	int i, j;

	s=sig; i=0; tb=buf;
	if(*s=='(')s++;
	//if(*s==')') { s++; if(esig)*esig=s; return(0); }
	while(*s && (*s!=':') && (*s!=')'))
	{
		if(*s=='z')break;
		if(i>=nargs)return(-1);

		switch(*s++)
		{
		case 'a': case 'b': case 'c': case 'h':
		case 's': case 't': case 'w':
		case 'i': case 'j':
			*(int *)tb=dyintv(arr[i++]);
			tb+=sizeof(nlint); break;
		case 'l': case 'm':
			*(long *)tb=dyintv(arr[i++]);
			tb+=sizeof(nlint); break;
		case 'f':
			*(float *)tb=dyfloatv(arr[i++]);
			tb+=sizeof(nlint); break;
		case 'd':
			*(double *)tb=dydoublev(arr[i++]);
			tb+=sizeof(double); break;
		case 'e':
			*(long double *)tb=dydoublev(arr[i++]);
			j=sizeof(long double);
			j=(j+(sizeof(nlint)-1))/sizeof(nlint);
			j*=sizeof(nlint);
			tb+=j; break;
//		case 'g':
//			arr[i++]=dydouble(va_arg(lst, long double));
//			break;
		case 'k':
			*(float *)tb=dyfloatv(arr[i++]);
			tb+=sizeof(nlint); break;

		case 'n': case 'o':
			*(s64 *)tb=dylongv(arr[i++]);
			*(s64 *)(tb+8)=(tb[7]&0x80)?-1LL:0LL;
			tb+=16; break;

		case 'r':
			*(dyt *)tb=arr[i++];
			tb+=sizeof(nlint); break;

		case 'x': case 'y':
			*(s64 *)tb=dylongv(arr[i++]);
			tb+=sizeof(s64); break;

		case 'C':
			switch(*s++)
			{
			case 'a':
				*(vec2 *)tb=dyvec2v(arr[i++]);
				tb+=sizeof(vec2); break;
			case 'b':
				*(vec3 *)tb=dyvec3v(arr[i++]);
				tb+=sizeof(vec3); break;
			case 'c':
				*(vec4 *)tb=dyvec4v(arr[i++]);
				tb+=sizeof(vec4); break;
			case 'q':
				*(quat *)tb=dyquatv(arr[i++]);
				tb+=sizeof(quat); break;

			case 'f':
				*(fcomplex *)tb=dyfcomplexv(arr[i++]);
				tb+=sizeof(fcomplex); break;
			case 'd':
				*(dcomplex *)tb=dydcomplexv(arr[i++]);
				tb+=sizeof(dcomplex); break;
//			case 'e': i+=32; break;
//			case 'g': i+=32; break;
//			case 'k': break;

			case 'r':
				*(dytf *)tb=dytfWrap(arr[i++]);
				tb+=sizeof(dytf); break;

			case 'x': case 'y':
				*(dyt *)tb=arr[i++];
				tb+=sizeof(dyt); break;
			case 'z':
				c=NULL;
				while(i<nargs)
					c=dycons(arr[i++], c);
				c=dynreverse(c);
				*(dyt *)tb=c;
				tb+=sizeof(dyt); break;

			case '2': case '3': case '4': case '5':
			case '6': case '7': case '8': case '9':
				*(dyt *)tb=arr[i++];
				tb+=sizeof(dyt); s=dyllSigNext(s); break;
			}
			break;

		case 'D':
			switch(*s++)
			{
			case 'a':
				*(vec2d *)tb=dyvec2vd(arr[i++]);
				tb+=sizeof(vec2d); break;
			case 'b':
				*(vec3d *)tb=dyvec3vd(arr[i++]);
				tb+=sizeof(vec3d); break;
			case 'c':
				*(vec4d *)tb=dyvec4vd(arr[i++]);
				tb+=sizeof(vec4d); break;
			case 'q':
				*(quatd *)tb=dyquatvd(arr[i++]);
				tb+=sizeof(quatd); break;
			}
			break;

		case 'P':
			*(dyt *)tb=arr[i++];
			tb+=sizeof(void *);
			while(*s=='P')s++;
			s=dyllSigNext(s);
			break;

		case 'Q':
			*(dyt *)tb=arr[i++];
			tb+=sizeof(void *);
			while(*s=='Q')s++;
			s=dyllSigNext(s);
			break;

		case 'L':
			*(dyt *)tb=arr[i++];
			tb+=sizeof(dyt);
			while(*s && *s!=';')s++;
			if(*s==';')s++;
			break;

		default: break;
		}
	}

	if(*s=='z')
	{
		s++;

		for(; i<nargs; i++)
		{
			if(dyintp(arr[i]))
			{
				*(int *)tb=dyintv(arr[i]);
				tb+=sizeof(nlint); continue;
			}
			if(dylongp(arr[i]))
			{
				*(s64 *)tb=dylongv(arr[i]);
				tb+=sizeof(s64); continue;
			}
			if(dyrealp(arr[i]))
			{
				*(double *)tb=dydoublev(arr[i]);
				tb+=sizeof(double); continue;
			}
			*(dyt *)tb=arr[i];
			tb+=sizeof(nlint);
		}
	}

	if(*s && (*s==':' || *s==')'))s++;
	if(esig)*esig=s;

	return(tb-buf);
}

BGBDY_API int dyllRepackBufSigArrayB(char *sig, char **esig,
	byte *buf, dytf *arr, int nargs)
{
	char *s;
	byte *tb;
	dyt c;
	int i, j;

	s=sig; i=0; tb=buf;
	if(*s=='(')s++;
	//if(*s==')') { s++; if(esig)*esig=s; return(0); }
	while(*s && (*s!=':') && (*s!=')'))
	{
		if(*s=='z')break;
		if(i>=nargs)return(-1);

		switch(*s++)
		{
		case 'a': case 'b': case 'c': case 'h':
		case 's': case 't': case 'w':
		case 'i': case 'j':
			*(int *)tb=dytfIntv(arr[i++]);
			tb+=sizeof(nlint); break;
		case 'l': case 'm':
			*(long *)tb=dytfIntv(arr[i++]);
			tb+=sizeof(nlint); break;
		case 'f':
			*(float *)tb=dytfFloatv(arr[i++]);
			tb+=sizeof(nlint); break;
		case 'd':
			*(double *)tb=dytfDoublev(arr[i++]);
			tb+=sizeof(double); break;
		case 'e':
			*(long double *)tb=dytfDoublev(arr[i++]);
			j=sizeof(long double);
			j=(j+(sizeof(nlint)-1))/sizeof(nlint);
			j*=sizeof(nlint);
			tb+=j; break;
//		case 'g':
//			arr[i++]=dydouble(va_arg(lst, long double));
//			break;
		case 'k':
			*(float *)tb=dytfFloatv(arr[i++]);
			tb+=sizeof(nlint); break;

		case 'n': case 'o':
			*(s64 *)tb=dytfLongv(arr[i++]);
			*(s64 *)(tb+8)=(tb[7]&0x80)?-1LL:0LL;
			tb+=16; break;

		case 'r':
			*(dyt *)tb=dytfUnwrap(arr[i++]);
			tb+=sizeof(nlint); break;

		case 'x': case 'y':
			*(s64 *)tb=dytfLongv(arr[i++]);
			tb+=sizeof(s64); break;

		case 'C':
			switch(*s++)
			{
			case 'a':
				*(vec2 *)tb=dyvec2v(dytfPtrvF(arr[i++]));
				tb+=sizeof(vec2); break;
			case 'b':
				*(vec3 *)tb=dyvec3v(dytfPtrvF(arr[i++]));
				tb+=sizeof(vec3); break;
			case 'c':
				*(vec4 *)tb=dyvec4v(dytfPtrvF(arr[i++]));
				tb+=sizeof(vec4); break;
			case 'q':
				*(quat *)tb=dyquatv(dytfPtrvF(arr[i++]));
				tb+=sizeof(quat); break;

			case 'f':
				*(fcomplex *)tb=dyfcomplexv(dytfPtrvF(arr[i++]));
				tb+=sizeof(fcomplex); break;
			case 'd':
				*(dcomplex *)tb=dydcomplexv(dytfPtrvF(arr[i++]));
				tb+=sizeof(dcomplex); break;
//			case 'e': i+=32; break;
//			case 'g': i+=32; break;
//			case 'k': break;

			case 'r':
				*(dytf *)tb=arr[i++];
				tb+=sizeof(dytf); break;

			case 'x': case 'y':
				*(dyt *)tb=dytfPtrvF(arr[i++]);
				tb+=sizeof(dyt); break;
			case 'z':
				c=NULL;
				while(i<nargs)
					c=dycons(dytfUnwrap(arr[i++]), c);
				c=dynreverse(c);
				*(dyt *)tb=c;
				tb+=sizeof(dyt); break;

			case '2': case '3': case '4': case '5':
			case '6': case '7': case '8': case '9':
				*(dyt *)tb=dytfPtrvF(arr[i++]);
				tb+=sizeof(dyt); s=dyllSigNext(s); break;
			}
			break;

		case 'D':
			switch(*s++)
			{
			case 'a':
				*(vec2d *)tb=dyvec2vd(dytfPtrvF(arr[i++]));
				tb+=sizeof(vec2d); break;
			case 'b':
				*(vec3d *)tb=dyvec3vd(dytfPtrvF(arr[i++]));
				tb+=sizeof(vec3d); break;
			case 'c':
				*(vec4d *)tb=dyvec4vd(dytfPtrvF(arr[i++]));
				tb+=sizeof(vec4d); break;
			case 'q':
				*(quatd *)tb=dyquatvd(dytfPtrvF(arr[i++]));
				tb+=sizeof(quatd); break;
			}
			break;

		case 'P':
			*(dyt *)tb=dytfPtrvF(arr[i++]);
			tb+=sizeof(void *);
			while(*s=='P')s++;
			s=dyllSigNext(s);
			break;

		case 'Q':
			*(dyt *)tb=dytfPtrvF(arr[i++]);
			tb+=sizeof(void *);
			while(*s=='Q')s++;
			s=dyllSigNext(s);
			break;

		case 'L':
			*(dyt *)tb=dytfPtrvF(arr[i++]);
			tb+=sizeof(dyt);
			while(*s && *s!=';')s++;
			if(*s==';')s++;
			break;

		default: break;
		}
	}

	if(*s=='z')
	{
		s++;

		for(; i<nargs; i++)
		{
			if(dytfIntP(arr[i]))
			{
				*(int *)tb=dytfIntv(arr[i]);
				tb+=sizeof(nlint); continue;
			}
			if(dytfLongP(arr[i]))
			{
				*(s64 *)tb=dytfLongv(arr[i]);
				tb+=sizeof(s64); continue;
			}
			if(dytfRealP(arr[i]))
			{
				*(double *)tb=dytfDoublev(arr[i]);
				tb+=sizeof(double); continue;
			}
			*(dyt *)tb=dytfPtrvF(arr[i]);
			tb+=sizeof(nlint);
		}
	}

	if(*s && (*s==':' || *s==')'))s++;
	if(esig)*esig=s;

	return(tb-buf);
}


BGBDY_API int dyllRepackBufSigArgs(char *dsig, char **esig, byte *buf,
	char *ssig, va_list lst)
{
	char *ds, *ss;
	byte *tb;
	int i, j;

	ds=dsig; ss=ssig; i=0; tb=buf;
	if((*ss=='(') && (*ds=='(')) { ss++; ds++; }
	//if(*ds==')') { ds++; if(esig)*esig=ds; return(0); }
	while(*ds && (*ds!=':') && (*ds!=')'))
	{
		if(*ds=='z')break;
//		if(i>=nargs)return(-1);

		if((*ds)!=(*ss))return(-1);
		j=*ds++; *ss++;
		switch(j)
		{
		case 'a': case 'b': case 'c': case 'h':
		case 's': case 't': case 'w':
		case 'i': case 'j':
			*(int *)tb=va_arg(lst, int);
			tb+=sizeof(nlint); break;
		case 'l': case 'm':
			*(long *)tb=va_arg(lst, long);
			tb+=sizeof(nlint); break;
		case 'f':
			*(float *)tb=va_arg(lst, double);
			tb+=sizeof(nlint); break;
		case 'd':
			*(double *)tb=va_arg(lst, double);
			tb+=sizeof(double); break;
//		case 'e':
//			*(long double *)tb=va_arg(lst, long double);
//			tb+=12; break;
//		case 'g':
//			arr[i++]=dydouble(va_arg(lst, long double));
//			break;
		case 'k':
			*(float *)tb=va_arg(lst, double);
			tb+=sizeof(nlint); break;

		case 'r':
			*(void **)tb=va_arg(lst, void *);
			tb+=sizeof(nlint); break;

		case 'x': case 'y':
			*(long long *)tb=va_arg(lst, long long);
			tb+=sizeof(s64); break;

		case 'C':
			if((*ds)!=(*ss))return(-1);
			j=*ds++; *ss++;
			switch(j)
			{
			case 'a':
				*(vec2 *)tb=va_arg(lst, vec2);
				tb+=sizeof(vec2); break;
			case 'b':
				*(vec3 *)tb=va_arg(lst, vec3);
				tb+=sizeof(vec3); break;
			case 'c':
				*(vec4 *)tb=va_arg(lst, vec4);
				tb+=sizeof(vec4); break;
			case 'q':
				*(quat *)tb=va_arg(lst, quat);
				tb+=sizeof(quat); break;

			case 'f':
				*(fcomplex *)tb=va_arg(lst, fcomplex);
				tb+=sizeof(fcomplex); break;
			case 'd':
				*(dcomplex *)tb=va_arg(lst, dcomplex);
				tb+=sizeof(dcomplex); break;
//			case 'e': i+=32; break;
//			case 'g': i+=32; break;
//			case 'k': break;

			case 'r':
				*(dytf *)tb=va_arg(lst, dytf);
				tb+=sizeof(dytf); break;

			case 'x': case 'y': case 'z':
				*(void **)tb=va_arg(lst, void *);
				tb+=sizeof(nlint); break;

			case '2': case '3': case '4': case '5':
			case '6': case '7': case '8': case '9':
				*(void **)tb=va_arg(lst, void *);
				tb+=sizeof(nlint); ds=dyllSigNext(ds);
				ss=dyllSigNext(ss); break;
			}
			break;

		case 'D':
			if((*ds)!=(*ss))return(-1);
			j=*ds++; *ss++;
			switch(j)
			{
			case 'a':
				*(vec2d *)tb=va_arg(lst, vec2d);
				tb+=sizeof(vec2d); break;
			case 'b':
				*(vec3d *)tb=va_arg(lst, vec3d);
				tb+=sizeof(vec3d); break;
			case 'c':
				*(vec4d *)tb=va_arg(lst, vec4d);
				tb+=sizeof(vec4d); break;
			case 'q':
				*(quatd *)tb=va_arg(lst, quatd);
				tb+=sizeof(quatd); break;
			}
			break;

		case 'P':
			*(void **)tb=va_arg(lst, void *);
			tb+=sizeof(nlint);

			while(*ds=='P')ds++;
			if((*ds>='a') && (*ds<='z'))
				{ ds++; break; }
			if(*ds=='X')
			{
				while(*ds && *ds!=';')ds++;
				if(*ds==';')ds++;
			}

			while(*ss=='P')ss++;
			if((*ss>='a') && (*ss<='z'))
				{ ss++; break; }
			if(*ss=='X')
			{
				while(*ss && *ss!=';')ss++;
				if(*ss==';')ss++;
			}
			break;

		case 'L':
			*(void **)tb=va_arg(lst, void *);
			tb+=sizeof(nlint);

			while(*ds && *ds!=';')ds++;
			if(*ds==';')ds++;
			while(*ss && *ss!=';')ss++;
			if(*ss==';')ss++;
			break;

		default: break;
		}
	}

	if(*ds=='z')
	{
		ds++;

		while(*ss && (*ss!=':'))
			switch(*ss++)
		{
		case 'a': case 'b': case 'c': case 'h':
		case 's': case 't': case 'w':
		case 'i': case 'j':
			*(int *)tb=va_arg(lst, int);
			tb+=sizeof(nlint); break;
		case 'l': case 'm':
			*(long *)tb=va_arg(lst, long);
			tb+=sizeof(nlint); break;
		case 'f':
			*(float *)tb=va_arg(lst, double);
			tb+=sizeof(nlint); break;
		case 'd':
			*(double *)tb=va_arg(lst, double);
			tb+=sizeof(double); break;
//		case 'e':
//			*(long double *)tb=va_arg(lst, long double);
//			tb+=12; break;
//		case 'g':
//			arr[i++]=dydouble(va_arg(lst, long double));
//			break;
		case 'k':
			*(float *)tb=va_arg(lst, double);
			tb+=sizeof(nlint); break;

		case 'r':
			*(void **)tb=va_arg(lst, void *);
			tb+=sizeof(nlint); break;

		case 'x': case 'y':
			*(long long *)tb=va_arg(lst, long long);
			tb+=sizeof(s64); break;

		case 'C':
			switch(*ss++)
			{
			case 'f':
				*(fcomplex *)tb=va_arg(lst, fcomplex);
				tb+=sizeof(fcomplex); break;
			case 'd':
				*(dcomplex *)tb=va_arg(lst, dcomplex);
				tb+=sizeof(dcomplex); break;
//			case 'e': i+=32; break;
//			case 'g': i+=32; break;
//			case 'k': break;
			}
			break;

		case 'P':
			*(void **)tb=va_arg(lst, void *);
			tb+=sizeof(nlint);

			while(*ss=='P')ss++;
			if((*ss>='a') && (*ss<='z'))
				{ ss++; break; }
			if(*ss=='X')
			{
				while(*ss && *ss!=';')ss++;
				if(*ss==';')ss++;
			}
			break;

		default: break;
		}
	}

	if(*ds && ((*ds==':') || (*ds==')')))ds++;
	if(esig)*esig=ds;

	return(tb-buf);
}

BGBDY_API int dyllRepackBufSigV(char *sig, byte *buf, va_list lst)
{
	char *s;
	byte *tb;
	int i, j;

	s=sig; i=0; tb=buf;
//	if(*s=='(')s++;
	//if(*s==')') { s++; return(0); }

	if(*s=='(')
		{ s++; if(*s==')')return(0); }

	while(*s && (*s!=')'))
	{
		switch(*s++)
		{
		case 'a': case 'b': case 'c': case 'h':
		case 's': case 't': case 'w':
		case 'i': case 'j':
			*(int *)tb=va_arg(lst, int);
			tb+=sizeof(nlint); break;
		case 'l': case 'm':
			*(long *)tb=va_arg(lst, long);
			tb+=sizeof(nlint); break;
		case 'f':
			*(float *)tb=va_arg(lst, double);
			tb+=sizeof(nlint); break;
		case 'd':
			*(double *)tb=va_arg(lst, double);
			tb+=sizeof(s64); break;
//		case 'e':
//			*(long double *)tb=va_arg(lst, long double);
//			tb+=12; break;
//		case 'g':
//			arr[i++]=dydouble(va_arg(lst, long double));
//			break;
		case 'k':
			*(float *)tb=va_arg(lst, double);
			tb+=sizeof(nlint); break;

		case 'r':
			*(void **)tb=va_arg(lst, void *);
			tb+=sizeof(nlint); break;

		case 'x': case 'y':
			*(long long *)tb=va_arg(lst, long long);
			tb+=sizeof(s64); break;

		case 'C':
			j=*s++;
			switch(j)
			{
			case 'a':
				*(vec2 *)tb=va_arg(lst, vec2);
				tb+=sizeof(vec2); break;
			case 'b':
				*(vec3 *)tb=va_arg(lst, vec3);
				tb+=sizeof(vec3); break;
			case 'c':
				*(vec4 *)tb=va_arg(lst, vec4);
				tb+=sizeof(vec4); break;
			case 'q':
				*(quat *)tb=va_arg(lst, quat);
				tb+=sizeof(quat); break;

			case 'f':
				*(fcomplex *)tb=va_arg(lst, fcomplex);
				tb+=sizeof(fcomplex); break;
			case 'd':
				*(dcomplex *)tb=va_arg(lst, dcomplex);
				tb+=sizeof(dcomplex); break;
//			case 'e': i+=32; break;
//			case 'g': i+=32; break;
//			case 'k': break;

			case 'r':
				*(dytf *)tb=va_arg(lst, dytf);
				tb+=sizeof(dytf); break;

			case '2': case '3': case '4': case '5':
			case '6': case '7': case '8': case '9':
				*(void **)tb=va_arg(lst, void *);
				tb+=sizeof(nlint); s=dyllSigNext(s); break;
			}
			break;

		case 'D':
			j=*s++;
			switch(j)
			{
			case 'a':
				*(vec2d *)tb=va_arg(lst, vec2d);
				tb+=sizeof(vec2d); break;
			case 'b':
				*(vec3d *)tb=va_arg(lst, vec3d);
				tb+=sizeof(vec3d); break;
			case 'c':
				*(vec4d *)tb=va_arg(lst, vec4d);
				tb+=sizeof(vec4d); break;
			case 'q':
				*(quatd *)tb=va_arg(lst, quatd);
				tb+=sizeof(quatd); break;
			}
			break;

		case 'P':
			*(void **)tb=va_arg(lst, void *);
			tb+=sizeof(nlint);

			while(*s=='P')s++;
			if((*s>='a') && (*s<='z'))
				{ s++; break; }
			if(*s=='X')
			{
				while(*s && *s!=';')s++;
				if(*s==';')s++;
			}
			break;

		case 'L':
			*(void **)tb=va_arg(lst, void *);
			tb+=sizeof(nlint);

			while(*s && *s!=';')s++;
			if(*s==';')s++;
			break;

		default: break;
		}
	}
	return(tb-buf);
}

// #ifdef __i386__
#if 1
BGBDY_API int dyllRepackBufSigU(char *sig, byte *buf, void **args)
{
	void **pa;
	char *s;
	byte *tb;
	int i, j;

	s=sig; i=0; tb=buf; pa=args;
	if(*s=='(')s++;
	//if(*s==')') { return(0); }
	while(*s && (*s!=')'))
	{
		switch(*s++)
		{
		case 'a': case 'b': case 'c': case 'h':
		case 's': case 't': case 'w':
		case 'i': case 'j':
			*(int *)tb=*(int *)(pa++);
			tb+=sizeof(nlint); break;
		case 'l': case 'm':
			*(long *)tb=*(long *)(pa++);
			tb+=sizeof(nlint); break;
		case 'f':
			*(float *)tb=*(float *)(pa++);
			tb+=sizeof(nlint); break;
		case 'd':
			*(double *)tb=*(double *)pa; pa+=2;
			tb+=sizeof(s64); break;
//		case 'e':
//		case 'g':
		case 'k':
			*(float *)tb=*(float *)(pa++);
			tb+=sizeof(nlint); break;

		case 'r':
			*(void **)tb=*(void **)(pa++);
			tb+=sizeof(nlint); break;

		case 'x': case 'y':
			*(long long *)tb=*(long long *)pa; pa+=2;
			tb+=sizeof(s64); break;

		case 'P':
			*(void **)tb=*(void **)(pa++);
			tb+=sizeof(nlint);

			while(*s=='P')s++;
			if((*s>='a') && (*s<='z'))
				{ s++; break; }
			if(*s=='X')
			{
				while(*s && *s!=';')s++;
				if(*s==';')s++;
			}

			break;

		case 'L':
			*(void **)tb=*(void **)(pa++);
			tb+=sizeof(nlint);

			while(*s && *s!=';')s++;
			if(*s==';')s++;
			break;

		default: break;
		}
	}
	return(tb-buf);
}
#endif

//#ifdef __x86_64__
#if 0
BGBDY_API int dyllRepackBufSigU(char *sig, byte *buf, void **args)
{
	void **pa;
	char *s;
	byte *tb;
	int i, j;

	s=sig; i=0; tb=buf; pa=args;
	if(*s=='(')s++;
	while(*s && (*s!=')'))
	{
		switch(*s++)
		{
		case 'a': case 'b': case 'c': case 'h':
		case 's': case 't': case 'w':
		case 'i': case 'j':
			*(int *)tb=*(int *)(pa++);
			tb+=8; break;
		case 'l': case 'm':
			*(long *)tb=*(long *)(pa++);
			tb+=8; break;
		case 'f':
			*(float *)tb=*(float *)(pa++);
			tb+=8; break;
		case 'd':
			*(double *)tb=*(double *)pa; pa+=2;
			tb+=8; break;
//		case 'e':
//		case 'g':
		case 'k':
			*(float *)tb=*(float *)(pa++);
			tb+=4; break;

		case 'r':
			*(void **)tb=*(void **)(pa++);
			tb+=8; break;

		case 'x': case 'y':
			*(long long *)tb=*(long long *)pa; pa+=2;
			tb+=8; break;

		case 'P':
			*(void **)tb=*(void **)(pa++);
			tb+=8;

			while(*s=='P')s++;
			if((*s>='a') && (*s<='z'))
				{ s++; break; }
			if(*s=='X')
			{
				while(*s && *s!=';')s++;
				if(*s==';')s++;
			}

			break;

		case 'L':
			*(void **)tb=*(void **)(pa++);
			tb+=8;

			while(*s && *s!=';')s++;
			if(*s==';')s++;
			break;

		default: break;
		}
	}
	return(tb-buf);
}
#endif


// #ifdef __i386__
#if 1
BGBDY_API int dyllRepackBufSigUArgs(char *dsig, char **esig, byte *buf,
	char *ssig, void **args)
{
	void **pa;
	char *ds, *ss;
	byte *tb;
	int i, j;

	ds=dsig; ss=ssig; i=0; tb=buf; pa=args;
	if((*ss=='(') && (*ds=='(')) { ss++; ds++; }
	while(*ds && (*ds!=':') && (*ds!=')'))
	{
		if(*ds=='z')break;
//		if(i>=nargs)return(-1);

		if((*ds)!=(*ss))return(-1);
		j=*ds++; *ss++;
		switch(j)
		{
		case 'a': case 'b': case 'c': case 'h':
		case 's': case 't': case 'w':
		case 'i': case 'j':
			*(int *)tb=*(int *)(pa++);
			tb+=sizeof(nlint); break;
		case 'l': case 'm':
			*(long *)tb=*(long *)(pa++);
			tb+=sizeof(nlint); break;
		case 'f':
			*(float *)tb=*(float *)(pa++);
			tb+=sizeof(nlint); break;
		case 'd':
			*(double *)tb=*(double *)pa; pa+=2;
			tb+=sizeof(s64); break;
//		case 'e':
//		case 'g':
		case 'k':
			*(float *)tb=*(float *)(pa++);
			tb+=sizeof(nlint); break;

		case 'r':
			*(void **)tb=*(void **)(pa++);
			tb+=sizeof(nlint); break;

		case 'x': case 'y':
			*(long long *)tb=*(long long *)pa; pa+=2;
			tb+=sizeof(s64); break;

		case 'P':
			*(void **)tb=*(void **)(pa++);
			tb+=sizeof(nlint);

			while(*ds=='P')ds++;
			if((*ds>='a') && (*ds<='z'))
				{ ds++; break; }
			if(*ds=='X')
			{
				while(*ds && *ds!=';')ds++;
				if(*ds==';')ds++;
			}

			while(*ss=='P')ss++;
			if((*ss>='a') && (*ss<='z'))
				{ ss++; break; }
			if(*ss=='X')
			{
				while(*ss && *ss!=';')ss++;
				if(*ss==';')ss++;
			}
			break;

		case 'L':
			*(void **)tb=*(void **)(pa++);
			tb+=sizeof(nlint);

			while(*ds && *ds!=';')ds++;
			if(*ds==';')ds++;
			while(*ss && *ss!=';')ss++;
			if(*ss==';')ss++;
			break;

		default: break;
		}
	}

	if(*ds=='z')
	{
		ds++;

		while(*ss && (*ss!=':') && (*ss!=')'))
			switch(*ss++)
		{
		case 'a': case 'b': case 'c': case 'h':
		case 's': case 't': case 'w':
		case 'i': case 'j':
			*(int *)tb=*(int *)(pa++);
			tb+=sizeof(nlint); break;
		case 'l': case 'm':
			*(long *)tb=*(long *)(pa++);
			tb+=sizeof(nlint); break;
		case 'f':
			*(float *)tb=*(float *)(pa++);
			tb+=sizeof(nlint); break;
		case 'd':
			*(double *)tb=*(double *)pa; pa+=2;
			tb+=sizeof(s64); break;
//		case 'e':
//		case 'g':
//		case 'k':

		case 'r':
			*(void **)tb=*(void **)(pa++);
			tb+=sizeof(nlint); break;

		case 'x': case 'y':
			*(long long *)tb=*(long long *)pa; pa+=2;
			tb+=sizeof(s64); break;

		case 'P':
			*(void **)tb=*(void **)(pa++);
			tb+=sizeof(nlint);

			while(*ss=='P')ss++;
			if((*ss>='a') && (*ss<='z'))
				{ ss++; break; }
			if(*ss=='X')
			{
				while(*ss && *ss!=';')ss++;
				if(*ss==';')ss++;
			}
			break;

		case 'L':
			*(void **)tb=*(void **)(pa++);
			tb+=sizeof(nlint);

			while(*ss && *ss!=';')ss++;
			if(*ss==';')ss++;
			break;

		default: break;
		}
	}

	if(*ds && ((*ds==':') || (*ds==')')))ds++;
	if(esig)*esig=ds;

	return(tb-buf);
}
#endif

// #ifdef __x86_64__
#if 0
BGBDY_API int dyllRepackBufSigUArgs(char *dsig, char **esig, byte *buf,
	char *ssig, void **args)
{
	void **pa;
	char *ds, *ss;
	byte *tb;
	int i, j;

	ds=dsig; ss=ssig; i=0; tb=buf; pa=args;
	if((*ss=='(') && (*ds=='(')) { ss++; ds++; }
	while(*ds && (*ds!=':') && (*ds!=')'))
	{
		if(*ds=='z')break;
//		if(i>=nargs)return(-1);

		if((*ds)!=(*ss))return(-1);
		j=*ds++; *ss++;
		switch(j)
		{
		case 'a': case 'b': case 'c': case 'h':
		case 's': case 't': case 'w':
		case 'i': case 'j':
			*(int *)tb=*(int *)(pa++);
			tb+=8; break;
		case 'l': case 'm':
			*(long *)tb=*(long *)(pa++);
			tb+=8; break;
		case 'f':
			*(float *)tb=*(float *)(pa++);
			tb+=8; break;
		case 'd':
			*(double *)tb=*(double *)pa; pa+=2;
			tb+=8; break;
//		case 'e':
//		case 'g':
		case 'k':
			*(float *)tb=*(float *)(pa++);
			tb+=8; break;

		case 'r':
			*(void **)tb=*(void **)(pa++);
			tb+=8; break;

		case 'x': case 'y':
			*(long long *)tb=*(long long *)pa; pa+=2;
			tb+=8; break;

		case 'P':
			*(void **)tb=*(void **)(pa++);
			tb+=8;

			while(*ds=='P')ds++;
			if((*ds>='a') && (*ds<='z'))
				{ ds++; break; }
			if(*ds=='X')
			{
				while(*ds && *ds!=';')ds++;
				if(*ds==';')ds++;
			}

			while(*ss=='P')ss++;
			if((*ss>='a') && (*ss<='z'))
				{ ss++; break; }
			if(*ss=='X')
			{
				while(*ss && *ss!=';')ss++;
				if(*ss==';')ss++;
			}
			break;

		case 'L':
			*(void **)tb=*(void **)(pa++);
			tb+=8;

			while(*ds && *ds!=';')ds++;
			if(*ds==';')ds++;
			while(*ss && *ss!=';')ss++;
			if(*ss==';')ss++;
			break;

		default: break;
		}
	}

	if(*ds=='z')
	{
		ds++;

		while(*ss && (*ss!=':') && (*ss!=')'))
			switch(*ss++)
		{
		case 'a': case 'b': case 'c': case 'h':
		case 's': case 't': case 'w':
		case 'i': case 'j':
			*(int *)tb=*(int *)(pa++);
			tb+=8; break;
		case 'l': case 'm':
			*(long *)tb=*(long *)(pa++);
			tb+=8; break;
		case 'f':
			*(float *)tb=*(float *)(pa++);
			tb+=8; break;
		case 'd':
			*(double *)tb=*(double *)pa; pa+=2;
			tb+=8; break;
//		case 'e':
//		case 'g':
//		case 'k':

		case 'r':
			*(void **)tb=*(void **)(pa++);
			tb+=8; break;

		case 'x': case 'y':
			*(long long *)tb=*(long long *)pa; pa+=2;
			tb+=8; break;

		case 'P':
			*(void **)tb=*(void **)(pa++);
			tb+=8;

			while(*ss=='P')ss++;
			if((*ss>='a') && (*ss<='z'))
				{ ss++; break; }
			if(*ss=='X')
			{
				while(*ss && *ss!=';')ss++;
				if(*ss==';')ss++;
			}
			break;

		case 'L':
			*(void **)tb=*(void **)(pa++);
			tb+=8;

			while(*ss && *ss!=';')ss++;
			if(*ss==';')ss++;
			break;

		default: break;
		}
	}

	if(*ds && ((*ds==':') || (*ds==')')))ds++;
	if(esig)*esig=ds;

	return(tb-buf);
}
#endif
