/*--
Cat pdnet;Protocols;XMPP/Jabber;JEP-0030
Text
	I will try to implement JEP-0030 (Service Discovery).
--*/

#include <bgbnet.h>

int jep30_seq=1;
XMPP_Interface *xmpp_cl_ifroot;

int xmpp_jep30_items_iq(Meta0_Con *con, dyxNode *node)
{
	char *s, *t;
	char *from, *tag;

	dyxNode *cur, *n, *n2;

	kprint("xmpp_jep30_items_iq\n");

	tag=dyxGetNodeAttr(node, "id");
	from=dyxGetNodeAttr(node, "from");

	if(dyxGetNodeAttrIsP(node, "type", "get"))
	{
		n=NULL;

		n2=n;
		n=dyxNewNode();
		n->key=kstrdup("query");
		n->first=n2;
		dyxAddAttr(n, "xmlns",
			"http://jabber.org/protocol/disco#items");

		XMPP_SendIQ(from, "result", tag, n);
		return(0);
	}

	if(dyxGetNodeAttrIsP(node, "type", "result") ||
		dyxGetNodeAttrIsP(node, "type", "error"))
	{
		n=dyxCopyNode(node);
		XMPP_AddResults(n);
	}
	return(0);
}

/*--
Cat pdnet;Protocols;XMPP/Jabber;JEP-0030
Form
	dyxNode *XMPP_JEP30_QueryItems(char *to, char *node);
Description
	Queries items from a given jid and node.
--*/
dyxNode *XMPP_JEP30_QueryItems(char *to, char *node)
{
	char buf[16];
	dyxNode *n;
	char *tag;

	n=dyxNewNode();
	n->key=kstrdup("query");
	dyxAddAttr(n, "xmlns",
		"http://jabber.org/protocol/disco#items");
	if(node)dyxAddAttr(n, "node", node);

	kprints(buf, "jep30-%d", jep30_seq++);
	tag=buf;

	XMPP_SendIQ(to, "get", tag, n);

	while(1)
	{
		n=XMPP_CheckResults(tag);
		if(n)break;
		NET_Poll();
	}

	return(n->first->first);
}

int xmpp_jep30_info_iq(Meta0_Con *con, dyxNode *node)
{
	char *s, *t;
	char *from, *tag;
	XMPP_Interface *cur;

	dyxNode *n, *n2;

	tag=dyxGetNodeAttr(node, "id");
	from=dyxGetNodeAttr(node, "from");

	if(dyxGetNodeAttrIsP(node, "type", "get"))
	{
		n=dyxNewNode();
		n->key=kstrdup("identity");
		dyxAddAttr(n, "catagory", "client");
		dyxAddAttr(n, "type", "pc");
		dyxAddAttr(n, "name", "<pdlib>");

		cur=xmpp_cl_ifroot;
		while(cur)
		{
			if(cur->xmlns)
			{
				n2=n;
				n=dyxNewNode();
				n->key=kstrdup("feature");
				n->next=n2;
				dyxAddAttr(n, "var", cur->xmlns);
			}
			cur=cur->next;
		}

		n2=n;
		n=dyxNewNode();
		n->key=kstrdup("query");
		n->first=n2;
		dyxAddAttr(n, "xmlns",
			"http://jabber.org/protocol/disco#info");

		XMPP_SendIQ(from, "result", tag, n);
		return(0);
	}

	if(dyxGetNodeAttrIsP(node, "type", "result") ||
		dyxGetNodeAttrIsP(node, "type", "error"))
	{
		n=dyxCopyNode(node);
		XMPP_AddResults(n);
	}

	return(0);
}

/*--
Cat pdnet;Protocols;XMPP/Jabber;JEP-0030
Form
	dyxNode *XMPP_JEP30_QueryInfo(char *to, char *node);
Description
	Queries info from a given jid and node.
--*/
dyxNode *XMPP_JEP30_QueryInfo(char *to, char *node)
{
	char buf[16];
	dyxNode *n;
	char *tag;

	n=dyxNewNode();
	n->key=kstrdup("query");
	dyxAddAttr(n, "xmlns",
		"http://jabber.org/protocol/disco#info");
	if(node)dyxAddAttr(n, "node", node);

	kprints(buf, "jep30-%d", jep30_seq++);
	tag=buf;

	XMPP_SendIQ(to, "get", tag, n);

	while(1)
	{
		n=XMPP_CheckResults(tag);
		if(n)break;
		NET_Poll();
	}

	return(n->first->first);
}

/*--
Cat pdnet;Protocols;XMPP/Jabber;JEP-0030
Form
	int XMPP_JEP30_Init();
Description
	Init function for JEP-0030.
Status Internal
--*/
int XMPP_JEP30_Init()
{
	XMPP_Interface *xmpp_if;

	xmpp_if=XMPP_CreateInterface("http://jabber.org/protocol/disco#items");
	xmpp_if->iq=&xmpp_jep30_items_iq;

	xmpp_if=XMPP_CreateInterface("http://jabber.org/protocol/disco#info");
	xmpp_if->iq=&xmpp_jep30_info_iq;

	return(0);
}
