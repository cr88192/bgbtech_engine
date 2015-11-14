/*
stuff related to generating random numbers.
seeds will not be generated here, it is expected that things will be seeded
externally.
 */

#include "bsde_internal.h"

//static unsigned int bsde_rng2_table[64];
//static int bsde_rng2_i;
//static unsigned long long bsde_rng3_state;
//static unsigned long bsde_rng3_state;

//first generator

void BSDE_SRand(BSDE_World *wrl, bsdeUInt seed)
{
	wrl->rng3_state=seed;
}

void BSDE_SRand2(BSDE_World *wrl, bsdeUInt seed1, bsdeUInt seed2)
{
//	bsde_rng3_state=seed1;
	wrl->rng3_state=(((long long)seed2)<<32)|seed1;
}

bsdeUInt BSDE_Rand(BSDE_World *wrl)
{
//	bsde_rng3_state=bsde_rng3_state * 65521 + 1;
//	return((bsde_rng3_state>>16)&65535);

	wrl->rng3_state=wrl->rng3_state * 6364136223846793005LL + 1;
	return((wrl->rng3_state>>48)&65535);
}


//second generator
bsdeUInt BSDE_Rand2(BSDE_World *wrl)
{
	wrl->rng2_i = (wrl->rng2_i+1) & 63;
	wrl->rng2_table[wrl->rng2_i] =
		wrl->rng2_table[(wrl->rng2_i-24) & 63] ^
		wrl->rng2_table[(wrl->rng2_i-55) & 63];
	return(wrl->rng2_table[wrl->rng2_i]);
}

void BSDE_Seed2(BSDE_World *wrl, bsdeUInt s0, bsdeUInt s1)
{
	int i;
	wrl->rng2_table[0]=s0;
	wrl->rng2_table[1]=s1;

	for(i=2; i<55; i++)
		wrl->rng2_table[i]=wrl->rng2_table[i-1]*11+
			wrl->rng2_table[i-2]*19/16;					
}

void BSDE_Seed21(BSDE_World *wrl)
{
	int i;
	for(i=0; i<64; i++)
	{
//		wrl->rng3_state=wrl->rng3_state * 6364136223846793005LL + 1;
//		wrl->rng2_table[i]=wrl->rng3_state>>32;

		wrl->rng2_table[i]=BSDE_Rand(wrl)|(BSDE_Rand(wrl)<<16);
	}
}


//joint generator
bsdeUInt BSDE_RandJ(BSDE_World *wrl)
{
	bsdeUInt i;

	i=BSDE_Rand(wrl)*BSDE_Rand2(wrl);
	return(i);
}


//random value between 0 and 1
bsde_real BSDE_RandF(BSDE_World *wrl)
{
	bsde_real f;

	f=((bsde_real)BSDE_Rand2(wrl))/4294967295.0;
	return(f);
}

//random value between -1 and 1
bsde_real BSDE_RandG(BSDE_World *wrl)
{
	return(2*BSDE_RandF(wrl)-1);
}
