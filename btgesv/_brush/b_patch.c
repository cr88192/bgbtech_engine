#include <btgesv.h>

BTGE_BrushPatchPoint *BTGE_BrushPatch_GetPoint(
	BTGE_Brush *brush, int x, int y)
{
	BTGE_BrushPatchPoint *tmp;

	if(x<0)x=0;
	if(y<0)y=0;

	if(x>=brush->patch->nx_pts)
		x=brush->patch->nx_pts-1;
	if(y>=brush->patch->ny_pts)
		y=brush->patch->ny_pts-1;
		
	tmp=brush->patch->pts[y][x];
	return(tmp);
}

void BTGE_BrushPatch_InterpolateVecXYZ(
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

void BTGE_BrushPatch_Interpolate5VecXYZ(
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
	
	BTGE_BrushPatch_InterpolateVecXYZ(
		xyz0, uv0, xyz1, uv1, xyz2, uv2,
		tv0, tw0, 0.5);
	BTGE_BrushPatch_InterpolateVecXYZ(
		xyz2, uv2, xyz3, uv3, xyz4, uv4,
		tv1, tw1, 0.5);
	BTGE_BrushPatch_InterpolateVecXYZ(
		tv0, tw0, xyz2, uv2, tv1, tw1,
		xyz, uv, t);
}

void BTGE_BrushPatch_InterpolateVecPts(
	BTGE_BrushPatchPoint *pt0,
	BTGE_BrushPatchPoint *pt1,
	BTGE_BrushPatchPoint *pt2,
	BTGE_BrushPatchPoint *pt,
	float t)
{
	BTGE_BrushPatch_InterpolateVecXYZ(
		pt0->xyz, pt0->uv,
		pt1->xyz, pt1->uv,
		pt2->xyz, pt2->uv,
		pt->xyz, pt->uv, t);
}

void BTGE_BrushPatch_Interpolate5VecPts(
	BTGE_BrushPatchPoint *pt0,
	BTGE_BrushPatchPoint *pt1,
	BTGE_BrushPatchPoint *pt2,
	BTGE_BrushPatchPoint *pt3,
	BTGE_BrushPatchPoint *pt4,
	BTGE_BrushPatchPoint *pt,
	float t)
{
	BTGE_BrushPatch_Interpolate5VecXYZ(
		pt0->xyz, pt0->uv,
		pt1->xyz, pt1->uv,
		pt2->xyz, pt2->uv,
		pt3->xyz, pt3->uv,
		pt4->xyz, pt4->uv,
		pt->xyz, pt->uv, t);
}

#if 1
void BTGE_BrushPatch_InterpolateVec(
	BTGE_Brush *brush, BTGE_BrushPatchPoint *vec, float xpos, float ypos)
{
	BTGE_BrushPatchPoint *pt00, *pt01, *pt02, *pt03, *pt04;
	BTGE_BrushPatchPoint *pt10, *pt11, *pt12, *pt13, *pt14;
	BTGE_BrushPatchPoint *pt20, *pt21, *pt22, *pt23, *pt24;
	BTGE_BrushPatchPoint *pt30, *pt31, *pt32, *pt33, *pt34;
	BTGE_BrushPatchPoint *pt40, *pt41, *pt42, *pt43, *pt44;

	BTGE_BrushPatchPoint tpt0, tpt1, tpt2, tpt3, tpt4;

	float xf, yf;
	int x, y;
	
//	x=(int)floor(xpos-0.5); y=(int)floor(ypos-0.5);
	x=(int)floor(xpos+0.5); y=(int)floor(ypos+0.5);
//	x=(int)floor(xpos); y=(int)floor(ypos);
	
	pt00=BTGE_BrushPatch_GetPoint(brush, x-2, y-2);
	pt01=BTGE_BrushPatch_GetPoint(brush, x-1, y-2);
	pt02=BTGE_BrushPatch_GetPoint(brush, x+0, y-2);
	pt03=BTGE_BrushPatch_GetPoint(brush, x+1, y-2);
	pt04=BTGE_BrushPatch_GetPoint(brush, x+2, y-2);

	pt10=BTGE_BrushPatch_GetPoint(brush, x-2, y-1);
	pt11=BTGE_BrushPatch_GetPoint(brush, x-1, y-1);
	pt12=BTGE_BrushPatch_GetPoint(brush, x+0, y-1);
	pt13=BTGE_BrushPatch_GetPoint(brush, x+1, y-1);
	pt14=BTGE_BrushPatch_GetPoint(brush, x+2, y-1);

	pt20=BTGE_BrushPatch_GetPoint(brush, x-2, y+0);
	pt21=BTGE_BrushPatch_GetPoint(brush, x-1, y+0);
	pt22=BTGE_BrushPatch_GetPoint(brush, x+0, y+0);
	pt23=BTGE_BrushPatch_GetPoint(brush, x+1, y+0);
	pt24=BTGE_BrushPatch_GetPoint(brush, x+2, y+0);

	pt30=BTGE_BrushPatch_GetPoint(brush, x-2, y+1);
	pt31=BTGE_BrushPatch_GetPoint(brush, x-1, y+1);
	pt32=BTGE_BrushPatch_GetPoint(brush, x+0, y+1);
	pt33=BTGE_BrushPatch_GetPoint(brush, x+1, y+1);
	pt34=BTGE_BrushPatch_GetPoint(brush, x+2, y+1);

	pt40=BTGE_BrushPatch_GetPoint(brush, x-2, y+2);
	pt41=BTGE_BrushPatch_GetPoint(brush, x-1, y+2);
	pt42=BTGE_BrushPatch_GetPoint(brush, x+0, y+2);
	pt43=BTGE_BrushPatch_GetPoint(brush, x+1, y+2);
	pt44=BTGE_BrushPatch_GetPoint(brush, x+2, y+2);

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
	
	BTGE_BrushPatch_Interpolate5VecPts(
		pt00, pt01, pt02, pt03, pt04, &tpt0, xf);
	BTGE_BrushPatch_Interpolate5VecPts(
		pt10, pt11, pt12, pt13, pt14, &tpt1, xf);
	BTGE_BrushPatch_Interpolate5VecPts(
		pt20, pt21, pt22, pt23, pt24, &tpt2, xf);
	BTGE_BrushPatch_Interpolate5VecPts(
		pt30, pt31, pt32, pt33, pt34, &tpt3, xf);
	BTGE_BrushPatch_Interpolate5VecPts(
		pt40, pt41, pt42, pt43, pt44, &tpt4, xf);

	BTGE_BrushPatch_Interpolate5VecPts(
		&tpt0, &tpt1, &tpt2, &tpt3, &tpt4, vec, yf);
}
#endif

void BTGE_BrushPatch_BuildFaces(BTGE_Brush *brush)
{
	char tb[256];
	BTGE_BrushPatchPoint *p0, *p1, *p2, *p3;
	float *xyz;
	float xsc, ysc;
	int tn, xs, ys;
	int i, j;

	brush->flags&=~(BTGE_BRFL_ALPHA|BTGE_TXFL_FLUID_MASK|
		BTGE_TXFL_EFFECT_MASK|BTGE_BRFL_NOSHADOW);

	sprintf(tb, "textures/%s", brush->patch->tex);
	tn=BTGE_Texture_LoadImageSize(tb, &xs, &ys);
	brush->patch->texnum=tn;
	i=BTGE_Texture_GetImageFlags(tn);
	brush->patch->texfl=i;

	if(i&BTGE_TXFL_ALPHA)brush->flags|=BTGE_BRFL_ALPHA;
	if(i&BTGE_TXFL_VALPHA)brush->flags|=BTGE_BRFL_ALPHA;
	if(i&BTGE_TXFL_WINDOW)brush->flags|=BTGE_BRFL_ALPHA;
//	if(i&BTGE_TXFL_NODRAW)brush->flags|=BTGE_BRFL_ALPHA;

	if(i&BTGE_TXFL_NOSHADOW)brush->flags|=BTGE_BRFL_NOSHADOW;

	if(i&BTGE_TXFL_FLUID)
	{
		brush->flags|=BTGE_BRFL_ALPHA|BTGE_BRFL_FLUID;
		brush->flags|=i&BTGE_TXFL_FLUID_MASK;
	}

	if(i&BTGE_TXFL_EFFECT)
	{
		brush->flags|=i&BTGE_TXFL_EFFECT_MASK;
	}

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
		BTGE_BrushPatch_CalcNormalPts(p0, p1, p2, p0->norm);
		p0->norm[3]=V3F_DOT(p0->xyz, p0->norm);
	}
#endif

#if 0
	if(brush->patch->vec)
	{
		gcfree(brush->patch->vec);
		brush->patch->vec=NULL;
	}

	xs=brush->patch->nx_pts*2;
	ys=brush->patch->ny_pts*2;

	brush->patch->vec=gcatomic(xs*ys*sizeof(BTGE_BrushPatchPoint));
	brush->patch->vec_xs=xs;
	brush->patch->vec_ys=ys;
	
	xsc=(brush->patch->nx_pts)/((float)(xs-2));
	ysc=(brush->patch->ny_pts)/((float)(ys-2));
//	xsc=(brush->patch->nx_pts)/((float)xs);
//	ysc=(brush->patch->ny_pts)/((float)ys);
	for(i=0; i<ys; i++)
		for(j=0; j<xs; j++)
	{
		BTGE_BrushPatch_InterpolateVec(brush,
			brush->patch->vec+i*xs+j, j*xsc, i*ysc);
	}

	for(i=0; i<(ys-1); i++)
		for(j=0; j<(xs-1); j++)
	{
		p0=brush->patch->vec+((i+0)*xs+(j+0));
		p1=brush->patch->vec+((i+0)*xs+(j+1));
		p2=brush->patch->vec+((i+1)*xs+(j+0));
		p3=brush->patch->vec+((i+1)*xs+(j+1));
		BTGE_BrushPatch_CalcNormalPts(p0, p1, p2, p0->norm);
		p0->norm[3]=V3F_DOT(p0->xyz, p0->norm);
	}

#endif
}

void BTGE_BrushPatch_CalcNormalPts(
	BTGE_BrushPatchPoint *pt0,
	BTGE_BrushPatchPoint *pt1,
	BTGE_BrushPatchPoint *pt2,
	float *norm)
{
	float tv0[3], tv1[3], tv2[3];

	Vec3F_Subtract(pt1->xyz, pt0->xyz, tv0);
	Vec3F_Subtract(pt2->xyz, pt0->xyz, tv1);
//	V3F_CROSS(tv0, tv1, tv2);
	V3F_CROSS(tv1, tv0, tv2);
	V3F_NORMALIZE(tv2, norm);
}

BTGE_API int BTGE_BrushPatch_CheckPointBrush(BTGE_Brush *brush, float *pt)
{
	return(0);
}

BTGE_API int BTGE_BrushPatch_CheckLineBrushClip(BTGE_Brush *brush,
	float *sv, float *ev, float *csv, float *cev)
{
	if(csv) { V3F_COPY(sv, csv); }
	if(cev) { V3F_COPY(ev, cev); }
	
	return(0);
}

BTGE_API int BTGE_BrushPatch_CheckBoxBrushSeg(BTGE_Brush *brush,
	BTGE_BrushPatchPoint *p0, BTGE_BrushPatchPoint *p1,
	float *norm,
	float *mins, float *maxs)
{
	float tv0[3], tv1[3], tn[4];
	float f, g;
	
	V3F_SUB(p1->xyz, p0->xyz, tv0);
	V3F_CROSS(tv0, norm, tv1);
	
	V3F_NORMALIZE(tv1, tn);
	tn[3]=V3F_DOT(p0->xyz, tn);

	Hull_BoxPlaneExtents(mins, maxs, tn, &f, &g);
	
	if(f>=0)return(0);
//	if(g<=0)return(0);

	return(1);
}

