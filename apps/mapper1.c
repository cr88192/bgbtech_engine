#include <windows.h>

#include <GL/gl.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI		3.1415926535897932384626433832795
#endif


// #include <pdlib.h>

#include <pdgl.h>
#include <pdglui.h>
#include <pdgl_stub.h>

#include <lbxgl2.h>
#include <libvecmath.h>

extern float cam_org[3];
extern float cam_ang[3];
extern float cam_rot[9];

extern float cam_speed;
extern float cam_width;
extern int cam_constrain;	//constrain orientation
extern int cam_ortho;

extern float cam_lorg[3];
extern float cam_lang[3];
extern float cam_lrot[9];

extern float cam_rt[3];
extern float cam_fw[3];
extern float cam_up[3];

extern float cam_morg[3];
extern float cam_mdir[3];
extern float cam_mend[3];

extern float cam_lmorg[3];
extern float cam_lmdir[3];
extern float cam_lmend[3];

int cam_id;

float main_time;


char *default_tex="aardo/aardmbr64_1";
LBXGL_Brush *brush_root;
LBXGL_SEntity *entity_root;

LBXGL_SEntity *entity_psel;	//parent of new brushes
LBXGL_SEntity *entity_sel;
LBXGL_Brush *brush_sel;
int brush_sel_face;
char *entity_sel_key;

int ui_state;

void add_brush(LBXGL_Brush *brsh);

LBXGL_SEntity *new_brush_entity(char *type)
{
	float pt[3];
	LBXGL_SEntity *tmp;
	LBXGL_Brush *brsh;

	tmp=gctalloc("lbxgl_sentity_s", sizeof(LBXGL_SEntity));
	LBXGL_SEnt_SetStr(tmp, "classname", type);
	LBXGL_SEnt_AddEntity(&entity_root, tmp);

	entity_psel=tmp;

	V3F_ADDSCALE(cam_org, cam_fw, 24, pt);
	brsh=LBXGL_Brush_MakeCube(pt[0], pt[1], pt[2], 6, 6, 6,
		default_tex);
	add_brush(brsh);

	return(tmp);
}


void add_brush(LBXGL_Brush *brsh)
{
	LBXGL_Brush *cur;

	if(brush_root)
	{
		cur=brush_root;
		while(cur->next)cur=cur->next;
		cur->next=brsh;
	}else brush_root=brsh;

	if(entity_psel)
	{
		brsh->se_owner=entity_psel;

		if(entity_psel->brush)
		{
			cur=entity_psel->brush;
			while(cur->enext)cur=cur->enext;
			cur->enext=brsh;
		}else entity_psel->brush=brsh;
	}
}


