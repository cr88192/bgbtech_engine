#include <bgbccc.h>

// #include <bgbdy.h>

char *BCCX_PrintText(char *s, char *t)
{
	int is;

	is=0;
	while(*t)
	{
		switch(*t)
		{
		case '<': strcpy(s, "&lt;"); s+=strlen(s); break;
		case '>': strcpy(s, "&gt;"); s+=strlen(s); break;
		case '&': strcpy(s, "&amp;"); s+=strlen(s); break;
		case '\'': strcpy(s, "&apos;"); s+=strlen(s); break;
		case '"': strcpy(s, "&quot;"); s+=strlen(s); break;

		case '\t': strcpy(s, "&#9;"); s+=strlen(s); break;
		case '\r': strcpy(s, "&#13;"); s+=strlen(s); break;
		case '\n': strcpy(s, "&#10;"); s+=strlen(s); break;

		case ' ':
			if(!is) { strcpy(s, "&#32;"); s+=strlen(s); }
				else *s++=' ';
			is=0;
			break;

		default:
			is=1;
			*s++=*t;
			break;
		}
		t++;
	}
	*s=0;
	return(s);
}

void BCCX_PrintBufSpecial(BCCX_Node *node, int ind,
	void (*pb)(void *p, char *b), void *ob)
{
	char buf[4096];
	BCCX_Node *ncur;
	BCCX_Attr *cur;
	char *tb, *t, *s;
	int i, j, k, sz;

	tb=buf;

	if(!strcmp(node->tag, "!CDATA"))
	{
		pb(ob, "<![CDATA[");
		pb(ob, node->text);
		pb(ob, "]]>");
		return;
	}

	if(!strcmp(node->tag, "!BDATA"))
	{
		sz=BCCX_GetInt(node, "size");
		
		s=node->text; t=tb;
		if((ind<0) || ((ind+16+(sz*3))<76))
		{
			t=tb;
			for(i=0; i<ind; i++)*t++=' ';
			sprintf(t, "<![BDATA["); t+=strlen(t);
			pb(ob, tb);

			for(i=0; i<sz; i++)
			{
				sprintf(tb, "%02X ", ((byte *)s)[i]);
				pb(ob, tb);
			}

			t=tb;
			sprintf(t, "]]>"); t+=strlen(t);
			if(ind>=0)*t++='\n';
			*t++=0;
			pb(ob, tb);
		}else
		{
			t=tb;
			for(i=0; i<ind; i++)*t++=' ';
			sprintf(t, "<![BDATA[\n"); t+=strlen(t);
			pb(ob, tb);

			k=(76-ind)/3;
			if(k>24)k=24;
			else if(k>16)k=16;
			else if(k>12)k=12;
			else if(k>8)k=8;
			else if(k>4)k=4;
			
			for(i=0; i<(sz/k); i++)
			{
				t=tb;
				for(j=0; j<ind; j++)*t++=' ';
				for(j=0; j<k; j++)
				{
					sprintf(t, "%02X ", *(byte *)s++);
					t+=strlen(t);
				}
				*t++='\n';
				*t++=0;
				pb(ob, tb);
			}
			if(sz%k)
			{
				t=tb;
				for(j=0; j<ind; j++)*t++=' ';
				for(i=0; i<(sz%k); i++)
				{
					sprintf(t, "%02X ", *(byte *)s++);
					t+=strlen(t);
				}
				*t++='\n';
				*t++=0;
				pb(ob, tb);
			}
			for(i=0; i<ind; i++)*t++=' ';
			sprintf(t, "]]>\n"); t+=strlen(t);
			pb(ob, tb);
		}
		return;
	}

	i=4;
	if(ind>=0)i+=ind+8;
	if(node->ns)i+=strlen(node->ns)+1;
	i+=strlen(node->tag);
	i+=strlen(node->text);

	cur=node->attr;
	while(cur)
	{
		if(!cur->var || !cur->val)
			{ cur=cur->next; continue; }
		if(cur->ns) i+=strlen(cur->ns)+1;
		i+=strlen(cur->var)+2;
		i+=strlen(cur->val)+2;
		cur=cur->next;
	}

	if(i>=256)tb=malloc(8*i);


	t=tb;

	for(i=0; i<ind; i++)*t++=' ';

	*t++='<';
	if(node->ns) { sprintf(t, "%s:", node->ns); t+=strlen(t); }
	sprintf(t, "%s", node->tag); t+=strlen(t);

	*t++=' ';
	t=BCCX_PrintText(t, node->text);

#if 0
	cur=node->attr;
	while(cur)
	{
		if(!cur->var || !cur->val)
			{ cur=cur->next; continue; }

		*t++=' ';
		if(cur->ns) { sprintf(t, "%s:", node->ns); t+=strlen(t); }
		sprintf(t, "%s=\"", cur->var); t+=strlen(t);
		t=BCCX_PrintText(t, cur->val);
		*t++='\"'; *t=0;
		cur=cur->next;
	}
#endif

	if(!node->down)
	{
//		*t++='/';

		if((*(node->tag))=='?')
			*t++='?';

		*t++='>';
		if(ind>=0)*t++='\n';
		*t=0;

		pb(ob, tb);
		if(tb!=buf)free(tb);
		return;
	}

	*t++='[';
	if(ind>=0)*t++='\n';
	*t=0;

	pb(ob, tb);

	i=ind; if(i>=0)i+=2;
	ncur=node->down;
	while(ncur)
	{
		BCCX_PrintBuf(ncur, i, pb, ob);
		ncur=ncur->next;
	}

	t=tb;
	for(i=0; i<ind; i++)*t++=' ';
	*t++=']';
	*t++='>';
	if(ind>=0)*t++='\n';
	*t=0;

	pb(ob, tb);

	if(tb!=buf)free(tb);
	return;
}

