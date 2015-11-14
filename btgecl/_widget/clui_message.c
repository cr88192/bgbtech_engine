#include <btgecl.h>

BTGECLUI_API BTGECLUI_Widget *BTGECLUI_Forms_GetWidget(char *form, char *widget)
{
	BTGECLUI_Form *tmp;
	BTGECLUI_Widget *obj;
	int i;

	tmp=BTGECLUI_Forms_Lookup(form);
	if(!tmp)return(NULL);

	obj=BTGECLUI_Widget_LookupID(tmp->root, widget);
	if(!obj)obj=BTGECLUI_Widget_FindName(tmp->root, widget);
	return(obj);
}

BTGECLUI_API char *BTGECLUI_Forms_GetPropertyS(char *form, char *widget, char *var)
{
	BTGECLUI_Widget *obj;
	char *s;
	int i;

	obj=BTGECLUI_Forms_GetWidget(form, widget);
	if(!obj)return(NULL);

	if(obj->funcs && obj->funcs->get_property_s)
	{
		s=obj->funcs->get_property_s(obj, var);
		return(s);
	}

	return(NULL);
}

BTGECLUI_API int BTGECLUI_Forms_SetPropertyS(char *form, char *widget, char *var, char *val)
{
	BTGECLUI_Widget *obj;
	int i;

	obj=BTGECLUI_Forms_GetWidget(form, widget);
	if(!obj)return(-1);

	if(obj->funcs && obj->funcs->set_property_s)
	{
		i=obj->funcs->set_property_s(obj, var, val);
		return(i);
	}

	return(-1);
}

BTGECLUI_API int BTGECLUI_Forms_GetPropertyI(char *form, char *widget, char *var)
{
	BTGECLUI_Widget *obj;
	char *s;
	int i;

	obj=BTGECLUI_Forms_GetWidget(form, widget);
	if(!obj)return(0);

	if(obj->funcs && obj->funcs->get_property_s)
	{
		i=obj->funcs->get_property_i(obj, var);
		return(i);
	}

	return(0);
}

BTGECLUI_API int BTGECLUI_Forms_SetPropertyI(char *form, char *widget, char *var, int val)
{
	BTGECLUI_Widget *obj;
	int i;

	obj=BTGECLUI_Forms_GetWidget(form, widget);
	if(!obj)return(-1);

	if(obj->funcs && obj->funcs->set_property_s)
	{
		i=obj->funcs->set_property_i(obj, var, val);
		return(i);
	}

	return(-1);
}
