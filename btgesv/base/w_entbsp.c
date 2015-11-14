#include <btgesv.h>

// BTGE_BrushWorld *btge_world;

btEntityNode btge_nodes_free=NULL;

void btEntityExtents(btEntity ent, vec3 dir,
	float *rm, float *rn)
{
}

void btEntityPlaneExtents(btEntity ent, vec4d norm,
	float *rm, float *rn)
{
	float mins[3], maxs[3], nv[4];
	vfvec3(mins, v3add(v3d2f(ent->origin), ent->mins));
	vfvec3(maxs, v3add(v3d2f(ent->origin), ent->maxs));
	vfvec4d(nv, norm);
	Hull_BoxPlaneExtents(mins, maxs, nv, rm, rn);
}

void btBoxPlaneExtents(vec3d mins, vec3d maxs, vec4d norm,
	float *rm, float *rn)
{
	float m[3], n[3], nv[4];
	vfvec3d(m, mins);
	vfvec3d(n, maxs);
	vfvec4d(nv, norm);
	Hull_BoxPlaneExtents(m, n, nv, rm, rn);
}

void BTGE_EntBSP_FreeNode(btEntityNode ncur)
{
	ncur->next=NULL;
	ncur->lnode=NULL;
	ncur->rnode=NULL;
	ncur->mid=NULL;

	ncur->next=btge_nodes_free;
	btge_nodes_free=ncur;	
}

btEntityNode BTGE_EntBSP_AllocNode(void)
{
	btEntityNode ntmp;

	if(btge_nodes_free)
	{
		ntmp=btge_nodes_free;
		btge_nodes_free=ntmp->next;
		ntmp->next=NULL;
		return(ntmp);
	}
	
	ntmp=gctalloc("btge_entnode_t", sizeof(BTGE_EntityNode));
	return(ntmp);
}

void BTGE_EntBSP_FlushNodes(btWorld wrl)
{
	btEntity cur;
	btEntityNode ncur, nnxt;
	int i;
	
	//unlink entities from entity-BSP
	for(i=0; i<wrl->n_ents; i++)
	{
		cur=wrl->ents[i];
		if(!cur)continue;
		cur->entNextNode=NULL;
		cur->entNode=NULL;
	}

	//free nodes
	ncur=wrl->entsNodeFirst;
	while(ncur)
	{
		nnxt=ncur->next;
		BTGE_EntBSP_FreeNode(ncur);
		ncur=nnxt;
	}

	//clear world fields
	wrl->entsNodeFirst=NULL;
	wrl->entsNodeRoot=NULL;
}

btEntity BTGE_EntBSP_GetEntityList(btWorld wrl)
{
	btEntity cur, lst;
	int i;
	
	//unlink entities from entity-BSP
	lst=NULL;
	for(i=0; i<wrl->n_ents; i++)
	{
		cur=wrl->ents[i];
		if(!cur)continue;
//		if(cur->solidtype==BT_SOLID_NOT)
//			continue;
		cur->chain=lst;
		lst=cur;
	}
	
	return(lst);
}

vec3d BTGE_EntBSP_CalcEntityListOrigin(btWorld wrl, btEntity lst)
{
	btEntity cur;
	vec3d org;
	int i, n;
	
	cur=lst; org=vec3d(0,0,0); n=0;
	while(cur)
	{
		org=v3dadd(org, btCalcCenter(cur));
		n++;
		cur=cur->chain;
	}
	if(!n)n=1;
	org=v3dscale(org, 1.0/n);
	
	return(org);
}

vec4d BTGE_EntBSP_CalcEntityListNodePlane(btWorld wrl,
	btEntity lst, vec3d org)
{
	btEntity cur;
	vec3d dir, tdir;
	vec4d norm;
	int i, n;
	
	cur=lst; dir=v3dzero();
	while(cur)
	{
		tdir=v3dsub(btCalcCenter(cur), org);
		tdir=v3dabs(tdir);
		dir=v3dadd(dir, tdir);
		cur=cur->chain;
	}
	dir=v3dnorm(dir);
	norm=v4dplane(dir, v3ddot(org, dir));
	
	return(norm);
}

void BTGE_EntBSP_BindEntityListNode(btWorld wrl,
	btEntity lst, btEntityNode node)
{
	btEntity cur;

	cur=lst;
	while(cur)
	{
		cur->entNode=node;
		cur->entNextNode=node->mid;
		node->mid=cur;
		cur=cur->chain;
	}
}

btEntityNode BTGE_EntBSP_BuildEntityNode(btWorld wrl, btEntity lst)
{
	btEntity cur, nxt, llst, rlst, mlst;
	btEntityNode tmp;
	vec3d org, dir, tdir;
	vec4d norm;
	float f, g;
	int i, n, nl, nr;

	if(!lst)
	{
		tmp=BTGE_EntBSP_AllocNode();
		tmp->next=wrl->entsNodeFirst;
		wrl->entsNodeFirst=tmp;
		return(tmp);
	}
	
	org=BTGE_EntBSP_CalcEntityListOrigin(wrl, lst);
	norm=BTGE_EntBSP_CalcEntityListNodePlane(wrl, lst, org);

	tmp=BTGE_EntBSP_AllocNode();
	tmp->next=wrl->entsNodeFirst;
	wrl->entsNodeFirst=tmp;
	tmp->plane=norm;

	cur=lst; nl=0; nr=0;
	while(cur)
	{
		btEntityPlaneExtents(cur, norm, &f, &g);
		if(g<0)nl++;
		if(f>0)nr++;
		cur=cur->chain;
	}
	
	if(!nl || !nr)
//	if(1)
	{
		BTGE_EntBSP_BindEntityListNode(wrl, lst, tmp);
//		tmp->mid=lst;
		return(tmp);
	}

	cur=lst; llst=NULL; rlst=NULL; mlst=NULL;
	while(cur)
	{
		nxt=cur->chain;
		btEntityPlaneExtents(cur, norm, &f, &g);
		
		if(g<0)
			{ cur->chain=llst; llst=cur; cur=nxt; continue; }
		if(f>0)
			{ cur->chain=rlst; rlst=cur; cur=nxt; continue; }
		
		cur->chain=mlst; mlst=cur;
		cur=nxt;
	}
	
	BTGE_EntBSP_BindEntityListNode(wrl, mlst, tmp);
//	tmp->mid=mlst;
	tmp->lnode=BTGE_EntBSP_BuildEntityNode(wrl, llst);
	tmp->rnode=BTGE_EntBSP_BuildEntityNode(wrl, rlst);

	return(tmp);
}

void BTGE_EntBSP_BuildEntityBSP(btWorld wrl)
{
	btEntity lst;
	btEntityNode tmp;

	BTGE_EntBSP_FlushNodes(wrl);
	lst=BTGE_EntBSP_GetEntityList(wrl);
	tmp=BTGE_EntBSP_BuildEntityNode(wrl, lst);
	wrl->entsNodeRoot=tmp;
}

void BTGE_EntBSP_DestroyEntityBSP(btWorld wrl)
{
	BTGE_EntBSP_FlushNodes(wrl);
	wrl->entsNodeRoot=NULL;
}

