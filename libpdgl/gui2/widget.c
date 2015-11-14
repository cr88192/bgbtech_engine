#include <pdgl.h>
#include <pdglui.h>

PDGLUI_WidgetType *pdglui_widget_types=NULL;
PDGLUI_Event *pdglui_widget_lastevent;

PDGLUI_WidgetType *PDGLUI_Widget_RegisterType(
	PDGLUI_WidgetType *tmp)
{
	tmp->next=pdglui_widget_types;
	pdglui_widget_types=tmp;

	return(tmp);
}

PDGLUI_WidgetType *PDGLUI_Widget_LookupType(char *name)
{
	PDGLUI_WidgetType *cur;
	char *s, *t;
	int i;
	
	cur=pdglui_widget_types;
	while(cur)
	{
		s=cur->name;
		while(*s)
		{
			t=name;
			while(*t && (*s==*t)) { s++; t++; }
			if(!*t && (!*s || (*s==';')))
				return(cur);
			while(*s && (*s!=';'))s++;
			if(*s==';')s++;
			while(*s && (*s<=' '))s++;
		}

		cur=cur->next;
	}
	return(NULL);
}

PDGLUI_Event *PDGLUI_Widget_AddEvent(PDGLUI_Widget *obj, char *event)
{
	PDGLUI_Event *tmp;
	char *s;

	s=obj->id;
	if(obj->name)s=obj->name;

	tmp=PDGLUI_Forms_AddEvent(obj->form, s, event);
	pdglui_widget_lastevent=tmp;
	return(tmp);
}

void PDGLUI_Widget_SetEventIVal(int val)
{
	pdglui_widget_lastevent->ival=val;
}

void PDGLUI_Widget_SetEventSVal(char *val)
{
	pdglui_widget_lastevent->sval=dystrdup(val);
}

int PDGLUI_Widget_BindForm(PDGLUI_Widget *obj, PDGLUI_Form *form)
{
	PDGLUI_Widget *cur;

	obj->form=form;

	cur=obj->first;
	while(cur)
	{
		PDGLUI_Widget_BindForm(cur, form);
		cur=cur->next;
	}
	return(0);
}

int PDGLUI_Widget_HandleInput(PDGLUI_Widget *obj,
	PDGLUI_InputState *state, int ox, int oy)
{
	int i;

	if(obj->funcs && obj->funcs->handle_input)
	{
		i=obj->funcs->handle_input(obj, state, ox, oy);
		return(i);
	}

	return(-1);
}

int PDGLUI_Widget_Render(PDGLUI_Widget *obj, int ox, int oy)
{
	int i;

	if(obj->funcs && obj->funcs->render)
	{
		i=obj->funcs->render(obj, ox, oy);
		return(i);
	}

	return(-1);
}

int PDGLUI_Widget_SizeWidget(PDGLUI_Widget *obj)
{
	int i;

	if(obj->funcs && obj->funcs->size)
	{
		i=obj->funcs->size(obj);
		return(i);
	}

	return(-1);
}

int PDGLUI_Widget_CheckFocus(int ox, int oy,
	PDGLUI_InputState *state, PDGLUI_Widget *obj)
{
	if(	(state->mx>=(obj->ox+ox)) &&
		(state->my>=(obj->oy+oy)) &&
		(state->mx<=(obj->ox+ox+obj->xs)) &&
		(state->my<=(obj->oy+oy+obj->ys)))
			return(1);

	if(	(state->lmx>=(obj->ox+ox)) &&
		(state->lmy>=(obj->oy+oy)) &&
		(state->lmx<=(obj->ox+ox+obj->xs)) &&
		(state->lmy<=(obj->oy+oy+obj->ys)))
			return(1);
	if(obj->state&3)return(1);
	return(0);
}

int PDGLUI_Widget_CheckClick(int ox, int oy,
	PDGLUI_InputState *state, PDGLUI_Widget *obj)
{
	if(	(state->mx>=(obj->ox+ox)) &&
		(state->my>=(obj->oy+oy)) &&
		(state->mx<=(obj->ox+ox+obj->xs)) &&
		(state->my<=(obj->oy+oy+obj->ys)) &&
		(state->mb&1))
			return(1);
	return(0);
}

