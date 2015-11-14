#include <bgbccc.h>

BCCX_Attr *BCCX_LookupAttr(BCCX_Node *n, char *var)
{
	BCCX_Attr *cur;

	cur=n->attr;
	while(cur)
	{
		if(!cur->ns && !strcmp(cur->var, var))
			return(cur);
		cur=cur->next;
	}
	return(NULL);
}

BCCX_Attr *BCCX_LookupAttrNS(BCCX_Node *n, char *ns, char *var)
{
	BCCX_Attr *cur;

	if(!ns || !*ns)return(BCCX_LookupAttr(n, var));

	cur=n->attr;
	while(cur)
	{
		if(cur->ns && !strcmp(cur->ns, ns) && !strcmp(cur->var, var))
			return(cur);
		cur=cur->next;
	}
	return(NULL);
}

BCCX_Attr *BCCX_NewAttr()
{
	BCCX_Attr *cur;

	cur=bgbcc_tmalloc("_bccx_attr_t", sizeof(BCCX_Attr));
	return(cur);
}

BCCX_Attr *BCCX_FetchAttr(BCCX_Node *n, char *var)
{
	BCCX_Attr *cur, *lst;

	cur=n->attr; lst=NULL;
	while(cur)
	{
		if(!cur->ns && !strcmp(cur->var, var))
			return(cur);
		lst=cur; cur=cur->next;
	}

	cur=bgbcc_tmalloc("_bccx_attr_t", sizeof(BCCX_Attr));
	cur->var=bgbcc_strdup(var);
	if(lst)lst->next=cur; else n->attr=cur;

	return(cur);
}

BCCX_Attr *BCCX_FetchAttrNS(BCCX_Node *n, char *ns, char *var)
{
	BCCX_Attr *cur, *lst;

	if(!ns || !*ns)return(BCCX_FetchAttr(n, var));

	cur=n->attr; lst=NULL;
	while(cur)
	{
		if(cur->ns && !strcmp(cur->ns, ns) && !strcmp(cur->var, var))
			return(cur);
		lst=cur; cur=cur->next;
	}

	cur=bgbcc_tmalloc("_bccx_attr_t", sizeof(BCCX_Attr));
	cur->ns=bgbcc_strdup(ns);
	cur->var=bgbcc_strdup(var);
	if(lst)lst->next=cur; else n->attr=cur;

	return(cur);
}

char *BCCX_Get(BCCX_Node *n, char *var)
{
	BCCX_Attr *cur;

//	cur=BCCX_LookupAttr(n, var);
//	if(cur)return(cur->val);

	cur=n->attr;
	while(cur)
	{
		if(!strcmp(cur->var, var) && !cur->ns)
			return(cur->val);
		cur=cur->next;
	}

	return(NULL);
}

char *BCCX_Get_SI(BCCX_Node *n, char *var)
{
	BCCX_Attr *cur;

//	cur=BCCX_LookupAttr(n, var);
//	if(cur)return(cur->val);

	cur=n->attr;
	while(cur)
	{
		if((cur->var==var) && !cur->ns)
			return(cur->val);
		cur=cur->next;
	}

	return(NULL);
}

int BCCX_GetInt(BCCX_Node *n, char *var)
{
	char *s;
	s=BCCX_Get(n, var);
	return(s?BGBCC_ParseNumber(s):0);
//	return(s?atoi(s):0);
}

double BCCX_GetFloat(BCCX_Node *n, char *var)
{
	char *s; s=BCCX_Get(n, var);
	return(s?BGBCC_ParseNumber(s):0);
//	return(s?atof(s):0);
}

char *BCCX_GetNS(BCCX_Node *n, char *ns, char *var)
{
	BCCX_Attr *cur;
	cur=BCCX_LookupAttrNS(n, ns, var);
	if(cur)return(cur->val);
	return(NULL);
}

int BCCX_GetIntNS(BCCX_Node *n, char *ns, char *var)
{
	char *s; s=BCCX_GetNS(n, ns, var);
	return(s?atoi(s):0);
}

double BCCX_GetFloatNS(BCCX_Node *n, char *ns, char *var)
{
	char *s; s=BCCX_GetNS(n, ns, var);
	return(s?atof(s):0);
}

void BCCX_Set(BCCX_Node *n, char *var, char *val)
{
	BCCX_Attr *cur;
	cur=BCCX_FetchAttr(n, var);
	cur->val=bgbcc_strdup(val);
}

void BCCX_SetInt(BCCX_Node *n, char *var, int val)
{
	char buf[64];
	sprintf(buf, "%d", val);
	BCCX_Set(n, var, buf);
}

void BCCX_SetFloat(BCCX_Node *n, char *var, double val)
{
	char buf[64];
	sprintf(buf, "%f", val);
	BCCX_Set(n, var, buf);
}

void BCCX_SetNS(BCCX_Node *n, char *ns, char *var, char *val)
{
	BCCX_Attr *cur;
	cur=BCCX_FetchAttrNS(n, ns, var);
	cur->val=bgbcc_strdup(val);
}

void BCCX_SetIntNS(BCCX_Node *n, char *ns, char *var, int val)
{
	char buf[64];
	sprintf(buf, "%d", val);
	BCCX_SetNS(n, ns, var, buf);
}

void BCCX_SetFloatNS(BCCX_Node *n, char *ns, char *var, double val)
{
	char buf[64];
	sprintf(buf, "%f", val);
	BCCX_SetNS(n, ns, var, buf);
}

char *BCCX_NSURI(BCCX_Node *n, char *ns)
{
	char *s;

	if(!n)return(NULL);

	if(!ns || !*ns)
	{
		s=BCCX_Get(n, "xmlns");
		if(s)return(s);
	}else
	{
		s=BCCX_GetNS(n, "xmlns", ns);
		if(s)return(s);
	}

	s=BCCX_NSURI(n->up, ns);
	return(s);
}

char *BCCX_URINS(BCCX_Node *n, char *uri)
{
	BCCX_Attr *cur;
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

	s=BCCX_URINS(n->up, uri);
	return(s);
}


char *BCCX_GetURI(BCCX_Node *n, char *uri, char *var)
{
	BCCX_Attr *cur;
	char *ns; ns=BCCX_URINS(n, uri);
	cur=BCCX_LookupAttrNS(n, ns, var);
	if(cur)return(cur->val);
	return(NULL);
}

int BCCX_GetIntURI(BCCX_Node *n, char *uri, char *var)
{
	char *s; s=BCCX_GetURI(n, uri, var);
	return(s?atoi(s):0);
}

double BCCX_GetFloatURI(BCCX_Node *n, char *uri, char *var)
{
	char *s; s=BCCX_GetURI(n, uri, var);
	return(s?atof(s):0);
}

void BCCX_SetURI(BCCX_Node *n, char *uri, char *var, char *val)
{
	BCCX_Attr *cur;
	char *ns; ns=BCCX_URINS(n, uri);
	cur=BCCX_FetchAttrNS(n, ns, var);
	cur->val=bgbcc_strdup(val);
}

void BCCX_SetIntURI(BCCX_Node *n, char *uri, char *var, int val)
{
	char buf[64];
	sprintf(buf, "%d", val);
	BCCX_SetURI(n, uri, var, buf);
}

void BCCX_SetFloatURI(BCCX_Node *n, char *uri, char *var, double val)
{
	char buf[64];
	sprintf(buf, "%f", val);
	BCCX_SetURI(n, uri, var, buf);
}


BCCX_Node *BCCX_New(char *tag)
{
	BCCX_Node *n;
	n=bgbcc_tmalloc("_bccx_node_t", sizeof(BCCX_Node));
	n->tag=bgbcc_strdup(tag);
	return(n);
}

BCCX_Node *BCCX_NewNS(char *ns, char *tag)
{
	BCCX_Node *n;
	if(ns && !*ns)ns=NULL;
	n=bgbcc_tmalloc("_bccx_node_t", sizeof(BCCX_Node));
	n->ns=bgbcc_strdup(ns);
	n->tag=bgbcc_strdup(tag);
	return(n);
}

BCCX_Node *BCCX_NewText(char *text)
{
	BCCX_Node *n;
	n=bgbcc_tmalloc("_bccx_node_t", sizeof(BCCX_Node));
	n->text=bgbcc_strdup(text);
	return(n);
}


void BCCX_Add(BCCX_Node *parent, BCCX_Node *child)
{
	BCCX_Node *cur, *lst;

	if(!child)return;

	if(child->prev || child->up)
	{
//		printf("BCCX_Add: link already linked\n");
//		*(int *)-1=-1;
//		return;
		child=BCCX_CloneList(child);
	}

	if(parent->down)
	{
//		cur=parent->down;
//		while(cur->next)cur=cur->next;
		cur=parent->down_end;
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
		lst=cur; cur=cur->next;
	}
	parent->down_end=lst;
}

BCCX_Node *BCCX_AddEnd(BCCX_Node *lst, BCCX_Node *n)
{
	BCCX_Node *cur;

	if(n->prev || n->up)
	{
//		printf("BCCX_AddEnd: link already linked\n");
//		*(int *)-1=-1;
//		return;
		n=BCCX_CloneList(n);
	}

	if(!lst)return(n);
	if(lst->up)
	{
		BCCX_Add(lst->up, n);
		return(lst);
	}

	cur=lst;
	while(cur->next)cur=cur->next;
	cur->next=n;
	cur->next->prev=cur;

	return(lst);
}

BCCX_Node *BCCX_AddEnd2(BCCX_Node *fst, BCCX_Node **rlst, BCCX_Node *n)
{
	BCCX_Node *c, *t;

	if(!rlst)return(BCCX_AddEnd(fst, n));

	if(!fst)
	{
		c=n;
		while(c && c->next)
			c=c->next;
		*rlst=c;
		return(n);
	}

	if(fst->up || !(*rlst))
	{
		t=BCCX_AddEnd(fst, n);
		c=t; while(c && c->next)c=c->next;
		*rlst=c;
		return(t);
	}

	if(!n)return(fst);

	c=*rlst;
	n->prev=c; c->next=n;

	while(c && c->next)c=c->next;
	*rlst=c;
	return(fst);
}

BCCX_Node *BCCX_New1(char *tag, BCCX_Node *a)
{
	BCCX_Node *n;
	n=BCCX_New(tag); BCCX_Add(n, a);
	return(n);
}

BCCX_Node *BCCX_New2(char *tag, BCCX_Node *a, BCCX_Node *b)
{
	BCCX_Node *n;
	n=BCCX_New1(tag, a); BCCX_Add(n, b);
	return(n);
}

BCCX_Node *BCCX_New3(char *tag, BCCX_Node *a, BCCX_Node *b, BCCX_Node *c)
{
	BCCX_Node *n;
	n=BCCX_New2(tag, a, b); BCCX_Add(n, c);
	return(n);
}

BCCX_Node *BCCX_New4(char *tag, BCCX_Node *a, BCCX_Node *b, BCCX_Node *c, BCCX_Node *d)
{
	BCCX_Node *n;
	n=BCCX_New3(tag, a, b, c); BCCX_Add(n, d);
	return(n);
}

BCCX_Node *BCCX_New1NS(char *ns, char *tag, BCCX_Node *a)
{
	BCCX_Node *n;
	n=BCCX_NewNS(ns, tag); BCCX_Add(n, a);
	return(n);
}

BCCX_Node *BCCX_New2NS(char *ns, char *tag, BCCX_Node *a, BCCX_Node *b)
{
	BCCX_Node *n;
	n=BCCX_New1NS(ns, tag, a); BCCX_Add(n, b);
	return(n);
}

BCCX_Node *BCCX_New3NS(char *ns, char *tag, BCCX_Node *a, BCCX_Node *b, BCCX_Node *c)
{
	BCCX_Node *n;
	n=BCCX_New2NS(ns, tag, a, b); BCCX_Add(n, c);
	return(n);
}

BCCX_Node *BCCX_New4NS(char *ns, char *tag,
	BCCX_Node *a, BCCX_Node *b, BCCX_Node *c, BCCX_Node *d)
{
	BCCX_Node *n;
	n=BCCX_New3NS(ns, tag, a, b, c); BCCX_Add(n, d);
	return(n);
}

int BCCX_NodeP(BCCX_Node *n)
{
	if(n->text)return(0);
	return(1);
}

int BCCX_TextP(BCCX_Node *n)
{
	if(n->text)return(1);
	return(0);
}

int BCCX_LeafP(BCCX_Node *n)
{
	if(n->text)return(0);
	if(n->down)return(0);
	return(1);
}

int BCCX_ChildP(BCCX_Node *n)
{
	if(n->text)return(0);
	if(n->down)return(1);
	return(0);
}

BCCX_Node *BCCX_Next(BCCX_Node *n)	{ return(n->next); }
BCCX_Node *BCCX_Prev(BCCX_Node *n)	{ return(n->prev); }
BCCX_Node *BCCX_Parent(BCCX_Node *n)	{ return(n->up); }
BCCX_Node *BCCX_Child(BCCX_Node *n)	{ return(n->down); }

char *BCCX_NS(BCCX_Node *n)		{ return(n->ns); }
char *BCCX_URI(BCCX_Node *n)	{ return(BCCX_NSURI(n, n->ns)); }
char *BCCX_Tag(BCCX_Node *n)	{ return(n->tag); }
char *BCCX_Text(BCCX_Node *n)	{ return(n->text); }

void BCCX_SetTag(BCCX_Node *n, char *s)
{
	n->tag=bgbcc_strdup(s);
}

int BCCX_TagIsP(BCCX_Node *n, char *str)
{
	if(!n)return(0);
	if(!n->tag)return(0);
	if(!strcmp(n->tag, str))
		return(1);
	return(0);
}

int BCCX_AttrIsP(BCCX_Node *n, char *var, char *val)
{
	char *s;

	if(!n)return(0);
	if(!n->tag)return(0);

	s=BCCX_Get(n, var);
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

int BCCX_TagAttrIsP(BCCX_Node *n, char *tag, char *var, char *val)
{
	char *s;

	if(!n)return(0);
	if(!n->tag)return(0);
	if(strcmp(n->tag, tag))
		return(0);

	s=BCCX_Get(n, var);
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


BCCX_Node *BCCX_FindNextTag(BCCX_Node *last, char *tag)
{
	BCCX_Node *cur;

	cur=last;
	while(cur)
	{
		if(!cur->ns && !strcmp(cur->tag, tag))
			return(cur);
		cur=cur->next;
	}
	return(NULL);
}

BCCX_Node *BCCX_FindNextAttr(BCCX_Node *last, char *var, char *val)
{
	BCCX_Node *cur;

	cur=last;
	while(cur)
	{
		if(BCCX_AttrIsP(cur, var, val))
			return(cur);
		cur=cur->next;
	}
	return(NULL);
}

BCCX_Node *BCCX_FindNextTagAttr(BCCX_Node *last,
	char *tag, char *var, char *val)
{
	BCCX_Node *cur;

	cur=last;
	while(cur)
	{
		if(!cur->ns && !strcmp(cur->tag, tag))
		{
			if(BCCX_AttrIsP(cur, var, val))
				return(cur);
		}
		cur=cur->next;
	}
	return(NULL);
}

BCCX_Node *BCCX_FindTag(BCCX_Node *parent, char *tag)
	{ return(BCCX_FindNextTag(parent->down, tag)); }
BCCX_Node *BCCX_FindAttr(BCCX_Node *parent, char *var, char *val)
	{ return(BCCX_FindNextAttr(parent->down, var, val)); }
BCCX_Node *BCCX_FindTagAttr(BCCX_Node *parent, char *tag, char *var, char *val)
	{ return(BCCX_FindNextTagAttr(parent->down, tag, var, val)); }

BCCX_Node *BCCX_Fetch(BCCX_Node *parent, char *tag)
{
	BCCX_Node *tmp;

	tmp=BCCX_FindTag(parent, tag);
	if(tmp)return(BCCX_Child(tmp));
	return(NULL);
}

BCCX_Node *BCCX_Clone(BCCX_Node *n)
{
	BCCX_Node *t, *t1, *c, *l;
	BCCX_Attr *ac, *at, *al;

	if(!n)return(NULL);

	t=bgbcc_tmalloc("_bccx_node_t", sizeof(BCCX_Node));

//	t->ns=bgbcc_strdup(n->ns);
//	t->tag=bgbcc_strdup(n->tag);
//	t->text=bgbcc_strdup(n->text);

	t->ns=n->ns;
	t->tag=n->tag;
	t->text=n->text;

	ac=n->attr; al=NULL;
	while(ac)
	{
//		BCCX_SetNS(t, ac->ns, ac->var, ac->val);

		at=bgbcc_tmalloc("_bccx_attr_t", sizeof(BCCX_Attr));
		at->ns=ac->ns; at->var=ac->var; at->val=ac->val;
		if(al)al->next=at; else t->attr=at;
		al=at;

		ac=ac->next;
	}

	c=n->down; l=NULL;
	while(c)
	{
		t1=BCCX_Clone(c);
//		BCCX_Add(t, t1);

		if(l)
		{
			t1->up=t;
			l->next=t1;
			t1->prev=l;
			l=t1;
		}else
		{
			t1->up=t;
			t->down=t1;
			l=t1;
		}

		c=c->next;
	}
	t->down_end=l;

	return(t);	
}

BCCX_Node *BCCX_CloneList(BCCX_Node *n)
{
	BCCX_Node *t, *c, *ls, *le;

	if(!n)return(NULL);

	c=n; ls=NULL; le=NULL;
	while(c)
	{
		t=BCCX_Clone(c);
//		l=BCCX_AddEnd(l, t);

		if(le)
		{
			t->prev=le;
			le->next=t;
			le=t;
		}else
		{
			ls=t; le=t;
		}

		c=c->next;
	}

	return(ls);
}

void BCCX_Unlink(BCCX_Node *n)
{
	BCCX_Node *nu, *np, *nn;

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
