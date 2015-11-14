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

float pvs_org[3];

byte bsp_autovis[4096];
byte bsp_autovis_shadow[4096];
byte mdl_autovis[4096];

float bsp_autovis_mins[3];
float bsp_autovis_maxs[3];

int cam_id;

float main_time;

int cam_lighting=0;
int cam_shadows=8;

int cam_flashlight=0;

char *default_tex="aardo/aardmbr64_1";
LBXGL_Brush *brush_root;
LBXGL_SEntity *entity_root;
LBXGL_Light *light_root;

LBXGL_LightNode *light_tree=NULL;
LBXGL_BrushNode *brush_tree=NULL;
LBXGL_BrushTempPortal *brush_prt=NULL;

LBXGL_SEntity *entity_psel;	//parent of new brushes
LBXGL_SEntity *entity_sel;
LBXGL_Brush *brush_sel;
int brush_sel_face;
char *entity_sel_key;

LBXGL_ModelState *mesh_root=NULL;

LBXGL_BrushWorld *world;

int ui_state;
int shader_phong=-1;
int shader_phong_spot=-1;
int tex_screen;

char *map_name=NULL;
int map_stat_brushes;
int map_stat_ents;
int map_stat_lights;

void add_brush(LBXGL_Brush *brsh);
void draw_flush();

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

int check_brush_msel(LBXGL_Brush *brush)
{
	float sx, sy, f, g;

	f=(V3F_DOT(brush->org, cam_fw)-
		V3F_DOT(cam_org, cam_fw))/64;
	g=Vec3F_DistLineSeg(brush->org, cam_morg, cam_mend);
	if(g<f)return(1);

	return(0);
}

void update_brush_face(LBXGL_Brush *brush, int id)
{
	char buf[256];

	float tv0[3], tv1[3], tv2[3];
	float org[3];
	float sx, sy, f, g;
	int tn, xs, ys;
	int i, j, k, l, t;

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

	V3F_COPY(brush->norg+id*3, org);
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

				V3F_SUB(cam_org, cam_lorg, tv1);
				V3F_ADD(tv0, tv1, tv0);

				g=V3F_DOT(tv0, brush->norm+id*4);
				brush->norm[id*4+3]+=g;

				LBXGL_Brush_DestroyFaces(brush);
				draw_flush();
			}
		}

#if 1
//		if(V3F_NDOT(cam_org, brush->norm+id*4)>=0)
		if(1)
		{
			f=(V3F_DOT(org, cam_fw)-
				V3F_DOT(cam_org, cam_fw))/64;

			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_STENCIL_TEST);

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
#endif
	}
}

LBXGL_Brush *trace_line_brush(float *s, float *e, LBXGL_Brush *ignore)
{
	LBXGL_Brush *bcur;

	bcur=brush_root;
	while(bcur)
	{
		if(bcur==ignore)
		{
			bcur=bcur->next;
			continue;
		}

		if(LBXGL_Brush_CheckLineBrush(bcur, s, e))
			return(bcur);
		bcur=bcur->next;
	}

	return(NULL);
}

void draw_controls_brush(LBXGL_Brush *brush)
{
	float tv0[3], tv1[3], tv2[3];
	float f, g;
	int i;

	brush->flags&=~(LBXGL_BRFL_SEL|LBXGL_BRFL_OPSEL);
	if(brush_sel==brush)
		brush->flags|=LBXGL_BRFL_SEL;
	if(LBXGL_Brush_CheckLineBrush(brush, cam_morg, cam_mend))
		brush->flags|=LBXGL_BRFL_OPSEL;

//	LBXGL_Brush_DrawFaces(brush);
//	LBXGL_Brush_DrawFacesShadow(brush);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for(i=0; i<brush->n_face; i++)
	{
		update_brush_face(brush, i);
	}

	if((pdgl_mb&1) && (pdgl_mb&2) && (pdgl_lmb&2) &&
		(brush_sel==brush))
	{
		f=V3F_DOT(brush->org, cam_lmdir)-
			V3F_DOT(cam_lmorg, cam_lmdir);
		V3F_ADDSCALE(cam_morg, cam_mdir, f, tv0);
		V3F_SUB(tv0, brush->org, tv0);

		for(i=0; i<brush->n_face; i++)
		{
			brush->norm[i*4+3]+=V3F_DOT(tv0, brush->norm+i*4);
			brush->sdir[i*4+3]+=V3F_DOT(tv0, brush->sdir+i*4);
			brush->tdir[i*4+3]+=V3F_DOT(tv0, brush->tdir+i*4);
		}

		LBXGL_Brush_DestroyFaces(brush);
		draw_flush();

		return;
	}

	if((pdgl_mb&1) && (brush_sel==brush) && (brush_sel_face==-1))
	{

		f=V3F_DOT(brush->org, cam_fw)-
			V3F_DOT(cam_org, cam_fw);
		V3F_SCALEADDSCALE(
			cam_rt, f*pdgl_dmx/400.0,
			cam_up, f*(-pdgl_dmy)/400.0, tv0);
		V3F_SUB(cam_org, cam_lorg, tv1);
		V3F_ADD(tv0, tv1, tv0);

		for(i=0; i<brush->n_face; i++)
		{
			brush->norm[i*4+3]+=V3F_DOT(tv0, brush->norm+i*4);
			brush->sdir[i*4+3]+=V3F_DOT(tv0, brush->sdir+i*4);
			brush->tdir[i*4+3]+=V3F_DOT(tv0, brush->tdir+i*4);
		}

		LBXGL_Brush_DestroyFaces(brush);
		draw_flush();
	}
}

LBXGL_Brush *query_brushes_light(LBXGL_Brush *fst, LBXGL_Light *light)
{
	LBXGL_Brush *cur, *nxt, *lst, *lstb;
	float f, g, h, d;
	int i;

	f=0.10;
	if(light->flags&LBXGL_LFL_LINEAR)
		d=(6.0*light->val[0])/f;
	else	d=sqrt((6.0*light->val[0])/f);

	lst=LBXGL_BrushBSP_QueryTreeNode(brush_tree, light->org, d);

#if 1
	cur=lst; lstb=NULL;
	while(cur)
	{
		nxt=cur->chain;

#if 1
		if(cur->node)
		{
			i=cur->node->id;
			i=bsp_autovis[i>>3]&(1<<(i&7));
//			i=bsp_autovis_shadow[i>>3]&(1<<(i&7));
			if(!i) { cur=nxt; continue; }
		}
#endif

		if(!LBXGL_Brush_CheckBrushShadowFrustum(cur, light->org))
			{ cur=nxt; continue; }

//		if(!LBXGL_BrushBSP_CheckBrushPVS(brush_tree, cur, pvs_org))
//			{ cur=nxt; continue; }

		cur->chain=lstb; lstb=cur;
		cur=nxt;
	}
	lst=lstb;
#endif

	return(lst);
}

LBXGL_Brush *query_brushes_light2(LBXGL_Brush *fst, LBXGL_Light *light)
{
	LBXGL_Brush *cur, *nxt, *lst, *lstb;
	float f, g, h, d;
	int i;

	f=0.02;
	if(light->flags&LBXGL_LFL_LINEAR)
		d=(6.0*light->val[0])/f;
	else	d=sqrt((6.0*light->val[0])/f);

	lst=LBXGL_BrushBSP_QueryTreeNode(brush_tree, light->org, d);

	cur=lst; lstb=NULL;
	while(cur)
	{
		nxt=cur->chain;

#if 1
		if(cur->node)
		{
			i=cur->node->id;
			i=bsp_autovis[i>>3]&(1<<(i&7));
			if(!i) { cur=nxt; continue; }
		}
#endif

//		if(!LBXGL_Brush_CheckSphereFrustum(cur->org, cur->rad))
//			{ cur=nxt; continue; }

		if(!LBXGL_Brush_CheckBrushFrustum(cur))
			{ cur=nxt; continue; }

//		if(!LBXGL_BrushBSP_CheckBrushPVS(brush_tree, cur, pvs_org))
//			{ cur=nxt; continue; }


		cur->chain=lstb; lstb=cur;
		cur=nxt;
	}

	return(lstb);
}

void draw_models_light(LBXGL_Light *light)
{
	float tv0[3], tv1[3];
	LBXGL_ModelState *mcur;
	int i, j;

	mcur=mesh_root; i=1;
	while(mcur)
	{
//		tv0[0]=mcur->org[0]-32; tv0[1]=mcur->org[1]-32; tv0[2]=mcur->org[2]-32;
//		tv1[0]=mcur->org[0]+32; tv1[1]=mcur->org[1]+32; tv1[2]=mcur->org[2]+32;
//		i=LBXGL_BrushBSP_CheckLeafVisBBox(brush_tree, tv0, tv1, bsp_autovis);
//		if(!i) { mcur=mcur->next; continue; }

		if(!(mdl_autovis[i>>3]&(1<<(i&7))))
			{ mcur=mcur->next; i++; continue; }
		if(!LBXGL_Mdl_CheckModelLight(mcur, light))
			{ mcur=mcur->next; i++; continue; }

		LBXGL_Mdl_DrawModelLight(mcur);
		mcur=mcur->next; i++;
	}
}

void draw_models_shadow(LBXGL_Light *light)
{
	float tv0[3], tv1[3];
	LBXGL_ModelState *mcur;
	int i;

	mcur=mesh_root; i=1;
	while(mcur)
	{
//		tv0[0]=mcur->org[0]-32; tv0[1]=mcur->org[1]-32; tv0[2]=mcur->org[2]-32;
//		tv1[0]=mcur->org[0]+32; tv1[1]=mcur->org[1]+32; tv1[2]=mcur->org[2]+32;
//		i=LBXGL_BrushBSP_CheckLeafVisBBox(brush_tree, tv0, tv1, bsp_autovis);
//		if(!i) { mcur=mcur->next; continue; }

		if(!(mdl_autovis[i>>3]&(1<<(i&7))))
			{ mcur=mcur->next; i++; continue; }
		if(!LBXGL_Mdl_CheckModelLight(mcur, light))
			{ mcur=mcur->next; i++; continue; }

		LBXGL_Mdl_DrawModelShadow(mcur, light->org, 1000);
		mcur=mcur->next; i++;
	}
}

void draw_shadows_light(LBXGL_Brush *fst, LBXGL_Light *light)
{
	LBXGL_Brush *cur;
	float f, g, d;

	glDisable(GL_TEXTURE_2D);
	glColor4f(0, 1, 1, 0.1);

//	draw_models_shadow(light);

	glDepthFunc(GL_LESS);

	glDepthMask(0);
	glColorMask(0, 0, 0, 0);

	glEnable(GL_CULL_FACE);
	glStencilFunc(GL_ALWAYS, 0, 255);


	glCullFace(GL_FRONT);
	glStencilOp(GL_KEEP, GL_INCR, GL_KEEP);

	cur=fst;
	while(cur)
	{
		LBXGL_Brush_DrawShadowVolume(cur, light->org);
		cur=cur->chain;
	}

	draw_models_shadow(light);

	glCullFace(GL_BACK);
	glStencilOp(GL_KEEP, GL_DECR, GL_KEEP);

	cur=fst;
	while(cur)
	{
		LBXGL_Brush_DrawShadowVolume(cur, light->org);
		cur=cur->chain;
	}

	draw_models_shadow(light);

	glDisable(GL_CULL_FACE);

	glDepthMask(1);
	glColorMask(1, 1, 1, 1);

}

float draw_setup_light(LBXGL_Light *light)
{
	float pt[4];
	float f, g, d, gam;
	int i, j;

	glDisable(GL_COLOR_MATERIAL);

	pt[0]=0; pt[1]=0; pt[2]=0; pt[3]=1;
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, pt);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, pt);

	pt[0]=1; pt[1]=1; pt[2]=1; pt[3]=1;
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, pt);

	pt[0]=0.2; pt[1]=0.2; pt[2]=0.2; pt[3]=0.2;
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, pt);

	pt[0]=1;
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, pt);


	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glShadeModel(GL_SMOOTH);

	V3F_ZERO(pt); pt[3]=1;
	glLightfv(GL_LIGHT0, GL_AMBIENT, pt);

	V3F_COPY(light->clr, pt); pt[3]=1;
	glLightfv(GL_LIGHT0, GL_DIFFUSE, pt);

	pt[0]=1; pt[1]=1; pt[2]=1; pt[3]=1;
	glLightfv(GL_LIGHT0, GL_SPECULAR, pt);

	V3F_COPY(light->org, pt); pt[3]=1;
	glLightfv(GL_LIGHT0, GL_POSITION, pt);

	if(light->flags&LBXGL_LFL_SPOTLIGHT)
	{
		V3F_COPY(light->dir, pt); pt[3]=0;
		glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, pt);
		glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 0);

		f=light->angle;
		if(f==0)f=15;
		glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, f);
	}else
	{
		pt[0]=0; pt[1]=0; pt[2]=-1; pt[3]=0;
		glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, pt);
		glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 180);
		glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 0);
	}


	gam=light->val[0]; f=pdgl_time/1000.0;

	switch(light->style)
	{
	case 1: gam*=sin(f*M_PI)*0.5+0.5; break;
	case 2: gam*=((int)f)&1; break;
	case 3: gam*=sin(2*f*M_PI)*0.5+0.5; break;
	case 4: gam*=sin(4*f*M_PI)*0.5+0.5; break;

	case 5: i=f*4; if(!(i%4))gam=0; break;
	case 6: i=f*4; if(i%4)gam=0; break;
	case 7: i=f*8; if(!(i%8))gam=0; break;
	case 8: i=f*8; if(i%8)gam=0; break;

	case 9: g=bgbrng_randf(); if(g<0.2)gam=0; break;
	case 10: g=bgbrng_randf(); if(g>0.2)gam=0; break;
	default: break;
	}


	if(light->flags&LBXGL_LFL_LINEAR)
	{
//		g=36*6.0/255.0;
		g=36*12.0/255.0;
		f=g*gam;
		glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 1.0/f);
		glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0);
		glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.0);
	}else
	{
//		g=(36*36)*6.0/255.0;
		g=(36*36)*24.0/255.0;
		f=g*gam;
		glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 1.0/f);
		glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.0);
		glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.0);
	}
	glEnable(GL_LIGHT0);

	return(light->val[0]);
}

float draw_light_gamma_point(LBXGL_Light *light, float *org, float rad)
{
	float f, d;

	d=(V3F_DIST(light->org, org)-rad)/36.0;
	if(d<0)d=0;
	if(light->flags&LBXGL_LFL_LINEAR)
		f=(6.0*light->val[0])/(d+1);
	else	f=(6.0*light->val[0])/(d*d+1);

	return(f);
}

void draw_brushes_light(LBXGL_Brush *fst, LBXGL_Light *light)
{
	LBXGL_Brush *cur;
	float f, g, d;

	draw_setup_light(light);

	glDisable(GL_TEXTURE_2D);
	glColor4f(1, 1, 1, 1);

	if(light->flags&LBXGL_LFL_SPOTLIGHT)
		PDGL_BindShader(shader_phong_spot);
		else PDGL_BindShader(shader_phong);

	glDepthFunc(GL_LEQUAL);
	glStencilFunc(GL_EQUAL, 0, 255);
	glBlendFunc(GL_SRC_COLOR, GL_ONE);

//	glDisable(GL_CULL_FACE);

	cur=fst;
	while(cur)
	{
		glEnable(GL_CULL_FACE);
		LBXGL_Brush_DrawBrushLight2(cur);
		cur=cur->chain;
	}

	PDGL_UnbindShader();

	draw_models_light(light);

//	PDGL_UnbindShader();

	glDisable(GL_LIGHTING);
}

LBXGL_Light *query_lights_cam()
{
	LBXGL_Light *cur, *cur1, *lst, *prv;
	float f;
	int i;

	cur=light_root; lst=NULL;
	while(cur)
	{
//		if(cur->val[0]<=100) { cur=cur->next; continue; }

#if 1
		f=LBXGL_Shadow_LightGammaRadius(cur);
		i=LBXGL_Brush_CheckSphereFrustum(cur->org, f);
		if(!i) { cur=cur->next; continue; }
#endif

		f=draw_light_gamma_point(cur, cam_org, 32);
//		if(f<0.10) { cur=cur->next; continue; }

//		f=1.0/(V3F_DIST(cur->org, cam_org)+1);

		cur->val[1]=f;

		if(cur->flags&LBXGL_LFL_NOSHADOW)
		{
			cur1=lst; prv=NULL;

			while(cur1)
			{
				if(cur1->flags&LBXGL_LFL_NOSHADOW)
					break;
				prv=cur1; cur1=cur1->chain;
			}

			while(cur1)
			{
				if(f>cur1->val[1])break;
				prv=cur1; cur1=cur1->chain;
			}
		}else
		{
			cur1=lst; prv=NULL;
			while(cur1)
			{
				if(cur1->flags&LBXGL_LFL_NOSHADOW)break;
				if(f>cur1->val[1])break;
				prv=cur1; cur1=cur1->chain;
			}
		}

		if(prv) { cur->chain=prv->chain; prv->chain=cur; }
			else { cur->chain=lst; lst=cur; }

		cur=cur->next;
	}

	return(lst);
}

void draw_bsp_light(LBXGL_BrushNode *root, LBXGL_Light *light)
{
	LBXGL_Brush *cur;
	float f, g, d;

	draw_setup_light(light);

	glDisable(GL_TEXTURE_2D);
	glColor4f(1, 1, 1, 1);

	if(light->flags&LBXGL_LFL_SPOTLIGHT)
		PDGL_BindShader(shader_phong_spot);
		else PDGL_BindShader(shader_phong);

	glDepthFunc(GL_LEQUAL);
	glStencilFunc(GL_EQUAL, 0, 255);
	glBlendFunc(GL_SRC_COLOR, GL_ONE);

//	glDisable(GL_CULL_FACE);

	f=0.02;
	if(light->flags&LBXGL_LFL_LINEAR)
		d=(6.0*light->val[0])/f;
	else	d=sqrt((6.0*light->val[0])/f);

	glDisable(GL_TEXTURE_2D);
	glColor4f(1, 1, 1, 1);

	glEnable(GL_CULL_FACE);
	LBXGL_BrushBSP_DrawFacesNodeFlatSphere(root, light->org, d);

	PDGL_UnbindShader();
	glDisable(GL_LIGHTING);
}

void draw_bsp_shadows(LBXGL_BrushNode *root, LBXGL_Light *light)
{
	LBXGL_Brush *cur;
	float f, g, d;

	f=0.15;
	if(light->flags&LBXGL_LFL_LINEAR)
		d=(6.0*light->val[0])/f;
	else	d=sqrt((6.0*light->val[0])/f);

	glDepthFunc(GL_LESS);

	glDepthMask(0);
	glColorMask(0, 0, 0, 0);

	glEnable(GL_CULL_FACE);
	glStencilFunc(GL_ALWAYS, 0, 255);


	glCullFace(GL_FRONT);
	glStencilOp(GL_KEEP, GL_INCR, GL_KEEP);

	LBXGL_BrushBSP_DrawVolFacesNodeFlatSphere(root, light->org, d);

	glCullFace(GL_BACK);
	glStencilOp(GL_KEEP, GL_DECR, GL_KEEP);

	LBXGL_BrushBSP_DrawVolFacesNodeFlatSphere(root, light->org, d);

	glDisable(GL_CULL_FACE);

	glDepthMask(1);
	glColorMask(1, 1, 1, 1);
}

void draw_scene_bsp_faces()
{
	LBXGL_Brush *cur, *lst;
	LBXGL_Light *lcur;
	int i, j;

	shader_phong=PDGL_LoadShader("pointlight");
	shader_phong_spot=PDGL_LoadShader("spotlight");

	glDepthFunc(GL_LEQUAL);

	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	glDisable(GL_TEXTURE_2D);
//	glColor4f(1, 1, 1, 1);
	glColor4f(0, 0, 0, 1);

	LBXGL_BrushBSP_DrawFacesNodeFlat(brush_tree);

//	return;

	lcur=query_lights_cam(); i=cam_shadows;
#if 1
	while(lcur && i)
	{
		glClear(GL_STENCIL_BUFFER_BIT);
		glEnable(GL_STENCIL_TEST);
		lst=query_brushes_light(NULL, lcur);
		draw_shadows_light(lst, lcur);

//		draw_bsp_shadows(brush_tree, lcur);

		draw_bsp_light(brush_tree, lcur);
		glDisable(GL_STENCIL_TEST);
		i--; lcur=lcur->chain;
	}
#endif

	while(lcur)
	{
		draw_bsp_light(brush_tree, lcur);
		lcur=lcur->chain;
	}

	LBXGL_BrushBSP_DrawNodeFacesShadow(brush_tree);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}


void draw_flashlight()
{
	float pt0[3], pt1[3], pt2[3], pt3[3];
	LBXGL_Brush *cur, *lst;
	LBXGL_Light *lcur;
	int i, j, tn;
	float f;

#if 0
	if(cam_flashlight)
	{
		lcur=gcalloc(sizeof(LBXGL_Light));

		f=0.57735;
		V3F_SET(lcur->clr, f, f, f);

		V3F_ADDSCALE(cam_org, cam_up, -12, lcur->org);

		lcur->val[0]=1000;
		lcur->val[1]=0;

		V3F_COPY(cam_fw, lcur->dir);

		lcur->flags|=LBXGL_LFL_SPOTLIGHT;
		lcur->angle=10;
		

		glClear(GL_STENCIL_BUFFER_BIT);
		glEnable(GL_STENCIL_TEST);
		lst=query_brushes_light(fst, lcur);
		draw_shadows_light(lst, lcur);
		lst=query_brushes_light2(fst, lcur);
		draw_brushes_light(lst, lcur);
		glDisable(GL_STENCIL_TEST);
	}
#endif

	if(!cam_flashlight)
		return;

	V3F_SCALEADDSCALE(cam_rt, -6, cam_up, -6, pt0);
	V3F_SCALEADDSCALE(cam_rt,  6, cam_up, -6, pt1);
	V3F_SCALEADDSCALE(cam_rt,  6, cam_up,  6, pt2);
	V3F_SCALEADDSCALE(cam_rt, -6, cam_up,  6, pt3);

	V3F_ADDSCALE(pt0, cam_fw, 12, pt0);
	V3F_ADDSCALE(pt1, cam_fw, 12, pt1);
	V3F_ADDSCALE(pt2, cam_fw, 12, pt2);
	V3F_ADDSCALE(pt3, cam_fw, 12, pt3);

	V3F_ADD(pt0, cam_org, pt0);
	V3F_ADD(pt1, cam_org, pt1);
	V3F_ADD(pt2, cam_org, pt2);
	V3F_ADD(pt3, cam_org, pt3);

	glDepthMask(0);
	glBlendFunc(GL_SRC_COLOR, GL_ONE);
	glColor4f(1, 1, 1, 1);

	tn=Tex_LoadFile("textures/effects/flash0", NULL, NULL);
	glBindTexture(GL_TEXTURE_2D, tn);
	glEnable(GL_TEXTURE_2D);

	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0);
	glVertex3fv(pt0);
	glTexCoord2f(1, 0);
	glVertex3fv(pt1);
	glTexCoord2f(1, 1);
	glVertex3fv(pt2);
	glTexCoord2f(0, 1);
	glVertex3fv(pt3);
	glEnd();

	glDepthMask(1);
}

void draw_models_dark()
{
	float tv0[3], tv1[3];
	LBXGL_ModelState *mcur;
	int i;

	mcur=mesh_root; i=1;
	while(mcur)
	{
//		tv0[0]=mcur->org[0]-32; tv0[1]=mcur->org[1]-32; tv0[2]=mcur->org[2]-32;
//		tv1[0]=mcur->org[0]+32; tv1[1]=mcur->org[1]+32; tv1[2]=mcur->org[2]+32;
//		i=LBXGL_BrushBSP_CheckLeafVisBBox(brush_tree, tv0, tv1, bsp_autovis);
//		if(!i) { mcur=mcur->next; continue; }

		if(!(mdl_autovis[i>>3]&(1<<(i&7))))
			{ mcur=mcur->next; i++; continue; }

		LBXGL_Mdl_DrawModelDark(mcur);
		mcur=mcur->next; i++;
	}
}

void draw_models_final()
{
	float tv0[3], tv1[3];
	LBXGL_ModelState *mcur;
	int i;

	mcur=mesh_root; i=1;
	while(mcur)
	{
//		tv0[0]=mcur->org[0]-32; tv0[1]=mcur->org[1]-32; tv0[2]=mcur->org[2]-32;
//		tv1[0]=mcur->org[0]+32; tv1[1]=mcur->org[1]+32; tv1[2]=mcur->org[2]+32;
//		i=LBXGL_BrushBSP_CheckLeafVisBBox(brush_tree, tv0, tv1, bsp_autovis);
//		if(!i) { mcur=mcur->next; continue; }

		if(!(mdl_autovis[i>>3]&(1<<(i&7))))
			{ mcur=mcur->next; i++; continue; }

		LBXGL_Mdl_DrawModelFinal(mcur);
		mcur=mcur->next; i++;
	}
}

void draw_brushes(LBXGL_Brush *fst)
{
	static short *stbuf=NULL;
	LBXGL_BrushNode *node;
	LBXGL_Brush *cur, *lst;
	LBXGL_Light *lcur;
	LBXGL_ModelState *mcur;
	int i, j;

	if(!cam_lighting)
	{
		glCullFace(GL_BACK);

		cur=fst;
		while(cur)
		{
			glEnable(GL_CULL_FACE);
			LBXGL_Brush_DrawFaces(cur);
			cur=cur->next;
		}

//		if(brush_sel)draw_controls_brush(brush_sel);

		cur=fst;
		while(cur)
		{
			draw_controls_brush(cur);
			cur=cur->next;
		}


		mcur=mesh_root;
		while(mcur)
		{
			LBXGL_Mdl_DrawModel(mcur);
			mcur=mcur->next;
		}

		return;
	}

	if(!brush_tree)
	{
		cur=fst; lst=NULL;
		while(cur)
		{
			if(!cur->xyz)LBXGL_Brush_BuildFaces(cur);
			cur->chain=lst; lst=cur; cur=cur->next;
		}

//		LBXGL_BrushBSP_FreeTreeNode(brush_tree);
//		brush_tree=LBXGL_BrushBSP_BuildNodeList(lst);
//		brush_tree=LBXGL_BrushBSP_BuildTreeList(lst);
//		brush_prt=LBXGL_BrushBSP_BuildPortals(brush_tree);

		brush_tree=LBXGL_BrushBSP_BuildTreeListFast(lst);
	}

	V3F_COPY(cam_org, pvs_org);

#if 0
//	i=LBXGL_BrushBSP_GetPVSLeafID(brush_tree, pvs_org);
	node=LBXGL_BrushBSP_LookupLeafPoint(brush_tree, pvs_org);
	printf("PVS ID %d\n", node->id);

	while(node)
	{
		printf("\t(%g %g %g %g)\n", node->norm[0], node->norm[1],
			node->norm[2], node->norm[3]);
		node=node->pnode;
	}
#endif

//	draw_scene_bsp_faces();
//	return;

	shader_phong=PDGL_LoadShader("pointlight");
	shader_phong_spot=PDGL_LoadShader("spotlight");

	glDepthFunc(GL_LEQUAL);

	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

#if 1
	cur=fst;
	while(cur)
	{
//		if(!LBXGL_BrushBSP_CheckBrushPVS(brush_tree, cur, pvs_org))
//			{ cur=cur->next; continue; }

#if 1
		if(cur->node)
		{
			i=cur->node->id;
			i=bsp_autovis[i>>3]&(1<<(i&7));
			if(!i) { cur=cur->next; continue; }
		}
#endif

		LBXGL_Brush_DrawBrushDark(cur);
		cur=cur->next;
	}
#endif

	draw_models_dark();

#if 1
	lcur=query_lights_cam(); i=cam_shadows;
	while(lcur && i)
	{
		if(lcur->flags&LBXGL_LFL_NOSHADOW)
			break;

		glClear(GL_STENCIL_BUFFER_BIT);
		glEnable(GL_STENCIL_TEST);
		lst=query_brushes_light(fst, lcur);
		draw_shadows_light(lst, lcur);
//		draw_models_shadow(lcur);
		lst=query_brushes_light2(fst, lcur);
		draw_brushes_light(lst, lcur);
//		draw_models_light(lcur);
		glDisable(GL_STENCIL_TEST);
		i--; lcur=lcur->chain;
	}

	while(lcur)
	{
//		glClear(GL_STENCIL_BUFFER_BIT);
//		glEnable(GL_STENCIL_TEST);
		lst=query_brushes_light2(fst, lcur);
		draw_brushes_light(lst, lcur);
//		draw_models_light(lcur);
//		glDisable(GL_STENCIL_TEST);
		lcur=lcur->chain;
	}
#endif

	draw_flashlight();

	glDepthFunc(GL_LEQUAL);

	glStencilFunc(GL_EQUAL, 0, 255);

	cur=fst;
	while(cur)
	{
//		if(!LBXGL_BrushBSP_CheckBrushPVS(brush_tree, cur, pvs_org))
//			{ cur=cur->next; continue; }

#if 1
		if(cur->node)
		{
			i=cur->node->id;
			i=bsp_autovis[i>>3]&(1<<(i&7));
			if(!i) { cur=cur->next; continue; }
		}
#endif

		glEnable(GL_CULL_FACE);
		glBlendFunc(GL_DST_COLOR, GL_ZERO);
		LBXGL_Brush_DrawFacesShadow(cur);
		cur=cur->next;
	}

	draw_models_final();

	LBXGL_Shader_FlushState();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	LBXGL_Shader_Color4f(1, 1, 1, 1);

	cur=fst;
	while(cur)
	{
		if(cur->flags&LBXGL_BRFL_ALPHA)
			LBXGL_Brush_DrawFaces(cur);
		cur=cur->next;
	}

//	if(brush_sel)draw_controls_brush(brush_sel);

#if 1
	cur=fst;
	while(cur)
	{
		draw_controls_brush(cur);
		cur=cur->next;
	}
#endif

//	LBXGL_BrushBSP_DrawPortalList(brush_tree, brush_prt);

	LBXGL_Shader_FlushState();

	return;
}


void draw_build_fast();

void draw_test_autovis()
{
	static byte *stbuf=NULL;
	float tv0[4], tv1[4];
	LBXGL_Light *lcur;
	LBXGL_ModelState *mcur;
	int i, j, k, l;
	float f, g, d;

	if(!stbuf)stbuf=malloc(320*240*4);

	draw_build_fast();

	UI_Camera_SetupView3D(0, 0, 160, 120);
	glCullFace(GL_BACK); glEnable(GL_CULL_FACE);

#if 1
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	LBXGL_Brush_DrawBSPNodeColors(brush_tree);

	glFinish();
	glReadPixels(0, 0, 160, 120, GL_RGB, GL_UNSIGNED_BYTE, stbuf);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	memset(bsp_autovis, 0, 4096);
	for(i=0; i<(160*120); i++)
	{
		j=(stbuf[i*3+0]>>2) | ((stbuf[i*3+1]>>2)<<6) |
			((stbuf[i*3+2]>>2)<<12);
		bsp_autovis[j>>3]|=1<<(j&7);
	}
#endif

#if 1
	LBXGL_Brush_DrawBSPNodeBlack(brush_tree);

	mcur=mesh_root; i=1;
	while(mcur)
	{
//		i=LBXGL_BrushBSP_GetPVSLeafID(brush_tree, mcur->org);
		j=i&63; k=(i>>6)&63; l=(i>>12)&63;
		glColor4ub((j<<2)+2, (k<<2)+2, (l<<2)+2, 255);
//		LBXGL_Mdl_DrawModelFlat(mcur);

		LBXGL_Mdl_BoxModel(mcur, tv0, tv1);
		PD3D_DrawFlatCube2(
			tv0[0], tv0[1], tv0[2], tv1[0], tv1[1], tv1[2]);
		mcur=mcur->next; i++;
	}

	glFinish();
	glReadPixels(0, 0, 160, 120, GL_RGB, GL_UNSIGNED_BYTE, stbuf);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	memset(mdl_autovis, 0, 4096);
	for(i=0; i<(160*120); i++)
	{
		j=(stbuf[i*3+0]>>2) | ((stbuf[i*3+1]>>2)<<6) |
			((stbuf[i*3+2]>>2)<<12);
		mdl_autovis[j>>3]|=1<<(j&7);
	}
#endif

	LBXGL_BrushBSP_BBoxVis(brush_tree, bsp_autovis,
		bsp_autovis_mins, bsp_autovis_maxs);

#if 0
	memset(bsp_autovis_shadow, 0, 4096);

	UI_Camera_SetupView3D(0, 0, 100, 75);

	LBXGL_Brush_DrawBSPNodeBlack(brush_tree);
	glDepthMask(0);

	lcur=query_lights_cam(); i=cam_shadows;
	while(lcur && i)
	{
		if(lcur->flags&LBXGL_LFL_NOSHADOW)
			break;

		f=0.15;
		if(lcur->flags&LBXGL_LFL_LINEAR)
			d=(6.0*lcur->val[0])/f;
		else	d=sqrt((6.0*lcur->val[0])/f);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		LBXGL_Brush_DrawBSPShadowNodeColors(brush_tree, lcur->org, d);

		glFinish();
		glReadPixels(0, 0, 100, 75, GL_RGB, GL_UNSIGNED_BYTE, stbuf);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for(j=0; j<(100*75); j++)
		{
			k=(stbuf[j*3+0]>>2) | ((stbuf[j*3+1]>>2)<<6) |
				((stbuf[j*3+2]>>2)<<12);
			bsp_autovis_shadow[k>>3]|=1<<(k&7);
		}

		i--; lcur=lcur->next;
	}

	glDepthMask(1);

//	for(i=0; i<64; i++)
//		glReadPixels(0, 0, 256, 256, GL_RGB, GL_UNSIGNED_BYTE, stbuf);
#endif

}

void draw_test_defsdr()
{
	int sdr, tex, def;

	return;

	Tex_FillTextureScreenshot(tex_screen);

	tex=Tex_LoadFile("textures/effects/fire/bgb_fire3", NULL, NULL);
	def=Tex_LoadFile("textures/effects/fire/turb0", NULL, NULL);

//	def=Tex_LoadFile("textures/effects/gloss0", NULL, NULL);
//	def=Tex_LoadFile("textures/effects/glass_hvgrid", NULL, NULL);

	sdr=PDGL_LoadShader("bgb_deform1");
//	sdr=PDGL_LoadShader("warp0");
	PDGL_BindShader(sdr);

	PDGL_Uniform1f("time", pdgl_time/1000.0);
	PDGL_Uniform1f("scale", 0.05);
	PDGL_Uniform1f("rate", 0.25);

//	PDGL_Uniform1i("basetex", tex_screen);
//	PDGL_Uniform1i("deformtex", def);

	PDGL_Uniform1i("texScreen", 0);
	PDGL_Uniform1i("texBase", 1);
	PDGL_Uniform1i("texDeform", 2);

//	glDisable(GL_TEXTURE_2D);

//	glBindTexture(GL_TEXTURE_2D, tex_screen);
//	glEnable(GL_TEXTURE_2D);

	pdglActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, tex_screen);
//	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glEnable(GL_TEXTURE_2D);

	pdglActiveTexture(1);
	glBindTexture(GL_TEXTURE_2D, tex);
	glEnable(GL_TEXTURE_2D);

	pdglActiveTexture(2);
	glBindTexture(GL_TEXTURE_2D, def);
	glEnable(GL_TEXTURE_2D);

	pdglActiveTexture(0);

//	glColor4f(0.01, 0.01, 0.01, 1);
	glColor4f(1, 1, 1, 0.75);

	glBegin(GL_QUADS);

	pdglMultiTexCoord2f(0, 0, 1);
	pdglMultiTexCoord2f(1, 0, 1);
	glVertex3f(-16, 16, 16);

	pdglMultiTexCoord2f(0, 0, 0);
	pdglMultiTexCoord2f(1, 0, 0);
	glVertex3f(-16, 16, -16);

	pdglMultiTexCoord2f(0, 1, 0);
	pdglMultiTexCoord2f(1, 1, 0);
	glVertex3f(16, 16, -16);

	pdglMultiTexCoord2f(0, 1, 1);
	pdglMultiTexCoord2f(1, 1, 1);
	glVertex3f(16, 16, 16);

	glEnd();


	pdglActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);

	pdglActiveTexture(1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);

	pdglActiveTexture(2);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);

	pdglActiveTexture(0);

	PDGL_UnbindShader();
}

void draw_build_fast()
{
	LBXGL_Brush *cur, *lst;

	if(!brush_tree)
	{
		cur=brush_root; lst=NULL;
		while(cur)
		{
			if(!cur->xyz)LBXGL_Brush_BuildFaces(cur);
			cur->chain=lst; lst=cur; cur=cur->next;
		}

		brush_tree=LBXGL_BrushBSP_BuildTreeListFast(lst);
	}
}

void draw_build()
{
	LBXGL_Brush *cur, *lst;

	draw_flush();

	cur=brush_root; lst=NULL;
	while(cur)
	{
		if(!cur->xyz)LBXGL_Brush_BuildFaces(cur);
		cur->chain=lst; lst=cur; cur=cur->next;
	}

	brush_tree=LBXGL_BrushBSP_BuildTreeList(lst);
}

