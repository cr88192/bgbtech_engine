#include <bgbnet.h>

/*--
Cat pdnet;RPC;XML-RPC;Encode;Ext-1
Form
	NetParse_Node *XmlRpc_EncodeVector(void *val, XMLRPC_Context *ctx);
Description
	Encode a BGB-RPC Vector.
	val is an array of pointers.
Status Internal
--*/
NetParse_Node *XmlRpc_EncodeVector(void *val, XMLRPC_Context *ctx)
{
	NetParse_Node *t, *t2, *lst;
	NET_Array *ns;
	int i;

	ns=(NET_Array *)val;

	lst=NULL;
	for(i=0; ns->values[i]; i++)
	{
		t2=XmlRpc_EncodeValue(ns->values[i], ctx);
		t=NetParse_NewNode();
		t->key=kstrdup("value");
		t->first=t2;
		lst=NetParse_AddNodeEnd(lst, t);
	}
	t2=NetParse_NewNode();
	t2->key=kstrdup("data");
	t2->first=lst;

	t=NetParse_NewNode();
	t->key=kstrdup("vector");
	t->first=t2;

	return(t);
}

NetParse_Node *xmlrpc_ext1_encode(XMLRPC_Context *ctx, void *val)
{
	NetParse_Node *t, *t2;
	char buf[16];
	byte *s, *s2;
	char *type;

	type=ObjType_GetTypeName(val);

	if(!strcmp(type, "char_t"))
	{
		sprintf(buf, "%d", *(int *)val);
		t2=NetParse_NewNode();
		t2->text=kstrdup(buf);

		t=NetParse_NewNode();
		t->key=kstrdup("char");
		t->first=t2;

		return(t);
	}

	if(!strcmp(type, "symbol_t"))
	{
		t2=NetParse_NewNode();
		t2->text=kstrdup((char *)val);

		t=NetParse_NewNode();
		t->key=kstrdup("symbol");
		t->first=t2;

		return(t);
	}

	if(!strcmp(type, "link_t"))
	{
		t2=NetParse_NewNode();
		t2->text=kstrdup((char *)val);

		t=NetParse_NewNode();
		t->key=kstrdup("link");
		t->first=t2;

		return(t);
	}

	if(!strcmp(type, "d_vector_t"))
	{
		return(XmlRpc_EncodeVector(val, ctx));
	}

	return(NULL);
}

void *xmlrpc_ext1_decode(XMLRPC_Context *ctx, NetParse_Node *exp)
{
	void *p;
	char *s;

	if(!strcmp(exp->key, "char"))
	{
		p=gctalloc("char_t", 0);
		*(int *)p=atoi(exp->first->text);
		return(p);
	}

	if(!strcmp(exp->key, "symbol"))
	{
		s=gctalloc("symbol_t", strlen(exp->first->text)+1);
		strcpy(s, exp->first->text);
		return(s);
	}
	if(!strcmp(exp->key, "link"))
	{
		s=gctalloc("link_t", strlen(exp->first->text)+1);
		strcpy(s, exp->first->text);
		return(s);
	}

	if(!strcmp(exp->key, "vector"))
	{
		p=XmlRpc_DecodeVector(exp, ctx);
		return(p);
	}

	return(NULL);
}

int XMLRPC_EXT1_Init()
{
	XMLRPC_Extension *ext;

	ext=XmlRpc_CreateExtension(
		"http://bgb-sys.sourceforge.net/bgb-ext/types");
	ext->encode=&xmlrpc_ext1_encode;
	ext->decode=&xmlrpc_ext1_decode;

	return(0);
}
