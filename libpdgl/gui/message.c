#include <pdgl.h>
#include <pdglui.h>

int PDGLUI_Forms_Message(PDGLUI_Form *form, char *widget, char *msg,
	int nargs, void **args)
{
	int i;

	if(!form || !widget || !msg)return(-1);
	kprint("event: %s %s %s\n", form->name, widget, msg);

	if(!form->callback)return(-1);

	i=form->callback(form->name, form->data, widget, msg, nargs, args);
	return(i);
}

int PDGLUI_Forms_SendMessage(char *form, char *widget, char *msg,
	int nargs, void **args)
{
	PDGLUI_Form *tmp;
	PDGLUI_Widget *obj;
	int i;

	tmp=PDGLUI_Forms_Lookup(form);
	if(!tmp)return(-1);

	obj=PDGLUI_Widget_LookupID(tmp->root, widget);
	if(!obj)obj=PDGLUI_Widget_FindName(tmp->root, widget);
	if(!obj)return(-1);

	if(obj->funcs && obj->funcs->message)
	{
		i=obj->funcs->message(obj, msg, nargs, args);
		return(i);
	}

	return(-1);
}

PDGLUI_Widget *PDGLUI_Forms_GetWidget(char *form, char *widget)
{
	PDGLUI_Form *tmp;
	PDGLUI_Widget *obj;
	int i;

	tmp=PDGLUI_Forms_Lookup(form);
	if(!tmp)return(-1);

	obj=PDGLUI_Widget_LookupID(tmp->root, widget);
	if(!obj)obj=PDGLUI_Widget_FindName(tmp->root, widget);
	return(obj);
}

void *PDGLUI_Forms_GetProperty(char *form, char *widget, char *var)
{
	PDGLUI_Widget *obj;
	void *p;
	int i;

	obj=PDGLUI_Forms_GetWidget(form, widget);
	if(!obj)return(NULL);

	if(obj->funcs && obj->funcs->get_property)
	{
		p=obj->funcs->get_property(obj, var);
		return(p);
	}

	return(NULL);
}

int PDGLUI_Forms_SetProperty(char *form, char *widget, char *var, void *val)
{
	PDGLUI_Widget *obj;
	int i;

	obj=PDGLUI_Forms_GetWidget(form, widget);
	if(!obj)return(-1);

	if(obj->funcs && obj->funcs->set_property)
	{
		i=obj->funcs->set_property(obj, var, val);
		return(i);
	}

	return(-1);
}

void *PDGLUI_Forms_Call(char *form, char *widget, char *msg,
	int nargs, void **args)
{
	PDGLUI_Form *tmp;
	PDGLUI_Widget *obj;
	void *p;
	int i;

	tmp=PDGLUI_Forms_Lookup(form);
	if(!tmp)return(NULL);

	obj=PDGLUI_Widget_LookupID(tmp->root, widget);
	if(!obj)obj=PDGLUI_Widget_FindName(tmp->root, widget);
	if(!obj)return(-1);

	if(obj->funcs && obj->funcs->call)
	{
		p=obj->funcs->call(obj, msg, nargs, args);
		return(p);
	}

	return(NULL);
}
