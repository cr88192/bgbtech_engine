#include <btgesv.h>

void BTGE_BrushGround_BuildFaceEVec(BTGE_BrushTempFace *face)
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

int BTGE_BrushGround_TraceFace(BTGE_BrushTempFace *face,
	float *sp, float *ep, float *io)
{
	float pt[3], dv[3], dn[4];
	float f, g;
	int i, j;

	if(!face->evec)
		BTGE_BrushGround_BuildFaceEVec(face);

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
BTGE_GroundPanel *BTGE_BrushGround_GetPanelForPoint(
	BTGE_BrushWorld *world, float *pt, float *io)
{
	float tv[3];
	BTGE_GroundPanel *cur, *plst;
	int i;

	plst=BTGE_BrushGround_GetGroundFaceList(world);

	tv[0]=pt[0];
	tv[1]=pt[1];
	tv[2]=pt[2]-64;

	cur=plst;		//same type, effectively
	while(cur)
	{
		i=BTGE_BrushGround_TraceFace(cur, pt, tv, io);
		if(i>0)return(cur);
		cur=cur->next;
	}

	return(NULL);
}

//try to find exact panel point is standing on
//otherwise, try to find one nearby
BTGE_GroundPanel *BTGE_BrushGround_FindGoodPanelForPoint(
	BTGE_BrushWorld *world, float *pt)
{
	float tv[3];
	BTGE_GroundPanel *cur, *best, *plst;
	float f, g, bw;

	cur=BTGE_BrushGround_GetPanelForPoint(world, pt, NULL);
	if(cur)return(cur);

	plst=BTGE_BrushGround_GetGroundFaceList(world);

	best=NULL; bw=999999;
	cur=plst;		//same type, effectively
	while(cur)
	{
		BTGE_BrushGround_GetPanelOrigin(cur, tv);
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

void BTGE_BrushGround_GetPanelOrigin(BTGE_GroundPanel *face, float *org)
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


int BTGE_BrushGround_CheckVertexOnFace(BTGE_BrushTempFace *face, float *vec)
{
	float f, g;
	int i;

	f=V3F_NDOT(vec, face->norm);
	if(fabs(f)>0.1)return(0);	//fairly loose epsilon

	if(!face->evec)
		BTGE_BrushGround_BuildFaceEVec(face);

	for(i=0; i<face->n_vec; i++)
	{
		f=V3F_NDOT(vec, face->evec+i*4);
		if(f>0.1)return(0);
	}
	return(1);
}

int BTGE_BrushGround_FindAdjacentFaces(
	BTGE_BrushTempFace *face, BTGE_BrushTempFace *flst)
{
	BTGE_BrushTempFace *cur;
	float *v0, *v1;
	int i, j, k;

	if(face->elink)
		return(0);

	face->elink=gcalloc(face->n_vec*sizeof(BTGE_BrushTempFace *));

	for(i=0; i<face->n_vec; i++)
	{
		j=(i+1)%face->n_vec;
		v0=face->xyz+i*3;
		v1=face->xyz+j*3;

		cur=flst;
		while(cur)
		{
			if(cur==face) { cur=cur->next; continue; }
			
			if(BTGE_BrushGround_CheckVertexOnFace(cur, v0) &&
				BTGE_BrushGround_CheckVertexOnFace(cur, v1))
					break;
			cur=cur->next;
		}
		face->elink[i]=cur;
	}

	return(0);
}

int BTGE_BrushGround_FindAdjacentFacesList(
	BTGE_BrushTempFace *flst)
{
	BTGE_BrushTempFace *cur;

	cur=flst;
	while(cur)
	{
		BTGE_BrushGround_FindAdjacentFaces(cur, flst);
		cur=cur->next;
	}
}

int BTGE_BrushGround_TracelineCheckBlocked(BTGE_BrushWorld *world,
	float *sv, float *ev)
{
	BTGE_Brush *bcur;

//	bcur=BTGE_BrushWorld_TraceLineBrush(world, sv, ev, NULL);
//	if(bcur)return(1);

	if(fabs(ev[2]-sv[2])>24)
		return(1);
	if(V3F_DIST(sv, ev)>64)
		return(1);

	bcur=BTGE_BrushBSP_TracelineFirst(world->bsp, sv, ev);
	if(bcur)return(1);

	return(0);
}

int BTGE_BrushGround_CheckVertexSlideFace(BTGE_BrushWorld *world,
	BTGE_BrushTempFace *face, float *vec)
{
	float tv0[3], tv1[3];

	float f, g;
	int i, j;

//	if(!face->evec)
//		BTGE_BrushGround_BuildFaceEVec(face);

	for(i=0; i<face->n_vec; i++)
	{
		tv0[0]=face->xyz[i*3+0];
		tv0[1]=face->xyz[i*3+1];
		tv0[2]=face->xyz[i*3+2]+16;

		j=BTGE_BrushGround_TracelineCheckBlocked(world, vec, tv0);
		if(!j)return(1);
	}

	return(0);
}

int BTGE_BrushGround_FindLinkedFaces(BTGE_BrushWorld *world,
	BTGE_BrushTempFace *face, BTGE_BrushTempFace *flst)
{
	BTGE_BrushTempFace *talink[1024];
	float tv0[3], tv1[3];

	BTGE_BrushTempFace *cur;
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

//	face->elink=gcalloc(face->n_vec*sizeof(BTGE_BrushTempFace *));

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
			
			j=BTGE_BrushGround_CheckVertexSlideFace(
				world, cur, tv0);

			if(j)talink[n++]=cur;
			if(n>=1000)break;

			cur->chain=face;
			cur=cur->next;
		}
	}

	if(n>0)
	{
		face->link=gcalloc(n*sizeof(BTGE_BrushTempFace *));
		face->n_link=n;

		for(i=0; i<n; i++)
			face->link[i]=talink[i];
	}

	return(0);
}

int BTGE_BrushGround_FindLinkedFacesList(
	BTGE_BrushWorld *world, BTGE_BrushTempFace *flst)
{
	BTGE_BrushTempFace *cur;

	cur=flst;
	while(cur)
	{
		BTGE_BrushGround_FindLinkedFaces(world, cur, flst);
		cur=cur->next;
	}
}


BTGE_Brush *BTGE_BrushGround_GetWorldBrushList(
	BTGE_BrushWorld *world)
{
	BTGE_SEntity *ent;
	BTGE_Brush *cur, *lst;

	ent=BTGE_BrushWorld_FindSEnt(world, NULL,
		"classname", "worldspawn");
	if(ent)
	{
		cur=ent->brush; lst=NULL;
		while(cur)
		{
			if(!cur->xyz)BTGE_Brush_BuildFaces(cur);
			cur->chain=lst; lst=cur; cur=cur->enext;
		}
	}

	return(lst);
}

BTGE_BrushTempFace *BTGE_BrushGround_BuildGroundFaceList(
	BTGE_BrushWorld *world)
{
	BTGE_Brush *cur, *lst;
	BTGE_BrushTempFace *fcur, *flst, *fnxt;
	float rad[3];

#if 0
	cur=world->brush; lst=NULL;
	while(cur)
	{
		if(!cur->xyz)BTGE_Brush_BuildFaces(cur);
		cur->chain=lst; lst=cur; cur=cur->next;
	}
#endif

	return(NULL);


	lst=BTGE_BrushGround_GetWorldBrushList(world);

	flst=BTGE_BrushCSG_MakeFacesBrushList(lst);

	fcur=flst; flst=NULL;
	while(fcur)
	{
		fnxt=fcur->next;
		//if a face doesn't point up, assume it can't be stood on
		if((fcur->norm[2]<0.667) ||
			(fcur->brush->flags&(BTGE_TXFL_NONSOLID|
				BTGE_TXFL_DETAIL|
				BTGE_TXFL_FLUID|
				BTGE_TXFL_EFFECT)))
		{
			BTGE_BrushCSG_FreeFace(fcur);
			fcur=fnxt;
			continue;
		}
		fcur->next=flst; flst=fcur;
		fcur=fnxt;
	}

//	flst=BTGE_BrushCSG_ClipFaceListBrushList(flst, lst);

	rad[0]=16; rad[1]=16; rad[2]=-64;
	flst=BTGE_BrushCSG_ClipFaceListBrushListExpand(flst, lst, rad);

//	BTGE_BrushGround_FindAdjacentFacesList(flst);
	BTGE_BrushGround_FindLinkedFacesList(world, flst);

	return(flst);
}

BTGE_BrushTempFace *BTGE_BrushGround_GetGroundFaceList(
	BTGE_BrushWorld *world)
{
	if(world->groundfaces)
		return(world->groundfaces);

	world->groundfaces=
		BTGE_BrushGround_BuildGroundFaceList(world);
	return(world->groundfaces);
}

void BTGE_BrushGround_FlushGround(
	BTGE_BrushWorld *world)
{
	if(!world->groundfaces)
		return;

	BTGE_BrushCSG_FreeFaceList(world->groundfaces);
	world->groundfaces=NULL;
}


void BTGE_BrushGround_DrawGroundFaces(BTGE_BrushWorld *world)
{
	float tv0[3], tv1[3];
	BTGE_BrushTempFace *cur, *lst;
	float *xyz, *xyz1;
	int i, j, k;

	lst=BTGE_BrushGround_GetGroundFaceList(world);

	glDisable(GL_TEXTURE_2D);


	cur=lst; i=0;
	while(cur)
	{
//		glColor4f((i&1)?1:0.5, (i&2)?1:0.5, (i&4)?1:0.5, 0.25);

		glColor4f(0, 1, 0, 0.25);
		glBegin(GL_POLYGON);
		for(j=0; j<cur->n_vec; j++)
		{
			xyz=cur->xyz+j*3;
			glVertex3f(xyz[0], xyz[1], xyz[2]+1);
		}
		glEnd();

#if 1
		glColor4f(1, 0, 0, 0.25);
		glBegin(GL_LINES);
		for(j=0; j<cur->n_link; j++)
		{
			glColor4f(1, 0, 0, 0.25);

			BTGE_BrushCSG_GetFaceOrigin(cur, tv0);
			BTGE_BrushCSG_GetFaceOrigin(cur->link[j], tv1);

			glVertex3f(tv0[0], tv0[1], tv0[2]+16);
			glVertex3f(tv1[0], tv1[1], tv1[2]+16);
		}
		glEnd();
#endif

#if 0
		glColor4f(1, 0, 0, 0.25);
		glBegin(GL_LINES);
		for(j=0; j<cur->n_vec; j++)
		{
			glColor4f(1, 0, 0, 0.25);
			if(cur->elink && cur->elink[j])
				glColor4f(1, 1, 0, 0.25);

			k=(j+1)%cur->n_vec;
			xyz=cur->xyz+j*3;
			xyz1=cur->xyz+k*3;
			glVertex3f(xyz[0], xyz[1], xyz[2]+1.1);
			glVertex3f(xyz1[0], xyz1[1], xyz1[2]+1.1);
		}
		glEnd();
#endif

		i++;
		cur=cur->next;
	}
}

