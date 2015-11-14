#include <btgecl.h>

int BTCLUI_WidgetList_Render(BTCLUI_Widget *obj, int ox, int oy)
{
	int r, g, b;
	int i, j, k, tc;

	BTCLUI_Forms_CalcColor(obj->bgcolor, &r, &g, &b);
	Draw_FillSquareRGBA(ox+obj->ox, oy+obj->oy,
		obj->xs, obj->ys, r, g, b, 255);

	for(i=0; i<obj->rows; i++)
	{
		j=obj->min+i;
		if(j>=obj->num_items)continue;

		tc=obj->fgcolor;
		if(j==obj->curval)
		{
			if(obj->flags&BTCLUI_GUISTATE_HASFOCUS)
				{ r=0; g=0; b=255; tc=0xFFFFFF; }
				else { r=192; g=192; b=192; }

			Draw_FillSquareRGBA(
				ox+obj->ox, oy+obj->oy+obj->ys-(i+1)*8,
				obj->xs, 8, r, g, b, 255);
		}

		GfxFont_SetFont("fixed", 0);
		BTCLUI_Forms_CalcColor(tc, &r, &g, &b);
		GfxFont_DrawString(obj->item_value[j],
			ox+obj->ox, oy+obj->oy+obj->ys-(i+1)*8,
			8, 8, r, g, b, 255);
	}

	if(!(obj->flags&65536))return(0);

	GfxFont_SetFont("gfx", 0);

	Draw_FillSquareRGBA(ox+obj->ox+obj->xs-12, oy+obj->oy,
		12, obj->ys, 192, 192, 192, 255);

	i=obj->curval*(obj->ys-36)/
		(float)obj->num_items;
	BTCLUI_Widget_Render3DBox(ox+obj->ox+obj->xs-12,
		oy+obj->oy+obj->ys-24-i,
		12, 12, obj->wbgcolor, 0);

	BTCLUI_Widget_Render3DBox(
		ox+obj->ox+obj->xs-12, oy+obj->oy,
		12, 12, obj->wbgcolor, 0);
	GfxFont_DrawChar(0x27,
		ox+obj->ox+obj->xs-12, oy+obj->oy,
		12, 12, 0, 0, 0, 255);

	BTCLUI_Widget_Render3DBox(
		ox+obj->ox+obj->xs-12, oy+obj->oy+obj->ys-12,
		12, 12, obj->wbgcolor, 0);
	GfxFont_DrawChar(0x26,
		ox+obj->ox+obj->xs-12, oy+obj->oy+obj->ys-12,
		12, 12, 0, 0, 0, 255);
	return(0);
}

int BTCLUI_WidgetList_HandleInput(BTCLUI_Widget *obj,
	BTCLUI_InputState *state, int ox, int oy)
{
	BTCLUI_Event *ev;
	ushort *ks;
	int i;

	if(state->mb&1)obj->flags&=~BTCLUI_GUISTATE_HASFOCUS;

	if(BTCLUI_Widget_CheckClick(ox, oy, state, obj) &&
		(obj->flags&65536) && ((state->mx-ox)>=obj->xs-12))
	{
		if(!BTCLUI_Widget_CheckClicked(ox, oy, state, obj))
			return(0);

		if(((state->my-oy)>=obj->ys-12))
		{
			obj->curval--;
			if(obj->curval<0)obj->curval=0;
			if(obj->curval<obj->min)
				obj->min=obj->curval;
		}
		if((state->my-oy)<=12)
		{
			obj->curval++;
			if(obj->curval>=obj->num_items)
				obj->curval=obj->num_items-1;
			if(obj->curval>=(obj->min+obj->rows))
				obj->min=obj->curval-obj->rows+1;
		}

		return(0);
	}

	if(BTCLUI_Widget_CheckClick(ox, oy, state, obj))
	{
		obj->flags|=BTCLUI_GUISTATE_HASFOCUS;

		i=(state->my-oy)/8;
		i=obj->min+(obj->rows-i-1);

		if((i>=0) && (i<obj->num_items) && (obj->curval!=i))
		{
			obj->curval=i;

			ev=BTCLUI_Widget_AddEvent(obj, "select");
			ev->ival=i;
			ev->sval=dystrdup(obj->item_name[i]);
//			BTCLUI_Widget_SetEventIVal(i);
//			BTCLUI_Widget_SetEventSVal(obj->item_name[i]);
		}
	}

	if(!(obj->flags&BTCLUI_GUISTATE_HASFOCUS))
		return(0);

	i=obj->curval;

	ks=state->keys;
	while(*ks)
	{
		if(*ks==K_UPARROW)
		{
			obj->curval--;
			if(obj->curval<0)obj->curval=0;
			if(obj->curval<obj->min)
				obj->min=obj->curval;
		}
		if(*ks==K_DOWNARROW)
		{
			obj->curval++;
			if(obj->curval>=obj->num_items)
				obj->curval=obj->num_items-1;
			if(obj->curval>=(obj->min+obj->rows))
				obj->min=obj->curval-obj->rows+1;
		}

		ks++;
	}

	if((obj->curval!=i) && (obj->curval>=0))
	{
		ev=BTCLUI_Widget_AddEvent(obj, "select");
		ev->ival=obj->curval;
		ev->sval=dystrdup(obj->item_name[obj->curval]);
//		BTCLUI_Widget_SetEventIVal(obj->curval);
//		BTCLUI_Widget_SetEventSVal(
//			obj->item_name[obj->curval]);
	}

	return(0);
}

