#include <bgbgc.h>
#include <bgbdy.h>

BGBDY_API dyxAttr *dyxLookupAttr(dyxNode *n, char *var)
{
	dyxAttr *cur;

	cur=n->attr;
	while(cur)
	{
		if(!cur->ns && !strcmp(cur->var, var))
			return(cur);
		cur=cur->next;
	}
	return(NULL);
}

BGBDY_API dyxAttr *dyxLookupAttrNS(dyxNode *n, char *ns, char *var)
{
	dyxAttr *cur;

	if(!ns || !*ns)return(dyxLookupAttr(n, var));

	cur=n->attr;
	while(cur)
	{
		if(cur->ns && !strcmp(cur->ns, ns) && !strcmp(cur->var, var))
			return(cur);
		cur=cur->next;
	}
	return(NULL);
}

BGBDY_API dyxAttr *dyxFetchAttr(dyxNode *n, char *var)
{
	dyxAttr *cur, *lst;

	cur=n->attr; lst=NULL;
	while(cur)
	{
		if(!cur->ns && !strcmp(cur->var, var))
			return(cur);
		lst=cur; cur=cur->next;
	}

	cur=gctalloc("_dyxattr_t", sizeof(dyxAttr));
	cur->var=dystrdup(var);
	if(lst)lst->next=cur; else n->attr=cur;

	return(cur);
}

BGBDY_API dyxAttr *dyxFetchAttrNS(dyxNode *n, char *ns, char *var)
{
	dyxAttr *cur, *lst;

	if(!ns || !*ns)return(dyxFetchAttr(n, var));

	cur=n->attr; lst=NULL;
	while(cur)
	{
		if(cur->ns && !strcmp(cur->ns, ns) && !strcmp(cur->var, var))
			return(cur);
		lst=cur; cur=cur->next;
	}

	cur=gctalloc("_dyxattr_t", sizeof(dyxAttr));
	cur->ns=dystrdup(ns);
	cur->var=dystrdup(var);
	if(lst)lst->next=cur; else n->attr=cur;

	return(cur);
}

BGBDY_API dyxAttr *dyxNewAttr()
{
	dyxAttr *cur;
	cur=gctalloc("_bccx_attr_t", sizeof(dyxAttr));
	return(cur);
}


BGBDY_API char *dyxGet(dyxNode *n, char *var)
{
	dyxAttr *cur;
	char *s;

	cur=dyxLookupAttr(n, var);
	if(cur)
	{
		if(cur->val==UNDEFINED)
		{
			if(cur->dyval)
			{
				return(dyToString(cur->dyval));
			}else
			if(((s64)cur->dval)==cur->dval)
			{
				s=gcrlltoa((s64)cur->dval);
				return(dystrdup(s));
			}else
			{
				s=gcrdtoa(cur->dval);
				return(dystrdup(s));
			}
		}
		return(cur->val);
	}
	return(NULL);
}

BGBDY_API int dyxGetInt(dyxNode *n, char *var)
{
	dyxAttr *cur;
	char *s;

	cur=dyxLookupAttr(n, var);
	if(cur && cur->val)
	{
		if(cur->val==UNDEFINED)
		{
			if(cur->dyval)
				{ return(dyintv(cur->dyval)); }
			else { return(cur->dval); }
		}
		return(atoi(cur->val));
	}
	return(0);

//	s=dyxGet(n, var);
//	return(s?atoi(s):0);
}

BGBDY_API double dyxGetFloat(dyxNode *n, char *var)
{
	dyxAttr *cur;
	char *s;

	cur=dyxLookupAttr(n, var);
	if(cur && cur->val)
	{
		if(cur->val==UNDEFINED)
		{
			if(cur->dyval)
				{ return(dyrealv(cur->dyval)); }
			else { return(cur->dval); }
		}
		return(atof(cur->val));
	}
	return(0);

//	char *s; s=dyxGet(n, var);
//	return(s?atof(s):0);
}

