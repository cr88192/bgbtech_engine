#include <bgbnet.h>

int XmlRpc_DecodeMember(NetParse_Node *exp, char **type, char **name, long *value)
{
	NetParse_Node *n;

	n=NetParse_FindKey(exp->first, "name");
	*name=kstrdup(n->first->text);

	n=NetParse_FindKey(exp->first, "value");
	XmlRpc_DecodeValue(n, type, value);

	return(0);
}

int XmlRpc_DecodeStruct(NetParse_Node *exp, char **type, long *value)
{
	NET_Struct *ns;
	NetParse_Node *cur;
	char **types;
	char **names;
	long *values;
	int i;

	types=kalloc(1024*sizeof(char *));
	names=kalloc(1024*sizeof(char *));
	values=kalloc(1024*sizeof(long));
	i=0;

	cur=exp->first;
	while(cur)
	{
		if(!strcmp(cur->key, "member"))
		{
			XmlRpc_DecodeMember(cur, &types[i], &names[i], &values[i]);
			i++;
		}
		cur=cur->next;
	}
	types[i]=NULL;

	ns=kalloc(sizeof(NET_Struct));
	ns->types=kalloc((i+1)*sizeof(char *));
	ns->names=kalloc((i+1)*sizeof(char *));
	ns->values=kalloc((i+1)*sizeof(long));

	memcpy(ns->types, types, (i+1)*sizeof(char *));
	memcpy(ns->names, names, (i+1)*sizeof(char *));
	memcpy(ns->values, values, (i+1)*sizeof(long));

	*type=kstrdup("struct");
	*value=ns;

	return(0);
}

int XmlRpc_DecodeArray(NetParse_Node *exp, char **type, long *value)
{
	NET_Array *ns;
	NetParse_Node *cur;
	char **types;
	long *values;
	int i;

	types=kalloc(1024*sizeof(char *));
	values=kalloc(1024*sizeof(long));
	i=0;

	cur=exp->first;

	if(cur->key)if(!strcmp(cur->key, "data"))cur=cur->first;

	while(cur)
	{
		XmlRpc_DecodeValue(cur->first, &types[i], &values[i]);
		i++;
		cur=cur->next;
	}
	types[i]=NULL;

	ns=kalloc(sizeof(NET_Array));
	ns->types=kalloc((i+1)*sizeof(char *));
	ns->values=kalloc((i+1)*sizeof(long));

	memcpy(ns->types, types, (i+1)*sizeof(char *));
	memcpy(ns->values, values, (i+1)*sizeof(long));

	*type=kstrdup("array");
	*value=ns;

	return(0);
}

int XmlRpc_DecodeValue(NetParse_Node *exp, char **type, long *value)
{
	char *s, *t, *b;
	int i, j;

	if(exp->text)
	{
		*type=kstrdup("string");
		*value=kstrdup(exp->text);
		return(0);
	}

	if(!strcmp(exp->key, "value"))
	{
		XmlRpc_DecodeValue(exp->first, type, value);
		return(0);
	}

	if(!strcmp(exp->key, "param"))
	{
		XmlRpc_DecodeValue(exp->first, type, value);
		return(0);
	}


	if(!strcmp(exp->key, "int"))
	{
		*type=kstrdup("int");
		*value=atoi(exp->first->text);
		return(0);
	}
	if(!strcmp(exp->key, "i4"))
	{
		*type=kstrdup("int");
		*value=atoi(exp->first->text);
		return(0);
	}

	if(!strcmp(exp->key, "bool"))
	{
		*type=kstrdup("bool");
		*value=atoi(exp->first->text);
		return(0);
	}

	if(!strcmp(exp->key, "string"))
	{
		*type=kstrdup("string");
		*value=kstrdup(exp->first->text);
		return(0);
	}

	if(!strcmp(exp->key, "double"))
	{
		*type=kstrdup("float");
		*(float *)value=atof(exp->first->text);
		return(0);
	}

	if(!strcmp(exp->key, "dateTime.iso8601"))
	{
		*type=kstrdup("date");

		b=kalloc(8);
		s=exp->first->text;
		t=b;

		i=((s[0]-'0')*1000)+((s[1]-'0')*100)+((s[2]-'0')*10)+(s[3]-'0');
		*t++=i>>8;
		*t++=i&0xff;
		s+=2;

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
		*value=b;
		return(0);
	}

	if(!strcmp(exp->key, "base64"))
	{
		*type=kstrdup("data");

		s=exp->first->text;
		i=strlen(s);
		j=(i*3)/4;
		b=kalloc(j+sizeof(long));
		*(long *)b=j;
		t=b+sizeof(long);

		kprint("recv mime %d->%d\n", i, j);

		HttpNode_DecodeMime(t, s, i);

		*value=b;

		return(0);
	}

	if(!strcmp(exp->key, "array"))
	{
		XmlRpc_DecodeArray(exp, type, value);
		return(0);
	}

	if(!strcmp(exp->key, "struct"))
	{
		XmlRpc_DecodeStruct(exp, type, value);
		return(0);
	}

	*type=kstrdup("unknown");
	return(-1);
}
