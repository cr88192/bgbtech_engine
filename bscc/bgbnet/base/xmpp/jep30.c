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

	gc_printf("xmpp_jep30_items_iq\n");

	tag=dyxGet(node, "id");
	from=dyxGet(node, "from");

	if(dyxAttrIsP(node, "type", "get"))
	{
		n=NULL;

		n2=n;
		n=dyxNew1("query", n2);
		dyxSet(n, "xmlns",
			"http://jabber.org/protocol/disco#items");

		XMPP_SendIQ(from, "result", tag, n);
		return(0);
	}

	if(dyxAttrIsP(node, "type", "result") ||
		dyxAttrIsP(node, "type", "error"))
	{
		n=dyxClone(node);
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

	n=dyxNew("query");
	dyxSet(n, "xmlns",
		"http://jabber.org/protocol/disco#items");
	if(node)dyxSet(n, "node", node);

	gcrsprints(buf, "jep30-%d", jep30_seq++);
	tag=buf;

	XMPP_SendIQ(to, "get", tag, n);

	while(1)
	{
		n=XMPP_CheckResults(tag);
		if(n)break;
		NET_Poll();
	}

	return(dyxChild(dyxChild(n)));
}

int xmpp_jep30_info_iq(Meta0_Con *con, dyxNode *node)
{
	char *s, *t;
	char *from, *tag;
	XMPP_Interface *cur;

	dyxNode *n, *n2;

	tag=dyxGet(node, "id");
	from=dyxGet(node, "from");

	if(dyxAttrIsP(node, "type", "get"))
	{
		n=dyxNew("identity");
		dyxSet(n, "catagory", "client");
		dyxSet(n, "type", "pc");
		dyxSet(n, "name", "<pdlib>");

		cur=xmpp_cl_ifroot;
		while(cur)
		{
			if(cur->xmlns)
			{
				n2=n;
				n=dyxNew1("feature", n2);
				dyxSet(n, "var", cur->xmlns);
			}
			cur=cur->next;
		}

		n2=n;
		n=dyxNew1("query", n2);
		dyxSet(n, "xmlns",
			"http://jabber.org/protocol/disco#info");

		XMPP_SendIQ(from, "result", tag, n);
		return(0);
	}

	if(dyxAttrIsP(node, "type", "result") ||
		dyxAttrIsP(node, "type", "error"))
	{
		n=dyxClone(node);
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

	n=dyxNew("query");
	dyxSet(n, "xmlns",
		"http://jabber.org/protocol/disco#info");
	if(node)dyxSet(n, "node", node);

	gcrsprints(buf, "jep30-%d", jep30_seq++);
	tag=buf;

	XMPP_SendIQ(to, "get", tag, n);

	while(1)
	{
		n=XMPP_CheckResults(tag);
		if(n)break;
		NET_Poll();
	}

	return(dyxChild(dyxChild(n)));
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
