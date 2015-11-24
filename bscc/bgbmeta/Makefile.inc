OBJS=\
	cparse/cparse_token$(OBJSUF) \
	cparse/cparse_misc$(OBJSUF) \
	cparse/cparse_type$(OBJSUF) \
	cparse/cparse_def$(OBJSUF) \
	cparse/cparse_expr2$(OBJSUF) \
	cparse/cparse_stmt$(OBJSUF) \
	cparse/preproc$(OBJSUF) \
	\
	jparse/jp_token2$(OBJSUF) \
	jparse/jp_misc$(OBJSUF) \
	jparse/jp_type$(OBJSUF) \
	jparse/jp_def$(OBJSUF) \
	jparse/jp_tyexpr$(OBJSUF) \
	jparse/jp_expr2$(OBJSUF) \
	jparse/jp_stmt$(OBJSUF) \
	\
	cc/cc_compile$(OBJSUF) \
	cc/cc_expr$(OBJSUF) \
	cc/cc_index$(OBJSUF) \
	cc/cc_reduce$(OBJSUF) \
	\
	cc/ccm_compile$(OBJSUF) \
	cc/cc_gast$(OBJSUF) \
	\
	cc/bs2cc_compile$(OBJSUF) \
	cc/bs2cc_expr$(OBJSUF) \
	cc/bs2cc_index$(OBJSUF) \
	\
	mm/cc_tokord$(OBJSUF) \
	\
	mm/ccx_node$(OBJSUF) \
	mm/ccx_print$(OBJSUF) \
	mm/ccx_parse$(OBJSUF) \
	mm/ccx_sbxe$(OBJSUF) \
	mm/cc_malloc$(OBJSUF) \

META_OBJS = \
	$(OBJS) \
	\
	mm/inflate$(OBJSUF) \
	mm/deflate$(OBJSUF) \
	mm/cc_exwad$(OBJSUF) \
	mm/ccl_context$(OBJSUF) \
	mm/ccl_coff$(OBJSUF) \
	mm/ccl_elf$(OBJSUF) \
	\
	mm/dyll_metapath$(OBJSUF) \
	mm/dyll_metabin$(OBJSUF) \
	bgbmeta$(OBJSUF)

DLL_OBJS = \
	$(OBJS) \
	cc_api$(OBJSUF) \
	cc_cache$(OBJSUF)
	

SRCS=$(OBJS:$(OBJSUF)=.c)
META_SRCS=$(META_OBJS:$(OBJSUF)=.c)
DLL_SRCS=$(DLL_OBJS:$(OBJSUF)=.c)

DEP_HDRS=\
	include/bgbcc_auto.h \
	include/bgbcc_autom.h

LIBS=$(DLLPF)bgbcc$(DLLSUF)
# APPS=bgbmeta$(BINSUF) bgbcc$(EXESUF)
# APPS=bgbmeta$(BINSUF)

all: libs apps

libs: $(LIBS) bgbmeta$(BINSUF)
apps: $(APPS)
includes: $(DEP_HDRS)


autohead$(BINSUF): autohead.c
	$(CC_OUTTOOL)autohead$(BINSUF) autohead.c

gen_ord$(BINSUF): gen_ord.c
	$(CC_OUTTOOL)gen_ord$(BINSUF) gen_ord.c

mm/cc_tokord.c: gen_ord$(BINSUF) bgbcc_token.txt
	./gen_ord$(BINSUF)

include/bgbcc_auto.h: autohead$(BINSUF) $(DLL_SRCS)
	./autohead$(BINSUF) -api BGBCC_API $(DLL_SRCS) > include/bgbcc_auto.h

include/bgbcc_autom.h: autohead$(BINSUF) $(META_SRCS)
	./autohead$(BINSUF) -api BGBCC_API $(META_SRCS) > include/bgbcc_autom.h

bgbmeta$(BINSUF): $(DEP_HDRS) $(META_SRCS)
	$(CC_OUTTOOL)bgbmeta$(BINSUF) $(META_SRCS) \
		$(CFLAGS_META) $(LDFLAGS_META)

$(DLLPF)bgbcc$(DLLSUF): $(DEP_HDRS) $(DLL_SRCS)
	$(CC_OUTDLL)$(DLLPF)bgbcc$(DLLSUF) $(DLL_SRCS) \
		$(DLL_CFLAGS) $(DLL_LDFLAGS)

bgbcc$(EXESUF): $(DEP_HDRS) bgbcc.c $(DLL_SRCS)
	$(CC_OUTEXE)bgbcc$(EXESUF) bgbcc.c $(DLL_SRCS) $(DLL_CFLAGS) $(LDFLAGS)
		

clean:
