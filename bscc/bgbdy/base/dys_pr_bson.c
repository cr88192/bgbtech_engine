#include <bgbgc.h>
#include <bgbdy.h>

void BGBDY_PrintBSON_FlattenItem(dysPrintStream strm, dyt obj)
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

	if(!strcmp(ty, "_int_t") || !strcmp(ty, "_fixint_t") ||
		!strcmp(ty, "_long_t") || !strcmp(ty, "_int128_t"))
		return;

	if(!strcmp(ty, "_float_t") || !strcmp(ty, "_double_t") ||
		!strcmp(ty, "_flonum_t") || !strcmp(ty, "_float128_t"))
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
			BGBDY_PrintBSON_FlattenItem(strm, dycar(cur));
			cur=dycdr(cur);
		}

		if(cur)BGBDY_PrintBSON_FlattenItem(strm, cur);
		return;
	}

//	if(!strcmp(ty, "_array_t"))
	if(dyarrayp(obj))
	{
		pa=dyarrayv(obj); n=dyarraysz(obj);
		for(i=0; i<n; i++)
			BGBDY_PrintBSON_FlattenItem(strm, pa[i]);
		return;
	}

	if(dycArrayP(obj))
	{
		j=dycArraySz((dycArray)obj);
//		s=dycArraySig((dycArray)obj);
		for(i=0; i<j; i++)
		{
			n1=dycGetArrayVar((dycArray)obj, i);
			BGBDY_PrintBSON_FlattenItem(strm, n1);
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
			BGBDY_PrintBSON_FlattenItem(strm, pobj->val[i]);
		}

		for(i=0; i<pobj->dcnt; i++)
		{
			if(!pobj->dvar[i])continue;
			BGBDY_PrintBSON_FlattenItem(strm, pobj->dval[i]);
		}
		return;
	}
}

void BGBDY_PrintBSON_DycObject(dysPrintStream strm, dycObject obj)
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
		dysBsonPrintItem(strm, t);
	}

	dysPrintf(strm, "}");
	return;
}

char *BGBDY_PrintBSON_SigSuffix(char *sig)
{
	char tb[256];
	char *str;
	
	switch(*sig)
	{
	case 'a': str='SB'; break;
	case 'b': str='TF'; break;
	case 'c': str='SC'; break;
	case 'd': str='D'; break;
	case 'e': str='LD'; break;
	case 'f': str='F'; break;
	case 'g': str='G'; break;
	case 'h': str='UB'; break;
	case 'i': str='SI'; break;
	case 'j': str='UI'; break;
	case 'k': str='SF'; break;
	case 'l': str='L'; break;
	case 'm': str='UL'; break;
	case 'n': str='LX'; break;
	case 'o': str='ULX'; break;
	//p/q
	case 'r': str='V'; break;
	case 's': str='SS'; break;
	case 't': str='US'; break;
	//u
//	case 'v': str='ZV'; break;
	case 'w': str='W'; break;
	case 'x': str='L'; break;
	case 'y': str='UL'; break;
//	case 'z': str='SI'; break;
	default: str=NULL;
	}
	
	if(str)return(str);
	
	return(NULL);
}

