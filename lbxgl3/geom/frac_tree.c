#include <lbxgl2.h>

typedef struct {
s64 seed;
int height;
}LBXGL_FracTreeInfo;

LBXGL_MeshGroup *lbxgl_frac_treenode=NULL;
LBXGL_MeshGroup *lbxgl_frac_treeleaves=NULL;

s64 LBXGL_Frac_StepRandom(s64 seed)
{
	seed=seed*4294967291LL+1;
	return(seed);
}

float LBXGL_Frac_Random(s64 *rseed)
{
	int i;

	*rseed=*rseed*4294967291LL+1;
	i=(*rseed>>32)&0xFFFFFF;
	return(i/16777215.0);
}

int lbxgl_fractree_draw(LBXGL_ModelState *ctx)
{
	LBXGL_FracTreeInfo *inf;
	s64 seed;

	float tmat[16];
	float dist, lod;
	int i, j;
	float f, alpha;

	inf=(LBXGL_FracTreeInfo *)(ctx->mdl);
//	printf("lbxgl_fractree_draw: %p\n", inf);

//	dist=V3F_DIST(lbxgl_cam->org, ctx->org);
//	lod=LBXGL_Mdl_LodForDistance(dist);

	LBXGL_Mdl_CalcModelTransform(ctx, tmat);

	LBXGL_Shader_PushMultMatrix(tmat);

	pdglScalef(36, 36, 36);

	LBXGL_Frac_DrawTree(inf->seed, inf->height);

	LBXGL_Shader_PopMatrix();

	return(0);
}

void LBXGL_Frac_Init()
{
	static int init=0;
	LBXGL_RenderMethods *mth;

	if(init)return;
	init=1;
	
	lbxgl_frac_treenode=LBXGL_Mesh_LoadModel(
		"models/objects/tree/stemseg0.ac");
	lbxgl_frac_treeleaves=LBXGL_Mesh_LoadModel(
		"models/objects/tree/leafseg0.ac");

	mth=LBXGL_Method_GetRenderMethods("lbxgl_fractree_t");
	mth->drawmdl_draw=lbxgl_fractree_draw;
}

void LBXGL_Frac_DrawTreeNode(s64 seed)
{
	LBXGL_Mesh_DrawGroup(lbxgl_frac_treenode);
}

void LBXGL_Frac_DrawTreeLeaves(s64 seed)
{
	LBXGL_Mesh_DrawGroup(lbxgl_frac_treeleaves);
}

void LBXGL_Frac_DrawTreeR(s64 seed, int level, int height)
{
	s64 savedseed;

	if(level==height)
	{
		pdglPushMatrix();
		pdglRotatef(LBXGL_Frac_Random(&seed)*180, 0, 1, 0);
		LBXGL_Frac_DrawTreeLeaves(seed);
		pdglPopMatrix();
		return;
	}

	LBXGL_Frac_DrawTreeNode(seed);

	pdglPushMatrix();
//	glRotatef(LBXGL_Frac_Random(&seed)*180, 0, 1, 0);
//	glTranslatef(0, 1, 0);
	pdglRotatef(LBXGL_Frac_Random(&seed)*180, 0, 0, 1);
	pdglTranslatef(0, 0, 1);
	pdglScalef(0.7, 0.7, 0.7);

	savedseed=LBXGL_Frac_StepRandom(seed);
	pdglPushMatrix();    
//	glRotatef(110 + LBXGL_Frac_Random(&seed)*40, 0, 1, 0);
//	glRotatef(30 + LBXGL_Frac_Random(&seed)*20, 0, 0, 1);
	pdglRotatef(110 + LBXGL_Frac_Random(&seed)*40, 0, 0, 1);
	pdglRotatef(30 + LBXGL_Frac_Random(&seed)*20, 0, 1, 0);
	LBXGL_Frac_DrawTreeR(seed, level+1, height);
	pdglPopMatrix();

	savedseed=LBXGL_Frac_StepRandom(savedseed);
	pdglPushMatrix();
//	glRotatef(-130 + LBXGL_Frac_Random(&seed)*40, 0, 1, 0);
//	glRotatef(30 + LBXGL_Frac_Random(&seed)*20, 0, 0, 1);
	pdglRotatef(-130 + LBXGL_Frac_Random(&seed)*40, 0, 0, 1);
	pdglRotatef(30 + LBXGL_Frac_Random(&seed)*20, 0, 1, 0);
	LBXGL_Frac_DrawTreeR(seed, level+1, height);
	pdglPopMatrix();

	seed=savedseed;
	pdglPushMatrix();
//	glRotatef(-20 + LBXGL_Frac_Random(&seed)*40, 0, 1, 0);
//	glRotatef(30 + LBXGL_Frac_Random(&seed)*20, 0, 0, 1);
	pdglRotatef(-20 + LBXGL_Frac_Random(&seed)*40, 0, 0, 1);
	pdglRotatef(30 + LBXGL_Frac_Random(&seed)*20, 0, 1, 0);
	LBXGL_Frac_DrawTreeR(seed, level+1, height);
	pdglPopMatrix();

	pdglPopMatrix();
}

void LBXGL_Frac_DrawTree(s64 seed, int height)
{
	LBXGL_Frac_Init();
	LBXGL_Frac_DrawTreeR(seed, 0, height);
}

LBXGL_API void *LBXGL_Frac_GenModel(char *name)
{
	LBXGL_FracTreeInfo *inf;
	void *p;

	LBXGL_Frac_Init();

	if(!strcmp(name, "fractal_tree"))
	{
		inf=(LBXGL_FracTreeInfo *)gctalloc(
			"lbxgl_fractree_t", sizeof(LBXGL_FracTreeInfo));
		inf->seed=rand();
		inf->height=4;
		
		printf("LBXGL_Frac_GenModel: %s %p\n", name, inf);
		
		p=(void *)inf;
		return(p);
	}

	return(NULL);
}
