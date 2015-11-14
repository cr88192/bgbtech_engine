#include <pdgl.h>
#include <pdglui.h>

extern int pdglui_fgcolor;	//text fg color
extern int pdglui_bgcolor;	//text bg color
extern int pdglui_wbgcolor;	//widget bg color

int PDGLUI_WidgetUnion_HandleInput(PDGLUI_Widget *obj,
	PDGLUI_InputState *state, int ox, int oy)
{
	PDGLUI_Widget *cur;

	if(!obj->value)return(0);

	cur=obj->first;
	while(cur)
	{
		if(strcmp(cur->id, obj->value))
		{
			cur=cur->next;
			continue;
		}

		PDGLUI_Widget_HandleInput(cur, state, ox, oy);
		break;
	}

	return(0);
}

int PDGLUI_WidgetUnion_Render(PDGLUI_Widget *obj, int ox, int oy)
{
	PDGLUI_Widget *cur;

	if(!obj->value)return(0);

	cur=obj->first;
	while(cur)
	{
		if(strcmp(cur->id, obj->value))
		{
			cur=cur->next;
			continue;
		}

		PDGLUI_Widget_Render(cur, ox, oy);
		break;
	}
	return(0);
}

int PDGLUI_WidgetUnion_SizeWidget(PDGLUI_Widget *obj)
{
	PDGLUI_Widget *wcur;

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

PDGLUI_Widget *PDGLUI_WidgetUnion_Begin(char *type, char *id)
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

int PDGLUI_WidgetUnion_AddWidget(PDGLUI_Widget *tmp, PDGLUI_Widget *tmp2)
{
	PDGLUI_Widget *cur;

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

int PDGLUI_WidgetUnion_End(PDGLUI_Widget *tmp)
{
	int i;

	printf("union: %d %d\n", tmp->xs, tmp->ys);

	return(0);
}

char *PDGLUI_WidgetUnion_GetPropertyS(PDGLUI_Widget *obj, char *var)
{
	if(!strcmp(var, "value"))return(obj->value);
	return(NULL);
}

int PDGLUI_WidgetUnion_SetPropertyS(PDGLUI_Widget *obj, char *var, char *val)
{
	if(!strcmp(var, "value"))obj->value=dystrdup(val);
	return(0);
}

int PDGLUI_WidgetUnion_GetPropertyI(PDGLUI_Widget *obj, char *var)
{
	return(0);
}

int PDGLUI_WidgetUnion_SetPropertyI(PDGLUI_Widget *obj, char *var, int val)
{
	return(0);
}

PDGLUI_WidgetType pdglui_union_methods=
{
NULL,
"union",

PDGLUI_WidgetUnion_Render,
PDGLUI_WidgetUnion_HandleInput,
NULL,
PDGLUI_WidgetUnion_Begin,
PDGLUI_WidgetUnion_End,
PDGLUI_WidgetUnion_AddWidget,
PDGLUI_WidgetUnion_GetPropertyS,
PDGLUI_WidgetUnion_SetPropertyS,
PDGLUI_WidgetUnion_GetPropertyI,
PDGLUI_WidgetUnion_SetPropertyI
};

int PDGLUI_WidgetUnion_Init()
{
	PDGLUI_Widget_RegisterType(&pdglui_union_methods);
}
