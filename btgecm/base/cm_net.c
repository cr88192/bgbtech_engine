#include <btgecm.h>

dyt btge_net_clqueue=NULL;
dyt btge_net_svqueue=NULL;
int btge_net_local=0;

void *btge_net_mutex=NULL;

BSXRP_Context *btge_net_clientctx=NULL;
BSXRP_Context *btge_net_serverctx[32];

void btge_net_lock()
{
	thLockFastMutex(btge_net_mutex);
}

void btge_net_unlock()
{
	thUnlockFastMutex(btge_net_mutex);
}

BTGE_API void BTGE_Net_ClientConnect(char *host)
{
	BTGE_Net_Init();

	if(btge_net_clientctx)
	{
		btge_net_lock();
		BSXRP_Close(btge_net_clientctx);
		btge_net_clientctx=NULL;
		btge_net_unlock();
	}

	btge_net_clientctx=BSXRP_ConnectName(host);
	if(!btge_net_clientctx)
	{
		printf("BTGE_Net_ClientConnect: Connect Failed\n");
	}else
	{
		printf("BTGE_Net_ClientConnect: Connected %p\n",
			btge_net_clientctx);
	}
}

BTGE_API void BTGE_Net_ClientDisconnect()
{
	BTGE_Net_Init();

	if(btge_net_clientctx)
	{
		btge_net_lock();
		BSXRP_Close(btge_net_clientctx);
		btge_net_clientctx=NULL;
		btge_net_local=2;	//disconnected
		btge_net_unlock();
	}
}

BTGE_API void BTGE_Net_ClientFlush()
{
	BTGE_Net_Init();

	btge_net_lock();
	btge_net_clqueue=NULL;
	btge_net_svqueue=NULL;
	btge_net_local=0;
	btge_net_unlock();
}

/** Get update message sent by server */
BTGE_API dyt BTGE_Net_ClientGetUpdate()
{
	dyt t, l;

	BTGE_Net_Init();

	NET_Poll();

	if(btge_net_clientctx)
	{
		if(!BSXRP_ConnectedP(btge_net_clientctx))
		{
			printf("Connection Lost\n");
			BTGE_Net_ClientDisconnect();
			return(NULL);
		}

		t=BSXRP_GetMessage(btge_net_clientctx);
		if(t)return(t);
	}

	if(btge_net_clqueue)
	{
		btge_net_lock();
		l=btge_net_clqueue;
		t=dycar(l);
		btge_net_clqueue=dycdr(l);
		dyFreeCons(l);
		btge_net_unlock();
		return(t);
	}
	return(NULL);
}

/** Send client update to server */
BTGE_API void BTGE_Net_ClientSendUpdate(dyt msg)
{
	int i;

	BTGE_Net_Init();

	if(btge_net_clientctx)
	{
		i=BSXRP_SendMessage(btge_net_clientctx, msg);
		dyFreeListR(msg);
		
		if(i<0)
		{
			printf("Connection Lost\n");
			BTGE_Net_ClientDisconnect();
			return;
		}
		
		return;
	}

	if(btge_net_local==0)
		btge_net_local=1;
	if(btge_net_local==1)
	{
		btge_net_lock();
		btge_net_svqueue=dynappend(btge_net_svqueue, dylist1(msg));
		btge_net_unlock();
	}
}


/** Get update message sent by server.
  * Gives number of connected client who sent message.
  */
BTGE_API dyt BTGE_Net_ServerGetUpdate(int *rclnum)
{
	BSXRP_Context *ctx;
	dyt t, l;
	int i, j;

	BTGE_Net_Init();

	NET_Poll();

	if(btge_net_svqueue)
	{
		btge_net_lock();
		if(rclnum)*rclnum=0;
		l=btge_net_svqueue;
		t=dycar(l);
		btge_net_svqueue=dycdr(l);
		dyFreeCons(l);
		btge_net_unlock();
		return(t);
	}
	
	for(i=0; i<32; i++)
	{
		if(btge_net_serverctx[i])
		{
			ctx=btge_net_serverctx[i];
			if(!BSXRP_ConnectedP(ctx))
			{
				BSXRP_Close(ctx);
				btge_net_serverctx[i]=NULL;
				continue;
			}
		
			t=BSXRP_GetMessage(ctx);
			if(t)
			{
//				dysPrintln(t);
				if(rclnum)*rclnum=i+1;
				return(t);
			}
		}
	}
	
	return(NULL);
}

BTGE_API s64 BTGE_Net_ServerGetClientMask()
{
	s64 v;
	int i;

	BTGE_Net_Init();
	btge_net_lock();
	v=0;
	for(i=0; i<32; i++)
		if(btge_net_serverctx[i])
			v|=1LL<<i;
	btge_net_unlock();
	return(v);
}

BTGE_API void BTGE_Net_ServerSendUpdate(int clnum, dyt msg)
{
	int i;

	BTGE_Net_Init();
	if((clnum>=1) && (clnum<=32))
	{
		if(btge_net_serverctx[clnum-1])
		{
			i=BSXRP_SendMessage(btge_net_serverctx[clnum-1], msg);
			dyFreeListR(msg);
			
			if(i<0)
			{
				BSXRP_Close(btge_net_serverctx[clnum-1]);
				btge_net_serverctx[clnum-1]=NULL;
			}
			
			return;
		}
		printf("BTGE_Net_ServerSendUpdate: No Client %d\n", clnum);
		return;
	}

	if(btge_net_local==1)
	{
		//local passing is enabled
		btge_net_lock();
		btge_net_clqueue=dynappend(btge_net_clqueue, dylist1(msg));
		btge_net_unlock();
	}else
	{
		//discard message
		dyFreeListR(msg);
	}
}

int btge_net_svaccept(BSXRP_Context *ctx)
{
	int i;
	
	printf("btge_net_svaccept %p\n", ctx);
	
	for(i=0; i<32; i++)
	{
		if(!btge_net_serverctx[i])
		{
			printf("btge_net_svaccept: OK, clnum %d = %p\n", i+1, ctx);
			btge_net_serverctx[i]=ctx;
			return(1);
		}
	}
	return(-1);
}

BTGE_API void BTGE_Net_Init()
{
	static int init=0;
	
	if(init)return;
	init=1;

	btge_net_mutex=thFastMutex();

	NET_Init();
	BSXRP_Init();
	BSXRP_RegisterAccept(btge_net_svaccept);
}
