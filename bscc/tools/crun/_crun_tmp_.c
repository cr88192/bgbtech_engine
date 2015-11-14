#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#ifdef _WIN32
#include <windows.h>
#endif

#ifndef BYTE_T
#define BYTE_T
typedef unsigned char byte;
#endif

#ifndef S64_T
#define S64_T
typedef unsigned char s64;
#endif

#ifndef CRUN_TIME_T
#define CRUN_TIME_T
typedef long long crun_time_t;
#endif

#ifndef CRUN_DEPNODE_T
#define CRUN_DEPNODE_T
typedef struct crun_depnode_s crun_depnode;
struct crun_depnode_s {
crun_depnode *next;
char *name;
char **deps;
char **cmds;
};
#endif

#ifndef CRUN_DEPVAR_T
#define CRUN_DEPVAR_T
typedef struct crun_depvar_s crun_depvar;
struct crun_depvar_s {
crun_depvar *next;
char *name;
char *value;
char *(*genvalue_f)(crun_depvar *self);
};
#endif

#ifndef CRUN_DEPCMD_T
#define CRUN_DEPCMD_T
typedef struct crun_depcmd_s crun_depcmd;
struct crun_depcmd_s {
crun_depcmd *next;
char *name;
int (*handle_f)(crun_depcmd *self, char **args);
};
#endif

int crun_sputs(char **rt, char *str)
{
	char *s, *t;

	s=str; t=*rt;

	while(*s)
	{
		if(s[0]=='\r')
		{
			if(s[1]=='\n')
				{ *t++=*s++; *t++=*s++; continue; }
			*t++=*s++; continue;
		}
		if(s[0]=='\n')
			{ s++; *t++='\r'; *t++='\n'; continue; }
		*t++=*s++;
	}
	*t=0;

	*rt=t;
	return(0);
}

int crun_printsf(char **rt, char *str, ...)
{
	char tb[4096];
	va_list lst;

	va_start(lst, str);
	vsprintf(tb, str, lst);
	va_end(lst);
	crun_sputs(rt, tb);
	return(0);
}

int crun_system(char *str)
{
	int i;
	i=system(str);
	return(i);
}

int crun_call(char *app, char **args)
{
	char tbuf[4096];
	char *t;
	int i;

	t=tbuf;
	crun_sputs(&t, app);
	for(i=0; args[i]; i++)
		crun_printsf(&t, " \"%s\"", args[i]);
	*t++=0;

	i=crun_system(tbuf);
	return(i);
}

int crun_callmake(char *dir, char *mkfile, char *targ)
{
	char *args[64];
	int i, n;

	n=0;
	if(dir) { args[n++]="-C"; args[n++]=dir; }
	if(mkfile) { args[n++]="-f"; args[n++]=mkfile; }
	if(targ) { args[n++]=targ; }
	args[n]=NULL;

	i=crun_call("make", args);
	return(i);
}

int crun_dechexchar(int i)
{
	if((i>='0') && (i<='9')) return(i-'0');
	if((i>='A') && (i<='F')) return(i-'A'+10);
	if((i>='a') && (i<='f')) return(i-'a'+10);
	return(-1);
}

int crun_dechexchar1(char *s)
	{ return(crun_dechexchar(*s)); }
int crun_dechexchar2(char *s)
	{ return((crun_dechexchar(s[0])<<4)+crun_dechexchar(s[1])); }


char *crun_ralloc_buffer=NULL;
int crun_ralloc_pos=0;

void *crun_ralloc(int sz)
{
	char *s;

	if(!crun_ralloc_buffer)crun_ralloc_buffer=(char *)malloc(1<<20);
	if((crun_ralloc_pos+sz)>=(1<<20))crun_ralloc_pos=0;

	s=crun_ralloc_buffer+crun_ralloc_pos;
	crun_ralloc_pos+=sz;
	return((void *)s);
}

char *crun_rstrdup(char *s)
{
	char *t;

	t=(char *)crun_ralloc(strlen(s)+1);
	strcpy(t, s);
	return(t);
}

char **crun_rsplit(char *s)
{
	char **a, *t;
	int i, j;

	a=(char **)crun_ralloc(256*sizeof(char *));
	i=0;
	while(*s)
	{
		while(*s && (*s<=' '))s++;
		if(!*s)break;
		if(*s=='#')break;
		if(*s==';')break;
		if((s[0]=='/') && (s[1]=='/'))
			break;

		t=(char *)crun_ralloc(256);
		a[i++]=t;

		if(*s=='"')
		{
			s++;
			while(*s && (*s!='"'))
			{
				if(*s=='\\')
				{
					s++; j=*s++;
					switch(j)
					{
					case 'r': *t++='\r'; break;
					case 'n': *t++='\n'; break;
					case 't': *t++='\t'; break;
					case '\\': *t++='\\'; break;
					case 'x':
						*t++=crun_dechexchar2(s);
						s+=2;
						break;
					default: *t++=j; break;
					}
					continue;
				}
				*t++=*s++;
			}
			if(*s=='"')s++;
			*t++=0; *t++='\"'; *t++=0;
		}else
		{
			while(*s && (*s>' '))*t++=*s++;
			*t++=0; *t++=0; *t++=0;
		}
	}
	a[i]=NULL;

	return(a);
}

char **crun_split(char *s)
{
	char tb[4096];
	char **a, *t;
	int i, j, m;

	a=(char **)malloc(64*sizeof(char *)); m=64;
	i=0;
	while(*s)
	{
		while(*s && (*s<=' '))s++;
		if(!*s)break;
		if(*s=='#')break;
		if(*s==';')break;
		if((s[0]=='/') && (s[1]=='/'))
			break;

		t=tb;
		if(*s=='"')
		{
			s++;
			while(*s && (*s!='"'))
			{
				if(*s=='\\')
				{
					s++; j=*s++;
					switch(j)
					{
					case 'r': *t++='\r'; break;
					case 'n': *t++='\n'; break;
					case 't': *t++='\t'; break;
					case '\\': *t++='\\'; break;
					case 'x':
						*t++=crun_dechexchar2(s);
						s+=2;
						break;
					default: *t++=j; break;
					}
					continue;
				}
				*t++=*s++;
			}
			if(*s=='"')s++;
			*t++=0; *t++='\"'; *t++=0;
		}else
		{
			while(*s && (*s>' '))*t++=*s++;
			*t++=0; *t++=0; *t++=0;
		}

		if(i>=m) { m+=m>>1; a=(char **)realloc(a, m*sizeof(char *)); }

		j=t-tb;
		t=(char *)malloc(j);
		memcpy(t, tb, j);
		a[i++]=t;
	}
	a[i]=NULL;

	return(a);
}

int crun_freestrs(char **a)
{
	int i;
	for(i=0; a[i]; i++)free(a[i]);
	free(a);
	return(0);
}

char **crun_dupstrs(char **a)
{
	char **b;
	int i;

	if(!a)return(NULL);

	for(i=0; a[i]; i++);
	b=(char **)malloc((i+1)*sizeof(char *));
	for(i=0; a[i]; i++)
		b[i]=strdup(a[i]);
	b[i]=NULL;
	return(b);
}


int crun_bufferhash(void *buf, int sz)
{
	unsigned char *cs, *ce;
	int hi;

	cs=(unsigned char *)buf; ce=cs+sz;
	hi=0; while(cs<ce)hi=hi*251+(*cs++);
	return(hi&0x3FFFFFFF);
}

int crun_filehash(char *name)
{
	FILE *fd;
	void *buf;
	int i, sz;

	fd=fopen(name, "rb");
	if(!fd)return(-1);
	fseek(fd, 0, 2);
	sz=ftell(fd);
	fseek(fd, 0, 0);

	buf=malloc(sz);
	fread(buf, 1, sz, fd);
	fclose(fd);

	i=crun_bufferhash(buf, sz);
	free(buf);
	return(i);
}

crun_time_t crun_futuretime()
	{ return(1LL<<62); }

#ifndef _WIN32
crun_time_t crun_filetime(char *name)
	{ return(-1); }
#endif

#ifdef _WIN32
crun_time_t crun_filetime(char *name)
{
	char buf[1024];
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	crun_time_t lt;
	char *s, *t;
	int i;

	strcpy(buf, name);

	s=buf; t=buf;
	while(*s)
	{
		while((s[0]=='/') && (s[1]=='/'))s++;
		if(*s=='/')*s='\\';
		*t++=*s++;
	}

	hFind=FindFirstFile(buf, &FindFileData);
	if(hFind==INVALID_HANDLE_VALUE) 
		return(-1);
	FindClose(hFind);

	lt=(((crun_time_t)FindFileData.ftLastWriteTime.dwHighDateTime)<<32)+
		((crun_time_t)FindFileData.ftLastWriteTime.dwLowDateTime);
	return(lt);
}
#endif

/*
   Stuff for vars and dependency checking.
 */

crun_depvar *crun_depvar_root=NULL;
crun_depcmd *crun_depcmd_root=NULL;
crun_depnode *crun_depnode_root=NULL;

crun_depvar *crun_lookupdepvar(char *name)
{
	crun_depvar *cur;

	cur=crun_depvar_root;
	while(cur)
	{
		if(!strcmp(cur->name, name))
			return(cur);
		cur=cur->next;
	}
	return(cur);
}

crun_depvar *crun_getdepvar(char *name)
{
	crun_depvar *tmp;

	tmp=crun_lookupdepvar(name);
	if(tmp)return(tmp);

	tmp=(crun_depvar *)malloc(sizeof(crun_depvar));
	memset(tmp, 0, sizeof(crun_depvar));
	tmp->name=strdup(name);
	return(tmp);
}

crun_depvar *crun_setdepvar(char *name, char *value)
{
	crun_depvar *tmp;

	tmp=crun_getdepvar(name);
	if(tmp->value) { free(tmp->value); tmp->value=NULL; }
	if(value)tmp->value=strdup(value);
	return(tmp);
}

char *crun_getvar(char *name)
{
	crun_depvar *tmp;
	tmp=crun_lookupdepvar(name);
	if(!tmp)return(NULL);
	if(tmp->genvalue_f)
		return(tmp->genvalue_f(tmp));
	return(tmp->value);
}

void crun_setvar(char *name, char *value)
	{ crun_setdepvar(name, value); }

char *crun_expandvars(char *str)
{
	char buf[4096], tb[4096], tb1[256];
	char *s, *t, *t1;

	/* check if no patterns to expand */
	s=str;
	while(*s)
	{
		if((s[0]=='$') && (s[1]=='('))break;
		s++;
	}
	if(!*s)return(str);

	strcpy(buf, str);
	while(1)
	{
		s=buf;
		while(*s)
		{
			if((s[0]=='$') && (s[1]=='('))break;
			s++;
		}
		if(!*s)break;

		s=buf; t=tb;
		while(*s)
		{
			if((s[0]=='$') && (s[1]=='('))
			{
				s+=2; t1=tb1;
				while(*s && (*s!=')'))
					*t1++=*s++;
				*t1++=0; if(*s==')')s++;

				t1=crun_getvar(tb1);
				if(t1) { strcpy(t, t1); t+=strlen(t); }
				continue;
			}
			*t++=*s++;
		}
		*t++=0;

		strcpy(buf, tb);
	}

	t=crun_rstrdup(buf);
	return(t);
}

/*
   Command handling
 */

crun_depcmd *crun_lookupdepcmd(char *name)
{
	crun_depcmd *cur;

	cur=crun_depcmd_root;
	while(cur)
	{
		if(!strcmp(cur->name, name))
			return(cur);
		cur=cur->next;
	}
	return(cur);
}

crun_depcmd *crun_getdepcmd(char *name)
{
	crun_depcmd *tmp;

	tmp=crun_lookupdepcmd(name);
	if(tmp)return(tmp);

	tmp=(crun_depcmd *)malloc(sizeof(crun_depcmd));
	memset(tmp, 0, sizeof(crun_depcmd));
	tmp->name=strdup(name);
	return(tmp);
}

crun_depcmd *crun_setdepcmd(char *name,
	int (*fcn)(crun_depcmd *self, char **args))
{
	crun_depcmd *tmp;

	tmp=crun_getdepcmd(name);
	tmp->handle_f=fcn;
	return(tmp);
}

int crun_runcmd(char *str)
{
	crun_depcmd *tmp;

	char *s;
	char **a;
	int i;

	s=crun_expandvars(str);
	a=crun_rsplit(s);

	if(!a[0])return(0);
	if(!*(a[0]))return(0);

	tmp=crun_lookupdepcmd(a[0]);
	if(tmp && tmp->handle_f)
	{
		i=tmp->handle_f(tmp, a);
		return(i);
	}

	i=crun_system(s);
	return(i);
}

/*
   Dependency checking.
 */

