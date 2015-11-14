#include <pdgl.h>
#include <pdglui.h>

struct PDGLUI_WidgetMethods_s *pdglui_widgettree_methods=NULL;

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
			p=NetVal_WrapBool(obj->checked);
			PDGLUI_Forms_Message(obj->form, obj->id, "checked", 1, &p);

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

PDGLUI_Widget *PDGLUI_WidgetTree_BuildWidget(NetParse_Node *node,
	PDGLUI_Form *form, PDGLUI_Forms_StyleState *style)
{
	PDGLUI_Widget *tmp, *tmp2;
	char *str;
	int fg, bg, wbg;
	int i;

	if(!node)return(NULL);

	if(!strcmp(node->key, "tree"))
	{
		tmp=kalloc(sizeof(PDGLUI_Widget));
		tmp->type=kstrdup("tree");
		tmp->form=form;
		tmp->funcs=pdglui_widgettree_methods;

		str=NetParse_GetNodeAttr(node, "id");
		if(str)tmp->id=kstrdup(str);

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
		kalloc(sizeof(struct PDGLUI_WidgetMethods_s));
	pdglui_widgettree_methods->render=&PDGLUI_WidgetTree_Render;
	pdglui_widgettree_methods->handle_input=&PDGLUI_WidgetTree_HandleInput;
	pdglui_widgettree_methods->size=&PDGLUI_WidgetTree_SizeWidget;
//	pdglui_widgettree_methods->message=&PDGLUI_WidgetTree_SendMessage;
}
