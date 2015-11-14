all: includes libs apps FORCE

FORCE:

BTGE_OBJS_DEL = \
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
	brush/b_patch$(OBJSUF) \
	brush/b_phys$(OBJSUF) \
	brush/b_prefab$(OBJSUF) \
	brush/sentity$(OBJSUF) \
	\

BTGE_OBJS = \
	base/b_phys$(OBJSUF) \
	\
	base/d_misc$(OBJSUF) \
	\
	base/e_ai$(OBJSUF) \
	base/e_client$(OBJSUF) \
	base/e_combat$(OBJSUF) \
	base/e_dialog$(OBJSUF) \
	base/e_func$(OBJSUF) \
	base/e_func_train$(OBJSUF) \
	base/e_func2$(OBJSUF) \
	base/e_info$(OBJSUF) \
	base/e_items$(OBJSUF) \
	base/e_misc$(OBJSUF) \
	base/e_monster$(OBJSUF) \
	base/e_trigger$(OBJSUF) \
	base/e_weapon$(OBJSUF) \
	\
	base/g_inventory$(OBJSUF) \
	\
	base/m_dog$(OBJSUF) \
	base/m_enforcer$(OBJSUF) \
	base/m_generic$(OBJSUF) \
	base/m_knight$(OBJSUF) \
	base/m_mechazombie$(OBJSUF) \
	base/m_ogre$(OBJSUF) \
	base/m_shalrath$(OBJSUF) \
	base/m_shambler$(OBJSUF) \
	base/m_wizard$(OBJSUF) \
	base/m_zombie$(OBJSUF) \
	\
	base/s_bsglue$(OBJSUF) \
	base/s_update$(OBJSUF) \
	base/s_gui$(OBJSUF) \
	base/s_seqevent$(OBJSUF) \
	base/s_voxel$(OBJSUF) \
	\
	base/w_entbsp$(OBJSUF) \
	base/w_misc$(OBJSUF) \
	base/w_move$(OBJSUF) \
	base/w_phys$(OBJSUF) \
	base/w_savegame$(OBJSUF) \
	base/w_solid$(OBJSUF) \
	base/w_spawn$(OBJSUF) \

#	brush/b_path$(OBJSUF) \
#	brush/b_ground$(OBJSUF) \
#	brush/b_phys$(OBJSUF) \

BTGE_HEAD = \
	include/btge_auto.h

BTGE_EXDEPS = \
	include/btgesv.h

BTGE_SRCS = \
	$(BTGE_OBJS:$(OBJSUF)=.c)

BTGE_SCRIPTS = \
	game_sv/main.bs \
	game_sv/EntityBase.bs \
	game_sv/MonsterGeneric.bs \
	game_sv/ClientStuff.bs \


BTGE_ADDSCRIPTS = /add $(BTGE_SCRIPTS)
# BTGE_DUMP = /Fnbtgesv_meta.txt
BTGE_DUMP = /Fnserver$(ARCH)$(DLLSUF)-meta.txt

autohead$(BINSUF): autohead.c
	$(CC_OUTTOOL)autohead$(BINSUF) autohead.c

adler32$(BINSUF): adler32.c
	$(CC_OUTTOOL)adler32$(BINSUF) adler32.c

text2c$(BINSUF): text2c.c
	$(CC_OUTTOOL)text2c$(BINSUF) text2c.c

include/btge_auto.h: autohead$(BINSUF) $(BTGE_SRCS)
	./autohead$(BINSUF) -api BTSV_API $(BTGE_SRCS) > include/btge_auto.h

base/s_bsglue.c: $(BTGE_SCRIPTS)
	$(BSPKG2C) -libhdr btgesv.h \
		-Oc base/s_bsglue.c -Oh include/btgesv_bsglue.h \
		-api BTSV_API $(BTGE_SCRIPTS)

$(DLLPF)server$(ARCH)$(DLLSUF): $(BTGE_SRCS) $(BTGE_SCRIPTS) $(BTGE_EXDEPS)
	$(CC_OUTDLL)$(DLLPF)server$(ARCH)$(DLLSUF) $(BTGE_SRCS) \
		$(DLL_CFLAGS) $(DLL_LDFLAGS)
	$(BGBMETA_EXW)$(DLLPF)server$(ARCH)$(DLLSUF) $(BTGE_DUMP) btgesv.h \
		$(BTGE_ADDSCRIPTS) $(BMETA_CFLAGS)


includes: $(BTGE_HEAD) $(PDGLUI_HEAD)
libs: $(DLLPF)server$(ARCH)$(DLLSUF)
apps:

clean:
