/*--
Cat pdnet;Protocols;XMPP/Jabber;JEP-0045(MultiUserChat)
Text
	JEP-0045 (Multi User Chat).

	struct XMPP_MucRoom_s {
	XMPP_MucRoom *next;	//next room in the list of room contexts

	char *room;		//room name
	char *host;		//host name
	char *nick;		//current nick within the room

	int num_users;		//number of users in the room
	char **users;		//info about the users
	void *data;		//data to be passed to callbacks

	//callbacks for specific stanzas
	int (*message)(XMPP_MucRoom *room, void *data, dyxNode *msg);
	int (*presence)(XMPP_MucRoom *room, void *data, dyxNode *msg);

	//callbacks for specific events
	int (*add_user)(XMPP_MucRoom *room, void *data, char *name);
	int (*remove_user)(XMPP_MucRoom *room, void *data, char *name);
	};
--*/

#include <bgbnet.h>

XMPP_MucRoom *xmpp_jep45_root=NULL;
int xmpp_jep45_seq=1;
XMPP_Interface *xmpp_jep45_if;

/*--
Cat pdnet;Protocols;XMPP/Jabber;JEP-0045(MultiUserChat)
Form
	XMPP_MucRoom *XMPP_MucRoom_NewRoom(char *room, char *host, char *nick);
Description
	Creates a new room context for a given room, host, and nick.
--*/
XMPP_MucRoom *XMPP_MucRoom_NewRoom(char *room, char *host, char *nick)
{
	XMPP_MucRoom *tmp;

	XMPP_MucRoom_Init();

	tmp=gcalloc(sizeof(XMPP_MucRoom));

	tmp->users=gcalloc(1024*sizeof(char *));

	tmp->room=dystrdup(room);
	tmp->host=dystrdup(host);
	tmp->nick=dystrdup(nick);

	tmp->next=xmpp_jep45_root;
	xmpp_jep45_root=tmp;

	return(tmp);
}

/*--
Cat pdnet;Protocols;XMPP/Jabber;JEP-0045(MultiUserChat)
Form
	XMPP_MucRoom *XMPP_MucRoom_FindJid(char *jid);
Description
	Find the room for which a given jid is associated (room@host/nick).
--*/
XMPP_MucRoom *XMPP_MucRoom_FindJid(char *jid)
{
	XMPP_MucRoom *cur;
	char room[64], host[64];
	char *s, *t;

	s=jid;
	t=room;
	while(*s && (*s!='@'))*t++=*s++;
	*t++=0;

	if(*s=='@')s++;
	t=host;
	while(*s && (*s!='/'))*t++=*s++;
	*t++=0;

	cur=xmpp_jep45_root;
	while(cur)
	{
		if(!strcmp(cur->room, room) && !strcmp(cur->host, host))
			return(cur);
		cur=cur->next;
	}
	return(NULL);
}

/*--
Cat pdnet;Protocols;XMPP/Jabber;JEP-0045(MultiUserChat)
Form
	int XMPP_MucRoom_SetPresence(XMPP_MucRoom *room, char *user, \
	char *status);
Description
	Sets the presence (in the room context) for a given user.
--*/
int XMPP_MucRoom_SetPresence(XMPP_MucRoom *room, char *user, char *status)
{
	int i;
	char *s;

	s=user;
	while(*s && (*s!='/'))s++;
	if(*s=='/')user=s+1;

	for(i=0; i<room->num_users; i++)
		if(room->users[i])if(!strcmp(room->users[i], user))
	{
		if(status)if(!strcmp(status, "unavailable"))
		{
			if(room->remove_user)
				room->remove_user(room, room->data, user);
			room->users[i]=NULL;
		}
		return(0);
	}

	if(status)if(!strcmp(status, "unavailable"))
		return(0);

	for(i=0; i<room->num_users; i++)if(!room->users[i])
	{
		if(room->add_user)
			room->add_user(room, room->data, user);
		room->users[i]=dystrdup(user);
		return(0);
	}

	if(room->add_user)room->add_user(room, room->data, user);

	i=room->num_users++;
	room->users[i]=dystrdup(user);
	return(0);
}

/*--
Cat pdnet;Protocols;XMPP/Jabber;JEP-0045(MultiUserChat)
Form
	XMPP_MucRoom *XMPP_MucRoom_Join(char *room, char *host, char *nick);
Description
	Joins a room.
--*/
XMPP_MucRoom *XMPP_MucRoom_Join(char *room, char *host, char *nick)
{
	XMPP_MucRoom *tmp;
	dyxNode *n, *n1;
	char buf[256];

	tmp=XMPP_MucRoom_NewRoom(room, host, nick);
	gcrsprints(buf, "%s@%s/%s", room, host, nick);

	gc_printf("join room '%s'\n", buf);

	n1=dyxNew("history");
	dyxSet(n1, "maxchars", "0");
	n=dyxNew1("x", n1);

	dyxSet(n, "xmlns", "http://jabber.org/protocol/muc");

	XMPP_SendPresence(buf, NULL, n);
	return(tmp);
}

/*--
Cat pdnet;Protocols;XMPP/Jabber;JEP-0045(MultiUserChat)
Form
	int XMPP_MucRoom_Leave(XMPP_MucRoom *room);
Description
	Leaves a room.
--*/
int XMPP_MucRoom_Leave(XMPP_MucRoom *room)
{
	char buf[256];

	gcrsprints(buf, "%s@%s/%s", room->room, room->host, room->nick);
	XMPP_SendPresence(buf, "unavailable", NULL);
	return(0);
}

/*--
Cat pdnet;Protocols;XMPP/Jabber;JEP-0045(MultiUserChat)
Form
	char *XMPP_MucRoom_UserJid(XMPP_MucRoom *room, char *nick);
Description
	Generates a jid for a given nick within the room.
--*/
char *XMPP_MucRoom_UserJid(XMPP_MucRoom *room, char *nick)
{
	char buf[256];

	gcrsprints(buf, "%s@%s/%s", room->room, room->host, nick);
	return(gcrstrdup(buf));
}

/*--
Cat pdnet;Protocols;XMPP/Jabber;JEP-0045(MultiUserChat)
Form
	int XMPP_MucRoom_SendMessage(XMPP_MucRoom *room, dyxNode *body);
Description
	Sends a message to the room.
	Body is the contents of the message stanza.
--*/
int XMPP_MucRoom_SendMessage(XMPP_MucRoom *room, dyxNode *body)
{
	char buf[256];

	gcrsprints(buf, "%s@%s", room->room, room->host);
	XMPP_SendMessageStanza(buf, "groupchat", body);
	return(0);
}

/*--
Cat pdnet;Protocols;XMPP/Jabber;JEP-0045(MultiUserChat)
Form
	int XMPP_MucRoom_SendMessageText(XMPP_MucRoom *room, char *str);
Description
	Sends a textual message to a room.
--*/
int XMPP_MucRoom_SendMessageText(XMPP_MucRoom *room, char *str)
{
	dyxNode *n, *n2;

	n2=dyxNewText(str);
	n=dyxNew1("body", n2);
	XMPP_MucRoom_SendMessage(room, n);

	return(0);
}

int xmpp_jep45_message(Meta0_Con *con, dyxNode *node)
{
	XMPP_MucRoom *room;
	dyxNode *body;
	char *from, *type;

	from=dyxGet(node, "from");
	type=dyxGet(node, "type");

	if(!from)return(0);

//	if(!type)return(0);
//	if(strcmp(type, "groupchat"))return(0);

	room=XMPP_MucRoom_FindJid(from);
	if(!room)return(0);

	if(room->message)room->message(room, room->data, node);

//	body=dyxFindKey(node->first, "body");
	return(0);
}

int xmpp_jep45_presence(Meta0_Con *con, dyxNode *node)
{
	XMPP_MucRoom *room;
	dyxNode *body;
	char *from, *type;

	from=dyxGet(node, "from");
	type=dyxGet(node, "type");

	room=XMPP_MucRoom_FindJid(from);
	if(!room)return(0);

	XMPP_MucRoom_SetPresence(room, from, type);

	if(room->presence)room->presence(room, room->data, node);

//	body=dyxFindKey(node->first, "body");
	return(0);
}

/*--
Cat pdnet;Protocols;XMPP/Jabber;JEP-0045(MultiUserChat)
Form
	int XMPP_MucRoom_Init();
Description
	Init function for JEP-0045.
Status Internal
--*/
int XMPP_MucRoom_Init()
{
	static int init;
	if(init)return(0);
	init=1;

	xmpp_jep45_if=XMPP_CreateInterface(NULL);
	xmpp_jep45_if->message=&xmpp_jep45_message;
	xmpp_jep45_if->presence=&xmpp_jep45_presence;

	return(0);
}
