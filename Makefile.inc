all: bscc base3d FORCE

base3d: includes libs apps FORCE

FORCE:


includes_bscc:
	$(MAKE) -C bscc -f Makefile$(MKSUF) includes
	cp -rf bscc/include/*		$(BUILD)/include

libs_bscc:
	$(MAKE) -C bscc -f Makefile$(MKSUF) libs
#	cp -rf bscc/*$(LIBSUF)		$(BUILD)
	cp -rf bscc/*$(DLLSUF)		$(BUILD)
	cp -rf bscc/*$(BINSUF)		$(BUILD)

apps_bscc:
	$(MAKE) -C bscc -f Makefile$(MKSUF) apps
	cp -rf bscc/*$(EXESUF)		$(BUILD)

meta_bscc: FORCE
	$(MAKE) -C bscc -f Makefile$(MKSUF) meta
	cp -rf bscc/*_meta.txt		$(BUILD)

bscc: includes_bscc libs_bscc apps_bscc
# meta_bscc


includes_q2core:
	$(MAKE) -C q2core -f Makefile$(MKSUF) includes
	cp -rf q2core/include/*		$(BUILD)/include

libs_q2core:
	$(MAKE) -C q2core -f Makefile$(MKSUF) libs
#	cp -rf q2core/*$(LIBSUF)		$(BUILD)
	cp -rf q2core/*$(DLLSUF)		$(BUILD)

apps_q2core:
	$(MAKE) -C q2core -f Makefile$(MKSUF) apps
	cp -rf q2core/*$(EXESUF)		$(BUILD)

meta_q2: FORCE
	./bgbmeta$(EXESUF) /Fngamex86_meta.txt	q2core/game/g_local.h /Iinclude
	./bgbmeta$(EXESUF) /Fnref_gl_meta.txt	q2core/ref_gl/gl_local.h /Iinclude

q2core: includes_q2core libs_q2core apps_q2core


includes: FORCE
	$(MAKE) -C libbsde21 -f Makefile$(MKSUF) includes
	$(MAKE) -C libvecmath -f Makefile$(MKSUF) includes
	$(MAKE) -C bsgen -f Makefile$(MKSUF) includes
	$(MAKE) -C bgbmid -f Makefile$(MKSUF) includes
	$(MAKE) -C tremor -f Makefile$(MKSUF) includes
#	$(MAKE) -C bscc -f Makefile$(MKSUF) includes

	$(MAKE) -C btgecm -f Makefile$(MKSUF) includes
	$(MAKE) -C btgesv -f Makefile$(MKSUF) includes
	$(MAKE) -C btgecl -f Makefile$(MKSUF) includes

	$(MAKE) -C bgbbtjpg -f Makefile$(MKSUF) includes
	$(MAKE) -C libpdgl -f Makefile$(MKSUF) includes
	$(MAKE) -C lbxgl3 -f Makefile$(MKSUF) includes

#	$(MAKE) -C q2core -f Makefile$(MKSUF) includes

	cp -rf libbsde21/include/*.h	$(BUILD)/include
	cp -rf libvecmath/*.h		$(BUILD)/include
	cp -rf bsgen/include/*.h	$(BUILD)/include
	cp -rf bgbmid/*.h		$(BUILD)/include
	cp -rf tremor/*.h		$(BUILD)/include
#	cp -rf bscc/include/*		$(BUILD)/include

#	cp -rf qcommon/*.h		$(BUILD)/include
#	cp -rf client/*.h		$(BUILD)/include
#	cp -rf server/*.h		$(BUILD)/include
#	cp -rf game/*.h			$(BUILD)/include

	cp -rf btgecm/include/*		$(BUILD)/include
	cp -rf btgesv/include/*		$(BUILD)/include
	cp -rf btgecl/include/*		$(BUILD)/include

	cp -rf bgbbtjpg/include/*	$(BUILD)/include
	cp -rf libpdgl/include/*	$(BUILD)/include
	cp -rf lbxgl3/include/*		$(BUILD)/include

#	cp -rf q2core/include/*		$(BUILD)/include

libs: FORCE
#	$(MAKE) -C bscc -f Makefile$(MKSUF) libs
#	cp -rf bscc/*$(LIBSUF)		$(BUILD)
#	cp -rf bscc/*$(DLLSUF)		$(BUILD)

	$(MAKE) -C libbsde21 -f Makefile$(MKSUF) libs
#	cp -rf libbsde21/*$(LIBSUF)		$(BUILD)
	cp -rf libbsde21/*$(DLLSUF)		$(BUILD)

	$(MAKE) -C libvecmath -f Makefile$(MKSUF) libs
#	cp -rf libvecmath/*$(LIBSUF)		$(BUILD)
	cp -rf libvecmath/*$(DLLSUF)		$(BUILD)

	$(MAKE) -C bsgen -f Makefile$(MKSUF) libs
#	cp -rf bsgen/*$(LIBSUF)		$(BUILD)
	cp -rf bsgen/*$(DLLSUF)		$(BUILD)

	$(MAKE) -C bgbmid -f Makefile$(MKSUF) libs
#	cp -rf bgbmid/*$(LIBSUF)		$(BUILD)
	cp -rf bgbmid/*$(DLLSUF)		$(BUILD)

	$(MAKE) -C tremor -f Makefile$(MKSUF) libs
#	cp -rf tremor/*$(LIBSUF)		$(BUILD)
	cp -rf tremor/*$(DLLSUF)		$(BUILD)

	$(MAKE) -C btgecm -f Makefile$(MKSUF) libs
#	cp -rf btgesv/*$(LIBSUF)		$(BUILD)
	cp -rf btgecm/*$(DLLSUF)		$(BUILD)

	$(MAKE) -C bgbbtjpg -f Makefile$(MKSUF) libs
#	cp -rf bgbbtjpg/*$(LIBSUF)		$(BUILD)
	cp -rf bgbbtjpg/*$(DLLSUF)		$(BUILD)

	$(MAKE) -C libpdgl -f Makefile$(MKSUF) libs
#	cp -rf libpdgl/*$(LIBSUF)		$(BUILD)
	cp -rf libpdgl/*$(DLLSUF)		$(BUILD)

	$(MAKE) -C lbxgl3 -f Makefile$(MKSUF) libs
#	cp -rf lbxgl3/*$(LIBSUF)		$(BUILD)
	cp -rf lbxgl3/*$(DLLSUF)		$(BUILD)

	$(MAKE) -C btgesv -f Makefile$(MKSUF) libs
#	cp -rf btgesv/*$(LIBSUF)		$(BUILD)
	cp -rf btgesv/*$(DLLSUF)		$(BUILD)/resource

	$(MAKE) -C btgecl -f Makefile$(MKSUF) libs
#	cp -rf btgecl/*$(LIBSUF)		$(BUILD)
	cp -rf btgecl/*$(DLLSUF)		$(BUILD)/resource

#	$(MAKE) -C q2core -f Makefile$(MKSUF) libs
#	cp -rf q2core/*$(LIBSUF)		$(BUILD)
#	cp -rf q2core/*$(DLLSUF)		$(BUILD)


apps: FORCE
	$(MAKE) -C libbsde21 -f Makefile$(MKSUF) apps
	$(MAKE) -C libvecmath -f Makefile$(MKSUF) apps
	$(MAKE) -C bsgen -f Makefile$(MKSUF) apps
	$(MAKE) -C bgbmid -f Makefile$(MKSUF) apps
	$(MAKE) -C tremor -f Makefile$(MKSUF) apps
#	$(MAKE) -C bscc -f Makefile$(MKSUF) apps

	$(MAKE) -C bgbbtjpg -f Makefile$(MKSUF) apps
	$(MAKE) -C libpdgl -f Makefile$(MKSUF) apps
	$(MAKE) -C lbxgl3 -f Makefile$(MKSUF) apps

#	$(MAKE) -C q2core -f Makefile$(MKSUF) apps

	$(MAKE) -C apps -f Makefile$(MKSUF) apps

#	cp -rf libbsde21/*$(EXESUF)		$(BUILD)
#	cp -rf libvecmath/*$(EXESUF)		$(BUILD)
	cp -rf bsgen/*$(EXESUF)		$(BUILD)
	cp -rf bgbmid/*$(EXESUF)		$(BUILD)
	cp -rf tremor/*$(EXESUF)		$(BUILD)
#	cp -rf bscc/*$(EXESUF)		$(BUILD)

#	cp -rf libpdgl/*$(EXESUF)		$(BUILD)
#	cp -rf lbxgl3/*$(EXESUF)		$(BUILD)
#	cp -rf q2core/*$(EXESUF)		$(BUILD)

	cp -rf apps/*$(EXESUF)		$(BUILD)


meta_libs: FORCE
	./bgbmeta$(EXESUF) /Fnlibbsde_meta.txt		libbsde.h /Iinclude
	./bgbmeta$(EXESUF) /Fnlibvecmath_meta.txt	libvecmath.h /Iinclude
	./bgbmeta$(EXESUF) /Fnlibbsgen_meta.txt		bsgen.h /Iinclude
	./bgbmeta$(EXESUF) /Fnlibbgbmid_meta.txt	bgbmid.h /Iinclude
	./bgbmeta$(EXESUF) /Fnlibtrem_meta.txt		ivorbisfile.h /Iinclude
	./bgbmeta$(EXESUF) /Fnlibpdgl_meta.txt		pdgl.h /Iinclude
	./bgbmeta$(EXESUF) /Fnlibpdglui_meta.txt	pdglui.h /Iinclude
	./bgbmeta$(EXESUF) /Fnliblbxgl2_meta.txt	lbxgl2.h /Iinclude
#	./bgbmeta$(EXESUF) /Fnbtgecm_meta.txt		btgecm.h /Iinclude
#	./bgbmeta$(EXESUF) /Fnbtgesv_meta.txt		btgesv.h /Iinclude


meta: meta_bscc meta_libs FORCE

clean:
