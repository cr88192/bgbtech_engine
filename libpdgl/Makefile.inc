EXE=$(EXESUF)

all: includes libs apps FORCE

FORCE:

#	base/render/jpg2.c \

PDGL_G_SRCS = \
	base/render/fonts.c \
	base/render/fonts_qi2.c \
	base/render/console.c \
	base/render/concmds.c \
	base/render/gl_draw.c \
	base/render/gl_draw_gles.c \
	base/render/gl_drawprim.c \
	base/render/gl_drawprim_gles.c \
	base/render/gl_texture.c \
	base/render/drawtext.c \
	base/render/pcx.c \
	base/render/png.c \
	base/render/jpg2.c \
	\
	base/render/out_avi.c \
	base/render/gl_shader.c \
	base/render/gl_shader_gles.c \
	\
	base/sound/sound3_bmid.c \

#	\
#	base/sound/sound2.c \
#	base/sound/snd_mix2.c \
#	base/sound/snd_wav.c \
#	base/sound/snd_lqac.c \
#	base/sound/snd_mp3.c \
#	base/sound/snd_ogg.c \
#	base/sound/snd_voice.c \
#	base/sound/snd_flac1.c \
#	base/sound/snd_btac_filt.c \
#	base/sound/snd_btac_sample.c \
#	\
#	base/sound/mpglib/layer1.c \
#	base/sound/mpglib/layer2.c \
#	base/sound/mpglib/common.c \
#	base/sound/mpglib/dct64_i386.c \
#	base/sound/mpglib/decode_i386.c \
#	base/sound/mpglib/layer3.c \
#	base/sound/mpglib/tabinit.c \
#	base/sound/mpglib/interface.c

#	base/sound/snd_pdsif.c \
#	base/sound/snd_mp3.c

PDGL_SRCS = \
	$(PDGL_G_SRCS) \
	base/system/gl_win2.c \
	base/system/gl_glx.c \
	base/system/gl_nacl.c \
	base/system/mouse_win.c \
	base/system/input.c \
	base/system/misc_win.c \
	base/system/asm_x86.c \
	base/system/ui_keybind.c \

#	base/system/snd_win2.c \
#	base/system/snd_stub.c \


PDGLUI_SRCS = \
	gui2/build.c \
	gui2/forms.c \
	gui2/menus.c \
	gui2/textarea.c \
	gui2/message.c \
	gui2/widget.c \
	gui2/widget_input.c \
	gui2/widget_box.c \
	gui2/widget_union.c \
	gui2/widget_label.c \
	gui2/widget_scroll.c \
	gui2/widget_list.c \
	gui2/widget_tree.c \
	\
	ui/ui_loadsave.c \
	ui/ui_texfinder.c \
	ui/ui_camera.c \
	ui/ui_metaui.c \


