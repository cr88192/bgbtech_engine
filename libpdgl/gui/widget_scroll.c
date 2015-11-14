#include <pdgl.h>
#include <pdglui.h>

struct PDGLUI_WidgetMethods_s *pdglui_widgetscroll_methods=NULL;

int PDGLUI_WidgetScroll_HandleInput(PDGLUI_Widget *obj,
	PDGLUI_InputState *state, int ox, int oy)
{
	PDGLUI_Widget *cur;
	int cx, cy;
	int i, j, k;
	float f;
	unsigned short *ks;
	void *p;

	if(!strcmp(obj->type, "hscroll"))
	{
		i=(obj->curval-obj->min)*(obj->xs-36)/
			(float)(obj->max-obj->min);
		i=PDGLUI_Widget_CheckClickedBox(ox+obj->ox+i+12, oy+obj->oy,
			12, 12, state);
		if(i)
		{
			f=state->dx*(obj->max-obj->min)/(float)(obj->xs-24);
			j=f;
			if((f!=0) && !j)j=(f<0)?-1:1;

			obj->curval+=j;
			if(obj->curval<obj->min)obj->curval=obj->min;
		}

		i=PDGLUI_Widget_CheckClickedBox(ox+obj->ox, oy+obj->oy,
			12, 12, state);
		if(i)
		{
			obj->curval--;
			if(obj->curval<obj->min)obj->curval=obj->min;
			if(obj->curval>obj->max)obj->curval=obj->max;
		}

		i=PDGLUI_Widget_CheckClickedBox(
			ox+obj->ox+obj->xs-12, oy+obj->oy,
			12, 12, state);
		if(i)
		{
			obj->curval++;
			if(obj->curval>obj->max)obj->curval=obj->max;
		}
	}

	if(!strcmp(obj->type, "vscroll"))
	{
		i=PDGLUI_Widget_CheckClickedBox(ox+obj->ox, oy+obj->oy,
			12, 12, state);
		if(i)
		{
			obj->curval++;
			if(obj->curval>obj->max)obj->curval=obj->max;
		}

		i=PDGLUI_Widget_CheckClickedBox(
			ox+obj->ox, oy+obj->oy+obj->ys-12,
			12, 12, state);
		if(i)
		{
			obj->curval--;
			if(obj->curval<obj->min)obj->curval=obj->min;
		}
	}
}

int PDGLUI_WidgetScroll_Render(PDGLUI_Widget *obj, int ox, int oy)
{
	PDGLUI_Widget *cur;
	int cx, cy;
	int r, g, b;
	int i, j, k;
	char *s;

	if(!strcmp(obj->type, "hscroll"))
	{
		GfxFont_SetFont("gfx", 0);

//		PDGLUI_Widget_Render3DBox(ox+obj->ox, oy+obj->oy,
//			obj->xs, obj->ys, 0xC0C0C0, 0);
		Draw_FillSquareRGBA(ox+obj->ox, oy+obj->oy,
			obj->xs, obj->ys, 192, 192, 192, 255);

		i=(obj->curval-obj->min)*(obj->xs-36)/
			(float)(obj->max-obj->min);
		PDGLUI_Widget_Render3DBox(ox+obj->ox+i+12, oy+obj->oy,
			12, 12, obj->wbgcolor, 0);

		PDGLUI_Widget_Render3DBox(ox+obj->ox, oy+obj->oy,
			12, 12, obj->wbgcolor, 0);
		GfxFont_DrawChar(0x28, ox+obj->ox, oy+obj->oy,
			12, 12, 0, 0, 0, 255);

		PDGLUI_Widget_Render3DBox(ox+obj->ox+obj->xs-12, oy+obj->oy,
			12, 12, obj->wbgcolor, 0);
		GfxFont_DrawChar(0x29, ox+obj->ox+obj->xs-12, oy+obj->oy,
			12, 12, 0, 0, 0, 255);
		return(0);
	}
	if(!strcmp(obj->type, "vscroll"))
	{
		GfxFont_SetFont("gfx", 0);

//		PDGLUI_Widget_Render3DBox(ox+obj->ox, oy+obj->oy,
//			obj->xs, obj->ys, 0xC0C0C0, 0);
		Draw_FillSquareRGBA(ox+obj->ox, oy+obj->oy,
			obj->xs, obj->ys, 192, 192, 192, 255);

//		i=(obj->curval-obj->min)*(obj->ys-24)/(obj->max-obj->min);
		i=(obj->curval-obj->min)*(obj->ys-36)/
			(float)(obj->max-obj->min);
		PDGLUI_Widget_Render3DBox(ox+obj->ox,
			oy+obj->oy+obj->ys-24-i,
			12, 12, obj->wbgcolor, 0);

		PDGLUI_Widget_Render3DBox(ox+obj->ox, oy+obj->oy,
			12, 12, obj->wbgcolor, 0);
		GfxFont_DrawChar(0x27, ox+obj->ox, oy+obj->oy,
			12, 12, 0, 0, 0, 255);

		PDGLUI_Widget_Render3DBox(ox+obj->ox, oy+obj->oy+obj->ys-12,
			12, 12, obj->wbgcolor, 0);
		GfxFont_DrawChar(0x26, ox+obj->ox, oy+obj->oy+obj->ys-12,
			12, 12, 0, 0, 0, 255);
		return(0);
	}

}

PDGLUI_Widget *PDGLUI_WidgetScroll_BuildWidget(NetParse_Node *node,
	PDGLUI_Form *form, PDGLUI_Forms_StyleState *style)
{
	PDGLUI_Widget *tmp, *tmp2;
	char *str;
	int fg, bg, wbg;
	int i;

	if(!node)return(NULL);

	if(!strcmp(node->key, "hscroll") || !strcmp(node->key, "vscroll"))
	{
		tmp=kalloc(sizeof(PDGLUI_Widget));
		tmp->type=kstrdup(node->key);
		tmp->form=form;
		tmp->funcs=pdglui_widgetscroll_methods;

		tmp->fgcolor=style->fgcolor;
		tmp->bgcolor=style->bgcolor;
		tmp->wbgcolor=style->wbgcolor;

		if(!strcmp(node->key, "hscroll"))tmp->xs=24;
			else tmp->xs=12;
		if(!strcmp(node->key, "vscroll"))tmp->ys=24;
			else tmp->ys=12;

		str=NetParse_GetNodeAttr(node, "width");
		if(str)tmp->xs=katoi(str);
		str=NetParse_GetNodeAttr(node, "height");
		if(str)tmp->ys=katoi(str);

		str=NetParse_GetNodeAttr(node, "min");
		if(str)tmp->min=katoi(str);
		str=NetParse_GetNodeAttr(node, "max");
		if(str)tmp->max=katoi(str);
		str=NetParse_GetNodeAttr(node, "value");
		if(str)tmp->curval=katoi(str);

		return(tmp);
	}
}

int PDGLUI_WidgetScroll_Init()
{
	PDGLUI_Widget_RegisterType("hscroll", PDGLUI_WidgetScroll_BuildWidget);
	PDGLUI_Widget_RegisterType("vscroll", PDGLUI_WidgetScroll_BuildWidget);

	pdglui_widgetscroll_methods=
		kalloc(sizeof(struct PDGLUI_WidgetMethods_s));
	pdglui_widgetscroll_methods->render=&PDGLUI_WidgetScroll_Render;
	pdglui_widgetscroll_methods->handle_input=
		&PDGLUI_WidgetScroll_HandleInput;
//	pdglui_widgetscroll_methods->size=&PDGLUI_WidgetScroll_SizeWidget;
//	pdglui_widgetscroll_methods->message=&PDGLUI_WidgetScroll_SendMessage;
}
