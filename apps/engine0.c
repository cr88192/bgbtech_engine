#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI		3.1415926535897932384626433832795
#endif

#define PDGL_BASEHEAP		(64*1024*1024)
#define PDGL_BASEHEAP_LIMIT	(256*1024*1024)
#define PDGL_BASEHEAP_LOBJ	(1024*1024*1024)
#define PDGL_BASEHEAP_CONS	(64*1024*1024)

#define PDGL_PRESTART

#include <pdgl.h>
#include <pdglui.h>
#include <pdgl_stub.h>

#include <lbxgl2.h>
#include <libvecmath.h>

#include <btgesv.h>

#include <bgbasm.h>
#include <bgbccc.h>
#include <bgbsvm.h>


int main_phys=1;
int main_sun=0;

LBXGL_BrushWorld *world;
LBXGL_Light *sun;

BTGE_BrushWorld *sv_world;
BTCL_World *cl_world;

btEntity player;

int ui_state;
int shader_phong;
int shader_phong_spot;
int tex_screen;

char *map_doLoad=NULL;
char map_doLoadStep=0;
char map_doLoadType=0;

int main_timestat=0;

int map_isedit=0;

// char *map_name=NULL;
// int map_stat_brushes;
// int map_stat_ents;
// int map_stat_lights;

LBXGL_API LBXGL_BrushWorld *lbxgl_brushmesh_world;

LBXGL_API LBXGL_MeshGroup *lbxgl_brushmesh_mdl;
LBXGL_API LBXGL_MeshGroup *lbxgl_meshsel_grp;
LBXGL_API LBXGL_MeshPrim *lbxgl_meshsel_prim;
LBXGL_API int lbxgl_meshsel_face, lbxgl_meshsel_vec;
LBXGL_API int lbxgl_meshsel_multi;

LBXGL_API int lbxgl_brushmesh_mode;
LBXGL_API int lbxgl_brushmesh_cmd;
LBXGL_API LBXGL_MeshGroup *lbxgl_brushmesh_clip;

static double bm_log2(double x)
	{ return(log(x)/log(2)); }

void MapEdit_CmdUndo()
{
	lbxgl_brushmesh_world=world;
	MapEdit_Undo();
	world=lbxgl_brushmesh_world;
}

void MapEdit_CmdRedo()
{
	lbxgl_brushmesh_world=world;
	MapEdit_Redo();
	world=lbxgl_brushmesh_world;
}

int MapEdit_IsEdit()
{
	if(cam->raw)return(1);
	if(cl_world && (cl_world->viewflags&BT_EF2_CAM_CREATIVE))
		return(1);
	return(map_isedit);
}

int main_DrawBox(
	float minx, float miny, float minz,
	float maxx, float maxy, float maxz)
{
	int i, j;

//	dyPrintf("cube (%f %f %f) (%f %f %f)\n",
//		minx, miny, minz,
//		maxx, maxy, maxz);

	float vtxa[8*4];
	static int vtxb[6*4]=
	{
		1, 3, 2, 0,
		4, 6, 7, 5,
		5, 7, 3, 1,
		0, 2, 6, 4,
		5, 1, 0, 4,
		3, 7, 6, 2
	};

	float *pt1, *pt2, *pt3, *pt4;

	for(i=0; i<8; i++)
	{
		vtxa[i*4+0]=minx;
		vtxa[i*4+1]=miny;
		vtxa[i*4+2]=minz;
		vtxa[i*4+3]=0;
		if(i&1)vtxa[i*4+0]=maxx;
		if(i&2)vtxa[i*4+1]=maxy;
		if(i&4)vtxa[i*4+2]=maxz;
	}


	for(i=0; i<6; i++)
	{
		pt1=vtxa+(vtxb[i*4+0]*4);
		pt2=vtxa+(vtxb[i*4+1]*4);
		pt3=vtxa+(vtxb[i*4+2]*4);
		pt4=vtxa+(vtxb[i*4+3]*4);

		LBXGL_Shader_BeginPoly();

		LBXGL_Shader_TexCoord2f(0, 0);
		LBXGL_Shader_Vertex4fv(pt1);

		LBXGL_Shader_TexCoord2f(1, 0);
		LBXGL_Shader_Vertex4fv(pt2);

		LBXGL_Shader_TexCoord2f(1, 1);
		LBXGL_Shader_Vertex4fv(pt3);

		LBXGL_Shader_TexCoord2f(0, 1);
		LBXGL_Shader_Vertex4fv(pt4);

		LBXGL_Shader_EndPoly();
	}
	return(0);
}

int main_DrawBox4D(
	float minx, float miny, float minz, float minw,
	float maxx, float maxy, float maxz, float maxw)
{
	int i, j;

//	dyPrintf("cube (%f %f %f) (%f %f %f)\n",
//		minx, miny, minz,
//		maxx, maxy, maxz);

	float vtxa[16*4];
	static int vtxb[24*4]=
	{
		 1,  3,  2,  0,
		 4,  6,  7,  5,
		 5,  7,  3,  1,
		 0,  2,  6,  4,
		 5,  1,  0,  4,
		 3,  7,  6,  2,

		 9, 11, 10,  8,
		12, 14, 15, 13,
		13, 15, 11,  9,
		 8, 10, 14, 12,
		13,  9,  8, 12,
		11, 15, 14, 10,

#if 0
		 1,  9,  8,  0,
		 4, 12, 13,  5,
		 5, 13,  9,  1,
		 0,  8, 12,  4,
		 5, 13, 12,  4,
		 3, 11, 10,  2,

		 3, 11, 10,  2,
		 6, 14, 15,  7,
		 7, 15, 11,  3,
		 2, 10, 14,  6,
		 1,  9,  8,  0,
		 7, 15, 14,  6,
#endif

#if 0
		 1,  9, 11,  3,
		 4, 12, 14,  6,
		 5, 13, 15,  7,
		 0,  8, 10,  2,
		 5, 13,  9,  1,
		 3, 11, 15,  7,

		 2, 10,  8,  0,
		 7, 15, 13,  5,
		 3, 11,  9,  1,
		 6, 14, 12,  4,
		 0,  8, 12,  4,
		 6, 14, 10,  2,
#endif

#if 1
		 1,  9,  8,  0,
		 4, 12, 13,  5,
		 5, 13,  9,  1,
		 0,  8, 12,  4,
		 3, 11, 10,  2,

		 6, 14, 15,  7,
		 7, 15, 11,  3,
		 2, 10, 14,  6,

		 1,  9, 11,  3,
		 4, 12, 14,  6,
		 5, 13, 15,  7,
		 0,  8, 10,  2,
#endif
	};

	float *pt1, *pt2, *pt3, *pt4;

	for(i=0; i<16; i++)
	{
		vtxa[i*4+0]=minx;
		vtxa[i*4+1]=miny;
		vtxa[i*4+2]=minz;
		vtxa[i*4+3]=minw;
		if(i&1)vtxa[i*4+0]=maxx;
		if(i&2)vtxa[i*4+1]=maxy;
		if(i&4)vtxa[i*4+2]=maxz;
		if(i&8)vtxa[i*4+3]=maxw;
	}


	for(i=0; i<24; i++)
	{
		pt1=vtxa+(vtxb[i*4+0]*4);
		pt2=vtxa+(vtxb[i*4+1]*4);
		pt3=vtxa+(vtxb[i*4+2]*4);
		pt4=vtxa+(vtxb[i*4+3]*4);

		LBXGL_Shader_BeginPoly();

		LBXGL_Shader_TexCoord2f(0, 0);
		LBXGL_Shader_Vertex4fv(pt1);

		LBXGL_Shader_TexCoord2f(1, 0);
		LBXGL_Shader_Vertex4fv(pt2);

		LBXGL_Shader_TexCoord2f(1, 1);
		LBXGL_Shader_Vertex4fv(pt3);

		LBXGL_Shader_TexCoord2f(0, 1);
		LBXGL_Shader_Vertex4fv(pt4);

		LBXGL_Shader_EndPoly();
	}
	return(0);
}

void draw_test_box4d()
{
	float f;
	int i, j, k, tex, xs, ys;

	return;

	tex=LBXGL_Texture_LoadImageSize(
		"textures/ground/cs_terrain", &xs, &ys);

	pdglDisable(GL_CULL_FACE);

	LBXGL_Shader_PushMatrix4D();

	f=(90/M_PI)*state->time_f;
//	LBXGL_Shader_RotateAngles4D_f(0,0,0, f,f,f);
	LBXGL_Shader_RotateAngles4D_f(0,0,0, f,0,0);

	LBXGL_Shader_Color4f(1, 1, 1, 0.5);

	LBXGL_Shader_BindTexture(tex);
//	main_DrawBox(-16, -16, -16, 16, 16, 16);
	main_DrawBox4D(-16, -16, -16, -16, 16, 16, 16, 16);

	LBXGL_Shader_Color4f(1, 1, 1, 1);

	LBXGL_Shader_PopMatrix4D();
}

void draw_test_defsdr()
{
	int sdr, tex, def;

	draw_test_box4d();

	return;
#if 0
	Tex_FillTextureScreenshot(tex_screen);

	tex=Tex_LoadFile("textures/effects/fire/bgb_fire3", NULL, NULL);
	def=Tex_LoadFile("textures/effects/fire/turb0", NULL, NULL);

//	def=Tex_LoadFile("textures/effects/gloss0", NULL, NULL);
//	def=Tex_LoadFile("textures/effects/glass_hvgrid", NULL, NULL);

	sdr=PDGL_LoadShader("bgb_deform1");
//	sdr=PDGL_LoadShader("warp0");
	PDGL_BindShader(sdr);

	PDGL_Uniform1f("time", state->time_f);
	PDGL_Uniform1f("scale", 0.05);
	PDGL_Uniform1f("rate", 0.25);

//	PDGL_Uniform1i("basetex", tex_screen);
//	PDGL_Uniform1i("deformtex", def);

	PDGL_Uniform1i("texScreen", 0);
	PDGL_Uniform1i("texBase", 1);
	PDGL_Uniform1i("texDeform", 2);

//	pdglDisable(GL_TEXTURE_2D);

//	pdglBindTexture(GL_TEXTURE_2D, tex_screen);
//	pdglEnable(GL_TEXTURE_2D);

	pdglActiveTexture(0);
	pdglBindTexture(GL_TEXTURE_2D, tex_screen);
//	pdglTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	pdglEnable(GL_TEXTURE_2D);

	pdglActiveTexture(1);
	pdglBindTexture(GL_TEXTURE_2D, tex);
	pdglEnable(GL_TEXTURE_2D);

	pdglActiveTexture(2);
	pdglBindTexture(GL_TEXTURE_2D, def);
	pdglEnable(GL_TEXTURE_2D);

	pdglActiveTexture(0);

//	pdglColor4f(0.01, 0.01, 0.01, 1);
	pdglColor4f(1, 1, 1, 0.75);

	pdglBegin(PDGL_QUADS);

	pdglMultiTexCoord2f(0, 0, 1);
	pdglMultiTexCoord2f(1, 0, 1);
	pdglVertex3f(-16, 16, 16);

	pdglMultiTexCoord2f(0, 0, 0);
	pdglMultiTexCoord2f(1, 0, 0);
	pdglVertex3f(-16, 16, -16);

	pdglMultiTexCoord2f(0, 1, 0);
	pdglMultiTexCoord2f(1, 1, 0);
	pdglVertex3f(16, 16, -16);

	pdglMultiTexCoord2f(0, 1, 1);
	pdglMultiTexCoord2f(1, 1, 1);
	pdglVertex3f(16, 16, 16);

	pdglEnd();


	pdglActiveTexture(0);
	pdglBindTexture(GL_TEXTURE_2D, 0);
	pdglDisable(GL_TEXTURE_2D);

	pdglActiveTexture(1);
	pdglBindTexture(GL_TEXTURE_2D, 0);
	pdglDisable(GL_TEXTURE_2D);

	pdglActiveTexture(2);
	pdglBindTexture(GL_TEXTURE_2D, 0);
	pdglDisable(GL_TEXTURE_2D);

	pdglActiveTexture(0);

	PDGL_UnbindShader();
#endif
}

void draw_test_terrain()
{
	static byte *hmap=NULL;

	float v0[3], v1[3], v2[3], v3[3];
	float st0[2], st1[2], st2[2], st3[2];
	float xsc, ysc;
	int a, b, c, d;
	int i, j, k, tex, xs, ys;


//	LBXGL_Terrain_GenTerrain(world);

	return;


	if(!hmap)
	{
		hmap=malloc(256*256);
		memset(hmap, 128, 256*256);

		for(i=1; i<256; i++)
			for(j=1; j<256; j++)
		{
			a=hmap[(i-1)*256+(j-1)];
			b=hmap[(i-1)*256+(j-0)];
			c=hmap[(i-0)*256+(j-1)];
			d=b+c-a;

			k=d+bgbrng_sfrand()*32;
			k=(0<=k)?((k<=255)?k:255):0;
			hmap[i*256+j]=k;
		}
	}

	tex=Tex_LoadFile("textures/ground/cs_terrain", &xs, &ys);

	if(tex>0)
	{
		pdglEnable(GL_TEXTURE_2D);
		pdglBindTexture(GL_TEXTURE_2D, tex);
		xsc=1.0/xs;
		ysc=1.0/ys;
	}else
	{
		pdglDisable(GL_TEXTURE_2D);
		xsc=1.0; ysc=1.0;
	}

	pdglCullFace(GL_BACK);

	pdglColor4f(0.5, 0.5, 0.5, 1);

	for(i=1; i<256; i++)
		for(j=1; j<256; j++)
	{
		a=hmap[(i-1)*256+(j-1)];
		b=hmap[(i-1)*256+(j-0)];
		c=hmap[(i-0)*256+(j-1)];
		d=hmap[(i-0)*256+(j-0)];

		v0[0]=(j-1)*128; v0[1]=(i-1)*128;
		v1[0]=(j-0)*128; v1[1]=(i-1)*128;
		v2[0]=(j-1)*128; v2[1]=(i-0)*128;
		v3[0]=(j-0)*128; v3[1]=(i-0)*128;

		v0[2]=a-128;
		v1[2]=b-128;
		v2[2]=c-128;
		v3[2]=d-128;

		st0[0]=v0[0]*xsc; st0[1]=v0[1]*xsc;
		st1[0]=v1[0]*xsc; st1[1]=v1[1]*xsc;
		st2[0]=v2[0]*xsc; st2[1]=v2[1]*xsc;
		st3[0]=v3[0]*xsc; st3[1]=v3[1]*xsc;

		pdglBegin(PDGL_TRIANGLES);
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
		pdglEnd();
	}

	pdglEnable(GL_TEXTURE_2D);
}

#if 1
void sel_brush_face()
{
	static float pts0[16*3];
	static float pts1[16*3];

	float org[3];
	float sx, sy, f, g;
	int tn, xs, ys;
	int i, j, k, l, t;

	world->brush_sel_face=-1;
	if(!world->brush_sel)return;

	for(i=0; i<world->brush_sel->n_face; i++)
	{
//		if(V3F_NDOT(cam->org, world->brush_sel->norm+i*4)<0)
//			continue;

		Hull_MakePlaneFace(world->brush_sel->norm+i*4, pts0);
		l=4;

		for(j=0; j<world->brush_sel->n_face; j++)
		{
			if(i==j)continue;
			l=Hull_ClipFace(world->brush_sel->norm+j*4, pts0, pts1, l);
			for(k=0; k<(l*3); k++)pts0[k]=pts1[k];
		}

		if(!l)continue;

		V3F_ZERO(org);
		for(j=0; j<l; j++) { V3F_ADD(org, pts0+j*3, org); }
		V3F_SCALE(org, 1.0/l, org);

		f=(V3F_DOT(org, cam->fw)-
			V3F_DOT(cam->org, cam->fw))/64;
		g=Vec3F_DistLineSeg(org, cam->morg, cam->mend);
		if(g<f)
		{
			world->brush_sel_face=i;
		}
	}
}

