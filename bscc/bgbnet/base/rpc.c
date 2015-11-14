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

int NET_ExportFunc(char *name, int (*func)(), char *ret, char *args)
{
	NET_Export *tmp;

	char *abuf[16];
	char buf[16];
	char *s, *t;
	int i, j;

	tmp=malloc(sizeof(NET_Export));
	tmp->func=func;
	tmp->name=strdup(name);
	tmp->ret=strdup(ret);

	s=args;
	i=0;
	while(*s)
	{
		t=buf;
		while(*s && (*s!=';') && (*s!='+') && (*s!=':'))
			*t++=*s++;
		while(*s && (*s!=';') && (*s!='+'))s++;
		*t++=0;
		if(*s=='+')s++;
		if(*s==';')s++;

		abuf[i++]=strdup(buf);
	}
	abuf[i++]=NULL;

	tmp->args=malloc(i*sizeof(char *));
	j=i;
	for(i=0; i<j; i++)tmp->args[i]=abuf[i];


	tmp->next=net_exports;
	net_exports=tmp;

	return(0);
}

NET_Interface *NET_CreateInterface(char *name)
{
	NET_Interface *tmp;

	tmp=malloc(sizeof(NET_Interface));
	memset(tmp, 0, sizeof(NET_Interface));

	tmp->name=strdup(name);

	tmp->next=net_interfaces;
	net_interfaces=tmp;

	return(tmp);
}

NET_Interface *NET_FindInterface(char *name)
{
	NET_Interface *cur;

	cur=net_interfaces;
	while(cur)
	{
		if(!strcmp(cur->name, name))return(cur);
		cur=cur->next;
	}
	return(NULL);
}

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
	tmp=iface->decode_url(s);

	return(tmp);
}

char *NET_EncodeURL(NET_Reference *ref)
{
	return(ref->iface->encode_url(ref));
}

long NET_CallReference(NET_Reference *ref, char *ret, char *form, ...)
{
	char *s, *t;
	char buf[16];
	va_list lst;
	int i;
	char *abuf[16];
	long bbuf[16];
	long tmp;

	va_start(lst, form);

	s=form;
	i=0;
	while(*s)
	{
		t=buf;
		while(*s && (*s!=';') && (*s!='+') && (*s!=':'))
			*t++=*s++;
		while(*s && (*s!=';') && (*s!='+'))s++;
		*t++=0;
		if(*s=='+')s++;
		if(*s==';')s++;

		abuf[i]=strdup(buf);
		bbuf[i]=va_arg(lst, long);
		i++;
	}
	abuf[i]=NULL;

	va_end(lst);

	tmp=ref->iface->rpc_call(ref, ret, bbuf, abuf);
	return(tmp);
}

long NET_CallExport(char *name, char **ret, long *args, char **types)
{
	NET_Export *f;
	int i;

	union {
		long (*f0)();
		long (*f1)(long);
		long (*f2)(long, long);
		long (*f3)(long, long, long);
		long (*f4)(long, long, long, long);
		long (*f5)(long, long, long, long, long);
		long (*f6)(long, long, long, long, long, long);
		long (*f7)(long, long, long, long, long, long, long);
		long (*f8)(long, long, long, long, long, long, long, long);
	}fn;

	f=NET_FindExport(name);
	if(!f)
	{
		gc_printf("NET_CallExport: no func '%s'\n", name);
		*ret=dystrdup("NoFunc");
		return(-1);
	}

	*ret=f->ret;

	for(i=0; types[i] || f->args[i]; i++)
	{
		if(!types[i])
		{
			gc_printf("NETAPI: args count mismatch.\n");
			return(-1);
		}
		if(!f->args[i])
		{
			gc_printf("NETAPI: args count mismatch.\n");
			return(-1);
		}
		if(strcmp(types[i], f->args[i]))
		{
			gc_printf("NETAPI: args type mismatch.\n");
			return(-1);
		}
	}

	fn.f0=f->func;
	switch(i)
	{
	case 0:
		return(fn.f0());
	case 1:
		return(fn.f1(args[0]));
	case 2:
		return(fn.f2(args[0], args[1]));
	case 3:
		return(fn.f3(args[0], args[1], args[2]));
	case 4:
		return(fn.f4(args[0], args[1], args[2], args[3]));
	case 5:
		return(fn.f5(args[0], args[1], args[2], args[3], args[4]));
	case 6:
		return(fn.f6(args[0], args[1], args[2], args[3], args[4], args[5]));
	case 7:
		return(fn.f7(args[0], args[1], args[2], args[3], args[4], args[5], args[6]));
	case 8:
		return(fn.f8(args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7]));
	default:
		return(fn.f8(args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7]));
	}
	return(0);
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

	return(0);
}
