#include <lbxgl2.h>

//LBXGL_LightNode *lbxgl_light_noderoot;
LBXGL_LightNode *lbxgl_light_nodefree;

extern PDGLUI_InputState *lbxgl_state;
extern PDGLUI_Camera *lbxgl_cam;

LBXGL_API float LBXGL_Shadow_LightGammaPoint(LBXGL_Light *light, float *org, float rad)
{
	float f, d;

//	d=(V3F_DIST(light->org, org)-rad)/36.0;
//	if(d<0)d=0;
//	if(light->flags&LBXGL_LFL_LINEAR)
//		f=(6.0*light->val[0]/255.0)/(d+1);
//	else	f=(6.0*light->val[0]/255.0)/(d*d+1);

	d=(V3F_DIST(light->org, org)-rad);
	f=(light->curval-d)/255.0;
	if(f<0)f=0;

	return(f);
}

LBXGL_API float LBXGL_Shadow_LightGammaDist(LBXGL_Light *light, float dist)
{
	float f, d;

//	d=dist/36.0;
//	if(d<0)d=0;
//	if(light->flags&LBXGL_LFL_LINEAR)
//		f=(6.0*light->val[0]/255.0)/(d+1);
//	else	f=(6.0*light->val[0]/255.0)/(d*d+1);

	f=(light->curval-dist)/255.0;
	if(f<0)f=0;

	return(f);
}

LBXGL_API float LBXGL_Shadow_LightGammaRadius(LBXGL_Light *light)
{
	float f, d;

//	f=0.02;
//	if(light->flags&LBXGL_LFL_LINEAR)
//		d=(6.0*light->val[0])/f;
//	else	d=sqrt((6.0*light->val[0])/f);

	d=light->curval;

	return(d);
}

LBXGL_API float LBXGL_Shadow_LightGammaEffectRadius(LBXGL_Light *light, float gamma)
{
	float f, d;

	if(gamma<=0)gamma=0.001;

//	f=gamma;
//	if(light->flags&LBXGL_LFL_LINEAR)
//		d=(6.0*light->val[0]/255.0)/f;
//	else	d=sqrt((6.0*light->val[0]/255.0)/f);
//	return(d*36);

	d=light->curval-gamma*256;
	if(d<0)d=0;
	return(d);
}

#if 0
LBXGL_API void LBXGL_Shadow_DrawShadowsLight(dyt fst, LBXGL_Light *light)
{
	float org[3];
	dyt cur, lst, t;
	float f, g, d;

	lst=NULL; cur=fst;
	while(dyconsp(cur))
	{
		t=dycar(cur);
		LBXGL_GenericGetSphere(t, org, &g);
		f=LBXGL_Shadow_LightGammaPoint(light, org, g);
		if(f<0.1) { cur=dycdr(cur); continue; }
		gcSet(lst, dycons(t, lst));
		cur=dycdr(cur);
	}


	pdglDepthFunc(GL_LESS);

	pdglDepthMask(0);
	pdglColorMask(0, 0, 0, 0);

	pdglEnable(GL_CULL_FACE);
	pdglStencilFunc(GL_ALWAYS, 0, 255);


	pdglCullFace(GL_FRONT);
	pdglStencilOp(GL_KEEP, GL_INCR, GL_KEEP);

	cur=lst;
	while(dyconsp(cur))
	{
		LBXGL_GenericRenderShadow(dycar(cur), light->org);
		cur=dycdr(cur);
	}


	pdglCullFace(GL_BACK);
	pdglStencilOp(GL_KEEP, GL_DECR, GL_KEEP);

	cur=lst;
	while(dyconsp(cur))
	{
		LBXGL_GenericRenderShadow(dycar(cur), light->org);
		cur=dycdr(cur);
	}

	pdglDisable(GL_CULL_FACE);

	pdglDepthMask(1);
	pdglColorMask(1, 1, 1, 1);

	dyFreeList(lst);
}
#endif

