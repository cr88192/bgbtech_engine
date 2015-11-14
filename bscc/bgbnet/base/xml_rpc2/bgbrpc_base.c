/*--
Cat pdnet;RPC;Jabber-RPC
Text
	Jabber-RPC is XML-RPC over XMPP.
	As such it will share much interface with XML-RPC.
	This will be primarily accessed through the RPC subsystem.
	The URL scheme is 'jabber-rpc', thus:
		jabber-rpc:<jid>[#<func>] represents a function.
		jid has the form <node>@<host>/<resource>.
	an example may then be:
		jabber-rpc:myself@localhost/pdlib#validator1.simpleStructReturnTest

	'bgbrpc-jid' is a variation of jabber-rpc.
	It uses a different scheme name, and differs from jabber-rpc \
	in that it has more types and uses a different namespace.
--*/

#include <bgbnet.h>

int jabberrpc_seq=1;
dyxNode *jabberrpc_results=NULL;

static dyxNode *xmlrpc_fault_make_name(char *name)
{
	dyxNode *n, *n2;

	n=dyxNewNode();
	n->text=dystrdup(name);
	n->first=n2;

	n2=n;
	n=dyxNewNode();
	n->key=dystrdup("name");
	n->first=n2;
	n->next=n3;

	return(n);
}

static dyxNode *xmlrpc_fault_make_ival(int val)
{
	dyxNode *n, *n2;
	char buf[64];

	ksprintf(buf, "%d", num);
	n=dyxNewNode();
	n->text=dystrdup(buf);
	n->first=n2;

	n2=n;
	n=dyxNewNode();
	n->key=dystrdup("int");
	n->first=n2;

	n2=n;
	n=dyxNewNode();
	n->key=dystrdup("value");
	n->first=n2;

	return(n);
}

static dyxNode *xmlrpc_fault_make_sval(char *s)
{
	dyxNode *n, *n2;

	n=dyxNewNode();
	n->text=dystrdup(s);
	n->first=n2;

	n2=n;
	n=dyxNewNode();
	n->key=dystrdup("string");
	n->first=n2;

	n2=n;
	n=dyxNewNode();
	n->key=dystrdup("value");
	n->first=n2;

	return(n);
}

int jabberrpc_send_fault(char *to, char *tag, int bgbrpc, int num, char *str)
{
	dyxNode *n, *n2, *n3;

	n2=xmlrpc_fault_make_sval(str);
	n=xmlrpc_fault_make_name("faultString");
	n->next=n2;

	n2=n;
	n=dyxNewNode();
	n->key=dystrdup("member");
	n->first=n2;

	n3=n;


	n2=xmlrpc_fault_make_ival(num);
	n=xmlrpc_fault_make_name("faultCode");
	n->next=n2;

	n2=n;
	n=dyxNewNode();
	n->key=dystrdup("member");
	n->first=n2;
	n->next=n3;

	n2=n;
	n=dyxNewNode();
	n->key=dystrdup("struct");
	n->first=n2;

	n2=n;
	n=dyxNewNode();
	n->key=dystrdup("value");
	n->first=n2;

	n2=n;
	n=dyxNewNode();
	n->key=dystrdup("fault");
	n->first=n2;

	n2=n;
	n=dyxNewNode();
	n->key=dystrdup("methodResponse");
	n->first=n2;


	n2=n;
	n=dyxNewNode();
	n->key=dystrdup("query");
	n->first=n2;
	if(!bgbrpc)
		dyxSet(n, "xmlns", "jabber:iq:rpc");
		else dyxSet(n, "xmlns",
			"http://bgb-sys.sourceforge.net/bgb-rpc");

	XMPP_SendIQ(to, "result", tag, n);
	dyxFree(n);

	return(0);
}

int jabberrpc_handle_call(Meta0_Con *con, dyxNode *node)
{
	char *s, *t;
	char *fcn;
	char *from, *tag;

	char **argn;
	long *args;
	int i, bgbrpc;
	void *l;
	char tbuf[256];

	dyxNode *exp, *cur, *n, *n2;

	tag=dyxGet(node, "id");
	from=dyxGet(node, "from");

	bgbrpc=dyxAttrIsP(node->first, "xmlns",
		"http://bgb-sys.sourceforge.net/bgb-rpc");

	if(!bgbrpc)
		gc_printf("PDLIB: Jabber-RPC: Recieved Call\n");
		else gc_printf("PDLIB: BGB-RPC/XMPP: Recieved Call\n");

	exp=node->first->first;

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
			args[i]=XmlRpc_DecodeValue(cur->first);
			i++;
			cur=cur->next;
		}
		args[i++]=NULL;

		gc_printf("Jabber-RPC: Call '%s'\n", fcn);

		l=NET_CallExport(fcn, args);

		if(!l)
		{
//			sprintf(tbuf, "PDLIB: Jabber-RPC: No Function '%s'\n", fcn);
			jabberrpc_send_fault(from, tag, bgbrpc, -32601,
				"server error. requested method not found");
			return(-1);
		}

		if(!bgbrpc)
			n=XmlRpc_EncodeValue(l);
			else n=BGBRPC_EncodeValue(l);

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


		n2=n;
		n=dyxNewNode();
		n->key=dystrdup("query");
		n->first=n2;
		if(!bgbrpc)
			dyxSet(n, "xmlns", "jabber:iq:rpc");
			else dyxSet(n, "xmlns",
				"http://bgb-sys.sourceforge.net/bgb-rpc");

		XMPP_SendIQ(from, "result", tag, n);
		dyxFree(n);
	}
	return(0);
}

int jabberrpc_iq(Meta0_Con *con, dyxNode *node)
{
	gc_printf("JabberRPC: IQ\n");

	if(dyxAttrIsP(node, "type", "result"))
	{
		node=dyxCopyNode(node);
		node->next=jabberrpc_results;
		jabberrpc_results=node;
		gc_printf("JabberRPC: IQ: result OK\n");
		return(0);
	}
	if(dyxAttrIsP(node, "type", "set"))
	{
		jabberrpc_handle_call(con, node);
		gc_printf("JabberRPC: IQ: set OK\n");
		return(0);
	}
	return(0);
}

dyxNode *JabberRPC_CheckResults(char *tag)
{
	dyxNode *cur, *lst;

	lst=NULL;
	cur=jabberrpc_results;
	while(cur)
	{
		if(dyxAttrIsP(cur, "id", tag))
		{
			if(lst)lst->next=cur->next;
				else jabberrpc_results=cur->next;
			return(cur->first->first);
		}
		lst=cur;
		cur=cur->next;
	}
	return(NULL);
}

NET_Reference *jabberrpc_decode_url(char *s)
{
	NET_Reference *ref;
	XMPP_RefInfo *ri;
	char *t;
	char buf[1024];
	VADDR *addr;

	ref=gctalloc("netref_t", 0);

	ri=gcalloc(sizeof(XMPP_RefInfo));
	ref->refinfo=ri;

	t=buf;
	while(*s!='@')*t++=*s++;
	if(*s=='@')s++;
	*t++=0;
	ri->node=dystrdup(buf);

	t=buf;
	while(*s!='/')*t++=*s++;
	if(*s=='/')s++;
	*t++=0;
	ri->host=dystrdup(buf);

	addr=XMPP_GetHostAddr(buf, 5222);

	ri->addr=addr;

	t=buf;
	while(*s && (*s!='#'))*t++=*s++;
	*t++=0;
	ri->rsrc=dystrdup(buf);

	if(*s=='#')
	{
		s++;

		t=buf;
		while(*s)*t++=*s++;
		*t++=0;
		ri->obj=dystrdup(buf);
	}

	return(ref);
}

char *jabberrpc_encode_url(NET_Reference *ref)
{
	XMPP_RefInfo *ri;
	char *s;
	char buf[1024];
	int bgbrpc;

	bgbrpc=!strcmp(ref->iface->name, "bgbrpc-jid");

	ri=ref->refinfo;
	s=buf;
	if(!bgbrpc)
		s=gcrsprints(s, "jabber-rpc:%s@%s/%s", ri->node, ri->host, ri->rsrc);
		else s=gcrsprints(s, "bgbrpc-jid:%s@%s/%s", ri->node, ri->host, ri->rsrc);
	if(ri->obj)s=gcrsprints(s, "#%s", ri->obj);

	s=dystrdup(buf);

	return(s);
}

