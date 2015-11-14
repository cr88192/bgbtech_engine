/*--
Cat pdnet;NetVal
Text
	objective:
	create code for managing net-related dynamic types.
	make use of pdlib type system as base.
--*/

#include <bgbnet.h>

/*--
Cat pdnet;NetVal
Form
	void *NetVal_WrapInt(int i);
Description
	Wraps an integer so that it can be passed through the RPC subsystem.
--*/
void *NetVal_WrapInt(int i)
{
	int *p;

	p=gctalloc("int_t", 0);
	*p=i;

	return((void *)p);
}

/*--
Cat pdnet;NetVal
Form
	void *NetVal_WrapStr(int i);
	void *NetVal_WrapString(int i);
Description
	Wraps a string so that it can be passed through the RPC subsystem.
--*/
void *NetVal_WrapStr(char *s)
{
	char *t;

	t=gctalloc("string_t", strlen(s)+1);
	strcpy(t, s);

	return(t);
}

void *NetVal_WrapString(char *s)
{
	char *t;

	t=gctalloc("string_t", strlen(s)+1);
	strcpy(t, s);

	return(t);
}

/*--
Cat pdnet;NetVal
Form
	void *NetVal_WrapFloat(double f);
Description
	Wraps a double so that it can be passed through the RPC subsystem.
--*/
void *NetVal_WrapFloat(double f)
{
	double *p;

	p=gctalloc("float_t", 0);
	*p=f;

	return((void *)p);
}

/*--
Cat pdnet;NetVal
Form
	void *NetVal_WrapArray(void *a);
Description
	Wraps an array so that it can be passed through the rpc subsystem.
--*/
void *NetVal_WrapArray(void *a)
{
	void **ip, **op;
	int i;

	ip=(void **)a;
	for(i=0; ip[i]; i++);

	op=gctalloc("array_t", (i+1)*sizeof(void *));
	for(i=0; ip[i]; i++)op[i]=ip[i];
	op[i++]=NULL;

	return((void *)op);
}

int NetVal_Init()
{
	gctallocType("int_t", "int;");
	gctallocType("float_t", "double;");
	gctallocType("bool_t", "int;");
	gctallocType("string_t", "char+");
	gctallocType("date_t",
		"2byte:Y;byte:M;byte:D;"
		"byte:h;byte:m;byte:s;");
	gctallocType("data_t", "byte+");

	gctallocType("array_t", "*struct+");
	gctallocType("d_array_t", "*array_t;");
	gctallocType("d_struct_t", "*array_t:names;*array_t:values;");

	gctallocType("netref_t", "int;int;int;");

	return(1);
}