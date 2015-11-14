#include <bgbnet.h>

int xmlrpc_get(HTTP_Resource *rsrc, HTTP_Con *con, char **type, char **data, int *len)
{
	*type=kstrdup("text/plain");
	*data=kstrdup("PDLIB: XML-RPC: GET Unsuported\n");
	*len=strlen(*data);
	return(403);
}

int xmlrpc_post(HTTP_Resource *rsrc, HTTP_Con *con, char **type, char **data, int *len)
{
	char *s, *t;
	char *fcn;

	char **argn;
	long *args;
	int i;
	long l;
	char tbuf[256];

	NetParse_Node *exp, *cur, *n, *n2;


	s=*data;
	exp=NetParse_XML_ParseExpr(&s);

	if(exp)if(exp->key)if(!strcmp(exp->key, "?xml"))
		exp=NetParse_XML_ParseExpr(&s);

	if(!exp)
	{
		*type=kstrdup("text/plain");
		*data=kstrdup("PDLIB: XML-RPC: Parsed invalid request\n");
		*len=strlen(*data);
		return(403);
	}
	if(!exp->key)
	{
		*type=kstrdup("text/plain");
		*data=kstrdup("PDLIB: XML-RPC: Parsed invalid request\n");
		*len=strlen(*data);
		return(403);
	}

	if(!strcmp(exp->key, "methodCall"))
	{
		argn=kalloc(8*sizeof(char *));
		args=kalloc(8*sizeof(long));
		i=0;

		n=NetParse_FindKey(exp->first, "methodName");
		fcn=n->first->text;

		n=NetParse_FindKey(exp->first, "params");
		cur=n->first;
		while(cur)
		{
			XmlRpc_DecodeValue(cur->first, &argn[i], &args[i]);
			i++;
			cur=cur->next;
		}
		argn[i++]=NULL;

		l=NET_CallExport(fcn, &s, args, argn);

		if(!strcmp(s, "NoFunc"))
		{
			sprintf(tbuf, "PDLIB: XML-RPC: No Function '%s'\n", fcn);

			*type=kstrdup("text/plain");
			*data=kstrdup(tbuf);
			*len=strlen(*data);
			return(403);
		}

		n=XmlRpc_EncodeValue(s, l);

		n2=n;
		n=NetParse_NewNode();
		n->key=kstrdup("value");
		n->first=n2;

		n2=n;
		n=NetParse_NewNode();
		n->key=kstrdup("param");
		n->first=n2;

		n2=n;
		n=NetParse_NewNode();
		n->key=kstrdup("params");
		n->first=n2;

		n2=n;
		n=NetParse_NewNode();
		n->key=kstrdup("methodResponse");
		n->first=n2;

		s=kalloc(16384);
		t=s;
		t=kprints(t, "<?xml version=\"1.0\"?>\n");
//		t=kprints(t, "<!-- test -->\n");
		t=NetParse_XML_PrintExpr(t, n);
		t=kprints(t, "\n");

		*type=kstrdup("text/xml");
		*data=s;
		*len=strlen(s);
		return(200);
	}

	sprintf(tbuf, "PDLIB: XML-RPC: Unknown request type '%s'\n", exp->key);

	*type=kstrdup("text/plain");
	*data=kstrdup(tbuf);
	*len=strlen(*data);
	return(403);
}

int xmlrpc_head(HTTP_Resource *rsrc, HTTP_Con *con, char **type, int *len)
{
	*type=kstrdup("text/plain");
//	*data=kstrdup("PDLIB: XML-RPC: HEAD Unsuported\n");
//	*len=strlen(*data);
	*len=0;

	return(403);
}

int XmlRpc_Init()
{
	HTTP_Resource *rsrc;

	kprint("XML-RPC: Init\n");
	rsrc=HttpNode_NewResource("/RPC2");
	rsrc->get=&xmlrpc_get;
	rsrc->post=&xmlrpc_post;
	rsrc->head=&xmlrpc_head;

	Verify_Init();

	return(0);
}
