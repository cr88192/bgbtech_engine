BINS = \
	autohead$(EXE) \
	bgbdoc$(EXE) \
	adler32$(EXE) \
	grandhash$(EXE) \
	itcfdump$(EXE) \
	bsodump$(EXE)

# lbx_app0$(EXE) mapper1$(EXE) rt_tst0$(EXE)

APPS = \
	gui_app0$(EXE) bonetool2$(EXE) lbx_app1$(EXE) \
	csgtool$(EXE) mapper2$(EXE) \
	lbx_runc$(EXE) lbx_texconv$(EXE) engine0$(EXE) \
	meshtool2$(EXE) \
	tile2d$(EXE)

all: FORCE apps

FORCE:

autohead$(EXE): tools/autohead.c
	$(CC_OUTEXE)autohead$(EXE) tools/autohead.c

adler32$(EXE): tools/adler32.c
	$(CC_OUTEXE)adler32$(EXE) tools/adler32.c

grandhash$(EXE): tools/grandhash.c
	$(CC_OUTEXE)grandhash$(EXE) tools/grandhash.c

itcfdump$(EXE): tools/itcfdump.c
	$(CC_OUTEXE)itcfdump$(EXE) tools/itcfdump.c

bsodump$(EXE): tools/bsodump.c
	$(CC_OUTEXE)bsodump$(EXE) tools/bsodump.c

bgbdoc$(EXE): FORCE
	make -C tools/bgbdoc -f Makefile.msvc
	cp tools/bgbdoc/bgbdoc$(EXE) .

includes:
libs:

apps: $(APPS) FORCE

%$(EXE): %.c
	$(CC_OUTEXE)$@ $< $(CFLAGS) $(LDFLAGS)

clean:
