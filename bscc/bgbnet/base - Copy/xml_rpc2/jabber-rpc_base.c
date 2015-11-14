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
XMLRPC_PeerInfo *jabberrpc_peers=NULL;

NetParse_Node *xmlrpc_fault_make_name(char *name)
{
	NetParse_Node *n, *n2;

	n=NetParse_NewNode();
	n->text=kstrdup(name);
	n->first=n2;

	n2=n;
	n=NetParse_NewNode();
	n->key=kstrdup("name");
	n->first=n2;

	return(n);
}

NetParse_Node *xmlrpc_fault_make_ival(int val)
{
	NetParse_Node *n, *n2;
	char buf[64];

	kprints(buf, "%d", val);
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

NetParse_Node *xmlrpc_fault_make_sval(char *s)
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

NetParse_Node *xmlrpc_fault_make_top(int num, char *str)
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

	return(n);
}

int jabberrpc_send_fault(char *to, char *tag, int num, char *str)
{
	NetParse_Node *n, *n2;

	n=xmlrpc_fault_make_top(num, str);

	n2=n;
	n=NetParse_NewNode();
	n->key=kstrdup("query");
	n->first=n2;
	NetParse_AddAttr(n, "xmlns", "jabber:iq:rpc");

	XMPP_SendIQ(to, "result", tag, n);
//	NetParse_FreeNode(n);

	return(0);
}

XMLRPC_PeerInfo *JabberRPC_FindPeer(char *name)
{
	XMLRPC_PeerInfo *cur;

	cur=jabberrpc_peers;
	while(cur)
	{
		if(!strcmp(cur->name, name))return(cur);
		cur=cur->next;
	}
	return(NULL);
}

XMLRPC_PeerInfo *JabberRPC_NewPeer(char *name)
{
	XMLRPC_PeerInfo *tmp;

	tmp=kalloc(sizeof(XMLRPC_PeerInfo));
	tmp->name=kstrdup(name);

	tmp->next=jabberrpc_peers;
	jabberrpc_peers=tmp;

	return(tmp);
}

int JabberRPC_CachePeerName(char *name)
{
	char *s, *t;
	NET_Reference *ref;
	XMPP_RefInfo *ri;
	XMLRPC_PeerInfo *peer;
	XMLRPC_Extension *ext;
	void *p, **a;
	int i, j;

	peer=JabberRPC_FindPeer(name);
	if(peer)return(0);

	peer=JabberRPC_NewPeer(name);

	t=kralloc(256);
	strcpy(t, name);
	strcat(t, "#system.listExtensionNamespaces");
	ref=jabberrpc_decode_url(t);

	p=jabberrpc_rpc_call(ref, NULL);
	if(!p)
	{
		peer->namespaces=kalloc(sizeof(char *));
		peer->extensions=kalloc(sizeof(XMLRPC_Extension *));
		peer->namespaces[0]=NULL;
		peer->extensions[0]=NULL;
		return(0);
	}

	a=(void **)(*(void **)p);
	for(i=0; a[i]; i++);

	peer->namespaces=kalloc((i+1)*sizeof(char *));
	peer->extensions=kalloc((i+1)*sizeof(XMLRPC_Extension *));
	j=0;
	for(i=0; a[i]; i++)
	{
		peer->namespaces[i]=kstrdup(a[i]);
		ext=XmlRpc_LookupExtension(a[i]);
		if(ext)peer->extensions[j++]=ext;
	}
	peer->namespaces[i]=NULL;
	peer->extensions[j]=NULL;

	return(0);
}

int JabberRPC_CachePeer(NET_Reference *ref)
{
	char *s;
	XMPP_RefInfo *ri;

	ri=ref->refinfo;

	s=jabberrpc_encode_jid(ri);
	JabberRPC_CachePeerName(s);
	return(0);
}

int jabberrpc_handle_call(Meta0_Con *con, NetParse_Node *node)
{
	char *s, *t;
	char *fcn;
	char *from, *tag;

	char **argn;
	long *args;
	int i;
	void *l;
	char tbuf[256];
	XMLRPC_Context *ctx;
	NetParse_Node *exp, *cur, *n, *n2;

	tag=NetParse_GetNodeAttr(node, "id");
	from=NetParse_GetNodeAttr(node, "from");

	kprintd("PDLIB: Jabber-RPC: Recieved Call\n");

	exp=node->first->first;
	ctx=kalloc(sizeof(XMLRPC_Context));
	ctx->peer=JabberRPC_FindPeer(from);

	if(!strcmp(exp->key, "methodCall"))
	{
		args=kalloc(8*sizeof(void *));
		i=0;

		n=NetParse_FindKey(exp->first, "methodName");
		fcn=n->first->text;

		if(strcmp(fcn, "system.listExtensionNamespaces") && !ctx->peer)
		{
			JabberRPC_CachePeerName(from);
			ctx->peer=JabberRPC_FindPeer(from);
		}

		n=NetParse_FindKey(exp->first, "params");
		cur=n->first;
		while(cur)
		{
			args[i]=XmlRpc_DecodeValue(cur->first, ctx);

			if(ctx->error_code)
			{
				jabberrpc_send_fault(from, tag,
					ctx->error_code, ctx->error_str);
				return(-1);
			}
			if(!args[i])
			{
				ctx->error_code=-32002;
				ctx->error_str="server error: decode failed";
				jabberrpc_send_fault(from, tag,
					ctx->error_code, ctx->error_str);
				return(-1);
			}

			i++;
			cur=cur->next;
		}
		args[i++]=NULL;

		kprintd("Jabber-RPC: Call '%s'\n", fcn);

		l=NET_CallExport(fcn, args);

		if(!l)
		{
//			sprintf(tbuf, "PDLIB: Jabber-RPC: No Function '%s'\n", fcn);
			jabberrpc_send_fault(from, tag, -32601,
				"server error. requested method not found");
			return(-1);
		}

//		n=XmlRpc_EncodeValue(l, ctx);
		n=XmlRpc_EncodeValueExtension(ctx, l);

//		n2=n;
//		n=NetParse_NewNode();
//		n->key=kstrdup("value");
//		n->first=n2;

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

		NetParse_AddAttr(n, "xmlns", "jabber:iq:rpc");

		XMPP_SendIQ(from, "result", tag, n);
		NetParse_FreeNode(n);
	}
	return(0);
}

int jabberrpc_iq(Meta0_Con *con, NetParse_Node *node)
{
	kprintd("JabberRPC: IQ\n");

	if(NetParse_GetNodeAttrIsP(node, "type", "result"))
	{
		node=NetParse_CopyNode(node);
		node->next=jabberrpc_results;
		jabberrpc_results=node;
		kprintd("JabberRPC: IQ: result OK\n");
		return(0);
	}
	if(NetParse_GetNodeAttrIsP(node, "type", "error"))
	{
		node=NetParse_CopyNode(node);
		node->next=jabberrpc_results;
		jabberrpc_results=node;
		kprintd("JabberRPC: IQ: error OK\n");
		return(0);
	}

	if(NetParse_GetNodeAttrIsP(node, "type", "set"))
	{
		jabberrpc_handle_call(con, node);
		kprintd("JabberRPC: IQ: set OK\n");
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

	ri=ref->refinfo;
	s=buf;
	s=kprints(s, "jabber-rpc:%s@%s/%s", ri->node, ri->host, ri->rsrc);
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

void *jabberrpc_rpc_call_func(NET_Reference *ref, char *func, void **args)
{
	XMPP_RefInfo *ri;
	NetParse_Node *exp, *cur, *n, *n2, *lst;
	char *s, *t;
	int i;
	void *p;
	char *tag;
	XMLRPC_Context *ctx;

	kprintd("Jabber-RPC: call\n");

	JabberRPC_CachePeer(ref);

	ri=ref->refinfo;

	ctx=kalloc(sizeof(XMLRPC_Context));
	ctx->peer=JabberRPC_FindPeer(jabberrpc_encode_jid(ri));

	lst=NULL;
	if(args)for(i=0; args[i]; i++)
	{
//		n=XmlRpc_EncodeValue(args[i], ctx);
		n=XmlRpc_EncodeValueExtension(ctx, args[i]);

//		n2=n;
//		n=NetParse_NewNode();
//		n->key=kstrdup("value");
//		n->first=n2;

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
	n->text=kstrdup(func);

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

	NetParse_AddAttr(n, "xmlns", "jabber:iq:rpc");

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

//	kprintd("XML-RPC: got %s\n", t);

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
		kprintd("PDLIB: XML-RPC: Parsed invalid response\n");
		return(NULL);
	}
	if(!exp->key)
	{
		kprintd("PDLIB: XML-RPC: Parsed invalid response\n");
		return(NULL);
	}

	if(!strcmp(exp->key, "methodResponse"))
	{
		n=NetParse_FindKey(exp->first, "params");
		if(!n)return(NULL);

		cur=n->first;
		p=XmlRpc_DecodeValue(cur->first, ctx);
//		NetParse_FreeNode(exp);
		return(p);
	}

	return(NULL);
}

void *jabberrpc_rpc_call(NET_Reference *ref, void **args)
{
	void *p;
	char *f;
	XMPP_RefInfo *ri;

	ri=ref->refinfo;
	f=ri->obj;
	if(!f)
	{
		kprintd("jabberrpc_rpc_call: ref does not specify function\n");
		return(NULL);
	}

	p=jabberrpc_rpc_call_func(ref, f, args);
	return(p);
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

	kprintd("Jabber-RPC: Init\n");

	xmpp_if=XMPP_CreateInterface("jabber:iq:rpc");
	xmpp_if->iq=&jabberrpc_iq;

	net_if=NET_CreateInterface("jabber-rpc");
	net_if->decode_url=&jabberrpc_decode_url;
	net_if->encode_url=&jabberrpc_encode_url;
	net_if->rpc_call=&jabberrpc_rpc_call;
	net_if->rpc_call_func=&jabberrpc_rpc_call_func;

	XmlRpc_Service_Init();

	return(0);
}
