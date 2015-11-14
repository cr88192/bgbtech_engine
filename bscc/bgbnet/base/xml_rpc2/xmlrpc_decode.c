#include <bgbnet.h>

/*--
Cat pdnet;RPC;XML-RPC;Decode
Form
	void *XmlRpc_DecodeMember(dyxNode *exp, char **name, XMLRPC_Context *ctx);
Description
	Decode an XML-RPC Struct Member.
Status Internal
--*/
void *XmlRpc_DecodeMember(dyxNode *exp, char **name, XMLRPC_Context *ctx)
{
	dyxNode *n;
	void *p;

	n=dyxFindKey(exp->first, "name");
	*name=dystrdup(n->first->text);

	n=dyxFindKey(exp->first, "value");
	p=XmlRpc_DecodeValue(n, ctx);

	return(p);
}

/*--
Cat pdnet;RPC;XML-RPC;Decode
Form
	void *XmlRpc_DecodeStruct(dyxNode *exp, XMLRPC_Context *ctx);
Description
	Decode an XML-RPC Struct.
Status Internal
--*/
void *XmlRpc_DecodeStruct(dyxNode *exp, XMLRPC_Context *ctx)
{
	NET_Struct *ns;
	dyxNode *cur;
	char **names;
	void **values;
	int i;

	names=gcalloc(1024*sizeof(char *));
	values=gcalloc(1024*sizeof(void *));
	i=0;

	cur=exp->first;
	while(cur)
	{
		if(!strcmp(cur->key, "member"))
		{
			values[i]=XmlRpc_DecodeMember(cur, &names[i], ctx);
			i++;
		}
		cur=cur->next;
	}
	names[i]=NULL;
	values[i]=NULL;

//	ns=gcalloc(sizeof(NET_Struct));
	ns=gctalloc("d_struct_t", 0);

//	ns->names=gcalloc((i+1)*sizeof(char *));
//	ns->values=gcalloc((i+1)*sizeof(void *));

	ns->names=gctalloc("array_t", (i+1)*sizeof(char *));
	ns->values=gctalloc("array_t", (i+1)*sizeof(void *));

	memcpy(ns->names, names, (i+1)*sizeof(char *));
	memcpy(ns->values, values, (i+1)*sizeof(void *));

	return(ns);
}

/*--
Cat pdnet;RPC;XML-RPC;Decode
Form
	void *XmlRpc_DecodeArray2(dyxNode *exp, XMLRPC_Context *ctx);
Description
	Decodes the internal portion of an XML-RPC Array.
Status Internal
--*/
void *XmlRpc_DecodeArray2(dyxNode *exp, XMLRPC_Context *ctx)
{
	dyxNode *cur;
	void **values;
	void *p;
	int i;

	values=gcalloc(1024*sizeof(void *));
	i=0;

	cur=exp->first;

	if(cur->key)if(!strcmp(cur->key, "data"))cur=cur->first;

	while(cur)
	{
		values[i]=XmlRpc_DecodeValue(cur->first, ctx);
		i++;
		cur=cur->next;
	}
	values[i]=NULL;

	p=gctalloc("array_t", (i+1)*sizeof(void *));

	memcpy(p, values, (i+1)*sizeof(void *));
	gcfree(values);

	return(p);
}

/*--
Cat pdnet;RPC;XML-RPC;Decode
Form
	void *XmlRpc_DecodeArray(dyxNode *exp, XMLRPC_Context *ctx);
Description
	Decode an XML-RPC Array.
Status Internal
--*/
void *XmlRpc_DecodeArray(dyxNode *exp, XMLRPC_Context *ctx)
{
	NET_Array *ns;
	dyxNode *cur;
	void **values;
	int i;

	ns=gctalloc("d_array_t", 0);

	ns->values=XmlRpc_DecodeArray2(exp, ctx);

//	memcpy(ns->values, values, (i+1)*sizeof(void *));

	return(ns);
}

/*--
Cat pdnet;RPC;XML-RPC;Decode
Form
	void *XmlRpc_DecodeVector(dyxNode *exp, XMLRPC_Context *ctx);
Description
	Decode a BGB-RPC Vector.
Status Internal
--*/
void *XmlRpc_DecodeVector(dyxNode *exp, XMLRPC_Context *ctx)
{
	NET_Array *ns;
	dyxNode *cur;
	void **values;
	int i;

	ns=gctalloc("d_vector_t", 0);

	ns->values=XmlRpc_DecodeArray2(exp, ctx);

//	memcpy(ns->values, values, (i+1)*sizeof(void *));

	return(ns);
}

/*--
Cat pdnet;RPC;XML-RPC;Decode
Form
	void *XmlRpc_DecodeValue(dyxNode *exp, XMLRPC_Context *ctx);
Description
	Decode an XML-RPC Value.
Status Internal
--*/
void *XmlRpc_DecodeValue(dyxNode *exp, XMLRPC_Context *ctx)
{
	char *s, *t, *b;
	int i, j;
	void *p;

	if(exp->text)
	{
		s=gctalloc("string_t", strlen(exp->text)+1);
		strcpy(s, exp->text);
		return(s);
	}

	if(!strcmp(exp->key, "value"))
	{
		s=dyxGet(exp, "xmlns");
		if(s)
		{
			p=XmlRpc_DecodeExtension(ctx, s, exp->first);
			return(p);
		}
		return(XmlRpc_DecodeValue(exp->first, ctx));
	}

	if(!strcmp(exp->key, "param"))
		return(XmlRpc_DecodeValue(exp->first, ctx));

	if(!strcmp(exp->key, "int"))
	{
		p=gctalloc("int_t", 0);
		*(int *)p=atoi(exp->first->text);
		return(p);
	}
	if(!strcmp(exp->key, "i4"))
	{
		p=gctalloc("int_t", 0);
		*(int *)p=atoi(exp->first->text);
		return(p);
	}

	if(!strcmp(exp->key, "bool"))
	{
		p=gctalloc("bool_t", 0);
		*(int *)p=atoi(exp->first->text);
		return(p);
	}

	if(!strcmp(exp->key, "boolean"))
	{
		p=gctalloc("bool_t", 0);
		*(int *)p=atoi(exp->first->text);
		return(p);
	}

	if(!strcmp(exp->key, "string"))
	{
		s=gctalloc("string_t", strlen(exp->first->text)+1);
		strcpy(s, exp->first->text);
		return(s);
	}

	if(!strcmp(exp->key, "double"))
	{
		p=gctalloc("float_t", 0);
		*(double *)p=atof(exp->first->text);
		return(p);
	}

	if(!strcmp(exp->key, "dateTime.iso8601"))
	{
//		b=gcalloc(8);
		b=gctalloc("date_t", 0);
		s=exp->first->text;
		t=b;

		i=((s[0]-'0')*1000)+((s[1]-'0')*100)+((s[2]-'0')*10)+(s[3]-'0');
		*t++=i>>8;
		*t++=i&0xff;
		s+=4;

		i=2;
		while(i--)
		{
			*t++=((s[0]-'0')*10)+(s[1]-'0');
			s+=2;
		}
		i=3;
		while(i--)
		{
			s++;
			*t++=((s[0]-'0')*10)+(s[1]-'0');
			s+=2;
		}
		return(b);
	}

	if(!strcmp(exp->key, "base64"))
	{
		s=exp->first->text;
		i=strlen(s);
		j=(i*3)/4;
//		b=gcalloc(j+sizeof(long));
		b=gctalloc("data_t", j);
//		*(long *)b=j;
//		t=b+sizeof(long);
		t=b;

		gc_printf("recv mime %d->%d\n", i, j);

		HttpNode_DecodeMime(t, s, i);

		return(b);
	}

	if(!strcmp(exp->key, "array"))
	{
		p=XmlRpc_DecodeArray(exp, ctx);
		return(p);
	}

	if(!strcmp(exp->key, "struct"))
	{
		return(XmlRpc_DecodeStruct(exp, ctx));
	}

	gc_printf("XML-RPC: Decode: Unknown type tag '%s'\n", exp->key);
	ctx->error_code=-32003;
	ctx->error_str="server error: unknown type tag";

	return(NULL);
}
