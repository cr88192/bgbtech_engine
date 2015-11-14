all: includes libs apps FORCE

FORCE:

BTGE_OBJS = \
	base/bt2d_base$(OBJSUF) \
	base/bt2d_bsglue$(OBJSUF) \
	base/bt2d_entity$(OBJSUF) \
	base/bt2d_move$(OBJSUF) \
	base/bt2d_player$(OBJSUF) \
	base/bt2d_tilemap$(OBJSUF) \

BTGE_HEAD = \
	include/bt2d_auto.h

BTGE_EXDEPS = \
	include/bttile2d.h

BTGE_SRCS = \
	$(BTGE_OBJS:$(OBJSUF)=.c)

BTGE_SCRIPTS = \
	tile2d/main.bs \
	tile2d/EntityBase.bs \
	tile2d/MonsterBase.bs \
#	tile2d/ClientStuff.bs \

BTGE_SCRIPTSRC = \
	base/bt2d_scripts.c \

BTGE_GENSCRIPTS = \
	tile2d/bt2d_bsglue.bs \

BTGE_ADDSCRIPTS = /add $(BTGE_SCRIPTS) $(BTGE_GENSCRIPTS)
# BTGE_DUMP = /Fnbtgesv_meta.txt
BTGE_DUMP = /Fntile2d$(ARCH)$(DLLSUF)-meta.txt

autohead$(BINSUF): autohead.c
	$(CC_OUTTOOL)autohead$(BINSUF) autohead.c

adler32$(BINSUF): adler32.c
	$(CC_OUTTOOL)adler32$(BINSUF) adler32.c

text2c$(BINSUF): text2c.c
	$(CC_OUTTOOL)text2c$(BINSUF) text2c.c

include/bt2d_auto.h: autohead$(BINSUF) $(BTGE_SRCS)
	./autohead$(BINSUF) -api BT2D_API $(BTGE_SRCS) > include/bt2d_auto.h

base/bt2d_bsglue.c: $(BTGE_SCRIPTS) $(BTGE_SCRIPTSRC)
	$(BSPKG2C) -libhdr bttile2d.h \
		-Oc base/bt2d_bsglue.c -Oh include/bt2d_bsglue.h \
		-Obs tile2d/bt2d_bsglue.bs \
		-api BT2D_API $(BTGE_SCRIPTS) $(BTGE_SCRIPTSRC)

$(DLLPF)tile2d$(ARCH)$(DLLSUF): $(BTGE_SRCS) $(BTGE_SCRIPTS) $(BTGE_EXDEPS)
	$(CC_OUTDLL)$(DLLPF)tile2d$(ARCH)$(DLLSUF) $(BTGE_SRCS) \
		$(DLL_CFLAGS) $(DLL_LDFLAGS)
	$(BGBMETA_EXW)$(DLLPF)tile2d$(ARCH)$(DLLSUF) $(BTGE_DUMP) bttile2d.h \
		$(BTGE_ADDSCRIPTS) $(BMETA_CFLAGS)


includes: $(BTGE_HEAD) $(PDGLUI_HEAD)
libs: $(DLLPF)tile2d$(ARCH)$(DLLSUF)
apps:

clean:
