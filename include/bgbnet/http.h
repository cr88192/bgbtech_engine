#ifndef __PDNET_HTTP_H__
#define __PDNET_HTTP_H__

typedef struct {
VFILE *sock;
int num;
byte *buf, *end;

int state;
byte *rbuf, *erbuf;
int rlen;

char req[256];
char ct[64];
char arg[1024];
}HTTP_Con;

typedef struct HTTP_Resource_s HTTP_Resource;

struct HTTP_Resource_s {
HTTP_Resource *next;

char *base;
void *data;

int (*get)(HTTP_Resource *rsrc, HTTP_Con *con, char **type, char **data, int *len);
int (*post)(HTTP_Resource *rsrc, HTTP_Con *con, char **type, char **data, int *len);
int (*head)(HTTP_Resource *rsrc, HTTP_Con *con, char **type, int *len);
};


typedef struct HTTP_RefInfo_s {
VADDR *addr;
char *host;
char *req;
char *ext;

int st, clen;

byte *rbuf, *erbuf;
int rlen, ok;
}HTTP_RefInfo;

#define HTTPSTATE_REQ	0
#define HTTPSTATE_GET	1
#define HTTPSTATE_POST	2
#define HTTPSTATE_HEAD	3
#define HTTPSTATE_RECD	4
#define HTTPSTATE_CLOSE	5

#endif
