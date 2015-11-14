#include <lbxgl2.h>
#include <btgecl.h>

BTCL_World *btcl_world;
PDGLUI_InputState *btcl_state;
PDGLUI_Camera *btcl_cam;
LBXGL_BrushWorld *btcl_brushWorld;
BTCL_MessageHandler *btcl_handler;
float btcl_dt;

bool btcl_useshadowmap=0;

void BTCL_FlushEntityVis(BTCL_World *wrl, BTCL_Entity *ent)
{
	if(btcl_state->time_f<btcl_brushWorld->time_autovis)
		{ btcl_brushWorld->time_autovis=btcl_state->time_f-0.1; }
}

void BTCL_FlushEntity(BTCL_World *wrl, BTCL_Entity *ent)
{
	LBXGL_Light *dl;

	BTCL_FlushEntityVis(wrl, ent);

	if(ent->mdl || ent->wmdl ||
		(ent->effects&BT_EFF_BEAM))
	{
		LBXGL_BrushWorld_DeleteModelStateEntNum(
			btcl_brushWorld, ent->entnum);
		ent->mdl=NULL;
		ent->wmdl=NULL;
	}

	if(ent->effects&BT_EFF_GLOW_MASK)
	{
		dl=lbxWorldGetLight(btcl_brushWorld, ent->entnum|BT_LNUM_GLOW);
		dl->flags|=LBXGL_LFL_DISABLE;
		LBXGL_BrushWorld_FlushLightEnvMap(btcl_brushWorld, dl);
	}
}

void BTCL_PredictEntFrame(BTCL_World *wrl, BTCL_Entity *ent)
{
	float ia[3], io[3], iv[3], iv1[3], iv2[3];
	PDGLUI_InputState *state;
	PDGLUI_Camera *cam;
	float f, g, h, dt;

	dt=btclTime(wrl)-ent->time;
	if(dt<0)dt=0;

#if 1
	f=ent->time - ent->time2;
	g=ent->time2 - ent->time3;
	if(f<=0)f=1;
	if(g<=0)g=1;
	V3F_SUB(ent->org, ent->org2, iv1);
	V3F_SUB(ent->org2, ent->org3, iv2);
	V3F_SCALE(iv1, 1.0/f, iv1);
	V3F_SCALE(iv2, 1.0/g, iv2);

//	V3F_ADDSCALE(iv1, iv2, -dt, iv);

	V3F_COPY(iv1, iv);

//	h=1.0+dt;
//	V3F_SCALEADDSCALE(iv1, h, iv2, 1.0-h, iv);
#endif

#if 1
	if(ent->entnum==wrl->camnum)
	{
		state=PDGLUI_GetState();
		cam=PDGLUI_GetCamera();

//		if((fabs(cam->ivel[0])+fabs(cam->ivel[1]))<0.1)
//			{ iv[0]=0; iv[1]=0; }
		V3F_SCALE(cam->ivel, 1.0/state->dt_f, iv1);
//		iv[0]=iv1[0];
//		iv[1]=iv1[1];
		iv1[2]=iv[2];

//		iv[0]=min(iv[0], iv1[0]);
//		iv[1]=min(iv[1], iv1[1]);

		iv[0]=(fabs(iv[0])<fabs(iv1[0]))?iv[0]:iv1[0];
		iv[1]=(fabs(iv[1])<fabs(iv1[1]))?iv[1]:iv1[1];

//		V3F_ADDSCALE(cam->org, iv1, state->dt_f, io);
		V3F_ADDSCALE(cam->org, iv, state->dt_f, io);
		V3F_SUB(io, ent->vorg, io);
	}
#endif

	if((dt<0.25) && !(ent->effects2&BT_EF2_NOLERP))
	{
//		V3F_ADDSCALE(ent->org, ent->vel, dt, ent->p_org);
		V3F_ADDSCALE(ent->org, iv, dt, ent->p_org);
	}else
	{
		V3F_COPY(ent->org, ent->p_org);
	}

#if 0
	if(ent->entnum==wrl->camnum)
	{
		V3F_SCALEADDSCALE(ent->p_org, 0.5, io, 0.5, ent->p_org);
	}
#endif

	if(V4F_LEN(ent->rot)>0.8)
	{
		V4F_COPY(ent->rot, ent->p_rot);
	}else
	{
		V3F_COPY(ent->ang, ent->p_ang);
//		V3F_SCALE(ent->ang, (M_PI/180), ia);
		ia[0]=ent->ang[0]*(M_PI/180);
		ia[1]=ent->ang[1]*(M_PI/180);
		ia[2]=ent->ang[2]*(M_PI/180);
		QuatF_FromAngles(ia, ent->p_rot);
//		QuatF_FromAngles(ent->ang, ent->p_rot);
	}
}

void BTCL_PredictEntWFrame(BTCL_World *wrl, BTCL_Entity *ent)
{
	PDGLUI_Camera *cam;
	float ia[3], iv[3], iv1[3], iv2[3];
	float f, g, h, dt;

#if 0
	dt=btclTime(wrl)-ent->time;
	if(dt<0)dt=0;

//	f=ent->time - ent->time2;
//	if(f<=0)f=1;
//	V3F_SUB(ent->org, ent->org2, iv);
//	V3F_SCALE(iv, 1.0/f, iv);

#if 1
	f=ent->time - ent->time2;
	g=ent->time2 - ent->time3;
	if(f<=0)f=1;
	if(g<=0)g=1;
	V3F_SUB(ent->org, ent->org2, iv1);
	V3F_SUB(ent->org2, ent->org3, iv2);
	V3F_SCALE(iv1, 1.0/f, iv1);
	V3F_SCALE(iv2, 1.0/g, iv2);

//	V3F_ADDSCALE(iv1, iv2, -dt, iv);

	h=1.0+dt;
	V3F_SCALEADDSCALE(iv1, h, iv2, 1.0-h, iv);
#endif

	if(dt<0.25)
	{
//		V3F_ADDSCALE(ent->org, ent->vel, dt, ent->p_org);
		V3F_ADDSCALE(ent->org, iv, dt, ent->p_org);
	}else
	{
		V3F_COPY(ent->org, ent->p_org);
	}
#endif

//	BTCL_PredictEntFrame(wrl, ent);

	cam=PDGLUI_GetCamera();

//	ent->p_org[2]+=48;
	V3F_ADD(ent->p_org, ent->vorg, ent->p_org);

	if(V4F_LEN(ent->rot)>0.8)
	{
		V4F_COPY(ent->rot, ent->p_rot);
	}else
	{
		V3F_COPY(ent->ang, ent->p_ang);
//		V3F_SCALE(ent->ang, (M_PI/180), ia);
//		ia[0]=(-ent->ang[0]-90)*(M_PI/180);
		ia[0]=(-ent->ang[0])*(M_PI/180);
		ia[1]=ent->ang[1]*(M_PI/180);
		ia[2]=ent->ang[2]*(M_PI/180);
		QuatF_FromAngles(ia, ent->p_rot);
//		QuatF_FromAngles(ent->ang, ent->p_rot);
	}
}

void BTCL_SetupClientEntFrameMdl(BTCL_World *wrl, BTCL_Entity *ent)
{
	LBXGL_ModelState *mdl;
	void *ptr;
	float v0[4];
	int i, j;

	if(wrl->camnum<=0)
		return;

	if(!ent->mdl)
	{
		if(ent->lmdl)
		{
			BTCL_FlushEntityVis(wrl, ent);
			LBXGL_BrushWorld_DeleteModelStateEntNum(
				btcl_brushWorld, ent->entnum);
			ent->lmdl=NULL;
		}
		return;
	}
	
	i=atoi(ent->mdl);
	if(i>0)
	{
		mdl=LBXGL_BrushWorld_LookupModelStateMdlNum(btcl_brushWorld, i);
		if(!mdl)
		{
			BTCL_FlushEntityVis(wrl, ent);
			ent->flags|=BTCL_EFL_DIRTY;
		}
	}else
	{
		if(ent->mdl!=ent->lmdl)
		{
			BTCL_FlushEntityVis(wrl, ent);
			LBXGL_BrushWorld_DeleteModelStateEntNum(
				btcl_brushWorld, ent->entnum);
		}

		mdl=LBXGL_BrushWorld_LookupModelStateEntNum(
			btcl_brushWorld, ent->entnum);

		if(!mdl)
		{
			ent->lmdl=ent->mdl;
			
			if(!strcmp(ent->mdl, "$laser"))
			{
//				ptr=LBXGL_LaserEffect_Create(ent->org, ent->vel, ent->ang, )
			}else
			{
				BTCL_FlushEntityVis(wrl, ent);
				mdl=LBXGL_Mdl_LoadModelInstance(ent->mdl);				
				LBXGL_BrushWorld_AddModelStateEntNum(
					btcl_brushWorld, mdl, ent->entnum);

				if(!mdl)
					ent->mdl=NULL;
			}
		}
	}

	if(!mdl)return;

	BTCL_PredictEntFrame(wrl, ent);
//	V3F_COPY(ent->p_org, mdl->org);	

	mdl->org[0]=ent->p_org[0]-wrl->cam_reforg[0];
	mdl->org[1]=ent->p_org[1]-wrl->cam_reforg[1];
	mdl->org[2]=ent->p_org[2]-wrl->cam_reforg[2];

	QuatF_To3Matrix(ent->p_rot, mdl->rot);
//	Mat3F_Identity(mdl->rot);

	if(ent->anim)
			LBXGL_Mdl_SetAnimFrame(mdl, ent->anim, ent->frm);
	else	LBXGL_Mdl_SetFrame(mdl, ent->frm);

	if(ent->n_bones)
	{
		for(j=0; j<ent->n_bones; j++)
		{
//			if(!(ent->bone_fl[j]&1))
//				continue;
			LBXGL_Mdl_SetFrameBone(mdl, j,
				ent->bone_org+(j*3),
				ent->bone_rot+(j*4),
				ent->bone_fl[j]);
		}
	}

//	if(ent->effects&BT_EFF_GLOW_MASK)
	if(ent->effects&BT_EFF_NOSHADOW)
		mdl->flags|=LBXGL_TXFL_NOSHADOW;
}

void BTCL_SetupClientEntFrameWMdl(BTCL_World *wrl, BTCL_Entity *ent)
{
	LBXGL_ModelState *mdl;
	float v0[4];
	int i;

	if(wrl->camnum<=0)
		return;

	if(!ent->wmdl)
	{
		if(ent->lmdl)
		{
			BTCL_FlushEntityVis(wrl, ent);
			LBXGL_BrushWorld_DeleteModelStateEntNum(
				btcl_brushWorld, ent->entnum);
			ent->lmdl=NULL;
		}
		return;
	}
	
	i=atoi(ent->wmdl);
	if(i>0)
	{
		mdl=LBXGL_BrushWorld_LookupModelStateMdlNum(btcl_brushWorld, i);
	}else
	{
		if(ent->wmdl!=ent->lmdl)
		{
			printf("BTCL_SetupClientEntFrameWMdl: Model Changed\n");
			BTCL_FlushEntityVis(wrl, ent);
			LBXGL_BrushWorld_DeleteModelStateEntNum(
				btcl_brushWorld, ent->entnum);
		}

		mdl=LBXGL_BrushWorld_LookupModelStateEntNum(
			btcl_brushWorld, ent->entnum);

		if(!mdl)
		{
			printf("BTCL_SetupClientEntFrameWMdl: New wmdl instance\n");
		
			BTCL_FlushEntityVis(wrl, ent);
			ent->lmdl=ent->wmdl;
			mdl=LBXGL_Mdl_LoadModelInstance(ent->wmdl);
			LBXGL_BrushWorld_AddModelStateEntNum(
				btcl_brushWorld, mdl, ent->entnum);
		}
	}

	if(!mdl)return;
	BTCL_PredictEntWFrame(wrl, ent);
//	V3F_COPY(ent->p_org, mdl->org);	

	if(ent->effects2&BT_EF2_CAM_WEAPONFLIP)
		{ mdl->effects2|=BT_EF2_CAM_WEAPONFLIP; }

	mdl->effects2|=BT_EF2_VIEWMODEL;

	mdl->org[0]=ent->p_org[0]-wrl->cam_reforg[0];
	mdl->org[1]=ent->p_org[1]-wrl->cam_reforg[1];
	mdl->org[2]=ent->p_org[2]-wrl->cam_reforg[2];

	QuatF_To3Matrix(ent->p_rot, mdl->rot);

//	mdl->org[2]+=48;

	if(ent->wanim)
			LBXGL_Mdl_SetAnimFrame(mdl, ent->wanim, ent->wfrm);
	else	LBXGL_Mdl_SetFrame(mdl, ent->wfrm);
}

void BTCL_SetupClientEntFrameEffects(BTCL_World *wrl, BTCL_Entity *ent)
{
	float ivel[4], svel[4], torg[4];
	float tv0[4], tv1[4];
	LBXGL_ModelState *mdl;
	LBXGL_Light *dl;
	void *ptr;
	float f, g;
	int i, j, k;

	if(wrl->camnum<=0)
		return;

#if 0
//	if(ent->effects || ent->effects2)
	if(ent->effects&BT_EFF_TRAIL_MASK)
	{
		printf("Effects %d: %08X %08X\n",
			ent->entnum, ent->effects, ent->effects2);
	}
#endif

	mdl=NULL;

#if 1
	torg[0]=(ent->org[0]-wrl->cam_reforg[0])+(ent->mins[0]+ent->maxs[0])*0.5;
	torg[1]=(ent->org[1]-wrl->cam_reforg[1])+(ent->mins[1]+ent->maxs[1])*0.5;
	torg[2]=(ent->org[2]-wrl->cam_reforg[2])+(ent->mins[2]+ent->maxs[2])*0.5;
	V3F_SCALE(torg, 1.0/32, torg);
	V3F_SCALE(ent->vel, 1.0/32, ivel);

	if(ent->s_chan[BT_CHAN_VOICE]>0)
		{ PDGL_Sound_SetPlayingOrigin(ent->s_chan[BT_CHAN_VOICE],
			torg, ivel); }
#endif


#if 0
	for(i=0; i<8; i++)
	{
		if(ent->s_chan[i]>0)
		{
			PDGL_Sound_SetPlayingOrigin(ent->s_chan[i], torg, ivel);
		}
	}
#endif

	if(ent->effects&BT_EFF_BEAM)
	{
		f=(ent->effects&BT_EFF_GLOW_HIGH)?0.5:0;
		ivel[0]=(ent->effects&BT_EFF_GLOW_RED)?1:f;
		ivel[1]=(ent->effects&BT_EFF_GLOW_GREEN)?1:f;
		ivel[2]=(ent->effects&BT_EFF_GLOW_BLUE)?1:f;
		V3F_NORMALIZE(ivel, ivel);

		i=0;
		if(ent->effects&BT_EFF_GLOW_BRIGHT0)i|=1;
		if(ent->effects&BT_EFF_GLOW_BRIGHT1)i|=2;
		if(ent->effects&BT_EFF_GLOW_BRIGHT2)i|=4;
		if(ent->effects&BT_EFF_GLOW_BRIGHT3)i|=8;
		f=0.25*pow(1.5, i);

		j=0;
		if(ent->effects&BT_EFF_GLOW_PULSE0)j|=1;
		if(ent->effects&BT_EFF_GLOW_PULSE1)j|=2;
		if(ent->effects&BT_EFF_GLOW_PULSE2)j|=4;
		if(ent->effects&BT_EFF_GLOW_PULSE3)j|=8;

		i=0;
		if(ent->effects&BT_EFF_ALPHA_BIT0)i|=1;
		if(ent->effects&BT_EFF_ALPHA_BIT1)i|=2;
		if(ent->effects&BT_EFF_ALPHA_BIT2)i|=4;
		if(ent->effects&BT_EFF_ALPHA_BIT3)i|=8;
//		g=i?(0.25*i):1.0;
		g=i?(i/16.0):1.0;
		ivel[3]=g;

//		V3F_ADD(ent->org, ent->vel, svel);

		torg[0]=ent->org[0]-wrl->cam_reforg[0];
		torg[1]=ent->org[1]-wrl->cam_reforg[1];
		torg[2]=ent->org[2]-wrl->cam_reforg[2];
		V3F_ADD(torg, ent->vel, svel);

//		printf("Laser: (%.2f %.2f %.2f) -> (%.2f %.2f %.2f) %.2f %.2f\n",
//			ent->org[0], ent->org[1], ent->org[2],
//			svel[0], svel[1], svel[2], f, g);

		if(!mdl)mdl=LBXGL_BrushWorld_LookupModelStateEntNum(
			btcl_brushWorld, ent->entnum);

		if(mdl)
		{
			gcfree(mdl->mdl);
//			ptr=LBXGL_LaserEffect_Create(ent->org, svel, ivel, f);
			ptr=LBXGL_LaserEffect_Create(torg, svel, ivel, f);
			LBXGL_Mdl_SetModel(mdl, ptr);
			mdl->effects|=BT_EFF_BEAM;
			mdl->flags|=LBXGL_TXFL_ALPHA;
		}else
		{
//			V3F_COPY(ent->clr, tv0);
//			ptr=LBXGL_LaserEffect_Create(ent->org, svel, ivel, f);
			ptr=LBXGL_LaserEffect_Create(torg, svel, ivel, f);
			mdl=LBXGL_Mdl_WrapModelInstance(ptr);
			LBXGL_BrushWorld_AddModelStateEntNum(
				btcl_brushWorld, mdl, ent->entnum);
			mdl->effects|=BT_EFF_BEAM;
			mdl->flags|=LBXGL_TXFL_ALPHA;
		}

		return;
	}

	if(ent->effects&BT_EFF_ALPHA_MASK)
	{
		i=0;
		if(ent->effects&BT_EFF_ALPHA_BIT0)i|=1;
		if(ent->effects&BT_EFF_ALPHA_BIT1)i|=2;
		if(ent->effects&BT_EFF_ALPHA_BIT2)i|=4;
		if(ent->effects&BT_EFF_ALPHA_BIT3)i|=8;

		if(!mdl)mdl=LBXGL_BrushWorld_LookupModelStateEntNum(
			btcl_brushWorld, ent->entnum);

//		printf("Eff %08X\n", ent->effects);

		if(mdl)
		{
			mdl->flags|=LBXGL_TXFL_VALPHA;
			mdl->flags&=~LBXGL_TXFL_ALPHA_MASK;
			mdl->flags|=i<<LBXGL_TXFL_ALPHA_LSHIFT;
		}
	}

	if(ent->effects2&BT_EF2_SCALE_MASK)
	{
		if(!mdl)mdl=LBXGL_BrushWorld_LookupModelStateEntNum(
			btcl_brushWorld, ent->entnum);
		if(mdl)
		{
			i=(ent->effects2&BT_EF2_SCALE_MASK) >>
				BT_EF2_SCALE_LSHIFT;
			mdl->flags&=~LBXGL_TXFL_SCALE_MASK;
			mdl->flags|=i<<LBXGL_TXFL_SCALE_LSHIFT;
		}
	}

	if(ent->effects&BT_EFF_ROTATE)
	{
		f=btclClientTime()*(360/10);
		f=((int)f)%360;
		ent->ang[0]=0;
		ent->ang[1]=0;
		ent->ang[2]=f;
	}

	if((ent->effects&BT_EFF_TRAIL_MASK)==BT_EFF_BLOODTRAIL)
	{
		torg[0]=ent->org[0]-wrl->cam_reforg[0];
		torg[1]=ent->org[1]-wrl->cam_reforg[1];
		torg[2]=ent->org[2]-wrl->cam_reforg[2];

		V3F_SCALE(ent->vel, 0.05, ivel);
		V3F_SET(svel, 1, 1, 1);
		LBXGL_Particle_SpawnSpread(
			"generic_light_fall",
			1000*btclFrameTime(wrl),
			BT_RGB_RED_DARK,
			torg, ivel, svel);
	}

	if((ent->effects&BT_EFF_TRAIL_MASK)==BT_EFF_SMOKETRAIL)
	{
		torg[0]=ent->org[0]-wrl->cam_reforg[0];
		torg[1]=ent->org[1]-wrl->cam_reforg[1];
		torg[2]=ent->org[2]-wrl->cam_reforg[2];

		V3F_SCALE(ent->vel, 0.05, ivel);
		V3F_SET(svel, 1, 1, 1);
		LBXGL_Particle_SpawnSpread(
			"smoke_light",
			1000*btclFrameTime(wrl),
			BT_RGB_WHITE,
			torg, ivel, svel);
	}

	if((ent->effects&BT_EFF_TRAIL_MASK)==BT_EFF_FIRETRAIL)
	{
		torg[0]=ent->org[0]-wrl->cam_reforg[0];
		torg[1]=ent->org[1]-wrl->cam_reforg[1];
		torg[2]=ent->org[2]-wrl->cam_reforg[2];

		V3F_SCALE(ent->vel, 0.05, ivel);
		V3F_SET(svel, 1, 1, 1);
		LBXGL_Particle_SpawnSpread(
			"flame",
			1000*btclFrameTime(wrl),
			BT_RGB_WHITE,
			torg, ivel, svel);
	}

	if((ent->effects&BT_EFF_TRAIL_MASK)==BT_EFF_SPARKSTRAIL)
	{
//		printf("Effect: Sparks Trail, %f\n", 100*btclFrameTime(wrl));

		torg[0]=ent->org[0]-wrl->cam_reforg[0];
		torg[1]=ent->org[1]-wrl->cam_reforg[1];
		torg[2]=ent->org[2]-wrl->cam_reforg[2];

		V3F_SCALE(ent->vel, 0.05, ivel);
		V3F_SET(svel, 1, 1, 1);
		LBXGL_Particle_SpawnSpread(
			"generic_light_fall",
			100*btclFrameTime(wrl),
			BT_RGB_YELLOW,
			torg, ivel, svel);
	}

	if(ent->effects&BT_EFF_GLOW_MASK)
	{
		f=(ent->effects&BT_EFF_GLOW_HIGH)?0.5:0;
		ivel[0]=(ent->effects&BT_EFF_GLOW_RED)?1:f;
		ivel[1]=(ent->effects&BT_EFF_GLOW_GREEN)?1:f;
		ivel[2]=(ent->effects&BT_EFF_GLOW_BLUE)?1:f;
		V3F_NORMALIZE(ivel, ivel);

		i=0;
		if(ent->effects&BT_EFF_GLOW_BRIGHT0)i|=1;
		if(ent->effects&BT_EFF_GLOW_BRIGHT1)i|=2;
		if(ent->effects&BT_EFF_GLOW_BRIGHT2)i|=4;
		if(ent->effects&BT_EFF_GLOW_BRIGHT3)i|=8;
		f=50*pow(i+1, 1.25);

		j=0;
		if(ent->effects&BT_EFF_GLOW_PULSE0)j|=1;
		if(ent->effects&BT_EFF_GLOW_PULSE1)j|=2;
		if(ent->effects&BT_EFF_GLOW_PULSE2)j|=4;
		if(ent->effects&BT_EFF_GLOW_PULSE3)j|=8;

		dl=lbxWorldGetLight(btcl_brushWorld, ent->entnum|0x10000);
//		V3F_COPY(ent->org, dl->org);
//		V3F_COPY(ent->org, dl->rel_org);

		dl->org[0]=ent->org[0]-wrl->cam_reforg[0];
		dl->org[1]=ent->org[1]-wrl->cam_reforg[1];
		dl->org[2]=ent->org[2]-wrl->cam_reforg[2];
		V3F_COPY(dl->org, dl->rel_org);

		V3F_COPY(ivel, dl->clr);
		dl->val[0]=f; dl->val[1]=0;
		dl->style=j;
		dl->curval=f;

		dl->coeff[0]=0.50/0.58;
		dl->coeff[1]=0.50/0.58;
		dl->coeff[2]=1;

		dl->flags&=~LBXGL_LFL_DISABLE;	//make sure is turned on

		dl->flags&=~(LBXGL_LFL_ENVRENDER|
			LBXGL_LFL_SHADOWMAP);

#if 0
		if(btcl_useshadowmap)
		{
			dl->flags|=LBXGL_LFL_ENVRENDER|
				LBXGL_LFL_SHADOWMAP;
		}else
		{
			dl->flags&=~(LBXGL_LFL_ENVRENDER|
				LBXGL_LFL_SHADOWMAP);
		}
#endif

//		V3F_COPY(ent->dir, dl->dir);
//		V2F_COPY(ent->val, dl->val);
//		dl->angle=ent->angle;
//		dl->flags=ent->flags;
//		dl->style=ent->style;
	}

	if(ent->effects || ent->effects2)
	{
		if(!mdl)mdl=LBXGL_BrushWorld_LookupModelStateEntNum(
			btcl_brushWorld, ent->entnum);
		if(mdl)
		{
			mdl->effects=ent->effects;
			mdl->effects2=ent->effects2;
		}
	}
//	LBXGL_Particle_SpawnSpread(ty, cnt, clr, org, vel, svel);
}

float btclAngleMod(float v)
{
	while(v>=360)v-=360;
	while(v<0)v+=360;
	return(v);
}

float BTCL_AngleDist1(float a, float b)
{
	float f;

	f=fabs(btclAngleMod(a)-btclAngleMod(b));
	if(f>180)f=360-f;

//	while(f>=360)f-=360;
//	while(f<0)f+=360;

	return(f);
}

float BTCL_AngleDist(float *v0, float *v1)
{
	float f;
	f=	BTCL_AngleDist1(v0[0], v1[0])+
		BTCL_AngleDist1(v0[1], v1[1])+
		BTCL_AngleDist1(v0[2], v1[2]);
	return(f);
}

void BTCL_SetupClientEntFrame(BTCL_World *wrl, BTCL_Entity *ent)
{
	float tv0[3], tv1[3], tv2[3];
	int i;

	if(wrl->camnum<=0)
		return;

	if(!(ent->flags&BTCL_EFL_DIRTY))
	{
		if(ent->mdl)
		{
			i=atoi(ent->mdl);
//			if(i>0)
//				return;
		}
	}
	ent->flags&=~BTCL_EFL_DIRTY;
	
	if(ent->entnum==wrl->camnum)
	{
		btcl_brushWorld->cam_reforg[0]=wrl->cam_reforg[0];
		btcl_brushWorld->cam_reforg[1]=wrl->cam_reforg[1];
		btcl_brushWorld->cam_reforg[2]=wrl->cam_reforg[2];

		BTCL_PredictEntFrame(wrl, ent);
		
		V3F_ADD(ent->p_org, ent->vorg, tv0);
		
		tv0[0]=tv0[0]-wrl->cam_reforg[0];
		tv0[1]=tv0[1]-wrl->cam_reforg[1];
		tv0[2]=tv0[2]-wrl->cam_reforg[2];
		
		UI_Camera_SetOrgFV(tv0);

		V3F_COPY(ent->p_ang, tv1);
		tv1[2]=btclAngleMod(tv1[2]-180);
		UI_Camera_GetAngFV(tv2);
//		printf("Angle Check %f %f %f\n",
//			tv2[0]-tv1[0],
//			tv2[1]-tv1[1],
//			tv2[2]-tv1[2]);

//		i=V3F_DIST(tv1, tv2);
		i=BTCL_AngleDist(tv1, tv2);

//		if((wrl->viewflags&BT_EF2_CAM_FORCEANGLE) ||
//			(V3F_DIST(tv1, tv2)>25))
//		if((wrl->viewflags&BT_EF2_CAM_FORCEANGLE) || (i>25))
		if(wrl->viewflags&BT_EF2_CAM_FORCEANGLE)
		{
			UI_Camera_SetAngFV(tv1);
		}else
		{
			//force update angle
			V3F_COPY(tv2, ent->ang);
			ent->ang[2]=btclAngleMod(ent->ang[2]+180);
//			BTCL_PredictEntFrame(wrl, ent);
		}

		if(wrl->viewflags&BT_EF2_CAM_HFLIP)
			{ btcl_cam->flags|=PDGLUI_CAMFL_HFLIP; }
		else
			{ btcl_cam->flags&=~PDGLUI_CAMFL_HFLIP; }
		if(wrl->viewflags&BT_EF2_CAM_VFLIP)
			{ btcl_cam->flags|=PDGLUI_CAMFL_VFLIP; }
		else
			{ btcl_cam->flags&=~PDGLUI_CAMFL_VFLIP; }

//		UI_Camera_SetOrgFV(ent->p_org);
//		UI_Camera_SetAngFV(ent->p_ang);

		ent->effects2|=BT_EF2_VIEWMODEL;	//viewmodel hint
		BTCL_SetupClientEntFrameEffects(wrl, ent);
		BTCL_SetupClientEntFrameWMdl(wrl, ent);
		return;
	}
	
	BTCL_SetupClientEntFrameEffects(wrl, ent);
	BTCL_SetupClientEntFrameMdl(wrl, ent);
}

