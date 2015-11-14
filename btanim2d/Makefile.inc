SRCS = \
	base/bta2d_api.c

DEP_HDRS = \
	include/bta2d_auto.h

# all: awal$(EXESUF) genavi$(EXESUF)
all: includes libs apps

apps: anim2d$(EXESUF)
libs: $(DLLPF)btanim2d$(DLLSUF)

includes: $(DEP_HDRS)

# awal$(EXESUF): $(WALPNG_SRCS) awal.c
#	cl /Feawal$(EXESUF) /I. $(WALPNG_SRCS) awal.c /Z7

include/bta2d_auto.h: autohead$(BINSUF) $(SRCS)
	./autohead$(BINSUF) -api BTA2D_API $(SRCS) > include/bta2d_auto.h

$(DLLPF)btanim2d$(DLLSUF): $(DEP_HDRS) $(SRCS)
	$(CC_OUTDLL)$(DLLPF)btanim2d$(DLLSUF) $(SRCS) $(DLL_CFLAGS) $(DLL_LDFLAGS)

autohead$(BINSUF): autohead.c
	$(CC_OUTTOOL)autohead$(BINSUF) autohead.c


genavi$(EXESUF): $(SRCS) genavi.c
	$(CC_OUTEXE)genavi$(EXESUF) genavi.c $(CFLAGS) $(LDFLAGS)

anim2d$(EXESUF): $(SRCS) anim2d.c
	$(CC_OUTEXE)anim2d$(EXESUF) anim2d.c $(CFLAGS) $(LDFLAGS)

clean:
