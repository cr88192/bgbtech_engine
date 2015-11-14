#include <bgbnet.h>

/*--
Cat pdnet;NET
Form
	int NET_AddrEqual(VADDR *a, VADDR *b);
Description
	Compares 2 addresses, returning a nonzero value if they are equal.
	If both addresses have the same type, address, and port then they are viewed as equal.
	An exception is in the case of addresses with ports of 0, which will match with any other address of the same protocol and address.
--*/

int NET_AddrEqual(VADDR *a, VADDR *b)
{
	if(!a || !b)return(0);
	if(a->proto != b->proto)return(0);
	switch(a->proto)
	{
	case PROTO_IPV4UDP:
		if(a->ipv4.addr != b->ipv4.addr)return(0);
		if(a->ipv4.port && b->ipv4.port)
			if(a->ipv4.port != b->ipv4.port)
			return(0);
		break;
	case PROTO_IPV4TCP:
		if(a->ipv4.addr != b->ipv4.addr)return(0);
		if(a->ipv4.port && b->ipv4.port)
			if(a->ipv4.port != b->ipv4.port)
			return(0);
		break;
	default:
		return(0);
		break;
	}
	return(1);
}

/*--
Cat pdnet;NET
Form
	int NET_Poll();
Description
	Polls for any activity over the network, calls handlers, ...
--*/
int NET_Poll()
{
	static int i;

//	if(!i)i=1000;
//	i--;
//	if(!i)*(int *)-1=-1;

	Meta0_Poll();
	NETAPI_Poll();

//	HttpNode_Poll();
//	SXRPC_Poll();

	return(0);
}

int NET_InitLow ();

/*--
Cat pdnet;NET
Form
	int NET_Init();
Description
	Initializes pdnet core network stuff.
	Does not start any specific protocols or services.
--*/
int NET_Init()
{
	static int init=0;
	if(init)return(0);
	init=1;

	NET_InitLow();
	Meta0_Init();
	NETAPI_Init();

//	HttpNode_Init();
//	SXRPC_Init();

	return(0);
}
