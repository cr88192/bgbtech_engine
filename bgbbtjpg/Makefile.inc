THEORA_SRCS = \
	libtheora_mini/lib/bitwise.c \
	libtheora_mini/lib/framing.c \
	\
	libtheora_mini/lib/apiwrapper.c \
	libtheora_mini/lib/bitpack.c \
	libtheora_mini/lib/decapiwrapper.c \
	libtheora_mini/lib/decinfo.c \
	libtheora_mini/lib/decode.c \
	libtheora_mini/lib/dequant.c \
	libtheora_mini/lib/fragment.c \
	libtheora_mini/lib/huffdec.c \
	libtheora_mini/lib/idct.c \
	libtheora_mini/lib/info.c \
	libtheora_mini/lib/internal.c \
	libtheora_mini/lib/quant.c \
	libtheora_mini/lib/state.c \
	\
	libtheora_mini/lib/analyze.c \
	libtheora_mini/lib/fdct.c \
	libtheora_mini/lib/encfrag.c \
	libtheora_mini/lib/encapiwrapper.c \
	libtheora_mini/lib/encinfo.c \
	libtheora_mini/lib/encode.c \
	libtheora_mini/lib/enquant.c \
	libtheora_mini/lib/huffenc.c \
	libtheora_mini/lib/mathops.c \
	libtheora_mini/lib/mcenc.c \
	libtheora_mini/lib/rate.c \
	libtheora_mini/lib/tokenize.c \

SRCS = \
	base/inflate.c \
	base/deflate.c \
	base/png.c \
	base/bufpng.c \
	base/pcx.c \
	base/tga.c \
	base/wal.c \
	base/img_util.c \
	base/out_avi.c \
	base/img_btv0.c \
	base/img_btic1.c \
	base/img_btic1_dec.c \
	\
	base/btlza_bitrange.c \
	base/btlza_bitdecode.c \
	base/btlza_bitencode.c \
	base/btlza_bitstream.c \
	base/btlza_decode.c \
	base/btlza_encode.c \
	base/btlza_fastdfl.c \
	\
	base/btj_bitrange.c \
	base/btj_bitrange2.c \
	base/btj_common.c \
	base/btj_dct.c \
	base/btj_dct2.c \
	base/btj_decode.c \
	base/btj_encode.c \
	base/btj_huff.c \
	base/btj_declayer.c \
	\
	base/btj_encfast.c \
	\
	base/rpza_decode.c \
	base/rpza_encode.c \
	base/rpza_quant.c \
	\
	base/btic1c_dec_bc4.c \
	base/btic1c_dec_bc6h.c \
	base/btic1c_dec_bc7.c \
	base/btic1c_dec_comm.c \
	base/btic1c_dec_dxtn.c \
	base/btic1c_dec_mb5c.c \
	base/btic1c_decode.c \
	base/btic1c_encode.c \
	base/btic1c_encfast.c \
	base/btic1c_metablock2.c \
	base/btic1c_quant.c \
	base/btic1c_tonemap.c \
	\
	base/btic1d_block.c \
	base/btic1d_block2.c \
	base/btic1d_decode.c \
	base/btic1d_encode.c \
	base/btic1d_quant.c \
	\
	base/btic1x_metablock.c \
	base/btic1x_palette.c \
	\
	base/btic2c_decode.c \
	base/btic2c_dec_clrs.c \
	base/btic2c_encode.c \
	base/btic2c_common.c \
	base/btic2c_arith.c \
	\
	base/bcn_filter.c \
	base/bcn_packimg.c \
	base/bcn_filt_lbr.c \
	base/bcn_filt_dxt1.c \
	base/bcn_filt_dxt5.c \
	base/bcn_filt_dxt5_uvay.c \
	base/bcn_filt_bc4.c \
	base/bcn_filt_mb5c.c \
	base/bcn_filt_mb5c2.c \
	base/bcn_filt_mb5c4hf.c \
	base/bcn_fastmip.c \
	\
	base/bcn_bitstream.c \
	base/bcn_float8.c \
	base/bcn_float16.c \
	base/bc6_filter.c \
	base/bc7_filter.c \
	base/bc7_filt_part2.c \
	base/bc7_filt_part3.c \
	\
	base/avi_base.c \
	base/avi_btic.c \
	base/avi_codec.c \
	base/avi_mbtc.c \
	base/avi_mjpg.c \
	base/avi_mpng.c \
	base/avi_theora.c \
	base/avi_vfw.c \
	\
	base/vlic_base.c \
	\
	base/btjdrv_base.c \
	base/btjdrv_vlcglue.c \
	base/btjvfw_driverproc.c \

