#include <bgbnet.h>

#if defined(WIN32)
#include <windows.h>
#include <winsock.h>
#else
#include <netinet/in.h>
#endif

/*--
Cat pdnet;Protocols;XMPP/Jabber
Text
	This is the XMPP (aka: Jabber) protocol.
--*/

typedef struct {
int open;
}XMPP_Info;

XMPP_Interface *xmpp_cl_ifroot;

Meta0_PortInfo *xmpp_cl_inf;
Meta0_Con *xmpp_cl_con;
NET_Interface *xmpp_cl_iface;

dyxNode *xmpp_cl_msgs;

char *xmpp_cl_user;
char *xmpp_cl_host;
char *xmpp_cl_passwd;
char *xmpp_cl_rsrc;

char *xmpp_auth_key=NULL;

int xmpp_cts=0, xmpp_auth=0, xmpp_con=0;
int xmpp_close=0;
int xmpp_seq=1;

/*--
Cat pdnet;Protocols;XMPP/Jabber
Form
	XMPP_Interface *XMPP_CreateInterface(char *xmlns);
Description
	Create an interface to the XMPP subsystem.
	If not null xmlns identifies the namespace used for incomming iq \
	requests.
--*/
XMPP_Interface *XMPP_CreateInterface(char *xmlns)
{
	XMPP_Interface *iface;

	iface=gcalloc(sizeof(XMPP_Interface));
	if(xmlns)
		iface->xmlns=dystrdup(xmlns);
		else iface->xmlns=NULL;

	iface->next=xmpp_cl_ifroot;
	xmpp_cl_ifroot=iface;

	return(iface);
}

/*--
Cat pdnet;Protocols;XMPP/Jabber
Form
	int XMPP_DispatchMessage(Meta0_Con *con, dyxNode *node);
Description
	Dispatch a message to all interfaces that can recieve messages.
Status Internal
--*/
int XMPP_DispatchMessage(Meta0_Con *con, dyxNode *node)
{
	XMPP_Interface *cur;

	cur=xmpp_cl_ifroot;
	while(cur)
	{
		if(cur->message)cur->message(con, node);
		cur=cur->next;
	}
	return(0);
}

/*--
Cat pdnet;Protocols;XMPP/Jabber
Form
	int XMPP_DispatchPresence(Meta0_Con *con, dyxNode *node);
Description
	Dispatch a presence stanza to all interfaces that can recieve them.
Status Internal
--*/
int XMPP_DispatchPresence(Meta0_Con *con, dyxNode *node)
{
	XMPP_Interface *cur;

	cur=xmpp_cl_ifroot;
	while(cur)
	{
		if(cur->presence)cur->presence(con, node);
		cur=cur->next;
	}
	return(0);
}

/*--
Cat pdnet;Protocols;XMPP/Jabber
Form
	int XMPP_DispatchTextMessage(char *from, char *subj, char *body);
Description
	Dispatch a text message to all interfaces that can recieve \
	text messages.
Status Internal
--*/
int XMPP_DispatchTextMessage(char *from, char *subj, char *body)
{
	XMPP_Interface *cur;

	cur=xmpp_cl_ifroot;
	while(cur)
	{
		if(cur->text_message)
			cur->text_message(from, subj, body);
		cur=cur->next;
	}
	return(0);
}

/*--
Cat pdnet;Protocols;XMPP/Jabber
Form
	int XMPP_DispatchIQ(Meta0_Con *con, dyxNode *node);
Description
	Dispatch an incomming IQ request to all interfaces with a matching namespace.
Status Internal
--*/
int XMPP_DispatchIQ(Meta0_Con *con, dyxNode *node)
{
	XMPP_Interface *cur;
	dyxNode *n;
	char *xmlns;

	n=dyxChild(node);
	xmlns=dyxGet(n, "xmlns");

	cur=xmpp_cl_ifroot;
	while(cur)
	{
		if(cur->xmlns)if(!strcmp(cur->xmlns, xmlns))
			if(cur->iq)cur->iq(con, node);
		cur=cur->next;
	}
	return(0);
}

