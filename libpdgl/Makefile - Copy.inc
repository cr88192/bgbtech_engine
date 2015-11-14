EXE=$(EXESUF)

all: includes libs apps FORCE

FORCE:

PDGL_G_OBJS = \
	base/render/fonts$(OBJSUF) \
	base/render/console$(OBJSUF) \
	base/render/concmds$(OBJSUF) \
	base/render/gl_draw$(OBJSUF) \
	base/render/gl_texture$(OBJSUF) \
	base/render/drawtext$(OBJSUF) \
	base/render/pcx$(OBJSUF) \
	base/render/png$(OBJSUF) \
	base/render/jpg2$(OBJSUF) \
	\
	base/render/out_avi$(OBJSUF) \
	base/render/gl_shader$(OBJSUF) \
	\
	base/sound/sound2$(OBJSUF) \
	base/sound/snd_mix2$(OBJSUF) \
	base/sound/snd_wav$(OBJSUF) \
	base/sound/snd_lqac$(OBJSUF) \
	base/sound/snd_mp3$(OBJSUF) \
	base/sound/snd_ogg$(OBJSUF) \
	base/sound/snd_voice$(OBJSUF) \
	base/sound/snd_flac1$(OBJSUF) \
	\
	base/sound/mpglib/layer1$(OBJSUF) \
	base/sound/mpglib/layer2$(OBJSUF) \
	base/sound/mpglib/common$(OBJSUF) \
	base/sound/mpglib/dct64_i386$(OBJSUF) \
	base/sound/mpglib/decode_i386$(OBJSUF) \
	base/sound/mpglib/layer3$(OBJSUF) \
	base/sound/mpglib/tabinit$(OBJSUF) \
	base/sound/mpglib/interface$(OBJSUF)

#	base/sound/snd_pdsif$(OBJSUF) \
#	base/sound/snd_mp3$(OBJSUF)

PDGL_OBJS = \
	$(PDGL_G_OBJS) \
	base/system/gl_win2$(OBJSUF) \
	base/system/gl_glx$(OBJSUF) \
	base/system/mouse_win$(OBJSUF) \
	base/system/input$(OBJSUF) \
	base/system/snd_win2$(OBJSUF) \
	base/system/snd_stub$(OBJSUF) \
	base/system/misc_win$(OBJSUF) \
	base/system/asm_x86$(OBJSUF) \
	base/system/ui_keybind$(OBJSUF) \


PDGLUI_OBJS = \
	gui2/build$(OBJSUF) \
	gui2/forms$(OBJSUF) \
	gui2/menus$(OBJSUF) \
	gui2/textarea$(OBJSUF) \
	gui2/message$(OBJSUF) \
	gui2/widget$(OBJSUF) \
	gui2/widget_input$(OBJSUF) \
	gui2/widget_box$(OBJSUF) \
	gui2/widget_union$(OBJSUF) \
	gui2/widget_label$(OBJSUF) \
	gui2/widget_scroll$(OBJSUF) \
	gui2/widget_list$(OBJSUF) \
	gui2/widget_tree$(OBJSUF) \
	\
	ui/ui_loadsave$(OBJSUF) \
	ui/ui_texfinder$(OBJSUF) \
	ui/ui_camera$(OBJSUF)

PDGL_HEAD = \
	include/pdgl/auto.h \
	include/pdgl/tyauto.h

PDGLUI_HEAD = \
	include/pdglui/auto.h \
	include/pdglui/tyauto.h

PDGL_SRCS = \
	$(PDGL_OBJS:$(OBJSUF)=.c)

PDGLUI_SRCS = \
	$(PDGLUI_OBJS:$(OBJSUF)=.c)


autohead$(EXESUF): autohead.c
	$(CC_OUTEXE)autohead$(EXESUF) autohead.c

adler32$(EXESUF): adler32.c
	$(CC_OUTEXE)adler32$(EXESUF) adler32.c

include/pdgl/auto.h: autohead$(EXESUF) $(PDGL_SRCS)
	./autohead$(EXESUF) -api PDGL_API $(PDGL_SRCS) > include/pdgl/auto.h

include/pdgl/tyauto.h: adler32$(EXESUF) include/pdgl/types.txt
	./adler32$(EXESUF) -header include/pdgl/types.txt include/pdgl/tyauto.h

include/pdglui/auto.h: autohead$(EXESUF) $(PDGLUI_SRCS)
	./autohead$(EXESUF) -api PDGLUI_API $(PDGLUI_SRCS) > include/pdglui/auto.h

include/pdglui/tyauto.h: adler32$(EXESUF) include/pdglui/types.txt
	./adler32$(EXESUF) -header include/pdglui/types.txt include/pdglui/tyauto.h

libpdgl$(DLLSUF): $(PDGL_SRCS)
	$(CC_OUTDLL)libpdgl$(DLLSUF) $(PDGL_SRCS) \
		$(DLL_CFLAGS) $(DLL_LDFLAGS)

libpdglui$(DLLSUF): $(PDGLUI_SRCS)
	$(CC_OUTDLL)libpdglui$(DLLSUF) $(PDGLUI_SRCS) \
		$(DLL_CFLAGS2) $(DLL_LDFLAGS2)

includes: $(PDGL_HEAD) $(PDGLUI_HEAD)
libs: libpdgl$(DLLSUF) libpdglui$(DLLSUF)
apps:

clean:
