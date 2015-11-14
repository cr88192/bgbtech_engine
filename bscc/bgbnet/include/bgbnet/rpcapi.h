#ifndef __PDLIB_RPCAPI_H__
#define __PDLIB_RPCAPI_H__

typedef struct NET_Export_s NET_Export;
typedef struct NET_Interface_s NET_Interface;
typedef struct NET_Reference_s NET_Reference;

typedef void *NETVAL;

typedef struct NET_Array_s NET_Array;
typedef struct NET_Struct_s NET_Struct;

struct NET_Export_s {
NET_Export *next;
int (*func)();
char *name;
char *ret;
char **args;
};



struct NET_Interface_s {
NET_Interface *next;
char *name;	//protocol name
NET_Reference *(*decode_url)(char *s);
char *(*encode_url)(NET_Reference *ref);
long (*rpc_call)(NET_Reference *ref, char *ret, long *args, char **types);
int (*poll)();
};

struct NET_Reference_s {
NET_Interface *iface;
void *nodeinfo;
void *refinfo;
};

struct NET_Array_s {
char **types;
long *values;
};

struct NET_Struct_s {
char **types;
char **names;
long *values;
};

#endif
