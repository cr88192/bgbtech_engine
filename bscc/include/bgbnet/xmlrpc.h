#ifndef __PDNET_XMLRPC_H__
#define __PDNET_XMLRPC_H__

typedef struct XMLRPC_Extension_s XMLRPC_Extension;
typedef struct XMLRPC_PeerInfo_s XMLRPC_PeerInfo;
typedef struct XMLRPC_Context_s XMLRPC_Context;

struct XMLRPC_Extension_s {
XMLRPC_Extension *next;

char *xmlns;
dyxNode *(*encode)(XMLRPC_Context *ctx, void *obj);
void *(*decode)(XMLRPC_Context *ctx, dyxNode *node);
};

struct XMLRPC_PeerInfo_s {
XMLRPC_PeerInfo *next;

char *name;
char **namespaces;
XMLRPC_Extension **extensions;
};

struct XMLRPC_Context_s {
XMLRPC_PeerInfo *peer;
int error_code;
char *error_str;
};

#endif
