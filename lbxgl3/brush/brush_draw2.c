#include <lbxgl2.h>

#define LBXGL_DRAW2_CYCLELIMIT	(1<<16)

typedef struct LBXGL_Draw2_QueryState_s LBXGL_Draw2_QueryState;

struct LBXGL_Draw2_QueryState_s
{
LBXGL_BrushDrawFace *faces;
LBXGL_Brush *nonbrush;
};

extern PDGLUI_InputState *lbxgl_state;
extern PDGLUI_Camera *lbxgl_cam;

LBXGL_Draw2_QueryState *lbxgl_draw2_query;

#if 1
LBXGL_BrushDrawFace *LBXGL_BrushDraw2_QueryNodeBrushFacesVisibleMid(
	LBXGL_BrushWorld *world, LBXGL_BrushNode *node,
	LBXGL_BrushDrawFace *lst)
{
	LBXGL_Brush *arr[16384];
	LBXGL_Brush *cur, *nxt;
	int i, n;

	if(!LBXGL_BrushWorld_CheckNodeFrustumVisible(world, node))
		return(lst);

	//use array, as face-building may thrash chains
	cur=node->mid; n=0;
	while(cur)
	{
		arr[n++]=cur;
		cur=cur->nnext;
	}

//	cur=node->mid;
//	while(cur)
//	{

	for(i=0; i<n; i++)
	{
		cur=arr[i];
		if(cur->flags&(LBXGL_BRFL_EFFECT|
				LBXGL_BRFL_FLUID|LBXGL_BRFL_VISCLIP|LBXGL_BRFL_PTLIGHT))
			continue;
//			{ cur=cur->nnext; continue; }

		if(cur->patch || cur->mesh)
		{
//			cur->chain=lbxgl_draw2_query->nonbrush;
//			lbxgl_draw2_query->nonbrush=cur;
//			cur=cur->nnext;
			continue;
		}

		lst=BTGE_BrushCSG_GetBrushFaces(cur, lst);
//		LBXGL_Brush_DrawSolidFlat(cur);
//		cur=cur->nnext;
	}
	
#if 1
	for(i=0; i<n; i++)
	{
		cur=arr[i];
		if(cur->flags&(LBXGL_BRFL_EFFECT|
				LBXGL_BRFL_FLUID|LBXGL_BRFL_VISCLIP))
					continue;

		if(cur->patch || cur->mesh || (cur->flags&LBXGL_BRFL_PTLIGHT))
		{
			cur->chain=lbxgl_draw2_query->nonbrush;
			lbxgl_draw2_query->nonbrush=cur;
			continue;
		}

//		lst=BTGE_BrushCSG_GetBrushFaces(cur, lst);
	}
#endif

	lbxgl_draw2_query->faces=lst;
	return(lst);
}

LBXGL_BrushDrawFace *LBXGL_BrushDraw2_QueryNodeBrushFacesVisible(
	LBXGL_BrushWorld *world, LBXGL_BrushNode *node,
	LBXGL_BrushDrawFace *lst)
{
	float f;

	if(!node)return(lst);

	f=V3F_NDOT(lbxgl_cam->org, node->norm);
	if(f<0)
	{
		lst=LBXGL_BrushDraw2_QueryNodeBrushFacesVisible(
			world, node->lnode, lst);
		lst=LBXGL_BrushDraw2_QueryNodeBrushFacesVisibleMid(
			world, node, lst);
		lst=LBXGL_BrushDraw2_QueryNodeBrushFacesVisible(
			world, node->rnode, lst);
	}else
	{
		lst=LBXGL_BrushDraw2_QueryNodeBrushFacesVisible(
			world, node->rnode, lst);
		lst=LBXGL_BrushDraw2_QueryNodeBrushFacesVisibleMid(
			world, node, lst);
		lst=LBXGL_BrushDraw2_QueryNodeBrushFacesVisible(
			world, node->lnode, lst);
	}
	return(lst);
}
#endif

