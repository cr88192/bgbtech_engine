#include <lbxgl2.h>

void BSP_LightFace(LBXGL_World *wrl, BSP_Model *mdl,
	BSP_Face *face, BSP_Node *node, LBXGL_Light *dl)
{
	LBXGL_Light *cur;
	float f, h, d;
	int i, j, nc, fl;
	float r, g, b;

	if(!dl)return;

	if(face->flags&BSPFACE_FL_PORTAL)
		return;

	if(face->flags&BSPFACE_FL_BACKSIDE)
		return;

	fl=mdl->tex_fl[face->num];
//	xs=mdl->tex_xs[face->num];
//	ys=mdl->tex_ys[face->num];

	if(fl&LBXGL_TXFL_WINDOW)
		return;

	nc=0;
	if(!face->rgb)
	{
		face->rgb=kalloc(3*face->nvecs);
		nc=1;
	}

	cur=dl;
	while(cur)
	{
		if(cur->flags&LBXGL_LFL_CHANGED)nc=1;
		cur=cur->chain;
	}

	if(!nc)return;

	for(i=0; i<face->nvecs; i++)
	{
		r=0; g=0; b=0;

		cur=dl;
		while(cur)
		{
			if(V3F_NDOT(cur->org, face->norm)<=0)
			{
				cur=cur->chain;
				continue;
			}

#if 0
			j=cur->leaf;
			if(node->vis && !(node->vis[j>>3]&(1<<(j&7))))
			{
				cur=cur->chain;
				continue;
			}
#endif


			d=V3F_DIST(face->vecs+i*3, cur->org)/36.0;
//			f=((6.0/255.0)*cur->val[0])/(d*d+1);

			h=V3F_NDOT(cur->org, face->norm)/(d*36);
			f=((6.0/255.0)*cur->val[0]*h)/(d*d+1);

			if(f<0.1125)
			{
				cur=cur->chain;
				continue;
			}

#if 1
			if(cur->leaf!=node->num)
			{
				j=BSP_TraceNode(wrl->mdls->root,
					cur->org, face->vecs+i*3);
				if(j)
				{
					cur=cur->chain;
					continue;
				}
			}
#endif

			r+=cur->clr[0]*f;
			g+=cur->clr[1]*f;
			b+=cur->clr[2]*f;

			cur=cur->chain;
		}

		r=(r<0)?0:((r>1.0)?1.0:r);
		g=(g<0)?0:((g>1.0)?1.0:g);
		b=(b<0)?0:((b>1.0)?1.0:b);
		face->rgb[i*3+0]=r*255.0;
		face->rgb[i*3+1]=g*255.0;
		face->rgb[i*3+2]=b*255.0;
	}
}

void BSP_LightFaces(LBXGL_World *wrl, BSP_Model *mdl,
	BSP_Face *face, BSP_Node *node)
{
	BSP_Face *cur;
	cur=face;
	while(cur)
	{
		BSP_LightFace(wrl, mdl, cur, node, wrl->dlights);
		cur=cur->next;
	}
}

void BSP_LightNodeFaces(LBXGL_World *wrl, BSP_Model *mdl,
	BSP_Face *face, BSP_Node *node, LBXGL_Light *dl)
{
	BSP_Face *cur;
	cur=face;
	while(cur)
	{
		BSP_LightFace(wrl, mdl, cur, cur->leaf, dl);
		cur=cur->nnext;
	}
}

LBXGL_Light *BSP_Light_GetLights(LBXGL_World *wrl,
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

void BSP_LightNode(LBXGL_World *wrl, BSP_Model *mdl, BSP_Node *node)
{
	LBXGL_Light *dl;
	BSP_Face *cur;
	float corg[3];
	float f;

	if(!node->lnode)
	{
//		BSP_LightEnts(node->ents);
		return;
	}

	LBXGL_GetCamOriginF(corg);
	f=Vec3F_DotProduct(corg, node->norm)-node->norm[3];
	if(f<0)
	{
		BSP_LightNode(wrl, mdl, node->rnode);

		dl=BSP_Light_GetLights(wrl, mdl, node);

		BSP_LightNodeFaces(wrl, mdl, node->face, node, dl);
//		BSP_LightEnts(node->ents);

		BSP_LightNode(wrl, mdl, node->lnode);
	}else
	{
		BSP_LightNode(wrl, mdl, node->lnode);

		dl=BSP_Light_GetLights(wrl, mdl, node);

		BSP_LightNodeFaces(wrl, mdl, node->face, node, dl);
//		BSP_LightEnts(node->ents);

		BSP_LightNode(wrl, mdl, node->rnode);
	}
}

void BSP_LightVec(LBXGL_World *wrl, LBXGL_Light *dl,
	float *xyz, byte *rgb, int style)
{
	LBXGL_Light *cur;
	float f, h, d;
	int i, j, nc, fl;
	float r, g, b;

	r=0; g=0; b=0;

	cur=dl;
	while(cur)
	{
		if(cur->style!=style)
		{
			cur=cur->next;
			continue;
		}

		d=V3F_DIST(xyz, cur->org)/36.0;
		f=((6.0/255.0)*cur->val[0])/(d*d+1);

		if(f<0.125)
		{
			cur=cur->next;
			continue;
		}

#if 1
		j=BSP_TraceNode(wrl->mdls->root, cur->org, xyz);
		if(j)
		{
			cur=cur->next;
			continue;
		}
#endif

		r+=cur->clr[0]*f;
		g+=cur->clr[1]*f;
		b+=cur->clr[2]*f;

		cur=cur->next;
	}

	r=(r<0)?0:((r>1.0)?1.0:r);
	g=(g<0)?0:((g>1.0)?1.0:g);
	b=(b<0)?0:((b>1.0)?1.0:b);
	rgb[0]=r*255.0;
	rgb[1]=g*255.0;
	rgb[2]=b*255.0;
}

void BSP_LightVecs(LBXGL_World *wrl, LBXGL_Light *dl,
	float *xyz, byte *rgb, int nvec, int style)
{
	int i;

	for(i=0; i<nvec; i++)
		BSP_LightVec(wrl, dl, xyz+i*3, rgb+i*3, style);
}

void BSP_LightLMap(LBXGL_World *wrl, BSP_LM *lm, LBXGL_Light *dl)
{
	LBXGL_Light *cur, *lst;
	float f, h, d;
	int i, j, nc, fl;
	float r, g, b;

	if(!lm->vec_srgb)
	{
		lm->vec_srgb=kalloc(3*lm->num_vecs);
		BSP_LightVecs(wrl, wrl->lights, lm->vec_xyz,
			lm->vec_srgb, lm->num_vecs, -1);

		for(i=0; i<lm->dn; i++)
		{
			lm->vec_drgb[i]=kalloc(3*lm->num_vecs);
			j=lm->dlmstyle[i];
			BSP_LightVecs(wrl, wrl->lights, lm->vec_xyz,
				lm->vec_drgb[i], lm->num_vecs, j);
		}
	}

	nc=0;
	if(!lm->vec_rgb)
	{
		lm->vec_rgb=kalloc(3*lm->num_vecs);
		nc=1;
	}

	if(!dl)return;

	cur=dl;
	while(cur)
	{
		if(cur->flags&LBXGL_LFL_CHANGED)nc=1;
//		cur=cur->chain;
		cur=cur->next;
	}

	if(!nc)return;

	cur=dl; lst=NULL;
	while(cur)
	{
		d=V3F_NDOT(cur->org, lm->norm)/36.0;
		f=((6.0/255.0)*cur->val[0])/(d*d+1);

		if((d<=0) || (f<0.125))
		{
			cur=cur->next;
			continue;
		}

		cur->chain=lst;
		lst=cur;

		cur=cur->next;
	}


	for(i=0; i<lm->num_vecs; i++)
	{
		r=0; g=0; b=0;

//		cur=dl;
		cur=lst;
		while(cur)
		{
#if 0
			if(V3F_NDOT(cur->org, lm->norm)<=0)
			{
				cur=cur->next;
//				cur=cur->chain;
				continue;
			}
#endif

			d=V3F_DIST(lm->vec_xyz+i*3, cur->org)/36.0;
//			f=((6.0/255.0)*cur->val[0])/(d*d+1);

			h=V3F_NDOT(cur->org, lm->norm)/(d*36);
			f=((6.0/255.0)*cur->val[0]*h)/(d*d+1);

			if(f<0.125)
			{
//				cur=cur->next;
				cur=cur->chain;
				continue;
			}

#if 1
			j=BSP_TraceNode(wrl->mdls->root,
				cur->org, lm->vec_xyz+i*3);
			if(j)
			{
//				cur=cur->next;
				cur=cur->chain;
				continue;
			}
#endif

			r+=cur->clr[0]*f;
			g+=cur->clr[1]*f;
			b+=cur->clr[2]*f;

//			cur=cur->next;
			cur=cur->chain;
		}

		r=(r<0)?0:((r>1.0)?1.0:r);
		g=(g<0)?0:((g>1.0)?1.0:g);
		b=(b<0)?0:((b>1.0)?1.0:b);
		lm->vec_rgb[i*3+0]=r*255.0;
		lm->vec_rgb[i*3+1]=g*255.0;
		lm->vec_rgb[i*3+2]=b*255.0;
	}
}

void BSP_LightLMaps(LBXGL_World *wrl, BSP_Model *mdl)
{
	BSP_LM *cur;

	cur=mdl->lmroot;
	while(cur)
	{
		BSP_LightLMap(wrl, cur, wrl->dlights);
		cur=cur->next;
	}
}

void BSP_LightWorld(LBXGL_World *wrl)
{
	BSP_Model *mcur;
	LBXGL_Light *lcur;
	char *s;
	int i;

	mcur=wrl->mdls;
	while(mcur)
	{
//		BSP_LightNode(wrl, mcur, mcur->root);
		BSP_LightLMaps(wrl, mcur);
		mcur=mcur->next;
	}

	lcur=wrl->dlights;
	while(lcur)
	{
		lcur->flags&=~LBXGL_LFL_CHANGED;
		lcur=lcur->next;
	}

	for(i=0; i<wrl->num_styles; i++)
	{
		wrl->style_gamma[i]=0;
		if(wrl->style_flags[i]&1)
			wrl->style_gamma[i]=1.0;

		s=wrl->styles[i];
		while(*s && (*s!=':'))s++;
		if(*s==':')s++;
		if(!*s)s=wrl->styles[i];

		if(!strcmp(s, "Slow Pulse"))
			wrl->style_gamma[i]*=sin(wrl->time*M_PI)*0.5+0.5;
		if(!strcmp(s, "Slow Strobe"))
			wrl->style_gamma[i]*=((int)wrl->time)&1;
	}
}
