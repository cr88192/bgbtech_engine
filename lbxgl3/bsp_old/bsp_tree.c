#include <lbxgl2.h>


void BSP_Trace_LinePlaneIntersect(float *start, float *end,
	float *norm, float *point)
{
	float dir[3], x;

	//calc direction
	dir[0]=end[0]-start[0];
	dir[1]=end[1]-start[1];
	dir[2]=end[2]-start[2];

	x=V3F_NDOT(start, norm);
	x=(-x)/V3F_DOT(dir, norm);
	point[0]=start[0]+dir[0]*x;
	point[1]=start[1]+dir[1]*x;
	point[2]=start[2]+dir[2]*x;
}

int BSP_TraceFace_F(float *ev, int nv, float *pt)
{
	while(nv--)
	{
		if(V3F_DOT(pt, ev)>=ev[3])return(0);
		ev+=4;
	}
	return(1);
}

int BSP_TraceNode(BSP_Node *node, float *sp, float *ep)
{
	float pt[3];
	BSP_Face *cur;
	float *n, f, g;
	int i, j;

	if(!node->lnode)
		return(0);

	n=node->norm;
	f=V3D_NDOT(sp, n);
	g=V3D_NDOT(ep, n);
	if(f==0)
	{
		if(g<0)return(BSP_TraceNode(node->lnode, sp, ep));
		if(g>0)return(BSP_TraceNode(node->rnode, sp, ep));
		return(0);
	}else if(f<0)
	{
		if(g<=0)return(BSP_TraceNode(node->lnode, sp, ep));

		i=BSP_TraceNode(node->lnode, sp, ep);
		if(i)return(i);

		BSP_Trace_LinePlaneIntersect(sp, ep, n, pt);
		cur=node->tsurf;
		while(cur)
		{
			i=BSP_TraceFace_F(cur->evecs, cur->nvecs, pt);
			if(i)return(1);
			cur=cur->next;
		}

		i=BSP_TraceNode(node->rnode, sp, ep);
		return(i);
	}else
	{
		if(g>=0)return(BSP_TraceNode(node->rnode, sp, ep));

		i=BSP_TraceNode(node->rnode, sp, ep);
		if(i)return(i);

		BSP_Trace_LinePlaneIntersect(sp, ep, n, pt);
		cur=node->tsurf;
		while(cur)
		{
			i=BSP_TraceFace_F(cur->evecs, cur->nvecs, pt);
			if(i)return(1);
			cur=cur->next;
		}

		i=BSP_TraceNode(node->lnode, sp, ep);
		return(i);
	}
}


int BSP_TraceFace2(BSP_Face *face, float *sp, float *ep,
	float *io)
{
	float pt[3], dv[3], dn[4];
	float f, g;
	int i, j;

	if(!face->evecs)
	{
		face->evecs=kalloc(4*face->nvecs*sizeof(float));
		for(i=0; i<face->nvecs; i++)
		{
			j=(i+1)%face->nvecs;
			V3F_SUB(face->vecs+j*3, face->vecs+i*3, dv);
			V3F_CROSS(face->norm, dv, dn);
			V3F_NORMALIZE(dn, dn);
			dn[3]=V3F_DOT(face->vecs+i*3, dn);

			V4F_COPY(dn, face->evecs+i*4);
		}
	}

	f=V3D_NDOT(sp, face->norm);
	g=V3D_NDOT(ep, face->norm);

	if((f<=0) && (g<=0))return(0);
	if((f>=0) && (g>=0))return(0);

	BspUtil_LinePlaneIntersect(sp, ep, face->norm, pt);
	for(i=0; i<face->nvecs; i++)
	{
		f=V3F_NDOT(pt, face->evecs+i*4);
		if(f>=0)return(0);
	}

	V3F_COPY(pt, io);
	return(1);
}

BSP_Face *BSP_TraceFaces2(BSP_Face *face,
	float *sp, float *ep, float *io)
{
	BSP_Face *cur;
	int i;

	cur=face;
	while(cur)
	{
		i=BSP_TraceFace2(cur, sp, ep, io);
		if(i)return(cur);
		cur=cur->next;
	}
	return(NULL);
}

BSP_Face *BSP_TraceNode2(BSP_Node *node,
	float *sp, float *ep, float *io)
{
	BSP_Face *tmp;
	float f, g;
	int i, j;

	if(!node->lnode)
	{
//		tmp=BSP_TraceFaces2(node->face, sp, ep, io);
//		return(tmp);

		return(NULL);
	}

	f=V3D_NDOT(sp, node->norm);
	g=V3D_NDOT(ep, node->norm);

	if((f<=0) && (g<=0))
	{
		tmp=BSP_TraceNode2(node->lnode, sp, ep, io);
		return(tmp);
	}
	if((f>=0) && (g>=0))
	{
		tmp=BSP_TraceNode2(node->rnode, sp, ep, io);
		return(tmp);
	}

	if(f<g)
	{
		tmp=BSP_TraceNode2(node->lnode, sp, ep, io);
		if(tmp)return(tmp);

//		tmp=BSP_TraceFaces2(node->face, sp, ep, io);
		tmp=BSP_TraceFaces2(node->tsurf, sp, ep, io);
		if(tmp)return(tmp);

		tmp=BSP_TraceNode2(node->rnode, sp, ep, io);
		return(tmp);
	}else
	{
		tmp=BSP_TraceNode2(node->rnode, sp, ep, io);
		if(tmp)return(tmp);

//		tmp=BSP_TraceFaces2(node->face, sp, ep, io);
		tmp=BSP_TraceFaces2(node->tsurf, sp, ep, io);
		if(tmp)return(tmp);

		tmp=BSP_TraceNode2(node->lnode, sp, ep, io);
		return(tmp);
	}
}


float BSP_BoundFaceSphere(BSP_Face *face, float *org)
{
	float *v;
	float r, d;
	int i;

	r=0;
	for(i=0; i<face->nvecs; i++)
	{
		v=face->vecs+i*3;
		d=V3F_DIST(v, org);
		if(d>r)r=d;
	}
	return(r);
}

float BSP_BoundFacesSphere(BSP_Face *faces, float *org)
{
	BSP_Face *cur;
	float r, d;

	r=0;
	cur=faces;
	while(cur)
	{
		d=BSP_BoundFaceSphere(cur, org);
		if(d>r)r=d;
		cur=cur->next;
	}
	return(r);
}

float BSP_BoundNodeSphere(BSP_Node *node, float *org)
{
	float r, d;
	int i, j;

	if(!node->lnode)
	{
		r=BSP_BoundFacesSphere(node->tsurf, org);
		return(r);
	}


	r=BSP_BoundFacesSphere(node->tsurf, org);

	d=BSP_BoundNodeSphere(node->lnode, org);
	if(d>r)r=d;

	d=BSP_BoundNodeSphere(node->rnode, org);
	if(d>r)r=d;

	return(r);
}
