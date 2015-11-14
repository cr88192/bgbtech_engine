#include <pdgl.h>
#include <pdglui.h>

extern int pdglui_fgcolor;	//text fg color
extern int pdglui_bgcolor;	//text bg color
extern int pdglui_wbgcolor;	//widget bg color

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

#if 0
//PDGLUI_Widget *PDGLUI_WidgetBox_BuildBox(NetParse_Node *node,
//	PDGLUI_Form *form, PDGLUI_Forms_StyleState *style)
{
	PDGLUI_Widget *tmp, *wt, *wl;
	NetParse_Node *cur;
	char *str;

	tmp=gcalloc(sizeof(PDGLUI_Widget));
	tmp->type=dystrdup(node->key);
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

//PDGLUI_Widget *PDGLUI_WidgetBox_BuildWidget(NetParse_Node *node,
//	PDGLUI_Form *form, PDGLUI_Forms_StyleState *style)
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

#endif


PDGLUI_Widget *PDGLUI_WidgetBox_Begin(char *type, char *id)
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

int PDGLUI_WidgetBox_AddWidget(PDGLUI_Widget *tmp, PDGLUI_Widget *tmp2)
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

int PDGLUI_WidgetBox_End(PDGLUI_Widget *tmp)
{
	int i;
	return(0);
}

char *PDGLUI_WidgetBox_GetPropertyS(PDGLUI_Widget *obj, char *var)
{
	return(NULL);
}

int PDGLUI_WidgetBox_SetPropertyS(PDGLUI_Widget *obj, char *var, char *val)
{
	return(0);
}

int PDGLUI_WidgetBox_GetPropertyI(PDGLUI_Widget *obj, char *var)
{
	return(0);
}

int PDGLUI_WidgetBox_SetPropertyI(PDGLUI_Widget *obj, char *var, int val)
{
	return(0);
}

PDGLUI_WidgetType pdglui_box_methods=
{
NULL,
"box;hbox;vbox",

PDGLUI_WidgetBox_Render,
PDGLUI_WidgetBox_HandleInput,
NULL,
PDGLUI_WidgetBox_Begin,
PDGLUI_WidgetBox_End,
PDGLUI_WidgetBox_AddWidget,
PDGLUI_WidgetBox_GetPropertyS,
PDGLUI_WidgetBox_SetPropertyS,
PDGLUI_WidgetBox_GetPropertyI,
PDGLUI_WidgetBox_SetPropertyI
};

int PDGLUI_WidgetBox_Init()
{
	PDGLUI_Widget_RegisterType(&pdglui_box_methods);
}