void sel_brushes(LBXGL_Brush *fst)
{
	float tv0[3], tv1[3], tv2[3];
	float f, g, bf;
	LBXGL_Brush *cur, *best;
	int i;

	if((!world->brush_sel ||
		!LBXGL_Brush_CheckLineBrush(world->brush_sel,
			cam->morg, cam->mend)) &&
		!cam->ortho)
	{
		Con_Printf("Sel Brush\n");

		best=NULL; bf=999999;

		cur=fst;
		while(cur)
		{
			if(cur->flags&LBXGL_BRFL_DEL)
				{ cur=cur->next; continue; }

			i=LBXGL_Brush_CheckLineBrush(cur,
				cam->morg, cam->mend);
			if(!i) { cur=cur->next; continue; }

			f=V3F_DOT(cur->org, cam->fw)-V3F_DOT(cam->org, cam->fw);

			if(!best || ((f>=0) && (f<bf))) { best=cur; bf=f; }

			cur=cur->next;
		}

		if(!best)Con_Printf("Sel: No Brush\n");

		world->brush_sel=best;
		world->brush_sel_face=-1;

//		world->entity_sel=best->se_owner;

		return;
	}

	Con_Printf("Sel 2\n");

	cur=fst;
	while(cur)
	{
		if(cur->flags&LBXGL_BRFL_DEL)
			{ cur=cur->next; continue; }

		if(world->brush_sel==cur)
		{
			world->brush_sel=NULL;
			world->brush_sel_face=-1;
			cur=cur->next;
			continue;
		}

		i=LBXGL_Brush_CheckLineBrush(cur, cam->morg, cam->mend);

		if(i && !world->brush_sel)
		{
			world->brush_sel=cur;
			world->brush_sel_face=-1;
//			world->entity_sel=cur->se_owner;
		}

		cur=cur->next;

#if 0
		V3F_COPY(cam->morg, tv0);
		V3F_COPY(cam->mend, tv1);

		for(i=0; i<cur->n_face; i++)
		{
			f=V3F_NDOT(tv0, cur->norm+i*4);
			g=V3F_NDOT(tv1, cur->norm+i*4);
			if((f>0) && (g>0))break;
			if((f<=0) && (g<=0))continue;

			Hull_LinePlaneIntersect(tv0, tv1, cur->norm+i*4, tv2);

			if(f>0) { V3F_COPY(tv2, tv0); }
				else { V3F_COPY(tv2, tv1); }
		}


		if(i==cur->n_face)
		{
			if(!world->brush_sel)
			{
				world->brush_sel=cur;
				world->brush_sel_face=-1;
			}
		}

		cur=cur->next;
#endif
	}

}

float ent_radius(LBXGL_SEntity *ent)
{
	char *ty;
	float f;

	f=8;

	ty=LBXGL_SEnt_GetStr(ent, "classname");

	if(!strncmp(ty, "light", 5))
	{
		f=LBXGL_SEnt_GetNum(ent, "light");
		f=bm_log2(f);
		if(f<4)f=4;
		if(f>16)f=16;
	}

//	if(!strcmp(ty, "info_player_start") ||
//		!strcmp(ty, "info_player_coop") ||
//		!strcmp(ty, "info_player_deathmatch"))
	if(!strncmp(ty, "info_player_", 12))
		f=36;

	if(!strncmp(ty, "monster_", 8))
		f=36;

	if(!strncmp(ty, "item_", 5) ||
		!strncmp(ty, "weapon_", 7))
		f=24;

	return(f);
}

void sel_ents()
{
	LBXGL_SEntity *cur;
	float org[3];
	float f, g;

	world->entity_sel=NULL;

//	return;

	cur=world->entity;
	while(cur)
	{
		f=ent_radius(cur);

		LBXGL_SEnt_GetVec3(cur, "origin", org);
		g=Vec3F_DistLineSeg(org, cam->morg, cam->mend);
		if(g<f)
		{
			world->entity_sel=cur;
			world->brush_sel=NULL;
		}

		cur=cur->next;
	}
}
#endif

int check_str_num(char *s)
{
	if(!s)return(0);

	while(*s && *s<=' ')s++;
	if(!(*s) || (*s<'0') || (*s>'9'))
		return(0);

	if((s[0]=='0') && (s[1]=='x'))
	{
		s+=2;
		while(*s)
		{
			if((*s>='0') && (*s<='9')) { s++; continue; }
			if((*s>='A') && (*s<='F')) { s++; continue; }
			if((*s>='a') && (*s<='f')) { s++; continue; }
			return(0);
		}
		return(1);
	}

	while(*s)
	{
		if((*s>='0') && (*s<='9')) { s++; continue; }
		if((*s=='.') || (*s=='e') || (*s=='E')) { s++; continue; }
		return(0);
	}
	return(1);
}

int check_str_real(char *s)
{
	if(!s)return(0);

	while(*s && *s<=' ')s++;
	if(!(*s) || (*s<'0') || (*s>'9'))return(0);
	if((s[0]=='0') && (s[1]=='x'))return(0);
	while(*s) { if((*s=='.') || (*s=='e') || (*s=='E'))return(1); s++; }
	return(1);
}

#if 0
dyt spawn_ents(LBXGL_BrushWorld *wrl)
{
	LBXGL_SEntity *cur;
	dyt tmp, lst;

	cur=wrl->entity; lst=NULL;
	while(cur)
	{
		lbxEntSpawnWorldSEnt(world, cur);
		cur=cur->next;
	}

	return(NULL);
}
#endif

void draw_ent(LBXGL_SEntity *ent)
{
	float org[3], tv[3], tv1[3], tv2[3], ang[3];
	LBXGL_SEntity *ent2;
	float f, g, h;
	char *ty, *s;
	int i, tn, xs, ys;

	ty=LBXGL_SEnt_GetStr(ent, "classname");
	LBXGL_SEnt_GetVec3(ent, "origin", org);

	i=LBXGL_SEnt_GetVec3(ent, "angles", ang);
	if(i<0)
	{
		f=LBXGL_SEnt_GetNum(ent, "angle");

		if(f==-1) { ang[0]=90; ang[1]=0; ang[2]=0; }
		else if(f==-2) { ang[0]=-90; ang[1]=0; ang[2]=0; }
		else { ang[0]=0; ang[1]=0; ang[2]=f; }
	}

	pdglColor4f(1, 1, 1, 1);

	f=ent_radius(ent);
	g=Vec3F_DistLineSeg(org, cam->morg, cam->mend);
	if(g<f)
		pdglColor4f(1, 0.5, 1, 1);

	if(ent==world->entity_sel)
		pdglColor4f(0.5, 1, 0.5, 1);

	if((ent==world->entity_sel) && (state->mb&1))
	{
		if((state->mb&2) && (state->lmb&2))
		{
			f=V3F_DOT(org, cam->lmdir)-
				V3F_DOT(cam->lmorg, cam->lmdir);
			V3F_ADDSCALE(cam->morg, cam->mdir, f, tv);
			V3F_SUB(tv, org, tv);
		}else
		{
			f=V3F_DOT(org, cam->fw)-
				V3F_DOT(cam->org, cam->fw);
			V3F_SCALEADDSCALE(
				cam->rt, f*state->dmx/(state->xs/2.0),
				cam->up, f*(-state->dmy)/(state->xs/2.0), tv);
			V3F_SUB(cam->org, cam->lorg, tv1);
			V3F_ADD(tv, tv1, tv);
		}

		V3F_ADD(org, tv, org);
		LBXGL_SEnt_SetVec3(ent, "origin", org);
	}

	tn=-1;

	tn=Tex_LoadFile("images/mystery.png", &xs, &ys);
	f=4;

	if(!strncmp(ty, "light", 5))
	{
		if(!strncmp(ty, "light_torch", 11))
		{
			tn=Tex_LoadFile("images/torch.png", &xs, &ys);
		}else if(!strncmp(ty, "light_flame", 11))
		{
			tn=Tex_LoadFile("images/flame.png", &xs, &ys);
		}else
		{
			tn=Tex_LoadFile("images/light0.png", &xs, &ys);
		}

//		f=(V3F_DOT(org, cam->fw)-
//			V3F_DOT(cam->org, cam->fw))/32;

		f=LBXGL_SEnt_GetNum(ent, "light");
//		f=bm_log2(f);
		f=sqrt(f)/4;
		if(f<4)f=4;
	}

	if(!strcmp(ty, "info_player_start"))
	{
		tn=Tex_LoadFile("images/dude.png", &xs, &ys);
		f=36;
	}

	if(!strcmp(ty, "info_player_coop"))
	{
		tn=Tex_LoadFile("images/dude_green.png", &xs, &ys);
		f=36;
	}

	if(!strcmp(ty, "info_player_deathmatch"))
	{
		tn=Tex_LoadFile("images/dude_gun.png", &xs, &ys);
		f=36;
	}

	if(!strncmp(ty, "info_player_", 12))
	{
		tn=Tex_LoadFile("images/dude.png", &xs, &ys);
		f=36;
	}

	if(!strncmp(ty, "monster", 7))
	{
		tn=Tex_LoadFile("images/dude_red.png", &xs, &ys);
		f=36;
	}

	if(!strncmp(ty, "weapon", 6))
	{
		tn=Tex_LoadFile("images/gun.png", &xs, &ys);
		f=24;
	}

	if(!strncmp(ty, "item", 4))
	{
		tn=Tex_LoadFile("images/bullet.png", &xs, &ys);
		f=24;
	}

	if(!strncmp(ty, "item_health", 11))
	{
		tn=Tex_LoadFile("images/health.png", &xs, &ys);
		f=24;
	}

	if(!strncmp(ty, "item_armor", 10))
	{
		tn=Tex_LoadFile("images/armor.png", &xs, &ys);
		f=24;
	}

	if(!strcmp(ty, "path_corner"))
	{
		tn=Tex_LoadFile("images/path0.png", &xs, &ys);
		f=8;
	}

	if(!strcmp(ty, "info_teleport_destination"))
	{
		tn=Tex_LoadFile("images/teledest.png", &xs, &ys);
		f=8;
	}

	if(!strcmp(ty, "misc_explobox") ||
		!strcmp(ty, "misc_explobox2"))
	{
		tn=Tex_LoadFile("images/barrel.png", &xs, &ys);
		f=16;
	}

	if(!strncmp(ty, "ambient_", 8))
	{
		tn=Tex_LoadFile("images/speaker.png", &xs, &ys);
		f=8;
	}

	if(!strcmp(ty, "target_speaker"))
	{
		tn=Tex_LoadFile("images/speaker.png", &xs, &ys);
		f=8;
	}

	if(tn>=0)
	{
//		pdglEnable(GL_ALPHA_TEST);

//		pdglEnable(GL_TEXTURE_2D);
		pdglBindTexture(GL_TEXTURE_2D, tn);

		pdglBegin(PDGL_POLYGON);

		pdglTexCoord2f(0, 0);
		V3F_ADDSCALEADDSCALE(org, cam->rt, -f,
			cam->up, -f, tv);
		pdglVertex3fv(tv);

		pdglTexCoord2f(1, 0);
		V3F_ADDSCALEADDSCALE(org, cam->rt, f,
			cam->up, -f, tv);
		pdglVertex3fv(tv);

		pdglTexCoord2f(1, 1);
		V3F_ADDSCALEADDSCALE(org, cam->rt, f,
			cam->up, f, tv);
		pdglVertex3fv(tv);

		pdglTexCoord2f(0, 1);
		V3F_ADDSCALEADDSCALE(org, cam->rt, -f,
			cam->up, f, tv);
		pdglVertex3fv(tv);

		pdglEnd();

//		pdglDisable(GL_ALPHA_TEST);
	}

	s=LBXGL_SEnt_GetStr(ent, "_flare");
	if(s)LBXGL_Sprite_RenderFlareName(s, org, 1000);

	s=LBXGL_SEnt_GetStr(ent, "target");
	if(s)
	{
		ent2=BTGE_SEnt_FindEntity(world->entity, "targetname", s);
		if(ent2)
		{
			LBXGL_SEnt_GetVec3(ent, "origin", tv1);
			LBXGL_SEnt_GetVec3(ent2, "origin", tv2);

			if(ent->bmdl) { V3F_COPY(ent->bmdl->com_org, tv1); }
			if(ent2->bmdl) { V3F_COPY(ent2->bmdl->com_org, tv2); }

			pdglDisable(GL_TEXTURE_2D);
			
			pdglBegin(PDGL_LINES);
			pdglColor4f(1,0,0,1);
			pdglVertex3fv(tv1);
			pdglColor4f(0,0,1,1);
			pdglVertex3fv(tv2);
			pdglEnd();
			
			pdglEnable(GL_TEXTURE_2D);
		}
	}

#if 0
	if(!strncmp(ty, "light_flame", 11) ||
		!strncmp(ty, "light_torch", 11))
	{
		V3F_ZERO(tv);

		f=25*bgbrng_randf()*state->dt_f;
		if(f<1.0)f=1.0;
		LBXGL_Particle_Spawn("flame", (int)f, org, tv);
	}
#endif

#if 0
	if(ent->mdl)
	{
		V3F_COPY(org, ent->mdl->org);

		V3F_SET(ent->mdl->rot+0, 1, 0, 0);
		V3F_SET(ent->mdl->rot+3, 0, 1, 0);
		V3F_SET(ent->mdl->rot+6, 0, 0, 1);

		if(!ent->bmdl)
		{
			h=ang[2]-90;
			f=cos(h*(M_PI/180)); g=sin(h*(M_PI/180));
			V3F_SET(ent->mdl->rot+0, f, g, 0);
			V3F_SET(ent->mdl->rot+3, -g, f, 0);

//			LBXGL_Mdl_DrawModel(ent->mdl);
		}else
		{
			h=ang[2]-90;
			f=cos(h*(M_PI/180)); g=sin(h*(M_PI/180));
			h=cos(ang[0]*(M_PI/180));

			tv[0]=-g*h; tv[1]=f*h;
			tv[2]=sin(ang[0]*(M_PI/180));

			LBXGL_BrushModel_GetExtents(ent->bmdl, tv, &f, &g);
			h=(g-f)*sin(state->time_f);

//			dyPrintf("%f\n", g-f);

//			h=40*sin(state->time_f);
			if(h<0)h=0;

			V3F_ADDSCALE(org, tv, h, ent->mdl->org);

//			org[0]+=-g * 10*sin(state->time_f);
//			org[1]+=f * 10*sin(state->time_f);

//			org[0]+=f * 10*sin(state->time_f);
//			org[1]+=g * 10*sin(state->time_f);

//			V3F_COPY(org, ent->mdl->org);
		}

		LBXGL_Mdl_StepTime(ent->mdl, state->dt_f);
	}
#endif
}

void draw_ents(LBXGL_SEntity *ent)
{
	LBXGL_SEntity *cur;

	cur=ent;
	while(cur)
	{
		draw_ent(cur);
		cur=cur->next;
	}
}

void unlink_entity(LBXGL_SEntity *ent)
{
	LBXGL_SEntity *cur;

	if(!ent)return;

	if(ent==world->entity)
	{
		world->entity=world->entity->next;
		return;
	}

	cur=world->entity;
	while(cur->next)
	{
		if(cur->next==ent)
		{
			cur->next=ent->next;
			break;
		}
		cur=cur->next;
	}
}


void setup_light_ents(LBXGL_SEntity *ent)
{
	float tv[3], tv1[3], tv2[3];
	LBXGL_SEntity *cur;
	LBXGL_Light *lcur, *lnxt;
	char *ty, *s;
	float f, g;
	int i, j, n;

#if 0
	lcur=world->light;
	while(lcur)
	{
		lnxt=lcur->next;
		gcfree(lcur);
		lcur=lnxt;
	}
#endif

//	world->light=NULL;
//	sun=NULL;
	world->skycam=0;

	cur=ent; n=1;
	while(cur)
	{
		ty=LBXGL_SEnt_GetStr(cur, "classname");
		if(!strncmp(ty, "light", 5))
		{
//			lcur=gcalloc(sizeof(LBXGL_Light));
//			lcur=lbxEntNewLightWorld(world);
//			cur->light=lcur;

			lcur=lbxWorldGetLight(world, n++);

#if 0
			i=LBXGL_SEnt_GetNum(cur, "_lightnum");
			if(i<=0)
			{
				i=lbxEntAllocId(world);
				LBXGL_SEnt_SetNum(cur, "_lightnum", i);
			}
#endif

//			i=n++;
//			lcur->num=i;

			i=LBXGL_SEnt_GetVec3(cur, "color", lcur->clr);
			if(i<0)i=LBXGL_SEnt_GetVec3(cur, "_color", lcur->clr);
			if(i<0)
			{
//				g=0.57735;
				g=1;
				V3F_SET(lcur->clr, g, g, g);
			}

			LBXGL_SEnt_GetVec3(cur, "origin", lcur->org);
			lcur->val[0]=LBXGL_SEnt_GetNum(cur, "light");
			lcur->val[1]=0;

			if(lcur->val[0]==0)
			{
				i=LBXGL_SEnt_GetVec3(cur, "light_radius", tv);
				if(i>=0)
				{
//					lcur->val[0]=V3F_LEN(tv);
//					lcur->val[0]=(tv[0]+tv[1]+tv[2])/3;

					V3F_COPY(tv, lcur->boxval);

					V3F_SCALE(tv, -1.001, lcur->mins);
					V3F_SCALE(tv,  1.001, lcur->maxs);
					lcur->flags|=LBXGL_LFL_BOXCULL;

					f=V3F_LEN(tv);
					g=(tv[0]+tv[1]+tv[2])/3;
					lcur->val[0]=(f+g)/2;
				}

				i=LBXGL_SEnt_GetVec3(cur, "light_start", tv1);
				j=LBXGL_SEnt_GetVec3(cur, "light_end", tv2);
				if((i>=0) && (j>=0))
				{
					f=V3F_SUB(tv2, tv1, lcur->dir);
					lcur->val[0]=f;
				}

				//adjust light center within bbox
				i=LBXGL_SEnt_GetVec3(cur, "light_center", tv);
				if(i>=0)
				{
					V3F_ADD(lcur->org, tv, lcur->org);
					V3F_SUB(lcur->mins, tv, lcur->mins);
					V3F_SUB(lcur->maxs, tv, lcur->maxs);
					
//					lcur->boxval[0]+=fabs(tv[0]);
//					lcur->boxval[1]+=fabs(tv[1]);
//					lcur->boxval[2]+=fabs(tv[2]);
				}
			}

			if(lcur->val[0]==0)
				{ lcur->val[0]=250; }

			lcur->curval=lcur->val[0];

			i=LBXGL_SEnt_GetNum(cur, "spawnflags");
			if(i&1)lcur->flags|=LBXGL_LFL_DISABLE;

			i=LBXGL_SEnt_GetNum(cur, "start_off");
			if(i)lcur->flags|=LBXGL_LFL_DISABLE;

			i=LBXGL_SEnt_GetNum(cur, "linear");
			if(i)lcur->flags|=LBXGL_LFL_LINEAR;

			i=LBXGL_SEnt_GetNum(cur, "noshadows");
			if(i)lcur->flags|=LBXGL_LFL_NOSHADOW;

			i=LBXGL_SEnt_GetNum(cur, "lightvol");
			if(i)lcur->flags|=LBXGL_LFL_LIGHTVOL;

			s=LBXGL_SEnt_GetStr(cur, "texture");
			if(s)
			{
				lcur->texname=dystrdup(s);
				lcur->tex_falloff=LBXGL_Texture_LoadImage(s);
			}

			s=LBXGL_SEnt_GetStr(cur, "style");
			if(s)
			{
				i=atoi(s);
				if(!strcmp(s, "Slow Pulse"))i=1;
				if(!strcmp(s, "Slow Strobe"))i=2;
				lcur->style=i;
			}

			i=LBXGL_SEnt_GetVec3(cur, "direction", lcur->dir);
			if(i>=0)lcur->flags|=LBXGL_LFL_SPOTLIGHT;
			lcur->angle=LBXGL_SEnt_GetNum(cur, "angle");

			lcur->flags|=LBXGL_LFL_STATIC;

//			if(lcur->curval>=350)
//			{
//				lcur->flags|=LBXGL_LFL_ENVGLOSS;
//				lcur->flags|=LBXGL_LFL_ENVRENDER;
//			}

//			lcur->next=world->light;
//			world->light=lcur;
//			cur->light=lcur;
		}

		if(!strcmp(ty, "sky_camera"))
//		if(!strcmp(ty, "sky_camera") || !strcmp(ty, "info_intermission"))
		{
			LBXGL_SEnt_GetVec3(cur, "origin", world->skycam_org);
			world->skycam=1;
		}

#if 0
		if(!strcmp(ty, "monster_zombie"))
		{
			if(!cur->mdl)
			{
				cur->mdl=LBXGL_Mdl_LoadModelInstance(
					"human2/human2");

				if(cur->mdl)
				{
					LBXGL_Mdl_SetAnim(cur->mdl, "cruc");

					cur->mdl->next=world->mesh;
					world->mesh=cur->mdl;
				}
			}

			cur=cur->next;
			continue;
		}
#endif

#if 0
		if(!strncmp(ty, "monster", 7))
		{
			if(!cur->mdl)
			{
				cur->mdl=LBXGL_Mdl_LoadModelInstance("objects/monitor31");

				if(cur->mdl)
				{
					cur->mdl->next=world->mesh;
					world->mesh=cur->mdl;
				}
			}
		}
#endif

#if 0
		if(!strcmp(ty, "rocket"))
		{
			if(!cur->mdl)
			{
				cur->mdl=LBXGL_Mdl_LoadModelInstance("projectile/rocket22");

				if(cur->mdl)
				{
					cur->mdl->next=world->mesh;
					world->mesh=cur->mdl;
				}
			}
		}
#endif

#if 0
		if(!strncmp(ty, "light_flame", 11))
		{
			if(!cur->mdl)
			{
				cur->mdl=LBXGL_Mdl_LoadModelInstance("objects/flame");
				if(cur->mdl)
				{
					cur->mdl->next=world->mesh;
					world->mesh=cur->mdl;
				}
			}
		}

		if(!strncmp(ty, "light_torch", 11))
		{
			if(!cur->mdl)
			{
				cur->mdl=LBXGL_Mdl_LoadModelInstance("objects/torch");
				if(cur->mdl)
				{
					cur->mdl->next=world->mesh;
					world->mesh=cur->mdl;
				}
			}
		}
#endif


		cur=cur->next;
	}


	if(main_sun)
	{
#if 0
		lcur=sun;
		if(!lcur)
		{
			sun=lbxEntNewLightWorld(world);
			lcur=sun;
		}

		g=0.57735;
		V3F_SET(lcur->clr, g, g, g);

		lcur->org[0]=cam->org[0];
		lcur->org[1]=cam->org[1];
		lcur->org[2]=cam->org[2]+16384;

		lcur->val[0]=65536;
		lcur->val[1]=0;

		lcur->curval=lcur->val[0];
#endif
	}else
	{
		sun=NULL;
	}
}

void MAP_LoadFileServer(LBXGL_BrushWorld *wrl, char *name)
{
	gcDelayGC(2);
	btWorldTeardown(sv_world);
//	BTGE_BrushMap_LoadFile(sv_world, name);
	btsvLoadMapFile(sv_world, name);
	btSpawnWorld();
	gcDelayGC(0);
}

void MAP_LoadFile(LBXGL_BrushWorld *wrl, char *name)
{
	dyt t;

	BTGE_Net_Init();

	if(map_doLoadType==1)
	{
		MAP_LoadFileServer(wrl, name);
		return;
	}

	gcDelayGC(2);
//	PDGL_Sound_StopPlaying(-1);	//stop all sounds
	btclFlushWorld(cl_world);
	btWorldTeardown(sv_world);
	BTGE_Net_ClientFlush();

//	LBXGL_BrushMap_LoadFile(world, name);
	btclLoadMapFile(cl_world, name);

//	BTGE_BrushMap_LoadFile(sv_world, name);
	btsvLoadMapFile(sv_world, name);
//	spawn_ents(world);
	btSpawnWorld();

	setup_light_ents(world->entity);

#if 0
//	player=btNewEntity();
	player=btSpawnPlayer();
	vfvec3(cam->org, player->origin);
	vfvec3(cam->ang, player->angles);
	cam->org[2]+=48;
	cam->ang[2]-=90;
#endif

	t=dylist1s("connect");
//	t=dylist2s("cldelta", t);
	t=dylist1(t);
	BTGE_Net_ClientSendUpdate(t);

	gcDelayGC(0);
}

void MAP_SaveFile(LBXGL_BrushWorld *wrl, char *name)
{
	LBXGL_BrushMap_SaveFile(world, name);
}

void MAP_LoadGame(BTGE_BrushWorld *wrl, char *name)
{
//	MAP_LoadFile(wrl, name);
}

void MAP_SaveGame(BTGE_BrushWorld *wrl, char *name)
{
	btWorldSaveGame(wrl, name);
//	BTGE_SaveGame_SaveWorld(wrl, name);
//	LBXGL_BrushMap_SaveFile(world, name);
}

int ConCmds_Connect(char **a)
{
	dyt t;

	if(a[1])
	{
		BTGE_Net_ClientConnect(a[1]);
	}else
	{
		BTGE_Net_ClientDisconnect();
	}

	gcDelayGC(2);
	t=dylist1s("connect");
//	t=dylist2s("cldelta", t);
	t=dylist1(t);
	BTGE_Net_ClientSendUpdate(t);
	gcDelayGC(0);
}

int ConCmds_Load(char **a)
{
	MAP_LoadFile(world, a[1]);
	return(0);
}

int ConCmds_Save(char **a)
{
	MAP_SaveFile(world, a[1]);
	return(0);
}

int ConCmds_Map(char **a)
{
	char tb[256];

	btCvarSet("mapname", a[1]);

	sprintf(tb, "maps/%s.map", a[1]);
//	MAP_LoadFile(world, tb);
	map_doLoad=dystrdup(tb);
	map_doLoadStep=0;
	map_doLoadType=0;
	return(0);
}

int ConCmds_DedicatedMap(char **a)
{
	char tb[256];

	btCvarSet("mapname", a[1]);

	sprintf(tb, "maps/%s.map", a[1]);
//	MAP_LoadFile(world, tb);
	map_doLoad=dystrdup(tb);
	map_doLoadStep=0;
	map_doLoadType=1;
	return(0);
}

int ConCmds_ListenMap(char **a)
{
	char tb[256];

	btCvarSet("mapname", a[1]);

	sprintf(tb, "maps/%s.map", a[1]);
//	MAP_LoadFile(world, tb);
	map_doLoad=dystrdup(tb);
	map_doLoadStep=0;
	map_doLoadType=2;
	return(0);
}

int ConCmds_LoadGame(char **a)
{
	char tb[256];

//	btCvarSet("mapname", a[1]);

	sprintf(tb, "saves/%s.sav", a[1]);
//	MAP_LoadGame(world, tb);
	MAP_LoadFile(world, tb);
//	map_doLoad=dystrdup(tb);
//	map_doLoadStep=0;
	return(0);
}

int ConCmds_SaveGame(char **a)
{
	char tb[256];

//	btCvarSet("mapname", a[1]);

	sprintf(tb, "saves/%s.sav", a[1]);
	MAP_SaveGame(sv_world, tb);
//	map_doLoad=dystrdup(tb);
//	map_doLoadStep=0;
	return(0);
}

int ConCmds_Sky(char **a)
{
	LBXGL_Sky_LoadSky(a[1]);
	return(0);
}

int ConCmds_Sky2(char **a)
{
	LBXGL_Sky_LoadSkyNum(a[1], 1);
	return(0);
}

int ConCmds_Sky3(char **a)
{
	LBXGL_Sky_LoadSkyNum(a[1], 2);
	return(0);
}

int ConCmds_Sky4(char **a)
{
	LBXGL_Sky_LoadSkyNum(a[1], 3);
	return(0);
}

int ConCmds_Ent(char **a)
{
	char *s;

	if(!a[1])
	{
		Con_Printf("ent <property> [value]\n");
		return(0);
	}

	if(!world->entity_sel)
	{
		Con_Printf("no entity selected\n");
		return(0);
	}

	if(a[2])
	{
		LBXGL_SEnt_SetStr(world->entity_sel, a[1], a[2]);
		return(0);
	}else
	{
		s=LBXGL_SEnt_GetStr(world->entity_sel, a[1]);
		if(s)Con_Printf("\"%s\" \"%s\"\n", a[1], s);
			else printf("no property \"%s\"\n", a[1]);
		return(0);
	}

	return(0);
}

int ConCmds_Shadows(char **a)
{
	if(!a[1])
	{
		Con_Printf("Shadows=%d\n", world->shadows);
		return(0);
	}

	world->shadows=atoi(a[1]);
	return(0);
}

int ConCmds_Gamma(char **a)
{
	if(!a[1])
	{
		Con_Printf("Gamma=%f\n", world->basegamma);
		return(0);
	}

	world->basegamma=atof(a[1]);
	world->curgamma=world->basegamma;
	return(0);
}

int ConCmds_Lighting(char **a)
{
	if(!a[1])
	{
		Con_Printf("Lighting=%d\n", world->lighting);
		return(0);
	}

	world->lighting=atoi(a[1]);
	return(0);
}

int ConCmds_Timestat(char **a)
{
	if(!a[1])
	{
		Con_Printf("timestat=%d\n", main_timestat);
		return(0);
	}

	main_timestat=atoi(a[1]);
	return(0);
}

int ConCmds_MemLog(char **a)
{
	if(!a[1])
	{
//		Con_Printf("memlog=%d\n", main_timestat);
		return(0);
	}

	gcDelayGC(atoi(a[1])?4:0);
	return(0);
}

int ConCmds_Eval(char **a)
{
	char tb[1024];
	char **ap;
	dyt p;
	char *s, *t;
	int i, fl;
	
	//reverse the 'split' operation
	t=tb; ap=a+1;
	while(*ap)
	{
		s=*ap++;
		if(s[strlen(s)+1]=='\"')
		{
			//quoted strings don't transform escapes
			//so just re-emit the quoted string
			*t++=' ';
			*t++='\"';
			while(*s)*t++=*s++;
			*t++='\"';
			continue;
		}
		
		*t++=' ';
		while(*s)*t++=*s++;
	}
	*t++=';';	//for good measure
	*t++=';';
	*t++=0;

	dyPrintf("Eval: %s\n", tb);

	fl=0;
	if(btCvarGeti("vm_debugstep"))
		fl|=BSVM_VMGFL_DEBUGSTEP;

	p=BSVM_EvalStringFlags(tb, fl);
	dyPrint(p);
	dyPrintf("\n");
}

int BTCL_ServerStuffCmd(char **a)
{
	char tb[1024];
	char **ap;
	dyt p;
	char *s, *t;
	int i;
	
	//reverse the 'split' operation
	t=tb; ap=a;
	while(*ap)
	{
		s=*ap++;
		if(s[strlen(s)+1]=='\"')
		{
			//quoted strings don't transform escapes
			//so just re-emit the quoted string
			*t++=' ';
			*t++='\"';
			while(*s)*t++=*s++;
			*t++='\"';
			continue;
		}
		
		*t++=' ';
		while(*s)*t++=*s++;
	}
//	*t++=';';	//for good measure
//	*t++=';';
	*t++=0;

//	dyPrintf("StuffCmd: %s\n", tb);

	if(cl_world)
	{
		p=dylist3s("stuffcmd",
			dyint(cl_world->camnum),
			dystring(tb));
		BTGE_Net_ClientSendUpdate(dylist1(p));
	}

//	p=BSVM_EvalString(tb);
//	dyPrint(p);
//	dyPrintf("\n");
}

int ConCmds_SV(char **a)
{
	if(!a[1])
	{
		Con_Printf("Usage: sv <command>\n");
		Con_Printf("Passes command to server\n");
		return(0);
	}

	BTCL_ServerStuffCmd(a+1);
//	BT_ClientConCmd(player, a+1);
	return(0);
}

int ConCmds_SV2(char **a)
{
	BTCL_ServerStuffCmd(a);
//	BT_ClientConCmd(player, a);
	return(0);
}

int main_new()
{
//	map_name=NULL;
	MapEdit_NewWorld(world);
	return(0);
}

#if 0
int main_new()
{
	float v[3];
	LBXGL_Brush *brsh;

	LBXGL_BrushWorld_DrawFlush(world);

	world->map_name=NULL;
	world->entity=NULL;
	world->brush=NULL;

	world->entity_psel=LBXGL_SEnt_NewEntity(&world->entity, "worldspawn");

//	world->brush=make_cube_default();

	brsh=LBXGL_Brush_MakeCube(0, 0, -64, 128, 128, 6, world->default_tex);
	LBXGL_BrushWorld_AddBrush(world, brsh);

	brsh=LBXGL_Brush_MakeCube(0, 0, 64, 128, 128, 6, world->default_tex);
	LBXGL_BrushWorld_AddBrush(world, brsh);

	brsh=LBXGL_Brush_MakeCube(-128, 0, 0, 6, 128, 64, world->default_tex);
	LBXGL_BrushWorld_AddBrush(world, brsh);

	brsh=LBXGL_Brush_MakeCube(128, 0, 0, 6, 128, 64, world->default_tex);
	LBXGL_BrushWorld_AddBrush(world, brsh);

	brsh=LBXGL_Brush_MakeCube(0, -128, 0, 128, 6, 64, world->default_tex);
	LBXGL_BrushWorld_AddBrush(world, brsh);

	brsh=LBXGL_Brush_MakeCube(0, 128, 0, 128, 6, 64, world->default_tex);
	LBXGL_BrushWorld_AddBrush(world, brsh);



	V3F_ZERO(v);
	LBXGL_SEnt_NewBasicLight(&world->entity, v, 1000);


	V3F_SET(v, 0, -100, -64+36);
	LBXGL_SEnt_NewBasicEntity(&world->entity, "info_player_start", v);

	return(0);
}
#endif


#if 1
int pdgl_main_serverFrame()
{
	dyt l0;
	s64 m;
	int i;
	bool b;

	return(0);

#if 1
#if 1
	i=0;
	l0=BTGE_Net_ServerGetUpdate(&i);
	while(l0)
	{
//		dysPrintln(l0);

		btDecodeClientUpdate(l0, state->dt_f, i);
		btFreeUpdate(l0);
		l0=BTGE_Net_ServerGetUpdate(&i);
	}
#endif

	b=btStepWorld(state->dt_f);

//	l0=btComposeUpdate(0);
//	BTGE_Net_ServerSendUpdate(0, l0);

#if 1
	if(b)
	{
		//send updates to all connected clients
		m=BTGE_Net_ServerGetClientMask();
//		dyPrintf("client mask %d\n", (int)m);

		l0=btComposeUpdate(0);
		BTGE_Net_ServerSendUpdate(0, l0);
		
		for(i=0; i<32; i++)
			if(m&(1LL<<i))
		{
			l0=btComposeUpdate(i+1);
			BTGE_Net_ServerSendUpdate(i+1, l0);
		}
		btFinishUpdate();
	}
#endif

#endif
}
#endif

#if 1
int Main_ServerFrame(float dt)
{
	dyt l0;
	s64 m;
	int i;
	bool b;

#if 1
#if 1
	i=0;
	l0=BTGE_Net_ServerGetUpdate(&i);
	while(l0)
	{
//		dysPrintln(l0);

		btDecodeClientUpdate(l0, dt, i);
		btFreeUpdate(l0);
		l0=BTGE_Net_ServerGetUpdate(&i);
	}
#endif

	b=btStepWorld(dt);

//	l0=btComposeUpdate(0);
//	BTGE_Net_ServerSendUpdate(0, l0);

#if 1
	if(b)
	{
		//send updates to all connected clients
		m=BTGE_Net_ServerGetClientMask();
//		dyPrintf("client mask %d\n", (int)m);

		l0=btComposeUpdate(0);
		BTGE_Net_ServerSendUpdate(0, l0);
		
		for(i=0; i<32; i++)
			if(m&(1LL<<i))
		{
			l0=btComposeUpdate(i+1);
			BTGE_Net_ServerSendUpdate(i+1, l0);
		}
		btFinishUpdate();
	}
#endif

#endif
}
#endif

int Main_ServerThread(void *data)
{
	int ct, lt, dt, ds, lds;
	float dt_f;

	lt=PDGL_TimeMS(); lds=0;
//	lt=pdgl_time;
	while(1)
	{
		ds=state->doshot;
		if(ds&32)
		{
			if(lds&32)
			{
				lds=ds;
				ct=pdgl_time;
				dt=ct-lt;
			}else
			{
				lds=ds;
				ct=pdgl_time;
				lt=ct;
				dt=0;
			}
		}else
		{
			if(!(lds&32))
			{
				lds=ds;
				ct=PDGL_TimeMS();
				dt=ct-lt;
			}else
			{
				lds=ds;
				ct=pdgl_time;
				lt=ct;
				dt=0;
			}
		}

		if(dt<10)
		{
			thSleep(10);
			continue;
		}
		
		lt=ct;
		dt_f=dt/1000.0;
		
		Main_ServerFrame(dt_f);
	}
}

char *games[16], *start_map;
int n_games;

//called before main window creation
int pdgl_main_prestart(int argc, char *argv[])
{
	char tb[256];
	char *opts[4];
	char *s;
	void *p;
	int xs, ys, fsm, fxs, fys;
	int i, j, k, l;

	xs=800; ys=600;

#if 1
	n_games=0; start_map=NULL;
	for(i=1; i<argc; i++)
	{
		dyPrintf("%d: %s\n", i, argv[i]);
		
		if(!strcmp(argv[i], "-game"))
			games[n_games++]=argv[i+1];
		if(!strcmp(argv[i], "+map"))
			start_map=argv[i+1];
		if(!strcmp(argv[i], "+devmap"))
			start_map=argv[i+1];

		if(!strcmp(argv[i], "-width"))
			xs=atoi(argv[i+1]);
		if(!strcmp(argv[i], "-height"))
			ys=atoi(argv[i+1]);

		if(!strcmp(argv[i], "+set") && !strcmp(argv[i+1], "game"))
			games[n_games++]=argv[i+2];
		if(!strcmp(argv[i], "+set") && !strcmp(argv[i+1], "fs_game"))
			games[n_games++]=argv[i+2];
	}
#endif

#if 1
	dyllAddLibraryPath("resource");
	dyllLoadLibrary("game_base");

	for(i=0; i<n_games; i++)
	{
		dyPrintf("add gamedir: %s\n", games[i]);
		vf_mount(games[i], "/", "dir", NULL);
		dyllAddLibraryPathFront(games[i]);
		
		sprintf(tb, "game_%s", games[i]);
		dyllLoadLibrary(tb);
	}
#endif

	Con_Init();
	ConCmds_Init();

#if 1
	btCvarSetf("deathmatch", 0);
	btCvarSetf("coop", 0);

	btCvarSetf("sv_cheats", 0);
	btCvarSetf("sv_nomonsters", 0);
	btCvarSetf("sv_gravity", 800);

	btCvarSetf("cl_showentids", 0);
	btCvarSetf("r_lightbudget", 0);
	btCvarSetf("r_noearlyout", 0);
	btCvarSetf("r_nohires", 0);
	btCvarSetf("r_skipfinal", 0);
	btCvarSetf("r_skipdark", 0);
	btCvarSetf("r_skiplight", 0);
	btCvarSetf("r_skipalpha", 0);

	btCvarSet("r_title", "BGBTech");
	btCvarSetf("r_width", xs);
	btCvarSetf("r_height", ys);
	btCvarSetf("r_fullscreen", 0);

	btCvarSetf("r_usefbo", 1);
	btCvarSetf("r_usehdr", 1);
	btCvarSetf("r_usebloom", 0);
	btCvarSetf("r_parallax", 1);
	btCvarSetf("r_cellshade", 0);
	btCvarSetf("r_anaglyph", 0);
#endif

	btCvarSetf("r_exposure_force", 0);
	btCvarSetf("r_exposure_scale", 1);

	btCvarSetf("cl_showmenu", 1);


	bsvmAddScriptPath("game");

	Con_ExecCmdBuf("game/default.cfg");
	Con_DoExecCmds();

	Con_ExecCmdBuf("game/config.cfg");
	Con_DoExecCmds();

//	Con_ExecCmdBuf("game/autoexec.cfg");

	s=btCvarGet("r_title");
	xs=btCvarGetf("r_width");
	ys=btCvarGetf("r_height");
	fsm=btCvarGetf("r_fullscreen");
	fxs=btCvarGetf("r_fullscreen_width");
	fys=btCvarGetf("r_fullscreen_height");

	GfxDrv_SetDefaults(s, xs, ys);
	GfxDrv_SetDefaultFullscreen(fsm);
	GfxDrv_SetFullscreenResolution(fxs, fys);
//	Con_ReInit();

	return(1);
}

int pdgl_main_startup(int argc, char *argv[])
{
	char tb[256];
	char *opts[4];
//	char *games[16], *map;
	char *s;
	void *p;
//	int n_games;
	int i, j, k, l;

	GfxFont_Init();
//	Con_Init();
//	ConCmds_Init();

	Con_ReInit();

#if 0
	n_games=0; map=NULL;
	for(i=1; i<argc; i++)
	{
		dyPrintf("%d: %s\n", i, argv[i]);
		
		if(!strcmp(argv[i], "-game"))
			games[n_games++]=argv[i+1];
		if(!strcmp(argv[i], "+map"))
			map=argv[i+1];
		if(!strcmp(argv[i], "+devmap"))
			map=argv[i+1];

		if(!strcmp(argv[i], "+set") && !strcmp(argv[i+1], "game"))
			games[n_games++]=argv[i+2];
		if(!strcmp(argv[i], "+set") && !strcmp(argv[i+1], "fs_game"))
			games[n_games++]=argv[i+2];
	}
#endif

#if 0
	if(game)
	{
		dyPrintf("gamedir=%s\n", game);
		vf_mount(game, "/", "dir", NULL);
	}
#endif

#if 0
	dyllLoadLibrary("game_base");

	for(i=0; i<n_games; i++)
	{
		dyPrintf("add gamedir: %s\n", games[i]);
		vf_mount(games[i], "/", "dir", NULL);
		
		sprintf(tb, "game_%s", games[i]);
		dyllLoadLibrary(tb);
	}
#endif

//	PDGL_InitASM(argc, argv);
//	ccLoadModule("scripts/e_mapper2.c");
//	ccLoadModule("lbxgl2.h");

	PDGLUI_Init();
	UI_Camera_Init();

	PDGL_Sound_Init();

//	PDGL_Sound_PlayPointSample("sounds/alarm/crashbuz", PDGL_SOUND_LOOP,
//		1.0, 0.1, VEC3(0, 0, 0), VEC3(0, 0, 0));
//	PDGL_Sound_PlayPointSample("sound/world/klaxon1", PDGL_SOUND_LOOP,
//		1.0, 0.1, VEC3(0, 0, 0), VEC3(0, 0, 0));

	tex_screen=Tex_ScreenshotTexture();
	LBXGL_Particle_Init();

	p=dyllGetAddr("mapper_init");
	if(p) { ((int(*)(int argc, char **argv))p)(argc, argv); }

	ConCmds_Register("ent", "Get/Set entity property", ConCmds_Ent);
	ConCmds_Register("load", "Load map file", ConCmds_Load);
	ConCmds_Register("save", "Save map file", ConCmds_Save);

	ConCmds_Register("loadgame", "Load savegame file", ConCmds_LoadGame);
	ConCmds_Register("savegame", "Save savegame file", ConCmds_SaveGame);

	ConCmds_Register("map", "Load map", ConCmds_Map);
	ConCmds_Register("sky", "Set sky", ConCmds_Sky);
	ConCmds_Register("sky2", "Set sky 2", ConCmds_Sky2);
	ConCmds_Register("sky3", "Set sky 3", ConCmds_Sky3);
	ConCmds_Register("sky4", "Set sky 4", ConCmds_Sky4);
	ConCmds_Register("shadows", "Set Shadows", ConCmds_Shadows);
	ConCmds_Register("gamma", "Set Gamma", ConCmds_Gamma);
	ConCmds_Register("lighting", "Set Lighting", ConCmds_Lighting);
	ConCmds_Register("timestat", "Set Timestat", ConCmds_Timestat);

	ConCmds_Register("memlog", "Set Memlog", ConCmds_MemLog);

	ConCmds_Register("dedicated", "Load map", ConCmds_DedicatedMap);
	ConCmds_Register("listen", "Load map", ConCmds_ListenMap);
	ConCmds_Register("connect", "Connect to server", ConCmds_Connect);

	ConCmds_Register("eval", "Eval Expression", ConCmds_Eval);
	ConCmds_Register("bs", "Eval Expression", ConCmds_Eval);
	ConCmds_Register(";", "Eval Expression", ConCmds_Eval);

	ConCmds_Register("sv", "Pass Command to Server", ConCmds_SV);
	ConCmds_Register("invul", "Invulnerability", ConCmds_SV2);
	ConCmds_Register("notarget", "Notarget", ConCmds_SV2);
	ConCmds_Register("noclip", "Noclip", ConCmds_SV2);
	ConCmds_Register("creative", "Creative", ConCmds_SV2);
	ConCmds_Register("give", "Give", ConCmds_SV2);
	ConCmds_Register("camera", "Camera State", ConCmds_SV2);
	ConCmds_Register("fastmp", "Fast MP Regen", ConCmds_SV2);
	ConCmds_Register("placervox", "Set Placer Voxel", ConCmds_SV2);
	ConCmds_Register("speak", "Say Something", ConCmds_SV2);

	ConCmds_Register("guimode", "GUI Mode", ConCmds_SV2);
	ConCmds_Register("clientmove", "Client Movement", ConCmds_SV2);

#if 0
	btCvarSetf("deathmatch", 0);
	btCvarSetf("coop", 0);
	btCvarSetf("sv_cheats", 0);
	btCvarSetf("cl_showentids", 0);
	btCvarSetf("r_lightbudget", 0);
	btCvarSetf("r_noearlyout", 0);
	btCvarSetf("r_nohires", 0);
#endif

	UI_TexFinder_SetTitle("Textures");


	cam->org[0]=0;
	cam->org[1]=-10;
	cam->org[2]=16;

//	cam->ang[0]=90;
//	cam->ang[1]=0;
//	cam->ang[2]=0;


#if 0

	PDGLUI_BeginMenu("menu");

	PDGLUI_BeginSubMenu("&New Game");
	PDGLUI_MenuItem("ng_peaceful",		"Peaceful");
	PDGLUI_MenuItem("ng_easy",			"Easy");
	PDGLUI_MenuItem("ng_medium",		"Medium");
	PDGLUI_MenuItem("ng_hard",			"Hard");
	PDGLUI_MenuItem("ng_nightmare",		"Nightmare");
	PDGLUI_EndSubMenu();

	PDGLUI_BeginSubMenu("&Load Game");
	PDGLUI_MenuItem("sv_load1",			"Game 1");
	PDGLUI_MenuItem("sv_load2",			"Game 2");
	PDGLUI_MenuItem("sv_load3",			"Game 3");
	PDGLUI_MenuItem("sv_load4",			"Game 4");
	PDGLUI_EndSubMenu();

	PDGLUI_BeginSubMenu("&Save Game");
	PDGLUI_MenuItem("sv_save1",			"Game 1");
	PDGLUI_MenuItem("sv_save2",			"Game 2");
	PDGLUI_MenuItem("sv_save3",			"Game 3");
	PDGLUI_MenuItem("sv_save4",			"Game 4");
	PDGLUI_EndSubMenu();

#if 0

	PDGLUI_BeginSubMenu("&File");
	PDGLUI_MenuItem("new",		"&New           (CTRL--N)");
	PDGLUI_MenuItem("open",		"&Open          (CTRL--O)");
	PDGLUI_MenuItem("save",		"&Save          (CTRL--S)");
	PDGLUI_MenuItem("save_as",	"Save &As");
	PDGLUI_EndSubMenu();

	PDGLUI_BeginSubMenu("&Edit");
	PDGLUI_MenuItem("edit_undo",	"Undo           (CTRL--Z)");
	PDGLUI_MenuItem("edit_redo",	"Redo           (CTRL--R)");
	PDGLUI_MenuItem(NULL, NULL);
	PDGLUI_MenuItem("edit_cut",	"Cut            (CTRL--X)");
	PDGLUI_MenuItem("edit_copy",	"Copy           (CTRL--C)");
	PDGLUI_MenuItem("edit_paste",	"Paste          (CTRL--V)");
	PDGLUI_MenuItem("edit_dup",	"Duplicate      (CTRL--D)");
	PDGLUI_EndSubMenu();


	PDGLUI_BeginSubMenu("Create");
	PDGLUI_MenuItem("create_cube",	"Cube");
	p=BASM_GetPtr("mapper_menu_create");
	if(p) ((int (*)())p)();
	PDGLUI_EndSubMenu();


	PDGLUI_BeginSubMenu("Command");
	PDGLUI_MenuItem("cmd_delete", 	"Delete Selection        (DEL)");
	PDGLUI_MenuItem("cmd_settex",	"Set Texture             (CTRL--T)");

	p=BASM_GetPtr("mapper_menu_command");
	if(p) ((int (*)())p)();
	PDGLUI_EndSubMenu();
#endif


	PDGLUI_BeginSubMenu("Help");
	PDGLUI_MenuItem("help_generic", "General");
	PDGLUI_MenuItem("help_about", "About");
	PDGLUI_EndSubMenu();

	p=BASM_GetPtr("mapper_menu_top");
	if(p) ((int (*)())p)();

	PDGLUI_EndMenu();

#endif


	world=LBXGL_Brush_NewWorld();
	sv_world=BTGE_Brush_NewWorld();
	cl_world=btclNewWorld();
	btBindWorld(sv_world);
	btclBindWorld(cl_world);
	btclBindBrushWorld(world);

	LBXGL_BrushDraw2D_BindWorld(world);

	world->grid_snap=1;

	lbxgl_brushmesh_world=world;

	lbxgl_brushmesh_mdl=gctalloc(
		"lbxgl_meshgroup_t", sizeof(LBXGL_MeshGroup));
	lbxgl_brushmesh_mode=LBXGL_EDMODE_BRUSH;

	p=BASM_GetPtr("game_base_init");
	if(p) ((int (*)())p)();

	for(i=0; i<n_games; i++)
	{
		sprintf(tb, "game_%s_init", games[i]);
		p=BASM_GetPtr(tb);
		if(p) ((int (*)())p)();
	}


//	bsvmAddScriptPath("game");
	Con_ExecCmdBuf("game/autoexec.cfg");

	if(start_map)
	{
		sprintf(tb, "maps/%s.map", start_map);
		MAP_LoadFile(world, tb);
	}else
	{
//		main_new();
	}

	world->lighting=1;		//enable lighting
	cam->raw=0;				//direct control
	cam->mgrab=1;			//mouse grab
//	cam->speed=25*12;		//25 ft/s / ~ 17MPH
	cam->speed=25*8;		//25 ft/s / ~ 17MPH
//	cam->speed=19*8;		//19 ft/s / ~ 13MPH
	cam->lockcfg=1;			//lock camera config
	cam->org[3]=-40;		//move along W axis

	if(cl_world)
	{
		V3F_SET(cl_world->view_mins,
			-(state->xs/2), -(state->ys/2), -4096);
		V3F_SET(cl_world->view_maxs, 
			 (state->xs/2),  (state->ys/2),  4096);
		cl_world->view_fov=90;
	}

	thThread(Main_ServerThread, NULL);

	return(1);
}

int main_handle_input()
{
	char buf[256];
	float mins[3], maxs[3];
	float io[3], iv[4], ia[8], fw[2], pt[4], f, g, v;
	float im[16], im1[16], ir[9], *imv;

	LBXGL_Brush *brsh;
	LBXGL_MeshPrim *prim;
	void *p;
	int i, j, k, imfl;
	unsigned short *kcur;
	LBXGL_SEntPair *epcur;
	dyt l0;
	char *s;

	if(Con_IsDown() || ui_state || PDGLUI_HasFocus() ||
		UI_MetaUI_IsActive())
	{
		if(Con_IsDown())
			{ PDGLUI_HideMenu(NULL); }

		return(0);
	}

//	if(state->mb&4)
	if(PDGL_KeyDown(K_ALT) && (state->mb&4))
	{
		PDGLUI_ShowMenu("menu", state->mx, -state->my);
		cam->mgrab=0;			//disable mouse grab
		return(0);
	}
	
//	if(!PDGLUI_MenuShowP("menu"))
	if(!PDGLUI_MenuShowP(NULL))
	{
		cam->mgrab=1;			//enable mouse grab
	}

	i=btclHandleInput(cl_world);
	if(i>0)
	{
		cam->mgrab=0;			//disable mouse grab
		return(0);	
	}

	i=LBXGL_BrushDraw2D_HandleInput(world);
	if(i>0)
	{
		cam->mgrab=0;			//disable mouse grab
		return(0);	
	}

	if(MapEdit_IsEdit())
	{
		cam->mgrab=0;			//disable mouse grab
	}

	s=PDGLUI_GetMenuSelection("menu");
	if(cl_world && !cl_world->camnum)		//not in-game, no mouse grab
	{
		if(!s)PDGLUI_ShowMenu("menu", 0, 0);

		cam->mgrab=0;
	}

	if(btCvarGetf("cl_showmenu")==0)
		{ PDGLUI_HideMenu(NULL); }

//	s=PDGLUI_GetMenuSelection("menu");
	if(s)
	{
		if(!strcmp(s, "new"))
		{
			main_new();
		}

		if(!strcmp(s, "open"))
		{
			UI_LoadSave_SetTitle("Open", "map;MAP");
			ui_state=2;
		}
		if(!strcmp(s, "save_as"))
		{
			UI_LoadSave_SetTitle("Save As", "map;MAP");
			ui_state=3;
		}
		if(!strcmp(s, "cmd_settex"))
		{
			UI_TexFinder_SetTitle("Textures");
			ui_state=1;
		}

		if(!strcmp(s, "save"))
		{
			if(world->map_name)
				MAP_SaveFile(world, world->map_name);
		}

		if(!strcmp(s, "edit_undo"))MapEdit_CmdUndo();
		if(!strcmp(s, "edit_redo"))MapEdit_CmdRedo();

		if(!strcmp(s, "edit_cut"))MapEdit_CmdSel(LBXGL_EDCMD_CUT);
		if(!strcmp(s, "edit_copy"))MapEdit_CmdSel(LBXGL_EDCMD_COPY);
		if(!strcmp(s, "edit_paste"))LBXGL_BrushMesh_Paste();
		if(!strcmp(s, "edit_dup"))MapEdit_CmdSel(LBXGL_EDCMD_DUPLICATE);

		if(!strcmp(s, "mode_group"))
			{ lbxgl_brushmesh_mode=LBXGL_EDMODE_GROUP; MapEdit_ClearSel(); }
//		if(!strcmp(s, "mode_object"))
//			{ lbxgl_brushmesh_mode=LBXGL_EDMODE_OBJECT; MapEdit_ClearSel(); }
		if(!strcmp(s, "mode_face"))
			{ lbxgl_brushmesh_mode=LBXGL_EDMODE_FACE; MapEdit_ClearSel(); }
		if(!strcmp(s, "mode_vertex"))
			{ lbxgl_brushmesh_mode=LBXGL_EDMODE_VERTEX; MapEdit_ClearSel(); }
		if(!strcmp(s, "mode_brush"))
			{ lbxgl_brushmesh_mode=LBXGL_EDMODE_BRUSH; MapEdit_ClearSel(); }

		if(!strcmp(s, "mode_object"))
		{
			if((lbxgl_brushmesh_mode!=LBXGL_EDMODE_OBJECT) &&
				(lbxgl_brushmesh_mode!=LBXGL_EDMODE_TEXTURE))
					MapEdit_ClearSel();
			lbxgl_brushmesh_mode=LBXGL_EDMODE_OBJECT;
		}
		if(!strcmp(s, "mode_texture"))
		{
			if((lbxgl_brushmesh_mode!=LBXGL_EDMODE_OBJECT) &&
				(lbxgl_brushmesh_mode!=LBXGL_EDMODE_TEXTURE))
					MapEdit_ClearSel();
			lbxgl_brushmesh_mode=LBXGL_EDMODE_TEXTURE;
		}

		if(!strcmp(s, "mode_persp"))cam->mode=PDGLUI_CAM_PERSP;
		if(!strcmp(s, "mode_ortho"))cam->mode=PDGLUI_CAM_ORTHO;
		if(!strcmp(s, "mode_orth_xy"))cam->mode=PDGLUI_CAM_XY;
		if(!strcmp(s, "mode_orth_xz"))cam->mode=PDGLUI_CAM_XZ;
		if(!strcmp(s, "mode_orth_yz"))cam->mode=PDGLUI_CAM_YZ;
		if(!strcmp(s, "mode_cad4v"))cam->mode=PDGLUI_CAM_CAD4V;

		if(!strcmp(s, "tool_sel"))lbxgl_brushmesh_cmd=LBXGL_EDCMD_SEL;
		if(!strcmp(s, "tool_trans"))lbxgl_brushmesh_cmd=LBXGL_EDCMD_TRANS;
		if(!strcmp(s, "tool_rot"))lbxgl_brushmesh_cmd=LBXGL_EDCMD_ROT;
		if(!strcmp(s, "tool_scale"))lbxgl_brushmesh_cmd=LBXGL_EDCMD_SCALE;
		if(!strcmp(s, "tool_scale2"))lbxgl_brushmesh_cmd=LBXGL_EDCMD_SCALE2;

		if(!strcmp(s, "create_cube"))
		{
			MapEdit_AddUndo();
			LBXGL_BrushMesh_CreateCube();
		}

		if(!strcmp(s, "cmd_delete"))MapEdit_CmdSel(LBXGL_EDCMD_DELETE);
		if(!strcmp(s, "cmd_extrude"))MapEdit_CmdSel(LBXGL_EDCMD_EXTRUDE);
		if(!strcmp(s, "cmd_subdiv"))MapEdit_CmdSel(LBXGL_EDCMD_SUBDIVIDE);
		if(!strcmp(s, "cmd_merge"))MapEdit_CmdSel(LBXGL_EDCMD_MERGE);
		if(!strcmp(s, "cmd_invert"))MapEdit_CmdSel(LBXGL_EDCMD_INVERT);

		if(!strcmp(s, "tex_plane") && lbxgl_meshsel_prim)
			lbxgl_meshsel_prim->tex_mode=LBXGL_TEX_PLANAR;
		if(!strcmp(s, "tex_box") && lbxgl_meshsel_prim)
			lbxgl_meshsel_prim->tex_mode=LBXGL_TEX_BOX;
		if(!strcmp(s, "tex_sphere") && lbxgl_meshsel_prim)
			lbxgl_meshsel_prim->tex_mode=LBXGL_TEX_SPHERICAL;
		if(!strcmp(s, "tex_cylinder") && lbxgl_meshsel_prim)
			lbxgl_meshsel_prim->tex_mode=LBXGL_TEX_CYLINDRICAL;

		if(!strcmp(s, "tex_proj_xy") && lbxgl_meshsel_prim)
			LBXGL_Mesh_ReprojectPrimXY(lbxgl_meshsel_prim);
		if(!strcmp(s, "tex_proj_xz") && lbxgl_meshsel_prim)
			LBXGL_Mesh_ReprojectPrimXZ(lbxgl_meshsel_prim);

		if(!strcmp(s, "ng_easy"))
			Con_StuffCmd("newgame_easy");
		if(!strcmp(s, "ng_medium"))
			Con_StuffCmd("newgame_medium");
		if(!strcmp(s, "ng_hard"))
			Con_StuffCmd("newgame_hard");
		if(!strcmp(s, "ng_nightmare"))
			Con_StuffCmd("newgame_nightmare");
		if(!strcmp(s, "ng_peaceful"))
			Con_StuffCmd("newgame_peaceful");

		if(!strcmp(s, "sv_load1"))
			Con_StuffCmd("loadgame save1");
		if(!strcmp(s, "sv_load2"))
			Con_StuffCmd("loadgame save2");
		if(!strcmp(s, "sv_load3"))
			Con_StuffCmd("loadgame save3");
		if(!strcmp(s, "sv_load4"))
			Con_StuffCmd("loadgame save4");

		if(!strcmp(s, "sv_save1"))
			Con_StuffCmd("savegame save1");
		if(!strcmp(s, "sv_save2"))
			Con_StuffCmd("savegame save2");
		if(!strcmp(s, "sv_save3"))
			Con_StuffCmd("savegame save3");
		if(!strcmp(s, "sv_save4"))
			Con_StuffCmd("savegame save4");

//		p=BASM_GetPtr("mapper_menu_event");
//		if(p) ((int (*)(char *))p)(s);
	}

//	p=BASM_GetPtr("mapper_handle_input");
//	if(p) ((int (*)())p)();

	imfl=0;

	kcur=state->keys;
	while(*kcur)
	{
#if 1
		if(MapEdit_IsEdit() && PDGL_KeyDown(K_CTRL))
		{
			if(*kcur=='z')MapEdit_CmdUndo();
			if(*kcur=='y')MapEdit_CmdRedo();

			if(*kcur=='c')MapEdit_CmdSel(LBXGL_EDCMD_COPY);
			if(*kcur=='x')MapEdit_CmdSel(LBXGL_EDCMD_CUT);
			if(*kcur=='d')MapEdit_CmdSel(LBXGL_EDCMD_DUPLICATE);
			if(*kcur=='v')LBXGL_BrushMesh_Paste();

			if(*kcur=='s')
			{
				if(world->map_name)
					MAP_SaveFile(world, world->map_name);
			}

			if(*kcur=='b')
			{
				LBXGL_BrushWorld_DrawBuild(world);
			}

			if(*kcur=='t')
			{
				UI_TexFinder_SetTitle("Textures");
				ui_state=1;
			}
			
			if(*kcur=='P' && lbxgl_meshsel_prim)
				lbxgl_meshsel_prim->tex_mode=LBXGL_TEX_PLANAR;
			if(*kcur=='B' && lbxgl_meshsel_prim)
				lbxgl_meshsel_prim->tex_mode=LBXGL_TEX_BOX;
			if(*kcur=='S' && lbxgl_meshsel_prim)
				lbxgl_meshsel_prim->tex_mode=LBXGL_TEX_SPHERICAL;
			if(*kcur=='C' && lbxgl_meshsel_prim)
				lbxgl_meshsel_prim->tex_mode=LBXGL_TEX_CYLINDRICAL;

			kcur++;
			continue;
		}
#endif

#if 0
		if(MapEdit_IsEdit() && (*kcur==K_ENTER))
		{
			if(lbxgl_brushmesh_mode==LBXGL_EDMODE_BRUSH)
			{
				sel_brush_face();
				if(world->brush_sel_face<0)sel_ents();
				if((world->brush_sel_face<0) && !world->entity_sel)
					sel_brushes(world->brush);
			}else
			{
				if(!PDGL_KeyDown(K_SHIFT) && lbxgl_meshsel_multi)
					MapEdit_ClearSel();
				LBXGL_BrushMesh_SelGroup(lbxgl_brushmesh_mdl);
			}
		}
#endif

		if(!MapEdit_IsEdit())
		{
//			if(PDGL_KeyDown(K_CTRL))
//				imfl|=BT_IMPULSE_FIRE;
//			if(PDGL_KeyDown(K_ENTER))
//				imfl|=BT_IMPULSE_USE;

			if(PDGL_KeyDown(K_ATTACK))
				imfl|=BT_IMPULSE_FIRE;
			if(PDGL_KeyDown(K_USE))
				imfl|=BT_IMPULSE_USE;
			if(PDGL_KeyDown(K_ATTACK2))
				imfl|=BT_IMPULSE_ALTFIRE;
			if(PDGL_KeyDown(K_ATTACK3))
				imfl|=BT_IMPULSE_ALTFIRE2;

			if(PDGL_KeyDown(K_NEXTWEAPON))
				imfl|=BT_IMPULSE_NEXTWEAP;
			if(PDGL_KeyDown(K_PREVWEAPON))
				imfl|=BT_IMPULSE_PREVWEAP;

//			if(state->mb&1)imfl|=BT_IMPULSE_FIRE;
//			if(state->mb&2)imfl|=BT_IMPULSE_ALTFIRE;

			if((*kcur>='0') && (*kcur<='9') && !PDGL_KeyDown(K_ALT))
			{
				imfl|=BT_IMPULSE_0<<((*kcur)-'0');
				kcur++;
				continue;
			}
		}

		if(MapEdit_IsEdit())
		{
			if(*kcur=='g')
			{
				lbxgl_brushmesh_mode=LBXGL_EDMODE_GROUP;
				MapEdit_ClearSel();
			}
			if(*kcur=='f')
			{
				lbxgl_brushmesh_mode=LBXGL_EDMODE_FACE;
				MapEdit_ClearSel();
			}
			if(*kcur=='v')
			{
				lbxgl_brushmesh_mode=LBXGL_EDMODE_VERTEX;
				MapEdit_ClearSel();
			}
			if(*kcur=='o')
			{
				if((lbxgl_brushmesh_mode!=LBXGL_EDMODE_OBJECT) &&
					(lbxgl_brushmesh_mode!=LBXGL_EDMODE_TEXTURE))
						MapEdit_ClearSel();
				lbxgl_brushmesh_mode=LBXGL_EDMODE_OBJECT;
			}

			if(*kcur=='T')
			{
				if((lbxgl_brushmesh_mode!=LBXGL_EDMODE_OBJECT) &&
					(lbxgl_brushmesh_mode!=LBXGL_EDMODE_TEXTURE))
						MapEdit_ClearSel();
				lbxgl_brushmesh_mode=LBXGL_EDMODE_TEXTURE;
			}

			if(*kcur=='b')
			{
				lbxgl_brushmesh_mode=LBXGL_EDMODE_BRUSH;
				MapEdit_ClearSel();
			}

			if(*kcur=='s')lbxgl_brushmesh_cmd=LBXGL_EDCMD_SEL;
			if(*kcur=='t')lbxgl_brushmesh_cmd=LBXGL_EDCMD_TRANS;
			if(*kcur=='r')lbxgl_brushmesh_cmd=LBXGL_EDCMD_ROT;
			if(*kcur=='S')lbxgl_brushmesh_cmd=LBXGL_EDCMD_SCALE;
			if(*kcur=='A')lbxgl_brushmesh_cmd=LBXGL_EDCMD_SCALE2;

			if(*kcur=='P')cam->mode=PDGLUI_CAM_PERSP;
			if(*kcur=='O')cam->mode=PDGLUI_CAM_ORTHO;
			if(*kcur=='X')cam->mode=PDGLUI_CAM_XY;
			if(*kcur=='Z')cam->mode=PDGLUI_CAM_XZ;
			if(*kcur=='Y')cam->mode=PDGLUI_CAM_YZ;
			if(*kcur=='Q')cam->mode=PDGLUI_CAM_CAD4V;

			if(*kcur==K_ENTER)
			{
				if(lbxgl_brushmesh_mode==LBXGL_EDMODE_BRUSH)
				{
					sel_brush_face();
					if(world->brush_sel_face<0)sel_ents();
					if((world->brush_sel_face<0) && !world->entity_sel)
						sel_brushes(world->brush);
				}else
				{
					if(!PDGL_KeyDown(K_SHIFT) && lbxgl_meshsel_multi)
						MapEdit_ClearSel();
					LBXGL_BrushMesh_SelGroup(lbxgl_brushmesh_mdl);
				}
			}

//			if(*kcur=='T')
//			{
//				UI_TexFinder_SetTitle("Textures");
//				ui_state=1;
//			}
			if(*kcur=='O')
			{
				world->entity_psel=world->entity_sel;
			}
			if(*kcur=='P')
			{
				world->entity_sel=world->entity_psel;
				world->brush_sel=NULL;
			}

			if(*kcur=='l')
			{
				MapEdit_AddUndo();
				LBXGL_SEnt_NewBasicLight(&world->entity,
					cam->org, 100);
			}

			if(*kcur=='L')
			{
				world->lighting=(world->lighting+1)%3;
				switch(world->lighting)
				{
				case 0: s="Fullbright"; break;
				case 1: s="Enabled(direct)"; break;
				case 2: s="Enabled(light-texture)"; break;
				default: s="Unknown"; break;
				}

				Con_Printf("Lighting: %s\n", s);
			}


			if(*kcur=='d')
			{
				if(lbxgl_brushmesh_mode==LBXGL_EDMODE_BRUSH)
				{
					if(!world->entity_sel && !world->brush_sel)
					{
						MapEdit_AddUndo();
						LBXGL_BrushWorld_NewBrushEntity(world, "func_door");
					}
					if(world->brush_sel)
					{
						MapEdit_AddUndo();
						LBXGL_BrushWorld_DrawFlush(world);

						world->brush_sel->flags|=
							LBXGL_BRFL_DEL|BTGE_BRFL_DIRTY;
						LBXGL_Brush_MakeRoomFromBrush(
							world, world->brush_sel);
						world->brush_sel=NULL;
					}
				}
			}

#if 0
			if(*kcur=='-')
			{
				if(world->brush_sel)
				{
					MapEdit_AddUndo();
					LBXGL_BrushWorld_DrawFlush(world);
					LBXGL_Brush_SubtractBrush(world, world->brush_sel);
				}
			}
#endif

			if(*kcur==K_INS)
			{
				if(PDGL_KeyDown(K_SHIFT))
				{
					MapEdit_AddUndo();
					LBXGL_BrushWorld_DrawFlush(world);
					LBXGL_BrushMesh_Paste();
				}else if(lbxgl_brushmesh_mode==LBXGL_EDMODE_BRUSH)
				{
					MapEdit_AddUndo();
					LBXGL_BrushWorld_DrawFlush(world);

					V3F_ADDSCALE(cam->org, cam->fw, 24, pt);

					brsh=LBXGL_Brush_MakeCube(
						pt[0], pt[1], pt[2], 8, 8, 8,
						world->default_tex);
					LBXGL_BrushWorld_AddBrush(world, brsh);
					brsh->flags|=BTGE_BRFL_NEW|BTGE_BRFL_DIRTY;
				}else if(lbxgl_brushmesh_mode==LBXGL_EDMODE_OBJECT)
				{
					MapEdit_AddUndo();
					LBXGL_BrushWorld_DrawFlush(world);

					prim=LBXGL_BrushMesh_CreateCube();
					brsh=LBXGL_BrushMesh_MakeBrushUsingPrim(prim);
					LBXGL_BrushWorld_AddBrush(world, brsh);
					brsh->flags|=BTGE_BRFL_NEW|BTGE_BRFL_DIRTY;

					sprintf(buf, "textures/%s", world->default_tex);
					prim->tex=dystrdup(world->default_tex);
					prim->texnum=LBXGL_Texture_LoadImage(buf);
					LBXGL_BrushMesh_ReprojectPrim(prim);
				}
			}

			if(*kcur==K_DEL)
			{
				if(world->brush_sel)
				{
					MapEdit_AddUndo();
					LBXGL_BrushWorld_DrawFlush(world);

					world->brush_sel->flags|=LBXGL_BRFL_DEL|BTGE_BRFL_DIRTY;
					world->brush_sel=NULL;
				}else if(world->entity_sel)
				{
					MapEdit_AddUndo();
					LBXGL_BrushWorld_DrawFlush(world);
					unlink_entity(world->entity_sel);
					kcur++;
					continue;
				}else
				{
//					MapEdit_AddUndo();
					LBXGL_BrushWorld_DrawFlush(world);
					MapEdit_CmdSel(LBXGL_EDCMD_DELETE);
				}
			}

//			if(*kcur==K_DEL)MapEdit_CmdSel(LBXGL_EDCMD_DELETE);
			if(*kcur=='E')MapEdit_CmdSel(LBXGL_EDCMD_EXTRUDE);
			if(*kcur=='D')MapEdit_CmdSel(LBXGL_EDCMD_SUBDIVIDE);
			if(*kcur=='M')MapEdit_CmdSel(LBXGL_EDCMD_MERGE);
			if(*kcur=='N')MapEdit_CmdSel(LBXGL_EDCMD_INVERT);

			if((lbxgl_brushmesh_mode==LBXGL_EDMODE_VERTEX) && (*kcur=='F'))
			{
				MapEdit_AddUndo();
				LBXGL_BrushMesh_CreateFace(lbxgl_meshsel_prim);
			}

		}

		if(*kcur=='+')
		{
			f=btCvarGetf("r_split_offset");
			f=f+0.001;
			btCvarSetf("r_split_offset", f);
			printf("r_split_offset %f\n", f);
		}

		if(*kcur=='-')
		{
			f=btCvarGetf("r_split_offset");
			f=f-0.001;
			btCvarSetf("r_split_offset", f);
			printf("r_split_offset %f\n", f);
		}

		if(*kcur=='f')
		{
			world->flashlight=!world->flashlight;
			Con_Printf("Flashlight: %s\n",
				world->flashlight?"Enabled":"Disabled");
		}

#if 0
		if(MapEdit_IsEdit() && (*kcur==K_INS))
		{
			LBXGL_BrushWorld_DrawFlush(world);

			V3F_ADDSCALE(cam->org, cam->fw, 24, pt);

			brsh=LBXGL_Brush_MakeCube(pt[0], pt[1], pt[2], 6, 6, 6,
				world->default_tex);
			LBXGL_BrushWorld_AddBrush(world, brsh);
		}

		if(MapEdit_IsEdit() && (*kcur==K_DEL))
		{
			if(world->entity_sel)
			{
				unlink_entity(world->entity_sel);
				kcur++;
				continue;
			}
		}
#endif

		if(MapEdit_IsEdit() && (*kcur=='p'))
		{
//			V3F_COPY(cam->org, pvs_org);

			main_phys=!main_phys;
			Con_Printf("Physics: %s\n",
				main_phys?"Enabled":"Disabled");
		}

		if(MapEdit_IsEdit() && (*kcur=='s'))
		{
			main_sun=!main_sun;
			Con_Printf("Sun: %s\n",
				main_sun?"Enabled":"Disabled");
		}

		if(MapEdit_IsEdit() && (*kcur=='G'))
		{
			gcForceGC(1);
		}

		kcur++;
	}

	if(MapEdit_IsEdit())
	{
		if(PDGL_KeyDown(K_CTRL))
		{
			if(!(state->mb&1) && (state->lmb&1))
			{
				i=(state->mx>state->omx)?
					(state->mx-state->omx):
					(state->omx-state->mx);
				j=(state->my>state->omy)?
					(state->my-state->omy):
					(state->omy-state->my);

				if((i>5) && (j>5))
				{
//					dyPrintf("Sel Box\n");
					if(!PDGL_KeyDown(K_SHIFT))
						MapEdit_ClearSel();
					LBXGL_BrushMesh_SelGroupBox(lbxgl_brushmesh_mdl);
					return(0);
				}
			}

			return(0);
		}

		if((lbxgl_brushmesh_mode!=LBXGL_EDMODE_BRUSH) &&
			(lbxgl_brushmesh_cmd==LBXGL_EDCMD_SEL) &&
			(state->mb&1) && !(state->lmb&1))
		{
			if(!PDGL_KeyDown(K_SHIFT) && lbxgl_meshsel_multi)
				MapEdit_ClearSel();
			LBXGL_BrushMesh_SelGroup(lbxgl_brushmesh_mdl);
			return(0);
		}

		if((lbxgl_brushmesh_mode!=LBXGL_EDMODE_BRUSH) &&
			(lbxgl_brushmesh_cmd==LBXGL_EDCMD_SEL) &&
			!(state->mb&1) && (state->lmb&1))
		{
			i=(state->mx>state->omx)?
				(state->mx-state->omx):
				(state->omx-state->mx);
			j=(state->my>state->omy)?
				(state->my-state->omy):
				(state->omy-state->my);

			if((i>5) && (j>5))
			{
//				dyPrintf("Sel Box\n");
				if(!PDGL_KeyDown(K_SHIFT))
					MapEdit_ClearSel();
				LBXGL_BrushMesh_SelGroupBox(lbxgl_brushmesh_mdl);
				return(0);
			}
		}

		if((lbxgl_brushmesh_cmd!=LBXGL_EDCMD_SEL) && (state->mb&1))
		{
//			LBXGL_BrushMesh_CmdSelGroup(
//				lbxgl_brushmesh_mdl, lbxgl_brushmesh_cmd);
			MapEdit_CmdSel(lbxgl_brushmesh_cmd);
			return(0);
		}
	}

	//set eye-distance (milimeters)
	f=btCvarGetf("r_eyedist");
	f=(f/1000.0)*32;
	cam->eyedist=f;

	i=btCvarGetf("r_splitstereo3d");
	if(i)
	{
		if(cam->mode==PDGLUI_CAM_PERSP)
			cam->mode=PDGLUI_CAM_SPLIT3D;
	}else
	{
		if(cam->mode==PDGLUI_CAM_SPLIT3D)
			cam->mode=PDGLUI_CAM_PERSP;
	}

	if(player && !MapEdit_IsEdit())
		btclSetupClientCamera(cl_world);

	UI_Camera_Update();

	if(state->mb&1)imfl|=BT_IMPULSE_FIRE;
//	if(state->mb&2)imfl|=BT_IMPULSE_ALTFIRE;
//	if(state->mb&4)imfl|=BT_IMPULSE_ALTFIRE2;
	if(state->mb&2)imfl|=BT_IMPULSE_ALTFIRE2;
	if(state->mb&4)imfl|=BT_IMPULSE_ALTFIRE;
	
	if(cl_world)
	{
		cl_world->impfl=imfl;
	}
	
//	if(imfl)
//		dyPrintf("Impulse: %04X\n", imfl);
	
//	if(player && !cam->raw)
	if(cl_world && cl_world->camnum && !cam->raw)
	{
//		player->classname="player";

		if(!(cl_world->viewflags&BT_EF2_CAM_FIXED_ROT))
		{
			V4F_COPY(cam->iang+0, ia+0);
			V4F_COPY(cam->iang+4, ia+4);
			V4F_ADD(cam->ang+0, ia+0, cam->ang+0);
			V4F_ADD(cam->ang+4, ia+4, cam->ang+4);

			Mat3F_Identity(ir);
			Mat3F_Rotate3Matrix(ir, ir+6, cam->ang[2]*(M_PI/180), ir);
			Mat3F_Rotate3Matrix(ir, ir+0, cam->ang[0]*(M_PI/180), ir);
			Mat3F_Rotate3Matrix(ir, ir+3, cam->ang[1]*(M_PI/180), ir);
			Mat3F_Copy(ir, cam->rot);
		}

		//update player/camera position (if not fixed)
		if(!(cl_world->viewflags&BT_EF2_CAM_FIXED_ORG))
		{
#if 0
			l0=btclComposeUpdate(cl_world);
//			l0=dylist2s("cldelta", l0);
			l0=dylist1(l0);
			BTGE_Net_ClientSendUpdate(l0);
#endif

//			V3F_COPY(cam->ivel, iv);
//			iv[2]=0;
//			V3F_ADD(cam->org, iv, cam->org);

//			dysPrintln(l0);
			
//			BTGE_DecodeClientUpdate(l0, state->dt_f);
//			vfvec3(cam->org, v3add(player->origin, player->vieworg));
//			vfvec3(cam->ang, v3sub(player->angles, vec3(0,0,90)));

//			dyFreeListR(l0);
//			btFreeUpdate(l0);
		}
	}

#if 0
	else if(player)
	{
		player->origin=v3sub(vec3vf(cam->org), vec3(0,0,48));
		player->angles=v3add(vec3vf(cam->ang), vec3(0,0,90));
		player->velocity=vec3(0,0,0);	//prevent mega-gravity

		player->morg=vec3vf(cam->morg);
		player->mend=vec3vf(cam->mend);
		player->mdir=vec3vf(cam->mdir);
	}
#endif
}

int pdgl_main_clientFrame()
{
	dyt l0;

	if(cl_world && cl_world->camnum)
	{
		l0=btclComposeUpdate(cl_world);
//		l0=dylist2s("cldelta", l0);
		l0=dylist1(l0);
		BTGE_Net_ClientSendUpdate(l0);
	}
}

void main_handle_form(char *frm)
{
	LBXGL_SEntity *ecur;
	LBXGL_SEntPair *epcur;

	char *s0, *s1, *s2, *s;
	int i, j;

	while(PDGLUI_HasEvent(frm))
	{
		s0=PDGLUI_GetEventWidget(frm);
		s1=PDGLUI_GetEventName(frm);
		i=PDGLUI_GetEventIVal(frm);
		s2=PDGLUI_GetEventSVal(frm);
		PDGLUI_NextEvent(frm);

		if(!s0)continue;

		if(!strcmp(frm, "help") && !strcmp(s0, "btn_ok"))
		{
			PDGLUI_Forms_Unlink(frm);
		}

		if(!strcmp(frm, "selbox") &&
			!strcmp(s0, "lst") && !strcmp(s1, "select"))
		{
			j=i; ecur=world->entity;
			while(ecur && j--)ecur=ecur->next;
			world->entity_sel=ecur;

			PDGLUI_SetWidgetPropertyS(
				"selbox", "key", "text", "");
			PDGLUI_SetWidgetPropertyS(
				"selbox", "value", "text", "");
		}

		if(!strcmp(frm, "selbox") &&
			!strcmp(s0, "plst") && !strcmp(s1, "select"))
		{
			if(!s2)continue;

			world->entity_sel_key=dystrdup(s2);
			epcur=world->entity_sel->keys;
			while(epcur && strcmp(epcur->name, s2))
				epcur=epcur->next;
			if(!epcur)continue;

			PDGLUI_SetWidgetPropertyS(
				"selbox", "key", "text", epcur->name);
			PDGLUI_SetWidgetPropertyS(
				"selbox", "value", "text", epcur->value);
		}

		if(!strcmp(frm, "selbox") &&
			!strcmp(s0, "value") &&
			!strcmp(s1, "submit"))
		{
			if(!world->entity_sel)continue;

			s=PDGLUI_GetWidgetPropertyS(
				"selbox", "key", "text");
			LBXGL_SEnt_SetStr(world->entity_sel, s, s2);
		}
	}
}

void main_handle_events()
{
//	main_handle_form("help");
//	main_handle_form("selbox");
}

static int main_menubkg=0;

int pdgl_main_drawMenuBackground()
{
	char *s;
	float tv[3];

	if(!main_menubkg)
	{
		s=btCvarGet("cl_menusky");
//		if(!s)s="space1";

		if(s)
		{
			LBXGL_Sky_LoadSky(s);

			V3F_SET(tv, 5, 4, 3);
			LBXGL_Sky_SetTorque(tv);

			V3F_SET(tv, 1, 2, 3);
			LBXGL_Sky_SetTorque2(tv);		
		}

		main_menubkg=1;
	}

	if(!cam->ortho)
		LBXGL_Sky_DrawSky();
}

int pdgl_main_draw3d()
{
	float pt[4];
	LBXGL_Brush *cur;
	int t0, t1;
	int i;

	LBXGL_Sprite_SetCameraOrgRot(cam->org, cam->rot);

	if(cl_world && !cl_world->camnum)
	{
		pdgl_main_drawMenuBackground();
		return(0);
	}

	main_menubkg=0;

	if(!cam->ortho)
		LBXGL_Sky_DrawSky();

	pdglDisable(GL_CULL_FACE);
	pdglDisable(GL_TEXTURE_2D);

#if 0
	pdglBegin(PDGL_LINES);

	pdglColor4f(1, 0, 0, 1);
	pdglVertex3f(0, 0, 0);
	pdglVertex3f(1000, 0, 0);

	pdglColor4f(0, 1, 0, 1);
	pdglVertex3f(0, 0, 0);
	pdglVertex3f(0, 1000, 0);

	pdglColor4f(0, 0, 1, 1);
	pdglVertex3f(0, 0, 0);
	pdglVertex3f(0, 0, 1000);

	pdglEnd();
#endif

//	pdglEnable(GL_CULL_FACE);

//	if(!cam->ortho || (cam->mode==PDGLUI_CAM_SOLID2D))
//		pdglEnable(GL_TEXTURE_2D);
	pdglDepthFunc(GL_LEQUAL);

	pdglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//	setup_light_ents(world->entity);

	if(cam->ortho && (cam->mode!=PDGLUI_CAM_SOLID2D))
	{
		pdglCullFace(GL_BACK);	pdglEnable(GL_CULL_FACE);
		pdglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		LBXGL_Shader_Color4f(1, 1, 1, 1);
//		pdglColor4f(1, 1, 1, 1);

		LBXGL_BrushWorld_DrawBrushesFlat(world, world->brush);

		LBXGL_Shader_Color4f(0, 1, 0, 1);

		LBXGL_BrushWorld_DrawModelsFlat(world);
	}else
	{
		pdglEnable(GL_TEXTURE_2D);
		LBXGL_BrushWorld_DrawBrushes(world, world->brush);
	}

	if(MapEdit_IsEdit())
	{
#if 1
		if(lbxgl_brushmesh_mode!=LBXGL_EDMODE_BRUSH)
		{
			//temporary hack...

			pdglColor4f(1, 1, 1, 1);
			pdglDisable(GL_CULL_FACE);

			LBXGL_BrushMesh_DrawGroupEdit(lbxgl_brushmesh_mdl);
		}
#endif

#if 1
		if(lbxgl_brushmesh_mode==LBXGL_EDMODE_BRUSH)
		{
			cur=world->brush;
			while(cur)
			{
				LBXGL_BrushWorld_DrawControlsBrush(world, cur);
				cur=cur->next;
			}
		}
#endif

//		pdglDisable(GL_ALPHA_TEST);

		draw_ents(world->entity);
	}

//	draw_ents(world->entity);

//	draw_test_defsdr();
//	draw_test_terrain();

#if 0
	t0=PDGL_TimeMS();
	LBXGL_Particle_Draw();
	t1=PDGL_TimeMS();
	state->dt_render_particle=t1-t0;
#endif
}

int pdgl_main_drawLoading()
{
	static char *imgs[]=
	{
		"load_silicon",
		"load_carbon",
		NULL
	};

	char buf[256];
	int i, j, k;

	Con_Update(state->dt_f);

	UI_Camera_Setup2D();

	i=rand()%2;
	sprintf(buf, "images/%s", imgs[i]);
//	j=Tex_LoadFile(buf, NULL, NULL);
	j=LBXGL_Texture_LoadImage(buf);

	LBXGL_Shader_Color4f(1, 1, 1, 1);
//	pdglEnable(GL_TEXTURE_2D);
	LBXGL_Shader_BindTexture(j);

	LBXGL_Shader_Begin(PDGL_POLYGON);
	LBXGL_Shader_TexCoord2f(0, 0);
	LBXGL_Shader_Vertex2f(-(state->xs/2), -(state->ys/2));
	LBXGL_Shader_TexCoord2f(1, 0);
	LBXGL_Shader_Vertex2f((state->xs/2), -(state->ys/2));
	LBXGL_Shader_TexCoord2f(1, 1);
	LBXGL_Shader_Vertex2f((state->xs/2), (state->ys/2));
	LBXGL_Shader_TexCoord2f(0, 1);
	LBXGL_Shader_Vertex2f(-(state->xs/2), (state->ys/2));
	LBXGL_Shader_End();

	GfxFont_SetFont("fixed", 0);

//	btclDraw2D(cl_world);	//HUD or similar

	Con_Render();
	GfxFont_SetFont("fixed", 0);

//	btclDraw2D(cl_world);	//HUD or similar
//	GfxFont_SetFont("fixed", 0);

//	adt=(adt*0.90)+(state->dt_f*0.10);
//	sprintf(buf, "(%g %g %g  %g) (%g %g %g  %g %g %g) %.2ffps",
//		cam->org[0], cam->org[1], cam->org[2], cam->org[3],
//		cam->ang[0], cam->ang[1], cam->ang[2],
//		cam->ang[3], cam->ang[4], cam->ang[5],
//		1.0/adt);
//	GfxFont_DrawString(buf, -400, 300-12,
//		8, 8,  0, 255, 0, 255);
}

int pdgl_main_drawRecSpectral()
{
	int blk[64];
	float ox, oy, f, g;
	float cr, cg, cb;
	int i, j, k, l;

//	PDGL_Sound_GetRecordSpectrum(blk, 64);
	
	pdglDisableTexture2D();
	pdglColor4f(1, 0, 0, 1);

	pdglBegin(PDGL_QUADS);
	
	ox=-640; oy=300;
	for(i=0; i<64; i++)
	{
		PDGL_Sound_GetPriorRecordSpectrum(63-i, blk, 64);
		for(j=0; j<64; j++)
		{
			f=log(blk[j])/log(2);
			f=f/15.0;
			if(f<0)f=0;
			if(f>1.0)f=1.0;
			cb=1.0-fabs(f-0.0);
			cg=1.0-fabs(f-0.5);
			cr=1.0-fabs(f-1.0);
			pdglColor4f(cr, cg, cb, 0.5);
			
			pdglVertex2f(ox+i*4+0, oy-j*4+0);
			pdglVertex2f(ox+i*4+0, oy-j*4-4);
			pdglVertex2f(ox+i*4+4, oy-j*4-4);
			pdglVertex2f(ox+i*4+4, oy-j*4+0);
		}
	}
	
//	pdglVertex2f(state->mx-10, -state->my);
//	pdglVertex2f(state->mx+10, -state->my);
//	pdglVertex2f(state->mx, -state->my-10);
//	pdglVertex2f(state->mx, -state->my+10);
	pdglEnd();

	pdglEnableTexture2D();
}

static double adt=1.0/30;

