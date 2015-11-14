#include <bgbnet.h>

NetParse_Node *XmlRpc_EncodeArray(long val)
{
	NetParse_Node *t, *t2, *lst;
	NET_Array *ns;
	int i;

	ns=(NET_Array *)val;

	lst=NULL;
	for(i=0; ns->types[i]; i++)
	{
		t2=XmlRpc_EncodeValue(ns->types[i], ns->values[i]);
		t=NetParse_NewNode();
		t->key=kstrdup("value");
		t->first=t2;
		lst=NetParse_AddNodeEnd(lst, t);
	}
	t2=NetParse_NewNode();
	t2->key=kstrdup("data");
	t2->first=lst;

	t=NetParse_NewNode();
	t->key=kstrdup("array");
	t->first=t2;

	return(t);
}

NetParse_Node *XmlRpc_EncodeStruct(long val)
{
	NetParse_Node *t, *t2, *t3, *lst;
	NET_Struct *ns;
	int i;

	ns=(NET_Struct *)val;

	lst=NULL;
	for(i=0; ns->types[i]; i++)
	{

		t3=XmlRpc_EncodeValue(ns->types[i], ns->values[i]);
		t2=NetParse_NewNode();
		t2->key=kstrdup("value");
		t2->first=t3;

		t3=NetParse_NewNode();
		t3->text=kstrdup(ns->names[i]);
		t=NetParse_NewNode();
		t->key=kstrdup("name");
		t->first=t3;
		t->next=t2;

		t2=t;
		t=NetParse_NewNode();
		t->key=kstrdup("member");
		t->first=t2;

		lst=NetParse_AddNodeEnd(lst, t);
	}
	t=NetParse_NewNode();
	t->key=kstrdup("struct");
	t->first=lst;

	return(t);
}

NetParse_Node *XmlRpc_EncodeValue(char *type, long val)
{
	NetParse_Node *t, *t2;
	char buf[16];
	byte *s, *s2;
	int i;

	if(!strcmp(type, "int"))
	{
		sprintf(buf, "%d", val);
		t2=NetParse_NewNode();
		t2->text=kstrdup(buf);

		t=NetParse_NewNode();
		t->key=kstrdup("i4");
		t->first=t2;

		return(t);
	}

	if(!strcmp(type, "bool"))
	{
		sprintf(buf, "%d", val);
		t2=NetParse_NewNode();
		t2->text=kstrdup(buf);

		t=NetParse_NewNode();
		t->key=kstrdup("bool");
		t->first=t2;

		return(t);
	}

	if(!strcmp(type, "string"))
	{
		sprintf(buf, "%d", val);
		t2=NetParse_NewNode();
		t2->text=kstrdup((char *)val);

		t=NetParse_NewNode();
		t->key=kstrdup("string");
		t->first=t2;

		return(t);
	}

	if(!strcmp(type, "float"))
	{
		sprintf(buf, "%f", *(float *)(&val));
		t2=NetParse_NewNode();
		t2->text=kstrdup(buf);

		t=NetParse_NewNode();
		t->key=kstrdup("double");
		t->first=t2;

		return(t);
	}

	if(!strcmp(type, "date"))
	{
		s=(char *)val;
		sprintf(buf, "%04d%02d%02dT%02d:%02d:%02d",
			(s[0]<<8)+s[1], s[2], s[3], s[4], s[5], s[6]);
		t2=NetParse_NewNode();
		t2->text=kstrdup(buf);

		t=NetParse_NewNode();
		t->key=kstrdup("dateTime.iso8601");
		t->first=t2;

		return(t);
	}

	if(!strcmp(type, "data"))
	{
		s=(byte *)(val+sizeof(long));
		i=*(long **)val;
		s2=kalloc(((i*4)/3)+5);
		HttpNode_EncodeMime(s2, s, i);

		t2=NetParse_NewNode();
		t2->text=s2;

		t=NetParse_NewNode();
		t->key=kstrdup("base64");
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

	kprint("XmlRpc_EncodeValue: Unknown type '%s'\n", type);
	return(NULL);
}
