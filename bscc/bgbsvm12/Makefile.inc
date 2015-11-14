OBJS=\
	svm/bsvm_ops$(OBJSUF) \
	svm/st_pkg2c$(OBJSUF) \
	svm/bssrc_auto$(OBJSUF) \
	\
	interp/si_base$(OBJSUF) \
	interp/si_baseops$(OBJSUF) \
	interp/si_interp$(OBJSUF) \
	interp/si_load$(OBJSUF) \
	interp/si_scope$(OBJSUF) \
	interp/si_statics$(OBJSUF) \
	\
	interp/si_thread$(OBJSUF) \
	interp/si_threadop$(OBJSUF) \
	interp/si_threadop2$(OBJSUF) \
	interp/si_thop_bvt$(OBJSUF) \
	interp/si_thop_array$(OBJSUF) \
	interp/si_thop_intrin$(OBJSUF) \
	interp/si_thop_ptr$(OBJSUF) \
	interp/si_fastbvt$(OBJSUF) \
	\
	interp/si_thread_trace$(OBJSUF) \
	interp/si_thread_x86$(OBJSUF) \
	\
	interp/sjx86_thop$(OBJSUF) \
	\
	compile/sc_compile$(OBJSUF) \
	compile/sc_reduce$(OBJSUF) \
	compile/sc_reverse$(OBJSUF) \
	compile/sc_index$(OBJSUF) \
	compile/sc_expr$(OBJSUF) \
	compile/sc_expr_arith$(OBJSUF) \
	\
	parse2/parse_token$(OBJSUF) \
	parse2/parse$(OBJSUF) \
	parse2/parse_expr2$(OBJSUF) \
	parse2/parse_tyexpr$(OBJSUF) \
	parse2/parse_stmt$(OBJSUF) \
	\
	builtin/builtin_base$(OBJSUF) \
	builtin/builtin_math$(OBJSUF) \
	\
	jit2/j_arch$(OBJSUF) \
	jit2/j_arith$(OBJSUF) \
	jit2/j_cast$(OBJSUF) \
	jit2/j_dyt$(OBJSUF) \
	jit2/j_jit2$(OBJSUF) \
	jit2/j_mem$(OBJSUF) \
	jit2/j_reg$(OBJSUF) \
	jit2/j_stack$(OBJSUF) \
	jit2/j_stackop$(OBJSUF) \
	jit2/j_tvar$(OBJSUF) \
	jit2/j_type$(OBJSUF) \
	jit2/j_vars$(OBJSUF) \
	\
	jit2/x86/jix86_arith$(OBJSUF) \
	jit2/x86/jix86_frame$(OBJSUF) \
	jit2/x86/jix86_mem$(OBJSUF) \
	jit2/x86/jix86_reg$(OBJSUF) \
	\
	jit2/j_core4$(OBJSUF) \
	\
	bsasm/bs2a_token.c \
	bsasm/bs2a_opcode.c \
	bsasm/bs2a_entparse.c \
	bsasm/bs2a_tagent.c \
	\
	bsasm/bs2a_api.c \
	bsasm/bs2a_vli.c \
	bsasm/bs2a_block.c \
	bsasm/bs2a_literal.c \
	bsasm/bs2a_lump.c \

#	compile/sc_gast$(OBJSUF) \

SRCS=$(OBJS:$(OBJSUF)=.c)

DEP_HDRS=\
	include/bsvm_ops.h \
	include/bsvm_auto.h

# BS_SRCS = $(shell find bssrc -name \*.bs)

BS_SRCS = \
	bssrc/bsvm/init/Exceptions.bs \
	bssrc/bsvm/init/Errors.bs \
	bssrc/bsvm/init.bs \
	\
	interp/asm_thread_x86.asm

BS_ADDSCRIPTS = /add $(BS_SRCS)
BS_DUMP = /Fnbgbsvm_meta.txt

all: libs apps

FORCE:

libs: $(DLLPF)bgbsvm$(DLLSUF)

# bsvmrt.bsar

apps: bsvm1cc$(EXESUF) bspkg2c$(EXESUF)

includes: $(DEP_HDRS)

include/bsvm_auto.h: autohead$(BINSUF) $(SRCS)
	./autohead$(BINSUF) -api BGBSVM_API $(SRCS) > include/bsvm_auto.h

include/bsvm_ops.h: genops$(EXESUF) bsvm_ops.txt
	./genops$(EXESUF)

%$(OBJSUF): %.c
	$(CC_OUTOBJ)$@ $< $(CFLAGS)

$(DLLPF)bgbsvm$(DLLSUF): $(DEP_HDRS) $(SRCS)
	$(CC_OUTDLL)$(DLLPF)bgbsvm$(DLLSUF) $(SRCS) \
		$(DLL_CFLAGS) $(DLL_LDFLAGS)
	$(BGBMETA_EXW)$(DLLPF)bgbsvm$(DLLSUF) $(BS_DUMP) bgbsvm.h \
		$(BS_ADDSCRIPTS) $(BMETA_CFLAGS)

autohead$(BINSUF): autohead.c
	$(CC_OUTTOOL)autohead$(BINSUF) autohead.c

genops$(EXESUF): genops.c
	$(CC_OUTTOOL)genops$(EXESUF) genops.c

txtclump$(EXESUF): txtclump.c
	$(CC_OUTTOOL)txtclump$(EXESUF) txtclump.c

svm/bssrc_auto.c: txtclump$(EXESUF) $(BS_SRCS)
	./txtclump$(EXESUF) -o svm/bssrc_auto.c -v bsvm_bssrc_clump \
		-eat "bssrc/" $(BS_SRCS)

bsvm1cc$(EXESUF): bs1cc.c $(DEP_HDRS) libs
	$(CC_OUTEXE)bsvm1cc$(EXESUF) bs1cc.c bgbsvm.lib $(CFLAGS) $(LDFLAGS)

bspkg2c$(EXESUF): bspkg2c.c $(DEP_HDRS) libs
	$(CC_OUTEXE)bspkg2c$(EXESUF) bspkg2c.c bgbsvm.lib $(CFLAGS) $(LDFLAGS)

# bsvmrt.bsar: FORCE
#	jar -cvf bsvmrt.bsar -C bssrc .

clean:
clean2:
