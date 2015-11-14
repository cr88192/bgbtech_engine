#include <btgecl.h>

int BTCLUI_WidgetLabel_Render(BTCLUI_Widget *obj, int ox, int oy)
{
	BTCLUI_Widget *cur;
	int cx, cy;
	int r, g, b;
	int i, j, k;
	char *s;

	if(!strcmp(obj->type, "label"))
	{
		GfxFont_SetFont("fixed", 0);
		BTCLUI_Forms_CalcColor(obj->fgcolor, &r, &g, &b);
		GfxFont_DrawFormatString(obj->label,
			ox+obj->ox, oy+obj->oy,
			8, 8, r, g, b, 255);

		return(0);
	}

	if(!strcmp(obj->type, "text"))
	{
		GfxFont_SetFont("fixed", 0);
		BTCLUI_Forms_CalcColor(obj->fgcolor, &r, &g, &b);
		GfxFont_DrawFormatString(obj->value,
			ox+obj->ox, oy+obj->oy+obj->ys-8,
			8, 8, r, g, b, 255);

//		BTCLUI_Widget_RenderContent(ox+obj->ox, oy+obj->oy,
//			obj->xs, obj->ys, obj->body, obj->fgcolor);
		return(0);
	}

	if(!strcmp(obj->type, "image"))
	{
//		glEnable(GL_TEXTURE_2D);
		pdglEnableTexture2D();
		Draw_Bind(obj->texnum);

		pdglColor4f(1, 1, 1, 1);

		pdglBegin(PDGL_QUADS);
		pdglTexCoord2f(0, 0);
		pdglVertex2f(ox+obj->ox, oy+obj->oy);
		pdglTexCoord2f(1, 0);
		pdglVertex2f(ox+obj->ox+obj->xs, oy+obj->oy);
		pdglTexCoord2f(1, 1);
		pdglVertex2f(ox+obj->ox+obj->xs, oy+obj->oy+obj->ys);
		pdglTexCoord2f(0, 1);
		pdglVertex2f(ox+obj->ox, oy+obj->oy+obj->ys);
		pdglEnd();

		return(0);
	}
	
	return(-1);
}

int BTCLUI_WidgetLabel_HandleInput(BTCLUI_Widget *obj,
	BTCLUI_InputState *state, int ox, int oy)
{
	if(BTCLUI_Widget_CheckClicked(ox, oy, state, obj))
		BTCLUI_Widget_AddEvent(obj, "clicked");
	if(BTCLUI_Widget_CheckMClicked(ox, oy, state, obj))
		BTCLUI_Widget_AddEvent(obj, "mclicked");
	if(BTCLUI_Widget_CheckRClicked(ox, oy, state, obj))
		BTCLUI_Widget_AddEvent(obj, "rclicked");

	return(0);
}

int BTCLUI_WidgetLabel_SizeWidget(BTCLUI_Widget *tmp)
{
	int i;

	if(!strcmp(tmp->type, "label"))
	{
		tmp->xs=8*strlen(tmp->label);
		tmp->ys=8;
	}

	if(!strcmp(tmp->type, "text"))
	{
		BTCLUI_Widget_SizeText(tmp->value, &tmp->xs, &tmp->ys);
	}

	if(!strcmp(tmp->type, "image"))
	{
		tmp->texnum=Tex_LoadFile(tmp->image, &tmp->xs, &tmp->ys);
		if(tmp->texnum<0)
			tmp->texnum=Tex_LoadFile("images/X.tga",
				&tmp->xs, &tmp->ys);
	}

	return(0);
}

int BTCLUI_WidgetLabel_Init()
{
	static char *tylist[]={
		"label", "text", "image", NULL};
	BTCLUI_WidgetType *ty;
	int i;

	for(i=0; tylist[i]; i++)
	{
		ty=BTCLUI_GetWidgetType(tylist[i]);
		ty->Render=BTCLUI_WidgetLabel_Render;
		ty->HandleInput=BTCLUI_WidgetLabel_HandleInput;
		ty->Size=BTCLUI_WidgetLabel_SizeWidget;
	}

	return(0);
}
