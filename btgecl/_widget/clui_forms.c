#include <btgecl.h>

// BTGECLUI_Event *btgeclui_context->ev_free;

// BTGECLUI_Form *btgeclui_context->forms_root=NULL;
// BTGECLUI_Form *btgeclui_context->forms_focus=NULL;
// int btgeclui_context->forms_index=1;

// BTGECLUI_InputState *btgeclui_state=NULL;
// BTGECLUI_InputState *btgeclui_state2=NULL;

// extern int btgeclui_context->fgcolor;	//text fg color
// extern int btgeclui_context->bgcolor;	//text bg color
// extern int btgeclui_context->wbgcolor;	//widget bg color

// int btgeclui_context->forms_hasfocus;

BTGECLUI_API BTGECLUI_Event *BTGECLUI_NewEvent()
{
	BTGECLUI_Event *tmp;

	if(btgeclui_context->ev_free)
	{
		tmp=btgeclui_context->ev_free;
		btgeclui_context->ev_free=tmp->next;

		memset(tmp, 0, sizeof(BTGECLUI_Event));
	}else
	{
		tmp=gcalloc(sizeof(BTGECLUI_Event));
	}
	return(tmp);
}

BTGECLUI_API void BTGECLUI_ReleaseEvent(BTGECLUI_Event *ev)
{
	ev->next=btgeclui_context->ev_free;
	btgeclui_context->ev_free=ev;
}

// BTGECLUI_API BTGECLUI_Event *BTGECLUI_Forms_AddEvent(
//	BTGECLUI_Form *form,
//	char *widget, char *event)

BTGECLUI_API BTGECLUI_Event *BTGECLUI_Forms_AddEvent(
	BTGECLUI_Form *form, BTGECLUI_Widget *widget,
	char *event)
{
	BTGECLUI_Event *tmp, *cur;

	tmp=BTGECLUI_NewEvent();
	tmp->form=form;
	tmp->widget=widget;
	tmp->event=dystrdup(event);
	tmp->eventid=BTGECLUI_EV_MISC;

	if(!strcmp(event, "clicked"))tmp->eventid=BTGECLUI_EV_CLICKED;
	if(!strcmp(event, "unclicked"))tmp->eventid=BTGECLUI_EV_UNCLICKED;
	if(!strcmp(event, "changed"))tmp->eventid=BTGECLUI_EV_CHANGED;
	if(!strcmp(event, "select"))tmp->eventid=BTGECLUI_EV_SELECT;

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

BTGECLUI_API BTGECLUI_Form *BTGECLUI_Forms_Lookup(char *name)
{
	BTGECLUI_Form *cur;

	cur=btgeclui_context->forms_root;
	while(cur)
	{
		if(!strcmp(cur->name, name))return(cur);
		cur=cur->next;
	}
	return(NULL);
}

BTGECLUI_API BTGECLUI_Form *BTGECLUI_Forms_Unlink(char *name)
{
	BTGECLUI_Form *cur, *lst;

	cur=btgeclui_context->forms_root; lst=NULL;
	while(cur)
	{
		if(!strcmp(cur->name, name))
		{
			if(lst)lst->next=cur->next;
				else btgeclui_context->forms_root=cur->next;
			return(cur);
		}

		lst=cur;
		cur=cur->next;
	}
	return(NULL);
}

BTGECLUI_API int BTGECLUI_Forms_HandleInput(BTGECLUI_InputState *state)
{
	BTGECLUI_Form *cur, *lfrm;

	lfrm=btgeclui_context->forms_focus;
	if(state->mb)btgeclui_context->forms_focus=NULL;

	cur=btgeclui_context->forms_root;
	while(cur)
	{
		if(
			(state->mx>=cur->ox) &&
			(state->my>=(cur->oy-cur->ys)) &&
			(state->mx<=(cur->ox+cur->xs)) &&
			(state->my<=cur->oy))
		{
			btgeclui_context->forms_hasfocus=1;
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
			btgeclui_context->forms_focus=cur;

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
			btgeclui_context->forms_focus=cur;

			//dragging current form
			cur->ox+=state->dx;
			cur->oy+=state->dy;
		}
#endif

		if(btgeclui_context->forms_focus==cur)
		{
			BTGECLUI_Widget_HandleInput(cur->root, state,
				cur->ox, cur->oy-cur->ys);
		}
		cur=cur->next;
	}

	cur=btgeclui_context->forms_focus;
	if(cur!=lfrm)
	{
		if(lfrm)BTGECLUI_Forms_AddEvent(lfrm, NULL, "lostfocus");
		if(cur)BTGECLUI_Forms_AddEvent(cur, NULL, "gotfocus");
	}

	return(0);
}

BTGECLUI_API int BTGECLUI_Forms_RenderColor(int c, int a)
{
	float r, g, b;

	r=((c>>16)&0xff)/255.0;
	g=((c>>8)&0xff)/255.0;
	b=(c&0xff)/255.0;
	glColor4f(r, g, b, a/255.0);
	return(0);
}

BTGECLUI_API int BTGECLUI_Forms_RenderDarkColor(int c, int a)
{
	float r, g, b;

	r=((c>>16)&0xff)/255.0;
	g=((c>>8)&0xff)/255.0;
	b=(c&0xff)/255.0;
	r*=2.0/3.0;
	g*=2.0/3.0;
	b*=2.0/3.0;
	glColor4f(r, g, b, a/255.0);
	return(0);
}

BTGECLUI_API int BTGECLUI_Forms_RenderLightColor(int c, int a)
{
	float r, g, b;

	r=((c>>16)&0xff)/255.0;
	g=((c>>8)&0xff)/255.0;
	b=(c&0xff)/255.0;
	r*=4.0/3.0;
	g*=4.0/3.0;
	b*=4.0/3.0;
	glColor4f(r, g, b, a/255.0);
	return(0);
}

BTGECLUI_API int BTGECLUI_Forms_CalcColor(int c, int *rr, int *gr, int *br)
{
	*rr=((c>>16)&0xff);
	*gr=((c>>8)&0xff);
	*br=(c&0xff);
	return(0);
}

BTGECLUI_API int BTGECLUI_Forms_CalcDarkColor(int c, int *rr, int *gr, int *br)
{
	*rr=((c>>16)&0xff)-64;
	*gr=((c>>8)&0xff)-64;
	*br=(c&0xff)-64;
	if(*rr<0)*rr=0;
	if(*gr<0)*gr=0;
	if(*br<0)*br=0;
	return(0);
}

BTGECLUI_API int BTGECLUI_Forms_CalcLightColor(int c, int *rr, int *gr, int *br)
{
	*rr=((c>>16)&0xff);
	*gr=((c>>8)&0xff);
	*br=(c&0xff);
	if(*rr>255)*rr=255;
	if(*gr>255)*gr=255;
	if(*br>255)*br=255;
	return(0);
}

BTGECLUI_API int BTGECLUI_Forms_Render(BTGECLUI_InputState *state)
{
	BTGECLUI_Form *cur;
	int r, g, b;
	int i;

	glMatrixMode(GL_MODELVIEW_MATRIX);
	glPushMatrix();

	i=(state->zoom>0)?(1024<<state->zoom):(1024>>(-state->zoom));
	glScalef(1024.0/i, 1024.0/i, 1.0);
	glTranslatef(-state->orgx, -state->orgy, 0);

	GfxFont_SetFont("fixed", 0);

	cur=btgeclui_context->forms_root;
	while(cur)
	{
		if(!strcmp(cur->style, "window"))
		{
			if(btgeclui_context->forms_focus==cur)
			{
				Draw_FillSquareRGBA(cur->ox, cur->oy,
					cur->xs, 16,
					0, 0, 127, 255);
				r=255; g=255; b=255;
			}else
			{
				Draw_FillSquareRGBA(cur->ox, cur->oy,
					cur->xs, 16, 127, 127, 127, 255);
				r=192; g=192; b=192;
			}

			GfxFont_SetFont("fixed", PDGL_FONT_BOLD);
			if(cur->title)GfxFont_DrawString(cur->title,
				cur->ox, cur->oy+4,
				8, 8, r, g, b, 255);
			
			BTGECLUI_Forms_CalcColor(cur->wbgcolor, &r, &g, &b);
			Draw_FillSquareRGBA(cur->ox, cur->oy-cur->ys,
				cur->xs, cur->ys, r, g, b, 255);
		}
		if(!strcmp(cur->style, "box") ||
			!strcmp(cur->style, "fixedbox"))
		{
			BTGECLUI_Forms_CalcColor(cur->wbgcolor, &r, &g, &b);
			Draw_FillSquareRGBA(cur->ox, cur->oy-cur->ys,
				cur->xs, cur->ys, r, g, b, 255);
		}
		if(!strcmp(cur->style, "none") ||
			!strcmp(cur->style, "fixed"))
		{
		}

		BTGECLUI_Widget_Render(cur->root, cur->ox, cur->oy-cur->ys);
		cur=cur->next;
	}

	glPopMatrix();
	return(0);
}

BTGECLUI_API int BTGECLUI_Forms_UpdateState(BTGECLUI_InputState *state,
	int mx, int my, int mb, unsigned short *keys)
{
	unsigned short *ks;
	int rmx, rmy, dx, dy, rdx, rdy;
	int i;

	state->rlmx=state->rmx;
	state->rlmy=state->rmy;
	state->rmx=mx;
	state->rmy=my;

	dx=state->rmx-state->rlmx;
	dy=state->rmy-state->rlmy;

	rmx=(state->zoom>=0)?(mx<<state->zoom):(mx>>(-state->zoom));
	rmy=(state->zoom>=0)?(my<<state->zoom):(my>>(-state->zoom));

	rdx=(state->zoom>=0)?(dx<<state->zoom):(dx>>(-state->zoom));
	rdy=(state->zoom>=0)?(dy<<state->zoom):(dy>>(-state->zoom));

	state->lmx=state->mx;
	state->lmy=state->my;
	state->lmb=state->mb;

	state->mx=rmx+state->orgx;
	state->my=rmy+state->orgy;
	state->mb=mb;

	if((state->mb&1) && !(state->lmb&1))
	{
		state->omx=state->lmx;
		state->omy=state->lmy;
	}

	state->dx=state->mx-state->lmx;
	state->dy=state->my-state->lmy;

	state->odx=state->mx-state->omx;
	state->ody=state->mx-state->omy;

	state->keys=keys;

#if 0
	if(BTGECLUI_KeyDown(K_ALT))
	{
		i=(state->zoom>0)?(64<<state->zoom):(64>>(-state->zoom));
		if(!i)i=1;

		ks=state->keys;
		while(*ks)
		{
			if(*ks==K_LEFTARROW)
				state->orgx-=i;
			if(*ks==K_RIGHTARROW)
				state->orgx+=i;

			if(*ks==K_UPARROW)
				state->orgy+=i;
			if(*ks==K_DOWNARROW)
				state->orgy-=i;

			if(*ks==K_MWHEELUP)
			{
				state->orgx-=rmx>>1;
				state->orgy-=rmy>>1;
				state->zoom--;
			}
			if(*ks==K_MWHEELDOWN)
			{
				state->orgx+=rmx>>1;
				state->orgy+=rmy>>1;
				state->zoom++;
			}

			ks++;
		}

		if(state->mb&1)
		{
			state->orgx-=rdx;
			state->orgy-=rdy;
		}
	}
#endif

	return(0);
}

BTGECLUI_API int BTGECLUI_KeyDown(int key)
{
	return(PDGL_KeyDown(key));
}

BTGECLUI_API BTGECLUI_InputState *BTGECLUI_GetState()
{
	if(!btgeclui_context->state)
		btgeclui_context->state=gcalloc(sizeof(BTGECLUI_InputState));
	return(btgeclui_context->state);
}

BTGECLUI_API int BTGECLUI_Forms_Init()
{
	static int init=0;

	if(init)return(0);
	init=1;

	BTGECLUI_Widget_Init();

	if(!btgeclui_context->state)
		btgeclui_context->state=gcalloc(sizeof(BTGECLUI_InputState));

	if(!btgeclui_context->state2)
		btgeclui_context->state2=gcalloc(sizeof(BTGECLUI_InputState));
	return(0);
}

#if 0
// BTGECLUI_API void BTGECLUI_SetKeybuf(ushort *kbuf, float dt)
{
	btgeclui_context->state->keys=kbuf;
	btgeclui_context->state->dt_f=dt;

//	btgeclui_context->state->lmx=btgeclui_context->state->mx;
//	btgeclui_context->state->lmy=btgeclui_context->state->my;
//	btgeclui_context->state->lmb=btgeclui_context->state->mb;

	GfxDrv_GetWindowSize(&btgeclui_context->state->xs, &btgeclui_context->state->ys);

//	Mouse_GetPos(&btgeclui_context->state->mx, &btgeclui_context->state->my, &btgeclui_context->state->mb);
}
#endif

BTGECLUI_API void BTGECLUI_Frame()
{
	int mx, my, mb;

	BTGECLUI_Forms_Init();

//	Mouse_GetPos(&mx, &my, &mb);

	btgeclui_context->forms_hasfocus=0;
	BTGECLUI_Forms_UpdateState(btgeclui_context->state2,
		btgeclui_context->state->mx, -btgeclui_context->state->my, btgeclui_context->state->mb,
		btgeclui_context->state->keys);
	BTGECLUI_Forms_HandleInput(btgeclui_context->state2);
	BTGECLUI_Forms_Render(btgeclui_context->state2);
	BTGECLUI_Menus_Render(btgeclui_context->state2);
	BTGECLUI_Menus_Render(btgeclui_context->state2);
}

BTGECLUI_API int BTGECLUI_HasFocus()
{
	return(btgeclui_context->forms_hasfocus);
}



BTGECLUI_API void BTGECLUI_NextEvent(char *form)
{
	BTGECLUI_Form *frm;
	BTGECLUI_Event *ev;

	frm=BTGECLUI_Forms_Lookup(form);
	if(!frm)return;

	ev=frm->event;
	if(!ev)return;

	frm->event=ev->next;
	BTGECLUI_ReleaseEvent(ev);
}

BTGECLUI_API int BTGECLUI_HasEvent(char *form)
{
	BTGECLUI_Form *frm;
	frm=BTGECLUI_Forms_Lookup(form);
	if(!frm)return(0);
	return(frm->event?1:0);
}

BTGECLUI_API int BTGECLUI_GetEventType(char *form)
{
	BTGECLUI_Form *frm;
	BTGECLUI_Event *ev;

	frm=BTGECLUI_Forms_Lookup(form);
	if(!frm)return(0);

	ev=frm->event;
	if(!ev)return(0);
	return(ev->eventid);
}

BTGECLUI_API char *BTGECLUI_GetEventName(char *form)
{
	BTGECLUI_Form *frm;
	BTGECLUI_Event *ev;

	frm=BTGECLUI_Forms_Lookup(form);
	if(!frm)return(NULL);

	ev=frm->event;
	if(!ev)return(NULL);
	return(ev->event);
}

BTGECLUI_API char *BTGECLUI_GetEventWidget(char *form)
{
	BTGECLUI_Form *frm;
	BTGECLUI_Event *ev;

	frm=BTGECLUI_Forms_Lookup(form);
	if(!frm)return(NULL);

	ev=frm->event;
	if(!ev)return(NULL);
	return(ev->widget);
}

BTGECLUI_API int BTGECLUI_GetEventIVal(char *form)
{
	BTGECLUI_Form *frm;
	BTGECLUI_Event *ev;

	frm=BTGECLUI_Forms_Lookup(form);
	if(!frm)return(0);

	ev=frm->event;
	if(!ev)return(0);
	return(ev->ival);
}

BTGECLUI_API char *BTGECLUI_GetEventSVal(char *form)
{
	BTGECLUI_Form *frm;
	BTGECLUI_Event *ev;

	frm=BTGECLUI_Forms_Lookup(form);
	if(!frm)return(NULL);

	ev=frm->event;
	if(!ev)return(NULL);
	return(ev->sval);
}

BTGECLUI_API char *BTGECLUI_GetWidgetPropertyS(char *form, char *id, char *var)
{
	BTGECLUI_Form *frm;
	BTGECLUI_Widget *obj;

	frm=BTGECLUI_Forms_Lookup(form);
	if(!frm)return(NULL);
	obj=BTGECLUI_Widget_LookupID(frm->root, id);
	if(!obj)return(NULL);

	if(!obj->funcs)return(NULL);
	if(!obj->funcs->get_property_s)return(NULL);
	return(obj->funcs->get_property_s(obj, var));
}

BTGECLUI_API int BTGECLUI_GetWidgetPropertyI(char *form, char *id, char *var)
{
	BTGECLUI_Form *frm;
	BTGECLUI_Widget *obj;

	frm=BTGECLUI_Forms_Lookup(form);
	if(!frm)return(0);
	obj=BTGECLUI_Widget_LookupID(frm->root, id);
	if(!obj)return(0);

	if(!obj->funcs)return(0);
	if(!obj->funcs->get_property_i)return(0);
	return(obj->funcs->get_property_i(obj, var));
}

BTGECLUI_API void BTGECLUI_SetWidgetPropertyS(char *form, char *id, char *var, char *val)
{
	BTGECLUI_Form *frm;
	BTGECLUI_Widget *obj;

	frm=BTGECLUI_Forms_Lookup(form);
	if(!frm)return;
	obj=BTGECLUI_Widget_LookupID(frm->root, id);
	if(!obj)return;

	if(!obj->funcs)return;
	if(!obj->funcs->set_property_s)return;
	obj->funcs->set_property_s(obj, var, val);
}

BTGECLUI_API void BTGECLUI_SetWidgetPropertyI(char *form, char *id, char *var, int val)
{
	BTGECLUI_Form *frm;
	BTGECLUI_Widget *obj;

	frm=BTGECLUI_Forms_Lookup(form);
	if(!frm)return;
	obj=BTGECLUI_Widget_LookupID(frm->root, id);
	if(!obj)return;

	if(!obj->funcs)return;
	if(!obj->funcs->set_property_i)return;
	obj->funcs->set_property_i(obj, var, val);
}

BTGECLUI_API void BTGECLUI_SetWidgetProperty2I(char *form, char *id, char *var, int x, int y)
{
	BTGECLUI_Form *frm;
	BTGECLUI_Widget *obj;

	frm=BTGECLUI_Forms_Lookup(form);
	if(!frm)return;
	obj=BTGECLUI_Widget_LookupID(frm->root, id);
	if(!obj)return;

	if(!obj->funcs)return;
	if(!obj->funcs->set_property_2i)return;
	obj->funcs->set_property_2i(obj, var, x, y);
}

BTGECLUI_API void BTGECLUI_AddItem(char *form, char *id, char *var, char *val)
{
	BTGECLUI_Form *frm;
	BTGECLUI_Widget *obj;

	frm=BTGECLUI_Forms_Lookup(form);
	if(!frm)return;
	obj=BTGECLUI_Widget_LookupID(frm->root, id);
	if(!obj)return;

	if(!obj->funcs)return;
	if(!obj->funcs->add_item)return;
	obj->funcs->add_item(obj, var, val);
}

BTGECLUI_API void BTGECLUI_SetItem(char *form, char *id, char *var, char *val)
{
	BTGECLUI_Form *frm;
	BTGECLUI_Widget *obj;

	frm=BTGECLUI_Forms_Lookup(form);
	if(!frm)return;
	obj=BTGECLUI_Widget_LookupID(frm->root, id);
	if(!obj)return;

	if(!obj->funcs)return;
	if(!obj->funcs->set_item)return;
	obj->funcs->set_item(obj, var, val);
}

BTGECLUI_API void BTGECLUI_ClearItems(char *form, char *id)
{
	BTGECLUI_Form *frm;
	BTGECLUI_Widget *obj;

	frm=BTGECLUI_Forms_Lookup(form);
	if(!frm)return;
	obj=BTGECLUI_Widget_LookupID(frm->root, id);
	if(!obj)return;

	if(!obj->funcs)return;
	if(!obj->funcs->clear_items)return;
	obj->funcs->clear_items(obj);
}

BTGECLUI_API void BTGECLUI_Init()
{
	BTGECLUI_Forms_Init();
	BTGECLUI_Widget_Init();
}
