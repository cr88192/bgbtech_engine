// #include <windows.h>
// #include <GL/gl.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI		3.1415926535897932384626433832795
#endif

#define PDGL_BASEHEAP		(64*1024*1024)
#define PDGL_BASEHEAP_LIMIT	(256*1024*1024)
#define PDGL_BASEHEAP_LOBJ	(1024*1024*1024)

#define PDGL_PRESTART


#include <pdgl.h>
#include <pdglui.h>
#include <pdgl_stub.h>

#include <lbxgl2.h>
#include <libvecmath.h>

#include <bgbasm.h>
#include <bgbccc.h>

int main_phys=0;

LBXGL_BrushWorld *world;

int ui_state;
int shader_phong;
int shader_phong_spot;
int tex_screen;

char *map_name=NULL;
int map_stat_brushes;
int map_stat_ents;
int map_stat_lights;

LBXGL_API LBXGL_BrushWorld *lbxgl_brushmesh_world;

LBXGL_API LBXGL_MeshGroup *lbxgl_brushmesh_mdl;
LBXGL_API LBXGL_MeshGroup *lbxgl_meshsel_grp;
LBXGL_API LBXGL_MeshPrim *lbxgl_meshsel_prim;
LBXGL_API int lbxgl_meshsel_face, lbxgl_meshsel_vec;
LBXGL_API int lbxgl_meshsel_multi;

LBXGL_API int lbxgl_brushmesh_mode;
LBXGL_API int lbxgl_brushmesh_cmd;
LBXGL_API LBXGL_MeshGroup *lbxgl_brushmesh_clip;

// int lbxgl_mapedit_dirty;
// int lbxgl_mapedit_lutime;
// LBXGL_BrushWorld *lbxgl_mapedit_undo[16];
// LBXGL_BrushWorld *lbxgl_mapedit_redo[16];

void MapEdit_AddUndo();
void MapEdit_ClearSel();

static double bm_log2(double x)
	{ return(log(x)/log(2)); }

#if 0
LBXGL_BrushWorld *MapEdit_CloneWorld(LBXGL_BrushWorld *world)
{
	return(LBXGL_Brush_CloneWorld(world));
}

void MapEdit_DestroyWorld(LBXGL_BrushWorld *world)
{
	LBXGL_Brush_FreeWorld(world);
}

void MapEdit_RelinkMesh()
{
	LBXGL_Brush *cur;

	if(!lbxgl_brushmesh_mdl)
		lbxgl_brushmesh_mdl=gctalloc("lbxgl_meshgroup_t", sizeof(LBXGL_MeshGroup));
	lbxgl_brushmesh_mdl->prim=NULL;
	
	cur=world->brush;
	while(cur)
	{
		if(cur->mesh)
		{
			cur->mesh->next=lbxgl_brushmesh_mdl->prim;
			lbxgl_brushmesh_mdl->prim=cur->mesh;
		}
		cur=cur->next;
	}
}

void MapEdit_SetDirty()
{
	if(lbxgl_mapedit_dirty &&
			((state->time_f*1000)>(lbxgl_mapedit_lutime+1000)))
		lbxgl_mapedit_dirty=0;

	if(lbxgl_mapedit_dirty)return;
	MapEdit_AddUndo();

	lbxgl_mapedit_dirty=1;
	lbxgl_mapedit_lutime=state->time_f*1000;
}

void MapEdit_AddUndo()
{
	int i;

	dyPrintf("AddUndo\n");

	for(i=0; i<16; i++)
	{
		if(lbxgl_mapedit_redo[i])
			MapEdit_DestroyWorld(lbxgl_mapedit_redo[i]);
		lbxgl_mapedit_redo[i]=NULL;
	}

	if(lbxgl_mapedit_undo[15])
		MapEdit_DestroyWorld(lbxgl_mapedit_undo[15]);
	for(i=15; i>0; i--)
		lbxgl_mapedit_undo[i]=lbxgl_mapedit_undo[i-1];
	lbxgl_mapedit_undo[0]=MapEdit_CloneWorld(world);
	lbxgl_mapedit_dirty=0;
}

void MapEdit_Undo()
{
	int i;

	if(!lbxgl_mapedit_undo[0])
		return;

	dyPrintf("Undo\n");

	MapEdit_ClearSel();

	if(lbxgl_mapedit_redo[15])
		MapEdit_DestroyWorld(lbxgl_mapedit_redo[15]);
	for(i=15; i>0; i--)
		lbxgl_mapedit_redo[i]=lbxgl_mapedit_redo[i-1];

	lbxgl_mapedit_redo[0]=world;
	world=lbxgl_mapedit_undo[0];
	lbxgl_brushmesh_world=world;

	for(i=0; i<15; i++)
		lbxgl_mapedit_undo[i]=lbxgl_mapedit_undo[i+1];
	lbxgl_mapedit_undo[15]=NULL;

	MapEdit_ClearSel();
	MapEdit_RelinkMesh();
}

void MapEdit_Redo()
{
	int i;

	if(!lbxgl_mapedit_redo[0])
		return;

	dyPrintf("Redo\n");

	MapEdit_ClearSel();

	if(lbxgl_mapedit_undo[15])
		MapEdit_DestroyWorld(lbxgl_mapedit_undo[15]);
	for(i=15; i>0; i--)
		lbxgl_mapedit_undo[i]=lbxgl_mapedit_undo[i-1];

	lbxgl_mapedit_undo[0]=world;
	world=lbxgl_mapedit_redo[0];
	lbxgl_brushmesh_world=world;

	for(i=0; i<15; i++)
		lbxgl_mapedit_redo[i]=lbxgl_mapedit_redo[i+1];
	lbxgl_mapedit_redo[15]=NULL;

	MapEdit_ClearSel();
	MapEdit_RelinkMesh();
}

//determine if a given mesh is attached to a brush
int MapEdit_CheckPrimHasBrush(LBXGL_MeshPrim *prim)
{
	LBXGL_Brush *bcur;
	
	bcur=world->brush;
	while(bcur)
	{
		if(bcur->mesh==prim)
			return(1);
		bcur=bcur->next;
	}
	return(0);
}

void MapEdit_CmdSel(int cmd)
{
	LBXGL_MeshPrim *pcur;
	LBXGL_Brush *brsh;

	if(lbxgl_brushmesh_mode==LBXGL_EDMODE_BRUSH)
	{
	}else
	{
		LBXGL_BrushWorld_DrawFlush(world);
		MapEdit_SetDirty();
//		MapEdit_AddUndo();
		LBXGL_BrushMesh_CmdSel(cmd);
		
		pcur=lbxgl_brushmesh_mdl->prim;
		while(pcur)
		{
			//add any new meshes to world
			if(!MapEdit_CheckPrimHasBrush(pcur))
			{
				brsh=LBXGL_BrushMesh_MakeBrushUsingPrim(pcur);
				LBXGL_BrushWorld_AddBrush(world, brsh);
			}

			pcur=pcur->next;
		}
	}
}

void MapEdit_ClearSel()
{
	LBXGL_BrushMesh_ClearSel();
	world->brush_sel=NULL;
	world->entity_sel=NULL;
}
#endif

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

void draw_test_defsdr()
{
	int sdr, tex, def;

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

dyt spawn_ents(LBXGL_SEntity *ent)
{
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
		if(!strncmp(ty, "light_torch", 11))
		{
			tn=Tex_LoadFile("resource/images/torch.png", &xs, &ys);
		}else if(!strncmp(ty, "light_flame", 11))
		{
			tn=Tex_LoadFile("resource/images/flame.png", &xs, &ys);
		}else
		{
			tn=Tex_LoadFile("resource/images/light0.png", &xs, &ys);
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

	if(!strncmp(ty, "info_player_", 12))
	{
		tn=Tex_LoadFile("resource/images/dude.png", &xs, &ys);
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

	if(!strcmp(ty, "path_corner"))
	{
		tn=Tex_LoadFile("resource/images/path0.png", &xs, &ys);
		f=8;
	}

	if(!strcmp(ty, "info_teleport_destination"))
	{
		tn=Tex_LoadFile("resource/images/teledest.png", &xs, &ys);
		f=8;
	}

	if(!strcmp(ty, "misc_explobox") ||
		!strcmp(ty, "misc_explobox2"))
	{
		tn=Tex_LoadFile("resource/images/barrel.png", &xs, &ys);
		f=16;
	}

	if(!strncmp(ty, "ambient_", 8))
	{
		tn=Tex_LoadFile("resource/images/speaker.png", &xs, &ys);
		f=8;
	}

	if(!strcmp(ty, "target_speaker"))
	{
		tn=Tex_LoadFile("resource/images/speaker.png", &xs, &ys);
		f=8;
	}

	if(tn>=0)
	{
//		pdglEnable(GL_ALPHA_TEST);

		pdglEnableTexture2D();
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
	world->light_sun=NULL;

	cur=ent;
	while(cur)
	{
		ty=LBXGL_SEnt_GetStr(cur, "classname");
		if(!strncmp(ty, "light", 5))
		{
			lcur=gcalloc(sizeof(LBXGL_Light));

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

			lcur->next=world->light;
			world->light=lcur;
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

//			V3F_ZERO(tv);
//			f=25*bgbrng_randf()*state->dt_f;
//			if(f<1.0)f=1.0;
//			LBXGL_Particle_Spawn("flame", (int)f, org, tv);
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
}


void load_file(char *name)
{
	gcDelayGC(2);
	LBXGL_BrushMap_LoadFile(world, name);
	map_name=dystrdup(name);
	gcDelayGC(0);

	MapEdit_RelinkMesh();
}

void save_file(char *name)
{
	LBXGL_BrushMap_SaveFile(world, name);
	map_name=dystrdup(name);
}

int ConCmds_Load(char **a)
{
	load_file(a[1]);
}

int ConCmds_Save(char **a)
{
	save_file(a[1]);
}

int ConCmds_Map(char **a)
{
	char tb[256];
	
	sprintf(tb, "maps/%s.map", a[1]);
	load_file(tb);
	return(0);
}

int ConCmds_Sky(char **a)
{
	LBXGL_Sky_LoadSky(a[1]);
}

int ConCmds_Ent(char **a)
{
	LBXGL_SEntity *ent;
	char *s;

	if(!a[1])
	{
		Con_Printf("ent <property> [value]\n");
		return(0);
	}

	ent=world->entity_sel;
	if(!ent)ent=world->entity_psel;

	if(!ent)
	{
		Con_Printf("no entity selected\n");
		return(0);
	}

	if(a[2])
	{
		LBXGL_SEnt_SetStr(ent, a[1], a[2]);
		return(0);
	}else
	{
		s=LBXGL_SEnt_GetStr(ent, a[1]);
		if(s)Con_Printf("\"%s\" \"%s\"\n", a[1], s);
			else Con_Printf("no property \"%s\"\n", a[1]);
		return(0);
	}
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

int ConCmds_GridSnap(char **a)
{
	if(!a[1])
	{
		Con_Printf("GridSnap=%d\n", world->grid_snap);
		return(0);
	}

	world->grid_snap=atoi(a[1]);
	return(0);
}

int ConCmds_Eval(char **a)
{
	char tb[1024];
	char **ap;
	dyt p;
	char *s, *t;
	int i;
	
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
	
	p=BSVM_EvalString(tb);
	dyPrint(p);
	dyPrintf("\n");
}

int main_new()
{
	map_name=NULL;
	MapEdit_NewWorld(world);
	return(0);
}

#if 0
int main_new()
{
	float v[3];
	LBXGL_Brush *brsh;

	LBXGL_BrushWorld_DrawFlush(world);

	map_name=NULL;
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
}
#endif

char *games[16], *start_map;
int n_games;

//called before main window creation
int pdgl_main_prestart(int argc, char *argv[])
{
	char tb[256];
	char *opts[4];
	char *s;
	void *p;
	int xs, ys;
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
		
		sprintf(tb, "tool_%s", games[i]);
		dyllLoadLibrary(tb);
	}
#endif

	Con_Init();
	ConCmds_Init();

#if 1
	btCvarSetf("deathmatch", 0);
	btCvarSetf("coop", 0);
	btCvarSetf("sv_cheats", 0);
	btCvarSetf("cl_showentids", 0);
	btCvarSetf("r_lightbudget", 0);
	btCvarSetf("r_noearlyout", 0);
	btCvarSetf("r_nohires", 0);

	btCvarSet("r_title", "BGBTech MeshTool");
	btCvarSetf("r_width", xs);
	btCvarSetf("r_height", ys);

	btCvarSetf("r_usefbo", 1);
	btCvarSetf("r_usehdr", 1);
	btCvarSetf("r_usebloom", 0);
	btCvarSetf("r_parallax", 1);
	btCvarSetf("r_cellshade", 0);
#endif

	bsvmAddScriptPath("game");

	Con_ExecCmdBuf("game/default.cfg");
	Con_DoExecCmds();

	Con_ExecCmdBuf("game/config.cfg");
	Con_DoExecCmds();

//	Con_ExecCmdBuf("game/autoexec.cfg");

	s=btCvarGet("r_title");
	xs=btCvarGetf("r_width");
	ys=btCvarGetf("r_height");

	GfxDrv_SetDefaults(s, xs, ys);
//	Con_ReInit();

	return(1);
}

int pdgl_main_startup(int argc, char *argv[])
{
	char *opts[4];
	char *s;
	void *p;
	int i, j, k, l;

	GfxFont_Init();
//	Con_Init();
//	ConCmds_Init();

	Con_ReInit();

//	PDGL_InitASM(argc, argv);
//	ccLoadModule("scripts/e_mapper2.c");
//	ccLoadModule("lbxgl2.h");

	PDGLUI_Init();
	UI_Camera_Init();

	tex_screen=Tex_ScreenshotTexture();
	LBXGL_Particle_Init();

	p=dyllGetAddr("mapper_init");
	if(p) { ((int(*)(int argc, char **argv))p)(argc, argv); }

	ConCmds_Register("ent", "Get/Set entity property", ConCmds_Ent);
	ConCmds_Register("load", "Load map file", ConCmds_Load);
	ConCmds_Register("save", "Save map file", ConCmds_Save);

	ConCmds_Register("map", "Load map", ConCmds_Map);
	ConCmds_Register("sky", "Set sky", ConCmds_Sky);
	ConCmds_Register("shadows", "Set Shadows", ConCmds_Shadows);
	ConCmds_Register("gamma", "Set Gamma", ConCmds_Gamma);
	ConCmds_Register("lighting", "Set Lighting", ConCmds_Lighting);
	ConCmds_Register("gridsnap", "Set Grid Snap", ConCmds_GridSnap);

	ConCmds_Register("eval", "Eval Expression", ConCmds_Eval);
	ConCmds_Register("bs", "Eval Expression", ConCmds_Eval);
	ConCmds_Register(";", "Eval Expression", ConCmds_Eval);

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
	PDGLUI_MenuItem("edit_redo",	"Redo           (CTRL--Y)");
	PDGLUI_MenuItem(NULL, NULL);
	PDGLUI_MenuItem("edit_cut",	"Cut            (CTRL--X)");
	PDGLUI_MenuItem("edit_copy",	"Copy           (CTRL--C)");
	PDGLUI_MenuItem("edit_paste",	"Paste          (CTRL--V)");
	PDGLUI_MenuItem("edit_dup",	"Duplicate      (CTRL--D)");
	PDGLUI_EndSubMenu();

#if 1
	PDGLUI_BeginSubMenu("Mode");

	PDGLUI_MenuItem("mode_group",	"Group          (G)");
	PDGLUI_MenuItem("mode_object",	"Object         (O)");
	PDGLUI_MenuItem("mode_face",	"Face           (F)");
	PDGLUI_MenuItem("mode_vertex",	"Vertex         (V)");
	PDGLUI_MenuItem("mode_texture",	"Texture        (SHIFT--T)");
	PDGLUI_MenuItem("mode_brush",	"Brush          (SHIFT--T)");

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

	PDGLUI_BeginSubMenu("Tool");
	PDGLUI_MenuItem("tool_sel",	"Select         (S)");
	PDGLUI_MenuItem("tool_trans",	"Translate      (T)");
	PDGLUI_MenuItem("tool_rot",	"Rotate         (R)");
	PDGLUI_MenuItem("tool_scale",	"Scale          (SHIFT--S)");
	PDGLUI_MenuItem("tool_scale2",	"Scale HV       (SHIFT--A)");
	PDGLUI_EndSubMenu();

	PDGLUI_BeginSubMenu("Create");
	PDGLUI_MenuItem("create_cube",	"Cube");
	p=BASM_GetPtr("mapper_menu_create");
	if(p) ((int (*)())p)();
	PDGLUI_EndSubMenu();

	PDGLUI_BeginSubMenu("Command");
	PDGLUI_MenuItem("cmd_delete", 	"Delete Selection        (DEL)");
	PDGLUI_MenuItem("cmd_subdiv",	"Subdivide Object//Faces  (SHIFT--D)");
	PDGLUI_MenuItem("cmd_extrude", 	"Extrude Face            (SHIFT--E)");
	PDGLUI_MenuItem("cmd_merge", 	"Merge Vertices//Objects  (SHIFT--M)");
	PDGLUI_MenuItem("cmd_invert", 	"Invert//Flip Normal      (SHIFT--N)");
	PDGLUI_MenuItem("cmd_settex",	"Set Texture             (CTRL--T)");

	p=BASM_GetPtr("mapper_menu_command");
	if(p) ((int (*)())p)();
	PDGLUI_EndSubMenu();

	PDGLUI_BeginSubMenu("Texture");
//	PDGLUI_MenuItem("tex_set",	"Set Texture             (CTRL--T)");
	PDGLUI_MenuItem("cmd_settex",	"Set Texture             (CTRL--T)");
	PDGLUI_MenuItem("tex_plane",	"Planar Projection       (CTRL--SHIFT--P)");
	PDGLUI_MenuItem("tex_box",	"Box Projection          (CTRL--SHIFT--B)");
	PDGLUI_MenuItem("tex_sphere",	"Spherical Projection    (CTRL--SHIFT--S)");
	PDGLUI_MenuItem("tex_cylinder",	"Cylindrical Projection  (CTRL--SHIFT--C)");

	PDGLUI_MenuItem(NULL, NULL);

	PDGLUI_MenuItem("tex_proj_xy",	"Project Texture XY      ()");
	PDGLUI_MenuItem("tex_proj_xz",	"Project Texture XZ      ()");
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
	world->grid_snap=1;

	lbxgl_brushmesh_world=world;

	lbxgl_brushmesh_mdl=gctalloc("lbxgl_meshgroup_t", sizeof(LBXGL_MeshGroup));

//	cam->mode=PDGLUI_CAM_PERSP;
//	cam->mode=PDGLUI_CAM_CAD4V;
	lbxgl_brushmesh_mode=LBXGL_EDMODE_BRUSH;

	main_new();

	return(1);
}

int main_handle_input()
{
	char buf[256];
	float mins[3], maxs[3];
	float io[3], iv[4], fw[2], pt[4], f, g, v;
	float im[16], im1[16], *imv;

	LBXGL_Brush *brsh;
	LBXGL_MeshPrim *prim;
	void *p;
	int i, j, k;
	unsigned short *kcur;
	LBXGL_SEntPair *epcur;
	char *s;

	if(Con_IsDown() || ui_state || PDGLUI_HasFocus())
		return(0);

	UI_Camera_Update();

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
			if(map_name)save_file(map_name);
		}

//		if(!strcmp(s, "edit_undo"))Mesh_Undo();
//		if(!strcmp(s, "edit_redo"))Mesh_Redo();

//		if(!strcmp(s, "edit_cut"))Mesh_CmdSel(CMD_CUT);
//		if(!strcmp(s, "edit_copy"))Mesh_CmdSel(CMD_COPY);
//		if(!strcmp(s, "edit_paste"))Mesh_Paste();
//		if(!strcmp(s, "edit_dup"))Mesh_CmdSel(CMD_DUPLICATE);

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

//		p=BASM_GetPtr("mapper_menu_event");
//		if(p) ((int (*)(char *))p)(s);
	}

//	p=BASM_GetPtr("mapper_handle_input");
//	if(p) ((int (*)())p)();

	kcur=state->keys;
	while(kcur && *kcur)
	{
		if(PDGL_KeyDown(K_CTRL))
		{
			if(*kcur=='z')MapEdit_CmdUndo();
			if(*kcur=='y')MapEdit_CmdRedo();

			if(*kcur=='c')MapEdit_CmdSel(LBXGL_EDCMD_COPY);
			if(*kcur=='x')MapEdit_CmdSel(LBXGL_EDCMD_CUT);
			if(*kcur=='d')MapEdit_CmdSel(LBXGL_EDCMD_DUPLICATE);
			if(*kcur=='v')LBXGL_BrushMesh_Paste();

//			if(*kcur=='o')
//			{
//				UI_LoadSave_SetTitle("Open", "map");
//				main_loadsave=1;
//			}

			if(*kcur=='s')
			{
				if(map_name)save_file(map_name);
			}

			if(*kcur=='b')
			{
//				lbxgl_brushmesh_mode=LBXGL_EDMODE_BRUSH;
//				MapEdit_ClearSel();
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

		if(*kcur=='g') { lbxgl_brushmesh_mode=LBXGL_EDMODE_GROUP; MapEdit_ClearSel(); }
		if(*kcur=='f') { lbxgl_brushmesh_mode=LBXGL_EDMODE_FACE; MapEdit_ClearSel(); }
		if(*kcur=='v') { lbxgl_brushmesh_mode=LBXGL_EDMODE_VERTEX; MapEdit_ClearSel(); }
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

//		if(*kcur=='T')
//		{
//			UI_TexFinder_SetTitle("Textures");
//			ui_state=1;
//		}

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

		if(*kcur=='F')
		{
			world->flashlight=!world->flashlight;
			Con_Printf("Flashlight: %s\n",
				world->flashlight?"Enabled":"Disabled");
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

					world->brush_sel->flags|=LBXGL_BRFL_DEL;
					LBXGL_Brush_MakeRoomFromBrush(world, world->brush_sel);
					world->brush_sel=NULL;
				}
			}
		}

		if(*kcur=='-')
		{
			if(world->brush_sel)
			{
				MapEdit_AddUndo();
				LBXGL_BrushWorld_DrawFlush(world);
				LBXGL_Brush_SubtractBrush(world, world->brush_sel);
			}
		}

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

				brsh=LBXGL_Brush_MakeCube(pt[0], pt[1], pt[2], 6, 6, 6,
					world->default_tex);
				LBXGL_BrushWorld_AddBrush(world, brsh);
			}else if(lbxgl_brushmesh_mode==LBXGL_EDMODE_OBJECT)
			{
				MapEdit_AddUndo();
				LBXGL_BrushWorld_DrawFlush(world);

				prim=LBXGL_BrushMesh_CreateCube();
				brsh=LBXGL_BrushMesh_MakeBrushUsingPrim(prim);
				LBXGL_BrushWorld_AddBrush(world, brsh);

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

				world->brush_sel->flags|=LBXGL_BRFL_DEL;
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
//				MapEdit_AddUndo();
				LBXGL_BrushWorld_DrawFlush(world);
				MapEdit_CmdSel(LBXGL_EDCMD_DELETE);
			}
		}

		if(*kcur=='p')
		{
//			V3F_COPY(cam->org, pvs_org);

			main_phys=!main_phys;
			Con_Printf("Physics: %s\n",
				main_phys?"Enabled":"Disabled");
		}

//		if(*kcur==K_DEL)MapEdit_CmdSel(LBXGL_EDCMD_DELETE);
		if(*kcur=='E')MapEdit_CmdSel(LBXGL_EDCMD_EXTRUDE);
		if(*kcur=='D')MapEdit_CmdSel(LBXGL_EDCMD_SUBDIVIDE);
		if(*kcur=='M')MapEdit_CmdSel(LBXGL_EDCMD_MERGE);
		if(*kcur=='N')MapEdit_CmdSel(LBXGL_EDCMD_INVERT);

		if((lbxgl_brushmesh_mode==LBXGL_EDMODE_VERTEX) && (*kcur=='F'))
		{
			MapEdit_AddUndo();
			LBXGL_BrushMesh_CreateFace(lbxgl_meshsel_prim);
		}

		kcur++;
	}

	if(PDGL_KeyDown(K_CTRL))
	{
		if(!(state->mb&1) && (state->lmb&1))
		{
			i=(state->mx>state->omx)?(state->mx-state->omx):(state->omx-state->mx);
			j=(state->my>state->omy)?(state->my-state->omy):(state->omy-state->my);

			if((i>5) && (j>5))
			{
//				dyPrintf("Sel Box\n");
				if(!PDGL_KeyDown(K_SHIFT))
					MapEdit_ClearSel();
				LBXGL_BrushMesh_SelGroupBox(lbxgl_brushmesh_mdl);
				return(0);
			}
		}

		return(0);
	}

	if((lbxgl_brushmesh_cmd==LBXGL_EDCMD_SEL) && (state->mb&1) && !(state->lmb&1))
	{
		if(!PDGL_KeyDown(K_SHIFT) && lbxgl_meshsel_multi)
			MapEdit_ClearSel();
		LBXGL_BrushMesh_SelGroup(lbxgl_brushmesh_mdl);
		return(0);
	}

	if((lbxgl_brushmesh_cmd==LBXGL_EDCMD_SEL) && !(state->mb&1) && (state->lmb&1))
	{
		i=(state->mx>state->omx)?(state->mx-state->omx):(state->omx-state->mx);
		j=(state->my>state->omy)?(state->my-state->omy):(state->omy-state->my);

		if((i>5) && (j>5))
		{
//			dyPrintf("Sel Box\n");
			if(!PDGL_KeyDown(K_SHIFT))
				MapEdit_ClearSel();
			LBXGL_BrushMesh_SelGroupBox(lbxgl_brushmesh_mdl);
			return(0);
		}
	}

	if((lbxgl_brushmesh_cmd!=LBXGL_EDCMD_SEL) && (state->mb&1))
	{
//		LBXGL_BrushMesh_CmdSelGroup(lbxgl_brushmesh_mdl, lbxgl_brushmesh_cmd);
		MapEdit_CmdSel(lbxgl_brushmesh_cmd);
		return(0);
	}

	return(0);
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

				dyPrintf("Rename Bone %s -> %s", s, s2);
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
	float pt[4];

	LBXGL_Brush *cur;
	int i;

	LBXGL_Sprite_SetCameraOrgRot(cam->org, cam->rot);

	if(!cam->ortho)
		LBXGL_Sky_DrawSky();

//	PD3D_DrawGrid(0, 0, 16, 1);

	i=0;
	if(cam->width<=(16*16))i=1;
	if(cam->width<=(4*16))i=2;
	if(cam->width<=4)i=3;

	pdglDisable(GL_CULL_FACE);
	pdglDisable(GL_TEXTURE_2D);

//	pdglDepthFunc(GL_LEQUAL);
	pdglDepthFunc(GL_ALWAYS);

//	switch(cam->mode)
	switch(cam->ortho)
	{
	case 1:
	case 2:
		pdglColor4f(0.1125, 0.1125, 0.1125, 1);
		if(i>2)PD3D_DrawXYGrid2(4096, 1.0/16);
		pdglColor4f(0.125, 0.125, 0.125, 1);
		if(i>1)PD3D_DrawXYGrid2(4096, 1);
		pdglColor4f(0.25, 0.25, 0.25, 1);
		if(i>0)PD3D_DrawXYGrid2(4096, 16);
		pdglColor4f(0.5, 0.5, 0.5, 1);
		PD3D_DrawXYGrid2(4096, 4*16);
		break;
	case 3:
		pdglColor4f(0.1125, 0.1125, 0.1125, 1);
		if(i>2)PD3D_DrawXZGrid2(4096, 1.0/16);
		pdglColor4f(0.125, 0.125, 0.125, 1);
		if(i>1)PD3D_DrawXZGrid2(4096, 1);
		pdglColor4f(0.25, 0.25, 0.25, 1);
		if(i>0)PD3D_DrawXZGrid2(4096, 16);
		pdglColor4f(0.5, 0.5, 0.5, 1);
		PD3D_DrawXZGrid2(4096, 4*16);
		break;
	case 4:
		pdglColor4f(0.1125, 0.1125, 0.1125, 1);
		if(i>2)PD3D_DrawYZGrid2(4096, 1.0/16);
		pdglColor4f(0.125, 0.125, 0.125, 1);
		if(i>1)PD3D_DrawYZGrid2(4096, 1);
		pdglColor4f(0.25, 0.25, 0.25, 1);
		if(i>0)PD3D_DrawYZGrid2(4096, 16);
		pdglColor4f(0.5, 0.5, 0.5, 1);
		PD3D_DrawYZGrid2(4096, 4*16);
		break;

	default:
//		pdglColor4f(0.125, 0.125, 0.125, 1);
//		PD3D_DrawGrid3D(4096, 16);
	
#if 0
//		PD3D_DrawGrid(0, 0, 16, 1);

		if(fabs(cam->org[2])<50)
		{
			pdglColor4f(0.5, 0.5, 0.5, 1);
			PD3D_DrawXYGrid2(256, 1);
		}

		pdglColor4f(0.75, 0.75, 0.75, 1);
		PD3D_DrawXYGrid2(256, 12);

//		pdglColor4f(0.75, 0.5, 0.5, 1);
//		PD3D_DrawXYGrid2(256, 10);
#endif
		break;
	}

	pdglDepthFunc(GL_LEQUAL);
//	pdglEnable(GL_DEPTH_TEST);

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

//	pdglEnable(GL_CULL_FACE);

	if(!cam->ortho)
		pdglEnable(GL_TEXTURE_2D);

	setup_light_ents(world->entity);

	if(cam->ortho)
	{
		pdglCullFace(GL_BACK);	pdglEnable(GL_CULL_FACE);
		pdglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		LBXGL_Shader_Color4f(1, 1, 1, 1);
		pdglColor4f(1, 1, 1, 1);

		LBXGL_BrushWorld_DrawBrushesFlat(world, world->brush);
	}else
	{
		LBXGL_BrushWorld_DrawBrushes(world, world->brush);
	}

#if 1
	if(lbxgl_brushmesh_mode!=LBXGL_EDMODE_BRUSH)
	{
		//temporary hack...

		pdglColor4f(1, 1, 1, 1);
		pdglDisable(GL_CULL_FACE);

#if 0
		pdglEnable(GL_LIGHTING);
		pdglEnable(GL_NORMALIZE);

		pdglLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

		if(lbxgl_brushmesh_mode!=LBXGL_EDMODE_FACE)
		{
			pt[0]=1; pt[1]=1; pt[2]=1; pt[3]=1;
			pdglMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, pt);

//			pdglMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, pt);
		}else
		{
			pt[0]=1; pt[1]=1; pt[2]=1; pt[3]=1;
			pdglMaterialfv(GL_FRONT, GL_DIFFUSE, pt);

//			pdglMaterialfv(GL_FRONT, GL_SPECULAR, pt);

			pt[0]=0.5; pt[1]=0.5; pt[2]=0.25; pt[3]=1;
			pdglMaterialfv(GL_BACK, GL_DIFFUSE, pt);
//			pdglMaterialfv(GL_BACK, GL_SPECULAR, pt);
		}

		UI_Camera_GetOrgFV(pt);
		pt[3]=1;
		pdglLightfv(GL_LIGHT0, GL_POSITION, pt);
		pdglLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 1.0/1000.0);
		pdglEnable(GL_LIGHT0);
#endif

		LBXGL_BrushMesh_DrawGroupEdit(lbxgl_brushmesh_mdl);

//		pdglDisable(GL_LIGHTING);
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

//	pdglDisable(GL_ALPHA_TEST);

	draw_ents(world->entity);

	draw_test_defsdr();
//	draw_test_terrain();

	LBXGL_Particle_Draw();
}

int pdgl_main_body()
{
	static int t;
	static float adt=1.0/30;
	char buf[256];
	float mins[3], maxs[3];
	float io[3], iv[4], fw[2], pt[4], f, g, v;
	float im[16], im1[16], *imv;

	LBXGL_Brush *brsh;
	LBXGL_SEntity *ecur;
	LBXGL_SEntPair *epcur;

	int i, j, k;
	unsigned short *kcur;
	char *s, *s0, *s1;

	main_handle_input();

//	main_time+=state->dt_f;
	LBXGL_Particle_Step(state->dt_f);

//	if(main_phys)
//		LBXGL_BrushPhys_StepWorld(world, state->dt_f);

	UI_Camera_Setup3D();
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

#if 0
	LBXGL_Sprite_SetCameraOrgRot(cam->org, cam->rot);

//	Draw_SetPerspective_3D(4.0/3.0, 90, 0,
//		cam->org, cam->ang, 0, 0, state->xs, state->ys);

	LBXGL_Sky_DrawSky();

	pdglDisable(GL_CULL_FACE);
	pdglDisable(GL_TEXTURE_2D);
//	PD3D_DrawGrid(0, 0, 16, 1);

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

//	pdglEnable(GL_CULL_FACE);

	pdglEnable(GL_TEXTURE_2D);

	setup_light_ents(world->entity);

	LBXGL_BrushWorld_DrawBrushes(world, world->brush);
	draw_ents(world->entity);

	draw_test_defsdr();

	LBXGL_Particle_Draw();
#endif

//	LBXGL_BrushPhys_UpdateWorld(world);

//	Tex_FillTextureScreenshot(tex_screen);

	UI_Camera_Setup2D();

//	Draw_SetSolid2_2D(4.0/3.0, 400,
//		VEC3(0, 0, 0), VEC3(0, 0, 0), 0, 0, state->xs, state->ys);

	if(0)
	{
		pdglColor4f(1, 1, 1, 1);
//		pdglEnable(GL_TEXTURE_2D);
		pdglBindTexture(GL_TEXTURE_2D, tex_screen);

		pdglBegin(PDGL_POLYGON);

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
	sprintf(buf, "(%.3f %.3f %.3f) (%.3f %.3f %.3f) %.2ffps",
		cam->org[0], cam->org[1], cam->org[2],
		cam->ang[0], cam->ang[1], cam->ang[2],
		1.0/adt);
	GfxFont_DrawString(buf, -400, 300-12,
		8, 8,  0, 255, 0, 255);

	sprintf(buf, "(%.3f %.3f %.3f) (%.3f %.3f %.3f)",
		cam->morg[0], cam->morg[1], cam->morg[2],
		cam->mdir[0], cam->mdir[1], cam->mdir[2]);
	GfxFont_DrawString(buf, -400, 300-12-8,
		8, 8,  0, 255, 0, 255);

	strcpy(buf, "");
	if(lbxgl_brushmesh_mode==LBXGL_EDMODE_GROUP)strcpy(buf, "Group");
	if(lbxgl_brushmesh_mode==LBXGL_EDMODE_OBJECT)strcpy(buf, "Object");
	if(lbxgl_brushmesh_mode==LBXGL_EDMODE_FACE)strcpy(buf, "Face");
	if(lbxgl_brushmesh_mode==LBXGL_EDMODE_VERTEX)strcpy(buf, "Vertex");
	if(lbxgl_brushmesh_mode==LBXGL_EDMODE_TEXTURE)strcpy(buf, "Texture");
	if(lbxgl_brushmesh_mode==LBXGL_EDMODE_BRUSH)strcpy(buf, "Brush");
	GfxFont_DrawString(buf, 400-(16*8), 300-12,
		8, 8,  0, 255, 0, 255);

	strcpy(buf, "");
	if(lbxgl_brushmesh_cmd==LBXGL_EDCMD_SEL)strcpy(buf, "Select");
	if(lbxgl_brushmesh_cmd==LBXGL_EDCMD_TRANS)strcpy(buf, "Translate");
	if(lbxgl_brushmesh_cmd==LBXGL_EDCMD_ROT)strcpy(buf, "Rotate");
	if(lbxgl_brushmesh_cmd==LBXGL_EDCMD_SCALE)strcpy(buf, "Scale");
	if(lbxgl_brushmesh_cmd==LBXGL_EDCMD_SCALE2)strcpy(buf, "Scale-HV");
	GfxFont_DrawString(buf, 400-(16*8), 300-12-8,
		8, 8,  0, 255, 0, 255);

	if(cam->mode==0)strcpy(buf, "Perspective 3D");
	if(cam->mode==1)strcpy(buf, "Ortho 3D");
	if(cam->mode==2)strcpy(buf, "Ortho XY");
	if(cam->mode==3)strcpy(buf, "Ortho XZ");
	if(cam->mode==4)strcpy(buf, "Ortho YZ");
	if(cam->mode==5)strcpy(buf, "Perspective 2");
	if(cam->mode==6)strcpy(buf, "4D");
	if(cam->mode==7)strcpy(buf, "4 Views");
	GfxFont_DrawString(buf, 400-(16*8), 300-12-16,
		8, 8,  0, 255, 0, 255);

#if 0
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
#endif

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
			sprintf(buf, "%s", s);
			GfxFont_DrawString(buf, 400-24*8, -300+24*8,
				8, 8,  0, 255, 0, 255);

//			sprintf(buf, "resource/textures/%s", s);
//			j=Tex_LoadFile(buf, NULL, NULL);

			sprintf(buf, "textures/%s", s);
			j=LBXGL_Texture_LoadImage(buf);

			pdglColor4f(1, 1, 1, 1);
			pdglEnableTexture2D();
			pdglBindTexture(GL_TEXTURE_2D, j);

			pdglBegin(PDGL_POLYGON);

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
	}else if(world->entity_psel)
	{
		epcur=world->entity_psel->keys;

		i=0;
		while(epcur)
		{
			sprintf(buf, "\"%s\" \"%s\"",
				epcur->name, epcur->value);
			GfxFont_DrawString(buf, 400-32*8, -300+24*8-i*8-8,
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
//					lbxgl_meshsel_prim->texnum=Tex_LoadFile(buf, NULL, NULL);

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
				load_file(s);
			if(*s && (ui_state==3))
				save_file(s);

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

	pdglDisableTexture2D();
	pdglColor4f(1, 0, 0, 1);

	pdglBegin(PDGL_LINES);
	pdglVertex2f(state->mx-10, -state->my);
	pdglVertex2f(state->mx+10, -state->my);
	pdglVertex2f(state->mx, -state->my-10);
	pdglVertex2f(state->mx, -state->my+10);
	pdglEnd();

	return(0);
}

int pdgl_main_shutdown()
{
	dyPrintf("app1 shutdown\n");
	return(0);
}
