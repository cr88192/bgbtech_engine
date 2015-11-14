#ifndef __SXRPC_H__
#define __SXRPC_H__

#define SXRPC_NULL	0
#define SXRPC_SYNC	1
#define SXRPC_CONNECTED	2
#define SXRPC_CLOSE	3

typedef struct {
VFILE *sock;
VADDR *addr;	//initial recieved addr, port may change
VGUID nodeid;
int num;
byte *buf, *end;

int state;
byte *rbuf, *erbuf;
int rlen;

char req[1024];
}SXRPC_Con;

typedef struct {
VADDR *host;
char *proto;
char *name;
int objid;
}SXRPC_Link;

#endif
