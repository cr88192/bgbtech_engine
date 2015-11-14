#	base/memory2_2$(OBJSUF) \
#	base/int128$(OBJSUF) \

OBJS=\
	base/th_api$(OBJSUF) \
	base/th_queue$(OBJSUF) \
	base/th_work$(OBJSUF) \
	base/th_profile$(OBJSUF) \
	base/eh_throw$(OBJSUF) \
	\
	base/mm_lobj$(OBJSUF) \
	base/mm_cell$(OBJSUF) \
	base/mm_cons$(OBJSUF) \
	base/mm_fastidx$(OBJSUF) \
	base/mm_gc$(OBJSUF) \
	base/mm_heapstat$(OBJSUF) \
	base/mm_space$(OBJSUF) \
	base/mm_strtab$(OBJSUF) \
	base/objtype$(OBJSUF) \
	\
	base/bgbgc_api$(OBJSUF) \
	base/gcp_api$(OBJSUF)

SRCS=$(OBJS:$(OBJSUF)=.c)

DEP_HDRS=\
	include/bgbgc_auto.h

all: libs apps

FORCE:

libs: $(DLLPF)bgbgc$(DLLSUF)
apps: gctst0$(EXESUF)

includes: $(DEP_HDRS)

include/bgbgc_auto.h: autohead$(BINSUF) $(SRCS)
	./autohead$(BINSUF) -api BGBGC_API $(SRCS) > include/bgbgc_auto.h

%$(OBJSUF): %.c
	$(CC_OUTOBJ)$@ $< $(CFLAGS)

$(DLLPF)bgbgc$(DLLSUF): $(DEP_HDRS) $(SRCS)
	$(CC_OUTDLL)$(DLLPF)bgbgc$(DLLSUF) $(SRCS) $(DLL_CFLAGS) $(DLL_LDFLAGS)

# bgbgc.lib: $(DEP_HDRS) $(OBJS)
#	lib /out:bgbgc.lib $(OBJS)

autohead$(BINSUF): autohead.c
	$(CC_OUTTOOL)autohead$(BINSUF) autohead.c

gctst0$(EXESUF): gctst0.c $(DEP_HDRS) libs
	$(CC_OUTEXE)gctst0$(EXESUF) gctst0.c $(CFLAGS) $(LDFLAGS)

tst_class0$(EXESUF): tst_class0.c $(DEP_HDRS) libs
	$(CC_OUTEXE)tst_class0$(EXESUF) tst_class0.c $(CFLAGS) $(LDFLAGS)

gatst0$(EXESUF): gatst0.c $(DEP_HDRS) libs
	$(CC_OUTEXE)gatst0$(EXESUF) gatst0.c $(CFLAGS) $(LDFLAGS)

clean:
clean2:

