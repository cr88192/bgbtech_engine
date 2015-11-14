all: includes libs apps FORCE

FORCE:

#	brush/b_phys$(OBJSUF) \

BTGE_OBJS = \
	brush/b_brush$(OBJSUF) \
	brush/b_bsp$(OBJSUF) \
	brush/b_csg$(OBJSUF) \
	brush/b_vis2$(OBJSUF) \
	brush/b_portal$(OBJSUF) \
	brush/b_light$(OBJSUF) \
	brush/b_bmodel$(OBJSUF) \
	brush/b_world$(OBJSUF) \
	brush/b_solidmap$(OBJSUF) \
	brush/b_texture$(OBJSUF) \
	brush/b_map$(OBJSUF) \
	brush/b_mapmesh$(OBJSUF) \
	brush/b_mesh$(OBJSUF) \
	brush/b_patch$(OBJSUF) \
	brush/b_prefab$(OBJSUF) \
	brush/be_misc$(OBJSUF) \
	brush/sentity$(OBJSUF) \
	\
	base/cm_ac3d$(OBJSUF) \
	base/cm_conedit$(OBJSUF) \
	base/cm_cvar$(OBJSUF) \
	base/cm_filecache$(OBJSUF) \
	base/cm_maze$(OBJSUF) \
	base/cm_mesh$(OBJSUF) \
	base/cm_meshutil$(OBJSUF) \
	base/cm_model$(OBJSUF) \
	base/cm_net$(OBJSUF) \
	base/cm_noise$(OBJSUF) \
	base/cm_seqevent$(OBJSUF) \
	base/cm_server$(OBJSUF) \
	base/cm_tile2d$(OBJSUF) \
	base/cm_client$(OBJSUF) \
	base/cm_png$(OBJSUF) \
	base/cm_colorkey$(OBJSUF) \
	base/cm_bsglue$(OBJSUF) \
	\
	base/cmui_delta$(OBJSUF) \
	base/cmui_form$(OBJSUF) \
	\
	base/voxcm_biome$(OBJSUF) \
	base/voxcm_chunk$(OBJSUF) \
	base/voxcm_delta$(OBJSUF) \
	base/voxcm_dungeon$(OBJSUF) \
	base/voxcm_elastic$(OBJSUF) \
	base/voxcm_elastic2$(OBJSUF) \
	base/voxcm_emit$(OBJSUF) \
	base/voxcm_entity$(OBJSUF) \
	base/voxcm_idxchunk$(OBJSUF) \
	base/voxcm_light$(OBJSUF) \
	base/voxcm_piston$(OBJSUF) \
	base/voxcm_props$(OBJSUF) \
	base/voxcm_save$(OBJSUF) \
	base/voxcm_stairs$(OBJSUF) \
	base/voxcm_terrgen$(OBJSUF) \
	base/voxcm_tick$(OBJSUF) \
	base/voxcm_rail$(OBJSUF) \
	base/voxcm_randthink$(OBJSUF) \
	base/voxcm_redtick$(OBJSUF) \

BTGE_HEAD = \
	include/btgecm_auto.h

BTGE_SRCS = \
	$(BTGE_OBJS:$(OBJSUF)=.c)

BTGE_SCRIPTS = \
	game_cm/cm_main.bs \

BTGE_ADDSCRIPTS = /add $(BTGE_SCRIPTS)
BTGE_DUMP = /Fnbtgecm_meta.txt

autohead$(BINSUF): autohead.c
	$(CC_OUTTOOL)autohead$(BINSUF) autohead.c

adler32$(BINSUF): adler32.c
	$(CC_OUTTOOL)adler32$(BINSUF) adler32.c

text2c$(BINSUF): text2c.c
	$(CC_OUTTOOL)text2c$(BINSUF) text2c.c

include/btgecm_auto.h: autohead$(BINSUF) $(BTGE_SRCS)
	./autohead$(BINSUF) -api BTGE_API $(BTGE_SRCS) > include/btgecm_auto.h

base/cm_bsglue.c: $(BTGE_SCRIPTS)
	$(BSPKG2C) -libhdr btgecm.h \
		-Oc base/cm_bsglue.c -Oh include/btgecm_bsglue.h \
		-api BTGE_API $(BTGE_SCRIPTS)

$(DLLPF)btgecm$(DLLSUF): $(BTGE_SRCS)
	$(CC_OUTDLL)$(DLLPF)btgecm$(DLLSUF) $(BTGE_SRCS) \
		$(DLL_CFLAGS) $(DLL_LDFLAGS)
	$(BGBMETA_EXW)$(DLLPF)btgecm$(DLLSUF) $(BTGE_DUMP) btgecm.h \
		$(BTGE_ADDSCRIPTS) $(BMETA_CFLAGS)

includes: $(BTGE_HEAD) $(PDGLUI_HEAD)
libs: $(DLLPF)btgecm$(DLLSUF)
apps:

clean:
