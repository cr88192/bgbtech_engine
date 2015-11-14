#include <pdgl.h>
#include <pdglui.h>

extern int pdglui_fgcolor;	//text fg color
extern int pdglui_bgcolor;	//text bg color
extern int pdglui_wbgcolor;	//widget bg color

int PDGLUI_WidgetLabel_Render(PDGLUI_Widget *obj, int ox, int oy)
{
	PDGLUI_Widget *cur;
	int cx, cy;
	int r, g, b;
	int i, j, k;
	char *s;

	if(!strcmp(obj->type, "label"))
	{
		GfxFont_SetFont("fixed", 0);
		PDGLUI_Forms_CalcColor(obj->fgcolor, &r, &g, &b);
		GfxFont_DrawFormatString(obj->label,
			ox+obj->ox, oy+obj->oy,
			8, 8, r, g, b, 255);

		return(0);
	}

	if(!strcmp(obj->type, "text"))
	{
		GfxFont_SetFont("fixed", 0);
		PDGLUI_Forms_CalcColor(obj->fgcolor, &r, &g, &b);
		GfxFont_DrawFormatString(obj->value,
			ox+obj->ox, oy+obj->oy+obj->ys-8,
			8, 8, r, g, b, 255);

//		PDGLUI_Widget_RenderContent(ox+obj->ox, oy+obj->oy,
//			obj->xs, obj->ys, obj->body, obj->fgcolor);
		return(0);
	}

	if(!strcmp(obj->type, "image"))
	{
//		glEnable(GL_TEXTURE_2D);
		pdglEnableTexture2D();
		Draw_Bind(obj->texnum);

		pdglColor4f(1, 1, 1, 1);

		pdglBegin(PDGL_QUADS);
		pdglTexCoord2f(0, 0);
		pdglVertex2f(ox+obj->ox, oy+obj->oy);
		pdglTexCoord2f(1, 0);
		pdglVertex2f(ox+obj->ox+obj->xs, oy+obj->oy);
		pdglTexCoord2f(1, 1);
		pdglVertex2f(ox+obj->ox+obj->xs, oy+obj->oy+obj->ys);
		pdglTexCoord2f(0, 1);
		pdglVertex2f(ox+obj->ox, oy+obj->oy+obj->ys);
		pdglEnd();

		return(0);
	}
}

int PDGLUI_WidgetLabel_HandleInput(PDGLUI_Widget *obj,
	PDGLUI_InputState *state, int ox, int oy)
{
	if(PDGLUI_Widget_CheckClicked(ox, oy, state, obj))
		PDGLUI_Widget_AddEvent(obj, "clicked");
	if(PDGLUI_Widget_CheckMClicked(ox, oy, state, obj))
		PDGLUI_Widget_AddEvent(obj, "mclicked");
	if(PDGLUI_Widget_CheckRClicked(ox, oy, state, obj))
		PDGLUI_Widget_AddEvent(obj, "rclicked");
}

#if 0
//PDGLUI_Widget *PDGLUI_WidgetLabel_BuildWidget(NetParse_Node *node,
//	PDGLUI_Form *form, PDGLUI_Forms_StyleState *style)
{
	PDGLUI_Widget *tmp, *tmp2;
	char *str;
	int fg, bg, wbg;
	int i;

	if(!node)return(NULL);

	if(!strcmp(node->key, "text"))
	{
		tmp=gcalloc(sizeof(PDGLUI_Widget));
		tmp->type=dystrdup("text");
		tmp->form=form;
		tmp->funcs=pdglui_widgetlabel_methods;

		tmp->fgcolor=style->fgcolor;
		tmp->bgcolor=style->bgcolor;
		tmp->wbgcolor=style->wbgcolor;

		str=NetParse_GetNodeAttr(node, "color");
		if(str)tmp->fgcolor=PDGLUI_Forms_DecodeColor(str);

		tmp->body=node->first;
		PDGLUI_Widget_SizeContent(tmp->body, &tmp->xs, &tmp->ys);

		return(tmp);
	}

	if(!strcmp(node->key, "label"))
	{
		tmp=gcalloc(sizeof(PDGLUI_Widget));
		tmp->type=dystrdup("label");
		tmp->form=form;
		tmp->funcs=pdglui_widgetlabel_methods;

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
		if(str)tmp->fgcolor=PDGLUI_Forms_DecodeColor(str);

		tmp->xs=8*tmp->size;
		tmp->ys=12;

		return(tmp);
	}

	if(!strcmp(node->key, "img"))
	{
		tmp=gcalloc(sizeof(PDGLUI_Widget));
		tmp->type=dystrdup("image");
		tmp->form=form;
		tmp->funcs=pdglui_widgetlabel_methods;

		str=NetParse_GetNodeAttr(node, "src");
		if(!str)str="images/X.tga";

//		tmp->texnum=PDGLUI_Texture_LoadImage(str);
//		PDGLUI_Texture_GetImageSize(tmp->texnum, &tmp->xs, &tmp->ys);
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

int PDGLUI_WidgetLabel_Init()
{
	PDGLUI_Widget_RegisterType("label", PDGLUI_WidgetLabel_BuildWidget);
	PDGLUI_Widget_RegisterType("text", PDGLUI_WidgetLabel_BuildWidget);
	PDGLUI_Widget_RegisterType("img", PDGLUI_WidgetLabel_BuildWidget);

	pdglui_widgetlabel_methods=
		gcalloc(sizeof(struct PDGLUI_WidgetMethods_s));
	pdglui_widgetlabel_methods->render=&PDGLUI_WidgetLabel_Render;
}
#endif

PDGLUI_Widget *PDGLUI_WidgetLabel_Begin(char *type, char *id)
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

	return(tmp);
}

int PDGLUI_WidgetLabel_End(PDGLUI_Widget *tmp)
{
	int i;

	if(!strcmp(tmp->type, "label"))
	{
		tmp->xs=8*strlen(tmp->label);
		tmp->ys=8;
	}

	if(!strcmp(tmp->type, "text"))
	{
		PDGLUI_Widget_SizeText(tmp->value, &tmp->xs, &tmp->ys);
	}

	return(0);
}

char *PDGLUI_WidgetLabel_GetPropertyS(PDGLUI_Widget *obj, char *var)
{
	return(NULL);
}

int PDGLUI_WidgetLabel_SetPropertyS(PDGLUI_Widget *obj, char *var, char *val)
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

int PDGLUI_WidgetLabel_GetPropertyI(PDGLUI_Widget *obj, char *var)
{
	return(0);
}

int PDGLUI_WidgetLabel_SetPropertyI(PDGLUI_Widget *obj, char *var, int val)
{
	if(!strcmp(var, "width"))obj->xs=val;
	if(!strcmp(var, "height"))obj->ys=val;
	return(0);
}

PDGLUI_WidgetType pdglui_label_methods=
{
NULL,
"label;text;image",

PDGLUI_WidgetLabel_Render,
PDGLUI_WidgetLabel_HandleInput,
NULL,
PDGLUI_WidgetLabel_Begin,
PDGLUI_WidgetLabel_End,
NULL,
PDGLUI_WidgetLabel_GetPropertyS,
PDGLUI_WidgetLabel_SetPropertyS,
PDGLUI_WidgetLabel_GetPropertyI,
PDGLUI_WidgetLabel_SetPropertyI
};

int PDGLUI_WidgetLabel_Init()
{
	PDGLUI_Widget_RegisterType(&pdglui_label_methods);
}