void draw_brush_face(LBXGL_Brush *brush, int id)
{
	static float pts0[16*3];
	static float pts1[16*3];
	char buf[256];

	float tv0[3], tv1[3], tv2[3];
	float org[3];
	float sx, sy, f, g;
	int tn, xs, ys;
	int i, j, k, l, t;

	Hull_MakePlaneFace(brush->norm+id*4, pts0);
	l=4;

	for(i=0; i<brush->n_face; i++)
	{
		if(i==id)continue;
		l=Hull_ClipFace(brush->norm+i*4, pts0, pts1, l);
		for(j=0; j<(l*3); j++)pts0[j]=pts1[j];
	}

	if(!l)return;


	glDisable(GL_TEXTURE_2D);
	glColor4f(0, 0, 1, 1);
	glBegin(GL_LINE_LOOP);
	for(i=0; i<l; i++)
	{
		glVertex3fv(pts0+i*3);
	}
	glEnd();


//	if(V3F_NDOT(cam_org, brush->norm+id*4)<0)
//		return;


//	sprintf(buf, "resource/textures/%s", brush->tex[id]);
	sprintf(buf, "textures/%s", brush->tex[id]);
	tn=Tex_LoadFile(buf, &xs, &ys);

	if(tn>=0)
	{
//		printf("%d %d %d\n", tn, xs, ys);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, tn);
	}else
	{
		xs=256;
		ys=256;
	}


	glColor4f(1, 1, 1, 1);

	V3F_COPY(cam_morg, tv0);
	V3F_COPY(cam_mend, tv1);

	for(i=0; i<brush->n_face; i++)
	{
		f=V3F_NDOT(tv0, brush->norm+i*4);
		g=V3F_NDOT(tv1, brush->norm+i*4);
		if((f>0) && (g>0))break;
		if((f<=0) && (g<=0))continue;

		Hull_LinePlaneIntersect(tv0, tv1, brush->norm+i*4, tv2);

		if(f>0) { V3F_COPY(tv2, tv0); }
			else { V3F_COPY(tv2, tv1); }
	}

	if(brush->se_owner==entity_psel)
		glColor4f(1, 0.75, 0.25, 1);

	if(i==brush->n_face)
		glColor4f(1, 0.5, 1, 1);

	if(brush==brush_sel)
		glColor4f(0.5, 1, 0.5, 1);


//	Hull_AdjacentNormals(brush->norm+id*4, tv0, tv1);

	glBegin(GL_POLYGON);
	for(i=0; i<l; i++)
	{
//		sx=V3F_DOT(pts0+i*3, tv0)/xs;
//		sy=V3F_DOT(pts0+i*3, tv1)/ys;

		sx=V3F_NDOT(pts0+i*3, brush->sdir+id*4)/xs;
		sy=V3F_NDOT(pts0+i*3, brush->tdir+id*4)/ys;

		glTexCoord2f(sx, sy);

		glVertex3fv(pts0+i*3);
	}
	glEnd();


	V3F_ZERO(org);
	for(i=0; i<l; i++) { V3F_ADD(org, pts0+i*3, org); }
	V3F_SCALE(org, 1.0/l, org);

	V3F_COPY(org, brush->norg+id*3);


	if(brush==brush_sel)
	{
		glColor4f(0, 0, 1, 1);

		f=(V3F_DOT(org, cam_fw)-
			V3F_DOT(cam_org, cam_fw))/64;
		g=Vec3F_DistLineSeg(org, cam_morg, cam_mend);
		if(g<f)glColor4f(1, 0, 1, 1);

		if(id==brush_sel_face)
		{
			glColor4f(0, 1, 0, 1);

			if(pdgl_mb&1)
			{
				f=V3F_DOT(org, cam_fw)-
					V3F_DOT(cam_org, cam_fw);
				V3F_SCALEADDSCALE(
					cam_rt, f*pdgl_dmx/400.0,
					cam_up, f*(-pdgl_dmy)/400.0, tv0);

				g=V3F_DOT(tv0, brush->norm+id*4);
				brush->norm[id*4+3]+=g;
			}
		}

//		if(V3F_NDOT(cam_org, brush->norm+id*4)>=0)
		if(1)
		{
			f=(V3F_DOT(org, cam_fw)-
				V3F_DOT(cam_org, cam_fw))/64;

			glDisable(GL_TEXTURE_2D);
			glDisable(GL_DEPTH_TEST);

			glBegin(GL_POLYGON);

			V3F_ADDSCALEADDSCALE(org, cam_rt, -f,
				cam_up, -f, tv0);
			glVertex3fv(tv0);

			V3F_ADDSCALEADDSCALE(org, cam_rt, f,
				cam_up, -f, tv0);
			glVertex3fv(tv0);

			V3F_ADDSCALEADDSCALE(org, cam_rt, f,
				cam_up, f, tv0);
			glVertex3fv(tv0);

			V3F_ADDSCALEADDSCALE(org, cam_rt, -f,
				cam_up, f, tv0);
			glVertex3fv(tv0);

			glEnd();

			glEnable(GL_DEPTH_TEST);
		}
	}
}

void draw_brush(LBXGL_Brush *brush)
{
	float tv0[3], tv1[3], tv2[3];
	float f, g;
	int i;

//	LBXGL_Brush_DrawFaces(brush);
//	return;

#if 1
	V3F_ZERO(brush->org);
	for(i=0; i<brush->n_face; i++)
	{
		draw_brush_face(brush, i);
		V3F_ADD(brush->org, brush->norg+i*3, brush->org);
	}
	V3F_SCALE(brush->org, 1.0/brush->n_face, brush->org);
#endif

	if((pdgl_mb&1) && (brush_sel==brush) && (brush_sel_face==-1))
	{

		f=V3F_DOT(brush->org, cam_fw)-
			V3F_DOT(cam_org, cam_fw);
		V3F_SCALEADDSCALE(
			cam_rt, f*pdgl_dmx/400.0,
			cam_up, f*(-pdgl_dmy)/400.0, tv0);

		for(i=0; i<brush->n_face; i++)
		{
			g=V3F_DOT(tv0, brush->norm+i*4);
			brush->norm[i*4+3]+=g;
		}
	}
}

void draw_brushes(LBXGL_Brush *fst)
{
	LBXGL_Brush *cur;
	int i;

	cur=fst;
	while(cur)
	{
		draw_brush(cur);
		cur=cur->next;
	}
}

void sel_brush_face()
{
	static float pts0[16*3];
	static float pts1[16*3];

	float org[3];
	float sx, sy, f, g;
	int tn, xs, ys;
	int i, j, k, l, t;

	brush_sel_face=-1;
	if(!brush_sel)return;

	for(i=0; i<brush_sel->n_face; i++)
	{
//		if(V3F_NDOT(cam_org, brush_sel->norm+i*4)<0)
//			continue;

		Hull_MakePlaneFace(brush_sel->norm+i*4, pts0);
		l=4;

		for(j=0; j<brush_sel->n_face; j++)
		{
			if(i==j)continue;
			l=Hull_ClipFace(brush_sel->norm+j*4, pts0, pts1, l);
			for(k=0; k<(l*3); k++)pts0[k]=pts1[k];
		}

		if(!l)continue;

		V3F_ZERO(org);
		for(j=0; j<l; j++) { V3F_ADD(org, pts0+j*3, org); }
		V3F_SCALE(org, 1.0/l, org);

		f=(V3F_DOT(org, cam_fw)-
			V3F_DOT(cam_org, cam_fw))/64;
		g=Vec3F_DistLineSeg(org, cam_morg, cam_mend);
		if(g<f)
		{
			brush_sel_face=i;
		}
	}
}

void sel_brushes(LBXGL_Brush *fst)
{
	float tv0[3], tv1[3], tv2[3];
	float f, g;
	LBXGL_Brush *cur;
	int i;

	cur=fst;
	while(cur)
	{
		if(brush_sel==cur)
		{
			brush_sel=NULL;
			brush_sel_face=-1;
			cur=cur->next;
			continue;
		}

		V3F_COPY(cam_morg, tv0);
		V3F_COPY(cam_mend, tv1);

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
			if(!brush_sel)
			{
				brush_sel=cur;
				brush_sel_face=-1;
			}
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
		f=log2(f);
		if(f<1)f=1;
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

	entity_sel=NULL;

	cur=entity_root;
	while(cur)
	{
		f=ent_radius(cur);

		LBXGL_SEnt_GetVec3(cur, "origin", org);
		g=Vec3F_DistLineSeg(org, cam_morg, cam_mend);
		if(g<f)
		{
			entity_sel=cur;
			brush_sel=NULL;
		}

		cur=cur->next;
	}
}

void draw_ent(LBXGL_SEntity *ent)
{
	float org[3], tv[3];
	float f, g;
	char *ty;
	int tn, xs, ys;

	ty=LBXGL_SEnt_GetStr(ent, "classname");
	LBXGL_SEnt_GetVec3(ent, "origin", org);

	glColor4f(1, 1, 1, 1);

	f=ent_radius(ent);
	g=Vec3F_DistLineSeg(org, cam_morg, cam_mend);
	if(g<f)glColor4f(1, 0.5, 1, 1);

	if(ent==entity_sel)glColor4f(0.5, 1, 0.5, 1);

	if((ent==entity_sel) && (pdgl_mb&1))
	{
		f=V3F_DOT(org, cam_fw)-
			V3F_DOT(cam_org, cam_fw);
		V3F_SCALEADDSCALE(
			cam_rt, f*pdgl_dmx/400.0,
			cam_up, f*(-pdgl_dmy)/400.0, tv);
		V3F_ADD(org, tv, org);
		LBXGL_SEnt_SetVec3(ent, "origin", org);
	}

	tn=-1;

	tn=Tex_LoadFile("resource/images/mystery.png", &xs, &ys);
	f=4;

	if(!strncmp(ty, "light", 5))
	{
		tn=Tex_LoadFile("resource/images/light0.png", &xs, &ys);

//		f=(V3F_DOT(org, cam_fw)-
//			V3F_DOT(cam_org, cam_fw))/32;

		f=LBXGL_SEnt_GetNum(ent, "light");
//		f=log2(f);
		f=sqrt(f)/4;
		if(f<1)f=1;
	}

	if(!strcmp(ty, "info_player_start"))
	{
		tn=Tex_LoadFile("resource/images/dude.png", &xs, &ys);

//		f=(V3F_DOT(org, cam_fw)-
//			V3F_DOT(cam_org, cam_fw))/32;

		f=36;
	}

	if(tn>=0)
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, tn);

		glBegin(GL_POLYGON);

		glTexCoord2f(0, 0);
		V3F_ADDSCALEADDSCALE(org, cam_rt, -f,
			cam_up, -f, tv);
		glVertex3fv(tv);

		glTexCoord2f(1, 0);
		V3F_ADDSCALEADDSCALE(org, cam_rt, f,
			cam_up, -f, tv);
		glVertex3fv(tv);

		glTexCoord2f(1, 1);
		V3F_ADDSCALEADDSCALE(org, cam_rt, f,
			cam_up, f, tv);
		glVertex3fv(tv);

		glTexCoord2f(0, 1);
		V3F_ADDSCALEADDSCALE(org, cam_rt, -f,
			cam_up, f, tv);
		glVertex3fv(tv);

		glEnd();
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

void save_brush(FILE *fd, LBXGL_Brush *brush)
{
	float tv0[3], tv1[3], tv2[3];
	float *sd, *td;
	int i;

	fprintf(fd, " {\n");

	for(i=0; i<brush->n_face; i++)
	{
		V3F_SCALE(brush->norm+i*4, brush->norm[i*4+3], tv0);
		Hull_AdjacentNormals(brush->norm+i*4, tv2, tv1);
		V3F_ADDSCALE(tv0, tv1, 128, tv1);
		V3F_ADDSCALE(tv0, tv2, 128, tv2);
		sd=brush->sdir+i*4;
		td=brush->tdir+i*4;

		fprintf(fd, "  ( %.2f %.2f %.2f ) ( %.2f %.2f %.2f ) "
			"( %.2f %.2f %.2f ) %s "
			"[ %.4f %.4f %.4f %.4f ] [ %.4f %.4f %.4f %.4f ] "
			"0  1.00000 1.00000 0 0 0\n",
			tv0[0], tv0[1], tv0[2],
			tv1[0], tv1[1], tv1[2],
			tv2[0], tv2[1], tv2[2], brush->tex[i],
			sd[0], sd[1], sd[2], sd[3],
			td[0], td[1], td[2], td[3]);
	}

	fprintf(fd, " }\n");
}

void save_entity(FILE *fd, LBXGL_SEntity *ent)
{
	LBXGL_SEntPair *epcur;
	LBXGL_Brush *bcur;
	char *s;

	fprintf(fd, "{\n");

	epcur=ent->keys;
	while(epcur)
	{
		fprintf(fd, "\"%s\" \"%s\"\n", epcur->name, epcur->value);
		epcur=epcur->next;
	}

	bcur=ent->brush;
	while(bcur)
	{
		save_brush(fd, bcur);
		bcur=bcur->enext;
	}

	//any brushes w/o owners go to worldspawn
	s=LBXGL_SEnt_GetStr(ent, "classname");
	if(!strcmp(s, "worldspawn"))
	{
		bcur=brush_root;
		while(bcur)
		{
			if(!bcur->se_owner)
			{
				save_brush(fd, bcur);
			}
			bcur=bcur->next;
		}
	}

	fprintf(fd, "}\n");
}

void save_file(char *name)
{
	FILE *fd;
	LBXGL_SEntity *ecur;

	fd=fopen(name, "wt");

	ecur=entity_root;
	while(ecur)
	{
		save_entity(fd, ecur);
		ecur=ecur->next;
	}

	fclose(fd);
}

LBXGL_Brush *load_brush(FILE *fd)
{
	char buf[256];
	float v0[3], v1[3], v2[3];
	float tv0[3], tv1[3], tv2[3];
	float vs[4], vt[4], tn[4];

	LBXGL_Brush *tmp;
	char **a, *tex;
	int n;

//	tmp=malloc(sizeof(LBXGL_Brush));
//	memset(tmp, 0, sizeof(LBXGL_Brush));
	tmp=LBXGL_Brush_New(16);
	n=0;

	while(!feof(fd))
	{
		memset(buf, 0, 256);
		fgets(buf, 255, fd);
		a=gcrsplit(buf);

		if(!a[0])continue;
		if(!strcmp(a[0], "}"))break;

		if(strcmp(a[0], "("))continue;
		v0[0]=atof(a[1]);
		v0[1]=atof(a[2]);
		v0[2]=atof(a[3]);
		v1[0]=atof(a[6]);
		v1[1]=atof(a[7]);
		v1[2]=atof(a[8]);
		v2[0]=atof(a[11]);
		v2[1]=atof(a[12]);
		v2[2]=atof(a[13]);

		tex=strdup(a[15]);

		V3F_SUB(v1, v0, tv0);
		V3F_SUB(v2, v0, tv1);
		V3F_NORMALIZE(tv0, tv0);
		V3F_NORMALIZE(tv1, tv1);

		V3F_CROSS(tv1, tv0, tn);
		V3F_NORMALIZE(tn, tn);
		tn[3]=V3F_DOT(v0, tn);

		if(!strcmp(a[16], "["))
		{
			vs[0]=atof(a[17]);
			vs[1]=atof(a[18]);
			vs[2]=atof(a[19]);
			vs[3]=atof(a[20]);

			vt[0]=atof(a[23]);
			vt[1]=atof(a[24]);
			vt[2]=atof(a[25]);
			vt[3]=atof(a[26]);
		}

		V4F_COPY(tn, tmp->norm+n*4);
		V4F_COPY(vs, tmp->sdir+n*4);
		V4F_COPY(vt, tmp->tdir+n*4);
		tmp->tex[n]=tex;
		n++;
	}

	tmp->n_face=n;
	return(tmp);
}

LBXGL_SEntity *load_entity(FILE *fd)
{
	char buf[256];
	LBXGL_SEntity *tmp;
	LBXGL_Brush *btmp;
	char **a;

	tmp=gctalloc("lbxgl_sentity_s", sizeof(LBXGL_SEntity));
	LBXGL_SEnt_AddEntity(&entity_root, tmp);

	while(!feof(fd))
	{
		memset(buf, 0, 256);
		fgets(buf, 255, fd);
		a=gcrsplit(buf);

		if(!a[0])continue;
		if(!strcmp(a[0], "}"))break;

		if(!strcmp(a[0], "{"))
		{
			entity_psel=tmp;
			btmp=load_brush(fd);
			add_brush(btmp);

			continue;
		}

		LBXGL_SEnt_SetStr(tmp, a[0], a[1]);
	}

	return(tmp);
}

void load_file(char *name)
{
	char buf[256];
	FILE *fd;
	LBXGL_SEntity *ecur;
	char **a;


	fd=fopen(name, "rt");
	if(!fd)return;

	entity_root=NULL;
	brush_root=NULL;
	entity_sel=NULL;
	entity_psel=NULL;
	brush_sel=NULL;


	while(!feof(fd))
	{
		memset(buf, 0, 256);
		fgets(buf, 255, fd);
		a=gcrsplit(buf);

		if(!a[0])continue;

		if(!strcmp(a[0], "{"))
		{
			ecur=load_entity(fd);
			continue;
		}
	}

	fclose(fd);

	entity_sel=NULL;
	entity_psel=NULL;
	brush_sel=NULL;
}

int ConCmds_Load(char **a)
{
	load_file(a[1]);
}

int ConCmds_Save(char **a)
{
	save_file(a[1]);
}

int ConCmds_Sky(char **a)
{
	LBXGL_Sky_LoadSky(a[1]);
}

int ConCmds_Ent(char **a)
{
	char *s;

	if(!a[1])
	{
		Con_Printf("ent <property> [value]\n");
		return(0);
	}

	if(!entity_sel)
	{
		Con_Printf("no entity selected\n");
		return(0);
	}

	if(a[2])
	{
		LBXGL_SEnt_SetStr(entity_sel, a[1], a[2]);
		return(0);
	}else
	{
		s=LBXGL_SEnt_GetStr(entity_sel, a[1]);
		if(s)Con_Printf("\"%s\" \"%s\"\n", a[1], s);
			else printf("no property \"%s\"\n", a[1]);
		return(0);
	}
}

int main_new()
{
	float v[3];
	LBXGL_Brush *brsh;

	entity_root=NULL;
	brush_root=NULL;

	entity_psel=LBXGL_SEnt_NewEntity(&entity_root, "worldspawn");

//	brush_root=make_cube_default();

	brsh=LBXGL_Brush_MakeCube(0, 0, -64, 128, 128, 6, default_tex);
	add_brush(brsh);

	brsh=LBXGL_Brush_MakeCube(0, 0, 64, 128, 128, 6, default_tex);
	add_brush(brsh);

	brsh=LBXGL_Brush_MakeCube(-128, 0, 0, 6, 128, 64, default_tex);
	add_brush(brsh);

	brsh=LBXGL_Brush_MakeCube(128, 0, 0, 6, 128, 64, default_tex);
	add_brush(brsh);

	brsh=LBXGL_Brush_MakeCube(0, -128, 0, 128, 6, 64, default_tex);
	add_brush(brsh);

	brsh=LBXGL_Brush_MakeCube(0, 128, 0, 128, 6, 64, default_tex);
	add_brush(brsh);



	V3F_ZERO(v);
	LBXGL_SEnt_NewBasicLight(&entity_root, v, 1000);


	V3F_SET(v, 0, -100, -64+36);
	LBXGL_SEnt_NewBasicEntity(&entity_root, "info_player_start", v);
}

int pdgl_main_startup(int argc, char *argv[])
{
	char *opts[4];
	char *s;
	int i, j, k, l;

	GfxFont_Init();
	Con_Init();
	ConCmds_Init();

	PDGLUI_Init();
	UI_Camera_Init();

	ConCmds_Register("ent", "Get/Set entity property", ConCmds_Ent);
	ConCmds_Register("load", "Load map file", ConCmds_Load);
	ConCmds_Register("save", "Save map file", ConCmds_Save);

	ConCmds_Register("sky", "Set sky", ConCmds_Sky);

	UI_TexFinder_SetTitle("Textures");


	cam_org[0]=0;
	cam_org[1]=-10;
	cam_org[2]=16;

//	cam_ang[0]=90;
//	cam_ang[1]=0;
//	cam_ang[2]=0;



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
	PDGLUI_EndSubMenu();

	PDGLUI_BeginSubMenu("Command");
	PDGLUI_MenuItem("cmd_delete", 	"Delete Selection        (DEL)");
	PDGLUI_MenuItem("cmd_settex",	"Set Texture             (CTRL--T)");
	PDGLUI_EndSubMenu();

	PDGLUI_BeginSubMenu("Help");
	PDGLUI_MenuItem("help_generic", "General");
	PDGLUI_MenuItem("help_about", "About");
	PDGLUI_EndSubMenu();

	PDGLUI_EndMenu();



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
	int i, j, k;
	unsigned short *kcur;
	LBXGL_SEntPair *epcur;
	char *s;

	if(Con_IsDown() || ui_state || PDGLUI_HasFocus())
		return(0);

	if(pdgl_mb&4)
	{
		PDGLUI_ShowMenu("menu", pdgl_mx, -pdgl_my);
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
			UI_LoadSave_SetTitle("Open", "map");
			ui_state=2;
		}
		if(!strcmp(s, "save_as"))
		{
			UI_LoadSave_SetTitle("Save As", "map");
			ui_state=3;
		}
		if(!strcmp(s, "cmd_settex"))
		{
			UI_TexFinder_SetTitle("Textures");
			ui_state=1;
		}

//		if(!strcmp(s, "edit_undo"))Mesh_Undo();
//		if(!strcmp(s, "edit_redo"))Mesh_Redo();

//		if(!strcmp(s, "edit_cut"))Mesh_CmdSel(CMD_CUT);
//		if(!strcmp(s, "edit_copy"))Mesh_CmdSel(CMD_COPY);
//		if(!strcmp(s, "edit_paste"))Mesh_Paste();
//		if(!strcmp(s, "edit_dup"))Mesh_CmdSel(CMD_DUPLICATE);
	}


	kcur=pdgl_keybuf;
	while(*kcur)
	{
		if(*kcur==K_ENTER)
		{
			sel_brush_face();
			if(brush_sel_face<0)sel_ents();
			if((brush_sel_face<0) && !entity_sel)
				sel_brushes(brush_root);
		}

		if(*kcur=='T')
		{
			UI_TexFinder_SetTitle("Textures");
			ui_state=1;
		}
		if(*kcur=='O')
		{
			entity_psel=entity_sel;
		}
		if(*kcur=='P')
		{
			entity_sel=entity_psel;
			brush_sel=NULL;
		}

		if(*kcur=='l')
		{
			LBXGL_SEnt_NewBasicLight(&entity_root,
				cam_org, 100);
		}

		if(*kcur=='d')
		{
			new_brush_entity("func_door");
		}

		if(*kcur==K_INS)
		{
			V3F_ADDSCALE(cam_org, cam_fw, 24, pt);

			brsh=LBXGL_Brush_MakeCube(pt[0], pt[1], pt[2], 6, 6, 6,
				default_tex);
			add_brush(brsh);
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
			j=i; ecur=entity_root;
			while(ecur && j--)ecur=ecur->next;
			entity_sel=ecur;

			PDGLUI_SetWidgetPropertyS(
				"selbox", "key", "text", "");
			PDGLUI_SetWidgetPropertyS(
				"selbox", "value", "text", "");
		}

		if(!strcmp(frm, "selbox") &&
			!strcmp(s0, "plst") && !strcmp(s1, "select"))
		{
			if(!s2)continue;

			entity_sel_key=dystring(s2);
			epcur=entity_sel->keys;
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
			if(!entity_sel)continue;

			s=PDGLUI_GetWidgetPropertyS(
				"selbox", "key", "text");
			LBXGL_SEnt_SetStr(entity_sel, s, s2);
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
	main_handle_form("help");
	main_handle_form("selbox");
}


int pdgl_main_body()
{
	static int t;
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

	main_time+=pdgl_dt_f;


	UI_Camera_Setup3D();

//	Draw_SetPerspective_3D(4.0/3.0, 90, 0,
//		cam_org, cam_ang, 0, 0, pdgl_xs, pdgl_ys);

	LBXGL_Sky_DrawSky();

	glDisable(GL_CULL_FACE);
	glDisable(GL_TEXTURE_2D);
//	PD3D_DrawGrid(0, 0, 16, 1);

	glBegin(GL_LINES);

	glColor4f(1, 0, 0, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(1000, 0, 0);

	glColor4f(0, 1, 0, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 1000, 0);

	glColor4f(0, 0, 1, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 1000);

	glEnd();

//	glEnable(GL_CULL_FACE);

	glEnable(GL_TEXTURE_2D);

	draw_brushes(brush_root);
	draw_ents(entity_root);

	UI_Camera_Setup2D();

//	Draw_SetSolid2_2D(4.0/3.0, 400,
//		VEC3(0, 0, 0), VEC3(0, 0, 0), 0, 0, pdgl_xs, pdgl_ys);

	Con_Render();

	GfxFont_SetFont("fixed", 0);

	sprintf(buf, "(%g %g %g) (%g %g %g)",
		cam_org[0], cam_org[1], cam_org[2],
		cam_ang[0], cam_ang[1], cam_ang[2]);
	GfxFont_DrawString(buf, -400, 300-12,
		8, 8,  0, 255, 0, 255);

	i=0;
	ecur=entity_root;
	while(ecur && (i<25))
	{
		j=7;
		if(ecur==entity_psel)j=3;
		if(ecur==entity_sel)j=2;

		s=LBXGL_SEnt_GetStr(ecur, "classname");
		GfxFont_DrawString(s, 400-24*8, 300-(i+2)*8,
			8, 8,  (j&1)?255:0, (j&2)?255:0, (j&4)?255:0, 255);

		ecur=ecur->next;
		i++;
	}

	if(brush_sel)
	{
		if(brush_sel->se_owner)
		{
			entity_psel=brush_sel->se_owner;

			s=LBXGL_SEnt_GetStr(brush_sel->se_owner, "classname");
			sprintf(buf, "Owner: %s", s);
			GfxFont_DrawString(buf, 400-24*8, -300+25*8,
				8, 8,  0, 255, 0, 255);
		}


		i=brush_sel_face;
		if(i<0)i=0;

		sprintf(buf, "%s", brush_sel->tex[i]);
		GfxFont_DrawString(buf, 400-24*8, -300+24*8,
			8, 8,  0, 255, 0, 255);


		sprintf(buf, "resource/textures/%s", brush_sel->tex[i]);
		j=Tex_LoadFile(buf, NULL, NULL);

		glColor4f(1, 1, 1, 1);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, j);

		glBegin(GL_POLYGON);

		glTexCoord2f(0, 0);
		glVertex2f(400-24*8, -300);

		glTexCoord2f(1, 0);
		glVertex2f(400, -300);

		glTexCoord2f(1, 1);
		glVertex2f(400, -300+24*8);

		glTexCoord2f(0, 1);
		glVertex2f(400-24*8, -300+24*8);

		glEnd();
	}

	if(entity_sel)
	{
		epcur=entity_sel->keys;

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

			if(brush_sel)
			{
				i=brush_sel_face;
				if(i<0)
				{
					for(i=0; i<brush_sel->n_face; i++)
						brush_sel->tex[i]=s;
				}else
				{
					brush_sel->tex[i]=s;
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


	PDGLUI_ClearItems("selbox", "lst");
	ecur=entity_root; i=0; j=-1;
	while(ecur)
	{
		if(ecur==entity_sel)j=i;

		s=LBXGL_SEnt_GetStr(ecur, "classname");
		PDGLUI_AddItem("selbox", "lst", s, s);

		ecur=ecur->next;
		i++;
	}

	PDGLUI_SetWidgetPropertyI("selbox", "lst", "value", j);


	PDGLUI_ClearItems("selbox", "plst");
	if(entity_sel)
	{
		epcur=entity_sel->keys;

		i=0;
		while(epcur)
		{
			sprintf(buf, "\"%s\" \"%s\"",
				epcur->name, epcur->value);
			PDGLUI_AddItem("selbox", "plst", epcur->name, buf);

			i++;
			epcur=epcur->next;
		}

		if(entity_sel_key)
			PDGLUI_SetWidgetPropertyS("selbox", "plst",
				"value", entity_sel_key);
	}


	PDGLUI_Frame();
	main_handle_events();

	glDisable(GL_TEXTURE_2D);
	glColor4f(1, 0, 0, 1);

	glBegin(GL_LINES);
	glVertex2f(pdgl_mx-10, -pdgl_my);
	glVertex2f(pdgl_mx+10, -pdgl_my);
	glVertex2f(pdgl_mx, -pdgl_my-10);
	glVertex2f(pdgl_mx, -pdgl_my+10);
	glEnd();

	return(0);
}

int pdgl_main_shutdown()
{
	printf("app1 shutdown\n");
	return(0);
}
