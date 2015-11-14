#include <pdgl.h>
#include <pdglui.h>

extern int pdglui_fgcolor;	//text fg color
extern int pdglui_bgcolor;	//text bg color
extern int pdglui_wbgcolor;	//widget bg color

int PDGLUI_WidgetTree_HandleInput(PDGLUI_Widget *obj,
	PDGLUI_InputState *state, int ox, int oy)
{
	PDGLUI_Widget *cur;
	int cx, cy;
	int i, j, k;
	unsigned short *ks;
	void *p;

	if(!strcmp(obj->type, "tree"))
	{
		if(PDGLUI_Widget_CheckClickedBox(ox,
			oy+obj->ys-obj->first->ys, 12, 12, state))
		{
			obj->checked=!obj->checked;
//			p=NetVal_WrapBool(obj->checked);
//			PDGLUI_Forms_Message(obj->form, obj->id, "checked", 1, &p);

			PDGLUI_Widget_AddEvent(obj,
				obj->checked?"clicked":"unclicked");

			PDGLUI_Widget_RSizeWidget(obj);
		}

		cx=ox;
		cy=oy+obj->ys;
		cur=obj->first;

		cy-=cur->ys;
		PDGLUI_Widget_HandleInput(cur, state, cx+16, cy);
		cur=cur->next;

		while(cur)
		{
			cy-=cur->ys;
			PDGLUI_Widget_HandleInput(cur, state, cx+32, cy);
			cur=cur->next;
		}
		return(0);
	}
}

int PDGLUI_WidgetTree_Render(PDGLUI_Widget *obj, int ox, int oy)
{
	PDGLUI_Widget *cur;
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

//		PDGLUI_Widget_Render3DBorder(cx, cy,
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

		PDGLUI_Widget_Render(obj->first, cx+16, cy);
		if(obj->checked)
		{
			cur=obj->first->next;

			while(cur)
			{
				cy-=cur->ys;
				PDGLUI_Widget_Render(cur, cx+32, cy);
				cur=cur->next;
			}
		}

		return(0);
	}
}

int PDGLUI_WidgetTree_SizeWidget(PDGLUI_Widget *obj)
{
	PDGLUI_Widget *wcur;

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
//PDGLUI_Widget *PDGLUI_WidgetTree_BuildWidget(NetParse_Node *node,
//	PDGLUI_Form *form, PDGLUI_Forms_StyleState *style)
{
	PDGLUI_Widget *tmp, *tmp2;
	char *str;
	int fg, bg, wbg;
	int i;

	if(!node)return(NULL);

	if(!strcmp(node->key, "tree"))
	{
		tmp=gcalloc(sizeof(PDGLUI_Widget));
		tmp->type=dystrdup("tree");
		tmp->form=form;
		tmp->funcs=pdglui_widgettree_methods;

		str=NetParse_GetNodeAttr(node, "id");
		if(str)tmp->id=dystrdup(str);

		str=NetParse_GetNodeAttr(node, "checked");
		if(str)tmp->checked=1;

		tmp->first=PDGLUI_Widget_BuildWidgetList(node->first, tmp, form, style);

		PDGLUI_Widget_SizeWidget(tmp);

		return(tmp);
	}
}

int PDGLUI_WidgetTree_Init()
{
	PDGLUI_Widget_RegisterType("tree", PDGLUI_WidgetTree_BuildWidget);

	pdglui_widgettree_methods=
		gcalloc(sizeof(struct PDGLUI_WidgetMethods_s));
	pdglui_widgettree_methods->render=&PDGLUI_WidgetTree_Render;
	pdglui_widgettree_methods->handle_input=&PDGLUI_WidgetTree_HandleInput;
	pdglui_widgettree_methods->size=&PDGLUI_WidgetTree_SizeWidget;
//	pdglui_widgettree_methods->message=&PDGLUI_WidgetTree_SendMessage;
}
#endif

PDGLUI_Widget *PDGLUI_WidgetTree_Begin(char *type, char *id)
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

int PDGLUI_WidgetTree_End(PDGLUI_Widget *tmp)
{
	int i;
	PDGLUI_WidgetTree_SizeWidget(tmp);
	return(0);
}

int PDGLUI_WidgetTree_AddWidget(PDGLUI_Widget *tmp, PDGLUI_Widget *tmp2)
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

	return(0);
}

char *PDGLUI_WidgetTree_GetPropertyS(PDGLUI_Widget *obj, char *var)
{
	return(NULL);
}

int PDGLUI_WidgetTree_SetPropertyS(PDGLUI_Widget *obj, char *var, char *val)
{
	return(0);
}

int PDGLUI_WidgetTree_GetPropertyI(PDGLUI_Widget *obj, char *var)
{
	if(!strcmp(var, "checked"))return(obj->checked);
	return(0);
}

int PDGLUI_WidgetTree_SetPropertyI(PDGLUI_Widget *obj, char *var, int val)
{
	if(!strcmp(var, "checked"))obj->checked=val;
	return(0);
}

PDGLUI_WidgetType pdglui_tree_methods=
{
NULL,
"tree",

PDGLUI_WidgetTree_Render,
PDGLUI_WidgetTree_HandleInput,
PDGLUI_WidgetTree_SizeWidget,
PDGLUI_WidgetTree_Begin,
PDGLUI_WidgetTree_End,
PDGLUI_WidgetTree_AddWidget,
PDGLUI_WidgetTree_GetPropertyS,
PDGLUI_WidgetTree_SetPropertyS,
PDGLUI_WidgetTree_GetPropertyI,
PDGLUI_WidgetTree_SetPropertyI
};

int PDGLUI_WidgetTree_Init()
{
	PDGLUI_Widget_RegisterType(&pdglui_tree_methods);
}
