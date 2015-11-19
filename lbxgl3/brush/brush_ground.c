#include <lbxgl2.h>

void LBXGL_BrushGround_BuildFaceEVec(LBXGL_BrushTempFace *face)
{
	float pt[3], dv[3], dn[4];
	float f, g;
	int i, j;

	if(face->evec)
		return;

	face->evec=gcatomic(4*face->n_vec*sizeof(float));

	for(i=0; i<face->n_vec; i++)
	{
		j=(i+1)%face->n_vec;
		V3F_SUB(face->xyz+j*3, face->xyz+i*3, dv);
		V3F_CROSS(face->norm, dv, dn);
		V3F_NORMALIZE(dn, dn);
		dn[3]=V3F_DOT(face->xyz+i*3, dn);

		V4F_COPY(dn, face->evec+i*4);
	}
}

int LBXGL_BrushGround_TraceFace(LBXGL_BrushTempFace *face,
	float *sp, float *ep, float *io)
{
	float pt[3], dv[3], dn[4];
	float f, g;
	int i, j;

	if(!face->evec)
		LBXGL_BrushGround_BuildFaceEVec(face);

	f=V3D_NDOT(sp, face->norm);
	g=V3D_NDOT(ep, face->norm);

	if((f<=0) && (g<=0))return(0);
	if((f>=0) && (g>=0))return(0);

	Hull_LinePlaneIntersect(sp, ep, face->norm, pt);
	for(i=0; i<face->n_vec; i++)
	{
		f=V3F_NDOT(pt, face->evec+i*4);
		if(f>=0)return(0);
	}

	if(io) { V3F_COPY(pt, io); }
	return(1);
}

//find panel pt is "standing on"
LBXGL_GroundPanel *LBXGL_BrushGround_GetPanelForPoint(
	LBXGL_BrushWorld *world, float *pt, float *io)
{
	float tv[3];
	LBXGL_GroundPanel *cur, *plst;
	int i;

	plst=LBXGL_BrushGround_GetGroundFaceList(world);

	tv[0]=pt[0];
	tv[1]=pt[1];
	tv[2]=pt[2]-64;

	cur=plst;		//same type, effectively
	while(cur)
	{
		i=LBXGL_BrushGround_TraceFace(cur, pt, tv, io);
		if(i>0)return(cur);
		cur=cur->next;
	}

	return(NULL);
}

//try to find exact panel point is standing on
//otherwise, try to find one nearby
LBXGL_GroundPanel *LBXGL_BrushGround_FindGoodPanelForPoint(
	LBXGL_BrushWorld *world, float *pt)
{
	float tv[3];
	LBXGL_GroundPanel *cur, *best, *plst;
	float f, g, bw;

	cur=LBXGL_BrushGround_GetPanelForPoint(world, pt, NULL);
	if(cur)return(cur);

	plst=LBXGL_BrushGround_GetGroundFaceList(world);

	best=NULL; bw=999999;
	cur=plst;		//same type, effectively
	while(cur)
	{
		LBXGL_BrushGround_GetPanelOrigin(cur, tv);
		f=V3F_DIST(tv, pt);

		if(f<bw)
		{
			best=cur;
			bw=f;
		}

		cur=cur->next;
	}

	return(best);
}

void LBXGL_BrushGround_GetPanelOrigin(LBXGL_GroundPanel *face, float *org)
{
	float f;
	int i;

	org[0]=0; org[1]=0; org[2]=0;
	for(i=0; i<face->n_vec; i++)
	{
		org[0]+=face->xyz[i*3+0];
		org[1]+=face->xyz[i*3+1];
		org[2]+=face->xyz[i*3+2];
	}

	f=1.0/face->n_vec;
	org[0]*=f; org[1]*=f; org[2]*=f;
}


int LBXGL_BrushGround_CheckVertexOnFace(LBXGL_BrushTempFace *face, float *vec)
{
	float f, g;
	int i;

	f=V3F_NDOT(vec, face->norm);
	if(fabs(f)>0.1)return(0);	//fairly loose epsilon

	if(!face->evec)
		LBXGL_BrushGround_BuildFaceEVec(face);

	for(i=0; i<face->n_vec; i++)
	{
		f=V3F_NDOT(vec, face->evec+i*4);
		if(f>0.1)return(0);
	}
	return(1);
}

int LBXGL_BrushGround_FindAdjacentFaces(
	LBXGL_BrushTempFace *face, LBXGL_BrushTempFace *flst)
{
	LBXGL_BrushTempFace *cur;
	float *v0, *v1;
	int i, j, k;

	if(face->elink)
		return(0);

	face->elink=gcalloc(face->n_vec*sizeof(LBXGL_BrushTempFace *));

	for(i=0; i<face->n_vec; i++)
	{
		j=(i+1)%face->n_vec;
		v0=face->xyz+i*3;
		v1=face->xyz+j*3;

		cur=flst;
		while(cur)
		{
			if(cur==face) { cur=cur->next; continue; }
			
			if(LBXGL_BrushGround_CheckVertexOnFace(cur, v0) &&
				LBXGL_BrushGround_CheckVertexOnFace(cur, v1))
					break;
			cur=cur->next;
		}
		face->elink[i]=cur;
	}

	return(0);
}

int LBXGL_BrushGround_FindAdjacentFacesList(
	LBXGL_BrushTempFace *flst)
{
	LBXGL_BrushTempFace *cur;

	cur=flst;
	while(cur)
	{
		LBXGL_BrushGround_FindAdjacentFaces(cur, flst);
		cur=cur->next;
	}
	return(0);
}

int LBXGL_BrushGround_TracelineCheckBlocked(LBXGL_BrushWorld *world,
	float *sv, float *ev)
{
	LBXGL_Brush *bcur;

//	bcur=LBXGL_BrushWorld_TraceLineBrush(world, sv, ev, NULL);
//	if(bcur)return(1);

	if(fabs(ev[2]-sv[2])>24)
		return(1);
	if(V3F_DIST(sv, ev)>64)
		return(1);

	bcur=LBXGL_BrushBSP_TracelineFirst(world->bsp, sv, ev);
	if(bcur)return(1);

	return(0);
}

int LBXGL_BrushGround_CheckVertexSlideFace(LBXGL_BrushWorld *world,
	LBXGL_BrushTempFace *face, float *vec)
{
	float tv0[3], tv1[3];

	float f, g;
	int i, j;

//	if(!face->evec)
//		LBXGL_BrushGround_BuildFaceEVec(face);

	for(i=0; i<face->n_vec; i++)
	{
		tv0[0]=face->xyz[i*3+0];
		tv0[1]=face->xyz[i*3+1];
		tv0[2]=face->xyz[i*3+2]+16;

		j=LBXGL_BrushGround_TracelineCheckBlocked(world, vec, tv0);
		if(!j)return(1);
	}

	return(0);
}

int LBXGL_BrushGround_FindLinkedFaces(LBXGL_BrushWorld *world,
	LBXGL_BrushTempFace *face, LBXGL_BrushTempFace *flst)
{
	LBXGL_BrushTempFace *talink[1024];
	float tv0[3], tv1[3];

	LBXGL_BrushTempFace *cur;
	float *v0, *v1;
	int i, j, k, n;

	if(face->link)
		return(0);

	cur=flst;
	while(cur)
	{
		cur->chain=NULL;
		cur=cur->next;
	}

//	face->elink=gcalloc(face->n_vec*sizeof(LBXGL_BrushTempFace *));

	n=0;
	for(i=0; i<face->n_vec; i++)
	{
		tv0[0]=face->xyz[i*3+0];
		tv0[1]=face->xyz[i*3+1];
		tv0[2]=face->xyz[i*3+2]+16;

		cur=flst;
		while(cur)
		{
			if(cur==face) { cur=cur->next; continue; }
			if(cur->chain==face) { cur=cur->next; continue; }
			
			j=LBXGL_BrushGround_CheckVertexSlideFace(
				world, cur, tv0);

			if(j)talink[n++]=cur;
			if(n>=1000)break;

			cur->chain=face;
			cur=cur->next;
		}
	}

	if(n>0)
	{
		face->link=gcalloc(n*sizeof(LBXGL_BrushTempFace *));
		face->n_link=n;

		for(i=0; i<n; i++)
			face->link[i]=talink[i];
	}

	return(0);
}

int LBXGL_BrushGround_FindLinkedFacesList(
	LBXGL_BrushWorld *world, LBXGL_BrushTempFace *flst)
{
	LBXGL_BrushTempFace *cur;

	cur=flst;
	while(cur)
	{
		LBXGL_BrushGround_FindLinkedFaces(world, cur, flst);
		cur=cur->next;
	}
	return(0);
}


LBXGL_Brush *LBXGL_BrushGround_GetWorldBrushList(
	LBXGL_BrushWorld *world)
{
	LBXGL_SEntity *ent;
	LBXGL_Brush *cur, *lst;

	ent=LBXGL_BrushWorld_FindSEnt(world, NULL,
		"classname", "worldspawn");
	if(ent)
	{
		cur=ent->brush; lst=NULL;
		while(cur)
		{
			if(!cur->xyz)LBXGL_Brush_BuildFaces(cur);
			cur->chain=lst; lst=cur; cur=cur->enext;
		}
	}

	return(lst);
}

