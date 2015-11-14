OBJS=\
	base/frir_api.c \
	base/frir_call.c \
	base/frir_ops.c \
	base/frir_jumpcc.c \
	base/frir_thread.c \
	base/frir_threadop.c \

SRCS=$(OBJS:$(OBJSUF)=.c)

DEP_HDRS=\
	include/bgbfrir_ops.h \
	include/bgbfrir_auto.h

BS_SRCS = \
	base/asm_frir_x86.asm

BS_ADDSCRIPTS = /add $(BS_SRCS)
BS_DUMP = /Fnbgbfrir_meta.txt

all: libs apps

FORCE:

libs: $(DLLPF)bgbfrir$(DLLSUF)

# bsvmrt.bsar

apps: frirtst0$(EXESUF)

includes: $(DEP_HDRS)

include/bgbfrir_auto.h: autohead$(EXESUF) $(SRCS)
	./autohead$(EXESUF) -api BGBFRIR_API $(SRCS) > include/bgbfrir_auto.h

include/bgbfrir_ops.h: genops_frir$(EXESUF) bgbfrir_ops.txt
	./genops_frir$(EXESUF) -It bgbfrir_ops.txt -Oc base/frir_ops.c \
		-Oh include/bgbfrir_ops.h -pfx FRIR

%$(OBJSUF): %.c
	$(CC_OUTOBJ)$@ $< $(CFLAGS)

$(DLLPF)bgbfrir$(DLLSUF): $(DEP_HDRS) $(SRCS) $(BS_SRCS)
	$(CC_OUTDLL)$(DLLPF)bgbfrir$(DLLSUF) $(SRCS) \
		$(DLL_CFLAGS) $(DLL_LDFLAGS)
	$(BGBMETA_EXW)$(DLLPF)bgbfrir$(DLLSUF) $(BS_DUMP) bgbfrir.h \
		$(BS_ADDSCRIPTS) $(BMETA_CFLAGS)

autohead$(EXESUF): autohead.c
	$(CC_OUTTOOL)autohead$(EXESUF) autohead.c

genops_frir$(EXESUF): genops_frir.c
	$(CC_OUTTOOL)genops_frir$(EXESUF) genops_frir.c

frirtst0$(EXESUF): frirtst0.c $(DEP_HDRS) libs
	$(CC_OUTEXE)frirtst0$(EXESUF) frirtst0.c $(CFLAGS) $(LDFLAGS)

# bsvm1cc$(EXESUF): bs1cc.c $(DEP_HDRS) libs
#	$(CC_OUTEXE)bsvm1cc$(EXESUF) bs1cc.c bgbfrir.lib $(CFLAGS) $(LDFLAGS)

# bsvmrt.bsar: FORCE
#	jar -cvf bsvmrt.bsar -C bssrc .

clean:
clean2:
