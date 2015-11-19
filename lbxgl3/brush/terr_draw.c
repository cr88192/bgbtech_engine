#include <lbxgl2.h>
#include <pdglui.h>

LBXGL_API void LBXGL_Terrain_GenTerrain(LBXGL_BrushWorld *world)
{
	int a, b, c, d;
	int i, j, k, tex, xs, ys;

	if(world->terr_hmap)
		return;

	world->terr_hmap=malloc(256*256);
	memset(world->terr_hmap, 128, 256*256);

	world->terr_xorg=-32;
	world->terr_yorg=-32;

	world->terr_xs=64;
	world->terr_ys=64;

	for(i=1; i<world->terr_ys; i++)
		for(j=1; j<world->terr_xs; j++)
	{
		a=world->terr_hmap[(i-1)*256+(j-1)];
		b=world->terr_hmap[(i-1)*256+(j-0)];
		c=world->terr_hmap[(i-0)*256+(j-1)];
		d=b+c-a;

		k=d+bgbrng_sfrand()*32;
		k=(0<=k)?((k<=255)?k:255):0;
		world->terr_hmap[i*256+j]=k;
	}

	world->terr_tex=Tex_LoadFile("textures/ground/cs_terrain", &xs, &ys);
	world->terr_tex_xs=xs;
	world->terr_tex_ys=ys;

//	world->terr_tex_xsc=1.0/xs;
//	world->terr_tex_ysc=1.0/ys;
}

void LBXGL_Terrain_DrawFlat(LBXGL_BrushWorld *world)
{
	float v0[3], v1[3], v2[3], v3[3];
	int a, b, c, d;
	int i, j, k, xo, yo;

	if(!world->terr_hmap)
		return;

	xo=world->terr_xorg*128;
	yo=world->terr_yorg*128;

	pdglBegin(PDGL_TRIANGLES);

	for(i=1; i<world->terr_ys; i++)
		for(j=1; j<world->terr_xs; j++)
	{
		a=world->terr_hmap[(i-1)*256+(j-1)];
		b=world->terr_hmap[(i-1)*256+(j-0)];
		c=world->terr_hmap[(i-0)*256+(j-1)];
		d=world->terr_hmap[(i-0)*256+(j-0)];

		v0[0]=(j-1)*128+xo; v0[1]=(i-1)*128+yo;
		v1[0]=(j-0)*128+xo; v1[1]=(i-1)*128+yo;
		v2[0]=(j-1)*128+xo; v2[1]=(i-0)*128+yo;
		v3[0]=(j-0)*128+xo; v3[1]=(i-0)*128+yo;

		v0[2]=a-128; v1[2]=b-128;
		v2[2]=c-128; v3[2]=d-128;

		pdglVertex3fv(v0);
		pdglVertex3fv(v1);
		pdglVertex3fv(v2);

		pdglVertex3fv(v1);
		pdglVertex3fv(v3);
		pdglVertex3fv(v2);
	}

	pdglEnd();
}

void LBXGL_Terrain_DrawFlatNorm(LBXGL_BrushWorld *world)
{
	float v0[3], v1[3], v2[3], v3[3];
	float tv0[3], tv1[3], tv2[3];
	int a, b, c, d;
	int i, j, k, xo, yo;

	if(!world->terr_hmap)
		return;

	xo=world->terr_xorg*128;
	yo=world->terr_yorg*128;

	pdglBegin(PDGL_TRIANGLES);

	for(i=1; i<world->terr_ys; i++)
		for(j=1; j<world->terr_xs; j++)
	{
		a=world->terr_hmap[(i-1)*256+(j-1)];
		b=world->terr_hmap[(i-1)*256+(j-0)];
		c=world->terr_hmap[(i-0)*256+(j-1)];
		d=world->terr_hmap[(i-0)*256+(j-0)];

		v0[0]=(j-1)*128+xo; v0[1]=(i-1)*128+yo;
		v1[0]=(j-0)*128+xo; v1[1]=(i-1)*128+yo;
		v2[0]=(j-1)*128+xo; v2[1]=(i-0)*128+yo;
		v3[0]=(j-0)*128+xo; v3[1]=(i-0)*128+yo;

		v0[2]=a-128; v1[2]=b-128;
		v2[2]=c-128; v3[2]=d-128;

		V3F_SUB(v1, v0, tv0);
		V3F_SUB(v2, v0, tv1);
		V3F_CROSS(tv0, tv1, tv2);
		V3F_NORMALIZE(tv2, tv2);
		pdglNormal3fv(tv2);

		pdglVertex3fv(v0);
		pdglVertex3fv(v1);
		pdglVertex3fv(v2);

		V3F_SUB(v3, v1, tv0);
		V3F_SUB(v2, v1, tv1);
		V3F_CROSS(tv0, tv1, tv2);
		V3F_NORMALIZE(tv2, tv2);
		pdglNormal3fv(tv2);

		pdglVertex3fv(v1);
		pdglVertex3fv(v3);
		pdglVertex3fv(v2);
	}

	pdglEnd();
}

