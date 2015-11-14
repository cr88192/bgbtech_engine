#include <lbxgl2.h>

#include <time.h>

void BSP_DrawFace2(float *xyz, float *st, int num,
	int fl, int txs, int tys)
{
	float s, t, f, g;
	int i;

	glBegin(GL_POLYGON);
	for(i=0; i<num; i++)
	{
		s=st[i*2+0];	t=st[i*2+1];

#if 1
		if(fl&LBXGL_TXFL_WARPST)
		{
			f=V3F_LEN(xyz+i*3);

			g=clock()/(float)CLOCKS_PER_SEC;
			s+=sin(g+f)/txs*4;
			t+=cos(g+f)/tys*4;
		}
#endif

		glTexCoord2f(s, t);
		glVertex3fv(xyz+i*3);
	}
	glEnd();
}

void BSP_DrawFace21(float *xyz, int num)
{
	float s, t, f;
	int i;

	glDisable(GL_TEXTURE_2D);
	glColor4f(1, 1, 1, 1);

	glBegin(GL_POLYGON);
	for(i=0; i<num; i++)
		glVertex3fv(xyz+i*3);
	glEnd();
}

void BSP_DrawFace22(float *xyz, int num)
{
	float s, t, f;
	int i;

	glDisable(GL_TEXTURE_2D);
	glColor4f(0, 1, 0, 0.5);

	glBegin(GL_POLYGON);
	for(i=0; i<num; i++)
		glVertex3fv(xyz+i*3);
	glEnd();
}

void BSP_DrawFace23(LBXGL_World *wrl, BSP_Face *face, float *xyz, float *st, int num,
	int fl, int txs, int tys)
{
	float s, t, f;
	float r, g, b;
	int i, j, k, l;

	if(!face->lm->vec_rgb)
		return;

	glBegin(GL_POLYGON);
	for(i=0; i<num; i++)
	{
		s=st[i*2+0];	t=st[i*2+1];

#if 1
		if(fl&LBXGL_TXFL_WARPST)
		{
			f=V3F_LEN(xyz+i*3);

			g=clock()/(float)CLOCKS_PER_SEC;
			s+=sin(g+f)/txs*4;
			t+=cos(g+f)/tys*4;
		}
#endif

		j=face->vidx[i];
		r=face->lm->vec_srgb[j*3+0];
		g=face->lm->vec_srgb[j*3+1];
		b=face->lm->vec_srgb[j*3+2];


		for(k=0; k<face->lm->dn; k++)
		{
			f=wrl->style_gamma[face->lm->dlmstyle[k]];
			r+=face->lm->vec_drgb[k][j*3+0]*f;
			g+=face->lm->vec_drgb[k][j*3+1]*f;
			b+=face->lm->vec_drgb[k][j*3+2]*f;
		}

		r+=face->lm->vec_rgb[j*3+0];
		g+=face->lm->vec_rgb[j*3+1];
		b+=face->lm->vec_rgb[j*3+2];

		glColor4f(r/255.0, g/255.0, b/255.0, 1.0);

		glTexCoord2f(s, t);
		glVertex3fv(xyz+i*3);
	}
	glEnd();
}

void BSP_DrawFace3(float *xyz, float *st, int num,
	int fl, int txs, int tys)
{
	float s, t, f;
	int i;

	glBegin(GL_POLYGON);
	for(i=0; i<num; i++)
	{
		glTexCoord2fv(st+i*2);
		glVertex3fv(xyz+i*3);
	}
	glEnd();
}

#if 0
void BSP_DrawFace4(float *xyz, float *st, int num)
{
	float f, d;
	int i, j;
	float r, g, b;

#if 0
	if(!main_dlights)return;

	glDisable(GL_TEXTURE_2D);

	glBegin(GL_POLYGON);
	for(i=0; i<num; i++)
	{
		r=0; g=0; b=0;

		for(j=0; j<main_dlights; j++)
		{
			d=V3F_DIST(xyz+i*3, main_dlight_org+j*3);
			f=((1.0/255.0)*main_light_val[j*2+0])/(d+1);
			r+=main_light_clr[j*3+0]*f;
			g+=main_light_clr[j*3+1]*f;
			b+=main_light_clr[j*3+2]*f;
		}

		glColor4f(r, g, b, 1.0);
//		glTexCoord2fv(st+i*2);
		glVertex3fv(xyz+i*3);
	}
	glEnd();

	glEnable(GL_TEXTURE_2D);
	glColor4f(1, 1, 1, 1);
#endif
}

