#include <bgbgc.h>
#include <bgbdy.h>

void BGBDY_PrintFlattenItem(dysPrintStream strm, dyt obj)
{
	BGBGC_Object *pobj;
	dyt *pa;
	dyt cur, n1;
	char *ty;
	int i, j, n;

	if(!obj)return;
	if(obj==UNDEFINED)return;
	if(obj==BGBDY_FALSE)return;
	if(obj==BGBDY_TRUE)return;

	ty=dygettype(obj);

	if(!ty)return;

	if(!strcmp(ty, "_int_t") || !strcmp(ty, "_fixint_t") ||
		!strcmp(ty, "_long_t"))
		return;

	if(!strcmp(ty, "_float_t") || !strcmp(ty, "_double_t") ||
		!strcmp(ty, "_flonum_t"))
			return;

	if(!strcmp(ty, "_char_t")) return;

	if(!strcmp(ty, "_symbol_t")) return;
	if(!strcmp(ty, "_keyword_t")) return;
	if(!strcmp(ty, "_string_t")) return;
	if(!strcmp(ty, "_complex_t"))return;

	for(i=0; i<strm->n_marr; i++) if(obj==strm->marr[i])return;
	for(i=0; i<strm->n_sarr; i++) if(obj==strm->sarr[i])break;
	if(i<strm->n_sarr)
	{
		i=strm->n_marr++;
		strm->marr[i]=obj;
		return;
	}

	i=strm->n_sarr++;
	strm->sarr[i]=obj;

	if(dyconsp(obj))
	{
		cur=obj;
		while(dyconsp(cur))
		{
			BGBDY_PrintFlattenItem(strm, dycar(cur));
			cur=dycdr(cur);
		}

		if(cur)BGBDY_PrintFlattenItem(strm, cur);
		return;
	}

//	if(!strcmp(ty, "_array_t"))
	if(dyarrayp(obj))
	{
		pa=dyarrayv(obj); n=dyarraysz(obj);
		for(i=0; i<n; i++)
			BGBDY_PrintFlattenItem(strm, pa[i]);
		return;
	}

	if(dycArrayP(obj))
	{
		j=dycArraySz((dycArray)obj);
//		s=dycArraySig((dycArray)obj);
		for(i=0; i<j; i++)
		{
			n1=dycGetArrayVar((dycArray)obj, i);
			BGBDY_PrintFlattenItem(strm, n1);
		}
		return;
	}

	if(!strcmp(ty, "_object_t"))
	{
		pobj=(BGBGC_Object *)obj;

		j=0;
		for(i=0; i<pobj->cnt; i++)
		{
			if(!pobj->var[i])continue;
			BGBDY_PrintFlattenItem(strm, dytfUnwrap(pobj->val[i]));
		}

		for(i=0; i<pobj->dcnt; i++)
		{
			if(!pobj->dvar[i])continue;
			BGBDY_PrintFlattenItem(strm, pobj->dval[i]);
		}
		return;
	}
}

BGBDY_API void dysPrintf(dysPrintStream strm, char *str, ...)
{
	char buf[4096];
	va_list lst;

	va_start(lst, str);
	vsprintf(buf, str, lst);
	va_end(lst);

	strm->prints_f(strm->buf, buf);
}

void BGBDY_PrintDycObject(dysPrintStream strm, dycObject obj)
{
	dycSlot *slot;
	dycClass clz;
	char *s;
	dyt t;
	int i;
	
	clz=dycGetObjClass(obj);
	slot=dycEnumSlotArray(clz);

	s=dycGetClassName(clz);
	dysPrintf(strm, "#L<%s>", s);
	dysPrintf(strm, "{");

	for(i=0; slot[i]; i++)
	{
		if(i)dysPrintf(strm, " ");
		
		s=dycGetSlotName(slot[i]);
		dysPrintf(strm, "%s: ", s);
		t=dycGet(obj, slot[i]);
		dysPrintItem(strm, t);
	}

	dysPrintf(strm, "}");
	return;
}