#	base/jpg2.c \

AUX_SRCS = \
	$(THEORA_SRCS)

DEP_HDRS = \
	include/bgbbtj_auto.h

# all: awal$(EXESUF) genavi$(EXESUF)
all: includes libs apps

apps: genavi$(EXESUF) animavi$(EXESUF) decavi$(EXESUF) \
	btjpack$(EXESUF) upsampavi$(EXESUF) \
	colortst0$(EXESUF) vlicavi$(EXESUF) \
	bptctst0$(EXESUF)

libs: $(DLLPF)bgbbtjpg$(DLLSUF) $(TGT_LIBS)

drv: $(DRV_LIBS)

includes: $(DEP_HDRS)

# awal$(EXESUF): $(WALPNG_SRCS) awal.c
#	cl /Feawal$(EXESUF) /I. $(WALPNG_SRCS) awal.c /Z7

include/bgbbtj_auto.h: autohead$(BINSUF) $(SRCS)
	./autohead$(BINSUF) -api BGBBTJ_API $(SRCS) > include/bgbbtj_auto.h

$(DLLPF)bgbbtjpg$(DLLSUF): $(DEP_HDRS) $(SRCS) $(AUX_SRCS)
	$(CC_OUTDLL)$(DLLPF)bgbbtjpg$(DLLSUF) $(SRCS) $(AUX_SRCS) \
		$(DLL_CFLAGS) $(DLL_LDFLAGS)

$(DLLPF)bgbtjvfw$(DLLSUF): $(DEP_HDRS) $(SRCS)
	$(CC_OUTDLL)$(DLLPF)bgbtjvfw$(DLLSUF) $(SRCS) \
		$(DLL_CFLAGS_VFWDRV) $(DLL_LDFLAGS_VFWDRV)

libbgbtjvlc_plugin$(DLLSUF): $(DEP_HDRS) $(SRCS)
	$(CC_OUTDLL)libbgbtjvlc_plugin$(DLLSUF) $(SRCS) \
		$(DLL_CFLAGS_VLCDRV) $(DLL_LDFLAGS_VLCDRV)

$(DLLPF)bgbtjvfw$(ARCH)$(DLLSUF): $(DEP_HDRS) $(SRCS)
	$(CC_OUTDLL)$(DLLPF)bgbtjvfw$(ARCH)$(DLLSUF) $(SRCS) \
		$(DLL_CFLAGS_VFWDRV) $(DLL_LDFLAGS_VFWDRV)

libbgbtjvlc_plugin$(ARCH)$(DLLSUF): $(DEP_HDRS) $(SRCS)
	$(CC_OUTDLL)libbgbtjvlc_plugin$(ARCH)$(DLLSUF) $(SRCS) \
		$(DLL_CFLAGS_VLCDRV) $(DLL_LDFLAGS_VLCDRV)

autohead$(BINSUF): autohead.c
	$(CC_OUTTOOL)autohead$(BINSUF) autohead.c


genavi$(EXESUF): $(SRCS) genavi.c
	$(CC_OUTEXE)genavi$(EXESUF) genavi.c $(CFLAGS) $(LDFLAGS)

animavi$(EXESUF): $(SRCS) animavi.c
	$(CC_OUTEXE)animavi$(EXESUF) animavi.c $(CFLAGS) $(LDFLAGS)

decavi$(EXESUF): $(SRCS) decavi.c
	$(CC_OUTEXE)decavi$(EXESUF) decavi.c $(CFLAGS) $(LDFLAGS)

btjpack$(EXESUF): $(SRCS) btjpack0.c
	$(CC_OUTEXE)btjpack$(EXESUF) btjpack0.c $(CFLAGS) $(LDFLAGS)

upsampavi$(EXESUF): $(SRCS) upsampavi.c
	$(CC_OUTEXE)upsampavi$(EXESUF) upsampavi.c $(CFLAGS) $(LDFLAGS)

colortst0$(EXESUF): $(SRCS) colortst0.c
	$(CC_OUTEXE)colortst0$(EXESUF) colortst0.c $(CFLAGS) $(LDFLAGS)

bptctst0$(EXESUF): $(SRCS) bptctst0.c
	$(CC_OUTEXE)bptctst0$(EXESUF) bptctst0.c $(CFLAGS) $(LDFLAGS)

vlicavi$(EXESUF): $(SRCS) vlicavi.c
	$(CC_OUTEXE)vlicavi$(EXESUF) vlicavi.c $(CFLAGS) $(LDFLAGS)

clean:
