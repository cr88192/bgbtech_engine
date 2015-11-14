/*
Iterator based on halving the frame.
*/

#include "bsde_internal.h"

bsde_real bsde_log2(bsde_real f)
{
	return(log(f)/log((bsde_real)2.0));
}

void BSDE_SetupStack(BSDE_World *world)
{
	int i;

	i=(int)ceil(bsde_log2(world->maxstep/world->minstep));
	i*=world->n_solids;

	if(i>world->stacksize)
	{
		if(world->stack)free(world->stack);
		world->stack=(BSDE_Solid **)malloc(i*sizeof(BSDE_Solid *));
		world->stacksize=i;
	}
}

void BSDE_PushCollisions(BSDE_World *world, int b)
{
	BSDE_Solid *sa, *sb;
	int i, j, k, l, n;

	n=world->stackpos-b;
	for(i=0; i<n; i++)
	{
		sa=world->stack[b+i];

		l=0;
		for(j=0; j<n; j++)
		{
			if(i==j)continue;
			sb=world->stack[b+j];

			if((sa->moveflags&BSDE_MOVEFL_STATIC) &&
				(sb->moveflags&BSDE_MOVEFL_STATIC))
					continue;

			k=BSDE_CheckCollide(sa, sb);
			if(k>0)l++;
		}
//		if(l)
		world->stack[world->stackpos++]=sa;
	}
}

void BSDE_PushInitial(BSDE_World *world)
{
	BSDE_Solid *sa, *sb;
	int i, j, k, l;

	world->stackpos=0;
	for(i=0; i<world->n_solids; i++)
	{
		if(!world->solids[i])continue;
		sa=world->solids[i];
		if(sa->moveflags&BSDE_MOVEFL_HIDDEN)
			continue;

		l=0;
		for(j=0; j<world->n_solids; j++)
		{
			if(i==j)continue;
			if(!world->solids[j])continue;
			sb=world->solids[j];
			if(sb->moveflags&BSDE_MOVEFL_HIDDEN)
				continue;

			if((sa->moveflags&BSDE_MOVEFL_STATIC) &&
				(sb->moveflags&BSDE_MOVEFL_STATIC))
					continue;

			k=BSDE_CheckInitialCollide(sa, sb);
			if(k>0)l++;
		}
//		if(!l)
		world->stack[world->stackpos++]=
			world->solids[i];
	}
}

void BSDE_PredictWorld(BSDE_World *world, bsde_real dt, int b)
{
//	int i, n;
	int i;

//	n=world->stackpos-b;
//	printf("predict %d ents\n", n);

//	for(i=0; i<n; i++)
//		BSDE_Predict(world->stack[b+i], dt);

	for(i=0; i<world->n_solids; i++)
	{
		if(!world->solids[i])continue;
		BSDE_Predict(world, world->solids[i], dt);
	}
}

void BSDE_UpdateWorld(BSDE_World *world, bsde_real dt, int b)
{
//	int i, n;
	int i;

//	n=world->stackpos-b;
//	printf("update %d ents\n", n);

//	for(i=0; i<n; i++)
//		BSDE_Update(world->stack[b+i], dt);

	for(i=0; i<world->n_solids; i++)
	{
		if(!world->solids[i])continue;
		BSDE_Update(world, world->solids[i], dt);
	}
}

void BSDE_ConstrainWorld(BSDE_World *world, bsde_real dt)
{
	int i;

	for(i=0; i<world->n_joints; i++)
	{
		if(!world->joints[i])continue;
		BSDE_HandleJoint(world, world->joints[i], dt);
	}
}

void BSDE_CollideWorld(BSDE_World *world, bsde_real dt, int b)
{
	BSDE_Solid *sa, *sb;
	int i, j, k, l, n;
	int fla, flo, flx;

	n=world->stackpos-b;
	for(i=0; i<n; i++)
	{
		sa=world->stack[b+i];

		l=0;
		for(j=i+1; j<n; j++)
		{
			if(i==j)continue;
			sb=world->stack[b+j];

			fla=sa->moveflags&sb->moveflags;
			flo=sa->moveflags|sb->moveflags;
			flx=sa->moveflags^sb->moveflags;
			k=(fla&(BSDE_MOVEFL_STATIC|BSDE_MOVEFL_SEMISTATIC|
				BSDE_MOVEFL_NONSOLID)) &&
				!(flo&BSDE_MOVEFL_TRIGGER);
			if(k)continue;

//			if((sa->moveflags&BSDE_MOVEFL_STATIC) &&
//				(sb->moveflags&BSDE_MOVEFL_STATIC))
//					continue;

			k=BSDE_Collide(world, world->stack[b+i],
				world->stack[b+j], world->contact+l);
//			if(k<=0)printf("apparent collider bug\n");
			if(k>0)l+=k;

			if((k>0) && (sa->moveflags&BSDE_MOVEFL_TRIGGER))
				BSDE_WorldAddTriggerContact(world, sa, sb);
			if((k>0) && (sb->moveflags&BSDE_MOVEFL_TRIGGER))
				BSDE_WorldAddTriggerContact(world, sb, sa);
		}

		for(k=0; k<l; k++)
			BSDE_HandleContact(world, world->contact[k], dt);
		BSDE_UpdateVel(world, world->stack[b+i], dt);
		BSDE_FreeContacts();
	}
}

