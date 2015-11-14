#include <btgecl.h>

// extern int btgeclui_context->fgcolor;	//text fg color
// extern int btgeclui_context->bgcolor;	//text bg color
// extern int btgeclui_context->wbgcolor;	//widget bg color

int BTGECLUI_WidgetBox_HandleInput(BTGECLUI_Widget *obj,
	BTGECLUI_InputState *state, int ox, int oy)
{
	BTGECLUI_Widget *cur;
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
		BTGECLUI_Widget_HandleInput(cur, state, cx, cy);
		if(!strcmp(obj->type, "hbox"))
			cx+=cur->xs;
		cur=cur->next;
	}

	return(0);
}

int BTGECLUI_WidgetBox_Render(BTGECLUI_Widget *obj, int ox, int oy)
{
	BTGECLUI_Widget *cur;
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
		BTGECLUI_Widget_Render(cur, cx, cy);
		if(!strcmp(obj->type, "hbox"))
			cx+=cur->xs;
		cur=cur->next;
	}
	return(0);
}

int BTGECLUI_WidgetBox_SizeWidget(BTGECLUI_Widget *obj)
{
	BTGECLUI_Widget *wcur;
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

#if 0
//BTGECLUI_Widget *BTGECLUI_WidgetBox_BuildBox(NetParse_Node *node,
//	BTGECLUI_Form *form, BTGECLUI_Forms_StyleState *style)
{
	BTGECLUI_Widget *tmp, *wt, *wl;
	NetParse_Node *cur;
	char *str;

	tmp=gcalloc(sizeof(BTGECLUI_Widget));
	tmp->type=dystrdup(node->key);
	tmp->form=form;
	tmp->funcs=btgeclui_context->widgetbox_methods;

	wl=NULL;
	cur=node->first;
	while(cur)
	{
		wt=BTGECLUI_Widget_BuildWidget(cur, form, style);
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

//BTGECLUI_Widget *BTGECLUI_WidgetBox_BuildWidget(NetParse_Node *node,
//	BTGECLUI_Form *form, BTGECLUI_Forms_StyleState *style)
{
	BTGECLUI_Widget *tmp, *tmp2;
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
		if(str)style->fgcolor=BTGECLUI_Forms_DecodeColor(str);
		str=NetParse_GetNodeAttr(node, "bgcolor");
		if(str)style->bgcolor=BTGECLUI_Forms_DecodeColor(str);
		str=NetParse_GetNodeAttr(node, "wbgcolor");
		if(str)style->wbgcolor=BTGECLUI_Forms_DecodeColor(str);

		tmp=BTGECLUI_WidgetBox_BuildBox(node, form, style);
		style->fgcolor=fg;
		style->bgcolor=bg;
		style->wbgcolor=wbg;


		return(tmp);
	}
}

#endif


BTGECLUI_Widget *BTGECLUI_WidgetBox_Begin(char *type, char *id)
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

int BTGECLUI_WidgetBox_AddWidget(BTGECLUI_Widget *tmp, BTGECLUI_Widget *tmp2)
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

int BTGECLUI_WidgetBox_End(BTGECLUI_Widget *tmp)
{
	int i;
	return(0);
}

char *BTGECLUI_WidgetBox_GetPropertyS(BTGECLUI_Widget *obj, char *var)
{
	return(NULL);
}

int BTGECLUI_WidgetBox_SetPropertyS(BTGECLUI_Widget *obj, char *var, char *val)
{
	return(0);
}

int BTGECLUI_WidgetBox_GetPropertyI(BTGECLUI_Widget *obj, char *var)
{
	return(0);
}

int BTGECLUI_WidgetBox_SetPropertyI(BTGECLUI_Widget *obj, char *var, int val)
{
	return(0);
}

BTGECLUI_WidgetType btgeclui_context->box_methods=
{
NULL,
"box;hbox;vbox",

BTGECLUI_WidgetBox_Render,
BTGECLUI_WidgetBox_HandleInput,
NULL,
BTGECLUI_WidgetBox_Begin,
BTGECLUI_WidgetBox_End,
BTGECLUI_WidgetBox_AddWidget,
BTGECLUI_WidgetBox_GetPropertyS,
BTGECLUI_WidgetBox_SetPropertyS,
BTGECLUI_WidgetBox_GetPropertyI,
BTGECLUI_WidgetBox_SetPropertyI
};

int BTGECLUI_WidgetBox_Init()
{
	BTGECLUI_Widget_RegisterType(&btgeclui_context->box_methods);
}
