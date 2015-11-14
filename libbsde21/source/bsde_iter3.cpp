/*
Thought:
Attempt to optimize iterator through the use of a quad tree.
Assume that, when the object counts get high enough that it should matter, the
quad tree will pay off.
*/

#include "bsde_internal.h"

//buffer for temporary nodes
BSDE_QuadNode *bsde_iter3_nodes=NULL;
int bsde_iter3_nodepos;

BSDE_BinaryNode *bsde_iter3_binodes=NULL;
int bsde_iter3_binodepos;
int bsde_iter3_maxbinodes;

BSDE_QuadNode *BSDE_Iter3_AllocQuadNode()
{
	BSDE_QuadNode *tmp;
	if(!bsde_iter3_nodes)
	{
		bsde_iter3_nodes=(BSDE_QuadNode *)
			malloc(4096*sizeof(BSDE_QuadNode));
		bsde_iter3_nodepos=0;
	}

	tmp=bsde_iter3_nodes+bsde_iter3_nodepos;
	bsde_iter3_nodepos++;
	return(tmp);
}

BSDE_BinaryNode *BSDE_Iter3_AllocBinaryNode()
{
	BSDE_BinaryNode *tmp;
	int i;

	if(!bsde_iter3_binodes)
	{
		bsde_iter3_binodes=(BSDE_BinaryNode *)
			malloc(4096*sizeof(BSDE_BinaryNode));
		bsde_iter3_binodepos=0;
		bsde_iter3_maxbinodes=4096;
	}

	if((bsde_iter3_binodepos+1)>=bsde_iter3_maxbinodes)
	{
		i=bsde_iter3_maxbinodes + (bsde_iter3_maxbinodes>>1);
		bsde_iter3_binodes=(BSDE_BinaryNode *)
			realloc(bsde_iter3_binodes,
				i*sizeof(BSDE_BinaryNode));
		bsde_iter3_maxbinodes=i;
	}

	tmp=&bsde_iter3_binodes[bsde_iter3_binodepos++];
	return(tmp);
}

//calculate the centroid for the objects
int BSDE_Iter3_CalcCentroid(BSDE_Solid *first, bsde_real *org)
{
	BSDE_Solid *cur;
	int i;

	V3_ZERO(org);
	cur=first;
	for(i=0; cur; i++)
	{
		V3_ADD(org, cur->org, org);
		cur=cur->qnext;
	}
	V3_SCALE(org, 1.0/(i?i:1), org);
	return(i);
}

//calculate the normal for the objects
int BSDE_Iter3_CalcNormal(BSDE_Solid *first, bsde_real *norm)
{
	bsde_real org[3], pt[3], dx[3], dy[3], dz[3];
	bsde_real f, g, h;
	BSDE_Solid *cur;
	int i;

	BSDE_Iter3_CalcCentroid(first, org);

#if 0
	V3_ZERO(norm);
	cur=first;
	for(i=0; cur; i++)
	{
		V3_SUB(cur->org, org, pt);
		norm[0]+=fabs(pt[0]);
		norm[1]+=fabs(pt[1]);
		norm[2]+=fabs(pt[2]);
		cur=cur->qnext;
	}

	f=V3_NORMALIZE(norm, norm);
	if(f<0.0001)
	{
		norm[0]=(char)rand();
		norm[1]=(char)rand();
		norm[2]=(char)rand();
		V3_NORMALIZE(norm, norm);
	}

	norm[3]=V3_DOT(org, norm);
#endif

#if 1
	cur=first; V3_ZERO(norm); i=0;
	V3_ZERO(dx); V3_ZERO(dy); V3_ZERO(dz);
	while(cur)
	{
		if(cur->org[0]>=org[0])
		{
			dx[0]+=cur->org[0]-org[0];
			dx[1]+=cur->org[1]-org[1];
			dx[2]+=cur->org[2]-org[2];
		}else
		{
			dx[0]-=cur->org[0]-org[0];
			dx[1]-=cur->org[1]-org[1];
			dx[2]-=cur->org[2]-org[2];
		}

		if(cur->org[1]>=org[1])
		{
			dy[0]+=cur->org[0]-org[0];
			dy[1]+=cur->org[1]-org[1];
			dy[2]+=cur->org[2]-org[2];
		}else
		{
			dy[0]-=cur->org[0]-org[0];
			dy[1]-=cur->org[1]-org[1];
			dy[2]-=cur->org[2]-org[2];
		}

		if(cur->org[1]>=org[1])
		{
			dz[0]+=cur->org[0]-org[0];
			dz[1]+=cur->org[1]-org[1];
			dz[2]+=cur->org[2]-org[2];
		}else
		{
			dz[0]-=cur->org[0]-org[0];
			dz[1]-=cur->org[1]-org[1];
			dz[2]-=cur->org[2]-org[2];
		}

		i++; cur=cur->qnext;
	}

	f=V3_NORMALIZE(dx, dx);
	g=V3_NORMALIZE(dy, dy);
	h=V3_NORMALIZE(dz, dz);

	if((f>=g) && (f>=h)){ V3_COPY(dx, norm); }
		else if((g>=f) && (g>=h)) { V3_COPY(dy, norm); }
		else { V3_COPY(dz, norm); }

	f=V3_NORMALIZE(norm, norm);
	if(f<0.0001)
	{
		norm[0]=(char)rand();
		norm[1]=(char)rand();
		norm[2]=(char)rand();
		V3_NORMALIZE(norm, norm);
	}

	norm[3]=V3_DOT(org, norm);
#endif

	return(i);
}

//collide an object with a list of objects
int BSDE_Iter3_CollideList(BSDE_World *world,
	BSDE_Solid *obj, BSDE_Solid *first, int n)
{
	BSDE_Solid *sa, *sb;
	int fla, flo, flx;
	int i, j, k, l;

	sa=obj;
	sb=first;
	while(sb)
	{
		if(sb->handle<=sa->handle)
		{
			j=	BSDE_MOVEFL_STATIC|
				BSDE_MOVEFL_SEMISTATIC|
				BSDE_MOVEFL_IDLE;
			if(!(sb->moveflags&j))
			{
				sb=sb->qnext;
				continue;
			}

//			sb=sb->qnext;
//			continue;
		}

		fla=sa->moveflags&sb->moveflags;
		flo=sa->moveflags|sb->moveflags;
		flx=sa->moveflags^sb->moveflags;
		j=(fla&(BSDE_MOVEFL_STATIC|BSDE_MOVEFL_SEMISTATIC|
			BSDE_MOVEFL_NONSOLID|BSDE_MOVEFL_IDLE)) &&
			!(flo&BSDE_MOVEFL_TRIGGER);
		if(j)
		{
			sb=sb->qnext;
			continue;
		}

		j=BSDE_Collide(world, sa, sb, world->contact+n);
		if(j>0)n+=j;

		if((j>0) && (sa->moveflags&BSDE_MOVEFL_TRIGGER))
			BSDE_WorldAddTriggerContact(world, sa, sb);
		if((j>0) && (sb->moveflags&BSDE_MOVEFL_TRIGGER))
			BSDE_WorldAddTriggerContact(world, sb, sa);
		sb=sb->qnext;
	}

	return(n);
}

//check collision with a list of objects
//m=0 for initial, 1 for final
int BSDE_Iter3_CheckCollideList(BSDE_World *world,
	BSDE_Solid *obj, BSDE_Solid *first, int m)
{
	BSDE_Solid *sa, *sb;
	int fla, flo, flx;
	int i, j, k, l;

	sa=obj;
	sb=first;
	while(sb)
	{
		fla=sa->moveflags&sb->moveflags;
		flo=sa->moveflags|sb->moveflags;
		flx=sa->moveflags^sb->moveflags;
		j=(fla&(BSDE_MOVEFL_STATIC|BSDE_MOVEFL_SEMISTATIC|
			BSDE_MOVEFL_NONSOLID)) &&
			!(flo&BSDE_MOVEFL_TRIGGER);
		if(j || (sa==sb))
		{
			sb=sb->qnext;
			continue;
		}

		j=BSDE_CheckCollide2(sa, sb, m);
		if(j)return(1);

		sb=sb->qnext;
	}

	return(0);
}


//quad trees

int BSDE_Iter3_SolidQuad(BSDE_Solid *solid, bsde_real *org)
{
	int i;

	i=0;
	if((solid->mmins[1]<org[1]) && (solid->mmins[0]<org[0]))i|=1;
	if((solid->mmins[1]<org[1]) && (solid->mmaxs[0]>org[0]))i|=2;
	if((solid->mmaxs[1]>org[1]) && (solid->mmins[0]<org[0]))i|=4;
	if((solid->mmaxs[1]>org[1]) && (solid->mmaxs[0]>org[0]))i|=8;

//	if((solid->mmins[0]<org[0]) && (solid->mmaxs[0]>org[0]))i|=16;
//	if((solid->mmins[1]<org[1]) && (solid->mmaxs[1]>org[1]))i|=16;

	return(i);
}

BSDE_QuadNode *BSDE_Iter3_BuildNode(BSDE_Solid *first)
{
	BSDE_QuadNode *tmp;
	BSDE_Solid *cur, *nxt, *l0, *l1, *l2, *l3, *lc;
	int i, j, k;

	tmp=BSDE_Iter3_AllocQuadNode();
	i=BSDE_Iter3_CalcCentroid(first, tmp->center);

	if(i<4)
	{
		tmp->node[0]=NULL;
		tmp->first=first;
		return(tmp);
	}

	l0=NULL; l1=NULL; l2=NULL; l3=NULL; lc=NULL;
	cur=first;
	while(cur)
	{
		nxt=cur->qnext;
		i=BSDE_Iter3_SolidQuad(cur, tmp->center);
		switch(i)
		{
		case 1:
			cur->qnext=l0;
			l0=cur;
			break;
		case 2:
			cur->qnext=l1;
			l1=cur;
			break;
		case 4:
			cur->qnext=l2;
			l2=cur;
			break;
		case 8:
			cur->qnext=l3;
			l3=cur;
			break;
		default:
			cur->qnext=lc;
			lc=cur;
			break;
		}
		cur=nxt;
	}

	if(l0 || l1 || l2 || l3)
	{
		tmp->node[0]=BSDE_Iter3_BuildNode(l0);
		tmp->node[1]=BSDE_Iter3_BuildNode(l1);
		tmp->node[2]=BSDE_Iter3_BuildNode(l2);
		tmp->node[3]=BSDE_Iter3_BuildNode(l3);
	}else
	{
		tmp->node[0]=NULL;
	}
	tmp->first=lc;

	return(tmp);
}

int BSDE_Iter3_CollideTree(BSDE_World *world, BSDE_QuadNode *node,
	BSDE_Solid *obj, int n)
{
	int i;

	if(!node->node[0])
	{
		n=BSDE_Iter3_CollideList(world, obj, node->first, n);
		return(n);
	}

	n=BSDE_Iter3_CollideList(world, obj, node->first, n);
	for(i=0; i<4; i++)
		if(BSDE_Iter3_SolidQuad(obj, node->center)&(1<<i))
			n=BSDE_Iter3_CollideTree(
				world, node->node[i], obj, n);
	return(n);
}


//BSP-tree like algo

BSDE_BinaryNode *BSDE_Iter3_BuildBiNode(BSDE_Solid *first)
{
	BSDE_BinaryNode *tmp;
	BSDE_Solid *cur, *cur2, *nxt, *ll, *lr, *lc;
	bsde_real dm, dn;
	int i, j, k;

	tmp=BSDE_Iter3_AllocBinaryNode();
	tmp->up=NULL;	//in case root
	i=BSDE_Iter3_CalcNormal(first, tmp->norm);

//	printf("BSDE_Iter3_BuildBiNode: ( %f %f %f %f )\n",
//		tmp->norm[0], tmp->norm[1], tmp->norm[2], tmp->norm[3]);

	if(i<4)
	{
		cur=first;
		while(cur)
		{
			cur->node=tmp;
			cur=cur->qnext;
		}

		tmp->node[0]=NULL;
		tmp->first=first;

		return(tmp);
	}

	ll=NULL; lr=NULL; lc=NULL;
	cur=first;
	while(cur)
	{
#if 0
		cur2=cur;
		while(cur2 && (cur2->qnext!=cur))
			cur2=cur2->qnext;
		if(cur2)cur2->qnext=NULL;
#endif

		nxt=cur->qnext;
		BSDE_SolidAABB_BoxPlaneExtents(cur->mmins, cur->mmaxs,
			tmp->norm, &dm, &dn);
		if(dn<tmp->norm[3])
		{
			cur->qnext=ll;
			ll=cur;
		}else if(dm>tmp->norm[3])
		{
			cur->qnext=lr;
			lr=cur;
		}else
		{
			cur->node=tmp;
			cur->qnext=lc;
			lc=cur;
		}
		cur=nxt;
	}

	if(ll && !lr && !lc)
	{
		cur=ll;
		while(cur)
		{
			cur->node=tmp;
			cur=cur->qnext;
		}

		tmp->node[0]=NULL;
		tmp->node[1]=NULL;
		tmp->first=ll;
		return(tmp);
	}

	if(lr && !ll && !lc)
	{
		cur=lr;
		while(cur)
		{
			cur->node=tmp;
			cur=cur->qnext;
		}

		tmp->node[0]=NULL;
		tmp->node[1]=NULL;
		tmp->first=lr;
		return(tmp);
	}

	if(ll || lr)
	{
		tmp->node[0]=BSDE_Iter3_BuildBiNode(ll);
		tmp->node[1]=BSDE_Iter3_BuildBiNode(lr);
		tmp->node[0]->up=tmp;
		tmp->node[1]->up=tmp;

		tmp->first=lc;
	}else
	{
		tmp->node[0]=NULL;
		tmp->node[1]=NULL;
		tmp->first=lc;
	}

	return(tmp);
}

int BSDE_Iter3_CollideBiTree(BSDE_World *world, BSDE_BinaryNode *node,
	BSDE_Solid *obj, int n)
{
	bsde_real dm, dn;
	int i;

	if(!node->node[0])
	{
		n=BSDE_Iter3_CollideList(world, obj, node->first, n);
		return(n);
	}

	n=BSDE_Iter3_CollideList(world, obj, node->first, n);

	BSDE_SolidAABB_BoxPlaneExtents(obj->mmins, obj->mmaxs,
		node->norm, &dm, &dn);
	if(dn<node->norm[3])
	{
		n=BSDE_Iter3_CollideBiTree(world, node->node[0], obj, n);
	}else if(dm>node->norm[3])
	{
		n=BSDE_Iter3_CollideBiTree(world, node->node[1], obj, n);
	}else
	{
		n=BSDE_Iter3_CollideBiTree(world, node->node[0], obj, n);
		n=BSDE_Iter3_CollideBiTree(world, node->node[1], obj, n);
	}
	return(n);
}

int BSDE_Iter3_CollideBiObj(BSDE_World *world, BSDE_Solid *obj, int n)
{
	BSDE_BinaryNode *cur;
	n=BSDE_Iter3_CollideBiTree(world, obj->node, obj, n);

	cur=obj->node->up;
	while(cur)
	{
		n=BSDE_Iter3_CollideList(world, obj, cur->first, n);
		cur=cur->up;
	}
	return(n);
}

int BSDE_Iter3_CheckCollideBiTree(BSDE_World *world,
	BSDE_BinaryNode *node, BSDE_Solid *obj, int m)
{
	bsde_real dm, dn;
	int i;

	if(!node->node[0])
	{
		i=BSDE_Iter3_CheckCollideList(world, obj, node->first, m);
		return(i);
	}

	i=BSDE_Iter3_CheckCollideList(world, obj, node->first, m);
	if(i)return(i);

	BSDE_SolidAABB_BoxPlaneExtents(obj->mmins, obj->mmaxs,
		node->norm, &dm, &dn);
	if(dn<node->norm[3])
	{
		i=BSDE_Iter3_CheckCollideBiTree(world, node->node[0], obj, m);
		return(i);
	}else if(dm>node->norm[3])
	{
		i=BSDE_Iter3_CheckCollideBiTree(world, node->node[1], obj, m);
		return(i);
	}else
	{
		i=BSDE_Iter3_CheckCollideBiTree(world, node->node[0], obj, m);
		if(i)return(i);

		i=BSDE_Iter3_CheckCollideBiTree(world, node->node[1], obj, m);
		return(i);
	}
	return(0);
}

int BSDE_Iter3_CheckCollideBiObj(BSDE_World *world, BSDE_Solid *obj, int m)
{
	BSDE_BinaryNode *cur;
	int i;

	i=BSDE_Iter3_CheckCollideBiTree(world, obj->node, obj, m);
	if(i)return(i);

	cur=obj->node->up;
	while(cur)
	{
		i=BSDE_Iter3_CheckCollideList(world, obj, cur->first, m);
		if(i)return(i);
		cur=cur->up;
	}
	return(0);
}

int BSDE_WorldSingleStep2(BSDE_World *world, bsde_real dt)
{
	BSDE_Solid *sa, *sb;
	int fla, flo, flx;
	int i, j, k, l, n;

	sb=NULL;
	for(i=0; i<world->n_solids; i++)
	{
		sa=world->solids[i];
		if(!sa)continue;
		if(sa->moveflags&BSDE_MOVEFL_HIDDEN)continue;

		BSDE_Predict(world, sa, dt);

		sa->qnext=sb;
		sb=sa;
	}

	bsde_iter3_nodepos=0;
	bsde_iter3_binodepos=0;
//	world->root=BSDE_Iter3_BuildNode(sb);
	world->root=BSDE_Iter3_BuildBiNode(sb);

//	printf("\n");

#if 1
	j=world->passes;
	if(!j)j=8;
	while(j--)
	{
		n=0;
		for(i=0; i<world->n_solids; i++)
		{
			if(!world->solids[i])continue;
			sa=world->solids[i];
			if(sa->moveflags&BSDE_MOVEFL_HIDDEN)
				continue;

			if(sa->moveflags&BSDE_MOVEFL_STATIC)continue;
			if(sa->moveflags&BSDE_MOVEFL_SEMISTATIC)continue;
			if(sa->moveflags&BSDE_MOVEFL_IDLE)continue;

			l=BSDE_Iter3_CollideBiObj(world, sa, 0);
			n+=l;
			for(k=0; k<l; k++)
				BSDE_HandleContact(world,
					world->contact[k], dt);
			BSDE_FreeContacts();
		}
		if(!n)break;

		for(i=0; i<world->n_solids; i++)
		{
			sa=world->solids[i];
			if(!sa)continue;
			if(sa->moveflags&BSDE_MOVEFL_HIDDEN)continue;
			if(sa->moveflags&BSDE_MOVEFL_STATIC)continue;

			BSDE_UpdateVel(world, sa, dt);
			BSDE_Predict(world, sa, dt);
		}
	}
#endif

	j=world->passes;
	if(!j)j=1;
	while(j--)
	{
		for(i=0; i<world->n_joints; i++)
		{
			if(!world->joints[i])continue;
			BSDE_HandleJoint(world, world->joints[i], dt);

			sa=world->joints[i]->obja;
			sb=world->joints[i]->objb;

			BSDE_UpdateVel(world, sa, dt);
			BSDE_Predict(world, sa, dt);

			BSDE_UpdateVel(world, sb, dt);
			BSDE_Predict(world, sb, dt);
		}
	}

#if 0
	if(world->flags&BSDE_WORLDFL_STRICT)
		for(i=0; i<world->n_solids; i++)
	{
		if(!world->solids[i])continue;
		sa=world->solids[i];
		if(sa->moveflags&BSDE_MOVEFL_HIDDEN)
			continue;

		j=BSDE_Iter3_CheckCollideBiObj(world, sa, 1);
		if(j)
		{
			j=BSDE_Iter3_CheckCollideBiObj(world, sa, 0);
			if(j)
			{
//				if(sa->moveflags&BSDE_MOVEFL_BLOCKED)
//					printf("old initial collide\n");
//				printf("initial collide\n");

				j=16;
				while(j--)
				{
//					sa->org[0]+=BSDE_RandG()*0.1;
//					sa->org[1]+=BSDE_RandG()*0.1;
//					sa->org[2]+=BSDE_RandG()*0.1;
					sa->org[2]+=0.1;

					k=BSDE_Iter3_CheckCollideBiObj(
						world, sa, 0);
					if(!k)break;
				}

				//can't unstick, hide
				if(!j)
				{
					printf("can't unstick\n");
					sa->moveflags|=BSDE_MOVEFL_HIDDEN;
				}

				continue;
			}

			sa->moveflags|=BSDE_MOVEFL_BLOCKED;

			V4_COPY(sa->org, sa->e_org);
			V4_COPY(sa->rot, sa->e_rot);

			V3_ZERO(sa->vel);
			V3_ZERO(sa->tvel);
		}else sa->moveflags&=~BSDE_MOVEFL_BLOCKED;
	}
#endif

	for(i=0; i<world->n_solids; i++)
	{
		sa=world->solids[i];
		if(!sa)continue;
		if(sa->moveflags&BSDE_MOVEFL_HIDDEN)continue;

		if(sa->moveflags&BSDE_MOVEFL_BLOCKED)
			continue;

		BSDE_UpdateVel(world, sa, dt);
		BSDE_Update(world, sa, dt);
	}

	return(n);
}
