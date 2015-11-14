#include <bgbnet.h>

/*--
Cat pdnet;RPC;XML-RPC;Encode
Form
	dyxNode *XmlRpc_EncodeArray(void *val, XMLRPC_Context *ctx);
Description
	Encode an XML-RPC Array.
	val is of the NET_Array type.
Status Internal
--*/
dyxNode *XmlRpc_EncodeArray(void *val, XMLRPC_Context *ctx)
{
	dyxNode *t, *t2, *lst;
	NET_Array *ns;
	int i;

	ns=(NET_Array *)val;

	lst=NULL;
	for(i=0; ns->values[i]; i++)
	{
//		t2=XmlRpc_EncodeValue(ns->values[i], ctx);
//		t=dyxNewNode();
//		t->key=dystrdup("value");
//		t->first=t2;

		t=XmlRpc_EncodeValueExtension(ctx, ns->values[i]);
		lst=dyxAddNodeEnd(lst, t);
	}
	t2=dyxNewNode();
	t2->key=dystrdup("data");
	t2->first=lst;

	t=dyxNewNode();
	t->key=dystrdup("array");
	t->first=t2;

	return(t);
}

/*--
Cat pdnet;RPC;XML-RPC;Encode
Form
	dyxNode *XmlRpc_EncodeArray2(void **val, XMLRPC_Context *ctx);
Description
	Encode an XML-RPC Array.
	val is an array of pointers.
Status Internal
--*/
dyxNode *XmlRpc_EncodeArray2(void **val, XMLRPC_Context *ctx)
{
	dyxNode *t, *t2, *lst;
	int i;

	lst=NULL;
	for(i=0; val[i]; i++)
	{
//		t2=XmlRpc_EncodeValue(val[i], ctx);
//		t=dyxNewNode();
//		t->key=dystrdup("value");
//		t->first=t2;

		t=XmlRpc_EncodeValueExtension(ctx, val[i]);
		lst=dyxAddNodeEnd(lst, t);
	}
	t2=dyxNewNode();
	t2->key=dystrdup("data");
	t2->first=lst;

	t=dyxNewNode();
	t->key=dystrdup("array");
	t->first=t2;

	return(t);
}

/*--
Cat pdnet;RPC;XML-RPC;Encode
Form
	dyxNode *XmlRpc_EncodeStruct(void *val, XMLRPC_Context *ctx);
Description
	Encode an XML-RPC Struct.
Status Internal
--*/
dyxNode *XmlRpc_EncodeStruct(void *val, XMLRPC_Context *ctx)
{
	dyxNode *t, *t2, *t3, *lst;
	NET_Struct *ns;
	int i;

	ns=(NET_Struct *)val;

	lst=NULL;
	for(i=0; ns->values[i]; i++)
	{

//		t3=XmlRpc_EncodeValue(ns->values[i], ctx);
//		t2=dyxNewNode();
//		t2->key=dystrdup("value");
//		t2->first=t3;

		t2=XmlRpc_EncodeValueExtension(ctx, ns->values[i]);

		t3=dyxNewNode();
		t3->text=dystrdup(ns->names[i]);
		t=dyxNewNode();
		t->key=dystrdup("name");
		t->first=t3;
		t->next=t2;

		t2=t;
		t=dyxNewNode();
		t->key=dystrdup("member");
		t->first=t2;

		lst=dyxAddNodeEnd(lst, t);
	}
	t=dyxNewNode();
	t->key=dystrdup("struct");
	t->first=lst;

	return(t);
}

/*--
Cat pdnet;RPC;XML-RPC;Encode
Form
	dyxNode *XmlRpc_EncodeValue(void *val, XMLRPC_Context *ctx);
Description
	Encode an XML-RPC Value.
Status Internal
--*/
dyxNode *XmlRpc_EncodeValue(void *val, XMLRPC_Context *ctx)
{
	dyxNode *t, *t2;
	char buf[16];
	byte *s, *s2;
	int i;
	char *type;

	type=ObjType_GetTypeName(val);

	if(!strcmp(type, "int_t") || !strcmp(type, "char_t"))
	{
		sprintf(buf, "%d", *(int *)val);
		t2=dyxNewNode();
		t2->text=dystrdup(buf);

		t=dyxNewNode();
		t->key=dystrdup("i4");
		t->first=t2;

		return(t);
	}

	if(!strcmp(type, "bool_t"))
	{
		sprintf(buf, "%d", *(int *)val);
		t2=dyxNewNode();
		t2->text=dystrdup(buf);

		t=dyxNewNode();
		t->key=dystrdup("boolean");
		t->first=t2;

		return(t);
	}

	if(!strcmp(type, "string_t") ||
		!strcmp(type, "symbol_t") ||
		!strcmp(type, "link_t"))
	{
//		sprintf(buf, "%s", val);
		t2=dyxNewNode();
		t2->text=dystrdup((char *)val);

		t=dyxNewNode();
		t->key=dystrdup("string");
		t->first=t2;

		return(t);
	}

	if(!strcmp(type, "float_t"))
	{
		sprintf(buf, "%f", *(double *)val);
		t2=dyxNewNode();
		t2->text=dystrdup(buf);

		t=dyxNewNode();
		t->key=dystrdup("double");
		t->first=t2;

		return(t);
	}

	if(!strcmp(type, "date_t"))
	{
		s=(char *)val;
		sprintf(buf, "%04d%02d%02dT%02d:%02d:%02d",
			(s[0]<<8)+s[1], s[2], s[3], s[4], s[5], s[6]);
		t2=dyxNewNode();
		t2->text=dystrdup(buf);

		t=dyxNewNode();
		t->key=dystrdup("dateTime.iso8601");
		t->first=t2;

		return(t);
	}

	if(!strcmp(type, "data_t"))
	{
		s=(byte *)val;
		i=ObjType_GetSize(val);
		s2=gcalloc(((i*4)/3)+5);
		HttpNode_EncodeMime(s2, s, i);

		t2=dyxNewNode();
		t2->text=s2;

		t=dyxNewNode();
		t->key=dystrdup("base64");
		t->first=t2;

		return(t);
	}

	if(!strcmp(type, "array_t"))
	{
		return(XmlRpc_EncodeArray2(val, ctx));
	}

	if(!strcmp(type, "d_array_t") || !strcmp(type, "d_vector_t"))
	{
		return(XmlRpc_EncodeArray(val, ctx));
	}

	if(!strcmp(type, "d_struct_t"))
	{
		return(XmlRpc_EncodeStruct(val, ctx));
	}

	gc_printf("XmlRpc_EncodeValue: Unknown type '%s'\n", type);
	ctx->error_code=-32603;
	ctx->error_str="server error. internal xml-rpc error";
	return(NULL);
}
