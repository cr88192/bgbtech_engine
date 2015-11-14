#include <lbxgl2.h>
#include <btgecl.h>

BTCLUI_WidgetType *btclui_widget_types=NULL;

void BTCLUI_Init()
{
	static int init=0;
	if(init)return;
	init=1;
	
	BTCLUI_Widget_Init();
}

BTCLUI_WidgetType *BTCLUI_LookupWidgetType(char *name)
{
	BTCLUI_WidgetType *cur;
	
	cur=btclui_widget_types;
	while(cur)
	{
		if(!strcmp(cur->name, name))
			return(cur);
		cur=cur->next;
	}
	return(NULL);
}

BTCLUI_WidgetType *BTCLUI_GetWidgetType(char *name)
{
	BTCLUI_WidgetType *tmp;

	tmp=BTCLUI_LookupWidgetType(name);
	if(tmp)return(tmp);
	
	tmp=gctalloc("btclui_widgettype_t", sizeof(BTCLUI_WidgetType));
	tmp->name=dystrdup(name);
	tmp->next=btclui_widget_types;
	btclui_widget_types=tmp;
	return(tmp);
}

BTCLUI_Form *BTCLUI_LookupUIForm(BTCLUI_Context *uictx,
	int idnum)
{
	BTCLUI_Form *cur;
	
	cur=uictx->forms;
	while(cur)
	{
		if(cur->idnum==idnum)
			return(cur);
		cur=cur->next;
	}
	return(NULL);
}

int BTCLUI_KeyDown(BTCLUI_InputState *state, int key)
{
	return(PDGL_KeyDown(key));
}

BTCL_API BTCLUI_Form *BTCLUI_NewForm(
	BTCLUI_Context *uictx, int idnum)
{
	BTCLUI_Form *tmp;

	tmp=gctalloc("btclui_form_t", sizeof(BTCLUI_Form));
	tmp->idnum=idnum;

	tmp->fgcolor=uictx->fgcolor;
	tmp->bgcolor=uictx->bgcolor;
	tmp->wbgcolor=uictx->wbgcolor;

	tmp->next=uictx->forms;
	uictx->forms=tmp;

//	uictx->stackpos=0;

	return(tmp);
}

BTCL_API BTCLUI_Widget *BTCLUI_NewWidget(
	BTCLUI_Context *uictx, BTCLUI_Form *form, int idnum)
{
	BTCLUI_Widget *tmp;

	tmp=gctalloc("btclui_widget_t", sizeof(BTCLUI_Widget));
	tmp->idnum=idnum;

	tmp->fgcolor=form->fgcolor;
	tmp->bgcolor=form->bgcolor;
	tmp->wbgcolor=form->wbgcolor;

	tmp->fnext=form->first;
	form->first=tmp;

//	uictx->stackpos=0;

	return(tmp);
}

BTCL_API BTCLUI_Widget *BTCLUI_FindFormWidgetName(
	BTCLUI_Form *form, char *name)
{
	BTCLUI_Widget *cur, *lst;
	
	cur=form->first; lst=NULL;
	while(cur)
	{
		if(cur->name && !strcmp(cur->name, name))
			{ cur->chain=lst; lst=cur; }
		cur=cur->fnext;
	}
	return(lst);
}

BTCL_API void BTCLUI_UnlinkForm(BTCLUI_Form *form)
{
	BTCLUI_Form *cur, *prv;
	
	if(form->uictx)
	{
		cur=form->uictx->forms; prv=NULL;
		while(cur && cur!=form)
			{ prv=cur; cur=cur->next; }
		if(cur)
		{
			if(prv)
				{ prv->next=cur->next; }
			else
				{ form->uictx->forms=cur->next; }
		}
		form->uictx=NULL;
	}
}

BTCL_API void BTCLUI_DeleteForm(BTCLUI_Form *form)
{
	BTCLUI_Widget *cur;
	
	BTCLUI_UnlinkForm(form);
	
	cur=form->first;
	while(cur)
	{
		BTCLUI_DeleteWidget(cur);
		cur=cur->fnext;
	}
	
	gcfree(form);
}

BTCL_API void BTCLUI_DeleteContextForms(BTCLUI_Context *uictx)
{
	BTCLUI_Form *cur, *nxt;
	
	cur=uictx->forms;
	while(cur)
	{
		nxt=cur->next;
		BTCLUI_DeleteForm(cur);
		cur=nxt;
	}

	uictx->forms=NULL;
}

BTCL_API void BTCLUI_DeleteContext(BTCLUI_Context *uictx)
{
	BTCLUI_DeleteContextForms(uictx);
	gcfree(uictx);
}

BTCL_API void BTCLUI_UnlinkWidget(BTCLUI_Widget *widget)
{
	BTCLUI_Widget *cur, *prv, *nxt;
	
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

BTCL_API void BTCLUI_FreeWidget(BTCLUI_Widget *widget)
{
	BTCLUI_Widget *cur, *prv;

	if(widget->vt && widget->vt->Destroy)
		widget->vt->Destroy(widget);
	
	if(widget->form)
	{
		cur=widget->form->first; prv=NULL;
		while(cur)
		{
			if(cur==widget)break;
			prv=cur; cur=cur->fnext;
		}
		if(cur)
		{
			if(prv)
				{ prv->fnext=cur->fnext; }
			else
				{ widget->form->first=cur->fnext; }
		}
		widget->form=NULL;
	}
	
	gcfree(widget);
}

BTCL_API void BTCLUI_DeleteWidget(BTCLUI_Widget *widget)
{
	BTCLUI_UnlinkWidget(widget);
	BTCLUI_FreeWidget(widget);
}

BTCL_API void BTCLUI_WidgetSetParent(
	BTCLUI_Widget *widget, BTCLUI_Widget *other)
{
	BTCLUI_UnlinkWidget(widget);
	
	if(other)
	{
		if(other->vt->AddWidget)
		{
			other->vt->AddWidget(other, widget);
		}else
		{
			widget->wnext=other->wchild;
			other->wchild=widget;
			widget->wparent=other;
		}
	}else
	{
		widget->wnext=widget->form->root;
		widget->form->root=widget;
	}
}

BTCL_API void BTCLUI_WidgetSetType(
	BTCLUI_Widget *widget, char *type)
{
	widget->vt=BTCLUI_GetWidgetType(type);
	widget->type=dystrdup(type);
}

BTCL_API BTCLUI_Event *BTCLUI_NewEvent()
{
	BTCLUI_Event *tmp;

	tmp=gctalloc("btclui_event_t", sizeof(BTCLUI_Event));
	return(tmp);
}

BTCL_API void BTCLUI_ReleaseEvent(BTCLUI_Event *ev)
{
	gcfree(ev);
//	ev->next=btgeclui_context->ev_free;
//	btgeclui_context->ev_free=ev;
}

BTCL_API BTCLUI_Event *BTCLUI_Forms_AddEvent(
	BTCLUI_Form *form, BTCLUI_Widget *widget,
	char *event)
{
	BTCLUI_Event *tmp, *cur;

	tmp=BTCLUI_NewEvent();
	tmp->form=form;
	tmp->widget=widget;
	tmp->event=dystrdup(event);
	tmp->eventid=BTCLUI_EV_MISC;

	if(!strcmp(event, "clicked"))tmp->eventid=BTCLUI_EV_CLICKED;
	if(!strcmp(event, "unclicked"))tmp->eventid=BTCLUI_EV_UNCLICKED;
	if(!strcmp(event, "changed"))tmp->eventid=BTCLUI_EV_CHANGED;
	if(!strcmp(event, "select"))tmp->eventid=BTCLUI_EV_SELECT;

	if(form->event)
	{
		cur=form->event;
		while(cur->next)cur=cur->next;
		cur->next=tmp;
		tmp->next=NULL;
	}else
	{
		form->event=tmp;
		tmp->next=NULL;
	}

	return(tmp);
}

BTCL_API int BTCLUI_Forms_HandleInput(
	BTCLUI_Context *uictx, BTCLUI_InputState *state)
{
	BTCLUI_Form *cur, *lfrm;

	lfrm=uictx->forms_focus;
	if(state->mb)uictx->forms_focus=NULL;

	cur=uictx->forms;
	while(cur)
	{
		if(
			(state->mx>=cur->ox) &&
			(state->my>=(cur->oy-cur->ys)) &&
			(state->mx<=(cur->ox+cur->xs)) &&
			(state->my<=cur->oy))
		{
			uictx->forms_hasfocus=1;
		}

		cur->xs=cur->root->xs;
		cur->ys=cur->root->ys;

		if(
			(state->lmx>=cur->ox) &&
			(state->lmy>=(cur->oy-cur->ys)) &&
			(state->lmx<=(cur->ox+cur->xs)) &&
			(state->lmy<=cur->oy) &&
			state->mb)
		{
			//changing focus to current form
			uictx->forms_focus=cur;

			//dragging current form
			if(!strcmp(cur->style, "box") ||
				!strcmp(cur->style, "none"))
			{
				cur->ox+=state->dx;
				cur->oy+=state->dy;
			}
		}
#if 1
		if(!strcmp(cur->style, "window") &&
			(state->lmx>=cur->ox) &&
			(state->lmy>=cur->oy) &&
			(state->lmx<=(cur->ox+cur->xs)) &&
			(state->lmy<=(cur->oy+16)) &&
			(state->mb&1))
		{
			//changing focus to current form
			uictx->forms_focus=cur;

			//dragging current form
			cur->ox+=state->dx;
			cur->oy+=state->dy;
		}
#endif

		if(uictx->forms_focus==cur)
		{
			BTCLUI_Widget_HandleInput(cur->root, state,
				cur->ox, cur->oy-cur->ys);
		}
		cur=cur->next;
	}

	cur=uictx->forms_focus;
	if(cur!=lfrm)
	{
		if(lfrm)BTCLUI_Forms_AddEvent(lfrm, NULL, "lostfocus");
		if(cur)BTCLUI_Forms_AddEvent(cur, NULL, "gotfocus");
	}

	return(0);
}