LBXGL_BrushDrawFace *LBXGL_BrushDraw2_QueryBrushListFacesVisible(
	LBXGL_BrushWorld *world, LBXGL_Brush *brush,
	LBXGL_BrushDrawFace *lst)
{
	LBXGL_Brush *arr[4096];
	LBXGL_Brush *cur, *nxt;
	int i, n;

	//use array, as face-building may thrash chains
	cur=brush; n=0;
	while(cur)
	{
		arr[n++]=cur;
		cur=cur->chain;
	}

	for(i=0; i<n; i++)
	{
		cur=arr[i];
		if(cur->flags&(LBXGL_BRFL_EFFECT|
				LBXGL_BRFL_FLUID|LBXGL_BRFL_VISCLIP))
					continue;

		if(cur->patch || cur->mesh)
		{
//			cur->chain=lbxgl_draw2_query->nonbrush;
//			lbxgl_draw2_query->nonbrush=cur;
			continue;
		}

		lst=BTGE_BrushCSG_GetBrushFaces(cur, lst);
	}

#if 1
	for(i=0; i<n; i++)
	{
		cur=arr[i];
		if(cur->flags&(LBXGL_BRFL_EFFECT|
				LBXGL_BRFL_FLUID|LBXGL_BRFL_VISCLIP))
					continue;

		if(cur->patch || cur->mesh)
		{
			cur->chain=lbxgl_draw2_query->nonbrush;
			lbxgl_draw2_query->nonbrush=cur;
			continue;
		}

//		lst=BTGE_BrushCSG_GetBrushFaces(cur, lst);
	}
#endif

	lbxgl_draw2_query->faces=lst;
	return(lst);

#if 0
	LBXGL_Brush *cur, *nxt;
	int i;

	cur=brush;
	while(cur)
		cur=cur->chain;

	cur=brush;
	while(cur)
	{
		if(cur->flags&(LBXGL_BRFL_EFFECT|
				LBXGL_BRFL_FLUID|LBXGL_BRFL_VISCLIP))
			{ cur=cur->chain; continue; }

		if(cur->patch || cur->mesh)
		{
			nxt=cur->chain;
			cur->chain=lbxgl_draw2_query->nonbrush;
			lbxgl_draw2_query->nonbrush=cur;
			cur=nxt;
			continue;
		}

		lst=BTGE_BrushCSG_GetBrushFaces(cur, lst);
//		LBXGL_Brush_DrawSolidFlat(cur);
		cur=cur->chain;
	}
	lbxgl_draw2_query->faces=lst;
	return(lst);
#endif
}

LBXGL_BrushDrawFace *LBXGL_BrushDraw2_QueryModelFacesVisible(
	LBXGL_BrushWorld *world, LBXGL_BrushDrawFace *lst)
{
	return(LBXGL_BrushDraw2_QueryModelFacesVisibleLight(world, lst, NULL));
}

LBXGL_BrushDrawFace *LBXGL_BrushDraw2_QueryModelFacesVisibleLight(
	LBXGL_BrushWorld *world, LBXGL_BrushDrawFace *lst, LBXGL_Light *light)
{
	LBXGL_ModelState *mcur;
	LBXGL_BrushDrawFace *fcur;
	int i, j;

	mcur=world->mesh; i=1;
	while(mcur)
	{
		if(!(world->mdlvis[i>>3]))
		{
			do { mcur=mcur->next; i++; } while(mcur && (i&7));
			continue;
		}
		
		if(!(world->mdlvis[i>>3]&(1<<(i&7))))
			{ mcur=mcur->next; i++; continue; }
		if(light && !LBXGL_Mdl_CheckModelLight(mcur, light))
			{ mcur=mcur->next; i++; continue; }

		if(mcur->tmp_drawface)
		{
			fcur=mcur->tmp_drawface;
			while(fcur)
			{
				fcur->next=lst;
				lst=fcur;
				fcur=fcur->bnext;
			}
		}

//		LBXGL_Mdl_DrawModelLight(mcur, light);
		mcur=mcur->next; i++;
	}
	
	return(lst);
}

void LBXGL_BrushDraw2_DrawFaceListDark(
	LBXGL_BrushWorld *world, LBXGL_BrushDrawFace *lst)
{
	LBXGL_BrushDrawFace *qlst;	//quads list
	LBXGL_BrushDrawFace *tlst;	//triangle list
	LBXGL_BrushDrawFace *plst;	//poly list
	LBXGL_BrushDrawFace *cur, *nxt;
	int i, xm;
	
	xm=BTGE_TXFL_ALPHA|BTGE_TXFL_FLUID|BTGE_TXFL_EFFECT|BTGE_TXFL_NODRAW;
	
	cur=lst; qlst=NULL; plst=NULL;
	while(cur)
	{
		nxt=cur->next;
		if(V3F_NDOT(lbxgl_cam->org, cur->norm)<=0)
			{ cur=nxt; continue; }
		
		if(cur->flags&xm)
			{ cur=nxt; continue; }
//		if(cur->brush->flags&LBXGL_BRFL_PTLIGHT)
//			{ cur=nxt; continue; }
		
		if(cur->n_vec==4)
		{
			cur->next=qlst;
			qlst=cur;
			cur=nxt;
			continue;
		}

		cur->next=plst;
		plst=cur;
		cur=nxt;
	}
	
	if(qlst)
	{
		LBXGL_Shader_Begin(PDGL_QUADS);

		cur=qlst;
		while(cur)
		{
			if(cur->n_vec!=4)
			{
				if(cur->vec)
				{
					for(i=0; i<cur->n_vec; i++)
					{
						LBXGL_Shader_Vertex3fv(
							cur->xyz+cur->vec[i*4+0]*3);
					}
				}else
				{
					for(i=0; i<cur->n_vec; i++)
					{
						LBXGL_Shader_Vertex3fv(cur->xyz+i*3);
					}
				}
			}else if(cur->vec)
			{
				LBXGL_Shader_Vertex3fv(cur->xyz+cur->vec[0*4+0]*3);
				LBXGL_Shader_Vertex3fv(cur->xyz+cur->vec[1*4+0]*3);
				LBXGL_Shader_Vertex3fv(cur->xyz+cur->vec[2*4+0]*3);
				LBXGL_Shader_Vertex3fv(cur->xyz+cur->vec[3*4+0]*3);
			}else
			{
				LBXGL_Shader_Vertex3fv(cur->xyz+0*3);
				LBXGL_Shader_Vertex3fv(cur->xyz+1*3);
				LBXGL_Shader_Vertex3fv(cur->xyz+2*3);
				LBXGL_Shader_Vertex3fv(cur->xyz+3*3);
			}
			cur=cur->next;
		}
		
		LBXGL_Shader_EndFlatVA();
	}

	if(plst)
	{
		cur=plst;
		while(cur)
		{
//			LBXGL_Shader_Begin(GL_POLYGON);
			LBXGL_Shader_Begin(cur->prim);
			if(cur->vec)
			{
				for(i=0; i<cur->n_vec; i++)
					LBXGL_Shader_Vertex3fv(cur->xyz+cur->vec[i*4+0]*3);
			}else
			{
				for(i=0; i<cur->n_vec; i++)
					LBXGL_Shader_Vertex3fv(cur->xyz+i*3);
			}
			LBXGL_Shader_EndFlatVA();

			cur=cur->next;
		}
	}
}

void LBXGL_BrushDraw2_DrawBrushListDark(
	LBXGL_BrushWorld *world, LBXGL_Brush *lst)
{
	LBXGL_Brush *cur;
	int k;
	
	cur=lst; k=LBXGL_DRAW2_CYCLELIMIT;
	while(cur && k--)
	{
		if(cur->flags&LBXGL_BRFL_PTLIGHT)
			{ cur=cur->chain; continue; }
		LBXGL_Brush_DrawBrushDark(cur);
		cur=cur->chain;
	}

	if(k<=0)
	{
		printf("LBXGL_BrushDraw2_DrawBrushListDark: "
			"Cycle Timeout\n");
	}
}

void LBXGL_BrushDraw2_DrawWorldDark(
	LBXGL_BrushWorld *world)
{
	LBXGL_Draw2_QueryState qtemp;
	LBXGL_BrushDrawFace *lst;
	
	lbxgl_draw2_query=&qtemp;
	memset(lbxgl_draw2_query, 0,
		sizeof(LBXGL_Draw2_QueryState));
	
	LBXGL_Shader_Color4f(0, 0, 0, 1);
	
	lst=LBXGL_BrushDraw2_QueryNodeBrushFacesVisible(
		world, world->bsp, NULL);
	lst=LBXGL_BrushDraw2_QueryModelFacesVisible(world, lst);
	LBXGL_BrushDraw2_DrawFaceListDark(world, lst);
	
	LBXGL_BrushDraw2_DrawBrushListDark(world,
		lbxgl_draw2_query->nonbrush);
//	LBXGL_BrushDraw2_DrawNodeNonBrushesDark(world, world->bsp);
}

int LBXGL_BrushDraw2_SplitFaceListTextures(
	LBXGL_BrushWorld *world, LBXGL_BrushDrawFace *lst,
	LBXGL_BrushDrawFace **chains)
{
	LBXGL_BrushDrawFace *tex_chain[4096];
	LBXGL_BrushDrawFace *cur, *nxt;
	int i, j, n;
	
	memset(tex_chain, 0, 4096*sizeof(LBXGL_BrushDrawFace *));

	cur=lst; n=1;
	while(cur)
	{
		nxt=cur->next;
			
		i=(cur->tex)&4095;
		if(i>=n)n=i+1;
		cur->next=tex_chain[i];
		tex_chain[i]=cur;
			
		cur=nxt;
	}

	j=0;
	for(i=0; i<n; i++)
		if(tex_chain[i])
	{
		if(j>=250)break;
		chains[j++]=tex_chain[i];
	}
	return(j);
}

int LBXGL_BrushDraw2_SplitFaceListLightTextures(
	LBXGL_BrushWorld *world, LBXGL_BrushDrawFace *lst,
	LBXGL_BrushDrawFace **chains)
{
	LBXGL_BrushDrawFace *tex_chain[4096];
	LBXGL_BrushDrawFace *cur, *nxt;
	int i, j, n;
	
	if(!lst)return(0);
	if(world->flags&BTGE_WFL_LIGHTBUDGET)
	{
		chains[0]=lst;
		return(1);
	}
	
	memset(tex_chain, 0, 4096*sizeof(LBXGL_BrushDrawFace *));

	cur=lst; n=1;
	while(cur)
	{
		nxt=cur->next;
		
		if((cur->flags&LBXGL_TXFL_LIGHTMAT) &&
			!(world->flags&BTGE_WFL_LIGHTBUDGET))
		{
			i=(cur->tex)&4095;
			if(i>=n)n=i+1;
			cur->next=tex_chain[i];
			tex_chain[i]=cur;
		}else
		{
			cur->next=tex_chain[0];
			tex_chain[0]=cur;
		}
			
		cur=nxt;
	}

	j=0;
	for(i=0; i<n; i++)
		if(tex_chain[i])
	{
		if(j>=250)break;
		chains[j++]=tex_chain[i];
	}
	return(j);
}

void LBXGL_BrushDraw2_SplitFaceListQuadPoly(
	LBXGL_BrushWorld *world, LBXGL_BrushDrawFace *lst,
	LBXGL_BrushDrawFace **rqlst, LBXGL_BrushDrawFace **rplst)
{
	LBXGL_BrushDrawFace *qlst;	//quads list
	LBXGL_BrushDrawFace *plst;	//poly list
	LBXGL_BrushDrawFace *cur, *nxt;
	int xm;

	xm=BTGE_TXFL_ALPHA|BTGE_TXFL_FLUID|BTGE_TXFL_EFFECT|BTGE_TXFL_NODRAW;	
	cur=lst; qlst=NULL; plst=NULL;
	while(cur)
	{
		nxt=cur->next;
		if(V3F_NDOT(lbxgl_cam->org, cur->norm)<=0)
			{ cur=nxt; continue; }
		
		if(cur->flags&xm)
			{ cur=nxt; continue; }
//		if(cur->brush->flags&LBXGL_BRFL_PTLIGHT)
//			{ cur=nxt; continue; }

		if(((cur->n_vec==4) && (cur->prim==BTGE_POLYGON)) ||
			(cur->prim==BTGE_QUADS))
		{
			cur->next=qlst;
			qlst=cur;
			cur=nxt;
			continue;
		}

		cur->next=plst;
		plst=cur;
		cur=nxt;
	}
	
	*rqlst=qlst;
	*rplst=plst;
}

