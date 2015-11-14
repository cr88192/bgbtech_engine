#include <bgbdy.h>

DYSH_ConCmd *dysh_cmdroot;

DYSH_ShellIface dysh_default_iface={
NULL, NULL, NULL, NULL,
DYSH_Puts,
DYSH_AddHistory,
DYSH_GetHistory,
DYSH_ExecuteCmd,
DYSH_ExecuteScript,
DYSH_Complete
};

dyshShell DYSH_AllocShell(int szhist)
{
	dyshShell tmp;
	
	tmp=gctalloc("_dysh_shell_t", sizeof(DYSH_Shell));
	tmp->szhistory=szhist;
	if(szhist>0)tmp->history=gcalloc(szhist*sizeof(char *));
//	tmp->vt=&dysh_default_iface;

	return(tmp);
}

void DYSH_ConsolePuts(DYSH_Console *con, char *str)
{
	if(!con || !con->vt || !con->vt->puts)
		return;
	con->vt->puts(con, str);
}

void DYSH_Puts(dyshShell shell, char *str)
{
	if(!shell)return;
	DYSH_ConsolePuts(shell->con, str);
}

void DYSH_AddHistory(dyshShell shell, char *str)
{
	int i;
	if(!shell)return;

	for(i=shell->szhistory-1; i>0; i--)
		shell->history[i]=shell->history[i-1];
	shell->history[i]=dystrdup(str);
}

char *DYSH_GetHistory(dyshShell shell, int idx)
{
	if((idx<0) || (idx>=shell->szhistory))
		return(NULL);
	return(shell->history[idx]);
}

int DYSH_ExecuteCmd(dyshShell shell, char *str)
{
	DYSH_ConCmd *cur;
	char **a;

	a=gcrsplit(str);
	if(!a[0])return(0);

//	Con_Printf("$ %s\n", str);

	cur=dysh_cmdroot;
	while(cur)
	{
		if(!strcmp(cur->name, a[0]))
		{
			cur->func(shell, a);
			return(0);
		}
		cur=cur->next;
	}

//	Con_Printf("Unrecognized Command '%s'\n", a[0]);
	return(-1);
}

char *DYSH_Complete(dyshShell shell, char *str)
{
	DYSH_ConCmd *cur;
	char **a;

	a=gcrsplit(str);
	if(!a[0])return(NULL);

	cur=dysh_cmdroot;
	while(cur)
	{
		if(!strncmp(cur->name, a[0], strlen(a[0])))
		{
			return(cur->name);
		}
		cur=cur->next;
	}

	return(NULL);
}

char *DYSH_FindLabel(dyshShell shell, char *str, char *lbl)
{
	char *s, *tc, *s0;
	int sz;

	sz=strlen(lbl);
	
	s=str;
	while(*s)
	{
		s0=s;
		while(*s0 && (*s0<=' '))s0++;
		if(!strncmp(s0, lbl, sz) && (s0[sz]==':'))
			return(s0+sz+1);
		tc=gcrgets(&s);
	}
	return(NULL);
}

int DYSH_ExecuteScript(dyshShell shell, char *str)
{
	char *s, *tc, *s0;
	int cnt;
	
	s=str; cnt=8192;
	while(*s && cnt--)
	{
		tc=gcrgets(&s);
		dyshExecuteCmd(shell, tc);

		if(shell->jmp_tgt)
		{
			s0=DYSH_FindLabel(shell, str, shell->jmp_tgt);
			if(s0) { s=s0; continue; }
		}
	}
	return(0);
}

BGBDY_API int DYSH_Register(char *name, char *desc,
	int (*func)(dyshShell shell, char **args))
{
	DYSH_ConCmd *cur, *tmp;

	tmp=gcalloc(sizeof(DYSH_ConCmd));
	memset(tmp, 0, sizeof(DYSH_ConCmd));

	tmp->name=dystrdup(name);
	tmp->desc=dystrdup(desc);
	tmp->func=func;

	if(dysh_cmdroot)
	{
		cur=dysh_cmdroot;
		while(cur->next && (strcmp(cur->next->name, name)<0))
			cur=cur->next;
		tmp->next=cur->next;
		cur->next=tmp;
	}else dysh_cmdroot=tmp;

	return(0);
}



BGBDY_API int dyshRegisterCommand(char *name, char *desc,
	int (*func)(dyshShell shell, char **args))
{
	return(DYSH_Register(name, desc, func));
}

BGBDY_API dyshShell dyshNewShellDefault(void)
{
	dyshShell tmp;
	
	tmp=DYSH_AllocShell(64);
	tmp->vt=&dysh_default_iface;
	return(tmp);
}

BGBDY_API void dyshPuts(dyshShell shell, char *str)
{
	if(!shell || !shell->vt || !shell->vt->Puts)
		shell->vt->Puts(shell, str);
}

BGBDY_API void dyshPrintv(dyshShell shell, char *str, va_list lst)
{
	char tb[4096];
	vsprintf(tb, str, lst);
	dyshPuts(shell, tb);
}

BGBDY_API void dyshPrintf(dyshShell shell, char *str, ...)
{
	va_list lst;
	
	va_start(lst, str);
	dyshPrintv(shell, str, lst);
	va_end(lst);
}

BGBDY_API void dyshAddHistory(dyshShell shell, char *str)
{
	if(!shell || !shell->vt || !shell->vt->AddHistory)
		shell->vt->AddHistory(shell, str);
}

BGBDY_API char *dyshGetHistory(dyshShell shell, int idx)
{
	if(!shell || !shell->vt || !shell->vt->GetHistory)
		return(shell->vt->GetHistory(shell, idx));
	return(NULL);
}

BGBDY_API int dyshExecuteCmd(dyshShell shell, char *str)
{
	if(!shell || !shell->vt || !shell->vt->ExecuteCmd)
		return(shell->vt->ExecuteCmd(shell, str));
	return(-1);
}

BGBDY_API int dyshExecuteScript(dyshShell shell, char *str)
{
	if(!shell || !shell->vt || !shell->vt->ExecuteScript)
		return(shell->vt->ExecuteScript(shell, str));
	return(-1);
}

BGBDY_API char *dyshComplete(dyshShell shell, char *str)
{
	if(!shell || !shell->vt || !shell->vt->Complete)
		return(shell->vt->Complete(shell, str));
	return(NULL);
}

BGBDY_API int dyshExecuteScriptFile(dyshShell shell, char *name)
{
	VFILE *fd;
	byte *buf;
	int i;
	
	fd=vffopen(name, "rb");
	if(!fd)return(-1);
	buf=vf_bufferin(fd);
	vfclose(fd);
	if(!buf)return(-1);
	i=dyshExecuteScript(shell, (char *)buf);
	gcfree(buf);
	return(i);
}