void BSP_DrawFace41(BSP_Face *face)
{
	float pt[3], pt1[3];
	float f, d, v;
	int i, j;
	float r, g, b;
	float a, x, y;

	if(!main_dlights)return;

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
//	glDepthFunc(GL_GREATER);
	glEnable(GL_STENCIL_TEST);

	r=0; g=0; b=0;
	for(i=0; i<main_dlights; i++)
	{
		V3F_COPY(main_dlight_org+i*3, pt);
		f=V3F_NDOT(pt, face->norm);
//		if(f<0)continue;

		V3F_ADDSCALE(pt, face->norm, -f-0.01, pt);

		v=main_light_val[i*2+0];
		d=V3F_DIST(pt, main_dlight_org+i*3);
		f=(6*v/255.0)/(d*d+1);
		if(f>1)f=1;

//		f=1;
//		if(f<0.1)continue;

		glBegin(GL_TRIANGLE_FAN);
		glColor4f(f, f, f, 1.0);
		glVertex3fv(pt);

#if 1
		glColor4f(0.1, 0.1, 0.1, 1.0);
		for(j=0; j<9; j++)
		{
			a=(1.0/4.0*M_PI)*j;
			x=sin(a)*(f/0.1); y=cos(a)*(f/0.1);

			pt1[0]=pt[0] + face->sdir[0]*x + face->tdir[0]*y;
			pt1[1]=pt[1] + face->sdir[1]*x + face->tdir[1]*y;
			pt1[2]=pt[2] + face->sdir[2]*x + face->tdir[2]*y;
			glVertex3fv(pt1);
		}
		glEnd();
#endif
	}

	glDisable(GL_STENCIL_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_TEXTURE_2D);
	glColor4f(1, 1, 1, 1);
}
#endif


void BSP_DrawFace42(LBXGL_World *wrl, BSP_Face *face, BSP_Node *node)
{
	float f, h, d;
	int i, j;
	float r, g, b;

//	if(!face->rgb)
//		return;
	if(!face->lm)
		return;
	if(!face->lm->vec_rgb)
		return;

	glDisable(GL_TEXTURE_2D);

	glBegin(GL_POLYGON);
	for(i=0; i<face->nvecs; i++)
	{
//		r=face->rgb[i*3+0]/255.0;
//		g=face->rgb[i*3+1]/255.0;
//		b=face->rgb[i*3+2]/255.0;

		j=face->vidx[i];
		r=face->lm->vec_rgb[j*3+0]/255.0;
		g=face->lm->vec_rgb[j*3+1]/255.0;
		b=face->lm->vec_rgb[j*3+2]/255.0;

		glColor4f(r, g, b, 1.0);
		glVertex3fv(face->vecs+i*3);
	}
	glEnd();

	glEnable(GL_TEXTURE_2D);
	glColor4f(1, 1, 1, 1);
}

void BSP_DrawFace(LBXGL_World *wrl, BSP_Model *mdl,
	BSP_Face *face, BSP_Node *node)
{
	float nv[3], corg[3];
	BSP_LM *lcur;
	float s, t, f;
	int i, fl, xs, ys;

//	return;

	fl=mdl->tex_fl[face->num];
	xs=mdl->tex_xs[face->num];
	ys=mdl->tex_ys[face->num];

	if(face->flags&BSPFACE_FL_PORTAL)
	{
		glDepthFunc(GL_LEQUAL);
		glDisable(GL_LIGHTING);
		glDisable(GL_CULL_FACE);

		BSP_DrawFace22(face->vecs, face->nvecs);
		return;
	}

	if(face->flags&BSPFACE_FL_BACKSIDE)
		return;


	if(fl&LBXGL_TXFL_NODRAW)
		return;

	//cull non-window backfaces
	LBXGL_GetCamOriginF(corg);
	if((V3F_NDOT(corg, face->norm)<=0) &&
		!(fl&LBXGL_TXFL_WINDOW))
			return;

	if(!face->lm && (face->lmnum!=0xFFFF))
	{
		lcur=mdl->lmroot;
		while(lcur)
		{
			if(lcur->num==face->lmnum)
			{
				face->lm=lcur;
				break;
			}
			lcur=lcur->next;
		}

		if(!face->lm)face->lmnum=0xFFFF;
	}

#if 0
	V3F_ZERO(corg);
	for(i=0; i<face->nvecs; i++)
		{ V3F_ADD(corg, face->vecs+i*3, corg); }
	V3F_SCALE(corg, 1.0/face->nvecs, corg);

	V3F_ADD(corg, face->norm, nv);

	glDisable(GL_TEXTURE_2D);
	glColor4f(1, 0, 0, 1);

	glBegin(GL_LINES);
	glVertex3fv(corg);
	glVertex3fv(nv);
	glEnd();
#endif

	glDepthFunc(GL_LEQUAL);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glDisable(GL_ALPHA_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1, 1, 1, 1);

	glEnable(GL_DEPTH_TEST);


	if(fl&LBXGL_TXFL_WINDOW)
	{
		if(mdl->tex_num[face->num]>=0)
		{
			glEnable(GL_TEXTURE_2D);
			Draw_Bind(mdl->tex_num[face->num]);

//			BSP_DrawFace2(face->vecs, face->st, face->nvecs,
//				fl, xs, ys);

			BSP_DrawFace23(wrl, face,
				face->vecs, face->st, face->nvecs,
				fl, xs, ys);
		}else
		{
//			glDisable(GL_TEXTURE_2D);
//			BSP_DrawFace21(face->vecs, face->nvecs);
		}

#if 0
		if(face->lm && (face->lm->texnum>=0))
		{
			glDisable(GL_DEPTH_TEST);

//			glBlendFunc(GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA);
			glBlendFunc(GL_ZERO, GL_SRC_COLOR);
			Draw_Bind(face->lm->texnum);
			BSP_DrawFace3(face->vecs, face->uv, face->nvecs,
				fl, xs, ys);

#if 1
			for(i=0; i<face->lm->dn; i++)
			{
				f=wrl->style_gamma[face->lm->dlmstyle[i]];
				glColor4f(f, f, f, 1);
				Draw_Bind(face->lm->dlmnum[i]);
				BSP_DrawFace3(face->vecs, face->uv, face->nvecs,
					fl, xs, ys);
			}
#endif

//			glColor4f(1, 1, 1, 1);
//			BSP_DrawFace42(wrl, face, node);
		}
#endif

		return;
	}

#if 0
//	Draw_Bind(mdl->tex_num[face->num]);
//	BSP_DrawFace2(face->vecs, face->st, face->nvecs, fl, xs, ys);

	LBXGL_Shader_Color4f(1, 1, 1, 1);

	LBXGL_Shader_BindTexture(mdl->tex_num[face->num]);
	LBXGL_Shader_BeginPoly();
	for(i=0; i<face->nvecs; i++)
	{
//		LBXGL_Shader_Normal3fv(face->nvecs+i*3);
		LBXGL_Shader_TexCoord2fv(face->st+i*2);
		LBXGL_Shader_Vertex3fv(face->vecs+i*3);
	}
	LBXGL_Shader_EndPoly();

#endif

//	BSP_DrawFace21(face->vecs, face->nvecs);
//	return;
	

	V3F_SCALE(face->norm, -1, nv);
	glNormal3fv(nv);


#if 1
	if(face->lm && (face->lm->texnum>=0))
	{
//		if(fl&LBXGL_TXFL_WINDOW)
		if(0)
		{
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);

			glBlendFunc(GL_ONE, GL_ZERO);
			Draw_Bind(mdl->tex_num[face->num]);
			BSP_DrawFace2(face->vecs, face->st, face->nvecs,
				fl, xs, ys);

			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);

			glBlendFunc(GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA);
			Draw_Bind(face->lm->texnum);
			BSP_DrawFace3(face->vecs, face->uv, face->nvecs,
				fl, xs, ys);

#if 1
			for(i=0; i<face->lm->dn; i++)
			{
				f=wrl->style_gamma[face->lm->dlmstyle[i]];
				glColor4f(f, f, f, 1);
				Draw_Bind(face->lm->dlmnum[i]);
				BSP_DrawFace3(face->vecs, face->uv, face->nvecs,
					fl, xs, ys);
			}
#endif

			glColor4f(1, 1, 1, 1);

			BSP_DrawFace42(wrl, face, node);

			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		}else
		{
//			glGetIntegerv(GL_STENCIL_BITS, &i);
//			if(i>16)i=16;
//			i=(1<<i)-1;

			glBlendFunc(GL_ONE, GL_ZERO);
			Draw_Bind(face->lm->texnum);
			BSP_DrawFace3(face->vecs, face->uv, face->nvecs,
				fl, xs, ys);

			glBlendFunc(GL_ONE, GL_ONE);

#if 1
			for(i=0; i<face->lm->dn; i++)
			{
				f=wrl->style_gamma[face->lm->dlmstyle[i]];
				glColor4f(f, f, f, 1);
				Draw_Bind(face->lm->dlmnum[i]);
				BSP_DrawFace3(face->vecs, face->uv, face->nvecs,
					fl, xs, ys);
			}
#endif

			glColor4f(1, 1, 1, 1);
			BSP_DrawFace42(wrl, face, node);
		}
	}else
	{
//		if(fl&LBXGL_TXFL_WINDOW)
		if(0)
		{
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);

			glBlendFunc(GL_ONE, GL_ZERO);
			Draw_Bind(mdl->tex_num[face->num]);
			BSP_DrawFace2(face->vecs, face->st, face->nvecs,
				fl, xs, ys);

			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

			glBlendFunc(GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA);
			BSP_DrawFace21(face->vecs, face->nvecs);
		}else
		{
			glDisable(GL_LIGHTING);
			glColor4f(1, 1, 1, 1);

			glBlendFunc(GL_ONE, GL_ZERO);
			BSP_DrawFace21(face->vecs, face->nvecs);
		}
	}