void BSDE_BlockCollisionsWorld(BSDE_World *world, bsde_real dt, int b)
{
	BSDE_Solid *obj;
	int i, j, k, l, n;

	n=world->stackpos-b;
	for(i=0; i<n; i++)
	{
		l=0;
		for(j=0; j<n; j++)
		{
			if(i==j)continue;
			k=BSDE_CheckCollide(world->stack[b+i],
				world->stack[b+j]);
			if(k>0)l++;
		}
		if(l)
		{
//			printf("hard block\n");

#if 1
			obj=world->stack[b+i];
			V3_ZERO(obj->vel);
			V3_ZERO(obj->tvel);
			BSDE_Predict(world, obj, dt);
#endif
		}
	}
}

void BSDE_RecurseWorld(BSDE_World *world, bsde_real dt, int lb)
{
	int b, n;

	b=world->stackpos;
	BSDE_PredictWorld(world, dt, lb);
	BSDE_PushCollisions(world, lb);
	n=world->stackpos-b;

//	printf("recurse {\n");

	if(!n)	//nothing collided
	{
		BSDE_ConstrainWorld(world, dt);
		BSDE_UpdateWorld(world, dt, lb);

//		printf("} step\n");
		world->stackpos=b;
		return;
	}

	if(dt>world->minstep)
	{
		BSDE_RecurseWorld(world, dt*0.5, b);
		BSDE_RecurseWorld(world, dt*0.5, b);

//		printf("} minstep\n");
		world->stackpos=b;
		return;
	}

	BSDE_CollideWorld(world, dt, b);
	BSDE_ConstrainWorld(world, dt);

	BSDE_PredictWorld(world, dt, b);

	//stop any colliding objs
//	BSDE_BlockCollisionsWorld(world, dt, b);

	BSDE_UpdateWorld(world, dt, b);

//	printf("} impact\n");
	world->stackpos=b;
}


//an attempt at a fast and simple iterator

int BSDE_WorldSingleStep(BSDE_World *world, bsde_real dt)
{
	BSDE_Solid *sa, *sb;
	int fla, flo, flx;
	int i, j, k, l, n;

	for(i=0; i<world->n_solids; i++)
	{
		if(!world->solids[i])continue;
		BSDE_Predict(world, world->solids[i], dt);
	}

	n=0;
	for(i=0; i<world->n_solids; i++)
	{
		if(!world->solids[i])continue;
		sa=world->solids[i];
		if(sa->moveflags&BSDE_MOVEFL_HIDDEN)
			continue;

		l=0;
		for(j=i+1; j<world->n_solids; j++)
		{
			if(i==j)continue;
			if(!world->solids[j])continue;
			sb=world->solids[j];
			if(sb->moveflags&BSDE_MOVEFL_HIDDEN)
				continue;

			fla=sa->moveflags&sb->moveflags;
			flo=sa->moveflags|sb->moveflags;
			flx=sa->moveflags^sb->moveflags;
			k=(fla&(BSDE_MOVEFL_STATIC|BSDE_MOVEFL_SEMISTATIC|
				BSDE_MOVEFL_NONSOLID)) &&
				!(flo&BSDE_MOVEFL_TRIGGER);
			if(k)continue;

			k=BSDE_Collide(world, sa, sb, world->contact+l);
			if(k>0)l+=k;

			if((k>0) && (sa->moveflags&BSDE_MOVEFL_TRIGGER))
				BSDE_WorldAddTriggerContact(world, sa, sb);
			if((k>0) && (sb->moveflags&BSDE_MOVEFL_TRIGGER))
				BSDE_WorldAddTriggerContact(world, sb, sa);
		}

		n+=l;
		for(k=0; k<l; k++)
			BSDE_HandleContact(world, world->contact[k], dt);
//		BSDE_UpdateVel(world, sa, dt);
		BSDE_FreeContacts();
	}

	for(i=0; i<world->n_solids; i++)
	{
		if(!world->solids[i])continue;
		BSDE_UpdateVel(world, world->solids[i], dt);
		BSDE_Update(world, world->solids[i], dt);
	}

	return(n);
}

void BSDE_WorldMultiStep(BSDE_World *world, bsde_real dt)
{
	int i, j;

	i=1;
	while(i--)
	{
		j=BSDE_WorldSingleStep(world, dt);
		if(!j)break;
	}

	for(i=0; i<world->n_solids; i++)
	{
		if(!world->solids[i])continue;
		BSDE_Update(world, world->solids[i], dt);
	}
}
