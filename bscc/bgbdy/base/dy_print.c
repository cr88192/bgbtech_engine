#include <bgbgc.h>
#include <bgbdy.h>

#include <math.h>
// #include <complex.h>

int BGBGC_PeekChar(char **str)
{
	unsigned char *s;
	int i;

	s=(unsigned char *)(*str);
	i=*s++;

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

int BGBGC_ParseChar(char **str)
{
	unsigned char *s;
	int i;

	s=(unsigned char *)(*str);
	i=*s++;

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

int BGBGC_EmitChar(char **str, int j)
{
	unsigned char *t;
	int i;

	t=(unsigned char *)(*str);

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

int BGBGC_ParseChar16(u16 **str)
{
	u16 *s;
	int i, j;

	s=*str;
	i=*s++;

	if((i>=0xD800) && (i<=0xDBFF))
	{
		//"proper" UTF-16 -> UTF-8
		j=*s++;
		i=((i-0xD800)<<10)|((j-0xDC00)&0x3FF);
		i+=0x10000;
	}

	if(i==0x10FFFF)i=0;
	return(i);
}

void BGBGC_EmitChar16(u16 **str, int i)
{
	u16 *t;
	int j;

	t=*str;
	if(i>=0x10000)
	{
		//encode as UTF-16 surrogate pairs
		j=i-0x10000;
		*t++=0xD800+((j>>10)&0x3FF);
		*t++=0xDC00+(j&0x3FF);
		*str=t;
		return;
	}

	if(!i)
	{
		*t++=0xDBFF;
		*t++=0xDFFF;
	}else *t++=i;

	*str=t;
}

int BGBGC_ParseNibble(char **str)
{
	int i;

	i=BGBGC_ParseChar(str);
	if((i>='0') && (i<='9'))return(i-'0');
	if((i>='A') && (i<='F'))return(i-'A'+10);
	if((i>='a') && (i<='f'))return(i-'a'+10);
	return(-1);
}

int BGBGC_ParseHex(char **str)
{
	char *s;
	int i, j;

	s=*str; i=0;
	while(1)
	{
		j=BGBGC_ParseNibble(&s);
		if(j<0)break;
		*str=s; i=(i<<4)|j;
	}
	return(i);
}

int BGBGC_ParseHex8(char **str)
{
	int i;

	i=BGBGC_ParseNibble(str);
	i=(i<<4)|BGBGC_ParseNibble(str);
	return(i);
}

int BGBGC_ParseHex16(char **str)
{
	int i;

	i=BGBGC_ParseNibble(str);
	i=(i<<4)|BGBGC_ParseNibble(str);
	i=(i<<4)|BGBGC_ParseNibble(str);
	i=(i<<4)|BGBGC_ParseNibble(str);
	return(i);
}

int BGBGC_ParseHex32(char **str)
{
	int i;

	i=BGBGC_ParseNibble(str);
	i=(i<<4)|BGBGC_ParseNibble(str);
	i=(i<<4)|BGBGC_ParseNibble(str);
	i=(i<<4)|BGBGC_ParseNibble(str);

	i=(i<<4)|BGBGC_ParseNibble(str);
	i=(i<<4)|BGBGC_ParseNibble(str);
	i=(i<<4)|BGBGC_ParseNibble(str);
	i=(i<<4)|BGBGC_ParseNibble(str);
	return(i);
}

int BGBGC_EmitNibble(char **str, int i)
{
	int j;

	j='0';
	if((i>=0) && (i<=9)) j='0'+i;
	if((i>=10) && (i<=15)) j='A'+i;
	BGBGC_EmitChar(str, j);
	return(0);
}

int BGBGC_EmitHex8(char **str, int i)
{
	BGBGC_EmitNibble(str, (i>>4)&15);
	BGBGC_EmitNibble(str, i&15);
	return(0);
}

int BGBGC_EmitHex16(char **str, int i)
{
	BGBGC_EmitNibble(str, (i>>12)&15);
	BGBGC_EmitNibble(str, (i>>8)&15);
	BGBGC_EmitNibble(str, (i>>4)&15);
	BGBGC_EmitNibble(str, i&15);
	return(0);
}

int BGBGC_EmitHex32(char **str, int i)
{
	BGBGC_EmitNibble(str, (i>>28)&15);
	BGBGC_EmitNibble(str, (i>>24)&15);
	BGBGC_EmitNibble(str, (i>>20)&15);
	BGBGC_EmitNibble(str, (i>>16)&15);

	BGBGC_EmitNibble(str, (i>>12)&15);
	BGBGC_EmitNibble(str, (i>>8)&15);
	BGBGC_EmitNibble(str, (i>>4)&15);
	BGBGC_EmitNibble(str, i&15);
	return(0);
}

char *BGBGC_FlattenString(char *str)
{
	char *s, *t, *b;
	int i, j;

	s=str; j=0;
	while(*s)
	{
		i=BGBGC_ParseChar(&s);
		if((i<' ') || (i>=127))
			{ j+=6; continue; }
		j++;
	}

	b=gcralloc(j+1);
	s=str; t=b;
	while(*s)
	{
		i=BGBGC_ParseChar(&s);
		if((i<' ') || (i>=127))
		{
			if(i=='\r') { *t++='\\'; *t++='r'; continue; }
			if(i=='\n') { *t++='\\'; *t++='n'; continue; }
			if(i=='\t') { *t++='\\'; *t++='t'; continue; }

			if(i>0xFFFF) { *t++='\\'; *t++='U';
				BGBGC_EmitHex32(&t, i); continue; }
			if(i>0xFF) { *t++='\\'; *t++='u';
				BGBGC_EmitHex16(&t, i); continue; }
			*t++='\\'; *t++='x'; BGBGC_EmitHex8(&t, i); continue;
		}
		*t++=i;
	}

	*t++=0;
	return(b);
}

int BGBGC_PrintSpecialP(char *s)
{
	switch(*s)
	{
	case '(': case ')':
	case '[': case ']':
	case '{': case '}':
		return(1);
		break;

	case '\'': case '\\':
	case '`': case '#':
	case ',': case '@':
		return(1);
		break;

//	case '.':
//		return(1);
//		break;

	default:
		return(0);
		break;
	}
	return(0);
}

int BGBGC_PrintSymbolEscapeP(char *str)
{
	char *s;

	s=str;
	while(*s)
	{
		if(BGBGC_PrintSpecialP(s))
			return(1);
		s++;
	}
	return(0);
}

BGBDY_API void dyPrintDycObject(dycObject obj, int ind,
	void (*pb)(void *p, char *b), void *ob)
{
	char buf[256];
	dycSlot *slot;
	dycClass clz;
	char *s;
	dyt t;
	int i;
	
	clz=dycGetObjClass(obj);
	slot=dycEnumSlotArray(clz);

	s=dycGetClassName(clz);
	sprintf(buf, "#L<%s>", s);
	pb(ob, buf);

	pb(ob, "{");

	for(i=0; slot[i]; i++)
	{
		if(i)pb(ob, " ");
		
		s=dycGetSlotName(slot[i]);
		sprintf(buf, "%s: ", s); pb(ob, buf);
		t=dycGet(obj, slot[i]);
		dyPrintBuf(t, ind, pb, ob);
	}

	pb(ob, "}");
	return;
}

BGBDY_API void dyPrintBuf(dyt n, int ind,
	void (*pb)(void *p, char *b), void *ob)
{
	char buf[512];
	dyt c, n1;
	dyt *an;
	int *ai;
	char *tb, *s, *t, *ty;
	BGBGC_Object *obj;
	dcomplex cf;
	double f, g;
	int i, j, k;

//	if(!n) { pb(ob, "NULL"); return; }
//	if(n==UNDEFINED) { pb(ob, "UNDEFINED"); return; }
//	if(n==BGBDY_FALSE) { pb(ob, "FALSE"); return; }
//	if(n==BGBDY_TRUE) { pb(ob, "TRUE"); return; }

	if(!n) { pb(ob, "()"); return; }
	if(n==UNDEFINED) { pb(ob, "#u"); return; }
	if(n==BGBDY_FALSE) { pb(ob, "#f"); return; }
	if(n==BGBDY_TRUE) { pb(ob, "#t"); return; }

	if(dyconsp(n))
	{
		pb(ob, "(");
		c=n;
		while(dyconsp(c))
		{
			n1=dycdr(c);
			dyPrintBuf(dycar(c), ind, pb, ob);
			if(n1)pb(ob, " ");
			c=n1;
		}

		if(c)
		{
			pb(ob, ". ");
			dyPrintBuf(c, ind, pb, ob);
		}

		pb(ob, ")");
		return;
	}

	if(dyarrayp(n))
	{
		an=dyarrayv(n);
		j=dyarraysz(n);

		pb(ob, "#(");
		for(i=0; i<j; i++)
		{
			dyPrintBuf(an[i], ind, pb, ob);
			if((i+1)<j)pb(ob, " ");
		}

		pb(ob, ")");
		return;
	}

	if(dycArrayP(n))
	{
		j=dycArraySz((dycArray)n);
		s=dycArraySig((dycArray)n);
		sprintf(buf, "#A<%s>(", s);
		pb(ob, buf);
		for(i=0; i<j; i++)
		{
			n1=dycGetArrayVar((dycArray)n, i);
			dyPrintBuf(n1, ind, pb, ob);
			if((i+1)<j)pb(ob, " ");
		}

		pb(ob, ")");
		return;
	}

#if 0
	if(dycArrayNP(n))
	{
//		j=dycArraySz(n);
		ai=dycArrayNSz(n);
		j=dycArrayNOrd(n);
		s=dycArraySig(n);
		sprintf(buf, "#A%d<%s>(", j, s);
		pb(ob, buf);
		for(i=0; i<j; i++)
		{
			for(k=0; k<ai[i]; k++)
			n1=dycGetArrayVar(n, i);
			dyPrintBuf(n1, ind, pb, ob);
			if((i+1)<j)pb(ob, " ");
		}

		pb(ob, ")");
		return;
	}
#endif

	if(dycObjectP(n))
	{
		dyPrintDycObject((dycObject)n, ind, pb, ob);
		return;
	}

	ty=dygettype(n);
	if(!ty)
	{
		sprintf(buf, "#!<NON-HEAP:%p>", n);
		pb(ob, buf);
		return;
	}

#if 0
//	if(!strcmp(ty, "_array_t"))
	if(dyarrayp(n))
	{
		an=dyarrayv(n);
		j=dyarraysz(n);

		pb(ob, "#(");
		for(i=0; i<j; i++)
		{
			dyPrintBuf(an[i], ind, pb, ob);
			if((i+1)<j)pb(ob, " ");
		}

		pb(ob, ")");
		return;
	}
#endif

	if(!strcmp(ty, "_int_t") || !strcmp(ty, "_fixint_t"))
	{
		sprintf(buf, "%d", dyintv(n));
		pb(ob, buf);
		return;
	}

	if(!strcmp(ty, "_long_t"))
	{
		sprintf(buf, "%lld", dylongv(n));
		pb(ob, buf);
		return;
	}

	if(!strcmp(ty, "_float_t") || !strcmp(ty, "_double_t") ||
		!strcmp(ty, "_flonum_t"))
	{
		f=dyrealv(n); i=f;
		if(i==f)sprintf(buf, "%d.0", i);
			else sprintf(buf, "%f", f);
		pb(ob, buf);
		return;
	}

	if(!strcmp(ty, "_symbol_t"))
	{
		if(BGBGC_PrintSymbolEscapeP(dysymv(n)))
		{
			sprintf(buf, "#\"%s\"", BGBGC_FlattenString(dysymv(n)));
			pb(ob, buf);
			return;
		}

		sprintf(buf, "%s", BGBGC_FlattenString(dysymv(n)));
		pb(ob, buf);
		return;
	}

	if(!strcmp(ty, "_keyword_t"))
	{
		if(BGBGC_PrintSymbolEscapeP(dysymv(n)))
		{
			sprintf(buf, "#:\"%s\"", BGBGC_FlattenString(dysymv(n)));
			pb(ob, buf);
			return;
		}

		sprintf(buf, "%s:", BGBGC_FlattenString(dysymv(n)));
		pb(ob, buf);
		return;
	}

	if(!strcmp(ty, "_string_t"))
	{
		sprintf(buf, "\"%s\"", BGBGC_FlattenString(dystringv(n)));
		pb(ob, buf);
		return;
	}

	if(!strcmp(ty, "_complex_t"))
	{
		cf=dycomplexv(n);
		f=dtyDCreal(cf); g=dtyDCimag(cf);

		if(g==0) { sprintf(buf, "%g", f); }
			else if(f==0) { sprintf(buf, "%gi", g); }
			else if(g<0) { sprintf(buf, "%g-%gi", f, -g); }
			else sprintf(buf, "%g+%gi", f, g);

		pb(ob, buf);
		return;
	}

	if(!strcmp(ty, "_object_t"))
	{
		obj=(BGBGC_Object *)n;

		pb(ob, "{");

		j=0;
		for(i=0; i<obj->cnt; i++)
		{
			if(!obj->var[i])continue;
			if(j)pb(ob, " "); j=1;
			sprintf(buf, "%s: ", obj->var[i]); pb(ob, buf);
			dyPrintBuf(dytfUnwrap(obj->val[i]), ind, pb, ob);
		}

#if 0
		for(i=0; i<obj->dcnt; i++)
		{
			if(!obj->dvar[i])continue;
			if(j)pb(ob, " "); j=1;
			sprintf(buf, "%s: ", obj->dvar[i]); pb(ob, buf);
			dyPrintBuf(obj->dval[i], ind, pb, ob);
		}
#endif

		pb(ob, "}");
		return;

		return;
	}

	s=BGBDY_ToString(n);
	if(s)
	{
		pb(ob, s);
		return;
	}

	sprintf(buf, "#!<%s:%p>", ty, n);
	pb(ob, buf);
	return;
}

static void dy_print_fd(void *p, char *b)
{
	fputs(b, (FILE *)p);
}

static void dy_print_str(void *p, char *b)
{
	char *s, **a;

	a=(char **)p;
	s=*a;
	strcpy(s, b);
	s+=strlen(s);
	*a=s;
}

static void dy_print_gcp(void *p, char *b)
{
	gc_printf("%s", b);
}

static void dy_print_lgcp(void *p, char *b)
{
	gc_logprintf("%s", b);
}

BGBDY_API void dyDumpFD(FILE *fd, dyt n)
	{ dyPrintBuf(n, -1, dy_print_fd, fd); }
BGBDY_API void dyPrintFD(FILE *fd, dyt n)
	{ dyPrintBuf(n, 0, dy_print_fd, fd); }

BGBDY_API void dyDumpGCP(dyt n)
	{ dyPrintBuf(n, -1, dy_print_gcp, NULL); }
BGBDY_API void dyPrintGCP(dyt n)
	{ dyPrintBuf(n, 0, dy_print_gcp, NULL); }

BGBDY_API void dyDumpLGCP(dyt n)
	{ dyPrintBuf(n, -1, dy_print_lgcp, NULL); }
BGBDY_API void dyPrintLGCP(dyt n)
	{ dyPrintBuf(n, 0, dy_print_lgcp, NULL); }

BGBDY_API char *dyDumpStr(char *buf, dyt n)
{
	char *s;

	s=buf;
	dyPrintBuf(n, -1, dy_print_str, &s);
	return(s);
}

BGBDY_API char *dyPrintStr(char *buf, dyt n)
{
	char *s;

	s=buf;
	dyPrintBuf(n, 0, dy_print_str, &s);
	return(s);
}

BGBDY_API void dyPrint(dyt n)
{
//	dyPrintFD(stdout, n);
//	dyPrintGCP(n);
	dysPrintGCP(n);
}

BGBDY_API void dyDPrint(dyt n)
{
//	dyPrintFD(stdout, n);
//	dyPrintGCP(n);
	dysPrintLGCP(n);
}

