/*--
Cat pdnet;ObjLst
Text
	objective:
		allow exporting objects;
		allow referring to exported objects;
		handle wrapping of objects;
		...
--*/

#include <bgbnet.h>

ObjLst_ObjExport *objlst_root=NULL;

/*--
Cat pdnet;ObjLst
Form
	ObjLst_ObjExport *ObjLst_LookupExport(char *name);
Description
	Looks up an exported obj and returns export info.
Status Internal
--*/
ObjLst_ObjExport *ObjLst_LookupExport(char *name)
{
	ObjLst_ObjExport *cur;

	cur=objlst_root;
	while(cur)
	{
		if(!strcmp(cur->name, name))
			return(cur);
		cur=cur->next;
	}
	return(NULL);
}

/*--
Cat pdnet;ObjLst
Form
	int ObjLst_ExportObj(char *name, void *obj);
Description
	Export an object to the objlst system.
	obj is expected to be an object within the ObjType system.
	If there is another export with the same name it will be replaced.
--*/
int ObjLst_ExportObj(char *name, void *obj)
{
	ObjLst_ObjExport *tmp;

	tmp=ObjLst_LookupExport(name);
	if(tmp)
	{
		tmp->obj=obj;
		return(0);
	}

	tmp=gcalloc(sizeof(ObjLst_ObjExport));

	tmp->name=dystrdup(name);
	tmp->obj=obj;

	tmp->next=objlst_root;
	objlst_root=tmp;

	return(0);
}

/*--
Cat pdnet;ObjLst
Form
	void *ObjLst_FetchObj(char *name);
Description
	Fetch an exported obj.
--*/
void *ObjLst_FetchObj(char *name)
{
	ObjLst_ObjExport *tmp;

	tmp=ObjLst_LookupExport(name);
	if(tmp)return(tmp->obj);
	return(NULL);
}


/*--
Cat pdnet;ObjLst
Form
	void *ObjLst_WrapObj(long o, char *type);
Description
	Wraps a misc object so that it can be exported or otherwise passed \
	through the objtype system.
	o is an object id (no assumptions are made about meaning, thus, \
	pointers could also be used here).
	type is a string intended to be used to help determine what o is. \
	It has no implicit meaning withing pdlib/pdnet.
--*/
void *ObjLst_WrapObj(long o, char *type)
{
	struct {
	long id;
	char *type;
	}*wrapobj;

	wrapobj=gctalloc("wrapobj_t", 0);
	wrapobj->id=o;
	wrapobj->type=dystrdup(type);

	return(wrapobj);
}

/*--
Cat pdnet;ObjLst
Form
	long ObjLst_WrapObjID(void *obj);
Description
	Fetch the object id associated with a wrapped object.
--*/
long ObjLst_WrapObjID(void *obj)
{
	struct {
	long id;
	char *type;
	}*wrapobj;

	wrapobj=obj;
	return(wrapobj->id);
}

/*--
Cat pdnet;ObjLst
Form
	char *ObjLst_WrapObjType(void *obj);
Description
	Fetch the type name associated with a wrapped object.
--*/
char *ObjLst_WrapObjType(void *obj)
{
	struct {
	long id;
	char *type;
	}*wrapobj;

	wrapobj=obj;
	return(wrapobj->type);
}
