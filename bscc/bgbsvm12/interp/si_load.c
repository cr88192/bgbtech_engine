#include <bgbsvm.h>

#define bsSet(rf, vl)		rf=vl

extern char *bsvm_ops_strs[];
extern char *bsvm_ops_args[];

extern char *bsvm_bssrc_clump[];

dytf bsvm_toplevel;
int bsvm_opcnt;

char *bsvm_scriptpath[256];		//paths to search for scripts
int bsvm_n_scriptpath;

char *bsvm_scriptname[1024];	//loaded scripts (extern import/...)
int bsvm_n_scriptname;

static int hexval(int i)
{
	if((i>='0') && (i<='9'))return(i-'0');
	if((i>='A') && (i<='F'))return(i-'A'+10);
	if((i>='a') && (i<='f'))return(i-'a'+10);
	return(-1);
}

static int hexbyte(char *s)
{
	int i, j;
	i=hexval(s[0]);
	j=hexval(s[1]);
	if((i<0) || (j<0))return(-1);
	return((i<<4)+j);
}

static int hexword(char *s)
{
	int i, j;
	i=hexbyte(s+0);
	j=hexbyte(s+2);
	if((i<0) || (j<0))return(-1);
	return((i<<8)+j);
}

static int hexdword(char *s)
{
	int i, j;
	i=hexword(s+0);
	j=hexword(s+4);
	if((i<0) || (j<0))return(-1);
	return((i<<16)+j);
}

BGBSVM_API char *BSVM_Clump_ReadInClump(char **strs)
{
	static char *tmpbuf=NULL;
	static int sztmpbuf=0;
	char **a;
	char *s, *t, *te;
	int i, j;
	
	if(!tmpbuf)
	{
		tmpbuf=malloc(4096);
		sztmpbuf=4096;
	}

	t=tmpbuf;
	te=tmpbuf+sztmpbuf;
	
	a=strs;
	while(*a)
	{
		s=*a;
		if((s[0]=='$') && (s[1]!='$'))
		{
			if(!strcmp(s, "$CLUMPEOF\n"))
				break;
			a++;
			continue;
		}

		while(*s)
		{
			if((t+1)>=te)
			{
				i=t-tmpbuf;
				sztmpbuf+=sztmpbuf>>1;
				tmpbuf=realloc(tmpbuf, sztmpbuf);
				t=tmpbuf+i;
				te=tmpbuf+sztmpbuf;
			}
			
			if(*s=='$')
			{
				s++;
				if(*s=='$')
					{ *t++=*s++; continue; }
				if(*s=='x')
					{ s++; *t++=hexbyte(s); s+=2; continue; }
				*t++=*s++;
				continue;
			}
			
			*t++=*s++;
		}
		*t=0;
		a++;
	}
	
	return(tmpbuf);
}

BGBSVM_API char **BSVM_Clump_LookupClump(char **strs, char *name)
{
	char **a;
	char *s, *s0, *s1;
	int pfsz;

	pfsz=strlen("$CLUMP ");
	a=strs;
	while(*a)
	{
		s=*a;
		if((*s=='$') && !strncmp(s, "$CLUMP ", pfsz))
		{
			s0=s+pfsz; s1=name;
			while(s0 && s1 && (*s0!='\n'))
			{
				if(*s0 != *s1)
					break;
				s0++; s1++;
			}
			
			if(!(*s1) && (!(*s0) || (*s0=='\n')))
				return(a);
		}
		a++;
	}
	return(a);
}

BGBSVM_API char *BSVM_Clump_BufferIn(char **strs, char *name)
{
	char **a;
	char *s;
	
	a=BSVM_Clump_LookupClump(strs, name);
	if(!a || !*a)
		return(NULL);
	s=BSVM_Clump_ReadInClump(a+1);
	return(s);
}

BGBSVM_API int BSVM_AddScriptPath(char *name)
{
	int i;

	for(i=0; i<bsvm_n_scriptpath; i++)
		if(!strcmp(bsvm_scriptpath[i], name))
			return(i);
			
	i=bsvm_n_scriptpath++;
	bsvm_scriptpath[i]=dystrdup(name);
	return(i);
}

BGBSVM_API byte *BSVM_ScriptBufferIn(char *name, int *rsz)
{
	char tb[256];
	VFILE *fd;
	byte *buf;
	char *s;
	int i, sz;

	if(!name)return(NULL);
	fd=vffopen(name, "rb");
	
	if(!fd)
	{
		for(i=0; i<bsvm_n_scriptpath; i++)
		{
			sprintf(tb, "%s/%s", bsvm_scriptpath[i], name);
			fd=vffopen(tb, "rb");
			if(fd)break;
		}
	}
	
	if(!fd)
	{
		fd=VFILE_ExWAD_OpenAny(name, "rb");
	}
	
	if(!fd)
	{
		s=BSVM_Clump_BufferIn(bsvm_bssrc_clump, name);
		if(s)
		{
			sz=strlen(s);
			buf=gcalloc(sz+16);
			strcpy(buf, s);
			if(rsz)*rsz=sz;
			return(buf);
		}

		return(NULL);
	}
	vfseek(fd, 0, 2);
	sz=vftell(fd);

	buf=gcalloc(sz+16);
	vfseek(fd, 0, 0);
	vfread(buf, 1, sz, fd);
	vfclose(fd);
	
	if(rsz)*rsz=sz;
	return(buf);
}

BGBSVM_API dyt BSVM_EvalExprsObj(dyt exp, dyt obj)
{
	return(BSVM_EvalExprsObjFlags(exp, obj, 0));
}

BGBSVM_API dyt BSVM_EvalExprsObjFlags(dyt exp, dyt obj, int vmgfl)
{
	static BSVM_SVMState *ctx2;

	BSVM_SVMCState *cctx;
	BSVM_SVMState *ctx;
	char *s;
	dyt t, l, f;
	int i;

	BSVM_Init();

	cctx=gcalloc(sizeof(BSVM_SVMCState));

	cctx->lvl=-1;
	cctx->vmgflags=vmgfl;
	f=BSVMC_CompileBlock(cctx, NULL, NULL, exp);

	gcfree(cctx);

	ctx=BSVM_NewState();
	ctx->flowstackpos=-1;
	ctx->top=bsvm_toplevel;
	ctx->vmgflags|=vmgfl;

	if(vmgfl&BSVM_VMGFL_UNTRUSTED)
	{
		if(vmgfl&BSVM_VMGFL_FROMCLIENT)
		{
			ctx->access=BGBDY_ACCESS_GID_CLIENT|
				BGBDY_ACCESS_UID_CLIENT|
				BGBDY_ACCESS_DEFAULT;
		}else if(vmgfl&BSVM_VMGFL_FROMSERVER)
		{
			ctx->access=BGBDY_ACCESS_GID_SERVER|
				BGBDY_ACCESS_UID_SERVER|
				BGBDY_ACCESS_DEFAULT;
		}else
		{
			ctx->access=BGBDY_ACCESS_GID_GUEST|
				BGBDY_ACCESS_UID_GUEST|
				BGBDY_ACCESS_DEFAULT;
		}
	}else
	{
//		ctx->access=BGBDY_ACCESS_GID_ROOT|
//			BGBDY_ACCESS_UID_ROOT|
//			BGBDY_ACCESS_DEFAULT;
		ctx->access=BGBDY_ACCESS_GID_ROOT|
			BGBDY_ACCESS_UID_ROOT|
			BGBDY_ACCESS_ROOTDEFAULT;
	}

	ctx2=ctx;

	BSVM_PushList(ctx, DYTF_NULL);
	BSVM_CallObj(ctx, dytfWrap(obj), dytfWrap(f));
	i=BSVM_Run((dyt)ctx);
	t=dytfUnwrap(BSVM_Pop(ctx));

	BSVM_DestroyState(ctx);

//	BSVM_GC_CheckRef(f);
//	BSVM_GC_CheckRef(ct);

	return(t);
}

BGBSVM_API dyt BSVM_EvalStringS(char *str)
{
	BSVM_SVMCState *cctx;
	BSVM_SVMState *ctx;
	dyt t, ct, l;
	int i;

	return(NULL);

//	l=BSVM_ParseS_MultiFromString(str);

	l=dysParseMultiFromString(str);
//	t=BSVM_EvalExprsObj(l, bsvm_toplevel);
	t=BSVM_EvalExprsObj(l, NULL);
//	BSVM_GC_CheckRef(l);

	return(t);
}

BGBSVM_API dyt BSVM_EvalString(char *str)
{
	return(BSVM_EvalStringFlags(str, 0));
}

BGBSVM_API dyt BSVM_EvalStringFlags(char *str, int vmgfl)
{
	BSVM_SVMCState *cctx;
	BSVM_SVMState *ctx;
	char *s;
	dyt t, l;
	int i;

	BSVM_Init();

	BSVMP_SetLinenum("<eval-str>", str, 1);

	s=str;
//	l=BSVMP_Block(&s);
	l=dytfUnwrap(BSVMP_Block(&s));
//	BSVM_TY_PrintLN(l);
//	dyPrint(l);
//	dyPrintf("\n");

//	t=BSVM_EvalExprsObj(l, bsvm_toplevel);
	t=BSVM_EvalExprsObjFlags(l, NULL, vmgfl);
//	BSVM_GC_CheckRef(l);

	return(t);
}

BGBSVM_API dyt BSVM_EvalStringObjName(char *str, dyt obj, char *name)
{
	return(BSVM_EvalStringObjNameFlags(str, obj, name, 0));
}

BGBSVM_API dyt BSVM_EvalStringObjNameFlags(char *str, dyt obj,
	char *name, int vmgfl)
{
	BSVM_SVMCState *cctx;
	BSVM_SVMState *ctx;
	char *s;
	dyt t, ct, l;
	int i;

	BSVM_Init();

//	if(obj==NULL)
//		obj=bsvm_toplevel;

	BSVMP_SetLinenum(name, str, 1);

	s=str;
//	l=BSVMP_Block(&s);
	l=dytfUnwrap(BSVMP_Block(&s));
//	BSVM_TY_PrintLN(l);
//	dyPrint(l);

	t=BSVM_EvalExprsObjFlags(l, obj, vmgfl);
//	BSVM_GC_CheckRef(l);

	return(t);
}

BGBSVM_API dyt BSVM_EvalStringObj(char *str, dyt obj)
{
	return(BSVM_EvalStringObjName(str, obj, "<eval-str>"));
}

BGBSVM_API dyt BSVM_EvalStringObjFlags(char *str, dyt obj, int vmgfl)
{
	return(BSVM_EvalStringObjNameFlags(str, obj, "<eval-str>", vmgfl));
}


BGBSVM_API dyt BSVM_EvalGastSBXE(byte *data, int sz, dyt obj)
{
	static BSVM_SVMState *ctx2;

	BSVM_SVMCState *cctx;
	BSVM_SVMState *ctx;
	char *s;
	dyt t, l, f;
	int i;

	BSVM_Init();

	cctx=gcalloc(sizeof(BSVM_SVMCState));

	cctx->lvl=-1;
//	f=BSVMC_CompileBlock(cctx, NULL, NULL, exp);
//	f=BSVMC_CompileGastSBXE(cctx, data, sz);

	gcfree(cctx);


	ctx=BSVM_NewState();
	ctx->flowstackpos=-1;
	ctx->top=bsvm_toplevel;

	ctx2=ctx;
	f=NULL;

	BSVM_PushList(ctx, DYTF_NULL);
	BSVM_CallObj(ctx, dytfWrap(obj), dytfWrap(f));
	i=BSVM_Run((dyt)ctx);
	t=dytfUnwrap(BSVM_Pop(ctx));

	BSVM_DestroyState(ctx);

//	BSVM_GC_CheckRef(f);
//	BSVM_GC_CheckRef(ct);

	return(t);
}

BGBSVM_API dyt BSVM_EvalGast(dyxNode *tn, dyt obj)
{
	static BSVM_SVMState *ctx2;

	BSVM_SVMCState *cctx;
	BSVM_SVMState *ctx;
	char *s;
	dyt t, ct, l, f;
	int i;

	BSVM_Init();

	cctx=gcalloc(sizeof(BSVM_SVMCState));

	cctx->lvl=-1;
//	f=BSVMC_CompileBlock(cctx, NULL, NULL, exp);
//	f=BSVMC_CompileGast(cctx, tn);

	gcfree(cctx);


	ctx=BSVM_NewState();
	ctx->flowstackpos=-1;
	ctx->top=bsvm_toplevel;

	ctx2=ctx;
	f=NULL;

	BSVM_PushList(ctx, DYTF_NULL);
	BSVM_CallObj(ctx, dytfWrap(obj), dytfWrap(f));
	i=BSVM_Run((dyt)ctx);
	t=dytfUnwrap(BSVM_Pop(ctx));

	BSVM_DestroyState(ctx);

//	BSVM_GC_CheckRef(f);
//	BSVM_GC_CheckRef(ct);

	return(t);
}

BGBSVM_API dyt BSVM_CallFuncObjCtx(
	BSVM_SVMState *ctx, dyt obj, dyt fcn, dyt args)
{
	char *s;
	dyt t, l, f;
	int i;

	BSVM_Init();

//	if(obj==NULL)
//		obj=bsvm_toplevel;

	if(dysymbolp(fcn))
	{
		fcn=dytfUnwrap(BSVM_Lookup(ctx, dytfWrap(fcn)));
		if((fcn==NULL) || (fcn==UNDEFINED))
			return(UNDEFINED);
	}

//	dyPrintf("call ");
//	BSVM_TY_PrintLN(dycons(fcn, args));

	BSVM_PushList(ctx, dytfWrap(args));
	BSVM_CallObj(ctx, dytfWrap(obj), dytfWrap(fcn));
	i=BSVM_Run((dyt)ctx);
	t=dytfUnwrap(BSVM_Pop(ctx));

	return(t);
}

BGBSVM_API dyt BSVM_CallFuncObj(dyt obj, dyt fcn, dyt args)
{
	BSVM_SVMState *ctx;
	char *s;
	dyt t, ct, l, f;
	int i;

	BSVM_Init();

//	if(obj==NULL)
//		obj=bsvm_toplevel;

	if(dysymbolp(fcn))
	{
		if(obj)
		{
			fcn=dyGet(obj, (char *)fcn);
			if((fcn==NULL) || (fcn==UNDEFINED))
				return(UNDEFINED);
		}else
		{
			fcn=dyGet(dytfUnwrap(bsvm_toplevel), (char *)fcn);
			if((fcn==NULL) || (fcn==UNDEFINED))
				return(UNDEFINED);
		}
	}

//	dyPrintf("call ");
//	BSVM_TY_PrintLN(dycons(fcn, args));

	ct=(dyt)BSVM_NewState();
	ctx=(BSVM_SVMState *)ct;
	ctx->flowstackpos=-1;
	ctx->top=bsvm_toplevel;

	BSVM_PushList(ctx, dytfWrap(args));
	BSVM_CallObj(ctx, dytfWrap(obj), dytfWrap(fcn));
	
	if(ctx->flowstackpos>=0)
		{ i=BSVM_Run((dyt)ct); }
	
//	i=BSVM_Run(ct);
	if(ctx->stackpos<=0)
		{ t=UNDEFINED; }
	else
		{ t=dytfUnwrap(BSVM_Pop(ctx)); }

	BSVM_DestroyState(ctx);

//	BSVM_GC_CheckRef(ct);

	return(t);
}