void BGBDY_PrintItemI(dysPrintStream strm, dyt obj)
{
	dyt c, n1;
	dyt *an;
	void *p;
	char *tb, *s, *t, *ty;
	BGBGC_Object *pobj;
	dcomplex cf;
	double f, g;
	int i, j;

	if(!obj) { dysPrintf(strm, "()"); return; }
	if(obj==UNDEFINED) { dysPrintf(strm, "#u"); return; }
	if(obj==BGBDY_FALSE) { dysPrintf(strm, "#f"); return; }
	if(obj==BGBDY_TRUE) { dysPrintf(strm, "#t"); return; }

	if(dyconsp(obj))
	{
		dysPrintf(strm, "(");
		c=obj;
		while(dyconsp(c))
		{
			n1=dycdr(c);
			dysPrintItem(strm, dycar(c));
			if(n1)dysPrintf(strm, " ");
			c=n1;
		}

		if(c)
		{
			dysPrintf(strm, ". ");
			dysPrintItem(strm, c);
		}

		dysPrintf(strm, ")");
		return;
	}

//	if(!strcmp(ty, "_array_t"))
	if(dyarrayp(obj))
	{
		an=dyarrayv(obj);
		j=dyarraysz(obj);

		dysPrintf(strm, "#(");
		for(i=0; i<j; i++)
		{
			dysPrintItem(strm, an[i]);
			if((i+1)<j)dysPrintf(strm, " ");
		}

		dysPrintf(strm, ")");
		return;
	}

	if(dycArrayP(obj))
	{
		j=dycArraySz((dycArray)obj);
		s=dycArraySig((dycArray)obj);
		dysPrintf(strm, "#A<%s>(", s);
		for(i=0; i<j; i++)
		{
			n1=dycGetArrayVar((dycArray)obj, i);
			dysPrintItem(strm, n1);
			if((i+1)<j)dysPrintf(strm, " ");
		}

		dysPrintf(strm, ")");
		return;
	}

	if(dycObjectP(obj))
	{
		BGBDY_PrintDycObject(strm, (dycObject)obj);
		return;
	}

	ty=dygettype(obj);
	if(!ty)
	{
		dysPrintf(strm, "#!<NON-HEAP:%p>", obj);
		return;
	}

	if(!strcmp(ty, "_int_t") || !strcmp(ty, "_fixint_t"))
		{ dysPrintf(strm, "%d", dyintv(obj)); return; }
	if(!strcmp(ty, "_long_t"))
		{ dysPrintf(strm, "%lld", dylongv(obj)); return; }
	if(!strcmp(ty, "_int128_t"))
		{ dysPrintf(strm, "%sLX", xltoa_iv(dyint128v(obj))); return; }
	if(!strcmp(ty, "_float128_t"))
		{ dysPrintf(strm, "%sG", fv128_ftoa(dyfloat128v(obj))); return; }

	if(!strcmp(ty, "_char_t"))
	{
		i=dyintv(obj);
		if(((i>='A') && (i<='Z')) || ((i>='a') && (i<='z')))
			{ dysPrintf(strm, "#\\%c", i); return; }
		else if((i>=32) && (i<=126))
			{ dysPrintf(strm, "#\\'%c'", i); return; }
		else if((i>=0) && (i<=255))
			{ dysPrintf(strm, "#\\x%02X", i); return; }
		else if((i>=0) && (i<=65535))
			{ dysPrintf(strm, "#\\x%04X", i); return; }
	}

	if(!strcmp(ty, "_float_t") || !strcmp(ty, "_double_t") ||
		!strcmp(ty, "_flonum_t"))
	{
		f=dyrealv(obj); i=f;
		if(i==f)dysPrintf(strm, "%d.0", i);
			else dysPrintf(strm, "%f", f);
		return;
	}

	if(!strcmp(ty, "_symbol_t"))
	{
		if(BGBGC_PrintSymbolEscapeP(dysymv(obj)))
		{
			dysPrintf(strm, "#\"%s\"", BGBGC_FlattenString(dysymv(obj)));
			return;
		}

		dysPrintf(strm, "%s", BGBGC_FlattenString(dysymv(obj)));
		return;
	}

	if(!strcmp(ty, "_keyword_t"))
	{
		if(BGBGC_PrintSymbolEscapeP(dysymv(obj)))
		{
			dysPrintf(strm, "#:\"%s\"", BGBGC_FlattenString(dysymv(obj)));
			return;
		}

		dysPrintf(strm, "%s:", BGBGC_FlattenString(dysymv(obj)));
		return;
	}

	if(!strcmp(ty, "_string_t"))
	{
		dysPrintf(strm, "\"%s\"", BGBGC_FlattenString(dystringv(obj)));
		return;
	}

#if 0
	if(!strcmp(ty, "_complex_t"))
	{
		cf=dycomplexv(n);
		f=creal(cf); g=cimag(cf);

		if(g==0) { sprintf(buf, "%g", f); }
			else if(f==0) { sprintf(buf, "%gi", g); }
			else if(g<0) { sprintf(buf, "%g-%gi", f, -g); }
			else sprintf(buf, "%g+%gi", f, g);

		dysPrintf(strm, buf);
		return;
	}
#endif

	if(!strcmp(ty, "_object_t"))
	{
		pobj=(BGBGC_Object *)obj;

		dysPrintf(strm, "{");

		j=0;
		for(i=0; i<pobj->cnt; i++)
		{
			if(!pobj->var[i])continue;
			if(j)dysPrintf(strm, " "); j=1;
			dysPrintf(strm, "%s: ", pobj->var[i]);
			dysPrintItem(strm, dytfUnwrap(pobj->val[i]));
		}

		for(i=0; i<pobj->dcnt; i++)
		{
			if(!pobj->dvar[i])continue;
			if(j)dysPrintf(strm, " "); j=1;
			dysPrintf(strm, "%s: ", pobj->dvar[i]);
			dysPrintItem(strm, pobj->dval[i]);
		}

		dysPrintf(strm, "}");
		return;
	}

	if(!strcmp(ty, "_dyxnode_t"))
	{
		dysPrintf(strm, "#X");
		dyxPrintStrm(strm, (dyxNode *)obj);
		return;
	}

	p=BGBDY_GetVtabMethod(obj, "print(rr)v");
	if(p)
	{
		dysPrintf(strm, "#T\"%s\"", ty);
		((void(*)(dysPrintStream,dyt))p)(strm, obj);
		return;
	}

	s=BGBDY_ToString(obj);
	if(s)
	{
		dysPrintf(strm, "%s", s);
		return;
	}

	dysPrintf(strm, "#!<%s:%p>", ty, obj);
	return;
}

