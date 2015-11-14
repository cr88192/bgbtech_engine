#include <lbxgl2.h>
#include <btgecl.h>

extern BTCL_World *btcl_world;
extern PDGLUI_InputState *btcl_state;
extern PDGLUI_Camera *btcl_cam;
extern LBXGL_BrushWorld *btcl_brushWorld;
extern float btcl_dt;

BTCLUI_Context *BTCL_LookupUIContext(BTCL_World *wrl, int idnum)
{
	BTCLUI_Context *cur;
	
	cur=wrl->guictx;
	while(cur)
	{
		if(cur->idnum==idnum)
			return(cur);
		cur=cur->next;
	}
	return(NULL);
}

BTCLUI_Context *BTCL_GetUIContext(BTCL_World *wrl, int idnum)
{
	BTCLUI_Context *ent;

	ent=BTCL_LookupUIContext(wrl, idnum);
	if(ent)return(ent);
	
	BTCLUI_Init();
	ent=gctalloc("btclui_context_t", sizeof(BTCLUI_Context));
	ent->idnum=idnum;
//	ent->flags|=BTCL_EFL_DIRTY;

	ent->next=wrl->guictx;
	wrl->guictx=ent;
	
	return(ent);
}

void BTCL_DeleteUIContext(BTCL_World *wrl, int idnum)
{
	BTCLUI_Context *cur, *prv;
	
	cur=wrl->guictx; prv=NULL;
	while(cur)
	{
		if(cur->idnum==idnum)
		{
			if(prv) { prv->next=cur->next; }
			else { wrl->guictx=cur->next; }
			BTCLUI_DeleteContext(cur);
			return;
		}
		prv=cur;
		cur=cur->next;
	}
	return;
}

BTCLUI_Widget *BTCL_GetUIWidget(BTCL_World *wrl,
	BTCLUI_Context *uictx, int idnum)
{
	return(NULL);
}

BTCLUI_Form *BTCL_LookupUIForm(BTCL_World *wrl,
	BTCLUI_Context *uictx, int idnum)
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

BTCLUI_Form *BTCL_GetUIForm(BTCL_World *wrl,
	BTCLUI_Context *uictx, int idnum)
{
	BTCLUI_Form *tmp;
	
	tmp=BTCL_LookupUIForm(wrl, uictx, idnum);
	if(tmp)return(tmp);
	
	tmp=BTCLUI_NewForm(uictx, idnum);
	return(tmp);
}

BTCLUI_Widget *BTCL_LookupUIFormWidget(BTCL_World *wrl,
	BTCLUI_Context *uictx, BTCLUI_Form *form, int idnum)
{
	BTCLUI_Widget *cur;
	
	cur=form->first;
	while(cur)
	{
		if(cur->idnum==idnum)
			return(cur);
		cur=cur->fnext;
	}
	return(NULL);
}

BTCLUI_Widget *BTCL_GetUIFormWidget(BTCL_World *wrl,
	BTCLUI_Context *uictx, BTCLUI_Form *form, int idnum)
{
	BTCLUI_Widget *tmp;

	tmp=BTCL_LookupUIFormWidget(wrl, uictx, form, idnum);
	if(tmp)return(tmp);
	
	tmp=BTCLUI_NewWidget(uictx, form, idnum);
	return(tmp);
}

