#include <lbxgl2.h>

extern PDGLUI_InputState *lbxgl_state;
extern PDGLUI_Camera *lbxgl_cam;
extern LBXGL_BrushWorld *lbxgl_world;

LBXGL_API LBXGL_Brush *LBXGL_BrushMesh_MakeBrushUsingPrim(
	LBXGL_MeshPrim *prim)
{
	LBXGL_Brush *tmp;

	LBXGL_Brush_Init();

	tmp=gctalloc("lbxgl_brush_t", sizeof(LBXGL_Brush));
	tmp->mesh=prim;
	return(tmp);
}

void LBXGL_BrushMesh_BuildFaces(LBXGL_Brush *brush)
{
	char tb[256];
	LBXGL_MeshPrim *cur;	
	float *xyz;
	float xsc, ysc;
	int tn, xs, ys;
	int i, j;

	brush->flags&=~(LBXGL_BRFL_ALPHA|LBXGL_TXFL_FLUID_MASK|
		LBXGL_TXFL_EFFECT_MASK|LBXGL_BRFL_NOSHADOW|LBXGL_BRFL_QUADS);

	sprintf(tb, "textures/%s", brush->mesh->tex);
	tn=LBXGL_Texture_LoadImageSize(tb, &xs, &ys);

	if(tn<0)
	{
		tn=LBXGL_Texture_LoadImageSize(
			"textures/effects/missingtex", &xs, &ys);
	}

	brush->mesh->texnum=tn;
	i=LBXGL_Texture_GetImageFlags(tn);
//	brush->patch->texfl=i;
	brush->texfl=i;

	if(i&LBXGL_TXFL_ALPHA)brush->flags|=LBXGL_BRFL_ALPHA;
	if(i&LBXGL_TXFL_VALPHA)brush->flags|=LBXGL_BRFL_ALPHA;
	if(i&LBXGL_TXFL_WINDOW)brush->flags|=LBXGL_BRFL_ALPHA;
//	if(i&LBXGL_TXFL_NODRAW)brush->flags|=LBXGL_BRFL_ALPHA;

	if(i&LBXGL_TXFL_NOSHADOW)brush->flags|=LBXGL_BRFL_NOSHADOW;

	if(i&LBXGL_TXFL_FLUID)
	{
		brush->flags|=LBXGL_BRFL_ALPHA|LBXGL_BRFL_FLUID;
		brush->flags|=i&LBXGL_TXFL_FLUID_MASK;
	}

	if(i&LBXGL_TXFL_EFFECT)
	{
		brush->flags|=i&LBXGL_TXFL_EFFECT_MASK;
	}

	brush->flags|=i&LBXGL_TXFL_MATERIAL_MASK;

	V3F_CONST(brush->mins,  999999999);
	V3F_CONST(brush->maxs, -999999999);
//	LBXGL_Mesh_BoxPrim(brush->mesh, brush->mins, brush->maxs);

	cur=brush->mesh;
	while(cur)
	{
		LBXGL_Mesh_BoxPrim(cur, brush->mins, brush->maxs);
//		LBXGL_Mesh_DrawPrim(cur);
		cur=cur->bnext;
	}

	V3F_SCALEADDSCALE(brush->mins, 0.5, brush->maxs, 0.5, brush->org);
	brush->rad=V3F_DIST(brush->maxs, brush->org);

	cur=brush->mesh;
	while(cur)
	{
		if(!cur->edge)
			LBXGL_Mesh_BuildCache(cur);
		cur=cur->bnext;
	}

//	if(!brush->mesh->edge)
//	{
//		LBXGL_Mesh_BuildCache(brush->mesh);
//	}
}

void LBXGL_BrushMesh_DestroyFaces(LBXGL_Brush *brush)
{
}

LBXGL_API int LBXGL_BrushMesh_CheckPointBrush(LBXGL_Brush *brush, float *pt)
{
	return(0);
}

LBXGL_API int LBXGL_BrushMesh_CheckLineBrushClip(LBXGL_Brush *brush,
	float *sv, float *ev, float *csv, float *cev)
{
	int i;

	if(csv) { V3F_COPY(sv, csv); }
	if(cev) { V3F_COPY(ev, cev); }
	
//	i=LBXGL_Mesh_CheckLinePrimClip(brush->mesh, sv, ev, csv, cev);
//	return(i);
	return(0);
}

LBXGL_API int LBXGL_BrushMesh_CheckBoxBrush(LBXGL_Brush *brush,
	float *mins, float *maxs)
{
	if(!SolidAABB_BoxCollideP(mins, maxs, brush->mins, brush->maxs))
		return(0);
	return(1);
}

LBXGL_API int LBXGL_BrushMesh_CheckSphereBrush(LBXGL_Brush *brush,
	float *org, float rad)
{
	float f;
	
	f=V3F_DIST(org, brush->org);
	if(f>(brush->rad+rad))
		return(0);
	return(1);
}

LBXGL_API int LBXGL_BrushMesh_GetExtents(LBXGL_Brush *brush, float *vec,
	float *rm, float *rn)
{
	LBXGL_Brush_GetBBoxExtents(brush, vec, rm, rn);
	return(0);
}

LBXGL_API int LBXGL_BrushMesh_GetPlaneExtents(LBXGL_Brush *brush, float *vec,
	float *rm, float *rn)
{
	LBXGL_Brush_GetBBoxPlaneExtents(brush, vec, rm, rn);
	return(0);
}

