#include <bgbnet.h>

#if defined(WIN32)
#include <windows.h>
#include <winsock.h>
#else
#include <netinet/in.h>
#endif

Meta0_PortInfo *httpnode_cl_inf;
NET_Interface *httpnode_iface;

NET_Reference *httpnode_decode_url(char *s)
{
	NET_Reference *ref;
	HTTP_RefInfo *ri;
	char *t;
	char buf[1024];
	VADDR *addr;

//	ref=kalloc(sizeof(NET_Reference));
	ref=gctalloc("netref_t", 0);

	ri=kalloc(sizeof(HTTP_RefInfo));
	ref->refinfo=ri;

	while(*s=='/')s++;
	t=buf;
	while(*s!='/')*t++=*s++;
	*t++=0;

	addr=NET_DecodeHostname(buf);
	if(!addr)
	{
		kprintd("HTTP: Unknown host %s\n", buf);
		kfree(ri);
		kfree(ref);
		return(NULL);
	}
	switch(addr->proto)
	{
	case PROTO_IPV4:
	case PROTO_IPV4UDP:
	case PROTO_IPV4TCP:
		if(!addr->ipv4.port)
			addr->ipv4.port=htons(80);
		break;
	case PROTO_IPV6:
	case PROTO_IPV6UDP:
	case PROTO_IPV6TCP:
		if(!addr->ipv6.port)
			addr->ipv6.port=htons(80);
		break;
	default:
		break;
	}

	ri->addr=addr;
	ri->host=kstrdup(buf);

	t=buf;
	while(*s)*t++=*s++;
	*t++=0;
	ri->req=kstrdup(buf);

	return(ref);
//	return(NULL);
}

char *httpnode_encode_url(NET_Reference *ref)
{
	HTTP_RefInfo *ri;
	char *s;
	char buf[1024];

	ri=ref->refinfo;
//	s=NET_Addr2Str(ri->addr);
	sprintf(buf, "http://%s%s", ri->host, ri->req);
	s=kstrdup(buf);

	return(s);
//	return(NULL);
}

void *httpnode_rpc_call(NET_Reference *ref, void **args)
{
	return(NULL);
}

void *httpnode_fetch(NET_Reference *ref)
{
	Meta0_Con *con;
	HTTP_RefInfo *ri;
	int i;

	ri=ref->refinfo;
	kprintd("HTTP: Getting http://%s%s\n", ri->host, ri->req);

	con=Meta0_Connect(httpnode_cl_inf, ri->addr);
	con->data=ri;

	if(!con)
	{
		kprintd("HTTP: Connection failed.\n");
		return(NULL);
	}

	kprintvf(con->sock, "GET %s HTTP/1.0\r\n", ri->req);
	kprintvf(con->sock, "Accept: */*\r\n");
	kprintvf(con->sock, "Accept-Language: en-us\r\n");
	kprintvf(con->sock, "User-Agent: %s/%s\r\n", PROJNAME, PROJVER);
	kprintvf(con->sock, "Host: %s\r\n", ri->host);
	kprintvf(con->sock, "Connection: Close\r\n");
	kprintvf(con->sock, "\r\n");

	ri->ok=0;
	ri->st=0;
	i=5000;
	while(!ri->ok && i)
	{
		NET_Poll();
//		usleep(1000);
		i--;
	}

	if(ri->ok)
		return(ri->rbuf);

	if(ri->rbuf)kfree(ri->rbuf);
	ri->rbuf=NULL;
	ri->rlen=0;

	return(NULL);
}

int httpnode_input_stat(Meta0_Con *con, char *buf)
{
	char tokb[8][64];
	char *s, *t;
	int i;
	HTTP_RefInfo *ri;

	kprintd("http-cl: status line '%s'\n", buf);

	ri=con->data;

	s=buf;
	i=0;
	while(*s)
	{
		while(*s && (*s<=' '))s++;
		t=tokb[i++];
		while(*s>' ')*t++=*s++;
		*t++=0;
	}

	if(strncmp(tokb[0], "HTTP", 4))
	{
		kprintd("HTTP: Non-HTTP Status line\n");
		return(-1);
	}
	ri->st=atoi(tokb[1]);
	if((ri->st/100)!=2)
	{
		kprintd("HTTP: Non-Good Status code %d\n", ri->st);
		return(-1);
	}

	return(0);
}

int httpnode_input_line(Meta0_Con *con, char *buf)
{
	char tokb[8][64];
	char *s, *t;
	int i;
	HTTP_RefInfo *ri;

	kprintd("http-cl: input line '%s'\n", buf);

	ri=con->data;

	s=buf;
	i=0;
	while(*s)
	{
		while(*s && (*s<=' '))s++;
		t=tokb[i++];
		while(*s>' ')*t++=*s++;
		*t++=0;
	}

	if(!strcmp(tokb[0], "Content-Length:"))
		ri->clen=atoi(tokb[1]);

	return(0);
}

int httpnode_input(Meta0_Con *con)
{
	char buf[256], *s, *t, *t2;
	int i;
	HTTP_RefInfo *ri;

	s=con->buf;
	t=s;

	ri=con->data;

	while(1)
	{
		if(ri->rlen)
		{
			kprintd("http-cl: data\n");
			i=(char *)con->end-t;
			if(!i)return(t-s);

			if(ri->rlen<i)i=ri->rlen;
			memcpy(ri->erbuf, t, i);
			ri->erbuf+=i;
			ri->rlen-=i;
			t+=i;

			if(!ri->rlen)
			{
				ri->ok=1;
				return(-1);
			}

			continue;
		}

		t2=ksgets(buf, 255, t);
		if(t2==t)break;
		t=t2;

		if(!ri->st)
		{
			kprintd("http-cl: stat\n");
			i=httpnode_input_stat(con, buf);
			if(i==-1)return(-1);
			continue;
		}

		if(!buf[0])
		{
			kprintd("http-cl: body\n");

			if(!ri->clen)
			{
				kprintd("http-cl: no-data\n");
				ri->rbuf=NULL;
				ri->ok=1;
				return(-1);
			}
			ri->rbuf=kalloc(ri->clen);
			ri->erbuf=ri->rbuf;
			ri->rlen=ri->clen;

			continue;
		}

		i=httpnode_input_line(con, buf);
		if(i==-1)return(-1);
	}

	return(t-s);
}

/*--
Cat pdnet;Protocols;HTTP
Form
	NET_Reference *HttpNode_DecodeURL(char *s);
Description
	Manually decodes an HTTP URL.
--*/
NET_Reference *HttpNode_DecodeURL(char *s)
{
	if(*s!='/')
	{
		while(*s && (*s!=':'))s++;
		if(*s==':')s++;
	}
	return(httpnode_decode_url(s));
}

/*--
Cat pdnet;Protocols;HTTP
Form
	void *HttpNode_Post(NET_Reference *ref, char *type, void *msg, int len);
Description
	Executes an HTTP POST request and returns the response data.
--*/
void *HttpNode_Post(NET_Reference *ref, char *type, void *msg, int len)
{
	int i;
	Meta0_Con *con;
	HTTP_RefInfo *ri;

	ri=ref->refinfo;
	kprintd("HTTP: Posting to http://%s%s\n", ri->host, ri->req);

	con=Meta0_Connect(httpnode_cl_inf, ri->addr);
	con->data=ri;

	if(!con)
	{
		kprintd("HTTP: Connection failed.\n");
		return(-1);
	}

	kprintvf(con->sock, "POST %s HTTP/1.0\r\n", ri->req);
	kprintvf(con->sock, "Accept: */*\r\n");
	kprintvf(con->sock, "Accept-Language: en-us\r\n");
	kprintvf(con->sock, "User-Agent: %s/%s\r\n", PROJNAME, PROJVER);
	kprintvf(con->sock, "Host: %s\r\n", ri->host);
	kprintvf(con->sock, "MIME-Version: 1.0\r\n");
	kprintvf(con->sock, "Content-Type: %s\r\n", type);
	kprintvf(con->sock, "Content-Length: %d\r\n", len);
	kprintvf(con->sock, "Connection: Close\r\n");
	kprintvf(con->sock, "\r\n");

	kprintd("HTTP: Post, content: %s, %d bytes\n", type, len);

	vfwrite(msg, 1, len, con->sock);

	ri->ok=0;
	ri->st=0;
	i=5000;
	while(!ri->ok && i)
	{
		NET_Poll();
//		usleep(1000);
		i--;
	}

	if(ri->ok)
		return(ri->rbuf);

	if(ri->rbuf)kfree(ri->rbuf);
	ri->rbuf=NULL;
	ri->rlen=0;

	return(NULL);
}

int HttpNode_InitCl()
{
	NET_Interface *iface;

//	iface=NET_CreateInterface("http");
	iface=httpnode_iface;

	iface->decode_url=&httpnode_decode_url;
	iface->encode_url=&httpnode_encode_url;
	iface->rpc_call=&httpnode_rpc_call;
	iface->fetch=&httpnode_fetch;

	httpnode_cl_inf=Meta0_CreateClientPort("http-client");
	httpnode_cl_inf->input=&httpnode_input;

	return(0);
}
