#include <btgecl.h>

// extern int btgeclui_context->fgcolor;	//text fg color
// extern int btgeclui_context->bgcolor;	//text bg color
// extern int btgeclui_context->wbgcolor;	//widget bg color

int BTGECLUI_WidgetLabel_Render(BTGECLUI_Widget *obj, int ox, int oy)
{
	BTGECLUI_Widget *cur;
	int cx, cy;
	int r, g, b;
	int i, j, k;
	char *s;

	if(!strcmp(obj->type, "label"))
	{
		GfxFont_SetFont("fixed", 0);
		BTGECLUI_Forms_CalcColor(obj->fgcolor, &r, &g, &b);
		GfxFont_DrawFormatString(obj->label,
			ox+obj->ox, oy+obj->oy,
			8, 8, r, g, b, 255);

		return(0);
	}

	if(!strcmp(obj->type, "text"))
	{
		GfxFont_SetFont("fixed", 0);
		BTGECLUI_Forms_CalcColor(obj->fgcolor, &r, &g, &b);
		GfxFont_DrawFormatString(obj->value,
			ox+obj->ox, oy+obj->oy+obj->ys-8,
			8, 8, r, g, b, 255);

//		BTGECLUI_Widget_RenderContent(ox+obj->ox, oy+obj->oy,
//			obj->xs, obj->ys, obj->body, obj->fgcolor);
		return(0);
	}

	if(!strcmp(obj->type, "image"))
	{
		glEnable(GL_TEXTURE_2D);
		Draw_Bind(obj->texnum);

		glColor4f(1, 1, 1, 1);

		glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex2f(ox+obj->ox, oy+obj->oy);
		glTexCoord2f(1, 0);
		glVertex2f(ox+obj->ox+obj->xs, oy+obj->oy);
		glTexCoord2f(1, 1);
		glVertex2f(ox+obj->ox+obj->xs, oy+obj->oy+obj->ys);
		glTexCoord2f(0, 1);
		glVertex2f(ox+obj->ox, oy+obj->oy+obj->ys);
		glEnd();

		return(0);
	}
}

int BTGECLUI_WidgetLabel_HandleInput(BTGECLUI_Widget *obj,
	BTGECLUI_InputState *state, int ox, int oy)
{
	if(BTGECLUI_Widget_CheckClicked(ox, oy, state, obj))
		BTGECLUI_Widget_AddEvent(obj, "clicked");
	if(BTGECLUI_Widget_CheckMClicked(ox, oy, state, obj))
		BTGECLUI_Widget_AddEvent(obj, "mclicked");
	if(BTGECLUI_Widget_CheckRClicked(ox, oy, state, obj))
		BTGECLUI_Widget_AddEvent(obj, "rclicked");
}

#if 0
//BTGECLUI_Widget *BTGECLUI_WidgetLabel_BuildWidget(NetParse_Node *node,
//	BTGECLUI_Form *form, BTGECLUI_Forms_StyleState *style)
{
	BTGECLUI_Widget *tmp, *tmp2;
	char *str;
	int fg, bg, wbg;
	int i;

	if(!node)return(NULL);

	if(!strcmp(node->key, "text"))
	{
		tmp=gcalloc(sizeof(BTGECLUI_Widget));
		tmp->type=dystrdup("text");
		tmp->form=form;
		tmp->funcs=btgeclui_context->widgetlabel_methods;

		tmp->fgcolor=style->fgcolor;
		tmp->bgcolor=style->bgcolor;
		tmp->wbgcolor=style->wbgcolor;

		str=NetParse_GetNodeAttr(node, "color");
		if(str)tmp->fgcolor=BTGECLUI_Forms_DecodeColor(str);

		tmp->body=node->first;
		BTGECLUI_Widget_SizeContent(tmp->body, &tmp->xs, &tmp->ys);

		return(tmp);
	}

	if(!strcmp(node->key, "label"))
	{
		tmp=gcalloc(sizeof(BTGECLUI_Widget));
		tmp->type=dystrdup("label");
		tmp->form=form;
		tmp->funcs=btgeclui_context->widgetlabel_methods;

		tmp->label=dystrdup(node->first->text);
		tmp->size=strlen(tmp->label);

		str=NetParse_GetNodeAttr(node, "size");
		if(str)tmp->size=katoi(str);

		str=NetParse_GetNodeAttr(node, "id");
		if(str)tmp->id=dystrdup(str);

		tmp->fgcolor=style->fgcolor;
		tmp->bgcolor=style->bgcolor;
		tmp->wbgcolor=style->wbgcolor;

		str=NetParse_GetNodeAttr(node, "color");
		if(str)tmp->fgcolor=BTGECLUI_Forms_DecodeColor(str);

		tmp->xs=8*tmp->size;
		tmp->ys=12;

		return(tmp);
	}

	if(!strcmp(node->key, "img"))
	{
		tmp=gcalloc(sizeof(BTGECLUI_Widget));
		tmp->type=dystrdup("image");
		tmp->form=form;
		tmp->funcs=btgeclui_context->widgetlabel_methods;

		str=NetParse_GetNodeAttr(node, "src");
		if(!str)str="images/X.tga";

//		tmp->texnum=BTGECLUI_Texture_LoadImage(str);
//		BTGECLUI_Texture_GetImageSize(tmp->texnum, &tmp->xs, &tmp->ys);
		tmp->texnum=Tex_LoadFile(str, &tmp->xs, &tmp->ys);
		if(tmp->texnum<0)
			tmp->texnum=Tex_LoadFile("images/X.tga",
				&tmp->xs, &tmp->ys);

		str=NetParse_GetNodeAttr(node, "width");
		if(str)tmp->xs=katoi(str);
		str=NetParse_GetNodeAttr(node, "height");
		if(str)tmp->ys=katoi(str);

		return(tmp);
	}
}

int BTGECLUI_WidgetLabel_Init()
{
	BTGECLUI_Widget_RegisterType("label", BTGECLUI_WidgetLabel_BuildWidget);
	BTGECLUI_Widget_RegisterType("text", BTGECLUI_WidgetLabel_BuildWidget);
	BTGECLUI_Widget_RegisterType("img", BTGECLUI_WidgetLabel_BuildWidget);

	btgeclui_context->widgetlabel_methods=
		gcalloc(sizeof(struct BTGECLUI_WidgetMethods_s));
	btgeclui_context->widgetlabel_methods->render=&BTGECLUI_WidgetLabel_Render;
}
#endif

BTGECLUI_Widget *BTGECLUI_WidgetLabel_Begin(char *type, char *id)
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

int BTGECLUI_WidgetLabel_End(BTGECLUI_Widget *tmp)
{
	int i;

	if(!strcmp(tmp->type, "label"))
	{
		tmp->xs=8*strlen(tmp->label);
		tmp->ys=8;
	}

	if(!strcmp(tmp->type, "text"))
	{
		BTGECLUI_Widget_SizeText(tmp->value, &tmp->xs, &tmp->ys);
	}

	return(0);
}

char *BTGECLUI_WidgetLabel_GetPropertyS(BTGECLUI_Widget *obj, char *var)
{
	return(NULL);
}

int BTGECLUI_WidgetLabel_SetPropertyS(BTGECLUI_Widget *obj, char *var, char *val)
{
	if(!strcmp(var, "value"))obj->value=dystrdup(val);
	if(!strcmp(var, "label"))obj->label=dystrdup(val);

	if(!strcmp(var, "image"))
	{
		obj->texnum=Tex_LoadFile(val, &obj->xs, &obj->ys);
		if(obj->texnum<0)
			obj->texnum=Tex_LoadFile("images/X.tga",
				&obj->xs, &obj->ys);
	}
	return(0);
}

int BTGECLUI_WidgetLabel_GetPropertyI(BTGECLUI_Widget *obj, char *var)
{
	return(0);
}

int BTGECLUI_WidgetLabel_SetPropertyI(BTGECLUI_Widget *obj, char *var, int val)
{
	if(!strcmp(var, "width"))obj->xs=val;
	if(!strcmp(var, "height"))obj->ys=val;
	return(0);
}

BTGECLUI_WidgetType btgeclui_context->label_methods=
{
NULL,
"label;text;image",

BTGECLUI_WidgetLabel_Render,
BTGECLUI_WidgetLabel_HandleInput,
NULL,
BTGECLUI_WidgetLabel_Begin,
BTGECLUI_WidgetLabel_End,
NULL,
BTGECLUI_WidgetLabel_GetPropertyS,
BTGECLUI_WidgetLabel_SetPropertyS,
BTGECLUI_WidgetLabel_GetPropertyI,
BTGECLUI_WidgetLabel_SetPropertyI
};

int BTGECLUI_WidgetLabel_Init()
{
	BTGECLUI_Widget_RegisterType(&btgeclui_context->label_methods);
}
