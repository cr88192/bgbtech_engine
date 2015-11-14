#include <lbxgl2.h>

int skel_lod;

LBXGL_API LBXGL_Skel2State *Skel_GetPose(LBXGL_Skel2Model *mdl, int idx)
{
	int i, j;

	if(idx<0)return(NULL);
	if(idx>=4096)return(NULL);	//current limit

	if(idx>=mdl->max_poses)
	{
		i=mdl->max_poses;
		if(!i)i=1;
		while(i<=idx)i=i+(i>>1);
		mdl->pose=gcrealloc(mdl->pose,
			i*sizeof(LBXGL_Skel2State *));
		for(j=mdl->max_poses; j<i; j++)
			mdl->pose[j]=NULL;
	}

	mdl->pose[idx]=Skel_NewInstance(mdl);
	return(mdl->pose[idx]);
}

LBXGL_API LBXGL_Skel2State *Skel_TryGetPose(LBXGL_Skel2Model *mdl, int idx)
{
	int i, j;

	if(idx<0)return(NULL);
	if(idx>=mdl->max_poses)return(NULL);
	return(mdl->pose[idx]);
}

LBXGL_API int Skel_SetPoseAnimFrame(
	LBXGL_Skel2Model *mdl, int idx, char *anim, int frame)
{
	LBXGL_Skel2State *ctx;

	ctx=Skel_GetPose(mdl, idx);
	Skel_SetAnim(ctx, anim);
	ctx->frame_cur=frame;
	return(0);
}

LBXGL_API int Skel_SetPoseBoneOrgQRot(
	LBXGL_Skel2Model *mdl, int pose, int bone,
	float *org, float *qrot)
{
	LBXGL_Skel2State *ctx;

	ctx=Skel_GetPose(mdl, pose);
	ctx->frame_calc=0;

	if(org) { Vec3F_Copy(org, ctx->bone_rorg+bone*3); }
	if(qrot) { QuatF_To3Matrix(qrot, ctx->bone_rmat+bone*9); }

	return(0);
}

LBXGL_API LBXGL_Skel2State *Skel_GetTempPoseAnimFrame(
	LBXGL_Skel2Model *mdl, char *anim, int frame)
{
	LBXGL_Skel2State *ctx;

	ctx=Skel_NewTempInstance(mdl);
	Skel_SetAnim(ctx, anim);
	ctx->frame_cur=frame;
	ctx->lod=skel_lod;
	return(ctx);
}

LBXGL_API LBXGL_Skel2State *Skel_GetTempPoseFrame(
	LBXGL_Skel2Model *mdl, int frame)
{
	LBXGL_Skel2State *ctx;
	LBXGL_Skel2Anim *cur;
	int i;
	
	cur=mdl->anim;
	while(cur)
	{
		i=frame-cur->frame_base;
		if(i>=0 && i<cur->num_frames)
			break;
		cur=cur->next;
	}

	ctx=Skel_NewTempInstance(mdl);
	ctx->anim=cur;
	if(cur)ctx->frame_cur=frame-cur->frame_base;
	ctx->lod=skel_lod;
	return(ctx);
}

LBXGL_API LBXGL_Skel2State *Skel_GetPoseOrFrame(
	LBXGL_Skel2Model *mdl, int frame)
{
	LBXGL_Skel2State *ctx;

	ctx=Skel_TryGetPose(mdl, frame);
	if(ctx)return(ctx);
	ctx=Skel_GetTempPoseFrame(mdl, frame);

//	ri.Con_Printf (PRINT_ALL, "skel %i, %s %d\n", frame,
//		ctx->anim->name, ctx->frame_cur);

	return(ctx);
}

LBXGL_API LBXGL_Skel2State *Skel_GetPoseOrFrame2(
	LBXGL_Skel2Model *mdl, int frame, int lframe, float backlerp)
{
	LBXGL_Skel2State *ctx;
	LBXGL_Skel2State *ctx0;
	LBXGL_Skel2State *ctx1;
	
	ctx0=Skel_GetPoseOrFrame(mdl, frame);
	ctx1=Skel_GetPoseOrFrame(mdl, lframe);
	
	if(!ctx0)return(ctx1);
	if(!ctx1)return(ctx0);

	ctx=Skel_NewTempInstance(mdl);
	ctx->anim=ctx0->anim;
	ctx->frame_cur=ctx0->frame_cur;
	ctx->last_anim=ctx1->anim;
	ctx->last_frame=ctx1->frame_cur;
	ctx->backlerp=backlerp;
	ctx->lod=skel_lod;

	return(ctx);
}

LBXGL_API void Skel_DrawPoseModelLerp(LBXGL_Skel2Model *mdl,
	int frame, int lframe, float backlerp)
{
	LBXGL_Skel2State *ctx;

//	ctx=Skel_TryGetPose(mdl, pose);
	ctx=Skel_GetPoseOrFrame2(mdl, frame, lframe, backlerp);
	Skel_DrawModel(ctx);
}

extern int ac3d_draw_nhack;	//hack to adjust color based on normals
extern float ac3d_draw_color[4];

LBXGL_API void Skel_DrawPoseModelLerpNH(LBXGL_Skel2Model *mdl,
	int frame, int lframe, float backlerp,
	float *clr, float alpha)
{
	LBXGL_Skel2State *ctx;

	ac3d_draw_color[0]=clr[0];
	ac3d_draw_color[1]=clr[1];
	ac3d_draw_color[2]=clr[2];
	ac3d_draw_color[3]=alpha;
	ac3d_draw_nhack=1;

	ctx=Skel_GetPoseOrFrame2(mdl, frame, lframe, backlerp);
	Skel_DrawModel(ctx);

	ac3d_draw_nhack=0;
}

LBXGL_API void Skel_DrawPoseModel(LBXGL_Skel2Model *mdl, int pose)
{
	LBXGL_Skel2State *ctx;

//	ctx=Skel_TryGetPose(mdl, pose);
	ctx=Skel_GetPoseOrFrame(mdl, pose);
	Skel_DrawModel(ctx);
}

LBXGL_API void Skel_DrawPoseModelFlat(LBXGL_Skel2Model *mdl, int pose)
{
	LBXGL_Skel2State *ctx;

//	ctx=Skel_TryGetPose(mdl, pose);
	ctx=Skel_GetPoseOrFrame(mdl, pose);
	Skel_DrawModelFlat(ctx);
}

LBXGL_API void Skel_DrawPoseModelLerpFlat(LBXGL_Skel2Model *mdl,
	int frame, int lframe, float backlerp)
{
	LBXGL_Skel2State *ctx;

	ctx=Skel_GetPoseOrFrame2(mdl, frame, lframe, backlerp);
	Skel_DrawModelFlat(ctx);
}

LBXGL_API void Skel_DrawPoseModelFinal(LBXGL_Skel2Model *mdl, int pose)
{
	LBXGL_Skel2State *ctx;

	ac3d_draw_color[0]=1;
	ac3d_draw_color[1]=1;
	ac3d_draw_color[2]=1;
	ac3d_draw_color[3]=1;
	ac3d_draw_nhack=1;

//	ctx=Skel_TryGetPose(mdl, pose);
	ctx=Skel_GetPoseOrFrame(mdl, pose);
	Skel_DrawModelFinal(ctx);
}

LBXGL_API void Skel_DrawPoseModelLerpFinal(LBXGL_Skel2Model *mdl,
	int frame, int lframe, float backlerp)
{
	LBXGL_Skel2State *ctx;

	ac3d_draw_color[0]=1;
	ac3d_draw_color[1]=1;
	ac3d_draw_color[2]=1;
	ac3d_draw_color[3]=1;
	ac3d_draw_nhack=1;

	ctx=Skel_GetPoseOrFrame2(mdl, frame, lframe, backlerp);
	Skel_DrawModelFinal(ctx);
}

LBXGL_API void Skel_DrawPoseModelShadow(LBXGL_Skel2Model *mdl, int pose,
	float *lorg, float lrad)
{
	LBXGL_Skel2State *ctx;

//	ctx=Skel_TryGetPose(mdl, pose);
	ctx=Skel_GetPoseOrFrame(mdl, pose);
	Skel_DrawModelShadow(ctx, lorg, lrad);
}

LBXGL_API void Skel_DrawPoseModelLerpShadow(LBXGL_Skel2Model *mdl,
	int frame, int lframe, float backlerp,
	float *lorg, float lrad)
{
	LBXGL_Skel2State *ctx;

	ctx=Skel_GetPoseOrFrame2(mdl, frame, lframe, backlerp);
	Skel_DrawModelShadow(ctx, lorg, lrad);
}


LBXGL_API float Skel_GetPoseModelRadius(LBXGL_Skel2Model *mdl, int pose)
{
//	LBXGL_Skel2State *ctx;
//	ctx=Skel_TryGetPose(mdl, pose);
//	ctx=Skel_GetPoseOrFrame(mdl, pose);
//	Skel_DrawModelFinal(ctx);

	return(mdl->radius);
}
