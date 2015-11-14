#include <lbxgl2.h>

extern PDGLUI_InputState *lbxgl_state;
extern PDGLUI_Camera *lbxgl_cam;
extern LBXGL_BrushWorld *lbxgl_world;

LBXGL_API int LBXGL_Brush_CheckSphereFrustum(float *org, float rad)
{
	float f, g;

	f=V3F_DOT(org, lbxgl_cam->fw)-V3F_DOT(lbxgl_cam->org, lbxgl_cam->fw);
	if(f<=(-rad))return(0);

	g=f+rad;
	f=(V3F_DOT(org, lbxgl_cam->rt)-V3F_DOT(lbxgl_cam->org, lbxgl_cam->rt))/g;
	if(f<=(-rad-1))return(0);
	if(f>=(rad+1))return(0);

	f=(V3F_DOT(org, lbxgl_cam->up)-V3F_DOT(lbxgl_cam->org, lbxgl_cam->up))/g;
	if(f<=(-rad-1))return(0);
	if(f>=(rad+1))return(0);

	return(1);
}

LBXGL_API int LBXGL_Brush_CheckBBoxFrustum(float *mins, float *maxs)
{
	float f, g;

	PlaneF_BoxPlaneExtents(mins, maxs, lbxgl_cam->clip_bk, &f, &g);
	if(f>=0)return(0);
	PlaneF_BoxPlaneExtents(mins, maxs, lbxgl_cam->clip_lf, &f, &g);
	if(f>=0)return(0);
	PlaneF_BoxPlaneExtents(mins, maxs, lbxgl_cam->clip_rt, &f, &g);
	if(f>=0)return(0);
	PlaneF_BoxPlaneExtents(mins, maxs, lbxgl_cam->clip_up, &f, &g);
	if(f>=0)return(0);
	PlaneF_BoxPlaneExtents(mins, maxs, lbxgl_cam->clip_dn, &f, &g);
	if(f>=0)return(0);

	return(1);
}

/** Determine if bbox is both within the frustum and overlaps
  * with the visible scene.
  */
LBXGL_API int LBXGL_Brush_CheckBBoxFrustumVisible(
	LBXGL_BrushWorld *world, float *mins, float *maxs)
{
	if(!SolidAABB_BoxCollideP(mins, maxs,
			world->autovis_mins, world->autovis_maxs))
		return(0);
	if(!LBXGL_Brush_CheckBBoxFrustum(mins, maxs))
		return(0);

	if(!LBXGL_Brush_CheckBBoxAntiportal(
		world, mins, maxs, lbxgl_cam->org))
			return(0);

	return(1);
}

/** Determine if sphere is both within the frustum and overlaps
  * with the visible scene.
  */
LBXGL_API int LBXGL_Brush_CheckSphereFrustumVisible(
	LBXGL_BrushWorld *world, float *org, float rad)
{
	float mins[3], maxs[3];

	if(!LBXGL_Brush_CheckSphereFrustum(org, rad))
		return(0);

	mins[0]=org[0]-rad;
	mins[1]=org[1]-rad;
	mins[2]=org[2]-rad;

	maxs[0]=org[0]+rad;
	maxs[1]=org[1]+rad;
	maxs[2]=org[2]+rad;

	if(!SolidAABB_BoxCollideP(mins, maxs,
			world->autovis_mins, world->autovis_maxs))
		return(0);
	if(!LBXGL_Brush_CheckBBoxFrustum(mins, maxs))
		return(0);

	if(!LBXGL_Brush_CheckBBoxAntiportal(
		world, mins, maxs, lbxgl_cam->org))
			return(0);

	return(1);
}

LBXGL_API int LBXGL_Brush_CheckBBoxFrustumDistance(
	float *mins, float *maxs, float dist)
{
	float org[3];
	float f, g;

	if(!LBXGL_Brush_CheckBBoxFrustum(mins, maxs))
		return(false);

	V3F_SCALEADDSCALE(mins, 0.5, maxs, 0.5, org);
	f=V3F_DIST(org, lbxgl_cam->org);
	g=V3F_DIST(maxs, org);
//	if(f>8192)
	if(f>(dist-g))
		return(0);

	return(1);
}

LBXGL_API int LBXGL_Brush_CheckBBoxFrustumDistanceVisible(
	LBXGL_BrushWorld *world, float *mins, float *maxs, float dist)
{
	float org[3];
	float f, g;

	if(!SolidAABB_BoxCollideP(mins, maxs,
			world->autovis_mins, world->autovis_maxs))
		return(0);
	if(!LBXGL_Brush_CheckBBoxFrustum(mins, maxs))
		return(false);

	V3F_SCALEADDSCALE(mins, 0.5, maxs, 0.5, org);
	f=V3F_DIST(org, lbxgl_cam->org);
	g=V3F_DIST(maxs, org);
//	if(f>8192)
	if(f>(dist-g))
		return(0);

	return(1);
}

LBXGL_API float LBXGL_Brush_GetBBoxFrustumDistance(
	float *mins, float *maxs)
{
	float org[3];
	float f, g;

	V3F_SCALEADDSCALE(mins, 0.5, maxs, 0.5, org);
	f=V3F_DIST(org, lbxgl_cam->org);
	g=V3F_DIST(maxs, org);
	return(f-g);
}

LBXGL_API int LBXGL_Brush_CheckBrushFrustum(LBXGL_Brush *brush)
{
	float f, g, h;

#if 0
	LBXGL_Brush_GetPlaneExtents(brush, lbxgl_cam->clip_bk, &f, &g);
	if(f>=0)return(0);

	LBXGL_Brush_GetPlaneExtents(brush, lbxgl_cam->clip_lf, &f, &g);
	if(f>=0)return(0);

	LBXGL_Brush_GetPlaneExtents(brush, lbxgl_cam->clip_rt, &f, &g);
	if(f>=0)return(0);

	LBXGL_Brush_GetPlaneExtents(brush, lbxgl_cam->clip_up, &f, &g);
	if(f>=0)return(0);

	LBXGL_Brush_GetPlaneExtents(brush, lbxgl_cam->clip_dn, &f, &g);
	if(f>=0)return(0);
#endif

#if 1
	LBXGL_Brush_GetBBoxPlaneExtents(brush, lbxgl_cam->clip_bk, &f, &g);
	if(f>=0)return(0);

	LBXGL_Brush_GetBBoxPlaneExtents(brush, lbxgl_cam->clip_lf, &f, &g);
	if(f>=0)return(0);

	LBXGL_Brush_GetBBoxPlaneExtents(brush, lbxgl_cam->clip_rt, &f, &g);
	if(f>=0)return(0);

	LBXGL_Brush_GetBBoxPlaneExtents(brush, lbxgl_cam->clip_up, &f, &g);
	if(f>=0)return(0);

	LBXGL_Brush_GetBBoxPlaneExtents(brush, lbxgl_cam->clip_dn, &f, &g);
	if(f>=0)return(0);
#endif

	return(1);
}

LBXGL_API int LBXGL_Brush_CheckBrushFrustumVisible(
	LBXGL_BrushWorld *world, LBXGL_Brush *brush)
{
	if(!SolidAABB_BoxCollideP(brush->mins, brush->maxs,
			world->autovis_mins, world->autovis_maxs))
		return(0);
	if(!LBXGL_Brush_CheckBrushFrustum(brush))
		return(0);
	return(1);
}

LBXGL_API int LBXGL_Brush_CheckBrushFrustumVisible2(
	LBXGL_Brush *brush)
{
	if(lbxgl_world)
	{
		if(!SolidAABB_BoxCollideP(brush->mins, brush->maxs,
			lbxgl_world->autovis_mins, lbxgl_world->autovis_maxs))
				return(0);
	}
	if(!LBXGL_Brush_CheckBrushFrustum(brush))
		return(0);
	return(1);
}

LBXGL_API int LBXGL_Brush_CheckBrushMarkedVisible(
	LBXGL_BrushWorld *world, LBXGL_Brush *brush)
{
	int i;

	if(brush->flags&LBXGL_BRFL_VISCLIP)
		return(0);
	if(brush->node)
	{
		i=brush->node->id;
		i=world->autovis[i>>3]&(1<<(i&7));
		if(!i)return(0);
	}
	return(1);
}

LBXGL_API int LBXGL_Brush_CheckBrushShadowFrustum(LBXGL_Brush *brush, float *org)
{
	float f, g, h;

#if 0
	f=V3F_DOT(brush->org, lbxgl_cam->fw)-V3F_DOT(lbxgl_cam->org, lbxgl_cam->fw);
	g=V3F_DOT(org, lbxgl_cam->fw)-V3F_DOT(lbxgl_cam->org, lbxgl_cam->fw);
	if((f<=g) && (f<=(-brush->rad)))return(0);

	h=f+brush->rad;
	f=V3F_DOT(brush->org, lbxgl_cam->rt)-V3F_DOT(lbxgl_cam->org, lbxgl_cam->rt);
	g=V3F_DOT(org, lbxgl_cam->rt)-V3F_DOT(lbxgl_cam->org, lbxgl_cam->rt);

	f/=h; g/=h;
	if((f<=g) && (f<=(-brush->rad-1)))return(0);
	if((f>=g) && (f>=(brush->rad+1)))return(0);
#endif

#if 0
	LBXGL_Brush_GetPlaneExtents(brush, lbxgl_cam->clip_bk, &f, &g);
	h=V3F_NDOT(org, lbxgl_cam->clip_bk);
	if((f>=h) && (f>=0))return(0);

	LBXGL_Brush_GetPlaneExtents(brush, lbxgl_cam->clip_lf, &f, &g);
	h=V3F_NDOT(org, lbxgl_cam->clip_lf);
	if((f>=h) && (f>=0))return(0);

	LBXGL_Brush_GetPlaneExtents(brush, lbxgl_cam->clip_rt, &f, &g);
	h=V3F_NDOT(org, lbxgl_cam->clip_rt);
	if((f>=h) && (f>=0))return(0);

	LBXGL_Brush_GetPlaneExtents(brush, lbxgl_cam->clip_up, &f, &g);
	h=V3F_NDOT(org, lbxgl_cam->clip_up);
	if((f>=h) && (f>=0))return(0);

	LBXGL_Brush_GetPlaneExtents(brush, lbxgl_cam->clip_dn, &f, &g);
	h=V3F_NDOT(org, lbxgl_cam->clip_dn);
	if((f>=h) && (f>=0))return(0);
#endif

#if 1
	LBXGL_Brush_GetBBoxPlaneExtents(brush, lbxgl_cam->clip_bk, &f, &g);
	h=V3F_NDOT(org, lbxgl_cam->clip_bk);
	if((f>=h) && (f>=0))return(0);

	LBXGL_Brush_GetBBoxPlaneExtents(brush, lbxgl_cam->clip_lf, &f, &g);
	h=V3F_NDOT(org, lbxgl_cam->clip_lf);
	if((f>=h) && (f>=0))return(0);

	LBXGL_Brush_GetBBoxPlaneExtents(brush, lbxgl_cam->clip_rt, &f, &g);
	h=V3F_NDOT(org, lbxgl_cam->clip_rt);
	if((f>=h) && (f>=0))return(0);

	LBXGL_Brush_GetBBoxPlaneExtents(brush, lbxgl_cam->clip_up, &f, &g);
	h=V3F_NDOT(org, lbxgl_cam->clip_up);
	if((f>=h) && (f>=0))return(0);

	LBXGL_Brush_GetBBoxPlaneExtents(brush, lbxgl_cam->clip_dn, &f, &g);
	h=V3F_NDOT(org, lbxgl_cam->clip_dn);
	if((f>=h) && (f>=0))return(0);
#endif

	return(1);
}

#if 0
LBXGL_API int LBXGL_Brush_CheckPointBrushShadow(
	LBXGL_Brush *brush, float *lorg, float lrad, float *pt)
{

	return(LBXGL_Brush_CheckBBoxBrushVolume(brush, pt, pt, lorg));

#if 0
	float dir[3];
	float f, g, d, sc;
	
	V3F_SUB(brush->org, lorg, dir);
	d=V3F_NORMALIZE(dir, dir);
	if(d<brush->rad)
		return(-1);
	
	f=V3F_DOT(pt, dir)-V3F_DOT(lorg, dir);
	if(f<(d-brush->rad))
		return(0);
	if(f>(lrad+brush->rad))
		return(0);
		
	g=brush->rad*(f/d);
#endif
}
#endif

LBXGL_API int LBXGL_Brush_CheckSphereShadowFrustum(
	float *sorg, float srad, float *lorg, float lrad)
{
	float vorg[3], vfw[3], vrt[3], sdir[3], tv0[4];
	float vrad;
	float f, g, h;

//	LBXGL_Shader_TransformPointLocal(lbxgl_cam->org, vorg);
//	LBXGL_Shader_TransformVectorLocal(lbxgl_cam->fw, vfw);
//	LBXGL_Shader_TransformVectorLocal(lbxgl_cam->rt, vrt);

	V3F_COPY(lbxgl_cam->org, vorg);
	V3F_COPY(lbxgl_cam->fw, vfw);
	V3F_COPY(lbxgl_cam->rt, vrt);

	vrad=16;


	f=V3F_DOT(sorg, vfw)-V3F_DOT(vorg, vfw);
	g=V3F_DOT(lorg, vfw)-V3F_DOT(vorg, vfw);
	
	//shadow points backwards and is behind the camera
	if((f<=g) && (f<=(0-(srad+vrad))))
		return(0);

	h=f+srad;
	f=V3F_DOT(sorg, vrt)-V3F_DOT(vorg, vrt);
	g=V3F_DOT(lorg, vrt)-V3F_DOT(vorg, vrt);

	f/=h; g/=h;
	if((f<=g) && (f<=(-srad-1)))return(0);
	if((f>=g) && (f>=(srad+1)))return(0);
	
	return(1);
}

LBXGL_API int LBXGL_Brush_CheckSphereShadowFrustumVisible(
	LBXGL_BrushWorld *world,
	float *sorg, float srad, float *lorg, float lrad)
{
	float vorg[3], dir[3], cpa[3];
	float vrad, sd, vd, d;
	float f, g;

	if(!LBXGL_Brush_CheckSphereShadowFrustum(sorg, srad, lorg, lrad))
		return(0);
	
	//bounding sphere for visible scene
	V3F_SCALEADDSCALE(
		world->autovis_mins, 0.5,
		world->autovis_maxs, 0.5, vorg);
	vrad=V3F_DIST(world->autovis_maxs, vorg);

	V3F_SUB(sorg, lorg, dir);
	sd=V3F_NORMALIZE(dir, dir);
//	if(sd>lrad)return(0);
	
	vd=V3F_DOT(vorg, dir)-V3F_DOT(lorg, dir);
	V3F_ADDSCALE(lorg, dir, vd, cpa);
	d=V3F_DIST(vorg, cpa);

	f=srad*(vd/sd);
	if(d>=(vrad+f))
		return(0);

	return(1);
}

LBXGL_API int LBXGL_Brush_CheckBBoxShadowFrustumVisible(
	LBXGL_BrushWorld *world, 
	float *mins, float *maxs, float *lorg, float lrad)
{
	float org[3], size[3], dir[3];
	float org1[3], mins1[3], maxs1[3];
	float mins2[3], maxs2[3];
	float rad, sd, sc;
	int i;
	
	V3F_SCALEADDSCALE(mins, 0.5, maxs, 0.5, org);
	V3F_SUB(maxs, org, size);
	rad=V3F_LEN(size);
	
	V3F_SUB(org, lorg, dir);
	sd=V3F_NORMALIZE(dir, dir);
	if(sd>lrad)return(0);
	
	sc=lrad/sd;	//box scale
//	sc=rad/sd;	//box scale
	
	//project the bounding-box
	V3F_ADDSCALE(lorg, dir, lrad, org1);
	V3F_ADDSCALE(org1, size, -sc, mins1);
	V3F_ADDSCALE(org1, size,  sc, maxs1);
	
	V3F_MIN(mins, mins1, mins2);
	V3F_MAX(maxs, maxs1, maxs2);
	
	i=LBXGL_Brush_CheckBBoxFrustumVisible(world, mins2, maxs2);
	if(!i)return(0);
	
//	i=LBXGL_Brush_CheckSphereShadowFrustum(org, rad, lorg, lrad);
	i=LBXGL_Brush_CheckSphereShadowFrustumVisible(
		world, org, rad, lorg, lrad);
	if(!i)return(0);

	return(1);
}