BGBDY_API char *dyxGetNS(dyxNode *n, char *ns, char *var)
{
	dyxAttr *cur;
	char *s;

	cur=dyxLookupAttrNS(n, ns, var);
	if(cur)
	{
		if(cur->val==UNDEFINED)
		{
			if(cur->dyval)
			{
				return(dyToString(cur->dyval));
			}else
			if(((s64)cur->dval)==cur->dval)
			{
				s=gcrlltoa((s64)cur->dval);
				return(dystrdup(s));
			}else
			{
				s=gcrdtoa(cur->dval);
				return(dystrdup(s));
			}
		}
		return(cur->val);
	}

//	if(cur)return(cur->val);
	return(NULL);
}

BGBDY_API int dyxGetIntNS(dyxNode *n, char *ns, char *var)
{
	char *s; s=dyxGetNS(n, ns, var);
	return(s?atoi(s):0);
}

BGBDY_API double dyxGetFloatNS(dyxNode *n, char *ns, char *var)
{
	char *s; s=dyxGetNS(n, ns, var);
	return(s?atof(s):0);
}

BGBDY_API void dyxSet(dyxNode *n, char *var, char *val)
{
	dyxAttr *cur;
	cur=dyxFetchAttr(n, var);
	cur->val=dystrdup(val);
	cur->dyval=NULL;
}

BGBDY_API void dyxSetInt(dyxNode *n, char *var, int val)
{
	dyxAttr *cur;
	cur=dyxFetchAttr(n, var);
	cur->val=UNDEFINED;
	cur->dval=val;
	cur->dyval=NULL;

//	char buf[64];
//	sprintf(buf, "%d", val);
//	dyxSet(n, var, buf);
}

BGBDY_API void dyxSetFloat(dyxNode *n, char *var, double val)
{
	dyxAttr *cur;
	cur=dyxFetchAttr(n, var);
	cur->val=UNDEFINED;
	cur->dval=val;
	cur->dyval=NULL;

//	char buf[64];
//	sprintf(buf, "%f", val);
//	dyxSet(n, var, buf);
}

BGBDY_API void dyxSetNS(dyxNode *n, char *ns, char *var, char *val)
{
	dyxAttr *cur;
	cur=dyxFetchAttrNS(n, ns, var);
	cur->val=dystrdup(val);
}

BGBDY_API void dyxSetIntNS(dyxNode *n, char *ns, char *var, int val)
{
	char buf[64];
	sprintf(buf, "%d", val);
	dyxSetNS(n, ns, var, buf);
}

BGBDY_API void dyxSetFloatNS(dyxNode *n, char *ns, char *var, double val)
{
	char buf[64];
	sprintf(buf, "%f", val);
	dyxSetNS(n, ns, var, buf);
}

BGBDY_API char *dyxNSURI(dyxNode *n, char *ns)
{
	char *s;

	if(!n)return(NULL);

	if(!ns || !*ns)
	{
		s=dyxGet(n, "xmlns");
		if(s)return(s);
	}else
	{
		s=dyxGetNS(n, "xmlns", ns);
		if(s)return(s);
	}

	s=dyxNSURI(n->up, ns);
	return(s);
}

BGBDY_API char *dyxURINS(dyxNode *n, char *uri)
{
	dyxAttr *cur;
	char *s;

	if(!n)return(NULL);

	cur=n->attr;
	while(cur)
	{
		if(cur->ns)
		{
			if(!strcmp(cur->ns, "xmlns"))
			{
				if(!strcmp(cur->val, uri))
					return(cur->var);
			}
		}else
		{
			if(!strcmp(cur->var, "xmlns"))
			{
				if(!strcmp(cur->val, uri))
					return("");
			}
		}
		cur=cur->next;
	}

	s=dyxURINS(n->up, uri);
	return(s);
}


BGBDY_API char *dyxGetURI(dyxNode *n, char *uri, char *var)
{
	dyxAttr *cur;
	char *ns; ns=dyxURINS(n, uri);
	cur=dyxLookupAttrNS(n, ns, var);
	if(cur)return(cur->val);
	return(NULL);
}