char *jabberrpc_encode_jid(XMPP_RefInfo *ri)
{
	char *s;
	char *buf;

	buf=gcralloc(256);
	s=buf;
	s=gcrsprints(s, "%s@%s/%s", ri->node, ri->host, ri->rsrc);
	s=gcrstrdup(buf);

	return(s);
}

void *jabberrpc_rpc_call(NET_Reference *ref, void **args)
{
	XMPP_RefInfo *ri;
	dyxNode *exp, *cur, *n, *n2, *lst;
	char *s, *t;
	int i;
	void *p;
	int bgbrpc;

	char *tag;

	gc_printf("Jabber-RPC: call\n");

	bgbrpc=!strcmp(ref->iface->name, "bgbrpc-jid");

	ri=ref->refinfo;

	lst=NULL;
	for(i=0; args[i]; i++)
	{
		if(!bgbrpc)
			n=XmlRpc_EncodeValue(args[i]);
			else n=BGBRPC_EncodeValue(args[i]);

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
	n->text=dystrdup(ri->obj);

	n2=n;
	n=dyxNewNode();
	n->key=dystrdup("methodName");
	n->first=n2;
	n->next=exp;

	n2=n;
	n=dyxNewNode();
	n->key=dystrdup("methodCall");
	n->first=n2;


	n2=n;
	n=dyxNewNode();
	n->key=dystrdup("query");
	n->first=n2;

	if(!bgbrpc)
		dyxSet(n, "xmlns", "jabber:iq:rpc");
		else dyxSet(n, "xmlns",
			"http://bgb-sys.sourceforge.net/bgb-rpc");

	tag=gcalloc(16);
	gcrsprints(tag, "rpc%d", jabberrpc_seq++);
	s=jabberrpc_encode_jid(ri);

	XMPP_SendIQ(s, "set", tag, n);
	dyxFree(n);

//	s=gcalloc(16384);
//	t=s;
//	t=gcrsprints(t, "<?xml version=\"1.0\"?>\n");
//	t=dyxParsePrintExpr(t, n);
//	t=gcrsprints(t, "\n");

//	t=HttpNode_Post(ref, "text/xml", s, strlen(s));
//	gcfree(s);

//	gc_printf("XML-RPC: got %s\n", t);

//	s=t;
//	exp=dyxParseParseExpr(&t);

//	if(exp)if(exp->key)if(!strcmp(exp->key, "?xml"))
//		exp=dyxParseParseExpr(&t);

	exp=NULL;
	while(!exp)
	{
		exp=JabberRPC_CheckResults(tag);
		NET_Poll();
	}

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
		p=XmlRpc_DecodeValue(cur->first);
//		dyxFree(exp);
		return(p);
	}

	return(NULL);
}

/*--
Cat pdnet;RPC;Jabber-RPC
Form
	int JabberRPC_Init();
Description
	Init function for Jabber-RPC.
--*/
int JabberRPC_Init()
{
	XMPP_Interface *xmpp_if;
	NET_Interface *net_if;

	gc_printf("Jabber-RPC: Init\n");

	//jabber-rpc
	xmpp_if=XMPP_CreateInterface("jabber:iq:rpc");
	xmpp_if->iq=&jabberrpc_iq;

	net_if=NET_CreateInterface("jabber-rpc");
	net_if->decode_url=&jabberrpc_decode_url;
	net_if->encode_url=&jabberrpc_encode_url;
	net_if->rpc_call=&jabberrpc_rpc_call;


	//bgbrpc-jid
	xmpp_if=XMPP_CreateInterface("http://bgb-sys.sourceforge.net/bgb-rpc");
	xmpp_if->iq=&jabberrpc_iq;

	net_if=NET_CreateInterface("bgbrpc-jid");
	net_if->decode_url=&jabberrpc_decode_url;
	net_if->encode_url=&jabberrpc_encode_url;
	net_if->rpc_call=&jabberrpc_rpc_call;


	return(0);
}