void draw_flush()
{
	if(brush_tree)
	{
		LBXGL_BrushBSP_FreeTreeNode(brush_tree);
		brush_tree=NULL;
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
	float f, g, bf;
	LBXGL_Brush *cur, *best;
	int i;

	if(!brush_sel && !cam_ortho)
	{
		best=NULL; bf=999999;

		cur=fst;
		while(cur)
		{
			i=LBXGL_Brush_CheckLineBrush(cur,
				cam_morg, cam_mend);
			if(!i) { cur=cur->next; continue; }

			f=V3F_DOT(cur->org, cam_fw)-V3F_DOT(cam_org, cam_fw);

			if(!best || f<bf) { best=cur; bf=f; }

			cur=cur->next;
		}


		brush_sel=best;
		brush_sel_face=-1;

		return;
	}

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

		i=LBXGL_Brush_CheckLineBrush(cur, cam_morg, cam_mend);

		if(i && !brush_sel)
		{
			brush_sel=cur;
			brush_sel_face=-1;
		}

		cur=cur->next;

#if 0
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
#endif
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
	float org[3], tv[3], tv1[3], ang[3];
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

	glColor4f(1, 1, 1, 1);

	f=ent_radius(ent);
	g=Vec3F_DistLineSeg(org, cam_morg, cam_mend);
	if(g<f)glColor4f(1, 0.5, 1, 1);

	if(ent==entity_sel)glColor4f(0.5, 1, 0.5, 1);

	if((ent==entity_sel) && (pdgl_mb&1))
	{
		if((pdgl_mb&2) && (pdgl_lmb&2))
		{
			f=V3F_DOT(org, cam_lmdir)-
				V3F_DOT(cam_lmorg, cam_lmdir);
			V3F_ADDSCALE(cam_morg, cam_mdir, f, tv);
			V3F_SUB(tv, org, tv);
		}else
		{
			f=V3F_DOT(org, cam_fw)-
				V3F_DOT(cam_org, cam_fw);
			V3F_SCALEADDSCALE(
				cam_rt, f*pdgl_dmx/400.0,
				cam_up, f*(-pdgl_dmy)/400.0, tv);
			V3F_SUB(cam_org, cam_lorg, tv1);
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

	s=LBXGL_SEnt_GetStr(ent, "_flare");
	if(s)LBXGL_Sprite_RenderFlareName(s, org, 1000);

	if(ent->mdl)
	{
		V3F_COPY(org, ent->mdl->org);

		h=ang[2]-90;
		f=cos(h*(M_PI/180)); g=sin(h*(M_PI/180));
		V3F_SET(ent->mdl->rot+0, f, g, 0);
		V3F_SET(ent->mdl->rot+3, -g, f, 0);

//		LBXGL_Mdl_DrawModel(ent->mdl);
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

	if(ent==entity_root)
	{
		entity_root=entity_root->next;
		return;
	}

	cur=entity_root;
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

	lcur=light_root;
	while(lcur)
	{
		lnxt=lcur->next;
		gcfree(lcur);
		lcur=lnxt;
	}

	light_root=NULL;

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
				g=0.57735;
				V3F_SET(lcur->clr, g, g, g);
			}

			LBXGL_SEnt_GetVec3(cur, "origin", lcur->org);
			lcur->val[0]=LBXGL_SEnt_GetNum(cur, "light");
			lcur->val[1]=0;

			if(lcur->val[0]==0)
				lcur->val[0]=250;

			i=LBXGL_SEnt_GetNum(cur, "linear");
			if(i)lcur->flags|=LBXGL_LFL_LINEAR;

			i=LBXGL_SEnt_GetNum(cur, "noshadows");
			if(i)lcur->flags|=LBXGL_LFL_NOSHADOW;

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

			lcur->next=light_root;
			light_root=lcur;
		}

#if 1
		if(!strncmp(ty, "monster", 7))
		{
			if(!cur->mdl)
			{
				cur->mdl=LBXGL_Mdl_LoadModelInstance("objects/monitor31");

				if(cur->mdl)
				{
					cur->mdl->next=mesh_root;
					mesh_root=cur->mdl;
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
					cur->mdl->next=mesh_root;
					mesh_root=cur->mdl;
				}
			}
		}

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

	Con_Printf("Saving Map %s\n", name);

	map_name=dystring(name);

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
	float tv0[4], tv1[4], tv2[4];
	float vs[4], vt[4], tn[4];
	float f, g, h;

	LBXGL_Brush *tmp;
	char **a, *tex;
	int i, n;

	map_stat_brushes++;

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

		tex=dystring(a[15]);

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

			tex=dystring(buf);

			Hull_AdjacentNormals21(tn, vs, vt);
			vs[3]=-atof(a[16]);
			vt[3]=atof(a[17]);

			f=atof(a[18])*(M_PI/180);
			g=cos(f); h=sin(f);
			V3F_SCALEADDSCALE(vs, g, vt, h, tv0);
			V3F_SCALEADDSCALE(vt, g, vs, -h, tv1);
//			V3F_COPY(tv0, vs);
//			V3F_COPY(tv1, vt);

			f=atof(a[19]); g=atof(a[20]);
			V3F_SCALE(tv0, f, vs);
			V3F_SCALE(tv1, g, vt);
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

	map_stat_ents++;

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

		if(!strcmp(a[0], "classname") && !strncmp(a[1], "light", 5))
			map_stat_lights++;

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

	draw_flush();

	fd=fopen(name, "rt");
	if(!fd)return;

	entity_root=NULL;
	brush_root=NULL;
	entity_sel=NULL;
	entity_psel=NULL;
	brush_sel=NULL;

	map_name=dystring(name);
	map_stat_brushes=0;
	map_stat_ents=0;
	map_stat_lights=0;

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

	printf("Map Loaded: %d brushes, %d ents, %d lights\n",
		map_stat_brushes, map_stat_ents, map_stat_lights);
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

int ConCmds_Shadows(char **a)
{
	if(!a[1])
	{
		Con_Printf("Shadows=%d\n", cam_shadows);
		return(0);
	}

	cam_shadows=atoi(a[1]);
	return(0);
}

int main_new()
{
	float v[3];
	LBXGL_Brush *brsh;

	draw_flush();

	map_name=NULL;
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

	tex_screen=Tex_ScreenshotTexture();
	LBXGL_Particle_Init();


	ConCmds_Register("ent", "Get/Set entity property", ConCmds_Ent);
	ConCmds_Register("load", "Load map file", ConCmds_Load);
	ConCmds_Register("save", "Save map file", ConCmds_Save);

	ConCmds_Register("sky", "Set sky", ConCmds_Sky);

	ConCmds_Register("shadows", "Set Shadows", ConCmds_Shadows);

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
	}


	kcur=pdgl_keybuf;
	while(*kcur)
	{
		if(PDGL_KeyDown(K_CTRL))
		{
			if(*kcur=='s')
			{
				if(map_name)save_file(map_name);
			}

			if(*kcur=='b')
			{
				draw_build();
			}

			kcur++;
			continue;
		}

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

		if(*kcur=='L')
		{
			cam_lighting=!cam_lighting;
			Con_Printf("Lighting: %s\n",
				cam_lighting?"Enabled":"Disabled");
		}

		if(*kcur=='f')
		{
			cam_flashlight=!cam_flashlight;
			Con_Printf("Flashlight: %s\n",
				cam_flashlight?"Enabled":"Disabled");
		}

		if(*kcur=='d')
		{
			new_brush_entity("func_door");
		}

		if(*kcur==K_INS)
		{
			draw_flush();

			V3F_ADDSCALE(cam_org, cam_fw, 24, pt);

			brsh=LBXGL_Brush_MakeCube(pt[0], pt[1], pt[2], 6, 6, 6,
				default_tex);
			add_brush(brsh);
		}

		if(*kcur==K_DEL)
		{
			if(entity_sel)
			{
				unlink_entity(entity_sel);
				kcur++;
				continue;
			}
		}

		if(*kcur=='p')
		{
			V3F_COPY(cam_org, pvs_org);
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
//	main_handle_form("help");
//	main_handle_form("selbox");
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

	main_time+=pdgl_dt_f;
	LBXGL_Particle_Step(pdgl_dt_f);

	draw_test_autovis();


	UI_Camera_Setup3D();

	LBXGL_Sprite_SetCameraOrgRot(cam_org, cam_rot);

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

	setup_light_ents(entity_root);

	draw_brushes(brush_root);
	draw_ents(entity_root);

	draw_test_defsdr();

	LBXGL_Particle_Draw();


//	Tex_FillTextureScreenshot(tex_screen);

	UI_Camera_Setup2D();

//	Draw_SetSolid2_2D(4.0/3.0, 400,
//		VEC3(0, 0, 0), VEC3(0, 0, 0), 0, 0, pdgl_xs, pdgl_ys);

	if(0)
	{
		glColor4f(1, 1, 1, 1);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, tex_screen);

		glBegin(GL_POLYGON);

		glTexCoord2f(0, 0);
		glVertex2f(400-16*8, -300);

		glTexCoord2f(1, 0);
		glVertex2f(400, -300);

		glTexCoord2f(1, 1);
		glVertex2f(400, -300+16*8);

		glTexCoord2f(0, 1);
		glVertex2f(400-16*8, -300+16*8);

		glEnd();
	}

	Con_Render();

	GfxFont_SetFont("fixed", 0);

	adt=(adt*0.90)+(pdgl_dt_f*0.10);
	sprintf(buf, "(%g %g %g) (%g %g %g) %.2ffps",
		cam_org[0], cam_org[1], cam_org[2],
		cam_ang[0], cam_ang[1], cam_ang[2],
		1.0/adt);
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
		glVertex2f(400-16*8, -300);

		glTexCoord2f(1, 0);
		glVertex2f(400, -300);

		glTexCoord2f(1, 1);
		glVertex2f(400, -300+16*8);

		glTexCoord2f(0, 1);
		glVertex2f(400-16*8, -300+16*8);

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

				LBXGL_Brush_DestroyFaces(brush_sel);
				draw_flush();
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
#endif


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
