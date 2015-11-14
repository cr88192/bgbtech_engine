#include <bgbnet.h>

#define BSXRP_PORT	18512

Meta0_PortInfo *bsxrp_cl_inf;
NET_Interface *bsxrp_cl_iface;

int (*bsxrp_sv_accept_f[16])(BSXRP_Context *ctx);

BSXRP_Context *BSXRP_AllocContext()
{
	BSXRP_Context *tmp;
	
	tmp=gctalloc("bxsrp_context_t",
		sizeof(BSXRP_Context));
	tmp->mux_rover=1;

	return(tmp);
}

BGBNET_API void BSXRP_FreeContext(BSXRP_Context *ctx)
{
	gcfree(ctx);
}

BGBNET_API BSXRP_Context *BSXRP_Connect(VADDR *addr)
{
	BSXRP_Context *tmp;
	Meta0_Con *con;

	BSXRP_Init();

	tmp=BSXRP_AllocContext();

	con=Meta0_Connect(bsxrp_cl_inf, addr);
	if(!con)
	{
		gc_printf("BSXRP: Connection failed.\n");
		BSXRP_FreeContext(tmp);
		return(NULL);
	}

	con->data=tmp;
	tmp->con=con;

	BSXRP_SendStreamInit(tmp);

	return(tmp);
}

BGBNET_API BSXRP_Context *BSXRP_ConnectName(char *host)
{
	BSXRP_Context *tmp;
	VADDR *addr;

	addr=NET_DecodeHostnamePort(host, BSXRP_PORT);
	if(!addr)return(NULL);
	tmp=BSXRP_Connect(addr);
	return(tmp);
}

BGBNET_API void BSXRP_Close(BSXRP_Context *ctx)
{
	BSXRP_FreeContext(ctx);
}

BGBNET_API int BSXRP_ConnectedP(BSXRP_Context *ctx)
{
	if(!ctx->con)return(0);
	return(1);
}


BGBNET_API int BSXRP_SendMessage(BSXRP_Context *ctx, dyt msg)
{
	byte buf[65536];
//	byte *buf;
	int i, j, k;

//	ctx->queue_out=dynappend(ctx->queue_out, dylist1(msg));

	if(!msg)return(0);

//	i=BSXRP_SendStreamDeflatedMessage(ctx, msg);
//	return(i);

#if 1
	if(!ctx->tx)
	{
		ctx->tx=BSXRP_CreateStreamContext();
	}
	
//	buf=malloc(65536);
	i=BSXRP_MessageEncodeValueBuffer(ctx->tx, msg, buf, 65536);

//	dyPrintf("BSXRP_SendStreamMessage: %d -> %d\n", i, ct-buf);
	k=vfwrite(buf, 1, i, ctx->con->sock);
//	dyPrintf("BSXRP_SendStreamMessage: sent=%d\n", k);
//	free(buf);

	return(k);
#endif
}

BGBNET_API dyt BSXRP_GetMessage(BSXRP_Context *ctx)
{
	dyt t, l;
	if(ctx->queue_in)
	{
		l=ctx->queue_in;
		t=dycar(l);
		ctx->queue_in=dycdr(l);
		dyFreeCons(l);
		return(t);
	}
	return(NULL);
}


void BSXRP_SendStreamInit(BSXRP_Context *ctx)
{
	byte buf[256];
	
	dyPrintf("BSXRP_SendStreamInit: %p\n", ctx);
	buf[0]=0x7F; buf[1]=0xFF; buf[2]=0xFE; buf[3]=0x03;
	buf[4]=0x00; buf[5]=0x01; buf[6]=0x01; buf[7]=0x00;
	vfwrite(buf, 1, 8, ctx->con->sock);
}

void BSXRP_SendStreamMessage(BSXRP_Context *ctx, dyt msg)
{
	byte buf[4096];
	
//	buf[0]=0x7F; buf[1]=0xFF; buf[2]=0xFE; buf[3]=0x01;
//	vfwrite(buf, 1, 4, ctx->con->sock);
}

int BSXRP_SendStreamDeflatedMessage(BSXRP_Context *ctx, dyt msg)
{
	byte buf[65536];
	byte buf1[65536];
	byte *cs, *cse, *ct;
	int i, j, k;
	
	if(!ctx->con)return(-1);
	
//	buf[0]=0x7F; buf[1]=0xFF; buf[2]=0xFE; buf[3]=0x01;
//	vfwrite(buf, 1, 4, ctx->con->sock);

	memset(buf, 0, 65536);
	memset(buf1, 0, 65536);

	dysDumpStr((char *)buf, msg);
	i=strlen((char *)buf);
	j=vfDeflateBuffer(buf1, buf, 65536, i);
//	j=0;
	
	cs=buf1; cse=cs+j;
	ct=buf;

	*ct++=0x7F; *ct++=0xFF; *ct++=0xFE; *ct++=0x09;
	*ct++=(i>> 0)&0xFF; *ct++=(i>> 8)&0xFF;
	*ct++=(i>>16)&0xFF; *ct++=(i>>24)&0xFF;

	while(cs<cse)
	{
		if((cs[0]==0x7F) && (cs[1]==0xFF) && (cs[2]==0xFE))
		{
			*ct++=*cs++; *ct++=*cs++; *ct++=*cs++;
			*ct++=0x00;
			continue;
		}
		*ct++=*cs++;
	}

	k=0;
	*ct++=0x7F; *ct++=0xFF; *ct++=0xFE; *ct++=0x0A;
	*ct++=(k>> 0)&0xFF; *ct++=(k>> 8)&0xFF;
	*ct++=(k>>16)&0xFF; *ct++=(k>>24)&0xFF;

//	dyPrintf("BSXRP_SendStreamDeflatedMessage: %d -> %d\n", i, ct-buf);

	k=vfwrite(buf, 1, ct-buf, ctx->con->sock);
//	dyPrintf("BSXRP_SendStreamDeflatedMessage: sent=%d\n", k);
	
	return(k);
}

int bsxrp_poll()
{
	return(0);
}

void BSXRP_HandleDeflatedMsg(BSXRP_Context *ctx, byte *css, byte *cse,
	int osz)
{
	byte *tbuf, *t2buf;
	byte *cs, *ct;
	dyt t;
	
//	dyPrintf("BSXRP_HandleDeflatedMsg: isz=%d, osz=%d\n", cse-css, osz);
	
	tbuf=malloc((cse-css)+16);
	cs=css; ct=tbuf;
	while(cs<cse)
	{
		if((cs[0]==0x7F) && (cs[1]==0xFF) &&
			(cs[2]==0xFE) && (cs[3]==0x00))
		{
			*ct++=*cs++; *ct++=*cs++; *ct++=*cs++;
			cs++; continue;
		}
		*ct++=*cs++;
	}
	
	t2buf=malloc(osz+256);
	memset(t2buf, 0, osz+256);
	vfInflateBuffer(t2buf, tbuf, osz, ct-tbuf);
	t=dysParseFromString(t2buf);

//	dyPrintf("%s\n", t2buf);
//	dyPrintf("Got Message: ");
//	dysPrintln(t);

	free(tbuf);
	free(t2buf);

	ctx->queue_in=dynappend(ctx->queue_in, dylist1(t));
}

void BSXRP_HandleCodedMsg(BSXRP_Context *ctx, byte *css, byte *cse)
{
	dyt v;
	int i;

	if(!ctx->rx)
	{
		ctx->rx=BSXRP_CreateStreamContext();
	}

	BSXRP_SetupBitstreamRead(ctx->rx, css, cse);
	while(1)
	{
		i=BSXRP_Read4Bits(ctx->rx);
		if(!i)break;
		
		if(i==1)
		{
			v=BSXRP_DecodeMessageValue(ctx->rx);
			ctx->queue_in=dynappend(ctx->queue_in, dylist1(v));
			
//			dyPrintf("Got: ");
//			dysPrintln(v);
			
			continue;
		}

		if(i==3)
		{
			BSXRP_DecodeMultiplexData(ctx, ctx->rx);
			continue;
		}

		if((i>=4) && (i<=7))
		{
			BSXRP_DecodeHuffTable(ctx->rx, i-4);
			continue;
		}

		dyPrintf("BSXRP_HandleCodedMsg: Bad MiniTag %d\n", i);
		break;
	}
	BSXRP_FinishBitstreamRead(ctx->rx);
}

void BSXRP_HandleHuffmanTable(BSXRP_Context *ctx,
	byte *css, byte *cse, int tab)
{
	BSXRP_SetupBitstreamRead(ctx->rx, css, cse);
	BSXRP_DecodeHuffTable(ctx->rx, tab);
	BSXRP_FinishBitstreamRead(ctx->rx);
}

int BSXRP_TagNotEscapeTagP(byte *cs)
{
	if((cs[0]==0x7F) && (cs[1]==0xFF) &&
		(cs[2]==0xFE) && (cs[3]!=0x00))
			return(1);
	return(0);
}

int BSXRP_TagEscapeTagP(byte *cs)
{
	if((cs[0]==0x7F) && (cs[1]==0xFF) &&
		(cs[2]==0xFE) && (cs[3]==0x00))
			return(1);
	return(0);
}

int BSXRP_TagResetP(byte *cs)
{
	if((cs[0]==0x7F) && (cs[1]==0xFF) &&
		(cs[2]==0xFE) && (cs[3]==0x03))
			return(1);
	return(0);
}

int BSXRP_TagStartMessageDeflateP(byte *cs)
{
	if((cs[0]==0x7F) && (cs[1]==0xFF) &&
		(cs[2]==0xFE) && (cs[3]==0x09))
			return(1);
	return(0);
}

int BSXRP_TagEndMessageDeflateP(byte *cs)
{
	if((cs[0]==0x7F) && (cs[1]==0xFF) &&
		(cs[2]==0xFE) && (cs[3]==0x0A))
			return(1);
	return(0);
}

int BSXRP_TagStartHuffmanTableP(byte *cs)
{
	if((cs[0]==0x7F) && (cs[1]==0xFF) &&
		(cs[2]==0xFE) && (cs[3]>=0x04) && (cs[3]<=0x07))
			return(1);
	return(0);
}

int BSXRP_TagEndHuffmanTableP(byte *cs)
{
	if((cs[0]==0x7F) && (cs[1]==0xFF) &&
		(cs[2]==0xFE) && (cs[3]==0x08))
			return(1);
	return(0);
}

int BSXRP_TagAllowedEndHuffmanTableP(byte *cs)
{
	if(!BSXRP_TagNotEscapeTagP(cs))
		return(0);
	if(cs[3]==0x08)return(1);	//cannonical end

	if(cs[3]==0x01)return(1);
	if(cs[3]==0x04)return(1);
	if(cs[3]==0x05)return(1);
	if(cs[3]==0x06)return(1);
	if(cs[3]==0x07)return(1);

//	if((cs[0]==0x7F) && (cs[1]==0xFF) &&
//		(cs[2]==0xFE) && (cs[3]==0x08))
//			return(1);
	return(0);
}

int BSXRP_TagStartMessageP(byte *cs)
{
	if((cs[0]==0x7F) && (cs[1]==0xFF) &&
		(cs[2]==0xFE) && (cs[3]==0x01))
			return(1);
	return(0);
}

int BSXRP_TagEndMessageP(byte *cs)
{
	if((cs[0]==0x7F) && (cs[1]==0xFF) &&
		(cs[2]==0xFE) && (cs[3]==0x02))
			return(1);
	return(0);
}

int bsxrp_input(Meta0_Con *con)
{
	BSXRP_Context *ctx;
	byte *cs, *ce, *cs1;
	int i, j, k;
	
	ctx=(BSXRP_Context *)con->data;
	cs=(byte *)con->buf;
	ce=(byte *)con->end;
	
//	dyPrintf("bsxrp_input: %p %d\n", ctx, ce-cs);
	
	while(cs<ce)
	{
		//look for a marker
		while(cs<(ce-4))
		{
//			if((cs[0]==0x7F) && (cs[1]==0xFF) &&
//				(cs[2]==0xFE) && (cs[3]!=0x00))
			if(BSXRP_TagNotEscapeTagP(cs))
				break;
			cs++;	//skip bytes
		}
		if(cs>=ce)
			break;

//		dyPrintf("bsxrp_input: marker %02X\n", cs[3]);

//		if((cs[0]==0x7F) && (cs[1]==0xFF) &&
//			(cs[2]==0xFE) && (cs[3]==0x03))
		if(BSXRP_TagResetP(cs))
		{
			cs+=8;
			continue;
		}

//		if((cs[0]==0x7F) && (cs[1]==0xFF) &&
//			(cs[2]==0xFE) && (cs[3]==0x09))
		if(BSXRP_TagStartMessageDeflateP(cs))
		{
			cs1=cs+8;
			while(cs1<=(ce-8))
			{
//				if((cs1[0]==0x7F) && (cs1[1]==0xFF) &&
//					(cs1[2]==0xFE) && (cs1[3]!=0x00))
				if(BSXRP_TagNotEscapeTagP(cs1))
					break;
				cs1++;	//skip bytes
			}

			if(cs1>(ce-8))
			{
				dyPrintf("bsxrp_input: incomplete message %02X\n", cs[3]);
				break;
			}

//			if((cs1[0]==0x7F) && (cs1[1]==0xFF) &&
//				(cs1[2]==0xFE) && (cs1[3]==0x0A))
			if(BSXRP_TagEndMessageDeflateP(cs1))
			{
				i=cs[4]|(cs[5]<<8)|(cs[6]<<16)|(cs[7]<<24);
				BSXRP_HandleDeflatedMsg(ctx, cs+8, cs1, i);
				cs=cs1+8;
				continue;
			}

//			if((cs1[0]==0x7F) && (cs1[1]==0xFF) &&
//				(cs1[2]==0xFE) && (cs1[3]!=0x00))
			if(BSXRP_TagNotEscapeTagP(cs1))
			{
				dyPrintf("bsxrp_input: broken message %02X\n", cs[3]);
				cs=cs1;
				continue;
			}

			continue;
		}


//		if((cs[0]==0x7F) && (cs[1]==0xFF) &&
//			(cs[2]==0xFE) && (cs[3]==0x01))
		if(BSXRP_TagStartMessageP(cs))
		{
			cs1=cs+4;
			while(cs1<=(ce-4))
			{
//				if((cs1[0]==0x7F) && (cs1[1]==0xFF) &&
//					(cs1[2]==0xFE) && (cs1[3]!=0x00))
				if(BSXRP_TagNotEscapeTagP(cs1))
					break;
				cs1++;	//skip bytes
			}

			if(cs1>(ce-4))
			{
				dyPrintf("bsxrp_input: incomplete message %02X\n", cs[3]);
				break;
			}

//			if((cs1[0]==0x7F) && (cs1[1]==0xFF) &&
//				(cs1[2]==0xFE) && (cs1[3]==0x02))
			if(BSXRP_TagEndMessageP(cs1))
			{
				BSXRP_HandleCodedMsg(ctx, cs+4, cs1);
				cs=cs1+4;
				continue;
			}

//			if((cs1[0]==0x7F) && (cs1[1]==0xFF) &&
//				(cs1[2]==0xFE) && (cs1[3]!=0x00))
			if(BSXRP_TagNotEscapeTagP(cs1))
			{
				dyPrintf("bsxrp_input: broken message %02X\n", cs[3]);
				cs=cs1;
				continue;
			}

			continue;
		}

//		if((cs[0]==0x7F) && (cs[1]==0xFF) &&
//			(cs[2]==0xFE) && (cs[3]>=0x04) && (cs[3]<=0x07))
		if(BSXRP_TagStartHuffmanTableP(cs))
		{
			cs1=cs+4;
			while(cs1<=(ce-4))
			{
//				if((cs1[0]==0x7F) && (cs1[1]==0xFF) &&
//					(cs1[2]==0xFE) && (cs1[3]!=0x00))
				if(BSXRP_TagNotEscapeTagP(cs1))
					break;
				cs1++;	//skip bytes
			}

			if(cs1>(ce-4))
			{
				dyPrintf("bsxrp_input: incomplete message %02X\n", cs[3]);
				break;
			}

//			if((cs1[0]==0x7F) && (cs1[1]==0xFF) &&
//				(cs1[2]==0xFE) && ( (cs1[3]==0x08) ||
//				((cs1[3]>=0x04) && (cs1[3]<=0x07)) ||
//				(cs1[3]==0x01))
//				)

			//Cannonical end of Huffman Table
			if(BSXRP_TagEndHuffmanTableP(cs1))
			{
				BSXRP_HandleHuffmanTable(ctx, cs+4, cs1, cs[3]-4);
				cs=cs1+4; continue;
			}

			//Alternate tags allowed to end Huffman Table
			//Does not eat ending tag
			if(BSXRP_TagAllowedEndHuffmanTableP(cs1))
			{
				BSXRP_HandleHuffmanTable(ctx, cs+4, cs1, cs[3]-4);
				cs=cs1; continue;
			}

//			if((cs1[0]==0x7F) && (cs1[1]==0xFF) &&
//				(cs1[2]==0xFE) && (cs1[3]!=0x00))
			if(BSXRP_TagNotEscapeTagP(cs1))
			{
				dyPrintf("bsxrp_input: broken message %02X\n", cs[3]);
				cs=cs1;
				continue;
			}

			continue;
		}
	}
	
	return(cs-(byte *)con->buf);
}

int bsxrp_closed(Meta0_Con *con)
{
	BSXRP_Context *ctx;
	byte *cs, *ce, *cs1;
	int i, j, k;
	
	ctx=(BSXRP_Context *)con->data;
	ctx->con=NULL;
	
	return(0);
}

int bsxrp_think(Meta0_Con *con)
{
	BSXRP_Context *ctx;

	ctx=(BSXRP_Context *)con->data;
	BSXRP_MultiplexThink(ctx);
	return(0);
}

int bsxrp_accept(Meta0_Con *con)
{
	BSXRP_Context *tmp;
	int i, j;

	tmp=BSXRP_AllocContext();
	con->data=tmp;
	tmp->con=con;

	dyPrintf("bsxrp_accept con=%p ctx=%p\n", con, tmp);

	j=-1;
	for(i=0; i<16; i++)
	{
		if(bsxrp_sv_accept_f[i])
		{
			j=bsxrp_sv_accept_f[i](tmp);
			if(j>=0)break;
		}
	}

	if(j>=0)
	{
		BSXRP_SendStreamInit(tmp);
		return(1);
	}
	
	return(-1);
}

BGBNET_API int BSXRP_RegisterAccept(int (*fcn)(BSXRP_Context *ctx))
{
	int i;

	for(i=0; i<16; i++)
	{
		if(!bsxrp_sv_accept_f[i])
		{
			bsxrp_sv_accept_f[i]=fcn;
			break;
		}
	}
	return(0);
}

BGBNET_API int BSXRP_Init()
{
	static int init;

	if(init)return(0);
	init=1;

	NET_Init();

	gc_printf("BSXRP Init\n");

//	bsxrp_cl_inf=Meta0_CreateClientPort("bsxrp-client");
	bsxrp_cl_inf=Meta0_CreatePort(BSXRP_PORT, "bsxrp-server");
	bsxrp_cl_inf->new_con=&bsxrp_accept;
	bsxrp_cl_inf->input=&bsxrp_input;
	bsxrp_cl_inf->closed=&bsxrp_closed;
	bsxrp_cl_inf->think=&bsxrp_think;

	bsxrp_cl_iface=NET_CreateInterface(NULL);
	bsxrp_cl_iface->poll=&bsxrp_poll;

	return(0);
}
