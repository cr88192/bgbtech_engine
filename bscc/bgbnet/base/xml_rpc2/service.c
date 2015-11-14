#include <bgbnet.h>

/*--
Cat pdnet;RPC;XML-RPC;Service
Text
	Basic services related to the protocol.
	Also handles extensions and such.
--*/

XMLRPC_Extension *xmlrpc_ext_root=NULL;

/*--
Cat pdnet;RPC;XML-RPC;Service
Form
	XMLRPC_Extension *XmlRpc_LookupExtension(char *ns);
Description
	Looks up a specific extension namespace.
--*/
XMLRPC_Extension *XmlRpc_LookupExtension(char *ns)
{
	XMLRPC_Extension *cur;

	cur=xmlrpc_ext_root;
	while(cur)
	{
		if(cur->xmlns)if(!strcmp(cur->xmlns, ns))
			return(cur);
		cur=cur->next;
	}
	return(NULL);
}

/*--
Cat pdnet;RPC;XML-RPC;Service
Form
	XMLRPC_Extension *XmlRpc_CreateExtension(char *ns);
Description
	Creates a new extension.
	ns specifies the namespace to be used for the extension.
	NULL is used for anonymous extensions...
--*/
XMLRPC_Extension *XmlRpc_CreateExtension(char *ns)
{
	XMLRPC_Extension *tmp;

	if(ns)tmp=XmlRpc_LookupExtension(ns);
		else tmp=NULL;
	if(tmp)return(tmp);

	tmp=gcalloc(sizeof(XMLRPC_Extension));
	if(ns)tmp->xmlns=dystrdup(ns);
	tmp->next=xmlrpc_ext_root;
	xmlrpc_ext_root=tmp;

	return(tmp);
}

void *XmlRpc_DecodeExtension(XMLRPC_Context *ctx, char *ns, dyxNode *node)
{
	XMLRPC_Extension *tmp;
	void *p;

	tmp=XmlRpc_LookupExtension(ns);
	if(!tmp)
	{
		ctx->error_code=-32001;
		ctx->error_str="server error: unknown extension namespace";
		return(NULL);
	}
	p=tmp->decode(ctx, node);
	if(!p)if(!ctx->error_code)
	{
		ctx->error_code=-32002;
		ctx->error_str="server error: decode failed";
	}
	return(p);
}

dyxNode *XmlRpc_EncodeValueExtension(XMLRPC_Context *ctx, void *val)
{
	int i;
	dyxNode *n, *n2;

	if(ctx->peer)if(ctx->peer->extensions)
		for(i=0; ctx->peer->extensions[i]; i++)
		if(ctx->peer->extensions[i]->encode)
	{
		n=ctx->peer->extensions[i]->encode(ctx, val);
		if(n)
		{
			n2=n;
			
			n=dyxNewNode();
			n->key=dystrdup("value");
			n->first=n2;
			dyxSet(n, "xmlns",
				ctx->peer->extensions[i]->xmlns);
			return(n);
		}
	}

	n2=XmlRpc_EncodeValue(val, ctx);
	if(!n2)return(n2);

	n=dyxNewNode();
	n->key=dystrdup("value");
	n->first=n2;
	return(n);
}

/*--
Cat pdnet;RPC;XML-RPC;Service
Form
	void *XmlRpc_Service_ListExtensionNamespaces();
Description
--*/
void *XmlRpc_Service_ListExtensionNamespaces()
{
	void **a;
	void *p;
	int i;
	XMLRPC_Extension *cur;

	a=gcralloc(256*sizeof(void *));
//	a[0]=NetVal_WrapString("http://bgb-sys.sourceforge.net/bgb-ext/types");

	i=0;
	cur=xmlrpc_ext_root;
	while(cur)
	{
		gc_printf("Ext %s\n", cur->xmlns);
		a[i++]=NetVal_WrapString(cur->xmlns);
		cur=cur->next;
	}
	a[i]=NULL;

	p=NetVal_WrapArray(a);
	return(p);
}

/*--
Cat pdnet;RPC;XML-RPC;Service
Form
	int XmlRpc_Service_Init();
Description
--*/
int XmlRpc_Service_Init()
{
	static int init=0;

	if(init)return(0);
	init=1;

	NET_ExportFunc("system.listExtensionNamespaces",
		&XmlRpc_Service_ListExtensionNamespaces);

	XMLRPC_EXT1_Init();
	return(0);
}
