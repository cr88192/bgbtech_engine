#include <pdgl.h>
#include <pdglui.h>

PDGLUI_API PDGLUI_Widget *PDGLUI_Forms_GetWidget(char *form, char *widget)
{
	PDGLUI_Form *tmp;
	PDGLUI_Widget *obj;
	int i;

	tmp=PDGLUI_Forms_Lookup(form);
	if(!tmp)return(NULL);

	obj=PDGLUI_Widget_LookupID(tmp->root, widget);
	if(!obj)obj=PDGLUI_Widget_FindName(tmp->root, widget);
	return(obj);
}

PDGLUI_API char *PDGLUI_Forms_GetPropertyS(char *form, char *widget, char *var)
{
	PDGLUI_Widget *obj;
	char *s;
	int i;

	obj=PDGLUI_Forms_GetWidget(form, widget);
	if(!obj)return(NULL);

	if(obj->funcs && obj->funcs->get_property_s)
	{
		s=obj->funcs->get_property_s(obj, var);
		return(s);
	}

	return(NULL);
}

PDGLUI_API int PDGLUI_Forms_SetPropertyS(char *form, char *widget, char *var, char *val)
{
	PDGLUI_Widget *obj;
	int i;

	obj=PDGLUI_Forms_GetWidget(form, widget);
	if(!obj)return(-1);

	if(obj->funcs && obj->funcs->set_property_s)
	{
		i=obj->funcs->set_property_s(obj, var, val);
		return(i);
	}

	return(-1);
}

PDGLUI_API int PDGLUI_Forms_GetPropertyI(char *form, char *widget, char *var)
{
	PDGLUI_Widget *obj;
	char *s;
	int i;

	obj=PDGLUI_Forms_GetWidget(form, widget);
	if(!obj)return(0);

	if(obj->funcs && obj->funcs->get_property_s)
	{
		i=obj->funcs->get_property_i(obj, var);
		return(i);
	}

	return(0);
}

PDGLUI_API int PDGLUI_Forms_SetPropertyI(char *form, char *widget, char *var, int val)
{
	PDGLUI_Widget *obj;
	int i;

	obj=PDGLUI_Forms_GetWidget(form, widget);
	if(!obj)return(-1);

	if(obj->funcs && obj->funcs->set_property_s)
	{
		i=obj->funcs->set_property_i(obj, var, val);
		return(i);
	}

	return(-1);
}
