#include <btgecl.h>

// extern int btgeclui_context->fgcolor;	//text fg color
// extern int btgeclui_context->bgcolor;	//text bg color
// extern int btgeclui_context->wbgcolor;	//widget bg color

int BTGECLUI_WidgetScroll_HandleInput(BTGECLUI_Widget *obj,
	BTGECLUI_InputState *state, int ox, int oy)
{
	BTGECLUI_Widget *cur;
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
		i=BTGECLUI_Widget_CheckClickedBox(ox+obj->ox+i+12, oy+obj->oy,
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

		i=BTGECLUI_Widget_CheckClickedBox(ox+obj->ox, oy+obj->oy,
			12, 12, state);
		if(i)
		{
			obj->curval--;
			if(obj->curval<obj->min)obj->curval=obj->min;
			if(obj->curval>obj->max)obj->curval=obj->max;
			ch=1;
		}

		i=BTGECLUI_Widget_CheckClickedBox(
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
		i=BTGECLUI_Widget_CheckClickedBox(ox+obj->ox, oy+obj->oy,
			12, 12, state);
		if(i)
		{
			obj->curval++;
			if(obj->curval>obj->max)obj->curval=obj->max;
			ch=1;
		}

		i=BTGECLUI_Widget_CheckClickedBox(
			ox+obj->ox, oy+obj->oy+obj->ys-12,
			12, 12, state);
		if(i)
		{
			obj->curval--;
			if(obj->curval<obj->min)obj->curval=obj->min;
			ch=1;
		}
	}

	BTGECLUI_Widget_AddEvent(obj, "changed");
	BTGECLUI_Widget_SetEventIVal(obj->curval);
}

int BTGECLUI_WidgetScroll_Render(BTGECLUI_Widget *obj, int ox, int oy)
{
	BTGECLUI_Widget *cur;
	int cx, cy;
	int r, g, b;
	int i, j, k;
	char *s;

	if(!strcmp(obj->type, "hscroll"))
	{
		GfxFont_SetFont("gfx", 0);

//		BTGECLUI_Widget_Render3DBox(ox+obj->ox, oy+obj->oy,
//			obj->xs, obj->ys, 0xC0C0C0, 0);
		Draw_FillSquareRGBA(ox+obj->ox, oy+obj->oy,
			obj->xs, obj->ys, 192, 192, 192, 255);

		i=(obj->curval-obj->min)*(obj->xs-36)/
			(float)(obj->max-obj->min);
		BTGECLUI_Widget_Render3DBox(ox+obj->ox+i+12, oy+obj->oy,
			12, 12, obj->wbgcolor, 0);

		BTGECLUI_Widget_Render3DBox(ox+obj->ox, oy+obj->oy,
			12, 12, obj->wbgcolor, 0);
		GfxFont_DrawChar(0x28, ox+obj->ox, oy+obj->oy,
			12, 12, 0, 0, 0, 255);

		BTGECLUI_Widget_Render3DBox(ox+obj->ox+obj->xs-12, oy+obj->oy,
			12, 12, obj->wbgcolor, 0);
		GfxFont_DrawChar(0x29, ox+obj->ox+obj->xs-12, oy+obj->oy,
			12, 12, 0, 0, 0, 255);
		return(0);
	}
	if(!strcmp(obj->type, "vscroll"))
	{
		GfxFont_SetFont("gfx", 0);

//		BTGECLUI_Widget_Render3DBox(ox+obj->ox, oy+obj->oy,
//			obj->xs, obj->ys, 0xC0C0C0, 0);
		Draw_FillSquareRGBA(ox+obj->ox, oy+obj->oy,
			obj->xs, obj->ys, 192, 192, 192, 255);

//		i=(obj->curval-obj->min)*(obj->ys-24)/(obj->max-obj->min);
		i=(obj->curval-obj->min)*(obj->ys-36)/
			(float)(obj->max-obj->min);
		BTGECLUI_Widget_Render3DBox(ox+obj->ox,
			oy+obj->oy+obj->ys-24-i,
			12, 12, obj->wbgcolor, 0);

		BTGECLUI_Widget_Render3DBox(ox+obj->ox, oy+obj->oy,
			12, 12, obj->wbgcolor, 0);
		GfxFont_DrawChar(0x27, ox+obj->ox, oy+obj->oy,
			12, 12, 0, 0, 0, 255);

		BTGECLUI_Widget_Render3DBox(ox+obj->ox, oy+obj->oy+obj->ys-12,
			12, 12, obj->wbgcolor, 0);
		GfxFont_DrawChar(0x26, ox+obj->ox, oy+obj->oy+obj->ys-12,
			12, 12, 0, 0, 0, 255);
		return(0);
	}

}

#if 0
//BTGECLUI_Widget *BTGECLUI_WidgetScroll_BuildWidget(NetParse_Node *node,
//	BTGECLUI_Form *form, BTGECLUI_Forms_StyleState *style)
{
	BTGECLUI_Widget *tmp, *tmp2;
	char *str;
	int fg, bg, wbg;
	int i;

	if(!node)return(NULL);

	if(!strcmp(node->key, "hscroll") || !strcmp(node->key, "vscroll"))
	{
		tmp=gcalloc(sizeof(BTGECLUI_Widget));
		tmp->type=dystrdup(node->key);
		tmp->form=form;
		tmp->funcs=btgeclui_context->widgetscroll_methods;

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

int BTGECLUI_WidgetScroll_Init()
{
	BTGECLUI_Widget_RegisterType("hscroll", BTGECLUI_WidgetScroll_BuildWidget);
	BTGECLUI_Widget_RegisterType("vscroll", BTGECLUI_WidgetScroll_BuildWidget);

	btgeclui_context->widgetscroll_methods=
		gcalloc(sizeof(struct BTGECLUI_WidgetMethods_s));
	btgeclui_context->widgetscroll_methods->render=&BTGECLUI_WidgetScroll_Render;
	btgeclui_context->widgetscroll_methods->handle_input=
		&BTGECLUI_WidgetScroll_HandleInput;
//	btgeclui_context->widgetscroll_methods->size=&BTGECLUI_WidgetScroll_SizeWidget;
//	btgeclui_context->widgetscroll_methods->message=&BTGECLUI_WidgetScroll_SendMessage;
}
#endif

BTGECLUI_Widget *BTGECLUI_WidgetScroll_Begin(char *type, char *id)
{
	BTGECLUI_Widget *tmp, *tmp2;
	char *str;
	int fg, bg, wbg;
	int i;

	tmp=gcalloc(sizeof(BTGECLUI_Widget));
	tmp->type=dystrdup(type);
	tmp->id=dystrdup(id);
	tmp->fgcolor=btgeclui_context->fgcolor;
	tmp->bgcolor=btgeclui_context->bgcolor;
	tmp->wbgcolor=btgeclui_context->wbgcolor;

	return(tmp);
}

int BTGECLUI_WidgetScroll_End(BTGECLUI_Widget *tmp)
{
	int i;
	return(0);
}

char *BTGECLUI_WidgetScroll_GetPropertyS(BTGECLUI_Widget *obj, char *var)
{
	return(NULL);
}

int BTGECLUI_WidgetScroll_SetPropertyS(BTGECLUI_Widget *obj, char *var, char *val)
{
	return(0);
}

int BTGECLUI_WidgetScroll_GetPropertyI(BTGECLUI_Widget *obj, char *var)
{
	if(!strcmp(var, "value"))return(obj->curval);

	return(0);
}

int BTGECLUI_WidgetScroll_SetPropertyI(BTGECLUI_Widget *obj, char *var, int val)
{
	if(!strcmp(var, "value"))obj->curval=val;
	return(0);
}

BTGECLUI_WidgetType btgeclui_context->scroll_methods=
{
NULL,
"hscroll;vscroll",

BTGECLUI_WidgetScroll_Render,
BTGECLUI_WidgetScroll_HandleInput,
NULL,
BTGECLUI_WidgetScroll_Begin,
BTGECLUI_WidgetScroll_End,
NULL,
BTGECLUI_WidgetScroll_GetPropertyS,
BTGECLUI_WidgetScroll_SetPropertyS,
BTGECLUI_WidgetScroll_GetPropertyI,
BTGECLUI_WidgetScroll_SetPropertyI
};

int BTGECLUI_WidgetScroll_Init()
{
	BTGECLUI_Widget_RegisterType(&btgeclui_context->scroll_methods);
}