#if 0
BTCLUI_Widget *BTCLUI_WidgetList_Begin(char *type, char *id)
{
	BTCLUI_Widget *tmp, *tmp2;
	char *str;
	int fg, bg, wbg;
	int i;

	tmp=gcalloc(sizeof(BTCLUI_Widget));
	tmp->type=dystrdup(type);
	tmp->id=dystrdup(id);
	tmp->fgcolor=btgeclui_context->fgcolor;
	tmp->bgcolor=btgeclui_context->bgcolor;
	tmp->wbgcolor=btgeclui_context->wbgcolor;

	tmp->item_name=gcalloc(256*sizeof(char *));
	tmp->item_value=gcalloc(256*sizeof(char *));
	tmp->num_items=0;

	return(tmp);
}
#endif

int BTCLUI_WidgetList_SizeWidget(BTCLUI_Widget *tmp)
{
	int i, j;

	if(!tmp->rows)
	{
		tmp->rows=tmp->num_items;
		if(tmp->rows>8)
		{
			tmp->flags|=65536;
			tmp->rows=8;
		}
		if(tmp->rows<1)tmp->rows=1;
	}
	if(!tmp->cols)
	{
		for(i=0; i<tmp->num_items; i++)
		{
			j=strlen(tmp->item_value[i]);
			if(j>tmp->cols)tmp->cols=j;
		}
		if(tmp->cols>16)tmp->cols=16;
		if(tmp->cols<4)tmp->cols=4;
	}

	tmp->xs=tmp->cols*8+((tmp->flags&65536)?12:0);
	tmp->ys=tmp->rows*8;

	return(0);
}

#if 0
int BTCLUI_WidgetList_SetPropertyS(BTCLUI_Widget *obj, char *var, char *val)
{
	int i;
	if(!strcmp(var, "value"))
	{
		for(i=0; i<obj->num_items; i++)
			if(!strcmp(obj->item_name[i], val))
		{
			obj->curval=i;
			return(0);
		}
		obj->curval=0;
	}
	return(0);
}
#endif

int BTCLUI_WidgetList_AddItem(BTCLUI_Widget *obj, char *name, char *value)
{
	if(!obj->item_name)
	{
		obj->item_name=gcalloc(256*sizeof(char *));
		obj->item_value=gcalloc(256*sizeof(char *));
		obj->num_items=0;
	}

	obj->item_name[obj->num_items]=dystrdup(name);
	obj->item_value[obj->num_items]=dystrdup(value);
	obj->num_items++;

	if(obj->num_items>obj->rows)
		obj->flags|=65536;

	return(0);
}

int BTCLUI_WidgetList_SetItem(BTCLUI_Widget *obj, char *name, char *value)
{
	int i;

	if(!obj->item_name)
	{
		obj->item_name=gcalloc(256*sizeof(char *));
		obj->item_value=gcalloc(256*sizeof(char *));
		obj->num_items=0;
	}

	for(i=0; i<obj->num_items; i++)
		if(!strcmp(obj->item_name[i], name))
	{
		obj->item_value[i]=dystrdup(value);
		return(0);
	}

	obj->item_name[obj->num_items]=dystrdup(name);
	obj->item_value[obj->num_items]=dystrdup(value);
	obj->num_items++;

	if(obj->num_items>obj->rows)
		obj->flags|=65536;

	return(0);
}

int BTCLUI_WidgetList_ClearItems(BTCLUI_Widget *obj)
{
	obj->num_items=0;
	obj->curval=0;
	obj->flags&=~65536;

	return(0);
}

int BTCLUI_WidgetList_Init()
{
	static char *tylist[]={
		"list", NULL};
	BTCLUI_WidgetType *ty;
	int i;

	for(i=0; tylist[i]; i++)
	{
		ty=BTCLUI_GetWidgetType(tylist[i]);
		ty->Render=BTCLUI_WidgetList_Render;
		ty->HandleInput=BTCLUI_WidgetList_HandleInput;
		ty->Size=BTCLUI_WidgetList_SizeWidget;
		ty->AddItem=BTCLUI_WidgetList_AddItem;
		ty->SetItem=BTCLUI_WidgetList_SetItem;
		ty->ClearItems=BTCLUI_WidgetList_ClearItems;
	}

	return(0);
}
