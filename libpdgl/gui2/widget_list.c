#include <pdgl.h>
#include <pdglui.h>

extern int pdglui_fgcolor;	//text fg color
extern int pdglui_bgcolor;	//text bg color
extern int pdglui_wbgcolor;	//widget bg color

int PDGLUI_WidgetList_Render(PDGLUI_Widget *obj, int ox, int oy)
{
	int r, g, b;
	int i, j, k, tc;

	PDGLUI_Forms_CalcColor(obj->bgcolor, &r, &g, &b);
	Draw_FillSquareRGBA(ox+obj->ox, oy+obj->oy,
		obj->xs, obj->ys, r, g, b, 255);

	for(i=0; i<obj->rows; i++)
	{
		j=obj->min+i;
		if(j>=obj->num_items)continue;

		tc=obj->fgcolor;
		if(j==obj->curval)
		{
			if(obj->state&PDGLUI_GUISTATE_HASFOCUS)
				{ r=0; g=0; b=255; tc=0xFFFFFF; }
				else { r=192; g=192; b=192; }

			Draw_FillSquareRGBA(
				ox+obj->ox, oy+obj->oy+obj->ys-(i+1)*8,
				obj->xs, 8, r, g, b, 255);
		}

		GfxFont_SetFont("fixed", 0);
		PDGLUI_Forms_CalcColor(tc, &r, &g, &b);
		GfxFont_DrawString(obj->item_value[j],
			ox+obj->ox, oy+obj->oy+obj->ys-(i+1)*8,
			8, 8, r, g, b, 255);
	}

	if(!(obj->state&65536))return(0);

	GfxFont_SetFont("gfx", 0);

	Draw_FillSquareRGBA(ox+obj->ox+obj->xs-12, oy+obj->oy,
		12, obj->ys, 192, 192, 192, 255);

	i=obj->curval*(obj->ys-36)/
		(float)obj->num_items;
	PDGLUI_Widget_Render3DBox(ox+obj->ox+obj->xs-12,
		oy+obj->oy+obj->ys-24-i,
		12, 12, obj->wbgcolor, 0);

	PDGLUI_Widget_Render3DBox(
		ox+obj->ox+obj->xs-12, oy+obj->oy,
		12, 12, obj->wbgcolor, 0);
	GfxFont_DrawChar(0x27,
		ox+obj->ox+obj->xs-12, oy+obj->oy,
		12, 12, 0, 0, 0, 255);

	PDGLUI_Widget_Render3DBox(
		ox+obj->ox+obj->xs-12, oy+obj->oy+obj->ys-12,
		12, 12, obj->wbgcolor, 0);
	GfxFont_DrawChar(0x26,
		ox+obj->ox+obj->xs-12, oy+obj->oy+obj->ys-12,
		12, 12, 0, 0, 0, 255);
}

int PDGLUI_WidgetList_HandleInput(PDGLUI_Widget *obj,
	PDGLUI_InputState *state, int ox, int oy)
{
	ushort *ks;
	int i;

	if(state->mb&1)obj->state&=~PDGLUI_GUISTATE_HASFOCUS;

	if(PDGLUI_Widget_CheckClick(ox, oy, state, obj) &&
		(obj->state&65536) && ((state->mx-ox)>=obj->xs-12))
	{
		if(!PDGLUI_Widget_CheckClicked(ox, oy, state, obj))
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

	if(PDGLUI_Widget_CheckClick(ox, oy, state, obj))
	{
		obj->state|=PDGLUI_GUISTATE_HASFOCUS;

		i=(state->my-oy)/8;
		i=obj->min+(obj->rows-i-1);

		if((i>=0) && (i<obj->num_items) && (obj->curval!=i))
		{
			obj->curval=i;

			PDGLUI_Widget_AddEvent(obj, "select");
			PDGLUI_Widget_SetEventIVal(i);
			PDGLUI_Widget_SetEventSVal(obj->item_name[i]);
		}
	}

	if(!(obj->state&PDGLUI_GUISTATE_HASFOCUS))
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
		PDGLUI_Widget_AddEvent(obj, "select");
		PDGLUI_Widget_SetEventIVal(obj->curval);
		PDGLUI_Widget_SetEventSVal(
			obj->item_name[obj->curval]);
	}
}

PDGLUI_Widget *PDGLUI_WidgetList_Begin(char *type, char *id)
{
	PDGLUI_Widget *tmp, *tmp2;
	char *str;
	int fg, bg, wbg;
	int i;

	tmp=gcalloc(sizeof(PDGLUI_Widget));
	tmp->type=dystrdup(type);
	tmp->id=dystrdup(id);
	tmp->fgcolor=pdglui_fgcolor;
	tmp->bgcolor=pdglui_bgcolor;
	tmp->wbgcolor=pdglui_wbgcolor;

	tmp->item_name=gcalloc(256*sizeof(char *));
	tmp->item_value=gcalloc(256*sizeof(char *));
	tmp->num_items=0;

	return(tmp);
}

int PDGLUI_WidgetList_End(PDGLUI_Widget *tmp)
{
	int i, j;

	if(!tmp->rows)
	{
		tmp->rows=tmp->num_items;
		if(tmp->rows>8)
		{
			tmp->state|=65536;
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

	tmp->xs=tmp->cols*8+((tmp->state&65536)?12:0);
	tmp->ys=tmp->rows*8;

	return(0);
}

char *PDGLUI_WidgetList_GetPropertyS(PDGLUI_Widget *obj, char *var)
{
	if(!strcmp(var, "value"))return(obj->item_name[obj->curval]);
	return(NULL);
}

int PDGLUI_WidgetList_SetPropertyS(PDGLUI_Widget *obj, char *var, char *val)
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

int PDGLUI_WidgetList_GetPropertyI(PDGLUI_Widget *obj, char *var)
{
	if(!strcmp(var, "value"))return(obj->curval);
	if(!strcmp(var, "rows"))return(obj->rows);
	if(!strcmp(var, "cols"))return(obj->cols);

	return(0);
}

int PDGLUI_WidgetList_SetPropertyI(PDGLUI_Widget *obj, char *var, int val)
{
	if(!strcmp(var, "value"))obj->curval=val;
	if(!strcmp(var, "rows"))obj->rows=val;
	if(!strcmp(var, "cols"))obj->cols=val;
	return(0);
}

int PDGLUI_WidgetList_AddItem(PDGLUI_Widget *obj, char *name, char *value)
{
	obj->item_name[obj->num_items]=dystrdup(name);
	obj->item_value[obj->num_items]=dystrdup(value);
	obj->num_items++;

	if(obj->num_items>obj->rows)
		obj->state|=65536;

	return(0);
}

int PDGLUI_WidgetList_SetItem(PDGLUI_Widget *obj, char *name, char *value)
{
	int i;

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
		obj->state|=65536;

	return(0);
}

int PDGLUI_WidgetList_ClearItems(PDGLUI_Widget *obj)
{
	obj->num_items=0;
	obj->curval=0;
	obj->state&=~65536;
}

PDGLUI_WidgetType pdglui_list_methods=
{
NULL,
"list",

PDGLUI_WidgetList_Render,
PDGLUI_WidgetList_HandleInput,
NULL,
PDGLUI_WidgetList_Begin,
PDGLUI_WidgetList_End,
NULL,
PDGLUI_WidgetList_GetPropertyS,
PDGLUI_WidgetList_SetPropertyS,
PDGLUI_WidgetList_GetPropertyI,
PDGLUI_WidgetList_SetPropertyI,
NULL,
NULL,
PDGLUI_WidgetList_AddItem,
PDGLUI_WidgetList_SetItem,
PDGLUI_WidgetList_ClearItems
};

int PDGLUI_WidgetList_Init()
{
	PDGLUI_Widget_RegisterType(&pdglui_list_methods);
}
