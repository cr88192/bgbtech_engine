#include <btgesv.h>

BTGE_API BTGE_BrushModel *BTGE_BrushBSP_BuildBrushModelListFast(BTGE_Brush *lst)
{
	BTGE_BrushModel *tmp;
	BTGE_Brush *cur;
	int i, j;

	tmp=gctalloc("lbxgl_brushmodel_t", sizeof(BTGE_BrushModel));
	tmp->flags=0;

	cur=lst;
	while(cur)
	{
		if(!cur->xyz)BTGE_Brush_BuildFaces(cur);

		i=cur->flags&BTGE_TXFL_MATERIAL_MASK;

		if(i==BTGE_TXFL_EFFECT_ORIGIN)
		{
			V3F_COPY(cur->org, tmp->mdl_org);

			printf("BTGE_BrushBSP_BuildBrushModelListFast: "
				"%p Origin=(%f %f %f)\n", tmp,
				tmp->mdl_org[0], tmp->mdl_org[1], tmp->mdl_org[2]);
		}else
		{
			tmp->flags=BTGE_Brush_CombineTextureFlags(
				tmp->flags, cur->flags);
		}

		cur->model=tmp;
		cur->mnext=tmp->brush;
		tmp->brush=cur;
		cur=cur->chain;
	}

	tmp->bsp=BTGE_BrushBSP_BuildTreeListFast(lst);
	return(tmp);
}

BTGE_API void BTGE_BrushBSP_FreeBrushModel(BTGE_BrushModel *bmdl)
{
	BTGE_Brush *cur;

	BTGE_BrushBSP_FreeTreeNode(bmdl->bsp);

	cur=bmdl->brush;
	while(cur)
	{
		cur->model=NULL;
		cur=cur->mnext;
	}

	gcfree(bmdl);
}

#if 0
// BTGE_API void BTGE_BrushModel_DrawBrushModel(BTGE_BrushModel *bmdl)
{
	BTGE_Brush *cur, *lst;

	cur=bmdl->brush;
	while(cur)
	{
		if(!(cur->flags&BTGE_BRFL_ALPHA))
			BTGE_Brush_DrawFaces(cur);
		cur=cur->mnext;
	}

	cur=bmdl->brush;
	while(cur)
	{
		if(cur->flags&BTGE_BRFL_ALPHA)
			BTGE_Brush_DrawFaces(cur);
		cur=cur->mnext;
	}
}
#endif

BTGE_API void BTGE_BrushModel_BoxModel(BTGE_BrushModel *bmdl,
	float *mins, float *maxs)
{
	BTGE_Brush *cur, *lst;

	cur=bmdl->brush; lst=NULL;
	while(cur)
	{
		cur->chain=lst; lst=cur;
		cur=cur->mnext;
	}

	BTGE_BrushBSP_BBoxList(lst, mins, maxs);

	V3F_SUB(mins, bmdl->mdl_org, mins);
	V3F_SUB(maxs, bmdl->mdl_org, maxs);
}

BTGE_API void BTGE_BrushModel_GetExtents(BTGE_BrushModel *bmdl,
	float *dir, float *min, float *max)
{
	BTGE_Brush *cur, *lst;
	float f, g, m, n;

	m=999999; n=-999999;
	cur=bmdl->brush;
	while(cur)
	{
		BTGE_Brush_GetExtents(cur, dir, &f, &g);
		if(f<m)m=f; if(g>n)n=g;
		cur=cur->mnext;
	}

	*min=m; *max=n;
}

#if 0
// BTGE_API bool BTGE_BrushModel_CheckBox(BTGE_BrushModel *bmdl,
	float *min, float *max)
{
	float bm[3], bn[3];
	BTGE_Brush *lst;

	lst=BTGE_BrushBSP_BoxQueryTreeNode(ent->bmdl->bsp, bm, bn);
	if(!lst)return(0);
	
	return(1);
}
#endif
