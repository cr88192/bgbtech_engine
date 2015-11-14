#include <btgecl.h>

// extern int btgeclui_context->fgcolor;	//text fg color
// extern int btgeclui_context->bgcolor;	//text bg color
// extern int btgeclui_context->wbgcolor;	//widget bg color

int BTGECLUI_WidgetInput_Render(BTGECLUI_Widget *obj, int ox, int oy)
{
	BTGECLUI_Widget *cur;
	int cx, cy;
	int r, g, b;
	int i, j, k;
	char *s;

	if(!strcmp(obj->type, "button"))
	{
		BTGECLUI_Forms_CalcColor(obj->wbgcolor, &r, &g, &b);
		Draw_FillSquareRGBA(ox+obj->ox, oy+obj->oy, obj->xs, obj->ys,
			r, g, b, 255);

		if(obj->label)
		{
			GfxFont_SetFont("fixed", 0);
			BTGECLUI_Forms_CalcColor(obj->fgcolor, &r, &g, &b);
			GfxFont_DrawFormatString(obj->label,
				ox+obj->ox+2, oy+obj->oy+2,
				8, 8, r, g, b, 255);
		}

		BTGECLUI_Widget_Render3DBorder(ox+obj->ox, oy+obj->oy,
			obj->xs, obj->ys, obj->wbgcolor, obj->checked);

		return(0);
	}
	if(!strcmp(obj->type, "checkbox"))
	{
		Draw_FillSquareRGBA(ox+obj->ox, oy+obj->oy+2, 8, 8,
			255, 255, 255, 255);
		BTGECLUI_Widget_Render3DBorder(ox+obj->ox, oy+obj->oy+1,
			9, 9, obj->wbgcolor, 1);

		if(obj->checked)
		{
			Draw_FillSquareRGBA(ox+obj->ox+2, oy+obj->oy+3,
				5, 5, 0, 0, 0, 255);
		}

		GfxFont_SetFont("fixed", 0);
		BTGECLUI_Forms_CalcColor(obj->fgcolor, &r, &g, &b);
		GfxFont_DrawFormatString(obj->label,
			ox+obj->ox+14, oy+obj->oy+2,
			8, 8, r, g, b, 255);

		return(0);
	}
	if(!strcmp(obj->type, "radio"))
	{
		Draw_FillCircle(ox+obj->ox+6, oy+obj->oy+8, 6,
			255, 255, 255, 255);

		if(obj->checked)
		{
			Draw_FillCircle(ox+obj->ox+6, oy+obj->oy+8, 3,
				0, 0, 0, 255);
		}

		GfxFont_SetFont("fixed", 0);
		BTGECLUI_Forms_CalcColor(obj->fgcolor, &r, &g, &b);
		GfxFont_DrawFormatString(obj->label,
			ox+obj->ox+14, oy+obj->oy+2,
			8, 8, r, g, b, 255);

		return(0);
	}

	if(!strcmp(obj->type, "textbox"))
	{
		BTGECLUI_Forms_CalcColor(obj->bgcolor, &r, &g, &b);
		Draw_FillSquareRGBA(ox+obj->ox, oy+obj->oy, obj->xs, obj->ys,
			r, g, b, 255);

		GfxFont_SetFont("fixed", 0);
		BTGECLUI_Forms_CalcColor(obj->fgcolor, &r, &g, &b);
		GfxFont_DrawString(obj->buffer, ox+obj->ox+2, oy+obj->oy+2,
			8, 8, r, g, b, 255);

		if(obj->state&BTGECLUI_GUISTATE_HASFOCUS)
		{
			GfxFont_SetFont("gfx", PDGL_FONT_BLINK);

			cx=ox+obj->ox+2+(strlen(obj->buffer)*8);
			cy=oy+obj->oy+2;

			GfxFont_DrawChar(0x21, cx, cy,
				8, 8, 0, 0, 0, 255);
		}

		BTGECLUI_Widget_Render3DBorder(ox+obj->ox, oy+obj->oy,
			obj->xs, obj->ys, obj->wbgcolor, 1);

		return(0);
	}


	return(-1);
}

