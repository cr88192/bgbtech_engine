#include <lbxgl2.h>

extern PDGLUI_InputState *lbxgl_state;
extern PDGLUI_Camera *lbxgl_cam;
extern LBXGL_BrushWorld *lbxgl_world;

LBXGL_BrushPatchPoint *LBXGL_BrushPatch_GetPoint(
	LBXGL_Brush *brush, int x, int y)
{
	LBXGL_BrushPatchPoint *tmp;

	if(x<0)x=0;
	if(y<0)y=0;

	if(x>=brush->patch->nx_pts)
		x=brush->patch->nx_pts-1;
	if(y>=brush->patch->ny_pts)
		y=brush->patch->ny_pts-1;
		
	tmp=brush->patch->pts[y][x];
	return(tmp);
}

void LBXGL_BrushPatch_InterpolateVecXYZ(
	float *xyz0, float *uv0,
	float *xyz1, float *uv1,
	float *xyz2, float *uv2,
	float *xyz, float *uv,
	float t)
{
	float tv0[3], tv1[3], tv2[3];
	float tw0[2], tw1[2], tw2[2];
	float nt;

#if 0
	if((xyz0==xyz1) || (xyz1==xyz2))
	{
		V3F_COPY(xyz1, xyz);
		V2F_COPY(uv1, uv);
		return;
	}
#endif
	
	if(t<0)t=0;
	if(t>1)t=1;
	
	nt=1.0-t;
	V3F_SCALEADDSCALE(xyz0, nt, xyz1, t, tv0);
	V3F_SCALEADDSCALE(xyz1, nt, xyz2, t, tv1);
	V3F_SCALEADDSCALE(tv0, nt, tv1, t, xyz);

	V2F_SCALEADDSCALE(uv0, nt, uv1, t, tw0);
	V2F_SCALEADDSCALE(uv1, nt, uv2, t, tw1);
	V2F_SCALEADDSCALE(tw0, nt, tw1, t, uv);
}

#if 0
void LBXGL_BrushPatch_Interpolate5VecXYZ(
	float *xyz0, float *uv0,
	float *xyz1, float *uv1,
	float *xyz2, float *uv2,
	float *xyz3, float *uv3,
	float *xyz4, float *uv4,
	float *xyz, float *uv,
	float t)
{
	float tv0[3], tv1[3], tv2[3];
	float tw0[2], tw1[2], tw2[2];
	
	LBXGL_BrushPatch_InterpolateVecXYZ(
		xyz0, uv0, xyz1, uv1, xyz2, uv2,
		tv0, tw0, 0.5);
	LBXGL_BrushPatch_InterpolateVecXYZ(
		xyz2, uv2, xyz3, uv3, xyz4, uv4,
		tv1, tw1, 0.5);
	LBXGL_BrushPatch_InterpolateVecXYZ(
		tv0, tw0, xyz2, uv2, tv1, tw1,
		xyz, uv, t);
}
#endif

void LBXGL_BrushPatch_InterpolateVecPts(
	LBXGL_BrushPatchPoint *pt0,
	LBXGL_BrushPatchPoint *pt1,
	LBXGL_BrushPatchPoint *pt2,
	LBXGL_BrushPatchPoint *pt,
	float t)
{
	LBXGL_BrushPatch_InterpolateVecXYZ(
		pt0->xyz, pt0->uv,
		pt1->xyz, pt1->uv,
		pt2->xyz, pt2->uv,
		pt->xyz, pt->uv, t);
}

#if 0
void LBXGL_BrushPatch_Interpolate5VecPts(
	LBXGL_BrushPatchPoint *pt0,
	LBXGL_BrushPatchPoint *pt1,
	LBXGL_BrushPatchPoint *pt2,
	LBXGL_BrushPatchPoint *pt3,
	LBXGL_BrushPatchPoint *pt4,
	LBXGL_BrushPatchPoint *pt,
	float t)
{
	LBXGL_BrushPatch_Interpolate5VecXYZ(
		pt0->xyz, pt0->uv,
		pt1->xyz, pt1->uv,
		pt2->xyz, pt2->uv,
		pt3->xyz, pt3->uv,
		pt4->xyz, pt4->uv,
		pt->xyz, pt->uv, t);
}
#endif

#if 1
void LBXGL_BrushPatch_InterpolateVec(
	LBXGL_Brush *brush, LBXGL_BrushPatchPoint *vec, float xpos, float ypos)
{
	LBXGL_BrushPatchPoint *pt11, *pt12, *pt13;
	LBXGL_BrushPatchPoint *pt21, *pt22, *pt23;
	LBXGL_BrushPatchPoint *pt31, *pt32, *pt33;

	LBXGL_BrushPatchPoint tpt0, tpt1, tpt2, tpt3, tpt4;

	float xf, yf;
	float f, g;
	int x, y;

	if(brush->patch->nx_pts==3)
	{
		x=1; xf=xpos;
	}else
	{
		f=(brush->patch->nx_pts-2)*xpos;
		x=(int)floor(f+1);
		xf=xpos;
	}

	if(brush->patch->ny_pts==3)
	{
		y=1; yf=ypos;
	}else
	{
		f=(brush->patch->ny_pts-2)*ypos;
		y=(int)floor(f+1);
		yf=ypos;
	}
	
	
//	xf=1-xf;
//	yf=1-yf;

//	x=(int)floor(xpos-0.5); y=(int)floor(ypos-0.5);
//	x=(int)floor(xpos+0.5); y=(int)floor(ypos+0.5);
//	x=(int)floor(xpos+1); y=(int)floor(ypos+1);
//	x=(int)floor(xpos); y=(int)floor(ypos);
	
	pt11=LBXGL_BrushPatch_GetPoint(brush, x-1, y-1);
	pt12=LBXGL_BrushPatch_GetPoint(brush, x+0, y-1);
	pt13=LBXGL_BrushPatch_GetPoint(brush, x+1, y-1);

	pt21=LBXGL_BrushPatch_GetPoint(brush, x-1, y+0);
	pt22=LBXGL_BrushPatch_GetPoint(brush, x+0, y+0);
	pt23=LBXGL_BrushPatch_GetPoint(brush, x+1, y+0);

	pt31=LBXGL_BrushPatch_GetPoint(brush, x-1, y+1);
	pt32=LBXGL_BrushPatch_GetPoint(brush, x+0, y+1);
	pt33=LBXGL_BrushPatch_GetPoint(brush, x+1, y+1);
	
	LBXGL_BrushPatch_InterpolateVecPts(
		pt11, pt12, pt13, &tpt1, xf);
	LBXGL_BrushPatch_InterpolateVecPts(
		pt21, pt22, pt23, &tpt2, xf);
	LBXGL_BrushPatch_InterpolateVecPts(
		pt31, pt32, pt33, &tpt3, xf);

	LBXGL_BrushPatch_InterpolateVecPts(
		&tpt1, &tpt2, &tpt3, vec, yf);
}
#endif

#if 0
void LBXGL_BrushPatch_InterpolateVec(
	LBXGL_Brush *brush, LBXGL_BrushPatchPoint *vec, float xpos, float ypos)
{
	LBXGL_BrushPatchPoint *pt00, *pt01, *pt02, *pt03, *pt04;
	LBXGL_BrushPatchPoint *pt10, *pt11, *pt12, *pt13, *pt14;
	LBXGL_BrushPatchPoint *pt20, *pt21, *pt22, *pt23, *pt24;
	LBXGL_BrushPatchPoint *pt30, *pt31, *pt32, *pt33, *pt34;
	LBXGL_BrushPatchPoint *pt40, *pt41, *pt42, *pt43, *pt44;

	LBXGL_BrushPatchPoint tpt0, tpt1, tpt2, tpt3, tpt4;

	float xf, yf;
	int x, y;
	
//	x=(int)floor(xpos-0.5); y=(int)floor(ypos-0.5);
	x=(int)floor(xpos+0.5); y=(int)floor(ypos+0.5);
//	x=(int)floor(xpos); y=(int)floor(ypos);
	
	pt00=LBXGL_BrushPatch_GetPoint(brush, x-2, y-2);
	pt01=LBXGL_BrushPatch_GetPoint(brush, x-1, y-2);
	pt02=LBXGL_BrushPatch_GetPoint(brush, x+0, y-2);
	pt03=LBXGL_BrushPatch_GetPoint(brush, x+1, y-2);
	pt04=LBXGL_BrushPatch_GetPoint(brush, x+2, y-2);

	pt10=LBXGL_BrushPatch_GetPoint(brush, x-2, y-1);
	pt11=LBXGL_BrushPatch_GetPoint(brush, x-1, y-1);
	pt12=LBXGL_BrushPatch_GetPoint(brush, x+0, y-1);
	pt13=LBXGL_BrushPatch_GetPoint(brush, x+1, y-1);
	pt14=LBXGL_BrushPatch_GetPoint(brush, x+2, y-1);

	pt20=LBXGL_BrushPatch_GetPoint(brush, x-2, y+0);
	pt21=LBXGL_BrushPatch_GetPoint(brush, x-1, y+0);
	pt22=LBXGL_BrushPatch_GetPoint(brush, x+0, y+0);
	pt23=LBXGL_BrushPatch_GetPoint(brush, x+1, y+0);
	pt24=LBXGL_BrushPatch_GetPoint(brush, x+2, y+0);

	pt30=LBXGL_BrushPatch_GetPoint(brush, x-2, y+1);
	pt31=LBXGL_BrushPatch_GetPoint(brush, x-1, y+1);
	pt32=LBXGL_BrushPatch_GetPoint(brush, x+0, y+1);
	pt33=LBXGL_BrushPatch_GetPoint(brush, x+1, y+1);
	pt34=LBXGL_BrushPatch_GetPoint(brush, x+2, y+1);

	pt40=LBXGL_BrushPatch_GetPoint(brush, x-2, y+2);
	pt41=LBXGL_BrushPatch_GetPoint(brush, x-1, y+2);
	pt42=LBXGL_BrushPatch_GetPoint(brush, x+0, y+2);
	pt43=LBXGL_BrushPatch_GetPoint(brush, x+1, y+2);
	pt44=LBXGL_BrushPatch_GetPoint(brush, x+2, y+2);

	if((pt21==pt22) || (pt23==pt22))
	{
		if(pt12==pt22)
		{
			*vec=*pt22;
			return;
		}
		if(pt32==pt22)
		{
			*vec=*pt22;
			return;
		}
	}

	xf=xpos-((int)xpos);
	yf=ypos-((int)ypos);
	
//	xf=1-xf;
//	yf=1-yf;
	
	LBXGL_BrushPatch_Interpolate5VecPts(
		pt00, pt01, pt02, pt03, pt04, &tpt0, xf);
	LBXGL_BrushPatch_Interpolate5VecPts(
		pt10, pt11, pt12, pt13, pt14, &tpt1, xf);
	LBXGL_BrushPatch_Interpolate5VecPts(
		pt20, pt21, pt22, pt23, pt24, &tpt2, xf);
	LBXGL_BrushPatch_Interpolate5VecPts(
		pt30, pt31, pt32, pt33, pt34, &tpt3, xf);
	LBXGL_BrushPatch_Interpolate5VecPts(
		pt40, pt41, pt42, pt43, pt44, &tpt4, xf);

	LBXGL_BrushPatch_Interpolate5VecPts(
		&tpt0, &tpt1, &tpt2, &tpt3, &tpt4, vec, yf);
}
#endif

void LBXGL_BrushPatch_BuildFaces(LBXGL_Brush *brush)
{
	char tb[256];
	LBXGL_BrushPatchPoint *p0, *p1, *p2, *p3;
	float *xyz;
	float xsc, ysc;
	int tn, xs, ys;
	int i, j;

	brush->flags&=~(LBXGL_BRFL_ALPHA|LBXGL_TXFL_FLUID_MASK|
		LBXGL_TXFL_EFFECT_MASK|LBXGL_BRFL_NOSHADOW|LBXGL_BRFL_QUADS);

	sprintf(tb, "textures/%s", brush->patch->tex);
	tn=LBXGL_Texture_LoadImageSize(tb, &xs, &ys);

	if(tn<0)
	{
		tn=LBXGL_Texture_LoadImageSize(
			"textures/effects/missingtex", &xs, &ys);
	}

	brush->patch->texnum=tn;
	i=LBXGL_Texture_GetImageFlags(tn);
	brush->patch->texfl=i;
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


	brush->mins[0]=999999; brush->mins[1]=999999; brush->mins[2]=999999;
	brush->maxs[0]=-999999; brush->maxs[1]=-999999; brush->maxs[2]=-999999;
	for(i=0; i<brush->patch->ny_pts; i++)
		for(j=0; j<brush->patch->nx_pts; j++)
	{
		xyz=brush->patch->pts[i][j]->xyz;
	
		if(xyz[0]<brush->mins[0])brush->mins[0]=xyz[0];
		if(xyz[1]<brush->mins[1])brush->mins[1]=xyz[1];
		if(xyz[2]<brush->mins[2])brush->mins[2]=xyz[2];

		if(xyz[0]>brush->maxs[0])brush->maxs[0]=xyz[0];
		if(xyz[1]>brush->maxs[1])brush->maxs[1]=xyz[1];
		if(xyz[2]>brush->maxs[2])brush->maxs[2]=xyz[2];
	}
	
	V3F_SCALEADDSCALE(brush->mins, 0.5, brush->maxs, 0.5, brush->org);
	brush->rad=V3F_DIST(brush->maxs, brush->org);

#if 1
	xs=brush->patch->nx_pts;
	ys=brush->patch->ny_pts;
	for(i=0; i<(ys-1); i++)
		for(j=0; j<(xs-1); j++)
	{
		p0=brush->patch->pts[i+0][j+0];
		p1=brush->patch->pts[i+0][j+1];
		p2=brush->patch->pts[i+1][j+0];
		p3=brush->patch->pts[i+1][j+1];
		LBXGL_BrushPatch_CalcNormalPts(p0, p1, p2, p0->norm);
		p0->norm[3]=V3F_DOT(p0->xyz, p0->norm);
	}
#endif

#if 1
	if(brush->patch->vec)
	{
		gcfree(brush->patch->vec);
		brush->patch->vec=NULL;
	}

	xs=brush->patch->nx_pts*3;
	ys=brush->patch->ny_pts*3;

	brush->patch->vec=gcatomic(xs*ys*sizeof(LBXGL_BrushPatchPoint));
	brush->patch->vec_xs=xs;
	brush->patch->vec_ys=ys;
	
//	xsc=(brush->patch->nx_pts)/((float)(xs-2));
//	ysc=(brush->patch->ny_pts)/((float)(ys-2));
//	xsc=(brush->patch->nx_pts)/((float)xs);
//	ysc=(brush->patch->ny_pts)/((float)ys);

//	xsc=(brush->patch->nx_pts-2)/((float)xs);
//	ysc=(brush->patch->ny_pts-2)/((float)ys);

	xsc=1.0/((float)(xs-1));
	ysc=1.0/((float)(ys-1));

	for(i=0; i<ys; i++)
		for(j=0; j<xs; j++)
	{
		LBXGL_BrushPatch_InterpolateVec(brush,
			brush->patch->vec+i*xs+j, j*xsc, i*ysc);
	}

	for(i=0; i<(ys-1); i++)
		for(j=0; j<(xs-1); j++)
	{
		p0=brush->patch->vec+((i+0)*xs+(j+0));
		p1=brush->patch->vec+((i+0)*xs+(j+1));
		p2=brush->patch->vec+((i+1)*xs+(j+0));
		p3=brush->patch->vec+((i+1)*xs+(j+1));
		LBXGL_BrushPatch_CalcNormalPts(p0, p1, p2, p0->norm);
		p0->norm[3]=V3F_DOT(p0->xyz, p0->norm);
		
		V4F_COPY(p0->norm, p1->norm);
		V4F_COPY(p0->norm, p2->norm);
		V4F_COPY(p0->norm, p3->norm);
	}

#endif
}

void LBXGL_BrushPatch_DestroyFaces(LBXGL_Brush *brush)
{
}

void LBXGL_BrushPatch_CalcNormalPts(
	LBXGL_BrushPatchPoint *pt0,
	LBXGL_BrushPatchPoint *pt1,
	LBXGL_BrushPatchPoint *pt2,
	float *norm)
{
	float tv0[3], tv1[3], tv2[3];

	Vec3F_Subtract(pt1->xyz, pt0->xyz, tv0);
	Vec3F_Subtract(pt2->xyz, pt0->xyz, tv1);
//	V3F_CROSS(tv0, tv1, tv2);
	V3F_CROSS(tv1, tv0, tv2);
	V3F_NORMALIZE(tv2, norm);
}

LBXGL_API int LBXGL_BrushPatch_CheckPointBrush(LBXGL_Brush *brush, float *pt)
{
	return(0);
}

LBXGL_API int LBXGL_BrushPatch_CheckLineBrushClip(LBXGL_Brush *brush,
	float *sv, float *ev, float *csv, float *cev)
{
	if(csv) { V3F_COPY(sv, csv); }
	if(cev) { V3F_COPY(ev, cev); }
	
	return(0);
}

LBXGL_API int LBXGL_BrushPatch_CheckBoxBrush(LBXGL_Brush *brush,
	float *mins, float *maxs)
{
	if(!SolidAABB_BoxCollideP(mins, maxs, brush->mins, brush->maxs))
		return(0);
	return(1);
}

LBXGL_API int LBXGL_BrushPatch_CheckSphereBrush(LBXGL_Brush *brush,
	float *org, float rad)
{
	float f;
	
	f=V3F_DIST(org, brush->org);
	if(f>(brush->rad+rad))
		return(0);
	return(1);
}

LBXGL_API int LBXGL_BrushPatch_GetExtents(LBXGL_Brush *brush, float *vec,
	float *rm, float *rn)
{
	LBXGL_Brush_GetBBoxExtents(brush, vec, rm, rn);
	return(0);
}

LBXGL_API int LBXGL_BrushPatch_GetPlaneExtents(LBXGL_Brush *brush, float *vec,
	float *rm, float *rn)
{
	LBXGL_Brush_GetBBoxPlaneExtents(brush, vec, rm, rn);
	return(0);
}


LBXGL_API void LBXGL_BrushPatch_DrawFaces(LBXGL_Brush *brush)
{
	float tv0[3], tv1[3], tv2[3];
	LBXGL_BrushPatchPoint *p0, *p1, *p2, *p3;
	int xs, ys;
	int i, j;

//	if(!LBXGL_Brush_CheckBrushFrustum(brush))return;

	LBXGL_Shader_BindTexture(brush->patch->texnum);
	LBXGL_Shader_Color4f(1, 1, 1, 1);

#if 1
	LBXGL_Shader_Begin(PDGL_QUADS);

	xs=brush->patch->vec_xs;
	ys=brush->patch->vec_ys;
//	xs=brush->patch->nx_pts;
//	ys=brush->patch->ny_pts;
	for(i=0; i<ys-1; i++)
		for(j=0; j<xs-1; j++)
	{
		p0=brush->patch->vec+((i+0)*xs+(j+0));
		p1=brush->patch->vec+((i+0)*xs+(j+1));
		p2=brush->patch->vec+((i+1)*xs+(j+0));
		p3=brush->patch->vec+((i+1)*xs+(j+1));

//		p0=brush->patch->pts[i+0][j+0];
//		p1=brush->patch->pts[i+0][j+1];
//		p2=brush->patch->pts[i+1][j+0];
//		p3=brush->patch->pts[i+1][j+1];

#if 0
		Vec3F_Subtract(p1->xyz, p0->xyz, tv0);
		Vec3F_Subtract(p2->xyz, p0->xyz, tv1);
//		V3F_CROSS(tv0, tv1, tv2);
		V3F_CROSS(tv1, tv0, tv2);
		V3F_NORMALIZE(tv2, tv2);
		LBXGL_Shader_Normal3fv(tv2);
#endif

		LBXGL_Shader_Normal3fv(p0->norm);

//		LBXGL_Shader_BeginPoly();

		LBXGL_Shader_TexCoord2fv(p0->uv);
		LBXGL_Shader_Vertex3fv(p0->xyz);

		LBXGL_Shader_TexCoord2fv(p2->uv);
		LBXGL_Shader_Vertex3fv(p2->xyz);

		LBXGL_Shader_TexCoord2fv(p3->uv);
		LBXGL_Shader_Vertex3fv(p3->xyz);

		LBXGL_Shader_TexCoord2fv(p1->uv);
		LBXGL_Shader_Vertex3fv(p1->xyz);

//		LBXGL_Shader_EndPoly();
	}

	LBXGL_Shader_End();
#endif

#if 0
	for(i=0; i<brush->patch->ny_pts-1; i++)
		for(j=0; j<brush->patch->nx_pts-1; j++)
	{
		Vec3F_Subtract(
			brush->patch->pts[i+0][j+1]->xyz,
			brush->patch->pts[i+0][j+0]->xyz, tv0);
		Vec3F_Subtract(
			brush->patch->pts[i+1][j+0]->xyz,
			brush->patch->pts[i+0][j+0]->xyz, tv1);
//		V3F_CROSS(tv0, tv1, tv2);
		V3F_CROSS(tv1, tv0, tv2);
		V3F_NORMALIZE(tv2, tv2);
		LBXGL_Shader_Normal3fv(tv2);
	
		LBXGL_Shader_BeginPoly();
		LBXGL_Shader_TexCoord2fv(brush->patch->pts[i+0][j+0]->uv);
		LBXGL_Shader_Vertex3fv(brush->patch->pts[i+0][j+0]->xyz);

		LBXGL_Shader_TexCoord2fv(brush->patch->pts[i+1][j+0]->uv);
		LBXGL_Shader_Vertex3fv(brush->patch->pts[i+1][j+0]->xyz);

		LBXGL_Shader_TexCoord2fv(brush->patch->pts[i+1][j+1]->uv);
		LBXGL_Shader_Vertex3fv(brush->patch->pts[i+1][j+1]->xyz);

		LBXGL_Shader_TexCoord2fv(brush->patch->pts[i+0][j+1]->uv);
		LBXGL_Shader_Vertex3fv(brush->patch->pts[i+0][j+1]->xyz);

		LBXGL_Shader_EndPoly();
	}
#endif
}

LBXGL_API void LBXGL_BrushPatch_DrawFacesAlpha(LBXGL_Brush *brush)
{
	float tv0[3], tv1[3], tv2[3];
	LBXGL_BrushPatchPoint *p0, *p1, *p2, *p3;
	int xs, ys;
	int i, j;

//	if(!LBXGL_Brush_CheckBrushFrustum(brush))return;

	LBXGL_Shader_BindTexture(brush->patch->texnum);

#if 1

	LBXGL_Shader_Begin(PDGL_QUADS);

	xs=brush->patch->vec_xs;
	ys=brush->patch->vec_ys;
	for(i=0; i<ys-1; i++)
		for(j=0; j<xs-1; j++)
	{
		p0=brush->patch->vec+((i+0)*xs+(j+0));
		p1=brush->patch->vec+((i+0)*xs+(j+1));
		p2=brush->patch->vec+((i+1)*xs+(j+0));
		p3=brush->patch->vec+((i+1)*xs+(j+1));

		LBXGL_Shader_Normal3fv(p0->norm);

//		LBXGL_Shader_BeginPoly();
		LBXGL_Shader_Color4fv(p0->rgba);
		LBXGL_Shader_TexCoord2fv(p0->uv);
		LBXGL_Shader_Vertex3fv(p0->xyz);

		LBXGL_Shader_Color4fv(p2->rgba);
		LBXGL_Shader_TexCoord2fv(p2->uv);
		LBXGL_Shader_Vertex3fv(p2->xyz);

		LBXGL_Shader_Color4fv(p3->rgba);
		LBXGL_Shader_TexCoord2fv(p3->uv);
		LBXGL_Shader_Vertex3fv(p3->xyz);

		LBXGL_Shader_Color4fv(p1->rgba);
		LBXGL_Shader_TexCoord2fv(p1->uv);
		LBXGL_Shader_Vertex3fv(p1->xyz);

//		LBXGL_Shader_EndPoly();
	}
	LBXGL_Shader_End();
#endif
}

LBXGL_API void LBXGL_BrushPatch_DrawFacesShadow(LBXGL_Brush *brush)
{
	float tv0[3], tv1[3], tv2[3];
	LBXGL_BrushPatchPoint *p0, *p1, *p2, *p3;
	int xs, ys;
	int i, j;

	if(brush->flags&(LBXGL_BRFL_HIDDEN|
			LBXGL_BRFL_ALPHA|LBXGL_BRFL_EFFECT))
		return;

//	if(!LBXGL_Brush_CheckBrushFrustum(brush))return;
//	if(!LBXGL_Brush_CheckBrushFrustumVisible2(brush))return;

	if(brush->patch->texfl&LBXGL_TXFL_DETAIL)
	{
		if(V3F_DIST(brush->org, lbxgl_cam->org)>(1024+brush->rad))
			return;
	}

//	LBXGL_Shader_BindTexture(brush->pf_tex[i]);
	LBXGL_Shader_BindTexture(brush->patch->texnum);

#if 0
	if(V3F_DIST(brush->org, lbxgl_cam->org)>384)
	{
		xs=brush->patch->nx_pts;
		ys=brush->patch->ny_pts;
		for(i=0; i<ys-1; i++)
			for(j=0; j<xs-1; j++)
		{
			p0=brush->patch->pts[i+0][j+0];
			p1=brush->patch->pts[i+0][j+1];
			p2=brush->patch->pts[i+1][j+0];
			p3=brush->patch->pts[i+1][j+1];

			LBXGL_Shader_Normal3fv(p0->norm);

			glBlendFunc(GL_DST_COLOR, GL_ZERO);

			LBXGL_Shader_BeginPoly();
			LBXGL_Shader_TexCoord2fv(p0->uv);
			LBXGL_Shader_Vertex3fv(p0->xyz);
			LBXGL_Shader_TexCoord2fv(p2->uv);
			LBXGL_Shader_Vertex3fv(p2->xyz);
			LBXGL_Shader_TexCoord2fv(p3->uv);
			LBXGL_Shader_Vertex3fv(p3->xyz);
			LBXGL_Shader_TexCoord2fv(p1->uv);
			LBXGL_Shader_Vertex3fv(p1->xyz);
			LBXGL_Shader_EndPoly();
		}

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		return;
	}
#endif

	glBlendFunc(GL_DST_COLOR, GL_ZERO);

	LBXGL_Shader_Begin(PDGL_QUADS);

	xs=brush->patch->vec_xs;
	ys=brush->patch->vec_ys;
//	xs=brush->patch->nx_pts;
//	ys=brush->patch->ny_pts;
	for(i=0; i<ys-1; i++)
		for(j=0; j<xs-1; j++)
	{
		p0=brush->patch->vec+((i+0)*xs+(j+0));
		p1=brush->patch->vec+((i+0)*xs+(j+1));
		p2=brush->patch->vec+((i+1)*xs+(j+0));
		p3=brush->patch->vec+((i+1)*xs+(j+1));

//		p0=brush->patch->pts[i+0][j+0];
//		p1=brush->patch->pts[i+0][j+1];
//		p2=brush->patch->pts[i+1][j+0];
//		p3=brush->patch->pts[i+1][j+1];

		LBXGL_Shader_Normal3fv(p0->norm);

//		glBlendFunc(GL_DST_COLOR, GL_ZERO);

//		LBXGL_Shader_BeginPoly();
		LBXGL_Shader_TexCoord2fv(p0->uv);
		LBXGL_Shader_Vertex3fv(p0->xyz);

		LBXGL_Shader_TexCoord2fv(p2->uv);
		LBXGL_Shader_Vertex3fv(p2->xyz);

		LBXGL_Shader_TexCoord2fv(p3->uv);
		LBXGL_Shader_Vertex3fv(p3->xyz);

		LBXGL_Shader_TexCoord2fv(p1->uv);
		LBXGL_Shader_Vertex3fv(p1->xyz);

//		LBXGL_Shader_EndPoly();
	}
	LBXGL_Shader_End();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

LBXGL_API void LBXGL_BrushPatch_DrawShadowVolumeDP(
	LBXGL_Brush *brush, LBXGL_Light *light)
{
}

LBXGL_API void LBXGL_BrushPatch_DrawShadowVolume(
	LBXGL_Brush *brush, LBXGL_Light *light)
{
}

LBXGL_API void LBXGL_BrushPatch_DrawShadowVolumeOrg(
	LBXGL_Brush *brush, float *org, float rad)
{
}

LBXGL_API void LBXGL_BrushPatch_DrawShadowVolumeDPOrgClip(
	LBXGL_Brush *brush, LBXGL_Light *light,
	float *lorg, float lrad)
{
}

LBXGL_API void LBXGL_BrushPatch_DrawBrushDark(LBXGL_Brush *brush)
{
	LBXGL_BrushPatchPoint *p0, *p1, *p2, *p3;
	int xs, ys;
	int i, j;

	if(brush->flags&(LBXGL_BRFL_HIDDEN|
			LBXGL_BRFL_ALPHA|LBXGL_BRFL_EFFECT))
		return;

	if(brush->patch->texfl&(LBXGL_TXFL_NODRAW|LBXGL_TXFL_ALPHA))
		return;

//	if(!LBXGL_Brush_CheckBrushFrustum(brush))return;
//	if(!LBXGL_Brush_CheckBrushFrustumVisible2(brush))return;
	if(!LBXGL_BrushModel_CheckBrushFrustum(brush))return;

	if(brush->patch->texfl&LBXGL_TXFL_DETAIL)
	{
		if(V3F_DIST(brush->org, lbxgl_cam->org)>(1024+brush->rad))
			return;
	}

	pdglDisableTexture2D();
	pdglColor4f(0, 0, 0, 1);

#if 0
	if(V3F_DIST(brush->org, lbxgl_cam->org)>384)
	{
		pdglBegin(PDGL_QUADS);
		xs=brush->patch->nx_pts;
		ys=brush->patch->ny_pts;
		for(i=0; i<ys-1; i++)
			for(j=0; j<xs-1; j++)
		{
			pdglVertex3fv(brush->patch->pts[i+0][j+0]->xyz);
			pdglVertex3fv(brush->patch->pts[i+1][j+0]->xyz);
			pdglVertex3fv(brush->patch->pts[i+1][j+1]->xyz);
			pdglVertex3fv(brush->patch->pts[i+0][j+1]->xyz);
		}
		pdglEnd();
		return;
	}
#endif

	pdglBegin(PDGL_QUADS);
	xs=brush->patch->vec_xs;
	ys=brush->patch->vec_ys;
//	xs=brush->patch->nx_pts;
//	ys=brush->patch->ny_pts;
	for(i=0; i<ys-1; i++)
		for(j=0; j<xs-1; j++)
	{
		p0=brush->patch->vec+((i+0)*xs+(j+0));
		p1=brush->patch->vec+((i+0)*xs+(j+1));
		p2=brush->patch->vec+((i+1)*xs+(j+0));
		p3=brush->patch->vec+((i+1)*xs+(j+1));

//		p0=brush->patch->pts[i+0][j+0];
//		p1=brush->patch->pts[i+0][j+1];
//		p2=brush->patch->pts[i+1][j+0];
//		p3=brush->patch->pts[i+1][j+1];

//		LBXGL_Shader_Normal3fv(p0->norm);

		pdglVertex3fv(p0->xyz);
		pdglVertex3fv(p2->xyz);
		pdglVertex3fv(p3->xyz);
		pdglVertex3fv(p1->xyz);
	}
	pdglEnd();
}

#if 1
LBXGL_API void LBXGL_BrushPatch_DrawBrushLight2(LBXGL_Brush *brush)
{
	float tv0[3], tv1[3], tv2[3];
	LBXGL_BrushPatchPoint *p0, *p1, *p2, *p3;
	int xs, ys;
	int i, j;

	if(brush->flags&(LBXGL_BRFL_HIDDEN|
			LBXGL_BRFL_ALPHA|LBXGL_BRFL_EFFECT))
		return;

	if(brush->patch->texfl&(LBXGL_TXFL_NODRAW|LBXGL_TXFL_ALPHA))
		return;

//	if(!LBXGL_Brush_CheckBrushFrustum(brush))return;
//	if(!LBXGL_Brush_CheckBrushFrustumVisible2(brush))return;
	if(!LBXGL_BrushModel_CheckBrushFrustum(brush))return;

	if(brush->patch->texfl&LBXGL_TXFL_DETAIL)
	{
		if(V3F_DIST(brush->org, lbxgl_cam->org)>(1024+brush->rad))
			return;
	}

	pdglDisableTexture2D();
	pdglColor4f(1, 1, 1, 1);

#if 0
	if(V3F_DIST(brush->org, lbxgl_cam->org)>384)
	{
		pdglBegin(PDGL_QUADS);
		xs=brush->patch->nx_pts;
		ys=brush->patch->ny_pts;
		for(i=0; i<ys-1; i++)
			for(j=0; j<xs-1; j++)
		{
			pdglNormal3fv(brush->patch->pts[i][j]->norm);
			pdglVertex3fv(brush->patch->pts[i+0][j+0]->xyz);
			pdglVertex3fv(brush->patch->pts[i+1][j+0]->xyz);
			pdglVertex3fv(brush->patch->pts[i+1][j+1]->xyz);
			pdglVertex3fv(brush->patch->pts[i+0][j+1]->xyz);
		}
		pdglEnd();
		return;
	}
#endif

	pdglBegin(PDGL_QUADS);

	xs=brush->patch->vec_xs;
	ys=brush->patch->vec_ys;
//	xs=brush->patch->nx_pts;
//	ys=brush->patch->ny_pts;
	for(i=0; i<ys-1; i++)
		for(j=0; j<xs-1; j++)
	{
		p0=brush->patch->vec+((i+0)*xs+(j+0));
		p1=brush->patch->vec+((i+0)*xs+(j+1));
		p2=brush->patch->vec+((i+1)*xs+(j+0));
		p3=brush->patch->vec+((i+1)*xs+(j+1));

//		p0=brush->patch->pts[i+0][j+0];
//		p1=brush->patch->pts[i+0][j+1];
//		p2=brush->patch->pts[i+1][j+0];
//		p3=brush->patch->pts[i+1][j+1];

		pdglNormal3fv(p0->norm);
		pdglVertex3fv(p0->xyz);

		pdglNormal3fv(p2->norm);
		pdglVertex3fv(p2->xyz);

		pdglNormal3fv(p3->norm);
		pdglVertex3fv(p3->xyz);

		pdglNormal3fv(p1->norm);
		pdglVertex3fv(p1->xyz);
	}
	pdglEnd();
}
#endif

#if 1
//LBXGL_API void LBXGL_BrushPatch_DrawBrushLight2(LBXGL_Brush *brush)
LBXGL_API void LBXGL_BrushPatch_DrawBrushLight3(
	LBXGL_Brush *brush, LBXGL_Light *light)
{
	static float *txyz=NULL;
	static float *tst=NULL;
	static float *tnorm=NULL;
	static float max_xyz=0;

	float tv0[3], tv1[3], tv2[3];
	LBXGL_BrushPatchPoint *p0, *p1, *p2, *p3;
	int xs, ys;
	int i, j, k;

	if(brush->flags&(LBXGL_BRFL_HIDDEN|
			LBXGL_BRFL_ALPHA|LBXGL_BRFL_EFFECT))
		return;

	if(brush->patch->texfl&(LBXGL_TXFL_NODRAW|LBXGL_TXFL_ALPHA))
		return;

	if(!LBXGL_BrushModel_CheckBrushFrustum(brush))return;

	if(brush->patch->texfl&LBXGL_TXFL_DETAIL)
	{
		if(V3F_DIST(brush->org, lbxgl_cam->org)>(1024+brush->rad))
			return;
	}

	pdglDisableTexture2D();
	pdglColor4f(1, 1, 1, 1);

	xs=brush->patch->vec_xs;
	ys=brush->patch->vec_ys;
	
	i=xs*ys*4;
	if(i>max_xyz)
	{
		txyz=realloc(txyz, i*3*sizeof(float));
		tst=realloc(tst, i*2*sizeof(float));
		tnorm=realloc(tnorm, i*4*sizeof(float));
		max_xyz=i;
	}
	
	k=0;
	for(i=0; i<ys-1; i++)
		for(j=0; j<xs-1; j++)
	{
		p0=brush->patch->vec+((i+0)*xs+(j+0));
		p1=brush->patch->vec+((i+0)*xs+(j+1));
		p2=brush->patch->vec+((i+1)*xs+(j+0));
		p3=brush->patch->vec+((i+1)*xs+(j+1));

		V3F_COPY(p0->xyz, txyz+k*3);
		V2F_COPY(p0->uv, tst+k*2);
		V4F_COPY(p0->norm, tnorm+k*4);
		k++;

		V3F_COPY(p2->xyz, txyz+k*3);
		V2F_COPY(p2->uv, tst+k*2);
		V4F_COPY(p2->norm, tnorm+k*4);
		k++;

		V3F_COPY(p3->xyz, txyz+k*3);
		V2F_COPY(p3->uv, tst+k*2);
		V4F_COPY(p3->norm, tnorm+k*4);
		k++;

		V3F_COPY(p1->xyz, txyz+k*3);
		V2F_COPY(p1->uv, tst+k*2);
		V4F_COPY(p1->norm, tnorm+k*4);
		k++;
	}
	
	if(brush->patch->texfl&LBXGL_TXFL_LIGHTMAT)
	{
		LBXGL_BrushLight_DrawPrimLight(light, PDGL_QUADS,
			txyz, tst, tnorm, k, brush->patch->texnum);
		LBXGL_BrushLight_DrawPrimLight_Reset(light);
	}else
	{
		PDGL_DrawPrim_DrawArraysNormalTex(PDGL_QUADS, 0, k,
			3, GL_FLOAT, 0, txyz,
			2, GL_FLOAT, 0, tst,
			3, GL_FLOAT, 4*sizeof(float), tnorm);

#if 0
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, txyz);
		glTexCoordPointer(2, GL_FLOAT, 0, tst);
		glNormalPointer(GL_FLOAT, 4*sizeof(float), tnorm);
		glDrawArrays(GL_QUADS, 0, k);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
#endif
	}
}
#endif

#if 0
LBXGL_API void LBXGL_BrushPatch_DrawBrushLight3(LBXGL_Brush *brush, LBXGL_Light *light)
{
	float v0[3], v1[3], v2[3];
	float tv0[3], tv1[3], tv2[3];
	float f, g;
	int i, j;

//	if(brush->patch->texfl&(LBXGL_TXFL_NODRAW|LBXGL_TXFL_ALPHA))
//		return;

//	if(!LBXGL_Brush_CheckBrushFrustum(brush))return;
//	if(!LBXGL_Brush_CheckBrushFrustumVisible2(brush))return;
	if(!LBXGL_BrushModel_CheckBrushFrustum(brush))return;

	LBXGL_BrushPatch_DrawBrushLight2(brush);

#if 0
	pdglDisableTexture2D();
	pdglColor4f(1, 1, 1, 1);

	pdglBegin(PDGL_QUADS);
	for(i=0; i<brush->patch->ny_pts-1; i++)
		for(j=0; j<brush->patch->nx_pts-1; j++)
	{
		Vec3F_Copy(brush->patch->pts[i+0][j+0]->xyz, v0);
		Vec3F_Copy(brush->patch->pts[i+0][j+1]->xyz, v1);
		Vec3F_Copy(brush->patch->pts[i+1][j+0]->xyz, v2);
		Vec3F_Subtract(v1, v0, tv0);
		Vec3F_Subtract(v2, v0, tv1);
//		V3F_CROSS(tv0, tv1, tv2);
		V3F_CROSS(tv1, tv0, tv2);
		V3F_NORMALIZE(tv2, tv2);
		
		//if we face away from the light, flip the normal
		f=V3F_DOT(light->org, tv2)-V3F_DOT(v0, tv2);
		if(f<0) { V3F_SCALE(tv2, -1, tv2); }
		
		pdglNormal3fv(tv2);
	
		pdglVertex3fv(brush->patch->pts[i+0][j+0]->xyz);
		pdglVertex3fv(brush->patch->pts[i+1][j+0]->xyz);
		pdglVertex3fv(brush->patch->pts[i+1][j+1]->xyz);
		pdglVertex3fv(brush->patch->pts[i+0][j+1]->xyz);
	}
	pdglEnd();
#endif
}
#endif

LBXGL_API void LBXGL_BrushPatch_DrawSolidFlat(LBXGL_Brush *brush)
{
	LBXGL_BrushPatchPoint *p0, *p1, *p2, *p3;
	int xs, ys;
	int i, j;

	if(brush->flags&(LBXGL_BRFL_HIDDEN|
			LBXGL_BRFL_ALPHA|LBXGL_BRFL_EFFECT))
		return;

//	if(brush->patch->texfl&(LBXGL_TXFL_NODRAW|LBXGL_TXFL_ALPHA))
//		return;

	if(brush->patch->texfl&LBXGL_TXFL_ALPHA)
		return;

//	if(!LBXGL_Brush_CheckBrushFrustum(brush))return;
	if(!LBXGL_BrushModel_CheckBrushFrustum(brush))return;

	if(brush->patch->texfl&LBXGL_TXFL_DETAIL)
	{
		if(V3F_DIST(brush->org, lbxgl_cam->org)>(1024+brush->rad))
			return;
	}

#if 1
	pdglBegin(PDGL_QUADS);

	xs=brush->patch->vec_xs;
	ys=brush->patch->vec_ys;
//	xs=brush->patch->nx_pts;
//	ys=brush->patch->ny_pts;
	for(i=0; i<ys-1; i++)
		for(j=0; j<xs-1; j++)
	{
		p0=brush->patch->vec+((i+0)*xs+(j+0));
		p1=brush->patch->vec+((i+0)*xs+(j+1));
		p2=brush->patch->vec+((i+1)*xs+(j+0));
		p3=brush->patch->vec+((i+1)*xs+(j+1));

//		p0=brush->patch->pts[i+0][j+0];
//		p1=brush->patch->pts[i+0][j+1];
//		p2=brush->patch->pts[i+1][j+0];
//		p3=brush->patch->pts[i+1][j+1];

//		pdglNormal3fv(p0->norm);
	
		pdglVertex3fv(p0->xyz);
		pdglVertex3fv(p2->xyz);
		pdglVertex3fv(p3->xyz);
		pdglVertex3fv(p1->xyz);
	}
	pdglEnd();
#endif

#if 0
	pdglBegin(PDGL_QUADS);
	for(i=0; i<brush->patch->ny_pts-1; i++)
		for(j=0; j<brush->patch->nx_pts-1; j++)
	{
		pdglVertex3fv(brush->patch->pts[i+0][j+0]->xyz);
		pdglVertex3fv(brush->patch->pts[i+1][j+0]->xyz);
		pdglVertex3fv(brush->patch->pts[i+1][j+1]->xyz);
		pdglVertex3fv(brush->patch->pts[i+0][j+1]->xyz);
	}
	pdglEnd();
#endif
}

