#include <btgecl.h>

// extern int btgeclui_context->fgcolor;	//text fg color
// extern int btgeclui_context->bgcolor;	//text bg color
// extern int btgeclui_context->wbgcolor;	//widget bg color

int BTCLUI_WidgetBox_HandleInput(BTCLUI_Widget *obj,
	BTCLUI_InputState *state, int ox, int oy)
{
	BTCLUI_Widget *cur;
	int cx, cy;
	int i, j, k;
	unsigned short *ks;
	void *p;

	cx=ox;
	cy=oy;
	if(!strcmp(obj->type, "vbox"))
		cy+=obj->ys;
	cur=obj->wchild;

	while(cur)
	{
		if(!strcmp(obj->type, "vbox"))
			cy-=cur->ys;
		BTCLUI_Widget_HandleInput(cur, state, cx, cy);
		if(!strcmp(obj->type, "hbox"))
			cx+=cur->xs;
		cur=cur->wnext;
	}

	return(0);
}

int BTCLUI_WidgetBox_Render(BTCLUI_Widget *obj, int ox, int oy)
{
	BTCLUI_Widget *cur;
	int cx, cy;
	int r, g, b;
	int i, j, k;
	char *s;

	cx=ox;
	cy=oy;
	if(!strcmp(obj->type, "vbox"))
		cy+=obj->ys;

	cur=obj->wchild;
	while(cur)
	{
		if(!strcmp(obj->type, "vbox"))
			cy-=cur->ys;
		BTCLUI_Widget_Render(cur, cx, cy);
		if(!strcmp(obj->type, "hbox"))
			cx+=cur->xs;
		cur=cur->wnext;
	}
	return(0);
}

int BTCLUI_WidgetBox_SizeWidget(BTCLUI_Widget *obj)
{
	BTCLUI_Widget *wcur;

	if(!strcmp(obj->type, "hbox"))
	{
		obj->xs=0;
		obj->ys=0;
		wcur=obj->wchild;
		while(wcur)
		{
			BTCLUI_Widget_SizeWidget(wcur);
			obj->xs+=wcur->xs;
			if(wcur->ys>obj->ys)obj->ys=wcur->ys;
			wcur=wcur->wnext;
		}
	}

	if(!strcmp(obj->type, "vbox"))
	{
		obj->xs=0;
		obj->ys=0;
		wcur=obj->wchild;
		while(wcur)
		{
			BTCLUI_Widget_SizeWidget(wcur);
			if(wcur->xs>obj->xs)
				obj->xs=wcur->xs;
			obj->ys+=wcur->ys;
			wcur=wcur->wnext;
		}
	}

	return(0);
}

int BTCLUI_WidgetBox_AddWidget(BTCLUI_Widget *tmp, BTCLUI_Widget *tmp2)
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

	if(!strcmp(tmp->type, "hbox"))
	{
		tmp->xs+=tmp2->xs;
		if(tmp2->ys>tmp->ys)
			tmp->ys=tmp2->ys;
	}else
	{
		if(tmp2->xs>tmp->xs)
			tmp->xs=tmp2->xs;
		tmp->ys+=tmp2->ys;
	}

	return(0);
}

int BTCLUI_WidgetBox_Init()
{
	static char *tylist[]={
		"box", "hbox", "vbox", NULL};
	BTCLUI_WidgetType *ty;
	int i;

	for(i=0; tylist[i]; i++)
	{
		ty=BTCLUI_GetWidgetType(tylist[i]);
		ty->Render=BTCLUI_WidgetBox_Render;
		ty->HandleInput=BTCLUI_WidgetBox_HandleInput;
		ty->Size=BTCLUI_WidgetBox_SizeWidget;
		ty->AddWidget=BTCLUI_WidgetBox_AddWidget;
	}

	return(0);
}
