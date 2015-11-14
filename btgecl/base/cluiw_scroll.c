#include <btgecl.h>

int BTCLUI_WidgetScroll_HandleInput(BTCLUI_Widget *obj,
	BTCLUI_InputState *state, int ox, int oy)
{
	BTCLUI_Widget *cur;
	BTCLUI_Event *ev;
	int cx, cy;
	int i, j, k, ch;
	float f;
	unsigned short *ks;
	void *p;

	ch=0;
	if(!strcmp(obj->type, "hscroll"))
	{
		i=(obj->curval-obj->min)*(obj->xs-36)/
			(float)(obj->max-obj->min);
		i=BTCLUI_Widget_CheckClickedBox(ox+obj->ox+i+12, oy+obj->oy,
			12, 12, state);
		if(i)
		{
			f=state->dx*(obj->max-obj->min)/(float)(obj->xs-24);
			j=f;
			if((f!=0) && !j)j=(f<0)?-1:1;

			obj->curval+=j;
			if(obj->curval<obj->min)obj->curval=obj->min;
			ch=1;
		}

		i=BTCLUI_Widget_CheckClickedBox(ox+obj->ox, oy+obj->oy,
			12, 12, state);
		if(i)
		{
			obj->curval--;
			if(obj->curval<obj->min)obj->curval=obj->min;
			if(obj->curval>obj->max)obj->curval=obj->max;
			ch=1;
		}

		i=BTCLUI_Widget_CheckClickedBox(
			ox+obj->ox+obj->xs-12, oy+obj->oy,
			12, 12, state);
		if(i)
		{
			obj->curval++;
			if(obj->curval>obj->max)obj->curval=obj->max;
			ch=1;
		}
	}

	if(!strcmp(obj->type, "vscroll"))
	{
		i=BTCLUI_Widget_CheckClickedBox(ox+obj->ox, oy+obj->oy,
			12, 12, state);
		if(i)
		{
			obj->curval++;
			if(obj->curval>obj->max)obj->curval=obj->max;
			ch=1;
		}

		i=BTCLUI_Widget_CheckClickedBox(
			ox+obj->ox, oy+obj->oy+obj->ys-12,
			12, 12, state);
		if(i)
		{
			obj->curval--;
			if(obj->curval<obj->min)obj->curval=obj->min;
			ch=1;
		}
	}

	ev=BTCLUI_Widget_AddEvent(obj, "changed");
	ev->ival=obj->curval;
//	BTCLUI_Widget_SetEventIVal(obj->curval);

	return(0);
}

int BTCLUI_WidgetScroll_Render(BTCLUI_Widget *obj, int ox, int oy)
{
	BTCLUI_Widget *cur;
	int cx, cy;
	int r, g, b;
	int i, j, k;
	char *s;

	if(!strcmp(obj->type, "hscroll"))
	{
		GfxFont_SetFont("gfx", 0);

//		BTCLUI_Widget_Render3DBox(ox+obj->ox, oy+obj->oy,
//			obj->xs, obj->ys, 0xC0C0C0, 0);
		Draw_FillSquareRGBA(ox+obj->ox, oy+obj->oy,
			obj->xs, obj->ys, 192, 192, 192, 255);

		i=(obj->curval-obj->min)*(obj->xs-36)/
			(float)(obj->max-obj->min);
		BTCLUI_Widget_Render3DBox(ox+obj->ox+i+12, oy+obj->oy,
			12, 12, obj->wbgcolor, 0);

		BTCLUI_Widget_Render3DBox(ox+obj->ox, oy+obj->oy,
			12, 12, obj->wbgcolor, 0);
		GfxFont_DrawChar(0x28, ox+obj->ox, oy+obj->oy,
			12, 12, 0, 0, 0, 255);

		BTCLUI_Widget_Render3DBox(ox+obj->ox+obj->xs-12, oy+obj->oy,
			12, 12, obj->wbgcolor, 0);
		GfxFont_DrawChar(0x29, ox+obj->ox+obj->xs-12, oy+obj->oy,
			12, 12, 0, 0, 0, 255);
		return(0);
	}
	if(!strcmp(obj->type, "vscroll"))
	{
		GfxFont_SetFont("gfx", 0);

//		BTCLUI_Widget_Render3DBox(ox+obj->ox, oy+obj->oy,
//			obj->xs, obj->ys, 0xC0C0C0, 0);
		Draw_FillSquareRGBA(ox+obj->ox, oy+obj->oy,
			obj->xs, obj->ys, 192, 192, 192, 255);

//		i=(obj->curval-obj->min)*(obj->ys-24)/(obj->max-obj->min);
		i=(obj->curval-obj->min)*(obj->ys-36)/
			(float)(obj->max-obj->min);
		BTCLUI_Widget_Render3DBox(ox+obj->ox,
			oy+obj->oy+obj->ys-24-i,
			12, 12, obj->wbgcolor, 0);

		BTCLUI_Widget_Render3DBox(ox+obj->ox, oy+obj->oy,
			12, 12, obj->wbgcolor, 0);
		GfxFont_DrawChar(0x27, ox+obj->ox, oy+obj->oy,
			12, 12, 0, 0, 0, 255);

		BTCLUI_Widget_Render3DBox(ox+obj->ox, oy+obj->oy+obj->ys-12,
			12, 12, obj->wbgcolor, 0);
		GfxFont_DrawChar(0x26, ox+obj->ox, oy+obj->oy+obj->ys-12,
			12, 12, 0, 0, 0, 255);
		return(0);
	}

	return(0);
}

int BTCLUI_WidgetScroll_Init()
{
	static char *tylist[]={
		"hscroll", "vscroll", NULL};
	BTCLUI_WidgetType *ty;
	int i;

	for(i=0; tylist[i]; i++)
	{
		ty=BTCLUI_GetWidgetType(tylist[i]);
		ty->Render=BTCLUI_WidgetScroll_Render;
		ty->HandleInput=BTCLUI_WidgetScroll_HandleInput;
//		ty->Size=BTCLUI_WidgetScroll_SizeWidget;
//		ty->AddWidget=BTCLUI_WidgetBox_AddWidget;
	}

	return(0);
}
