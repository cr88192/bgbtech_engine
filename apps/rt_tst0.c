#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <windows.h>
#include <GL/gl.h>

#ifndef M_PI
#define M_PI		3.1415926535897932384626433832795
#endif

// #include <pdlib.h>

#include <pdgl.h>
#include <pdgl_stub.h>

#include <lbxgl2.h>
#include <libvecmath.h>

#include <bgbscr1.h>

typedef struct BXRE_Poly_S BXRE_Poly;

struct BXRE_Poly_S {
	BXRE_Poly *gnext;	//next polygon (group)
	int vfst;
};


LBXGL_Brush *brush_root;
LBXGL_SEntity *entity_root;

int draw_fargrid()
{
	float tv0[3], tv1[3], tv2[3], tv3[3];
	float r, g, b;
	int i, j, k;

	k=32;

	for(i=0; i<64; i++)
		for(j=0; j<64; j++)
//	for(i=0; i<16; i++)
//		for(j=0; j<16; j++)
	{
		V3F_ADDSCALE(cam_org, cam_fw, 1024, tv0);
		V3F_ADDSCALE(tv0, cam_rt, k*(j-32), tv0);
		V3F_ADDSCALE(tv0, cam_up, (0.75*k)*(i-32), tv0);

		V3F_COPY(tv0, tv1);
		V3F_ADDSCALE(tv1, cam_rt, k, tv1);

		V3F_COPY(tv0, tv2);
		V3F_ADDSCALE(tv2, cam_up, 0.75*k, tv2);

		V3F_COPY(tv1, tv3);
		V3F_ADDSCALE(tv3, cam_up, 0.75*k, tv3);

		glDisable(GL_TEXTURE_2D);
//		glColor4f(1,1,1,1);

//		r=sin(i*M_PI/32)*0.5+0.5;
//		g=0.5;
//		b=cos(j*M_PI/32)*0.5+0.5;

		r=(i&1)?1:0;
		b=(j&1)?1:0;
		g=1;

		glColor4f(r,g,b,1);

		glBegin(GL_POLYGON);
		glVertex3fv(tv0);
		glVertex3fv(tv1);
		glVertex3fv(tv3);
		glVertex3fv(tv2);
		glEnd();
	}
}

void add_brush_ent(LBXGL_SEntity *ent, LBXGL_Brush *brsh)
{
	LBXGL_Brush *cur;
	char *s;

	s=LBXGL_SEnt_GetStr(ent, "classname");
	if(!strncmp(s, "trigger_", 8))
		brsh->flags|=LBXGL_BRFL_HIDDEN;

	if(brush_root)
	{
		cur=brush_root;
		while(cur->next)cur=cur->next;
		cur->next=brsh;
	}else brush_root=brsh;

	if(ent)
	{
		brsh->se_owner=ent;

		if(ent->brush)
		{
			cur=ent->brush;
			while(cur->enext)cur=cur->enext;
			cur->enext=brsh;
		}else ent->brush=brsh;
	}
}

#if 0
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


	sprintf(buf, "textures/%s", brush->tex[id]);
	tn=Tex_LoadFile(buf, &xs, &ys);

	if(tn>=0)
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, tn);
	}else
	{
		glDisable(GL_TEXTURE_2D);
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

	glBegin(GL_POLYGON);
	for(i=0; i<l; i++)
	{
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
}
#endif

void draw_brush(LBXGL_Brush *brush)
{
	float tv0[3], tv1[3], tv2[3];
	float f, g;
	int i;

	LBXGL_Brush_DrawFaces(brush);
	return;

#if 1
	V3F_ZERO(brush->org);
	for(i=0; i<brush->n_face; i++)
	{
		draw_brush_face(brush, i);
		V3F_ADD(brush->org, brush->norg+i*3, brush->org);
	}
	V3F_SCALE(brush->org, 1.0/brush->n_face, brush->org);
#endif
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
		a=ksplit(buf);

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

		tex=kstrdup(a[15]);

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
		}else
		{
//			V3F_SCALE(tv0, 128, vs);
//			V3F_SCALE(tv1, 128, vt);
//			V3F_COPY(tv0, vs);
//			V3F_COPY(tv1, vt);

			Hull_AdjacentNormals21(tn, tv0, tv1);

			V3F_SCALE(tv0, atof(a[19]), vs);
			V3F_SCALE(tv1, atof(a[20]), vt);

			vs[3]=atof(a[16]);
			vt[3]=atof(a[17]);

			if(*tex=='*')*tex='#';
			sprintf(buf, "q1tex/%s", tex);
			tex=kstrdup(buf);
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

	tmp=ObjType_New("lbxgl_sentity_s", sizeof(LBXGL_SEntity));
	LBXGL_SEnt_AddEntity(&entity_root, tmp);

	while(!feof(fd))
	{
		memset(buf, 0, 256);
		fgets(buf, 255, fd);
		a=ksplit(buf);

		if(!a[0])continue;
		if(!strcmp(a[0], "}"))break;

		if(!strcmp(a[0], "{"))
		{
			btmp=load_brush(fd);
			add_brush_ent(tmp, btmp);

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

	while(!feof(fd))
	{
		memset(buf, 0, 256);
		fgets(buf, 255, fd);
		a=ksplit(buf);

		if(!a[0])continue;

		if(!strcmp(a[0], "{"))
		{
			ecur=load_entity(fd);
			continue;
		}
	}

	fclose(fd);
}

int pdgl_main_startup(int argc, char *argv[])
{
	float v0[3], v1[3], v2[3], f, g, h;
	LBXGL_SEntity *ecur;
	void *p;
	char *s;
	int i, j, k, l;

	GfxFont_Init();
	Con_Init();
	ConCmds_Init();

	PDGL_InitASM(argc, argv);

	PDGLUI_Init();
	UI_Camera_Init();

//	load_file("resource/maps/tst1.map");
//	load_file("resource/qmaps/start.map");
//	load_file("resource/qmaps/e3m5.map");
//	load_file("resource/qmaps/e3m6.map");
	load_file("resource/qmaps/e1m1.map");

	ecur=entity_root;
	while(ecur)
	{
		s=LBXGL_SEnt_GetStr(ecur, "classname");
		if(s && !strcmp(s, "worldspawn"))break;
		ecur=ecur->next;
	}

	if(ecur)
	{
		printf("Begin Build BSP\n");
		BSP_TransEntModel(ecur->brush);
		printf("End Build BSP\n");
	}

	i=PDGL_CheckLinkage();
	if(i<0)return(-1);

//	cam_org[0]=0;
//	cam_org[1]=-10;
//	cam_org[2]=16;

//	cam_ang[0]=90;
//	cam_ang[1]=0;
//	cam_ang[2]=0;


	return(1);
}

int main_handle_input()
{
	char buf[64];
	float mins[3], maxs[3];
	float io[3], iv[4], fw[2], pt[4], f, g, v;
	float im[16], im1[16], *imv;

	int i, j, k;
	unsigned short *kcur;

	if(Con_IsDown())
		return(0);

	if(PDGLUI_HasFocus())
		return(0);

	UI_Camera_Update();
}

int pdgl_main_body()
{
	static int t;
	char buf[64];
	float mins[3], maxs[3];
	float io[3], iv[4], fw[2], pt[4], f, g, v;

	int i, j, k;
	char *s;


	main_handle_input();


	UI_Camera_Setup3D();

	glDisable(GL_CULL_FACE);
	glDisable(GL_TEXTURE_2D);
	PD3D_DrawGrid(0, 0, 16, 1);

	LBXGL_SetCamOriginF(cam_org);

	LBXGL_SkyBox_DrawSkyBox();
	LBXGL_SkyDome_DrawSkyDome();

//	draw_fargrid();

	glEnable(GL_TEXTURE_2D);

	draw_brushes(brush_root);
//	draw_ents(entity_root);



	Draw_SetSolid2_2D(4.0/3.0, 400,
		VEC3(0, 0, 0), VEC3(0, 0, 0), 0, 0, pdgl_xs, pdgl_ys);

	Con_Render();

	GfxFont_SetFont("fixed", 0);

	sprintf(buf, "(%g %g %g) (%g %g %g)",
		cam_org[0], cam_org[1], cam_org[2],
		cam_ang[0], cam_ang[1], cam_ang[2]);
	GfxFont_DrawString(buf, -400, 300-12,
		8, 8,  0, 255, 0, 255);


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
	printf("shutdown\n");
	return(0);
}