void LBXGL_Terrain_DrawFlatNormClip(LBXGL_BrushWorld *world,
	int mx, int my, int nx, int ny)
{
	float v0[3], v1[3], v2[3], v3[3];
	float tv0[3], tv1[3], tv2[3];
	int a, b, c, d;
	int i, j, k, xo, yo;

	if(!world->terr_hmap)
		return;

	xo=world->terr_xorg*128;
	yo=world->terr_yorg*128;

	pdglBegin(PDGL_TRIANGLES);

	mx=(1<mx)?((mx<world->terr_xs)?mx:world->terr_xs):1;
	my=(1<my)?((my<world->terr_ys)?my:world->terr_ys):1;
	nx=(1<nx)?((nx<world->terr_xs)?nx:world->terr_xs):1;
	ny=(1<ny)?((ny<world->terr_ys)?ny:world->terr_ys):1;

	for(i=my; i<ny; i++)
		for(j=mx; j<nx; j++)
	{
		a=world->terr_hmap[(i-1)*256+(j-1)];
		b=world->terr_hmap[(i-1)*256+(j-0)];
		c=world->terr_hmap[(i-0)*256+(j-1)];
		d=world->terr_hmap[(i-0)*256+(j-0)];

		v0[0]=(j-1)*128+xo; v0[1]=(i-1)*128+yo;
		v1[0]=(j-0)*128+xo; v1[1]=(i-1)*128+yo;
		v2[0]=(j-1)*128+xo; v2[1]=(i-0)*128+yo;
		v3[0]=(j-0)*128+xo; v3[1]=(i-0)*128+yo;

		v0[2]=a-128; v1[2]=b-128;
		v2[2]=c-128; v3[2]=d-128;

		V3F_SUB(v1, v0, tv0);
		V3F_SUB(v2, v0, tv1);
		V3F_CROSS(tv0, tv1, tv2);
		V3F_NORMALIZE(tv2, tv2);
		pdglNormal3fv(tv2);

		pdglVertex3fv(v0);
		pdglVertex3fv(v1);
		pdglVertex3fv(v2);

		V3F_SUB(v3, v1, tv0);
		V3F_SUB(v2, v1, tv1);
		V3F_CROSS(tv0, tv1, tv2);
		V3F_NORMALIZE(tv2, tv2);
		pdglNormal3fv(tv2);

		pdglVertex3fv(v1);
		pdglVertex3fv(v3);
		pdglVertex3fv(v2);
	}

	pdglEnd();
}

void LBXGL_Terrain_DrawDark(LBXGL_BrushWorld *world)
{
	pdglDisableTexture2D();
	pdglColor4f(0, 0, 0, 1);

	LBXGL_Terrain_DrawFlat(world);
}

void LBXGL_Terrain_DrawLight(LBXGL_BrushWorld *world, LBXGL_Light *light)
{
	int x, y, w;
	float r;

#if 1
//	pdglDisable(GL_TEXTURE_2D);
	pdglDisableTexture2D();
	pdglColor4f(1, 1, 1, 1);

	LBXGL_BrushWorld_DrawSetupLight(world, light);
//	LBXGL_BrushWorld_DrawSetupLightScale_NoPhong(world, light, 1.0);

	pdglDepthFunc(GL_LEQUAL);
	pdglStencilFunc(GL_EQUAL, 0, 255);
	pdglBlendFunc(GL_SRC_COLOR, GL_ONE);

	pdglEnable(GL_CULL_FACE);
#endif

//	r=LBXGL_Shadow_LightGammaEffectRadius(light, 0.01);
	r=light->val[0];
	x=(light->org[0]/128)-world->terr_xorg;
	y=(light->org[1]/128)-world->terr_yorg;
	w=(((int)r)+127)/128 + 2;

	LBXGL_Terrain_DrawFlatNormClip(world, x-w, y-w, x+w, y+w);

//	LBXGL_Terrain_DrawFlatNorm(world);

#if 1
	PDGL_UnbindShader();
//	pdglDisable(GL_LIGHTING);
	pdglDisableLighting();
#endif
}

void LBXGL_Terrain_DrawFinal(LBXGL_BrushWorld *world)
{
	pdglBlendFunc(GL_DST_COLOR, GL_ZERO);

	LBXGL_Terrain_DrawTexture(world);
}

void LBXGL_Terrain_DrawTexture(LBXGL_BrushWorld *world)
{
	float v0[3], v1[3], v2[3], v3[3];
	float st0[2], st1[2], st2[2], st3[2];
	float xsc, ysc;
	int a, b, c, d;
	int i, j, k, tex, xs, ys, xo, yo;

	if(!world->terr_hmap)
		return;

	if(world->terr_tex>0)
	{
//		pdglEnableTexture2D();
		pdglEnableTexture2D();
		pdglBindTexture(GL_TEXTURE_2D, world->terr_tex);
		xsc=1.0/world->terr_tex_xs;
		ysc=1.0/world->terr_tex_ys;
	}else
	{
//		pdglDisableTexture2D();
		pdglDisableTexture2D();
		xsc=1.0; ysc=1.0;
	}

	pdglEnable(GL_CULL_FACE);
	pdglCullFace(GL_BACK);

	pdglColor4f(1, 1, 1, 1);

	xo=world->terr_xorg*128;
	yo=world->terr_yorg*128;

	pdglBegin(PDGL_TRIANGLES);

	for(i=1; i<world->terr_ys; i++)
		for(j=1; j<world->terr_xs; j++)
	{
		a=world->terr_hmap[(i-1)*256+(j-1)];
		b=world->terr_hmap[(i-1)*256+(j-0)];
		c=world->terr_hmap[(i-0)*256+(j-1)];
		d=world->terr_hmap[(i-0)*256+(j-0)];

		v0[0]=(j-1)*128+xo; v0[1]=(i-1)*128+yo;
		v1[0]=(j-0)*128+xo; v1[1]=(i-1)*128+yo;
		v2[0]=(j-1)*128+xo; v2[1]=(i-0)*128+yo;
		v3[0]=(j-0)*128+xo; v3[1]=(i-0)*128+yo;

		v0[2]=a-128;
		v1[2]=b-128;
		v2[2]=c-128;
		v3[2]=d-128;

		st0[0]=v0[0]*xsc; st0[1]=v0[1]*xsc;
		st1[0]=v1[0]*xsc; st1[1]=v1[1]*xsc;
		st2[0]=v2[0]*xsc; st2[1]=v2[1]*xsc;
		st3[0]=v3[0]*xsc; st3[1]=v3[1]*xsc;

		pdglTexCoord2fv(st0);
		pdglVertex3fv(v0);
		pdglTexCoord2fv(st1);
		pdglVertex3fv(v1);
		pdglTexCoord2fv(st2);
		pdglVertex3fv(v2);

		pdglTexCoord2fv(st1);
		pdglVertex3fv(v1);
		pdglTexCoord2fv(st3);
		pdglVertex3fv(v3);
		pdglTexCoord2fv(st2);
		pdglVertex3fv(v2);
	}

	pdglEnd();

//	pdglEnableTexture2D();
}
