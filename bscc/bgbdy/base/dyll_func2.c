/*
This is related to "GenApply", which is basically a fallback apply
mechanism in-case the target lacks a better mechanism (ASM or similar).

ID numbers are used which can be run through a big-ass switch block, with
the numbers encoded using a base-5 scheme.

Type Numbers:
	0=Int(s32)
	1=Long(s64)
	2=Float(f32)
	3=Double(f64)
	4=Pointer(void*)
*/

#include <bgbdy.h>

#ifdef BGBDY_USEGENAPPLY

int DYLL_StepCalcSigID(char **str)
{
	char *s;
	int id;
	
	s=*str;
	*str=dyllSigNext(s);

	id=4;
	switch(*s++)
	{
	case 'a': case 'c': case 'h':
	case 's': case 't': case 'w':
	case 'b': case 'i': case 'j':
		id=0; break;
	case 'l': case 'm':
		id=1; break;
	case 'f':	id=2; break;
	case 'd':	id=3; break;
	case 'e':	id=3; break;
	case 'k':	id=2; break;

	case 'g': case 'n': case 'o':
		id=4; break;

	case 'r':
		id=4; break;

	case 'x': case 'y':
		id=1; break;

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
			id=4; break;
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
		id=4; break;
		break;

	case 'U':
	case 'X':
	case 'L':
		id=4; break;
		break;

	case 'Q':
		id=4; break;
		break;

	case 'A':
		if((*s>='0') && (*s<='9'))
		{
			id=4; break;
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
		break;

	case '(':
		id=4; break;

	default: break;
	}

	return(id);
}

int DYLL_CalcSigID(char *sig)
{
	char *s;
	int i, id;

	s=sig; id=1;
	if(*s=='(')s++;
	while(*s && (*s!=')'))
	{
		i=DYLL_StepCalcSigID(&s);
		id=id*5+i;
	}
	if(*s==')')
	{
		s++;
		i=DYLL_StepCalcSigID(&s);
		id=id*5+i;
	}
	return(id);
}

#endif
