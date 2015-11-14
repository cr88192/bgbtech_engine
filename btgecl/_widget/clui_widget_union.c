#include <btgecl.h>

// extern int btgeclui_context->fgcolor;	//text fg color
// extern int btgeclui_context->bgcolor;	//text bg color
// extern int btgeclui_context->wbgcolor;	//widget bg color

int BTGECLUI_WidgetUnion_HandleInput(BTGECLUI_Widget *obj,
	BTGECLUI_InputState *state, int ox, int oy)
{
	BTGECLUI_Widget *cur;

	if(!obj->value)return(0);

	cur=obj->first;
	while(cur)
	{
		if(strcmp(cur->id, obj->value))
		{
			cur=cur->next;
			continue;
		}

		BTGECLUI_Widget_HandleInput(cur, state, ox, oy);
		break;
	}

	return(0);
}

int BTGECLUI_WidgetUnion_Render(BTGECLUI_Widget *obj, int ox, int oy)
{
	BTGECLUI_Widget *cur;

	if(!obj->value)return(0);

	cur=obj->first;
	while(cur)
	{
		if(strcmp(cur->id, obj->value))
		{
			cur=cur->next;
			continue;
		}

		BTGECLUI_Widget_Render(cur, ox, oy);
		break;
	}
	return(0);
}

int BTGECLUI_WidgetUnion_SizeWidget(BTGECLUI_Widget *obj)
{
	BTGECLUI_Widget *wcur;

	obj->xs=0;
	obj->ys=0;
	wcur=obj->first;
	while(wcur)
	{
		if(wcur->xs>obj->xs)obj->xs=wcur->xs;
		if(wcur->ys>obj->ys)obj->ys=wcur->ys;
		wcur=wcur->next;
	}
}

BTGECLUI_Widget *BTGECLUI_WidgetUnion_Begin(char *type, char *id)
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

int BTGECLUI_WidgetUnion_AddWidget(BTGECLUI_Widget *tmp, BTGECLUI_Widget *tmp2)
{
	BTGECLUI_Widget *cur;

	cur=tmp->first;
	if(cur)
	{
		while(cur->next)cur=cur->next;
		cur->next=tmp2;
		tmp2->prev=cur;
		tmp2->up=tmp;
	}else
	{
		tmp2->next=NULL;
		tmp2->prev=NULL;
		tmp->first=tmp2;
		tmp2->up=tmp;
	}

	if(tmp2->xs>tmp->xs)tmp->xs=tmp2->xs;
	if(tmp2->ys>tmp->ys)tmp->ys=tmp2->ys;
	return(0);
}

int BTGECLUI_WidgetUnion_End(BTGECLUI_Widget *tmp)
{
	int i;

	printf("union: %d %d\n", tmp->xs, tmp->ys);

	return(0);
}

char *BTGECLUI_WidgetUnion_GetPropertyS(BTGECLUI_Widget *obj, char *var)
{
	if(!strcmp(var, "value"))return(obj->value);
	return(NULL);
}

int BTGECLUI_WidgetUnion_SetPropertyS(BTGECLUI_Widget *obj, char *var, char *val)
{
	if(!strcmp(var, "value"))obj->value=dystrdup(val);
	return(0);
}

int BTGECLUI_WidgetUnion_GetPropertyI(BTGECLUI_Widget *obj, char *var)
{
	return(0);
}

int BTGECLUI_WidgetUnion_SetPropertyI(BTGECLUI_Widget *obj, char *var, int val)
{
	return(0);
}

BTGECLUI_WidgetType btgeclui_context->union_methods=
{
NULL,
"union",

BTGECLUI_WidgetUnion_Render,
BTGECLUI_WidgetUnion_HandleInput,
NULL,
BTGECLUI_WidgetUnion_Begin,
BTGECLUI_WidgetUnion_End,
BTGECLUI_WidgetUnion_AddWidget,
BTGECLUI_WidgetUnion_GetPropertyS,
BTGECLUI_WidgetUnion_SetPropertyS,
BTGECLUI_WidgetUnion_GetPropertyI,
BTGECLUI_WidgetUnion_SetPropertyI
};

int BTGECLUI_WidgetUnion_Init()
{
	BTGECLUI_Widget_RegisterType(&btgeclui_context->union_methods);
}