static crun_depnode *depstk[256];
static int depstkpos=0;

crun_depnode *crun_lookupdep(char *name)
{
	crun_depnode *cur;

	cur=crun_depnode_root;
	while(cur)
	{
		if(!strcmp(cur->name, name))
			return(cur);
		cur=cur->next;
	}
	return(cur);
}

crun_depnode *crun_newdep(char *name, char **deps, char **cmds)
{
	crun_depnode *tmp;

	tmp=crun_lookupdep(name);
	if(tmp)return(tmp);

	tmp=(crun_depnode *)malloc(sizeof(crun_depnode));
	memset(tmp, 0, sizeof(crun_depnode));

	tmp->name=strdup(name);
	tmp->deps=crun_dupstrs(deps);
	tmp->cmds=crun_dupstrs(cmds);

	tmp->next=crun_depnode_root;
	crun_depnode_root=tmp;

	return(tmp);
}

/* -1=missing deps, >0=dep-time */
crun_time_t crun_deptime(char *name)
{
	crun_depnode *tmp;
	crun_time_t ct, tt;
	int i, j, mrk;

	tmp=crun_lookupdep(name);
	if(!tmp)return(-1);

	/* no deps or cmds == "FORCE" */
	if(!tmp->deps && !tmp->cmds)
		return(crun_futuretime());

	ct=crun_filetime(tmp->name);

	if(tmp->deps)
	{
		/* circular dependency check */
		for(i=0; i<depstkpos; i++)
			if(depstk[i]==tmp)
				return(0);
		mrk=depstkpos++;
		depstk[mrk]=tmp;

		for(i=0; tmp->deps[i]; i++)
		{
			tt=crun_deptime(tmp->deps[i]);
			if(tt<0) { ct=tt; break; }
			if(tt>ct)ct=tt;
		}
		depstkpos=mrk;
		return(ct);
	}

	return(ct);
}

/* -1=missing deps, 0=up-to-date, 1=needs rebuild */
int crun_checkdep(char *name)
{
	crun_depnode *tmp;
	crun_time_t ct, tt;
	int i, j, k, ret, mrk;

	tmp=crun_lookupdep(name);
	if(!tmp)return(-1);

	/* no deps or cmds == "FORCE" */
	if((!tmp->deps) && (!tmp->cmds))
		return(1);

	ret=0;
	if(tmp->deps)
	{
		/* circular dependency check */
		for(i=0; i<depstkpos; i++)
			if(depstk[i]==tmp)
				return(0);
		mrk=depstkpos++;
		depstk[mrk]=tmp;

		for(i=0; tmp->deps[i]; i++)
		{
			j=crun_checkdep(tmp->deps[i]);
			if(j<0) { ret=j; break; }
			if(j>0)ret|=j;
		}

		if(ret) { depstkpos=mrk; return(ret); }

		ct=crun_filetime(tmp->name);

		for(i=0; tmp->deps[i]; i++)
		{
			tt=crun_deptime(tmp->deps[i]);
			if(tt<0) { ret=(int)tt; break; }
			if(tt>ct) ret|=1;
		}

		if(ret) { depstkpos=mrk; return(ret); }
		depstkpos=mrk;
	}

	return(0);
}

int crun_rundeps(char *name)
{
	crun_depnode *tmp;
	int i, j, ret, mrk;

	tmp=crun_lookupdep(name);
	if(!tmp)return(-1);

	if(!tmp->deps && !tmp->cmds)
		return(0);

	i=crun_checkdep(name);
	if(!i)return(0);	/* nothing to do */
	if(i<0)return(i);	/* problem */

	ret=0;
	if(tmp->deps)
	{
		/* circular dependency check */
		for(i=0; i<depstkpos; i++)
			if(depstk[i]==tmp)
				return(0);
		mrk=depstkpos++;
		depstk[mrk]=tmp;

		for(i=0; tmp->deps[i]; i++)
		{
			j=crun_rundeps(tmp->deps[i]);
			if(j<0) { ret=j; break; }
		}
		depstkpos=mrk;
	}

	if(ret<0)return(ret);

	for(i=0; tmp->cmds[i]; i++)
	{
		j=crun_runcmd(tmp->cmds[i]);
		if(j) { ret=-1; break; }
	}

	return(ret);
}


/* End Prologue */


#include <stdio.h>

int main()
{
	printf("OK\n");
	return(0);
}
