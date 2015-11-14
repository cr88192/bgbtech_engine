/*--
Cat pdnet;RPC
Text
	create a number of primitive types 'type_t' that are structs \
	containing a single elememt of the given primitive type:
	"int_t": "int;";
	"float_t": "float;";
	...
	
	and some specials:
	"string_t": "char+" (strings need to be correctly dynamic, and a \
	cast of "*(char **)p" is an annoyance);
	"array_t": "*struct+".
	
	structs can be associative, ie: pairs of pointers with the first \
	being the name and the second the value (they will be represented \
	as normal arrays).
	
	annoyingly neither arrays nor structs would be dynamically sizeable. \
	involving an extra indirection step could fix this but I am unsure \
	if it is worth it.
	additionally, both static and dynamic structs/arrays could exist:
	"d_array_t": "*array_t;";
	"d_struct_t": "*array_t;*array_t;".
	these would be dynamically resized if too small...
--*/

/*
	goal:
	provide a fairly abstract interface into rpc subsystem.
	
	2 arrays to define args:
	first arg type;
	second arg value.
	
	arg types:
	int: long-int value;
	bool: long-int bool value;
	string: string pointer;
	float: float value;
	date: pointer to a byte array, YMDhms;
	data: pointer to a long size followed by a buffer;
	
	array: pointer to an array structure,
	this has:
	a pointer to a types array (null terminated);
	a pointer to a values array.
	
	struct: pointer to an struct structure,
	this has:
	a pointer to a types array;
	a pointer to a names array;
	a pointer to a values array.
	
	2003-09-01:
	this type system blows ass.
	replace by making use of pd-lib type-check facilities, and have everything be
	of type 'void *' and point to the body of the object.
	create a number of primitive types 'type_t' that are structs containing a
	single elememt of the given primitive type:
	"int_t": "int;";
	"float_t": "float;";
	...
	
	and some specials:
	"string_t": "char+" (strings need to be correctly dynamic, and a cast of
	"*(char **)p" is an annoyance);
	"array_t": "*struct+".
	
	structs can be associative, ie: pairs of pointers with the first being the
	name and the second the value (they will be represented as normal arrays).
	
	annoyingly neither arrays nor structs would be dynamically sizeable. involving
	an extra indirection step could fix this but I am unsure if it is worth it.
	additionally, both static and dynamic structs/arrays could exist:
	"d_array_t": "*array_t;";
	"d_struct_t": "*array_t;*array_t;".
	these would be dynamically resized if too small...
*/

#include <bgbnet.h>

#include <stdarg.h>

NET_Export *net_exports;
NET_Interface *net_interfaces;

/*--
Cat pdnet;RPC
Form
	int NET_ExportFunc(char *name, void *(*func)());
Description
	Export a function to the rpc subsystem.
	The function is called with a variable number of arguments coresponding to those recieved on the call.
	Each arg is a NetVal.
--*/
int NET_ExportFunc(char *name, void *(*func)())
{
	NET_Export *tmp;

	char *abuf[16];
	char buf[16];
	char *s, *t;
	int i, j;

	tmp=gcalloc(sizeof(NET_Export));
	tmp->name=dystrdup(name);
	tmp->func=func;

	tmp->next=net_exports;
	net_exports=tmp;

	return(0);
}

/*--
Cat pdnet;RPC
Form
	int NET_ExportFunc2(char *name, int args, void *(*func)(void *data), void *data);
Description
	Exports a function to RPC.
	name is the name used to call the function;
	args is the number of arguments func accepts:
		>=0, function accepts at least args arguments;
		<0, function accepts |args|-1 basic arguments, and the rest as a null terminated array.
	func is called with the first argument being a pointer to data.
	data is the pointer passed as the first arg on call.
--*/
int NET_ExportFunc2(char *name, int args, void *(*func)(void *data), void *data)
{
	NET_Export *tmp;

	char *abuf[16];
	char buf[16];
	char *s, *t;
	int i, j;

	tmp=gcalloc(sizeof(NET_Export));
	tmp->name=dystrdup(name);

	tmp->args=args;
	tmp->data=data;
	tmp->func2=func;

	tmp->next=net_exports;
	net_exports=tmp;

	return(0);
}

