#include <btgecm.h>

BTGE_API void BTGE_VoxelProps_FreeModel(BTGE_ModelState *ctx)
{
//	if(dytypep(ctx->mdl, "lbxgl_skel2model_t"))
//		Skel_FreeInstance(ctx->state);
	gcfree(ctx);
}

BTGE_API BTGE_ModelState *BTGE_VoxelProps_AllocModel(void)
{
	BTGE_ModelState *tmp;
	tmp=gctalloc("lbxgl_modelstate_t", sizeof(BTGE_ModelState));
	return(tmp);
}

BTGE_API void BTGE_VoxelProps_FlushReleaseChunk(BTGE_VoxelChunk *chk)
{
	BTGE_ModelState *mcur, *mnxt;
	
	if(!chk->mesh)
		return;
	
	mcur=chk->mesh->props;
	chk->mesh->props=NULL;
	while(mcur)
	{
		mnxt=mcur->next;
		BTGE_VoxelProps_FreeModel(mcur);
		mcur=mnxt;
	}
}

BTGE_ModelState *BTGE_VoxelProps_EmitProp(
	BTGE_VoxelChunk *chk, BTGE_VoxelData *vox,
	float *org, float *rot,
	char *name)
{
	BTGE_ModelState *tmp;

	if(!chk->mesh)
	{
		chk->mesh=gctalloc("btge_voxelchunkmesh_t",
			sizeof(BTGE_VoxelChunkMesh));
	}

	tmp=BTGE_VoxelProps_AllocModel();
	tmp->rel_org[0]=org[0]-(chk->ox*chk->rgn->voxsz);
	tmp->rel_org[1]=org[1]-(chk->oy*chk->rgn->voxsz);
	tmp->rel_org[2]=org[2]-(chk->oz*chk->rgn->voxsz);
	
	tmp->modelname=dystrdup(name);
	
	if(rot)
	{
		V3F_COPY(rot+0, tmp->rot+0);
		V3F_COPY(rot+3, tmp->rot+3);
		V3F_COPY(rot+6, tmp->rot+6);
	}else
	{
		V3F_SET(tmp->rot+0, 1, 0, 0);
		V3F_SET(tmp->rot+3, 0, 1, 0);
		V3F_SET(tmp->rot+6, 0, 0, 1);
	}
	
	tmp->next=chk->mesh->props;
	chk->mesh->props=tmp;
	
	return(tmp);
}