int PDGLUI_Widget_CheckClicked(int ox, int oy,
	PDGLUI_InputState *state, PDGLUI_Widget *obj)
{
	if(	(state->mx>=(obj->ox+ox)) &&
		(state->my>=(obj->oy+oy)) &&
		(state->mx<=(obj->ox+ox+obj->xs)) &&
		(state->my<=(obj->oy+oy+obj->ys)) &&
		(state->mb&1) && !(state->lmb&1))
			return(1);
	return(0);
}

int PDGLUI_Widget_CheckClickedBox(int ox, int oy, int xs, int ys,
	PDGLUI_InputState *state)
{
	if(	(state->mx>=ox) && (state->my>=oy) &&
		(state->mx<=(ox+xs)) && (state->my<=(oy+ys)) &&
		(state->mb&1) && !(state->lmb&1))
			return(1);
	return(0);
}

int PDGLUI_Widget_CheckDragBox(int ox, int oy, int xs, int ys,
	PDGLUI_InputState *state)
{
	if(	(state->lmx>=ox) && (state->lmy>=oy) &&
		(state->lmx<=(ox+xs)) && (state->lmy<=(oy+ys)) &&
		(state->mb&1) && (state->lmb&1))
			return(1);
	return(0);
}


int PDGLUI_Widget_CheckMClick(int ox, int oy,
	PDGLUI_InputState *state, PDGLUI_Widget *obj)
{
	if(	(state->mx>=(obj->ox+ox)) &&
		(state->my>=(obj->oy+oy)) &&
		(state->mx<=(obj->ox+ox+obj->xs)) &&
		(state->my<=(obj->oy+oy+obj->ys)) &&
		(state->mb&2))
			return(1);
	return(0);
}

int PDGLUI_Widget_CheckMClicked(int ox, int oy,
	PDGLUI_InputState *state, PDGLUI_Widget *obj)
{
	if(	(state->mx>=(obj->ox+ox)) &&
		(state->my>=(obj->oy+oy)) &&
		(state->mx<=(obj->ox+ox+obj->xs)) &&
		(state->my<=(obj->oy+oy+obj->ys)) &&
		(state->mb&2) && !(state->lmb&2))
			return(1);
	return(0);
}

int PDGLUI_Widget_CheckMClickedBox(int ox, int oy, int xs, int ys,
	PDGLUI_InputState *state)
{
	if(	(state->mx>=ox) && (state->my>=oy) &&
		(state->mx<=(ox+xs)) && (state->my<=(oy+ys)) &&
		(state->mb&2) && !(state->lmb&2))
			return(1);
	return(0);
}

int PDGLUI_Widget_CheckMDragBox(int ox, int oy, int xs, int ys,
	PDGLUI_InputState *state)
{
	if(	(state->lmx>=ox) && (state->lmy>=oy) &&
		(state->lmx<=(ox+xs)) && (state->lmy<=(oy+ys)) &&
		(state->mb&2) && (state->lmb&2))
			return(1);
	return(0);
}


int PDGLUI_Widget_CheckRClick(int ox, int oy,
	PDGLUI_InputState *state, PDGLUI_Widget *obj)
{
	if(	(state->mx>=(obj->ox+ox)) &&
		(state->my>=(obj->oy+oy)) &&
		(state->mx<=(obj->ox+ox+obj->xs)) &&
		(state->my<=(obj->oy+oy+obj->ys)) &&
		(state->mb&4))
			return(1);
	return(0);
}

int PDGLUI_Widget_CheckRClicked(int ox, int oy,
	PDGLUI_InputState *state, PDGLUI_Widget *obj)
{
	if(	(state->mx>=(obj->ox+ox)) &&
		(state->my>=(obj->oy+oy)) &&
		(state->mx<=(obj->ox+ox+obj->xs)) &&
		(state->my<=(obj->oy+oy+obj->ys)) &&
		(state->mb&4) && !(state->lmb&4))
			return(1);
	return(0);
}

int PDGLUI_Widget_CheckRClickedBox(int ox, int oy, int xs, int ys,
	PDGLUI_InputState *state)
{
	if(	(state->mx>=ox) && (state->my>=oy) &&
		(state->mx<=(ox+xs)) && (state->my<=(oy+ys)) &&
		(state->mb&4) && !(state->lmb&4))
			return(1);
	return(0);
}

int PDGLUI_Widget_CheckRDragBox(int ox, int oy, int xs, int ys,
	PDGLUI_InputState *state)
{
	if(	(state->lmx>=ox) && (state->lmy>=oy) &&
		(state->lmx<=(ox+xs)) && (state->lmy<=(oy+ys)) &&
		(state->mb&4) && (state->lmb&4))
			return(1);
	return(0);
}


int PDGLUI_Widget_CheckHKey(PDGLUI_InputState *state, int key)
{
	ushort *ks;
	int ki;

	if((key>='A') && (key<='Z'))key=key-'A'+'a';
	if(!PDGLUI_KeyDown(K_ALT))return(0);

	ks=state->keys;
	while(*ks)
	{
		ki=*ks++;
		if((ki>='A') && (ki<='Z'))ki=ki-'A'+'a';
		if(ki==key)return(1);
	}

	return(0);
}

int PDGLUI_Widget_CheckHKeyString(PDGLUI_InputState *state, char *str)
{
	char *s;
	int i;

	if(!PDGLUI_KeyDown(K_ALT))return(0);

	s=str;
	while(*s)
	{
		if((s[0]=='&') && (s[1]!='&'))
		{
			i=PDGLUI_Widget_CheckHKey(state, s[1]);
			if(i)return(i);
			s+=2;
			continue;
		}
		s++;
	}
	return(0);
}

PDGLUI_Widget *PDGLUI_Widget_FindName(PDGLUI_Widget *parent, char *name)
{
	PDGLUI_Widget *first, *last, *cur, *tmp;

	first=NULL;
	last=NULL;

	cur=parent->first;
	while(cur)
	{
		if(cur->name)
			if(!strcmp(cur->name, name))
		{
			if(last)
			{
				last->chain=cur;
				last=cur;
			}else
			{
				first=cur;
				last=cur;
			}
		}
		tmp=PDGLUI_Widget_FindName(cur, name);
		if(tmp)
		{
			if(last)
			{
				last->chain=tmp;
				last=tmp;
			}else
			{
				first=tmp;
				last=tmp;
			}
			while(last->chain)last=last->chain;
		}
		cur=cur->next;
	}
	return(first);
}

PDGLUI_Widget *PDGLUI_Widget_LookupID(PDGLUI_Widget *parent, char *name)
{
	PDGLUI_Widget *cur, *tmp;

	if(parent->id)if(!strcmp(parent->id, name))
		return(parent);

	cur=parent->first;
	while(cur)
	{
		if(cur->id)if(!strcmp(cur->id, name))
			return(cur);
		tmp=PDGLUI_Widget_LookupID(cur, name);
		if(tmp)return(tmp);
		cur=cur->next;
	}
	return(NULL);
}


int PDGLUI_Widget_Render3DBorder(int ox, int oy, int xs, int ys,
	int wbgcolor, int ind)
{
	int i;

	pdglDisableTexture2D();

	pdglBegin(PDGL_LINES);

	if(ind)PDGLUI_Forms_RenderDarkColor(wbgcolor, 255);
		else PDGLUI_Forms_RenderLightColor(wbgcolor, 255);
	for(i=0; i<2; i++)
	{
		pdglVertex2f(ox+i,	oy+i);
		pdglVertex2f(ox+i,	oy+ys-i);
		pdglVertex2f(ox+i,	oy+ys-i);
		pdglVertex2f(ox+xs-i,	oy+ys-i);
	}

	if(ind)PDGLUI_Forms_RenderLightColor(wbgcolor, 255);
		else PDGLUI_Forms_RenderDarkColor(wbgcolor, 255);
	for(i=0; i<2; i++)
	{
		pdglVertex2f(ox+i,	oy+i);
		pdglVertex2f(ox+xs-i,	oy+i);
		pdglVertex2f(ox+xs-i,	oy+i);
		pdglVertex2f(ox+xs-i,	oy+ys-i);
	}
	pdglEnd();

	return(0);
}

int PDGLUI_Widget_Render3DBox(int ox, int oy, int xs, int ys,
	int wbgcolor, int ind)
{
//	glDisable(GL_TEXTURE_2D);
	pdglDisableTexture2D();
	PDGLUI_Forms_RenderColor(wbgcolor, 255);

	pdglBegin(PDGL_QUADS);
	pdglVertex2f(ox, oy);
	pdglVertex2f(ox+xs, oy);
	pdglVertex2f(ox+xs, oy+ys);
	pdglVertex2f(ox, oy+ys);
	pdglEnd();

	PDGLUI_Widget_Render3DBorder(ox, oy, xs, ys, wbgcolor, ind);
	return(0);
}

int PDGLUI_Widget_RenderLight3DBorder(int ox, int oy, int xs, int ys,
	int wbgcolor, int ind)
{
	int i;

//	glDisable(GL_TEXTURE_2D);
	pdglDisableTexture2D();

	pdglBegin(PDGL_LINES);

	if(ind)PDGLUI_Forms_RenderDarkColor(wbgcolor, 255);
		else PDGLUI_Forms_RenderLightColor(wbgcolor, 255);

	pdglVertex2f(ox,		oy);
	pdglVertex2f(ox,		oy+ys);
	pdglVertex2f(ox,		oy+ys);
	pdglVertex2f(ox+xs,	oy+ys);

	if(ind)PDGLUI_Forms_RenderLightColor(wbgcolor, 255);
		else PDGLUI_Forms_RenderDarkColor(wbgcolor, 255);

	pdglVertex2f(ox,		oy);
	pdglVertex2f(ox+xs,	oy);
	pdglVertex2f(ox+xs,	oy);
	pdglVertex2f(ox+xs,	oy+ys);

	pdglEnd();

	return(0);
}

int PDGLUI_Widget_RenderContent(int ox, int oy, int xs, int ys,
	dyxNode *body, int fgc)
{
	dyxNode *cur;
	int xp, yp, lh;
	int r, g, b;

	xp=ox;
	yp=oy+ys;
	lh=12;
	cur=body;
	while(cur)
	{
		if(!dyxTextP(cur))
		{
			GfxFont_SetFont("fixed", 0);
			PDGLUI_Forms_CalcColor(fgc, &r, &g, &b);
			GfxFont_DrawString(dyxText(cur), xp, yp-12,
				8, 12, r, g, b, 255);

			xp+=8*strlen(dyxText(cur));
			cur=dyxNext(cur);
			continue;
		}

		if(!dyxTagIsP(cur, "br"))
		{
			yp-=lh;
			xp=ox;
			lh=12;
		}
		cur=dyxNext(cur);
	}
	return(0);
}

int PDGLUI_Widget_RSizeWidget(PDGLUI_Widget *obj)
{
	PDGLUI_Widget *cur;

	cur=obj;
	while(cur)
	{
		PDGLUI_Widget_SizeWidget(cur);
		cur=cur->up;
	}
	return(0);
}

int PDGLUI_Widget_SizeContent(dyxNode *body, int *xs, int *ys)
{
	dyxNode *cur;
	int xp, yp, lh;

	xp=0;
	yp=0;
	lh=12;
	cur=body;
	while(cur)
	{
		if(!dyxTextP(cur))
		{
			xp+=8*strlen(dyxText(cur));
			if(xp>*xs)*xs=xp;
			if(yp>*ys)*ys=yp;
			cur=dyxNext(cur);
			continue;
		}

		if(!dyxTagIsP(cur, "br"))
		{
			yp+=lh;
			xp=0;
			lh=12;
		}

		if(xp>*xs)*xs=xp;
		if(yp>*ys)*ys=yp;
		cur=dyxNext(cur);
	}
	return(0);
}

int PDGLUI_Widget_SizeText(char *str, int *xs, int *ys)
{
	char *s;
	int xp, yp;

	xp=0;
	yp=0;
	s=str;
	while(*s)
	{
		if((*s>=' ') && (*s<='~'))
		{
			xp+=8;
			if(xp>*xs)*xs=xp;
			if(yp>*ys)*ys=yp;
			s++;
			continue;
		}

		if(*s=='\n')
		{
			yp+=8;
			xp=0;

			if(xp>*xs)*xs=xp;
			if(yp>*ys)*ys=yp;
			s++;
			continue;
		}

		if(*s=='\t')
		{
			xp=(xp+64)&(~63);

			if(xp>*xs)*xs=xp;
			if(yp>*ys)*ys=yp;
			s++;
			continue;
		}

		s++;
	}
	return(0);
}

int PDGLUI_Widget_Init()
{
	PDGLUI_WidgetInput_Init();
	PDGLUI_WidgetBox_Init();
	PDGLUI_WidgetUnion_Init();
	PDGLUI_WidgetLabel_Init();
	PDGLUI_WidgetScroll_Init();
	PDGLUI_WidgetList_Init();
	PDGLUI_WidgetTree_Init();

	PDGLUI_TextArea_Init();
}
