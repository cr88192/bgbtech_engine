#include <btgecl.h>

// extern int btgeclui_context->fgcolor;	//text fg color
// extern int btgeclui_context->bgcolor;	//text bg color
// extern int btgeclui_context->wbgcolor;	//widget bg color

int BTGECLUI_WidgetTree_HandleInput(BTGECLUI_Widget *obj,
	BTGECLUI_InputState *state, int ox, int oy)
{
	BTGECLUI_Widget *cur;
	int cx, cy;
	int i, j, k;
	unsigned short *ks;
	void *p;

	if(!strcmp(obj->type, "tree"))
	{
		if(BTGECLUI_Widget_CheckClickedBox(ox,
			oy+obj->ys-obj->first->ys, 12, 12, state))
		{
			obj->checked=!obj->checked;
//			p=NetVal_WrapBool(obj->checked);
//			BTGECLUI_Forms_Message(obj->form, obj->id, "checked", 1, &p);

			BTGECLUI_Widget_AddEvent(obj,
				obj->checked?"clicked":"unclicked");

			BTGECLUI_Widget_RSizeWidget(obj);
		}

		cx=ox;
		cy=oy+obj->ys;
		cur=obj->first;

		cy-=cur->ys;
		BTGECLUI_Widget_HandleInput(cur, state, cx+16, cy);
		cur=cur->next;

		while(cur)
		{
			cy-=cur->ys;
			BTGECLUI_Widget_HandleInput(cur, state, cx+32, cy);
			cur=cur->next;
		}
		return(0);
	}
}

int BTGECLUI_WidgetTree_Render(BTGECLUI_Widget *obj, int ox, int oy)
{
	BTGECLUI_Widget *cur;
	int cx, cy;
	int r, g, b;
	int i, j, k;
	char *s;

	if(!strcmp(obj->type, "tree"))
	{
//		Draw_FillSquareRGBA(ox+obj->ox, oy+obj->oy, obj->xs, obj->ys,
//			127, 127, 127, 255);

		cx=ox+obj->ox;
		cy=oy+obj->oy+obj->ys-obj->first->ys;

		Draw_FillSquareRGBA(cx, cy, 12, 12,
			255, 255, 255, 255);

//		BTGECLUI_Widget_Render3DBorder(cx, cy,
//			12, 12, obj->wbgcolor, 1);

		GfxFont_SetFont("fixed", 0);
		if(obj->checked)
		{
//			Draw_FillSquareRGBA(cx+3, cy+3,
//				6, 6, 0, 0, 0, 255);
			GfxFont_DrawChar('+', cx, cy-2,
				12, 12, 0, 0, 0, 255);
		}else
		{
			GfxFont_DrawChar('-', cx, cy-2,
				12, 12, 0, 0, 0, 255);
		}

		BTGECLUI_Widget_Render(obj->first, cx+16, cy);
		if(obj->checked)
		{
			cur=obj->first->next;

			while(cur)
			{
				cy-=cur->ys;
				BTGECLUI_Widget_Render(cur, cx+32, cy);
				cur=cur->next;
			}
		}

		return(0);
	}
}

int BTGECLUI_WidgetTree_SizeWidget(BTGECLUI_Widget *obj)
{
	BTGECLUI_Widget *wcur;

	if(!strcmp(obj->type, "tree"))
	{
		obj->xs=obj->first->xs+12;
		obj->ys=obj->first->ys;

		if(obj->checked)
		{
			wcur=obj->first->next;
			while(wcur)
			{
				if((wcur->xs+24)>obj->xs)
					obj->xs=wcur->xs+24;
				obj->ys+=wcur->ys;
				wcur=wcur->next;
			}
		}
	}
}

#if 0
//BTGECLUI_Widget *BTGECLUI_WidgetTree_BuildWidget(NetParse_Node *node,
//	BTGECLUI_Form *form, BTGECLUI_Forms_StyleState *style)
{
	BTGECLUI_Widget *tmp, *tmp2;
	char *str;
	int fg, bg, wbg;
	int i;

	if(!node)return(NULL);

	if(!strcmp(node->key, "tree"))
	{
		tmp=gcalloc(sizeof(BTGECLUI_Widget));
		tmp->type=dystrdup("tree");
		tmp->form=form;
		tmp->funcs=btgeclui_context->widgettree_methods;

		str=NetParse_GetNodeAttr(node, "id");
		if(str)tmp->id=dystrdup(str);

		str=NetParse_GetNodeAttr(node, "checked");
		if(str)tmp->checked=1;

		tmp->first=BTGECLUI_Widget_BuildWidgetList(node->first, tmp, form, style);

		BTGECLUI_Widget_SizeWidget(tmp);

		return(tmp);
	}
}

int BTGECLUI_WidgetTree_Init()
{
	BTGECLUI_Widget_RegisterType("tree", BTGECLUI_WidgetTree_BuildWidget);

	btgeclui_context->widgettree_methods=
		gcalloc(sizeof(struct BTGECLUI_WidgetMethods_s));
	btgeclui_context->widgettree_methods->render=&BTGECLUI_WidgetTree_Render;
	btgeclui_context->widgettree_methods->handle_input=&BTGECLUI_WidgetTree_HandleInput;
	btgeclui_context->widgettree_methods->size=&BTGECLUI_WidgetTree_SizeWidget;
//	btgeclui_context->widgettree_methods->message=&BTGECLUI_WidgetTree_SendMessage;
}
#endif

BTGECLUI_Widget *BTGECLUI_WidgetTree_Begin(char *type, char *id)
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

int BTGECLUI_WidgetTree_End(BTGECLUI_Widget *tmp)
{
	int i;
	BTGECLUI_WidgetTree_SizeWidget(tmp);
	return(0);
}

int BTGECLUI_WidgetTree_AddWidget(BTGECLUI_Widget *tmp, BTGECLUI_Widget *tmp2)
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

	return(0);
}

char *BTGECLUI_WidgetTree_GetPropertyS(BTGECLUI_Widget *obj, char *var)
{
	return(NULL);
}

int BTGECLUI_WidgetTree_SetPropertyS(BTGECLUI_Widget *obj, char *var, char *val)
{
	return(0);
}

int BTGECLUI_WidgetTree_GetPropertyI(BTGECLUI_Widget *obj, char *var)
{
	if(!strcmp(var, "checked"))return(obj->checked);
	return(0);
}

int BTGECLUI_WidgetTree_SetPropertyI(BTGECLUI_Widget *obj, char *var, int val)
{
	if(!strcmp(var, "checked"))obj->checked=val;
	return(0);
}

BTGECLUI_WidgetType btgeclui_context->tree_methods=
{
NULL,
"tree",

BTGECLUI_WidgetTree_Render,
BTGECLUI_WidgetTree_HandleInput,
BTGECLUI_WidgetTree_SizeWidget,
BTGECLUI_WidgetTree_Begin,
BTGECLUI_WidgetTree_End,
BTGECLUI_WidgetTree_AddWidget,
BTGECLUI_WidgetTree_GetPropertyS,
BTGECLUI_WidgetTree_SetPropertyS,
BTGECLUI_WidgetTree_GetPropertyI,
BTGECLUI_WidgetTree_SetPropertyI
};

int BTGECLUI_WidgetTree_Init()
{
	BTGECLUI_Widget_RegisterType(&btgeclui_context->tree_methods);
}
