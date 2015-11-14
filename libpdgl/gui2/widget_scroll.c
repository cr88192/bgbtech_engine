#include <pdgl.h>
#include <pdglui.h>

extern int pdglui_fgcolor;	//text fg color
extern int pdglui_bgcolor;	//text bg color
extern int pdglui_wbgcolor;	//widget bg color

int PDGLUI_WidgetScroll_HandleInput(PDGLUI_Widget *obj,
	PDGLUI_InputState *state, int ox, int oy)
{
	PDGLUI_Widget *cur;
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
		i=PDGLUI_Widget_CheckClickedBox(ox+obj->ox+i+12, oy+obj->oy,
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

		i=PDGLUI_Widget_CheckClickedBox(ox+obj->ox, oy+obj->oy,
			12, 12, state);
		if(i)
		{
			obj->curval--;
			if(obj->curval<obj->min)obj->curval=obj->min;
			if(obj->curval>obj->max)obj->curval=obj->max;
			ch=1;
		}

		i=PDGLUI_Widget_CheckClickedBox(
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
		i=PDGLUI_Widget_CheckClickedBox(ox+obj->ox, oy+obj->oy,
			12, 12, state);
		if(i)
		{
			obj->curval++;
			if(obj->curval>obj->max)obj->curval=obj->max;
			ch=1;
		}

		i=PDGLUI_Widget_CheckClickedBox(
			ox+obj->ox, oy+obj->oy+obj->ys-12,
			12, 12, state);
		if(i)
		{
			obj->curval--;
			if(obj->curval<obj->min)obj->curval=obj->min;
			ch=1;
		}
	}

	PDGLUI_Widget_AddEvent(obj, "changed");
	PDGLUI_Widget_SetEventIVal(obj->curval);
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

#if 0
//PDGLUI_Widget *PDGLUI_WidgetScroll_BuildWidget(NetParse_Node *node,
//	PDGLUI_Form *form, PDGLUI_Forms_StyleState *style)
{
	PDGLUI_Widget *tmp, *tmp2;
	char *str;
	int fg, bg, wbg;
	int i;

	if(!node)return(NULL);

	if(!strcmp(node->key, "hscroll") || !strcmp(node->key, "vscroll"))
	{
		tmp=gcalloc(sizeof(PDGLUI_Widget));
		tmp->type=dystrdup(node->key);
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
		gcalloc(sizeof(struct PDGLUI_WidgetMethods_s));
	pdglui_widgetscroll_methods->render=&PDGLUI_WidgetScroll_Render;
	pdglui_widgetscroll_methods->handle_input=
		&PDGLUI_WidgetScroll_HandleInput;
//	pdglui_widgetscroll_methods->size=&PDGLUI_WidgetScroll_SizeWidget;
//	pdglui_widgetscroll_methods->message=&PDGLUI_WidgetScroll_SendMessage;
}
#endif

PDGLUI_Widget *PDGLUI_WidgetScroll_Begin(char *type, char *id)
{
	PDGLUI_Widget *tmp, *tmp2;
	char *str;
	int fg, bg, wbg;
	int i;

	tmp=gcalloc(sizeof(PDGLUI_Widget));
	tmp->type=dystrdup(type);
	tmp->id=dystrdup(id);
	tmp->fgcolor=pdglui_fgcolor;
	tmp->bgcolor=pdglui_bgcolor;
	tmp->wbgcolor=pdglui_wbgcolor;

	return(tmp);
}

int PDGLUI_WidgetScroll_End(PDGLUI_Widget *tmp)
{
	int i;
	return(0);
}

char *PDGLUI_WidgetScroll_GetPropertyS(PDGLUI_Widget *obj, char *var)
{
	return(NULL);
}

int PDGLUI_WidgetScroll_SetPropertyS(PDGLUI_Widget *obj, char *var, char *val)
{
	return(0);
}

int PDGLUI_WidgetScroll_GetPropertyI(PDGLUI_Widget *obj, char *var)
{
	if(!strcmp(var, "value"))return(obj->curval);

	return(0);
}

int PDGLUI_WidgetScroll_SetPropertyI(PDGLUI_Widget *obj, char *var, int val)
{
	if(!strcmp(var, "value"))obj->curval=val;
	return(0);
}

PDGLUI_WidgetType pdglui_scroll_methods=
{
NULL,
"hscroll;vscroll",

PDGLUI_WidgetScroll_Render,
PDGLUI_WidgetScroll_HandleInput,
NULL,
PDGLUI_WidgetScroll_Begin,
PDGLUI_WidgetScroll_End,
NULL,
PDGLUI_WidgetScroll_GetPropertyS,
PDGLUI_WidgetScroll_SetPropertyS,
PDGLUI_WidgetScroll_GetPropertyI,
PDGLUI_WidgetScroll_SetPropertyI
};

int PDGLUI_WidgetScroll_Init()
{
	PDGLUI_Widget_RegisterType(&pdglui_scroll_methods);
}
