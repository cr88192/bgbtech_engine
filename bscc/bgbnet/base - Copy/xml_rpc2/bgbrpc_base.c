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
NetParse_Node *jabberrpc_results=NULL;

static NetParse_Node *xmlrpc_fault_make_name(char *name)
{
	NetParse_Node *n, *n2;

	n=NetParse_NewNode();
	n->text=kstrdup(name);
	n->first=n2;

	n2=n;
	n=NetParse_NewNode();
	n->key=kstrdup("name");
	n->first=n2;
	n->next=n3;

	return(n);
}

static NetParse_Node *xmlrpc_fault_make_ival(int val)
{
	NetParse_Node *n, *n2;
	char buf[64];

	ksprintf(buf, "%d", num);
	n=NetParse_NewNode();
	n->text=kstrdup(buf);
	n->first=n2;

	n2=n;
	n=NetParse_NewNode();
	n->key=kstrdup("int");
	n->first=n2;

	n2=n;
	n=NetParse_NewNode();
	n->key=kstrdup("value");
	n->first=n2;

	return(n);
}

static NetParse_Node *xmlrpc_fault_make_sval(char *s)
{
	NetParse_Node *n, *n2;

	n=NetParse_NewNode();
	n->text=kstrdup(s);
	n->first=n2;

	n2=n;
	n=NetParse_NewNode();
	n->key=kstrdup("string");
	n->first=n2;

	n2=n;
	n=NetParse_NewNode();
	n->key=kstrdup("value");
	n->first=n2;

	return(n);
}

int jabberrpc_send_fault(char *to, char *tag, int bgbrpc, int num, char *str)
{
	NetParse_Node *n, *n2, *n3;

	n2=xmlrpc_fault_make_sval(str);
	n=xmlrpc_fault_make_name("faultString");
	n->next=n2;

	n2=n;
	n=NetParse_NewNode();
	n->key=kstrdup("member");
	n->first=n2;

	n3=n;


	n2=xmlrpc_fault_make_ival(num);
	n=xmlrpc_fault_make_name("faultCode");
	n->next=n2;

	n2=n;
	n=NetParse_NewNode();
	n->key=kstrdup("member");
	n->first=n2;
	n->next=n3;

	n2=n;
	n=NetParse_NewNode();
	n->key=kstrdup("struct");
	n->first=n2;

	n2=n;
	n=NetParse_NewNode();
	n->key=kstrdup("value");
	n->first=n2;

	n2=n;
	n=NetParse_NewNode();
	n->key=kstrdup("fault");
	n->first=n2;

	n2=n;
	n=NetParse_NewNode();
	n->key=kstrdup("methodResponse");
	n->first=n2;


	n2=n;
	n=NetParse_NewNode();
	n->key=kstrdup("query");
	n->first=n2;
	if(!bgbrpc)
		NetParse_AddAttr(n, "xmlns", "jabber:iq:rpc");
		else NetParse_AddAttr(n, "xmlns",
			"http://bgb-sys.sourceforge.net/bgb-rpc");

	XMPP_SendIQ(to, "result", tag, n);
	NetParse_FreeNode(n);

	return(0);
}

int jabberrpc_handle_call(Meta0_Con *con, NetParse_Node *node)
{
	char *s, *t;
	char *fcn;
	char *from, *tag;

	char **argn;
	long *args;
	int i, bgbrpc;
	void *l;
	char tbuf[256];

	NetParse_Node *exp, *cur, *n, *n2;

	tag=NetParse_GetNodeAttr(node, "id");
	from=NetParse_GetNodeAttr(node, "from");

	bgbrpc=NetParse_GetNodeAttrIsP(node->first, "xmlns",
		"http://bgb-sys.sourceforge.net/bgb-rpc");

	if(!bgbrpc)
		kprint("PDLIB: Jabber-RPC: Recieved Call\n");
		else kprint("PDLIB: BGB-RPC/XMPP: Recieved Call\n");

	exp=node->first->first;

	if(!strcmp(exp->key, "methodCall"))
	{
		args=kalloc(8*sizeof(void *));
		i=0;

		n=NetParse_FindKey(exp->first, "methodName");
		fcn=n->first->text;

		n=NetParse_FindKey(exp->first, "params");
		cur=n->first;
		while(cur)
		{
			args[i]=XmlRpc_DecodeValue(cur->first);
			i++;
			cur=cur->next;
		}
		args[i++]=NULL;

		kprint("Jabber-RPC: Call '%s'\n", fcn);

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


		n2=n;
		n=NetParse_NewNode();
		n->key=kstrdup("query");
		n->first=n2;
		if(!bgbrpc)
			NetParse_AddAttr(n, "xmlns", "jabber:iq:rpc");
			else NetParse_AddAttr(n, "xmlns",
				"http://bgb-sys.sourceforge.net/bgb-rpc");

		XMPP_SendIQ(from, "result", tag, n);
		NetParse_FreeNode(n);
	}
	return(0);
}

int jabberrpc_iq(Meta0_Con *con, NetParse_Node *node)
{
	kprint("JabberRPC: IQ\n");

	if(NetParse_GetNodeAttrIsP(node, "type", "result"))
	{
		node=NetParse_CopyNode(node);
		node->next=jabberrpc_results;
		jabberrpc_results=node;
		kprint("JabberRPC: IQ: result OK\n");
		return(0);
	}
	if(NetParse_GetNodeAttrIsP(node, "type", "set"))
	{
		jabberrpc_handle_call(con, node);
		kprint("JabberRPC: IQ: set OK\n");
		return(0);
	}
	return(0);
}

NetParse_Node *JabberRPC_CheckResults(char *tag)
{
	NetParse_Node *cur, *lst;

	lst=NULL;
	cur=jabberrpc_results;
	while(cur)
	{
		if(NetParse_GetNodeAttrIsP(cur, "id", tag))
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

	ri=kalloc(sizeof(XMPP_RefInfo));
	ref->refinfo=ri;

	t=buf;
	while(*s!='@')*t++=*s++;
	if(*s=='@')s++;
	*t++=0;
	ri->node=kstrdup(buf);

	t=buf;
	while(*s!='/')*t++=*s++;
	if(*s=='/')s++;
	*t++=0;
	ri->host=kstrdup(buf);

	addr=XMPP_GetHostAddr(buf, 5222);

	ri->addr=addr;

	t=buf;
	while(*s && (*s!='#'))*t++=*s++;
	*t++=0;
	ri->rsrc=kstrdup(buf);

	if(*s=='#')
	{
		s++;

		t=buf;
		while(*s)*t++=*s++;
		*t++=0;
		ri->obj=kstrdup(buf);
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
		s=kprints(s, "jabber-rpc:%s@%s/%s", ri->node, ri->host, ri->rsrc);
		else s=kprints(s, "bgbrpc-jid:%s@%s/%s", ri->node, ri->host, ri->rsrc);
	if(ri->obj)s=kprints(s, "#%s", ri->obj);

	s=kstrdup(buf);

	return(s);
}

char *jabberrpc_encode_jid(XMPP_RefInfo *ri)
{
	char *s;
	char *buf;

	buf=kralloc(256);
	s=buf;
	s=kprints(s, "%s@%s/%s", ri->node, ri->host, ri->rsrc);
	s=krstrdup(buf);

	return(s);
}

void *jabberrpc_rpc_call(NET_Reference *ref, void **args)
{
	XMPP_RefInfo *ri;
	NetParse_Node *exp, *cur, *n, *n2, *lst;
	char *s, *t;
	int i;
	void *p;
	int bgbrpc;

	char *tag;

	kprint("Jabber-RPC: call\n");

	bgbrpc=!strcmp(ref->iface->name, "bgbrpc-jid");

	ri=ref->refinfo;

	lst=NULL;
	for(i=0; args[i]; i++)
	{
		if(!bgbrpc)
			n=XmlRpc_EncodeValue(args[i]);
			else n=BGBRPC_EncodeValue(args[i]);

		n2=n;
		n=NetParse_NewNode();
		n->key=kstrdup("value");
		n->first=n2;

		n2=n;
		n=NetParse_NewNode();
		n->key=kstrdup("param");
		n->first=n2;

		lst=NetParse_AddNodeEnd(lst, n);
	}

	n2=n;
	n=NetParse_NewNode();
	n->key=kstrdup("params");
	n->first=lst;

	exp=n;

	n=NetParse_NewNode();
	n->text=kstrdup(ri->obj);

	n2=n;
	n=NetParse_NewNode();
	n->key=kstrdup("methodName");
	n->first=n2;
	n->next=exp;

	n2=n;
	n=NetParse_NewNode();
	n->key=kstrdup("methodCall");
	n->first=n2;


	n2=n;
	n=NetParse_NewNode();
	n->key=kstrdup("query");
	n->first=n2;

	if(!bgbrpc)
		NetParse_AddAttr(n, "xmlns", "jabber:iq:rpc");
		else NetParse_AddAttr(n, "xmlns",
			"http://bgb-sys.sourceforge.net/bgb-rpc");

	tag=kalloc(16);
	kprints(tag, "rpc%d", jabberrpc_seq++);
	s=jabberrpc_encode_jid(ri);

	XMPP_SendIQ(s, "set", tag, n);
	NetParse_FreeNode(n);

//	s=kalloc(16384);
//	t=s;
//	t=kprints(t, "<?xml version=\"1.0\"?>\n");
//	t=NetParse_XML_PrintExpr(t, n);
//	t=kprints(t, "\n");

//	t=HttpNode_Post(ref, "text/xml", s, strlen(s));
//	kfree(s);

//	kprint("XML-RPC: got %s\n", t);

//	s=t;
//	exp=NetParse_XML_ParseExpr(&t);

//	if(exp)if(exp->key)if(!strcmp(exp->key, "?xml"))
//		exp=NetParse_XML_ParseExpr(&t);

	exp=NULL;
	while(!exp)
	{
		exp=JabberRPC_CheckResults(tag);
		NET_Poll();
	}

	if(!exp)
	{
		kprint("PDLIB: XML-RPC: Parsed invalid response\n");
		return(NULL);
	}
	if(!exp->key)
	{
		kprint("PDLIB: XML-RPC: Parsed invalid response\n");
		return(NULL);
	}

	if(!strcmp(exp->key, "methodResponse"))
	{
		n=NetParse_FindKey(exp->first, "params");
		cur=n->first;
		p=XmlRpc_DecodeValue(cur->first);
//		NetParse_FreeNode(exp);
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

	kprint("Jabber-RPC: Init\n");

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
