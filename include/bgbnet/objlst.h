#ifndef __PDNET_OBJLST_H__
#define __PDNET_OBJLST_H__

typedef struct ObjLst_ObjExport_s ObjLst_ObjExport;

struct ObjLst_ObjExport_s {
ObjLst_ObjExport *next;

char *name;
void *obj;
};

ObjLst_ObjExport *ObjLst_LookupExport(char *name);
int ObjLst_ExportObj(char *name, void *obj);
void *ObjLst_FetchObj(char *name);
void *ObjLst_WrapObj(long o, char *type);
long ObjLst_WrapObjID(void *obj);
char *ObjLst_WrapObjType(void *obj);

#endif
