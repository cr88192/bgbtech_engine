#include <lbxgl2.h>

BSP_Brush *BSP_BuildBrush(
	float *norm, char **tex, float *vs, float *vt, int num)
{
	static float pts0[16*3];
	static float pts1[16*3];

	BSP_Brush *tmp;
	BSP_Face *fst, *lst, *ftmp;

	float tv0[3], tv1[3], tv2[3];
	float sx, sy, f;
	int xs, ys, tn, fl, fl2;
	int i, j, k, l;

	fl2=0;
	fst=NULL;
	lst=NULL;
	for(i=0; i<num; i++)
	{
		ftmp=kalloc(sizeof(BSP_Face));
		memset(ftmp, 0, sizeof(BSP_Face));
		V4F_COPY(norm+i*4, ftmp->norm);
		V4F_COPY(vs+i*4, ftmp->sdir);
		V4F_COPY(vt+i*4, ftmp->tdir);
		ftmp->tex=tex[i];

//		BSP_GetTexture(ftmp->tex, &xs, &ys, &fl);
		tn=LBXGL_Texture_LoadImageSize(ftmp->tex, &xs, &ys);
		j=LBXGL_Texture_GetImageFlags(tn);

		fl=0;
		if(j&LBXGL_TXFL_WINDOW)fl|=BSPFACE_FL_WINDOW;
		if(j&LBXGL_TXFL_NOCSG)fl|=BSPFACE_FL_NOCSG;

		ftmp->flags=fl;
		fl2|=fl;

		Hull_MakePlaneFace(norm+i*4, pts0);
		l=4;

		for(j=0; j<num; j++)
		{
			if(i==j)continue;

			l=Hull_ClipFace(norm+j*4, pts0, pts1, l);
			for(k=0; k<(l*3); k++)pts0[k]=pts1[k];
		}

		ftmp->nvecs=l;
		ftmp->vecs=kalloc(3*l*sizeof(float));
		ftmp->st=kalloc(2*l*sizeof(float));

		for(j=0; j<l; j++)
		{
			V3F_COPY(pts0+j*3, ftmp->vecs+j*3);
			ftmp->st[j*2+0]=V3F_NDOT(pts0+j*3, ftmp->sdir)/xs;
			ftmp->st[j*2+1]=V3F_NDOT(pts0+j*3, ftmp->tdir)/ys;
		}

		if(fst)
		{
			lst->next=ftmp;
			lst=ftmp;
		}else
		{
			fst=ftmp;
			lst=ftmp;
		}
	}

	tmp=kalloc(sizeof(BSP_Brush));
	tmp->face=fst;
	tmp->flags=fl2;

	return(tmp);
}

BSP_Brush *BSP_TransBrush(LBXGL_Brush *brsh)
{
	BSP_Brush *tmp;
	tmp=BSP_BuildBrush(brsh->norm, brsh->tex, brsh->sdir, brsh->tdir,
		brsh->n_face);
	return(tmp);
}

BSP_Brush *BSP_TransEntBrushes(LBXGL_Brush *brsh)
{
	BSP_Brush *lst, *tmp;
	LBXGL_Brush *cur;

	lst=NULL; cur=brsh;
	while(cur)
	{
		tmp=BSP_TransBrush(cur);
		tmp->next=lst;
		lst=tmp;

		cur=cur->enext;
	}

	return(lst);
}

BSP_Model *BSP_TransEntModel(LBXGL_Brush *brsh)
{
	BSP_Model *tmp;
	BSP_Brush *blst;

	blst=BSP_TransEntBrushes(brsh);
	tmp=BSP_BuildModel(blst);

	return(tmp);
}