//purposes: initial sizing of widgets; handling resizing of widgets
int BTGECLUI_WidgetInput_SizeWidget(BTGECLUI_Widget *obj)
{
	BTGECLUI_Widget *wcur;

	if(!strcmp(obj->type, "button"))
	{
		if(!obj->label)obj->label=obj->value;
		if(!obj->label)obj->label=dystrdup("    ");

		obj->xs=(strlen(obj->label)*8)+4;
		obj->ys=8+4;
	}

	if(!strcmp(obj->type, "checkbox"))
	{
		if(!obj->label)obj->label=obj->value;
		if(!obj->label)obj->label=dystrdup("");

		obj->xs=(strlen(obj->label)*8)+12+4;
		obj->ys=8+4;
	}
	if(!strcmp(obj->type, "radio"))
	{
		if(!obj->label)obj->label=obj->value;
		if(!obj->label)obj->label=dystrdup("");

		obj->xs=(strlen(obj->label)*8)+12+4;
		obj->ys=8+4;
	}

	if(!strcmp(obj->type, "password"))
	{
		if(!obj->value)obj->value="";
		if(!obj->maxlength)obj->maxlength=20;
		obj->buffer=gcalloc(obj->maxlength+1);
		strcpy(obj->buffer, obj->value);

		if(!obj->size)obj->size=20;

		obj->xs=obj->size*8+4;
		obj->ys=8+4;
	}
	if(!strcmp(obj->type, "textbox"))
	{
		if(!obj->value)obj->value="";
		if(!obj->maxlength)obj->maxlength=20;
		obj->buffer=gcalloc(obj->maxlength+1);
		strcpy(obj->buffer, obj->value);
		obj->bufend=obj->buffer+strlen(obj->buffer);

		if(!obj->size)obj->size=20;

		obj->xs=obj->size*8+4;
		obj->ys=8+4;
	}

	return(0);
}

int BTGECLUI_WidgetInput_HandleInput(BTGECLUI_Widget *obj,
	BTGECLUI_InputState *state, int ox, int oy)
{
	BTGECLUI_Widget *cur;
	int cx, cy;
	int i, j, k;
	unsigned short *ks;
	void *p;

	if(!strcmp(obj->type, "button"))
	{
		if(BTGECLUI_Widget_CheckClick(ox, oy, state, obj) ||
			BTGECLUI_Widget_CheckHKeyString(state, obj->label))
		{
			if(!obj->checked)
			{
				obj->checked=1;
//				p=NetVal_WrapBool(obj->checked);
//				BTGECLUI_Forms_Message(obj->form,
//					obj->id, "down", 1, &p);

				BTGECLUI_Widget_AddEvent(obj, "clicked");
			}
		}else if(obj->checked)
		{
			obj->checked=0;
//			p=NetVal_WrapBool(obj->checked);
//			BTGECLUI_Forms_Message(obj->form, obj->id, "down", 1, &p);
			BTGECLUI_Widget_AddEvent(obj, "unclicked");
		}

		if(BTGECLUI_Widget_CheckMClicked(ox, oy, state, obj))
			BTGECLUI_Widget_AddEvent(obj, "mclicked");
		if(BTGECLUI_Widget_CheckRClicked(ox, oy, state, obj))
			BTGECLUI_Widget_AddEvent(obj, "rclicked");

		return(0);
	}
	if(!strcmp(obj->type, "checkbox"))
	{
		if(BTGECLUI_Widget_CheckClicked(ox, oy, state, obj) ||
			BTGECLUI_Widget_CheckHKeyString(state, obj->label))
		{
			obj->checked=!obj->checked;
//			p=NetVal_WrapBool(obj->checked);
//			BTGECLUI_Forms_Message(obj->form, obj->id, "checked", 1, &p);
			BTGECLUI_Widget_AddEvent(obj,
				obj->checked?"clicked":"unclicked");
		}
		if(BTGECLUI_Widget_CheckMClicked(ox, oy, state, obj))
			BTGECLUI_Widget_AddEvent(obj, "mclicked");
		if(BTGECLUI_Widget_CheckRClicked(ox, oy, state, obj))
			BTGECLUI_Widget_AddEvent(obj, "rclicked");
		return(0);
	}
	if(!strcmp(obj->type, "radio"))
	{
		if(BTGECLUI_Widget_CheckClicked(ox, oy, state, obj) ||
			BTGECLUI_Widget_CheckHKeyString(state, obj->label))
		{
			cur=BTGECLUI_Widget_FindName(obj->form->root, obj->name);
			while(cur)
			{
				cur->checked=0;
				cur=cur->chain;
			}
			obj->checked=1;

			if(obj->value)
			{
//				p=NetVal_WrapString(obj->value);
//				BTGECLUI_Forms_Message(obj->form, obj->name,
//					"select", 1, &p);

				BTGECLUI_Widget_AddEvent(obj, "select");
				BTGECLUI_Widget_SetEventSVal(obj->value);
			}
		}
		if(BTGECLUI_Widget_CheckMClicked(ox, oy, state, obj))
			BTGECLUI_Widget_AddEvent(obj, "mclicked");
		if(BTGECLUI_Widget_CheckRClicked(ox, oy, state, obj))
			BTGECLUI_Widget_AddEvent(obj, "rclicked");
		return(0);
	}

	if(!strcmp(obj->type, "textbox"))
	{
		if(state->mb&1)obj->state&=~BTGECLUI_GUISTATE_HASFOCUS;

		if(BTGECLUI_Widget_CheckClick(ox, oy, state, obj))
			obj->state|=BTGECLUI_GUISTATE_HASFOCUS;

		if((obj->state&BTGECLUI_GUISTATE_HASFOCUS) &&
			!BTGECLUI_KeyDown(K_CTRL) && !BTGECLUI_KeyDown(K_ALT))
		{
			i=0;
			ks=state->keys;
			while(*ks)
			{
				if((*ks<127) && (*ks>=' '))
				{
					*obj->bufend++=*ks;
					*obj->bufend=0;
					i=1;
				}
				if(*ks=='\b')
				{
					if(obj->bufend>obj->buffer)
						obj->bufend--;
					*obj->bufend=0;
					i=1;
				}
				if(*ks=='\r')
				{
					BTGECLUI_Widget_AddEvent(obj, "submit");
					BTGECLUI_Widget_SetEventSVal(obj->buffer);
				}
				ks++;
			}

			if(i)
			{
//				p=NetVal_WrapString(obj->buffer);
//				BTGECLUI_Forms_Message(obj->form, obj->id, "changed",
//					1, &p);
				BTGECLUI_Widget_AddEvent(obj, "changed");
			}
		}

		if(BTGECLUI_Widget_CheckMClicked(ox, oy, state, obj))
			BTGECLUI_Widget_AddEvent(obj, "mclicked");
		if(BTGECLUI_Widget_CheckRClicked(ox, oy, state, obj))
			BTGECLUI_Widget_AddEvent(obj, "rclicked");

		return(0);
	}

	return(-1);
}

#if 0
int BTGECLUI_WidgetInput_SendMessage(BTGECLUI_Widget *obj, char *msg,
	int nargs, void **args)
{
	BTGECLUI_Widget *cur;
	int i;

	if(!strcmp(msg, "value"))
	{
		if(!stricmp(obj->type, "textbox"))
		{
			strcpy(obj->buffer, args[0]);
			obj->bufend=obj->buffer+strlen(obj->buffer);
		}else obj->value=dystrdup(args[0]);
	}
	if(!strcmp(msg, "label"))
		obj->label=dystrdup(args[0]);
	if(!strcmp(msg, "checked"))
	{
		if(!stricmp(obj->type, "radio"))
		{
			cur=BTGECLUI_Widget_FindName(obj->form->root, obj->name);
			while(cur)
			{
				cur->checked=0;
				cur=cur->chain;
			}
		}
		obj->checked=NetVal_UnwrapBool(args[0]);
	}
	return(0);
}

int BTGECLUI_WidgetInput_SetProperty(BTGECLUI_Widget *obj,
	char *slot, void *value)
{
	BTGECLUI_Widget *cur;
	char *s;
	int i;

	if(!strcmp(slot, "value"))
	{
		if(!stricmp(obj->type, "textbox"))
		{
			strcpy(obj->buffer, value);
			obj->bufend=obj->buffer+strlen(obj->buffer);
			return(0);
		}

		obj->value=dystrdup(value);
		return(0);
	}

	if(!strcmp(slot, "label"))
	{
		obj->label=dystrdup(value);
		return(0);
	}

	if(!strcmp(slot, "select"))
	{
		if(!stricmp(obj->type, "radio"))
		{
			i=1;
			cur=BTGECLUI_Widget_FindName(obj->form->root, obj->name);
			while(cur)
			{
				cur->checked=0;
				if(cur->value &&
					!strcmp(cur->value, value) && i)
				{
					cur->checked=1;
					i=0;
				}
				cur=cur->chain;
			}
		}
	}

	return(-1);
}
#endif


#if 0
//BTGECLUI_Widget *BTGECLUI_WidgetInput_BuildWidget(NetParse_Node *node,
//	BTGECLUI_Form *form, BTGECLUI_Forms_StyleState *style)
{
	BTGECLUI_Widget *tmp, *tmp2;
	char *str;
	int fg, bg, wbg;
	int i;

	if(!node)return(NULL);

	if(!strcmp(node->key, "input"))
	{
		tmp=gcalloc(sizeof(BTGECLUI_Widget));
		tmp->form=form;
		tmp->funcs=btgeclui_context->widgetinput_methods;

		str=NetParse_GetNodeAttr(node, "type");
		if(!str)str="text";
		if(!strcmp(str, "text"))str="textbox";
		if(!strcmp(str, "radio"))str="radio";
		tmp->type=dystrdup(str);

		str=NetParse_GetNodeAttr(node, "name");
		if(str)tmp->name=dystrdup(str);
		str=NetParse_GetNodeAttr(node, "id");
		if(str)tmp->id=dystrdup(str);
		str=NetParse_GetNodeAttr(node, "value");
		if(str)tmp->value=dystrdup(str);
		str=NetParse_GetNodeAttr(node, "label");
		if(str)tmp->label=dystrdup(str);

		str=NetParse_GetNodeAttr(node, "checked");
		if(str)tmp->checked=1;

		str=NetParse_GetNodeAttr(node, "size");
		if(str)tmp->size=katoi(str);
		str=NetParse_GetNodeAttr(node, "maxlength");
		if(str)tmp->maxlength=katoi(str);

		tmp->fgcolor=style->fgcolor;
		tmp->bgcolor=style->bgcolor;
		tmp->wbgcolor=style->wbgcolor;

		str=NetParse_GetNodeAttr(node, "fgcolor");
		if(str)tmp->fgcolor=BTGECLUI_Forms_DecodeColor(str);
		str=NetParse_GetNodeAttr(node, "color");
		if(str)tmp->fgcolor=BTGECLUI_Forms_DecodeColor(str);

		str=NetParse_GetNodeAttr(node, "bgcolor");
		if(str)tmp->bgcolor=BTGECLUI_Forms_DecodeColor(str);
		str=NetParse_GetNodeAttr(node, "wbgcolor");
		if(str)tmp->wbgcolor=BTGECLUI_Forms_DecodeColor(str);

		BTGECLUI_WidgetInput_SizeWidget(tmp);

		return(tmp);
	}

	return(NULL);
}
#endif

BTGECLUI_Widget *BTGECLUI_WidgetInput_Begin(char *type, char *id)
{
	BTGECLUI_Widget *tmp, *tmp2;
	char *str;
	int fg, bg, wbg;
	int i;

	tmp=gcalloc(sizeof(BTGECLUI_Widget));
	tmp->type=dystrdup(type);
	tmp->id=dystrdup(id);
	tmp->fgcolor=btgeclui_context->fgcolor;
	tmp->bgcolor=btgeclui_context->bgcolor;
	tmp->wbgcolor=btgeclui_context->wbgcolor;

	return(tmp);
}

int BTGECLUI_WidgetInput_End(BTGECLUI_Widget *tmp)
{
	int i;
	BTGECLUI_WidgetInput_SizeWidget(tmp);
	return(0);
}

char *BTGECLUI_WidgetInput_GetPropertyS(BTGECLUI_Widget *obj, char *var)
{
	if(!strcmp(var, "text"))return(dystrdup(obj->buffer));
	return(NULL);
}

int BTGECLUI_WidgetInput_SetPropertyS(BTGECLUI_Widget *obj, char *var, char *val)
{
	if(!strcmp(var, "name"))obj->name=dystrdup(val);
	if(!strcmp(var, "value"))obj->value=dystrdup(val);
	if(!strcmp(var, "label"))obj->label=dystrdup(val);

	if(!strcmp(var, "text"))
	{
		strcpy(obj->buffer, val);
		obj->bufend=obj->buffer+strlen(val);
	}

	return(0);
}

int BTGECLUI_WidgetInput_GetPropertyI(BTGECLUI_Widget *obj, char *var)
{
	if(!strcmp(var, "checked"))return(obj->checked);
	return(0);
}

int BTGECLUI_WidgetInput_SetPropertyI(BTGECLUI_Widget *obj, char *var, int val)
{
	if(!strcmp(var, "checked"))obj->checked=val;
	if(!strcmp(var, "size"))obj->size=val;
	if(!strcmp(var, "maxlength"))obj->maxlength=val;

	return(0);
}

BTGECLUI_WidgetType btgeclui_context->input_methods=
{
NULL,
"button;radio;checkbox;textbox",

BTGECLUI_WidgetInput_Render,
BTGECLUI_WidgetInput_HandleInput,
NULL,
BTGECLUI_WidgetInput_Begin,
BTGECLUI_WidgetInput_End,
NULL,
BTGECLUI_WidgetInput_GetPropertyS,
BTGECLUI_WidgetInput_SetPropertyS,
BTGECLUI_WidgetInput_GetPropertyI,
BTGECLUI_WidgetInput_SetPropertyI
};

int BTGECLUI_WidgetInput_Init()
{
	BTGECLUI_Widget_RegisterType(&btgeclui_context->input_methods);
}
