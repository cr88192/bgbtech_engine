#include <bgbsvm.h>
#include <btgesv.h>

#ifdef __cplusplus
extern "C" {
#endif

BTSV_API void BTSV_ScriptInit()
{
	static dyt hdl=NULL;
	bsvmCachedCallSigv(&hdl, "game_sv/main", "BTSV_ScriptInit", "()v");
}


#ifdef __cplusplus
}
#endif
