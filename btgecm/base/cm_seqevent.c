#include <btgecm.h>

BTGE_SequencedEvent *btgesv_seqevent_root=NULL;

BTGE_API BTGE_SequencedEvent *BTGE_AllocSequencedEvent(void)
{
	BTGE_SequencedEvent *tmp;
	tmp=gctalloc("btge_sequencedevent_t", sizeof(BTGE_SequencedEvent));
	return(tmp);
}

BTGE_API void BTGE_AddSequencedEventCmd(
	BTGE_SequencedEvent *sev, char *str)
{
	int i;

	if(!sev->cmds)
	{
		i=16;
		sev->cmds=gcalloc(i*sizeof(char *));
		sev->n_cmds=0;
		sev->m_cmds=i;
	}
	
	if((sev->n_cmds+1)>=sev->m_cmds)
	{
		i=sev->m_cmds; i=i+(i>>1);
		sev->cmds=gcrealloc(sev->cmds, i*sizeof(char *));
		sev->m_cmds=i;
	}
	
	i=sev->n_cmds++;
	sev->cmds[i]=dystrdup(str);
}

BTGE_API void BTGE_AddSequencedEventVar(
	BTGE_SequencedEvent *sev, char *var, char *val)
{
	int i;

	if(!sev->vars_var)
	{
		i=16;
		sev->vars_var=gcalloc(i*sizeof(char *));
		sev->vars_val=gcalloc(i*sizeof(char *));
		sev->n_vars=0;
		sev->m_vars=i;
	}
	
	if((sev->n_vars+1)>=sev->m_vars)
	{
		i=sev->m_vars; i=i+(i>>1);
		sev->vars_var=gcrealloc(sev->vars_var, i*sizeof(char *));
		sev->vars_val=gcrealloc(sev->vars_val, i*sizeof(char *));
		sev->m_vars=i;
	}
	
	i=sev->n_vars++;
	sev->vars_var[i]=dystrdup(var);
	sev->vars_val[i]=dystrdup(val);
}

BTGE_API BTGE_SequencedEvent *BTGE_LoadSequencedEvent(char *name)
{
	BTGE_SequencedEvent *tmp;
	VFILE *fd;
	void *p;
	char buf[256];
	char **a, *s;
	int i;

	if(!name || !(*name))
		return(NULL);

	fd=vffopen(name, "rt");
	if(!fd)return(NULL);

	tmp=BTGE_AllocSequencedEvent();

	while(!vfeof(fd))
	{
		vfgets(buf, 255, fd);
		BTGE_AddSequencedEventCmd(tmp, buf);
//		a=gcrsplit(buf);
//		if(!a[0])continue;
	}
	
	vfclose(fd);
	return(tmp);
}

BTGE_API BTGE_SequencedEvent *BTGE_ParseSequencedEvent(char *cmdbuf)
{
	BTGE_SequencedEvent *tmp;
	void *p;
//	char buf[256];
	char **a, *s, *cs, *cse;
	int i;

	tmp=BTGE_AllocSequencedEvent();

	cs=cmdbuf; cse=cs+strlen(cs);
	while(cs && (*cs) && (cs<cse))
	{
//		cs=gcrgets(buf, 255, cs);
		s=gcrgets(&cs);
		BTGE_AddSequencedEventCmd(tmp, s);
	}
	return(tmp);
}

BTGE_API void BTGE_StartSequencedEvent(BTGE_SequencedEvent *sev)
{
	sev->next=btgesv_seqevent_root;
	btgesv_seqevent_root=sev;
}

BTGE_API void BTGE_UnlinkSequencedEvent(BTGE_SequencedEvent *sev)
{
	BTGE_SequencedEvent *cur, *prv;
	
	cur=btgesv_seqevent_root; prv=NULL;
	while(cur)
	{
		if(cur==sev)break;
		prv=cur; cur=cur->next;
	}
	
	if(cur)
	{
		if(prv)
		{
			prv->next=cur->next;
		}else
		{
			btgesv_seqevent_root=cur->next;
		}
	}
}

BTGE_API void BTGE_FreeSequencedEvent(BTGE_SequencedEvent *sev)
{
	if(sev->cmds)
		gcfree(sev->cmds);
	gcfree(sev);
}

static int btge_seqevent_dummy() { return(0); }

BTGE_API int BTGE_SeqEvent_PlaySound(
	BTGE_SequencedEvent *sev, char *name)
{
	static int (*fcn)(char *name, int flags, float vol);
	
	if(!fcn)
	{
		fcn=dyllGetAddr("PDGL_Sound_PlaySample");
		if(!fcn)fcn=(void *)btge_seqevent_dummy;
	}
	return(fcn(name, 0, 1.0));
}

BTGE_API int BTGE_SeqEvent_PlaySpeak(
	BTGE_SequencedEvent *sev, char *voice, char *text)
{
	static int (*fcn)(char *voice, char *text,
		int flags, float vol, float scale);
	
	if(!fcn)
	{
		fcn=dyllGetAddr("PDGL_Sound_Speak");
		if(!fcn)fcn=(void *)btge_seqevent_dummy;
	}
	return(fcn(voice, text, 0, 1.0, 1.0));
}

BTGE_API int BTGE_SeqEvent_SoundStillPlayingP(
	BTGE_SequencedEvent *sev, int id)
{
	static int (*fcn)(int id);
	
	if(!fcn)
	{
		fcn=dyllGetAddr("PDGL_Sound_StillPlayingP");
		if(!fcn)fcn=(void *)btge_seqevent_dummy;
	}
	return(fcn(id));
}

BTGE_API BTGE_SequencedEvent *BTGE_LoadStartSequencedEvent(char *name)
{
	BTGE_SequencedEvent *tmp;
	tmp=BTGE_LoadSequencedEvent(name);
	if(!tmp)return(NULL);
	BTGE_StartSequencedEvent(tmp);
	tmp->flags|=BT_SEQEV_UNLINK;
	return(tmp);
}

BTGE_API char *BTGE_CmdSeq_LookupSeqVar(
	BTGE_SequencedEvent *sev, char *name)
{
	char *s;
	int i, j, k;
	
	for(i=0; i<sev->n_vars; i++)
		if(!strcmp(sev->vars_var[i], name))
			return(sev->vars_val[i]);
	if(sev->parent)
		{ return(BTGE_CmdSeq_LookupSeqVar(sev->parent, name)); }

	s=BTGE_CvarGet(name);
	if(s)return(s);
	return(NULL);
}

BTGE_API char *BTGE_CmdSeq_GetCmdReplaceVars(
	BTGE_SequencedEvent *sev)
{
	char tb[256], tbv[64];
	char **a, *s, *t, *s1, *t1, *sv;
	char *cmd, *buf;
	int i, j, bc;

	cmd=sev->cmds[0];
	j=sev->n_cmds--;
	for(i=0; i<j; i++)
		{ sev->cmds[i]=sev->cmds[i+1]; }
	
	s=cmd; t=tb;
	while(*s)
	{
		if((s[0]=='$') && (s[1]=='('))
		{
			s1=s+2; t1=tbv;
			while(*s1 && (*s1!=')'))
				*t1++=*s1++;
			*t1++=0;
			if(*s1==')')s1++;
			
			sv=BTGE_CmdSeq_LookupSeqVar(sev, tbv);
			if(sv)
			{
				s=s1;
				s1=sv;
				while(*s1)
					{ *t++=*s1++; }
				continue;
			}
		}
		*t++=*s++;
	}
	*t++=0;
	
	if(!strcmp(tb, cmd))
		return(cmd);
	return(gcrstrdup(tb));
}

BTGE_API void BTGE_CmdEvalBlock(
	BTGE_SequencedEvent *sev)
{
	char **a, *s, *t;
	char *cmd, *buf;
	int i, j, bc;
	
	buf=malloc(1<<16);
	t=buf;
	
	bc=1;
	while((sev->n_cmds>0) && (bc>0))
	{
//		cmd=sev->cmds[0];
//		j=sev->n_cmds--;
//		for(i=0; i<j; i++)
//			{ sev->cmds[i]=sev->cmds[i+1]; }
		cmd=BTGE_CmdSeq_GetCmdReplaceVars(sev);

		a=gcrsplit(cmd);
		if(!a[0]) { *t++='\n'; continue; }

		if((bc==1) && !strcmp(a[0], "}"))
			{ bc--; break; }

#if 0
		for(i=0; a[i]; i++)
		{
			if(!strcmp(a[i], "{"))bc++;
			if(!strcmp(a[i], "}"))bc--;
		}
#endif

#if 1
		for(i=0; cmd[i]; i++)
		{
			if(cmd[i]=='{')bc++;
			if(cmd[i]=='}')bc--;
		}
#endif

//		if(bc<=0)break;

		strcpy(t, cmd);
		t+=strlen(t);
		*t++='\n';
	}
	*t++=0;

	btScriptEval(buf);
	free(buf);
}

BTGE_API BTGE_SequencedEvent *BTGE_CmdAsyncBlock(
	BTGE_SequencedEvent *sev)
{
	BTGE_SequencedEvent *sev1;
	char **a, *s, *t;
	char *cmd, *buf;
	int i, j, bc;
	
	buf=malloc(1<<16);
	t=buf;
	
	bc=1;
	while((sev->n_cmds>0) && (bc>0))
	{
//		cmd=sev->cmds[0];
//		j=sev->n_cmds--;
//		for(i=0; i<j; i++)
//			{ sev->cmds[i]=sev->cmds[i+1]; }
		cmd=BTGE_CmdSeq_GetCmdReplaceVars(sev);

		a=gcrsplit(cmd);
		if(!a[0]) { *t++='\n'; continue; }

		if((bc==1) && !strcmp(a[0], "}"))
			{ bc--; break; }

#if 1
		for(i=0; cmd[i]; i++)
		{
			if(cmd[i]=='{')bc++;
			if(cmd[i]=='}')bc--;
		}
#endif

//		if(bc<=0)break;

		strcpy(t, cmd);
		t+=strlen(t);
		*t++='\n';
	}
	*t++=0;

	sev1=BTGE_ParseSequencedEvent(buf);
	BTGE_StartSequencedEvent(sev1);
	sev1->flags|=BT_SEQEV_UNLINK;

//	btScriptEval(buf);
	free(buf);
	return(sev1);
}

