#	base/memory2_2$(OBJSUF) \
#	base/int128$(OBJSUF) \

#	base/dyll_frame$(OBJSUF) \
#	base/dyll_frame_x86$(OBJSUF) \
#	base/dyll_frame_x64$(OBJSUF) \
#	base/dyc_rcit$(OBJSUF) \

OBJS=\
	base/sock_us/w32_net$(OBJSUF) \
	base/sock_us/w32_guid$(OBJSUF) \
	base/sock_us/w32_tcpip$(OBJSUF) \
	base/sock_us/w32_udpip$(OBJSUF) \
	\
	base/sock_us/lnx_net$(OBJSUF) \
	base/sock_us/lnx_guid$(OBJSUF) \
	base/sock_us/lnx_tcpip$(OBJSUF) \
	base/sock_us/lnx_udpip$(OBJSUF) \
	\
	base/sock_us/nacl_net$(OBJSUF) \
	\
	base/net$(OBJSUF) \
	base/rpc1$(OBJSUF) \
	base/objlst$(OBJSUF) \
	\
	base/meta0/meta0_base$(OBJSUF) \
	\
	base/http/http_sv$(OBJSUF) \
	base/http/http_cl$(OBJSUF) \
	base/http/mime$(OBJSUF) \
	\
	base/xmpp/xmpp$(OBJSUF) \
	base/xmpp/xmpp_util$(OBJSUF) \
	base/xmpp/jep30$(OBJSUF) \
	base/xmpp/jep45$(OBJSUF) \
	\
	base/bsxrp/bsxrp_bitstream$(OBJSUF) \
	base/bsxrp/bsxrp_huffman$(OBJSUF) \
	base/bsxrp/bsxrp_lz77$(OBJSUF) \
	base/bsxrp/bsxrp_message$(OBJSUF) \
	base/bsxrp/bsxrp_msgvalue$(OBJSUF) \
	base/bsxrp/bsxrp_muxbuf$(OBJSUF) \

SRCS=$(OBJS:$(OBJSUF)=.c)

DEP_HDRS = \
	include/bgbnet/auto.h \
	include/bgbnet/autoi.h

all: libs apps

FORCE:

libs: $(DLLPF)bgbnet$(DLLSUF)
apps:

# dytst0$(EXESUF) tst_class0$(EXESUF) gatst0$(EXESUF)

includes: $(DEP_HDRS)

include/bgbnet/auto.h: autohead$(BINSUF) $(SRCS)
	./autohead$(BINSUF) -apionly BGBNET_API $(SRCS) > include/bgbnet/auto.h
include/bgbnet/autoi.h: autohead$(BINSUF) $(SRCS)
	./autohead$(BINSUF) -noapi BGBNET_API $(SRCS) > include/bgbnet/autoi.h

%$(OBJSUF): %.c
	$(CC_OUTOBJ)$@ $< $(CFLAGS)

# bgbnet.lib: $(DEP_HDRS) $(OBJS)
#	lib /out:bgbnet.lib $(OBJS)

$(DLLPF)bgbnet$(DLLSUF): $(DEP_HDRS) $(SRCS)
	$(CC_OUTDLL)$(DLLPF)bgbnet$(DLLSUF) $(SRCS) $(DLL_CFLAGS) $(DLL_LDFLAGS)

autohead$(BINSUF): autohead.c
	$(CC_OUTTOOL)autohead$(BINSUF) autohead.c

# dytst0$(EXESUF): dytst0.c $(DEP_HDRS) libs
#	$(CC_OUTEXE)dytst0$(EXESUF) dytst0.c $(CFLAGS) $(LDFLAGS)

clean:
