#include <bgbsvm.h>
#include <btgecl.h>

#ifdef __cplusplus
extern "C" {
#endif

BTCL_API void BTCL_ScriptInit()
{
	static dyt hdl=NULL;
	bsvmCachedCallSigv(&hdl, "game_cl/main", "BTCL_ScriptInit", "()v");
}

BTCL_API void BTCL_ScriptDrawUI()
{
	static dyt hdl=NULL;
	bsvmCachedCallSigv(&hdl, "game_cl/main", "BTCL_ScriptDrawUI", "()v");
}


#ifdef __cplusplus
}
#endif
