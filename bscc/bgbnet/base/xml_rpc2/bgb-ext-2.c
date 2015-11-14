#include <bgbnet.h>

static int readhex(char *s, int n)
{
	int i, j;

	i=0;
	while(n)
	{
		j=0;
		if((*s>='0') && (*s<='9'))j=*s-'0';
		if((*s>='A') && (*s<='F'))j=(*s-'A')+10;
		if((*s>='a') && (*s<='f'))j=(*s-'a')+10;

		i=(i<<4)+j;

		s++;
		n++;
	}
	return(i);
}

static char *encstring(char *s)
{
	char *b, *t;

	b=gcralloc(strlen(s)*8);
	t=b;

	while(*s)
}

int XMLRPC_EXT2_ParseDate(NET_DateTime *dt, char *s)
{
	//YYYY
	i=((s[0]-'0')*1000)+((s[1]-'0')*100)+((s[2]-'0')*10)+(s[3]-'0');
	dt->year[0]=(i>>8)&0xff;
	dt->year[1]=i&0xff;
	s+=4;

	if(*s!='-')return(0);
	s++;

	//MM
	i=((s[0]-'0')*10)+(s[1]-'0');
	dt->month=i;
	s+=2;

	if(*s!='-')return(0);
	s++;

	//DD
	i=((s[0]-'0')*10)+(s[1]-'0');
	dt->day=i;
	s+=2;

	if(*s=='T')
	{
		s++;
		i=((s[0]-'0')*10)+(s[1]-'0');
		dt->hour=i;
		s+=2;

		if(*s==':')
		{
			s++;
			i=((s[0]-'0')*10)+(s[1]-'0');
			dt->minute=i;
			s+=2;
		}

		if(*s==':')
		{
			s++;
			i=((s[0]-'0')*10)+(s[1]-'0');
			dt->second=i;
			s+=2;
		}

		if(*s=='.')
		{
			s++;
			i=atoi(s);

			j=1000000;
			while((*s>='0') && (*s<='9'))
			{
				s++;
				j/=10;
			}
			i*=j;

			dt->us[0]=(i>>16)&0xff;
			dt->us[1]=(i>>8)&0xff;
			dt->us[2]=i&0xff;
		}
		if(*s=='-')
		{
			s++;
			i=((s[0]-'0')*10)+(s[1]-'0');
			dt->tz_h=-i;
			s+=2;

			s++;
			i=((s[0]-'0')*10)+(s[1]-'0');
			dt->tz_m=i;
			s+=2;
		}
		if(*s=='+')
		{
			s++;
			i=((s[0]-'0')*10)+(s[1]-'0');
			dt->tz_h=i;
			s+=2;

			s++;
			i=((s[0]-'0')*10)+(s[1]-'0');
			dt->tz_m=i;
			s+=2;
		}
	}
	return(0);
}

int XMLRPC_EXT2_PrintDate(char *s, NET_DateTime *dt)
{
	int y, us, tzh;

	y=(dt->year[0]<<8)+dt->year[1];
	s=gcrsprints(s, "%04d-%02d-%02d", y, dt->month, dy->day);
	
	s=gcrsprints(s, "T%02d:%02d:%02d",
		dt->hour, dt->minute, dt->second);

	us=(dt->us[0]<<16)+(dt->us[1]<<8)+dt->us[2];
	s=gcrsprints(s, ".%06d", us);

	if(dt->tz_h || dt->tz_m)
	{
		tzh=dt->tz_h;
		if(tzh<0)
		{
			*s++='-';
			tzh=0-tzh;
		}else *s++='+';

		s=gcrsprints(s, "%02d:%02d", tzh, dt->tz_m);
	}else *s++='Z';

	*s++=0;
	return(0);
}

dyxNode *xmlrpc_ext2_encode(XMLRPC_Context *ctx, void *val)
{
	dyxNode *t, *t2;
	char buf[16];
	byte *s, *s2;
	char *type;

	type=ObjType_GetTypeName(val);

	if(!strcmp(type, "null_t"))
	{
		t=dyxNewNode();
		t->key=dystrdup("null");

		return(t);
	}

	if(!strcmp(type, "string_t"))
	{
		t2=dyxNewNode();
		t2->text=dystrdup((char *)val);

		t=dyxNewNode();
		t->key=dystrdup("symbol");
		t->first=t2;

		return(t);
	}

	if(!strcmp(type, "link_t"))
	{
		t2=dyxNewNode();
		t2->text=dystrdup((char *)val);

		t=dyxNewNode();
		t->key=dystrdup("link");
		t->first=t2;

		return(t);
	}

	if(!strcmp(type, "d_vector_t"))
	{
		return(XmlRpc_EncodeVector(val, ctx));
	}

	return(NULL);
}

void *xmlrpc_ext2_decode(XMLRPC_Context *ctx, dyxNode *exp)
{
	void *p;
	char *s, *t;
	int i, j;

	if(!strcmp(exp->key, "null"))
	{
		p=gctalloc("null_t", 0);
		return(p);
	}

	if(!strcmp(exp->key, "xstring"))
	{
		p=gctalloc("string_t", strlen(exp->first->text)+1);
		s=p;
		while(*s)
		{
			if(*s=='\\')
			{
				s++;
				switch(*s)
				{
				case 'b':
					s++;
					*t++='\b';
					break;
				case 'f':
					s++;
					*t++='\f';
					break;
				case 'n':
					s++;
					*t++='\n';
					break;
				case 'r':
					s++;
					*t++='\r';
					break;
				case '\\':
					s++;
					*t++='\\';
					break;
				case 'x':
					s++;
					i=readhex(s, 2);
					s+=2;
					*t++=i;
					break;
				case 'u':
					s++;
					i=readhex(s, 4);
					s+=4;
					if(i>127)i='?';
					*t++=i;
					break;
				case 'U':
					s++;
					i=readhex(s, 8);
					s+=8;
					if(i>127)i='?';
					*t++=i;
					break;
				default:
					s++;
					break;
				}
				continue;
			}
			*t++=*s++;
		}
		return(p);
	}

	if(!strcmp(exp->key, "absdate"))
	{
		p=gctalloc("date_t", 0);
		s=exp->first->text;

		XMLRPC_EXT2_ParseDate(p);

		return(p);
	}

	return(NULL);
}

int XMLRPC_EXT2_Init()
{
	XMLRPC_Extension *ext;

	ext=XmlRpc_CreateExtension(
		"http://bgb-sys.sourceforge.net/bgb-ext/types2");
	ext->encode=&xmlrpc_ext2_encode;
	ext->decode=&xmlrpc_ext2_decode;

	return(0);
}
