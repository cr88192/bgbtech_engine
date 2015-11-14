#include <lbxgl2.h>

#include <time.h>



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
		return;

	if(face->flags&BSPFACE_FL_BACKSIDE)
		return;

	//cull non-window backfaces
	LBXGL_GetCamOriginF(corg);
	if((V3F_NDOT(corg, face->norm)<=0) &&
		!(fl&LBXGL_TXFL_WINDOW))
			return;

	if(fl&LBXGL_TXFL_WINDOW)
		return;


	glDepthFunc(GL_LEQUAL);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
//	glDisable(GL_ALPHA_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1, 1, 1, 1);

	glEnable(GL_DEPTH_TEST);

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


//	BSP_DrawFace21(face->vecs, face->nvecs);
	return;

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


void BSP_DrawNode(LBXGL_World *wrl, BSP_Model *mdl, BSP_Node *node)
{
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

		BSP_DrawNodeFaces(wrl, mdl, node->face, node);
		BSP_DrawEnts(wrl, node->ents);

//		BSP_DrawFaces(wrl, mdl, node->prt, node);
//		BSP_DrawFaces(wrl, mdl, node->tsurf, node);

		BSP_DrawNode(wrl, mdl, node->lnode);
	}else
	{
		BSP_DrawNode(wrl, mdl, node->lnode);

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
