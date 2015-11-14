#include <bgbgc.h>
#include <bgbdy.h>
		
struct DYLL_Iface_s dyll_iface={
NULL,							// 0
NULL,							// 1
NULL,							// 2
NULL,							// 3

dyllGetAddr,					// 4
dyllGetAddrName,				// 5
dyllSetAddr,					// 6

dyllGetPtr,						// 7
dyllSetPtr,						// 8
dyllGetInt,						// 9
dyllSetInt,						// 10
dyllGetLong,					// 11
dyllSetLong,					// 12
dyllGetFloat,					// 13
dyllSetFloat,					// 14
dyllGetDouble,					// 15
dyllSetDouble,					// 16

dyllGetDynamic,					// 17
dyllSetDynamic,					// 18

dyllNewEnv,						// 19
dyllEnvBindAddr,				// 20
dyllEnvBindAddrSig,				// 21
dyllEnvSetAddr,					// 22
dyllEnvGetAddr,					// 23
dyllEnvGetSig,					// 24
dyllEnvAllocVar,				// 25
dyllThunkEnv,					// 26
dyllThunkBindAddr,				// 27
dyllThunkBindAddrSig,			// 28
dyllThunkSetAddr,				// 29
dyllThunkGetAddr,				// 30
dyllThunkGetSig,				// 31

dyllAsmThunk,					// 32
dyllAsmThunkEnv,				// 33
dyllAsmThunkSuperEnv,			// 34
dyllAsmThunkInline,				// 35
dyllAsmThunkInlineEnv,			// 36
dyllAsmThunkInlineSuperEnv,		// 37
dyllAsmBegin,					// 38
dyllAsmEnd,						// 39
dyllAsmBeginThunkInline,		// 40
dyllAsmEndThunkInline,			// 41
dyllAsmPuts,					// 42
dyllAsmPrintv,					// 43
dyllAsmPrint,					// 44
dyllAsmRegisterLinkMeta,		// 45
dyllAsmRegisterLinkNotify,		// 46

dyllComposeLinkMetaName,		// 47
dyllComposeLinkNotifyName,		// 48
dyllComposeLinkMetaName0,		// 49
dyllComposeLinkMetaName1,		// 50
dyllComposeLinkMetaName2,		// 51
dyllComposeLinkMetaName3,		// 52
dyllComposeLinkMetaName4,		// 53
dyllGetAddrSig,					// 54
dyllGetNameSig,					// 55

dyllApplyPtrV,					// 56
dyllApplyPtrU,					// 57
dyllApplyPtrVi,					// 58
dyllApplyPtrVl,					// 59
dyllApplyPtrVf,					// 60
dyllApplyPtrVd,					// 61
dyllApplyPtrVp,					// 62
dyllApplyPtrObjV,				// 63
dyllApplyPtrObjU,				// 64
dyllApplyPtrSigV,				// 65
dyllApplyPtrSigU,				// 66
dyllApplyPtrSigVi,				// 67
dyllApplyPtrSigVl,				// 68
dyllApplyPtrSigVf,				// 69
dyllApplyPtrSigVd,				// 70
dyllApplyPtrSigVp,				// 71
dyllApplyPtrSigObjV,			// 72
dyllApplyPtrSigObjU,			// 73

dyllWrapClosure,				// 74
dyllLoadLibrary,				// 75

dyllMetaInit,					// 76
dyllMetaCommit,					// 77
dyllMetaLoadDB,					// 78
dyllMetaSaveDB,					// 79
dyllMetaBindKey,				// 80
dyllMetaLookupKey,				// 81

dyllGetObject,					// 82
dyllSetObject,					// 83

dyllSigQuickSize,				// 84
dyllSigAlignSize,				// 85
dyllSigSizeAlign,				// 86
dyllSigNext,					// 87
dyllSigSizeListEsig,			// 88
dyllSigSizeList,				// 89

dyllGetVarSig,					// 90
dyllSetVarSig,					// 91

dyllSigRet,						// 92
dyllCheckCallSig,				// 93

dyllGenSym						// 94
};

struct DYO_Iface_s dyo_iface = {
NULL,							// 0
NULL,							// 1
NULL,							// 2
NULL,							// 3

//func
dyRawFunc,						//
dyRawClose,						//
dyRawMethod,					//
dyRawMethodData,				//
dyRawStaticMethod,				//
dyRawStaticMethodData,			//

dyRawFuncP,						//
dyllGenSym,						//
dyToString,						//

dyllConvObjCFunc,				//

//method
dyApplyMethod,					//
dyApplyMethodLst,				//
dyApplyMethodU,					//
dyApplyMethodV,					//

//basic OO
dyTypeGetSlot,					//
dyTypeSetSlot,					//
dyTypeNextSlot,					//
dyTypeTrySetSlot,				//
dyTypeGetDelegate,				//
dyTypeSetDelegate,				//
dyTypeNextDelegate,				//
dyTypeCallMethod,				//
dyTypeApply,					//
dyTypeApplyObj,					//
dyTypeToString,					//

dyObject,						//
dyObjectParent,					//
dyObjectp,						//
dyObjectP,						//
dyBind,							//
dyTrySet,						//
dyGet,							//
dyGeti,							//
dyGetl,							//
dyGetf,							//
dyGetd,							//
dyGet2fv,						//
dyGet3fv,						//
dyGet4fv,						//
dyGet2dv,						//
dyGet3dv,						//
dyGet4dv,						//
dyGets,							//
dySet,							//
dySeti,							//
dySetl,							//
dySetf,							//
dySetd,							//
dySet2f,						//
dySet3f,						//
dySet4f,						//
dySet2fv,						//
dySet3fv,						//
dySet4fv,						//
dySet2dv,						//
dySet3dv,						//
dySet4dv,						//
dySets,							//
dyGetIdx,						//
dySetIdx,						//
dyGetKey,						//
dySetKey,						//
dyGetDelegate,					//
dySetDelegate,					//
dyGetParent,					//
dySetParent,					//
dyBindMethod,					//
dyDefMethod,					//
dyDefMethodN,					//
dyCall,							//
dyCallN,						//
dyCallSig,						//
dyLookupMethodSig,				//
};


