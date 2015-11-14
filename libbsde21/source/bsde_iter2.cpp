/*
attempt at a continuous time iterator.
will work by trying to locate and resolve each possible collision.
*/

#include "bsde_internal.h"

void BSDE_TimeIter_PushInitial(BSDE_World *world)
{
	BSDE_Solid *sa, *sb;
	int i, j, k, l;

	world->stackpos=0;
	for(i=0; i<world->n_solids; i++)
	{
		if(!world->solids[i])continue;
		sa=world->solids[i];

		l=0;
		for(j=0; j<world->n_solids; j++)
		{
			if(i==j)continue;
			if(!world->solids[j])continue;
			sb=world->solids[j];

			if((sa->moveflags&BSDE_MOVEFL_STATIC) &&
				(sb->moveflags&BSDE_MOVEFL_STATIC))
					continue;

			k=BSDE_CheckInitialCollide(sa, sb);
			if(k>0)l++;
		}
		world->stack[world->stackpos++]=
			world->solids[i];
	}
}

void BSDE_TimeIter_FirstCollision(BSDE_World *world, bsde_real dt,
	BSDE_Solid **aobj, BSDE_Solid **bobj, bsde_real *rdt)
{
	BSDE_Solid *sa, *sb;
	bsde_real f;
	int i, j, k, l;

	*aobj=NULL;
	*bobj=NULL;
	*rdt=dt;

	for(i=0; i<world->stackpos; i++)
	{
		sa=world->stack[i];

		l=0;
		for(j=i+1; j<world->stackpos; j++)
		{
//			if(i==j)continue;
			sb=world->stack[j];

			if((sa->moveflags&BSDE_MOVEFL_STATIC) &&
				(sb->moveflags&BSDE_MOVEFL_STATIC))
					continue;

			k=BSDE_CheckCollide(sa, sb);

			if(k>0)
			{
				f=BSDE_CollideTime(sa, sb, dt,
					world->minstep);
				if((f<(*rdt)) || !(*aobj))
				{
					*rdt=f;
					*aobj=sa;
					*bobj=sb;
				}
			}
		}
	}
}

void BSDE_TimeIter_StepWorld(BSDE_World *world, bsde_real dt)
{
	BSDE_Solid *sa, *sb;
	bsde_real f;
	int i, j;

	while(dt>world->minstep)
	{
		BSDE_TimeIter_FirstCollision(world, dt, &sa, &sb, &f);
		if(!sa)
		{
			BSDE_PredictWorld(world, dt, 0);
			BSDE_UpdateWorld(world, dt, 0);
			break;
		}

		BSDE_Predict(world, sa, f);
		BSDE_Predict(world, sb, f);

		i=BSDE_Collide(world, sa, sb, world->contact);

		for(j=0; j<i; j++)
			BSDE_HandleContact(world, world->contact[j], f);

		BSDE_PredictWorld(world, f, 0);
		BSDE_UpdateWorld(world, f, 0);
		dt-=f;
	}
}
