#include <pdgl.h>
#include <pdglui.h>

extern PDGLUI_Form *pdglui_forms_root;

PDGLUI_Form *pdglui_form;

PDGLUI_Widget *pdglui_stack[256];
int pdglui_stackpos;

int pdglui_formrover=0;
int pdglui_widgetrover=0;

int pdglui_fgcolor=0x000000;	//text fg color
int pdglui_bgcolor=0xFFFFFF;	//text bg color
int pdglui_wbgcolor=0xC0C0C0;	//widget bg color

PDGLUI_API void PDGLUI_FgColor3I(int r, int g, int b)
{
	pdglui_fgcolor=r|(g<<8)|(b<<16);
}

PDGLUI_API void PDGLUI_BgColor3I(int r, int g, int b)
{
	pdglui_bgcolor=r|(g<<8)|(b<<16);
}

PDGLUI_API void PDGLUI_WbgColor3I(int r, int g, int b)
{
	pdglui_wbgcolor=r|(g<<8)|(b<<16);
}

PDGLUI_API char *PDGLUI_BeginForm(char *type, char *id)
{
	if(id)
	{
		pdglui_form=PDGLUI_Forms_Lookup(id);
		if(pdglui_form)return(NULL);

	}else
	{
		id=gcrsprint("form_%d", pdglui_formrover++);
	}

	if(!type)type="window";

	pdglui_form=gcalloc(sizeof(PDGLUI_Form));
	pdglui_form->name=dystrdup(id);
	pdglui_form->style=dystrdup(type);

	pdglui_form->fgcolor=pdglui_fgcolor;
	pdglui_form->bgcolor=pdglui_bgcolor;
	pdglui_form->wbgcolor=pdglui_wbgcolor;

	pdglui_form->next=pdglui_forms_root;
	pdglui_forms_root=pdglui_form;

	pdglui_stackpos=0;

	return(pdglui_form->name);
}

PDGLUI_API void PDGLUI_EndForm()
{
	pdglui_form=NULL;
}

PDGLUI_API void PDGLUI_FormPropertyS(char *name, char *value)
{
	if(!strcmp(name, "title"))pdglui_form->title=dystrdup(value);
}

PDGLUI_API void PDGLUI_FormPropertyI(char *name, int value)
{
}

PDGLUI_API void PDGLUI_FormProperty2I(char *name, int x, int y)
{
	if(!strcmp(name, "origin"))
	{
		pdglui_form->ox=x;
		pdglui_form->oy=y;
	}
}

PDGLUI_API char *PDGLUI_BeginWidget(char *type, char *id)
{
	PDGLUI_WidgetType *ty;
	PDGLUI_Widget *tmp;

	if(!id)id=gcrsprint("widget_%d", pdglui_widgetrover++);

	ty=PDGLUI_Widget_LookupType(type);
	if(!ty)return(NULL);
	if(!ty->begin)return(NULL);

	tmp=ty->begin(type, id);
	if(!tmp->funcs)tmp->funcs=ty;

	pdglui_stack[pdglui_stackpos++]=tmp;
	return(tmp->id);
}

PDGLUI_API void PDGLUI_EndWidget()
{
	PDGLUI_Widget *tmp, *tmp2;

	pdglui_stackpos--;
	tmp=pdglui_stack[pdglui_stackpos];
	if(tmp->funcs->end)tmp->funcs->end(tmp);

	if(pdglui_stackpos)
	{
		tmp2=pdglui_stack[pdglui_stackpos-1];
		if(!tmp2->funcs->add_widget)
		{
			printf("Widget Type '%s' can't have sub-widgets\n",
				tmp2->type);
			return;
		}

		tmp2->funcs->add_widget(tmp2, tmp);
	}else
	{
		if(pdglui_form->root)
		{
			printf("Forms can only have a single root\n");
			return;
		}

		PDGLUI_Widget_BindForm(tmp, pdglui_form);

		pdglui_form->root=tmp;
		pdglui_form->xs=tmp->xs;
		pdglui_form->ys=tmp->ys;

		if(pdglui_form->xs<(8*8))
			pdglui_form->xs=8*8;
		if(pdglui_form->ys<(8*8))
			pdglui_form->ys=8*8;
	}
}

PDGLUI_API void PDGLUI_WidgetPropertyS(char *name, char *value)
{
	PDGLUI_Widget *tmp;

	tmp=pdglui_stack[pdglui_stackpos-1];
	tmp->funcs->set_property_s(tmp, name, value);
}

PDGLUI_API void PDGLUI_WidgetPropertyI(char *name, int value)
{
	PDGLUI_Widget *tmp;

	tmp=pdglui_stack[pdglui_stackpos-1];
	tmp->funcs->set_property_i(tmp, name, value);
}

PDGLUI_API void PDGLUI_WidgetProperty2I(char *name, int x, int y)
{
	PDGLUI_Widget *tmp;

	tmp=pdglui_stack[pdglui_stackpos-1];
	tmp->funcs->set_property_2i(tmp, name, x, y);
}

PDGLUI_API void PDGLUI_WidgetProperty3I(char *name, int x, int y, int z)
{
	PDGLUI_Widget *tmp;

	tmp=pdglui_stack[pdglui_stackpos-1];
	tmp->funcs->set_property_3i(tmp, name, x, y, z);
}

PDGLUI_API void PDGLUI_Item(char *name, char *value)
{
	PDGLUI_Widget *tmp;

	tmp=pdglui_stack[pdglui_stackpos-1];
	tmp->funcs->add_item(tmp, name, value);
}
