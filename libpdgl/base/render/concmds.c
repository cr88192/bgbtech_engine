#include <pdgl.h>

#ifndef MSVC
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#endif

struct ConCmds_Cmd_s {
struct ConCmds_Cmd_s *next;
char *name;
char *desc;
int (*func)(char **args);
};

struct ConCmds_Cmd_s *comcmds_root;
BTGE_ConsoleInfo *con_info;

#if 0
PDGL_API char *ConCmds_CvarGet(char *name)
{
	char tb[256];
	char *s;

	sprintf(tb, "console/cvar/%s:value", name);
	s=dyllMetaLookupKey(tb);
	return(s);
}

PDGL_API bool ConCmds_CvarSet(char *name, char *val)
{
	char tb[256];
	char *s;

	sprintf(tb, "console/cvar/%s", name);
	dyllMetaBindKey(tb, "cvar");
	sprintf(tb, "console/cvar/%s:value", name);
	dyllMetaBindKey(tb, val);
	return(true);
}

PDGL_API char *ConCmds_CvarComplete(char *str)
{
	return(NULL);
}

#endif

#if 1
PDGL_API char *ConCmds_CvarGet(char *name)
	{ return(BTGE_CvarGet(name)); }
PDGL_API bool ConCmds_CvarSet(char *name, char *val)
	{ return(BTGE_CvarSet(name, val)); }
PDGL_API char *ConCmds_CvarComplete(char *str)
	{ return(BTGE_CvarComplete(str)); }
#endif

PDGL_API double ConCmds_CvarGetNum(char *name)
{
	char *s;
	s=ConCmds_CvarGet(name);
	if(s)return(atof(s));
	return(0);
}

PDGL_API bool ConCmds_CvarSetNum(char *name, double val)
{
	char tb[256];
	sprintf(tb, "%f", val);
	return(ConCmds_CvarSet(name, tb));
}

PDGL_API char *ConCmds_AliasGet(char *str)
{
	char tb[256];
	char *s;

	sprintf(tb, "console/alias/%s:value", str);
	s=dyllMetaLookupKey(tb);
	return(s);
}

PDGL_API bool ConCmds_AliasSet(char *name, char *val)
{
	char tb[256];
	char *s;

	sprintf(tb, "console/alias/%s:value", name);
	dyllMetaBindKey(tb, val);
}

int concmd_isalpha(int c)
{
	if((c>='a') && (c<='z'))return(1);
	if((c>='A') && (c<='Z'))return(1);
	return(0);
}

//handler which tries to filter out potentially unsafe commands
PDGL_API int ConCmds_FilteredHandler(char *str)
{
	char **a;
	char *s;

	a=gcrsplit(str);
	if(!a[0])return(-1);

	if(!concmd_isalpha(*a[0]))
		return(-1);
		
	if(!strcmp(a[0], "eval"))return(-1);
	if(!strcmp(a[0], "sv"))return(-1);
	if(!strcmp(a[0], "exec"))return(-1);

	return(ConCmds_Handler(str));
}

PDGL_API int ConCmds_Handler(char *str)
{
	char tb[256];
	struct ConCmds_Cmd_s *cur;
	char **a;
	char *s;
	void *p;
	int i;

	a=gcrsplit(str);
	if(!a[0])return(0);

#if 1
	//Another Hack
	if(*str=='*')
	{
		Con_Printf("$ %s\n", str);
		return(BTGE_ConEdit_Handler(con_info, str));
	}
#endif

#if 1
	//HACK...
	if(*str==';')
	{
		a=gcralloc(3*sizeof(char *));
		a[0]="eval";
		a[1]=str+1;
		a[2]=NULL;
	}
#endif

	Con_Printf("$ %s\n", str);

	cur=comcmds_root;
	while(cur)
	{
		if(!strcmp(cur->name, a[0]))
		{
			cur->func(a);
			return(0);
		}
		cur=cur->next;
	}

//	sprintf(tb, "console/cvar/%s:value", a[0]);
//	s=dyllMetaLookupKey(tb);
	s=ConCmds_CvarGet(a[0]);
	if(s)
	{
		if(a[1])
		{
//			dyllMetaBindKey(tb, a[1]);
			ConCmds_CvarSet(a[0], a[1]);
			return(0);
		}
		Con_Printf("%s = %s\n", a[0], s);
		return(0);
	}

	s=ConCmds_AliasGet(a[0]);
	if(s)
	{
		Con_StuffCmdBuf(s);
		return(0);
	}

	sprintf(tb, "ConCmd_%s", a[0]);
	p=dyllGetAddr(tb);
	if(p)
	{
		ConCmds_Register(a[0], NULL, p);
		((int (*)(char **))p)(a);
		return(0);
	}

	i=BTGE_ConCmd_TryExec(str);
	if(i>=0)return(i);

	Con_Printf("Unrecognized Command '%s'\n", a[0]);
	return(-1);
}

