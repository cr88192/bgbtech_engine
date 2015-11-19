#include <lbxgl2.h>
#include <btgecl.h>

BTCL_World *btcl_world;
PDGLUI_InputState *btcl_state;
PDGLUI_Camera *btcl_cam;
LBXGL_BrushWorld *btcl_brushWorld;
BTCL_MessageHandler *btcl_handler;
float btcl_dt;

BTCL_API BTCL_World *BTCL_GetClientWorld()
{
	return(btcl_world);
}

BTCL_API void BTCL_DrawImageName(
	float ox, float oy, float xs, float ys, char *name)
{
	int tex;
	
	tex=LBXGL_Texture_LoadImage(name);
	BTCL_DrawImage(ox, oy, xs, ys, tex);
}

BTCL_API void BTCL_DrawImage(
	float ox, float oy, float xs, float ys, int tex)
{
	pdglColor4f(1, 1, 1, 1);
	pdglEnableTexture2D();

	LBXGL_Shader_Color4f(1, 1, 1, 1);

	LBXGL_Shader_BindTexture(tex);

	LBXGL_Shader_Begin(PDGL_POLYGON);

	LBXGL_Shader_TexCoord2f(0, 0);
	LBXGL_Shader_Vertex2f(ox, oy);

	LBXGL_Shader_TexCoord2f(1, 0);
	LBXGL_Shader_Vertex2f(ox+xs, oy);

	LBXGL_Shader_TexCoord2f(1, 1);
	LBXGL_Shader_Vertex2f(ox+xs, oy+ys);

	LBXGL_Shader_TexCoord2f(0, 1);
	LBXGL_Shader_Vertex2f(ox, oy+ys);

	LBXGL_Shader_End();
}

BTCL_API char *BTCL_GetInventoryItem(BTCL_World *wrl,
	int slot, int *rcnt)
{
	BTCL_Inventory *cur;
	char *s;
	int n, idx;

	idx=slot+wrl->inv_slot_offset;

	cur=wrl->inven;
	while(cur)
	{
		if(cur->slot==idx)
		{
			*rcnt=cur->count;
			return(cur->classname);
		}
		cur=cur->next;
	}

	s=BTGE_Voxel_GetNameForVoxelIndex(idx);
//	while(!s && (n<4096))n++;
	*rcnt=1;
	return(s);
}

BTCL_API void BTCL_DrawInventory(BTCL_World *wrl)
{
	char tb[256];
	char *s, *t;
	int tex, sz;
	int i, j, n, cnt;
	
	n=0;
	for(i=0; i<8; i++)
	{
		for(j=0; j<16; j++)
		{
#if 1
//			s=BTGE_Voxel_GetNameForVoxelIndex(i*16+j);
//			if(!s)continue;

//			s=BTCL_GetInventoryItem(wrl, (7-i)*16+j, &cnt);
			s=BTCL_GetInventoryItem(wrl, i*16+j, &cnt);
			if(!s)continue;

			sprintf(tb, "textures/voxel/%s_item", s);
			tex=LBXGL_Texture_LoadImage(tb);
			
			if(tex<=0)
			{
				sprintf(tb, "textures/voxel/%s", s);
				tex=LBXGL_Texture_LoadImage(tb);
			}

			if(tex<=0)
			{
				sprintf(tb, "textures/voxel/%s_top", s);
				tex=LBXGL_Texture_LoadImage(tb);
			}

#if 0
			if(tex<=0)
			{
				t=tb;
				while(*s)
				{
					if(*s)*t++=*s++;
					if(*s)*t++=*s++;
					if(*s)*t++=*s++;
					if(*s)*t++=*s++;
					if(*s)*t++='\n';
				}
				*t++=0;
				
				GfxFont_DrawString2(tb, (j-8)*44, ((7-i)-4)*44+32,
					8, 8,  255, 255, 255, 255);

				continue;
			}
#endif

			sz=32;
			if(i*16+j == wrl->inv_slot_focus)
				{ sz=36; }

			if(tex<=0)
			{
				tex=LBXGL_Texture_LoadImage("images/mystery");
			}

			BTCL_DrawImage((j-8)*44, ((7-i)-4)*44, sz, sz,  tex);
			if(cnt>1)
			{
				sprintf(tb, "%d", cnt);
				GfxFont_DrawString2(tb, (j-8)*44, ((7-i)-4)*44,
					8, 8,  255, 255, 255, 255);
			}
#endif

#if 0
			tex=-1;

			while((tex<=0) && (n<4096))
			{
				s=BTGE_Voxel_GetNameForVoxelIndex(n);
				while(!s && (n<4096))n++;

				sprintf(tb, "textures/voxel/%s_item", s);
				tex=LBXGL_Texture_LoadImage(tb);

				if(tex<=0)
				{
					sprintf(tb, "textures/voxel/%s", s);
					tex=LBXGL_Texture_LoadImage(tb);
				}

				if(tex<=0)
				{
					sprintf(tb, "textures/voxel/%s_top", s);
					tex=LBXGL_Texture_LoadImage(tb);
				}

				if(tex<=0)
				{
					n++;
					continue;
				}
			}

			BTCL_DrawImage((j-8)*44, (i-4)*44, 32, 32,  tex);
			n++;
#endif

#if 0
//			s=BTGE_Voxel_GetNameForVoxelIndex(i*16+j);
//			if(!s)continue;

			sprintf(tb, "textures/voxel/%s_item", s);
			tex=LBXGL_Texture_LoadImage(tb);
			
			if(tex<=0)
			{
				sprintf(tb, "textures/voxel/%s", s);
				tex=LBXGL_Texture_LoadImage(tb);
			}

			if(tex<=0)
			{
				sprintf(tb, "textures/voxel/%s_top", s);
				tex=LBXGL_Texture_LoadImage(tb);
			}
			
			if(tex<=0)
			{
				t=tb;
				while(*s)
				{
					if(*s)*t++=*s++;
					if(*s)*t++=*s++;
					if(*s)*t++=*s++;
					if(*s)*t++=*s++;
					if(*s)*t++='\n';
				}
				*t++=0;
				
				GfxFont_DrawString2(tb, (j-8)*44, (i-4)*44+32,
					8, 8,  255, 255, 255, 255);

				continue;
			}

			BTCL_DrawImage((j-8)*44, (i-4)*44, 32, 32,  tex);
			
//			BTCL_DrawImageName((j-8)*44, (i-4)*44, 32, 32, 
//				"textures/voxel/brick");
#endif
		}
	}

	BTCL_DrawImageName((16-8)*44, ((7-0)-4)*44, 32, 32, 
		"images/uparrow");

	BTCL_DrawImageName((16-8)*44, ((7-7)-4)*44, 32, 32, 
		"images/downarrow");

	LBXGL_Shader_BindTexture(-1);
}

BTCL_API int BTCL_HandleInputInventory(
	BTCL_World *wrl, PDGLUI_InputState *state)
{
	char *s;
	int mx, my, cnt;
	int i;

	for(i=0; state->keys[i]; i++)
	{
		if(state->keys[i]==K_TAB)
		{
			wrl->clientflags^=BT_CLFL_INVEN_OPEN;
		}
//		BGBDY_EmitChar(&ct, state->keys[i]);
	}

	if(!(wrl->clientflags&BT_CLFL_INVEN_OPEN))
		return(0);

	for(i=0; state->keys[i]; i++)
	{
		if(state->keys[i]==K_UPARROW)
		{
//			wrl->clientflags^=BT_CLFL_INVEN_OPEN;

			printf("BTCL_HandleInputInventory: Key UpArrow\n");

			wrl->inv_slot_offset-=16;
			if(wrl->inv_slot_offset<0)
				wrl->inv_slot_offset=0;
		}

		if(state->keys[i]==K_DOWNARROW)
		{
//			wrl->clientflags^=BT_CLFL_INVEN_OPEN;

			printf("BTCL_HandleInputInventory: Key DownArrow\n");

			wrl->inv_slot_offset+=16;
			if(wrl->inv_slot_offset>(4096-128))
				wrl->inv_slot_offset=(4096-128);
		}
	}

	mx=floor(state->mx/44.0)+8;
	my=floor(state->my/44.0)+4;

	i=my*16+mx;
	if((mx>=0) && (mx<16) && (my>=0) && (my<16))
	{
		wrl->inv_slot_focus=i;
		
		if((state->mb&1) && !(state->lmb&1))
		{
			s=BTCL_GetInventoryItem(wrl, i, &cnt);
			if(s)
			{
				printf("BTCL_HandleInputInventory: Click %s\n", s);
				BTCL_SendMessage(wrl, dylist2s("inv_click", dystring(s)));
			}
		}
	}
	
	if((mx==16) && (my==0))
	{
		if((state->mb&1) && !(state->lmb&1))
		{
			printf("BTCL_HandleInputInventory: Click UpArrow\n");

			wrl->inv_slot_offset-=16;
			if(wrl->inv_slot_offset<0)
				wrl->inv_slot_offset=0;
		}
	}

	if((mx==16) && (my==7))
	{
		if((state->mb&1) && !(state->lmb&1))
		{
			printf("BTCL_HandleInputInventory: Click DownArrow\n");

			wrl->inv_slot_offset+=16;
			if(wrl->inv_slot_offset>(4096-128))
				wrl->inv_slot_offset=(4096-128);
		}
	}
	
	return(1);
}

BTCL_API void BTCL_Draw2D(BTCL_World *wrl)
{
	char tb[256];
	float tv[3], torg[3];
	BTCL_Entity *cur;
	char *s;
	float cr, cg, cb;
	float f, g;
	int w, h, wxs, wys, wxs2, wys2;
	int i, j;

	wxs=btcl_state->xs;
	wys=btcl_state->ys;
	wxs2=wxs/2; wys2=wys/2;

	if(wrl->viewflags&BT_EF2_ISDEAD)
	{
		pdglDisable(GL_TEXTURE_2D);
		pdglColor4f(0.25, 0.0, 0.0, 0.5);
		Draw_Square(-wxs2, -wys2, wxs, wys);
		pdglEnable(GL_TEXTURE_2D);
	}

	if(wrl->flash_time>0)
	{
		pdglDisable(GL_TEXTURE_2D);
		f=wrl->flash_time*wrl->flash_scale;
		cr=(wrl->flash_color&0x0000FF)/((float)0x0000FF);
		cg=(wrl->flash_color&0x00FF00)/((float)0x00FF00);
		cb=(wrl->flash_color&0xFF0000)/((float)0xFF0000);

		pdglColor4f(cr, cg, cb, f);
		Draw_Square(-wxs2, -wys2, wxs, wys);
		wrl->flash_time-=btclFrameTime(wrl);
		pdglEnable(GL_TEXTURE_2D);
	}

	if(wrl->cprint_msg)
	{
		w=0; h=0; i=0;
		s=wrl->cprint_msg;
		while(*s)
		{
			if(*s=='\n')
			{
				s++;
				if(i>w)w=i;
				i=0; h++;
				continue;
			}
			if(*s=='\t')
			{
				s++; i=(i+8)&(~7);
				continue;
			}
			i++; s++;
			if(i>w)w=i;
		}

		GfxFont_DrawString2(wrl->cprint_msg, -w*6, -6+(h*6),
			12, 12,  255, 255, 255, 255);
	
//		i=-strlen(wrl->cprint_msg)*6;
//		i=-128;
//		GfxFont_DrawString(wrl->cprint_msg, i, -6,
//			12, 12,  255, 255, 255, 255);

		if(wrl->cprint_time<btclClientTime())
			wrl->cprint_msg=NULL;
	}

	if(wrl->cprint_boxmsg)
	{
		w=0; h=0; i=0;
		s=wrl->cprint_boxmsg;
		while(*s)
		{
			if(*s=='\n')
			{
				s++;
				if(i>w)w=i;
				i=0; h++;
				continue;
			}
			if(*s=='\t')
			{
				s++; i=(i+8)&(~7);
				continue;
			}
			i++; s++;
			if(i>w)w=i;
		}

//		Draw_FillSquareRGBA(-w*6-32, -6-(h*6)-32,
//			w*12+64, h*12+64,  0, 0, 0, 127);
//		GfxFont_DrawString2(wrl->cprint_boxmsg, -w*6, -6+(h*6),
//			12, 12,  255, 255, 255, 255);

		if(wrl->cprint_boximg)
		{
//			sprintf(buf, "%s, %d", s, world->brush_sel->idnum);
//			GfxFont_DrawString(buf,
//				(state->xs/2)-24*8, -(state->ys/2)+24*8,
//				8, 8,  0, 255, 0, 255);

//			sprintf(buf, "resource/textures/%s", s);
//			j=Tex_LoadFile(buf, NULL, NULL);

			j=LBXGL_Texture_LoadImage(wrl->cprint_boximg);

			pdglColor4f(1, 1, 1, 1);
//			pdglEnable(GL_TEXTURE_2D);
			pdglEnableTexture2D();
			pdglBindTexture(GL_TEXTURE_2D, j);

			pdglBegin(PDGL_POLYGON);
			pdglTexCoord2f(0, 0);
			pdglVertex2f(-wxs2,			-wys2*0.25);
			pdglTexCoord2f(1, 0);
			pdglVertex2f(-wxs2+16*8,	-wys2*0.25);
			pdglTexCoord2f(1, 1);
			pdglVertex2f(-wxs2+16*8,	-wys2*0.25+16*8);
			pdglTexCoord2f(0, 1);
			pdglVertex2f(-wxs2,			-wys2*0.25+16*8);
			pdglEnd();
		}

		if(wrl->cprint_boxname)
		{
			i=16;
			Draw_FillSquareRGBA(-wxs2, -wys2*0.25,
				i*16, 16,  0, 0, 0, 192);

			pdglEnable(GL_TEXTURE_2D);
			GfxFont_DrawString2(wrl->cprint_boxname,
				-wxs2, -(wys2*0.25),
				16, 16,  255, 255, 255, 255);
		}

		Draw_FillSquareRGBA(-wxs2, -wys2,
			wxs, wys*0.375,  0, 0, 0, 192);
		pdglEnable(GL_TEXTURE_2D);
		GfxFont_DrawString2(wrl->cprint_boxmsg,
			-wxs2, -(wys2*0.25)-16,
			16, 16,  255, 255, 255, 255);
	}

	if(wrl->maxHP>0)
	{
		Draw_FillSquareRGBA(wxs2-10*16, wys2-3*16,
			10*16, 3*16,  0, 0, 0, 128);

		sprintf(tb, "HP=%d/%d\nMP=%d/%d\nAP=%d/%d",
			(int)(wrl->curHP), (int)(wrl->maxHP),
			(int)(wrl->curMP), (int)(wrl->maxMP),
			(int)(wrl->curAP), (int)(wrl->maxAP));
		GfxFont_DrawString2(tb, wxs2-10*16, wys2-1*16,
			16, 16,  255, 255, 0, 255);
	}

	if(gcCheckMemoryLow())
	{
		sprintf(tb, "RAM(%d)",
			gcCheckMemoryLow());
		GfxFont_DrawString2(tb, wxs2-16*16, wys2-1*16,
			16, 16,  255, 0, 0, 255);
	}

	i=ConCmds_CvarGetNum("cl_showentids");
	if(i>0)
	{
		cur=wrl->ents;
		while(cur)
		{
//			GfxFont_DrawString(wrl->cprint_msg, i, -6,
//				12, 12,  255, 255, 255, 255);

			torg[0]=cur->org[0]-wrl->cam_reforg[0];
			torg[1]=cur->org[1]-wrl->cam_reforg[1];
			torg[2]=cur->org[2]-wrl->cam_reforg[2];

			sprintf(tb, "%d", cur->entnum);
//			UI_Camera_ProjectWorldToScreen(cur->org, tv);
			UI_Camera_ProjectWorldToScreen(torg, tv);
			GfxFont_DrawString(tb, tv[0], tv[1],
				8, 8,  255, 255, 255, 255);

//			if(cur->entnum==entnum)
//				return(cur);
			cur=cur->next;
		}
	}

	if(wrl->cprint_eval_val)
	{
//		btScriptEvalFromServer(wrl->cprint_eval);
		BSVM_CallFuncObj(NULL, wrl->cprint_eval_val, NULL);
	}

	if(wrl->clientflags&BT_CLFL_INVEN_OPEN)
	{
		BTCL_DrawInventory(wrl);
	}

	bsvmCallName0("BTCL_ClientStuff_Draw");

	BTCL_ScriptDrawUI();

	BTCL_DrawGUI(wrl);
//	return(NULL);
}

BTCL_API int BTCL_HandleInput(BTCL_World *wrl)
{
	PDGLUI_InputState *state;
	PDGLUI_Camera *cam;
	int i;

	state=PDGLUI_GetState();
	cam=PDGLUI_GetCamera();
	
	btcl_state=state;
	btcl_cam=cam;

	bsvmCallName2("BTCL_ClientStuff_HandleInput", (dyt)wrl, (dyt)btcl_state);

	i=BTCLUI_HandleInput(wrl, btcl_state);
	if(i>0)return(1);
	
	if(wrl->viewflags&BT_EF2_CAM_GUIMODE)
		return(1);

	i=BTCL_HandleInputInventory(wrl, btcl_state);
	if(i>0)return(1);

	return(0);
}
