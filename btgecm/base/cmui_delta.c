#include <btgecm.h>

dyt BTCMUI_WidgetDelta(BTCMUI_Widget *widget)
{
	dyt lst, t, t0, t1, t2, t3;
	int i, fdm;

	if(!widget->old)
	{
		widget->old=gctalloc("btcmui_widget_t", sizeof(BTCMUI_Widget));
	}
	
	lst=NULL;

	if((widget->ox!=widget->old->ox) ||
		(widget->oy!=widget->old->oy))
	{
		t=dylist3s("org", dyint(widget->ox), dyint(widget->oy));
		lst=dycons(t, lst);
	}

	if((widget->xs!=widget->old->xs) ||
		(widget->ys!=widget->old->ys))
	{
		t=dylist3s("size", dyint(widget->xs), dyint(widget->ys));
		lst=dycons(t, lst);
	}

	if(widget->fgcolor!=widget->old->fgcolor)
	{
		t=dylist2s("fgcolor", dyint(widget->fgcolor));
		lst=dycons(t, lst);
	}

	if(widget->bgcolor!=widget->old->bgcolor)
	{
		t=dylist2s("bgcolor", dyint(widget->bgcolor));
		lst=dycons(t, lst);
	}

	if(widget->wbgcolor!=widget->old->wbgcolor)
	{
		t=dylist2s("wbgcolor", dyint(widget->wbgcolor));
		lst=dycons(t, lst);
	}

	if(widget->rows!=widget->old->rows)
	{
		t=dylist2s("rows", dyint(widget->rows));
		lst=dycons(t, lst);
	}

	if(widget->cols!=widget->old->cols)
	{
		t=dylist2s("cols", dyint(widget->cols));
		lst=dycons(t, lst);
	}

	if(widget->flags!=widget->old->flags)
	{
		t=dylist2s("flags", dyint(widget->flags));
		lst=dycons(t, lst);
	}

	if(widget->type!=widget->old->type)
	{
		t=dylist2s("type", dystring(widget->type));
		lst=dycons(t, lst);
	}

	if(widget->name!=widget->old->name)
	{
		t=dylist2s("name", dystring(widget->name));
		lst=dycons(t, lst);
	}

	if(widget->id!=widget->old->id)
	{
		t=dylist2s("id", dystring(widget->id));
		lst=dycons(t, lst);
	}

	if(widget->value!=widget->old->value)
	{
		t=dylist2s("value", dystring(widget->value));
		lst=dycons(t, lst);
	}

	if(widget->label!=widget->old->label)
	{
		t=dylist2s("label", dystring(widget->label));
		lst=dycons(t, lst);
	}

	if(widget->image!=widget->old->image)
	{
		t=dylist2s("image", dystring(widget->image));
		lst=dycons(t, lst);
	}

	if(!lst)return(NULL);

	t=dycons2s("widget_delta", dyint(widget->idnum), lst);
	return(t);
}

void BTCMUI_WidgetPostDelta(BTCMUI_Widget *widget)
{
	if(!widget->old)
	{
		widget->old=gctalloc("btcmui_widget_t", sizeof(BTCMUI_Widget));
	}

	widget->old->type=widget->type;
	widget->old->name=widget->name;
	widget->old->id=widget->id;
	widget->old->value=widget->value;
	widget->old->label=widget->label;
	widget->old->image=widget->image;

	widget->old->ox=widget->ox;
	widget->old->oy=widget->oy;
	widget->old->xs=widget->xs;
	widget->old->ys=widget->ys;

	widget->old->rows=widget->rows;
	widget->old->cols=widget->cols;
	widget->old->flags=widget->flags;

	widget->old->fgcolor=widget->fgcolor;
	widget->old->bgcolor=widget->bgcolor;
	widget->old->wbgcolor=widget->wbgcolor;
}

dyt BTCMUI_FormDelta(BTCMUI_Form *form)
{
	BTCMUI_Widget *cur;
	dyt lst, t, t0, t1, t2, t3;

	if(!form->old)
	{
		form->old=gctalloc("btcmui_form_t", sizeof(BTCMUI_Form));
	}
	
	lst=NULL;
	
	cur=form->first;
	while(cur)
	{
		t=BTCMUI_WidgetDelta(cur);
		if(t)lst=dycons(t, lst);
		cur=cur->fnext;
	}

	if((form->ox!=form->old->ox) ||
		(form->oy!=form->old->oy))
	{
		t=dylist3s("org", dyint(form->ox), dyint(form->oy));
		lst=dycons(t, lst);
	}

	if((form->xs!=form->old->xs) ||
		(form->ys!=form->old->ys))
	{
		t=dylist3s("size", dyint(form->xs), dyint(form->ys));
		lst=dycons(t, lst);
	}

	if(form->fgcolor!=form->old->fgcolor)
	{
		t=dylist2s("fgcolor", dyint(form->fgcolor));
		lst=dycons(t, lst);
	}

	if(form->bgcolor!=form->old->bgcolor)
	{
		t=dylist2s("bgcolor", dyint(form->bgcolor));
		lst=dycons(t, lst);
	}

	if(form->wbgcolor!=form->old->wbgcolor)
	{
		t=dylist2s("wbgcolor", dyint(form->wbgcolor));
		lst=dycons(t, lst);
	}

	if(form->flags!=form->old->flags)
	{
		t=dylist2s("flags", dyint(form->flags));
		lst=dycons(t, lst);
	}


	if(form->name!=form->old->name)
	{
		t=dylist2s("name", dystring(form->name));
		lst=dycons(t, lst);
	}

	if(form->title!=form->old->title)
	{
		t=dylist2s("title", dystring(form->title));
		lst=dycons(t, lst);
	}

	if(form->style!=form->old->style)
	{
		t=dylist2s("style", dystring(form->style));
		lst=dycons(t, lst);
	}


	if(!lst)return(NULL);

	t=dycons2s("form_delta", dyint(form->idnum), lst);
	return(t);
}

void BTCMUI_FormPostDelta(BTCMUI_Form *form)
{
	BTCMUI_Widget *cur;

	if(!form->old)
	{
		form->old=gctalloc("btcmui_form_t", sizeof(BTCMUI_Form));
	}

	cur=form->first;
	while(cur)
	{
		BTCMUI_WidgetPostDelta(cur);
		cur=cur->fnext;
	}
	
	form->old->name=form->name;
	form->old->title=form->title;
	form->old->style=form->style;

	form->old->ox=form->ox;
	form->old->oy=form->oy;
	form->old->xs=form->xs;
	form->old->ys=form->ys;

	form->old->flags=form->flags;

	form->old->fgcolor=form->fgcolor;
	form->old->bgcolor=form->bgcolor;
	form->old->wbgcolor=form->wbgcolor;
}

dyt BTCMUI_ContextDelta(BTCMUI_Context *uictx)
{
	BTCMUI_Form *cur;
	dyt t, lst;

	if(!uictx->old)
	{
		uictx->old=gctalloc("btcmui_context_t", sizeof(BTCMUI_Context));
	}

	lst=NULL;
	
	cur=uictx->forms;
	while(cur)
	{
		t=BTCMUI_FormDelta(cur);
		if(t)lst=dycons(t, lst);
		cur=cur->next;
	}

	if(uictx->fgcolor!=uictx->old->fgcolor)
	{
		t=dylist2s("fgcolor", dyint(uictx->fgcolor));
		lst=dycons(t, lst);
	}

	if(uictx->bgcolor!=uictx->old->bgcolor)
	{
		t=dylist2s("bgcolor", dyint(uictx->bgcolor));
		lst=dycons(t, lst);
	}

	if(uictx->wbgcolor!=uictx->old->wbgcolor)
	{
		t=dylist2s("wbgcolor", dyint(uictx->wbgcolor));
		lst=dycons(t, lst);
	}

	if(uictx->flags!=uictx->old->flags)
	{
		t=dylist2s("flags", dyint(uictx->flags));
		lst=dycons(t, lst);
	}

	if(!lst)return(NULL);

	t=dycons2s("uidelta", dyint(uictx->idnum), lst);
	return(t);
}

BTGE_API void BTCMUI_ContextPostDelta(BTCMUI_Context *uictx)
{
	BTCMUI_Form *cur;

	if(!uictx->old)
	{
		uictx->old=gctalloc("btcmui_context_t", sizeof(BTCMUI_Context));
	}

	cur=uictx->forms;
	while(cur)
	{
		BTCMUI_FormPostDelta(cur);
		cur=cur->next;
	}
	
	uictx->old->flags=uictx->flags;

	uictx->old->fgcolor=uictx->fgcolor;
	uictx->old->bgcolor=uictx->bgcolor;
	uictx->old->wbgcolor=uictx->wbgcolor;
}

BTGE_API dyt BTCMUI_WorldDelta(BTGE_BrushWorld *world, dyt lst)
{
	BTCMUI_Context *cur;
	dyt t;

	cur=world->guis;
	while(cur)
	{
		t=BTCMUI_ContextDelta(cur);
		if(t)lst=dycons(t, lst);
		cur=cur->next;
	}
	return(lst);
}

BTGE_API void BTCMUI_WorldPostDelta(BTGE_BrushWorld *world)
{
	BTCMUI_Context *cur;

	cur=world->guis;
	while(cur)
	{
		BTCMUI_ContextPostDelta(cur);
		cur=cur->next;
	}
}

BTGE_API void BTCMUI_WorldDecodeDelta(BTGE_BrushWorld *world, dyt msg)
{
	BTCMUI_Context *uictx;
	dyt c;
	int i;

	printf("BTCMUI_DecodeUIDelta: ");
	dysPrintln(msg);

	i=dyintv(dycadr(msg));
	if(i<=0)return;
//	uictx=BTCMUI_GetUIContext(world, i);
	uictx=BTCMUI_WorldLookupUIContext(world, i);
	if(!uictx)return;
	
//	ent->time=btclMsgTime(wrl);
	
	c=dycddr(msg);
	while(c)
	{
		BTCMUI_DecodeUIDeltaAttr(world, uictx, dycar(c));
		c=dycdr(c);
	}
}


void BTCMUI_DecodeFormWidgetDeltaAttr(BTGE_BrushWorld *wrl,
	BTCMUI_Context *uictx, BTCMUI_Form *form,
	BTCMUI_Widget *widget, dyt msg)
{
	BTCMUI_Widget *obj;
	dyt v, c;
	int i;

	v=dycar(msg);

	if(v==dysym("parent"))
	{
		i=dyintv(dycadr(msg));
//		obj=BTCMUI_GetUIFormWidget(wrl, uictx, form, i);
		obj=BTCMUI_LookupFormWidget(form, i);
		BTCMUI_WidgetSetParent(widget, obj);
		return;
	}

	if(v==dysym("org"))
	{
		widget->ox=dyintv(dycadr(msg));
		widget->oy=dyintv(dycaddr(msg));
		return;
	}

	if(v==dysym("size"))
	{
		widget->xs=dyintv(dycadr(msg));
		widget->ys=dyintv(dycaddr(msg));
		return;
	}

	if(v==dysym("rows"))
		{ widget->rows=dyintv(dycadr(msg)); return; }
	if(v==dysym("cols"))
		{ widget->cols=dyintv(dycadr(msg)); return; }

	if(v==dysym("fgcolor"))
		{ widget->fgcolor=dyintv(dycadr(msg)); return; }
	if(v==dysym("bgcolor"))
		{ widget->bgcolor=dyintv(dycadr(msg)); return; }
	if(v==dysym("wbgcolor"))
		{ widget->wbgcolor=dyintv(dycadr(msg)); return; }

//	if(v==dysym("type"))
//		{ BTCMUI_WidgetSetType(widget, dystringv(dycadr(msg))); return; }

	if(v==dysym("type"))
		{ widget->type=dystrdup(dystringv(dycadr(msg))); return; }
	if(v==dysym("name"))
		{ widget->name=dystrdup(dystringv(dycadr(msg))); return; }
	if(v==dysym("id"))
		{ widget->id=dystrdup(dystringv(dycadr(msg))); return; }
	if(v==dysym("value"))
		{ widget->value=dystrdup(dystringv(dycadr(msg))); return; }
	if(v==dysym("label"))
		{ widget->label=dystrdup(dystringv(dycadr(msg))); return; }
	if(v==dysym("image"))
		{ widget->image=dystrdup(dystringv(dycadr(msg))); return; }

}

void BTCMUI_DecodeFormWidgetDelta(BTGE_BrushWorld *wrl,
	BTCMUI_Context *uictx, BTCMUI_Form *form, dyt msg)
{
	BTCMUI_Widget *obj;
	dyt c;
	int i;

	printf("BTCMUI_DecodeFormWidgetDelta: ");
	dysPrintln(msg);

	i=dyintv(dycadr(msg));
	if(i<=0)return;
//	ent=BTCMUI_GetUIContext(wrl, i);
//	obj=BTCMUI_LookupUIFormWidget(wrl, uictx, form, i);
//	obj=BTCMUI_GetUIFormWidget(wrl, uictx, form, i);
	obj=BTCMUI_LookupFormWidget(form, i);
	
//	ent->time=btclMsgTime(wrl);
	
	c=dycddr(msg);
	while(c)
	{
		BTCMUI_DecodeFormWidgetDeltaAttr(wrl,
			uictx, form, obj, dycar(c));
		c=dycdr(c);
	}
}

void BTCMUI_DecodeFormEventAttr(BTGE_BrushWorld *wrl,
	BTCMUI_Context *uictx, BTCMUI_Form *form,
	BTCMUI_Event *ev, dyt msg)
{
	dyt v;
	int i;
	
	v=dycar(msg);

	if(v==dysym("widget_idnum"))
	{
//		BTCMUI_DecodeFormWidgetDelta(wrl, uictx, form, msg);
		i=dyintv(dycadr(msg));
		ev->widget=BTCMUI_LookupFormWidget(form, i);
		return;
	}

	if(v==dysym("event_name"))
	{
		ev->event=dystrdup(dystringv(dycadr(msg)));
		return;
	}

	if(v==dysym("event_id"))
	{
		i=dyintv(dycadr(msg));
		ev->eventid=i;
		return;
	}

	if(v==dysym("event_ival"))
	{
		i=dyintv(dycadr(msg));
		ev->ival=i;
		return;
	}

	if(v==dysym("event_sval"))
	{
		ev->sval=dystrdup(dystringv(dycadr(msg)));
		return;
	}

}

void BTCMUI_DecodeFormEvent(BTGE_BrushWorld *wrl,
	BTCMUI_Context *uictx, BTCMUI_Form *form, dyt msg)
{
	BTCMUI_Event *ev;
	dyt c;
	int i;

//	printf("BTCMUI_DecodeFormDelta: ");
//	dysPrintln(msg);

	ev=gctalloc("btcmui_event_t", sizeof(BTCMUI_Event));
	ev->form=form;

	c=dycdr(msg);
	while(c)
	{
		BTCMUI_DecodeFormEventAttr(wrl, uictx, form, ev, dycar(c));
		c=dycdr(c);
	}
	
	if(form->HandleEvent)
	{
		form->HandleEvent(form, ev);
		gcfree(ev);
	}else
	{
		ev->next=form->event;
		form->event=ev;
	}
}

void BTCMUI_DecodeFormDeltaAttr(BTGE_BrushWorld *wrl,
	BTCMUI_Context *uictx, BTCMUI_Form *form, dyt msg)
{
	dyt v;
	
	v=dycar(msg);

	if(v==dysym("widget_delta"))
		{ BTCMUI_DecodeFormWidgetDelta(wrl, uictx, form, msg); return; }
	if(v==dysym("event"))
		{ BTCMUI_DecodeFormEvent(wrl, uictx, form, msg); return; }

	if(v==dysym("org"))
	{
		form->ox=dyintv(dycadr(msg));
		form->oy=dyintv(dycaddr(msg));
		return;
	}

	if(v==dysym("size"))
	{
		form->xs=dyintv(dycadr(msg));
		form->ys=dyintv(dycaddr(msg));
		return;
	}

	if(v==dysym("fgcolor"))
		{ form->fgcolor=dyintv(dycadr(msg)); return; }
	if(v==dysym("bgcolor"))
		{ form->bgcolor=dyintv(dycadr(msg)); return; }
	if(v==dysym("wbgcolor"))
		{ form->wbgcolor=dyintv(dycadr(msg)); return; }

	if(v==dysym("style"))
		{ form->style=dystrdup(dystringv(dycadr(msg))); return; }

//	if(v==dysym("org"))
//		{ BTCMUI_DecodeMsgVec3(ent->org, msg); return; }
}

void BTCMUI_DecodeFormDelta(BTGE_BrushWorld *wrl,
	BTCMUI_Context *uictx, dyt msg)
{
	BTCMUI_Form *frm;
	dyt c;
	int i;

	printf("BTCMUI_DecodeFormDelta: ");
	dysPrintln(msg);

	i=dyintv(dycadr(msg));
	if(i<=0)return;
//	ent=BTCMUI_GetUIContext(wrl, i);
//	obj=BTCMUI_GetUIWidget(wrl, uictx, i);

//	frm=BTCMUI_LookupUIForm(wrl, uictx, i);
//	frm=BTCMUI_GetUIForm(wrl, uictx, i);
	frm=BTCMUI_LookupUIForm(uictx, i);
	
//	ent->time=btclMsgTime(wrl);
	
	c=dycddr(msg);
	while(c)
	{
		BTCMUI_DecodeFormDeltaAttr(wrl, uictx, frm, dycar(c));
		c=dycdr(c);
	}
}

void BTCMUI_DecodeUIDeltaAttr(BTGE_BrushWorld *wrl,
	BTCMUI_Context *uictx, dyt msg)
{
	dyt v;

	v=dycar(msg);
	if(v==dysym("form_delta"))
		{ BTCMUI_DecodeFormDelta(wrl, uictx, msg); return; }
}