PDGL_API char *ConCmds_Complete(char *str)
{
	struct ConCmds_Cmd_s *cur;
	char **a;
	char *s;

	a=gcrsplit(str);
	if(!a[0])return(NULL);

	cur=comcmds_root;
	while(cur)
	{
		if(!strncmp(cur->name, a[0], strlen(a[0])))
		{
			return(cur->name);
		}
		cur=cur->next;
	}
	
	s=ConCmds_CvarComplete(a[0]);
	if(s)return(s);

	return(NULL);
}

PDGL_API int ConCmds_Register(char *name, char *desc, int (*func)(char **args))
{
	struct ConCmds_Cmd_s *cur, *tmp;

	tmp=malloc(sizeof(struct ConCmds_Cmd_s));
	memset(tmp, 0, sizeof(struct ConCmds_Cmd_s));

	tmp->name=strdup(name);
	if(desc)tmp->desc=strdup(desc);
	tmp->func=func;

	if(comcmds_root)
	{
		cur=comcmds_root;
		while(cur->next && (strcmp(cur->next->name, name)<0))
			cur=cur->next;
		tmp->next=cur->next;
		cur->next=tmp;
	}else comcmds_root=tmp;

	return(0);
}

// #ifndef MSVC
#if 0
int ConCmds_Cwd(char **a)
{
	char buf[256];

	getcwd(buf, 255);
	Con_Printf("%s\n", buf);
	return(0);
}

int ConCmds_Chdir(char **a)
{
	char buf[256];

//	Con_Printf("chdir %s\n", a[1]);
	chdir(a[1]);

	getcwd(buf, 255);
	Con_Printf("%s\n", buf);

	return(0);
}

int ConCmds_Mkdir(char **a)
{
	char buf[256];

#ifdef WIN32
	mkdir(a[1]);
#endif

#ifdef LINUX
	mkdir(a[1], 0);
#endif

	return(0);
}

int ConCmds_Ls(char **a)
{
	char buf[256];
	DIR *dir;
	struct dirent *de;
	int i, j, k, l;

	getcwd(buf, 255);
	dir=opendir(buf);

	if(!dir)return(-1);

	i=0;
	while(1)
	{
		de=readdir(dir);
		if(!de)break;
		if(strlen(de->d_name)>i)i=strlen(de->d_name);
	}

	if(!i)return(0);
	rewinddir(dir);

	i++;
	j=80/i;

	k=0;
	while(1)
	{
		de=readdir(dir);
		if(!de)break;

		Con_WriteString(de->d_name);
		l=i-strlen(de->d_name);
		while(l--)Con_WriteString(" ");

		k++;
		if(k>=j)
		{
			Con_WriteString("\n");
			k=0;
		}
	}
	if(k)Con_WriteString("\n");
}
#endif

#if 1
static char concmds_cwd[256];

int ConCmds_Cwd(char **a)
{
	char buf[256];

//	getcwd(buf, 255);
	strcpy(buf, concmds_cwd);
	Con_Printf("%s\n", buf);
	return(0);
}

int ConCmds_Chdir(char **a)
{
	char buf[256];
	char *s, *s1;

//	Con_Printf("chdir %s\n", a[1]);
//	chdir(a[1]);

	s=a[1];
	s1=s;
	while(*s1 && (((*s1>='A')&&(*s1<='Z'))||((*s1>='a')&&(*s1<='z'))))
		s1++;
	
	if(*s=='/' || (*s1==':'))
	{
		strcpy(concmds_cwd, s);
	}else
	{
		strcpy(buf, concmds_cwd);
		s1=buf+strlen(buf);
		while((s1>buf) && (*(s1-1)=='/'))
			s1--;
		*s1++='/';
		strcpy(s1, s);

		strcpy(concmds_cwd, buf);
	}

	strcpy(buf, concmds_cwd);
//	getcwd(buf, 255);
	Con_Printf("%s\n", buf);

	return(0);
}

