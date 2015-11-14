#include <bgbnet.h>

VFILE *httpnode_sock=NULL;
int httpnode_port=-1;

HTTP_Con *httpnode_cons[256];
HTTP_Resource *httpnode_rsrc;
NET_Interface *httpnode_iface;

char *httpnode_basedir=".";

/*--
Cat pdnet;Protocols;HTTP
Form
	HTTP_Con *HttpNode_NewCon();
Description
	Creates a new HTTP connection structure.
Status Internal
--*/
HTTP_Con *HttpNode_NewCon()
{
	HTTP_Con *con;
	int i;

	for(i=0; i<256; i++)if(!httpnode_cons[i])
	{
		con=kalloc(sizeof(HTTP_Con));
		httpnode_cons[i]=con;
		con->num=i;
		return(con);
	}
	return(NULL);
}

/*--
Cat pdnet;Protocols;HTTP
Form
	int HttpNode_SendContents(HTTP_Con *con, byte *type, byte *msg, int len);
Description
	Sends contents as the result of a HTTP request.
Status Internal
--*/
int HttpNode_SendContents(HTTP_Con *con, byte *type, byte *msg, int len)
{
	int sz, i;
	char *obuf, *s, *t;
	char *tbuf[16];

	kprint("send response, content: %s, %d bytes\n", type, len);

	obuf=kalloc(1024);
	s=obuf;
	s=kprints(s, "HTTP/1.0 200 OK\r\n");
	s=kprints(s, "Server: %s/%s\r\n", PROJNAME, PROJVER);
	s=kprints(s, "MIME-Version: 1.0\r\n");
	s=kprints(s, "Content-Type: %s\r\n", type);
	s=kprints(s, "Content-Length: %d\r\n", len);
	s=kprints(s, "\r\n");

	vfwrite(obuf, 1, s-obuf, con->sock);
	vfwrite(msg, 1, len, con->sock);

	return(0);
}

int HttpNode_SendResponse(HTTP_Con *con, byte *msg, int len)
{
	HttpNode_SendContents(con, "text/xml", msg, len);

	return(0);
}

/*--
Cat pdnet;Protocols;HTTP
Form
	char *HttpNode_CodeName(int code);
Description
	Retrieves the string name of a given HTTP error code.
--*/
char *HttpNode_CodeName(int code)
{
	char *s;

	switch(code)
	{
	case 100:
		s="Continue";
		break;
	case 101:
		s="Switching Protocols";
		break;

	case 200:
		s="OK";
		break;
	case 201:
		s="Created";
		break;
	case 202:
		s="Accepted";
		break;
	case 203:
		s="Non-Authoritative Information";
		break;
	case 204:
		s="No Content";
		break;
	case 205:
		s="Reset Content";
		break;
	case 206:
		s="Partial Content";
		break;

	case 300:
		s="Multiple Choices";
		break;
	case 301:
		s="Moved Permanently";
		break;
	case 302:
		s="Moved Temporarily";
		break;
	case 303:
		s="See Other";
		break;
	case 304:
		s="Not Modified";
		break;
	case 305:
		s="Use Proxy";
		break;

	case 400:
		s="Bad Request";
		break;
	case 401:
		s="Unauthorized";
		break;
	case 402:
		s="Payment Required";
		break;
	case 403:
		s="Forbidden";
		break;
	case 404:
		s="Not Found";
		break;
	case 405:
		s="Method Not Allowed";
		break;
	case 406:
		s="Not Acceptable";
		break;
	case 407:
		s="Proxy Authentication Required";
		break;
	case 408:
		s="Request Time-out";
		break;
	case 409:
		s="Conflict";
		break;
	case 410:
		s="Gone";
		break;
	case 411:
		s="Length Required";
		break;
	case 412:
		s="Precondition Failed";
		break;
	case 413:
		s="Request Entity Too Large";
		break;
	case 414:
		s="Request-URI Too Large";
		break;
	case 415:
		s="Unsupported Media Type";
		break;

	case 500:
		s="Internal Server Error";
		break;
	case 501:
		s="Not Implemented";
		break;
	case 502:
		s="Bad Gateway";
		break;
	case 503:
		s="Service Unavailable";
		break;
	case 504:
		s="Gateway Time-out";
		break;
	case 505:
		s="HTTP Version not supported";
		break;
	default:
		s="Unknown-Code";
		break;
	}
	return(s);
}

/*--
Cat pdnet;Protocols;HTTP
Form
	int HttpNode_SendContentsCode(HTTP_Con *con, int code, byte *type, byte *msg, int len);
Description
	Sends contents for the response to a HTTP request with an error code.
Status Internal
--*/
int HttpNode_SendContentsCode(HTTP_Con *con, int code, byte *type, byte *msg, int len)
{
	int sz, i;
	char *obuf, *s, *t;
	char *tbuf[16];

	kprint("send response, content: %s, %d bytes\n", type, len);

	obuf=kalloc(1024);
	s=obuf;
	s=kprints(s, "HTTP/1.0 %d %s\r\n", code, HttpNode_CodeName(code));
	s=kprints(s, "Server: %s/%s\r\n", PROJNAME, PROJVER);
	s=kprints(s, "MIME-Version: 1.0\r\n");
	s=kprints(s, "Content-Type: %s\r\n", type);
	s=kprints(s, "Content-Length: %d\r\n", len);
	s=kprints(s, "\r\n");

	vfwrite(obuf, 1, s-obuf, con->sock);
	if(msg)vfwrite(msg, 1, len, con->sock);

	return(0);
}

#if 0
int HttpNode_SendDir(HTTP_Con *con, char *name)
{
	VDIR *dir;
	long key, lkey;

	long buf[16];
	char *cols[16];
	int i;

	char *obuf, *s;

	kprint("send dir\n");

	dir=vfopendir(name);

	if(!dir)
	{
		kprint("dir %s\n", name);
		HttpNode_Send404(con, "text/html", NULL, 0);
		return(0);
	}

	obuf=kalloc(16384);
	s=obuf;

	s=kprints(s, "<html><body>\n");

	key=0;
	while(1)
	{
		lkey=VDir_DupKey(dir, key);
		i=VDir_ReadDir(buf, cols, &key, dir);
		if(!i)break;
		if(!lkey)
		{
			s=kprints(s, "(");
			for(i=0; i<16 && cols[i]; i++)
			{
				if(i)s=kprints(s, " ");
				s=kprints(s, "%s", cols[i]);
			}
			s=kprints(s, ")<br><br>\n");
		}

		s=kprints(s, "(");
		for(i=0; i<16 && cols[i]; i++)
		{
			if(i)s=kprints(s, " ");
			if(!strcmp(cols[i], "$name"))
			{
				s=kprints(s, "<a href=\"");
				s=kprints(s, "%s%s\">", name, buf[i]);
				s=kprints(s, "%s", buf[i]);
				s=kprints(s, "</a>");
			}else
			{
				if(cols[i][0]=='$')s=kprints(s, "%s", buf[i]);
				if(cols[i][0]=='%')s=kprints(s, "%d", buf[i]);
			}
		}
		s=kprints(s, ")<br>\n");
	}

	s=kprints(s, "</body></html>\n");

	kprint("send dir: ok %d\n", s-obuf);

	HttpNode_SendContents(con, "text/html", obuf, s-obuf);
	con->state=HTTPSTATE_CLOSE;

	kfree(obuf);

	return(0);
}
#endif

/*--
Cat pdnet;Protocols;HTTP
Form
	int HttpNode_SendPage(HTTP_Con *con);
Description
	Handles an HTTP GET request.
Status Internal
--*/
int HttpNode_SendPage(HTTP_Con *con)
{
	int sz, i;
	char *buf, *s, *t, *ct;
//	VDIR *dir;
	VFILE *fd;
	HTTP_Resource *cur;

	cur=httpnode_rsrc;
	while(cur)
	{
		if(!strncmp(con->req, cur->base, strlen(cur->base)))
			if(cur->get)
		{
			s=NULL;
			t=NULL;
			i=cur->get(cur, con, &s, &t, &sz);
			HttpNode_SendContentsCode(con, i, s, t, sz);

			if(s)kfree(s);
			if(t)kfree(t);
			con->state=HTTPSTATE_CLOSE;
			return(0);
		}
		cur=cur->next;
	}

	s=kralloc(strlen(httpnode_basedir)+strlen(con->req)+25);
	strcpy(s, httpnode_basedir);
	strcat(s, con->req);
	if(s[strlen(s)-1]=='/')strcat(s, "index.html");

	kprint("send page %s\n", s);

	buf=NULL;
//	buf=sa_loadfile(s, &sz);
	fd=vffopen(s, "r+b");
	if(fd)
	{
		buf=vf_bufferin(fd);
		sz=strlen(buf);
	}
	ct=HttpNode_MimeFromName(s);

	if(!buf)
	{
		s=con->req;
		if(s[strlen(s)-1]=='/')
		{
//			HttpNode_SendDir(con, con->req);
//			return(0);
		}
		HttpNode_Send404(con, "text/html", NULL, 0);
		return(0);
	}

/*
	s=buf;
	s=kprints(s, "\n");
	s=kprints(s, "<html><body>\n");
	s=kprints(s, "hello!<br>sv time=%d<br>\n", Sys_TimeMS());
	s=kprints(s, "</body></html>\n");

	sz=s-buf;
*/
//	sz=strlen(buf);
	HttpNode_SendContents(con, ct, buf, sz); //"text/html"
	con->state=HTTPSTATE_CLOSE;
	kfree(buf);

	vfclose(fd);

	return(0);
}

/*--
Cat pdnet;Protocols;HTTP
Form
	int HttpNode_SendPageHead(HTTP_Con *con);
Description
	Handles an HTTP HEAD request.
Status Internal
--*/
int HttpNode_SendPageHead(HTTP_Con *con)
{
	int sz, i;
	char *buf, *s, *t, *ct;
//	VDIR *dir;
	VFILE *fd;
	HTTP_Resource *cur;

	cur=httpnode_rsrc;
	while(cur)
	{
		if(!strncmp(con->req, cur->base, strlen(cur->base)))
			if(cur->get)
		{
			s=NULL;
			t=NULL;
			i=cur->head(cur, con, &s, &sz);
			HttpNode_SendContentsCode(con, i, s, NULL, sz);

			if(s)kfree(s);
			con->state=HTTPSTATE_CLOSE;

			return(0);
		}
		cur=cur->next;
	}

	s=kralloc(strlen(httpnode_basedir)+strlen(con->req)+25);
	strcpy(s, httpnode_basedir);
	strcat(s, con->req);
	if(s[strlen(s)-1]=='/')strcat(s, "index.html");

	kprint("send page head %s\n", s);

	buf=NULL;
//	buf=sa_loadfile(s, &sz);
	fd=vffopen(s, "r+b");
	if(fd)
	{
		buf=vf_bufferin(fd);
		sz=strlen(buf);
	}
	ct=HttpNode_MimeFromName(s);

	if(!buf)
	{
		s=con->req;
		if(s[strlen(s)-1]=='/')
		{
//			HttpNode_SendDir(con, con->req);
//			return(0);
		}
		HttpNode_Send404(con, "text/html", NULL, 0);
		return(0);
	}

/*
	s=buf;
	s=kprints(s, "\n");
	s=kprints(s, "<html><body>\n");
	s=kprints(s, "hello!<br>sv time=%d<br>\n", Sys_TimeMS());
	s=kprints(s, "</body></html>\n");

	sz=s-buf;
*/
//	sz=strlen(buf);
	HttpNode_SendContents(con, ct, NULL, sz); //"text/html"
	con->state=HTTPSTATE_CLOSE;
	kfree(buf);

	return(0);
}

/*--
Cat pdnet;Protocols;HTTP
Form
	int HttpNode_Send404(HTTP_Con *con, byte *type, byte *msg, int len);
Description
	Sends a 404 error.
Status Internal
--*/
int HttpNode_Send404(HTTP_Con *con, byte *type, byte *msg, int len)
{
	int sz, i;
	char *obuf, *s, *t;
	char *tbuf[16];

	kprint("send 404\n");

	obuf=kalloc(1024);

	s=obuf;
	s=kprints(s, "HTTP/1.0 404 Not Found\r\n");
	s=kprints(s, "Server: %s/%s\r\n", PROJNAME, PROJVER);
//	s=kprints(s, "MIME-Version: 1.0\r\n");
//	s=kprints(s, "Content-Type: %s\r\n", type);
	s=kprints(s, "Content-Length: %d\r\n", 0);
	s=kprints(s, "Connection: close\r\n");
	s=kprints(s, "\r\n");

	vfwrite(obuf, 1, s-obuf, con->sock);
//	vfwrite(msg, 1, len, con->sock);

	con->state=HTTPSTATE_CLOSE;
/*
	vfclose(con->sock);
	httpnode_cons[con->num]=NULL;
	if(con->buf)kfree(con->buf);
	kfree(con);
*/

	return(0);
}

/*--
Cat pdnet;Protocols;HTTP
Form
	int HttpNode_HandleLine(HTTP_Con *con, char *lb);
Description
	Handles a line located in the header of an HTTP request.
Status Internal
--*/
int HttpNode_HandleLine(HTTP_Con *con, char *lb)
{
	int sz, i;
	char *obuf, *s, *t;
	char **tbuf;

	tbuf=kralloc(16*sizeof(char *));
	for(i=0; i<16; i++)tbuf[i]=kralloc(64);

	kprint("line: '%s'\n", lb);

	s=lb;
	i=0;
	while(*s)
	{
//		kprint(" a %d\n", i);
		t=tbuf[i++];
		while(*s>' ')*t++=*s++;
		*t++=0;
		while(*s && (*s<=' '))s++;
	}

//	kprint(" b\n");

	if(!*tbuf[0])
	{
		switch(con->state)
		{
		case HTTPSTATE_GET:
			con->state=0;
			HttpNode_SendPage(con);
			break;
		case HTTPSTATE_POST:
			con->state=HTTPSTATE_RECD;
			con->rbuf=malloc(con->rlen);
			con->erbuf=con->rbuf;
			break;
		default:
			con->state=0;
			break;
		}
		return(0);
	}

//	kprint(" c\n");

	if(!kstricmp(tbuf[0], "GET"))
	{
		con->state=HTTPSTATE_GET;

		s=tbuf[1];
		t=con->req;
		while(*s && (*s!='?'))*t++=*s++;
		*t++=0;

		if(*s=='?')s++;
		t=con->arg;
		while(*s)*t++=*s++;
		*t++=0;
	}
	if(!kstricmp(tbuf[0], "POST"))
	{
		con->state=HTTPSTATE_POST;
//		strcpy(con->req, tbuf[1]);

		s=tbuf[1];
		t=con->req;
		while(*s && (*s!='?'))*t++=*s++;
		*t++=0;

		if(*s=='?')s++;
		t=con->arg;
		while(*s)*t++=*s++;
		*t++=0;
	}
	if(!kstricmp(tbuf[0], "HEAD"))
	{
		con->state=HTTPSTATE_HEAD;
//		strcpy(con->req, tbuf[1]);

		s=tbuf[1];
		t=con->req;
		while(*s && (*s!='?'))*t++=*s++;
		*t++=0;

		if(*s=='?')s++;
		t=con->arg;
		while(*s)*t++=*s++;
		*t++=0;
	}

	if(!kstricmp(tbuf[0], "Content-Length:"))
	{
		con->rlen=atoi(tbuf[1]);
	}
	if(!kstricmp(tbuf[0], "Content-Type:"))
	{
		strcpy(con->ct, tbuf[1]);
	}

//	kprint(" d\n");
	return(0);
}

/*--
Cat pdnet;Protocols;HTTP
Form
	int HttpNode_HandleMessage(HTTP_Con *con, char *msg, int len);
Description
	Handles the data recieved from an HTTP POST request.
Status Internal
--*/
int HttpNode_HandleMessage(HTTP_Con *con, char *msg, int len)
{
	int sz, i;
	char *s, *t;
	HTTP_Resource *cur;
	VFILE *fd;

//	kprint("message: %s", msg);
//	XmlRpc_HandleMessage(con, msg);

	cur=httpnode_rsrc;
	while(cur)
	{
		if(!strncmp(con->req, cur->base, strlen(cur->base)))
			if(cur->get)
		{
			s=kstrdup(con->ct);
			t=kstrdup(msg);
			sz=len;
			i=cur->post(cur, con, &s, &t, &sz);
			HttpNode_SendContentsCode(con, i, s, t, sz);

			if(s)kfree(s);
			if(t)kfree(t);
			con->state=HTTPSTATE_CLOSE;

			return(0);
		}
		cur=cur->next;
	}

	i=Var_GetValue("http_post_base");

	if(!i)
	{
		i=403;
		s="text/html";
		t="<html><body>"
			"403 Forbidden<br>"
			"Posting to base paths is disallowed.<br>"
			"</body></html>";
		sz=strlen(t);
		HttpNode_SendContentsCode(con, i, s, t, sz);

		return(0);
	}

	s=kralloc(strlen(httpnode_basedir)+strlen(con->req)+25);
	strcpy(s, httpnode_basedir);
	strcat(s, con->req);
	if(s[strlen(s)-1]=='/')strcat(s, "index.html");

	fd=vffopen(s, "wb");
	if(fd)
	{
		vfwrite(msg, 1, len, fd);

		i=201;
		s="text/html";
		t="<html><body>"
			"201 Created<br>"
			"Posting to base paths is allowed.<br>"
			"Defined resource was created.<br>"
			"</body></html>";
		sz=strlen(t);
		HttpNode_SendContentsCode(con, i, s, t, sz);
	}

	return(0);
}

/*--
Cat pdnet;Protocols;HTTP
Form
	int HttpNode_HandleConnection(HTTP_Con *con);
Description
	Polls an HTTP connection to see if anything has changed.
Status Internal
--*/
int HttpNode_HandleConnection(HTTP_Con *con)
{
	int sz, i;
	char *obuf, *s, *t;
	char mbuf[1024], *tbuf[16];

	if(!con->buf)
	{
		con->buf=malloc(16384);
		con->end=con->buf;
	}

	while(1)
	{
		sz=vfread(con->end, 1, 16484, con->sock);
		if(sz<0)break;
		con->end+=sz;
//		kprint("%s", con->buf);
		if(!(con->end-con->buf))break;

		i=con->end-con->buf;
		if((con->state==HTTPSTATE_RECD) && con->rlen)
		{
			sz=(con->rlen<i)?con->rlen:i;
			memcpy(con->erbuf, con->buf, sz);
			con->erbuf+=sz;
			con->rlen-=sz;

			s=con->buf+sz;
			t=con->buf;
			while(s<con->end)*t++=*s++;
			con->end=t;

			if(!con->rlen)
			{
				con->state=0;
				HttpNode_HandleMessage(con, con->rbuf,
					con->erbuf-con->rbuf);
				free(con->rbuf);
			}

			continue;
		}

		s=con->buf;
		t=mbuf;
		while(*s && (*s!='\r') && (*s!='\n'))*t++=*s++;
		*t++=0;

		if((*s!='\r') && (*s!='\n'))continue;

		if(*s=='\r')s++;
		if(*s=='\n')s++;

		t=con->buf;
		while(s<con->end)*t++=*s++;
		con->end=t;

		HttpNode_HandleLine(con, mbuf);
	}
	if((sz==-1) || (con->state==HTTPSTATE_CLOSE))
	{
		vfclose(con->sock);
		httpnode_cons[con->num]=NULL;
		if(con->buf)free(con->buf);
		kfree(con);
	}

	return(0);
}

/*--
Cat pdnet;Protocols;HTTP
Form
	HTTP_Resource *HttpNode_NewResource(char *base);
Description
	Creates a new HTTP Resource.
	A Resourse is used for making some service available to clients.
--*/
HTTP_Resource *HttpNode_NewResource(char *base)
{
	HTTP_Resource *tmp;

	kprint("HTTP: new resource %s\n", base);

	tmp=kalloc(sizeof(HTTP_Resource));

	tmp->base=kstrdup(base);

	tmp->next=httpnode_rsrc;
	httpnode_rsrc=tmp;

	return(tmp);
}

/*--
Cat pdnet;Protocols;HTTP
Form
	int HttpNode_Poll();
Description
	Polls for any activity over connections or any new connections.
Status Internal
--*/
int HttpNode_Poll()
{
	VADDR addr;
	int i;
	VFILE *newsock;
//	elem t, cur;
	HTTP_Con *con;

	int stime;

	if(!httpnode_sock)return(-1);

//	*(int *)-1=0;

//	stime=Sys_TimeMS();

	while(1)
	{
//		vfioctls(httpnode_sock, "accept", &newsock, &addr);
		vfioctl2(httpnode_sock, VFSOCK_IOCTL_ACCEPT, &newsock, &addr);
		if(!newsock)break;

		kprint("HTTP: recieved connection\n");

		con=HttpNode_NewCon();
		con->sock=newsock;
	}

	for(i=0; i<256; i++)if(httpnode_cons[i])
		HttpNode_HandleConnection(httpnode_cons[i]);

	return(0);
}

/*--
Cat pdnet;Protocols;HTTP
Form
	int HttpNode_GetPort();
Description
	Returns the port the HTTP server is running on.
--*/
int HttpNode_GetPort()
{
	return(httpnode_port);
}

/*--
Cat pdnet;Protocols;HTTP
Form
	int HttpNode_Init();
Description
	Initialize HTTP support.
--*/
int HttpNode_Init()
{
	NET_Interface *iface;
	char *s;
	int i;

	if(httpnode_sock)return(0);

	iface=NET_CreateInterface("http");
	iface->poll=&HttpNode_Poll;

	httpnode_iface=iface;

	s=Var_GetString("http_base");
	if(s)httpnode_basedir=s;

	httpnode_sock=NULL;

	i=Var_GetValue("http_port");
	if(!httpnode_sock && i)
	{
		httpnode_port=i;
		httpnode_sock=TCP_OpenListen(httpnode_port);
	}

	if(!httpnode_sock)
	{
		httpnode_port=80;
		httpnode_sock=TCP_OpenListen(httpnode_port);
	}
	if(!httpnode_sock)
	{
		httpnode_port=8080;
		httpnode_sock=TCP_OpenListen(httpnode_port);
	}
	if(!httpnode_sock)
	{
		httpnode_port=7936;
		while(httpnode_port<8192)
		{
			httpnode_sock=TCP_OpenListen(httpnode_port);
			if(httpnode_sock)break;
			httpnode_port++;
		}
	}

	if(httpnode_sock)
	{
		kprint("HTTP: port opened %d\n", httpnode_port);
	}else
	{
		kprint("HTTP: fail open host port\n");
	}

	HttpNode_InitCl();
	HttpNode_MimeInit();

	return(0);
}
