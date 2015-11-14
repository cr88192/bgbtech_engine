#	base/memory2_2$(OBJSUF) \
#	base/int128$(OBJSUF) \

#	base/dyll_frame$(OBJSUF) \
#	base/dyll_frame_x86$(OBJSUF) \
#	base/dyll_frame_x64$(OBJSUF) \
#	base/dyc_rcit$(OBJSUF) \

OBJS=\
	base/prng$(OBJSUF) \
	base/int128$(OBJSUF) \
	base/float128$(OBJSUF) \
	\
	base/vfile2$(OBJSUF) \
	base/vfile2_buffer$(OBJSUF) \
	base/vfile2_dir$(OBJSUF) \
	base/vfile2_zip$(OBJSUF) \
	base/vfile2_zpak$(OBJSUF) \
	base/inflate$(OBJSUF) \
	base/deflate$(OBJSUF) \
	base/vfile2_exwad$(OBJSUF) \
	base/inflate2$(OBJSUF) \
	\
	base/netval$(OBJSUF) \
	\
	base/dyll_func$(OBJSUF) \
	base/dyll_func2$(OBJSUF) \
	base/dyll_func2_auto$(OBJSUF) \
	base/dyll_sig$(OBJSUF) \
	base/dyll_sig_arg$(OBJSUF) \
	base/dyll_sig_flags$(OBJSUF) \
	base/dyll_sig_trans$(OBJSUF) \
	base/dyll_addr$(OBJSUF) \
	base/dyll_sigcache$(OBJSUF) \
	base/dyll_metapath$(OBJSUF) \
	base/dyll_catch$(OBJSUF) \
	base/dyll_thunk$(OBJSUF) \
	base/dyll_typebox$(OBJSUF) \
	base/dyll_mrbc2$(OBJSUF) \
	base/dyll_iface$(OBJSUF) \
	base/dyll_struct$(OBJSUF) \
	\
	base/dyll_exwad$(OBJSUF) \
	\
	base/ty_complex$(OBJSUF) \
	base/ty_quat$(OBJSUF) \
	base/ty_matrix$(OBJSUF) \
	\
	base/dy_wref$(OBJSUF) \
	base/dy_strarith$(OBJSUF) \
	base/dy_chan$(OBJSUF) \
	base/dy_dytf2$(OBJSUF) \
	\
	base/dy_method$(OBJSUF) \
	base/dy_oo$(OBJSUF) \
	base/dy_dyo$(OBJSUF) \
	base/dy_access$(OBJSUF) \
	\
	base/dyc_class$(OBJSUF) \
	base/dyc_lookup$(OBJSUF) \
	base/dyc_hash$(OBJSUF) \
	base/dyc_proto$(OBJSUF) \
	base/dyc_struct$(OBJSUF) \
	base/dyc_ns$(OBJSUF) \
	base/dyc_array$(OBJSUF) \
	base/dyc_api$(OBJSUF) \
	base/dyc_dyii$(OBJSUF) \
	\
	base/dyc_jni_fcn$(OBJSUF) \
	base/dyc_jni_iface$(OBJSUF) \
	base/dyc_jvmti_fcn$(OBJSUF) \
	base/dyc_jvmti_iface$(OBJSUF) \
	\
	base/dy_opr$(OBJSUF) \
	base/dy_cons$(OBJSUF) \
	base/dy_sparse$(OBJSUF) \
	base/dy_smxl$(OBJSUF) \
	base/dy_array$(OBJSUF) \
	base/dy_string$(OBJSUF) \
	\
	base/dytf_cons$(OBJSUF) \
	base/dytf_oo$(OBJSUF) \
	base/dytf_opr$(OBJSUF) \
	base/dytf_packvec$(OBJSUF) \
	\
	base/dy_xml$(OBJSUF) \
	base/dy_print$(OBJSUF) \
	base/dy_vli$(OBJSUF) \
	base/dys_binenc$(OBJSUF) \
	base/dys_bindec$(OBJSUF) \
	base/dys_parse$(OBJSUF) \
	base/dys_print$(OBJSUF) \
	base/dyx_parse$(OBJSUF) \
	base/dyx_print$(OBJSUF) \
	base/dyx_sbxe$(OBJSUF) \
	base/dyx_print_ze$(OBJSUF) \
	\
	base/dysh_console$(OBJSUF) \
	base/dysh_shell$(OBJSUF) \
	\
	base/bgbdy_api$(OBJSUF) \
	\
	bgal/bgal_main$(OBJSUF) \
	bgal/bgal_interp$(OBJSUF) \
	bgal/bgal_neuron$(OBJSUF)

SRCS=$(OBJS:$(OBJSUF)=.c)

DEP_HDRS = \
	include/bgbdy_auto.h \
	include/bgbdy_autoi.h \
	include/dyc_auto.h

all: libs apps

FORCE:

libs: $(DLLPF)bgbdy$(DLLSUF)
apps: dytst0$(EXESUF) tst_class0$(EXESUF) gatst0$(EXESUF)

includes: $(DEP_HDRS)

include/bgbdy_auto.h: autohead$(BINSUF) $(SRCS)
	./autohead$(BINSUF) -apionly BGBDY_API $(SRCS) > include/bgbdy_auto.h
include/bgbdy_autoi.h: autohead$(BINSUF) $(SRCS)
	./autohead$(BINSUF) -noapi BGBDY_API $(SRCS) > include/bgbdy_autoi.h

include/dyc_auto.h: autohead$(BINSUF) base/dyc_api.c base/dyc_array.c
	./autohead$(BINSUF) -api BGBDY_API base/dyc_api.c \
		base/dyc_array.c > include/dyc_auto.h

%$(OBJSUF): %.c
	$(CC_OUTOBJ)$@ $< $(CFLAGS)

# bgbdy.lib: $(DEP_HDRS) $(OBJS)
#	lib /out:bgbdy.lib $(OBJS)

$(DLLPF)bgbdy$(DLLSUF): $(DEP_HDRS) $(SRCS)
	$(CC_OUTDLL)$(DLLPF)bgbdy$(DLLSUF) $(SRCS) $(DLL_CFLAGS) $(DLL_LDFLAGS)

autohead$(BINSUF): autohead.c
	$(CC_OUTTOOL)autohead$(BINSUF) autohead.c

dytst0$(EXESUF): dytst0.c $(DEP_HDRS) libs
	$(CC_OUTEXE)dytst0$(EXESUF) dytst0.c $(CFLAGS) $(LDFLAGS)

tst_class0$(EXESUF): tst_class0.c $(DEP_HDRS) libs
	$(CC_OUTEXE)tst_class0$(EXESUF) tst_class0.c $(CFLAGS) $(LDFLAGS)

gatst0$(EXESUF): gatst0.c $(DEP_HDRS) libs
	$(CC_OUTEXE)gatst0$(EXESUF) gatst0.c $(CFLAGS) $(LDFLAGS)

clean:
