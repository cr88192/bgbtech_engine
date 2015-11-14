#include <btgecm.h>


BTGE_Cvar *cvars_root;

BTGE_Cvar *BTGE_LookupCvar(char *name)
{
	BTGE_Cvar *cur;
	
	cur=cvars_root;
	while(cur)
	{
		if(!strcmp(cur->name, name))
			return(cur);
		cur=cur->next;
	}
	
	return(NULL);
}

BTGE_Cvar *BTGE_GetCvar(char *name)
{
	BTGE_Cvar *cur;

	cur=BTGE_LookupCvar(name);
	if(cur)return(cur);
	
	cur=gctalloc("btge_cvar_t", sizeof(BTGE_Cvar));
	cur->name=dystrdup(name);
	cur->next=cvars_root;
	cvars_root=cur;
	
	return(cur);
}

BTGE_Cvar *BTGE_LookupCvarPrefix(char *name)
{
	BTGE_Cvar *cur;
	
	cur=cvars_root;
	while(cur)
	{
		if(!strncmp(cur->name, name, strlen(name)))
			return(cur);
		cur=cur->next;
	}
	
	return(NULL);
}

BTGE_API char *BTGE_CvarComplete(char *name)
{
	BTGE_Cvar *cvar;

	cvar=BTGE_LookupCvarPrefix(name);
	if(cvar)return(cvar->name);
	return(NULL);
}

BTGE_API char *BTGE_CvarGet(char *name)
{
	char tb[256];
	BTGE_Cvar *cvar;
	char *s;

//	cvar=BTGE_GetCvar(name);
	cvar=BTGE_LookupCvar(name);
	if(cvar && cvar->value)
		return(cvar->value);

	sprintf(tb, "console/cvar/%s:value", name);
	s=dyllMetaLookupKey(tb);
	if(s)
	{
		cvar=BTGE_GetCvar(name);
		cvar->value=dystrdup(s);
	}
	return(s);
}

BTGE_API bool BTGE_CvarSet(char *name, char *val)
{
	char tb[256];
	BTGE_Cvar *cvar;
	char *s;

	cvar=BTGE_GetCvar(name);
	if(cvar)cvar->value=dystrdup(val);

	sprintf(tb, "console/cvar/%s", name);
	dyllMetaBindKey(tb, "cvar");
	sprintf(tb, "console/cvar/%s:value", name);
	dyllMetaBindKey(tb, val);
	return(true);
}

BTGE_API bool BTGE_CvarSetDefault(char *name, char *val)
{
	char *s;
	s=BTGE_CvarGet(name);
	if(s)return(false);
	return(BTGE_CvarSet(name, val));
}

BTGE_API double BTGE_CvarGetNum(char *name)
{
	char *s;
	s=BTGE_CvarGet(name);
	if(s)return(atof(s));
	return(0);
}

BTGE_API bool BTGE_CvarSetNum(char *name, double val)
{
	char tb[256];
	sprintf(tb, "%f", val);
	return(BTGE_CvarSet(name, tb));
}

BTGE_API char *BTGE_AliasGet(char *str)
{
	char tb[256];
	char *s;

	sprintf(tb, "console/alias/%s:value", str);
	s=dyllMetaLookupKey(tb);
	return(s);
}

BTGE_API bool BTGE_AliasSet(char *name, char *val)
{
	char tb[256];
	char *s;

	sprintf(tb, "console/alias/%s:value", name);
	dyllMetaBindKey(tb, val);
}

static int (*btge_con_handler)(char *str);
static int (*con_doExecCmds)(void);

BTGE_API int BTGE_SetConsoleHandler(int (*fcn)(char *str))
{
	btge_con_handler=fcn;
	return(0);
}

BTGE_API int BTGE_ConsoleCmd(char *str)
{
	if(btge_con_handler)
		return(btge_con_handler(str));
	return(-1);
}

BTGE_API int BTGE_SetConsoleDoExec(int (*fcn)(void))
{
	con_doExecCmds=fcn;
	return(0);
}

BTGE_API int BTGE_DoExecCmds(void)
{
	if(con_doExecCmds)
		return(con_doExecCmds());
	return(-1);
}


BTGE_API char *btCvarGet(char *var)
{
	char tb[256];
	char *s;
	
//	sprintf(tb, "console/cvar/%s:value", var);
//	s=dyllMetaLookupKey(tb);

	s=BTGE_CvarGet(var);

	return(s);
}

BTGE_API void btCvarSet(char *var, char *val)
{
	char tb[256];
	char *s;
	
//	sprintf(tb, "console/cvar/%s", var);
//	dyllMetaBindKey(tb, "cvar");
//	sprintf(tb, "console/cvar/%s:value", var);
//	dyllMetaBindKey(tb, val);

	BTGE_CvarSet(var, val);
}

BTGE_API void btCvarSetDefault(char *var, char *val)
{
	char tb[256];
	char *s;
	
//	sprintf(tb, "console/cvar/%s:value", var);
//	s=dyllMetaLookupKey(tb);
	s=BTGE_CvarGet(var);
	if(s)return;
//	dyllMetaBindKey(tb, val);
	BTGE_CvarSet(var, val);
}

BTGE_API double btCvarGetf(char *var)
{
	char *s;
	s=btCvarGet(var);
	if(!s)return(0);
	return(atof(s));
}

BTGE_API void btCvarSetf(char *var, double val)
{
	char tb[256];
	
	if(val==((int)val))
			sprintf(tb, "%d", (int)val);
	else	sprintf(tb, "%f", val);
	btCvarSet(var, tb);
}

BTGE_API bool btCvarGet3fv(char *var, float *fv)
{
	char *s;
	s=btCvarGet(var);
	if(!s)
	{
		V3F_ZERO(fv);
		return(false);
	}
	
	V3F_ZERO(fv);
	sscanf(s, "%f %f %f", fv+0, fv+1, fv+2);
	return(true);
}

BTGE_API bool btCvarGet4fv(char *var, float *fv)
{
	char *s;
	s=btCvarGet(var);
	if(!s)
	{
		V4F_ZERO(fv);
		return(false);
	}
	
	V4F_ZERO(fv);
	sscanf(s, "%f %f %f %f", fv+0, fv+1, fv+2, fv+3);
	return(true);
}

BTGE_API int btCvarGeti(char *var)
{
	char *s;
	s=btCvarGet(var);
	if(!s)return(0);
	return(atoi(s));
}

BTGE_API dyt btScriptEval(char *str)
{
	int fl;
	
	fl=0;
	if(btCvarGeti("vm_debugstep"))
		fl|=BSVM_VMGFL_DEBUGSTEP;

	return(BSVM_EvalStringFlags(str, fl));
}

BTGE_API dyt btScriptEvalObj(char *str, dyt obj)
{
	int fl;
	
	fl=0;
	if(btCvarGeti("vm_debugstep"))
		fl|=BSVM_VMGFL_DEBUGSTEP;

	return(BSVM_EvalStringObjFlags(str, obj, fl));
}

BTGE_API dyt btScriptLoad(char *name)
{
	return(BSVM_LoadScript(name, NULL));
}


BTGE_API dyt btScriptEvalFromServer(char *str)
{
	int fl;
	
	fl=0;
//	fl|=BSVM_VMGFL_UNTRUSTED;
//	fl|=BSVM_VMGFL_FROMSERVER;
	if(btCvarGeti("vm_debugstep"))
		fl|=BSVM_VMGFL_DEBUGSTEP;

	return(BSVM_EvalStringFlags(str, fl));
}

BTGE_API int BTGE_ConCmd_TryExec(char *str)
{
	char tb[256];
	char **a;
	char *s;
	int i;

	a=gcrsplit(str);
	if(!a[0])return(-1);

	sprintf(tb, "game/bin/%s.cfg", a[0]);
	i=btSequenceExecArgv(tb, a);
	if(i>=0)return(i);

	sprintf(tb, "game/bin/%s.bs", a[0]);
	i=BTGE_ConCmd_TryExecScript(tb, a);
	if(i>=0)return(i);

	return(-1);
}

BTGE_API int BTGE_ConCmd_TryExecScript(char *str, char **args)
{
	dyt arr[64];
	dyt top, arv;
	dyt t;
	char *buf;
	int sz, arc;
	int i;

	buf=BSVM_ScriptBufferIn(str, &sz);
	if(!buf)return(-1);
	gcfree(buf);

	top=BSVM_CreateLocalToplevel();
	
	for(arc=0; args[arc]; arc++)
		arr[arc]=dystring(args[arc]);
	arv=dyWrapArray(arr, arc);
//	dyBind(top, "argv", arv);
	
	t=BSVM_LoadScriptTop(str, NULL, top);
	if(t==UNDEFINED)
		return(-1);
	BSVM_CallFuncTopObj(top, NULL, dysymbol("main"),
		dylist2(dyint(arc), arv));

	return(0);
}
