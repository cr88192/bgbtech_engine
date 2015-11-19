#include <windows.h>

#include <GL/gl.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI		3.1415926535897932384626433832795
#endif

#include <pdgl.h>
#include <pdglui.h>
#include <pdgl_stub.h>

#include <lbxgl2.h>
#include <libvecmath.h>

#include <bgbasm.h>
#include <bgbccc.h>


int main_phys=1;
int main_sun=0;

LBXGL_BrushWorld *world;
LBXGL_Light *sun;

int ui_state;
int shader_phong;
int shader_phong_spot;
int tex_screen;

// char *map_name=NULL;
// int map_stat_brushes;
// int map_stat_ents;
// int map_stat_lights;

static double bm_log2(double x)
	{ return(log(x)/log(2)); }

int main_DrawBox(
	float minx, float miny, float minz,
	float maxx, float maxy, float maxz)
{
	int i, j;

//	printf("cube (%f %f %f) (%f %f %f)\n",
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

//	printf("cube (%f %f %f) (%f %f %f)\n",
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

	tex=LBXGL_Texture_LoadImageSize(
		"textures/ground/cs_terrain", &xs, &ys);

	pdglDisable(GL_CULL_FACE);

	LBXGL_Shader_PushMatrix4D();

	f=(90/M_PI)*state->time_f;
//	LBXGL_Shader_RotateAngles4D_f(0,0,0, f,f,f);
	LBXGL_Shader_RotateAngles4D_f(0,0,0, f,0,0);

	LBXGL_Shader_BindTexture(tex);
//	main_DrawBox(-16, -16, -16, 16, 16, 16);
	main_DrawBox4D(-16, -16, -16, -16, 16, 16, 16, 16);

	LBXGL_Shader_PopMatrix4D();
}

void draw_test_defsdr()
{
	int sdr, tex, def;

	draw_test_box4d();

	return;

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

	pdglBegin(GL_QUADS);

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

		pdglBegin(GL_TRIANGLES);
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

	if(!world->brush_sel && !cam->ortho)
	{
		Con_Printf("Sel Brush\n");

		best=NULL; bf=999999;

		cur=fst;
		while(cur)
		{
			i=LBXGL_Brush_CheckLineBrush(cur,
				cam->morg, cam->mend);
			if(!i) { cur=cur->next; continue; }

			f=V3F_DOT(cur->org, cam->fw)-V3F_DOT(cam->org, cam->fw);

			if(!best || f<bf) { best=cur; bf=f; }

			cur=cur->next;
		}

		if(!best)Con_Printf("Sel: No Brush\n");

		world->brush_sel=best;
		world->brush_sel_face=-1;

		return;
	}

	Con_Printf("Sel 2\n");

	cur=fst;
	while(cur)
	{
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
		}

		cur=cur->next;
	}

}

float ent_radius(LBXGL_SEntity *ent)
{
	char *ty;
	float f;

	f=4;

	ty=LBXGL_SEnt_GetStr(ent, "classname");

	if(!strncmp(ty, "light", 5))
	{
		f=LBXGL_SEnt_GetNum(ent, "light");
		f=bm_log2(f);
		if(f<1)f=1;
		if(f>8)f=8;
	}

	if(!strcmp(ty, "info_player_start"))
		f=36;

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
dyt spawn_ent(LBXGL_SEntity *ent)
{
	char tb[256];
	LBXGL_SEntPair *cur;
	char **a;
	char *s;
	void *p;
	dyt obj, q;

	obj=dyObject();
	cur=ent->keys;
	while(cur)
	{
		a=gcrsplit(cur->value);

		if(check_str_num(a[0]) && !a[1])
		{
			if(!check_str_real(a[0]))
			{
				dySeti(obj, cur->name, atoi(a[0]));
				cur=cur->next; continue;
			}

			dySetf(obj, cur->name, atof(a[0]));
			cur=cur->next; continue;
		}

		if(check_str_num(a[0]) && check_str_num(a[1]) && !a[2])
		{
			dySet2f(obj, cur->name, atof(a[0]), atof(a[1]));
			cur=cur->next; continue;
		}
		if(check_str_num(a[0]) && check_str_num(a[1]) &&
			check_str_num(a[2]) && !a[3])
		{
			dySet3f(obj, cur->name,
				atof(a[0]), atof(a[1]), atof(a[2]));
			cur=cur->next; continue;
		}
		if(check_str_num(a[0]) && check_str_num(a[1]) &&
			check_str_num(a[2]) && check_str_num(a[3]) && !a[4])
		{
			dySet4f(obj, cur->name, atof(a[0]), atof(a[1]),
				atof(a[2]), atof(a[3]));
			cur=cur->next; continue;
		}

		dySets(obj, cur->name, cur->value);

		cur=cur->next;
	}

	s=dyGets(obj, "classname");
	if(!s)return(NULL);
	sprintf(tb, "lbeSpawn_%s", s);
	p=ccGetAddr(tb);
	if(!p)return(NULL);
	q=((dyt(*)(dyt))p)(obj);
	if(!q)return(NULL);

	return(obj);
}

// dyt spawn_ents(LBXGL_SEntity *ent)
{
	LBXGL_SEntity *cur;
	dyt tmp, lst;

	cur=ent; lst=NULL;
	while(cur)
	{
		tmp=spawn_ent(cur);
		if(tmp)lst=dycons(tmp, lst);
		cur=cur->next;
	}
	return(lst);
}
#endif

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

void draw_ent(LBXGL_SEntity *ent)
{
	float org[3], tv[3], tv1[3], tv2[3], ang[3];
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
	if(g<f)glColor4f(1, 0.5, 1, 1);

	if(ent==world->entity_sel)glColor4f(0.5, 1, 0.5, 1);

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
				cam->rt, f*state->dmx/400.0,
				cam->up, f*(-state->dmy)/400.0, tv);
			V3F_SUB(cam->org, cam->lorg, tv1);
			V3F_ADD(tv, tv1, tv);
		}

		V3F_ADD(org, tv, org);
		LBXGL_SEnt_SetVec3(ent, "origin", org);
	}

	tn=-1;

	tn=Tex_LoadFile("resource/images/mystery.png", &xs, &ys);
	f=4;

	if(!strncmp(ty, "light", 5))
	{
		tn=Tex_LoadFile("resource/images/light0.png", &xs, &ys);

//		f=(V3F_DOT(org, cam->fw)-
//			V3F_DOT(cam->org, cam->fw))/32;

		f=LBXGL_SEnt_GetNum(ent, "light");
//		f=bm_log2(f);
		f=sqrt(f)/4;
		if(f<1)f=1;
	}

	if(!strcmp(ty, "info_player_start"))
	{
		tn=Tex_LoadFile("resource/images/dude.png", &xs, &ys);
		f=36;
	}

	if(!strcmp(ty, "info_player_coop"))
	{
		tn=Tex_LoadFile("resource/images/dude_green.png", &xs, &ys);
		f=36;
	}

	if(!strcmp(ty, "info_player_deathmatch"))
	{
		tn=Tex_LoadFile("resource/images/dude_gun.png", &xs, &ys);
		f=36;
	}

	if(!strncmp(ty, "monster", 7))
	{
		tn=Tex_LoadFile("resource/images/dude_red.png", &xs, &ys);
		f=36;
	}

	if(!strncmp(ty, "weapon", 6))
	{
		tn=Tex_LoadFile("resource/images/gun.png", &xs, &ys);
		f=24;
	}

	if(!strncmp(ty, "item", 4))
	{
		tn=Tex_LoadFile("resource/images/bullet.png", &xs, &ys);
		f=24;
	}

	if(!strncmp(ty, "item_health", 11))
	{
		tn=Tex_LoadFile("resource/images/health.png", &xs, &ys);
		f=24;
	}

	if(!strncmp(ty, "item_armor", 10))
	{
		tn=Tex_LoadFile("resource/images/armor.png", &xs, &ys);
		f=24;
	}

	if(tn>=0)
	{
		pdglEnable(GL_ALPHA_TEST);

		pdglEnable(GL_TEXTURE_2D);
		pdglBindTexture(GL_TEXTURE_2D, tn);

		pdglBegin(GL_POLYGON);

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

		pdglDisable(GL_ALPHA_TEST);

	}

	s=LBXGL_SEnt_GetStr(ent, "_flare");
	if(s)LBXGL_Sprite_RenderFlareName(s, org, 1000);

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

			if(h<0)h=0;

			V3F_ADDSCALE(org, tv, h, ent->mdl->org);
		}

		LBXGL_Mdl_StepTime(ent->mdl, state->dt_f);
	}
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
	LBXGL_SEntity *cur;
	LBXGL_Light *lcur, *lnxt;
	char *ty, *s;
	float f, g;
	int i;

	lcur=world->light;
	while(lcur)
	{
		lnxt=lcur->next;
		gcfree(lcur);
		lcur=lnxt;
	}

	world->light=NULL;
	sun=NULL;

	cur=ent;
	while(cur)
	{
		ty=LBXGL_SEnt_GetStr(cur, "classname");
		if(!strncmp(ty, "light", 5))
		{
//			lcur=gcalloc(sizeof(LBXGL_Light));
			lcur=lbxEntNewLightWorld(world);
//			cur->light=lcur;

			i=LBXGL_SEnt_GetNum(cur, "_lightnum");
			if(i<=0)
			{
				i=lbxEntAllocId(world);
				LBXGL_SEnt_SetNum(cur, "_lightnum", i);
			}

			lcur->num=i;


			i=LBXGL_SEnt_GetVec3(cur, "color", lcur->clr);
			if(i<0)i=LBXGL_SEnt_GetVec3(cur, "_color", lcur->clr);
			if(i<0)
			{
				g=0.57735;
				V3F_SET(lcur->clr, g, g, g);
			}

			LBXGL_SEnt_GetVec3(cur, "origin", lcur->org);
			lcur->val[0]=LBXGL_SEnt_GetNum(cur, "light");
			lcur->val[1]=0;

			if(lcur->val[0]==0)
				lcur->val[0]=250;

			lcur->curval=lcur->val[0];

			i=LBXGL_SEnt_GetNum(cur, "linear");
			if(i)lcur->flags|=LBXGL_LFL_LINEAR;

			i=LBXGL_SEnt_GetNum(cur, "noshadows");
			if(i)lcur->flags|=LBXGL_LFL_NOSHADOW;

			i=LBXGL_SEnt_GetNum(cur, "lightvol");
			if(i)lcur->flags|=LBXGL_LFL_LIGHTVOL;

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

//			lcur->next=world->light;
//			world->light=lcur;
//			cur->light=lcur;
		}

#if 1
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

#if 1
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

#if 1
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
		sun=lbxEntNewLightWorld(world);
		lcur=sun;

		g=0.57735;
		V3F_SET(lcur->clr, g, g, g);

		lcur->org[0]=cam->org[0];
		lcur->org[1]=cam->org[1];
		lcur->org[2]=cam->org[2]+16384;

		lcur->val[0]=65536;
		lcur->val[1]=0;

		lcur->curval=lcur->val[0];
	}
}

