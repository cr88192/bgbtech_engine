#include <pdgl.h>
#include <pdglui.h>

struct PDGLUI_WidgetMethods_s *pdglui_widgetbox_methods=NULL;

int PDGLUI_WidgetBox_HandleInput(PDGLUI_Widget *obj,
	PDGLUI_InputState *state, int ox, int oy)
{
	PDGLUI_Widget *cur;
	int cx, cy;
	int i, j, k;
	unsigned short *ks;
	void *p;

	cx=ox;
	cy=oy;
	if(!strcmp(obj->type, "vbox"))
		cy+=obj->ys;
	cur=obj->first;

	while(cur)
	{
		if(!strcmp(obj->type, "vbox"))
			cy-=cur->ys;
		PDGLUI_Widget_HandleInput(cur, state, cx, cy);
		if(!strcmp(obj->type, "hbox"))
			cx+=cur->xs;
		cur=cur->next;
	}

	return(0);
}

int PDGLUI_WidgetBox_Render(PDGLUI_Widget *obj, int ox, int oy)
{
	PDGLUI_Widget *cur;
	int cx, cy;
	int r, g, b;
	int i, j, k;
	char *s;

	cx=ox;
	cy=oy;
	if(!strcmp(obj->type, "vbox"))
		cy+=obj->ys;

	cur=obj->first;
	while(cur)
	{
		if(!strcmp(obj->type, "vbox"))
			cy-=cur->ys;
		PDGLUI_Widget_Render(cur, cx, cy);
		if(!strcmp(obj->type, "hbox"))
			cx+=cur->xs;
		cur=cur->next;
	}
	return(0);
}

int PDGLUI_WidgetBox_SizeWidget(PDGLUI_Widget *obj)
{
	PDGLUI_Widget *wcur;
	if(!strcmp(obj->type, "hbox"))
	{
		obj->xs=0;
		obj->ys=0;
		wcur=obj->first;
		while(wcur)
		{
			obj->xs+=wcur->xs;
			if(wcur->ys>obj->ys)obj->ys=wcur->ys;
			wcur=wcur->next;
		}
	}
	if(!strcmp(obj->type, "vbox"))
	{
		obj->xs=0;
		obj->ys=0;
		wcur=obj->first;
		while(wcur)
		{
			if(wcur->xs>obj->xs)
				obj->xs=wcur->xs;
			obj->ys+=wcur->ys;
			wcur=wcur->next;
		}
	}
}

PDGLUI_Widget *PDGLUI_WidgetBox_BuildBox(NetParse_Node *node,
	PDGLUI_Form *form, PDGLUI_Forms_StyleState *style)
{
	PDGLUI_Widget *tmp, *wt, *wl;
	NetParse_Node *cur;
	char *str;

	tmp=kalloc(sizeof(PDGLUI_Widget));
	tmp->type=kstrdup(node->key);
	tmp->form=form;
	tmp->funcs=pdglui_widgetbox_methods;

	wl=NULL;
	cur=node->first;
	while(cur)
	{
		wt=PDGLUI_Widget_BuildWidget(cur, form, style);
		if(wt)
		{
			wt->up=tmp;
			if(wl)
			{
				wl->next=wt;
				wt->prev=wl;
				wl=wt;
				if(!strcmp(tmp->type, "hbox"))
				{
					tmp->xs+=wt->xs;
					if(wt->ys>tmp->ys)
						tmp->ys=wt->ys;
				}else
				{
					if(wt->xs>tmp->xs)
						tmp->xs=wt->xs;
					tmp->ys+=wt->ys;
				}
			}else
			{
				tmp->first=wt;
				wl=wt;
				tmp->xs=wt->xs;
				tmp->ys=wt->ys;
			}
		}
		cur=cur->next;
	}

	return(tmp);
}


PDGLUI_Widget *PDGLUI_WidgetBox_BuildWidget(NetParse_Node *node,
	PDGLUI_Form *form, PDGLUI_Forms_StyleState *style)
{
	PDGLUI_Widget *tmp, *tmp2;
	char *str;
	int fg, bg, wbg;
	int i;

	if(!node)return(NULL);

	if(!strcmp(node->key, "hbox") || !strcmp(node->key, "vbox"))
	{
		fg=style->fgcolor;
		bg=style->bgcolor;
		wbg=style->bgcolor;

		str=NetParse_GetNodeAttr(node, "fgcolor");
		if(str)style->fgcolor=PDGLUI_Forms_DecodeColor(str);
		str=NetParse_GetNodeAttr(node, "bgcolor");
		if(str)style->bgcolor=PDGLUI_Forms_DecodeColor(str);
		str=NetParse_GetNodeAttr(node, "wbgcolor");
		if(str)style->wbgcolor=PDGLUI_Forms_DecodeColor(str);

		tmp=PDGLUI_WidgetBox_BuildBox(node, form, style);
		style->fgcolor=fg;
		style->bgcolor=bg;
		style->wbgcolor=wbg;


		return(tmp);
	}
}

int PDGLUI_WidgetBox_Init()
{
	PDGLUI_Widget_RegisterType("hbox", PDGLUI_WidgetBox_BuildWidget);
	PDGLUI_Widget_RegisterType("vbox", PDGLUI_WidgetBox_BuildWidget);

	pdglui_widgetbox_methods=kalloc(
		sizeof(struct PDGLUI_WidgetMethods_s));
	pdglui_widgetbox_methods->render=&PDGLUI_WidgetBox_Render;
	pdglui_widgetbox_methods->handle_input=&PDGLUI_WidgetBox_HandleInput;
	pdglui_widgetbox_methods->size=&PDGLUI_WidgetBox_SizeWidget;
//	pdglui_widgetbox_methods->message=&PDGLUI_WidgetBox_SendMessage;
}
