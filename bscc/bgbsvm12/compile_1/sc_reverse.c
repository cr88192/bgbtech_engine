/*
 Reverse Preprocessing Step
 Process and rebuild parse trees in reverse order for reasons of building a
 forwards context (and possible rebuilding of future statements).
*/

#include <bgbsvm.h>

dytf BSVMC_ReverseStepStmt(BSVM_SVMCState *ctx, dytf l)
{
	return(l);
}

dytf BSVMC_ReverseStepBlock(BSVM_SVMCState *ctx, dytf l)
{
	dytf t, r;
	if(!dytfConsP(l))return(l);
	r=BSVMC_ReverseStepBlock(ctx, dytfCdr(l));
	t=BSVMC_ReverseStepStmt(ctx, dytfCar(l));

	if(dytfEqP(t, dytfCar(l)) && dytfEqP(r, dytfCdr(l)))
		return(l);

	t=dytfCons(t, r);
	return(t);
}
