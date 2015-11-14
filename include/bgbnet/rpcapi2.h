#ifndef __PDLIB_RPCAPI_H__
#define __PDLIB_RPCAPI_H__

typedef struct NET_Export_s NET_Export;
typedef struct NET_Interface_s NET_Interface;
typedef struct NET_Reference_s NET_Reference;

struct NET_Export_s {
NET_Export *next;
char *name;
void *data;
int args;

void *(*func)();
void *(*func2)(void *data);
};



struct NET_Interface_s {
NET_Interface *next;
char *name;	//protocol name
NET_Reference *(*decode_url)(char *s);
char *(*encode_url)(NET_Reference *ref);
void *(*rpc_call)(NET_Reference *ref, void **args);
void *(*rpc_call_func)(NET_Reference *ref, char *func, void **args);
int (*poll)();
void *(*fetch)(NET_Reference *ref);
void *(*gen_link)(char *exp); //make a link to self with a given export
void *(*conv_link)(void *link); //convert a link to an object or netref
};

struct NET_Reference_s {
NET_Interface *iface;
void *nodeinfo;
void *refinfo;
};

int NET_ExportFunc(char *name, void *(*func)());
int NET_ExportFunc2(char *name, int args, void *(*func)(void *data), void *data);
NET_Interface *NET_CreateInterface(char *name);
NET_Interface *NET_FindInterface(char *name);
NET_Export *NET_FindExport(char *name);
NET_Reference *NET_DecodeURL(char *url);
char *NET_EncodeURL(NET_Reference *ref);
void *NET_CallReference(NET_Reference *ref, void **args);
void *NET_CallReferenceFunc(NET_Reference *ref, char *func, void **args);
void *NET_FetchReference(NET_Reference *ref);
void *NET_GenLink(char *scheme, char *exp);
void *NET_ConvLink(void *link);
void *NET_ApplyFunc1(void *(*f)(void *x0, ...), int i, void **args);
void *NET_ApplyFunc2(void *(*f)(void *data, ...), int i, void *data, void **args);
void *NET_CallExport(char *name, void **args);
int NETAPI_Poll();
int NETAPI_Init();

#endif
