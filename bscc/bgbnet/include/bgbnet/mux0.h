#ifndef __PDNET_MUX0_H__
#define __PDNET_MUX0_H__

typedef struct MUX_Link_s MUX_Link;
typedef struct MUX_Interface_s MUX_Interface;
typedef struct MUX_Message_s MUX_Message;

struct MUX_Link_s {
MUX_Link *next;

char *name;
VADDR *addr;
Meta0_Con *con;

dyxAttr *attr;

int bufsz, bufend;
byte *buf;
};

struct MUX_Interface_s {
MUX_Interface *next;

char *ns;
int (*message)(MUX_Link *link, MUX_Message *msg);
};

struct MUX_Message_s {
MUX_Message *next;
MUX_Link *link;

dyxAttr *attr;

int size;
byte *body;
};

#endif
