BSDE_SRCS = \
	source/solid_aabb.cpp \
	source/solid_obb.cpp \
	source/solid_obb3.cpp \
	source/solid_cylinder.cpp \
	source/solid_brep.cpp \
	source/solid_sphere.cpp \
	source/solid_plane.cpp \
	source/solid_hull.cpp \
	source/solid_hull2.cpp \
	source/solid_bsp.cpp \
	source/solid_bsp2.cpp \
	\
	source/bsde_collide.cpp \
	source/bsde_solid.cpp \
	source/bsde_joint.cpp \
	source/bsde_world.cpp \
	source/bsde_math.cpp \
	source/bsde_force.cpp \
	source/bsde_iter.cpp \
	source/bsde_iter2.cpp \
	source/bsde_iter3.cpp \
	\
	source/bsde_message.cpp \
	source/bsde_thread.cpp \
	source/bsde_build.cpp \
	source/bsde_random.cpp \
	\
	source/misc_vecheap.cpp \
	\
	source/math_vec3.cpp \
	source/math_matd.cpp \
	source/math_quatd.cpp \
	source/math_planed.cpp \
	source/math_hull.cpp

BSDE_APISRCS = \
	source/api_bsde.cpp

BSDE_OBJS = $(BSDE_SRCS:.cpp=$(OBJSUF)) $(BSDE_APISRCS:.cpp=$(OBJSUF))


BSDE_STUB_SRCS = \
	apps/gl_win.c \
	apps/input.c \
	apps/mouse_win.c \
	apps/stub_w32.c \
	apps/prim3d2.c \
	apps/gl_draw.c \
	apps/map.c

BSDE_APP0_SRCS = \
	apps/bsde_app0.c

BSDE_APP1_SRCS = \
	apps/bsde_app1.c

BSDE_APP2_SRCS = \
	apps/bsde_app2.c

BSDE_STUB_OBJS = $(BSDE_STUB_SRCS:.c=$(OBJSUF))
BSDE_APP0_OBJS = $(BSDE_APP0_SRCS:.c=$(OBJSUF))
BSDE_APP1_OBJS = $(BSDE_APP1_SRCS:.c=$(OBJSUF))
BSDE_APP2_OBJS = $(BSDE_APP2_SRCS:.c=$(OBJSUF))

all: libbsde$(DLLSUF)
# bsde_app0$(EXESUF) bsde_app1$(EXESUF) bsde_app2$(EXESUF)

FORCE:

autohead$(BINSUF): autohead.c
	$(CC_OUTTOOL)autohead$(BINSUF) autohead.c

include/bsde_auto.h: autohead$(BINSUF) $(BSDE_SRCS)
	./autohead$(BINSUF) -api BSDE_API $(BSDE_SRCS) > \
		include/bsde_auto.h

include/bsde_funcs.h: autohead$(BINSUF) $(BSDE_APISRCS)
	./autohead$(BINSUF) -api BSDE_API $(BSDE_APISRCS) > \
		include/bsde_funcs.h

# libbsde.lib: include/bsde_auto.h include/bsde_funcs.h $(BSDE_OBJS)
#	lib /out:libbsde.lib $(BSDE_OBJS)

libbsde$(DLLSUF): include/bsde_auto.h include/bsde_funcs.h \
		$(BSDE_SRCS) $(BSDE_APISRCS)
	$(CC_OUTDLL)libbsde$(DLLSUF) \
		$(BSDE_SRCS) $(BSDE_APISRCS) $(DLL_CFLAGS) $(DLL_LDFLAGS)

bsde_app0$(EXESUF): libs $(BSDE_STUB_OBJS) $(BSDE_APP0_OBJS)
	$(CC_OUTEXE)bsde_app0$(EXESUF) $(BSDE_STUB_OBJS) \
		$(BSDE_APP0_OBJS) $(LDFLAGS)

bsde_app1$(EXESUF): libs $(BSDE_STUB_OBJS) $(BSDE_APP1_OBJS)
	$(CC_OUTEXE)bsde_app1$(EXESUF) $(BSDE_STUB_OBJS) \
		$(BSDE_APP1_OBJS) $(LDFLAGS)

bsde_app2$(EXESUF): libs $(BSDE_STUB_OBJS) $(BSDE_APP2_OBJS)
	$(CC_OUTEXE)bsde_app2$(EXESUF) $(BSDE_STUB_OBJS) \
		$(BSDE_APP2_OBJS) $(LDFLAGS)

includes: include/bsde_auto.h include/bsde_funcs.h
libs: libbsde$(DLLSUF)

clean:

