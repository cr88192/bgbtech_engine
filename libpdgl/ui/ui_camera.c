/*
thought:
attempt a 'generic' way of handling input and manipulating the camera.
will also set up rendering in GL, and similar.
camera should be usable for both constrained and unconstrained movement.

orientation will be based on a matrix.

[Rx Ry Rz]
[Fx Fy Fz]
[Ux Uy Uz]

or: X right, Y forwards, Z up.

camera will have 2 major modes:
constrained, where orientation is done in the normal FPS style;
unconstrained, where orientation is done relative to the current orientation.

*/

#include <pdgl.h>
#include <pdglui.h>

//extern int pdglui_mx, pdglui_my, pdglui_mb;
//extern int pdglui_lmx, pdglui_lmy, pdglui_lmb;
//extern unsigned short *pdglui_keybuf;

//extern float pdglui_dt_f;
//extern int pdglui_xs, pdglui_ys;

//int pdglui_doshot;
//int pdglui_kill;

#if 0
float cam_org[3];
float cam_ang[3];
float cam_vel[3];
float cam_rot[9];

float cam_ivel[3];	//impulse velocity
float cam_iang[3];	//impulse angle
float cam_irot[9];	//impulse rotation

float cam_speed;	//camera movement speed

float cam_width;	//camera Ortho width
float cam_fov=90;	//camera fov (Persp)
float cam_width2=1;	//camera P2 width
float cam_fov2=90;	//camera fov (P2)

int cam_constrain=1;	//constrain orientation
int cam_mode=0;
int cam_ortho=0;
int cam_raw=1;		//raw control, 0=frontend specific

float cam_lorg[3];
float cam_lang[3];
float cam_lvel[3];
float cam_lrot[9];

float cam_rt[3];
float cam_fw[3];
float cam_up[3];

float cam_clip_lf[4];
float cam_clip_rt[4];
float cam_clip_up[4];
float cam_clip_dn[4];
float cam_clip_bk[4];

float cam_morg[3];
float cam_mdir[3];
float cam_mend[3];

float cam_lmorg[3];
float cam_lmdir[3];
float cam_lmend[3];
#endif

extern PDGLUI_InputState *pdglui_state;
PDGLUI_Camera *pdglui_cam;

PDGLUI_API void UI_Camera_GetOrgFV(float *org)
{
	UI_Camera_Init();
	V3F_COPY(pdglui_cam->org, org);
}

PDGLUI_API void UI_Camera_SetOrgFV(float *org)
{
	UI_Camera_Init();
	V3F_COPY(org, pdglui_cam->org);
}

PDGLUI_API void UI_Camera_GetAngFV(float *ang)
{
	UI_Camera_Init();
	V3F_COPY(pdglui_cam->ang, ang);
}

PDGLUI_API void UI_Camera_SetAngFV(float *ang)
{
	UI_Camera_Init();
	V3F_COPY(ang, pdglui_cam->ang);
}

PDGLUI_API int UI_Camera_GetKill()
	{ return(pdglui_state->kill); }
PDGLUI_API int UI_Camera_GetDoShot()
	{ return(pdglui_state->doshot); }
PDGLUI_API void UI_Camera_SetDoShot(int v)
	{ pdglui_state->doshot=v; }

PDGLUI_API void UI_Camera_BeginRecord(void)
	{ pdglui_state->doshot=2; }
PDGLUI_API void UI_Camera_EndRecord(void)
	{ pdglui_state->doshot=0; }
PDGLUI_API void UI_Camera_RecordDoShot(void)
	{ pdglui_state->doshot|=16; }
PDGLUI_API void UI_Camera_BeginRecordFixed(void)
	{ pdglui_state->doshot=2|32; }

PDGLUI_API void UI_Camera_RecordHideUI(bool state)
{
	if(state)
		{ pdglui_cam->flags|=PDGLUI_CAMFL_RECORDHIDEUI; }
	else
		{ pdglui_cam->flags&=~PDGLUI_CAMFL_RECORDHIDEUI; }
//	pdglui_state->doshot=0;
}

PDGLUI_API int UI_Camera_GetHideUI(void)
{
	if(((pdglui_state->doshot&15)==2) && 
			(pdglui_cam->flags&PDGLUI_CAMFL_RECORDHIDEUI))
		return(1);
	return(0);
}

PDGLUI_API PDGLUI_Camera *PDGLUI_GetCamera()
{
	if(!pdglui_cam)
	{
		pdglui_cam=malloc(sizeof(PDGLUI_Camera));
		memset(pdglui_cam, 0, sizeof(PDGLUI_Camera));
	}

	return(pdglui_cam);
}

PDGLUI_API void UI_Camera_Init()
{
	static int init=0;
	float f, g;

	if(init)return;
	init=1;

	if(!pdglui_cam)
	{
		pdglui_cam=malloc(sizeof(PDGLUI_Camera));
		memset(pdglui_cam, 0, sizeof(PDGLUI_Camera));
	}

	pdglui_cam->speed=12;
	pdglui_cam->constrain=1;
	pdglui_cam->mode=0;
	pdglui_cam->raw=1;
	Mat3F_Identity(pdglui_cam->rot);

	pdglui_cam->org[0]=0;
	pdglui_cam->org[1]=-10;
	pdglui_cam->org[2]=5;
	pdglui_cam->org[3]=-20;

	pdglui_cam->fov=90;	//camera fov (Persp)
	pdglui_cam->width2=1;	//camera P2 width
	pdglui_cam->fov2=90;	//camera fov (P2)

	f=btCvarGetf("r_fov");
	if(f>0.1)
	{
		pdglui_cam->fov=f;
		pdglui_cam->fov2=f;
	}

	pdglui_cam->constrain=1;	//constrain orientation
	pdglui_cam->mode=0;
	pdglui_cam->ortho=0;
	pdglui_cam->raw=1;		//raw control, 0=frontend specific

	pdglui_cam->mins2d[0]=-400;
	pdglui_cam->mins2d[1]=-300;
	pdglui_cam->mins2d[2]=-4094;
	pdglui_cam->maxs2d[0]=400;
	pdglui_cam->maxs2d[1]=300;
	pdglui_cam->maxs2d[2]=4094;

	pdglui_cam->qxform=malloc(8*20*sizeof(float));
	memset(pdglui_cam->qxform, 0, 8*20*sizeof(float));
	Mat4F_Identity(pdglui_cam->qxform);
	pdglui_cam->nqxform=1;
}

PDGLUI_API void UI_Camera_Update()
{
	char *str_ortho[]={
		"Persp", "Ortho", "XY", "XZ", "YZ",
		"Persp2", "4D", "4View", "Solid2D"};
	unsigned short *kcur;
	char *s;
	float iv[4], ia[8], ir[9], is[16], ivp[4], ia2[4];
	float vwx, vwy, vw, vh, vw2, vh2, izv;
	float f, g;
	int i, j;

	UI_Camera_Init();

	f=btCvarGetf("r_fov");
	if(f>0.1)
	{
		pdglui_cam->fov=f;
	}

	V4F_COPY(pdglui_cam->org, pdglui_cam->lorg);
	V4F_COPY(pdglui_cam->ang+0, pdglui_cam->lang+0);
	V4F_COPY(pdglui_cam->ang+4, pdglui_cam->lang+4);
	V4F_COPY(pdglui_cam->vel, pdglui_cam->lvel);

//	V3F_COPY(pdglui_cam->rot+0, pdglui_cam->rt);
//	V3F_COPY(pdglui_cam->rot+3, pdglui_cam->fw);
//	V3F_COPY(pdglui_cam->rot+6, pdglui_cam->up);

	V4F_ZERO(pdglui_cam->rt); V4F_ZERO(pdglui_cam->fw);
	V4F_ZERO(pdglui_cam->up); V4F_ZERO(pdglui_cam->zn);

	if(pdglui_cam->mode==PDGLUI_CAM_QUAT)
	{
//		V4F_COPY(pdglui_cam->qrot+ 0, pdglui_cam->rt);
//		V4F_COPY(pdglui_cam->qrot+ 4, pdglui_cam->fw);
//		V4F_COPY(pdglui_cam->qrot+ 8, pdglui_cam->up);
//		V4F_COPY(pdglui_cam->qrot+12, pdglui_cam->zn);

		Mat4F_Transpose(pdglui_cam->qrot, is);
		V4F_COPY(is+ 0, pdglui_cam->rt);
		V4F_COPY(is+ 4, pdglui_cam->fw);
		V4F_COPY(is+ 8, pdglui_cam->up);
		V4F_COPY(is+12, pdglui_cam->zn);
	}else
	{
		V3F_COPY(pdglui_cam->rot+0, pdglui_cam->rt);
		V3F_COPY(pdglui_cam->rot+3, pdglui_cam->fw);
		V3F_COPY(pdglui_cam->rot+6, pdglui_cam->up);
		V4F_SET(pdglui_cam->zn, 0, 0, 0, 1);
	}

	pdglui_cam->qmode=0;
	pdglui_cam->ortho=0;
	if((pdglui_cam->mode>=1) && (pdglui_cam->mode<=4))
		pdglui_cam->ortho=pdglui_cam->mode;

	if(pdglui_cam->mode==PDGLUI_CAM_SOLID2D)
//		pdglui_cam->ortho=PDGLUI_CAM_XY;
		pdglui_cam->ortho=PDGLUI_CAM_ORTHO;

//	if(UI_Camera_4ViewsP())
	if(UI_Camera_4ViewsP() &&
		!((pdglui_state->mb&2) || pdglui_cam->mgrab))
	{
		vw=pdglui_state->xs/2.0;
		vh=pdglui_state->ys/2.0;
		vw2=vw/2.0;
		vh2=vh/2.0;

		if((pdglui_state->mx<0) && (pdglui_state->my<0))
		{
			if(pdglui_cam->mode==PDGLUI_CAM_CAD4V)
				pdglui_cam->ortho=PDGLUI_CAM_XY;

			pdglui_cam->qmode=1;
			vwx=(pdglui_state->mx+vw2)/vw2;
			vwy=-((pdglui_state->my+vh2)/vw2);
		}
		if((pdglui_state->mx>=0) && (pdglui_state->my<0))
		{
			if(pdglui_cam->mode==PDGLUI_CAM_CAD4V)
				pdglui_cam->ortho=PDGLUI_CAM_XZ;

			pdglui_cam->qmode=2;
			vwx=(pdglui_state->mx-vw2)/vw2;
			vwy=-((pdglui_state->my+vh2)/vw2);
		}
		if((pdglui_state->mx<0) && (pdglui_state->my>=0))
		{
			if(pdglui_cam->mode==PDGLUI_CAM_CAD4V)
				pdglui_cam->ortho=PDGLUI_CAM_YZ;

			pdglui_cam->qmode=3;
			vwx=(pdglui_state->mx+vw2)/vw2;
			vwy=-((pdglui_state->my-vh2)/vw2);
		}
		if((pdglui_state->mx>=0) && (pdglui_state->my>=0))
		{
//			if(pdglui_cam->mode==PDGLUI_CAM_CAD4V)
//				pdglui_cam->ortho=PDGLUI_QMODE_XY;

			pdglui_cam->qmode=4;
			vwx=(pdglui_state->mx-vw2)/vw2;
			vwy=-((pdglui_state->my-vh2)/vw2);
		}
	}else
	{
		vw=pdglui_state->xs;
		vh=pdglui_state->ys;
		vw2=vw/2.0;
		vh2=vh/2.0;
		vwx=pdglui_state->mx/(pdglui_state->xs/2.0);
		vwy=-(pdglui_state->my/(pdglui_state->xs/2.0));
	}

//	switch(pdglui_cam->mode)
	switch(pdglui_cam->ortho)
	{
	case PDGLUI_CAM_XY:
		V4F_SET(pdglui_cam->rt, 1, 0, 0, 0);
		V4F_SET(pdglui_cam->up, 0, 1, 0, 0);
		V4F_SET(pdglui_cam->fw, 0, 0, 1, 0);
		V4F_SET(pdglui_cam->zn, 0, 0, 0, 1);
		break;
	case PDGLUI_CAM_XZ:
		V4F_SET(pdglui_cam->rt, 1, 0, 0, 0);
		V4F_SET(pdglui_cam->up, 0, 0, 1, 0);
		V4F_SET(pdglui_cam->fw, 0, 1, 0, 0);
		V4F_SET(pdglui_cam->zn, 0, 0, 0, 1);
		break;
	case PDGLUI_CAM_YZ:
		V4F_SET(pdglui_cam->rt, 0, 1, 0, 0);
		V4F_SET(pdglui_cam->up, 0, 0, 1, 0);
		V4F_SET(pdglui_cam->fw, 1, 0, 0, 0);
		V4F_SET(pdglui_cam->zn, 0, 0, 0, 1);
		break;
	default:
		break;
	}

	kcur=pdglui_state->keys;
	while(*kcur)
	{
		s=UI_BindingForKey2((*kcur)&(~0x8000));
		if(s)
		{
//			printf("binding for key %d -> %s\n", *kcur, s);
		
			//'+' events are treated as virtual keys
			if(*s=='+')
			{
				i=UI_LookupKey(s+1);
				if(i>0)
				{
					GfxDev_Key_Event(
						i, !((*kcur)&0x8000));
				}
				kcur++;
				continue;
			}
		
			//if a plain keypress, send to console
			if(!((*kcur)&0x8000))
			{
				Con_StuffCmd(s);
				kcur++;
				continue;
			}
		}

#if 1
		if(PDGL_KeyDown(K_SHIFT))
		{
#if 1
			if(*kcur==K_ESCAPE)
			{
				pdglui_state->kill=1;
				return;
			}
#endif

			if(*kcur==K_F2)
			{
				pdglui_cam->lockcfg=!pdglui_cam->lockcfg;
				Con_Printf("Camera Config Locked: %s\n",
					pdglui_cam->lockcfg?"true":"false");
				kcur++;
				continue;
			}
		}
#endif

		if(PDGL_KeyDown(K_SHIFT) && (pdglui_cam->mode==0))
		{
			if(pdglui_cam->lockcfg)
				{ kcur++; continue; }

			if(*kcur==K_MWHEELUP)
			{
//				pdglui_cam->fov+=5;
				pdglui_cam->fov*=1.1;
				if(pdglui_cam->fov>180)pdglui_cam->fov=180;
				Con_Printf("FOV %g degrees\n", pdglui_cam->fov);
			}
			if(*kcur==K_MWHEELDOWN)
			{
//				pdglui_cam->fov-=5;
				pdglui_cam->fov*=1.0/1.1;
				if(pdglui_cam->fov2<5)pdglui_cam->fov=5;
				Con_Printf("FOV %g degrees\n", pdglui_cam->fov);
			}

			kcur++;
			continue;
		}

		if(PDGL_KeyDown(K_SHIFT) && (pdglui_cam->mode==5))
		{
			if(pdglui_cam->lockcfg)
				{ kcur++; continue; }

			if(*kcur==K_MWHEELUP)
			{
//				pdglui_cam->fov2+=5;
				pdglui_cam->fov2*=1.1;
				if(pdglui_cam->fov2>180)pdglui_cam->fov2=180;
				Con_Printf("FOV %g degrees\n", pdglui_cam->fov2);
			}
			if(*kcur==K_MWHEELDOWN)
			{
//				pdglui_cam->fov2-=5;
				pdglui_cam->fov2*=1.0/1.1;
				if(pdglui_cam->fov2<0.1)pdglui_cam->fov2=0.1;
				Con_Printf("FOV %g degrees\n", pdglui_cam->fov2);
			}

			kcur++;
			continue;
		}

		if(PDGL_KeyDown(K_CTRL) && (pdglui_cam->mode==5))
		{
			if(pdglui_cam->lockcfg)
				{ kcur++; continue; }

			if(*kcur==K_MWHEELUP)
			{
				pdglui_cam->width2*=1.5;
				Con_Printf("View Width %g\n", pdglui_cam->width2);
			}
			if(*kcur==K_MWHEELDOWN)
			{
				pdglui_cam->width2*=1.0/1.5;
				Con_Printf("View Width %g\n", pdglui_cam->width2);
			}

			kcur++;
			continue;
		}

		if(PDGL_KeyDown(K_SHIFT) || PDGL_KeyDown(K_CTRL) ||
			PDGL_KeyDown(K_ALT))
		{
			kcur++;
			continue;
		}

#if 0
		if(*kcur==K_ESCAPE)
		{
			pdglui_state->kill=1;
			return;
		}
#endif

		if(*kcur==K_MWHEELUP)
		{
			if(pdglui_cam->lockcfg)
				{ kcur++; continue; }
			pdglui_cam->speed*=1.5;
			Con_Printf("Move Speed %g ft/s, %gmph\n",
				pdglui_cam->speed/12, (pdglui_cam->speed/12)*(3600.0/5280));
		}
		if(*kcur==K_MWHEELDOWN)
		{
			if(pdglui_cam->lockcfg)
				{ kcur++; continue; }
			pdglui_cam->speed*=1.0/1.5;
			Con_Printf("Move Speed %g ft/s, %gmph\n",
				pdglui_cam->speed/12, (pdglui_cam->speed/12)*(3600.0/5280));
		}

		if(*kcur==K_F2)
		{
			if(pdglui_cam->lockcfg)
				{ kcur++; continue; }
			pdglui_cam->raw=!pdglui_cam->raw;
			Con_Printf("Direct Camera Control: %s\n",
				pdglui_cam->raw?"true":"false");
		}

		if(*kcur==K_F3)
		{
			if(pdglui_cam->lockcfg)
				{ kcur++; continue; }
			pdglui_cam->constrain=!pdglui_cam->constrain;
			Con_Printf("Camera Constrained: %s\n",
				pdglui_cam->constrain?"true":"false");
		}

		if(*kcur==K_F4)
		{
			if(pdglui_cam->lockcfg)
				{ kcur++; continue; }
			pdglui_cam->mode=(pdglui_cam->mode+1)%
				PDGLUI_CAM_NMODE;
			Con_Printf("Camera Mode: %s\n",
				str_ortho[pdglui_cam->mode]);
		}

		if(*kcur==K_F5)
		{
			if(pdglui_cam->lockcfg)
				{ kcur++; continue; }
			pdglui_cam->mgrab=!pdglui_cam->mgrab;
			Con_Printf("Mouse Grab: %s\n",
				pdglui_cam->mgrab?"true":"false");
		}

		if(*kcur==K_F10)
			pdglui_state->doshot=1;
		if(*kcur==K_F11)
		{
			pdglui_state->doshot=(pdglui_state->doshot!=2)?2:0;
			Con_Printf("Recording: %s\n",
				pdglui_state->doshot?"true":"false");
		}

		if(*kcur==K_F12)
		{
//			pdglui_state->doshot=(pdglui_state->doshot!=(2|32))?(2|32):0;
			pdglui_state->doshot=(pdglui_state->doshot!=(2|32))?(2|32):32;
			Con_Printf("Fixed-Framerate Recording: %s\n",
				pdglui_state->doshot?"true":"false");
		}

		kcur++;
	}

//	pdglui_cam->ortho=0;
//	if((pdglui_cam->mode>=1) && (pdglui_cam->mode<=4))
//		pdglui_cam->ortho=pdglui_cam->mode;

	V4F_ZERO(iv);
	V4F_ZERO(ivp);

	V4F_COPY(pdglui_cam->ang+0, ia+0);
	V4F_COPY(pdglui_cam->ang+4, ia+4);
	izv=0;

	Mat3F_Copy(pdglui_cam->rot, ir);
	Mat4F_Copy(pdglui_cam->qrot, is);

	if(pdglui_cam->ortho)
	{
//		if(PDGL_KeyDown(K_LEFTARROW))
		if(PDGL_KeyDown(K_MOVELEFT))
		{
			V4F_ADDSCALE(iv, pdglui_cam->rt, -pdglui_state->dt_f*pdglui_cam->speed, iv);
		}

//		if(PDGL_KeyDown(K_RIGHTARROW))
		if(PDGL_KeyDown(K_MOVERIGHT))
		{
			V4F_ADDSCALE(iv, pdglui_cam->rt, pdglui_state->dt_f*pdglui_cam->speed, iv);
		}

//		if(PDGL_KeyDown(K_UPARROW))
		if(PDGL_KeyDown(K_FORWARD))
		{
			V4F_ADDSCALE(iv, pdglui_cam->up, pdglui_state->dt_f*pdglui_cam->speed, iv);
		}
//		if(PDGL_KeyDown(K_DOWNARROW))
		if(PDGL_KeyDown(K_BACKWARD))
		{
			V4F_ADDSCALE(iv, pdglui_cam->up, -pdglui_state->dt_f*pdglui_cam->speed, iv);
		}

		if(PDGL_KeyDown(K_HOME))
		{
//			i=(int)((pdglui_cam->ang[0]+22.5)/45);
//			pdglui_cam->ang[0]=(i%8)*45;
			i=(int)(((ia[0]+180)+22.5)/45);
			ia[0]=((i%8)*45)-180;

			i=(int)((ia[2]+22.5)/45);
			ia[2]=(i%8)*45;

			ia[1]=0;
			ia[3]=0; ia[4]=0; ia[5]=0;
		}
	}else if(pdglui_cam->raw && PDGL_KeyDown(K_CTRL))
	{
		if(PDGL_KeyDown(K_SHIFT))
		{
			if(PDGL_KeyDown(K_LEFTARROW))
				{ ia[5]+=90*pdglui_state->dt_f; }
			if(PDGL_KeyDown(K_RIGHTARROW))
				{ ia[5]-=90*pdglui_state->dt_f; }
			if(PDGL_KeyDown(K_UPARROW))
				{ ia[3]+=90*pdglui_state->dt_f; }
			if(PDGL_KeyDown(K_DOWNARROW))
				{ ia[3]-=90*pdglui_state->dt_f; }

			if(PDGL_KeyDown(K_HOME))
				{ ia[4]+=90*pdglui_state->dt_f; }
			if(PDGL_KeyDown(K_END))
				{ ia[4]-=90*pdglui_state->dt_f; }
		}else
		{
			if(PDGL_KeyDown(K_LEFTARROW))
				{ ia[2]+=90*pdglui_state->dt_f; }
			if(PDGL_KeyDown(K_RIGHTARROW))
				{ ia[2]-=90*pdglui_state->dt_f; }
			if(PDGL_KeyDown(K_UPARROW))
				{ ia[0]+=90*pdglui_state->dt_f; }
			if(PDGL_KeyDown(K_DOWNARROW))
				{ ia[0]-=90*pdglui_state->dt_f; }

			if(PDGL_KeyDown(K_HOME))
				{ ia[1]+=90*pdglui_state->dt_f; }
			if(PDGL_KeyDown(K_END))
				{ ia[1]-=90*pdglui_state->dt_f; }
		}

		if(ia[0]<-90)ia[0]=-90; if(ia[0]>=90)ia[0]=90;

		if(ia[1]<0)ia[1]+=360; if(ia[1]>=360)ia[1]-=360;
		if(ia[2]<0)ia[2]+=360; if(ia[2]>=360)ia[2]-=360;
		if(ia[3]<0)ia[3]+=360; if(ia[3]>=360)ia[3]-=360;
		if(ia[4]<0)ia[4]+=360; if(ia[4]>=360)ia[4]-=360;
		if(ia[5]<0)ia[5]+=360; if(ia[5]>=360)ia[5]-=360;

	}else if(pdglui_cam->raw && PDGL_KeyDown(K_SHIFT))
	{
		if(PDGL_KeyDown(K_LEFTARROW))
		{
			V4F_ADDSCALE(iv, pdglui_cam->rt, -pdglui_state->dt_f*pdglui_cam->speed, iv);
		}
		if(PDGL_KeyDown(K_RIGHTARROW))
		{
			V4F_ADDSCALE(iv, pdglui_cam->rt, pdglui_state->dt_f*pdglui_cam->speed, iv);
		}

		if(PDGL_KeyDown(K_UPARROW))
		{
			V4F_ADDSCALE(iv, pdglui_cam->up, pdglui_state->dt_f*pdglui_cam->speed, iv);
		}
		if(PDGL_KeyDown(K_DOWNARROW))
		{
			V4F_ADDSCALE(iv, pdglui_cam->up, -pdglui_state->dt_f*pdglui_cam->speed, iv);
		}

		V4F_COPY(iv, ivp);

		if(PDGL_KeyDown(K_PGUP))
		{
			V4F_ADDSCALE(iv, pdglui_cam->zn, pdglui_state->dt_f*pdglui_cam->speed, iv);
		}
		if(PDGL_KeyDown(K_PGDN))
		{
			V4F_ADDSCALE(iv, pdglui_cam->zn, -pdglui_state->dt_f*pdglui_cam->speed, iv);
		}

		if(PDGL_KeyDown(K_HOME))
		{
			i=(int)(((ia[0]+180)+22.5)/45);
			ia[0]=((i%8)*45)-180;

			i=(int)((ia[2]+22.5)/45);
			ia[2]=(i%8)*45;

			ia[1]=0;
			ia[3]=0; ia[4]=0; ia[5]=0;
		}
	}else
	{
		if(pdglui_cam->flags&PDGLUI_CAMFL_HFLIP)
		{
//			if(PDGL_KeyDown(K_LEFTARROW) || PDGL_KeyDown(K_MOVELEFT))
			if(PDGL_KeyDown(K_MOVELEFT))
			{
				V4F_ADDSCALE(iv, pdglui_cam->rt,
					pdglui_state->dt_f*pdglui_cam->speed, iv);
			}
//			if(PDGL_KeyDown(K_RIGHTARROW) || PDGL_KeyDown(K_MOVERIGHT))
			if(PDGL_KeyDown(K_MOVERIGHT))
			{
				V4F_ADDSCALE(iv, pdglui_cam->rt,
					-pdglui_state->dt_f*pdglui_cam->speed, iv);
			}
		}else
		{
//			if(PDGL_KeyDown(K_LEFTARROW) || PDGL_KeyDown(K_MOVELEFT))
			if(PDGL_KeyDown(K_MOVELEFT))
			{
				V4F_ADDSCALE(iv, pdglui_cam->rt,
					-pdglui_state->dt_f*pdglui_cam->speed, iv);
			}
//			if(PDGL_KeyDown(K_RIGHTARROW) || PDGL_KeyDown(K_MOVERIGHT))
			if(PDGL_KeyDown(K_MOVERIGHT))
			{
				V4F_ADDSCALE(iv, pdglui_cam->rt,
					pdglui_state->dt_f*pdglui_cam->speed, iv);
			}
		}

//		if(PDGL_KeyDown(K_UPARROW) || PDGL_KeyDown(K_FORWARD))
		if(PDGL_KeyDown(K_FORWARD))
		{
			V4F_ADDSCALE(iv, pdglui_cam->fw, pdglui_state->dt_f*pdglui_cam->speed, iv);
		}
//		if(PDGL_KeyDown(K_DOWNARROW) || PDGL_KeyDown(K_BACKWARD))
		if(PDGL_KeyDown(K_BACKWARD))
		{
			V4F_ADDSCALE(iv, pdglui_cam->fw, -pdglui_state->dt_f*pdglui_cam->speed, iv);
		}

		V4F_COPY(iv, ivp);

//		if(PDGL_KeyDown(K_HOME) || PDGL_KeyDown(K_MOVEUP))
		if(PDGL_KeyDown(K_MOVEUP))
		{
			V4F_ADDSCALE(iv, pdglui_cam->up, pdglui_state->dt_f*pdglui_cam->speed, iv);
			izv+=pdglui_state->dt_f*pdglui_cam->speed;
		}
//		if(PDGL_KeyDown(K_END) || PDGL_KeyDown(K_MOVEDOWN))
		if(PDGL_KeyDown(K_MOVEDOWN))
		{
			V4F_ADDSCALE(iv, pdglui_cam->up, -pdglui_state->dt_f*pdglui_cam->speed, iv);
			izv-=pdglui_state->dt_f*pdglui_cam->speed;
		}

#if 0
		if(PDGL_KeyDown(K_PGUP))
		{
			V4F_ADDSCALE(iv, pdglui_cam->zn, pdglui_state->dt_f*pdglui_cam->speed, iv);
		}
		if(PDGL_KeyDown(K_PGDN))
		{
			V4F_ADDSCALE(iv, pdglui_cam->zn, -pdglui_state->dt_f*pdglui_cam->speed, iv);
		}
#endif
	}

//	if((pdglui_state->mb&2) || pdglui_cam->mgrab)
	if(GfxDrv_WindowIsActiveP() &&
		((pdglui_state->lmb&2) || pdglui_cam->mgrab))
	{
		if(pdglui_cam->raw && PDGL_KeyDown(K_CTRL))
		{
			if(pdglui_cam->constrain)
			{
				ia[4]-=pdglui_state->my;
				ia[5]-=pdglui_state->mx;

				if(ia[4]<0)ia[4]+=360;
				if(ia[4]>=360)ia[4]-=360;

				if(ia[5]<0)ia[5]+=360;
				if(ia[5]>=360)ia[5]-=360;
			}
		}else if(pdglui_cam->raw && PDGL_KeyDown(K_SHIFT))
		{
//			if((pdglui_state->lmb&2) || pdglui_cam->mgrab)
			if(GfxDrv_WindowIsFullActiveP() &&
				((pdglui_state->lmb&2) || pdglui_cam->mgrab))
			{
				if(pdglui_cam->constrain)
				{
					ia[0]-=pdglui_state->my;
					ia[3]-=pdglui_state->mx;

					if(ia[0]<-90)ia[0]=-90;
					if(ia[0]>90)ia[0]=90;

					if(ia[3]<0)ia[3]+=360;
					if(ia[3]>=360)ia[3]-=360;
				}else
				{
					Mat3F_Rotate3Matrix(ir, pdglui_cam->zn,
						-pdglui_state->mx*(M_PI/180), ir);
					Mat3F_Rotate3Matrix(ir, pdglui_cam->rt,
						-pdglui_state->my*(M_PI/180), ir);

//					Mat4F_Rotate4Matrix(is, pdglui_cam->zn,
//						-pdglui_state->mx*(M_PI/180), is);
//					Mat4F_Rotate4Matrix(is, pdglui_cam->rt,
//						-pdglui_state->my*(M_PI/180), is);

					Mat4F_Rotate4MatrixXW(is, -pdglui_state->mx*(M_PI/180), is);
//					Mat4F_Rotate4MatrixYZ(is, -pdglui_state->my*(M_PI/180), is);
					Mat4F_Rotate4MatrixZW(is, -pdglui_state->my*(M_PI/180), is);
					Mat4F_Filter4Matrix(is, is);
				}
			}
		}else
		{
//			if((pdglui_state->lmb&2) || pdglui_cam->mgrab)
			if(GfxDrv_WindowIsFullActiveP() &&
				((pdglui_state->lmb&2) || pdglui_cam->mgrab))
			{
				if(pdglui_cam->constrain)
				{
					if(pdglui_cam->flags&PDGLUI_CAMFL_VFLIP)
						{ ia[0]+=2*pdglui_state->my; }

					ia[0]-=pdglui_state->my;
					ia[2]-=pdglui_state->mx;

//					if(ia[0]<-90)ia[0]=-90;
//					if(ia[0]>90)ia[0]=90;

					if(ia[0]<-89)ia[0]=-89;
					if(ia[0]>89)ia[0]=89;

					if(ia[2]<0)ia[2]+=360;
					if(ia[2]>=360)ia[2]-=360;
				}else
				{
					Mat3F_Rotate3Matrix(ir, pdglui_cam->up,
						-pdglui_state->mx*(M_PI/180), ir);
					Mat3F_Rotate3Matrix(ir, pdglui_cam->rt,
						-pdglui_state->my*(M_PI/180), ir);

//					Mat4F_Rotate4Matrix(is, pdglui_cam->up,
//						-pdglui_state->mx*(M_PI/180), is);
//					Mat4F_Rotate4Matrix(is, pdglui_cam->rt,
//						-pdglui_state->my*(M_PI/180), is);

					Mat4F_Rotate4MatrixXY(is, -pdglui_state->mx*(M_PI/180), is);
					Mat4F_Rotate4MatrixYZ(is, -pdglui_state->my*(M_PI/180), is);
					Mat4F_Filter4Matrix(is, is);
				}
			}
		}

		if(GfxDrv_WindowIsActiveP())
		{
			Mouse_SetPos(0, 0);
		}
	}

#ifdef PDGLUI_HASRIFT
	if(1)
	{
		PDGLUI_GetRiftDeltaOrientation(ia2);
		ia[0]+=ia2[0];
		ia[1]+=ia2[1];
		ia[2]+=ia2[2];
		
//		printf("Rift Angles: %f %f %f\n", ia2[0], ia2[1], ia2[2]);
	}
#endif

	if(pdglui_cam->flags&PDGLUI_CAMFL_HFLIP)
	{
		ia[2]=ia[2]-2*(ia[2]-pdglui_cam->ang[2]);
		while(ia[2]<0)ia[2]+=360;
		while(ia[2]>=360)ia[2]-=360;
	}

#if 0
	if(pdglui_cam->flags&PDGLUI_CAMFL_VFLIP)
	{
		ia[0]=ia[0]-2*(ia[0]-pdglui_cam->ang[0]);
//		while(ia[0]<0)ia[0]+=360;
//		while(ia[0]>=360)ia[0]-=360;
		if(ia[0]<-89)ia[0]=-89;
		if(ia[0]>89)ia[0]=89;
//		ia[0]=-ia[0];
	}
#endif

	if(pdglui_cam->constrain)
	{
		Mat3F_Identity(ir);
		Mat3F_Rotate3Matrix(ir, ir+6, ia[2]*(M_PI/180), ir);
		Mat3F_Rotate3Matrix(ir, ir+0, ia[0]*(M_PI/180), ir);
		Mat3F_Rotate3Matrix(ir, ir+3, ia[1]*(M_PI/180), ir);

		Mat4F_Identity(is);
		Mat4F_Rotate4MatrixAngles(is, ia, is);
	}else
	{
		ia[0]=asin(pdglui_cam->fw[2])*(180/M_PI);
		ia[1]=0;
		ia[2]=atan2(pdglui_cam->fw[0], pdglui_cam->fw[1])*(180/M_PI);
		ia[3]=0; ia[4]=0; ia[5]=0;

		if(ia[0]<-90)ia[0]=-90;
		if(ia[0]>90)ia[0]=90;
		if(ia[2]<0)ia[2]+=360;
	}

	V4F_COPY(iv, pdglui_cam->ivel);
	V4F_COPY(ivp, pdglui_cam->ipvel);

//	V3F_SUB(ia, pdglui_cam->ang, pdglui_cam->iang);
//	Mat3F_Mat3MultT(ir, pdglui_cam->rot, pdglui_cam->irot);
	Mat3F_Mat3TMult(pdglui_cam->rot, ir, pdglui_cam->irot);

	V4F_SUB(ia+0, pdglui_cam->ang+0, pdglui_cam->iang+0);
	V4F_SUB(ia+4, pdglui_cam->ang+4, pdglui_cam->iang+4);

	pdglui_cam->izvel=izv;

	if(pdglui_cam->raw)
	{
		V4F_ADD(pdglui_cam->org, iv, pdglui_cam->org);
		V4F_COPY(ia+0, pdglui_cam->ang+0);
		V4F_COPY(ia+4, pdglui_cam->ang+4);
		V4F_SCALE(iv, 1.0/pdglui_state->dt_f, pdglui_cam->vel);
		Mat3F_Copy(ir, pdglui_cam->rot);
		Mat4F_Copy(is, pdglui_cam->qrot);
	}


	V4F_COPY(pdglui_cam->morg, pdglui_cam->lmorg);
	V4F_COPY(pdglui_cam->mdir, pdglui_cam->lmdir);
	V4F_COPY(pdglui_cam->mend, pdglui_cam->lmend);

	if(pdglui_cam->ortho)
	{
#if 0
		iv[0]=(pdglui_state->mx/400.0)*pdglui_cam->rt[0]+
			(-pdglui_state->my/400.0)*pdglui_cam->up[0];
		iv[1]=(pdglui_state->mx/400.0)*pdglui_cam->rt[1]+
			(-pdglui_state->my/400.0)*pdglui_cam->up[1];
		iv[2]=(pdglui_state->mx/400.0)*pdglui_cam->rt[2]+
			(-pdglui_state->my/400.0)*pdglui_cam->up[2];
#endif
#if 1
		iv[0]=vwx*pdglui_cam->rt[0]+vwy*pdglui_cam->up[0];
		iv[1]=vwx*pdglui_cam->rt[1]+vwy*pdglui_cam->up[1];
		iv[2]=vwx*pdglui_cam->rt[2]+vwy*pdglui_cam->up[2];
#endif

		V3F_SCALE(iv, pdglui_cam->width, iv);
		V3F_ADD(pdglui_cam->org, iv, pdglui_cam->morg);

		V3F_COPY(pdglui_cam->fw, pdglui_cam->mdir);
		V3F_ADDSCALE(pdglui_cam->morg, pdglui_cam->mdir, 9999, pdglui_cam->mend);
		V3F_ADDSCALE(pdglui_cam->morg, pdglui_cam->mdir, -9999, pdglui_cam->morg);
	}else
	{
#if 0
		iv[0]=(pdglui_state->mx/400.0)*pdglui_cam->rt[0]+
			(-pdglui_state->my/400.0)*pdglui_cam->up[0];
		iv[1]=(pdglui_state->mx/400.0)*pdglui_cam->rt[1]+
			(-pdglui_state->my/400.0)*pdglui_cam->up[1];
		iv[2]=(pdglui_state->mx/400.0)*pdglui_cam->rt[2]+
			(-pdglui_state->my/400.0)*pdglui_cam->up[2];
#endif
#if 1
		iv[0]=vwx*pdglui_cam->rt[0]+vwy*pdglui_cam->up[0];
		iv[1]=vwx*pdglui_cam->rt[1]+vwy*pdglui_cam->up[1];
		iv[2]=vwx*pdglui_cam->rt[2]+vwy*pdglui_cam->up[2];
#endif
		V4F_ADD(pdglui_cam->fw, iv, pdglui_cam->mdir);
		V4F_ADD(pdglui_cam->org, pdglui_cam->mdir, pdglui_cam->morg);

		V3F_NORMALIZE(pdglui_cam->mdir, pdglui_cam->mdir);

		V4F_ADDSCALE(pdglui_cam->morg, pdglui_cam->mdir, 9999, pdglui_cam->mend);
	}

	if(pdglui_cam->mode==PDGLUI_CAM_QUAT)
	{
		V4F_COPY(pdglui_cam->qrot+ 0, pdglui_cam->rt);
		V4F_COPY(pdglui_cam->qrot+ 4, pdglui_cam->fw);
		V4F_COPY(pdglui_cam->qrot+ 8, pdglui_cam->up);
		V4F_COPY(pdglui_cam->qrot+12, pdglui_cam->zn);
	}else if(!pdglui_cam->ortho)
	{
		V4F_ZERO(pdglui_cam->rt);
		V4F_ZERO(pdglui_cam->fw);
		V4F_ZERO(pdglui_cam->up);
		V4F_ZERO(pdglui_cam->zn);

		V3F_COPY(pdglui_cam->rot+0, pdglui_cam->rt);
		V3F_COPY(pdglui_cam->rot+3, pdglui_cam->fw);
		V3F_COPY(pdglui_cam->rot+6, pdglui_cam->up);
		V4F_SET(pdglui_cam->zn, 0, 0, 0, 1);
	}
}

PDGLUI_API void UI_Camera_GetZNearFar(float *rznear, float *rzfar)
{
	*rznear=1.0/tan(90*(M_PI/360.0));
	*rzfar=100000;
}

PDGLUI_API void UI_Camera_SetupViewAtPersp3D(
	int xo, int yo, int xs, int ys, float aspect, float fov,
	float *vorg, float *vrot)
{
	float rot[16];
	float org[3], ang[3], tv[4];
	float f, g;

	V4F_ZERO(pdglui_cam->rt);
	V4F_ZERO(pdglui_cam->fw);
	V4F_ZERO(pdglui_cam->up);
	V4F_ZERO(pdglui_cam->zn);

	V3F_COPY(vrot+0, pdglui_cam->rt);
	V3F_COPY(vrot+3, pdglui_cam->fw);
	V3F_COPY(vrot+6, pdglui_cam->up);
	V4F_SET(pdglui_cam->zn, 0, 0, 0, 1);

	org[0]=0; org[1]=0; org[2]=0;
	ang[0]=90; ang[1]=0; ang[2]=0;

	Draw_SetPerspective2_3D(aspect, fov, 1.0,
		vorg, vrot, xo, yo, xs, ys);

	if(pdglui_cam->flags&PDGLUI_CAMFL_HFLIP)
	{
		glScalef(-1, 1, 1);
		glFrontFace(GL_CW);
	}

	V3F_SCALE(pdglui_cam->fw, -1, pdglui_cam->clip_bk);
	pdglui_cam->clip_bk[3]=V3F_DOT(pdglui_cam->org, pdglui_cam->clip_bk);

	f=cos(fov * M_PI/360.0);
	g=sin(fov * M_PI/360.0);

	V3F_SCALEADDSCALE(pdglui_cam->rt, -f, pdglui_cam->fw, -g, pdglui_cam->clip_lf);
	V3F_NORMALIZE(pdglui_cam->clip_lf, pdglui_cam->clip_lf);
	pdglui_cam->clip_lf[3]=V3F_DOT(pdglui_cam->org, pdglui_cam->clip_lf);

	V3F_SCALEADDSCALE(pdglui_cam->rt, f, pdglui_cam->fw, -g, pdglui_cam->clip_rt);
	V3F_NORMALIZE(pdglui_cam->clip_rt, pdglui_cam->clip_rt);
	pdglui_cam->clip_rt[3]=V3F_DOT(pdglui_cam->org, pdglui_cam->clip_rt);

	f=cos(aspect * fov * M_PI/360.0);
	g=sin(aspect * fov * M_PI/360.0);

	V3F_SCALEADDSCALE(pdglui_cam->up, f, pdglui_cam->fw, -g, pdglui_cam->clip_up);
	V3F_NORMALIZE(pdglui_cam->clip_up, pdglui_cam->clip_up);
	pdglui_cam->clip_up[3]=V3F_DOT(pdglui_cam->org, pdglui_cam->clip_up);

	V3F_SCALEADDSCALE(pdglui_cam->up, -f, pdglui_cam->fw, -g, pdglui_cam->clip_dn);
	V3F_NORMALIZE(pdglui_cam->clip_dn, pdglui_cam->clip_dn);
	pdglui_cam->clip_dn[3]=V3F_DOT(pdglui_cam->org, pdglui_cam->clip_dn);
}

PDGLUI_API void UI_Camera_SetupView3D(int xo, int yo, int xs, int ys)
{
	float rot[16];
	float org[3], ang[3], tv[4];
	float aspect;
	float f, g;

	UI_Camera_Init();

	pdglui_cam->v_xo=xo;
	pdglui_cam->v_yo=yo;
	pdglui_cam->v_xs=xs;
	pdglui_cam->v_ys=ys;

#if 1
	if(pdglui_cam->mode==PDGLUI_CAM_QUAT)
	{
		V4F_COPY(pdglui_cam->qrot+ 0, pdglui_cam->rt);
		V4F_COPY(pdglui_cam->qrot+ 4, pdglui_cam->fw);
		V4F_COPY(pdglui_cam->qrot+ 8, pdglui_cam->up);
		V4F_COPY(pdglui_cam->qrot+12, pdglui_cam->zn);
	}else if(!pdglui_cam->ortho)
	{
		V4F_ZERO(pdglui_cam->rt);
		V4F_ZERO(pdglui_cam->fw);
		V4F_ZERO(pdglui_cam->up);
		V4F_ZERO(pdglui_cam->zn);

		V3F_COPY(pdglui_cam->rot+0, pdglui_cam->rt);
		V3F_COPY(pdglui_cam->rot+3, pdglui_cam->fw);
		V3F_COPY(pdglui_cam->rot+6, pdglui_cam->up);
		V4F_SET(pdglui_cam->zn, 0, 0, 0, 1);
	}
#endif

	aspect=btCvarGetf("r_aspect");
	if(aspect<=0.1)
		{ aspect=4.0/3.0; }

	if(pdglui_cam->mode==PDGLUI_CAM_PERSP2)
	{
		
		Draw_SetPerspective2_3D(aspect, pdglui_cam->fov2, pdglui_cam->width2,
			pdglui_cam->org, pdglui_cam->rot, xo, yo, xs, ys);

		V3F_SCALE(pdglui_cam->fw, -1, pdglui_cam->clip_bk);
		pdglui_cam->clip_bk[3]=V3F_DOT(pdglui_cam->org, pdglui_cam->clip_bk);

		f=cos(pdglui_cam->fov2 * M_PI/360.0);
		g=sin(pdglui_cam->fov2 * M_PI/360.0);

		V3F_SCALEADDSCALE(pdglui_cam->rt, -f, pdglui_cam->fw, -g, pdglui_cam->clip_lf);
		V3F_NORMALIZE(pdglui_cam->clip_lf, pdglui_cam->clip_lf);
		pdglui_cam->clip_lf[3]=V3F_DOT(pdglui_cam->org, pdglui_cam->clip_lf);

		V3F_SCALEADDSCALE(pdglui_cam->rt, f, pdglui_cam->fw, -g, pdglui_cam->clip_rt);
		V3F_NORMALIZE(pdglui_cam->clip_rt, pdglui_cam->clip_rt);
		pdglui_cam->clip_rt[3]=V3F_DOT(pdglui_cam->org, pdglui_cam->clip_rt);

//		f=cos(aspect * pdglui_cam->fov2 * M_PI/360.0);
//		g=sin(aspect * pdglui_cam->fov2 * M_PI/360.0);
		f=cos(pdglui_cam->fov * M_PI/360.0);
		g=sin(pdglui_cam->fov * M_PI/360.0);

		V3F_SCALEADDSCALE(pdglui_cam->up, f, pdglui_cam->fw, -g, pdglui_cam->clip_up);
		V3F_NORMALIZE(pdglui_cam->clip_up, pdglui_cam->clip_up);
		pdglui_cam->clip_up[3]=V3F_DOT(pdglui_cam->org, pdglui_cam->clip_up);

		V3F_SCALEADDSCALE(pdglui_cam->up, -f, pdglui_cam->fw, -g, pdglui_cam->clip_dn);
		V3F_NORMALIZE(pdglui_cam->clip_dn, pdglui_cam->clip_dn);
		pdglui_cam->clip_dn[3]=V3F_DOT(pdglui_cam->org, pdglui_cam->clip_dn);

		return;
	}

	if(pdglui_cam->mode==PDGLUI_CAM_SOLID2D)
	{
		org[0]=0; org[1]=0; org[2]=0;
		ang[0]=90; ang[1]=0; ang[2]=0;

//		org[0]=(pdglui_cam->mins2d[0]+pdglui_cam->maxs2d[0])*0.5;
//		org[1]=(pdglui_cam->mins2d[1]+pdglui_cam->maxs2d[1])*0.5;
//		tv[0]=pdglui_cam->maxs2d[0]-org[0];
//		tv[1]=pdglui_cam->maxs2d[1]-org[1];

		Draw_SetSolid4_2D(
			pdglui_cam->mins2d, pdglui_cam->maxs2d,
			org, ang, xo, yo, xs, ys);

//		Mat3to4F_Copy(pdglui_cam->rot, rot);
		Mat3to4F_Transpose(pdglui_cam->rot, rot);
//		glMatrixMode(GL_PROJECTION);
		pdglProjectionMatrix();
//		glMatrixMode(GL_MODELVIEW);

		pdglMultMatrixf(rot);
		pdglTranslatef(
			-pdglui_cam->org[0],
			-pdglui_cam->org[1],
			-pdglui_cam->org[2]);

//		Draw_SetSolid2_2D(
//			tv[0]/tv[1], tv[0], org, ang, xo, yo, xs, ys);
//		glTranslatef(
//			-pdglui_cam->org[0],
//			-pdglui_cam->org[1],
//			-pdglui_cam->org[2]);

//		pdglMatrixMode(GL_MODELVIEW);
		pdglModelviewMatrix();
		pdglLoadIdentity();
		return;
	}


	if(pdglui_cam->ortho>1)
	{
		org[0]=0; org[1]=0; org[2]=0;

		switch(pdglui_cam->ortho)
		{
		case 2:		ang[0]=0; ang[1]=0; ang[2]=0; break;
		case 3:		ang[0]=90; ang[1]=0; ang[2]=0; break;
		case 4:		ang[0]=90; ang[1]=0; ang[2]=90; break;
		default: break;
		}

		pdglui_cam->width=2*pdglui_cam->speed;
		Draw_SetOrtho_3D(aspect, pdglui_cam->width,
			org, ang, xo, yo, xs, ys);
		pdglTranslatef(
			-pdglui_cam->org[0],
			-pdglui_cam->org[1],
			-pdglui_cam->org[2]);
		return;
	}

	org[0]=0; org[1]=0; org[2]=0;
	ang[0]=90; ang[1]=0; ang[2]=0;

	if(!pdglui_cam->ortho)
	{
		Draw_SetPerspective_3D(aspect, pdglui_cam->fov, 0,
			org, ang, xo, yo, xs, ys);

		if(pdglui_cam->flags&PDGLUI_CAMFL_HFLIP)
		{
			glMatrixMode(GL_PROJECTION);
			glScalef(-1, 1, 1);
			glFrontFace(GL_CW);
			glMatrixMode(GL_MODELVIEW);
		}
		if(pdglui_cam->flags&PDGLUI_CAMFL_VFLIP)
		{
			glMatrixMode(GL_PROJECTION);
			glScalef(1, 1, -1);
			glFrontFace(GL_CW);
			glMatrixMode(GL_MODELVIEW);
		}

		V3F_SCALE(pdglui_cam->fw, -1, pdglui_cam->clip_bk);
		pdglui_cam->clip_bk[3]=V3F_DOT(pdglui_cam->org, pdglui_cam->clip_bk);

//		f=cos(pdglui_cam->fov * M_PI/360.0);
//		g=sin(pdglui_cam->fov * M_PI/360.0);
//		f=cos(aspect * pdglui_cam->fov * M_PI/360.0);
//		g=sin(aspect * pdglui_cam->fov * M_PI/360.0);

		f=cos(1.25*aspect * pdglui_cam->fov * M_PI/360.0);
		g=sin(1.25*aspect * pdglui_cam->fov * M_PI/360.0);

		V3F_SCALEADDSCALE(pdglui_cam->rt, -f, pdglui_cam->fw, -g, pdglui_cam->clip_lf);
		V3F_NORMALIZE(pdglui_cam->clip_lf, pdglui_cam->clip_lf);
		pdglui_cam->clip_lf[3]=V3F_DOT(pdglui_cam->org, pdglui_cam->clip_lf);

		V3F_SCALEADDSCALE(pdglui_cam->rt, f, pdglui_cam->fw, -g, pdglui_cam->clip_rt);
		V3F_NORMALIZE(pdglui_cam->clip_rt, pdglui_cam->clip_rt);
		pdglui_cam->clip_rt[3]=V3F_DOT(pdglui_cam->org, pdglui_cam->clip_rt);

//		f=cos(aspect * pdglui_cam->fov * M_PI/360.0);
//		g=sin(aspect * pdglui_cam->fov * M_PI/360.0);
//		f=cos(pdglui_cam->fov * M_PI/360.0);
//		g=sin(pdglui_cam->fov * M_PI/360.0);
//		f=cos((1.0/aspect) * pdglui_cam->fov * M_PI/360.0);
//		g=sin((1.0/aspect) * pdglui_cam->fov * M_PI/360.0);
		f=cos(1.25 * (1.0/aspect) * pdglui_cam->fov * M_PI/360.0);
		g=sin(1.25 * (1.0/aspect) * pdglui_cam->fov * M_PI/360.0);

		V3F_SCALEADDSCALE(pdglui_cam->up, f, pdglui_cam->fw, -g, pdglui_cam->clip_up);
		V3F_NORMALIZE(pdglui_cam->clip_up, pdglui_cam->clip_up);
		pdglui_cam->clip_up[3]=V3F_DOT(pdglui_cam->org, pdglui_cam->clip_up);

		V3F_SCALEADDSCALE(pdglui_cam->up, -f, pdglui_cam->fw, -g, pdglui_cam->clip_dn);
		V3F_NORMALIZE(pdglui_cam->clip_dn, pdglui_cam->clip_dn);
		pdglui_cam->clip_dn[3]=V3F_DOT(pdglui_cam->org, pdglui_cam->clip_dn);
	}else
	{
		pdglui_cam->width=2*pdglui_cam->speed;
		Draw_SetOrtho_3D(aspect, pdglui_cam->width,
			org, ang, xo, yo, xs, ys);
	}

	if(pdglui_cam->mode==PDGLUI_CAM_QUAT)
	{
//		Draw_SetSolid3_2D(1000, 750, org, xo, yo, xs, ys);

//		glMatrixMode(GL_PROJECTION);

		Mat4F_Copy(pdglui_cam->qrot, pdglui_cam->qproj);
		UI_Camera_XFormMatNormal4D(
			pdglui_cam->org, pdglui_cam->qproj, tv);
		pdglui_cam->qproj[16]=-tv[0]; pdglui_cam->qproj[17]=-tv[1];
		pdglui_cam->qproj[18]=-tv[2]; pdglui_cam->qproj[19]=-tv[3];

//		pdglMatrixMode(GL_MODELVIEW);
		pdglModelviewMatrix();
		pdglLoadIdentity();
		return;
	}

//	Mat3to4F_Copy(pdglui_cam->rot, rot);
	Mat3to4F_Transpose(pdglui_cam->rot, rot);
//	glMatrixMode(GL_PROJECTION);
	pdglProjectionMatrix();
//	glMatrixMode(GL_MODELVIEW);

	pdglMultMatrixf(rot);

//	pdglTranslatef(
//		-pdglui_cam->org[0],
//		-pdglui_cam->org[1],
//		-pdglui_cam->org[2]);

	switch(pdglui_cam->qmode)
	{
	case PDGLUI_QMODE_LEFT:
		V3F_ADDSCALE(pdglui_cam->org, pdglui_cam->rt,
			-0.5*pdglui_cam->eyedist, pdglui_cam->v_org);
		pdglTranslatef(
			-pdglui_cam->v_org[0],
			-pdglui_cam->v_org[1],
			-pdglui_cam->v_org[2]);
		break;
	case PDGLUI_QMODE_RIGHT:
		V3F_ADDSCALE(pdglui_cam->org, pdglui_cam->rt,
			0.5*pdglui_cam->eyedist, pdglui_cam->v_org);
		pdglTranslatef(
			-pdglui_cam->v_org[0],
			-pdglui_cam->v_org[1],
			-pdglui_cam->v_org[2]);
		break;
	default:
		V3F_COPY(pdglui_cam->org, pdglui_cam->v_org);
		pdglTranslatef(
			-pdglui_cam->org[0],
			-pdglui_cam->org[1],
			-pdglui_cam->org[2]);
		break;
	}

//	pdglMatrixMode(GL_MODELVIEW);
	pdglModelviewMatrix();
	pdglLoadIdentity();
}


PDGLUI_API void UI_Camera_Setup3D()
{
	UI_Camera_Init();
	UI_Camera_SetupView3D(0, 0, pdglui_state->xs, pdglui_state->ys);
}

PDGLUI_API void UI_Camera_Setup2D()
{
	float f;
	UI_Camera_Init();
//	Draw_SetSolid2_2D(4.0/3.0, 400,
//		VEC3(0, 0, 0), VEC3(0, 0, 0), 0, 0,
//		pdglui_state->xs, pdglui_state->ys);

	f=((float)pdglui_state->xs)/pdglui_state->ys;
	Draw_SetSolid2_2D(f, pdglui_state->xs/2,
		VEC3(0, 0, 0), VEC3(0, 0, 0), 0, 0,
		pdglui_state->xs, pdglui_state->ys);
}


PDGLUI_API int UI_Camera_4ViewsP()
{
	if(pdglui_cam->mode==PDGLUI_CAM_QUAT)
		return(1);
	if(pdglui_cam->mode==PDGLUI_CAM_CAD4V)
		return(1);
	return(0);
}

PDGLUI_API int UI_Camera_2ViewsP()
{
	if(pdglui_cam->mode==PDGLUI_CAM_SPLIT3D)
		return(1);
	return(0);
}

PDGLUI_API void UI_Camera_Setup3D_A()
{
	float t[16], tv[4];
	int w, h;

	UI_Camera_Init();

	if(pdglui_cam->mode==PDGLUI_CAM_CAD4V)
	{
		pdglui_cam->qmode=PDGLUI_QMODE_XY;
		pdglui_cam->ortho=PDGLUI_CAM_XY;
		w=pdglui_state->xs/2; h=pdglui_state->ys/2;
		UI_Camera_SetupView3D(0, h, w, h);
		return;
	}

	if(pdglui_cam->mode==PDGLUI_CAM_SPLIT3D)
	{
		pdglui_cam->qmode=PDGLUI_QMODE_LEFT;
//		pdglui_cam->ortho=PDGLUI_CAM_XY;
		w=pdglui_state->xs/2; h=pdglui_state->ys;
		UI_Camera_SetupView3D(0, 0, w, h);
		return;
	}

	if(pdglui_cam->mode!=PDGLUI_CAM_QUAT)
	{
		pdglui_cam->qmode=PDGLUI_QMODE_XYZ;
		w=pdglui_state->xs; h=pdglui_state->ys;
		UI_Camera_SetupView3D(0, 0, w, h);
		return;
	}

	pdglui_cam->qmode=PDGLUI_QMODE_XYZ;
	w=pdglui_state->xs/2; h=pdglui_state->ys/2;
	UI_Camera_SetupView3D(0, 0, w, h);

	Mat4F_Rotate4MatrixXW(pdglui_cam->qrot, 25*(M_PI/180), t);
	Mat4F_Rotate4MatrixZW(t, -25*(M_PI/180), pdglui_cam->qproj);
	UI_Camera_XFormMatNormal4D(pdglui_cam->org, pdglui_cam->qproj, tv);
	pdglui_cam->qproj[16]=-tv[0]; pdglui_cam->qproj[17]=-tv[1];
	pdglui_cam->qproj[18]=-tv[2]; pdglui_cam->qproj[19]=-tv[3];
}

PDGLUI_API void UI_Camera_Setup3D_B()
{
	float t[16], tv[4];
	int w, h;

	UI_Camera_Init();

	if(pdglui_cam->mode==PDGLUI_CAM_CAD4V)
	{
		pdglui_cam->qmode=PDGLUI_QMODE_XZ;
		pdglui_cam->ortho=PDGLUI_CAM_XZ;
		w=pdglui_state->xs/2; h=pdglui_state->ys/2;
		UI_Camera_SetupView3D(w, h, w, h);
		return;
	}

	if(pdglui_cam->mode==PDGLUI_CAM_SPLIT3D)
	{
		pdglui_cam->qmode=PDGLUI_QMODE_RIGHT;
//		pdglui_cam->ortho=PDGLUI_CAM_XY;
		w=pdglui_state->xs/2; h=pdglui_state->ys;
		UI_Camera_SetupView3D(w, 0, w, h);
		return;
	}

	if(pdglui_cam->mode!=PDGLUI_CAM_QUAT)
		return;

	pdglui_cam->qmode=PDGLUI_QMODE_XYW;
	w=pdglui_state->xs/2; h=pdglui_state->ys/2;
	UI_Camera_SetupView3D(w, 0, w, h);

	Mat4F_Rotate4MatrixXW(pdglui_cam->qrot, -25*(M_PI/180), t);
	Mat4F_Rotate4MatrixZW(t, -25*(M_PI/180), pdglui_cam->qproj);
	UI_Camera_XFormMatNormal4D(pdglui_cam->org, pdglui_cam->qproj, tv);
	pdglui_cam->qproj[16]=-tv[0]; pdglui_cam->qproj[17]=-tv[1];
	pdglui_cam->qproj[18]=-tv[2]; pdglui_cam->qproj[19]=-tv[3];
}

PDGLUI_API void UI_Camera_Setup3D_C()
{
	float t[16], tv[4];
	int w, h;

	UI_Camera_Init();

	if(pdglui_cam->mode==PDGLUI_CAM_CAD4V)
	{
		pdglui_cam->qmode=PDGLUI_QMODE_YZ;
		pdglui_cam->ortho=PDGLUI_CAM_YZ;
		w=pdglui_state->xs/2; h=pdglui_state->ys/2;
		UI_Camera_SetupView3D(0, 0, w, h);
		return;
	}

	if(pdglui_cam->mode!=PDGLUI_CAM_QUAT)
		return;

	pdglui_cam->qmode=PDGLUI_QMODE_XZW;
	w=pdglui_state->xs/2; h=pdglui_state->ys/2;
	UI_Camera_SetupView3D(0, h, w, h);

	Mat4F_Rotate4MatrixXW(pdglui_cam->qrot, 25*(M_PI/180), t);
	Mat4F_Rotate4MatrixZW(t, 25*(M_PI/180), pdglui_cam->qproj);
	UI_Camera_XFormMatNormal4D(pdglui_cam->org, pdglui_cam->qproj, tv);
	pdglui_cam->qproj[16]=-tv[0]; pdglui_cam->qproj[17]=-tv[1];
	pdglui_cam->qproj[18]=-tv[2]; pdglui_cam->qproj[19]=-tv[3];
}

PDGLUI_API void UI_Camera_Setup3D_D()
{
	float t[16], tv[4];
	int w, h;

	UI_Camera_Init();

	if(pdglui_cam->mode==PDGLUI_CAM_CAD4V)
	{
		pdglui_cam->qmode=PDGLUI_QMODE_XYZ;
		pdglui_cam->ortho=PDGLUI_CAM_PERSP;
		w=pdglui_state->xs/2; h=pdglui_state->ys/2;
		UI_Camera_SetupView3D(w, 0, w, h);
		return;
	}

	if(pdglui_cam->mode!=PDGLUI_CAM_QUAT)
		return;

	pdglui_cam->qmode=PDGLUI_QMODE_YZW;
	w=pdglui_state->xs/2; h=pdglui_state->ys/2;
	UI_Camera_SetupView3D(w, h, w, h);

	Mat4F_Rotate4MatrixXW(pdglui_cam->qrot, -25*(M_PI/180), t);
	Mat4F_Rotate4MatrixZW(t, 25*(M_PI/180), pdglui_cam->qproj);
	UI_Camera_XFormMatNormal4D(pdglui_cam->org, pdglui_cam->qproj, tv);
	pdglui_cam->qproj[16]=-tv[0]; pdglui_cam->qproj[17]=-tv[1];
	pdglui_cam->qproj[18]=-tv[2]; pdglui_cam->qproj[19]=-tv[3];
}

PDGLUI_API void UI_Camera_Project4D(float *qv, float *v)
{
	float v0[4], v1[4];

	if(pdglui_cam->mode!=PDGLUI_CAM_QUAT)
	{
		v[0]=qv[0]; v[1]=qv[1]; v[2]=qv[2];
		return;
	}

//	V4F_SUB(qv, pdglui_cam->org, v0);
//	UI_Camera_XFormMatNormal4D(v0, pdglui_cam->qproj, v1);

	UI_Camera_XFormMat4D(qv, pdglui_cam->qproj, v1);

#if 0
	v1[0]=	v0[0]*pdglui_cam->qrot[ 0]+
		v0[1]*pdglui_cam->qrot[ 4]+
		v0[2]*pdglui_cam->qrot[ 8]+
		v0[3]*pdglui_cam->qrot[12];
	v1[1]=	v0[0]*pdglui_cam->qrot[ 1]+
		v0[1]*pdglui_cam->qrot[ 5]+
		v0[2]*pdglui_cam->qrot[ 9]+
		v0[3]*pdglui_cam->qrot[13];
	v1[2]=	v0[0]*pdglui_cam->qrot[ 2]+
		v0[1]*pdglui_cam->qrot[ 6]+
		v0[2]*pdglui_cam->qrot[10]+
		v0[3]*pdglui_cam->qrot[14];
	v1[3]=	v0[0]*pdglui_cam->qrot[ 3]+
		v0[1]*pdglui_cam->qrot[ 7]+
		v0[2]*pdglui_cam->qrot[11]+
		v0[3]*pdglui_cam->qrot[15];
#endif

#if 0
	v1[0]=	v0[0]*pdglui_cam->qrot[ 0]+
		v0[1]*pdglui_cam->qrot[ 1]+
		v0[2]*pdglui_cam->qrot[ 2]+
		v0[3]*pdglui_cam->qrot[ 3];
	v1[1]=	v0[0]*pdglui_cam->qrot[ 4]+
		v0[1]*pdglui_cam->qrot[ 5]+
		v0[2]*pdglui_cam->qrot[ 6]+
		v0[3]*pdglui_cam->qrot[ 7];
	v1[2]=	v0[0]*pdglui_cam->qrot[ 9]+
		v0[1]*pdglui_cam->qrot[ 9]+
		v0[2]*pdglui_cam->qrot[10]+
		v0[3]*pdglui_cam->qrot[11];
	v1[3]=	v0[0]*pdglui_cam->qrot[12]+
		v0[1]*pdglui_cam->qrot[13]+
		v0[2]*pdglui_cam->qrot[14]+
		v0[3]*pdglui_cam->qrot[15];
#endif

#if 0
	switch(pdglui_cam->qmode)
	{
	case PDGLUI_QMODE_XYZ:
		v0[0]=v1[0]; v0[1]=v1[1]; v0[2]=v1[2]; v0[3]=v1[3];
		break;
	case PDGLUI_QMODE_XYW:
		v0[0]=v1[0]; v0[1]=v1[1]; v0[2]=v1[3]; v0[3]=v1[2];
		break;
	case PDGLUI_QMODE_XZW:
		v0[0]=v1[0]; v0[1]=v1[2]; v0[2]=v1[3]; v0[3]=v1[1];
		break;
	case PDGLUI_QMODE_YZW:
		v0[0]=v1[1]; v0[1]=v1[2]; v0[2]=v1[3]; v0[3]=v1[0];
		break;
	}
	UI_Camera_Project4D_3D(v0, v);
#endif

	UI_Camera_Project4D_3D_2(v1, v);
}

PDGLUI_API void UI_Camera_ProjectNormal4D(float *qv, float *v)
{
	float v0[4], v1[4];

	if(pdglui_cam->mode!=PDGLUI_CAM_QUAT)
	{
		v[0]=qv[0]; v[1]=qv[1]; v[2]=qv[2];
		return;
	}

	V4F_COPY(qv, v0);

	UI_Camera_XFormMatNormal4D(v0, pdglui_cam->qproj, v1);

#if 0
	v1[0]=	v0[0]*pdglui_cam->qrot[ 0]+
		v0[1]*pdglui_cam->qrot[ 4]+
		v0[2]*pdglui_cam->qrot[ 8]+
		v0[3]*pdglui_cam->qrot[12];
	v1[1]=	v0[0]*pdglui_cam->qrot[ 1]+
		v0[1]*pdglui_cam->qrot[ 5]+
		v0[2]*pdglui_cam->qrot[ 9]+
		v0[3]*pdglui_cam->qrot[13];
	v1[2]=	v0[0]*pdglui_cam->qrot[ 2]+
		v0[1]*pdglui_cam->qrot[ 6]+
		v0[2]*pdglui_cam->qrot[10]+
		v0[3]*pdglui_cam->qrot[14];
	v1[3]=	v0[0]*pdglui_cam->qrot[ 3]+
		v0[1]*pdglui_cam->qrot[ 7]+
		v0[2]*pdglui_cam->qrot[11]+
		v0[3]*pdglui_cam->qrot[15];
#endif

#if 0
	switch(pdglui_cam->qmode)
	{
	case PDGLUI_QMODE_XYZ:
		v0[0]=v1[0]; v0[1]=v1[1]; v0[2]=v1[2]; v0[3]=v1[3];
		break;
	case PDGLUI_QMODE_XYW:
		v0[0]=v1[0]; v0[1]=v1[1]; v0[2]=v1[3]; v0[3]=v1[2];
		break;
	case PDGLUI_QMODE_XZW:
		v0[0]=v1[0]; v0[1]=v1[2]; v0[2]=v1[3]; v0[3]=v1[1];
		break;
	case PDGLUI_QMODE_YZW:
		v0[0]=v1[1]; v0[1]=v1[2]; v0[2]=v1[3]; v0[3]=v1[0];
		break;
	}

	UI_Camera_Project4D_3D(v0, v);
#endif

	UI_Camera_Project4D_3D_2(v1, v);
}


PDGLUI_API void UI_Camera_XFormMat4D(float *qv, float *mat, float *v)
{
	float v1[4];

	v1[0]=	qv[0]*mat[ 0] + qv[1]*mat[ 4] + qv[2]*mat[ 8] + qv[3]*mat[12];
	v1[1]=	qv[0]*mat[ 1] + qv[1]*mat[ 5] + qv[2]*mat[ 9] + qv[3]*mat[13];
	v1[2]=	qv[0]*mat[ 2] + qv[1]*mat[ 6] + qv[2]*mat[10] + qv[3]*mat[14];
	v1[3]=	qv[0]*mat[ 3] + qv[1]*mat[ 7] + qv[2]*mat[11] + qv[3]*mat[15];

	V4F_ADD(v1, mat+16, v);
}

PDGLUI_API void UI_Camera_XFormMatNormal4D(float *qv, float *mat, float *v)
{
	float v1[4];

	v1[0] = qv[0]*mat[ 0] + qv[1]*mat[ 4] + qv[2]*mat[ 8] + qv[3]*mat[12];
	v1[1] = qv[0]*mat[ 1] + qv[1]*mat[ 5] + qv[2]*mat[ 9] + qv[3]*mat[13];
	v1[2] = qv[0]*mat[ 2] + qv[1]*mat[ 6] + qv[2]*mat[10] + qv[3]*mat[14];
	v1[3] = qv[0]*mat[ 3] + qv[1]*mat[ 7] + qv[2]*mat[11] + qv[3]*mat[15];
	V4F_COPY(v1, v);
}

PDGLUI_API void UI_Camera_XForm4D(float *qv, float *v)
{
	float *mat;

	mat=pdglui_cam->qxform+
		(pdglui_cam->nqxform-1)*20;
	UI_Camera_XFormMat4D(qv, mat, v);
}

PDGLUI_API void UI_Camera_XFormNormal4D(float *qv, float *v)
{
	float *mat;

	mat=pdglui_cam->qxform+
		(pdglui_cam->nqxform-1)*20;
	UI_Camera_XFormMatNormal4D(qv, mat, v);
}

PDGLUI_API void UI_Camera_Project4D_3D(float *qv, float *v)
{
	float f;

//	f=fabs(qv[3]-pdglui_cam->org[3]);
//	f=1.0/sqrt(f+1);

	f=V4F_DOT(qv, pdglui_cam->zn)-
		V4F_DOT(pdglui_cam->org, pdglui_cam->zn);

//	if(f<1)f=1;

//	f=1.0/sqrt(f);
//	f=1.0/f;

//	V3F_SCALE(qv, f, v);

	V3F_ADDSCALE(qv, pdglui_cam->fw, f, v);
	v[3]=f;
}

PDGLUI_API void UI_Camera_Project4D_3D_2(float *qv, float *v)
{
	float f;

	f=qv[3];
	if(f<0)f=0;

	f=1.0/sqrt(f+1);
//	f=1.0/(f+1);

//	v[0]=qv[0]*f;
//	v[1]=qv[1]*pow(f, 0.75);
//	v[2]=qv[2]*f;
//	v[3]=qv[3];

//	f=V4F_LEN(qv);
//	if((qv[1]<0) || (qv[3]<0))f=-f;
//	v[1]=f;

	v[0]=qv[0];
	v[1]=qv[1]+0.001*qv[3];
	v[2]=qv[2];
	v[3]=qv[3];

	return;

//	f=sqrt((qv[1]*qv[1])+(qv[3]*qv[3]));
//	if(qv[1]<0)f=-1;
//	if(qv[3]<0)f=-1;

//	v[1]=qv[1]*(f/(fabs(qv[1])+1));

//	v[0]=qv[0];
//	v[1]=qv[1]+qv[3];
//	v[2]=qv[2];
//	v[3]=qv[3];

//	v[0]=1000*(qv[0]/f)+0.5;
//	v[1]=750*(qv[2]/f)+0.5;
//	v[2]=f;
//	v[3]=1;

	return;

	f=qv[3];
	if(f<1)f=1;

	f=1.0/sqrt(f);
//	f=1.0/f;

//	v[0]=qv[0]*f;
//	v[1]=qv[1];
//	v[2]=qv[2]*f;
//	v[3]=qv[3];

	V3F_SCALE(qv, f, v);
}

PDGLUI_API void UI_Camera_ProjectWorldToScreen(float *sv, float *dv)
{
	float tv0[3], tv1[3];
	float f, vw, vh;

	V3F_SUB(sv, pdglui_cam->org, tv0);
	tv1[0]=V3F_DOT(tv0, pdglui_cam->rt);
	tv1[1]=V3F_DOT(tv0, pdglui_cam->up);
	tv1[2]=V3F_DOT(tv0, pdglui_cam->fw);

	vw=(pdglui_state->xs)/2.0;
	vh=(pdglui_state->ys)/2.0;

	f=tv1[2];
	if(f<0)f=1;
	dv[0]=vw*tv1[0]/f;
	dv[1]=vh*tv1[1]/f;
}
