#include <btgecm.h>

BTGE_API BTGE_Brush *BTGE_BrushMesh_MakeBrushUsingPrim(
	BTGE_MeshPrim *prim)
{
	BTGE_Brush *tmp;

	BTGE_Brush_Init();

	tmp=gctalloc("lbxgl_brush_t", sizeof(BTGE_Brush));
	tmp->mesh=prim;
	return(tmp);
}

void BTGE_BrushMesh_BuildFaces(BTGE_Brush *brush)
{
	char tb[256];
	BTGE_MeshPrim *cur;	
	float *xyz;
	float xsc, ysc;
	int tn, xs, ys;
	int i, j;

	brush->flags&=~(BTGE_BRFL_ALPHA|BTGE_TXFL_FLUID_MASK|
		BTGE_TXFL_EFFECT_MASK|BTGE_BRFL_NOSHADOW);

	sprintf(tb, "textures/%s", brush->mesh->tex);
	tn=BTGE_Texture_LoadImageSize(tb, &xs, &ys);

	if(tn<0)
	{
		tn=BTGE_Texture_LoadImageSize(
			"textures/effects/missingtex", &xs, &ys);
	}

	brush->mesh->texnum=tn;
	i=BTGE_Texture_GetImageFlags(tn);
//	brush->patch->texfl=i;
	brush->texfl=i;

	if(i&BTGE_TXFL_ALPHA)brush->flags|=BTGE_BRFL_ALPHA;
	if(i&BTGE_TXFL_VALPHA)brush->flags|=BTGE_BRFL_ALPHA;
	if(i&BTGE_TXFL_WINDOW)brush->flags|=BTGE_BRFL_ALPHA;
//	if(i&BTGE_TXFL_NODRAW)brush->flags|=BTGE_BRFL_ALPHA;

	if(i&BTGE_TXFL_NOSHADOW)brush->flags|=BTGE_BRFL_NOSHADOW;
	if(i&BTGE_TXFL_NONSOLID)brush->flags|=BTGE_BRFL_NONSOLID;

	if(i&BTGE_TXFL_FLUID)
	{
		brush->flags|=BTGE_BRFL_ALPHA|BTGE_BRFL_FLUID;
		brush->flags|=i&BTGE_TXFL_FLUID_MASK;
	}

	if(i&BTGE_TXFL_EFFECT)
	{
		brush->flags|=i&BTGE_TXFL_EFFECT_MASK;
	}

	brush->flags|=i&BTGE_TXFL_MATERIAL_MASK;

	V3F_CONST(brush->mins,  999999999);
	V3F_CONST(brush->maxs, -999999999);
//	BTGE_Mesh_BoxPrim(brush->mesh, brush->mins, brush->maxs);

	cur=brush->mesh;
	while(cur)
	{
		BTGE_Mesh_BoxPrim(cur, brush->mins, brush->maxs);
		cur=cur->bnext;
	}

	V3F_SCALEADDSCALE(brush->mins, 0.5, brush->maxs, 0.5, brush->org);
	brush->rad=V3F_DIST(brush->maxs, brush->org);

	cur=brush->mesh;
	while(cur)
	{
		if(!cur->edge)
			BTGE_Mesh_BuildCache(cur);
		cur=cur->bnext;
	}

//	if(!brush->mesh->edge)
//	{
//		BTGE_Mesh_BuildCache(brush->mesh);
//	}
}

BTGE_API int BTGE_BrushMesh_CheckPointBrush(BTGE_Brush *brush, float *pt)
{
	return(0);
}

BTGE_API int BTGE_BrushMesh_CheckLineBrushClip(BTGE_Brush *brush,
	float *sv, float *ev, float *csv, float *cev)
{
	int i;
	if(csv) { V3F_COPY(sv, csv); }
	if(cev) { V3F_COPY(ev, cev); }
	
	i=BTGE_Mesh_CheckLinePrimClip(brush->mesh, sv, ev, csv, cev);
	return(i);
}

BTGE_API int BTGE_BrushMesh_CheckLineBrush(BTGE_Brush *brush,
	float *sv, float *ev)
{
	BTGE_MeshPrim *cur;	

	cur=brush->mesh;
	while(cur)
	{
		if(BTGE_Mesh_CheckLinePrim(cur, sv, ev))
			return(1);
		cur=cur->bnext;
	}
	return(0);
//	return(BTGE_Mesh_CheckLinePrim(brush->mesh, sv, ev));
}

BTGE_API int BTGE_BrushMesh_CheckBoxBrush(BTGE_Brush *brush,
	float *mins, float *maxs)
{
	BTGE_MeshPrim *cur;	

	if(!SolidAABB_BoxCollideP(mins, maxs, brush->mins, brush->maxs))
		return(0);
//	if(!BTGE_Mesh_CheckBoxPrim(brush->mesh, mins, maxs))
//		return(0);
//	return(1);

	cur=brush->mesh;
	while(cur)
	{
		if(BTGE_Mesh_CheckBoxPrim(cur, mins, maxs))
			return(1);
		cur=cur->bnext;
	}
	return(0);
}

BTGE_API int BTGE_BrushMesh_CheckSphereBrush(BTGE_Brush *brush,
	float *org, float rad)
{
	float f;
	
	f=V3F_DIST(org, brush->org);
	if(f>(brush->rad+rad))
		return(0);
	return(1);
}

BTGE_API int BTGE_BrushMesh_GetExtents(BTGE_Brush *brush, float *vec,
	float *rm, float *rn)
{
	BTGE_Brush_GetBBoxExtents(brush, vec, rm, rn);
	return(0);
}

BTGE_API int BTGE_BrushMesh_GetPlaneExtents(BTGE_Brush *brush, float *vec,
	float *rm, float *rn)
{
	BTGE_Brush_GetBBoxPlaneExtents(brush, vec, rm, rn);
	return(0);
}

void BTGE_BrushMesh_InitPatchRGB(
	BTGE_BrushWorld *world, BTGE_Brush *brush)
{
}

BTGE_API void BTGE_BrushMesh_UpdatePatchLight(
	BTGE_BrushWorld *world, BTGE_Brush *brush, BTGE_Light *light)
{
}


BTGE_API int BTGE_Mesh_CheckBoxFace(
	BTGE_MeshPrim *prim, int fn,
	float *mins, float *maxs)
{
	float v0[3], v1[3], v2[3];
	float dv0[3], dv1[3], dv2[3];
	float io[3], pt[4], norm[4], nv[4];
	float f, g;
	int n, p;
	int i0, i1, i2;
	int i, j, k;


	if(prim->fl_face[fn]&128)return(0);

	p=prim->face[fn*2+0];
	n=prim->face[fn*2+1];

	i0=prim->pts[(p+0)*2+0];
	i1=prim->pts[(p+1)*2+0];
	i2=prim->pts[(p+2)*2+0];

	V3F_COPY(prim->xyz+i0*3, v0);
	V3F_COPY(prim->xyz+i1*3, v1);
	V3F_COPY(prim->xyz+i2*3, v2);

	V3F_SUB(v0, v1, dv0);
	V3F_SUB(v2, v1, dv1);
	V3F_CROSS(dv1, dv0, norm);
	V3F_NORMALIZE(norm, norm);
	norm[3]=V3F_DOT(v0, norm);

//	printf("P0\n");
//	return(1);

	Hull_BoxPlaneExtents(mins, maxs, norm, &f, &g);

	if((f<=0) && (g<=0)) return(0);
	if((f>=0) && (g>=0)) return(0);

//	printf("P1\n");

	for(j=0; j<n; j++)
	{
		V3F_COPY(prim->xyz+prim->pts[(p+j)*2+0]*3, v0);
		V3F_COPY(prim->xyz+prim->pts[(p+(j+1)%n)*2+0]*3, v1);
		V3F_SUB(v1, v0, dv0);
		V3F_CROSS(dv0, norm, nv);
		V3F_NORMALIZE(nv, nv);
		nv[3]=V3F_DOT(v0, nv);

		Hull_BoxPlaneExtents(mins, maxs, nv, &f, &g);
		if(f>=0) return(0);
	}
	
	return(1);
}

BTGE_API int BTGE_Mesh_CheckBoxPrim(
	BTGE_MeshPrim *prim, float *mins, float *maxs)
{
	int i, j;

	for(i=0; i<prim->n_face; i++)
	{
		if(prim->fl_face[i]&128)continue;
		j=BTGE_Mesh_CheckBoxFace(prim, i, mins, maxs);
		if(j)return(1);
	}
	return(0);
}
