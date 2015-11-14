#include <pdgl.h>
#include <pdglui.h>

PDGLUI_WidgetType *pdglui_widget_types=NULL;

PDGLUI_WidgetType *PDGLUI_Widget_RegisterType(char *name,
	PDGLUI_Widget *(*func)(NetParse_Node *node,
		PDGLUI_Form *form, PDGLUI_Forms_StyleState *style))
{
	PDGLUI_WidgetType *tmp;

	tmp=kalloc(sizeof(PDGLUI_WidgetType));
	tmp->name=kstrdup(name);
	tmp->build=func;

	tmp->next=pdglui_widget_types;
	pdglui_widget_types=tmp;

	return(tmp);
}

PDGLUI_WidgetType *PDGLUI_Widget_LookupType(char *name)
{
	PDGLUI_WidgetType *cur;
	
	cur=pdglui_widget_types;
	while(cur)
	{
		if(!strcmp(cur->name, name))
			return(cur);
		cur=cur->next;
	}
	return(NULL);
}

PDGLUI_Widget *PDGLUI_Widget_BuildWidget(NetParse_Node *node,
	PDGLUI_Form *form, PDGLUI_Forms_StyleState *style)
{
	PDGLUI_Widget *tmp;
	PDGLUI_WidgetType *cur;

	cur=PDGLUI_Widget_LookupType(node->key);
	if(cur)
	{
		tmp=cur->build(node, form, style);
		return(tmp);
	}

	return(NULL);
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

	glDisable(GL_TEXTURE_2D);

	glBegin(GL_LINES);

	if(ind)PDGLUI_Forms_RenderDarkColor(wbgcolor, 255);
		else PDGLUI_Forms_RenderLightColor(wbgcolor, 255);
	for(i=0; i<2; i++)
	{
		glVertex2f(ox+i,	oy+i);
		glVertex2f(ox+i,	oy+ys-i);
		glVertex2f(ox+i,	oy+ys-i);
		glVertex2f(ox+xs-i,	oy+ys-i);
	}

	if(ind)PDGLUI_Forms_RenderLightColor(wbgcolor, 255);
		else PDGLUI_Forms_RenderDarkColor(wbgcolor, 255);
	for(i=0; i<2; i++)
	{
		glVertex2f(ox+i,	oy+i);
		glVertex2f(ox+xs-i,	oy+i);
		glVertex2f(ox+xs-i,	oy+i);
		glVertex2f(ox+xs-i,	oy+ys-i);
	}
	glEnd();

	return(0);
}

int PDGLUI_Widget_Render3DBox(int ox, int oy, int xs, int ys,
	int wbgcolor, int ind)
{
	glDisable(GL_TEXTURE_2D);
	PDGLUI_Forms_RenderColor(wbgcolor, 255);

	glBegin(GL_QUADS);
	glVertex2f(ox, oy);
	glVertex2f(ox+xs, oy);
	glVertex2f(ox+xs, oy+ys);
	glVertex2f(ox, oy+ys);
	glEnd();

	PDGLUI_Widget_Render3DBorder(ox, oy, xs, ys, wbgcolor, ind);
	return(0);
}

int PDGLUI_Widget_RenderContent(int ox, int oy, int xs, int ys,
	NetParse_Node *body, int fgc)
{
	NetParse_Node *cur;
	int xp, yp, lh;
	int r, g, b;

	xp=ox;
	yp=oy+ys;
	lh=12;
	cur=body;
	while(cur)
	{
		if(!cur->key)
		{
			GfxFont_SetFont("fixed", 0);
			PDGLUI_Forms_CalcColor(fgc, &r, &g, &b);
			GfxFont_DrawString(cur->text, xp, yp-12,
				8, 12, r, g, b, 255);

			xp+=8*strlen(cur->text);
			cur=cur->next;
			continue;
		}

		if(!strcmp(cur->key, "br"))
		{
			yp-=lh;
			xp=ox;
			lh=12;
		}
		cur=cur->next;
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

int PDGLUI_Widget_SizeContent(NetParse_Node *body, int *xs, int *ys)
{
	NetParse_Node *cur;
	int xp, yp, lh;

	xp=0;
	yp=0;
	lh=12;
	cur=body;
	while(cur)
	{
		if(!cur->key)
		{
			xp+=8*strlen(cur->text);
			if(xp>*xs)*xs=xp;
			if(yp>*ys)*ys=yp;
			cur=cur->next;
			continue;
		}

		if(!strcmp(cur->key, "br"))
		{
			yp+=lh;
			xp=0;
			lh=12;
		}

		if(xp>*xs)*xs=xp;
		if(yp>*ys)*ys=yp;
		cur=cur->next;
	}
	return(0);
}

PDGLUI_Widget *PDGLUI_Widget_BuildWidgetList(NetParse_Node *first,
	PDGLUI_Widget *parent, PDGLUI_Form *form,
	PDGLUI_Forms_StyleState *style)
{
	PDGLUI_Widget *lst, *wt, *wl;
	NetParse_Node *cur;
	char *str;

	lst=NULL;
	wl=NULL;
	cur=first;
	while(cur)
	{
		wt=PDGLUI_Widget_BuildWidget(cur, form, style);
		if(wt)
		{
			wt->up=parent;
			if(wl)
			{
				wl->next=wt;
				wt->prev=wl;
				wl=wt;
			}else
			{
				lst=wt;
				wl=wt;
			}
		}
		cur=cur->next;
	}

	return(lst);
}

int PDGLUI_Widget_Init()
{
	PDGLUI_WidgetInput_Init();
	PDGLUI_WidgetBox_Init();
	PDGLUI_WidgetLabel_Init();
	PDGLUI_WidgetScroll_Init();
	PDGLUI_WidgetTree_Init();

	PDGLUI_TextArea_Init();
}
