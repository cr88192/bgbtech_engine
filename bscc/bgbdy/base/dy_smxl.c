#include <bgbgc.h>
#include <bgbdy.h>

dyt *dy_smxl_qnhash;

dyt dyqsym(char *ns, char *name)
{
	int i, j, k, hi;
	dyt p, *pp, nss, nns;
	char *s;

	if(!dy_smxl_qnhash)
		dy_smxl_qnhash=gcweak(16384*sizeof(dyt));

	hi=0;
	if(ns) { s=ns; while(*s)hi=(hi*4093)+(*s++); hi=(hi*4093)+':'; }
	s=name;
	while(*s)hi=(hi*4093)+(*s++);
	hi*=4093;

	nss=dysymbol(ns);
	nns=dysymbol(name);

	j=hi;
	for(i=0; i<64; i++)
	{
		k=(j>>12)&16383;
		if(!dy_smxl_qnhash[k])
			continue;
		p=dy_smxl_qnhash[k];
		pp=(dyt *)p;
		if((pp[0]==nss) && (pp[1]==nns))
			return(p);
		j*=4093;
	}

	p=gctalloc("_qname_t", 2*sizeof(dyt));
	gcSet(((dyt *)p)[0], nss);
	gcSet(((dyt *)p)[1], nns);

	j=hi;
	for(i=0; i<64; i++)
	{
		k=(j>>12)&16383;
		if(!dy_smxl_qnhash[k])
			{ dy_smxl_qnhash[k]=p; break; }
		j*=4093;
	}

	return(p);
}

dyt dyqsymns(dyt p)
{
	return(((dyt *)p)[0]);
}

dyt dyqsymn(dyt p)
{
	return(((dyt *)p)[1]);
}

int dyqsymp(dyt p)
{
	return(dytypep(p, "_qname_t"));
}


dyt dytlAttr(dyt var, dyt val)
{
	dyt p;
	p=gctalloc("_tagattr_t", 2*sizeof(dyt));
	gcSet(((dyt *)p)[0], var);
	gcSet(((dyt *)p)[1], val);
	return(p);
}

dyt dytlAttrS(char *var, dyt val)
	{ return(dytlAttr(dysym(var), val)); }
dyt dytlAttrNS(char *ns, char *var, dyt val)
	{ return(dytlAttr(dyqsym(ns, var), val)); }

dyt dytlAttrVar(dyt p)
	{ return(((dyt *)p)[0]); }
dyt dytlAttrVal(dyt p)
	{ return(((dyt *)p)[1]); }
void dytlAttrSetVal(dyt p, dyt val)
	{ gcSet(((dyt *)p)[1], val); }

int dytlAttrP(dyt p)
	{ return(dytypep(p, "_tagattr_t")); }


dyt dytlTag(dyt tag, dyt attr, dyt rest)
{
	dyt p;
	p=gctalloc("_taglist_t", 3*sizeof(dyt));
	gcSet(((dyt *)p)[0], tag);
	gcSet(((dyt *)p)[1], attr);
	gcSet(((dyt *)p)[3], rest);
	return(p);
}

dyt dytlTagGetAttr(dyt tl, dyt var)
{
	dyt cur, p;

	cur=((dyt *)tl)[1];
	while(dyconsp(cur))
	{
		p=dycar(cur);
		if(dytlAttrVar(p)==var)
			return(dytlAttrVal(p));
		cur=dycdr(cur);
	}
	return(NULL);
}

void dytlTagSetAttr(dyt tl, dyt var, dyt val)
{
	dyt cur, p;

	cur=((dyt *)tl)[1];
	while(dyconsp(cur))
	{
		p=dycar(cur);
		if(dytlAttrVar(p)==var)
			{ dytlAttrSetVal(p, val); return; }
		cur=dycdr(cur);
	}

	p=((dyt *)tl)[1];
	gcSet(((dyt *)tl)[1], dycons(dytlAttr(var, val), p));
}

void dytlTagAddFirst(dyt tl, dyt val)
{
	dyt p;
	p=((dyt *)tl)[3];
	gcSet(((dyt *)tl)[3], dycons(val, p));
}

void dytlTagAddLast(dyt tl, dyt val)
{
	dyt cur, p;

	cur=((dyt *)tl)[3];
	if(cur)
	{
		while(dycdr(cur))
			cur=dycdr(cur);
		dysetcdr(cur, dylist1(val));
	}else gcSet(((dyt *)tl)[3], dylist1(val));
}

dyt dytlTagGetAttrS(dyt tl, char *var)
	{ return(dytlTagGetAttr(tl, dysym(var))); }
dyt dytlTagGetAttrNS(dyt tl, char *ns, char *var)
	{ return(dytlTagGetAttr(tl, dyqsym(ns, var))); }

void dytlTagSetAttrS(dyt tl, char *var, dyt val)
	{ dytlTagSetAttr(tl, dysym(var), val); }
void dytlTagSetAttrNS(dyt tl, char *ns, char *var, dyt val)
	{ dytlTagSetAttr(tl, dyqsym(ns, var), val); }

