#include <btgecl.h>

int BTCLUI_WidgetInput_Render(BTCLUI_Widget *obj, int ox, int oy)
{
	BTCLUI_Widget *cur;
	int cx, cy;
	int r, g, b;
	int i, j, k;
	char *s;

	if(!strcmp(obj->type, "button"))
	{
		BTCLUI_Forms_CalcColor(obj->wbgcolor, &r, &g, &b);
		Draw_FillSquareRGBA(ox+obj->ox, oy+obj->oy, obj->xs, obj->ys,
			r, g, b, 255);

		if(obj->label)
		{
			GfxFont_SetFont("fixed", 0);
			BTCLUI_Forms_CalcColor(obj->fgcolor, &r, &g, &b);
			GfxFont_DrawFormatString(obj->label,
				ox+obj->ox+2, oy+obj->oy+2,
				8, 8, r, g, b, 255);
		}

		BTCLUI_Widget_Render3DBorder(ox+obj->ox, oy+obj->oy,
			obj->xs, obj->ys, obj->wbgcolor, obj->checked);

		return(0);
	}

	if(!strcmp(obj->type, "checkbox"))
	{
		Draw_FillSquareRGBA(ox+obj->ox, oy+obj->oy+2, 8, 8,
			255, 255, 255, 255);
		BTCLUI_Widget_Render3DBorder(ox+obj->ox, oy+obj->oy+1,
			9, 9, obj->wbgcolor, 1);

		if(obj->checked)
		{
			Draw_FillSquareRGBA(ox+obj->ox+2, oy+obj->oy+3,
				5, 5, 0, 0, 0, 255);
		}

		GfxFont_SetFont("fixed", 0);
		BTCLUI_Forms_CalcColor(obj->fgcolor, &r, &g, &b);
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
		BTCLUI_Forms_CalcColor(obj->fgcolor, &r, &g, &b);
		GfxFont_DrawFormatString(obj->label,
			ox+obj->ox+14, oy+obj->oy+2,
			8, 8, r, g, b, 255);

		return(0);
	}

	if(!strcmp(obj->type, "textbox"))
	{
		BTCLUI_Forms_CalcColor(obj->bgcolor, &r, &g, &b);
		Draw_FillSquareRGBA(ox+obj->ox, oy+obj->oy, obj->xs, obj->ys,
			r, g, b, 255);

		GfxFont_SetFont("fixed", 0);
		BTCLUI_Forms_CalcColor(obj->fgcolor, &r, &g, &b);
		GfxFont_DrawString(obj->buffer, ox+obj->ox+2, oy+obj->oy+2,
			8, 8, r, g, b, 255);

		if(obj->flags&BTCLUI_GUISTATE_HASFOCUS)
		{
			GfxFont_SetFont("gfx", PDGL_FONT_BLINK);

			cx=ox+obj->ox+2+(strlen(obj->buffer)*8);
			cy=oy+obj->oy+2;

			GfxFont_DrawChar(0x21, cx, cy,
				8, 8, 0, 0, 0, 255);
		}

		BTCLUI_Widget_Render3DBorder(ox+obj->ox, oy+obj->oy,
			obj->xs, obj->ys, obj->wbgcolor, 1);

		return(0);
	}

	return(-1);
}

