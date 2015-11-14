#include <bgbgc.h>
#include <bgbdy.h>

BGBGC_Namespace *bgbdyc_nsroot=NULL;

BGBGC_Namespace *BGBDYC_LookupNamespaceInner(
	BGBGC_Namespace *ns, char *name)
{
	BGBGC_Namespace *cur;

	cur=ns->first;
	while(cur)
	{
		if(!strcmp(cur->name, name))
			return(cur);
		cur=cur->next;
	}
	return(NULL);
}

BGBGC_Namespace *BGBDYC_GetNamespaceInner(
	BGBGC_Namespace *ns, char *name)
{
	BGBGC_Namespace *cur;

	cur=ns->first;
	while(cur)
	{
		if(!strcmp(cur->name, name))
			return(cur);
		cur=cur->next;
	}

	cur=gctalloc("_namespace_t", sizeof(BGBGC_Namespace));
	cur->name=dystrsym(name);

	cur->next=ns->first;
	cur->up=ns;
	ns->first=cur;

	return(cur);
}

BGBGC_Namespace *BGBDYC_LookupNamespace(BGBGC_Namespace *ns, char *name)
{
	BGBGC_Namespace *cur;
	char buf[256];
	char *s, *t, *suf;

	suf=NULL; s=name;
	while(*s && ((*s=='.') || (*s=='/')))s++;
	if(!(*s))return(ns);

	s=name; t=buf;
	while(*s && (*s!='.') && (*s!='/'))
		*t++=*s++;
	*t++=0;
	if(*s)suf=s;

	cur=BGBDYC_LookupNamespaceInner(ns, buf);
	if(!cur)return(NULL);
	if(suf)cur=BGBDYC_LookupNamespace(cur, suf);
	return(cur);
}

BGBGC_Namespace *BGBDYC_GetNamespace(BGBGC_Namespace *ns, char *name)
{
	BGBGC_Namespace *cur;
	char buf[256];
	char *s, *t, *suf;

	suf=NULL; s=name;
	while(*s && ((*s=='.') || (*s=='/')))s++;
	if(!(*s))return(ns);

	s=name; t=buf;
	while(*s && (*s!='.') && (*s!='/'))
		*t++=*s++;
	*t++=0;
	if(*s)suf=s;

	cur=BGBDYC_GetNamespaceInner(ns, buf);
	if(!cur)return(NULL);
	if(suf)cur=BGBDYC_GetNamespace(cur, suf);
	return(cur);
}

BGBGC_Namespace *BGBDYC_LookupNamespaceGlobal(char *name)
{
	if(!bgbdyc_nsroot)return(NULL);
	return(BGBDYC_LookupNamespace(bgbdyc_nsroot, name));
}

BGBGC_Namespace *BGBDYC_GetNamespaceGlobal(char *name)
{
	if(!bgbdyc_nsroot)
	{
		bgbdyc_nsroot=gctalloc("_namespace_t",
			sizeof(BGBGC_Namespace));
	}

	return(BGBDYC_GetNamespace(bgbdyc_nsroot, name));
}

void BGBDYC_LinkClassNamespace(BGBGC_ClassInfo *inf)
{
	BGBGC_Namespace *ns;
	char *s, *s0, *qn;

	qn=inf->qname;
	s=qn+strlen(qn);
	while((s>qn) && (*s!='.'))s--;

	s0=gcrstrdup(qn); s0[s-qn]=0;
	ns=BGBDYC_GetNamespaceGlobal(s0);
	inf->nsnext=ns->info;
	ns->info=inf;
}

BGBGC_NamespaceSlot *BGBDYC_LookupNamespaceSlot(
	BGBGC_Namespace *ns, char *name)
{
	BGBGC_NamespaceSlot *cur;

	cur=ns->slot;
	while(cur)
	{
		if(!strcmp(cur->name, name))
			return(cur);
		cur=cur->next;
	}

	return(NULL);
}

BGBGC_NamespaceSlot *BGBDYC_GetNamespaceSlot(
	BGBGC_Namespace *ns, char *name, char *sig)
{
	BGBGC_NamespaceSlot *cur;
	int i, j, k;

	cur=ns->slot;
	while(cur)
	{
		if(!strcmp(cur->name, name))
			return(cur);
		cur=cur->next;
	}

	cur=gctalloc("_namespace_slot_t", sizeof(BGBGC_NamespaceSlot));
	cur->name=dystrsym(name);
	cur->sig=dystrsym(sig);

	dyllSigSizeAlign(sig, &j, &k);
	if(j<=(2*sizeof(void *)))cur->val=&(cur->val_i);
		else cur->val=gcalloc(j);

	cur->next=ns->slot;
	cur->ns=ns;
	ns->slot=cur;

	return(cur);
}

BGBGC_NamespaceSlot *BGBDYC_LookupNamespaceFunc(
	BGBGC_Namespace *ns, char *name, char *sig)
{
	BGBGC_NamespaceSlot *cur;

	cur=ns->func;
	while(cur)
	{
		if(!strcmp(cur->name, name) &&
			!strcmp(cur->sig, sig))
				return(cur);
		cur=cur->next;
	}

	return(NULL);
}

BGBGC_NamespaceSlot *BGBDYC_GetNamespaceFunc(
	BGBGC_Namespace *ns, char *name, char *sig)
{
	BGBGC_NamespaceSlot *cur;
	int i, j, k;

	cur=ns->func;
	while(cur)
	{
		if(!strcmp(cur->name, name) &&
			!strcmp(cur->sig, sig))
				return(cur);
		cur=cur->next;
	}

	cur=gctalloc("_namespace_slot_t", sizeof(BGBGC_NamespaceSlot));
	cur->name=dystrsym(name);
	cur->sig=dystrsym(sig);

	cur->next=ns->func;
	cur->ns=ns;
	ns->func=cur;

	return(cur);
}

