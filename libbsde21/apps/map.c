#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <math.h>

#include <libbsde.h>

// typedef unsigned char byte;

#include "brush.h"

#include <pdgl_keys.h>
#include <pdgl_math.h>

#ifndef M_PI
#define M_PI		3.1415926535897932384626433832795
#endif


char *map_name=NULL;
int map_stat_brushes;
int map_stat_ents;
int map_stat_lights;

LBXGL_BrushWorld *world;


byte *kralloc(int sz);
char **ksplit(char *s);

void Hull_AdjacentNormals22(float *norm, float *udir, float *vdir)
{
	float f, ax, ay, az;

	V3F_ZERO(udir);
	V3F_ZERO(vdir);

	//UxV=N

	ax=fabs(norm[0]); ay=fabs(norm[1]); az=fabs(norm[2]);

	if(ax>=ay && ax>=az) { udir[1]=1; vdir[2]=1; }
		else if(ay>=ax && ay>=az) { udir[0]=1; vdir[2]=1; }
		else { udir[0]=1; vdir[1]=1; }

	V3F_NORMALIZE(udir, udir);
	V3F_NORMALIZE(vdir, vdir);
}

LBXGL_Brush *load_brush(FILE *fd)
{
	char buf[256];
	float v0[3], v1[3], v2[3];
	float tv0[4], tv1[4], tv2[4];
	float vs[4], vt[4], tn[4];
	float f, g, h;

	LBXGL_Brush *tmp;
	char **a, *tex;
	int i, n;

	map_stat_brushes++;

	tmp=malloc(sizeof(LBXGL_Brush));
	memset(tmp, 0, sizeof(LBXGL_Brush));

	tmp->norm=(float *)malloc(16*4*sizeof(float));
	tmp->sdir=(float *)malloc(16*4*sizeof(float));
	tmp->tdir=(float *)malloc(16*4*sizeof(float));
	tmp->tex=(char **)malloc(16*sizeof(char *));

//	tmp=LBXGL_Brush_New(16);
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
		}else
		{
			sprintf(buf, "q1tex/%s", tex);

			for(i=0; buf[i]; i++)
			{
				if(buf[i]=='*')buf[i]='#';
				if((buf[i]>='A') && (buf[i]<='Z'))
					buf[i]=buf[i]-'A'+'a';
			}

			tex=strdup(buf);

			Hull_AdjacentNormals22(tn, vs, vt);
			vs[3]=-atof(a[16]);
			vt[3]=atof(a[17]);

			f=-atof(a[18])*(M_PI/180);
			g=cos(f); h=sin(f);
//			V3F_SCALEADDSCALE(vs, g, vt, h, tv0);
//			V3F_SCALEADDSCALE(vt, g, vs, -h, tv1);

			V3F_SCALEADDSCALE(vs, g, vt, -h, tv0);
			V3F_SCALEADDSCALE(vt, g, vs, h, tv1);

//			V3F_COPY(tv0, vs);
//			V3F_COPY(tv1, vt);

			f=atof(a[19]); g=atof(a[20]);
			if(f==0)f=1; if(g==0)g=1;
			V3F_SCALE(tv0, 1.0/f, vs);
			V3F_SCALE(tv1, 1.0/g, vt);

//			vs[3]*=1.0/f; vt[3]*=1.0/g;
		}

//		vs[3]*=1.0/40;
//		vt[3]*=1.0/40;
//		tn[3]*=1.0/40;

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

	map_stat_ents++;

//	tmp=gctalloc("lbxgl_sentity_s", sizeof(LBXGL_SEntity));
//	LBXGL_SEnt_AddEntity(&world->entity, tmp);

	while(!feof(fd))
	{
		memset(buf, 0, 256);
		fgets(buf, 255, fd);
		a=ksplit(buf);

		if(!a[0])continue;
		if(!strcmp(a[0], "}"))break;

		if(!strcmp(a[0], "{"))
		{
//			world->entity_psel=tmp;
			btmp=load_brush(fd);

//			LBXGL_BrushWorld_AddBrush(world, btmp);

			btmp->next=world->brush;
			if(btmp->next)btmp->next->prev=btmp;
			world->brush=btmp;

			continue;
		}

		if(!strcmp(a[0], "classname") && !strncmp(a[1], "light", 5))
			map_stat_lights++;

//		LBXGL_SEnt_SetStr(tmp, a[0], a[1]);
	}

	return(tmp);
}

void send_brushes(LBXGL_Brush *brsh);

void load_map(char *name)
{
	char buf[256];
	FILE *fd;
	LBXGL_SEntity *ecur;
	char **a;

	if(!world)
	{
		world=malloc(sizeof(LBXGL_BrushWorld));
		memset(world, 0, sizeof(LBXGL_BrushWorld));
	}

//	LBXGL_BrushWorld_DrawFlush(world);

	fd=fopen(name, "rt");
	if(!fd)return;

//	world->entity=NULL;
	world->brush=NULL;
//	world->entity_sel=NULL;
//	world->entity_psel=NULL;
	world->brush_sel=NULL;

//	world->mesh=NULL;
//	world->object_list=NULL;


//	map_name=dystring(name);
	map_stat_brushes=0;
	map_stat_ents=0;
	map_stat_lights=0;

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

//	world->entity_sel=NULL;
//	world->entity_psel=NULL;
	world->brush_sel=NULL;

	printf("Map Loaded: %d brushes, %d ents, %d lights\n",
		map_stat_brushes, map_stat_ents, map_stat_lights);

//	world->object_list=spawn_ents(world->entity);

	send_brushes(world->brush);
}

void main_addsolid(int id);

void send_brush(LBXGL_Brush *brsh)
{
	float org[3], pt[3];
	float nv[16*4];
	float f;

	int i;

	V3F_ZERO(org);
	for(i=0; i<brsh->n_face; i++)
	{
		V3F_SCALE(brsh->norm+i*4, brsh->norm[i*4+3], pt);
		V3F_ADD(org, pt, org);
	}
	V3F_SCALE(org, 1.0/brsh->n_face, org);

	for(i=0; i<brsh->n_face; i++)
	{
		nv[i*4+0]=brsh->norm[i*4+0];
		nv[i*4+1]=brsh->norm[i*4+1];
		nv[i*4+2]=brsh->norm[i*4+2];

		f=V3F_DOT(org, nv+i*4);
		nv[i*4+3]=(brsh->norm[i*4+3]-f) * (1.0/36.0);
	}

	printf("Brush Org ( %f %f %f )\n", org[0], org[1], org[2]);

	V3F_SCALE(org, 1.0/36.0, org);

	i=bsdeNewHullF(nv, brsh->n_face);
//	i=bsdeNewHullF(brsh->norm, brsh->n_face);

//	main_objs[main_nobjs++]=i;
	main_addsolid(i);

	bsdeSetAttrFV(i, BSDE_ORIGIN, org, 3);
//	bsdeSetAttrFV(i, BSDE_ORIGIN, VEC3(0, 0, 0), 3);
	bsdeSetAttrF(i, BSDE_DENSITY, 1000);
	bsdeSetAttrI(i, BSDE_STATIC, 1);
//	bsdeSetAttrI(i, BSDE_NONSOLID, 1);
}

void send_brushes(LBXGL_Brush *brsh)
{
	LBXGL_Brush *cur;

	cur=brsh;
	while(cur)
	{
		send_brush(cur);

//		break;
		cur=cur->next;
	}
}
