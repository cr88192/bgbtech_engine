BMID_SRCS = \
	base/bm_util.c \
	base/bm_wave.c \
	base/bm_patch.c \
	base/bm_synth.c \
	base/bm_stream.c \
	base/bm_mix.c \
	base/bm_voxenv.c \
	base/bm_file.c \
	base/bm_api.c \
	\
	base/bm_codec.c \
	base/bmac_adpcm.c \
	\
	base/snd_stub.c \
	base/snd_win2.c \
	base/sys_generic.c \
	\
	base/bm_rec_stt.c \
	base/snd_rec_win.c \
	base/bmsr/bmsr_match.c \
	\
	base/bm_btac_filter3.c \
	base/bm_btac_filter4.c \
	base/bm_btac_filter5.c \
	base/bm_btac_sample.c \
	base/bm_pack.c \
	\
	base/btac2_common.c \
	base/btac2_encode.c \
	base/btac2_decode.c \
	base/btac2_dct.c \
	\
	base/mpglib/layer1.c \
	base/mpglib/layer2.c \
	base/mpglib/common.c \
	base/mpglib/dct64_i386.c \
	base/mpglib/decode_i386.c \
	base/mpglib/layer3.c \
	base/mpglib/tabinit.c \
	base/mpglib/interface.c \
	\
	base/tts/tts_lpcfir.c \
	base/tts/bsg_bitstream.c \
	base/tts/bsg_bsgen.c \
	base/tts/bsg_lexitem.c \
	base/tts/bsg_samp.c \
	base/tts/bsg_wavez.c \

#	base/tts/bsg_pack.c \

BMID_OBJS = $(BMID_SRCS:.c=.obj)

all: includes libs apps

autohead$(BINSUF): autohead.c
	$(CC_OUTTOOL)autohead$(BINSUF) autohead.c

include/bgbmid_auto.h: autohead$(BINSUF) $(BMID_SRCS)
	./autohead$(BINSUF) -api BMID_API $(BMID_SRCS) > include/bgbmid_auto.h

# libbgbmid.lib: $(BMID_OBJS)
#	lib /out:libbgbmid.lib $(BMID_OBJS)

libbgbmid$(DLLSUF): include/bgbmid_auto.h $(BMID_SRCS)
	$(CC_OUTDLL)libbgbmid$(DLLSUF) $(BMID_SRCS) $(CFLAGS_DLL)

dmid2$(EXESUF): dmid2.c libbgbmid$(DLLSUF)
	$(CC_OUTEXE)dmid2$(EXESUF) dmid2.c $(CFLAGS_APP)

mid2asc$(EXESUF): mid2asc.c libbgbmid$(DLLSUF)
	$(CC_OUTEXE)mid2asc$(EXESUF) mid2asc.c $(CFLAGS_APP)

asc2mid$(EXESUF): asc2mid.c libbgbmid$(DLLSUF)
	$(CC_OUTEXE)asc2mid$(EXESUF) asc2mid.c $(CFLAGS_APP)

tfmid$(EXESUF): tfmid0.c libbgbmid$(DLLSUF)
	$(CC_OUTEXE)tfmid$(EXESUF) tfmid0.c $(CFLAGS_APP)

btac0$(EXESUF): btac0.c libbgbmid$(DLLSUF)
	$(CC_OUTEXE)btac0$(EXESUF) btac0.c $(CFLAGS_APP)

bmpack0$(EXESUF): bmpack0.c libbgbmid$(DLLSUF)
	$(CC_OUTEXE)bmpack0$(EXESUF) bmpack0.c $(CFLAGS_APP)

bmtts0$(EXESUF): bmtts0.c libbgbmid$(DLLSUF)
	$(CC_OUTEXE)bmtts0$(EXESUF) bmtts0.c $(CFLAGS_APP)

bsgen3$(EXESUF): bsgen3.c libbgbmid$(DLLSUF)
	$(CC_OUTEXE)bsgen3$(EXESUF) bsgen3.c $(CFLAGS_APP)

bm_filt1bpp$(EXESUF): bm_filt1bpp.c libbgbmid$(DLLSUF)
	$(CC_OUTEXE)bm_filt1bpp$(EXESUF) bm_filt1bpp.c $(CFLAGS_APP)

bm_bsg1bpp$(EXESUF): bm_bsg1bpp.c libbgbmid$(DLLSUF)
	$(CC_OUTEXE)bm_bsg1bpp$(EXESUF) bm_bsg1bpp.c $(CFLAGS_APP)

clean:

includes:
libs: libbgbmid$(DLLSUF)
apps: dmid2$(EXESUF) mid2asc$(EXESUF) asc2mid$(EXESUF) tfmid$(EXESUF) btac0$(EXESUF) \
	bmpack0$(EXESUF) bmtts0$(EXESUF) bsgen3$(EXESUF) \
	bm_filt1bpp$(EXESUF) bm_bsg1bpp$(EXESUF)