BTGE_API int BTGE_BrushPatch_CheckBoxBrushFace(BTGE_Brush *brush,
	BTGE_BrushPatchPoint *p0, BTGE_BrushPatchPoint *p1,
	BTGE_BrushPatchPoint *p2, BTGE_BrushPatchPoint *p3,
	float *mins, float *maxs)
{
	float f, g;

	//does it cross face plane?
	Hull_BoxPlaneExtents(mins, maxs, p0->norm, &f, &g);
	if(f>=0)return(0);
	if(g<=0)return(0);

	//now check against edges
	if(!BTGE_BrushPatch_CheckBoxBrushSeg(
		brush, p0, p2, p0->norm, mins, maxs))
			return(0);
	if(!BTGE_BrushPatch_CheckBoxBrushSeg(
		brush, p2, p3, p0->norm, mins, maxs))
			return(0);
	if(!BTGE_BrushPatch_CheckBoxBrushSeg(
		brush, p3, p1, p0->norm, mins, maxs))
			return(0);
	if(!BTGE_BrushPatch_CheckBoxBrushSeg(
		brush, p1, p0, p0->norm, mins, maxs))
			return(0);
			
	return(1);
}

BTGE_API int BTGE_BrushPatch_CheckSphereBrushSeg(BTGE_Brush *brush,
	BTGE_BrushPatchPoint *p0, BTGE_BrushPatchPoint *p1,
	float *norm,
	float *org, float rad)
{
	float tv0[3], tv1[3], tn[4];
	float f, g;
	
	V3F_SUB(p1->xyz, p0->xyz, tv0);
	V3F_CROSS(tv0, norm, tv1);
	
	V3F_NORMALIZE(tv1, tn);
	tn[3]=V3F_DOT(p0->xyz, tn);

	Hull_SpherePlaneExtents(org, rad, tn, &f, &g);
	
	if(f>=0)return(0);
//	if(g<=0)return(0);

	return(1);
}

BTGE_API int BTGE_BrushPatch_CheckSphereBrushFace(BTGE_Brush *brush,
	BTGE_BrushPatchPoint *p0, BTGE_BrushPatchPoint *p1,
	BTGE_BrushPatchPoint *p2, BTGE_BrushPatchPoint *p3,
	float *org, float rad)
{
	float f, g;

	//does it cross face plane?
	Hull_SpherePlaneExtents(org, rad, p0->norm, &f, &g);
	if(f>=0)return(0);
	if(g<=0)return(0);

	//now check against edges
	if(!BTGE_BrushPatch_CheckSphereBrushSeg(
		brush, p0, p2, p0->norm, org, rad))
			return(0);
	if(!BTGE_BrushPatch_CheckSphereBrushSeg(
		brush, p2, p3, p0->norm, org, rad))
			return(0);
	if(!BTGE_BrushPatch_CheckSphereBrushSeg(
		brush, p3, p1, p0->norm, org, rad))
			return(0);
	if(!BTGE_BrushPatch_CheckSphereBrushSeg(
		brush, p1, p0, p0->norm, org, rad))
			return(0);
			
	return(1);
}

BTGE_API int BTGE_BrushPatch_CheckBoxBrush(BTGE_Brush *brush,
	float *mins, float *maxs)
{
	BTGE_BrushPatchPoint *p0, *p1, *p2, *p3;
	int xs, ys;
	int i, j;

	//check if bbox may collide
	if(!SolidAABB_BoxCollideP(mins, maxs, brush->mins, brush->maxs))
		return(0);

	//ok, now check patch faces
	xs=brush->patch->nx_pts;
	ys=brush->patch->ny_pts;
	for(i=0; i<ys-1; i++)
	{
		for(j=0; j<xs-1; j++)
		{
			p0=brush->patch->pts[i+0][j+0];
			p1=brush->patch->pts[i+0][j+1];
			p2=brush->patch->pts[i+1][j+0];
			p3=brush->patch->pts[i+1][j+1];

			if(!BTGE_BrushPatch_CheckBoxBrushFace(
					brush, p0, p1, p2, p3, mins, maxs))
				continue;

			//got something
//			printf("BTGE_BrushPatch_CheckBoxBrush: HIT\n");
			return(1);
		}
	}

	return(0);
}

