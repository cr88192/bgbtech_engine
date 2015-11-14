#include <btgecl.h>

BTCLUI_Event *BTCLUI_Widget_AddEvent(BTCLUI_Widget *obj, char *event)
{
	BTCLUI_Event *tmp;
//	char *s;

//	s=obj->id;
//	if(obj->name)s=obj->name;

	tmp=BTCLUI_Forms_AddEvent(obj->form, obj, event);
//	btgeclui_context->widget_lastevent=tmp;
	return(tmp);
}

//void BTCLUI_Widget_SetEventIVal(int val)
//{
//	btgeclui_context->widget_lastevent->ival=val;
//}

//void BTCLUI_Widget_SetEventSVal(char *val)
//{
//	btgeclui_context->widget_lastevent->sval=dystrdup(val);
//}

int BTCLUI_Widget_BindForm(BTCLUI_Widget *obj, BTCLUI_Form *form)
{
	BTCLUI_Widget *cur;

	obj->form=form;

	cur=obj->wchild;
	while(cur)
	{
		BTCLUI_Widget_BindForm(cur, form);
		cur=cur->wnext;
	}
	return(0);
}

int BTCLUI_Widget_HandleInput(BTCLUI_Widget *obj,
	BTCLUI_InputState *state, int ox, int oy)
{
	int i;

	if(obj->vt && obj->vt->HandleInput)
	{
		i=obj->vt->HandleInput(obj, state, ox, oy);
		return(i);
	}

	return(-1);
}

int BTCLUI_Widget_Render(BTCLUI_Widget *obj, int ox, int oy)
{
	int i;

	if(obj->vt && obj->vt->Render)
	{
		i=obj->vt->Render(obj, ox, oy);
		return(i);
	}

	return(-1);
}

int BTCLUI_Widget_SizeWidget(BTCLUI_Widget *obj)
{
	int i;

	if(obj->vt && obj->vt->Size)
	{
		i=obj->vt->Size(obj);
		return(i);
	}

	return(-1);
}

int BTCLUI_Widget_CheckFocus(int ox, int oy,
	BTCLUI_InputState *state, BTCLUI_Widget *obj)
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
	if(obj->flags&3)return(1);
	return(0);
}

int BTCLUI_Widget_CheckClick(int ox, int oy,
	BTCLUI_InputState *state, BTCLUI_Widget *obj)
{
	if(	(state->mx>=(obj->ox+ox)) &&
		(state->my>=(obj->oy+oy)) &&
		(state->mx<=(obj->ox+ox+obj->xs)) &&
		(state->my<=(obj->oy+oy+obj->ys)) &&
		(state->mb&1))
			return(1);
	return(0);
}

int BTCLUI_Widget_CheckClicked(int ox, int oy,
	BTCLUI_InputState *state, BTCLUI_Widget *obj)
{
	if(	(state->mx>=(obj->ox+ox)) &&
		(state->my>=(obj->oy+oy)) &&
		(state->mx<=(obj->ox+ox+obj->xs)) &&
		(state->my<=(obj->oy+oy+obj->ys)) &&
		(state->mb&1) && !(state->lmb&1))
			return(1);
	return(0);
}

int BTCLUI_Widget_CheckClickedBox(int ox, int oy, int xs, int ys,
	BTCLUI_InputState *state)
{
	if(	(state->mx>=ox) && (state->my>=oy) &&
		(state->mx<=(ox+xs)) && (state->my<=(oy+ys)) &&
		(state->mb&1) && !(state->lmb&1))
			return(1);
	return(0);
}

int BTCLUI_Widget_CheckDragBox(int ox, int oy, int xs, int ys,
	BTCLUI_InputState *state)
{
	if(	(state->lmx>=ox) && (state->lmy>=oy) &&
		(state->lmx<=(ox+xs)) && (state->lmy<=(oy+ys)) &&
		(state->mb&1) && (state->lmb&1))
			return(1);
	return(0);
}


int BTCLUI_Widget_CheckMClick(int ox, int oy,
	BTCLUI_InputState *state, BTCLUI_Widget *obj)
{
	if(	(state->mx>=(obj->ox+ox)) &&
		(state->my>=(obj->oy+oy)) &&
		(state->mx<=(obj->ox+ox+obj->xs)) &&
		(state->my<=(obj->oy+oy+obj->ys)) &&
		(state->mb&2))
			return(1);
	return(0);
}

int BTCLUI_Widget_CheckMClicked(int ox, int oy,
	BTCLUI_InputState *state, BTCLUI_Widget *obj)
{
	if(	(state->mx>=(obj->ox+ox)) &&
		(state->my>=(obj->oy+oy)) &&
		(state->mx<=(obj->ox+ox+obj->xs)) &&
		(state->my<=(obj->oy+oy+obj->ys)) &&
		(state->mb&2) && !(state->lmb&2))
			return(1);
	return(0);
}

