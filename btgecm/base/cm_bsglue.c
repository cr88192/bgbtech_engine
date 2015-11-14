#include <bgbsvm.h>
#include <btgecm.h>

#ifdef __cplusplus
extern "C" {
#endif

BTGE_API void BTCM_ScriptInit()
{
	static dyt hdl=NULL;
	bsvmCachedCallSigv(&hdl, "game_cm/cm_main", "BTCM_ScriptInit", "()v");
}


#ifdef __cplusplus
}
#endif
