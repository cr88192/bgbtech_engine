# all: liblbxgl2$(DLLSUF)
all: includes libs apps FORCE


FORCE:

#	brush/vox_chunk$(OBJSUF) \
#	brush/vox_terrgen$(OBJSUF) \

LBXGL_OBJS = \
	geom/prim3d1$(OBJSUF) \
	geom/prim3d2$(OBJSUF) \
	geom/draw2$(OBJSUF) \
	geom/texture$(OBJSUF) \
	geom/skyplane$(OBJSUF) \
	geom/skycube$(OBJSUF) \
	geom/skyclouds$(OBJSUF) \
	geom/mesh$(OBJSUF) \
	geom/mesh_edit$(OBJSUF) \
	geom/shader$(OBJSUF) \
	geom/shader2$(OBJSUF) \
	geom/particle$(OBJSUF) \
	geom/particle2$(OBJSUF) \
	geom/method$(OBJSUF) \
	geom/model$(OBJSUF) \
	geom/frac_tree$(OBJSUF) \
	\
	brush/brush$(OBJSUF) \
	brush/brush_antiportal$(OBJSUF) \
	brush/brush_autovis$(OBJSUF) \
	brush/brush_bmodel$(OBJSUF) \
	brush/brush_bsp$(OBJSUF) \
	brush/brush_cull$(OBJSUF) \
	brush/brush_csg$(OBJSUF) \
	brush/brush_draw$(OBJSUF) \
	brush/brush_draw2$(OBJSUF) \
	brush/brush_drawcube$(OBJSUF) \
	brush/brush_drawnode$(OBJSUF) \
	brush/brush_edit$(OBJSUF) \
	brush/brush_ground$(OBJSUF) \
	brush/brush_light$(OBJSUF) \
	brush/brush_map$(OBJSUF) \
	brush/brush_mapmesh$(OBJSUF) \
	brush/brush_mesh$(OBJSUF) \
	brush/brush_meshedit$(OBJSUF) \
	brush/brush_patch$(OBJSUF) \
	brush/brush_path$(OBJSUF) \
	brush/brush_portal$(OBJSUF) \
	brush/brush_prefab$(OBJSUF) \
	brush/brush_shadow$(OBJSUF) \
	brush/brush_solidmap$(OBJSUF) \
	brush/brush_vis2$(OBJSUF) \
	brush/brush_world$(OBJSUF) \
	\
	voxel/vox_barf$(OBJSUF) \
	voxel/vox_build$(OBJSUF) \
	voxel/vox_draw$(OBJSUF) \
	voxel/vox_light$(OBJSUF) \
	voxel/vox_misc$(OBJSUF) \
	voxel/vox_query$(OBJSUF) \
	\
	geom/barf_array$(OBJSUF) \
	geom/barf_draw$(OBJSUF) \
	\
	draw2d/misc_draw2d$(OBJSUF) \
	draw2d/draw2d_sequence$(OBJSUF) \
	draw2d/draw2d_seq_ui$(OBJSUF) \
	\
	brush/terr_draw$(OBJSUF) \
	geom/shadow$(OBJSUF) \
	\
	format/sprite$(OBJSUF) \
	format/asm$(OBJSUF) \
	format/effect$(OBJSUF) \
	\
	format/fmt_qmdl$(OBJSUF) \
	format/fmt_qmd3$(OBJSUF) \
	format/fmt_smd$(OBJSUF) \
	format/fmt_ac3d$(OBJSUF) \
	format/fmt_ase$(OBJSUF) \
	\
	geom/video/avi$(OBJSUF) \
	geom/video/codec$(OBJSUF) \
	geom/video/codec_vfw$(OBJSUF) \
	geom/video/codec_mbtc$(OBJSUF) \
	geom/video/codec_mjpg$(OBJSUF) \
	geom/video/codec_mpng$(OBJSUF) \
	geom/video/codec_rpza$(OBJSUF) \
	\
	skel/skel_base$(OBJSUF) \
	skel/skel_draw$(OBJSUF) \
	skel/skel_save$(OBJSUF) \
	skel/skel_mesh$(OBJSUF) \
	skel/skel_smd$(OBJSUF) \
	skel/skel_util$(OBJSUF) \
	skel/skel_pose$(OBJSUF) \
	skel/skel_vbo$(OBJSUF) \
	\
	entity21/sentity$(OBJSUF) \

#	\
#	btgecl/cl_update$(OBJSUF)

#	brush/brush_phys$(OBJSUF) \
#	\
#	entity23/entity$(OBJSUF) \
#	entity23/ent_misc$(OBJSUF) \
#	entity23/ent_scr$(OBJSUF) \
#	entity23/ent_trace$(OBJSUF) \
#	entity23/ent_phys$(OBJSUF) \
#	\

#	entity23/query$(OBJSUF) \
#	entity23/physics$(OBJSUF) \


DEL_OBJS2 = \
	entity2/entity$(OBJSUF) \
	entity2/physics$(OBJSUF) \
	entity2/ent_scr$(OBJSUF) \
	entity2/ent_trace$(OBJSUF) \
	entity21/sentity$(OBJSUF) \
	\
	entity2/ent_misc$(OBJSUF) \
	\
	bsp/bsp_entity$(OBJSUF) \
	bsp/bsp_draw$(OBJSUF) \
	bsp/bsp_dlight$(OBJSUF) \
	bsp/bsp_tree$(OBJSUF) \
	bsp/bsp_util$(OBJSUF) \
	bsp/bsp_fileio$(OBJSUF) \
	bsp/itcf_bsp$(OBJSUF) \
	\
	bsp/bsp_build$(OBJSUF) \
	bsp/bsp_clip$(OBJSUF) \
	bsp/bsp_brush$(OBJSUF) \
	\
	bsp/bsp_bsde$(OBJSUF)

#	format/fmt_acxml$(OBJSUF) \

DEL_OBJS = \
	math/mathlib_f$(OBJSUF) \
	math/mathlib_d$(OBJSUF) \
	math/math_quat$(OBJSUF) \
	math/math_mf$(OBJSUF) \
	math/math_plane$(OBJSUF) \
	math/math_v3f$(OBJSUF) \
	math/math_vnf$(OBJSUF) \
	\
	entity/event$(OBJSUF) \
	entity/entity$(OBJSUF) \
	entity/sequence$(OBJSUF) \
	entity/query$(OBJSUF) \
	entity/ent_class$(OBJSUF) \
	\
	bsp/bsp_entity$(OBJSUF) \
	bsp/bsp_draw$(OBJSUF) \
	bsp/bsp_tree$(OBJSUF) \
	bsp/bsp_util$(OBJSUF) \
	bsp/bsp_fileio$(OBJSUF) \
	bsp/itcf_bsp$(OBJSUF) \

LBXGL_SRCS = \
	$(LBXGL_OBJS:$(OBJSUF)=.c)

LBXGL_HDRS = $(shell find include -name \*.h)

# LBXGL_SCRIPTS = \
#	game_cm/cm_main.bs \

LBXGL_ADDSCRIPTS = /add $(LBXGL_SCRIPTS)
LBXGL_DUMP = /Fnlbxgl2_meta.txt

autohead$(BINSUF): autohead.c
	$(CC_OUTTOOL)autohead$(BINSUF) autohead.c

adler32$(BINSUF): adler32.c
	$(CC_OUTTOOL)adler32$(BINSUF) adler32.c

include/lbxgl2/auto.h: autohead$(BINSUF) $(LBXGL_SRCS)
	./autohead$(BINSUF) -api LBXGL_API $(LBXGL_SRCS) \
		> include/lbxgl2/auto.h
include/lbxgl2/ty_auto.h: adler32$(BINSUF) include/lbxgl2/types.txt
	./adler32$(BINSUF) -header include/lbxgl2/types.txt \
		include/lbxgl2/ty_auto.h

liblbxgl2$(DLLSUF): $(LBXGL_SRCS)
	$(CC_OUTDLL)liblbxgl2$(DLLSUF) $(LBXGL_SRCS) \
		$(DLL_CFLAGS) $(DLL_LDFLAGS)
	$(BGBMETA_EXW)$(DLLPF)liblbxgl2$(DLLSUF) $(LBXGL_DUMP) lbxgl2.h \
		$(LBXGL_ADDSCRIPTS) $(BMETA_CFLAGS)

includes: include/lbxgl2/auto.h include/lbxgl2/ty_auto.h
libs: liblbxgl2$(DLLSUF)
apps:

clean:
