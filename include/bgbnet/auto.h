//AHSRC:base/sock_us/w32_net.c
BGBNET_API VADDR *NET_LookupHost(char *name);
BGBNET_API VADDR *NET_LookupHost2(char *name, char *serv, int proto);
BGBNET_API VADDR *NET_DecodeHostnamePort(char *name, int defport);
BGBNET_API VADDR *NET_DecodeHostname(char *name);
//AHSRC:base/sock_us/w32_guid.c
//AHSRC:base/sock_us/w32_tcpip.c
BGBNET_API int TCP_GetSocketAddr(int socket, VADDR *addrbuf);
BGBNET_API VFILE *TCP_WrapSocket(int sock);
BGBNET_API VFILE *TCP_OpenSocket(int port);
BGBNET_API VFILE *TCP_OpenListen(int port);
BGBNET_API VFILE *TCP_OpenListen_IPv6(int port);
BGBNET_API VFILE *TCP_OpenConnect(VADDR *targ);
//AHSRC:base/sock_us/w32_udpip.c
BGBNET_API int UDP_GetSocketAddr(int socket, VADDR *addrbuf);
BGBNET_API VFILE *UDP_OpenSocket(int port);
//AHSRC:base/sock_us/lnx_net.c
BGBNET_API VADDR *NET_LookupHost(char *name);
BGBNET_API VADDR *NET_LookupHost2(char *name, char *serv, int proto);
BGBNET_API VADDR *NET_DecodeHostnamePort(char *name, int defport);
BGBNET_API VADDR *NET_DecodeHostname(char *name);
//AHSRC:base/sock_us/lnx_guid.c
//AHSRC:base/sock_us/lnx_tcpip.c
BGBNET_API int TCP_GetSocketAddr(int socket, VADDR *addrbuf);
BGBNET_API VFILE *TCP_WrapSocket(int sock);
BGBNET_API VFILE *TCP_WrapSocket(int sock);
BGBNET_API VFILE *TCP_OpenSocket(int port);
BGBNET_API VFILE *TCP_OpenListen(int port);
BGBNET_API VFILE *TCP_OpenListen_IPv6(int port);
BGBNET_API VFILE *TCP_OpenConnect(VADDR *targ);
//AHSRC:base/sock_us/lnx_udpip.c
BGBNET_API int UDP_GetSocketAddr(int socket, VADDR *addrbuf);
BGBNET_API VFILE *UDP_OpenSocket(int socknum);
BGBNET_API VADDR *NET_LookupHost(char *name);
//AHSRC:base/sock_us/nacl_net.c
BGBNET_API VADDR *NET_LookupHost(char *name);
BGBNET_API VADDR *NET_DecodeHostnamePort(char *name, int defport);
BGBNET_API VADDR *NET_DecodeHostname(char *name);
BGBNET_API int TCP_GetSocketAddr(int socket, VADDR *addrbuf);
BGBNET_API VFILE *TCP_WrapSocket(int sock);
BGBNET_API VFILE *TCP_WrapSocket(int sock);
BGBNET_API VFILE *TCP_OpenSocket(int port);
BGBNET_API VFILE *TCP_OpenListen(int port);
BGBNET_API VFILE *TCP_OpenConnect(VADDR *targ);
BGBNET_API int UDP_GetSocketAddr(int socket, VADDR *addrbuf);
//AHSRC:base/net.c
BGBNET_API int NET_AddrEqual(VADDR *a, VADDR *b);
BGBNET_API int NET_Poll();
BGBNET_API int NET_Init();
//AHSRC:base/rpc1.c
//AHSRC:base/objlst.c
//AHSRC:base/meta0/meta0_base.c
//AHSRC:base/http/http_sv.c
//AHSRC:base/http/http_cl.c
//AHSRC:base/http/mime.c
//AHSRC:base/xmpp/xmpp.c
//AHSRC:base/xmpp/xmpp_util.c
//AHSRC:base/xmpp/jep30.c
//AHSRC:base/xmpp/jep45.c
//AHSRC:base/bsxrp/bsxrp_bitstream.c
//AHSRC:base/bsxrp/bsxrp_huffman.c
//AHSRC:base/bsxrp/bsxrp_lz77.c
//AHSRC:base/bsxrp/bsxrp_message.c
BGBNET_API void BSXRP_FreeContext(BSXRP_Context *ctx);
BGBNET_API BSXRP_Context *BSXRP_Connect(VADDR *addr);
BGBNET_API BSXRP_Context *BSXRP_ConnectName(char *host);
BGBNET_API void BSXRP_Close(BSXRP_Context *ctx);
BGBNET_API int BSXRP_ConnectedP(BSXRP_Context *ctx);
BGBNET_API int BSXRP_SendMessage(BSXRP_Context *ctx, dyt msg);
BGBNET_API dyt BSXRP_GetMessage(BSXRP_Context *ctx);
BGBNET_API int BSXRP_RegisterAccept(int (*fcn)(BSXRP_Context *ctx));
BGBNET_API int BSXRP_Init();
//AHSRC:base/bsxrp/bsxrp_msgvalue.c
//AHSRC:base/bsxrp/bsxrp_muxbuf.c
BGBNET_API BSXRP_MuxBuffer *BSXRP_LookupMuxBufferRX(BSXRP_Context *ctx, int muxid);
BGBNET_API BSXRP_MuxBuffer *BSXRP_LookupMuxBufferTX(BSXRP_Context *ctx, int muxid);
BGBNET_API BSXRP_MuxBuffer *BSXRP_GetMuxBufferRX(BSXRP_Context *ctx, int muxid);
BGBNET_API BSXRP_MuxBuffer *BSXRP_GetMuxBufferTX(BSXRP_Context *ctx, int muxid);
BGBNET_API int BSXRP_SendMultiplexMessage(BSXRP_Context *ctx,int tag, int muxid, int offset, byte *ibuf, int isz);
