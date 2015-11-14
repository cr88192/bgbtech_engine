#include <lbxgl2.h>

extern PDGLUI_InputState *lbxgl_state;
extern PDGLUI_Camera *lbxgl_cam;

int LBXGL_BrushModel_CheckBBoxFrustum(float *mins, float *maxs)
{
	float trorg[3], trdisp[3];
	float trmins[3], trmaxs[3];
	float org[3];

	return(1);
	
	V3F_SCALEADDSCALE(mins, 0.5, maxs, 0.5, org);

	LBXGL_Shader_TransformPoint(org, trorg);
	V3F_SUB(trorg, org, trdisp);
	V3F_ADD(mins, trdisp, trmins);
	V3F_ADD(maxs, trdisp, trmaxs);

	if(!LBXGL_Brush_CheckBBoxFrustum(trmins, trmaxs))
		return(0);		
	return(1);
}

int LBXGL_BrushModel_CheckNodeFrustum(LBXGL_BrushNode *node)
{
	if(!node->mid)
		return(0);

	if(!LBXGL_BrushModel_CheckBBoxFrustum(node->mins, node->maxs))
		return(0);
		
	return(1);
}

int LBXGL_BrushModel_CheckBrushFrustum(LBXGL_Brush *brush)
{
	if(!brush)
		return(0);

	if(!LBXGL_BrushModel_CheckBBoxFrustum(brush->mins, brush->maxs))
		return(0);
		
	return(1);
}

#if 0
// int LBXGL_BrushModel_CheckNodeFrustumVisible(
	LBXGL_BrushWorld *world, LBXGL_BrushNode *node)
{
	int i;
	
	if(!node->mid)
		return(0);

//	i=node->id;
//	i=world->autovis[i>>3]&(1<<(i&7));
//	if(!i)return(0);

	if(!LBXGL_BrushModel_CheckBBoxFrustum(node->mins, node->maxs))
		return(0);

	if(!LBXGL_BrushModel_CheckBBoxAntiportal(
		world, node->mins, node->maxs, lbxgl_cam->org))
			return(0);

	return(1);
}
#endif

LBXGL_API void LBXGL_BrushModel_DrawBrushDark(LBXGL_Brush *brush)
{
	float r, g, b, tr, tg, tb;
	float f;
	int i, j, k;

	if(brush->patch)
	{
		LBXGL_BrushPatch_DrawBrushDark(brush);
		return;
	}

	if(brush->flags&LBXGL_BRFL_HIDDEN)return;
	if(brush->flags&LBXGL_BRFL_ALPHA)return;

//	glDisable(GL_TEXTURE_2D);
	pdglColor4f(0, 0, 0, 1);

	k=0;
	for(i=0; i<brush->n_poly; i++)
	{
		k=brush->pf_flag[i];
		if(k&(LBXGL_TXFL_NODRAW|LBXGL_TXFL_ALPHA))
			continue;

		k=brush->pf_ivec[i];
		pdglBegin(PDGL_POLYGON);
		for(j=0; j<brush->pf_nvec[i]; j++)
		{
			pdglVertex3fv(brush->xyz+brush->vec[k*4+0]*3);
			k++;
		}
		pdglEnd();
	}
}

LBXGL_API void LBXGL_BrushModel_DrawBrushLight(LBXGL_Brush *brush,
	LBXGL_Light *light)
{
	float *xyz;
	short *vec;
	float f, g, d;
	int i, j, k, l;

	LBXGL_Brush_DrawBrushLight3(brush, light);
	return;

#if 0
	if(brush->patch)
	{
		LBXGL_BrushPatch_DrawBrushLight2(brush);
		return;
	}

	if(brush->flags&LBXGL_BRFL_HIDDEN)return;
	if(brush->flags&LBXGL_BRFL_ALPHA)return;

	xyz=brush->xyz; vec=brush->vec;
	for(i=0; i<brush->n_poly; i++)
	{
		if(brush->pf_flag[i]&LBXGL_TXFL_NODRAW)
			{ vec+=brush->pf_nvec[i]*4; continue; }
		glNormal3fv(brush->norm+brush->pf_plane[i]*4);
		pdglBegin(PDGL_POLYGON);
		l=brush->pf_nvec[i];
		for(j=0; j<l; j++)
		{
			pdglVertex3fv(xyz+(*vec)*3);
			vec+=4;
		}
		pdglEnd();
	}
#endif
}

LBXGL_API void LBXGL_BrushModel_DrawBrushShadow(LBXGL_Brush *brush)
{
	float r, g, b, tr, tg, tb;
	float f;
	int i, j, k;

	if(brush->patch)
	{
		LBXGL_BrushPatch_DrawFacesShadow(brush);
		return;
	}

	if(brush->flags&LBXGL_BRFL_HIDDEN)return;
	if(brush->flags&LBXGL_BRFL_ALPHA)return;

	for(i=0; i<brush->n_poly; i++)
	{
		k=brush->pf_flag[i];
		if(k&(LBXGL_TXFL_NODRAW|LBXGL_TXFL_ALPHA))
			continue;

		glBlendFunc(GL_DST_COLOR, GL_ZERO);
		LBXGL_Shader_BindTexture(brush->pf_tex[i]);
		LBXGL_Shader_Normal3fv(brush->norm+brush->pf_plane[i]*4);
		LBXGL_Shader_Color4f(1, 1, 1, 1);
		LBXGL_Shader_BeginPoly();
		for(j=0; j<brush->pf_nvec[i]; j++)
		{
			k=brush->pf_ivec[i]+j;
			LBXGL_Shader_TexCoord2fv(
				brush->st+brush->vec[k*4+1]*2);
			LBXGL_Shader_Vertex3fv(
				brush->xyz+brush->vec[k*4+0]*3);
		}
		LBXGL_Shader_EndPoly();
	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

#if 1
LBXGL_API void LBXGL_BrushModel_DrawShadowVolume(LBXGL_Brush *brush,
	float *org, float rad, int flags)
{
//	return;

	if(brush->patch)
	{
		LBXGL_BrushPatch_DrawShadowVolumeOrg(brush, org, rad);
		return;
	}

	if(brush->mesh)
	{
		LBXGL_BrushMesh_DrawShadowVolumeOrg(brush, org, rad);
		return;
	}

	if((flags&1) && LBXGL_Brush_CheckViewBrushShadow(brush, org, rad))
	{
		LBXGL_Brush_DrawShadowVolumeDPOrgClip(brush, NULL, org, rad);
	}else
	{
		LBXGL_Brush_DrawShadowVolumeDPOrg(brush, NULL, org, rad);
	}
}
#endif

#if 0
// LBXGL_API void LBXGL_BrushModel_DrawShadowVolume(LBXGL_Brush *brush,
	float *org, float rad)
{
//	static float xyza[256*3];
//	static float xyzb[256*3];
	float tv0[3], tv1[3], tv2[3], tv3[3];
	float dv0[3], dv1[3];
	float *nv, *nv0, *nv1, *tv, *xyz;
	float f, g, h;
	int i, j, k, l, i0, i1;

	if(brush->patch)
	{
		LBXGL_BrushPatch_DrawShadowVolumeOrg(brush, org, rad);
		return;
	}

	if(brush->mesh)
	{
		LBXGL_BrushMesh_DrawShadowVolumeOrg(brush, org, rad);
		return;
	}

	if(brush->flags&LBXGL_BRFL_HIDDEN)return;
	if(brush->flags&LBXGL_BRFL_ALPHA)return;
	if(brush->flags&LBXGL_BRFL_EFFECT)return;
	if(brush->flags&LBXGL_BRFL_CHKDRAW)return;

	xyz=brush->xyz;
	for(i=0; i<brush->n_xyz; i++)
	{
#if 0
		V3F_COPY(xyz+i*3, tv0);
		V3F_SUB(tv0, org, dv0);
		V3F_NORMALIZE(dv0, dv0);
		V3F_ADDSCALE(tv0, dv0, 1, xyza+i*3);
		V3F_ADDSCALE(tv0, dv0, 2*rad, xyzb+i*3);
#endif

#if 1
		V3F_COPY(xyz+i*3, tv0);
		V3F_SCALEADDSCALE(tv0, 0.99, brush->org, 0.01, tv0);
		V3F_SUB(tv0, org, dv0);
//		V3F_NORMALIZE(dv0, dv0);
		V3F_FRNORMALIZE(dv0, dv0);
		V3F_ADD(tv0, lbxgl_cam->fw, tv0);
		V3F_ADDSCALE(tv0, dv0, 2, xyza+i*3);
		V3F_ADDSCALE(tv0, dv0, 2*rad, xyzb+i*3);
#endif

	}

	pdglBegin(PDGL_QUADS);
	for(i=0; i<brush->n_edge; i++)
	{
		i0=brush->edge[i*4+2]; i1=brush->edge[i*4+3];
		nv0=brush->norm+brush->pf_plane[i0]*4;
		nv1=brush->norm+brush->pf_plane[i1]*4;

		f=V3F_NDOT(org, nv0);
		g=V3F_NDOT(org, nv1);

		if((f>0) && (g<=0))
		{
			i0=brush->edge[i*4+0]; i1=brush->edge[i*4+1];
			pdglVertex3fv(xyza+i1*3); pdglVertex3fv(xyza+i0*3);
			pdglVertex3fv(xyzb+i0*3); pdglVertex3fv(xyzb+i1*3);
		}

		if((g>0) && (f<=0))
		{
			i0=brush->edge[i*4+0]; i1=brush->edge[i*4+1];
			pdglVertex3fv(xyza+i0*3); pdglVertex3fv(xyza+i1*3);
			pdglVertex3fv(xyzb+i1*3); pdglVertex3fv(xyzb+i0*3);
		}
	}
	pdglEnd();

	k=0;
	for(i=0; i<brush->n_poly; i++)
	{
		nv=brush->norm+brush->pf_plane[i]*4;
		f=V3F_NDOT(org, nv);
		if(f>0)xyz=xyza; else xyz=xyzb;

		k=brush->pf_ivec[i];
		pdglBegin(PDGL_POLYGON);
		for(j=0; j<brush->pf_nvec[i]; j++)
			{ pdglVertex3fv(xyz+brush->vec[k*4+0]*3); k++; }
		pdglEnd();
	}
}
#endif

LBXGL_API void LBXGL_BrushModel_DrawNodeBrushesMidDark(LBXGL_BrushNode *node)
{
	LBXGL_Brush *cur;

//	if(!LBXGL_BrushModel_CheckNodeFrustumVisible(world, node))
//		return;

	if(!LBXGL_BrushModel_CheckNodeFrustum(node))
		return;

	cur=node->mid;
	while(cur)
	{
		if(cur->flags&LBXGL_BRFL_EFFECT)
			{ cur=cur->nnext; continue; }
		LBXGL_BrushModel_DrawBrushDark(cur);
		cur=cur->nnext;
	}
}

LBXGL_API void LBXGL_BrushModel_DrawNodeBrushesDark(LBXGL_BrushNode *node)
{
	float f;

	if(!node)return;

	f=V3F_NDOT(lbxgl_cam->org, node->norm);
	if(f<0)
	{
		LBXGL_BrushModel_DrawNodeBrushesDark(node->lnode);
		LBXGL_BrushModel_DrawNodeBrushesMidDark(node);
		LBXGL_BrushModel_DrawNodeBrushesDark(node->rnode);
	}else
	{
		LBXGL_BrushModel_DrawNodeBrushesDark(node->rnode);
		LBXGL_BrushModel_DrawNodeBrushesMidDark(node);
		LBXGL_BrushModel_DrawNodeBrushesDark(node->lnode);
	}
}

LBXGL_API void LBXGL_BrushModel_DrawNodeBrushesMidLight(
	LBXGL_BrushNode *node, LBXGL_Light *light,
	float *lorg, float rad)
{
	LBXGL_Brush *cur;

	if(!LBXGL_BrushModel_CheckNodeFrustum(node))
		return;

	cur=node->mid;
	while(cur)
	{
		if(cur->flags&LBXGL_BRFL_EFFECT)
			{ cur=cur->nnext; continue; }
		if(!LBXGL_Brush_CheckSphereBrush(cur, lorg, 2*rad))
			{ cur=cur->nnext; continue; }
		LBXGL_BrushModel_DrawBrushLight(cur, light);
		cur=cur->nnext;
	}
}

LBXGL_API void LBXGL_BrushModel_DrawNodeBrushesLight(
	LBXGL_BrushNode *node, LBXGL_Light *light,
	float *lorg, float rad)
{
	float f;

	if(!node)return;

	f=V3F_NDOT(lorg, node->norm);

#if 1
	if(f<(-rad))
	{
		LBXGL_BrushModel_DrawNodeBrushesLight(node->lnode, light, lorg, rad);
		LBXGL_BrushModel_DrawNodeBrushesMidLight(node, light, lorg, rad);
		return;
	}

	if(f>rad)
	{
		LBXGL_BrushModel_DrawNodeBrushesLight(node->rnode, light, lorg, rad);
		LBXGL_BrushModel_DrawNodeBrushesMidLight(node, light, lorg, rad);
		return;
	}
#endif

	f=V3F_NDOT(lbxgl_cam->org, node->norm);
	if(f<0)
	{
		LBXGL_BrushModel_DrawNodeBrushesLight(node->lnode, light, lorg, rad);
		LBXGL_BrushModel_DrawNodeBrushesMidLight(node, light, lorg, rad);
		LBXGL_BrushModel_DrawNodeBrushesLight(node->rnode, light, lorg, rad);
	}else
	{
		LBXGL_BrushModel_DrawNodeBrushesLight(node->rnode, light, lorg, rad);
		LBXGL_BrushModel_DrawNodeBrushesMidLight(node, light, lorg, rad);
		LBXGL_BrushModel_DrawNodeBrushesLight(node->lnode, light, lorg, rad);
	}
}

LBXGL_API void LBXGL_BrushModel_DrawNodeBrushesMidShadow(
	LBXGL_BrushNode *node,
	float *lorg, float rad, int flags)
{
	LBXGL_Brush *cur;

//	if(!LBXGL_BrushModel_CheckNodeFrustum(node))
//		return;

	cur=node->mid;
	while(cur)
	{
		if(cur->flags&LBXGL_BRFL_EFFECT)
			{ cur=cur->nnext; continue; }
		if(!LBXGL_Brush_CheckSphereBrush(cur, lorg, rad))
			{ cur=cur->nnext; continue; }
		LBXGL_BrushModel_DrawShadowVolume(cur, lorg, rad, flags);
		cur=cur->nnext;
	}
}

LBXGL_API void LBXGL_BrushModel_DrawNodeBrushesShadow(
	LBXGL_BrushNode *node,
	float *lorg, float rad, int flags)
{
	float f;

	if(!node)return;

	f=V3F_NDOT(lorg, node->norm);

	if(f<(-rad))
	{
		LBXGL_BrushModel_DrawNodeBrushesShadow(node->lnode, lorg, rad, flags);
		LBXGL_BrushModel_DrawNodeBrushesMidShadow(node, lorg, rad, flags);
		return;
	}

	if(f>rad)
	{
		LBXGL_BrushModel_DrawNodeBrushesShadow(node->rnode, lorg, rad, flags);
		LBXGL_BrushModel_DrawNodeBrushesMidShadow(node, lorg, rad, flags);
		return;
	}

	f=V3F_NDOT(lbxgl_cam->org, node->norm);
	if(f<0)
	{
		LBXGL_BrushModel_DrawNodeBrushesShadow(node->lnode, lorg, rad, flags);
		LBXGL_BrushModel_DrawNodeBrushesMidShadow(node, lorg, rad, flags);
		LBXGL_BrushModel_DrawNodeBrushesShadow(node->rnode, lorg, rad, flags);
	}else
	{
		LBXGL_BrushModel_DrawNodeBrushesShadow(node->rnode, lorg, rad, flags);
		LBXGL_BrushModel_DrawNodeBrushesMidShadow(node, lorg, rad, flags);
		LBXGL_BrushModel_DrawNodeBrushesShadow(node->lnode, lorg, rad, flags);
	}
}

LBXGL_API void LBXGL_BrushModel_DrawNodeBrushesMidFinal(
	LBXGL_BrushNode *node)
{
	LBXGL_Brush *cur;

	if(!LBXGL_BrushModel_CheckNodeFrustum(node))
		return;

	cur=node->mid;
	while(cur)
	{
		if(cur->flags&LBXGL_BRFL_EFFECT)
			{ cur=cur->nnext; continue; }
		LBXGL_BrushModel_DrawBrushShadow(cur);
		cur=cur->nnext;
	}
}

LBXGL_API void LBXGL_BrushModel_DrawNodeBrushesFinal(
	LBXGL_BrushNode *node)
{
	float f;

	if(!node)return;

	f=V3F_NDOT(lbxgl_cam->org, node->norm);
	if(f<0)
	{
		LBXGL_BrushModel_DrawNodeBrushesFinal(node->lnode);
		LBXGL_BrushModel_DrawNodeBrushesMidFinal(node);
		LBXGL_BrushModel_DrawNodeBrushesFinal(node->rnode);
	}else
	{
		LBXGL_BrushModel_DrawNodeBrushesFinal(node->rnode);
		LBXGL_BrushModel_DrawNodeBrushesMidFinal(node);
		LBXGL_BrushModel_DrawNodeBrushesFinal(node->lnode);
	}
}

LBXGL_API void LBXGL_BrushModel_DrawNodeBrushesMidFlat(
	LBXGL_BrushNode *node)
{
	LBXGL_Brush *cur;

	if(!LBXGL_BrushModel_CheckNodeFrustum(node))
		return;

	cur=node->mid;
	while(cur) { LBXGL_Brush_DrawSolidFlat(cur); cur=cur->nnext; }
}

LBXGL_API void LBXGL_BrushModel_DrawNodeBrushesFlat(
	LBXGL_BrushNode *node)
{
	float f;

	if(!node)return;

	f=V3F_NDOT(lbxgl_cam->org, node->norm);
	if(f<0)
	{
		LBXGL_BrushModel_DrawNodeBrushesFlat(node->lnode);
		LBXGL_BrushModel_DrawNodeBrushesMidFlat(node);
		LBXGL_BrushModel_DrawNodeBrushesFlat(node->rnode);
	}else
	{
		LBXGL_BrushModel_DrawNodeBrushesFlat(node->rnode);
		LBXGL_BrushModel_DrawNodeBrushesMidFlat(node);
		LBXGL_BrushModel_DrawNodeBrushesFlat(node->lnode);
	}
}

LBXGL_API LBXGL_BrushModel *LBXGL_BrushBSP_BuildBrushModelListFast(
	LBXGL_Brush *lst)
{
	LBXGL_BrushModel *tmp;
	LBXGL_Brush *cur;

	tmp=gctalloc("lbxgl_brushmodel_t", sizeof(LBXGL_BrushModel));

#if 0
	cur=lst;
	while(cur && cur->patch)
		cur=cur->chain;
	if(!cur && lst->se_owner)
	{
		LBXGL_SEnt_GetVec3(lst->se_owner, "origin", tmp->mdl_org);
	}
#endif

	cur=lst;
	while(cur)
	{
		if(!cur->xyz)LBXGL_Brush_BuildFaces(cur);

		if((cur->flags&LBXGL_TXFL_EFFECT_MASK)==LBXGL_TXFL_EFFECT_ORIGIN)
		{
			V3F_COPY(cur->org, tmp->mdl_org);

			printf("LBXGL_BrushBSP_BuildBrushModelListFast: "
				"%p Origin=(%f %f %f)\n", tmp,
				tmp->mdl_org[0], tmp->mdl_org[1], tmp->mdl_org[2]);
		}

		cur->model=tmp;
		cur->mnext=tmp->brush;
		tmp->brush=cur;
		cur=cur->chain;
	}

	LBXGL_BrushBSP_SphereList(lst, tmp->com_org, &(tmp->com_rad));

	tmp->bsp=LBXGL_BrushBSP_BuildTreeListFast(lst);
	return(tmp);
}

LBXGL_API void LBXGL_BrushBSP_FreeBrushModel(LBXGL_BrushModel *bmdl)
{
	LBXGL_Brush *cur;

	LBXGL_BrushBSP_FreeTreeNode(bmdl->bsp);

	cur=bmdl->brush;
	while(cur)
	{
		cur->model=NULL;
		cur=cur->mnext;
	}

	gcfree(bmdl);
}

LBXGL_API void LBXGL_BrushModel_DrawBrushModel(LBXGL_BrushModel *bmdl)
{
	LBXGL_Brush *cur, *lst;

//	printf("LBXGL_BrushModel_DrawBrushModel: "
//		"%p Origin=(%f %f %f)\n", bmdl,
//		bmdl->mdl_org[0], bmdl->mdl_org[1], bmdl->mdl_org[2]);

//	glTranslatef(-bmdl->mdl_org[0], -bmdl->mdl_org[1], -bmdl->mdl_org[2]);
	LBXGL_Shader_Translatef(-bmdl->mdl_org[0], -bmdl->mdl_org[1], -bmdl->mdl_org[2]);

	cur=bmdl->brush;
	while(cur)
	{
		if(!(cur->flags&LBXGL_BRFL_ALPHA))
			LBXGL_Brush_DrawFaces(cur);
		cur=cur->mnext;
	}

	cur=bmdl->brush;
	while(cur)
	{
		if(cur->flags&LBXGL_BRFL_ALPHA)
			LBXGL_Brush_DrawFaces(cur);
		cur=cur->mnext;
	}
}

LBXGL_API void LBXGL_BrushModel_DrawBrushModelAlpha(LBXGL_BrushModel *bmdl)
{
	LBXGL_Brush *cur, *lst;

//	printf("LBXGL_BrushModel_DrawBrushModelAlpha: "
//		"%p Origin=(%f %f %f)\n", bmdl,
//		bmdl->mdl_org[0], bmdl->mdl_org[1], bmdl->mdl_org[2]);

//	glTranslatef(-bmdl->mdl_org[0], -bmdl->mdl_org[1], -bmdl->mdl_org[2]);
	LBXGL_Shader_Translatef(-bmdl->mdl_org[0], -bmdl->mdl_org[1], -bmdl->mdl_org[2]);

	cur=bmdl->brush;
	while(cur)
	{
		if(cur->flags&LBXGL_BRFL_ALPHA)
			LBXGL_Brush_DrawFaces(cur);
		cur=cur->mnext;
	}
}

LBXGL_API void LBXGL_BrushModel_DrawBrushModelDark(LBXGL_BrushModel *bmdl)
{
//	printf("LBXGL_BrushModel_DrawBrushModelDark\n");
//	glTranslatef(-bmdl->mdl_org[0], -bmdl->mdl_org[1], -bmdl->mdl_org[2]);
	LBXGL_Shader_Translatef(-bmdl->mdl_org[0], -bmdl->mdl_org[1], -bmdl->mdl_org[2]);
	LBXGL_BrushModel_DrawNodeBrushesDark(bmdl->bsp);
}

LBXGL_API void LBXGL_BrushModel_DrawBrushModelLight(LBXGL_BrushModel *bmdl,
	LBXGL_Light *light, float *org, float rad)
{
	float lorg[3], lorg1[3];
//	float rad;

//	LBXGL_Shader_TransformPointLocal(light->org, lorg);
//	V3F_ADD(lorg, bmdl->mdl_org, lorg1);
//	rad=LBXGL_Shadow_LightGammaRadius(light);

	V3F_ADD(org, bmdl->mdl_org, lorg1);

//	printf("LBXGL_BrushModel_DrawBrushModelLight\n");
//	glTranslatef(-bmdl->mdl_org[0], -bmdl->mdl_org[1], -bmdl->mdl_org[2]);
	LBXGL_Shader_Translatef(-bmdl->mdl_org[0], -bmdl->mdl_org[1], -bmdl->mdl_org[2]);
	LBXGL_BrushModel_DrawNodeBrushesLight(bmdl->bsp, light, lorg1, rad);
}

LBXGL_API void LBXGL_BrushModel_DrawBrushModelFinal(LBXGL_BrushModel *bmdl)
{
//	printf("LBXGL_BrushModel_DrawBrushModelFinal\n");
//	glTranslatef(-bmdl->mdl_org[0], -bmdl->mdl_org[1], -bmdl->mdl_org[2]);
	LBXGL_Shader_Translatef(-bmdl->mdl_org[0], -bmdl->mdl_org[1], -bmdl->mdl_org[2]);
	LBXGL_BrushModel_DrawNodeBrushesFinal(bmdl->bsp);
}

LBXGL_API void LBXGL_BrushModel_DrawBrushModelFlat(LBXGL_BrushModel *bmdl)
{
//	glTranslatef(-bmdl->mdl_org[0], -bmdl->mdl_org[1], -bmdl->mdl_org[2]);
	LBXGL_Shader_Translatef(-bmdl->mdl_org[0], -bmdl->mdl_org[1], -bmdl->mdl_org[2]);
	LBXGL_BrushModel_DrawNodeBrushesFlat(bmdl->bsp);
}


LBXGL_API void LBXGL_BrushModel_DrawBrushModelShadow(
	LBXGL_BrushModel *bmdl, float *org, float rad)
{
	float org1[3];
	LBXGL_Brush *cur, *lst;
	int fl;

//	return;

	fl=0;

#if 1
	if(LBXGL_Brush_CheckViewSphereShadow(
		bmdl->com_org, bmdl->com_rad, org, 2*rad))
	{
		fl|=1;
	}
#endif

//	glTranslatef(-bmdl->mdl_org[0], -bmdl->mdl_org[1], -bmdl->mdl_org[2]);
	LBXGL_Shader_Translatef(-bmdl->mdl_org[0], -bmdl->mdl_org[1], -bmdl->mdl_org[2]);
	V3F_ADD(org, bmdl->mdl_org, org1);

	LBXGL_BrushModel_DrawNodeBrushesShadow(bmdl->bsp, org1, rad, fl);

#if 0
	cur=bmdl->brush;
	while(cur)
	{
		if(!(cur->flags&LBXGL_BRFL_ALPHA))
		{
//			LBXGL_Brush_DrawShadowVolume2(cur, org1);
			LBXGL_BrushModel_DrawShadowVolume(cur, org1, rad);
		}
		cur=cur->mnext;
	}
#endif
}

LBXGL_API void LBXGL_BrushModel_BoxModel(LBXGL_BrushModel *bmdl,
	float *mins, float *maxs)
{
	LBXGL_Brush *cur, *lst;

	cur=bmdl->brush; lst=NULL;
	while(cur)
	{
		cur->chain=lst; lst=cur;
		cur=cur->mnext;
	}

	LBXGL_BrushBSP_BBoxList(lst, mins, maxs);

	V3F_SUB(mins, bmdl->mdl_org, mins);
	V3F_SUB(maxs, bmdl->mdl_org, maxs);
}

LBXGL_API void LBXGL_BrushModel_GetExtents(LBXGL_BrushModel *bmdl,
	float *dir, float *min, float *max)
{
	LBXGL_Brush *cur, *lst;
	float f, g, m, n;

	m=999999; n=-999999;
	cur=bmdl->brush;
	while(cur)
	{
		LBXGL_Brush_GetExtents(cur, dir, &f, &g);
		if(f<m)m=f; if(g>n)n=g;
		cur=cur->mnext;
	}

	*min=m; *max=n;
}