int BTCLUI_Widget_CheckMClickedBox(int ox, int oy, int xs, int ys,
	BTCLUI_InputState *state)
{
	if(	(state->mx>=ox) && (state->my>=oy) &&
		(state->mx<=(ox+xs)) && (state->my<=(oy+ys)) &&
		(state->mb&2) && !(state->lmb&2))
			return(1);
	return(0);
}

int BTCLUI_Widget_CheckMDragBox(int ox, int oy, int xs, int ys,
	BTCLUI_InputState *state)
{
	if(	(state->lmx>=ox) && (state->lmy>=oy) &&
		(state->lmx<=(ox+xs)) && (state->lmy<=(oy+ys)) &&
		(state->mb&2) && (state->lmb&2))
			return(1);
	return(0);
}


int BTCLUI_Widget_CheckRClick(int ox, int oy,
	BTCLUI_InputState *state, BTCLUI_Widget *obj)
{
	if(	(state->mx>=(obj->ox+ox)) &&
		(state->my>=(obj->oy+oy)) &&
		(state->mx<=(obj->ox+ox+obj->xs)) &&
		(state->my<=(obj->oy+oy+obj->ys)) &&
		(state->mb&4))
			return(1);
	return(0);
}

int BTCLUI_Widget_CheckRClicked(int ox, int oy,
	BTCLUI_InputState *state, BTCLUI_Widget *obj)
{
	if(	(state->mx>=(obj->ox+ox)) &&
		(state->my>=(obj->oy+oy)) &&
		(state->mx<=(obj->ox+ox+obj->xs)) &&
		(state->my<=(obj->oy+oy+obj->ys)) &&
		(state->mb&4) && !(state->lmb&4))
			return(1);
	return(0);
}

int BTCLUI_Widget_CheckRClickedBox(int ox, int oy, int xs, int ys,
	BTCLUI_InputState *state)
{
	if(	(state->mx>=ox) && (state->my>=oy) &&
		(state->mx<=(ox+xs)) && (state->my<=(oy+ys)) &&
		(state->mb&4) && !(state->lmb&4))
			return(1);
	return(0);
}

int BTCLUI_Widget_CheckRDragBox(int ox, int oy, int xs, int ys,
	BTCLUI_InputState *state)
{
	if(	(state->lmx>=ox) && (state->lmy>=oy) &&
		(state->lmx<=(ox+xs)) && (state->lmy<=(oy+ys)) &&
		(state->mb&4) && (state->lmb&4))
			return(1);
	return(0);
}


int BTCLUI_Widget_CheckHKey(BTCLUI_InputState *state, int key)
{
	ushort *ks;
	int ki;

	if((key>='A') && (key<='Z'))key=key-'A'+'a';
	if(!BTCLUI_KeyDown(state, K_ALT))return(0);

	ks=state->keys;
	while(*ks)
	{
		ki=*ks++;
		if((ki>='A') && (ki<='Z'))ki=ki-'A'+'a';
		if(ki==key)return(1);
	}

	return(0);
}

int BTCLUI_Widget_CheckHKeyString(BTCLUI_InputState *state, char *str)
{
	char *s;
	int i;

	if(!BTCLUI_KeyDown(state, K_ALT))return(0);

	s=str;
	while(*s)
	{
		if((s[0]=='&') && (s[1]!='&'))
		{
			i=BTCLUI_Widget_CheckHKey(state, s[1]);
			if(i)return(i);
			s+=2;
			continue;
		}
		s++;
	}
	return(0);
}

BTCLUI_Widget *BTCLUI_Widget_FindName(BTCLUI_Widget *parent, char *name)
{
	BTCLUI_Widget *first, *last, *cur, *tmp;

	first=NULL;
	last=NULL;

	cur=parent->wchild;
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
		tmp=BTCLUI_Widget_FindName(cur, name);
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
		cur=cur->wnext;
	}
	return(first);
}

BTCLUI_Widget *BTCLUI_Widget_LookupID(BTCLUI_Widget *parent, char *name)
{
	BTCLUI_Widget *cur, *tmp;

	if(parent->id)if(!strcmp(parent->id, name))
		return(parent);

	cur=parent->wchild;
	while(cur)
	{
		if(cur->id)if(!strcmp(cur->id, name))
			return(cur);
		tmp=BTCLUI_Widget_LookupID(cur, name);
		if(tmp)return(tmp);
		cur=cur->wnext;
	}
	return(NULL);
}


BTCL_API int BTCLUI_Forms_RenderColor(int c, int a)
{
	float r, g, b;

	r=((c>>16)&0xff)/255.0;
	g=((c>>8)&0xff)/255.0;
	b=(c&0xff)/255.0;
	pdglColor4f(r, g, b, a/255.0);
	return(0);
}

BTCL_API int BTCLUI_Forms_RenderDarkColor(int c, int a)
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

BTCL_API int BTCLUI_Forms_RenderLightColor(int c, int a)
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

BTCL_API int BTCLUI_Forms_CalcColor(int c, int *rr, int *gr, int *br)
{
	*rr=((c>>16)&0xff);
	*gr=((c>>8)&0xff);
	*br=(c&0xff);
	return(0);
}

BTCL_API int BTCLUI_Forms_CalcDarkColor(int c, int *rr, int *gr, int *br)
{
	*rr=((c>>16)&0xff)-64;
	*gr=((c>>8)&0xff)-64;
	*br=(c&0xff)-64;
	if(*rr<0)*rr=0;
	if(*gr<0)*gr=0;
	if(*br<0)*br=0;
	return(0);
}

BTCL_API int BTCLUI_Forms_CalcLightColor(int c, int *rr, int *gr, int *br)
{
	*rr=((c>>16)&0xff);
	*gr=((c>>8)&0xff);
	*br=(c&0xff);
	if(*rr>255)*rr=255;
	if(*gr>255)*gr=255;
	if(*br>255)*br=255;
	return(0);
}

int BTCLUI_Widget_Render3DBorder(int ox, int oy, int xs, int ys,
	int wbgcolor, int ind)
{
	int i;

	glDisable(GL_TEXTURE_2D);

	pdglBegin(PDGL_LINES);

	if(ind)BTCLUI_Forms_RenderDarkColor(wbgcolor, 255);
		else BTCLUI_Forms_RenderLightColor(wbgcolor, 255);
	for(i=0; i<2; i++)
	{
		pdglVertex2f(ox+i,	oy+i);
		pdglVertex2f(ox+i,	oy+ys-i);
		pdglVertex2f(ox+i,	oy+ys-i);
		pdglVertex2f(ox+xs-i,	oy+ys-i);
	}

	if(ind)BTCLUI_Forms_RenderLightColor(wbgcolor, 255);
		else BTCLUI_Forms_RenderDarkColor(wbgcolor, 255);
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

int BTCLUI_Widget_Render3DBox(int ox, int oy, int xs, int ys,
	int wbgcolor, int ind)
{
	glDisable(GL_TEXTURE_2D);
	BTCLUI_Forms_RenderColor(wbgcolor, 255);

	pdglBegin(PDGL_QUADS);
	pdglVertex2f(ox, oy);
	pdglVertex2f(ox+xs, oy);
	pdglVertex2f(ox+xs, oy+ys);
	pdglVertex2f(ox, oy+ys);
	pdglEnd();

	BTCLUI_Widget_Render3DBorder(ox, oy, xs, ys, wbgcolor, ind);
	return(0);
}

int BTCLUI_Widget_RenderLight3DBorder(int ox, int oy, int xs, int ys,
	int wbgcolor, int ind)
{
	int i;

	glDisable(GL_TEXTURE_2D);

	pdglBegin(PDGL_LINES);

	if(ind)BTCLUI_Forms_RenderDarkColor(wbgcolor, 255);
		else BTCLUI_Forms_RenderLightColor(wbgcolor, 255);

	pdglVertex2f(ox,		oy);
	pdglVertex2f(ox,		oy+ys);
	pdglVertex2f(ox,		oy+ys);
	pdglVertex2f(ox+xs,	oy+ys);

	if(ind)BTCLUI_Forms_RenderLightColor(wbgcolor, 255);
		else BTCLUI_Forms_RenderDarkColor(wbgcolor, 255);

	pdglVertex2f(ox,		oy);
	pdglVertex2f(ox+xs,	oy);
	pdglVertex2f(ox+xs,	oy);
	pdglVertex2f(ox+xs,	oy+ys);

	pdglEnd();

	return(0);
}

int BTCLUI_Widget_RenderContent(int ox, int oy, int xs, int ys,
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
			BTCLUI_Forms_CalcColor(fgc, &r, &g, &b);
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

int BTCLUI_Widget_RSizeWidget(BTCLUI_Widget *obj)
{
	BTCLUI_Widget *cur;

	cur=obj;
	while(cur)
	{
		BTCLUI_Widget_SizeWidget(cur);
		cur=cur->wparent;
	}
	return(0);
}

int BTCLUI_Widget_SizeContent(dyxNode *body, int *xs, int *ys)
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

int BTCLUI_Widget_SizeText(char *str, int *xs, int *ys)
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

int BTCLUI_Widget_Init()
{
	BTCLUI_WidgetInput_Init();
	BTCLUI_WidgetBox_Init();
	BTCLUI_WidgetUnion_Init();
	BTCLUI_WidgetLabel_Init();
	BTCLUI_WidgetScroll_Init();
	BTCLUI_WidgetList_Init();
	BTCLUI_WidgetTree_Init();

	BTCLUI_TextArea_Init();

	return(0);
}