int xmpp_handle_auth(Meta0_Con *con, dyxNode *node)
{
	dyxNode *t, *t2;
	dyxNode *c, *l;

	char *buf, *s;

	gc_printf("XMPP: Authenticate\n");

	c=dyxChild(node);
	l=NULL;

	while(c)
	{
		if(!strcmp(dyxTag(c), "username"))
		{
			gc_printf("AUTH: User\n");
			t2=dyxNewText(xmpp_cl_user);
			t=dyxNew1("username", t2);
			l=dyxAddEnd(l, t);
		}
		if(!strcmp(dyxTag(c), "password"))
		{
			gc_printf("AUTH: Password\n");
			t2=dyxNewText(xmpp_cl_passwd);
			t=dyxNew1("password", t2);
			l=dyxAddEnd(l, t);
		}
		if(!strcmp(dyxTag(c), "resource"))
		{
			gc_printf("AUTH: Resource\n");
			t2=dyxNewText(xmpp_cl_rsrc);
			t=dyxNew1("resource", t2);
			l=dyxAddEnd(l, t);
		}
		c=c->next;
	}

	t2=dyxNew1("query", l);
	dyxSet(t2, "xmlns", "jabber:iq:auth");
	t=dyxNew1("iq", t2);

	buf=gcralloc(16);
	gcrsprints(buf, "k%d", xmpp_seq++);
	xmpp_auth_key=dystrdup(buf);

	dyxSet(t, "type", "set");
	dyxSet(t, "id", xmpp_auth_key);

	buf=gcralloc(1024);
	s=buf;
	s=dyxPrintStr(s, t);
	s=gcrsprints(s, "\n");

	gc_printf("SEND: %s\n", buf);
	vfprint(con->sock, "%s", buf);

	return(0);
}

int xmpp_handle_message(Meta0_Con *con, dyxNode *node)
{
	dyxNode *c;

	char *to, *from;
	char *subj;
	char *body;

	char *buf, *s;

	XMPP_DispatchMessage(con, node);

	to=dyxGet(node, "to");
	from=dyxGet(node, "from");

	subj=NULL;
	body=NULL;

	c=dyxChild(node);
	while(c)
	{
		if(!strcmp(dyxTag(c), "subject"))
			subj=dyxText(dyxChild(c));
		if(!strcmp(dyxTag(c), "body"))
			body=dyxText(dyxChild(c));
		c=c->next;
	}
//	gc_printf("RECV MESSAGE: from: '%s' subj: '%s' body: '%s'\n",
//		from, subj, body);

	if(body)XMPP_DispatchTextMessage(from, subj, body);

//	buf=gcralloc(strlen(subj)+256);
//	s=gcrsprints(buf, "BOUNCE: %s", subj);

//	XMPP_Message(from, to, buf, body);

	return(0);
}

int xmpp_handle_form(Meta0_Con *con, dyxNode *node)
{
	dyxNode *n;

//	gc_printf("XMPP: Handle Form\n");

	if(!dyxTag(node))return(0);

	if(!strcmp(dyxTag(node), "iq"))
	{
		if(xmpp_auth_key)
		{
			if(dyxAttrIsP(node, "type", "result") &&
				dyxAttrIsP(node, "id", xmpp_auth_key))
			{
				gcfree(xmpp_auth_key);
				xmpp_auth_key=NULL;

				xmpp_auth=1;
				gc_printf("XMPP: AUTH: OK\n");
				return(0);
			}

			if(dyxAttrIsP(node, "type", "error") &&
				dyxAttrIsP(node, "id", xmpp_auth_key))
			{
				gcfree(xmpp_auth_key);
				xmpp_auth_key=NULL;

				xmpp_auth=0;
				xmpp_close=1;
				gc_printf("XMPP: AUTH: Rejected\n");

				return(0);
			}
		}

		n=dyxChild(node);
		if(!n)return(0);

		XMPP_DispatchIQ(con, node);

		if(!strcmp(dyxTag(n), "query") &&
			dyxAttrIsP(n, "xmlns", "jabber:iq:auth"))
		{
			xmpp_handle_auth(con, n);
		}
	}

	if(!strcmp(dyxTag(node), "message"))
	{
		xmpp_handle_message(con, node);
		return(0);
	}

	if(!strcmp(dyxTag(node), "presence"))
	{
		XMPP_DispatchPresence(con, node);
		return(0);
	}

	return(0);
}

int xmpp_poll()
{
	dyxNode *cur;

	while(xmpp_cl_msgs)
	{
		cur=xmpp_cl_msgs;
		xmpp_cl_msgs=cur->next;

		xmpp_handle_form(xmpp_cl_con, cur);
		dyxFree(cur);
	}
	return(0);
}

int xmpp_input(Meta0_Con *con)
{
	XMPP_Info *inf;
	dyxNode *n, *nc;

	char buf[256], *s, *t, *t2;
	char *s2;
	int i;

	s=con->buf;
	t=s;

	inf=con->data;
	gc_printf("XMPP INPUT: [[ %s ]]\n", s);

	while(1)
	{
		if(!inf->open)
		{
			t2=t;
			n=dyxParseTagSBuf(&t2);
			if(t2==t)break;

			if(dyxTag(n))if(!strcmp(dyxTag(n), "?xml"))
				n=dyxParseTagSBuf(&t2);

			if(dyxTag(n))if(!strcmp(dyxTag(n), "stream"))
			{
				gc_printf("XMPP: Con Open\n");

				xmpp_cts=1;

				t=t2;
				inf->open=1;
				continue;
			}
			continue;
		}
//		gc_printf("XMPP: Con Open\n");

		t=dyxParseEatWhiteStr(t);
		if(!*t)break;

		if(((t[0]=='<') && (t[1]=='/')) || xmpp_close)
			break;

		t2=t;
		n=dyxParseExprSBuf(&t2);
		if(t2==t)break;
		t=t2;

//		xmpp_handle_form(con, n);
//		dyxFree(n);

		if(xmpp_cl_msgs)
		{
			nc=xmpp_cl_msgs;
			while(nc->next)nc=nc->next;
			nc->next=n;
		}else xmpp_cl_msgs=n;

		if(((t[0]=='<') && (t[1]=='/')) || xmpp_close)
			break;
	}

	if(((t[0]=='<') && (t[1]=='/')) || xmpp_close)
	{
		xmpp_cts=0;
		xmpp_con=0;

		gc_printf("XMPP: Close Detected\n");
		s2="</stream:stream>";
		gc_printf("SEND: %s\n", s2);
		vfprint(con->sock, "%s", s2);

		xmpp_cl_con=NULL;

		return(-1);
	}

	gc_printf("XMPP: loop break\n");

	return(t-s);
}

int xmpp_closed(Meta0_Con *con)
{
	xmpp_cts=0;
	xmpp_con=0;

	return(0);
}

/*--
Cat pdnet;Protocols;XMPP/Jabber
Form
	VADDR *XMPP_GetHostAddr(char *host, int defport);
Description
	Performs a hostname lookup.
	It also fills in a defualt port value if none is present.
--*/
VADDR *XMPP_GetHostAddr(char *host, int defport)
{
	VADDR *addr;

	addr=NET_DecodeHostname(host);
	if(!addr)
	{
		gc_printf("XMPP: Unknown host %s\n", host);
//		gcfree(ri);
//		gcfree(ref);
		return(NULL);
	}
	switch(addr->proto)
	{
	case PROTO_IPV4:
	case PROTO_IPV4UDP:
	case PROTO_IPV4TCP:
		if(!addr->ipv4.port)
			addr->ipv4.port=htons(defport);
		break;
	case PROTO_IPV6:
	case PROTO_IPV6UDP:
	case PROTO_IPV6TCP:
		if(!addr->ipv6.port)
			addr->ipv6.port=htons(defport);
		break;
	default:
		break;
	}

	return(addr);
}

/*--
Cat pdnet;Protocols;XMPP/Jabber
Form
	int XMPP_Connect(char *server);
Description
	Connect to a specified XMPP server.
--*/
int XMPP_Connect(char *server)
{
	Meta0_Con *con;
	XMPP_Info *inf;
	VADDR *addr;
	char *s;
	char *buf;

	XMPP_Init();

	if(xmpp_cl_con)
	{
		gc_printf("XMPP: Allready connected.\n");
		return(-1);
	}

	gc_printf("XMPP: Connecting to server '%s'\n", server);

	xmpp_cl_host=dystrdup(server);

	addr=XMPP_GetHostAddr(server, 5222);

	inf=gcalloc(sizeof(XMPP_Info));
	inf->open=0;

	con=Meta0_Connect(xmpp_cl_inf, addr);

	if(!con)
	{
		gc_printf("XMPP: Connection failed.\n");
		return(-1);
	}

	con->data=inf;
	xmpp_cl_con=con;

//	s="<stream:stream "
//		"to=\"localhost\" "
//		"xmlns=\"jabber:client\" "
//		"xmlns:stream=\"http://etherx.jabber.org/streams\" "
//		"version=\"1.0\">";

	buf=gcralloc(256);
	s=buf;
	s=gcrsprints(s, "<stream:stream ");
	s=gcrsprints(s, "to='%s' ", server);
	s=gcrsprints(s, "xmlns='jabber:client' ");
	s=gcrsprints(s, "xmlns:stream='http://etherx.jabber.org/streams' ");
	s=gcrsprints(s, "version='1.0'>");

	gc_printf("SEND: %s\n", buf);
	vfprint(con->sock, "%s", buf);

	xmpp_con=1;
	while(!xmpp_cts && xmpp_con)
	{
		NET_Poll();
		thSleep(1);
	}
	if(!xmpp_con)
	{
		gc_printf("XMPP_Connect: Init stream failed\n");
		return(-1);
	}

	return(0);
}

/*--
Cat pdnet;Protocols;XMPP/Jabber
Form
	int XMPP_Disconnect();
Description
	Disconnect from XMPP server.
--*/
int XMPP_Disconnect()
{
	char *s;

	if(!xmpp_con)return(-1);
	if(!xmpp_cl_con)return(-1);

	xmpp_cts=0;
	xmpp_con=0;

	gc_printf("XMPP: Disconnect\n");

	s="</stream:stream>";
	gc_printf("SEND: %s\n", s);
	vfprint(xmpp_cl_con->sock, "%s", s);
	Meta0_CloseConnection(xmpp_cl_con);
	xmpp_cl_con=NULL;

	return(0);
}

/*--
Cat pdnet;Protocols;XMPP/Jabber
Form
	int XMPP_Login(char *user, char *passwd, char *rsrc);
Description
	Log into the server.
--*/
int XMPP_Login(char *user, char *passwd, char *rsrc)
{
	char *s, *t;

	while(!xmpp_cts && xmpp_con)
	{
		NET_Poll();
		thSleep(1);
	}
	if(!xmpp_con)
	{
		gc_printf("XMPP_Login: Connection failed\n");
		return(-1);
	}

	xmpp_cl_user=user;
	xmpp_cl_passwd=passwd;
	xmpp_cl_rsrc=rsrc;

	gc_printf("XMPP: Logging in as '%s@%s/%s'\n", user, xmpp_cl_host, rsrc);

	s=gcralloc(256);
	t=s;

	t=gcrsprints(t, "<iq id='k%d' type='get'>", xmpp_seq++);
	t=gcrsprints(t, "<query xmlns='jabber:iq:auth'>");
	t=gcrsprints(t, "<username>%s</username>", user);
	t=gcrsprints(t, "</query></iq>\n");

//	s=
//		"<iq id='jcl_41' type='get'>"
//		"<query xmlns='jabber:iq:auth'>"
//		"<username>cr88192</username>"
//		"</query></iq>";

	gc_printf("SEND: %s\n", s);
	vfprint(xmpp_cl_con->sock, "%s", s);

	while(!xmpp_auth && xmpp_con)
	{
		NET_Poll();
		thSleep(1);
	}
	if(!xmpp_con)
	{
		gc_printf("XMPP_Login: Login failed\n");
		return(-1);
	}

	return(0);
}

/*--
Cat pdnet;Protocols;XMPP/Jabber
Form
	int XMPP_Message(char *to, char *from, char *subj, char *body);
Description
	Sends a message with the from specified.
--*/
int XMPP_Message(char *to, char *from, char *subj, char *body)
{
	char *s, *buf;

	while(!xmpp_cts)
	{
		NET_Poll();
	}

//	s="<message id='' "
//		"to='cr88192@localhost/Exodus' "
//		"from='null@localhost/pdlib'>"
//		"<subject>test</subject>"
//		"<body>a test</body>"
//		"</message>";
//	s=
//		"<iq id='jcl_41' type='get'>"
//		"<query xmlns='jabber:iq:auth'>"
//		"<username>cr88192</username>"
//		"</query></iq>";

	buf=gcralloc(1024);
	s=buf;

	s=gcrsprints(s, "<message id='' to='%s' from='%s'>", to, from);
	s=gcrsprints(s, "<subject>%s</subject>", subj);
	s=gcrsprints(s, "<body>%s</body>", body);
	s=gcrsprints(s, "</message>");

	gc_printf("SEND: %s\n", buf);
	vfprint(xmpp_cl_con->sock, "%s", buf);

	return(0);
}

/*--
Cat pdnet;Protocols;XMPP/Jabber
Form
	int XMPP_SendIQ(char *to, char *type, char *tag, dyxNode *body);
Description
	Sends an IQ request to 'to'.
--*/
int XMPP_SendIQ(char *to, char *type, char *tag, dyxNode *body)
{
	dyxNode *t, *t2;
	char *s;
	char *buf;
	char *from;

	while(!xmpp_auth)
	{
		NET_Poll();
	}

	buf=gcralloc(64);
	gcrsprints(buf, "%s@%s/%s", xmpp_cl_user, xmpp_cl_host, xmpp_cl_rsrc);
	from=dystrdup(buf);

	t2=body;

	t=dyxNew1("iq", t2);
	dyxSet(t, "type", type);
	dyxSet(t, "id", tag);
	dyxSet(t, "to", to);
	dyxSet(t, "from", from);

//	dyxSTE0_PrintCompare(t);

	buf=gcralloc(1024);
	s=buf;
	s=dyxPrintStr(s, t);
	s=gcrsprints(s, "\n");

	gc_printf("SEND: %s\n", buf);
	vfprint(xmpp_cl_con->sock, "%s", buf);

	return(0);
}

/*--
Cat pdnet;Protocols;XMPP/Jabber
Form
	int XMPP_SendMessage(char *to, char *subj, char *body);
Description
	Sends a message to 'to'.
	This time 'from' is generated from locally known info.
--*/
int XMPP_SendMessage(char *to, char *subj, char *body)
{
	char *s, *buf;
	char *from;

	while(!xmpp_cts)
	{
		NET_Poll();
	}

	buf=gcralloc(64);
	gcrsprints(buf, "%s@%s/%s", xmpp_cl_user, xmpp_cl_host, xmpp_cl_rsrc);
	from=gcrstrdup(buf);

	buf=gcralloc(1024);
	s=buf;

	s=gcrsprints(s, "<message id='' to='%s' from='%s'>", to, from);
	if(subj)s=gcrsprints(s, "<subject>%s</subject>", subj);

//	s=gcrsprints(s, "<body>%s</body>", body);

	s=gcrsprints(s, "<body>");
	s=dyxPrintTextStr(s, body);
	s=gcrsprints(s, "</body>");

	s=gcrsprints(s, "</message>");

	gc_printf("SEND: %s\n", buf);
	vfprint(xmpp_cl_con->sock, "%s", buf);

	return(0);
}

/*--
Cat pdnet;Protocols;XMPP/Jabber
Form
	int XMPP_SendMessageChat(char *to, char *body);
Description
	Sends a chat message to 'to'.
	This time 'from' is generated from locally known info.
--*/
int XMPP_SendMessageChat(char *to, char *body)
{
	char *s, *buf;
	char *from;

	while(!xmpp_cts)
	{
		NET_Poll();
	}

	buf=gcralloc(64);
	gcrsprints(buf, "%s@%s/%s", xmpp_cl_user, xmpp_cl_host, xmpp_cl_rsrc);
	from=gcrstrdup(buf);

	buf=gcralloc(1024);
	s=buf;

	s=gcrsprints(s, "<message to='%s' from='%s' type='chat'>", to, from);

	s=gcrsprints(s, "<body>");
	s=dyxPrintTextStr(s, body);
	s=gcrsprints(s, "</body>");

	s=gcrsprints(s, "</message>");

	gc_printf("SEND: %s\n", buf);
	vfprint(xmpp_cl_con->sock, "%s", buf);

	return(0);
}

/*--
Cat pdnet;Protocols;XMPP/Jabber
Form
	int XMPP_SendStanza(dyxNode *body);
Description
	Sends a raw stanza.
--*/
int XMPP_SendStanza(dyxNode *body)
{
	dyxNode *nc;
	char *s, *to, *from;
	char *buf;

	//HACK: bounce messages sent to self
	from=dyxGet(body, "to");
	if(!from)from=XMPP_GenLocal();
	to=dyxGet(body, "to");
	if(!strcmp(to, from) || !strcmp(to, "local"))
	{
		buf=gcralloc(1024);
		s=buf;
		s=dyxPrintStr(s, body);
		s=gcrsprints(s, "\n");

		gc_printf("BOUNCE: %s\n", buf);

		if(xmpp_cl_msgs)
		{
			nc=xmpp_cl_msgs;
			while(nc->next)nc=nc->next;
			nc->next=body;
		}else xmpp_cl_msgs=body;
		return(0);
	}

	while(!xmpp_auth)
	{
		NET_Poll();
	}

	buf=gcralloc(1024);
	s=buf;
	s=dyxPrintStr(s, body);
	s=gcrsprints(s, "\n");

	gc_printf("SEND: %s\n", buf);
	vfprint(xmpp_cl_con->sock, "%s", buf);

	return(0);
}

/*--
Cat pdnet;Protocols;XMPP/Jabber
Form
	int XMPP_SendPresence(char *to, char *type, dyxNode *body);
Description
	Sends a presence stanza to 'to'.
--*/
int XMPP_SendPresence(char *to, char *type, dyxNode *body)
{
	dyxNode *t, *t2;
	char *from;

	t2=body;

	t=dyxNew1("presence", t2);
	from=XMPP_GenLocal();

	if(to)dyxSet(t, "to", to);
	if(from)dyxSet(t, "from", from);
	if(type)dyxSet(t, "type", type);

	XMPP_SendStanza(t);
	return(0);
}

/*--
Cat pdnet;Protocols;XMPP/Jabber
Form
	int XMPP_SendMessageStanza(char *to, char *type, dyxNode *body);
Description
	Sends a presence stanza to 'to'.
--*/
int XMPP_SendMessageStanza(char *to, char *type, dyxNode *body)
{
	dyxNode *t, *t2;
	char *from;

	t2=body;

	t=dyxNew1("message", t2);
	from=XMPP_GenLocal();

	if(to)dyxSet(t, "to", to);
	if(from)dyxSet(t, "from", from);
	if(type)dyxSet(t, "type", type);

	XMPP_SendStanza(t);
	return(0);
}

/*--
Cat pdnet;Protocols;XMPP/Jabber
Form
	int XMPP_Init();
Description
	Initializes XMPP.
--*/
int XMPP_Init()
{
	static int init;

	if(init)return(0);
	init=1;

	gc_printf("XMPP Init\n");
	xmpp_cl_ifroot=NULL;
	xmpp_cl_msgs=NULL;

	xmpp_cl_inf=Meta0_CreateClientPort("xmpp-client");
	xmpp_cl_inf->input=&xmpp_input;
	xmpp_cl_inf->closed=&xmpp_closed;

	xmpp_cl_iface=NET_CreateInterface(NULL);
	xmpp_cl_iface->poll=&xmpp_poll;

	XMPP_JEP30_Init();
	XMPP_MucRoom_Init();
//	XMPP_TMP_Init();

	return(0);
}