/*--
Cat pdnet;RPC
Form
	NET_Interface *NET_CreateInterface(char *name);
Description
	Creates an "interface".
	Interfaces are used within the protocol code to handle tasks such as:
		Enoding URL's;
		Decoding URL's;
		Executing RPC calls;
		...
	name is used to identify urls related to this interface.
	name may be null, in which case the interface does not have urls \
	(this may be useful, eg, if the interface just needs to poll or such).
--*/
NET_Interface *NET_CreateInterface(char *name)
{
	NET_Interface *tmp;

	tmp=gcalloc(sizeof(NET_Interface));
	memset(tmp, 0, sizeof(NET_Interface));

	if(name)tmp->name=dystrdup(name);

	tmp->next=net_interfaces;
	net_interfaces=tmp;

	return(tmp);
}

/*--
Cat pdnet;RPC
Form
	NET_Interface *NET_FindInterface(char *name);
Description
	Find the interface associated with a given name.
	Returns NULL if the interface is not found.
--*/
NET_Interface *NET_FindInterface(char *name)
{
	NET_Interface *cur;

	cur=net_interfaces;
	while(cur)
	{
		if(cur->name)if(!strcmp(cur->name, name))
			return(cur);
		cur=cur->next;
	}
	return(NULL);
}

/*--
Cat pdnet;RPC
Form
	NET_Export *NET_FindExport(char *name);
Description
	Finds an exported function.
--*/
NET_Export *NET_FindExport(char *name)
{
	NET_Export *cur;

	cur=net_exports;
	while(cur)
	{
		if(!strcmp(cur->name, name))return(cur);
		cur=cur->next;
	}
	return(NULL);
}

/*--
Cat pdnet;RPC
Form
	NET_Reference *NET_DecodeURL(char *url);
Description
	Decodes a URL.
	URL's have the form <scheme>:<protocol-specific>.
		scheme refers to the specific interface.
--*/
NET_Reference *NET_DecodeURL(char *url)
{
	char buf[16];
	char *s, *t;
	NET_Interface *iface;
	NET_Reference *tmp;

	s=url;
	t=buf;
	while(*s && (*s!=':'))*t++=*s++;
	*t++=0;
	s++;

	iface=NET_FindInterface(buf);
	if(!iface)return(NULL);

	if(!iface->decode_url)return(NULL);
	tmp=iface->decode_url(s);
	if(tmp)
		tmp->iface=iface;

	return(tmp);
}

/*--
Cat pdnet;RPC
Form
	char *NET_EncodeURL(NET_Reference *ref);
Description
	Encodes a URL.
--*/
char *NET_EncodeURL(NET_Reference *ref)
{
	if(!ref->iface->encode_url)return(NULL);
	return(ref->iface->encode_url(ref));
}

/*--
Cat pdnet;RPC
Form
	void *NET_CallReference(NET_Reference *ref, void **args);
Description
	Calls a reference (gained via decoding a url or such).
--*/
void *NET_CallReference(NET_Reference *ref, void **args)
{
	void *tmp;

	if(!ref->iface->rpc_call)return(NULL);
	tmp=ref->iface->rpc_call(ref, args);
	return(tmp);
}

/*--
Cat pdnet;RPC
Form
	void *NET_CallReferenceFunc(NET_Reference *ref, char *func, void **args);
Description
	Calls a function. The reference only identifies the host.
--*/
void *NET_CallReferenceFunc(NET_Reference *ref, char *func, void **args)
{
	void *tmp;

	if(!ref->iface->rpc_call_func)return(NULL);
	tmp=ref->iface->rpc_call_func(ref, func, args);
	return(tmp);
}

/*--
Cat pdnet;RPC
Form
	void *NET_FetchReference(NET_Reference *ref);
Description
	Retrieves the data associated with a given reference.
--*/
void *NET_FetchReference(NET_Reference *ref)
{
	void *tmp;

	if(!ref->iface->fetch)return(NULL);
	tmp=ref->iface->fetch(ref);
	return(tmp);
}

