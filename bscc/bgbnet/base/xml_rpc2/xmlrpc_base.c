#include <bgbnet.h>

#if defined(WIN32)
#include <windows.h>
#include <winsock.h>
#else
#include <netinet/in.h>
#endif

/*--
Cat pdnet;RPC;XML-RPC
Text
	Like Jabber-RPC, XML-RPC lacks all that much directly usable interface...
--*/

int xmlrpc_get(HTTP_Resource *rsrc, HTTP_Con *con, char **type, char **data, int *len)
{
	*type=dystrdup("text/plain");
	*data=dystrdup("PDLIB: XML-RPC: GET Unsuported\n");
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
	void *l;
	char tbuf[256];
	XMLRPC_Context *ctx;
	dyxNode *exp, *cur, *n, *n2;


	s=*data;
	exp=dyxParseParseExpr(&s);

	if(exp)if(exp->key)if(!strcmp(exp->key, "?xml"))
		exp=dyxParseParseExpr(&s);

	ctx=gcalloc(sizeof(XMLRPC_Context));

	if(!exp)
	{
		*type=dystrdup("text/plain");
		*data=dystrdup("PDLIB: XML-RPC: Parsed invalid request\n");
		*len=strlen(*data);
		return(403);
	}
	if(!exp->key)
	{
		*type=dystrdup("text/plain");
		*data=dystrdup("PDLIB: XML-RPC: Parsed invalid request\n");
		*len=strlen(*data);
		return(403);
	}

	if(!strcmp(exp->key, "methodCall"))
	{
		args=gcalloc(8*sizeof(void *));
		i=0;

		n=dyxFindKey(exp->first, "methodName");
		fcn=n->first->text;

		n=dyxFindKey(exp->first, "params");
		cur=n->first;
		while(cur)
		{
			args[i]=XmlRpc_DecodeValue(cur->first, ctx);
			i++;
			cur=cur->next;
		}
		args[i++]=NULL;

		gc_printf("XML-RPC: Call '%s'\n", fcn);

		l=NET_CallExport(fcn, args);

		if(!l)
		{
//			sprintf(tbuf, "PDLIB: XML-RPC: No Function '%s'\n", fcn);

//			*type=dystrdup("text/plain");
//			*data=dystrdup(tbuf);
//			*len=strlen(*data);
//			return(403);

			ctx->error_code=-32601;
			ctx->error_str="server error. requested method not found";
		}

		if(!ctx->error_code)
			n=XmlRpc_EncodeValue(l, ctx);

		if(!ctx->error_code)
		{
			n2=n;
			n=dyxNewNode();
			n->key=dystrdup("value");
			n->first=n2;

			n2=n;
			n=dyxNewNode();
			n->key=dystrdup("param");
			n->first=n2;

			n2=n;
			n=dyxNewNode();
			n->key=dystrdup("params");
			n->first=n2;

			n2=n;
			n=dyxNewNode();
			n->key=dystrdup("methodResponse");
			n->first=n2;
		}else
			n=xmlrpc_fault_make_top(ctx->error_code,
				ctx->error_str);

		s=gcalloc(16384);
		t=s;
		t=gcrsprints(t, "<?xml version=\"1.0\"?>\n");
//		t=gcrsprints(t, "<!-- test -->\n");
		t=dyxParsePrintExpr(t, n);
		t=gcrsprints(t, "\n");

		gc_printf("XML-RPC: send %s\n", s);

		*type=dystrdup("text/xml");
		*data=s;
		*len=strlen(s);
		return(200);
	}

	sprintf(tbuf, "PDLIB: XML-RPC: Unknown request type '%s'\n", exp->key);

	*type=dystrdup("text/plain");
	*data=dystrdup(tbuf);
	*len=strlen(*data);
	return(403);
}

int xmlrpc_head(HTTP_Resource *rsrc, HTTP_Con *con, char **type, int *len)
{
	*type=dystrdup("text/plain");
//	*data=dystrdup("PDLIB: XML-RPC: HEAD Unsuported\n");
//	*len=strlen(*data);
	*len=0;

	return(403);
}