int ConCmds_Mkdir(char **a)
{
	char buf[256];

	vfmkdir(a[1], 0);

	return(0);
}

int ConCmds_Ls(char **a)
{
	char buf[256];
	VFDIR *dir;
	char *s;
	int i, j, k, l;

//	getcwd(buf, 255);
	strcpy(buf, concmds_cwd);
	dir=vfopendir(buf);

	if(!dir)return(-1);

	i=0;
	while(1)
	{
		s=vfreaddir(dir);
		if(!s)break;
		if(strlen(s)>i)i=strlen(s);
	}

	if(!i)return(0);
	vfrewinddir(dir);

	i++;
	j=100/i;

	k=0;
	while(1)
	{
		s=vfreaddir(dir);
		if(!s)break;

		Con_WriteString(s);
		l=i-strlen(s);
		while(l--)Con_WriteString(" ");

		k++;
		if(k>=j)
		{
			Con_WriteString("\n");
			k=0;
		}
	}
	if(k)Con_WriteString("\n");
	
	vfclosedir(dir);
}
#endif

int ConCmds_Exec(char **a)
{
	Con_ExecCmdBuf(a[1]);
	return(0);
}

int ConCmds_Set(char **a)
{
	ConCmds_CvarSet(a[1], a[2]);
	return(0);
}

int ConCmds_Alias(char **a)
{
	ConCmds_AliasSet(a[1], a[2]);
	return(0);
}

int ConCmds_Bind(char **a)
{
	char *s;
	int i;

	if(!a[1])
	{
		Con_Printf("bind <keyname> [command]\n");
		return(0);
	}

	if(!a[2])
	{
		i=UI_LookupKey(a[1]);
		if(i>0)
		{
			s=UI_BindingForKey(i);
			Con_Printf("%s\n", s);
			return(0);
		}
		Con_Printf("Bad key name %s\n", a[1]);
		return(0);
	}
	
	i=UI_BindKey(a[1], a[2]);
	if(i<0)
		Con_Printf("Bad key name %s\n", a[1]);
	return(0);
}

int ConCmds_Mount(char **a)
{
	char *s;
	int i;

	if(!a[1])
	{
		Con_Printf("mount <src> <dst> <fstype> <opts*>\n");
		vfDumpMounts();
		return(0);
	}

	vf_mount(a[1], a[2], a[3], a+4);
	return(0);
}

int ConCmds_LocalSpeak(char **a)
{
	char *s;
	int i;

	if(!a[1])
	{
		Con_Printf("localspeak <voice> <text>\n");
//		vfDumpMounts();
		return(0);
	}

	PDGL_Sound_Speak(a[1], a[2], 0, 1.0, 1.0);

//	vf_mount(a[1], a[2], a[3], a+4);
	return(0);
}

PDGL_API int ConCmds_Init()
{
	BTGE_SetConsoleHandler(ConCmds_Handler);
	BTGE_SetConsoleDoExec(Con_DoExecCmds);

	Con_AddModeComplete('$', ConCmds_Handler, ConCmds_Complete);

	ConCmds_Register("exec",	"Execute a console script", ConCmds_Exec);
	ConCmds_Register("set",		"Set a cvar", ConCmds_Set);
	ConCmds_Register("alias",	"Set an alias", ConCmds_Alias);
	ConCmds_Register("bind",	"Bind a key", ConCmds_Bind);

	ConCmds_Register("mount",	"Mount VFS", ConCmds_Mount);

	ConCmds_Register("localspeak",	"Local Speak", ConCmds_LocalSpeak);

//#ifndef MSVC
#if 1
	ConCmds_Register("cwd", "Print working directory", ConCmds_Cwd);
	ConCmds_Register("pwd", "Print working directory", ConCmds_Cwd);
	ConCmds_Register("chdir", "Change working directory", ConCmds_Chdir);
	ConCmds_Register("mkdir", "Make new directory", ConCmds_Mkdir);
	ConCmds_Register("cd", "Change working directory", ConCmds_Chdir);
	ConCmds_Register("md", "Make new directory", ConCmds_Mkdir);

	ConCmds_Register("ls", "List Directory Contents", ConCmds_Ls);
	ConCmds_Register("dir", "List Directory Contents", ConCmds_Ls);
#endif
}
