#include <btgecl.h>

// extern BTGECLUI_Form *btgeclui_context->forms_root;

// BTGECLUI_Form *btgeclui_context->form;

// BTGECLUI_Widget *btgeclui_context->stack[256];
// int btgeclui_context->stackpos;

// int btgeclui_context->formrover=0;
// int btgeclui_context->widgetrover=0;

// int btgeclui_context->fgcolor=0x000000;	//text fg color
// int btgeclui_context->bgcolor=0xFFFFFF;	//text bg color
// int btgeclui_context->wbgcolor=0xC0C0C0;	//widget bg color

BTGECLUI_Context *btgeclui_context=NULL;

BTGECLUI_API void BTGECLUI_FgColor3I(int r, int g, int b)
{
	btgeclui_context->fgcolor=r|(g<<8)|(b<<16);
}

BTGECLUI_API void BTGECLUI_BgColor3I(int r, int g, int b)
{
	btgeclui_context->bgcolor=r|(g<<8)|(b<<16);
}

BTGECLUI_API void BTGECLUI_WbgColor3I(int r, int g, int b)
{
	btgeclui_context->wbgcolor=r|(g<<8)|(b<<16);
}

BTGECLUI_API BTGECLUI_Form *BTGECLUI_BeginForm(char *type, char *id)
{
	if(id)
	{
		btgeclui_context->form=BTGECLUI_Forms_Lookup(id);
		if(btgeclui_context->form)return(NULL);

	}else
	{
		id=gcrsprint("form_%d", btgeclui_context->formrover++);
	}

	if(!type)type="window";

	btgeclui_context->form=gcalloc(sizeof(BTGECLUI_Form));
	btgeclui_context->form->name=dystrdup(id);
	btgeclui_context->form->style=dystrdup(type);

	btgeclui_context->form->fgcolor=btgeclui_context->fgcolor;
	btgeclui_context->form->bgcolor=btgeclui_context->bgcolor;
	btgeclui_context->form->wbgcolor=btgeclui_context->wbgcolor;

	btgeclui_context->form->next=btgeclui_context->forms_root;
	btgeclui_context->forms_root=btgeclui_context->form;

	btgeclui_context->stackpos=0;

	return(btgeclui_context->form->name);
}

BTGECLUI_API BTGECLUI_Form *BTGECLUI_EndForm()
{
	return(btgeclui_context->form);
	btgeclui_context->form=NULL;
}

BTGECLUI_API void BTGECLUI_FormPropertyS(char *name, char *value)
{
	if(!strcmp(name, "title"))btgeclui_context->form->title=dystrdup(value);
}

BTGECLUI_API void BTGECLUI_FormPropertyI(char *name, int value)
{
}

BTGECLUI_API void BTGECLUI_FormProperty2I(char *name, int x, int y)
{
	if(!strcmp(name, "origin"))
	{
		btgeclui_context->form->ox=x;
		btgeclui_context->form->oy=y;
	}
}

BTGECLUI_API char *BTGECLUI_BeginWidget(char *type, char *id)
{
	BTGECLUI_WidgetType *ty;
	BTGECLUI_Widget *tmp;

	if(!id)id=gcrsprint("widget_%d", btgeclui_context->widgetrover++);

	ty=BTGECLUI_Widget_LookupType(type);
	if(!ty)return(NULL);
	if(!ty->begin)return(NULL);

	tmp=ty->begin(type, id);
	if(!tmp->funcs)tmp->funcs=ty;

	btgeclui_context->stack[btgeclui_context->stackpos++]=tmp;
	return(tmp->id);
}

BTGECLUI_API void BTGECLUI_EndWidget()
{
	BTGECLUI_Widget *tmp, *tmp2;

	btgeclui_context->stackpos--;
	tmp=btgeclui_context->stack[btgeclui_context->stackpos];
	if(tmp->funcs->end)tmp->funcs->end(tmp);

	if(btgeclui_context->stackpos)
	{
		tmp2=btgeclui_context->stack[btgeclui_context->stackpos-1];
		if(!tmp2->funcs->add_widget)
		{
			printf("Widget Type '%s' can't have sub-widgets\n",
				tmp2->type);
			return;
		}

		tmp2->funcs->add_widget(tmp2, tmp);
	}else
	{
		if(btgeclui_context->form->root)
		{
			printf("Forms can only have a single root\n");
			return;
		}

		BTGECLUI_Widget_BindForm(tmp, btgeclui_context->form);

		btgeclui_context->form->root=tmp;
		btgeclui_context->form->xs=tmp->xs;
		btgeclui_context->form->ys=tmp->ys;

		if(btgeclui_context->form->xs<(8*8))
			btgeclui_context->form->xs=8*8;
		if(btgeclui_context->form->ys<(8*8))
			btgeclui_context->form->ys=8*8;
	}
}

BTGECLUI_API void BTGECLUI_WidgetPropertyS(char *name, char *value)
{
	BTGECLUI_Widget *tmp;

	tmp=btgeclui_context->stack[btgeclui_context->stackpos-1];
	tmp->funcs->set_property_s(tmp, name, value);
}

BTGECLUI_API void BTGECLUI_WidgetPropertyI(char *name, int value)
{
	BTGECLUI_Widget *tmp;

	tmp=btgeclui_context->stack[btgeclui_context->stackpos-1];
	tmp->funcs->set_property_i(tmp, name, value);
}

BTGECLUI_API void BTGECLUI_WidgetProperty2I(char *name, int x, int y)
{
	BTGECLUI_Widget *tmp;

	tmp=btgeclui_context->stack[btgeclui_context->stackpos-1];
	tmp->funcs->set_property_2i(tmp, name, x, y);
}

BTGECLUI_API void BTGECLUI_WidgetProperty3I(char *name, int x, int y, int z)
{
	BTGECLUI_Widget *tmp;

	tmp=btgeclui_context->stack[btgeclui_context->stackpos-1];
	tmp->funcs->set_property_3i(tmp, name, x, y, z);
}

BTGECLUI_API void BTGECLUI_Item(char *name, char *value)
{
	BTGECLUI_Widget *tmp;

	tmp=btgeclui_context->stack[btgeclui_context->stackpos-1];
	tmp->funcs->add_item(tmp, name, value);
}
