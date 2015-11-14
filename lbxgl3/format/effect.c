#include <lbxgl2.h>

typedef struct {
float start[3];
float end[3];
float radius;
float color[4];
}LBXGL_LaserEffect;

int lbxgl_lasereffect_draw(LBXGL_ModelState *ctx)
{
	LBXGL_LaserEffect *mdl;

	mdl=ctx->mdl;
	
	PD3D_DrawFlatCapsule(mdl->start, mdl->end, mdl->radius,
		mdl->color[0], mdl->color[1], mdl->color[2], mdl->color[3]);

//	glEnable(GL_TEXTURE_2D);
	pdglEnableTexture2D();
	pdglColor4f(1, 1, 1, 1);

	return(0);
}

LBXGL_API void *LBXGL_LaserEffect_Create(
	float *sv, float *ev, float *clr, float rad)
{
	LBXGL_LaserEffect *tmp;
	LBXGL_RenderMethods *mth;
	
	mth=LBXGL_Method_GetRenderMethods("lbxgl_lasereffect_t");
	mth->drawmdl_draw=lbxgl_lasereffect_draw;

	tmp=gctalloc("lbxgl_lasereffect_t", sizeof(LBXGL_LaserEffect));
	V3F_COPY(sv, tmp->start);
	V3F_COPY(ev, tmp->end);
	V4F_COPY(clr, tmp->color);
	tmp->radius=rad;
	
	return(tmp);
}