BTGE_API int BTGE_BrushPatch_CheckSphereBrush(BTGE_Brush *brush,
	float *org, float rad)
{
	BTGE_BrushPatchPoint *p0, *p1, *p2, *p3;
	float f;
	int xs, ys;
	int i, j;
	
	f=V3F_DIST(org, brush->org);
	if(f>=(brush->rad+rad))
		return(0);

	//ok, now check patch faces
	xs=brush->patch->nx_pts;
	ys=brush->patch->ny_pts;
	for(i=0; i<ys-1; i++)
	{
		for(j=0; j<xs-1; j++)
		{
			p0=brush->patch->pts[i+0][j+0];
			p1=brush->patch->pts[i+0][j+1];
			p2=brush->patch->pts[i+1][j+0];
			p3=brush->patch->pts[i+1][j+1];

			if(!BTGE_BrushPatch_CheckSphereBrushFace(
					brush, p0, p1, p2, p3, org, rad))
				continue;

			//got something
//			printf("BTGE_BrushPatch_CheckSphereBrush: HIT\n");
			return(1);
		}
	}

	return(0);

//	return(1);
}

BTGE_API int BTGE_BrushPatch_GetExtents(BTGE_Brush *brush, float *vec,
	float *rm, float *rn)
{
	BTGE_Brush_GetBBoxExtents(brush, vec, rm, rn);
	return(0);
}

BTGE_API int BTGE_BrushPatch_GetPlaneExtents(BTGE_Brush *brush, float *vec,
	float *rm, float *rn)
{
	BTGE_Brush_GetBBoxPlaneExtents(brush, vec, rm, rn);
	return(0);
}

#if 0
// BTGE_API void BTGE_BrushPatch_DrawBrushDark(BTGE_Brush *brush)
{
	BTGE_BrushPatchPoint *p0, *p1, *p2, *p3;
	int xs, ys;
	int i, j;

	if(brush->patch->texfl&(BTGE_TXFL_NODRAW|BTGE_TXFL_ALPHA))
		return;

//	if(!BTGE_Brush_CheckBrushFrustum(brush))return;
	if(!BTGE_Brush_CheckBrushFrustumVisible2(brush))return;

	glDisable(GL_TEXTURE_2D);
	glColor4f(0, 0, 0, 1);

	glBegin(GL_QUADS);
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

//		BTGE_Shader_Normal3fv(p0->norm);

		glVertex3fv(p0->xyz);
		glVertex3fv(p2->xyz);
		glVertex3fv(p3->xyz);
		glVertex3fv(p1->xyz);
	}
	glEnd();
}

// BTGE_API void BTGE_BrushPatch_DrawSolidFlat(BTGE_Brush *brush)
{
	int i, j;

	if(!BTGE_Brush_CheckBrushFrustum(brush))return;

	glBegin(GL_QUADS);
	for(i=0; i<brush->patch->ny_pts-1; i++)
		for(j=0; j<brush->patch->nx_pts-1; j++)
	{
		glVertex3fv(brush->patch->pts[i+0][j+0]->xyz);
		glVertex3fv(brush->patch->pts[i+1][j+0]->xyz);
		glVertex3fv(brush->patch->pts[i+1][j+1]->xyz);
		glVertex3fv(brush->patch->pts[i+0][j+1]->xyz);
	}
	glEnd();
}
#endif
