#ifndef __PDNET_XMPP_H__
#define __PDNET_XMPP_H__

/*--
Cat pdnet;Protocols;XMPP/Jabber
Name XMPP_Interface Struct
Description
	typedef struct XMPP_Interface_s XMPP_Interface;
	
	struct XMPP_Interface_s {
	XMPP_Interface *next;
	
	char *xmlns;
	int (*message)(Meta0_Con *con, NetParse_Node *node);
	int (*iq)(Meta0_Con *con, NetParse_Node *node);
	int (*text_message)(char *from, char *subj, char *body);
	int (*presence)(Meta0_Con *con, NetParse_Node *node);
	};
--*/

typedef struct XMPP_Interface_s XMPP_Interface;

struct XMPP_Interface_s {
XMPP_Interface *next;

char *xmlns;
int (*message)(Meta0_Con *con, dyxNode *node);
int (*iq)(Meta0_Con *con, dyxNode *node);
int (*text_message)(char *from, char *subj, char *body);
int (*presence)(Meta0_Con *con, dyxNode *node);
};

typedef struct {
char *node;
char *host;
char *rsrc;

VADDR *addr;
char *obj;
}XMPP_RefInfo;


//util
typedef struct XMPP_MessagePool_s XMPP_MessagePool;

struct XMPP_MessagePool_s {
XMPP_MessagePool *next;
char **from;
char **message;
int first, last;
};

//jep45
typedef struct XMPP_MucRoom_s XMPP_MucRoom;

struct XMPP_MucRoom_s {
XMPP_MucRoom *next;

char *room;
char *host;
char *nick;

int num_users;
char **users;
void *data;

int (*message)(XMPP_MucRoom *room, void *data, dyxNode *msg);
int (*presence)(XMPP_MucRoom *room, void *data, dyxNode *msg);

int (*add_user)(XMPP_MucRoom *room, void *data, char *name);
int (*remove_user)(XMPP_MucRoom *room, void *data, char *name);
};

typedef struct XMPP_PoolExport_s XMPP_PoolExport;

struct XMPP_PoolExport_s {
XMPP_PoolExport *next;
char *name;
void *data;
int (*func)(char *name, void *data, void **msg);
};

typedef struct {
char *jid;
char *name;
}XMPP_PoolReference;

#endif
