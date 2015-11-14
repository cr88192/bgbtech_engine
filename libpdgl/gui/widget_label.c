#include <pdgl.h>
#include <pdglui.h>

struct PDGLUI_WidgetMethods_s *pdglui_widgetlabel_methods=NULL;

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
		GfxFont_DrawString(obj->label, ox+obj->ox, oy+obj->oy,
			8, 12, r, g, b, 255);

		return(0);
	}

	if(!strcmp(obj->type, "text"))
	{
		GfxFont_SetFont("fixed", 0);
		PDGLUI_Widget_RenderContent(ox+obj->ox, oy+obj->oy,
			obj->xs, obj->ys, obj->body, obj->fgcolor);
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

PDGLUI_Widget *PDGLUI_WidgetLabel_BuildWidget(NetParse_Node *node,
	PDGLUI_Form *form, PDGLUI_Forms_StyleState *style)
{
	PDGLUI_Widget *tmp, *tmp2;
	char *str;
	int fg, bg, wbg;
	int i;

	if(!node)return(NULL);

	if(!strcmp(node->key, "text"))
	{
		tmp=kalloc(sizeof(PDGLUI_Widget));
		tmp->type=kstrdup("text");
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
		tmp=kalloc(sizeof(PDGLUI_Widget));
		tmp->type=kstrdup("label");
		tmp->form=form;
		tmp->funcs=pdglui_widgetlabel_methods;

		tmp->label=kstrdup(node->first->text);
		tmp->size=strlen(tmp->label);

		str=NetParse_GetNodeAttr(node, "size");
		if(str)tmp->size=katoi(str);

		str=NetParse_GetNodeAttr(node, "id");
		if(str)tmp->id=kstrdup(str);

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
		tmp=kalloc(sizeof(PDGLUI_Widget));
		tmp->type=kstrdup("image");
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
		kalloc(sizeof(struct PDGLUI_WidgetMethods_s));
	pdglui_widgetlabel_methods->render=&PDGLUI_WidgetLabel_Render;
}
