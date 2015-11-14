//AHSRC:base/sock_us/w32_net.c
char *ipv4tostr(unsigned long addr);
char *ipv6tostr(byte *addr);
char *NET_Addr2Str(VADDR *addr);
VADDR *NET_Str2Addr(char *str, int proto);
int NET_InitLow (void);
//AHSRC:base/sock_us/w32_guid.c
int NET_CreateGuid(VGUID *buf);
int NET_GuidEqualP(VGUID *a, VGUID *b);
char *NET_Guid2String(VGUID *guid);
char *NET_String2Guid(VGUID *guid, char *s);
//AHSRC:base/sock_us/w32_tcpip.c
int __TCP_OpenSocket (int port);
int __TCP_OpenSocket_IPv6 (int port);
int tcpsock_write(void *msg, int s1, int s2, VFILE *sock);
int tcpsock_flush(VFILE *sock);
int tcpsock_ioctl(VFILE *fd, int req, void *arg);
int tcpsock_close(VFILE *sock);
VADDR *tcpsock_getaddr(VFILE *sock);
//AHSRC:base/sock_us/w32_udpip.c
char *ipv4tostr(unsigned long addr);
char *ipv6tostr(byte addr[16]);
int NET_InitLow (void);
int __UDP_OpenSocket (int port);
int __UDP_OpenSocket (int port);
int udpsock_get(void *buf, int lim, VADDR *from, VFILE *sock);
int udpsock_send(VFILE *sock, VADDR *targ, void *msg, int len, int flags);
VADDR *udpsock_getaddr(VFILE *sock);
//AHSRC:base/sock_us/lnx_net.c
char *ipv4tostr(unsigned long addr);
char *ipv6tostr(byte *addr);
char *NET_Addr2Str(VADDR *addr);
VADDR *NET_Str2Addr(char *str, int proto);
int NET_InitLow (void);
//AHSRC:base/sock_us/lnx_guid.c
int NET_CreateGuid(VGUID *buf);
int NET_GuidEqualP(VGUID *a, VGUID *b);
char *NET_Guid2String(VGUID *guid);
char *NET_String2Guid(VGUID *guid, char *s);
//AHSRC:base/sock_us/lnx_tcpip.c
int __TCP_OpenSocket (int port);
int __TCP_OpenSocket_IPv6 (int port);
int tcpsock_write(void *msg, int s1, int s2, VFILE *sock);
int tcpsock_flush(VFILE *sock);
int tcpsock_ioctls(VFILE *fd, void **arg);
int tcpsock_ioctl(VFILE *fd, int req, void *arg);
int tcpsock_close(VFILE *sock);
VADDR *tcpsock_getaddr(VFILE *sock);
//AHSRC:base/sock_us/lnx_udpip.c
char *ipv4tostr(unsigned long addr);
char *ipv6tostr(byte addr[16]);
int NET_InitLow (void);
int __UDP_OpenSocket (int port);
int __UDP_OpenSocket (int port);
int udpsock_get(void *buf, int lim, VADDR *from, VFILE *sock);
int udpsock_send(VFILE *sock, VADDR *targ, void *msg, int len, int flags);
VADDR *udpsock_getaddr(VFILE *sock);
//AHSRC:base/sock_us/nacl_net.c
int NET_CreateGuid(VGUID *buf);
int NET_GuidEqualP(VGUID *a, VGUID *b);
char *NET_Guid2String(VGUID *guid);
char *NET_String2Guid(VGUID *guid, char *s);
char *ipv4tostr(unsigned long addr);
char *ipv6tostr(byte *addr);
char *NET_Addr2Str(VADDR *addr);
VADDR *NET_Str2Addr(char *str, int proto);
int NET_InitLow (void);
int tcpsock_read(void *buf, int s1, int s2, VFILE *sock);
int tcpsock_write(void *msg, int s1, int s2, VFILE *sock);
int tcpsock_flush(VFILE *sock);
int tcpsock_ioctl(VFILE *fd, int req, void *arg);
int tcpsock_close(VFILE *sock);
VADDR *tcpsock_getaddr(VFILE *sock);
//AHSRC:base/net.c
int NET_InitLow ();
char *Var_GetString(char *name);
double Var_GetValue(char *name);
//AHSRC:base/rpc1.c
int NET_ExportFunc(char *name, void *(*func)());
int NET_ExportFunc2(char *name, int args, void *(*func)(void *data), void *data);
NET_Interface *NET_CreateInterface(char *name);
NET_Interface *NET_FindInterface(char *name);
NET_Export *NET_FindExport(char *name);
NET_Reference *NET_DecodeURL(char *url);
char *NET_EncodeURL(NET_Reference *ref);
void *NET_CallReference(NET_Reference *ref, void **args);
void *NET_CallReferenceFunc(NET_Reference *ref, char *func, void **args);
void *NET_FetchReference(NET_Reference *ref);
void *NET_GenLink(char *scheme, char *exp);
void *NET_ConvLink(void *link);
void *NET_ApplyFunc1(void *(*f)(void *x0, ...), int i, void **args);
void *NET_ApplyFunc2(void *(*f)(void *data, ...), int i, void *data, void **args);
void *NET_CallExport(char *name, void **args);
int NETAPI_Poll();
int NETAPI_Init();
//AHSRC:base/objlst.c
ObjLst_ObjExport *ObjLst_LookupExport(char *name);
int ObjLst_ExportObj(char *name, void *obj);
void *ObjLst_FetchObj(char *name);
void *ObjLst_WrapObj(long o, char *type);
long ObjLst_WrapObjID(void *obj);
char *ObjLst_WrapObjType(void *obj);
//AHSRC:base/meta0/meta0_base.c
Meta0_Con *Meta0_NewCon();
Meta0_PortInfo *Meta0_NewPortInfo();
Meta0_Con *Meta0_Connect(Meta0_PortInfo *inf, VADDR *addr);
int Meta0_CloseConnection(Meta0_Con *con);
Meta0_Con *Meta0_FindCon(VADDR *addr);
int Meta0_HandleConnection(Meta0_Con *con);
int Meta0_PollPort(Meta0_PortInfo *inf);
int Meta0_Poll();
Meta0_PortInfo *Meta0_CreatePort(int port, char *name);
Meta0_PortInfo *Meta0_CreateClientPort(char *name);
int Meta0_Init();
//AHSRC:base/http/http_sv.c
HTTP_Con *HttpNode_NewCon();
int HttpNode_SendContents(HTTP_Con *con, byte *type, byte *msg, int len);
int HttpNode_SendResponse(HTTP_Con *con, byte *msg, int len);
char *HttpNode_CodeName(int code);
int HttpNode_SendContentsCode(HTTP_Con *con, int code, byte *type, byte *msg, int len);
int HttpNode_SendDir(HTTP_Con *con, char *name);
int HttpNode_SendPage(HTTP_Con *con);
int HttpNode_SendPageHead(HTTP_Con *con);
int HttpNode_Send404(HTTP_Con *con, byte *type, byte *msg, int len);
int HttpNode_HandleLine(HTTP_Con *con, char *lb);
int HttpNode_HandleMessage(HTTP_Con *con, char *msg, int len);
int HttpNode_HandleConnection(HTTP_Con *con);
HTTP_Resource *HttpNode_NewResource(char *base);
int HttpNode_Poll();
int HttpNode_GetPort();
int HttpNode_Init();
//AHSRC:base/http/http_cl.c
NET_Reference *httpnode_decode_url(char *s);
char *httpnode_encode_url(NET_Reference *ref);
void *httpnode_rpc_call(NET_Reference *ref, void **args);
void *httpnode_fetch(NET_Reference *ref);
int httpnode_input_stat(Meta0_Con *con, char *buf);
int httpnode_input_line(Meta0_Con *con, char *buf);
int httpnode_input(Meta0_Con *con);
NET_Reference *HttpNode_DecodeURL(char *s);
void *HttpNode_Post(NET_Reference *ref, char *type, void *msg, int len);
int HttpNode_InitCl();
//AHSRC:base/http/mime.c
int HttpNode_EncodeMime(byte *o, byte *i, int sz);
int HttpNode_DecodeMime(byte *o, byte *i, int sz);
char *HttpNode_MimeFromExt(char *ext);
char *HttpNode_MimeFromName(char *name);
int HttpNode_MimeInit();
//AHSRC:base/xmpp/xmpp.c
XMPP_Interface *XMPP_CreateInterface(char *xmlns);
int XMPP_DispatchMessage(Meta0_Con *con, dyxNode *node);
int XMPP_DispatchPresence(Meta0_Con *con, dyxNode *node);
int XMPP_DispatchTextMessage(char *from, char *subj, char *body);
int XMPP_DispatchIQ(Meta0_Con *con, dyxNode *node);
int xmpp_handle_auth(Meta0_Con *con, dyxNode *node);
int xmpp_handle_message(Meta0_Con *con, dyxNode *node);
int xmpp_handle_form(Meta0_Con *con, dyxNode *node);
int xmpp_poll();
int xmpp_input(Meta0_Con *con);
int xmpp_closed(Meta0_Con *con);
VADDR *XMPP_GetHostAddr(char *host, int defport);
int XMPP_Connect(char *server);
int XMPP_Disconnect();
int XMPP_Login(char *user, char *passwd, char *rsrc);
int XMPP_Message(char *to, char *from, char *subj, char *body);
int XMPP_SendIQ(char *to, char *type, char *tag, dyxNode *body);
int XMPP_SendMessage(char *to, char *subj, char *body);
int XMPP_SendMessageChat(char *to, char *body);
int XMPP_SendStanza(dyxNode *body);
int XMPP_SendPresence(char *to, char *type, dyxNode *body);
int XMPP_SendMessageStanza(char *to, char *type, dyxNode *body);
int XMPP_Init();
//AHSRC:base/xmpp/xmpp_util.c
dyxNode *XMPP_CheckResults(char *tag);
int XMPP_AddResults(dyxNode *node);
char *XMPP_GenLocal();
//AHSRC:base/xmpp/jep30.c
int xmpp_jep30_items_iq(Meta0_Con *con, dyxNode *node);
dyxNode *XMPP_JEP30_QueryItems(char *to, char *node);
int xmpp_jep30_info_iq(Meta0_Con *con, dyxNode *node);
dyxNode *XMPP_JEP30_QueryInfo(char *to, char *node);
int XMPP_JEP30_Init();
//AHSRC:base/xmpp/jep45.c
XMPP_MucRoom *XMPP_MucRoom_NewRoom(char *room, char *host, char *nick);
XMPP_MucRoom *XMPP_MucRoom_FindJid(char *jid);
int XMPP_MucRoom_SetPresence(XMPP_MucRoom *room, char *user, char *status);
XMPP_MucRoom *XMPP_MucRoom_Join(char *room, char *host, char *nick);
int XMPP_MucRoom_Leave(XMPP_MucRoom *room);
char *XMPP_MucRoom_UserJid(XMPP_MucRoom *room, char *nick);
int XMPP_MucRoom_SendMessage(XMPP_MucRoom *room, dyxNode *body);
int XMPP_MucRoom_SendMessageText(XMPP_MucRoom *room, char *str);
int xmpp_jep45_message(Meta0_Con *con, dyxNode *node);
int xmpp_jep45_presence(Meta0_Con *con, dyxNode *node);
int XMPP_MucRoom_Init();
//AHSRC:base/bsxrp/bsxrp_bitstream.c
void BSXRP_SetupBitstreamRead(BSXRP_StreamContext *ctx,byte *css, byte *cse);
void BSXRP_SetupBitstreamWrite(BSXRP_StreamContext *ctx,byte *cts, byte *cte);
byte *BSXRP_FinishBitstreamRead(BSXRP_StreamContext *ctx);
byte *BSXRP_FinishBitstreamWrite(BSXRP_StreamContext *ctx);
void BSXRP_EmitCodedByte(BSXRP_StreamContext *ctx, int i);
int BSXRP_ReadCodedByte(BSXRP_StreamContext *ctx);
void BSXRP_WriteBit(BSXRP_StreamContext *ctx, int i);
void BSXRP_Write2Bits(BSXRP_StreamContext *ctx, int i);
void BSXRP_Write3Bits(BSXRP_StreamContext *ctx, int i);
void BSXRP_Write4Bits(BSXRP_StreamContext *ctx, int i);
void BSXRP_Write5Bits(BSXRP_StreamContext *ctx, int i);
void BSXRP_WriteNBits(BSXRP_StreamContext *ctx, int i, int n);
void BSXRP_WriteNBitsL(BSXRP_StreamContext *ctx, u64 val, int n);
void BSXRP_FlushBits(BSXRP_StreamContext *ctx);
void BSXRP_WriteRice(BSXRP_StreamContext *ctx, int v, int n);
int BSXRP_ReadBit(BSXRP_StreamContext *ctx);
int BSXRP_Read2Bits(BSXRP_StreamContext *ctx);
int BSXRP_Read3Bits(BSXRP_StreamContext *ctx);
int BSXRP_Read4Bits(BSXRP_StreamContext *ctx);
int BSXRP_Read5Bits(BSXRP_StreamContext *ctx);
int BSXRP_ReadNBits(BSXRP_StreamContext *ctx, int n);
u64 BSXRP_ReadNBitsL(BSXRP_StreamContext *ctx, int n);
void BSXRP_SkipNBits(BSXRP_StreamContext *ctx, int n);
int BSXRP_ReadRice(BSXRP_StreamContext *ctx, int n);
int BSXRP_PeekWord(BSXRP_StreamContext *ctx);
void BSXRP_AlignByte(BSXRP_StreamContext *ctx);
int BSXRP_ReadAlignedByte(BSXRP_StreamContext *ctx);
int BSXRP_ReadAlignedWord(BSXRP_StreamContext *ctx);
//AHSRC:base/bsxrp/bsxrp_huffman.c
int BSXRP_BalanceTree_r(short *nodes, short *nlen, int root, int h, int ml);
void BSXRP_CalcLengths_r(short *nodes, byte *cl, int root, int h);
void BSXRP_BuildLengths(int *stat, int nc, byte *cl, int ml);
int BSXRP_TransposeByte(int v);
int BSXRP_TransposeWord(int v);
int BSXRP_SetupTable(byte *cl, int ncl,u16 *tc, u16 *tm, byte *tl, u16 *ti, u16 *tn);
void BSXRP_HuffmanLengthSanityAdjust(BSXRP_StreamContext *ctx, int tab);
void BSXRP_EncodeSymbol(BSXRP_StreamContext *ctx, int tab, int v);
int BSXRP_DecodeSymbol(BSXRP_StreamContext *ctx, int tab);
int BSXRP_DecodeHuffTableFixed(BSXRP_StreamContext *ctx,int tab, int tn);
int BSXRP_DecodeHufftabSymbol(BSXRP_StreamContext *ctx);
void BSXRP_EncodeHufftabSymbol(BSXRP_StreamContext *ctx, int v);
void BSXRP_StatLengths(BSXRP_StreamContext *ctx,byte *cl, int nc, int *stat);
int BSXRP_DecodeCodeLengths(BSXRP_StreamContext *ctx, byte *cl, int ncl);
void BSXRP_EncodeCodeLengths(BSXRP_StreamContext *ctx, byte *cl, int nc);
int BSXRP_DecodeHuffTable(BSXRP_StreamContext *ctx, int tab);
int BSXRP_EncodeHuffTable(BSXRP_StreamContext *ctx,int tab, byte *cl, int nc);
int BSXRP_CheckEncodeHuffTable(BSXRP_StreamContext *ctx,int tab, byte *cl, int nc);
void BSXRP_EncodeUInteger(BSXRP_StreamContext *ctx, u64 val);
u64 BSXRP_DecodeUInteger(BSXRP_StreamContext *ctx);
void BSXRP_EncodeSInteger(BSXRP_StreamContext *ctx, s64 val);
s64 BSXRP_DecodeSInteger(BSXRP_StreamContext *ctx);
int BSXRP_StatUIntPrefix(BSXRP_StreamContext *ctx, u64 val);
int BSXRP_StatSIntPrefix(BSXRP_StreamContext *ctx, u64 val);
void BSXRP_EncodeDistance(BSXRP_StreamContext *ctx, int val);
int BSXRP_DecodeDistance(BSXRP_StreamContext *ctx);
void BSXRP_EncodeValueIndex(BSXRP_StreamContext *ctx, int val);
u64 BSXRP_DecodeValueIndex(BSXRP_StreamContext *ctx, int pf);
int BSXRP_DecodeDistance(BSXRP_StreamContext *ctx);
//AHSRC:base/bsxrp/bsxrp_lz77.c
void BSXRP_UpdateWindowByte(BSXRP_StreamContext *ctx, byte *s);
void BSXRP_UpdateWindowString(BSXRP_StreamContext *ctx, byte *s, int l);
int BSXRP_LookupString(BSXRP_StreamContext *ctx,byte *cs, byte *ce, int *rl, int *rd);
int BSXRP_LZCompressBuffer(BSXRP_StreamContext *ctx,byte *ibuf, u16 *obuf, int isz, int osz);
void BSXRP_StatLZRun(BSXRP_StreamContext *ctx,u16 *cs, int *lstat, int *dstat);
void BSXRP_StatLZBuffer(BSXRP_StreamContext *ctx,u16 *tbuf, int n, int *cstat, int *lstat, int *dstat);
void BSXRP_EncodeLZRun(BSXRP_StreamContext *ctx, u16 *cs);
void BSXRP_EncodeLZBuffer(BSXRP_StreamContext *ctx,u16 *tbuf, int n);
int BSXRP_DecodeRun(BSXRP_StreamContext *ctx, int sym);
int BSXRP_DecodeBlockData(BSXRP_StreamContext *ctx);
//AHSRC:base/bsxrp/bsxrp_message.c
BSXRP_Context *BSXRP_AllocContext();
void BSXRP_SendStreamInit(BSXRP_Context *ctx);
void BSXRP_SendStreamMessage(BSXRP_Context *ctx, dyt msg);
int BSXRP_SendStreamDeflatedMessage(BSXRP_Context *ctx, dyt msg);
int bsxrp_poll();
void BSXRP_HandleDeflatedMsg(BSXRP_Context *ctx, byte *css, byte *cse,int osz);
void BSXRP_HandleCodedMsg(BSXRP_Context *ctx, byte *css, byte *cse);
void BSXRP_HandleHuffmanTable(BSXRP_Context *ctx,byte *css, byte *cse, int tab);
int BSXRP_TagNotEscapeTagP(byte *cs);
int BSXRP_TagEscapeTagP(byte *cs);
int BSXRP_TagResetP(byte *cs);
int BSXRP_TagStartMessageDeflateP(byte *cs);
int BSXRP_TagEndMessageDeflateP(byte *cs);
int BSXRP_TagStartHuffmanTableP(byte *cs);
int BSXRP_TagEndHuffmanTableP(byte *cs);
int BSXRP_TagAllowedEndHuffmanTableP(byte *cs);
int BSXRP_TagStartMessageP(byte *cs);
int BSXRP_TagEndMessageP(byte *cs);
int bsxrp_input(Meta0_Con *con);
int bsxrp_closed(Meta0_Con *con);
int bsxrp_think(Meta0_Con *con);
int bsxrp_accept(Meta0_Con *con);
//AHSRC:base/bsxrp/bsxrp_msgvalue.c
BSXRP_StreamContext *BSXRP_AllocStreamContext();
BSXRP_StreamContext *BSXRP_CreateStreamContext();
dyt BSXRP_ShiftBackMRU(BSXRP_StreamContext *ctx, int idx);
dyt BSXRP_ShiftIndexMTF(BSXRP_StreamContext *ctx, int idx);
int BSXRP_IndexValueMRU(BSXRP_StreamContext *ctx, dyt val);
void BSXRP_AddValueMRU(BSXRP_StreamContext *ctx, dyt val);
void BSXRP_EncodeValue(BSXRP_StreamContext *ctx, dyt val);
void BSXRP_ValueEncodeSymbol(BSXRP_StreamContext *ctx, int sym);
void BSXRP_ValueEncodeIndex(BSXRP_StreamContext *ctx, int idx);
void BSXRP_ValueEncodeInteger(BSXRP_StreamContext *ctx, s64 val);
void BSXRP_ValueEncodeReal(BSXRP_StreamContext *ctx, double val);
void BSXRP_ValueEncodeReal32(BSXRP_StreamContext *ctx, float val);
void BSXRP_ValueEncodeString(BSXRP_StreamContext *ctx, char *str);
void BSXRP_ValueEncodeByteData(BSXRP_StreamContext *ctx,byte *buf, int sz);
void BSXRP_EncodeValueList(BSXRP_StreamContext *ctx, dyt lst);
void BSXRP_EncodeValueArray(BSXRP_StreamContext *ctx, dyt val);
void BSXRP_EncodeValueByteArray(BSXRP_StreamContext *ctx, dyt val);
void BSXRP_EncodeValueByteArrayNoMRU(BSXRP_StreamContext *ctx, dyt val);
void BSXRP_EncodeValueBuffer(BSXRP_StreamContext *ctx, dyt val);
byte *BSXRP_EmitTag(byte *ct, int tag);
int BSXRP_MessageEncodeValueBuffer(BSXRP_StreamContext *ctx, dyt val, byte *obuf, int osz);
double BSXRP_DecodeMessageReal(BSXRP_StreamContext *ctx);
char *BSXRP_DecodeMessageStringData(BSXRP_StreamContext *ctx);
dyt BSXRP_DecodeMessageByteArray(BSXRP_StreamContext *ctx);
dyt BSXRP_DecodeMessageList(BSXRP_StreamContext *ctx);
dyt BSXRP_DecodeMessageDotList(BSXRP_StreamContext *ctx);
dyt BSXRP_DecodeMessageArray(BSXRP_StreamContext *ctx);
dyt BSXRP_DecodeMessageValue(BSXRP_StreamContext *ctx);
//AHSRC:base/bsxrp/bsxrp_muxbuf.c
void BSXRP_DecodeMultiplexData(BSXRP_Context *ctx,BSXRP_StreamContext *sctx);
int BSXRP_BeginMessageEncodeBuffer(BSXRP_StreamContext *ctx);
int BSXRP_EndMessageEncodeBuffer(BSXRP_StreamContext *ctx,int minitag, byte *obuf, int osz);
int BSXRP_MultiplexEncodeChunkBuffer(BSXRP_StreamContext *ctx, int tag, int muxid, int offset, byte *ibuf, int isz, byte *obuf, int osz);
void BSXRP_MultiplexThink(BSXRP_Context *ctx);
