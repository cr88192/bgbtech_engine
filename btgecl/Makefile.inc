all: includes libs apps FORCE

FORCE:

BTGE_OBJS = \
	base/cl_update$(OBJSUF) \
	base/cl_hud$(OBJSUF) \
	base/cl_gui$(OBJSUF) \
	base/cl_frame$(OBJSUF) \
	base/cl_impulse$(OBJSUF) \
	base/cl_bsglue$(OBJSUF) \
	\
	base/clui_draw$(OBJSUF) \
	base/clui_form$(OBJSUF) \
	base/clui_widget$(OBJSUF) \
	base/cluiw_box$(OBJSUF) \
	base/cluiw_input$(OBJSUF) \
	base/cluiw_label$(OBJSUF) \
	base/cluiw_list$(OBJSUF) \
	base/cluiw_scroll$(OBJSUF) \
	base/cluiw_textarea$(OBJSUF) \
	base/cluiw_tree$(OBJSUF) \
	base/cluiw_union$(OBJSUF) \

BTGE_HEAD = \
	include/btgecl_auto.h

BTGE_SRCS = \
	$(BTGE_OBJS:$(OBJSUF)=.c)

BTGE_SCRIPTS = \
	game_cl/main.bs

BTGE_ADDSCRIPTS = /add $(BTGE_SCRIPTS)
BTGE_DUMP = /Fnclient$(ARCH)$(DLLSUF)-meta.txt


autohead$(BINSUF): autohead.c
	$(CC_OUTTOOL)autohead$(BINSUF) autohead.c

adler32$(BINSUF): adler32.c
	$(CC_OUTTOOL)adler32$(BINSUF) adler32.c

text2c$(BINSUF): text2c.c
	$(CC_OUTTOOL)text2c$(BINSUF) text2c.c

include/btgecl_auto.h: autohead$(BINSUF) $(BTGE_SRCS)
	./autohead$(BINSUF) -api BTCL_API $(BTGE_SRCS) > include/btgecl_auto.h

base/cl_bsglue.c: $(BTGE_SCRIPTS)
	$(BSPKG2C) -libhdr btgecl.h \
		-Oc base/cl_bsglue.c -Oh include/btgecl_bsglue.h \
		-api BTCL_API $(BTGE_SCRIPTS)

$(DLLPF)client$(ARCH)$(DLLSUF): $(BTGE_SRCS) $(BTGE_SCRIPTS)
	$(CC_OUTDLL)$(DLLPF)client$(ARCH)$(DLLSUF) $(BTGE_SRCS) \
		$(DLL_CFLAGS) $(DLL_LDFLAGS)
	$(BGBMETA_EXW)$(DLLPF)client$(ARCH)$(DLLSUF) $(BTGE_DUMP) btgecl.h \
		$(BTGE_ADDSCRIPTS) $(BMETA_CFLAGS)

includes: $(BTGE_HEAD) $(PDGLUI_HEAD)
libs: $(DLLPF)client$(ARCH)$(DLLSUF)
apps:

clean:
