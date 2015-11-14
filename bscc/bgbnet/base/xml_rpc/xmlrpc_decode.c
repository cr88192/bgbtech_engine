#include <bgbnet.h>

int XmlRpc_DecodeMember(dyxNode *exp, char **type, char **name, long *value)
{
	dyxNode *n;

	n=dyxFindKey(exp->first, "name");
	*name=dystrdup(n->first->text);

	n=dyxFindKey(exp->first, "value");
	XmlRpc_DecodeValue(n, type, value);

	return(0);
}

int XmlRpc_DecodeStruct(dyxNode *exp, char **type, long *value)
{
	NET_Struct *ns;
	dyxNode *cur;
	char **types;
	char **names;
	long *values;
	int i;

	types=gcalloc(1024*sizeof(char *));
	names=gcalloc(1024*sizeof(char *));
	values=gcalloc(1024*sizeof(long));
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

	ns=gcalloc(sizeof(NET_Struct));
	ns->types=gcalloc((i+1)*sizeof(char *));
	ns->names=gcalloc((i+1)*sizeof(char *));
	ns->values=gcalloc((i+1)*sizeof(long));

	memcpy(ns->types, types, (i+1)*sizeof(char *));
	memcpy(ns->names, names, (i+1)*sizeof(char *));
	memcpy(ns->values, values, (i+1)*sizeof(long));

	*type=dystrdup("struct");
	*value=ns;

	return(0);
}

int XmlRpc_DecodeArray(dyxNode *exp, char **type, long *value)
{
	NET_Array *ns;
	dyxNode *cur;
	char **types;
	long *values;
	int i;

	types=gcalloc(1024*sizeof(char *));
	values=gcalloc(1024*sizeof(long));
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

	ns=gcalloc(sizeof(NET_Array));
	ns->types=gcalloc((i+1)*sizeof(char *));
	ns->values=gcalloc((i+1)*sizeof(long));

	memcpy(ns->types, types, (i+1)*sizeof(char *));
	memcpy(ns->values, values, (i+1)*sizeof(long));

	*type=dystrdup("array");
	*value=ns;

	return(0);
}

int XmlRpc_DecodeValue(dyxNode *exp, char **type, long *value)
{
	char *s, *t, *b;
	int i, j;

	if(exp->text)
	{
		*type=dystrdup("string");
		*value=dystrdup(exp->text);
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
		*type=dystrdup("int");
		*value=atoi(exp->first->text);
		return(0);
	}
	if(!strcmp(exp->key, "i4"))
	{
		*type=dystrdup("int");
		*value=atoi(exp->first->text);
		return(0);
	}

	if(!strcmp(exp->key, "bool"))
	{
		*type=dystrdup("bool");
		*value=atoi(exp->first->text);
		return(0);
	}

	if(!strcmp(exp->key, "string"))
	{
		*type=dystrdup("string");
		*value=dystrdup(exp->first->text);
		return(0);
	}

	if(!strcmp(exp->key, "double"))
	{
		*type=dystrdup("float");
		*(float *)value=atof(exp->first->text);
		return(0);
	}

	if(!strcmp(exp->key, "dateTime.iso8601"))
	{
		*type=dystrdup("date");

		b=gcalloc(8);
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
		*type=dystrdup("data");

		s=exp->first->text;
		i=strlen(s);
		j=(i*3)/4;
		b=gcalloc(j+sizeof(long));
		*(long *)b=j;
		t=b+sizeof(long);

		gc_printf("recv mime %d->%d\n", i, j);

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

	*type=dystrdup("unknown");
	return(-1);
}
