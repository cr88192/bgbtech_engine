all: FORCE build

FORCE:

includes: FORCE
	$(MAKE) -C bgbasm21 -f Makefile$(MKSUF) includes
	cp -rf bgbasm21/include/*	$(BUILD)/include

	$(MAKE) -C bgbgc1 -f Makefile$(MKSUF) includes
	cp -rf bgbgc1/include/*		$(BUILD)/include

	$(MAKE) -C bgbdy -f Makefile$(MKSUF) includes
	cp -rf bgbdy/include/*		$(BUILD)/include

	$(MAKE) -C zpak2 -f Makefile$(MKSUF) includes
	cp -rf zpak2/*.h		$(BUILD)/include

#	$(MAKE) -C bgbsxil22 -f Makefile$(MKSUF) includes
#	cp -rf bgbsxil22/include/*	$(BUILD)/include

#	$(MAKE) -C bgbmcc -f Makefile$(MKSUF) includes
#	cp -rf bgbmcc/include/*		$(BUILD)/include

#	$(MAKE) -C bgbccc1 -f Makefile$(MKSUF) includes
#	cp -rf bgbccc1/include/*	$(BUILD)/include

#	$(MAKE) -C bgbbcx -f Makefile$(MKSUF) includes
#	cp -rf bgbbcx/include/*		$(BUILD)/include

	$(MAKE) -C bgbnet -f Makefile$(MKSUF) includes
	cp -rf bgbnet/include/*		$(BUILD)/include

	$(MAKE) -C bgbjvm -f Makefile$(MKSUF) includes
	cp -rf bgbjvm/include/*		$(BUILD)/include

	$(MAKE) -C bgbsvm12 -f Makefile$(MKSUF) includes
	cp -rf bgbsvm12/include/*	$(BUILD)/include

	$(MAKE) -C bgbvx86 -f Makefile$(MKSUF) includes
	cp -rf bgbvx86/include/*	$(BUILD)/include

	$(MAKE) -C bgbmeta -f Makefile$(MKSUF) includes
	cp -rf bgbmeta/include/*	$(BUILD)/include

#	$(MAKE) -C bgbjrt -f Makefile$(MKSUF) includes
#	cp -rf bgbjrt/include/*	$(BUILD)/include


libs: FORCE
	$(MAKE) -C bgbasm21 -f Makefile$(MKSUF) libs
#	cp bgbasm21/*$(LIBSUF)	$(BUILD)
	cp bgbasm21/*$(DLLSUF)	$(BUILD)
#	cp bgbasm21/*$(DLLSUF) bgbasm21/*$(LIBSUF) $(BUILD)

	$(MAKE) -C bgbgc1 -f Makefile$(MKSUF) libs
#	cp bgbgc1/*$(LIBSUF)		$(BUILD)
	cp bgbgc1/*$(DLLSUF)		$(BUILD)
#	cp bgbgc1/*$(DLLSUF) bgbgc1/*$(LIBSUF) $(BUILD)

	$(MAKE) -C zpak2 -f Makefile$(MKSUF) libs
#	cp zpak2/*$(LIBSUF)		$(BUILD)
	cp zpak2/*$(DLLSUF)		$(BUILD)
#	cp zpak2/*$(DLLSUF) zpak2/*$(LIBSUF) $(BUILD)

	$(MAKE) -C bgbdy -f Makefile$(MKSUF) libs
#	cp bgbdy/*$(LIBSUF)		$(BUILD)
	cp bgbdy/*$(DLLSUF)		$(BUILD)
#	cp bgbdy/*$(DLLSUF) bgbdy/*$(LIBSUF) $(BUILD)

#	$(MAKE) -C bgbsxil22 -f Makefile$(MKSUF) libs
#	cp bgbsxil22/*$(LIBSUF)	$(BUILD)
#	cp bgbsxil22/*$(DLLSUF)	$(BUILD)

#	$(MAKE) -C bgbmcc -f Makefile$(MKSUF) libs
#	cp bgbmcc/*$(LIBSUF)		$(BUILD)
#	cp bgbmcc/*$(DLLSUF)		$(BUILD)

#	$(MAKE) -C bgbccc1 -f Makefile$(MKSUF) libs
#	cp bgbccc1/*$(LIBSUF)	$(BUILD)
#	cp bgbccc1/*$(DLLSUF)	$(BUILD)

#	$(MAKE) -C bgbbcx -f Makefile$(MKSUF) libs
#	cp bgbbcx/*$(LIBSUF)		$(BUILD)
#	cp bgbbcx/*$(DLLSUF)		$(BUILD)

	$(MAKE) -C bgbnet -f Makefile$(MKSUF) libs
#	cp bgbjvm/*$(LIBSUF)		$(BUILD)
	cp bgbnet/*$(DLLSUF)		$(BUILD)

	$(MAKE) -C bgbjvm -f Makefile$(MKSUF) libs
#	cp bgbjvm/*$(LIBSUF)		$(BUILD)
	cp bgbjvm/*$(DLLSUF)		$(BUILD)

	$(MAKE) -C bgbsvm12 -f Makefile$(MKSUF) libs
#	cp bgbsvm12/*$(LIBSUF)	$(BUILD)
	cp bgbsvm12/*$(DLLSUF)	$(BUILD)

	$(MAKE) -C bgbvx86 -f Makefile$(MKSUF) libs
#	cp bgbvx86/*$(LIBSUF)	$(BUILD)
	cp bgbvx86/*$(DLLSUF)	$(BUILD)

	$(MAKE) -C bgbmeta -f Makefile$(MKSUF) libs
#	cp bgbmeta/*$(LIBSUF)	$(BUILD)
	cp bgbmeta/*$(DLLSUF)	$(BUILD)

#	$(MAKE) -C bgbjrt -f Makefile$(MKSUF) libs
#	cp bgbjrt/*$(LIBSUF)		$(BUILD)
#	cp bgbjrt/*$(DLLSUF)		$(BUILD)
#	cp bgbjrt/*.jar		$(BUILD)


apps: FORCE
	$(MAKE) -C bgbasm21 -f Makefile$(MKSUF) apps
	cp bgbasm21/*$(EXESUF)	$(BUILD)

	$(MAKE) -C bgbgc1 -f Makefile$(MKSUF) apps
	cp bgbgc1/*$(EXESUF)		$(BUILD)

	$(MAKE) -C bgbdy -f Makefile$(MKSUF) apps
	cp bgbdy/*$(EXESUF)		$(BUILD)

	$(MAKE) -C zpak2 -f Makefile$(MKSUF) apps
	cp zpak2/*$(EXESUF)		$(BUILD)

#	$(MAKE) -C bgbsxil22 -f Makefile$(MKSUF) apps
#	cp bgbsxil22/*$(EXESUF)	$(BUILD)

#	$(MAKE) -C bgbmcc -f Makefile$(MKSUF) apps
#	cp bgbmcc/*$(EXESUF)		$(BUILD)

#	$(MAKE) -C bgbccc1 -f Makefile$(MKSUF) apps
#	cp bgbccc1/*$(EXESUF)	$(BUILD)

#	$(MAKE) -C bgbbcx -f Makefile$(MKSUF) apps
#	cp bgbbcx/*$(EXESUF)		$(BUILD)

	$(MAKE) -C bgbnet -f Makefile$(MKSUF) apps
	cp bgbnet/*$(EXESUF)		$(BUILD)

	$(MAKE) -C bgbjvm -f Makefile$(MKSUF) apps
	cp bgbjvm/*$(EXESUF)		$(BUILD)

	$(MAKE) -C bgbsvm12 -f Makefile$(MKSUF) apps
	cp bgbsvm12/*$(EXESUF)	$(BUILD)

	$(MAKE) -C bgbvx86 -f Makefile$(MKSUF) apps
	cp bgbvx86/*$(EXESUF)	$(BUILD)

	$(MAKE) -C bgbmeta -f Makefile$(MKSUF) apps
#	cp bgbmeta/*$(EXESUF)	$(BUILD)

meta: FORCE
	./bgbmeta$(BINSUF) /Fnbgbasm_meta.txt	bgbasm.h /Iinclude
	./bgbmeta$(BINSUF) /Fnbgbgc_meta.txt		bgbgc.h /Iinclude
	./bgbmeta$(BINSUF) /Fnbgbdy_meta.txt		bgbdy.h /Iinclude
	./bgbmeta$(BINSUF) /Fnzpack_meta.txt		zpack.h /Iinclude
	./bgbmeta$(BINSUF) /Fnbgbsxil_meta.txt	sxil.h /Iinclude
	./bgbmeta$(BINSUF) /Fnbgbcc_meta.txt		bgbccc.h /Iinclude
	./bgbmeta$(BINSUF) /Fnbgbjvm_meta.txt	bgbjvm.h /Iinclude
	./bgbmeta$(BINSUF) /Fnbgbsvm_meta.txt	bgbsvm.h /Iinclude
	./bgbmeta$(BINSUF) /Fnbgbvx86_meta.txt	bgbvx86.h /Iinclude

tests: FORCE
#	./bgbcc$(EXESUF) -c -o fibtst.o fibtst.c /FZy /DPF=cc_
#	cl /c /DPF=rf_ /Fofibtst_r.o fibtst.c
#	cl /Fetst_fib$(EXESUF) tester.c fibtst.o fibtst_r.o

#	./bgbcc$(EXESUF) -c -o sorttest1_c.o sorttest1.c /FZy /DPF=cc_
#	cl /c /Fosorttest1_r.o sorttest1.c /DPF=rf_
#	cl /Fetst_sort$(EXESUF) tester.c sorttest1_c.o sorttest1_r.o

#	./bgbcc$(EXESUF) -c -o test_hash2_c.o test_hash2.c /FZy /DPF=cc_
#	cl /c /Fotest_hash2_r.o test_hash2.c /DPF=rf_
#	cl /Fetst_hash$(EXESUF) tester.c test_hash2_c.o test_hash2_r.o

build: includes libs apps

# meta tests FORCE

install:
	cp -rf include/*	$(INST_INC)
	cp *$(DLLSUF)		$(INST_DLL)
	cp *$(EXESUF)		$(INST_BIN)
	cp *_meta.txt		$(INST_META)
	cp *.jar		$(INST_LIB)
	cp *$(LIBSUF)		$(INST_LIB)

clean: FORCE
#	del /S *.obj
#	del /S *$(LIBSUF)

clean2: FORCE
#	del /S *.obj
#	del /S *$(LIBSUF)
#	del /S *$(EXESUF)
#	rm -rf $(shell find -name \*$(BIN))
#	rm -rf $(shell find -name \*\~)

clean_o: FORCE
#	rm -rf $(shell find -name \*.obj)

clean_a: FORCE
#	rm -rf $(shell find -name \*$(LIBSUF))
