#include <btgecm.h>

BTCMUI_WidgetType *btcmui_widget_types=NULL;

BTCMUI_WidgetType *BTCMUI_LookupWidgetType(char *name)
{
	BTCMUI_WidgetType *cur;
	
	cur=btcmui_widget_types;
	while(cur)
	{
		if(!strcmp(cur->name, name))
			return(cur);
		cur=cur->next;
	}
	return(NULL);
}

BTCMUI_WidgetType *BTCMUI_GetWidgetType(char *name)
{
	BTCMUI_WidgetType *tmp;

	tmp=BTCMUI_LookupWidgetType(name);
	if(tmp)return(tmp);
	
	tmp=gctalloc("btcmui_widgettype_t", sizeof(BTCMUI_WidgetType));
	tmp->name=dystrdup(name);
	tmp->next=btcmui_widget_types;
	btcmui_widget_types=tmp;
	return(tmp);
}

BTCMUI_Form *BTCMUI_LookupUIForm(BTCMUI_Context *uictx,
	int idnum)
{
	BTCMUI_Form *cur;
	
	cur=uictx->forms;
	while(cur)
	{
		if(cur->idnum==idnum)
			return(cur);
		cur=cur->next;
	}
	return(NULL);
}

BTCMUI_Widget *BTCMUI_LookupFormWidget(BTCMUI_Form *form,
	int idnum)
{
	BTCMUI_Widget *cur;
	
	cur=form->first;
	while(cur)
	{
		if(cur->idnum==idnum)
			return(cur);
		cur=cur->fnext;
	}
	return(NULL);
}

BTGE_API BTCMUI_Context *BTCMUI_NewUIContext(int idnum)
{
	BTCMUI_Context *tmp;

	tmp=gctalloc("btcmui_context_t", sizeof(BTCMUI_Context));
	tmp->idnum=idnum;

	tmp->fgcolor=0x000000;	//text fg color
	tmp->bgcolor=0xFFFFFF;	//text bg color
	tmp->wbgcolor=0xC0C0C0;	//widget bg color

	return(tmp);
}

BTGE_API BTCMUI_Context *BTCMUI_WorldNewUIContext(
	BTGE_BrushWorld *world, int idnum)
{
	BTCMUI_Context *tmp;

	tmp=BTCMUI_NewUIContext(idnum);
	tmp->world=world;
	tmp->next=world->guis;
	world->guis=tmp;
	return(tmp);
}

BTGE_API void BTCMUI_UnlinkUIContext(BTCMUI_Context *uictx)
{
	BTCMUI_Context *cur, *prv;

	if(!uictx || !uictx->world)
		return;

	cur=uictx->world->guis; prv=NULL;
	while(cur)
	{
		if(cur==uictx)
			break;
		prv=cur;
		cur=cur->next;
	}
	
	if(cur)
	{
		if(prv)
			{ prv->next=cur->next; }
		else
			{ cur->world->guis=cur->next; }
		cur->world=NULL;
	}
}

BTGE_API BTCMUI_Context *BTCMUI_WorldLookupUIContext(
	BTGE_BrushWorld *world, int idnum)
{
	BTCMUI_Context *cur;

	cur=world->guis;
	while(cur)
	{
		if(cur->idnum==idnum)
			return(cur);
		cur=cur->next;
	}
	return(NULL);
}

BTGE_API BTCMUI_Context *BTCMUI_WorldGetUIContext(
	BTGE_BrushWorld *world, int idnum)
{
	BTCMUI_Context *tmp;
	
	tmp=BTCMUI_WorldLookupUIContext(world, idnum);
	if(tmp)return(tmp);
	
	tmp=BTCMUI_WorldNewUIContext(world, idnum);
	return(tmp);
}

BTGE_API BTCMUI_Form *BTCMUI_NewForm(
	BTCMUI_Context *uictx, int idnum)
{
	BTCMUI_Form *tmp;

	tmp=gctalloc("btcmui_form_t", sizeof(BTCMUI_Form));
	tmp->idnum=idnum;

	tmp->fgcolor=uictx->fgcolor;
	tmp->bgcolor=uictx->bgcolor;
	tmp->wbgcolor=uictx->wbgcolor;

	tmp->next=uictx->forms;
	uictx->forms=tmp;

//	uictx->stackpos=0;

	return(tmp);
}

BTGE_API BTCMUI_Widget *BTCMUI_NewWidget(
	BTCMUI_Context *uictx, BTCMUI_Form *form, int idnum)
{
	BTCMUI_Widget *tmp;

	tmp=gctalloc("btcmui_widget_t", sizeof(BTCMUI_Widget));
	tmp->idnum=idnum;

	tmp->fgcolor=form->fgcolor;
	tmp->bgcolor=form->bgcolor;
	tmp->wbgcolor=form->wbgcolor;

	tmp->fnext=form->first;
	form->first=tmp;

//	uictx->stackpos=0;

	return(tmp);
}

BTGE_API void BTCMUI_UnlinkWidget(
	BTCMUI_Widget *widget)
{
	BTCMUI_Widget *cur, *prv, *nxt;
	
	if(widget->wparent)
	{
		cur=widget->wparent->wchild; prv=NULL;
		while(cur && (cur!=widget))
			{ prv=cur; cur=cur->wnext; }
		if(cur)
		{
			if(prv)
				{ prv->wnext=cur->wnext; }
			else
				{ widget->wparent->wchild=cur->wnext; }
			widget->wparent=NULL;
		}
	}else
	{
		cur=widget->form->root; prv=NULL;
		while(cur && (cur!=widget))
			{ prv=cur; cur=cur->wnext; }
		if(cur)
		{
			if(prv)
				{ prv->wnext=cur->wnext; }
			else
				{ widget->form->root=cur->wnext; }
		}
	}
}

BTGE_API void BTCMUI_WidgetSetParent(
	BTCMUI_Widget *widget, BTCMUI_Widget *other)
{
	BTCMUI_UnlinkWidget(widget);
	
	if(other)
	{
		widget->wnext=other->wchild;
		other->wchild=widget;
		widget->wparent=other;
	}else
	{
		widget->wnext=widget->form->root;
		widget->form->root=widget;
	}
}