BGBDY_API int dyxGetIntURI(dyxNode *n, char *uri, char *var)
{
	char *s; s=dyxGetURI(n, uri, var);
	return(s?atoi(s):0);
}

BGBDY_API double dyxGetFloatURI(dyxNode *n, char *uri, char *var)
{
	char *s; s=dyxGetURI(n, uri, var);
	return(s?atof(s):0);
}

BGBDY_API void dyxSetURI(dyxNode *n, char *uri, char *var, char *val)
{
	dyxAttr *cur;
	char *ns; ns=dyxURINS(n, uri);
	cur=dyxFetchAttrNS(n, ns, var);
	cur->val=dystrdup(val);
}

BGBDY_API void dyxSetIntURI(dyxNode *n, char *uri, char *var, int val)
{
	char buf[64];
	sprintf(buf, "%d", val);
	dyxSetURI(n, uri, var, buf);
}

BGBDY_API void dyxSetFloatURI(dyxNode *n, char *uri, char *var, double val)
{
	char buf[64];
	sprintf(buf, "%f", val);
	dyxSetURI(n, uri, var, buf);
}


BGBDY_API dyxNode *dyxNew(char *tag)
{
	dyxNode *n;
	n=gctalloc("_dyxnode_t", sizeof(dyxNode));
	n->tag=dystrdup(tag);
	return(n);
}

BGBDY_API dyxNode *dyxNewNS(char *ns, char *tag)
{
	dyxNode *n;
	if(ns && !*ns)ns=NULL;
	n=gctalloc("_dyxnode_t", sizeof(dyxNode));
	n->ns=dystrdup(ns);
	n->tag=dystrdup(tag);
	return(n);
}

BGBDY_API dyxNode *dyxNewText(char *text)
{
	dyxNode *n;
	n=gctalloc("_dyxnode_t", sizeof(dyxNode));
	n->text=dystrdup(text);
	return(n);
}


BGBDY_API void dyxAdd(dyxNode *parent, dyxNode *child)
{
	dyxNode *cur;

	if(!child)return;

	if(child->prev || child->up)
	{
		printf("dyxAdd: link already linked\n");
//		*(int *)-1=-1;
//		return;
		child=dyxCloneList(child);
	}

	if(parent->down)
	{
		cur=parent->down;
		while(cur->next)cur=cur->next;
		cur->next=child;
		child->prev=cur;
	}else
	{
		parent->down=child;
	}

	cur=child;
	while(cur)
	{
		cur->up=parent;
		cur=cur->next;
	}
}

BGBDY_API dyxNode *dyxAddEnd(dyxNode *lst, dyxNode *n)
{
	dyxNode *cur;

	if(n->prev || n->up)
	{
		printf("dyxAddEnd: link already linked\n");
//		*(int *)-1=-1;
//		return;
		n=dyxCloneList(n);
	}

	if(!lst)return(n);
	if(lst->up)
	{
		dyxAdd(lst->up, n);
		return(lst);
	}

	cur=lst;
	while(cur->next)cur=cur->next;
	cur->next=n;
	cur->next->prev=cur;

	return(lst);
}

BGBDY_API dyxNode *dyxNew1(char *tag, dyxNode *a)
{
	dyxNode *n;
	n=dyxNew(tag); dyxAdd(n, a);
	return(n);
}

BGBDY_API dyxNode *dyxNew2(char *tag, dyxNode *a, dyxNode *b)
{
	dyxNode *n;
	n=dyxNew1(tag, a); dyxAdd(n, b);
	return(n);
}

BGBDY_API dyxNode *dyxNew3(char *tag, dyxNode *a, dyxNode *b, dyxNode *c)
{
	dyxNode *n;
	n=dyxNew2(tag, a, b); dyxAdd(n, c);
	return(n);
}

