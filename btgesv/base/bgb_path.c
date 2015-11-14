#include "g_local.h"


char *g_strings[1024];
int g_nstrings=0;

GPath_Node *gpath_nodes=NULL;
int gpath_nnodes=0;
int gpath_mnodes=0;

GPath_Link *gpath_links=NULL;
int gpath_nlinks=0;
int gpath_mlinks=0;

GPath_BiNode *gpath_bnodes=NULL;
int gpath_nbnodes=0;
int gpath_mbnodes=0;
int gpath_broot=0;

int *gpath_nodeflags;

char *G_strdup(char *str)
{
	char *s;
	int i;

	for(i=0; i<g_nstrings; i++)
		if(!strcmp(g_strings[i], str))
			return(g_strings[i]);

	i=g_nstrings++;
	s=strdup(str);
	g_strings[i]=s;

	return(s);
}

void GPath_ClearWorld()
{
	if(!gpath_nodes)
	{
		gpath_nodes=malloc(256*sizeof(GPath_Node));
		gpath_nodeflags=malloc(256*sizeof(*gpath_nodeflags));
		gpath_mnodes=256;
	}
	gpath_nnodes=0;

	if(!gpath_links)
	{
		gpath_links=malloc(256*sizeof(GPath_Link));
		gpath_mlinks=256;
	}
	gpath_nlinks=0;

	if(!gpath_bnodes)
	{
		gpath_bnodes=malloc(256*sizeof(GPath_BiNode));
		gpath_mbnodes=256;
	}
	gpath_nbnodes=0;
	gpath_broot=0;
}

GPath_Node *GPath_AllocNode()
{
	GPath_Node *tmp;
	int i;

	if((gpath_nnodes+1)>=gpath_mnodes)
	{
		i=gpath_mnodes+(gpath_mnodes>>1);
		gpath_nodes=realloc(gpath_nodes, i*sizeof(GPath_Node));
		gpath_nodeflags=realloc(gpath_nodeflags,
			i*sizeof(*gpath_nodeflags));
		gpath_mnodes=i;
	}
	
	i=gpath_nnodes++;
	tmp=gpath_nodes+i;
	memset(tmp, 0, sizeof(GPath_Node));
	return(tmp);
}

GPath_Link *GPath_AllocLink()
{
	GPath_Link *tmp;
	int i;

	if((gpath_nlinks+1)>=gpath_mlinks)
	{
		i=gpath_mlinks+(gpath_mlinks>>1);
		gpath_links=realloc(gpath_links, i*sizeof(GPath_Link));
		gpath_mlinks=i;
	}
	
	i=gpath_nlinks++;
	tmp=gpath_links+i;
	memset(tmp, 0, sizeof(GPath_Link));
	return(tmp);
}

GPath_BiNode *GPath_AllocBiNode()
{
	GPath_BiNode *tmp;
	int i;

	if((gpath_nbnodes+1)>=gpath_mbnodes)
	{
		i=gpath_mbnodes+(gpath_mbnodes>>1);
		gpath_bnodes=realloc(gpath_bnodes, i*sizeof(GPath_BiNode));
		gpath_mbnodes=i;
	}
	
	i=gpath_nbnodes++;
	tmp=gpath_bnodes+i;
	memset(tmp, 0, sizeof(GPath_BiNode));
	return(tmp);
}

//determine if a (generic) monster can stand here
qboolean GPath_CheckValidNodeFl(vec3_t org, int flags)
{
	trace_t	trace;
	vec3_t org0, org1;
	float bottom, width, height;
	int i, j, mask;

	mask=CONTENTS_SOLID|CONTENTS_WINDOW|CONTENTS_WATER|
		CONTENTS_SLIME|CONTENTS_LAVA;

	width=GPATH_WIDTH;
	height=GPATH_HEIGHT;
	
	if(flags&GPFL_SMALL)
		{ width=16; height=56; }
	if(flags&GPFL_MEDIUM)
		{ width=24; height=64; }
	if(flags&GPFL_LARGE)
		{ width=32; height=96; }
	if(flags&GPFL_HUGE)
		{ width=64; height=128; }

	i=g_pointcontents (org);
	if(i&mask) return(false);
	
	org1[0]=org[0];
	org1[1]=org[1];
	org1[2]=org[2]-GPATH_STEP2;

	mask=MASK_SOLID;
//	mask=MASK_MONSTERSOLID;

	trace = gi.trace (org, vec3_origin, vec3_origin, org1,
		NULL, mask);

	if (trace.fraction == 1.0)
		return(false);

	bottom = trace.endpos[2];	

	for	(i=0; i<4; i++)
	{
		org0[0] = org1[0] = org[0] + ((i&1) ? width : -width);
		org0[1] = org1[1] = org[1] + ((i&2) ? width : -width);
		org0[2] = bottom + height;
		org1[2] = bottom + 1;
			
		trace = gi.trace (org0, vec3_origin, vec3_origin, org1, NULL,
			mask);

		if (trace.startsolid)
			return false;
		if (trace.fraction != 1.0)
			bottom = trace.endpos[2];
	}

	if(bottom > org[2])
		return(false);

	for	(i=0; i<4; i++)
	{
		org0[0] = org1[0] = org[0] + ((i&1) ? width : -width);
		org0[1] = org1[1] = org[1] + ((i&2) ? width : -width);
		org0[2] = bottom + height;
		org1[2] = bottom + 1;
			
		trace = gi.trace (org0, vec3_origin, vec3_origin, org1, NULL,
			mask);
			
		if (trace.fraction != 1.0)
			return(false);
	}

	return(true);
}

int GPath_CheckValidNode(vec3_t org)
{
	int i;

	if(!GPath_CheckValidNodeFl(org, GPFL_SMALL))
		return(0);
	i=GPFL_SMALL;
	if(!GPath_CheckValidNodeFl(org, GPFL_MEDIUM))
		return(i);
	i|=GPFL_MEDIUM;
	if(!GPath_CheckValidNodeFl(org, GPFL_LARGE))
		return(i);
	i|=GPFL_LARGE;
	if(!GPath_CheckValidNodeFl(org, GPFL_HUGE))
		return(i);
	i|=GPFL_HUGE;

	return(i);
}

//returns true if the path from src to dst is clear of obstacles
int GPath_CheckPathClear(vec3_t src, vec3_t dst)
{
	trace_t	trace;
	vec3_t org;
	int i;

	trace = gi.trace (src, vec3_origin, vec3_origin, dst, NULL,
		MASK_SOLID);

	if (trace.fraction != 1.0)
		return(0);

	org[0]=(src[0]+dst[0])*0.5;
	org[1]=(src[1]+dst[1])*0.5;
	org[2]=(src[2]+dst[2])*0.5;

	i=GPath_CheckValidNode(org);
	return(i);
}

char *GPath_GetMapNodesName()
{
	char tb[256];
	
	sprintf(tb, "baseq2/save/%s_nodes.bin", level.mapname);
	return(G_strdup(tb));
}

char *GPath_GetMapNodesName2()
{
	char tb[256];
	
	sprintf(tb, "baseq2/save/%s_nodes.txt", level.mapname);
	return(G_strdup(tb));
}

int GPath_LoadNodes()
{
	GPath_NodeHead head;
	FILE *fd;
	char *fn;
	int i;
	
	fn=GPath_GetMapNodesName();
	fd=fopen(fn, "rb");
	if(!fd)return(-1);

	fread(&head, 1, sizeof(head), fd);

	if((head.magic0!=GPATH_MAGIC0) || (head.magic1!=GPATH_MAGIC1) ||
		(head.ver!=GPATH_VER))
	{
		fclose(fd);
		return(-1);
	}

	if(gpath_mnodes<head.nnodes)
	{
		i=gpath_mnodes;
		while(i<=head.nnodes)
			i=i+(i>>1);
		gpath_nodes=realloc(gpath_nodes, i*sizeof(GPath_Node));
		gpath_nodeflags=realloc(gpath_nodeflags,
			i*sizeof(*gpath_nodeflags));
		gpath_mnodes=i;
	}
	
	gpath_nnodes=head.nnodes;
	fread(gpath_nodes, sizeof(GPath_Node), gpath_nnodes, fd);

	if(gpath_mlinks<head.nlinks)
	{
		i=gpath_mlinks;
		while(i<=head.nlinks)
			i=i+(i>>1);
		gpath_links=realloc(gpath_links, i*sizeof(GPath_Link));
		gpath_mlinks=i;
	}
	
	gpath_nlinks=head.nlinks;
	fread(gpath_links, sizeof(GPath_Link), gpath_nlinks, fd);

	if(gpath_mbnodes<head.nbnodes)
	{
		i=gpath_mbnodes;
		while(i<=head.nbnodes)
			i=i+(i>>1);
		gpath_bnodes=realloc(gpath_bnodes, i*sizeof(GPath_BiNode));
		gpath_mbnodes=i;
	}
	
	gpath_nbnodes=head.nbnodes;
	gpath_broot=head.root;

	fread(gpath_bnodes, sizeof(GPath_BiNode), gpath_nbnodes, fd);
	
	fclose(fd);
//	return(-1);
	return(0);
}