BTGE_API void BTGE_RunSequencedEventCmds(
	BTGE_SequencedEvent *sev)
{
	BTGE_SequencedEvent *sev1;
	char **a, *s;
	char *cmd;
	dyt t;
	int i, j;
	
	if(sev->wait>0)return;
	if(sev->wait_eval)
	{
		t=btScriptEval(sev->wait_eval);
		if(!dytruep(t))
			return;
		sev->wait_eval=NULL;
	}

	if(sev->wait_sound>0)
	{
		if(BTGE_SeqEvent_SoundStillPlayingP(sev, sev->wait_sound))
			return;
		sev->wait_sound=0;
	}
	
	while((sev->n_cmds>0) && (sev->wait<=0))
	{
//		cmd=sev->cmds[0];
//		j=sev->n_cmds--;
//		for(i=0; i<j; i++)
//			{ sev->cmds[i]=sev->cmds[i+1]; }
		cmd=BTGE_CmdSeq_GetCmdReplaceVars(sev);

		a=gcrsplit(cmd);
		if(!a[0])continue;

		if(!strcmp(a[0], "wait"))
		{
			if(a[1])
			{
				sev->wait+=atof(a[1]);
				continue;
			}
			sev->wait+=0.1;
			continue;
		}

		if(!strcmp(a[0], "wait_until_true"))
		{
			if(a[1])
			{
				sev->wait_eval=dystrdup(a[1]);
				continue;
			}
			//sev->wait+=0.1;
			continue;
		}

		if(!strcmp(a[0], "wait_sound_eval"))
		{
			t=btScriptEval(a[1]);
			if(!dyintp(t))
				{ sev->wait_sound=dyintv(t); }
			continue;
		}

		if(!strcmp(a[0], "wait_sound"))
		{
			sev->wait_sound=BTGE_SeqEvent_PlaySound(sev, a[1]);
			continue;
		}

		if(!strcmp(a[0], "wait_speak"))
		{
			sev->wait_sound=BTGE_SeqEvent_PlaySpeak(sev, a[1], a[2]);
			continue;
		}

		if(!strcmp(a[0], "exec"))
		{
			sev1=BTGE_LoadStartSequencedEvent(a[1]);
			sev1->parent=sev;
			sev->wait=999999;
			continue;
		}

		if(!strcmp(a[0], "exec_nb") || !strcmp(a[0], "exec_async"))
		{
			sev1=BTGE_LoadStartSequencedEvent(a[1]);
//			sev1->parent=sev;
//			sev->wait=999999;
			continue;
		}

		if(!strcmp(a[0], "async") && !strcmp(a[1], "{"))
		{
			BTGE_CmdAsyncBlock(sev);
			continue;
		}

		if(!strcmp(a[0], "eval") && !strcmp(a[1], "{"))
		{
			BTGE_CmdEvalBlock(sev);
			continue;
		}

		BTGE_ConsoleCmd(cmd);
	}
	
	if(sev->n_cmds<=0)
	{
		if(sev->parent)
		{
			sev->parent->wait=0;
		}
		
		if(sev->flags&BT_SEQEV_UNLINK)
		{
			BTGE_UnlinkSequencedEvent(sev);
			BTGE_FreeSequencedEvent(sev);
		}
	}
}

BTGE_API void BTGE_StepSequencedEvent(
	BTGE_SequencedEvent *sev, float dt)
{
	if(sev->wait>0)
		sev->wait-=dt;
}

BTGE_API void BTGE_RunStepSequencedEvent(
	BTGE_SequencedEvent *sev, float dt)
{
	if(sev->wait>0)
		sev->wait-=dt;
	BTGE_RunSequencedEventCmds(sev);
}

BTGE_API void BTGE_RunStepSequencedEvents(float dt)
{
	BTGE_SequencedEvent *cur, *nxt;

	cur=btgesv_seqevent_root;
	while(cur)
	{
		nxt=cur->next;
		BTGE_RunStepSequencedEvent(cur, dt);
		cur=nxt;
	}
}


BTGE_API int btSequenceExec(char *name)
{
	BTGE_SequencedEvent *tmp;

	tmp=BTGE_LoadStartSequencedEvent(name);
	if(!tmp)return(-1);
	return(0);
}

BTGE_API int btSequenceExecArgv(char *name, char **argv)
{
	char tb[256];
	BTGE_SequencedEvent *tmp;
	char *s, *t;
	int i, j, k;

	tmp=BTGE_LoadStartSequencedEvent(name);
	if(!tmp)return(-1);
	
	for(i=0; argv[i]; i++)
	{
		s=argv[i]; t=tb;
		while(*s && (*s!='='))
			{ *t++=*s++; }
		if(*s=='=')
		{
			*t++=0;
			BTGE_AddSequencedEventVar(tmp, tb, s+1);
		}
	
		sprintf(tb, "argv_%d", i);
		BTGE_AddSequencedEventVar(tmp, tb, argv[i]);
	}
	return(0);
}