void BCCX_PrintBuf(BCCX_Node *node, int ind,
	void (*pb)(void *p, char *b), void *ob)
{
	char buf[4096];
	BCCX_Node *ncur;
	BCCX_Attr *cur;
	char *tb, *t;
	int i;

	if(!node)return;

	if(node->tag && node->text)
	{
		BCCX_PrintBufSpecial(node, ind, pb, ob);
		return;
	}

	if(node->text)
	{
		i=strlen(node->text);
		if(ind>=0)i+=ind+8;

		tb=buf;
		if(i>256)tb=malloc(2*i);

		t=tb;
		for(i=0; i<ind; i++)*t++=' ';
		t=BCCX_PrintText(t, node->text);
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
		if(!cur->var || !cur->val)
			{ cur=cur->next; continue; }
		if(cur->ns) i+=strlen(cur->ns)+1;
		i+=strlen(cur->var)+2;
		i+=strlen(cur->val)+2;
		cur=cur->next;
	}

	if(i>=256)tb=malloc(8*i);


	t=tb;

	for(i=0; i<ind; i++)*t++=' ';

	*t++='<';
	if(node->ns) { sprintf(t, "%s:", node->ns); t+=strlen(t); }
	sprintf(t, "%s", node->tag); t+=strlen(t);

	cur=node->attr;
	while(cur)
	{
		if(!cur->var || !cur->val)
			{ cur=cur->next; continue; }

		*t++=' ';
		if(cur->ns) { sprintf(t, "%s:", node->ns); t+=strlen(t); }
		sprintf(t, "%s=\"", cur->var); t+=strlen(t);
		t=BCCX_PrintText(t, cur->val);
		*t++='\"'; *t=0;
		cur=cur->next;
	}

	if(!node->down)
	{
		*t++='/'; *t++='>';
		if(ind>=0)*t++='\n';
		*t=0;

		pb(ob, tb);
		if(tb!=buf)free(tb);
		return;
	}

	*t++='>';
	if(ind>=0)*t++='\n';
	*t=0;

	pb(ob, tb);

	i=ind; if(i>=0)i+=2;
	ncur=node->down;
	while(ncur)
	{
		BCCX_PrintBuf(ncur, i, pb, ob);
		ncur=ncur->next;
	}

	t=tb;
	for(i=0; i<ind; i++)*t++=' ';
	*t++='<'; *t++='/';
	if(node->ns) { sprintf(t, "%s:", node->ns); t+=strlen(t); }
	sprintf(t, "%s", node->tag); t+=strlen(t);
	*t++='>';
	if(ind>=0)*t++='\n';
	*t=0;

	pb(ob, tb);

	if(tb!=buf)free(tb);
	return;
}

// void BCCX_PrintStrm(dysPrintStream strm, BCCX_Node *node)
// {
// 	BCCX_PrintBuf(node, strm->ind, strm->prints_f, strm->buf);
// }

static void bccx_print_fd(void *p, char *b)
{
	fputs(b, (FILE *)p);
}


static void bccx_print_str(void *p, char *b)
{
	char *s, **a;

	a=(char **)p;
	s=*a;
	strcpy(s, b);
	s+=strlen(s);
	*a=s;
}

void BCCX_DumpFD(FILE *fd, BCCX_Node *node)
	{ BCCX_PrintBuf(node, -1, bccx_print_fd, fd); }
void BCCX_PrintFD(FILE *fd, BCCX_Node *node)
	{ BCCX_PrintBuf(node, 0, bccx_print_fd, fd); }

// static void bccx_print_vf(void *p, char *b)
//	{ vfwrite(b, 1, strlen(b), (VFILE *)p); }

//void BCCX_DumpVF(void *fd, BCCX_Node *node)
//	{ BCCX_PrintBuf(node, -1, bccx_print_vf, fd); }
//void BCCX_PrintVF(void *fd, BCCX_Node *node)
//	{ BCCX_PrintBuf(node, 0, bccx_print_vf, fd); }

char *BCCX_DumpStr(char *buf, BCCX_Node *node)
{
	char *s;

	s=buf;
	BCCX_PrintBuf(node, -1, bccx_print_str, &s);
	return(s);
}

char *BCCX_PrintStr(char *buf, BCCX_Node *node)
{
	char *s;

	s=buf;
	BCCX_PrintBuf(node, 0, bccx_print_str, &s);
	return(s);
}

void BCCX_Print(BCCX_Node *node)
{
	BCCX_PrintFD(stdout, node);
}