int GPath_SaveNodes()
{
	GPath_NodeHead head;
	FILE *fd;
	char *fn;
	int i, j;
	
	fn=GPath_GetMapNodesName();
	fd=fopen(fn, "wb");
	if(!fd)return(-1);
	
	head.magic0=GPATH_MAGIC0;
	head.magic1=GPATH_MAGIC1;

	head.ver=GPATH_VER;
	head.root=gpath_broot;

	head.nnodes=gpath_nnodes;
	head.mnodes=gpath_mnodes;

	head.nlinks=gpath_nlinks;
	head.mlinks=gpath_mlinks;

	head.nbnodes=gpath_nbnodes;
	head.mbnodes=gpath_mbnodes;
	
	fwrite(&head, 1, sizeof(head), fd);
	fwrite(gpath_nodes, sizeof(GPath_Node), gpath_nnodes, fd);
	fwrite(gpath_links, sizeof(GPath_Link), gpath_nlinks, fd);
	fwrite(gpath_bnodes, sizeof(GPath_BiNode), gpath_nbnodes, fd);
	
	fclose(fd);


	fn=GPath_GetMapNodesName2();
	fd=fopen(fn, "wt");
	if(fd)
	{
		for(i=1; i<gpath_nnodes; i++)
		{
			fprintf(fd, "%d: %f %f %f  %d\n", i,
				gpath_nodes[i].origin[0],
				gpath_nodes[i].origin[1],
				gpath_nodes[i].origin[2],
				gpath_nodes[i].flags);
		}
		fclose(fd);
	}

	return(0);
}


//building binary tree

int GPath_BiNode_BuildList()
{
	int i, lst;
	
	lst=0;
	for(i=gpath_nnodes-1; i>0; i--)
		{ gpath_nodes[i].next=lst; lst=i; }
	return(lst);
}

int GPath_BiNode_CalcListOrigin(int lst, vec3_t org)
{
	int cur, n;

	VectorCopy(vec3_origin, org);

	cur=lst; n=0;
	while(cur)
	{
		VectorAdd(org, gpath_nodes[cur].origin, org);
		n++;
		cur=gpath_nodes[cur].next;
	}
	
	if(n>0) { VectorScale(org, 1.0/n, org); }
	return(n);
}

void GPath_BiNode_CalcListNorm(int lst, vec3_t org, vec3_t norm)
{
	int cur, n;

	VectorCopy(vec3_origin, norm);
	n=0;
	
	cur=lst;
	while(cur)
	{
		norm[0]=norm[0]+fabs(gpath_nodes[cur].origin[0]-org[0]);
		norm[1]=norm[1]+fabs(gpath_nodes[cur].origin[1]-org[1]);
		norm[2]=norm[2]+fabs(gpath_nodes[cur].origin[2]-org[2]);
	
		n++;
		cur=gpath_nodes[cur].next;
	}
	
	VectorNormalize(norm);
}

int GPath_BiNode_BuildNodeListR(int lst)
{
	vec3_t org, norm;
	GPath_BiNode *tmp;
	vec_t dst, f, g;
	int llst, rlst, mlst;
	int cur, nxt, n;

	n=GPath_BiNode_CalcListOrigin(lst, org);
	if(n<16)
	{
		tmp=GPath_AllocBiNode();
		tmp->first=lst;
		return(tmp-gpath_bnodes);
	}

	GPath_BiNode_CalcListNorm(lst, org, norm);
	dst=DotProduct(org, norm);

	cur=lst; llst=0; rlst=0; mlst=0;
	while(cur)
	{
		f=DotProduct(gpath_nodes[cur].origin, norm)-dst;
		if(f<0) { llst++; }
		else if(f>0) { rlst++; }
		else { mlst++; }
		cur=gpath_nodes[cur].next;
	}

	if(!llst || !rlst)
	{
		tmp=GPath_AllocBiNode();
		tmp->first=lst;
		return(tmp-gpath_bnodes);
	}

	cur=lst; llst=0; rlst=0; mlst=0;
	while(cur)
	{
		f=DotProduct(gpath_nodes[cur].origin, norm)-dst;
		nxt=gpath_nodes[cur].next;
		
		if(f<0)
			{ gpath_nodes[cur].next=llst; llst=cur; }
		else if(f>0)
			{ gpath_nodes[cur].next=rlst; rlst=cur; }
		else
			{ gpath_nodes[cur].next=mlst; mlst=cur; }
		
		cur=nxt;
	}

	tmp=GPath_AllocBiNode();
	VectorCopy(norm, tmp->norm);
	tmp->dist=dst;
	tmp->first=mlst;

	n=tmp-gpath_bnodes;
	gpath_bnodes[n].lnode=GPath_BiNode_BuildNodeListR(llst);
	gpath_bnodes[n].rnode=GPath_BiNode_BuildNodeListR(rlst);
	return(n);
}

int GPath_BiNode_BuildNodes()
{
	int lst, nidx;
	
	lst=GPath_BiNode_BuildList();
	nidx=GPath_BiNode_BuildNodeListR(lst);
	gpath_broot=nidx;
	return(nidx);
}

//building nodes and links

static int gpath_z;
static int gpath_cn;

int GPath_LinkNode(GPath_Node *node)
{
	GPath_Node *tmp;
	GPath_Link *lnk;
	int i, j, k, n, fl;

//	node->link=gpath_nlinks;

	n=node-gpath_nodes;
	for(i=n+1; i<gpath_nnodes; i++)
//	for(i=0; i<gpath_nnodes; i++)
	{
//		if(i==n)continue;
	
		tmp=gpath_nodes+i;
		if(VectorDistance(node->origin, tmp->origin)>GPATH_STEP2)
			continue;
		fl=GPath_CheckPathClear(node->origin, tmp->origin);
		fl=fl&(node->flags&tmp->flags);

		if(fl)
		{
			lnk=GPath_AllocLink();
			lnk->anode=n;
			lnk->bnode=i;
			lnk->flags|=fl;

			lnk->anext=node->link;
			lnk->bnext=tmp->link;
			
			k=lnk-gpath_links;
			node->link=k;
			tmp->link=k;
			
			node->nlink++;
			tmp->nlink++;

//			j=gpath_nlinks++;
//			gpath_links[j]=i;
		}
	}

//	node->nlink=gpath_nlinks-node->link;
	
	return(0);
}

int GPath_LinkNodes()
{
	GPath_Node *tmp;
	int i, j;

	if(gpath_cn>=gpath_nnodes)
		return(0);

//	j=gpath_cn+256;
//	j=gpath_cn+64;
	j=gpath_cn+1024;
	if(j>gpath_nnodes)j=gpath_nnodes;
	for(i=gpath_cn; i<j; i++)
	{
		tmp=gpath_nodes+i;
		GPath_LinkNode(tmp);
	}
	gpath_cn=i;

	if(gpath_cn>=gpath_nnodes)
	{
		gi.cprintf(NULL, PRINT_HIGH, "Linked path nodes, %d links\n",
			gpath_nlinks);
		GPath_SaveNodes();
	}
	return(1);
}

int GPath_DoBuildLayer()
{
	GPath_Node *tmp;
	vec3_t org;
	int i, j, k, fl;

	for(i=-4096; i<4096; i+=GPATH_STEP)
		for(j=-4096; j<4096; j+=GPATH_STEP)
	{
		org[0]=i;
		org[1]=j;
		org[2]=gpath_z;
		fl=GPath_CheckValidNode(org);
		
		if(fl)
		{
			tmp=GPath_AllocNode();
			VectorCopy(org, tmp->origin);
			tmp->flags=fl;
		}
	}
	
	return(0);
}

int GPath_DoFrame()
{
	int i;
	
	return(-1);
	
	if(gpath_z > 4096)
	{
		i=GPath_LinkNodes();
		return(i);
	}

	for(i=0; i<16; i++)
	{
		GPath_DoBuildLayer();
		gpath_z += GPATH_STEP;
		if(gpath_z > 4096)
			break;
	}

	if(gpath_z > 4096)
	{
		GPath_BiNode_BuildNodes();
		gi.cprintf(NULL, PRINT_HIGH, "Built %d path nodes\n",
			gpath_nnodes);
	}

	return(1);
}

int GPath_SpawnWorld()
{
	int i, j, k;

	GPath_ClearWorld();

	i=GPath_LoadNodes();
	if(i>=0)
	{
		gpath_z = 65536;
		gpath_cn = 65536;
		return(0);
	}

	gi.cprintf(NULL, PRINT_HIGH, "GPath_SpawnWorld: Rebuilding Path Nodes\n");

	GPath_AllocNode();
	GPath_AllocLink();
	GPath_AllocBiNode();

	gpath_z = -4096;
	gpath_cn = 0;
	
	return(0);
}


//stuff for path tracing

#if 1
int GPath_IndexNodeSeq(vec3_t org)
{
	float f, bd;
	int i, b;
	
	b=-1; bd=999999;
	for(i=1; i<gpath_nnodes; i++)
	{
//		f=fabs(gpath_nodes[i].origin[2]-org[2]);
//		if(f>GPATH_STEP2)continue;

		f=VectorDistance(gpath_nodes[i].origin, org);
		if(f<bd) { b=i; bd=f; }
	}

#if 0
	i=b; if(i<0)i=0;
	gi.cprintf(NULL, PRINT_HIGH, "GPath_IndexNodeSeq: %d: %f %f %f  %f\n",
		b,
		gpath_nodes[i].origin[0], 
		gpath_nodes[i].origin[1], 
		gpath_nodes[i].origin[2], 
		bd);
#endif

	if(bd>(2*GPATH_STEP2))
		return(-1);
	return(b);
}
#endif

#if 1
int GPath_IndexNodeR(vec3_t org, int cur)
{
	GPath_BiNode *bn;
	float f, g, bd;
	int i, b, c;
	
	bn=gpath_bnodes+cur;
	if(!bn->lnode)
	{
		c=bn->first;
		b=-1; bd=999999;
		while(c)
		{
			f=fabs(gpath_nodes[c].origin[2]-org[2]);
			if(f<=GPATH_STEP2)
			{
				f=VectorDistance(gpath_nodes[c].origin, org);
				if(f<bd) { b=c; bd=f; }
			}
			c=gpath_nodes[c].next;
		}
	
		if(bd>64)
			return(-1);
		return(b);
	}

	g=DotProduct(org, bn->norm);
	if(g<=(-64))
		return(GPath_IndexNodeR(org, bn->lnode));
	if(g>=64)
		return(GPath_IndexNodeR(org, bn->rnode));
	
	c=bn->first;
	
	b=-1; bd=999999;
	while(c)
	{
		f=fabs(gpath_nodes[c].origin[2]-org[2]);
		if(f<=GPATH_STEP2)
		{
			f=VectorDistance(gpath_nodes[c].origin, org);
			if(f<bd) { b=c; bd=f; }
		}
		c=gpath_nodes[c].next;
	}

	if(bd<=GPATH_STEP)
		return(b);
	
	i=GPath_IndexNodeR(org, bn->lnode);
	f=fabs(gpath_nodes[i].origin[2]-org[2]);
	if(f<=GPATH_STEP2)
	{
		f=VectorDistance(gpath_nodes[i].origin, org);
		if(f<bd) { b=i; bd=f; }
	}

	i=GPath_IndexNodeR(org, bn->rnode);
	f=fabs(gpath_nodes[i].origin[2]-org[2]);
	if(f<=GPATH_STEP2)
	{
		f=VectorDistance(gpath_nodes[i].origin, org);
		if(f<bd) { b=i; bd=f; }
	}
	
	if(bd>64)
		return(-1);
	return(b);
}

int GPath_IndexNode(vec3_t org)
{
//	if(!gpath_broot)
		return(GPath_IndexNodeSeq(org));
	return(GPath_IndexNodeR(org, gpath_broot));
}

#endif


int GPath_TracePathR(int cur, int dst, vec3_t org,
	int ofs, int *cpts, int *bpts,
	int max, int fl, int *rblen)
{
	int link[32];
	float dist[32];
	GPath_Node *node;
	GPath_Link *lnk;
	qboolean haspath;
	float f, g;
	int i, j, k, n;
	
	//if target reached
	if(cur==dst)
	{
		n=ofs+1;
		if(n<(*rblen))
		{
			cpts[ofs]=cur;
			for(i=0; i<n; i++)
				bpts[i]=cpts[i];
			*rblen=n;
		}
		return(0);
	}

	//path too long
	if(ofs>=max)return(-1);		//path>max
	if(ofs>=(*rblen))return(-1);	//path>best

	if(gpath_nodeflags[cur]&(GPTF_CURPATH|GPTF_BADPATH))
		return(-1);
	gpath_nodeflags[cur]|=GPTF_CURPATH|GPTF_SEEN;
	
	n=0;
	lnk=gpath_links + gpath_nodes[cur].link;
	while(lnk)
	{
		if(n>=30)break;
		if(cur==lnk->anode)
		{
			if(lnk->flags&fl)
				link[n++]=lnk->bnode;
			lnk=gpath_links + lnk->anext;
		}
		else if(cur==lnk->bnode)
		{
			if(lnk->flags&fl)
				link[n++]=lnk->anode;
			lnk=gpath_links + lnk->bnext;
		}
		else
		{
			//bad link
//			spos--;
//			return(-1);
//			gpath_nodeflags[cur]|=GPTF_BADPATH;
//			return(-1);
			break;
		}
	}

	for(i=0; i<n; i++)
	{
		dist[i]=VectorDistance(gpath_nodes[link[i]].origin, org);
	}

	g=999999;
	for(i=0; i<n; i++)
		for(j=i+1; j<n; j++)
			if(dist[j]<dist[i])
	{
		k=link[i]; link[i]=link[j]; link[j]=k;
		f=dist[i]; dist[i]=dist[j]; dist[j]=f;
	}

	cpts[ofs]=cur;
	haspath=false;
	for(i=0; i<n; i++)
	{
		j=GPath_TracePathR(
			link[i], dst, org,
			ofs+1, cpts, bpts,
			max, fl, rblen);
		if(j>=0)
		{
			haspath=true;
//			cpts[0]=link[i];
		}
	}

	if(haspath)
	{
		gpath_nodeflags[cur]&=~GPTF_CURPATH;
		return(0);
	}

	gpath_nodeflags[cur]|=GPTF_BADPATH;
	gpath_nodeflags[cur]&=~GPTF_CURPATH;
	return(-1);
}

int GPath_TracePathFlags(vec3_t src, vec3_t dst, vec_t *pts,
	int max, int flags)
{
	int tpts[4096], bpts[4096];
	int p0, p1;
	int i, j, n, m, b;

	//get start and end nodes
	p0=GPath_IndexNode(src);
	if(p0<0)return(-1);
	p1=GPath_IndexNode(dst);
	if(p1<0)return(-1);
	
	//start and end on same node
	if(p0==p1)return(0);

	//assume short-trace == partial-trace
	m=max; if(m<24)m=4096;
	
	memset(gpath_nodeflags, 0, gpath_nnodes*sizeof(*gpath_nodeflags));
	
	//big recursive trace
	n=65536;
	i=GPath_TracePathR(p0, p1, dst,
		0, tpts, bpts,
		m, flags, &n);
	if(i<0)return(i);

	j=n; if(max<j) j=max;
	for(i=0; i<j; i++)
		{ _VectorCopy(gpath_nodes[bpts[i]].origin, pts+i*3); }
	return(n);
}

int GPath_TracePath(vec3_t src, vec3_t dst, vec_t *pts, int max)
{
	int i;
	i=GPath_TracePathFlags(src, dst, pts, max, GPFL_SMALL);
	return(i);
}

int GPath_ClasifyBox(float xs, float ys, float zs)
{
	int fl;
	
	fl=GPFL_SMALL;
	if((xs>32) || (ys>32) || (zs>56))fl=GPFL_MEDIUM;
	if((xs>48) || (ys>48) || (zs>64))fl=GPFL_LARGE;
	if((xs>64) || (ys>64) || (zs>96))fl=GPFL_HUGE;
	return(fl);
}

void GPath_PointToFloor(vec3_t org, vec3_t pt)
{
	trace_t	trace;
	vec3_t org1;
	int i;

	VectorCopy(org, org1);
	org1[2]-=256;

	trace = gi.trace (org, vec3_origin, vec3_origin, org1, NULL,
		MASK_MONSTERSOLID);
	VectorCopy(trace.endpos, pt);
}

#if 0
int GPath_TracePathBBox(vec3_t src, vec3_t dst,
	vec3_t mins, vec3_t maxs,
	vec_t *pts, int max)
{
	vec3_t sorg, dorg;
	int i, fl;
	
#if 1
	sorg[0]=src[0]; sorg[1]=src[1];
	sorg[2]=src[2]+mins[2]+GPATH_STEP;

	dorg[0]=dst[0]; dorg[1]=dst[1];
	dorg[2]=dst[2]+mins[2]+GPATH_STEP;
#endif

#if 0
	GPath_PointToFloor(src, sorg);
	GPath_PointToFloor(dst, dorg);
	sorg[2]+=GPATH_STEP; dorg[2]+=GPATH_STEP;
#endif

	fl=GPath_ClasifyBox(maxs[0]-mins[0],
		maxs[1]-mins[1], maxs[2]-mins[2]);
	i=GPath_TracePathFlags(sorg, dorg, pts, max, fl);
	return(i);
}
#endif

void GPath_UpdatePathGoal(edict_t *ent, edict_t *goal)
{
	vec_t		v[18*3];
	vec3_t		sorg, dorg;
	vec3_t		v0, dv;
	float f;
	int i, j, n, fl;

	if(!goal)
		return;

	if(!strcmp(goal->classname, "path_corner"))
		return;

	sorg[0]=ent->s.origin[0];
	sorg[1]=ent->s.origin[1];
	sorg[2]=ent->s.origin[2]+ent->mins[2]+GPATH_STEP;

	dorg[0]=goal->s.origin[0];
	dorg[1]=goal->s.origin[1];
	dorg[2]=goal->s.origin[2]+ent->mins[2]+GPATH_STEP;

	fl=GPath_ClasifyBox(
		ent->maxs[0] - ent->mins[0],
		ent->maxs[1] - ent->mins[1],
		ent->maxs[2] - ent->mins[2]);
	n=GPath_TracePathFlags(sorg, dorg, v, 18, fl);

//	i=GPath_TracePathBBox(sorg, dorg,
//		ent->mins, ent->maxs, 
//		v, 8);

	if(n<=0)
	{
		ent->n_move_points=0;
		return;
	}

	//discard first point
	for(i=0; i<16; i++)
		_VectorCopy(v+i*3+3, v+i*3);
	n--;

//	gi.cprintf(NULL, PRINT_HIGH, "TracePath: %s->%s, %s\n",
//		ent->classname, goal->classname, 
//		(i>0)?"pass":"fail");


	VectorSubtract(v, ent->s.origin, dv);
	f=vectoyaw (dv);
	ent->ideal_yaw = f;

	if(n>16)n=16;
	for(i=0; i<n; i++)
		_VectorCopy(v+i*3, ent->move_points+i*3);
	ent->n_move_points=n;

	M_LaserPointsList (v, n);

	//return;
}
