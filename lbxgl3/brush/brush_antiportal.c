#include <lbxgl2.h>

extern PDGLUI_Camera *lbxgl_cam;

//Check if BBox is inside of the projected volume.
//1&=inside, 2&=outside
LBXGL_API int LBXGL_Brush_CheckBBoxBrushVolume(LBXGL_Brush *brush,
	float *mins, float *maxs, float *vorg)
{
	float tv0[4], tv1[4], tv2[4], tv3[4];
	float dv0[3], dv1[3];
	float *nv, *nv0, *nv1, *tv, *xyz;
	float f, g, h;
	int i, j, k, l, i0, i1;

	if(!brush->xyz)
	{
		if(brush->patch)
			return(0);
		LBXGL_Brush_BuildFaces(brush);
	}

	k=0;

#if 1
	for(i=0; i<brush->n_face; i++)
	{
		f=V3F_NDOT(vorg, brush->norm+i*4);
		if(f<=0)continue;

		Hull_BoxPlaneExtents(mins, maxs, brush->norm+i*4, &f, &g);
		if(f>=0)k|=1;	//not inside
		if(g>0)k|=2;	//outside
	}
#endif

	xyz=brush->xyz;
	for(i=0; i<brush->n_edge; i++)
	{
		i0=brush->edge[i*4+2]; i1=brush->edge[i*4+3];
		nv0=brush->norm+brush->pf_plane[i0]*4;
		nv1=brush->norm+brush->pf_plane[i1]*4;

		f=V3F_NDOT(vorg, nv0);
		g=V3F_NDOT(vorg, nv1);

		i0=-1; i1=-1;
		if((f>0) && (g<=0))
			{ i0=brush->edge[i*4+0]; i1=brush->edge[i*4+1]; }
		if((g>0) && (f<=0))
			{ i1=brush->edge[i*4+0]; i0=brush->edge[i*4+1]; }
		if((i0<0) || (i1<0))
			continue;

		V3F_SUB(xyz+i1*3, xyz+i0*3, tv0);
		V3F_SUB(xyz+i0*3, vorg, tv1);
		V3F_CROSS(tv1, tv0, tv2);
//		V3F_CROSS(tv0, tv1, tv2);
		V3F_NORMALIZE(tv2, tv2);
		tv2[3]=V3F_DOT(xyz+i0*3, tv2);

		Hull_BoxPlaneExtents(mins, maxs, tv2, &f, &g);
		if(f>=0)k|=1;	//not inside
		if(g>0)k|=2;	//outside
	}
	
	return(k^1);
}

LBXGL_API int LBXGL_Brush_CheckBrushBrushVolume(
	LBXGL_Brush *tbrush, LBXGL_Brush *vbrush, float *vorg)
{
	return(LBXGL_Brush_CheckBBoxBrushVolume(
		vbrush, tbrush->mins, tbrush->maxs, vorg));
}

LBXGL_API int LBXGL_Brush_ClipFaceBrushVolume(LBXGL_Brush *brush,
	float *vorg, float vrad, float *ipts, float *opts, int npts)
{
	float pts0[64*3];
	float pts1[64*3];

	float tv0[4], tv1[4], tv2[4], tv3[4];
	float dv0[3], dv1[3];
	float *nv, *nv0, *nv1, *tv, *xyz;
	float f, g, h;
	int i, j, k, l, i0, i1, n;

	if(!brush->xyz)
	{
		if(brush->patch)
			return(0);
		LBXGL_Brush_BuildFaces(brush);
	}

	n=npts;
	for(j=0; j<(n*3); j++)
		pts0[j]=ipts[j];

	for(i=0; i<brush->n_face; i++)
	{
		f=V3F_NDOT(vorg, brush->norm+i*4);
		if(f<=0)continue;

		n=Hull_ClipFace(brush->norm+i*4, pts0, pts1, n);
		for(j=0; j<(n*3); j++)pts0[j]=pts1[j];
	}

	if(!n)return(0);

	V3F_SUB(brush->org, vorg, dv0);
	V3F_NORMALIZE(dv0, dv0);

#if 0
	for(i=0; i<brush->n_face; i++)
	{
		f=V3F_NDOT(vorg, brush->norm+i*4);
		if(f>=0)continue;

//		g=V3F_NDOT(vorg, brush->norm+i*4);
		V4F_COPY(brush->norm+i*4, tv0);
		
		g=fabs(V3F_DOT(tv0, dv0));
		tv0[3]+=g*2*vrad;

		n=Hull_ClipFace(tv0, pts0, pts1, n);
		for(j=0; j<(n*3); j++)pts0[j]=pts1[j];
	}

	if(!n)return(0);
#endif

	xyz=brush->xyz; k=0;
	for(i=0; i<brush->n_edge; i++)
	{
		i0=brush->edge[i*4+2]; i1=brush->edge[i*4+3];
		nv0=brush->norm+brush->pf_plane[i0]*4;
		nv1=brush->norm+brush->pf_plane[i1]*4;

		f=V3F_NDOT(vorg, nv0);
		g=V3F_NDOT(vorg, nv1);

		i0=-1; i1=-1;
		if((f>0) && (g<=0))
			{ i0=brush->edge[i*4+0]; i1=brush->edge[i*4+1]; }
		if((g>0) && (f<=0))
			{ i1=brush->edge[i*4+0]; i0=brush->edge[i*4+1]; }
		if((i0<0) || (i1<0))
			continue;

		V3F_SUB(xyz+i1*3, xyz+i0*3, tv0);
		V3F_SUB(xyz+i0*3, vorg, tv1);
		V3F_CROSS(tv1, tv0, tv2);
//		V3F_CROSS(tv0, tv1, tv2);
		V3F_NORMALIZE(tv2, tv2);
		tv2[3]=V3F_DOT(xyz+i0*3, tv2);

		n=Hull_ClipFace(tv2, pts0, pts1, n);
		if(n<=0)break;
		for(j=0; j<(n*3); j++)
			pts0[j]=pts1[j];
	}

	if(!n)return(0);

	for(j=0; j<(n*3); j++)
		opts[j]=pts1[j];
	return(n);
}

LBXGL_API int LBXGL_Brush_ClipFacePlanes(float *clip, int nclip,
	float *ipts, float *opts, int npts)
{
	float pts0[64*3];
	float pts1[64*3];

	int i, j, n;

	n=npts;
	for(j=0; j<(n*3); j++)
		pts0[j]=ipts[j];

	for(i=0; i<nclip; i++)
	{
		n=Hull_ClipFace(clip+i*4, pts0, pts1, n);
		if(!n)break;
		for(j=0; j<(n*3); j++)pts0[j]=pts1[j];
	}

	if(!n)return(0);

	for(j=0; j<(n*3); j++)
		opts[j]=pts0[j];
	return(n);
}

/** Check if the bbox falls within an antiportal (is occluded).
  * Returns false if visibility would be blocked.
  */
int LBXGL_Brush_CheckBBoxAntiportal(
	LBXGL_BrushWorld *world, float *mins, float *maxs, float *vorg)
{
	LBXGL_Brush *cur;
	int i;
	
//	return(1);
	
	cur=world->brush_antiportal;
	while(cur)
	{
		i=LBXGL_Brush_CheckBBoxBrushVolume(cur, mins, maxs, vorg);
		if(i==1)return(0);
		cur=cur->knext;
	}
	return(1);
}

int LBXGL_Brush_CheckNodeAntiportal(
	LBXGL_BrushWorld *world, LBXGL_BrushNode *node, float *vorg)
{
	return(LBXGL_Brush_CheckBBoxAntiportal(
		world, node->mins, node->maxs, vorg));
}

/** Check if the line passes through an antiportal (is occluded).
  * Returns false if visibility would be blocked.
  */
int LBXGL_Brush_CheckLineAntiportal(
	LBXGL_BrushWorld *world, float *sv, float *ev)
{
	LBXGL_Brush *cur;
	int i;
	
//	return(1);

	cur=world->brush_antiportal;
	while(cur)
	{
//		i=LBXGL_Brush_CheckBBoxBrushVolume(cur, mins, maxs, vorg);
//		if(i==1)return(0);
		if(LBXGL_Brush_CheckLineBrush(cur, sv, ev))
			return(0);

		cur=cur->knext;
	}
	return(1);
}

/** Check if the bbox falls within a brush volume (is occluded).
  * Returns false if visibility would be blocked.
  */
int LBXGL_Brush_CheckBBoxBrushVolumeList(
	LBXGL_BrushWorld *world, LBXGL_Brush *lst,
	float *mins, float *maxs, float *vorg)
{
	LBXGL_Brush *cur;
	int i;
	
//	return(1);
	
	cur=lst;
	while(cur)
	{
		i=LBXGL_Brush_CheckBBoxBrushVolume(cur, mins, maxs, vorg);
		if(i==1)return(0);
		cur=cur->chain;
	}
	return(1);
}

/** Check if the bbox falls within a brush volume (is occluded).
  * Returns false if visibility would be blocked.
  */
int LBXGL_Brush_CheckBrushBrushVolumeList(
	LBXGL_BrushWorld *world, LBXGL_Brush *lst,
	LBXGL_Brush *brush, float *vorg)
{
	LBXGL_Brush *cur;
	int i;
	
//	return(1);
	
	cur=lst;
	while(cur)
	{
		if(cur==brush)
			{ cur=cur->chain; continue; }
		i=LBXGL_Brush_CheckBBoxBrushVolume(cur,
			brush->mins, brush->maxs, vorg);
		if(i==1)return(0);
		cur=cur->chain;
	}
	return(1);
}
