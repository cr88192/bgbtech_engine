#include <bgbnet.h>

dyxNode *XmlRpc_EncodeArray(long val)
{
	dyxNode *t, *t2, *lst;
	NET_Array *ns;
	int i;

	ns=(NET_Array *)val;

	lst=NULL;
	for(i=0; ns->types[i]; i++)
	{
		t2=XmlRpc_EncodeValue(ns->types[i], ns->values[i]);
		t=dyxNewNode();
		t->key=dystrdup("value");
		t->first=t2;
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

dyxNode *XmlRpc_EncodeStruct(long val)
{
	dyxNode *t, *t2, *t3, *lst;
	NET_Struct *ns;
	int i;

	ns=(NET_Struct *)val;

	lst=NULL;
	for(i=0; ns->types[i]; i++)
	{

		t3=XmlRpc_EncodeValue(ns->types[i], ns->values[i]);
		t2=dyxNewNode();
		t2->key=dystrdup("value");
		t2->first=t3;

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

dyxNode *XmlRpc_EncodeValue(char *type, long val)
{
	dyxNode *t, *t2;
	char buf[16];
	byte *s, *s2;
	int i;

	if(!strcmp(type, "int"))
	{
		sprintf(buf, "%d", val);
		t2=dyxNewNode();
		t2->text=dystrdup(buf);

		t=dyxNewNode();
		t->key=dystrdup("i4");
		t->first=t2;

		return(t);
	}

	if(!strcmp(type, "bool"))
	{
		sprintf(buf, "%d", val);
		t2=dyxNewNode();
		t2->text=dystrdup(buf);

		t=dyxNewNode();
		t->key=dystrdup("bool");
		t->first=t2;

		return(t);
	}

	if(!strcmp(type, "string"))
	{
		sprintf(buf, "%d", val);
		t2=dyxNewNode();
		t2->text=dystrdup((char *)val);

		t=dyxNewNode();
		t->key=dystrdup("string");
		t->first=t2;

		return(t);
	}

	if(!strcmp(type, "float"))
	{
		sprintf(buf, "%f", *(float *)(&val));
		t2=dyxNewNode();
		t2->text=dystrdup(buf);

		t=dyxNewNode();
		t->key=dystrdup("double");
		t->first=t2;

		return(t);
	}

	if(!strcmp(type, "date"))
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

	if(!strcmp(type, "data"))
	{
		s=(byte *)(val+sizeof(long));
		i=*(long **)val;
		s2=gcalloc(((i*4)/3)+5);
		HttpNode_EncodeMime(s2, s, i);

		t2=dyxNewNode();
		t2->text=s2;

		t=dyxNewNode();
		t->key=dystrdup("base64");
		t->first=t2;

		return(t);
	}

	if(!strcmp(type, "array"))
	{
		return(XmlRpc_EncodeArray(val));
	}

	if(!strcmp(type, "struct"))
	{
		return(XmlRpc_EncodeStruct(val));
	}

	gc_printf("XmlRpc_EncodeValue: Unknown type '%s'\n", type);
	return(NULL);
}
