#	base/memory2_2$(OBJSUF) \
#	base/int128$(OBJSUF) \

FRBCI_SRCS=\
	base/fri_context.c \
	base/fri_function.c \
	base/fri_image.c \
	base/fri_loader.c \
	base/fri_natcall.c \
	base/fri_opcode.c \
	base/fri_struct.c \
	base/fri_trace.c \
	\
	base/fri_opbinop.c \
	base/fri_opbinopc.c \
	base/fri_opcall.c \
	base/fri_opcmp.c \
	base/fri_opcmpc.c \
	base/fri_opconv.c \
	base/fri_opjcmp.c \
	base/fri_oploadindex.c \
	base/fri_oploadslot.c \
	base/fri_opmisc.c \
	base/fri_opunop.c \
	base/fri_opunopc.c \
	\
	base/fri_natfcn_w32.c \
	base/fri_natfcn_lnx.c \

FRBCGL_SRCS=\
	frbcgl/frgl_input.c \
	\
	frbcgl/w32_glw.c \
	frbcgl/w32_mouse.c \
	frbcgl/w32_sound.c \
	

SRCS=$(FRBCI_SRCS) $(FRBCGL_SRCS)

DEP_HDRS=\
	include/frbc2ci_auto.h

all: libs apps

FORCE:

libs: $(DLLPF)frbc2ci$(DLLSUF)

apps: frbc2ci_tst0$(EXESUF) frbc2ci_tst1$(EXESUF)

includes: $(DEP_HDRS)

include/frbc2ci_auto.h: autohead$(BINSUF) $(SRCS)
	./autohead$(BINSUF) -api FRBC2CI_API $(SRCS) > include/frbc2ci_auto.h

$(DLLPF)frbc2ci$(DLLSUF): $(DEP_HDRS) $(SRCS)
	$(CC_OUTDLL)$(DLLPF)frbc2ci$(DLLSUF) $(SRCS) $(DLL_CFLAGS) $(DLL_LDFLAGS)

autohead$(BINSUF): autohead.c
	$(CC_OUTTOOL)autohead$(BINSUF) autohead.c

frbc2ci_tst0$(EXESUF): frbc2ci_tst0.c $(DEP_HDRS) libs
	$(CC_OUTEXE)frbc2ci_tst0$(EXESUF) frbc2ci_tst0.c $(CFLAGS) $(LDFLAGS)

frbc2ci_tst1$(EXESUF): frbc2ci_tst1.c $(DEP_HDRS) libs
	$(CC_OUTEXE)frbc2ci_tst1$(EXESUF) frbc2ci_tst1.c $(CFLAGS) $(LDFLAGS)

clean:
clean2:

