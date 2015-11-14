#include <lbxgl2.h>


void BSP_SendFace(BSP_Face *face)
{
	float tv[4];
	int i;

	bsdeBegin(BSDE_BSP_FACE);

//	V4F_SCALE(face->norm, -1, tv);
//	bsdeNormal4fv(tv);

	bsdeNormal4fv(face->norm);
	for(i=0; i<face->nvecs; i++)
		bsdeVertex3fv(face->vecs+i*3);
	bsdeEnd();
}

void BSP_SendFaces(BSP_Face *face)
{
	BSP_Face *cur;
	cur=face;
	while(cur)
	{
		BSP_SendFace(cur);
		cur=cur->next;
	}
}

void BSP_SendNode(BSP_Node *node)
{
	BSP_Face *cur;
	float corg[3];
	float f;

	if(!node->lnode)
	{
		bsdeBegin(BSDE_BSP_LEAF);
		BSP_SendFaces(node->tsurf);
		bsdeEnd();
		return;
	}

	bsdeBegin(BSDE_BSP_NODE);
	bsdeNormal4fv(node->norm);
	BSP_SendNode(node->lnode);
	BSP_SendNode(node->rnode);
	bsdeEnd();
}

int BSP_SendModel(LBXGL_World *wrl, BSP_Model *mdl)
{
	int i;

	bsdeBegin(BSDE_SOLID_BSP);
	BSP_SendNode(mdl->root);
	i=bsdeEnd();
	mdl->solid=i;

	bsdeSetAttrF(i, BSDE_DENSITY, LBXGL_DENSITY_LB_IN3);
	bsdeSetAttrI(i, BSDE_STATIC, 1);

	return(i);
}