BGBDY_API void dysPrintItem(dysPrintStream strm, dyt obj)
{
	int i;

	for(i=0; i<strm->n_marr; i++)
		if(obj==strm->marr[i])
	{
		dysPrintf(strm, "#%d#", i);
		return;
	}

	BGBDY_PrintItemI(strm, obj);
}

BGBDY_API void dysPrintFlatten(dysPrintStream strm, dyt obj)
{
	int i;

	strm->sarr=gcalloc((1<<16)*sizeof(dyt));
	strm->marr=gcalloc((1<<12)*sizeof(dyt));
	strm->n_sarr=0; strm->n_marr=0;

	BGBDY_PrintFlattenItem(strm, obj);

	for(i=0; i<strm->n_marr; i++)
	{
		dysPrintf(strm, "#;#%d=", i);
		BGBDY_PrintItemI(strm, strm->marr[i]);
	}

	dysPrintItem(strm, obj);
	gcfree(strm->sarr); strm->sarr=NULL;
	gcfree(strm->marr); strm->marr=NULL;
}


static void dys_print_fd(void *p, char *b)
{
	fputs(b, (FILE *)p);
}

static void dys_print_str(void *p, char *b)
{
	char *s, **a;

	a=(char **)p;
	s=*a;
	strcpy(s, b);
	s+=strlen(s);
	*a=s;
}

static void dys_print_gcp(void *p, char *b)
{
	gc_printf("%s", b);
}

static void dys_print_lgcp(void *p, char *b)
{
	gc_logprintf("%s", b);
}

BGBDY_API dysPrintStream dysNewStreamFD(FILE *fd)
{
	dysPrintStream strm;

	strm=gctalloc("_dys_stream_t", sizeof(struct dysPrintStream_s));
	strm->buf=fd;
	strm->prints_f=dys_print_fd;
	return(strm);
}

BGBDY_API dysPrintStream dysNewStreamBuf(char **buf)
{
	dysPrintStream strm;

	strm=gctalloc("_dys_stream_t", sizeof(struct dysPrintStream_s));
	strm->buf=buf;
	strm->prints_f=dys_print_str;
	return(strm);
}

BGBDY_API dysPrintStream dysNewStreamGCP()
{
	dysPrintStream strm;

	strm=gctalloc("_dys_stream_t", sizeof(struct dysPrintStream_s));
	strm->buf=NULL;
	strm->prints_f=dys_print_gcp;
	return(strm);
}

BGBDY_API dysPrintStream dysNewStreamLGCP()
{
	dysPrintStream strm;

	strm=gctalloc("_dys_stream_t", sizeof(struct dysPrintStream_s));
	strm->buf=NULL;
	strm->prints_f=dys_print_lgcp;
	return(strm);
}

BGBDY_API void dysDestroyStream(dysPrintStream strm)
{
	gcfree(strm);
}

BGBDY_API dysPrintStream dysCloneStream(dysPrintStream strm)
{
	dysPrintStream strm2;
	strm2=gctalloc("_dys_stream_t", sizeof(struct dysPrintStream_s));
	strm2->buf=strm->buf;
	strm2->prints_f=strm->prints_f;
	return(strm2);
}

BGBDY_API void dysDumpFD(FILE *fd, dyt n)
{
	dysPrintStream strm;
	strm=dysNewStreamFD(fd); strm->ind=-1;
	dysPrintFlatten(strm, n);
	dysDestroyStream(strm);
}

BGBDY_API void dysPrintFD(FILE *fd, dyt n)
{
	dysPrintStream strm;
	strm=dysNewStreamFD(fd);
	dysPrintFlatten(strm, n);
	dysDestroyStream(strm);
}

BGBDY_API void dysPrintGCP(dyt n)
{
	dysPrintStream strm;
	strm=dysNewStreamGCP();
	dysPrintFlatten(strm, n);
	dysDestroyStream(strm);
}

BGBDY_API void dysPrintLGCP(dyt n)
{
	dysPrintStream strm;
	strm=dysNewStreamLGCP();
	dysPrintFlatten(strm, n);
	dysDestroyStream(strm);
}

BGBDY_API char *dysDumpStr(char *buf, dyt n)
{
	dysPrintStream strm;
	strm=dysNewStreamBuf(&buf); strm->ind=-1;
	dysPrintFlatten(strm, n);
	dysDestroyStream(strm);
	return(buf);
}

BGBDY_API char *dysPrintStr(char *buf, dyt n)
{
	dysPrintStream strm;
	strm=dysNewStreamBuf(&buf);
	dysPrintFlatten(strm, n);
	dysDestroyStream(strm);
	return(buf);
}

BGBDY_API void dysPrint(dyt n)
{
//	dysPrintFD(stdout, n);
	dysPrintGCP(n);
}

BGBDY_API void dysPrintln(dyt n)
{
//	dysPrintFD(stdout, n);
	dysPrintGCP(n);
	dyPrintf("\n");
}

