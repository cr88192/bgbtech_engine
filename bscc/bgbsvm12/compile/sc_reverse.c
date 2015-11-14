/*
 Reverse Preprocessing Step
 Process and rebuild parse trees in reverse order for reasons of building a
 forwards context (and possible rebuilding of future statements).
*/

#include <bgbsvm.h>

dyt BSVMC_ReverseStepStmt(BSVM_SVMCState *ctx, dyt l)
{
	return(l);
}

dyt BSVMC_ReverseStepBlock(BSVM_SVMCState *ctx, dyt l)
{
	dyt t, r;
	if(!dyconsp(l))return(l);
	r=BSVMC_ReverseStepBlock(ctx, dycdr(l));
	t=BSVMC_ReverseStepStmt(ctx, dycar(l));

	if((t==dycar(l)) && (r==dycdr(l)))
		return(l);

	t=dycons(t, r);
	return(t);
}