LBXGL_API void LBXGL_BrushPatch_DrawFlat(LBXGL_Brush *brush)
{
	LBXGL_BrushPatchPoint *p0, *p1, *p2, *p3;
	int xs, ys;
	int i, j;

//	if(brush->patch->texfl&(LBXGL_TXFL_NODRAW|LBXGL_TXFL_ALPHA))
//		return;

//	if(!LBXGL_Brush_CheckBrushFrustum(brush))return;
	if(!LBXGL_BrushModel_CheckBrushFrustum(brush))return;

	if(brush->patch->texfl&LBXGL_TXFL_DETAIL)
	{
		if(V3F_DIST(brush->org, lbxgl_cam->org)>(1024+brush->rad))
			return;
	}

#if 1
	pdglBegin(PDGL_QUADS);

	xs=brush->patch->vec_xs;
	ys=brush->patch->vec_ys;
	for(i=0; i<ys-1; i++)
		for(j=0; j<xs-1; j++)
	{
		p0=brush->patch->vec+((i+0)*xs+(j+0));
		p1=brush->patch->vec+((i+0)*xs+(j+1));
		p2=brush->patch->vec+((i+1)*xs+(j+0));
		p3=brush->patch->vec+((i+1)*xs+(j+1));

		pdglVertex3fv(p0->xyz);
		pdglVertex3fv(p2->xyz);
		pdglVertex3fv(p3->xyz);
		pdglVertex3fv(p1->xyz);
	}
	pdglEnd();
#endif
}


void LBXGL_BrushPatch_InitPatchRGB(
	LBXGL_BrushWorld *world, LBXGL_Brush *brush)
{
	LBXGL_BrushPatchPoint *pt;
	int xs, ys;
	int i, j;

	V4F_SET(brush->ptrgb, 0, 0, 0, 1);
//	V4F_SET(brush->ptrgb, 1, 1, 1, 1);

	xs=brush->patch->vec_xs;
	ys=brush->patch->vec_ys;
	for(i=0; i<ys; i++)
		for(j=0; j<xs; j++)
	{
		pt=brush->patch->vec+(i*xs+j);

		pt->rgba[0]=0;
		pt->rgba[1]=0;
		pt->rgba[2]=0;
		pt->rgba[3]=1;
	}
}

LBXGL_API void LBXGL_BrushPatch_UpdatePatchLight(
	LBXGL_BrushWorld *world, LBXGL_Brush *brush, LBXGL_Light *light)
{
	float tv0[3], tv1[3], tv2[3];
	LBXGL_BrushPatchPoint *pt;
	float f, g, d;
	float tr, tg, tb;
	int xs, ys;
	int i, j;

	xs=brush->patch->vec_xs;
	ys=brush->patch->vec_ys;
	for(i=0; i<ys; i++)
		for(j=0; j<xs; j++)
	{
		pt=brush->patch->vec+(i*xs+j);

		d=V3F_DIST(pt->xyz, light->org);
//		f=light->val[0]-d;
		f=light->curval-d;
		if(f<=0)continue;

		g=f/256.0;
		tr=pt->rgba[0]+light->clr[0]*g;
		tg=pt->rgba[1]+light->clr[1]*g;
		tb=pt->rgba[2]+light->clr[2]*g;
		pt->rgba[0]=tr;
		pt->rgba[1]=tg;
		pt->rgba[2]=tb;
		pt->rgba[3]=1.0;
	}
}