void MAP_LoadFile(LBXGL_BrushWorld *wrl, char *name)
{
	LBXGL_BrushMap_LoadFile(world, name);
	spawn_ents(world);
}

void MAP_SaveFile(LBXGL_BrushWorld *wrl, char *name)
{
	LBXGL_BrushMap_SaveFile(world, name);
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

int ConCmds_Sky(char **a)
{
	LBXGL_Sky_LoadSky(a[1]);
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

int pdgl_main_startup(int argc, char *argv[])
{
	char *opts[4];
	char *s;
	void *p;
	int i, j, k, l;

	GfxFont_Init();
	Con_Init();
	ConCmds_Init();

	PDGL_InitASM(argc, argv);

//	ccLoadModule("scripts/e_mapper2.c");
	ccLoadModule("lbxgl2.h");

	PDGLUI_Init();
	UI_Camera_Init();

	tex_screen=Tex_ScreenshotTexture();
	LBXGL_Particle_Init();

	p=ccGetAddr("mapper_init");
	if(p) { ((int(*)(int argc, char **argv))p)(argc, argv); }

	ConCmds_Register("ent", "Get/Set entity property", ConCmds_Ent);
	ConCmds_Register("load", "Load map file", ConCmds_Load);
	ConCmds_Register("save", "Save map file", ConCmds_Save);

	ConCmds_Register("sky", "Set sky", ConCmds_Sky);

	ConCmds_Register("shadows", "Set Shadows", ConCmds_Shadows);

	UI_TexFinder_SetTitle("Textures");


	cam->org[0]=0;
	cam->org[1]=-10;
	cam->org[2]=16;

//	cam->ang[0]=90;
//	cam->ang[1]=0;
//	cam->ang[2]=0;



	PDGLUI_BeginMenu("menu");

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

#if 0
	PDGLUI_BeginSubMenu("Mode");

//	PDGLUI_MenuItem(NULL, NULL);

#if 0
	PDGLUI_MenuItem("mode_persp",	"Perspective 3D (SHIFT--P)");
	PDGLUI_MenuItem("mode_ortho",	"Ortho 3D       (SHIFT--O)");
	PDGLUI_MenuItem("mode_orth_xy",	"Ortho XY       (SHIFT--X)");
	PDGLUI_MenuItem("mode_orth_xz",	"Ortho XZ       (SHIFT--Z)");
	PDGLUI_MenuItem("mode_orth_yz",	"Ortho YZ       (SHIFT--Y)");
#endif
	PDGLUI_EndSubMenu();
#endif


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

	PDGLUI_BeginSubMenu("Help");
	PDGLUI_MenuItem("help_generic", "General");
	PDGLUI_MenuItem("help_about", "About");
	PDGLUI_EndSubMenu();

	p=BASM_GetPtr("mapper_menu_top");
	if(p) ((int (*)())p)();

	PDGLUI_EndMenu();


#if 0
	s=PDGLUI_BeginForm("fixedbox", "selbox");
	PDGLUI_FormProperty2I("origin", 400-(32*8), 300);

	PDGLUI_BeginWidget("vbox", NULL);


	PDGLUI_BeginWidget("list", "lst");
	PDGLUI_WidgetPropertyI("rows", 40);
	PDGLUI_WidgetPropertyI("cols", 32);
	PDGLUI_EndWidget();


	PDGLUI_BeginWidget("hbox", NULL);
	PDGLUI_BeginWidget("label", NULL);
	PDGLUI_WidgetPropertyS("label", "Key: ");
	PDGLUI_EndWidget();
	PDGLUI_BeginWidget("textbox", "key");
	PDGLUI_WidgetPropertyI("size", 28);
	PDGLUI_EndWidget();
	PDGLUI_EndWidget();

	PDGLUI_BeginWidget("hbox", NULL);
	PDGLUI_BeginWidget("label", NULL);
	PDGLUI_WidgetPropertyS("label", "Val: ");
	PDGLUI_EndWidget();
	PDGLUI_BeginWidget("textbox", "value");
	PDGLUI_WidgetPropertyI("size", 28);
	PDGLUI_EndWidget();
	PDGLUI_EndWidget();

	PDGLUI_BeginWidget("list", "plst");
	PDGLUI_WidgetPropertyI("rows", 16);
	PDGLUI_WidgetPropertyI("cols", 32);
	PDGLUI_EndWidget();

	PDGLUI_EndWidget();

	PDGLUI_EndForm();

#endif

	world=LBXGL_Brush_NewWorld();

	main_new();

	return(1);
}

int main_handle_input()
{
	char buf[64];
	float mins[3], maxs[3];
	float io[3], iv[4], fw[2], pt[4], f, g, v;
	float im[16], im1[16], *imv;

	LBXGL_Brush *brsh;
	void *p;
	int i, j, k;
	unsigned short *kcur;
	LBXGL_SEntPair *epcur;
	char *s;

	if(Con_IsDown() || ui_state || PDGLUI_HasFocus())
		return(0);

	if(state->mb&4)
	{
		PDGLUI_ShowMenu("menu", state->mx, -state->my);
		return(0);
	}

	s=PDGLUI_GetMenuSelection("menu");
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

//		if(!strcmp(s, "edit_undo"))Mesh_Undo();
//		if(!strcmp(s, "edit_redo"))Mesh_Redo();

//		if(!strcmp(s, "edit_cut"))Mesh_CmdSel(CMD_CUT);
//		if(!strcmp(s, "edit_copy"))Mesh_CmdSel(CMD_COPY);
//		if(!strcmp(s, "edit_paste"))Mesh_Paste();
//		if(!strcmp(s, "edit_dup"))Mesh_CmdSel(CMD_DUPLICATE);

		p=BASM_GetPtr("mapper_menu_event");
		if(p) ((int (*)(char *))p)(s);
	}

	p=BASM_GetPtr("mapper_handle_input");
	if(p) ((int (*)())p)();

	kcur=state->keys;
	while(*kcur)
	{
		if(PDGL_KeyDown(K_CTRL))
		{
			if(*kcur=='s')
			{
				if(world->map_name)
					MAP_SaveFile(world, world->map_name);
			}

			if(*kcur=='b')
			{
				LBXGL_BrushWorld_DrawBuild(world);
			}

			kcur++;
			continue;
		}

		if(*kcur==K_ENTER)
		{
			sel_brush_face();
			if(world->brush_sel_face<0)sel_ents();
			if((world->brush_sel_face<0) && !world->entity_sel)
				sel_brushes(world->brush);
		}

		if(*kcur=='T')
		{
			UI_TexFinder_SetTitle("Textures");
			ui_state=1;
		}
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
			LBXGL_SEnt_NewBasicLight(&world->entity,
				cam->org, 100);
		}

		if(*kcur=='L')
		{
//			world->lighting=!world->lighting;
//			Con_Printf("Lighting: %s\n",
//				world->lighting?"Enabled":"Disabled");

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

		if(*kcur=='f')
		{
			world->flashlight=!world->flashlight;
			Con_Printf("Flashlight: %s\n",
				world->flashlight?"Enabled":"Disabled");
		}

		if(*kcur=='d')
		{
			LBXGL_BrushWorld_NewBrushEntity(world, "func_door");
		}

		if(*kcur==K_INS)
		{
			LBXGL_BrushWorld_DrawFlush(world);

			V3F_ADDSCALE(cam->org, cam->fw, 24, pt);

			brsh=LBXGL_Brush_MakeCube(pt[0], pt[1], pt[2], 6, 6, 6,
				world->default_tex);
			LBXGL_BrushWorld_AddBrush(world, brsh);
		}

		if(*kcur==K_DEL)
		{
			if(world->entity_sel)
			{
				unlink_entity(world->entity_sel);
				kcur++;
				continue;
			}
		}

		if(*kcur=='p')
		{
//			V3F_COPY(cam->org, pvs_org);

			main_phys=!main_phys;
			Con_Printf("Physics: %s\n",
				main_phys?"Enabled":"Disabled");
		}

		if(*kcur=='s')
		{
			main_sun=!main_sun;
			Con_Printf("Sun: %s\n",
				main_sun?"Enabled":"Disabled");
		}

		kcur++;
	}

	UI_Camera_Update();
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

			world->entity_sel_key=dystring(s2);
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

#if 0
		if(!strcmp(frm, "selbox") &&
			!strcmp(s0, "rename") &&
			!strcmp(s1, "submit"))
		{
			i=main_skel->bone_sel;
			if(i>=0)
			{
				s=main_skel->mdl->bone_name[i];
				main_skel->mdl->bone_name[i]=dystring(s2);

				printf("Rename Bone %s -> %s", s, s2);
			}
			PDGLUI_SetWidgetPropertyS(
				"bones", "rename", "text", "");
		}

		if(!strcmp(frm, "bones") && !strcmp(s0, "show_bones"))
		{
			if(!strcmp(s1, "clicked"))main_showbones=1;
			if(!strcmp(s1, "unclicked"))main_showbones=0;
		}
		if(!strcmp(frm, "bones") && !strcmp(s0, "show_solids"))
		{
			if(!strcmp(s1, "clicked"))main_showsolids=1;
			if(!strcmp(s1, "unclicked"))main_showsolids=0;
		}
#endif
	}
}

void main_handle_events()
{
//	main_handle_form("help");
//	main_handle_form("selbox");
}

int pdgl_main_draw3d()
{
	LBXGL_Sprite_SetCameraOrgRot(cam->org, cam->rot);

//	Draw_SetPerspective_3D(4.0/3.0, 90, 0,
//		cam->org, cam->ang, 0, 0, state->xs, state->ys);

	LBXGL_Sky_DrawSky();

	pdglDisable(GL_CULL_FACE);
	pdglDisable(GL_TEXTURE_2D);
//	PD3D_DrawGrid(0, 0, 16, 1);

	pdglBegin(GL_LINES);

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

//	pdglEnable(GL_CULL_FACE);

	pdglEnable(GL_TEXTURE_2D);

	setup_light_ents(world->entity);

	LBXGL_BrushWorld_DrawBrushes(world, world->brush);
	draw_ents(world->entity);

	draw_test_defsdr();
	draw_test_terrain();

	LBXGL_Particle_Draw();
}

int pdgl_main_body()
{
	static int t;
	static float adt=1.0/30;
	char buf[64];
	float mins[3], maxs[3];
	float io[3], iv[4], fw[2], pt[4], f, g, v;
	float im[16], im1[16], *imv;

	LBXGL_Brush *brsh;
	LBXGL_SEntity *ecur;
	LBXGL_SEntPair *epcur;

	int i, j, k;
	unsigned short *kcur;
	char *s;

	main_handle_input();

//	main_time+=state->dt_f;
	LBXGL_Particle_Step(state->dt_f);

	if(main_phys)
		LBXGL_BrushPhys_StepWorld(world, state->dt_f);

	LBXGL_BrushWorld_DrawTestAutovis(world);

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
	}else
	{
		UI_Camera_Setup3D();
		pdgl_main_draw3d();
	}


	LBXGL_BrushPhys_UpdateWorld(world);


//	Tex_FillTextureScreenshot(tex_screen);


	UI_Camera_Setup2D();

//	Draw_SetSolid2_2D(4.0/3.0, 400,
//		VEC3(0, 0, 0), VEC3(0, 0, 0), 0, 0, state->xs, state->ys);

	if(0)
	{
		pdglColor4f(1, 1, 1, 1);
		pdglEnable(GL_TEXTURE_2D);
		pdglBindTexture(GL_TEXTURE_2D, tex_screen);

		pdglBegin(GL_POLYGON);

		pdglTexCoord2f(0, 0);
		pdglVertex2f(400-16*8, -300);

		pdglTexCoord2f(1, 0);
		pdglVertex2f(400, -300);

		pdglTexCoord2f(1, 1);
		pdglVertex2f(400, -300+16*8);

		pdglTexCoord2f(0, 1);
		pdglVertex2f(400-16*8, -300+16*8);

		pdglEnd();
	}

	Con_Render();

	GfxFont_SetFont("fixed", 0);

	adt=(adt*0.90)+(state->dt_f*0.10);
	sprintf(buf, "(%g %g %g  %g) (%g %g %g  %g %g %g) %.2ffps",
		cam->org[0], cam->org[1], cam->org[2], cam->org[3],
		cam->ang[0], cam->ang[1], cam->ang[2],
			cam->ang[3], cam->ang[4], cam->ang[5],
		1.0/adt);
	GfxFont_DrawString(buf, -400, 300-12,
		8, 8,  0, 255, 0, 255);

	i=0;
	ecur=world->entity;
	while(ecur && (i<25))
	{
		j=7;
		if(ecur==world->entity_psel)j=3;
		if(ecur==world->entity_sel)j=2;

		s=LBXGL_SEnt_GetStr(ecur, "classname");
		GfxFont_DrawString(s, 400-24*8, 300-(i+2)*8,
			8, 8,  (j&1)?255:0, (j&2)?255:0, (j&4)?255:0, 255);

		ecur=ecur->next;
		i++;
	}

	if(world->brush_sel)
	{
		if(world->brush_sel->se_owner)
		{
			world->entity_psel=world->brush_sel->se_owner;

			s=LBXGL_SEnt_GetStr(world->brush_sel->se_owner, "classname");
			sprintf(buf, "Owner: %s", s);
			GfxFont_DrawString(buf, 400-24*8, -300+25*8,
				8, 8,  0, 255, 0, 255);
		}


		i=world->brush_sel_face;
		if(i<0)i=0;

		sprintf(buf, "%s", world->brush_sel->tex[i]);
		GfxFont_DrawString(buf, 400-24*8, -300+24*8,
			8, 8,  0, 255, 0, 255);


		sprintf(buf, "resource/textures/%s", world->brush_sel->tex[i]);
		j=Tex_LoadFile(buf, NULL, NULL);

		pdglColor4f(1, 1, 1, 1);
		pdglEnable(GL_TEXTURE_2D);
		pdglBindTexture(GL_TEXTURE_2D, j);

		pdglBegin(GL_POLYGON);

		pdglTexCoord2f(0, 0);
		pdglVertex2f(400-16*8, -300);

		pdglTexCoord2f(1, 0);
		pdglVertex2f(400, -300);

		pdglTexCoord2f(1, 1);
		pdglVertex2f(400, -300+16*8);

		pdglTexCoord2f(0, 1);
		pdglVertex2f(400-16*8, -300+16*8);

		pdglEnd();
	}

	if(world->entity_sel)
	{
		epcur=world->entity_sel->keys;

		i=0;
		while(epcur)
		{
			sprintf(buf, "\"%s\" \"%s\"",
				epcur->name, epcur->value);
			GfxFont_DrawString(buf, 400-32*8, -300+24*8-i*8,
				8, 8,  0, 255, 0, 255);

			i++;
			epcur=epcur->next;
		}
	}

	if(ui_state==1)
	{
		UI_TexFinder_Draw();

		s=UI_TexFinder_GetName();
		if(s)
		{
			ui_state=0;

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

#if 0
	PDGLUI_ClearItems("selbox", "lst");
	ecur=world->entity; i=0; j=-1;
	while(ecur)
	{
		if(ecur==world->entity_sel)j=i;

		s=LBXGL_SEnt_GetStr(ecur, "classname");
		PDGLUI_AddItem("selbox", "lst", s, s);

		ecur=ecur->next;
		i++;
	}

	PDGLUI_SetWidgetPropertyI("selbox", "lst", "value", j);


	PDGLUI_ClearItems("selbox", "plst");
	if(world->entity_sel)
	{
		epcur=world->entity_sel->keys;

		i=0;
		while(epcur)
		{
			sprintf(buf, "\"%s\" \"%s\"",
				epcur->name, epcur->value);
			PDGLUI_AddItem("selbox", "plst", epcur->name, buf);

			i++;
			epcur=epcur->next;
		}

		if(world->entity_sel_key)
			PDGLUI_SetWidgetPropertyS("selbox", "plst",
				"value", world->entity_sel_key);
	}
#endif


	PDGLUI_Frame();
	main_handle_events();

	pdglDisable(GL_TEXTURE_2D);
	pdglColor4f(1, 0, 0, 1);

	pdglBegin(GL_LINES);
	pdglVertex2f(state->mx-10, -state->my);
	pdglVertex2f(state->mx+10, -state->my);
	pdglVertex2f(state->mx, -state->my-10);
	pdglVertex2f(state->mx, -state->my+10);
	pdglEnd();

	return(0);
}

int pdgl_main_shutdown()
{
	printf("app1 shutdown\n");
	return(0);
}