LBXGL_BrushTempFace *LBXGL_BrushGround_BuildGroundFaceList(
	LBXGL_BrushWorld *world)
{
	LBXGL_Brush *cur, *lst;
	LBXGL_BrushTempFace *fcur, *flst, *fnxt;
	float rad[3];

#if 0
	cur=world->brush; lst=NULL;
	while(cur)
	{
		if(!cur->xyz)LBXGL_Brush_BuildFaces(cur);
		cur->chain=lst; lst=cur; cur=cur->next;
	}
#endif

	return(NULL);


	lst=LBXGL_BrushGround_GetWorldBrushList(world);

	flst=LBXGL_BrushCSG_MakeFacesBrushList(lst);

	fcur=flst; flst=NULL;
	while(fcur)
	{
		fnxt=fcur->next;
		//if a face doesn't point up, assume it can't be stood on
		if((fcur->norm[2]<0.667) ||
			(fcur->brush->flags&(LBXGL_TXFL_NONSOLID|
				LBXGL_TXFL_DETAIL|
				LBXGL_TXFL_FLUID|
				LBXGL_TXFL_EFFECT)))
		{
			LBXGL_BrushCSG_FreeFace(fcur);
			fcur=fnxt;
			continue;
		}
		fcur->next=flst; flst=fcur;
		fcur=fnxt;
	}

//	flst=LBXGL_BrushCSG_ClipFaceListBrushList(flst, lst);

	rad[0]=16; rad[1]=16; rad[2]=-64;
	flst=LBXGL_BrushCSG_ClipFaceListBrushListExpand(flst, lst, rad);

//	LBXGL_BrushGround_FindAdjacentFacesList(flst);
	LBXGL_BrushGround_FindLinkedFacesList(world, flst);

	return(flst);
}

LBXGL_BrushTempFace *LBXGL_BrushGround_GetGroundFaceList(
	LBXGL_BrushWorld *world)
{
	if(world->groundfaces)
		return(world->groundfaces);

	world->groundfaces=
		LBXGL_BrushGround_BuildGroundFaceList(world);
	return(world->groundfaces);
}

void LBXGL_BrushGround_FlushGround(
	LBXGL_BrushWorld *world)
{
	if(!world->groundfaces)
		return;

	LBXGL_BrushCSG_FreeFaceList(world->groundfaces);
	world->groundfaces=NULL;
}


void LBXGL_BrushGround_DrawGroundFaces(LBXGL_BrushWorld *world)
{
	float tv0[3], tv1[3];
	LBXGL_BrushTempFace *cur, *lst;
	float *xyz, *xyz1;
	int i, j, k;

	lst=LBXGL_BrushGround_GetGroundFaceList(world);

	pdglDisable(GL_TEXTURE_2D);


	cur=lst; i=0;
	while(cur)
	{
//		pdglColor4f((i&1)?1:0.5, (i&2)?1:0.5, (i&4)?1:0.5, 0.25);

		pdglColor4f(0, 1, 0, 0.25);
		pdglBegin(PDGL_POLYGON);
		for(j=0; j<cur->n_vec; j++)
		{
			xyz=cur->xyz+j*3;
			pdglVertex3f(xyz[0], xyz[1], xyz[2]+1);
		}
		pdglEnd();

#if 1
		pdglColor4f(1, 0, 0, 0.25);
		pdglBegin(PDGL_LINES);
		for(j=0; j<cur->n_link; j++)
		{
			pdglColor4f(1, 0, 0, 0.25);

			LBXGL_BrushCSG_GetFaceOrigin(cur, tv0);
			LBXGL_BrushCSG_GetFaceOrigin(cur->link[j], tv1);

			pdglVertex3f(tv0[0], tv0[1], tv0[2]+16);
			pdglVertex3f(tv1[0], tv1[1], tv1[2]+16);
		}
		pdglEnd();
#endif

#if 0
		pdglColor4f(1, 0, 0, 0.25);
		pdglBegin(PDGL_LINES);
		for(j=0; j<cur->n_vec; j++)
		{
			pdglColor4f(1, 0, 0, 0.25);
			if(cur->elink && cur->elink[j])
				pdglColor4f(1, 1, 0, 0.25);

			k=(j+1)%cur->n_vec;
			xyz=cur->xyz+j*3;
			xyz1=cur->xyz+k*3;
			pdglVertex3f(xyz[0], xyz[1], xyz[2]+1.1);
			pdglVertex3f(xyz1[0], xyz1[1], xyz1[2]+1.1);
		}
		pdglEnd();
#endif

		i++;
		cur=cur->next;
	}
}