#if 0
LBXGL_API float LBXGL_Shadow_SetupLight(LBXGL_Light *light)
{
	float pt[4];
	float f, g, d, gam;
	int i, j;

#if 1
#ifndef GLES
//	pdglDisable(GL_COLOR_MATERIAL);
	pdglDisable(GL_COLOR_MATERIAL);
#endif

	pt[0]=0; pt[1]=0; pt[2]=0; pt[3]=1;
	pdglMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, pt);
	pdglMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, pt);

	pt[0]=1; pt[1]=1; pt[2]=1; pt[3]=1;
	pdglMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, pt);

	pt[0]=0.2; pt[1]=0.2; pt[2]=0.2; pt[3]=0.2;
	pdglMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, pt);

	pt[0]=1;
	pdglMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, pt);


	pdglEnable(GL_LIGHTING);
	pdglEnable(GL_NORMALIZE);

	pdglLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	pdglShadeModel(GL_SMOOTH);

	V3F_ZERO(pt); pt[3]=1;
	pdglLightfv(GL_LIGHT0, GL_AMBIENT, pt);

	V3F_COPY(light->clr, pt); pt[3]=1;
	pdglLightfv(GL_LIGHT0, GL_DIFFUSE, pt);

	pt[0]=1; pt[1]=1; pt[2]=1; pt[3]=1;
	pdglLightfv(GL_LIGHT0, GL_SPECULAR, pt);

	V3F_COPY(light->org, pt); pt[3]=1;
	pdglLightfv(GL_LIGHT0, GL_POSITION, pt);


	gam=light->val[0]; f=lbxgl_state->time_f;

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
		pdglLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 1.0/f);
		pdglLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0);
		pdglLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.0);
	}else
	{
//		g=(36*36)*6.0/255.0;
		g=(36*36)*18.0/255.0;
		f=g*gam;
		pdglLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 1.0/f);
		pdglLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.0);
		pdglLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.0);
	}
	pdglEnable(GL_LIGHT0);
#endif

	return(light->val[0]);
}
#endif

#if 0
LBXGL_API void LBXGL_Shadow_DrawObjectsLight(dyt fst, LBXGL_Light *light)
{
	float org[3];
	dyt cur, t;
	float f, g, d;

	LBXGL_Shadow_SetupLight(light);

	pdglDisable(GL_TEXTURE_2D);
	pdglColor4f(1, 1, 1, 1);

	pdglDepthFunc(GL_LEQUAL);
	pdglStencilFunc(GL_EQUAL, 0, 255);
	pdglBlendFunc(GL_SRC_COLOR, GL_ONE);

	cur=fst;
	while(dyconsp(cur))
	{
		t=dycar(cur);
		LBXGL_GenericGetSphere(t, org, &g);
		f=LBXGL_Shadow_LightGammaPoint(light, org, g);
		if(f<0.1) { cur=dycdr(cur); continue; }
		LBXGL_GenericRenderLight(t);
		cur=dycdr(cur);
	}

	pdglDisable(GL_LIGHTING);
}

LBXGL_API void LBXGL_Shadow_DrawObjects(dyt fst, LBXGL_Light *lights)
{
	float tv0[3];
	dyt cur, t;
	LBXGL_Light *lcur;
	int i;

	pdglDepthFunc(GL_LEQUAL);

	cur=fst;
	while(dyconsp(cur))
	{
		t=dycar(cur);
		LBXGL_GenericRenderDark(t);
		cur=dycdr(cur);
	}

	lcur=lights;
	while(lcur)
	{
		pdglClear(GL_STENCIL_BUFFER_BIT);

		pdglEnable(GL_STENCIL_TEST);
		LBXGL_Shadow_DrawShadowsLight(fst, lcur);
		LBXGL_Shadow_DrawObjectsLight(fst, lcur);
		pdglDisable(GL_STENCIL_TEST);

		lcur=lcur->next;
	}

	pdglDepthFunc(GL_LEQUAL);

	pdglStencilFunc(GL_EQUAL, 0, 255);

	cur=fst;
	while(dyconsp(cur))
	{
		t=dycar(cur);
		pdglBlendFunc(GL_DST_COLOR, GL_ZERO);
		LBXGL_GenericRenderFinal(t);
		cur=dycdr(cur);
	}
}
#endif

LBXGL_API LBXGL_LightNode *LBXGL_Light_AllocNode()
{
	LBXGL_LightNode *tmp;
	if(lbxgl_light_nodefree)
	{
		tmp=lbxgl_light_nodefree;
		lbxgl_light_nodefree=tmp->lnode;

		tmp->lnode=NULL;
		tmp->rnode=NULL;
		tmp->mid=NULL;
		return(tmp);
	}

	tmp=gcalloc(sizeof(LBXGL_LightNode));
	return(tmp);
}

LBXGL_API void LBXGL_Light_FreeNode(LBXGL_LightNode *tmp)
{
	tmp->lnode=NULL;
	tmp->rnode=NULL;
	tmp->mid=NULL;
	tmp->lnode=lbxgl_light_nodefree;
	lbxgl_light_nodefree=tmp;
}

LBXGL_API void LBXGL_Light_FreeTreeNode(LBXGL_LightNode *tmp)
{
	if(!tmp)return;
	LBXGL_Light_FreeTreeNode(tmp->lnode);
	LBXGL_Light_FreeTreeNode(tmp->rnode);
	LBXGL_Light_FreeNode(tmp);
}

LBXGL_API LBXGL_LightNode *LBXGL_Light_BuildNodeList(LBXGL_Light *lst)
{
	float org[3], dir[4];
	LBXGL_LightNode *tmp;
	LBXGL_Light *cur, *nxt, *llst, *rlst, *mlst;
	float f, g;
	int i, j, k;

	tmp=LBXGL_Light_AllocNode();
	if(!lst || !lst->chain)
	{
		cur=lst;
		while(cur)
		{
			cur->nnext=tmp->mid;
			tmp->mid=cur;
			cur=cur->chain;
		}
		return(tmp);
	}

	cur=lst; V3F_ZERO(org); i=0;
	while(cur)
	{
		org[0]+=cur->org[0];
		org[1]+=cur->org[1];
		org[2]+=cur->org[2];
		i++; cur=cur->chain;
	}
	org[0]/=i; org[1]/=i; org[2]/=i;

	cur=lst; V3F_ZERO(dir);
	while(cur)
	{
		dir[0]+=fabs(cur->org[0]-org[0]);
		dir[1]+=fabs(cur->org[1]-org[0]);
		dir[2]+=fabs(cur->org[2]-org[0]);
		cur=cur->chain;
	}
	V3F_NORMALIZE(dir, dir); dir[3]=V3F_DOT(org, dir);

	V4F_COPY(dir, tmp->norm);

	cur=lst; i=0; j=0;
	while(cur)
	{
		f=V3F_NDOT(cur->org, dir);
		if(f==0) { k++; cur=cur->chain; continue; }

		g=LBXGL_Shadow_LightGammaDist(cur, f);
		if(g>0.1) { k++; cur=cur->chain; continue; }

		if(f<0)i++; if(f>0)j++;
		if(i && j) break;
		cur=cur->chain;
	}

	if(!i || !j)
	{
		cur=lst;
		while(cur)
		{
			cur->nnext=tmp->mid;
			tmp->mid=cur;
			cur=cur->chain;
		}
		return(tmp);
	}

	cur=lst; llst=NULL; rlst=NULL; mlst=NULL;
	while(cur)
	{
		nxt=cur->chain;
		f=V3F_NDOT(cur->org, dir);
		if(f==0) { cur->chain=mlst; mlst=cur; cur=nxt; continue; }

		g=LBXGL_Shadow_LightGammaDist(cur, f);
		if(g>0.1) { cur->chain=mlst; mlst=cur; cur=nxt; continue; }

		if(f<0) { cur->chain=llst; llst=cur; cur=nxt; continue; }
		if(f>0) { cur->chain=rlst; rlst=cur; cur=nxt; continue; }
		cur->chain=mlst; mlst=cur; cur=nxt; continue;
	}

	tmp->lnode=LBXGL_Light_BuildNodeList(llst);
	tmp->rnode=LBXGL_Light_BuildNodeList(rlst);

	cur=mlst;
	while(cur)
	{
		cur->nnext=tmp->mid;
		tmp->mid=cur;
		cur=cur->chain;
	}
	return(tmp);
}

#if 0
void LBXGL_Shadow_RebuildTree()
{
	LBXGL_Light *cur, *lst;
	float f;

	LBXGL_Shadow_FreeTreeNode(lbxgl_part_noderoot);
	lbxgl_part_noderoot=NULL;

	cur=lbxgl_part_root; lst=NULL;
	while(cur)
	{
		f=fabs(cur->psi_lin)+
			fabs(cur->psi_quad)+
			fabs(cur->psi_cube);
		if(f<0.1) { cur=cur->next; continue; }

		cur->chain=lst; lst=cur;
		cur=cur->next;
	}

	if(!lst)return;

	lbxgl_part_noderoot=LBXGL_Shadow_BuildNodeList(lst);
}
#endif


LBXGL_Light *LBXGL_Light_QueryTreeNodeR(LBXGL_LightNode *node,
	LBXGL_Light *llst, float *org, float rad)
{
	LBXGL_Light *lcur;
	float f;

	if(!node)return(llst);

	lcur=node->mid;
	while(lcur)
	{
		f=LBXGL_Shadow_LightGammaPoint(lcur, org, rad);
		if(f<0.1) { lcur=lcur->nnext; continue; }

		lcur->chain=llst;
		llst=lcur;

		lcur=lcur->nnext;
	}

	if(!node->lnode)
		return(llst);

	f=V3F_NDOT(org, node->norm);
	if(fabs(f)<rad)
	{
		llst=LBXGL_Light_QueryTreeNodeR(node->lnode, llst, org, rad);
		llst=LBXGL_Light_QueryTreeNodeR(node->rnode, llst, org, rad);
		return(llst);
	}

	if(f<0)
	{
		llst=LBXGL_Light_QueryTreeNodeR(node->lnode, llst, org, rad);
		return(llst);
	}

	if(f>0)
	{
		llst=LBXGL_Light_QueryTreeNodeR(node->rnode, llst, org, rad);
		return(llst);
	}

	return(llst);
}

LBXGL_API LBXGL_Light *LBXGL_Light_QueryTreeNode(LBXGL_LightNode *node,
	float *org, float rad)
{
	LBXGL_Light *llst;
	llst=LBXGL_Light_QueryTreeNodeR(node, NULL, org, rad);
	return(llst);
}

