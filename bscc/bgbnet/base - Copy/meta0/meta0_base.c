/*--
Cat pdnet;Protocols;Meta0
Text
	meta 0 will attempt to be a "metaprotocol".
	basically it will allow sockets to be created and dispatch to handlers defined for that port.
--*/

#include <bgbnet.h>

Meta0_Con *meta0_cons[256];
Meta0_PortInfo *meta0_ports[256];

/*--
Cat pdnet;Protocols;Meta0
Form
	Meta0_Con *Meta0_NewCon();
Description
	Creates a connection structure.
--*/
Meta0_Con *Meta0_NewCon()
{
	Meta0_Con *con;
	int i;

	for(i=0; i<256; i++)if(!meta0_cons[i])
	{
		con=kalloc(sizeof(Meta0_Con));
		meta0_cons[i]=con;
		con->num=i;
		return(con);
	}
	return(NULL);
}

/*--
Cat pdnet;Protocols;Meta0
Form
	Meta0_PortInfo *Meta0_NewPortInfo();
Description
	Creates a PortInfo structure.
--*/
Meta0_PortInfo *Meta0_NewPortInfo()
{
	Meta0_PortInfo *inf;
	int i;

	for(i=0; i<256; i++)if(!meta0_ports[i])
	{
		inf=kalloc(sizeof(Meta0_PortInfo));
		meta0_ports[i]=inf;
		inf->num=i;
		return(inf);
	}
	return(NULL);
}

/*--
Cat pdnet;Protocols;Meta0
Form
	Meta0_Con *Meta0_Connect(Meta0_PortInfo *inf, VADDR *addr);
Description
	Connect to a server using inf for handlers and such.
--*/
Meta0_Con *Meta0_Connect(Meta0_PortInfo *inf, VADDR *addr)
{
	Meta0_Con *con;
	VFILE *sock;

	if(!addr)return(NULL);
	sock=TCP_OpenConnect(addr);
	if(!sock)return(NULL);

	con=Meta0_NewCon();
	con->sock=sock;
	con->addr=kmemdup(addr, sizeof(VADDR));
	con->info=inf;

	return(con);
}

/*--
Cat pdnet;Protocols;Meta0
Form
	int Meta0_CloseConnection(Meta0_Con *con);
Description
	Close a connection.
--*/
int Meta0_CloseConnection(Meta0_Con *con)
{
	Meta0_PortInfo *inf;

	inf=con->info;

	kprintd("Meta0: Close connection, service %s.\n", inf->name);

	if(inf->closed)
		inf->closed(con);

	vfclose(con->sock);
	meta0_cons[con->num]=NULL;
	if(con->buf)kfree(con->buf);
	kfree(con);

	return(0);
}

/*--
Cat pdnet;Protocols;Meta0
Form
	Meta0_Con *Meta0_FindCon(VADDR *addr);
Description
	Find the connection associated with a given address.
--*/
Meta0_Con *Meta0_FindCon(VADDR *addr)
{
	int i;

	for(i=0; i<256; i++)
		if(meta0_cons[i])
			if(NET_AddrEqual(meta0_cons[i]->addr, addr))
				return(meta0_cons[i]);
	return(NULL);
}

/*--
Cat pdnet;Protocols;Meta0
Form
	int Meta0_HandleConnection(Meta0_Con *con);
Description
	Polls a connection for activity.
Status Internal
--*/
int Meta0_HandleConnection(Meta0_Con *con)
{
	Meta0_PortInfo *inf;
	int i, sz;
	char *s, *t;

	if(!con->buf)
	{
		con->buf=kalloc(65536);
		con->end=con->buf;
	}

	inf=con->info;

	while(1)
	{
		memset(con->end, 0, 65536-(con->end-con->buf));
		sz=vfread(con->end, 1, 65536-(con->end-con->buf), con->sock);
		if(sz<0)break;

//		kprintd("meta0: got %d bytes\n", sz);

		con->end+=sz;
		*con->end=0;
//		kprintd("%s", con->buf);
		if(con->end==con->buf)break;

		i=con->end-con->buf;
		s=con->buf;

		if(inf->input)
			sz=inf->input(con);
			else sz=-1;

//		kprintd("meta0 stat: %p %p %d\n", con->buf, s, sz);

		if(sz==-1)break;
		s+=sz;

		if(s!=con->buf)
		{
			t=con->buf;
			while(s<con->end)*t++=*s++;
			con->end=t;
		}else break;
	}

	if(sz<0)
	{
		kprintd("Meta0: Connection closed, service %s.\n", inf->name);

		if(inf->closed)
			inf->closed(con);

		vfclose(con->sock);
		meta0_cons[con->num]=NULL;
		if(con->buf)kfree(con->buf);
		kfree(con);
	}

	return(0);
}

/*--
Cat pdnet;Protocols;Meta0
Form
	int Meta0_PollPort(Meta0_PortInfo *inf);
Description
	Polls a PortInfo structure for new connections.
Status Internal
--*/
int Meta0_PollPort(Meta0_PortInfo *inf)
{
	VADDR addr;
	int i;

	VFILE *newsock;
	Meta0_Con *con;

	if(!inf->sock)return(0); //client port

	while(1)
	{
		vfioctl2(inf->sock, VFSOCK_IOCTL_ACCEPT, &newsock, &addr);
		if(!newsock)break;

		kprintd("Meta0: recieved connection, service %s\n", inf->name);

		con=Meta0_NewCon();
		con->sock=newsock;
		con->addr=kmemdup(&addr, sizeof(VADDR));
		con->info=inf;

		if(inf->new_con)
			inf->new_con(con);
	}
	return(0);
}

/*--
Cat pdnet;Protocols;Meta0
Form
	int Meta0_Poll();
Description
	Polls all open ports and connections for activity.
Status Internal
--*/
int Meta0_Poll()
{
	int i;

	for(i=0; i<256; i++)if(meta0_ports[i])
		Meta0_PollPort(meta0_ports[i]);

	for(i=0; i<256; i++)if(meta0_cons[i])
		Meta0_HandleConnection(meta0_cons[i]);

	return(0);
}

/*--
Cat pdnet;Protocols;Meta0
Form
	Meta0_PortInfo *Meta0_CreatePort(int port, char *name);
Description
	Creates a new server socket.
--*/
Meta0_PortInfo *Meta0_CreatePort(int port, char *name)
{
	Meta0_PortInfo *inf;

	if(!name)name="(unnamed)";
	kprintd("Meta0: new port %d, service %s\n", port, name);

	inf=Meta0_NewPortInfo();
	inf->sock=TCP_OpenListen(port);
	inf->port=port;
	inf->name=kstrdup(name);

	return(inf);
}

/*--
Cat pdnet;Protocols;Meta0
Form
	Meta0_PortInfo *Meta0_CreateClientPort(char *name);
Description
	Creates a client socket.
--*/
Meta0_PortInfo *Meta0_CreateClientPort(char *name)
{
	Meta0_PortInfo *inf;

	if(!name)name="(unnamed)";
	kprintd("Meta0: new client port %s\n", name);

	inf=Meta0_NewPortInfo();
	inf->name=kstrdup(name);

	return(inf);
}

/*--
Cat pdnet;Protocols;Meta0
Form
	int Meta0_Init();
Description
	Init Meta0 subsystem.
Status Internal
--*/
int Meta0_Init()
{
	kprintd("Meta0_Init\n");

	memset(meta0_ports, 0, 256*sizeof(Meta0_PortInfo *));
	memset(meta0_cons, 0, 256*sizeof(Meta0_Con *));

	return(0);
}