void BTCL_SetupClientLightFrame(BTCL_World *wrl, BTCL_Light *ent)
{
	float tv0[4], tv1[4], torg[4];
	LBXGL_ModelState *mdl;
	LBXGL_Light *dl;
	void *ptr;

	if(wrl->camnum<=0)
		return;

	if(ent->flags&LBXGL_LFL_LASER)
	{
		mdl=LBXGL_BrushWorld_LookupModelStateEntNum(
			btcl_brushWorld, ent->num);

		if(!mdl)
		{
			torg[0]=ent->org[0]-wrl->cam_reforg[0];
			torg[1]=ent->org[1]-wrl->cam_reforg[1];
			torg[2]=ent->org[2]-wrl->cam_reforg[2];

			V3F_COPY(ent->clr, tv0);
//			V3F_ADD(ent->org, ent->dir, tv1);
			V3F_ADD(torg, ent->dir, tv1);
			tv0[3]=ent->val[0];
//			ptr=LBXGL_LaserEffect_Create(ent->org, tv1, tv0, ent->val[1]);
			ptr=LBXGL_LaserEffect_Create(torg, tv1, tv0, ent->val[1]);
			mdl=LBXGL_Mdl_WrapModelInstance(ptr);
			LBXGL_BrushWorld_AddModelStateEntNum(
				btcl_brushWorld, mdl, ent->num);
		}
		return;
	}

	if((ent->flags&LBXGL_LFL_CHANGED) || wrl->cam_rebase)
	{
		if(ent->flags&LBXGL_LFL_CAMREL)
		{
			torg[0]=ent->org[0];
			torg[1]=ent->org[1];
			torg[2]=ent->org[2];
		}else
		{
			torg[0]=ent->org[0]-wrl->cam_reforg[0];
			torg[1]=ent->org[1]-wrl->cam_reforg[1];
			torg[2]=ent->org[2]-wrl->cam_reforg[2];
		}

		dl=lbxWorldGetLight(btcl_brushWorld, ent->num);
//		V3F_COPY(ent->org, dl->org);
		V3F_COPY(torg, dl->org);
		V3F_COPY(ent->dir, dl->dir);
		V3F_COPY(ent->clr, dl->clr);
		V2F_COPY(ent->val, dl->val);
		V2F_COPY(ent->coeff, dl->coeff);

		V3F_COPY(dl->org, dl->rel_org);
		V3F_COPY(dl->dir, dl->rel_dir);

		V3F_COPY(ent->mins, dl->mins);
		V3F_COPY(ent->maxs, dl->maxs);
		V3F_COPY(ent->boxval, dl->boxval);

		dl->angle=ent->angle;
		dl->flags=ent->flags;
		dl->style=ent->style;
		
		dl->texname=ent->texname;
		
		ent->flags&=~LBXGL_LFL_CHANGED;

//		if(btcl_useshadowmap)
		if(btcl_useshadowmap && (ent->flags&LBXGL_LFL_CAMREL))
		{
			dl->flags|=LBXGL_LFL_ENVRENDER|
				LBXGL_LFL_SHADOWMAP;
		}else
		{
			dl->flags&=~(LBXGL_LFL_ENVRENDER|
				LBXGL_LFL_SHADOWMAP);
		}

		LBXGL_BrushWorld_FlushLightEnvMap(btcl_brushWorld, dl);
	}
}