BGBSVM_API dyt BSVM_CallFuncTopObj(dyt top, dyt obj, dyt fcn, dyt args)
{
	BSVM_SVMState *ctx;
	char *s;
	dyt t, ct, l, f;
	int i;

	BSVM_Init();

	if(top==NULL)
		top=dytfUnwrap(bsvm_toplevel);

	if(dysymbolp(fcn))
	{
		if(obj)
		{
			fcn=dyGet(obj, (char *)fcn);
			if((fcn==NULL) || (fcn==UNDEFINED))
				return(UNDEFINED);
		}else
		{
			fcn=dyGet(top, (char *)fcn);
			if((fcn==NULL) || (fcn==UNDEFINED))
				return(UNDEFINED);
		}
	}

//	dyPrintf("call ");
//	BSVM_TY_PrintLN(dycons(fcn, args));

	ct=(dyt)BSVM_NewState();
	ctx=(BSVM_SVMState *)ct;
	ctx->flowstackpos=-1;
	ctx->top=dytfWrap(top);

	BSVM_PushList(ctx, dytfWrap(args));
	BSVM_CallObj(ctx, dytfWrap(obj), dytfWrap(fcn));
	
	if(ctx->flowstackpos>=0)
		{ i=BSVM_Run((dyt)ct); }
	
//	i=BSVM_Run(ct);
	if(ctx->stackpos<=0)
		{ t=UNDEFINED; }
	else
		{ t=dytfUnwrap(BSVM_Pop(ctx)); }

	BSVM_DestroyState(ctx);

//	BSVM_GC_CheckRef(ct);

	return(t);
}

BGBSVM_API int BSVM_SaveObject(char *name, dyt obj)
{
	char tb[256];
	VFILE *fd;
	byte *tbuf;
	char *s;
	int i;

	strcpy(tb, name);
	s=tb+strlen(tb);
	while((s>tb) && (*s!='.'))s--;
	strcpy(s, ".bso");
	
	tbuf=malloc(1<<20);
	i=dysEncodeBinaryBuffer(tbuf, 1<<20, obj);
	
	if(i>0)
	{
		fd=vffopen(tb, "wb");
		if(fd)
		{
			vfwrite(tbuf, 1, i, fd);
			vfclose(fd);
		}
	}
	free(tbuf);
	
	return(0);
}

BGBSVM_API dyt BSVM_LoadObject(char *name)
{
	char tb[256];
	VFILE *fd;
	byte *buf;
	char *s;
	dyt t;
	int i, sz;

	strcpy(tb, name);
	s=tb+strlen(tb);
	while((s>tb) && (*s!='.'))s--;
	strcpy(s, ".bso");
	
	buf=BSVM_ScriptBufferIn(tb, &sz);
	if(!buf)return(UNDEFINED);

	t=dysDecodeBinaryBuffer(buf, 1<<20);
	return(t);
}

BGBSVM_API char *BSVM_GetQNameScriptName(char *name)
{
	char tb[256];
	char *buf;
	char *s;
	int i;

	if(!name)return(NULL);

	strcpy(tb, name);
	s=tb;
	while(*s)
	{
		if(*s=='.')*s='/';
		s++;
	}

	strcat(tb, ".bs");

	return(dystrdup(tb));
}

BGBSVM_API dyt BSVM_LoadQNameScriptCtx(
	BSVM_SVMState *ctx, char *name, dyt obj)
{
	char tb[256];
	BSVM_SVMCState *cctx;
	VFILE *fd;
	byte *tbuf;
	dyt t, l, f;
	char *buf;
	char *s;
	int sz;
	int i;

	if(!name)return(BGBDY_NULL);

	strcpy(tb, name);
	s=tb;
	while(*s)
	{
		if(*s=='.')*s='/';
		s++;
	}

	strcat(tb, ".bs");

	for(i=0; i<bsvm_n_scriptname; i++)
		if(!strcmp(bsvm_scriptname[i], tb))
			return(NULL);

	buf=BSVM_ScriptBufferIn(tb, &sz);
	if(!buf)return(NULL);

//	t=BSVM_EvalStringObj(buf, obj);
	t=BSVM_EvalStringObjName(buf, obj, tb);

	i=bsvm_n_scriptname++;
	bsvm_scriptname[i]=dystrdup(tb);

	gcfree(buf);
	return(t);
}