#endif

	glEnable(GL_DEPTH_TEST);

	if(mdl->tex_num[face->num]>=0)
	{
		glEnable(GL_TEXTURE_2D);
		glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);
		Draw_Bind(mdl->tex_num[face->num]);
		BSP_DrawFace2(face->vecs, face->st, face->nvecs, fl, xs, ys);
	}else
	{
//		glDisable(GL_TEXTURE_2D);
//		glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);
//		BSP_DrawFace21(face->vecs, face->nvecs);
	}

#if 1
	if(mdl->tex_num2[face->num]>=0)
	{
		glBlendFunc(GL_SRC_COLOR, GL_ONE);
		Draw_Bind(mdl->tex_num2[face->num]);
		BSP_DrawFace2(face->vecs, face->st, face->nvecs,
			fl, xs, ys);
	}
#endif

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
}

void BSP_DrawFaces(LBXGL_World *wrl, BSP_Model *mdl,
	BSP_Face *face, BSP_Node *node)
{
	BSP_Face *cur;
	cur=face;
	while(cur)
	{
		BSP_DrawFace(wrl, mdl, cur, node);
		cur=cur->next;
	}
}

void BSP_DrawNodeFaces(LBXGL_World *wrl, BSP_Model *mdl,
	BSP_Face *face, BSP_Node *node)
{
	BSP_Face *cur;
	cur=face;
	while(cur)
	{
		BSP_DrawFace(wrl, mdl, cur, cur->leaf);
		cur=cur->nnext;
	}
}

#if 0
LBXGL_Light *BSP_Draw_GetLights(LBXGL_World *wrl,
	BSP_Model *mdl, BSP_Node *node)
{
	LBXGL_Light *lfst, *lcur;
	float f, d;

	lfst=NULL;
	lcur=wrl->dlights;
	while(lcur)
	{
		f=V3F_DIST(lcur->org, node->org);
		if(f<=node->radius)
		{
			lcur->chain=lfst;
			lfst=lcur;

			lcur=lcur->next;
			continue;
		}

		d=(f-node->radius)/36.0;
		f=((6.0/255.0)*lcur->val[0])/(d*d+1);


		if(f>=0.1125)
		{
			lcur->chain=lfst;
			lfst=lcur;

			lcur=lcur->next;
			continue;
		}

		lcur=lcur->next;
	}

	return(lfst);
}
#endif

void BSP_DrawNode(LBXGL_World *wrl, BSP_Model *mdl, BSP_Node *node)
{
//	LBXGL_Light *dl;
	BSP_Face *cur;
	float corg[3];
	float f;

	if(!node->lnode)
	{
//		BSP_DrawFaces(wrl, mdl, node->face, node);
		BSP_DrawEnts(wrl, node->ents);
		return;
	}

	LBXGL_GetCamOriginF(corg);
	f=Vec3F_DotProduct(corg, node->norm)-node->norm[3];
	if(f<0)
	{
		BSP_DrawNode(wrl, mdl, node->rnode);

//		dl=BSP_Draw_GetLights(wrl, mdl, node);

		BSP_DrawNodeFaces(wrl, mdl, node->face, node);
		BSP_DrawEnts(wrl, node->ents);

//		BSP_DrawFaces(wrl, mdl, node->prt, node);
//		BSP_DrawFaces(wrl, mdl, node->tsurf, node);

		BSP_DrawNode(wrl, mdl, node->lnode);
	}else
	{
		BSP_DrawNode(wrl, mdl, node->lnode);

//		dl=BSP_Draw_GetLights(wrl, mdl, node);

		BSP_DrawNodeFaces(wrl, mdl, node->face, node);
		BSP_DrawEnts(wrl, node->ents);

//		BSP_DrawFaces(wrl, mdl, node->prt, node);
//		BSP_DrawFaces(wrl, mdl, node->tsurf, node);

		BSP_DrawNode(wrl, mdl, node->rnode);
	}
}

void BSP_DrawModel(LBXGL_World *wrl, BSP_Model *mdl)
{
	float *org;

//	glMatrixMode(GL_MODELVIEW);
//	glPushMatrix();

//	org=mdl->ent->base->org;
//	glTranslatef(org[0], org[1], org[2]);

	BSP_DrawNode(wrl, mdl, mdl->root);

//	glPopMatrix();
}