LBXGL_API void LBXGL_BrushMesh_DrawFaces(LBXGL_Brush *brush)
{
	LBXGL_MeshPrim *cur;
	
	cur=brush->mesh;
	while(cur)
	{
		LBXGL_Mesh_DrawPrim(cur);
		cur=cur->bnext;
	}

//	LBXGL_Mesh_DrawPrim(brush->mesh);
}

LBXGL_API void LBXGL_BrushMesh_DrawFacesAlpha(LBXGL_Brush *brush)
{
	LBXGL_MeshPrim *cur;
	
	cur=brush->mesh;
	while(cur)
	{
		LBXGL_Mesh_DrawPrimAlpha(cur);
		cur=cur->bnext;
	}

//	LBXGL_Mesh_DrawPrimAlpha(brush->mesh);
}

LBXGL_API void LBXGL_BrushMesh_DrawFacesShadow(LBXGL_Brush *brush)
{
	LBXGL_MeshPrim *cur;
	
	cur=brush->mesh;
	while(cur)
	{
		LBXGL_Mesh_DrawPrimFinal(cur);
		cur=cur->bnext;
	}

//	LBXGL_Mesh_DrawPrimFinal(brush->mesh);
}

LBXGL_API void LBXGL_BrushMesh_DrawShadowVolumeDP(
	LBXGL_Brush *brush, LBXGL_Light *light)
{
	LBXGL_MeshPrim *cur;
	
	cur=brush->mesh;
	while(cur)
	{
		LBXGL_Mesh_DrawPrimShadowVolume(cur,
			light->org, light->curval, 0);
		cur=cur->bnext;
	}

//	LBXGL_Mesh_DrawPrimShadowVolume(brush->mesh,
//		light->org, light->curval, 0);
}

LBXGL_API void LBXGL_BrushMesh_DrawShadowVolume(
	LBXGL_Brush *brush, LBXGL_Light *light)
{
//	LBXGL_MeshPrim *cur;
	
//	cur=brush->mesh;
//	while(cur)
//	{
//		LBXGL_BrushMesh_DrawShadowVolumeDP(cur, light);
//		cur=cur->bnext;
//	}

	LBXGL_BrushMesh_DrawShadowVolumeDP(brush, light);
}

LBXGL_API void LBXGL_BrushMesh_DrawShadowVolumeOrg(
	LBXGL_Brush *brush, float *lorg, float lrad)
{
	LBXGL_MeshPrim *cur;
	
	cur=brush->mesh;
	while(cur)
	{
		LBXGL_Mesh_DrawPrimShadowVolume(cur, lorg, lrad, 1);
		cur=cur->bnext;
	}

//	LBXGL_Mesh_DrawPrimShadowVolume(brush->mesh, lorg, lrad, 1);
}

LBXGL_API void LBXGL_BrushMesh_DrawShadowVolumeDPOrgClip(
	LBXGL_Brush *brush, LBXGL_Light *light,
	float *lorg, float lrad)
{
	LBXGL_MeshPrim *cur;
	
	cur=brush->mesh;
	while(cur)
	{
		LBXGL_Mesh_DrawPrimShadowVolume(cur, lorg, lrad, 1);
		cur=cur->bnext;
	}

//	LBXGL_Mesh_DrawPrimShadowVolume(brush->mesh, lorg, lrad, 1);
}

LBXGL_API void LBXGL_BrushMesh_DrawBrushDark(LBXGL_Brush *brush)
{
	LBXGL_MeshPrim *cur;
	
	cur=brush->mesh;
	while(cur)
	{
		LBXGL_Mesh_DrawPrimDark(cur);
		cur=cur->bnext;
	}

//	LBXGL_Mesh_DrawPrimDark(brush->mesh);
}

LBXGL_API void LBXGL_BrushMesh_DrawBrushLight2(LBXGL_Brush *brush)
{
	LBXGL_MeshPrim *cur;
	
	cur=brush->mesh;
	while(cur)
	{
		LBXGL_Mesh_DrawPrimLight(cur);
		cur=cur->bnext;
	}

//	LBXGL_Mesh_DrawPrimLight(brush->mesh);
}

LBXGL_API void LBXGL_BrushMesh_DrawBrushLight3(
	LBXGL_Brush *brush, LBXGL_Light *light)
{
	LBXGL_MeshPrim *cur;
	
	cur=brush->mesh;
	while(cur)
	{
		LBXGL_Mesh_DrawPrimLight(cur);
		cur=cur->bnext;
	}

//	LBXGL_Mesh_DrawPrimLight(brush->mesh);
}

LBXGL_API void LBXGL_BrushMesh_DrawSolidFlat(LBXGL_Brush *brush)
{
	LBXGL_MeshPrim *cur;
	
	cur=brush->mesh;
	while(cur)
	{
		LBXGL_Mesh_DrawPrimFlat(cur);
		cur=cur->bnext;
	}

//	LBXGL_Mesh_DrawPrimFlat(brush->mesh);
}

LBXGL_API void LBXGL_BrushMesh_DrawFlat(LBXGL_Brush *brush)
{
	LBXGL_MeshPrim *cur;
	
	cur=brush->mesh;
	while(cur)
	{
		LBXGL_Mesh_DrawPrimFlat(cur);
		cur=cur->bnext;
	}

//	LBXGL_Mesh_DrawPrimFlat(brush->mesh);
}

void LBXGL_BrushMesh_InitMeshRGB(
	LBXGL_BrushWorld *world, LBXGL_Brush *brush)
{
}

LBXGL_API void LBXGL_BrushMesh_UpdateMeshLight(
	LBXGL_BrushWorld *world, LBXGL_Brush *brush, LBXGL_Light *light)
{
}