BGBSVM_API dyt BSVM_LoadScript(char *name, dyt obj)
{
	char tb[256];
	BSVM_SVMCState *cctx;
	BSVM_SVMState *ctx;
	VFILE *fd;
	byte *tbuf;
	dyt t, ct, l, f;
	char *buf;
	char *s;
	int sz;
	int i;

	buf=BSVM_ScriptBufferIn(name, &sz);
	if(!buf)return(NULL);

//	t=BSVM_EvalStringObj(buf, obj);

#if 1
	BSVM_Init();

//	if(obj==NULL)
//		obj=bsvm_toplevel;

	BSVMP_SetLinenum(name, buf, 1);

	s=buf;
//	l=BSVMP_Block(&s);
	l=dytfUnwrap(BSVMP_Block(&s));
//	dyPrint(l);

//	t=BSVM_EvalExprsObj(l, obj);

#if 1
	cctx=gcalloc(sizeof(BSVM_SVMCState));

	cctx->lvl=-1;
	f=BSVMC_CompileBlock(cctx, NULL, NULL, l);
	gcfree(cctx);

	BSVM_SaveObject(name, f);
//	BSVM_J2Core_TestJitBlock(f);

#if 0
	strcpy(tb, name);
	s=tb+strlen(tb);
	while((s>tb) && (*s!='.'))s--;
	strcpy(s, ".bso");
	
	tbuf=malloc(1<<20);
	i=dysEncodeBinaryBuffer(tbuf, 1<<20, f);
	
	if(i>0)
	{
		fd=vffopen(tb, "wb");
		if(fd)
		{
			vfwrite(tbuf, 1, i, fd);
			vfclose(fd);
		}
	}
	free(tbuf);
#endif

	ct=(dyt)BSVM_NewState();
	ctx=(BSVM_SVMState *)ct;
	ctx->flowstackpos=-1;
	ctx->top=bsvm_toplevel;

	BSVM_PushList(ctx, DYTF_NULL);
	BSVM_CallObj(ctx, dytfWrap(obj), dytfWrap(f));
	i=BSVM_Run(ct);
	t=dytfUnwrap(BSVM_Pop(ctx));

	BSVM_DestroyState(ctx);

//	return(t);
#endif

#endif

	gcfree(buf);
	return(t);
}

BGBSVM_API dyt BSVM_LoadScriptAsync(char *name, dyt obj)
{
	char tb[256];
	BSVM_SVMCState *cctx;
	BSVM_SVMState *ctx;
	VFILE *fd;
	byte *tbuf;
	dyt t, ct, l, f;
	char *buf;
	char *s;
	int sz;
	int i;

	buf=BSVM_ScriptBufferIn(name, &sz);
	if(!buf)return(NULL);

//	t=BSVM_EvalStringObj(buf, obj);

#if 1
	BSVM_Init();

//	if(obj==NULL)
//		obj=bsvm_toplevel;

	BSVMP_SetLinenum(name, buf, 1);

	s=buf;
//	l=BSVMP_Block(&s);
	l=dytfUnwrap(BSVMP_Block(&s));
//	dyPrint(l);

//	t=BSVM_EvalExprsObj(l, obj);

#if 1
	cctx=gcalloc(sizeof(BSVM_SVMCState));
	cctx->lvl=-1;
	f=BSVMC_CompileBlock(cctx, NULL, NULL, l);
	gcfree(cctx);

//	BSVM_SaveObject(name, f);
//	BSVM_J2Core_TestJitBlock(f);

	ct=(dyt)BSVM_NewState();
	ctx=(BSVM_SVMState *)ct;
	ctx->flowstackpos=-1;
	ctx->top=bsvm_toplevel;

	BSVM_PushList(ctx, DYTF_NULL);
	BSVM_CallObj(ctx, dytfWrap(obj), dytfWrap(f));
//	i=BSVM_Run(ct);
	BSVM_RunThread(ctx);
	t=(dyt)ctx;
//	t=BSVM_Pop(ctx);
//	BSVM_DestroyState(ctx);

//	return(t);
#endif

#endif

	gcfree(buf);
	return(t);
}