void LBXGL_BrushDraw2_DrawFaceListFinal(
	LBXGL_BrushWorld *world, LBXGL_BrushDrawFace *lst)
{
	LBXGL_BrushDrawFace *tex_chain[256];
	LBXGL_BrushDrawFace *qlst;	//quads list
	LBXGL_BrushDrawFace *plst;	//poly list
	LBXGL_BrushDrawFace *cur, *nxt;
	int i, j, k, xm;
	
	LBXGL_BrushDraw2_SplitFaceListQuadPoly(world, lst, &qlst, &plst);
	
	if(qlst)
	{
		j=LBXGL_BrushDraw2_SplitFaceListTextures(world, qlst, tex_chain);

		for(i=0; i<j; i++)
//			if(tex_chain[i])
		{
			LBXGL_Shader_BlendFunc(GL_DST_COLOR, GL_ZERO);
			LBXGL_Shader_Color4f(1, 1, 1, 1);

			cur=tex_chain[i];
			LBXGL_Shader_BindTexture(cur->tex);

			LBXGL_Shader_Begin(PDGL_QUADS); k=LBXGL_DRAW2_CYCLELIMIT;
			while(cur && k--)
			{
				LBXGL_Shader_Normal4fv(cur->norm);
				LBXGL_Shader_NormalX4fv(cur->sdir);
				LBXGL_Shader_NormalY4fv(cur->tdir);

				if(cur->n_vec!=4)
				{
					if(cur->vec)
					{
						for(i=0; i<cur->n_vec; i++)
						{
							LBXGL_Shader_TexCoord2fv(
								cur->st+cur->vec[i*4+1]*2);
							LBXGL_Shader_Vertex3fv(
								cur->xyz+cur->vec[i*4+0]*3);
						}
					}else
					{
						for(i=0; i<cur->n_vec; i++)
						{
							LBXGL_Shader_TexCoord2fv(cur->st+i*2);
							LBXGL_Shader_Vertex3fv(cur->xyz+i*3);
						}
					}
				}else if(cur->vec)
				{
					LBXGL_Shader_TexCoord2fv(cur->st+cur->vec[0*4+1]*2);
					LBXGL_Shader_Vertex3fv(cur->xyz+cur->vec[0*4+0]*3);

					LBXGL_Shader_TexCoord2fv(cur->st+cur->vec[1*4+1]*2);
					LBXGL_Shader_Vertex3fv(cur->xyz+cur->vec[1*4+0]*3);

					LBXGL_Shader_TexCoord2fv(cur->st+cur->vec[2*4+1]*2);
					LBXGL_Shader_Vertex3fv(cur->xyz+cur->vec[2*4+0]*3);

					LBXGL_Shader_TexCoord2fv(cur->st+cur->vec[3*4+1]*2);
					LBXGL_Shader_Vertex3fv(cur->xyz+cur->vec[3*4+0]*3);
				}else
				{
					LBXGL_Shader_TexCoord2fv(cur->st+0*2);
					LBXGL_Shader_Vertex3fv(cur->xyz+0*3);

					LBXGL_Shader_TexCoord2fv(cur->st+1*2);
					LBXGL_Shader_Vertex3fv(cur->xyz+1*3);

					LBXGL_Shader_TexCoord2fv(cur->st+2*2);
					LBXGL_Shader_Vertex3fv(cur->xyz+2*3);

					LBXGL_Shader_TexCoord2fv(cur->st+3*2);
					LBXGL_Shader_Vertex3fv(cur->xyz+3*3);
				}
				cur=cur->next;
			}
			LBXGL_Shader_End();

			if(k<=0)
			{
				printf("LBXGL_BrushDraw2_DrawFaceListFinal: "
					"Cycle Timeout A\n");
			}
		}
	}

	if(plst)
	{
		cur=plst; k=LBXGL_DRAW2_CYCLELIMIT;
		while(cur && k--)
		{
			LBXGL_Shader_BlendFunc(GL_DST_COLOR, GL_ZERO);
			LBXGL_Shader_Color4f(1, 1, 1, 1);

			LBXGL_Shader_Normal4fv(cur->norm);
			LBXGL_Shader_NormalX4fv(cur->sdir);
			LBXGL_Shader_NormalY4fv(cur->tdir);

			LBXGL_Shader_BindTexture(cur->tex);
//			LBXGL_Shader_Begin(GL_POLYGON);
			LBXGL_Shader_Begin(cur->prim);
			if(cur->vec)
			{
				for(i=0; i<cur->n_vec; i++)
				{
					LBXGL_Shader_TexCoord2fv(cur->st+cur->vec[i*4+1]*2);
					LBXGL_Shader_Vertex3fv(cur->xyz+cur->vec[i*4+0]*3);
				}
			}else
			{
				for(i=0; i<cur->n_vec; i++)
				{
					LBXGL_Shader_TexCoord2fv(cur->st+i*2);
					LBXGL_Shader_Vertex3fv(cur->xyz+i*3);
				}
			}
			LBXGL_Shader_End();

			cur=cur->next;
		}

		if(k<=0)
		{
			printf("LBXGL_BrushDraw2_DrawFaceListFinal: "
				"Cycle Timeout B\n");
		}
	}
}

void LBXGL_BrushDraw2_DrawWorldFinal(
	LBXGL_BrushWorld *world)
{
	LBXGL_Draw2_QueryState qtemp;
	LBXGL_BrushDrawFace *lst;
	LBXGL_Brush *cur;
	int k;
	
	lbxgl_draw2_query=&qtemp;
	memset(lbxgl_draw2_query, 0,
		sizeof(LBXGL_Draw2_QueryState));
	
//	LBXGL_Shader_Color4f(0, 0, 0, 1);
	
	lst=LBXGL_BrushDraw2_QueryNodeBrushFacesVisible(
		world, world->bsp, NULL);
	lst=LBXGL_BrushDraw2_QueryModelFacesVisible(world, lst);
	LBXGL_BrushDraw2_DrawFaceListFinal(world, lst);

	cur=lbxgl_draw2_query->nonbrush; k=LBXGL_DRAW2_CYCLELIMIT;
	while(cur && k--)
	{
		pdglEnable(GL_CULL_FACE);
//		LBXGL_Brush_DrawBrushLight3(cur, light);
		LBXGL_Brush_DrawBrushFinal(cur);
		cur=cur->chain;
	}
	if(k<=0)
	{
		printf("LBXGL_BrushDraw2_DrawWorldFinal: "
			"Cycle Timeout\n");
	}

	LBXGL_Shader_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//	LBXGL_BrushDraw2_DrawNodeNonBrushesDark(world, world->bsp);
}

void LBXGL_BrushDraw2_DrawFaceListBasic(
	LBXGL_BrushWorld *world, LBXGL_BrushDrawFace *lst)
{
	LBXGL_BrushDrawFace *tex_chain[256];
	LBXGL_BrushDrawFace *qlst;	//quads list
	LBXGL_BrushDrawFace *plst;	//poly list
	LBXGL_BrushDrawFace *cur, *nxt;
	int i, j, k, xm;
	
	LBXGL_BrushDraw2_SplitFaceListQuadPoly(world, lst, &qlst, &plst);
	
	if(qlst)
	{
		j=LBXGL_BrushDraw2_SplitFaceListTextures(world, qlst, tex_chain);

		for(i=0; i<j; i++)
//			if(tex_chain[i])
		{
//			LBXGL_Shader_BlendFunc(GL_DST_COLOR, GL_ZERO);
			LBXGL_Shader_Color4f(1, 1, 1, 1);

			cur=tex_chain[i];
			LBXGL_Shader_BindTexture(cur->tex);

			LBXGL_Shader_Begin(PDGL_QUADS); k=LBXGL_DRAW2_CYCLELIMIT;
			while(cur && k--)
			{
				LBXGL_Shader_Normal4fv(cur->norm);
				LBXGL_Shader_NormalX4fv(cur->sdir);
				LBXGL_Shader_NormalY4fv(cur->tdir);

				if(cur->n_vec!=4)
				{
					if(cur->vec)
					{
						for(i=0; i<cur->n_vec; i++)
						{
							LBXGL_Shader_TexCoord2fv(
								cur->st+cur->vec[i*4+1]*2);
							LBXGL_Shader_Vertex3fv(
								cur->xyz+cur->vec[i*4+0]*3);
						}
					}else
					{
						for(i=0; i<cur->n_vec; i++)
						{
							LBXGL_Shader_TexCoord2fv(cur->st+i*2);
							LBXGL_Shader_Vertex3fv(cur->xyz+i*3);
						}
					}
				}else if(cur->vec)
				{
					LBXGL_Shader_TexCoord2fv(cur->st+cur->vec[0*4+1]*2);
					LBXGL_Shader_Vertex3fv(cur->xyz+cur->vec[0*4+0]*3);

					LBXGL_Shader_TexCoord2fv(cur->st+cur->vec[1*4+1]*2);
					LBXGL_Shader_Vertex3fv(cur->xyz+cur->vec[1*4+0]*3);

					LBXGL_Shader_TexCoord2fv(cur->st+cur->vec[2*4+1]*2);
					LBXGL_Shader_Vertex3fv(cur->xyz+cur->vec[2*4+0]*3);

					LBXGL_Shader_TexCoord2fv(cur->st+cur->vec[3*4+1]*2);
					LBXGL_Shader_Vertex3fv(cur->xyz+cur->vec[3*4+0]*3);
				}else
				{
					LBXGL_Shader_TexCoord2fv(cur->st+0*2);
					LBXGL_Shader_Vertex3fv(cur->xyz+0*3);

					LBXGL_Shader_TexCoord2fv(cur->st+1*2);
					LBXGL_Shader_Vertex3fv(cur->xyz+1*3);

					LBXGL_Shader_TexCoord2fv(cur->st+2*2);
					LBXGL_Shader_Vertex3fv(cur->xyz+2*3);

					LBXGL_Shader_TexCoord2fv(cur->st+3*2);
					LBXGL_Shader_Vertex3fv(cur->xyz+3*3);
				}
				cur=cur->next;
			}
			LBXGL_Shader_End();

			if(k<=0)
			{
				printf("LBXGL_BrushDraw2_DrawFaceListFinal: "
					"Cycle Timeout A\n");
			}
		}
	}

	if(plst)
	{
		cur=plst; k=LBXGL_DRAW2_CYCLELIMIT;
		while(cur && k--)
		{
//			LBXGL_Shader_BlendFunc(GL_DST_COLOR, GL_ZERO);
			LBXGL_Shader_Color4f(1, 1, 1, 1);

			LBXGL_Shader_Normal4fv(cur->norm);
			LBXGL_Shader_NormalX4fv(cur->sdir);
			LBXGL_Shader_NormalY4fv(cur->tdir);

			LBXGL_Shader_BindTexture(cur->tex);
//			LBXGL_Shader_Begin(GL_POLYGON);
			LBXGL_Shader_Begin(cur->prim);
			if(cur->vec)
			{
				for(i=0; i<cur->n_vec; i++)
				{
					LBXGL_Shader_TexCoord2fv(cur->st+cur->vec[i*4+1]*2);
					LBXGL_Shader_Vertex3fv(cur->xyz+cur->vec[i*4+0]*3);
				}
			}else
			{
				for(i=0; i<cur->n_vec; i++)
				{
					LBXGL_Shader_TexCoord2fv(cur->st+i*2);
					LBXGL_Shader_Vertex3fv(cur->xyz+i*3);
				}
			}
			LBXGL_Shader_End();

			cur=cur->next;
		}

		if(k<=0)
		{
			printf("LBXGL_BrushDraw2_DrawFaceListFinal: "
				"Cycle Timeout B\n");
		}
	}
}

