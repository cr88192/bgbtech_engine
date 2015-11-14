#include <btgecl.h>

int BTCLUI_WidgetUnion_HandleInput(BTCLUI_Widget *obj,
	BTCLUI_InputState *state, int ox, int oy)
{
	BTCLUI_Widget *cur;

	if(!obj->value)return(0);

	cur=obj->wchild;
	while(cur)
	{
		if(strcmp(cur->id, obj->value))
		{
			cur=cur->wnext;
			continue;
		}

		BTCLUI_Widget_HandleInput(cur, state, ox, oy);
		break;
	}

	return(0);
}

int BTCLUI_WidgetUnion_Render(BTCLUI_Widget *obj, int ox, int oy)
{
	BTCLUI_Widget *cur;

	if(!obj->value)return(0);

	cur=obj->wchild;
	while(cur)
	{
		if(strcmp(cur->id, obj->value))
		{
			cur=cur->wnext;
			continue;
		}

		BTCLUI_Widget_Render(cur, ox, oy);
		break;
	}
	return(0);
}

int BTCLUI_WidgetUnion_SizeWidget(BTCLUI_Widget *obj)
{
	BTCLUI_Widget *wcur;

	obj->xs=0;
	obj->ys=0;
	wcur=obj->wchild;
	while(wcur)
	{
		BTCLUI_Widget_SizeWidget(wcur);
		if(wcur->xs>obj->xs)obj->xs=wcur->xs;
		if(wcur->ys>obj->ys)obj->ys=wcur->ys;
		wcur=wcur->wnext;
	}
	
	return(0);
}

int BTCLUI_WidgetUnion_AddWidget(BTCLUI_Widget *tmp, BTCLUI_Widget *tmp2)
{
	BTCLUI_Widget *cur;

	BTCLUI_Widget_SizeWidget(tmp2);

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

	if(tmp2->xs>tmp->xs)tmp->xs=tmp2->xs;
	if(tmp2->ys>tmp->ys)tmp->ys=tmp2->ys;
	return(0);
}

int BTCLUI_WidgetUnion_Init()
{
	static char *tylist[]={"union", NULL};
	BTCLUI_WidgetType *ty;
	int i;

	for(i=0; tylist[i]; i++)
	{
		ty=BTCLUI_GetWidgetType(tylist[i]);
		ty->Render=BTCLUI_WidgetUnion_Render;
		ty->HandleInput=BTCLUI_WidgetUnion_HandleInput;
		ty->Size=BTCLUI_WidgetUnion_SizeWidget;
		ty->AddWidget=BTCLUI_WidgetUnion_AddWidget;
	}

	return(0);
}
