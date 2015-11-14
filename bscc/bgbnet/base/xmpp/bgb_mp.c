/*--
Cat pdnet;Protocols;XMPP/Jabber;TMP
Text
	XMPP: Tupple Message Passing

	allows "exporting" things, which allows incomming messages to be \
	directed to them. an incomming message will be handled via a callback \
	associated with that export.

	imports will also be possible.
--*/

#include <bgbnet.h>

#define XMPP_TMP_XMLNS "http://bgb-sys.sourceforge.net/xmlns/xmpp_tmp"

static XMPP_PoolExport *xmpp_tmp_exports;
static XMPP_Interface *xmpp_tmp_if;

/*--
Cat pdnet;Protocols;XMPP/Jabber;TMP
Form
	int XMPP_TMP_Export(char *name, void *data, \
	int (*func)(char *name, void *data, void **msg));
Description
	Exports a given pool (data), with an associated callback for \
	handling messages.
--*/
int XMPP_TMP_Export(char *name, void *data,
	int (*func)(char *name, void *data, void **msg))
{
	XMPP_PoolExport *tmp;

	tmp=gcalloc(sizeof(XMPP_PoolExport));

	tmp->name=dystrdup(name);
	tmp->data=data;
	tmp->func=func;

	tmp->next=xmpp_tmp_exports;
	xmpp_tmp_exports=tmp;

	return(0);
}

/*--
Cat pdnet;Protocols;XMPP/Jabber;TMP
Form
	XMPP_PoolReference *XMPP_TMP_NewPoolRef(char *jid, char *name);
Description
	Construct a pool reference from a jid and name.
--*/
XMPP_PoolReference *XMPP_TMP_NewPoolRef(char *jid, char *name)
{
	XMPP_PoolReference *ref;
	ref=gctalloc("xmpp_poolref_t",
		sizeof(XMPP_PoolReference));
	ref->jid=dystrdup(jid);
	ref->name=dystrdup(name);

	return(ref);
}

/*--
Cat pdnet;Protocols;XMPP/Jabber;TMP
Form
	XMPP_PoolReference *XMPP_TMP_NewLocalPoolRef(char *name);
Description
	Construct a pool reference to a pool on the current node.
--*/
XMPP_PoolReference *XMPP_TMP_NewLocalPoolRef(char *name)
{
	XMPP_PoolReference *ref;
	ref=gctalloc("xmpp_poolref_t",
		sizeof(XMPP_PoolReference));
	ref->jid=dystrdup("local");
	ref->name=dystrdup(name);

	return(ref);
}

/*--
Cat pdnet;Protocols;XMPP/Jabber;TMP
Form
	void *XMPP_TMP_DecodeValue(dyxNode *node);
Description
	Decodes a value.
Status Internal
--*/
void *XMPP_TMP_DecodeValue(dyxNode *node)
{
	XMPP_PoolReference *ref;
	char *jid, *name, *value;
	dyxNode *cur;
	void **a;
	int i;

	if(!strcmp(node->key, "pool"))
	{
		jid=dyxGet(node, "jid");
		name=dyxGet(node, "name");

		if(!strcmp(jid, XMPP_GenLocal()))
			jid="local";

		ref=gctalloc("xmpp_poolref_t",
			sizeof(XMPP_PoolReference));
		if(jid)ref->jid=dystrdup(jid);
		if(name)ref->name=dystrdup(name);

		return(ref);
	}
	if(!strcmp(node->key, "array"))
	{
		a=gcralloc(64*sizeof(void *));
		i=0;

		cur=node->first;
		while(cur)
		{
			a[i++]=XMPP_TMP_DecodeValue(cur);
			cur=cur->next;
		}
		a[i++]=NULL;

		return(NetVal_WrapDArray(a));
	}

	if(!strcmp(node->key, "number"))
	{
		value=dyxGet(node, "value");
		return(NetVal_WrapNumber(atof(value)));
	}
	if(!strcmp(node->key, "string"))
	{
		value=dyxGet(node, "value");
		return(NetVal_WrapString(value));
	}

	if(!strcmp(node->key, "null"))
		return(NetVal_MakeNull());
	if(!strcmp(node->key, "true"))
		return(NetVal_WrapBool(1));
	if(!strcmp(node->key, "false"))
		return(NetVal_WrapBool(0));

	return(NetVal_MakeNull());
}

static int xmpp_tmp_message(Meta0_Con *con, dyxNode *node)
{
	XMPP_PoolExport *cur;

	dyxNode *n, *n1;
	char *from, *pool, *s, *ns, *val;
	int i;
	void **a;

	from=dyxGet(node, "from");

	gc_printfd("XMPP_TMP: got message\n");

	ns=NULL;
	n=node->first;
	while(n)
	{
		if(!strcmp(n->key, "x"))
		{
			ns=dyxGet(n, "xmlns");
			if(ns)if(!strcmp(ns, XMPP_TMP_XMLNS))
				break;
		}
		n=n->next;
	}
	if(!n)return(0);

	n1=dyxFindKey(n->first, "msg");
	if(n1)
	{
		n=n1;
		pool=dyxGet(n, "pool");

		gc_printfd("XMPP_TMP: got message for %s\n", pool);

		cur=xmpp_tmp_exports;
		while(cur)
		{
			if(!strcmp(cur->name, pool))
				break;
			cur=cur->next;
		}
		if(!cur)return(0);

		a=gcalloc(64*sizeof(void *));
		i=0;

		n1=n->first;
		while(n1)
		{
			a[i++]=XMPP_TMP_DecodeValue(n1);
			n1=n1->next;
		}
		a[i++]=NULL;

		cur->func(cur->name, cur->data, a);
		gcfree(a);

		return(0);
	}
	return(0);
}

/*--
Cat pdnet;Protocols;XMPP/Jabber;TMP
Form
	dyxNode *XMPP_TMP_EncodeValue(void *obj);
Description
	Encodes a value.
Status Internal
--*/
dyxNode *XMPP_TMP_EncodeValue(void *obj)
{
	dyxNode *n, *n1;
	XMPP_PoolReference *ref;
	void **a;
	char *ty, *jid;
	char buf[256];
	int i;

	ty=ObjType_GetTypeName(obj);
	if(!strcmp(ty, "xmpp_poolref_t"))
	{
		ref=obj;
		jid=ref->jid;

		if(!strcmp(jid, "local"))
			jid=XMPP_GenLocal();

		n=dyxNewNode();
		n->key=dystrdup("pool");
		if(ref->jid)dyxSet(n, "jid", jid);
		if(ref->name)dyxSet(n, "name", ref->name);
		return(n);
	}

	if(!strcmp(ty, "d_array_t"))
	{
		n1=NULL;
		a=NetVal_UnwrapDArray(obj);
		i=0;
		while(a[i])
		{
			n1=dyxAddNodeEnd(n1,
				XMPP_TMP_EncodeValue(a[i++]));
		}

		n=dyxNewNode();
		n->key=dystrdup("array");
		n->first=n1;
		return(n);
	}

	if(!strcmp(ty, "int_t"))
	{
		gcrsprints(buf, "%d", NetVal_UnwrapInt(obj));
		n=dyxNewNode();
		n->key=dystrdup("number");
		dyxSet(n, "value", buf);
		return(n);
	}
	if(!strcmp(ty, "float_t"))
	{
		gcrsprints(buf, "%lf", NetVal_UnwrapFloat(obj));
		n=dyxNewNode();
		n->key=dystrdup("number");
		dyxSet(n, "value", buf);
		return(n);
	}
	if(!strcmp(ty, "string_t"))
	{
		n=dyxNewNode();
		n->key=dystrdup("string");
		dyxSet(n, "value", obj);
		return(n);
	}

	if(!strcmp(ty, "null_t"))
	{
		n=dyxNewNode();
		n->key=dystrdup("null");
		return(n);
	}
	if(!strcmp(ty, "bool_t"))
	{
		i=NetVal_UnwrapBool(obj);
		n=dyxNewNode();
		if(i)n->key=dystrdup("true");
			else n->key=dystrdup("false");
		return(n);
	}

	gc_printf("XMPP_TMP: failed encode type %s\n", ty);

	n=dyxNewNode();
	n->key=dystrdup("null");
	return(n);
}

/*--
Cat pdnet;Protocols;XMPP/Jabber;TMP
Form
	int XMPP_TMP_SendMessageTo(char *jid, char *pool, void **msg);
Description
	Sends a message to a given jid and pool.
--*/
int XMPP_TMP_SendMessageTo(char *jid, char *pool, void **msg)
{
	dyxNode *n, *n1;
	void **a;
	int i;

	n=NULL;
	i=0;
	while(msg[i])
	{
//		gc_printf("XMPP_TMP: arg %d\n", i);
		n1=XMPP_TMP_EncodeValue(msg[i]);
		i++;

		n=dyxAddNodeEnd(n, n1);
	}

	n1=n;
	n=dyxNewNode();
	n->key=dystrdup("msg");
	n->first=n1;
	dyxSet(n, "pool", pool);

	n1=n;
	n=dyxNewNode();
	n->key=dystrdup("x");
	n->first=n1;
	dyxSet(n, "xmlns", XMPP_TMP_XMLNS);

	XMPP_SendMessageStanza(jid, NULL, n);

	return(0);
}

/*--
Cat pdnet;Protocols;XMPP/Jabber;TMP
Form
	int XMPP_TMP_SendMessage(XMPP_PoolReference *ref, void **msg);
Description
	Sends a message to a given reference.
--*/
int XMPP_TMP_SendMessage(XMPP_PoolReference *ref, void **msg)
{
	XMPP_TMP_SendMessageTo(ref->jid, ref->name, msg);
	return(0);
}

/*--
Cat pdnet;Protocols;XMPP/Jabber;TMP
Form
	int XMPP_TMP_Init();
Description
	Init function.
Status Internal
--*/
int XMPP_TMP_Init()
{
	xmpp_tmp_if=XMPP_CreateInterface(NULL);
	xmpp_tmp_if->message=&xmpp_tmp_message;
//	xmpp_tmp_if->presence=&xmpp_tmp_presence;

	return(0);
}