PDGLUI_OVR_SRCS = \
	rift/OculusSDK/LibOVR/Src/Kernel/OVR_Alg.cpp \
	rift/OculusSDK/LibOVR/Src/Kernel/OVR_Allocator.cpp \
	rift/OculusSDK/LibOVR/Src/Kernel/OVR_Atomic.cpp \
	rift/OculusSDK/LibOVR/Src/Kernel/OVR_File.cpp \
	rift/OculusSDK/LibOVR/Src/Kernel/OVR_FileFILE.cpp \
	rift/OculusSDK/LibOVR/Src/Kernel/OVR_Log.cpp \
	rift/OculusSDK/LibOVR/Src/Kernel/OVR_Math.cpp \
	rift/OculusSDK/LibOVR/Src/Kernel/OVR_RefCount.cpp \
	rift/OculusSDK/LibOVR/Src/Kernel/OVR_Std.cpp \
	rift/OculusSDK/LibOVR/Src/Kernel/OVR_String.cpp \
	rift/OculusSDK/LibOVR/Src/Kernel/OVR_String_FormatUtil.cpp \
	rift/OculusSDK/LibOVR/Src/Kernel/OVR_String_PathUtil.cpp \
	rift/OculusSDK/LibOVR/Src/Kernel/OVR_SysFile.cpp \
	rift/OculusSDK/LibOVR/Src/Kernel/OVR_System.cpp \
	rift/OculusSDK/LibOVR/Src/Kernel/OVR_ThreadsWinAPI.cpp \
	rift/OculusSDK/LibOVR/Src/Kernel/OVR_Timer.cpp \
	rift/OculusSDK/LibOVR/Src/Kernel/OVR_UTF8Util.cpp \
	rift/OculusSDK/LibOVR/Src/OVR_DeviceHandle.cpp \
	rift/OculusSDK/LibOVR/Src/OVR_DeviceImpl.cpp \
	rift/OculusSDK/LibOVR/Src/OVR_JSON.cpp \
	rift/OculusSDK/LibOVR/Src/OVR_LatencyTestImpl.cpp \
	rift/OculusSDK/LibOVR/Src/OVR_Profile.cpp \
	rift/OculusSDK/LibOVR/Src/OVR_SensorFilter.cpp \
	rift/OculusSDK/LibOVR/Src/OVR_SensorFusion.cpp \
	rift/OculusSDK/LibOVR/Src/OVR_SensorImpl.cpp \
	rift/OculusSDK/LibOVR/Src/OVR_ThreadCommandQueue.cpp \
	rift/OculusSDK/LibOVR/Src/OVR_Win32_DeviceManager.cpp \
	rift/OculusSDK/LibOVR/Src/OVR_Win32_DeviceStatus.cpp \
	rift/OculusSDK/LibOVR/Src/OVR_Win32_HIDDevice.cpp \
	rift/OculusSDK/LibOVR/Src/OVR_Win32_HMDDevice.cpp \
	rift/OculusSDK/LibOVR/Src/Util/Util_LatencyTest.cpp \
	rift/OculusSDK/LibOVR/Src/OVR_Win32_SensorDevice.cpp \
	rift/OculusSDK/LibOVR/Src/Util/Util_MagCalibration.cpp \
	rift/OculusSDK/LibOVR/Src/Util/Util_Render_Stereo.cpp

PDGLUI_OVR_SRCS2 = \
	rift/ovr_glue.cpp

# PDGLUI_AUX_SRCS = $(PDGLUI_OVR_SRCS)
# PDGLUI_AUX_SRCS2 = $(PDGLUI_OVR_SRCS2)

PDGLUI_AUX_SRCS = 
PDGLUI_AUX_SRCS2 = 

PDGL_HEAD = \
	include/pdgl/auto.h \
	include/pdgl/tyauto.h

PDGLUI_HEAD = \
	include/pdglui/auto.h \
	include/pdglui/tyauto.h


autohead$(BINSUF): autohead.c
	$(CC_OUTTOOL)autohead$(BINSUF) autohead.c

adler32$(BINSUF): adler32.c
	$(CC_OUTTOOL)adler32$(BINSUF) adler32.c

include/pdgl/auto.h: autohead$(BINSUF) $(PDGL_SRCS)
	./autohead$(BINSUF) -api PDGL_API $(PDGL_SRCS) > include/pdgl/auto.h

include/pdgl/tyauto.h: adler32$(BINSUF) include/pdgl/types.txt
	./adler32$(BINSUF) -header include/pdgl/types.txt include/pdgl/tyauto.h

include/pdglui/auto.h: autohead$(BINSUF) $(PDGLUI_SRCS) $(PDGLUI_AUX_SRCS2)
	./autohead$(BINSUF) -api PDGLUI_API $(PDGLUI_SRCS) \
		$(PDGLUI_OVR_SRCS2) > include/pdglui/auto.h

include/pdglui/tyauto.h: adler32$(BINSUF) include/pdglui/types.txt
	./adler32$(BINSUF) -header include/pdglui/types.txt include/pdglui/tyauto.h

libpdgl$(DLLSUF): $(PDGL_SRCS)
	$(CC_OUTDLL)libpdgl$(DLLSUF) $(PDGL_SRCS) \
		$(DLL_CFLAGS) $(DLL_LDFLAGS)

libpdglui$(DLLSUF): $(PDGLUI_SRCS) $(PDGLUI_AUX_SRCS) $(PDGLUI_AUX_SRCS2)
	$(CC_OUTDLL)libpdglui$(DLLSUF) $(PDGLUI_SRCS) \
		$(PDGLUI_AUX_SRCS) $(PDGLUI_AUX_SRCS2) \
		$(DLL_CFLAGS2) $(DLL_LDFLAGS2)

includes: $(PDGL_HEAD) $(PDGLUI_HEAD)
libs: libpdgl$(DLLSUF) libpdglui$(DLLSUF)
apps:

clean:
