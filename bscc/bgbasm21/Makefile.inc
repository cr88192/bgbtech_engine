OBJS_FRONT = \
	front/basm_api$(OBJSUF) \
	front/basm_rand$(OBJSUF) \
	front/basm_callsig$(OBJSUF) \
	front/basm_vfs$(OBJSUF) \
	front/basm_xcall$(OBJSUF) \
	front/basm_xc_sysv$(OBJSUF) \
	front/basm_xc_w64$(OBJSUF) \
	front/basm_xmeta$(OBJSUF) \
	front/basm_unwind$(OBJSUF) \
	front/basm_preproc$(OBJSUF)

OBJS_ASM = \
	asm/basm_core$(OBJSUF) \
	asm/basm_parse$(OBJSUF) \
	asm/basm_insns_x86$(OBJSUF) \
	asm/basm_insns_arm$(OBJSUF) \
	asm/basm_insns_thumb$(OBJSUF) \
	asm/basm_disasm$(OBJSUF) \
	asm/basm_coff$(OBJSUF)

OBJS_LINK = \
	link/link_context$(OBJSUF) \
	link/link_core$(OBJSUF) \
	link/link_thunk$(OBJSUF) \
	link/link_lnx$(OBJSUF) \
	link/link_nacl$(OBJSUF) \
	link/link_w32$(OBJSUF) \
	link/link_vfs$(OBJSUF) \
	link/bldr_coff$(OBJSUF) \
	link/bldr_elf$(OBJSUF)

OBJS_ASMX = \
	asmx/basmx_core$(OBJSUF)

SRCS_FRONT=$(OBJS_FRONT:$(OBJSUF)=.c)
SRCS_ASM=$(OBJS_ASM:$(OBJSUF)=.c)
SRCS_LINK=$(OBJS_LINK:$(OBJSUF)=.c)
SRCS_ASMX=$(OBJS_ASMX:$(OBJSUF)=.c)

OBJS = \
	$(OBJS_FRONT)	\
	$(OBJS_ASM)	\
	$(OBJS_LINK)	\
	$(OBJS_ASMX)	\

SRCS = \
	$(SRCS_FRONT)	\
	$(SRCS_ASM)	\
	$(SRCS_LINK)	\
	$(SRCS_ASMX)	\


DEP_HDRS=\
	include/bgbasm_x86ops.h \
	include/bgbasm_armops.h \
	include/bgbasm_thumbops.h \
	include/bgbasm_auto_front.h \
	include/bgbasm_auto_asm.h \
	include/bgbasm_auto_link.h \
	include/bgbasm_auto2_asm.h \
	include/bgbasm_autoi_front.h \
	include/bgbasm_autoi_asm.h \
	include/bgbasm_autoi_link.h \
	\
	include/bgbasm_auto_asmx.h \
	include/bgbasm_autoi_asmx.h \

all: libs apps

FORCE:

# bgbasm$(LIBSUF)

libs: $(DLLPF)bgbasm$(DLLSUF) 
apps: basmtst$(EXESUF) basmtst2$(EXESUF) basmtst3$(EXESUF)

includes: $(DEP_HDRS)

include/bgbasm_auto_front.h: autohead$(BINSUF) $(SRCS_FRONT)
	./autohead$(BINSUF) -apionly BASM_API $(SRCS_FRONT) > \
		include/bgbasm_auto_front.h
include/bgbasm_auto_asm.h: autohead$(BINSUF) $(SRCS_ASM)
	./autohead$(BINSUF) -apionly BASM_API $(SRCS_ASM) > \
		include/bgbasm_auto_asm.h
include/bgbasm_auto_link.h: autohead$(BINSUF) $(SRCS_LINK)
	./autohead$(BINSUF) -apionly BASM_API $(SRCS_LINK) > \
		include/bgbasm_auto_link.h

include/bgbasm_auto2_asm.h: autohead$(BINSUF) $(SRCS_ASM)
	./autohead$(BINSUF) -apionly BASM2_API $(SRCS_ASM) > \
		include/bgbasm_auto2_asm.h

include/bgbasm_autoi_front.h: autohead$(BINSUF) $(SRCS_FRONT)
	./autohead$(BINSUF) -noapi BASM_API $(SRCS_FRONT) > \
		include/bgbasm_autoi_front.h
include/bgbasm_autoi_asm.h: autohead$(BINSUF) $(SRCS_ASM)
	./autohead$(BINSUF) -noapi BASM_API $(SRCS_ASM) > \
		include/bgbasm_autoi_asm.h
include/bgbasm_autoi_link.h: autohead$(BINSUF) $(SRCS_LINK)
	./autohead$(BINSUF) -noapi BASM_API $(SRCS_LINK) > \
		include/bgbasm_autoi_link.h

include/bgbasm_auto_asmx.h: autohead$(BINSUF) $(SRCS_ASMX)
	./autohead$(BINSUF) -apionly BASMX_API $(SRCS_ASMX) > \
		include/bgbasm_auto_asmx.h
include/bgbasm_autoi_asmx.h: autohead$(BINSUF) $(SRCS_ASMX)
	./autohead$(BINSUF) -noapi BASMX_API $(SRCS_ASMX) > \
		include/bgbasm_autoi_asmx.h

include/bgbasm_x86ops.h: genx86$(BINSUF) basm_insns_x86.txt
	./genx86$(BINSUF) -Mx86

include/bgbasm_armops.h: genx86$(BINSUF) basm_insns_arm.txt
	./genx86$(BINSUF) -Marm
include/bgbasm_thumbops.h: genx86$(BINSUF) basm_insns_thumb.txt
	./genx86$(BINSUF) -Mthumb

$(DLLPF)bgbasm$(DLLSUF): $(DEP_HDRS) $(SRCS)
	$(CC_OUTDLL)$(DLLPF)bgbasm$(DLLSUF) $(SRCS) \
		$(DLL_CFLAGS) $(DLL_LDFLAGS)

# bgbasm$(DLLSUF): $(DEP_HDRS) $(OBJS)
#	cl /LD /Febgbasm$(DLLSUF) $(OBJS) $(LDFLAGS)

# bgbasm$(LIBSUF): $(DEP_HDRS) $(OBJS)
#	lib /out:bgbasm$(LIBSUF) $(OBJS)

autohead$(BINSUF): autohead.c
	$(CC_OUTTOOL)autohead$(BINSUF) autohead.c

genx86$(BINSUF): gen_x86_4.c
	$(CC_OUTTOOL)genx86$(BINSUF) gen_x86_4.c $(CFLAGS)

basmtst$(EXESUF): basmtst.c $(DEP_HDRS) libs
	$(CC_OUTEXE)basmtst$(EXESUF) basmtst.c $(CFLAGS) $(LDFLAGS)

basmtst2$(EXESUF): basmtst2.c $(DEP_HDRS) libs
	$(CC_OUTEXE)basmtst2$(EXESUF) basmtst2.c $(CFLAGS) $(LDFLAGS)

basmtst3$(EXESUF): basmtst3.c $(DEP_HDRS) libs
	$(CC_OUTEXE)basmtst3$(EXESUF) basmtst3.c $(CFLAGS) $(LDFLAGS)

%$(OBJSUF): %.c
	$(CC_OUTOBJ)$@ $< $(CFLAGS)

clean:

clean2:

