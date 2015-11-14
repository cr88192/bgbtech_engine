#include <bgbdy.h>

#ifndef BGBNET_H
#define BGBNET_H

#ifdef WIN32
#ifdef BGBNET_DLL
#define BGBNET_API __declspec(dllexport)
#else
#define BGBNET_API __declspec(dllimport)
#endif
#else
#define BGBNET_API
#endif

/*--
Cat bgbnet;NET;Sock
Name VGUID
Description
	typedef struct {
	byte data1[4];
	byte data2[2];
	byte data3[2];
	byte data4[8];
	}VGUID;
--*/

typedef struct {
byte data1[4];
byte data2[2];
byte data3[2];
byte data4[8];
}VGUID;

#define PROTO_IPV4	(1<<8)
#define	PROTO_IPV6	(2<<8)
#define PROTO_TCP	1
#define	PROTO_UDP	2

#define PROTO_UNDEFINED	0
#define PROTO_IPV4UDP	(PROTO_IPV4|PROTO_UDP)
#define PROTO_IPV4TCP	(PROTO_IPV4|PROTO_TCP)
#define PROTO_IPV6UDP	(PROTO_IPV6|PROTO_UDP)
#define PROTO_IPV6TCP	(PROTO_IPV6|PROTO_TCP)

/*--
Cat bgbnet;NET;Sock
Form
	PROTO_IPV4
	PROTO_IPV6
	PROTO_TCP
	PROTO_UDP
	PROTO_UNDEFINED
	PROTO_IPV4UDP
	PROTO_IPV4TCP
	PROTO_IPV6UDP
	PROTO_IPV6TCP
Description
	constants defined within VADDR.
--*/

/* these may be loaded in the lower 8 flag bits for send and similar */
#define PROTO_PF_UNDEFINED	0

#define PROTO_PF_IPV4		4
#define PROTO_PF_TCP		6
#define PROTO_PF_UDP		17

#define PROTO_PF_IPV6		41
#define PROTO_PF_IPV6ROUTE	43
#define PROTO_PF_IPV6FRAG	44
#define PROTO_PF_IPV6ICMP	58
#define PROTO_PF_IPV6NONXT	59
#define PROTO_PF_IPV6OPTS	60

typedef union VADDR_u VADDR;

union VADDR_u {
	int proto;

	struct {
	int proto;
	int flags; /* lower 8 bits=netmask count */
	unsigned short port;
	unsigned long addr;
	}ipv4;

	struct {
	int proto;
	int flags; /* lower 8 bits=netmask count */
	unsigned short port;
	byte addr[16];
	}ipv6;
};

/*--
Cat bgbnet;NET;Sock
Form
	typedef union VADDR_u VADDR;
	
	union VADDR_u {
		int proto;
	
		struct {
		int proto;
		int flags;
		unsigned short port;
		unsigned long addr;
		}ipv4;
	
		struct {
		int proto;
		int flags;
		unsigned short port;
		byte addr[16];
		}ipv6;
	};

Description
	VADDR structure.
--*/

#define VFSOCK_IOCTL_ACCEPT	1
#define VFSOCK_IOCTL_CANWRITE	2

#ifdef __cplusplus
extern "C" {
#endif

#include <bgbnet/http.h>
//#include <bgbnet/sxrpc.h>
//#include <bgbnet/parse.h>
#include <bgbnet/rpcapi2.h>
#include <bgbnet/meta0.h>
//#include <bgbnet/mrpt022.h>
#include <bgbnet/xmpp.h>
#include <bgbnet/xmlrpc.h>
#include <bgbnet/objlst.h>
#include <bgbnet/mux0.h>

#include <bgbnet/bsxrp.h>

#include <bgbnet/auto.h>
#include <bgbnet/autoi.h>
// #include <bgbnet/ty_auto.h>

#ifdef __cplusplus
}
#endif

#endif
