#include <btgecl.h>

int BTCLUI_WidgetTree_HandleInput(BTCLUI_Widget *obj,
	BTCLUI_InputState *state, int ox, int oy)
{
	BTCLUI_Widget *cur;
	int cx, cy;
	int i, j, k;
	unsigned short *ks;
	void *p;

	if(!strcmp(obj->type, "tree"))
	{
		if(BTCLUI_Widget_CheckClickedBox(ox,
			oy+obj->ys-obj->wchild->ys, 12, 12, state))
		{
			obj->checked=!obj->checked;
//			p=NetVal_WrapBool(obj->checked);
//			BTCLUI_Forms_Message(obj->form, obj->id, "checked", 1, &p);

			BTCLUI_Widget_AddEvent(obj,
				obj->checked?"clicked":"unclicked");

			BTCLUI_Widget_RSizeWidget(obj);
		}

		cx=ox;
		cy=oy+obj->ys;
		cur=obj->wchild;

		cy-=cur->ys;
		BTCLUI_Widget_HandleInput(cur, state, cx+16, cy);
		cur=cur->wnext;

		while(cur)
		{
			cy-=cur->ys;
			BTCLUI_Widget_HandleInput(cur, state, cx+32, cy);
			cur=cur->wnext;
		}
		return(0);
	}

	return(0);
}

int BTCLUI_WidgetTree_Render(BTCLUI_Widget *obj, int ox, int oy)
{
	BTCLUI_Widget *cur;
	int cx, cy;
	int r, g, b;
	int i, j, k;
	char *s;

	if(!strcmp(obj->type, "tree"))
	{
//		Draw_FillSquareRGBA(ox+obj->ox, oy+obj->oy, obj->xs, obj->ys,
//			127, 127, 127, 255);

		cx=ox+obj->ox;
		cy=oy+obj->oy+obj->ys-obj->wchild->ys;

		Draw_FillSquareRGBA(cx, cy, 12, 12,
			255, 255, 255, 255);

//		BTCLUI_Widget_Render3DBorder(cx, cy,
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

		BTCLUI_Widget_Render(obj->wchild, cx+16, cy);
		if(obj->checked)
		{
			cur=obj->wchild->wnext;

			while(cur)
			{
				cy-=cur->ys;
				BTCLUI_Widget_Render(cur, cx+32, cy);
				cur=cur->wnext;
			}
		}

		return(0);
	}

	return(0);
}

int BTCLUI_WidgetTree_SizeWidget(BTCLUI_Widget *obj)
{
	BTCLUI_Widget *wcur;

	if(!strcmp(obj->type, "tree"))
	{
		obj->xs=obj->wchild->xs+12;
		obj->ys=obj->wchild->ys;

		if(obj->checked)
		{
			wcur=obj->wchild->wnext;
			while(wcur)
			{
				if((wcur->xs+24)>obj->xs)
					obj->xs=wcur->xs+24;
				obj->ys+=wcur->ys;
				wcur=wcur->wnext;
			}
		}
	}

	return(0);
}

int BTCLUI_WidgetTree_AddWidget(BTCLUI_Widget *tmp, BTCLUI_Widget *tmp2)
{
	BTCLUI_Widget *cur;

	cur=tmp->wchild;
	if(cur)
	{
		while(cur->wnext)cur=cur->wnext;
		cur->wnext=tmp2;
//		tmp2->prev=cur;
		tmp2->wparent=tmp;
	}else
	{
		tmp2->wnext=NULL;
//		tmp2->prev=NULL;

		tmp->wchild=tmp2;
		tmp2->wparent=tmp;
	}

	return(0);
}

int BTCLUI_WidgetTree_Init()
{
	static char *tylist[]={"tree", NULL};
	BTCLUI_WidgetType *ty;
	int i;

	for(i=0; tylist[i]; i++)
	{
		ty=BTCLUI_GetWidgetType(tylist[i]);
		ty->Render=BTCLUI_WidgetTree_Render;
		ty->HandleInput=BTCLUI_WidgetTree_HandleInput;
		ty->Size=BTCLUI_WidgetTree_SizeWidget;
		ty->AddWidget=BTCLUI_WidgetTree_AddWidget;
	}

	return(0);
}