/*--
Cat pdnet;RPC
Form
	void *NET_GenLink(char *scheme, char *export);
Description
	Generate a link to self for a given scheme and export.
--*/
void *NET_GenLink(char *scheme, char *exp)
{
	void *tmp;
	char *s, *t;
	NET_Interface *iface;

	iface=NET_FindInterface(scheme);
	if(!iface)return(NULL);

	if(!iface->gen_link)return(NULL);
	tmp=iface->gen_link(exp);
	return(tmp);
}

/*--
Cat pdnet;RPC
Form
	void *NET_ConvLink(void *link);
Description
	Generate a link to self for a given scheme and export.
--*/
void *NET_ConvLink(void *link)
{
	void *tmp;
	char *s, *t;
	NET_Interface *iface;
	char buf[16];

	s=link;
	t=buf;
	while(*s && (*s!=':'))*t++=*s++;
	*t++=0;
	s++;

	iface=NET_FindInterface(buf);
	if(!iface)return(NULL);

	if(!iface->conv_link)return(NULL);
	tmp=iface->conv_link(link);
	return(tmp);
}

void *NET_ApplyFunc1(void *(*f)(void *x0, ...), int i, void **args)
{
	switch(i)
	{
	case 0:
		return(f(NULL));
	case 1:
		return(f(args[0], NULL));
	case 2:
		return(f(args[0], args[1], NULL));
	case 3:
		return(f(args[0], args[1], args[2], NULL));
	case 4:
		return(f(args[0], args[1], args[2], args[3], NULL));
	case 5:
		return(f(args[0], args[1], args[2], args[3], args[4], NULL));
	case 6:
		return(f(args[0], args[1], args[2], args[3], args[4], args[5], NULL));
	case 7:
		return(f(args[0], args[1], args[2], args[3], args[4], args[5], args[6], NULL));
	case 8:
		return(f(args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], NULL));
	default:
		return(f(args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], NULL));
	}
	return(NULL);
}

void *NET_ApplyFunc2(void *(*f)(void *data, ...), int i, void *data, void **args)
{
	switch(i)
	{
	case 0:
		return(f(data, NULL));
	case 1:
		return(f(data, args[0], NULL));
	case 2:
		return(f(data, args[0], args[1], NULL));
	case 3:
		return(f(data, args[0], args[1], args[2], NULL));
	case 4:
		return(f(data, args[0], args[1], args[2], args[3], NULL));
	case 5:
		return(f(data, args[0], args[1], args[2], args[3], args[4], NULL));
	case 6:
		return(f(data, args[0], args[1], args[2], args[3], args[4], args[5], NULL));
	case 7:
		return(f(data, args[0], args[1], args[2], args[3], args[4], args[5], args[6], NULL));
	case 8:
		return(f(data, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], NULL));
	default:
		return(f(data, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], NULL));
	}
	return(NULL);
}

/*--
Cat pdnet;RPC
Form
	void *NET_CallExport(char *name, void **args);
Description
	Calls an exported function.
	This is intended primarily to be used within protocols to handle RPC requests.
--*/
void *NET_CallExport(char *name, void **args)
{
	NET_Export *f;
	int i, j;
	void **args2;
	void *argb[16];

	f=NET_FindExport(name);
	if(!f)
	{
		gc_printf("NET_CallExport: no func '%s'\n", name);
		return(NULL);
	}

	for(i=0; args[i]; i++);

	if(f->args>=0)
	{
		if(i<f->args)
		{
			gc_printf("NET_CallExport: Args Count mismatch %d<%d\n",
				i, f->args);
			return(NULL);
		}
		args2=args;
	}
	if(f->args<0)
	{
		j=0-f->args;
		args2=argb;
		for(i=0; i<(j-1); i++)args2[i]=args[i];
		args2[j-i]=args+(j-1);

		i=j;
	}

	if(f->func)
		return(NET_ApplyFunc1(f->func, i, args2));
	if(f->func2)
		return(NET_ApplyFunc2(f->func2, i, f->data, args2));
	return(NULL);
}

int NETAPI_Poll()
{
	NET_Interface *cur;

	cur=net_interfaces;
	while(cur)
	{
		if(cur->poll)cur->poll();
		cur=cur->next;
	}
	return(0);
}

int NETAPI_Init()
{
	net_exports=NULL;
	net_interfaces=NULL;

//	NetVal_Init();

	return(0);
}
