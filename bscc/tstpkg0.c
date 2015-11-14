#include <bgbsvm.h>

#ifdef __cplusplus
extern "C" {
#endif

void foo(s32 a0, s32 a1)
{
	static dyt hdl=NULL;
	bsvmCachedCallSigv(&hdl, "tstpkg", "foo", "(ii)v", a0, a1);
}

double bar(float a0, float a1)
{
	static dyt hdl=NULL;
	return(bsvmCachedCallSigd(&hdl, "tstpkg", "bar", "(ff)d", a0, a1));
}

#ifdef __cplusplus
}
#endif