void BGBDY_PrintBSON_ItemI(dysPrintStream strm, dyt obj)
{
	dyt c, n1;
	dyt *an;
	void *p;
	char *tb, *s, *t, *ty;
	BGBGC_Object *pobj;
	dcomplex cf;
	double f, g;
	int i, j;

	if(!obj) { dysPrintf(strm, "null"); return; }
	if(obj==UNDEFINED) { dysPrintf(strm, "undefined"); return; }
	if(obj==BGBDY_FALSE) { dysPrintf(strm, "false"); return; }
	if(obj==BGBDY_TRUE) { dysPrintf(strm, "true"); return; }

	if(dyconsp(obj))
	{
		dysPrintf(strm, "#{");
		c=obj;
		while(dyconsp(c))
		{
			n1=dycdr(c);
			dysBsonPrintItem(strm, dycar(c));
			if(n1)dysPrintf(strm, ", ");
			c=n1;
		}

		if(c)
		{
			dysPrintf(strm, "; ");
			dysBsonPrintItem(strm, c);
		}

		dysPrintf(strm, "}");
		return;
	}

	if(dyarrayp(obj))
	{
		an=dyarrayv(obj);
		j=dyarraysz(obj);

		dysPrintf(strm, "[");
		for(i=0; i<j; i++)
		{
			dysBsonPrintItem(strm, an[i]);
			if((i+1)<j)dysPrintf(strm, ", ");
		}

		dysPrintf(strm, "]");
		return;
	}

	if(dycArrayP(obj))
	{
		j=dycArraySz((dycArray)obj);
//		s=dycArraySig((dycArray)obj);
		dysPrintf(strm, "[");
		for(i=0; i<j; i++)
		{
			n1=dycGetArrayVar((dycArray)obj, i);
			dysBsonPrintItem(strm, n1);
			if((i+1)<j)dysPrintf(strm, ", ");
		}

		s=dycArraySig((dycArray)obj);
		t=BGBDY_PrintBSON_SigSuffix(s);
		dysPrintf(strm, "]%s", t);
		return;
	}

	if(dycObjectP(obj))
	{
		BGBDY_PrintBSON_DycObject(strm, (dycObject)obj);
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
		{ dysPrintf(strm, "%lldL", dylongv(obj)); return; }
	if(!strcmp(ty, "_int128_t"))
		{ dysPrintf(strm, "%sLX", xltoa_iv(dyint128v(obj))); return; }
	if(!strcmp(ty, "_float128_t"))
		{ dysPrintf(strm, "%sG", fv128_ftoa(dyfloat128v(obj))); return; }

	if(!strcmp(ty, "_char_t"))
	{
		i=dyintv(obj);
		if((i>=32) && (i<=126))
			{ dysPrintf(strm, "'%c'", i); return; }
		else if((i>=0) && (i<=255))
			{ dysPrintf(strm, "#\\x%02X", i); return; }
		else if((i>=0) && (i<=65535))
			{ dysPrintf(strm, "#\\x%04X", i); return; }
	}

	if(!strcmp(ty, "_float_t") ||
		!strcmp(ty, "_flonum_t"))
	{
		f=dyrealv(obj); i=f;
		if(i==f)dysPrintf(strm, "%d.0", i);
			else dysPrintf(strm, "%f", f);
		return;
	}

	if(dyrealp(obj))
	{
		f=dyrealv(obj); i=f;
		if(i==f)dysPrintf(strm, "%d.0D", i);
			else dysPrintf(strm, "%fD", f);
		return;
	}

	if(!strcmp(ty, "_symbol_t"))
	{
		if(BGBGC_PrintSymbolEscapeP(dysymv(obj)))
		{
			dysPrintf(strm, "#\"%s\"", BGBGC_FlattenString(dysymv(obj)));
			return;
		}

		dysPrintf(strm, "#%s", BGBGC_FlattenString(dysymv(obj)));
		return;
	}

	if(!strcmp(ty, "_keyword_t"))
	{
		if(BGBGC_PrintSymbolEscapeP(dysymv(obj)))
		{
			dysPrintf(strm, "#:\"%s\"", BGBGC_FlattenString(dysymv(obj)));
			return;
		}

		dysPrintf(strm, "#:%s", BGBGC_FlattenString(dysymv(obj)));
		return;
	}

	if(!strcmp(ty, "_string_t"))
	{
		dysPrintf(strm, "\"%s\"", BGBGC_FlattenString(dystringv(obj)));
		return;
	}

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
			dysBsonPrintItem(strm, pobj->val[i]);
		}

		for(i=0; i<pobj->dcnt; i++)
		{
			if(!pobj->dvar[i])continue;
			if(j)dysPrintf(strm, " "); j=1;
			dysPrintf(strm, "%s: ", pobj->dvar[i]);
			dysBsonPrintItem(strm, pobj->dval[i]);
		}

		dysPrintf(strm, "}");
		return;
	}

	if(!strcmp(ty, "_dyxnode_t"))
	{
		dysPrintf(strm, "xml ");
		dyxPrintStrm(strm, (dyxNode *)obj);
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

BGBDY_API void dysBsonPrintItem(dysPrintStream strm, dyt obj)
{
	int i;

	for(i=0; i<strm->n_marr; i++)
		if(obj==strm->marr[i])
	{
		dysPrintf(strm, "_V%d", i);
		return;
	}

	BGBDY_PrintBSON_ItemI(strm, obj);
}

BGBDY_API void dysBsonPrintFlatten(dysPrintStream strm, dyt obj)
{
	int i;

	strm->sarr=gcalloc((1<<16)*sizeof(dyt));
	strm->marr=gcalloc((1<<12)*sizeof(dyt));
	strm->n_sarr=0; strm->n_marr=0;

	BGBDY_PrintBSON_FlattenItem(strm, obj);

	for(i=0; i<strm->n_marr; i++)
	{
		dysPrintf(strm, "_V%d=", i);
		BGBDY_PrintBSON_ItemI(strm, strm->marr[i]);
		dysPrintf(strm, "\n");
	}

	dysBsonPrintItem(strm, obj);
	gcfree(strm->sarr); strm->sarr=NULL;
	gcfree(strm->marr); strm->marr=NULL;
}


BGBDY_API void dysDumpBsonFD(FILE *fd, dyt n)
{
	dysPrintStream strm;
	strm=dysNewStreamFD(fd); strm->ind=-1;
	dysBsonPrintFlatten(strm, n);
	dysDestroyStream(strm);
}

BGBDY_API void dysBsonPrintFD(FILE *fd, dyt n)
{
	dysPrintStream strm;
	strm=dysNewStreamFD(fd);
	dysBsonPrintFlatten(strm, n);
	dysDestroyStream(strm);
}

BGBDY_API void dysBsonPrintGCP(dyt n)
{
	dysPrintStream strm;
	strm=dysNewStreamGCP();
	dysBsonPrintFlatten(strm, n);
	dysDestroyStream(strm);
}

BGBDY_API char *dysDumpBsonStr(char *buf, dyt n)
{
	dysPrintStream strm;
	strm=dysNewStreamBuf(&buf); strm->ind=-1;
	dysBsonPrintFlatten(strm, n);
	dysDestroyStream(strm);
	return(buf);
}

BGBDY_API char *dysBsonPrintStr(char *buf, dyt n)
{
	dysPrintStream strm;
	strm=dysNewStreamBuf(&buf);
	dysBsonPrintFlatten(strm, n);
	dysDestroyStream(strm);
	return(buf);
}