void LBXGL_BrushDraw2_DrawWorldBasic(
	LBXGL_BrushWorld *world)
{
	LBXGL_Draw2_QueryState qtemp;
	LBXGL_BrushDrawFace *lst;
	LBXGL_Brush *cur;
	int k;
	
	lbxgl_draw2_query=&qtemp;
	memset(lbxgl_draw2_query, 0,
		sizeof(LBXGL_Draw2_QueryState));
	
//	LBXGL_Shader_Color4f(0, 0, 0, 1);
	
	lst=LBXGL_BrushDraw2_QueryNodeBrushFacesVisible(
		world, world->bsp, NULL);
	lst=LBXGL_BrushDraw2_QueryModelFacesVisible(world, lst);
	LBXGL_BrushDraw2_DrawFaceListBasic(world, lst);

	cur=lbxgl_draw2_query->nonbrush; k=LBXGL_DRAW2_CYCLELIMIT;
	while(cur && k--)
	{
		LBXGL_Shader_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		pdglEnable(GL_CULL_FACE);
//		LBXGL_Brush_DrawBrushLight3(cur, light);
//		LBXGL_Brush_DrawBrush(cur);
		LBXGL_Brush_DrawFaces(cur);
		cur=cur->chain;
	}
	if(k<=0)
	{
		printf("LBXGL_BrushDraw2_DrawWorldFinal: "
			"Cycle Timeout\n");
	}

	LBXGL_Shader_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//	LBXGL_BrushDraw2_DrawNodeNonBrushesDark(world, world->bsp);
}


void LBXGL_BrushDraw2_DrawFaceListLight(
	LBXGL_BrushWorld *world, LBXGL_BrushDrawFace *lst,
	LBXGL_Light *light)
{
	LBXGL_BrushDrawFace *tex_chain[256];
	float lorg[3];
	LBXGL_BrushDrawFace *qlst;	//quads list
	LBXGL_BrushDrawFace *plst;	//poly list
	LBXGL_BrushDrawFace *cur, *nxt;
	int i, j, k, xm;

	LBXGL_Shader_Color4f(1, 1, 1, 1);

	LBXGL_Shader_TransformPointLocal(light->org, lorg);

	LBXGL_BrushDraw2_SplitFaceListQuadPoly(world, lst, &qlst, &plst);

	if(qlst)
	{
//		j=LBXGL_BrushDraw2_SplitFaceListTextures(world, qlst, tex_chain);
		j=LBXGL_BrushDraw2_SplitFaceListLightTextures(
			world, qlst, tex_chain);

		for(i=0; i<j; i++)
//			if(tex_chain[i])
		{
//			LBXGL_Shader_BlendFunc(GL_DST_COLOR, GL_ZERO);
			LBXGL_Shader_Color4f(1, 1, 1, 1);

			cur=tex_chain[i];
			LBXGL_Shader_BindTexture(cur->tex);
			LBXGL_Shader_SetCurrentMaterial();

			LBXGL_Shader_BeginLight(PDGL_QUADS); k=LBXGL_DRAW2_CYCLELIMIT;
			while(cur && k--)
			{
				if(V3F_NDOT(lorg, cur->norm)<=0)
					{ cur=cur->next; continue; }
			
				LBXGL_Shader_Normal4fv(cur->norm);
				LBXGL_Shader_NormalX4fv(cur->sdir);
				LBXGL_Shader_NormalY4fv(cur->tdir);
				
				if(cur->n_vec!=4)
				{
					if(cur->vec)
					{
						for(i=0; i<cur->n_vec; i++)
						{
							LBXGL_Shader_TexCoord2fv(
								cur->st+cur->vec[i*4+1]*2);
							LBXGL_Shader_Vertex3fv(
								cur->xyz+cur->vec[i*4+0]*3);
						}
					}else
					{
						for(i=0; i<cur->n_vec; i++)
						{
							LBXGL_Shader_TexCoord2fv(cur->st+i*2);
							LBXGL_Shader_Vertex3fv(cur->xyz+i*3);
						}
					}
				}else if(cur->vec)
				{
					LBXGL_Shader_TexCoord2fv(cur->st+cur->vec[0*4+1]*2);
					LBXGL_Shader_Vertex3fv(cur->xyz+cur->vec[0*4+0]*3);

					LBXGL_Shader_TexCoord2fv(cur->st+cur->vec[1*4+1]*2);
					LBXGL_Shader_Vertex3fv(cur->xyz+cur->vec[1*4+0]*3);

					LBXGL_Shader_TexCoord2fv(cur->st+cur->vec[2*4+1]*2);
					LBXGL_Shader_Vertex3fv(cur->xyz+cur->vec[2*4+0]*3);

					LBXGL_Shader_TexCoord2fv(cur->st+cur->vec[3*4+1]*2);
					LBXGL_Shader_Vertex3fv(cur->xyz+cur->vec[3*4+0]*3);
				}else
				{
					LBXGL_Shader_TexCoord2fv(cur->st+0*2);
					LBXGL_Shader_Vertex3fv(cur->xyz+0*3);

					LBXGL_Shader_TexCoord2fv(cur->st+1*2);
					LBXGL_Shader_Vertex3fv(cur->xyz+1*3);

					LBXGL_Shader_TexCoord2fv(cur->st+2*2);
					LBXGL_Shader_Vertex3fv(cur->xyz+2*3);

					LBXGL_Shader_TexCoord2fv(cur->st+3*2);
					LBXGL_Shader_Vertex3fv(cur->xyz+3*3);
				}

				cur=cur->next;
			}
			LBXGL_Shader_EndLight();
			
			if(k<=0)
			{
				printf("LBXGL_BrushDraw2_DrawFaceListLight: "
					"Cycle Timeout A\n");
			}
		}
	}

	if(plst)
	{
		cur=plst; k=LBXGL_DRAW2_CYCLELIMIT;
		while(cur && k--)
		{
			if(V3F_NDOT(lorg, cur->norm)<=0)
				{ cur=cur->next; continue; }

//			LBXGL_Shader_BlendFunc(GL_DST_COLOR, GL_ZERO);
			LBXGL_Shader_Color4f(1, 1, 1, 1);

			LBXGL_Shader_BindTexture(cur->tex);
			LBXGL_Shader_SetCurrentMaterial();

			LBXGL_Shader_Normal4fv(cur->norm);
			LBXGL_Shader_NormalX4fv(cur->sdir);
			LBXGL_Shader_NormalY4fv(cur->tdir);

//			LBXGL_Shader_BeginLight(GL_POLYGON);
			LBXGL_Shader_BeginLight(cur->prim);
			if(cur->vec)
			{
				for(i=0; i<cur->n_vec; i++)
				{
					LBXGL_Shader_TexCoord2fv(cur->st+cur->vec[i*4+1]*2);
					LBXGL_Shader_Vertex3fv(cur->xyz+cur->vec[i*4+0]*3);
				}
			}else
			{
				for(i=0; i<cur->n_vec; i++)
				{
					LBXGL_Shader_TexCoord2fv(cur->st+i*2);
					LBXGL_Shader_Vertex3fv(cur->xyz+i*3);
				}
			}
			LBXGL_Shader_EndLight();

			cur=cur->next;
		}

		if(k<=0)
		{
			printf("LBXGL_BrushDraw2_DrawFaceListLight: "
				"Cycle Timeout B\n");
		}
	}

	LBXGL_Shader_BindTexture(0);
}

void LBXGL_BrushDraw2_DrawBrushesLight(
	LBXGL_BrushWorld *world, LBXGL_Brush *fst, LBXGL_Light *light)
{
	LBXGL_Draw2_QueryState qtemp;
	LBXGL_BrushDrawFace *lst;
	LBXGL_Brush *cur;
	int k;

	LBXGL_Shader_Color4f(1, 1, 1, 1);

	lbxgl_draw2_query=&qtemp;
	memset(lbxgl_draw2_query, 0,
		sizeof(LBXGL_Draw2_QueryState));
	
	lst=LBXGL_BrushDraw2_QueryBrushListFacesVisible(world, fst, NULL);
	lst=LBXGL_BrushDraw2_QueryModelFacesVisibleLight(world, lst, light);
//	LBXGL_BrushDraw2_SplitFaceListQuadPoly(world, lst, &qlst, &plst);

	LBXGL_BrushDraw2_DrawFaceListLight(world, lst, light);

	cur=lbxgl_draw2_query->nonbrush; k=LBXGL_DRAW2_CYCLELIMIT;
	while(cur && k--)
	{
		if(cur->flags&LBXGL_BRFL_PTLIGHT)
			{ cur=cur->chain; continue; }

		pdglEnable(GL_CULL_FACE);
		LBXGL_Brush_DrawBrushLight3(cur, light);
		cur=cur->chain;
	}
	if(k<=0)
	{
		printf("LBXGL_BrushDraw2_DrawBrushesLight: "
			"Cycle Timeout\n");
	}

	LBXGL_Shader_BindTexture(0);
}