BGBDY_API dyxNode *dyxNew4(char *tag, dyxNode *a, dyxNode *b, dyxNode *c, dyxNode *d)
{
	dyxNode *n;
	n=dyxNew3(tag, a, b, c); dyxAdd(n, d);
	return(n);
}

BGBDY_API dyxNode *dyxNew1NS(char *ns, char *tag, dyxNode *a)
{
	dyxNode *n;
	n=dyxNewNS(ns, tag); dyxAdd(n, a);
	return(n);
}

BGBDY_API dyxNode *dyxNew2NS(char *ns, char *tag, dyxNode *a, dyxNode *b)
{
	dyxNode *n;
	n=dyxNew1NS(ns, tag, a); dyxAdd(n, b);
	return(n);
}

BGBDY_API dyxNode *dyxNew3NS(char *ns, char *tag, dyxNode *a, dyxNode *b, dyxNode *c)
{
	dyxNode *n;
	n=dyxNew2NS(ns, tag, a, b); dyxAdd(n, c);
	return(n);
}

BGBDY_API dyxNode *dyxNew4NS(char *ns, char *tag,
	dyxNode *a, dyxNode *b, dyxNode *c, dyxNode *d)
{
	dyxNode *n;
	n=dyxNew3NS(ns, tag, a, b, c); dyxAdd(n, d);
	return(n);
}

BGBDY_API int dyxNodeP(dyxNode *n)
{
	if(n->text)return(0);
	return(1);
}

BGBDY_API int dyxTextP(dyxNode *n)
{
	if(n->text)return(1);
	return(0);
}

BGBDY_API int dyxLeafP(dyxNode *n)
{
	if(n->text)return(0);
	if(n->down)return(0);
	return(1);
}

BGBDY_API int dyxChildP(dyxNode *n)
{
	if(n->text)return(0);
	if(n->down)return(1);
	return(0);
}

BGBDY_API dyxNode *dyxNext(dyxNode *n)	{ return(n->next); }
BGBDY_API dyxNode *dyxPrev(dyxNode *n)	{ return(n->prev); }
BGBDY_API dyxNode *dyxParent(dyxNode *n)	{ return(n->up); }
BGBDY_API dyxNode *dyxChild(dyxNode *n)	{ return(n->down); }

BGBDY_API char *dyxNS(dyxNode *n)		{ return(n->ns); }
BGBDY_API char *dyxURI(dyxNode *n)	{ return(dyxNSURI(n, n->ns)); }
BGBDY_API char *dyxTag(dyxNode *n)	{ return(n->tag); }
BGBDY_API char *dyxText(dyxNode *n)	{ return(n->text); }

BGBDY_API void dyxSetTag(dyxNode *n, char *s)
{
	n->tag=dystrdup(s);
}

BGBDY_API int dyxTagIsP(dyxNode *n, char *str)
{
	if(!n)return(0);
	if(!n->tag)return(0);
	if(!strcmp(n->tag, str))
		return(1);
	return(0);
}

BGBDY_API int dyxAttrIsP(dyxNode *n, char *var, char *val)
{
	char *s;

	if(!n)return(0);
	if(!n->tag)return(0);

	s=dyxGet(n, var);
	if(!s || !*s)
	{
		if(!val)return(1);
		if(!*val)return(1);
		return(0);
	}
	if(!val || !*val)return(0);
	if(!strcmp(s, val))
		return(1);
	return(0);
}

BGBDY_API int dyxTagAttrIsP(dyxNode *n, char *tag, char *var, char *val)
{
	char *s;

	if(!n)return(0);
	if(!n->tag)return(0);
	if(strcmp(n->tag, tag))
		return(0);

	s=dyxGet(n, var);
	if(!s || !*s)
	{
		if(!val)return(1);
		if(!*val)return(1);
		return(0);
	}
	if(!val || !*val)return(0);
	if(!strcmp(s, val))
		return(1);
	return(0);
}


BGBDY_API dyxNode *dyxFindNextTag(dyxNode *last, char *tag)
{
	dyxNode *cur;

	cur=last;
	while(cur)
	{
		if(!cur->ns && !strcmp(cur->tag, tag))
			return(cur);
		cur=cur->next;
	}
	return(NULL);
}

BGBDY_API dyxNode *dyxFindNextAttr(dyxNode *last, char *var, char *val)
{
	dyxNode *cur;

	cur=last;
	while(cur)
	{
		if(dyxAttrIsP(cur, var, val))
			return(cur);
		cur=cur->next;
	}
	return(NULL);
}

BGBDY_API dyxNode *dyxFindNextTagAttr(dyxNode *last,
	char *tag, char *var, char *val)
{
	dyxNode *cur;

	cur=last;
	while(cur)
	{
		if(!cur->ns && !strcmp(cur->tag, tag))
		{
			if(dyxAttrIsP(cur, var, val))
				return(cur);
		}
		cur=cur->next;
	}
	return(NULL);
}

BGBDY_API dyxNode *dyxFindTag(dyxNode *parent, char *tag)
	{ return(dyxFindNextTag(parent->down, tag)); }
BGBDY_API dyxNode *dyxFindAttr(dyxNode *parent, char *var, char *val)
	{ return(dyxFindNextAttr(parent->down, var, val)); }
BGBDY_API dyxNode *dyxFindTagAttr(dyxNode *parent, char *tag, char *var, char *val)
	{ return(dyxFindNextTagAttr(parent->down, tag, var, val)); }

BGBDY_API dyxNode *dyxFetch(dyxNode *parent, char *tag)
{
	dyxNode *tmp;

	tmp=dyxFindTag(parent, tag);
	if(tmp)return(dyxChild(tmp));
	return(NULL);
}

BGBDY_API dyxNode *dyxClone(dyxNode *n)
{
	dyxNode *t, *t1, *c;
	dyxAttr *ac;

	if(!n)return(NULL);

	t=gctalloc("_dyxnode_t", sizeof(dyxNode));
	t->ns=dystrdup(n->ns);
	t->tag=dystrdup(n->tag);
	t->text=dystrdup(n->text);

	ac=n->attr;
	while(ac)
	{
		if(ac->val==UNDEFINED)
		{
			dyxSetFloatNS(t, ac->ns, ac->var, ac->dval);
		}else
		{
			dyxSetNS(t, ac->ns, ac->var, ac->val);
		}
		ac=ac->next;
	}

	c=n->down;
	while(c)
	{
		t1=dyxClone(c);
		dyxAdd(t, t1);
		c=c->next;
	}

	return(t);	
}

BGBDY_API dyxNode *dyxCloneList(dyxNode *n)
{
	dyxNode *t, *c, *l;

	if(!n)return(NULL);

	c=n; l=NULL;
	while(c)
	{
		t=dyxClone(c);
		l=dyxAddEnd(l, t);
		c=c->next;
	}

	return(l);	
}

BGBDY_API void dyxUnlink(dyxNode *n)
{
	dyxNode *nu, *np, *nn;

	nu=n->up;
	np=n->prev;
	nn=n->next;

	if(!nu)return;
	if(nu->down==n)
	{
		nu->down=nn;
		if(nn)nn->prev=NULL;
	}else
	{
		if(np)np->next=nn;
		if(nn)nn->prev=np;
	}

	n->up=NULL;
	n->next=NULL;
	n->prev=NULL;
}

BGBDY_API void dyxFree(dyxNode *n)
{
	dyxNode *t, *t1, *c;
	dyxAttr *ac, *ta;

	if(!n)return;

	ac=n->attr;
	while(ac)
	{
		ta=ac->next;
		gcfree(ac);
		ac=ta;
	}

	c=n->down;
	while(c)
	{
		t=c->next;
		dyxFree(t);
		c=t;
	}
	gcfree(n);
}

BGBDY_API void dyxFreeList(dyxNode *n)
{
	dyxNode *t, *c;

	if(!n)return;

	c=n;
	while(c)
	{
		t=c->next;
		dyxFree(c);
		c=t;
	}
}