BTSV_API bool BTGE_EntBSP_CheckBoxEnt(
	vec3d mins, vec3d maxs, int flags, btEntity forent,
	btEntity cur)
{
	int i;

	if(!cur)return(false);
	if(forent)
	{
		if(cur==forent)
			return(false);
		if(cur->owner==forent)
			return(false);
		if(cur==forent->owner)
			return(false);
		if(cur->owner && cur->owner==forent->owner)
			return(false);
	}

	if(cur->solidtype==BT_SOLID_NOT)
	{
		//check ignore non-solid
		if(!(flags&BT_TRACE_NONSOLID))
			return(false);
	}
		
	if(cur->solidtype==BT_SOLID_TRIGGER)
	{
		//check ignore triggers
		if(!(flags&BT_TRACE_TRIGGER))
			return(false);
	}

	if((cur->solidtype==BT_SOLID_BBOX) ||
		(cur->solidtype==BT_SOLID_SLIDEBOX))
	{
		//check ignore monsters
		if(!(flags&BT_TRACE_MONSTERS))
			return(false);
	}

	if(cur->solidtype==BT_SOLID_BSP)
	{
		//check ignore triggers
		if(!(flags&BT_TRACE_BSPSOLID))
			return(false);
	}

	i=btEntityCheckBox(cur, mins, maxs);
	if(i)return(true);

	return(false);
}

BTSV_API btEntity BTGE_EntBSP_CheckBoxEntsMid(
	vec3d mins, vec3d maxs, int flags, btEntity forent,
	btEntity lst, btEntityNode node)
{
	btEntity cur;
	int i, j;
	
	cur=node->mid;
	while(cur)
	{
		j=BTGE_EntBSP_CheckBoxEnt(mins, maxs, flags, forent, cur);
		if(j)
		{
			cur->chain=lst;
			lst=cur;
		}
		cur=cur->entNextNode;
	}
//	printf("BT_CheckBoxEnts: OK %p\n", lst);
	return(lst);
}

BTSV_API btEntity BTGE_EntBSP_CheckBoxEntsR(
	vec3d mins, vec3d maxs, int flags, btEntity forent,
	btEntity lst, btEntityNode node)
{
	btEntity cur;
	float f, g;
	int i, j;
	
//	printf("BT_CheckBoxEnts\n");

	if(!node)return(lst);

	lst=BTGE_EntBSP_CheckBoxEntsMid(mins, maxs, flags, forent, lst, node);
	
	btBoxPlaneExtents(mins, maxs, node->plane, &f, &g);
	if(g<0)
	{
		lst=BTGE_EntBSP_CheckBoxEntsR(mins, maxs, flags,
			forent, lst, node->lnode);
		return(lst);
	}
	if(f>0)
	{
		lst=BTGE_EntBSP_CheckBoxEntsR(mins, maxs, flags,
			forent, lst, node->rnode);
		return(lst);
	}

	lst=BTGE_EntBSP_CheckBoxEntsR(mins, maxs, flags,
		forent, lst, node->lnode);
	lst=BTGE_EntBSP_CheckBoxEntsR(mins, maxs, flags,
		forent, lst, node->rnode);
	return(lst);	
}

#if 0
// BTSV_API bool BTGE_EntBSP_CheckLineEnt(
	vec3d spos, vec3d epos, int flags, btEntity forent,
	btEntity cur)
{
	int i;

	if(!cur)return(false);
	if(forent)
	{
		if(cur==forent)
			return(false);
		if(cur->owner==forent)
			return(false);
		if(cur==forent->owner)
			return(false);
		if(cur->owner && cur->owner==forent->owner)
			return(false);
	}

	if(cur->solidtype==BT_SOLID_NOT)
	{
		//check ignore non-solid
		if(!(flags&BT_TRACE_NONSOLID))
			return(false);
	}
		
	if(cur->solidtype==BT_SOLID_TRIGGER)
	{
		//check ignore triggers
		if(!(flags&BT_TRACE_TRIGGER))
			return(false);
	}

	if((cur->solidtype==BT_SOLID_BBOX) ||
		(cur->solidtype==BT_SOLID_SLIDEBOX))
	{
		//check ignore monsters
		if(!(flags&BT_TRACE_MONSTERS))
			return(false);
	}

	if(cur->solidtype==BT_SOLID_BSP)
	{
		//check ignore triggers
		if(!(flags&BT_TRACE_BSPSOLID))
			return(false);
	}

	i=btEntityCheckBox(cur, mins, maxs);
	if(i)return(true);

	return(false);
}
#endif