int pdgl_main_draw2D()
{
	char buf[65536];
	char buf1[65536];

	LBXGL_Brush *brsh;
	LBXGL_SEntity *ecur;
	LBXGL_SEntPair *epcur;
//	dyt l0;

	int t0, t1;
	int i, j, k;
//	unsigned short *kcur;
	char *s, *s0, *s1;

	GfxFont_SetFont("fixed", 0);

	LBXGL_BrushDraw2D_DrawWorld2D(world);
	
//	pdgl_main_drawRecSpectral();
	
//	if(((state->doshot&15)==2) && (cam->flags&PDGLUI_CAMFL_RECORDHIDEUI))
	if(UI_Camera_GetHideUI())
		return(0);
	
	btclDraw2D(cl_world);	//HUD or similar

	UI_MetaUI_Draw();
	Con_Render();
	GfxFont_SetFont("fixed", 0);

	if(UI_MetaUI_IsActive())
	{
		PDGLUI_Frame();
		main_handle_events();

#if 0
		pdglDisable(GL_TEXTURE_2D);
		pdglColor4f(1, 0, 0, 1);

		pdglBegin(PDGL_LINES);
		pdglVertex2f(state->mx-10, -state->my);
		pdglVertex2f(state->mx+10, -state->my);
		pdglVertex2f(state->mx, -state->my-10);
		pdglVertex2f(state->mx, -state->my+10);
		pdglEnd();
#endif

		GfxFont_SetFont("fixed", 0);
		GfxFont_DrawString("X", state->mx-8, -state->my-8,
			16, 16,  255, 0, 0, 255);

		return(0);
	}

//	btclDraw2D(cl_world);	//HUD or similar
//	GfxFont_SetFont("fixed", 0);

	adt=(adt*0.90)+(state->dt_f*0.10);
	sprintf(buf, "org=(%g %g %g  %g) ang=(%g %g %g  %g %g %g) %.2ffps",
		cam->org[0], cam->org[1], cam->org[2], cam->org[3],
		cam->ang[0], cam->ang[1], cam->ang[2],
			cam->ang[3], cam->ang[4], cam->ang[5],
		1.0/adt);
	GfxFont_DrawString(buf, -(state->xs/2), (state->ys/2)-12,
		8, 8,  0, 255, 0, 255);

	sprintf(buf, "absorg=(%g %g %g) reforg=(%g %g %g) cam=%d",
		cam->org[0]+world->cam_reforg[0],
		cam->org[1]+world->cam_reforg[1],
		cam->org[2]+world->cam_reforg[2],
		(double)(world->cam_reforg[0]),
		(double)(world->cam_reforg[1]),
		(double)(world->cam_reforg[2]),
		cl_world?cl_world->camnum:-999);
	GfxFont_DrawString(buf, -(state->xs/2), (state->ys/2)-12-8,
		8, 8,  0, 255, 0, 255);

	sprintf(buf, "absorg(m)=(%.2f %.2f %.2f) "
		"reforg(m)=(%.2f %.2f %.2f) cam=%d",
		(cam->org[0]+world->cam_reforg[0])*(1.0/32),
		(cam->org[1]+world->cam_reforg[1])*(1.0/32),
		(cam->org[2]+world->cam_reforg[2])*(1.0/32),
		((double)(world->cam_reforg[0]))*(1.0/32),
		((double)(world->cam_reforg[1]))*(1.0/32),
		((double)(world->cam_reforg[2]))*(1.0/32),
		cl_world?cl_world->camnum:-999);
	GfxFont_DrawString(buf, -(state->xs/2), (state->ys/2)-12-16,
		8, 8,  0, 255, 0, 255);

	if(MapEdit_IsEdit())
	{
		sprintf(buf, "(%.3f %.3f %.3f) (%.3f %.3f %.3f)",
			cam->morg[0], cam->morg[1], cam->morg[2],
			cam->mdir[0], cam->mdir[1], cam->mdir[2]);
		GfxFont_DrawString(buf, -(state->xs/2), (state->ys/2)-12-8,
			8, 8,  0, 255, 0, 255);

		strcpy(buf, "");
		if(lbxgl_brushmesh_mode==LBXGL_EDMODE_GROUP)strcpy(buf, "Group");
		if(lbxgl_brushmesh_mode==LBXGL_EDMODE_OBJECT)strcpy(buf, "Object");
		if(lbxgl_brushmesh_mode==LBXGL_EDMODE_FACE)strcpy(buf, "Face");
		if(lbxgl_brushmesh_mode==LBXGL_EDMODE_VERTEX)strcpy(buf, "Vertex");
		if(lbxgl_brushmesh_mode==LBXGL_EDMODE_TEXTURE)strcpy(buf, "Texture");
		if(lbxgl_brushmesh_mode==LBXGL_EDMODE_BRUSH)strcpy(buf, "Brush");
		GfxFont_DrawString(buf, (state->xs/2)-(16*8), (state->ys/2)-12,
			8, 8,  0, 255, 0, 255);

		strcpy(buf, "");
		if(lbxgl_brushmesh_cmd==LBXGL_EDCMD_SEL)strcpy(buf, "Select");
		if(lbxgl_brushmesh_cmd==LBXGL_EDCMD_TRANS)strcpy(buf, "Translate");
		if(lbxgl_brushmesh_cmd==LBXGL_EDCMD_ROT)strcpy(buf, "Rotate");
		if(lbxgl_brushmesh_cmd==LBXGL_EDCMD_SCALE)strcpy(buf, "Scale");
		if(lbxgl_brushmesh_cmd==LBXGL_EDCMD_SCALE2)strcpy(buf, "Scale-HV");
		GfxFont_DrawString(buf, (state->xs/2)-(16*8), (state->ys/2)-12-8,
			8, 8,  0, 255, 0, 255);

		if(cam->mode==0)strcpy(buf, "Perspective 3D");
		if(cam->mode==1)strcpy(buf, "Ortho 3D");
		if(cam->mode==2)strcpy(buf, "Ortho XY");
		if(cam->mode==3)strcpy(buf, "Ortho XZ");
		if(cam->mode==4)strcpy(buf, "Ortho YZ");
		if(cam->mode==5)strcpy(buf, "Perspective 2");
		if(cam->mode==6)strcpy(buf, "4D");
		if(cam->mode==7)strcpy(buf, "4 Views");
		GfxFont_DrawString(buf, (state->xs/2)-(16*8), (state->ys/2)-12-16,
			8, 8,  0, 255, 0, 255);
	}

	if(player)
	{
		sprintf(buf, "HP=%d/%d MP=%d/%d",
			(int)(player->health), (int)(player->maxHealth),
			(int)(player->curMP), (int)(player->maxMP));
		GfxFont_DrawString(buf, -200, -300,
			8, 8,  255, 255, 0, 255);
	}

#if 0
	i=0;
	ecur=world->entity;
	while(ecur && (i<25))
	{
		j=7;
		if(ecur==world->entity_psel)j=3;
		if(ecur==world->entity_sel)j=2;

		s=LBXGL_SEnt_GetStr(ecur, "classname");
		GfxFont_DrawString(s, (state->xs/2)-24*8, (state->ys/2)-(i+2)*8,
			8, 8,  (j&1)?255:0, (j&2)?255:0, (j&4)?255:0, 255);

		ecur=ecur->next;
		i++;
	}
#endif

#if 1
	if(MapEdit_IsEdit() && world->brush_sel)
	{
		if(world->brush_sel->se_owner)
		{
			world->entity_psel=world->brush_sel->se_owner;

			s=LBXGL_SEnt_GetStr(world->brush_sel->se_owner, "classname");
			sprintf(buf, "Owner: %s, %d", s,
				world->brush_sel->se_owner->idnum);
			GfxFont_DrawString(buf,
				(state->xs/2)-24*8, -(state->ys/2)+25*8,
				8, 8,  0, 255, 0, 255);
		}


		i=world->brush_sel_face;
		if(i<0)i=0;

		if(world->brush_sel->patch)
		{
			s=world->brush_sel->patch->tex;
		}else if(world->brush_sel->mesh)
		{
			s=world->brush_sel->mesh->tex;
		}else
		{
			if(world->brush_sel->tex)
				s=world->brush_sel->tex[i];
		}
		
		if(s)
		{
			sprintf(buf, "%s, %d", s, world->brush_sel->idnum);
			GfxFont_DrawString(buf,
				(state->xs/2)-24*8, -(state->ys/2)+24*8,
				8, 8,  0, 255, 0, 255);

//			sprintf(buf, "textures/%s", s);
//			j=Tex_LoadFile(buf, NULL, NULL);

			sprintf(buf, "textures/%s", s);
			j=LBXGL_Texture_LoadImage(buf);

			LBXGL_Shader_Color4f(1, 1, 1, 1);
//			pdglEnable(GL_TEXTURE_2D);
			LBXGL_Shader_BindTexture(j);

			LBXGL_Shader_Begin(PDGL_POLYGON);

			LBXGL_Shader_TexCoord2f(0, 0);
			LBXGL_Shader_Vertex2f((state->xs/2)-16*8, -(state->ys/2));

			LBXGL_Shader_TexCoord2f(1, 0);
			LBXGL_Shader_Vertex2f((state->xs/2), -(state->ys/2));

			LBXGL_Shader_TexCoord2f(1, 1);
			LBXGL_Shader_Vertex2f((state->xs/2), -(state->ys/2)+16*8);

			LBXGL_Shader_TexCoord2f(0, 1);
			LBXGL_Shader_Vertex2f((state->xs/2)-16*8, -(state->ys/2)+16*8);

			LBXGL_Shader_End();
		}
	}

	if(MapEdit_IsEdit() && world->entity_sel)
	{
		epcur=world->entity_sel->keys;

		i=0;
		while(epcur)
		{
			sprintf(buf, "\"%s\" \"%s\"",
				epcur->name, epcur->value);
			GfxFont_DrawString(buf,
				(state->xs/2)-32*8, -(state->ys/2)+24*8-i*8,
				8, 8,  0, 255, 0, 255);

			i++;
			epcur=epcur->next;
		}
	}else if(MapEdit_IsEdit() && world->entity_psel)
	{
		epcur=world->entity_psel->keys;

		i=0;
		while(epcur)
		{
			sprintf(buf, "\"%s\" \"%s\"",
				epcur->name, epcur->value);
			GfxFont_DrawString(buf,
				(state->xs/2)-32*8, -(state->ys/2)+24*8-i*8-8,
				8, 8,  0, 255, 0, 255);

			i++;
			epcur=epcur->next;
		}
	}
#endif

	if(ui_state==1)
	{
		UI_TexFinder_Draw();

		s=UI_TexFinder_GetName();
		if(s)
		{
			strcpy(buf, s);
			s=buf;
			while(*s=='/')s++;
			s0=s+strlen(s);
			while((s0>s) && (*s0!='/') && (*s0!='.'))s0--;
			if(*s0=='.')*s0=0;
			s=dystrdup(s);
			
			ui_state=0;

			if(lbxgl_brushmesh_mode==LBXGL_EDMODE_BRUSH)
			{
				if(world->brush_sel)
				{
					i=world->brush_sel_face;
					if(i<0)
					{
						for(i=0; i<world->brush_sel->n_face; i++)
							world->brush_sel->tex[i]=s;
					}else
					{
						world->brush_sel->tex[i]=s;
					}

					LBXGL_Brush_DestroyFaces(world->brush_sel);
					LBXGL_BrushWorld_DrawFlush(world);
				}
			}else
			{
				if(lbxgl_meshsel_prim)
				{
					sprintf(buf, "textures/%s", s);
//					dyPrintf("Set Tex Prim %s\n", buf);

//					lbxgl_meshsel_prim->tex=dystring(buf);
//					lbxgl_meshsel_prim->texnum=
//						Tex_LoadFile(buf, NULL, NULL);

					lbxgl_meshsel_prim->tex=dystrdup(s);
					lbxgl_meshsel_prim->texnum=LBXGL_Texture_LoadImage(buf);

					dyPrintf("Set Tex Prim %s %d\n",
						s, lbxgl_meshsel_prim->texnum);
				}
			}
		}
	}

	if((ui_state==2) || (ui_state==3))
	{
		UI_LoadSave_Draw();
		s=UI_LoadSave_GetName();

		if(s)
		{
			if(*s && (ui_state==2))
				MAP_LoadFile(world, s);
			if(*s && (ui_state==3))
				MAP_SaveFile(world, s);

			ui_state=0;
		}
	}

	PDGLUI_Frame();
	main_handle_events();

	pdglDisable(GL_STENCIL_TEST);
	pdglDisable(GL_SCISSOR_TEST);

	GfxFont_SetFont("fixed", 0);
	GfxFont_DrawString("X", state->mx-8, -state->my-8,
		16, 16,  255, 0, 0, 255);

#if 0
	pdglDisable(GL_TEXTURE_2D);
	pdglColor4f(1, 0, 0, 1);

	pdglBegin(PDGL_LINES);
	pdglVertex2f(state->mx-10, -state->my);
	pdglVertex2f(state->mx+10, -state->my);
	pdglVertex2f(state->mx, -state->my-10);
	pdglVertex2f(state->mx, -state->my+10);
	pdglEnd();
#endif
}

int pdgl_main_body()
{
	static int t;
//	static float adt=1.0/30;
	
	static int msg_sz=0;
	static int msg_cnt=0;
	static int msg_lzsz=0;
	static int msg_lzcnt=0;
	
//	char buf[256];
	char buf[65536];
	char buf1[65536];
	float mins[3], maxs[3];
	float io[3], iv[4], fw[2], pt[4], f, g, v;
	float im[16], im1[16], *imv;

	LBXGL_Brush *brsh;
	LBXGL_SEntity *ecur;
	LBXGL_SEntPair *epcur;
	dyt l0;

	int t0, t1;
	int i, j, k;
	unsigned short *kcur;
	char *s, *s0, *s1;

	if(map_doLoad)
	{
		pdgl_main_drawLoading();
	
		switch(map_doLoadStep++)
		{
		case 0:
			break;
		case 1:
			MAP_LoadFile(world, map_doLoad);
			break;
		case 2:
			map_doLoad=NULL;
			break;
		default:
			map_doLoad=NULL;
			break;
		}
//		MAP_LoadFile(world, map_doLoad);
//		map_doLoad=NULL;
		return;
	}

	i=btCvarGeti("g_profiler");
	BGBGC_ProfilerSetActive(i);

	BTGE_RunStepSequencedEvents(state->dt_f);

	Con_Update(state->dt_f);
	UI_MetaUI_HandleInput();
	main_handle_input();
	thDoWorkMS(10);
	LBXGL_BrushDraw2D_UpdateWorld2D(world, state->dt_f);

	pdgl_main_clientFrame();

//	setup_light_ents(world->entity);

//	BTGE_Voxel_GenerateChunks(sv_world);
//	world->vox_region=
//		sv_world->vox_region;

	t0=PDGL_TimeMS();

//	main_time+=state->dt_f;
	LBXGL_Particle_Step(state->dt_f);
	btclStepWorld(cl_world, state->dt_f);

#if 0
	if(player)
	{
		player->origin=v3sub(vec3vf(cam->org), player->vieworg);
		player->angles=v3add(vec3vf(cam->ang), vec3(0,0,90));
	}

	btStepWorld(state->dt_f);
	l0=btComposeUpdate();
#endif

	pdgl_main_serverFrame();

	l0=BTGE_Net_ClientGetUpdate();
	while(l0)
	{
		btclDecodeUpdate(cl_world, l0);
		btFreeUpdate(l0);
		l0=BTGE_Net_ClientGetUpdate();
	}

#if 0
	if(player)
	{
		vfvec3(cam->org, v3add(player->origin, player->vieworg));
		vfvec3(cam->ang, v3sub(player->angles, vec3(0,0,90)));
	}
#endif

//	V3F_SCALE(cam->org, 1.0/40.0, io);
//	V3F_SCALE(cam->vel, 1.0/40.0, iv);
	V3F_SCALE(cam->org, 1.0/32.0, io);
	V3F_SCALE(cam->vel, 1.0/32.0, iv);
	PDGL_Sound_SetWorld(world);
	PDGL_Sound_CamOriginFV(io);
	PDGL_Sound_CamVelocityFV(iv);
	PDGL_Sound_CamRotationFV(cam->rot);
	PDGL_Sound_Frame(state->dt_f);

#if 0
	if(l0)
	{
		dyPrint(l0);
		dyPrintf("\n");
	}
#endif

#if 0
	if(l0)
	{
		dysDumpStr(buf, l0);
		i=strlen(buf);
		
		msg_sz+=i; msg_cnt++;
		j=msg_sz/msg_cnt;
		
		
		dyPrintf("Net: %5d %3dkB/s AVG=%5d %3dkB/s %5dkbps\n",
			i, (i*10)/1000, j, (j*10)/1000, (j*10*8)/1000);

		k=vfDeflateBuffer(buf1, buf, 65536, i);
		msg_lzsz+=k; msg_lzcnt++;
		j=msg_lzsz/msg_lzcnt;

		dyPrintf("NetLZ: %5d %3dkB/s AVG=%5d %3dkB/s %5dkbps\n",
			k, (k*10)/1000, j, (j*10)/1000, (j*10*8)/1000);
	}
#endif

//	btFreeUpdate(l0);

	btclSetupClientFrame(cl_world);

//	if(main_phys)
//		LBXGL_BrushPhys_StepWorld(world, state->dt_f);

	t1=PDGL_TimeMS();
	state->dt_update=t1-t0;
//	if(main_timestat)
//		dyPrintf("Update Step %dms\n", t1-t0);


	t0=PDGL_TimeMS();

	UI_Camera_Setup3D();
	LBXGL_BrushWorld_DrawTestAutovis(world);

	t1=PDGL_TimeMS();
	state->dt_vischeck=t1-t0;
//	if(main_timestat)
//		dyPrintf("Visibility %dms\n", t1-t0);

	t0=PDGL_TimeMS();
	if(UI_Camera_4ViewsP())
	{
		UI_Camera_Setup3D_A();
		pdgl_main_draw3d();

		UI_Camera_Setup3D_B();
		pdgl_main_draw3d();

		UI_Camera_Setup3D_C();
		pdgl_main_draw3d();

		UI_Camera_Setup3D_D();
		pdgl_main_draw3d();
	}else if(UI_Camera_2ViewsP())
	{
		UI_Camera_Setup3D_A();
		pdgl_main_draw3d();

		UI_Camera_Setup3D_B();
		pdgl_main_draw3d();
	}else
	{
		UI_Camera_Setup3D();
		pdgl_main_draw3d();
	}

	t1=PDGL_TimeMS();
	state->dt_render=t1-t0;

	state->dt_render_avg=
		(state->dt_render_avg*0.9)+
		(state->dt_render*0.1);

//	if(main_timestat)
//		dyPrintf("Render %dms\n", t1-t0);

	if(main_timestat)
	{
		t1=PDGL_AVI_GetEncodeTime();
	
		dyPrintf("tick=%2d vis=%2d render=%2d videnc=%2d\n",
			state->dt_update, state->dt_vischeck, state->dt_render, t1);

		dyPrintf("r_dark=%2d r_shadow=%2d (face=%2d, vol=%2d) "
				"r_noshadow=%2d r_vertex=%2d\n",
			state->dt_render_dark,
			state->dt_render_fullshadow,
			state->dt_render_shadow_face,
			state->dt_render_shadow_volume,
			state->dt_render_noshadow,
			state->dt_render_vertex);

		dyPrintf("r_final=%2d r_decal=%2d r_alpha=%2d r_particle=%2d\n",
			state->dt_render_final,
			state->dt_render_decal,
			state->dt_render_alpha,
			state->dt_render_particle);
	}


//	LBXGL_BrushPhys_UpdateWorld(world);

	UI_Camera_Setup2D();
	pdgl_main_draw2D();

	return(0);
}

int pdgl_main_shutdown()
{
	dyPrintf("app1 shutdown\n");
	return(0);
}