void BTCL_DecodeFormWidgetDeltaAttr(BTCL_World *wrl,
	BTCLUI_Context *uictx, BTCLUI_Form *form,
	BTCLUI_Widget *widget, dyt msg)
{
	BTCLUI_Widget *obj;
	dyt v, c;
	int i;

	v=dycar(msg);

	if(v==dysym("parent"))
	{
		i=dyintv(dycadr(msg));
		obj=BTCL_GetUIFormWidget(wrl, uictx, form, i);
		BTCLUI_WidgetSetParent(widget, obj);
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

	if(v==dysym("type"))
		{ BTCLUI_WidgetSetType(widget, dystringv(dycadr(msg))); return; }

//	if(v==dysym("type"))
//		{ widget->type=dystrdup(dystringv(dycadr(msg))); return; }
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

void BTCL_DecodeFormWidgetDelta(BTCL_World *wrl,
	BTCLUI_Context *uictx, BTCLUI_Form *form, dyt msg)
{
	BTCLUI_Widget *obj;
	dyt c;
	int i;

	printf("BTCL_DecodeFormWidgetDelta: ");
	dysPrintln(msg);

	i=dyintv(dycadr(msg));
	if(i<=0)return;
//	ent=BTCL_GetUIContext(wrl, i);
//	obj=BTCL_LookupUIFormWidget(wrl, uictx, form, i);
	obj=BTCL_GetUIFormWidget(wrl, uictx, form, i);
	
//	ent->time=btclMsgTime(wrl);
	
	c=dycddr(msg);
	while(c)
	{
		BTCL_DecodeFormWidgetDeltaAttr(wrl,
			uictx, form, obj, dycar(c));
		c=dycdr(c);
	}
}

void BTCL_DecodeFormDeleteWidget(BTCL_World *wrl,
	BTCLUI_Context *uictx, BTCLUI_Form *form, dyt msg)
{
	BTCLUI_Widget *obj;
	int i;

	printf("BTCL_DecodeFormDeleteWidget: ");
	dysPrintln(msg);

	i=dyintv(dycadr(msg));
	if(i<=0)return;
	obj=BTCL_LookupUIFormWidget(wrl, uictx, form, i);
	if(!obj)return;
	BTCLUI_UnlinkWidget(obj);
	BTCLUI_FreeWidget(obj);
}

#if 0
// void BTCL_DecodeWidgetDelta(BTCL_World *wrl,
	BTCLUI_Context *uictx, dyt msg)
{
	BTCLUI_Widget *obj;
	dyt c;
	int i;

	printf("BTCL_DecodeUIDelta: ");
	dysPrintln(msg);

	i=dyintv(dycadr(msg));
	if(i<=0)return;
//	ent=BTCL_GetUIContext(wrl, i);
//	obj=BTCL_GetUIWidget(wrl, uictx, i);
	
//	ent->time=btclMsgTime(wrl);
	
	c=dycddr(msg);
	while(c)
	{
		BTCL_DecodeUIDeltaAttr(wrl, ent, dycar(c));
		c=dycdr(c);
	}
}
#endif

void BTCL_DecodeFormDeltaAttr(BTCL_World *wrl,
	BTCLUI_Context *uictx, BTCLUI_Form *form, dyt msg)
{
	dyt v;
	
	v=dycar(msg);

	if(v==dysym("widget_delta"))
		{ BTCL_DecodeFormWidgetDelta(wrl, uictx, form, msg); return; }
	if(v==dysym("delete_widget"))
		{ BTCL_DecodeFormDeleteWidget(wrl, uictx, form, msg); return; }

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
//		{ BTCL_DecodeMsgVec3(ent->org, msg); return; }
}

void BTCL_DecodeFormDelta(BTCL_World *wrl,
	BTCLUI_Context *uictx, dyt msg)
{
	BTCLUI_Form *frm;
	dyt c;
	int i;

	printf("BTCL_DecodeFormDelta: ");
	dysPrintln(msg);

	i=dyintv(dycadr(msg));
	if(i<=0)return;
//	ent=BTCL_GetUIContext(wrl, i);
//	obj=BTCL_GetUIWidget(wrl, uictx, i);

//	frm=BTCL_LookupUIForm(wrl, uictx, i);
	frm=BTCL_GetUIForm(wrl, uictx, i);
	
//	ent->time=btclMsgTime(wrl);
	
	c=dycddr(msg);
	while(c)
	{
		BTCL_DecodeFormDeltaAttr(wrl, uictx, frm, dycar(c));
		c=dycdr(c);
	}
}

void BTCL_DecodeDeleteForm(BTCL_World *wrl,
	BTCLUI_Context *uictx, dyt msg)
{
	BTCLUI_Form *frm;
	dyt c;
	int i;

	printf("BTCL_DecodeDeleteForm: ");
	dysPrintln(msg);

	i=dyintv(dycadr(msg));
	if(i<=0)return;

	frm=BTCL_LookupUIForm(wrl, uictx, i);
	if(!frm)return;
	
	BTCLUI_DeleteForm(frm);
}

void BTCL_DecodeUIDeltaAttr(BTCL_World *wrl,
	BTCLUI_Context *uictx, dyt msg)
{
	dyt v;

	v=dycar(msg);
	if(v==dysym("form_delta"))
		{ BTCL_DecodeFormDelta(wrl, uictx, msg); return; }
	if(v==dysym("delete_form"))
		{ BTCL_DecodeDeleteForm(wrl, uictx, msg); return; }
}

void BTCL_DecodeUIDelta(BTCL_World *wrl, dyt msg)
{
	BTCLUI_Context *uictx;
	dyt c;
	int i;

	printf("BTCL_DecodeUIDelta: ");
	dysPrintln(msg);

	i=dyintv(dycadr(msg));
	if(i<=0)return;
	uictx=BTCL_GetUIContext(wrl, i);
	
//	ent->time=btclMsgTime(wrl);
	
	c=dycddr(msg);
	while(c)
	{
		BTCL_DecodeUIDeltaAttr(wrl, uictx, dycar(c));
		c=dycdr(c);
	}
}

void BTCL_DecodeDeleteUI(BTCL_World *wrl, dyt msg)
{
	BTCLUI_Context *uictx;
	dyt c;
	int i;

	printf("BTCL_DecodeDeleteUI: ");
	dysPrintln(msg);

	i=dyintv(dycadr(msg));
	if(i<=0)return;
	BTCL_DeleteUIContext(wrl, i);
}


BTCL_API dyt BTCLUI_ComposeFormEventDelta(BTCL_World *wrl,
	BTCLUI_Context *uictx, BTCLUI_Form *form, BTCLUI_Event *ev)
{
	dyt t, lst;
	
	lst=NULL;

	if(ev->widget)
	{
		if(ev->widget->id)
		{
			t=dylist2s("widget_id", dystring(ev->widget->id));
			lst=dycons(t, lst);
		}else if(ev->widget->name)
		{
			t=dylist2s("widget_name", dystring(ev->widget->name));
			lst=dycons(t, lst);
		}

		t=dylist2s("widget_idnum", dyint(ev->widget->idnum));
		lst=dycons(t, lst);
	}

	t=dylist2s("event_name", dystring(ev->event));
	lst=dycons(t, lst);

	if(ev->eventid)
	{
		t=dylist2s("event_id", dyint(ev->eventid));
		lst=dycons(t, lst);
	}

	if(ev->ival)
	{
		t=dylist2s("event_ival", dyint(ev->ival));
		lst=dycons(t, lst);
	}

	if(ev->sval)
	{
		t=dylist2s("event_sval", dystring(ev->sval));
		lst=dycons(t, lst);
	}

	if(!lst)return(NULL);

	t=dyconss("event", lst);
	return(t);
}

BTCL_API dyt BTCLUI_ComposeFormDelta(BTCL_World *wrl,
	BTCLUI_Context *uictx, BTCLUI_Form *form)
{
	BTCLUI_Event *cur, *nxt;
	dyt t, lst;

	cur=form->event; lst=NULL;
	while(cur)
	{
		t=BTCLUI_ComposeFormEventDelta(wrl, uictx, form, cur);
		if(t)lst=dycons(t, lst);
//		cur=cur->next;

		nxt=cur->next;
		BTCLUI_ReleaseEvent(cur);
		cur=nxt;
	}

	form->event=NULL;

	if(!lst)return(NULL);

	t=dycons2s("form_delta", dyint(form->idnum), lst);
	return(t);
}

BTCL_API dyt BTCLUI_ComposeContextDelta(BTCL_World *wrl,
	BTCLUI_Context *uictx)
{
	BTCLUI_Form *cur;
	dyt t, lst;

	cur=uictx->forms; lst=NULL;
	while(cur)
	{
		t=BTCLUI_ComposeFormDelta(wrl, uictx, cur);
		if(t)lst=dycons(t, lst);
		cur=cur->next;
	}

	if(!lst)return(NULL);

	t=dycons2s("uidelta", dyint(uictx->idnum), lst);
	return(t);
}

BTCL_API dyt BTCLUI_ComposeUpdate(BTCL_World *wrl, dyt lst)
{
	BTCLUI_Context *cur;
	dyt t;

	cur=wrl->guictx;
	while(cur)
	{
		t=BTCLUI_ComposeContextDelta(wrl, cur);
		if(t)lst=dycons(t, lst);
		cur=cur->next;
	}

	return(lst);
}

void BTCL_DrawGUI(BTCL_World *wrl)
{
	BTCLUI_Context *uictx;
	
	if(wrl->guinum<=0)
		return;

	BTCLUI_Init();
	uictx=BTCL_LookupUIContext(wrl, wrl->guinum);
	if(!uictx)return;
	BTCLUI_DrawContext(uictx);
}

int BTCLUI_HandleInput(BTCL_World *wrl, PDGLUI_InputState *state)
{
	BTCLUI_Context *uictx;
	
	if(wrl->guinum<=0)
		return(0);

	BTCLUI_Init();
	uictx=BTCL_LookupUIContext(wrl, wrl->guinum);
	if(!uictx)return(0);
	BTCLUI_Forms_HandleInput(uictx, state);
	return(1);
}