LBXGL_API int LBXGL_Brush_CheckBoxBrushShadow(
	LBXGL_Brush *brush, float *mins, float *maxs,
	float *lorg, float lrad)
{
	int i;

//	return(1);

	i=LBXGL_Brush_CheckBBoxBrushVolume(brush, mins, maxs, lorg);
	if(i&1)return(1);
	return(0);
}

LBXGL_API int LBXGL_Brush_CheckViewBrushShadow(
	LBXGL_Brush *brush, float *lorg, float lrad)
{
	float vorg[3], vmins[3], vmaxs[3];

	LBXGL_Shader_TransformPointLocal(lbxgl_cam->org, vorg);

	vmins[0]=vorg[0]-16;
	vmins[1]=vorg[1]-16;
	vmins[2]=vorg[2]-16;

	vmaxs[0]=vorg[0]+16;
	vmaxs[1]=vorg[1]+16;
	vmaxs[2]=vorg[2]+16;
	
	return(LBXGL_Brush_CheckBoxBrushShadow(
		brush, vmins, vmaxs, lorg, lrad));
}

LBXGL_API int LBXGL_Brush_CheckViewSphereShadow(
	float *sorg, float srad, float *lorg, float lrad)
{
	return(LBXGL_Brush_CheckViewSphereShadowFlags(
		sorg, srad, lorg, lrad, LBXGL_SFL_INFINITE));
}

LBXGL_API int LBXGL_Brush_CheckViewSphereShadowFlags(
	float *sorg, float srad, float *lorg, float lrad, int flags)
{
	float vorg[3], sdir[3], tv0[4];
	float f, g, d, sd, vrad;

	LBXGL_Shader_TransformPointLocal(lbxgl_cam->org, vorg);
	vrad=16;

	V3F_SUB(sorg, lorg, sdir);
	sd=V3F_NORMALIZE(sdir, sdir);

	if(lrad>0)
	{
		//cull if no effect
//		f=V3F_DOT(sorg, sdir)-V3F_DOT(lorg, sdir);
//		if(f>(lrad+srad))return(0);

		if(sd>(lrad+srad))return(0);

		if(!(flags&LBXGL_SFL_INFINITE))
		{
			//cull if view out of range
			f=V3F_DOT(vorg, sdir)-V3F_DOT(lorg, sdir);
			if(f>(lrad+vrad))return(0);
		}
	}

#if 0
	f=V3F_DOT(vorg, sdir)-V3F_DOT(lorg, sdir);
	if(f>(lr+vr)) return(0);

	if(f<sd)
	{
		d=V3F_DIST(vorg, sorg);
		if(d>(srad+vr)) { cur=nxt; continue; }
	}else
	{
		g=(f/sd)*srad;
		V3F_ADDSCALE(lorg, sdir, f, tv0);
		d=V3F_DIST(vorg, tv0);
		if(d>(g+vr)) { cur=nxt; continue; }
	}
#endif

	//check view (as sphere), against cone
	f=V3F_DOT(vorg, sdir)-V3F_DOT(lorg, sdir);
	if(f>=sd)
	{
		g=(f/sd)*srad;
		V3F_ADDSCALE(lorg, sdir, f, tv0);
		d=V3F_DIST(vorg, tv0);
		if(d<=(g+vrad))return(1);
	}else
	{
		d=V3F_DIST(vorg, sorg);
		if(d<=(srad+vrad))return(1);
	}

	return(0);
}