//purposes: initial sizing of widgets; handling resizing of widgets
int BTCLUI_WidgetInput_SizeWidget(BTCLUI_Widget *obj)
{
	BTCLUI_Widget *wcur;

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

int BTCLUI_WidgetInput_HandleInput(BTCLUI_Widget *obj,
	BTCLUI_InputState *state, int ox, int oy)
{
	BTCLUI_Widget *cur;
	BTCLUI_Event *ev;
	int cx, cy;
	int i, j, k;
	unsigned short *ks;
	void *p;

	if(!strcmp(obj->type, "button"))
	{
		if(BTCLUI_Widget_CheckClick(ox, oy, state, obj) ||
			BTCLUI_Widget_CheckHKeyString(state, obj->label))
		{
			if(!obj->checked)
			{
				obj->checked=1;
//				p=NetVal_WrapBool(obj->checked);
//				BTCLUI_Forms_Message(obj->form,
//					obj->id, "down", 1, &p);

				BTCLUI_Widget_AddEvent(obj, "clicked");
			}
		}else if(obj->checked)
		{
			obj->checked=0;
//			p=NetVal_WrapBool(obj->checked);
//			BTCLUI_Forms_Message(obj->form, obj->id, "down", 1, &p);
			BTCLUI_Widget_AddEvent(obj, "unclicked");
		}

		if(BTCLUI_Widget_CheckMClicked(ox, oy, state, obj))
			BTCLUI_Widget_AddEvent(obj, "mclicked");
		if(BTCLUI_Widget_CheckRClicked(ox, oy, state, obj))
			BTCLUI_Widget_AddEvent(obj, "rclicked");

		return(0);
	}
	if(!strcmp(obj->type, "checkbox"))
	{
		if(BTCLUI_Widget_CheckClicked(ox, oy, state, obj) ||
			BTCLUI_Widget_CheckHKeyString(state, obj->label))
		{
			obj->checked=!obj->checked;
//			p=NetVal_WrapBool(obj->checked);
//			BTCLUI_Forms_Message(obj->form, obj->id, "checked", 1, &p);
			BTCLUI_Widget_AddEvent(obj,
				obj->checked?"clicked":"unclicked");
		}
		if(BTCLUI_Widget_CheckMClicked(ox, oy, state, obj))
			BTCLUI_Widget_AddEvent(obj, "mclicked");
		if(BTCLUI_Widget_CheckRClicked(ox, oy, state, obj))
			BTCLUI_Widget_AddEvent(obj, "rclicked");
		return(0);
	}
	if(!strcmp(obj->type, "radio"))
	{
		if(BTCLUI_Widget_CheckClicked(ox, oy, state, obj) ||
			BTCLUI_Widget_CheckHKeyString(state, obj->label))
		{
//			cur=BTCLUI_Widget_FindName(obj->form->root, obj->name);
			cur=BTCLUI_FindFormWidgetName(obj->form, obj->name);
			while(cur)
			{
				cur->checked=0;
				cur=cur->chain;
			}
			obj->checked=1;

			if(obj->value)
			{
//				p=NetVal_WrapString(obj->value);
//				BTCLUI_Forms_Message(obj->form, obj->name,
//					"select", 1, &p);

				ev=BTCLUI_Widget_AddEvent(obj, "select");
				ev->sval=dystrdup(obj->value);
//				BTCLUI_Widget_SetEventSVal(obj->value);
			}
		}
		if(BTCLUI_Widget_CheckMClicked(ox, oy, state, obj))
			BTCLUI_Widget_AddEvent(obj, "mclicked");
		if(BTCLUI_Widget_CheckRClicked(ox, oy, state, obj))
			BTCLUI_Widget_AddEvent(obj, "rclicked");
		return(0);
	}

	if(!strcmp(obj->type, "textbox"))
	{
		if(state->mb&1)obj->flags&=~BTCLUI_GUISTATE_HASFOCUS;

		if(BTCLUI_Widget_CheckClick(ox, oy, state, obj))
			obj->flags|=BTCLUI_GUISTATE_HASFOCUS;

		if((obj->flags&BTCLUI_GUISTATE_HASFOCUS) &&
			!BTCLUI_KeyDown(state, K_CTRL) && !BTCLUI_KeyDown(state, K_ALT))
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
					ev=BTCLUI_Widget_AddEvent(obj, "submit");
					ev->sval=dystrdup(obj->buffer);
//					BTCLUI_Widget_SetEventSVal(obj->buffer);
				}
				ks++;
			}

			if(i)
			{
//				p=NetVal_WrapString(obj->buffer);
//				BTCLUI_Forms_Message(obj->form, obj->id, "changed",
//					1, &p);
				BTCLUI_Widget_AddEvent(obj, "changed");
			}
		}

		if(BTCLUI_Widget_CheckMClicked(ox, oy, state, obj))
			BTCLUI_Widget_AddEvent(obj, "mclicked");
		if(BTCLUI_Widget_CheckRClicked(ox, oy, state, obj))
			BTCLUI_Widget_AddEvent(obj, "rclicked");

		return(0);
	}

	return(-1);
}

int BTCLUI_WidgetInput_Init()
{
	static char *tylist[]={
		"button", "radio", "checkbox", "textbox", NULL};
	BTCLUI_WidgetType *ty;
	int i;

	for(i=0; tylist[i]; i++)
	{
		ty=BTCLUI_GetWidgetType(tylist[i]);
		ty->Render=BTCLUI_WidgetInput_Render;
		ty->HandleInput=BTCLUI_WidgetInput_HandleInput;
		ty->Size=BTCLUI_WidgetInput_SizeWidget;
	}

//	BTCLUI_Widget_RegisterType(&btgeclui_context->input_methods);

	return(0);
}