BGBSVM_API void *BSVM_LoadModuleASM(char *name)
{
	char tb[256];
	char *buf;
	char *s;
	void *p;
	int sz;
	int i;

	buf=BSVM_ScriptBufferIn(name, &sz);
	if(!buf)
	{
		dyPrintf("BSVM_LoadModuleASM: Fail Load %s\n", name);
		return(NULL);
	}

//	dyllAsmModuleBuffer(name, buf);

	dyllAsmBegin();
	BSVM_Thread_LoadEmitPrefixASM();
	dyllAsmPuts(buf);
	p=dyllAsmEnd();
	return(p);
}

BGBSVM_API dyt BSVM_LoadScriptTop(char *name, dyt obj, dyt top)
{
	char tb[256];
	BSVM_SVMCState *cctx;
	BSVM_SVMState *ctx;
	VFILE *fd;
	byte *tbuf;
	dyt t, ct, l, f;
	char *buf;
	char *s;
	int sz;
	int i;

	buf=BSVM_ScriptBufferIn(name, &sz);
//	if(!buf)return(NULL);
	if(!buf)return(UNDEFINED);

//	t=BSVM_EvalStringObj(buf, obj);

#if 1
	BSVM_Init();

//	if(obj==NULL)
//		obj=bsvm_toplevel;

	BSVMP_SetLinenum(name, buf, 1);

	s=buf;
//	l=BSVMP_Block(&s);
	l=dytfUnwrap(BSVMP_Block(&s));
//	dyPrint(l);

//	t=BSVM_EvalExprsObj(l, obj);

#if 1
	cctx=gcalloc(sizeof(BSVM_SVMCState));

	cctx->lvl=-1;
	f=BSVMC_CompileBlock(cctx, NULL, NULL, l);
	gcfree(cctx);

	BSVM_SaveObject(name, f);
//	BSVM_J2Core_TestJitBlock(f);

#if 0
	strcpy(tb, name);
	s=tb+strlen(tb);
	while((s>tb) && (*s!='.'))s--;
	strcpy(s, ".bso");
	
	tbuf=malloc(1<<20);
	i=dysEncodeBinaryBuffer(tbuf, 1<<20, f);
	
	if(i>0)
	{
		fd=vffopen(tb, "wb");
		if(fd)
		{
			vfwrite(tbuf, 1, i, fd);
			vfclose(fd);
		}
	}
	free(tbuf);
#endif

	ct=(dyt)BSVM_NewState();
	ctx=(BSVM_SVMState *)ct;
	ctx->flowstackpos=-1;
	ctx->top=dytfWrap(top);

	BSVM_PushList(ctx, DYTF_NULL);
	BSVM_CallObj(ctx, dytfWrap(obj), dytfWrap(f));
	i=BSVM_Run(ct);
	t=dytfUnwrap(BSVM_Pop(ctx));

	BSVM_DestroyState(ctx);

//	return(t);
#endif

#endif

	gcfree(buf);
	return(t);
}

BGBSVM_API dyt BSVM_CreateLocalToplevel(void)
{
	return(dyObjectParent(dytfUnwrap(bsvm_toplevel)));
}

BGBSVM_API int bsvmAddScriptPath(char *name)
{
	return(BSVM_AddScriptPath(name));
}

BGBSVM_API dyt bsvmRootImportPackage(char *name)
{
	return(BSVM_LoadQNameScriptCtx(NULL, name, NULL));
}

BGBSVM_API dyt bsvmRootImportPackageList(char *name)
{
	char **a;
	int i;

	a=dyStableSplit(name);
	for(i=0; a[i]; i++)
		bsvmRootImportPackage(a[i]);

	return(NULL);
}

BGBSVM_API dyt bsvmLoadScript(char *name, dyt obj)
{
	return(BSVM_LoadScript(name, obj));
}

BGBSVM_API dyt bsvmLoadScriptAsync(char *name, dyt obj)
{
	return(BSVM_LoadScriptAsync(name, obj));
}

BGBSVM_API void *bsvmLoadModuleASM(char *name)
{
	return(BSVM_LoadModuleASM(name));
}
