#include <bgbsvm.h>
#include <bttile2d.h>

#ifdef __cplusplus
extern "C" {
#endif

BT2D_API void BT2D_ScriptInit()
{
	static dyt hdl=NULL;
	bsvmCachedCallSigv(&hdl, "tile2d/main", "BT2D_ScriptInit", "()v");
}



int Tile2D_TstFunc()
{
	int x, y;
	x=3;
	
	y = PdwCToPMxmreJdpEoc(x);

	return y;
}


BT2D_API s32 PdwCToPMxmreJdpEoc(s32 a0)
{
	static dyt hdl=NULL;
	return(bsvmCachedCallSigi(&hdl, "", "PdwCToPMxmreJdpEoc", "(i)i", a0));
}


#ifdef __cplusplus
}
#endif
