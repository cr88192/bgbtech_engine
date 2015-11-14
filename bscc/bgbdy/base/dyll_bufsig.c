#include <bgbgc.h>
#include <bgbdy.h>

void dyllBufItemParseStrSig(void *buf, char **str, char **sig)
{
	char *s, *sg;

	s=*str;
	sg=*sig;

	switch(*sg++)
	{
	case 'a': *(signed char *)p=atoi(s); break;
	case 'b': case 'c':
		*(char *)p=atoi(s); break;
	case 'd': *(double *)p=atof(s); break;
	case 'e': *(long double *)p=atof(s); break;
	case 'f': *(float *)p=atof(s); break;

	case 'h': *(unsigned char *)p=atoi(s); break;
	case 'i': *(int *)p=atoi(s); break;
	case 'j': *(unsigned int *)p=atoi(s); break;

	case 'l': *(long *)p=atoi(s); break;
	case 'm': *(unsigned long *)p=atoi(s); break;


	case 'r': *(dyt *)p=dysParseFromString(s); break;

	case 's': *(short *)p=atoi(s); break;
	case 't': *(unsigned short *)p=atoi(s); break;

	case 'v': break;

	case 's': *(short *)p=atoi(s); break;

	case 'x': *(long long *)p=atoll(s); break;
	case 'y': *(unsigned long long *)p=atoll(s); break;
	}

	*str=s;
	*sig=sg;

#if 0
	if(!strcmp(sig, "a"))
	{
		*(signed char *)p=atoi(s);
		return;
	}

//	if(!strcmp(sig, "Cd"))return(dydcomplex(*(dcomplex *)p));
//	if(!strcmp(sig, "Cf"))return(dyfcomplex(*(fcomplex *)p));

//	if(*sig=='P')return((dyt)(*(void **)p));
//	if(*sig=='L')return(*(dyt *)p);
//	if(*sig=='Q')return(*(dyt *)p);
#endif

	return;
}

void dyllBufParseStrSig(void *buf, char *str, char *sig)
{
}
