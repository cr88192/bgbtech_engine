#include <pdgl.h>
#include <pdglui.h>

//ushort *pdglui_keybuf;
//int pdglui_mx, pdglui_my, pdglui_mb;
//int pdglui_lmx, pdglui_lmy, pdglui_lmb;
//int pdglui_xs, pdglui_ys;
//float pdglui_dt_f;

PDGLUI_Event *pdglui_ev_free;

PDGLUI_Form *pdglui_forms_root=NULL;
PDGLUI_Form *pdglui_forms_focus=NULL;
int pdglui_forms_index=1;

PDGLUI_InputState *pdglui_state=NULL;
PDGLUI_InputState *pdglui_state2=NULL;

extern int pdglui_fgcolor;	//text fg color
extern int pdglui_bgcolor;	//text bg color
extern int pdglui_wbgcolor;	//widget bg color

int pdglui_forms_hasfocus;

PDGLUI_API PDGLUI_Event *PDGLUI_NewEvent()
{
	PDGLUI_Event *tmp;

	if(pdglui_ev_free)
	{
		tmp=pdglui_ev_free;
		pdglui_ev_free=tmp->next;

		memset(tmp, 0, sizeof(PDGLUI_Event));
	}else
	{
		tmp=gcalloc(sizeof(PDGLUI_Event));
	}
	return(tmp);
}

PDGLUI_API void PDGLUI_ReleaseEvent(PDGLUI_Event *ev)
{
	ev->next=pdglui_ev_free;
	pdglui_ev_free=ev;
}

PDGLUI_API PDGLUI_Event *PDGLUI_Forms_AddEvent(PDGLUI_Form *form,
	char *widget, char *event)
{
	PDGLUI_Event *tmp, *cur;

	tmp=PDGLUI_NewEvent();
	tmp->widget=dystrdup(widget);
	tmp->event=dystrdup(event);
	tmp->eventid=PDGLUI_EV_MISC;

	if(!strcmp(event, "clicked"))tmp->eventid=PDGLUI_EV_CLICKED;
	if(!strcmp(event, "unclicked"))tmp->eventid=PDGLUI_EV_UNCLICKED;
	if(!strcmp(event, "changed"))tmp->eventid=PDGLUI_EV_CHANGED;
	if(!strcmp(event, "select"))tmp->eventid=PDGLUI_EV_SELECT;

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

PDGLUI_API PDGLUI_Form *PDGLUI_Forms_Lookup(char *name)
{
	PDGLUI_Form *cur;

	cur=pdglui_forms_root;
	while(cur)
	{
		if(!strcmp(cur->name, name))return(cur);
		cur=cur->next;
	}
	return(NULL);
}

PDGLUI_API PDGLUI_Form *PDGLUI_Forms_Unlink(char *name)
{
	PDGLUI_Form *cur, *lst;

	cur=pdglui_forms_root; lst=NULL;
	while(cur)
	{
		if(!strcmp(cur->name, name))
		{
			if(lst)lst->next=cur->next;
				else pdglui_forms_root=cur->next;
			return(cur);
		}

		lst=cur;
		cur=cur->next;
	}
	return(NULL);
}

PDGLUI_API int PDGLUI_Forms_HandleInput(PDGLUI_InputState *state)
{
	PDGLUI_Form *cur, *lfrm;

	lfrm=pdglui_forms_focus;
	if(state->mb)pdglui_forms_focus=NULL;

	cur=pdglui_forms_root;
	while(cur)
	{
		if(
			(state->mx>=cur->ox) &&
			(state->my>=(cur->oy-cur->ys)) &&
			(state->mx<=(cur->ox+cur->xs)) &&
			(state->my<=cur->oy))
		{
			pdglui_forms_hasfocus=1;
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
			pdglui_forms_focus=cur;

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
			pdglui_forms_focus=cur;

			//dragging current form
			cur->ox+=state->dx;
			cur->oy+=state->dy;
		}
#endif

		if(pdglui_forms_focus==cur)
		{
			PDGLUI_Widget_HandleInput(cur->root, state,
				cur->ox, cur->oy-cur->ys);
		}
		cur=cur->next;
	}

	cur=pdglui_forms_focus;
	if(cur!=lfrm)
	{
		if(lfrm)PDGLUI_Forms_AddEvent(lfrm, NULL, "lostfocus");
		if(cur)PDGLUI_Forms_AddEvent(cur, NULL, "gotfocus");
	}

	return(0);
}

PDGLUI_API int PDGLUI_Forms_RenderColor(int c, int a)
{
	float r, g, b;

	r=((c>>16)&0xff)/255.0;
	g=((c>>8)&0xff)/255.0;
	b=(c&0xff)/255.0;
	pdglColor4f(r, g, b, a/255.0);
	return(0);
}

PDGLUI_API int PDGLUI_Forms_RenderDarkColor(int c, int a)
{
	float r, g, b;

	r=((c>>16)&0xff)/255.0;
	g=((c>>8)&0xff)/255.0;
	b=(c&0xff)/255.0;
	r*=2.0/3.0;
	g*=2.0/3.0;
	b*=2.0/3.0;
	pdglColor4f(r, g, b, a/255.0);
	return(0);
}

PDGLUI_API int PDGLUI_Forms_RenderLightColor(int c, int a)
{
	float r, g, b;

	r=((c>>16)&0xff)/255.0;
	g=((c>>8)&0xff)/255.0;
	b=(c&0xff)/255.0;
	r*=4.0/3.0;
	g*=4.0/3.0;
	b*=4.0/3.0;
	pdglColor4f(r, g, b, a/255.0);
	return(0);
}

PDGLUI_API int PDGLUI_Forms_CalcColor(int c, int *rr, int *gr, int *br)
{
	*rr=((c>>16)&0xff);
	*gr=((c>>8)&0xff);
	*br=(c&0xff);
	return(0);
}

PDGLUI_API int PDGLUI_Forms_CalcDarkColor(int c, int *rr, int *gr, int *br)
{
	*rr=((c>>16)&0xff)-64;
	*gr=((c>>8)&0xff)-64;
	*br=(c&0xff)-64;
	if(*rr<0)*rr=0;
	if(*gr<0)*gr=0;
	if(*br<0)*br=0;
	return(0);
}

PDGLUI_API int PDGLUI_Forms_CalcLightColor(int c, int *rr, int *gr, int *br)
{
	*rr=((c>>16)&0xff);
	*gr=((c>>8)&0xff);
	*br=(c&0xff);
	if(*rr>255)*rr=255;
	if(*gr>255)*gr=255;
	if(*br>255)*br=255;
	return(0);
}

PDGLUI_API int PDGLUI_Forms_Render(PDGLUI_InputState *state)
{
	PDGLUI_Form *cur;
	int r, g, b;
	int i;

//	glMatrixMode(GL_MODELVIEW_MATRIX);
	pdglModelviewMatrix();
	pdglPushMatrix();

	i=(state->zoom>0)?(1024<<state->zoom):(1024>>(-state->zoom));
	pdglScalef(1024.0/i, 1024.0/i, 1.0);
	pdglTranslatef(-state->orgx, -state->orgy, 0);

	GfxFont_SetFont("fixed", 0);

	cur=pdglui_forms_root;
	while(cur)
	{
		if(!strcmp(cur->style, "window"))
		{
			if(pdglui_forms_focus==cur)
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
			
			PDGLUI_Forms_CalcColor(cur->wbgcolor, &r, &g, &b);
			Draw_FillSquareRGBA(cur->ox, cur->oy-cur->ys,
				cur->xs, cur->ys, r, g, b, 255);
		}
		if(!strcmp(cur->style, "box") ||
			!strcmp(cur->style, "fixedbox"))
		{
			PDGLUI_Forms_CalcColor(cur->wbgcolor, &r, &g, &b);
			Draw_FillSquareRGBA(cur->ox, cur->oy-cur->ys,
				cur->xs, cur->ys, r, g, b, 255);
		}
		if(!strcmp(cur->style, "none") ||
			!strcmp(cur->style, "fixed"))
		{
		}

		PDGLUI_Widget_Render(cur->root, cur->ox, cur->oy-cur->ys);
		cur=cur->next;
	}

	pdglPopMatrix();
	return(0);
}

PDGLUI_API int PDGLUI_Forms_UpdateState(PDGLUI_InputState *state,
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
	PDGL_GetKeyMap(state->keymap);

#if 0
	if(PDGLUI_KeyDown(K_ALT))
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

PDGLUI_API int PDGLUI_KeyDown(int key)
{
	return(PDGL_KeyDown(key));
}

PDGLUI_API PDGLUI_InputState *PDGLUI_GetState()
{
	if(!pdglui_state)
		pdglui_state=gctalloc("pdglui_inputstate_t",
			sizeof(PDGLUI_InputState));
	return(pdglui_state);
}

PDGLUI_API int PDGLUI_Forms_Init()
{
	static int init=0;

	if(init)return(0);
	init=1;

	PDGLUI_Widget_Init();

	if(!pdglui_state)
		pdglui_state=gctalloc("pdglui_inputstate_t",
			sizeof(PDGLUI_InputState));

	if(!pdglui_state2)
		pdglui_state2=gctalloc("pdglui_inputstate_t",
			sizeof(PDGLUI_InputState));
	return(0);
}

#if 0
// PDGLUI_API void PDGLUI_SetKeybuf(ushort *kbuf, float dt)
{
	pdglui_state->keys=kbuf;
	pdglui_state->dt_f=dt;

//	pdglui_state->lmx=pdglui_state->mx;
//	pdglui_state->lmy=pdglui_state->my;
//	pdglui_state->lmb=pdglui_state->mb;

	GfxDrv_GetWindowSize(&pdglui_state->xs, &pdglui_state->ys);

//	Mouse_GetPos(&pdglui_state->mx, &pdglui_state->my, &pdglui_state->mb);
}
#endif

PDGLUI_API void PDGLUI_Frame()
{
	int mx, my, mb;

	PDGLUI_Forms_Init();

//	Mouse_GetPos(&mx, &my, &mb);

	pdglui_forms_hasfocus=0;
	PDGLUI_Forms_UpdateState(pdglui_state2,
		pdglui_state->mx, -pdglui_state->my, pdglui_state->mb,
		pdglui_state->keys);
	PDGLUI_Forms_HandleInput(pdglui_state2);
	PDGLUI_Forms_Render(pdglui_state2);
	PDGLUI_Menus_Render(pdglui_state2);
	PDGLUI_Menus_Render(pdglui_state2);
}

PDGLUI_API int PDGLUI_HasFocus()
{
	return(pdglui_forms_hasfocus);
}



PDGLUI_API void PDGLUI_NextEvent(char *form)
{
	PDGLUI_Form *frm;
	PDGLUI_Event *ev;

	frm=PDGLUI_Forms_Lookup(form);
	if(!frm)return;

	ev=frm->event;
	if(!ev)return;

	frm->event=ev->next;
	PDGLUI_ReleaseEvent(ev);
}

PDGLUI_API int PDGLUI_HasEvent(char *form)
{
	PDGLUI_Form *frm;
	frm=PDGLUI_Forms_Lookup(form);
	if(!frm)return(0);
	return(frm->event?1:0);
}

PDGLUI_API int PDGLUI_GetEventType(char *form)
{
	PDGLUI_Form *frm;
	PDGLUI_Event *ev;

	frm=PDGLUI_Forms_Lookup(form);
	if(!frm)return(0);

	ev=frm->event;
	if(!ev)return(0);
	return(ev->eventid);
}

PDGLUI_API char *PDGLUI_GetEventName(char *form)
{
	PDGLUI_Form *frm;
	PDGLUI_Event *ev;

	frm=PDGLUI_Forms_Lookup(form);
	if(!frm)return(NULL);

	ev=frm->event;
	if(!ev)return(NULL);
	return(ev->event);
}

PDGLUI_API char *PDGLUI_GetEventWidget(char *form)
{
	PDGLUI_Form *frm;
	PDGLUI_Event *ev;

	frm=PDGLUI_Forms_Lookup(form);
	if(!frm)return(NULL);

	ev=frm->event;
	if(!ev)return(NULL);
	return(ev->widget);
}

PDGLUI_API int PDGLUI_GetEventIVal(char *form)
{
	PDGLUI_Form *frm;
	PDGLUI_Event *ev;

	frm=PDGLUI_Forms_Lookup(form);
	if(!frm)return(0);

	ev=frm->event;
	if(!ev)return(0);
	return(ev->ival);
}

PDGLUI_API char *PDGLUI_GetEventSVal(char *form)
{
	PDGLUI_Form *frm;
	PDGLUI_Event *ev;

	frm=PDGLUI_Forms_Lookup(form);
	if(!frm)return(NULL);

	ev=frm->event;
	if(!ev)return(NULL);
	return(ev->sval);
}

PDGLUI_API char *PDGLUI_GetWidgetPropertyS(char *form, char *id, char *var)
{
	PDGLUI_Form *frm;
	PDGLUI_Widget *obj;

	frm=PDGLUI_Forms_Lookup(form);
	if(!frm)return(NULL);
	obj=PDGLUI_Widget_LookupID(frm->root, id);
	if(!obj)return(NULL);

	if(!obj->funcs)return(NULL);
	if(!obj->funcs->get_property_s)return(NULL);
	return(obj->funcs->get_property_s(obj, var));
}

PDGLUI_API int PDGLUI_GetWidgetPropertyI(char *form, char *id, char *var)
{
	PDGLUI_Form *frm;
	PDGLUI_Widget *obj;

	frm=PDGLUI_Forms_Lookup(form);
	if(!frm)return(0);
	obj=PDGLUI_Widget_LookupID(frm->root, id);
	if(!obj)return(0);

	if(!obj->funcs)return(0);
	if(!obj->funcs->get_property_i)return(0);
	return(obj->funcs->get_property_i(obj, var));
}

PDGLUI_API void PDGLUI_SetWidgetPropertyS(char *form, char *id, char *var, char *val)
{
	PDGLUI_Form *frm;
	PDGLUI_Widget *obj;

	frm=PDGLUI_Forms_Lookup(form);
	if(!frm)return;
	obj=PDGLUI_Widget_LookupID(frm->root, id);
	if(!obj)return;

	if(!obj->funcs)return;
	if(!obj->funcs->set_property_s)return;
	obj->funcs->set_property_s(obj, var, val);
}

PDGLUI_API void PDGLUI_SetWidgetPropertyI(char *form, char *id, char *var, int val)
{
	PDGLUI_Form *frm;
	PDGLUI_Widget *obj;

	frm=PDGLUI_Forms_Lookup(form);
	if(!frm)return;
	obj=PDGLUI_Widget_LookupID(frm->root, id);
	if(!obj)return;

	if(!obj->funcs)return;
	if(!obj->funcs->set_property_i)return;
	obj->funcs->set_property_i(obj, var, val);
}

PDGLUI_API void PDGLUI_SetWidgetProperty2I(char *form, char *id, char *var, int x, int y)
{
	PDGLUI_Form *frm;
	PDGLUI_Widget *obj;

	frm=PDGLUI_Forms_Lookup(form);
	if(!frm)return;
	obj=PDGLUI_Widget_LookupID(frm->root, id);
	if(!obj)return;

	if(!obj->funcs)return;
	if(!obj->funcs->set_property_2i)return;
	obj->funcs->set_property_2i(obj, var, x, y);
}

PDGLUI_API void PDGLUI_AddItem(char *form, char *id, char *var, char *val)
{
	PDGLUI_Form *frm;
	PDGLUI_Widget *obj;

	frm=PDGLUI_Forms_Lookup(form);
	if(!frm)return;
	obj=PDGLUI_Widget_LookupID(frm->root, id);
	if(!obj)return;

	if(!obj->funcs)return;
	if(!obj->funcs->add_item)return;
	obj->funcs->add_item(obj, var, val);
}

PDGLUI_API void PDGLUI_SetItem(char *form, char *id, char *var, char *val)
{
	PDGLUI_Form *frm;
	PDGLUI_Widget *obj;

	frm=PDGLUI_Forms_Lookup(form);
	if(!frm)return;
	obj=PDGLUI_Widget_LookupID(frm->root, id);
	if(!obj)return;

	if(!obj->funcs)return;
	if(!obj->funcs->set_item)return;
	obj->funcs->set_item(obj, var, val);
}

PDGLUI_API void PDGLUI_ClearItems(char *form, char *id)
{
	PDGLUI_Form *frm;
	PDGLUI_Widget *obj;

	frm=PDGLUI_Forms_Lookup(form);
	if(!frm)return;
	obj=PDGLUI_Widget_LookupID(frm->root, id);
	if(!obj)return;

	if(!obj->funcs)return;
	if(!obj->funcs->clear_items)return;
	obj->funcs->clear_items(obj);
}

PDGLUI_API void PDGLUI_Init()
{
	PDGLUI_Forms_Init();
	PDGLUI_Widget_Init();
}
