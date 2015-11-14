#include <pdgl.h>
#include <pdglui.h>

extern PDGLUI_InputState *pdglui_state;
extern PDGLUI_Camera *pdglui_cam;

PDGLUI_ProgramInfo *ui_metaui_proglist[256];

bool ui_metaui_active;
int ui_metaui_select;

PDGLUI_API void UI_MetaUI_SetActive(bool active)
{
	ui_metaui_active=active;
}

PDGLUI_API bool UI_MetaUI_IsActive(void)
{
	return(ui_metaui_active);
}

PDGLUI_API PDGLUI_ProgramInfo *UI_MetaUI_CreateProgramInfo(char *title)
{
	PDGLUI_ProgramInfo *pcur;
	int i, j, k;
	
	pcur=gctalloc("pdglui_programinfo_t", sizeof(PDGLUI_ProgramInfo));
	pcur->title=dystrdup(title);
	
	for(i=0; i<256; i++)
	{
		if(!ui_metaui_proglist[i])
		{
			ui_metaui_proglist[i]=pcur;
			break;
		}
	}
	
	return(pcur);
}

PDGLUI_API void UI_MetaUI_DestroyProgramInfo(PDGLUI_ProgramInfo *prog)
{
	int i, j;
	
	j=0;
	for(i=0; i<256; i++)
	{
		if(!ui_metaui_proglist[i])
			continue;
		if(ui_metaui_proglist[i]==prog)
			continue;
		ui_metaui_proglist[j++]=ui_metaui_proglist[i];
	}
	for(; j<256; j++)
		{ ui_metaui_proglist[j]=NULL; }
}

PDGLUI_API void UI_MetaUI_Draw()
{
	PDGLUI_ProgramInfo *pcur;
	char *s;
	int xs, ys, xs2, ys2;
	int i, j, k;
	
	if(!ui_metaui_active)
		return;
	
	xs=pdglui_state->xs;
	ys=pdglui_state->ys;
	xs2=(pdglui_state->xs/2);
	ys2=(pdglui_state->ys/2);
	
//	glDisable(GL_TEXTURE_2D);
	pdglDisableTexture2D();

	pdglBegin(PDGL_QUADS);

//	pdglColor4f(0, 0, 1, 1);
//	pdglVertex2f(-400, 300-16);
//	pdglVertex2f(-400, 300-24);
//	pdglVertex2f(200, 300-24);
//	pdglVertex2f(200, 300-16);

//	pdglColor4f(0.5, 0.5, 0.5, 1);
//	pdglVertex2f(-400, 300-24);
//	pdglVertex2f(-400, -200);
//	pdglVertex2f(200, -200);
//	pdglVertex2f(200, 300-24);

//	pdglColor4f(1, 1, 1, 1);
//	pdglVertex2f(-400, 300-32);
//	pdglVertex2f(-400, 300-40);
//	pdglVertex2f(200, 300-40);
//	pdglVertex2f(200, 300-32);

	pdglColor4f(0.5, 0.5, 0.5, 1);
	pdglVertex2f(-xs2, ys2);
	pdglVertex2f(-xs2, ys2-12);
	pdglVertex2f(xs2, ys2-12);
	pdglVertex2f(xs2, ys2);

	pdglColor4f(0.5, 0.5, 0.5, 1);
	pdglVertex2f(-xs2, ys2);
	pdglVertex2f(-xs2, -ys2);
	pdglVertex2f(-xs2+12*8+4, -ys2);
	pdglVertex2f(-xs2+12*8+4, ys2);

	pdglEnd();

	PDGLUI_Widget_Render3DBorder(-xs2, -ys2,
		12*8+4, ys-12, 0x808080, 0);
	PDGLUI_Widget_Render3DBorder(-xs2, ys2-12,
		xs2, 12, 0x808080, 0);
	
	for(i=0; i<64; i++)
	{
		pcur=ui_metaui_proglist[i];
		if(!pcur)
			continue;
		
		s="Test";
		if(pcur)
			{ s=pcur->title; }

		j=(i==ui_metaui_select);
		k=0x808080;
		k=j?0xE0E0E0:0xA0A0A0;

		PDGLUI_Widget_Render3DBorder(-xs2, ys2-(i+2)*12,
			12*8+4, 12, k, j);
//		GfxFont_DrawFormatString(s,
		GfxFont_DrawString(s,
			-xs2+2, ys2-(i+2)*12+2,
			8, 8, 0, 0, 0, 255);
	}

	pcur=ui_metaui_proglist[ui_metaui_select];
	if(pcur)
	{
		if(pcur->Render2D)
			pcur->Render2D(pcur);
	}
}

PDGLUI_API int UI_MetaUI_HandleInput(void)
{
	PDGLUI_ProgramInfo *pcur;
	int xs, ys, xs2, ys2, mx, my, mb;
	int i, j, k;
	
	UI_MetaUI_Init();
	
	if(!ui_metaui_active)
		return(0);
	
	xs=pdglui_state->xs;
	ys=pdglui_state->ys;
	xs2=(pdglui_state->xs/2);
	ys2=(pdglui_state->ys/2);
	mx=pdglui_state->mx;
	my=pdglui_state->my;
	mb=pdglui_state->mb;

	if(mx<(-xs2+12*8))
	{
//		i=(ys2-my)/12-2;
//		i=-(((-my)-ys2)/12-2);
		i=-(((-my)-ys2)/12+1);
		if((i>=0) && (i<256) && (mb&1))
			ui_metaui_select=i;
	}

	pcur=ui_metaui_proglist[ui_metaui_select];
	if(pcur)
	{
		if(pcur->HandleInput)
		{
			pcur->HandleInput(pcur, pdglui_state);
			return(1);
		}
	}

	return(0);
//	return(1);
}

PDGLUI_API int UI_MetaUI_HandleKey(int num, int down)
{
//	if(down && (num==(K_SKM_ESC|K_ESC)))
	if(down && (num==(K_SKM_ESC|'`')))
	{
		ui_metaui_active=!ui_metaui_active;
		return(0);
	}
	
	return(0);
}

PDGLUI_API void UI_MetaUI_Init(void)
{
	static int init=0;
	
	if(init)return;
	init=1;

	PDGL_AddKeyHandler(&UI_MetaUI_HandleKey);
}

int MetaUI_ProgTest_Render2D(PDGLUI_ProgramInfo *prog)
{
	Draw_FillSquareRGBA(-384, -256, 768, 512,
		255, 255, 255, 255);
	GfxFont_DrawString(prog->title,
		0, 0,
		64, 64, 0, 0, 0, 255);
//	GfxFont_DrawFormatString(prog->title,
//		0, 0,
//		64, 64, 0, 0, 0, 255);
	return(0);
}

PDGLUI_API PDGLUI_ProgramInfo *UI_MetaUI_CreateProgramInfoTest(void)
{
	static int seq=1;
	char tb[256];
	PDGLUI_ProgramInfo *tmp;
	
	sprintf(tb, "Test-%d", seq++);
	
	tmp=UI_MetaUI_CreateProgramInfo(tb);
	tmp->Render2D=MetaUI_ProgTest_Render2D;
}
