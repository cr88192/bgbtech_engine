/*
attempt at a customized physics system for skeletal models.

 */

#include <lbxgl2.h>

void LBXGL_PhysSkel_CalcTransform(float *pos, float *rot,
	float *pxform, float *xform)
{
	float tmat[16], tmat2[16];
	float q[4];

	QuatF_FromAngles(rot, q);
	QuatF_ToMatrix(q, tmat);
	tmat[3]=pos[0];
	tmat[7]=pos[1];
	tmat[11]=pos[2];

	if(pxform)
	{
//		MatNF_MatMult(pxform, 4, 4,
//			tmat, 4, 4, tmat2);
		Mat4F_MatMult(pxform, tmat, tmat2);
		MatNF_Copy(tmat2, xform, 4, 4);
	}else MatNF_Copy(tmat, xform, 4, 4);
}

void LBXGL_PhysSkel_CalcCurTransforms(LBXGL_SkelState *state)
{
	float pos[3], rot[3];
	int i, j, k;

	for(i=0; i<state->mesh->num_bones; i++)
	{
		pos[0]=state->basepos[i*6+0]+state->relpos[i*6+0];
		pos[1]=state->basepos[i*6+1]+state->relpos[i*6+1];
		pos[2]=state->basepos[i*6+2]+state->relpos[i*6+2];
		rot[0]=state->basepos[i*6+3]+state->relpos[i*6+3];
		rot[1]=state->basepos[i*6+4]+state->relpos[i*6+4];
		rot[2]=state->basepos[i*6+5]+state->relpos[i*6+5];

		j=state->mesh->bone[i]->parent;
		LBXGL_PhysSkel_CalcTransform(pos, rot,
			(j>=0)?(state->xforms+(j*16)):NULL,
			state->xforms+(i*16));
	}
}

void LBXGL_PhysSkel_CalcNextTransforms(LBXGL_SkelState *state)
{
	float pos[3], rot[3];
	int i, j, k;

	for(i=0; i<state->mesh->num_bones; i++)
	{
		pos[0]=state->basepos[i*6+0]+state->nrelpos[i*6+0];
		pos[1]=state->basepos[i*6+1]+state->nrelpos[i*6+1];
		pos[2]=state->basepos[i*6+2]+state->nrelpos[i*6+2];
		rot[0]=state->basepos[i*6+3]+state->nrelpos[i*6+3];
		rot[1]=state->basepos[i*6+4]+state->nrelpos[i*6+4];
		rot[2]=state->basepos[i*6+5]+state->nrelpos[i*6+5];

		j=state->mesh->bone[i]->parent;
		LBXGL_PhysSkel_CalcTransform(pos, rot,
			(j>=0)?(state->nxforms+(j*16)):NULL,
			state->nxforms+(i*16));
	}
}

void LBXGL_PhysSkel_ReBox(float *amins, float *amaxs, float *sp,
	float *bmins, float *bmaxs)
{
	float x, y;
	int i, j;

	for(i=0; i<3; i++)
		for(j=0; j<3; j++)
	{
		x=amins[j]*sp[i*4+j]+sp[i*4+3];
		y=amaxs[j]*sp[i*4+j]+sp[i*4+3];

		if(x<bmins[i])bmins[i]=x;
		if(x>bmaxs[i])bmaxs[i]=x;
		if(y<bmins[i])bmins[i]=y;
		if(y>bmaxs[i])bmaxs[i]=y;
	}

//	b[0]=(a[0]*sp[0])+(a[1]*sp[1])+(a[2]*sp[2])+sp[3];
//	b[1]=(a[0]*sp[4])+(a[1]*sp[5])+(a[2]*sp[6])+sp[7];
//	b[2]=(a[0]*sp[8])+(a[1]*sp[9])+(a[2]*sp[10])+sp[11];
}

void LBXGL_PhysSkel_RecalcCurBBox(LBXGL_SkelState *state)
{
	int i;

	for(i=0; i<3; i++)state->mins[i]=999999;
	for(i=0; i<3; i++)state->maxs[i]=-999999;

	for(i=0; i<state->mesh->num_bones; i++)
		LBXGL_PhysSkel_ReBox(
			state->mesh->bone[i]->min,
			state->mesh->bone[i]->max,
			state->xforms+(i*16), state->mins, state->maxs);
}

void LBXGL_PhysSkel_RecalcNextBBox(LBXGL_SkelState *state)
{
	int i;

	for(i=0; i<3; i++)state->nmins[i]=999999;
	for(i=0; i<3; i++)state->nmaxs[i]=-999999;

	for(i=0; i<state->mesh->num_bones; i++)
		LBXGL_PhysSkel_ReBox(
			state->mesh->bone[i]->min,
			state->mesh->bone[i]->max,
			state->nxforms+(i*16), state->nmins, state->nmaxs);
}

void LBXGL_PhysSkel_BoxMove2(
	float *smins, float *smaxs, float *start,
	float *emins, float *emaxs, float *end,
	float *mins, float *maxs)
{
	float mins2[3], maxs2[3];
	int i;

	VecNF_Add(smins, start, mins, 3);
	VecNF_Add(smaxs, start, maxs, 3);

	VecNF_Add(emins, end, mins2, 3);
	VecNF_Add(emaxs, end, maxs2, 3);

	for(i=0; i<3; i++)
	{
		if(mins2[i]<mins[i])mins[i]=mins2[i];
		if(maxs2[i]>maxs[i])maxs[i]=maxs2[i];
	}
}

int LBXGL_PhysSkel_SetupEntity(LBXGL_Entity *ent)
{
	LBXGL_SkelMesh *mdl;

	if(!ent->skel)ent->skel=kalloc(sizeof(LBXGL_SkelState));
	if(ent->phys->model!=ent->skel->mesh)
	{
		if(ent->skel->mesh)
		{
			kfree(ent->skel->basepos);
			kfree(ent->skel->relpos);
			kfree(ent->skel->nrelpos);

			kfree(ent->skel->vel);
			kfree(ent->skel->xforms);
			kfree(ent->skel->nxforms);

			kfree(ent->skel->attach);
		}
		if(ent->phys->model)
		{
			ent->skel->mesh=ent->phys->model;
			ent->skel->seq=-1;
			ent->skel->frame=-1;

			mdl=ent->skel->mesh;
			ent->skel->basepos=
				kalloc(mdl->num_bones*6*sizeof(float));
			ent->skel->relpos=
				kalloc(mdl->num_bones*6*sizeof(float));
			ent->skel->nrelpos=
				kalloc(mdl->num_bones*6*sizeof(float));

			ent->skel->vel=
				kalloc(mdl->num_bones*3*sizeof(float));

			ent->skel->xforms=
				kalloc(mdl->num_bones*16*sizeof(float));
			ent->skel->nxforms=
				kalloc(mdl->num_bones*16*sizeof(float));

			ent->skel->attach=kalloc(mdl->num_bones*
				sizeof(LBXGL_SkelState *));

			LBXGL_FmtSmd_SetStateFrame(ent->skel,
				ent->skel->seq, ent->skel->frame);
		}else
		{
			ent->skel->mesh=NULL;

			ent->skel->basepos=NULL;
			ent->skel->relpos=NULL;
			ent->skel->nrelpos=NULL;
			ent->skel->vel=NULL;
			ent->skel->xforms=NULL;
			ent->skel->nxforms=NULL;
			ent->skel->attach=NULL;
		}
	}
}

int LBXGL_PhysSkel_FinishMove(LBXGL_Entity *ent)
{
}

int LBXGL_PhysSkel_HandleImpact(LBXGL_Entity *ent1, LBXGL_Entity *ent2)
{
}

int LBXGL_PhysSkel_HandlePlaneImpact(LBXGL_Entity *ent, double *norm)
{
}

int LBXGL_PhysSkel_FrameEvent(LBXGL_Entity *ent, LBXGL_SkelMesh *mdl)
{
	LBXGL_SkelEvent *cur;

	cur=mdl->anim[ent->skel->seq]->events;
	while(cur)
	{
		if(cur->frame==ent->skel->frame)
		{
			if(!strcmp(cur->target, "SELF"))
				LBXGL_Entity_SendMessage(ent,
					cur->event, cur->args);
				else LBXGL_Event_SendEvent(
					cur->target, cur->event,
					cur->args);
		}
		cur=cur->next;
	}
	return(0);
}

int LBXGL_PhysSkel_NextSequence(LBXGL_Entity *ent, LBXGL_SkelMesh *mdl)
{
	int i, j, k, n, w;

	j=rand()%100;
	n=mdl->anim[ent->skel->seq]->num_rnext;
	for(i=0; i<n; i++)
	{
		w=mdl->anim[ent->skel->seq]->rnext_weight[i];
		if(j>w)
		{
			j-=w;
			continue;
		}

		k=LBXGL_FmtSmd_IndexAnim(mdl,
			mdl->anim[ent->skel->seq]->rnext[i]);
		ent->skel->seq=k;
		ent->skel->frame=0;

//		ent->skel->frametime+=1.0/mdl->anim[ent->skel->seq]->fps;
		LBXGL_PhysSkel_FrameEvent(ent, mdl);

		return(k);
	}

	if(mdl->anim[ent->skel->seq]->next)
	{
		i=LBXGL_FmtSmd_IndexAnim(mdl,
			mdl->anim[ent->skel->seq]->next);
		ent->skel->seq=i;
		ent->skel->frame=0;

//		ent->skel->frametime+=1.0/mdl->anim[ent->skel->seq]->fps;
		LBXGL_PhysSkel_FrameEvent(ent, mdl);

		return(i);
	}

	ent->skel->frametime+=1.0/mdl->anim[ent->skel->seq]->fps;
	return(ent->skel->seq);
}

int LBXGL_PhysSkel_SequenceEntity(LBXGL_Entity *ent, float dt)
{
	LBXGL_SkelMesh *mdl;
	int i, j;

	mdl=ent->skel->mesh;

	ent->skel->frametime-=dt;
	while((ent->skel->frametime<=0))
	{
		if((ent->skel->seq<0) || (ent->skel->frame<0))
		{
			ent->skel->frametime=1.0/10.0;
			continue;
		}

		if(ent->skel->frame<mdl->anim[ent->skel->seq]->num_frames)
		{
			ent->skel->frametime+=
				1.0/mdl->anim[ent->skel->seq]->fps;
			ent->skel->frame++;
			LBXGL_PhysSkel_FrameEvent(ent, mdl);
			continue;
		}

		if(ent->skel->frame>=mdl->anim[ent->skel->seq]->num_frames)
		{
			LBXGL_PhysSkel_NextSequence(ent, mdl);
			continue;
		}
	}
	LBXGL_FmtSmd_SetStateFrame(ent->skel,
		ent->skel->seq, ent->skel->frame);

	return(0);
}

int LBXGL_PhysSkel_SetEntitySequence(LBXGL_Entity *ent, char *seq)
{
	int i;

	LBXGL_Physics2_SetupEntity(ent, 0);
	LBXGL_PhysSkel_SetupEntity(ent);

	i=LBXGL_FmtSmd_IndexAnim(ent->skel->mesh, seq);
	ent->skel->seq=i;
	ent->skel->frame=0;

	LBXGL_FmtSmd_SetStateFrame(ent->skel,
		ent->skel->seq, ent->skel->frame);

	return(0);
}

LBXGL_Entity *LBXGL_PhysSkel_MoveTimeExtents(LBXGL_Entity *ent,
	LBXGL_Entity *ents, float dt)
{
	LBXGL_Entity *ents1;
	double mins2[3], maxs2[3];

	float mins[3], maxs[3];
	float smins[3], smaxs[3];
	float emins[3], emaxs[3];
	float sorg[3], eorg[3];
	int i;

	LBXGL_PhysSkel_CalcCurTransforms(ent->skel);
	LBXGL_PhysSkel_CalcNextTransforms(ent->skel);
	LBXGL_PhysSkel_RecalcCurBBox(ent->skel);
	LBXGL_PhysSkel_RecalcNextBBox(ent->skel);

	//type conversion
	VecND2F_Copy(ent->phys->smove, sorg, 3);
	VecND2F_Copy(ent->phys->emove, eorg, 3);

	for(i=0; i<3; i++)
	{
		smins[i]=ent->skel->mins[i]+ent->phys->smove[i];
		smaxs[i]=ent->skel->maxs[i]+ent->phys->smove[i];

		emins[i]=ent->skel->nmins[i]+ent->phys->emove[i];
		emaxs[i]=ent->skel->nmaxs[i]+ent->phys->emove[i];
	}

	LBXGL_PhysSkel_BoxMove2(
		ent->skel->mins, ent->skel->maxs, sorg,
		ent->skel->nmins, ent->skel->nmaxs, eorg,
		mins, maxs);

//	LBXGL_Physics2_BoxMove(ent->skel->mins, ent->skel->maxs, 
//		ent->phys->smove, ent->phys->emove, mins, maxs);

	VecNF2D_Copy(mins, mins2, 3);
	VecNF2D_Copy(maxs, maxs2, 3);
	ents1=LBXGL_Physics2_QueryVolume(mins2, maxs2, ents, 0);

}

int LBXGL_PhysSkel_Init()
{
}
