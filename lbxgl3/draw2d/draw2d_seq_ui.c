#include <lbxgl2.h>

extern PDGLUI_InputState *lbxgl_state;
extern PDGLUI_Camera *lbxgl_cam;
extern LBXGL_BrushWorld *lbxgl_world;
extern BTGE_DrawSequence2D *lbxgl_draw2d_editseq;

void LBXGL_Draw2D_DrawStar(float *org)
{
	float v[3];
	int i;

	LBXGL_Shader_Begin(PDGL_LINES);

	for(i=1; i<9; i++)
	{
		v[0]=((i/1)%3)?((((i/1)%3)==1)?1:-1):0;
		v[1]=((i/3)%3)?((((i/3)%3)==1)?1:-1):0;
		V2F_NORMALIZE(v, v);

		LBXGL_Shader_Vertex2f(org[0]-v[0]*0.5, org[1]-v[1]*0.5);
		LBXGL_Shader_Vertex2f(org[0]+v[0]*0.5, org[1]+v[1]*0.5);
	}

	LBXGL_Shader_End();
}

int LBXGL_Draw2DSeq_HandleInput_Select(BTGE_DrawSequence2D *seq)
{
	BTGE_DrawableObject2D *ecur;
	int i;

	ecur=seq->entity;
	
	if(seq->sel_entid)
	{
		while(ecur)
		{
			if(seq->sel_entid==ecur->entid)
				{ ecur=ecur->seqnext; break; }
			ecur=ecur->seqnext;
		}
	}
	
	while(ecur)
	{
//		LBXGL_Draw2D_SequenceUpdateEntity(seq, ecur->entid, frame);
//		i=LBXGL_BrushDraw2D_CheckPointObject(ecur,
//			vec2(lbxgl_state->mx, lbxgl_state->my));
		i=LBXGL_BrushDraw2D_CheckPointObject(ecur,
			LBXGL_Draw2DSeq_HandleInput_GetMouseRelPos(seq));
		if(i)
		{
			seq->sel_entid=ecur->entid;
			break;
		}
		ecur=ecur->seqnext;
	}
	
	if(!ecur)
		{ seq->sel_entid=0; }
	
	return(0);
}

vec2 LBXGL_Draw2DSeq_HandleInput_GetMouseRelPos(
	BTGE_DrawSequence2D *seq)
{
	vec2 org;

	org=vec2(lbxgl_state->mx, -lbxgl_state->my);
	return(org);
}

vec2 LBXGL_Draw2DSeq_HandleInput_GetMouseRelLPos(
	BTGE_DrawSequence2D *seq)
{
	vec2 org;

	org=vec2(lbxgl_state->lmx, -lbxgl_state->lmy);
	return(org);
}

void LBXGL_Draw2DSeq_SetSelectTexture(BTGE_DrawSequence2D *seq, char *tex)
{
	BTGE_DrawableObject2D *ent;
	int dx, dy;

	if(!seq->sel_entid)
		return;
	ent=LBXGL_Draw2D_SequenceLookupEntity(seq, seq->sel_entid);
	if(!ent)return;
	
	ent->imgname=dystrdup(tex);
	ent->tex=LBXGL_Texture_LoadImage(ent->imgname);
}

void LBXGL_Draw2DSeq_SetSelectSize(BTGE_DrawSequence2D *seq,
	float xs, float ys)
{
	BTGE_DrawableObject2D *ent;
	int dx, dy;

	if(!seq->sel_entid)
		return;
	ent=LBXGL_Draw2D_SequenceLookupEntity(seq, seq->sel_entid);
	if(!ent)return;
	
	ent->size=vec2(xs, ys);
//	ent->imgname=dystrdup(tex);
//	ent->tex=LBXGL_Texture_LoadImage(ent->imgname);
}

void LBXGL_Draw2DSeq_HandleInput_UpdateSelection(BTGE_DrawSequence2D *seq)
{
	BTGE_DrawableObject2D *ent;
	vec2 org, morg, lmorg, d0, d1;
	float a0, a1, da;
	int dx, dy;

	if(!seq->sel_entid)
		return;
	ent=LBXGL_Draw2D_SequenceLookupEntity(seq, seq->sel_entid);
	if(!ent)return;
	
	if((lbxgl_state->mb&1) && (lbxgl_state->lmb&1))
	{
		if(PDGL_KeyDown(K_CTRL))
		{
//			morg=vec2(lbxgl_state->mx, lbxgl_state->my);
//			lmorg=vec2(lbxgl_state->lmx, lbxgl_state->lmy);
			morg=LBXGL_Draw2DSeq_HandleInput_GetMouseRelPos(seq);
			lmorg=LBXGL_Draw2DSeq_HandleInput_GetMouseRelLPos(seq);
			org=LBXGL_BrushDraw2D_CalcObjectCenter(ent);
			d0=v2sub(morg, org);
			d1=v2sub(lmorg, org);
			a0=atan2(v2y(d0), v2x(d0));
			a1=atan2(v2y(d1), v2x(d1));
			da=a0-a1;

			ent->angle+=da*(180.0/M_PI);
			while(ent->angle<0)ent->angle+=360;
			while(ent->angle>=360)ent->angle-=360;
		}else
		{
//			dx=lbxgl_state->mx-lbxgl_state->lmx;
//			dy=lbxgl_state->my-lbxgl_state->lmy;
//			ent->org=v2add(ent->org, vec2(dx, dy));

			morg=LBXGL_Draw2DSeq_HandleInput_GetMouseRelPos(seq);
			lmorg=LBXGL_Draw2DSeq_HandleInput_GetMouseRelLPos(seq);
			ent->org=v2add(ent->org, v2sub(morg, lmorg));
		}
	}
}

int LBXGL_Draw2DSeq_HandleInput_Insert(BTGE_DrawSequence2D *seq)
{
	BTGE_DrawableObject2D *ent;
	BTGE_DrawSequence2D_FramePos *frpos;

	if(seq->mode==BTGE_DO2D_MODE_ANIMATE)
	{
		ent=LBXGL_Draw2D_SequenceLookupEntity(seq, seq->sel_entid);
		if(!ent)return(-1);

		frpos=LBXGL_Draw2D_SequenceInsertFramePos(seq,
			seq->cur_frame, seq->sel_entid);
		frpos->org=ent->org;
		return(0);
	}
	
	if(seq->mode==BTGE_DO2D_MODE_OBJECT)
	{
		ent=LBXGL_Draw2D_SequenceCreateEntity(seq);
		ent->org=LBXGL_Draw2DSeq_HandleInput_GetMouseRelPos(seq);
		ent->size=vec2(64, 64);

		ent->coffs=vec2(0, 0);
		ent->angle=0;
		ent->angvel=0;

		ent->imgname=dystrdup("images/mystery.png");
//		ent->imgname=dystrdup(name);
		ent->tex=LBXGL_Texture_LoadImage(ent->imgname);
	
		ent->Draw=LBXGL_BrushDraw2D_DrawImage2;
		ent->Update=LBXGL_BrushDraw2D_UpdateDefault;
	}
	
	return(0);
}

int LBXGL_Draw2DSeq_HandleInput_Delete(BTGE_DrawSequence2D *seq)
{
	BTGE_DrawableObject2D *ent;

	if(seq->mode==BTGE_DO2D_MODE_ANIMATE)
	{
		LBXGL_Draw2D_SequenceDeleteFramePos(seq,
			seq->cur_frame, seq->sel_entid);
		return(0);
	}
	
}

int LBXGL_Draw2DSeq_HandleInput(BTGE_DrawSequence2D *seq)
{
	char buf[64];
	float io[3], iv[4], fw[2], pt[4], tv[3], f, g, v;
	float im[16], im1[16], *imv;
	char *s;
	int i, j, k, l;
	unsigned short *kcur;

//	if(Con_IsDown() || main_loadsave || PDGLUI_HasFocus())
//		return;

	kcur=lbxgl_state->keys;
	while(*kcur)
	{
		if(PDGL_KeyDown(K_CTRL))
		{
			if(*kcur=='s')
			{
			}
		}

		if(*kcur=='B')
			{ seq->mode=BTGE_DO2D_MODE_BONES; }
		if(*kcur=='A')
			{ seq->mode=BTGE_DO2D_MODE_ANIMATE; }
		if(*kcur=='M')
			{ seq->mode=BTGE_DO2D_MODE_MESH; }
		if(*kcur=='S')
			{ seq->mode=BTGE_DO2D_MODE_SOLID; }
		if(*kcur=='O')
			{ seq->mode=BTGE_DO2D_MODE_OBJECT; }

		if(*kcur==K_ENTER)
		{
			LBXGL_Draw2DSeq_HandleInput_Select(seq);
		}

		if(*kcur==K_INS)
		{
			LBXGL_Draw2DSeq_HandleInput_Insert(seq);
		}

		if(*kcur==K_DEL)
		{
			LBXGL_Draw2DSeq_HandleInput_Delete(seq);
		}

//		if(((*kcur=='+') || (*kcur=='=')) &&
//			(seq->frame_time>=(1.0/15.0)))
		if((*kcur==')') &&
			(seq->frame_time>=(1.0/15.0)))
		{
			seq->mode=BTGE_DO2D_MODE_ANIMATE;

			seq->frame_time-=1.0/15.0;
			seq->cur_frame++;
			if(seq->n_frame)
				seq->cur_frame%=seq->n_frame;
				else seq->cur_frame=0;

			LBXGL_Draw2D_SequenceUpdateEntities(seq, seq->cur_frame);
		}

//		if(((*kcur=='-') || (*kcur=='_')) &&
//			(seq->frame_time>=(1.0/15.0)))
		if((*kcur=='(') &&
			(seq->frame_time>=(1.0/15.0)))
		{
			seq->mode=BTGE_DO2D_MODE_ANIMATE;

			seq->frame_time-=1.0/15.0;
			seq->cur_frame--;
			if(seq->n_frame)
			{
				if(seq->cur_frame<0)
					seq->cur_frame+=seq->n_frame;
				seq->cur_frame%=seq->n_frame;
			}else
			{
				seq->cur_frame=0;
			}

			LBXGL_Draw2D_SequenceUpdateEntities(seq, seq->cur_frame);
		}
		if(*kcur=='*')
		{
			seq->mode=BTGE_DO2D_MODE_ANIMATE;
			LBXGL_Draw2D_SequenceInsertFrame(seq, seq->cur_frame);
			LBXGL_Draw2D_SequenceUpdateEntities(seq, seq->cur_frame);
		}

		if((seq->n_frame>0) && (*kcur=='/'))
		{
			seq->mode=BTGE_DO2D_MODE_ANIMATE;
			LBXGL_Draw2D_SequenceDeleteFrame(seq, seq->cur_frame);
			LBXGL_Draw2D_SequenceUpdateEntities(seq, seq->cur_frame);
		}

		kcur++;
	}

	V3F_ZERO(iv);
	if(PDGL_KeyDown(K_SHIFT))
	{
		if(PDGL_KeyDown(K_LEFTARROW))
		{
//			V3F_ADDSCALE(iv, main_cam_rt,
//				-state->dt_f*main_speed, iv);
			iv[0]-=lbxgl_state->dt_f*100;
		}
		if(PDGL_KeyDown(K_RIGHTARROW))
		{
//			V3F_ADDSCALE(iv, main_cam_rt,
//				state->dt_f*main_speed, iv);
			iv[0]+=lbxgl_state->dt_f*100;
		}

		if(PDGL_KeyDown(K_UPARROW))
		{
//			V3F_ADDSCALE(iv, main_cam_up,
//				state->dt_f*main_speed, iv);
			iv[2]+=lbxgl_state->dt_f*100;
		}
		if(PDGL_KeyDown(K_DOWNARROW))
		{
//			V3F_ADDSCALE(iv, main_cam_up,
//				-state->dt_f*main_speed, iv);
			iv[2]-=lbxgl_state->dt_f*100;
		}
	}

	V3F_ADD(lbxgl_cam->org, iv, lbxgl_cam->org);

	V3F_ZERO(iv);

	LBXGL_Draw2DSeq_HandleInput_UpdateSelection(seq);

//	LBXGL_Draw2D_SequenceUpdateEntities(seq, seq->cur_frame);

	return(0);
}

void LBXGL_Draw2DSeq_DrawUI(BTGE_DrawSequence2D *seq)
{
	char buf[64];

	sprintf(buf, "%d/%d",
		(seq->cur_frame+1),
		seq->n_frame);
	GfxFont_DrawString(buf, -400, 300-12-8,
		8, 8,  0, 255, 0, 255);

	strcpy(buf, "");
	if(seq->mode==BTGE_DO2D_MODE_BONES)
		strcpy(buf, "Bones");
	if(seq->mode==BTGE_DO2D_MODE_ANIMATE)
		strcpy(buf, "Animate");
	if(seq->mode==BTGE_DO2D_MODE_MESH)
		strcpy(buf, "Mesh");
	if(seq->mode==BTGE_DO2D_MODE_SOLID)
		strcpy(buf, "Solid");
	if(seq->mode==BTGE_DO2D_MODE_OBJECT)
		strcpy(buf, "Object");
	GfxFont_DrawString(buf, 400-(16*8), 300-12,
		8, 8,  0, 255, 0, 255);
}

void LBXGL_Draw2DSeq_UpdateUI(BTGE_DrawSequence2D *seq, float dt)
{
	seq->frame_time+=dt;
	if(seq->frame_time>=(2.0/15.0))
		seq->frame_time=2.0/15.0;
}

LBXGL_API void LBXGL_Draw2DSeq_BeginEditNew(void)
{
	lbxgl_draw2d_editseq=LBXGL_Draw2D_AllocSequence();
}

LBXGL_API int ConCmd_draw2d_new(char **a)
{
	LBXGL_Draw2DSeq_BeginEditNew();
	return(0);
}

LBXGL_API int ConCmd_draw2d_settexture(char **a)
{
//	LBXGL_Draw2DSeq_BeginEditNew();
	LBXGL_Draw2DSeq_SetSelectTexture(
		lbxgl_draw2d_editseq, a[1]);
	return(0);
}

LBXGL_API int ConCmd_draw2d_setsize(char **a)
{
//	LBXGL_Draw2DSeq_BeginEditNew();
	LBXGL_Draw2DSeq_SetSelectSize(
		lbxgl_draw2d_editseq, atof(a[1]), atof(a[2]));
	return(0);
}

LBXGL_API void LBXGL_Draw2DSeq_Init(void)
{
	static int init=0;
	
	if(init)return;
	init=1;
}