NET_Reference *xmlrpc_decode_url(char *s)
{
	NET_Reference *ref;
	HTTP_RefInfo *ri;
	char *t;
	char buf[1024];
	VADDR *addr;

	ref=gctalloc("netref_t", 0);

	ri=gcalloc(sizeof(HTTP_RefInfo));
	ref->refinfo=ri;

	while(*s=='/')s++;
	t=buf;
	while(*s!='/')*t++=*s++;
	*t++=0;

	addr=NET_DecodeHostname(buf);
	if(!addr)
	{
		gc_printf("HTTP: Unknown host %s\n", buf);
		gcfree(ri);
		gcfree(ref);
		return(NULL);
	}
	switch(addr->proto)
	{
	case PROTO_IPV4:
	case PROTO_IPV4UDP:
	case PROTO_IPV4TCP:
		if(!addr->ipv4.port)
			addr->ipv4.port=htons(80);
		break;
	case PROTO_IPV6:
	case PROTO_IPV6UDP:
	case PROTO_IPV6TCP:
		if(!addr->ipv6.port)
			addr->ipv6.port=htons(80);
		break;
	default:
		break;
	}

	ri->addr=addr;
	ri->host=dystrdup(buf);

	t=buf;
	while(*s && (*s!='#'))*t++=*s++;
	*t++=0;
	ri->req=dystrdup(buf);

	if(*s=='#')s++;

	t=buf;
	while(*s)*t++=*s++;
	*t++=0;
	ri->ext=dystrdup(buf);

	return(ref);
}

char *xmlrpc_encode_url(NET_Reference *ref)
{
	HTTP_RefInfo *ri;
	char *s;
	char buf[1024];

	ri=ref->refinfo;
	sprintf(buf, "xml-rpc://%s%s#%s", ri->host, ri->req, ri->ext);
	s=dystrdup(buf);

	return(s);
}

void *xmlrpc_rpc_call(NET_Reference *ref, void **args)
{
	HTTP_RefInfo *ri;
	dyxNode *exp, *cur, *n, *n2, *lst;
	char *s, *t;
	int i;
	void *p;
	XMLRPC_Context *ctx;

	gc_printf("XML-RPC: call\n");

	ri=ref->refinfo;

	ctx=gcalloc(sizeof(XMLRPC_Context));

	lst=NULL;
	for(i=0; args[i]; i++)
	{
		n=XmlRpc_EncodeValue(args[i], ctx);

		n2=n;
		n=dyxNewNode();
		n->key=dystrdup("value");
		n->first=n2;

		n2=n;
		n=dyxNewNode();
		n->key=dystrdup("param");
		n->first=n2;

		lst=dyxAddNodeEnd(lst, n);
	}

	n2=n;
	n=dyxNewNode();
	n->key=dystrdup("params");
	n->first=lst;

	exp=n;

	n=dyxNewNode();
	n->text=dystrdup(ri->ext);

	n2=n;
	n=dyxNewNode();
	n->key=dystrdup("methodName");
	n->first=n2;
	n->next=exp;


	n2=n;
	n=dyxNewNode();
	n->key=dystrdup("methodCall");
	n->first=n2;

	s=gcalloc(16384);
	t=s;
	t=gcrsprints(t, "<?xml version=\"1.0\"?>\n");
	t=dyxParsePrintExpr(t, n);
	t=gcrsprints(t, "\n");

	t=HttpNode_Post(ref, "text/xml", s, strlen(s));
	gcfree(s);

	gc_printf("XML-RPC: got %s\n", t);

	s=t;
	exp=dyxParseParseExpr(&t);

	if(exp)if(exp->key)if(!strcmp(exp->key, "?xml"))
		exp=dyxParseParseExpr(&t);

	if(!exp)
	{
		gc_printf("PDLIB: XML-RPC: Parsed invalid response\n");
		return(NULL);
	}
	if(!exp->key)
	{
		gc_printf("PDLIB: XML-RPC: Parsed invalid response\n");
		return(NULL);
	}

	if(!strcmp(exp->key, "methodResponse"))
	{
		n=dyxFindKey(exp->first, "params");
		cur=n->first;
		p=XmlRpc_DecodeValue(cur->first, ctx);
		return(p);
	}

	return(NULL);
}

/*--
Cat pdnet;RPC;XML-RPC
Form
	int XmlRpc_Init();
Description
	Init function for XML-RPC.
--*/
int XmlRpc_Init()
{
	HTTP_Resource *rsrc;
	NET_Interface *iface;

	gc_printf("XML-RPC: Init\n");
	rsrc=HttpNode_NewResource("/RPC2");
	rsrc->get=&xmlrpc_get;
	rsrc->post=&xmlrpc_post;
	rsrc->head=&xmlrpc_head;

	iface=NET_CreateInterface("xml-rpc");
	iface->decode_url=&xmlrpc_decode_url;
	iface->encode_url=&xmlrpc_encode_url;
	iface->rpc_call=&xmlrpc_rpc_call;

//	JabberRPC_Init();

	XmlRpc_Service_Init();
//	Verify_Init();

	return(0);
}