struct DY_Iface_s dy_iface = {
NULL,							// 0
NULL,							// 1
NULL,							// 2
NULL,							// 3

//se/dy_opr.c
dyadd,						//
dysub,						//
dymul,						//
dydiv,						//
dymod,						//
dyand,						//
dyor,						//
dyxor,						//
dyshl,						//
dyshr,						//
dyexp,						//
dyneg,						//
dynot,						//
dylnot,						//
dyeqp,						//
dyneqp,						//
dyltp,						//
dygtp,						//
dylep,						//
dygep,						//
dytruep,						//
dyfalsep,						//
dysqr,						//
dysqrt,						//
//se/dy_cons.c
dycons,						//
dyconsp,						//
dylistp,						//
dylistlen,						//
dyFreeCons,						//
dyFreeList,						//
dyFormIsP,						//
dycar,						//
dycdr,						//
dysetcar,						//
dysetcdr,						//
dyreverse,						//
dynreverse,						//
dynappend,						//
dyListToArray,						//
dylistcopy,						//
dyappend,						//
//se/dy_array.c
dyarray,						//
dyarrayp,						//
dyarrayidx,						//
dyarraysetidx,						//
*dyarrayv,						//
dyarraysz,						//
dyWrapArray,						//
//se/dy_print.c
dyPrintBuf,						//
dyDumpFD,						//
dyPrintFD,						//
dyDumpGCP,						//
dyPrintGCP,						//
dyDumpStr,						//
dyPrintStr,						//
dyPrint,						//
//se/dys_parse.c
dysParseFromString,						//
dysParseMultiFromString,						//
//se/dys_print.c
dysDumpFD,						//
dysPrintFD,						//
dysDumpStr,						//
dysPrintStr,						//
dysPrint,						//
dysPrintln,						//
//se/bgbdy_api.c
dy_init,						//
dy_init2,						//
dyAddLogHook,						//
dyAddTypeHook,						//
dygettype,						//
dytypep,						//
dybool,						//
dyint,						//
dylong,						//
dylonglong,						//
dyflonum,						//
dyfloat,						//
dydouble,						//
dycomplex2,						//
dystring,						//
dysymbol,						//
dykeyword,						//
dysym,						//
dyksym,						//
dywstring,						//
dywstring2,						//
dyustring,						//
dyusym,						//
dyboolp,						//
dyfixintp,						//
dyintp,						//
dylongp,						//
dylonglongp,						//
dyflonump,						//
dyfloatp,						//
dydoublep,						//
dyrealp,						//
dyfcomplexp,						//
dydcomplexp,						//
dycomplexp,						//
dystringp,						//
dysymbolp,						//
dykeywordp,						//
dywstringp,						//
dyustringp,						//
dyusymp,						//
dystringv,						//
dysymbolv,						//
dywstringv,						//
dyintv,						//
dylongv,						//
dylonglongv,						//
dyfloatv,						//
dydoublev,						//
dyrealv,						//
dycreal,						//
dycimag,						//
dyvec2,						//
dyvec3,						//
dyvec4,						//
dyvec2fv,						//
dyvec3fv,						//
dyvec4fv,						//
dyvec2dv,						//
dyvec3dv,						//
dyvec4dv,						//
dyvec2p,						//
dyvec3p,						//
dyvec4p,						//
dyvecv2fv,						//
dyvecv3fv,						//
dyvecv4fv,						//
dyvecv2dv,						//
dyvecv3dv,						//
dyvecv4dv,						//
dyhash,						//
dyhashp,						//
dyhashget,						//
dyhashset,						//

};


BGBDY_API void *dycJniGetJvmtiEnv();	//?...

BGBDY_API void *dyllGetIface()
{
	static DYLL_Iface *iface;
	BGBGC_InitDyLL();
	iface=&dyll_iface;
	return((void *)iface);
}

BGBDY_API void *dyllGetObject(char *name)
{
	if(!strcmp(name, "DYLL"))
		return(dyllGetIface());
	if(!strcmp(name, "VFILE"))
		return(vfGetEnv());

	if(!strcmp(name, "DYO"))
		return((void *)(&dyo_iface));
	if(!strcmp(name, "DY"))
		return((void *)(&dy_iface));

	if(!strcmp(name, "DYC_JNI"))
		return(dycJniGetEnv());
	if(!strcmp(name, "DYC_JavaVM"))
		return(dycJniGetJavaVM());
	if(!strcmp(name, "DYC_JVMTI"))
		return(dycJniGetJvmtiEnv());

	return(dyllGetAddr(name));
}

BGBDY_API void dyllSetObject(char *name, void *value)
{
	dyllSetAddr(name, value);
}
