#include <bgbgc.h>
#include <bgbdy.h>

char *DYX_PrintZeText(char *s, char *t)
{
	int is;

	*t++='\"';

	is=0;
	while(*t)
	{
		switch(*t)
		{
//		case '<': strcpy(s, "&lt;"); s+=strlen(s); break;
//		case '>': strcpy(s, "&gt;"); s+=strlen(s); break;
//		case '&': strcpy(s, "&amp;"); s+=strlen(s); break;
//		case '\'': strcpy(s, "&apos;"); s+=strlen(s); break;
//		case '"': strcpy(s, "&quot;"); s+=strlen(s); break;

		case '\t': strcpy(s, "\\t"); s+=strlen(s); break;
		case '\r': strcpy(s, "\\r"); s+=strlen(s); break;
		case '\n': strcpy(s, "\\n"); s+=strlen(s); break;

//		case ' ':
//			if(!is) { strcpy(s, "&#32;"); s+=strlen(s); }
//				else *s++=' ';
//			is=0;
//			break;

		default:
			is=1;
			*s++=*t;
			break;
		}
		t++;
	}
	*t++='\"';
	*s=0;
	return(s);
}

BGBDY_API void dyxPrintZeBuf(dyxNode *node, int ind,
	void (*pb)(void *p, char *b), void *ob)
{
	char buf[512];
	dyxNode *ncur;
	dyxAttr *cur;
	char *tb, *t, *s;
	int i;

	if(!node)return;

	if(node->text)
	{
		i=strlen(node->text);
		if(ind>=0)i+=ind+8;

		tb=buf;
		if(i>256)tb=malloc(2*i);

		t=tb;
		for(i=0; i<ind; i++)*t++=' ';
		t=DYX_PrintZeText(t, node->text);
		if(ind>=0)*t++='\n';
		*t=0;

		pb(ob, tb);

		if(tb!=buf)free(tb);
		return;
	}

	if(!node->tag)
	{
		pb(ob, "<!--BAD-NODE-->");
		return;
	}

	tb=buf;

	i=4;
	if(ind>=0)i+=ind+8;
	if(node->ns)i+=strlen(node->ns)+1;
	i+=strlen(node->tag);

	cur=node->attr;
	while(cur)
	{
		if(cur->ns) i+=strlen(cur->ns)+1;
		i+=strlen(cur->var)+2;
		if(cur->val==UNDEFINED)i+=16;
		else i+=strlen(cur->val)+2;
		cur=cur->next;
	}

	if(i>=256)tb=malloc(2*i);


	t=tb;

	for(i=0; i<ind; i++)*t++=' ';

	*t++='<';
	if(node->ns) { sprintf(t, "%s:", node->ns); t+=strlen(t); }
	sprintf(t, "%s", node->tag); t+=strlen(t);

	cur=node->attr;
	while(cur)
	{
		if(!cur->val)
		{
			cur=cur->next;
			continue;
		}
		*t++=' ';
		if(cur->ns) { sprintf(t, "%s:", node->ns); t+=strlen(t); }
		sprintf(t, "%s=", cur->var); t+=strlen(t);
		if(cur->val==UNDEFINED)
		{
			s=gcrdtoa(cur->dval);
			sprintf(t, "%s", s); t+=strlen(t);
//			t=DYX_PrintZeText(t, s);
		}else
		{
			t=DYX_PrintZeText(t, cur->val);
		}
//		*t++='\"';
		*t=0;
		cur=cur->next;
	}

	if(!node->down)
	{
//		*t++='/'; *t++='>';
		*t++='>';
		if(ind>=0)*t++='\n';
		*t=0;

		pb(ob, tb);
		if(tb!=buf)free(tb);
		return;
	}

//	*t++='>';
	if(ind>=0)*t++='\n';
	*t=0;

	pb(ob, tb);

	i=ind; if(i>=0)i+=2;
	ncur=node->down;
	while(ncur)
	{
		dyxPrintZeBuf(ncur, i, pb, ob);
		ncur=ncur->next;
	}

	t=tb;
	for(i=0; i<ind; i++)*t++=' ';
//	*t++='<'; *t++='/';
//	if(node->ns) { sprintf(t, "%s:", node->ns); t+=strlen(t); }
//	sprintf(t, "%s", node->tag); t+=strlen(t);
	*t++='>';
	if(ind>=0)*t++='\n';
	*t=0;

	pb(ob, tb);

	if(tb!=buf)free(tb);
	return;
}

BGBDY_API void dyxPrintZeStrm(dysPrintStream strm, dyxNode *node)
{
	dyxPrintZeBuf(node, strm->ind, strm->prints_f, strm->buf);
}

static void dyx_print_fd(void *p, char *b)
{
	fputs(b, (FILE *)p);
}

static void dyx_print_vf(void *p, char *b)
{
	vfwrite(b, 1, strlen(b), (VFILE *)p);
}

static void dyx_print_str(void *p, char *b)
{
	char *s, **a;

	a=(char **)p;
	s=*a;
	strcpy(s, b);
	s+=strlen(s);
	*a=s;
}

BGBDY_API void dyxDumpZeFD(FILE *fd, dyxNode *node)
	{ dyxPrintZeBuf(node, -1, dyx_print_fd, fd); }
BGBDY_API void dyxPrintZeFD(FILE *fd, dyxNode *node)
	{ dyxPrintZeBuf(node, 0, dyx_print_fd, fd); }

BGBDY_API void dyxDumpZeVF(VFILE *fd, dyxNode *node)
	{ dyxPrintZeBuf(node, -1, dyx_print_vf, fd); }
BGBDY_API void dyxPrintZeVF(VFILE *fd, dyxNode *node)
	{ dyxPrintZeBuf(node, 0, dyx_print_vf, fd); }

BGBDY_API char *dyxDumpZeStr(char *buf, dyxNode *node)
{
	char *s;

	s=buf;
	dyxPrintZeBuf(node, -1, dyx_print_str, &s);
	return(s);
}

BGBDY_API char *dyxPrintZeStr(char *buf, dyxNode *node)
{
	char *s;

	s=buf;
	dyxPrintZeBuf(node, 0, dyx_print_str, &s);
	return(s);
}

BGBDY_API char *dyxPrintZeTextStr(char *buf, char *text)
{
	char *s;

	s=DYX_PrintText(buf, text);
	return(s);
}

BGBDY_API void dyxPrintZe(dyxNode *node)
{
	char *tbuf;
	
	tbuf=malloc(1<<24);
	dyxPrintZeStr(tbuf, node);
	gc_puts(tbuf);
	free(tbuf);

//	dyxPrintZeFD(stdout, node);
}