BTCL_API void BTCL_SetupClientFrame(BTCL_World *wrl)
{
	BTCL_Entity *cur;
	BTCL_Light *lcur;
	float f, g;

	if(wrl->camnum<=0)
		return;

	f=btclClientTime()+wrl->sv_dtadjust;
	wrl->sv_time=f;	//estimated time on server

	btcl_brushWorld->time_daynight=f;

	btcl_useshadowmap=btCvarGetf("r_shadowmap");

	cur=wrl->ents;
	while(cur)
	{
		BTCL_SetupClientEntFrame(wrl, cur);
		cur=cur->next;
	}

	lcur=wrl->lights;
	while(lcur)
	{
		BTCL_SetupClientLightFrame(wrl, lcur);
		lcur=lcur->next;
	}
	
	wrl->cam_rebase=0;
}

BTCL_API void BTCL_SetupClientCamera(BTCL_World *wrl)
{
	PDGLUI_InputState *state;
	PDGLUI_Camera *cam;

	state=PDGLUI_GetState();
	cam=PDGLUI_GetCamera();

	if(wrl->viewflags&BT_EF2_CAM_2D)
	{
		cam->mode=PDGLUI_CAM_SOLID2D;
		V3F_COPY(wrl->view_mins, cam->mins2d);
		V3F_COPY(wrl->view_maxs, cam->maxs2d);
	}else
	{
		cam->mode=PDGLUI_CAM_PERSP;
		cam->fov=wrl->view_fov;
	}
}