dyt dytlTagS(char *tag, dyt attr, dyt rest)
	{ return(dytlTag(dysym(tag), attr, rest)); }
dyt dytlTagNS(char *ns, char *tag, dyt attr, dyt rest)
	{ return(dytlTag(dyqsym(ns, tag), attr, rest)); }

dyt dytlTag0S(char *tag, dyt attr)
	{ return(dytlTagS(tag, attr, NULL)); }
dyt dytlTag1S(char *tag, dyt attr, dyt a)
	{ return(dytlTagS(tag, attr, dylist1(a))); }
dyt dytlTag2S(char *tag, dyt attr, dyt a, dyt b)
	{ return(dytlTagS(tag, attr, dylist2(a, b))); }
dyt dytlTag3S(char *tag, dyt attr, dyt a, dyt b, dyt c)
	{ return(dytlTagS(tag, attr, dylist3(a, b, c))); }
dyt dytlTag4S(char *tag, dyt attr, dyt a, dyt b, dyt c, dyt d)
	{ return(dytlTagS(tag, attr, dylist4(a, b, c, d))); }
dyt dytlTag5S(char *tag, dyt attr, dyt a, dyt b, dyt c, dyt d, dyt e)
	{ return(dytlTagS(tag, attr, dylist5(a, b, c, d, e))); }
dyt dytlTag6S(char *tag, dyt attr, dyt a, dyt b, dyt c, dyt d, dyt e, dyt f)
	{ return(dytlTagS(tag, attr, dylist6(a, b, c, d, e, f))); }

dyt dytlTag0NS(char *ns, char *tag, dyt attr)
	{ return(dytlTagNS(ns, tag, attr, NULL)); }
dyt dytlTag1NS(char *ns, char *tag, dyt attr, dyt a)
	{ return(dytlTagNS(ns, tag, attr, dylist1(a))); }
dyt dytlTag2NS(char *ns, char *tag, dyt attr, dyt a, dyt b)
	{ return(dytlTagNS(ns, tag, attr, dylist2(a, b))); }
dyt dytlTag3NS(char *ns, char *tag, dyt attr, dyt a, dyt b, dyt c)
	{ return(dytlTagNS(ns, tag, attr, dylist3(a, b, c))); }
dyt dytlTag4NS(char *ns, char *tag, dyt attr, dyt a, dyt b, dyt c, dyt d)
	{ return(dytlTagNS(ns, tag, attr, dylist4(a, b, c, d))); }

dyt dytlTagA0S(char *tag)
	{ return(dytlTagS(tag, NULL, NULL)); }
dyt dytlTagA1S(char *tag, dyt a)
	{ return(dytlTagS(tag, NULL, dylist1(a))); }
dyt dytlTagA2S(char *tag, dyt a, dyt b)
	{ return(dytlTagS(tag, NULL, dylist2(a, b))); }
dyt dytlTagA3S(char *tag, dyt a, dyt b, dyt c)
	{ return(dytlTagS(tag, NULL, dylist3(a, b, c))); }
dyt dytlTagA4S(char *tag, dyt a, dyt b, dyt c, dyt d)
	{ return(dytlTagS(tag, NULL, dylist4(a, b, c, d))); }
dyt dytlTagA5S(char *tag, dyt a, dyt b, dyt c, dyt d, dyt e)
	{ return(dytlTagS(tag, NULL, dylist5(a, b, c, d, e))); }
dyt dytlTagA6S(char *tag, dyt a, dyt b, dyt c, dyt d, dyt e, dyt f)
	{ return(dytlTagS(tag, NULL, dylist6(a, b, c, d, e, f))); }

dyt dytlTagA0NS(char *ns, char *tag)
	{ return(dytlTagNS(ns, tag, NULL, NULL)); }
dyt dytlTagA1NS(char *ns, char *tag, dyt a)
	{ return(dytlTagNS(ns, tag, NULL, dylist1(a))); }
dyt dytlTagA2NS(char *ns, char *tag, dyt a, dyt b)
	{ return(dytlTagNS(ns, tag, NULL, dylist2(a, b))); }
dyt dytlTagA3NS(char *ns, char *tag, dyt a, dyt b, dyt c)
	{ return(dytlTagNS(ns, tag, NULL, dylist3(a, b, c))); }
dyt dytlTagA4NS(char *ns, char *tag, dyt a, dyt b, dyt c, dyt d)
	{ return(dytlTagNS(ns, tag, NULL, dylist4(a, b, c, d))); }
dyt dytlTagA5NS(char *ns, char *tag, dyt a, dyt b, dyt c, dyt d, dyt e)
	{ return(dytlTagNS(ns, tag, NULL, dylist5(a, b, c, d, e))); }
dyt dytlTagA6NS(char *ns, char *tag, dyt a, dyt b, dyt c, dyt d, dyt e, dyt f)
	{ return(dytlTagNS(ns, tag, NULL, dylist6(a, b, c, d, e, f))); }

