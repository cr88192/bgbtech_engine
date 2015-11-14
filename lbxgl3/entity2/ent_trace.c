#include <lbxgl2.h>

static Entity *ent_trace_ignore=NULL;

int EntGetBBox(Entity *ent, float *mins, float *maxs)
{
	float org[4];
	int i;

	i=EntGet3FV(ent, "origin", org);
	if(i<0) { V3F_ZERO(mins); V3F_ZERO(maxs); return(-1); }
	i=EntGet3FV(ent, "mins", mins);
	if(i<0) { V3F_ZERO(mins); V3F_ZERO(maxs); return(-1); }
	i=EntGet3FV(ent, "maxs", maxs);
	if(i<0) { V3F_ZERO(mins); V3F_ZERO(maxs); return(-1); }

	V3F_ADD(mins, org, mins);
	V3F_ADD(maxs, org, maxs);

	return(0);
}

int EntGetPos(Entity *ent, float *pos)
{
	float org[3], rot[9];

	EntGet3FV(ent, "origin", org);
	EntGet9FV(ent, "rot", rot);
	pos[0]=rot[0];	pos[1]=rot[1];	pos[2]=rot[2];	pos[3]=0;
	pos[4]=rot[3];	pos[5]=rot[4];	pos[6]=rot[5];	pos[7]=0;
	pos[8]=rot[6];	pos[9]=rot[7];	pos[10]=rot[8];	pos[11]=0;
	pos[12]=org[0];	pos[13]=org[1];	pos[14]=org[2];	pos[15]=1;

	return(0);
}

int EntCheckLineSolidEntity(Entity *ent, float *sp, float *ep)
{
	float mins[4], maxs[4], org[4], pos[16];
	float r, d;
	char *s, *t;
	int i;

	s=EntGetString(ent, "solidtype");
	if(!s)return(0);

	if(!strcmp(s, "solid_prim"))
	{
		t=EntGetString(ent, "classname");
		if(t && !strcmp(t, "worldspawn"))return(0);

		EntGetPos(ent, pos);
		i=BSP_TraceNodePos(ent->bmdl->root, pos, sp, ep);
		return(i);
	}

	if(!strcmp(s, "solid_sphere"))
	{
		i=EntGet3FV(ent, "origin", org);
		if(i<0)return(0);
		r=EntGetFloat(ent, "radius");
		if(r<0)return(0);

		d=Vec3F_DistLineSeg(org, sp, ep);
		if(d<=r)return(1);
		return(0);
	}

	i=EntGetBBox(ent, mins, maxs);
	if(i<0)return(0);

	if(!strcmp(s, "solid_obb"))
	{
		EntGetPos(ent, pos);
		i=SolidOBB_BoxLineCollideP(mins, maxs, pos, sp, ep);
		return(i);
	}

	i=SolidAABB_BoxLineCollideP(mins, maxs, sp, ep);
	return(i);
}


int EntCheckLineBSP(World *wrl, float *sp, float *ep)
{
	int i;
	i=BSP_TraceNode(wrl->bsp, sp, ep);
	return(i);
}

int EntTraceLineBSP(World *wrl, float *sp, float *ep, float *io)
{
	BSP_Face *tmp;
	tmp=BSP_TraceNode2(wrl->bsp, sp, ep, io);
	return(tmp?1:0);
}

int EntCheckLine(World *wrl, float *sp, float *ep)
{
	Entity *cur;
	int i;
	i=BSP_TraceNode(wrl->bsp, sp, ep);
	if(i)return(i);

	cur=wrl->ents;
	while(cur)
	{
		if(cur==ent_trace_ignore)
			{ cur=cur->next; continue; }
		i=EntCheckLineSolidEntity(cur, sp, ep);
		if(i)return(1);
		cur=cur->next;
	}

	return(i);
}

Entity *EntQueryLine(World *wrl, float *sp, float *iep)
{
	float ep[4];
	BSP_Face *tmp;
	Entity *cur, *lst;
	int i;

	tmp=BSP_TraceNode2(wrl->bsp, sp, iep, ep);
	if(!tmp) { V3F_COPY(iep, ep); }

	cur=wrl->ents; lst=NULL;
	while(cur)
	{
		if(cur==ent_trace_ignore)
			{ cur=cur->next; continue; }
		i=EntCheckLineSolidEntity(cur, sp, ep);
		if(i) { cur->chain=lst; lst=cur; }
		cur=cur->next;
	}

	return(lst);
}

Entity *EntQueryLineFirst(World *wrl, float *sp, float *ep)
{
	float dir[4], io[4];
	Entity *cur, *lst, *best;
	float d, bd;

	lst=EntQueryLine(wrl, sp, ep);

	V3F_SUB(ep, sp, dir);
	V3F_NORMALIZE(dir, dir);
	dir[3]=V3F_DOT(sp, dir);

	cur=lst; best=NULL; bd=999999;
	while(cur)
	{
		EntGet3FV(cur, "origin", io);
		d=V3F_NDOT(io, dir);
		if(d<bd) { best=cur; bd=d; }
		cur=cur->chain;
	}

	return(best);
}


Entity *EntQueryLineSort(World *wrl, float *sp, float *ep)
{
	float dir[4], io[4];
	Entity *cur, *lst;
	Entity **arr;
	float *darr;
	float d;
	int i, j, n;

	lst=EntQueryLine(wrl, sp, ep);
	arr=EntFlattenChainN(lst, &n);
	darr=gcatomic(n*sizeof(float));

	V3F_SUB(ep, sp, dir);
	V3F_NORMALIZE(dir, dir);
	dir[3]=V3F_DOT(sp, dir);

	for(i=0; i<n; i++)
	{
		EntGet3FV(arr[i], "origin", io);
		darr[i]=V3F_NDOT(io, dir);
	}

	//sort array, lazy, selection sort...
	for(i=0; i<n; i++)
		for(j=i+1; j<n; j++)
			if(darr[j]<darr[i])
	{
		cur=arr[i]; d=darr[i];
		arr[i]=arr[j]; darr[i]=darr[j];
		arr[j]=cur; darr[j]=d;
	}

	lst=NULL;
	for(i=n-1; i>=0; i--)
		{ arr[i]->chain=lst; lst=arr[i]; }

	gcfree(arr);
	gcfree(darr);

	return(lst);
}


int EntCheckEntityVisibleOrg(Entity *ent, float *vorg)
{
	float eorg[4], mins[4], maxs[4];
	float v[3];
	Entity *tent;
	int i;

	i=EntGet3FV(ent, "origin", eorg);
	if(i<0)return(0);

	//can see entity origin?...
	tent=EntQueryLineFirst(ent->world, vorg, eorg);
	if(!tent || (tent==ent))return(1);

	//can see one of the corners?
	EntGetBBox(ent, mins, maxs);
	for(i=0; i<8; i++)
	{
		v[0]=(i&1)?maxs[0]:mins[0];
		v[1]=(i&2)?maxs[1]:mins[1];
		v[2]=(i&4)?maxs[2]:mins[2];

		tent=EntQueryLineFirst(ent->world, vorg, v);
		if(!tent || (tent==ent))return(1);
	}

	return(0);
}

int EntCheckVisible(Entity *ent, Entity *other)
{
	float org[4], v[4];
	float d;
	int i, j;

	i=EntGet3FV(ent, "origin", org);
	if(i<0)return(0);

	d=EntGetFloat(ent, "viewheight");
	if(d>0)org[2]+=d;

	ent_trace_ignore=ent;

	//directly visible
	i=EntCheckEntityVisibleOrg(other, org);
	if(i)return(1);

	//try PVS
	for(i=0; i<8; i++)
	{
		v[0]=org[0]+((i&1)?12:-12);
		v[1]=org[1]+((i&2)?12:-12);
		v[2]=org[2]+((i&4)?12:-12);

		j=EntCheckEntityVisibleOrg(other, v);
		if(j)return(1);
	}

	ent_trace_ignore=NULL;
	return(0);
}

