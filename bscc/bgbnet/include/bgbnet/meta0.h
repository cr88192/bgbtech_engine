#ifndef __PDNET_META0_H__
#define __PDNET_META0_H__

typedef struct Meta0_PortInfo_s Meta0_PortInfo;
typedef struct Meta0_Con_s Meta0_Con;

struct Meta0_PortInfo_s {
VFILE *sock;
int port;
char *name;

int num;

int (*new_con)(Meta0_Con *con);		//called when accepting a new connection
int (*input)(Meta0_Con *con);		//called when new input arrives
int (*closed)(Meta0_Con *con);		//called when the connection is closed
int (*think)(Meta0_Con *con);		//called on poll events
};

struct Meta0_Con_s {
VFILE *sock;
VADDR *addr;	//initial recieved addr, port may change

Meta0_PortInfo *info;
void *data;

int num;
byte *buf, *end;
};

#endif
